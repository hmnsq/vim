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

#অন্তর্ভাব "vim.h"

স্থির পূর্ণসংখ্যা	quitmore = 0;
স্থির পূর্ণসংখ্যা	ex_pressedreturn = FALSE;
#ifndef FEAT_PRINTER
# define ex_hardcopy	ex_ni
#endif

#ifdef FEAT_EVAL
স্থির char_u	*do_one_cmd(char_u **, পূর্ণসংখ্যা, cstack_T *, char_u *(*fgetline)(পূর্ণসংখ্যা, শূন্য *, পূর্ণসংখ্যা, getline_opt_T), শূন্য *cookie);
#অন্যথা
স্থির char_u	*do_one_cmd(char_u **, পূর্ণসংখ্যা, char_u *(*fgetline)(পূর্ণসংখ্যা, শূন্য *, পূর্ণসংখ্যা, getline_opt_T), শূন্য *cookie);
স্থির পূর্ণসংখ্যা	if_level = 0;		// depth in :if
#endif
স্থির শূন্য	append_command(char_u *cmd);

#ifndef FEAT_MENU
# define ex_emenu		ex_ni
# define ex_menu		ex_ni
# define ex_menutranslate	ex_ni
#endif
স্থির শূন্য	ex_autocmd(exarg_T *eap);
স্থির শূন্য	ex_doautocmd(exarg_T *eap);
স্থির শূন্য	ex_bunload(exarg_T *eap);
স্থির শূন্য	ex_বাফার(exarg_T *eap);
স্থির শূন্য	do_exbuffer(exarg_T *eap);
স্থির শূন্য	ex_bmodified(exarg_T *eap);
স্থির শূন্য	ex_bnext(exarg_T *eap);
স্থির শূন্য	ex_bprevious(exarg_T *eap);
স্থির শূন্য	ex_brewind(exarg_T *eap);
স্থির শূন্য	ex_blast(exarg_T *eap);
স্থির char_u	*getargcmd(char_u **);
স্থির পূর্ণসংখ্যা	getargopt(exarg_T *eap);
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
#যদি !defined(FEAT_QUICKFIX) || !defined(FEAT_EVAL)
# define ex_cexpr		ex_ni
#endif

স্থির linenr_T default_address(exarg_T *eap);
স্থির শূন্য address_default_all(exarg_T *eap);
স্থির শূন্য	get_flags(exarg_T *eap);
#যদি !defined(FEAT_PERL) \
	|| !defined(FEAT_PYTHON) || !defined(FEAT_PYTHON3) \
	|| !defined(FEAT_TCL) \
	|| !defined(FEAT_RUBY) \
	|| !defined(FEAT_LUA) \
	|| !defined(FEAT_MZSCHEME)
# define HAVE_EX_SCRIPT_NI
স্থির শূন্য	ex_script_ni(exarg_T *eap);
#endif
স্থির অক্ষর	*invalid_range(exarg_T *eap);
স্থির শূন্য	correct_range(exarg_T *eap);
#ifdef FEAT_QUICKFIX
স্থির char_u	*replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep);
#endif
স্থির char_u	*repl_cmdline(exarg_T *eap, char_u *src, size_t srclen, char_u *repl, char_u **cmdlinep);
স্থির শূন্য	ex_highlight(exarg_T *eap);
স্থির শূন্য	ex_colorscheme(exarg_T *eap);
স্থির শূন্য	ex_cquit(exarg_T *eap);
স্থির শূন্য	ex_quit_all(exarg_T *eap);
স্থির শূন্য	ex_close(exarg_T *eap);
স্থির শূন্য	ex_win_close(পূর্ণসংখ্যা forceit, win_T *win, tabpage_T *tp);
স্থির শূন্য	ex_only(exarg_T *eap);
স্থির শূন্য	ex_resize(exarg_T *eap);
স্থির শূন্য	ex_stag(exarg_T *eap);
স্থির শূন্য	ex_tabclose(exarg_T *eap);
স্থির শূন্য	ex_tabonly(exarg_T *eap);
স্থির শূন্য	ex_tabnext(exarg_T *eap);
স্থির শূন্য	ex_tabmove(exarg_T *eap);
স্থির শূন্য	ex_tabs(exarg_T *eap);
#যদি defined(FEAT_QUICKFIX)
স্থির শূন্য	ex_pclose(exarg_T *eap);
স্থির শূন্য	ex_ptag(exarg_T *eap);
স্থির শূন্য	ex_pedit(exarg_T *eap);
স্থির শূন্য	ex_pbuffer(exarg_T *eap);
স্থির শূন্য	prepare_preview_window(শূন্য);
স্থির শূন্য	back_to_current_window(win_T *curwin_save);
#অন্যথা
# define ex_pclose		ex_ni
# define ex_ptag		ex_ni
# define ex_pedit		ex_ni
# define ex_pbuffer		ex_ni
#endif
স্থির শূন্য	ex_hide(exarg_T *eap);
স্থির শূন্য	ex_exit(exarg_T *eap);
স্থির শূন্য	ex_print(exarg_T *eap);
#ifdef FEAT_BYTEOFF
স্থির শূন্য	ex_goto(exarg_T *eap);
#অন্যথা
# define ex_goto		ex_ni
#endif
স্থির শূন্য	ex_shell(exarg_T *eap);
স্থির শূন্য	ex_preserve(exarg_T *eap);
স্থির শূন্য	ex_recover(exarg_T *eap);
স্থির শূন্য	ex_mode(exarg_T *eap);
স্থির শূন্য	ex_wrongmodifier(exarg_T *eap);
স্থির শূন্য	ex_find(exarg_T *eap);
স্থির শূন্য	ex_open(exarg_T *eap);
স্থির শূন্য	ex_edit(exarg_T *eap);
#ifndef FEAT_GUI
# define ex_gui			ex_nogui
স্থির শূন্য	ex_nogui(exarg_T *eap);
#endif
#যদি defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
স্থির শূন্য	ex_tearoff(exarg_T *eap);
#অন্যথা
# define ex_tearoff		ex_ni
#endif
#যদি (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
স্থির শূন্য	ex_popup(exarg_T *eap);
#অন্যথা
# define ex_popup		ex_ni
#endif
#ifndef FEAT_GUI_MSWIN
# define ex_simalt		ex_ni
#endif
#যদি !defined(FEAT_GUI_MSWIN) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
# define gui_mch_find_dialog	ex_ni
# define gui_mch_replace_dialog ex_ni
#endif
#যদি !defined(FEAT_GUI_GTK)
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
#যদি !defined(FEAT_SYN_HL) || !defined(FEAT_PROFILE)
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
#যদি !defined(FEAT_PYTHON) && !defined(FEAT_PYTHON3)
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
স্থির শূন্য	ex_swapname(exarg_T *eap);
স্থির শূন্য	ex_syncbind(exarg_T *eap);
স্থির শূন্য	ex_read(exarg_T *eap);
স্থির শূন্য	ex_pwd(exarg_T *eap);
স্থির শূন্য	ex_equal(exarg_T *eap);
স্থির শূন্য	ex_sleep(exarg_T *eap);
স্থির শূন্য	ex_winsize(exarg_T *eap);
স্থির শূন্য	ex_wincmd(exarg_T *eap);
#যদি defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
স্থির শূন্য	ex_winpos(exarg_T *eap);
#অন্যথা
# define ex_winpos	    ex_ni
#endif
স্থির শূন্য	ex_operators(exarg_T *eap);
স্থির শূন্য	ex_put(exarg_T *eap);
স্থির শূন্য	ex_iput(exarg_T *eap);
স্থির শূন্য	ex_copymove(exarg_T *eap);
স্থির শূন্য	ex_submagic(exarg_T *eap);
স্থির শূন্য	ex_join(exarg_T *eap);
স্থির শূন্য	ex_at(exarg_T *eap);
স্থির শূন্য	ex_bang(exarg_T *eap);
স্থির শূন্য	ex_undo(exarg_T *eap);
#ifdef FEAT_PERSISTENT_UNDO
স্থির শূন্য	ex_wundo(exarg_T *eap);
স্থির শূন্য	ex_rundo(exarg_T *eap);
#endif
স্থির শূন্য	ex_redo(exarg_T *eap);
স্থির শূন্য	ex_later(exarg_T *eap);
স্থির শূন্য	ex_redir(exarg_T *eap);
স্থির শূন্য	ex_redrawstatus(exarg_T *eap);
স্থির শূন্য	ex_redrawtabline(exarg_T *eap);
স্থির শূন্য	ex_redrawtabpanel(exarg_T *eap);
স্থির শূন্য	close_redir(শূন্য);
স্থির শূন্য	ex_mark(exarg_T *eap);
স্থির শূন্য	ex_startinsert(exarg_T *eap);
স্থির শূন্য	ex_stopinsert(exarg_T *eap);
#ifdef FEAT_FIND_ID
স্থির শূন্য	ex_checkpath(exarg_T *eap);
স্থির শূন্য	ex_findpat(exarg_T *eap);
#অন্যথা
# define ex_findpat		ex_ni
# define ex_checkpath		ex_ni
#endif
#যদি defined(FEAT_FIND_ID) && defined(FEAT_QUICKFIX)
স্থির শূন্য	ex_psearch(exarg_T *eap);
#অন্যথা
# define ex_psearch		ex_ni
#endif
স্থির শূন্য	ex_tag(exarg_T *eap);
স্থির শূন্য	ex_tag_cmd(exarg_T *eap, char_u *name);
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
স্থির শূন্য	ex_behave(exarg_T *eap);
স্থির শূন্য	ex_filetype(exarg_T *eap);
স্থির শূন্য	ex_setfiletype(exarg_T *eap);
#ifndef FEAT_DIFF
# define ex_diffoff		ex_ni
# define ex_diffpatch		ex_ni
# define ex_diffgetput		ex_ni
# define ex_diffsplit		ex_ni
# define ex_diffthis		ex_ni
# define ex_diffupdate		ex_ni
#endif
স্থির শূন্য	ex_digraphs(exarg_T *eap);
#ifdef FEAT_অনুসন্ধান_EXTRA
স্থির শূন্য	ex_nohlsearch(exarg_T *eap);
#অন্যথা
# define ex_nohlsearch		ex_ni
# define ex_match		ex_ni
#endif
#ifdef FEAT_CRYPT
স্থির শূন্য	ex_X(exarg_T *eap);
#অন্যথা
# define ex_X			ex_ni
#endif
#ifdef FEAT_FOLDING
স্থির শূন্য	ex_fold(exarg_T *eap);
স্থির শূন্য	ex_foldopen(exarg_T *eap);
স্থির শূন্য	ex_folddo(exarg_T *eap);
#অন্যথা
# define ex_fold		ex_ni
# define ex_foldopen		ex_ni
# define ex_folddo		ex_ni
#endif
#যদি !(defined(HAVE_LOCALE_H) || defined(X_LOCALE))
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
#যদি !defined(FEAT_X11) || !defined(FEAT_XCLIPBOARD)
# define ex_xrestore		ex_ni
#endif
#যদি !defined(FEAT_WAYLAND)
# define ex_wlrestore		ex_ni
#endif
#যদি !defined(HAVE_CLIPMETHOD)
# define ex_clipreset		ex_ni
#endif
#যদি !defined(FEAT_PROP_POPUP)
# define ex_popupclear		ex_ni
#endif

/*
 * Declare cmdnames[].
 */
#define DO_DECLARE_EXCMD
#অন্তর্ভাব "ex_cmds.h"
#অন্তর্ভাব "ex_cmdidxs.h"

স্থির char_u dollar_command[2] = {'$', 0};


#ifdef FEAT_EVAL
// Struct for storing a line inside a while/for loop
প্রকারলেখ কাঠামো
{
    char_u	*সারি;		// command line
    linenr_T	lnum;		// sourcing_lnum of the line
} wcmd_T;

/*
 * Structure used to store info for line position in a while or for loop.
 * This is required, because do_one_cmd() may invoke ex_function(), which
 * reads more lines that may come from the while/for loop.
 */
কাঠামো loop_cookie
{
    garray_T	*সারিগুলি_gap;		// growarray with line info
    পূর্ণসংখ্যা		current_সারি;		// last read line from growarray
    পূর্ণসংখ্যা		repeating;		// TRUE when looping a second time
    // When "repeating" is FALSE use "getline" and "cookie" to get lines
    char_u	*(*lc_getline)(পূর্ণসংখ্যা, শূন্য *, পূর্ণসংখ্যা, getline_opt_T);
    শূন্য	*cookie;
};

স্থির char_u	*get_loop_সারি(পূর্ণসংখ্যা c, শূন্য *cookie, পূর্ণসংখ্যা indent, getline_opt_T options);
স্থির পূর্ণসংখ্যা	store_loop_সারি(garray_T *gap, char_u *সারি);
স্থির শূন্য	free_cmdlines(garray_T *gap);

// Struct to save a few things while debugging.  Used in do_cmdline() only.
কাঠামো dbg_stuff
{
    পূর্ণসংখ্যা		trylevel;
    পূর্ণসংখ্যা		force_abort;
    except_T	*caught_স্তূপ;
    char_u	*vv_exception;
    char_u	*vv_throwpoint;
    পূর্ণসংখ্যা		did_emsg;
    পূর্ণসংখ্যা		got_int;
    পূর্ণসংখ্যা		did_throw;
    পূর্ণসংখ্যা		need_rethrow;
    পূর্ণসংখ্যা		check_cstack;
    except_T	*current_exception;
};

    স্থির শূন্য
save_dbg_stuff(কাঠামো dbg_stuff *dsp)
{
    dsp->trylevel	= trylevel;		trylevel = 0;
    dsp->force_abort	= force_abort;		force_abort = FALSE;
    dsp->caught_স্তূপ	= caught_স্তূপ;		caught_স্তূপ = ফাঁকা;
    dsp->vv_exception	= v_exception(ফাঁকা);
    dsp->vv_throwpoint	= v_throwpoint(ফাঁকা);

    // Necessary for debugging an inactive ":catch", ":finally", ":endtry"
    dsp->did_emsg	= did_emsg;		did_emsg     = FALSE;
    dsp->got_int	= got_int;		got_int	     = FALSE;
    dsp->did_throw	= did_throw;		did_throw    = FALSE;
    dsp->need_rethrow	= need_rethrow;		need_rethrow = FALSE;
    dsp->check_cstack	= check_cstack;		check_cstack = FALSE;
    dsp->current_exception = current_exception;	current_exception = ফাঁকা;
}

    স্থির শূন্য
restore_dbg_stuff(কাঠামো dbg_stuff *dsp)
{
    suppress_errthrow = FALSE;
    trylevel = dsp->trylevel;
    force_abort = dsp->force_abort;
    caught_স্তূপ = dsp->caught_স্তূপ;
    (শূন্য)v_exception(dsp->vv_exception);
    (শূন্য)v_throwpoint(dsp->vv_throwpoint);
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
    স্থির পূর্ণসংখ্যা
is_other_file(পূর্ণসংখ্যা fnum, char_u *ffname)
{
    যদি (fnum != 0)
    {
	যদি (fnum == curbuf->b_fnum)
	    ফেরত FALSE;

	ফেরত TRUE;
    }

    যদি (ffname == ফাঁকা)
	ফেরত TRUE;

    যদি (*ffname == NUL)
	ফেরত FALSE;

    // TODO: Need a reliable way to know whether a buffer is meant to live
    // on-disk !curbuf->b_dev_valid is not always available (example: missing
    // on Windows)
    যদি (curbuf->b_sfname != ফাঁকা
	    && *curbuf->b_sfname != NUL)
	// This occurs with unsaved buffers. In which case `ffname` actually
	// corresponds to curbuf->b_sfname
	ফেরত fnamecmp(ffname, curbuf->b_sfname) != 0;

    ফেরত otherfile(ffname);
}

/*
 * do_exmode(): Repeatedly get commands for the "Ex" mode, until the ":vi"
 * command is given.
 */
    শূন্য
do_exmode(
    পূর্ণসংখ্যা		improved)	    // TRUE for "improved Ex" mode
{
    পূর্ণসংখ্যা		save_বার্তা_scroll;
    পূর্ণসংখ্যা		পূর্ব_বার্তা_row;
    linenr_T	পূর্ব_সারি;
    varnumber_T	changedtick;

    যদি (improved)
	exmode_active = EXMODE_VIM;
    অন্যথা
	exmode_active = EXMODE_NORMAL;
    State = MODE_NORMAL;
    may_trigger_modechanged();

    // When using ":global /pat/ visual" and then "Q" we return to continue
    // the :global command.
    যদি (global_busy)
	ফেরত;

    save_বার্তা_scroll = বার্তা_scroll;
    ++RedrawingDisabled;	    // don't redisplay the window
    ++no_wait_return;		    // don't wait for return
#ifdef FEAT_GUI
    // Ignore scrollbar and mouse events in Ex mode
    ++hold_gui_events;
#endif

    বার্তা(_("Entering Ex mode.  Type \"visual\" to go to Normal mode."));
    যখন (exmode_active)
    {
	// Check for a ":normal" command and no more characters left.
	যদি (ex_normal_busy > 0 && typebuf.tb_len == 0)
	{
	    exmode_active = FALSE;
	    বিরতি;
	}
	বার্তা_scroll = TRUE;
	need_wait_return = FALSE;
	ex_pressedreturn = FALSE;
	ex_no_reprint = FALSE;
	changedtick = CHANGEDTICK(curbuf);
	পূর্ব_বার্তা_row = বার্তা_row;
	পূর্ব_সারি = curwin->w_cursor.lnum;
	যদি (improved)
	{
	    cmdline_row = বার্তা_row;
	    do_cmdline(ফাঁকা, getexline, ফাঁকা, 0);
	}
	অন্যথা
	    do_cmdline(ফাঁকা, getexmodeline, ফাঁকা, DOCMD_NOWAIT);
	সারিগুলি_বাম = Rows - 1;

	যদি ((পূর্ব_সারি != curwin->w_cursor.lnum
		   || changedtick != CHANGEDTICK(curbuf)) && !ex_no_reprint)
	{
	    যদি (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_বাফার));
	    অন্যথা
	    {
		যদি (ex_pressedreturn)
		{
		    // go up one line, to overwrite the ":<CR>" line, so the
		    // output doesn't contain empty lines.
		    বার্তা_row = পূর্ব_বার্তা_row;
		    যদি (পূর্ব_বার্তা_row == Rows - 1)
			বার্তা_row--;
		}
		বার্তা_col = 0;
		print_সারি_no_prefix(curwin->w_cursor.lnum, FALSE, FALSE);
		বার্তা_clr_eos();
	    }
	}
	অন্যথা যদি (ex_pressedreturn && !ex_no_reprint)	// must be at EOF
	{
	    যদি (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_বাফার));
	    অন্যথা
		emsg(_(e_at_end_of_file));
	}
    }

#ifdef FEAT_GUI
    --hold_gui_events;
#endif
    যদি (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    update_screen(UPD_CLEAR);
    need_wait_return = FALSE;
    বার্তা_scroll = save_বার্তা_scroll;
}

/*
 * Print the executed command for when 'verbose' is set.
 * When "lnum" is 0 only print the command.
 */
    স্থির শূন্য
বার্তা_verbose_cmd(linenr_T lnum, char_u *cmd)
{
    ++no_wait_return;
    verbose_enter_scroll();

    যদি (lnum == 0)
	smsg(_("Executing: %s"), cmd);
    অন্যথা
	smsg(_("line %ld: %s"), (long)lnum, cmd);
    যদি (বার্তা_silent == 0)
	বার্তা_puts("\n");   // don't overwrite this

    verbose_leave_scroll();
    --no_wait_return;
}

/*
 * Execute a simple command line.  Used for translated commands like "*".
 */
    পূর্ণসংখ্যা
do_cmdline_cmd(char_u *cmd)
{
    ফেরত do_cmdline(cmd, ফাঁকা, ফাঁকা,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
}

/*
 * Execute the "+cmd" argument of "edit +cmd fname" and the like.
 * This allows for using a range without ":" in Vim9 script.
 */
    স্থির পূর্ণসংখ্যা
do_cmd_argument(char_u *cmd)
{
    ফেরত do_cmdline(cmd, ফাঁকা, ফাঁকা,
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
    পূর্ণসংখ্যা
do_cmdline(
    char_u	*cmdline,
    char_u	*(*fgetline)(পূর্ণসংখ্যা, শূন্য *, পূর্ণসংখ্যা, getline_opt_T),
    শূন্য	*cookie,		// argument for fgetline()
    পূর্ণসংখ্যা		flags)
{
    char_u	*পরবর্তী_cmdline;		// next cmd to execute
    char_u	*cmdline_copy = ফাঁকা;	// copy of cmd line
    পূর্ণসংখ্যা		used_getline = FALSE;	// used "fgetline" to obtain command
    স্থির পূর্ণসংখ্যা	recursive = 0;		// recursive depth
    পূর্ণসংখ্যা		বার্তা_didout_before_start = 0;
    পূর্ণসংখ্যা		গুনতি = 0;		// line number count
    পূর্ণসংখ্যা		did_inc_RedrawingDisabled = FALSE;
    পূর্ণসংখ্যা		retval = OK;
#ifdef FEAT_EVAL
    cstack_T	cstack;			// conditional stack
    garray_T	সারিগুলি_ga;		// keep lines for ":while"/":for"
    পূর্ণসংখ্যা		current_সারি = 0;	// active line in lines_ga
    পূর্ণসংখ্যা		current_সারি_before = 0;
    char_u	*fname = ফাঁকা;		// function or script name
    linenr_T	*breakpoint = ফাঁকা;	// ptr to breakpoint field in cookie
    পূর্ণসংখ্যা		*dbg_tick = ফাঁকা;	// ptr to dbg_tick field in cookie
    কাঠামো dbg_stuff debug_saved;	// saved things for debug mode
    পূর্ণসংখ্যা		initial_trylevel;
    msglist_T	**saved_বার্তা_list = ফাঁকা;
    msglist_T	*private_বার্তা_list = ফাঁকা;

    // "fgetline" and "cookie" passed to do_one_cmd()
    char_u	*(*cmd_getline)(পূর্ণসংখ্যা, শূন্য *, পূর্ণসংখ্যা, getline_opt_T);
    শূন্য	*cmd_cookie;
    কাঠামো loop_cookie cmd_loop_cookie;
    শূন্য	*real_cookie;
    পূর্ণসংখ্যা		getline_is_func;
#অন্যথা
# define cmd_getline fgetline
# define cmd_cookie cookie
#endif
    স্থির পূর্ণসংখ্যা	call_depth = 0;		// recursiveness
#ifdef FEAT_EVAL
    // For every pair of do_cmdline()/do_one_cmd() calls, use an extra memory
    // location for storing error messages to be converted to an exception.
    // This ensures that the do_errthrow() call in do_one_cmd() does not
    // combine the messages stored by an earlier invocation of do_one_cmd()
    // with the command name of the later one.  This would happen when
    // BufWritePost autocommands are executed after a write error.
    saved_বার্তা_list = বার্তা_list;
    বার্তা_list = &private_বার্তা_list;
#endif

    // It's possible to create an endless loop with ":execute", catch that
    // here.  The value of 200 allows nested function calls, ":source", etc.
    // Allow 200 or 'maxfuncdepth', whatever is larger.
    যদি (call_depth >= 200
#ifdef FEAT_EVAL
	    && call_depth >= p_mfd
#endif
	    )
    {
	emsg(_(e_command_too_recursive));
#ifdef FEAT_EVAL
	// When converting to an exception, we do not include the command name
	// since this is not an error of the specific command.
	do_errthrow((cstack_T *)ফাঁকা, (char_u *)ফাঁকা);
	বার্তা_list = saved_বার্তা_list;
#endif
	ফেরত FAIL;
    }
    ++call_depth;

#ifdef FEAT_EVAL
    CLEAR_FIELD(cstack);
    cstack.cs_idx = -1;
    ga_init2(&সারিগুলি_ga, আকার(wcmd_T), 10);

    real_cookie = getline_cookie(fgetline, cookie);

    // Inside a function use a higher nesting level.
    getline_is_func = getline_equal(fgetline, cookie, get_func_সারি);
    যদি (getline_is_func && ex_nesting_level == func_level(real_cookie))
	++ex_nesting_level;

    // Get the function or script name and the address where the next breakpoint
    // line and the debug tick for a function or script are stored.
    যদি (getline_is_func)
    {
	fname = func_name(real_cookie);
	breakpoint = func_breakpoint(real_cookie);
	dbg_tick = func_dbg_tick(real_cookie);
    }
    অন্যথা যদি (getline_equal(fgetline, cookie, getsourceline))
    {
	fname = SOURCING_NAME;
	breakpoint = source_breakpoint(real_cookie);
	dbg_tick = source_dbg_tick(real_cookie);
    }

    /*
     * Initialize "force_abort"  and "suppress_errthrow" at the top level.
     */
    যদি (!recursive)
    {
	force_abort = FALSE;
	suppress_errthrow = FALSE;
    }

    /*
     * If requested, store and reset the global values controlling the
     * exception handling (used when debugging).  Otherwise clear it to avoid
     * a bogus compiler warning when the optimizer uses inline functions...
     */
    যদি (flags & DOCMD_EXCRESET)
	save_dbg_stuff(&debug_saved);
    অন্যথা
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
    যদি (!(flags & DOCMD_KEYTYPED)
			       && !getline_equal(fgetline, cookie, getexline))
	চাবি_Typed = FALSE;

    /*
     * Continue executing command lines:
     * - when inside an ":if", ":while" or ":for"
     * - for multiple commands on one line, separated with '|'
     * - when repeating until there are no more lines (for ":source")
     */
    পরবর্তী_cmdline = cmdline;
    করো
    {
#ifdef FEAT_EVAL
	getline_is_func = getline_equal(fgetline, cookie, get_func_সারি);
#endif

	// stop skipping cmds for an error msg after all endif/while/for
	যদি (পরবর্তী_cmdline == ফাঁকা
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
	যদি (cstack.cs_looplevel > 0 && current_সারি < সারিগুলি_ga.ga_len)
	{
	    // Each '|' separated command is stored separately in lines_ga, to
	    // be able to jump to it.  Don't use next_cmdline now.
	    VIM_CLEAR(cmdline_copy);

	    // Check if a function has returned or, unless it has an unclosed
	    // try conditional, aborted.
	    যদি (getline_is_func)
	    {
# ifdef FEAT_PROFILE
		যদি (do_profiling == PROF_YES)
		    func_সারি_end(real_cookie);
# endif
		যদি (func_has_ended(real_cookie))
		{
		    retval = FAIL;
		    বিরতি;
		}
	    }
# ifdef FEAT_PROFILE
	    অন্যথা যদি (do_profiling == PROF_YES
			    && getline_equal(fgetline, cookie, getsourceline))
		script_সারি_end();
# endif

	    // Check if a sourced file hit a ":finish" command.
	    যদি (source_finished(fgetline, cookie))
	    {
		retval = FAIL;
		বিরতি;
	    }

	    // If breakpoints have been added/deleted need to check for it.
	    যদি (breakpoint != ফাঁকা && dbg_tick != ফাঁকা
						   && *dbg_tick != debug_tick)
	    {
		*breakpoint = dbg_find_breakpoint(
				getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }

	    পরবর্তী_cmdline = ((wcmd_T *)(সারিগুলি_ga.ga_তথ্য))[current_সারি].সারি;
	    SOURCING_LNUM = ((wcmd_T *)(সারিগুলি_ga.ga_তথ্য))[current_সারি].lnum;

	    // Did we encounter a breakpoint?
	    যদি (breakpoint != ফাঁকা && *breakpoint != 0
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
	    যদি (do_profiling == PROF_YES)
	    {
		যদি (getline_is_func)
		    func_সারি_start(real_cookie, SOURCING_LNUM);
		অন্যথা যদি (getline_equal(fgetline, cookie, getsourceline))
		    script_সারি_start();
	    }
# endif
	}
#endif

	// 2. If no line given, get an allocated line with fgetline().
	যদি (পরবর্তী_cmdline == ফাঁকা)
	{
	    /*
	     * Need to set msg_didout for the first line after an ":if",
	     * otherwise the ":if" will be overwritten.
	     */
	    যদি (গুনতি == 1 && getline_equal(fgetline, cookie, getexline))
		বার্তা_didout = TRUE;
	    যদি (fgetline == ফাঁকা || (পরবর্তী_cmdline = fgetline(':', cookie,
#ifdef FEAT_EVAL
		    cstack.cs_idx < 0 ? 0 : (cstack.cs_idx + 1) * 2
#অন্যথা
		    0
#endif
		    , in_vim9script() ? GETLINE_CONCAT_CONTBAR
					       : GETLINE_CONCAT_CONT)) == ফাঁকা)
	    {
		// Don't call wait_return() for aborted command line.  The NULL
		// returned for the end of a sourced file or executed function
		// doesn't do this.
		যদি (চাবি_Typed && !(flags & DOCMD_REPEAT))
		    need_wait_return = FALSE;
		retval = FAIL;
		বিরতি;
	    }
	    used_getline = TRUE;

	    /*
	     * Keep the first typed line.  Clear it when more lines are typed.
	     */
	    যদি (flags & DOCMD_KEEPLINE)
	    {
		vim_free(repeat_cmdline);
		যদি (গুনতি == 0)
		    repeat_cmdline = vim_strsave(পরবর্তী_cmdline);
		অন্যথা
		    repeat_cmdline = ফাঁকা;
	    }
	}

	// 3. Make a copy of the command so we can mess with it.
	অন্যথা যদি (cmdline_copy == ফাঁকা)
	{
	    পরবর্তী_cmdline = vim_strsave(পরবর্তী_cmdline);
	    যদি (পরবর্তী_cmdline == ফাঁকা)
	    {
		emsg(_(e_out_of_memory));
		retval = FAIL;
		বিরতি;
	    }
	}
	cmdline_copy = পরবর্তী_cmdline;

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
	যদি ((cstack.cs_looplevel > 0 || has_loop_cmd(পরবর্তী_cmdline)))
	{
	    cmd_getline = get_loop_সারি;
	    cmd_cookie = (শূন্য *)&cmd_loop_cookie;
	    cmd_loop_cookie.সারিগুলি_gap = &সারিগুলি_ga;
	    cmd_loop_cookie.current_সারি = current_সারি;
	    cmd_loop_cookie.lc_getline = fgetline;
	    cmd_loop_cookie.cookie = cookie;
	    cmd_loop_cookie.repeating = (current_সারি < সারিগুলি_ga.ga_len);

	    // Save the current line when encountering it the first time.
	    যদি (current_সারি == সারিগুলি_ga.ga_len
		    && store_loop_সারি(&সারিগুলি_ga, পরবর্তী_cmdline) == FAIL)
	    {
		retval = FAIL;
		বিরতি;
	    }
	    current_সারি_before = current_সারি;
	}
	অন্যথা
	{
	    cmd_getline = fgetline;
	    cmd_cookie = cookie;
	}

	did_endif = FALSE;
#endif

	যদি (গুনতি++ == 0)
	{
	    /*
	     * All output from the commands is put below each other, without
	     * waiting for a return. Don't do this when executing commands
	     * from a script or when being called recursive (e.g. for ":e
	     * +command file").
	     */
	    যদি (!(flags & DOCMD_NOWAIT) && !recursive)
	    {
		বার্তা_didout_before_start = বার্তা_didout;
		বার্তা_didany = FALSE; // no output yet
		বার্তা_start();
		বার্তা_scroll = TRUE;  // put messages below each other
		++no_wait_return;   // don't wait for return until finished
		++RedrawingDisabled;
		did_inc_RedrawingDisabled = TRUE;
	    }
	}

	যদি ((p_verbose >= 15 && SOURCING_NAME != ফাঁকা) || p_verbose >= 16)
	    বার্তা_verbose_cmd(SOURCING_LNUM, cmdline_copy);

	/*
	 * 2. Execute one '|' separated command.
	 *    do_one_cmd() will return NULL if there is no trailing '|'.
	 *    "cmdline_copy" can change, e.g. for '%' and '#' expansion.
	 */
	++recursive;
	পরবর্তী_cmdline = do_one_cmd(&cmdline_copy, flags,
#ifdef FEAT_EVAL
				&cstack,
#endif
				cmd_getline, cmd_cookie);
	--recursive;

#ifdef FEAT_EVAL
	যদি (cmd_cookie == (শূন্য *)&cmd_loop_cookie)
	    // Use "current_line" from "cmd_loop_cookie", it may have been
	    // incremented when defining a function.
	    current_সারি = cmd_loop_cookie.current_সারি;
#endif

	যদি (পরবর্তী_cmdline == ফাঁকা)
	{
	    VIM_CLEAR(cmdline_copy);

	    /*
	     * If the command was typed, remember it for the ':' register.
	     * Do this AFTER executing the command to make :@: work.
	     */
	    যদি (getline_equal(fgetline, cookie, getexline)
						  && new_last_cmdline != ফাঁকা)
	    {
		vim_free(last_cmdline);
		last_cmdline = new_last_cmdline;
		new_last_cmdline = ফাঁকা;
	    }
	}
	অন্যথা
	{
	    // need to copy the command after the '|' to cmdline_copy, for the
	    // next do_one_cmd()
	    STRMOVE(cmdline_copy, পরবর্তী_cmdline);
	    পরবর্তী_cmdline = cmdline_copy;
	}


#ifdef FEAT_EVAL
	// reset did_emsg for a function that is not aborted by an error
	যদি (did_emsg && !force_abort
		&& getline_equal(fgetline, cookie, get_func_সারি)
					      && !func_has_abort(real_cookie))
	{
	    // did_emsg_cumul is not set here
	    did_emsg = FALSE;
	}

	যদি (cstack.cs_looplevel > 0)
	{
	    ++current_সারি;

	    /*
	     * An ":endwhile", ":endfor" and ":continue" is handled here.
	     * If we were executing commands, jump back to the ":while" or
	     * ":for".
	     * If we were not executing commands, decrement cs_looplevel.
	     */
	    যদি (cstack.cs_lflags & (CSL_HAD_CONT | CSL_HAD_ENDLOOP))
	    {
		cstack.cs_lflags &= ~(CSL_HAD_CONT | CSL_HAD_ENDLOOP);

		// Jump back to the matching ":while" or ":for".  Be careful
		// not to use a cs_line[] from an entry that isn't a ":while"
		// or ":for": It would make "current_line" invalid and can
		// cause a crash.
		যদি (!did_emsg && !got_int && !did_throw
			&& cstack.cs_idx >= 0
			&& (cstack.cs_flags[cstack.cs_idx]
						      & (CSF_WHILE | CSF_FOR))
			&& cstack.cs_সারি[cstack.cs_idx] >= 0
			&& (cstack.cs_flags[cstack.cs_idx] & CSF_ACTIVE))
		{
		    current_সারি = cstack.cs_সারি[cstack.cs_idx];
						// remember we jumped there
		    cstack.cs_lflags |= CSL_HAD_LOOP;
		    সারি_breakcheck();		// check if CTRL-C typed

		    // Check for the next breakpoint at or after the ":while"
		    // or ":for".
		    যদি (breakpoint != ফাঁকা && সারিগুলি_ga.ga_len > current_সারি)
		    {
			*breakpoint = dbg_find_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
									fname,
			   ((wcmd_T *)সারিগুলি_ga.ga_তথ্য)[current_সারি].lnum-1);
			*dbg_tick = debug_tick;
		    }
		}
		অন্যথা
		{
		    // can only get here with ":endwhile" or ":endfor"
		    যদি (cstack.cs_idx >= 0)
			rewind_conditionals(&cstack, cstack.cs_idx - 1,
				   CSF_WHILE | CSF_FOR, &cstack.cs_looplevel);
		}
	    }

	    /*
	     * For a ":while" or ":for" we need to remember the line number.
	     */
	    অন্যথা যদি (cstack.cs_lflags & CSL_HAD_LOOP)
	    {
		cstack.cs_lflags &= ~CSL_HAD_LOOP;
		cstack.cs_সারি[cstack.cs_idx] = current_সারি_before;
	    }
	}

	// Check for the next breakpoint after a watchexpression
	যদি (breakpoint != ফাঁকা && has_watchexpr())
	{
	    *breakpoint = dbg_find_breakpoint(FALSE, fname, SOURCING_LNUM);
	    *dbg_tick = debug_tick;
	}

	/*
	 * When not inside any ":while" loop, clear remembered lines.
	 */
	যদি (cstack.cs_looplevel == 0)
	{
	    যদি (সারিগুলি_ga.ga_len > 0)
	    {
		SOURCING_LNUM =
		       ((wcmd_T *)সারিগুলি_ga.ga_তথ্য)[সারিগুলি_ga.ga_len - 1].lnum;
		free_cmdlines(&সারিগুলি_ga);
	    }
	    current_সারি = 0;
	}

	/*
	 * A ":finally" makes did_emsg, got_int, and did_throw pending for
	 * being restored at the ":endtry".  Reset them here and set the
	 * ACTIVE and FINALLY flags, so that the finally clause gets executed.
	 * This includes the case where a missing ":endif", ":endwhile" or
	 * ":endfor" was detected by the ":finally" itself.
	 */
	যদি (cstack.cs_lflags & CSL_HAD_FINA)
	{
	    cstack.cs_lflags &= ~CSL_HAD_FINA;
	    report_make_pending(cstack.cs_pending[cstack.cs_idx]
		    & (CSTP_ERROR | CSTP_INTERRUPT | CSTP_THROW),
		    did_throw ? (শূন্য *)current_exception : ফাঁকা);
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
	যদি (trylevel == 0 && !did_emsg && !got_int && !did_throw)
	    force_abort = FALSE;

	// Convert an interrupt to an exception if appropriate.
	(শূন্য)do_intthrow(&cstack);
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
    যখন (!((got_int
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
	    && (পরবর্তী_cmdline != ফাঁকা
#ifdef FEAT_EVAL
			|| cstack.cs_idx >= 0
#endif
			|| (flags & DOCMD_REPEAT)));

    vim_free(cmdline_copy);
    did_emsg_syntax = FALSE;
#ifdef FEAT_EVAL
    free_cmdlines(&সারিগুলি_ga);
    ga_clear(&সারিগুলি_ga);

    যদি (cstack.cs_idx >= 0)
    {
	/*
	 * If a sourced file or executed function ran to its end, report the
	 * unclosed conditional.
	 * In Vim9 script do not give a second error, executing aborts after
	 * the first one.
	 */
	যদি (!got_int && !did_throw && !aborting()
		&& !(did_emsg && in_vim9script())
		&& ((getline_equal(fgetline, cookie, getsourceline)
			&& !source_finished(fgetline, cookie))
		    || (getline_equal(fgetline, cookie, get_func_সারি)
					    && !func_has_ended(real_cookie))))
	{
	    যদি (cstack.cs_flags[cstack.cs_idx] & CSF_TRY)
		emsg(_(e_missing_endtry));
	    অন্যথা যদি (cstack.cs_flags[cstack.cs_idx] & CSF_WHILE)
		emsg(_(e_missing_endwhile));
	    অন্যথা যদি (cstack.cs_flags[cstack.cs_idx] & CSF_FOR)
		emsg(_(e_missing_endfor));
	    অন্যথা
		emsg(_(e_missing_endif));
	}

	/*
	 * Reset "trylevel" in case of a ":finish" or ":return" or a missing
	 * ":endtry" in a sourced file or executed function.  If the try
	 * conditional is in its finally clause, ignore anything pending.
	 * If it is in a catch clause, finish the caught exception.
	 * Also cleanup any "cs_forinfo" structures.
	 */
	করো
	{
	    পূর্ণসংখ্যা idx = cleanup_conditionals(&cstack, 0, TRUE);

	    যদি (idx >= 0)
		--idx;	    // remove try block not in its finally clause
	    rewind_conditionals(&cstack, idx, CSF_WHILE | CSF_FOR,
							&cstack.cs_looplevel);
	}
	যখন (cstack.cs_idx >= 0);
	trylevel = initial_trylevel;
    }

    // If a missing ":endtry", ":endwhile", ":endfor", or ":endif" or a memory
    // lack was reported above and the error message is to be converted to an
    // exception, do this now after rewinding the cstack.
    do_errthrow(&cstack, getline_equal(fgetline, cookie, get_func_সারি)
				  ? (char_u *)"endfunction" : (char_u *)ফাঁকা);

    যদি (trylevel == 0)
    {
	// Just in case did_throw got set but current_exception wasn't.
	যদি (current_exception == ফাঁকা)
	    did_throw = FALSE;

	/*
	 * When an exception is being thrown out of the outermost try
	 * conditional, discard the uncaught exception, disable the conversion
	 * of interrupts or errors to exceptions, and ensure that no more
	 * commands are executed.
	 */
	যদি (did_throw)
	    handle_did_throw();

	/*
	 * On an interrupt or an aborting error not converted to an exception,
	 * disable the conversion of errors to exceptions.  (Interrupts are not
	 * converted anymore, here.) This enables also the interrupt message
	 * when force_abort is set and did_emsg unset in case of an interrupt
	 * from a finally clause after an error.
	 */
	অন্যথা যদি (got_int || (did_emsg && force_abort))
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
    যদি (did_throw)
	need_rethrow = TRUE;
    যদি ((getline_equal(fgetline, cookie, getsourceline)
		&& ex_nesting_level > source_level(real_cookie))
	    || (getline_equal(fgetline, cookie, get_func_সারি)
		&& ex_nesting_level > func_level(real_cookie) + 1))
    {
	যদি (!did_throw)
	    check_cstack = TRUE;
    }
    অন্যথা
    {
	// When leaving a function, reduce nesting level.
	যদি (getline_equal(fgetline, cookie, get_func_সারি))
	    --ex_nesting_level;
	/*
	 * Go to debug mode when returning from a function in which we are
	 * single-stepping.
	 */
	যদি ((getline_equal(fgetline, cookie, getsourceline)
		    || getline_equal(fgetline, cookie, get_func_সারি))
		&& ex_nesting_level + 1 <= debug_break_level)
	    do_debug(getline_equal(fgetline, cookie, getsourceline)
		    ? (char_u *)_("End of sourced file")
		    : (char_u *)_("End of function"));
    }

    /*
     * Restore the exception environment (done after returning from the
     * debugger).
     */
    যদি (flags & DOCMD_EXCRESET)
	restore_dbg_stuff(&debug_saved);

    বার্তা_list = saved_বার্তা_list;

    // Cleanup if "cs_emsg_silent_list" remains.
    যদি (cstack.cs_emsg_silent_list != ফাঁকা)
    {
	eslist_T *elem, *অস্থায়ী;

	জন্য (elem = cstack.cs_emsg_silent_list; elem != ফাঁকা; elem = অস্থায়ী)
	{
	    অস্থায়ী = elem->পরবর্তী;
	    vim_free(elem);
	}
    }
#endif // FEAT_EVAL

    /*
     * If there was too much output to fit on the command line, ask the user to
     * hit return before redrawing the screen. With the ":global" command we do
     * this only once after the command is finished.
     */
    যদি (did_inc_RedrawingDisabled)
    {
	যদি (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	--no_wait_return;
	বার্তা_scroll = FALSE;

	/*
	 * When just finished an ":if"-":else" which was typed, no need to
	 * wait for hit-return.  Also for an error situation.
	 */
	যদি (retval == FAIL
#ifdef FEAT_EVAL
		|| (did_endif && চাবি_Typed && !did_emsg)
#endif
					    )
	{
	    need_wait_return = FALSE;
	    বার্তা_didany = FALSE;		// don't wait when restarting edit
	}
	অন্যথা যদি (need_wait_return)
	{
	    /*
	     * The msg_start() above clears msg_didout. The wait_return() we do
	     * here should not overwrite the command that may be shown before
	     * doing that.
	     */
	    বার্তা_didout |= বার্তা_didout_before_start;
	    wait_return(FALSE);
	}
    }

#ifdef FEAT_EVAL
    did_endif = FALSE;  // in case do_cmdline used recursively
#অন্যথা
    /*
     * Reset if_level, in case a sourced script file contains more ":if" than
     * ":endif" (could be ":if x | foo | endif").
     */
    if_level = 0;
#endif

    --call_depth;
    ফেরত retval;
}

#যদি defined(FEAT_EVAL)
/*
 * Handle when "did_throw" is set after executing commands.
 */
    শূন্য
handle_did_throw(শূন্য)
{
    অক্ষর	*p = ফাঁকা;
    msglist_T	*messages = ফাঁকা;
    ESTACK_CHECK_DECLARATION;

    /*
     * If the uncaught exception is a user exception, report it as an
     * error.  If it is an error exception, display the saved error
     * message now.  For an interrupt exception, do nothing; the
     * interrupt message is given elsewhere.
     */
    নির্বাচন (current_exception->type)
    {
	ক্ষেত্র ET_USER:
	    vim_snprintf((অক্ষর *)IObuff, IOSIZE,
		    _(e_exception_not_caught_str),
		    current_exception->মান);
	    p = (অক্ষর *)vim_strsave(IObuff);
	    বিরতি;
	ক্ষেত্র ET_ERROR:
	    messages = current_exception->messages;
	    current_exception->messages = ফাঁকা;
	    বিরতি;
	ক্ষেত্র ET_INTERRUPT:
	    বিরতি;
    }

    estack_ঠেলো(ETYPE_EXCEPT, current_exception->throw_name,
					current_exception->throw_lnum);
    ESTACK_CHECK_SETUP;
    current_exception->throw_name = ফাঁকা;

    discard_current_exception();	// uses IObuff if 'verbose'

    // If "silent!" is active the uncaught exception is not fatal.
    যদি (emsg_silent == 0)
    {
	suppress_errthrow = TRUE;
	force_abort = TRUE;
    }

    যদি (messages != ফাঁকা)
    {
	করো
	{
	    msglist_T	*পরবর্তী = messages->পরবর্তী;
	    পূর্ণসংখ্যা		save_compiling = estack_compiling;

	    estack_compiling = messages->বার্তা_compiling;
	    emsg(messages->বার্তা);
	    vim_free(messages->বার্তা);
	    vim_free(messages->sfile);
	    vim_free(messages);
	    messages = পরবর্তী;
	    estack_compiling = save_compiling;
	}
	যখন (messages != ফাঁকা);
    }
    অন্যথা যদি (p != ফাঁকা)
    {
	emsg(p);
	vim_free(p);
    }
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_বের_করো();
}

/*
 * Obtain a line when inside a ":while" or ":for" loop.
 */
    স্থির char_u *
get_loop_সারি(পূর্ণসংখ্যা c, শূন্য *cookie, পূর্ণসংখ্যা indent, getline_opt_T options)
{
    কাঠামো loop_cookie	*cp = (কাঠামো loop_cookie *)cookie;
    wcmd_T		*wp;
    char_u		*সারি;

    যদি (cp->current_সারি + 1 >= cp->সারিগুলি_gap->ga_len)
    {
	যদি (cp->repeating)
	    ফেরত ফাঁকা;	// trying to read past ":endwhile"/":endfor"

	// First time inside the ":while"/":for": get line normally.
	যদি (cp->lc_getline == ফাঁকা)
	    সারি = getcmdline(c, 0L, indent, 0);
	অন্যথা
	    সারি = cp->lc_getline(c, cp->cookie, indent, options);
	যদি (সারি != ফাঁকা && store_loop_সারি(cp->সারিগুলি_gap, সারি) == OK)
	    ++cp->current_সারি;

	ফেরত সারি;
    }

    চাবি_Typed = FALSE;
    ++cp->current_সারি;
    wp = (wcmd_T *)(cp->সারিগুলি_gap->ga_তথ্য) + cp->current_সারি;
    SOURCING_LNUM = wp->lnum;
    ফেরত vim_strsave(wp->সারি);
}

/*
 * Store a line in "gap" so that a ":while" loop can execute it again.
 */
    স্থির পূর্ণসংখ্যা
store_loop_সারি(garray_T *gap, char_u *সারি)
{
    যদি (ga_grow(gap, 1) == FAIL)
	ফেরত FAIL;
    ((wcmd_T *)(gap->ga_তথ্য))[gap->ga_len].সারি = vim_strsave(সারি);
    ((wcmd_T *)(gap->ga_তথ্য))[gap->ga_len].lnum = SOURCING_LNUM;
    ++gap->ga_len;
    ফেরত OK;
}

/*
 * Free the lines stored for a ":while" or ":for" loop.
 */
    স্থির শূন্য
free_cmdlines(garray_T *gap)
{
    যখন (gap->ga_len > 0)
    {
	vim_free(((wcmd_T *)(gap->ga_তথ্য))[gap->ga_len - 1].সারি);
	--gap->ga_len;
    }
}
#endif

/*
 * If "fgetline" is get_loop_line(), return TRUE if the getline it uses equals
 * "func".  * Otherwise return TRUE when "fgetline" equals "func".
 */
    পূর্ণসংখ্যা
getline_equal(
    char_u	*(*fgetline)(পূর্ণসংখ্যা, শূন্য *, পূর্ণসংখ্যা, getline_opt_T),
    শূন্য	*cookie UNUSED,		// argument for fgetline()
    char_u	*(*func)(পূর্ণসংখ্যা, শূন্য *, পূর্ণসংখ্যা, getline_opt_T))
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(পূর্ণসংখ্যা, শূন্য *, পূর্ণসংখ্যা, getline_opt_T);
    কাঠামো loop_cookie *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // function that's originally used to obtain the lines.  This may be
    // nested several levels.
    gp = fgetline;
    cp = (কাঠামো loop_cookie *)cookie;
    যখন (gp == get_loop_সারি)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    ফেরত gp == func;
#অন্যথা
    ফেরত fgetline == func;
#endif
}

/*
 * If "fgetline" is get_loop_line(), return the cookie used by the original
 * getline function.  Otherwise return "cookie".
 */
    শূন্য *
getline_cookie(
    char_u	*(*fgetline)(পূর্ণসংখ্যা, শূন্য *, পূর্ণসংখ্যা, getline_opt_T) UNUSED,
    শূন্য	*cookie)		// argument for fgetline()
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(পূর্ণসংখ্যা, শূন্য *, পূর্ণসংখ্যা, getline_opt_T);
    কাঠামো loop_cookie  *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (কাঠামো loop_cookie *)cookie;
    যখন (gp == get_loop_সারি)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    ফেরত cp;
#অন্যথা
    ফেরত cookie;
#endif
}

#যদি defined(FEAT_EVAL)
/*
 * Get the next line source line without advancing.
 */
    char_u *
getline_peek(
    char_u	*(*fgetline)(পূর্ণসংখ্যা, শূন্য *, পূর্ণসংখ্যা, getline_opt_T) UNUSED,
    শূন্য	*cookie)		// argument for fgetline()
{
    char_u		*(*gp)(পূর্ণসংখ্যা, শূন্য *, পূর্ণসংখ্যা, getline_opt_T);
    কাঠামো loop_cookie  *cp;
    wcmd_T		*wp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (কাঠামো loop_cookie *)cookie;
    যখন (gp == get_loop_সারি)
    {
	যদি (cp->current_সারি + 1 < cp->সারিগুলি_gap->ga_len)
	{
	    // executing lines a second time, use the stored copy
	    wp = (wcmd_T *)(cp->সারিগুলি_gap->ga_তথ্য) + cp->current_সারি + 1;
	    ফেরত wp->সারি;
	}
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    যদি (gp == getsourceline)
	ফেরত source_nextline(cp);
    ফেরত ফাঁকা;
}
#endif


/*
 * Helper function to apply an offset for buffer commands, i.e. ":bdelete",
 * ":bwipeout", etc.
 * Returns the buffer number.
 */
    স্থির পূর্ণসংখ্যা
compute_বাফার_local_গুনতি(পূর্ণসংখ্যা addr_type, পূর্ণসংখ্যা lnum, পূর্ণসংখ্যা offset)
{
    buf_T   *buf;
    buf_T   *nextbuf;
    পূর্ণসংখ্যা     গুনতি = offset;

    buf = firstbuf;
    যখন (buf->b_পরবর্তী != ফাঁকা && buf->b_fnum < lnum)
	buf = buf->b_পরবর্তী;
    যখন (গুনতি != 0)
    {
	গুনতি += (offset < 0) ? 1 : -1;
	nextbuf = (offset < 0) ? buf->b_পূর্ব : buf->b_পরবর্তী;
	যদি (nextbuf == ফাঁকা)
	    বিরতি;
	buf = nextbuf;
	যদি (addr_type == ADDR_LOADED_BUFFERS)
	    // skip over unloaded buffers
	    যখন (buf->b_ml.ml_mfp == ফাঁকা)
	    {
		nextbuf = (offset < 0) ? buf->b_পূর্ব : buf->b_পরবর্তী;
		যদি (nextbuf == ফাঁকা)
		    বিরতি;
		buf = nextbuf;
	    }
    }
    // we might have gone too far, last buffer is not loadedd
    যদি (addr_type == ADDR_LOADED_BUFFERS)
	যখন (buf->b_ml.ml_mfp == ফাঁকা)
	{
	    nextbuf = (offset >= 0) ? buf->b_পূর্ব : buf->b_পরবর্তী;
	    যদি (nextbuf == ফাঁকা)
		বিরতি;
	    buf = nextbuf;
	}
    ফেরত buf->b_fnum;
}

/*
 * Return the window number of "win".
 * When "win" is NULL return the number of windows.
 */
    স্থির পূর্ণসংখ্যা
current_win_nr(win_T *win)
{
    win_T	*wp;
    পূর্ণসংখ্যা		nr = 0;

    FOR_ALL_WINDOWS(wp)
    {
	++nr;
	যদি (wp == win)
	    বিরতি;
    }
    ফেরত nr;
}

    স্থির পূর্ণসংখ্যা
current_tab_nr(tabpage_T *tab)
{
    tabpage_T	*tp;
    পূর্ণসংখ্যা		nr = 0;

    FOR_ALL_TABPAGES(tp)
    {
	++nr;
	যদি (tp == tab)
	    বিরতি;
    }
    ফেরত nr;
}

    স্থির পূর্ণসংখ্যা
comment_start(char_u *p, পূর্ণসংখ্যা starts_with_colon UNUSED)
{
    যদি (in_vim9script())
	ফেরত p[0] == '#' && !starts_with_colon;
    ফেরত *p == '"';
}

#define CURRENT_WIN_NR current_win_nr(curwin)
#define LAST_WIN_NR current_win_nr(ফাঁকা)
#define CURRENT_TAB_NR current_tab_nr(curtab)
#define LAST_TAB_NR current_tab_nr(ফাঁকা)

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
    স্থির char_u *
do_one_cmd(
    char_u	**cmdlinep,
    পূর্ণসংখ্যা		flags,
#ifdef FEAT_EVAL
    cstack_T	*cstack,
#endif
    char_u	*(*fgetline)(পূর্ণসংখ্যা, শূন্য *, পূর্ণসংখ্যা, getline_opt_T),
    শূন্য	*cookie)		// argument for fgetline()
{
    char_u	*p;
    linenr_T	lnum;
    long	n;
    অক্ষর	*errormsg = ফাঁকা;	// error message
    char_u	*after_modifier = ফাঁকা;
    exarg_T	ea;			// Ex command arguments
    cmdmod_T	save_cmdmod;
    পূর্ণসংখ্যা		save_reg_executing = reg_executing;
    পূর্ণসংখ্যা		save_pending_end_reg_executing = pending_end_reg_executing;
    পূর্ণসংখ্যা		ni;			// set when Not Implemented
    char_u	*cmd;
    পূর্ণসংখ্যা		starts_with_colon = FALSE;
    পূর্ণসংখ্যা		may_have_range;
#ifdef FEAT_EVAL
    পূর্ণসংখ্যা		did_set_রাশিমালা_সারি = FALSE;
#endif
    পূর্ণসংখ্যা		sourcing = flags & DOCMD_VERBOSE;
    পূর্ণসংখ্যা		did_append_cmd = FALSE;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
#ifdef FEAT_EVAL
    ++ex_nesting_level;
#endif

    // When the last file has not been edited :q has to be typed twice.
    যদি (quitmore
#ifdef FEAT_EVAL
	    // avoid that a function call in 'statusline' does this
	    && !getline_equal(fgetline, cookie, get_func_সারি)
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
    যদি ((*cmdlinep)[0] == '#' && (*cmdlinep)[1] == '!')
	যাও doend;

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
    যদি (parse_command_modifiers(&ea, &errormsg, &cmdmod, FALSE) == FAIL)
	যাও doend;
    apply_cmdmod(&cmdmod);
    after_modifier = ea.cmd;

#ifdef FEAT_EVAL
    ea.skip = did_emsg || got_int || did_throw || (cstack->cs_idx >= 0
			 && !(cstack->cs_flags[cstack->cs_idx] & CSF_ACTIVE));
#অন্যথা
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
    পূর্ণসংখ্যা vim9script = in_vim9script();
    যদি (vim9script && (flags & DOCMD_RANGEOK) == 0)
    {
	may_have_range = FALSE;
	জন্য (p = ea.cmd; p >= *cmdlinep; --p)
	{
	    যদি (*p == ':')
		may_have_range = TRUE;
	    যদি (p < ea.cmd && !VIM_ISWHITE(*p))
		বিরতি;
	}
    }
    অন্যথা
	may_have_range = TRUE;
    যদি (may_have_range)
	ea.cmd = skip_range(ea.cmd, TRUE, ফাঁকা);

#ifdef FEAT_EVAL
    // Handle ":export" - it functions almost like a command modifier.
    // ":export var Name: type"
    // ":export def Name(..."
    // etc.
    যদি (vim9script && checkforcmd_noparen(&ea.cmd, "export", 6))
	is_export = TRUE;
#endif

    যদি (vim9script && !may_have_range)
    {
	যদি (ea.cmd == cmd + 1 && *cmd == '$')
	    // should be "$VAR = val"
	    --ea.cmd;
#ifdef FEAT_EVAL
	p = find_ex_command(&ea, ফাঁকা, lookup_scriptitem, ফাঁকা);
#অন্যথা
	p = find_ex_command(&ea, ফাঁকা, ফাঁকা, ফাঁকা);
#endif
	যদি (ea.cmdidx == CMD_আকার)
	{
	    char_u *ar = skip_range(ea.cmd, TRUE, ফাঁকা);

	    // If a ':' before the range is missing, give a clearer error
	    // message.
	    যদি (ar > ea.cmd && !ea.skip)
	    {
		semsg(_(e_colon_required_before_range_str), ea.cmd);
		যাও doend;
	    }
	}
    }
    অন্যথা
	p = find_ex_command(&ea, ফাঁকা, ফাঁকা, ফাঁকা);

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    // Count this line for profiling if skip is TRUE.
    যদি (do_profiling == PROF_YES
	    && (!ea.skip || cstack->cs_idx == 0 || (cstack->cs_idx > 0
		     && (cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE))))
    {
	পূর্ণসংখ্যা skip = did_emsg || got_int || did_throw;

	যদি (ea.cmdidx == CMD_catch)
	    skip = !skip && !(cstack->cs_idx >= 0
			  && (cstack->cs_flags[cstack->cs_idx] & CSF_THROWN)
			  && !(cstack->cs_flags[cstack->cs_idx] & CSF_CAUGHT));
	অন্যথা যদি (ea.cmdidx == CMD_else || ea.cmdidx == CMD_elseif)
	    skip = skip || !(cstack->cs_idx >= 0
			  && !(cstack->cs_flags[cstack->cs_idx]
						  & (CSF_ACTIVE | CSF_TRUE)));
	অন্যথা যদি (ea.cmdidx == CMD_finally)
	    skip = FALSE;
	অন্যথা যদি (ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endtry
		&& ea.cmdidx != CMD_endwhile)
	    skip = ea.skip;

	যদি (!skip)
	{
	    যদি (getline_equal(fgetline, cookie, get_func_সারি))
		func_সারি_exec(getline_cookie(fgetline, cookie));
	    অন্যথা যদি (getline_equal(fgetline, cookie, getsourceline))
		script_সারি_exec();
	}
    }
# endif
#endif

    ea.cmd = cmd;

#ifdef FEAT_EVAL
    // May go to debug mode.  If this happens and the ">quit" debug command is
    // used, throw an interrupt exception and skip the next command.
    dbg_check_breakpoint(&ea);
    যদি (!ea.skip && got_int)
    {
	ea.skip = TRUE;
	(শূন্য)do_intthrow(cstack);
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
    যদি (!IS_USER_CMDIDX(ea.cmdidx))
    {
	যদি (ea.cmdidx != CMD_আকার)
	    ea.addr_type = cmdnames[(পূর্ণসংখ্যা)ea.cmdidx].cmd_addr_type;
	অন্যথা
	    ea.addr_type = ADDR_সারিগুলি;

	// :wincmd range depends on the argument.
	যদি (ea.cmdidx == CMD_wincmd && p != ফাঁকা)
	    get_wincmd_addr_type(skipwhite(p), &ea);
#ifdef FEAT_QUICKFIX
	// :.cc in quickfix window uses line number
	যদি ((ea.cmdidx == CMD_cc || ea.cmdidx == CMD_ll) && bt_quickfix(curbuf))
	    ea.addr_type = ADDR_OTHER;
#endif
    }

    যদি (!may_have_range)
	ea.line1 = ea.line2 = default_address(&ea);
    অন্যথা যদি (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
	যাও doend;

/*
 * 5. Parse the command.
 */

    /*
     * Skip ':' and any white space
     */
    ea.cmd = skipwhite(ea.cmd);
    যখন (*ea.cmd == ':')
	ea.cmd = skipwhite(ea.cmd + 1);

    /*
     * If we got a line, but no command, then go to the line.
     * If we find a '|' or '\n' we set ea.nextcmd.
     */
    যদি (*ea.cmd == NUL || comment_start(ea.cmd, starts_with_colon)
			       || (ea.nextcmd = check_nextcmd(ea.cmd)) != ফাঁকা)
    {
	/*
	 * strange vi behaviour:
	 * ":3"		jumps to line 3
	 * ":3|..."	prints line 3  (not in Vim9 script)
	 * ":|"		prints current line  (not in Vim9 script)
	 */
	যদি (ea.skip)	    // skip this if inside :if
	    যাও doend;
	errormsg = ex_range_without_command(&ea);
	যাও doend;
    }

    // If this looks like an undefined user command and there are CmdUndefined
    // autocommands defined, trigger the matching autocommands.
    যদি (p != ফাঁকা && ea.cmdidx == CMD_আকার && !ea.skip
	    && ASCII_ISUPPER(*ea.cmd)
	    && has_cmdundefined())
    {
	পূর্ণসংখ্যা ret;

	p = ea.cmd;
	যখন (ASCII_ISALNUM(*p))
	    ++p;
	p = vim_strnsave(ea.cmd, p - ea.cmd);
	ret = apply_autocmds(EVENT_CMDUNDEFINED, p, p, TRUE, ফাঁকা);
	vim_free(p);
	// If the autocommands did something and didn't cause an error, try
	// finding the command again.
	p = (ret
#ifdef FEAT_EVAL
		&& !aborting()
#endif
		) ? find_ex_command(&ea, ফাঁকা, ফাঁকা, ফাঁকা) : ea.cmd;
    }

    যদি (p == ফাঁকা)
    {
	যদি (!ea.skip)
	    errormsg = _(e_ambiguous_use_of_user_defined_command);
	যাও doend;
    }
    // Check for wrong commands.
    যদি (*p == '!' && ea.cmd[1] == 0151 && ea.cmd[0] == 78
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	errormsg = uc_fun_cmd();
	যাও doend;
    }

    যদি (ea.cmdidx == CMD_আকার)
    {
	যদি (!ea.skip)
	{
	    STRCPY(IObuff, _(e_not_an_editor_command));
	    যদি (!sourcing)
	    {
		// If the modifier was parsed OK the error must be in the
		// following command
		যদি (after_modifier != ফাঁকা)
		    append_command(after_modifier);
		অন্যথা
		    append_command(*cmdlinep);
		did_append_cmd = TRUE;
	    }
	    errormsg = (অক্ষর *)IObuff;
	    did_emsg_syntax = TRUE;
	}
	যাও doend;
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
    যদি (ea.cmdidx == CMD_if)
	++if_level;
    যদি (if_level)
    {
	যদি (ea.cmdidx == CMD_endif)
	    --if_level;
	যাও doend;
    }

#endif

    // forced commands
    যদি (*p == '!' && ea.cmdidx != CMD_substitute
	    && ea.cmdidx != CMD_smagic && ea.cmdidx != CMD_snomagic)
    {
	++p;
	ea.forceit = TRUE;
    }
    অন্যথা
	ea.forceit = FALSE;

/*
 * 6. Parse arguments.  Then check for errors.
 */
    যদি (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = (long)cmdnames[(পূর্ণসংখ্যা)ea.cmdidx].cmd_argt;

    যদি (!ea.skip)
    {
#ifdef HAVE_SANDBOX
	যদি (sandbox != 0 && !(ea.argt & EX_SBOXOK))
	{
	    // Command not allowed in sandbox.
	    errormsg = _(e_not_allowed_in_sandbox);
	    যাও doend;
	}
#endif
	যদি (restricted != 0 && (ea.argt & EX_RESTRICT))
	{
	    errormsg = _(e_command_not_allowed_in_rvim);
	    যাও doend;
	}
	যদি (!curbuf->b_p_ma && (ea.argt & EX_MODIFY))
	{
	    // Command not allowed in non-'modifiable' buffer
	    errormsg = _(e_cannot_make_changes_modifiable_is_off);
	    যাও doend;
	}

	যদি (!IS_USER_CMDIDX(ea.cmdidx))
	{
	    যদি (cmdwin_type != 0 && !(ea.argt & EX_CMDWIN))
	    {
		// Command not allowed in the command line window
		errormsg = _(e_invalid_in_cmdline_window);
		যাও doend;
	    }
	    যদি (text_locked() && !(ea.argt & EX_LOCK_OK))
	    {
		// Command not allowed when text is locked
		errormsg = _(get_text_locked_বার্তা());
		যাও doend;
	    }
	}

	// Disallow editing another buffer when "curbuf_lock" is set.
	// Do allow ":checktime" (it is postponed).
	// Do allow ":edit" (check for an argument later).
	// Do allow ":file" with no arguments (check for an argument later).
	যদি (!(ea.argt & (EX_CMDWIN | EX_LOCK_OK))
		&& ea.cmdidx != CMD_checktime
		&& ea.cmdidx != CMD_edit
		&& ea.cmdidx != CMD_file
		&& !IS_USER_CMDIDX(ea.cmdidx)
		&& curbuf_locked())
	    যাও doend;

	যদি (!ni && !(ea.argt & EX_RANGE) && ea.addr_গুনতি > 0)
	{
	    errormsg = _(e_no_range_allowed);
	    যাও doend;
	}
    }

    যদি (!ni && !(ea.argt & EX_BANG) && ea.forceit)
    {
	errormsg = _(e_no_bang_allowed);
	যাও doend;
    }

    /*
     * Don't complain about the range if it is not used
     * (could happen if line_count is accidentally set to 0).
     */
    যদি (!ea.skip && !ni && (ea.argt & EX_RANGE))
    {
	/*
	 * If the range is backwards, ask for confirmation and, if given, swap
	 * ea.line1 & ea.line2 so it's forwards again.
	 * When global command is busy, don't ask, will fail below.
	 */
	যদি (!global_busy && ea.line1 > ea.line2)
	{
	    যদি (বার্তা_silent == 0)
	    {
		যদি (sourcing || exmode_active)
		{
		    errormsg = _(e_backwards_range_given);
		    যাও doend;
		}
		যদি (ask_yesno((char_u *)
			_("Backwards range given, OK to swap"), FALSE) != 'y')
		    যাও doend;
	    }
	    lnum = ea.line1;
	    ea.line1 = ea.line2;
	    ea.line2 = lnum;
	}
	যদি ((errormsg = invalid_range(&ea)) != ফাঁকা)
	    যাও doend;
    }

    যদি ((ea.addr_type == ADDR_OTHER) && ea.addr_গুনতি == 0)
	// default is 1, not cursor
	ea.line2 = 1;

    correct_range(&ea);

#ifdef FEAT_FOLDING
    যদি (((ea.argt & EX_WHOLEFOLD) || ea.addr_গুনতি >= 2) && !global_busy
	    && ea.addr_type == ADDR_সারিগুলি)
    {
	// Put the first line at the start of a closed fold, put the last line
	// at the end of a closed fold.
	(শূন্য)hasFolding(ea.line1, &ea.line1, ফাঁকা);
	(শূন্য)hasFolding(ea.line2, ফাঁকা, &ea.line2);
    }
#endif

#ifdef FEAT_QUICKFIX
    /*
     * For the ":make" and ":grep" commands we insert the 'makeprg'/'grepprg'
     * option here, so things like % get expanded.
     */
    p = replace_makeprg(&ea, p, cmdlinep);
    যদি (p == ফাঁকা)
	যাও doend;
#endif

    /*
     * Skip to start of argument.
     * Don't do this for the ":!" command, because ":!! -l" needs the space.
     */
    যদি (ea.cmdidx == CMD_bang)
	ea.arg = p;
    অন্যথা
	ea.arg = skipwhite(p);

    // ":file" cannot be run with an argument when "curbuf_lock" is set
    যদি (ea.cmdidx == CMD_file && *ea.arg != NUL && curbuf_locked())
	যাও doend;

    /*
     * Check for "++opt=val" argument.
     * Must be first, allow ":w ++enc=utf8 !cmd"
     */
    যদি (ea.argt & EX_ARGOPT)
	যখন (ea.arg[0] == '+' && ea.arg[1] == '+')
	    যদি (getargopt(&ea) == FAIL && !ni)
	    {
		errormsg = _(e_invalid_argument);
		যাও doend;
	    }

    যদি (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	যদি (*ea.arg == '>')			// append
	{
	    যদি (*++ea.arg != '>')		// typed wrong
	    {
		errormsg = _(e_use_w_or_w_gt_gt);
		যাও doend;
	    }
	    ea.arg = skipwhite(ea.arg + 1);
	    ea.append = TRUE;
	}
	অন্যথা যদি (*ea.arg == '!' && ea.cmdidx == CMD_write)  // :w !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    যদি (ea.cmdidx == CMD_read)
    {
	যদি (ea.forceit)
	{
	    ea.usefilter = TRUE;		// :r! filter if ea.forceit
	    ea.forceit = FALSE;
	}
	অন্যথা যদি (*ea.arg == '!')		// :r !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    যদি (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ea.amount = 1;
	যখন (*ea.arg == *ea.cmd)		// count number of '>' or '<'
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
    যদি ((ea.argt & EX_CMDARG) && !ea.usefilter)
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
    যদি ((ea.argt & EX_TRLBAR) && !ea.usefilter)
    {
	separate_nextcmd(&ea, FALSE);
    }
    অন্যথা যদি (ea.cmdidx == CMD_bang
	    || ea.cmdidx == CMD_terminal
	    || ea.cmdidx == CMD_global
	    || ea.cmdidx == CMD_vglobal
	    || ea.usefilter
#ifdef FEAT_EVAL
	    || inside_block(&ea)
#endif
	    )
    {
	জন্য (p = ea.arg; *p; ++p)
	{
	    // Remove one backslash before a newline
	    যদি (*p == '\\' && p[1] == '\n')
		STRMOVE(p, p + 1);
	    অন্যথা যদি (*p == '\n' && !(ea.argt & EX_রাশিমালা_ARG))
	    {
		ea.nextcmd = p + 1;
		*p = NUL;
		বিরতি;
	    }
	}
    }

    যদি ((ea.argt & EX_DFLALL) && ea.addr_গুনতি == 0)
	address_default_all(&ea);

    // accept numbered register only when no count allowed (:put)
    যদি (       (ea.argt & EX_REGSTR)
	    && *ea.arg != NUL
	       // Do not allow register = for user commands
	    && (!IS_USER_CMDIDX(ea.cmdidx) || *ea.arg != '=')
	    && !((ea.argt & EX_গুনতি) && VIM_ISDIGIT(*ea.arg)))
    {
#যদি !defined(FEAT_CLIPBOARD) && !defined(FEAT_CLIPBOARD_PROVIDER)
	// check these explicitly for a more specific error message
	যদি (*ea.arg == '*' || *ea.arg == '+')
	{
	    errormsg = _(e_invalid_register_name);
	    যাও doend;
	}
#endif
	যদি (valid_yank_reg(*ea.arg, (!IS_USER_CMDIDX(ea.cmdidx)
			    && ea.cmdidx != CMD_put && ea.cmdidx != CMD_iput)))
	{
	    ea.regname = *ea.arg++;
#ifdef FEAT_EVAL
	    // for '=' register: accept the rest of the line as an expression
	    যদি (ea.arg[-1] == '=' && ea.arg[0] != NUL)
	    {
		যদি (!ea.skip)
		{
		    set_রাশিমালা_সারি(vim_strsave(ea.arg), &ea);
		    did_set_রাশিমালা_সারি = TRUE;
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
    যদি ((ea.argt & EX_গুনতি) && VIM_ISDIGIT(*ea.arg)
	    && (!(ea.argt & EX_BUFNAME) || *(p = skipdigits(ea.arg + 1)) == NUL
							  || VIM_ISWHITE(*p)))
    {
	n = getdigits_quoted(&ea.arg);
	ea.arg = skipwhite(ea.arg);
	যদি (n <= 0 && !ni && (ea.argt & EX_ZEROR) == 0)
	{
	    errormsg = _(e_positive_গুনতি_required);
	    যাও doend;
	}
	যদি (ea.addr_type != ADDR_সারিগুলি)	// e.g. :buffer 2, :sleep 3
	{
	    ea.line2 = n;
	    যদি (ea.addr_গুনতি == 0)
		ea.addr_গুনতি = 1;
	}
	অন্যথা
	{
	    ea.line1 = ea.line2;
	    যদি (ea.line2 >= LONG_MAX - (n - 1))
		ea.line2 = LONG_MAX;  // avoid overflow
	    অন্যথা
		ea.line2 += n - 1;
	    ++ea.addr_গুনতি;
	    /*
	     * Be vi compatible: no error message for out of range.
	     */
	    যদি (ea.line2 > curbuf->b_ml.ml_সারি_গুনতি)
		ea.line2 = curbuf->b_ml.ml_সারি_গুনতি;
	}
    }

    /*
     * Check for flags: 'l', 'p' and '#'.
     */
    যদি (ea.argt & EX_FLAGS)
	get_flags(&ea);
    যদি (!ni && !(ea.argt & EX_EXTRA) && *ea.arg != NUL
	    && *ea.arg != '"' && (*ea.arg != '|' || (ea.argt & EX_TRLBAR) == 0))
    {
	// no arguments allowed but there is something
	errormsg = ex_errmsg(e_trailing_characters_str, ea.arg);
	যাও doend;
    }

    যদি (!ni && (ea.argt & EX_NEEDARG) && *ea.arg == NUL)
    {
	errormsg = _(e_argument_required);
	যাও doend;
    }

#ifdef FEAT_EVAL
    /*
     * Skip the command when it's not going to be executed.
     * The commands like :if, :endif, etc. always need to be executed.
     * Also make an exception for commands that handle a trailing command
     * themselves.
     */
    যদি (ea.skip)
    {
	নির্বাচন (ea.cmdidx)
	{
	    // commands that need evaluation
	    ক্ষেত্র CMD_while:
	    ক্ষেত্র CMD_endwhile:
	    ক্ষেত্র CMD_for:
	    ক্ষেত্র CMD_endfor:
	    ক্ষেত্র CMD_if:
	    ক্ষেত্র CMD_elseif:
	    ক্ষেত্র CMD_else:
	    ক্ষেত্র CMD_endif:
	    ক্ষেত্র CMD_try:
	    ক্ষেত্র CMD_catch:
	    ক্ষেত্র CMD_finally:
	    ক্ষেত্র CMD_endtry:
	    ক্ষেত্র CMD_function:
	    ক্ষেত্র CMD_def:
				বিরতি;

	    // commands that read a block of lines
	    ক্ষেত্র CMD_abstract:
	    ক্ষেত্র CMD_append:
	    ক্ষেত্র CMD_autocmd:
	    ক্ষেত্র CMD_change:
	    ক্ষেত্র CMD_class:
	    ক্ষেত্র CMD_command:
	    ক্ষেত্র CMD_enum:
	    ক্ষেত্র CMD_ঢোকাও:
	    ক্ষেত্র CMD_interface:
	    ক্ষেত্র CMD_loadkeymap:
				বিরতি;

	    // Commands that handle '|' themselves.  Check: A command should
	    // either have the EX_TRLBAR flag, appear in this list or appear in
	    // the list at ":help :bar".
	    ক্ষেত্র CMD_aboveleft:
	    ক্ষেত্র CMD_and:
	    ক্ষেত্র CMD_belowright:
	    ক্ষেত্র CMD_botright:
	    ক্ষেত্র CMD_browse:
	    ক্ষেত্র CMD_call:
	    ক্ষেত্র CMD_confirm:
	    ক্ষেত্র CMD_const:
	    ক্ষেত্র CMD_delfunction:
	    ক্ষেত্র CMD_djump:
	    ক্ষেত্র CMD_dlist:
	    ক্ষেত্র CMD_dsearch:
	    ক্ষেত্র CMD_dsplit:
	    ক্ষেত্র CMD_echo:
	    ক্ষেত্র CMD_echoerr:
	    ক্ষেত্র CMD_echomsg:
	    ক্ষেত্র CMD_echon:
	    ক্ষেত্র CMD_eval:
	    ক্ষেত্র CMD_execute:
	    ক্ষেত্র CMD_filter:
	    ক্ষেত্র CMD_final:
	    ক্ষেত্র CMD_help:
	    ক্ষেত্র CMD_hide:
	    ক্ষেত্র CMD_horizontal:
	    ক্ষেত্র CMD_ijump:
	    ক্ষেত্র CMD_ilist:
	    ক্ষেত্র CMD_isearch:
	    ক্ষেত্র CMD_isplit:
	    ক্ষেত্র CMD_keepalt:
	    ক্ষেত্র CMD_keepjumps:
	    ক্ষেত্র CMD_keepmarks:
	    ক্ষেত্র CMD_keeppatterns:
	    ক্ষেত্র CMD_leftabove:
	    ক্ষেত্র CMD_let:
	    ক্ষেত্র CMD_lockmarks:
	    ক্ষেত্র CMD_lockvar:
	    ক্ষেত্র CMD_lua:
	    ক্ষেত্র CMD_match:
	    ক্ষেত্র CMD_mzscheme:
	    ক্ষেত্র CMD_noautocmd:
	    ক্ষেত্র CMD_noswapfile:
	    ক্ষেত্র CMD_perl:
	    ক্ষেত্র CMD_psearch:
	    ক্ষেত্র CMD_py3:
	    ক্ষেত্র CMD_python3:
	    ক্ষেত্র CMD_python:
	    ক্ষেত্র CMD_return:
	    ক্ষেত্র CMD_rightbelow:
	    ক্ষেত্র CMD_ruby:
	    ক্ষেত্র CMD_silent:
	    ক্ষেত্র CMD_smagic:
	    ক্ষেত্র CMD_snomagic:
	    ক্ষেত্র CMD_substitute:
	    ক্ষেত্র CMD_syntax:
	    ক্ষেত্র CMD_tab:
	    ক্ষেত্র CMD_tcl:
	    ক্ষেত্র CMD_throw:
	    ক্ষেত্র CMD_tilde:
	    ক্ষেত্র CMD_topleft:
	    ক্ষেত্র CMD_unlet:
	    ক্ষেত্র CMD_unlockvar:
	    ক্ষেত্র CMD_var:
	    ক্ষেত্র CMD_verbose:
	    ক্ষেত্র CMD_vertical:
	    ক্ষেত্র CMD_wincmd:
				বিরতি;

	    পূর্বনির্ধারিত:		যাও doend;
	}
    }
#endif

    যদি ((ea.argt & EX_XFILE)
	    && expand_filename(&ea, cmdlinep, &errormsg) == FAIL)
	যাও doend;

#ifdef FEAT_EVAL
    যদি (is_export && (ea.argt & EX_EXPORT) == 0)
    {
	emsg(_(e_invalid_command_after_export));
	যাও doend;
    }
#endif

    /*
     * Accept buffer name.  Cannot be used at the same time with a buffer
     * number.  Don't do this for a user command.
     */
    যদি ((ea.argt & EX_BUFNAME) && *ea.arg != NUL && ea.addr_গুনতি == 0
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * :bdelete, :bwipeout and :bunload take several arguments, separated
	 * by spaces: find next space (skipping over escaped characters).
	 * The others take one argument: ignore trailing spaces.
	 */
	যদি (ea.cmdidx == CMD_bdelete || ea.cmdidx == CMD_bwipeout
						  || ea.cmdidx == CMD_bunload)
	    p = skiptowhite_esc(ea.arg);
	অন্যথা
	{
	    p = ea.arg + STRLEN(ea.arg);
	    যখন (p > ea.arg && VIM_ISWHITE(p[-1]))
		--p;
	}
	ea.line2 = buflist_findpat(ea.arg, p, (ea.argt & EX_BUFUNL) != 0,
								FALSE, FALSE);
	যদি (ea.line2 < 0)	    // failed
	    যাও doend;
	ea.addr_গুনতি = 1;
	ea.arg = skipwhite(p);
    }

    // The :try command saves the emsg_silent flag, reset it here when
    // ":silent! try" was used, it should only apply to :try itself.
    যদি (ea.cmdidx == CMD_try && cmdmod.cmod_did_esilent > 0)
    {
	emsg_silent -= cmdmod.cmod_did_esilent;
	যদি (emsg_silent < 0)
	    emsg_silent = 0;
	cmdmod.cmod_did_esilent = 0;
    }

/*
 * 7. Execute the command.
 */

    যদি (IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * Execute a user-defined command.
	 */
	do_ucmd(&ea);
    }
    অন্যথা
    {
	/*
	 * Call the function to execute the builtin command.
	 */
	(cmdnames[ea.cmdidx].cmd_func)(&ea);
	যদি (ea.errmsg != ফাঁকা)
	    errormsg = ea.errmsg;
    }

#ifdef FEAT_EVAL
    // A command will reset "is_export" when exporting an item.  If it is still
    // set something went wrong or the command was never executed.
    যদি (!ea.skip && is_export)
    {
	যদি (errormsg == ফাঁকা)
	    errormsg = _(e_export_with_invalid_argument);
	is_export = FALSE;
    }

    // Set flag that any command was executed, used by ex_vim9script().
    // Not if this was a command that wasn't executed or :endif.
    যদি (sourcing_a_script(&ea)
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
    যদি (need_rethrow)
	do_throw(cstack);
    অন্যথা যদি (check_cstack)
    {
	যদি (source_finished(fgetline, cookie))
	    do_finish(&ea, TRUE);
	অন্যথা যদি (getline_equal(fgetline, cookie, get_func_সারি)
						   && current_func_returned())
	    do_return(&ea, TRUE, FALSE, ফাঁকা);
    }
    need_rethrow = check_cstack = FALSE;
#endif

doend:
    যদি (curwin->w_cursor.lnum == 0)	// can happen with zero line number
    {
	curwin->w_cursor.lnum = 1;
	curwin->w_cursor.col = 0;
    }

    যদি (errormsg != ফাঁকা && *errormsg != NUL && !did_emsg)
    {
	যদি ((sourcing || !চাবি_Typed) && !did_append_cmd)
	{
	    যদি (errormsg != (অক্ষর *)IObuff)
	    {
		STRCPY(IObuff, errormsg);
		errormsg = (অক্ষর *)IObuff;
	    }
	    append_command(*cmdlinep);
	}
	emsg(errormsg);
    }
#ifdef FEAT_EVAL
    do_errthrow(cstack,
	    (ea.cmdidx != CMD_আকার && !IS_USER_CMDIDX(ea.cmdidx))
			? cmdnames[(পূর্ণসংখ্যা)ea.cmdidx].cmd_name : (char_u *)ফাঁকা);

    যদি (did_set_রাশিমালা_সারি)
	set_রাশিমালা_সারি(ফাঁকা, ফাঁকা);
    is_export = FALSE;
#endif

    undo_cmdmod(&cmdmod);
    cmdmod = save_cmdmod;
    reg_executing = save_reg_executing;
    pending_end_reg_executing = save_pending_end_reg_executing;

    যদি (ea.nextcmd && *ea.nextcmd == NUL)	// not really a next command
	ea.nextcmd = ফাঁকা;

#ifdef FEAT_EVAL
    --ex_nesting_level;
    vim_free(ea.cmdline_tofree);
#endif

    ফেরত ea.nextcmd;
}

স্থির অক্ষর ex_error_buf[বার্তা_BUF_LEN];

/*
 * Return an error message with argument included.
 * Uses a static buffer, only the last error will be kept.
 * "msg" will be translated, caller should use N_().
 */
    অক্ষর *
ex_errmsg(অক্ষর *বার্তা, char_u *arg)
{
    vim_snprintf(ex_error_buf, বার্তা_BUF_LEN, _(বার্তা), arg);
    ফেরত ex_error_buf;
}

/*
 * The "+" string used in place of an empty command in Ex mode.
 * This string is used in pointer comparison.
 */
স্থির ধ্রুবক অক্ষর exmode_plus[] = "+";

/*
 * Handle a range without a command.
 * Returns an error message on failure.
 */
    অক্ষর *
ex_range_without_command(exarg_T *eap)
{
    অক্ষর *errormsg = ফাঁকা;

    যদি ((*eap->cmd == '|' ||
		(exmode_active && eap->cmd != (char_u *)exmode_plus + 1))
#ifdef FEAT_EVAL
	    && !in_vim9script()
#endif
       )
    {
	eap->cmdidx = CMD_print;
	eap->argt = EX_RANGE+EX_গুনতি+EX_TRLBAR;
	যদি ((errormsg = invalid_range(eap)) == ফাঁকা)
	{
	    correct_range(eap);
	    ex_print(eap);
	}
    }
    অন্যথা যদি (eap->addr_গুনতি != 0)
    {
	যদি (eap->line2 > curbuf->b_ml.ml_সারি_গুনতি)
	{
	    // With '-' in 'cpoptions' a line number past the file is an
	    // error, otherwise put it at the end of the file.
	    যদি (vim_strchr(p_cpo, CPO_MINUS) != ফাঁকা)
		eap->line2 = -1;
	    অন্যথা
		eap->line2 = curbuf->b_ml.ml_সারি_গুনতি;
	}

	যদি (eap->line2 < 0)
	    errormsg = _(e_invalid_range);
	অন্যথা
	{
	    যদি (eap->line2 == 0)
		curwin->w_cursor.lnum = 1;
	    অন্যথা
		curwin->w_cursor.lnum = eap->line2;
	    beginline(BL_SOL | BL_FIX);
	}
    }
    ফেরত errormsg;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 * If "noparen" is TRUE do not recognize the command followed by "(" or ".".
 */
    স্থির পূর্ণসংখ্যা
checkforcmd_opt(
    char_u	**pp,		// start of command
    অক্ষর	*cmd,		// name of command
    পূর্ণসংখ্যা		len,		// required length
    পূর্ণসংখ্যা		noparen)
{
    পূর্ণসংখ্যা		i;

    জন্য (i = 0; cmd[i] != NUL; ++i)
	যদি (((char_u *)cmd)[i] != (*pp)[i])
	    বিরতি;
    যদি (i >= len && !ASCII_ISALPHA((*pp)[i]) && (*pp)[i] != '_'
			 && (!noparen || ((*pp)[i] != '(' && (*pp)[i] != '.')))
    {
	*pp = skipwhite(*pp + i);
	ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    পূর্ণসংখ্যা
checkforcmd(
    char_u	**pp,		// start of command
    অক্ষর	*cmd,		// name of command
    পূর্ণসংখ্যা		len)		// required length
{
    ফেরত checkforcmd_opt(pp, cmd, len, FALSE);
}

/*
 * Check for an Ex command with optional tail, not followed by "(" or ".".
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    পূর্ণসংখ্যা
checkforcmd_noparen(
    char_u	**pp,		// start of command
    অক্ষর	*cmd,		// name of command
    পূর্ণসংখ্যা		len)		// required length
{
    ফেরত checkforcmd_opt(pp, cmd, len, TRUE);
}

/*
 * Find the replacement text of a ":command", the part after the attributes and
 * the command name.  Returns NULL when there is none.  Does not modify "arg"
 * and gives no error messages.
 */
    স্থির char_u *
find_ucmd_repl(char_u *arg)
{
    char_u	*p = arg;

    // Skip over the attributes.
    যখন (*p == '-')
	p = skipwhite(skiptowhite(p));

    // Skip over the command name.
    যদি (!ASCII_ISALPHA(*p))
	ফেরত ফাঁকা;
    যখন (ASCII_ISALNUM(*p))
	++p;

    ফেরত *p == NUL ? ফাঁকা : skipwhite(p);
}

/*
 * Find the "{" in "line" that starts a block for ":command" or ":autocmd".
 * That is the case when the command argument is "{" at the end of the line,
 * also when the command is nested in another ":command" or ":autocmd".
 * Returns NULL when the line does not start such a block.
 */
    char_u *
find_cmd_block_start(char_u *সারি)
{
    char_u	*p = skipwhite(সারি);

    জন্য (;;)
    {
	char_u	*arg = p;

	যদি (*p == '{' && ends_excmd2(p, skipwhite(p + 1)))
	    ফেরত p;

	যদি (checkforcmd_noparen(&arg, "autocmd", 2))
	{
	    যদি (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = au_find_cmd_arg(arg);
	}
	অন্যথা যদি (checkforcmd_noparen(&arg, "command", 3))
	{
	    যদি (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = find_ucmd_repl(arg);
	}
	অন্যথা
	    ফেরত ফাঁকা;

	যদি (p == ফাঁকা)
	    ফেরত ফাঁকা;
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
    পূর্ণসংখ্যা
parse_command_modifiers(
	exarg_T	    *eap,
	অক্ষর	    **errormsg,
	cmdmod_T    *cmod,
	পূর্ণসংখ্যা	    skip_only)
{
    char_u  *orig_cmd = eap->cmd;
    char_u  *cmd_start = ফাঁকা;
    পূর্ণসংখ্যা	    use_plus_cmd = FALSE;
    পূর্ণসংখ্যা	    starts_with_colon = FALSE;
    পূর্ণসংখ্যা	    vim9script = in_vim9script();
    পূর্ণসংখ্যা	    has_visual_range = FALSE;

    CLEAR_POINTER(cmod);
    cmod->cmod_flags = sticky_cmdmod_flags;

    যদি (STRNCMP(eap->cmd, "'<,'>", 5) == 0)
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
    জন্য (;;)
    {
	char_u  *p;

	যখন (*eap->cmd == ' ' || *eap->cmd == '\t' || *eap->cmd == ':')
	{
	    যদি (*eap->cmd == ':')
		starts_with_colon = TRUE;
	    ++eap->cmd;
	}

	// in ex mode, an empty command (after modifiers) works like :+
	যদি (*eap->cmd == NUL && exmode_active
		   && (getline_equal(eap->ea_getline, eap->cookie, getexmodeline)
		       || getline_equal(eap->ea_getline, eap->cookie, getexline))
			&& curwin->w_cursor.lnum < curbuf->b_ml.ml_সারি_গুনতি)
	{
	    use_plus_cmd = TRUE;
	    যদি (!skip_only)
		ex_pressedreturn = TRUE;
	    বিরতি;  // no modifiers following
	}

	// ignore comment and empty lines
	যদি (comment_start(eap->cmd, starts_with_colon))
	{
	    // a comment ends at a NL
	    eap->nextcmd = vim_strchr(eap->cmd, '\n');
	    যদি (eap->nextcmd != ফাঁকা)
		++eap->nextcmd;
	    যদি (vim9script)
	    {
		যদি (has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
#ifdef FEAT_EVAL
		যদি (eap->cmd[0] == '#' && eap->cmd[1] == '{'
							 && eap->cmd[2] != '{')
		    *errormsg = _(e_cannot_use_hash_curly_to_start_comment);
#endif
	    }
	    ফেরত FAIL;
	}
	যদি (*eap->cmd == '\n')
	{
	    eap->nextcmd = eap->cmd + 1;
	    ফেরত FAIL;
	}
	যদি (*eap->cmd == NUL)
	{
	    যদি (!skip_only)
	    {
		ex_pressedreturn = TRUE;
		যদি (vim9script && has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
	    }
	    ফেরত FAIL;
	}

	p = skip_range(eap->cmd, TRUE, ফাঁকা);

	// In Vim9 script a variable can shadow a command modifier:
	//   verbose = 123
	//   verbose += 123
	//   silent! verbose = func()
	//   verbose.member = 2
	//   verbose[expr] = 2
	// But not:
	//   verbose [a, b] = list
	যদি (vim9script)
	{
	    char_u *s, *n;

	    জন্য (s = eap->cmd; ASCII_ISALPHA(*s); ++s)
		;
	    n = skipwhite(s);
	    যদি (*n == '.' || *n == '=' || (*n != NUL && n[1] == '=')
		    || *s == '[')
		বিরতি;
	}

	নির্বাচন (*p)
	{
	    // When adding an entry, also modify cmdmod_info_tab[].
	    ক্ষেত্র 'a':	যদি (!checkforcmd_noparen(&eap->cmd, "aboveleft", 3))
			    বিরতি;
			cmod->cmod_split |= WSP_ABOVE;
			চালু;

	    ক্ষেত্র 'b':	যদি (checkforcmd_noparen(&eap->cmd, "belowright", 3))
			{
			    cmod->cmod_split |= WSP_BELOW;
			    চালু;
			}
			যদি (checkforcmd_opt(&eap->cmd, "browse", 3, TRUE))
			{
#ifdef FEAT_BROWSE_CMD
			    cmod->cmod_flags |= CMOD_BROWSE;
#endif
			    চালু;
			}
			যদি (!checkforcmd_noparen(&eap->cmd, "botright", 2))
			    বিরতি;
			cmod->cmod_split |= WSP_BOT;
			চালু;

	    ক্ষেত্র 'c':	যদি (!checkforcmd_opt(&eap->cmd, "confirm", 4, TRUE))
			    বিরতি;
#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			cmod->cmod_flags |= CMOD_CONFIRM;
#endif
			চালু;

	    ক্ষেত্র 'k':	যদি (checkforcmd_noparen(&eap->cmd, "keepmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_KEEPMARKS;
			    চালু;
			}
			যদি (checkforcmd_noparen(&eap->cmd, "keepalt", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPALT;
			    চালু;
			}
			যদি (checkforcmd_noparen(&eap->cmd, "keeppatterns", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPPATTERNS;
			    চালু;
			}
			যদি (!checkforcmd_noparen(&eap->cmd, "keepjumps", 5))
			    বিরতি;
			cmod->cmod_flags |= CMOD_KEEPJUMPS;
			চালু;

	    ক্ষেত্র 'f':	// only accept ":filter {pat} cmd"
			{
			    char_u  *reg_pat;
			    char_u  *nulp = ফাঁকা;
			    পূর্ণসংখ্যা	    c = 0;

			    যদি (!checkforcmd_noparen(&p, "filter", 4)
				    || *p == NUL
				    || (ends_excmd(*p)
#ifdef FEAT_EVAL
					// in ":filter #pat# cmd" # does not
					// start a comment
				     && (!vim9script || VIM_ISWHITE(p[1]))
#endif
				     ))
				বিরতি;
			    যদি (*p == '!')
			    {
				cmod->cmod_filter_force = TRUE;
				p = skipwhite(p + 1);
				যদি (*p == NUL || ends_excmd(*p))
				    বিরতি;
			    }
#ifdef FEAT_EVAL
			    // Avoid that "filter(arg)" is recognized.
			    যদি (vim9script && !VIM_ISWHITE(p[-1]))
				বিরতি;
#endif
			    যদি (skip_only)
				p = skip_vimgrep_pat(p, ফাঁকা, ফাঁকা);
			    অন্যথা
				// NOTE: This puts a NUL after the pattern.
				p = skip_vimgrep_pat_ext(p, &reg_pat, ফাঁকা,
								    &nulp, &c);
			    যদি (p == ফাঁকা || *p == NUL)
				বিরতি;
			    যদি (!skip_only)
			    {
				cmod->cmod_filter_regmatch.regprog =
						vim_regcomp(reg_pat, RE_MAGIC);
				যদি (cmod->cmod_filter_regmatch.regprog == ফাঁকা)
				    বিরতি;
				// restore the character overwritten by NUL
				যদি (nulp != ফাঁকা)
				    *nulp = c;
			    }
			    eap->cmd = p;
			    চালু;
			}

	    ক্ষেত্র 'h':	যদি (checkforcmd_noparen(&eap->cmd, "horizontal", 3))
			{
			    cmod->cmod_split |= WSP_HOR;
			    চালু;
			}
			// ":hide" and ":hide | cmd" are not modifiers
			যদি (p != eap->cmd || !checkforcmd_noparen(&p, "hide", 3)
					       || *p == NUL || ends_excmd(*p))
			    বিরতি;
			eap->cmd = p;
			cmod->cmod_flags |= CMOD_HIDE;
			চালু;

	    ক্ষেত্র 'l':	যদি (checkforcmd_noparen(&eap->cmd, "lockmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_LOCKMARKS;
			    চালু;
			}
			যদি (checkforcmd_noparen(&eap->cmd, "legacy", 3))
			{
			    যদি (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_legacy_must_be_followed_by_command);
				ফেরত FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_VIM9CMD;
			    cmod->cmod_flags |= CMOD_LEGACY;
			    চালু;
			}

			যদি (!checkforcmd_noparen(&eap->cmd, "leftabove", 5))
			    বিরতি;
			cmod->cmod_split |= WSP_ABOVE;
			চালু;

	    ক্ষেত্র 'n':	যদি (checkforcmd_noparen(&eap->cmd, "noautocmd", 3))
			{
			    cmod->cmod_flags |= CMOD_NOAUTOCMD;
			    চালু;
			}
			যদি (!checkforcmd_noparen(&eap->cmd, "noswapfile", 3))
			    বিরতি;
			cmod->cmod_flags |= CMOD_NOSWAPFILE;
			চালু;

	    ক্ষেত্র 'r':	যদি (!checkforcmd_noparen(&eap->cmd, "rightbelow", 6))
			    বিরতি;
			cmod->cmod_split |= WSP_BELOW;
			চালু;

	    ক্ষেত্র 's':	যদি (checkforcmd_noparen(&eap->cmd, "sandbox", 3))
			{
			    cmod->cmod_flags |= CMOD_SANDBOX;
			    চালু;
			}
			যদি (!checkforcmd_noparen(&eap->cmd, "silent", 3))
			    বিরতি;
			cmod->cmod_flags |= CMOD_SILENT;
			যদি (*eap->cmd == '!' && !VIM_ISWHITE(eap->cmd[-1]))
			{
			    // ":silent!", but not "silent !cmd"
			    eap->cmd = skipwhite(eap->cmd + 1);
			    cmod->cmod_flags |= CMOD_ERRSILENT;
			}
			চালু;

	    ক্ষেত্র 't':	যদি (checkforcmd_noparen(&p, "tab", 3))
			{
			    যদি (!skip_only)
			    {
				long tabnr = get_address(eap, &eap->cmd,
						    ADDR_TABS, eap->skip,
						    skip_only, FALSE, 1);
				যদি (tabnr == MAXLNUM)
				    cmod->cmod_tab = tabpage_সূচক(curtab) + 1;
				অন্যথা
				{
				    যদি (tabnr < 0 || tabnr > LAST_TAB_NR)
				    {
					*errormsg = _(e_invalid_range);
					ফেরত FAIL;
				    }
				    cmod->cmod_tab = tabnr + 1;
				}
			    }
			    eap->cmd = p;
			    চালু;
			}
			যদি (!checkforcmd_noparen(&eap->cmd, "topleft", 2))
			    বিরতি;
			cmod->cmod_split |= WSP_শীর্ষ;
			চালু;

	    ক্ষেত্র 'u':	যদি (!checkforcmd_noparen(&eap->cmd, "unsilent", 3))
			    বিরতি;
			cmod->cmod_flags |= CMOD_UNSILENT;
			চালু;

	    ক্ষেত্র 'v':	যদি (checkforcmd_noparen(&eap->cmd, "vertical", 4))
			{
			    cmod->cmod_split |= WSP_VERT;
			    চালু;
			}
			যদি (checkforcmd_noparen(&eap->cmd, "vim9cmd", 4))
			{
			    যদি (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_vim9cmd_must_be_followed_by_command);
				ফেরত FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_LEGACY;
			    cmod->cmod_flags |= CMOD_VIM9CMD;
			    চালু;
			}
			যদি (!checkforcmd_noparen(&p, "verbose", 4))
			    বিরতি;
			যদি (vim_isdigit(*eap->cmd))
			{
			    // zero means not set, one is verbose == 0, etc.
			    cmod->cmod_verbose = atoi((অক্ষর *)eap->cmd) + 1;
			}
			অন্যথা
			    cmod->cmod_verbose = 2;  // default: verbose == 1
			eap->cmd = p;
			চালু;
	}
	বিরতি;
    }

    যদি (has_visual_range)
    {
	যদি (eap->cmd > cmd_start)
	{
	    // Move the '<,'> range to after the modifiers and insert a colon.
	    // Since the modifiers have been parsed put the colon on top of the
	    // space: "'<,'>mod cmd" -> "mod:'<,'>cmd
	    // Put eap->cmd after the colon.
	    যদি (use_plus_cmd)
	    {
		size_t len = STRLEN(cmd_start);

		// Special case: empty command uses "+":
		//  "'<,'>mods" -> "mods *+
		//  Use "*" instead of "'<,'>" to avoid the command getting
		//  longer, in case it was allocated.
		mch_memmove(orig_cmd, cmd_start, len);
		STRCPY(orig_cmd + len, " *+");
	    }
	    অন্যথা
	    {
		mch_memmove(cmd_start - 5, cmd_start, eap->cmd - cmd_start);
		eap->cmd -= 5;
		mch_memmove(eap->cmd - 1, ":'<,'>", 6);
	    }
	}
	অন্যথা
	    // No modifiers, move the pointer back.
	    // Special case: change empty command to "+".
	    যদি (use_plus_cmd)
		eap->cmd = (char_u *)"'<,'>+";
	    অন্যথা
		eap->cmd = orig_cmd;
    }
    অন্যথা যদি (use_plus_cmd)
	eap->cmd = (char_u *)exmode_plus;

    ফেরত OK;
}

/*
 * Return TRUE if "cmod" has anything set.
 */
    পূর্ণসংখ্যা
has_cmdmod(cmdmod_T *cmod, পূর্ণসংখ্যা ignore_silent)
{
    ফেরত (cmod->cmod_flags != 0 && (!ignore_silent
		|| (cmod->cmod_flags
		      & ~(CMOD_SILENT | CMOD_ERRSILENT | CMOD_UNSILENT)) != 0))
	    || cmod->cmod_split != 0
	    || cmod->cmod_verbose > 0
	    || cmod->cmod_tab != 0
	    || cmod->cmod_filter_regmatch.regprog != ফাঁকা;
}

#যদি defined(FEAT_EVAL)
/*
 * If Vim9 script and "cmdmod" has anything set give an error and return TRUE.
 */
    পূর্ণসংখ্যা
cmdmod_error(পূর্ণসংখ্যা ignore_silent)
{
    যদি (in_vim9script() && has_cmdmod(&cmdmod, ignore_silent))
    {
	emsg(_(e_misplaced_command_modifier));
	ফেরত TRUE;
    }
    ফেরত FALSE;
}
#endif

/*
 * Apply the command modifiers.  Saves current state in "cmdmod", call
 * undo_cmdmod() later.
 */
    শূন্য
apply_cmdmod(cmdmod_T *cmod)
{
#ifdef HAVE_SANDBOX
    যদি ((cmod->cmod_flags & CMOD_SANDBOX) && !cmod->cmod_did_sandbox)
    {
	++sandbox;
	cmod->cmod_did_sandbox = TRUE;
    }
#endif
    যদি (cmod->cmod_verbose > 0)
    {
	যদি (cmod->cmod_verbose_save == 0)
	    cmod->cmod_verbose_save = p_verbose + 1;
	p_verbose = cmod->cmod_verbose - 1;
    }

    যদি ((cmod->cmod_flags & (CMOD_SILENT | CMOD_UNSILENT))
	    && cmod->cmod_save_বার্তা_silent == 0)
    {
	cmod->cmod_save_বার্তা_silent = বার্তা_silent + 1;
	cmod->cmod_save_বার্তা_scroll = বার্তা_scroll;
    }
    যদি (cmod->cmod_flags & CMOD_SILENT)
	++বার্তা_silent;
    যদি (cmod->cmod_flags & CMOD_UNSILENT)
	বার্তা_silent = 0;

    যদি (cmod->cmod_flags & CMOD_ERRSILENT)
    {
	++emsg_silent;
	++cmod->cmod_did_esilent;
    }

    যদি ((cmod->cmod_flags & CMOD_NOAUTOCMD) && cmod->cmod_save_ei == ফাঁকা)
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
    শূন্য
undo_cmdmod(cmdmod_T *cmod)
{
    যদি (cmod->cmod_verbose_save > 0)
    {
	p_verbose = cmod->cmod_verbose_save - 1;
	cmod->cmod_verbose_save = 0;
    }

#ifdef HAVE_SANDBOX
    যদি (cmod->cmod_did_sandbox)
    {
	--sandbox;
	cmod->cmod_did_sandbox = FALSE;
    }
#endif

    যদি (cmod->cmod_save_ei != ফাঁকা)
    {
	// Restore 'eventignore' to the value before ":noautocmd".
	set_string_option_direct((char_u *)"ei", -1, cmod->cmod_save_ei,
							   OPT_FREE, SID_NONE);
	free_string_option(cmod->cmod_save_ei);
	cmod->cmod_save_ei = ফাঁকা;
    }

    vim_regfree(cmod->cmod_filter_regmatch.regprog);

    যদি (cmod->cmod_save_বার্তা_silent > 0)
    {
	// messages could be enabled for a serious error, need to check if the
	// counters don't become negative
	যদি (!did_emsg || বার্তা_silent > cmod->cmod_save_বার্তা_silent - 1)
	    বার্তা_silent = cmod->cmod_save_বার্তা_silent - 1;
	emsg_silent -= cmod->cmod_did_esilent;
	যদি (emsg_silent < 0)
	    emsg_silent = 0;
	// Restore msg_scroll, it's set by file I/O commands, even when no
	// message is actually displayed.
	বার্তা_scroll = cmod->cmod_save_বার্তা_scroll;

	// "silent reg" or "silent echo x" inside "redir" leaves msg_col
	// somewhere in the line.  Put it back in the first column.
	যদি (redirecting())
	    বার্তা_col = 0;

	cmod->cmod_save_বার্তা_silent = 0;
	cmod->cmod_did_esilent = 0;
    }
}

/*
 * Parse the address range, if any, in "eap".
 * May set the last search pattern, unless "silent" is TRUE.
 * Return FAIL and set "errormsg" or return OK.
 */
    পূর্ণসংখ্যা
parse_cmd_address(exarg_T *eap, অক্ষর **errormsg, পূর্ণসংখ্যা silent)
{
    পূর্ণসংখ্যা		address_গুনতি = 1;
    linenr_T	lnum;
    পূর্ণসংখ্যা		need_check_cursor = FALSE;
    পূর্ণসংখ্যা		ret = FAIL;

    // Repeat for all ',' or ';' separated addresses.
    জন্য (;;)
    {
	eap->line1 = eap->line2;
	eap->line2 = default_address(eap);
	eap->cmd = skipwhite(eap->cmd);
	lnum = get_address(eap, &eap->cmd, eap->addr_type, eap->skip, silent,
					eap->addr_গুনতি == 0, address_গুনতি++);
	যদি (eap->cmd == ফাঁকা)	// error detected
	    যাও theend;
	যদি (lnum == MAXLNUM)
	{
	    যদি (*eap->cmd == '%')   // '%' - all lines
	    {
		++eap->cmd;
		নির্বাচন (eap->addr_type)
		{
		    ক্ষেত্র ADDR_সারিগুলি:
		    ক্ষেত্র ADDR_OTHER:
			eap->line1 = 1;
			eap->line2 = curbuf->b_ml.ml_সারি_গুনতি;
			বিরতি;
		    ক্ষেত্র ADDR_LOADED_BUFFERS:
			{
			    buf_T	*buf = firstbuf;

			    যখন (buf->b_পরবর্তী != ফাঁকা
						  && buf->b_ml.ml_mfp == ফাঁকা)
				buf = buf->b_পরবর্তী;
			    eap->line1 = buf->b_fnum;
			    buf = lastbuf;
			    যখন (buf->b_পূর্ব != ফাঁকা
						  && buf->b_ml.ml_mfp == ফাঁকা)
				buf = buf->b_পূর্ব;
			    eap->line2 = buf->b_fnum;
			    বিরতি;
			}
		    ক্ষেত্র ADDR_BUFFERS:
			eap->line1 = firstbuf->b_fnum;
			eap->line2 = lastbuf->b_fnum;
			বিরতি;
		    ক্ষেত্র ADDR_WINDOWS:
		    ক্ষেত্র ADDR_TABS:
			যদি (IS_USER_CMDIDX(eap->cmdidx))
			{
			    eap->line1 = 1;
			    eap->line2 = eap->addr_type == ADDR_WINDOWS
						  ? LAST_WIN_NR : LAST_TAB_NR;
			}
			অন্যথা
			{
			    // there is no Vim command which uses '%' and
			    // ADDR_WINDOWS or ADDR_TABS
			    *errormsg = _(e_invalid_range);
			    যাও theend;
			}
			বিরতি;
		    ক্ষেত্র ADDR_TABS_RELATIVE:
		    ক্ষেত্র ADDR_UNSIGNED:
		    ক্ষেত্র ADDR_QUICKFIX:
			*errormsg = _(e_invalid_range);
			যাও theend;
		    ক্ষেত্র ADDR_ARGUMENTS:
			যদি (ARGCOUNT == 0)
			    eap->line1 = eap->line2 = 0;
			অন্যথা
			{
			    eap->line1 = 1;
			    eap->line2 = ARGCOUNT;
			}
			বিরতি;
		    ক্ষেত্র ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			eap->line1 = 1;
			eap->line2 = qf_get_valid_আকার(eap);
			যদি (eap->line2 == 0)
			    eap->line2 = 1;
#endif
			বিরতি;
		    ক্ষেত্র ADDR_NONE:
			// Will give an error later if a range is found.
			বিরতি;
		}
		++eap->addr_গুনতি;
	    }
	    অন্যথা যদি (*eap->cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == ফাঁকা)
	    {
		pos_T	    *fp;

		// '*' - visual area
		যদি (eap->addr_type != ADDR_সারিগুলি)
		{
		    *errormsg = _(e_invalid_range);
		    যাও theend;
		}

		++eap->cmd;
		যদি (!eap->skip)
		{
		    fp = getmark('<', FALSE);
		    যদি (check_mark(fp) == FAIL)
			যাও theend;
		    eap->line1 = fp->lnum;
		    fp = getmark('>', FALSE);
		    যদি (check_mark(fp) == FAIL)
			যাও theend;
		    eap->line2 = fp->lnum;
		    ++eap->addr_গুনতি;
		}
	    }
	}
	অন্যথা
	    eap->line2 = lnum;
	eap->addr_গুনতি++;

	যদি (*eap->cmd == ';')
	{
	    যদি (!eap->skip)
	    {
		curwin->w_cursor.lnum = eap->line2;

		// Don't leave the cursor on an illegal line or column, but do
		// accept zero as address, so 0;/PATTERN/ works correctly
		// (where zero usually means to use the first line).
		// Check the cursor position before returning.
		যদি (eap->line2 > 0)
		    check_cursor();
		অন্যথা
		    check_cursor_col();
		need_check_cursor = TRUE;
	    }
	}
	অন্যথা যদি (*eap->cmd != ',')
	    বিরতি;
	++eap->cmd;
    }

    // One address given: set start and end lines.
    যদি (eap->addr_গুনতি == 1)
    {
	eap->line1 = eap->line2;
	// ... but only implicit: really no address given
	যদি (lnum == MAXLNUM)
	    eap->addr_গুনতি = 0;
    }
    ret = OK;

theend:
    যদি (need_check_cursor)
	check_cursor();
    ফেরত ret;
}

/*
 * Append "cmd" to the error message in IObuff.
 * Takes care of limiting the length and handling 0xa0, which would be
 * invisible otherwise.
 */
    স্থির শূন্য
append_command(char_u *cmd)
{
    size_t  len = STRLEN(IObuff);
    char_u  *s = cmd;
    char_u  *d;

    যদি (len > IOSIZE - 100)
    {
	// Not enough space, truncate and put in "...".
	d = IObuff + IOSIZE - 100;
	d -= mb_শীর্ষ_off(IObuff, d);
	STRCPY(d, "...");
    }
    STRCAT(IObuff, ": ");
    d = IObuff + STRLEN(IObuff);
    যখন (*s != NUL && d - IObuff + 5 < IOSIZE)
    {
	যদি (enc_utf8 ? (s[0] == 0xc2 && s[1] == 0xa0) : *s == 0xa0)
	{
	    s += enc_utf8 ? 2 : 1;
	    STRCPY(d, "<a0>");
	    d += 4;
	}
	অন্যথা যদি (d - IObuff + (*mb_ptr2len)(s) + 1 >= IOSIZE)
	    বিরতি;
	অন্যথা
	    MB_COPY_CHAR(s, d);
    }
    *d = NUL;
}

#যদি defined(FEAT_EVAL)
/*
 * If "start" points "&opt", "&l:opt", "&g:opt" or "$ENV" return a pointer to
 * the name.  Otherwise just return "start".
 */
    char_u *
skip_option_env_lead(char_u *start)
{
    char_u *name = start;

    যদি (*start == '&')
    {
	যদি ((start[1] == 'l' || start[1] == 'g') && start[2] == ':')
	    name += 3;
	অন্যথা
	    name += 1;
    }
    অন্যথা যদি (*start == '$')
	name += 1;
    ফেরত name;
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
    স্থির পূর্ণসংখ্যা
one_letter_cmd(char_u *p, cmdidx_T *idx)
{
    যদি (in_vim9script())
	ফেরত FALSE;
    যদি (p[0] == 'k'
	    && (p[1] != 'e' || (p[1] == 'e' && p[2] != 'e')))
    {
	*idx = CMD_k;
	ফেরত TRUE;
    }
    যদি (p[0] == 's'
	    && ((p[1] == 'c' && (p[2] == NUL || (p[2] != 's' && p[2] != 'r'
			&& (p[3] == NUL || (p[3] != 'i' && p[4] != 'p')))))
		|| p[1] == 'g'
		|| (p[1] == 'i' && p[2] != 'm' && p[2] != 'l' && p[2] != 'g')
		|| p[1] == 'I'
		|| (p[1] == 'r' && p[2] != 'e')))
    {
	*idx = CMD_substitute;
	ফেরত TRUE;
    }
    ফেরত FALSE;
}

#যদি defined(FEAT_EVAL)
/*
 * Return TRUE if "cmd" starts with "123->", a number followed by a method
 * call.
 */
    পূর্ণসংখ্যা
সংখ্যা_method(char_u *cmd)
{
    char_u *p = skipdigits(cmd);

    ফেরত p > cmd && (p = skipwhite(p))[0] == '-' && p[1] == '>';
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
	পূর্ণসংখ্যা	*full UNUSED,
	পূর্ণসংখ্যা	(*lookup)(char_u *, size_t, পূর্ণসংখ্যা cmd, cctx_T *) UNUSED,
	cctx_T	*cctx UNUSED)
{
    পূর্ণসংখ্যা		len;
    char_u	*p;
    পূর্ণসংখ্যা		i;
#ifndef FEAT_EVAL
    পূর্ণসংখ্যা		vim9 = FALSE;
#অন্যথা
    পূর্ণসংখ্যা		vim9 = in_vim9script();

    /*
     * Recognize a Vim9 script function/method call and assignment:
     * "lvar = value", "lvar(arg)", "[1, 2 3]->Func()"
     */
    p = eap->cmd;
    যদি (lookup != ফাঁকা)
    {
	char_u *pskip = skip_option_env_lead(eap->cmd);

	যদি (vim_strchr((char_u *)"{('[\"@&$", *p) != ফাঁকা
	       || ((p = to_name_const_end(pskip)) > eap->cmd && *p != NUL)
	       || (p[0] == '0' && p[1] == 'z'))
	{
	    পূর্ণসংখ্যা	    oplen;
	    পূর্ণসংখ্যা	    heredoc;
	    char_u  *swp;

	    যদি (*eap->cmd == '&'
		    || (eap->cmd[0] == '$'
				  && eap->cmd[1] != '\'' && eap->cmd[1] != '"')
		    || (eap->cmd[0] == '@'
					&& (valid_yank_reg(eap->cmd[1], FALSE)
						       || eap->cmd[1] == '@')))
	    {
		যদি (*eap->cmd == '&')
		{
		    p = eap->cmd + 1;
		    যদি (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
			p += 2;
		    p = to_name_end(p, FALSE);
		}
		অন্যথা যদি (*eap->cmd == '$')
		    p = to_name_end(eap->cmd + 1, FALSE);
		অন্যথা
		    p = eap->cmd + 2;
		যদি (ends_excmd(*skipwhite(p)))
		{
		    // "&option <NL>", "$ENV <NL>" and "@r <NL>" are the start
		    // of an expression.
		    eap->cmdidx = CMD_eval;
		    ফেরত eap->cmd;
		}
		// "&option" can be followed by "->" or "=", check below
	    }

	    যদি (vim9 && *p == '<')
	    {
		// generic function type args
		যদি (skip_generic_func_type_args(&p) == FAIL)
		{
		    eap->cmdidx = CMD_আকার;
		    ফেরত p;
		}
	    }

	    swp = skipwhite(p);

	    যদি (
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
		যদি (*eap->cmd == '{' && ends_excmd(*skipwhite(eap->cmd + 1)))
		{
		    // "{" by itself is the start of a block.
		    eap->cmdidx = CMD_block;
		    ফেরত eap->cmd + 1;
		}
		eap->cmdidx = CMD_eval;
		ফেরত eap->cmd;
	    }

	    যদি ((p != eap->cmd && (
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
		যদি (skip_রাশিমালা(&after, ফাঁকা) == OK)
		{
		    after = skipwhite(after);
		    যদি (*after == '=' || (*after != NUL && after[1] == '=')
					 || (after[0] == '.' && after[1] == '.'
							   && after[2] == '='))
			eap->cmdidx = CMD_var;
		}
		--emsg_off;
		ফেরত eap->cmd;
	    }

	    // "[...]->Method()" is a list expression, but "[a, b] = Func()" is
	    // an assignment.
	    // If there is no line break inside the "[...]" then "p" is
	    // advanced to after the "]" by to_name_const_end(): check if a "="
	    // follows.
	    // If "[...]" has a line break "p" still points at the "[" and it
	    // can't be an assignment.
	    যদি (*eap->cmd == '[')
	    {
		char_u	    *eq;

		p = to_name_const_end(eap->cmd);
		যদি (p == eap->cmd && *p == '[')
		{
		    পূর্ণসংখ্যা গুনতি = 0;
		    পূর্ণসংখ্যা	semicolon = FALSE;

		    p = skip_var_list(eap->cmd, TRUE, &গুনতি, &semicolon, TRUE);
		}
		eq = p;
		যদি (eq != ফাঁকা)
		{
		    eq = skipwhite(eq);
		    যদি (vim_strchr((char_u *)"+-*/%.", *eq) != ফাঁকা)
		    {
			যদি (eq[0] == '.' && eq[1] == '.')
			    ++eq;
			++eq;
		    }
		}
		যদি (p == ফাঁকা || p == eap->cmd || *eq != '=')
		{
		    eap->cmdidx = CMD_eval;
		    ফেরত eap->cmd;
		}
		যদি (p > eap->cmd && *eq == '=')
		{
		    eap->cmdidx = CMD_var;
		    ফেরত eap->cmd;
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
	    যদি (oplen > 0)
	    {
		যদি (((p - eap->cmd) > 2 && eap->cmd[1] == ':')
			|| *eap->cmd == '&'
			|| *eap->cmd == '$'
			|| *eap->cmd == '@'
			|| (eap->skip && IS_WHITE_OR_NUL(
						      *(skipwhite(p) + oplen)))
			|| lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK)
		{
		    eap->cmdidx = CMD_var;
		    ফেরত eap->cmd;
		}
	    }

	    // Recognize trying to use a type for a w:, b:, t: or g: variable:
	    // "w:varname: number = 123".
	    যদি (eap->cmd[1] == ':' && *p == ':')
	    {
		eap->cmdidx = CMD_var;
		ফেরত eap->cmd;
	    }
	}

	// 1234->func() is a method call
	যদি (সংখ্যা_method(eap->cmd))
	{
	    eap->cmdidx = CMD_eval;
	    ফেরত eap->cmd;
	}

	// "g:", "s:" and "l:" are always assumed to be a variable, thus start
	// an expression.  A global/substitute/list command needs to use a
	// longer name.
	যদি (vim_strchr((char_u *)"gsl", *p) != ফাঁকা && p[1] == ':')
	{
	    eap->cmdidx = CMD_eval;
	    ফেরত eap->cmd;
	}

	// If it is an ID it might be a variable with an operator on the next
	// line, if the variable exists it can't be an Ex command.
	যদি (p > eap->cmd && ends_excmd(*skipwhite(p))
		&& (lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK
		    || (ASCII_ISALPHA(eap->cmd[0]) && eap->cmd[1] == ':')))
	{
	    eap->cmdidx = CMD_eval;
	    ফেরত eap->cmd;
	}

	// Check for "++nr" and "--nr".
	যদি (p == eap->cmd && p[0] != NUL && p[0] == p[1]
						   && (*p == '+' || *p == '-'))
	{
	    eap->cmdidx = *p == '+' ? CMD_increment : CMD_decrement;
	    ফেরত eap->cmd + 2;
	}
    }
#endif

    /*
     * Isolate the command and search for it in the command table.
     */
    p = eap->cmd;
    যদি (one_letter_cmd(p, &eap->cmdidx))
    {
	++p;
	যদি (full != ফাঁকা)
	    *full = TRUE;
    }
    অন্যথা
    {
	যখন (ASCII_ISALPHA(*p))
	    ++p;
	// for python 3.x support ":py3", ":python3", ":py3file", etc.
	যদি (eap->cmd[0] == 'p' && eap->cmd[1] == 'y')
	{
	    যখন (ASCII_ISALNUM(*p))
		++p;
	}
	অন্যথা যদি (*p == '9' && STRNCMP("vim9", eap->cmd, 4) == 0)
	{
	    // include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	    ++p;
	    যখন (ASCII_ISALPHA(*p))
		++p;
	}

	// check for non-alpha command
	যদি (p == eap->cmd && vim_strchr((char_u *)"@*!=><&~#}", *p) != ফাঁকা)
	    ++p;
	len = (পূর্ণসংখ্যা)(p - eap->cmd);
	// The "d" command can directly be followed by 'l' or 'p' flag, when
	// not in Vim9 script.
	যদি (!vim9 && *eap->cmd == 'd' && (p[-1] == 'l' || p[-1] == 'p'))
	{
	    // Check for ":dl", ":dell", etc. to ":deletel": that's
	    // :delete with the 'l' flag.  Same for 'p'.
	    জন্য (i = 0; i < len; ++i)
		যদি (eap->cmd[i] != ((char_u *)"delete")[i])
		    বিরতি;
	    যদি (i == len - 1)
	    {
		--len;
		যদি (p[-1] == 'l')
		    eap->flags |= EXFLAG_LIST;
		অন্যথা
		    eap->flags |= EXFLAG_PRINT;
	    }
	}

	যদি (ASCII_ISLOWER(eap->cmd[0]))
	{
	    পূর্ণসংখ্যা c1 = eap->cmd[0];
	    পূর্ণসংখ্যা c2 = len == 1 ? NUL : eap->cmd[1];

	    যদি (command_গুনতি != (পূর্ণসংখ্যা)CMD_আকার)
	    {
		iemsg(e_command_table_needs_to_be_updated_run_make_cmdidxs);
		getout(1);
	    }

	    // Use a precomputed index for fast look-up in cmdnames[]
	    // taking into account the first 2 letters of eap->cmd.
	    eap->cmdidx = cmdidxs1[Char_Ord_নিচের(c1)];
	    যদি (ASCII_ISLOWER(c2))
		eap->cmdidx += cmdidxs2[Char_Ord_নিচের(c1)][Char_Ord_নিচের(c2)];
	}
	অন্যথা যদি (ASCII_ISUPPER(eap->cmd[0]))
	    eap->cmdidx = CMD_পরবর্তী_3;
	অন্যথা
	    eap->cmdidx = CMD_bang;

	জন্য ( ; (পূর্ণসংখ্যা)eap->cmdidx < (পূর্ণসংখ্যা)CMD_আকার;
			       eap->cmdidx = (cmdidx_T)((পূর্ণসংখ্যা)eap->cmdidx + 1))
	    যদি (STRNCMP(cmdnames[(পূর্ণসংখ্যা)eap->cmdidx].cmd_name, (অক্ষর *)eap->cmd,
							    (size_t)len) == 0)
	    {
#ifdef FEAT_EVAL
		যদি (full != ফাঁকা && cmdnames[eap->cmdidx].cmd_name[len] == NUL)
		    *full = TRUE;
#endif
		বিরতি;
	    }

	// :Print and :mode are not supported in Vim9 script.
	// Some commands cannot be shortened in Vim9 script.
	যদি (vim9 && eap->cmdidx != CMD_আকার)
	{
	    যদি (eap->cmdidx == CMD_mode || eap->cmdidx == CMD_Print)
		eap->cmdidx = CMD_আকার;
	    অন্যথা যদি ((cmdnames[eap->cmdidx].cmd_argt & EX_WHOLE)
			  && len < (পূর্ণসংখ্যা)cmdnames[eap->cmdidx].cmd_namelen)
	    {
		semsg(_(e_command_cannot_be_shortened_str), eap->cmd);
		eap->cmdidx = CMD_আকার;
	    }
	}

	// Do not recognize ":*" as the star command unless '*' is in
	// 'cpoptions'.
	যদি (eap->cmdidx == CMD_star && vim_strchr(p_cpo, CPO_STAR) == ফাঁকা)
	    p = eap->cmd;

	// Look for a user defined command as a last resort.  Let ":Print" be
	// overruled by a user defined command.
	যদি ((eap->cmdidx == CMD_আকার || eap->cmdidx == CMD_Print)
		&& *eap->cmd >= 'A' && *eap->cmd <= 'Z')
	{
	    // User defined commands may contain digits.
	    যখন (ASCII_ISALNUM(*p))
		++p;
	    p = find_ucmd(eap, p, full, ফাঁকা, ফাঁকা);
	}
	যদি (p == ফাঁকা || p == eap->cmd)
	    eap->cmdidx = CMD_আকার;
    }

    // ":fina" means ":finally" in legacy script, for backwards compatibility.
    যদি (eap->cmdidx == CMD_final && p - eap->cmd == 4 && !vim9)
	eap->cmdidx = CMD_finally;

     // Force ":ho" to be unresolved.  Without this, find_ex_command()
     // matches it to CMD_horizontal (the only "ho*" entry), which makes
     // fullcommand("ho") return "horizontal" even though ":ho" cannot be
     // used as the modifier (cmdmods[] requires 3 chars, "hor").
    যদি (eap->cmdidx == CMD_horizontal && p - eap->cmd == 2)
	eap->cmdidx = CMD_আকার;

#ifdef FEAT_EVAL
    যদি (eap->cmdidx < CMD_আকার
	    && vim9
	    && !IS_WHITE_NL_OR_NUL(*p) && *p != '!' && *p != '|'
	    && (eap->cmdidx < 0 ||
		(cmdnames[eap->cmdidx].cmd_argt & EX_NONWHITE_OK) == 0))
    {
	// A name that goes on with an underscore is not this command with an
	// argument: "ch_log" is not ":change".  Leave it to be reported as an
	// invalid command.
	যদি (*p != '_')
	{
	    char_u *cmd = vim_strnsave(eap->cmd, p - eap->cmd);

	    semsg(_(e_command_str_not_followed_by_white_space_str), cmd,
								    eap->cmd);
	    vim_free(cmd);
	}
	eap->cmdidx = CMD_আকার;
    }
#endif

    ফেরত p;
}

#যদি defined(FEAT_EVAL)
প্রকারলেখ কাঠামো
{
    অক্ষর	*name;
    পূর্ণসংখ্যা		minlen;
    পূর্ণসংখ্যা		has_গুনতি;  // :123verbose  :3tab
} cmdmod_info_T;

স্থির cmdmod_info_T cmdmod_info_tab[] = {
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
    পূর্ণসংখ্যা
modifier_len(char_u *cmd)
{
    পূর্ণসংখ্যা		i, j;
    char_u	*p = cmd;

    যদি (VIM_ISDIGIT(*cmd))
	p = skipwhite(skipdigits(cmd + 1));
    জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(cmdmod_info_tab); ++i)
    {
	// cmdmod_info_tab[] is sorted by name: once the first letter is past
	// the command's first letter no later entry can match.
	যদি (cmdmod_info_tab[i].name[0] > *p)
	    বিরতি;
	জন্য (j = 0; p[j] != NUL; ++j)
	    যদি (p[j] != cmdmod_info_tab[i].name[j])
		বিরতি;
	যদি (!ASCII_ISALPHA(p[j]) && j >= cmdmod_info_tab[i].minlen
					&& (p == cmd || cmdmod_info_tab[i].has_গুনতি))
	    ফেরত j + (পূর্ণসংখ্যা)(p - cmd);
    }
    ফেরত 0;
}

/*
 * Return > 0 if an Ex command "name" exists.
 * Return 2 if there is an exact match.
 * Return 3 if there is an ambiguous match.
 */
    পূর্ণসংখ্যা
cmd_exists(char_u *name)
{
    exarg_T	ea;
    পূর্ণসংখ্যা		full = FALSE;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		j;
    char_u	*p;

    // Check command modifiers.
    জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(cmdmod_info_tab); ++i)
    {
	জন্য (j = 0; name[j] != NUL; ++j)
	    যদি (name[j] != cmdmod_info_tab[i].name[j])
		বিরতি;
	যদি (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	    ফেরত (cmdmod_info_tab[i].name[j] == NUL ? 2 : 1);
    }

    // Check built-in commands and user defined commands.
    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    p = find_ex_command(&ea, &full, ফাঁকা, ফাঁকা);
    যদি (p == ফাঁকা)
	ফেরত 3;
    যদি (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	ফেরত 0;
    যদি (*skipwhite(p) != NUL)
	ফেরত 0;	// trailing garbage
    ফেরত (ea.cmdidx == CMD_আকার ? 0 : (full ? 2 : 1));
}

/*
 * Add the attributes of an Ex command to "d" in the terms of ":command":
 * "argt" are the EX_ flags, "addr_type" the address type and "def" the
 * default for a range or count, -1 when there is none.
 */
    শূন্য
ex_command_attrs(dict_T *d, long_u argt, cmd_addr_T addr_type, long def)
{
    স্থির অক্ষর *addr_names[] = {
	"lines", "windows", "arguments", "loaded_buffers", "buffers", "tabs",
	"tabs_relative", "quickfix_valid", "quickfix", "unsigned", "other",
	"none"
    };
    অক্ষর	*nargs;

    যদি (!(argt & EX_EXTRA))
	nargs = "0";
    অন্যথা যদি (!(argt & EX_NOSPC))
	nargs = (argt & EX_NEEDARG) ? "+" : "*";
    অন্যথা যদি (!(argt & EX_NEEDARG))
	nargs = "?";
    অন্যথা
	nargs = (argt & EX_ARGSPACE) ? "_" : "1";
    dict_add_string(d, "nargs", (char_u *)nargs);

    যদি (!(argt & EX_RANGE))
	dict_add_bool(d, "range", FALSE);
    অন্যথা যদি (argt & EX_DFLALL)
	dict_add_string(d, "range", (char_u *)"%");
    অন্যথা যদি (def >= 0 && !(argt & EX_গুনতি))
	dict_add_সংখ্যা(d, "range", def);
    অন্যথা
	dict_add_string(d, "range", (char_u *)".");
    যদি (!(argt & EX_গুনতি))
	dict_add_bool(d, "count", FALSE);
    অন্যথা
	dict_add_সংখ্যা(d, "count", def < 0 ? 0 : def);
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
    পূর্ণসংখ্যা
ex_command_info(char_u *name, পূর্ণসংখ্যা vim9, dict_T *d)
{
    exarg_T	ea;
    char_u	*p;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		j;
    পূর্ণসংখ্যা		save_cmod_flags = cmdmod.cmod_flags;
    পূর্ণসংখ্যা		ret = OK;

    যদি (vim9 != -1)
    {
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9 ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    // An abbreviation of a command modifier, like ":bo" for ":botright".
    জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(cmdmod_info_tab); ++i)
    {
	জন্য (j = 0; name[j] != NUL; ++j)
	    যদি (name[j] != cmdmod_info_tab[i].name[j])
		বিরতি;
	যদি (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	{
	    name = (char_u *)cmdmod_info_tab[i].name;
	    বিরতি;
	}
    }

    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    ea.addr_গুনতি = 0;
    // Don't complain about using "en" or ":let" in Vim9 script.
    ++emsg_silent;
    p = find_ex_command(&ea, ফাঁকা, ফাঁকা, ফাঁকা);
    যদি (p == ফাঁকা || ea.cmdidx == CMD_আকার
	    || (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	    || *skipwhite(p) != NUL
	    || not_in_vim9(&ea, true) == FAIL)
	ret = FAIL;
    --emsg_silent;

    যদি (ret == FAIL)
	;
    অন্যথা যদি (IS_USER_CMDIDX(ea.cmdidx))
	ret = user_command_info(ea.useridx, ea.cmdidx, d);
    অন্যথা
    {
	পূর্ণসংখ্যা	notimpl = cmdnames[ea.cmdidx].cmd_func == ex_ni
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
    ফেরত ret;
}

/*
 * "fullcommand" function
 */
    শূন্য
f_fullcommand(typval_T *argvars, typval_T *rettv)
{
    exarg_T	ea;
    char_u	*name;
    char_u	*p;
    পূর্ণসংখ্যা		vim9script = in_vim9script();
    পূর্ণসংখ্যা		save_cmod_flags = cmdmod.cmod_flags;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ফেরত;

    name = tv_get_string(&argvars[0]);
    যদি (name == ফাঁকা)
	ফেরত;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	vim9script = tv_get_bool(&argvars[1]);
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9script ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    যখন (*name == ':')
	name++;
    name = skip_range(name, TRUE, ফাঁকা);

    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.addr_গুনতি = 0;
    ++emsg_silent;  // don't complain about using "en" in Vim9 script
    p = find_ex_command(&ea, ফাঁকা, ফাঁকা, ফাঁকা);
    --emsg_silent;
    যদি (p == ফাঁকা || ea.cmdidx == CMD_আকার)
	যাও theend;

    যদি (vim9script && not_in_vim9(&ea, true) == FAIL)
	যাও theend;

    rettv->vval.v_string = vim_strsave(IS_USER_CMDIDX(ea.cmdidx)
				 ? get_user_command_name(ea.useridx, ea.cmdidx)
				 : cmdnames[ea.cmdidx].cmd_name);
theend:
    cmdmod.cmod_flags = save_cmod_flags;
}
#endif

    cmdidx_T
excmd_get_cmdidx(char_u *cmd, পূর্ণসংখ্যা len)
{
    cmdidx_T idx;

    যদি (!one_letter_cmd(cmd, &idx))
	জন্য (idx = (cmdidx_T)0; (পূর্ণসংখ্যা)idx < (পূর্ণসংখ্যা)CMD_আকার;
		idx = (cmdidx_T)((পূর্ণসংখ্যা)idx + 1))
	    যদি (STRNCMP(cmdnames[(পূর্ণসংখ্যা)idx].cmd_name, cmd, (size_t)len) == 0)
		বিরতি;

    ফেরত idx;
}

    long
excmd_get_argt(cmdidx_T idx)
{
    ফেরত (long)cmdnames[(পূর্ণসংখ্যা)idx].cmd_argt;
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
    পূর্ণসংখ্যা		skip_star,	// skip "*" used for Visual range
    পূর্ণসংখ্যা		*ctx)		// pointer to xp_context or NULL
{
    char_u	*cmd = cmd_start;
    unsigned	delim;

    যখন (vim_strchr((char_u *)" \t0123456789.$%'/?-+,;\\", *cmd) != ফাঁকা)
    {
	যদি (*cmd == '\\')
	{
	    যদি (cmd[1] == '?' || cmd[1] == '/' || cmd[1] == '&')
		++cmd;
	    অন্যথা
		বিরতি;
	}
	অন্যথা যদি (*cmd == '\'')
	{
	    char_u *p = cmd;

	    // a quote is only valid at the start or after a separator
	    যখন (p > cmd_start)
	    {
		--p;
		যদি (!VIM_ISWHITE(*p))
		    বিরতি;
	    }
	    যদি (cmd > cmd_start && !VIM_ISWHITE(*p) && *p != ',' && *p != ';')
		বিরতি;
	    যদি (*++cmd == NUL && ctx != ফাঁকা)
		*ctx = EXPAND_NOTHING;
	}
	অন্যথা যদি (*cmd == '/' || *cmd == '?')
	{
	    delim = *cmd++;
	    যখন (*cmd != NUL && *cmd != delim)
		যদি (*cmd++ == '\\' && *cmd != NUL)
		    ++cmd;
	    যদি (*cmd == NUL && ctx != ফাঁকা)
		*ctx = EXPAND_NOTHING;
	}
	যদি (*cmd != NUL)
	    ++cmd;
    }

    // Skip ":" and white space.
    যখন (*cmd == ':')
	cmd = skipwhite(cmd + 1);

    // Skip "*" used for Visual range.
    যদি (skip_star && *cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == ফাঁকা)
	cmd = skipwhite(cmd + 1);

    ফেরত cmd;
}

    স্থির শূন্য
addr_error(cmd_addr_T addr_type)
{
    যদি (addr_type == ADDR_NONE)
	emsg(_(e_no_range_allowed));
    অন্যথা
	emsg(_(e_invalid_range));
}

/*
 * Return the default address for an address type.
 */
    স্থির linenr_T
default_address(exarg_T *eap)
{
    linenr_T lnum = 0;

    নির্বাচন (eap->addr_type)
    {
	ক্ষেত্র ADDR_সারিগুলি:
	ক্ষেত্র ADDR_OTHER:
	    // Default is the cursor line number.  Avoid using an invalid
	    // line number though.
	    যদি (curwin->w_cursor.lnum > curbuf->b_ml.ml_সারি_গুনতি)
		lnum = curbuf->b_ml.ml_সারি_গুনতি;
	    অন্যথা
		lnum = curwin->w_cursor.lnum;
	    বিরতি;
	ক্ষেত্র ADDR_WINDOWS:
	    lnum = CURRENT_WIN_NR;
	    বিরতি;
	ক্ষেত্র ADDR_ARGUMENTS:
	    lnum = curwin->w_arg_idx + 1;
	    যদি (lnum > ARGCOUNT)
		lnum = ARGCOUNT;
	    বিরতি;
	ক্ষেত্র ADDR_LOADED_BUFFERS:
	ক্ষেত্র ADDR_BUFFERS:
	    lnum = curbuf->b_fnum;
	    বিরতি;
	ক্ষেত্র ADDR_TABS:
	    lnum = CURRENT_TAB_NR;
	    বিরতি;
	ক্ষেত্র ADDR_TABS_RELATIVE:
	ক্ষেত্র ADDR_UNSIGNED:
	    lnum = 1;
	    বিরতি;
	ক্ষেত্র ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_idx(eap);
#endif
	    বিরতি;
	ক্ষেত্র ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_valid_idx(eap);
#endif
	    বিরতি;
	ক্ষেত্র ADDR_NONE:
	    // Will give an error later if a range is found.
	    বিরতি;
    }
    ফেরত lnum;
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
    পূর্ণসংখ্যা		skip,		// only skip the address, don't use it
    পূর্ণসংখ্যা		silent,		// no errors or side effects
    পূর্ণসংখ্যা		to_other_file,  // flag: may jump to other file
    পূর্ণসংখ্যা		address_গুনতি UNUSED) // 1 for first address, >1 after comma
{
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		i;
    long	n;
    char_u	*cmd;
    pos_T	pos;
    pos_T	*fp;
    linenr_T	lnum;
    buf_T	*buf;

    cmd = skipwhite(*ptr);
    lnum = MAXLNUM;
    করো
    {
	নির্বাচন (*cmd)
	{
	    ক্ষেত্র '.':			    // '.' - Cursor position
		++cmd;
		নির্বাচন (addr_type)
		{
		    ক্ষেত্র ADDR_সারিগুলি:
		    ক্ষেত্র ADDR_OTHER:
			lnum = curwin->w_cursor.lnum;
			বিরতি;
		    ক্ষেত্র ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			বিরতি;
		    ক্ষেত্র ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			বিরতি;
		    ক্ষেত্র ADDR_LOADED_BUFFERS:
		    ক্ষেত্র ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			বিরতি;
		    ক্ষেত্র ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			বিরতি;
		    ক্ষেত্র ADDR_NONE:
		    ক্ষেত্র ADDR_TABS_RELATIVE:
		    ক্ষেত্র ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = ফাঁকা;
			যাও error;
			বিরতি;
		    ক্ষেত্র ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			বিরতি;
		    ক্ষেত্র ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			বিরতি;
		}
		বিরতি;

	    ক্ষেত্র '$':			    // '$' - last line
		++cmd;
		নির্বাচন (addr_type)
		{
		    ক্ষেত্র ADDR_সারিগুলি:
		    ক্ষেত্র ADDR_OTHER:
			lnum = curbuf->b_ml.ml_সারি_গুনতি;
			বিরতি;
		    ক্ষেত্র ADDR_WINDOWS:
			lnum = LAST_WIN_NR;
			বিরতি;
		    ক্ষেত্র ADDR_ARGUMENTS:
			lnum = ARGCOUNT;
			বিরতি;
		    ক্ষেত্র ADDR_LOADED_BUFFERS:
			buf = lastbuf;
			যখন (buf->b_ml.ml_mfp == ফাঁকা)
			{
			    যদি (buf->b_পূর্ব == ফাঁকা)
				বিরতি;
			    buf = buf->b_পূর্ব;
			}
			lnum = buf->b_fnum;
			বিরতি;
		    ক্ষেত্র ADDR_BUFFERS:
			lnum = lastbuf->b_fnum;
			বিরতি;
		    ক্ষেত্র ADDR_TABS:
			lnum = LAST_TAB_NR;
			বিরতি;
		    ক্ষেত্র ADDR_NONE:
		    ক্ষেত্র ADDR_TABS_RELATIVE:
		    ক্ষেত্র ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = ফাঁকা;
			যাও error;
			বিরতি;
		    ক্ষেত্র ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_আকার(eap);
			যদি (lnum == 0)
			    lnum = 1;
#endif
			বিরতি;
		    ক্ষেত্র ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_valid_আকার(eap);
			যদি (lnum == 0)
			    lnum = 1;
#endif
			বিরতি;
		}
		বিরতি;

	    ক্ষেত্র '\'':			    // ''' - mark
		যদি (*++cmd == NUL)
		{
		    cmd = ফাঁকা;
		    যাও error;
		}
		যদি (addr_type != ADDR_সারিগুলি)
		{
		    addr_error(addr_type);
		    cmd = ফাঁকা;
		    যাও error;
		}
		যদি (skip)
		    ++cmd;
		অন্যথা
		{
		    // Only accept a mark in another file when it is
		    // used by itself: ":'M".
		    fp = getmark(*cmd, to_other_file && cmd[1] == NUL);
		    ++cmd;
		    যদি (fp == (pos_T *)-1)
			// Jumped to another file.
			lnum = curwin->w_cursor.lnum;
		    অন্যথা
		    {
			যদি (check_mark(fp) == FAIL)
			{
			    cmd = ফাঁকা;
			    যাও error;
			}
			lnum = fp->lnum;
		    }
		}
		বিরতি;

	    ক্ষেত্র '/':
	    ক্ষেত্র '?':			// '/' or '?' - search
		c = *cmd++;
		যদি (addr_type != ADDR_সারিগুলি)
		{
		    addr_error(addr_type);
		    cmd = ফাঁকা;
		    যাও error;
		}
		যদি (skip)	// skip "/pat/"
		{
		    cmd = skip_regexp(cmd, c, magic_isset());
		    যদি (*cmd == c)
			++cmd;
		}
		অন্যথা
		{
		    পূর্ণসংখ্যা flags;

		    pos = curwin->w_cursor; // save curwin->w_cursor

		    // When '/' or '?' follows another address, start from
		    // there.
		    যদি (lnum > 0 && lnum != MAXLNUM)
			curwin->w_cursor.lnum =
				lnum > curbuf->b_ml.ml_সারি_গুনতি
					   ? curbuf->b_ml.ml_সারি_গুনতি : lnum;

		    // Start a forward search at the end of the line (unless
		    // before the first line).
		    // Start a backward search at the start of the line.
		    // This makes sure we never match in the current
		    // line, and can match anywhere in the
		    // next/previous line.
		    যদি (c == '/' && curwin->w_cursor.lnum > 0)
			curwin->w_cursor.col = MAXCOL;
		    অন্যথা
			curwin->w_cursor.col = 0;
		    searchcmdlen = 0;
		    flags = silent ? অনুসন্ধান_KEEP : অনুসন্ধান_HIS | অনুসন্ধান_বার্তা;
		    যদি (!do_অনুসন্ধান(ফাঁকা, c, c, cmd, STRLEN(cmd), 1L, flags, ফাঁকা))
		    {
			curwin->w_cursor = pos;
			cmd = ফাঁকা;
			যাও error;
		    }
		    lnum = curwin->w_cursor.lnum;
		    curwin->w_cursor = pos;
		    // adjust command string pointer
		    cmd += searchcmdlen;
		}
		বিরতি;

	    ক্ষেত্র '\\':		    // "\?", "\/" or "\&", repeat search
		++cmd;
		যদি (addr_type != ADDR_সারিগুলি)
		{
		    addr_error(addr_type);
		    cmd = ফাঁকা;
		    যাও error;
		}
		যদি (*cmd == '&')
		    i = RE_SUBST;
		অন্যথা যদি (*cmd == '?' || *cmd == '/')
		    i = RE_অনুসন্ধান;
		অন্যথা
		{
		    emsg(_(e_backslash_should_be_followed_by));
		    cmd = ফাঁকা;
		    যাও error;
		}

		যদি (!skip)
		{
		    /*
		     * When search follows another address, start from
		     * there.
		     */
		    যদি (lnum != MAXLNUM)
			pos.lnum = lnum;
		    অন্যথা
			pos.lnum = curwin->w_cursor.lnum;

		    /*
		     * Start the search just like for the above
		     * do_search().
		     */
		    যদি (*cmd != '?')
			pos.col = MAXCOL;
		    অন্যথা
			pos.col = 0;
		    pos.coladd = 0;
		    যদি (searchit(curwin, curbuf, &pos, ফাঁকা,
				*cmd == '?' ? BACKWARD : FORWARD,
				(char_u *)"", 0, 1L, অনুসন্ধান_বার্তা, i, ফাঁকা) != FAIL)
			lnum = pos.lnum;
		    অন্যথা
		    {
			cmd = ফাঁকা;
			যাও error;
		    }
		}
		++cmd;
		বিরতি;

	    পূর্বনির্ধারিত:
		যদি (VIM_ISDIGIT(*cmd))	// absolute line number
		    lnum = getdigits(&cmd);
	}

	জন্য (;;)
	{
	    cmd = skipwhite(cmd);
	    যদি (*cmd != '-' && *cmd != '+' && !VIM_ISDIGIT(*cmd))
		বিরতি;

	    যদি (lnum == MAXLNUM)
	    {
		নির্বাচন (addr_type)
		{
		    ক্ষেত্র ADDR_সারিগুলি:
		    ক্ষেত্র ADDR_OTHER:
			// "+1" is same as ".+1"
			lnum = curwin->w_cursor.lnum;
			বিরতি;
		    ক্ষেত্র ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			বিরতি;
		    ক্ষেত্র ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			বিরতি;
		    ক্ষেত্র ADDR_LOADED_BUFFERS:
		    ক্ষেত্র ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			বিরতি;
		    ক্ষেত্র ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			বিরতি;
		    ক্ষেত্র ADDR_TABS_RELATIVE:
			lnum = 1;
			বিরতি;
		    ক্ষেত্র ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			বিরতি;
		    ক্ষেত্র ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			বিরতি;
		    ক্ষেত্র ADDR_NONE:
		    ক্ষেত্র ADDR_UNSIGNED:
			lnum = 0;
			বিরতি;
		}
	    }

	    যদি (VIM_ISDIGIT(*cmd))
		i = '+';		// "number" is same as "+number"
	    অন্যথা
		i = *cmd++;
	    যদি (!VIM_ISDIGIT(*cmd))	// '+' is '+1'
		n = 1;
	    অন্যথা
	    {
		// "number", "+number" or "-number"
		n = getdigits(&cmd);
		যদি (n == MAXLNUM)
		{
		    emsg(_(e_সারি_সংখ্যা_out_of_range));
		    cmd = ফাঁকা;
		    যাও error;
		}
	    }

	    যদি (addr_type == ADDR_TABS_RELATIVE)
	    {
		emsg(_(e_invalid_range));
		cmd = ফাঁকা;
		যাও error;
	    }
	    অন্যথা যদি (addr_type == ADDR_LOADED_BUFFERS
		    || addr_type == ADDR_BUFFERS)
		lnum = compute_বাফার_local_গুনতি(
				    addr_type, lnum, (i == '-') ? -1 * n : n);
	    অন্যথা
	    {
#ifdef FEAT_FOLDING
		// Relative line addressing: need to adjust for lines in a
		// closed fold after the first address.
		যদি (addr_type == ADDR_সারিগুলি && (i == '-' || i == '+')
							 && address_গুনতি >= 2)
		    (শূন্য)hasFolding(lnum, ফাঁকা, &lnum);
#endif
		যদি (i == '-')
		    lnum -= n;
		অন্যথা
		{
		    যদি (lnum >= 0 && n >= LONG_MAX - lnum)
		    {
			emsg(_(e_সারি_সংখ্যা_out_of_range));
			cmd = ফাঁকা;
			যাও error;
		    }
		    lnum += n;
		}
	    }
	}
    } যখন (*cmd == '/' || *cmd == '?');

error:
    *ptr = cmd;
    ফেরত lnum;
}

/*
 * Set eap->line1 and eap->line2 to the whole range.
 * Used for commands with the EX_DFLALL flag and no range given.
 */
    স্থির শূন্য
address_default_all(exarg_T *eap)
{
    eap->line1 = 1;
    নির্বাচন (eap->addr_type)
    {
	ক্ষেত্র ADDR_সারিগুলি:
	ক্ষেত্র ADDR_OTHER:
	    eap->line2 = curbuf->b_ml.ml_সারি_গুনতি;
	    বিরতি;
	ক্ষেত্র ADDR_LOADED_BUFFERS:
	    {
		buf_T *buf = firstbuf;

		যখন (buf->b_পরবর্তী != ফাঁকা && buf->b_ml.ml_mfp == ফাঁকা)
		    buf = buf->b_পরবর্তী;
		eap->line1 = buf->b_fnum;
		buf = lastbuf;
		যখন (buf->b_পূর্ব != ফাঁকা && buf->b_ml.ml_mfp == ফাঁকা)
		    buf = buf->b_পূর্ব;
		eap->line2 = buf->b_fnum;
	    }
	    বিরতি;
	ক্ষেত্র ADDR_BUFFERS:
	    eap->line1 = firstbuf->b_fnum;
	    eap->line2 = lastbuf->b_fnum;
	    বিরতি;
	ক্ষেত্র ADDR_WINDOWS:
	    eap->line2 = LAST_WIN_NR;
	    বিরতি;
	ক্ষেত্র ADDR_TABS:
	    eap->line2 = LAST_TAB_NR;
	    বিরতি;
	ক্ষেত্র ADDR_TABS_RELATIVE:
	    eap->line2 = 1;
	    বিরতি;
	ক্ষেত্র ADDR_ARGUMENTS:
	    যদি (ARGCOUNT == 0)
		eap->line1 = eap->line2 = 0;
	    অন্যথা
		eap->line2 = ARGCOUNT;
	    বিরতি;
	ক্ষেত্র ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    eap->line2 = qf_get_valid_আকার(eap);
	    যদি (eap->line2 == 0)
		eap->line2 = 1;
#endif
	    বিরতি;
	ক্ষেত্র ADDR_NONE:
	ক্ষেত্র ADDR_UNSIGNED:
	ক্ষেত্র ADDR_QUICKFIX:
	    iemsg("Cannot use EX_DFLALL with ADDR_NONE, ADDR_UNSIGNED or ADDR_QUICKFIX");
	    বিরতি;
    }
}


/*
 * Get flags from an Ex command argument.
 */
    স্থির শূন্য
get_flags(exarg_T *eap)
{
    যখন (vim_strchr((char_u *)"lp#", *eap->arg) != ফাঁকা)
    {
	যদি (*eap->arg == 'l')
	    eap->flags |= EXFLAG_LIST;
	অন্যথা যদি (*eap->arg == 'p')
	    eap->flags |= EXFLAG_PRINT;
	অন্যথা
	    eap->flags |= EXFLAG_NR;
	eap->arg = skipwhite(eap->arg + 1);
    }
}

/*
 * Function called for command which is Not Implemented.  NI!
 */
    শূন্য
ex_ni(exarg_T *eap)
{
    যদি (!eap->skip)
	eap->errmsg =
		_(e_sorry_command_is_not_available_in_this_version);
}

#ifdef HAVE_EX_SCRIPT_NI
/*
 * Function called for script command which is Not Implemented.  NI!
 * Skips over ":perl <<EOF" constructs.
 */
    স্থির শূন্য
ex_script_ni(exarg_T *eap)
{
    যদি (!eap->skip)
	ex_ni(eap);
    অন্যথা
	vim_free(script_get(eap, eap->arg));
}
#endif

/*
 * Check range in Ex command for validity.
 * Return NULL when valid, error message when invalid.
 */
    স্থির অক্ষর *
invalid_range(exarg_T *eap)
{
    buf_T	*buf;

    যদি (       eap->line1 < 0
	    || eap->line2 < 0
	    || eap->line1 > eap->line2)
	ফেরত _(e_invalid_range);

    যদি (eap->argt & EX_RANGE)
    {
	নির্বাচন (eap->addr_type)
	{
	    ক্ষেত্র ADDR_সারিগুলি:
		যদি (eap->line2 > curbuf->b_ml.ml_সারি_গুনতি
#ifdef FEAT_DIFF
			    + (eap->cmdidx == CMD_diffget ||
				eap->cmdidx == CMD_diffput)
#endif
		   )
		    ফেরত _(e_invalid_range);
		বিরতি;
	    ক্ষেত্র ADDR_ARGUMENTS:
		// add 1 if ARGCOUNT is 0
		যদি (eap->line2 > ARGCOUNT + (!ARGCOUNT))
		    ফেরত _(e_invalid_range);
		বিরতি;
	    ক্ষেত্র ADDR_BUFFERS:
		// Only a boundary check, not whether the buffers actually
		// exist.
		যদি (eap->line1 < 1 || eap->line2 > get_highest_fnum())
		    ফেরত _(e_invalid_range);
		বিরতি;
	    ক্ষেত্র ADDR_LOADED_BUFFERS:
		buf = firstbuf;
		যখন (buf->b_ml.ml_mfp == ফাঁকা)
		{
		    যদি (buf->b_পরবর্তী == ফাঁকা)
			ফেরত _(e_invalid_range);
		    buf = buf->b_পরবর্তী;
		}
		যদি (eap->line1 < buf->b_fnum)
		    ফেরত _(e_invalid_range);
		buf = lastbuf;
		যখন (buf->b_ml.ml_mfp == ফাঁকা)
		{
		    যদি (buf->b_পূর্ব == ফাঁকা)
			ফেরত _(e_invalid_range);
		    buf = buf->b_পূর্ব;
		}
		যদি (eap->line2 > buf->b_fnum)
		    ফেরত _(e_invalid_range);
		বিরতি;
	    ক্ষেত্র ADDR_WINDOWS:
		যদি (eap->line2 > LAST_WIN_NR)
		    ফেরত _(e_invalid_range);
		বিরতি;
	    ক্ষেত্র ADDR_TABS:
		যদি (eap->line2 > LAST_TAB_NR)
		    ফেরত _(e_invalid_range);
		বিরতি;
	    ক্ষেত্র ADDR_TABS_RELATIVE:
	    ক্ষেত্র ADDR_OTHER:
		// Any range is OK.
		বিরতি;
	    ক্ষেত্র ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
		// No error for value that is too big, will use the last entry.
		যদি (eap->line2 <= 0)
		{
		    যদি (eap->addr_গুনতি == 0)
			ফেরত _(e_no_errors);
		    ফেরত _(e_invalid_range);
		}
#endif
		বিরতি;
	    ক্ষেত্র ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
		যদি ((eap->line2 != 1 && eap->line2 > qf_get_valid_আকার(eap))
			|| eap->line2 < 0)
		    ফেরত _(e_invalid_range);
#endif
		বিরতি;
	    ক্ষেত্র ADDR_UNSIGNED:
	    ক্ষেত্র ADDR_NONE:
		// Will give an error elsewhere.
		বিরতি;
	}
    }
    ফেরত ফাঁকা;
}

/*
 * Correct the range for zero line number, if required.
 */
    স্থির শূন্য
correct_range(exarg_T *eap)
{
    যদি (!(eap->argt & EX_ZEROR))	    // zero in range not allowed
    {
	যদি (eap->line1 == 0)
	    eap->line1 = 1;
	যদি (eap->line2 == 0)
	    eap->line2 = 1;
    }
}

#ifdef FEAT_QUICKFIX
/*
 * For a ":vimgrep" or ":vimgrepadd" command return a pointer past the
 * pattern.  Otherwise return eap->arg.
 */
    স্থির char_u *
skip_grep_pat(exarg_T *eap)
{
    char_u	*p = eap->arg;

    যদি (*p != NUL && (eap->cmdidx == CMD_vimgrep || eap->cmdidx == CMD_lvimgrep
		|| eap->cmdidx == CMD_vimgrepadd
		|| eap->cmdidx == CMD_lvimgrepadd
		|| grep_internal(eap->cmdidx)))
    {
	p = skip_vimgrep_pat(p, ফাঁকা, ফাঁকা);
	যদি (p == ফাঁকা)
	    p = eap->arg;
    }
    ফেরত p;
}

/*
 * For the ":make" and ":grep" commands insert the 'makeprg'/'grepprg' option
 * in the command line, so that things like % get expanded.
 */
    স্থির char_u *
replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep)
{
    char_u	*new_cmdline;
    char_u	*program;
    char_u	*pos;
    char_u	*ptr;
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		i;

    /*
     * Don't do it when ":vimgrep" is used for ":grep".
     */
    যদি ((eap->cmdidx == CMD_make || eap->cmdidx == CMD_lmake
		     || eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
		     || eap->cmdidx == CMD_grepadd
		     || eap->cmdidx == CMD_lgrepadd)
	    && !grep_internal(eap->cmdidx))
    {
	যদি (eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
	    || eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	{
	    যদি (*curbuf->b_p_gp == NUL)
		program = p_gp;
	    অন্যথা
		program = curbuf->b_p_gp;
	}
	অন্যথা
	{
	    যদি (*curbuf->b_p_mp == NUL)
		program = p_mp;
	    অন্যথা
		program = curbuf->b_p_mp;
	}

	p = skipwhite(p);

	যদি ((pos = (char_u *)strstr((অক্ষর *)program, "$*")) != ফাঁকা)
	{
	    // replace $* by given arguments
	    i = 1;
	    যখন ((pos = (char_u *)strstr((অক্ষর *)pos + 2, "$*")) != ফাঁকা)
		++i;
	    len = (পূর্ণসংখ্যা)STRLEN(p);
	    new_cmdline = alloc(STRLEN(program) + (size_t)i * (len - 2) + 1);
	    যদি (new_cmdline == ফাঁকা)
		ফেরত ফাঁকা;			// out of memory
	    ptr = new_cmdline;
	    যখন ((pos = (char_u *)strstr((অক্ষর *)program, "$*")) != ফাঁকা)
	    {
		i = (পূর্ণসংখ্যা)(pos - program);
		STRNCPY(ptr, program, i);
		STRCPY(ptr += i, p);
		ptr += len;
		program = pos + 2;
	    }
	    STRCPY(ptr, program);
	}
	অন্যথা
	{
	    size_t program_len = STRLEN(program);

	    new_cmdline = alloc(program_len + STRLEN(p) + 2);
	    যদি (new_cmdline == ফাঁকা)
		ফেরত ফাঁকা;			// out of memory
	    STRCPY(new_cmdline, program);
	    STRCPY(new_cmdline + program_len, " ");
	    STRCPY(new_cmdline + program_len + 1, p);
	}
	বার্তা_make(p);

	// 'eap->cmd' is not set here, because it is not used at CMD_make
	vim_free(*cmdlinep);
	*cmdlinep = new_cmdline;
	p = new_cmdline;
    }
    ফেরত p;
}
#endif

/*
 * Expand file name in Ex command argument.
 * When an error is detected, "errormsgp" is set to a non-NULL pointer.
 * Return FAIL for failure, OK otherwise.
 */
    পূর্ণসংখ্যা
expand_filename(
    exarg_T	*eap,
    char_u	**cmdlinep,
    অক্ষর	**errormsgp)
{
    পূর্ণসংখ্যা		has_wildcards;	// need to expand wildcards
    char_u	*repl;
    size_t	srclen;
    char_u	*p;
    পূর্ণসংখ্যা		n;
    পূর্ণসংখ্যা		escaped;

#ifdef FEAT_QUICKFIX
    // Skip a regexp pattern for ":vimgrep[add] pat file..."
    p = skip_grep_pat(eap);
#অন্যথা
    p = eap->arg;
#endif

    /*
     * Decide to expand wildcards *before* replacing '%', '#', etc.  If
     * the file name contains a wildcard it should not cause expanding.
     * (it will be expanded anyway if there is a wildcard before replacing).
     */
    has_wildcards = mch_has_wildcard(p);
    যখন (*p != NUL)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`, wildcards in it are not expanded.
	যদি (p[0] == '`' && p[1] == '=')
	{
	    p += 2;
	    (শূন্য)skip_রাশিমালা(&p, ফাঁকা);
	    যদি (*p == '`')
		++p;
	    চালু;
	}
#endif
	/*
	 * Quick check if this cannot be the start of a special string.
	 * Also removes backslash before '%', '#' and '<'.
	 */
	যদি (vim_strchr((char_u *)"%#<", *p) == ফাঁকা)
	{
	    ++p;
	    চালু;
	}

	/*
	 * Try to find a match at this position.
	 */
	repl = eval_vars(p, eap->arg, &srclen, &(eap->do_ecmd_lnum),
						    errormsgp, &escaped, TRUE);
	যদি (*errormsgp != ফাঁকা)		// error detected
	    ফেরত FAIL;
	যদি (repl == ফাঁকা)		// no match found
	{
	    p += srclen;
	    চালু;
	}

	// Wildcards won't be expanded below, the replacement is taken
	// literally.  But do expand "~/file", "~user/file" and "$HOME/file".
	যদি (vim_strchr(repl, '$') != ফাঁকা || vim_strchr(repl, '~') != ফাঁকা)
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
	যদি (!eap->usefilter
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
	    স্থির char_u *nobslash = (char_u *)" \t\"|";
# define ESCAPE_CHARS nobslash
#অন্যথা
# define ESCAPE_CHARS escape_chars
#endif

	    জন্য (l = repl; *l; ++l)
		যদি (vim_strchr(ESCAPE_CHARS, *l) != ফাঁকা)
		{
		    l = vim_strsave_escaped(repl, ESCAPE_CHARS);
		    যদি (l != ফাঁকা)
		    {
			vim_free(repl);
			repl = l;
		    }
		    বিরতি;
		}
	}

	// For a shell command a '!' must be escaped.
	যদি ((eap->usefilter || eap->cmdidx == CMD_bang
						|| eap->cmdidx == CMD_terminal)
			    && vim_strpbrk(repl, (char_u *)"!") != ফাঁকা)
	{
	    char_u	*l;

	    l = vim_strsave_escaped(repl, (char_u *)"!");
	    যদি (l != ফাঁকা)
	    {
		vim_free(repl);
		repl = l;
	    }
	}

	p = repl_cmdline(eap, p, srclen, repl, cmdlinep);
	vim_free(repl);
	যদি (p == ফাঁকা)
	    ফেরত FAIL;
    }

    /*
     * One file argument: Expand wildcards.
     * Don't do this with ":r !command" or ":w !command".
     */
    যদি ((eap->argt & EX_NOSPC) && !eap->usefilter)
    {
	/*
	 * May do this twice:
	 * 1. Replace environment variables.
	 * 2. Replace any other wildcards, remove backslashes.
	 */
	জন্য (n = 1; n <= 2; ++n)
	{
	    যদি (n == 2)
	    {
		/*
		 * Halve the number of backslashes (this is Vi compatible).
		 * For Unix and OS/2, when wildcards are expanded, this is
		 * done by ExpandOne() below.
		 */
#যদি defined(UNIX)
		যদি (!has_wildcards)
#endif
		    backslash_halve(eap->arg);
	    }

	    যদি (has_wildcards)
	    {
		যদি (n == 1)
		{
		    /*
		     * First loop: May expand environment variables.  This
		     * can be done much faster with expand_env() than with
		     * something else (e.g., calling a shell).
		     * After expanding environment variables, check again
		     * if there are still wildcards present.
		     */
		    যদি (vim_strchr(eap->arg, '$') != ফাঁকা
			    || vim_strchr(eap->arg, '~') != ফাঁকা)
		    {
			expand_env_esc(eap->arg, NameBuff, MAXPATHL,
					(char_u *)(" \t" PATH_ESC_WILDCARDS),
					TRUE, ফাঁকা);
			has_wildcards = mch_has_wildcard(NameBuff);
			p = NameBuff;
		    }
		    অন্যথা
			p = ফাঁকা;
		}
		অন্যথা // n == 2
		{
		    expand_T	xpc;
		    পূর্ণসংখ্যা		options = WILD_LIST_NOTFOUND
					       | WILD_NOERROR | WILD_ADD_SLASH;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    যদি (p_wic)
			options += WILD_ICASE;
		    p = ExpandOne(&xpc, eap->arg, ফাঁকা,
						   options, WILD_EXPAND_FREE);
		    যদি (p == ফাঁকা)
			ফেরত FAIL;
		}
		যদি (p != ফাঁকা)
		{
		    (শূন্য)repl_cmdline(eap, eap->arg, STRLEN(eap->arg),
								 p, cmdlinep);
		    যদি (n == 2)	// p came from ExpandOne()
			vim_free(p);
		}
	    }
	}
    }
    ফেরত OK;
}

/*
 * Replace part of the command line, keeping eap->cmd, eap->arg and
 * eap->nextcmd correct.
 * "src" points to the part that is to be replaced, of length "srclen".
 * "repl" is the replacement string.
 * Returns a pointer to the character after the replaced string.
 * Returns NULL for failure.
 */
    স্থির char_u *
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
    যদি (eap->nextcmd != ফাঁকা)
	i += STRLEN(eap->nextcmd);	// add space for next command
    যদি ((new_cmdline = alloc(i)) == ফাঁকা)
	ফেরত ফাঁকা;			// out of memory!

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

    যদি (eap->nextcmd != ফাঁকা)		// append next command
    {
	new_cmdlinelen += taillen + 1;
	STRCPY(new_cmdline + new_cmdlinelen, eap->nextcmd);
	eap->nextcmd = new_cmdline + new_cmdlinelen;
    }
    eap->cmd = new_cmdline + (eap->cmd - *cmdlinep);
    eap->arg = new_cmdline + (eap->arg - *cmdlinep);
    যদি (eap->do_ecmd_cmd != ফাঁকা && eap->do_ecmd_cmd != dollar_command)
	eap->do_ecmd_cmd = new_cmdline + (eap->do_ecmd_cmd - *cmdlinep);
    vim_free(*cmdlinep);
    *cmdlinep = new_cmdline;

    ফেরত src;
}

/*
 * Check for '|' to separate commands and '"' to start comments.
 * If "keep_backslash" is TRUE do not remove any backslash.
 */
    শূন্য
separate_nextcmd(exarg_T *eap, পূর্ণসংখ্যা keep_backslash)
{
    পূর্ণসংখ্যা		vim9script = in_vim9script();
    char_u	*p;

#ifdef FEAT_QUICKFIX
    p = skip_grep_pat(eap);
#অন্যথা
    p = eap->arg;
#endif

    জন্য ( ; *p; MB_PTR_ADV(p))
    {
	যদি (*p == Ctrl_V)
	{
	    যদি ((eap->argt & (EX_CTRLV | EX_XFILE)) || keep_backslash)
		++p;		// skip CTRL-V and next char
	    অন্যথা
				// remove CTRL-V and skip next char
		STRMOVE(p, p + 1);
	    যদি (*p == NUL)		// stop at NUL after CTRL-V
		বিরতি;
	}

#ifdef FEAT_EVAL
	// Skip over `=expr` when wildcards are expanded.
	অন্যথা যদি (p[0] == '`' && p[1] == '=' && (eap->argt & EX_XFILE))
	{
	    p += 2;
	    (শূন্য)skip_রাশিমালা(&p, ফাঁকা);
	    যদি (*p == NUL)		// stop at NUL after CTRL-V
		বিরতি;
	}
#endif

	// Check for '"'/'#': start of comment or '|': next command
	// :@" and :*" do not start a comment!
	// :redir @" doesn't either.
	অন্যথা যদি ((*p == '"'
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
		    && eap->cmdidx != CMD_ঢোকাও)
		|| *p == '\n')
	{
	    /*
	     * We remove the '\' before the '|', unless EX_CTRLV is used
	     * AND 'b' is present in 'cpoptions'.
	     */
	    যদি ((vim_strchr(p_cpo, CPO_BAR) == ফাঁকা
			      || !(eap->argt & EX_CTRLV)) && *(p - 1) == '\\')
	    {
		যদি (!keep_backslash)
		{
		    STRMOVE(p - 1, p);	// remove the '\'
		    --p;
		}
	    }
	    অন্যথা
	    {
		set_nextcmd(eap, p);
		*p = NUL;
		বিরতি;
	    }
	}
    }

    যদি (!(eap->argt & EX_NOTRLCOM))	// remove trailing spaces
	del_trailing_spaces(eap->arg);
}

/*
 * get + command from ex argument
 */
    স্থির char_u *
getargcmd(char_u **argp)
{
    char_u *arg = *argp;
    char_u *command = ফাঁকা;

    যদি (*arg == '+')	    // +[command]
    {
	++arg;
	যদি (vim_isspace(*arg) || *arg == NUL)
	    command = dollar_command;
	অন্যথা
	{
	    command = arg;
	    arg = skip_cmd_arg(command, TRUE);
	    যদি (*arg != NUL)
		*arg++ = NUL;		// terminate command with NUL
	}

	arg = skipwhite(arg);	// skip over spaces
	*argp = arg;
    }
    ফেরত command;
}

/*
 * Find end of "+command" argument.  Skip over "\ " and "\\".
 */
    char_u *
skip_cmd_arg(
    char_u *p,
    পূর্ণসংখ্যা	   rembs)	// TRUE to halve the number of backslashes
{
    যখন (*p && !vim_isspace(*p))
    {
	যদি (*p == '\\' && p[1] != NUL)
	{
	    যদি (rembs)
		STRMOVE(p, p + 1);
	    অন্যথা
		++p;
	}
	MB_PTR_ADV(p);
    }
    ফেরত p;
}

    পূর্ণসংখ্যা
get_bad_opt(char_u *p, exarg_T *eap)
{
    যদি (STRICMP(p, "keep") == 0)
	eap->bad_char = BAD_KEEP;
    অন্যথা যদি (STRICMP(p, "drop") == 0)
	eap->bad_char = BAD_DROP;
    অন্যথা যদি (MB_BYTE2LEN(*p) == 1 && p[1] == NUL)
	eap->bad_char = *p;
    অন্যথা
	ফেরত FAIL;
    ফেরত OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of bad= names.
 */
    স্থির char_u *
get_bad_name(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    // Note: Keep this in sync with getargopt.
    স্থির অক্ষর *(p_bad_মানসমূহ[]) =
    {
	"?",
	"keep",
	"drop",
    };

    যদি (idx < 0 || idx >= (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(p_bad_মানসমূহ))
	ফেরত ফাঁকা;

    ফেরত (char_u*)p_bad_মানসমূহ[idx];
}

/*
 * Get "++opt=arg" argument.
 * Return FAIL or OK.
 */
    স্থির পূর্ণসংখ্যা
getargopt(exarg_T *eap)
{
    char_u	*arg = eap->arg + 2;
    পূর্ণসংখ্যা		*pp = ফাঁকা;
    পূর্ণসংখ্যা		bad_char_idx;
    char_u	*p;

    // Note: Keep this in sync with get_argopt_name.

    // ":edit ++[no]bin[ary] file"
    যদি (STRNCMP(arg, "bin", 3) == 0 || STRNCMP(arg, "nobin", 5) == 0)
    {
	যদি (*arg == 'n')
	{
	    arg += 2;
	    eap->force_bin = FORCE_NOBIN;
	}
	অন্যথা
	    eap->force_bin = FORCE_BIN;
	যদি (!checkforcmd(&arg, "binary", 3))
	    ফেরত FAIL;
	eap->arg = skipwhite(arg);
	ফেরত OK;
    }

    // ":read ++edit file"
    যদি (STRNCMP(arg, "edit", 4) == 0)
    {
	eap->read_edit = TRUE;
	eap->arg = skipwhite(arg + 4);
	ফেরত OK;
    }

    যদি (STRNCMP(arg, "ff", 2) == 0)
    {
	arg += 2;
	pp = &eap->force_ff;
    }
    অন্যথা যদি (STRNCMP(arg, "fileformat", 10) == 0)
    {
	arg += 10;
	pp = &eap->force_ff;
    }
    অন্যথা যদি (STRNCMP(arg, "enc", 3) == 0)
    {
	যদি (STRNCMP(arg, "encoding", 8) == 0)
	    arg += 8;
	অন্যথা
	    arg += 3;
	pp = &eap->force_enc;
    }
    অন্যথা যদি (STRNCMP(arg, "bad", 3) == 0)
    {
	arg += 3;
	pp = &bad_char_idx;
    }

    যদি (pp == ফাঁকা || *arg != '=')
	ফেরত FAIL;

    ++arg;
    *pp = (পূর্ণসংখ্যা)(arg - eap->cmd);
    arg = skip_cmd_arg(arg, FALSE);
    eap->arg = skipwhite(arg);
    *arg = NUL;

    যদি (pp == &eap->force_ff)
    {
	যদি (check_ff_মান(eap->cmd + eap->force_ff) == FAIL)
	    ফেরত FAIL;
	eap->force_ff = eap->cmd[eap->force_ff];
    }
    অন্যথা যদি (pp == &eap->force_enc)
    {
	// Make 'fileencoding' lower case.
	জন্য (p = eap->cmd + eap->force_enc; *p != NUL; ++p)
	    *p = TOLOWER_ASC(*p);
    }
    অন্যথা
    {
	// Check ++bad= argument.  Must be a single-byte character, "keep" or
	// "drop".
	যদি (get_bad_opt(eap->cmd + bad_char_idx, eap) == FAIL)
	    ফেরত FAIL;
    }

    ফেরত OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of ++opt names.
 */
    স্থির char_u *
get_argopt_name(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    // Note: Keep this in sync with getargopt.
    স্থির অক্ষর *(p_opt_মানসমূহ[]) =
    {
	"fileformat=",
	"encoding=",
	"binary",
	"nobinary",
	"bad=",
	"edit",
    };

    যদি (idx < 0 || idx >= (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(p_opt_মানসমূহ))
	ফেরত ফাঁকা;

    ফেরত (char_u*)p_opt_মানসমূহ[idx];
}

/*
 * Command-line expansion for ++opt=name.
 */
    পূর্ণসংখ্যা
expand_argopt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	পূর্ণসংখ্যা	    *অঙ্ক_Matches)
{
    যদি (xp->xp_pattern > xp->xp_সারি && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, পূর্ণসংখ্যা) = ফাঁকা;

	char_u *name_end = xp->xp_pattern - 1;
	যদি (name_end - xp->xp_সারি >= 2
		&& STRNCMP(name_end - 2, "ff", 2) == 0)
	    cb = get_fileformat_name;
	অন্যথা যদি (name_end - xp->xp_সারি >= 10
		&& STRNCMP(name_end - 10, "fileformat", 10) == 0)
	    cb = get_fileformat_name;
	অন্যথা যদি (name_end - xp->xp_সারি >= 3
		&& STRNCMP(name_end - 3, "enc", 3) == 0)
	    cb = get_encoding_name;
	অন্যথা যদি (name_end - xp->xp_সারি >= 8
		&& STRNCMP(name_end - 8, "encoding", 8) == 0)
	    cb = get_encoding_name;
	অন্যথা যদি (name_end - xp->xp_সারি >= 3
		&& STRNCMP(name_end - 3, "bad", 3) == 0)
	    cb = get_bad_name;

	যদি (cb != ফাঁকা)
	{
	    ফেরত ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    অঙ্ক_Matches,
		    cb,
		    FALSE);
	}
	ফেরত FAIL;
    }

    // Special handling of "ff" which acts as a short form of
    // "fileformat", as "ff" is not a substring of it.
    যদি (xp->xp_pattern_len == 2
	    && STRNCMP(xp->xp_pattern, "ff", xp->xp_pattern_len) == 0)
    {
	*matches = ALLOC_MULT(char_u *, 1);
	যদি (*matches == ফাঁকা)
	    ফেরত FAIL;
	*অঙ্ক_Matches = 1;
	(*matches)[0] = vim_strsave((char_u*)"fileformat=");
	ফেরত OK;
    }

    ফেরত ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    অঙ্ক_Matches,
	    get_argopt_name,
	    FALSE);
}

    স্থির শূন্য
ex_autocmd(exarg_T *eap)
{
    যদি (eap->skip)
    {
	skip_cmd_block(eap);
	ফেরত;
    }

    /*
     * Disallow autocommands from .exrc and .vimrc in current
     * directory for security reasons.
     */
    যদি (secure)
    {
	secure = 2;
	eap->errmsg =
	      _(e_command_not_allowed_from_vimrc_in_current_dir_or_tag_অনুসন্ধান);
    }
    অন্যথা যদি (eap->cmdidx == CMD_autocmd)
	do_autocmd(eap, eap->arg, eap->forceit);
    অন্যথা
	do_augroup(eap->arg, eap->forceit);
}

/*
 * ":doautocmd": Apply the automatic commands to the current buffer.
 */
    স্থির শূন্য
ex_doautocmd(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    পূর্ণসংখ্যা		call_do_modelines = check_nomodeline(&arg);
    পূর্ণসংখ্যা		did_aucmd;

    (শূন্য)do_doautocmd(arg, TRUE, &did_aucmd);
    // Only when there is no <nomodeline>.
    যদি (call_do_modelines && did_aucmd)
	do_modelines(0);
}

/*
 * :[N]bunload[!] [N] [bufname] unload buffer
 * :[N]bdelete[!] [N] [bufname] delete buffer from buffer list
 * :[N]bwipeout[!] [N] [bufname] delete buffer really
 */
    স্থির শূন্য
ex_bunload(exarg_T *eap)
{
    যদি (ERROR_IF_ANY_POPUP_WINDOW)
	ফেরত;
    eap->errmsg = do_bufdel(
	    eap->cmdidx == CMD_bdelete ? DOBUF_DEL
		: eap->cmdidx == CMD_bwipeout ? DOBUF_WIPE
		: DOBUF_UNLOAD, eap->arg,
	    eap->addr_গুনতি, (পূর্ণসংখ্যা)eap->line1, (পূর্ণসংখ্যা)eap->line2, eap->forceit);
}

/*
 * :[N]buffer [N]	to buffer N
 * :[N]sbuffer [N]	to buffer N
 */
    স্থির শূন্য
ex_বাফার(exarg_T *eap)
{
    যদি (ERROR_IF_ANY_POPUP_WINDOW)
	ফেরত;
    do_exbuffer(eap);
}

/*
 * ":buffer" command and alike.
 */
    স্থির শূন্য
do_exbuffer(exarg_T *eap)
{
    যদি (*eap->arg)
	eap->errmsg = ex_errmsg(e_trailing_characters_str, eap->arg);
    অন্যথা
    {
	যদি (eap->addr_গুনতি == 0)	// default is current buffer
	    goto_বাফার(eap, DOBUF_CURRENT, FORWARD, 0);
	অন্যথা
	    goto_বাফার(eap, DOBUF_FIRST, FORWARD, (পূর্ণসংখ্যা)eap->line2);
	যদি (eap->do_ecmd_cmd != ফাঁকা)
	    do_cmd_argument(eap->do_ecmd_cmd);
    }
}

/*
 * :[N]bmodified [N]	to next mod. buffer
 * :[N]sbmodified [N]	to next mod. buffer
 */
    স্থির শূন্য
ex_bmodified(exarg_T *eap)
{
    goto_বাফার(eap, DOBUF_MOD, FORWARD, (পূর্ণসংখ্যা)eap->line2);
    যদি (eap->do_ecmd_cmd != ফাঁকা)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bnext [N]	to next buffer
 * :[N]sbnext [N]	split and to next buffer
 */
    স্থির শূন্য
ex_bnext(exarg_T *eap)
{
    যদি (ERROR_IF_ANY_POPUP_WINDOW)
	ফেরত;

    goto_বাফার(eap, DOBUF_CURRENT, FORWARD, (পূর্ণসংখ্যা)eap->line2);
    যদি (eap->do_ecmd_cmd != ফাঁকা)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bNext [N]	to previous buffer
 * :[N]bprevious [N]	to previous buffer
 * :[N]sbNext [N]	split and to previous buffer
 * :[N]sbprevious [N]	split and to previous buffer
 */
    স্থির শূন্য
ex_bprevious(exarg_T *eap)
{
    যদি (ERROR_IF_ANY_POPUP_WINDOW)
	ফেরত;

    goto_বাফার(eap, DOBUF_CURRENT, BACKWARD, (পূর্ণসংখ্যা)eap->line2);
    যদি (eap->do_ecmd_cmd != ফাঁকা)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :brewind		to first buffer
 * :bfirst		to first buffer
 * :sbrewind		split and to first buffer
 * :sbfirst		split and to first buffer
 */
    স্থির শূন্য
ex_brewind(exarg_T *eap)
{
    যদি (ERROR_IF_ANY_POPUP_WINDOW)
	ফেরত;

    goto_বাফার(eap, DOBUF_FIRST, FORWARD, 0);
    যদি (eap->do_ecmd_cmd != ফাঁকা)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :blast		to last buffer
 * :sblast		split and to last buffer
 */
    স্থির শূন্য
ex_blast(exarg_T *eap)
{
    যদি (ERROR_IF_ANY_POPUP_WINDOW)
	ফেরত;

    goto_বাফার(eap, DOBUF_LAST, BACKWARD, 0);
    যদি (eap->do_ecmd_cmd != ফাঁকা)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * Check if "c" ends an Ex command.
 * In Vim9 script does not check for white space before #.
 */
    পূর্ণসংখ্যা
ends_excmd(পূর্ণসংখ্যা c)
{
    পূর্ণসংখ্যা comment_char = '"';

    যদি (in_vim9script())
	comment_char = '#';
    ফেরত (c == NUL || c == '|' || c == comment_char || c == '\n');
}

/*
 * Like ends_excmd() but checks that a # in Vim9 script either has "cmd" equal
 * to "cmd_start" or has a white space character before it.
 */
    পূর্ণসংখ্যা
ends_excmd2(char_u *cmd_start UNUSED, char_u *cmd)
{
    পূর্ণসংখ্যা c = *cmd;

    যদি (c == NUL || c == '|' || c == '\n')
	ফেরত TRUE;
    যদি (in_vim9script())
	//  # starts a comment, #{ might be a mistake, #{{ can start a fold
	ফেরত c == '#' && (cmd[1] != '{' || cmd[2] == '{')
				 && (cmd == cmd_start || VIM_ISWHITE(cmd[-1]));
    ফেরত c == '"';
}

#যদি defined(FEAT_SYN_HL) || defined(FEAT_অনুসন্ধান_EXTRA) || defined(FEAT_EVAL)
/*
 * Return the next command, after the first '|' or '\n'.
 * Return NULL if not found.
 */
    char_u *
find_nextcmd(char_u *p)
{
    যখন (*p != '|' && *p != '\n')
    {
	যদি (*p == NUL)
	    ফেরত ফাঁকা;
	++p;
    }
    ফেরত (p + 1);
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

    যদি (*s == '|' || *s == '\n')
	ফেরত (s + 1);
    অন্যথা
	ফেরত ফাঁকা;
}

/*
 * If "eap->nextcmd" is not set, check for a next command at "p".
 */
    শূন্য
set_nextcmd(exarg_T *eap, char_u *arg)
{
    char_u *p = skipwhite(arg);

    যদি (in_vim9script() && *p == '#')
    {
	char_u *nl = vim_strchr(p, NL);
	যদি (nl != ফাঁকা)
	    p = nl;
    }

    p = check_nextcmd(p);

    যদি (eap->nextcmd == ফাঁকা)
	eap->nextcmd = p;
    অন্যথা যদি (p != ফাঁকা)
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
    স্থির পূর্ণসংখ্যা
check_more(
    পূর্ণসংখ্যা message,	    // when FALSE check only, no messages
    পূর্ণসংখ্যা forceit)
{
    পূর্ণসংখ্যা	    n = ARGCOUNT - curwin->w_arg_idx - 1;

    যদি (!forceit && only_one_window()
	    && ARGCOUNT > 1 && !arg_had_last && n > 0 && quitmore == 0)
    {
	যদি (message)
	{
#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    যদি ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						    && curbuf->b_fname != ফাঁকা)
	    {
		char_u	buff[DIALOG_বার্তা_আকার];

		vim_snprintf((অক্ষর *)buff, DIALOG_বার্তা_আকার,
			NGETTEXT("%d more file to edit.  Quit anyway?",
			    "%d more files to edit.  Quit anyway?", n), n);
		যদি (vim_dialog_yesno(VIM_QUESTION, ফাঁকা, buff, 1) == VIM_YES)
		    ফেরত OK;
		ফেরত FAIL;
	    }
#endif
	    semsg(NGETTEXT(e_nr_more_file_to_edit,
			   e_nr_more_files_to_edit , n), n);
	    quitmore = 2;	    // next try to quit is allowed
	}
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of command names.
 */
    char_u *
get_command_name(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    যদি (idx >= (পূর্ণসংখ্যা)CMD_আকার)
	ফেরত expand_user_command_name(idx);
    // the following are no real commands
    যদি (STRNCMP(cmdnames[idx].cmd_name, "{", 1) == 0 ||
	STRNCMP(cmdnames[idx].cmd_name, "}", 1) == 0)
	ফেরত (char_u *)"";
    ফেরত cmdnames[idx].cmd_name;
}

    স্থির শূন্য
ex_colorscheme(exarg_T *eap)
{
    যদি (*eap->arg == NUL)
    {
#ifdef FEAT_EVAL
	char_u *রাশিমালা = vim_strsave((char_u *)"g:colors_name");
	char_u *p = ফাঁকা;

	যদি (রাশিমালা != ফাঁকা)
	{
	    ++emsg_off;
	    p = eval_to_string(রাশিমালা, FALSE, FALSE);
	    --emsg_off;
	    vim_free(রাশিমালা);
	}
	যদি (p != ফাঁকা)
	{
	    বার্তা((অক্ষর *)p);
	    vim_free(p);
	}
	অন্যথা
	    বার্তা("default");
#অন্যথা
	বার্তা(_("unknown"));
#endif
    }
    অন্যথা যদি (load_colors(eap->arg) == FAIL)
	semsg(_(e_cannot_find_color_scheme_str), eap->arg);

#ifdef FEAT_VTP
    অন্যথা যদি (has_vtp_working())
    {
	// background color change requires clear + redraw
	update_screen(UPD_CLEAR);
	redrawcmd();
    }
#endif
}

    স্থির শূন্য
ex_highlight(exarg_T *eap)
{
    যদি (*eap->arg == NUL && eap->cmd[2] == '!')
	বার্তা(_("Greetings, Vim user!"));
    do_highlight(eap->arg, eap->forceit, FALSE);
}


/*
 * Call this function if we thought we were going to exit, but we won't
 * (because of an error).  May need to restore the terminal mode.
 */
    শূন্য
not_exiting(পূর্ণসংখ্যা save_exiting)
{
    exiting = save_exiting;
    settmode(TMODE_RAW);
}

    পূর্ণসংখ্যা
before_quit_autocmds(win_T *wp, পূর্ণসংখ্যা quit_all, পূর্ণসংখ্যা forceit)
{
    apply_autocmds(EVENT_QUITPRE, ফাঁকা, ফাঁকা, FALSE, wp->w_বাফার);

    // Bail out when autocommands closed the window.
    // Refuse to quit when the buffer in the last window is being closed (can
    // only happen in autocommands).
    যদি (!win_valid(wp)
	    || curbuf_locked()
	    || (wp->w_বাফার->b_nwindows == 1 && wp->w_বাফার->b_locked > 0))
	ফেরত TRUE;

    যদি (quit_all || (check_more(FALSE, forceit) == OK && only_one_window()))
    {
	apply_autocmds(EVENT_EXITPRE, ফাঁকা, ফাঁকা, FALSE, curbuf);
	// Refuse to quit when locked or when the window was closed or the
	// buffer in the last window is being closed (can only happen in
	// autocommands).
	যদি (!win_valid(wp) || curbuf_locked()
			  || (curbuf->b_nwindows == 1 && curbuf->b_locked > 0))
	    ফেরত TRUE;
    }

    ফেরত FALSE;
}

/*
 * ":quit": quit current window, quit Vim if the last window is closed.
 * ":{nr}quit": quit window {nr}
 * Also used when closing a terminal window that's the last one.
 */
    শূন্য
ex_quit(exarg_T *eap)
{
    win_T	*wp;

    যদি (cmdwin_type != 0)
    {
	cmdwin_ফলাফল = Ctrl_C;
	ফেরত;
    }
    // Don't quit while editing the command line.
    যদি (text_locked())
    {
	text_locked_বার্তা();
	ফেরত;
    }
    যদি (eap->addr_গুনতি > 0)
    {
	পূর্ণসংখ্যা	wnr = eap->line2;

	জন্য (wp = firstwin; wp->w_পরবর্তী != ফাঁকা; wp = wp->w_পরবর্তী)
	    যদি (--wnr <= 0)
		বিরতি;
    }
    অন্যথা
	wp = curwin;

    // Refuse to quit when locked.
    যদি (curbuf_locked())
	ফেরত;

    // Trigger QuitPre and maybe ExitPre
    যদি (before_quit_autocmds(wp, FALSE, eap->forceit))
	ফেরত;

#ifdef FEAT_NETBEANS_INTG
    netbeansForcedQuit = eap->forceit;
#endif

    পূর্ণসংখ্যা save_exiting = exiting;
    /*
     * If there is only one relevant window we will exit.
     */
    যদি (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;
    যদি ((!buf_hide(wp->w_বাফার)
		&& check_changed(wp->w_বাফার, (p_awa ? CCGD_AW : 0)
				       | (eap->forceit ? CCGD_FORCEIT : 0)
				       | CCGD_EXCMD))
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, TRUE)))
    {
	not_exiting(save_exiting);
    }
    অন্যথা
    {
	// quit last window
	// Note: only_one_window() returns true, even so a help window is
	// still open. In that case only quit, if no address has been
	// specified. Example:
	// :h|wincmd w|1q     - don't quit
	// :h|wincmd w|q      - quit
	যদি (only_one_window() && (ONE_WINDOW || eap->addr_গুনতি == 0))
	    getout(0);
	not_exiting(save_exiting);
#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	// close window; may free buffer
	win_close(wp, !buf_hide(wp->w_বাফার) || eap->forceit);
    }
}

/*
 * ":cquit".
 */
    স্থির শূন্য
ex_cquit(exarg_T *eap UNUSED)
{
    // this does not always pass on the exit code to the Manx compiler. why?
    getout(eap->addr_গুনতি > 0 ? (পূর্ণসংখ্যা)eap->line2 : EXIT_FAILURE);
}

/*
 * Do preparations for "qall" and "wqall".
 * Returns FAIL when quitting should be aborted.
 */
    পূর্ণসংখ্যা
before_quit_all(exarg_T *eap)
{
    যদি (cmdwin_type != 0)
    {
	যদি (eap->forceit)
	    cmdwin_ফলাফল = K_XF1;	// ex_window() takes care of this
	অন্যথা
	    cmdwin_ফলাফল = K_XF2;
	ফেরত FAIL;
    }

    // Don't quit while editing the command line.
    যদি (text_locked())
    {
	text_locked_বার্তা();
	ফেরত FAIL;
    }

    যদি (before_quit_autocmds(curwin, TRUE, eap->forceit))
	ফেরত FAIL;

    ফেরত OK;
}

/*
 * ":qall": try to quit all windows
 */
    স্থির শূন্য
ex_quit_all(exarg_T *eap)
{
    যদি (before_quit_all(eap) == FAIL)
	ফেরত;
    পূর্ণসংখ্যা save_exiting = exiting;
    exiting = TRUE;
    যদি (eap->forceit || !check_changed_any(FALSE, FALSE))
	getout(0);
    not_exiting(save_exiting);
}

/*
 * ":close": close current window, unless it is the last one
 */
    স্থির শূন্য
ex_close(exarg_T *eap)
{
    win_T	*win;
    পূর্ণসংখ্যা		winnr = 0;
    যদি (cmdwin_type != 0)
	cmdwin_ফলাফল = Ctrl_C;
    অন্যথা
	যদি (!text_locked() && !curbuf_locked())
	{
	    যদি (eap->addr_গুনতি == 0)
		ex_win_close(eap->forceit, curwin, ফাঁকা);
	    অন্যথা
	    {
		FOR_ALL_WINDOWS(win)
		{
		    winnr++;
		    যদি (winnr == eap->line2)
			বিরতি;
		}
		যদি (win == ফাঁকা)
		    win = lastwin;
		ex_win_close(eap->forceit, win, ফাঁকা);
	    }
	}
}

#ifdef FEAT_QUICKFIX
/*
 * ":pclose": Close any preview window.
 */
    স্থির শূন্য
ex_pclose(exarg_T *eap)
{
    win_T	*win;

    // First close any normal window.
    FOR_ALL_WINDOWS(win)
	যদি (win->w_p_pvw)
	{
	    ex_win_close(eap->forceit, win, ফাঁকা);
	    ফেরত;
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
    স্থির শূন্য
ex_win_close(
    পূর্ণসংখ্যা		forceit,
    win_T	*win,
    tabpage_T	*tp)		// NULL or the tab page "win" is in
{
    পূর্ণসংখ্যা		need_hide;
    buf_T	*buf = win->w_বাফার;

    // Never close the autocommand window.
    যদি (is_aucmd_win(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	ফেরত;
    }
    যদি (window_layout_locked(CMD_close))
	ফেরত;

    need_hide = (bufIsChanged(buf) && buf->b_nwindows <= 1);
    যদি (need_hide && !buf_hide(buf) && !forceit)
    {
#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	যদি ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    যদি (term_job_running(buf->b_term))
	    {
		যদি (term_confirm_stop(buf) == FAIL)
		    ফেরত;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(buf);
		need_hide = FALSE;
	    }
	    অন্যথা
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(buf, FALSE);
		যদি (bufref_valid(&bufref) && bufIsChanged(buf))
		    ফেরত;
		need_hide = FALSE;
	    }
	}
	অন্যথা
#endif
	{
	    no_write_message();
	    ফেরত;
	}
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // free buffer when not hiding it or when it's a scratch buffer
    যদি (tp == ফাঁকা)
	win_close(win, !need_hide && !buf_hide(buf));
    অন্যথা
	win_close_othertab(win, !need_hide && !buf_hide(buf), tp);
}

/*
 * Handle the argument for a tabpage related ex command.
 * Returns a tabpage number.
 * When an error is encountered then eap->errmsg is set.
 */
    স্থির পূর্ণসংখ্যা
get_tabpage_arg(exarg_T *eap)
{
    পূর্ণসংখ্যা tab_সংখ্যা;
    পূর্ণসংখ্যা unaccept_arg0 = (eap->cmdidx == CMD_tabmove) ? 0 : 1;

    যদি (eap->arg && *eap->arg != NUL)
    {
	char_u *p = eap->arg;
	char_u *p_save;
	পূর্ণসংখ্যা    relative = 0; // argument +N/-N means: go to N places to the
			     // right/left relative to the current position.

	যদি (*p == '-')
	{
	    relative = -1;
	    p++;
	}
	অন্যথা যদি (*p == '+')
	{
	    relative = 1;
	    p++;
	}

	p_save = p;
	tab_সংখ্যা = getdigits(&p);

	যদি (relative == 0)
	{
	    যদি (STRCMP(p, "$") == 0)
		tab_সংখ্যা = LAST_TAB_NR;
	    অন্যথা যদি (STRCMP(p, "#") == 0)
		যদি (valid_tabpage(lastused_tabpage))
		    tab_সংখ্যা = tabpage_সূচক(lastused_tabpage);
		অন্যথা
		{
		    eap->errmsg = ex_errmsg(e_invalid_মান_for_argument_str,
								     eap->arg);
		    tab_সংখ্যা = 0;
		    যাও theend;
		}
	    অন্যথা যদি (p == p_save || *p_save == '-' || *p != NUL
		    || tab_সংখ্যা > LAST_TAB_NR)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		যাও theend;
	    }
	}
	অন্যথা
	{
	    যদি (*p_save == NUL)
		tab_সংখ্যা = 1;
	    অন্যথা যদি (p == p_save || *p_save == '-' || *p != NUL
		    || tab_সংখ্যা == 0)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		যাও theend;
	    }
	    tab_সংখ্যা = tab_সংখ্যা * relative + tabpage_সূচক(curtab);
	    যদি (!unaccept_arg0 && relative == -1)
		--tab_সংখ্যা;
	}
	যদি (tab_সংখ্যা < unaccept_arg0 || tab_সংখ্যা > LAST_TAB_NR)
	    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
    }
    অন্যথা যদি (eap->addr_গুনতি > 0)
    {
	যদি (unaccept_arg0 && eap->line2 == 0)
	{
	    eap->errmsg = _(e_invalid_range);
	    tab_সংখ্যা = 0;
	}
	অন্যথা
	{
	    tab_সংখ্যা = eap->line2;
	    যদি (!unaccept_arg0)
	    {
		char_u *cmdp = eap->cmd;

		যখন (--cmdp > *eap->cmdlinep
			&& (VIM_ISWHITE(*cmdp) || VIM_ISDIGIT(*cmdp)))
		    ;
		যদি (*cmdp == '-')
		{
		    --tab_সংখ্যা;
		    যদি (tab_সংখ্যা < unaccept_arg0)
			eap->errmsg = _(e_invalid_range);
		}
	    }
	}
    }
    অন্যথা
    {
	নির্বাচন (eap->cmdidx)
	{
	ক্ষেত্র CMD_tabnext:
	    tab_সংখ্যা = tabpage_সূচক(curtab) + 1;
	    যদি (tab_সংখ্যা > LAST_TAB_NR)
		tab_সংখ্যা = 1;
	    বিরতি;
	ক্ষেত্র CMD_tabmove:
	    tab_সংখ্যা = LAST_TAB_NR;
	    বিরতি;
	পূর্বনির্ধারিত:
	    tab_সংখ্যা = tabpage_সূচক(curtab);
	}
    }

theend:
    ফেরত tab_সংখ্যা;
}

/*
 * ":tabclose": close current tab page, unless it is the last one.
 * ":tabclose N": close tab page N.
 */
    স্থির শূন্য
ex_tabclose(exarg_T *eap)
{
    tabpage_T	*tp;
    পূর্ণসংখ্যা		tab_সংখ্যা;

    যদি (cmdwin_type != 0)
    {
	cmdwin_ফলাফল = K_IGNORE;
	ফেরত;
    }

    যদি (first_tabpage->tp_পরবর্তী == ফাঁকা)
    {
	emsg(_(e_cannot_close_last_tab_page));
	ফেরত;
    }

    যদি (window_layout_locked(CMD_tabclose))
	ফেরত;

    tab_সংখ্যা = get_tabpage_arg(eap);
    যদি (eap->errmsg != ফাঁকা)
	ফেরত;

    tp = find_tabpage(tab_সংখ্যা);
    যদি (tp == ফাঁকা)
    {
	beep_flush();
	ফেরত;
    }
    যদি (tp != curtab)
    {
	tabpage_close_other(tp, eap->forceit);
	ফেরত;
    }
    অন্যথা যদি (!text_locked() && !curbuf_locked())
	tabpage_close(eap->forceit);
}

/*
 * ":tabonly": close all tab pages except the current one
 */
    স্থির শূন্য
ex_tabonly(exarg_T *eap)
{
    tabpage_T	*tp;
    পূর্ণসংখ্যা		done;
    পূর্ণসংখ্যা		tab_সংখ্যা;

    যদি (cmdwin_type != 0)
    {
	cmdwin_ফলাফল = K_IGNORE;
	ফেরত;
    }

    যদি (first_tabpage->tp_পরবর্তী == ফাঁকা)
    {
	বার্তা(_("Already only one tab page"));
	ফেরত;
    }

    যদি (window_layout_locked(CMD_tabonly))
	ফেরত;

    tab_সংখ্যা = get_tabpage_arg(eap);
    যদি (eap->errmsg != ফাঁকা)
	ফেরত;

    goto_tabpage(tab_সংখ্যা);
    // Repeat this up to a 1000 times, because autocommands may
    // mess up the lists.
    জন্য (done = 0; done < 1000; ++done)
    {
	FOR_ALL_TABPAGES(tp)
	    যদি (tp->tp_topframe != topframe)
	    {
		tabpage_close_other(tp, eap->forceit);
		// if we failed to close it quit
		যদি (valid_tabpage(tp))
		    done = 1000;
		// start over, "tp" is now invalid
		বিরতি;
	    }
	যদি (first_tabpage->tp_পরবর্তী == ফাঁকা)
	    বিরতি;
    }
}

/*
 * Close the current tab page.
 */
    শূন্য
tabpage_close(পূর্ণসংখ্যা forceit)
{
    যদি (window_layout_locked(CMD_tabclose))
	ফেরত;

    trigger_tabclosedpre(curtab);
    curtab->tp_did_tabclosedpre = TRUE;
    tabpage_T *save_curtab = curtab;

    // First close all the windows but the current one.  If that worked then
    // close the last window in this tab, that will close it.
    যদি (!ONE_WINDOW)
	close_others(TRUE, forceit);
    যদি (ONE_WINDOW)
	ex_win_close(forceit, curwin, ফাঁকা);
    যদি (curtab == save_curtab)
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
    শূন্য
tabpage_close_other(tabpage_T *tp, পূর্ণসংখ্যা forceit)
{
    পূর্ণসংখ্যা		done = 0;
    win_T	*wp;

    যদি (window_layout_locked(CMD_আকার))
	ফেরত;

    trigger_tabclosedpre(tp);
    tp->tp_did_tabclosedpre = TRUE;

    // Limit to 1000 windows, autocommands may add a window while we close
    // one.  OK, so I'm paranoid...
    যখন (++done < 1000)
    {
	wp = tp->tp_firstwin;
	ex_win_close(forceit, wp, tp);

	// Autocommands may delete the tab page under our fingers.
	যদি (!valid_tabpage(tp))
	    বিরতি;
	// We may fail to close a window with a modified buffer.
	যদি (tp->tp_firstwin == wp)
	{
	    done = 1000;
	    বিরতি;
	}
    }
    যদি (done >= 1000)
    {
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	tp->tp_did_tabclosedpre = FALSE;
	ফেরত;
    }

    apply_autocmds(EVENT_TABCLOSED, ফাঁকা, ফাঁকা, FALSE, curbuf);
}

/*
 * ":only".
 */
    স্থির শূন্য
ex_only(exarg_T *eap)
{
    যদি (window_layout_locked(CMD_only))
	ফেরত;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    যদি (eap->addr_গুনতি > 0)
    {
	win_T   *wp;
	পূর্ণসংখ্যা	wnr = eap->line2;
	জন্য (wp = firstwin; --wnr > 0; )
	{
	    যদি (wp->w_পরবর্তী == ফাঁকা)
		বিরতি;
	    অন্যথা
		wp = wp->w_পরবর্তী;
	}
	win_goto(wp);
    }
    close_others(TRUE, eap->forceit);
}

    স্থির শূন্য
ex_hide(exarg_T *eap UNUSED)
{
    // ":hide" or ":hide | cmd": hide current window
    যদি (eap->skip)
	ফেরত;

    যদি (window_layout_locked(CMD_hide))
	ফেরত;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    যদি (eap->addr_গুনতি == 0)
	win_close(curwin, FALSE);	// don't free buffer
    অন্যথা
    {
	পূর্ণসংখ্যা	winnr = 0;
	win_T	*win;

	FOR_ALL_WINDOWS(win)
	{
	    winnr++;
	    যদি (winnr == eap->line2)
		বিরতি;
	}
	যদি (win == ফাঁকা)
	    win = lastwin;
	win_close(win, FALSE);
    }
}

/*
 * ":stop" and ":suspend": Suspend Vim.
 */
    শূন্য
ex_stop(exarg_T *eap)
{
    /*
     * Disallow suspending for "rvim".
     */
    যদি (check_restricted())
	ফেরত;

    যদি (!eap->forceit)
	autowrite_all();
    apply_autocmds(EVENT_VIMSUSPEND, ফাঁকা, ফাঁকা, FALSE, ফাঁকা);
    windgoto((পূর্ণসংখ্যা)Rows - 1, cmdline_col_off);
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
    apply_autocmds(EVENT_VIMRESUME, ফাঁকা, ফাঁকা, FALSE, ফাঁকা);
}

/*
 * ":exit", ":xit" and ":wq": Write file and quit the current window.
 */
    স্থির শূন্য
ex_exit(exarg_T *eap)
{
#ifdef FEAT_EVAL
    যদি (not_in_vim9(eap, false) == FAIL)
	ফেরত;
#endif
    যদি (cmdwin_type != 0)
    {
	cmdwin_ফলাফল = Ctrl_C;
	ফেরত;
    }
    // Don't quit while editing the command line.
    যদি (text_locked())
    {
	text_locked_বার্তা();
	ফেরত;
    }

    পূর্ণসংখ্যা save_exiting = exiting;
    /*
     * we plan to exit if there is only one relevant window
     */
    যদি (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;

    // Write the buffer for ":wq" or when it was changed.
    // Trigger QuitPre and ExitPre.
    // Check if we can exit now, after autocommands have changed things.
    যদি (((eap->cmdidx == CMD_wq || curbufIsChanged()) && do_write(eap) == FAIL)
	    || before_quit_autocmds(curwin, FALSE, eap->forceit)
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, FALSE)))
    {
	not_exiting(save_exiting);
    }
    অন্যথা
    {
	যদি (only_one_window())	    // quit last window, exit Vim
	    getout(0);
	not_exiting(save_exiting);
#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	// Quit current window, may free the buffer.
	win_close(curwin, !buf_hide(curwin->w_বাফার));
    }
}

/*
 * ":print", ":list", ":number".
 */
    স্থির শূন্য
ex_print(exarg_T *eap)
{
    যদি (curbuf->b_ml.ml_flags & ML_EMPTY)
	emsg(_(e_empty_বাফার));
    অন্যথা
    {
	জন্য ( ;!got_int; ui_breakcheck())
	{
	    print_সারি(eap->line1,
		    (eap->cmdidx == CMD_সংখ্যা || eap->cmdidx == CMD_pound
						 || (eap->flags & EXFLAG_NR)),
		    eap->cmdidx == CMD_list || (eap->flags & EXFLAG_LIST));
	    যদি (++eap->line1 > eap->line2)
		বিরতি;
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
    স্থির শূন্য
ex_goto(exarg_T *eap)
{
    goto_byte(eap->line2);
}
#endif

/*
 * ":shell".
 */
    স্থির শূন্য
ex_shell(exarg_T *eap UNUSED)
{
    do_shell(ফাঁকা, 0);
}

#যদি defined(HAVE_DROP_FILE)

স্থির পূর্ণসংখ্যা drop_busy = FALSE;
স্থির পূর্ণসংখ্যা drop_filec;
স্থির char_u **drop_filev = ফাঁকা;
স্থির পূর্ণসংখ্যা drop_split;
স্থির শূন্য (*drop_callback)(শূন্য *);
স্থির শূন্য *drop_cookie;

    স্থির শূন্য
handle_drop_internal(শূন্য)
{
    exarg_T	ea;
    পূর্ণসংখ্যা		save_বার্তা_scroll = বার্তা_scroll;

    // Setting the argument list may cause screen updates and being called
    // recursively.  Avoid that by setting drop_busy.
    drop_busy = TRUE;

    // Check whether the current buffer is changed. If so, we will need
    // to split the current window or data could be lost.
    // We don't need to check if the 'hidden' option is set, as in this
    // case the buffer won't be lost.
    যদি (!buf_hide(curbuf) && !drop_split)
    {
	++emsg_off;
	drop_split = check_changed(curbuf, CCGD_AW);
	--emsg_off;
    }
    যদি (drop_split)
    {
	যদি (win_split(0, 0) == FAIL)
	    ফেরত;
	RESET_BINDING(curwin);

	// When splitting the window, create a new alist.  Otherwise the
	// existing one is overwritten.
	alist_unlink(curwin->w_alist);
	alist_new();
    }

    /*
     * Set up the new argument list.
     */
    alist_set(ALIST(curwin), drop_filec, drop_filev, FALSE, ফাঁকা, 0);

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
    বার্তা_scroll = save_বার্তা_scroll;

    যদি (drop_callback != ফাঁকা)
	drop_callback(drop_cookie);

    drop_filev = ফাঁকা;
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
    শূন্য
handle_drop(
    পূর্ণসংখ্যা		filec,		// the number of files dropped
    char_u	**filev,	// the list of files dropped
    পূর্ণসংখ্যা		split,		// force splitting the window
    শূন্য	(*callback)(শূন্য *), // to be called after setting the argument
				     // list
    শূন্য	*cookie)	// argument for "callback" (allocated)
{
    // Cannot handle recursive drops, finish the pending one.
    যদি (drop_busy)
    {
	FreeWild(filec, filev);
	vim_free(cookie);
	ফেরত;
    }

    // When calling handle_drop() more than once in a row we only use the last
    // one.
    যদি (drop_filev != ফাঁকা)
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
    যদি (text_locked() || curbuf_locked() || updating_screen)
	ফেরত;

    handle_drop_internal();
}

/*
 * To be called when text is unlocked, curbuf is unlocked or updating_screen is
 * reset: Handle a postponed drop.
 */
    শূন্য
handle_any_postponed_drop(শূন্য)
{
    যদি (!drop_busy && drop_filev != ফাঁকা
	     && !text_locked() && !curbuf_locked() && !updating_screen)
	handle_drop_internal();
}
#endif

/*
 * ":preserve".
 */
    স্থির শূন্য
ex_preserve(exarg_T *eap UNUSED)
{
    curbuf->b_flags |= BF_PRESERVED;
    ml_preserve(curbuf, TRUE);
}

/*
 * ":recover".
 */
    স্থির শূন্য
ex_recover(exarg_T *eap)
{
    // Set recoverymode right away to avoid the ATTENTION prompt.
    recoverymode = TRUE;
    যদি (!check_changed(curbuf, (p_awa ? CCGD_AW : 0)
			     | CCGD_MULTWIN
			     | (eap->forceit ? CCGD_FORCEIT : 0)
			     | CCGD_EXCMD)

	    && (*eap->arg == NUL
			     || setfname(curbuf, eap->arg, ফাঁকা, TRUE) == OK))
	ml_recover(TRUE);
    recoverymode = FALSE;
}

/*
 * Command modifier used in a wrong way.  Also for other commands that can't
 * appear at the toplevel.
 */
    স্থির শূন্য
ex_wrongmodifier(exarg_T *eap)
{
    eap->errmsg = ex_errmsg(e_invalid_command_str, eap->cmd);
}

#যদি defined(FEAT_EVAL)

// callback function for 'findfunc'
স্থির callback_T ffu_cb;

    স্থির callback_T *
get_findfunc_callback(শূন্য)
{
    ফেরত *curbuf->b_p_ffu != NUL ? &curbuf->b_ffu_cb : &ffu_cb;
}

/*
 * Call 'findfunc' to obtain a list of file names.
 */
    স্থির list_T *
call_findfunc(char_u *pat, পূর্ণসংখ্যা cmdcomplete)
{
    typval_T	args[3];
    callback_T	*cb;
    typval_T	rettv;
    পূর্ণসংখ্যা		retval;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_BOOL;
    args[1].vval.v_সংখ্যা = cmdcomplete;
    args[2].v_type = VAR_UNKNOWN;

    // Lock the text to prevent weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;

    ctx = get_option_sctx("findfunc");
    যদি (ctx != ফাঁকা)
	current_sctx = *ctx;

    cb = get_findfunc_callback();
    retval = call_callback(cb, -1, &rettv, 2, args);

    current_sctx = saved_sctx;

    --textlock;

    list_T *retlist = ফাঁকা;

    যদি (retval == OK)
    {
	যদি (rettv.v_type == VAR_LIST)
	    retlist = list_copy(rettv.vval.v_list, FALSE, FALSE, get_copyID());
	অন্যথা
	    emsg(_(e_invalid_return_type_from_findfunc));

	clear_tv(&rettv);
    }

    ফেরত retlist;
}

/*
 * Find file names matching "pat" using 'findfunc' and return it in "files".
 * Used for expanding the :find, :sfind and :tabfind command argument.
 * Returns OK on success and FAIL otherwise.
 */
    পূর্ণসংখ্যা
expand_findfunc(expand_T *xp, char_u *pat, char_u ***files, পূর্ণসংখ্যা *অঙ্ক_Matches)
{
    list_T	*l;
    পূর্ণসংখ্যা		len;

    *অঙ্ক_Matches = 0;
    *files = ফাঁকা;

    l = call_findfunc(pat, VVAL_TRUE);

    যদি (l == ফাঁকা)
	ফেরত FAIL;

    len = list_len(l);
    যদি (len == 0)	    // empty List
    {
	list_free(l);
	ফেরত FAIL;
    }

    expand_process_user_list(l, files, অঙ্ক_Matches, xp);
    list_free(l);

    ফেরত OK;
}

/*
 * Use 'findfunc' to find file 'findarg'.  The 'count' argument is used to find
 * the n'th matching file.
 */
    স্থির char_u *
findfunc_find_file(char_u *findarg, পূর্ণসংখ্যা findarg_len, পূর্ণসংখ্যা গুনতি)
{
    list_T	*fname_list;
    char_u	*ret_fname = ফাঁকা;
    char_u	cc;
    পূর্ণসংখ্যা		fname_গুনতি;

    cc = findarg[findarg_len];
    findarg[findarg_len] = NUL;

    fname_list = call_findfunc(findarg, VVAL_FALSE);
    fname_গুনতি = list_len(fname_list);

    যদি (fname_গুনতি == 0)
	semsg(_(e_cant_find_file_str_in_path), findarg);
    অন্যথা
    {
	যদি (গুনতি > fname_গুনতি)
	    semsg(_(e_no_more_file_str_found_in_path), findarg);
	অন্যথা
	{
	    listitem_T *li = list_find(fname_list, গুনতি - 1);

	    যদি (li != ফাঁকা)
	    {
		typval_T *tv = &li->li_tv;

		যদি (tv->v_type == VAR_STRING && tv->vval.v_string != ফাঁকা)
		    ret_fname = vim_strsave(tv->vval.v_string);
		অন্যথা যদি (tv->v_type == VAR_DICT && tv->vval.v_dict != ফাঁকা)
		    ret_fname = dict_get_string(tv->vval.v_dict, "word", TRUE);
	    }
	}
    }

    যদি (fname_list != ফাঁকা)
	list_free(fname_list);

    findarg[findarg_len] = cc;

    ফেরত ret_fname;
}

/*
 * Process the 'findfunc' option value.
 * Returns NULL on success and an error message on failure.
 */
    অক্ষর *
did_set_findfunc(optset_T *args UNUSED)
{
    পূর্ণসংখ্যা	retval;

    যদি (args->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_ffu, &curbuf->b_ffu_cb);
    অন্যথা
    {
	// global option set
	retval = option_set_callback_func(p_ffu, &ffu_cb);
	// when using :set, free the local callback
	যদি (!(args->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_ffu_cb);
    }

    যদি (retval == FAIL)
	ফেরত e_invalid_argument;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*name = get_scriptlocal_funcname(*varp);
    যদি (name != ফাঁকা)
    {
	free_string_option(*varp);
	*varp = name;
    }

    ফেরত ফাঁকা;
}

# যদি defined(EXITFREE)
    শূন্য
free_findfunc_option(শূন্য)
{
    free_callback(&ffu_cb);
}
# endif

/*
 * Mark the global 'findfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    পূর্ণসংখ্যা
set_ref_in_findfunc(পূর্ণসংখ্যা copyID UNUSED)
{
    পূর্ণসংখ্যা abort = FALSE;
    abort = set_ref_in_callback(&ffu_cb, copyID);
    ফেরত abort;
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
    শূন্য
ex_splitview(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    char_u	*fname = ফাঁকা;
#ifdef FEAT_BROWSE
    char_u	dot_path[] = ".";
    পূর্ণসংখ্যা		save_cmod_flags = cmdmod.cmod_flags;
#endif
    পূর্ণসংখ্যা		use_tab = eap->cmdidx == CMD_tabedit
		       || eap->cmdidx == CMD_tabfind
		       || eap->cmdidx == CMD_tabnew;

    যদি (ERROR_IF_ANY_POPUP_WINDOW)
	ফেরত;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#ifdef FEAT_QUICKFIX
    // A ":split" in the quickfix window works like ":new".  Don't want two
    // quickfix windows.  But it's OK when doing ":tab split".
    যদি (bt_quickfix(curbuf) && cmdmod.cmod_tab == 0)
    {
	যদি (eap->cmdidx == CMD_split)
	    eap->cmdidx = CMD_new;
	যদি (eap->cmdidx == CMD_vsplit)
	    eap->cmdidx = CMD_vnew;
    }
#endif

    যদি (eap->cmdidx == CMD_sfind || eap->cmdidx == CMD_tabfind)
    {
	char_u	*file_to_find = ফাঁকা;
	অক্ষর	*অনুসন্ধান_ctx = ফাঁকা;

	যদি (*get_findfunc() != NUL)
	{
#ifdef FEAT_EVAL
	    fname = findfunc_find_file(eap->arg, (পূর্ণসংখ্যা)STRLEN(eap->arg),
				       eap->addr_গুনতি > 0 ? eap->line2 : 1);
#endif
	}
	অন্যথা
	{
	    fname = find_file_in_path(eap->arg, (পূর্ণসংখ্যা)STRLEN(eap->arg),
					      FNAME_MESS, TRUE, curbuf->b_ffname,
					      &file_to_find, &অনুসন্ধান_ctx);
	    vim_free(file_to_find);
	    vim_findfile_cleanup(অনুসন্ধান_ctx);
	}
	যদি (fname == ফাঁকা)
	    যাও theend;
	eap->arg = fname;
    }
#ifdef FEAT_BROWSE
    অন্যথা যদি ((cmdmod.cmod_flags & CMOD_BROWSE)
	    && eap->cmdidx != CMD_vnew
	    && eap->cmdidx != CMD_new)
    {
	যদি (
# ifdef FEAT_GUI
	    !gui.in_use &&
# endif
		au_has_group((char_u *)"FileExplorer"))
	{
	    // No browsing supported but we do have the file explorer:
	    // Edit the directory.
	    যদি (*eap->arg == NUL || !mch_isdir(eap->arg))
		eap->arg = dot_path;
	}
	অন্যথা
	{
	    fname = do_browse(0, (char_u *)(use_tab
			? _("Edit File in new tab page")
			: _("Edit File in new window")),
					  eap->arg, ফাঁকা, ফাঁকা, ফাঁকা, curbuf);
	    যদি (fname == ফাঁকা)
		যাও theend;
	    eap->arg = fname;
	}
    }
    cmdmod.cmod_flags &= ~CMOD_BROWSE;	// Don't browse again in do_ecmd().
#endif

    /*
     * Either open new tab page or split the window.
     */
    যদি (use_tab)
    {
	যদি (win_new_tabpage(cmdmod.cmod_tab != 0 ? cmdmod.cmod_tab
			 : eap->addr_গুনতি == 0 ? 0
					       : (পূর্ণসংখ্যা)eap->line2 + 1) != FAIL)
	{
	    do_exedit(eap, old_curwin);

	    // set the alternate buffer for the window we came from
	    যদি (curwin != old_curwin
		    && win_valid(old_curwin)
		    && old_curwin->w_বাফার != curbuf
		    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		old_curwin->w_alt_fnum = curbuf->b_fnum;
	}
    }
    অন্যথা যদি (win_split(eap->addr_গুনতি > 0 ? (পূর্ণসংখ্যা)eap->line2 : 0,
				     *eap->cmd == 'v' ? WSP_VERT : 0) != FAIL)
    {
	// Reset 'scrollbind' when editing another file, but keep it when
	// doing ":split" without arguments.
	যদি (*eap->arg != NUL)
	    RESET_BINDING(curwin);
	অন্যথা
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
    শূন্য
tabpage_new(শূন্য)
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
    স্থির শূন্য
ex_tabnext(exarg_T *eap)
{
    পূর্ণসংখ্যা tab_সংখ্যা;

    যদি (ERROR_IF_POPUP_WINDOW)
	ফেরত;
    নির্বাচন (eap->cmdidx)
    {
	ক্ষেত্র CMD_tabfirst:
	ক্ষেত্র CMD_tabrewind:
	    goto_tabpage(1);
	    বিরতি;
	ক্ষেত্র CMD_tablast:
	    goto_tabpage(9999);
	    বিরতি;
	ক্ষেত্র CMD_tabprevious:
	ক্ষেত্র CMD_tab_পরবর্তী:
	    যদি (eap->arg && *eap->arg != NUL)
	    {
		char_u *p = eap->arg;
		char_u *p_save = p;

		tab_সংখ্যা = getdigits(&p);
		যদি (p == p_save || *p_save == '-' || *p != NUL
			    || tab_সংখ্যা == 0)
		{
		    // No numbers as argument.
		    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		    ফেরত;
		}
	    }
	    অন্যথা
	    {
		যদি (eap->addr_গুনতি == 0)
		    tab_সংখ্যা = 1;
		অন্যথা
		{
		    tab_সংখ্যা = eap->line2;
		    যদি (tab_সংখ্যা < 1)
		    {
			eap->errmsg = _(e_invalid_range);
			ফেরত;
		    }
		}
	    }
	    goto_tabpage(-tab_সংখ্যা);
	    বিরতি;
	পূর্বনির্ধারিত: // CMD_tabnext
	    tab_সংখ্যা = get_tabpage_arg(eap);
	    যদি (eap->errmsg == ফাঁকা)
		goto_tabpage(tab_সংখ্যা);
	    বিরতি;
    }
}

/*
 * :tabmove command
 */
    স্থির শূন্য
ex_tabmove(exarg_T *eap)
{
    পূর্ণসংখ্যা tab_সংখ্যা;

    tab_সংখ্যা = get_tabpage_arg(eap);
    যদি (eap->errmsg == ফাঁকা)
	tabpage_move(tab_সংখ্যা);
}

/*
 * :tabs command: List tabs and their contents.
 */
    স্থির শূন্য
ex_tabs(exarg_T *eap UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp;
    পূর্ণসংখ্যা		tabcount = 1;

    বার্তা_start();
    বার্তা_scroll = TRUE;
    জন্য (tp = first_tabpage; tp != ফাঁকা && !got_int; tp = tp->tp_পরবর্তী)
    {
	বার্তা_putchar('\n');
	vim_snprintf((অক্ষর *)IObuff, IOSIZE, _("Tab page %d"), tabcount++);
	বার্তা_outtrans_attr(IObuff, HL_ATTR(HLF_T));
	out_flush();	    // output one line at a time
	ui_breakcheck();

	যদি (tp  == curtab)
	    wp = firstwin;
	অন্যথা
	    wp = tp->tp_firstwin;
	জন্য ( ; wp != ফাঁকা && !got_int; wp = wp->w_পরবর্তী)
	{
	    বার্তা_putchar('\n');
	    বার্তা_putchar(wp == curwin ? '>' : ' ');
	    বার্তা_putchar(' ');
	    বার্তা_putchar(bufIsChanged(wp->w_বাফার) ? '+' : ' ');
	    বার্তা_putchar(' ');
	    যদি (buf_spname(wp->w_বাফার) != ফাঁকা)
		vim_strncpy(IObuff, buf_spname(wp->w_বাফার), IOSIZE - 1);
	    অন্যথা
		home_replace(wp->w_বাফার, wp->w_বাফার->b_fname,
							IObuff, IOSIZE, TRUE);
	    বার্তা_outtrans(IObuff);
	    out_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }
}

/*
 * ":mode": Set screen mode.
 * If no argument given, just get the screen size and redraw.
 */
    স্থির শূন্য
ex_mode(exarg_T *eap)
{
    যদি (*eap->arg == NUL)
	shell_resized();
    অন্যথা
	emsg(_(e_screen_mode_setting_not_supported));
}

/*
 * ":resize".
 * set, increment or decrement current window height
 */
    স্থির শূন্য
ex_resize(exarg_T *eap)
{
    পূর্ণসংখ্যা		n;
    win_T	*wp = curwin;

    যদি (eap->addr_গুনতি > 0)
    {
	n = eap->line2;
	জন্য (wp = firstwin; wp->w_পরবর্তী != ফাঁকা && --n > 0; wp = wp->w_পরবর্তী)
	    ;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    n = atol((অক্ষর *)eap->arg);
    যদি (cmdmod.cmod_split & WSP_VERT)
    {
	যদি (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_width;
	অন্যথা যদি (n == 0 && eap->arg[0] == NUL)	// default is very wide
	    n = 9999;
	win_setwidth_win(n, wp);
    }
    অন্যথা
    {
	যদি (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_height;
	অন্যথা যদি (n == 0 && eap->arg[0] == NUL)	// default is very high
	    n = 9999;
	win_setheight_win(n, wp);
    }
}

/*
 * ":find [+command] <file>" command.
 */
    স্থির শূন্য
ex_find(exarg_T *eap)
{
    যদি (!check_can_set_curbuf_forceit(eap->forceit))
	ফেরত;

    char_u	*fname = ফাঁকা;
    পূর্ণসংখ্যা		গুনতি;
    char_u	*file_to_find = ফাঁকা;
    অক্ষর	*অনুসন্ধান_ctx = ফাঁকা;

    যদি (*get_findfunc() != NUL)
    {
#ifdef FEAT_EVAL
	fname = findfunc_find_file(eap->arg, (পূর্ণসংখ্যা)STRLEN(eap->arg),
					eap->addr_গুনতি > 0 ? eap->line2 : 1);
#endif
    }
    অন্যথা
    {
	fname = find_file_in_path(eap->arg, (পূর্ণসংখ্যা)STRLEN(eap->arg), FNAME_MESS,
			       TRUE, curbuf->b_ffname, &file_to_find, &অনুসন্ধান_ctx);
	যদি (eap->addr_গুনতি > 0)
	{
	    // Repeat finding the file "count" times.  This matters when it appears
	    // several times in the path.
	    গুনতি = eap->line2;
	    যখন (fname != ফাঁকা && --গুনতি > 0)
	    {
		vim_free(fname);
		fname = find_file_in_path(ফাঁকা, 0, FNAME_MESS,
			      FALSE, curbuf->b_ffname, &file_to_find, &অনুসন্ধান_ctx);
	    }
	}
	VIM_CLEAR(file_to_find);
	vim_findfile_cleanup(অনুসন্ধান_ctx);
    }

    যদি (fname == ফাঁকা)
	ফেরত;

    eap->arg = fname;
    do_exedit(eap, ফাঁকা);
    vim_free(fname);
}

/*
 * ":open" simulation: for now works just like ":visual".
 */
    স্থির শূন্য
ex_open(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*p;

#ifdef FEAT_EVAL
    যদি (not_in_vim9(eap, false) == FAIL)
	ফেরত;
#endif
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_SOL | BL_FIX);
    যদি (*eap->arg == '/')
    {
	// ":open /pattern/": put cursor in column found with pattern
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg, magic_isset() ? RE_MAGIC : 0);
	যদি (regmatch.regprog != ফাঁকা)
	{
	    // make a copy of the line, when searching for a mark it might be
	    // flushed
	    char_u *সারি = vim_strsave(ml_get_curline());

	    regmatch.rm_ic = p_ic;
	    যদি (vim_regexec(&regmatch, সারি, (colnr_T)0))
		curwin->w_cursor.col = (colnr_T)(regmatch.startp[0] - সারি);
	    অন্যথা
		emsg(_(e_no_match));
	    vim_regfree(regmatch.regprog);
	    vim_free(সারি);
	}
	// Move to the NUL, ignore any other arguments.
	eap->arg += STRLEN(eap->arg);
    }
    check_cursor();

    eap->cmdidx = CMD_visual;
    do_exedit(eap, ফাঁকা);
}

/*
 * ":edit", ":badd", ":balt", ":visual".
 */
    স্থির শূন্য
ex_edit(exarg_T *eap)
{
    char_u *ffname = eap->cmdidx == CMD_enew ? ফাঁকা : eap->arg;

    // Exclude commands which keep the window's current buffer
    যদি (
	    eap->cmdidx != CMD_badd
	    && eap->cmdidx != CMD_balt
	    // All other commands must obey 'winfixbuf' / ! rules
	    && (is_other_file(0, ffname) && !check_can_set_curbuf_forceit(eap->forceit))
    )
	ফেরত;

    do_exedit(eap, ফাঁকা);
}

/*
 * ":edit <file>" command and alike.
 */
    শূন্য
do_exedit(
    exarg_T	*eap,
    win_T	*old_curwin)	    // curwin before doing a split or NULL
{
    পূর্ণসংখ্যা		n;
    পূর্ণসংখ্যা		need_hide;
    পূর্ণসংখ্যা		exmode_was = exmode_active;

    যদি ((eap->cmdidx != CMD_pedit && ERROR_IF_POPUP_WINDOW)
						 || ERROR_IF_TERM_POPUP_WINDOW)
	ফেরত;
    /*
     * ":vi" command ends Ex mode.
     */
    যদি (exmode_active && (eap->cmdidx == CMD_visual
						|| eap->cmdidx == CMD_view))
    {
	exmode_active = FALSE;
	ex_pressedreturn = FALSE;
	যদি (*eap->arg == NUL)
	{
	    // Special case:  ":global/pat/visual\NLvi-commands"
	    যদি (global_busy)
	    {
		যদি (eap->nextcmd != ফাঁকা)
		{
		    stuffReadbuff(eap->nextcmd);
		    eap->nextcmd = ফাঁকা;
		}

		যদি (exmode_was != EXMODE_VIM)
		    settmode(TMODE_RAW);
		পূর্ণসংখ্যা save_RedrawingDisabled = RedrawingDisabled;
		RedrawingDisabled = 0;
		পূর্ণসংখ্যা save_nwr = no_wait_return;
		no_wait_return = 0;
		need_wait_return = FALSE;
		পূর্ণসংখ্যা save_ms = বার্তা_scroll;
		বার্তা_scroll = 0;
#ifdef FEAT_GUI
		পূর্ণসংখ্যা save_he = hold_gui_events;
		hold_gui_events = 0;
#endif
		set_must_redraw(UPD_CLEAR);
		pending_exmode_active = TRUE;

		main_loop(FALSE, TRUE);

		pending_exmode_active = FALSE;
		RedrawingDisabled = save_RedrawingDisabled;
		no_wait_return = save_nwr;
		বার্তা_scroll = save_ms;
#ifdef FEAT_GUI
		hold_gui_events = save_he;
#endif
	    }
	    ফেরত;
	}
    }

    যদি ((eap->cmdidx == CMD_new
		|| eap->cmdidx == CMD_tabnew
		|| eap->cmdidx == CMD_tabedit
		|| eap->cmdidx == CMD_vnew) && *eap->arg == NUL)
    {
	// ":new" or ":tabnew" without argument: edit a new empty buffer
	setpcmark();
	(শূন্য)do_ecmd(0, ফাঁকা, ফাঁকা, eap, ECMD_ONE,
		      ECMD_HIDE + (eap->forceit ? ECMD_FORCEIT : 0),
		      old_curwin == ফাঁকা ? curwin : ফাঁকা);
    }
    অন্যথা যদি ((eap->cmdidx != CMD_split && eap->cmdidx != CMD_vsplit)
	    || *eap->arg != NUL
#ifdef FEAT_BROWSE
	    || (cmdmod.cmod_flags & CMOD_BROWSE)
#endif
	    )
    {
	// Can't edit another file when "textlock" or "curbuf_lock" is set.
	// Only ":edit" or ":script" can bring us here, others are stopped
	// earlier.
	যদি (*eap->arg != NUL && text_or_buf_locked())
	    ফেরত;

	n = readonlymode;
	যদি (eap->cmdidx == CMD_view || eap->cmdidx == CMD_sview)
	    readonlymode = TRUE;
	অন্যথা যদি (eap->cmdidx == CMD_enew)
	    readonlymode = FALSE;   // 'readonly' doesn't make sense in an
				    // empty buffer
	যদি (eap->cmdidx != CMD_balt && eap->cmdidx != CMD_badd)
	    setpcmark();
	যদি (do_ecmd(0, (eap->cmdidx == CMD_enew ? ফাঁকা : eap->arg),
		    ফাঁকা, eap,
		    // ":edit" goes to first line if Vi compatible
		    (*eap->arg == NUL && eap->do_ecmd_lnum == 0
				      && vim_strchr(p_cpo, CPO_GOTO1) != ফাঁকা)
					       ? ECMD_ONE : eap->do_ecmd_lnum,
		    (buf_hide(curbuf) ? ECMD_HIDE : 0)
		    + (eap->forceit ? ECMD_FORCEIT : 0)
		      // after a split we can use an existing buffer
		    + (old_curwin != ফাঁকা ? ECMD_OLDBUF : 0)
		    + (eap->cmdidx == CMD_badd ? ECMD_ADDBUF : 0)
		    + (eap->cmdidx == CMD_balt ? ECMD_ALTBUF : 0)
		    , old_curwin == ফাঁকা ? curwin : ফাঁকা) == FAIL)
	{
	    // Editing the file failed.  If the window was split, close it.
	    যদি (old_curwin != ফাঁকা)
	    {
		need_hide = (curbufIsChanged() && curbuf->b_nwindows <= 1);
		যদি (!need_hide || buf_hide(curbuf))
		{
#যদি defined(FEAT_EVAL)
		    cleanup_T   cs;

		    // Reset the error/interrupt/exception state here so that
		    // aborting() returns FALSE when closing a window.
		    enter_cleanup(&cs);
#endif
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_close(curwin, !need_hide && !buf_hide(curbuf));

#যদি defined(FEAT_EVAL)
		    // Restore the error/interrupt/exception state if not
		    // discarded by a new aborting error, interrupt, or
		    // uncaught exception.
		    leave_cleanup(&cs);
#endif
		}
	    }
	}
	অন্যথা যদি (readonlymode && curbuf->b_nwindows == 1)
	{
	    // When editing an already visited buffer, 'readonly' won't be set
	    // but the previous value is kept.  With ":view" and ":sview" we
	    // want the  file to be readonly, except when another window is
	    // editing the same buffer.
	    curbuf->b_p_ro = TRUE;
	}
	readonlymode = n;
    }
    অন্যথা
    {
	যদি (eap->do_ecmd_cmd != ফাঁকা)
	    do_cmd_argument(eap->do_ecmd_cmd);
	n = curwin->w_arg_idx_invalid;
	check_arg_idx(curwin);
	যদি (n != curwin->w_arg_idx_invalid)
	    maketitle();
    }

    /*
     * if ":split file" worked, set alternate file name in old window to new
     * file
     */
    যদি (old_curwin != ফাঁকা
	    && *eap->arg != NUL
	    && curwin != old_curwin
	    && win_valid(old_curwin)
	    && old_curwin->w_বাফার != curbuf
	    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	old_curwin->w_alt_fnum = curbuf->b_fnum;

    ex_no_reprint = TRUE;
}

#ifndef FEAT_GUI
/*
 * ":gui" and ":gvim" when there is no GUI.
 */
    স্থির শূন্য
ex_nogui(exarg_T *eap)
{
    eap->errmsg = _(e_gui_cannot_be_used_not_enabled_at_compile_time);
}
#endif

#যদি defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
    স্থির শূন্য
ex_tearoff(exarg_T *eap)
{
    gui_make_tearoff(eap->arg);
}
#endif

#যদি (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
    স্থির শূন্য
ex_popup(exarg_T *eap)
{
# যদি defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    যদি (gui.in_use)
	gui_make_popup(eap->arg, eap->forceit);
#  ifdef FEAT_TERM_POPUP_MENU
    অন্যথা
#  endif
# endif
# ifdef FEAT_TERM_POPUP_MENU
	pum_make_popup(eap->arg, eap->forceit);
# endif
}
#endif

    স্থির শূন্য
ex_swapname(exarg_T *eap UNUSED)
{
    যদি (curbuf->b_ml.ml_mfp == ফাঁকা || curbuf->b_ml.ml_mfp->mf_fname == ফাঁকা)
	বার্তা(_("No swap file"));
    অন্যথা
	বার্তা((অক্ষর *)curbuf->b_ml.ml_mfp->mf_fname);
}

/*
 * ":syncbind" forces all 'scrollbind' windows to have the same relative
 * offset.
 * (1998-11-02 16:21:01  R. Edward Ralston <eralston@computer.org>)
 */
    স্থির শূন্য
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
    যদি (curwin->w_p_scb)
    {
	topline = curwin->w_topline;
	FOR_ALL_WINDOWS(wp)
	{
	    যদি (wp->w_p_scb && wp->w_বাফার)
	    {
		y = wp->w_বাফার->b_ml.ml_সারি_গুনতি - get_scrolloff_মান();
		যদি (topline > y)
		    topline = y;
	    }
	}
	যদি (topline < 1)
	    topline = 1;
    }
    অন্যথা
    {
	topline = 1;
    }


    /*
     * Set all scrollbind windows to the same topline.
     */
    FOR_ALL_WINDOWS(curwin)
    {
	যদি (curwin->w_p_scb)
	{
	    curbuf = curwin->w_বাফার;
	    y = topline - curwin->w_topline;
	    যদি (y > 0)
		scrollup(y, TRUE);
	    অন্যথা
		scrolldown(-y, TRUE);
	    curwin->w_scbind_pos = topline;
	    redraw_later(UPD_VALID);
	    cursor_correct();
	    curwin->w_redr_status = true;
	}
    }
    curwin = save_curwin;
    curbuf = save_curbuf;
    যদি (curwin->w_p_scb)
    {
	did_syncbind = TRUE;
	checkpcmark();
	যদি (old_linenr != curwin->w_cursor.lnum)
	{
	    char_u ctrl_o[2];

	    ctrl_o[0] = Ctrl_O;
	    ctrl_o[1] = 0;
	    ins_typebuf(ctrl_o, REMAP_NONE, 0, TRUE, FALSE);
	}
    }
}


    স্থির শূন্য
ex_read(exarg_T *eap)
{
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum;

    যদি (eap->usefilter)			// :r!cmd
    {
	do_bang(1, eap, FALSE, FALSE, TRUE);
	ফেরত;
    }

    যদি (u_save(eap->line2, (linenr_T)(eap->line2 + 1)) == FAIL)
	ফেরত;

#ifdef FEAT_BROWSE
    যদি (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browseFile;

	browseFile = do_browse(0, (char_u *)_("Append File"), eap->arg,
		ফাঁকা, ফাঁকা, ফাঁকা, curbuf);
	যদি (browseFile != ফাঁকা)
	{
	    i = readfile(browseFile, ফাঁকা,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	    vim_free(browseFile);
	}
	অন্যথা
	    i = OK;
    }
    অন্যথা
#endif
	যদি (*eap->arg == NUL)
	{
	    যদি (check_fname() == FAIL)	// check for no file name
		ফেরত;
	    i = readfile(curbuf->b_ffname, curbuf->b_fname,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	}
	অন্যথা
	{
	    যদি (vim_strchr(p_cpo, CPO_ALTREAD) != ফাঁকা)
		(শূন্য)setaltfname(eap->arg, eap->arg, (linenr_T)1);
	    i = readfile(eap->arg, ফাঁকা,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);

	}
    যদি (i != OK)
    {
#যদি defined(FEAT_EVAL)
	যদি (!aborting())
#endif
	    semsg(_(e_cant_open_file_str), eap->arg);
    }
    অন্যথা
    {
	যদি (empty && exmode_active)
	{
	    // Delete the empty line that remains.  Historically ex does
	    // this but vi doesn't.
	    যদি (eap->line2 == 0)
		lnum = curbuf->b_ml.ml_সারি_গুনতি;
	    অন্যথা
		lnum = 1;
	    যদি (*ml_get(lnum) == NUL && u_savedel(lnum, 1L) == OK)
	    {
		ml_delete(lnum);
		যদি (curwin->w_cursor.lnum > 1
			&& curwin->w_cursor.lnum >= lnum)
		    --curwin->w_cursor.lnum;
		deleted_সারিগুলি_mark(lnum, 1L);
	    }
	}
	redraw_curbuf_later(UPD_VALID);
    }
}

স্থির char_u	*পূর্ব_dir = ফাঁকা;

#যদি defined(EXITFREE)
    শূন্য
free_cd_dir(শূন্য)
{
    VIM_CLEAR(পূর্ব_dir);
    VIM_CLEAR(globaldir);
}
#endif

/*
 * Get the previous directory for the given chdir scope.
 */
    স্থির char_u *
get_prevdir(cdscope_T scope)
{
    যদি (scope == CDSCOPE_WINDOW)
	ফেরত curwin->w_prevdir;
    অন্যথা যদি (scope == CDSCOPE_TABPAGE)
	ফেরত curtab->tp_prevdir;
    ফেরত পূর্ব_dir;
}

/*
 * Deal with the side effects of changing the current directory.
 * When 'scope' is CDSCOPE_TABPAGE then this was after an ":tcd" command.
 * When 'scope' is CDSCOPE_WINDOW then this was after an ":lcd" command.
 */
    শূন্য
post_chdir(cdscope_T scope)
{
    যদি (scope != CDSCOPE_WINDOW)
	// Clear tab local directory for both :cd and :tcd
	VIM_CLEAR(curtab->tp_localdir);
    VIM_CLEAR(curwin->w_localdir);
    যদি (scope != CDSCOPE_GLOBAL)
    {
	char_u	*pdir = get_prevdir(scope);

	// If still in the global directory, need to remember current
	// directory as the global directory.
	যদি (globaldir == ফাঁকা && pdir != ফাঁকা)
	    globaldir = vim_strsave(pdir);

	// Remember this local directory for the window.
	যদি (mch_dirname(NameBuff, MAXPATHL) == OK)
	{
	    যদি (scope == CDSCOPE_TABPAGE)
		curtab->tp_localdir = vim_strsave(NameBuff);
	    অন্যথা
		curwin->w_localdir = vim_strsave(NameBuff);
	}
    }
    অন্যথা
    {
	// We are now in the global directory, no need to remember its name.
	VIM_CLEAR(globaldir);
    }

    last_chdir_reason = ফাঁকা;
    shorten_fnames(vim_strchr(p_cpo, CPO_NOSYMLINKS) == ফাঁকা);
}

/*
 * Trigger DirChangedPre for "acmd_fname" with directory "new_dir".
 */
    শূন্য
trigger_DirChangedPre(char_u *acmd_fname, char_u *new_dir)
{
#ifdef FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;

    v_event = get_v_event(&save_v_event);
    (শূন্য)dict_add_string(v_event, "directory", new_dir);
    dict_set_বস্তুসমূহ_ro(v_event);
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
    পূর্ণসংখ্যা
changedir_func(
	char_u		*new_dir,
	পূর্ণসংখ্যা		forceit,
	cdscope_T	scope)
{
    char_u	*pdir = ফাঁকা;
    পূর্ণসংখ্যা		dir_differs;
    char_u	*acmd_fname = ফাঁকা;
    char_u	**pp;
    char_u	*tofree;

    যদি (new_dir == ফাঁকা || allbuf_locked())
	ফেরত FALSE;

    যদি (vim_strchr(p_cpo, CPO_CHDIR) != ফাঁকা && curbufIsChanged() && !forceit)
    {
	emsg(_(e_cannot_change_directory_বাফার_is_modified_add_bang_to_override));
	ফেরত FALSE;
    }

    // ":cd -": Change to previous directory
    যদি (STRCMP(new_dir, "-") == 0)
    {
	pdir = get_prevdir(scope);
	যদি (pdir == ফাঁকা)
	{
	    emsg(_(e_no_previous_directory));
	    ফেরত FALSE;
	}
	new_dir = pdir;
    }

    // Save current directory for next ":cd -"
    যদি (mch_dirname(NameBuff, MAXPATHL) == OK)
	pdir = vim_strsave(NameBuff);
    অন্যথা
	pdir = ফাঁকা;

    // For UNIX ":cd" means: go to home directory.
    // On other systems too if 'cdhome' is set.
#যদি defined(UNIX) || defined(VMS)
    যদি (*new_dir == NUL)
#অন্যথা
    যদি (*new_dir == NUL && p_cdh)
#endif
    {
	// use NameBuff for home directory name
#ifdef VMS
	char_u	*p;

	p = mch_getenv((char_u *)"SYS$LOGIN");
	যদি (p == ফাঁকা || *p == NUL)	// empty is the same as not set
	    NameBuff[0] = NUL;
	অন্যথা
	    vim_strncpy(NameBuff, p, MAXPATHL - 1);
#অন্যথা
	expand_env((char_u *)"$HOME", NameBuff, MAXPATHL);
#endif
	new_dir = NameBuff;
    }
    dir_differs = pdir == ফাঁকা
			    || pathcmp((অক্ষর *)pdir, (অক্ষর *)new_dir, -1) != 0;
    যদি (dir_differs)
    {
	যদি (scope == CDSCOPE_WINDOW)
	    acmd_fname = (char_u *)"window";
	অন্যথা যদি (scope == CDSCOPE_TABPAGE)
	    acmd_fname = (char_u *)"tabpage";
	অন্যথা
	    acmd_fname = (char_u *)"global";
	trigger_DirChangedPre(acmd_fname, new_dir);

	যদি (vim_chdir(new_dir))
	{
	    emsg(_(e_command_failed));
	    vim_free(pdir);
	    ফেরত FALSE;
	}
    }

    যদি (scope == CDSCOPE_WINDOW)
	pp = &curwin->w_prevdir;
    অন্যথা যদি (scope == CDSCOPE_TABPAGE)
	pp = &curtab->tp_prevdir;
    অন্যথা
	pp = &পূর্ব_dir;
    tofree = *pp;  // new_dir may use this
    *pp = pdir;

    post_chdir(scope);

    যদি (dir_differs)
	apply_autocmds(EVENT_DIRCHANGED, acmd_fname, new_dir, FALSE, curbuf);
    vim_free(tofree);
    ফেরত TRUE;
}

/*
 * ":cd", ":tcd", ":lcd", ":chdir" ":tchdir" and ":lchdir".
 */
    শূন্য
ex_cd(exarg_T *eap)
{
    char_u	*new_dir;

    new_dir = eap->arg;
#যদি !defined(UNIX) && !defined(VMS)
    // for non-UNIX ":cd" means: print current directory unless 'cdhome' is set
    যদি (*new_dir == NUL && !p_cdh)
    {
	ex_pwd(ফাঁকা);
	ফেরত;
    }
#endif

    cdscope_T	scope = CDSCOPE_GLOBAL;

    যদি (eap->cmdidx == CMD_lcd || eap->cmdidx == CMD_lchdir)
	scope = CDSCOPE_WINDOW;
    অন্যথা যদি (eap->cmdidx == CMD_tcd || eap->cmdidx == CMD_tchdir)
	scope = CDSCOPE_TABPAGE;

    যদি (changedir_func(new_dir, eap->forceit, scope))
    {
	// Echo the new current directory if the command was typed.
	যদি (চাবি_Typed || p_verbose >= 5)
	    ex_pwd(eap);
    }
}

/*
 * ":pwd".
 */
    স্থির শূন্য
ex_pwd(exarg_T *eap UNUSED)
{
    যদি (mch_dirname(NameBuff, MAXPATHL) == OK)
    {
#ifdef BACKSLASH_IN_FILENAME
	slash_adjust(NameBuff);
#endif
	যদি (p_verbose > 0)
	{
	    অক্ষর *context = "global";

	    যদি (last_chdir_reason != ফাঁকা)
		context = last_chdir_reason;
	    অন্যথা যদি (curwin->w_localdir != ফাঁকা)
		context = "window";
	    অন্যথা যদি (curtab->tp_localdir != ফাঁকা)
		context = "tabpage";
	    smsg("[%s] %s", context, (অক্ষর *)NameBuff);
	}
	অন্যথা
	    বার্তা((অক্ষর *)NameBuff);
    }
    অন্যথা
	emsg(_(e_directory_unknown));
}

/*
 * ":=".
 */
    স্থির শূন্য
ex_equal(exarg_T *eap)
{
    smsg("%ld", (long)eap->line2);
    ex_may_print(eap);
}

    স্থির শূন্য
ex_sleep(exarg_T *eap)
{
    পূর্ণসংখ্যা		n;
    long	len;

    যদি (cursor_valid())
    {
	n = W_WINROW(curwin) + curwin->w_wrow - বার্তা_scrolled;
	যদি (n >= 0)
	    windgoto(n, curwin->w_wincol + curwin->w_wcol);
    }

    len = eap->line2;
    নির্বাচন (*eap->arg)
    {
	ক্ষেত্র 'm': বিরতি;
	ক্ষেত্র NUL: len *= 1000L; বিরতি;
	পূর্বনির্ধারিত: semsg(_(e_invalid_argument_str), eap->arg); ফেরত;
    }

    // Hide the cursor if invoked with !
    do_sleep(len, eap->forceit);
}

/*
 * Sleep for "msec" milliseconds, but keep checking for a CTRL-C every second.
 * Hide the cursor if "hide_cursor" is TRUE.
 */
    শূন্য
do_sleep(long msec, পূর্ণসংখ্যা hide_cursor)
{
    long	done = 0;
    long	wait_now;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    // Remember at what time we started, so that we know how much longer we
    // should wait after waiting for a bit.
    ELAPSED_INIT(start_tv);
#endif

    যদি (hide_cursor)
	cursor_sleep();
    অন্যথা
	cursor_on();

    out_flush_cursor(FALSE, FALSE);
    যখন (!got_int && done < msec)
    {
	wait_now = msec - done > 1000L ? 1000L : msec - done;
#ifdef FEAT_TIMERS
	{
	    long    due_time = check_due_timer();

	    যদি (due_time > 0 && due_time < wait_now)
		wait_now = due_time;
	}
#endif
#ifdef FEAT_JOB_CHANNEL
	যদি (has_any_channel() && wait_now > 20L)
	    wait_now = 20L;
#endif
#ifdef FEAT_SOUND
	যদি (has_any_sound_callback() && wait_now > 20L)
	    wait_now = 20L;
#endif
	ui_delay(wait_now, TRUE);

#ifdef FEAT_JOB_CHANNEL
	যদি (has_any_channel())
	    ui_breakcheck_force(TRUE);
	অন্যথা
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
#অন্যথা
	// guesstimate time passed (will actually be more)
	done += wait_now;
#endif
    }

    // If CTRL-C was typed to interrupt the sleep, drop the CTRL-C from the
    // input buffer, otherwise a following call to input() fails.
    যদি (got_int)
	(শূন্য)vpeekc();

    যদি (hide_cursor)
	cursor_unsleep();
}

/*
 * ":winsize" command (obsolete).
 */
    স্থির শূন্য
ex_winsize(exarg_T *eap)
{
    পূর্ণসংখ্যা		w, h;
    char_u	*arg = eap->arg;
    char_u	*p;

    যদি (!SAFE_isdigit(*arg))
    {
	semsg(_(e_invalid_argument_str), arg);
	ফেরত;
    }
    w = getdigits(&arg);
    arg = skipwhite(arg);
    p = arg;
    h = getdigits(&arg);
    যদি (*p != NUL && *arg == NUL)
	set_shellsize(w, h, TRUE);
    অন্যথা
	emsg(_(e_winsize_requires_two_সংখ্যা_arguments));
}

    স্থির শূন্য
ex_wincmd(exarg_T *eap)
{
    পূর্ণসংখ্যা		xchar = NUL;
    char_u	*p;

    যদি (*eap->arg == 'g' || *eap->arg == Ctrl_G)
    {
	// CTRL-W g and CTRL-W CTRL-G  have an extra command character
	যদি (eap->arg[1] == NUL)
	{
	    emsg(_(e_invalid_argument));
	    ফেরত;
	}
	xchar = eap->arg[1];
	p = eap->arg + 2;
    }
    অন্যথা
	p = eap->arg + 1;

    set_nextcmd(eap, p);
    p = skipwhite(p);
    যদি (*p != NUL && *p != (
#ifdef FEAT_EVAL
	    in_vim9script() ? '#' :
#endif
		'"')
	    && eap->nextcmd == ফাঁকা)
	emsg(_(e_invalid_argument));
    অন্যথা যদি (!eap->skip)
    {
	// Pass flags on for ":vertical wincmd ]".
	postponed_split_flags = cmdmod.cmod_split;
	postponed_split_tab = cmdmod.cmod_tab;
	do_window(*eap->arg, eap->addr_গুনতি > 0 ? eap->line2 : 0L, xchar);
	postponed_split_flags = 0;
	postponed_split_tab = 0;
    }
}

#যদি defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * ":winpos".
 */
    স্থির শূন্য
ex_winpos(exarg_T *eap)
{
    পূর্ণসংখ্যা		x, y;
    char_u	*arg = eap->arg;
    char_u	*p;

    যদি (*arg == NUL)
    {
# যদি defined(FEAT_GUI) || defined(MSWIN)
#  ifdef VIMDLL
	যদি (gui.in_use ? gui_mch_get_winpos(&x, &y) != FAIL :
		mch_get_winpos(&x, &y) != FAIL)
#  elif defined(FEAT_GUI)
	যদি (gui.in_use && gui_mch_get_winpos(&x, &y) != FAIL)
#  অন্যথা
	যদি (mch_get_winpos(&x, &y) != FAIL)
#  endif
	{
	    sprintf((অক্ষর *)IObuff, _("Window position: X %d, Y %d"), x, y);
	    বার্তা((অক্ষর *)IObuff);
	}
	অন্যথা
# endif
	    emsg(_(e_obtaining_window_position_not_implemented_for_this_platform));
    }
    অন্যথা
    {
	x = getdigits(&arg);
	arg = skipwhite(arg);
	p = arg;
	y = getdigits(&arg);
	যদি (*p == NUL || *arg != NUL)
	{
	    emsg(_(e_winpos_requires_two_সংখ্যা_arguments));
	    ফেরত;
	}
# ifdef FEAT_GUI
	যদি (gui.in_use)
	    gui_mch_set_winpos(x, y);
	অন্যথা যদি (gui.starting)
	{
	    // Remember the coordinates for when the window is opened.
	    gui_win_x = x;
	    gui_win_y = y;
	}
#  যদি defined(HAVE_TGETENT) || defined(VIMDLL)
	অন্যথা
#  endif
# endif
# যদি defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	    mch_set_winpos(x, y);
# endif
# ifdef HAVE_TGETENT
	যদি (*T_CWP)
	    term_set_winpos(x, y);
# endif
    }
}
#endif

/*
 * Handle command that work like operators: ":delete", ":yank", ":>" and ":<".
 */
    স্থির শূন্য
ex_operators(exarg_T *eap)
{
    oparg_T	oa;

    clear_oparg(&oa);
    oa.regname = eap->regname;
    oa.start.lnum = eap->line1;
    oa.end.lnum = eap->line2;
    oa.সারি_গুনতি = eap->line2 - eap->line1 + 1;
    oa.motion_type = MLINE;
    virtual_op = FALSE;
    যদি (eap->cmdidx != CMD_yank)	// position cursor for undo
    {
	setpcmark();
	curwin->w_cursor.lnum = eap->line1;
	beginline(BL_SOL | BL_FIX);
    }

    যদি (VIsual_active)
	end_visual_mode();

    নির্বাচন (eap->cmdidx)
    {
	ক্ষেত্র CMD_delete:
	    oa.op_type = OP_DELETE;
	    op_delete(&oa);
	    বিরতি;

	ক্ষেত্র CMD_yank:
	    oa.op_type = OP_YANK;
	    (শূন্য)op_yank(&oa, FALSE, TRUE);
	    বিরতি;

	পূর্বনির্ধারিত:    // CMD_rshift or CMD_lshift
	    যদি (
#ifdef FEAT_RIGHTLEFT
		(eap->cmdidx == CMD_rshift) ^ curwin->w_p_rl
#অন্যথা
		eap->cmdidx == CMD_rshift
#endif
						)
		oa.op_type = OP_RSHIFT;
	    অন্যথা
		oa.op_type = OP_LSHIFT;
	    op_shift(&oa, FALSE, eap->amount);
	    বিরতি;
    }
    virtual_op = MAYBE;
    ex_may_print(eap);
}

/*
 * ":put".
 */
    স্থির শূন্য
ex_put(exarg_T *eap)
{
    // ":0put" works like ":1put!".
    যদি (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, ফাঁকা, eap->forceit ? BACKWARD : FORWARD, 1L,
						       PUT_সারি|PUT_CURSLINE);
}

/*
 * ":iput".
 */
    স্থির শূন্য
ex_iput(exarg_T *eap)
{
    // ":0iput" works like ":1iput!".
    যদি (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, ফাঁকা, eap->forceit ? BACKWARD : FORWARD, 1L,
						      PUT_সারি|PUT_CURSLINE
						      |PUT_FIXINDENT);
}

/*
 * Handle ":copy" and ":move".
 */
    স্থির শূন্য
ex_copymove(exarg_T *eap)
{
    long	n;

#ifdef FEAT_EVAL
    যদি (not_in_vim9(eap, false) == FAIL)
	ফেরত;
#endif
    n = get_address(eap, &eap->arg, eap->addr_type, FALSE, FALSE, FALSE, 1);
    যদি (eap->arg == ফাঁকা)	    // error detected
    {
	eap->nextcmd = ফাঁকা;
	ফেরত;
    }
    get_flags(eap);

    /*
     * move or copy lines from 'eap->line1'-'eap->line2' to below line 'n'
     */
    যদি (n == MAXLNUM || n < 0 || n > curbuf->b_ml.ml_সারি_গুনতি)
    {
	emsg(_(e_invalid_range));
	ফেরত;
    }

    যদি (eap->cmdidx == CMD_move)
    {
	যদি (do_move(eap->line1, eap->line2, n) == FAIL)
	    ফেরত;
    }
    অন্যথা
	ex_copy(eap->line1, eap->line2, n);
    u_clearline();
    beginline(BL_SOL | BL_FIX);
    ex_may_print(eap);
}

/*
 * Print the current line if flags were given to the Ex command.
 */
    শূন্য
ex_may_print(exarg_T *eap)
{
    যদি (eap->flags != 0)
    {
	print_সারি(curwin->w_cursor.lnum, (eap->flags & EXFLAG_NR),
						  (eap->flags & EXFLAG_LIST));
	ex_no_reprint = TRUE;
    }
}

/*
 * ":smagic" and ":snomagic".
 */
    স্থির শূন্য
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
    স্থির শূন্য
ex_join(exarg_T *eap)
{
    curwin->w_cursor.lnum = eap->line1;
    যদি (eap->line1 == eap->line2)
    {
	যদি (eap->addr_গুনতি >= 2)   // :2,2join does nothing
	    ফেরত;
	যদি (eap->line2 == curbuf->b_ml.ml_সারি_গুনতি)
	{
	    beep_flush();
	    ফেরত;
	}
	++eap->line2;
    }
    (শূন্য)do_join(eap->line2 - eap->line1 + 1, !eap->forceit, TRUE, TRUE, TRUE);
    beginline(BL_WHITE | BL_FIX);
    ex_may_print(eap);
}

/*
 * ":[addr]@r" or ":[addr]*r": execute register
 */
    স্থির শূন্য
ex_at(exarg_T *eap)
{
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		পূর্ব_len = typebuf.tb_len;

    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif

    // get the register name.  No name means to use the previous one
    c = *eap->arg;
    যদি (c == NUL || (c == '*' && *eap->cmd == '*'))
	c = '@';
    // Put the register in the typeahead buffer with the "silent" flag.
    যদি (do_execreg(c, TRUE, vim_strchr(p_cpo, CPO_EXECBUF) != ফাঁকা, TRUE)
								      == FAIL)
    {
	beep_flush();
	ফেরত;
    }

    পূর্ণসংখ্যা	save_efr = exec_from_reg;

    exec_from_reg = TRUE;

    /*
     * Execute from the typeahead buffer.
     * Continue until the stuff buffer is empty and all added characters
     * have been consumed.
     */
    যখন (!stuff_empty() || typebuf.tb_len > পূর্ব_len)
	(শূন্য)do_cmdline(ফাঁকা, getexline, ফাঁকা, DOCMD_NOWAIT|DOCMD_VERBOSE);

    exec_from_reg = save_efr;
}

/*
 * ":!".
 */
    স্থির শূন্য
ex_bang(exarg_T *eap)
{
    do_bang(eap->addr_গুনতি, eap, eap->forceit, TRUE, TRUE);
}

/*
 * ":undo".
 */
    স্থির শূন্য
ex_undo(exarg_T *eap)
{
    যদি (eap->addr_গুনতি == 1)	    // :undo 123
	undo_time(eap->line2, FALSE, FALSE, TRUE);
    অন্যথা
	u_undo(1);
}

#ifdef FEAT_PERSISTENT_UNDO
    স্থির শূন্য
ex_wundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_আকার];

    u_compute_hash(hash);
    u_write_undo(eap->arg, eap->forceit, curbuf, hash);
}

    স্থির শূন্য
ex_rundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_আকার];

    u_compute_hash(hash);
    u_read_undo(eap->arg, hash, ফাঁকা);
}
#endif

/*
 * ":redo".
 */
    স্থির শূন্য
ex_redo(exarg_T *eap UNUSED)
{
    u_redo(1);
}

/*
 * ":earlier" and ":later".
 */
    স্থির শূন্য
ex_later(exarg_T *eap)
{
    long	গুনতি = 0;
    পূর্ণসংখ্যা		sec = FALSE;
    পূর্ণসংখ্যা		file = FALSE;
    char_u	*p = eap->arg;

    যদি (*p == NUL)
	গুনতি = 1;
    অন্যথা যদি (SAFE_isdigit(*p))
    {
	গুনতি = getdigits(&p);
	নির্বাচন (*p)
	{
	    ক্ষেত্র 's': ++p; sec = TRUE; বিরতি;
	    ক্ষেত্র 'm': ++p; sec = TRUE; গুনতি *= 60; বিরতি;
	    ক্ষেত্র 'h': ++p; sec = TRUE; গুনতি *= 60 * 60; বিরতি;
	    ক্ষেত্র 'd': ++p; sec = TRUE; গুনতি *= 24 * 60 * 60; বিরতি;
	    ক্ষেত্র 'f': ++p; file = TRUE; বিরতি;
	}
    }

    যদি (*p != NUL)
	semsg(_(e_invalid_argument_str), eap->arg);
    অন্যথা
	undo_time(eap->cmdidx == CMD_earlier ? -গুনতি : গুনতি,
							    sec, file, FALSE);
}

/*
 * ":redir": start/stop redirection.
 */
    স্থির শূন্য
ex_redir(exarg_T *eap)
{
    অক্ষর	*mode;
    char_u	*fname;
    char_u	*arg = eap->arg;

#ifdef FEAT_EVAL
    যদি (redir_execute)
    {
	emsg(_(e_cannot_use_redir_inside_execute));
	ফেরত;
    }
#endif

    যদি (STRICMP(eap->arg, "END") == 0)
	close_redir();
    অন্যথা
    {
	যদি (*arg == '>')
	{
	    ++arg;
	    যদি (*arg == '>')
	    {
		++arg;
		mode = "a";
	    }
	    অন্যথা
		mode = "w";
	    arg = skipwhite(arg);

	    close_redir();

	    // Expand environment variables and "~/".
	    fname = expand_env_save(arg);
	    যদি (fname == ফাঁকা)
		ফেরত;
#ifdef FEAT_BROWSE
	    যদি (cmdmod.cmod_flags & CMOD_BROWSE)
	    {
		char_u	*browseFile;

		browseFile = do_browse(BROWSE_SAVE,
			(char_u *)_("Save Redirection"),
			fname, ফাঁকা, ফাঁকা,
			(char_u *)_(BROWSE_FILTER_ALL_FILES), curbuf);
		যদি (browseFile == ফাঁকা)
		    ফেরত;		// operation cancelled
		vim_free(fname);
		fname = browseFile;
		eap->forceit = TRUE;	// since dialog already asked
	    }
#endif

	    redir_fd = open_exfile(fname, eap->forceit, mode);
	    vim_free(fname);
	}
#ifdef FEAT_EVAL
	অন্যথা যদি (*arg == '@')
	{
	    // redirect to a register a-z (resp. A-Z for appending)
	    close_redir();
	    ++arg;
	    যদি (ASCII_ISALPHA(*arg)
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
		যদি (*arg == '>' && arg[1] == '>')  // append
		    arg += 2;
		অন্যথা
		{
		    // Can use both "@a" and "@a>".
		    যদি (*arg == '>')
			arg++;
		    // Make register empty when not using @A-@Z and the
		    // command is valid.
		    যদি (*arg == NUL && !SAFE_isupper(redir_reg))
			write_reg_contents(redir_reg, (char_u *)"", -1, FALSE);
		}
	    }
	    যদি (*arg != NUL)
	    {
		redir_reg = 0;
		semsg(_(e_invalid_argument_str), eap->arg);
	    }
	}
	অন্যথা যদি (*arg == '=' && arg[1] == '>')
	{
	    পূর্ণসংখ্যা append;

	    // redirect to a variable
	    close_redir();
	    arg += 2;

	    যদি (*arg == '>')
	    {
		++arg;
		append = TRUE;
	    }
	    অন্যথা
		append = FALSE;

	    যদি (var_redir_start(skipwhite(arg), append) == OK)
		redir_vname = 1;
	}
#endif

	// TODO: redirect to a buffer

	অন্যথা
	    semsg(_(e_invalid_argument_str), eap->arg);
    }

    // Make sure redirection is not off.  Can happen for cmdline completion
    // that indirectly invokes a command to catch its output.
    যদি (redir_fd != ফাঁকা
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname
#endif
							)
	redir_off = FALSE;
}

/*
 * ":redraw": force redraw, with clear for ":redraw!".
 */
    শূন্য
ex_redraw(exarg_T *eap)
{
    redraw_cmd(eap->forceit);
}

/*
 * ":redraw": force redraw, with clear if "clear" is TRUE.
 */
    শূন্য
redraw_cmd(পূর্ণসংখ্যা clear)
{
    পূর্ণসংখ্যা save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    পূর্ণসংখ্যা save_p_lz = p_lz;
    p_lz = FALSE;

    validate_cursor();
    update_topline();
    update_screen(clear ? UPD_CLEAR : VIsual_active ? UPD_INVERTED : 0);
    যদি ((State & MODE_CMDLINE) == 0)
	setcursor(); // put cursor back where it belongs
    যদি (need_maketitle)
	maketitle();
#যদি defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
    যদি (!gui.in_use)
# endif
	resize_console_buf();
#endif
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;

    // After drawing the statusline screen_attr may still be set.
    screen_stop_highlight();

    // Reset msg_didout, so that a message that's there is overwritten.
    বার্তা_didout = FALSE;
    বার্তা_col = 0;

    // No need to wait after an intentional redraw.
    need_wait_return = FALSE;

    // When invoked from a callback or autocmd the command line may be active.
    যদি (State & MODE_CMDLINE)
	redrawcmdline();

    out_flush();
}

/*
 * ":redrawstatus": force redraw of status line(s)
 */
    স্থির শূন্য
ex_redrawstatus(exarg_T *eap UNUSED)
{
    যদি (eap->forceit)
	status_redraw_all();
    অন্যথা
	status_redraw_curbuf();
    redraw_vseps = TRUE;
    যদি (বার্তা_scrolled && (State & MODE_CMDLINE))
	ফেরত;  // redraw later

    পূর্ণসংখ্যা save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    পূর্ণসংখ্যা save_p_lz = p_lz;
    p_lz = FALSE;

    যদি (State & MODE_CMDLINE)
	redraw_statuslines();
    অন্যথা
	update_screen(VIsual_active ? UPD_INVERTED : 0);
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabline": force redraw of the tabline
 */
    স্থির শূন্য
ex_redrawtabline(exarg_T *eap UNUSED)
{
    পূর্ণসংখ্যা save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    পূর্ণসংখ্যা save_p_lz = p_lz;
    p_lz = FALSE;

    draw_tabline();

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabpanel": force redraw of the tabpanel
 */
    স্থির শূন্য
ex_redrawtabpanel(exarg_T *eap UNUSED)
{
    পূর্ণসংখ্যা save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    পূর্ণসংখ্যা save_p_lz = p_lz;
    p_lz = FALSE;

#যদি defined(FEAT_TABPANEL)
    draw_tabpanel();
#অন্যথা
    emsg(_(e_cannot_not_support_redrawtabpanel));
#endif

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

    স্থির শূন্য
close_redir(শূন্য)
{
    যদি (redir_fd != ফাঁকা)
    {
	fclose(redir_fd);
	redir_fd = ফাঁকা;
    }
#ifdef FEAT_EVAL
    redir_reg = 0;
    যদি (redir_vname)
    {
	var_redir_stop();
	redir_vname = 0;
    }
#endif
}

#যদি defined(FEAT_SESSION) || defined(FEAT_EVAL)
    পূর্ণসংখ্যা
vim_mkdir_emsg(char_u *name, পূর্ণসংখ্যা prot UNUSED)
{
    যদি (vim_mkdir(name, prot) != 0)
    {
	semsg(_(e_cannot_create_directory_str), name);
	ফেরত FAIL;
    }
    ফেরত OK;
}
#endif

/*
 * Open a file for writing for an Ex command, with some checks.
 * Return file descriptor, or NULL on failure.
 */
    FILE *
open_exfile(
    char_u	*fname,
    পূর্ণসংখ্যা		forceit,
    অক্ষর	*mode)	    // "w" for create new file or "a" for append
{
    FILE	*fd;

#ifdef UNIX
    // with Unix it is possible to open a directory
    যদি (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	ফেরত ফাঁকা;
    }
#endif
    যদি (!forceit && *mode != 'a' && vim_fexists(fname))
    {
	semsg(_(e_str_exists_add_bang_to_override), fname);
	ফেরত ফাঁকা;
    }

    যদি ((fd = mch_fopen((অক্ষর *)fname, mode)) == ফাঁকা)
	semsg(_(e_cannot_open_str_for_writing_2), fname);

    ফেরত fd;
}

/*
 * ":mark" and ":k".
 */
    স্থির শূন্য
ex_mark(exarg_T *eap)
{
    pos_T	pos;

#ifdef FEAT_EVAL
    যদি (not_in_vim9(eap, false) == FAIL)
	ফেরত;
#endif
    যদি (*eap->arg == NUL)		// No argument?
    {
	emsg(_(e_argument_required));
	ফেরত;
    }

    যদি (eap->arg[1] != NUL)	// more than one character?
    {
	semsg(_(e_trailing_characters_str), eap->arg);
	ফেরত;
    }

    pos = curwin->w_cursor;		// save curwin->w_cursor
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_WHITE | BL_FIX);
    যদি (setmark(*eap->arg) == FAIL)	// set mark
	emsg(_(e_argument_must_be_letter_or_forward_backward_quote));
    curwin->w_cursor = pos;		// restore curwin->w_cursor
}

/*
 * Update w_topline, w_leftcol and the cursor position.
 */
    শূন্য
update_topline_cursor(শূন্য)
{
    check_cursor();		// put cursor on valid line
    update_topline();
    যদি (!curwin->w_p_wrap)
	validate_cursor();
    update_curswant();
}

/*
 * Save the current State and go to Normal mode.
 * Return TRUE if the typeahead could be saved.
 */
    পূর্ণসংখ্যা
save_current_state(save_state_T *sst)
{
    sst->save_বার্তা_scroll = বার্তা_scroll;
    sst->save_restart_edit = restart_edit;
    sst->save_বার্তা_didout = বার্তা_didout;
    sst->save_State = State;
    sst->save_insertmode = p_im;
    sst->save_finish_op = finish_op;
    sst->save_opcount = opcount;
    sst->save_reg_executing = reg_executing;
    sst->save_pending_end_reg_executing = pending_end_reg_executing;

    বার্তা_scroll = FALSE;		    // no msg scrolling in Normal mode
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
    ফেরত sst->tabuf.typebuf_valid;
}

    শূন্য
restore_current_state(save_state_T *sst)
{
    // Restore the previous typeahead.
    restore_typeahead(&sst->tabuf, FALSE);

    বার্তা_scroll = sst->save_বার্তা_scroll;
    restart_edit = sst->save_restart_edit;
    p_im = sst->save_insertmode;
    finish_op = sst->save_finish_op;
    opcount = sst->save_opcount;
    reg_executing = sst->save_reg_executing;
    pending_end_reg_executing = sst->save_pending_end_reg_executing;
    বার্তা_didout |= sst->save_বার্তা_didout;	// don't reset msg_didout now
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
    শূন্য
ex_normal(exarg_T *eap)
{
    save_state_T save_state;
    char_u	*arg = ফাঁকা;
    পূর্ণসংখ্যা		l;
    char_u	*p;

    যদি (ex_normal_lock > 0)
    {
	emsg(_(e_not_allowed_here));
	ফেরত;
    }
    যদি (ex_normal_busy >= p_mmd)
    {
	emsg(_(e_recursive_use_of_normal_too_deep));
	ফেরত;
    }

    /*
     * vgetc() expects a CSI and K_SPECIAL to have been escaped.  Don't do
     * this for the K_SPECIAL leading byte, otherwise special keys will not
     * work.
     */
    যদি (has_mbyte)
    {
	পূর্ণসংখ্যা	len = 0;

	// Count the number of characters to be escaped.
	জন্য (p = eap->arg; *p != NUL; ++p)
	{
#ifdef FEAT_GUI
	    যদি (*p == CSI)  // leadbyte CSI
		len += 2;
#endif
	    জন্য (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		যদি (*++p == K_SPECIAL	  // trailbyte K_SPECIAL or CSI
#ifdef FEAT_GUI
			|| *p == CSI
#endif
			)
		    len += 2;
	}
	যদি (len > 0)
	{
	    arg = alloc(STRLEN(eap->arg) + len + 1);
	    যদি (arg != ফাঁকা)
	    {
		len = 0;
		জন্য (p = eap->arg; *p != NUL; ++p)
		{
		    arg[len++] = *p;
#ifdef FEAT_GUI
		    যদি (*p == CSI)
		    {
			arg[len++] = KS_EXTRA;
			arg[len++] = (পূর্ণসংখ্যা)KE_CSI;
		    }
#endif
		    জন্য (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		    {
			arg[len++] = *++p;
			যদি (*p == K_SPECIAL)
			{
			    arg[len++] = KS_SPECIAL;
			    arg[len++] = KE_FILLER;
			}
#ifdef FEAT_GUI
			অন্যথা যদি (*p == CSI)
			{
			    arg[len++] = KS_EXTRA;
			    arg[len++] = (পূর্ণসংখ্যা)KE_CSI;
			}
#endif
		    }
		    arg[len] = NUL;
		}
	    }
	}
    }

    ++ex_normal_busy;
    যদি (save_current_state(&save_state))
    {
	/*
	 * Repeat the :normal command for each line in the range.  When no
	 * range given, execute it just once, without positioning the cursor
	 * first.
	 */
	করো
	{
	    যদি (eap->addr_গুনতি != 0)
	    {
		curwin->w_cursor.lnum = eap->line1++;
		curwin->w_cursor.col = 0;
		check_cursor_moved(curwin);
	    }

	    exec_normal_cmd(arg != ফাঁকা
		     ? arg
		     : eap->arg, eap->forceit ? REMAP_NONE : REMAP_YES, FALSE);
	}
	যখন (eap->addr_গুনতি > 0 && eap->line1 <= eap->line2 && !got_int);
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
    স্থির শূন্য
ex_startinsert(exarg_T *eap)
{
#ifdef FEAT_TERMINAL
    // Ignore this when running in an active terminal.
    যদি (term_job_running(curbuf->b_term))
	ফেরত;
#endif
    যদি (!curbuf->b_p_ma && !p_im)
    {
	// Only give this error when 'insertmode' is off.
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ফেরত;
    }
    যদি (eap->forceit)
    {
	// cursor line can be zero on startup
	যদি (!curwin->w_cursor.lnum)
	    curwin->w_cursor.lnum = 1;
	set_cursor_for_append_to_সারি();
    }

    // Ignore the command when already in Insert mode.  Inserting an
    // expression register that invokes a function can do this.
    যদি (State & MODE_ঢোকাও)
	ফেরত;

    যদি (eap->cmdidx == CMD_startinsert)
	restart_edit = 'a';
    অন্যথা যদি (eap->cmdidx == CMD_startreplace)
	restart_edit = 'R';
    অন্যথা
	restart_edit = 'V';

    যদি (!eap->forceit)
    {
	যদি (eap->cmdidx == CMD_startinsert)
	    restart_edit = 'i';
	curwin->w_curswant = 0;	    // avoid MAXCOL
    }

    যদি (VIsual_active)
	showmode();
}

/*
 * ":stopinsert"
 */
    স্থির শূন্য
ex_stopinsert(exarg_T *eap UNUSED)
{
    restart_edit = 0;
    stop_ঢোকাও_mode = TRUE;
#যদি defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // when called from remote_expr in insert mode, make sure insert mode is
    // ended by adding K_NOP to the typeahead buffer
    যদি (vgetc_busy)
	ins_char_typebuf(K_NOP, 0);
#endif
    clearmode();
}

/*
 * Execute normal mode command "cmd".
 * "remap" can be REMAP_NONE or REMAP_YES.
 */
    শূন্য
exec_normal_cmd(char_u *cmd, পূর্ণসংখ্যা remap, পূর্ণসংখ্যা silent)
{
    // Stuff the argument into the typeahead buffer.
    ins_typebuf(cmd, remap, 0, TRUE, silent);
    exec_normal(FALSE, FALSE, FALSE);
}

/*
 * Execute normal_cmd() until there is no typeahead left.
 * When "use_vpeekc" is TRUE use vpeekc() to check for available chars.
 */
    শূন্য
exec_normal(পূর্ণসংখ্যা was_typed, পূর্ণসংখ্যা use_vpeekc, পূর্ণসংখ্যা may_use_terminal_loop UNUSED)
{
    oparg_T	oa;
    পূর্ণসংখ্যা		c;

    // When calling vpeekc() from feedkeys() it will return Ctrl_C when there
    // is nothing to get, so also check for Ctrl_C.
    clear_oparg(&oa);
    finish_op = FALSE;
    যখন ((!stuff_empty()
		|| ((was_typed || !typebuf_typed()) && typebuf.tb_len > 0)
		|| (use_vpeekc && (c = vpeekc()) != NUL && c != Ctrl_C))
	    && !got_int)
    {
	update_topline_cursor();
#ifdef FEAT_TERMINAL
	যদি (may_use_terminal_loop && term_use_loop()
		&& oa.op_type == OP_NOP && oa.regname == NUL
		&& !VIsual_active)
	{
	    // If terminal_loop() returns OK we got a key that is handled
	    // in Normal model.  With FAIL we first need to position the
	    // cursor and the screen needs to be redrawn.
	    যদি (terminal_loop(TRUE) == OK)
		normal_cmd(&oa, TRUE);
	}
	অন্যথা
#endif
	    // execute a Normal mode cmd
	    normal_cmd(&oa, TRUE);
    }
}

#ifdef FEAT_FIND_ID
    স্থির শূন্য
ex_checkpath(exarg_T *eap)
{
    find_pattern_in_path(ফাঁকা, 0, 0, FALSE, FALSE, CHECK_PATH, 1L,
	    eap->forceit ? ACTION_SHOW_ALL : ACTION_SHOW,
	    (linenr_T)1, (linenr_T)MAXLNUM, eap->forceit, FALSE);
}

# যদি defined(FEAT_QUICKFIX)
/*
 * ":psearch"
 */
    স্থির শূন্য
ex_psearch(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;
    ex_findpat(eap);
    g_do_tagpreview = 0;
}
# endif

    স্থির শূন্য
ex_findpat(exarg_T *eap)
{
    পূর্ণসংখ্যা		whole = TRUE;
    long	n;
    char_u	*p;
    পূর্ণসংখ্যা		action;

    নির্বাচন (cmdnames[eap->cmdidx].cmd_name[2])
    {
	ক্ষেত্র 'e':	// ":psearch", ":isearch" and ":dsearch"
		যদি (cmdnames[eap->cmdidx].cmd_name[0] == 'p')
		    action = ACTION_GOTO;
		অন্যথা
		    action = ACTION_SHOW;
		বিরতি;
	ক্ষেত্র 'i':	// ":ilist" and ":dlist"
		action = ACTION_SHOW_ALL;
		বিরতি;
	ক্ষেত্র 'u':	// ":ijump" and ":djump"
		action = ACTION_GOTO;
		বিরতি;
	পূর্বনির্ধারিত:	// ":isplit" and ":dsplit"
		action = ACTION_SPLIT;
		বিরতি;
    }

    n = 1;
    যদি (vim_isdigit(*eap->arg))	// get count
    {
	n = getdigits(&eap->arg);
	eap->arg = skipwhite(eap->arg);
    }
    যদি (*eap->arg == '/')   // Match regexp, not just whole words
    {
	whole = FALSE;
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	যদি (*p)
	{
	    *p++ = NUL;
	    p = skipwhite(p);

	    // Check for trailing illegal characters
	    যদি (!ends_excmd2(eap->arg, p))
		eap->errmsg = ex_errmsg(e_trailing_characters_str, p);
	    অন্যথা
		set_nextcmd(eap, p);
	}
    }
    যদি (!eap->skip)
	find_pattern_in_path(eap->arg, 0, (পূর্ণসংখ্যা)STRLEN(eap->arg),
		whole, !eap->forceit,
		*eap->cmd == 'd' ? FIND_DEFINE : FIND_ANY, n, action,
		eap->line1, eap->line2, eap->forceit, FALSE);
}
#endif


#ifdef FEAT_QUICKFIX
/*
 * ":ptag", ":ptselect", ":ptjump", ":ptnext", etc.
 */
    স্থির শূন্য
ex_ptag(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;  // will be reset to 0 in ex_tag_cmd()
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
}

/*
 * ":pedit"
 */
    স্থির শূন্য
ex_pedit(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Edit the file.
    do_exedit(eap, ফাঁকা);

    back_to_current_window(curwin_save);
}

/*
 * ":pbuffer"
 */
    স্থির শূন্য
ex_pbuffer(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Go to the buffer.
    do_exbuffer(eap);

    back_to_current_window(curwin_save);
}

    স্থির শূন্য
prepare_preview_window(শূন্য)
{
    যদি (ERROR_IF_ANY_POPUP_WINDOW)
	ফেরত;

    // Open the preview window or popup and make it the current window.
    g_do_tagpreview = p_pvh;
    prepare_tagpreview(TRUE, TRUE, FALSE);
}

    স্থির শূন্য
back_to_current_window(win_T *curwin_save)
{
    যদি (curwin != curwin_save && win_valid(curwin_save))
    {
	// Return cursor to where we were
	validate_cursor();
	redraw_later(UPD_VALID);
	win_enter(curwin_save, TRUE);
    }
# ifdef FEAT_PROP_POPUP
    অন্যথা যদি (WIN_IS_POPUP(curwin))
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
    স্থির শূন্য
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
    স্থির শূন্য
ex_tag(exarg_T *eap)
{
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name);
}

    স্থির শূন্য
ex_tag_cmd(exarg_T *eap, char_u *name)
{
    পূর্ণসংখ্যা		cmd;

    নির্বাচন (name[1])
    {
	ক্ষেত্র 'j': cmd = DT_JUMP;	// ":tjump"
		  বিরতি;
	ক্ষেত্র 's': cmd = DT_SELECT;	// ":tselect"
		  বিরতি;
	ক্ষেত্র 'p': cmd = DT_পূর্ব;	// ":tprevious"
		  বিরতি;
	ক্ষেত্র 'N': cmd = DT_পূর্ব;	// ":tNext"
		  বিরতি;
	ক্ষেত্র 'n': cmd = DT_পরবর্তী;	// ":tnext"
		  বিরতি;
	ক্ষেত্র 'o': cmd = DT_বের_করো;		// ":pop"
		  বিরতি;
	ক্ষেত্র 'f':			// ":tfirst"
	ক্ষেত্র 'r': cmd = DT_FIRST;	// ":trewind"
		  বিরতি;
	ক্ষেত্র 'l': cmd = DT_LAST;	// ":tlast"
		  বিরতি;
	পূর্বনির্ধারিত:			// ":tag"
#ifdef FEAT_CSCOPE
		  যদি (p_cst && *eap->arg != NUL)
		  {
		      ex_cstag(eap);
		      ফেরত;
		  }
#endif
		  cmd = DT_TAG;
		  বিরতি;
    }

    যদি (name[0] == 'l')
    {
#ifndef FEAT_QUICKFIX
	ex_ni(eap);
	ফেরত;
#অন্যথা
	cmd = DT_LTAG;
#endif
    }

    do_tag(eap->arg, cmd, eap->addr_গুনতি > 0 ? (পূর্ণসংখ্যা)eap->line2 : 1,
							  eap->forceit, TRUE);
}

গণনা {
    SPEC_PERC = 0,
    SPEC_HASH,
    SPEC_CWORD,	    // cursor word
    SPEC_CCWORD,    // cursor WORD
    SPEC_CEXPR,	    // expr under cursor
    SPEC_CFILE,	    // cursor path name
    SPEC_SFILE,	    // ":so" file name
    SPEC_SLNUM,	    // ":so" file line number
    SPEC_স্তূপ,	    // call stack
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
    পূর্ণসংখ্যা
find_cmdline_var(char_u *src, size_t *usedlen)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    স্থির keyvalue_T spec_str_tab[] = {
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
	KEYVALUE_ENTRY(SPEC_স্তূপ, "stack>")	    // call stack
    };
    keyvalue_T target;
    keyvalue_T *entry;

    নির্বাচন (*src)
    {
    ক্ষেত্র '%':
	*usedlen = 1;
	ফেরত SPEC_PERC;

    ক্ষেত্র '#':
	*usedlen = 1;
	ফেরত SPEC_HASH;

    ক্ষেত্র '<':
	target.চাবি = 0;
	target.মান.string = src + 1;	    // skip over '<'
	target.মান.দৈর্ঘ্য = 0;	    // not used, see cmp_keyvalue_value_n()

	entry = (keyvalue_T *)bsearch(&target, &spec_str_tab,
		ARRAY_দৈর্ঘ্য(spec_str_tab), আকার(spec_str_tab[0]),
		cmp_keyvalue_মান_n);
	যদি (entry == ফাঁকা)
	    ফেরত -1;

	*usedlen = entry->মান.দৈর্ঘ্য + 1;
	ফেরত entry->চাবি;

    পূর্বনির্ধারিত:
	বিরতি;
    }

    ফেরত -1;
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
    অক্ষর	**errormsg,	// pointer to error message
    পূর্ণসংখ্যা		*escaped,	// return value has escaped white space (can
				// be NULL)
    পূর্ণসংখ্যা		empty_is_error)	// empty result is considered an error
{
    পূর্ণসংখ্যা		i;
    char_u	*s;
    char_u	*ফলাফল = (char_u *)"";
    char_u	*resultbuf = ফাঁকা;
    size_t	resultlen;
    buf_T	*buf;
    পূর্ণসংখ্যা		valid = VALID_শীর্ষ + VALID_PATH;    // assume valid result
    পূর্ণসংখ্যা		spec_idx;
    পূর্ণসংখ্যা		tilde_file = FALSE;
    পূর্ণসংখ্যা		skip_mod = FALSE;
    char_u	strbuf[30];

    *errormsg = ফাঁকা;
    যদি (escaped != ফাঁকা)
	*escaped = FALSE;

    /*
     * Check if there is something to do.
     */
    spec_idx = find_cmdline_var(src, usedlen);
    যদি (spec_idx < 0)	// no match
    {
	*usedlen = 1;
	ফেরত ফাঁকা;
    }

    /*
     * Skip when preceded with a backslash "\%" and "\#".
     * Note: In "\\%" the % is also not recognized!
     */
    যদি (src > srcstart && src[-1] == '\\')
    {
	*usedlen = 0;
	STRMOVE(src - 1, src);	// remove backslash
	ফেরত ফাঁকা;
    }

    /*
     * word or WORD under cursor
     */
    যদি (spec_idx == SPEC_CWORD || spec_idx == SPEC_CCWORD
						     || spec_idx == SPEC_CEXPR)
    {
	resultlen = find_ident_under_cursor(&ফলাফল,
		spec_idx == SPEC_CWORD ? (FIND_IDENT | FIND_STRING)
	      : spec_idx == SPEC_CEXPR ? (FIND_IDENT | FIND_STRING | FIND_EVAL)
	      : FIND_STRING);
	যদি (resultlen == 0)
	{
	    *errormsg = "";
	    ফেরত ফাঁকা;
	}
    }

    /*
     * '#': Alternate file name
     * '%': Current file name
     *	    File name under the cursor
     *	    File name for autocommand
     *	and following modifiers
     */
    অন্যথা
    {
	size_t off = 0;

	নির্বাচন (spec_idx)
	{
	ক্ষেত্র SPEC_PERC:
#ifdef FEAT_EVAL
		যদি (!in_vim9script() || src[1] != '%')
#endif
		{
		    // '%': current file
		    যদি (curbuf->b_fname == ফাঁকা)
		    {
			ফলাফল = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    অন্যথা
		    {
			ফলাফল = curbuf->b_fname;
			tilde_file = STRCMP(ফলাফল, "~") == 0;
		    }
		    বিরতি;
		}
#ifdef FEAT_EVAL
		// "%%" alternate file
		off = 1;
#endif
		// FALLTHROUGH
	ক্ষেত্র SPEC_HASH:		// '#' or "#99": alternate file
		যদি (off == 0 ? src[1] == '#' : src[2] == '%')
		{
		    // "##" or "%%%": the argument list
		    ফলাফল = arg_all();
		    resultbuf = ফলাফল;
		    *usedlen = off + 2;
		    যদি (escaped != ফাঁকা)
			*escaped = TRUE;
		    skip_mod = TRUE;
		    বিরতি;
		}
		s = src + off + 1;
		যদি (*s == '<')		// "#<99" uses v:oldfiles
		    ++s;
		i = (পূর্ণসংখ্যা)getdigits(&s);
		যদি (s == src + off + 2 && src[off + 1] == '-')
		    // just a minus sign, don't skip over it
		    s--;
		*usedlen = (পূর্ণসংখ্যা)(s - src); // length of what we expand

		যদি (src[off + 1] == '<' && i != 0)
		{
		    যদি (*usedlen < off + 2)
		    {
			// Should we give an error message for #<text?
			*usedlen = off + 1;
			ফেরত ফাঁকা;
		    }
#ifdef FEAT_EVAL
		    ফলাফল = list_find_str(get_vim_var_list(VV_OLDFILES),
								     (long)i);
		    যদি (ফলাফল == ফাঁকা)
		    {
			*errormsg = "";
			ফেরত ফাঁকা;
		    }
#অন্যথা
		    *errormsg = _(e_hashsmall_is_not_available_without_the_eval_feature);
		    ফেরত ফাঁকা;
#endif
		}
		অন্যথা
		{
		    যদি (i == 0 && src[off + 1] == '<' && *usedlen > off + 1)
			*usedlen = off + 1;
		    buf = buflist_findnr(i);
		    যদি (buf == ফাঁকা)
		    {
			*errormsg = _(e_no_alternate_file_name_to_substitute_for_hash);
			ফেরত ফাঁকা;
		    }
		    যদি (lnump != ফাঁকা)
			*lnump = ECMD_LAST;
		    যদি (buf->b_fname == ফাঁকা)
		    {
			ফলাফল = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    অন্যথা
		    {
			ফলাফল = buf->b_fname;
			tilde_file = STRCMP(ফলাফল, "~") == 0;
		    }
		}
		বিরতি;

	ক্ষেত্র SPEC_CFILE:	// file name under cursor
		ফলাফল = file_name_at_cursor(FNAME_MESS|FNAME_HYP, 1L, ফাঁকা);
		যদি (ফলাফল == ফাঁকা)
		{
		    *errormsg = "";
		    ফেরত ফাঁকা;
		}
		resultbuf = ফলাফল;	    // remember allocated string
		বিরতি;

	ক্ষেত্র SPEC_AFILE:	// file name for autocommand
		ফলাফল = autocmd_fname;
		যদি (ফলাফল != ফাঁকা && !autocmd_fname_full)
		{
		    // Still need to turn the fname into a full path.  It is
		    // postponed to avoid a delay when <afile> is not used.
		    autocmd_fname_full = TRUE;
		    ফলাফল = FullName_save(autocmd_fname, FALSE);
		    vim_free(autocmd_fname);
		    autocmd_fname = ফলাফল;
		}
		যদি (ফলাফল == ফাঁকা)
		{
		    *errormsg = _(e_no_autocommand_file_name_to_substitute_for_afile);
		    ফেরত ফাঁকা;
		}
		ফলাফল = shorten_fname1(ফলাফল);
		বিরতি;

	ক্ষেত্র SPEC_ABUF:		// buffer number for autocommand
		যদি (autocmd_bufnr <= 0)
		{
		    *errormsg = _(e_no_autocommand_বাফার_সংখ্যা_to_substitute_for_abuf);
		    ফেরত ফাঁকা;
		}
		sprintf((অক্ষর *)strbuf, "%d", autocmd_bufnr);
		ফলাফল = strbuf;
		বিরতি;

	ক্ষেত্র SPEC_AMATCH:	// match name for autocommand
		ফলাফল = autocmd_match;
		যদি (ফলাফল == ফাঁকা)
		{
		    *errormsg = _(e_no_autocommand_match_name_to_substitute_for_amatch);
		    ফেরত ফাঁকা;
		}
		বিরতি;

	ক্ষেত্র SPEC_SFILE:	// file name for ":so" command
		ফলাফল = estack_sfile(ESTACK_SFILE);
		যদি (ফলাফল == ফাঁকা)
		{
		    *errormsg = _(e_no_source_file_name_to_substitute_for_sfile);
		    ফেরত ফাঁকা;
		}
		resultbuf = ফলাফল;	    // remember allocated string
		বিরতি;
	ক্ষেত্র SPEC_স্তূপ:	// call stack
		ফলাফল = estack_sfile(ESTACK_স্তূপ);
		যদি (ফলাফল == ফাঁকা)
		{
		    *errormsg = _(e_no_call_স্তূপ_to_substitute_for_স্তূপ);
		    ফেরত ফাঁকা;
		}
		resultbuf = ফলাফল;	    // remember allocated string
		বিরতি;
	ক্ষেত্র SPEC_SCRIPT:	// script file name
		ফলাফল = estack_sfile(ESTACK_SCRIPT);
		যদি (ফলাফল == ফাঁকা)
		{
		    *errormsg = _(e_no_script_file_name_to_substitute_for_script);
		    ফেরত ফাঁকা;
		}
		resultbuf = ফলাফল;	    // remember allocated string
		বিরতি;

	ক্ষেত্র SPEC_SLNUM:	// line in file for ":so" command
		যদি (SOURCING_NAME == ফাঁকা || SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_সারি_সংখ্যা_to_use_for_slnum);
		    ফেরত ফাঁকা;
		}
		sprintf((অক্ষর *)strbuf, "%ld", SOURCING_LNUM);
		ফলাফল = strbuf;
		বিরতি;

#ifdef FEAT_EVAL
	ক্ষেত্র SPEC_SFLNUM:	// line in script file
		যদি (current_sctx.sc_lnum + SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_সারি_সংখ্যা_to_use_for_sflnum);
		    ফেরত ফাঁকা;
		}
		sprintf((অক্ষর *)strbuf, "%ld",
				 (long)(current_sctx.sc_lnum + SOURCING_LNUM));
		ফলাফল = strbuf;
		বিরতি;

	ক্ষেত্র SPEC_SID:
		যদি (current_sctx.sc_sid <= 0)
		{
		    *errormsg = _(e_using_sid_not_in_script_context);
		    ফেরত ফাঁকা;
		}
		sprintf((অক্ষর *)strbuf, "<SNR>%d_", current_sctx.sc_sid);
		ফলাফল = strbuf;
		বিরতি;
#endif

#ifdef FEAT_CLIENTSERVER
	ক্ষেত্র SPEC_CLIENT:	// Source of last submitted input
# ifdef MSWIN
		sprintf((অক্ষর *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		ফলাফল = strbuf;
# অন্যথা
#  ifdef FEAT_SOCKETSERVER
		যদি (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		{
		    যদি (client_socket == ফাঁকা)
			ফলাফল = (char_u *)"";
		    অন্যথা
			ফলাফল = client_socket;
		}
#  endif
#  ifdef FEAT_X11
		যদি (clientserver_method == CLIENTSERVER_METHOD_X11)
		{
		    sprintf((অক্ষর *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		    ফলাফল = strbuf;
		}
#  endif
# endif
		বিরতি;
#endif
	}

	resultlen = STRLEN(ফলাফল);	// length of new string
	যদি (src[*usedlen] == '<')	// remove the file name extension
	{
	    ++*usedlen;
	    যদি ((s = vim_strrchr(ফলাফল, '.')) != ফাঁকা && s >= gettail(ফলাফল))
		resultlen = s - ফলাফল;
	}
	অন্যথা যদি (!skip_mod)
	{
	    valid |= modify_fname(src, tilde_file, usedlen, &ফলাফল, &resultbuf,
								  &resultlen);
	    যদি (ফলাফল == ফাঁকা)
	    {
		*errormsg = "";
		ফেরত ফাঁকা;
	    }
	}
    }

    যদি (resultlen == 0 || valid != VALID_শীর্ষ + VALID_PATH)
    {
	যদি (empty_is_error)
	{
	    যদি (valid != VALID_শীর্ষ + VALID_PATH)
		*errormsg = _(e_empty_file_name_for_percent_or_hash_only_works_with_ph);
	    অন্যথা
		*errormsg = _(e_evaluates_to_an_empty_string);
	}
	ফলাফল = ফাঁকা;
    }
    অন্যথা
	ফলাফল = vim_strnsave(ফলাফল, resultlen);
    vim_free(resultbuf);
    ফেরত ফলাফল;
}

/*
 * Expand the <sfile> string in "arg".
 *
 * Returns an allocated string, or NULL for any error.
 */
    char_u *
expand_sfile(char_u *arg)
{
    অক্ষর	*errormsg;
    char_u	*ফলাফল;
    size_t	resultlen;
    char_u	*newres;
    size_t	len;
    char_u	*repl;
    size_t	repllen;
    size_t	srclen;
    char_u	*p;

    resultlen = STRLEN(arg);
    ফলাফল = vim_strnsave(arg, resultlen);
    যদি (ফলাফল == ফাঁকা)
	ফেরত ফাঁকা;

    জন্য (p = ফলাফল; *p; )
    {
	যদি (STRNCMP(p, "<sfile>", 7) != 0)
	    ++p;
	অন্যথা
	{
	    // replace "<sfile>" with the sourced file name, and do ":" stuff
	    repl = eval_vars(p, ফলাফল, &srclen, ফাঁকা, &errormsg, ফাঁকা, TRUE);
	    যদি (errormsg != ফাঁকা)
	    {
		যদি (*errormsg)
		    emsg(errormsg);
		vim_free(ফলাফল);
		ফেরত ফাঁকা;
	    }
	    যদি (repl == ফাঁকা)		// no match (cannot happen)
	    {
		p += srclen;
		চালু;
	    }
	    repllen = STRLEN(repl);
	    resultlen += repllen - srclen;
	    newres = alloc(resultlen + 1);
	    যদি (newres == ফাঁকা)
	    {
		vim_free(repl);
		vim_free(ফলাফল);
		ফেরত ফাঁকা;
	    }
	    len = p - ফলাফল;
	    mch_memmove(newres, ফলাফল, len);
	    STRCPY(newres + len, repl);
	    len += repllen;
	    STRCPY(newres + len, p + srclen);
	    vim_free(repl);
	    vim_free(ফলাফল);
	    ফলাফল = newres;
	    p = newres + len;		// continue after the match
	}
    }

    ফেরত ফলাফল;
}

#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
/*
 * Make a dialog message in "buff[DIALOG_MSG_SIZE]".
 * "format" must contain "%s".
 */
    শূন্য
dialog_বার্তা(char_u *buff, অক্ষর *format, char_u *fname)
{
    যদি (fname == ফাঁকা)
	fname = (char_u *)_("Untitled");
    vim_snprintf((অক্ষর *)buff, DIALOG_বার্তা_আকার, format, fname);
}
#endif

/*
 * ":behave {mswin,xterm}"
 */
    স্থির শূন্য
ex_behave(exarg_T *eap)
{
    যদি (STRCMP(eap->arg, "mswin") == 0)
    {
	set_option_মান_give_err((char_u *)"selection",
						 0L, (char_u *)"exclusive", 0);
	set_option_মান_give_err((char_u *)"selectmode",
						 0L, (char_u *)"mouse,key", 0);
	set_option_মান_give_err((char_u *)"mousemodel",
						     0L, (char_u *)"popup", 0);
	set_option_মান_give_err((char_u *)"keymodel",
					  0L, (char_u *)"startsel,stopsel", 0);
    }
    অন্যথা যদি (STRCMP(eap->arg, "xterm") == 0)
    {
	set_option_মান_give_err((char_u *)"selection",
						 0L, (char_u *)"inclusive", 0);
	set_option_মান_give_err((char_u *)"selectmode", 0L, (char_u *)"", 0);
	set_option_মান_give_err((char_u *)"mousemodel",
						    0L, (char_u *)"extend", 0);
	set_option_মান_give_err((char_u *)"keymodel", 0L, (char_u *)"", 0);
    }
    অন্যথা
	semsg(_(e_invalid_argument_str), eap->arg);
}

স্থির পূর্ণসংখ্যা filetype_detect = FALSE;
স্থির পূর্ণসংখ্যা filetype_plugin = FALSE;
স্থির পূর্ণসংখ্যা filetype_indent = FALSE;

/*
 * ":filetype [plugin] [indent] {on,off,detect}"
 * on: Load the filetype.vim file to install autocommands for file types.
 * off: Load the ftoff.vim file to remove all autocommands for file types.
 * plugin on: load filetype.vim and ftplugin.vim
 * plugin off: load ftplugof.vim
 * indent on: load filetype.vim and indent.vim
 * indent off: load indoff.vim
 */
    স্থির শূন্য
ex_filetype(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    পূর্ণসংখ্যা		plugin = FALSE;
    পূর্ণসংখ্যা		indent = FALSE;

    যদি (*eap->arg == NUL)
    {
	// Print current status.
	smsg("filetype detection:%s  plugin:%s  indent:%s",
		filetype_detect ? "ON" : "OFF",
		filetype_plugin ? (filetype_detect ? "ON" : "(on)") : "OFF",
		filetype_indent ? (filetype_detect ? "ON" : "(on)") : "OFF");
	ফেরত;
    }

    // Accept "plugin" and "indent" in any order.
    জন্য (;;)
    {
	যদি (STRNCMP(arg, "plugin", 6) == 0)
	{
	    plugin = TRUE;
	    arg = skipwhite(arg + 6);
	    চালু;
	}
	যদি (STRNCMP(arg, "indent", 6) == 0)
	{
	    indent = TRUE;
	    arg = skipwhite(arg + 6);
	    চালু;
	}
	বিরতি;
    }
    যদি (STRCMP(arg, "on") == 0 || STRCMP(arg, "detect") == 0)
    {
	যদি (*arg == 'o' || !filetype_detect)
	{
	    source_runtime((char_u *)FILETYPE_FILE, DIP_ALL);
	    filetype_detect = TRUE;
	    যদি (plugin)
	    {
		source_runtime((char_u *)FTPLUGIN_FILE, DIP_ALL);
		filetype_plugin = TRUE;
	    }
	    যদি (indent)
	    {
		source_runtime((char_u *)INDENT_FILE, DIP_ALL);
		filetype_indent = TRUE;
	    }
	}
	যদি (*arg == 'd')
	{
	    (শূন্য)do_doautocmd((char_u *)"filetypedetect BufRead", TRUE, ফাঁকা);
	    do_modelines(0);
	}
    }
    অন্যথা যদি (STRCMP(arg, "off") == 0)
    {
	যদি (plugin || indent)
	{
	    যদি (plugin)
	    {
		source_runtime((char_u *)FTPLUGOF_FILE, DIP_ALL);
		filetype_plugin = FALSE;
	    }
	    যদি (indent)
	    {
		source_runtime((char_u *)INDOFF_FILE, DIP_ALL);
		filetype_indent = FALSE;
	    }
	}
	অন্যথা
	{
	    source_runtime((char_u *)FTOFF_FILE, DIP_ALL);
	    filetype_detect = FALSE;
	}
    }
    অন্যথা
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * ":setfiletype [FALLBACK] {name}"
 */
    স্থির শূন্য
ex_setfiletype(exarg_T *eap)
{
    যদি (curbuf->b_did_filetype)
	ফেরত;

    char_u *arg = eap->arg;
    যদি (STRNCMP(arg, "FALLBACK ", 9) == 0)
	arg += 9;

    set_option_মান_give_err((char_u *)"filetype", 0L, arg, OPT_LOCAL);
    যদি (arg != eap->arg)
	curbuf->b_did_filetype = false;
}

    স্থির শূন্য
ex_digraphs(exarg_T *eap UNUSED)
{
#ifdef FEAT_DIGRAPHS
    যদি (*eap->arg != NUL)
	putdigraph(eap->arg);
    অন্যথা
	listdigraphs(eap->forceit);
#অন্যথা
    emsg(_(e_no_digraphs_version));
#endif
}

#যদি defined(FEAT_অনুসন্ধান_EXTRA)
    শূন্য
set_no_hlsearch(পূর্ণসংখ্যা flag)
{
    no_hlsearch = flag;
# ifdef FEAT_EVAL
    set_vim_var_nr(VV_HLSEARCH, !no_hlsearch && p_hls);
# endif
}

/*
 * ":nohlsearch"
 */
    স্থির শূন্য
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
    স্থির শূন্য
ex_X(exarg_T *eap UNUSED)
{
    crypt_check_current_method();
    (শূন্য)crypt_get_চাবি(TRUE, TRUE);
}
#endif

#ifdef FEAT_FOLDING
    স্থির শূন্য
ex_fold(exarg_T *eap)
{
    যদি (foldManualAllowed(TRUE))
	foldCreate(eap->line1, eap->line2);
}

    স্থির শূন্য
ex_foldopen(exarg_T *eap)
{
    opFoldRange(eap->line1, eap->line2, eap->cmdidx == CMD_foldopen,
							 eap->forceit, FALSE);
}

    স্থির শূন্য
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
    জন্য (lnum = eap->line1; lnum <= eap->line2; ++lnum)
	যদি (hasFolding(lnum, ফাঁকা, ফাঁকা) == (eap->cmdidx == CMD_folddoclosed))
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

#যদি defined(FEAT_QUICKFIX)
/*
 * Returns TRUE if the supplied Ex cmdidx is for a location list command
 * instead of a quickfix command.
 */
    পূর্ণসংখ্যা
is_loclist_cmd(পূর্ণসংখ্যা cmdidx)
{
    যদি (cmdidx < 0 || cmdidx >= CMD_আকার)
	ফেরত FALSE;
    ফেরত cmdnames[cmdidx].cmd_name[0] == 'l';
}
#endif

    পূর্ণসংখ্যা
get_pressedreturn(শূন্য)
{
    ফেরত ex_pressedreturn;
}

    শূন্য
set_pressedreturn(পূর্ণসংখ্যা val)
{
    ex_pressedreturn = val;
}
