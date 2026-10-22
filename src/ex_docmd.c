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

स्थिर पूर्णाङ्क	quitmore = 0;
स्थिर पूर्णाङ्क	ex_pressedreturn = FALSE;
#ifndef FEAT_PRINTER
# define ex_hardcopy	ex_ni
#endif

#ifdef FEAT_EVAL
स्थिर char_u	*do_one_cmd(char_u **, पूर्णाङ्क, cstack_T *, char_u *(*fgetline)(पूर्णाङ्क, शून्य *, पूर्णाङ्क, getline_opt_T), शून्य *cookie);
#अण्णहा
स्थिर char_u	*do_one_cmd(char_u **, पूर्णाङ्क, char_u *(*fgetline)(पूर्णाङ्क, शून्य *, पूर्णाङ्क, getline_opt_T), शून्य *cookie);
स्थिर पूर्णाङ्क	if_level = 0;		// depth in :if
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
स्थिर पूर्णाङ्क	getargopt(exarg_T *eap);
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
#जइ !defined(FEAT_QUICKFIX) || !defined(FEAT_EVAL)
# define ex_cexpr		ex_ni
#endif

स्थिर linenr_T default_address(exarg_T *eap);
स्थिर शून्य address_default_all(exarg_T *eap);
स्थिर शून्य	get_flags(exarg_T *eap);
#जइ !defined(FEAT_PERL) \
	|| !defined(FEAT_PYTHON) || !defined(FEAT_PYTHON3) \
	|| !defined(FEAT_TCL) \
	|| !defined(FEAT_RUBY) \
	|| !defined(FEAT_LUA) \
	|| !defined(FEAT_MZSCHEME)
# define HAVE_EX_SCRIPT_NI
स्थिर शून्य	ex_script_ni(exarg_T *eap);
#endif
स्थिर वर्ण	*invalid_range(exarg_T *eap);
स्थिर शून्य	correct_range(exarg_T *eap);
#ifdef FEAT_QUICKFIX
स्थिर char_u	*replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep);
#endif
स्थिर char_u	*repl_cmdline(exarg_T *eap, char_u *src, size_t srclen, char_u *repl, char_u **cmdlinep);
स्थिर शून्य	ex_highlight(exarg_T *eap);
स्थिर शून्य	ex_colorscheme(exarg_T *eap);
स्थिर शून्य	ex_cquit(exarg_T *eap);
स्थिर शून्य	ex_quit_all(exarg_T *eap);
स्थिर शून्य	ex_close(exarg_T *eap);
स्थिर शून्य	ex_win_close(पूर्णाङ्क forceit, win_T *win, tabpage_T *tp);
स्थिर शून्य	ex_only(exarg_T *eap);
स्थिर शून्य	ex_resize(exarg_T *eap);
स्थिर शून्य	ex_stag(exarg_T *eap);
स्थिर शून्य	ex_tabclose(exarg_T *eap);
स्थिर शून्य	ex_tabonly(exarg_T *eap);
स्थिर शून्य	ex_tabnext(exarg_T *eap);
स्थिर शून्य	ex_tabmove(exarg_T *eap);
स्थिर शून्य	ex_tabs(exarg_T *eap);
#जइ defined(FEAT_QUICKFIX)
स्थिर शून्य	ex_pclose(exarg_T *eap);
स्थिर शून्य	ex_ptag(exarg_T *eap);
स्थिर शून्य	ex_pedit(exarg_T *eap);
स्थिर शून्य	ex_pbuffer(exarg_T *eap);
स्थिर शून्य	prepare_preview_window(शून्य);
स्थिर शून्य	back_to_current_window(win_T *curwin_save);
#अण्णहा
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
#अण्णहा
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
#जइ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
स्थिर शून्य	ex_tearoff(exarg_T *eap);
#अण्णहा
# define ex_tearoff		ex_ni
#endif
#जइ (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
स्थिर शून्य	ex_popup(exarg_T *eap);
#अण्णहा
# define ex_popup		ex_ni
#endif
#ifndef FEAT_GUI_MSWIN
# define ex_simalt		ex_ni
#endif
#जइ !defined(FEAT_GUI_MSWIN) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
# define gui_mch_find_dialog	ex_ni
# define gui_mch_replace_dialog ex_ni
#endif
#जइ !defined(FEAT_GUI_GTK)
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
#जइ !defined(FEAT_SYN_HL) || !defined(FEAT_PROFILE)
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
#जइ !defined(FEAT_PYTHON) && !defined(FEAT_PYTHON3)
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
#जइ defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
स्थिर शून्य	ex_winpos(exarg_T *eap);
#अण्णहा
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
#अण्णहा
# define ex_findpat		ex_ni
# define ex_checkpath		ex_ni
#endif
#जइ defined(FEAT_FIND_ID) && defined(FEAT_QUICKFIX)
स्थिर शून्य	ex_psearch(exarg_T *eap);
#अण्णहा
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
#ifdef FEAT_अन्वेषणम्_EXTRA
स्थिर शून्य	ex_nohlsearch(exarg_T *eap);
#अण्णहा
# define ex_nohlsearch		ex_ni
# define ex_match		ex_ni
#endif
#ifdef FEAT_CRYPT
स्थिर शून्य	ex_X(exarg_T *eap);
#अण्णहा
# define ex_X			ex_ni
#endif
#ifdef FEAT_FOLDING
स्थिर शून्य	ex_fold(exarg_T *eap);
स्थिर शून्य	ex_foldopen(exarg_T *eap);
स्थिर शून्य	ex_folddo(exarg_T *eap);
#अण्णहा
# define ex_fold		ex_ni
# define ex_foldopen		ex_ni
# define ex_folddo		ex_ni
#endif
#जइ !(defined(HAVE_LOCALE_H) || defined(X_LOCALE))
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
#जइ !defined(FEAT_X11) || !defined(FEAT_XCLIPBOARD)
# define ex_xrestore		ex_ni
#endif
#जइ !defined(FEAT_WAYLAND)
# define ex_wlrestore		ex_ni
#endif
#जइ !defined(HAVE_CLIPMETHOD)
# define ex_clipreset		ex_ni
#endif
#जइ !defined(FEAT_PROP_POPUP)
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
प्रकारनिर्देश संरचना
{
    char_u	*पङ्क्तिः;		// command line
    linenr_T	lnum;		// sourcing_lnum of the line
} wcmd_T;

/*
 * Structure used to store info for line position in a while or for loop.
 * This is required, because do_one_cmd() may invoke ex_function(), which
 * reads more lines that may come from the while/for loop.
 */
संरचना loop_cookie
{
    garray_T	*पङ्क्तयः_gap;		// growarray with line info
    पूर्णाङ्क		current_पङ्क्तिः;		// last read line from growarray
    पूर्णाङ्क		repeating;		// TRUE when looping a second time
    // When "repeating" is FALSE use "getline" and "cookie" to get lines
    char_u	*(*lc_getline)(पूर्णाङ्क, शून्य *, पूर्णाङ्क, getline_opt_T);
    शून्य	*cookie;
};

स्थिर char_u	*get_loop_पङ्क्तिः(पूर्णाङ्क c, शून्य *cookie, पूर्णाङ्क indent, getline_opt_T options);
स्थिर पूर्णाङ्क	store_loop_पङ्क्तिः(garray_T *gap, char_u *पङ्क्तिः);
स्थिर शून्य	free_cmdlines(garray_T *gap);

// Struct to save a few things while debugging.  Used in do_cmdline() only.
संरचना dbg_stuff
{
    पूर्णाङ्क		trylevel;
    पूर्णाङ्क		force_abort;
    except_T	*caught_राशिः;
    char_u	*vv_exception;
    char_u	*vv_throwpoint;
    पूर्णाङ्क		did_emsg;
    पूर्णाङ्क		got_int;
    पूर्णाङ्क		did_throw;
    पूर्णाङ्क		need_rethrow;
    पूर्णाङ्क		check_cstack;
    except_T	*current_exception;
};

    स्थिर शून्य
save_dbg_stuff(संरचना dbg_stuff *dsp)
{
    dsp->trylevel	= trylevel;		trylevel = 0;
    dsp->force_abort	= force_abort;		force_abort = FALSE;
    dsp->caught_राशिः	= caught_राशिः;		caught_राशिः = रिक्त;
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
    caught_राशिः = dsp->caught_राशिः;
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
    स्थिर पूर्णाङ्क
is_other_file(पूर्णाङ्क fnum, char_u *ffname)
{
    जइ (fnum != 0)
    {
	जइ (fnum == curbuf->b_fnum)
	    प्रत्यागम FALSE;

	प्रत्यागम TRUE;
    }

    जइ (ffname == रिक्त)
	प्रत्यागम TRUE;

    जइ (*ffname == NUL)
	प्रत्यागम FALSE;

    // TODO: Need a reliable way to know whether a buffer is meant to live
    // on-disk !curbuf->b_dev_valid is not always available (example: missing
    // on Windows)
    जइ (curbuf->b_sfname != रिक्त
	    && *curbuf->b_sfname != NUL)
	// This occurs with unsaved buffers. In which case `ffname` actually
	// corresponds to curbuf->b_sfname
	प्रत्यागम fnamecmp(ffname, curbuf->b_sfname) != 0;

    प्रत्यागम otherfile(ffname);
}

/*
 * do_exmode(): Repeatedly get commands for the "Ex" mode, until the ":vi"
 * command is given.
 */
    शून्य
do_exmode(
    पूर्णाङ्क		improved)	    // TRUE for "improved Ex" mode
{
    पूर्णाङ्क		save_सन्देशः_scroll;
    पूर्णाङ्क		prev_सन्देशः_row;
    linenr_T	prev_पङ्क्तिः;
    varnumber_T	changedtick;

    जइ (improved)
	exmode_active = EXMODE_VIM;
    अण्णहा
	exmode_active = EXMODE_NORMAL;
    State = MODE_NORMAL;
    may_trigger_modechanged();

    // When using ":global /pat/ visual" and then "Q" we return to continue
    // the :global command.
    जइ (global_busy)
	प्रत्यागम;

    save_सन्देशः_scroll = सन्देशः_scroll;
    ++RedrawingDisabled;	    // don't redisplay the window
    ++no_wait_return;		    // don't wait for return
#ifdef FEAT_GUI
    // Ignore scrollbar and mouse events in Ex mode
    ++hold_gui_events;
#endif

    सन्देशः(_("Entering Ex mode.  Type \"visual\" to go to Normal mode."));
    जाव (exmode_active)
    {
	// Check for a ":normal" command and no more characters left.
	जइ (ex_normal_busy > 0 && typebuf.tb_len == 0)
	{
	    exmode_active = FALSE;
	    भङ्ग;
	}
	सन्देशः_scroll = TRUE;
	need_wait_return = FALSE;
	ex_pressedreturn = FALSE;
	ex_no_reprint = FALSE;
	changedtick = CHANGEDTICK(curbuf);
	prev_सन्देशः_row = सन्देशः_row;
	prev_पङ्क्तिः = curwin->w_cursor.lnum;
	जइ (improved)
	{
	    cmdline_row = सन्देशः_row;
	    do_cmdline(रिक्त, getexline, रिक्त, 0);
	}
	अण्णहा
	    do_cmdline(रिक्त, getexmodeline, रिक्त, DOCMD_NOWAIT);
	पङ्क्तयः_वामः = Rows - 1;

	जइ ((prev_पङ्क्तिः != curwin->w_cursor.lnum
		   || changedtick != CHANGEDTICK(curbuf)) && !ex_no_reprint)
	{
	    जइ (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    अण्णहा
	    {
		जइ (ex_pressedreturn)
		{
		    // go up one line, to overwrite the ":<CR>" line, so the
		    // output doesn't contain empty lines.
		    सन्देशः_row = prev_सन्देशः_row;
		    जइ (prev_सन्देशः_row == Rows - 1)
			सन्देशः_row--;
		}
		सन्देशः_col = 0;
		print_पङ्क्तिः_no_prefix(curwin->w_cursor.lnum, FALSE, FALSE);
		सन्देशः_clr_eos();
	    }
	}
	अण्णहा जइ (ex_pressedreturn && !ex_no_reprint)	// must be at EOF
	{
	    जइ (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    अण्णहा
		emsg(_(e_at_end_of_file));
	}
    }

#ifdef FEAT_GUI
    --hold_gui_events;
#endif
    जइ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    update_screen(UPD_CLEAR);
    need_wait_return = FALSE;
    सन्देशः_scroll = save_सन्देशः_scroll;
}

/*
 * Print the executed command for when 'verbose' is set.
 * When "lnum" is 0 only print the command.
 */
    स्थिर शून्य
सन्देशः_verbose_cmd(linenr_T lnum, char_u *cmd)
{
    ++no_wait_return;
    verbose_enter_scroll();

    जइ (lnum == 0)
	smsg(_("Executing: %s"), cmd);
    अण्णहा
	smsg(_("line %ld: %s"), (long)lnum, cmd);
    जइ (सन्देशः_silent == 0)
	सन्देशः_puts("\n");   // don't overwrite this

    verbose_leave_scroll();
    --no_wait_return;
}

/*
 * Execute a simple command line.  Used for translated commands like "*".
 */
    पूर्णाङ्क
do_cmdline_cmd(char_u *cmd)
{
    प्रत्यागम do_cmdline(cmd, रिक्त, रिक्त,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
}

/*
 * Execute the "+cmd" argument of "edit +cmd fname" and the like.
 * This allows for using a range without ":" in Vim9 script.
 */
    स्थिर पूर्णाङ्क
do_cmd_argument(char_u *cmd)
{
    प्रत्यागम do_cmdline(cmd, रिक्त, रिक्त,
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
    पूर्णाङ्क
do_cmdline(
    char_u	*cmdline,
    char_u	*(*fgetline)(पूर्णाङ्क, शून्य *, पूर्णाङ्क, getline_opt_T),
    शून्य	*cookie,		// argument for fgetline()
    पूर्णाङ्क		flags)
{
    char_u	*next_cmdline;		// next cmd to execute
    char_u	*cmdline_copy = रिक्त;	// copy of cmd line
    पूर्णाङ्क		used_getline = FALSE;	// used "fgetline" to obtain command
    स्थिर पूर्णाङ्क	recursive = 0;		// recursive depth
    पूर्णाङ्क		सन्देशः_didout_before_start = 0;
    पूर्णाङ्क		गणनम् = 0;		// line number count
    पूर्णाङ्क		did_inc_RedrawingDisabled = FALSE;
    पूर्णाङ्क		retval = OK;
#ifdef FEAT_EVAL
    cstack_T	cstack;			// conditional stack
    garray_T	पङ्क्तयः_ga;		// keep lines for ":while"/":for"
    पूर्णाङ्क		current_पङ्क्तिः = 0;	// active line in lines_ga
    पूर्णाङ्क		current_पङ्क्तिः_before = 0;
    char_u	*fname = रिक्त;		// function or script name
    linenr_T	*breakpoint = रिक्त;	// ptr to breakpoint field in cookie
    पूर्णाङ्क		*dbg_tick = रिक्त;	// ptr to dbg_tick field in cookie
    संरचना dbg_stuff debug_saved;	// saved things for debug mode
    पूर्णाङ्क		initial_trylevel;
    msglist_T	**saved_सन्देशः_list = रिक्त;
    msglist_T	*private_सन्देशः_list = रिक्त;

    // "fgetline" and "cookie" passed to do_one_cmd()
    char_u	*(*cmd_getline)(पूर्णाङ्क, शून्य *, पूर्णाङ्क, getline_opt_T);
    शून्य	*cmd_cookie;
    संरचना loop_cookie cmd_loop_cookie;
    शून्य	*real_cookie;
    पूर्णाङ्क		getline_is_func;
#अण्णहा
# define cmd_getline fgetline
# define cmd_cookie cookie
#endif
    स्थिर पूर्णाङ्क	call_depth = 0;		// recursiveness
#ifdef FEAT_EVAL
    // For every pair of do_cmdline()/do_one_cmd() calls, use an extra memory
    // location for storing error messages to be converted to an exception.
    // This ensures that the do_errthrow() call in do_one_cmd() does not
    // combine the messages stored by an earlier invocation of do_one_cmd()
    // with the command name of the later one.  This would happen when
    // BufWritePost autocommands are executed after a write error.
    saved_सन्देशः_list = सन्देशः_list;
    सन्देशः_list = &private_सन्देशः_list;
#endif

    // It's possible to create an endless loop with ":execute", catch that
    // here.  The value of 200 allows nested function calls, ":source", etc.
    // Allow 200 or 'maxfuncdepth', whatever is larger.
    जइ (call_depth >= 200
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
	सन्देशः_list = saved_सन्देशः_list;
#endif
	प्रत्यागम FAIL;
    }
    ++call_depth;

#ifdef FEAT_EVAL
    CLEAR_FIELD(cstack);
    cstack.cs_idx = -1;
    ga_init2(&पङ्क्तयः_ga, परिमाण(wcmd_T), 10);

    real_cookie = getline_cookie(fgetline, cookie);

    // Inside a function use a higher nesting level.
    getline_is_func = getline_equal(fgetline, cookie, get_func_पङ्क्तिः);
    जइ (getline_is_func && ex_nesting_level == func_level(real_cookie))
	++ex_nesting_level;

    // Get the function or script name and the address where the next breakpoint
    // line and the debug tick for a function or script are stored.
    जइ (getline_is_func)
    {
	fname = func_name(real_cookie);
	breakpoint = func_breakpoint(real_cookie);
	dbg_tick = func_dbg_tick(real_cookie);
    }
    अण्णहा जइ (getline_equal(fgetline, cookie, getsourceline))
    {
	fname = SOURCING_NAME;
	breakpoint = source_breakpoint(real_cookie);
	dbg_tick = source_dbg_tick(real_cookie);
    }

    /*
     * Initialize "force_abort"  and "suppress_errthrow" at the top level.
     */
    जइ (!recursive)
    {
	force_abort = FALSE;
	suppress_errthrow = FALSE;
    }

    /*
     * If requested, store and reset the global values controlling the
     * exception handling (used when debugging).  Otherwise clear it to avoid
     * a bogus compiler warning when the optimizer uses inline functions...
     */
    जइ (flags & DOCMD_EXCRESET)
	save_dbg_stuff(&debug_saved);
    अण्णहा
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
    जइ (!(flags & DOCMD_KEYTYPED)
			       && !getline_equal(fgetline, cookie, getexline))
	कुञ्चिका_Typed = FALSE;

    /*
     * Continue executing command lines:
     * - when inside an ":if", ":while" or ":for"
     * - for multiple commands on one line, separated with '|'
     * - when repeating until there are no more lines (for ":source")
     */
    next_cmdline = cmdline;
    करेहि
    {
#ifdef FEAT_EVAL
	getline_is_func = getline_equal(fgetline, cookie, get_func_पङ्क्तिः);
#endif

	// stop skipping cmds for an error msg after all endif/while/for
	जइ (next_cmdline == रिक्त
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
	जइ (cstack.cs_looplevel > 0 && current_पङ्क्तिः < पङ्क्तयः_ga.ga_len)
	{
	    // Each '|' separated command is stored separately in lines_ga, to
	    // be able to jump to it.  Don't use next_cmdline now.
	    VIM_CLEAR(cmdline_copy);

	    // Check if a function has returned or, unless it has an unclosed
	    // try conditional, aborted.
	    जइ (getline_is_func)
	    {
# ifdef FEAT_PROFILE
		जइ (do_profiling == PROF_YES)
		    func_पङ्क्तिः_end(real_cookie);
# endif
		जइ (func_has_ended(real_cookie))
		{
		    retval = FAIL;
		    भङ्ग;
		}
	    }
# ifdef FEAT_PROFILE
	    अण्णहा जइ (do_profiling == PROF_YES
			    && getline_equal(fgetline, cookie, getsourceline))
		script_पङ्क्तिः_end();
# endif

	    // Check if a sourced file hit a ":finish" command.
	    जइ (source_finished(fgetline, cookie))
	    {
		retval = FAIL;
		भङ्ग;
	    }

	    // If breakpoints have been added/deleted need to check for it.
	    जइ (breakpoint != रिक्त && dbg_tick != रिक्त
						   && *dbg_tick != debug_tick)
	    {
		*breakpoint = dbg_find_breakpoint(
				getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }

	    next_cmdline = ((wcmd_T *)(पङ्क्तयः_ga.ga_data))[current_पङ्क्तिः].पङ्क्तिः;
	    SOURCING_LNUM = ((wcmd_T *)(पङ्क्तयः_ga.ga_data))[current_पङ्क्तिः].lnum;

	    // Did we encounter a breakpoint?
	    जइ (breakpoint != रिक्त && *breakpoint != 0
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
	    जइ (do_profiling == PROF_YES)
	    {
		जइ (getline_is_func)
		    func_पङ्क्तिः_start(real_cookie, SOURCING_LNUM);
		अण्णहा जइ (getline_equal(fgetline, cookie, getsourceline))
		    script_पङ्क्तिः_start();
	    }
# endif
	}
#endif

	// 2. If no line given, get an allocated line with fgetline().
	जइ (next_cmdline == रिक्त)
	{
	    /*
	     * Need to set msg_didout for the first line after an ":if",
	     * otherwise the ":if" will be overwritten.
	     */
	    जइ (गणनम् == 1 && getline_equal(fgetline, cookie, getexline))
		सन्देशः_didout = TRUE;
	    जइ (fgetline == रिक्त || (next_cmdline = fgetline(':', cookie,
#ifdef FEAT_EVAL
		    cstack.cs_idx < 0 ? 0 : (cstack.cs_idx + 1) * 2
#अण्णहा
		    0
#endif
		    , in_vim9script() ? GETLINE_CONCAT_CONTBAR
					       : GETLINE_CONCAT_CONT)) == रिक्त)
	    {
		// Don't call wait_return() for aborted command line.  The NULL
		// returned for the end of a sourced file or executed function
		// doesn't do this.
		जइ (कुञ्चिका_Typed && !(flags & DOCMD_REPEAT))
		    need_wait_return = FALSE;
		retval = FAIL;
		भङ्ग;
	    }
	    used_getline = TRUE;

	    /*
	     * Keep the first typed line.  Clear it when more lines are typed.
	     */
	    जइ (flags & DOCMD_KEEPLINE)
	    {
		vim_free(repeat_cmdline);
		जइ (गणनम् == 0)
		    repeat_cmdline = vim_strsave(next_cmdline);
		अण्णहा
		    repeat_cmdline = रिक्त;
	    }
	}

	// 3. Make a copy of the command so we can mess with it.
	अण्णहा जइ (cmdline_copy == रिक्त)
	{
	    next_cmdline = vim_strsave(next_cmdline);
	    जइ (next_cmdline == रिक्त)
	    {
		emsg(_(e_out_of_memory));
		retval = FAIL;
		भङ्ग;
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
	जइ ((cstack.cs_looplevel > 0 || has_loop_cmd(next_cmdline)))
	{
	    cmd_getline = get_loop_पङ्क्तिः;
	    cmd_cookie = (शून्य *)&cmd_loop_cookie;
	    cmd_loop_cookie.पङ्क्तयः_gap = &पङ्क्तयः_ga;
	    cmd_loop_cookie.current_पङ्क्तिः = current_पङ्क्तिः;
	    cmd_loop_cookie.lc_getline = fgetline;
	    cmd_loop_cookie.cookie = cookie;
	    cmd_loop_cookie.repeating = (current_पङ्क्तिः < पङ्क्तयः_ga.ga_len);

	    // Save the current line when encountering it the first time.
	    जइ (current_पङ्क्तिः == पङ्क्तयः_ga.ga_len
		    && store_loop_पङ्क्तिः(&पङ्क्तयः_ga, next_cmdline) == FAIL)
	    {
		retval = FAIL;
		भङ्ग;
	    }
	    current_पङ्क्तिः_before = current_पङ्क्तिः;
	}
	अण्णहा
	{
	    cmd_getline = fgetline;
	    cmd_cookie = cookie;
	}

	did_endif = FALSE;
#endif

	जइ (गणनम्++ == 0)
	{
	    /*
	     * All output from the commands is put below each other, without
	     * waiting for a return. Don't do this when executing commands
	     * from a script or when being called recursive (e.g. for ":e
	     * +command file").
	     */
	    जइ (!(flags & DOCMD_NOWAIT) && !recursive)
	    {
		सन्देशः_didout_before_start = सन्देशः_didout;
		सन्देशः_didany = FALSE; // no output yet
		सन्देशः_start();
		सन्देशः_scroll = TRUE;  // put messages below each other
		++no_wait_return;   // don't wait for return until finished
		++RedrawingDisabled;
		did_inc_RedrawingDisabled = TRUE;
	    }
	}

	जइ ((p_verbose >= 15 && SOURCING_NAME != रिक्त) || p_verbose >= 16)
	    सन्देशः_verbose_cmd(SOURCING_LNUM, cmdline_copy);

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
	जइ (cmd_cookie == (शून्य *)&cmd_loop_cookie)
	    // Use "current_line" from "cmd_loop_cookie", it may have been
	    // incremented when defining a function.
	    current_पङ्क्तिः = cmd_loop_cookie.current_पङ्क्तिः;
#endif

	जइ (next_cmdline == रिक्त)
	{
	    VIM_CLEAR(cmdline_copy);

	    /*
	     * If the command was typed, remember it for the ':' register.
	     * Do this AFTER executing the command to make :@: work.
	     */
	    जइ (getline_equal(fgetline, cookie, getexline)
						  && new_last_cmdline != रिक्त)
	    {
		vim_free(last_cmdline);
		last_cmdline = new_last_cmdline;
		new_last_cmdline = रिक्त;
	    }
	}
	अण्णहा
	{
	    // need to copy the command after the '|' to cmdline_copy, for the
	    // next do_one_cmd()
	    STRMOVE(cmdline_copy, next_cmdline);
	    next_cmdline = cmdline_copy;
	}


#ifdef FEAT_EVAL
	// reset did_emsg for a function that is not aborted by an error
	जइ (did_emsg && !force_abort
		&& getline_equal(fgetline, cookie, get_func_पङ्क्तिः)
					      && !func_has_abort(real_cookie))
	{
	    // did_emsg_cumul is not set here
	    did_emsg = FALSE;
	}

	जइ (cstack.cs_looplevel > 0)
	{
	    ++current_पङ्क्तिः;

	    /*
	     * An ":endwhile", ":endfor" and ":continue" is handled here.
	     * If we were executing commands, jump back to the ":while" or
	     * ":for".
	     * If we were not executing commands, decrement cs_looplevel.
	     */
	    जइ (cstack.cs_lflags & (CSL_HAD_CONT | CSL_HAD_ENDLOOP))
	    {
		cstack.cs_lflags &= ~(CSL_HAD_CONT | CSL_HAD_ENDLOOP);

		// Jump back to the matching ":while" or ":for".  Be careful
		// not to use a cs_line[] from an entry that isn't a ":while"
		// or ":for": It would make "current_line" invalid and can
		// cause a crash.
		जइ (!did_emsg && !got_int && !did_throw
			&& cstack.cs_idx >= 0
			&& (cstack.cs_flags[cstack.cs_idx]
						      & (CSF_WHILE | CSF_FOR))
			&& cstack.cs_पङ्क्तिः[cstack.cs_idx] >= 0
			&& (cstack.cs_flags[cstack.cs_idx] & CSF_ACTIVE))
		{
		    current_पङ्क्तिः = cstack.cs_पङ्क्तिः[cstack.cs_idx];
						// remember we jumped there
		    cstack.cs_lflags |= CSL_HAD_LOOP;
		    पङ्क्तिः_breakcheck();		// check if CTRL-C typed

		    // Check for the next breakpoint at or after the ":while"
		    // or ":for".
		    जइ (breakpoint != रिक्त && पङ्क्तयः_ga.ga_len > current_पङ्क्तिः)
		    {
			*breakpoint = dbg_find_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
									fname,
			   ((wcmd_T *)पङ्क्तयः_ga.ga_data)[current_पङ्क्तिः].lnum-1);
			*dbg_tick = debug_tick;
		    }
		}
		अण्णहा
		{
		    // can only get here with ":endwhile" or ":endfor"
		    जइ (cstack.cs_idx >= 0)
			rewind_conditionals(&cstack, cstack.cs_idx - 1,
				   CSF_WHILE | CSF_FOR, &cstack.cs_looplevel);
		}
	    }

	    /*
	     * For a ":while" or ":for" we need to remember the line number.
	     */
	    अण्णहा जइ (cstack.cs_lflags & CSL_HAD_LOOP)
	    {
		cstack.cs_lflags &= ~CSL_HAD_LOOP;
		cstack.cs_पङ्क्तिः[cstack.cs_idx] = current_पङ्क्तिः_before;
	    }
	}

	// Check for the next breakpoint after a watchexpression
	जइ (breakpoint != रिक्त && has_watchexpr())
	{
	    *breakpoint = dbg_find_breakpoint(FALSE, fname, SOURCING_LNUM);
	    *dbg_tick = debug_tick;
	}

	/*
	 * When not inside any ":while" loop, clear remembered lines.
	 */
	जइ (cstack.cs_looplevel == 0)
	{
	    जइ (पङ्क्तयः_ga.ga_len > 0)
	    {
		SOURCING_LNUM =
		       ((wcmd_T *)पङ्क्तयः_ga.ga_data)[पङ्क्तयः_ga.ga_len - 1].lnum;
		free_cmdlines(&पङ्क्तयः_ga);
	    }
	    current_पङ्क्तिः = 0;
	}

	/*
	 * A ":finally" makes did_emsg, got_int, and did_throw pending for
	 * being restored at the ":endtry".  Reset them here and set the
	 * ACTIVE and FINALLY flags, so that the finally clause gets executed.
	 * This includes the case where a missing ":endif", ":endwhile" or
	 * ":endfor" was detected by the ":finally" itself.
	 */
	जइ (cstack.cs_lflags & CSL_HAD_FINA)
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
	जइ (trylevel == 0 && !did_emsg && !got_int && !did_throw)
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
    जाव (!((got_int
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
    free_cmdlines(&पङ्क्तयः_ga);
    ga_clear(&पङ्क्तयः_ga);

    जइ (cstack.cs_idx >= 0)
    {
	/*
	 * If a sourced file or executed function ran to its end, report the
	 * unclosed conditional.
	 * In Vim9 script do not give a second error, executing aborts after
	 * the first one.
	 */
	जइ (!got_int && !did_throw && !aborting()
		&& !(did_emsg && in_vim9script())
		&& ((getline_equal(fgetline, cookie, getsourceline)
			&& !source_finished(fgetline, cookie))
		    || (getline_equal(fgetline, cookie, get_func_पङ्क्तिः)
					    && !func_has_ended(real_cookie))))
	{
	    जइ (cstack.cs_flags[cstack.cs_idx] & CSF_TRY)
		emsg(_(e_missing_endtry));
	    अण्णहा जइ (cstack.cs_flags[cstack.cs_idx] & CSF_WHILE)
		emsg(_(e_missing_endwhile));
	    अण्णहा जइ (cstack.cs_flags[cstack.cs_idx] & CSF_FOR)
		emsg(_(e_missing_endfor));
	    अण्णहा
		emsg(_(e_missing_endif));
	}

	/*
	 * Reset "trylevel" in case of a ":finish" or ":return" or a missing
	 * ":endtry" in a sourced file or executed function.  If the try
	 * conditional is in its finally clause, ignore anything pending.
	 * If it is in a catch clause, finish the caught exception.
	 * Also cleanup any "cs_forinfo" structures.
	 */
	करेहि
	{
	    पूर्णाङ्क idx = cleanup_conditionals(&cstack, 0, TRUE);

	    जइ (idx >= 0)
		--idx;	    // remove try block not in its finally clause
	    rewind_conditionals(&cstack, idx, CSF_WHILE | CSF_FOR,
							&cstack.cs_looplevel);
	}
	जाव (cstack.cs_idx >= 0);
	trylevel = initial_trylevel;
    }

    // If a missing ":endtry", ":endwhile", ":endfor", or ":endif" or a memory
    // lack was reported above and the error message is to be converted to an
    // exception, do this now after rewinding the cstack.
    do_errthrow(&cstack, getline_equal(fgetline, cookie, get_func_पङ्क्तिः)
				  ? (char_u *)"endfunction" : (char_u *)रिक्त);

    जइ (trylevel == 0)
    {
	// Just in case did_throw got set but current_exception wasn't.
	जइ (current_exception == रिक्त)
	    did_throw = FALSE;

	/*
	 * When an exception is being thrown out of the outermost try
	 * conditional, discard the uncaught exception, disable the conversion
	 * of interrupts or errors to exceptions, and ensure that no more
	 * commands are executed.
	 */
	जइ (did_throw)
	    handle_did_throw();

	/*
	 * On an interrupt or an aborting error not converted to an exception,
	 * disable the conversion of errors to exceptions.  (Interrupts are not
	 * converted anymore, here.) This enables also the interrupt message
	 * when force_abort is set and did_emsg unset in case of an interrupt
	 * from a finally clause after an error.
	 */
	अण्णहा जइ (got_int || (did_emsg && force_abort))
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
    जइ (did_throw)
	need_rethrow = TRUE;
    जइ ((getline_equal(fgetline, cookie, getsourceline)
		&& ex_nesting_level > source_level(real_cookie))
	    || (getline_equal(fgetline, cookie, get_func_पङ्क्तिः)
		&& ex_nesting_level > func_level(real_cookie) + 1))
    {
	जइ (!did_throw)
	    check_cstack = TRUE;
    }
    अण्णहा
    {
	// When leaving a function, reduce nesting level.
	जइ (getline_equal(fgetline, cookie, get_func_पङ्क्तिः))
	    --ex_nesting_level;
	/*
	 * Go to debug mode when returning from a function in which we are
	 * single-stepping.
	 */
	जइ ((getline_equal(fgetline, cookie, getsourceline)
		    || getline_equal(fgetline, cookie, get_func_पङ्क्तिः))
		&& ex_nesting_level + 1 <= debug_break_level)
	    do_debug(getline_equal(fgetline, cookie, getsourceline)
		    ? (char_u *)_("End of sourced file")
		    : (char_u *)_("End of function"));
    }

    /*
     * Restore the exception environment (done after returning from the
     * debugger).
     */
    जइ (flags & DOCMD_EXCRESET)
	restore_dbg_stuff(&debug_saved);

    सन्देशः_list = saved_सन्देशः_list;

    // Cleanup if "cs_emsg_silent_list" remains.
    जइ (cstack.cs_emsg_silent_list != रिक्त)
    {
	eslist_T *elem, *temp;

	प्रति (elem = cstack.cs_emsg_silent_list; elem != रिक्त; elem = temp)
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
    जइ (did_inc_RedrawingDisabled)
    {
	जइ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	--no_wait_return;
	सन्देशः_scroll = FALSE;

	/*
	 * When just finished an ":if"-":else" which was typed, no need to
	 * wait for hit-return.  Also for an error situation.
	 */
	जइ (retval == FAIL
#ifdef FEAT_EVAL
		|| (did_endif && कुञ्चिका_Typed && !did_emsg)
#endif
					    )
	{
	    need_wait_return = FALSE;
	    सन्देशः_didany = FALSE;		// don't wait when restarting edit
	}
	अण्णहा जइ (need_wait_return)
	{
	    /*
	     * The msg_start() above clears msg_didout. The wait_return() we do
	     * here should not overwrite the command that may be shown before
	     * doing that.
	     */
	    सन्देशः_didout |= सन्देशः_didout_before_start;
	    wait_return(FALSE);
	}
    }

#ifdef FEAT_EVAL
    did_endif = FALSE;  // in case do_cmdline used recursively
#अण्णहा
    /*
     * Reset if_level, in case a sourced script file contains more ":if" than
     * ":endif" (could be ":if x | foo | endif").
     */
    if_level = 0;
#endif

    --call_depth;
    प्रत्यागम retval;
}

#जइ defined(FEAT_EVAL)
/*
 * Handle when "did_throw" is set after executing commands.
 */
    शून्य
handle_did_throw(शून्य)
{
    वर्ण	*p = रिक्त;
    msglist_T	*messages = रिक्त;
    ESTACK_CHECK_DECLARATION;

    /*
     * If the uncaught exception is a user exception, report it as an
     * error.  If it is an error exception, display the saved error
     * message now.  For an interrupt exception, do nothing; the
     * interrupt message is given elsewhere.
     */
    विकल्प (current_exception->type)
    {
	पक्ष ET_USER:
	    vim_snprintf((वर्ण *)IObuff, IOSIZE,
		    _(e_exception_not_caught_str),
		    current_exception->मानम्);
	    p = (वर्ण *)vim_strsave(IObuff);
	    भङ्ग;
	पक्ष ET_ERROR:
	    messages = current_exception->messages;
	    current_exception->messages = रिक्त;
	    भङ्ग;
	पक्ष ET_INTERRUPT:
	    भङ्ग;
    }

    estack_प्रक्षिप(ETYPE_EXCEPT, current_exception->throw_name,
					current_exception->throw_lnum);
    ESTACK_CHECK_SETUP;
    current_exception->throw_name = रिक्त;

    discard_current_exception();	// uses IObuff if 'verbose'

    // If "silent!" is active the uncaught exception is not fatal.
    जइ (emsg_silent == 0)
    {
	suppress_errthrow = TRUE;
	force_abort = TRUE;
    }

    जइ (messages != रिक्त)
    {
	करेहि
	{
	    msglist_T	*next = messages->next;
	    पूर्णाङ्क		save_compiling = estack_compiling;

	    estack_compiling = messages->सन्देशः_compiling;
	    emsg(messages->सन्देशः);
	    vim_free(messages->सन्देशः);
	    vim_free(messages->sfile);
	    vim_free(messages);
	    messages = next;
	    estack_compiling = save_compiling;
	}
	जाव (messages != रिक्त);
    }
    अण्णहा जइ (p != रिक्त)
    {
	emsg(p);
	vim_free(p);
    }
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_निष्कासय();
}

/*
 * Obtain a line when inside a ":while" or ":for" loop.
 */
    स्थिर char_u *
get_loop_पङ्क्तिः(पूर्णाङ्क c, शून्य *cookie, पूर्णाङ्क indent, getline_opt_T options)
{
    संरचना loop_cookie	*cp = (संरचना loop_cookie *)cookie;
    wcmd_T		*wp;
    char_u		*पङ्क्तिः;

    जइ (cp->current_पङ्क्तिः + 1 >= cp->पङ्क्तयः_gap->ga_len)
    {
	जइ (cp->repeating)
	    प्रत्यागम रिक्त;	// trying to read past ":endwhile"/":endfor"

	// First time inside the ":while"/":for": get line normally.
	जइ (cp->lc_getline == रिक्त)
	    पङ्क्तिः = getcmdline(c, 0L, indent, 0);
	अण्णहा
	    पङ्क्तिः = cp->lc_getline(c, cp->cookie, indent, options);
	जइ (पङ्क्तिः != रिक्त && store_loop_पङ्क्तिः(cp->पङ्क्तयः_gap, पङ्क्तिः) == OK)
	    ++cp->current_पङ्क्तिः;

	प्रत्यागम पङ्क्तिः;
    }

    कुञ्चिका_Typed = FALSE;
    ++cp->current_पङ्क्तिः;
    wp = (wcmd_T *)(cp->पङ्क्तयः_gap->ga_data) + cp->current_पङ्क्तिः;
    SOURCING_LNUM = wp->lnum;
    प्रत्यागम vim_strsave(wp->पङ्क्तिः);
}

/*
 * Store a line in "gap" so that a ":while" loop can execute it again.
 */
    स्थिर पूर्णाङ्क
store_loop_पङ्क्तिः(garray_T *gap, char_u *पङ्क्तिः)
{
    जइ (ga_grow(gap, 1) == FAIL)
	प्रत्यागम FAIL;
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].पङ्क्तिः = vim_strsave(पङ्क्तिः);
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].lnum = SOURCING_LNUM;
    ++gap->ga_len;
    प्रत्यागम OK;
}

/*
 * Free the lines stored for a ":while" or ":for" loop.
 */
    स्थिर शून्य
free_cmdlines(garray_T *gap)
{
    जाव (gap->ga_len > 0)
    {
	vim_free(((wcmd_T *)(gap->ga_data))[gap->ga_len - 1].पङ्क्तिः);
	--gap->ga_len;
    }
}
#endif

/*
 * If "fgetline" is get_loop_line(), return TRUE if the getline it uses equals
 * "func".  * Otherwise return TRUE when "fgetline" equals "func".
 */
    पूर्णाङ्क
getline_equal(
    char_u	*(*fgetline)(पूर्णाङ्क, शून्य *, पूर्णाङ्क, getline_opt_T),
    शून्य	*cookie UNUSED,		// argument for fgetline()
    char_u	*(*func)(पूर्णाङ्क, शून्य *, पूर्णाङ्क, getline_opt_T))
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(पूर्णाङ्क, शून्य *, पूर्णाङ्क, getline_opt_T);
    संरचना loop_cookie *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // function that's originally used to obtain the lines.  This may be
    // nested several levels.
    gp = fgetline;
    cp = (संरचना loop_cookie *)cookie;
    जाव (gp == get_loop_पङ्क्तिः)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    प्रत्यागम gp == func;
#अण्णहा
    प्रत्यागम fgetline == func;
#endif
}

/*
 * If "fgetline" is get_loop_line(), return the cookie used by the original
 * getline function.  Otherwise return "cookie".
 */
    शून्य *
getline_cookie(
    char_u	*(*fgetline)(पूर्णाङ्क, शून्य *, पूर्णाङ्क, getline_opt_T) UNUSED,
    शून्य	*cookie)		// argument for fgetline()
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(पूर्णाङ्क, शून्य *, पूर्णाङ्क, getline_opt_T);
    संरचना loop_cookie  *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (संरचना loop_cookie *)cookie;
    जाव (gp == get_loop_पङ्क्तिः)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    प्रत्यागम cp;
#अण्णहा
    प्रत्यागम cookie;
#endif
}

#जइ defined(FEAT_EVAL)
/*
 * Get the next line source line without advancing.
 */
    char_u *
getline_peek(
    char_u	*(*fgetline)(पूर्णाङ्क, शून्य *, पूर्णाङ्क, getline_opt_T) UNUSED,
    शून्य	*cookie)		// argument for fgetline()
{
    char_u		*(*gp)(पूर्णाङ्क, शून्य *, पूर्णाङ्क, getline_opt_T);
    संरचना loop_cookie  *cp;
    wcmd_T		*wp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (संरचना loop_cookie *)cookie;
    जाव (gp == get_loop_पङ्क्तिः)
    {
	जइ (cp->current_पङ्क्तिः + 1 < cp->पङ्क्तयः_gap->ga_len)
	{
	    // executing lines a second time, use the stored copy
	    wp = (wcmd_T *)(cp->पङ्क्तयः_gap->ga_data) + cp->current_पङ्क्तिः + 1;
	    प्रत्यागम wp->पङ्क्तिः;
	}
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    जइ (gp == getsourceline)
	प्रत्यागम source_nextline(cp);
    प्रत्यागम रिक्त;
}
#endif


/*
 * Helper function to apply an offset for buffer commands, i.e. ":bdelete",
 * ":bwipeout", etc.
 * Returns the buffer number.
 */
    स्थिर पूर्णाङ्क
compute_buffer_local_गणनम्(पूर्णाङ्क addr_type, पूर्णाङ्क lnum, पूर्णाङ्क offset)
{
    buf_T   *buf;
    buf_T   *nextbuf;
    पूर्णाङ्क     गणनम् = offset;

    buf = firstbuf;
    जाव (buf->b_next != रिक्त && buf->b_fnum < lnum)
	buf = buf->b_next;
    जाव (गणनम् != 0)
    {
	गणनम् += (offset < 0) ? 1 : -1;
	nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
	जइ (nextbuf == रिक्त)
	    भङ्ग;
	buf = nextbuf;
	जइ (addr_type == ADDR_LOADED_BUFFERS)
	    // skip over unloaded buffers
	    जाव (buf->b_ml.ml_mfp == रिक्त)
	    {
		nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
		जइ (nextbuf == रिक्त)
		    भङ्ग;
		buf = nextbuf;
	    }
    }
    // we might have gone too far, last buffer is not loadedd
    जइ (addr_type == ADDR_LOADED_BUFFERS)
	जाव (buf->b_ml.ml_mfp == रिक्त)
	{
	    nextbuf = (offset >= 0) ? buf->b_prev : buf->b_next;
	    जइ (nextbuf == रिक्त)
		भङ्ग;
	    buf = nextbuf;
	}
    प्रत्यागम buf->b_fnum;
}

/*
 * Return the window number of "win".
 * When "win" is NULL return the number of windows.
 */
    स्थिर पूर्णाङ्क
current_win_nr(win_T *win)
{
    win_T	*wp;
    पूर्णाङ्क		nr = 0;

    FOR_ALL_WINDOWS(wp)
    {
	++nr;
	जइ (wp == win)
	    भङ्ग;
    }
    प्रत्यागम nr;
}

    स्थिर पूर्णाङ्क
current_tab_nr(tabpage_T *tab)
{
    tabpage_T	*tp;
    पूर्णाङ्क		nr = 0;

    FOR_ALL_TABPAGES(tp)
    {
	++nr;
	जइ (tp == tab)
	    भङ्ग;
    }
    प्रत्यागम nr;
}

    स्थिर पूर्णाङ्क
comment_start(char_u *p, पूर्णाङ्क starts_with_colon UNUSED)
{
    जइ (in_vim9script())
	प्रत्यागम p[0] == '#' && !starts_with_colon;
    प्रत्यागम *p == '"';
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
    पूर्णाङ्क		flags,
#ifdef FEAT_EVAL
    cstack_T	*cstack,
#endif
    char_u	*(*fgetline)(पूर्णाङ्क, शून्य *, पूर्णाङ्क, getline_opt_T),
    शून्य	*cookie)		// argument for fgetline()
{
    char_u	*p;
    linenr_T	lnum;
    long	n;
    वर्ण	*errormsg = रिक्त;	// error message
    char_u	*after_modifier = रिक्त;
    exarg_T	ea;			// Ex command arguments
    cmdmod_T	save_cmdmod;
    पूर्णाङ्क		save_reg_executing = reg_executing;
    पूर्णाङ्क		save_pending_end_reg_executing = pending_end_reg_executing;
    पूर्णाङ्क		ni;			// set when Not Implemented
    char_u	*cmd;
    पूर्णाङ्क		starts_with_colon = FALSE;
    पूर्णाङ्क		may_have_range;
#ifdef FEAT_EVAL
    पूर्णाङ्क		did_set_व्यञ्जकम्_पङ्क्तिः = FALSE;
#endif
    पूर्णाङ्क		sourcing = flags & DOCMD_VERBOSE;
    पूर्णाङ्क		did_append_cmd = FALSE;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
#ifdef FEAT_EVAL
    ++ex_nesting_level;
#endif

    // When the last file has not been edited :q has to be typed twice.
    जइ (quitmore
#ifdef FEAT_EVAL
	    // avoid that a function call in 'statusline' does this
	    && !getline_equal(fgetline, cookie, get_func_पङ्क्तिः)
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
    जइ ((*cmdlinep)[0] == '#' && (*cmdlinep)[1] == '!')
	गच्छ doend;

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
    जइ (parse_command_modifiers(&ea, &errormsg, &cmdmod, FALSE) == FAIL)
	गच्छ doend;
    apply_cmdmod(&cmdmod);
    after_modifier = ea.cmd;

#ifdef FEAT_EVAL
    ea.skip = did_emsg || got_int || did_throw || (cstack->cs_idx >= 0
			 && !(cstack->cs_flags[cstack->cs_idx] & CSF_ACTIVE));
#अण्णहा
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
    पूर्णाङ्क vim9script = in_vim9script();
    जइ (vim9script && (flags & DOCMD_RANGEOK) == 0)
    {
	may_have_range = FALSE;
	प्रति (p = ea.cmd; p >= *cmdlinep; --p)
	{
	    जइ (*p == ':')
		may_have_range = TRUE;
	    जइ (p < ea.cmd && !VIM_ISWHITE(*p))
		भङ्ग;
	}
    }
    अण्णहा
	may_have_range = TRUE;
    जइ (may_have_range)
	ea.cmd = skip_range(ea.cmd, TRUE, रिक्त);

#ifdef FEAT_EVAL
    // Handle ":export" - it functions almost like a command modifier.
    // ":export var Name: type"
    // ":export def Name(..."
    // etc.
    जइ (vim9script && checkforcmd_noparen(&ea.cmd, "export", 6))
	is_export = TRUE;
#endif

    जइ (vim9script && !may_have_range)
    {
	जइ (ea.cmd == cmd + 1 && *cmd == '$')
	    // should be "$VAR = val"
	    --ea.cmd;
#ifdef FEAT_EVAL
	p = find_ex_command(&ea, रिक्त, lookup_scriptitem, रिक्त);
#अण्णहा
	p = find_ex_command(&ea, रिक्त, रिक्त, रिक्त);
#endif
	जइ (ea.cmdidx == CMD_SIZE)
	{
	    char_u *ar = skip_range(ea.cmd, TRUE, रिक्त);

	    // If a ':' before the range is missing, give a clearer error
	    // message.
	    जइ (ar > ea.cmd && !ea.skip)
	    {
		semsg(_(e_colon_required_before_range_str), ea.cmd);
		गच्छ doend;
	    }
	}
    }
    अण्णहा
	p = find_ex_command(&ea, रिक्त, रिक्त, रिक्त);

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    // Count this line for profiling if skip is TRUE.
    जइ (do_profiling == PROF_YES
	    && (!ea.skip || cstack->cs_idx == 0 || (cstack->cs_idx > 0
		     && (cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE))))
    {
	पूर्णाङ्क skip = did_emsg || got_int || did_throw;

	जइ (ea.cmdidx == CMD_catch)
	    skip = !skip && !(cstack->cs_idx >= 0
			  && (cstack->cs_flags[cstack->cs_idx] & CSF_THROWN)
			  && !(cstack->cs_flags[cstack->cs_idx] & CSF_CAUGHT));
	अण्णहा जइ (ea.cmdidx == CMD_else || ea.cmdidx == CMD_elseif)
	    skip = skip || !(cstack->cs_idx >= 0
			  && !(cstack->cs_flags[cstack->cs_idx]
						  & (CSF_ACTIVE | CSF_TRUE)));
	अण्णहा जइ (ea.cmdidx == CMD_finally)
	    skip = FALSE;
	अण्णहा जइ (ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endtry
		&& ea.cmdidx != CMD_endwhile)
	    skip = ea.skip;

	जइ (!skip)
	{
	    जइ (getline_equal(fgetline, cookie, get_func_पङ्क्तिः))
		func_पङ्क्तिः_exec(getline_cookie(fgetline, cookie));
	    अण्णहा जइ (getline_equal(fgetline, cookie, getsourceline))
		script_पङ्क्तिः_exec();
	}
    }
# endif
#endif

    ea.cmd = cmd;

#ifdef FEAT_EVAL
    // May go to debug mode.  If this happens and the ">quit" debug command is
    // used, throw an interrupt exception and skip the next command.
    dbg_check_breakpoint(&ea);
    जइ (!ea.skip && got_int)
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
    जइ (!IS_USER_CMDIDX(ea.cmdidx))
    {
	जइ (ea.cmdidx != CMD_SIZE)
	    ea.addr_type = cmdnames[(पूर्णाङ्क)ea.cmdidx].cmd_addr_type;
	अण्णहा
	    ea.addr_type = ADDR_पङ्क्तयः;

	// :wincmd range depends on the argument.
	जइ (ea.cmdidx == CMD_wincmd && p != रिक्त)
	    get_wincmd_addr_type(skipwhite(p), &ea);
#ifdef FEAT_QUICKFIX
	// :.cc in quickfix window uses line number
	जइ ((ea.cmdidx == CMD_cc || ea.cmdidx == CMD_ll) && bt_quickfix(curbuf))
	    ea.addr_type = ADDR_OTHER;
#endif
    }

    जइ (!may_have_range)
	ea.line1 = ea.line2 = default_address(&ea);
    अण्णहा जइ (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
	गच्छ doend;

/*
 * 5. Parse the command.
 */

    /*
     * Skip ':' and any white space
     */
    ea.cmd = skipwhite(ea.cmd);
    जाव (*ea.cmd == ':')
	ea.cmd = skipwhite(ea.cmd + 1);

    /*
     * If we got a line, but no command, then go to the line.
     * If we find a '|' or '\n' we set ea.nextcmd.
     */
    जइ (*ea.cmd == NUL || comment_start(ea.cmd, starts_with_colon)
			       || (ea.nextcmd = check_nextcmd(ea.cmd)) != रिक्त)
    {
	/*
	 * strange vi behaviour:
	 * ":3"		jumps to line 3
	 * ":3|..."	prints line 3  (not in Vim9 script)
	 * ":|"		prints current line  (not in Vim9 script)
	 */
	जइ (ea.skip)	    // skip this if inside :if
	    गच्छ doend;
	errormsg = ex_range_without_command(&ea);
	गच्छ doend;
    }

    // If this looks like an undefined user command and there are CmdUndefined
    // autocommands defined, trigger the matching autocommands.
    जइ (p != रिक्त && ea.cmdidx == CMD_SIZE && !ea.skip
	    && ASCII_ISUPPER(*ea.cmd)
	    && has_cmdundefined())
    {
	पूर्णाङ्क ret;

	p = ea.cmd;
	जाव (ASCII_ISALNUM(*p))
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

    जइ (p == रिक्त)
    {
	जइ (!ea.skip)
	    errormsg = _(e_ambiguous_use_of_user_defined_command);
	गच्छ doend;
    }
    // Check for wrong commands.
    जइ (*p == '!' && ea.cmd[1] == 0151 && ea.cmd[0] == 78
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	errormsg = uc_fun_cmd();
	गच्छ doend;
    }

    जइ (ea.cmdidx == CMD_SIZE)
    {
	जइ (!ea.skip)
	{
	    STRCPY(IObuff, _(e_not_an_editor_command));
	    जइ (!sourcing)
	    {
		// If the modifier was parsed OK the error must be in the
		// following command
		जइ (after_modifier != रिक्त)
		    append_command(after_modifier);
		अण्णहा
		    append_command(*cmdlinep);
		did_append_cmd = TRUE;
	    }
	    errormsg = (वर्ण *)IObuff;
	    did_emsg_syntax = TRUE;
	}
	गच्छ doend;
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
    जइ (ea.cmdidx == CMD_if)
	++if_level;
    जइ (if_level)
    {
	जइ (ea.cmdidx == CMD_endif)
	    --if_level;
	गच्छ doend;
    }

#endif

    // forced commands
    जइ (*p == '!' && ea.cmdidx != CMD_substitute
	    && ea.cmdidx != CMD_smagic && ea.cmdidx != CMD_snomagic)
    {
	++p;
	ea.forceit = TRUE;
    }
    अण्णहा
	ea.forceit = FALSE;

/*
 * 6. Parse arguments.  Then check for errors.
 */
    जइ (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = (long)cmdnames[(पूर्णाङ्क)ea.cmdidx].cmd_argt;

    जइ (!ea.skip)
    {
#ifdef HAVE_SANDBOX
	जइ (sandbox != 0 && !(ea.argt & EX_SBOXOK))
	{
	    // Command not allowed in sandbox.
	    errormsg = _(e_not_allowed_in_sandbox);
	    गच्छ doend;
	}
#endif
	जइ (restricted != 0 && (ea.argt & EX_RESTRICT))
	{
	    errormsg = _(e_command_not_allowed_in_rvim);
	    गच्छ doend;
	}
	जइ (!curbuf->b_p_ma && (ea.argt & EX_MODIFY))
	{
	    // Command not allowed in non-'modifiable' buffer
	    errormsg = _(e_cannot_make_changes_modifiable_is_off);
	    गच्छ doend;
	}

	जइ (!IS_USER_CMDIDX(ea.cmdidx))
	{
	    जइ (cmdwin_type != 0 && !(ea.argt & EX_CMDWIN))
	    {
		// Command not allowed in the command line window
		errormsg = _(e_invalid_in_cmdline_window);
		गच्छ doend;
	    }
	    जइ (text_locked() && !(ea.argt & EX_LOCK_OK))
	    {
		// Command not allowed when text is locked
		errormsg = _(get_text_locked_सन्देशः());
		गच्छ doend;
	    }
	}

	// Disallow editing another buffer when "curbuf_lock" is set.
	// Do allow ":checktime" (it is postponed).
	// Do allow ":edit" (check for an argument later).
	// Do allow ":file" with no arguments (check for an argument later).
	जइ (!(ea.argt & (EX_CMDWIN | EX_LOCK_OK))
		&& ea.cmdidx != CMD_checktime
		&& ea.cmdidx != CMD_edit
		&& ea.cmdidx != CMD_file
		&& !IS_USER_CMDIDX(ea.cmdidx)
		&& curbuf_locked())
	    गच्छ doend;

	जइ (!ni && !(ea.argt & EX_RANGE) && ea.addr_गणनम् > 0)
	{
	    errormsg = _(e_no_range_allowed);
	    गच्छ doend;
	}
    }

    जइ (!ni && !(ea.argt & EX_BANG) && ea.forceit)
    {
	errormsg = _(e_no_bang_allowed);
	गच्छ doend;
    }

    /*
     * Don't complain about the range if it is not used
     * (could happen if line_count is accidentally set to 0).
     */
    जइ (!ea.skip && !ni && (ea.argt & EX_RANGE))
    {
	/*
	 * If the range is backwards, ask for confirmation and, if given, swap
	 * ea.line1 & ea.line2 so it's forwards again.
	 * When global command is busy, don't ask, will fail below.
	 */
	जइ (!global_busy && ea.line1 > ea.line2)
	{
	    जइ (सन्देशः_silent == 0)
	    {
		जइ (sourcing || exmode_active)
		{
		    errormsg = _(e_backwards_range_given);
		    गच्छ doend;
		}
		जइ (ask_yesno((char_u *)
			_("Backwards range given, OK to swap"), FALSE) != 'y')
		    गच्छ doend;
	    }
	    lnum = ea.line1;
	    ea.line1 = ea.line2;
	    ea.line2 = lnum;
	}
	जइ ((errormsg = invalid_range(&ea)) != रिक्त)
	    गच्छ doend;
    }

    जइ ((ea.addr_type == ADDR_OTHER) && ea.addr_गणनम् == 0)
	// default is 1, not cursor
	ea.line2 = 1;

    correct_range(&ea);

#ifdef FEAT_FOLDING
    जइ (((ea.argt & EX_WHOLEFOLD) || ea.addr_गणनम् >= 2) && !global_busy
	    && ea.addr_type == ADDR_पङ्क्तयः)
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
    जइ (p == रिक्त)
	गच्छ doend;
#endif

    /*
     * Skip to start of argument.
     * Don't do this for the ":!" command, because ":!! -l" needs the space.
     */
    जइ (ea.cmdidx == CMD_bang)
	ea.arg = p;
    अण्णहा
	ea.arg = skipwhite(p);

    // ":file" cannot be run with an argument when "curbuf_lock" is set
    जइ (ea.cmdidx == CMD_file && *ea.arg != NUL && curbuf_locked())
	गच्छ doend;

    /*
     * Check for "++opt=val" argument.
     * Must be first, allow ":w ++enc=utf8 !cmd"
     */
    जइ (ea.argt & EX_ARGOPT)
	जाव (ea.arg[0] == '+' && ea.arg[1] == '+')
	    जइ (getargopt(&ea) == FAIL && !ni)
	    {
		errormsg = _(e_invalid_argument);
		गच्छ doend;
	    }

    जइ (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	जइ (*ea.arg == '>')			// append
	{
	    जइ (*++ea.arg != '>')		// typed wrong
	    {
		errormsg = _(e_use_w_or_w_gt_gt);
		गच्छ doend;
	    }
	    ea.arg = skipwhite(ea.arg + 1);
	    ea.append = TRUE;
	}
	अण्णहा जइ (*ea.arg == '!' && ea.cmdidx == CMD_write)  // :w !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    जइ (ea.cmdidx == CMD_read)
    {
	जइ (ea.forceit)
	{
	    ea.usefilter = TRUE;		// :r! filter if ea.forceit
	    ea.forceit = FALSE;
	}
	अण्णहा जइ (*ea.arg == '!')		// :r !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    जइ (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ea.amount = 1;
	जाव (*ea.arg == *ea.cmd)		// count number of '>' or '<'
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
    जइ ((ea.argt & EX_CMDARG) && !ea.usefilter)
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
    जइ ((ea.argt & EX_TRLBAR) && !ea.usefilter)
    {
	separate_nextcmd(&ea, FALSE);
    }
    अण्णहा जइ (ea.cmdidx == CMD_bang
	    || ea.cmdidx == CMD_terminal
	    || ea.cmdidx == CMD_global
	    || ea.cmdidx == CMD_vglobal
	    || ea.usefilter
#ifdef FEAT_EVAL
	    || inside_block(&ea)
#endif
	    )
    {
	प्रति (p = ea.arg; *p; ++p)
	{
	    // Remove one backslash before a newline
	    जइ (*p == '\\' && p[1] == '\n')
		STRMOVE(p, p + 1);
	    अण्णहा जइ (*p == '\n' && !(ea.argt & EX_व्यञ्जकम्_ARG))
	    {
		ea.nextcmd = p + 1;
		*p = NUL;
		भङ्ग;
	    }
	}
    }

    जइ ((ea.argt & EX_DFLALL) && ea.addr_गणनम् == 0)
	address_default_all(&ea);

    // accept numbered register only when no count allowed (:put)
    जइ (       (ea.argt & EX_REGSTR)
	    && *ea.arg != NUL
	       // Do not allow register = for user commands
	    && (!IS_USER_CMDIDX(ea.cmdidx) || *ea.arg != '=')
	    && !((ea.argt & EX_गणनम्) && VIM_ISDIGIT(*ea.arg)))
    {
#जइ !defined(FEAT_CLIPBOARD) && !defined(FEAT_CLIPBOARD_PROVIDER)
	// check these explicitly for a more specific error message
	जइ (*ea.arg == '*' || *ea.arg == '+')
	{
	    errormsg = _(e_invalid_register_name);
	    गच्छ doend;
	}
#endif
	जइ (valid_yank_reg(*ea.arg, (!IS_USER_CMDIDX(ea.cmdidx)
			    && ea.cmdidx != CMD_put && ea.cmdidx != CMD_iput)))
	{
	    ea.regname = *ea.arg++;
#ifdef FEAT_EVAL
	    // for '=' register: accept the rest of the line as an expression
	    जइ (ea.arg[-1] == '=' && ea.arg[0] != NUL)
	    {
		जइ (!ea.skip)
		{
		    set_व्यञ्जकम्_पङ्क्तिः(vim_strsave(ea.arg), &ea);
		    did_set_व्यञ्जकम्_पङ्क्तिः = TRUE;
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
    जइ ((ea.argt & EX_गणनम्) && VIM_ISDIGIT(*ea.arg)
	    && (!(ea.argt & EX_BUFNAME) || *(p = skipdigits(ea.arg + 1)) == NUL
							  || VIM_ISWHITE(*p)))
    {
	n = getdigits_quoted(&ea.arg);
	ea.arg = skipwhite(ea.arg);
	जइ (n <= 0 && !ni && (ea.argt & EX_ZEROR) == 0)
	{
	    errormsg = _(e_positive_गणनम्_required);
	    गच्छ doend;
	}
	जइ (ea.addr_type != ADDR_पङ्क्तयः)	// e.g. :buffer 2, :sleep 3
	{
	    ea.line2 = n;
	    जइ (ea.addr_गणनम् == 0)
		ea.addr_गणनम् = 1;
	}
	अण्णहा
	{
	    ea.line1 = ea.line2;
	    जइ (ea.line2 >= LONG_MAX - (n - 1))
		ea.line2 = LONG_MAX;  // avoid overflow
	    अण्णहा
		ea.line2 += n - 1;
	    ++ea.addr_गणनम्;
	    /*
	     * Be vi compatible: no error message for out of range.
	     */
	    जइ (ea.line2 > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
		ea.line2 = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
	}
    }

    /*
     * Check for flags: 'l', 'p' and '#'.
     */
    जइ (ea.argt & EX_FLAGS)
	get_flags(&ea);
    जइ (!ni && !(ea.argt & EX_EXTRA) && *ea.arg != NUL
	    && *ea.arg != '"' && (*ea.arg != '|' || (ea.argt & EX_TRLBAR) == 0))
    {
	// no arguments allowed but there is something
	errormsg = ex_errmsg(e_trailing_characters_str, ea.arg);
	गच्छ doend;
    }

    जइ (!ni && (ea.argt & EX_NEEDARG) && *ea.arg == NUL)
    {
	errormsg = _(e_argument_required);
	गच्छ doend;
    }

#ifdef FEAT_EVAL
    /*
     * Skip the command when it's not going to be executed.
     * The commands like :if, :endif, etc. always need to be executed.
     * Also make an exception for commands that handle a trailing command
     * themselves.
     */
    जइ (ea.skip)
    {
	विकल्प (ea.cmdidx)
	{
	    // commands that need evaluation
	    पक्ष CMD_while:
	    पक्ष CMD_endwhile:
	    पक्ष CMD_for:
	    पक्ष CMD_endfor:
	    पक्ष CMD_if:
	    पक्ष CMD_elseif:
	    पक्ष CMD_else:
	    पक्ष CMD_endif:
	    पक्ष CMD_try:
	    पक्ष CMD_catch:
	    पक्ष CMD_finally:
	    पक्ष CMD_endtry:
	    पक्ष CMD_function:
	    पक्ष CMD_def:
				भङ्ग;

	    // commands that read a block of lines
	    पक्ष CMD_abstract:
	    पक्ष CMD_append:
	    पक्ष CMD_autocmd:
	    पक्ष CMD_change:
	    पक्ष CMD_class:
	    पक्ष CMD_command:
	    पक्ष CMD_enum:
	    पक्ष CMD_निवेशय:
	    पक्ष CMD_interface:
	    पक्ष CMD_loadkeymap:
				भङ्ग;

	    // Commands that handle '|' themselves.  Check: A command should
	    // either have the EX_TRLBAR flag, appear in this list or appear in
	    // the list at ":help :bar".
	    पक्ष CMD_aboveleft:
	    पक्ष CMD_and:
	    पक्ष CMD_belowright:
	    पक्ष CMD_botright:
	    पक्ष CMD_browse:
	    पक्ष CMD_call:
	    पक्ष CMD_confirm:
	    पक्ष CMD_const:
	    पक्ष CMD_delfunction:
	    पक्ष CMD_djump:
	    पक्ष CMD_dlist:
	    पक्ष CMD_dsearch:
	    पक्ष CMD_dsplit:
	    पक्ष CMD_echo:
	    पक्ष CMD_echoerr:
	    पक्ष CMD_echomsg:
	    पक्ष CMD_echon:
	    पक्ष CMD_eval:
	    पक्ष CMD_execute:
	    पक्ष CMD_filter:
	    पक्ष CMD_final:
	    पक्ष CMD_help:
	    पक्ष CMD_hide:
	    पक्ष CMD_horizontal:
	    पक्ष CMD_ijump:
	    पक्ष CMD_ilist:
	    पक्ष CMD_isearch:
	    पक्ष CMD_isplit:
	    पक्ष CMD_keepalt:
	    पक्ष CMD_keepjumps:
	    पक्ष CMD_keepmarks:
	    पक्ष CMD_keeppatterns:
	    पक्ष CMD_leftabove:
	    पक्ष CMD_let:
	    पक्ष CMD_lockmarks:
	    पक्ष CMD_lockvar:
	    पक्ष CMD_lua:
	    पक्ष CMD_match:
	    पक्ष CMD_mzscheme:
	    पक्ष CMD_noautocmd:
	    पक्ष CMD_noswapfile:
	    पक्ष CMD_perl:
	    पक्ष CMD_psearch:
	    पक्ष CMD_py3:
	    पक्ष CMD_python3:
	    पक्ष CMD_python:
	    पक्ष CMD_return:
	    पक्ष CMD_rightbelow:
	    पक्ष CMD_ruby:
	    पक्ष CMD_silent:
	    पक्ष CMD_smagic:
	    पक्ष CMD_snomagic:
	    पक्ष CMD_substitute:
	    पक्ष CMD_syntax:
	    पक्ष CMD_tab:
	    पक्ष CMD_tcl:
	    पक्ष CMD_throw:
	    पक्ष CMD_tilde:
	    पक्ष CMD_topleft:
	    पक्ष CMD_unlet:
	    पक्ष CMD_unlockvar:
	    पक्ष CMD_var:
	    पक्ष CMD_verbose:
	    पक्ष CMD_vertical:
	    पक्ष CMD_wincmd:
				भङ्ग;

	    स्वभाव:		गच्छ doend;
	}
    }
#endif

    जइ ((ea.argt & EX_XFILE)
	    && expand_filename(&ea, cmdlinep, &errormsg) == FAIL)
	गच्छ doend;

#ifdef FEAT_EVAL
    जइ (is_export && (ea.argt & EX_EXPORT) == 0)
    {
	emsg(_(e_invalid_command_after_export));
	गच्छ doend;
    }
#endif

    /*
     * Accept buffer name.  Cannot be used at the same time with a buffer
     * number.  Don't do this for a user command.
     */
    जइ ((ea.argt & EX_BUFNAME) && *ea.arg != NUL && ea.addr_गणनम् == 0
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * :bdelete, :bwipeout and :bunload take several arguments, separated
	 * by spaces: find next space (skipping over escaped characters).
	 * The others take one argument: ignore trailing spaces.
	 */
	जइ (ea.cmdidx == CMD_bdelete || ea.cmdidx == CMD_bwipeout
						  || ea.cmdidx == CMD_bunload)
	    p = skiptowhite_esc(ea.arg);
	अण्णहा
	{
	    p = ea.arg + STRLEN(ea.arg);
	    जाव (p > ea.arg && VIM_ISWHITE(p[-1]))
		--p;
	}
	ea.line2 = buflist_findpat(ea.arg, p, (ea.argt & EX_BUFUNL) != 0,
								FALSE, FALSE);
	जइ (ea.line2 < 0)	    // failed
	    गच्छ doend;
	ea.addr_गणनम् = 1;
	ea.arg = skipwhite(p);
    }

    // The :try command saves the emsg_silent flag, reset it here when
    // ":silent! try" was used, it should only apply to :try itself.
    जइ (ea.cmdidx == CMD_try && cmdmod.cmod_did_esilent > 0)
    {
	emsg_silent -= cmdmod.cmod_did_esilent;
	जइ (emsg_silent < 0)
	    emsg_silent = 0;
	cmdmod.cmod_did_esilent = 0;
    }

/*
 * 7. Execute the command.
 */

    जइ (IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * Execute a user-defined command.
	 */
	do_ucmd(&ea);
    }
    अण्णहा
    {
	/*
	 * Call the function to execute the builtin command.
	 */
	(cmdnames[ea.cmdidx].cmd_func)(&ea);
	जइ (ea.errmsg != रिक्त)
	    errormsg = ea.errmsg;
    }

#ifdef FEAT_EVAL
    // A command will reset "is_export" when exporting an item.  If it is still
    // set something went wrong or the command was never executed.
    जइ (!ea.skip && is_export)
    {
	जइ (errormsg == रिक्त)
	    errormsg = _(e_export_with_invalid_argument);
	is_export = FALSE;
    }

    // Set flag that any command was executed, used by ex_vim9script().
    // Not if this was a command that wasn't executed or :endif.
    जइ (sourcing_a_script(&ea)
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
    जइ (need_rethrow)
	do_throw(cstack);
    अण्णहा जइ (check_cstack)
    {
	जइ (source_finished(fgetline, cookie))
	    do_finish(&ea, TRUE);
	अण्णहा जइ (getline_equal(fgetline, cookie, get_func_पङ्क्तिः)
						   && current_func_returned())
	    do_return(&ea, TRUE, FALSE, रिक्त);
    }
    need_rethrow = check_cstack = FALSE;
#endif

doend:
    जइ (curwin->w_cursor.lnum == 0)	// can happen with zero line number
    {
	curwin->w_cursor.lnum = 1;
	curwin->w_cursor.col = 0;
    }

    जइ (errormsg != रिक्त && *errormsg != NUL && !did_emsg)
    {
	जइ ((sourcing || !कुञ्चिका_Typed) && !did_append_cmd)
	{
	    जइ (errormsg != (वर्ण *)IObuff)
	    {
		STRCPY(IObuff, errormsg);
		errormsg = (वर्ण *)IObuff;
	    }
	    append_command(*cmdlinep);
	}
	emsg(errormsg);
    }
#ifdef FEAT_EVAL
    do_errthrow(cstack,
	    (ea.cmdidx != CMD_SIZE && !IS_USER_CMDIDX(ea.cmdidx))
			? cmdnames[(पूर्णाङ्क)ea.cmdidx].cmd_name : (char_u *)रिक्त);

    जइ (did_set_व्यञ्जकम्_पङ्क्तिः)
	set_व्यञ्जकम्_पङ्क्तिः(रिक्त, रिक्त);
    is_export = FALSE;
#endif

    undo_cmdmod(&cmdmod);
    cmdmod = save_cmdmod;
    reg_executing = save_reg_executing;
    pending_end_reg_executing = save_pending_end_reg_executing;

    जइ (ea.nextcmd && *ea.nextcmd == NUL)	// not really a next command
	ea.nextcmd = रिक्त;

#ifdef FEAT_EVAL
    --ex_nesting_level;
    vim_free(ea.cmdline_tofree);
#endif

    प्रत्यागम ea.nextcmd;
}

स्थिर वर्ण ex_error_buf[सन्देशः_BUF_LEN];

/*
 * Return an error message with argument included.
 * Uses a static buffer, only the last error will be kept.
 * "msg" will be translated, caller should use N_().
 */
    वर्ण *
ex_errmsg(वर्ण *सन्देशः, char_u *arg)
{
    vim_snprintf(ex_error_buf, सन्देशः_BUF_LEN, _(सन्देशः), arg);
    प्रत्यागम ex_error_buf;
}

/*
 * The "+" string used in place of an empty command in Ex mode.
 * This string is used in pointer comparison.
 */
स्थिर ध्रुव वर्ण exmode_plus[] = "+";

/*
 * Handle a range without a command.
 * Returns an error message on failure.
 */
    वर्ण *
ex_range_without_command(exarg_T *eap)
{
    वर्ण *errormsg = रिक्त;

    जइ ((*eap->cmd == '|' ||
		(exmode_active && eap->cmd != (char_u *)exmode_plus + 1))
#ifdef FEAT_EVAL
	    && !in_vim9script()
#endif
       )
    {
	eap->cmdidx = CMD_print;
	eap->argt = EX_RANGE+EX_गणनम्+EX_TRLBAR;
	जइ ((errormsg = invalid_range(eap)) == रिक्त)
	{
	    correct_range(eap);
	    ex_print(eap);
	}
    }
    अण्णहा जइ (eap->addr_गणनम् != 0)
    {
	जइ (eap->line2 > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	{
	    // With '-' in 'cpoptions' a line number past the file is an
	    // error, otherwise put it at the end of the file.
	    जइ (vim_strchr(p_cpo, CPO_MINUS) != रिक्त)
		eap->line2 = -1;
	    अण्णहा
		eap->line2 = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
	}

	जइ (eap->line2 < 0)
	    errormsg = _(e_invalid_range);
	अण्णहा
	{
	    जइ (eap->line2 == 0)
		curwin->w_cursor.lnum = 1;
	    अण्णहा
		curwin->w_cursor.lnum = eap->line2;
	    beginline(BL_SOL | BL_FIX);
	}
    }
    प्रत्यागम errormsg;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 * If "noparen" is TRUE do not recognize the command followed by "(" or ".".
 */
    स्थिर पूर्णाङ्क
checkforcmd_opt(
    char_u	**pp,		// start of command
    वर्ण	*cmd,		// name of command
    पूर्णाङ्क		len,		// required length
    पूर्णाङ्क		noparen)
{
    पूर्णाङ्क		i;

    प्रति (i = 0; cmd[i] != NUL; ++i)
	जइ (((char_u *)cmd)[i] != (*pp)[i])
	    भङ्ग;
    जइ (i >= len && !ASCII_ISALPHA((*pp)[i]) && (*pp)[i] != '_'
			 && (!noparen || ((*pp)[i] != '(' && (*pp)[i] != '.')))
    {
	*pp = skipwhite(*pp + i);
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    पूर्णाङ्क
checkforcmd(
    char_u	**pp,		// start of command
    वर्ण	*cmd,		// name of command
    पूर्णाङ्क		len)		// required length
{
    प्रत्यागम checkforcmd_opt(pp, cmd, len, FALSE);
}

/*
 * Check for an Ex command with optional tail, not followed by "(" or ".".
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    पूर्णाङ्क
checkforcmd_noparen(
    char_u	**pp,		// start of command
    वर्ण	*cmd,		// name of command
    पूर्णाङ्क		len)		// required length
{
    प्रत्यागम checkforcmd_opt(pp, cmd, len, TRUE);
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
    जाव (*p == '-')
	p = skipwhite(skiptowhite(p));

    // Skip over the command name.
    जइ (!ASCII_ISALPHA(*p))
	प्रत्यागम रिक्त;
    जाव (ASCII_ISALNUM(*p))
	++p;

    प्रत्यागम *p == NUL ? रिक्त : skipwhite(p);
}

/*
 * Find the "{" in "line" that starts a block for ":command" or ":autocmd".
 * That is the case when the command argument is "{" at the end of the line,
 * also when the command is nested in another ":command" or ":autocmd".
 * Returns NULL when the line does not start such a block.
 */
    char_u *
find_cmd_block_start(char_u *पङ्क्तिः)
{
    char_u	*p = skipwhite(पङ्क्तिः);

    प्रति (;;)
    {
	char_u	*arg = p;

	जइ (*p == '{' && ends_excmd2(p, skipwhite(p + 1)))
	    प्रत्यागम p;

	जइ (checkforcmd_noparen(&arg, "autocmd", 2))
	{
	    जइ (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = au_find_cmd_arg(arg);
	}
	अण्णहा जइ (checkforcmd_noparen(&arg, "command", 3))
	{
	    जइ (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = find_ucmd_repl(arg);
	}
	अण्णहा
	    प्रत्यागम रिक्त;

	जइ (p == रिक्त)
	    प्रत्यागम रिक्त;
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
    पूर्णाङ्क
parse_command_modifiers(
	exarg_T	    *eap,
	वर्ण	    **errormsg,
	cmdmod_T    *cmod,
	पूर्णाङ्क	    skip_only)
{
    char_u  *orig_cmd = eap->cmd;
    char_u  *cmd_start = रिक्त;
    पूर्णाङ्क	    use_plus_cmd = FALSE;
    पूर्णाङ्क	    starts_with_colon = FALSE;
    पूर्णाङ्क	    vim9script = in_vim9script();
    पूर्णाङ्क	    has_visual_range = FALSE;

    CLEAR_POINTER(cmod);
    cmod->cmod_flags = sticky_cmdmod_flags;

    जइ (STRNCMP(eap->cmd, "'<,'>", 5) == 0)
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
    प्रति (;;)
    {
	char_u  *p;

	जाव (*eap->cmd == ' ' || *eap->cmd == '\t' || *eap->cmd == ':')
	{
	    जइ (*eap->cmd == ':')
		starts_with_colon = TRUE;
	    ++eap->cmd;
	}

	// in ex mode, an empty command (after modifiers) works like :+
	जइ (*eap->cmd == NUL && exmode_active
		   && (getline_equal(eap->ea_getline, eap->cookie, getexmodeline)
		       || getline_equal(eap->ea_getline, eap->cookie, getexline))
			&& curwin->w_cursor.lnum < curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	{
	    use_plus_cmd = TRUE;
	    जइ (!skip_only)
		ex_pressedreturn = TRUE;
	    भङ्ग;  // no modifiers following
	}

	// ignore comment and empty lines
	जइ (comment_start(eap->cmd, starts_with_colon))
	{
	    // a comment ends at a NL
	    eap->nextcmd = vim_strchr(eap->cmd, '\n');
	    जइ (eap->nextcmd != रिक्त)
		++eap->nextcmd;
	    जइ (vim9script)
	    {
		जइ (has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
#ifdef FEAT_EVAL
		जइ (eap->cmd[0] == '#' && eap->cmd[1] == '{'
							 && eap->cmd[2] != '{')
		    *errormsg = _(e_cannot_use_hash_curly_to_start_comment);
#endif
	    }
	    प्रत्यागम FAIL;
	}
	जइ (*eap->cmd == '\n')
	{
	    eap->nextcmd = eap->cmd + 1;
	    प्रत्यागम FAIL;
	}
	जइ (*eap->cmd == NUL)
	{
	    जइ (!skip_only)
	    {
		ex_pressedreturn = TRUE;
		जइ (vim9script && has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
	    }
	    प्रत्यागम FAIL;
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
	जइ (vim9script)
	{
	    char_u *s, *n;

	    प्रति (s = eap->cmd; ASCII_ISALPHA(*s); ++s)
		;
	    n = skipwhite(s);
	    जइ (*n == '.' || *n == '=' || (*n != NUL && n[1] == '=')
		    || *s == '[')
		भङ्ग;
	}

	विकल्प (*p)
	{
	    // When adding an entry, also modify cmdmod_info_tab[].
	    पक्ष 'a':	जइ (!checkforcmd_noparen(&eap->cmd, "aboveleft", 3))
			    भङ्ग;
			cmod->cmod_split |= WSP_ABOVE;
			अनुवर्त;

	    पक्ष 'b':	जइ (checkforcmd_noparen(&eap->cmd, "belowright", 3))
			{
			    cmod->cmod_split |= WSP_BELOW;
			    अनुवर्त;
			}
			जइ (checkforcmd_opt(&eap->cmd, "browse", 3, TRUE))
			{
#ifdef FEAT_BROWSE_CMD
			    cmod->cmod_flags |= CMOD_BROWSE;
#endif
			    अनुवर्त;
			}
			जइ (!checkforcmd_noparen(&eap->cmd, "botright", 2))
			    भङ्ग;
			cmod->cmod_split |= WSP_BOT;
			अनुवर्त;

	    पक्ष 'c':	जइ (!checkforcmd_opt(&eap->cmd, "confirm", 4, TRUE))
			    भङ्ग;
#जइ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			cmod->cmod_flags |= CMOD_CONFIRM;
#endif
			अनुवर्त;

	    पक्ष 'k':	जइ (checkforcmd_noparen(&eap->cmd, "keepmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_KEEPMARKS;
			    अनुवर्त;
			}
			जइ (checkforcmd_noparen(&eap->cmd, "keepalt", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPALT;
			    अनुवर्त;
			}
			जइ (checkforcmd_noparen(&eap->cmd, "keeppatterns", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPPATTERNS;
			    अनुवर्त;
			}
			जइ (!checkforcmd_noparen(&eap->cmd, "keepjumps", 5))
			    भङ्ग;
			cmod->cmod_flags |= CMOD_KEEPJUMPS;
			अनुवर्त;

	    पक्ष 'f':	// only accept ":filter {pat} cmd"
			{
			    char_u  *reg_pat;
			    char_u  *nulp = रिक्त;
			    पूर्णाङ्क	    c = 0;

			    जइ (!checkforcmd_noparen(&p, "filter", 4)
				    || *p == NUL
				    || (ends_excmd(*p)
#ifdef FEAT_EVAL
					// in ":filter #pat# cmd" # does not
					// start a comment
				     && (!vim9script || VIM_ISWHITE(p[1]))
#endif
				     ))
				भङ्ग;
			    जइ (*p == '!')
			    {
				cmod->cmod_filter_force = TRUE;
				p = skipwhite(p + 1);
				जइ (*p == NUL || ends_excmd(*p))
				    भङ्ग;
			    }
#ifdef FEAT_EVAL
			    // Avoid that "filter(arg)" is recognized.
			    जइ (vim9script && !VIM_ISWHITE(p[-1]))
				भङ्ग;
#endif
			    जइ (skip_only)
				p = skip_vimgrep_pat(p, रिक्त, रिक्त);
			    अण्णहा
				// NOTE: This puts a NUL after the pattern.
				p = skip_vimgrep_pat_ext(p, &reg_pat, रिक्त,
								    &nulp, &c);
			    जइ (p == रिक्त || *p == NUL)
				भङ्ग;
			    जइ (!skip_only)
			    {
				cmod->cmod_filter_regmatch.regprog =
						vim_regcomp(reg_pat, RE_MAGIC);
				जइ (cmod->cmod_filter_regmatch.regprog == रिक्त)
				    भङ्ग;
				// restore the character overwritten by NUL
				जइ (nulp != रिक्त)
				    *nulp = c;
			    }
			    eap->cmd = p;
			    अनुवर्त;
			}

	    पक्ष 'h':	जइ (checkforcmd_noparen(&eap->cmd, "horizontal", 3))
			{
			    cmod->cmod_split |= WSP_HOR;
			    अनुवर्त;
			}
			// ":hide" and ":hide | cmd" are not modifiers
			जइ (p != eap->cmd || !checkforcmd_noparen(&p, "hide", 3)
					       || *p == NUL || ends_excmd(*p))
			    भङ्ग;
			eap->cmd = p;
			cmod->cmod_flags |= CMOD_HIDE;
			अनुवर्त;

	    पक्ष 'l':	जइ (checkforcmd_noparen(&eap->cmd, "lockmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_LOCKMARKS;
			    अनुवर्त;
			}
			जइ (checkforcmd_noparen(&eap->cmd, "legacy", 3))
			{
			    जइ (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_legacy_must_be_followed_by_command);
				प्रत्यागम FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_VIM9CMD;
			    cmod->cmod_flags |= CMOD_LEGACY;
			    अनुवर्त;
			}

			जइ (!checkforcmd_noparen(&eap->cmd, "leftabove", 5))
			    भङ्ग;
			cmod->cmod_split |= WSP_ABOVE;
			अनुवर्त;

	    पक्ष 'n':	जइ (checkforcmd_noparen(&eap->cmd, "noautocmd", 3))
			{
			    cmod->cmod_flags |= CMOD_NOAUTOCMD;
			    अनुवर्त;
			}
			जइ (!checkforcmd_noparen(&eap->cmd, "noswapfile", 3))
			    भङ्ग;
			cmod->cmod_flags |= CMOD_NOSWAPFILE;
			अनुवर्त;

	    पक्ष 'r':	जइ (!checkforcmd_noparen(&eap->cmd, "rightbelow", 6))
			    भङ्ग;
			cmod->cmod_split |= WSP_BELOW;
			अनुवर्त;

	    पक्ष 's':	जइ (checkforcmd_noparen(&eap->cmd, "sandbox", 3))
			{
			    cmod->cmod_flags |= CMOD_SANDBOX;
			    अनुवर्त;
			}
			जइ (!checkforcmd_noparen(&eap->cmd, "silent", 3))
			    भङ्ग;
			cmod->cmod_flags |= CMOD_SILENT;
			जइ (*eap->cmd == '!' && !VIM_ISWHITE(eap->cmd[-1]))
			{
			    // ":silent!", but not "silent !cmd"
			    eap->cmd = skipwhite(eap->cmd + 1);
			    cmod->cmod_flags |= CMOD_ERRSILENT;
			}
			अनुवर्त;

	    पक्ष 't':	जइ (checkforcmd_noparen(&p, "tab", 3))
			{
			    जइ (!skip_only)
			    {
				long tabnr = get_address(eap, &eap->cmd,
						    ADDR_TABS, eap->skip,
						    skip_only, FALSE, 1);
				जइ (tabnr == MAXLNUM)
				    cmod->cmod_tab = tabpage_index(curtab) + 1;
				अण्णहा
				{
				    जइ (tabnr < 0 || tabnr > LAST_TAB_NR)
				    {
					*errormsg = _(e_invalid_range);
					प्रत्यागम FAIL;
				    }
				    cmod->cmod_tab = tabnr + 1;
				}
			    }
			    eap->cmd = p;
			    अनुवर्त;
			}
			जइ (!checkforcmd_noparen(&eap->cmd, "topleft", 2))
			    भङ्ग;
			cmod->cmod_split |= WSP_शिखरम्;
			अनुवर्त;

	    पक्ष 'u':	जइ (!checkforcmd_noparen(&eap->cmd, "unsilent", 3))
			    भङ्ग;
			cmod->cmod_flags |= CMOD_UNSILENT;
			अनुवर्त;

	    पक्ष 'v':	जइ (checkforcmd_noparen(&eap->cmd, "vertical", 4))
			{
			    cmod->cmod_split |= WSP_VERT;
			    अनुवर्त;
			}
			जइ (checkforcmd_noparen(&eap->cmd, "vim9cmd", 4))
			{
			    जइ (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_vim9cmd_must_be_followed_by_command);
				प्रत्यागम FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_LEGACY;
			    cmod->cmod_flags |= CMOD_VIM9CMD;
			    अनुवर्त;
			}
			जइ (!checkforcmd_noparen(&p, "verbose", 4))
			    भङ्ग;
			जइ (vim_isdigit(*eap->cmd))
			{
			    // zero means not set, one is verbose == 0, etc.
			    cmod->cmod_verbose = atoi((वर्ण *)eap->cmd) + 1;
			}
			अण्णहा
			    cmod->cmod_verbose = 2;  // default: verbose == 1
			eap->cmd = p;
			अनुवर्त;
	}
	भङ्ग;
    }

    जइ (has_visual_range)
    {
	जइ (eap->cmd > cmd_start)
	{
	    // Move the '<,'> range to after the modifiers and insert a colon.
	    // Since the modifiers have been parsed put the colon on top of the
	    // space: "'<,'>mod cmd" -> "mod:'<,'>cmd
	    // Put eap->cmd after the colon.
	    जइ (use_plus_cmd)
	    {
		size_t len = STRLEN(cmd_start);

		// Special case: empty command uses "+":
		//  "'<,'>mods" -> "mods *+
		//  Use "*" instead of "'<,'>" to avoid the command getting
		//  longer, in case it was allocated.
		mch_memmove(orig_cmd, cmd_start, len);
		STRCPY(orig_cmd + len, " *+");
	    }
	    अण्णहा
	    {
		mch_memmove(cmd_start - 5, cmd_start, eap->cmd - cmd_start);
		eap->cmd -= 5;
		mch_memmove(eap->cmd - 1, ":'<,'>", 6);
	    }
	}
	अण्णहा
	    // No modifiers, move the pointer back.
	    // Special case: change empty command to "+".
	    जइ (use_plus_cmd)
		eap->cmd = (char_u *)"'<,'>+";
	    अण्णहा
		eap->cmd = orig_cmd;
    }
    अण्णहा जइ (use_plus_cmd)
	eap->cmd = (char_u *)exmode_plus;

    प्रत्यागम OK;
}

/*
 * Return TRUE if "cmod" has anything set.
 */
    पूर्णाङ्क
has_cmdmod(cmdmod_T *cmod, पूर्णाङ्क ignore_silent)
{
    प्रत्यागम (cmod->cmod_flags != 0 && (!ignore_silent
		|| (cmod->cmod_flags
		      & ~(CMOD_SILENT | CMOD_ERRSILENT | CMOD_UNSILENT)) != 0))
	    || cmod->cmod_split != 0
	    || cmod->cmod_verbose > 0
	    || cmod->cmod_tab != 0
	    || cmod->cmod_filter_regmatch.regprog != रिक्त;
}

#जइ defined(FEAT_EVAL)
/*
 * If Vim9 script and "cmdmod" has anything set give an error and return TRUE.
 */
    पूर्णाङ्क
cmdmod_error(पूर्णाङ्क ignore_silent)
{
    जइ (in_vim9script() && has_cmdmod(&cmdmod, ignore_silent))
    {
	emsg(_(e_misplaced_command_modifier));
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
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
    जइ ((cmod->cmod_flags & CMOD_SANDBOX) && !cmod->cmod_did_sandbox)
    {
	++sandbox;
	cmod->cmod_did_sandbox = TRUE;
    }
#endif
    जइ (cmod->cmod_verbose > 0)
    {
	जइ (cmod->cmod_verbose_save == 0)
	    cmod->cmod_verbose_save = p_verbose + 1;
	p_verbose = cmod->cmod_verbose - 1;
    }

    जइ ((cmod->cmod_flags & (CMOD_SILENT | CMOD_UNSILENT))
	    && cmod->cmod_save_सन्देशः_silent == 0)
    {
	cmod->cmod_save_सन्देशः_silent = सन्देशः_silent + 1;
	cmod->cmod_save_सन्देशः_scroll = सन्देशः_scroll;
    }
    जइ (cmod->cmod_flags & CMOD_SILENT)
	++सन्देशः_silent;
    जइ (cmod->cmod_flags & CMOD_UNSILENT)
	सन्देशः_silent = 0;

    जइ (cmod->cmod_flags & CMOD_ERRSILENT)
    {
	++emsg_silent;
	++cmod->cmod_did_esilent;
    }

    जइ ((cmod->cmod_flags & CMOD_NOAUTOCMD) && cmod->cmod_save_ei == रिक्त)
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
    जइ (cmod->cmod_verbose_save > 0)
    {
	p_verbose = cmod->cmod_verbose_save - 1;
	cmod->cmod_verbose_save = 0;
    }

#ifdef HAVE_SANDBOX
    जइ (cmod->cmod_did_sandbox)
    {
	--sandbox;
	cmod->cmod_did_sandbox = FALSE;
    }
#endif

    जइ (cmod->cmod_save_ei != रिक्त)
    {
	// Restore 'eventignore' to the value before ":noautocmd".
	set_string_option_direct((char_u *)"ei", -1, cmod->cmod_save_ei,
							   OPT_FREE, SID_NONE);
	free_string_option(cmod->cmod_save_ei);
	cmod->cmod_save_ei = रिक्त;
    }

    vim_regfree(cmod->cmod_filter_regmatch.regprog);

    जइ (cmod->cmod_save_सन्देशः_silent > 0)
    {
	// messages could be enabled for a serious error, need to check if the
	// counters don't become negative
	जइ (!did_emsg || सन्देशः_silent > cmod->cmod_save_सन्देशः_silent - 1)
	    सन्देशः_silent = cmod->cmod_save_सन्देशः_silent - 1;
	emsg_silent -= cmod->cmod_did_esilent;
	जइ (emsg_silent < 0)
	    emsg_silent = 0;
	// Restore msg_scroll, it's set by file I/O commands, even when no
	// message is actually displayed.
	सन्देशः_scroll = cmod->cmod_save_सन्देशः_scroll;

	// "silent reg" or "silent echo x" inside "redir" leaves msg_col
	// somewhere in the line.  Put it back in the first column.
	जइ (redirecting())
	    सन्देशः_col = 0;

	cmod->cmod_save_सन्देशः_silent = 0;
	cmod->cmod_did_esilent = 0;
    }
}

/*
 * Parse the address range, if any, in "eap".
 * May set the last search pattern, unless "silent" is TRUE.
 * Return FAIL and set "errormsg" or return OK.
 */
    पूर्णाङ्क
parse_cmd_address(exarg_T *eap, वर्ण **errormsg, पूर्णाङ्क silent)
{
    पूर्णाङ्क		address_गणनम् = 1;
    linenr_T	lnum;
    पूर्णाङ्क		need_check_cursor = FALSE;
    पूर्णाङ्क		ret = FAIL;

    // Repeat for all ',' or ';' separated addresses.
    प्रति (;;)
    {
	eap->line1 = eap->line2;
	eap->line2 = default_address(eap);
	eap->cmd = skipwhite(eap->cmd);
	lnum = get_address(eap, &eap->cmd, eap->addr_type, eap->skip, silent,
					eap->addr_गणनम् == 0, address_गणनम्++);
	जइ (eap->cmd == रिक्त)	// error detected
	    गच्छ theend;
	जइ (lnum == MAXLNUM)
	{
	    जइ (*eap->cmd == '%')   // '%' - all lines
	    {
		++eap->cmd;
		विकल्प (eap->addr_type)
		{
		    पक्ष ADDR_पङ्क्तयः:
		    पक्ष ADDR_OTHER:
			eap->line1 = 1;
			eap->line2 = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
			भङ्ग;
		    पक्ष ADDR_LOADED_BUFFERS:
			{
			    buf_T	*buf = firstbuf;

			    जाव (buf->b_next != रिक्त
						  && buf->b_ml.ml_mfp == रिक्त)
				buf = buf->b_next;
			    eap->line1 = buf->b_fnum;
			    buf = lastbuf;
			    जाव (buf->b_prev != रिक्त
						  && buf->b_ml.ml_mfp == रिक्त)
				buf = buf->b_prev;
			    eap->line2 = buf->b_fnum;
			    भङ्ग;
			}
		    पक्ष ADDR_BUFFERS:
			eap->line1 = firstbuf->b_fnum;
			eap->line2 = lastbuf->b_fnum;
			भङ्ग;
		    पक्ष ADDR_WINDOWS:
		    पक्ष ADDR_TABS:
			जइ (IS_USER_CMDIDX(eap->cmdidx))
			{
			    eap->line1 = 1;
			    eap->line2 = eap->addr_type == ADDR_WINDOWS
						  ? LAST_WIN_NR : LAST_TAB_NR;
			}
			अण्णहा
			{
			    // there is no Vim command which uses '%' and
			    // ADDR_WINDOWS or ADDR_TABS
			    *errormsg = _(e_invalid_range);
			    गच्छ theend;
			}
			भङ्ग;
		    पक्ष ADDR_TABS_RELATIVE:
		    पक्ष ADDR_UNSIGNED:
		    पक्ष ADDR_QUICKFIX:
			*errormsg = _(e_invalid_range);
			गच्छ theend;
		    पक्ष ADDR_ARGUMENTS:
			जइ (ARGCOUNT == 0)
			    eap->line1 = eap->line2 = 0;
			अण्णहा
			{
			    eap->line1 = 1;
			    eap->line2 = ARGCOUNT;
			}
			भङ्ग;
		    पक्ष ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			eap->line1 = 1;
			eap->line2 = qf_get_valid_size(eap);
			जइ (eap->line2 == 0)
			    eap->line2 = 1;
#endif
			भङ्ग;
		    पक्ष ADDR_NONE:
			// Will give an error later if a range is found.
			भङ्ग;
		}
		++eap->addr_गणनम्;
	    }
	    अण्णहा जइ (*eap->cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == रिक्त)
	    {
		pos_T	    *fp;

		// '*' - visual area
		जइ (eap->addr_type != ADDR_पङ्क्तयः)
		{
		    *errormsg = _(e_invalid_range);
		    गच्छ theend;
		}

		++eap->cmd;
		जइ (!eap->skip)
		{
		    fp = getmark('<', FALSE);
		    जइ (check_mark(fp) == FAIL)
			गच्छ theend;
		    eap->line1 = fp->lnum;
		    fp = getmark('>', FALSE);
		    जइ (check_mark(fp) == FAIL)
			गच्छ theend;
		    eap->line2 = fp->lnum;
		    ++eap->addr_गणनम्;
		}
	    }
	}
	अण्णहा
	    eap->line2 = lnum;
	eap->addr_गणनम्++;

	जइ (*eap->cmd == ';')
	{
	    जइ (!eap->skip)
	    {
		curwin->w_cursor.lnum = eap->line2;

		// Don't leave the cursor on an illegal line or column, but do
		// accept zero as address, so 0;/PATTERN/ works correctly
		// (where zero usually means to use the first line).
		// Check the cursor position before returning.
		जइ (eap->line2 > 0)
		    check_cursor();
		अण्णहा
		    check_cursor_col();
		need_check_cursor = TRUE;
	    }
	}
	अण्णहा जइ (*eap->cmd != ',')
	    भङ्ग;
	++eap->cmd;
    }

    // One address given: set start and end lines.
    जइ (eap->addr_गणनम् == 1)
    {
	eap->line1 = eap->line2;
	// ... but only implicit: really no address given
	जइ (lnum == MAXLNUM)
	    eap->addr_गणनम् = 0;
    }
    ret = OK;

theend:
    जइ (need_check_cursor)
	check_cursor();
    प्रत्यागम ret;
}

/*
 * Append "cmd" to the error message in IObuff.
 * Takes care of limiting the length and handling 0xa0, which would be
 * invisible otherwise.
 */
    स्थिर शून्य
append_command(char_u *cmd)
{
    size_t  len = STRLEN(IObuff);
    char_u  *s = cmd;
    char_u  *d;

    जइ (len > IOSIZE - 100)
    {
	// Not enough space, truncate and put in "...".
	d = IObuff + IOSIZE - 100;
	d -= mb_head_off(IObuff, d);
	STRCPY(d, "...");
    }
    STRCAT(IObuff, ": ");
    d = IObuff + STRLEN(IObuff);
    जाव (*s != NUL && d - IObuff + 5 < IOSIZE)
    {
	जइ (enc_utf8 ? (s[0] == 0xc2 && s[1] == 0xa0) : *s == 0xa0)
	{
	    s += enc_utf8 ? 2 : 1;
	    STRCPY(d, "<a0>");
	    d += 4;
	}
	अण्णहा जइ (d - IObuff + (*mb_ptr2len)(s) + 1 >= IOSIZE)
	    भङ्ग;
	अण्णहा
	    MB_COPY_CHAR(s, d);
    }
    *d = NUL;
}

#जइ defined(FEAT_EVAL)
/*
 * If "start" points "&opt", "&l:opt", "&g:opt" or "$ENV" return a pointer to
 * the name.  Otherwise just return "start".
 */
    char_u *
skip_option_env_lead(char_u *start)
{
    char_u *name = start;

    जइ (*start == '&')
    {
	जइ ((start[1] == 'l' || start[1] == 'g') && start[2] == ':')
	    name += 3;
	अण्णहा
	    name += 1;
    }
    अण्णहा जइ (*start == '$')
	name += 1;
    प्रत्यागम name;
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
    स्थिर पूर्णाङ्क
one_letter_cmd(char_u *p, cmdidx_T *idx)
{
    जइ (in_vim9script())
	प्रत्यागम FALSE;
    जइ (p[0] == 'k'
	    && (p[1] != 'e' || (p[1] == 'e' && p[2] != 'e')))
    {
	*idx = CMD_k;
	प्रत्यागम TRUE;
    }
    जइ (p[0] == 's'
	    && ((p[1] == 'c' && (p[2] == NUL || (p[2] != 's' && p[2] != 'r'
			&& (p[3] == NUL || (p[3] != 'i' && p[4] != 'p')))))
		|| p[1] == 'g'
		|| (p[1] == 'i' && p[2] != 'm' && p[2] != 'l' && p[2] != 'g')
		|| p[1] == 'I'
		|| (p[1] == 'r' && p[2] != 'e')))
    {
	*idx = CMD_substitute;
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

#जइ defined(FEAT_EVAL)
/*
 * Return TRUE if "cmd" starts with "123->", a number followed by a method
 * call.
 */
    पूर्णाङ्क
सङ्ख्या_method(char_u *cmd)
{
    char_u *p = skipdigits(cmd);

    प्रत्यागम p > cmd && (p = skipwhite(p))[0] == '-' && p[1] == '>';
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
	पूर्णाङ्क	*full UNUSED,
	पूर्णाङ्क	(*lookup)(char_u *, size_t, पूर्णाङ्क cmd, cctx_T *) UNUSED,
	cctx_T	*cctx UNUSED)
{
    पूर्णाङ्क		len;
    char_u	*p;
    पूर्णाङ्क		i;
#ifndef FEAT_EVAL
    पूर्णाङ्क		vim9 = FALSE;
#अण्णहा
    पूर्णाङ्क		vim9 = in_vim9script();

    /*
     * Recognize a Vim9 script function/method call and assignment:
     * "lvar = value", "lvar(arg)", "[1, 2 3]->Func()"
     */
    p = eap->cmd;
    जइ (lookup != रिक्त)
    {
	char_u *pskip = skip_option_env_lead(eap->cmd);

	जइ (vim_strchr((char_u *)"{('[\"@&$", *p) != रिक्त
	       || ((p = to_name_const_end(pskip)) > eap->cmd && *p != NUL)
	       || (p[0] == '0' && p[1] == 'z'))
	{
	    पूर्णाङ्क	    oplen;
	    पूर्णाङ्क	    heredoc;
	    char_u  *swp;

	    जइ (*eap->cmd == '&'
		    || (eap->cmd[0] == '$'
				  && eap->cmd[1] != '\'' && eap->cmd[1] != '"')
		    || (eap->cmd[0] == '@'
					&& (valid_yank_reg(eap->cmd[1], FALSE)
						       || eap->cmd[1] == '@')))
	    {
		जइ (*eap->cmd == '&')
		{
		    p = eap->cmd + 1;
		    जइ (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
			p += 2;
		    p = to_name_end(p, FALSE);
		}
		अण्णहा जइ (*eap->cmd == '$')
		    p = to_name_end(eap->cmd + 1, FALSE);
		अण्णहा
		    p = eap->cmd + 2;
		जइ (ends_excmd(*skipwhite(p)))
		{
		    // "&option <NL>", "$ENV <NL>" and "@r <NL>" are the start
		    // of an expression.
		    eap->cmdidx = CMD_eval;
		    प्रत्यागम eap->cmd;
		}
		// "&option" can be followed by "->" or "=", check below
	    }

	    जइ (vim9 && *p == '<')
	    {
		// generic function type args
		जइ (skip_generic_func_type_args(&p) == FAIL)
		{
		    eap->cmdidx = CMD_SIZE;
		    प्रत्यागम p;
		}
	    }

	    swp = skipwhite(p);

	    जइ (
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
		जइ (*eap->cmd == '{' && ends_excmd(*skipwhite(eap->cmd + 1)))
		{
		    // "{" by itself is the start of a block.
		    eap->cmdidx = CMD_block;
		    प्रत्यागम eap->cmd + 1;
		}
		eap->cmdidx = CMD_eval;
		प्रत्यागम eap->cmd;
	    }

	    जइ ((p != eap->cmd && (
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
		जइ (skip_व्यञ्जकम्(&after, रिक्त) == OK)
		{
		    after = skipwhite(after);
		    जइ (*after == '=' || (*after != NUL && after[1] == '=')
					 || (after[0] == '.' && after[1] == '.'
							   && after[2] == '='))
			eap->cmdidx = CMD_var;
		}
		--emsg_off;
		प्रत्यागम eap->cmd;
	    }

	    // "[...]->Method()" is a list expression, but "[a, b] = Func()" is
	    // an assignment.
	    // If there is no line break inside the "[...]" then "p" is
	    // advanced to after the "]" by to_name_const_end(): check if a "="
	    // follows.
	    // If "[...]" has a line break "p" still points at the "[" and it
	    // can't be an assignment.
	    जइ (*eap->cmd == '[')
	    {
		char_u	    *eq;

		p = to_name_const_end(eap->cmd);
		जइ (p == eap->cmd && *p == '[')
		{
		    पूर्णाङ्क गणनम् = 0;
		    पूर्णाङ्क	semicolon = FALSE;

		    p = skip_var_list(eap->cmd, TRUE, &गणनम्, &semicolon, TRUE);
		}
		eq = p;
		जइ (eq != रिक्त)
		{
		    eq = skipwhite(eq);
		    जइ (vim_strchr((char_u *)"+-*/%.", *eq) != रिक्त)
		    {
			जइ (eq[0] == '.' && eq[1] == '.')
			    ++eq;
			++eq;
		    }
		}
		जइ (p == रिक्त || p == eap->cmd || *eq != '=')
		{
		    eap->cmdidx = CMD_eval;
		    प्रत्यागम eap->cmd;
		}
		जइ (p > eap->cmd && *eq == '=')
		{
		    eap->cmdidx = CMD_var;
		    प्रत्यागम eap->cmd;
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
	    जइ (oplen > 0)
	    {
		जइ (((p - eap->cmd) > 2 && eap->cmd[1] == ':')
			|| *eap->cmd == '&'
			|| *eap->cmd == '$'
			|| *eap->cmd == '@'
			|| (eap->skip && IS_WHITE_OR_NUL(
						      *(skipwhite(p) + oplen)))
			|| lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK)
		{
		    eap->cmdidx = CMD_var;
		    प्रत्यागम eap->cmd;
		}
	    }

	    // Recognize trying to use a type for a w:, b:, t: or g: variable:
	    // "w:varname: number = 123".
	    जइ (eap->cmd[1] == ':' && *p == ':')
	    {
		eap->cmdidx = CMD_var;
		प्रत्यागम eap->cmd;
	    }
	}

	// 1234->func() is a method call
	जइ (सङ्ख्या_method(eap->cmd))
	{
	    eap->cmdidx = CMD_eval;
	    प्रत्यागम eap->cmd;
	}

	// "g:", "s:" and "l:" are always assumed to be a variable, thus start
	// an expression.  A global/substitute/list command needs to use a
	// longer name.
	जइ (vim_strchr((char_u *)"gsl", *p) != रिक्त && p[1] == ':')
	{
	    eap->cmdidx = CMD_eval;
	    प्रत्यागम eap->cmd;
	}

	// If it is an ID it might be a variable with an operator on the next
	// line, if the variable exists it can't be an Ex command.
	जइ (p > eap->cmd && ends_excmd(*skipwhite(p))
		&& (lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK
		    || (ASCII_ISALPHA(eap->cmd[0]) && eap->cmd[1] == ':')))
	{
	    eap->cmdidx = CMD_eval;
	    प्रत्यागम eap->cmd;
	}

	// Check for "++nr" and "--nr".
	जइ (p == eap->cmd && p[0] != NUL && p[0] == p[1]
						   && (*p == '+' || *p == '-'))
	{
	    eap->cmdidx = *p == '+' ? CMD_increment : CMD_decrement;
	    प्रत्यागम eap->cmd + 2;
	}
    }
#endif

    /*
     * Isolate the command and search for it in the command table.
     */
    p = eap->cmd;
    जइ (one_letter_cmd(p, &eap->cmdidx))
    {
	++p;
	जइ (full != रिक्त)
	    *full = TRUE;
    }
    अण्णहा
    {
	जाव (ASCII_ISALPHA(*p))
	    ++p;
	// for python 3.x support ":py3", ":python3", ":py3file", etc.
	जइ (eap->cmd[0] == 'p' && eap->cmd[1] == 'y')
	{
	    जाव (ASCII_ISALNUM(*p))
		++p;
	}
	अण्णहा जइ (*p == '9' && STRNCMP("vim9", eap->cmd, 4) == 0)
	{
	    // include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	    ++p;
	    जाव (ASCII_ISALPHA(*p))
		++p;
	}

	// check for non-alpha command
	जइ (p == eap->cmd && vim_strchr((char_u *)"@*!=><&~#}", *p) != रिक्त)
	    ++p;
	len = (पूर्णाङ्क)(p - eap->cmd);
	// The "d" command can directly be followed by 'l' or 'p' flag, when
	// not in Vim9 script.
	जइ (!vim9 && *eap->cmd == 'd' && (p[-1] == 'l' || p[-1] == 'p'))
	{
	    // Check for ":dl", ":dell", etc. to ":deletel": that's
	    // :delete with the 'l' flag.  Same for 'p'.
	    प्रति (i = 0; i < len; ++i)
		जइ (eap->cmd[i] != ((char_u *)"delete")[i])
		    भङ्ग;
	    जइ (i == len - 1)
	    {
		--len;
		जइ (p[-1] == 'l')
		    eap->flags |= EXFLAG_LIST;
		अण्णहा
		    eap->flags |= EXFLAG_PRINT;
	    }
	}

	जइ (ASCII_ISLOWER(eap->cmd[0]))
	{
	    पूर्णाङ्क c1 = eap->cmd[0];
	    पूर्णाङ्क c2 = len == 1 ? NUL : eap->cmd[1];

	    जइ (command_गणनम् != (पूर्णाङ्क)CMD_SIZE)
	    {
		iemsg(e_command_table_needs_to_be_updated_run_make_cmdidxs);
		getout(1);
	    }

	    // Use a precomputed index for fast look-up in cmdnames[]
	    // taking into account the first 2 letters of eap->cmd.
	    eap->cmdidx = cmdidxs1[Char_Ord_नीचः(c1)];
	    जइ (ASCII_ISLOWER(c2))
		eap->cmdidx += cmdidxs2[Char_Ord_नीचः(c1)][Char_Ord_नीचः(c2)];
	}
	अण्णहा जइ (ASCII_ISUPPER(eap->cmd[0]))
	    eap->cmdidx = CMD_Next;
	अण्णहा
	    eap->cmdidx = CMD_bang;

	प्रति ( ; (पूर्णाङ्क)eap->cmdidx < (पूर्णाङ्क)CMD_SIZE;
			       eap->cmdidx = (cmdidx_T)((पूर्णाङ्क)eap->cmdidx + 1))
	    जइ (STRNCMP(cmdnames[(पूर्णाङ्क)eap->cmdidx].cmd_name, (वर्ण *)eap->cmd,
							    (size_t)len) == 0)
	    {
#ifdef FEAT_EVAL
		जइ (full != रिक्त && cmdnames[eap->cmdidx].cmd_name[len] == NUL)
		    *full = TRUE;
#endif
		भङ्ग;
	    }

	// :Print and :mode are not supported in Vim9 script.
	// Some commands cannot be shortened in Vim9 script.
	जइ (vim9 && eap->cmdidx != CMD_SIZE)
	{
	    जइ (eap->cmdidx == CMD_mode || eap->cmdidx == CMD_Print)
		eap->cmdidx = CMD_SIZE;
	    अण्णहा जइ ((cmdnames[eap->cmdidx].cmd_argt & EX_WHOLE)
			  && len < (पूर्णाङ्क)cmdnames[eap->cmdidx].cmd_namelen)
	    {
		semsg(_(e_command_cannot_be_shortened_str), eap->cmd);
		eap->cmdidx = CMD_SIZE;
	    }
	}

	// Do not recognize ":*" as the star command unless '*' is in
	// 'cpoptions'.
	जइ (eap->cmdidx == CMD_star && vim_strchr(p_cpo, CPO_STAR) == रिक्त)
	    p = eap->cmd;

	// Look for a user defined command as a last resort.  Let ":Print" be
	// overruled by a user defined command.
	जइ ((eap->cmdidx == CMD_SIZE || eap->cmdidx == CMD_Print)
		&& *eap->cmd >= 'A' && *eap->cmd <= 'Z')
	{
	    // User defined commands may contain digits.
	    जाव (ASCII_ISALNUM(*p))
		++p;
	    p = find_ucmd(eap, p, full, रिक्त, रिक्त);
	}
	जइ (p == रिक्त || p == eap->cmd)
	    eap->cmdidx = CMD_SIZE;
    }

    // ":fina" means ":finally" in legacy script, for backwards compatibility.
    जइ (eap->cmdidx == CMD_final && p - eap->cmd == 4 && !vim9)
	eap->cmdidx = CMD_finally;

     // Force ":ho" to be unresolved.  Without this, find_ex_command()
     // matches it to CMD_horizontal (the only "ho*" entry), which makes
     // fullcommand("ho") return "horizontal" even though ":ho" cannot be
     // used as the modifier (cmdmods[] requires 3 chars, "hor").
    जइ (eap->cmdidx == CMD_horizontal && p - eap->cmd == 2)
	eap->cmdidx = CMD_SIZE;

#ifdef FEAT_EVAL
    जइ (eap->cmdidx < CMD_SIZE
	    && vim9
	    && !IS_WHITE_NL_OR_NUL(*p) && *p != '!' && *p != '|'
	    && (eap->cmdidx < 0 ||
		(cmdnames[eap->cmdidx].cmd_argt & EX_NONWHITE_OK) == 0))
    {
	// A name that goes on with an underscore is not this command with an
	// argument: "ch_log" is not ":change".  Leave it to be reported as an
	// invalid command.
	जइ (*p != '_')
	{
	    char_u *cmd = vim_strnsave(eap->cmd, p - eap->cmd);

	    semsg(_(e_command_str_not_followed_by_white_space_str), cmd,
								    eap->cmd);
	    vim_free(cmd);
	}
	eap->cmdidx = CMD_SIZE;
    }
#endif

    प्रत्यागम p;
}

#जइ defined(FEAT_EVAL)
प्रकारनिर्देश संरचना
{
    वर्ण	*name;
    पूर्णाङ्क		minlen;
    पूर्णाङ्क		has_गणनम्;  // :123verbose  :3tab
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
    पूर्णाङ्क
modifier_len(char_u *cmd)
{
    पूर्णाङ्क		i, j;
    char_u	*p = cmd;

    जइ (VIM_ISDIGIT(*cmd))
	p = skipwhite(skipdigits(cmd + 1));
    प्रति (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	// cmdmod_info_tab[] is sorted by name: once the first letter is past
	// the command's first letter no later entry can match.
	जइ (cmdmod_info_tab[i].name[0] > *p)
	    भङ्ग;
	प्रति (j = 0; p[j] != NUL; ++j)
	    जइ (p[j] != cmdmod_info_tab[i].name[j])
		भङ्ग;
	जइ (!ASCII_ISALPHA(p[j]) && j >= cmdmod_info_tab[i].minlen
					&& (p == cmd || cmdmod_info_tab[i].has_गणनम्))
	    प्रत्यागम j + (पूर्णाङ्क)(p - cmd);
    }
    प्रत्यागम 0;
}

/*
 * Return > 0 if an Ex command "name" exists.
 * Return 2 if there is an exact match.
 * Return 3 if there is an ambiguous match.
 */
    पूर्णाङ्क
cmd_exists(char_u *name)
{
    exarg_T	ea;
    पूर्णाङ्क		full = FALSE;
    पूर्णाङ्क		i;
    पूर्णाङ्क		j;
    char_u	*p;

    // Check command modifiers.
    प्रति (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	प्रति (j = 0; name[j] != NUL; ++j)
	    जइ (name[j] != cmdmod_info_tab[i].name[j])
		भङ्ग;
	जइ (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	    प्रत्यागम (cmdmod_info_tab[i].name[j] == NUL ? 2 : 1);
    }

    // Check built-in commands and user defined commands.
    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    p = find_ex_command(&ea, &full, रिक्त, रिक्त);
    जइ (p == रिक्त)
	प्रत्यागम 3;
    जइ (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	प्रत्यागम 0;
    जइ (*skipwhite(p) != NUL)
	प्रत्यागम 0;	// trailing garbage
    प्रत्यागम (ea.cmdidx == CMD_SIZE ? 0 : (full ? 2 : 1));
}

/*
 * Add the attributes of an Ex command to "d" in the terms of ":command":
 * "argt" are the EX_ flags, "addr_type" the address type and "def" the
 * default for a range or count, -1 when there is none.
 */
    शून्य
ex_command_attrs(dict_T *d, long_u argt, cmd_addr_T addr_type, long def)
{
    स्थिर वर्ण *addr_names[] = {
	"lines", "windows", "arguments", "loaded_buffers", "buffers", "tabs",
	"tabs_relative", "quickfix_valid", "quickfix", "unsigned", "other",
	"none"
    };
    वर्ण	*nargs;

    जइ (!(argt & EX_EXTRA))
	nargs = "0";
    अण्णहा जइ (!(argt & EX_NOSPC))
	nargs = (argt & EX_NEEDARG) ? "+" : "*";
    अण्णहा जइ (!(argt & EX_NEEDARG))
	nargs = "?";
    अण्णहा
	nargs = (argt & EX_ARGSPACE) ? "_" : "1";
    dict_add_string(d, "nargs", (char_u *)nargs);

    जइ (!(argt & EX_RANGE))
	dict_add_bool(d, "range", FALSE);
    अण्णहा जइ (argt & EX_DFLALL)
	dict_add_string(d, "range", (char_u *)"%");
    अण्णहा जइ (def >= 0 && !(argt & EX_गणनम्))
	dict_add_सङ्ख्या(d, "range", def);
    अण्णहा
	dict_add_string(d, "range", (char_u *)".");
    जइ (!(argt & EX_गणनम्))
	dict_add_bool(d, "count", FALSE);
    अण्णहा
	dict_add_सङ्ख्या(d, "count", def < 0 ? 0 : def);
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
    पूर्णाङ्क
ex_command_info(char_u *name, पूर्णाङ्क vim9, dict_T *d)
{
    exarg_T	ea;
    char_u	*p;
    पूर्णाङ्क		i;
    पूर्णाङ्क		j;
    पूर्णाङ्क		save_cmod_flags = cmdmod.cmod_flags;
    पूर्णाङ्क		ret = OK;

    जइ (vim9 != -1)
    {
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9 ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    // An abbreviation of a command modifier, like ":bo" for ":botright".
    प्रति (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	प्रति (j = 0; name[j] != NUL; ++j)
	    जइ (name[j] != cmdmod_info_tab[i].name[j])
		भङ्ग;
	जइ (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	{
	    name = (char_u *)cmdmod_info_tab[i].name;
	    भङ्ग;
	}
    }

    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    ea.addr_गणनम् = 0;
    // Don't complain about using "en" or ":let" in Vim9 script.
    ++emsg_silent;
    p = find_ex_command(&ea, रिक्त, रिक्त, रिक्त);
    जइ (p == रिक्त || ea.cmdidx == CMD_SIZE
	    || (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	    || *skipwhite(p) != NUL
	    || not_in_vim9(&ea, true) == FAIL)
	ret = FAIL;
    --emsg_silent;

    जइ (ret == FAIL)
	;
    अण्णहा जइ (IS_USER_CMDIDX(ea.cmdidx))
	ret = user_command_info(ea.useridx, ea.cmdidx, d);
    अण्णहा
    {
	पूर्णाङ्क	notimpl = cmdnames[ea.cmdidx].cmd_func == ex_ni
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
    प्रत्यागम ret;
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
    पूर्णाङ्क		vim9script = in_vim9script();
    पूर्णाङ्क		save_cmod_flags = cmdmod.cmod_flags;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    जइ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    name = tv_get_string(&argvars[0]);
    जइ (name == रिक्त)
	प्रत्यागम;

    जइ (argvars[1].v_type != VAR_UNKNOWN)
    {
	vim9script = tv_get_bool(&argvars[1]);
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9script ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    जाव (*name == ':')
	name++;
    name = skip_range(name, TRUE, रिक्त);

    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.addr_गणनम् = 0;
    ++emsg_silent;  // don't complain about using "en" in Vim9 script
    p = find_ex_command(&ea, रिक्त, रिक्त, रिक्त);
    --emsg_silent;
    जइ (p == रिक्त || ea.cmdidx == CMD_SIZE)
	गच्छ theend;

    जइ (vim9script && not_in_vim9(&ea, true) == FAIL)
	गच्छ theend;

    rettv->vval.v_string = vim_strsave(IS_USER_CMDIDX(ea.cmdidx)
				 ? get_user_command_name(ea.useridx, ea.cmdidx)
				 : cmdnames[ea.cmdidx].cmd_name);
theend:
    cmdmod.cmod_flags = save_cmod_flags;
}
#endif

    cmdidx_T
excmd_get_cmdidx(char_u *cmd, पूर्णाङ्क len)
{
    cmdidx_T idx;

    जइ (!one_letter_cmd(cmd, &idx))
	प्रति (idx = (cmdidx_T)0; (पूर्णाङ्क)idx < (पूर्णाङ्क)CMD_SIZE;
		idx = (cmdidx_T)((पूर्णाङ्क)idx + 1))
	    जइ (STRNCMP(cmdnames[(पूर्णाङ्क)idx].cmd_name, cmd, (size_t)len) == 0)
		भङ्ग;

    प्रत्यागम idx;
}

    long
excmd_get_argt(cmdidx_T idx)
{
    प्रत्यागम (long)cmdnames[(पूर्णाङ्क)idx].cmd_argt;
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
    पूर्णाङ्क		skip_star,	// skip "*" used for Visual range
    पूर्णाङ्क		*ctx)		// pointer to xp_context or NULL
{
    char_u	*cmd = cmd_start;
    unsigned	delim;

    जाव (vim_strchr((char_u *)" \t0123456789.$%'/?-+,;\\", *cmd) != रिक्त)
    {
	जइ (*cmd == '\\')
	{
	    जइ (cmd[1] == '?' || cmd[1] == '/' || cmd[1] == '&')
		++cmd;
	    अण्णहा
		भङ्ग;
	}
	अण्णहा जइ (*cmd == '\'')
	{
	    char_u *p = cmd;

	    // a quote is only valid at the start or after a separator
	    जाव (p > cmd_start)
	    {
		--p;
		जइ (!VIM_ISWHITE(*p))
		    भङ्ग;
	    }
	    जइ (cmd > cmd_start && !VIM_ISWHITE(*p) && *p != ',' && *p != ';')
		भङ्ग;
	    जइ (*++cmd == NUL && ctx != रिक्त)
		*ctx = EXPAND_NOTHING;
	}
	अण्णहा जइ (*cmd == '/' || *cmd == '?')
	{
	    delim = *cmd++;
	    जाव (*cmd != NUL && *cmd != delim)
		जइ (*cmd++ == '\\' && *cmd != NUL)
		    ++cmd;
	    जइ (*cmd == NUL && ctx != रिक्त)
		*ctx = EXPAND_NOTHING;
	}
	जइ (*cmd != NUL)
	    ++cmd;
    }

    // Skip ":" and white space.
    जाव (*cmd == ':')
	cmd = skipwhite(cmd + 1);

    // Skip "*" used for Visual range.
    जइ (skip_star && *cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == रिक्त)
	cmd = skipwhite(cmd + 1);

    प्रत्यागम cmd;
}

    स्थिर शून्य
addr_error(cmd_addr_T addr_type)
{
    जइ (addr_type == ADDR_NONE)
	emsg(_(e_no_range_allowed));
    अण्णहा
	emsg(_(e_invalid_range));
}

/*
 * Return the default address for an address type.
 */
    स्थिर linenr_T
default_address(exarg_T *eap)
{
    linenr_T lnum = 0;

    विकल्प (eap->addr_type)
    {
	पक्ष ADDR_पङ्क्तयः:
	पक्ष ADDR_OTHER:
	    // Default is the cursor line number.  Avoid using an invalid
	    // line number though.
	    जइ (curwin->w_cursor.lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
		lnum = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
	    अण्णहा
		lnum = curwin->w_cursor.lnum;
	    भङ्ग;
	पक्ष ADDR_WINDOWS:
	    lnum = CURRENT_WIN_NR;
	    भङ्ग;
	पक्ष ADDR_ARGUMENTS:
	    lnum = curwin->w_arg_idx + 1;
	    जइ (lnum > ARGCOUNT)
		lnum = ARGCOUNT;
	    भङ्ग;
	पक्ष ADDR_LOADED_BUFFERS:
	पक्ष ADDR_BUFFERS:
	    lnum = curbuf->b_fnum;
	    भङ्ग;
	पक्ष ADDR_TABS:
	    lnum = CURRENT_TAB_NR;
	    भङ्ग;
	पक्ष ADDR_TABS_RELATIVE:
	पक्ष ADDR_UNSIGNED:
	    lnum = 1;
	    भङ्ग;
	पक्ष ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_idx(eap);
#endif
	    भङ्ग;
	पक्ष ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_valid_idx(eap);
#endif
	    भङ्ग;
	पक्ष ADDR_NONE:
	    // Will give an error later if a range is found.
	    भङ्ग;
    }
    प्रत्यागम lnum;
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
    पूर्णाङ्क		skip,		// only skip the address, don't use it
    पूर्णाङ्क		silent,		// no errors or side effects
    पूर्णाङ्क		to_other_file,  // flag: may jump to other file
    पूर्णाङ्क		address_गणनम् UNUSED) // 1 for first address, >1 after comma
{
    पूर्णाङ्क		c;
    पूर्णाङ्क		i;
    long	n;
    char_u	*cmd;
    pos_T	pos;
    pos_T	*fp;
    linenr_T	lnum;
    buf_T	*buf;

    cmd = skipwhite(*ptr);
    lnum = MAXLNUM;
    करेहि
    {
	विकल्प (*cmd)
	{
	    पक्ष '.':			    // '.' - Cursor position
		++cmd;
		विकल्प (addr_type)
		{
		    पक्ष ADDR_पङ्क्तयः:
		    पक्ष ADDR_OTHER:
			lnum = curwin->w_cursor.lnum;
			भङ्ग;
		    पक्ष ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			भङ्ग;
		    पक्ष ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			भङ्ग;
		    पक्ष ADDR_LOADED_BUFFERS:
		    पक्ष ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			भङ्ग;
		    पक्ष ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			भङ्ग;
		    पक्ष ADDR_NONE:
		    पक्ष ADDR_TABS_RELATIVE:
		    पक्ष ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = रिक्त;
			गच्छ error;
			भङ्ग;
		    पक्ष ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			भङ्ग;
		    पक्ष ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			भङ्ग;
		}
		भङ्ग;

	    पक्ष '$':			    // '$' - last line
		++cmd;
		विकल्प (addr_type)
		{
		    पक्ष ADDR_पङ्क्तयः:
		    पक्ष ADDR_OTHER:
			lnum = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
			भङ्ग;
		    पक्ष ADDR_WINDOWS:
			lnum = LAST_WIN_NR;
			भङ्ग;
		    पक्ष ADDR_ARGUMENTS:
			lnum = ARGCOUNT;
			भङ्ग;
		    पक्ष ADDR_LOADED_BUFFERS:
			buf = lastbuf;
			जाव (buf->b_ml.ml_mfp == रिक्त)
			{
			    जइ (buf->b_prev == रिक्त)
				भङ्ग;
			    buf = buf->b_prev;
			}
			lnum = buf->b_fnum;
			भङ्ग;
		    पक्ष ADDR_BUFFERS:
			lnum = lastbuf->b_fnum;
			भङ्ग;
		    पक्ष ADDR_TABS:
			lnum = LAST_TAB_NR;
			भङ्ग;
		    पक्ष ADDR_NONE:
		    पक्ष ADDR_TABS_RELATIVE:
		    पक्ष ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = रिक्त;
			गच्छ error;
			भङ्ग;
		    पक्ष ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_size(eap);
			जइ (lnum == 0)
			    lnum = 1;
#endif
			भङ्ग;
		    पक्ष ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_valid_size(eap);
			जइ (lnum == 0)
			    lnum = 1;
#endif
			भङ्ग;
		}
		भङ्ग;

	    पक्ष '\'':			    // ''' - mark
		जइ (*++cmd == NUL)
		{
		    cmd = रिक्त;
		    गच्छ error;
		}
		जइ (addr_type != ADDR_पङ्क्तयः)
		{
		    addr_error(addr_type);
		    cmd = रिक्त;
		    गच्छ error;
		}
		जइ (skip)
		    ++cmd;
		अण्णहा
		{
		    // Only accept a mark in another file when it is
		    // used by itself: ":'M".
		    fp = getmark(*cmd, to_other_file && cmd[1] == NUL);
		    ++cmd;
		    जइ (fp == (pos_T *)-1)
			// Jumped to another file.
			lnum = curwin->w_cursor.lnum;
		    अण्णहा
		    {
			जइ (check_mark(fp) == FAIL)
			{
			    cmd = रिक्त;
			    गच्छ error;
			}
			lnum = fp->lnum;
		    }
		}
		भङ्ग;

	    पक्ष '/':
	    पक्ष '?':			// '/' or '?' - search
		c = *cmd++;
		जइ (addr_type != ADDR_पङ्क्तयः)
		{
		    addr_error(addr_type);
		    cmd = रिक्त;
		    गच्छ error;
		}
		जइ (skip)	// skip "/pat/"
		{
		    cmd = skip_regexp(cmd, c, magic_isset());
		    जइ (*cmd == c)
			++cmd;
		}
		अण्णहा
		{
		    पूर्णाङ्क flags;

		    pos = curwin->w_cursor; // save curwin->w_cursor

		    // When '/' or '?' follows another address, start from
		    // there.
		    जइ (lnum > 0 && lnum != MAXLNUM)
			curwin->w_cursor.lnum =
				lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्
					   ? curbuf->b_ml.ml_पङ्क्तिः_गणनम् : lnum;

		    // Start a forward search at the end of the line (unless
		    // before the first line).
		    // Start a backward search at the start of the line.
		    // This makes sure we never match in the current
		    // line, and can match anywhere in the
		    // next/previous line.
		    जइ (c == '/' && curwin->w_cursor.lnum > 0)
			curwin->w_cursor.col = MAXCOL;
		    अण्णहा
			curwin->w_cursor.col = 0;
		    searchcmdlen = 0;
		    flags = silent ? अन्वेषणम्_KEEP : अन्वेषणम्_HIS | अन्वेषणम्_सन्देशः;
		    जइ (!do_अन्वेषणम्(रिक्त, c, c, cmd, STRLEN(cmd), 1L, flags, रिक्त))
		    {
			curwin->w_cursor = pos;
			cmd = रिक्त;
			गच्छ error;
		    }
		    lnum = curwin->w_cursor.lnum;
		    curwin->w_cursor = pos;
		    // adjust command string pointer
		    cmd += searchcmdlen;
		}
		भङ्ग;

	    पक्ष '\\':		    // "\?", "\/" or "\&", repeat search
		++cmd;
		जइ (addr_type != ADDR_पङ्क्तयः)
		{
		    addr_error(addr_type);
		    cmd = रिक्त;
		    गच्छ error;
		}
		जइ (*cmd == '&')
		    i = RE_SUBST;
		अण्णहा जइ (*cmd == '?' || *cmd == '/')
		    i = RE_अन्वेषणम्;
		अण्णहा
		{
		    emsg(_(e_backslash_should_be_followed_by));
		    cmd = रिक्त;
		    गच्छ error;
		}

		जइ (!skip)
		{
		    /*
		     * When search follows another address, start from
		     * there.
		     */
		    जइ (lnum != MAXLNUM)
			pos.lnum = lnum;
		    अण्णहा
			pos.lnum = curwin->w_cursor.lnum;

		    /*
		     * Start the search just like for the above
		     * do_search().
		     */
		    जइ (*cmd != '?')
			pos.col = MAXCOL;
		    अण्णहा
			pos.col = 0;
		    pos.coladd = 0;
		    जइ (searchit(curwin, curbuf, &pos, रिक्त,
				*cmd == '?' ? BACKWARD : FORWARD,
				(char_u *)"", 0, 1L, अन्वेषणम्_सन्देशः, i, रिक्त) != FAIL)
			lnum = pos.lnum;
		    अण्णहा
		    {
			cmd = रिक्त;
			गच्छ error;
		    }
		}
		++cmd;
		भङ्ग;

	    स्वभाव:
		जइ (VIM_ISDIGIT(*cmd))	// absolute line number
		    lnum = getdigits(&cmd);
	}

	प्रति (;;)
	{
	    cmd = skipwhite(cmd);
	    जइ (*cmd != '-' && *cmd != '+' && !VIM_ISDIGIT(*cmd))
		भङ्ग;

	    जइ (lnum == MAXLNUM)
	    {
		विकल्प (addr_type)
		{
		    पक्ष ADDR_पङ्क्तयः:
		    पक्ष ADDR_OTHER:
			// "+1" is same as ".+1"
			lnum = curwin->w_cursor.lnum;
			भङ्ग;
		    पक्ष ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			भङ्ग;
		    पक्ष ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			भङ्ग;
		    पक्ष ADDR_LOADED_BUFFERS:
		    पक्ष ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			भङ्ग;
		    पक्ष ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			भङ्ग;
		    पक्ष ADDR_TABS_RELATIVE:
			lnum = 1;
			भङ्ग;
		    पक्ष ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			भङ्ग;
		    पक्ष ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			भङ्ग;
		    पक्ष ADDR_NONE:
		    पक्ष ADDR_UNSIGNED:
			lnum = 0;
			भङ्ग;
		}
	    }

	    जइ (VIM_ISDIGIT(*cmd))
		i = '+';		// "number" is same as "+number"
	    अण्णहा
		i = *cmd++;
	    जइ (!VIM_ISDIGIT(*cmd))	// '+' is '+1'
		n = 1;
	    अण्णहा
	    {
		// "number", "+number" or "-number"
		n = getdigits(&cmd);
		जइ (n == MAXLNUM)
		{
		    emsg(_(e_पङ्क्तिः_सङ्ख्या_out_of_range));
		    cmd = रिक्त;
		    गच्छ error;
		}
	    }

	    जइ (addr_type == ADDR_TABS_RELATIVE)
	    {
		emsg(_(e_invalid_range));
		cmd = रिक्त;
		गच्छ error;
	    }
	    अण्णहा जइ (addr_type == ADDR_LOADED_BUFFERS
		    || addr_type == ADDR_BUFFERS)
		lnum = compute_buffer_local_गणनम्(
				    addr_type, lnum, (i == '-') ? -1 * n : n);
	    अण्णहा
	    {
#ifdef FEAT_FOLDING
		// Relative line addressing: need to adjust for lines in a
		// closed fold after the first address.
		जइ (addr_type == ADDR_पङ्क्तयः && (i == '-' || i == '+')
							 && address_गणनम् >= 2)
		    (शून्य)hasFolding(lnum, रिक्त, &lnum);
#endif
		जइ (i == '-')
		    lnum -= n;
		अण्णहा
		{
		    जइ (lnum >= 0 && n >= LONG_MAX - lnum)
		    {
			emsg(_(e_पङ्क्तिः_सङ्ख्या_out_of_range));
			cmd = रिक्त;
			गच्छ error;
		    }
		    lnum += n;
		}
	    }
	}
    } जाव (*cmd == '/' || *cmd == '?');

error:
    *ptr = cmd;
    प्रत्यागम lnum;
}

/*
 * Set eap->line1 and eap->line2 to the whole range.
 * Used for commands with the EX_DFLALL flag and no range given.
 */
    स्थिर शून्य
address_default_all(exarg_T *eap)
{
    eap->line1 = 1;
    विकल्प (eap->addr_type)
    {
	पक्ष ADDR_पङ्क्तयः:
	पक्ष ADDR_OTHER:
	    eap->line2 = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
	    भङ्ग;
	पक्ष ADDR_LOADED_BUFFERS:
	    {
		buf_T *buf = firstbuf;

		जाव (buf->b_next != रिक्त && buf->b_ml.ml_mfp == रिक्त)
		    buf = buf->b_next;
		eap->line1 = buf->b_fnum;
		buf = lastbuf;
		जाव (buf->b_prev != रिक्त && buf->b_ml.ml_mfp == रिक्त)
		    buf = buf->b_prev;
		eap->line2 = buf->b_fnum;
	    }
	    भङ्ग;
	पक्ष ADDR_BUFFERS:
	    eap->line1 = firstbuf->b_fnum;
	    eap->line2 = lastbuf->b_fnum;
	    भङ्ग;
	पक्ष ADDR_WINDOWS:
	    eap->line2 = LAST_WIN_NR;
	    भङ्ग;
	पक्ष ADDR_TABS:
	    eap->line2 = LAST_TAB_NR;
	    भङ्ग;
	पक्ष ADDR_TABS_RELATIVE:
	    eap->line2 = 1;
	    भङ्ग;
	पक्ष ADDR_ARGUMENTS:
	    जइ (ARGCOUNT == 0)
		eap->line1 = eap->line2 = 0;
	    अण्णहा
		eap->line2 = ARGCOUNT;
	    भङ्ग;
	पक्ष ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    eap->line2 = qf_get_valid_size(eap);
	    जइ (eap->line2 == 0)
		eap->line2 = 1;
#endif
	    भङ्ग;
	पक्ष ADDR_NONE:
	पक्ष ADDR_UNSIGNED:
	पक्ष ADDR_QUICKFIX:
	    iemsg("Cannot use EX_DFLALL with ADDR_NONE, ADDR_UNSIGNED or ADDR_QUICKFIX");
	    भङ्ग;
    }
}


/*
 * Get flags from an Ex command argument.
 */
    स्थिर शून्य
get_flags(exarg_T *eap)
{
    जाव (vim_strchr((char_u *)"lp#", *eap->arg) != रिक्त)
    {
	जइ (*eap->arg == 'l')
	    eap->flags |= EXFLAG_LIST;
	अण्णहा जइ (*eap->arg == 'p')
	    eap->flags |= EXFLAG_PRINT;
	अण्णहा
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
    जइ (!eap->skip)
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
    जइ (!eap->skip)
	ex_ni(eap);
    अण्णहा
	vim_free(script_get(eap, eap->arg));
}
#endif

/*
 * Check range in Ex command for validity.
 * Return NULL when valid, error message when invalid.
 */
    स्थिर वर्ण *
invalid_range(exarg_T *eap)
{
    buf_T	*buf;

    जइ (       eap->line1 < 0
	    || eap->line2 < 0
	    || eap->line1 > eap->line2)
	प्रत्यागम _(e_invalid_range);

    जइ (eap->argt & EX_RANGE)
    {
	विकल्प (eap->addr_type)
	{
	    पक्ष ADDR_पङ्क्तयः:
		जइ (eap->line2 > curbuf->b_ml.ml_पङ्क्तिः_गणनम्
#ifdef FEAT_DIFF
			    + (eap->cmdidx == CMD_diffget ||
				eap->cmdidx == CMD_diffput)
#endif
		   )
		    प्रत्यागम _(e_invalid_range);
		भङ्ग;
	    पक्ष ADDR_ARGUMENTS:
		// add 1 if ARGCOUNT is 0
		जइ (eap->line2 > ARGCOUNT + (!ARGCOUNT))
		    प्रत्यागम _(e_invalid_range);
		भङ्ग;
	    पक्ष ADDR_BUFFERS:
		// Only a boundary check, not whether the buffers actually
		// exist.
		जइ (eap->line1 < 1 || eap->line2 > get_highest_fnum())
		    प्रत्यागम _(e_invalid_range);
		भङ्ग;
	    पक्ष ADDR_LOADED_BUFFERS:
		buf = firstbuf;
		जाव (buf->b_ml.ml_mfp == रिक्त)
		{
		    जइ (buf->b_next == रिक्त)
			प्रत्यागम _(e_invalid_range);
		    buf = buf->b_next;
		}
		जइ (eap->line1 < buf->b_fnum)
		    प्रत्यागम _(e_invalid_range);
		buf = lastbuf;
		जाव (buf->b_ml.ml_mfp == रिक्त)
		{
		    जइ (buf->b_prev == रिक्त)
			प्रत्यागम _(e_invalid_range);
		    buf = buf->b_prev;
		}
		जइ (eap->line2 > buf->b_fnum)
		    प्रत्यागम _(e_invalid_range);
		भङ्ग;
	    पक्ष ADDR_WINDOWS:
		जइ (eap->line2 > LAST_WIN_NR)
		    प्रत्यागम _(e_invalid_range);
		भङ्ग;
	    पक्ष ADDR_TABS:
		जइ (eap->line2 > LAST_TAB_NR)
		    प्रत्यागम _(e_invalid_range);
		भङ्ग;
	    पक्ष ADDR_TABS_RELATIVE:
	    पक्ष ADDR_OTHER:
		// Any range is OK.
		भङ्ग;
	    पक्ष ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
		// No error for value that is too big, will use the last entry.
		जइ (eap->line2 <= 0)
		{
		    जइ (eap->addr_गणनम् == 0)
			प्रत्यागम _(e_no_errors);
		    प्रत्यागम _(e_invalid_range);
		}
#endif
		भङ्ग;
	    पक्ष ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
		जइ ((eap->line2 != 1 && eap->line2 > qf_get_valid_size(eap))
			|| eap->line2 < 0)
		    प्रत्यागम _(e_invalid_range);
#endif
		भङ्ग;
	    पक्ष ADDR_UNSIGNED:
	    पक्ष ADDR_NONE:
		// Will give an error elsewhere.
		भङ्ग;
	}
    }
    प्रत्यागम रिक्त;
}

/*
 * Correct the range for zero line number, if required.
 */
    स्थिर शून्य
correct_range(exarg_T *eap)
{
    जइ (!(eap->argt & EX_ZEROR))	    // zero in range not allowed
    {
	जइ (eap->line1 == 0)
	    eap->line1 = 1;
	जइ (eap->line2 == 0)
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

    जइ (*p != NUL && (eap->cmdidx == CMD_vimgrep || eap->cmdidx == CMD_lvimgrep
		|| eap->cmdidx == CMD_vimgrepadd
		|| eap->cmdidx == CMD_lvimgrepadd
		|| grep_internal(eap->cmdidx)))
    {
	p = skip_vimgrep_pat(p, रिक्त, रिक्त);
	जइ (p == रिक्त)
	    p = eap->arg;
    }
    प्रत्यागम p;
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
    पूर्णाङ्क		len;
    पूर्णाङ्क		i;

    /*
     * Don't do it when ":vimgrep" is used for ":grep".
     */
    जइ ((eap->cmdidx == CMD_make || eap->cmdidx == CMD_lmake
		     || eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
		     || eap->cmdidx == CMD_grepadd
		     || eap->cmdidx == CMD_lgrepadd)
	    && !grep_internal(eap->cmdidx))
    {
	जइ (eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
	    || eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	{
	    जइ (*curbuf->b_p_gp == NUL)
		program = p_gp;
	    अण्णहा
		program = curbuf->b_p_gp;
	}
	अण्णहा
	{
	    जइ (*curbuf->b_p_mp == NUL)
		program = p_mp;
	    अण्णहा
		program = curbuf->b_p_mp;
	}

	p = skipwhite(p);

	जइ ((pos = (char_u *)strstr((वर्ण *)program, "$*")) != रिक्त)
	{
	    // replace $* by given arguments
	    i = 1;
	    जाव ((pos = (char_u *)strstr((वर्ण *)pos + 2, "$*")) != रिक्त)
		++i;
	    len = (पूर्णाङ्क)STRLEN(p);
	    new_cmdline = alloc(STRLEN(program) + (size_t)i * (len - 2) + 1);
	    जइ (new_cmdline == रिक्त)
		प्रत्यागम रिक्त;			// out of memory
	    ptr = new_cmdline;
	    जाव ((pos = (char_u *)strstr((वर्ण *)program, "$*")) != रिक्त)
	    {
		i = (पूर्णाङ्क)(pos - program);
		STRNCPY(ptr, program, i);
		STRCPY(ptr += i, p);
		ptr += len;
		program = pos + 2;
	    }
	    STRCPY(ptr, program);
	}
	अण्णहा
	{
	    size_t program_len = STRLEN(program);

	    new_cmdline = alloc(program_len + STRLEN(p) + 2);
	    जइ (new_cmdline == रिक्त)
		प्रत्यागम रिक्त;			// out of memory
	    STRCPY(new_cmdline, program);
	    STRCPY(new_cmdline + program_len, " ");
	    STRCPY(new_cmdline + program_len + 1, p);
	}
	सन्देशः_make(p);

	// 'eap->cmd' is not set here, because it is not used at CMD_make
	vim_free(*cmdlinep);
	*cmdlinep = new_cmdline;
	p = new_cmdline;
    }
    प्रत्यागम p;
}
#endif

/*
 * Expand file name in Ex command argument.
 * When an error is detected, "errormsgp" is set to a non-NULL pointer.
 * Return FAIL for failure, OK otherwise.
 */
    पूर्णाङ्क
expand_filename(
    exarg_T	*eap,
    char_u	**cmdlinep,
    वर्ण	**errormsgp)
{
    पूर्णाङ्क		has_wildcards;	// need to expand wildcards
    char_u	*repl;
    size_t	srclen;
    char_u	*p;
    पूर्णाङ्क		n;
    पूर्णाङ्क		escaped;

#ifdef FEAT_QUICKFIX
    // Skip a regexp pattern for ":vimgrep[add] pat file..."
    p = skip_grep_pat(eap);
#अण्णहा
    p = eap->arg;
#endif

    /*
     * Decide to expand wildcards *before* replacing '%', '#', etc.  If
     * the file name contains a wildcard it should not cause expanding.
     * (it will be expanded anyway if there is a wildcard before replacing).
     */
    has_wildcards = mch_has_wildcard(p);
    जाव (*p != NUL)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`, wildcards in it are not expanded.
	जइ (p[0] == '`' && p[1] == '=')
	{
	    p += 2;
	    (शून्य)skip_व्यञ्जकम्(&p, रिक्त);
	    जइ (*p == '`')
		++p;
	    अनुवर्त;
	}
#endif
	/*
	 * Quick check if this cannot be the start of a special string.
	 * Also removes backslash before '%', '#' and '<'.
	 */
	जइ (vim_strchr((char_u *)"%#<", *p) == रिक्त)
	{
	    ++p;
	    अनुवर्त;
	}

	/*
	 * Try to find a match at this position.
	 */
	repl = eval_vars(p, eap->arg, &srclen, &(eap->do_ecmd_lnum),
						    errormsgp, &escaped, TRUE);
	जइ (*errormsgp != रिक्त)		// error detected
	    प्रत्यागम FAIL;
	जइ (repl == रिक्त)		// no match found
	{
	    p += srclen;
	    अनुवर्त;
	}

	// Wildcards won't be expanded below, the replacement is taken
	// literally.  But do expand "~/file", "~user/file" and "$HOME/file".
	जइ (vim_strchr(repl, '$') != रिक्त || vim_strchr(repl, '~') != रिक्त)
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
	जइ (!eap->usefilter
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
#अण्णहा
# define ESCAPE_CHARS escape_chars
#endif

	    प्रति (l = repl; *l; ++l)
		जइ (vim_strchr(ESCAPE_CHARS, *l) != रिक्त)
		{
		    l = vim_strsave_escaped(repl, ESCAPE_CHARS);
		    जइ (l != रिक्त)
		    {
			vim_free(repl);
			repl = l;
		    }
		    भङ्ग;
		}
	}

	// For a shell command a '!' must be escaped.
	जइ ((eap->usefilter || eap->cmdidx == CMD_bang
						|| eap->cmdidx == CMD_terminal)
			    && vim_strpbrk(repl, (char_u *)"!") != रिक्त)
	{
	    char_u	*l;

	    l = vim_strsave_escaped(repl, (char_u *)"!");
	    जइ (l != रिक्त)
	    {
		vim_free(repl);
		repl = l;
	    }
	}

	p = repl_cmdline(eap, p, srclen, repl, cmdlinep);
	vim_free(repl);
	जइ (p == रिक्त)
	    प्रत्यागम FAIL;
    }

    /*
     * One file argument: Expand wildcards.
     * Don't do this with ":r !command" or ":w !command".
     */
    जइ ((eap->argt & EX_NOSPC) && !eap->usefilter)
    {
	/*
	 * May do this twice:
	 * 1. Replace environment variables.
	 * 2. Replace any other wildcards, remove backslashes.
	 */
	प्रति (n = 1; n <= 2; ++n)
	{
	    जइ (n == 2)
	    {
		/*
		 * Halve the number of backslashes (this is Vi compatible).
		 * For Unix and OS/2, when wildcards are expanded, this is
		 * done by ExpandOne() below.
		 */
#जइ defined(UNIX)
		जइ (!has_wildcards)
#endif
		    backslash_halve(eap->arg);
	    }

	    जइ (has_wildcards)
	    {
		जइ (n == 1)
		{
		    /*
		     * First loop: May expand environment variables.  This
		     * can be done much faster with expand_env() than with
		     * something else (e.g., calling a shell).
		     * After expanding environment variables, check again
		     * if there are still wildcards present.
		     */
		    जइ (vim_strchr(eap->arg, '$') != रिक्त
			    || vim_strchr(eap->arg, '~') != रिक्त)
		    {
			expand_env_esc(eap->arg, NameBuff, MAXPATHL,
					(char_u *)(" \t" PATH_ESC_WILDCARDS),
					TRUE, रिक्त);
			has_wildcards = mch_has_wildcard(NameBuff);
			p = NameBuff;
		    }
		    अण्णहा
			p = रिक्त;
		}
		अण्णहा // n == 2
		{
		    expand_T	xpc;
		    पूर्णाङ्क		options = WILD_LIST_NOTFOUND
					       | WILD_NOERROR | WILD_ADD_SLASH;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    जइ (p_wic)
			options += WILD_ICASE;
		    p = ExpandOne(&xpc, eap->arg, रिक्त,
						   options, WILD_EXPAND_FREE);
		    जइ (p == रिक्त)
			प्रत्यागम FAIL;
		}
		जइ (p != रिक्त)
		{
		    (शून्य)repl_cmdline(eap, eap->arg, STRLEN(eap->arg),
								 p, cmdlinep);
		    जइ (n == 2)	// p came from ExpandOne()
			vim_free(p);
		}
	    }
	}
    }
    प्रत्यागम OK;
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
    जइ (eap->nextcmd != रिक्त)
	i += STRLEN(eap->nextcmd);	// add space for next command
    जइ ((new_cmdline = alloc(i)) == रिक्त)
	प्रत्यागम रिक्त;			// out of memory!

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

    जइ (eap->nextcmd != रिक्त)		// append next command
    {
	new_cmdlinelen += taillen + 1;
	STRCPY(new_cmdline + new_cmdlinelen, eap->nextcmd);
	eap->nextcmd = new_cmdline + new_cmdlinelen;
    }
    eap->cmd = new_cmdline + (eap->cmd - *cmdlinep);
    eap->arg = new_cmdline + (eap->arg - *cmdlinep);
    जइ (eap->do_ecmd_cmd != रिक्त && eap->do_ecmd_cmd != dollar_command)
	eap->do_ecmd_cmd = new_cmdline + (eap->do_ecmd_cmd - *cmdlinep);
    vim_free(*cmdlinep);
    *cmdlinep = new_cmdline;

    प्रत्यागम src;
}

/*
 * Check for '|' to separate commands and '"' to start comments.
 * If "keep_backslash" is TRUE do not remove any backslash.
 */
    शून्य
separate_nextcmd(exarg_T *eap, पूर्णाङ्क keep_backslash)
{
    पूर्णाङ्क		vim9script = in_vim9script();
    char_u	*p;

#ifdef FEAT_QUICKFIX
    p = skip_grep_pat(eap);
#अण्णहा
    p = eap->arg;
#endif

    प्रति ( ; *p; MB_PTR_ADV(p))
    {
	जइ (*p == Ctrl_V)
	{
	    जइ ((eap->argt & (EX_CTRLV | EX_XFILE)) || keep_backslash)
		++p;		// skip CTRL-V and next char
	    अण्णहा
				// remove CTRL-V and skip next char
		STRMOVE(p, p + 1);
	    जइ (*p == NUL)		// stop at NUL after CTRL-V
		भङ्ग;
	}

#ifdef FEAT_EVAL
	// Skip over `=expr` when wildcards are expanded.
	अण्णहा जइ (p[0] == '`' && p[1] == '=' && (eap->argt & EX_XFILE))
	{
	    p += 2;
	    (शून्य)skip_व्यञ्जकम्(&p, रिक्त);
	    जइ (*p == NUL)		// stop at NUL after CTRL-V
		भङ्ग;
	}
#endif

	// Check for '"'/'#': start of comment or '|': next command
	// :@" and :*" do not start a comment!
	// :redir @" doesn't either.
	अण्णहा जइ ((*p == '"'
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
		    && eap->cmdidx != CMD_निवेशय)
		|| *p == '\n')
	{
	    /*
	     * We remove the '\' before the '|', unless EX_CTRLV is used
	     * AND 'b' is present in 'cpoptions'.
	     */
	    जइ ((vim_strchr(p_cpo, CPO_BAR) == रिक्त
			      || !(eap->argt & EX_CTRLV)) && *(p - 1) == '\\')
	    {
		जइ (!keep_backslash)
		{
		    STRMOVE(p - 1, p);	// remove the '\'
		    --p;
		}
	    }
	    अण्णहा
	    {
		set_nextcmd(eap, p);
		*p = NUL;
		भङ्ग;
	    }
	}
    }

    जइ (!(eap->argt & EX_NOTRLCOM))	// remove trailing spaces
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

    जइ (*arg == '+')	    // +[command]
    {
	++arg;
	जइ (vim_isspace(*arg) || *arg == NUL)
	    command = dollar_command;
	अण्णहा
	{
	    command = arg;
	    arg = skip_cmd_arg(command, TRUE);
	    जइ (*arg != NUL)
		*arg++ = NUL;		// terminate command with NUL
	}

	arg = skipwhite(arg);	// skip over spaces
	*argp = arg;
    }
    प्रत्यागम command;
}

/*
 * Find end of "+command" argument.  Skip over "\ " and "\\".
 */
    char_u *
skip_cmd_arg(
    char_u *p,
    पूर्णाङ्क	   rembs)	// TRUE to halve the number of backslashes
{
    जाव (*p && !vim_isspace(*p))
    {
	जइ (*p == '\\' && p[1] != NUL)
	{
	    जइ (rembs)
		STRMOVE(p, p + 1);
	    अण्णहा
		++p;
	}
	MB_PTR_ADV(p);
    }
    प्रत्यागम p;
}

    पूर्णाङ्क
get_bad_opt(char_u *p, exarg_T *eap)
{
    जइ (STRICMP(p, "keep") == 0)
	eap->bad_char = BAD_KEEP;
    अण्णहा जइ (STRICMP(p, "drop") == 0)
	eap->bad_char = BAD_DROP;
    अण्णहा जइ (MB_BYTE2LEN(*p) == 1 && p[1] == NUL)
	eap->bad_char = *p;
    अण्णहा
	प्रत्यागम FAIL;
    प्रत्यागम OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of bad= names.
 */
    स्थिर char_u *
get_bad_name(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    // Note: Keep this in sync with getargopt.
    स्थिर वर्ण *(p_bad_मानानि[]) =
    {
	"?",
	"keep",
	"drop",
    };

    जइ (idx < 0 || idx >= (पूर्णाङ्क)ARRAY_LENGTH(p_bad_मानानि))
	प्रत्यागम रिक्त;

    प्रत्यागम (char_u*)p_bad_मानानि[idx];
}

/*
 * Get "++opt=arg" argument.
 * Return FAIL or OK.
 */
    स्थिर पूर्णाङ्क
getargopt(exarg_T *eap)
{
    char_u	*arg = eap->arg + 2;
    पूर्णाङ्क		*pp = रिक्त;
    पूर्णाङ्क		bad_char_idx;
    char_u	*p;

    // Note: Keep this in sync with get_argopt_name.

    // ":edit ++[no]bin[ary] file"
    जइ (STRNCMP(arg, "bin", 3) == 0 || STRNCMP(arg, "nobin", 5) == 0)
    {
	जइ (*arg == 'n')
	{
	    arg += 2;
	    eap->force_bin = FORCE_NOBIN;
	}
	अण्णहा
	    eap->force_bin = FORCE_BIN;
	जइ (!checkforcmd(&arg, "binary", 3))
	    प्रत्यागम FAIL;
	eap->arg = skipwhite(arg);
	प्रत्यागम OK;
    }

    // ":read ++edit file"
    जइ (STRNCMP(arg, "edit", 4) == 0)
    {
	eap->read_edit = TRUE;
	eap->arg = skipwhite(arg + 4);
	प्रत्यागम OK;
    }

    जइ (STRNCMP(arg, "ff", 2) == 0)
    {
	arg += 2;
	pp = &eap->force_ff;
    }
    अण्णहा जइ (STRNCMP(arg, "fileformat", 10) == 0)
    {
	arg += 10;
	pp = &eap->force_ff;
    }
    अण्णहा जइ (STRNCMP(arg, "enc", 3) == 0)
    {
	जइ (STRNCMP(arg, "encoding", 8) == 0)
	    arg += 8;
	अण्णहा
	    arg += 3;
	pp = &eap->force_enc;
    }
    अण्णहा जइ (STRNCMP(arg, "bad", 3) == 0)
    {
	arg += 3;
	pp = &bad_char_idx;
    }

    जइ (pp == रिक्त || *arg != '=')
	प्रत्यागम FAIL;

    ++arg;
    *pp = (पूर्णाङ्क)(arg - eap->cmd);
    arg = skip_cmd_arg(arg, FALSE);
    eap->arg = skipwhite(arg);
    *arg = NUL;

    जइ (pp == &eap->force_ff)
    {
	जइ (check_ff_मानम्(eap->cmd + eap->force_ff) == FAIL)
	    प्रत्यागम FAIL;
	eap->force_ff = eap->cmd[eap->force_ff];
    }
    अण्णहा जइ (pp == &eap->force_enc)
    {
	// Make 'fileencoding' lower case.
	प्रति (p = eap->cmd + eap->force_enc; *p != NUL; ++p)
	    *p = TOLOWER_ASC(*p);
    }
    अण्णहा
    {
	// Check ++bad= argument.  Must be a single-byte character, "keep" or
	// "drop".
	जइ (get_bad_opt(eap->cmd + bad_char_idx, eap) == FAIL)
	    प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of ++opt names.
 */
    स्थिर char_u *
get_argopt_name(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    // Note: Keep this in sync with getargopt.
    स्थिर वर्ण *(p_opt_मानानि[]) =
    {
	"fileformat=",
	"encoding=",
	"binary",
	"nobinary",
	"bad=",
	"edit",
    };

    जइ (idx < 0 || idx >= (पूर्णाङ्क)ARRAY_LENGTH(p_opt_मानानि))
	प्रत्यागम रिक्त;

    प्रत्यागम (char_u*)p_opt_मानानि[idx];
}

/*
 * Command-line expansion for ++opt=name.
 */
    पूर्णाङ्क
expand_argopt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	पूर्णाङ्क	    *अङ्कः_Matches)
{
    जइ (xp->xp_pattern > xp->xp_पङ्क्तिः && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, पूर्णाङ्क) = रिक्त;

	char_u *name_end = xp->xp_pattern - 1;
	जइ (name_end - xp->xp_पङ्क्तिः >= 2
		&& STRNCMP(name_end - 2, "ff", 2) == 0)
	    cb = get_fileformat_name;
	अण्णहा जइ (name_end - xp->xp_पङ्क्तिः >= 10
		&& STRNCMP(name_end - 10, "fileformat", 10) == 0)
	    cb = get_fileformat_name;
	अण्णहा जइ (name_end - xp->xp_पङ्क्तिः >= 3
		&& STRNCMP(name_end - 3, "enc", 3) == 0)
	    cb = get_encoding_name;
	अण्णहा जइ (name_end - xp->xp_पङ्क्तिः >= 8
		&& STRNCMP(name_end - 8, "encoding", 8) == 0)
	    cb = get_encoding_name;
	अण्णहा जइ (name_end - xp->xp_पङ्क्तिः >= 3
		&& STRNCMP(name_end - 3, "bad", 3) == 0)
	    cb = get_bad_name;

	जइ (cb != रिक्त)
	{
	    प्रत्यागम ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    अङ्कः_Matches,
		    cb,
		    FALSE);
	}
	प्रत्यागम FAIL;
    }

    // Special handling of "ff" which acts as a short form of
    // "fileformat", as "ff" is not a substring of it.
    जइ (xp->xp_pattern_len == 2
	    && STRNCMP(xp->xp_pattern, "ff", xp->xp_pattern_len) == 0)
    {
	*matches = ALLOC_MULT(char_u *, 1);
	जइ (*matches == रिक्त)
	    प्रत्यागम FAIL;
	*अङ्कः_Matches = 1;
	(*matches)[0] = vim_strsave((char_u*)"fileformat=");
	प्रत्यागम OK;
    }

    प्रत्यागम ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    अङ्कः_Matches,
	    get_argopt_name,
	    FALSE);
}

    स्थिर शून्य
ex_autocmd(exarg_T *eap)
{
    जइ (eap->skip)
    {
	skip_cmd_block(eap);
	प्रत्यागम;
    }

    /*
     * Disallow autocommands from .exrc and .vimrc in current
     * directory for security reasons.
     */
    जइ (secure)
    {
	secure = 2;
	eap->errmsg =
	      _(e_command_not_allowed_from_vimrc_in_current_dir_or_tag_अन्वेषणम्);
    }
    अण्णहा जइ (eap->cmdidx == CMD_autocmd)
	do_autocmd(eap, eap->arg, eap->forceit);
    अण्णहा
	do_augroup(eap->arg, eap->forceit);
}

/*
 * ":doautocmd": Apply the automatic commands to the current buffer.
 */
    स्थिर शून्य
ex_doautocmd(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    पूर्णाङ्क		call_do_modelines = check_nomodeline(&arg);
    पूर्णाङ्क		did_aucmd;

    (शून्य)do_doautocmd(arg, TRUE, &did_aucmd);
    // Only when there is no <nomodeline>.
    जइ (call_do_modelines && did_aucmd)
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
    जइ (ERROR_IF_ANY_POPUP_WINDOW)
	प्रत्यागम;
    eap->errmsg = do_bufdel(
	    eap->cmdidx == CMD_bdelete ? DOBUF_DEL
		: eap->cmdidx == CMD_bwipeout ? DOBUF_WIPE
		: DOBUF_UNLOAD, eap->arg,
	    eap->addr_गणनम्, (पूर्णाङ्क)eap->line1, (पूर्णाङ्क)eap->line2, eap->forceit);
}

/*
 * :[N]buffer [N]	to buffer N
 * :[N]sbuffer [N]	to buffer N
 */
    स्थिर शून्य
ex_buffer(exarg_T *eap)
{
    जइ (ERROR_IF_ANY_POPUP_WINDOW)
	प्रत्यागम;
    do_exbuffer(eap);
}

/*
 * ":buffer" command and alike.
 */
    स्थिर शून्य
do_exbuffer(exarg_T *eap)
{
    जइ (*eap->arg)
	eap->errmsg = ex_errmsg(e_trailing_characters_str, eap->arg);
    अण्णहा
    {
	जइ (eap->addr_गणनम् == 0)	// default is current buffer
	    goto_buffer(eap, DOBUF_CURRENT, FORWARD, 0);
	अण्णहा
	    goto_buffer(eap, DOBUF_FIRST, FORWARD, (पूर्णाङ्क)eap->line2);
	जइ (eap->do_ecmd_cmd != रिक्त)
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
    goto_buffer(eap, DOBUF_MOD, FORWARD, (पूर्णाङ्क)eap->line2);
    जइ (eap->do_ecmd_cmd != रिक्त)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bnext [N]	to next buffer
 * :[N]sbnext [N]	split and to next buffer
 */
    स्थिर शून्य
ex_bnext(exarg_T *eap)
{
    जइ (ERROR_IF_ANY_POPUP_WINDOW)
	प्रत्यागम;

    goto_buffer(eap, DOBUF_CURRENT, FORWARD, (पूर्णाङ्क)eap->line2);
    जइ (eap->do_ecmd_cmd != रिक्त)
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
    जइ (ERROR_IF_ANY_POPUP_WINDOW)
	प्रत्यागम;

    goto_buffer(eap, DOBUF_CURRENT, BACKWARD, (पूर्णाङ्क)eap->line2);
    जइ (eap->do_ecmd_cmd != रिक्त)
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
    जइ (ERROR_IF_ANY_POPUP_WINDOW)
	प्रत्यागम;

    goto_buffer(eap, DOBUF_FIRST, FORWARD, 0);
    जइ (eap->do_ecmd_cmd != रिक्त)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :blast		to last buffer
 * :sblast		split and to last buffer
 */
    स्थिर शून्य
ex_blast(exarg_T *eap)
{
    जइ (ERROR_IF_ANY_POPUP_WINDOW)
	प्रत्यागम;

    goto_buffer(eap, DOBUF_LAST, BACKWARD, 0);
    जइ (eap->do_ecmd_cmd != रिक्त)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * Check if "c" ends an Ex command.
 * In Vim9 script does not check for white space before #.
 */
    पूर्णाङ्क
ends_excmd(पूर्णाङ्क c)
{
    पूर्णाङ्क comment_char = '"';

    जइ (in_vim9script())
	comment_char = '#';
    प्रत्यागम (c == NUL || c == '|' || c == comment_char || c == '\n');
}

/*
 * Like ends_excmd() but checks that a # in Vim9 script either has "cmd" equal
 * to "cmd_start" or has a white space character before it.
 */
    पूर्णाङ्क
ends_excmd2(char_u *cmd_start UNUSED, char_u *cmd)
{
    पूर्णाङ्क c = *cmd;

    जइ (c == NUL || c == '|' || c == '\n')
	प्रत्यागम TRUE;
    जइ (in_vim9script())
	//  # starts a comment, #{ might be a mistake, #{{ can start a fold
	प्रत्यागम c == '#' && (cmd[1] != '{' || cmd[2] == '{')
				 && (cmd == cmd_start || VIM_ISWHITE(cmd[-1]));
    प्रत्यागम c == '"';
}

#जइ defined(FEAT_SYN_HL) || defined(FEAT_अन्वेषणम्_EXTRA) || defined(FEAT_EVAL)
/*
 * Return the next command, after the first '|' or '\n'.
 * Return NULL if not found.
 */
    char_u *
find_nextcmd(char_u *p)
{
    जाव (*p != '|' && *p != '\n')
    {
	जइ (*p == NUL)
	    प्रत्यागम रिक्त;
	++p;
    }
    प्रत्यागम (p + 1);
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

    जइ (*s == '|' || *s == '\n')
	प्रत्यागम (s + 1);
    अण्णहा
	प्रत्यागम रिक्त;
}

/*
 * If "eap->nextcmd" is not set, check for a next command at "p".
 */
    शून्य
set_nextcmd(exarg_T *eap, char_u *arg)
{
    char_u *p = skipwhite(arg);

    जइ (in_vim9script() && *p == '#')
    {
	char_u *nl = vim_strchr(p, NL);
	जइ (nl != रिक्त)
	    p = nl;
    }

    p = check_nextcmd(p);

    जइ (eap->nextcmd == रिक्त)
	eap->nextcmd = p;
    अण्णहा जइ (p != रिक्त)
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
    स्थिर पूर्णाङ्क
check_more(
    पूर्णाङ्क message,	    // when FALSE check only, no messages
    पूर्णाङ्क forceit)
{
    पूर्णाङ्क	    n = ARGCOUNT - curwin->w_arg_idx - 1;

    जइ (!forceit && only_one_window()
	    && ARGCOUNT > 1 && !arg_had_last && n > 0 && quitmore == 0)
    {
	जइ (message)
	{
#जइ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    जइ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						    && curbuf->b_fname != रिक्त)
	    {
		char_u	buff[DIALOG_सन्देशः_SIZE];

		vim_snprintf((वर्ण *)buff, DIALOG_सन्देशः_SIZE,
			NGETTEXT("%d more file to edit.  Quit anyway?",
			    "%d more files to edit.  Quit anyway?", n), n);
		जइ (vim_dialog_yesno(VIM_QUESTION, रिक्त, buff, 1) == VIM_YES)
		    प्रत्यागम OK;
		प्रत्यागम FAIL;
	    }
#endif
	    semsg(NGETTEXT(e_nr_more_file_to_edit,
			   e_nr_more_files_to_edit , n), n);
	    quitmore = 2;	    // next try to quit is allowed
	}
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of command names.
 */
    char_u *
get_command_name(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    जइ (idx >= (पूर्णाङ्क)CMD_SIZE)
	प्रत्यागम expand_user_command_name(idx);
    // the following are no real commands
    जइ (STRNCMP(cmdnames[idx].cmd_name, "{", 1) == 0 ||
	STRNCMP(cmdnames[idx].cmd_name, "}", 1) == 0)
	प्रत्यागम (char_u *)"";
    प्रत्यागम cmdnames[idx].cmd_name;
}

    स्थिर शून्य
ex_colorscheme(exarg_T *eap)
{
    जइ (*eap->arg == NUL)
    {
#ifdef FEAT_EVAL
	char_u *व्यञ्जकम् = vim_strsave((char_u *)"g:colors_name");
	char_u *p = रिक्त;

	जइ (व्यञ्जकम् != रिक्त)
	{
	    ++emsg_off;
	    p = eval_to_string(व्यञ्जकम्, FALSE, FALSE);
	    --emsg_off;
	    vim_free(व्यञ्जकम्);
	}
	जइ (p != रिक्त)
	{
	    सन्देशः((वर्ण *)p);
	    vim_free(p);
	}
	अण्णहा
	    सन्देशः("default");
#अण्णहा
	सन्देशः(_("unknown"));
#endif
    }
    अण्णहा जइ (load_colors(eap->arg) == FAIL)
	semsg(_(e_cannot_find_color_scheme_str), eap->arg);

#ifdef FEAT_VTP
    अण्णहा जइ (has_vtp_working())
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
    जइ (*eap->arg == NUL && eap->cmd[2] == '!')
	सन्देशः(_("Greetings, Vim user!"));
    do_highlight(eap->arg, eap->forceit, FALSE);
}


/*
 * Call this function if we thought we were going to exit, but we won't
 * (because of an error).  May need to restore the terminal mode.
 */
    शून्य
not_exiting(पूर्णाङ्क save_exiting)
{
    exiting = save_exiting;
    settmode(TMODE_RAW);
}

    पूर्णाङ्क
before_quit_autocmds(win_T *wp, पूर्णाङ्क quit_all, पूर्णाङ्क forceit)
{
    apply_autocmds(EVENT_QUITPRE, रिक्त, रिक्त, FALSE, wp->w_buffer);

    // Bail out when autocommands closed the window.
    // Refuse to quit when the buffer in the last window is being closed (can
    // only happen in autocommands).
    जइ (!win_valid(wp)
	    || curbuf_locked()
	    || (wp->w_buffer->b_nwindows == 1 && wp->w_buffer->b_locked > 0))
	प्रत्यागम TRUE;

    जइ (quit_all || (check_more(FALSE, forceit) == OK && only_one_window()))
    {
	apply_autocmds(EVENT_EXITPRE, रिक्त, रिक्त, FALSE, curbuf);
	// Refuse to quit when locked or when the window was closed or the
	// buffer in the last window is being closed (can only happen in
	// autocommands).
	जइ (!win_valid(wp) || curbuf_locked()
			  || (curbuf->b_nwindows == 1 && curbuf->b_locked > 0))
	    प्रत्यागम TRUE;
    }

    प्रत्यागम FALSE;
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

    जइ (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	प्रत्यागम;
    }
    // Don't quit while editing the command line.
    जइ (text_locked())
    {
	text_locked_सन्देशः();
	प्रत्यागम;
    }
    जइ (eap->addr_गणनम् > 0)
    {
	पूर्णाङ्क	wnr = eap->line2;

	प्रति (wp = firstwin; wp->w_next != रिक्त; wp = wp->w_next)
	    जइ (--wnr <= 0)
		भङ्ग;
    }
    अण्णहा
	wp = curwin;

    // Refuse to quit when locked.
    जइ (curbuf_locked())
	प्रत्यागम;

    // Trigger QuitPre and maybe ExitPre
    जइ (before_quit_autocmds(wp, FALSE, eap->forceit))
	प्रत्यागम;

#ifdef FEAT_NETBEANS_INTG
    netbeansForcedQuit = eap->forceit;
#endif

    पूर्णाङ्क save_exiting = exiting;
    /*
     * If there is only one relevant window we will exit.
     */
    जइ (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;
    जइ ((!buf_hide(wp->w_buffer)
		&& check_changed(wp->w_buffer, (p_awa ? CCGD_AW : 0)
				       | (eap->forceit ? CCGD_FORCEIT : 0)
				       | CCGD_EXCMD))
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, TRUE)))
    {
	not_exiting(save_exiting);
    }
    अण्णहा
    {
	// quit last window
	// Note: only_one_window() returns true, even so a help window is
	// still open. In that case only quit, if no address has been
	// specified. Example:
	// :h|wincmd w|1q     - don't quit
	// :h|wincmd w|q      - quit
	जइ (only_one_window() && (ONE_WINDOW || eap->addr_गणनम् == 0))
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
    getout(eap->addr_गणनम् > 0 ? (पूर्णाङ्क)eap->line2 : EXIT_FAILURE);
}

/*
 * Do preparations for "qall" and "wqall".
 * Returns FAIL when quitting should be aborted.
 */
    पूर्णाङ्क
before_quit_all(exarg_T *eap)
{
    जइ (cmdwin_type != 0)
    {
	जइ (eap->forceit)
	    cmdwin_result = K_XF1;	// ex_window() takes care of this
	अण्णहा
	    cmdwin_result = K_XF2;
	प्रत्यागम FAIL;
    }

    // Don't quit while editing the command line.
    जइ (text_locked())
    {
	text_locked_सन्देशः();
	प्रत्यागम FAIL;
    }

    जइ (before_quit_autocmds(curwin, TRUE, eap->forceit))
	प्रत्यागम FAIL;

    प्रत्यागम OK;
}

/*
 * ":qall": try to quit all windows
 */
    स्थिर शून्य
ex_quit_all(exarg_T *eap)
{
    जइ (before_quit_all(eap) == FAIL)
	प्रत्यागम;
    पूर्णाङ्क save_exiting = exiting;
    exiting = TRUE;
    जइ (eap->forceit || !check_changed_any(FALSE, FALSE))
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
    पूर्णाङ्क		winnr = 0;
    जइ (cmdwin_type != 0)
	cmdwin_result = Ctrl_C;
    अण्णहा
	जइ (!text_locked() && !curbuf_locked())
	{
	    जइ (eap->addr_गणनम् == 0)
		ex_win_close(eap->forceit, curwin, रिक्त);
	    अण्णहा
	    {
		FOR_ALL_WINDOWS(win)
		{
		    winnr++;
		    जइ (winnr == eap->line2)
			भङ्ग;
		}
		जइ (win == रिक्त)
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
	जइ (win->w_p_pvw)
	{
	    ex_win_close(eap->forceit, win, रिक्त);
	    प्रत्यागम;
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
    पूर्णाङ्क		forceit,
    win_T	*win,
    tabpage_T	*tp)		// NULL or the tab page "win" is in
{
    पूर्णाङ्क		need_hide;
    buf_T	*buf = win->w_buffer;

    // Never close the autocommand window.
    जइ (is_aucmd_win(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	प्रत्यागम;
    }
    जइ (window_layout_locked(CMD_close))
	प्रत्यागम;

    need_hide = (bufIsChanged(buf) && buf->b_nwindows <= 1);
    जइ (need_hide && !buf_hide(buf) && !forceit)
    {
#जइ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	जइ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    जइ (term_job_running(buf->b_term))
	    {
		जइ (term_confirm_stop(buf) == FAIL)
		    प्रत्यागम;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(buf);
		need_hide = FALSE;
	    }
	    अण्णहा
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(buf, FALSE);
		जइ (bufref_valid(&bufref) && bufIsChanged(buf))
		    प्रत्यागम;
		need_hide = FALSE;
	    }
	}
	अण्णहा
#endif
	{
	    no_write_message();
	    प्रत्यागम;
	}
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // free buffer when not hiding it or when it's a scratch buffer
    जइ (tp == रिक्त)
	win_close(win, !need_hide && !buf_hide(buf));
    अण्णहा
	win_close_othertab(win, !need_hide && !buf_hide(buf), tp);
}

/*
 * Handle the argument for a tabpage related ex command.
 * Returns a tabpage number.
 * When an error is encountered then eap->errmsg is set.
 */
    स्थिर पूर्णाङ्क
get_tabpage_arg(exarg_T *eap)
{
    पूर्णाङ्क tab_सङ्ख्या;
    पूर्णाङ्क unaccept_arg0 = (eap->cmdidx == CMD_tabmove) ? 0 : 1;

    जइ (eap->arg && *eap->arg != NUL)
    {
	char_u *p = eap->arg;
	char_u *p_save;
	पूर्णाङ्क    relative = 0; // argument +N/-N means: go to N places to the
			     // right/left relative to the current position.

	जइ (*p == '-')
	{
	    relative = -1;
	    p++;
	}
	अण्णहा जइ (*p == '+')
	{
	    relative = 1;
	    p++;
	}

	p_save = p;
	tab_सङ्ख्या = getdigits(&p);

	जइ (relative == 0)
	{
	    जइ (STRCMP(p, "$") == 0)
		tab_सङ्ख्या = LAST_TAB_NR;
	    अण्णहा जइ (STRCMP(p, "#") == 0)
		जइ (valid_tabpage(lastused_tabpage))
		    tab_सङ्ख्या = tabpage_index(lastused_tabpage);
		अण्णहा
		{
		    eap->errmsg = ex_errmsg(e_invalid_मानम्_for_argument_str,
								     eap->arg);
		    tab_सङ्ख्या = 0;
		    गच्छ theend;
		}
	    अण्णहा जइ (p == p_save || *p_save == '-' || *p != NUL
		    || tab_सङ्ख्या > LAST_TAB_NR)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		गच्छ theend;
	    }
	}
	अण्णहा
	{
	    जइ (*p_save == NUL)
		tab_सङ्ख्या = 1;
	    अण्णहा जइ (p == p_save || *p_save == '-' || *p != NUL
		    || tab_सङ्ख्या == 0)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		गच्छ theend;
	    }
	    tab_सङ्ख्या = tab_सङ्ख्या * relative + tabpage_index(curtab);
	    जइ (!unaccept_arg0 && relative == -1)
		--tab_सङ्ख्या;
	}
	जइ (tab_सङ्ख्या < unaccept_arg0 || tab_सङ्ख्या > LAST_TAB_NR)
	    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
    }
    अण्णहा जइ (eap->addr_गणनम् > 0)
    {
	जइ (unaccept_arg0 && eap->line2 == 0)
	{
	    eap->errmsg = _(e_invalid_range);
	    tab_सङ्ख्या = 0;
	}
	अण्णहा
	{
	    tab_सङ्ख्या = eap->line2;
	    जइ (!unaccept_arg0)
	    {
		char_u *cmdp = eap->cmd;

		जाव (--cmdp > *eap->cmdlinep
			&& (VIM_ISWHITE(*cmdp) || VIM_ISDIGIT(*cmdp)))
		    ;
		जइ (*cmdp == '-')
		{
		    --tab_सङ्ख्या;
		    जइ (tab_सङ्ख्या < unaccept_arg0)
			eap->errmsg = _(e_invalid_range);
		}
	    }
	}
    }
    अण्णहा
    {
	विकल्प (eap->cmdidx)
	{
	पक्ष CMD_tabnext:
	    tab_सङ्ख्या = tabpage_index(curtab) + 1;
	    जइ (tab_सङ्ख्या > LAST_TAB_NR)
		tab_सङ्ख्या = 1;
	    भङ्ग;
	पक्ष CMD_tabmove:
	    tab_सङ्ख्या = LAST_TAB_NR;
	    भङ्ग;
	स्वभाव:
	    tab_सङ्ख्या = tabpage_index(curtab);
	}
    }

theend:
    प्रत्यागम tab_सङ्ख्या;
}

/*
 * ":tabclose": close current tab page, unless it is the last one.
 * ":tabclose N": close tab page N.
 */
    स्थिर शून्य
ex_tabclose(exarg_T *eap)
{
    tabpage_T	*tp;
    पूर्णाङ्क		tab_सङ्ख्या;

    जइ (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	प्रत्यागम;
    }

    जइ (first_tabpage->tp_next == रिक्त)
    {
	emsg(_(e_cannot_close_last_tab_page));
	प्रत्यागम;
    }

    जइ (window_layout_locked(CMD_tabclose))
	प्रत्यागम;

    tab_सङ्ख्या = get_tabpage_arg(eap);
    जइ (eap->errmsg != रिक्त)
	प्रत्यागम;

    tp = find_tabpage(tab_सङ्ख्या);
    जइ (tp == रिक्त)
    {
	beep_flush();
	प्रत्यागम;
    }
    जइ (tp != curtab)
    {
	tabpage_close_other(tp, eap->forceit);
	प्रत्यागम;
    }
    अण्णहा जइ (!text_locked() && !curbuf_locked())
	tabpage_close(eap->forceit);
}

/*
 * ":tabonly": close all tab pages except the current one
 */
    स्थिर शून्य
ex_tabonly(exarg_T *eap)
{
    tabpage_T	*tp;
    पूर्णाङ्क		done;
    पूर्णाङ्क		tab_सङ्ख्या;

    जइ (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	प्रत्यागम;
    }

    जइ (first_tabpage->tp_next == रिक्त)
    {
	सन्देशः(_("Already only one tab page"));
	प्रत्यागम;
    }

    जइ (window_layout_locked(CMD_tabonly))
	प्रत्यागम;

    tab_सङ्ख्या = get_tabpage_arg(eap);
    जइ (eap->errmsg != रिक्त)
	प्रत्यागम;

    goto_tabpage(tab_सङ्ख्या);
    // Repeat this up to a 1000 times, because autocommands may
    // mess up the lists.
    प्रति (done = 0; done < 1000; ++done)
    {
	FOR_ALL_TABPAGES(tp)
	    जइ (tp->tp_topframe != topframe)
	    {
		tabpage_close_other(tp, eap->forceit);
		// if we failed to close it quit
		जइ (valid_tabpage(tp))
		    done = 1000;
		// start over, "tp" is now invalid
		भङ्ग;
	    }
	जइ (first_tabpage->tp_next == रिक्त)
	    भङ्ग;
    }
}

/*
 * Close the current tab page.
 */
    शून्य
tabpage_close(पूर्णाङ्क forceit)
{
    जइ (window_layout_locked(CMD_tabclose))
	प्रत्यागम;

    trigger_tabclosedpre(curtab);
    curtab->tp_did_tabclosedpre = TRUE;
    tabpage_T *save_curtab = curtab;

    // First close all the windows but the current one.  If that worked then
    // close the last window in this tab, that will close it.
    जइ (!ONE_WINDOW)
	close_others(TRUE, forceit);
    जइ (ONE_WINDOW)
	ex_win_close(forceit, curwin, रिक्त);
    जइ (curtab == save_curtab)
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
tabpage_close_other(tabpage_T *tp, पूर्णाङ्क forceit)
{
    पूर्णाङ्क		done = 0;
    win_T	*wp;

    जइ (window_layout_locked(CMD_SIZE))
	प्रत्यागम;

    trigger_tabclosedpre(tp);
    tp->tp_did_tabclosedpre = TRUE;

    // Limit to 1000 windows, autocommands may add a window while we close
    // one.  OK, so I'm paranoid...
    जाव (++done < 1000)
    {
	wp = tp->tp_firstwin;
	ex_win_close(forceit, wp, tp);

	// Autocommands may delete the tab page under our fingers.
	जइ (!valid_tabpage(tp))
	    भङ्ग;
	// We may fail to close a window with a modified buffer.
	जइ (tp->tp_firstwin == wp)
	{
	    done = 1000;
	    भङ्ग;
	}
    }
    जइ (done >= 1000)
    {
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	tp->tp_did_tabclosedpre = FALSE;
	प्रत्यागम;
    }

    apply_autocmds(EVENT_TABCLOSED, रिक्त, रिक्त, FALSE, curbuf);
}

/*
 * ":only".
 */
    स्थिर शून्य
ex_only(exarg_T *eap)
{
    जइ (window_layout_locked(CMD_only))
	प्रत्यागम;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    जइ (eap->addr_गणनम् > 0)
    {
	win_T   *wp;
	पूर्णाङ्क	wnr = eap->line2;
	प्रति (wp = firstwin; --wnr > 0; )
	{
	    जइ (wp->w_next == रिक्त)
		भङ्ग;
	    अण्णहा
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
    जइ (eap->skip)
	प्रत्यागम;

    जइ (window_layout_locked(CMD_hide))
	प्रत्यागम;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    जइ (eap->addr_गणनम् == 0)
	win_close(curwin, FALSE);	// don't free buffer
    अण्णहा
    {
	पूर्णाङ्क	winnr = 0;
	win_T	*win;

	FOR_ALL_WINDOWS(win)
	{
	    winnr++;
	    जइ (winnr == eap->line2)
		भङ्ग;
	}
	जइ (win == रिक्त)
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
    जइ (check_restricted())
	प्रत्यागम;

    जइ (!eap->forceit)
	autowrite_all();
    apply_autocmds(EVENT_VIMSUSPEND, रिक्त, रिक्त, FALSE, रिक्त);
    windgoto((पूर्णाङ्क)Rows - 1, cmdline_col_off);
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
    जइ (not_in_vim9(eap, false) == FAIL)
	प्रत्यागम;
#endif
    जइ (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	प्रत्यागम;
    }
    // Don't quit while editing the command line.
    जइ (text_locked())
    {
	text_locked_सन्देशः();
	प्रत्यागम;
    }

    पूर्णाङ्क save_exiting = exiting;
    /*
     * we plan to exit if there is only one relevant window
     */
    जइ (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;

    // Write the buffer for ":wq" or when it was changed.
    // Trigger QuitPre and ExitPre.
    // Check if we can exit now, after autocommands have changed things.
    जइ (((eap->cmdidx == CMD_wq || curbufIsChanged()) && do_write(eap) == FAIL)
	    || before_quit_autocmds(curwin, FALSE, eap->forceit)
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, FALSE)))
    {
	not_exiting(save_exiting);
    }
    अण्णहा
    {
	जइ (only_one_window())	    // quit last window, exit Vim
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
    जइ (curbuf->b_ml.ml_flags & ML_EMPTY)
	emsg(_(e_empty_buffer));
    अण्णहा
    {
	प्रति ( ;!got_int; ui_breakcheck())
	{
	    print_पङ्क्तिः(eap->line1,
		    (eap->cmdidx == CMD_सङ्ख्या || eap->cmdidx == CMD_pound
						 || (eap->flags & EXFLAG_NR)),
		    eap->cmdidx == CMD_list || (eap->flags & EXFLAG_LIST));
	    जइ (++eap->line1 > eap->line2)
		भङ्ग;
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

#जइ defined(HAVE_DROP_FILE)

स्थिर पूर्णाङ्क drop_busy = FALSE;
स्थिर पूर्णाङ्क drop_filec;
स्थिर char_u **drop_filev = रिक्त;
स्थिर पूर्णाङ्क drop_split;
स्थिर शून्य (*drop_callback)(शून्य *);
स्थिर शून्य *drop_cookie;

    स्थिर शून्य
handle_drop_internal(शून्य)
{
    exarg_T	ea;
    पूर्णाङ्क		save_सन्देशः_scroll = सन्देशः_scroll;

    // Setting the argument list may cause screen updates and being called
    // recursively.  Avoid that by setting drop_busy.
    drop_busy = TRUE;

    // Check whether the current buffer is changed. If so, we will need
    // to split the current window or data could be lost.
    // We don't need to check if the 'hidden' option is set, as in this
    // case the buffer won't be lost.
    जइ (!buf_hide(curbuf) && !drop_split)
    {
	++emsg_off;
	drop_split = check_changed(curbuf, CCGD_AW);
	--emsg_off;
    }
    जइ (drop_split)
    {
	जइ (win_split(0, 0) == FAIL)
	    प्रत्यागम;
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
    सन्देशः_scroll = save_सन्देशः_scroll;

    जइ (drop_callback != रिक्त)
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
    पूर्णाङ्क		filec,		// the number of files dropped
    char_u	**filev,	// the list of files dropped
    पूर्णाङ्क		split,		// force splitting the window
    शून्य	(*callback)(शून्य *), // to be called after setting the argument
				     // list
    शून्य	*cookie)	// argument for "callback" (allocated)
{
    // Cannot handle recursive drops, finish the pending one.
    जइ (drop_busy)
    {
	FreeWild(filec, filev);
	vim_free(cookie);
	प्रत्यागम;
    }

    // When calling handle_drop() more than once in a row we only use the last
    // one.
    जइ (drop_filev != रिक्त)
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
    जइ (text_locked() || curbuf_locked() || updating_screen)
	प्रत्यागम;

    handle_drop_internal();
}

/*
 * To be called when text is unlocked, curbuf is unlocked or updating_screen is
 * reset: Handle a postponed drop.
 */
    शून्य
handle_any_postponed_drop(शून्य)
{
    जइ (!drop_busy && drop_filev != रिक्त
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
    जइ (!check_changed(curbuf, (p_awa ? CCGD_AW : 0)
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

#जइ defined(FEAT_EVAL)

// callback function for 'findfunc'
स्थिर callback_T ffu_cb;

    स्थिर callback_T *
get_findfunc_callback(शून्य)
{
    प्रत्यागम *curbuf->b_p_ffu != NUL ? &curbuf->b_ffu_cb : &ffu_cb;
}

/*
 * Call 'findfunc' to obtain a list of file names.
 */
    स्थिर list_T *
call_findfunc(char_u *pat, पूर्णाङ्क cmdcomplete)
{
    typval_T	args[3];
    callback_T	*cb;
    typval_T	rettv;
    पूर्णाङ्क		retval;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_BOOL;
    args[1].vval.v_सङ्ख्या = cmdcomplete;
    args[2].v_type = VAR_UNKNOWN;

    // Lock the text to prevent weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;

    ctx = get_option_sctx("findfunc");
    जइ (ctx != रिक्त)
	current_sctx = *ctx;

    cb = get_findfunc_callback();
    retval = call_callback(cb, -1, &rettv, 2, args);

    current_sctx = saved_sctx;

    --textlock;

    list_T *retlist = रिक्त;

    जइ (retval == OK)
    {
	जइ (rettv.v_type == VAR_LIST)
	    retlist = list_copy(rettv.vval.v_list, FALSE, FALSE, get_copyID());
	अण्णहा
	    emsg(_(e_invalid_return_type_from_findfunc));

	clear_tv(&rettv);
    }

    प्रत्यागम retlist;
}

/*
 * Find file names matching "pat" using 'findfunc' and return it in "files".
 * Used for expanding the :find, :sfind and :tabfind command argument.
 * Returns OK on success and FAIL otherwise.
 */
    पूर्णाङ्क
expand_findfunc(expand_T *xp, char_u *pat, char_u ***files, पूर्णाङ्क *अङ्कः_Matches)
{
    list_T	*l;
    पूर्णाङ्क		len;

    *अङ्कः_Matches = 0;
    *files = रिक्त;

    l = call_findfunc(pat, VVAL_TRUE);

    जइ (l == रिक्त)
	प्रत्यागम FAIL;

    len = list_len(l);
    जइ (len == 0)	    // empty List
    {
	list_free(l);
	प्रत्यागम FAIL;
    }

    expand_process_user_list(l, files, अङ्कः_Matches, xp);
    list_free(l);

    प्रत्यागम OK;
}

/*
 * Use 'findfunc' to find file 'findarg'.  The 'count' argument is used to find
 * the n'th matching file.
 */
    स्थिर char_u *
findfunc_find_file(char_u *findarg, पूर्णाङ्क findarg_len, पूर्णाङ्क गणनम्)
{
    list_T	*fname_list;
    char_u	*ret_fname = रिक्त;
    char_u	cc;
    पूर्णाङ्क		fname_गणनम्;

    cc = findarg[findarg_len];
    findarg[findarg_len] = NUL;

    fname_list = call_findfunc(findarg, VVAL_FALSE);
    fname_गणनम् = list_len(fname_list);

    जइ (fname_गणनम् == 0)
	semsg(_(e_cant_find_file_str_in_path), findarg);
    अण्णहा
    {
	जइ (गणनम् > fname_गणनम्)
	    semsg(_(e_no_more_file_str_found_in_path), findarg);
	अण्णहा
	{
	    listitem_T *li = list_find(fname_list, गणनम् - 1);

	    जइ (li != रिक्त)
	    {
		typval_T *tv = &li->li_tv;

		जइ (tv->v_type == VAR_STRING && tv->vval.v_string != रिक्त)
		    ret_fname = vim_strsave(tv->vval.v_string);
		अण्णहा जइ (tv->v_type == VAR_DICT && tv->vval.v_dict != रिक्त)
		    ret_fname = dict_get_string(tv->vval.v_dict, "word", TRUE);
	    }
	}
    }

    जइ (fname_list != रिक्त)
	list_free(fname_list);

    findarg[findarg_len] = cc;

    प्रत्यागम ret_fname;
}

/*
 * Process the 'findfunc' option value.
 * Returns NULL on success and an error message on failure.
 */
    वर्ण *
did_set_findfunc(optset_T *args UNUSED)
{
    पूर्णाङ्क	retval;

    जइ (args->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_ffu, &curbuf->b_ffu_cb);
    अण्णहा
    {
	// global option set
	retval = option_set_callback_func(p_ffu, &ffu_cb);
	// when using :set, free the local callback
	जइ (!(args->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_ffu_cb);
    }

    जइ (retval == FAIL)
	प्रत्यागम e_invalid_argument;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*name = get_scriptlocal_funcname(*varp);
    जइ (name != रिक्त)
    {
	free_string_option(*varp);
	*varp = name;
    }

    प्रत्यागम रिक्त;
}

# जइ defined(EXITFREE)
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
    पूर्णाङ्क
set_ref_in_findfunc(पूर्णाङ्क copyID UNUSED)
{
    पूर्णाङ्क abort = FALSE;
    abort = set_ref_in_callback(&ffu_cb, copyID);
    प्रत्यागम abort;
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
    पूर्णाङ्क		save_cmod_flags = cmdmod.cmod_flags;
#endif
    पूर्णाङ्क		use_tab = eap->cmdidx == CMD_tabedit
		       || eap->cmdidx == CMD_tabfind
		       || eap->cmdidx == CMD_tabnew;

    जइ (ERROR_IF_ANY_POPUP_WINDOW)
	प्रत्यागम;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#ifdef FEAT_QUICKFIX
    // A ":split" in the quickfix window works like ":new".  Don't want two
    // quickfix windows.  But it's OK when doing ":tab split".
    जइ (bt_quickfix(curbuf) && cmdmod.cmod_tab == 0)
    {
	जइ (eap->cmdidx == CMD_split)
	    eap->cmdidx = CMD_new;
	जइ (eap->cmdidx == CMD_vsplit)
	    eap->cmdidx = CMD_vnew;
    }
#endif

    जइ (eap->cmdidx == CMD_sfind || eap->cmdidx == CMD_tabfind)
    {
	char_u	*file_to_find = रिक्त;
	वर्ण	*अन्वेषणम्_ctx = रिक्त;

	जइ (*get_findfunc() != NUL)
	{
#ifdef FEAT_EVAL
	    fname = findfunc_find_file(eap->arg, (पूर्णाङ्क)STRLEN(eap->arg),
				       eap->addr_गणनम् > 0 ? eap->line2 : 1);
#endif
	}
	अण्णहा
	{
	    fname = find_file_in_path(eap->arg, (पूर्णाङ्क)STRLEN(eap->arg),
					      FNAME_MESS, TRUE, curbuf->b_ffname,
					      &file_to_find, &अन्वेषणम्_ctx);
	    vim_free(file_to_find);
	    vim_findfile_cleanup(अन्वेषणम्_ctx);
	}
	जइ (fname == रिक्त)
	    गच्छ theend;
	eap->arg = fname;
    }
#ifdef FEAT_BROWSE
    अण्णहा जइ ((cmdmod.cmod_flags & CMOD_BROWSE)
	    && eap->cmdidx != CMD_vnew
	    && eap->cmdidx != CMD_new)
    {
	जइ (
# ifdef FEAT_GUI
	    !gui.in_use &&
# endif
		au_has_group((char_u *)"FileExplorer"))
	{
	    // No browsing supported but we do have the file explorer:
	    // Edit the directory.
	    जइ (*eap->arg == NUL || !mch_isdir(eap->arg))
		eap->arg = dot_path;
	}
	अण्णहा
	{
	    fname = do_browse(0, (char_u *)(use_tab
			? _("Edit File in new tab page")
			: _("Edit File in new window")),
					  eap->arg, रिक्त, रिक्त, रिक्त, curbuf);
	    जइ (fname == रिक्त)
		गच्छ theend;
	    eap->arg = fname;
	}
    }
    cmdmod.cmod_flags &= ~CMOD_BROWSE;	// Don't browse again in do_ecmd().
#endif

    /*
     * Either open new tab page or split the window.
     */
    जइ (use_tab)
    {
	जइ (win_new_tabpage(cmdmod.cmod_tab != 0 ? cmdmod.cmod_tab
			 : eap->addr_गणनम् == 0 ? 0
					       : (पूर्णाङ्क)eap->line2 + 1) != FAIL)
	{
	    do_exedit(eap, old_curwin);

	    // set the alternate buffer for the window we came from
	    जइ (curwin != old_curwin
		    && win_valid(old_curwin)
		    && old_curwin->w_buffer != curbuf
		    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		old_curwin->w_alt_fnum = curbuf->b_fnum;
	}
    }
    अण्णहा जइ (win_split(eap->addr_गणनम् > 0 ? (पूर्णाङ्क)eap->line2 : 0,
				     *eap->cmd == 'v' ? WSP_VERT : 0) != FAIL)
    {
	// Reset 'scrollbind' when editing another file, but keep it when
	// doing ":split" without arguments.
	जइ (*eap->arg != NUL)
	    RESET_BINDING(curwin);
	अण्णहा
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
    पूर्णाङ्क tab_सङ्ख्या;

    जइ (ERROR_IF_POPUP_WINDOW)
	प्रत्यागम;
    विकल्प (eap->cmdidx)
    {
	पक्ष CMD_tabfirst:
	पक्ष CMD_tabrewind:
	    goto_tabpage(1);
	    भङ्ग;
	पक्ष CMD_tablast:
	    goto_tabpage(9999);
	    भङ्ग;
	पक्ष CMD_tabprevious:
	पक्ष CMD_tabNext:
	    जइ (eap->arg && *eap->arg != NUL)
	    {
		char_u *p = eap->arg;
		char_u *p_save = p;

		tab_सङ्ख्या = getdigits(&p);
		जइ (p == p_save || *p_save == '-' || *p != NUL
			    || tab_सङ्ख्या == 0)
		{
		    // No numbers as argument.
		    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		    प्रत्यागम;
		}
	    }
	    अण्णहा
	    {
		जइ (eap->addr_गणनम् == 0)
		    tab_सङ्ख्या = 1;
		अण्णहा
		{
		    tab_सङ्ख्या = eap->line2;
		    जइ (tab_सङ्ख्या < 1)
		    {
			eap->errmsg = _(e_invalid_range);
			प्रत्यागम;
		    }
		}
	    }
	    goto_tabpage(-tab_सङ्ख्या);
	    भङ्ग;
	स्वभाव: // CMD_tabnext
	    tab_सङ्ख्या = get_tabpage_arg(eap);
	    जइ (eap->errmsg == रिक्त)
		goto_tabpage(tab_सङ्ख्या);
	    भङ्ग;
    }
}

/*
 * :tabmove command
 */
    स्थिर शून्य
ex_tabmove(exarg_T *eap)
{
    पूर्णाङ्क tab_सङ्ख्या;

    tab_सङ्ख्या = get_tabpage_arg(eap);
    जइ (eap->errmsg == रिक्त)
	tabpage_move(tab_सङ्ख्या);
}

/*
 * :tabs command: List tabs and their contents.
 */
    स्थिर शून्य
ex_tabs(exarg_T *eap UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp;
    पूर्णाङ्क		tabcount = 1;

    सन्देशः_start();
    सन्देशः_scroll = TRUE;
    प्रति (tp = first_tabpage; tp != रिक्त && !got_int; tp = tp->tp_next)
    {
	सन्देशः_putchar('\n');
	vim_snprintf((वर्ण *)IObuff, IOSIZE, _("Tab page %d"), tabcount++);
	सन्देशः_outtrans_attr(IObuff, HL_ATTR(HLF_T));
	out_flush();	    // output one line at a time
	ui_breakcheck();

	जइ (tp  == curtab)
	    wp = firstwin;
	अण्णहा
	    wp = tp->tp_firstwin;
	प्रति ( ; wp != रिक्त && !got_int; wp = wp->w_next)
	{
	    सन्देशः_putchar('\n');
	    सन्देशः_putchar(wp == curwin ? '>' : ' ');
	    सन्देशः_putchar(' ');
	    सन्देशः_putchar(bufIsChanged(wp->w_buffer) ? '+' : ' ');
	    सन्देशः_putchar(' ');
	    जइ (buf_spname(wp->w_buffer) != रिक्त)
		vim_strncpy(IObuff, buf_spname(wp->w_buffer), IOSIZE - 1);
	    अण्णहा
		home_replace(wp->w_buffer, wp->w_buffer->b_fname,
							IObuff, IOSIZE, TRUE);
	    सन्देशः_outtrans(IObuff);
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
    जइ (*eap->arg == NUL)
	shell_resized();
    अण्णहा
	emsg(_(e_screen_mode_setting_not_supported));
}

/*
 * ":resize".
 * set, increment or decrement current window height
 */
    स्थिर शून्य
ex_resize(exarg_T *eap)
{
    पूर्णाङ्क		n;
    win_T	*wp = curwin;

    जइ (eap->addr_गणनम् > 0)
    {
	n = eap->line2;
	प्रति (wp = firstwin; wp->w_next != रिक्त && --n > 0; wp = wp->w_next)
	    ;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    n = atol((वर्ण *)eap->arg);
    जइ (cmdmod.cmod_split & WSP_VERT)
    {
	जइ (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_width;
	अण्णहा जइ (n == 0 && eap->arg[0] == NUL)	// default is very wide
	    n = 9999;
	win_setwidth_win(n, wp);
    }
    अण्णहा
    {
	जइ (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_height;
	अण्णहा जइ (n == 0 && eap->arg[0] == NUL)	// default is very high
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
    जइ (!check_can_set_curbuf_forceit(eap->forceit))
	प्रत्यागम;

    char_u	*fname = रिक्त;
    पूर्णाङ्क		गणनम्;
    char_u	*file_to_find = रिक्त;
    वर्ण	*अन्वेषणम्_ctx = रिक्त;

    जइ (*get_findfunc() != NUL)
    {
#ifdef FEAT_EVAL
	fname = findfunc_find_file(eap->arg, (पूर्णाङ्क)STRLEN(eap->arg),
					eap->addr_गणनम् > 0 ? eap->line2 : 1);
#endif
    }
    अण्णहा
    {
	fname = find_file_in_path(eap->arg, (पूर्णाङ्क)STRLEN(eap->arg), FNAME_MESS,
			       TRUE, curbuf->b_ffname, &file_to_find, &अन्वेषणम्_ctx);
	जइ (eap->addr_गणनम् > 0)
	{
	    // Repeat finding the file "count" times.  This matters when it appears
	    // several times in the path.
	    गणनम् = eap->line2;
	    जाव (fname != रिक्त && --गणनम् > 0)
	    {
		vim_free(fname);
		fname = find_file_in_path(रिक्त, 0, FNAME_MESS,
			      FALSE, curbuf->b_ffname, &file_to_find, &अन्वेषणम्_ctx);
	    }
	}
	VIM_CLEAR(file_to_find);
	vim_findfile_cleanup(अन्वेषणम्_ctx);
    }

    जइ (fname == रिक्त)
	प्रत्यागम;

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
    जइ (not_in_vim9(eap, false) == FAIL)
	प्रत्यागम;
#endif
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_SOL | BL_FIX);
    जइ (*eap->arg == '/')
    {
	// ":open /pattern/": put cursor in column found with pattern
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg, magic_isset() ? RE_MAGIC : 0);
	जइ (regmatch.regprog != रिक्त)
	{
	    // make a copy of the line, when searching for a mark it might be
	    // flushed
	    char_u *पङ्क्तिः = vim_strsave(ml_get_curline());

	    regmatch.rm_ic = p_ic;
	    जइ (vim_regexec(&regmatch, पङ्क्तिः, (colnr_T)0))
		curwin->w_cursor.col = (colnr_T)(regmatch.startp[0] - पङ्क्तिः);
	    अण्णहा
		emsg(_(e_no_match));
	    vim_regfree(regmatch.regprog);
	    vim_free(पङ्क्तिः);
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
    जइ (
	    eap->cmdidx != CMD_badd
	    && eap->cmdidx != CMD_balt
	    // All other commands must obey 'winfixbuf' / ! rules
	    && (is_other_file(0, ffname) && !check_can_set_curbuf_forceit(eap->forceit))
    )
	प्रत्यागम;

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
    पूर्णाङ्क		n;
    पूर्णाङ्क		need_hide;
    पूर्णाङ्क		exmode_was = exmode_active;

    जइ ((eap->cmdidx != CMD_pedit && ERROR_IF_POPUP_WINDOW)
						 || ERROR_IF_TERM_POPUP_WINDOW)
	प्रत्यागम;
    /*
     * ":vi" command ends Ex mode.
     */
    जइ (exmode_active && (eap->cmdidx == CMD_visual
						|| eap->cmdidx == CMD_view))
    {
	exmode_active = FALSE;
	ex_pressedreturn = FALSE;
	जइ (*eap->arg == NUL)
	{
	    // Special case:  ":global/pat/visual\NLvi-commands"
	    जइ (global_busy)
	    {
		जइ (eap->nextcmd != रिक्त)
		{
		    stuffReadbuff(eap->nextcmd);
		    eap->nextcmd = रिक्त;
		}

		जइ (exmode_was != EXMODE_VIM)
		    settmode(TMODE_RAW);
		पूर्णाङ्क save_RedrawingDisabled = RedrawingDisabled;
		RedrawingDisabled = 0;
		पूर्णाङ्क save_nwr = no_wait_return;
		no_wait_return = 0;
		need_wait_return = FALSE;
		पूर्णाङ्क save_ms = सन्देशः_scroll;
		सन्देशः_scroll = 0;
#ifdef FEAT_GUI
		पूर्णाङ्क save_he = hold_gui_events;
		hold_gui_events = 0;
#endif
		set_must_redraw(UPD_CLEAR);
		pending_exmode_active = TRUE;

		main_loop(FALSE, TRUE);

		pending_exmode_active = FALSE;
		RedrawingDisabled = save_RedrawingDisabled;
		no_wait_return = save_nwr;
		सन्देशः_scroll = save_ms;
#ifdef FEAT_GUI
		hold_gui_events = save_he;
#endif
	    }
	    प्रत्यागम;
	}
    }

    जइ ((eap->cmdidx == CMD_new
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
    अण्णहा जइ ((eap->cmdidx != CMD_split && eap->cmdidx != CMD_vsplit)
	    || *eap->arg != NUL
#ifdef FEAT_BROWSE
	    || (cmdmod.cmod_flags & CMOD_BROWSE)
#endif
	    )
    {
	// Can't edit another file when "textlock" or "curbuf_lock" is set.
	// Only ":edit" or ":script" can bring us here, others are stopped
	// earlier.
	जइ (*eap->arg != NUL && text_or_buf_locked())
	    प्रत्यागम;

	n = readonlymode;
	जइ (eap->cmdidx == CMD_view || eap->cmdidx == CMD_sview)
	    readonlymode = TRUE;
	अण्णहा जइ (eap->cmdidx == CMD_enew)
	    readonlymode = FALSE;   // 'readonly' doesn't make sense in an
				    // empty buffer
	जइ (eap->cmdidx != CMD_balt && eap->cmdidx != CMD_badd)
	    setpcmark();
	जइ (do_ecmd(0, (eap->cmdidx == CMD_enew ? रिक्त : eap->arg),
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
	    जइ (old_curwin != रिक्त)
	    {
		need_hide = (curbufIsChanged() && curbuf->b_nwindows <= 1);
		जइ (!need_hide || buf_hide(curbuf))
		{
#जइ defined(FEAT_EVAL)
		    cleanup_T   cs;

		    // Reset the error/interrupt/exception state here so that
		    // aborting() returns FALSE when closing a window.
		    enter_cleanup(&cs);
#endif
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_close(curwin, !need_hide && !buf_hide(curbuf));

#जइ defined(FEAT_EVAL)
		    // Restore the error/interrupt/exception state if not
		    // discarded by a new aborting error, interrupt, or
		    // uncaught exception.
		    leave_cleanup(&cs);
#endif
		}
	    }
	}
	अण्णहा जइ (readonlymode && curbuf->b_nwindows == 1)
	{
	    // When editing an already visited buffer, 'readonly' won't be set
	    // but the previous value is kept.  With ":view" and ":sview" we
	    // want the  file to be readonly, except when another window is
	    // editing the same buffer.
	    curbuf->b_p_ro = TRUE;
	}
	readonlymode = n;
    }
    अण्णहा
    {
	जइ (eap->do_ecmd_cmd != रिक्त)
	    do_cmd_argument(eap->do_ecmd_cmd);
	n = curwin->w_arg_idx_invalid;
	check_arg_idx(curwin);
	जइ (n != curwin->w_arg_idx_invalid)
	    maketitle();
    }

    /*
     * if ":split file" worked, set alternate file name in old window to new
     * file
     */
    जइ (old_curwin != रिक्त
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

#जइ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
    स्थिर शून्य
ex_tearoff(exarg_T *eap)
{
    gui_make_tearoff(eap->arg);
}
#endif

#जइ (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
    स्थिर शून्य
ex_popup(exarg_T *eap)
{
# जइ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    जइ (gui.in_use)
	gui_make_popup(eap->arg, eap->forceit);
#  ifdef FEAT_TERM_POPUP_MENU
    अण्णहा
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
    जइ (curbuf->b_ml.ml_mfp == रिक्त || curbuf->b_ml.ml_mfp->mf_fname == रिक्त)
	सन्देशः(_("No swap file"));
    अण्णहा
	सन्देशः((वर्ण *)curbuf->b_ml.ml_mfp->mf_fname);
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
    long	topline;
    long	y;
    linenr_T	old_linenr = curwin->w_cursor.lnum;

    setpcmark();

    /*
     * determine max topline
     */
    जइ (curwin->w_p_scb)
    {
	topline = curwin->w_topline;
	FOR_ALL_WINDOWS(wp)
	{
	    जइ (wp->w_p_scb && wp->w_buffer)
	    {
		y = wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम् - get_scrolloff_मानम्();
		जइ (topline > y)
		    topline = y;
	    }
	}
	जइ (topline < 1)
	    topline = 1;
    }
    अण्णहा
    {
	topline = 1;
    }


    /*
     * Set all scrollbind windows to the same topline.
     */
    FOR_ALL_WINDOWS(curwin)
    {
	जइ (curwin->w_p_scb)
	{
	    curbuf = curwin->w_buffer;
	    y = topline - curwin->w_topline;
	    जइ (y > 0)
		scrollup(y, TRUE);
	    अण्णहा
		scrolldown(-y, TRUE);
	    curwin->w_scbind_pos = topline;
	    redraw_later(UPD_VALID);
	    cursor_correct();
	    curwin->w_redr_status = true;
	}
    }
    curwin = save_curwin;
    curbuf = save_curbuf;
    जइ (curwin->w_p_scb)
    {
	did_syncbind = TRUE;
	checkpcmark();
	जइ (old_linenr != curwin->w_cursor.lnum)
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
    पूर्णाङ्क		i;
    पूर्णाङ्क		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum;

    जइ (eap->usefilter)			// :r!cmd
    {
	do_bang(1, eap, FALSE, FALSE, TRUE);
	प्रत्यागम;
    }

    जइ (u_save(eap->line2, (linenr_T)(eap->line2 + 1)) == FAIL)
	प्रत्यागम;

#ifdef FEAT_BROWSE
    जइ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browseFile;

	browseFile = do_browse(0, (char_u *)_("Append File"), eap->arg,
		रिक्त, रिक्त, रिक्त, curbuf);
	जइ (browseFile != रिक्त)
	{
	    i = readfile(browseFile, रिक्त,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	    vim_free(browseFile);
	}
	अण्णहा
	    i = OK;
    }
    अण्णहा
#endif
	जइ (*eap->arg == NUL)
	{
	    जइ (check_fname() == FAIL)	// check for no file name
		प्रत्यागम;
	    i = readfile(curbuf->b_ffname, curbuf->b_fname,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	}
	अण्णहा
	{
	    जइ (vim_strchr(p_cpo, CPO_ALTREAD) != रिक्त)
		(शून्य)setaltfname(eap->arg, eap->arg, (linenr_T)1);
	    i = readfile(eap->arg, रिक्त,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);

	}
    जइ (i != OK)
    {
#जइ defined(FEAT_EVAL)
	जइ (!aborting())
#endif
	    semsg(_(e_cant_open_file_str), eap->arg);
    }
    अण्णहा
    {
	जइ (empty && exmode_active)
	{
	    // Delete the empty line that remains.  Historically ex does
	    // this but vi doesn't.
	    जइ (eap->line2 == 0)
		lnum = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
	    अण्णहा
		lnum = 1;
	    जइ (*ml_get(lnum) == NUL && u_savedel(lnum, 1L) == OK)
	    {
		ml_delete(lnum);
		जइ (curwin->w_cursor.lnum > 1
			&& curwin->w_cursor.lnum >= lnum)
		    --curwin->w_cursor.lnum;
		deleted_पङ्क्तयः_mark(lnum, 1L);
	    }
	}
	redraw_curbuf_later(UPD_VALID);
    }
}

स्थिर char_u	*prev_dir = रिक्त;

#जइ defined(EXITFREE)
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
    जइ (scope == CDSCOPE_WINDOW)
	प्रत्यागम curwin->w_prevdir;
    अण्णहा जइ (scope == CDSCOPE_TABPAGE)
	प्रत्यागम curtab->tp_prevdir;
    प्रत्यागम prev_dir;
}

/*
 * Deal with the side effects of changing the current directory.
 * When 'scope' is CDSCOPE_TABPAGE then this was after an ":tcd" command.
 * When 'scope' is CDSCOPE_WINDOW then this was after an ":lcd" command.
 */
    शून्य
post_chdir(cdscope_T scope)
{
    जइ (scope != CDSCOPE_WINDOW)
	// Clear tab local directory for both :cd and :tcd
	VIM_CLEAR(curtab->tp_localdir);
    VIM_CLEAR(curwin->w_localdir);
    जइ (scope != CDSCOPE_GLOBAL)
    {
	char_u	*pdir = get_prevdir(scope);

	// If still in the global directory, need to remember current
	// directory as the global directory.
	जइ (globaldir == रिक्त && pdir != रिक्त)
	    globaldir = vim_strsave(pdir);

	// Remember this local directory for the window.
	जइ (mch_dirname(NameBuff, MAXPATHL) == OK)
	{
	    जइ (scope == CDSCOPE_TABPAGE)
		curtab->tp_localdir = vim_strsave(NameBuff);
	    अण्णहा
		curwin->w_localdir = vim_strsave(NameBuff);
	}
    }
    अण्णहा
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
    dict_set_वस्तूनि_ro(v_event);
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
    पूर्णाङ्क
changedir_func(
	char_u		*new_dir,
	पूर्णाङ्क		forceit,
	cdscope_T	scope)
{
    char_u	*pdir = रिक्त;
    पूर्णाङ्क		dir_differs;
    char_u	*acmd_fname = रिक्त;
    char_u	**pp;
    char_u	*tofree;

    जइ (new_dir == रिक्त || allbuf_locked())
	प्रत्यागम FALSE;

    जइ (vim_strchr(p_cpo, CPO_CHDIR) != रिक्त && curbufIsChanged() && !forceit)
    {
	emsg(_(e_cannot_change_directory_buffer_is_modified_add_bang_to_override));
	प्रत्यागम FALSE;
    }

    // ":cd -": Change to previous directory
    जइ (STRCMP(new_dir, "-") == 0)
    {
	pdir = get_prevdir(scope);
	जइ (pdir == रिक्त)
	{
	    emsg(_(e_no_previous_directory));
	    प्रत्यागम FALSE;
	}
	new_dir = pdir;
    }

    // Save current directory for next ":cd -"
    जइ (mch_dirname(NameBuff, MAXPATHL) == OK)
	pdir = vim_strsave(NameBuff);
    अण्णहा
	pdir = रिक्त;

    // For UNIX ":cd" means: go to home directory.
    // On other systems too if 'cdhome' is set.
#जइ defined(UNIX) || defined(VMS)
    जइ (*new_dir == NUL)
#अण्णहा
    जइ (*new_dir == NUL && p_cdh)
#endif
    {
	// use NameBuff for home directory name
#ifdef VMS
	char_u	*p;

	p = mch_getenv((char_u *)"SYS$LOGIN");
	जइ (p == रिक्त || *p == NUL)	// empty is the same as not set
	    NameBuff[0] = NUL;
	अण्णहा
	    vim_strncpy(NameBuff, p, MAXPATHL - 1);
#अण्णहा
	expand_env((char_u *)"$HOME", NameBuff, MAXPATHL);
#endif
	new_dir = NameBuff;
    }
    dir_differs = pdir == रिक्त
			    || pathcmp((वर्ण *)pdir, (वर्ण *)new_dir, -1) != 0;
    जइ (dir_differs)
    {
	जइ (scope == CDSCOPE_WINDOW)
	    acmd_fname = (char_u *)"window";
	अण्णहा जइ (scope == CDSCOPE_TABPAGE)
	    acmd_fname = (char_u *)"tabpage";
	अण्णहा
	    acmd_fname = (char_u *)"global";
	trigger_DirChangedPre(acmd_fname, new_dir);

	जइ (vim_chdir(new_dir))
	{
	    emsg(_(e_command_failed));
	    vim_free(pdir);
	    प्रत्यागम FALSE;
	}
    }

    जइ (scope == CDSCOPE_WINDOW)
	pp = &curwin->w_prevdir;
    अण्णहा जइ (scope == CDSCOPE_TABPAGE)
	pp = &curtab->tp_prevdir;
    अण्णहा
	pp = &prev_dir;
    tofree = *pp;  // new_dir may use this
    *pp = pdir;

    post_chdir(scope);

    जइ (dir_differs)
	apply_autocmds(EVENT_DIRCHANGED, acmd_fname, new_dir, FALSE, curbuf);
    vim_free(tofree);
    प्रत्यागम TRUE;
}

/*
 * ":cd", ":tcd", ":lcd", ":chdir" ":tchdir" and ":lchdir".
 */
    शून्य
ex_cd(exarg_T *eap)
{
    char_u	*new_dir;

    new_dir = eap->arg;
#जइ !defined(UNIX) && !defined(VMS)
    // for non-UNIX ":cd" means: print current directory unless 'cdhome' is set
    जइ (*new_dir == NUL && !p_cdh)
    {
	ex_pwd(रिक्त);
	प्रत्यागम;
    }
#endif

    cdscope_T	scope = CDSCOPE_GLOBAL;

    जइ (eap->cmdidx == CMD_lcd || eap->cmdidx == CMD_lchdir)
	scope = CDSCOPE_WINDOW;
    अण्णहा जइ (eap->cmdidx == CMD_tcd || eap->cmdidx == CMD_tchdir)
	scope = CDSCOPE_TABPAGE;

    जइ (changedir_func(new_dir, eap->forceit, scope))
    {
	// Echo the new current directory if the command was typed.
	जइ (कुञ्चिका_Typed || p_verbose >= 5)
	    ex_pwd(eap);
    }
}

/*
 * ":pwd".
 */
    स्थिर शून्य
ex_pwd(exarg_T *eap UNUSED)
{
    जइ (mch_dirname(NameBuff, MAXPATHL) == OK)
    {
#ifdef BACKSLASH_IN_FILENAME
	slash_adjust(NameBuff);
#endif
	जइ (p_verbose > 0)
	{
	    वर्ण *context = "global";

	    जइ (last_chdir_reason != रिक्त)
		context = last_chdir_reason;
	    अण्णहा जइ (curwin->w_localdir != रिक्त)
		context = "window";
	    अण्णहा जइ (curtab->tp_localdir != रिक्त)
		context = "tabpage";
	    smsg("[%s] %s", context, (वर्ण *)NameBuff);
	}
	अण्णहा
	    सन्देशः((वर्ण *)NameBuff);
    }
    अण्णहा
	emsg(_(e_directory_unknown));
}

/*
 * ":=".
 */
    स्थिर शून्य
ex_equal(exarg_T *eap)
{
    smsg("%ld", (long)eap->line2);
    ex_may_print(eap);
}

    स्थिर शून्य
ex_sleep(exarg_T *eap)
{
    पूर्णाङ्क		n;
    long	len;

    जइ (cursor_valid())
    {
	n = W_WINROW(curwin) + curwin->w_wrow - सन्देशः_scrolled;
	जइ (n >= 0)
	    windgoto(n, curwin->w_wincol + curwin->w_wcol);
    }

    len = eap->line2;
    विकल्प (*eap->arg)
    {
	पक्ष 'm': भङ्ग;
	पक्ष NUL: len *= 1000L; भङ्ग;
	स्वभाव: semsg(_(e_invalid_argument_str), eap->arg); प्रत्यागम;
    }

    // Hide the cursor if invoked with !
    do_sleep(len, eap->forceit);
}

/*
 * Sleep for "msec" milliseconds, but keep checking for a CTRL-C every second.
 * Hide the cursor if "hide_cursor" is TRUE.
 */
    शून्य
do_sleep(long msec, पूर्णाङ्क hide_cursor)
{
    long	done = 0;
    long	wait_now;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    // Remember at what time we started, so that we know how much longer we
    // should wait after waiting for a bit.
    ELAPSED_INIT(start_tv);
#endif

    जइ (hide_cursor)
	cursor_sleep();
    अण्णहा
	cursor_on();

    out_flush_cursor(FALSE, FALSE);
    जाव (!got_int && done < msec)
    {
	wait_now = msec - done > 1000L ? 1000L : msec - done;
#ifdef FEAT_TIMERS
	{
	    long    due_time = check_due_timer();

	    जइ (due_time > 0 && due_time < wait_now)
		wait_now = due_time;
	}
#endif
#ifdef FEAT_JOB_CHANNEL
	जइ (has_any_channel() && wait_now > 20L)
	    wait_now = 20L;
#endif
#ifdef FEAT_SOUND
	जइ (has_any_sound_callback() && wait_now > 20L)
	    wait_now = 20L;
#endif
	ui_delay(wait_now, TRUE);

#ifdef FEAT_JOB_CHANNEL
	जइ (has_any_channel())
	    ui_breakcheck_force(TRUE);
	अण्णहा
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
#अण्णहा
	// guesstimate time passed (will actually be more)
	done += wait_now;
#endif
    }

    // If CTRL-C was typed to interrupt the sleep, drop the CTRL-C from the
    // input buffer, otherwise a following call to input() fails.
    जइ (got_int)
	(शून्य)vpeekc();

    जइ (hide_cursor)
	cursor_unsleep();
}

/*
 * ":winsize" command (obsolete).
 */
    स्थिर शून्य
ex_winsize(exarg_T *eap)
{
    पूर्णाङ्क		w, h;
    char_u	*arg = eap->arg;
    char_u	*p;

    जइ (!SAFE_isdigit(*arg))
    {
	semsg(_(e_invalid_argument_str), arg);
	प्रत्यागम;
    }
    w = getdigits(&arg);
    arg = skipwhite(arg);
    p = arg;
    h = getdigits(&arg);
    जइ (*p != NUL && *arg == NUL)
	set_shellsize(w, h, TRUE);
    अण्णहा
	emsg(_(e_winsize_requires_two_सङ्ख्या_arguments));
}

    स्थिर शून्य
ex_wincmd(exarg_T *eap)
{
    पूर्णाङ्क		xchar = NUL;
    char_u	*p;

    जइ (*eap->arg == 'g' || *eap->arg == Ctrl_G)
    {
	// CTRL-W g and CTRL-W CTRL-G  have an extra command character
	जइ (eap->arg[1] == NUL)
	{
	    emsg(_(e_invalid_argument));
	    प्रत्यागम;
	}
	xchar = eap->arg[1];
	p = eap->arg + 2;
    }
    अण्णहा
	p = eap->arg + 1;

    set_nextcmd(eap, p);
    p = skipwhite(p);
    जइ (*p != NUL && *p != (
#ifdef FEAT_EVAL
	    in_vim9script() ? '#' :
#endif
		'"')
	    && eap->nextcmd == रिक्त)
	emsg(_(e_invalid_argument));
    अण्णहा जइ (!eap->skip)
    {
	// Pass flags on for ":vertical wincmd ]".
	postponed_split_flags = cmdmod.cmod_split;
	postponed_split_tab = cmdmod.cmod_tab;
	do_window(*eap->arg, eap->addr_गणनम् > 0 ? eap->line2 : 0L, xchar);
	postponed_split_flags = 0;
	postponed_split_tab = 0;
    }
}

#जइ defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * ":winpos".
 */
    स्थिर शून्य
ex_winpos(exarg_T *eap)
{
    पूर्णाङ्क		x, y;
    char_u	*arg = eap->arg;
    char_u	*p;

    जइ (*arg == NUL)
    {
# जइ defined(FEAT_GUI) || defined(MSWIN)
#  ifdef VIMDLL
	जइ (gui.in_use ? gui_mch_get_winpos(&x, &y) != FAIL :
		mch_get_winpos(&x, &y) != FAIL)
#  elif defined(FEAT_GUI)
	जइ (gui.in_use && gui_mch_get_winpos(&x, &y) != FAIL)
#  अण्णहा
	जइ (mch_get_winpos(&x, &y) != FAIL)
#  endif
	{
	    sprintf((वर्ण *)IObuff, _("Window position: X %d, Y %d"), x, y);
	    सन्देशः((वर्ण *)IObuff);
	}
	अण्णहा
# endif
	    emsg(_(e_obtaining_window_position_not_implemented_for_this_platform));
    }
    अण्णहा
    {
	x = getdigits(&arg);
	arg = skipwhite(arg);
	p = arg;
	y = getdigits(&arg);
	जइ (*p == NUL || *arg != NUL)
	{
	    emsg(_(e_winpos_requires_two_सङ्ख्या_arguments));
	    प्रत्यागम;
	}
# ifdef FEAT_GUI
	जइ (gui.in_use)
	    gui_mch_set_winpos(x, y);
	अण्णहा जइ (gui.starting)
	{
	    // Remember the coordinates for when the window is opened.
	    gui_win_x = x;
	    gui_win_y = y;
	}
#  जइ defined(HAVE_TGETENT) || defined(VIMDLL)
	अण्णहा
#  endif
# endif
# जइ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	    mch_set_winpos(x, y);
# endif
# ifdef HAVE_TGETENT
	जइ (*T_CWP)
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
    oa.पङ्क्तिः_गणनम् = eap->line2 - eap->line1 + 1;
    oa.motion_type = MLINE;
    virtual_op = FALSE;
    जइ (eap->cmdidx != CMD_yank)	// position cursor for undo
    {
	setpcmark();
	curwin->w_cursor.lnum = eap->line1;
	beginline(BL_SOL | BL_FIX);
    }

    जइ (VIsual_active)
	end_visual_mode();

    विकल्प (eap->cmdidx)
    {
	पक्ष CMD_delete:
	    oa.op_type = OP_DELETE;
	    op_delete(&oa);
	    भङ्ग;

	पक्ष CMD_yank:
	    oa.op_type = OP_YANK;
	    (शून्य)op_yank(&oa, FALSE, TRUE);
	    भङ्ग;

	स्वभाव:    // CMD_rshift or CMD_lshift
	    जइ (
#ifdef FEAT_RIGHTLEFT
		(eap->cmdidx == CMD_rshift) ^ curwin->w_p_rl
#अण्णहा
		eap->cmdidx == CMD_rshift
#endif
						)
		oa.op_type = OP_RSHIFT;
	    अण्णहा
		oa.op_type = OP_LSHIFT;
	    op_shift(&oa, FALSE, eap->amount);
	    भङ्ग;
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
    जइ (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, रिक्त, eap->forceit ? BACKWARD : FORWARD, 1L,
						       PUT_पङ्क्तिः|PUT_CURSLINE);
}

/*
 * ":iput".
 */
    स्थिर शून्य
ex_iput(exarg_T *eap)
{
    // ":0iput" works like ":1iput!".
    जइ (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, रिक्त, eap->forceit ? BACKWARD : FORWARD, 1L,
						      PUT_पङ्क्तिः|PUT_CURSLINE
						      |PUT_FIXINDENT);
}

/*
 * Handle ":copy" and ":move".
 */
    स्थिर शून्य
ex_copymove(exarg_T *eap)
{
    long	n;

#ifdef FEAT_EVAL
    जइ (not_in_vim9(eap, false) == FAIL)
	प्रत्यागम;
#endif
    n = get_address(eap, &eap->arg, eap->addr_type, FALSE, FALSE, FALSE, 1);
    जइ (eap->arg == रिक्त)	    // error detected
    {
	eap->nextcmd = रिक्त;
	प्रत्यागम;
    }
    get_flags(eap);

    /*
     * move or copy lines from 'eap->line1'-'eap->line2' to below line 'n'
     */
    जइ (n == MAXLNUM || n < 0 || n > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
    {
	emsg(_(e_invalid_range));
	प्रत्यागम;
    }

    जइ (eap->cmdidx == CMD_move)
    {
	जइ (do_move(eap->line1, eap->line2, n) == FAIL)
	    प्रत्यागम;
    }
    अण्णहा
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
    जइ (eap->flags != 0)
    {
	print_पङ्क्तिः(curwin->w_cursor.lnum, (eap->flags & EXFLAG_NR),
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
    जइ (eap->line1 == eap->line2)
    {
	जइ (eap->addr_गणनम् >= 2)   // :2,2join does nothing
	    प्रत्यागम;
	जइ (eap->line2 == curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	{
	    beep_flush();
	    प्रत्यागम;
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
    पूर्णाङ्क		c;
    पूर्णाङ्क		prev_len = typebuf.tb_len;

    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif

    // get the register name.  No name means to use the previous one
    c = *eap->arg;
    जइ (c == NUL || (c == '*' && *eap->cmd == '*'))
	c = '@';
    // Put the register in the typeahead buffer with the "silent" flag.
    जइ (do_execreg(c, TRUE, vim_strchr(p_cpo, CPO_EXECBUF) != रिक्त, TRUE)
								      == FAIL)
    {
	beep_flush();
	प्रत्यागम;
    }

    पूर्णाङ्क	save_efr = exec_from_reg;

    exec_from_reg = TRUE;

    /*
     * Execute from the typeahead buffer.
     * Continue until the stuff buffer is empty and all added characters
     * have been consumed.
     */
    जाव (!stuff_empty() || typebuf.tb_len > prev_len)
	(शून्य)do_cmdline(रिक्त, getexline, रिक्त, DOCMD_NOWAIT|DOCMD_VERBOSE);

    exec_from_reg = save_efr;
}

/*
 * ":!".
 */
    स्थिर शून्य
ex_bang(exarg_T *eap)
{
    do_bang(eap->addr_गणनम्, eap, eap->forceit, TRUE, TRUE);
}

/*
 * ":undo".
 */
    स्थिर शून्य
ex_undo(exarg_T *eap)
{
    जइ (eap->addr_गणनम् == 1)	    // :undo 123
	undo_time(eap->line2, FALSE, FALSE, TRUE);
    अण्णहा
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
    long	गणनम् = 0;
    पूर्णाङ्क		sec = FALSE;
    पूर्णाङ्क		file = FALSE;
    char_u	*p = eap->arg;

    जइ (*p == NUL)
	गणनम् = 1;
    अण्णहा जइ (SAFE_isdigit(*p))
    {
	गणनम् = getdigits(&p);
	विकल्प (*p)
	{
	    पक्ष 's': ++p; sec = TRUE; भङ्ग;
	    पक्ष 'm': ++p; sec = TRUE; गणनम् *= 60; भङ्ग;
	    पक्ष 'h': ++p; sec = TRUE; गणनम् *= 60 * 60; भङ्ग;
	    पक्ष 'd': ++p; sec = TRUE; गणनम् *= 24 * 60 * 60; भङ्ग;
	    पक्ष 'f': ++p; file = TRUE; भङ्ग;
	}
    }

    जइ (*p != NUL)
	semsg(_(e_invalid_argument_str), eap->arg);
    अण्णहा
	undo_time(eap->cmdidx == CMD_earlier ? -गणनम् : गणनम्,
							    sec, file, FALSE);
}

/*
 * ":redir": start/stop redirection.
 */
    स्थिर शून्य
ex_redir(exarg_T *eap)
{
    वर्ण	*mode;
    char_u	*fname;
    char_u	*arg = eap->arg;

#ifdef FEAT_EVAL
    जइ (redir_execute)
    {
	emsg(_(e_cannot_use_redir_inside_execute));
	प्रत्यागम;
    }
#endif

    जइ (STRICMP(eap->arg, "END") == 0)
	close_redir();
    अण्णहा
    {
	जइ (*arg == '>')
	{
	    ++arg;
	    जइ (*arg == '>')
	    {
		++arg;
		mode = "a";
	    }
	    अण्णहा
		mode = "w";
	    arg = skipwhite(arg);

	    close_redir();

	    // Expand environment variables and "~/".
	    fname = expand_env_save(arg);
	    जइ (fname == रिक्त)
		प्रत्यागम;
#ifdef FEAT_BROWSE
	    जइ (cmdmod.cmod_flags & CMOD_BROWSE)
	    {
		char_u	*browseFile;

		browseFile = do_browse(BROWSE_SAVE,
			(char_u *)_("Save Redirection"),
			fname, रिक्त, रिक्त,
			(char_u *)_(BROWSE_FILTER_ALL_FILES), curbuf);
		जइ (browseFile == रिक्त)
		    प्रत्यागम;		// operation cancelled
		vim_free(fname);
		fname = browseFile;
		eap->forceit = TRUE;	// since dialog already asked
	    }
#endif

	    redir_fd = open_exfile(fname, eap->forceit, mode);
	    vim_free(fname);
	}
#ifdef FEAT_EVAL
	अण्णहा जइ (*arg == '@')
	{
	    // redirect to a register a-z (resp. A-Z for appending)
	    close_redir();
	    ++arg;
	    जइ (ASCII_ISALPHA(*arg)
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
		जइ (*arg == '>' && arg[1] == '>')  // append
		    arg += 2;
		अण्णहा
		{
		    // Can use both "@a" and "@a>".
		    जइ (*arg == '>')
			arg++;
		    // Make register empty when not using @A-@Z and the
		    // command is valid.
		    जइ (*arg == NUL && !SAFE_isupper(redir_reg))
			write_reg_contents(redir_reg, (char_u *)"", -1, FALSE);
		}
	    }
	    जइ (*arg != NUL)
	    {
		redir_reg = 0;
		semsg(_(e_invalid_argument_str), eap->arg);
	    }
	}
	अण्णहा जइ (*arg == '=' && arg[1] == '>')
	{
	    पूर्णाङ्क append;

	    // redirect to a variable
	    close_redir();
	    arg += 2;

	    जइ (*arg == '>')
	    {
		++arg;
		append = TRUE;
	    }
	    अण्णहा
		append = FALSE;

	    जइ (var_redir_start(skipwhite(arg), append) == OK)
		redir_vname = 1;
	}
#endif

	// TODO: redirect to a buffer

	अण्णहा
	    semsg(_(e_invalid_argument_str), eap->arg);
    }

    // Make sure redirection is not off.  Can happen for cmdline completion
    // that indirectly invokes a command to catch its output.
    जइ (redir_fd != रिक्त
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
redraw_cmd(पूर्णाङ्क clear)
{
    पूर्णाङ्क save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    पूर्णाङ्क save_p_lz = p_lz;
    p_lz = FALSE;

    validate_cursor();
    update_topline();
    update_screen(clear ? UPD_CLEAR : VIsual_active ? UPD_INVERTED : 0);
    जइ ((State & MODE_CMDLINE) == 0)
	setcursor(); // put cursor back where it belongs
    जइ (need_maketitle)
	maketitle();
#जइ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
    जइ (!gui.in_use)
# endif
	resize_console_buf();
#endif
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;

    // After drawing the statusline screen_attr may still be set.
    screen_stop_highlight();

    // Reset msg_didout, so that a message that's there is overwritten.
    सन्देशः_didout = FALSE;
    सन्देशः_col = 0;

    // No need to wait after an intentional redraw.
    need_wait_return = FALSE;

    // When invoked from a callback or autocmd the command line may be active.
    जइ (State & MODE_CMDLINE)
	redrawcmdline();

    out_flush();
}

/*
 * ":redrawstatus": force redraw of status line(s)
 */
    स्थिर शून्य
ex_redrawstatus(exarg_T *eap UNUSED)
{
    जइ (eap->forceit)
	status_redraw_all();
    अण्णहा
	status_redraw_curbuf();
    redraw_vseps = TRUE;
    जइ (सन्देशः_scrolled && (State & MODE_CMDLINE))
	प्रत्यागम;  // redraw later

    पूर्णाङ्क save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    पूर्णाङ्क save_p_lz = p_lz;
    p_lz = FALSE;

    जइ (State & MODE_CMDLINE)
	redraw_statuslines();
    अण्णहा
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
    पूर्णाङ्क save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    पूर्णाङ्क save_p_lz = p_lz;
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
    पूर्णाङ्क save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    पूर्णाङ्क save_p_lz = p_lz;
    p_lz = FALSE;

#जइ defined(FEAT_TABPANEL)
    draw_tabpanel();
#अण्णहा
    emsg(_(e_cannot_not_support_redrawtabpanel));
#endif

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

    स्थिर शून्य
close_redir(शून्य)
{
    जइ (redir_fd != रिक्त)
    {
	fclose(redir_fd);
	redir_fd = रिक्त;
    }
#ifdef FEAT_EVAL
    redir_reg = 0;
    जइ (redir_vname)
    {
	var_redir_stop();
	redir_vname = 0;
    }
#endif
}

#जइ defined(FEAT_SESSION) || defined(FEAT_EVAL)
    पूर्णाङ्क
vim_mkdir_emsg(char_u *name, पूर्णाङ्क prot UNUSED)
{
    जइ (vim_mkdir(name, prot) != 0)
    {
	semsg(_(e_cannot_create_directory_str), name);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}
#endif

/*
 * Open a file for writing for an Ex command, with some checks.
 * Return file descriptor, or NULL on failure.
 */
    FILE *
open_exfile(
    char_u	*fname,
    पूर्णाङ्क		forceit,
    वर्ण	*mode)	    // "w" for create new file or "a" for append
{
    FILE	*fd;

#ifdef UNIX
    // with Unix it is possible to open a directory
    जइ (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	प्रत्यागम रिक्त;
    }
#endif
    जइ (!forceit && *mode != 'a' && vim_fexists(fname))
    {
	semsg(_(e_str_exists_add_bang_to_override), fname);
	प्रत्यागम रिक्त;
    }

    जइ ((fd = mch_fopen((वर्ण *)fname, mode)) == रिक्त)
	semsg(_(e_cannot_open_str_for_writing_2), fname);

    प्रत्यागम fd;
}

/*
 * ":mark" and ":k".
 */
    स्थिर शून्य
ex_mark(exarg_T *eap)
{
    pos_T	pos;

#ifdef FEAT_EVAL
    जइ (not_in_vim9(eap, false) == FAIL)
	प्रत्यागम;
#endif
    जइ (*eap->arg == NUL)		// No argument?
    {
	emsg(_(e_argument_required));
	प्रत्यागम;
    }

    जइ (eap->arg[1] != NUL)	// more than one character?
    {
	semsg(_(e_trailing_characters_str), eap->arg);
	प्रत्यागम;
    }

    pos = curwin->w_cursor;		// save curwin->w_cursor
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_WHITE | BL_FIX);
    जइ (setmark(*eap->arg) == FAIL)	// set mark
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
    जइ (!curwin->w_p_wrap)
	validate_cursor();
    update_curswant();
}

/*
 * Save the current State and go to Normal mode.
 * Return TRUE if the typeahead could be saved.
 */
    पूर्णाङ्क
save_current_state(save_state_T *sst)
{
    sst->save_सन्देशः_scroll = सन्देशः_scroll;
    sst->save_restart_edit = restart_edit;
    sst->save_सन्देशः_didout = सन्देशः_didout;
    sst->save_State = State;
    sst->save_insertmode = p_im;
    sst->save_finish_op = finish_op;
    sst->save_opcount = opcount;
    sst->save_reg_executing = reg_executing;
    sst->save_pending_end_reg_executing = pending_end_reg_executing;

    सन्देशः_scroll = FALSE;		    // no msg scrolling in Normal mode
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
    प्रत्यागम sst->tabuf.typebuf_valid;
}

    शून्य
restore_current_state(save_state_T *sst)
{
    // Restore the previous typeahead.
    restore_typeahead(&sst->tabuf, FALSE);

    सन्देशः_scroll = sst->save_सन्देशः_scroll;
    restart_edit = sst->save_restart_edit;
    p_im = sst->save_insertmode;
    finish_op = sst->save_finish_op;
    opcount = sst->save_opcount;
    reg_executing = sst->save_reg_executing;
    pending_end_reg_executing = sst->save_pending_end_reg_executing;
    सन्देशः_didout |= sst->save_सन्देशः_didout;	// don't reset msg_didout now
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
    पूर्णाङ्क		l;
    char_u	*p;

    जइ (ex_normal_lock > 0)
    {
	emsg(_(e_not_allowed_here));
	प्रत्यागम;
    }
    जइ (ex_normal_busy >= p_mmd)
    {
	emsg(_(e_recursive_use_of_normal_too_deep));
	प्रत्यागम;
    }

    /*
     * vgetc() expects a CSI and K_SPECIAL to have been escaped.  Don't do
     * this for the K_SPECIAL leading byte, otherwise special keys will not
     * work.
     */
    जइ (has_mbyte)
    {
	पूर्णाङ्क	len = 0;

	// Count the number of characters to be escaped.
	प्रति (p = eap->arg; *p != NUL; ++p)
	{
#ifdef FEAT_GUI
	    जइ (*p == CSI)  // leadbyte CSI
		len += 2;
#endif
	    प्रति (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		जइ (*++p == K_SPECIAL	  // trailbyte K_SPECIAL or CSI
#ifdef FEAT_GUI
			|| *p == CSI
#endif
			)
		    len += 2;
	}
	जइ (len > 0)
	{
	    arg = alloc(STRLEN(eap->arg) + len + 1);
	    जइ (arg != रिक्त)
	    {
		len = 0;
		प्रति (p = eap->arg; *p != NUL; ++p)
		{
		    arg[len++] = *p;
#ifdef FEAT_GUI
		    जइ (*p == CSI)
		    {
			arg[len++] = KS_EXTRA;
			arg[len++] = (पूर्णाङ्क)KE_CSI;
		    }
#endif
		    प्रति (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		    {
			arg[len++] = *++p;
			जइ (*p == K_SPECIAL)
			{
			    arg[len++] = KS_SPECIAL;
			    arg[len++] = KE_FILLER;
			}
#ifdef FEAT_GUI
			अण्णहा जइ (*p == CSI)
			{
			    arg[len++] = KS_EXTRA;
			    arg[len++] = (पूर्णाङ्क)KE_CSI;
			}
#endif
		    }
		    arg[len] = NUL;
		}
	    }
	}
    }

    ++ex_normal_busy;
    जइ (save_current_state(&save_state))
    {
	/*
	 * Repeat the :normal command for each line in the range.  When no
	 * range given, execute it just once, without positioning the cursor
	 * first.
	 */
	करेहि
	{
	    जइ (eap->addr_गणनम् != 0)
	    {
		curwin->w_cursor.lnum = eap->line1++;
		curwin->w_cursor.col = 0;
		check_cursor_moved(curwin);
	    }

	    exec_normal_cmd(arg != रिक्त
		     ? arg
		     : eap->arg, eap->forceit ? REMAP_NONE : REMAP_YES, FALSE);
	}
	जाव (eap->addr_गणनम् > 0 && eap->line1 <= eap->line2 && !got_int);
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
    जइ (term_job_running(curbuf->b_term))
	प्रत्यागम;
#endif
    जइ (!curbuf->b_p_ma && !p_im)
    {
	// Only give this error when 'insertmode' is off.
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	प्रत्यागम;
    }
    जइ (eap->forceit)
    {
	// cursor line can be zero on startup
	जइ (!curwin->w_cursor.lnum)
	    curwin->w_cursor.lnum = 1;
	set_cursor_for_append_to_पङ्क्तिः();
    }

    // Ignore the command when already in Insert mode.  Inserting an
    // expression register that invokes a function can do this.
    जइ (State & MODE_निवेशय)
	प्रत्यागम;

    जइ (eap->cmdidx == CMD_startinsert)
	restart_edit = 'a';
    अण्णहा जइ (eap->cmdidx == CMD_startreplace)
	restart_edit = 'R';
    अण्णहा
	restart_edit = 'V';

    जइ (!eap->forceit)
    {
	जइ (eap->cmdidx == CMD_startinsert)
	    restart_edit = 'i';
	curwin->w_curswant = 0;	    // avoid MAXCOL
    }

    जइ (VIsual_active)
	showmode();
}

/*
 * ":stopinsert"
 */
    स्थिर शून्य
ex_stopinsert(exarg_T *eap UNUSED)
{
    restart_edit = 0;
    stop_निवेशय_mode = TRUE;
#जइ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // when called from remote_expr in insert mode, make sure insert mode is
    // ended by adding K_NOP to the typeahead buffer
    जइ (vgetc_busy)
	ins_char_typebuf(K_NOP, 0);
#endif
    clearmode();
}

/*
 * Execute normal mode command "cmd".
 * "remap" can be REMAP_NONE or REMAP_YES.
 */
    शून्य
exec_normal_cmd(char_u *cmd, पूर्णाङ्क remap, पूर्णाङ्क silent)
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
exec_normal(पूर्णाङ्क was_typed, पूर्णाङ्क use_vpeekc, पूर्णाङ्क may_use_terminal_loop UNUSED)
{
    oparg_T	oa;
    पूर्णाङ्क		c;

    // When calling vpeekc() from feedkeys() it will return Ctrl_C when there
    // is nothing to get, so also check for Ctrl_C.
    clear_oparg(&oa);
    finish_op = FALSE;
    जाव ((!stuff_empty()
		|| ((was_typed || !typebuf_typed()) && typebuf.tb_len > 0)
		|| (use_vpeekc && (c = vpeekc()) != NUL && c != Ctrl_C))
	    && !got_int)
    {
	update_topline_cursor();
#ifdef FEAT_TERMINAL
	जइ (may_use_terminal_loop && term_use_loop()
		&& oa.op_type == OP_NOP && oa.regname == NUL
		&& !VIsual_active)
	{
	    // If terminal_loop() returns OK we got a key that is handled
	    // in Normal model.  With FAIL we first need to position the
	    // cursor and the screen needs to be redrawn.
	    जइ (terminal_loop(TRUE) == OK)
		normal_cmd(&oa, TRUE);
	}
	अण्णहा
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

# जइ defined(FEAT_QUICKFIX)
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
    पूर्णाङ्क		whole = TRUE;
    long	n;
    char_u	*p;
    पूर्णाङ्क		action;

    विकल्प (cmdnames[eap->cmdidx].cmd_name[2])
    {
	पक्ष 'e':	// ":psearch", ":isearch" and ":dsearch"
		जइ (cmdnames[eap->cmdidx].cmd_name[0] == 'p')
		    action = ACTION_GOTO;
		अण्णहा
		    action = ACTION_SHOW;
		भङ्ग;
	पक्ष 'i':	// ":ilist" and ":dlist"
		action = ACTION_SHOW_ALL;
		भङ्ग;
	पक्ष 'u':	// ":ijump" and ":djump"
		action = ACTION_GOTO;
		भङ्ग;
	स्वभाव:	// ":isplit" and ":dsplit"
		action = ACTION_SPLIT;
		भङ्ग;
    }

    n = 1;
    जइ (vim_isdigit(*eap->arg))	// get count
    {
	n = getdigits(&eap->arg);
	eap->arg = skipwhite(eap->arg);
    }
    जइ (*eap->arg == '/')   // Match regexp, not just whole words
    {
	whole = FALSE;
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	जइ (*p)
	{
	    *p++ = NUL;
	    p = skipwhite(p);

	    // Check for trailing illegal characters
	    जइ (!ends_excmd2(eap->arg, p))
		eap->errmsg = ex_errmsg(e_trailing_characters_str, p);
	    अण्णहा
		set_nextcmd(eap, p);
	}
    }
    जइ (!eap->skip)
	find_pattern_in_path(eap->arg, 0, (पूर्णाङ्क)STRLEN(eap->arg),
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
    जइ (ERROR_IF_ANY_POPUP_WINDOW)
	प्रत्यागम;

    // Open the preview window or popup and make it the current window.
    g_do_tagpreview = p_pvh;
    prepare_tagpreview(TRUE, TRUE, FALSE);
}

    स्थिर शून्य
back_to_current_window(win_T *curwin_save)
{
    जइ (curwin != curwin_save && win_valid(curwin_save))
    {
	// Return cursor to where we were
	validate_cursor();
	redraw_later(UPD_VALID);
	win_enter(curwin_save, TRUE);
    }
# ifdef FEAT_PROP_POPUP
    अण्णहा जइ (WIN_IS_POPUP(curwin))
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
    पूर्णाङ्क		cmd;

    विकल्प (name[1])
    {
	पक्ष 'j': cmd = DT_JUMP;	// ":tjump"
		  भङ्ग;
	पक्ष 's': cmd = DT_SELECT;	// ":tselect"
		  भङ्ग;
	पक्ष 'p': cmd = DT_PREV;	// ":tprevious"
		  भङ्ग;
	पक्ष 'N': cmd = DT_PREV;	// ":tNext"
		  भङ्ग;
	पक्ष 'n': cmd = DT_NEXT;	// ":tnext"
		  भङ्ग;
	पक्ष 'o': cmd = DT_निष्कासय;		// ":pop"
		  भङ्ग;
	पक्ष 'f':			// ":tfirst"
	पक्ष 'r': cmd = DT_FIRST;	// ":trewind"
		  भङ्ग;
	पक्ष 'l': cmd = DT_LAST;	// ":tlast"
		  भङ्ग;
	स्वभाव:			// ":tag"
#ifdef FEAT_CSCOPE
		  जइ (p_cst && *eap->arg != NUL)
		  {
		      ex_cstag(eap);
		      प्रत्यागम;
		  }
#endif
		  cmd = DT_TAG;
		  भङ्ग;
    }

    जइ (name[0] == 'l')
    {
#ifndef FEAT_QUICKFIX
	ex_ni(eap);
	प्रत्यागम;
#अण्णहा
	cmd = DT_LTAG;
#endif
    }

    do_tag(eap->arg, cmd, eap->addr_गणनम् > 0 ? (पूर्णाङ्क)eap->line2 : 1,
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
    SPEC_राशिः,	    // call stack
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
    पूर्णाङ्क
find_cmdline_var(char_u *src, size_t *usedlen)
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
	KEYVALUE_ENTRY(SPEC_राशिः, "stack>")	    // call stack
    };
    keyvalue_T target;
    keyvalue_T *entry;

    विकल्प (*src)
    {
    पक्ष '%':
	*usedlen = 1;
	प्रत्यागम SPEC_PERC;

    पक्ष '#':
	*usedlen = 1;
	प्रत्यागम SPEC_HASH;

    पक्ष '<':
	target.कुञ्चिका = 0;
	target.मानम्.string = src + 1;	    // skip over '<'
	target.मानम्.length = 0;	    // not used, see cmp_keyvalue_value_n()

	entry = (keyvalue_T *)bsearch(&target, &spec_str_tab,
		ARRAY_LENGTH(spec_str_tab), परिमाण(spec_str_tab[0]),
		cmp_keyvalue_मानम्_n);
	जइ (entry == रिक्त)
	    प्रत्यागम -1;

	*usedlen = entry->मानम्.length + 1;
	प्रत्यागम entry->कुञ्चिका;

    स्वभाव:
	भङ्ग;
    }

    प्रत्यागम -1;
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
    वर्ण	**errormsg,	// pointer to error message
    पूर्णाङ्क		*escaped,	// return value has escaped white space (can
				// be NULL)
    पूर्णाङ्क		empty_is_error)	// empty result is considered an error
{
    पूर्णाङ्क		i;
    char_u	*s;
    char_u	*result = (char_u *)"";
    char_u	*resultbuf = रिक्त;
    size_t	resultlen;
    buf_T	*buf;
    पूर्णाङ्क		valid = VALID_HEAD + VALID_PATH;    // assume valid result
    पूर्णाङ्क		spec_idx;
    पूर्णाङ्क		tilde_file = FALSE;
    पूर्णाङ्क		skip_mod = FALSE;
    char_u	strbuf[30];

    *errormsg = रिक्त;
    जइ (escaped != रिक्त)
	*escaped = FALSE;

    /*
     * Check if there is something to do.
     */
    spec_idx = find_cmdline_var(src, usedlen);
    जइ (spec_idx < 0)	// no match
    {
	*usedlen = 1;
	प्रत्यागम रिक्त;
    }

    /*
     * Skip when preceded with a backslash "\%" and "\#".
     * Note: In "\\%" the % is also not recognized!
     */
    जइ (src > srcstart && src[-1] == '\\')
    {
	*usedlen = 0;
	STRMOVE(src - 1, src);	// remove backslash
	प्रत्यागम रिक्त;
    }

    /*
     * word or WORD under cursor
     */
    जइ (spec_idx == SPEC_CWORD || spec_idx == SPEC_CCWORD
						     || spec_idx == SPEC_CEXPR)
    {
	resultlen = find_ident_under_cursor(&result,
		spec_idx == SPEC_CWORD ? (FIND_IDENT | FIND_STRING)
	      : spec_idx == SPEC_CEXPR ? (FIND_IDENT | FIND_STRING | FIND_EVAL)
	      : FIND_STRING);
	जइ (resultlen == 0)
	{
	    *errormsg = "";
	    प्रत्यागम रिक्त;
	}
    }

    /*
     * '#': Alternate file name
     * '%': Current file name
     *	    File name under the cursor
     *	    File name for autocommand
     *	and following modifiers
     */
    अण्णहा
    {
	size_t off = 0;

	विकल्प (spec_idx)
	{
	पक्ष SPEC_PERC:
#ifdef FEAT_EVAL
		जइ (!in_vim9script() || src[1] != '%')
#endif
		{
		    // '%': current file
		    जइ (curbuf->b_fname == रिक्त)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    अण्णहा
		    {
			result = curbuf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		    भङ्ग;
		}
#ifdef FEAT_EVAL
		// "%%" alternate file
		off = 1;
#endif
		// FALLTHROUGH
	पक्ष SPEC_HASH:		// '#' or "#99": alternate file
		जइ (off == 0 ? src[1] == '#' : src[2] == '%')
		{
		    // "##" or "%%%": the argument list
		    result = arg_all();
		    resultbuf = result;
		    *usedlen = off + 2;
		    जइ (escaped != रिक्त)
			*escaped = TRUE;
		    skip_mod = TRUE;
		    भङ्ग;
		}
		s = src + off + 1;
		जइ (*s == '<')		// "#<99" uses v:oldfiles
		    ++s;
		i = (पूर्णाङ्क)getdigits(&s);
		जइ (s == src + off + 2 && src[off + 1] == '-')
		    // just a minus sign, don't skip over it
		    s--;
		*usedlen = (पूर्णाङ्क)(s - src); // length of what we expand

		जइ (src[off + 1] == '<' && i != 0)
		{
		    जइ (*usedlen < off + 2)
		    {
			// Should we give an error message for #<text?
			*usedlen = off + 1;
			प्रत्यागम रिक्त;
		    }
#ifdef FEAT_EVAL
		    result = list_find_str(get_vim_var_list(VV_OLDFILES),
								     (long)i);
		    जइ (result == रिक्त)
		    {
			*errormsg = "";
			प्रत्यागम रिक्त;
		    }
#अण्णहा
		    *errormsg = _(e_hashsmall_is_not_available_without_the_eval_feature);
		    प्रत्यागम रिक्त;
#endif
		}
		अण्णहा
		{
		    जइ (i == 0 && src[off + 1] == '<' && *usedlen > off + 1)
			*usedlen = off + 1;
		    buf = buflist_findnr(i);
		    जइ (buf == रिक्त)
		    {
			*errormsg = _(e_no_alternate_file_name_to_substitute_for_hash);
			प्रत्यागम रिक्त;
		    }
		    जइ (lnump != रिक्त)
			*lnump = ECMD_LAST;
		    जइ (buf->b_fname == रिक्त)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    अण्णहा
		    {
			result = buf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		}
		भङ्ग;

	पक्ष SPEC_CFILE:	// file name under cursor
		result = file_name_at_cursor(FNAME_MESS|FNAME_HYP, 1L, रिक्त);
		जइ (result == रिक्त)
		{
		    *errormsg = "";
		    प्रत्यागम रिक्त;
		}
		resultbuf = result;	    // remember allocated string
		भङ्ग;

	पक्ष SPEC_AFILE:	// file name for autocommand
		result = autocmd_fname;
		जइ (result != रिक्त && !autocmd_fname_full)
		{
		    // Still need to turn the fname into a full path.  It is
		    // postponed to avoid a delay when <afile> is not used.
		    autocmd_fname_full = TRUE;
		    result = FullName_save(autocmd_fname, FALSE);
		    vim_free(autocmd_fname);
		    autocmd_fname = result;
		}
		जइ (result == रिक्त)
		{
		    *errormsg = _(e_no_autocommand_file_name_to_substitute_for_afile);
		    प्रत्यागम रिक्त;
		}
		result = shorten_fname1(result);
		भङ्ग;

	पक्ष SPEC_ABUF:		// buffer number for autocommand
		जइ (autocmd_bufnr <= 0)
		{
		    *errormsg = _(e_no_autocommand_buffer_सङ्ख्या_to_substitute_for_abuf);
		    प्रत्यागम रिक्त;
		}
		sprintf((वर्ण *)strbuf, "%d", autocmd_bufnr);
		result = strbuf;
		भङ्ग;

	पक्ष SPEC_AMATCH:	// match name for autocommand
		result = autocmd_match;
		जइ (result == रिक्त)
		{
		    *errormsg = _(e_no_autocommand_match_name_to_substitute_for_amatch);
		    प्रत्यागम रिक्त;
		}
		भङ्ग;

	पक्ष SPEC_SFILE:	// file name for ":so" command
		result = estack_sfile(ESTACK_SFILE);
		जइ (result == रिक्त)
		{
		    *errormsg = _(e_no_source_file_name_to_substitute_for_sfile);
		    प्रत्यागम रिक्त;
		}
		resultbuf = result;	    // remember allocated string
		भङ्ग;
	पक्ष SPEC_राशिः:	// call stack
		result = estack_sfile(ESTACK_राशिः);
		जइ (result == रिक्त)
		{
		    *errormsg = _(e_no_call_राशिः_to_substitute_for_राशिः);
		    प्रत्यागम रिक्त;
		}
		resultbuf = result;	    // remember allocated string
		भङ्ग;
	पक्ष SPEC_SCRIPT:	// script file name
		result = estack_sfile(ESTACK_SCRIPT);
		जइ (result == रिक्त)
		{
		    *errormsg = _(e_no_script_file_name_to_substitute_for_script);
		    प्रत्यागम रिक्त;
		}
		resultbuf = result;	    // remember allocated string
		भङ्ग;

	पक्ष SPEC_SLNUM:	// line in file for ":so" command
		जइ (SOURCING_NAME == रिक्त || SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_पङ्क्तिः_सङ्ख्या_to_use_for_slnum);
		    प्रत्यागम रिक्त;
		}
		sprintf((वर्ण *)strbuf, "%ld", SOURCING_LNUM);
		result = strbuf;
		भङ्ग;

#ifdef FEAT_EVAL
	पक्ष SPEC_SFLNUM:	// line in script file
		जइ (current_sctx.sc_lnum + SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_पङ्क्तिः_सङ्ख्या_to_use_for_sflnum);
		    प्रत्यागम रिक्त;
		}
		sprintf((वर्ण *)strbuf, "%ld",
				 (long)(current_sctx.sc_lnum + SOURCING_LNUM));
		result = strbuf;
		भङ्ग;

	पक्ष SPEC_SID:
		जइ (current_sctx.sc_sid <= 0)
		{
		    *errormsg = _(e_using_sid_not_in_script_context);
		    प्रत्यागम रिक्त;
		}
		sprintf((वर्ण *)strbuf, "<SNR>%d_", current_sctx.sc_sid);
		result = strbuf;
		भङ्ग;
#endif

#ifdef FEAT_CLIENTSERVER
	पक्ष SPEC_CLIENT:	// Source of last submitted input
# ifdef MSWIN
		sprintf((वर्ण *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		result = strbuf;
# अण्णहा
#  ifdef FEAT_SOCKETSERVER
		जइ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		{
		    जइ (client_socket == रिक्त)
			result = (char_u *)"";
		    अण्णहा
			result = client_socket;
		}
#  endif
#  ifdef FEAT_X11
		जइ (clientserver_method == CLIENTSERVER_METHOD_X11)
		{
		    sprintf((वर्ण *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		    result = strbuf;
		}
#  endif
# endif
		भङ्ग;
#endif
	}

	resultlen = STRLEN(result);	// length of new string
	जइ (src[*usedlen] == '<')	// remove the file name extension
	{
	    ++*usedlen;
	    जइ ((s = vim_strrchr(result, '.')) != रिक्त && s >= gettail(result))
		resultlen = s - result;
	}
	अण्णहा जइ (!skip_mod)
	{
	    valid |= modify_fname(src, tilde_file, usedlen, &result, &resultbuf,
								  &resultlen);
	    जइ (result == रिक्त)
	    {
		*errormsg = "";
		प्रत्यागम रिक्त;
	    }
	}
    }

    जइ (resultlen == 0 || valid != VALID_HEAD + VALID_PATH)
    {
	जइ (empty_is_error)
	{
	    जइ (valid != VALID_HEAD + VALID_PATH)
		*errormsg = _(e_empty_file_name_for_percent_or_hash_only_works_with_ph);
	    अण्णहा
		*errormsg = _(e_evaluates_to_an_empty_string);
	}
	result = रिक्त;
    }
    अण्णहा
	result = vim_strnsave(result, resultlen);
    vim_free(resultbuf);
    प्रत्यागम result;
}

/*
 * Expand the <sfile> string in "arg".
 *
 * Returns an allocated string, or NULL for any error.
 */
    char_u *
expand_sfile(char_u *arg)
{
    वर्ण	*errormsg;
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
    जइ (result == रिक्त)
	प्रत्यागम रिक्त;

    प्रति (p = result; *p; )
    {
	जइ (STRNCMP(p, "<sfile>", 7) != 0)
	    ++p;
	अण्णहा
	{
	    // replace "<sfile>" with the sourced file name, and do ":" stuff
	    repl = eval_vars(p, result, &srclen, रिक्त, &errormsg, रिक्त, TRUE);
	    जइ (errormsg != रिक्त)
	    {
		जइ (*errormsg)
		    emsg(errormsg);
		vim_free(result);
		प्रत्यागम रिक्त;
	    }
	    जइ (repl == रिक्त)		// no match (cannot happen)
	    {
		p += srclen;
		अनुवर्त;
	    }
	    repllen = STRLEN(repl);
	    resultlen += repllen - srclen;
	    newres = alloc(resultlen + 1);
	    जइ (newres == रिक्त)
	    {
		vim_free(repl);
		vim_free(result);
		प्रत्यागम रिक्त;
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

    प्रत्यागम result;
}

#जइ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
/*
 * Make a dialog message in "buff[DIALOG_MSG_SIZE]".
 * "format" must contain "%s".
 */
    शून्य
dialog_सन्देशः(char_u *buff, वर्ण *format, char_u *fname)
{
    जइ (fname == रिक्त)
	fname = (char_u *)_("Untitled");
    vim_snprintf((वर्ण *)buff, DIALOG_सन्देशः_SIZE, format, fname);
}
#endif

/*
 * ":behave {mswin,xterm}"
 */
    स्थिर शून्य
ex_behave(exarg_T *eap)
{
    जइ (STRCMP(eap->arg, "mswin") == 0)
    {
	set_option_मानम्_give_err((char_u *)"selection",
						 0L, (char_u *)"exclusive", 0);
	set_option_मानम्_give_err((char_u *)"selectmode",
						 0L, (char_u *)"mouse,key", 0);
	set_option_मानम्_give_err((char_u *)"mousemodel",
						     0L, (char_u *)"popup", 0);
	set_option_मानम्_give_err((char_u *)"keymodel",
					  0L, (char_u *)"startsel,stopsel", 0);
    }
    अण्णहा जइ (STRCMP(eap->arg, "xterm") == 0)
    {
	set_option_मानम्_give_err((char_u *)"selection",
						 0L, (char_u *)"inclusive", 0);
	set_option_मानम्_give_err((char_u *)"selectmode", 0L, (char_u *)"", 0);
	set_option_मानम्_give_err((char_u *)"mousemodel",
						    0L, (char_u *)"extend", 0);
	set_option_मानम्_give_err((char_u *)"keymodel", 0L, (char_u *)"", 0);
    }
    अण्णहा
	semsg(_(e_invalid_argument_str), eap->arg);
}

स्थिर पूर्णाङ्क filetype_detect = FALSE;
स्थिर पूर्णाङ्क filetype_plugin = FALSE;
स्थिर पूर्णाङ्क filetype_indent = FALSE;

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
    पूर्णाङ्क		plugin = FALSE;
    पूर्णाङ्क		indent = FALSE;

    जइ (*eap->arg == NUL)
    {
	// Print current status.
	smsg("filetype detection:%s  plugin:%s  indent:%s",
		filetype_detect ? "ON" : "OFF",
		filetype_plugin ? (filetype_detect ? "ON" : "(on)") : "OFF",
		filetype_indent ? (filetype_detect ? "ON" : "(on)") : "OFF");
	प्रत्यागम;
    }

    // Accept "plugin" and "indent" in any order.
    प्रति (;;)
    {
	जइ (STRNCMP(arg, "plugin", 6) == 0)
	{
	    plugin = TRUE;
	    arg = skipwhite(arg + 6);
	    अनुवर्त;
	}
	जइ (STRNCMP(arg, "indent", 6) == 0)
	{
	    indent = TRUE;
	    arg = skipwhite(arg + 6);
	    अनुवर्त;
	}
	भङ्ग;
    }
    जइ (STRCMP(arg, "on") == 0 || STRCMP(arg, "detect") == 0)
    {
	जइ (*arg == 'o' || !filetype_detect)
	{
	    source_runtime((char_u *)FILETYPE_FILE, DIP_ALL);
	    filetype_detect = TRUE;
	    जइ (plugin)
	    {
		source_runtime((char_u *)FTPLUGIN_FILE, DIP_ALL);
		filetype_plugin = TRUE;
	    }
	    जइ (indent)
	    {
		source_runtime((char_u *)INDENT_FILE, DIP_ALL);
		filetype_indent = TRUE;
	    }
	}
	जइ (*arg == 'd')
	{
	    (शून्य)do_doautocmd((char_u *)"filetypedetect BufRead", TRUE, रिक्त);
	    do_modelines(0);
	}
    }
    अण्णहा जइ (STRCMP(arg, "off") == 0)
    {
	जइ (plugin || indent)
	{
	    जइ (plugin)
	    {
		source_runtime((char_u *)FTPLUGOF_FILE, DIP_ALL);
		filetype_plugin = FALSE;
	    }
	    जइ (indent)
	    {
		source_runtime((char_u *)INDOFF_FILE, DIP_ALL);
		filetype_indent = FALSE;
	    }
	}
	अण्णहा
	{
	    source_runtime((char_u *)FTOFF_FILE, DIP_ALL);
	    filetype_detect = FALSE;
	}
    }
    अण्णहा
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * ":setfiletype [FALLBACK] {name}"
 */
    स्थिर शून्य
ex_setfiletype(exarg_T *eap)
{
    जइ (curbuf->b_did_filetype)
	प्रत्यागम;

    char_u *arg = eap->arg;
    जइ (STRNCMP(arg, "FALLBACK ", 9) == 0)
	arg += 9;

    set_option_मानम्_give_err((char_u *)"filetype", 0L, arg, OPT_LOCAL);
    जइ (arg != eap->arg)
	curbuf->b_did_filetype = false;
}

    स्थिर शून्य
ex_digraphs(exarg_T *eap UNUSED)
{
#ifdef FEAT_DIGRAPHS
    जइ (*eap->arg != NUL)
	putdigraph(eap->arg);
    अण्णहा
	listdigraphs(eap->forceit);
#अण्णहा
    emsg(_(e_no_digraphs_version));
#endif
}

#जइ defined(FEAT_अन्वेषणम्_EXTRA)
    शून्य
set_no_hlsearch(पूर्णाङ्क flag)
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
    (शून्य)crypt_get_कुञ्चिका(TRUE, TRUE);
}
#endif

#ifdef FEAT_FOLDING
    स्थिर शून्य
ex_fold(exarg_T *eap)
{
    जइ (foldManualAllowed(TRUE))
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
    प्रति (lnum = eap->line1; lnum <= eap->line2; ++lnum)
	जइ (hasFolding(lnum, रिक्त, रिक्त) == (eap->cmdidx == CMD_folddoclosed))
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

#जइ defined(FEAT_QUICKFIX)
/*
 * Returns TRUE if the supplied Ex cmdidx is for a location list command
 * instead of a quickfix command.
 */
    पूर्णाङ्क
is_loclist_cmd(पूर्णाङ्क cmdidx)
{
    जइ (cmdidx < 0 || cmdidx >= CMD_SIZE)
	प्रत्यागम FALSE;
    प्रत्यागम cmdnames[cmdidx].cmd_name[0] == 'l';
}
#endif

    पूर्णाङ्क
get_pressedreturn(शून्य)
{
    प्रत्यागम ex_pressedreturn;
}

    शून्य
set_pressedreturn(पूर्णाङ्क val)
{
    ex_pressedreturn = val;
}
