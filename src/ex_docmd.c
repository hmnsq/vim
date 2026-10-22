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

#અન્તર્ભાવ "vim.h"

સ્થિર પૂર્ણાંક	quitmore = 0;
સ્થિર પૂર્ણાંક	ex_pressedreturn = FALSE;
#ifndef FEAT_PRINTER
# define ex_hardcopy	ex_ni
#endif

#ifdef FEAT_EVAL
સ્થિર char_u	*do_one_cmd(char_u **, પૂર્ણાંક, cstack_T *, char_u *(*fgetline)(પૂર્ણાંક, શૂન્ય *, પૂર્ણાંક, getline_opt_T), શૂન્ય *cookie);
#નહીંતર
સ્થિર char_u	*do_one_cmd(char_u **, પૂર્ણાંક, char_u *(*fgetline)(પૂર્ણાંક, શૂન્ય *, પૂર્ણાંક, getline_opt_T), શૂન્ય *cookie);
સ્થિર પૂર્ણાંક	if_level = 0;		// depth in :if
#endif
સ્થિર શૂન્ય	append_command(char_u *cmd);

#ifndef FEAT_MENU
# define ex_emenu		ex_ni
# define ex_menu		ex_ni
# define ex_menutranslate	ex_ni
#endif
સ્થિર શૂન્ય	ex_autocmd(exarg_T *eap);
સ્થિર શૂન્ય	ex_doautocmd(exarg_T *eap);
સ્થિર શૂન્ય	ex_bunload(exarg_T *eap);
સ્થિર શૂન્ય	ex_buffer(exarg_T *eap);
સ્થિર શૂન્ય	do_exbuffer(exarg_T *eap);
સ્થિર શૂન્ય	ex_bmodified(exarg_T *eap);
સ્થિર શૂન્ય	ex_bnext(exarg_T *eap);
સ્થિર શૂન્ય	ex_bprevious(exarg_T *eap);
સ્થિર શૂન્ય	ex_brewind(exarg_T *eap);
સ્થિર શૂન્ય	ex_blast(exarg_T *eap);
સ્થિર char_u	*getargcmd(char_u **);
સ્થિર પૂર્ણાંક	getargopt(exarg_T *eap);
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
#જો !defined(FEAT_QUICKFIX) || !defined(FEAT_EVAL)
# define ex_cexpr		ex_ni
#endif

સ્થિર linenr_T default_address(exarg_T *eap);
સ્થિર શૂન્ય address_default_all(exarg_T *eap);
સ્થિર શૂન્ય	get_flags(exarg_T *eap);
#જો !defined(FEAT_PERL) \
	|| !defined(FEAT_PYTHON) || !defined(FEAT_PYTHON3) \
	|| !defined(FEAT_TCL) \
	|| !defined(FEAT_RUBY) \
	|| !defined(FEAT_LUA) \
	|| !defined(FEAT_MZSCHEME)
# define HAVE_EX_SCRIPT_NI
સ્થિર શૂન્ય	ex_script_ni(exarg_T *eap);
#endif
સ્થિર અક્ષર	*invalid_range(exarg_T *eap);
સ્થિર શૂન્ય	correct_range(exarg_T *eap);
#ifdef FEAT_QUICKFIX
સ્થિર char_u	*replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep);
#endif
સ્થિર char_u	*repl_cmdline(exarg_T *eap, char_u *src, size_t srclen, char_u *repl, char_u **cmdlinep);
સ્થિર શૂન્ય	ex_highlight(exarg_T *eap);
સ્થિર શૂન્ય	ex_colorscheme(exarg_T *eap);
સ્થિર શૂન્ય	ex_cquit(exarg_T *eap);
સ્થિર શૂન્ય	ex_quit_all(exarg_T *eap);
સ્થિર શૂન્ય	ex_close(exarg_T *eap);
સ્થિર શૂન્ય	ex_win_close(પૂર્ણાંક forceit, win_T *win, tabpage_T *tp);
સ્થિર શૂન્ય	ex_only(exarg_T *eap);
સ્થિર શૂન્ય	ex_resize(exarg_T *eap);
સ્થિર શૂન્ય	ex_stag(exarg_T *eap);
સ્થિર શૂન્ય	ex_tabclose(exarg_T *eap);
સ્થિર શૂન્ય	ex_tabonly(exarg_T *eap);
સ્થિર શૂન્ય	ex_tabnext(exarg_T *eap);
સ્થિર શૂન્ય	ex_tabmove(exarg_T *eap);
સ્થિર શૂન્ય	ex_tabs(exarg_T *eap);
#જો defined(FEAT_QUICKFIX)
સ્થિર શૂન્ય	ex_pclose(exarg_T *eap);
સ્થિર શૂન્ય	ex_ptag(exarg_T *eap);
સ્થિર શૂન્ય	ex_pedit(exarg_T *eap);
સ્થિર શૂન્ય	ex_pbuffer(exarg_T *eap);
સ્થિર શૂન્ય	prepare_preview_window(શૂન્ય);
સ્થિર શૂન્ય	back_to_current_window(win_T *curwin_save);
#નહીંતર
# define ex_pclose		ex_ni
# define ex_ptag		ex_ni
# define ex_pedit		ex_ni
# define ex_pbuffer		ex_ni
#endif
સ્થિર શૂન્ય	ex_hide(exarg_T *eap);
સ્થિર શૂન્ય	ex_exit(exarg_T *eap);
સ્થિર શૂન્ય	ex_print(exarg_T *eap);
#ifdef FEAT_BYTEOFF
સ્થિર શૂન્ય	ex_goto(exarg_T *eap);
#નહીંતર
# define ex_goto		ex_ni
#endif
સ્થિર શૂન્ય	ex_shell(exarg_T *eap);
સ્થિર શૂન્ય	ex_preserve(exarg_T *eap);
સ્થિર શૂન્ય	ex_recover(exarg_T *eap);
સ્થિર શૂન્ય	ex_mode(exarg_T *eap);
સ્થિર શૂન્ય	ex_wrongmodifier(exarg_T *eap);
સ્થિર શૂન્ય	ex_find(exarg_T *eap);
સ્થિર શૂન્ય	ex_open(exarg_T *eap);
સ્થિર શૂન્ય	ex_edit(exarg_T *eap);
#ifndef FEAT_GUI
# define ex_gui			ex_nogui
સ્થિર શૂન્ય	ex_nogui(exarg_T *eap);
#endif
#જો defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
સ્થિર શૂન્ય	ex_tearoff(exarg_T *eap);
#નહીંતર
# define ex_tearoff		ex_ni
#endif
#જો (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
સ્થિર શૂન્ય	ex_popup(exarg_T *eap);
#નહીંતર
# define ex_popup		ex_ni
#endif
#ifndef FEAT_GUI_MSWIN
# define ex_simalt		ex_ni
#endif
#જો !defined(FEAT_GUI_MSWIN) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
# define gui_mch_find_dialog	ex_ni
# define gui_mch_replace_dialog ex_ni
#endif
#જો !defined(FEAT_GUI_GTK)
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
#જો !defined(FEAT_SYN_HL) || !defined(FEAT_PROFILE)
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
#જો !defined(FEAT_PYTHON) && !defined(FEAT_PYTHON3)
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
સ્થિર શૂન્ય	ex_swapname(exarg_T *eap);
સ્થિર શૂન્ય	ex_syncbind(exarg_T *eap);
સ્થિર શૂન્ય	ex_read(exarg_T *eap);
સ્થિર શૂન્ય	ex_pwd(exarg_T *eap);
સ્થિર શૂન્ય	ex_equal(exarg_T *eap);
સ્થિર શૂન્ય	ex_sleep(exarg_T *eap);
સ્થિર શૂન્ય	ex_winsize(exarg_T *eap);
સ્થિર શૂન્ય	ex_wincmd(exarg_T *eap);
#જો defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
સ્થિર શૂન્ય	ex_winpos(exarg_T *eap);
#નહીંતર
# define ex_winpos	    ex_ni
#endif
સ્થિર શૂન્ય	ex_operators(exarg_T *eap);
સ્થિર શૂન્ય	ex_put(exarg_T *eap);
સ્થિર શૂન્ય	ex_iput(exarg_T *eap);
સ્થિર શૂન્ય	ex_copymove(exarg_T *eap);
સ્થિર શૂન્ય	ex_submagic(exarg_T *eap);
સ્થિર શૂન્ય	ex_join(exarg_T *eap);
સ્થિર શૂન્ય	ex_at(exarg_T *eap);
સ્થિર શૂન્ય	ex_bang(exarg_T *eap);
સ્થિર શૂન્ય	ex_undo(exarg_T *eap);
#ifdef FEAT_PERSISTENT_UNDO
સ્થિર શૂન્ય	ex_wundo(exarg_T *eap);
સ્થિર શૂન્ય	ex_rundo(exarg_T *eap);
#endif
સ્થિર શૂન્ય	ex_redo(exarg_T *eap);
સ્થિર શૂન્ય	ex_later(exarg_T *eap);
સ્થિર શૂન્ય	ex_redir(exarg_T *eap);
સ્થિર શૂન્ય	ex_redrawstatus(exarg_T *eap);
સ્થિર શૂન્ય	ex_redrawtabline(exarg_T *eap);
સ્થિર શૂન્ય	ex_redrawtabpanel(exarg_T *eap);
સ્થિર શૂન્ય	close_redir(શૂન્ય);
સ્થિર શૂન્ય	ex_mark(exarg_T *eap);
સ્થિર શૂન્ય	ex_startinsert(exarg_T *eap);
સ્થિર શૂન્ય	ex_stopinsert(exarg_T *eap);
#ifdef FEAT_FIND_ID
સ્થિર શૂન્ય	ex_checkpath(exarg_T *eap);
સ્થિર શૂન્ય	ex_findpat(exarg_T *eap);
#નહીંતર
# define ex_findpat		ex_ni
# define ex_checkpath		ex_ni
#endif
#જો defined(FEAT_FIND_ID) && defined(FEAT_QUICKFIX)
સ્થિર શૂન્ય	ex_psearch(exarg_T *eap);
#નહીંતર
# define ex_psearch		ex_ni
#endif
સ્થિર શૂન્ય	ex_tag(exarg_T *eap);
સ્થિર શૂન્ય	ex_tag_cmd(exarg_T *eap, char_u *name);
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
સ્થિર શૂન્ય	ex_behave(exarg_T *eap);
સ્થિર શૂન્ય	ex_filetype(exarg_T *eap);
સ્થિર શૂન્ય	ex_setfiletype(exarg_T *eap);
#ifndef FEAT_DIFF
# define ex_diffoff		ex_ni
# define ex_diffpatch		ex_ni
# define ex_diffgetput		ex_ni
# define ex_diffsplit		ex_ni
# define ex_diffthis		ex_ni
# define ex_diffupdate		ex_ni
#endif
સ્થિર શૂન્ય	ex_digraphs(exarg_T *eap);
#ifdef FEAT_શોધ_EXTRA
સ્થિર શૂન્ય	ex_nohlsearch(exarg_T *eap);
#નહીંતર
# define ex_nohlsearch		ex_ni
# define ex_match		ex_ni
#endif
#ifdef FEAT_CRYPT
સ્થિર શૂન્ય	ex_X(exarg_T *eap);
#નહીંતર
# define ex_X			ex_ni
#endif
#ifdef FEAT_FOLDING
સ્થિર શૂન્ય	ex_fold(exarg_T *eap);
સ્થિર શૂન્ય	ex_foldopen(exarg_T *eap);
સ્થિર શૂન્ય	ex_folddo(exarg_T *eap);
#નહીંતર
# define ex_fold		ex_ni
# define ex_foldopen		ex_ni
# define ex_folddo		ex_ni
#endif
#જો !(defined(HAVE_LOCALE_H) || defined(X_LOCALE))
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
#જો !defined(FEAT_X11) || !defined(FEAT_XCLIPBOARD)
# define ex_xrestore		ex_ni
#endif
#જો !defined(FEAT_WAYLAND)
# define ex_wlrestore		ex_ni
#endif
#જો !defined(HAVE_CLIPMETHOD)
# define ex_clipreset		ex_ni
#endif
#જો !defined(FEAT_PROP_POPUP)
# define ex_popupclear		ex_ni
#endif

/*
 * Declare cmdnames[].
 */
#define DO_DECLARE_EXCMD
#અન્તર્ભાવ "ex_cmds.h"
#અન્તર્ભાવ "ex_cmdidxs.h"

સ્થિર char_u dollar_command[2] = {'$', 0};


#ifdef FEAT_EVAL
// Struct for storing a line inside a while/for loop
પ્રકારવ્યાખ્યા સંરચના
{
    char_u	*લીટી;		// command line
    linenr_T	lnum;		// sourcing_lnum of the line
} wcmd_T;

/*
 * Structure used to store info for line position in a while or for loop.
 * This is required, because do_one_cmd() may invoke ex_function(), which
 * reads more lines that may come from the while/for loop.
 */
સંરચના loop_cookie
{
    garray_T	*લીટીઓ_gap;		// growarray with line info
    પૂર્ણાંક		current_લીટી;		// last read line from growarray
    પૂર્ણાંક		repeating;		// TRUE when looping a second time
    // When "repeating" is FALSE use "getline" and "cookie" to get lines
    char_u	*(*lc_getline)(પૂર્ણાંક, શૂન્ય *, પૂર્ણાંક, getline_opt_T);
    શૂન્ય	*cookie;
};

સ્થિર char_u	*get_loop_લીટી(પૂર્ણાંક c, શૂન્ય *cookie, પૂર્ણાંક indent, getline_opt_T options);
સ્થિર પૂર્ણાંક	store_loop_લીટી(garray_T *gap, char_u *લીટી);
સ્થિર શૂન્ય	free_cmdlines(garray_T *gap);

// Struct to save a few things while debugging.  Used in do_cmdline() only.
સંરચના dbg_stuff
{
    પૂર્ણાંક		trylevel;
    પૂર્ણાંક		force_abort;
    except_T	*caught_ઢગલો;
    char_u	*vv_exception;
    char_u	*vv_throwpoint;
    પૂર્ણાંક		did_emsg;
    પૂર્ણાંક		got_int;
    પૂર્ણાંક		did_throw;
    પૂર્ણાંક		need_rethrow;
    પૂર્ણાંક		check_cstack;
    except_T	*current_exception;
};

    સ્થિર શૂન્ય
save_dbg_stuff(સંરચના dbg_stuff *dsp)
{
    dsp->trylevel	= trylevel;		trylevel = 0;
    dsp->force_abort	= force_abort;		force_abort = FALSE;
    dsp->caught_ઢગલો	= caught_ઢગલો;		caught_ઢગલો = ખાલી;
    dsp->vv_exception	= v_exception(ખાલી);
    dsp->vv_throwpoint	= v_throwpoint(ખાલી);

    // Necessary for debugging an inactive ":catch", ":finally", ":endtry"
    dsp->did_emsg	= did_emsg;		did_emsg     = FALSE;
    dsp->got_int	= got_int;		got_int	     = FALSE;
    dsp->did_throw	= did_throw;		did_throw    = FALSE;
    dsp->need_rethrow	= need_rethrow;		need_rethrow = FALSE;
    dsp->check_cstack	= check_cstack;		check_cstack = FALSE;
    dsp->current_exception = current_exception;	current_exception = ખાલી;
}

    સ્થિર શૂન્ય
restore_dbg_stuff(સંરચના dbg_stuff *dsp)
{
    suppress_errthrow = FALSE;
    trylevel = dsp->trylevel;
    force_abort = dsp->force_abort;
    caught_ઢગલો = dsp->caught_ઢગલો;
    (શૂન્ય)v_exception(dsp->vv_exception);
    (શૂન્ય)v_throwpoint(dsp->vv_throwpoint);
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
    સ્થિર પૂર્ણાંક
is_other_file(પૂર્ણાંક fnum, char_u *ffname)
{
    જો (fnum != 0)
    {
	જો (fnum == curbuf->b_fnum)
	    પાછા FALSE;

	પાછા TRUE;
    }

    જો (ffname == ખાલી)
	પાછા TRUE;

    જો (*ffname == NUL)
	પાછા FALSE;

    // TODO: Need a reliable way to know whether a buffer is meant to live
    // on-disk !curbuf->b_dev_valid is not always available (example: missing
    // on Windows)
    જો (curbuf->b_sfname != ખાલી
	    && *curbuf->b_sfname != NUL)
	// This occurs with unsaved buffers. In which case `ffname` actually
	// corresponds to curbuf->b_sfname
	પાછા fnamecmp(ffname, curbuf->b_sfname) != 0;

    પાછા otherfile(ffname);
}

/*
 * do_exmode(): Repeatedly get commands for the "Ex" mode, until the ":vi"
 * command is given.
 */
    શૂન્ય
do_exmode(
    પૂર્ણાંક		improved)	    // TRUE for "improved Ex" mode
{
    પૂર્ણાંક		save_સંદેશ_scroll;
    પૂર્ણાંક		prev_સંદેશ_row;
    linenr_T	prev_લીટી;
    varnumber_T	changedtick;

    જો (improved)
	exmode_active = EXMODE_VIM;
    નહીંતર
	exmode_active = EXMODE_NORMAL;
    State = MODE_NORMAL;
    may_trigger_modechanged();

    // When using ":global /pat/ visual" and then "Q" we return to continue
    // the :global command.
    જો (global_busy)
	પાછા;

    save_સંદેશ_scroll = સંદેશ_scroll;
    ++RedrawingDisabled;	    // don't redisplay the window
    ++no_wait_return;		    // don't wait for return
#ifdef FEAT_GUI
    // Ignore scrollbar and mouse events in Ex mode
    ++hold_gui_events;
#endif

    સંદેશ(_("Entering Ex mode.  Type \"visual\" to go to Normal mode."));
    જ્યારે (exmode_active)
    {
	// Check for a ":normal" command and no more characters left.
	જો (ex_normal_busy > 0 && typebuf.tb_len == 0)
	{
	    exmode_active = FALSE;
	    તોડો;
	}
	સંદેશ_scroll = TRUE;
	need_wait_return = FALSE;
	ex_pressedreturn = FALSE;
	ex_no_reprint = FALSE;
	changedtick = CHANGEDTICK(curbuf);
	prev_સંદેશ_row = સંદેશ_row;
	prev_લીટી = curwin->w_cursor.lnum;
	જો (improved)
	{
	    cmdline_row = સંદેશ_row;
	    do_cmdline(ખાલી, getexline, ખાલી, 0);
	}
	નહીંતર
	    do_cmdline(ખાલી, getexmodeline, ખાલી, DOCMD_NOWAIT);
	લીટીઓ_ડાબું = Rows - 1;

	જો ((prev_લીટી != curwin->w_cursor.lnum
		   || changedtick != CHANGEDTICK(curbuf)) && !ex_no_reprint)
	{
	    જો (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    નહીંતર
	    {
		જો (ex_pressedreturn)
		{
		    // go up one line, to overwrite the ":<CR>" line, so the
		    // output doesn't contain empty lines.
		    સંદેશ_row = prev_સંદેશ_row;
		    જો (prev_સંદેશ_row == Rows - 1)
			સંદેશ_row--;
		}
		સંદેશ_col = 0;
		print_લીટી_no_prefix(curwin->w_cursor.lnum, FALSE, FALSE);
		સંદેશ_clr_eos();
	    }
	}
	નહીંતર જો (ex_pressedreturn && !ex_no_reprint)	// must be at EOF
	{
	    જો (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    નહીંતર
		emsg(_(e_at_end_of_file));
	}
    }

#ifdef FEAT_GUI
    --hold_gui_events;
#endif
    જો (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    update_screen(UPD_CLEAR);
    need_wait_return = FALSE;
    સંદેશ_scroll = save_સંદેશ_scroll;
}

/*
 * Print the executed command for when 'verbose' is set.
 * When "lnum" is 0 only print the command.
 */
    સ્થિર શૂન્ય
સંદેશ_verbose_cmd(linenr_T lnum, char_u *cmd)
{
    ++no_wait_return;
    verbose_enter_scroll();

    જો (lnum == 0)
	smsg(_("Executing: %s"), cmd);
    નહીંતર
	smsg(_("line %ld: %s"), (long)lnum, cmd);
    જો (સંદેશ_silent == 0)
	સંદેશ_puts("\n");   // don't overwrite this

    verbose_leave_scroll();
    --no_wait_return;
}

/*
 * Execute a simple command line.  Used for translated commands like "*".
 */
    પૂર્ણાંક
do_cmdline_cmd(char_u *cmd)
{
    પાછા do_cmdline(cmd, ખાલી, ખાલી,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
}

/*
 * Execute the "+cmd" argument of "edit +cmd fname" and the like.
 * This allows for using a range without ":" in Vim9 script.
 */
    સ્થિર પૂર્ણાંક
do_cmd_argument(char_u *cmd)
{
    પાછા do_cmdline(cmd, ખાલી, ખાલી,
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
    પૂર્ણાંક
do_cmdline(
    char_u	*cmdline,
    char_u	*(*fgetline)(પૂર્ણાંક, શૂન્ય *, પૂર્ણાંક, getline_opt_T),
    શૂન્ય	*cookie,		// argument for fgetline()
    પૂર્ણાંક		flags)
{
    char_u	*next_cmdline;		// next cmd to execute
    char_u	*cmdline_copy = ખાલી;	// copy of cmd line
    પૂર્ણાંક		used_getline = FALSE;	// used "fgetline" to obtain command
    સ્થિર પૂર્ણાંક	recursive = 0;		// recursive depth
    પૂર્ણાંક		સંદેશ_didout_before_start = 0;
    પૂર્ણાંક		ગણતરી = 0;		// line number count
    પૂર્ણાંક		did_inc_RedrawingDisabled = FALSE;
    પૂર્ણાંક		retval = OK;
#ifdef FEAT_EVAL
    cstack_T	cstack;			// conditional stack
    garray_T	લીટીઓ_ga;		// keep lines for ":while"/":for"
    પૂર્ણાંક		current_લીટી = 0;	// active line in lines_ga
    પૂર્ણાંક		current_લીટી_before = 0;
    char_u	*fname = ખાલી;		// function or script name
    linenr_T	*breakpoint = ખાલી;	// ptr to breakpoint field in cookie
    પૂર્ણાંક		*dbg_tick = ખાલી;	// ptr to dbg_tick field in cookie
    સંરચના dbg_stuff debug_saved;	// saved things for debug mode
    પૂર્ણાંક		initial_trylevel;
    msglist_T	**saved_સંદેશ_list = ખાલી;
    msglist_T	*private_સંદેશ_list = ખાલી;

    // "fgetline" and "cookie" passed to do_one_cmd()
    char_u	*(*cmd_getline)(પૂર્ણાંક, શૂન્ય *, પૂર્ણાંક, getline_opt_T);
    શૂન્ય	*cmd_cookie;
    સંરચના loop_cookie cmd_loop_cookie;
    શૂન્ય	*real_cookie;
    પૂર્ણાંક		getline_is_func;
#નહીંતર
# define cmd_getline fgetline
# define cmd_cookie cookie
#endif
    સ્થિર પૂર્ણાંક	call_depth = 0;		// recursiveness
#ifdef FEAT_EVAL
    // For every pair of do_cmdline()/do_one_cmd() calls, use an extra memory
    // location for storing error messages to be converted to an exception.
    // This ensures that the do_errthrow() call in do_one_cmd() does not
    // combine the messages stored by an earlier invocation of do_one_cmd()
    // with the command name of the later one.  This would happen when
    // BufWritePost autocommands are executed after a write error.
    saved_સંદેશ_list = સંદેશ_list;
    સંદેશ_list = &private_સંદેશ_list;
#endif

    // It's possible to create an endless loop with ":execute", catch that
    // here.  The value of 200 allows nested function calls, ":source", etc.
    // Allow 200 or 'maxfuncdepth', whatever is larger.
    જો (call_depth >= 200
#ifdef FEAT_EVAL
	    && call_depth >= p_mfd
#endif
	    )
    {
	emsg(_(e_command_too_recursive));
#ifdef FEAT_EVAL
	// When converting to an exception, we do not include the command name
	// since this is not an error of the specific command.
	do_errthrow((cstack_T *)ખાલી, (char_u *)ખાલી);
	સંદેશ_list = saved_સંદેશ_list;
#endif
	પાછા FAIL;
    }
    ++call_depth;

#ifdef FEAT_EVAL
    CLEAR_FIELD(cstack);
    cstack.cs_idx = -1;
    ga_init2(&લીટીઓ_ga, કદ(wcmd_T), 10);

    real_cookie = getline_cookie(fgetline, cookie);

    // Inside a function use a higher nesting level.
    getline_is_func = getline_equal(fgetline, cookie, get_func_લીટી);
    જો (getline_is_func && ex_nesting_level == func_level(real_cookie))
	++ex_nesting_level;

    // Get the function or script name and the address where the next breakpoint
    // line and the debug tick for a function or script are stored.
    જો (getline_is_func)
    {
	fname = func_name(real_cookie);
	breakpoint = func_breakpoint(real_cookie);
	dbg_tick = func_dbg_tick(real_cookie);
    }
    નહીંતર જો (getline_equal(fgetline, cookie, getsourceline))
    {
	fname = SOURCING_NAME;
	breakpoint = source_breakpoint(real_cookie);
	dbg_tick = source_dbg_tick(real_cookie);
    }

    /*
     * Initialize "force_abort"  and "suppress_errthrow" at the top level.
     */
    જો (!recursive)
    {
	force_abort = FALSE;
	suppress_errthrow = FALSE;
    }

    /*
     * If requested, store and reset the global values controlling the
     * exception handling (used when debugging).  Otherwise clear it to avoid
     * a bogus compiler warning when the optimizer uses inline functions...
     */
    જો (flags & DOCMD_EXCRESET)
	save_dbg_stuff(&debug_saved);
    નહીંતર
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
    જો (!(flags & DOCMD_KEYTYPED)
			       && !getline_equal(fgetline, cookie, getexline))
	ચાવી_Typed = FALSE;

    /*
     * Continue executing command lines:
     * - when inside an ":if", ":while" or ":for"
     * - for multiple commands on one line, separated with '|'
     * - when repeating until there are no more lines (for ":source")
     */
    next_cmdline = cmdline;
    કરો
    {
#ifdef FEAT_EVAL
	getline_is_func = getline_equal(fgetline, cookie, get_func_લીટી);
#endif

	// stop skipping cmds for an error msg after all endif/while/for
	જો (next_cmdline == ખાલી
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
	જો (cstack.cs_looplevel > 0 && current_લીટી < લીટીઓ_ga.ga_len)
	{
	    // Each '|' separated command is stored separately in lines_ga, to
	    // be able to jump to it.  Don't use next_cmdline now.
	    VIM_CLEAR(cmdline_copy);

	    // Check if a function has returned or, unless it has an unclosed
	    // try conditional, aborted.
	    જો (getline_is_func)
	    {
# ifdef FEAT_PROFILE
		જો (do_profiling == PROF_YES)
		    func_લીટી_end(real_cookie);
# endif
		જો (func_has_ended(real_cookie))
		{
		    retval = FAIL;
		    તોડો;
		}
	    }
# ifdef FEAT_PROFILE
	    નહીંતર જો (do_profiling == PROF_YES
			    && getline_equal(fgetline, cookie, getsourceline))
		script_લીટી_end();
# endif

	    // Check if a sourced file hit a ":finish" command.
	    જો (source_finished(fgetline, cookie))
	    {
		retval = FAIL;
		તોડો;
	    }

	    // If breakpoints have been added/deleted need to check for it.
	    જો (breakpoint != ખાલી && dbg_tick != ખાલી
						   && *dbg_tick != debug_tick)
	    {
		*breakpoint = dbg_find_breakpoint(
				getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }

	    next_cmdline = ((wcmd_T *)(લીટીઓ_ga.ga_data))[current_લીટી].લીટી;
	    SOURCING_LNUM = ((wcmd_T *)(લીટીઓ_ga.ga_data))[current_લીટી].lnum;

	    // Did we encounter a breakpoint?
	    જો (breakpoint != ખાલી && *breakpoint != 0
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
	    જો (do_profiling == PROF_YES)
	    {
		જો (getline_is_func)
		    func_લીટી_start(real_cookie, SOURCING_LNUM);
		નહીંતર જો (getline_equal(fgetline, cookie, getsourceline))
		    script_લીટી_start();
	    }
# endif
	}
#endif

	// 2. If no line given, get an allocated line with fgetline().
	જો (next_cmdline == ખાલી)
	{
	    /*
	     * Need to set msg_didout for the first line after an ":if",
	     * otherwise the ":if" will be overwritten.
	     */
	    જો (ગણતરી == 1 && getline_equal(fgetline, cookie, getexline))
		સંદેશ_didout = TRUE;
	    જો (fgetline == ખાલી || (next_cmdline = fgetline(':', cookie,
#ifdef FEAT_EVAL
		    cstack.cs_idx < 0 ? 0 : (cstack.cs_idx + 1) * 2
#નહીંતર
		    0
#endif
		    , in_vim9script() ? GETLINE_CONCAT_CONTBAR
					       : GETLINE_CONCAT_CONT)) == ખાલી)
	    {
		// Don't call wait_return() for aborted command line.  The NULL
		// returned for the end of a sourced file or executed function
		// doesn't do this.
		જો (ચાવી_Typed && !(flags & DOCMD_REPEAT))
		    need_wait_return = FALSE;
		retval = FAIL;
		તોડો;
	    }
	    used_getline = TRUE;

	    /*
	     * Keep the first typed line.  Clear it when more lines are typed.
	     */
	    જો (flags & DOCMD_KEEPLINE)
	    {
		vim_free(repeat_cmdline);
		જો (ગણતરી == 0)
		    repeat_cmdline = vim_strsave(next_cmdline);
		નહીંતર
		    repeat_cmdline = ખાલી;
	    }
	}

	// 3. Make a copy of the command so we can mess with it.
	નહીંતર જો (cmdline_copy == ખાલી)
	{
	    next_cmdline = vim_strsave(next_cmdline);
	    જો (next_cmdline == ખાલી)
	    {
		emsg(_(e_out_of_memory));
		retval = FAIL;
		તોડો;
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
	જો ((cstack.cs_looplevel > 0 || has_loop_cmd(next_cmdline)))
	{
	    cmd_getline = get_loop_લીટી;
	    cmd_cookie = (શૂન્ય *)&cmd_loop_cookie;
	    cmd_loop_cookie.લીટીઓ_gap = &લીટીઓ_ga;
	    cmd_loop_cookie.current_લીટી = current_લીટી;
	    cmd_loop_cookie.lc_getline = fgetline;
	    cmd_loop_cookie.cookie = cookie;
	    cmd_loop_cookie.repeating = (current_લીટી < લીટીઓ_ga.ga_len);

	    // Save the current line when encountering it the first time.
	    જો (current_લીટી == લીટીઓ_ga.ga_len
		    && store_loop_લીટી(&લીટીઓ_ga, next_cmdline) == FAIL)
	    {
		retval = FAIL;
		તોડો;
	    }
	    current_લીટી_before = current_લીટી;
	}
	નહીંતર
	{
	    cmd_getline = fgetline;
	    cmd_cookie = cookie;
	}

	did_endif = FALSE;
#endif

	જો (ગણતરી++ == 0)
	{
	    /*
	     * All output from the commands is put below each other, without
	     * waiting for a return. Don't do this when executing commands
	     * from a script or when being called recursive (e.g. for ":e
	     * +command file").
	     */
	    જો (!(flags & DOCMD_NOWAIT) && !recursive)
	    {
		સંદેશ_didout_before_start = સંદેશ_didout;
		સંદેશ_didany = FALSE; // no output yet
		સંદેશ_start();
		સંદેશ_scroll = TRUE;  // put messages below each other
		++no_wait_return;   // don't wait for return until finished
		++RedrawingDisabled;
		did_inc_RedrawingDisabled = TRUE;
	    }
	}

	જો ((p_verbose >= 15 && SOURCING_NAME != ખાલી) || p_verbose >= 16)
	    સંદેશ_verbose_cmd(SOURCING_LNUM, cmdline_copy);

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
	જો (cmd_cookie == (શૂન્ય *)&cmd_loop_cookie)
	    // Use "current_line" from "cmd_loop_cookie", it may have been
	    // incremented when defining a function.
	    current_લીટી = cmd_loop_cookie.current_લીટી;
#endif

	જો (next_cmdline == ખાલી)
	{
	    VIM_CLEAR(cmdline_copy);

	    /*
	     * If the command was typed, remember it for the ':' register.
	     * Do this AFTER executing the command to make :@: work.
	     */
	    જો (getline_equal(fgetline, cookie, getexline)
						  && new_last_cmdline != ખાલી)
	    {
		vim_free(last_cmdline);
		last_cmdline = new_last_cmdline;
		new_last_cmdline = ખાલી;
	    }
	}
	નહીંતર
	{
	    // need to copy the command after the '|' to cmdline_copy, for the
	    // next do_one_cmd()
	    STRMOVE(cmdline_copy, next_cmdline);
	    next_cmdline = cmdline_copy;
	}


#ifdef FEAT_EVAL
	// reset did_emsg for a function that is not aborted by an error
	જો (did_emsg && !force_abort
		&& getline_equal(fgetline, cookie, get_func_લીટી)
					      && !func_has_abort(real_cookie))
	{
	    // did_emsg_cumul is not set here
	    did_emsg = FALSE;
	}

	જો (cstack.cs_looplevel > 0)
	{
	    ++current_લીટી;

	    /*
	     * An ":endwhile", ":endfor" and ":continue" is handled here.
	     * If we were executing commands, jump back to the ":while" or
	     * ":for".
	     * If we were not executing commands, decrement cs_looplevel.
	     */
	    જો (cstack.cs_lflags & (CSL_HAD_CONT | CSL_HAD_ENDLOOP))
	    {
		cstack.cs_lflags &= ~(CSL_HAD_CONT | CSL_HAD_ENDLOOP);

		// Jump back to the matching ":while" or ":for".  Be careful
		// not to use a cs_line[] from an entry that isn't a ":while"
		// or ":for": It would make "current_line" invalid and can
		// cause a crash.
		જો (!did_emsg && !got_int && !did_throw
			&& cstack.cs_idx >= 0
			&& (cstack.cs_flags[cstack.cs_idx]
						      & (CSF_WHILE | CSF_FOR))
			&& cstack.cs_લીટી[cstack.cs_idx] >= 0
			&& (cstack.cs_flags[cstack.cs_idx] & CSF_ACTIVE))
		{
		    current_લીટી = cstack.cs_લીટી[cstack.cs_idx];
						// remember we jumped there
		    cstack.cs_lflags |= CSL_HAD_LOOP;
		    લીટી_breakcheck();		// check if CTRL-C typed

		    // Check for the next breakpoint at or after the ":while"
		    // or ":for".
		    જો (breakpoint != ખાલી && લીટીઓ_ga.ga_len > current_લીટી)
		    {
			*breakpoint = dbg_find_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
									fname,
			   ((wcmd_T *)લીટીઓ_ga.ga_data)[current_લીટી].lnum-1);
			*dbg_tick = debug_tick;
		    }
		}
		નહીંતર
		{
		    // can only get here with ":endwhile" or ":endfor"
		    જો (cstack.cs_idx >= 0)
			rewind_conditionals(&cstack, cstack.cs_idx - 1,
				   CSF_WHILE | CSF_FOR, &cstack.cs_looplevel);
		}
	    }

	    /*
	     * For a ":while" or ":for" we need to remember the line number.
	     */
	    નહીંતર જો (cstack.cs_lflags & CSL_HAD_LOOP)
	    {
		cstack.cs_lflags &= ~CSL_HAD_LOOP;
		cstack.cs_લીટી[cstack.cs_idx] = current_લીટી_before;
	    }
	}

	// Check for the next breakpoint after a watchexpression
	જો (breakpoint != ખાલી && has_watchexpr())
	{
	    *breakpoint = dbg_find_breakpoint(FALSE, fname, SOURCING_LNUM);
	    *dbg_tick = debug_tick;
	}

	/*
	 * When not inside any ":while" loop, clear remembered lines.
	 */
	જો (cstack.cs_looplevel == 0)
	{
	    જો (લીટીઓ_ga.ga_len > 0)
	    {
		SOURCING_LNUM =
		       ((wcmd_T *)લીટીઓ_ga.ga_data)[લીટીઓ_ga.ga_len - 1].lnum;
		free_cmdlines(&લીટીઓ_ga);
	    }
	    current_લીટી = 0;
	}

	/*
	 * A ":finally" makes did_emsg, got_int, and did_throw pending for
	 * being restored at the ":endtry".  Reset them here and set the
	 * ACTIVE and FINALLY flags, so that the finally clause gets executed.
	 * This includes the case where a missing ":endif", ":endwhile" or
	 * ":endfor" was detected by the ":finally" itself.
	 */
	જો (cstack.cs_lflags & CSL_HAD_FINA)
	{
	    cstack.cs_lflags &= ~CSL_HAD_FINA;
	    report_make_pending(cstack.cs_pending[cstack.cs_idx]
		    & (CSTP_ERROR | CSTP_INTERRUPT | CSTP_THROW),
		    did_throw ? (શૂન્ય *)current_exception : ખાલી);
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
	જો (trylevel == 0 && !did_emsg && !got_int && !did_throw)
	    force_abort = FALSE;

	// Convert an interrupt to an exception if appropriate.
	(શૂન્ય)do_intthrow(&cstack);
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
    જ્યારે (!((got_int
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
	    && (next_cmdline != ખાલી
#ifdef FEAT_EVAL
			|| cstack.cs_idx >= 0
#endif
			|| (flags & DOCMD_REPEAT)));

    vim_free(cmdline_copy);
    did_emsg_syntax = FALSE;
#ifdef FEAT_EVAL
    free_cmdlines(&લીટીઓ_ga);
    ga_clear(&લીટીઓ_ga);

    જો (cstack.cs_idx >= 0)
    {
	/*
	 * If a sourced file or executed function ran to its end, report the
	 * unclosed conditional.
	 * In Vim9 script do not give a second error, executing aborts after
	 * the first one.
	 */
	જો (!got_int && !did_throw && !aborting()
		&& !(did_emsg && in_vim9script())
		&& ((getline_equal(fgetline, cookie, getsourceline)
			&& !source_finished(fgetline, cookie))
		    || (getline_equal(fgetline, cookie, get_func_લીટી)
					    && !func_has_ended(real_cookie))))
	{
	    જો (cstack.cs_flags[cstack.cs_idx] & CSF_TRY)
		emsg(_(e_missing_endtry));
	    નહીંતર જો (cstack.cs_flags[cstack.cs_idx] & CSF_WHILE)
		emsg(_(e_missing_endwhile));
	    નહીંતર જો (cstack.cs_flags[cstack.cs_idx] & CSF_FOR)
		emsg(_(e_missing_endfor));
	    નહીંતર
		emsg(_(e_missing_endif));
	}

	/*
	 * Reset "trylevel" in case of a ":finish" or ":return" or a missing
	 * ":endtry" in a sourced file or executed function.  If the try
	 * conditional is in its finally clause, ignore anything pending.
	 * If it is in a catch clause, finish the caught exception.
	 * Also cleanup any "cs_forinfo" structures.
	 */
	કરો
	{
	    પૂર્ણાંક idx = cleanup_conditionals(&cstack, 0, TRUE);

	    જો (idx >= 0)
		--idx;	    // remove try block not in its finally clause
	    rewind_conditionals(&cstack, idx, CSF_WHILE | CSF_FOR,
							&cstack.cs_looplevel);
	}
	જ્યારે (cstack.cs_idx >= 0);
	trylevel = initial_trylevel;
    }

    // If a missing ":endtry", ":endwhile", ":endfor", or ":endif" or a memory
    // lack was reported above and the error message is to be converted to an
    // exception, do this now after rewinding the cstack.
    do_errthrow(&cstack, getline_equal(fgetline, cookie, get_func_લીટી)
				  ? (char_u *)"endfunction" : (char_u *)ખાલી);

    જો (trylevel == 0)
    {
	// Just in case did_throw got set but current_exception wasn't.
	જો (current_exception == ખાલી)
	    did_throw = FALSE;

	/*
	 * When an exception is being thrown out of the outermost try
	 * conditional, discard the uncaught exception, disable the conversion
	 * of interrupts or errors to exceptions, and ensure that no more
	 * commands are executed.
	 */
	જો (did_throw)
	    handle_did_throw();

	/*
	 * On an interrupt or an aborting error not converted to an exception,
	 * disable the conversion of errors to exceptions.  (Interrupts are not
	 * converted anymore, here.) This enables also the interrupt message
	 * when force_abort is set and did_emsg unset in case of an interrupt
	 * from a finally clause after an error.
	 */
	નહીંતર જો (got_int || (did_emsg && force_abort))
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
    જો (did_throw)
	need_rethrow = TRUE;
    જો ((getline_equal(fgetline, cookie, getsourceline)
		&& ex_nesting_level > source_level(real_cookie))
	    || (getline_equal(fgetline, cookie, get_func_લીટી)
		&& ex_nesting_level > func_level(real_cookie) + 1))
    {
	જો (!did_throw)
	    check_cstack = TRUE;
    }
    નહીંતર
    {
	// When leaving a function, reduce nesting level.
	જો (getline_equal(fgetline, cookie, get_func_લીટી))
	    --ex_nesting_level;
	/*
	 * Go to debug mode when returning from a function in which we are
	 * single-stepping.
	 */
	જો ((getline_equal(fgetline, cookie, getsourceline)
		    || getline_equal(fgetline, cookie, get_func_લીટી))
		&& ex_nesting_level + 1 <= debug_break_level)
	    do_debug(getline_equal(fgetline, cookie, getsourceline)
		    ? (char_u *)_("End of sourced file")
		    : (char_u *)_("End of function"));
    }

    /*
     * Restore the exception environment (done after returning from the
     * debugger).
     */
    જો (flags & DOCMD_EXCRESET)
	restore_dbg_stuff(&debug_saved);

    સંદેશ_list = saved_સંદેશ_list;

    // Cleanup if "cs_emsg_silent_list" remains.
    જો (cstack.cs_emsg_silent_list != ખાલી)
    {
	eslist_T *elem, *temp;

	માટે (elem = cstack.cs_emsg_silent_list; elem != ખાલી; elem = temp)
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
    જો (did_inc_RedrawingDisabled)
    {
	જો (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	--no_wait_return;
	સંદેશ_scroll = FALSE;

	/*
	 * When just finished an ":if"-":else" which was typed, no need to
	 * wait for hit-return.  Also for an error situation.
	 */
	જો (retval == FAIL
#ifdef FEAT_EVAL
		|| (did_endif && ચાવી_Typed && !did_emsg)
#endif
					    )
	{
	    need_wait_return = FALSE;
	    સંદેશ_didany = FALSE;		// don't wait when restarting edit
	}
	નહીંતર જો (need_wait_return)
	{
	    /*
	     * The msg_start() above clears msg_didout. The wait_return() we do
	     * here should not overwrite the command that may be shown before
	     * doing that.
	     */
	    સંદેશ_didout |= સંદેશ_didout_before_start;
	    wait_return(FALSE);
	}
    }

#ifdef FEAT_EVAL
    did_endif = FALSE;  // in case do_cmdline used recursively
#નહીંતર
    /*
     * Reset if_level, in case a sourced script file contains more ":if" than
     * ":endif" (could be ":if x | foo | endif").
     */
    if_level = 0;
#endif

    --call_depth;
    પાછા retval;
}

#જો defined(FEAT_EVAL)
/*
 * Handle when "did_throw" is set after executing commands.
 */
    શૂન્ય
handle_did_throw(શૂન્ય)
{
    અક્ષર	*p = ખાલી;
    msglist_T	*messages = ખાલી;
    ESTACK_CHECK_DECLARATION;

    /*
     * If the uncaught exception is a user exception, report it as an
     * error.  If it is an error exception, display the saved error
     * message now.  For an interrupt exception, do nothing; the
     * interrupt message is given elsewhere.
     */
    પસંદગી (current_exception->type)
    {
	સ્થિતિ ET_USER:
	    vim_snprintf((અક્ષર *)IObuff, IOSIZE,
		    _(e_exception_not_caught_str),
		    current_exception->કિંમત);
	    p = (અક્ષર *)vim_strsave(IObuff);
	    તોડો;
	સ્થિતિ ET_ERROR:
	    messages = current_exception->messages;
	    current_exception->messages = ખાલી;
	    તોડો;
	સ્થિતિ ET_INTERRUPT:
	    તોડો;
    }

    estack_ધકેલો(ETYPE_EXCEPT, current_exception->throw_name,
					current_exception->throw_lnum);
    ESTACK_CHECK_SETUP;
    current_exception->throw_name = ખાલી;

    discard_current_exception();	// uses IObuff if 'verbose'

    // If "silent!" is active the uncaught exception is not fatal.
    જો (emsg_silent == 0)
    {
	suppress_errthrow = TRUE;
	force_abort = TRUE;
    }

    જો (messages != ખાલી)
    {
	કરો
	{
	    msglist_T	*next = messages->next;
	    પૂર્ણાંક		save_compiling = estack_compiling;

	    estack_compiling = messages->સંદેશ_compiling;
	    emsg(messages->સંદેશ);
	    vim_free(messages->સંદેશ);
	    vim_free(messages->sfile);
	    vim_free(messages);
	    messages = next;
	    estack_compiling = save_compiling;
	}
	જ્યારે (messages != ખાલી);
    }
    નહીંતર જો (p != ખાલી)
    {
	emsg(p);
	vim_free(p);
    }
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_કાઢો();
}

/*
 * Obtain a line when inside a ":while" or ":for" loop.
 */
    સ્થિર char_u *
get_loop_લીટી(પૂર્ણાંક c, શૂન્ય *cookie, પૂર્ણાંક indent, getline_opt_T options)
{
    સંરચના loop_cookie	*cp = (સંરચના loop_cookie *)cookie;
    wcmd_T		*wp;
    char_u		*લીટી;

    જો (cp->current_લીટી + 1 >= cp->લીટીઓ_gap->ga_len)
    {
	જો (cp->repeating)
	    પાછા ખાલી;	// trying to read past ":endwhile"/":endfor"

	// First time inside the ":while"/":for": get line normally.
	જો (cp->lc_getline == ખાલી)
	    લીટી = getcmdline(c, 0L, indent, 0);
	નહીંતર
	    લીટી = cp->lc_getline(c, cp->cookie, indent, options);
	જો (લીટી != ખાલી && store_loop_લીટી(cp->લીટીઓ_gap, લીટી) == OK)
	    ++cp->current_લીટી;

	પાછા લીટી;
    }

    ચાવી_Typed = FALSE;
    ++cp->current_લીટી;
    wp = (wcmd_T *)(cp->લીટીઓ_gap->ga_data) + cp->current_લીટી;
    SOURCING_LNUM = wp->lnum;
    પાછા vim_strsave(wp->લીટી);
}

/*
 * Store a line in "gap" so that a ":while" loop can execute it again.
 */
    સ્થિર પૂર્ણાંક
store_loop_લીટી(garray_T *gap, char_u *લીટી)
{
    જો (ga_grow(gap, 1) == FAIL)
	પાછા FAIL;
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].લીટી = vim_strsave(લીટી);
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].lnum = SOURCING_LNUM;
    ++gap->ga_len;
    પાછા OK;
}

/*
 * Free the lines stored for a ":while" or ":for" loop.
 */
    સ્થિર શૂન્ય
free_cmdlines(garray_T *gap)
{
    જ્યારે (gap->ga_len > 0)
    {
	vim_free(((wcmd_T *)(gap->ga_data))[gap->ga_len - 1].લીટી);
	--gap->ga_len;
    }
}
#endif

/*
 * If "fgetline" is get_loop_line(), return TRUE if the getline it uses equals
 * "func".  * Otherwise return TRUE when "fgetline" equals "func".
 */
    પૂર્ણાંક
getline_equal(
    char_u	*(*fgetline)(પૂર્ણાંક, શૂન્ય *, પૂર્ણાંક, getline_opt_T),
    શૂન્ય	*cookie UNUSED,		// argument for fgetline()
    char_u	*(*func)(પૂર્ણાંક, શૂન્ય *, પૂર્ણાંક, getline_opt_T))
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(પૂર્ણાંક, શૂન્ય *, પૂર્ણાંક, getline_opt_T);
    સંરચના loop_cookie *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // function that's originally used to obtain the lines.  This may be
    // nested several levels.
    gp = fgetline;
    cp = (સંરચના loop_cookie *)cookie;
    જ્યારે (gp == get_loop_લીટી)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    પાછા gp == func;
#નહીંતર
    પાછા fgetline == func;
#endif
}

/*
 * If "fgetline" is get_loop_line(), return the cookie used by the original
 * getline function.  Otherwise return "cookie".
 */
    શૂન્ય *
getline_cookie(
    char_u	*(*fgetline)(પૂર્ણાંક, શૂન્ય *, પૂર્ણાંક, getline_opt_T) UNUSED,
    શૂન્ય	*cookie)		// argument for fgetline()
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(પૂર્ણાંક, શૂન્ય *, પૂર્ણાંક, getline_opt_T);
    સંરચના loop_cookie  *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (સંરચના loop_cookie *)cookie;
    જ્યારે (gp == get_loop_લીટી)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    પાછા cp;
#નહીંતર
    પાછા cookie;
#endif
}

#જો defined(FEAT_EVAL)
/*
 * Get the next line source line without advancing.
 */
    char_u *
getline_peek(
    char_u	*(*fgetline)(પૂર્ણાંક, શૂન્ય *, પૂર્ણાંક, getline_opt_T) UNUSED,
    શૂન્ય	*cookie)		// argument for fgetline()
{
    char_u		*(*gp)(પૂર્ણાંક, શૂન્ય *, પૂર્ણાંક, getline_opt_T);
    સંરચના loop_cookie  *cp;
    wcmd_T		*wp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (સંરચના loop_cookie *)cookie;
    જ્યારે (gp == get_loop_લીટી)
    {
	જો (cp->current_લીટી + 1 < cp->લીટીઓ_gap->ga_len)
	{
	    // executing lines a second time, use the stored copy
	    wp = (wcmd_T *)(cp->લીટીઓ_gap->ga_data) + cp->current_લીટી + 1;
	    પાછા wp->લીટી;
	}
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    જો (gp == getsourceline)
	પાછા source_nextline(cp);
    પાછા ખાલી;
}
#endif


/*
 * Helper function to apply an offset for buffer commands, i.e. ":bdelete",
 * ":bwipeout", etc.
 * Returns the buffer number.
 */
    સ્થિર પૂર્ણાંક
compute_buffer_local_ગણતરી(પૂર્ણાંક addr_type, પૂર્ણાંક lnum, પૂર્ણાંક offset)
{
    buf_T   *buf;
    buf_T   *nextbuf;
    પૂર્ણાંક     ગણતરી = offset;

    buf = firstbuf;
    જ્યારે (buf->b_next != ખાલી && buf->b_fnum < lnum)
	buf = buf->b_next;
    જ્યારે (ગણતરી != 0)
    {
	ગણતરી += (offset < 0) ? 1 : -1;
	nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
	જો (nextbuf == ખાલી)
	    તોડો;
	buf = nextbuf;
	જો (addr_type == ADDR_LOADED_BUFFERS)
	    // skip over unloaded buffers
	    જ્યારે (buf->b_ml.ml_mfp == ખાલી)
	    {
		nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
		જો (nextbuf == ખાલી)
		    તોડો;
		buf = nextbuf;
	    }
    }
    // we might have gone too far, last buffer is not loadedd
    જો (addr_type == ADDR_LOADED_BUFFERS)
	જ્યારે (buf->b_ml.ml_mfp == ખાલી)
	{
	    nextbuf = (offset >= 0) ? buf->b_prev : buf->b_next;
	    જો (nextbuf == ખાલી)
		તોડો;
	    buf = nextbuf;
	}
    પાછા buf->b_fnum;
}

/*
 * Return the window number of "win".
 * When "win" is NULL return the number of windows.
 */
    સ્થિર પૂર્ણાંક
current_win_nr(win_T *win)
{
    win_T	*wp;
    પૂર્ણાંક		nr = 0;

    FOR_ALL_WINDOWS(wp)
    {
	++nr;
	જો (wp == win)
	    તોડો;
    }
    પાછા nr;
}

    સ્થિર પૂર્ણાંક
current_tab_nr(tabpage_T *tab)
{
    tabpage_T	*tp;
    પૂર્ણાંક		nr = 0;

    FOR_ALL_TABPAGES(tp)
    {
	++nr;
	જો (tp == tab)
	    તોડો;
    }
    પાછા nr;
}

    સ્થિર પૂર્ણાંક
comment_start(char_u *p, પૂર્ણાંક starts_with_colon UNUSED)
{
    જો (in_vim9script())
	પાછા p[0] == '#' && !starts_with_colon;
    પાછા *p == '"';
}

#define CURRENT_WIN_NR current_win_nr(curwin)
#define LAST_WIN_NR current_win_nr(ખાલી)
#define CURRENT_TAB_NR current_tab_nr(curtab)
#define LAST_TAB_NR current_tab_nr(ખાલી)

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
    સ્થિર char_u *
do_one_cmd(
    char_u	**cmdlinep,
    પૂર્ણાંક		flags,
#ifdef FEAT_EVAL
    cstack_T	*cstack,
#endif
    char_u	*(*fgetline)(પૂર્ણાંક, શૂન્ય *, પૂર્ણાંક, getline_opt_T),
    શૂન્ય	*cookie)		// argument for fgetline()
{
    char_u	*p;
    linenr_T	lnum;
    long	n;
    અક્ષર	*errormsg = ખાલી;	// error message
    char_u	*after_modifier = ખાલી;
    exarg_T	ea;			// Ex command arguments
    cmdmod_T	save_cmdmod;
    પૂર્ણાંક		save_reg_executing = reg_executing;
    પૂર્ણાંક		save_pending_end_reg_executing = pending_end_reg_executing;
    પૂર્ણાંક		ni;			// set when Not Implemented
    char_u	*cmd;
    પૂર્ણાંક		starts_with_colon = FALSE;
    પૂર્ણાંક		may_have_range;
#ifdef FEAT_EVAL
    પૂર્ણાંક		did_set_પદાવલિ_લીટી = FALSE;
#endif
    પૂર્ણાંક		sourcing = flags & DOCMD_VERBOSE;
    પૂર્ણાંક		did_append_cmd = FALSE;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
#ifdef FEAT_EVAL
    ++ex_nesting_level;
#endif

    // When the last file has not been edited :q has to be typed twice.
    જો (quitmore
#ifdef FEAT_EVAL
	    // avoid that a function call in 'statusline' does this
	    && !getline_equal(fgetline, cookie, get_func_લીટી)
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
    જો ((*cmdlinep)[0] == '#' && (*cmdlinep)[1] == '!')
	જાઓ doend;

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
    જો (parse_command_modifiers(&ea, &errormsg, &cmdmod, FALSE) == FAIL)
	જાઓ doend;
    apply_cmdmod(&cmdmod);
    after_modifier = ea.cmd;

#ifdef FEAT_EVAL
    ea.skip = did_emsg || got_int || did_throw || (cstack->cs_idx >= 0
			 && !(cstack->cs_flags[cstack->cs_idx] & CSF_ACTIVE));
#નહીંતર
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
    પૂર્ણાંક vim9script = in_vim9script();
    જો (vim9script && (flags & DOCMD_RANGEOK) == 0)
    {
	may_have_range = FALSE;
	માટે (p = ea.cmd; p >= *cmdlinep; --p)
	{
	    જો (*p == ':')
		may_have_range = TRUE;
	    જો (p < ea.cmd && !VIM_ISWHITE(*p))
		તોડો;
	}
    }
    નહીંતર
	may_have_range = TRUE;
    જો (may_have_range)
	ea.cmd = skip_range(ea.cmd, TRUE, ખાલી);

#ifdef FEAT_EVAL
    // Handle ":export" - it functions almost like a command modifier.
    // ":export var Name: type"
    // ":export def Name(..."
    // etc.
    જો (vim9script && checkforcmd_noparen(&ea.cmd, "export", 6))
	is_export = TRUE;
#endif

    જો (vim9script && !may_have_range)
    {
	જો (ea.cmd == cmd + 1 && *cmd == '$')
	    // should be "$VAR = val"
	    --ea.cmd;
#ifdef FEAT_EVAL
	p = find_ex_command(&ea, ખાલી, lookup_scriptitem, ખાલી);
#નહીંતર
	p = find_ex_command(&ea, ખાલી, ખાલી, ખાલી);
#endif
	જો (ea.cmdidx == CMD_SIZE)
	{
	    char_u *ar = skip_range(ea.cmd, TRUE, ખાલી);

	    // If a ':' before the range is missing, give a clearer error
	    // message.
	    જો (ar > ea.cmd && !ea.skip)
	    {
		semsg(_(e_colon_required_before_range_str), ea.cmd);
		જાઓ doend;
	    }
	}
    }
    નહીંતર
	p = find_ex_command(&ea, ખાલી, ખાલી, ખાલી);

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    // Count this line for profiling if skip is TRUE.
    જો (do_profiling == PROF_YES
	    && (!ea.skip || cstack->cs_idx == 0 || (cstack->cs_idx > 0
		     && (cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE))))
    {
	પૂર્ણાંક skip = did_emsg || got_int || did_throw;

	જો (ea.cmdidx == CMD_catch)
	    skip = !skip && !(cstack->cs_idx >= 0
			  && (cstack->cs_flags[cstack->cs_idx] & CSF_THROWN)
			  && !(cstack->cs_flags[cstack->cs_idx] & CSF_CAUGHT));
	નહીંતર જો (ea.cmdidx == CMD_else || ea.cmdidx == CMD_elseif)
	    skip = skip || !(cstack->cs_idx >= 0
			  && !(cstack->cs_flags[cstack->cs_idx]
						  & (CSF_ACTIVE | CSF_TRUE)));
	નહીંતર જો (ea.cmdidx == CMD_finally)
	    skip = FALSE;
	નહીંતર જો (ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endtry
		&& ea.cmdidx != CMD_endwhile)
	    skip = ea.skip;

	જો (!skip)
	{
	    જો (getline_equal(fgetline, cookie, get_func_લીટી))
		func_લીટી_exec(getline_cookie(fgetline, cookie));
	    નહીંતર જો (getline_equal(fgetline, cookie, getsourceline))
		script_લીટી_exec();
	}
    }
# endif
#endif

    ea.cmd = cmd;

#ifdef FEAT_EVAL
    // May go to debug mode.  If this happens and the ">quit" debug command is
    // used, throw an interrupt exception and skip the next command.
    dbg_check_breakpoint(&ea);
    જો (!ea.skip && got_int)
    {
	ea.skip = TRUE;
	(શૂન્ય)do_intthrow(cstack);
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
    જો (!IS_USER_CMDIDX(ea.cmdidx))
    {
	જો (ea.cmdidx != CMD_SIZE)
	    ea.addr_type = cmdnames[(પૂર્ણાંક)ea.cmdidx].cmd_addr_type;
	નહીંતર
	    ea.addr_type = ADDR_લીટીઓ;

	// :wincmd range depends on the argument.
	જો (ea.cmdidx == CMD_wincmd && p != ખાલી)
	    get_wincmd_addr_type(skipwhite(p), &ea);
#ifdef FEAT_QUICKFIX
	// :.cc in quickfix window uses line number
	જો ((ea.cmdidx == CMD_cc || ea.cmdidx == CMD_ll) && bt_quickfix(curbuf))
	    ea.addr_type = ADDR_OTHER;
#endif
    }

    જો (!may_have_range)
	ea.line1 = ea.line2 = default_address(&ea);
    નહીંતર જો (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
	જાઓ doend;

/*
 * 5. Parse the command.
 */

    /*
     * Skip ':' and any white space
     */
    ea.cmd = skipwhite(ea.cmd);
    જ્યારે (*ea.cmd == ':')
	ea.cmd = skipwhite(ea.cmd + 1);

    /*
     * If we got a line, but no command, then go to the line.
     * If we find a '|' or '\n' we set ea.nextcmd.
     */
    જો (*ea.cmd == NUL || comment_start(ea.cmd, starts_with_colon)
			       || (ea.nextcmd = check_nextcmd(ea.cmd)) != ખાલી)
    {
	/*
	 * strange vi behaviour:
	 * ":3"		jumps to line 3
	 * ":3|..."	prints line 3  (not in Vim9 script)
	 * ":|"		prints current line  (not in Vim9 script)
	 */
	જો (ea.skip)	    // skip this if inside :if
	    જાઓ doend;
	errormsg = ex_range_without_command(&ea);
	જાઓ doend;
    }

    // If this looks like an undefined user command and there are CmdUndefined
    // autocommands defined, trigger the matching autocommands.
    જો (p != ખાલી && ea.cmdidx == CMD_SIZE && !ea.skip
	    && ASCII_ISUPPER(*ea.cmd)
	    && has_cmdundefined())
    {
	પૂર્ણાંક ret;

	p = ea.cmd;
	જ્યારે (ASCII_ISALNUM(*p))
	    ++p;
	p = vim_strnsave(ea.cmd, p - ea.cmd);
	ret = apply_autocmds(EVENT_CMDUNDEFINED, p, p, TRUE, ખાલી);
	vim_free(p);
	// If the autocommands did something and didn't cause an error, try
	// finding the command again.
	p = (ret
#ifdef FEAT_EVAL
		&& !aborting()
#endif
		) ? find_ex_command(&ea, ખાલી, ખાલી, ખાલી) : ea.cmd;
    }

    જો (p == ખાલી)
    {
	જો (!ea.skip)
	    errormsg = _(e_ambiguous_use_of_user_defined_command);
	જાઓ doend;
    }
    // Check for wrong commands.
    જો (*p == '!' && ea.cmd[1] == 0151 && ea.cmd[0] == 78
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	errormsg = uc_fun_cmd();
	જાઓ doend;
    }

    જો (ea.cmdidx == CMD_SIZE)
    {
	જો (!ea.skip)
	{
	    STRCPY(IObuff, _(e_not_an_editor_command));
	    જો (!sourcing)
	    {
		// If the modifier was parsed OK the error must be in the
		// following command
		જો (after_modifier != ખાલી)
		    append_command(after_modifier);
		નહીંતર
		    append_command(*cmdlinep);
		did_append_cmd = TRUE;
	    }
	    errormsg = (અક્ષર *)IObuff;
	    did_emsg_syntax = TRUE;
	}
	જાઓ doend;
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
    જો (ea.cmdidx == CMD_if)
	++if_level;
    જો (if_level)
    {
	જો (ea.cmdidx == CMD_endif)
	    --if_level;
	જાઓ doend;
    }

#endif

    // forced commands
    જો (*p == '!' && ea.cmdidx != CMD_substitute
	    && ea.cmdidx != CMD_smagic && ea.cmdidx != CMD_snomagic)
    {
	++p;
	ea.forceit = TRUE;
    }
    નહીંતર
	ea.forceit = FALSE;

/*
 * 6. Parse arguments.  Then check for errors.
 */
    જો (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = (long)cmdnames[(પૂર્ણાંક)ea.cmdidx].cmd_argt;

    જો (!ea.skip)
    {
#ifdef HAVE_SANDBOX
	જો (sandbox != 0 && !(ea.argt & EX_SBOXOK))
	{
	    // Command not allowed in sandbox.
	    errormsg = _(e_not_allowed_in_sandbox);
	    જાઓ doend;
	}
#endif
	જો (restricted != 0 && (ea.argt & EX_RESTRICT))
	{
	    errormsg = _(e_command_not_allowed_in_rvim);
	    જાઓ doend;
	}
	જો (!curbuf->b_p_ma && (ea.argt & EX_MODIFY))
	{
	    // Command not allowed in non-'modifiable' buffer
	    errormsg = _(e_cannot_make_changes_modifiable_is_off);
	    જાઓ doend;
	}

	જો (!IS_USER_CMDIDX(ea.cmdidx))
	{
	    જો (cmdwin_type != 0 && !(ea.argt & EX_CMDWIN))
	    {
		// Command not allowed in the command line window
		errormsg = _(e_invalid_in_cmdline_window);
		જાઓ doend;
	    }
	    જો (text_locked() && !(ea.argt & EX_LOCK_OK))
	    {
		// Command not allowed when text is locked
		errormsg = _(get_text_locked_સંદેશ());
		જાઓ doend;
	    }
	}

	// Disallow editing another buffer when "curbuf_lock" is set.
	// Do allow ":checktime" (it is postponed).
	// Do allow ":edit" (check for an argument later).
	// Do allow ":file" with no arguments (check for an argument later).
	જો (!(ea.argt & (EX_CMDWIN | EX_LOCK_OK))
		&& ea.cmdidx != CMD_checktime
		&& ea.cmdidx != CMD_edit
		&& ea.cmdidx != CMD_file
		&& !IS_USER_CMDIDX(ea.cmdidx)
		&& curbuf_locked())
	    જાઓ doend;

	જો (!ni && !(ea.argt & EX_RANGE) && ea.addr_ગણતરી > 0)
	{
	    errormsg = _(e_no_range_allowed);
	    જાઓ doend;
	}
    }

    જો (!ni && !(ea.argt & EX_BANG) && ea.forceit)
    {
	errormsg = _(e_no_bang_allowed);
	જાઓ doend;
    }

    /*
     * Don't complain about the range if it is not used
     * (could happen if line_count is accidentally set to 0).
     */
    જો (!ea.skip && !ni && (ea.argt & EX_RANGE))
    {
	/*
	 * If the range is backwards, ask for confirmation and, if given, swap
	 * ea.line1 & ea.line2 so it's forwards again.
	 * When global command is busy, don't ask, will fail below.
	 */
	જો (!global_busy && ea.line1 > ea.line2)
	{
	    જો (સંદેશ_silent == 0)
	    {
		જો (sourcing || exmode_active)
		{
		    errormsg = _(e_backwards_range_given);
		    જાઓ doend;
		}
		જો (ask_yesno((char_u *)
			_("Backwards range given, OK to swap"), FALSE) != 'y')
		    જાઓ doend;
	    }
	    lnum = ea.line1;
	    ea.line1 = ea.line2;
	    ea.line2 = lnum;
	}
	જો ((errormsg = invalid_range(&ea)) != ખાલી)
	    જાઓ doend;
    }

    જો ((ea.addr_type == ADDR_OTHER) && ea.addr_ગણતરી == 0)
	// default is 1, not cursor
	ea.line2 = 1;

    correct_range(&ea);

#ifdef FEAT_FOLDING
    જો (((ea.argt & EX_WHOLEFOLD) || ea.addr_ગણતરી >= 2) && !global_busy
	    && ea.addr_type == ADDR_લીટીઓ)
    {
	// Put the first line at the start of a closed fold, put the last line
	// at the end of a closed fold.
	(શૂન્ય)hasFolding(ea.line1, &ea.line1, ખાલી);
	(શૂન્ય)hasFolding(ea.line2, ખાલી, &ea.line2);
    }
#endif

#ifdef FEAT_QUICKFIX
    /*
     * For the ":make" and ":grep" commands we insert the 'makeprg'/'grepprg'
     * option here, so things like % get expanded.
     */
    p = replace_makeprg(&ea, p, cmdlinep);
    જો (p == ખાલી)
	જાઓ doend;
#endif

    /*
     * Skip to start of argument.
     * Don't do this for the ":!" command, because ":!! -l" needs the space.
     */
    જો (ea.cmdidx == CMD_bang)
	ea.arg = p;
    નહીંતર
	ea.arg = skipwhite(p);

    // ":file" cannot be run with an argument when "curbuf_lock" is set
    જો (ea.cmdidx == CMD_file && *ea.arg != NUL && curbuf_locked())
	જાઓ doend;

    /*
     * Check for "++opt=val" argument.
     * Must be first, allow ":w ++enc=utf8 !cmd"
     */
    જો (ea.argt & EX_ARGOPT)
	જ્યારે (ea.arg[0] == '+' && ea.arg[1] == '+')
	    જો (getargopt(&ea) == FAIL && !ni)
	    {
		errormsg = _(e_invalid_argument);
		જાઓ doend;
	    }

    જો (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	જો (*ea.arg == '>')			// append
	{
	    જો (*++ea.arg != '>')		// typed wrong
	    {
		errormsg = _(e_use_w_or_w_gt_gt);
		જાઓ doend;
	    }
	    ea.arg = skipwhite(ea.arg + 1);
	    ea.append = TRUE;
	}
	નહીંતર જો (*ea.arg == '!' && ea.cmdidx == CMD_write)  // :w !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    જો (ea.cmdidx == CMD_read)
    {
	જો (ea.forceit)
	{
	    ea.usefilter = TRUE;		// :r! filter if ea.forceit
	    ea.forceit = FALSE;
	}
	નહીંતર જો (*ea.arg == '!')		// :r !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    જો (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ea.amount = 1;
	જ્યારે (*ea.arg == *ea.cmd)		// count number of '>' or '<'
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
    જો ((ea.argt & EX_CMDARG) && !ea.usefilter)
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
    જો ((ea.argt & EX_TRLBAR) && !ea.usefilter)
    {
	separate_nextcmd(&ea, FALSE);
    }
    નહીંતર જો (ea.cmdidx == CMD_bang
	    || ea.cmdidx == CMD_terminal
	    || ea.cmdidx == CMD_global
	    || ea.cmdidx == CMD_vglobal
	    || ea.usefilter
#ifdef FEAT_EVAL
	    || inside_block(&ea)
#endif
	    )
    {
	માટે (p = ea.arg; *p; ++p)
	{
	    // Remove one backslash before a newline
	    જો (*p == '\\' && p[1] == '\n')
		STRMOVE(p, p + 1);
	    નહીંતર જો (*p == '\n' && !(ea.argt & EX_પદાવલિ_ARG))
	    {
		ea.nextcmd = p + 1;
		*p = NUL;
		તોડો;
	    }
	}
    }

    જો ((ea.argt & EX_DFLALL) && ea.addr_ગણતરી == 0)
	address_default_all(&ea);

    // accept numbered register only when no count allowed (:put)
    જો (       (ea.argt & EX_REGSTR)
	    && *ea.arg != NUL
	       // Do not allow register = for user commands
	    && (!IS_USER_CMDIDX(ea.cmdidx) || *ea.arg != '=')
	    && !((ea.argt & EX_ગણતરી) && VIM_ISDIGIT(*ea.arg)))
    {
#જો !defined(FEAT_CLIPBOARD) && !defined(FEAT_CLIPBOARD_PROVIDER)
	// check these explicitly for a more specific error message
	જો (*ea.arg == '*' || *ea.arg == '+')
	{
	    errormsg = _(e_invalid_register_name);
	    જાઓ doend;
	}
#endif
	જો (valid_yank_reg(*ea.arg, (!IS_USER_CMDIDX(ea.cmdidx)
			    && ea.cmdidx != CMD_put && ea.cmdidx != CMD_iput)))
	{
	    ea.regname = *ea.arg++;
#ifdef FEAT_EVAL
	    // for '=' register: accept the rest of the line as an expression
	    જો (ea.arg[-1] == '=' && ea.arg[0] != NUL)
	    {
		જો (!ea.skip)
		{
		    set_પદાવલિ_લીટી(vim_strsave(ea.arg), &ea);
		    did_set_પદાવલિ_લીટી = TRUE;
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
    જો ((ea.argt & EX_ગણતરી) && VIM_ISDIGIT(*ea.arg)
	    && (!(ea.argt & EX_BUFNAME) || *(p = skipdigits(ea.arg + 1)) == NUL
							  || VIM_ISWHITE(*p)))
    {
	n = getdigits_quoted(&ea.arg);
	ea.arg = skipwhite(ea.arg);
	જો (n <= 0 && !ni && (ea.argt & EX_ZEROR) == 0)
	{
	    errormsg = _(e_positive_ગણતરી_required);
	    જાઓ doend;
	}
	જો (ea.addr_type != ADDR_લીટીઓ)	// e.g. :buffer 2, :sleep 3
	{
	    ea.line2 = n;
	    જો (ea.addr_ગણતરી == 0)
		ea.addr_ગણતરી = 1;
	}
	નહીંતર
	{
	    ea.line1 = ea.line2;
	    જો (ea.line2 >= LONG_MAX - (n - 1))
		ea.line2 = LONG_MAX;  // avoid overflow
	    નહીંતર
		ea.line2 += n - 1;
	    ++ea.addr_ગણતરી;
	    /*
	     * Be vi compatible: no error message for out of range.
	     */
	    જો (ea.line2 > curbuf->b_ml.ml_લીટી_ગણતરી)
		ea.line2 = curbuf->b_ml.ml_લીટી_ગણતરી;
	}
    }

    /*
     * Check for flags: 'l', 'p' and '#'.
     */
    જો (ea.argt & EX_FLAGS)
	get_flags(&ea);
    જો (!ni && !(ea.argt & EX_EXTRA) && *ea.arg != NUL
	    && *ea.arg != '"' && (*ea.arg != '|' || (ea.argt & EX_TRLBAR) == 0))
    {
	// no arguments allowed but there is something
	errormsg = ex_errmsg(e_trailing_characters_str, ea.arg);
	જાઓ doend;
    }

    જો (!ni && (ea.argt & EX_NEEDARG) && *ea.arg == NUL)
    {
	errormsg = _(e_argument_required);
	જાઓ doend;
    }

#ifdef FEAT_EVAL
    /*
     * Skip the command when it's not going to be executed.
     * The commands like :if, :endif, etc. always need to be executed.
     * Also make an exception for commands that handle a trailing command
     * themselves.
     */
    જો (ea.skip)
    {
	પસંદગી (ea.cmdidx)
	{
	    // commands that need evaluation
	    સ્થિતિ CMD_while:
	    સ્થિતિ CMD_endwhile:
	    સ્થિતિ CMD_for:
	    સ્થિતિ CMD_endfor:
	    સ્થિતિ CMD_if:
	    સ્થિતિ CMD_elseif:
	    સ્થિતિ CMD_else:
	    સ્થિતિ CMD_endif:
	    સ્થિતિ CMD_try:
	    સ્થિતિ CMD_catch:
	    સ્થિતિ CMD_finally:
	    સ્થિતિ CMD_endtry:
	    સ્થિતિ CMD_function:
	    સ્થિતિ CMD_def:
				તોડો;

	    // commands that read a block of lines
	    સ્થિતિ CMD_abstract:
	    સ્થિતિ CMD_append:
	    સ્થિતિ CMD_autocmd:
	    સ્થિતિ CMD_change:
	    સ્થિતિ CMD_class:
	    સ્થિતિ CMD_command:
	    સ્થિતિ CMD_enum:
	    સ્થિતિ CMD_ઉમેરો:
	    સ્થિતિ CMD_interface:
	    સ્થિતિ CMD_loadkeymap:
				તોડો;

	    // Commands that handle '|' themselves.  Check: A command should
	    // either have the EX_TRLBAR flag, appear in this list or appear in
	    // the list at ":help :bar".
	    સ્થિતિ CMD_aboveleft:
	    સ્થિતિ CMD_and:
	    સ્થિતિ CMD_belowright:
	    સ્થિતિ CMD_botright:
	    સ્થિતિ CMD_browse:
	    સ્થિતિ CMD_call:
	    સ્થિતિ CMD_confirm:
	    સ્થિતિ CMD_const:
	    સ્થિતિ CMD_delfunction:
	    સ્થિતિ CMD_djump:
	    સ્થિતિ CMD_dlist:
	    સ્થિતિ CMD_dsearch:
	    સ્થિતિ CMD_dsplit:
	    સ્થિતિ CMD_echo:
	    સ્થિતિ CMD_echoerr:
	    સ્થિતિ CMD_echomsg:
	    સ્થિતિ CMD_echon:
	    સ્થિતિ CMD_eval:
	    સ્થિતિ CMD_execute:
	    સ્થિતિ CMD_filter:
	    સ્થિતિ CMD_final:
	    સ્થિતિ CMD_help:
	    સ્થિતિ CMD_hide:
	    સ્થિતિ CMD_horizontal:
	    સ્થિતિ CMD_ijump:
	    સ્થિતિ CMD_ilist:
	    સ્થિતિ CMD_isearch:
	    સ્થિતિ CMD_isplit:
	    સ્થિતિ CMD_keepalt:
	    સ્થિતિ CMD_keepjumps:
	    સ્થિતિ CMD_keepmarks:
	    સ્થિતિ CMD_keeppatterns:
	    સ્થિતિ CMD_leftabove:
	    સ્થિતિ CMD_let:
	    સ્થિતિ CMD_lockmarks:
	    સ્થિતિ CMD_lockvar:
	    સ્થિતિ CMD_lua:
	    સ્થિતિ CMD_match:
	    સ્થિતિ CMD_mzscheme:
	    સ્થિતિ CMD_noautocmd:
	    સ્થિતિ CMD_noswapfile:
	    સ્થિતિ CMD_perl:
	    સ્થિતિ CMD_psearch:
	    સ્થિતિ CMD_py3:
	    સ્થિતિ CMD_python3:
	    સ્થિતિ CMD_python:
	    સ્થિતિ CMD_return:
	    સ્થિતિ CMD_rightbelow:
	    સ્થિતિ CMD_ruby:
	    સ્થિતિ CMD_silent:
	    સ્થિતિ CMD_smagic:
	    સ્થિતિ CMD_snomagic:
	    સ્થિતિ CMD_substitute:
	    સ્થિતિ CMD_syntax:
	    સ્થિતિ CMD_tab:
	    સ્થિતિ CMD_tcl:
	    સ્થિતિ CMD_throw:
	    સ્થિતિ CMD_tilde:
	    સ્થિતિ CMD_topleft:
	    સ્થિતિ CMD_unlet:
	    સ્થિતિ CMD_unlockvar:
	    સ્થિતિ CMD_var:
	    સ્થિતિ CMD_verbose:
	    સ્થિતિ CMD_vertical:
	    સ્થિતિ CMD_wincmd:
				તોડો;

	    મૂળભૂત:		જાઓ doend;
	}
    }
#endif

    જો ((ea.argt & EX_XFILE)
	    && expand_filename(&ea, cmdlinep, &errormsg) == FAIL)
	જાઓ doend;

#ifdef FEAT_EVAL
    જો (is_export && (ea.argt & EX_EXPORT) == 0)
    {
	emsg(_(e_invalid_command_after_export));
	જાઓ doend;
    }
#endif

    /*
     * Accept buffer name.  Cannot be used at the same time with a buffer
     * number.  Don't do this for a user command.
     */
    જો ((ea.argt & EX_BUFNAME) && *ea.arg != NUL && ea.addr_ગણતરી == 0
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * :bdelete, :bwipeout and :bunload take several arguments, separated
	 * by spaces: find next space (skipping over escaped characters).
	 * The others take one argument: ignore trailing spaces.
	 */
	જો (ea.cmdidx == CMD_bdelete || ea.cmdidx == CMD_bwipeout
						  || ea.cmdidx == CMD_bunload)
	    p = skiptowhite_esc(ea.arg);
	નહીંતર
	{
	    p = ea.arg + STRLEN(ea.arg);
	    જ્યારે (p > ea.arg && VIM_ISWHITE(p[-1]))
		--p;
	}
	ea.line2 = buflist_findpat(ea.arg, p, (ea.argt & EX_BUFUNL) != 0,
								FALSE, FALSE);
	જો (ea.line2 < 0)	    // failed
	    જાઓ doend;
	ea.addr_ગણતરી = 1;
	ea.arg = skipwhite(p);
    }

    // The :try command saves the emsg_silent flag, reset it here when
    // ":silent! try" was used, it should only apply to :try itself.
    જો (ea.cmdidx == CMD_try && cmdmod.cmod_did_esilent > 0)
    {
	emsg_silent -= cmdmod.cmod_did_esilent;
	જો (emsg_silent < 0)
	    emsg_silent = 0;
	cmdmod.cmod_did_esilent = 0;
    }

/*
 * 7. Execute the command.
 */

    જો (IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * Execute a user-defined command.
	 */
	do_ucmd(&ea);
    }
    નહીંતર
    {
	/*
	 * Call the function to execute the builtin command.
	 */
	(cmdnames[ea.cmdidx].cmd_func)(&ea);
	જો (ea.errmsg != ખાલી)
	    errormsg = ea.errmsg;
    }

#ifdef FEAT_EVAL
    // A command will reset "is_export" when exporting an item.  If it is still
    // set something went wrong or the command was never executed.
    જો (!ea.skip && is_export)
    {
	જો (errormsg == ખાલી)
	    errormsg = _(e_export_with_invalid_argument);
	is_export = FALSE;
    }

    // Set flag that any command was executed, used by ex_vim9script().
    // Not if this was a command that wasn't executed or :endif.
    જો (sourcing_a_script(&ea)
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
    જો (need_rethrow)
	do_throw(cstack);
    નહીંતર જો (check_cstack)
    {
	જો (source_finished(fgetline, cookie))
	    do_finish(&ea, TRUE);
	નહીંતર જો (getline_equal(fgetline, cookie, get_func_લીટી)
						   && current_func_returned())
	    do_return(&ea, TRUE, FALSE, ખાલી);
    }
    need_rethrow = check_cstack = FALSE;
#endif

doend:
    જો (curwin->w_cursor.lnum == 0)	// can happen with zero line number
    {
	curwin->w_cursor.lnum = 1;
	curwin->w_cursor.col = 0;
    }

    જો (errormsg != ખાલી && *errormsg != NUL && !did_emsg)
    {
	જો ((sourcing || !ચાવી_Typed) && !did_append_cmd)
	{
	    જો (errormsg != (અક્ષર *)IObuff)
	    {
		STRCPY(IObuff, errormsg);
		errormsg = (અક્ષર *)IObuff;
	    }
	    append_command(*cmdlinep);
	}
	emsg(errormsg);
    }
#ifdef FEAT_EVAL
    do_errthrow(cstack,
	    (ea.cmdidx != CMD_SIZE && !IS_USER_CMDIDX(ea.cmdidx))
			? cmdnames[(પૂર્ણાંક)ea.cmdidx].cmd_name : (char_u *)ખાલી);

    જો (did_set_પદાવલિ_લીટી)
	set_પદાવલિ_લીટી(ખાલી, ખાલી);
    is_export = FALSE;
#endif

    undo_cmdmod(&cmdmod);
    cmdmod = save_cmdmod;
    reg_executing = save_reg_executing;
    pending_end_reg_executing = save_pending_end_reg_executing;

    જો (ea.nextcmd && *ea.nextcmd == NUL)	// not really a next command
	ea.nextcmd = ખાલી;

#ifdef FEAT_EVAL
    --ex_nesting_level;
    vim_free(ea.cmdline_tofree);
#endif

    પાછા ea.nextcmd;
}

સ્થિર અક્ષર ex_error_buf[સંદેશ_BUF_LEN];

/*
 * Return an error message with argument included.
 * Uses a static buffer, only the last error will be kept.
 * "msg" will be translated, caller should use N_().
 */
    અક્ષર *
ex_errmsg(અક્ષર *સંદેશ, char_u *arg)
{
    vim_snprintf(ex_error_buf, સંદેશ_BUF_LEN, _(સંદેશ), arg);
    પાછા ex_error_buf;
}

/*
 * The "+" string used in place of an empty command in Ex mode.
 * This string is used in pointer comparison.
 */
સ્થિર અચલ અક્ષર exmode_plus[] = "+";

/*
 * Handle a range without a command.
 * Returns an error message on failure.
 */
    અક્ષર *
ex_range_without_command(exarg_T *eap)
{
    અક્ષર *errormsg = ખાલી;

    જો ((*eap->cmd == '|' ||
		(exmode_active && eap->cmd != (char_u *)exmode_plus + 1))
#ifdef FEAT_EVAL
	    && !in_vim9script()
#endif
       )
    {
	eap->cmdidx = CMD_print;
	eap->argt = EX_RANGE+EX_ગણતરી+EX_TRLBAR;
	જો ((errormsg = invalid_range(eap)) == ખાલી)
	{
	    correct_range(eap);
	    ex_print(eap);
	}
    }
    નહીંતર જો (eap->addr_ગણતરી != 0)
    {
	જો (eap->line2 > curbuf->b_ml.ml_લીટી_ગણતરી)
	{
	    // With '-' in 'cpoptions' a line number past the file is an
	    // error, otherwise put it at the end of the file.
	    જો (vim_strchr(p_cpo, CPO_MINUS) != ખાલી)
		eap->line2 = -1;
	    નહીંતર
		eap->line2 = curbuf->b_ml.ml_લીટી_ગણતરી;
	}

	જો (eap->line2 < 0)
	    errormsg = _(e_invalid_range);
	નહીંતર
	{
	    જો (eap->line2 == 0)
		curwin->w_cursor.lnum = 1;
	    નહીંતર
		curwin->w_cursor.lnum = eap->line2;
	    beginline(BL_SOL | BL_FIX);
	}
    }
    પાછા errormsg;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 * If "noparen" is TRUE do not recognize the command followed by "(" or ".".
 */
    સ્થિર પૂર્ણાંક
checkforcmd_opt(
    char_u	**pp,		// start of command
    અક્ષર	*cmd,		// name of command
    પૂર્ણાંક		len,		// required length
    પૂર્ણાંક		noparen)
{
    પૂર્ણાંક		i;

    માટે (i = 0; cmd[i] != NUL; ++i)
	જો (((char_u *)cmd)[i] != (*pp)[i])
	    તોડો;
    જો (i >= len && !ASCII_ISALPHA((*pp)[i]) && (*pp)[i] != '_'
			 && (!noparen || ((*pp)[i] != '(' && (*pp)[i] != '.')))
    {
	*pp = skipwhite(*pp + i);
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    પૂર્ણાંક
checkforcmd(
    char_u	**pp,		// start of command
    અક્ષર	*cmd,		// name of command
    પૂર્ણાંક		len)		// required length
{
    પાછા checkforcmd_opt(pp, cmd, len, FALSE);
}

/*
 * Check for an Ex command with optional tail, not followed by "(" or ".".
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    પૂર્ણાંક
checkforcmd_noparen(
    char_u	**pp,		// start of command
    અક્ષર	*cmd,		// name of command
    પૂર્ણાંક		len)		// required length
{
    પાછા checkforcmd_opt(pp, cmd, len, TRUE);
}

/*
 * Find the replacement text of a ":command", the part after the attributes and
 * the command name.  Returns NULL when there is none.  Does not modify "arg"
 * and gives no error messages.
 */
    સ્થિર char_u *
find_ucmd_repl(char_u *arg)
{
    char_u	*p = arg;

    // Skip over the attributes.
    જ્યારે (*p == '-')
	p = skipwhite(skiptowhite(p));

    // Skip over the command name.
    જો (!ASCII_ISALPHA(*p))
	પાછા ખાલી;
    જ્યારે (ASCII_ISALNUM(*p))
	++p;

    પાછા *p == NUL ? ખાલી : skipwhite(p);
}

/*
 * Find the "{" in "line" that starts a block for ":command" or ":autocmd".
 * That is the case when the command argument is "{" at the end of the line,
 * also when the command is nested in another ":command" or ":autocmd".
 * Returns NULL when the line does not start such a block.
 */
    char_u *
find_cmd_block_start(char_u *લીટી)
{
    char_u	*p = skipwhite(લીટી);

    માટે (;;)
    {
	char_u	*arg = p;

	જો (*p == '{' && ends_excmd2(p, skipwhite(p + 1)))
	    પાછા p;

	જો (checkforcmd_noparen(&arg, "autocmd", 2))
	{
	    જો (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = au_find_cmd_arg(arg);
	}
	નહીંતર જો (checkforcmd_noparen(&arg, "command", 3))
	{
	    જો (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = find_ucmd_repl(arg);
	}
	નહીંતર
	    પાછા ખાલી;

	જો (p == ખાલી)
	    પાછા ખાલી;
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
    પૂર્ણાંક
parse_command_modifiers(
	exarg_T	    *eap,
	અક્ષર	    **errormsg,
	cmdmod_T    *cmod,
	પૂર્ણાંક	    skip_only)
{
    char_u  *orig_cmd = eap->cmd;
    char_u  *cmd_start = ખાલી;
    પૂર્ણાંક	    use_plus_cmd = FALSE;
    પૂર્ણાંક	    starts_with_colon = FALSE;
    પૂર્ણાંક	    vim9script = in_vim9script();
    પૂર્ણાંક	    has_visual_range = FALSE;

    CLEAR_POINTER(cmod);
    cmod->cmod_flags = sticky_cmdmod_flags;

    જો (STRNCMP(eap->cmd, "'<,'>", 5) == 0)
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
    માટે (;;)
    {
	char_u  *p;

	જ્યારે (*eap->cmd == ' ' || *eap->cmd == '\t' || *eap->cmd == ':')
	{
	    જો (*eap->cmd == ':')
		starts_with_colon = TRUE;
	    ++eap->cmd;
	}

	// in ex mode, an empty command (after modifiers) works like :+
	જો (*eap->cmd == NUL && exmode_active
		   && (getline_equal(eap->ea_getline, eap->cookie, getexmodeline)
		       || getline_equal(eap->ea_getline, eap->cookie, getexline))
			&& curwin->w_cursor.lnum < curbuf->b_ml.ml_લીટી_ગણતરી)
	{
	    use_plus_cmd = TRUE;
	    જો (!skip_only)
		ex_pressedreturn = TRUE;
	    તોડો;  // no modifiers following
	}

	// ignore comment and empty lines
	જો (comment_start(eap->cmd, starts_with_colon))
	{
	    // a comment ends at a NL
	    eap->nextcmd = vim_strchr(eap->cmd, '\n');
	    જો (eap->nextcmd != ખાલી)
		++eap->nextcmd;
	    જો (vim9script)
	    {
		જો (has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
#ifdef FEAT_EVAL
		જો (eap->cmd[0] == '#' && eap->cmd[1] == '{'
							 && eap->cmd[2] != '{')
		    *errormsg = _(e_cannot_use_hash_curly_to_start_comment);
#endif
	    }
	    પાછા FAIL;
	}
	જો (*eap->cmd == '\n')
	{
	    eap->nextcmd = eap->cmd + 1;
	    પાછા FAIL;
	}
	જો (*eap->cmd == NUL)
	{
	    જો (!skip_only)
	    {
		ex_pressedreturn = TRUE;
		જો (vim9script && has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
	    }
	    પાછા FAIL;
	}

	p = skip_range(eap->cmd, TRUE, ખાલી);

	// In Vim9 script a variable can shadow a command modifier:
	//   verbose = 123
	//   verbose += 123
	//   silent! verbose = func()
	//   verbose.member = 2
	//   verbose[expr] = 2
	// But not:
	//   verbose [a, b] = list
	જો (vim9script)
	{
	    char_u *s, *n;

	    માટે (s = eap->cmd; ASCII_ISALPHA(*s); ++s)
		;
	    n = skipwhite(s);
	    જો (*n == '.' || *n == '=' || (*n != NUL && n[1] == '=')
		    || *s == '[')
		તોડો;
	}

	પસંદગી (*p)
	{
	    // When adding an entry, also modify cmdmod_info_tab[].
	    સ્થિતિ 'a':	જો (!checkforcmd_noparen(&eap->cmd, "aboveleft", 3))
			    તોડો;
			cmod->cmod_split |= WSP_ABOVE;
			ચાલુ;

	    સ્થિતિ 'b':	જો (checkforcmd_noparen(&eap->cmd, "belowright", 3))
			{
			    cmod->cmod_split |= WSP_BELOW;
			    ચાલુ;
			}
			જો (checkforcmd_opt(&eap->cmd, "browse", 3, TRUE))
			{
#ifdef FEAT_BROWSE_CMD
			    cmod->cmod_flags |= CMOD_BROWSE;
#endif
			    ચાલુ;
			}
			જો (!checkforcmd_noparen(&eap->cmd, "botright", 2))
			    તોડો;
			cmod->cmod_split |= WSP_BOT;
			ચાલુ;

	    સ્થિતિ 'c':	જો (!checkforcmd_opt(&eap->cmd, "confirm", 4, TRUE))
			    તોડો;
#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			cmod->cmod_flags |= CMOD_CONFIRM;
#endif
			ચાલુ;

	    સ્થિતિ 'k':	જો (checkforcmd_noparen(&eap->cmd, "keepmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_KEEPMARKS;
			    ચાલુ;
			}
			જો (checkforcmd_noparen(&eap->cmd, "keepalt", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPALT;
			    ચાલુ;
			}
			જો (checkforcmd_noparen(&eap->cmd, "keeppatterns", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPPATTERNS;
			    ચાલુ;
			}
			જો (!checkforcmd_noparen(&eap->cmd, "keepjumps", 5))
			    તોડો;
			cmod->cmod_flags |= CMOD_KEEPJUMPS;
			ચાલુ;

	    સ્થિતિ 'f':	// only accept ":filter {pat} cmd"
			{
			    char_u  *reg_pat;
			    char_u  *nulp = ખાલી;
			    પૂર્ણાંક	    c = 0;

			    જો (!checkforcmd_noparen(&p, "filter", 4)
				    || *p == NUL
				    || (ends_excmd(*p)
#ifdef FEAT_EVAL
					// in ":filter #pat# cmd" # does not
					// start a comment
				     && (!vim9script || VIM_ISWHITE(p[1]))
#endif
				     ))
				તોડો;
			    જો (*p == '!')
			    {
				cmod->cmod_filter_force = TRUE;
				p = skipwhite(p + 1);
				જો (*p == NUL || ends_excmd(*p))
				    તોડો;
			    }
#ifdef FEAT_EVAL
			    // Avoid that "filter(arg)" is recognized.
			    જો (vim9script && !VIM_ISWHITE(p[-1]))
				તોડો;
#endif
			    જો (skip_only)
				p = skip_vimgrep_pat(p, ખાલી, ખાલી);
			    નહીંતર
				// NOTE: This puts a NUL after the pattern.
				p = skip_vimgrep_pat_ext(p, &reg_pat, ખાલી,
								    &nulp, &c);
			    જો (p == ખાલી || *p == NUL)
				તોડો;
			    જો (!skip_only)
			    {
				cmod->cmod_filter_regmatch.regprog =
						vim_regcomp(reg_pat, RE_MAGIC);
				જો (cmod->cmod_filter_regmatch.regprog == ખાલી)
				    તોડો;
				// restore the character overwritten by NUL
				જો (nulp != ખાલી)
				    *nulp = c;
			    }
			    eap->cmd = p;
			    ચાલુ;
			}

	    સ્થિતિ 'h':	જો (checkforcmd_noparen(&eap->cmd, "horizontal", 3))
			{
			    cmod->cmod_split |= WSP_HOR;
			    ચાલુ;
			}
			// ":hide" and ":hide | cmd" are not modifiers
			જો (p != eap->cmd || !checkforcmd_noparen(&p, "hide", 3)
					       || *p == NUL || ends_excmd(*p))
			    તોડો;
			eap->cmd = p;
			cmod->cmod_flags |= CMOD_HIDE;
			ચાલુ;

	    સ્થિતિ 'l':	જો (checkforcmd_noparen(&eap->cmd, "lockmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_LOCKMARKS;
			    ચાલુ;
			}
			જો (checkforcmd_noparen(&eap->cmd, "legacy", 3))
			{
			    જો (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_legacy_must_be_followed_by_command);
				પાછા FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_VIM9CMD;
			    cmod->cmod_flags |= CMOD_LEGACY;
			    ચાલુ;
			}

			જો (!checkforcmd_noparen(&eap->cmd, "leftabove", 5))
			    તોડો;
			cmod->cmod_split |= WSP_ABOVE;
			ચાલુ;

	    સ્થિતિ 'n':	જો (checkforcmd_noparen(&eap->cmd, "noautocmd", 3))
			{
			    cmod->cmod_flags |= CMOD_NOAUTOCMD;
			    ચાલુ;
			}
			જો (!checkforcmd_noparen(&eap->cmd, "noswapfile", 3))
			    તોડો;
			cmod->cmod_flags |= CMOD_NOSWAPFILE;
			ચાલુ;

	    સ્થિતિ 'r':	જો (!checkforcmd_noparen(&eap->cmd, "rightbelow", 6))
			    તોડો;
			cmod->cmod_split |= WSP_BELOW;
			ચાલુ;

	    સ્થિતિ 's':	જો (checkforcmd_noparen(&eap->cmd, "sandbox", 3))
			{
			    cmod->cmod_flags |= CMOD_SANDBOX;
			    ચાલુ;
			}
			જો (!checkforcmd_noparen(&eap->cmd, "silent", 3))
			    તોડો;
			cmod->cmod_flags |= CMOD_SILENT;
			જો (*eap->cmd == '!' && !VIM_ISWHITE(eap->cmd[-1]))
			{
			    // ":silent!", but not "silent !cmd"
			    eap->cmd = skipwhite(eap->cmd + 1);
			    cmod->cmod_flags |= CMOD_ERRSILENT;
			}
			ચાલુ;

	    સ્થિતિ 't':	જો (checkforcmd_noparen(&p, "tab", 3))
			{
			    જો (!skip_only)
			    {
				long tabnr = get_address(eap, &eap->cmd,
						    ADDR_TABS, eap->skip,
						    skip_only, FALSE, 1);
				જો (tabnr == MAXLNUM)
				    cmod->cmod_tab = tabpage_index(curtab) + 1;
				નહીંતર
				{
				    જો (tabnr < 0 || tabnr > LAST_TAB_NR)
				    {
					*errormsg = _(e_invalid_range);
					પાછા FAIL;
				    }
				    cmod->cmod_tab = tabnr + 1;
				}
			    }
			    eap->cmd = p;
			    ચાલુ;
			}
			જો (!checkforcmd_noparen(&eap->cmd, "topleft", 2))
			    તોડો;
			cmod->cmod_split |= WSP_ટોચ;
			ચાલુ;

	    સ્થિતિ 'u':	જો (!checkforcmd_noparen(&eap->cmd, "unsilent", 3))
			    તોડો;
			cmod->cmod_flags |= CMOD_UNSILENT;
			ચાલુ;

	    સ્થિતિ 'v':	જો (checkforcmd_noparen(&eap->cmd, "vertical", 4))
			{
			    cmod->cmod_split |= WSP_VERT;
			    ચાલુ;
			}
			જો (checkforcmd_noparen(&eap->cmd, "vim9cmd", 4))
			{
			    જો (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_vim9cmd_must_be_followed_by_command);
				પાછા FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_LEGACY;
			    cmod->cmod_flags |= CMOD_VIM9CMD;
			    ચાલુ;
			}
			જો (!checkforcmd_noparen(&p, "verbose", 4))
			    તોડો;
			જો (vim_isdigit(*eap->cmd))
			{
			    // zero means not set, one is verbose == 0, etc.
			    cmod->cmod_verbose = atoi((અક્ષર *)eap->cmd) + 1;
			}
			નહીંતર
			    cmod->cmod_verbose = 2;  // default: verbose == 1
			eap->cmd = p;
			ચાલુ;
	}
	તોડો;
    }

    જો (has_visual_range)
    {
	જો (eap->cmd > cmd_start)
	{
	    // Move the '<,'> range to after the modifiers and insert a colon.
	    // Since the modifiers have been parsed put the colon on top of the
	    // space: "'<,'>mod cmd" -> "mod:'<,'>cmd
	    // Put eap->cmd after the colon.
	    જો (use_plus_cmd)
	    {
		size_t len = STRLEN(cmd_start);

		// Special case: empty command uses "+":
		//  "'<,'>mods" -> "mods *+
		//  Use "*" instead of "'<,'>" to avoid the command getting
		//  longer, in case it was allocated.
		mch_memmove(orig_cmd, cmd_start, len);
		STRCPY(orig_cmd + len, " *+");
	    }
	    નહીંતર
	    {
		mch_memmove(cmd_start - 5, cmd_start, eap->cmd - cmd_start);
		eap->cmd -= 5;
		mch_memmove(eap->cmd - 1, ":'<,'>", 6);
	    }
	}
	નહીંતર
	    // No modifiers, move the pointer back.
	    // Special case: change empty command to "+".
	    જો (use_plus_cmd)
		eap->cmd = (char_u *)"'<,'>+";
	    નહીંતર
		eap->cmd = orig_cmd;
    }
    નહીંતર જો (use_plus_cmd)
	eap->cmd = (char_u *)exmode_plus;

    પાછા OK;
}

/*
 * Return TRUE if "cmod" has anything set.
 */
    પૂર્ણાંક
has_cmdmod(cmdmod_T *cmod, પૂર્ણાંક ignore_silent)
{
    પાછા (cmod->cmod_flags != 0 && (!ignore_silent
		|| (cmod->cmod_flags
		      & ~(CMOD_SILENT | CMOD_ERRSILENT | CMOD_UNSILENT)) != 0))
	    || cmod->cmod_split != 0
	    || cmod->cmod_verbose > 0
	    || cmod->cmod_tab != 0
	    || cmod->cmod_filter_regmatch.regprog != ખાલી;
}

#જો defined(FEAT_EVAL)
/*
 * If Vim9 script and "cmdmod" has anything set give an error and return TRUE.
 */
    પૂર્ણાંક
cmdmod_error(પૂર્ણાંક ignore_silent)
{
    જો (in_vim9script() && has_cmdmod(&cmdmod, ignore_silent))
    {
	emsg(_(e_misplaced_command_modifier));
	પાછા TRUE;
    }
    પાછા FALSE;
}
#endif

/*
 * Apply the command modifiers.  Saves current state in "cmdmod", call
 * undo_cmdmod() later.
 */
    શૂન્ય
apply_cmdmod(cmdmod_T *cmod)
{
#ifdef HAVE_SANDBOX
    જો ((cmod->cmod_flags & CMOD_SANDBOX) && !cmod->cmod_did_sandbox)
    {
	++sandbox;
	cmod->cmod_did_sandbox = TRUE;
    }
#endif
    જો (cmod->cmod_verbose > 0)
    {
	જો (cmod->cmod_verbose_save == 0)
	    cmod->cmod_verbose_save = p_verbose + 1;
	p_verbose = cmod->cmod_verbose - 1;
    }

    જો ((cmod->cmod_flags & (CMOD_SILENT | CMOD_UNSILENT))
	    && cmod->cmod_save_સંદેશ_silent == 0)
    {
	cmod->cmod_save_સંદેશ_silent = સંદેશ_silent + 1;
	cmod->cmod_save_સંદેશ_scroll = સંદેશ_scroll;
    }
    જો (cmod->cmod_flags & CMOD_SILENT)
	++સંદેશ_silent;
    જો (cmod->cmod_flags & CMOD_UNSILENT)
	સંદેશ_silent = 0;

    જો (cmod->cmod_flags & CMOD_ERRSILENT)
    {
	++emsg_silent;
	++cmod->cmod_did_esilent;
    }

    જો ((cmod->cmod_flags & CMOD_NOAUTOCMD) && cmod->cmod_save_ei == ખાલી)
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
    શૂન્ય
undo_cmdmod(cmdmod_T *cmod)
{
    જો (cmod->cmod_verbose_save > 0)
    {
	p_verbose = cmod->cmod_verbose_save - 1;
	cmod->cmod_verbose_save = 0;
    }

#ifdef HAVE_SANDBOX
    જો (cmod->cmod_did_sandbox)
    {
	--sandbox;
	cmod->cmod_did_sandbox = FALSE;
    }
#endif

    જો (cmod->cmod_save_ei != ખાલી)
    {
	// Restore 'eventignore' to the value before ":noautocmd".
	set_string_option_direct((char_u *)"ei", -1, cmod->cmod_save_ei,
							   OPT_FREE, SID_NONE);
	free_string_option(cmod->cmod_save_ei);
	cmod->cmod_save_ei = ખાલી;
    }

    vim_regfree(cmod->cmod_filter_regmatch.regprog);

    જો (cmod->cmod_save_સંદેશ_silent > 0)
    {
	// messages could be enabled for a serious error, need to check if the
	// counters don't become negative
	જો (!did_emsg || સંદેશ_silent > cmod->cmod_save_સંદેશ_silent - 1)
	    સંદેશ_silent = cmod->cmod_save_સંદેશ_silent - 1;
	emsg_silent -= cmod->cmod_did_esilent;
	જો (emsg_silent < 0)
	    emsg_silent = 0;
	// Restore msg_scroll, it's set by file I/O commands, even when no
	// message is actually displayed.
	સંદેશ_scroll = cmod->cmod_save_સંદેશ_scroll;

	// "silent reg" or "silent echo x" inside "redir" leaves msg_col
	// somewhere in the line.  Put it back in the first column.
	જો (redirecting())
	    સંદેશ_col = 0;

	cmod->cmod_save_સંદેશ_silent = 0;
	cmod->cmod_did_esilent = 0;
    }
}

/*
 * Parse the address range, if any, in "eap".
 * May set the last search pattern, unless "silent" is TRUE.
 * Return FAIL and set "errormsg" or return OK.
 */
    પૂર્ણાંક
parse_cmd_address(exarg_T *eap, અક્ષર **errormsg, પૂર્ણાંક silent)
{
    પૂર્ણાંક		address_ગણતરી = 1;
    linenr_T	lnum;
    પૂર્ણાંક		need_check_cursor = FALSE;
    પૂર્ણાંક		ret = FAIL;

    // Repeat for all ',' or ';' separated addresses.
    માટે (;;)
    {
	eap->line1 = eap->line2;
	eap->line2 = default_address(eap);
	eap->cmd = skipwhite(eap->cmd);
	lnum = get_address(eap, &eap->cmd, eap->addr_type, eap->skip, silent,
					eap->addr_ગણતરી == 0, address_ગણતરી++);
	જો (eap->cmd == ખાલી)	// error detected
	    જાઓ theend;
	જો (lnum == MAXLNUM)
	{
	    જો (*eap->cmd == '%')   // '%' - all lines
	    {
		++eap->cmd;
		પસંદગી (eap->addr_type)
		{
		    સ્થિતિ ADDR_લીટીઓ:
		    સ્થિતિ ADDR_OTHER:
			eap->line1 = 1;
			eap->line2 = curbuf->b_ml.ml_લીટી_ગણતરી;
			તોડો;
		    સ્થિતિ ADDR_LOADED_BUFFERS:
			{
			    buf_T	*buf = firstbuf;

			    જ્યારે (buf->b_next != ખાલી
						  && buf->b_ml.ml_mfp == ખાલી)
				buf = buf->b_next;
			    eap->line1 = buf->b_fnum;
			    buf = lastbuf;
			    જ્યારે (buf->b_prev != ખાલી
						  && buf->b_ml.ml_mfp == ખાલી)
				buf = buf->b_prev;
			    eap->line2 = buf->b_fnum;
			    તોડો;
			}
		    સ્થિતિ ADDR_BUFFERS:
			eap->line1 = firstbuf->b_fnum;
			eap->line2 = lastbuf->b_fnum;
			તોડો;
		    સ્થિતિ ADDR_WINDOWS:
		    સ્થિતિ ADDR_TABS:
			જો (IS_USER_CMDIDX(eap->cmdidx))
			{
			    eap->line1 = 1;
			    eap->line2 = eap->addr_type == ADDR_WINDOWS
						  ? LAST_WIN_NR : LAST_TAB_NR;
			}
			નહીંતર
			{
			    // there is no Vim command which uses '%' and
			    // ADDR_WINDOWS or ADDR_TABS
			    *errormsg = _(e_invalid_range);
			    જાઓ theend;
			}
			તોડો;
		    સ્થિતિ ADDR_TABS_RELATIVE:
		    સ્થિતિ ADDR_UNSIGNED:
		    સ્થિતિ ADDR_QUICKFIX:
			*errormsg = _(e_invalid_range);
			જાઓ theend;
		    સ્થિતિ ADDR_ARGUMENTS:
			જો (ARGCOUNT == 0)
			    eap->line1 = eap->line2 = 0;
			નહીંતર
			{
			    eap->line1 = 1;
			    eap->line2 = ARGCOUNT;
			}
			તોડો;
		    સ્થિતિ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			eap->line1 = 1;
			eap->line2 = qf_get_valid_size(eap);
			જો (eap->line2 == 0)
			    eap->line2 = 1;
#endif
			તોડો;
		    સ્થિતિ ADDR_NONE:
			// Will give an error later if a range is found.
			તોડો;
		}
		++eap->addr_ગણતરી;
	    }
	    નહીંતર જો (*eap->cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == ખાલી)
	    {
		pos_T	    *fp;

		// '*' - visual area
		જો (eap->addr_type != ADDR_લીટીઓ)
		{
		    *errormsg = _(e_invalid_range);
		    જાઓ theend;
		}

		++eap->cmd;
		જો (!eap->skip)
		{
		    fp = getmark('<', FALSE);
		    જો (check_mark(fp) == FAIL)
			જાઓ theend;
		    eap->line1 = fp->lnum;
		    fp = getmark('>', FALSE);
		    જો (check_mark(fp) == FAIL)
			જાઓ theend;
		    eap->line2 = fp->lnum;
		    ++eap->addr_ગણતરી;
		}
	    }
	}
	નહીંતર
	    eap->line2 = lnum;
	eap->addr_ગણતરી++;

	જો (*eap->cmd == ';')
	{
	    જો (!eap->skip)
	    {
		curwin->w_cursor.lnum = eap->line2;

		// Don't leave the cursor on an illegal line or column, but do
		// accept zero as address, so 0;/PATTERN/ works correctly
		// (where zero usually means to use the first line).
		// Check the cursor position before returning.
		જો (eap->line2 > 0)
		    check_cursor();
		નહીંતર
		    check_cursor_col();
		need_check_cursor = TRUE;
	    }
	}
	નહીંતર જો (*eap->cmd != ',')
	    તોડો;
	++eap->cmd;
    }

    // One address given: set start and end lines.
    જો (eap->addr_ગણતરી == 1)
    {
	eap->line1 = eap->line2;
	// ... but only implicit: really no address given
	જો (lnum == MAXLNUM)
	    eap->addr_ગણતરી = 0;
    }
    ret = OK;

theend:
    જો (need_check_cursor)
	check_cursor();
    પાછા ret;
}

/*
 * Append "cmd" to the error message in IObuff.
 * Takes care of limiting the length and handling 0xa0, which would be
 * invisible otherwise.
 */
    સ્થિર શૂન્ય
append_command(char_u *cmd)
{
    size_t  len = STRLEN(IObuff);
    char_u  *s = cmd;
    char_u  *d;

    જો (len > IOSIZE - 100)
    {
	// Not enough space, truncate and put in "...".
	d = IObuff + IOSIZE - 100;
	d -= mb_head_off(IObuff, d);
	STRCPY(d, "...");
    }
    STRCAT(IObuff, ": ");
    d = IObuff + STRLEN(IObuff);
    જ્યારે (*s != NUL && d - IObuff + 5 < IOSIZE)
    {
	જો (enc_utf8 ? (s[0] == 0xc2 && s[1] == 0xa0) : *s == 0xa0)
	{
	    s += enc_utf8 ? 2 : 1;
	    STRCPY(d, "<a0>");
	    d += 4;
	}
	નહીંતર જો (d - IObuff + (*mb_ptr2len)(s) + 1 >= IOSIZE)
	    તોડો;
	નહીંતર
	    MB_COPY_CHAR(s, d);
    }
    *d = NUL;
}

#જો defined(FEAT_EVAL)
/*
 * If "start" points "&opt", "&l:opt", "&g:opt" or "$ENV" return a pointer to
 * the name.  Otherwise just return "start".
 */
    char_u *
skip_option_env_lead(char_u *start)
{
    char_u *name = start;

    જો (*start == '&')
    {
	જો ((start[1] == 'l' || start[1] == 'g') && start[2] == ':')
	    name += 3;
	નહીંતર
	    name += 1;
    }
    નહીંતર જો (*start == '$')
	name += 1;
    પાછા name;
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
    સ્થિર પૂર્ણાંક
one_letter_cmd(char_u *p, cmdidx_T *idx)
{
    જો (in_vim9script())
	પાછા FALSE;
    જો (p[0] == 'k'
	    && (p[1] != 'e' || (p[1] == 'e' && p[2] != 'e')))
    {
	*idx = CMD_k;
	પાછા TRUE;
    }
    જો (p[0] == 's'
	    && ((p[1] == 'c' && (p[2] == NUL || (p[2] != 's' && p[2] != 'r'
			&& (p[3] == NUL || (p[3] != 'i' && p[4] != 'p')))))
		|| p[1] == 'g'
		|| (p[1] == 'i' && p[2] != 'm' && p[2] != 'l' && p[2] != 'g')
		|| p[1] == 'I'
		|| (p[1] == 'r' && p[2] != 'e')))
    {
	*idx = CMD_substitute;
	પાછા TRUE;
    }
    પાછા FALSE;
}

#જો defined(FEAT_EVAL)
/*
 * Return TRUE if "cmd" starts with "123->", a number followed by a method
 * call.
 */
    પૂર્ણાંક
સંખ્યા_method(char_u *cmd)
{
    char_u *p = skipdigits(cmd);

    પાછા p > cmd && (p = skipwhite(p))[0] == '-' && p[1] == '>';
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
	પૂર્ણાંક	*full UNUSED,
	પૂર્ણાંક	(*lookup)(char_u *, size_t, પૂર્ણાંક cmd, cctx_T *) UNUSED,
	cctx_T	*cctx UNUSED)
{
    પૂર્ણાંક		len;
    char_u	*p;
    પૂર્ણાંક		i;
#ifndef FEAT_EVAL
    પૂર્ણાંક		vim9 = FALSE;
#નહીંતર
    પૂર્ણાંક		vim9 = in_vim9script();

    /*
     * Recognize a Vim9 script function/method call and assignment:
     * "lvar = value", "lvar(arg)", "[1, 2 3]->Func()"
     */
    p = eap->cmd;
    જો (lookup != ખાલી)
    {
	char_u *pskip = skip_option_env_lead(eap->cmd);

	જો (vim_strchr((char_u *)"{('[\"@&$", *p) != ખાલી
	       || ((p = to_name_const_end(pskip)) > eap->cmd && *p != NUL)
	       || (p[0] == '0' && p[1] == 'z'))
	{
	    પૂર્ણાંક	    oplen;
	    પૂર્ણાંક	    heredoc;
	    char_u  *swp;

	    જો (*eap->cmd == '&'
		    || (eap->cmd[0] == '$'
				  && eap->cmd[1] != '\'' && eap->cmd[1] != '"')
		    || (eap->cmd[0] == '@'
					&& (valid_yank_reg(eap->cmd[1], FALSE)
						       || eap->cmd[1] == '@')))
	    {
		જો (*eap->cmd == '&')
		{
		    p = eap->cmd + 1;
		    જો (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
			p += 2;
		    p = to_name_end(p, FALSE);
		}
		નહીંતર જો (*eap->cmd == '$')
		    p = to_name_end(eap->cmd + 1, FALSE);
		નહીંતર
		    p = eap->cmd + 2;
		જો (ends_excmd(*skipwhite(p)))
		{
		    // "&option <NL>", "$ENV <NL>" and "@r <NL>" are the start
		    // of an expression.
		    eap->cmdidx = CMD_eval;
		    પાછા eap->cmd;
		}
		// "&option" can be followed by "->" or "=", check below
	    }

	    જો (vim9 && *p == '<')
	    {
		// generic function type args
		જો (skip_generic_func_type_args(&p) == FAIL)
		{
		    eap->cmdidx = CMD_SIZE;
		    પાછા p;
		}
	    }

	    swp = skipwhite(p);

	    જો (
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
		જો (*eap->cmd == '{' && ends_excmd(*skipwhite(eap->cmd + 1)))
		{
		    // "{" by itself is the start of a block.
		    eap->cmdidx = CMD_block;
		    પાછા eap->cmd + 1;
		}
		eap->cmdidx = CMD_eval;
		પાછા eap->cmd;
	    }

	    જો ((p != eap->cmd && (
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
		જો (skip_પદાવલિ(&after, ખાલી) == OK)
		{
		    after = skipwhite(after);
		    જો (*after == '=' || (*after != NUL && after[1] == '=')
					 || (after[0] == '.' && after[1] == '.'
							   && after[2] == '='))
			eap->cmdidx = CMD_var;
		}
		--emsg_off;
		પાછા eap->cmd;
	    }

	    // "[...]->Method()" is a list expression, but "[a, b] = Func()" is
	    // an assignment.
	    // If there is no line break inside the "[...]" then "p" is
	    // advanced to after the "]" by to_name_const_end(): check if a "="
	    // follows.
	    // If "[...]" has a line break "p" still points at the "[" and it
	    // can't be an assignment.
	    જો (*eap->cmd == '[')
	    {
		char_u	    *eq;

		p = to_name_const_end(eap->cmd);
		જો (p == eap->cmd && *p == '[')
		{
		    પૂર્ણાંક ગણતરી = 0;
		    પૂર્ણાંક	semicolon = FALSE;

		    p = skip_var_list(eap->cmd, TRUE, &ગણતરી, &semicolon, TRUE);
		}
		eq = p;
		જો (eq != ખાલી)
		{
		    eq = skipwhite(eq);
		    જો (vim_strchr((char_u *)"+-*/%.", *eq) != ખાલી)
		    {
			જો (eq[0] == '.' && eq[1] == '.')
			    ++eq;
			++eq;
		    }
		}
		જો (p == ખાલી || p == eap->cmd || *eq != '=')
		{
		    eap->cmdidx = CMD_eval;
		    પાછા eap->cmd;
		}
		જો (p > eap->cmd && *eq == '=')
		{
		    eap->cmdidx = CMD_var;
		    પાછા eap->cmd;
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
	    જો (oplen > 0)
	    {
		જો (((p - eap->cmd) > 2 && eap->cmd[1] == ':')
			|| *eap->cmd == '&'
			|| *eap->cmd == '$'
			|| *eap->cmd == '@'
			|| (eap->skip && IS_WHITE_OR_NUL(
						      *(skipwhite(p) + oplen)))
			|| lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK)
		{
		    eap->cmdidx = CMD_var;
		    પાછા eap->cmd;
		}
	    }

	    // Recognize trying to use a type for a w:, b:, t: or g: variable:
	    // "w:varname: number = 123".
	    જો (eap->cmd[1] == ':' && *p == ':')
	    {
		eap->cmdidx = CMD_var;
		પાછા eap->cmd;
	    }
	}

	// 1234->func() is a method call
	જો (સંખ્યા_method(eap->cmd))
	{
	    eap->cmdidx = CMD_eval;
	    પાછા eap->cmd;
	}

	// "g:", "s:" and "l:" are always assumed to be a variable, thus start
	// an expression.  A global/substitute/list command needs to use a
	// longer name.
	જો (vim_strchr((char_u *)"gsl", *p) != ખાલી && p[1] == ':')
	{
	    eap->cmdidx = CMD_eval;
	    પાછા eap->cmd;
	}

	// If it is an ID it might be a variable with an operator on the next
	// line, if the variable exists it can't be an Ex command.
	જો (p > eap->cmd && ends_excmd(*skipwhite(p))
		&& (lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK
		    || (ASCII_ISALPHA(eap->cmd[0]) && eap->cmd[1] == ':')))
	{
	    eap->cmdidx = CMD_eval;
	    પાછા eap->cmd;
	}

	// Check for "++nr" and "--nr".
	જો (p == eap->cmd && p[0] != NUL && p[0] == p[1]
						   && (*p == '+' || *p == '-'))
	{
	    eap->cmdidx = *p == '+' ? CMD_increment : CMD_decrement;
	    પાછા eap->cmd + 2;
	}
    }
#endif

    /*
     * Isolate the command and search for it in the command table.
     */
    p = eap->cmd;
    જો (one_letter_cmd(p, &eap->cmdidx))
    {
	++p;
	જો (full != ખાલી)
	    *full = TRUE;
    }
    નહીંતર
    {
	જ્યારે (ASCII_ISALPHA(*p))
	    ++p;
	// for python 3.x support ":py3", ":python3", ":py3file", etc.
	જો (eap->cmd[0] == 'p' && eap->cmd[1] == 'y')
	{
	    જ્યારે (ASCII_ISALNUM(*p))
		++p;
	}
	નહીંતર જો (*p == '9' && STRNCMP("vim9", eap->cmd, 4) == 0)
	{
	    // include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	    ++p;
	    જ્યારે (ASCII_ISALPHA(*p))
		++p;
	}

	// check for non-alpha command
	જો (p == eap->cmd && vim_strchr((char_u *)"@*!=><&~#}", *p) != ખાલી)
	    ++p;
	len = (પૂર્ણાંક)(p - eap->cmd);
	// The "d" command can directly be followed by 'l' or 'p' flag, when
	// not in Vim9 script.
	જો (!vim9 && *eap->cmd == 'd' && (p[-1] == 'l' || p[-1] == 'p'))
	{
	    // Check for ":dl", ":dell", etc. to ":deletel": that's
	    // :delete with the 'l' flag.  Same for 'p'.
	    માટે (i = 0; i < len; ++i)
		જો (eap->cmd[i] != ((char_u *)"delete")[i])
		    તોડો;
	    જો (i == len - 1)
	    {
		--len;
		જો (p[-1] == 'l')
		    eap->flags |= EXFLAG_LIST;
		નહીંતર
		    eap->flags |= EXFLAG_PRINT;
	    }
	}

	જો (ASCII_ISLOWER(eap->cmd[0]))
	{
	    પૂર્ણાંક c1 = eap->cmd[0];
	    પૂર્ણાંક c2 = len == 1 ? NUL : eap->cmd[1];

	    જો (command_ગણતરી != (પૂર્ણાંક)CMD_SIZE)
	    {
		iemsg(e_command_table_needs_to_be_updated_run_make_cmdidxs);
		getout(1);
	    }

	    // Use a precomputed index for fast look-up in cmdnames[]
	    // taking into account the first 2 letters of eap->cmd.
	    eap->cmdidx = cmdidxs1[Char_Ord_નીચલું(c1)];
	    જો (ASCII_ISLOWER(c2))
		eap->cmdidx += cmdidxs2[Char_Ord_નીચલું(c1)][Char_Ord_નીચલું(c2)];
	}
	નહીંતર જો (ASCII_ISUPPER(eap->cmd[0]))
	    eap->cmdidx = CMD_Next;
	નહીંતર
	    eap->cmdidx = CMD_bang;

	માટે ( ; (પૂર્ણાંક)eap->cmdidx < (પૂર્ણાંક)CMD_SIZE;
			       eap->cmdidx = (cmdidx_T)((પૂર્ણાંક)eap->cmdidx + 1))
	    જો (STRNCMP(cmdnames[(પૂર્ણાંક)eap->cmdidx].cmd_name, (અક્ષર *)eap->cmd,
							    (size_t)len) == 0)
	    {
#ifdef FEAT_EVAL
		જો (full != ખાલી && cmdnames[eap->cmdidx].cmd_name[len] == NUL)
		    *full = TRUE;
#endif
		તોડો;
	    }

	// :Print and :mode are not supported in Vim9 script.
	// Some commands cannot be shortened in Vim9 script.
	જો (vim9 && eap->cmdidx != CMD_SIZE)
	{
	    જો (eap->cmdidx == CMD_mode || eap->cmdidx == CMD_Print)
		eap->cmdidx = CMD_SIZE;
	    નહીંતર જો ((cmdnames[eap->cmdidx].cmd_argt & EX_WHOLE)
			  && len < (પૂર્ણાંક)cmdnames[eap->cmdidx].cmd_namelen)
	    {
		semsg(_(e_command_cannot_be_shortened_str), eap->cmd);
		eap->cmdidx = CMD_SIZE;
	    }
	}

	// Do not recognize ":*" as the star command unless '*' is in
	// 'cpoptions'.
	જો (eap->cmdidx == CMD_star && vim_strchr(p_cpo, CPO_STAR) == ખાલી)
	    p = eap->cmd;

	// Look for a user defined command as a last resort.  Let ":Print" be
	// overruled by a user defined command.
	જો ((eap->cmdidx == CMD_SIZE || eap->cmdidx == CMD_Print)
		&& *eap->cmd >= 'A' && *eap->cmd <= 'Z')
	{
	    // User defined commands may contain digits.
	    જ્યારે (ASCII_ISALNUM(*p))
		++p;
	    p = find_ucmd(eap, p, full, ખાલી, ખાલી);
	}
	જો (p == ખાલી || p == eap->cmd)
	    eap->cmdidx = CMD_SIZE;
    }

    // ":fina" means ":finally" in legacy script, for backwards compatibility.
    જો (eap->cmdidx == CMD_final && p - eap->cmd == 4 && !vim9)
	eap->cmdidx = CMD_finally;

     // Force ":ho" to be unresolved.  Without this, find_ex_command()
     // matches it to CMD_horizontal (the only "ho*" entry), which makes
     // fullcommand("ho") return "horizontal" even though ":ho" cannot be
     // used as the modifier (cmdmods[] requires 3 chars, "hor").
    જો (eap->cmdidx == CMD_horizontal && p - eap->cmd == 2)
	eap->cmdidx = CMD_SIZE;

#ifdef FEAT_EVAL
    જો (eap->cmdidx < CMD_SIZE
	    && vim9
	    && !IS_WHITE_NL_OR_NUL(*p) && *p != '!' && *p != '|'
	    && (eap->cmdidx < 0 ||
		(cmdnames[eap->cmdidx].cmd_argt & EX_NONWHITE_OK) == 0))
    {
	// A name that goes on with an underscore is not this command with an
	// argument: "ch_log" is not ":change".  Leave it to be reported as an
	// invalid command.
	જો (*p != '_')
	{
	    char_u *cmd = vim_strnsave(eap->cmd, p - eap->cmd);

	    semsg(_(e_command_str_not_followed_by_white_space_str), cmd,
								    eap->cmd);
	    vim_free(cmd);
	}
	eap->cmdidx = CMD_SIZE;
    }
#endif

    પાછા p;
}

#જો defined(FEAT_EVAL)
પ્રકારવ્યાખ્યા સંરચના
{
    અક્ષર	*name;
    પૂર્ણાંક		minlen;
    પૂર્ણાંક		has_ગણતરી;  // :123verbose  :3tab
} cmdmod_info_T;

સ્થિર cmdmod_info_T cmdmod_info_tab[] = {
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
    પૂર્ણાંક
modifier_len(char_u *cmd)
{
    પૂર્ણાંક		i, j;
    char_u	*p = cmd;

    જો (VIM_ISDIGIT(*cmd))
	p = skipwhite(skipdigits(cmd + 1));
    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	// cmdmod_info_tab[] is sorted by name: once the first letter is past
	// the command's first letter no later entry can match.
	જો (cmdmod_info_tab[i].name[0] > *p)
	    તોડો;
	માટે (j = 0; p[j] != NUL; ++j)
	    જો (p[j] != cmdmod_info_tab[i].name[j])
		તોડો;
	જો (!ASCII_ISALPHA(p[j]) && j >= cmdmod_info_tab[i].minlen
					&& (p == cmd || cmdmod_info_tab[i].has_ગણતરી))
	    પાછા j + (પૂર્ણાંક)(p - cmd);
    }
    પાછા 0;
}

/*
 * Return > 0 if an Ex command "name" exists.
 * Return 2 if there is an exact match.
 * Return 3 if there is an ambiguous match.
 */
    પૂર્ણાંક
cmd_exists(char_u *name)
{
    exarg_T	ea;
    પૂર્ણાંક		full = FALSE;
    પૂર્ણાંક		i;
    પૂર્ણાંક		j;
    char_u	*p;

    // Check command modifiers.
    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	માટે (j = 0; name[j] != NUL; ++j)
	    જો (name[j] != cmdmod_info_tab[i].name[j])
		તોડો;
	જો (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	    પાછા (cmdmod_info_tab[i].name[j] == NUL ? 2 : 1);
    }

    // Check built-in commands and user defined commands.
    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    p = find_ex_command(&ea, &full, ખાલી, ખાલી);
    જો (p == ખાલી)
	પાછા 3;
    જો (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	પાછા 0;
    જો (*skipwhite(p) != NUL)
	પાછા 0;	// trailing garbage
    પાછા (ea.cmdidx == CMD_SIZE ? 0 : (full ? 2 : 1));
}

/*
 * Add the attributes of an Ex command to "d" in the terms of ":command":
 * "argt" are the EX_ flags, "addr_type" the address type and "def" the
 * default for a range or count, -1 when there is none.
 */
    શૂન્ય
ex_command_attrs(dict_T *d, long_u argt, cmd_addr_T addr_type, long def)
{
    સ્થિર અક્ષર *addr_names[] = {
	"lines", "windows", "arguments", "loaded_buffers", "buffers", "tabs",
	"tabs_relative", "quickfix_valid", "quickfix", "unsigned", "other",
	"none"
    };
    અક્ષર	*nargs;

    જો (!(argt & EX_EXTRA))
	nargs = "0";
    નહીંતર જો (!(argt & EX_NOSPC))
	nargs = (argt & EX_NEEDARG) ? "+" : "*";
    નહીંતર જો (!(argt & EX_NEEDARG))
	nargs = "?";
    નહીંતર
	nargs = (argt & EX_ARGSPACE) ? "_" : "1";
    dict_add_string(d, "nargs", (char_u *)nargs);

    જો (!(argt & EX_RANGE))
	dict_add_bool(d, "range", FALSE);
    નહીંતર જો (argt & EX_DFLALL)
	dict_add_string(d, "range", (char_u *)"%");
    નહીંતર જો (def >= 0 && !(argt & EX_ગણતરી))
	dict_add_સંખ્યા(d, "range", def);
    નહીંતર
	dict_add_string(d, "range", (char_u *)".");
    જો (!(argt & EX_ગણતરી))
	dict_add_bool(d, "count", FALSE);
    નહીંતર
	dict_add_સંખ્યા(d, "count", def < 0 ? 0 : def);
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
    પૂર્ણાંક
ex_command_info(char_u *name, પૂર્ણાંક vim9, dict_T *d)
{
    exarg_T	ea;
    char_u	*p;
    પૂર્ણાંક		i;
    પૂર્ણાંક		j;
    પૂર્ણાંક		save_cmod_flags = cmdmod.cmod_flags;
    પૂર્ણાંક		ret = OK;

    જો (vim9 != -1)
    {
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9 ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    // An abbreviation of a command modifier, like ":bo" for ":botright".
    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	માટે (j = 0; name[j] != NUL; ++j)
	    જો (name[j] != cmdmod_info_tab[i].name[j])
		તોડો;
	જો (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	{
	    name = (char_u *)cmdmod_info_tab[i].name;
	    તોડો;
	}
    }

    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    ea.addr_ગણતરી = 0;
    // Don't complain about using "en" or ":let" in Vim9 script.
    ++emsg_silent;
    p = find_ex_command(&ea, ખાલી, ખાલી, ખાલી);
    જો (p == ખાલી || ea.cmdidx == CMD_SIZE
	    || (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	    || *skipwhite(p) != NUL
	    || not_in_vim9(&ea, true) == FAIL)
	ret = FAIL;
    --emsg_silent;

    જો (ret == FAIL)
	;
    નહીંતર જો (IS_USER_CMDIDX(ea.cmdidx))
	ret = user_command_info(ea.useridx, ea.cmdidx, d);
    નહીંતર
    {
	પૂર્ણાંક	notimpl = cmdnames[ea.cmdidx].cmd_func == ex_ni
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
    પાછા ret;
}

/*
 * "fullcommand" function
 */
    શૂન્ય
f_fullcommand(typval_T *argvars, typval_T *rettv)
{
    exarg_T	ea;
    char_u	*name;
    char_u	*p;
    પૂર્ણાંક		vim9script = in_vim9script();
    પૂર્ણાંક		save_cmod_flags = cmdmod.cmod_flags;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	પાછા;

    name = tv_get_string(&argvars[0]);
    જો (name == ખાલી)
	પાછા;

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	vim9script = tv_get_bool(&argvars[1]);
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9script ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    જ્યારે (*name == ':')
	name++;
    name = skip_range(name, TRUE, ખાલી);

    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.addr_ગણતરી = 0;
    ++emsg_silent;  // don't complain about using "en" in Vim9 script
    p = find_ex_command(&ea, ખાલી, ખાલી, ખાલી);
    --emsg_silent;
    જો (p == ખાલી || ea.cmdidx == CMD_SIZE)
	જાઓ theend;

    જો (vim9script && not_in_vim9(&ea, true) == FAIL)
	જાઓ theend;

    rettv->vval.v_string = vim_strsave(IS_USER_CMDIDX(ea.cmdidx)
				 ? get_user_command_name(ea.useridx, ea.cmdidx)
				 : cmdnames[ea.cmdidx].cmd_name);
theend:
    cmdmod.cmod_flags = save_cmod_flags;
}
#endif

    cmdidx_T
excmd_get_cmdidx(char_u *cmd, પૂર્ણાંક len)
{
    cmdidx_T idx;

    જો (!one_letter_cmd(cmd, &idx))
	માટે (idx = (cmdidx_T)0; (પૂર્ણાંક)idx < (પૂર્ણાંક)CMD_SIZE;
		idx = (cmdidx_T)((પૂર્ણાંક)idx + 1))
	    જો (STRNCMP(cmdnames[(પૂર્ણાંક)idx].cmd_name, cmd, (size_t)len) == 0)
		તોડો;

    પાછા idx;
}

    long
excmd_get_argt(cmdidx_T idx)
{
    પાછા (long)cmdnames[(પૂર્ણાંક)idx].cmd_argt;
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
    પૂર્ણાંક		skip_star,	// skip "*" used for Visual range
    પૂર્ણાંક		*ctx)		// pointer to xp_context or NULL
{
    char_u	*cmd = cmd_start;
    unsigned	delim;

    જ્યારે (vim_strchr((char_u *)" \t0123456789.$%'/?-+,;\\", *cmd) != ખાલી)
    {
	જો (*cmd == '\\')
	{
	    જો (cmd[1] == '?' || cmd[1] == '/' || cmd[1] == '&')
		++cmd;
	    નહીંતર
		તોડો;
	}
	નહીંતર જો (*cmd == '\'')
	{
	    char_u *p = cmd;

	    // a quote is only valid at the start or after a separator
	    જ્યારે (p > cmd_start)
	    {
		--p;
		જો (!VIM_ISWHITE(*p))
		    તોડો;
	    }
	    જો (cmd > cmd_start && !VIM_ISWHITE(*p) && *p != ',' && *p != ';')
		તોડો;
	    જો (*++cmd == NUL && ctx != ખાલી)
		*ctx = EXPAND_NOTHING;
	}
	નહીંતર જો (*cmd == '/' || *cmd == '?')
	{
	    delim = *cmd++;
	    જ્યારે (*cmd != NUL && *cmd != delim)
		જો (*cmd++ == '\\' && *cmd != NUL)
		    ++cmd;
	    જો (*cmd == NUL && ctx != ખાલી)
		*ctx = EXPAND_NOTHING;
	}
	જો (*cmd != NUL)
	    ++cmd;
    }

    // Skip ":" and white space.
    જ્યારે (*cmd == ':')
	cmd = skipwhite(cmd + 1);

    // Skip "*" used for Visual range.
    જો (skip_star && *cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == ખાલી)
	cmd = skipwhite(cmd + 1);

    પાછા cmd;
}

    સ્થિર શૂન્ય
addr_error(cmd_addr_T addr_type)
{
    જો (addr_type == ADDR_NONE)
	emsg(_(e_no_range_allowed));
    નહીંતર
	emsg(_(e_invalid_range));
}

/*
 * Return the default address for an address type.
 */
    સ્થિર linenr_T
default_address(exarg_T *eap)
{
    linenr_T lnum = 0;

    પસંદગી (eap->addr_type)
    {
	સ્થિતિ ADDR_લીટીઓ:
	સ્થિતિ ADDR_OTHER:
	    // Default is the cursor line number.  Avoid using an invalid
	    // line number though.
	    જો (curwin->w_cursor.lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
		lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
	    નહીંતર
		lnum = curwin->w_cursor.lnum;
	    તોડો;
	સ્થિતિ ADDR_WINDOWS:
	    lnum = CURRENT_WIN_NR;
	    તોડો;
	સ્થિતિ ADDR_ARGUMENTS:
	    lnum = curwin->w_arg_idx + 1;
	    જો (lnum > ARGCOUNT)
		lnum = ARGCOUNT;
	    તોડો;
	સ્થિતિ ADDR_LOADED_BUFFERS:
	સ્થિતિ ADDR_BUFFERS:
	    lnum = curbuf->b_fnum;
	    તોડો;
	સ્થિતિ ADDR_TABS:
	    lnum = CURRENT_TAB_NR;
	    તોડો;
	સ્થિતિ ADDR_TABS_RELATIVE:
	સ્થિતિ ADDR_UNSIGNED:
	    lnum = 1;
	    તોડો;
	સ્થિતિ ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_idx(eap);
#endif
	    તોડો;
	સ્થિતિ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_valid_idx(eap);
#endif
	    તોડો;
	સ્થિતિ ADDR_NONE:
	    // Will give an error later if a range is found.
	    તોડો;
    }
    પાછા lnum;
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
    પૂર્ણાંક		skip,		// only skip the address, don't use it
    પૂર્ણાંક		silent,		// no errors or side effects
    પૂર્ણાંક		to_other_file,  // flag: may jump to other file
    પૂર્ણાંક		address_ગણતરી UNUSED) // 1 for first address, >1 after comma
{
    પૂર્ણાંક		c;
    પૂર્ણાંક		i;
    long	n;
    char_u	*cmd;
    pos_T	pos;
    pos_T	*fp;
    linenr_T	lnum;
    buf_T	*buf;

    cmd = skipwhite(*ptr);
    lnum = MAXLNUM;
    કરો
    {
	પસંદગી (*cmd)
	{
	    સ્થિતિ '.':			    // '.' - Cursor position
		++cmd;
		પસંદગી (addr_type)
		{
		    સ્થિતિ ADDR_લીટીઓ:
		    સ્થિતિ ADDR_OTHER:
			lnum = curwin->w_cursor.lnum;
			તોડો;
		    સ્થિતિ ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			તોડો;
		    સ્થિતિ ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			તોડો;
		    સ્થિતિ ADDR_LOADED_BUFFERS:
		    સ્થિતિ ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			તોડો;
		    સ્થિતિ ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			તોડો;
		    સ્થિતિ ADDR_NONE:
		    સ્થિતિ ADDR_TABS_RELATIVE:
		    સ્થિતિ ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = ખાલી;
			જાઓ error;
			તોડો;
		    સ્થિતિ ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			તોડો;
		    સ્થિતિ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			તોડો;
		}
		તોડો;

	    સ્થિતિ '$':			    // '$' - last line
		++cmd;
		પસંદગી (addr_type)
		{
		    સ્થિતિ ADDR_લીટીઓ:
		    સ્થિતિ ADDR_OTHER:
			lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
			તોડો;
		    સ્થિતિ ADDR_WINDOWS:
			lnum = LAST_WIN_NR;
			તોડો;
		    સ્થિતિ ADDR_ARGUMENTS:
			lnum = ARGCOUNT;
			તોડો;
		    સ્થિતિ ADDR_LOADED_BUFFERS:
			buf = lastbuf;
			જ્યારે (buf->b_ml.ml_mfp == ખાલી)
			{
			    જો (buf->b_prev == ખાલી)
				તોડો;
			    buf = buf->b_prev;
			}
			lnum = buf->b_fnum;
			તોડો;
		    સ્થિતિ ADDR_BUFFERS:
			lnum = lastbuf->b_fnum;
			તોડો;
		    સ્થિતિ ADDR_TABS:
			lnum = LAST_TAB_NR;
			તોડો;
		    સ્થિતિ ADDR_NONE:
		    સ્થિતિ ADDR_TABS_RELATIVE:
		    સ્થિતિ ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = ખાલી;
			જાઓ error;
			તોડો;
		    સ્થિતિ ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_size(eap);
			જો (lnum == 0)
			    lnum = 1;
#endif
			તોડો;
		    સ્થિતિ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_valid_size(eap);
			જો (lnum == 0)
			    lnum = 1;
#endif
			તોડો;
		}
		તોડો;

	    સ્થિતિ '\'':			    // ''' - mark
		જો (*++cmd == NUL)
		{
		    cmd = ખાલી;
		    જાઓ error;
		}
		જો (addr_type != ADDR_લીટીઓ)
		{
		    addr_error(addr_type);
		    cmd = ખાલી;
		    જાઓ error;
		}
		જો (skip)
		    ++cmd;
		નહીંતર
		{
		    // Only accept a mark in another file when it is
		    // used by itself: ":'M".
		    fp = getmark(*cmd, to_other_file && cmd[1] == NUL);
		    ++cmd;
		    જો (fp == (pos_T *)-1)
			// Jumped to another file.
			lnum = curwin->w_cursor.lnum;
		    નહીંતર
		    {
			જો (check_mark(fp) == FAIL)
			{
			    cmd = ખાલી;
			    જાઓ error;
			}
			lnum = fp->lnum;
		    }
		}
		તોડો;

	    સ્થિતિ '/':
	    સ્થિતિ '?':			// '/' or '?' - search
		c = *cmd++;
		જો (addr_type != ADDR_લીટીઓ)
		{
		    addr_error(addr_type);
		    cmd = ખાલી;
		    જાઓ error;
		}
		જો (skip)	// skip "/pat/"
		{
		    cmd = skip_regexp(cmd, c, magic_isset());
		    જો (*cmd == c)
			++cmd;
		}
		નહીંતર
		{
		    પૂર્ણાંક flags;

		    pos = curwin->w_cursor; // save curwin->w_cursor

		    // When '/' or '?' follows another address, start from
		    // there.
		    જો (lnum > 0 && lnum != MAXLNUM)
			curwin->w_cursor.lnum =
				lnum > curbuf->b_ml.ml_લીટી_ગણતરી
					   ? curbuf->b_ml.ml_લીટી_ગણતરી : lnum;

		    // Start a forward search at the end of the line (unless
		    // before the first line).
		    // Start a backward search at the start of the line.
		    // This makes sure we never match in the current
		    // line, and can match anywhere in the
		    // next/previous line.
		    જો (c == '/' && curwin->w_cursor.lnum > 0)
			curwin->w_cursor.col = MAXCOL;
		    નહીંતર
			curwin->w_cursor.col = 0;
		    searchcmdlen = 0;
		    flags = silent ? શોધ_KEEP : શોધ_HIS | શોધ_સંદેશ;
		    જો (!do_શોધ(ખાલી, c, c, cmd, STRLEN(cmd), 1L, flags, ખાલી))
		    {
			curwin->w_cursor = pos;
			cmd = ખાલી;
			જાઓ error;
		    }
		    lnum = curwin->w_cursor.lnum;
		    curwin->w_cursor = pos;
		    // adjust command string pointer
		    cmd += searchcmdlen;
		}
		તોડો;

	    સ્થિતિ '\\':		    // "\?", "\/" or "\&", repeat search
		++cmd;
		જો (addr_type != ADDR_લીટીઓ)
		{
		    addr_error(addr_type);
		    cmd = ખાલી;
		    જાઓ error;
		}
		જો (*cmd == '&')
		    i = RE_SUBST;
		નહીંતર જો (*cmd == '?' || *cmd == '/')
		    i = RE_શોધ;
		નહીંતર
		{
		    emsg(_(e_backslash_should_be_followed_by));
		    cmd = ખાલી;
		    જાઓ error;
		}

		જો (!skip)
		{
		    /*
		     * When search follows another address, start from
		     * there.
		     */
		    જો (lnum != MAXLNUM)
			pos.lnum = lnum;
		    નહીંતર
			pos.lnum = curwin->w_cursor.lnum;

		    /*
		     * Start the search just like for the above
		     * do_search().
		     */
		    જો (*cmd != '?')
			pos.col = MAXCOL;
		    નહીંતર
			pos.col = 0;
		    pos.coladd = 0;
		    જો (searchit(curwin, curbuf, &pos, ખાલી,
				*cmd == '?' ? BACKWARD : FORWARD,
				(char_u *)"", 0, 1L, શોધ_સંદેશ, i, ખાલી) != FAIL)
			lnum = pos.lnum;
		    નહીંતર
		    {
			cmd = ખાલી;
			જાઓ error;
		    }
		}
		++cmd;
		તોડો;

	    મૂળભૂત:
		જો (VIM_ISDIGIT(*cmd))	// absolute line number
		    lnum = getdigits(&cmd);
	}

	માટે (;;)
	{
	    cmd = skipwhite(cmd);
	    જો (*cmd != '-' && *cmd != '+' && !VIM_ISDIGIT(*cmd))
		તોડો;

	    જો (lnum == MAXLNUM)
	    {
		પસંદગી (addr_type)
		{
		    સ્થિતિ ADDR_લીટીઓ:
		    સ્થિતિ ADDR_OTHER:
			// "+1" is same as ".+1"
			lnum = curwin->w_cursor.lnum;
			તોડો;
		    સ્થિતિ ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			તોડો;
		    સ્થિતિ ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			તોડો;
		    સ્થિતિ ADDR_LOADED_BUFFERS:
		    સ્થિતિ ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			તોડો;
		    સ્થિતિ ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			તોડો;
		    સ્થિતિ ADDR_TABS_RELATIVE:
			lnum = 1;
			તોડો;
		    સ્થિતિ ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			તોડો;
		    સ્થિતિ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			તોડો;
		    સ્થિતિ ADDR_NONE:
		    સ્થિતિ ADDR_UNSIGNED:
			lnum = 0;
			તોડો;
		}
	    }

	    જો (VIM_ISDIGIT(*cmd))
		i = '+';		// "number" is same as "+number"
	    નહીંતર
		i = *cmd++;
	    જો (!VIM_ISDIGIT(*cmd))	// '+' is '+1'
		n = 1;
	    નહીંતર
	    {
		// "number", "+number" or "-number"
		n = getdigits(&cmd);
		જો (n == MAXLNUM)
		{
		    emsg(_(e_લીટી_સંખ્યા_out_of_range));
		    cmd = ખાલી;
		    જાઓ error;
		}
	    }

	    જો (addr_type == ADDR_TABS_RELATIVE)
	    {
		emsg(_(e_invalid_range));
		cmd = ખાલી;
		જાઓ error;
	    }
	    નહીંતર જો (addr_type == ADDR_LOADED_BUFFERS
		    || addr_type == ADDR_BUFFERS)
		lnum = compute_buffer_local_ગણતરી(
				    addr_type, lnum, (i == '-') ? -1 * n : n);
	    નહીંતર
	    {
#ifdef FEAT_FOLDING
		// Relative line addressing: need to adjust for lines in a
		// closed fold after the first address.
		જો (addr_type == ADDR_લીટીઓ && (i == '-' || i == '+')
							 && address_ગણતરી >= 2)
		    (શૂન્ય)hasFolding(lnum, ખાલી, &lnum);
#endif
		જો (i == '-')
		    lnum -= n;
		નહીંતર
		{
		    જો (lnum >= 0 && n >= LONG_MAX - lnum)
		    {
			emsg(_(e_લીટી_સંખ્યા_out_of_range));
			cmd = ખાલી;
			જાઓ error;
		    }
		    lnum += n;
		}
	    }
	}
    } જ્યારે (*cmd == '/' || *cmd == '?');

error:
    *ptr = cmd;
    પાછા lnum;
}

/*
 * Set eap->line1 and eap->line2 to the whole range.
 * Used for commands with the EX_DFLALL flag and no range given.
 */
    સ્થિર શૂન્ય
address_default_all(exarg_T *eap)
{
    eap->line1 = 1;
    પસંદગી (eap->addr_type)
    {
	સ્થિતિ ADDR_લીટીઓ:
	સ્થિતિ ADDR_OTHER:
	    eap->line2 = curbuf->b_ml.ml_લીટી_ગણતરી;
	    તોડો;
	સ્થિતિ ADDR_LOADED_BUFFERS:
	    {
		buf_T *buf = firstbuf;

		જ્યારે (buf->b_next != ખાલી && buf->b_ml.ml_mfp == ખાલી)
		    buf = buf->b_next;
		eap->line1 = buf->b_fnum;
		buf = lastbuf;
		જ્યારે (buf->b_prev != ખાલી && buf->b_ml.ml_mfp == ખાલી)
		    buf = buf->b_prev;
		eap->line2 = buf->b_fnum;
	    }
	    તોડો;
	સ્થિતિ ADDR_BUFFERS:
	    eap->line1 = firstbuf->b_fnum;
	    eap->line2 = lastbuf->b_fnum;
	    તોડો;
	સ્થિતિ ADDR_WINDOWS:
	    eap->line2 = LAST_WIN_NR;
	    તોડો;
	સ્થિતિ ADDR_TABS:
	    eap->line2 = LAST_TAB_NR;
	    તોડો;
	સ્થિતિ ADDR_TABS_RELATIVE:
	    eap->line2 = 1;
	    તોડો;
	સ્થિતિ ADDR_ARGUMENTS:
	    જો (ARGCOUNT == 0)
		eap->line1 = eap->line2 = 0;
	    નહીંતર
		eap->line2 = ARGCOUNT;
	    તોડો;
	સ્થિતિ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    eap->line2 = qf_get_valid_size(eap);
	    જો (eap->line2 == 0)
		eap->line2 = 1;
#endif
	    તોડો;
	સ્થિતિ ADDR_NONE:
	સ્થિતિ ADDR_UNSIGNED:
	સ્થિતિ ADDR_QUICKFIX:
	    iemsg("Cannot use EX_DFLALL with ADDR_NONE, ADDR_UNSIGNED or ADDR_QUICKFIX");
	    તોડો;
    }
}


/*
 * Get flags from an Ex command argument.
 */
    સ્થિર શૂન્ય
get_flags(exarg_T *eap)
{
    જ્યારે (vim_strchr((char_u *)"lp#", *eap->arg) != ખાલી)
    {
	જો (*eap->arg == 'l')
	    eap->flags |= EXFLAG_LIST;
	નહીંતર જો (*eap->arg == 'p')
	    eap->flags |= EXFLAG_PRINT;
	નહીંતર
	    eap->flags |= EXFLAG_NR;
	eap->arg = skipwhite(eap->arg + 1);
    }
}

/*
 * Function called for command which is Not Implemented.  NI!
 */
    શૂન્ય
ex_ni(exarg_T *eap)
{
    જો (!eap->skip)
	eap->errmsg =
		_(e_sorry_command_is_not_available_in_this_version);
}

#ifdef HAVE_EX_SCRIPT_NI
/*
 * Function called for script command which is Not Implemented.  NI!
 * Skips over ":perl <<EOF" constructs.
 */
    સ્થિર શૂન્ય
ex_script_ni(exarg_T *eap)
{
    જો (!eap->skip)
	ex_ni(eap);
    નહીંતર
	vim_free(script_get(eap, eap->arg));
}
#endif

/*
 * Check range in Ex command for validity.
 * Return NULL when valid, error message when invalid.
 */
    સ્થિર અક્ષર *
invalid_range(exarg_T *eap)
{
    buf_T	*buf;

    જો (       eap->line1 < 0
	    || eap->line2 < 0
	    || eap->line1 > eap->line2)
	પાછા _(e_invalid_range);

    જો (eap->argt & EX_RANGE)
    {
	પસંદગી (eap->addr_type)
	{
	    સ્થિતિ ADDR_લીટીઓ:
		જો (eap->line2 > curbuf->b_ml.ml_લીટી_ગણતરી
#ifdef FEAT_DIFF
			    + (eap->cmdidx == CMD_diffget ||
				eap->cmdidx == CMD_diffput)
#endif
		   )
		    પાછા _(e_invalid_range);
		તોડો;
	    સ્થિતિ ADDR_ARGUMENTS:
		// add 1 if ARGCOUNT is 0
		જો (eap->line2 > ARGCOUNT + (!ARGCOUNT))
		    પાછા _(e_invalid_range);
		તોડો;
	    સ્થિતિ ADDR_BUFFERS:
		// Only a boundary check, not whether the buffers actually
		// exist.
		જો (eap->line1 < 1 || eap->line2 > get_highest_fnum())
		    પાછા _(e_invalid_range);
		તોડો;
	    સ્થિતિ ADDR_LOADED_BUFFERS:
		buf = firstbuf;
		જ્યારે (buf->b_ml.ml_mfp == ખાલી)
		{
		    જો (buf->b_next == ખાલી)
			પાછા _(e_invalid_range);
		    buf = buf->b_next;
		}
		જો (eap->line1 < buf->b_fnum)
		    પાછા _(e_invalid_range);
		buf = lastbuf;
		જ્યારે (buf->b_ml.ml_mfp == ખાલી)
		{
		    જો (buf->b_prev == ખાલી)
			પાછા _(e_invalid_range);
		    buf = buf->b_prev;
		}
		જો (eap->line2 > buf->b_fnum)
		    પાછા _(e_invalid_range);
		તોડો;
	    સ્થિતિ ADDR_WINDOWS:
		જો (eap->line2 > LAST_WIN_NR)
		    પાછા _(e_invalid_range);
		તોડો;
	    સ્થિતિ ADDR_TABS:
		જો (eap->line2 > LAST_TAB_NR)
		    પાછા _(e_invalid_range);
		તોડો;
	    સ્થિતિ ADDR_TABS_RELATIVE:
	    સ્થિતિ ADDR_OTHER:
		// Any range is OK.
		તોડો;
	    સ્થિતિ ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
		// No error for value that is too big, will use the last entry.
		જો (eap->line2 <= 0)
		{
		    જો (eap->addr_ગણતરી == 0)
			પાછા _(e_no_errors);
		    પાછા _(e_invalid_range);
		}
#endif
		તોડો;
	    સ્થિતિ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
		જો ((eap->line2 != 1 && eap->line2 > qf_get_valid_size(eap))
			|| eap->line2 < 0)
		    પાછા _(e_invalid_range);
#endif
		તોડો;
	    સ્થિતિ ADDR_UNSIGNED:
	    સ્થિતિ ADDR_NONE:
		// Will give an error elsewhere.
		તોડો;
	}
    }
    પાછા ખાલી;
}

/*
 * Correct the range for zero line number, if required.
 */
    સ્થિર શૂન્ય
correct_range(exarg_T *eap)
{
    જો (!(eap->argt & EX_ZEROR))	    // zero in range not allowed
    {
	જો (eap->line1 == 0)
	    eap->line1 = 1;
	જો (eap->line2 == 0)
	    eap->line2 = 1;
    }
}

#ifdef FEAT_QUICKFIX
/*
 * For a ":vimgrep" or ":vimgrepadd" command return a pointer past the
 * pattern.  Otherwise return eap->arg.
 */
    સ્થિર char_u *
skip_grep_pat(exarg_T *eap)
{
    char_u	*p = eap->arg;

    જો (*p != NUL && (eap->cmdidx == CMD_vimgrep || eap->cmdidx == CMD_lvimgrep
		|| eap->cmdidx == CMD_vimgrepadd
		|| eap->cmdidx == CMD_lvimgrepadd
		|| grep_internal(eap->cmdidx)))
    {
	p = skip_vimgrep_pat(p, ખાલી, ખાલી);
	જો (p == ખાલી)
	    p = eap->arg;
    }
    પાછા p;
}

/*
 * For the ":make" and ":grep" commands insert the 'makeprg'/'grepprg' option
 * in the command line, so that things like % get expanded.
 */
    સ્થિર char_u *
replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep)
{
    char_u	*new_cmdline;
    char_u	*program;
    char_u	*pos;
    char_u	*ptr;
    પૂર્ણાંક		len;
    પૂર્ણાંક		i;

    /*
     * Don't do it when ":vimgrep" is used for ":grep".
     */
    જો ((eap->cmdidx == CMD_make || eap->cmdidx == CMD_lmake
		     || eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
		     || eap->cmdidx == CMD_grepadd
		     || eap->cmdidx == CMD_lgrepadd)
	    && !grep_internal(eap->cmdidx))
    {
	જો (eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
	    || eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	{
	    જો (*curbuf->b_p_gp == NUL)
		program = p_gp;
	    નહીંતર
		program = curbuf->b_p_gp;
	}
	નહીંતર
	{
	    જો (*curbuf->b_p_mp == NUL)
		program = p_mp;
	    નહીંતર
		program = curbuf->b_p_mp;
	}

	p = skipwhite(p);

	જો ((pos = (char_u *)strstr((અક્ષર *)program, "$*")) != ખાલી)
	{
	    // replace $* by given arguments
	    i = 1;
	    જ્યારે ((pos = (char_u *)strstr((અક્ષર *)pos + 2, "$*")) != ખાલી)
		++i;
	    len = (પૂર્ણાંક)STRLEN(p);
	    new_cmdline = alloc(STRLEN(program) + (size_t)i * (len - 2) + 1);
	    જો (new_cmdline == ખાલી)
		પાછા ખાલી;			// out of memory
	    ptr = new_cmdline;
	    જ્યારે ((pos = (char_u *)strstr((અક્ષર *)program, "$*")) != ખાલી)
	    {
		i = (પૂર્ણાંક)(pos - program);
		STRNCPY(ptr, program, i);
		STRCPY(ptr += i, p);
		ptr += len;
		program = pos + 2;
	    }
	    STRCPY(ptr, program);
	}
	નહીંતર
	{
	    size_t program_len = STRLEN(program);

	    new_cmdline = alloc(program_len + STRLEN(p) + 2);
	    જો (new_cmdline == ખાલી)
		પાછા ખાલી;			// out of memory
	    STRCPY(new_cmdline, program);
	    STRCPY(new_cmdline + program_len, " ");
	    STRCPY(new_cmdline + program_len + 1, p);
	}
	સંદેશ_make(p);

	// 'eap->cmd' is not set here, because it is not used at CMD_make
	vim_free(*cmdlinep);
	*cmdlinep = new_cmdline;
	p = new_cmdline;
    }
    પાછા p;
}
#endif

/*
 * Expand file name in Ex command argument.
 * When an error is detected, "errormsgp" is set to a non-NULL pointer.
 * Return FAIL for failure, OK otherwise.
 */
    પૂર્ણાંક
expand_filename(
    exarg_T	*eap,
    char_u	**cmdlinep,
    અક્ષર	**errormsgp)
{
    પૂર્ણાંક		has_wildcards;	// need to expand wildcards
    char_u	*repl;
    size_t	srclen;
    char_u	*p;
    પૂર્ણાંક		n;
    પૂર્ણાંક		escaped;

#ifdef FEAT_QUICKFIX
    // Skip a regexp pattern for ":vimgrep[add] pat file..."
    p = skip_grep_pat(eap);
#નહીંતર
    p = eap->arg;
#endif

    /*
     * Decide to expand wildcards *before* replacing '%', '#', etc.  If
     * the file name contains a wildcard it should not cause expanding.
     * (it will be expanded anyway if there is a wildcard before replacing).
     */
    has_wildcards = mch_has_wildcard(p);
    જ્યારે (*p != NUL)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`, wildcards in it are not expanded.
	જો (p[0] == '`' && p[1] == '=')
	{
	    p += 2;
	    (શૂન્ય)skip_પદાવલિ(&p, ખાલી);
	    જો (*p == '`')
		++p;
	    ચાલુ;
	}
#endif
	/*
	 * Quick check if this cannot be the start of a special string.
	 * Also removes backslash before '%', '#' and '<'.
	 */
	જો (vim_strchr((char_u *)"%#<", *p) == ખાલી)
	{
	    ++p;
	    ચાલુ;
	}

	/*
	 * Try to find a match at this position.
	 */
	repl = eval_vars(p, eap->arg, &srclen, &(eap->do_ecmd_lnum),
						    errormsgp, &escaped, TRUE);
	જો (*errormsgp != ખાલી)		// error detected
	    પાછા FAIL;
	જો (repl == ખાલી)		// no match found
	{
	    p += srclen;
	    ચાલુ;
	}

	// Wildcards won't be expanded below, the replacement is taken
	// literally.  But do expand "~/file", "~user/file" and "$HOME/file".
	જો (vim_strchr(repl, '$') != ખાલી || vim_strchr(repl, '~') != ખાલી)
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
	જો (!eap->usefilter
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
	    સ્થિર char_u *nobslash = (char_u *)" \t\"|";
# define ESCAPE_CHARS nobslash
#નહીંતર
# define ESCAPE_CHARS escape_chars
#endif

	    માટે (l = repl; *l; ++l)
		જો (vim_strchr(ESCAPE_CHARS, *l) != ખાલી)
		{
		    l = vim_strsave_escaped(repl, ESCAPE_CHARS);
		    જો (l != ખાલી)
		    {
			vim_free(repl);
			repl = l;
		    }
		    તોડો;
		}
	}

	// For a shell command a '!' must be escaped.
	જો ((eap->usefilter || eap->cmdidx == CMD_bang
						|| eap->cmdidx == CMD_terminal)
			    && vim_strpbrk(repl, (char_u *)"!") != ખાલી)
	{
	    char_u	*l;

	    l = vim_strsave_escaped(repl, (char_u *)"!");
	    જો (l != ખાલી)
	    {
		vim_free(repl);
		repl = l;
	    }
	}

	p = repl_cmdline(eap, p, srclen, repl, cmdlinep);
	vim_free(repl);
	જો (p == ખાલી)
	    પાછા FAIL;
    }

    /*
     * One file argument: Expand wildcards.
     * Don't do this with ":r !command" or ":w !command".
     */
    જો ((eap->argt & EX_NOSPC) && !eap->usefilter)
    {
	/*
	 * May do this twice:
	 * 1. Replace environment variables.
	 * 2. Replace any other wildcards, remove backslashes.
	 */
	માટે (n = 1; n <= 2; ++n)
	{
	    જો (n == 2)
	    {
		/*
		 * Halve the number of backslashes (this is Vi compatible).
		 * For Unix and OS/2, when wildcards are expanded, this is
		 * done by ExpandOne() below.
		 */
#જો defined(UNIX)
		જો (!has_wildcards)
#endif
		    backslash_halve(eap->arg);
	    }

	    જો (has_wildcards)
	    {
		જો (n == 1)
		{
		    /*
		     * First loop: May expand environment variables.  This
		     * can be done much faster with expand_env() than with
		     * something else (e.g., calling a shell).
		     * After expanding environment variables, check again
		     * if there are still wildcards present.
		     */
		    જો (vim_strchr(eap->arg, '$') != ખાલી
			    || vim_strchr(eap->arg, '~') != ખાલી)
		    {
			expand_env_esc(eap->arg, NameBuff, MAXPATHL,
					(char_u *)(" \t" PATH_ESC_WILDCARDS),
					TRUE, ખાલી);
			has_wildcards = mch_has_wildcard(NameBuff);
			p = NameBuff;
		    }
		    નહીંતર
			p = ખાલી;
		}
		નહીંતર // n == 2
		{
		    expand_T	xpc;
		    પૂર્ણાંક		options = WILD_LIST_NOTFOUND
					       | WILD_NOERROR | WILD_ADD_SLASH;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    જો (p_wic)
			options += WILD_ICASE;
		    p = ExpandOne(&xpc, eap->arg, ખાલી,
						   options, WILD_EXPAND_FREE);
		    જો (p == ખાલી)
			પાછા FAIL;
		}
		જો (p != ખાલી)
		{
		    (શૂન્ય)repl_cmdline(eap, eap->arg, STRLEN(eap->arg),
								 p, cmdlinep);
		    જો (n == 2)	// p came from ExpandOne()
			vim_free(p);
		}
	    }
	}
    }
    પાછા OK;
}

/*
 * Replace part of the command line, keeping eap->cmd, eap->arg and
 * eap->nextcmd correct.
 * "src" points to the part that is to be replaced, of length "srclen".
 * "repl" is the replacement string.
 * Returns a pointer to the character after the replaced string.
 * Returns NULL for failure.
 */
    સ્થિર char_u *
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
    જો (eap->nextcmd != ખાલી)
	i += STRLEN(eap->nextcmd);	// add space for next command
    જો ((new_cmdline = alloc(i)) == ખાલી)
	પાછા ખાલી;			// out of memory!

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

    જો (eap->nextcmd != ખાલી)		// append next command
    {
	new_cmdlinelen += taillen + 1;
	STRCPY(new_cmdline + new_cmdlinelen, eap->nextcmd);
	eap->nextcmd = new_cmdline + new_cmdlinelen;
    }
    eap->cmd = new_cmdline + (eap->cmd - *cmdlinep);
    eap->arg = new_cmdline + (eap->arg - *cmdlinep);
    જો (eap->do_ecmd_cmd != ખાલી && eap->do_ecmd_cmd != dollar_command)
	eap->do_ecmd_cmd = new_cmdline + (eap->do_ecmd_cmd - *cmdlinep);
    vim_free(*cmdlinep);
    *cmdlinep = new_cmdline;

    પાછા src;
}

/*
 * Check for '|' to separate commands and '"' to start comments.
 * If "keep_backslash" is TRUE do not remove any backslash.
 */
    શૂન્ય
separate_nextcmd(exarg_T *eap, પૂર્ણાંક keep_backslash)
{
    પૂર્ણાંક		vim9script = in_vim9script();
    char_u	*p;

#ifdef FEAT_QUICKFIX
    p = skip_grep_pat(eap);
#નહીંતર
    p = eap->arg;
#endif

    માટે ( ; *p; MB_PTR_ADV(p))
    {
	જો (*p == Ctrl_V)
	{
	    જો ((eap->argt & (EX_CTRLV | EX_XFILE)) || keep_backslash)
		++p;		// skip CTRL-V and next char
	    નહીંતર
				// remove CTRL-V and skip next char
		STRMOVE(p, p + 1);
	    જો (*p == NUL)		// stop at NUL after CTRL-V
		તોડો;
	}

#ifdef FEAT_EVAL
	// Skip over `=expr` when wildcards are expanded.
	નહીંતર જો (p[0] == '`' && p[1] == '=' && (eap->argt & EX_XFILE))
	{
	    p += 2;
	    (શૂન્ય)skip_પદાવલિ(&p, ખાલી);
	    જો (*p == NUL)		// stop at NUL after CTRL-V
		તોડો;
	}
#endif

	// Check for '"'/'#': start of comment or '|': next command
	// :@" and :*" do not start a comment!
	// :redir @" doesn't either.
	નહીંતર જો ((*p == '"'
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
		    && eap->cmdidx != CMD_ઉમેરો)
		|| *p == '\n')
	{
	    /*
	     * We remove the '\' before the '|', unless EX_CTRLV is used
	     * AND 'b' is present in 'cpoptions'.
	     */
	    જો ((vim_strchr(p_cpo, CPO_BAR) == ખાલી
			      || !(eap->argt & EX_CTRLV)) && *(p - 1) == '\\')
	    {
		જો (!keep_backslash)
		{
		    STRMOVE(p - 1, p);	// remove the '\'
		    --p;
		}
	    }
	    નહીંતર
	    {
		set_nextcmd(eap, p);
		*p = NUL;
		તોડો;
	    }
	}
    }

    જો (!(eap->argt & EX_NOTRLCOM))	// remove trailing spaces
	del_trailing_spaces(eap->arg);
}

/*
 * get + command from ex argument
 */
    સ્થિર char_u *
getargcmd(char_u **argp)
{
    char_u *arg = *argp;
    char_u *command = ખાલી;

    જો (*arg == '+')	    // +[command]
    {
	++arg;
	જો (vim_isspace(*arg) || *arg == NUL)
	    command = dollar_command;
	નહીંતર
	{
	    command = arg;
	    arg = skip_cmd_arg(command, TRUE);
	    જો (*arg != NUL)
		*arg++ = NUL;		// terminate command with NUL
	}

	arg = skipwhite(arg);	// skip over spaces
	*argp = arg;
    }
    પાછા command;
}

/*
 * Find end of "+command" argument.  Skip over "\ " and "\\".
 */
    char_u *
skip_cmd_arg(
    char_u *p,
    પૂર્ણાંક	   rembs)	// TRUE to halve the number of backslashes
{
    જ્યારે (*p && !vim_isspace(*p))
    {
	જો (*p == '\\' && p[1] != NUL)
	{
	    જો (rembs)
		STRMOVE(p, p + 1);
	    નહીંતર
		++p;
	}
	MB_PTR_ADV(p);
    }
    પાછા p;
}

    પૂર્ણાંક
get_bad_opt(char_u *p, exarg_T *eap)
{
    જો (STRICMP(p, "keep") == 0)
	eap->bad_char = BAD_KEEP;
    નહીંતર જો (STRICMP(p, "drop") == 0)
	eap->bad_char = BAD_DROP;
    નહીંતર જો (MB_BYTE2LEN(*p) == 1 && p[1] == NUL)
	eap->bad_char = *p;
    નહીંતર
	પાછા FAIL;
    પાછા OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of bad= names.
 */
    સ્થિર char_u *
get_bad_name(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    // Note: Keep this in sync with getargopt.
    સ્થિર અક્ષર *(p_bad_કિંમતો[]) =
    {
	"?",
	"keep",
	"drop",
    };

    જો (idx < 0 || idx >= (પૂર્ણાંક)ARRAY_LENGTH(p_bad_કિંમતો))
	પાછા ખાલી;

    પાછા (char_u*)p_bad_કિંમતો[idx];
}

/*
 * Get "++opt=arg" argument.
 * Return FAIL or OK.
 */
    સ્થિર પૂર્ણાંક
getargopt(exarg_T *eap)
{
    char_u	*arg = eap->arg + 2;
    પૂર્ણાંક		*pp = ખાલી;
    પૂર્ણાંક		bad_char_idx;
    char_u	*p;

    // Note: Keep this in sync with get_argopt_name.

    // ":edit ++[no]bin[ary] file"
    જો (STRNCMP(arg, "bin", 3) == 0 || STRNCMP(arg, "nobin", 5) == 0)
    {
	જો (*arg == 'n')
	{
	    arg += 2;
	    eap->force_bin = FORCE_NOBIN;
	}
	નહીંતર
	    eap->force_bin = FORCE_BIN;
	જો (!checkforcmd(&arg, "binary", 3))
	    પાછા FAIL;
	eap->arg = skipwhite(arg);
	પાછા OK;
    }

    // ":read ++edit file"
    જો (STRNCMP(arg, "edit", 4) == 0)
    {
	eap->read_edit = TRUE;
	eap->arg = skipwhite(arg + 4);
	પાછા OK;
    }

    જો (STRNCMP(arg, "ff", 2) == 0)
    {
	arg += 2;
	pp = &eap->force_ff;
    }
    નહીંતર જો (STRNCMP(arg, "fileformat", 10) == 0)
    {
	arg += 10;
	pp = &eap->force_ff;
    }
    નહીંતર જો (STRNCMP(arg, "enc", 3) == 0)
    {
	જો (STRNCMP(arg, "encoding", 8) == 0)
	    arg += 8;
	નહીંતર
	    arg += 3;
	pp = &eap->force_enc;
    }
    નહીંતર જો (STRNCMP(arg, "bad", 3) == 0)
    {
	arg += 3;
	pp = &bad_char_idx;
    }

    જો (pp == ખાલી || *arg != '=')
	પાછા FAIL;

    ++arg;
    *pp = (પૂર્ણાંક)(arg - eap->cmd);
    arg = skip_cmd_arg(arg, FALSE);
    eap->arg = skipwhite(arg);
    *arg = NUL;

    જો (pp == &eap->force_ff)
    {
	જો (check_ff_કિંમત(eap->cmd + eap->force_ff) == FAIL)
	    પાછા FAIL;
	eap->force_ff = eap->cmd[eap->force_ff];
    }
    નહીંતર જો (pp == &eap->force_enc)
    {
	// Make 'fileencoding' lower case.
	માટે (p = eap->cmd + eap->force_enc; *p != NUL; ++p)
	    *p = TOLOWER_ASC(*p);
    }
    નહીંતર
    {
	// Check ++bad= argument.  Must be a single-byte character, "keep" or
	// "drop".
	જો (get_bad_opt(eap->cmd + bad_char_idx, eap) == FAIL)
	    પાછા FAIL;
    }

    પાછા OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of ++opt names.
 */
    સ્થિર char_u *
get_argopt_name(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    // Note: Keep this in sync with getargopt.
    સ્થિર અક્ષર *(p_opt_કિંમતો[]) =
    {
	"fileformat=",
	"encoding=",
	"binary",
	"nobinary",
	"bad=",
	"edit",
    };

    જો (idx < 0 || idx >= (પૂર્ણાંક)ARRAY_LENGTH(p_opt_કિંમતો))
	પાછા ખાલી;

    પાછા (char_u*)p_opt_કિંમતો[idx];
}

/*
 * Command-line expansion for ++opt=name.
 */
    પૂર્ણાંક
expand_argopt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	પૂર્ણાંક	    *અંક_Matches)
{
    જો (xp->xp_pattern > xp->xp_લીટી && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, પૂર્ણાંક) = ખાલી;

	char_u *name_end = xp->xp_pattern - 1;
	જો (name_end - xp->xp_લીટી >= 2
		&& STRNCMP(name_end - 2, "ff", 2) == 0)
	    cb = get_fileformat_name;
	નહીંતર જો (name_end - xp->xp_લીટી >= 10
		&& STRNCMP(name_end - 10, "fileformat", 10) == 0)
	    cb = get_fileformat_name;
	નહીંતર જો (name_end - xp->xp_લીટી >= 3
		&& STRNCMP(name_end - 3, "enc", 3) == 0)
	    cb = get_encoding_name;
	નહીંતર જો (name_end - xp->xp_લીટી >= 8
		&& STRNCMP(name_end - 8, "encoding", 8) == 0)
	    cb = get_encoding_name;
	નહીંતર જો (name_end - xp->xp_લીટી >= 3
		&& STRNCMP(name_end - 3, "bad", 3) == 0)
	    cb = get_bad_name;

	જો (cb != ખાલી)
	{
	    પાછા ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    અંક_Matches,
		    cb,
		    FALSE);
	}
	પાછા FAIL;
    }

    // Special handling of "ff" which acts as a short form of
    // "fileformat", as "ff" is not a substring of it.
    જો (xp->xp_pattern_len == 2
	    && STRNCMP(xp->xp_pattern, "ff", xp->xp_pattern_len) == 0)
    {
	*matches = ALLOC_MULT(char_u *, 1);
	જો (*matches == ખાલી)
	    પાછા FAIL;
	*અંક_Matches = 1;
	(*matches)[0] = vim_strsave((char_u*)"fileformat=");
	પાછા OK;
    }

    પાછા ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    અંક_Matches,
	    get_argopt_name,
	    FALSE);
}

    સ્થિર શૂન્ય
ex_autocmd(exarg_T *eap)
{
    જો (eap->skip)
    {
	skip_cmd_block(eap);
	પાછા;
    }

    /*
     * Disallow autocommands from .exrc and .vimrc in current
     * directory for security reasons.
     */
    જો (secure)
    {
	secure = 2;
	eap->errmsg =
	      _(e_command_not_allowed_from_vimrc_in_current_dir_or_tag_શોધ);
    }
    નહીંતર જો (eap->cmdidx == CMD_autocmd)
	do_autocmd(eap, eap->arg, eap->forceit);
    નહીંતર
	do_augroup(eap->arg, eap->forceit);
}

/*
 * ":doautocmd": Apply the automatic commands to the current buffer.
 */
    સ્થિર શૂન્ય
ex_doautocmd(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    પૂર્ણાંક		call_do_modelines = check_nomodeline(&arg);
    પૂર્ણાંક		did_aucmd;

    (શૂન્ય)do_doautocmd(arg, TRUE, &did_aucmd);
    // Only when there is no <nomodeline>.
    જો (call_do_modelines && did_aucmd)
	do_modelines(0);
}

/*
 * :[N]bunload[!] [N] [bufname] unload buffer
 * :[N]bdelete[!] [N] [bufname] delete buffer from buffer list
 * :[N]bwipeout[!] [N] [bufname] delete buffer really
 */
    સ્થિર શૂન્ય
ex_bunload(exarg_T *eap)
{
    જો (ERROR_IF_ANY_POPUP_WINDOW)
	પાછા;
    eap->errmsg = do_bufdel(
	    eap->cmdidx == CMD_bdelete ? DOBUF_DEL
		: eap->cmdidx == CMD_bwipeout ? DOBUF_WIPE
		: DOBUF_UNLOAD, eap->arg,
	    eap->addr_ગણતરી, (પૂર્ણાંક)eap->line1, (પૂર્ણાંક)eap->line2, eap->forceit);
}

/*
 * :[N]buffer [N]	to buffer N
 * :[N]sbuffer [N]	to buffer N
 */
    સ્થિર શૂન્ય
ex_buffer(exarg_T *eap)
{
    જો (ERROR_IF_ANY_POPUP_WINDOW)
	પાછા;
    do_exbuffer(eap);
}

/*
 * ":buffer" command and alike.
 */
    સ્થિર શૂન્ય
do_exbuffer(exarg_T *eap)
{
    જો (*eap->arg)
	eap->errmsg = ex_errmsg(e_trailing_characters_str, eap->arg);
    નહીંતર
    {
	જો (eap->addr_ગણતરી == 0)	// default is current buffer
	    goto_buffer(eap, DOBUF_CURRENT, FORWARD, 0);
	નહીંતર
	    goto_buffer(eap, DOBUF_FIRST, FORWARD, (પૂર્ણાંક)eap->line2);
	જો (eap->do_ecmd_cmd != ખાલી)
	    do_cmd_argument(eap->do_ecmd_cmd);
    }
}

/*
 * :[N]bmodified [N]	to next mod. buffer
 * :[N]sbmodified [N]	to next mod. buffer
 */
    સ્થિર શૂન્ય
ex_bmodified(exarg_T *eap)
{
    goto_buffer(eap, DOBUF_MOD, FORWARD, (પૂર્ણાંક)eap->line2);
    જો (eap->do_ecmd_cmd != ખાલી)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bnext [N]	to next buffer
 * :[N]sbnext [N]	split and to next buffer
 */
    સ્થિર શૂન્ય
ex_bnext(exarg_T *eap)
{
    જો (ERROR_IF_ANY_POPUP_WINDOW)
	પાછા;

    goto_buffer(eap, DOBUF_CURRENT, FORWARD, (પૂર્ણાંક)eap->line2);
    જો (eap->do_ecmd_cmd != ખાલી)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bNext [N]	to previous buffer
 * :[N]bprevious [N]	to previous buffer
 * :[N]sbNext [N]	split and to previous buffer
 * :[N]sbprevious [N]	split and to previous buffer
 */
    સ્થિર શૂન્ય
ex_bprevious(exarg_T *eap)
{
    જો (ERROR_IF_ANY_POPUP_WINDOW)
	પાછા;

    goto_buffer(eap, DOBUF_CURRENT, BACKWARD, (પૂર્ણાંક)eap->line2);
    જો (eap->do_ecmd_cmd != ખાલી)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :brewind		to first buffer
 * :bfirst		to first buffer
 * :sbrewind		split and to first buffer
 * :sbfirst		split and to first buffer
 */
    સ્થિર શૂન્ય
ex_brewind(exarg_T *eap)
{
    જો (ERROR_IF_ANY_POPUP_WINDOW)
	પાછા;

    goto_buffer(eap, DOBUF_FIRST, FORWARD, 0);
    જો (eap->do_ecmd_cmd != ખાલી)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :blast		to last buffer
 * :sblast		split and to last buffer
 */
    સ્થિર શૂન્ય
ex_blast(exarg_T *eap)
{
    જો (ERROR_IF_ANY_POPUP_WINDOW)
	પાછા;

    goto_buffer(eap, DOBUF_LAST, BACKWARD, 0);
    જો (eap->do_ecmd_cmd != ખાલી)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * Check if "c" ends an Ex command.
 * In Vim9 script does not check for white space before #.
 */
    પૂર્ણાંક
ends_excmd(પૂર્ણાંક c)
{
    પૂર્ણાંક comment_char = '"';

    જો (in_vim9script())
	comment_char = '#';
    પાછા (c == NUL || c == '|' || c == comment_char || c == '\n');
}

/*
 * Like ends_excmd() but checks that a # in Vim9 script either has "cmd" equal
 * to "cmd_start" or has a white space character before it.
 */
    પૂર્ણાંક
ends_excmd2(char_u *cmd_start UNUSED, char_u *cmd)
{
    પૂર્ણાંક c = *cmd;

    જો (c == NUL || c == '|' || c == '\n')
	પાછા TRUE;
    જો (in_vim9script())
	//  # starts a comment, #{ might be a mistake, #{{ can start a fold
	પાછા c == '#' && (cmd[1] != '{' || cmd[2] == '{')
				 && (cmd == cmd_start || VIM_ISWHITE(cmd[-1]));
    પાછા c == '"';
}

#જો defined(FEAT_SYN_HL) || defined(FEAT_શોધ_EXTRA) || defined(FEAT_EVAL)
/*
 * Return the next command, after the first '|' or '\n'.
 * Return NULL if not found.
 */
    char_u *
find_nextcmd(char_u *p)
{
    જ્યારે (*p != '|' && *p != '\n')
    {
	જો (*p == NUL)
	    પાછા ખાલી;
	++p;
    }
    પાછા (p + 1);
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

    જો (*s == '|' || *s == '\n')
	પાછા (s + 1);
    નહીંતર
	પાછા ખાલી;
}

/*
 * If "eap->nextcmd" is not set, check for a next command at "p".
 */
    શૂન્ય
set_nextcmd(exarg_T *eap, char_u *arg)
{
    char_u *p = skipwhite(arg);

    જો (in_vim9script() && *p == '#')
    {
	char_u *nl = vim_strchr(p, NL);
	જો (nl != ખાલી)
	    p = nl;
    }

    p = check_nextcmd(p);

    જો (eap->nextcmd == ખાલી)
	eap->nextcmd = p;
    નહીંતર જો (p != ખાલી)
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
    સ્થિર પૂર્ણાંક
check_more(
    પૂર્ણાંક message,	    // when FALSE check only, no messages
    પૂર્ણાંક forceit)
{
    પૂર્ણાંક	    n = ARGCOUNT - curwin->w_arg_idx - 1;

    જો (!forceit && only_one_window()
	    && ARGCOUNT > 1 && !arg_had_last && n > 0 && quitmore == 0)
    {
	જો (message)
	{
#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    જો ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						    && curbuf->b_fname != ખાલી)
	    {
		char_u	buff[DIALOG_સંદેશ_SIZE];

		vim_snprintf((અક્ષર *)buff, DIALOG_સંદેશ_SIZE,
			NGETTEXT("%d more file to edit.  Quit anyway?",
			    "%d more files to edit.  Quit anyway?", n), n);
		જો (vim_dialog_yesno(VIM_QUESTION, ખાલી, buff, 1) == VIM_YES)
		    પાછા OK;
		પાછા FAIL;
	    }
#endif
	    semsg(NGETTEXT(e_nr_more_file_to_edit,
			   e_nr_more_files_to_edit , n), n);
	    quitmore = 2;	    // next try to quit is allowed
	}
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of command names.
 */
    char_u *
get_command_name(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx >= (પૂર્ણાંક)CMD_SIZE)
	પાછા expand_user_command_name(idx);
    // the following are no real commands
    જો (STRNCMP(cmdnames[idx].cmd_name, "{", 1) == 0 ||
	STRNCMP(cmdnames[idx].cmd_name, "}", 1) == 0)
	પાછા (char_u *)"";
    પાછા cmdnames[idx].cmd_name;
}

    સ્થિર શૂન્ય
ex_colorscheme(exarg_T *eap)
{
    જો (*eap->arg == NUL)
    {
#ifdef FEAT_EVAL
	char_u *પદાવલિ = vim_strsave((char_u *)"g:colors_name");
	char_u *p = ખાલી;

	જો (પદાવલિ != ખાલી)
	{
	    ++emsg_off;
	    p = eval_to_string(પદાવલિ, FALSE, FALSE);
	    --emsg_off;
	    vim_free(પદાવલિ);
	}
	જો (p != ખાલી)
	{
	    સંદેશ((અક્ષર *)p);
	    vim_free(p);
	}
	નહીંતર
	    સંદેશ("default");
#નહીંતર
	સંદેશ(_("unknown"));
#endif
    }
    નહીંતર જો (load_colors(eap->arg) == FAIL)
	semsg(_(e_cannot_find_color_scheme_str), eap->arg);

#ifdef FEAT_VTP
    નહીંતર જો (has_vtp_working())
    {
	// background color change requires clear + redraw
	update_screen(UPD_CLEAR);
	redrawcmd();
    }
#endif
}

    સ્થિર શૂન્ય
ex_highlight(exarg_T *eap)
{
    જો (*eap->arg == NUL && eap->cmd[2] == '!')
	સંદેશ(_("Greetings, Vim user!"));
    do_highlight(eap->arg, eap->forceit, FALSE);
}


/*
 * Call this function if we thought we were going to exit, but we won't
 * (because of an error).  May need to restore the terminal mode.
 */
    શૂન્ય
not_exiting(પૂર્ણાંક save_exiting)
{
    exiting = save_exiting;
    settmode(TMODE_RAW);
}

    પૂર્ણાંક
before_quit_autocmds(win_T *wp, પૂર્ણાંક quit_all, પૂર્ણાંક forceit)
{
    apply_autocmds(EVENT_QUITPRE, ખાલી, ખાલી, FALSE, wp->w_buffer);

    // Bail out when autocommands closed the window.
    // Refuse to quit when the buffer in the last window is being closed (can
    // only happen in autocommands).
    જો (!win_valid(wp)
	    || curbuf_locked()
	    || (wp->w_buffer->b_nwindows == 1 && wp->w_buffer->b_locked > 0))
	પાછા TRUE;

    જો (quit_all || (check_more(FALSE, forceit) == OK && only_one_window()))
    {
	apply_autocmds(EVENT_EXITPRE, ખાલી, ખાલી, FALSE, curbuf);
	// Refuse to quit when locked or when the window was closed or the
	// buffer in the last window is being closed (can only happen in
	// autocommands).
	જો (!win_valid(wp) || curbuf_locked()
			  || (curbuf->b_nwindows == 1 && curbuf->b_locked > 0))
	    પાછા TRUE;
    }

    પાછા FALSE;
}

/*
 * ":quit": quit current window, quit Vim if the last window is closed.
 * ":{nr}quit": quit window {nr}
 * Also used when closing a terminal window that's the last one.
 */
    શૂન્ય
ex_quit(exarg_T *eap)
{
    win_T	*wp;

    જો (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	પાછા;
    }
    // Don't quit while editing the command line.
    જો (text_locked())
    {
	text_locked_સંદેશ();
	પાછા;
    }
    જો (eap->addr_ગણતરી > 0)
    {
	પૂર્ણાંક	wnr = eap->line2;

	માટે (wp = firstwin; wp->w_next != ખાલી; wp = wp->w_next)
	    જો (--wnr <= 0)
		તોડો;
    }
    નહીંતર
	wp = curwin;

    // Refuse to quit when locked.
    જો (curbuf_locked())
	પાછા;

    // Trigger QuitPre and maybe ExitPre
    જો (before_quit_autocmds(wp, FALSE, eap->forceit))
	પાછા;

#ifdef FEAT_NETBEANS_INTG
    netbeansForcedQuit = eap->forceit;
#endif

    પૂર્ણાંક save_exiting = exiting;
    /*
     * If there is only one relevant window we will exit.
     */
    જો (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;
    જો ((!buf_hide(wp->w_buffer)
		&& check_changed(wp->w_buffer, (p_awa ? CCGD_AW : 0)
				       | (eap->forceit ? CCGD_FORCEIT : 0)
				       | CCGD_EXCMD))
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, TRUE)))
    {
	not_exiting(save_exiting);
    }
    નહીંતર
    {
	// quit last window
	// Note: only_one_window() returns true, even so a help window is
	// still open. In that case only quit, if no address has been
	// specified. Example:
	// :h|wincmd w|1q     - don't quit
	// :h|wincmd w|q      - quit
	જો (only_one_window() && (ONE_WINDOW || eap->addr_ગણતરી == 0))
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
    સ્થિર શૂન્ય
ex_cquit(exarg_T *eap UNUSED)
{
    // this does not always pass on the exit code to the Manx compiler. why?
    getout(eap->addr_ગણતરી > 0 ? (પૂર્ણાંક)eap->line2 : EXIT_FAILURE);
}

/*
 * Do preparations for "qall" and "wqall".
 * Returns FAIL when quitting should be aborted.
 */
    પૂર્ણાંક
before_quit_all(exarg_T *eap)
{
    જો (cmdwin_type != 0)
    {
	જો (eap->forceit)
	    cmdwin_result = K_XF1;	// ex_window() takes care of this
	નહીંતર
	    cmdwin_result = K_XF2;
	પાછા FAIL;
    }

    // Don't quit while editing the command line.
    જો (text_locked())
    {
	text_locked_સંદેશ();
	પાછા FAIL;
    }

    જો (before_quit_autocmds(curwin, TRUE, eap->forceit))
	પાછા FAIL;

    પાછા OK;
}

/*
 * ":qall": try to quit all windows
 */
    સ્થિર શૂન્ય
ex_quit_all(exarg_T *eap)
{
    જો (before_quit_all(eap) == FAIL)
	પાછા;
    પૂર્ણાંક save_exiting = exiting;
    exiting = TRUE;
    જો (eap->forceit || !check_changed_any(FALSE, FALSE))
	getout(0);
    not_exiting(save_exiting);
}

/*
 * ":close": close current window, unless it is the last one
 */
    સ્થિર શૂન્ય
ex_close(exarg_T *eap)
{
    win_T	*win;
    પૂર્ણાંક		winnr = 0;
    જો (cmdwin_type != 0)
	cmdwin_result = Ctrl_C;
    નહીંતર
	જો (!text_locked() && !curbuf_locked())
	{
	    જો (eap->addr_ગણતરી == 0)
		ex_win_close(eap->forceit, curwin, ખાલી);
	    નહીંતર
	    {
		FOR_ALL_WINDOWS(win)
		{
		    winnr++;
		    જો (winnr == eap->line2)
			તોડો;
		}
		જો (win == ખાલી)
		    win = lastwin;
		ex_win_close(eap->forceit, win, ખાલી);
	    }
	}
}

#ifdef FEAT_QUICKFIX
/*
 * ":pclose": Close any preview window.
 */
    સ્થિર શૂન્ય
ex_pclose(exarg_T *eap)
{
    win_T	*win;

    // First close any normal window.
    FOR_ALL_WINDOWS(win)
	જો (win->w_p_pvw)
	{
	    ex_win_close(eap->forceit, win, ખાલી);
	    પાછા;
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
    સ્થિર શૂન્ય
ex_win_close(
    પૂર્ણાંક		forceit,
    win_T	*win,
    tabpage_T	*tp)		// NULL or the tab page "win" is in
{
    પૂર્ણાંક		need_hide;
    buf_T	*buf = win->w_buffer;

    // Never close the autocommand window.
    જો (is_aucmd_win(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	પાછા;
    }
    જો (window_layout_locked(CMD_close))
	પાછા;

    need_hide = (bufIsChanged(buf) && buf->b_nwindows <= 1);
    જો (need_hide && !buf_hide(buf) && !forceit)
    {
#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	જો ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    જો (term_job_running(buf->b_term))
	    {
		જો (term_confirm_stop(buf) == FAIL)
		    પાછા;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(buf);
		need_hide = FALSE;
	    }
	    નહીંતર
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(buf, FALSE);
		જો (bufref_valid(&bufref) && bufIsChanged(buf))
		    પાછા;
		need_hide = FALSE;
	    }
	}
	નહીંતર
#endif
	{
	    no_write_message();
	    પાછા;
	}
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // free buffer when not hiding it or when it's a scratch buffer
    જો (tp == ખાલી)
	win_close(win, !need_hide && !buf_hide(buf));
    નહીંતર
	win_close_othertab(win, !need_hide && !buf_hide(buf), tp);
}

/*
 * Handle the argument for a tabpage related ex command.
 * Returns a tabpage number.
 * When an error is encountered then eap->errmsg is set.
 */
    સ્થિર પૂર્ણાંક
get_tabpage_arg(exarg_T *eap)
{
    પૂર્ણાંક tab_સંખ્યા;
    પૂર્ણાંક unaccept_arg0 = (eap->cmdidx == CMD_tabmove) ? 0 : 1;

    જો (eap->arg && *eap->arg != NUL)
    {
	char_u *p = eap->arg;
	char_u *p_save;
	પૂર્ણાંક    relative = 0; // argument +N/-N means: go to N places to the
			     // right/left relative to the current position.

	જો (*p == '-')
	{
	    relative = -1;
	    p++;
	}
	નહીંતર જો (*p == '+')
	{
	    relative = 1;
	    p++;
	}

	p_save = p;
	tab_સંખ્યા = getdigits(&p);

	જો (relative == 0)
	{
	    જો (STRCMP(p, "$") == 0)
		tab_સંખ્યા = LAST_TAB_NR;
	    નહીંતર જો (STRCMP(p, "#") == 0)
		જો (valid_tabpage(lastused_tabpage))
		    tab_સંખ્યા = tabpage_index(lastused_tabpage);
		નહીંતર
		{
		    eap->errmsg = ex_errmsg(e_invalid_કિંમત_for_argument_str,
								     eap->arg);
		    tab_સંખ્યા = 0;
		    જાઓ theend;
		}
	    નહીંતર જો (p == p_save || *p_save == '-' || *p != NUL
		    || tab_સંખ્યા > LAST_TAB_NR)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		જાઓ theend;
	    }
	}
	નહીંતર
	{
	    જો (*p_save == NUL)
		tab_સંખ્યા = 1;
	    નહીંતર જો (p == p_save || *p_save == '-' || *p != NUL
		    || tab_સંખ્યા == 0)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		જાઓ theend;
	    }
	    tab_સંખ્યા = tab_સંખ્યા * relative + tabpage_index(curtab);
	    જો (!unaccept_arg0 && relative == -1)
		--tab_સંખ્યા;
	}
	જો (tab_સંખ્યા < unaccept_arg0 || tab_સંખ્યા > LAST_TAB_NR)
	    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
    }
    નહીંતર જો (eap->addr_ગણતરી > 0)
    {
	જો (unaccept_arg0 && eap->line2 == 0)
	{
	    eap->errmsg = _(e_invalid_range);
	    tab_સંખ્યા = 0;
	}
	નહીંતર
	{
	    tab_સંખ્યા = eap->line2;
	    જો (!unaccept_arg0)
	    {
		char_u *cmdp = eap->cmd;

		જ્યારે (--cmdp > *eap->cmdlinep
			&& (VIM_ISWHITE(*cmdp) || VIM_ISDIGIT(*cmdp)))
		    ;
		જો (*cmdp == '-')
		{
		    --tab_સંખ્યા;
		    જો (tab_સંખ્યા < unaccept_arg0)
			eap->errmsg = _(e_invalid_range);
		}
	    }
	}
    }
    નહીંતર
    {
	પસંદગી (eap->cmdidx)
	{
	સ્થિતિ CMD_tabnext:
	    tab_સંખ્યા = tabpage_index(curtab) + 1;
	    જો (tab_સંખ્યા > LAST_TAB_NR)
		tab_સંખ્યા = 1;
	    તોડો;
	સ્થિતિ CMD_tabmove:
	    tab_સંખ્યા = LAST_TAB_NR;
	    તોડો;
	મૂળભૂત:
	    tab_સંખ્યા = tabpage_index(curtab);
	}
    }

theend:
    પાછા tab_સંખ્યા;
}

/*
 * ":tabclose": close current tab page, unless it is the last one.
 * ":tabclose N": close tab page N.
 */
    સ્થિર શૂન્ય
ex_tabclose(exarg_T *eap)
{
    tabpage_T	*tp;
    પૂર્ણાંક		tab_સંખ્યા;

    જો (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	પાછા;
    }

    જો (first_tabpage->tp_next == ખાલી)
    {
	emsg(_(e_cannot_close_last_tab_page));
	પાછા;
    }

    જો (window_layout_locked(CMD_tabclose))
	પાછા;

    tab_સંખ્યા = get_tabpage_arg(eap);
    જો (eap->errmsg != ખાલી)
	પાછા;

    tp = find_tabpage(tab_સંખ્યા);
    જો (tp == ખાલી)
    {
	beep_flush();
	પાછા;
    }
    જો (tp != curtab)
    {
	tabpage_close_other(tp, eap->forceit);
	પાછા;
    }
    નહીંતર જો (!text_locked() && !curbuf_locked())
	tabpage_close(eap->forceit);
}

/*
 * ":tabonly": close all tab pages except the current one
 */
    સ્થિર શૂન્ય
ex_tabonly(exarg_T *eap)
{
    tabpage_T	*tp;
    પૂર્ણાંક		done;
    પૂર્ણાંક		tab_સંખ્યા;

    જો (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	પાછા;
    }

    જો (first_tabpage->tp_next == ખાલી)
    {
	સંદેશ(_("Already only one tab page"));
	પાછા;
    }

    જો (window_layout_locked(CMD_tabonly))
	પાછા;

    tab_સંખ્યા = get_tabpage_arg(eap);
    જો (eap->errmsg != ખાલી)
	પાછા;

    goto_tabpage(tab_સંખ્યા);
    // Repeat this up to a 1000 times, because autocommands may
    // mess up the lists.
    માટે (done = 0; done < 1000; ++done)
    {
	FOR_ALL_TABPAGES(tp)
	    જો (tp->tp_topframe != topframe)
	    {
		tabpage_close_other(tp, eap->forceit);
		// if we failed to close it quit
		જો (valid_tabpage(tp))
		    done = 1000;
		// start over, "tp" is now invalid
		તોડો;
	    }
	જો (first_tabpage->tp_next == ખાલી)
	    તોડો;
    }
}

/*
 * Close the current tab page.
 */
    શૂન્ય
tabpage_close(પૂર્ણાંક forceit)
{
    જો (window_layout_locked(CMD_tabclose))
	પાછા;

    trigger_tabclosedpre(curtab);
    curtab->tp_did_tabclosedpre = TRUE;
    tabpage_T *save_curtab = curtab;

    // First close all the windows but the current one.  If that worked then
    // close the last window in this tab, that will close it.
    જો (!ONE_WINDOW)
	close_others(TRUE, forceit);
    જો (ONE_WINDOW)
	ex_win_close(forceit, curwin, ખાલી);
    જો (curtab == save_curtab)
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
    શૂન્ય
tabpage_close_other(tabpage_T *tp, પૂર્ણાંક forceit)
{
    પૂર્ણાંક		done = 0;
    win_T	*wp;

    જો (window_layout_locked(CMD_SIZE))
	પાછા;

    trigger_tabclosedpre(tp);
    tp->tp_did_tabclosedpre = TRUE;

    // Limit to 1000 windows, autocommands may add a window while we close
    // one.  OK, so I'm paranoid...
    જ્યારે (++done < 1000)
    {
	wp = tp->tp_firstwin;
	ex_win_close(forceit, wp, tp);

	// Autocommands may delete the tab page under our fingers.
	જો (!valid_tabpage(tp))
	    તોડો;
	// We may fail to close a window with a modified buffer.
	જો (tp->tp_firstwin == wp)
	{
	    done = 1000;
	    તોડો;
	}
    }
    જો (done >= 1000)
    {
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	tp->tp_did_tabclosedpre = FALSE;
	પાછા;
    }

    apply_autocmds(EVENT_TABCLOSED, ખાલી, ખાલી, FALSE, curbuf);
}

/*
 * ":only".
 */
    સ્થિર શૂન્ય
ex_only(exarg_T *eap)
{
    જો (window_layout_locked(CMD_only))
	પાછા;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    જો (eap->addr_ગણતરી > 0)
    {
	win_T   *wp;
	પૂર્ણાંક	wnr = eap->line2;
	માટે (wp = firstwin; --wnr > 0; )
	{
	    જો (wp->w_next == ખાલી)
		તોડો;
	    નહીંતર
		wp = wp->w_next;
	}
	win_goto(wp);
    }
    close_others(TRUE, eap->forceit);
}

    સ્થિર શૂન્ય
ex_hide(exarg_T *eap UNUSED)
{
    // ":hide" or ":hide | cmd": hide current window
    જો (eap->skip)
	પાછા;

    જો (window_layout_locked(CMD_hide))
	પાછા;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    જો (eap->addr_ગણતરી == 0)
	win_close(curwin, FALSE);	// don't free buffer
    નહીંતર
    {
	પૂર્ણાંક	winnr = 0;
	win_T	*win;

	FOR_ALL_WINDOWS(win)
	{
	    winnr++;
	    જો (winnr == eap->line2)
		તોડો;
	}
	જો (win == ખાલી)
	    win = lastwin;
	win_close(win, FALSE);
    }
}

/*
 * ":stop" and ":suspend": Suspend Vim.
 */
    શૂન્ય
ex_stop(exarg_T *eap)
{
    /*
     * Disallow suspending for "rvim".
     */
    જો (check_restricted())
	પાછા;

    જો (!eap->forceit)
	autowrite_all();
    apply_autocmds(EVENT_VIMSUSPEND, ખાલી, ખાલી, FALSE, ખાલી);
    windgoto((પૂર્ણાંક)Rows - 1, cmdline_col_off);
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
    apply_autocmds(EVENT_VIMRESUME, ખાલી, ખાલી, FALSE, ખાલી);
}

/*
 * ":exit", ":xit" and ":wq": Write file and quit the current window.
 */
    સ્થિર શૂન્ય
ex_exit(exarg_T *eap)
{
#ifdef FEAT_EVAL
    જો (not_in_vim9(eap, false) == FAIL)
	પાછા;
#endif
    જો (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	પાછા;
    }
    // Don't quit while editing the command line.
    જો (text_locked())
    {
	text_locked_સંદેશ();
	પાછા;
    }

    પૂર્ણાંક save_exiting = exiting;
    /*
     * we plan to exit if there is only one relevant window
     */
    જો (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;

    // Write the buffer for ":wq" or when it was changed.
    // Trigger QuitPre and ExitPre.
    // Check if we can exit now, after autocommands have changed things.
    જો (((eap->cmdidx == CMD_wq || curbufIsChanged()) && do_write(eap) == FAIL)
	    || before_quit_autocmds(curwin, FALSE, eap->forceit)
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, FALSE)))
    {
	not_exiting(save_exiting);
    }
    નહીંતર
    {
	જો (only_one_window())	    // quit last window, exit Vim
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
    સ્થિર શૂન્ય
ex_print(exarg_T *eap)
{
    જો (curbuf->b_ml.ml_flags & ML_EMPTY)
	emsg(_(e_empty_buffer));
    નહીંતર
    {
	માટે ( ;!got_int; ui_breakcheck())
	{
	    print_લીટી(eap->line1,
		    (eap->cmdidx == CMD_સંખ્યા || eap->cmdidx == CMD_pound
						 || (eap->flags & EXFLAG_NR)),
		    eap->cmdidx == CMD_list || (eap->flags & EXFLAG_LIST));
	    જો (++eap->line1 > eap->line2)
		તોડો;
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
    સ્થિર શૂન્ય
ex_goto(exarg_T *eap)
{
    goto_byte(eap->line2);
}
#endif

/*
 * ":shell".
 */
    સ્થિર શૂન્ય
ex_shell(exarg_T *eap UNUSED)
{
    do_shell(ખાલી, 0);
}

#જો defined(HAVE_DROP_FILE)

સ્થિર પૂર્ણાંક drop_busy = FALSE;
સ્થિર પૂર્ણાંક drop_filec;
સ્થિર char_u **drop_filev = ખાલી;
સ્થિર પૂર્ણાંક drop_split;
સ્થિર શૂન્ય (*drop_callback)(શૂન્ય *);
સ્થિર શૂન્ય *drop_cookie;

    સ્થિર શૂન્ય
handle_drop_internal(શૂન્ય)
{
    exarg_T	ea;
    પૂર્ણાંક		save_સંદેશ_scroll = સંદેશ_scroll;

    // Setting the argument list may cause screen updates and being called
    // recursively.  Avoid that by setting drop_busy.
    drop_busy = TRUE;

    // Check whether the current buffer is changed. If so, we will need
    // to split the current window or data could be lost.
    // We don't need to check if the 'hidden' option is set, as in this
    // case the buffer won't be lost.
    જો (!buf_hide(curbuf) && !drop_split)
    {
	++emsg_off;
	drop_split = check_changed(curbuf, CCGD_AW);
	--emsg_off;
    }
    જો (drop_split)
    {
	જો (win_split(0, 0) == FAIL)
	    પાછા;
	RESET_BINDING(curwin);

	// When splitting the window, create a new alist.  Otherwise the
	// existing one is overwritten.
	alist_unlink(curwin->w_alist);
	alist_new();
    }

    /*
     * Set up the new argument list.
     */
    alist_set(ALIST(curwin), drop_filec, drop_filev, FALSE, ખાલી, 0);

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
    સંદેશ_scroll = save_સંદેશ_scroll;

    જો (drop_callback != ખાલી)
	drop_callback(drop_cookie);

    drop_filev = ખાલી;
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
    શૂન્ય
handle_drop(
    પૂર્ણાંક		filec,		// the number of files dropped
    char_u	**filev,	// the list of files dropped
    પૂર્ણાંક		split,		// force splitting the window
    શૂન્ય	(*callback)(શૂન્ય *), // to be called after setting the argument
				     // list
    શૂન્ય	*cookie)	// argument for "callback" (allocated)
{
    // Cannot handle recursive drops, finish the pending one.
    જો (drop_busy)
    {
	FreeWild(filec, filev);
	vim_free(cookie);
	પાછા;
    }

    // When calling handle_drop() more than once in a row we only use the last
    // one.
    જો (drop_filev != ખાલી)
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
    જો (text_locked() || curbuf_locked() || updating_screen)
	પાછા;

    handle_drop_internal();
}

/*
 * To be called when text is unlocked, curbuf is unlocked or updating_screen is
 * reset: Handle a postponed drop.
 */
    શૂન્ય
handle_any_postponed_drop(શૂન્ય)
{
    જો (!drop_busy && drop_filev != ખાલી
	     && !text_locked() && !curbuf_locked() && !updating_screen)
	handle_drop_internal();
}
#endif

/*
 * ":preserve".
 */
    સ્થિર શૂન્ય
ex_preserve(exarg_T *eap UNUSED)
{
    curbuf->b_flags |= BF_PRESERVED;
    ml_preserve(curbuf, TRUE);
}

/*
 * ":recover".
 */
    સ્થિર શૂન્ય
ex_recover(exarg_T *eap)
{
    // Set recoverymode right away to avoid the ATTENTION prompt.
    recoverymode = TRUE;
    જો (!check_changed(curbuf, (p_awa ? CCGD_AW : 0)
			     | CCGD_MULTWIN
			     | (eap->forceit ? CCGD_FORCEIT : 0)
			     | CCGD_EXCMD)

	    && (*eap->arg == NUL
			     || setfname(curbuf, eap->arg, ખાલી, TRUE) == OK))
	ml_recover(TRUE);
    recoverymode = FALSE;
}

/*
 * Command modifier used in a wrong way.  Also for other commands that can't
 * appear at the toplevel.
 */
    સ્થિર શૂન્ય
ex_wrongmodifier(exarg_T *eap)
{
    eap->errmsg = ex_errmsg(e_invalid_command_str, eap->cmd);
}

#જો defined(FEAT_EVAL)

// callback function for 'findfunc'
સ્થિર callback_T ffu_cb;

    સ્થિર callback_T *
get_findfunc_callback(શૂન્ય)
{
    પાછા *curbuf->b_p_ffu != NUL ? &curbuf->b_ffu_cb : &ffu_cb;
}

/*
 * Call 'findfunc' to obtain a list of file names.
 */
    સ્થિર list_T *
call_findfunc(char_u *pat, પૂર્ણાંક cmdcomplete)
{
    typval_T	args[3];
    callback_T	*cb;
    typval_T	rettv;
    પૂર્ણાંક		retval;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_BOOL;
    args[1].vval.v_સંખ્યા = cmdcomplete;
    args[2].v_type = VAR_UNKNOWN;

    // Lock the text to prevent weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;

    ctx = get_option_sctx("findfunc");
    જો (ctx != ખાલી)
	current_sctx = *ctx;

    cb = get_findfunc_callback();
    retval = call_callback(cb, -1, &rettv, 2, args);

    current_sctx = saved_sctx;

    --textlock;

    list_T *retlist = ખાલી;

    જો (retval == OK)
    {
	જો (rettv.v_type == VAR_LIST)
	    retlist = list_copy(rettv.vval.v_list, FALSE, FALSE, get_copyID());
	નહીંતર
	    emsg(_(e_invalid_return_type_from_findfunc));

	clear_tv(&rettv);
    }

    પાછા retlist;
}

/*
 * Find file names matching "pat" using 'findfunc' and return it in "files".
 * Used for expanding the :find, :sfind and :tabfind command argument.
 * Returns OK on success and FAIL otherwise.
 */
    પૂર્ણાંક
expand_findfunc(expand_T *xp, char_u *pat, char_u ***files, પૂર્ણાંક *અંક_Matches)
{
    list_T	*l;
    પૂર્ણાંક		len;

    *અંક_Matches = 0;
    *files = ખાલી;

    l = call_findfunc(pat, VVAL_TRUE);

    જો (l == ખાલી)
	પાછા FAIL;

    len = list_len(l);
    જો (len == 0)	    // empty List
    {
	list_free(l);
	પાછા FAIL;
    }

    expand_process_user_list(l, files, અંક_Matches, xp);
    list_free(l);

    પાછા OK;
}

/*
 * Use 'findfunc' to find file 'findarg'.  The 'count' argument is used to find
 * the n'th matching file.
 */
    સ્થિર char_u *
findfunc_find_file(char_u *findarg, પૂર્ણાંક findarg_len, પૂર્ણાંક ગણતરી)
{
    list_T	*fname_list;
    char_u	*ret_fname = ખાલી;
    char_u	cc;
    પૂર્ણાંક		fname_ગણતરી;

    cc = findarg[findarg_len];
    findarg[findarg_len] = NUL;

    fname_list = call_findfunc(findarg, VVAL_FALSE);
    fname_ગણતરી = list_len(fname_list);

    જો (fname_ગણતરી == 0)
	semsg(_(e_cant_find_file_str_in_path), findarg);
    નહીંતર
    {
	જો (ગણતરી > fname_ગણતરી)
	    semsg(_(e_no_more_file_str_found_in_path), findarg);
	નહીંતર
	{
	    listitem_T *li = list_find(fname_list, ગણતરી - 1);

	    જો (li != ખાલી)
	    {
		typval_T *tv = &li->li_tv;

		જો (tv->v_type == VAR_STRING && tv->vval.v_string != ખાલી)
		    ret_fname = vim_strsave(tv->vval.v_string);
		નહીંતર જો (tv->v_type == VAR_DICT && tv->vval.v_dict != ખાલી)
		    ret_fname = dict_get_string(tv->vval.v_dict, "word", TRUE);
	    }
	}
    }

    જો (fname_list != ખાલી)
	list_free(fname_list);

    findarg[findarg_len] = cc;

    પાછા ret_fname;
}

/*
 * Process the 'findfunc' option value.
 * Returns NULL on success and an error message on failure.
 */
    અક્ષર *
did_set_findfunc(optset_T *args UNUSED)
{
    પૂર્ણાંક	retval;

    જો (args->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_ffu, &curbuf->b_ffu_cb);
    નહીંતર
    {
	// global option set
	retval = option_set_callback_func(p_ffu, &ffu_cb);
	// when using :set, free the local callback
	જો (!(args->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_ffu_cb);
    }

    જો (retval == FAIL)
	પાછા e_invalid_argument;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*name = get_scriptlocal_funcname(*varp);
    જો (name != ખાલી)
    {
	free_string_option(*varp);
	*varp = name;
    }

    પાછા ખાલી;
}

# જો defined(EXITFREE)
    શૂન્ય
free_findfunc_option(શૂન્ય)
{
    free_callback(&ffu_cb);
}
# endif

/*
 * Mark the global 'findfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    પૂર્ણાંક
set_ref_in_findfunc(પૂર્ણાંક copyID UNUSED)
{
    પૂર્ણાંક abort = FALSE;
    abort = set_ref_in_callback(&ffu_cb, copyID);
    પાછા abort;
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
    શૂન્ય
ex_splitview(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    char_u	*fname = ખાલી;
#ifdef FEAT_BROWSE
    char_u	dot_path[] = ".";
    પૂર્ણાંક		save_cmod_flags = cmdmod.cmod_flags;
#endif
    પૂર્ણાંક		use_tab = eap->cmdidx == CMD_tabedit
		       || eap->cmdidx == CMD_tabfind
		       || eap->cmdidx == CMD_tabnew;

    જો (ERROR_IF_ANY_POPUP_WINDOW)
	પાછા;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#ifdef FEAT_QUICKFIX
    // A ":split" in the quickfix window works like ":new".  Don't want two
    // quickfix windows.  But it's OK when doing ":tab split".
    જો (bt_quickfix(curbuf) && cmdmod.cmod_tab == 0)
    {
	જો (eap->cmdidx == CMD_split)
	    eap->cmdidx = CMD_new;
	જો (eap->cmdidx == CMD_vsplit)
	    eap->cmdidx = CMD_vnew;
    }
#endif

    જો (eap->cmdidx == CMD_sfind || eap->cmdidx == CMD_tabfind)
    {
	char_u	*file_to_find = ખાલી;
	અક્ષર	*શોધ_ctx = ખાલી;

	જો (*get_findfunc() != NUL)
	{
#ifdef FEAT_EVAL
	    fname = findfunc_find_file(eap->arg, (પૂર્ણાંક)STRLEN(eap->arg),
				       eap->addr_ગણતરી > 0 ? eap->line2 : 1);
#endif
	}
	નહીંતર
	{
	    fname = find_file_in_path(eap->arg, (પૂર્ણાંક)STRLEN(eap->arg),
					      FNAME_MESS, TRUE, curbuf->b_ffname,
					      &file_to_find, &શોધ_ctx);
	    vim_free(file_to_find);
	    vim_findfile_cleanup(શોધ_ctx);
	}
	જો (fname == ખાલી)
	    જાઓ theend;
	eap->arg = fname;
    }
#ifdef FEAT_BROWSE
    નહીંતર જો ((cmdmod.cmod_flags & CMOD_BROWSE)
	    && eap->cmdidx != CMD_vnew
	    && eap->cmdidx != CMD_new)
    {
	જો (
# ifdef FEAT_GUI
	    !gui.in_use &&
# endif
		au_has_group((char_u *)"FileExplorer"))
	{
	    // No browsing supported but we do have the file explorer:
	    // Edit the directory.
	    જો (*eap->arg == NUL || !mch_isdir(eap->arg))
		eap->arg = dot_path;
	}
	નહીંતર
	{
	    fname = do_browse(0, (char_u *)(use_tab
			? _("Edit File in new tab page")
			: _("Edit File in new window")),
					  eap->arg, ખાલી, ખાલી, ખાલી, curbuf);
	    જો (fname == ખાલી)
		જાઓ theend;
	    eap->arg = fname;
	}
    }
    cmdmod.cmod_flags &= ~CMOD_BROWSE;	// Don't browse again in do_ecmd().
#endif

    /*
     * Either open new tab page or split the window.
     */
    જો (use_tab)
    {
	જો (win_new_tabpage(cmdmod.cmod_tab != 0 ? cmdmod.cmod_tab
			 : eap->addr_ગણતરી == 0 ? 0
					       : (પૂર્ણાંક)eap->line2 + 1) != FAIL)
	{
	    do_exedit(eap, old_curwin);

	    // set the alternate buffer for the window we came from
	    જો (curwin != old_curwin
		    && win_valid(old_curwin)
		    && old_curwin->w_buffer != curbuf
		    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		old_curwin->w_alt_fnum = curbuf->b_fnum;
	}
    }
    નહીંતર જો (win_split(eap->addr_ગણતરી > 0 ? (પૂર્ણાંક)eap->line2 : 0,
				     *eap->cmd == 'v' ? WSP_VERT : 0) != FAIL)
    {
	// Reset 'scrollbind' when editing another file, but keep it when
	// doing ":split" without arguments.
	જો (*eap->arg != NUL)
	    RESET_BINDING(curwin);
	નહીંતર
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
    શૂન્ય
tabpage_new(શૂન્ય)
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
    સ્થિર શૂન્ય
ex_tabnext(exarg_T *eap)
{
    પૂર્ણાંક tab_સંખ્યા;

    જો (ERROR_IF_POPUP_WINDOW)
	પાછા;
    પસંદગી (eap->cmdidx)
    {
	સ્થિતિ CMD_tabfirst:
	સ્થિતિ CMD_tabrewind:
	    goto_tabpage(1);
	    તોડો;
	સ્થિતિ CMD_tablast:
	    goto_tabpage(9999);
	    તોડો;
	સ્થિતિ CMD_tabprevious:
	સ્થિતિ CMD_tabNext:
	    જો (eap->arg && *eap->arg != NUL)
	    {
		char_u *p = eap->arg;
		char_u *p_save = p;

		tab_સંખ્યા = getdigits(&p);
		જો (p == p_save || *p_save == '-' || *p != NUL
			    || tab_સંખ્યા == 0)
		{
		    // No numbers as argument.
		    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		    પાછા;
		}
	    }
	    નહીંતર
	    {
		જો (eap->addr_ગણતરી == 0)
		    tab_સંખ્યા = 1;
		નહીંતર
		{
		    tab_સંખ્યા = eap->line2;
		    જો (tab_સંખ્યા < 1)
		    {
			eap->errmsg = _(e_invalid_range);
			પાછા;
		    }
		}
	    }
	    goto_tabpage(-tab_સંખ્યા);
	    તોડો;
	મૂળભૂત: // CMD_tabnext
	    tab_સંખ્યા = get_tabpage_arg(eap);
	    જો (eap->errmsg == ખાલી)
		goto_tabpage(tab_સંખ્યા);
	    તોડો;
    }
}

/*
 * :tabmove command
 */
    સ્થિર શૂન્ય
ex_tabmove(exarg_T *eap)
{
    પૂર્ણાંક tab_સંખ્યા;

    tab_સંખ્યા = get_tabpage_arg(eap);
    જો (eap->errmsg == ખાલી)
	tabpage_move(tab_સંખ્યા);
}

/*
 * :tabs command: List tabs and their contents.
 */
    સ્થિર શૂન્ય
ex_tabs(exarg_T *eap UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp;
    પૂર્ણાંક		tabcount = 1;

    સંદેશ_start();
    સંદેશ_scroll = TRUE;
    માટે (tp = first_tabpage; tp != ખાલી && !got_int; tp = tp->tp_next)
    {
	સંદેશ_putchar('\n');
	vim_snprintf((અક્ષર *)IObuff, IOSIZE, _("Tab page %d"), tabcount++);
	સંદેશ_outtrans_attr(IObuff, HL_ATTR(HLF_T));
	out_flush();	    // output one line at a time
	ui_breakcheck();

	જો (tp  == curtab)
	    wp = firstwin;
	નહીંતર
	    wp = tp->tp_firstwin;
	માટે ( ; wp != ખાલી && !got_int; wp = wp->w_next)
	{
	    સંદેશ_putchar('\n');
	    સંદેશ_putchar(wp == curwin ? '>' : ' ');
	    સંદેશ_putchar(' ');
	    સંદેશ_putchar(bufIsChanged(wp->w_buffer) ? '+' : ' ');
	    સંદેશ_putchar(' ');
	    જો (buf_spname(wp->w_buffer) != ખાલી)
		vim_strncpy(IObuff, buf_spname(wp->w_buffer), IOSIZE - 1);
	    નહીંતર
		home_replace(wp->w_buffer, wp->w_buffer->b_fname,
							IObuff, IOSIZE, TRUE);
	    સંદેશ_outtrans(IObuff);
	    out_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }
}

/*
 * ":mode": Set screen mode.
 * If no argument given, just get the screen size and redraw.
 */
    સ્થિર શૂન્ય
ex_mode(exarg_T *eap)
{
    જો (*eap->arg == NUL)
	shell_resized();
    નહીંતર
	emsg(_(e_screen_mode_setting_not_supported));
}

/*
 * ":resize".
 * set, increment or decrement current window height
 */
    સ્થિર શૂન્ય
ex_resize(exarg_T *eap)
{
    પૂર્ણાંક		n;
    win_T	*wp = curwin;

    જો (eap->addr_ગણતરી > 0)
    {
	n = eap->line2;
	માટે (wp = firstwin; wp->w_next != ખાલી && --n > 0; wp = wp->w_next)
	    ;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    n = atol((અક્ષર *)eap->arg);
    જો (cmdmod.cmod_split & WSP_VERT)
    {
	જો (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_width;
	નહીંતર જો (n == 0 && eap->arg[0] == NUL)	// default is very wide
	    n = 9999;
	win_setwidth_win(n, wp);
    }
    નહીંતર
    {
	જો (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_height;
	નહીંતર જો (n == 0 && eap->arg[0] == NUL)	// default is very high
	    n = 9999;
	win_setheight_win(n, wp);
    }
}

/*
 * ":find [+command] <file>" command.
 */
    સ્થિર શૂન્ય
ex_find(exarg_T *eap)
{
    જો (!check_can_set_curbuf_forceit(eap->forceit))
	પાછા;

    char_u	*fname = ખાલી;
    પૂર્ણાંક		ગણતરી;
    char_u	*file_to_find = ખાલી;
    અક્ષર	*શોધ_ctx = ખાલી;

    જો (*get_findfunc() != NUL)
    {
#ifdef FEAT_EVAL
	fname = findfunc_find_file(eap->arg, (પૂર્ણાંક)STRLEN(eap->arg),
					eap->addr_ગણતરી > 0 ? eap->line2 : 1);
#endif
    }
    નહીંતર
    {
	fname = find_file_in_path(eap->arg, (પૂર્ણાંક)STRLEN(eap->arg), FNAME_MESS,
			       TRUE, curbuf->b_ffname, &file_to_find, &શોધ_ctx);
	જો (eap->addr_ગણતરી > 0)
	{
	    // Repeat finding the file "count" times.  This matters when it appears
	    // several times in the path.
	    ગણતરી = eap->line2;
	    જ્યારે (fname != ખાલી && --ગણતરી > 0)
	    {
		vim_free(fname);
		fname = find_file_in_path(ખાલી, 0, FNAME_MESS,
			      FALSE, curbuf->b_ffname, &file_to_find, &શોધ_ctx);
	    }
	}
	VIM_CLEAR(file_to_find);
	vim_findfile_cleanup(શોધ_ctx);
    }

    જો (fname == ખાલી)
	પાછા;

    eap->arg = fname;
    do_exedit(eap, ખાલી);
    vim_free(fname);
}

/*
 * ":open" simulation: for now works just like ":visual".
 */
    સ્થિર શૂન્ય
ex_open(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*p;

#ifdef FEAT_EVAL
    જો (not_in_vim9(eap, false) == FAIL)
	પાછા;
#endif
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_SOL | BL_FIX);
    જો (*eap->arg == '/')
    {
	// ":open /pattern/": put cursor in column found with pattern
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg, magic_isset() ? RE_MAGIC : 0);
	જો (regmatch.regprog != ખાલી)
	{
	    // make a copy of the line, when searching for a mark it might be
	    // flushed
	    char_u *લીટી = vim_strsave(ml_get_curline());

	    regmatch.rm_ic = p_ic;
	    જો (vim_regexec(&regmatch, લીટી, (colnr_T)0))
		curwin->w_cursor.col = (colnr_T)(regmatch.startp[0] - લીટી);
	    નહીંતર
		emsg(_(e_no_match));
	    vim_regfree(regmatch.regprog);
	    vim_free(લીટી);
	}
	// Move to the NUL, ignore any other arguments.
	eap->arg += STRLEN(eap->arg);
    }
    check_cursor();

    eap->cmdidx = CMD_visual;
    do_exedit(eap, ખાલી);
}

/*
 * ":edit", ":badd", ":balt", ":visual".
 */
    સ્થિર શૂન્ય
ex_edit(exarg_T *eap)
{
    char_u *ffname = eap->cmdidx == CMD_enew ? ખાલી : eap->arg;

    // Exclude commands which keep the window's current buffer
    જો (
	    eap->cmdidx != CMD_badd
	    && eap->cmdidx != CMD_balt
	    // All other commands must obey 'winfixbuf' / ! rules
	    && (is_other_file(0, ffname) && !check_can_set_curbuf_forceit(eap->forceit))
    )
	પાછા;

    do_exedit(eap, ખાલી);
}

/*
 * ":edit <file>" command and alike.
 */
    શૂન્ય
do_exedit(
    exarg_T	*eap,
    win_T	*old_curwin)	    // curwin before doing a split or NULL
{
    પૂર્ણાંક		n;
    પૂર્ણાંક		need_hide;
    પૂર્ણાંક		exmode_was = exmode_active;

    જો ((eap->cmdidx != CMD_pedit && ERROR_IF_POPUP_WINDOW)
						 || ERROR_IF_TERM_POPUP_WINDOW)
	પાછા;
    /*
     * ":vi" command ends Ex mode.
     */
    જો (exmode_active && (eap->cmdidx == CMD_visual
						|| eap->cmdidx == CMD_view))
    {
	exmode_active = FALSE;
	ex_pressedreturn = FALSE;
	જો (*eap->arg == NUL)
	{
	    // Special case:  ":global/pat/visual\NLvi-commands"
	    જો (global_busy)
	    {
		જો (eap->nextcmd != ખાલી)
		{
		    stuffReadbuff(eap->nextcmd);
		    eap->nextcmd = ખાલી;
		}

		જો (exmode_was != EXMODE_VIM)
		    settmode(TMODE_RAW);
		પૂર્ણાંક save_RedrawingDisabled = RedrawingDisabled;
		RedrawingDisabled = 0;
		પૂર્ણાંક save_nwr = no_wait_return;
		no_wait_return = 0;
		need_wait_return = FALSE;
		પૂર્ણાંક save_ms = સંદેશ_scroll;
		સંદેશ_scroll = 0;
#ifdef FEAT_GUI
		પૂર્ણાંક save_he = hold_gui_events;
		hold_gui_events = 0;
#endif
		set_must_redraw(UPD_CLEAR);
		pending_exmode_active = TRUE;

		main_loop(FALSE, TRUE);

		pending_exmode_active = FALSE;
		RedrawingDisabled = save_RedrawingDisabled;
		no_wait_return = save_nwr;
		સંદેશ_scroll = save_ms;
#ifdef FEAT_GUI
		hold_gui_events = save_he;
#endif
	    }
	    પાછા;
	}
    }

    જો ((eap->cmdidx == CMD_new
		|| eap->cmdidx == CMD_tabnew
		|| eap->cmdidx == CMD_tabedit
		|| eap->cmdidx == CMD_vnew) && *eap->arg == NUL)
    {
	// ":new" or ":tabnew" without argument: edit a new empty buffer
	setpcmark();
	(શૂન્ય)do_ecmd(0, ખાલી, ખાલી, eap, ECMD_ONE,
		      ECMD_HIDE + (eap->forceit ? ECMD_FORCEIT : 0),
		      old_curwin == ખાલી ? curwin : ખાલી);
    }
    નહીંતર જો ((eap->cmdidx != CMD_split && eap->cmdidx != CMD_vsplit)
	    || *eap->arg != NUL
#ifdef FEAT_BROWSE
	    || (cmdmod.cmod_flags & CMOD_BROWSE)
#endif
	    )
    {
	// Can't edit another file when "textlock" or "curbuf_lock" is set.
	// Only ":edit" or ":script" can bring us here, others are stopped
	// earlier.
	જો (*eap->arg != NUL && text_or_buf_locked())
	    પાછા;

	n = readonlymode;
	જો (eap->cmdidx == CMD_view || eap->cmdidx == CMD_sview)
	    readonlymode = TRUE;
	નહીંતર જો (eap->cmdidx == CMD_enew)
	    readonlymode = FALSE;   // 'readonly' doesn't make sense in an
				    // empty buffer
	જો (eap->cmdidx != CMD_balt && eap->cmdidx != CMD_badd)
	    setpcmark();
	જો (do_ecmd(0, (eap->cmdidx == CMD_enew ? ખાલી : eap->arg),
		    ખાલી, eap,
		    // ":edit" goes to first line if Vi compatible
		    (*eap->arg == NUL && eap->do_ecmd_lnum == 0
				      && vim_strchr(p_cpo, CPO_GOTO1) != ખાલી)
					       ? ECMD_ONE : eap->do_ecmd_lnum,
		    (buf_hide(curbuf) ? ECMD_HIDE : 0)
		    + (eap->forceit ? ECMD_FORCEIT : 0)
		      // after a split we can use an existing buffer
		    + (old_curwin != ખાલી ? ECMD_OLDBUF : 0)
		    + (eap->cmdidx == CMD_badd ? ECMD_ADDBUF : 0)
		    + (eap->cmdidx == CMD_balt ? ECMD_ALTBUF : 0)
		    , old_curwin == ખાલી ? curwin : ખાલી) == FAIL)
	{
	    // Editing the file failed.  If the window was split, close it.
	    જો (old_curwin != ખાલી)
	    {
		need_hide = (curbufIsChanged() && curbuf->b_nwindows <= 1);
		જો (!need_hide || buf_hide(curbuf))
		{
#જો defined(FEAT_EVAL)
		    cleanup_T   cs;

		    // Reset the error/interrupt/exception state here so that
		    // aborting() returns FALSE when closing a window.
		    enter_cleanup(&cs);
#endif
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_close(curwin, !need_hide && !buf_hide(curbuf));

#જો defined(FEAT_EVAL)
		    // Restore the error/interrupt/exception state if not
		    // discarded by a new aborting error, interrupt, or
		    // uncaught exception.
		    leave_cleanup(&cs);
#endif
		}
	    }
	}
	નહીંતર જો (readonlymode && curbuf->b_nwindows == 1)
	{
	    // When editing an already visited buffer, 'readonly' won't be set
	    // but the previous value is kept.  With ":view" and ":sview" we
	    // want the  file to be readonly, except when another window is
	    // editing the same buffer.
	    curbuf->b_p_ro = TRUE;
	}
	readonlymode = n;
    }
    નહીંતર
    {
	જો (eap->do_ecmd_cmd != ખાલી)
	    do_cmd_argument(eap->do_ecmd_cmd);
	n = curwin->w_arg_idx_invalid;
	check_arg_idx(curwin);
	જો (n != curwin->w_arg_idx_invalid)
	    maketitle();
    }

    /*
     * if ":split file" worked, set alternate file name in old window to new
     * file
     */
    જો (old_curwin != ખાલી
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
    સ્થિર શૂન્ય
ex_nogui(exarg_T *eap)
{
    eap->errmsg = _(e_gui_cannot_be_used_not_enabled_at_compile_time);
}
#endif

#જો defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
    સ્થિર શૂન્ય
ex_tearoff(exarg_T *eap)
{
    gui_make_tearoff(eap->arg);
}
#endif

#જો (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
    સ્થિર શૂન્ય
ex_popup(exarg_T *eap)
{
# જો defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    જો (gui.in_use)
	gui_make_popup(eap->arg, eap->forceit);
#  ifdef FEAT_TERM_POPUP_MENU
    નહીંતર
#  endif
# endif
# ifdef FEAT_TERM_POPUP_MENU
	pum_make_popup(eap->arg, eap->forceit);
# endif
}
#endif

    સ્થિર શૂન્ય
ex_swapname(exarg_T *eap UNUSED)
{
    જો (curbuf->b_ml.ml_mfp == ખાલી || curbuf->b_ml.ml_mfp->mf_fname == ખાલી)
	સંદેશ(_("No swap file"));
    નહીંતર
	સંદેશ((અક્ષર *)curbuf->b_ml.ml_mfp->mf_fname);
}

/*
 * ":syncbind" forces all 'scrollbind' windows to have the same relative
 * offset.
 * (1998-11-02 16:21:01  R. Edward Ralston <eralston@computer.org>)
 */
    સ્થિર શૂન્ય
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
    જો (curwin->w_p_scb)
    {
	topline = curwin->w_topline;
	FOR_ALL_WINDOWS(wp)
	{
	    જો (wp->w_p_scb && wp->w_buffer)
	    {
		y = wp->w_buffer->b_ml.ml_લીટી_ગણતરી - get_scrolloff_કિંમત();
		જો (topline > y)
		    topline = y;
	    }
	}
	જો (topline < 1)
	    topline = 1;
    }
    નહીંતર
    {
	topline = 1;
    }


    /*
     * Set all scrollbind windows to the same topline.
     */
    FOR_ALL_WINDOWS(curwin)
    {
	જો (curwin->w_p_scb)
	{
	    curbuf = curwin->w_buffer;
	    y = topline - curwin->w_topline;
	    જો (y > 0)
		scrollup(y, TRUE);
	    નહીંતર
		scrolldown(-y, TRUE);
	    curwin->w_scbind_pos = topline;
	    redraw_later(UPD_VALID);
	    cursor_correct();
	    curwin->w_redr_status = true;
	}
    }
    curwin = save_curwin;
    curbuf = save_curbuf;
    જો (curwin->w_p_scb)
    {
	did_syncbind = TRUE;
	checkpcmark();
	જો (old_linenr != curwin->w_cursor.lnum)
	{
	    char_u ctrl_o[2];

	    ctrl_o[0] = Ctrl_O;
	    ctrl_o[1] = 0;
	    ins_typebuf(ctrl_o, REMAP_NONE, 0, TRUE, FALSE);
	}
    }
}


    સ્થિર શૂન્ય
ex_read(exarg_T *eap)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum;

    જો (eap->usefilter)			// :r!cmd
    {
	do_bang(1, eap, FALSE, FALSE, TRUE);
	પાછા;
    }

    જો (u_save(eap->line2, (linenr_T)(eap->line2 + 1)) == FAIL)
	પાછા;

#ifdef FEAT_BROWSE
    જો (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browseFile;

	browseFile = do_browse(0, (char_u *)_("Append File"), eap->arg,
		ખાલી, ખાલી, ખાલી, curbuf);
	જો (browseFile != ખાલી)
	{
	    i = readfile(browseFile, ખાલી,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	    vim_free(browseFile);
	}
	નહીંતર
	    i = OK;
    }
    નહીંતર
#endif
	જો (*eap->arg == NUL)
	{
	    જો (check_fname() == FAIL)	// check for no file name
		પાછા;
	    i = readfile(curbuf->b_ffname, curbuf->b_fname,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	}
	નહીંતર
	{
	    જો (vim_strchr(p_cpo, CPO_ALTREAD) != ખાલી)
		(શૂન્ય)setaltfname(eap->arg, eap->arg, (linenr_T)1);
	    i = readfile(eap->arg, ખાલી,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);

	}
    જો (i != OK)
    {
#જો defined(FEAT_EVAL)
	જો (!aborting())
#endif
	    semsg(_(e_cant_open_file_str), eap->arg);
    }
    નહીંતર
    {
	જો (empty && exmode_active)
	{
	    // Delete the empty line that remains.  Historically ex does
	    // this but vi doesn't.
	    જો (eap->line2 == 0)
		lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
	    નહીંતર
		lnum = 1;
	    જો (*ml_get(lnum) == NUL && u_savedel(lnum, 1L) == OK)
	    {
		ml_delete(lnum);
		જો (curwin->w_cursor.lnum > 1
			&& curwin->w_cursor.lnum >= lnum)
		    --curwin->w_cursor.lnum;
		deleted_લીટીઓ_mark(lnum, 1L);
	    }
	}
	redraw_curbuf_later(UPD_VALID);
    }
}

સ્થિર char_u	*prev_dir = ખાલી;

#જો defined(EXITFREE)
    શૂન્ય
free_cd_dir(શૂન્ય)
{
    VIM_CLEAR(prev_dir);
    VIM_CLEAR(globaldir);
}
#endif

/*
 * Get the previous directory for the given chdir scope.
 */
    સ્થિર char_u *
get_prevdir(cdscope_T scope)
{
    જો (scope == CDSCOPE_WINDOW)
	પાછા curwin->w_prevdir;
    નહીંતર જો (scope == CDSCOPE_TABPAGE)
	પાછા curtab->tp_prevdir;
    પાછા prev_dir;
}

/*
 * Deal with the side effects of changing the current directory.
 * When 'scope' is CDSCOPE_TABPAGE then this was after an ":tcd" command.
 * When 'scope' is CDSCOPE_WINDOW then this was after an ":lcd" command.
 */
    શૂન્ય
post_chdir(cdscope_T scope)
{
    જો (scope != CDSCOPE_WINDOW)
	// Clear tab local directory for both :cd and :tcd
	VIM_CLEAR(curtab->tp_localdir);
    VIM_CLEAR(curwin->w_localdir);
    જો (scope != CDSCOPE_GLOBAL)
    {
	char_u	*pdir = get_prevdir(scope);

	// If still in the global directory, need to remember current
	// directory as the global directory.
	જો (globaldir == ખાલી && pdir != ખાલી)
	    globaldir = vim_strsave(pdir);

	// Remember this local directory for the window.
	જો (mch_dirname(NameBuff, MAXPATHL) == OK)
	{
	    જો (scope == CDSCOPE_TABPAGE)
		curtab->tp_localdir = vim_strsave(NameBuff);
	    નહીંતર
		curwin->w_localdir = vim_strsave(NameBuff);
	}
    }
    નહીંતર
    {
	// We are now in the global directory, no need to remember its name.
	VIM_CLEAR(globaldir);
    }

    last_chdir_reason = ખાલી;
    shorten_fnames(vim_strchr(p_cpo, CPO_NOSYMLINKS) == ખાલી);
}

/*
 * Trigger DirChangedPre for "acmd_fname" with directory "new_dir".
 */
    શૂન્ય
trigger_DirChangedPre(char_u *acmd_fname, char_u *new_dir)
{
#ifdef FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;

    v_event = get_v_event(&save_v_event);
    (શૂન્ય)dict_add_string(v_event, "directory", new_dir);
    dict_set_વસ્તુઓ_ro(v_event);
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
    પૂર્ણાંક
changedir_func(
	char_u		*new_dir,
	પૂર્ણાંક		forceit,
	cdscope_T	scope)
{
    char_u	*pdir = ખાલી;
    પૂર્ણાંક		dir_differs;
    char_u	*acmd_fname = ખાલી;
    char_u	**pp;
    char_u	*tofree;

    જો (new_dir == ખાલી || allbuf_locked())
	પાછા FALSE;

    જો (vim_strchr(p_cpo, CPO_CHDIR) != ખાલી && curbufIsChanged() && !forceit)
    {
	emsg(_(e_cannot_change_directory_buffer_is_modified_add_bang_to_override));
	પાછા FALSE;
    }

    // ":cd -": Change to previous directory
    જો (STRCMP(new_dir, "-") == 0)
    {
	pdir = get_prevdir(scope);
	જો (pdir == ખાલી)
	{
	    emsg(_(e_no_previous_directory));
	    પાછા FALSE;
	}
	new_dir = pdir;
    }

    // Save current directory for next ":cd -"
    જો (mch_dirname(NameBuff, MAXPATHL) == OK)
	pdir = vim_strsave(NameBuff);
    નહીંતર
	pdir = ખાલી;

    // For UNIX ":cd" means: go to home directory.
    // On other systems too if 'cdhome' is set.
#જો defined(UNIX) || defined(VMS)
    જો (*new_dir == NUL)
#નહીંતર
    જો (*new_dir == NUL && p_cdh)
#endif
    {
	// use NameBuff for home directory name
#ifdef VMS
	char_u	*p;

	p = mch_getenv((char_u *)"SYS$LOGIN");
	જો (p == ખાલી || *p == NUL)	// empty is the same as not set
	    NameBuff[0] = NUL;
	નહીંતર
	    vim_strncpy(NameBuff, p, MAXPATHL - 1);
#નહીંતર
	expand_env((char_u *)"$HOME", NameBuff, MAXPATHL);
#endif
	new_dir = NameBuff;
    }
    dir_differs = pdir == ખાલી
			    || pathcmp((અક્ષર *)pdir, (અક્ષર *)new_dir, -1) != 0;
    જો (dir_differs)
    {
	જો (scope == CDSCOPE_WINDOW)
	    acmd_fname = (char_u *)"window";
	નહીંતર જો (scope == CDSCOPE_TABPAGE)
	    acmd_fname = (char_u *)"tabpage";
	નહીંતર
	    acmd_fname = (char_u *)"global";
	trigger_DirChangedPre(acmd_fname, new_dir);

	જો (vim_chdir(new_dir))
	{
	    emsg(_(e_command_failed));
	    vim_free(pdir);
	    પાછા FALSE;
	}
    }

    જો (scope == CDSCOPE_WINDOW)
	pp = &curwin->w_prevdir;
    નહીંતર જો (scope == CDSCOPE_TABPAGE)
	pp = &curtab->tp_prevdir;
    નહીંતર
	pp = &prev_dir;
    tofree = *pp;  // new_dir may use this
    *pp = pdir;

    post_chdir(scope);

    જો (dir_differs)
	apply_autocmds(EVENT_DIRCHANGED, acmd_fname, new_dir, FALSE, curbuf);
    vim_free(tofree);
    પાછા TRUE;
}

/*
 * ":cd", ":tcd", ":lcd", ":chdir" ":tchdir" and ":lchdir".
 */
    શૂન્ય
ex_cd(exarg_T *eap)
{
    char_u	*new_dir;

    new_dir = eap->arg;
#જો !defined(UNIX) && !defined(VMS)
    // for non-UNIX ":cd" means: print current directory unless 'cdhome' is set
    જો (*new_dir == NUL && !p_cdh)
    {
	ex_pwd(ખાલી);
	પાછા;
    }
#endif

    cdscope_T	scope = CDSCOPE_GLOBAL;

    જો (eap->cmdidx == CMD_lcd || eap->cmdidx == CMD_lchdir)
	scope = CDSCOPE_WINDOW;
    નહીંતર જો (eap->cmdidx == CMD_tcd || eap->cmdidx == CMD_tchdir)
	scope = CDSCOPE_TABPAGE;

    જો (changedir_func(new_dir, eap->forceit, scope))
    {
	// Echo the new current directory if the command was typed.
	જો (ચાવી_Typed || p_verbose >= 5)
	    ex_pwd(eap);
    }
}

/*
 * ":pwd".
 */
    સ્થિર શૂન્ય
ex_pwd(exarg_T *eap UNUSED)
{
    જો (mch_dirname(NameBuff, MAXPATHL) == OK)
    {
#ifdef BACKSLASH_IN_FILENAME
	slash_adjust(NameBuff);
#endif
	જો (p_verbose > 0)
	{
	    અક્ષર *context = "global";

	    જો (last_chdir_reason != ખાલી)
		context = last_chdir_reason;
	    નહીંતર જો (curwin->w_localdir != ખાલી)
		context = "window";
	    નહીંતર જો (curtab->tp_localdir != ખાલી)
		context = "tabpage";
	    smsg("[%s] %s", context, (અક્ષર *)NameBuff);
	}
	નહીંતર
	    સંદેશ((અક્ષર *)NameBuff);
    }
    નહીંતર
	emsg(_(e_directory_unknown));
}

/*
 * ":=".
 */
    સ્થિર શૂન્ય
ex_equal(exarg_T *eap)
{
    smsg("%ld", (long)eap->line2);
    ex_may_print(eap);
}

    સ્થિર શૂન્ય
ex_sleep(exarg_T *eap)
{
    પૂર્ણાંક		n;
    long	len;

    જો (cursor_valid())
    {
	n = W_WINROW(curwin) + curwin->w_wrow - સંદેશ_scrolled;
	જો (n >= 0)
	    windgoto(n, curwin->w_wincol + curwin->w_wcol);
    }

    len = eap->line2;
    પસંદગી (*eap->arg)
    {
	સ્થિતિ 'm': તોડો;
	સ્થિતિ NUL: len *= 1000L; તોડો;
	મૂળભૂત: semsg(_(e_invalid_argument_str), eap->arg); પાછા;
    }

    // Hide the cursor if invoked with !
    do_sleep(len, eap->forceit);
}

/*
 * Sleep for "msec" milliseconds, but keep checking for a CTRL-C every second.
 * Hide the cursor if "hide_cursor" is TRUE.
 */
    શૂન્ય
do_sleep(long msec, પૂર્ણાંક hide_cursor)
{
    long	done = 0;
    long	wait_now;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    // Remember at what time we started, so that we know how much longer we
    // should wait after waiting for a bit.
    ELAPSED_INIT(start_tv);
#endif

    જો (hide_cursor)
	cursor_sleep();
    નહીંતર
	cursor_on();

    out_flush_cursor(FALSE, FALSE);
    જ્યારે (!got_int && done < msec)
    {
	wait_now = msec - done > 1000L ? 1000L : msec - done;
#ifdef FEAT_TIMERS
	{
	    long    due_time = check_due_timer();

	    જો (due_time > 0 && due_time < wait_now)
		wait_now = due_time;
	}
#endif
#ifdef FEAT_JOB_CHANNEL
	જો (has_any_channel() && wait_now > 20L)
	    wait_now = 20L;
#endif
#ifdef FEAT_SOUND
	જો (has_any_sound_callback() && wait_now > 20L)
	    wait_now = 20L;
#endif
	ui_delay(wait_now, TRUE);

#ifdef FEAT_JOB_CHANNEL
	જો (has_any_channel())
	    ui_breakcheck_force(TRUE);
	નહીંતર
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
#નહીંતર
	// guesstimate time passed (will actually be more)
	done += wait_now;
#endif
    }

    // If CTRL-C was typed to interrupt the sleep, drop the CTRL-C from the
    // input buffer, otherwise a following call to input() fails.
    જો (got_int)
	(શૂન્ય)vpeekc();

    જો (hide_cursor)
	cursor_unsleep();
}

/*
 * ":winsize" command (obsolete).
 */
    સ્થિર શૂન્ય
ex_winsize(exarg_T *eap)
{
    પૂર્ણાંક		w, h;
    char_u	*arg = eap->arg;
    char_u	*p;

    જો (!SAFE_isdigit(*arg))
    {
	semsg(_(e_invalid_argument_str), arg);
	પાછા;
    }
    w = getdigits(&arg);
    arg = skipwhite(arg);
    p = arg;
    h = getdigits(&arg);
    જો (*p != NUL && *arg == NUL)
	set_shellsize(w, h, TRUE);
    નહીંતર
	emsg(_(e_winsize_requires_two_સંખ્યા_arguments));
}

    સ્થિર શૂન્ય
ex_wincmd(exarg_T *eap)
{
    પૂર્ણાંક		xchar = NUL;
    char_u	*p;

    જો (*eap->arg == 'g' || *eap->arg == Ctrl_G)
    {
	// CTRL-W g and CTRL-W CTRL-G  have an extra command character
	જો (eap->arg[1] == NUL)
	{
	    emsg(_(e_invalid_argument));
	    પાછા;
	}
	xchar = eap->arg[1];
	p = eap->arg + 2;
    }
    નહીંતર
	p = eap->arg + 1;

    set_nextcmd(eap, p);
    p = skipwhite(p);
    જો (*p != NUL && *p != (
#ifdef FEAT_EVAL
	    in_vim9script() ? '#' :
#endif
		'"')
	    && eap->nextcmd == ખાલી)
	emsg(_(e_invalid_argument));
    નહીંતર જો (!eap->skip)
    {
	// Pass flags on for ":vertical wincmd ]".
	postponed_split_flags = cmdmod.cmod_split;
	postponed_split_tab = cmdmod.cmod_tab;
	do_window(*eap->arg, eap->addr_ગણતરી > 0 ? eap->line2 : 0L, xchar);
	postponed_split_flags = 0;
	postponed_split_tab = 0;
    }
}

#જો defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * ":winpos".
 */
    સ્થિર શૂન્ય
ex_winpos(exarg_T *eap)
{
    પૂર્ણાંક		x, y;
    char_u	*arg = eap->arg;
    char_u	*p;

    જો (*arg == NUL)
    {
# જો defined(FEAT_GUI) || defined(MSWIN)
#  ifdef VIMDLL
	જો (gui.in_use ? gui_mch_get_winpos(&x, &y) != FAIL :
		mch_get_winpos(&x, &y) != FAIL)
#  elif defined(FEAT_GUI)
	જો (gui.in_use && gui_mch_get_winpos(&x, &y) != FAIL)
#  નહીંતર
	જો (mch_get_winpos(&x, &y) != FAIL)
#  endif
	{
	    sprintf((અક્ષર *)IObuff, _("Window position: X %d, Y %d"), x, y);
	    સંદેશ((અક્ષર *)IObuff);
	}
	નહીંતર
# endif
	    emsg(_(e_obtaining_window_position_not_implemented_for_this_platform));
    }
    નહીંતર
    {
	x = getdigits(&arg);
	arg = skipwhite(arg);
	p = arg;
	y = getdigits(&arg);
	જો (*p == NUL || *arg != NUL)
	{
	    emsg(_(e_winpos_requires_two_સંખ્યા_arguments));
	    પાછા;
	}
# ifdef FEAT_GUI
	જો (gui.in_use)
	    gui_mch_set_winpos(x, y);
	નહીંતર જો (gui.starting)
	{
	    // Remember the coordinates for when the window is opened.
	    gui_win_x = x;
	    gui_win_y = y;
	}
#  જો defined(HAVE_TGETENT) || defined(VIMDLL)
	નહીંતર
#  endif
# endif
# જો defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	    mch_set_winpos(x, y);
# endif
# ifdef HAVE_TGETENT
	જો (*T_CWP)
	    term_set_winpos(x, y);
# endif
    }
}
#endif

/*
 * Handle command that work like operators: ":delete", ":yank", ":>" and ":<".
 */
    સ્થિર શૂન્ય
ex_operators(exarg_T *eap)
{
    oparg_T	oa;

    clear_oparg(&oa);
    oa.regname = eap->regname;
    oa.start.lnum = eap->line1;
    oa.end.lnum = eap->line2;
    oa.લીટી_ગણતરી = eap->line2 - eap->line1 + 1;
    oa.motion_type = MLINE;
    virtual_op = FALSE;
    જો (eap->cmdidx != CMD_yank)	// position cursor for undo
    {
	setpcmark();
	curwin->w_cursor.lnum = eap->line1;
	beginline(BL_SOL | BL_FIX);
    }

    જો (VIsual_active)
	end_visual_mode();

    પસંદગી (eap->cmdidx)
    {
	સ્થિતિ CMD_delete:
	    oa.op_type = OP_DELETE;
	    op_delete(&oa);
	    તોડો;

	સ્થિતિ CMD_yank:
	    oa.op_type = OP_YANK;
	    (શૂન્ય)op_yank(&oa, FALSE, TRUE);
	    તોડો;

	મૂળભૂત:    // CMD_rshift or CMD_lshift
	    જો (
#ifdef FEAT_RIGHTLEFT
		(eap->cmdidx == CMD_rshift) ^ curwin->w_p_rl
#નહીંતર
		eap->cmdidx == CMD_rshift
#endif
						)
		oa.op_type = OP_RSHIFT;
	    નહીંતર
		oa.op_type = OP_LSHIFT;
	    op_shift(&oa, FALSE, eap->amount);
	    તોડો;
    }
    virtual_op = MAYBE;
    ex_may_print(eap);
}

/*
 * ":put".
 */
    સ્થિર શૂન્ય
ex_put(exarg_T *eap)
{
    // ":0put" works like ":1put!".
    જો (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, ખાલી, eap->forceit ? BACKWARD : FORWARD, 1L,
						       PUT_લીટી|PUT_CURSLINE);
}

/*
 * ":iput".
 */
    સ્થિર શૂન્ય
ex_iput(exarg_T *eap)
{
    // ":0iput" works like ":1iput!".
    જો (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, ખાલી, eap->forceit ? BACKWARD : FORWARD, 1L,
						      PUT_લીટી|PUT_CURSLINE
						      |PUT_FIXINDENT);
}

/*
 * Handle ":copy" and ":move".
 */
    સ્થિર શૂન્ય
ex_copymove(exarg_T *eap)
{
    long	n;

#ifdef FEAT_EVAL
    જો (not_in_vim9(eap, false) == FAIL)
	પાછા;
#endif
    n = get_address(eap, &eap->arg, eap->addr_type, FALSE, FALSE, FALSE, 1);
    જો (eap->arg == ખાલી)	    // error detected
    {
	eap->nextcmd = ખાલી;
	પાછા;
    }
    get_flags(eap);

    /*
     * move or copy lines from 'eap->line1'-'eap->line2' to below line 'n'
     */
    જો (n == MAXLNUM || n < 0 || n > curbuf->b_ml.ml_લીટી_ગણતરી)
    {
	emsg(_(e_invalid_range));
	પાછા;
    }

    જો (eap->cmdidx == CMD_move)
    {
	જો (do_move(eap->line1, eap->line2, n) == FAIL)
	    પાછા;
    }
    નહીંતર
	ex_copy(eap->line1, eap->line2, n);
    u_clearline();
    beginline(BL_SOL | BL_FIX);
    ex_may_print(eap);
}

/*
 * Print the current line if flags were given to the Ex command.
 */
    શૂન્ય
ex_may_print(exarg_T *eap)
{
    જો (eap->flags != 0)
    {
	print_લીટી(curwin->w_cursor.lnum, (eap->flags & EXFLAG_NR),
						  (eap->flags & EXFLAG_LIST));
	ex_no_reprint = TRUE;
    }
}

/*
 * ":smagic" and ":snomagic".
 */
    સ્થિર શૂન્ય
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
    સ્થિર શૂન્ય
ex_join(exarg_T *eap)
{
    curwin->w_cursor.lnum = eap->line1;
    જો (eap->line1 == eap->line2)
    {
	જો (eap->addr_ગણતરી >= 2)   // :2,2join does nothing
	    પાછા;
	જો (eap->line2 == curbuf->b_ml.ml_લીટી_ગણતરી)
	{
	    beep_flush();
	    પાછા;
	}
	++eap->line2;
    }
    (શૂન્ય)do_join(eap->line2 - eap->line1 + 1, !eap->forceit, TRUE, TRUE, TRUE);
    beginline(BL_WHITE | BL_FIX);
    ex_may_print(eap);
}

/*
 * ":[addr]@r" or ":[addr]*r": execute register
 */
    સ્થિર શૂન્ય
ex_at(exarg_T *eap)
{
    પૂર્ણાંક		c;
    પૂર્ણાંક		prev_len = typebuf.tb_len;

    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif

    // get the register name.  No name means to use the previous one
    c = *eap->arg;
    જો (c == NUL || (c == '*' && *eap->cmd == '*'))
	c = '@';
    // Put the register in the typeahead buffer with the "silent" flag.
    જો (do_execreg(c, TRUE, vim_strchr(p_cpo, CPO_EXECBUF) != ખાલી, TRUE)
								      == FAIL)
    {
	beep_flush();
	પાછા;
    }

    પૂર્ણાંક	save_efr = exec_from_reg;

    exec_from_reg = TRUE;

    /*
     * Execute from the typeahead buffer.
     * Continue until the stuff buffer is empty and all added characters
     * have been consumed.
     */
    જ્યારે (!stuff_empty() || typebuf.tb_len > prev_len)
	(શૂન્ય)do_cmdline(ખાલી, getexline, ખાલી, DOCMD_NOWAIT|DOCMD_VERBOSE);

    exec_from_reg = save_efr;
}

/*
 * ":!".
 */
    સ્થિર શૂન્ય
ex_bang(exarg_T *eap)
{
    do_bang(eap->addr_ગણતરી, eap, eap->forceit, TRUE, TRUE);
}

/*
 * ":undo".
 */
    સ્થિર શૂન્ય
ex_undo(exarg_T *eap)
{
    જો (eap->addr_ગણતરી == 1)	    // :undo 123
	undo_time(eap->line2, FALSE, FALSE, TRUE);
    નહીંતર
	u_undo(1);
}

#ifdef FEAT_PERSISTENT_UNDO
    સ્થિર શૂન્ય
ex_wundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_SIZE];

    u_compute_hash(hash);
    u_write_undo(eap->arg, eap->forceit, curbuf, hash);
}

    સ્થિર શૂન્ય
ex_rundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_SIZE];

    u_compute_hash(hash);
    u_read_undo(eap->arg, hash, ખાલી);
}
#endif

/*
 * ":redo".
 */
    સ્થિર શૂન્ય
ex_redo(exarg_T *eap UNUSED)
{
    u_redo(1);
}

/*
 * ":earlier" and ":later".
 */
    સ્થિર શૂન્ય
ex_later(exarg_T *eap)
{
    long	ગણતરી = 0;
    પૂર્ણાંક		sec = FALSE;
    પૂર્ણાંક		file = FALSE;
    char_u	*p = eap->arg;

    જો (*p == NUL)
	ગણતરી = 1;
    નહીંતર જો (SAFE_isdigit(*p))
    {
	ગણતરી = getdigits(&p);
	પસંદગી (*p)
	{
	    સ્થિતિ 's': ++p; sec = TRUE; તોડો;
	    સ્થિતિ 'm': ++p; sec = TRUE; ગણતરી *= 60; તોડો;
	    સ્થિતિ 'h': ++p; sec = TRUE; ગણતરી *= 60 * 60; તોડો;
	    સ્થિતિ 'd': ++p; sec = TRUE; ગણતરી *= 24 * 60 * 60; તોડો;
	    સ્થિતિ 'f': ++p; file = TRUE; તોડો;
	}
    }

    જો (*p != NUL)
	semsg(_(e_invalid_argument_str), eap->arg);
    નહીંતર
	undo_time(eap->cmdidx == CMD_earlier ? -ગણતરી : ગણતરી,
							    sec, file, FALSE);
}

/*
 * ":redir": start/stop redirection.
 */
    સ્થિર શૂન્ય
ex_redir(exarg_T *eap)
{
    અક્ષર	*mode;
    char_u	*fname;
    char_u	*arg = eap->arg;

#ifdef FEAT_EVAL
    જો (redir_execute)
    {
	emsg(_(e_cannot_use_redir_inside_execute));
	પાછા;
    }
#endif

    જો (STRICMP(eap->arg, "END") == 0)
	close_redir();
    નહીંતર
    {
	જો (*arg == '>')
	{
	    ++arg;
	    જો (*arg == '>')
	    {
		++arg;
		mode = "a";
	    }
	    નહીંતર
		mode = "w";
	    arg = skipwhite(arg);

	    close_redir();

	    // Expand environment variables and "~/".
	    fname = expand_env_save(arg);
	    જો (fname == ખાલી)
		પાછા;
#ifdef FEAT_BROWSE
	    જો (cmdmod.cmod_flags & CMOD_BROWSE)
	    {
		char_u	*browseFile;

		browseFile = do_browse(BROWSE_SAVE,
			(char_u *)_("Save Redirection"),
			fname, ખાલી, ખાલી,
			(char_u *)_(BROWSE_FILTER_ALL_FILES), curbuf);
		જો (browseFile == ખાલી)
		    પાછા;		// operation cancelled
		vim_free(fname);
		fname = browseFile;
		eap->forceit = TRUE;	// since dialog already asked
	    }
#endif

	    redir_fd = open_exfile(fname, eap->forceit, mode);
	    vim_free(fname);
	}
#ifdef FEAT_EVAL
	નહીંતર જો (*arg == '@')
	{
	    // redirect to a register a-z (resp. A-Z for appending)
	    close_redir();
	    ++arg;
	    જો (ASCII_ISALPHA(*arg)
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
		જો (*arg == '>' && arg[1] == '>')  // append
		    arg += 2;
		નહીંતર
		{
		    // Can use both "@a" and "@a>".
		    જો (*arg == '>')
			arg++;
		    // Make register empty when not using @A-@Z and the
		    // command is valid.
		    જો (*arg == NUL && !SAFE_isupper(redir_reg))
			write_reg_contents(redir_reg, (char_u *)"", -1, FALSE);
		}
	    }
	    જો (*arg != NUL)
	    {
		redir_reg = 0;
		semsg(_(e_invalid_argument_str), eap->arg);
	    }
	}
	નહીંતર જો (*arg == '=' && arg[1] == '>')
	{
	    પૂર્ણાંક append;

	    // redirect to a variable
	    close_redir();
	    arg += 2;

	    જો (*arg == '>')
	    {
		++arg;
		append = TRUE;
	    }
	    નહીંતર
		append = FALSE;

	    જો (var_redir_start(skipwhite(arg), append) == OK)
		redir_vname = 1;
	}
#endif

	// TODO: redirect to a buffer

	નહીંતર
	    semsg(_(e_invalid_argument_str), eap->arg);
    }

    // Make sure redirection is not off.  Can happen for cmdline completion
    // that indirectly invokes a command to catch its output.
    જો (redir_fd != ખાલી
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname
#endif
							)
	redir_off = FALSE;
}

/*
 * ":redraw": force redraw, with clear for ":redraw!".
 */
    શૂન્ય
ex_redraw(exarg_T *eap)
{
    redraw_cmd(eap->forceit);
}

/*
 * ":redraw": force redraw, with clear if "clear" is TRUE.
 */
    શૂન્ય
redraw_cmd(પૂર્ણાંક clear)
{
    પૂર્ણાંક save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    પૂર્ણાંક save_p_lz = p_lz;
    p_lz = FALSE;

    validate_cursor();
    update_topline();
    update_screen(clear ? UPD_CLEAR : VIsual_active ? UPD_INVERTED : 0);
    જો ((State & MODE_CMDLINE) == 0)
	setcursor(); // put cursor back where it belongs
    જો (need_maketitle)
	maketitle();
#જો defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
    જો (!gui.in_use)
# endif
	resize_console_buf();
#endif
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;

    // After drawing the statusline screen_attr may still be set.
    screen_stop_highlight();

    // Reset msg_didout, so that a message that's there is overwritten.
    સંદેશ_didout = FALSE;
    સંદેશ_col = 0;

    // No need to wait after an intentional redraw.
    need_wait_return = FALSE;

    // When invoked from a callback or autocmd the command line may be active.
    જો (State & MODE_CMDLINE)
	redrawcmdline();

    out_flush();
}

/*
 * ":redrawstatus": force redraw of status line(s)
 */
    સ્થિર શૂન્ય
ex_redrawstatus(exarg_T *eap UNUSED)
{
    જો (eap->forceit)
	status_redraw_all();
    નહીંતર
	status_redraw_curbuf();
    redraw_vseps = TRUE;
    જો (સંદેશ_scrolled && (State & MODE_CMDLINE))
	પાછા;  // redraw later

    પૂર્ણાંક save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    પૂર્ણાંક save_p_lz = p_lz;
    p_lz = FALSE;

    જો (State & MODE_CMDLINE)
	redraw_statuslines();
    નહીંતર
	update_screen(VIsual_active ? UPD_INVERTED : 0);
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabline": force redraw of the tabline
 */
    સ્થિર શૂન્ય
ex_redrawtabline(exarg_T *eap UNUSED)
{
    પૂર્ણાંક save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    પૂર્ણાંક save_p_lz = p_lz;
    p_lz = FALSE;

    draw_tabline();

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabpanel": force redraw of the tabpanel
 */
    સ્થિર શૂન્ય
ex_redrawtabpanel(exarg_T *eap UNUSED)
{
    પૂર્ણાંક save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    પૂર્ણાંક save_p_lz = p_lz;
    p_lz = FALSE;

#જો defined(FEAT_TABPANEL)
    draw_tabpanel();
#નહીંતર
    emsg(_(e_cannot_not_support_redrawtabpanel));
#endif

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

    સ્થિર શૂન્ય
close_redir(શૂન્ય)
{
    જો (redir_fd != ખાલી)
    {
	fclose(redir_fd);
	redir_fd = ખાલી;
    }
#ifdef FEAT_EVAL
    redir_reg = 0;
    જો (redir_vname)
    {
	var_redir_stop();
	redir_vname = 0;
    }
#endif
}

#જો defined(FEAT_SESSION) || defined(FEAT_EVAL)
    પૂર્ણાંક
vim_mkdir_emsg(char_u *name, પૂર્ણાંક prot UNUSED)
{
    જો (vim_mkdir(name, prot) != 0)
    {
	semsg(_(e_cannot_create_directory_str), name);
	પાછા FAIL;
    }
    પાછા OK;
}
#endif

/*
 * Open a file for writing for an Ex command, with some checks.
 * Return file descriptor, or NULL on failure.
 */
    FILE *
open_exfile(
    char_u	*fname,
    પૂર્ણાંક		forceit,
    અક્ષર	*mode)	    // "w" for create new file or "a" for append
{
    FILE	*fd;

#ifdef UNIX
    // with Unix it is possible to open a directory
    જો (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	પાછા ખાલી;
    }
#endif
    જો (!forceit && *mode != 'a' && vim_fexists(fname))
    {
	semsg(_(e_str_exists_add_bang_to_override), fname);
	પાછા ખાલી;
    }

    જો ((fd = mch_fopen((અક્ષર *)fname, mode)) == ખાલી)
	semsg(_(e_cannot_open_str_for_writing_2), fname);

    પાછા fd;
}

/*
 * ":mark" and ":k".
 */
    સ્થિર શૂન્ય
ex_mark(exarg_T *eap)
{
    pos_T	pos;

#ifdef FEAT_EVAL
    જો (not_in_vim9(eap, false) == FAIL)
	પાછા;
#endif
    જો (*eap->arg == NUL)		// No argument?
    {
	emsg(_(e_argument_required));
	પાછા;
    }

    જો (eap->arg[1] != NUL)	// more than one character?
    {
	semsg(_(e_trailing_characters_str), eap->arg);
	પાછા;
    }

    pos = curwin->w_cursor;		// save curwin->w_cursor
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_WHITE | BL_FIX);
    જો (setmark(*eap->arg) == FAIL)	// set mark
	emsg(_(e_argument_must_be_letter_or_forward_backward_quote));
    curwin->w_cursor = pos;		// restore curwin->w_cursor
}

/*
 * Update w_topline, w_leftcol and the cursor position.
 */
    શૂન્ય
update_topline_cursor(શૂન્ય)
{
    check_cursor();		// put cursor on valid line
    update_topline();
    જો (!curwin->w_p_wrap)
	validate_cursor();
    update_curswant();
}

/*
 * Save the current State and go to Normal mode.
 * Return TRUE if the typeahead could be saved.
 */
    પૂર્ણાંક
save_current_state(save_state_T *sst)
{
    sst->save_સંદેશ_scroll = સંદેશ_scroll;
    sst->save_restart_edit = restart_edit;
    sst->save_સંદેશ_didout = સંદેશ_didout;
    sst->save_State = State;
    sst->save_insertmode = p_im;
    sst->save_finish_op = finish_op;
    sst->save_opcount = opcount;
    sst->save_reg_executing = reg_executing;
    sst->save_pending_end_reg_executing = pending_end_reg_executing;

    સંદેશ_scroll = FALSE;		    // no msg scrolling in Normal mode
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
    પાછા sst->tabuf.typebuf_valid;
}

    શૂન્ય
restore_current_state(save_state_T *sst)
{
    // Restore the previous typeahead.
    restore_typeahead(&sst->tabuf, FALSE);

    સંદેશ_scroll = sst->save_સંદેશ_scroll;
    restart_edit = sst->save_restart_edit;
    p_im = sst->save_insertmode;
    finish_op = sst->save_finish_op;
    opcount = sst->save_opcount;
    reg_executing = sst->save_reg_executing;
    pending_end_reg_executing = sst->save_pending_end_reg_executing;
    સંદેશ_didout |= sst->save_સંદેશ_didout;	// don't reset msg_didout now
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
    શૂન્ય
ex_normal(exarg_T *eap)
{
    save_state_T save_state;
    char_u	*arg = ખાલી;
    પૂર્ણાંક		l;
    char_u	*p;

    જો (ex_normal_lock > 0)
    {
	emsg(_(e_not_allowed_here));
	પાછા;
    }
    જો (ex_normal_busy >= p_mmd)
    {
	emsg(_(e_recursive_use_of_normal_too_deep));
	પાછા;
    }

    /*
     * vgetc() expects a CSI and K_SPECIAL to have been escaped.  Don't do
     * this for the K_SPECIAL leading byte, otherwise special keys will not
     * work.
     */
    જો (has_mbyte)
    {
	પૂર્ણાંક	len = 0;

	// Count the number of characters to be escaped.
	માટે (p = eap->arg; *p != NUL; ++p)
	{
#ifdef FEAT_GUI
	    જો (*p == CSI)  // leadbyte CSI
		len += 2;
#endif
	    માટે (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		જો (*++p == K_SPECIAL	  // trailbyte K_SPECIAL or CSI
#ifdef FEAT_GUI
			|| *p == CSI
#endif
			)
		    len += 2;
	}
	જો (len > 0)
	{
	    arg = alloc(STRLEN(eap->arg) + len + 1);
	    જો (arg != ખાલી)
	    {
		len = 0;
		માટે (p = eap->arg; *p != NUL; ++p)
		{
		    arg[len++] = *p;
#ifdef FEAT_GUI
		    જો (*p == CSI)
		    {
			arg[len++] = KS_EXTRA;
			arg[len++] = (પૂર્ણાંક)KE_CSI;
		    }
#endif
		    માટે (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		    {
			arg[len++] = *++p;
			જો (*p == K_SPECIAL)
			{
			    arg[len++] = KS_SPECIAL;
			    arg[len++] = KE_FILLER;
			}
#ifdef FEAT_GUI
			નહીંતર જો (*p == CSI)
			{
			    arg[len++] = KS_EXTRA;
			    arg[len++] = (પૂર્ણાંક)KE_CSI;
			}
#endif
		    }
		    arg[len] = NUL;
		}
	    }
	}
    }

    ++ex_normal_busy;
    જો (save_current_state(&save_state))
    {
	/*
	 * Repeat the :normal command for each line in the range.  When no
	 * range given, execute it just once, without positioning the cursor
	 * first.
	 */
	કરો
	{
	    જો (eap->addr_ગણતરી != 0)
	    {
		curwin->w_cursor.lnum = eap->line1++;
		curwin->w_cursor.col = 0;
		check_cursor_moved(curwin);
	    }

	    exec_normal_cmd(arg != ખાલી
		     ? arg
		     : eap->arg, eap->forceit ? REMAP_NONE : REMAP_YES, FALSE);
	}
	જ્યારે (eap->addr_ગણતરી > 0 && eap->line1 <= eap->line2 && !got_int);
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
    સ્થિર શૂન્ય
ex_startinsert(exarg_T *eap)
{
#ifdef FEAT_TERMINAL
    // Ignore this when running in an active terminal.
    જો (term_job_running(curbuf->b_term))
	પાછા;
#endif
    જો (!curbuf->b_p_ma && !p_im)
    {
	// Only give this error when 'insertmode' is off.
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	પાછા;
    }
    જો (eap->forceit)
    {
	// cursor line can be zero on startup
	જો (!curwin->w_cursor.lnum)
	    curwin->w_cursor.lnum = 1;
	set_cursor_for_append_to_લીટી();
    }

    // Ignore the command when already in Insert mode.  Inserting an
    // expression register that invokes a function can do this.
    જો (State & MODE_ઉમેરો)
	પાછા;

    જો (eap->cmdidx == CMD_startinsert)
	restart_edit = 'a';
    નહીંતર જો (eap->cmdidx == CMD_startreplace)
	restart_edit = 'R';
    નહીંતર
	restart_edit = 'V';

    જો (!eap->forceit)
    {
	જો (eap->cmdidx == CMD_startinsert)
	    restart_edit = 'i';
	curwin->w_curswant = 0;	    // avoid MAXCOL
    }

    જો (VIsual_active)
	showmode();
}

/*
 * ":stopinsert"
 */
    સ્થિર શૂન્ય
ex_stopinsert(exarg_T *eap UNUSED)
{
    restart_edit = 0;
    stop_ઉમેરો_mode = TRUE;
#જો defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // when called from remote_expr in insert mode, make sure insert mode is
    // ended by adding K_NOP to the typeahead buffer
    જો (vgetc_busy)
	ins_char_typebuf(K_NOP, 0);
#endif
    clearmode();
}

/*
 * Execute normal mode command "cmd".
 * "remap" can be REMAP_NONE or REMAP_YES.
 */
    શૂન્ય
exec_normal_cmd(char_u *cmd, પૂર્ણાંક remap, પૂર્ણાંક silent)
{
    // Stuff the argument into the typeahead buffer.
    ins_typebuf(cmd, remap, 0, TRUE, silent);
    exec_normal(FALSE, FALSE, FALSE);
}

/*
 * Execute normal_cmd() until there is no typeahead left.
 * When "use_vpeekc" is TRUE use vpeekc() to check for available chars.
 */
    શૂન્ય
exec_normal(પૂર્ણાંક was_typed, પૂર્ણાંક use_vpeekc, પૂર્ણાંક may_use_terminal_loop UNUSED)
{
    oparg_T	oa;
    પૂર્ણાંક		c;

    // When calling vpeekc() from feedkeys() it will return Ctrl_C when there
    // is nothing to get, so also check for Ctrl_C.
    clear_oparg(&oa);
    finish_op = FALSE;
    જ્યારે ((!stuff_empty()
		|| ((was_typed || !typebuf_typed()) && typebuf.tb_len > 0)
		|| (use_vpeekc && (c = vpeekc()) != NUL && c != Ctrl_C))
	    && !got_int)
    {
	update_topline_cursor();
#ifdef FEAT_TERMINAL
	જો (may_use_terminal_loop && term_use_loop()
		&& oa.op_type == OP_NOP && oa.regname == NUL
		&& !VIsual_active)
	{
	    // If terminal_loop() returns OK we got a key that is handled
	    // in Normal model.  With FAIL we first need to position the
	    // cursor and the screen needs to be redrawn.
	    જો (terminal_loop(TRUE) == OK)
		normal_cmd(&oa, TRUE);
	}
	નહીંતર
#endif
	    // execute a Normal mode cmd
	    normal_cmd(&oa, TRUE);
    }
}

#ifdef FEAT_FIND_ID
    સ્થિર શૂન્ય
ex_checkpath(exarg_T *eap)
{
    find_pattern_in_path(ખાલી, 0, 0, FALSE, FALSE, CHECK_PATH, 1L,
	    eap->forceit ? ACTION_SHOW_ALL : ACTION_SHOW,
	    (linenr_T)1, (linenr_T)MAXLNUM, eap->forceit, FALSE);
}

# જો defined(FEAT_QUICKFIX)
/*
 * ":psearch"
 */
    સ્થિર શૂન્ય
ex_psearch(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;
    ex_findpat(eap);
    g_do_tagpreview = 0;
}
# endif

    સ્થિર શૂન્ય
ex_findpat(exarg_T *eap)
{
    પૂર્ણાંક		whole = TRUE;
    long	n;
    char_u	*p;
    પૂર્ણાંક		action;

    પસંદગી (cmdnames[eap->cmdidx].cmd_name[2])
    {
	સ્થિતિ 'e':	// ":psearch", ":isearch" and ":dsearch"
		જો (cmdnames[eap->cmdidx].cmd_name[0] == 'p')
		    action = ACTION_GOTO;
		નહીંતર
		    action = ACTION_SHOW;
		તોડો;
	સ્થિતિ 'i':	// ":ilist" and ":dlist"
		action = ACTION_SHOW_ALL;
		તોડો;
	સ્થિતિ 'u':	// ":ijump" and ":djump"
		action = ACTION_GOTO;
		તોડો;
	મૂળભૂત:	// ":isplit" and ":dsplit"
		action = ACTION_SPLIT;
		તોડો;
    }

    n = 1;
    જો (vim_isdigit(*eap->arg))	// get count
    {
	n = getdigits(&eap->arg);
	eap->arg = skipwhite(eap->arg);
    }
    જો (*eap->arg == '/')   // Match regexp, not just whole words
    {
	whole = FALSE;
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	જો (*p)
	{
	    *p++ = NUL;
	    p = skipwhite(p);

	    // Check for trailing illegal characters
	    જો (!ends_excmd2(eap->arg, p))
		eap->errmsg = ex_errmsg(e_trailing_characters_str, p);
	    નહીંતર
		set_nextcmd(eap, p);
	}
    }
    જો (!eap->skip)
	find_pattern_in_path(eap->arg, 0, (પૂર્ણાંક)STRLEN(eap->arg),
		whole, !eap->forceit,
		*eap->cmd == 'd' ? FIND_DEFINE : FIND_ANY, n, action,
		eap->line1, eap->line2, eap->forceit, FALSE);
}
#endif


#ifdef FEAT_QUICKFIX
/*
 * ":ptag", ":ptselect", ":ptjump", ":ptnext", etc.
 */
    સ્થિર શૂન્ય
ex_ptag(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;  // will be reset to 0 in ex_tag_cmd()
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
}

/*
 * ":pedit"
 */
    સ્થિર શૂન્ય
ex_pedit(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Edit the file.
    do_exedit(eap, ખાલી);

    back_to_current_window(curwin_save);
}

/*
 * ":pbuffer"
 */
    સ્થિર શૂન્ય
ex_pbuffer(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Go to the buffer.
    do_exbuffer(eap);

    back_to_current_window(curwin_save);
}

    સ્થિર શૂન્ય
prepare_preview_window(શૂન્ય)
{
    જો (ERROR_IF_ANY_POPUP_WINDOW)
	પાછા;

    // Open the preview window or popup and make it the current window.
    g_do_tagpreview = p_pvh;
    prepare_tagpreview(TRUE, TRUE, FALSE);
}

    સ્થિર શૂન્ય
back_to_current_window(win_T *curwin_save)
{
    જો (curwin != curwin_save && win_valid(curwin_save))
    {
	// Return cursor to where we were
	validate_cursor();
	redraw_later(UPD_VALID);
	win_enter(curwin_save, TRUE);
    }
# ifdef FEAT_PROP_POPUP
    નહીંતર જો (WIN_IS_POPUP(curwin))
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
    સ્થિર શૂન્ય
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
    સ્થિર શૂન્ય
ex_tag(exarg_T *eap)
{
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name);
}

    સ્થિર શૂન્ય
ex_tag_cmd(exarg_T *eap, char_u *name)
{
    પૂર્ણાંક		cmd;

    પસંદગી (name[1])
    {
	સ્થિતિ 'j': cmd = DT_JUMP;	// ":tjump"
		  તોડો;
	સ્થિતિ 's': cmd = DT_SELECT;	// ":tselect"
		  તોડો;
	સ્થિતિ 'p': cmd = DT_PREV;	// ":tprevious"
		  તોડો;
	સ્થિતિ 'N': cmd = DT_PREV;	// ":tNext"
		  તોડો;
	સ્થિતિ 'n': cmd = DT_NEXT;	// ":tnext"
		  તોડો;
	સ્થિતિ 'o': cmd = DT_કાઢો;		// ":pop"
		  તોડો;
	સ્થિતિ 'f':			// ":tfirst"
	સ્થિતિ 'r': cmd = DT_FIRST;	// ":trewind"
		  તોડો;
	સ્થિતિ 'l': cmd = DT_LAST;	// ":tlast"
		  તોડો;
	મૂળભૂત:			// ":tag"
#ifdef FEAT_CSCOPE
		  જો (p_cst && *eap->arg != NUL)
		  {
		      ex_cstag(eap);
		      પાછા;
		  }
#endif
		  cmd = DT_TAG;
		  તોડો;
    }

    જો (name[0] == 'l')
    {
#ifndef FEAT_QUICKFIX
	ex_ni(eap);
	પાછા;
#નહીંતર
	cmd = DT_LTAG;
#endif
    }

    do_tag(eap->arg, cmd, eap->addr_ગણતરી > 0 ? (પૂર્ણાંક)eap->line2 : 1,
							  eap->forceit, TRUE);
}

ગણના {
    SPEC_PERC = 0,
    SPEC_HASH,
    SPEC_CWORD,	    // cursor word
    SPEC_CCWORD,    // cursor WORD
    SPEC_CEXPR,	    // expr under cursor
    SPEC_CFILE,	    // cursor path name
    SPEC_SFILE,	    // ":so" file name
    SPEC_SLNUM,	    // ":so" file line number
    SPEC_ઢગલો,	    // call stack
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
    પૂર્ણાંક
find_cmdline_var(char_u *src, size_t *usedlen)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    સ્થિર keyvalue_T spec_str_tab[] = {
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
	KEYVALUE_ENTRY(SPEC_ઢગલો, "stack>")	    // call stack
    };
    keyvalue_T target;
    keyvalue_T *entry;

    પસંદગી (*src)
    {
    સ્થિતિ '%':
	*usedlen = 1;
	પાછા SPEC_PERC;

    સ્થિતિ '#':
	*usedlen = 1;
	પાછા SPEC_HASH;

    સ્થિતિ '<':
	target.ચાવી = 0;
	target.કિંમત.string = src + 1;	    // skip over '<'
	target.કિંમત.length = 0;	    // not used, see cmp_keyvalue_value_n()

	entry = (keyvalue_T *)bsearch(&target, &spec_str_tab,
		ARRAY_LENGTH(spec_str_tab), કદ(spec_str_tab[0]),
		cmp_keyvalue_કિંમત_n);
	જો (entry == ખાલી)
	    પાછા -1;

	*usedlen = entry->કિંમત.length + 1;
	પાછા entry->ચાવી;

    મૂળભૂત:
	તોડો;
    }

    પાછા -1;
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
    અક્ષર	**errormsg,	// pointer to error message
    પૂર્ણાંક		*escaped,	// return value has escaped white space (can
				// be NULL)
    પૂર્ણાંક		empty_is_error)	// empty result is considered an error
{
    પૂર્ણાંક		i;
    char_u	*s;
    char_u	*result = (char_u *)"";
    char_u	*resultbuf = ખાલી;
    size_t	resultlen;
    buf_T	*buf;
    પૂર્ણાંક		valid = VALID_HEAD + VALID_PATH;    // assume valid result
    પૂર્ણાંક		spec_idx;
    પૂર્ણાંક		tilde_file = FALSE;
    પૂર્ણાંક		skip_mod = FALSE;
    char_u	strbuf[30];

    *errormsg = ખાલી;
    જો (escaped != ખાલી)
	*escaped = FALSE;

    /*
     * Check if there is something to do.
     */
    spec_idx = find_cmdline_var(src, usedlen);
    જો (spec_idx < 0)	// no match
    {
	*usedlen = 1;
	પાછા ખાલી;
    }

    /*
     * Skip when preceded with a backslash "\%" and "\#".
     * Note: In "\\%" the % is also not recognized!
     */
    જો (src > srcstart && src[-1] == '\\')
    {
	*usedlen = 0;
	STRMOVE(src - 1, src);	// remove backslash
	પાછા ખાલી;
    }

    /*
     * word or WORD under cursor
     */
    જો (spec_idx == SPEC_CWORD || spec_idx == SPEC_CCWORD
						     || spec_idx == SPEC_CEXPR)
    {
	resultlen = find_ident_under_cursor(&result,
		spec_idx == SPEC_CWORD ? (FIND_IDENT | FIND_STRING)
	      : spec_idx == SPEC_CEXPR ? (FIND_IDENT | FIND_STRING | FIND_EVAL)
	      : FIND_STRING);
	જો (resultlen == 0)
	{
	    *errormsg = "";
	    પાછા ખાલી;
	}
    }

    /*
     * '#': Alternate file name
     * '%': Current file name
     *	    File name under the cursor
     *	    File name for autocommand
     *	and following modifiers
     */
    નહીંતર
    {
	size_t off = 0;

	પસંદગી (spec_idx)
	{
	સ્થિતિ SPEC_PERC:
#ifdef FEAT_EVAL
		જો (!in_vim9script() || src[1] != '%')
#endif
		{
		    // '%': current file
		    જો (curbuf->b_fname == ખાલી)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    નહીંતર
		    {
			result = curbuf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		    તોડો;
		}
#ifdef FEAT_EVAL
		// "%%" alternate file
		off = 1;
#endif
		// FALLTHROUGH
	સ્થિતિ SPEC_HASH:		// '#' or "#99": alternate file
		જો (off == 0 ? src[1] == '#' : src[2] == '%')
		{
		    // "##" or "%%%": the argument list
		    result = arg_all();
		    resultbuf = result;
		    *usedlen = off + 2;
		    જો (escaped != ખાલી)
			*escaped = TRUE;
		    skip_mod = TRUE;
		    તોડો;
		}
		s = src + off + 1;
		જો (*s == '<')		// "#<99" uses v:oldfiles
		    ++s;
		i = (પૂર્ણાંક)getdigits(&s);
		જો (s == src + off + 2 && src[off + 1] == '-')
		    // just a minus sign, don't skip over it
		    s--;
		*usedlen = (પૂર્ણાંક)(s - src); // length of what we expand

		જો (src[off + 1] == '<' && i != 0)
		{
		    જો (*usedlen < off + 2)
		    {
			// Should we give an error message for #<text?
			*usedlen = off + 1;
			પાછા ખાલી;
		    }
#ifdef FEAT_EVAL
		    result = list_find_str(get_vim_var_list(VV_OLDFILES),
								     (long)i);
		    જો (result == ખાલી)
		    {
			*errormsg = "";
			પાછા ખાલી;
		    }
#નહીંતર
		    *errormsg = _(e_hashsmall_is_not_available_without_the_eval_feature);
		    પાછા ખાલી;
#endif
		}
		નહીંતર
		{
		    જો (i == 0 && src[off + 1] == '<' && *usedlen > off + 1)
			*usedlen = off + 1;
		    buf = buflist_findnr(i);
		    જો (buf == ખાલી)
		    {
			*errormsg = _(e_no_alternate_file_name_to_substitute_for_hash);
			પાછા ખાલી;
		    }
		    જો (lnump != ખાલી)
			*lnump = ECMD_LAST;
		    જો (buf->b_fname == ખાલી)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    નહીંતર
		    {
			result = buf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		}
		તોડો;

	સ્થિતિ SPEC_CFILE:	// file name under cursor
		result = file_name_at_cursor(FNAME_MESS|FNAME_HYP, 1L, ખાલી);
		જો (result == ખાલી)
		{
		    *errormsg = "";
		    પાછા ખાલી;
		}
		resultbuf = result;	    // remember allocated string
		તોડો;

	સ્થિતિ SPEC_AFILE:	// file name for autocommand
		result = autocmd_fname;
		જો (result != ખાલી && !autocmd_fname_full)
		{
		    // Still need to turn the fname into a full path.  It is
		    // postponed to avoid a delay when <afile> is not used.
		    autocmd_fname_full = TRUE;
		    result = FullName_save(autocmd_fname, FALSE);
		    vim_free(autocmd_fname);
		    autocmd_fname = result;
		}
		જો (result == ખાલી)
		{
		    *errormsg = _(e_no_autocommand_file_name_to_substitute_for_afile);
		    પાછા ખાલી;
		}
		result = shorten_fname1(result);
		તોડો;

	સ્થિતિ SPEC_ABUF:		// buffer number for autocommand
		જો (autocmd_bufnr <= 0)
		{
		    *errormsg = _(e_no_autocommand_buffer_સંખ્યા_to_substitute_for_abuf);
		    પાછા ખાલી;
		}
		sprintf((અક્ષર *)strbuf, "%d", autocmd_bufnr);
		result = strbuf;
		તોડો;

	સ્થિતિ SPEC_AMATCH:	// match name for autocommand
		result = autocmd_match;
		જો (result == ખાલી)
		{
		    *errormsg = _(e_no_autocommand_match_name_to_substitute_for_amatch);
		    પાછા ખાલી;
		}
		તોડો;

	સ્થિતિ SPEC_SFILE:	// file name for ":so" command
		result = estack_sfile(ESTACK_SFILE);
		જો (result == ખાલી)
		{
		    *errormsg = _(e_no_source_file_name_to_substitute_for_sfile);
		    પાછા ખાલી;
		}
		resultbuf = result;	    // remember allocated string
		તોડો;
	સ્થિતિ SPEC_ઢગલો:	// call stack
		result = estack_sfile(ESTACK_ઢગલો);
		જો (result == ખાલી)
		{
		    *errormsg = _(e_no_call_ઢગલો_to_substitute_for_ઢગલો);
		    પાછા ખાલી;
		}
		resultbuf = result;	    // remember allocated string
		તોડો;
	સ્થિતિ SPEC_SCRIPT:	// script file name
		result = estack_sfile(ESTACK_SCRIPT);
		જો (result == ખાલી)
		{
		    *errormsg = _(e_no_script_file_name_to_substitute_for_script);
		    પાછા ખાલી;
		}
		resultbuf = result;	    // remember allocated string
		તોડો;

	સ્થિતિ SPEC_SLNUM:	// line in file for ":so" command
		જો (SOURCING_NAME == ખાલી || SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_લીટી_સંખ્યા_to_use_for_slnum);
		    પાછા ખાલી;
		}
		sprintf((અક્ષર *)strbuf, "%ld", SOURCING_LNUM);
		result = strbuf;
		તોડો;

#ifdef FEAT_EVAL
	સ્થિતિ SPEC_SFLNUM:	// line in script file
		જો (current_sctx.sc_lnum + SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_લીટી_સંખ્યા_to_use_for_sflnum);
		    પાછા ખાલી;
		}
		sprintf((અક્ષર *)strbuf, "%ld",
				 (long)(current_sctx.sc_lnum + SOURCING_LNUM));
		result = strbuf;
		તોડો;

	સ્થિતિ SPEC_SID:
		જો (current_sctx.sc_sid <= 0)
		{
		    *errormsg = _(e_using_sid_not_in_script_context);
		    પાછા ખાલી;
		}
		sprintf((અક્ષર *)strbuf, "<SNR>%d_", current_sctx.sc_sid);
		result = strbuf;
		તોડો;
#endif

#ifdef FEAT_CLIENTSERVER
	સ્થિતિ SPEC_CLIENT:	// Source of last submitted input
# ifdef MSWIN
		sprintf((અક્ષર *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		result = strbuf;
# નહીંતર
#  ifdef FEAT_SOCKETSERVER
		જો (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		{
		    જો (client_socket == ખાલી)
			result = (char_u *)"";
		    નહીંતર
			result = client_socket;
		}
#  endif
#  ifdef FEAT_X11
		જો (clientserver_method == CLIENTSERVER_METHOD_X11)
		{
		    sprintf((અક્ષર *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		    result = strbuf;
		}
#  endif
# endif
		તોડો;
#endif
	}

	resultlen = STRLEN(result);	// length of new string
	જો (src[*usedlen] == '<')	// remove the file name extension
	{
	    ++*usedlen;
	    જો ((s = vim_strrchr(result, '.')) != ખાલી && s >= gettail(result))
		resultlen = s - result;
	}
	નહીંતર જો (!skip_mod)
	{
	    valid |= modify_fname(src, tilde_file, usedlen, &result, &resultbuf,
								  &resultlen);
	    જો (result == ખાલી)
	    {
		*errormsg = "";
		પાછા ખાલી;
	    }
	}
    }

    જો (resultlen == 0 || valid != VALID_HEAD + VALID_PATH)
    {
	જો (empty_is_error)
	{
	    જો (valid != VALID_HEAD + VALID_PATH)
		*errormsg = _(e_empty_file_name_for_percent_or_hash_only_works_with_ph);
	    નહીંતર
		*errormsg = _(e_evaluates_to_an_empty_string);
	}
	result = ખાલી;
    }
    નહીંતર
	result = vim_strnsave(result, resultlen);
    vim_free(resultbuf);
    પાછા result;
}

/*
 * Expand the <sfile> string in "arg".
 *
 * Returns an allocated string, or NULL for any error.
 */
    char_u *
expand_sfile(char_u *arg)
{
    અક્ષર	*errormsg;
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
    જો (result == ખાલી)
	પાછા ખાલી;

    માટે (p = result; *p; )
    {
	જો (STRNCMP(p, "<sfile>", 7) != 0)
	    ++p;
	નહીંતર
	{
	    // replace "<sfile>" with the sourced file name, and do ":" stuff
	    repl = eval_vars(p, result, &srclen, ખાલી, &errormsg, ખાલી, TRUE);
	    જો (errormsg != ખાલી)
	    {
		જો (*errormsg)
		    emsg(errormsg);
		vim_free(result);
		પાછા ખાલી;
	    }
	    જો (repl == ખાલી)		// no match (cannot happen)
	    {
		p += srclen;
		ચાલુ;
	    }
	    repllen = STRLEN(repl);
	    resultlen += repllen - srclen;
	    newres = alloc(resultlen + 1);
	    જો (newres == ખાલી)
	    {
		vim_free(repl);
		vim_free(result);
		પાછા ખાલી;
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

    પાછા result;
}

#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
/*
 * Make a dialog message in "buff[DIALOG_MSG_SIZE]".
 * "format" must contain "%s".
 */
    શૂન્ય
dialog_સંદેશ(char_u *buff, અક્ષર *format, char_u *fname)
{
    જો (fname == ખાલી)
	fname = (char_u *)_("Untitled");
    vim_snprintf((અક્ષર *)buff, DIALOG_સંદેશ_SIZE, format, fname);
}
#endif

/*
 * ":behave {mswin,xterm}"
 */
    સ્થિર શૂન્ય
ex_behave(exarg_T *eap)
{
    જો (STRCMP(eap->arg, "mswin") == 0)
    {
	set_option_કિંમત_give_err((char_u *)"selection",
						 0L, (char_u *)"exclusive", 0);
	set_option_કિંમત_give_err((char_u *)"selectmode",
						 0L, (char_u *)"mouse,key", 0);
	set_option_કિંમત_give_err((char_u *)"mousemodel",
						     0L, (char_u *)"popup", 0);
	set_option_કિંમત_give_err((char_u *)"keymodel",
					  0L, (char_u *)"startsel,stopsel", 0);
    }
    નહીંતર જો (STRCMP(eap->arg, "xterm") == 0)
    {
	set_option_કિંમત_give_err((char_u *)"selection",
						 0L, (char_u *)"inclusive", 0);
	set_option_કિંમત_give_err((char_u *)"selectmode", 0L, (char_u *)"", 0);
	set_option_કિંમત_give_err((char_u *)"mousemodel",
						    0L, (char_u *)"extend", 0);
	set_option_કિંમત_give_err((char_u *)"keymodel", 0L, (char_u *)"", 0);
    }
    નહીંતર
	semsg(_(e_invalid_argument_str), eap->arg);
}

સ્થિર પૂર્ણાંક filetype_detect = FALSE;
સ્થિર પૂર્ણાંક filetype_plugin = FALSE;
સ્થિર પૂર્ણાંક filetype_indent = FALSE;

/*
 * ":filetype [plugin] [indent] {on,off,detect}"
 * on: Load the filetype.vim file to install autocommands for file types.
 * off: Load the ftoff.vim file to remove all autocommands for file types.
 * plugin on: load filetype.vim and ftplugin.vim
 * plugin off: load ftplugof.vim
 * indent on: load filetype.vim and indent.vim
 * indent off: load indoff.vim
 */
    સ્થિર શૂન્ય
ex_filetype(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    પૂર્ણાંક		plugin = FALSE;
    પૂર્ણાંક		indent = FALSE;

    જો (*eap->arg == NUL)
    {
	// Print current status.
	smsg("filetype detection:%s  plugin:%s  indent:%s",
		filetype_detect ? "ON" : "OFF",
		filetype_plugin ? (filetype_detect ? "ON" : "(on)") : "OFF",
		filetype_indent ? (filetype_detect ? "ON" : "(on)") : "OFF");
	પાછા;
    }

    // Accept "plugin" and "indent" in any order.
    માટે (;;)
    {
	જો (STRNCMP(arg, "plugin", 6) == 0)
	{
	    plugin = TRUE;
	    arg = skipwhite(arg + 6);
	    ચાલુ;
	}
	જો (STRNCMP(arg, "indent", 6) == 0)
	{
	    indent = TRUE;
	    arg = skipwhite(arg + 6);
	    ચાલુ;
	}
	તોડો;
    }
    જો (STRCMP(arg, "on") == 0 || STRCMP(arg, "detect") == 0)
    {
	જો (*arg == 'o' || !filetype_detect)
	{
	    source_runtime((char_u *)FILETYPE_FILE, DIP_ALL);
	    filetype_detect = TRUE;
	    જો (plugin)
	    {
		source_runtime((char_u *)FTPLUGIN_FILE, DIP_ALL);
		filetype_plugin = TRUE;
	    }
	    જો (indent)
	    {
		source_runtime((char_u *)INDENT_FILE, DIP_ALL);
		filetype_indent = TRUE;
	    }
	}
	જો (*arg == 'd')
	{
	    (શૂન્ય)do_doautocmd((char_u *)"filetypedetect BufRead", TRUE, ખાલી);
	    do_modelines(0);
	}
    }
    નહીંતર જો (STRCMP(arg, "off") == 0)
    {
	જો (plugin || indent)
	{
	    જો (plugin)
	    {
		source_runtime((char_u *)FTPLUGOF_FILE, DIP_ALL);
		filetype_plugin = FALSE;
	    }
	    જો (indent)
	    {
		source_runtime((char_u *)INDOFF_FILE, DIP_ALL);
		filetype_indent = FALSE;
	    }
	}
	નહીંતર
	{
	    source_runtime((char_u *)FTOFF_FILE, DIP_ALL);
	    filetype_detect = FALSE;
	}
    }
    નહીંતર
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * ":setfiletype [FALLBACK] {name}"
 */
    સ્થિર શૂન્ય
ex_setfiletype(exarg_T *eap)
{
    જો (curbuf->b_did_filetype)
	પાછા;

    char_u *arg = eap->arg;
    જો (STRNCMP(arg, "FALLBACK ", 9) == 0)
	arg += 9;

    set_option_કિંમત_give_err((char_u *)"filetype", 0L, arg, OPT_LOCAL);
    જો (arg != eap->arg)
	curbuf->b_did_filetype = false;
}

    સ્થિર શૂન્ય
ex_digraphs(exarg_T *eap UNUSED)
{
#ifdef FEAT_DIGRAPHS
    જો (*eap->arg != NUL)
	putdigraph(eap->arg);
    નહીંતર
	listdigraphs(eap->forceit);
#નહીંતર
    emsg(_(e_no_digraphs_version));
#endif
}

#જો defined(FEAT_શોધ_EXTRA)
    શૂન્ય
set_no_hlsearch(પૂર્ણાંક flag)
{
    no_hlsearch = flag;
# ifdef FEAT_EVAL
    set_vim_var_nr(VV_HLSEARCH, !no_hlsearch && p_hls);
# endif
}

/*
 * ":nohlsearch"
 */
    સ્થિર શૂન્ય
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
    સ્થિર શૂન્ય
ex_X(exarg_T *eap UNUSED)
{
    crypt_check_current_method();
    (શૂન્ય)crypt_get_ચાવી(TRUE, TRUE);
}
#endif

#ifdef FEAT_FOLDING
    સ્થિર શૂન્ય
ex_fold(exarg_T *eap)
{
    જો (foldManualAllowed(TRUE))
	foldCreate(eap->line1, eap->line2);
}

    સ્થિર શૂન્ય
ex_foldopen(exarg_T *eap)
{
    opFoldRange(eap->line1, eap->line2, eap->cmdidx == CMD_foldopen,
							 eap->forceit, FALSE);
}

    સ્થિર શૂન્ય
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
    માટે (lnum = eap->line1; lnum <= eap->line2; ++lnum)
	જો (hasFolding(lnum, ખાલી, ખાલી) == (eap->cmdidx == CMD_folddoclosed))
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

#જો defined(FEAT_QUICKFIX)
/*
 * Returns TRUE if the supplied Ex cmdidx is for a location list command
 * instead of a quickfix command.
 */
    પૂર્ણાંક
is_loclist_cmd(પૂર્ણાંક cmdidx)
{
    જો (cmdidx < 0 || cmdidx >= CMD_SIZE)
	પાછા FALSE;
    પાછા cmdnames[cmdidx].cmd_name[0] == 'l';
}
#endif

    પૂર્ણાંક
get_pressedreturn(શૂન્ય)
{
    પાછા ex_pressedreturn;
}

    શૂન્ય
set_pressedreturn(પૂર્ણાંક val)
{
    ex_pressedreturn = val;
}
