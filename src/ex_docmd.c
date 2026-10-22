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

#அந்தர்பாவ "vim.h"

நிலையான முழு	quitmore = 0;
நிலையான முழு	ex_pressedreturn = FALSE;
#ifndef FEAT_PRINTER
# define ex_hardcopy	ex_ni
#endif

#ifdef FEAT_EVAL
நிலையான char_u	*do_one_cmd(char_u **, முழு, cstack_T *, char_u *(*fgetline)(முழு, வெறுமை *, முழு, getline_opt_T), வெறுமை *cookie);
#இல்லையெனில்
நிலையான char_u	*do_one_cmd(char_u **, முழு, char_u *(*fgetline)(முழு, வெறுமை *, முழு, getline_opt_T), வெறுமை *cookie);
நிலையான முழு	if_level = 0;		// depth in :if
#endif
நிலையான வெறுமை	append_command(char_u *cmd);

#ifndef FEAT_MENU
# define ex_emenu		ex_ni
# define ex_menu		ex_ni
# define ex_menutranslate	ex_ni
#endif
நிலையான வெறுமை	ex_autocmd(exarg_T *eap);
நிலையான வெறுமை	ex_doautocmd(exarg_T *eap);
நிலையான வெறுமை	ex_bunload(exarg_T *eap);
நிலையான வெறுமை	ex_buffer(exarg_T *eap);
நிலையான வெறுமை	do_exbuffer(exarg_T *eap);
நிலையான வெறுமை	ex_bmodified(exarg_T *eap);
நிலையான வெறுமை	ex_bnext(exarg_T *eap);
நிலையான வெறுமை	ex_bprevious(exarg_T *eap);
நிலையான வெறுமை	ex_brewind(exarg_T *eap);
நிலையான வெறுமை	ex_blast(exarg_T *eap);
நிலையான char_u	*getargcmd(char_u **);
நிலையான முழு	getargopt(exarg_T *eap);
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
#எனில் !defined(FEAT_QUICKFIX) || !defined(FEAT_EVAL)
# define ex_cexpr		ex_ni
#endif

நிலையான linenr_T default_address(exarg_T *eap);
நிலையான வெறுமை address_default_all(exarg_T *eap);
நிலையான வெறுமை	get_flags(exarg_T *eap);
#எனில் !defined(FEAT_PERL) \
	|| !defined(FEAT_PYTHON) || !defined(FEAT_PYTHON3) \
	|| !defined(FEAT_TCL) \
	|| !defined(FEAT_RUBY) \
	|| !defined(FEAT_LUA) \
	|| !defined(FEAT_MZSCHEME)
# define HAVE_EX_SCRIPT_NI
நிலையான வெறுமை	ex_script_ni(exarg_T *eap);
#endif
நிலையான எழுத்து	*invalid_range(exarg_T *eap);
நிலையான வெறுமை	correct_range(exarg_T *eap);
#ifdef FEAT_QUICKFIX
நிலையான char_u	*replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep);
#endif
நிலையான char_u	*repl_cmdline(exarg_T *eap, char_u *src, size_t srclen, char_u *repl, char_u **cmdlinep);
நிலையான வெறுமை	ex_highlight(exarg_T *eap);
நிலையான வெறுமை	ex_colorscheme(exarg_T *eap);
நிலையான வெறுமை	ex_cquit(exarg_T *eap);
நிலையான வெறுமை	ex_quit_all(exarg_T *eap);
நிலையான வெறுமை	ex_close(exarg_T *eap);
நிலையான வெறுமை	ex_win_close(முழு forceit, win_T *win, tabpage_T *tp);
நிலையான வெறுமை	ex_only(exarg_T *eap);
நிலையான வெறுமை	ex_resize(exarg_T *eap);
நிலையான வெறுமை	ex_stag(exarg_T *eap);
நிலையான வெறுமை	ex_tabclose(exarg_T *eap);
நிலையான வெறுமை	ex_tabonly(exarg_T *eap);
நிலையான வெறுமை	ex_tabnext(exarg_T *eap);
நிலையான வெறுமை	ex_tabmove(exarg_T *eap);
நிலையான வெறுமை	ex_tabs(exarg_T *eap);
#எனில் defined(FEAT_QUICKFIX)
நிலையான வெறுமை	ex_pclose(exarg_T *eap);
நிலையான வெறுமை	ex_ptag(exarg_T *eap);
நிலையான வெறுமை	ex_pedit(exarg_T *eap);
நிலையான வெறுமை	ex_pbuffer(exarg_T *eap);
நிலையான வெறுமை	prepare_preview_window(வெறுமை);
நிலையான வெறுமை	back_to_current_window(win_T *curwin_save);
#இல்லையெனில்
# define ex_pclose		ex_ni
# define ex_ptag		ex_ni
# define ex_pedit		ex_ni
# define ex_pbuffer		ex_ni
#endif
நிலையான வெறுமை	ex_hide(exarg_T *eap);
நிலையான வெறுமை	ex_exit(exarg_T *eap);
நிலையான வெறுமை	ex_print(exarg_T *eap);
#ifdef FEAT_BYTEOFF
நிலையான வெறுமை	ex_goto(exarg_T *eap);
#இல்லையெனில்
# define ex_goto		ex_ni
#endif
நிலையான வெறுமை	ex_shell(exarg_T *eap);
நிலையான வெறுமை	ex_preserve(exarg_T *eap);
நிலையான வெறுமை	ex_recover(exarg_T *eap);
நிலையான வெறுமை	ex_mode(exarg_T *eap);
நிலையான வெறுமை	ex_wrongmodifier(exarg_T *eap);
நிலையான வெறுமை	ex_find(exarg_T *eap);
நிலையான வெறுமை	ex_open(exarg_T *eap);
நிலையான வெறுமை	ex_edit(exarg_T *eap);
#ifndef FEAT_GUI
# define ex_gui			ex_nogui
நிலையான வெறுமை	ex_nogui(exarg_T *eap);
#endif
#எனில் defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
நிலையான வெறுமை	ex_tearoff(exarg_T *eap);
#இல்லையெனில்
# define ex_tearoff		ex_ni
#endif
#எனில் (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
நிலையான வெறுமை	ex_popup(exarg_T *eap);
#இல்லையெனில்
# define ex_popup		ex_ni
#endif
#ifndef FEAT_GUI_MSWIN
# define ex_simalt		ex_ni
#endif
#எனில் !defined(FEAT_GUI_MSWIN) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
# define gui_mch_find_dialog	ex_ni
# define gui_mch_replace_dialog ex_ni
#endif
#எனில் !defined(FEAT_GUI_GTK)
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
#எனில் !defined(FEAT_SYN_HL) || !defined(FEAT_PROFILE)
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
#எனில் !defined(FEAT_PYTHON) && !defined(FEAT_PYTHON3)
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
நிலையான வெறுமை	ex_swapname(exarg_T *eap);
நிலையான வெறுமை	ex_syncbind(exarg_T *eap);
நிலையான வெறுமை	ex_read(exarg_T *eap);
நிலையான வெறுமை	ex_pwd(exarg_T *eap);
நிலையான வெறுமை	ex_equal(exarg_T *eap);
நிலையான வெறுமை	ex_sleep(exarg_T *eap);
நிலையான வெறுமை	ex_winsize(exarg_T *eap);
நிலையான வெறுமை	ex_wincmd(exarg_T *eap);
#எனில் defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
நிலையான வெறுமை	ex_winpos(exarg_T *eap);
#இல்லையெனில்
# define ex_winpos	    ex_ni
#endif
நிலையான வெறுமை	ex_operators(exarg_T *eap);
நிலையான வெறுமை	ex_put(exarg_T *eap);
நிலையான வெறுமை	ex_iput(exarg_T *eap);
நிலையான வெறுமை	ex_copymove(exarg_T *eap);
நிலையான வெறுமை	ex_submagic(exarg_T *eap);
நிலையான வெறுமை	ex_join(exarg_T *eap);
நிலையான வெறுமை	ex_at(exarg_T *eap);
நிலையான வெறுமை	ex_bang(exarg_T *eap);
நிலையான வெறுமை	ex_undo(exarg_T *eap);
#ifdef FEAT_PERSISTENT_UNDO
நிலையான வெறுமை	ex_wundo(exarg_T *eap);
நிலையான வெறுமை	ex_rundo(exarg_T *eap);
#endif
நிலையான வெறுமை	ex_redo(exarg_T *eap);
நிலையான வெறுமை	ex_later(exarg_T *eap);
நிலையான வெறுமை	ex_redir(exarg_T *eap);
நிலையான வெறுமை	ex_redrawstatus(exarg_T *eap);
நிலையான வெறுமை	ex_redrawtabline(exarg_T *eap);
நிலையான வெறுமை	ex_redrawtabpanel(exarg_T *eap);
நிலையான வெறுமை	close_redir(வெறுமை);
நிலையான வெறுமை	ex_mark(exarg_T *eap);
நிலையான வெறுமை	ex_startinsert(exarg_T *eap);
நிலையான வெறுமை	ex_stopinsert(exarg_T *eap);
#ifdef FEAT_FIND_ID
நிலையான வெறுமை	ex_checkpath(exarg_T *eap);
நிலையான வெறுமை	ex_findpat(exarg_T *eap);
#இல்லையெனில்
# define ex_findpat		ex_ni
# define ex_checkpath		ex_ni
#endif
#எனில் defined(FEAT_FIND_ID) && defined(FEAT_QUICKFIX)
நிலையான வெறுமை	ex_psearch(exarg_T *eap);
#இல்லையெனில்
# define ex_psearch		ex_ni
#endif
நிலையான வெறுமை	ex_tag(exarg_T *eap);
நிலையான வெறுமை	ex_tag_cmd(exarg_T *eap, char_u *name);
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
நிலையான வெறுமை	ex_behave(exarg_T *eap);
நிலையான வெறுமை	ex_filetype(exarg_T *eap);
நிலையான வெறுமை	ex_setfiletype(exarg_T *eap);
#ifndef FEAT_DIFF
# define ex_diffoff		ex_ni
# define ex_diffpatch		ex_ni
# define ex_diffgetput		ex_ni
# define ex_diffsplit		ex_ni
# define ex_diffthis		ex_ni
# define ex_diffupdate		ex_ni
#endif
நிலையான வெறுமை	ex_digraphs(exarg_T *eap);
#ifdef FEAT_தேடல்_EXTRA
நிலையான வெறுமை	ex_nohlsearch(exarg_T *eap);
#இல்லையெனில்
# define ex_nohlsearch		ex_ni
# define ex_match		ex_ni
#endif
#ifdef FEAT_CRYPT
நிலையான வெறுமை	ex_X(exarg_T *eap);
#இல்லையெனில்
# define ex_X			ex_ni
#endif
#ifdef FEAT_FOLDING
நிலையான வெறுமை	ex_fold(exarg_T *eap);
நிலையான வெறுமை	ex_foldopen(exarg_T *eap);
நிலையான வெறுமை	ex_folddo(exarg_T *eap);
#இல்லையெனில்
# define ex_fold		ex_ni
# define ex_foldopen		ex_ni
# define ex_folddo		ex_ni
#endif
#எனில் !(defined(HAVE_LOCALE_H) || defined(X_LOCALE))
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
#எனில் !defined(FEAT_X11) || !defined(FEAT_XCLIPBOARD)
# define ex_xrestore		ex_ni
#endif
#எனில் !defined(FEAT_WAYLAND)
# define ex_wlrestore		ex_ni
#endif
#எனில் !defined(HAVE_CLIPMETHOD)
# define ex_clipreset		ex_ni
#endif
#எனில் !defined(FEAT_PROP_POPUP)
# define ex_popupclear		ex_ni
#endif

/*
 * Declare cmdnames[].
 */
#define DO_DECLARE_EXCMD
#அந்தர்பாவ "ex_cmds.h"
#அந்தர்பாவ "ex_cmdidxs.h"

நிலையான char_u dollar_command[2] = {'$', 0};


#ifdef FEAT_EVAL
// Struct for storing a line inside a while/for loop
வகைவரை கட்டமைப்பு
{
    char_u	*வரி;		// command line
    linenr_T	lnum;		// sourcing_lnum of the line
} wcmd_T;

/*
 * Structure used to store info for line position in a while or for loop.
 * This is required, because do_one_cmd() may invoke ex_function(), which
 * reads more lines that may come from the while/for loop.
 */
கட்டமைப்பு loop_cookie
{
    garray_T	*வரிகள்_gap;		// growarray with line info
    முழு		current_வரி;		// last read line from growarray
    முழு		repeating;		// TRUE when looping a second time
    // When "repeating" is FALSE use "getline" and "cookie" to get lines
    char_u	*(*lc_getline)(முழு, வெறுமை *, முழு, getline_opt_T);
    வெறுமை	*cookie;
};

நிலையான char_u	*get_loop_வரி(முழு c, வெறுமை *cookie, முழு indent, getline_opt_T options);
நிலையான முழு	store_loop_வரி(garray_T *gap, char_u *வரி);
நிலையான வெறுமை	free_cmdlines(garray_T *gap);

// Struct to save a few things while debugging.  Used in do_cmdline() only.
கட்டமைப்பு dbg_stuff
{
    முழு		trylevel;
    முழு		force_abort;
    except_T	*caught_அடுக்கு;
    char_u	*vv_exception;
    char_u	*vv_throwpoint;
    முழு		did_emsg;
    முழு		got_int;
    முழு		did_throw;
    முழு		need_rethrow;
    முழு		check_cstack;
    except_T	*current_exception;
};

    நிலையான வெறுமை
save_dbg_stuff(கட்டமைப்பு dbg_stuff *dsp)
{
    dsp->trylevel	= trylevel;		trylevel = 0;
    dsp->force_abort	= force_abort;		force_abort = FALSE;
    dsp->caught_அடுக்கு	= caught_அடுக்கு;		caught_அடுக்கு = வெற்று;
    dsp->vv_exception	= v_exception(வெற்று);
    dsp->vv_throwpoint	= v_throwpoint(வெற்று);

    // Necessary for debugging an inactive ":catch", ":finally", ":endtry"
    dsp->did_emsg	= did_emsg;		did_emsg     = FALSE;
    dsp->got_int	= got_int;		got_int	     = FALSE;
    dsp->did_throw	= did_throw;		did_throw    = FALSE;
    dsp->need_rethrow	= need_rethrow;		need_rethrow = FALSE;
    dsp->check_cstack	= check_cstack;		check_cstack = FALSE;
    dsp->current_exception = current_exception;	current_exception = வெற்று;
}

    நிலையான வெறுமை
restore_dbg_stuff(கட்டமைப்பு dbg_stuff *dsp)
{
    suppress_errthrow = FALSE;
    trylevel = dsp->trylevel;
    force_abort = dsp->force_abort;
    caught_அடுக்கு = dsp->caught_அடுக்கு;
    (வெறுமை)v_exception(dsp->vv_exception);
    (வெறுமை)v_throwpoint(dsp->vv_throwpoint);
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
    நிலையான முழு
is_other_file(முழு fnum, char_u *ffname)
{
    எனில் (fnum != 0)
    {
	எனில் (fnum == curbuf->b_fnum)
	    திரும்பு FALSE;

	திரும்பு TRUE;
    }

    எனில் (ffname == வெற்று)
	திரும்பு TRUE;

    எனில் (*ffname == NUL)
	திரும்பு FALSE;

    // TODO: Need a reliable way to know whether a buffer is meant to live
    // on-disk !curbuf->b_dev_valid is not always available (example: missing
    // on Windows)
    எனில் (curbuf->b_sfname != வெற்று
	    && *curbuf->b_sfname != NUL)
	// This occurs with unsaved buffers. In which case `ffname` actually
	// corresponds to curbuf->b_sfname
	திரும்பு fnamecmp(ffname, curbuf->b_sfname) != 0;

    திரும்பு otherfile(ffname);
}

/*
 * do_exmode(): Repeatedly get commands for the "Ex" mode, until the ":vi"
 * command is given.
 */
    வெறுமை
do_exmode(
    முழு		improved)	    // TRUE for "improved Ex" mode
{
    முழு		save_செய்தி_scroll;
    முழு		prev_செய்தி_row;
    linenr_T	prev_வரி;
    varnumber_T	changedtick;

    எனில் (improved)
	exmode_active = EXMODE_VIM;
    இல்லையெனில்
	exmode_active = EXMODE_NORMAL;
    State = MODE_NORMAL;
    may_trigger_modechanged();

    // When using ":global /pat/ visual" and then "Q" we return to continue
    // the :global command.
    எனில் (global_busy)
	திரும்பு;

    save_செய்தி_scroll = செய்தி_scroll;
    ++RedrawingDisabled;	    // don't redisplay the window
    ++no_wait_return;		    // don't wait for return
#ifdef FEAT_GUI
    // Ignore scrollbar and mouse events in Ex mode
    ++hold_gui_events;
#endif

    செய்தி(_("Entering Ex mode.  Type \"visual\" to go to Normal mode."));
    போது (exmode_active)
    {
	// Check for a ":normal" command and no more characters left.
	எனில் (ex_normal_busy > 0 && typebuf.tb_len == 0)
	{
	    exmode_active = FALSE;
	    நிறுத்து;
	}
	செய்தி_scroll = TRUE;
	need_wait_return = FALSE;
	ex_pressedreturn = FALSE;
	ex_no_reprint = FALSE;
	changedtick = CHANGEDTICK(curbuf);
	prev_செய்தி_row = செய்தி_row;
	prev_வரி = curwin->w_cursor.lnum;
	எனில் (improved)
	{
	    cmdline_row = செய்தி_row;
	    do_cmdline(வெற்று, getexline, வெற்று, 0);
	}
	இல்லையெனில்
	    do_cmdline(வெற்று, getexmodeline, வெற்று, DOCMD_NOWAIT);
	வரிகள்_இடது = Rows - 1;

	எனில் ((prev_வரி != curwin->w_cursor.lnum
		   || changedtick != CHANGEDTICK(curbuf)) && !ex_no_reprint)
	{
	    எனில் (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    இல்லையெனில்
	    {
		எனில் (ex_pressedreturn)
		{
		    // go up one line, to overwrite the ":<CR>" line, so the
		    // output doesn't contain empty lines.
		    செய்தி_row = prev_செய்தி_row;
		    எனில் (prev_செய்தி_row == Rows - 1)
			செய்தி_row--;
		}
		செய்தி_col = 0;
		print_வரி_no_prefix(curwin->w_cursor.lnum, FALSE, FALSE);
		செய்தி_clr_eos();
	    }
	}
	இல்லையெனில் எனில் (ex_pressedreturn && !ex_no_reprint)	// must be at EOF
	{
	    எனில் (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    இல்லையெனில்
		emsg(_(e_at_end_of_file));
	}
    }

#ifdef FEAT_GUI
    --hold_gui_events;
#endif
    எனில் (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    update_screen(UPD_CLEAR);
    need_wait_return = FALSE;
    செய்தி_scroll = save_செய்தி_scroll;
}

/*
 * Print the executed command for when 'verbose' is set.
 * When "lnum" is 0 only print the command.
 */
    நிலையான வெறுமை
செய்தி_verbose_cmd(linenr_T lnum, char_u *cmd)
{
    ++no_wait_return;
    verbose_enter_scroll();

    எனில் (lnum == 0)
	smsg(_("Executing: %s"), cmd);
    இல்லையெனில்
	smsg(_("line %ld: %s"), (long)lnum, cmd);
    எனில் (செய்தி_silent == 0)
	செய்தி_puts("\n");   // don't overwrite this

    verbose_leave_scroll();
    --no_wait_return;
}

/*
 * Execute a simple command line.  Used for translated commands like "*".
 */
    முழு
do_cmdline_cmd(char_u *cmd)
{
    திரும்பு do_cmdline(cmd, வெற்று, வெற்று,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
}

/*
 * Execute the "+cmd" argument of "edit +cmd fname" and the like.
 * This allows for using a range without ":" in Vim9 script.
 */
    நிலையான முழு
do_cmd_argument(char_u *cmd)
{
    திரும்பு do_cmdline(cmd, வெற்று, வெற்று,
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
    முழு
do_cmdline(
    char_u	*cmdline,
    char_u	*(*fgetline)(முழு, வெறுமை *, முழு, getline_opt_T),
    வெறுமை	*cookie,		// argument for fgetline()
    முழு		flags)
{
    char_u	*next_cmdline;		// next cmd to execute
    char_u	*cmdline_copy = வெற்று;	// copy of cmd line
    முழு		used_getline = FALSE;	// used "fgetline" to obtain command
    நிலையான முழு	recursive = 0;		// recursive depth
    முழு		செய்தி_didout_before_start = 0;
    முழு		மொத்தம் = 0;		// line number count
    முழு		did_inc_RedrawingDisabled = FALSE;
    முழு		retval = OK;
#ifdef FEAT_EVAL
    cstack_T	cstack;			// conditional stack
    garray_T	வரிகள்_ga;		// keep lines for ":while"/":for"
    முழு		current_வரி = 0;	// active line in lines_ga
    முழு		current_வரி_before = 0;
    char_u	*fname = வெற்று;		// function or script name
    linenr_T	*breakpoint = வெற்று;	// ptr to breakpoint field in cookie
    முழு		*dbg_tick = வெற்று;	// ptr to dbg_tick field in cookie
    கட்டமைப்பு dbg_stuff debug_saved;	// saved things for debug mode
    முழு		initial_trylevel;
    msglist_T	**saved_செய்தி_list = வெற்று;
    msglist_T	*private_செய்தி_list = வெற்று;

    // "fgetline" and "cookie" passed to do_one_cmd()
    char_u	*(*cmd_getline)(முழு, வெறுமை *, முழு, getline_opt_T);
    வெறுமை	*cmd_cookie;
    கட்டமைப்பு loop_cookie cmd_loop_cookie;
    வெறுமை	*real_cookie;
    முழு		getline_is_func;
#இல்லையெனில்
# define cmd_getline fgetline
# define cmd_cookie cookie
#endif
    நிலையான முழு	call_depth = 0;		// recursiveness
#ifdef FEAT_EVAL
    // For every pair of do_cmdline()/do_one_cmd() calls, use an extra memory
    // location for storing error messages to be converted to an exception.
    // This ensures that the do_errthrow() call in do_one_cmd() does not
    // combine the messages stored by an earlier invocation of do_one_cmd()
    // with the command name of the later one.  This would happen when
    // BufWritePost autocommands are executed after a write error.
    saved_செய்தி_list = செய்தி_list;
    செய்தி_list = &private_செய்தி_list;
#endif

    // It's possible to create an endless loop with ":execute", catch that
    // here.  The value of 200 allows nested function calls, ":source", etc.
    // Allow 200 or 'maxfuncdepth', whatever is larger.
    எனில் (call_depth >= 200
#ifdef FEAT_EVAL
	    && call_depth >= p_mfd
#endif
	    )
    {
	emsg(_(e_command_too_recursive));
#ifdef FEAT_EVAL
	// When converting to an exception, we do not include the command name
	// since this is not an error of the specific command.
	do_errthrow((cstack_T *)வெற்று, (char_u *)வெற்று);
	செய்தி_list = saved_செய்தி_list;
#endif
	திரும்பு FAIL;
    }
    ++call_depth;

#ifdef FEAT_EVAL
    CLEAR_FIELD(cstack);
    cstack.cs_idx = -1;
    ga_init2(&வரிகள்_ga, அளவு(wcmd_T), 10);

    real_cookie = getline_cookie(fgetline, cookie);

    // Inside a function use a higher nesting level.
    getline_is_func = getline_equal(fgetline, cookie, get_func_வரி);
    எனில் (getline_is_func && ex_nesting_level == func_level(real_cookie))
	++ex_nesting_level;

    // Get the function or script name and the address where the next breakpoint
    // line and the debug tick for a function or script are stored.
    எனில் (getline_is_func)
    {
	fname = func_name(real_cookie);
	breakpoint = func_breakpoint(real_cookie);
	dbg_tick = func_dbg_tick(real_cookie);
    }
    இல்லையெனில் எனில் (getline_equal(fgetline, cookie, getsourceline))
    {
	fname = SOURCING_NAME;
	breakpoint = source_breakpoint(real_cookie);
	dbg_tick = source_dbg_tick(real_cookie);
    }

    /*
     * Initialize "force_abort"  and "suppress_errthrow" at the top level.
     */
    எனில் (!recursive)
    {
	force_abort = FALSE;
	suppress_errthrow = FALSE;
    }

    /*
     * If requested, store and reset the global values controlling the
     * exception handling (used when debugging).  Otherwise clear it to avoid
     * a bogus compiler warning when the optimizer uses inline functions...
     */
    எனில் (flags & DOCMD_EXCRESET)
	save_dbg_stuff(&debug_saved);
    இல்லையெனில்
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
    எனில் (!(flags & DOCMD_KEYTYPED)
			       && !getline_equal(fgetline, cookie, getexline))
	சாவி_Typed = FALSE;

    /*
     * Continue executing command lines:
     * - when inside an ":if", ":while" or ":for"
     * - for multiple commands on one line, separated with '|'
     * - when repeating until there are no more lines (for ":source")
     */
    next_cmdline = cmdline;
    செய்
    {
#ifdef FEAT_EVAL
	getline_is_func = getline_equal(fgetline, cookie, get_func_வரி);
#endif

	// stop skipping cmds for an error msg after all endif/while/for
	எனில் (next_cmdline == வெற்று
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
	எனில் (cstack.cs_looplevel > 0 && current_வரி < வரிகள்_ga.ga_len)
	{
	    // Each '|' separated command is stored separately in lines_ga, to
	    // be able to jump to it.  Don't use next_cmdline now.
	    VIM_CLEAR(cmdline_copy);

	    // Check if a function has returned or, unless it has an unclosed
	    // try conditional, aborted.
	    எனில் (getline_is_func)
	    {
# ifdef FEAT_PROFILE
		எனில் (do_profiling == PROF_YES)
		    func_வரி_end(real_cookie);
# endif
		எனில் (func_has_ended(real_cookie))
		{
		    retval = FAIL;
		    நிறுத்து;
		}
	    }
# ifdef FEAT_PROFILE
	    இல்லையெனில் எனில் (do_profiling == PROF_YES
			    && getline_equal(fgetline, cookie, getsourceline))
		script_வரி_end();
# endif

	    // Check if a sourced file hit a ":finish" command.
	    எனில் (source_finished(fgetline, cookie))
	    {
		retval = FAIL;
		நிறுத்து;
	    }

	    // If breakpoints have been added/deleted need to check for it.
	    எனில் (breakpoint != வெற்று && dbg_tick != வெற்று
						   && *dbg_tick != debug_tick)
	    {
		*breakpoint = dbg_find_breakpoint(
				getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }

	    next_cmdline = ((wcmd_T *)(வரிகள்_ga.ga_data))[current_வரி].வரி;
	    SOURCING_LNUM = ((wcmd_T *)(வரிகள்_ga.ga_data))[current_வரி].lnum;

	    // Did we encounter a breakpoint?
	    எனில் (breakpoint != வெற்று && *breakpoint != 0
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
	    எனில் (do_profiling == PROF_YES)
	    {
		எனில் (getline_is_func)
		    func_வரி_start(real_cookie, SOURCING_LNUM);
		இல்லையெனில் எனில் (getline_equal(fgetline, cookie, getsourceline))
		    script_வரி_start();
	    }
# endif
	}
#endif

	// 2. If no line given, get an allocated line with fgetline().
	எனில் (next_cmdline == வெற்று)
	{
	    /*
	     * Need to set msg_didout for the first line after an ":if",
	     * otherwise the ":if" will be overwritten.
	     */
	    எனில் (மொத்தம் == 1 && getline_equal(fgetline, cookie, getexline))
		செய்தி_didout = TRUE;
	    எனில் (fgetline == வெற்று || (next_cmdline = fgetline(':', cookie,
#ifdef FEAT_EVAL
		    cstack.cs_idx < 0 ? 0 : (cstack.cs_idx + 1) * 2
#இல்லையெனில்
		    0
#endif
		    , in_vim9script() ? GETLINE_CONCAT_CONTBAR
					       : GETLINE_CONCAT_CONT)) == வெற்று)
	    {
		// Don't call wait_return() for aborted command line.  The NULL
		// returned for the end of a sourced file or executed function
		// doesn't do this.
		எனில் (சாவி_Typed && !(flags & DOCMD_REPEAT))
		    need_wait_return = FALSE;
		retval = FAIL;
		நிறுத்து;
	    }
	    used_getline = TRUE;

	    /*
	     * Keep the first typed line.  Clear it when more lines are typed.
	     */
	    எனில் (flags & DOCMD_KEEPLINE)
	    {
		vim_free(repeat_cmdline);
		எனில் (மொத்தம் == 0)
		    repeat_cmdline = vim_strsave(next_cmdline);
		இல்லையெனில்
		    repeat_cmdline = வெற்று;
	    }
	}

	// 3. Make a copy of the command so we can mess with it.
	இல்லையெனில் எனில் (cmdline_copy == வெற்று)
	{
	    next_cmdline = vim_strsave(next_cmdline);
	    எனில் (next_cmdline == வெற்று)
	    {
		emsg(_(e_out_of_memory));
		retval = FAIL;
		நிறுத்து;
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
	எனில் ((cstack.cs_looplevel > 0 || has_loop_cmd(next_cmdline)))
	{
	    cmd_getline = get_loop_வரி;
	    cmd_cookie = (வெறுமை *)&cmd_loop_cookie;
	    cmd_loop_cookie.வரிகள்_gap = &வரிகள்_ga;
	    cmd_loop_cookie.current_வரி = current_வரி;
	    cmd_loop_cookie.lc_getline = fgetline;
	    cmd_loop_cookie.cookie = cookie;
	    cmd_loop_cookie.repeating = (current_வரி < வரிகள்_ga.ga_len);

	    // Save the current line when encountering it the first time.
	    எனில் (current_வரி == வரிகள்_ga.ga_len
		    && store_loop_வரி(&வரிகள்_ga, next_cmdline) == FAIL)
	    {
		retval = FAIL;
		நிறுத்து;
	    }
	    current_வரி_before = current_வரி;
	}
	இல்லையெனில்
	{
	    cmd_getline = fgetline;
	    cmd_cookie = cookie;
	}

	did_endif = FALSE;
#endif

	எனில் (மொத்தம்++ == 0)
	{
	    /*
	     * All output from the commands is put below each other, without
	     * waiting for a return. Don't do this when executing commands
	     * from a script or when being called recursive (e.g. for ":e
	     * +command file").
	     */
	    எனில் (!(flags & DOCMD_NOWAIT) && !recursive)
	    {
		செய்தி_didout_before_start = செய்தி_didout;
		செய்தி_didany = FALSE; // no output yet
		செய்தி_start();
		செய்தி_scroll = TRUE;  // put messages below each other
		++no_wait_return;   // don't wait for return until finished
		++RedrawingDisabled;
		did_inc_RedrawingDisabled = TRUE;
	    }
	}

	எனில் ((p_verbose >= 15 && SOURCING_NAME != வெற்று) || p_verbose >= 16)
	    செய்தி_verbose_cmd(SOURCING_LNUM, cmdline_copy);

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
	எனில் (cmd_cookie == (வெறுமை *)&cmd_loop_cookie)
	    // Use "current_line" from "cmd_loop_cookie", it may have been
	    // incremented when defining a function.
	    current_வரி = cmd_loop_cookie.current_வரி;
#endif

	எனில் (next_cmdline == வெற்று)
	{
	    VIM_CLEAR(cmdline_copy);

	    /*
	     * If the command was typed, remember it for the ':' register.
	     * Do this AFTER executing the command to make :@: work.
	     */
	    எனில் (getline_equal(fgetline, cookie, getexline)
						  && new_last_cmdline != வெற்று)
	    {
		vim_free(last_cmdline);
		last_cmdline = new_last_cmdline;
		new_last_cmdline = வெற்று;
	    }
	}
	இல்லையெனில்
	{
	    // need to copy the command after the '|' to cmdline_copy, for the
	    // next do_one_cmd()
	    STRMOVE(cmdline_copy, next_cmdline);
	    next_cmdline = cmdline_copy;
	}


#ifdef FEAT_EVAL
	// reset did_emsg for a function that is not aborted by an error
	எனில் (did_emsg && !force_abort
		&& getline_equal(fgetline, cookie, get_func_வரி)
					      && !func_has_abort(real_cookie))
	{
	    // did_emsg_cumul is not set here
	    did_emsg = FALSE;
	}

	எனில் (cstack.cs_looplevel > 0)
	{
	    ++current_வரி;

	    /*
	     * An ":endwhile", ":endfor" and ":continue" is handled here.
	     * If we were executing commands, jump back to the ":while" or
	     * ":for".
	     * If we were not executing commands, decrement cs_looplevel.
	     */
	    எனில் (cstack.cs_lflags & (CSL_HAD_CONT | CSL_HAD_ENDLOOP))
	    {
		cstack.cs_lflags &= ~(CSL_HAD_CONT | CSL_HAD_ENDLOOP);

		// Jump back to the matching ":while" or ":for".  Be careful
		// not to use a cs_line[] from an entry that isn't a ":while"
		// or ":for": It would make "current_line" invalid and can
		// cause a crash.
		எனில் (!did_emsg && !got_int && !did_throw
			&& cstack.cs_idx >= 0
			&& (cstack.cs_flags[cstack.cs_idx]
						      & (CSF_WHILE | CSF_FOR))
			&& cstack.cs_வரி[cstack.cs_idx] >= 0
			&& (cstack.cs_flags[cstack.cs_idx] & CSF_ACTIVE))
		{
		    current_வரி = cstack.cs_வரி[cstack.cs_idx];
						// remember we jumped there
		    cstack.cs_lflags |= CSL_HAD_LOOP;
		    வரி_breakcheck();		// check if CTRL-C typed

		    // Check for the next breakpoint at or after the ":while"
		    // or ":for".
		    எனில் (breakpoint != வெற்று && வரிகள்_ga.ga_len > current_வரி)
		    {
			*breakpoint = dbg_find_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
									fname,
			   ((wcmd_T *)வரிகள்_ga.ga_data)[current_வரி].lnum-1);
			*dbg_tick = debug_tick;
		    }
		}
		இல்லையெனில்
		{
		    // can only get here with ":endwhile" or ":endfor"
		    எனில் (cstack.cs_idx >= 0)
			rewind_conditionals(&cstack, cstack.cs_idx - 1,
				   CSF_WHILE | CSF_FOR, &cstack.cs_looplevel);
		}
	    }

	    /*
	     * For a ":while" or ":for" we need to remember the line number.
	     */
	    இல்லையெனில் எனில் (cstack.cs_lflags & CSL_HAD_LOOP)
	    {
		cstack.cs_lflags &= ~CSL_HAD_LOOP;
		cstack.cs_வரி[cstack.cs_idx] = current_வரி_before;
	    }
	}

	// Check for the next breakpoint after a watchexpression
	எனில் (breakpoint != வெற்று && has_watchexpr())
	{
	    *breakpoint = dbg_find_breakpoint(FALSE, fname, SOURCING_LNUM);
	    *dbg_tick = debug_tick;
	}

	/*
	 * When not inside any ":while" loop, clear remembered lines.
	 */
	எனில் (cstack.cs_looplevel == 0)
	{
	    எனில் (வரிகள்_ga.ga_len > 0)
	    {
		SOURCING_LNUM =
		       ((wcmd_T *)வரிகள்_ga.ga_data)[வரிகள்_ga.ga_len - 1].lnum;
		free_cmdlines(&வரிகள்_ga);
	    }
	    current_வரி = 0;
	}

	/*
	 * A ":finally" makes did_emsg, got_int, and did_throw pending for
	 * being restored at the ":endtry".  Reset them here and set the
	 * ACTIVE and FINALLY flags, so that the finally clause gets executed.
	 * This includes the case where a missing ":endif", ":endwhile" or
	 * ":endfor" was detected by the ":finally" itself.
	 */
	எனில் (cstack.cs_lflags & CSL_HAD_FINA)
	{
	    cstack.cs_lflags &= ~CSL_HAD_FINA;
	    report_make_pending(cstack.cs_pending[cstack.cs_idx]
		    & (CSTP_ERROR | CSTP_INTERRUPT | CSTP_THROW),
		    did_throw ? (வெறுமை *)current_exception : வெற்று);
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
	எனில் (trylevel == 0 && !did_emsg && !got_int && !did_throw)
	    force_abort = FALSE;

	// Convert an interrupt to an exception if appropriate.
	(வெறுமை)do_intthrow(&cstack);
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
    போது (!((got_int
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
	    && (next_cmdline != வெற்று
#ifdef FEAT_EVAL
			|| cstack.cs_idx >= 0
#endif
			|| (flags & DOCMD_REPEAT)));

    vim_free(cmdline_copy);
    did_emsg_syntax = FALSE;
#ifdef FEAT_EVAL
    free_cmdlines(&வரிகள்_ga);
    ga_clear(&வரிகள்_ga);

    எனில் (cstack.cs_idx >= 0)
    {
	/*
	 * If a sourced file or executed function ran to its end, report the
	 * unclosed conditional.
	 * In Vim9 script do not give a second error, executing aborts after
	 * the first one.
	 */
	எனில் (!got_int && !did_throw && !aborting()
		&& !(did_emsg && in_vim9script())
		&& ((getline_equal(fgetline, cookie, getsourceline)
			&& !source_finished(fgetline, cookie))
		    || (getline_equal(fgetline, cookie, get_func_வரி)
					    && !func_has_ended(real_cookie))))
	{
	    எனில் (cstack.cs_flags[cstack.cs_idx] & CSF_TRY)
		emsg(_(e_missing_endtry));
	    இல்லையெனில் எனில் (cstack.cs_flags[cstack.cs_idx] & CSF_WHILE)
		emsg(_(e_missing_endwhile));
	    இல்லையெனில் எனில் (cstack.cs_flags[cstack.cs_idx] & CSF_FOR)
		emsg(_(e_missing_endfor));
	    இல்லையெனில்
		emsg(_(e_missing_endif));
	}

	/*
	 * Reset "trylevel" in case of a ":finish" or ":return" or a missing
	 * ":endtry" in a sourced file or executed function.  If the try
	 * conditional is in its finally clause, ignore anything pending.
	 * If it is in a catch clause, finish the caught exception.
	 * Also cleanup any "cs_forinfo" structures.
	 */
	செய்
	{
	    முழு idx = cleanup_conditionals(&cstack, 0, TRUE);

	    எனில் (idx >= 0)
		--idx;	    // remove try block not in its finally clause
	    rewind_conditionals(&cstack, idx, CSF_WHILE | CSF_FOR,
							&cstack.cs_looplevel);
	}
	போது (cstack.cs_idx >= 0);
	trylevel = initial_trylevel;
    }

    // If a missing ":endtry", ":endwhile", ":endfor", or ":endif" or a memory
    // lack was reported above and the error message is to be converted to an
    // exception, do this now after rewinding the cstack.
    do_errthrow(&cstack, getline_equal(fgetline, cookie, get_func_வரி)
				  ? (char_u *)"endfunction" : (char_u *)வெற்று);

    எனில் (trylevel == 0)
    {
	// Just in case did_throw got set but current_exception wasn't.
	எனில் (current_exception == வெற்று)
	    did_throw = FALSE;

	/*
	 * When an exception is being thrown out of the outermost try
	 * conditional, discard the uncaught exception, disable the conversion
	 * of interrupts or errors to exceptions, and ensure that no more
	 * commands are executed.
	 */
	எனில் (did_throw)
	    handle_did_throw();

	/*
	 * On an interrupt or an aborting error not converted to an exception,
	 * disable the conversion of errors to exceptions.  (Interrupts are not
	 * converted anymore, here.) This enables also the interrupt message
	 * when force_abort is set and did_emsg unset in case of an interrupt
	 * from a finally clause after an error.
	 */
	இல்லையெனில் எனில் (got_int || (did_emsg && force_abort))
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
    எனில் (did_throw)
	need_rethrow = TRUE;
    எனில் ((getline_equal(fgetline, cookie, getsourceline)
		&& ex_nesting_level > source_level(real_cookie))
	    || (getline_equal(fgetline, cookie, get_func_வரி)
		&& ex_nesting_level > func_level(real_cookie) + 1))
    {
	எனில் (!did_throw)
	    check_cstack = TRUE;
    }
    இல்லையெனில்
    {
	// When leaving a function, reduce nesting level.
	எனில் (getline_equal(fgetline, cookie, get_func_வரி))
	    --ex_nesting_level;
	/*
	 * Go to debug mode when returning from a function in which we are
	 * single-stepping.
	 */
	எனில் ((getline_equal(fgetline, cookie, getsourceline)
		    || getline_equal(fgetline, cookie, get_func_வரி))
		&& ex_nesting_level + 1 <= debug_break_level)
	    do_debug(getline_equal(fgetline, cookie, getsourceline)
		    ? (char_u *)_("End of sourced file")
		    : (char_u *)_("End of function"));
    }

    /*
     * Restore the exception environment (done after returning from the
     * debugger).
     */
    எனில் (flags & DOCMD_EXCRESET)
	restore_dbg_stuff(&debug_saved);

    செய்தி_list = saved_செய்தி_list;

    // Cleanup if "cs_emsg_silent_list" remains.
    எனில் (cstack.cs_emsg_silent_list != வெற்று)
    {
	eslist_T *elem, *temp;

	க்கு (elem = cstack.cs_emsg_silent_list; elem != வெற்று; elem = temp)
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
    எனில் (did_inc_RedrawingDisabled)
    {
	எனில் (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	--no_wait_return;
	செய்தி_scroll = FALSE;

	/*
	 * When just finished an ":if"-":else" which was typed, no need to
	 * wait for hit-return.  Also for an error situation.
	 */
	எனில் (retval == FAIL
#ifdef FEAT_EVAL
		|| (did_endif && சாவி_Typed && !did_emsg)
#endif
					    )
	{
	    need_wait_return = FALSE;
	    செய்தி_didany = FALSE;		// don't wait when restarting edit
	}
	இல்லையெனில் எனில் (need_wait_return)
	{
	    /*
	     * The msg_start() above clears msg_didout. The wait_return() we do
	     * here should not overwrite the command that may be shown before
	     * doing that.
	     */
	    செய்தி_didout |= செய்தி_didout_before_start;
	    wait_return(FALSE);
	}
    }

#ifdef FEAT_EVAL
    did_endif = FALSE;  // in case do_cmdline used recursively
#இல்லையெனில்
    /*
     * Reset if_level, in case a sourced script file contains more ":if" than
     * ":endif" (could be ":if x | foo | endif").
     */
    if_level = 0;
#endif

    --call_depth;
    திரும்பு retval;
}

#எனில் defined(FEAT_EVAL)
/*
 * Handle when "did_throw" is set after executing commands.
 */
    வெறுமை
handle_did_throw(வெறுமை)
{
    எழுத்து	*p = வெற்று;
    msglist_T	*messages = வெற்று;
    ESTACK_CHECK_DECLARATION;

    /*
     * If the uncaught exception is a user exception, report it as an
     * error.  If it is an error exception, display the saved error
     * message now.  For an interrupt exception, do nothing; the
     * interrupt message is given elsewhere.
     */
    தேர்வு (current_exception->type)
    {
	நிலை ET_USER:
	    vim_snprintf((எழுத்து *)IObuff, IOSIZE,
		    _(e_exception_not_caught_str),
		    current_exception->மதிப்பு);
	    p = (எழுத்து *)vim_strsave(IObuff);
	    நிறுத்து;
	நிலை ET_ERROR:
	    messages = current_exception->messages;
	    current_exception->messages = வெற்று;
	    நிறுத்து;
	நிலை ET_INTERRUPT:
	    நிறுத்து;
    }

    estack_தள்ளு(ETYPE_EXCEPT, current_exception->throw_name,
					current_exception->throw_lnum);
    ESTACK_CHECK_SETUP;
    current_exception->throw_name = வெற்று;

    discard_current_exception();	// uses IObuff if 'verbose'

    // If "silent!" is active the uncaught exception is not fatal.
    எனில் (emsg_silent == 0)
    {
	suppress_errthrow = TRUE;
	force_abort = TRUE;
    }

    எனில் (messages != வெற்று)
    {
	செய்
	{
	    msglist_T	*next = messages->next;
	    முழு		save_compiling = estack_compiling;

	    estack_compiling = messages->செய்தி_compiling;
	    emsg(messages->செய்தி);
	    vim_free(messages->செய்தி);
	    vim_free(messages->sfile);
	    vim_free(messages);
	    messages = next;
	    estack_compiling = save_compiling;
	}
	போது (messages != வெற்று);
    }
    இல்லையெனில் எனில் (p != வெற்று)
    {
	emsg(p);
	vim_free(p);
    }
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_எடு();
}

/*
 * Obtain a line when inside a ":while" or ":for" loop.
 */
    நிலையான char_u *
get_loop_வரி(முழு c, வெறுமை *cookie, முழு indent, getline_opt_T options)
{
    கட்டமைப்பு loop_cookie	*cp = (கட்டமைப்பு loop_cookie *)cookie;
    wcmd_T		*wp;
    char_u		*வரி;

    எனில் (cp->current_வரி + 1 >= cp->வரிகள்_gap->ga_len)
    {
	எனில் (cp->repeating)
	    திரும்பு வெற்று;	// trying to read past ":endwhile"/":endfor"

	// First time inside the ":while"/":for": get line normally.
	எனில் (cp->lc_getline == வெற்று)
	    வரி = getcmdline(c, 0L, indent, 0);
	இல்லையெனில்
	    வரி = cp->lc_getline(c, cp->cookie, indent, options);
	எனில் (வரி != வெற்று && store_loop_வரி(cp->வரிகள்_gap, வரி) == OK)
	    ++cp->current_வரி;

	திரும்பு வரி;
    }

    சாவி_Typed = FALSE;
    ++cp->current_வரி;
    wp = (wcmd_T *)(cp->வரிகள்_gap->ga_data) + cp->current_வரி;
    SOURCING_LNUM = wp->lnum;
    திரும்பு vim_strsave(wp->வரி);
}

/*
 * Store a line in "gap" so that a ":while" loop can execute it again.
 */
    நிலையான முழு
store_loop_வரி(garray_T *gap, char_u *வரி)
{
    எனில் (ga_grow(gap, 1) == FAIL)
	திரும்பு FAIL;
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].வரி = vim_strsave(வரி);
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].lnum = SOURCING_LNUM;
    ++gap->ga_len;
    திரும்பு OK;
}

/*
 * Free the lines stored for a ":while" or ":for" loop.
 */
    நிலையான வெறுமை
free_cmdlines(garray_T *gap)
{
    போது (gap->ga_len > 0)
    {
	vim_free(((wcmd_T *)(gap->ga_data))[gap->ga_len - 1].வரி);
	--gap->ga_len;
    }
}
#endif

/*
 * If "fgetline" is get_loop_line(), return TRUE if the getline it uses equals
 * "func".  * Otherwise return TRUE when "fgetline" equals "func".
 */
    முழு
getline_equal(
    char_u	*(*fgetline)(முழு, வெறுமை *, முழு, getline_opt_T),
    வெறுமை	*cookie UNUSED,		// argument for fgetline()
    char_u	*(*func)(முழு, வெறுமை *, முழு, getline_opt_T))
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(முழு, வெறுமை *, முழு, getline_opt_T);
    கட்டமைப்பு loop_cookie *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // function that's originally used to obtain the lines.  This may be
    // nested several levels.
    gp = fgetline;
    cp = (கட்டமைப்பு loop_cookie *)cookie;
    போது (gp == get_loop_வரி)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    திரும்பு gp == func;
#இல்லையெனில்
    திரும்பு fgetline == func;
#endif
}

/*
 * If "fgetline" is get_loop_line(), return the cookie used by the original
 * getline function.  Otherwise return "cookie".
 */
    வெறுமை *
getline_cookie(
    char_u	*(*fgetline)(முழு, வெறுமை *, முழு, getline_opt_T) UNUSED,
    வெறுமை	*cookie)		// argument for fgetline()
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(முழு, வெறுமை *, முழு, getline_opt_T);
    கட்டமைப்பு loop_cookie  *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (கட்டமைப்பு loop_cookie *)cookie;
    போது (gp == get_loop_வரி)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    திரும்பு cp;
#இல்லையெனில்
    திரும்பு cookie;
#endif
}

#எனில் defined(FEAT_EVAL)
/*
 * Get the next line source line without advancing.
 */
    char_u *
getline_peek(
    char_u	*(*fgetline)(முழு, வெறுமை *, முழு, getline_opt_T) UNUSED,
    வெறுமை	*cookie)		// argument for fgetline()
{
    char_u		*(*gp)(முழு, வெறுமை *, முழு, getline_opt_T);
    கட்டமைப்பு loop_cookie  *cp;
    wcmd_T		*wp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (கட்டமைப்பு loop_cookie *)cookie;
    போது (gp == get_loop_வரி)
    {
	எனில் (cp->current_வரி + 1 < cp->வரிகள்_gap->ga_len)
	{
	    // executing lines a second time, use the stored copy
	    wp = (wcmd_T *)(cp->வரிகள்_gap->ga_data) + cp->current_வரி + 1;
	    திரும்பு wp->வரி;
	}
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    எனில் (gp == getsourceline)
	திரும்பு source_nextline(cp);
    திரும்பு வெற்று;
}
#endif


/*
 * Helper function to apply an offset for buffer commands, i.e. ":bdelete",
 * ":bwipeout", etc.
 * Returns the buffer number.
 */
    நிலையான முழு
compute_buffer_local_மொத்தம்(முழு addr_type, முழு lnum, முழு offset)
{
    buf_T   *buf;
    buf_T   *nextbuf;
    முழு     மொத்தம் = offset;

    buf = firstbuf;
    போது (buf->b_next != வெற்று && buf->b_fnum < lnum)
	buf = buf->b_next;
    போது (மொத்தம் != 0)
    {
	மொத்தம் += (offset < 0) ? 1 : -1;
	nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
	எனில் (nextbuf == வெற்று)
	    நிறுத்து;
	buf = nextbuf;
	எனில் (addr_type == ADDR_LOADED_BUFFERS)
	    // skip over unloaded buffers
	    போது (buf->b_ml.ml_mfp == வெற்று)
	    {
		nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
		எனில் (nextbuf == வெற்று)
		    நிறுத்து;
		buf = nextbuf;
	    }
    }
    // we might have gone too far, last buffer is not loadedd
    எனில் (addr_type == ADDR_LOADED_BUFFERS)
	போது (buf->b_ml.ml_mfp == வெற்று)
	{
	    nextbuf = (offset >= 0) ? buf->b_prev : buf->b_next;
	    எனில் (nextbuf == வெற்று)
		நிறுத்து;
	    buf = nextbuf;
	}
    திரும்பு buf->b_fnum;
}

/*
 * Return the window number of "win".
 * When "win" is NULL return the number of windows.
 */
    நிலையான முழு
current_win_nr(win_T *win)
{
    win_T	*wp;
    முழு		nr = 0;

    FOR_ALL_WINDOWS(wp)
    {
	++nr;
	எனில் (wp == win)
	    நிறுத்து;
    }
    திரும்பு nr;
}

    நிலையான முழு
current_tab_nr(tabpage_T *tab)
{
    tabpage_T	*tp;
    முழு		nr = 0;

    FOR_ALL_TABPAGES(tp)
    {
	++nr;
	எனில் (tp == tab)
	    நிறுத்து;
    }
    திரும்பு nr;
}

    நிலையான முழு
comment_start(char_u *p, முழு starts_with_colon UNUSED)
{
    எனில் (in_vim9script())
	திரும்பு p[0] == '#' && !starts_with_colon;
    திரும்பு *p == '"';
}

#define CURRENT_WIN_NR current_win_nr(curwin)
#define LAST_WIN_NR current_win_nr(வெற்று)
#define CURRENT_TAB_NR current_tab_nr(curtab)
#define LAST_TAB_NR current_tab_nr(வெற்று)

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
    நிலையான char_u *
do_one_cmd(
    char_u	**cmdlinep,
    முழு		flags,
#ifdef FEAT_EVAL
    cstack_T	*cstack,
#endif
    char_u	*(*fgetline)(முழு, வெறுமை *, முழு, getline_opt_T),
    வெறுமை	*cookie)		// argument for fgetline()
{
    char_u	*p;
    linenr_T	lnum;
    long	n;
    எழுத்து	*errormsg = வெற்று;	// error message
    char_u	*after_modifier = வெற்று;
    exarg_T	ea;			// Ex command arguments
    cmdmod_T	save_cmdmod;
    முழு		save_reg_executing = reg_executing;
    முழு		save_pending_end_reg_executing = pending_end_reg_executing;
    முழு		ni;			// set when Not Implemented
    char_u	*cmd;
    முழு		starts_with_colon = FALSE;
    முழு		may_have_range;
#ifdef FEAT_EVAL
    முழு		did_set_கோவை_வரி = FALSE;
#endif
    முழு		sourcing = flags & DOCMD_VERBOSE;
    முழு		did_append_cmd = FALSE;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
#ifdef FEAT_EVAL
    ++ex_nesting_level;
#endif

    // When the last file has not been edited :q has to be typed twice.
    எனில் (quitmore
#ifdef FEAT_EVAL
	    // avoid that a function call in 'statusline' does this
	    && !getline_equal(fgetline, cookie, get_func_வரி)
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
    எனில் ((*cmdlinep)[0] == '#' && (*cmdlinep)[1] == '!')
	செல் doend;

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
    எனில் (parse_command_modifiers(&ea, &errormsg, &cmdmod, FALSE) == FAIL)
	செல் doend;
    apply_cmdmod(&cmdmod);
    after_modifier = ea.cmd;

#ifdef FEAT_EVAL
    ea.skip = did_emsg || got_int || did_throw || (cstack->cs_idx >= 0
			 && !(cstack->cs_flags[cstack->cs_idx] & CSF_ACTIVE));
#இல்லையெனில்
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
    முழு vim9script = in_vim9script();
    எனில் (vim9script && (flags & DOCMD_RANGEOK) == 0)
    {
	may_have_range = FALSE;
	க்கு (p = ea.cmd; p >= *cmdlinep; --p)
	{
	    எனில் (*p == ':')
		may_have_range = TRUE;
	    எனில் (p < ea.cmd && !VIM_ISWHITE(*p))
		நிறுத்து;
	}
    }
    இல்லையெனில்
	may_have_range = TRUE;
    எனில் (may_have_range)
	ea.cmd = skip_range(ea.cmd, TRUE, வெற்று);

#ifdef FEAT_EVAL
    // Handle ":export" - it functions almost like a command modifier.
    // ":export var Name: type"
    // ":export def Name(..."
    // etc.
    எனில் (vim9script && checkforcmd_noparen(&ea.cmd, "export", 6))
	is_export = TRUE;
#endif

    எனில் (vim9script && !may_have_range)
    {
	எனில் (ea.cmd == cmd + 1 && *cmd == '$')
	    // should be "$VAR = val"
	    --ea.cmd;
#ifdef FEAT_EVAL
	p = find_ex_command(&ea, வெற்று, lookup_scriptitem, வெற்று);
#இல்லையெனில்
	p = find_ex_command(&ea, வெற்று, வெற்று, வெற்று);
#endif
	எனில் (ea.cmdidx == CMD_SIZE)
	{
	    char_u *ar = skip_range(ea.cmd, TRUE, வெற்று);

	    // If a ':' before the range is missing, give a clearer error
	    // message.
	    எனில் (ar > ea.cmd && !ea.skip)
	    {
		semsg(_(e_colon_required_before_range_str), ea.cmd);
		செல் doend;
	    }
	}
    }
    இல்லையெனில்
	p = find_ex_command(&ea, வெற்று, வெற்று, வெற்று);

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    // Count this line for profiling if skip is TRUE.
    எனில் (do_profiling == PROF_YES
	    && (!ea.skip || cstack->cs_idx == 0 || (cstack->cs_idx > 0
		     && (cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE))))
    {
	முழு skip = did_emsg || got_int || did_throw;

	எனில் (ea.cmdidx == CMD_catch)
	    skip = !skip && !(cstack->cs_idx >= 0
			  && (cstack->cs_flags[cstack->cs_idx] & CSF_THROWN)
			  && !(cstack->cs_flags[cstack->cs_idx] & CSF_CAUGHT));
	இல்லையெனில் எனில் (ea.cmdidx == CMD_else || ea.cmdidx == CMD_elseif)
	    skip = skip || !(cstack->cs_idx >= 0
			  && !(cstack->cs_flags[cstack->cs_idx]
						  & (CSF_ACTIVE | CSF_TRUE)));
	இல்லையெனில் எனில் (ea.cmdidx == CMD_finally)
	    skip = FALSE;
	இல்லையெனில் எனில் (ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endtry
		&& ea.cmdidx != CMD_endwhile)
	    skip = ea.skip;

	எனில் (!skip)
	{
	    எனில் (getline_equal(fgetline, cookie, get_func_வரி))
		func_வரி_exec(getline_cookie(fgetline, cookie));
	    இல்லையெனில் எனில் (getline_equal(fgetline, cookie, getsourceline))
		script_வரி_exec();
	}
    }
# endif
#endif

    ea.cmd = cmd;

#ifdef FEAT_EVAL
    // May go to debug mode.  If this happens and the ">quit" debug command is
    // used, throw an interrupt exception and skip the next command.
    dbg_check_breakpoint(&ea);
    எனில் (!ea.skip && got_int)
    {
	ea.skip = TRUE;
	(வெறுமை)do_intthrow(cstack);
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
    எனில் (!IS_USER_CMDIDX(ea.cmdidx))
    {
	எனில் (ea.cmdidx != CMD_SIZE)
	    ea.addr_type = cmdnames[(முழு)ea.cmdidx].cmd_addr_type;
	இல்லையெனில்
	    ea.addr_type = ADDR_வரிகள்;

	// :wincmd range depends on the argument.
	எனில் (ea.cmdidx == CMD_wincmd && p != வெற்று)
	    get_wincmd_addr_type(skipwhite(p), &ea);
#ifdef FEAT_QUICKFIX
	// :.cc in quickfix window uses line number
	எனில் ((ea.cmdidx == CMD_cc || ea.cmdidx == CMD_ll) && bt_quickfix(curbuf))
	    ea.addr_type = ADDR_OTHER;
#endif
    }

    எனில் (!may_have_range)
	ea.line1 = ea.line2 = default_address(&ea);
    இல்லையெனில் எனில் (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
	செல் doend;

/*
 * 5. Parse the command.
 */

    /*
     * Skip ':' and any white space
     */
    ea.cmd = skipwhite(ea.cmd);
    போது (*ea.cmd == ':')
	ea.cmd = skipwhite(ea.cmd + 1);

    /*
     * If we got a line, but no command, then go to the line.
     * If we find a '|' or '\n' we set ea.nextcmd.
     */
    எனில் (*ea.cmd == NUL || comment_start(ea.cmd, starts_with_colon)
			       || (ea.nextcmd = check_nextcmd(ea.cmd)) != வெற்று)
    {
	/*
	 * strange vi behaviour:
	 * ":3"		jumps to line 3
	 * ":3|..."	prints line 3  (not in Vim9 script)
	 * ":|"		prints current line  (not in Vim9 script)
	 */
	எனில் (ea.skip)	    // skip this if inside :if
	    செல் doend;
	errormsg = ex_range_without_command(&ea);
	செல் doend;
    }

    // If this looks like an undefined user command and there are CmdUndefined
    // autocommands defined, trigger the matching autocommands.
    எனில் (p != வெற்று && ea.cmdidx == CMD_SIZE && !ea.skip
	    && ASCII_ISUPPER(*ea.cmd)
	    && has_cmdundefined())
    {
	முழு ret;

	p = ea.cmd;
	போது (ASCII_ISALNUM(*p))
	    ++p;
	p = vim_strnsave(ea.cmd, p - ea.cmd);
	ret = apply_autocmds(EVENT_CMDUNDEFINED, p, p, TRUE, வெற்று);
	vim_free(p);
	// If the autocommands did something and didn't cause an error, try
	// finding the command again.
	p = (ret
#ifdef FEAT_EVAL
		&& !aborting()
#endif
		) ? find_ex_command(&ea, வெற்று, வெற்று, வெற்று) : ea.cmd;
    }

    எனில் (p == வெற்று)
    {
	எனில் (!ea.skip)
	    errormsg = _(e_ambiguous_use_of_user_defined_command);
	செல் doend;
    }
    // Check for wrong commands.
    எனில் (*p == '!' && ea.cmd[1] == 0151 && ea.cmd[0] == 78
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	errormsg = uc_fun_cmd();
	செல் doend;
    }

    எனில் (ea.cmdidx == CMD_SIZE)
    {
	எனில் (!ea.skip)
	{
	    STRCPY(IObuff, _(e_not_an_editor_command));
	    எனில் (!sourcing)
	    {
		// If the modifier was parsed OK the error must be in the
		// following command
		எனில் (after_modifier != வெற்று)
		    append_command(after_modifier);
		இல்லையெனில்
		    append_command(*cmdlinep);
		did_append_cmd = TRUE;
	    }
	    errormsg = (எழுத்து *)IObuff;
	    did_emsg_syntax = TRUE;
	}
	செல் doend;
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
    எனில் (ea.cmdidx == CMD_if)
	++if_level;
    எனில் (if_level)
    {
	எனில் (ea.cmdidx == CMD_endif)
	    --if_level;
	செல் doend;
    }

#endif

    // forced commands
    எனில் (*p == '!' && ea.cmdidx != CMD_substitute
	    && ea.cmdidx != CMD_smagic && ea.cmdidx != CMD_snomagic)
    {
	++p;
	ea.forceit = TRUE;
    }
    இல்லையெனில்
	ea.forceit = FALSE;

/*
 * 6. Parse arguments.  Then check for errors.
 */
    எனில் (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = (long)cmdnames[(முழு)ea.cmdidx].cmd_argt;

    எனில் (!ea.skip)
    {
#ifdef HAVE_SANDBOX
	எனில் (sandbox != 0 && !(ea.argt & EX_SBOXOK))
	{
	    // Command not allowed in sandbox.
	    errormsg = _(e_not_allowed_in_sandbox);
	    செல் doend;
	}
#endif
	எனில் (restricted != 0 && (ea.argt & EX_RESTRICT))
	{
	    errormsg = _(e_command_not_allowed_in_rvim);
	    செல் doend;
	}
	எனில் (!curbuf->b_p_ma && (ea.argt & EX_MODIFY))
	{
	    // Command not allowed in non-'modifiable' buffer
	    errormsg = _(e_cannot_make_changes_modifiable_is_off);
	    செல் doend;
	}

	எனில் (!IS_USER_CMDIDX(ea.cmdidx))
	{
	    எனில் (cmdwin_type != 0 && !(ea.argt & EX_CMDWIN))
	    {
		// Command not allowed in the command line window
		errormsg = _(e_invalid_in_cmdline_window);
		செல் doend;
	    }
	    எனில் (text_locked() && !(ea.argt & EX_LOCK_OK))
	    {
		// Command not allowed when text is locked
		errormsg = _(get_text_locked_செய்தி());
		செல் doend;
	    }
	}

	// Disallow editing another buffer when "curbuf_lock" is set.
	// Do allow ":checktime" (it is postponed).
	// Do allow ":edit" (check for an argument later).
	// Do allow ":file" with no arguments (check for an argument later).
	எனில் (!(ea.argt & (EX_CMDWIN | EX_LOCK_OK))
		&& ea.cmdidx != CMD_checktime
		&& ea.cmdidx != CMD_edit
		&& ea.cmdidx != CMD_file
		&& !IS_USER_CMDIDX(ea.cmdidx)
		&& curbuf_locked())
	    செல் doend;

	எனில் (!ni && !(ea.argt & EX_RANGE) && ea.addr_மொத்தம் > 0)
	{
	    errormsg = _(e_no_range_allowed);
	    செல் doend;
	}
    }

    எனில் (!ni && !(ea.argt & EX_BANG) && ea.forceit)
    {
	errormsg = _(e_no_bang_allowed);
	செல் doend;
    }

    /*
     * Don't complain about the range if it is not used
     * (could happen if line_count is accidentally set to 0).
     */
    எனில் (!ea.skip && !ni && (ea.argt & EX_RANGE))
    {
	/*
	 * If the range is backwards, ask for confirmation and, if given, swap
	 * ea.line1 & ea.line2 so it's forwards again.
	 * When global command is busy, don't ask, will fail below.
	 */
	எனில் (!global_busy && ea.line1 > ea.line2)
	{
	    எனில் (செய்தி_silent == 0)
	    {
		எனில் (sourcing || exmode_active)
		{
		    errormsg = _(e_backwards_range_given);
		    செல் doend;
		}
		எனில் (ask_yesno((char_u *)
			_("Backwards range given, OK to swap"), FALSE) != 'y')
		    செல் doend;
	    }
	    lnum = ea.line1;
	    ea.line1 = ea.line2;
	    ea.line2 = lnum;
	}
	எனில் ((errormsg = invalid_range(&ea)) != வெற்று)
	    செல் doend;
    }

    எனில் ((ea.addr_type == ADDR_OTHER) && ea.addr_மொத்தம் == 0)
	// default is 1, not cursor
	ea.line2 = 1;

    correct_range(&ea);

#ifdef FEAT_FOLDING
    எனில் (((ea.argt & EX_WHOLEFOLD) || ea.addr_மொத்தம் >= 2) && !global_busy
	    && ea.addr_type == ADDR_வரிகள்)
    {
	// Put the first line at the start of a closed fold, put the last line
	// at the end of a closed fold.
	(வெறுமை)hasFolding(ea.line1, &ea.line1, வெற்று);
	(வெறுமை)hasFolding(ea.line2, வெற்று, &ea.line2);
    }
#endif

#ifdef FEAT_QUICKFIX
    /*
     * For the ":make" and ":grep" commands we insert the 'makeprg'/'grepprg'
     * option here, so things like % get expanded.
     */
    p = replace_makeprg(&ea, p, cmdlinep);
    எனில் (p == வெற்று)
	செல் doend;
#endif

    /*
     * Skip to start of argument.
     * Don't do this for the ":!" command, because ":!! -l" needs the space.
     */
    எனில் (ea.cmdidx == CMD_bang)
	ea.arg = p;
    இல்லையெனில்
	ea.arg = skipwhite(p);

    // ":file" cannot be run with an argument when "curbuf_lock" is set
    எனில் (ea.cmdidx == CMD_file && *ea.arg != NUL && curbuf_locked())
	செல் doend;

    /*
     * Check for "++opt=val" argument.
     * Must be first, allow ":w ++enc=utf8 !cmd"
     */
    எனில் (ea.argt & EX_ARGOPT)
	போது (ea.arg[0] == '+' && ea.arg[1] == '+')
	    எனில் (getargopt(&ea) == FAIL && !ni)
	    {
		errormsg = _(e_invalid_argument);
		செல் doend;
	    }

    எனில் (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	எனில் (*ea.arg == '>')			// append
	{
	    எனில் (*++ea.arg != '>')		// typed wrong
	    {
		errormsg = _(e_use_w_or_w_gt_gt);
		செல் doend;
	    }
	    ea.arg = skipwhite(ea.arg + 1);
	    ea.append = TRUE;
	}
	இல்லையெனில் எனில் (*ea.arg == '!' && ea.cmdidx == CMD_write)  // :w !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    எனில் (ea.cmdidx == CMD_read)
    {
	எனில் (ea.forceit)
	{
	    ea.usefilter = TRUE;		// :r! filter if ea.forceit
	    ea.forceit = FALSE;
	}
	இல்லையெனில் எனில் (*ea.arg == '!')		// :r !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    எனில் (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ea.amount = 1;
	போது (*ea.arg == *ea.cmd)		// count number of '>' or '<'
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
    எனில் ((ea.argt & EX_CMDARG) && !ea.usefilter)
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
    எனில் ((ea.argt & EX_TRLBAR) && !ea.usefilter)
    {
	separate_nextcmd(&ea, FALSE);
    }
    இல்லையெனில் எனில் (ea.cmdidx == CMD_bang
	    || ea.cmdidx == CMD_terminal
	    || ea.cmdidx == CMD_global
	    || ea.cmdidx == CMD_vglobal
	    || ea.usefilter
#ifdef FEAT_EVAL
	    || inside_block(&ea)
#endif
	    )
    {
	க்கு (p = ea.arg; *p; ++p)
	{
	    // Remove one backslash before a newline
	    எனில் (*p == '\\' && p[1] == '\n')
		STRMOVE(p, p + 1);
	    இல்லையெனில் எனில் (*p == '\n' && !(ea.argt & EX_கோவை_ARG))
	    {
		ea.nextcmd = p + 1;
		*p = NUL;
		நிறுத்து;
	    }
	}
    }

    எனில் ((ea.argt & EX_DFLALL) && ea.addr_மொத்தம் == 0)
	address_default_all(&ea);

    // accept numbered register only when no count allowed (:put)
    எனில் (       (ea.argt & EX_REGSTR)
	    && *ea.arg != NUL
	       // Do not allow register = for user commands
	    && (!IS_USER_CMDIDX(ea.cmdidx) || *ea.arg != '=')
	    && !((ea.argt & EX_மொத்தம்) && VIM_ISDIGIT(*ea.arg)))
    {
#எனில் !defined(FEAT_CLIPBOARD) && !defined(FEAT_CLIPBOARD_PROVIDER)
	// check these explicitly for a more specific error message
	எனில் (*ea.arg == '*' || *ea.arg == '+')
	{
	    errormsg = _(e_invalid_register_name);
	    செல் doend;
	}
#endif
	எனில் (valid_yank_reg(*ea.arg, (!IS_USER_CMDIDX(ea.cmdidx)
			    && ea.cmdidx != CMD_put && ea.cmdidx != CMD_iput)))
	{
	    ea.regname = *ea.arg++;
#ifdef FEAT_EVAL
	    // for '=' register: accept the rest of the line as an expression
	    எனில் (ea.arg[-1] == '=' && ea.arg[0] != NUL)
	    {
		எனில் (!ea.skip)
		{
		    set_கோவை_வரி(vim_strsave(ea.arg), &ea);
		    did_set_கோவை_வரி = TRUE;
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
    எனில் ((ea.argt & EX_மொத்தம்) && VIM_ISDIGIT(*ea.arg)
	    && (!(ea.argt & EX_BUFNAME) || *(p = skipdigits(ea.arg + 1)) == NUL
							  || VIM_ISWHITE(*p)))
    {
	n = getdigits_quoted(&ea.arg);
	ea.arg = skipwhite(ea.arg);
	எனில் (n <= 0 && !ni && (ea.argt & EX_ZEROR) == 0)
	{
	    errormsg = _(e_positive_மொத்தம்_required);
	    செல் doend;
	}
	எனில் (ea.addr_type != ADDR_வரிகள்)	// e.g. :buffer 2, :sleep 3
	{
	    ea.line2 = n;
	    எனில் (ea.addr_மொத்தம் == 0)
		ea.addr_மொத்தம் = 1;
	}
	இல்லையெனில்
	{
	    ea.line1 = ea.line2;
	    எனில் (ea.line2 >= LONG_MAX - (n - 1))
		ea.line2 = LONG_MAX;  // avoid overflow
	    இல்லையெனில்
		ea.line2 += n - 1;
	    ++ea.addr_மொத்தம்;
	    /*
	     * Be vi compatible: no error message for out of range.
	     */
	    எனில் (ea.line2 > curbuf->b_ml.ml_வரி_மொத்தம்)
		ea.line2 = curbuf->b_ml.ml_வரி_மொத்தம்;
	}
    }

    /*
     * Check for flags: 'l', 'p' and '#'.
     */
    எனில் (ea.argt & EX_FLAGS)
	get_flags(&ea);
    எனில் (!ni && !(ea.argt & EX_EXTRA) && *ea.arg != NUL
	    && *ea.arg != '"' && (*ea.arg != '|' || (ea.argt & EX_TRLBAR) == 0))
    {
	// no arguments allowed but there is something
	errormsg = ex_errmsg(e_trailing_characters_str, ea.arg);
	செல் doend;
    }

    எனில் (!ni && (ea.argt & EX_NEEDARG) && *ea.arg == NUL)
    {
	errormsg = _(e_argument_required);
	செல் doend;
    }

#ifdef FEAT_EVAL
    /*
     * Skip the command when it's not going to be executed.
     * The commands like :if, :endif, etc. always need to be executed.
     * Also make an exception for commands that handle a trailing command
     * themselves.
     */
    எனில் (ea.skip)
    {
	தேர்வு (ea.cmdidx)
	{
	    // commands that need evaluation
	    நிலை CMD_while:
	    நிலை CMD_endwhile:
	    நிலை CMD_for:
	    நிலை CMD_endfor:
	    நிலை CMD_if:
	    நிலை CMD_elseif:
	    நிலை CMD_else:
	    நிலை CMD_endif:
	    நிலை CMD_try:
	    நிலை CMD_catch:
	    நிலை CMD_finally:
	    நிலை CMD_endtry:
	    நிலை CMD_function:
	    நிலை CMD_def:
				நிறுத்து;

	    // commands that read a block of lines
	    நிலை CMD_abstract:
	    நிலை CMD_append:
	    நிலை CMD_autocmd:
	    நிலை CMD_change:
	    நிலை CMD_class:
	    நிலை CMD_command:
	    நிலை CMD_enum:
	    நிலை CMD_செருகு:
	    நிலை CMD_interface:
	    நிலை CMD_loadkeymap:
				நிறுத்து;

	    // Commands that handle '|' themselves.  Check: A command should
	    // either have the EX_TRLBAR flag, appear in this list or appear in
	    // the list at ":help :bar".
	    நிலை CMD_aboveleft:
	    நிலை CMD_and:
	    நிலை CMD_belowright:
	    நிலை CMD_botright:
	    நிலை CMD_browse:
	    நிலை CMD_call:
	    நிலை CMD_confirm:
	    நிலை CMD_const:
	    நிலை CMD_delfunction:
	    நிலை CMD_djump:
	    நிலை CMD_dlist:
	    நிலை CMD_dsearch:
	    நிலை CMD_dsplit:
	    நிலை CMD_echo:
	    நிலை CMD_echoerr:
	    நிலை CMD_echomsg:
	    நிலை CMD_echon:
	    நிலை CMD_eval:
	    நிலை CMD_execute:
	    நிலை CMD_filter:
	    நிலை CMD_final:
	    நிலை CMD_help:
	    நிலை CMD_hide:
	    நிலை CMD_horizontal:
	    நிலை CMD_ijump:
	    நிலை CMD_ilist:
	    நிலை CMD_isearch:
	    நிலை CMD_isplit:
	    நிலை CMD_keepalt:
	    நிலை CMD_keepjumps:
	    நிலை CMD_keepmarks:
	    நிலை CMD_keeppatterns:
	    நிலை CMD_leftabove:
	    நிலை CMD_let:
	    நிலை CMD_lockmarks:
	    நிலை CMD_lockvar:
	    நிலை CMD_lua:
	    நிலை CMD_match:
	    நிலை CMD_mzscheme:
	    நிலை CMD_noautocmd:
	    நிலை CMD_noswapfile:
	    நிலை CMD_perl:
	    நிலை CMD_psearch:
	    நிலை CMD_py3:
	    நிலை CMD_python3:
	    நிலை CMD_python:
	    நிலை CMD_return:
	    நிலை CMD_rightbelow:
	    நிலை CMD_ruby:
	    நிலை CMD_silent:
	    நிலை CMD_smagic:
	    நிலை CMD_snomagic:
	    நிலை CMD_substitute:
	    நிலை CMD_syntax:
	    நிலை CMD_tab:
	    நிலை CMD_tcl:
	    நிலை CMD_throw:
	    நிலை CMD_tilde:
	    நிலை CMD_topleft:
	    நிலை CMD_unlet:
	    நிலை CMD_unlockvar:
	    நிலை CMD_var:
	    நிலை CMD_verbose:
	    நிலை CMD_vertical:
	    நிலை CMD_wincmd:
				நிறுத்து;

	    இயல்பு:		செல் doend;
	}
    }
#endif

    எனில் ((ea.argt & EX_XFILE)
	    && expand_filename(&ea, cmdlinep, &errormsg) == FAIL)
	செல் doend;

#ifdef FEAT_EVAL
    எனில் (is_export && (ea.argt & EX_EXPORT) == 0)
    {
	emsg(_(e_invalid_command_after_export));
	செல் doend;
    }
#endif

    /*
     * Accept buffer name.  Cannot be used at the same time with a buffer
     * number.  Don't do this for a user command.
     */
    எனில் ((ea.argt & EX_BUFNAME) && *ea.arg != NUL && ea.addr_மொத்தம் == 0
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * :bdelete, :bwipeout and :bunload take several arguments, separated
	 * by spaces: find next space (skipping over escaped characters).
	 * The others take one argument: ignore trailing spaces.
	 */
	எனில் (ea.cmdidx == CMD_bdelete || ea.cmdidx == CMD_bwipeout
						  || ea.cmdidx == CMD_bunload)
	    p = skiptowhite_esc(ea.arg);
	இல்லையெனில்
	{
	    p = ea.arg + STRLEN(ea.arg);
	    போது (p > ea.arg && VIM_ISWHITE(p[-1]))
		--p;
	}
	ea.line2 = buflist_findpat(ea.arg, p, (ea.argt & EX_BUFUNL) != 0,
								FALSE, FALSE);
	எனில் (ea.line2 < 0)	    // failed
	    செல் doend;
	ea.addr_மொத்தம் = 1;
	ea.arg = skipwhite(p);
    }

    // The :try command saves the emsg_silent flag, reset it here when
    // ":silent! try" was used, it should only apply to :try itself.
    எனில் (ea.cmdidx == CMD_try && cmdmod.cmod_did_esilent > 0)
    {
	emsg_silent -= cmdmod.cmod_did_esilent;
	எனில் (emsg_silent < 0)
	    emsg_silent = 0;
	cmdmod.cmod_did_esilent = 0;
    }

/*
 * 7. Execute the command.
 */

    எனில் (IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * Execute a user-defined command.
	 */
	do_ucmd(&ea);
    }
    இல்லையெனில்
    {
	/*
	 * Call the function to execute the builtin command.
	 */
	(cmdnames[ea.cmdidx].cmd_func)(&ea);
	எனில் (ea.errmsg != வெற்று)
	    errormsg = ea.errmsg;
    }

#ifdef FEAT_EVAL
    // A command will reset "is_export" when exporting an item.  If it is still
    // set something went wrong or the command was never executed.
    எனில் (!ea.skip && is_export)
    {
	எனில் (errormsg == வெற்று)
	    errormsg = _(e_export_with_invalid_argument);
	is_export = FALSE;
    }

    // Set flag that any command was executed, used by ex_vim9script().
    // Not if this was a command that wasn't executed or :endif.
    எனில் (sourcing_a_script(&ea)
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
    எனில் (need_rethrow)
	do_throw(cstack);
    இல்லையெனில் எனில் (check_cstack)
    {
	எனில் (source_finished(fgetline, cookie))
	    do_finish(&ea, TRUE);
	இல்லையெனில் எனில் (getline_equal(fgetline, cookie, get_func_வரி)
						   && current_func_returned())
	    do_return(&ea, TRUE, FALSE, வெற்று);
    }
    need_rethrow = check_cstack = FALSE;
#endif

doend:
    எனில் (curwin->w_cursor.lnum == 0)	// can happen with zero line number
    {
	curwin->w_cursor.lnum = 1;
	curwin->w_cursor.col = 0;
    }

    எனில் (errormsg != வெற்று && *errormsg != NUL && !did_emsg)
    {
	எனில் ((sourcing || !சாவி_Typed) && !did_append_cmd)
	{
	    எனில் (errormsg != (எழுத்து *)IObuff)
	    {
		STRCPY(IObuff, errormsg);
		errormsg = (எழுத்து *)IObuff;
	    }
	    append_command(*cmdlinep);
	}
	emsg(errormsg);
    }
#ifdef FEAT_EVAL
    do_errthrow(cstack,
	    (ea.cmdidx != CMD_SIZE && !IS_USER_CMDIDX(ea.cmdidx))
			? cmdnames[(முழு)ea.cmdidx].cmd_name : (char_u *)வெற்று);

    எனில் (did_set_கோவை_வரி)
	set_கோவை_வரி(வெற்று, வெற்று);
    is_export = FALSE;
#endif

    undo_cmdmod(&cmdmod);
    cmdmod = save_cmdmod;
    reg_executing = save_reg_executing;
    pending_end_reg_executing = save_pending_end_reg_executing;

    எனில் (ea.nextcmd && *ea.nextcmd == NUL)	// not really a next command
	ea.nextcmd = வெற்று;

#ifdef FEAT_EVAL
    --ex_nesting_level;
    vim_free(ea.cmdline_tofree);
#endif

    திரும்பு ea.nextcmd;
}

நிலையான எழுத்து ex_error_buf[செய்தி_BUF_LEN];

/*
 * Return an error message with argument included.
 * Uses a static buffer, only the last error will be kept.
 * "msg" will be translated, caller should use N_().
 */
    எழுத்து *
ex_errmsg(எழுத்து *செய்தி, char_u *arg)
{
    vim_snprintf(ex_error_buf, செய்தி_BUF_LEN, _(செய்தி), arg);
    திரும்பு ex_error_buf;
}

/*
 * The "+" string used in place of an empty command in Ex mode.
 * This string is used in pointer comparison.
 */
நிலையான மாறிலி எழுத்து exmode_plus[] = "+";

/*
 * Handle a range without a command.
 * Returns an error message on failure.
 */
    எழுத்து *
ex_range_without_command(exarg_T *eap)
{
    எழுத்து *errormsg = வெற்று;

    எனில் ((*eap->cmd == '|' ||
		(exmode_active && eap->cmd != (char_u *)exmode_plus + 1))
#ifdef FEAT_EVAL
	    && !in_vim9script()
#endif
       )
    {
	eap->cmdidx = CMD_print;
	eap->argt = EX_RANGE+EX_மொத்தம்+EX_TRLBAR;
	எனில் ((errormsg = invalid_range(eap)) == வெற்று)
	{
	    correct_range(eap);
	    ex_print(eap);
	}
    }
    இல்லையெனில் எனில் (eap->addr_மொத்தம் != 0)
    {
	எனில் (eap->line2 > curbuf->b_ml.ml_வரி_மொத்தம்)
	{
	    // With '-' in 'cpoptions' a line number past the file is an
	    // error, otherwise put it at the end of the file.
	    எனில் (vim_strchr(p_cpo, CPO_MINUS) != வெற்று)
		eap->line2 = -1;
	    இல்லையெனில்
		eap->line2 = curbuf->b_ml.ml_வரி_மொத்தம்;
	}

	எனில் (eap->line2 < 0)
	    errormsg = _(e_invalid_range);
	இல்லையெனில்
	{
	    எனில் (eap->line2 == 0)
		curwin->w_cursor.lnum = 1;
	    இல்லையெனில்
		curwin->w_cursor.lnum = eap->line2;
	    beginline(BL_SOL | BL_FIX);
	}
    }
    திரும்பு errormsg;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 * If "noparen" is TRUE do not recognize the command followed by "(" or ".".
 */
    நிலையான முழு
checkforcmd_opt(
    char_u	**pp,		// start of command
    எழுத்து	*cmd,		// name of command
    முழு		len,		// required length
    முழு		noparen)
{
    முழு		i;

    க்கு (i = 0; cmd[i] != NUL; ++i)
	எனில் (((char_u *)cmd)[i] != (*pp)[i])
	    நிறுத்து;
    எனில் (i >= len && !ASCII_ISALPHA((*pp)[i]) && (*pp)[i] != '_'
			 && (!noparen || ((*pp)[i] != '(' && (*pp)[i] != '.')))
    {
	*pp = skipwhite(*pp + i);
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    முழு
checkforcmd(
    char_u	**pp,		// start of command
    எழுத்து	*cmd,		// name of command
    முழு		len)		// required length
{
    திரும்பு checkforcmd_opt(pp, cmd, len, FALSE);
}

/*
 * Check for an Ex command with optional tail, not followed by "(" or ".".
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    முழு
checkforcmd_noparen(
    char_u	**pp,		// start of command
    எழுத்து	*cmd,		// name of command
    முழு		len)		// required length
{
    திரும்பு checkforcmd_opt(pp, cmd, len, TRUE);
}

/*
 * Find the replacement text of a ":command", the part after the attributes and
 * the command name.  Returns NULL when there is none.  Does not modify "arg"
 * and gives no error messages.
 */
    நிலையான char_u *
find_ucmd_repl(char_u *arg)
{
    char_u	*p = arg;

    // Skip over the attributes.
    போது (*p == '-')
	p = skipwhite(skiptowhite(p));

    // Skip over the command name.
    எனில் (!ASCII_ISALPHA(*p))
	திரும்பு வெற்று;
    போது (ASCII_ISALNUM(*p))
	++p;

    திரும்பு *p == NUL ? வெற்று : skipwhite(p);
}

/*
 * Find the "{" in "line" that starts a block for ":command" or ":autocmd".
 * That is the case when the command argument is "{" at the end of the line,
 * also when the command is nested in another ":command" or ":autocmd".
 * Returns NULL when the line does not start such a block.
 */
    char_u *
find_cmd_block_start(char_u *வரி)
{
    char_u	*p = skipwhite(வரி);

    க்கு (;;)
    {
	char_u	*arg = p;

	எனில் (*p == '{' && ends_excmd2(p, skipwhite(p + 1)))
	    திரும்பு p;

	எனில் (checkforcmd_noparen(&arg, "autocmd", 2))
	{
	    எனில் (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = au_find_cmd_arg(arg);
	}
	இல்லையெனில் எனில் (checkforcmd_noparen(&arg, "command", 3))
	{
	    எனில் (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = find_ucmd_repl(arg);
	}
	இல்லையெனில்
	    திரும்பு வெற்று;

	எனில் (p == வெற்று)
	    திரும்பு வெற்று;
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
    முழு
parse_command_modifiers(
	exarg_T	    *eap,
	எழுத்து	    **errormsg,
	cmdmod_T    *cmod,
	முழு	    skip_only)
{
    char_u  *orig_cmd = eap->cmd;
    char_u  *cmd_start = வெற்று;
    முழு	    use_plus_cmd = FALSE;
    முழு	    starts_with_colon = FALSE;
    முழு	    vim9script = in_vim9script();
    முழு	    has_visual_range = FALSE;

    CLEAR_POINTER(cmod);
    cmod->cmod_flags = sticky_cmdmod_flags;

    எனில் (STRNCMP(eap->cmd, "'<,'>", 5) == 0)
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
    க்கு (;;)
    {
	char_u  *p;

	போது (*eap->cmd == ' ' || *eap->cmd == '\t' || *eap->cmd == ':')
	{
	    எனில் (*eap->cmd == ':')
		starts_with_colon = TRUE;
	    ++eap->cmd;
	}

	// in ex mode, an empty command (after modifiers) works like :+
	எனில் (*eap->cmd == NUL && exmode_active
		   && (getline_equal(eap->ea_getline, eap->cookie, getexmodeline)
		       || getline_equal(eap->ea_getline, eap->cookie, getexline))
			&& curwin->w_cursor.lnum < curbuf->b_ml.ml_வரி_மொத்தம்)
	{
	    use_plus_cmd = TRUE;
	    எனில் (!skip_only)
		ex_pressedreturn = TRUE;
	    நிறுத்து;  // no modifiers following
	}

	// ignore comment and empty lines
	எனில் (comment_start(eap->cmd, starts_with_colon))
	{
	    // a comment ends at a NL
	    eap->nextcmd = vim_strchr(eap->cmd, '\n');
	    எனில் (eap->nextcmd != வெற்று)
		++eap->nextcmd;
	    எனில் (vim9script)
	    {
		எனில் (has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
#ifdef FEAT_EVAL
		எனில் (eap->cmd[0] == '#' && eap->cmd[1] == '{'
							 && eap->cmd[2] != '{')
		    *errormsg = _(e_cannot_use_hash_curly_to_start_comment);
#endif
	    }
	    திரும்பு FAIL;
	}
	எனில் (*eap->cmd == '\n')
	{
	    eap->nextcmd = eap->cmd + 1;
	    திரும்பு FAIL;
	}
	எனில் (*eap->cmd == NUL)
	{
	    எனில் (!skip_only)
	    {
		ex_pressedreturn = TRUE;
		எனில் (vim9script && has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
	    }
	    திரும்பு FAIL;
	}

	p = skip_range(eap->cmd, TRUE, வெற்று);

	// In Vim9 script a variable can shadow a command modifier:
	//   verbose = 123
	//   verbose += 123
	//   silent! verbose = func()
	//   verbose.member = 2
	//   verbose[expr] = 2
	// But not:
	//   verbose [a, b] = list
	எனில் (vim9script)
	{
	    char_u *s, *n;

	    க்கு (s = eap->cmd; ASCII_ISALPHA(*s); ++s)
		;
	    n = skipwhite(s);
	    எனில் (*n == '.' || *n == '=' || (*n != NUL && n[1] == '=')
		    || *s == '[')
		நிறுத்து;
	}

	தேர்வு (*p)
	{
	    // When adding an entry, also modify cmdmod_info_tab[].
	    நிலை 'a':	எனில் (!checkforcmd_noparen(&eap->cmd, "aboveleft", 3))
			    நிறுத்து;
			cmod->cmod_split |= WSP_ABOVE;
			தொடர்;

	    நிலை 'b':	எனில் (checkforcmd_noparen(&eap->cmd, "belowright", 3))
			{
			    cmod->cmod_split |= WSP_BELOW;
			    தொடர்;
			}
			எனில் (checkforcmd_opt(&eap->cmd, "browse", 3, TRUE))
			{
#ifdef FEAT_BROWSE_CMD
			    cmod->cmod_flags |= CMOD_BROWSE;
#endif
			    தொடர்;
			}
			எனில் (!checkforcmd_noparen(&eap->cmd, "botright", 2))
			    நிறுத்து;
			cmod->cmod_split |= WSP_BOT;
			தொடர்;

	    நிலை 'c':	எனில் (!checkforcmd_opt(&eap->cmd, "confirm", 4, TRUE))
			    நிறுத்து;
#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			cmod->cmod_flags |= CMOD_CONFIRM;
#endif
			தொடர்;

	    நிலை 'k':	எனில் (checkforcmd_noparen(&eap->cmd, "keepmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_KEEPMARKS;
			    தொடர்;
			}
			எனில் (checkforcmd_noparen(&eap->cmd, "keepalt", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPALT;
			    தொடர்;
			}
			எனில் (checkforcmd_noparen(&eap->cmd, "keeppatterns", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPPATTERNS;
			    தொடர்;
			}
			எனில் (!checkforcmd_noparen(&eap->cmd, "keepjumps", 5))
			    நிறுத்து;
			cmod->cmod_flags |= CMOD_KEEPJUMPS;
			தொடர்;

	    நிலை 'f':	// only accept ":filter {pat} cmd"
			{
			    char_u  *reg_pat;
			    char_u  *nulp = வெற்று;
			    முழு	    c = 0;

			    எனில் (!checkforcmd_noparen(&p, "filter", 4)
				    || *p == NUL
				    || (ends_excmd(*p)
#ifdef FEAT_EVAL
					// in ":filter #pat# cmd" # does not
					// start a comment
				     && (!vim9script || VIM_ISWHITE(p[1]))
#endif
				     ))
				நிறுத்து;
			    எனில் (*p == '!')
			    {
				cmod->cmod_filter_force = TRUE;
				p = skipwhite(p + 1);
				எனில் (*p == NUL || ends_excmd(*p))
				    நிறுத்து;
			    }
#ifdef FEAT_EVAL
			    // Avoid that "filter(arg)" is recognized.
			    எனில் (vim9script && !VIM_ISWHITE(p[-1]))
				நிறுத்து;
#endif
			    எனில் (skip_only)
				p = skip_vimgrep_pat(p, வெற்று, வெற்று);
			    இல்லையெனில்
				// NOTE: This puts a NUL after the pattern.
				p = skip_vimgrep_pat_ext(p, &reg_pat, வெற்று,
								    &nulp, &c);
			    எனில் (p == வெற்று || *p == NUL)
				நிறுத்து;
			    எனில் (!skip_only)
			    {
				cmod->cmod_filter_regmatch.regprog =
						vim_regcomp(reg_pat, RE_MAGIC);
				எனில் (cmod->cmod_filter_regmatch.regprog == வெற்று)
				    நிறுத்து;
				// restore the character overwritten by NUL
				எனில் (nulp != வெற்று)
				    *nulp = c;
			    }
			    eap->cmd = p;
			    தொடர்;
			}

	    நிலை 'h':	எனில் (checkforcmd_noparen(&eap->cmd, "horizontal", 3))
			{
			    cmod->cmod_split |= WSP_HOR;
			    தொடர்;
			}
			// ":hide" and ":hide | cmd" are not modifiers
			எனில் (p != eap->cmd || !checkforcmd_noparen(&p, "hide", 3)
					       || *p == NUL || ends_excmd(*p))
			    நிறுத்து;
			eap->cmd = p;
			cmod->cmod_flags |= CMOD_HIDE;
			தொடர்;

	    நிலை 'l':	எனில் (checkforcmd_noparen(&eap->cmd, "lockmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_LOCKMARKS;
			    தொடர்;
			}
			எனில் (checkforcmd_noparen(&eap->cmd, "legacy", 3))
			{
			    எனில் (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_legacy_must_be_followed_by_command);
				திரும்பு FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_VIM9CMD;
			    cmod->cmod_flags |= CMOD_LEGACY;
			    தொடர்;
			}

			எனில் (!checkforcmd_noparen(&eap->cmd, "leftabove", 5))
			    நிறுத்து;
			cmod->cmod_split |= WSP_ABOVE;
			தொடர்;

	    நிலை 'n':	எனில் (checkforcmd_noparen(&eap->cmd, "noautocmd", 3))
			{
			    cmod->cmod_flags |= CMOD_NOAUTOCMD;
			    தொடர்;
			}
			எனில் (!checkforcmd_noparen(&eap->cmd, "noswapfile", 3))
			    நிறுத்து;
			cmod->cmod_flags |= CMOD_NOSWAPFILE;
			தொடர்;

	    நிலை 'r':	எனில் (!checkforcmd_noparen(&eap->cmd, "rightbelow", 6))
			    நிறுத்து;
			cmod->cmod_split |= WSP_BELOW;
			தொடர்;

	    நிலை 's':	எனில் (checkforcmd_noparen(&eap->cmd, "sandbox", 3))
			{
			    cmod->cmod_flags |= CMOD_SANDBOX;
			    தொடர்;
			}
			எனில் (!checkforcmd_noparen(&eap->cmd, "silent", 3))
			    நிறுத்து;
			cmod->cmod_flags |= CMOD_SILENT;
			எனில் (*eap->cmd == '!' && !VIM_ISWHITE(eap->cmd[-1]))
			{
			    // ":silent!", but not "silent !cmd"
			    eap->cmd = skipwhite(eap->cmd + 1);
			    cmod->cmod_flags |= CMOD_ERRSILENT;
			}
			தொடர்;

	    நிலை 't':	எனில் (checkforcmd_noparen(&p, "tab", 3))
			{
			    எனில் (!skip_only)
			    {
				long tabnr = get_address(eap, &eap->cmd,
						    ADDR_TABS, eap->skip,
						    skip_only, FALSE, 1);
				எனில் (tabnr == MAXLNUM)
				    cmod->cmod_tab = tabpage_index(curtab) + 1;
				இல்லையெனில்
				{
				    எனில் (tabnr < 0 || tabnr > LAST_TAB_NR)
				    {
					*errormsg = _(e_invalid_range);
					திரும்பு FAIL;
				    }
				    cmod->cmod_tab = tabnr + 1;
				}
			    }
			    eap->cmd = p;
			    தொடர்;
			}
			எனில் (!checkforcmd_noparen(&eap->cmd, "topleft", 2))
			    நிறுத்து;
			cmod->cmod_split |= WSP_உச்சி;
			தொடர்;

	    நிலை 'u':	எனில் (!checkforcmd_noparen(&eap->cmd, "unsilent", 3))
			    நிறுத்து;
			cmod->cmod_flags |= CMOD_UNSILENT;
			தொடர்;

	    நிலை 'v':	எனில் (checkforcmd_noparen(&eap->cmd, "vertical", 4))
			{
			    cmod->cmod_split |= WSP_VERT;
			    தொடர்;
			}
			எனில் (checkforcmd_noparen(&eap->cmd, "vim9cmd", 4))
			{
			    எனில் (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_vim9cmd_must_be_followed_by_command);
				திரும்பு FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_LEGACY;
			    cmod->cmod_flags |= CMOD_VIM9CMD;
			    தொடர்;
			}
			எனில் (!checkforcmd_noparen(&p, "verbose", 4))
			    நிறுத்து;
			எனில் (vim_isdigit(*eap->cmd))
			{
			    // zero means not set, one is verbose == 0, etc.
			    cmod->cmod_verbose = atoi((எழுத்து *)eap->cmd) + 1;
			}
			இல்லையெனில்
			    cmod->cmod_verbose = 2;  // default: verbose == 1
			eap->cmd = p;
			தொடர்;
	}
	நிறுத்து;
    }

    எனில் (has_visual_range)
    {
	எனில் (eap->cmd > cmd_start)
	{
	    // Move the '<,'> range to after the modifiers and insert a colon.
	    // Since the modifiers have been parsed put the colon on top of the
	    // space: "'<,'>mod cmd" -> "mod:'<,'>cmd
	    // Put eap->cmd after the colon.
	    எனில் (use_plus_cmd)
	    {
		size_t len = STRLEN(cmd_start);

		// Special case: empty command uses "+":
		//  "'<,'>mods" -> "mods *+
		//  Use "*" instead of "'<,'>" to avoid the command getting
		//  longer, in case it was allocated.
		mch_memmove(orig_cmd, cmd_start, len);
		STRCPY(orig_cmd + len, " *+");
	    }
	    இல்லையெனில்
	    {
		mch_memmove(cmd_start - 5, cmd_start, eap->cmd - cmd_start);
		eap->cmd -= 5;
		mch_memmove(eap->cmd - 1, ":'<,'>", 6);
	    }
	}
	இல்லையெனில்
	    // No modifiers, move the pointer back.
	    // Special case: change empty command to "+".
	    எனில் (use_plus_cmd)
		eap->cmd = (char_u *)"'<,'>+";
	    இல்லையெனில்
		eap->cmd = orig_cmd;
    }
    இல்லையெனில் எனில் (use_plus_cmd)
	eap->cmd = (char_u *)exmode_plus;

    திரும்பு OK;
}

/*
 * Return TRUE if "cmod" has anything set.
 */
    முழு
has_cmdmod(cmdmod_T *cmod, முழு ignore_silent)
{
    திரும்பு (cmod->cmod_flags != 0 && (!ignore_silent
		|| (cmod->cmod_flags
		      & ~(CMOD_SILENT | CMOD_ERRSILENT | CMOD_UNSILENT)) != 0))
	    || cmod->cmod_split != 0
	    || cmod->cmod_verbose > 0
	    || cmod->cmod_tab != 0
	    || cmod->cmod_filter_regmatch.regprog != வெற்று;
}

#எனில் defined(FEAT_EVAL)
/*
 * If Vim9 script and "cmdmod" has anything set give an error and return TRUE.
 */
    முழு
cmdmod_error(முழு ignore_silent)
{
    எனில் (in_vim9script() && has_cmdmod(&cmdmod, ignore_silent))
    {
	emsg(_(e_misplaced_command_modifier));
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}
#endif

/*
 * Apply the command modifiers.  Saves current state in "cmdmod", call
 * undo_cmdmod() later.
 */
    வெறுமை
apply_cmdmod(cmdmod_T *cmod)
{
#ifdef HAVE_SANDBOX
    எனில் ((cmod->cmod_flags & CMOD_SANDBOX) && !cmod->cmod_did_sandbox)
    {
	++sandbox;
	cmod->cmod_did_sandbox = TRUE;
    }
#endif
    எனில் (cmod->cmod_verbose > 0)
    {
	எனில் (cmod->cmod_verbose_save == 0)
	    cmod->cmod_verbose_save = p_verbose + 1;
	p_verbose = cmod->cmod_verbose - 1;
    }

    எனில் ((cmod->cmod_flags & (CMOD_SILENT | CMOD_UNSILENT))
	    && cmod->cmod_save_செய்தி_silent == 0)
    {
	cmod->cmod_save_செய்தி_silent = செய்தி_silent + 1;
	cmod->cmod_save_செய்தி_scroll = செய்தி_scroll;
    }
    எனில் (cmod->cmod_flags & CMOD_SILENT)
	++செய்தி_silent;
    எனில் (cmod->cmod_flags & CMOD_UNSILENT)
	செய்தி_silent = 0;

    எனில் (cmod->cmod_flags & CMOD_ERRSILENT)
    {
	++emsg_silent;
	++cmod->cmod_did_esilent;
    }

    எனில் ((cmod->cmod_flags & CMOD_NOAUTOCMD) && cmod->cmod_save_ei == வெற்று)
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
    வெறுமை
undo_cmdmod(cmdmod_T *cmod)
{
    எனில் (cmod->cmod_verbose_save > 0)
    {
	p_verbose = cmod->cmod_verbose_save - 1;
	cmod->cmod_verbose_save = 0;
    }

#ifdef HAVE_SANDBOX
    எனில் (cmod->cmod_did_sandbox)
    {
	--sandbox;
	cmod->cmod_did_sandbox = FALSE;
    }
#endif

    எனில் (cmod->cmod_save_ei != வெற்று)
    {
	// Restore 'eventignore' to the value before ":noautocmd".
	set_string_option_direct((char_u *)"ei", -1, cmod->cmod_save_ei,
							   OPT_FREE, SID_NONE);
	free_string_option(cmod->cmod_save_ei);
	cmod->cmod_save_ei = வெற்று;
    }

    vim_regfree(cmod->cmod_filter_regmatch.regprog);

    எனில் (cmod->cmod_save_செய்தி_silent > 0)
    {
	// messages could be enabled for a serious error, need to check if the
	// counters don't become negative
	எனில் (!did_emsg || செய்தி_silent > cmod->cmod_save_செய்தி_silent - 1)
	    செய்தி_silent = cmod->cmod_save_செய்தி_silent - 1;
	emsg_silent -= cmod->cmod_did_esilent;
	எனில் (emsg_silent < 0)
	    emsg_silent = 0;
	// Restore msg_scroll, it's set by file I/O commands, even when no
	// message is actually displayed.
	செய்தி_scroll = cmod->cmod_save_செய்தி_scroll;

	// "silent reg" or "silent echo x" inside "redir" leaves msg_col
	// somewhere in the line.  Put it back in the first column.
	எனில் (redirecting())
	    செய்தி_col = 0;

	cmod->cmod_save_செய்தி_silent = 0;
	cmod->cmod_did_esilent = 0;
    }
}

/*
 * Parse the address range, if any, in "eap".
 * May set the last search pattern, unless "silent" is TRUE.
 * Return FAIL and set "errormsg" or return OK.
 */
    முழு
parse_cmd_address(exarg_T *eap, எழுத்து **errormsg, முழு silent)
{
    முழு		address_மொத்தம் = 1;
    linenr_T	lnum;
    முழு		need_check_cursor = FALSE;
    முழு		ret = FAIL;

    // Repeat for all ',' or ';' separated addresses.
    க்கு (;;)
    {
	eap->line1 = eap->line2;
	eap->line2 = default_address(eap);
	eap->cmd = skipwhite(eap->cmd);
	lnum = get_address(eap, &eap->cmd, eap->addr_type, eap->skip, silent,
					eap->addr_மொத்தம் == 0, address_மொத்தம்++);
	எனில் (eap->cmd == வெற்று)	// error detected
	    செல் theend;
	எனில் (lnum == MAXLNUM)
	{
	    எனில் (*eap->cmd == '%')   // '%' - all lines
	    {
		++eap->cmd;
		தேர்வு (eap->addr_type)
		{
		    நிலை ADDR_வரிகள்:
		    நிலை ADDR_OTHER:
			eap->line1 = 1;
			eap->line2 = curbuf->b_ml.ml_வரி_மொத்தம்;
			நிறுத்து;
		    நிலை ADDR_LOADED_BUFFERS:
			{
			    buf_T	*buf = firstbuf;

			    போது (buf->b_next != வெற்று
						  && buf->b_ml.ml_mfp == வெற்று)
				buf = buf->b_next;
			    eap->line1 = buf->b_fnum;
			    buf = lastbuf;
			    போது (buf->b_prev != வெற்று
						  && buf->b_ml.ml_mfp == வெற்று)
				buf = buf->b_prev;
			    eap->line2 = buf->b_fnum;
			    நிறுத்து;
			}
		    நிலை ADDR_BUFFERS:
			eap->line1 = firstbuf->b_fnum;
			eap->line2 = lastbuf->b_fnum;
			நிறுத்து;
		    நிலை ADDR_WINDOWS:
		    நிலை ADDR_TABS:
			எனில் (IS_USER_CMDIDX(eap->cmdidx))
			{
			    eap->line1 = 1;
			    eap->line2 = eap->addr_type == ADDR_WINDOWS
						  ? LAST_WIN_NR : LAST_TAB_NR;
			}
			இல்லையெனில்
			{
			    // there is no Vim command which uses '%' and
			    // ADDR_WINDOWS or ADDR_TABS
			    *errormsg = _(e_invalid_range);
			    செல் theend;
			}
			நிறுத்து;
		    நிலை ADDR_TABS_RELATIVE:
		    நிலை ADDR_UNSIGNED:
		    நிலை ADDR_QUICKFIX:
			*errormsg = _(e_invalid_range);
			செல் theend;
		    நிலை ADDR_ARGUMENTS:
			எனில் (ARGCOUNT == 0)
			    eap->line1 = eap->line2 = 0;
			இல்லையெனில்
			{
			    eap->line1 = 1;
			    eap->line2 = ARGCOUNT;
			}
			நிறுத்து;
		    நிலை ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			eap->line1 = 1;
			eap->line2 = qf_get_valid_size(eap);
			எனில் (eap->line2 == 0)
			    eap->line2 = 1;
#endif
			நிறுத்து;
		    நிலை ADDR_NONE:
			// Will give an error later if a range is found.
			நிறுத்து;
		}
		++eap->addr_மொத்தம்;
	    }
	    இல்லையெனில் எனில் (*eap->cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == வெற்று)
	    {
		pos_T	    *fp;

		// '*' - visual area
		எனில் (eap->addr_type != ADDR_வரிகள்)
		{
		    *errormsg = _(e_invalid_range);
		    செல் theend;
		}

		++eap->cmd;
		எனில் (!eap->skip)
		{
		    fp = getmark('<', FALSE);
		    எனில் (check_mark(fp) == FAIL)
			செல் theend;
		    eap->line1 = fp->lnum;
		    fp = getmark('>', FALSE);
		    எனில் (check_mark(fp) == FAIL)
			செல் theend;
		    eap->line2 = fp->lnum;
		    ++eap->addr_மொத்தம்;
		}
	    }
	}
	இல்லையெனில்
	    eap->line2 = lnum;
	eap->addr_மொத்தம்++;

	எனில் (*eap->cmd == ';')
	{
	    எனில் (!eap->skip)
	    {
		curwin->w_cursor.lnum = eap->line2;

		// Don't leave the cursor on an illegal line or column, but do
		// accept zero as address, so 0;/PATTERN/ works correctly
		// (where zero usually means to use the first line).
		// Check the cursor position before returning.
		எனில் (eap->line2 > 0)
		    check_cursor();
		இல்லையெனில்
		    check_cursor_col();
		need_check_cursor = TRUE;
	    }
	}
	இல்லையெனில் எனில் (*eap->cmd != ',')
	    நிறுத்து;
	++eap->cmd;
    }

    // One address given: set start and end lines.
    எனில் (eap->addr_மொத்தம் == 1)
    {
	eap->line1 = eap->line2;
	// ... but only implicit: really no address given
	எனில் (lnum == MAXLNUM)
	    eap->addr_மொத்தம் = 0;
    }
    ret = OK;

theend:
    எனில் (need_check_cursor)
	check_cursor();
    திரும்பு ret;
}

/*
 * Append "cmd" to the error message in IObuff.
 * Takes care of limiting the length and handling 0xa0, which would be
 * invisible otherwise.
 */
    நிலையான வெறுமை
append_command(char_u *cmd)
{
    size_t  len = STRLEN(IObuff);
    char_u  *s = cmd;
    char_u  *d;

    எனில் (len > IOSIZE - 100)
    {
	// Not enough space, truncate and put in "...".
	d = IObuff + IOSIZE - 100;
	d -= mb_head_off(IObuff, d);
	STRCPY(d, "...");
    }
    STRCAT(IObuff, ": ");
    d = IObuff + STRLEN(IObuff);
    போது (*s != NUL && d - IObuff + 5 < IOSIZE)
    {
	எனில் (enc_utf8 ? (s[0] == 0xc2 && s[1] == 0xa0) : *s == 0xa0)
	{
	    s += enc_utf8 ? 2 : 1;
	    STRCPY(d, "<a0>");
	    d += 4;
	}
	இல்லையெனில் எனில் (d - IObuff + (*mb_ptr2len)(s) + 1 >= IOSIZE)
	    நிறுத்து;
	இல்லையெனில்
	    MB_COPY_CHAR(s, d);
    }
    *d = NUL;
}

#எனில் defined(FEAT_EVAL)
/*
 * If "start" points "&opt", "&l:opt", "&g:opt" or "$ENV" return a pointer to
 * the name.  Otherwise just return "start".
 */
    char_u *
skip_option_env_lead(char_u *start)
{
    char_u *name = start;

    எனில் (*start == '&')
    {
	எனில் ((start[1] == 'l' || start[1] == 'g') && start[2] == ':')
	    name += 3;
	இல்லையெனில்
	    name += 1;
    }
    இல்லையெனில் எனில் (*start == '$')
	name += 1;
    திரும்பு name;
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
    நிலையான முழு
one_letter_cmd(char_u *p, cmdidx_T *idx)
{
    எனில் (in_vim9script())
	திரும்பு FALSE;
    எனில் (p[0] == 'k'
	    && (p[1] != 'e' || (p[1] == 'e' && p[2] != 'e')))
    {
	*idx = CMD_k;
	திரும்பு TRUE;
    }
    எனில் (p[0] == 's'
	    && ((p[1] == 'c' && (p[2] == NUL || (p[2] != 's' && p[2] != 'r'
			&& (p[3] == NUL || (p[3] != 'i' && p[4] != 'p')))))
		|| p[1] == 'g'
		|| (p[1] == 'i' && p[2] != 'm' && p[2] != 'l' && p[2] != 'g')
		|| p[1] == 'I'
		|| (p[1] == 'r' && p[2] != 'e')))
    {
	*idx = CMD_substitute;
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

#எனில் defined(FEAT_EVAL)
/*
 * Return TRUE if "cmd" starts with "123->", a number followed by a method
 * call.
 */
    முழு
எண்_method(char_u *cmd)
{
    char_u *p = skipdigits(cmd);

    திரும்பு p > cmd && (p = skipwhite(p))[0] == '-' && p[1] == '>';
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
	முழு	*full UNUSED,
	முழு	(*lookup)(char_u *, size_t, முழு cmd, cctx_T *) UNUSED,
	cctx_T	*cctx UNUSED)
{
    முழு		len;
    char_u	*p;
    முழு		i;
#ifndef FEAT_EVAL
    முழு		vim9 = FALSE;
#இல்லையெனில்
    முழு		vim9 = in_vim9script();

    /*
     * Recognize a Vim9 script function/method call and assignment:
     * "lvar = value", "lvar(arg)", "[1, 2 3]->Func()"
     */
    p = eap->cmd;
    எனில் (lookup != வெற்று)
    {
	char_u *pskip = skip_option_env_lead(eap->cmd);

	எனில் (vim_strchr((char_u *)"{('[\"@&$", *p) != வெற்று
	       || ((p = to_name_const_end(pskip)) > eap->cmd && *p != NUL)
	       || (p[0] == '0' && p[1] == 'z'))
	{
	    முழு	    oplen;
	    முழு	    heredoc;
	    char_u  *swp;

	    எனில் (*eap->cmd == '&'
		    || (eap->cmd[0] == '$'
				  && eap->cmd[1] != '\'' && eap->cmd[1] != '"')
		    || (eap->cmd[0] == '@'
					&& (valid_yank_reg(eap->cmd[1], FALSE)
						       || eap->cmd[1] == '@')))
	    {
		எனில் (*eap->cmd == '&')
		{
		    p = eap->cmd + 1;
		    எனில் (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
			p += 2;
		    p = to_name_end(p, FALSE);
		}
		இல்லையெனில் எனில் (*eap->cmd == '$')
		    p = to_name_end(eap->cmd + 1, FALSE);
		இல்லையெனில்
		    p = eap->cmd + 2;
		எனில் (ends_excmd(*skipwhite(p)))
		{
		    // "&option <NL>", "$ENV <NL>" and "@r <NL>" are the start
		    // of an expression.
		    eap->cmdidx = CMD_eval;
		    திரும்பு eap->cmd;
		}
		// "&option" can be followed by "->" or "=", check below
	    }

	    எனில் (vim9 && *p == '<')
	    {
		// generic function type args
		எனில் (skip_generic_func_type_args(&p) == FAIL)
		{
		    eap->cmdidx = CMD_SIZE;
		    திரும்பு p;
		}
	    }

	    swp = skipwhite(p);

	    எனில் (
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
		எனில் (*eap->cmd == '{' && ends_excmd(*skipwhite(eap->cmd + 1)))
		{
		    // "{" by itself is the start of a block.
		    eap->cmdidx = CMD_block;
		    திரும்பு eap->cmd + 1;
		}
		eap->cmdidx = CMD_eval;
		திரும்பு eap->cmd;
	    }

	    எனில் ((p != eap->cmd && (
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
		எனில் (skip_கோவை(&after, வெற்று) == OK)
		{
		    after = skipwhite(after);
		    எனில் (*after == '=' || (*after != NUL && after[1] == '=')
					 || (after[0] == '.' && after[1] == '.'
							   && after[2] == '='))
			eap->cmdidx = CMD_var;
		}
		--emsg_off;
		திரும்பு eap->cmd;
	    }

	    // "[...]->Method()" is a list expression, but "[a, b] = Func()" is
	    // an assignment.
	    // If there is no line break inside the "[...]" then "p" is
	    // advanced to after the "]" by to_name_const_end(): check if a "="
	    // follows.
	    // If "[...]" has a line break "p" still points at the "[" and it
	    // can't be an assignment.
	    எனில் (*eap->cmd == '[')
	    {
		char_u	    *eq;

		p = to_name_const_end(eap->cmd);
		எனில் (p == eap->cmd && *p == '[')
		{
		    முழு மொத்தம் = 0;
		    முழு	semicolon = FALSE;

		    p = skip_var_list(eap->cmd, TRUE, &மொத்தம், &semicolon, TRUE);
		}
		eq = p;
		எனில் (eq != வெற்று)
		{
		    eq = skipwhite(eq);
		    எனில் (vim_strchr((char_u *)"+-*/%.", *eq) != வெற்று)
		    {
			எனில் (eq[0] == '.' && eq[1] == '.')
			    ++eq;
			++eq;
		    }
		}
		எனில் (p == வெற்று || p == eap->cmd || *eq != '=')
		{
		    eap->cmdidx = CMD_eval;
		    திரும்பு eap->cmd;
		}
		எனில் (p > eap->cmd && *eq == '=')
		{
		    eap->cmdidx = CMD_var;
		    திரும்பு eap->cmd;
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
	    எனில் (oplen > 0)
	    {
		எனில் (((p - eap->cmd) > 2 && eap->cmd[1] == ':')
			|| *eap->cmd == '&'
			|| *eap->cmd == '$'
			|| *eap->cmd == '@'
			|| (eap->skip && IS_WHITE_OR_NUL(
						      *(skipwhite(p) + oplen)))
			|| lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK)
		{
		    eap->cmdidx = CMD_var;
		    திரும்பு eap->cmd;
		}
	    }

	    // Recognize trying to use a type for a w:, b:, t: or g: variable:
	    // "w:varname: number = 123".
	    எனில் (eap->cmd[1] == ':' && *p == ':')
	    {
		eap->cmdidx = CMD_var;
		திரும்பு eap->cmd;
	    }
	}

	// 1234->func() is a method call
	எனில் (எண்_method(eap->cmd))
	{
	    eap->cmdidx = CMD_eval;
	    திரும்பு eap->cmd;
	}

	// "g:", "s:" and "l:" are always assumed to be a variable, thus start
	// an expression.  A global/substitute/list command needs to use a
	// longer name.
	எனில் (vim_strchr((char_u *)"gsl", *p) != வெற்று && p[1] == ':')
	{
	    eap->cmdidx = CMD_eval;
	    திரும்பு eap->cmd;
	}

	// If it is an ID it might be a variable with an operator on the next
	// line, if the variable exists it can't be an Ex command.
	எனில் (p > eap->cmd && ends_excmd(*skipwhite(p))
		&& (lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK
		    || (ASCII_ISALPHA(eap->cmd[0]) && eap->cmd[1] == ':')))
	{
	    eap->cmdidx = CMD_eval;
	    திரும்பு eap->cmd;
	}

	// Check for "++nr" and "--nr".
	எனில் (p == eap->cmd && p[0] != NUL && p[0] == p[1]
						   && (*p == '+' || *p == '-'))
	{
	    eap->cmdidx = *p == '+' ? CMD_increment : CMD_decrement;
	    திரும்பு eap->cmd + 2;
	}
    }
#endif

    /*
     * Isolate the command and search for it in the command table.
     */
    p = eap->cmd;
    எனில் (one_letter_cmd(p, &eap->cmdidx))
    {
	++p;
	எனில் (full != வெற்று)
	    *full = TRUE;
    }
    இல்லையெனில்
    {
	போது (ASCII_ISALPHA(*p))
	    ++p;
	// for python 3.x support ":py3", ":python3", ":py3file", etc.
	எனில் (eap->cmd[0] == 'p' && eap->cmd[1] == 'y')
	{
	    போது (ASCII_ISALNUM(*p))
		++p;
	}
	இல்லையெனில் எனில் (*p == '9' && STRNCMP("vim9", eap->cmd, 4) == 0)
	{
	    // include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	    ++p;
	    போது (ASCII_ISALPHA(*p))
		++p;
	}

	// check for non-alpha command
	எனில் (p == eap->cmd && vim_strchr((char_u *)"@*!=><&~#}", *p) != வெற்று)
	    ++p;
	len = (முழு)(p - eap->cmd);
	// The "d" command can directly be followed by 'l' or 'p' flag, when
	// not in Vim9 script.
	எனில் (!vim9 && *eap->cmd == 'd' && (p[-1] == 'l' || p[-1] == 'p'))
	{
	    // Check for ":dl", ":dell", etc. to ":deletel": that's
	    // :delete with the 'l' flag.  Same for 'p'.
	    க்கு (i = 0; i < len; ++i)
		எனில் (eap->cmd[i] != ((char_u *)"delete")[i])
		    நிறுத்து;
	    எனில் (i == len - 1)
	    {
		--len;
		எனில் (p[-1] == 'l')
		    eap->flags |= EXFLAG_LIST;
		இல்லையெனில்
		    eap->flags |= EXFLAG_PRINT;
	    }
	}

	எனில் (ASCII_ISLOWER(eap->cmd[0]))
	{
	    முழு c1 = eap->cmd[0];
	    முழு c2 = len == 1 ? NUL : eap->cmd[1];

	    எனில் (command_மொத்தம் != (முழு)CMD_SIZE)
	    {
		iemsg(e_command_table_needs_to_be_updated_run_make_cmdidxs);
		getout(1);
	    }

	    // Use a precomputed index for fast look-up in cmdnames[]
	    // taking into account the first 2 letters of eap->cmd.
	    eap->cmdidx = cmdidxs1[Char_Ord_கீழ்(c1)];
	    எனில் (ASCII_ISLOWER(c2))
		eap->cmdidx += cmdidxs2[Char_Ord_கீழ்(c1)][Char_Ord_கீழ்(c2)];
	}
	இல்லையெனில் எனில் (ASCII_ISUPPER(eap->cmd[0]))
	    eap->cmdidx = CMD_Next;
	இல்லையெனில்
	    eap->cmdidx = CMD_bang;

	க்கு ( ; (முழு)eap->cmdidx < (முழு)CMD_SIZE;
			       eap->cmdidx = (cmdidx_T)((முழு)eap->cmdidx + 1))
	    எனில் (STRNCMP(cmdnames[(முழு)eap->cmdidx].cmd_name, (எழுத்து *)eap->cmd,
							    (size_t)len) == 0)
	    {
#ifdef FEAT_EVAL
		எனில் (full != வெற்று && cmdnames[eap->cmdidx].cmd_name[len] == NUL)
		    *full = TRUE;
#endif
		நிறுத்து;
	    }

	// :Print and :mode are not supported in Vim9 script.
	// Some commands cannot be shortened in Vim9 script.
	எனில் (vim9 && eap->cmdidx != CMD_SIZE)
	{
	    எனில் (eap->cmdidx == CMD_mode || eap->cmdidx == CMD_Print)
		eap->cmdidx = CMD_SIZE;
	    இல்லையெனில் எனில் ((cmdnames[eap->cmdidx].cmd_argt & EX_WHOLE)
			  && len < (முழு)cmdnames[eap->cmdidx].cmd_namelen)
	    {
		semsg(_(e_command_cannot_be_shortened_str), eap->cmd);
		eap->cmdidx = CMD_SIZE;
	    }
	}

	// Do not recognize ":*" as the star command unless '*' is in
	// 'cpoptions'.
	எனில் (eap->cmdidx == CMD_star && vim_strchr(p_cpo, CPO_STAR) == வெற்று)
	    p = eap->cmd;

	// Look for a user defined command as a last resort.  Let ":Print" be
	// overruled by a user defined command.
	எனில் ((eap->cmdidx == CMD_SIZE || eap->cmdidx == CMD_Print)
		&& *eap->cmd >= 'A' && *eap->cmd <= 'Z')
	{
	    // User defined commands may contain digits.
	    போது (ASCII_ISALNUM(*p))
		++p;
	    p = find_ucmd(eap, p, full, வெற்று, வெற்று);
	}
	எனில் (p == வெற்று || p == eap->cmd)
	    eap->cmdidx = CMD_SIZE;
    }

    // ":fina" means ":finally" in legacy script, for backwards compatibility.
    எனில் (eap->cmdidx == CMD_final && p - eap->cmd == 4 && !vim9)
	eap->cmdidx = CMD_finally;

     // Force ":ho" to be unresolved.  Without this, find_ex_command()
     // matches it to CMD_horizontal (the only "ho*" entry), which makes
     // fullcommand("ho") return "horizontal" even though ":ho" cannot be
     // used as the modifier (cmdmods[] requires 3 chars, "hor").
    எனில் (eap->cmdidx == CMD_horizontal && p - eap->cmd == 2)
	eap->cmdidx = CMD_SIZE;

#ifdef FEAT_EVAL
    எனில் (eap->cmdidx < CMD_SIZE
	    && vim9
	    && !IS_WHITE_NL_OR_NUL(*p) && *p != '!' && *p != '|'
	    && (eap->cmdidx < 0 ||
		(cmdnames[eap->cmdidx].cmd_argt & EX_NONWHITE_OK) == 0))
    {
	// A name that goes on with an underscore is not this command with an
	// argument: "ch_log" is not ":change".  Leave it to be reported as an
	// invalid command.
	எனில் (*p != '_')
	{
	    char_u *cmd = vim_strnsave(eap->cmd, p - eap->cmd);

	    semsg(_(e_command_str_not_followed_by_white_space_str), cmd,
								    eap->cmd);
	    vim_free(cmd);
	}
	eap->cmdidx = CMD_SIZE;
    }
#endif

    திரும்பு p;
}

#எனில் defined(FEAT_EVAL)
வகைவரை கட்டமைப்பு
{
    எழுத்து	*name;
    முழு		minlen;
    முழு		has_மொத்தம்;  // :123verbose  :3tab
} cmdmod_info_T;

நிலையான cmdmod_info_T cmdmod_info_tab[] = {
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
    முழு
modifier_len(char_u *cmd)
{
    முழு		i, j;
    char_u	*p = cmd;

    எனில் (VIM_ISDIGIT(*cmd))
	p = skipwhite(skipdigits(cmd + 1));
    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	// cmdmod_info_tab[] is sorted by name: once the first letter is past
	// the command's first letter no later entry can match.
	எனில் (cmdmod_info_tab[i].name[0] > *p)
	    நிறுத்து;
	க்கு (j = 0; p[j] != NUL; ++j)
	    எனில் (p[j] != cmdmod_info_tab[i].name[j])
		நிறுத்து;
	எனில் (!ASCII_ISALPHA(p[j]) && j >= cmdmod_info_tab[i].minlen
					&& (p == cmd || cmdmod_info_tab[i].has_மொத்தம்))
	    திரும்பு j + (முழு)(p - cmd);
    }
    திரும்பு 0;
}

/*
 * Return > 0 if an Ex command "name" exists.
 * Return 2 if there is an exact match.
 * Return 3 if there is an ambiguous match.
 */
    முழு
cmd_exists(char_u *name)
{
    exarg_T	ea;
    முழு		full = FALSE;
    முழு		i;
    முழு		j;
    char_u	*p;

    // Check command modifiers.
    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	க்கு (j = 0; name[j] != NUL; ++j)
	    எனில் (name[j] != cmdmod_info_tab[i].name[j])
		நிறுத்து;
	எனில் (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	    திரும்பு (cmdmod_info_tab[i].name[j] == NUL ? 2 : 1);
    }

    // Check built-in commands and user defined commands.
    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    p = find_ex_command(&ea, &full, வெற்று, வெற்று);
    எனில் (p == வெற்று)
	திரும்பு 3;
    எனில் (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	திரும்பு 0;
    எனில் (*skipwhite(p) != NUL)
	திரும்பு 0;	// trailing garbage
    திரும்பு (ea.cmdidx == CMD_SIZE ? 0 : (full ? 2 : 1));
}

/*
 * Add the attributes of an Ex command to "d" in the terms of ":command":
 * "argt" are the EX_ flags, "addr_type" the address type and "def" the
 * default for a range or count, -1 when there is none.
 */
    வெறுமை
ex_command_attrs(dict_T *d, long_u argt, cmd_addr_T addr_type, long def)
{
    நிலையான எழுத்து *addr_names[] = {
	"lines", "windows", "arguments", "loaded_buffers", "buffers", "tabs",
	"tabs_relative", "quickfix_valid", "quickfix", "unsigned", "other",
	"none"
    };
    எழுத்து	*nargs;

    எனில் (!(argt & EX_EXTRA))
	nargs = "0";
    இல்லையெனில் எனில் (!(argt & EX_NOSPC))
	nargs = (argt & EX_NEEDARG) ? "+" : "*";
    இல்லையெனில் எனில் (!(argt & EX_NEEDARG))
	nargs = "?";
    இல்லையெனில்
	nargs = (argt & EX_ARGSPACE) ? "_" : "1";
    dict_add_string(d, "nargs", (char_u *)nargs);

    எனில் (!(argt & EX_RANGE))
	dict_add_bool(d, "range", FALSE);
    இல்லையெனில் எனில் (argt & EX_DFLALL)
	dict_add_string(d, "range", (char_u *)"%");
    இல்லையெனில் எனில் (def >= 0 && !(argt & EX_மொத்தம்))
	dict_add_எண்(d, "range", def);
    இல்லையெனில்
	dict_add_string(d, "range", (char_u *)".");
    எனில் (!(argt & EX_மொத்தம்))
	dict_add_bool(d, "count", FALSE);
    இல்லையெனில்
	dict_add_எண்(d, "count", def < 0 ? 0 : def);
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
    முழு
ex_command_info(char_u *name, முழு vim9, dict_T *d)
{
    exarg_T	ea;
    char_u	*p;
    முழு		i;
    முழு		j;
    முழு		save_cmod_flags = cmdmod.cmod_flags;
    முழு		ret = OK;

    எனில் (vim9 != -1)
    {
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9 ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    // An abbreviation of a command modifier, like ":bo" for ":botright".
    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	க்கு (j = 0; name[j] != NUL; ++j)
	    எனில் (name[j] != cmdmod_info_tab[i].name[j])
		நிறுத்து;
	எனில் (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	{
	    name = (char_u *)cmdmod_info_tab[i].name;
	    நிறுத்து;
	}
    }

    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    ea.addr_மொத்தம் = 0;
    // Don't complain about using "en" or ":let" in Vim9 script.
    ++emsg_silent;
    p = find_ex_command(&ea, வெற்று, வெற்று, வெற்று);
    எனில் (p == வெற்று || ea.cmdidx == CMD_SIZE
	    || (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	    || *skipwhite(p) != NUL
	    || not_in_vim9(&ea, true) == FAIL)
	ret = FAIL;
    --emsg_silent;

    எனில் (ret == FAIL)
	;
    இல்லையெனில் எனில் (IS_USER_CMDIDX(ea.cmdidx))
	ret = user_command_info(ea.useridx, ea.cmdidx, d);
    இல்லையெனில்
    {
	முழு	notimpl = cmdnames[ea.cmdidx].cmd_func == ex_ni
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
    திரும்பு ret;
}

/*
 * "fullcommand" function
 */
    வெறுமை
f_fullcommand(typval_T *argvars, typval_T *rettv)
{
    exarg_T	ea;
    char_u	*name;
    char_u	*p;
    முழு		vim9script = in_vim9script();
    முழு		save_cmod_flags = cmdmod.cmod_flags;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	திரும்பு;

    name = tv_get_string(&argvars[0]);
    எனில் (name == வெற்று)
	திரும்பு;

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	vim9script = tv_get_bool(&argvars[1]);
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9script ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    போது (*name == ':')
	name++;
    name = skip_range(name, TRUE, வெற்று);

    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.addr_மொத்தம் = 0;
    ++emsg_silent;  // don't complain about using "en" in Vim9 script
    p = find_ex_command(&ea, வெற்று, வெற்று, வெற்று);
    --emsg_silent;
    எனில் (p == வெற்று || ea.cmdidx == CMD_SIZE)
	செல் theend;

    எனில் (vim9script && not_in_vim9(&ea, true) == FAIL)
	செல் theend;

    rettv->vval.v_string = vim_strsave(IS_USER_CMDIDX(ea.cmdidx)
				 ? get_user_command_name(ea.useridx, ea.cmdidx)
				 : cmdnames[ea.cmdidx].cmd_name);
theend:
    cmdmod.cmod_flags = save_cmod_flags;
}
#endif

    cmdidx_T
excmd_get_cmdidx(char_u *cmd, முழு len)
{
    cmdidx_T idx;

    எனில் (!one_letter_cmd(cmd, &idx))
	க்கு (idx = (cmdidx_T)0; (முழு)idx < (முழு)CMD_SIZE;
		idx = (cmdidx_T)((முழு)idx + 1))
	    எனில் (STRNCMP(cmdnames[(முழு)idx].cmd_name, cmd, (size_t)len) == 0)
		நிறுத்து;

    திரும்பு idx;
}

    long
excmd_get_argt(cmdidx_T idx)
{
    திரும்பு (long)cmdnames[(முழு)idx].cmd_argt;
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
    முழு		skip_star,	// skip "*" used for Visual range
    முழு		*ctx)		// pointer to xp_context or NULL
{
    char_u	*cmd = cmd_start;
    unsigned	delim;

    போது (vim_strchr((char_u *)" \t0123456789.$%'/?-+,;\\", *cmd) != வெற்று)
    {
	எனில் (*cmd == '\\')
	{
	    எனில் (cmd[1] == '?' || cmd[1] == '/' || cmd[1] == '&')
		++cmd;
	    இல்லையெனில்
		நிறுத்து;
	}
	இல்லையெனில் எனில் (*cmd == '\'')
	{
	    char_u *p = cmd;

	    // a quote is only valid at the start or after a separator
	    போது (p > cmd_start)
	    {
		--p;
		எனில் (!VIM_ISWHITE(*p))
		    நிறுத்து;
	    }
	    எனில் (cmd > cmd_start && !VIM_ISWHITE(*p) && *p != ',' && *p != ';')
		நிறுத்து;
	    எனில் (*++cmd == NUL && ctx != வெற்று)
		*ctx = EXPAND_NOTHING;
	}
	இல்லையெனில் எனில் (*cmd == '/' || *cmd == '?')
	{
	    delim = *cmd++;
	    போது (*cmd != NUL && *cmd != delim)
		எனில் (*cmd++ == '\\' && *cmd != NUL)
		    ++cmd;
	    எனில் (*cmd == NUL && ctx != வெற்று)
		*ctx = EXPAND_NOTHING;
	}
	எனில் (*cmd != NUL)
	    ++cmd;
    }

    // Skip ":" and white space.
    போது (*cmd == ':')
	cmd = skipwhite(cmd + 1);

    // Skip "*" used for Visual range.
    எனில் (skip_star && *cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == வெற்று)
	cmd = skipwhite(cmd + 1);

    திரும்பு cmd;
}

    நிலையான வெறுமை
addr_error(cmd_addr_T addr_type)
{
    எனில் (addr_type == ADDR_NONE)
	emsg(_(e_no_range_allowed));
    இல்லையெனில்
	emsg(_(e_invalid_range));
}

/*
 * Return the default address for an address type.
 */
    நிலையான linenr_T
default_address(exarg_T *eap)
{
    linenr_T lnum = 0;

    தேர்வு (eap->addr_type)
    {
	நிலை ADDR_வரிகள்:
	நிலை ADDR_OTHER:
	    // Default is the cursor line number.  Avoid using an invalid
	    // line number though.
	    எனில் (curwin->w_cursor.lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
		lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
	    இல்லையெனில்
		lnum = curwin->w_cursor.lnum;
	    நிறுத்து;
	நிலை ADDR_WINDOWS:
	    lnum = CURRENT_WIN_NR;
	    நிறுத்து;
	நிலை ADDR_ARGUMENTS:
	    lnum = curwin->w_arg_idx + 1;
	    எனில் (lnum > ARGCOUNT)
		lnum = ARGCOUNT;
	    நிறுத்து;
	நிலை ADDR_LOADED_BUFFERS:
	நிலை ADDR_BUFFERS:
	    lnum = curbuf->b_fnum;
	    நிறுத்து;
	நிலை ADDR_TABS:
	    lnum = CURRENT_TAB_NR;
	    நிறுத்து;
	நிலை ADDR_TABS_RELATIVE:
	நிலை ADDR_UNSIGNED:
	    lnum = 1;
	    நிறுத்து;
	நிலை ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_idx(eap);
#endif
	    நிறுத்து;
	நிலை ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_valid_idx(eap);
#endif
	    நிறுத்து;
	நிலை ADDR_NONE:
	    // Will give an error later if a range is found.
	    நிறுத்து;
    }
    திரும்பு lnum;
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
    முழு		skip,		// only skip the address, don't use it
    முழு		silent,		// no errors or side effects
    முழு		to_other_file,  // flag: may jump to other file
    முழு		address_மொத்தம் UNUSED) // 1 for first address, >1 after comma
{
    முழு		c;
    முழு		i;
    long	n;
    char_u	*cmd;
    pos_T	pos;
    pos_T	*fp;
    linenr_T	lnum;
    buf_T	*buf;

    cmd = skipwhite(*ptr);
    lnum = MAXLNUM;
    செய்
    {
	தேர்வு (*cmd)
	{
	    நிலை '.':			    // '.' - Cursor position
		++cmd;
		தேர்வு (addr_type)
		{
		    நிலை ADDR_வரிகள்:
		    நிலை ADDR_OTHER:
			lnum = curwin->w_cursor.lnum;
			நிறுத்து;
		    நிலை ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			நிறுத்து;
		    நிலை ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			நிறுத்து;
		    நிலை ADDR_LOADED_BUFFERS:
		    நிலை ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			நிறுத்து;
		    நிலை ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			நிறுத்து;
		    நிலை ADDR_NONE:
		    நிலை ADDR_TABS_RELATIVE:
		    நிலை ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = வெற்று;
			செல் error;
			நிறுத்து;
		    நிலை ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			நிறுத்து;
		    நிலை ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			நிறுத்து;
		}
		நிறுத்து;

	    நிலை '$':			    // '$' - last line
		++cmd;
		தேர்வு (addr_type)
		{
		    நிலை ADDR_வரிகள்:
		    நிலை ADDR_OTHER:
			lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
			நிறுத்து;
		    நிலை ADDR_WINDOWS:
			lnum = LAST_WIN_NR;
			நிறுத்து;
		    நிலை ADDR_ARGUMENTS:
			lnum = ARGCOUNT;
			நிறுத்து;
		    நிலை ADDR_LOADED_BUFFERS:
			buf = lastbuf;
			போது (buf->b_ml.ml_mfp == வெற்று)
			{
			    எனில் (buf->b_prev == வெற்று)
				நிறுத்து;
			    buf = buf->b_prev;
			}
			lnum = buf->b_fnum;
			நிறுத்து;
		    நிலை ADDR_BUFFERS:
			lnum = lastbuf->b_fnum;
			நிறுத்து;
		    நிலை ADDR_TABS:
			lnum = LAST_TAB_NR;
			நிறுத்து;
		    நிலை ADDR_NONE:
		    நிலை ADDR_TABS_RELATIVE:
		    நிலை ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = வெற்று;
			செல் error;
			நிறுத்து;
		    நிலை ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_size(eap);
			எனில் (lnum == 0)
			    lnum = 1;
#endif
			நிறுத்து;
		    நிலை ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_valid_size(eap);
			எனில் (lnum == 0)
			    lnum = 1;
#endif
			நிறுத்து;
		}
		நிறுத்து;

	    நிலை '\'':			    // ''' - mark
		எனில் (*++cmd == NUL)
		{
		    cmd = வெற்று;
		    செல் error;
		}
		எனில் (addr_type != ADDR_வரிகள்)
		{
		    addr_error(addr_type);
		    cmd = வெற்று;
		    செல் error;
		}
		எனில் (skip)
		    ++cmd;
		இல்லையெனில்
		{
		    // Only accept a mark in another file when it is
		    // used by itself: ":'M".
		    fp = getmark(*cmd, to_other_file && cmd[1] == NUL);
		    ++cmd;
		    எனில் (fp == (pos_T *)-1)
			// Jumped to another file.
			lnum = curwin->w_cursor.lnum;
		    இல்லையெனில்
		    {
			எனில் (check_mark(fp) == FAIL)
			{
			    cmd = வெற்று;
			    செல் error;
			}
			lnum = fp->lnum;
		    }
		}
		நிறுத்து;

	    நிலை '/':
	    நிலை '?':			// '/' or '?' - search
		c = *cmd++;
		எனில் (addr_type != ADDR_வரிகள்)
		{
		    addr_error(addr_type);
		    cmd = வெற்று;
		    செல் error;
		}
		எனில் (skip)	// skip "/pat/"
		{
		    cmd = skip_regexp(cmd, c, magic_isset());
		    எனில் (*cmd == c)
			++cmd;
		}
		இல்லையெனில்
		{
		    முழு flags;

		    pos = curwin->w_cursor; // save curwin->w_cursor

		    // When '/' or '?' follows another address, start from
		    // there.
		    எனில் (lnum > 0 && lnum != MAXLNUM)
			curwin->w_cursor.lnum =
				lnum > curbuf->b_ml.ml_வரி_மொத்தம்
					   ? curbuf->b_ml.ml_வரி_மொத்தம் : lnum;

		    // Start a forward search at the end of the line (unless
		    // before the first line).
		    // Start a backward search at the start of the line.
		    // This makes sure we never match in the current
		    // line, and can match anywhere in the
		    // next/previous line.
		    எனில் (c == '/' && curwin->w_cursor.lnum > 0)
			curwin->w_cursor.col = MAXCOL;
		    இல்லையெனில்
			curwin->w_cursor.col = 0;
		    searchcmdlen = 0;
		    flags = silent ? தேடல்_KEEP : தேடல்_HIS | தேடல்_செய்தி;
		    எனில் (!do_தேடல்(வெற்று, c, c, cmd, STRLEN(cmd), 1L, flags, வெற்று))
		    {
			curwin->w_cursor = pos;
			cmd = வெற்று;
			செல் error;
		    }
		    lnum = curwin->w_cursor.lnum;
		    curwin->w_cursor = pos;
		    // adjust command string pointer
		    cmd += searchcmdlen;
		}
		நிறுத்து;

	    நிலை '\\':		    // "\?", "\/" or "\&", repeat search
		++cmd;
		எனில் (addr_type != ADDR_வரிகள்)
		{
		    addr_error(addr_type);
		    cmd = வெற்று;
		    செல் error;
		}
		எனில் (*cmd == '&')
		    i = RE_SUBST;
		இல்லையெனில் எனில் (*cmd == '?' || *cmd == '/')
		    i = RE_தேடல்;
		இல்லையெனில்
		{
		    emsg(_(e_backslash_should_be_followed_by));
		    cmd = வெற்று;
		    செல் error;
		}

		எனில் (!skip)
		{
		    /*
		     * When search follows another address, start from
		     * there.
		     */
		    எனில் (lnum != MAXLNUM)
			pos.lnum = lnum;
		    இல்லையெனில்
			pos.lnum = curwin->w_cursor.lnum;

		    /*
		     * Start the search just like for the above
		     * do_search().
		     */
		    எனில் (*cmd != '?')
			pos.col = MAXCOL;
		    இல்லையெனில்
			pos.col = 0;
		    pos.coladd = 0;
		    எனில் (searchit(curwin, curbuf, &pos, வெற்று,
				*cmd == '?' ? BACKWARD : FORWARD,
				(char_u *)"", 0, 1L, தேடல்_செய்தி, i, வெற்று) != FAIL)
			lnum = pos.lnum;
		    இல்லையெனில்
		    {
			cmd = வெற்று;
			செல் error;
		    }
		}
		++cmd;
		நிறுத்து;

	    இயல்பு:
		எனில் (VIM_ISDIGIT(*cmd))	// absolute line number
		    lnum = getdigits(&cmd);
	}

	க்கு (;;)
	{
	    cmd = skipwhite(cmd);
	    எனில் (*cmd != '-' && *cmd != '+' && !VIM_ISDIGIT(*cmd))
		நிறுத்து;

	    எனில் (lnum == MAXLNUM)
	    {
		தேர்வு (addr_type)
		{
		    நிலை ADDR_வரிகள்:
		    நிலை ADDR_OTHER:
			// "+1" is same as ".+1"
			lnum = curwin->w_cursor.lnum;
			நிறுத்து;
		    நிலை ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			நிறுத்து;
		    நிலை ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			நிறுத்து;
		    நிலை ADDR_LOADED_BUFFERS:
		    நிலை ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			நிறுத்து;
		    நிலை ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			நிறுத்து;
		    நிலை ADDR_TABS_RELATIVE:
			lnum = 1;
			நிறுத்து;
		    நிலை ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			நிறுத்து;
		    நிலை ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			நிறுத்து;
		    நிலை ADDR_NONE:
		    நிலை ADDR_UNSIGNED:
			lnum = 0;
			நிறுத்து;
		}
	    }

	    எனில் (VIM_ISDIGIT(*cmd))
		i = '+';		// "number" is same as "+number"
	    இல்லையெனில்
		i = *cmd++;
	    எனில் (!VIM_ISDIGIT(*cmd))	// '+' is '+1'
		n = 1;
	    இல்லையெனில்
	    {
		// "number", "+number" or "-number"
		n = getdigits(&cmd);
		எனில் (n == MAXLNUM)
		{
		    emsg(_(e_வரி_எண்_out_of_range));
		    cmd = வெற்று;
		    செல் error;
		}
	    }

	    எனில் (addr_type == ADDR_TABS_RELATIVE)
	    {
		emsg(_(e_invalid_range));
		cmd = வெற்று;
		செல் error;
	    }
	    இல்லையெனில் எனில் (addr_type == ADDR_LOADED_BUFFERS
		    || addr_type == ADDR_BUFFERS)
		lnum = compute_buffer_local_மொத்தம்(
				    addr_type, lnum, (i == '-') ? -1 * n : n);
	    இல்லையெனில்
	    {
#ifdef FEAT_FOLDING
		// Relative line addressing: need to adjust for lines in a
		// closed fold after the first address.
		எனில் (addr_type == ADDR_வரிகள் && (i == '-' || i == '+')
							 && address_மொத்தம் >= 2)
		    (வெறுமை)hasFolding(lnum, வெற்று, &lnum);
#endif
		எனில் (i == '-')
		    lnum -= n;
		இல்லையெனில்
		{
		    எனில் (lnum >= 0 && n >= LONG_MAX - lnum)
		    {
			emsg(_(e_வரி_எண்_out_of_range));
			cmd = வெற்று;
			செல் error;
		    }
		    lnum += n;
		}
	    }
	}
    } போது (*cmd == '/' || *cmd == '?');

error:
    *ptr = cmd;
    திரும்பு lnum;
}

/*
 * Set eap->line1 and eap->line2 to the whole range.
 * Used for commands with the EX_DFLALL flag and no range given.
 */
    நிலையான வெறுமை
address_default_all(exarg_T *eap)
{
    eap->line1 = 1;
    தேர்வு (eap->addr_type)
    {
	நிலை ADDR_வரிகள்:
	நிலை ADDR_OTHER:
	    eap->line2 = curbuf->b_ml.ml_வரி_மொத்தம்;
	    நிறுத்து;
	நிலை ADDR_LOADED_BUFFERS:
	    {
		buf_T *buf = firstbuf;

		போது (buf->b_next != வெற்று && buf->b_ml.ml_mfp == வெற்று)
		    buf = buf->b_next;
		eap->line1 = buf->b_fnum;
		buf = lastbuf;
		போது (buf->b_prev != வெற்று && buf->b_ml.ml_mfp == வெற்று)
		    buf = buf->b_prev;
		eap->line2 = buf->b_fnum;
	    }
	    நிறுத்து;
	நிலை ADDR_BUFFERS:
	    eap->line1 = firstbuf->b_fnum;
	    eap->line2 = lastbuf->b_fnum;
	    நிறுத்து;
	நிலை ADDR_WINDOWS:
	    eap->line2 = LAST_WIN_NR;
	    நிறுத்து;
	நிலை ADDR_TABS:
	    eap->line2 = LAST_TAB_NR;
	    நிறுத்து;
	நிலை ADDR_TABS_RELATIVE:
	    eap->line2 = 1;
	    நிறுத்து;
	நிலை ADDR_ARGUMENTS:
	    எனில் (ARGCOUNT == 0)
		eap->line1 = eap->line2 = 0;
	    இல்லையெனில்
		eap->line2 = ARGCOUNT;
	    நிறுத்து;
	நிலை ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    eap->line2 = qf_get_valid_size(eap);
	    எனில் (eap->line2 == 0)
		eap->line2 = 1;
#endif
	    நிறுத்து;
	நிலை ADDR_NONE:
	நிலை ADDR_UNSIGNED:
	நிலை ADDR_QUICKFIX:
	    iemsg("Cannot use EX_DFLALL with ADDR_NONE, ADDR_UNSIGNED or ADDR_QUICKFIX");
	    நிறுத்து;
    }
}


/*
 * Get flags from an Ex command argument.
 */
    நிலையான வெறுமை
get_flags(exarg_T *eap)
{
    போது (vim_strchr((char_u *)"lp#", *eap->arg) != வெற்று)
    {
	எனில் (*eap->arg == 'l')
	    eap->flags |= EXFLAG_LIST;
	இல்லையெனில் எனில் (*eap->arg == 'p')
	    eap->flags |= EXFLAG_PRINT;
	இல்லையெனில்
	    eap->flags |= EXFLAG_NR;
	eap->arg = skipwhite(eap->arg + 1);
    }
}

/*
 * Function called for command which is Not Implemented.  NI!
 */
    வெறுமை
ex_ni(exarg_T *eap)
{
    எனில் (!eap->skip)
	eap->errmsg =
		_(e_sorry_command_is_not_available_in_this_version);
}

#ifdef HAVE_EX_SCRIPT_NI
/*
 * Function called for script command which is Not Implemented.  NI!
 * Skips over ":perl <<EOF" constructs.
 */
    நிலையான வெறுமை
ex_script_ni(exarg_T *eap)
{
    எனில் (!eap->skip)
	ex_ni(eap);
    இல்லையெனில்
	vim_free(script_get(eap, eap->arg));
}
#endif

/*
 * Check range in Ex command for validity.
 * Return NULL when valid, error message when invalid.
 */
    நிலையான எழுத்து *
invalid_range(exarg_T *eap)
{
    buf_T	*buf;

    எனில் (       eap->line1 < 0
	    || eap->line2 < 0
	    || eap->line1 > eap->line2)
	திரும்பு _(e_invalid_range);

    எனில் (eap->argt & EX_RANGE)
    {
	தேர்வு (eap->addr_type)
	{
	    நிலை ADDR_வரிகள்:
		எனில் (eap->line2 > curbuf->b_ml.ml_வரி_மொத்தம்
#ifdef FEAT_DIFF
			    + (eap->cmdidx == CMD_diffget ||
				eap->cmdidx == CMD_diffput)
#endif
		   )
		    திரும்பு _(e_invalid_range);
		நிறுத்து;
	    நிலை ADDR_ARGUMENTS:
		// add 1 if ARGCOUNT is 0
		எனில் (eap->line2 > ARGCOUNT + (!ARGCOUNT))
		    திரும்பு _(e_invalid_range);
		நிறுத்து;
	    நிலை ADDR_BUFFERS:
		// Only a boundary check, not whether the buffers actually
		// exist.
		எனில் (eap->line1 < 1 || eap->line2 > get_highest_fnum())
		    திரும்பு _(e_invalid_range);
		நிறுத்து;
	    நிலை ADDR_LOADED_BUFFERS:
		buf = firstbuf;
		போது (buf->b_ml.ml_mfp == வெற்று)
		{
		    எனில் (buf->b_next == வெற்று)
			திரும்பு _(e_invalid_range);
		    buf = buf->b_next;
		}
		எனில் (eap->line1 < buf->b_fnum)
		    திரும்பு _(e_invalid_range);
		buf = lastbuf;
		போது (buf->b_ml.ml_mfp == வெற்று)
		{
		    எனில் (buf->b_prev == வெற்று)
			திரும்பு _(e_invalid_range);
		    buf = buf->b_prev;
		}
		எனில் (eap->line2 > buf->b_fnum)
		    திரும்பு _(e_invalid_range);
		நிறுத்து;
	    நிலை ADDR_WINDOWS:
		எனில் (eap->line2 > LAST_WIN_NR)
		    திரும்பு _(e_invalid_range);
		நிறுத்து;
	    நிலை ADDR_TABS:
		எனில் (eap->line2 > LAST_TAB_NR)
		    திரும்பு _(e_invalid_range);
		நிறுத்து;
	    நிலை ADDR_TABS_RELATIVE:
	    நிலை ADDR_OTHER:
		// Any range is OK.
		நிறுத்து;
	    நிலை ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
		// No error for value that is too big, will use the last entry.
		எனில் (eap->line2 <= 0)
		{
		    எனில் (eap->addr_மொத்தம் == 0)
			திரும்பு _(e_no_errors);
		    திரும்பு _(e_invalid_range);
		}
#endif
		நிறுத்து;
	    நிலை ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
		எனில் ((eap->line2 != 1 && eap->line2 > qf_get_valid_size(eap))
			|| eap->line2 < 0)
		    திரும்பு _(e_invalid_range);
#endif
		நிறுத்து;
	    நிலை ADDR_UNSIGNED:
	    நிலை ADDR_NONE:
		// Will give an error elsewhere.
		நிறுத்து;
	}
    }
    திரும்பு வெற்று;
}

/*
 * Correct the range for zero line number, if required.
 */
    நிலையான வெறுமை
correct_range(exarg_T *eap)
{
    எனில் (!(eap->argt & EX_ZEROR))	    // zero in range not allowed
    {
	எனில் (eap->line1 == 0)
	    eap->line1 = 1;
	எனில் (eap->line2 == 0)
	    eap->line2 = 1;
    }
}

#ifdef FEAT_QUICKFIX
/*
 * For a ":vimgrep" or ":vimgrepadd" command return a pointer past the
 * pattern.  Otherwise return eap->arg.
 */
    நிலையான char_u *
skip_grep_pat(exarg_T *eap)
{
    char_u	*p = eap->arg;

    எனில் (*p != NUL && (eap->cmdidx == CMD_vimgrep || eap->cmdidx == CMD_lvimgrep
		|| eap->cmdidx == CMD_vimgrepadd
		|| eap->cmdidx == CMD_lvimgrepadd
		|| grep_internal(eap->cmdidx)))
    {
	p = skip_vimgrep_pat(p, வெற்று, வெற்று);
	எனில் (p == வெற்று)
	    p = eap->arg;
    }
    திரும்பு p;
}

/*
 * For the ":make" and ":grep" commands insert the 'makeprg'/'grepprg' option
 * in the command line, so that things like % get expanded.
 */
    நிலையான char_u *
replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep)
{
    char_u	*new_cmdline;
    char_u	*program;
    char_u	*pos;
    char_u	*ptr;
    முழு		len;
    முழு		i;

    /*
     * Don't do it when ":vimgrep" is used for ":grep".
     */
    எனில் ((eap->cmdidx == CMD_make || eap->cmdidx == CMD_lmake
		     || eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
		     || eap->cmdidx == CMD_grepadd
		     || eap->cmdidx == CMD_lgrepadd)
	    && !grep_internal(eap->cmdidx))
    {
	எனில் (eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
	    || eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	{
	    எனில் (*curbuf->b_p_gp == NUL)
		program = p_gp;
	    இல்லையெனில்
		program = curbuf->b_p_gp;
	}
	இல்லையெனில்
	{
	    எனில் (*curbuf->b_p_mp == NUL)
		program = p_mp;
	    இல்லையெனில்
		program = curbuf->b_p_mp;
	}

	p = skipwhite(p);

	எனில் ((pos = (char_u *)strstr((எழுத்து *)program, "$*")) != வெற்று)
	{
	    // replace $* by given arguments
	    i = 1;
	    போது ((pos = (char_u *)strstr((எழுத்து *)pos + 2, "$*")) != வெற்று)
		++i;
	    len = (முழு)STRLEN(p);
	    new_cmdline = alloc(STRLEN(program) + (size_t)i * (len - 2) + 1);
	    எனில் (new_cmdline == வெற்று)
		திரும்பு வெற்று;			// out of memory
	    ptr = new_cmdline;
	    போது ((pos = (char_u *)strstr((எழுத்து *)program, "$*")) != வெற்று)
	    {
		i = (முழு)(pos - program);
		STRNCPY(ptr, program, i);
		STRCPY(ptr += i, p);
		ptr += len;
		program = pos + 2;
	    }
	    STRCPY(ptr, program);
	}
	இல்லையெனில்
	{
	    size_t program_len = STRLEN(program);

	    new_cmdline = alloc(program_len + STRLEN(p) + 2);
	    எனில் (new_cmdline == வெற்று)
		திரும்பு வெற்று;			// out of memory
	    STRCPY(new_cmdline, program);
	    STRCPY(new_cmdline + program_len, " ");
	    STRCPY(new_cmdline + program_len + 1, p);
	}
	செய்தி_make(p);

	// 'eap->cmd' is not set here, because it is not used at CMD_make
	vim_free(*cmdlinep);
	*cmdlinep = new_cmdline;
	p = new_cmdline;
    }
    திரும்பு p;
}
#endif

/*
 * Expand file name in Ex command argument.
 * When an error is detected, "errormsgp" is set to a non-NULL pointer.
 * Return FAIL for failure, OK otherwise.
 */
    முழு
expand_filename(
    exarg_T	*eap,
    char_u	**cmdlinep,
    எழுத்து	**errormsgp)
{
    முழு		has_wildcards;	// need to expand wildcards
    char_u	*repl;
    size_t	srclen;
    char_u	*p;
    முழு		n;
    முழு		escaped;

#ifdef FEAT_QUICKFIX
    // Skip a regexp pattern for ":vimgrep[add] pat file..."
    p = skip_grep_pat(eap);
#இல்லையெனில்
    p = eap->arg;
#endif

    /*
     * Decide to expand wildcards *before* replacing '%', '#', etc.  If
     * the file name contains a wildcard it should not cause expanding.
     * (it will be expanded anyway if there is a wildcard before replacing).
     */
    has_wildcards = mch_has_wildcard(p);
    போது (*p != NUL)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`, wildcards in it are not expanded.
	எனில் (p[0] == '`' && p[1] == '=')
	{
	    p += 2;
	    (வெறுமை)skip_கோவை(&p, வெற்று);
	    எனில் (*p == '`')
		++p;
	    தொடர்;
	}
#endif
	/*
	 * Quick check if this cannot be the start of a special string.
	 * Also removes backslash before '%', '#' and '<'.
	 */
	எனில் (vim_strchr((char_u *)"%#<", *p) == வெற்று)
	{
	    ++p;
	    தொடர்;
	}

	/*
	 * Try to find a match at this position.
	 */
	repl = eval_vars(p, eap->arg, &srclen, &(eap->do_ecmd_lnum),
						    errormsgp, &escaped, TRUE);
	எனில் (*errormsgp != வெற்று)		// error detected
	    திரும்பு FAIL;
	எனில் (repl == வெற்று)		// no match found
	{
	    p += srclen;
	    தொடர்;
	}

	// Wildcards won't be expanded below, the replacement is taken
	// literally.  But do expand "~/file", "~user/file" and "$HOME/file".
	எனில் (vim_strchr(repl, '$') != வெற்று || vim_strchr(repl, '~') != வெற்று)
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
	எனில் (!eap->usefilter
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
	    நிலையான char_u *nobslash = (char_u *)" \t\"|";
# define ESCAPE_CHARS nobslash
#இல்லையெனில்
# define ESCAPE_CHARS escape_chars
#endif

	    க்கு (l = repl; *l; ++l)
		எனில் (vim_strchr(ESCAPE_CHARS, *l) != வெற்று)
		{
		    l = vim_strsave_escaped(repl, ESCAPE_CHARS);
		    எனில் (l != வெற்று)
		    {
			vim_free(repl);
			repl = l;
		    }
		    நிறுத்து;
		}
	}

	// For a shell command a '!' must be escaped.
	எனில் ((eap->usefilter || eap->cmdidx == CMD_bang
						|| eap->cmdidx == CMD_terminal)
			    && vim_strpbrk(repl, (char_u *)"!") != வெற்று)
	{
	    char_u	*l;

	    l = vim_strsave_escaped(repl, (char_u *)"!");
	    எனில் (l != வெற்று)
	    {
		vim_free(repl);
		repl = l;
	    }
	}

	p = repl_cmdline(eap, p, srclen, repl, cmdlinep);
	vim_free(repl);
	எனில் (p == வெற்று)
	    திரும்பு FAIL;
    }

    /*
     * One file argument: Expand wildcards.
     * Don't do this with ":r !command" or ":w !command".
     */
    எனில் ((eap->argt & EX_NOSPC) && !eap->usefilter)
    {
	/*
	 * May do this twice:
	 * 1. Replace environment variables.
	 * 2. Replace any other wildcards, remove backslashes.
	 */
	க்கு (n = 1; n <= 2; ++n)
	{
	    எனில் (n == 2)
	    {
		/*
		 * Halve the number of backslashes (this is Vi compatible).
		 * For Unix and OS/2, when wildcards are expanded, this is
		 * done by ExpandOne() below.
		 */
#எனில் defined(UNIX)
		எனில் (!has_wildcards)
#endif
		    backslash_halve(eap->arg);
	    }

	    எனில் (has_wildcards)
	    {
		எனில் (n == 1)
		{
		    /*
		     * First loop: May expand environment variables.  This
		     * can be done much faster with expand_env() than with
		     * something else (e.g., calling a shell).
		     * After expanding environment variables, check again
		     * if there are still wildcards present.
		     */
		    எனில் (vim_strchr(eap->arg, '$') != வெற்று
			    || vim_strchr(eap->arg, '~') != வெற்று)
		    {
			expand_env_esc(eap->arg, NameBuff, MAXPATHL,
					(char_u *)(" \t" PATH_ESC_WILDCARDS),
					TRUE, வெற்று);
			has_wildcards = mch_has_wildcard(NameBuff);
			p = NameBuff;
		    }
		    இல்லையெனில்
			p = வெற்று;
		}
		இல்லையெனில் // n == 2
		{
		    expand_T	xpc;
		    முழு		options = WILD_LIST_NOTFOUND
					       | WILD_NOERROR | WILD_ADD_SLASH;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    எனில் (p_wic)
			options += WILD_ICASE;
		    p = ExpandOne(&xpc, eap->arg, வெற்று,
						   options, WILD_EXPAND_FREE);
		    எனில் (p == வெற்று)
			திரும்பு FAIL;
		}
		எனில் (p != வெற்று)
		{
		    (வெறுமை)repl_cmdline(eap, eap->arg, STRLEN(eap->arg),
								 p, cmdlinep);
		    எனில் (n == 2)	// p came from ExpandOne()
			vim_free(p);
		}
	    }
	}
    }
    திரும்பு OK;
}

/*
 * Replace part of the command line, keeping eap->cmd, eap->arg and
 * eap->nextcmd correct.
 * "src" points to the part that is to be replaced, of length "srclen".
 * "repl" is the replacement string.
 * Returns a pointer to the character after the replaced string.
 * Returns NULL for failure.
 */
    நிலையான char_u *
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
    எனில் (eap->nextcmd != வெற்று)
	i += STRLEN(eap->nextcmd);	// add space for next command
    எனில் ((new_cmdline = alloc(i)) == வெற்று)
	திரும்பு வெற்று;			// out of memory!

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

    எனில் (eap->nextcmd != வெற்று)		// append next command
    {
	new_cmdlinelen += taillen + 1;
	STRCPY(new_cmdline + new_cmdlinelen, eap->nextcmd);
	eap->nextcmd = new_cmdline + new_cmdlinelen;
    }
    eap->cmd = new_cmdline + (eap->cmd - *cmdlinep);
    eap->arg = new_cmdline + (eap->arg - *cmdlinep);
    எனில் (eap->do_ecmd_cmd != வெற்று && eap->do_ecmd_cmd != dollar_command)
	eap->do_ecmd_cmd = new_cmdline + (eap->do_ecmd_cmd - *cmdlinep);
    vim_free(*cmdlinep);
    *cmdlinep = new_cmdline;

    திரும்பு src;
}

/*
 * Check for '|' to separate commands and '"' to start comments.
 * If "keep_backslash" is TRUE do not remove any backslash.
 */
    வெறுமை
separate_nextcmd(exarg_T *eap, முழு keep_backslash)
{
    முழு		vim9script = in_vim9script();
    char_u	*p;

#ifdef FEAT_QUICKFIX
    p = skip_grep_pat(eap);
#இல்லையெனில்
    p = eap->arg;
#endif

    க்கு ( ; *p; MB_PTR_ADV(p))
    {
	எனில் (*p == Ctrl_V)
	{
	    எனில் ((eap->argt & (EX_CTRLV | EX_XFILE)) || keep_backslash)
		++p;		// skip CTRL-V and next char
	    இல்லையெனில்
				// remove CTRL-V and skip next char
		STRMOVE(p, p + 1);
	    எனில் (*p == NUL)		// stop at NUL after CTRL-V
		நிறுத்து;
	}

#ifdef FEAT_EVAL
	// Skip over `=expr` when wildcards are expanded.
	இல்லையெனில் எனில் (p[0] == '`' && p[1] == '=' && (eap->argt & EX_XFILE))
	{
	    p += 2;
	    (வெறுமை)skip_கோவை(&p, வெற்று);
	    எனில் (*p == NUL)		// stop at NUL after CTRL-V
		நிறுத்து;
	}
#endif

	// Check for '"'/'#': start of comment or '|': next command
	// :@" and :*" do not start a comment!
	// :redir @" doesn't either.
	இல்லையெனில் எனில் ((*p == '"'
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
		    && eap->cmdidx != CMD_செருகு)
		|| *p == '\n')
	{
	    /*
	     * We remove the '\' before the '|', unless EX_CTRLV is used
	     * AND 'b' is present in 'cpoptions'.
	     */
	    எனில் ((vim_strchr(p_cpo, CPO_BAR) == வெற்று
			      || !(eap->argt & EX_CTRLV)) && *(p - 1) == '\\')
	    {
		எனில் (!keep_backslash)
		{
		    STRMOVE(p - 1, p);	// remove the '\'
		    --p;
		}
	    }
	    இல்லையெனில்
	    {
		set_nextcmd(eap, p);
		*p = NUL;
		நிறுத்து;
	    }
	}
    }

    எனில் (!(eap->argt & EX_NOTRLCOM))	// remove trailing spaces
	del_trailing_spaces(eap->arg);
}

/*
 * get + command from ex argument
 */
    நிலையான char_u *
getargcmd(char_u **argp)
{
    char_u *arg = *argp;
    char_u *command = வெற்று;

    எனில் (*arg == '+')	    // +[command]
    {
	++arg;
	எனில் (vim_isspace(*arg) || *arg == NUL)
	    command = dollar_command;
	இல்லையெனில்
	{
	    command = arg;
	    arg = skip_cmd_arg(command, TRUE);
	    எனில் (*arg != NUL)
		*arg++ = NUL;		// terminate command with NUL
	}

	arg = skipwhite(arg);	// skip over spaces
	*argp = arg;
    }
    திரும்பு command;
}

/*
 * Find end of "+command" argument.  Skip over "\ " and "\\".
 */
    char_u *
skip_cmd_arg(
    char_u *p,
    முழு	   rembs)	// TRUE to halve the number of backslashes
{
    போது (*p && !vim_isspace(*p))
    {
	எனில் (*p == '\\' && p[1] != NUL)
	{
	    எனில் (rembs)
		STRMOVE(p, p + 1);
	    இல்லையெனில்
		++p;
	}
	MB_PTR_ADV(p);
    }
    திரும்பு p;
}

    முழு
get_bad_opt(char_u *p, exarg_T *eap)
{
    எனில் (STRICMP(p, "keep") == 0)
	eap->bad_char = BAD_KEEP;
    இல்லையெனில் எனில் (STRICMP(p, "drop") == 0)
	eap->bad_char = BAD_DROP;
    இல்லையெனில் எனில் (MB_BYTE2LEN(*p) == 1 && p[1] == NUL)
	eap->bad_char = *p;
    இல்லையெனில்
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of bad= names.
 */
    நிலையான char_u *
get_bad_name(expand_T *xp UNUSED, முழு idx)
{
    // Note: Keep this in sync with getargopt.
    நிலையான எழுத்து *(p_bad_மதிப்புகள்[]) =
    {
	"?",
	"keep",
	"drop",
    };

    எனில் (idx < 0 || idx >= (முழு)ARRAY_LENGTH(p_bad_மதிப்புகள்))
	திரும்பு வெற்று;

    திரும்பு (char_u*)p_bad_மதிப்புகள்[idx];
}

/*
 * Get "++opt=arg" argument.
 * Return FAIL or OK.
 */
    நிலையான முழு
getargopt(exarg_T *eap)
{
    char_u	*arg = eap->arg + 2;
    முழு		*pp = வெற்று;
    முழு		bad_char_idx;
    char_u	*p;

    // Note: Keep this in sync with get_argopt_name.

    // ":edit ++[no]bin[ary] file"
    எனில் (STRNCMP(arg, "bin", 3) == 0 || STRNCMP(arg, "nobin", 5) == 0)
    {
	எனில் (*arg == 'n')
	{
	    arg += 2;
	    eap->force_bin = FORCE_NOBIN;
	}
	இல்லையெனில்
	    eap->force_bin = FORCE_BIN;
	எனில் (!checkforcmd(&arg, "binary", 3))
	    திரும்பு FAIL;
	eap->arg = skipwhite(arg);
	திரும்பு OK;
    }

    // ":read ++edit file"
    எனில் (STRNCMP(arg, "edit", 4) == 0)
    {
	eap->read_edit = TRUE;
	eap->arg = skipwhite(arg + 4);
	திரும்பு OK;
    }

    எனில் (STRNCMP(arg, "ff", 2) == 0)
    {
	arg += 2;
	pp = &eap->force_ff;
    }
    இல்லையெனில் எனில் (STRNCMP(arg, "fileformat", 10) == 0)
    {
	arg += 10;
	pp = &eap->force_ff;
    }
    இல்லையெனில் எனில் (STRNCMP(arg, "enc", 3) == 0)
    {
	எனில் (STRNCMP(arg, "encoding", 8) == 0)
	    arg += 8;
	இல்லையெனில்
	    arg += 3;
	pp = &eap->force_enc;
    }
    இல்லையெனில் எனில் (STRNCMP(arg, "bad", 3) == 0)
    {
	arg += 3;
	pp = &bad_char_idx;
    }

    எனில் (pp == வெற்று || *arg != '=')
	திரும்பு FAIL;

    ++arg;
    *pp = (முழு)(arg - eap->cmd);
    arg = skip_cmd_arg(arg, FALSE);
    eap->arg = skipwhite(arg);
    *arg = NUL;

    எனில் (pp == &eap->force_ff)
    {
	எனில் (check_ff_மதிப்பு(eap->cmd + eap->force_ff) == FAIL)
	    திரும்பு FAIL;
	eap->force_ff = eap->cmd[eap->force_ff];
    }
    இல்லையெனில் எனில் (pp == &eap->force_enc)
    {
	// Make 'fileencoding' lower case.
	க்கு (p = eap->cmd + eap->force_enc; *p != NUL; ++p)
	    *p = TOLOWER_ASC(*p);
    }
    இல்லையெனில்
    {
	// Check ++bad= argument.  Must be a single-byte character, "keep" or
	// "drop".
	எனில் (get_bad_opt(eap->cmd + bad_char_idx, eap) == FAIL)
	    திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of ++opt names.
 */
    நிலையான char_u *
get_argopt_name(expand_T *xp UNUSED, முழு idx)
{
    // Note: Keep this in sync with getargopt.
    நிலையான எழுத்து *(p_opt_மதிப்புகள்[]) =
    {
	"fileformat=",
	"encoding=",
	"binary",
	"nobinary",
	"bad=",
	"edit",
    };

    எனில் (idx < 0 || idx >= (முழு)ARRAY_LENGTH(p_opt_மதிப்புகள்))
	திரும்பு வெற்று;

    திரும்பு (char_u*)p_opt_மதிப்புகள்[idx];
}

/*
 * Command-line expansion for ++opt=name.
 */
    முழு
expand_argopt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	முழு	    *இலக்கம்_Matches)
{
    எனில் (xp->xp_pattern > xp->xp_வரி && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, முழு) = வெற்று;

	char_u *name_end = xp->xp_pattern - 1;
	எனில் (name_end - xp->xp_வரி >= 2
		&& STRNCMP(name_end - 2, "ff", 2) == 0)
	    cb = get_fileformat_name;
	இல்லையெனில் எனில் (name_end - xp->xp_வரி >= 10
		&& STRNCMP(name_end - 10, "fileformat", 10) == 0)
	    cb = get_fileformat_name;
	இல்லையெனில் எனில் (name_end - xp->xp_வரி >= 3
		&& STRNCMP(name_end - 3, "enc", 3) == 0)
	    cb = get_encoding_name;
	இல்லையெனில் எனில் (name_end - xp->xp_வரி >= 8
		&& STRNCMP(name_end - 8, "encoding", 8) == 0)
	    cb = get_encoding_name;
	இல்லையெனில் எனில் (name_end - xp->xp_வரி >= 3
		&& STRNCMP(name_end - 3, "bad", 3) == 0)
	    cb = get_bad_name;

	எனில் (cb != வெற்று)
	{
	    திரும்பு ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    இலக்கம்_Matches,
		    cb,
		    FALSE);
	}
	திரும்பு FAIL;
    }

    // Special handling of "ff" which acts as a short form of
    // "fileformat", as "ff" is not a substring of it.
    எனில் (xp->xp_pattern_len == 2
	    && STRNCMP(xp->xp_pattern, "ff", xp->xp_pattern_len) == 0)
    {
	*matches = ALLOC_MULT(char_u *, 1);
	எனில் (*matches == வெற்று)
	    திரும்பு FAIL;
	*இலக்கம்_Matches = 1;
	(*matches)[0] = vim_strsave((char_u*)"fileformat=");
	திரும்பு OK;
    }

    திரும்பு ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    இலக்கம்_Matches,
	    get_argopt_name,
	    FALSE);
}

    நிலையான வெறுமை
ex_autocmd(exarg_T *eap)
{
    எனில் (eap->skip)
    {
	skip_cmd_block(eap);
	திரும்பு;
    }

    /*
     * Disallow autocommands from .exrc and .vimrc in current
     * directory for security reasons.
     */
    எனில் (secure)
    {
	secure = 2;
	eap->errmsg =
	      _(e_command_not_allowed_from_vimrc_in_current_dir_or_tag_தேடல்);
    }
    இல்லையெனில் எனில் (eap->cmdidx == CMD_autocmd)
	do_autocmd(eap, eap->arg, eap->forceit);
    இல்லையெனில்
	do_augroup(eap->arg, eap->forceit);
}

/*
 * ":doautocmd": Apply the automatic commands to the current buffer.
 */
    நிலையான வெறுமை
ex_doautocmd(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    முழு		call_do_modelines = check_nomodeline(&arg);
    முழு		did_aucmd;

    (வெறுமை)do_doautocmd(arg, TRUE, &did_aucmd);
    // Only when there is no <nomodeline>.
    எனில் (call_do_modelines && did_aucmd)
	do_modelines(0);
}

/*
 * :[N]bunload[!] [N] [bufname] unload buffer
 * :[N]bdelete[!] [N] [bufname] delete buffer from buffer list
 * :[N]bwipeout[!] [N] [bufname] delete buffer really
 */
    நிலையான வெறுமை
ex_bunload(exarg_T *eap)
{
    எனில் (ERROR_IF_ANY_POPUP_WINDOW)
	திரும்பு;
    eap->errmsg = do_bufdel(
	    eap->cmdidx == CMD_bdelete ? DOBUF_DEL
		: eap->cmdidx == CMD_bwipeout ? DOBUF_WIPE
		: DOBUF_UNLOAD, eap->arg,
	    eap->addr_மொத்தம், (முழு)eap->line1, (முழு)eap->line2, eap->forceit);
}

/*
 * :[N]buffer [N]	to buffer N
 * :[N]sbuffer [N]	to buffer N
 */
    நிலையான வெறுமை
ex_buffer(exarg_T *eap)
{
    எனில் (ERROR_IF_ANY_POPUP_WINDOW)
	திரும்பு;
    do_exbuffer(eap);
}

/*
 * ":buffer" command and alike.
 */
    நிலையான வெறுமை
do_exbuffer(exarg_T *eap)
{
    எனில் (*eap->arg)
	eap->errmsg = ex_errmsg(e_trailing_characters_str, eap->arg);
    இல்லையெனில்
    {
	எனில் (eap->addr_மொத்தம் == 0)	// default is current buffer
	    goto_buffer(eap, DOBUF_CURRENT, FORWARD, 0);
	இல்லையெனில்
	    goto_buffer(eap, DOBUF_FIRST, FORWARD, (முழு)eap->line2);
	எனில் (eap->do_ecmd_cmd != வெற்று)
	    do_cmd_argument(eap->do_ecmd_cmd);
    }
}

/*
 * :[N]bmodified [N]	to next mod. buffer
 * :[N]sbmodified [N]	to next mod. buffer
 */
    நிலையான வெறுமை
ex_bmodified(exarg_T *eap)
{
    goto_buffer(eap, DOBUF_MOD, FORWARD, (முழு)eap->line2);
    எனில் (eap->do_ecmd_cmd != வெற்று)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bnext [N]	to next buffer
 * :[N]sbnext [N]	split and to next buffer
 */
    நிலையான வெறுமை
ex_bnext(exarg_T *eap)
{
    எனில் (ERROR_IF_ANY_POPUP_WINDOW)
	திரும்பு;

    goto_buffer(eap, DOBUF_CURRENT, FORWARD, (முழு)eap->line2);
    எனில் (eap->do_ecmd_cmd != வெற்று)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bNext [N]	to previous buffer
 * :[N]bprevious [N]	to previous buffer
 * :[N]sbNext [N]	split and to previous buffer
 * :[N]sbprevious [N]	split and to previous buffer
 */
    நிலையான வெறுமை
ex_bprevious(exarg_T *eap)
{
    எனில் (ERROR_IF_ANY_POPUP_WINDOW)
	திரும்பு;

    goto_buffer(eap, DOBUF_CURRENT, BACKWARD, (முழு)eap->line2);
    எனில் (eap->do_ecmd_cmd != வெற்று)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :brewind		to first buffer
 * :bfirst		to first buffer
 * :sbrewind		split and to first buffer
 * :sbfirst		split and to first buffer
 */
    நிலையான வெறுமை
ex_brewind(exarg_T *eap)
{
    எனில் (ERROR_IF_ANY_POPUP_WINDOW)
	திரும்பு;

    goto_buffer(eap, DOBUF_FIRST, FORWARD, 0);
    எனில் (eap->do_ecmd_cmd != வெற்று)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :blast		to last buffer
 * :sblast		split and to last buffer
 */
    நிலையான வெறுமை
ex_blast(exarg_T *eap)
{
    எனில் (ERROR_IF_ANY_POPUP_WINDOW)
	திரும்பு;

    goto_buffer(eap, DOBUF_LAST, BACKWARD, 0);
    எனில் (eap->do_ecmd_cmd != வெற்று)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * Check if "c" ends an Ex command.
 * In Vim9 script does not check for white space before #.
 */
    முழு
ends_excmd(முழு c)
{
    முழு comment_char = '"';

    எனில் (in_vim9script())
	comment_char = '#';
    திரும்பு (c == NUL || c == '|' || c == comment_char || c == '\n');
}

/*
 * Like ends_excmd() but checks that a # in Vim9 script either has "cmd" equal
 * to "cmd_start" or has a white space character before it.
 */
    முழு
ends_excmd2(char_u *cmd_start UNUSED, char_u *cmd)
{
    முழு c = *cmd;

    எனில் (c == NUL || c == '|' || c == '\n')
	திரும்பு TRUE;
    எனில் (in_vim9script())
	//  # starts a comment, #{ might be a mistake, #{{ can start a fold
	திரும்பு c == '#' && (cmd[1] != '{' || cmd[2] == '{')
				 && (cmd == cmd_start || VIM_ISWHITE(cmd[-1]));
    திரும்பு c == '"';
}

#எனில் defined(FEAT_SYN_HL) || defined(FEAT_தேடல்_EXTRA) || defined(FEAT_EVAL)
/*
 * Return the next command, after the first '|' or '\n'.
 * Return NULL if not found.
 */
    char_u *
find_nextcmd(char_u *p)
{
    போது (*p != '|' && *p != '\n')
    {
	எனில் (*p == NUL)
	    திரும்பு வெற்று;
	++p;
    }
    திரும்பு (p + 1);
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

    எனில் (*s == '|' || *s == '\n')
	திரும்பு (s + 1);
    இல்லையெனில்
	திரும்பு வெற்று;
}

/*
 * If "eap->nextcmd" is not set, check for a next command at "p".
 */
    வெறுமை
set_nextcmd(exarg_T *eap, char_u *arg)
{
    char_u *p = skipwhite(arg);

    எனில் (in_vim9script() && *p == '#')
    {
	char_u *nl = vim_strchr(p, NL);
	எனில் (nl != வெற்று)
	    p = nl;
    }

    p = check_nextcmd(p);

    எனில் (eap->nextcmd == வெற்று)
	eap->nextcmd = p;
    இல்லையெனில் எனில் (p != வெற்று)
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
    நிலையான முழு
check_more(
    முழு message,	    // when FALSE check only, no messages
    முழு forceit)
{
    முழு	    n = ARGCOUNT - curwin->w_arg_idx - 1;

    எனில் (!forceit && only_one_window()
	    && ARGCOUNT > 1 && !arg_had_last && n > 0 && quitmore == 0)
    {
	எனில் (message)
	{
#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    எனில் ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						    && curbuf->b_fname != வெற்று)
	    {
		char_u	buff[DIALOG_செய்தி_SIZE];

		vim_snprintf((எழுத்து *)buff, DIALOG_செய்தி_SIZE,
			NGETTEXT("%d more file to edit.  Quit anyway?",
			    "%d more files to edit.  Quit anyway?", n), n);
		எனில் (vim_dialog_yesno(VIM_QUESTION, வெற்று, buff, 1) == VIM_YES)
		    திரும்பு OK;
		திரும்பு FAIL;
	    }
#endif
	    semsg(NGETTEXT(e_nr_more_file_to_edit,
			   e_nr_more_files_to_edit , n), n);
	    quitmore = 2;	    // next try to quit is allowed
	}
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of command names.
 */
    char_u *
get_command_name(expand_T *xp UNUSED, முழு idx)
{
    எனில் (idx >= (முழு)CMD_SIZE)
	திரும்பு expand_user_command_name(idx);
    // the following are no real commands
    எனில் (STRNCMP(cmdnames[idx].cmd_name, "{", 1) == 0 ||
	STRNCMP(cmdnames[idx].cmd_name, "}", 1) == 0)
	திரும்பு (char_u *)"";
    திரும்பு cmdnames[idx].cmd_name;
}

    நிலையான வெறுமை
ex_colorscheme(exarg_T *eap)
{
    எனில் (*eap->arg == NUL)
    {
#ifdef FEAT_EVAL
	char_u *கோவை = vim_strsave((char_u *)"g:colors_name");
	char_u *p = வெற்று;

	எனில் (கோவை != வெற்று)
	{
	    ++emsg_off;
	    p = eval_to_string(கோவை, FALSE, FALSE);
	    --emsg_off;
	    vim_free(கோவை);
	}
	எனில் (p != வெற்று)
	{
	    செய்தி((எழுத்து *)p);
	    vim_free(p);
	}
	இல்லையெனில்
	    செய்தி("default");
#இல்லையெனில்
	செய்தி(_("unknown"));
#endif
    }
    இல்லையெனில் எனில் (load_colors(eap->arg) == FAIL)
	semsg(_(e_cannot_find_color_scheme_str), eap->arg);

#ifdef FEAT_VTP
    இல்லையெனில் எனில் (has_vtp_working())
    {
	// background color change requires clear + redraw
	update_screen(UPD_CLEAR);
	redrawcmd();
    }
#endif
}

    நிலையான வெறுமை
ex_highlight(exarg_T *eap)
{
    எனில் (*eap->arg == NUL && eap->cmd[2] == '!')
	செய்தி(_("Greetings, Vim user!"));
    do_highlight(eap->arg, eap->forceit, FALSE);
}


/*
 * Call this function if we thought we were going to exit, but we won't
 * (because of an error).  May need to restore the terminal mode.
 */
    வெறுமை
not_exiting(முழு save_exiting)
{
    exiting = save_exiting;
    settmode(TMODE_RAW);
}

    முழு
before_quit_autocmds(win_T *wp, முழு quit_all, முழு forceit)
{
    apply_autocmds(EVENT_QUITPRE, வெற்று, வெற்று, FALSE, wp->w_buffer);

    // Bail out when autocommands closed the window.
    // Refuse to quit when the buffer in the last window is being closed (can
    // only happen in autocommands).
    எனில் (!win_valid(wp)
	    || curbuf_locked()
	    || (wp->w_buffer->b_nwindows == 1 && wp->w_buffer->b_locked > 0))
	திரும்பு TRUE;

    எனில் (quit_all || (check_more(FALSE, forceit) == OK && only_one_window()))
    {
	apply_autocmds(EVENT_EXITPRE, வெற்று, வெற்று, FALSE, curbuf);
	// Refuse to quit when locked or when the window was closed or the
	// buffer in the last window is being closed (can only happen in
	// autocommands).
	எனில் (!win_valid(wp) || curbuf_locked()
			  || (curbuf->b_nwindows == 1 && curbuf->b_locked > 0))
	    திரும்பு TRUE;
    }

    திரும்பு FALSE;
}

/*
 * ":quit": quit current window, quit Vim if the last window is closed.
 * ":{nr}quit": quit window {nr}
 * Also used when closing a terminal window that's the last one.
 */
    வெறுமை
ex_quit(exarg_T *eap)
{
    win_T	*wp;

    எனில் (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	திரும்பு;
    }
    // Don't quit while editing the command line.
    எனில் (text_locked())
    {
	text_locked_செய்தி();
	திரும்பு;
    }
    எனில் (eap->addr_மொத்தம் > 0)
    {
	முழு	wnr = eap->line2;

	க்கு (wp = firstwin; wp->w_next != வெற்று; wp = wp->w_next)
	    எனில் (--wnr <= 0)
		நிறுத்து;
    }
    இல்லையெனில்
	wp = curwin;

    // Refuse to quit when locked.
    எனில் (curbuf_locked())
	திரும்பு;

    // Trigger QuitPre and maybe ExitPre
    எனில் (before_quit_autocmds(wp, FALSE, eap->forceit))
	திரும்பு;

#ifdef FEAT_NETBEANS_INTG
    netbeansForcedQuit = eap->forceit;
#endif

    முழு save_exiting = exiting;
    /*
     * If there is only one relevant window we will exit.
     */
    எனில் (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;
    எனில் ((!buf_hide(wp->w_buffer)
		&& check_changed(wp->w_buffer, (p_awa ? CCGD_AW : 0)
				       | (eap->forceit ? CCGD_FORCEIT : 0)
				       | CCGD_EXCMD))
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, TRUE)))
    {
	not_exiting(save_exiting);
    }
    இல்லையெனில்
    {
	// quit last window
	// Note: only_one_window() returns true, even so a help window is
	// still open. In that case only quit, if no address has been
	// specified. Example:
	// :h|wincmd w|1q     - don't quit
	// :h|wincmd w|q      - quit
	எனில் (only_one_window() && (ONE_WINDOW || eap->addr_மொத்தம் == 0))
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
    நிலையான வெறுமை
ex_cquit(exarg_T *eap UNUSED)
{
    // this does not always pass on the exit code to the Manx compiler. why?
    getout(eap->addr_மொத்தம் > 0 ? (முழு)eap->line2 : EXIT_FAILURE);
}

/*
 * Do preparations for "qall" and "wqall".
 * Returns FAIL when quitting should be aborted.
 */
    முழு
before_quit_all(exarg_T *eap)
{
    எனில் (cmdwin_type != 0)
    {
	எனில் (eap->forceit)
	    cmdwin_result = K_XF1;	// ex_window() takes care of this
	இல்லையெனில்
	    cmdwin_result = K_XF2;
	திரும்பு FAIL;
    }

    // Don't quit while editing the command line.
    எனில் (text_locked())
    {
	text_locked_செய்தி();
	திரும்பு FAIL;
    }

    எனில் (before_quit_autocmds(curwin, TRUE, eap->forceit))
	திரும்பு FAIL;

    திரும்பு OK;
}

/*
 * ":qall": try to quit all windows
 */
    நிலையான வெறுமை
ex_quit_all(exarg_T *eap)
{
    எனில் (before_quit_all(eap) == FAIL)
	திரும்பு;
    முழு save_exiting = exiting;
    exiting = TRUE;
    எனில் (eap->forceit || !check_changed_any(FALSE, FALSE))
	getout(0);
    not_exiting(save_exiting);
}

/*
 * ":close": close current window, unless it is the last one
 */
    நிலையான வெறுமை
ex_close(exarg_T *eap)
{
    win_T	*win;
    முழு		winnr = 0;
    எனில் (cmdwin_type != 0)
	cmdwin_result = Ctrl_C;
    இல்லையெனில்
	எனில் (!text_locked() && !curbuf_locked())
	{
	    எனில் (eap->addr_மொத்தம் == 0)
		ex_win_close(eap->forceit, curwin, வெற்று);
	    இல்லையெனில்
	    {
		FOR_ALL_WINDOWS(win)
		{
		    winnr++;
		    எனில் (winnr == eap->line2)
			நிறுத்து;
		}
		எனில் (win == வெற்று)
		    win = lastwin;
		ex_win_close(eap->forceit, win, வெற்று);
	    }
	}
}

#ifdef FEAT_QUICKFIX
/*
 * ":pclose": Close any preview window.
 */
    நிலையான வெறுமை
ex_pclose(exarg_T *eap)
{
    win_T	*win;

    // First close any normal window.
    FOR_ALL_WINDOWS(win)
	எனில் (win->w_p_pvw)
	{
	    ex_win_close(eap->forceit, win, வெற்று);
	    திரும்பு;
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
    நிலையான வெறுமை
ex_win_close(
    முழு		forceit,
    win_T	*win,
    tabpage_T	*tp)		// NULL or the tab page "win" is in
{
    முழு		need_hide;
    buf_T	*buf = win->w_buffer;

    // Never close the autocommand window.
    எனில் (is_aucmd_win(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	திரும்பு;
    }
    எனில் (window_layout_locked(CMD_close))
	திரும்பு;

    need_hide = (bufIsChanged(buf) && buf->b_nwindows <= 1);
    எனில் (need_hide && !buf_hide(buf) && !forceit)
    {
#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	எனில் ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    எனில் (term_job_running(buf->b_term))
	    {
		எனில் (term_confirm_stop(buf) == FAIL)
		    திரும்பு;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(buf);
		need_hide = FALSE;
	    }
	    இல்லையெனில்
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(buf, FALSE);
		எனில் (bufref_valid(&bufref) && bufIsChanged(buf))
		    திரும்பு;
		need_hide = FALSE;
	    }
	}
	இல்லையெனில்
#endif
	{
	    no_write_message();
	    திரும்பு;
	}
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // free buffer when not hiding it or when it's a scratch buffer
    எனில் (tp == வெற்று)
	win_close(win, !need_hide && !buf_hide(buf));
    இல்லையெனில்
	win_close_othertab(win, !need_hide && !buf_hide(buf), tp);
}

/*
 * Handle the argument for a tabpage related ex command.
 * Returns a tabpage number.
 * When an error is encountered then eap->errmsg is set.
 */
    நிலையான முழு
get_tabpage_arg(exarg_T *eap)
{
    முழு tab_எண்;
    முழு unaccept_arg0 = (eap->cmdidx == CMD_tabmove) ? 0 : 1;

    எனில் (eap->arg && *eap->arg != NUL)
    {
	char_u *p = eap->arg;
	char_u *p_save;
	முழு    relative = 0; // argument +N/-N means: go to N places to the
			     // right/left relative to the current position.

	எனில் (*p == '-')
	{
	    relative = -1;
	    p++;
	}
	இல்லையெனில் எனில் (*p == '+')
	{
	    relative = 1;
	    p++;
	}

	p_save = p;
	tab_எண் = getdigits(&p);

	எனில் (relative == 0)
	{
	    எனில் (STRCMP(p, "$") == 0)
		tab_எண் = LAST_TAB_NR;
	    இல்லையெனில் எனில் (STRCMP(p, "#") == 0)
		எனில் (valid_tabpage(lastused_tabpage))
		    tab_எண் = tabpage_index(lastused_tabpage);
		இல்லையெனில்
		{
		    eap->errmsg = ex_errmsg(e_invalid_மதிப்பு_for_argument_str,
								     eap->arg);
		    tab_எண் = 0;
		    செல் theend;
		}
	    இல்லையெனில் எனில் (p == p_save || *p_save == '-' || *p != NUL
		    || tab_எண் > LAST_TAB_NR)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		செல் theend;
	    }
	}
	இல்லையெனில்
	{
	    எனில் (*p_save == NUL)
		tab_எண் = 1;
	    இல்லையெனில் எனில் (p == p_save || *p_save == '-' || *p != NUL
		    || tab_எண் == 0)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		செல் theend;
	    }
	    tab_எண் = tab_எண் * relative + tabpage_index(curtab);
	    எனில் (!unaccept_arg0 && relative == -1)
		--tab_எண்;
	}
	எனில் (tab_எண் < unaccept_arg0 || tab_எண் > LAST_TAB_NR)
	    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
    }
    இல்லையெனில் எனில் (eap->addr_மொத்தம் > 0)
    {
	எனில் (unaccept_arg0 && eap->line2 == 0)
	{
	    eap->errmsg = _(e_invalid_range);
	    tab_எண் = 0;
	}
	இல்லையெனில்
	{
	    tab_எண் = eap->line2;
	    எனில் (!unaccept_arg0)
	    {
		char_u *cmdp = eap->cmd;

		போது (--cmdp > *eap->cmdlinep
			&& (VIM_ISWHITE(*cmdp) || VIM_ISDIGIT(*cmdp)))
		    ;
		எனில் (*cmdp == '-')
		{
		    --tab_எண்;
		    எனில் (tab_எண் < unaccept_arg0)
			eap->errmsg = _(e_invalid_range);
		}
	    }
	}
    }
    இல்லையெனில்
    {
	தேர்வு (eap->cmdidx)
	{
	நிலை CMD_tabnext:
	    tab_எண் = tabpage_index(curtab) + 1;
	    எனில் (tab_எண் > LAST_TAB_NR)
		tab_எண் = 1;
	    நிறுத்து;
	நிலை CMD_tabmove:
	    tab_எண் = LAST_TAB_NR;
	    நிறுத்து;
	இயல்பு:
	    tab_எண் = tabpage_index(curtab);
	}
    }

theend:
    திரும்பு tab_எண்;
}

/*
 * ":tabclose": close current tab page, unless it is the last one.
 * ":tabclose N": close tab page N.
 */
    நிலையான வெறுமை
ex_tabclose(exarg_T *eap)
{
    tabpage_T	*tp;
    முழு		tab_எண்;

    எனில் (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	திரும்பு;
    }

    எனில் (first_tabpage->tp_next == வெற்று)
    {
	emsg(_(e_cannot_close_last_tab_page));
	திரும்பு;
    }

    எனில் (window_layout_locked(CMD_tabclose))
	திரும்பு;

    tab_எண் = get_tabpage_arg(eap);
    எனில் (eap->errmsg != வெற்று)
	திரும்பு;

    tp = find_tabpage(tab_எண்);
    எனில் (tp == வெற்று)
    {
	beep_flush();
	திரும்பு;
    }
    எனில் (tp != curtab)
    {
	tabpage_close_other(tp, eap->forceit);
	திரும்பு;
    }
    இல்லையெனில் எனில் (!text_locked() && !curbuf_locked())
	tabpage_close(eap->forceit);
}

/*
 * ":tabonly": close all tab pages except the current one
 */
    நிலையான வெறுமை
ex_tabonly(exarg_T *eap)
{
    tabpage_T	*tp;
    முழு		done;
    முழு		tab_எண்;

    எனில் (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	திரும்பு;
    }

    எனில் (first_tabpage->tp_next == வெற்று)
    {
	செய்தி(_("Already only one tab page"));
	திரும்பு;
    }

    எனில் (window_layout_locked(CMD_tabonly))
	திரும்பு;

    tab_எண் = get_tabpage_arg(eap);
    எனில் (eap->errmsg != வெற்று)
	திரும்பு;

    goto_tabpage(tab_எண்);
    // Repeat this up to a 1000 times, because autocommands may
    // mess up the lists.
    க்கு (done = 0; done < 1000; ++done)
    {
	FOR_ALL_TABPAGES(tp)
	    எனில் (tp->tp_topframe != topframe)
	    {
		tabpage_close_other(tp, eap->forceit);
		// if we failed to close it quit
		எனில் (valid_tabpage(tp))
		    done = 1000;
		// start over, "tp" is now invalid
		நிறுத்து;
	    }
	எனில் (first_tabpage->tp_next == வெற்று)
	    நிறுத்து;
    }
}

/*
 * Close the current tab page.
 */
    வெறுமை
tabpage_close(முழு forceit)
{
    எனில் (window_layout_locked(CMD_tabclose))
	திரும்பு;

    trigger_tabclosedpre(curtab);
    curtab->tp_did_tabclosedpre = TRUE;
    tabpage_T *save_curtab = curtab;

    // First close all the windows but the current one.  If that worked then
    // close the last window in this tab, that will close it.
    எனில் (!ONE_WINDOW)
	close_others(TRUE, forceit);
    எனில் (ONE_WINDOW)
	ex_win_close(forceit, curwin, வெற்று);
    எனில் (curtab == save_curtab)
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
    வெறுமை
tabpage_close_other(tabpage_T *tp, முழு forceit)
{
    முழு		done = 0;
    win_T	*wp;

    எனில் (window_layout_locked(CMD_SIZE))
	திரும்பு;

    trigger_tabclosedpre(tp);
    tp->tp_did_tabclosedpre = TRUE;

    // Limit to 1000 windows, autocommands may add a window while we close
    // one.  OK, so I'm paranoid...
    போது (++done < 1000)
    {
	wp = tp->tp_firstwin;
	ex_win_close(forceit, wp, tp);

	// Autocommands may delete the tab page under our fingers.
	எனில் (!valid_tabpage(tp))
	    நிறுத்து;
	// We may fail to close a window with a modified buffer.
	எனில் (tp->tp_firstwin == wp)
	{
	    done = 1000;
	    நிறுத்து;
	}
    }
    எனில் (done >= 1000)
    {
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	tp->tp_did_tabclosedpre = FALSE;
	திரும்பு;
    }

    apply_autocmds(EVENT_TABCLOSED, வெற்று, வெற்று, FALSE, curbuf);
}

/*
 * ":only".
 */
    நிலையான வெறுமை
ex_only(exarg_T *eap)
{
    எனில் (window_layout_locked(CMD_only))
	திரும்பு;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    எனில் (eap->addr_மொத்தம் > 0)
    {
	win_T   *wp;
	முழு	wnr = eap->line2;
	க்கு (wp = firstwin; --wnr > 0; )
	{
	    எனில் (wp->w_next == வெற்று)
		நிறுத்து;
	    இல்லையெனில்
		wp = wp->w_next;
	}
	win_goto(wp);
    }
    close_others(TRUE, eap->forceit);
}

    நிலையான வெறுமை
ex_hide(exarg_T *eap UNUSED)
{
    // ":hide" or ":hide | cmd": hide current window
    எனில் (eap->skip)
	திரும்பு;

    எனில் (window_layout_locked(CMD_hide))
	திரும்பு;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    எனில் (eap->addr_மொத்தம் == 0)
	win_close(curwin, FALSE);	// don't free buffer
    இல்லையெனில்
    {
	முழு	winnr = 0;
	win_T	*win;

	FOR_ALL_WINDOWS(win)
	{
	    winnr++;
	    எனில் (winnr == eap->line2)
		நிறுத்து;
	}
	எனில் (win == வெற்று)
	    win = lastwin;
	win_close(win, FALSE);
    }
}

/*
 * ":stop" and ":suspend": Suspend Vim.
 */
    வெறுமை
ex_stop(exarg_T *eap)
{
    /*
     * Disallow suspending for "rvim".
     */
    எனில் (check_restricted())
	திரும்பு;

    எனில் (!eap->forceit)
	autowrite_all();
    apply_autocmds(EVENT_VIMSUSPEND, வெற்று, வெற்று, FALSE, வெற்று);
    windgoto((முழு)Rows - 1, cmdline_col_off);
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
    apply_autocmds(EVENT_VIMRESUME, வெற்று, வெற்று, FALSE, வெற்று);
}

/*
 * ":exit", ":xit" and ":wq": Write file and quit the current window.
 */
    நிலையான வெறுமை
ex_exit(exarg_T *eap)
{
#ifdef FEAT_EVAL
    எனில் (not_in_vim9(eap, false) == FAIL)
	திரும்பு;
#endif
    எனில் (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	திரும்பு;
    }
    // Don't quit while editing the command line.
    எனில் (text_locked())
    {
	text_locked_செய்தி();
	திரும்பு;
    }

    முழு save_exiting = exiting;
    /*
     * we plan to exit if there is only one relevant window
     */
    எனில் (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;

    // Write the buffer for ":wq" or when it was changed.
    // Trigger QuitPre and ExitPre.
    // Check if we can exit now, after autocommands have changed things.
    எனில் (((eap->cmdidx == CMD_wq || curbufIsChanged()) && do_write(eap) == FAIL)
	    || before_quit_autocmds(curwin, FALSE, eap->forceit)
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, FALSE)))
    {
	not_exiting(save_exiting);
    }
    இல்லையெனில்
    {
	எனில் (only_one_window())	    // quit last window, exit Vim
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
    நிலையான வெறுமை
ex_print(exarg_T *eap)
{
    எனில் (curbuf->b_ml.ml_flags & ML_EMPTY)
	emsg(_(e_empty_buffer));
    இல்லையெனில்
    {
	க்கு ( ;!got_int; ui_breakcheck())
	{
	    print_வரி(eap->line1,
		    (eap->cmdidx == CMD_எண் || eap->cmdidx == CMD_pound
						 || (eap->flags & EXFLAG_NR)),
		    eap->cmdidx == CMD_list || (eap->flags & EXFLAG_LIST));
	    எனில் (++eap->line1 > eap->line2)
		நிறுத்து;
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
    நிலையான வெறுமை
ex_goto(exarg_T *eap)
{
    goto_byte(eap->line2);
}
#endif

/*
 * ":shell".
 */
    நிலையான வெறுமை
ex_shell(exarg_T *eap UNUSED)
{
    do_shell(வெற்று, 0);
}

#எனில் defined(HAVE_DROP_FILE)

நிலையான முழு drop_busy = FALSE;
நிலையான முழு drop_filec;
நிலையான char_u **drop_filev = வெற்று;
நிலையான முழு drop_split;
நிலையான வெறுமை (*drop_callback)(வெறுமை *);
நிலையான வெறுமை *drop_cookie;

    நிலையான வெறுமை
handle_drop_internal(வெறுமை)
{
    exarg_T	ea;
    முழு		save_செய்தி_scroll = செய்தி_scroll;

    // Setting the argument list may cause screen updates and being called
    // recursively.  Avoid that by setting drop_busy.
    drop_busy = TRUE;

    // Check whether the current buffer is changed. If so, we will need
    // to split the current window or data could be lost.
    // We don't need to check if the 'hidden' option is set, as in this
    // case the buffer won't be lost.
    எனில் (!buf_hide(curbuf) && !drop_split)
    {
	++emsg_off;
	drop_split = check_changed(curbuf, CCGD_AW);
	--emsg_off;
    }
    எனில் (drop_split)
    {
	எனில் (win_split(0, 0) == FAIL)
	    திரும்பு;
	RESET_BINDING(curwin);

	// When splitting the window, create a new alist.  Otherwise the
	// existing one is overwritten.
	alist_unlink(curwin->w_alist);
	alist_new();
    }

    /*
     * Set up the new argument list.
     */
    alist_set(ALIST(curwin), drop_filec, drop_filev, FALSE, வெற்று, 0);

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
    செய்தி_scroll = save_செய்தி_scroll;

    எனில் (drop_callback != வெற்று)
	drop_callback(drop_cookie);

    drop_filev = வெற்று;
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
    வெறுமை
handle_drop(
    முழு		filec,		// the number of files dropped
    char_u	**filev,	// the list of files dropped
    முழு		split,		// force splitting the window
    வெறுமை	(*callback)(வெறுமை *), // to be called after setting the argument
				     // list
    வெறுமை	*cookie)	// argument for "callback" (allocated)
{
    // Cannot handle recursive drops, finish the pending one.
    எனில் (drop_busy)
    {
	FreeWild(filec, filev);
	vim_free(cookie);
	திரும்பு;
    }

    // When calling handle_drop() more than once in a row we only use the last
    // one.
    எனில் (drop_filev != வெற்று)
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
    எனில் (text_locked() || curbuf_locked() || updating_screen)
	திரும்பு;

    handle_drop_internal();
}

/*
 * To be called when text is unlocked, curbuf is unlocked or updating_screen is
 * reset: Handle a postponed drop.
 */
    வெறுமை
handle_any_postponed_drop(வெறுமை)
{
    எனில் (!drop_busy && drop_filev != வெற்று
	     && !text_locked() && !curbuf_locked() && !updating_screen)
	handle_drop_internal();
}
#endif

/*
 * ":preserve".
 */
    நிலையான வெறுமை
ex_preserve(exarg_T *eap UNUSED)
{
    curbuf->b_flags |= BF_PRESERVED;
    ml_preserve(curbuf, TRUE);
}

/*
 * ":recover".
 */
    நிலையான வெறுமை
ex_recover(exarg_T *eap)
{
    // Set recoverymode right away to avoid the ATTENTION prompt.
    recoverymode = TRUE;
    எனில் (!check_changed(curbuf, (p_awa ? CCGD_AW : 0)
			     | CCGD_MULTWIN
			     | (eap->forceit ? CCGD_FORCEIT : 0)
			     | CCGD_EXCMD)

	    && (*eap->arg == NUL
			     || setfname(curbuf, eap->arg, வெற்று, TRUE) == OK))
	ml_recover(TRUE);
    recoverymode = FALSE;
}

/*
 * Command modifier used in a wrong way.  Also for other commands that can't
 * appear at the toplevel.
 */
    நிலையான வெறுமை
ex_wrongmodifier(exarg_T *eap)
{
    eap->errmsg = ex_errmsg(e_invalid_command_str, eap->cmd);
}

#எனில் defined(FEAT_EVAL)

// callback function for 'findfunc'
நிலையான callback_T ffu_cb;

    நிலையான callback_T *
get_findfunc_callback(வெறுமை)
{
    திரும்பு *curbuf->b_p_ffu != NUL ? &curbuf->b_ffu_cb : &ffu_cb;
}

/*
 * Call 'findfunc' to obtain a list of file names.
 */
    நிலையான list_T *
call_findfunc(char_u *pat, முழு cmdcomplete)
{
    typval_T	args[3];
    callback_T	*cb;
    typval_T	rettv;
    முழு		retval;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_BOOL;
    args[1].vval.v_எண் = cmdcomplete;
    args[2].v_type = VAR_UNKNOWN;

    // Lock the text to prevent weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;

    ctx = get_option_sctx("findfunc");
    எனில் (ctx != வெற்று)
	current_sctx = *ctx;

    cb = get_findfunc_callback();
    retval = call_callback(cb, -1, &rettv, 2, args);

    current_sctx = saved_sctx;

    --textlock;

    list_T *retlist = வெற்று;

    எனில் (retval == OK)
    {
	எனில் (rettv.v_type == VAR_LIST)
	    retlist = list_copy(rettv.vval.v_list, FALSE, FALSE, get_copyID());
	இல்லையெனில்
	    emsg(_(e_invalid_return_type_from_findfunc));

	clear_tv(&rettv);
    }

    திரும்பு retlist;
}

/*
 * Find file names matching "pat" using 'findfunc' and return it in "files".
 * Used for expanding the :find, :sfind and :tabfind command argument.
 * Returns OK on success and FAIL otherwise.
 */
    முழு
expand_findfunc(expand_T *xp, char_u *pat, char_u ***files, முழு *இலக்கம்_Matches)
{
    list_T	*l;
    முழு		len;

    *இலக்கம்_Matches = 0;
    *files = வெற்று;

    l = call_findfunc(pat, VVAL_TRUE);

    எனில் (l == வெற்று)
	திரும்பு FAIL;

    len = list_len(l);
    எனில் (len == 0)	    // empty List
    {
	list_free(l);
	திரும்பு FAIL;
    }

    expand_process_user_list(l, files, இலக்கம்_Matches, xp);
    list_free(l);

    திரும்பு OK;
}

/*
 * Use 'findfunc' to find file 'findarg'.  The 'count' argument is used to find
 * the n'th matching file.
 */
    நிலையான char_u *
findfunc_find_file(char_u *findarg, முழு findarg_len, முழு மொத்தம்)
{
    list_T	*fname_list;
    char_u	*ret_fname = வெற்று;
    char_u	cc;
    முழு		fname_மொத்தம்;

    cc = findarg[findarg_len];
    findarg[findarg_len] = NUL;

    fname_list = call_findfunc(findarg, VVAL_FALSE);
    fname_மொத்தம் = list_len(fname_list);

    எனில் (fname_மொத்தம் == 0)
	semsg(_(e_cant_find_file_str_in_path), findarg);
    இல்லையெனில்
    {
	எனில் (மொத்தம் > fname_மொத்தம்)
	    semsg(_(e_no_more_file_str_found_in_path), findarg);
	இல்லையெனில்
	{
	    listitem_T *li = list_find(fname_list, மொத்தம் - 1);

	    எனில் (li != வெற்று)
	    {
		typval_T *tv = &li->li_tv;

		எனில் (tv->v_type == VAR_STRING && tv->vval.v_string != வெற்று)
		    ret_fname = vim_strsave(tv->vval.v_string);
		இல்லையெனில் எனில் (tv->v_type == VAR_DICT && tv->vval.v_dict != வெற்று)
		    ret_fname = dict_get_string(tv->vval.v_dict, "word", TRUE);
	    }
	}
    }

    எனில் (fname_list != வெற்று)
	list_free(fname_list);

    findarg[findarg_len] = cc;

    திரும்பு ret_fname;
}

/*
 * Process the 'findfunc' option value.
 * Returns NULL on success and an error message on failure.
 */
    எழுத்து *
did_set_findfunc(optset_T *args UNUSED)
{
    முழு	retval;

    எனில் (args->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_ffu, &curbuf->b_ffu_cb);
    இல்லையெனில்
    {
	// global option set
	retval = option_set_callback_func(p_ffu, &ffu_cb);
	// when using :set, free the local callback
	எனில் (!(args->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_ffu_cb);
    }

    எனில் (retval == FAIL)
	திரும்பு e_invalid_argument;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*name = get_scriptlocal_funcname(*varp);
    எனில் (name != வெற்று)
    {
	free_string_option(*varp);
	*varp = name;
    }

    திரும்பு வெற்று;
}

# எனில் defined(EXITFREE)
    வெறுமை
free_findfunc_option(வெறுமை)
{
    free_callback(&ffu_cb);
}
# endif

/*
 * Mark the global 'findfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    முழு
set_ref_in_findfunc(முழு copyID UNUSED)
{
    முழு abort = FALSE;
    abort = set_ref_in_callback(&ffu_cb, copyID);
    திரும்பு abort;
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
    வெறுமை
ex_splitview(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    char_u	*fname = வெற்று;
#ifdef FEAT_BROWSE
    char_u	dot_path[] = ".";
    முழு		save_cmod_flags = cmdmod.cmod_flags;
#endif
    முழு		use_tab = eap->cmdidx == CMD_tabedit
		       || eap->cmdidx == CMD_tabfind
		       || eap->cmdidx == CMD_tabnew;

    எனில் (ERROR_IF_ANY_POPUP_WINDOW)
	திரும்பு;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#ifdef FEAT_QUICKFIX
    // A ":split" in the quickfix window works like ":new".  Don't want two
    // quickfix windows.  But it's OK when doing ":tab split".
    எனில் (bt_quickfix(curbuf) && cmdmod.cmod_tab == 0)
    {
	எனில் (eap->cmdidx == CMD_split)
	    eap->cmdidx = CMD_new;
	எனில் (eap->cmdidx == CMD_vsplit)
	    eap->cmdidx = CMD_vnew;
    }
#endif

    எனில் (eap->cmdidx == CMD_sfind || eap->cmdidx == CMD_tabfind)
    {
	char_u	*file_to_find = வெற்று;
	எழுத்து	*தேடல்_ctx = வெற்று;

	எனில் (*get_findfunc() != NUL)
	{
#ifdef FEAT_EVAL
	    fname = findfunc_find_file(eap->arg, (முழு)STRLEN(eap->arg),
				       eap->addr_மொத்தம் > 0 ? eap->line2 : 1);
#endif
	}
	இல்லையெனில்
	{
	    fname = find_file_in_path(eap->arg, (முழு)STRLEN(eap->arg),
					      FNAME_MESS, TRUE, curbuf->b_ffname,
					      &file_to_find, &தேடல்_ctx);
	    vim_free(file_to_find);
	    vim_findfile_cleanup(தேடல்_ctx);
	}
	எனில் (fname == வெற்று)
	    செல் theend;
	eap->arg = fname;
    }
#ifdef FEAT_BROWSE
    இல்லையெனில் எனில் ((cmdmod.cmod_flags & CMOD_BROWSE)
	    && eap->cmdidx != CMD_vnew
	    && eap->cmdidx != CMD_new)
    {
	எனில் (
# ifdef FEAT_GUI
	    !gui.in_use &&
# endif
		au_has_group((char_u *)"FileExplorer"))
	{
	    // No browsing supported but we do have the file explorer:
	    // Edit the directory.
	    எனில் (*eap->arg == NUL || !mch_isdir(eap->arg))
		eap->arg = dot_path;
	}
	இல்லையெனில்
	{
	    fname = do_browse(0, (char_u *)(use_tab
			? _("Edit File in new tab page")
			: _("Edit File in new window")),
					  eap->arg, வெற்று, வெற்று, வெற்று, curbuf);
	    எனில் (fname == வெற்று)
		செல் theend;
	    eap->arg = fname;
	}
    }
    cmdmod.cmod_flags &= ~CMOD_BROWSE;	// Don't browse again in do_ecmd().
#endif

    /*
     * Either open new tab page or split the window.
     */
    எனில் (use_tab)
    {
	எனில் (win_new_tabpage(cmdmod.cmod_tab != 0 ? cmdmod.cmod_tab
			 : eap->addr_மொத்தம் == 0 ? 0
					       : (முழு)eap->line2 + 1) != FAIL)
	{
	    do_exedit(eap, old_curwin);

	    // set the alternate buffer for the window we came from
	    எனில் (curwin != old_curwin
		    && win_valid(old_curwin)
		    && old_curwin->w_buffer != curbuf
		    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		old_curwin->w_alt_fnum = curbuf->b_fnum;
	}
    }
    இல்லையெனில் எனில் (win_split(eap->addr_மொத்தம் > 0 ? (முழு)eap->line2 : 0,
				     *eap->cmd == 'v' ? WSP_VERT : 0) != FAIL)
    {
	// Reset 'scrollbind' when editing another file, but keep it when
	// doing ":split" without arguments.
	எனில் (*eap->arg != NUL)
	    RESET_BINDING(curwin);
	இல்லையெனில்
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
    வெறுமை
tabpage_new(வெறுமை)
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
    நிலையான வெறுமை
ex_tabnext(exarg_T *eap)
{
    முழு tab_எண்;

    எனில் (ERROR_IF_POPUP_WINDOW)
	திரும்பு;
    தேர்வு (eap->cmdidx)
    {
	நிலை CMD_tabfirst:
	நிலை CMD_tabrewind:
	    goto_tabpage(1);
	    நிறுத்து;
	நிலை CMD_tablast:
	    goto_tabpage(9999);
	    நிறுத்து;
	நிலை CMD_tabprevious:
	நிலை CMD_tabNext:
	    எனில் (eap->arg && *eap->arg != NUL)
	    {
		char_u *p = eap->arg;
		char_u *p_save = p;

		tab_எண் = getdigits(&p);
		எனில் (p == p_save || *p_save == '-' || *p != NUL
			    || tab_எண் == 0)
		{
		    // No numbers as argument.
		    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		    திரும்பு;
		}
	    }
	    இல்லையெனில்
	    {
		எனில் (eap->addr_மொத்தம் == 0)
		    tab_எண் = 1;
		இல்லையெனில்
		{
		    tab_எண் = eap->line2;
		    எனில் (tab_எண் < 1)
		    {
			eap->errmsg = _(e_invalid_range);
			திரும்பு;
		    }
		}
	    }
	    goto_tabpage(-tab_எண்);
	    நிறுத்து;
	இயல்பு: // CMD_tabnext
	    tab_எண் = get_tabpage_arg(eap);
	    எனில் (eap->errmsg == வெற்று)
		goto_tabpage(tab_எண்);
	    நிறுத்து;
    }
}

/*
 * :tabmove command
 */
    நிலையான வெறுமை
ex_tabmove(exarg_T *eap)
{
    முழு tab_எண்;

    tab_எண் = get_tabpage_arg(eap);
    எனில் (eap->errmsg == வெற்று)
	tabpage_move(tab_எண்);
}

/*
 * :tabs command: List tabs and their contents.
 */
    நிலையான வெறுமை
ex_tabs(exarg_T *eap UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp;
    முழு		tabcount = 1;

    செய்தி_start();
    செய்தி_scroll = TRUE;
    க்கு (tp = first_tabpage; tp != வெற்று && !got_int; tp = tp->tp_next)
    {
	செய்தி_putchar('\n');
	vim_snprintf((எழுத்து *)IObuff, IOSIZE, _("Tab page %d"), tabcount++);
	செய்தி_outtrans_attr(IObuff, HL_ATTR(HLF_T));
	out_flush();	    // output one line at a time
	ui_breakcheck();

	எனில் (tp  == curtab)
	    wp = firstwin;
	இல்லையெனில்
	    wp = tp->tp_firstwin;
	க்கு ( ; wp != வெற்று && !got_int; wp = wp->w_next)
	{
	    செய்தி_putchar('\n');
	    செய்தி_putchar(wp == curwin ? '>' : ' ');
	    செய்தி_putchar(' ');
	    செய்தி_putchar(bufIsChanged(wp->w_buffer) ? '+' : ' ');
	    செய்தி_putchar(' ');
	    எனில் (buf_spname(wp->w_buffer) != வெற்று)
		vim_strncpy(IObuff, buf_spname(wp->w_buffer), IOSIZE - 1);
	    இல்லையெனில்
		home_replace(wp->w_buffer, wp->w_buffer->b_fname,
							IObuff, IOSIZE, TRUE);
	    செய்தி_outtrans(IObuff);
	    out_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }
}

/*
 * ":mode": Set screen mode.
 * If no argument given, just get the screen size and redraw.
 */
    நிலையான வெறுமை
ex_mode(exarg_T *eap)
{
    எனில் (*eap->arg == NUL)
	shell_resized();
    இல்லையெனில்
	emsg(_(e_screen_mode_setting_not_supported));
}

/*
 * ":resize".
 * set, increment or decrement current window height
 */
    நிலையான வெறுமை
ex_resize(exarg_T *eap)
{
    முழு		n;
    win_T	*wp = curwin;

    எனில் (eap->addr_மொத்தம் > 0)
    {
	n = eap->line2;
	க்கு (wp = firstwin; wp->w_next != வெற்று && --n > 0; wp = wp->w_next)
	    ;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    n = atol((எழுத்து *)eap->arg);
    எனில் (cmdmod.cmod_split & WSP_VERT)
    {
	எனில் (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_width;
	இல்லையெனில் எனில் (n == 0 && eap->arg[0] == NUL)	// default is very wide
	    n = 9999;
	win_setwidth_win(n, wp);
    }
    இல்லையெனில்
    {
	எனில் (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_height;
	இல்லையெனில் எனில் (n == 0 && eap->arg[0] == NUL)	// default is very high
	    n = 9999;
	win_setheight_win(n, wp);
    }
}

/*
 * ":find [+command] <file>" command.
 */
    நிலையான வெறுமை
ex_find(exarg_T *eap)
{
    எனில் (!check_can_set_curbuf_forceit(eap->forceit))
	திரும்பு;

    char_u	*fname = வெற்று;
    முழு		மொத்தம்;
    char_u	*file_to_find = வெற்று;
    எழுத்து	*தேடல்_ctx = வெற்று;

    எனில் (*get_findfunc() != NUL)
    {
#ifdef FEAT_EVAL
	fname = findfunc_find_file(eap->arg, (முழு)STRLEN(eap->arg),
					eap->addr_மொத்தம் > 0 ? eap->line2 : 1);
#endif
    }
    இல்லையெனில்
    {
	fname = find_file_in_path(eap->arg, (முழு)STRLEN(eap->arg), FNAME_MESS,
			       TRUE, curbuf->b_ffname, &file_to_find, &தேடல்_ctx);
	எனில் (eap->addr_மொத்தம் > 0)
	{
	    // Repeat finding the file "count" times.  This matters when it appears
	    // several times in the path.
	    மொத்தம் = eap->line2;
	    போது (fname != வெற்று && --மொத்தம் > 0)
	    {
		vim_free(fname);
		fname = find_file_in_path(வெற்று, 0, FNAME_MESS,
			      FALSE, curbuf->b_ffname, &file_to_find, &தேடல்_ctx);
	    }
	}
	VIM_CLEAR(file_to_find);
	vim_findfile_cleanup(தேடல்_ctx);
    }

    எனில் (fname == வெற்று)
	திரும்பு;

    eap->arg = fname;
    do_exedit(eap, வெற்று);
    vim_free(fname);
}

/*
 * ":open" simulation: for now works just like ":visual".
 */
    நிலையான வெறுமை
ex_open(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*p;

#ifdef FEAT_EVAL
    எனில் (not_in_vim9(eap, false) == FAIL)
	திரும்பு;
#endif
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_SOL | BL_FIX);
    எனில் (*eap->arg == '/')
    {
	// ":open /pattern/": put cursor in column found with pattern
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg, magic_isset() ? RE_MAGIC : 0);
	எனில் (regmatch.regprog != வெற்று)
	{
	    // make a copy of the line, when searching for a mark it might be
	    // flushed
	    char_u *வரி = vim_strsave(ml_get_curline());

	    regmatch.rm_ic = p_ic;
	    எனில் (vim_regexec(&regmatch, வரி, (colnr_T)0))
		curwin->w_cursor.col = (colnr_T)(regmatch.startp[0] - வரி);
	    இல்லையெனில்
		emsg(_(e_no_match));
	    vim_regfree(regmatch.regprog);
	    vim_free(வரி);
	}
	// Move to the NUL, ignore any other arguments.
	eap->arg += STRLEN(eap->arg);
    }
    check_cursor();

    eap->cmdidx = CMD_visual;
    do_exedit(eap, வெற்று);
}

/*
 * ":edit", ":badd", ":balt", ":visual".
 */
    நிலையான வெறுமை
ex_edit(exarg_T *eap)
{
    char_u *ffname = eap->cmdidx == CMD_enew ? வெற்று : eap->arg;

    // Exclude commands which keep the window's current buffer
    எனில் (
	    eap->cmdidx != CMD_badd
	    && eap->cmdidx != CMD_balt
	    // All other commands must obey 'winfixbuf' / ! rules
	    && (is_other_file(0, ffname) && !check_can_set_curbuf_forceit(eap->forceit))
    )
	திரும்பு;

    do_exedit(eap, வெற்று);
}

/*
 * ":edit <file>" command and alike.
 */
    வெறுமை
do_exedit(
    exarg_T	*eap,
    win_T	*old_curwin)	    // curwin before doing a split or NULL
{
    முழு		n;
    முழு		need_hide;
    முழு		exmode_was = exmode_active;

    எனில் ((eap->cmdidx != CMD_pedit && ERROR_IF_POPUP_WINDOW)
						 || ERROR_IF_TERM_POPUP_WINDOW)
	திரும்பு;
    /*
     * ":vi" command ends Ex mode.
     */
    எனில் (exmode_active && (eap->cmdidx == CMD_visual
						|| eap->cmdidx == CMD_view))
    {
	exmode_active = FALSE;
	ex_pressedreturn = FALSE;
	எனில் (*eap->arg == NUL)
	{
	    // Special case:  ":global/pat/visual\NLvi-commands"
	    எனில் (global_busy)
	    {
		எனில் (eap->nextcmd != வெற்று)
		{
		    stuffReadbuff(eap->nextcmd);
		    eap->nextcmd = வெற்று;
		}

		எனில் (exmode_was != EXMODE_VIM)
		    settmode(TMODE_RAW);
		முழு save_RedrawingDisabled = RedrawingDisabled;
		RedrawingDisabled = 0;
		முழு save_nwr = no_wait_return;
		no_wait_return = 0;
		need_wait_return = FALSE;
		முழு save_ms = செய்தி_scroll;
		செய்தி_scroll = 0;
#ifdef FEAT_GUI
		முழு save_he = hold_gui_events;
		hold_gui_events = 0;
#endif
		set_must_redraw(UPD_CLEAR);
		pending_exmode_active = TRUE;

		main_loop(FALSE, TRUE);

		pending_exmode_active = FALSE;
		RedrawingDisabled = save_RedrawingDisabled;
		no_wait_return = save_nwr;
		செய்தி_scroll = save_ms;
#ifdef FEAT_GUI
		hold_gui_events = save_he;
#endif
	    }
	    திரும்பு;
	}
    }

    எனில் ((eap->cmdidx == CMD_new
		|| eap->cmdidx == CMD_tabnew
		|| eap->cmdidx == CMD_tabedit
		|| eap->cmdidx == CMD_vnew) && *eap->arg == NUL)
    {
	// ":new" or ":tabnew" without argument: edit a new empty buffer
	setpcmark();
	(வெறுமை)do_ecmd(0, வெற்று, வெற்று, eap, ECMD_ONE,
		      ECMD_HIDE + (eap->forceit ? ECMD_FORCEIT : 0),
		      old_curwin == வெற்று ? curwin : வெற்று);
    }
    இல்லையெனில் எனில் ((eap->cmdidx != CMD_split && eap->cmdidx != CMD_vsplit)
	    || *eap->arg != NUL
#ifdef FEAT_BROWSE
	    || (cmdmod.cmod_flags & CMOD_BROWSE)
#endif
	    )
    {
	// Can't edit another file when "textlock" or "curbuf_lock" is set.
	// Only ":edit" or ":script" can bring us here, others are stopped
	// earlier.
	எனில் (*eap->arg != NUL && text_or_buf_locked())
	    திரும்பு;

	n = readonlymode;
	எனில் (eap->cmdidx == CMD_view || eap->cmdidx == CMD_sview)
	    readonlymode = TRUE;
	இல்லையெனில் எனில் (eap->cmdidx == CMD_enew)
	    readonlymode = FALSE;   // 'readonly' doesn't make sense in an
				    // empty buffer
	எனில் (eap->cmdidx != CMD_balt && eap->cmdidx != CMD_badd)
	    setpcmark();
	எனில் (do_ecmd(0, (eap->cmdidx == CMD_enew ? வெற்று : eap->arg),
		    வெற்று, eap,
		    // ":edit" goes to first line if Vi compatible
		    (*eap->arg == NUL && eap->do_ecmd_lnum == 0
				      && vim_strchr(p_cpo, CPO_GOTO1) != வெற்று)
					       ? ECMD_ONE : eap->do_ecmd_lnum,
		    (buf_hide(curbuf) ? ECMD_HIDE : 0)
		    + (eap->forceit ? ECMD_FORCEIT : 0)
		      // after a split we can use an existing buffer
		    + (old_curwin != வெற்று ? ECMD_OLDBUF : 0)
		    + (eap->cmdidx == CMD_badd ? ECMD_ADDBUF : 0)
		    + (eap->cmdidx == CMD_balt ? ECMD_ALTBUF : 0)
		    , old_curwin == வெற்று ? curwin : வெற்று) == FAIL)
	{
	    // Editing the file failed.  If the window was split, close it.
	    எனில் (old_curwin != வெற்று)
	    {
		need_hide = (curbufIsChanged() && curbuf->b_nwindows <= 1);
		எனில் (!need_hide || buf_hide(curbuf))
		{
#எனில் defined(FEAT_EVAL)
		    cleanup_T   cs;

		    // Reset the error/interrupt/exception state here so that
		    // aborting() returns FALSE when closing a window.
		    enter_cleanup(&cs);
#endif
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_close(curwin, !need_hide && !buf_hide(curbuf));

#எனில் defined(FEAT_EVAL)
		    // Restore the error/interrupt/exception state if not
		    // discarded by a new aborting error, interrupt, or
		    // uncaught exception.
		    leave_cleanup(&cs);
#endif
		}
	    }
	}
	இல்லையெனில் எனில் (readonlymode && curbuf->b_nwindows == 1)
	{
	    // When editing an already visited buffer, 'readonly' won't be set
	    // but the previous value is kept.  With ":view" and ":sview" we
	    // want the  file to be readonly, except when another window is
	    // editing the same buffer.
	    curbuf->b_p_ro = TRUE;
	}
	readonlymode = n;
    }
    இல்லையெனில்
    {
	எனில் (eap->do_ecmd_cmd != வெற்று)
	    do_cmd_argument(eap->do_ecmd_cmd);
	n = curwin->w_arg_idx_invalid;
	check_arg_idx(curwin);
	எனில் (n != curwin->w_arg_idx_invalid)
	    maketitle();
    }

    /*
     * if ":split file" worked, set alternate file name in old window to new
     * file
     */
    எனில் (old_curwin != வெற்று
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
    நிலையான வெறுமை
ex_nogui(exarg_T *eap)
{
    eap->errmsg = _(e_gui_cannot_be_used_not_enabled_at_compile_time);
}
#endif

#எனில் defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
    நிலையான வெறுமை
ex_tearoff(exarg_T *eap)
{
    gui_make_tearoff(eap->arg);
}
#endif

#எனில் (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
    நிலையான வெறுமை
ex_popup(exarg_T *eap)
{
# எனில் defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    எனில் (gui.in_use)
	gui_make_popup(eap->arg, eap->forceit);
#  ifdef FEAT_TERM_POPUP_MENU
    இல்லையெனில்
#  endif
# endif
# ifdef FEAT_TERM_POPUP_MENU
	pum_make_popup(eap->arg, eap->forceit);
# endif
}
#endif

    நிலையான வெறுமை
ex_swapname(exarg_T *eap UNUSED)
{
    எனில் (curbuf->b_ml.ml_mfp == வெற்று || curbuf->b_ml.ml_mfp->mf_fname == வெற்று)
	செய்தி(_("No swap file"));
    இல்லையெனில்
	செய்தி((எழுத்து *)curbuf->b_ml.ml_mfp->mf_fname);
}

/*
 * ":syncbind" forces all 'scrollbind' windows to have the same relative
 * offset.
 * (1998-11-02 16:21:01  R. Edward Ralston <eralston@computer.org>)
 */
    நிலையான வெறுமை
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
    எனில் (curwin->w_p_scb)
    {
	topline = curwin->w_topline;
	FOR_ALL_WINDOWS(wp)
	{
	    எனில் (wp->w_p_scb && wp->w_buffer)
	    {
		y = wp->w_buffer->b_ml.ml_வரி_மொத்தம் - get_scrolloff_மதிப்பு();
		எனில் (topline > y)
		    topline = y;
	    }
	}
	எனில் (topline < 1)
	    topline = 1;
    }
    இல்லையெனில்
    {
	topline = 1;
    }


    /*
     * Set all scrollbind windows to the same topline.
     */
    FOR_ALL_WINDOWS(curwin)
    {
	எனில் (curwin->w_p_scb)
	{
	    curbuf = curwin->w_buffer;
	    y = topline - curwin->w_topline;
	    எனில் (y > 0)
		scrollup(y, TRUE);
	    இல்லையெனில்
		scrolldown(-y, TRUE);
	    curwin->w_scbind_pos = topline;
	    redraw_later(UPD_VALID);
	    cursor_correct();
	    curwin->w_redr_status = true;
	}
    }
    curwin = save_curwin;
    curbuf = save_curbuf;
    எனில் (curwin->w_p_scb)
    {
	did_syncbind = TRUE;
	checkpcmark();
	எனில் (old_linenr != curwin->w_cursor.lnum)
	{
	    char_u ctrl_o[2];

	    ctrl_o[0] = Ctrl_O;
	    ctrl_o[1] = 0;
	    ins_typebuf(ctrl_o, REMAP_NONE, 0, TRUE, FALSE);
	}
    }
}


    நிலையான வெறுமை
ex_read(exarg_T *eap)
{
    முழு		i;
    முழு		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum;

    எனில் (eap->usefilter)			// :r!cmd
    {
	do_bang(1, eap, FALSE, FALSE, TRUE);
	திரும்பு;
    }

    எனில் (u_save(eap->line2, (linenr_T)(eap->line2 + 1)) == FAIL)
	திரும்பு;

#ifdef FEAT_BROWSE
    எனில் (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browseFile;

	browseFile = do_browse(0, (char_u *)_("Append File"), eap->arg,
		வெற்று, வெற்று, வெற்று, curbuf);
	எனில் (browseFile != வெற்று)
	{
	    i = readfile(browseFile, வெற்று,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	    vim_free(browseFile);
	}
	இல்லையெனில்
	    i = OK;
    }
    இல்லையெனில்
#endif
	எனில் (*eap->arg == NUL)
	{
	    எனில் (check_fname() == FAIL)	// check for no file name
		திரும்பு;
	    i = readfile(curbuf->b_ffname, curbuf->b_fname,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	}
	இல்லையெனில்
	{
	    எனில் (vim_strchr(p_cpo, CPO_ALTREAD) != வெற்று)
		(வெறுமை)setaltfname(eap->arg, eap->arg, (linenr_T)1);
	    i = readfile(eap->arg, வெற்று,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);

	}
    எனில் (i != OK)
    {
#எனில் defined(FEAT_EVAL)
	எனில் (!aborting())
#endif
	    semsg(_(e_cant_open_file_str), eap->arg);
    }
    இல்லையெனில்
    {
	எனில் (empty && exmode_active)
	{
	    // Delete the empty line that remains.  Historically ex does
	    // this but vi doesn't.
	    எனில் (eap->line2 == 0)
		lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
	    இல்லையெனில்
		lnum = 1;
	    எனில் (*ml_get(lnum) == NUL && u_savedel(lnum, 1L) == OK)
	    {
		ml_delete(lnum);
		எனில் (curwin->w_cursor.lnum > 1
			&& curwin->w_cursor.lnum >= lnum)
		    --curwin->w_cursor.lnum;
		deleted_வரிகள்_mark(lnum, 1L);
	    }
	}
	redraw_curbuf_later(UPD_VALID);
    }
}

நிலையான char_u	*prev_dir = வெற்று;

#எனில் defined(EXITFREE)
    வெறுமை
free_cd_dir(வெறுமை)
{
    VIM_CLEAR(prev_dir);
    VIM_CLEAR(globaldir);
}
#endif

/*
 * Get the previous directory for the given chdir scope.
 */
    நிலையான char_u *
get_prevdir(cdscope_T scope)
{
    எனில் (scope == CDSCOPE_WINDOW)
	திரும்பு curwin->w_prevdir;
    இல்லையெனில் எனில் (scope == CDSCOPE_TABPAGE)
	திரும்பு curtab->tp_prevdir;
    திரும்பு prev_dir;
}

/*
 * Deal with the side effects of changing the current directory.
 * When 'scope' is CDSCOPE_TABPAGE then this was after an ":tcd" command.
 * When 'scope' is CDSCOPE_WINDOW then this was after an ":lcd" command.
 */
    வெறுமை
post_chdir(cdscope_T scope)
{
    எனில் (scope != CDSCOPE_WINDOW)
	// Clear tab local directory for both :cd and :tcd
	VIM_CLEAR(curtab->tp_localdir);
    VIM_CLEAR(curwin->w_localdir);
    எனில் (scope != CDSCOPE_GLOBAL)
    {
	char_u	*pdir = get_prevdir(scope);

	// If still in the global directory, need to remember current
	// directory as the global directory.
	எனில் (globaldir == வெற்று && pdir != வெற்று)
	    globaldir = vim_strsave(pdir);

	// Remember this local directory for the window.
	எனில் (mch_dirname(NameBuff, MAXPATHL) == OK)
	{
	    எனில் (scope == CDSCOPE_TABPAGE)
		curtab->tp_localdir = vim_strsave(NameBuff);
	    இல்லையெனில்
		curwin->w_localdir = vim_strsave(NameBuff);
	}
    }
    இல்லையெனில்
    {
	// We are now in the global directory, no need to remember its name.
	VIM_CLEAR(globaldir);
    }

    last_chdir_reason = வெற்று;
    shorten_fnames(vim_strchr(p_cpo, CPO_NOSYMLINKS) == வெற்று);
}

/*
 * Trigger DirChangedPre for "acmd_fname" with directory "new_dir".
 */
    வெறுமை
trigger_DirChangedPre(char_u *acmd_fname, char_u *new_dir)
{
#ifdef FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;

    v_event = get_v_event(&save_v_event);
    (வெறுமை)dict_add_string(v_event, "directory", new_dir);
    dict_set_பொருட்கள்_ro(v_event);
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
    முழு
changedir_func(
	char_u		*new_dir,
	முழு		forceit,
	cdscope_T	scope)
{
    char_u	*pdir = வெற்று;
    முழு		dir_differs;
    char_u	*acmd_fname = வெற்று;
    char_u	**pp;
    char_u	*tofree;

    எனில் (new_dir == வெற்று || allbuf_locked())
	திரும்பு FALSE;

    எனில் (vim_strchr(p_cpo, CPO_CHDIR) != வெற்று && curbufIsChanged() && !forceit)
    {
	emsg(_(e_cannot_change_directory_buffer_is_modified_add_bang_to_override));
	திரும்பு FALSE;
    }

    // ":cd -": Change to previous directory
    எனில் (STRCMP(new_dir, "-") == 0)
    {
	pdir = get_prevdir(scope);
	எனில் (pdir == வெற்று)
	{
	    emsg(_(e_no_previous_directory));
	    திரும்பு FALSE;
	}
	new_dir = pdir;
    }

    // Save current directory for next ":cd -"
    எனில் (mch_dirname(NameBuff, MAXPATHL) == OK)
	pdir = vim_strsave(NameBuff);
    இல்லையெனில்
	pdir = வெற்று;

    // For UNIX ":cd" means: go to home directory.
    // On other systems too if 'cdhome' is set.
#எனில் defined(UNIX) || defined(VMS)
    எனில் (*new_dir == NUL)
#இல்லையெனில்
    எனில் (*new_dir == NUL && p_cdh)
#endif
    {
	// use NameBuff for home directory name
#ifdef VMS
	char_u	*p;

	p = mch_getenv((char_u *)"SYS$LOGIN");
	எனில் (p == வெற்று || *p == NUL)	// empty is the same as not set
	    NameBuff[0] = NUL;
	இல்லையெனில்
	    vim_strncpy(NameBuff, p, MAXPATHL - 1);
#இல்லையெனில்
	expand_env((char_u *)"$HOME", NameBuff, MAXPATHL);
#endif
	new_dir = NameBuff;
    }
    dir_differs = pdir == வெற்று
			    || pathcmp((எழுத்து *)pdir, (எழுத்து *)new_dir, -1) != 0;
    எனில் (dir_differs)
    {
	எனில் (scope == CDSCOPE_WINDOW)
	    acmd_fname = (char_u *)"window";
	இல்லையெனில் எனில் (scope == CDSCOPE_TABPAGE)
	    acmd_fname = (char_u *)"tabpage";
	இல்லையெனில்
	    acmd_fname = (char_u *)"global";
	trigger_DirChangedPre(acmd_fname, new_dir);

	எனில் (vim_chdir(new_dir))
	{
	    emsg(_(e_command_failed));
	    vim_free(pdir);
	    திரும்பு FALSE;
	}
    }

    எனில் (scope == CDSCOPE_WINDOW)
	pp = &curwin->w_prevdir;
    இல்லையெனில் எனில் (scope == CDSCOPE_TABPAGE)
	pp = &curtab->tp_prevdir;
    இல்லையெனில்
	pp = &prev_dir;
    tofree = *pp;  // new_dir may use this
    *pp = pdir;

    post_chdir(scope);

    எனில் (dir_differs)
	apply_autocmds(EVENT_DIRCHANGED, acmd_fname, new_dir, FALSE, curbuf);
    vim_free(tofree);
    திரும்பு TRUE;
}

/*
 * ":cd", ":tcd", ":lcd", ":chdir" ":tchdir" and ":lchdir".
 */
    வெறுமை
ex_cd(exarg_T *eap)
{
    char_u	*new_dir;

    new_dir = eap->arg;
#எனில் !defined(UNIX) && !defined(VMS)
    // for non-UNIX ":cd" means: print current directory unless 'cdhome' is set
    எனில் (*new_dir == NUL && !p_cdh)
    {
	ex_pwd(வெற்று);
	திரும்பு;
    }
#endif

    cdscope_T	scope = CDSCOPE_GLOBAL;

    எனில் (eap->cmdidx == CMD_lcd || eap->cmdidx == CMD_lchdir)
	scope = CDSCOPE_WINDOW;
    இல்லையெனில் எனில் (eap->cmdidx == CMD_tcd || eap->cmdidx == CMD_tchdir)
	scope = CDSCOPE_TABPAGE;

    எனில் (changedir_func(new_dir, eap->forceit, scope))
    {
	// Echo the new current directory if the command was typed.
	எனில் (சாவி_Typed || p_verbose >= 5)
	    ex_pwd(eap);
    }
}

/*
 * ":pwd".
 */
    நிலையான வெறுமை
ex_pwd(exarg_T *eap UNUSED)
{
    எனில் (mch_dirname(NameBuff, MAXPATHL) == OK)
    {
#ifdef BACKSLASH_IN_FILENAME
	slash_adjust(NameBuff);
#endif
	எனில் (p_verbose > 0)
	{
	    எழுத்து *context = "global";

	    எனில் (last_chdir_reason != வெற்று)
		context = last_chdir_reason;
	    இல்லையெனில் எனில் (curwin->w_localdir != வெற்று)
		context = "window";
	    இல்லையெனில் எனில் (curtab->tp_localdir != வெற்று)
		context = "tabpage";
	    smsg("[%s] %s", context, (எழுத்து *)NameBuff);
	}
	இல்லையெனில்
	    செய்தி((எழுத்து *)NameBuff);
    }
    இல்லையெனில்
	emsg(_(e_directory_unknown));
}

/*
 * ":=".
 */
    நிலையான வெறுமை
ex_equal(exarg_T *eap)
{
    smsg("%ld", (long)eap->line2);
    ex_may_print(eap);
}

    நிலையான வெறுமை
ex_sleep(exarg_T *eap)
{
    முழு		n;
    long	len;

    எனில் (cursor_valid())
    {
	n = W_WINROW(curwin) + curwin->w_wrow - செய்தி_scrolled;
	எனில் (n >= 0)
	    windgoto(n, curwin->w_wincol + curwin->w_wcol);
    }

    len = eap->line2;
    தேர்வு (*eap->arg)
    {
	நிலை 'm': நிறுத்து;
	நிலை NUL: len *= 1000L; நிறுத்து;
	இயல்பு: semsg(_(e_invalid_argument_str), eap->arg); திரும்பு;
    }

    // Hide the cursor if invoked with !
    do_sleep(len, eap->forceit);
}

/*
 * Sleep for "msec" milliseconds, but keep checking for a CTRL-C every second.
 * Hide the cursor if "hide_cursor" is TRUE.
 */
    வெறுமை
do_sleep(long msec, முழு hide_cursor)
{
    long	done = 0;
    long	wait_now;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    // Remember at what time we started, so that we know how much longer we
    // should wait after waiting for a bit.
    ELAPSED_INIT(start_tv);
#endif

    எனில் (hide_cursor)
	cursor_sleep();
    இல்லையெனில்
	cursor_on();

    out_flush_cursor(FALSE, FALSE);
    போது (!got_int && done < msec)
    {
	wait_now = msec - done > 1000L ? 1000L : msec - done;
#ifdef FEAT_TIMERS
	{
	    long    due_time = check_due_timer();

	    எனில் (due_time > 0 && due_time < wait_now)
		wait_now = due_time;
	}
#endif
#ifdef FEAT_JOB_CHANNEL
	எனில் (has_any_channel() && wait_now > 20L)
	    wait_now = 20L;
#endif
#ifdef FEAT_SOUND
	எனில் (has_any_sound_callback() && wait_now > 20L)
	    wait_now = 20L;
#endif
	ui_delay(wait_now, TRUE);

#ifdef FEAT_JOB_CHANNEL
	எனில் (has_any_channel())
	    ui_breakcheck_force(TRUE);
	இல்லையெனில்
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
#இல்லையெனில்
	// guesstimate time passed (will actually be more)
	done += wait_now;
#endif
    }

    // If CTRL-C was typed to interrupt the sleep, drop the CTRL-C from the
    // input buffer, otherwise a following call to input() fails.
    எனில் (got_int)
	(வெறுமை)vpeekc();

    எனில் (hide_cursor)
	cursor_unsleep();
}

/*
 * ":winsize" command (obsolete).
 */
    நிலையான வெறுமை
ex_winsize(exarg_T *eap)
{
    முழு		w, h;
    char_u	*arg = eap->arg;
    char_u	*p;

    எனில் (!SAFE_isdigit(*arg))
    {
	semsg(_(e_invalid_argument_str), arg);
	திரும்பு;
    }
    w = getdigits(&arg);
    arg = skipwhite(arg);
    p = arg;
    h = getdigits(&arg);
    எனில் (*p != NUL && *arg == NUL)
	set_shellsize(w, h, TRUE);
    இல்லையெனில்
	emsg(_(e_winsize_requires_two_எண்_arguments));
}

    நிலையான வெறுமை
ex_wincmd(exarg_T *eap)
{
    முழு		xchar = NUL;
    char_u	*p;

    எனில் (*eap->arg == 'g' || *eap->arg == Ctrl_G)
    {
	// CTRL-W g and CTRL-W CTRL-G  have an extra command character
	எனில் (eap->arg[1] == NUL)
	{
	    emsg(_(e_invalid_argument));
	    திரும்பு;
	}
	xchar = eap->arg[1];
	p = eap->arg + 2;
    }
    இல்லையெனில்
	p = eap->arg + 1;

    set_nextcmd(eap, p);
    p = skipwhite(p);
    எனில் (*p != NUL && *p != (
#ifdef FEAT_EVAL
	    in_vim9script() ? '#' :
#endif
		'"')
	    && eap->nextcmd == வெற்று)
	emsg(_(e_invalid_argument));
    இல்லையெனில் எனில் (!eap->skip)
    {
	// Pass flags on for ":vertical wincmd ]".
	postponed_split_flags = cmdmod.cmod_split;
	postponed_split_tab = cmdmod.cmod_tab;
	do_window(*eap->arg, eap->addr_மொத்தம் > 0 ? eap->line2 : 0L, xchar);
	postponed_split_flags = 0;
	postponed_split_tab = 0;
    }
}

#எனில் defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * ":winpos".
 */
    நிலையான வெறுமை
ex_winpos(exarg_T *eap)
{
    முழு		x, y;
    char_u	*arg = eap->arg;
    char_u	*p;

    எனில் (*arg == NUL)
    {
# எனில் defined(FEAT_GUI) || defined(MSWIN)
#  ifdef VIMDLL
	எனில் (gui.in_use ? gui_mch_get_winpos(&x, &y) != FAIL :
		mch_get_winpos(&x, &y) != FAIL)
#  elif defined(FEAT_GUI)
	எனில் (gui.in_use && gui_mch_get_winpos(&x, &y) != FAIL)
#  இல்லையெனில்
	எனில் (mch_get_winpos(&x, &y) != FAIL)
#  endif
	{
	    sprintf((எழுத்து *)IObuff, _("Window position: X %d, Y %d"), x, y);
	    செய்தி((எழுத்து *)IObuff);
	}
	இல்லையெனில்
# endif
	    emsg(_(e_obtaining_window_position_not_implemented_for_this_platform));
    }
    இல்லையெனில்
    {
	x = getdigits(&arg);
	arg = skipwhite(arg);
	p = arg;
	y = getdigits(&arg);
	எனில் (*p == NUL || *arg != NUL)
	{
	    emsg(_(e_winpos_requires_two_எண்_arguments));
	    திரும்பு;
	}
# ifdef FEAT_GUI
	எனில் (gui.in_use)
	    gui_mch_set_winpos(x, y);
	இல்லையெனில் எனில் (gui.starting)
	{
	    // Remember the coordinates for when the window is opened.
	    gui_win_x = x;
	    gui_win_y = y;
	}
#  எனில் defined(HAVE_TGETENT) || defined(VIMDLL)
	இல்லையெனில்
#  endif
# endif
# எனில் defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	    mch_set_winpos(x, y);
# endif
# ifdef HAVE_TGETENT
	எனில் (*T_CWP)
	    term_set_winpos(x, y);
# endif
    }
}
#endif

/*
 * Handle command that work like operators: ":delete", ":yank", ":>" and ":<".
 */
    நிலையான வெறுமை
ex_operators(exarg_T *eap)
{
    oparg_T	oa;

    clear_oparg(&oa);
    oa.regname = eap->regname;
    oa.start.lnum = eap->line1;
    oa.end.lnum = eap->line2;
    oa.வரி_மொத்தம் = eap->line2 - eap->line1 + 1;
    oa.motion_type = MLINE;
    virtual_op = FALSE;
    எனில் (eap->cmdidx != CMD_yank)	// position cursor for undo
    {
	setpcmark();
	curwin->w_cursor.lnum = eap->line1;
	beginline(BL_SOL | BL_FIX);
    }

    எனில் (VIsual_active)
	end_visual_mode();

    தேர்வு (eap->cmdidx)
    {
	நிலை CMD_delete:
	    oa.op_type = OP_DELETE;
	    op_delete(&oa);
	    நிறுத்து;

	நிலை CMD_yank:
	    oa.op_type = OP_YANK;
	    (வெறுமை)op_yank(&oa, FALSE, TRUE);
	    நிறுத்து;

	இயல்பு:    // CMD_rshift or CMD_lshift
	    எனில் (
#ifdef FEAT_RIGHTLEFT
		(eap->cmdidx == CMD_rshift) ^ curwin->w_p_rl
#இல்லையெனில்
		eap->cmdidx == CMD_rshift
#endif
						)
		oa.op_type = OP_RSHIFT;
	    இல்லையெனில்
		oa.op_type = OP_LSHIFT;
	    op_shift(&oa, FALSE, eap->amount);
	    நிறுத்து;
    }
    virtual_op = MAYBE;
    ex_may_print(eap);
}

/*
 * ":put".
 */
    நிலையான வெறுமை
ex_put(exarg_T *eap)
{
    // ":0put" works like ":1put!".
    எனில் (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, வெற்று, eap->forceit ? BACKWARD : FORWARD, 1L,
						       PUT_வரி|PUT_CURSLINE);
}

/*
 * ":iput".
 */
    நிலையான வெறுமை
ex_iput(exarg_T *eap)
{
    // ":0iput" works like ":1iput!".
    எனில் (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, வெற்று, eap->forceit ? BACKWARD : FORWARD, 1L,
						      PUT_வரி|PUT_CURSLINE
						      |PUT_FIXINDENT);
}

/*
 * Handle ":copy" and ":move".
 */
    நிலையான வெறுமை
ex_copymove(exarg_T *eap)
{
    long	n;

#ifdef FEAT_EVAL
    எனில் (not_in_vim9(eap, false) == FAIL)
	திரும்பு;
#endif
    n = get_address(eap, &eap->arg, eap->addr_type, FALSE, FALSE, FALSE, 1);
    எனில் (eap->arg == வெற்று)	    // error detected
    {
	eap->nextcmd = வெற்று;
	திரும்பு;
    }
    get_flags(eap);

    /*
     * move or copy lines from 'eap->line1'-'eap->line2' to below line 'n'
     */
    எனில் (n == MAXLNUM || n < 0 || n > curbuf->b_ml.ml_வரி_மொத்தம்)
    {
	emsg(_(e_invalid_range));
	திரும்பு;
    }

    எனில் (eap->cmdidx == CMD_move)
    {
	எனில் (do_move(eap->line1, eap->line2, n) == FAIL)
	    திரும்பு;
    }
    இல்லையெனில்
	ex_copy(eap->line1, eap->line2, n);
    u_clearline();
    beginline(BL_SOL | BL_FIX);
    ex_may_print(eap);
}

/*
 * Print the current line if flags were given to the Ex command.
 */
    வெறுமை
ex_may_print(exarg_T *eap)
{
    எனில் (eap->flags != 0)
    {
	print_வரி(curwin->w_cursor.lnum, (eap->flags & EXFLAG_NR),
						  (eap->flags & EXFLAG_LIST));
	ex_no_reprint = TRUE;
    }
}

/*
 * ":smagic" and ":snomagic".
 */
    நிலையான வெறுமை
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
    நிலையான வெறுமை
ex_join(exarg_T *eap)
{
    curwin->w_cursor.lnum = eap->line1;
    எனில் (eap->line1 == eap->line2)
    {
	எனில் (eap->addr_மொத்தம் >= 2)   // :2,2join does nothing
	    திரும்பு;
	எனில் (eap->line2 == curbuf->b_ml.ml_வரி_மொத்தம்)
	{
	    beep_flush();
	    திரும்பு;
	}
	++eap->line2;
    }
    (வெறுமை)do_join(eap->line2 - eap->line1 + 1, !eap->forceit, TRUE, TRUE, TRUE);
    beginline(BL_WHITE | BL_FIX);
    ex_may_print(eap);
}

/*
 * ":[addr]@r" or ":[addr]*r": execute register
 */
    நிலையான வெறுமை
ex_at(exarg_T *eap)
{
    முழு		c;
    முழு		prev_len = typebuf.tb_len;

    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif

    // get the register name.  No name means to use the previous one
    c = *eap->arg;
    எனில் (c == NUL || (c == '*' && *eap->cmd == '*'))
	c = '@';
    // Put the register in the typeahead buffer with the "silent" flag.
    எனில் (do_execreg(c, TRUE, vim_strchr(p_cpo, CPO_EXECBUF) != வெற்று, TRUE)
								      == FAIL)
    {
	beep_flush();
	திரும்பு;
    }

    முழு	save_efr = exec_from_reg;

    exec_from_reg = TRUE;

    /*
     * Execute from the typeahead buffer.
     * Continue until the stuff buffer is empty and all added characters
     * have been consumed.
     */
    போது (!stuff_empty() || typebuf.tb_len > prev_len)
	(வெறுமை)do_cmdline(வெற்று, getexline, வெற்று, DOCMD_NOWAIT|DOCMD_VERBOSE);

    exec_from_reg = save_efr;
}

/*
 * ":!".
 */
    நிலையான வெறுமை
ex_bang(exarg_T *eap)
{
    do_bang(eap->addr_மொத்தம், eap, eap->forceit, TRUE, TRUE);
}

/*
 * ":undo".
 */
    நிலையான வெறுமை
ex_undo(exarg_T *eap)
{
    எனில் (eap->addr_மொத்தம் == 1)	    // :undo 123
	undo_time(eap->line2, FALSE, FALSE, TRUE);
    இல்லையெனில்
	u_undo(1);
}

#ifdef FEAT_PERSISTENT_UNDO
    நிலையான வெறுமை
ex_wundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_SIZE];

    u_compute_hash(hash);
    u_write_undo(eap->arg, eap->forceit, curbuf, hash);
}

    நிலையான வெறுமை
ex_rundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_SIZE];

    u_compute_hash(hash);
    u_read_undo(eap->arg, hash, வெற்று);
}
#endif

/*
 * ":redo".
 */
    நிலையான வெறுமை
ex_redo(exarg_T *eap UNUSED)
{
    u_redo(1);
}

/*
 * ":earlier" and ":later".
 */
    நிலையான வெறுமை
ex_later(exarg_T *eap)
{
    long	மொத்தம் = 0;
    முழு		sec = FALSE;
    முழு		file = FALSE;
    char_u	*p = eap->arg;

    எனில் (*p == NUL)
	மொத்தம் = 1;
    இல்லையெனில் எனில் (SAFE_isdigit(*p))
    {
	மொத்தம் = getdigits(&p);
	தேர்வு (*p)
	{
	    நிலை 's': ++p; sec = TRUE; நிறுத்து;
	    நிலை 'm': ++p; sec = TRUE; மொத்தம் *= 60; நிறுத்து;
	    நிலை 'h': ++p; sec = TRUE; மொத்தம் *= 60 * 60; நிறுத்து;
	    நிலை 'd': ++p; sec = TRUE; மொத்தம் *= 24 * 60 * 60; நிறுத்து;
	    நிலை 'f': ++p; file = TRUE; நிறுத்து;
	}
    }

    எனில் (*p != NUL)
	semsg(_(e_invalid_argument_str), eap->arg);
    இல்லையெனில்
	undo_time(eap->cmdidx == CMD_earlier ? -மொத்தம் : மொத்தம்,
							    sec, file, FALSE);
}

/*
 * ":redir": start/stop redirection.
 */
    நிலையான வெறுமை
ex_redir(exarg_T *eap)
{
    எழுத்து	*mode;
    char_u	*fname;
    char_u	*arg = eap->arg;

#ifdef FEAT_EVAL
    எனில் (redir_execute)
    {
	emsg(_(e_cannot_use_redir_inside_execute));
	திரும்பு;
    }
#endif

    எனில் (STRICMP(eap->arg, "END") == 0)
	close_redir();
    இல்லையெனில்
    {
	எனில் (*arg == '>')
	{
	    ++arg;
	    எனில் (*arg == '>')
	    {
		++arg;
		mode = "a";
	    }
	    இல்லையெனில்
		mode = "w";
	    arg = skipwhite(arg);

	    close_redir();

	    // Expand environment variables and "~/".
	    fname = expand_env_save(arg);
	    எனில் (fname == வெற்று)
		திரும்பு;
#ifdef FEAT_BROWSE
	    எனில் (cmdmod.cmod_flags & CMOD_BROWSE)
	    {
		char_u	*browseFile;

		browseFile = do_browse(BROWSE_SAVE,
			(char_u *)_("Save Redirection"),
			fname, வெற்று, வெற்று,
			(char_u *)_(BROWSE_FILTER_ALL_FILES), curbuf);
		எனில் (browseFile == வெற்று)
		    திரும்பு;		// operation cancelled
		vim_free(fname);
		fname = browseFile;
		eap->forceit = TRUE;	// since dialog already asked
	    }
#endif

	    redir_fd = open_exfile(fname, eap->forceit, mode);
	    vim_free(fname);
	}
#ifdef FEAT_EVAL
	இல்லையெனில் எனில் (*arg == '@')
	{
	    // redirect to a register a-z (resp. A-Z for appending)
	    close_redir();
	    ++arg;
	    எனில் (ASCII_ISALPHA(*arg)
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
		எனில் (*arg == '>' && arg[1] == '>')  // append
		    arg += 2;
		இல்லையெனில்
		{
		    // Can use both "@a" and "@a>".
		    எனில் (*arg == '>')
			arg++;
		    // Make register empty when not using @A-@Z and the
		    // command is valid.
		    எனில் (*arg == NUL && !SAFE_isupper(redir_reg))
			write_reg_contents(redir_reg, (char_u *)"", -1, FALSE);
		}
	    }
	    எனில் (*arg != NUL)
	    {
		redir_reg = 0;
		semsg(_(e_invalid_argument_str), eap->arg);
	    }
	}
	இல்லையெனில் எனில் (*arg == '=' && arg[1] == '>')
	{
	    முழு append;

	    // redirect to a variable
	    close_redir();
	    arg += 2;

	    எனில் (*arg == '>')
	    {
		++arg;
		append = TRUE;
	    }
	    இல்லையெனில்
		append = FALSE;

	    எனில் (var_redir_start(skipwhite(arg), append) == OK)
		redir_vname = 1;
	}
#endif

	// TODO: redirect to a buffer

	இல்லையெனில்
	    semsg(_(e_invalid_argument_str), eap->arg);
    }

    // Make sure redirection is not off.  Can happen for cmdline completion
    // that indirectly invokes a command to catch its output.
    எனில் (redir_fd != வெற்று
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname
#endif
							)
	redir_off = FALSE;
}

/*
 * ":redraw": force redraw, with clear for ":redraw!".
 */
    வெறுமை
ex_redraw(exarg_T *eap)
{
    redraw_cmd(eap->forceit);
}

/*
 * ":redraw": force redraw, with clear if "clear" is TRUE.
 */
    வெறுமை
redraw_cmd(முழு clear)
{
    முழு save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    முழு save_p_lz = p_lz;
    p_lz = FALSE;

    validate_cursor();
    update_topline();
    update_screen(clear ? UPD_CLEAR : VIsual_active ? UPD_INVERTED : 0);
    எனில் ((State & MODE_CMDLINE) == 0)
	setcursor(); // put cursor back where it belongs
    எனில் (need_maketitle)
	maketitle();
#எனில் defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
    எனில் (!gui.in_use)
# endif
	resize_console_buf();
#endif
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;

    // After drawing the statusline screen_attr may still be set.
    screen_stop_highlight();

    // Reset msg_didout, so that a message that's there is overwritten.
    செய்தி_didout = FALSE;
    செய்தி_col = 0;

    // No need to wait after an intentional redraw.
    need_wait_return = FALSE;

    // When invoked from a callback or autocmd the command line may be active.
    எனில் (State & MODE_CMDLINE)
	redrawcmdline();

    out_flush();
}

/*
 * ":redrawstatus": force redraw of status line(s)
 */
    நிலையான வெறுமை
ex_redrawstatus(exarg_T *eap UNUSED)
{
    எனில் (eap->forceit)
	status_redraw_all();
    இல்லையெனில்
	status_redraw_curbuf();
    redraw_vseps = TRUE;
    எனில் (செய்தி_scrolled && (State & MODE_CMDLINE))
	திரும்பு;  // redraw later

    முழு save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    முழு save_p_lz = p_lz;
    p_lz = FALSE;

    எனில் (State & MODE_CMDLINE)
	redraw_statuslines();
    இல்லையெனில்
	update_screen(VIsual_active ? UPD_INVERTED : 0);
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabline": force redraw of the tabline
 */
    நிலையான வெறுமை
ex_redrawtabline(exarg_T *eap UNUSED)
{
    முழு save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    முழு save_p_lz = p_lz;
    p_lz = FALSE;

    draw_tabline();

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabpanel": force redraw of the tabpanel
 */
    நிலையான வெறுமை
ex_redrawtabpanel(exarg_T *eap UNUSED)
{
    முழு save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    முழு save_p_lz = p_lz;
    p_lz = FALSE;

#எனில் defined(FEAT_TABPANEL)
    draw_tabpanel();
#இல்லையெனில்
    emsg(_(e_cannot_not_support_redrawtabpanel));
#endif

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

    நிலையான வெறுமை
close_redir(வெறுமை)
{
    எனில் (redir_fd != வெற்று)
    {
	fclose(redir_fd);
	redir_fd = வெற்று;
    }
#ifdef FEAT_EVAL
    redir_reg = 0;
    எனில் (redir_vname)
    {
	var_redir_stop();
	redir_vname = 0;
    }
#endif
}

#எனில் defined(FEAT_SESSION) || defined(FEAT_EVAL)
    முழு
vim_mkdir_emsg(char_u *name, முழு prot UNUSED)
{
    எனில் (vim_mkdir(name, prot) != 0)
    {
	semsg(_(e_cannot_create_directory_str), name);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}
#endif

/*
 * Open a file for writing for an Ex command, with some checks.
 * Return file descriptor, or NULL on failure.
 */
    FILE *
open_exfile(
    char_u	*fname,
    முழு		forceit,
    எழுத்து	*mode)	    // "w" for create new file or "a" for append
{
    FILE	*fd;

#ifdef UNIX
    // with Unix it is possible to open a directory
    எனில் (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	திரும்பு வெற்று;
    }
#endif
    எனில் (!forceit && *mode != 'a' && vim_fexists(fname))
    {
	semsg(_(e_str_exists_add_bang_to_override), fname);
	திரும்பு வெற்று;
    }

    எனில் ((fd = mch_fopen((எழுத்து *)fname, mode)) == வெற்று)
	semsg(_(e_cannot_open_str_for_writing_2), fname);

    திரும்பு fd;
}

/*
 * ":mark" and ":k".
 */
    நிலையான வெறுமை
ex_mark(exarg_T *eap)
{
    pos_T	pos;

#ifdef FEAT_EVAL
    எனில் (not_in_vim9(eap, false) == FAIL)
	திரும்பு;
#endif
    எனில் (*eap->arg == NUL)		// No argument?
    {
	emsg(_(e_argument_required));
	திரும்பு;
    }

    எனில் (eap->arg[1] != NUL)	// more than one character?
    {
	semsg(_(e_trailing_characters_str), eap->arg);
	திரும்பு;
    }

    pos = curwin->w_cursor;		// save curwin->w_cursor
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_WHITE | BL_FIX);
    எனில் (setmark(*eap->arg) == FAIL)	// set mark
	emsg(_(e_argument_must_be_letter_or_forward_backward_quote));
    curwin->w_cursor = pos;		// restore curwin->w_cursor
}

/*
 * Update w_topline, w_leftcol and the cursor position.
 */
    வெறுமை
update_topline_cursor(வெறுமை)
{
    check_cursor();		// put cursor on valid line
    update_topline();
    எனில் (!curwin->w_p_wrap)
	validate_cursor();
    update_curswant();
}

/*
 * Save the current State and go to Normal mode.
 * Return TRUE if the typeahead could be saved.
 */
    முழு
save_current_state(save_state_T *sst)
{
    sst->save_செய்தி_scroll = செய்தி_scroll;
    sst->save_restart_edit = restart_edit;
    sst->save_செய்தி_didout = செய்தி_didout;
    sst->save_State = State;
    sst->save_insertmode = p_im;
    sst->save_finish_op = finish_op;
    sst->save_opcount = opcount;
    sst->save_reg_executing = reg_executing;
    sst->save_pending_end_reg_executing = pending_end_reg_executing;

    செய்தி_scroll = FALSE;		    // no msg scrolling in Normal mode
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
    திரும்பு sst->tabuf.typebuf_valid;
}

    வெறுமை
restore_current_state(save_state_T *sst)
{
    // Restore the previous typeahead.
    restore_typeahead(&sst->tabuf, FALSE);

    செய்தி_scroll = sst->save_செய்தி_scroll;
    restart_edit = sst->save_restart_edit;
    p_im = sst->save_insertmode;
    finish_op = sst->save_finish_op;
    opcount = sst->save_opcount;
    reg_executing = sst->save_reg_executing;
    pending_end_reg_executing = sst->save_pending_end_reg_executing;
    செய்தி_didout |= sst->save_செய்தி_didout;	// don't reset msg_didout now
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
    வெறுமை
ex_normal(exarg_T *eap)
{
    save_state_T save_state;
    char_u	*arg = வெற்று;
    முழு		l;
    char_u	*p;

    எனில் (ex_normal_lock > 0)
    {
	emsg(_(e_not_allowed_here));
	திரும்பு;
    }
    எனில் (ex_normal_busy >= p_mmd)
    {
	emsg(_(e_recursive_use_of_normal_too_deep));
	திரும்பு;
    }

    /*
     * vgetc() expects a CSI and K_SPECIAL to have been escaped.  Don't do
     * this for the K_SPECIAL leading byte, otherwise special keys will not
     * work.
     */
    எனில் (has_mbyte)
    {
	முழு	len = 0;

	// Count the number of characters to be escaped.
	க்கு (p = eap->arg; *p != NUL; ++p)
	{
#ifdef FEAT_GUI
	    எனில் (*p == CSI)  // leadbyte CSI
		len += 2;
#endif
	    க்கு (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		எனில் (*++p == K_SPECIAL	  // trailbyte K_SPECIAL or CSI
#ifdef FEAT_GUI
			|| *p == CSI
#endif
			)
		    len += 2;
	}
	எனில் (len > 0)
	{
	    arg = alloc(STRLEN(eap->arg) + len + 1);
	    எனில் (arg != வெற்று)
	    {
		len = 0;
		க்கு (p = eap->arg; *p != NUL; ++p)
		{
		    arg[len++] = *p;
#ifdef FEAT_GUI
		    எனில் (*p == CSI)
		    {
			arg[len++] = KS_EXTRA;
			arg[len++] = (முழு)KE_CSI;
		    }
#endif
		    க்கு (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		    {
			arg[len++] = *++p;
			எனில் (*p == K_SPECIAL)
			{
			    arg[len++] = KS_SPECIAL;
			    arg[len++] = KE_FILLER;
			}
#ifdef FEAT_GUI
			இல்லையெனில் எனில் (*p == CSI)
			{
			    arg[len++] = KS_EXTRA;
			    arg[len++] = (முழு)KE_CSI;
			}
#endif
		    }
		    arg[len] = NUL;
		}
	    }
	}
    }

    ++ex_normal_busy;
    எனில் (save_current_state(&save_state))
    {
	/*
	 * Repeat the :normal command for each line in the range.  When no
	 * range given, execute it just once, without positioning the cursor
	 * first.
	 */
	செய்
	{
	    எனில் (eap->addr_மொத்தம் != 0)
	    {
		curwin->w_cursor.lnum = eap->line1++;
		curwin->w_cursor.col = 0;
		check_cursor_moved(curwin);
	    }

	    exec_normal_cmd(arg != வெற்று
		     ? arg
		     : eap->arg, eap->forceit ? REMAP_NONE : REMAP_YES, FALSE);
	}
	போது (eap->addr_மொத்தம் > 0 && eap->line1 <= eap->line2 && !got_int);
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
    நிலையான வெறுமை
ex_startinsert(exarg_T *eap)
{
#ifdef FEAT_TERMINAL
    // Ignore this when running in an active terminal.
    எனில் (term_job_running(curbuf->b_term))
	திரும்பு;
#endif
    எனில் (!curbuf->b_p_ma && !p_im)
    {
	// Only give this error when 'insertmode' is off.
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	திரும்பு;
    }
    எனில் (eap->forceit)
    {
	// cursor line can be zero on startup
	எனில் (!curwin->w_cursor.lnum)
	    curwin->w_cursor.lnum = 1;
	set_cursor_for_append_to_வரி();
    }

    // Ignore the command when already in Insert mode.  Inserting an
    // expression register that invokes a function can do this.
    எனில் (State & MODE_செருகு)
	திரும்பு;

    எனில் (eap->cmdidx == CMD_startinsert)
	restart_edit = 'a';
    இல்லையெனில் எனில் (eap->cmdidx == CMD_startreplace)
	restart_edit = 'R';
    இல்லையெனில்
	restart_edit = 'V';

    எனில் (!eap->forceit)
    {
	எனில் (eap->cmdidx == CMD_startinsert)
	    restart_edit = 'i';
	curwin->w_curswant = 0;	    // avoid MAXCOL
    }

    எனில் (VIsual_active)
	showmode();
}

/*
 * ":stopinsert"
 */
    நிலையான வெறுமை
ex_stopinsert(exarg_T *eap UNUSED)
{
    restart_edit = 0;
    stop_செருகு_mode = TRUE;
#எனில் defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // when called from remote_expr in insert mode, make sure insert mode is
    // ended by adding K_NOP to the typeahead buffer
    எனில் (vgetc_busy)
	ins_char_typebuf(K_NOP, 0);
#endif
    clearmode();
}

/*
 * Execute normal mode command "cmd".
 * "remap" can be REMAP_NONE or REMAP_YES.
 */
    வெறுமை
exec_normal_cmd(char_u *cmd, முழு remap, முழு silent)
{
    // Stuff the argument into the typeahead buffer.
    ins_typebuf(cmd, remap, 0, TRUE, silent);
    exec_normal(FALSE, FALSE, FALSE);
}

/*
 * Execute normal_cmd() until there is no typeahead left.
 * When "use_vpeekc" is TRUE use vpeekc() to check for available chars.
 */
    வெறுமை
exec_normal(முழு was_typed, முழு use_vpeekc, முழு may_use_terminal_loop UNUSED)
{
    oparg_T	oa;
    முழு		c;

    // When calling vpeekc() from feedkeys() it will return Ctrl_C when there
    // is nothing to get, so also check for Ctrl_C.
    clear_oparg(&oa);
    finish_op = FALSE;
    போது ((!stuff_empty()
		|| ((was_typed || !typebuf_typed()) && typebuf.tb_len > 0)
		|| (use_vpeekc && (c = vpeekc()) != NUL && c != Ctrl_C))
	    && !got_int)
    {
	update_topline_cursor();
#ifdef FEAT_TERMINAL
	எனில் (may_use_terminal_loop && term_use_loop()
		&& oa.op_type == OP_NOP && oa.regname == NUL
		&& !VIsual_active)
	{
	    // If terminal_loop() returns OK we got a key that is handled
	    // in Normal model.  With FAIL we first need to position the
	    // cursor and the screen needs to be redrawn.
	    எனில் (terminal_loop(TRUE) == OK)
		normal_cmd(&oa, TRUE);
	}
	இல்லையெனில்
#endif
	    // execute a Normal mode cmd
	    normal_cmd(&oa, TRUE);
    }
}

#ifdef FEAT_FIND_ID
    நிலையான வெறுமை
ex_checkpath(exarg_T *eap)
{
    find_pattern_in_path(வெற்று, 0, 0, FALSE, FALSE, CHECK_PATH, 1L,
	    eap->forceit ? ACTION_SHOW_ALL : ACTION_SHOW,
	    (linenr_T)1, (linenr_T)MAXLNUM, eap->forceit, FALSE);
}

# எனில் defined(FEAT_QUICKFIX)
/*
 * ":psearch"
 */
    நிலையான வெறுமை
ex_psearch(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;
    ex_findpat(eap);
    g_do_tagpreview = 0;
}
# endif

    நிலையான வெறுமை
ex_findpat(exarg_T *eap)
{
    முழு		whole = TRUE;
    long	n;
    char_u	*p;
    முழு		action;

    தேர்வு (cmdnames[eap->cmdidx].cmd_name[2])
    {
	நிலை 'e':	// ":psearch", ":isearch" and ":dsearch"
		எனில் (cmdnames[eap->cmdidx].cmd_name[0] == 'p')
		    action = ACTION_GOTO;
		இல்லையெனில்
		    action = ACTION_SHOW;
		நிறுத்து;
	நிலை 'i':	// ":ilist" and ":dlist"
		action = ACTION_SHOW_ALL;
		நிறுத்து;
	நிலை 'u':	// ":ijump" and ":djump"
		action = ACTION_GOTO;
		நிறுத்து;
	இயல்பு:	// ":isplit" and ":dsplit"
		action = ACTION_SPLIT;
		நிறுத்து;
    }

    n = 1;
    எனில் (vim_isdigit(*eap->arg))	// get count
    {
	n = getdigits(&eap->arg);
	eap->arg = skipwhite(eap->arg);
    }
    எனில் (*eap->arg == '/')   // Match regexp, not just whole words
    {
	whole = FALSE;
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	எனில் (*p)
	{
	    *p++ = NUL;
	    p = skipwhite(p);

	    // Check for trailing illegal characters
	    எனில் (!ends_excmd2(eap->arg, p))
		eap->errmsg = ex_errmsg(e_trailing_characters_str, p);
	    இல்லையெனில்
		set_nextcmd(eap, p);
	}
    }
    எனில் (!eap->skip)
	find_pattern_in_path(eap->arg, 0, (முழு)STRLEN(eap->arg),
		whole, !eap->forceit,
		*eap->cmd == 'd' ? FIND_DEFINE : FIND_ANY, n, action,
		eap->line1, eap->line2, eap->forceit, FALSE);
}
#endif


#ifdef FEAT_QUICKFIX
/*
 * ":ptag", ":ptselect", ":ptjump", ":ptnext", etc.
 */
    நிலையான வெறுமை
ex_ptag(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;  // will be reset to 0 in ex_tag_cmd()
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
}

/*
 * ":pedit"
 */
    நிலையான வெறுமை
ex_pedit(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Edit the file.
    do_exedit(eap, வெற்று);

    back_to_current_window(curwin_save);
}

/*
 * ":pbuffer"
 */
    நிலையான வெறுமை
ex_pbuffer(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Go to the buffer.
    do_exbuffer(eap);

    back_to_current_window(curwin_save);
}

    நிலையான வெறுமை
prepare_preview_window(வெறுமை)
{
    எனில் (ERROR_IF_ANY_POPUP_WINDOW)
	திரும்பு;

    // Open the preview window or popup and make it the current window.
    g_do_tagpreview = p_pvh;
    prepare_tagpreview(TRUE, TRUE, FALSE);
}

    நிலையான வெறுமை
back_to_current_window(win_T *curwin_save)
{
    எனில் (curwin != curwin_save && win_valid(curwin_save))
    {
	// Return cursor to where we were
	validate_cursor();
	redraw_later(UPD_VALID);
	win_enter(curwin_save, TRUE);
    }
# ifdef FEAT_PROP_POPUP
    இல்லையெனில் எனில் (WIN_IS_POPUP(curwin))
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
    நிலையான வெறுமை
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
    நிலையான வெறுமை
ex_tag(exarg_T *eap)
{
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name);
}

    நிலையான வெறுமை
ex_tag_cmd(exarg_T *eap, char_u *name)
{
    முழு		cmd;

    தேர்வு (name[1])
    {
	நிலை 'j': cmd = DT_JUMP;	// ":tjump"
		  நிறுத்து;
	நிலை 's': cmd = DT_SELECT;	// ":tselect"
		  நிறுத்து;
	நிலை 'p': cmd = DT_PREV;	// ":tprevious"
		  நிறுத்து;
	நிலை 'N': cmd = DT_PREV;	// ":tNext"
		  நிறுத்து;
	நிலை 'n': cmd = DT_NEXT;	// ":tnext"
		  நிறுத்து;
	நிலை 'o': cmd = DT_எடு;		// ":pop"
		  நிறுத்து;
	நிலை 'f':			// ":tfirst"
	நிலை 'r': cmd = DT_FIRST;	// ":trewind"
		  நிறுத்து;
	நிலை 'l': cmd = DT_LAST;	// ":tlast"
		  நிறுத்து;
	இயல்பு:			// ":tag"
#ifdef FEAT_CSCOPE
		  எனில் (p_cst && *eap->arg != NUL)
		  {
		      ex_cstag(eap);
		      திரும்பு;
		  }
#endif
		  cmd = DT_TAG;
		  நிறுத்து;
    }

    எனில் (name[0] == 'l')
    {
#ifndef FEAT_QUICKFIX
	ex_ni(eap);
	திரும்பு;
#இல்லையெனில்
	cmd = DT_LTAG;
#endif
    }

    do_tag(eap->arg, cmd, eap->addr_மொத்தம் > 0 ? (முழு)eap->line2 : 1,
							  eap->forceit, TRUE);
}

எண்ணிக்கை {
    SPEC_PERC = 0,
    SPEC_HASH,
    SPEC_CWORD,	    // cursor word
    SPEC_CCWORD,    // cursor WORD
    SPEC_CEXPR,	    // expr under cursor
    SPEC_CFILE,	    // cursor path name
    SPEC_SFILE,	    // ":so" file name
    SPEC_SLNUM,	    // ":so" file line number
    SPEC_அடுக்கு,	    // call stack
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
    முழு
find_cmdline_var(char_u *src, size_t *usedlen)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    நிலையான keyvalue_T spec_str_tab[] = {
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
	KEYVALUE_ENTRY(SPEC_அடுக்கு, "stack>")	    // call stack
    };
    keyvalue_T target;
    keyvalue_T *entry;

    தேர்வு (*src)
    {
    நிலை '%':
	*usedlen = 1;
	திரும்பு SPEC_PERC;

    நிலை '#':
	*usedlen = 1;
	திரும்பு SPEC_HASH;

    நிலை '<':
	target.சாவி = 0;
	target.மதிப்பு.string = src + 1;	    // skip over '<'
	target.மதிப்பு.length = 0;	    // not used, see cmp_keyvalue_value_n()

	entry = (keyvalue_T *)bsearch(&target, &spec_str_tab,
		ARRAY_LENGTH(spec_str_tab), அளவு(spec_str_tab[0]),
		cmp_keyvalue_மதிப்பு_n);
	எனில் (entry == வெற்று)
	    திரும்பு -1;

	*usedlen = entry->மதிப்பு.length + 1;
	திரும்பு entry->சாவி;

    இயல்பு:
	நிறுத்து;
    }

    திரும்பு -1;
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
    எழுத்து	**errormsg,	// pointer to error message
    முழு		*escaped,	// return value has escaped white space (can
				// be NULL)
    முழு		empty_is_error)	// empty result is considered an error
{
    முழு		i;
    char_u	*s;
    char_u	*result = (char_u *)"";
    char_u	*resultbuf = வெற்று;
    size_t	resultlen;
    buf_T	*buf;
    முழு		valid = VALID_HEAD + VALID_PATH;    // assume valid result
    முழு		spec_idx;
    முழு		tilde_file = FALSE;
    முழு		skip_mod = FALSE;
    char_u	strbuf[30];

    *errormsg = வெற்று;
    எனில் (escaped != வெற்று)
	*escaped = FALSE;

    /*
     * Check if there is something to do.
     */
    spec_idx = find_cmdline_var(src, usedlen);
    எனில் (spec_idx < 0)	// no match
    {
	*usedlen = 1;
	திரும்பு வெற்று;
    }

    /*
     * Skip when preceded with a backslash "\%" and "\#".
     * Note: In "\\%" the % is also not recognized!
     */
    எனில் (src > srcstart && src[-1] == '\\')
    {
	*usedlen = 0;
	STRMOVE(src - 1, src);	// remove backslash
	திரும்பு வெற்று;
    }

    /*
     * word or WORD under cursor
     */
    எனில் (spec_idx == SPEC_CWORD || spec_idx == SPEC_CCWORD
						     || spec_idx == SPEC_CEXPR)
    {
	resultlen = find_ident_under_cursor(&result,
		spec_idx == SPEC_CWORD ? (FIND_IDENT | FIND_STRING)
	      : spec_idx == SPEC_CEXPR ? (FIND_IDENT | FIND_STRING | FIND_EVAL)
	      : FIND_STRING);
	எனில் (resultlen == 0)
	{
	    *errormsg = "";
	    திரும்பு வெற்று;
	}
    }

    /*
     * '#': Alternate file name
     * '%': Current file name
     *	    File name under the cursor
     *	    File name for autocommand
     *	and following modifiers
     */
    இல்லையெனில்
    {
	size_t off = 0;

	தேர்வு (spec_idx)
	{
	நிலை SPEC_PERC:
#ifdef FEAT_EVAL
		எனில் (!in_vim9script() || src[1] != '%')
#endif
		{
		    // '%': current file
		    எனில் (curbuf->b_fname == வெற்று)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    இல்லையெனில்
		    {
			result = curbuf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		    நிறுத்து;
		}
#ifdef FEAT_EVAL
		// "%%" alternate file
		off = 1;
#endif
		// FALLTHROUGH
	நிலை SPEC_HASH:		// '#' or "#99": alternate file
		எனில் (off == 0 ? src[1] == '#' : src[2] == '%')
		{
		    // "##" or "%%%": the argument list
		    result = arg_all();
		    resultbuf = result;
		    *usedlen = off + 2;
		    எனில் (escaped != வெற்று)
			*escaped = TRUE;
		    skip_mod = TRUE;
		    நிறுத்து;
		}
		s = src + off + 1;
		எனில் (*s == '<')		// "#<99" uses v:oldfiles
		    ++s;
		i = (முழு)getdigits(&s);
		எனில் (s == src + off + 2 && src[off + 1] == '-')
		    // just a minus sign, don't skip over it
		    s--;
		*usedlen = (முழு)(s - src); // length of what we expand

		எனில் (src[off + 1] == '<' && i != 0)
		{
		    எனில் (*usedlen < off + 2)
		    {
			// Should we give an error message for #<text?
			*usedlen = off + 1;
			திரும்பு வெற்று;
		    }
#ifdef FEAT_EVAL
		    result = list_find_str(get_vim_var_list(VV_OLDFILES),
								     (long)i);
		    எனில் (result == வெற்று)
		    {
			*errormsg = "";
			திரும்பு வெற்று;
		    }
#இல்லையெனில்
		    *errormsg = _(e_hashsmall_is_not_available_without_the_eval_feature);
		    திரும்பு வெற்று;
#endif
		}
		இல்லையெனில்
		{
		    எனில் (i == 0 && src[off + 1] == '<' && *usedlen > off + 1)
			*usedlen = off + 1;
		    buf = buflist_findnr(i);
		    எனில் (buf == வெற்று)
		    {
			*errormsg = _(e_no_alternate_file_name_to_substitute_for_hash);
			திரும்பு வெற்று;
		    }
		    எனில் (lnump != வெற்று)
			*lnump = ECMD_LAST;
		    எனில் (buf->b_fname == வெற்று)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    இல்லையெனில்
		    {
			result = buf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		}
		நிறுத்து;

	நிலை SPEC_CFILE:	// file name under cursor
		result = file_name_at_cursor(FNAME_MESS|FNAME_HYP, 1L, வெற்று);
		எனில் (result == வெற்று)
		{
		    *errormsg = "";
		    திரும்பு வெற்று;
		}
		resultbuf = result;	    // remember allocated string
		நிறுத்து;

	நிலை SPEC_AFILE:	// file name for autocommand
		result = autocmd_fname;
		எனில் (result != வெற்று && !autocmd_fname_full)
		{
		    // Still need to turn the fname into a full path.  It is
		    // postponed to avoid a delay when <afile> is not used.
		    autocmd_fname_full = TRUE;
		    result = FullName_save(autocmd_fname, FALSE);
		    vim_free(autocmd_fname);
		    autocmd_fname = result;
		}
		எனில் (result == வெற்று)
		{
		    *errormsg = _(e_no_autocommand_file_name_to_substitute_for_afile);
		    திரும்பு வெற்று;
		}
		result = shorten_fname1(result);
		நிறுத்து;

	நிலை SPEC_ABUF:		// buffer number for autocommand
		எனில் (autocmd_bufnr <= 0)
		{
		    *errormsg = _(e_no_autocommand_buffer_எண்_to_substitute_for_abuf);
		    திரும்பு வெற்று;
		}
		sprintf((எழுத்து *)strbuf, "%d", autocmd_bufnr);
		result = strbuf;
		நிறுத்து;

	நிலை SPEC_AMATCH:	// match name for autocommand
		result = autocmd_match;
		எனில் (result == வெற்று)
		{
		    *errormsg = _(e_no_autocommand_match_name_to_substitute_for_amatch);
		    திரும்பு வெற்று;
		}
		நிறுத்து;

	நிலை SPEC_SFILE:	// file name for ":so" command
		result = estack_sfile(ESTACK_SFILE);
		எனில் (result == வெற்று)
		{
		    *errormsg = _(e_no_source_file_name_to_substitute_for_sfile);
		    திரும்பு வெற்று;
		}
		resultbuf = result;	    // remember allocated string
		நிறுத்து;
	நிலை SPEC_அடுக்கு:	// call stack
		result = estack_sfile(ESTACK_அடுக்கு);
		எனில் (result == வெற்று)
		{
		    *errormsg = _(e_no_call_அடுக்கு_to_substitute_for_அடுக்கு);
		    திரும்பு வெற்று;
		}
		resultbuf = result;	    // remember allocated string
		நிறுத்து;
	நிலை SPEC_SCRIPT:	// script file name
		result = estack_sfile(ESTACK_SCRIPT);
		எனில் (result == வெற்று)
		{
		    *errormsg = _(e_no_script_file_name_to_substitute_for_script);
		    திரும்பு வெற்று;
		}
		resultbuf = result;	    // remember allocated string
		நிறுத்து;

	நிலை SPEC_SLNUM:	// line in file for ":so" command
		எனில் (SOURCING_NAME == வெற்று || SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_வரி_எண்_to_use_for_slnum);
		    திரும்பு வெற்று;
		}
		sprintf((எழுத்து *)strbuf, "%ld", SOURCING_LNUM);
		result = strbuf;
		நிறுத்து;

#ifdef FEAT_EVAL
	நிலை SPEC_SFLNUM:	// line in script file
		எனில் (current_sctx.sc_lnum + SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_வரி_எண்_to_use_for_sflnum);
		    திரும்பு வெற்று;
		}
		sprintf((எழுத்து *)strbuf, "%ld",
				 (long)(current_sctx.sc_lnum + SOURCING_LNUM));
		result = strbuf;
		நிறுத்து;

	நிலை SPEC_SID:
		எனில் (current_sctx.sc_sid <= 0)
		{
		    *errormsg = _(e_using_sid_not_in_script_context);
		    திரும்பு வெற்று;
		}
		sprintf((எழுத்து *)strbuf, "<SNR>%d_", current_sctx.sc_sid);
		result = strbuf;
		நிறுத்து;
#endif

#ifdef FEAT_CLIENTSERVER
	நிலை SPEC_CLIENT:	// Source of last submitted input
# ifdef MSWIN
		sprintf((எழுத்து *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		result = strbuf;
# இல்லையெனில்
#  ifdef FEAT_SOCKETSERVER
		எனில் (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		{
		    எனில் (client_socket == வெற்று)
			result = (char_u *)"";
		    இல்லையெனில்
			result = client_socket;
		}
#  endif
#  ifdef FEAT_X11
		எனில் (clientserver_method == CLIENTSERVER_METHOD_X11)
		{
		    sprintf((எழுத்து *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		    result = strbuf;
		}
#  endif
# endif
		நிறுத்து;
#endif
	}

	resultlen = STRLEN(result);	// length of new string
	எனில் (src[*usedlen] == '<')	// remove the file name extension
	{
	    ++*usedlen;
	    எனில் ((s = vim_strrchr(result, '.')) != வெற்று && s >= gettail(result))
		resultlen = s - result;
	}
	இல்லையெனில் எனில் (!skip_mod)
	{
	    valid |= modify_fname(src, tilde_file, usedlen, &result, &resultbuf,
								  &resultlen);
	    எனில் (result == வெற்று)
	    {
		*errormsg = "";
		திரும்பு வெற்று;
	    }
	}
    }

    எனில் (resultlen == 0 || valid != VALID_HEAD + VALID_PATH)
    {
	எனில் (empty_is_error)
	{
	    எனில் (valid != VALID_HEAD + VALID_PATH)
		*errormsg = _(e_empty_file_name_for_percent_or_hash_only_works_with_ph);
	    இல்லையெனில்
		*errormsg = _(e_evaluates_to_an_empty_string);
	}
	result = வெற்று;
    }
    இல்லையெனில்
	result = vim_strnsave(result, resultlen);
    vim_free(resultbuf);
    திரும்பு result;
}

/*
 * Expand the <sfile> string in "arg".
 *
 * Returns an allocated string, or NULL for any error.
 */
    char_u *
expand_sfile(char_u *arg)
{
    எழுத்து	*errormsg;
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
    எனில் (result == வெற்று)
	திரும்பு வெற்று;

    க்கு (p = result; *p; )
    {
	எனில் (STRNCMP(p, "<sfile>", 7) != 0)
	    ++p;
	இல்லையெனில்
	{
	    // replace "<sfile>" with the sourced file name, and do ":" stuff
	    repl = eval_vars(p, result, &srclen, வெற்று, &errormsg, வெற்று, TRUE);
	    எனில் (errormsg != வெற்று)
	    {
		எனில் (*errormsg)
		    emsg(errormsg);
		vim_free(result);
		திரும்பு வெற்று;
	    }
	    எனில் (repl == வெற்று)		// no match (cannot happen)
	    {
		p += srclen;
		தொடர்;
	    }
	    repllen = STRLEN(repl);
	    resultlen += repllen - srclen;
	    newres = alloc(resultlen + 1);
	    எனில் (newres == வெற்று)
	    {
		vim_free(repl);
		vim_free(result);
		திரும்பு வெற்று;
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

    திரும்பு result;
}

#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
/*
 * Make a dialog message in "buff[DIALOG_MSG_SIZE]".
 * "format" must contain "%s".
 */
    வெறுமை
dialog_செய்தி(char_u *buff, எழுத்து *format, char_u *fname)
{
    எனில் (fname == வெற்று)
	fname = (char_u *)_("Untitled");
    vim_snprintf((எழுத்து *)buff, DIALOG_செய்தி_SIZE, format, fname);
}
#endif

/*
 * ":behave {mswin,xterm}"
 */
    நிலையான வெறுமை
ex_behave(exarg_T *eap)
{
    எனில் (STRCMP(eap->arg, "mswin") == 0)
    {
	set_option_மதிப்பு_give_err((char_u *)"selection",
						 0L, (char_u *)"exclusive", 0);
	set_option_மதிப்பு_give_err((char_u *)"selectmode",
						 0L, (char_u *)"mouse,key", 0);
	set_option_மதிப்பு_give_err((char_u *)"mousemodel",
						     0L, (char_u *)"popup", 0);
	set_option_மதிப்பு_give_err((char_u *)"keymodel",
					  0L, (char_u *)"startsel,stopsel", 0);
    }
    இல்லையெனில் எனில் (STRCMP(eap->arg, "xterm") == 0)
    {
	set_option_மதிப்பு_give_err((char_u *)"selection",
						 0L, (char_u *)"inclusive", 0);
	set_option_மதிப்பு_give_err((char_u *)"selectmode", 0L, (char_u *)"", 0);
	set_option_மதிப்பு_give_err((char_u *)"mousemodel",
						    0L, (char_u *)"extend", 0);
	set_option_மதிப்பு_give_err((char_u *)"keymodel", 0L, (char_u *)"", 0);
    }
    இல்லையெனில்
	semsg(_(e_invalid_argument_str), eap->arg);
}

நிலையான முழு filetype_detect = FALSE;
நிலையான முழு filetype_plugin = FALSE;
நிலையான முழு filetype_indent = FALSE;

/*
 * ":filetype [plugin] [indent] {on,off,detect}"
 * on: Load the filetype.vim file to install autocommands for file types.
 * off: Load the ftoff.vim file to remove all autocommands for file types.
 * plugin on: load filetype.vim and ftplugin.vim
 * plugin off: load ftplugof.vim
 * indent on: load filetype.vim and indent.vim
 * indent off: load indoff.vim
 */
    நிலையான வெறுமை
ex_filetype(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    முழு		plugin = FALSE;
    முழு		indent = FALSE;

    எனில் (*eap->arg == NUL)
    {
	// Print current status.
	smsg("filetype detection:%s  plugin:%s  indent:%s",
		filetype_detect ? "ON" : "OFF",
		filetype_plugin ? (filetype_detect ? "ON" : "(on)") : "OFF",
		filetype_indent ? (filetype_detect ? "ON" : "(on)") : "OFF");
	திரும்பு;
    }

    // Accept "plugin" and "indent" in any order.
    க்கு (;;)
    {
	எனில் (STRNCMP(arg, "plugin", 6) == 0)
	{
	    plugin = TRUE;
	    arg = skipwhite(arg + 6);
	    தொடர்;
	}
	எனில் (STRNCMP(arg, "indent", 6) == 0)
	{
	    indent = TRUE;
	    arg = skipwhite(arg + 6);
	    தொடர்;
	}
	நிறுத்து;
    }
    எனில் (STRCMP(arg, "on") == 0 || STRCMP(arg, "detect") == 0)
    {
	எனில் (*arg == 'o' || !filetype_detect)
	{
	    source_runtime((char_u *)FILETYPE_FILE, DIP_ALL);
	    filetype_detect = TRUE;
	    எனில் (plugin)
	    {
		source_runtime((char_u *)FTPLUGIN_FILE, DIP_ALL);
		filetype_plugin = TRUE;
	    }
	    எனில் (indent)
	    {
		source_runtime((char_u *)INDENT_FILE, DIP_ALL);
		filetype_indent = TRUE;
	    }
	}
	எனில் (*arg == 'd')
	{
	    (வெறுமை)do_doautocmd((char_u *)"filetypedetect BufRead", TRUE, வெற்று);
	    do_modelines(0);
	}
    }
    இல்லையெனில் எனில் (STRCMP(arg, "off") == 0)
    {
	எனில் (plugin || indent)
	{
	    எனில் (plugin)
	    {
		source_runtime((char_u *)FTPLUGOF_FILE, DIP_ALL);
		filetype_plugin = FALSE;
	    }
	    எனில் (indent)
	    {
		source_runtime((char_u *)INDOFF_FILE, DIP_ALL);
		filetype_indent = FALSE;
	    }
	}
	இல்லையெனில்
	{
	    source_runtime((char_u *)FTOFF_FILE, DIP_ALL);
	    filetype_detect = FALSE;
	}
    }
    இல்லையெனில்
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * ":setfiletype [FALLBACK] {name}"
 */
    நிலையான வெறுமை
ex_setfiletype(exarg_T *eap)
{
    எனில் (curbuf->b_did_filetype)
	திரும்பு;

    char_u *arg = eap->arg;
    எனில் (STRNCMP(arg, "FALLBACK ", 9) == 0)
	arg += 9;

    set_option_மதிப்பு_give_err((char_u *)"filetype", 0L, arg, OPT_LOCAL);
    எனில் (arg != eap->arg)
	curbuf->b_did_filetype = false;
}

    நிலையான வெறுமை
ex_digraphs(exarg_T *eap UNUSED)
{
#ifdef FEAT_DIGRAPHS
    எனில் (*eap->arg != NUL)
	putdigraph(eap->arg);
    இல்லையெனில்
	listdigraphs(eap->forceit);
#இல்லையெனில்
    emsg(_(e_no_digraphs_version));
#endif
}

#எனில் defined(FEAT_தேடல்_EXTRA)
    வெறுமை
set_no_hlsearch(முழு flag)
{
    no_hlsearch = flag;
# ifdef FEAT_EVAL
    set_vim_var_nr(VV_HLSEARCH, !no_hlsearch && p_hls);
# endif
}

/*
 * ":nohlsearch"
 */
    நிலையான வெறுமை
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
    நிலையான வெறுமை
ex_X(exarg_T *eap UNUSED)
{
    crypt_check_current_method();
    (வெறுமை)crypt_get_சாவி(TRUE, TRUE);
}
#endif

#ifdef FEAT_FOLDING
    நிலையான வெறுமை
ex_fold(exarg_T *eap)
{
    எனில் (foldManualAllowed(TRUE))
	foldCreate(eap->line1, eap->line2);
}

    நிலையான வெறுமை
ex_foldopen(exarg_T *eap)
{
    opFoldRange(eap->line1, eap->line2, eap->cmdidx == CMD_foldopen,
							 eap->forceit, FALSE);
}

    நிலையான வெறுமை
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
    க்கு (lnum = eap->line1; lnum <= eap->line2; ++lnum)
	எனில் (hasFolding(lnum, வெற்று, வெற்று) == (eap->cmdidx == CMD_folddoclosed))
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

#எனில் defined(FEAT_QUICKFIX)
/*
 * Returns TRUE if the supplied Ex cmdidx is for a location list command
 * instead of a quickfix command.
 */
    முழு
is_loclist_cmd(முழு cmdidx)
{
    எனில் (cmdidx < 0 || cmdidx >= CMD_SIZE)
	திரும்பு FALSE;
    திரும்பு cmdnames[cmdidx].cmd_name[0] == 'l';
}
#endif

    முழு
get_pressedreturn(வெறுமை)
{
    திரும்பு ex_pressedreturn;
}

    வெறுமை
set_pressedreturn(முழு val)
{
    ex_pressedreturn = val;
}
