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

#అన్తర్భావ "vim.h"

స్థిర పూర్ణాంకం	quitmore = 0;
స్థిర పూర్ణాంకం	ex_pressedreturn = FALSE;
#నిర్వచితం_కాకపోతే FEAT_PRINTER
# define ex_hardcopy	ex_ni
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
స్థిర char_u	*do_one_cmd(char_u **, పూర్ణాంకం, cstack_T *, char_u *(*fgetline)(పూర్ణాంకం, శూన్య *, పూర్ణాంకం, getline_opt_T), శూన్య *cookie);
#లేకపోతే
స్థిర char_u	*do_one_cmd(char_u **, పూర్ణాంకం, char_u *(*fgetline)(పూర్ణాంకం, శూన్య *, పూర్ణాంకం, getline_opt_T), శూన్య *cookie);
స్థిర పూర్ణాంకం	if_level = 0;		// depth in :if
#అయితే_ముగింపు
స్థిర శూన్య	append_command(char_u *cmd);

#నిర్వచితం_కాకపోతే FEAT_MENU
# define ex_emenu		ex_ni
# define ex_menu		ex_ni
# define ex_menutranslate	ex_ni
#అయితే_ముగింపు
స్థిర శూన్య	ex_autocmd(exarg_T *eap);
స్థిర శూన్య	ex_doautocmd(exarg_T *eap);
స్థిర శూన్య	ex_bunload(exarg_T *eap);
స్థిర శూన్య	ex_బఫరు(exarg_T *eap);
స్థిర శూన్య	do_exbuffer(exarg_T *eap);
స్థిర శూన్య	ex_bmodified(exarg_T *eap);
స్థిర శూన్య	ex_bnext(exarg_T *eap);
స్థిర శూన్య	ex_bprevious(exarg_T *eap);
స్థిర శూన్య	ex_brewind(exarg_T *eap);
స్థిర శూన్య	ex_blast(exarg_T *eap);
స్థిర char_u	*getargcmd(char_u **);
స్థిర పూర్ణాంకం	getargopt(exarg_T *eap);
#నిర్వచితం_కాకపోతే FEAT_QUICKFIX
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
#అయితే_ముగింపు
#ఒకవేళ !defined(FEAT_QUICKFIX) || !defined(FEAT_EVAL)
# define ex_cexpr		ex_ni
#అయితే_ముగింపు

స్థిర linenr_T default_చిరునామా(exarg_T *eap);
స్థిర శూన్య చిరునామా_default_all(exarg_T *eap);
స్థిర శూన్య	get_flags(exarg_T *eap);
#ఒకవేళ !defined(FEAT_PERL) \
	|| !defined(FEAT_PYTHON) || !defined(FEAT_PYTHON3) \
	|| !defined(FEAT_TCL) \
	|| !defined(FEAT_RUBY) \
	|| !defined(FEAT_LUA) \
	|| !defined(FEAT_MZSCHEME)
# define HAVE_EX_SCRIPT_NI
స్థిర శూన్య	ex_script_ni(exarg_T *eap);
#అయితే_ముగింపు
స్థిర అక్షరం	*invalid_range(exarg_T *eap);
స్థిర శూన్య	correct_range(exarg_T *eap);
#నిర్వచితమైతే FEAT_QUICKFIX
స్థిర char_u	*replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep);
#అయితే_ముగింపు
స్థిర char_u	*repl_cmdline(exarg_T *eap, char_u *src, size_t srclen, char_u *repl, char_u **cmdlinep);
స్థిర శూన్య	ex_highlight(exarg_T *eap);
స్థిర శూన్య	ex_colorscheme(exarg_T *eap);
స్థిర శూన్య	ex_cquit(exarg_T *eap);
స్థిర శూన్య	ex_quit_all(exarg_T *eap);
స్థిర శూన్య	ex_close(exarg_T *eap);
స్థిర శూన్య	ex_win_close(పూర్ణాంకం forceit, win_T *win, tabpage_T *tp);
స్థిర శూన్య	ex_only(exarg_T *eap);
స్థిర శూన్య	ex_resize(exarg_T *eap);
స్థిర శూన్య	ex_stag(exarg_T *eap);
స్థిర శూన్య	ex_tabclose(exarg_T *eap);
స్థిర శూన్య	ex_tabonly(exarg_T *eap);
స్థిర శూన్య	ex_tabnext(exarg_T *eap);
స్థిర శూన్య	ex_tabmove(exarg_T *eap);
స్థిర శూన్య	ex_tabs(exarg_T *eap);
#ఒకవేళ defined(FEAT_QUICKFIX)
స్థిర శూన్య	ex_pclose(exarg_T *eap);
స్థిర శూన్య	ex_ptag(exarg_T *eap);
స్థిర శూన్య	ex_pedit(exarg_T *eap);
స్థిర శూన్య	ex_pbuffer(exarg_T *eap);
స్థిర శూన్య	prepare_preview_కిటికీ(శూన్య);
స్థిర శూన్య	వెనుక_గమ్యం_current_కిటికీ(win_T *curwin_save);
#లేకపోతే
# define ex_pclose		ex_ni
# define ex_ptag		ex_ni
# define ex_pedit		ex_ni
# define ex_pbuffer		ex_ni
#అయితే_ముగింపు
స్థిర శూన్య	ex_hide(exarg_T *eap);
స్థిర శూన్య	ex_exit(exarg_T *eap);
స్థిర శూన్య	ex_print(exarg_T *eap);
#నిర్వచితమైతే FEAT_BYTEOFF
స్థిర శూన్య	ex_goto(exarg_T *eap);
#లేకపోతే
# define ex_goto		ex_ni
#అయితే_ముగింపు
స్థిర శూన్య	ex_shell(exarg_T *eap);
స్థిర శూన్య	ex_preserve(exarg_T *eap);
స్థిర శూన్య	ex_recover(exarg_T *eap);
స్థిర శూన్య	ex_బహుళకం(exarg_T *eap);
స్థిర శూన్య	ex_wrongmodifier(exarg_T *eap);
స్థిర శూన్య	ex_కనుగొను(exarg_T *eap);
స్థిర శూన్య	ex_open(exarg_T *eap);
స్థిర శూన్య	ex_edit(exarg_T *eap);
#నిర్వచితం_కాకపోతే FEAT_GUI
# define ex_gui			ex_nogui
స్థిర శూన్య	ex_nogui(exarg_T *eap);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
స్థిర శూన్య	ex_tearoff(exarg_T *eap);
#లేకపోతే
# define ex_tearoff		ex_ni
#అయితే_ముగింపు
#ఒకవేళ (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_పదం_POPUP_MENU)) && defined(FEAT_MENU)
స్థిర శూన్య	ex_popup(exarg_T *eap);
#లేకపోతే
# define ex_popup		ex_ni
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_GUI_MSWIN
# define ex_simalt		ex_ni
#అయితే_ముగింపు
#ఒకవేళ !defined(FEAT_GUI_MSWIN) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
# define gui_mch_కనుగొను_dialog	ex_ni
# define gui_mch_replace_dialog ex_ni
#అయితే_ముగింపు
#ఒకవేళ !defined(FEAT_GUI_GTK)
# define ex_helpfind		ex_ni
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_CSCOPE
# define ex_cscope		ex_ni
# define ex_scscope		ex_ni
# define ex_cstag		ex_ni
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_SYN_HL
# define ex_syntax		ex_ni
# define ex_ownsyntax		ex_ni
#అయితే_ముగింపు
#ఒకవేళ !defined(FEAT_SYN_HL) || !defined(FEAT_PROFILE)
# define ex_syntime		ex_ni
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_SPELL
# define ex_spell		ex_ni
# define ex_mkspell		ex_ni
# define ex_spelldump		ex_ni
# define ex_spellinfo		ex_ni
# define ex_spellrepall		ex_ni
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_PERSISTENT_UNDO
# define ex_rundo		ex_ni
# define ex_wundo		ex_ni
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_LUA
# define ex_lua			ex_script_ni
# define ex_luado		ex_ni
# define ex_luafile		ex_ni
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_MZSCHEME
# define ex_mzscheme		ex_script_ni
# define ex_mzfile		ex_ni
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_PERL
# define ex_perl		ex_script_ni
# define ex_perldo		ex_ni
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_PYTHON
# define ex_python		ex_script_ni
# define ex_pydo		ex_ni
# define ex_pyfile		ex_ni
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_PYTHON3
# define ex_py3			ex_script_ni
# define ex_py3do		ex_ni
# define ex_py3file		ex_ni
#అయితే_ముగింపు
#ఒకవేళ !defined(FEAT_PYTHON) && !defined(FEAT_PYTHON3)
# define ex_pyx			ex_script_ni
# define ex_pyxdo		ex_ni
# define ex_pyxfile		ex_ni
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_TCL
# define ex_tcl			ex_script_ni
# define ex_tcldo		ex_ni
# define ex_tclfile		ex_ni
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_RUBY
# define ex_ruby		ex_script_ni
# define ex_rubydo		ex_ni
# define ex_rubyfile		ex_ni
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_KEYMAP
# define ex_loadkeymap		ex_ni
#అయితే_ముగింపు
స్థిర శూన్య	ex_swapname(exarg_T *eap);
స్థిర శూన్య	ex_syncbind(exarg_T *eap);
స్థిర శూన్య	ex_read(exarg_T *eap);
స్థిర శూన్య	ex_pwd(exarg_T *eap);
స్థిర శూన్య	ex_equal(exarg_T *eap);
స్థిర శూన్య	ex_sleep(exarg_T *eap);
స్థిర శూన్య	ex_winsize(exarg_T *eap);
స్థిర శూన్య	ex_wincmd(exarg_T *eap);
#ఒకవేళ defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
స్థిర శూన్య	ex_winpos(exarg_T *eap);
#లేకపోతే
# define ex_winpos	    ex_ni
#అయితే_ముగింపు
స్థిర శూన్య	ex_operators(exarg_T *eap);
స్థిర శూన్య	ex_put(exarg_T *eap);
స్థిర శూన్య	ex_iput(exarg_T *eap);
స్థిర శూన్య	ex_copymove(exarg_T *eap);
స్థిర శూన్య	ex_submagic(exarg_T *eap);
స్థిర శూన్య	ex_join(exarg_T *eap);
స్థిర శూన్య	ex_at(exarg_T *eap);
స్థిర శూన్య	ex_bang(exarg_T *eap);
స్థిర శూన్య	ex_undo(exarg_T *eap);
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
స్థిర శూన్య	ex_wundo(exarg_T *eap);
స్థిర శూన్య	ex_rundo(exarg_T *eap);
#అయితే_ముగింపు
స్థిర శూన్య	ex_redo(exarg_T *eap);
స్థిర శూన్య	ex_later(exarg_T *eap);
స్థిర శూన్య	ex_redir(exarg_T *eap);
స్థిర శూన్య	ex_redrawstatus(exarg_T *eap);
స్థిర శూన్య	ex_redrawtabline(exarg_T *eap);
స్థిర శూన్య	ex_redrawtabpanel(exarg_T *eap);
స్థిర శూన్య	close_redir(శూన్య);
స్థిర శూన్య	ex_mark(exarg_T *eap);
స్థిర శూన్య	ex_startinsert(exarg_T *eap);
స్థిర శూన్య	ex_stopinsert(exarg_T *eap);
#నిర్వచితమైతే FEAT_కనుగొను_ID
స్థిర శూన్య	ex_checkpath(exarg_T *eap);
స్థిర శూన్య	ex_findpat(exarg_T *eap);
#లేకపోతే
# define ex_findpat		ex_ni
# define ex_checkpath		ex_ni
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_కనుగొను_ID) && defined(FEAT_QUICKFIX)
స్థిర శూన్య	ex_psearch(exarg_T *eap);
#లేకపోతే
# define ex_psearch		ex_ni
#అయితే_ముగింపు
స్థిర శూన్య	ex_ట్యాగ్(exarg_T *eap);
స్థిర శూన్య	ex_ట్యాగ్_cmd(exarg_T *eap, char_u *name);
#నిర్వచితం_కాకపోతే FEAT_EVAL
# define ex_ఖండం		ex_ni
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
# define ex_ముగింపు		ex_ni
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
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_SESSION
# define ex_loadview		ex_ni
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_VIMINFO
# define ex_viminfo		ex_ni
#అయితే_ముగింపు
స్థిర శూన్య	ex_behave(exarg_T *eap);
స్థిర శూన్య	ex_filetype(exarg_T *eap);
స్థిర శూన్య	ex_setfiletype(exarg_T *eap);
#నిర్వచితం_కాకపోతే FEAT_DIFF
# define ex_diffoff		ex_ni
# define ex_diffpatch		ex_ni
# define ex_diffgetput		ex_ni
# define ex_diffsplit		ex_ni
# define ex_diffthis		ex_ni
# define ex_diffupdate		ex_ni
#అయితే_ముగింపు
స్థిర శూన్య	ex_digraphs(exarg_T *eap);
#నిర్వచితమైతే FEAT_శోధన_EXTRA
స్థిర శూన్య	ex_nohlsearch(exarg_T *eap);
#లేకపోతే
# define ex_nohlsearch		ex_ni
# define ex_సరిపోలిక		ex_ni
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CRYPT
స్థిర శూన్య	ex_X(exarg_T *eap);
#లేకపోతే
# define ex_X			ex_ni
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_FOLDING
స్థిర శూన్య	ex_fold(exarg_T *eap);
స్థిర శూన్య	ex_foldopen(exarg_T *eap);
స్థిర శూన్య	ex_folddo(exarg_T *eap);
#లేకపోతే
# define ex_fold		ex_ni
# define ex_foldopen		ex_ni
# define ex_folddo		ex_ni
#అయితే_ముగింపు
#ఒకవేళ !(defined(HAVE_LOCALE_H) || defined(X_LOCALE))
# define ex_language		ex_ni
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_SIGNS
# define ex_sign		ex_ni
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_NETBEANS_INTG
# define ex_nbclose		ex_ni
# define ex_nbkey		ex_ni
# define ex_nbstart		ex_ni
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే FEAT_PROFILE
# define ex_profile		ex_ni
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_TERMINAL
# define ex_terminal		ex_ni
#అయితే_ముగింపు
#ఒకవేళ !defined(FEAT_X11) || !defined(FEAT_XCLIPBOARD)
# define ex_xrestore		ex_ni
#అయితే_ముగింపు
#ఒకవేళ !defined(FEAT_WAYLAND)
# define ex_wlrestore		ex_ni
#అయితే_ముగింపు
#ఒకవేళ !defined(HAVE_CLIPMETHOD)
# define ex_clipreset		ex_ni
#అయితే_ముగింపు
#ఒకవేళ !defined(FEAT_PROP_POPUP)
# define ex_popupclear		ex_ni
#అయితే_ముగింపు

/*
 * Declare cmdnames[].
 */
#నిర్వచించు DO_DECLARE_EXCMD
#అన్తర్భావ "ex_cmds.h"
#అన్తర్భావ "ex_cmdidxs.h"

స్థిర char_u dollar_command[2] = {'$', 0};


#నిర్వచితమైతే FEAT_EVAL
// Struct for storing a line inside a while/for loop
రకనిర్వచనం నిర్మాణం
{
    char_u	*పంక్తి;		// command line
    linenr_T	lnum;		// sourcing_lnum of the line
} wcmd_T;

/*
 * Structure used to store info for line position in a while or for loop.
 * This is required, because do_one_cmd() may invoke ex_function(), which
 * reads more lines that may come from the while/for loop.
 */
నిర్మాణం loop_cookie
{
    garray_T	*పంక్తులు_gap;		// growarray with line info
    పూర్ణాంకం		current_పంక్తి;		// last read line from growarray
    పూర్ణాంకం		repeating;		// TRUE when looping a second time
    // When "repeating" is FALSE use "getline" and "cookie" to get lines
    char_u	*(*lc_getline)(పూర్ణాంకం, శూన్య *, పూర్ణాంకం, getline_opt_T);
    శూన్య	*cookie;
};

స్థిర char_u	*get_loop_పంక్తి(పూర్ణాంకం c, శూన్య *cookie, పూర్ణాంకం indent, getline_opt_T options);
స్థిర పూర్ణాంకం	store_loop_పంక్తి(garray_T *gap, char_u *పంక్తి);
స్థిర శూన్య	free_cmdlines(garray_T *gap);

// Struct to save a few things while debugging.  Used in do_cmdline() only.
నిర్మాణం dbg_stuff
{
    పూర్ణాంకం		trylevel;
    పూర్ణాంకం		force_abort;
    except_T	*caught_దొంతర;
    char_u	*vv_exception;
    char_u	*vv_throwpoint;
    పూర్ణాంకం		did_emsg;
    పూర్ణాంకం		got_int;
    పూర్ణాంకం		did_throw;
    పూర్ణాంకం		need_rethrow;
    పూర్ణాంకం		check_cstack;
    except_T	*current_exception;
};

    స్థిర శూన్య
save_dbg_stuff(నిర్మాణం dbg_stuff *dsp)
{
    dsp->trylevel	= trylevel;		trylevel = 0;
    dsp->force_abort	= force_abort;		force_abort = FALSE;
    dsp->caught_దొంతర	= caught_దొంతర;		caught_దొంతర = ఖాళీ;
    dsp->vv_exception	= v_exception(ఖాళీ);
    dsp->vv_throwpoint	= v_throwpoint(ఖాళీ);

    // Necessary for debugging an inactive ":catch", ":finally", ":endtry"
    dsp->did_emsg	= did_emsg;		did_emsg     = FALSE;
    dsp->got_int	= got_int;		got_int	     = FALSE;
    dsp->did_throw	= did_throw;		did_throw    = FALSE;
    dsp->need_rethrow	= need_rethrow;		need_rethrow = FALSE;
    dsp->check_cstack	= check_cstack;		check_cstack = FALSE;
    dsp->current_exception = current_exception;	current_exception = ఖాళీ;
}

    స్థిర శూన్య
restore_dbg_stuff(నిర్మాణం dbg_stuff *dsp)
{
    suppress_errthrow = FALSE;
    trylevel = dsp->trylevel;
    force_abort = dsp->force_abort;
    caught_దొంతర = dsp->caught_దొంతర;
    (శూన్య)v_exception(dsp->vv_exception);
    (శూన్య)v_throwpoint(dsp->vv_throwpoint);
    did_emsg = dsp->did_emsg;
    got_int = dsp->got_int;
    did_throw = dsp->did_throw;
    need_rethrow = dsp->need_rethrow;
    check_cstack = dsp->check_cstack;
    current_exception = dsp->current_exception;
}
#అయితే_ముగింపు

/*
 * Check if ffname differs from fnum.
 * fnum is a buffer number. 0 == current buffer, 1-or-more must be a valid
 * buffer ID.
 * ffname is a full path to where a buffer lives on-disk or would live on-disk.
 *
 */
    స్థిర పూర్ణాంకం
is_other_file(పూర్ణాంకం fnum, char_u *ffname)
{
    ఒకవేళ (fnum != 0)
    {
	ఒకవేళ (fnum == curbuf->b_fnum)
	    తిరిగి FALSE;

	తిరిగి TRUE;
    }

    ఒకవేళ (ffname == ఖాళీ)
	తిరిగి TRUE;

    ఒకవేళ (*ffname == NUL)
	తిరిగి FALSE;

    // TODO: Need a reliable way to know whether a buffer is meant to live
    // on-disk !curbuf->b_dev_valid is not always available (example: missing
    // on Windows)
    ఒకవేళ (curbuf->b_sfname != ఖాళీ
	    && *curbuf->b_sfname != NUL)
	// This occurs with unsaved buffers. In which case `ffname` actually
	// corresponds to curbuf->b_sfname
	తిరిగి fnamecmp(ffname, curbuf->b_sfname) != 0;

    తిరిగి otherfile(ffname);
}

/*
 * do_exmode(): Repeatedly get commands for the "Ex" mode, until the ":vi"
 * command is given.
 */
    శూన్య
do_exmode(
    పూర్ణాంకం		improved)	    // TRUE for "improved Ex" mode
{
    పూర్ణాంకం		save_సందేశం_scroll;
    పూర్ణాంకం		prev_సందేశం_అడ్డువరుస;
    linenr_T	prev_పంక్తి;
    varnumber_T	changedtick;

    ఒకవేళ (improved)
	exmode_చురుకైన = EXMODE_VIM;
    లేకపోతే
	exmode_చురుకైన = EXMODE_సాధారణ;
    State = బహుళకం_సాధారణ;
    may_trigger_modechanged();

    // When using ":global /pat/ visual" and then "Q" we return to continue
    // the :global command.
    ఒకవేళ (global_busy)
	తిరిగి;

    save_సందేశం_scroll = సందేశం_scroll;
    ++RedrawingDisabled;	    // don't redisplay the window
    ++no_wait_return;		    // don't wait for return
#నిర్వచితమైతే FEAT_GUI
    // Ignore scrollbar and mouse events in Ex mode
    ++hold_gui_events;
#అయితే_ముగింపు

    సందేశం(_("Entering Ex mode.  Type \"visual\" to go to Normal mode."));
    అయితే (exmode_చురుకైన)
    {
	// Check for a ":normal" command and no more characters left.
	ఒకవేళ (ex_సాధారణ_busy > 0 && typebuf.tb_len == 0)
	{
	    exmode_చురుకైన = FALSE;
	    ఆపు;
	}
	సందేశం_scroll = TRUE;
	need_wait_return = FALSE;
	ex_pressedreturn = FALSE;
	ex_no_reprint = FALSE;
	changedtick = CHANGEDTICK(curbuf);
	prev_సందేశం_అడ్డువరుస = సందేశం_అడ్డువరుస;
	prev_పంక్తి = curwin->w_cursor.lnum;
	ఒకవేళ (improved)
	{
	    cmdline_అడ్డువరుస = సందేశం_అడ్డువరుస;
	    do_cmdline(ఖాళీ, getexline, ఖాళీ, 0);
	}
	లేకపోతే
	    do_cmdline(ఖాళీ, getexmodeline, ఖాళీ, DOCMD_NOWAIT);
	పంక్తులు_ఎడమ = అడ్డువరుసలు - 1;

	ఒకవేళ ((prev_పంక్తి != curwin->w_cursor.lnum
		   || changedtick != CHANGEDTICK(curbuf)) && !ex_no_reprint)
	{
	    ఒకవేళ (curbuf->b_ml.ml_flags & ML_ఖాళీలు)
		emsg(_(e_ఖాళీలు_బఫరు));
	    లేకపోతే
	    {
		ఒకవేళ (ex_pressedreturn)
		{
		    // go up one line, to overwrite the ":<CR>" line, so the
		    // output doesn't contain empty lines.
		    సందేశం_అడ్డువరుస = prev_సందేశం_అడ్డువరుస;
		    ఒకవేళ (prev_సందేశం_అడ్డువరుస == అడ్డువరుసలు - 1)
			సందేశం_అడ్డువరుస--;
		}
		సందేశం_నిలువు = 0;
		print_పంక్తి_no_ఉపసర్గ(curwin->w_cursor.lnum, FALSE, FALSE);
		సందేశం_clr_eos();
	    }
	}
	లేకపోతే ఒకవేళ (ex_pressedreturn && !ex_no_reprint)	// must be at EOF
	{
	    ఒకవేళ (curbuf->b_ml.ml_flags & ML_ఖాళీలు)
		emsg(_(e_ఖాళీలు_బఫరు));
	    లేకపోతే
		emsg(_(e_at_చివర_యొక్క_file));
	}
    }

#నిర్వచితమైతే FEAT_GUI
    --hold_gui_events;
#అయితే_ముగింపు
    ఒకవేళ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    update_screen(UPD_CLEAR);
    need_wait_return = FALSE;
    సందేశం_scroll = save_సందేశం_scroll;
}

/*
 * Print the executed command for when 'verbose' is set.
 * When "lnum" is 0 only print the command.
 */
    స్థిర శూన్య
సందేశం_verbose_cmd(linenr_T lnum, char_u *cmd)
{
    ++no_wait_return;
    verbose_enter_scroll();

    ఒకవేళ (lnum == 0)
	smsg(_("Executing: %s"), cmd);
    లేకపోతే
	smsg(_("line %ld: %s"), (దీర్ఘ)lnum, cmd);
    ఒకవేళ (సందేశం_silent == 0)
	సందేశం_puts("\n");   // don't overwrite this

    verbose_leave_scroll();
    --no_wait_return;
}

/*
 * Execute a simple command line.  Used for translated commands like "*".
 */
    పూర్ణాంకం
do_cmdline_cmd(char_u *cmd)
{
    తిరిగి do_cmdline(cmd, ఖాళీ, ఖాళీ,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
}

/*
 * Execute the "+cmd" argument of "edit +cmd fname" and the like.
 * This allows for using a range without ":" in Vim9 script.
 */
    స్థిర పూర్ణాంకం
do_cmd_వాదం(char_u *cmd)
{
    తిరిగి do_cmdline(cmd, ఖాళీ, ఖాళీ,
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
    పూర్ణాంకం
do_cmdline(
    char_u	*cmdline,
    char_u	*(*fgetline)(పూర్ణాంకం, శూన్య *, పూర్ణాంకం, getline_opt_T),
    శూన్య	*cookie,		// argument for fgetline()
    పూర్ణాంకం		flags)
{
    char_u	*తదుపరి_cmdline;		// next cmd to execute
    char_u	*cmdline_copy = ఖాళీ;	// copy of cmd line
    పూర్ణాంకం		వాడిన_getline = FALSE;	// used "fgetline" to obtain command
    స్థిర పూర్ణాంకం	recursive = 0;		// recursive depth
    పూర్ణాంకం		సందేశం_didout_before_ప్రారంభం = 0;
    పూర్ణాంకం		లెక్క = 0;		// line number count
    పూర్ణాంకం		did_inc_RedrawingDisabled = FALSE;
    పూర్ణాంకం		retval = OK;
#నిర్వచితమైతే FEAT_EVAL
    cstack_T	cstack;			// conditional stack
    garray_T	పంక్తులు_ga;		// keep lines for ":while"/":for"
    పూర్ణాంకం		current_పంక్తి = 0;	// active line in lines_ga
    పూర్ణాంకం		current_పంక్తి_before = 0;
    char_u	*fname = ఖాళీ;		// function or script name
    linenr_T	*breakpoint = ఖాళీ;	// ptr to breakpoint field in cookie
    పూర్ణాంకం		*dbg_tick = ఖాళీ;	// ptr to dbg_tick field in cookie
    నిర్మాణం dbg_stuff debug_saved;	// saved things for debug mode
    పూర్ణాంకం		initial_trylevel;
    msglist_T	**saved_సందేశం_list = ఖాళీ;
    msglist_T	*వ్యక్తిగత_సందేశం_list = ఖాళీ;

    // "fgetline" and "cookie" passed to do_one_cmd()
    char_u	*(*cmd_getline)(పూర్ణాంకం, శూన్య *, పూర్ణాంకం, getline_opt_T);
    శూన్య	*cmd_cookie;
    నిర్మాణం loop_cookie cmd_loop_cookie;
    శూన్య	*real_cookie;
    పూర్ణాంకం		getline_is_func;
#లేకపోతే
# define cmd_getline fgetline
# define cmd_cookie cookie
#అయితే_ముగింపు
    స్థిర పూర్ణాంకం	call_లోతు = 0;		// recursiveness
#నిర్వచితమైతే FEAT_EVAL
    // For every pair of do_cmdline()/do_one_cmd() calls, use an extra memory
    // location for storing error messages to be converted to an exception.
    // This ensures that the do_errthrow() call in do_one_cmd() does not
    // combine the messages stored by an earlier invocation of do_one_cmd()
    // with the command name of the later one.  This would happen when
    // BufWritePost autocommands are executed after a write error.
    saved_సందేశం_list = సందేశం_list;
    సందేశం_list = &వ్యక్తిగత_సందేశం_list;
#అయితే_ముగింపు

    // It's possible to create an endless loop with ":execute", catch that
    // here.  The value of 200 allows nested function calls, ":source", etc.
    // Allow 200 or 'maxfuncdepth', whatever is larger.
    ఒకవేళ (call_లోతు >= 200
#నిర్వచితమైతే FEAT_EVAL
	    && call_లోతు >= p_mfd
#అయితే_ముగింపు
	    )
    {
	emsg(_(e_command_too_recursive));
#నిర్వచితమైతే FEAT_EVAL
	// When converting to an exception, we do not include the command name
	// since this is not an error of the specific command.
	do_errthrow((cstack_T *)ఖాళీ, (char_u *)ఖాళీ);
	సందేశం_list = saved_సందేశం_list;
#అయితే_ముగింపు
	తిరిగి FAIL;
    }
    ++call_లోతు;

#నిర్వచితమైతే FEAT_EVAL
    CLEAR_క్షేత్రం(cstack);
    cstack.cs_idx = -1;
    ga_init2(&పంక్తులు_ga, పరిమాణం(wcmd_T), 10);

    real_cookie = getline_cookie(fgetline, cookie);

    // Inside a function use a higher nesting level.
    getline_is_func = getline_equal(fgetline, cookie, get_func_పంక్తి);
    ఒకవేళ (getline_is_func && ex_nesting_level == func_level(real_cookie))
	++ex_nesting_level;

    // Get the function or script name and the address where the next breakpoint
    // line and the debug tick for a function or script are stored.
    ఒకవేళ (getline_is_func)
    {
	fname = func_name(real_cookie);
	breakpoint = func_breakpoint(real_cookie);
	dbg_tick = func_dbg_tick(real_cookie);
    }
    లేకపోతే ఒకవేళ (getline_equal(fgetline, cookie, getsourceline))
    {
	fname = SOURCING_NAME;
	breakpoint = ఆధారం_breakpoint(real_cookie);
	dbg_tick = ఆధారం_dbg_tick(real_cookie);
    }

    /*
     * Initialize "force_abort"  and "suppress_errthrow" at the top level.
     */
    ఒకవేళ (!recursive)
    {
	force_abort = FALSE;
	suppress_errthrow = FALSE;
    }

    /*
     * If requested, store and reset the global values controlling the
     * exception handling (used when debugging).  Otherwise clear it to avoid
     * a bogus compiler warning when the optimizer uses inline functions...
     */
    ఒకవేళ (flags & DOCMD_EXCRESET)
	save_dbg_stuff(&debug_saved);
    లేకపోతే
	CLEAR_క్షేత్రం(debug_saved);

    initial_trylevel = trylevel;

    /*
     * "did_throw" will be set to TRUE when an exception is being thrown.
     */
    did_throw = FALSE;
#అయితే_ముగింపు
    /*
     * "did_emsg" will be set to TRUE when emsg() is used, in which case we
     * cancel the whole command line, and any if/endif or loop.
     * If force_abort is set, we cancel everything.
     */
#నిర్వచితమైతే FEAT_EVAL
    did_emsg_cumul += did_emsg;
#అయితే_ముగింపు
    did_emsg = FALSE;

    /*
     * KeyTyped is only set when calling vgetc().  Reset it here when not
     * calling vgetc() (sourced command lines).
     */
    ఒకవేళ (!(flags & DOCMD_KEYTYPED)
			       && !getline_equal(fgetline, cookie, getexline))
	కీలకం_Typed = FALSE;

    /*
     * Continue executing command lines:
     * - when inside an ":if", ":while" or ":for"
     * - for multiple commands on one line, separated with '|'
     * - when repeating until there are no more lines (for ":source")
     */
    తదుపరి_cmdline = cmdline;
    చేయి
    {
#నిర్వచితమైతే FEAT_EVAL
	getline_is_func = getline_equal(fgetline, cookie, get_func_పంక్తి);
#అయితే_ముగింపు

	// stop skipping cmds for an error msg after all endif/while/for
	ఒకవేళ (తదుపరి_cmdline == ఖాళీ
#నిర్వచితమైతే FEAT_EVAL
		&& !force_abort
		&& cstack.cs_idx < 0
		&& !(getline_is_func && func_has_abort(real_cookie))
#అయితే_ముగింపు
							)
	{
#నిర్వచితమైతే FEAT_EVAL
	    did_emsg_cumul += did_emsg;
#అయితే_ముగింపు
	    did_emsg = FALSE;
	}

	/*
	 * 1. If repeating a line in a loop, get a line from lines_ga.
	 * 2. If no line given: Get an allocated line with fgetline().
	 * 3. If a line is given: Make a copy, so we can mess with it.
	 */

#నిర్వచితమైతే FEAT_EVAL
	// 1. If repeating, get a previous line from lines_ga.
	ఒకవేళ (cstack.cs_looplevel > 0 && current_పంక్తి < పంక్తులు_ga.ga_len)
	{
	    // Each '|' separated command is stored separately in lines_ga, to
	    // be able to jump to it.  Don't use next_cmdline now.
	    VIM_CLEAR(cmdline_copy);

	    // Check if a function has returned or, unless it has an unclosed
	    // try conditional, aborted.
	    ఒకవేళ (getline_is_func)
	    {
# ifdef FEAT_PROFILE
		ఒకవేళ (do_profiling == PROF_YES)
		    func_పంక్తి_చివర(real_cookie);
# endif
		ఒకవేళ (func_has_ended(real_cookie))
		{
		    retval = FAIL;
		    ఆపు;
		}
	    }
# ifdef FEAT_PROFILE
	    లేకపోతే ఒకవేళ (do_profiling == PROF_YES
			    && getline_equal(fgetline, cookie, getsourceline))
		script_పంక్తి_చివర();
# endif

	    // Check if a sourced file hit a ":finish" command.
	    ఒకవేళ (ఆధారం_ముగిసిన(fgetline, cookie))
	    {
		retval = FAIL;
		ఆపు;
	    }

	    // If breakpoints have been added/deleted need to check for it.
	    ఒకవేళ (breakpoint != ఖాళీ && dbg_tick != ఖాళీ
						   && *dbg_tick != debug_tick)
	    {
		*breakpoint = dbg_కనుగొను_breakpoint(
				getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }

	    తదుపరి_cmdline = ((wcmd_T *)(పంక్తులు_ga.ga_దత్తాంశం))[current_పంక్తి].పంక్తి;
	    SOURCING_LNUM = ((wcmd_T *)(పంక్తులు_ga.ga_దత్తాంశం))[current_పంక్తి].lnum;

	    // Did we encounter a breakpoint?
	    ఒకవేళ (breakpoint != ఖాళీ && *breakpoint != 0
					      && *breakpoint <= SOURCING_LNUM)
	    {
		dbg_breakpoint(fname, SOURCING_LNUM);
		// Find next breakpoint.
		*breakpoint = dbg_కనుగొను_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }
# ifdef FEAT_PROFILE
	    ఒకవేళ (do_profiling == PROF_YES)
	    {
		ఒకవేళ (getline_is_func)
		    func_పంక్తి_ప్రారంభం(real_cookie, SOURCING_LNUM);
		లేకపోతే ఒకవేళ (getline_equal(fgetline, cookie, getsourceline))
		    script_పంక్తి_ప్రారంభం();
	    }
# endif
	}
#అయితే_ముగింపు

	// 2. If no line given, get an allocated line with fgetline().
	ఒకవేళ (తదుపరి_cmdline == ఖాళీ)
	{
	    /*
	     * Need to set msg_didout for the first line after an ":if",
	     * otherwise the ":if" will be overwritten.
	     */
	    ఒకవేళ (లెక్క == 1 && getline_equal(fgetline, cookie, getexline))
		సందేశం_didout = TRUE;
	    ఒకవేళ (fgetline == ఖాళీ || (తదుపరి_cmdline = fgetline(':', cookie,
#నిర్వచితమైతే FEAT_EVAL
		    cstack.cs_idx < 0 ? 0 : (cstack.cs_idx + 1) * 2
#లేకపోతే
		    0
#అయితే_ముగింపు
		    , లోపల_vim9script() ? GETLINE_CONCAT_CONTBAR
					       : GETLINE_CONCAT_CONT)) == ఖాళీ)
	    {
		// Don't call wait_return() for aborted command line.  The NULL
		// returned for the end of a sourced file or executed function
		// doesn't do this.
		ఒకవేళ (కీలకం_Typed && !(flags & DOCMD_REPEAT))
		    need_wait_return = FALSE;
		retval = FAIL;
		ఆపు;
	    }
	    వాడిన_getline = TRUE;

	    /*
	     * Keep the first typed line.  Clear it when more lines are typed.
	     */
	    ఒకవేళ (flags & DOCMD_KEEPLINE)
	    {
		vim_free(repeat_cmdline);
		ఒకవేళ (లెక్క == 0)
		    repeat_cmdline = vim_strsave(తదుపరి_cmdline);
		లేకపోతే
		    repeat_cmdline = ఖాళీ;
	    }
	}

	// 3. Make a copy of the command so we can mess with it.
	లేకపోతే ఒకవేళ (cmdline_copy == ఖాళీ)
	{
	    తదుపరి_cmdline = vim_strsave(తదుపరి_cmdline);
	    ఒకవేళ (తదుపరి_cmdline == ఖాళీ)
	    {
		emsg(_(e_బహిర్గతం_యొక్క_జ్ఞాపకం));
		retval = FAIL;
		ఆపు;
	    }
	}
	cmdline_copy = తదుపరి_cmdline;

#నిర్వచితమైతే FEAT_EVAL
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
	ఒకవేళ ((cstack.cs_looplevel > 0 || has_loop_cmd(తదుపరి_cmdline)))
	{
	    cmd_getline = get_loop_పంక్తి;
	    cmd_cookie = (శూన్య *)&cmd_loop_cookie;
	    cmd_loop_cookie.పంక్తులు_gap = &పంక్తులు_ga;
	    cmd_loop_cookie.current_పంక్తి = current_పంక్తి;
	    cmd_loop_cookie.lc_getline = fgetline;
	    cmd_loop_cookie.cookie = cookie;
	    cmd_loop_cookie.repeating = (current_పంక్తి < పంక్తులు_ga.ga_len);

	    // Save the current line when encountering it the first time.
	    ఒకవేళ (current_పంక్తి == పంక్తులు_ga.ga_len
		    && store_loop_పంక్తి(&పంక్తులు_ga, తదుపరి_cmdline) == FAIL)
	    {
		retval = FAIL;
		ఆపు;
	    }
	    current_పంక్తి_before = current_పంక్తి;
	}
	లేకపోతే
	{
	    cmd_getline = fgetline;
	    cmd_cookie = cookie;
	}

	did_endif = FALSE;
#అయితే_ముగింపు

	ఒకవేళ (లెక్క++ == 0)
	{
	    /*
	     * All output from the commands is put below each other, without
	     * waiting for a return. Don't do this when executing commands
	     * from a script or when being called recursive (e.g. for ":e
	     * +command file").
	     */
	    ఒకవేళ (!(flags & DOCMD_NOWAIT) && !recursive)
	    {
		సందేశం_didout_before_ప్రారంభం = సందేశం_didout;
		సందేశం_didany = FALSE; // no output yet
		సందేశం_ప్రారంభం();
		సందేశం_scroll = TRUE;  // put messages below each other
		++no_wait_return;   // don't wait for return until finished
		++RedrawingDisabled;
		did_inc_RedrawingDisabled = TRUE;
	    }
	}

	ఒకవేళ ((p_verbose >= 15 && SOURCING_NAME != ఖాళీ) || p_verbose >= 16)
	    సందేశం_verbose_cmd(SOURCING_LNUM, cmdline_copy);

	/*
	 * 2. Execute one '|' separated command.
	 *    do_one_cmd() will return NULL if there is no trailing '|'.
	 *    "cmdline_copy" can change, e.g. for '%' and '#' expansion.
	 */
	++recursive;
	తదుపరి_cmdline = do_one_cmd(&cmdline_copy, flags,
#నిర్వచితమైతే FEAT_EVAL
				&cstack,
#అయితే_ముగింపు
				cmd_getline, cmd_cookie);
	--recursive;

#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (cmd_cookie == (శూన్య *)&cmd_loop_cookie)
	    // Use "current_line" from "cmd_loop_cookie", it may have been
	    // incremented when defining a function.
	    current_పంక్తి = cmd_loop_cookie.current_పంక్తి;
#అయితే_ముగింపు

	ఒకవేళ (తదుపరి_cmdline == ఖాళీ)
	{
	    VIM_CLEAR(cmdline_copy);

	    /*
	     * If the command was typed, remember it for the ':' register.
	     * Do this AFTER executing the command to make :@: work.
	     */
	    ఒకవేళ (getline_equal(fgetline, cookie, getexline)
						  && new_చివరి_cmdline != ఖాళీ)
	    {
		vim_free(చివరి_cmdline);
		చివరి_cmdline = new_చివరి_cmdline;
		new_చివరి_cmdline = ఖాళీ;
	    }
	}
	లేకపోతే
	{
	    // need to copy the command after the '|' to cmdline_copy, for the
	    // next do_one_cmd()
	    STRMOVE(cmdline_copy, తదుపరి_cmdline);
	    తదుపరి_cmdline = cmdline_copy;
	}


#నిర్వచితమైతే FEAT_EVAL
	// reset did_emsg for a function that is not aborted by an error
	ఒకవేళ (did_emsg && !force_abort
		&& getline_equal(fgetline, cookie, get_func_పంక్తి)
					      && !func_has_abort(real_cookie))
	{
	    // did_emsg_cumul is not set here
	    did_emsg = FALSE;
	}

	ఒకవేళ (cstack.cs_looplevel > 0)
	{
	    ++current_పంక్తి;

	    /*
	     * An ":endwhile", ":endfor" and ":continue" is handled here.
	     * If we were executing commands, jump back to the ":while" or
	     * ":for".
	     * If we were not executing commands, decrement cs_looplevel.
	     */
	    ఒకవేళ (cstack.cs_lflags & (CSL_HAD_CONT | CSL_HAD_ENDLOOP))
	    {
		cstack.cs_lflags &= ~(CSL_HAD_CONT | CSL_HAD_ENDLOOP);

		// Jump back to the matching ":while" or ":for".  Be careful
		// not to use a cs_line[] from an entry that isn't a ":while"
		// or ":for": It would make "current_line" invalid and can
		// cause a crash.
		ఒకవేళ (!did_emsg && !got_int && !did_throw
			&& cstack.cs_idx >= 0
			&& (cstack.cs_flags[cstack.cs_idx]
						      & (CSF_WHILE | CSF_FOR))
			&& cstack.cs_పంక్తి[cstack.cs_idx] >= 0
			&& (cstack.cs_flags[cstack.cs_idx] & CSF_చురుకైన))
		{
		    current_పంక్తి = cstack.cs_పంక్తి[cstack.cs_idx];
						// remember we jumped there
		    cstack.cs_lflags |= CSL_HAD_LOOP;
		    పంక్తి_breakcheck();		// check if CTRL-C typed

		    // Check for the next breakpoint at or after the ":while"
		    // or ":for".
		    ఒకవేళ (breakpoint != ఖాళీ && పంక్తులు_ga.ga_len > current_పంక్తి)
		    {
			*breakpoint = dbg_కనుగొను_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
									fname,
			   ((wcmd_T *)పంక్తులు_ga.ga_దత్తాంశం)[current_పంక్తి].lnum-1);
			*dbg_tick = debug_tick;
		    }
		}
		లేకపోతే
		{
		    // can only get here with ":endwhile" or ":endfor"
		    ఒకవేళ (cstack.cs_idx >= 0)
			rewind_conditionals(&cstack, cstack.cs_idx - 1,
				   CSF_WHILE | CSF_FOR, &cstack.cs_looplevel);
		}
	    }

	    /*
	     * For a ":while" or ":for" we need to remember the line number.
	     */
	    లేకపోతే ఒకవేళ (cstack.cs_lflags & CSL_HAD_LOOP)
	    {
		cstack.cs_lflags &= ~CSL_HAD_LOOP;
		cstack.cs_పంక్తి[cstack.cs_idx] = current_పంక్తి_before;
	    }
	}

	// Check for the next breakpoint after a watchexpression
	ఒకవేళ (breakpoint != ఖాళీ && has_watchexpr())
	{
	    *breakpoint = dbg_కనుగొను_breakpoint(FALSE, fname, SOURCING_LNUM);
	    *dbg_tick = debug_tick;
	}

	/*
	 * When not inside any ":while" loop, clear remembered lines.
	 */
	ఒకవేళ (cstack.cs_looplevel == 0)
	{
	    ఒకవేళ (పంక్తులు_ga.ga_len > 0)
	    {
		SOURCING_LNUM =
		       ((wcmd_T *)పంక్తులు_ga.ga_దత్తాంశం)[పంక్తులు_ga.ga_len - 1].lnum;
		free_cmdlines(&పంక్తులు_ga);
	    }
	    current_పంక్తి = 0;
	}

	/*
	 * A ":finally" makes did_emsg, got_int, and did_throw pending for
	 * being restored at the ":endtry".  Reset them here and set the
	 * ACTIVE and FINALLY flags, so that the finally clause gets executed.
	 * This includes the case where a missing ":endif", ":endwhile" or
	 * ":endfor" was detected by the ":finally" itself.
	 */
	ఒకవేళ (cstack.cs_lflags & CSL_HAD_FINA)
	{
	    cstack.cs_lflags &= ~CSL_HAD_FINA;
	    report_make_pending(cstack.cs_pending[cstack.cs_idx]
		    & (CSTP_ERROR | CSTP_అంతరాయం | CSTP_THROW),
		    did_throw ? (శూన్య *)current_exception : ఖాళీ);
	    did_emsg = got_int = did_throw = FALSE;
	    cstack.cs_flags[cstack.cs_idx] |= CSF_చురుకైన | CSF_FINALLY;
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
	ఒకవేళ (trylevel == 0 && !did_emsg && !got_int && !did_throw)
	    force_abort = FALSE;

	// Convert an interrupt to an exception if appropriate.
	(శూన్య)do_intthrow(&cstack);
#అయితే_ముగింపు // FEAT_EVAL

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
    అయితే (!((got_int
#నిర్వచితమైతే FEAT_EVAL
		    || (did_emsg && (force_abort || లోపల_vim9script()))
		    || did_throw
#అయితే_ముగింపు
	     )
#నిర్వచితమైతే FEAT_EVAL
		&& cstack.cs_trylevel == 0
#అయితే_ముగింపు
	    )
	    && !(did_emsg
#నిర్వచితమైతే FEAT_EVAL
		// Keep going when inside try/catch, so that the error can be
		// deal with, except when it is a syntax error, it may cause
		// the :endtry to be missed.
		&& (cstack.cs_trylevel == 0 || did_emsg_syntax)
#అయితే_ముగింపు
		&& వాడిన_getline
			    && (getline_equal(fgetline, cookie, getexmodeline)
			       || getline_equal(fgetline, cookie, getexline)))
	    && (తదుపరి_cmdline != ఖాళీ
#నిర్వచితమైతే FEAT_EVAL
			|| cstack.cs_idx >= 0
#అయితే_ముగింపు
			|| (flags & DOCMD_REPEAT)));

    vim_free(cmdline_copy);
    did_emsg_syntax = FALSE;
#నిర్వచితమైతే FEAT_EVAL
    free_cmdlines(&పంక్తులు_ga);
    ga_clear(&పంక్తులు_ga);

    ఒకవేళ (cstack.cs_idx >= 0)
    {
	/*
	 * If a sourced file or executed function ran to its end, report the
	 * unclosed conditional.
	 * In Vim9 script do not give a second error, executing aborts after
	 * the first one.
	 */
	ఒకవేళ (!got_int && !did_throw && !aborting()
		&& !(did_emsg && లోపల_vim9script())
		&& ((getline_equal(fgetline, cookie, getsourceline)
			&& !ఆధారం_ముగిసిన(fgetline, cookie))
		    || (getline_equal(fgetline, cookie, get_func_పంక్తి)
					    && !func_has_ended(real_cookie))))
	{
	    ఒకవేళ (cstack.cs_flags[cstack.cs_idx] & CSF_TRY)
		emsg(_(e_missing_endtry));
	    లేకపోతే ఒకవేళ (cstack.cs_flags[cstack.cs_idx] & CSF_WHILE)
		emsg(_(e_missing_endwhile));
	    లేకపోతే ఒకవేళ (cstack.cs_flags[cstack.cs_idx] & CSF_FOR)
		emsg(_(e_missing_endfor));
	    లేకపోతే
		emsg(_(e_missing_endif));
	}

	/*
	 * Reset "trylevel" in case of a ":finish" or ":return" or a missing
	 * ":endtry" in a sourced file or executed function.  If the try
	 * conditional is in its finally clause, ignore anything pending.
	 * If it is in a catch clause, finish the caught exception.
	 * Also cleanup any "cs_forinfo" structures.
	 */
	చేయి
	{
	    పూర్ణాంకం idx = cleanup_conditionals(&cstack, 0, TRUE);

	    ఒకవేళ (idx >= 0)
		--idx;	    // remove try block not in its finally clause
	    rewind_conditionals(&cstack, idx, CSF_WHILE | CSF_FOR,
							&cstack.cs_looplevel);
	}
	అయితే (cstack.cs_idx >= 0);
	trylevel = initial_trylevel;
    }

    // If a missing ":endtry", ":endwhile", ":endfor", or ":endif" or a memory
    // lack was reported above and the error message is to be converted to an
    // exception, do this now after rewinding the cstack.
    do_errthrow(&cstack, getline_equal(fgetline, cookie, get_func_పంక్తి)
				  ? (char_u *)"endfunction" : (char_u *)ఖాళీ);

    ఒకవేళ (trylevel == 0)
    {
	// Just in case did_throw got set but current_exception wasn't.
	ఒకవేళ (current_exception == ఖాళీ)
	    did_throw = FALSE;

	/*
	 * When an exception is being thrown out of the outermost try
	 * conditional, discard the uncaught exception, disable the conversion
	 * of interrupts or errors to exceptions, and ensure that no more
	 * commands are executed.
	 */
	ఒకవేళ (did_throw)
	    handle_did_throw();

	/*
	 * On an interrupt or an aborting error not converted to an exception,
	 * disable the conversion of errors to exceptions.  (Interrupts are not
	 * converted anymore, here.) This enables also the interrupt message
	 * when force_abort is set and did_emsg unset in case of an interrupt
	 * from a finally clause after an error.
	 */
	లేకపోతే ఒకవేళ (got_int || (did_emsg && force_abort))
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
    ఒకవేళ (did_throw)
	need_rethrow = TRUE;
    ఒకవేళ ((getline_equal(fgetline, cookie, getsourceline)
		&& ex_nesting_level > ఆధారం_level(real_cookie))
	    || (getline_equal(fgetline, cookie, get_func_పంక్తి)
		&& ex_nesting_level > func_level(real_cookie) + 1))
    {
	ఒకవేళ (!did_throw)
	    check_cstack = TRUE;
    }
    లేకపోతే
    {
	// When leaving a function, reduce nesting level.
	ఒకవేళ (getline_equal(fgetline, cookie, get_func_పంక్తి))
	    --ex_nesting_level;
	/*
	 * Go to debug mode when returning from a function in which we are
	 * single-stepping.
	 */
	ఒకవేళ ((getline_equal(fgetline, cookie, getsourceline)
		    || getline_equal(fgetline, cookie, get_func_పంక్తి))
		&& ex_nesting_level + 1 <= debug_break_level)
	    do_debug(getline_equal(fgetline, cookie, getsourceline)
		    ? (char_u *)_("End of sourced file")
		    : (char_u *)_("End of function"));
    }

    /*
     * Restore the exception environment (done after returning from the
     * debugger).
     */
    ఒకవేళ (flags & DOCMD_EXCRESET)
	restore_dbg_stuff(&debug_saved);

    సందేశం_list = saved_సందేశం_list;

    // Cleanup if "cs_emsg_silent_list" remains.
    ఒకవేళ (cstack.cs_emsg_silent_list != ఖాళీ)
    {
	eslist_T *elem, *తాత్కాలిక;

	కొరకు (elem = cstack.cs_emsg_silent_list; elem != ఖాళీ; elem = తాత్కాలిక)
	{
	    తాత్కాలిక = elem->తదుపరి;
	    vim_free(elem);
	}
    }
#అయితే_ముగింపు // FEAT_EVAL

    /*
     * If there was too much output to fit on the command line, ask the user to
     * hit return before redrawing the screen. With the ":global" command we do
     * this only once after the command is finished.
     */
    ఒకవేళ (did_inc_RedrawingDisabled)
    {
	ఒకవేళ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	--no_wait_return;
	సందేశం_scroll = FALSE;

	/*
	 * When just finished an ":if"-":else" which was typed, no need to
	 * wait for hit-return.  Also for an error situation.
	 */
	ఒకవేళ (retval == FAIL
#నిర్వచితమైతే FEAT_EVAL
		|| (did_endif && కీలకం_Typed && !did_emsg)
#అయితే_ముగింపు
					    )
	{
	    need_wait_return = FALSE;
	    సందేశం_didany = FALSE;		// don't wait when restarting edit
	}
	లేకపోతే ఒకవేళ (need_wait_return)
	{
	    /*
	     * The msg_start() above clears msg_didout. The wait_return() we do
	     * here should not overwrite the command that may be shown before
	     * doing that.
	     */
	    సందేశం_didout |= సందేశం_didout_before_ప్రారంభం;
	    wait_return(FALSE);
	}
    }

#నిర్వచితమైతే FEAT_EVAL
    did_endif = FALSE;  // in case do_cmdline used recursively
#లేకపోతే
    /*
     * Reset if_level, in case a sourced script file contains more ":if" than
     * ":endif" (could be ":if x | foo | endif").
     */
    if_level = 0;
#అయితే_ముగింపు

    --call_లోతు;
    తిరిగి retval;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Handle when "did_throw" is set after executing commands.
 */
    శూన్య
handle_did_throw(శూన్య)
{
    అక్షరం	*p = ఖాళీ;
    msglist_T	*messages = ఖాళీ;
    ESTACK_CHECK_DECLARATION;

    /*
     * If the uncaught exception is a user exception, report it as an
     * error.  If it is an error exception, display the saved error
     * message now.  For an interrupt exception, do nothing; the
     * interrupt message is given elsewhere.
     */
    ఎంపిక (current_exception->type)
    {
	స్థితి ET_USER:
	    vim_snprintf((అక్షరం *)IObuff, IOSIZE,
		    _(e_exception_not_caught_str),
		    current_exception->విలువ);
	    p = (అక్షరం *)vim_strsave(IObuff);
	    ఆపు;
	స్థితి ET_ERROR:
	    messages = current_exception->messages;
	    current_exception->messages = ఖాళీ;
	    ఆపు;
	స్థితి ET_అంతరాయం:
	    ఆపు;
    }

    estack_నెట్టు(ETYPE_EXCEPT, current_exception->throw_name,
					current_exception->throw_lnum);
    ESTACK_CHECK_అమరిక;
    current_exception->throw_name = ఖాళీ;

    discard_current_exception();	// uses IObuff if 'verbose'

    // If "silent!" is active the uncaught exception is not fatal.
    ఒకవేళ (emsg_silent == 0)
    {
	suppress_errthrow = TRUE;
	force_abort = TRUE;
    }

    ఒకవేళ (messages != ఖాళీ)
    {
	చేయి
	{
	    msglist_T	*తదుపరి = messages->తదుపరి;
	    పూర్ణాంకం		save_compiling = estack_compiling;

	    estack_compiling = messages->సందేశం_compiling;
	    emsg(messages->సందేశం);
	    vim_free(messages->సందేశం);
	    vim_free(messages->sfile);
	    vim_free(messages);
	    messages = తదుపరి;
	    estack_compiling = save_compiling;
	}
	అయితే (messages != ఖాళీ);
    }
    లేకపోతే ఒకవేళ (p != ఖాళీ)
    {
	emsg(p);
	vim_free(p);
    }
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_తీయి();
}

/*
 * Obtain a line when inside a ":while" or ":for" loop.
 */
    స్థిర char_u *
get_loop_పంక్తి(పూర్ణాంకం c, శూన్య *cookie, పూర్ణాంకం indent, getline_opt_T options)
{
    నిర్మాణం loop_cookie	*cp = (నిర్మాణం loop_cookie *)cookie;
    wcmd_T		*wp;
    char_u		*పంక్తి;

    ఒకవేళ (cp->current_పంక్తి + 1 >= cp->పంక్తులు_gap->ga_len)
    {
	ఒకవేళ (cp->repeating)
	    తిరిగి ఖాళీ;	// trying to read past ":endwhile"/":endfor"

	// First time inside the ":while"/":for": get line normally.
	ఒకవేళ (cp->lc_getline == ఖాళీ)
	    పంక్తి = getcmdline(c, 0L, indent, 0);
	లేకపోతే
	    పంక్తి = cp->lc_getline(c, cp->cookie, indent, options);
	ఒకవేళ (పంక్తి != ఖాళీ && store_loop_పంక్తి(cp->పంక్తులు_gap, పంక్తి) == OK)
	    ++cp->current_పంక్తి;

	తిరిగి పంక్తి;
    }

    కీలకం_Typed = FALSE;
    ++cp->current_పంక్తి;
    wp = (wcmd_T *)(cp->పంక్తులు_gap->ga_దత్తాంశం) + cp->current_పంక్తి;
    SOURCING_LNUM = wp->lnum;
    తిరిగి vim_strsave(wp->పంక్తి);
}

/*
 * Store a line in "gap" so that a ":while" loop can execute it again.
 */
    స్థిర పూర్ణాంకం
store_loop_పంక్తి(garray_T *gap, char_u *పంక్తి)
{
    ఒకవేళ (ga_grow(gap, 1) == FAIL)
	తిరిగి FAIL;
    ((wcmd_T *)(gap->ga_దత్తాంశం))[gap->ga_len].పంక్తి = vim_strsave(పంక్తి);
    ((wcmd_T *)(gap->ga_దత్తాంశం))[gap->ga_len].lnum = SOURCING_LNUM;
    ++gap->ga_len;
    తిరిగి OK;
}

/*
 * Free the lines stored for a ":while" or ":for" loop.
 */
    స్థిర శూన్య
free_cmdlines(garray_T *gap)
{
    అయితే (gap->ga_len > 0)
    {
	vim_free(((wcmd_T *)(gap->ga_దత్తాంశం))[gap->ga_len - 1].పంక్తి);
	--gap->ga_len;
    }
}
#అయితే_ముగింపు

/*
 * If "fgetline" is get_loop_line(), return TRUE if the getline it uses equals
 * "func".  * Otherwise return TRUE when "fgetline" equals "func".
 */
    పూర్ణాంకం
getline_equal(
    char_u	*(*fgetline)(పూర్ణాంకం, శూన్య *, పూర్ణాంకం, getline_opt_T),
    శూన్య	*cookie వాడని,		// argument for fgetline()
    char_u	*(*func)(పూర్ణాంకం, శూన్య *, పూర్ణాంకం, getline_opt_T))
{
#నిర్వచితమైతే FEAT_EVAL
    char_u		*(*gp)(పూర్ణాంకం, శూన్య *, పూర్ణాంకం, getline_opt_T);
    నిర్మాణం loop_cookie *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // function that's originally used to obtain the lines.  This may be
    // nested several levels.
    gp = fgetline;
    cp = (నిర్మాణం loop_cookie *)cookie;
    అయితే (gp == get_loop_పంక్తి)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    తిరిగి gp == func;
#లేకపోతే
    తిరిగి fgetline == func;
#అయితే_ముగింపు
}

/*
 * If "fgetline" is get_loop_line(), return the cookie used by the original
 * getline function.  Otherwise return "cookie".
 */
    శూన్య *
getline_cookie(
    char_u	*(*fgetline)(పూర్ణాంకం, శూన్య *, పూర్ణాంకం, getline_opt_T) వాడని,
    శూన్య	*cookie)		// argument for fgetline()
{
#నిర్వచితమైతే FEAT_EVAL
    char_u		*(*gp)(పూర్ణాంకం, శూన్య *, పూర్ణాంకం, getline_opt_T);
    నిర్మాణం loop_cookie  *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (నిర్మాణం loop_cookie *)cookie;
    అయితే (gp == get_loop_పంక్తి)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    తిరిగి cp;
#లేకపోతే
    తిరిగి cookie;
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Get the next line source line without advancing.
 */
    char_u *
getline_peek(
    char_u	*(*fgetline)(పూర్ణాంకం, శూన్య *, పూర్ణాంకం, getline_opt_T) వాడని,
    శూన్య	*cookie)		// argument for fgetline()
{
    char_u		*(*gp)(పూర్ణాంకం, శూన్య *, పూర్ణాంకం, getline_opt_T);
    నిర్మాణం loop_cookie  *cp;
    wcmd_T		*wp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (నిర్మాణం loop_cookie *)cookie;
    అయితే (gp == get_loop_పంక్తి)
    {
	ఒకవేళ (cp->current_పంక్తి + 1 < cp->పంక్తులు_gap->ga_len)
	{
	    // executing lines a second time, use the stored copy
	    wp = (wcmd_T *)(cp->పంక్తులు_gap->ga_దత్తాంశం) + cp->current_పంక్తి + 1;
	    తిరిగి wp->పంక్తి;
	}
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    ఒకవేళ (gp == getsourceline)
	తిరిగి ఆధారం_nextline(cp);
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు


/*
 * Helper function to apply an offset for buffer commands, i.e. ":bdelete",
 * ":bwipeout", etc.
 * Returns the buffer number.
 */
    స్థిర పూర్ణాంకం
compute_బఫరు_local_లెక్క(పూర్ణాంకం addr_type, పూర్ణాంకం lnum, పూర్ణాంకం స్థానభ్రంశం)
{
    buf_T   *buf;
    buf_T   *nextbuf;
    పూర్ణాంకం     లెక్క = స్థానభ్రంశం;

    buf = firstbuf;
    అయితే (buf->b_తదుపరి != ఖాళీ && buf->b_fnum < lnum)
	buf = buf->b_తదుపరి;
    అయితే (లెక్క != 0)
    {
	లెక్క += (స్థానభ్రంశం < 0) ? 1 : -1;
	nextbuf = (స్థానభ్రంశం < 0) ? buf->b_prev : buf->b_తదుపరి;
	ఒకవేళ (nextbuf == ఖాళీ)
	    ఆపు;
	buf = nextbuf;
	ఒకవేళ (addr_type == ADDR_LOADED_BUFFERS)
	    // skip over unloaded buffers
	    అయితే (buf->b_ml.ml_mfp == ఖాళీ)
	    {
		nextbuf = (స్థానభ్రంశం < 0) ? buf->b_prev : buf->b_తదుపరి;
		ఒకవేళ (nextbuf == ఖాళీ)
		    ఆపు;
		buf = nextbuf;
	    }
    }
    // we might have gone too far, last buffer is not loadedd
    ఒకవేళ (addr_type == ADDR_LOADED_BUFFERS)
	అయితే (buf->b_ml.ml_mfp == ఖాళీ)
	{
	    nextbuf = (స్థానభ్రంశం >= 0) ? buf->b_prev : buf->b_తదుపరి;
	    ఒకవేళ (nextbuf == ఖాళీ)
		ఆపు;
	    buf = nextbuf;
	}
    తిరిగి buf->b_fnum;
}

/*
 * Return the window number of "win".
 * When "win" is NULL return the number of windows.
 */
    స్థిర పూర్ణాంకం
current_win_nr(win_T *win)
{
    win_T	*wp;
    పూర్ణాంకం		nr = 0;

    FOR_ALL_WINDOWS(wp)
    {
	++nr;
	ఒకవేళ (wp == win)
	    ఆపు;
    }
    తిరిగి nr;
}

    స్థిర పూర్ణాంకం
current_tab_nr(tabpage_T *tab)
{
    tabpage_T	*tp;
    పూర్ణాంకం		nr = 0;

    FOR_ALL_TABPAGES(tp)
    {
	++nr;
	ఒకవేళ (tp == tab)
	    ఆపు;
    }
    తిరిగి nr;
}

    స్థిర పూర్ణాంకం
comment_ప్రారంభం(char_u *p, పూర్ణాంకం starts_with_colon వాడని)
{
    ఒకవేళ (లోపల_vim9script())
	తిరిగి p[0] == '#' && !starts_with_colon;
    తిరిగి *p == '"';
}

#నిర్వచించు CURRENT_WIN_NR current_win_nr(curwin)
#నిర్వచించు చివరి_WIN_NR current_win_nr(ఖాళీ)
#నిర్వచించు CURRENT_TAB_NR current_tab_nr(curtab)
#నిర్వచించు చివరి_TAB_NR current_tab_nr(ఖాళీ)

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
    స్థిర char_u *
do_one_cmd(
    char_u	**cmdlinep,
    పూర్ణాంకం		flags,
#నిర్వచితమైతే FEAT_EVAL
    cstack_T	*cstack,
#అయితే_ముగింపు
    char_u	*(*fgetline)(పూర్ణాంకం, శూన్య *, పూర్ణాంకం, getline_opt_T),
    శూన్య	*cookie)		// argument for fgetline()
{
    char_u	*p;
    linenr_T	lnum;
    దీర్ఘ	n;
    అక్షరం	*errormsg = ఖాళీ;	// error message
    char_u	*after_modifier = ఖాళీ;
    exarg_T	ea;			// Ex command arguments
    cmdmod_T	save_cmdmod;
    పూర్ణాంకం		save_reg_executing = reg_executing;
    పూర్ణాంకం		save_pending_చివర_reg_executing = pending_చివర_reg_executing;
    పూర్ణాంకం		ni;			// set when Not Implemented
    char_u	*cmd;
    పూర్ణాంకం		starts_with_colon = FALSE;
    పూర్ణాంకం		may_have_range;
#నిర్వచితమైతే FEAT_EVAL
    పూర్ణాంకం		did_సమితి_సమాసం_పంక్తి = FALSE;
#అయితే_ముగింపు
    పూర్ణాంకం		sourcing = flags & DOCMD_VERBOSE;
    పూర్ణాంకం		did_append_cmd = FALSE;

    CLEAR_క్షేత్రం(ea);
    ea.line1 = 1;
    ea.line2 = 1;
#నిర్వచితమైతే FEAT_EVAL
    ++ex_nesting_level;
#అయితే_ముగింపు

    // When the last file has not been edited :q has to be typed twice.
    ఒకవేళ (quitmore
#నిర్వచితమైతే FEAT_EVAL
	    // avoid that a function call in 'statusline' does this
	    && !getline_equal(fgetline, cookie, get_func_పంక్తి)
#అయితే_ముగింపు
	    // avoid that an autocommand, e.g. QuitPre, does this
	    && !getline_equal(fgetline, cookie, getnextac))
	--quitmore;

    /*
     * Reset browse, confirm, etc..  They are restored when returning, for
     * recursive calls.
     */
    save_cmdmod = cmdmod;

    // "#!anything" is handled like a comment.
    ఒకవేళ ((*cmdlinep)[0] == '#' && (*cmdlinep)[1] == '!')
	వెళ్ళు doend;

/*
 * 1. Skip comment lines and leading white space and colons.
 * 2. Handle command modifiers.
 */
    // The "ea" structure holds the arguments that can be used.
    ea.cmd = *cmdlinep;
    ea.cmdlinep = cmdlinep;
    ea.ea_getline = fgetline;
    ea.cookie = cookie;
#నిర్వచితమైతే FEAT_EVAL
    ea.cstack = cstack;
    starts_with_colon = *skipwhite(ea.cmd) == ':';
#అయితే_ముగింపు
    ఒకవేళ (parse_command_modifiers(&ea, &errormsg, &cmdmod, FALSE) == FAIL)
	వెళ్ళు doend;
    apply_cmdmod(&cmdmod);
    after_modifier = ea.cmd;

#నిర్వచితమైతే FEAT_EVAL
    ea.skip = did_emsg || got_int || did_throw || (cstack->cs_idx >= 0
			 && !(cstack->cs_flags[cstack->cs_idx] & CSF_చురుకైన));
#లేకపోతే
    ea.skip = (if_level > 0);
#అయితే_ముగింపు

/*
 * 3. Skip over the range to find the command.  Let "p" point to after it.
 *
 * We need the command to know what kind of range it uses.
 */
    cmd = ea.cmd;

    // In Vim9 script a colon is required before the range.  This may also be
    // after command modifiers.
    పూర్ణాంకం vim9script = లోపల_vim9script();
    ఒకవేళ (vim9script && (flags & DOCMD_RANGEOK) == 0)
    {
	may_have_range = FALSE;
	కొరకు (p = ea.cmd; p >= *cmdlinep; --p)
	{
	    ఒకవేళ (*p == ':')
		may_have_range = TRUE;
	    ఒకవేళ (p < ea.cmd && !VIM_ISWHITE(*p))
		ఆపు;
	}
    }
    లేకపోతే
	may_have_range = TRUE;
    ఒకవేళ (may_have_range)
	ea.cmd = skip_range(ea.cmd, TRUE, ఖాళీ);

#నిర్వచితమైతే FEAT_EVAL
    // Handle ":export" - it functions almost like a command modifier.
    // ":export var Name: type"
    // ":export def Name(..."
    // etc.
    ఒకవేళ (vim9script && checkforcmd_noparen(&ea.cmd, "export", 6))
	is_export = TRUE;
#అయితే_ముగింపు

    ఒకవేళ (vim9script && !may_have_range)
    {
	ఒకవేళ (ea.cmd == cmd + 1 && *cmd == '$')
	    // should be "$VAR = val"
	    --ea.cmd;
#నిర్వచితమైతే FEAT_EVAL
	p = కనుగొను_ex_command(&ea, ఖాళీ, lookup_scriptitem, ఖాళీ);
#లేకపోతే
	p = కనుగొను_ex_command(&ea, ఖాళీ, ఖాళీ, ఖాళీ);
#అయితే_ముగింపు
	ఒకవేళ (ea.cmdidx == CMD_కొలత)
	{
	    char_u *ar = skip_range(ea.cmd, TRUE, ఖాళీ);

	    // If a ':' before the range is missing, give a clearer error
	    // message.
	    ఒకవేళ (ar > ea.cmd && !ea.skip)
	    {
		semsg(_(e_colon_required_before_range_str), ea.cmd);
		వెళ్ళు doend;
	    }
	}
    }
    లేకపోతే
	p = కనుగొను_ex_command(&ea, ఖాళీ, ఖాళీ, ఖాళీ);

#నిర్వచితమైతే FEAT_EVAL
# ifdef FEAT_PROFILE
    // Count this line for profiling if skip is TRUE.
    ఒకవేళ (do_profiling == PROF_YES
	    && (!ea.skip || cstack->cs_idx == 0 || (cstack->cs_idx > 0
		     && (cstack->cs_flags[cstack->cs_idx - 1] & CSF_చురుకైన))))
    {
	పూర్ణాంకం skip = did_emsg || got_int || did_throw;

	ఒకవేళ (ea.cmdidx == CMD_catch)
	    skip = !skip && !(cstack->cs_idx >= 0
			  && (cstack->cs_flags[cstack->cs_idx] & CSF_THROWN)
			  && !(cstack->cs_flags[cstack->cs_idx] & CSF_CAUGHT));
	లేకపోతే ఒకవేళ (ea.cmdidx == CMD_else || ea.cmdidx == CMD_elseif)
	    skip = skip || !(cstack->cs_idx >= 0
			  && !(cstack->cs_flags[cstack->cs_idx]
						  & (CSF_చురుకైన | CSF_TRUE)));
	లేకపోతే ఒకవేళ (ea.cmdidx == CMD_finally)
	    skip = FALSE;
	లేకపోతే ఒకవేళ (ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endtry
		&& ea.cmdidx != CMD_endwhile)
	    skip = ea.skip;

	ఒకవేళ (!skip)
	{
	    ఒకవేళ (getline_equal(fgetline, cookie, get_func_పంక్తి))
		func_పంక్తి_exec(getline_cookie(fgetline, cookie));
	    లేకపోతే ఒకవేళ (getline_equal(fgetline, cookie, getsourceline))
		script_పంక్తి_exec();
	}
    }
# endif
#అయితే_ముగింపు

    ea.cmd = cmd;

#నిర్వచితమైతే FEAT_EVAL
    // May go to debug mode.  If this happens and the ">quit" debug command is
    // used, throw an interrupt exception and skip the next command.
    dbg_check_breakpoint(&ea);
    ఒకవేళ (!ea.skip && got_int)
    {
	ea.skip = TRUE;
	(శూన్య)do_intthrow(cstack);
    }
#అయితే_ముగింపు

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
    ఒకవేళ (!IS_USER_CMDIDX(ea.cmdidx))
    {
	ఒకవేళ (ea.cmdidx != CMD_కొలత)
	    ea.addr_type = cmdnames[(పూర్ణాంకం)ea.cmdidx].cmd_addr_type;
	లేకపోతే
	    ea.addr_type = ADDR_పంక్తులు;

	// :wincmd range depends on the argument.
	ఒకవేళ (ea.cmdidx == CMD_wincmd && p != ఖాళీ)
	    get_wincmd_addr_type(skipwhite(p), &ea);
#నిర్వచితమైతే FEAT_QUICKFIX
	// :.cc in quickfix window uses line number
	ఒకవేళ ((ea.cmdidx == CMD_cc || ea.cmdidx == CMD_ll) && bt_quickfix(curbuf))
	    ea.addr_type = ADDR_OTHER;
#అయితే_ముగింపు
    }

    ఒకవేళ (!may_have_range)
	ea.line1 = ea.line2 = default_చిరునామా(&ea);
    లేకపోతే ఒకవేళ (parse_cmd_చిరునామా(&ea, &errormsg, FALSE) == FAIL)
	వెళ్ళు doend;

/*
 * 5. Parse the command.
 */

    /*
     * Skip ':' and any white space
     */
    ea.cmd = skipwhite(ea.cmd);
    అయితే (*ea.cmd == ':')
	ea.cmd = skipwhite(ea.cmd + 1);

    /*
     * If we got a line, but no command, then go to the line.
     * If we find a '|' or '\n' we set ea.nextcmd.
     */
    ఒకవేళ (*ea.cmd == NUL || comment_ప్రారంభం(ea.cmd, starts_with_colon)
			       || (ea.nextcmd = check_nextcmd(ea.cmd)) != ఖాళీ)
    {
	/*
	 * strange vi behaviour:
	 * ":3"		jumps to line 3
	 * ":3|..."	prints line 3  (not in Vim9 script)
	 * ":|"		prints current line  (not in Vim9 script)
	 */
	ఒకవేళ (ea.skip)	    // skip this if inside :if
	    వెళ్ళు doend;
	errormsg = ex_range_without_command(&ea);
	వెళ్ళు doend;
    }

    // If this looks like an undefined user command and there are CmdUndefined
    // autocommands defined, trigger the matching autocommands.
    ఒకవేళ (p != ఖాళీ && ea.cmdidx == CMD_కొలత && !ea.skip
	    && ASCII_ISUPPER(*ea.cmd)
	    && has_cmdundefined())
    {
	పూర్ణాంకం ret;

	p = ea.cmd;
	అయితే (ASCII_ISALNUM(*p))
	    ++p;
	p = vim_strnsave(ea.cmd, p - ea.cmd);
	ret = apply_autocmds(EVENT_CMDUNDEFINED, p, p, TRUE, ఖాళీ);
	vim_free(p);
	// If the autocommands did something and didn't cause an error, try
	// finding the command again.
	p = (ret
#నిర్వచితమైతే FEAT_EVAL
		&& !aborting()
#అయితే_ముగింపు
		) ? కనుగొను_ex_command(&ea, ఖాళీ, ఖాళీ, ఖాళీ) : ea.cmd;
    }

    ఒకవేళ (p == ఖాళీ)
    {
	ఒకవేళ (!ea.skip)
	    errormsg = _(e_ambiguous_use_యొక్క_user_defined_command);
	వెళ్ళు doend;
    }
    // Check for wrong commands.
    ఒకవేళ (*p == '!' && ea.cmd[1] == 0151 && ea.cmd[0] == 78
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	errormsg = uc_fun_cmd();
	వెళ్ళు doend;
    }

    ఒకవేళ (ea.cmdidx == CMD_కొలత)
    {
	ఒకవేళ (!ea.skip)
	{
	    STRCPY(IObuff, _(e_not_an_editor_command));
	    ఒకవేళ (!sourcing)
	    {
		// If the modifier was parsed OK the error must be in the
		// following command
		ఒకవేళ (after_modifier != ఖాళీ)
		    append_command(after_modifier);
		లేకపోతే
		    append_command(*cmdlinep);
		did_append_cmd = TRUE;
	    }
	    errormsg = (అక్షరం *)IObuff;
	    did_emsg_syntax = TRUE;
	}
	వెళ్ళు doend;
    }

    ni = (!IS_USER_CMDIDX(ea.cmdidx)
	    && (cmdnames[ea.cmdidx].cmd_func == ex_ni
#నిర్వచితమైతే HAVE_EX_SCRIPT_NI
	     || cmdnames[ea.cmdidx].cmd_func == ex_script_ni
#అయితే_ముగింపు
	     ));

#నిర్వచితం_కాకపోతే FEAT_EVAL
    /*
     * When the expression evaluation is disabled, recognize the ":if" and
     * ":endif" commands and ignore everything in between it.
     */
    ఒకవేళ (ea.cmdidx == CMD_if)
	++if_level;
    ఒకవేళ (if_level)
    {
	ఒకవేళ (ea.cmdidx == CMD_endif)
	    --if_level;
	వెళ్ళు doend;
    }

#అయితే_ముగింపు

    // forced commands
    ఒకవేళ (*p == '!' && ea.cmdidx != CMD_substitute
	    && ea.cmdidx != CMD_smagic && ea.cmdidx != CMD_snomagic)
    {
	++p;
	ea.forceit = TRUE;
    }
    లేకపోతే
	ea.forceit = FALSE;

/*
 * 6. Parse arguments.  Then check for errors.
 */
    ఒకవేళ (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = (దీర్ఘ)cmdnames[(పూర్ణాంకం)ea.cmdidx].cmd_argt;

    ఒకవేళ (!ea.skip)
    {
#నిర్వచితమైతే HAVE_SANDBOX
	ఒకవేళ (sandbox != 0 && !(ea.argt & EX_SBOXOK))
	{
	    // Command not allowed in sandbox.
	    errormsg = _(e_not_allowed_లోపల_sandbox);
	    వెళ్ళు doend;
	}
#అయితే_ముగింపు
	ఒకవేళ (restricted != 0 && (ea.argt & EX_RESTRICT))
	{
	    errormsg = _(e_command_not_allowed_లోపల_rvim);
	    వెళ్ళు doend;
	}
	ఒకవేళ (!curbuf->b_p_ma && (ea.argt & EX_MODIFY))
	{
	    // Command not allowed in non-'modifiable' buffer
	    errormsg = _(e_cannot_make_changes_modifiable_is_off);
	    వెళ్ళు doend;
	}

	ఒకవేళ (!IS_USER_CMDIDX(ea.cmdidx))
	{
	    ఒకవేళ (cmdwin_type != 0 && !(ea.argt & EX_CMDWIN))
	    {
		// Command not allowed in the command line window
		errormsg = _(e_invalid_లోపల_cmdline_కిటికీ);
		వెళ్ళు doend;
	    }
	    ఒకవేళ (పాఠ్యం_locked() && !(ea.argt & EX_తాళం_OK))
	    {
		// Command not allowed when text is locked
		errormsg = _(get_పాఠ్యం_locked_సందేశం());
		వెళ్ళు doend;
	    }
	}

	// Disallow editing another buffer when "curbuf_lock" is set.
	// Do allow ":checktime" (it is postponed).
	// Do allow ":edit" (check for an argument later).
	// Do allow ":file" with no arguments (check for an argument later).
	ఒకవేళ (!(ea.argt & (EX_CMDWIN | EX_తాళం_OK))
		&& ea.cmdidx != CMD_checktime
		&& ea.cmdidx != CMD_edit
		&& ea.cmdidx != CMD_file
		&& !IS_USER_CMDIDX(ea.cmdidx)
		&& curbuf_locked())
	    వెళ్ళు doend;

	ఒకవేళ (!ni && !(ea.argt & EX_RANGE) && ea.addr_లెక్క > 0)
	{
	    errormsg = _(e_no_range_allowed);
	    వెళ్ళు doend;
	}
    }

    ఒకవేళ (!ni && !(ea.argt & EX_BANG) && ea.forceit)
    {
	errormsg = _(e_no_bang_allowed);
	వెళ్ళు doend;
    }

    /*
     * Don't complain about the range if it is not used
     * (could happen if line_count is accidentally set to 0).
     */
    ఒకవేళ (!ea.skip && !ni && (ea.argt & EX_RANGE))
    {
	/*
	 * If the range is backwards, ask for confirmation and, if given, swap
	 * ea.line1 & ea.line2 so it's forwards again.
	 * When global command is busy, don't ask, will fail below.
	 */
	ఒకవేళ (!global_busy && ea.line1 > ea.line2)
	{
	    ఒకవేళ (సందేశం_silent == 0)
	    {
		ఒకవేళ (sourcing || exmode_చురుకైన)
		{
		    errormsg = _(e_backwards_range_given);
		    వెళ్ళు doend;
		}
		ఒకవేళ (ask_yesno((char_u *)
			_("Backwards range given, OK to swap"), FALSE) != 'y')
		    వెళ్ళు doend;
	    }
	    lnum = ea.line1;
	    ea.line1 = ea.line2;
	    ea.line2 = lnum;
	}
	ఒకవేళ ((errormsg = invalid_range(&ea)) != ఖాళీ)
	    వెళ్ళు doend;
    }

    ఒకవేళ ((ea.addr_type == ADDR_OTHER) && ea.addr_లెక్క == 0)
	// default is 1, not cursor
	ea.line2 = 1;

    correct_range(&ea);

#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ (((ea.argt & EX_WHOLEFOLD) || ea.addr_లెక్క >= 2) && !global_busy
	    && ea.addr_type == ADDR_పంక్తులు)
    {
	// Put the first line at the start of a closed fold, put the last line
	// at the end of a closed fold.
	(శూన్య)hasFolding(ea.line1, &ea.line1, ఖాళీ);
	(శూన్య)hasFolding(ea.line2, ఖాళీ, &ea.line2);
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_QUICKFIX
    /*
     * For the ":make" and ":grep" commands we insert the 'makeprg'/'grepprg'
     * option here, so things like % get expanded.
     */
    p = replace_makeprg(&ea, p, cmdlinep);
    ఒకవేళ (p == ఖాళీ)
	వెళ్ళు doend;
#అయితే_ముగింపు

    /*
     * Skip to start of argument.
     * Don't do this for the ":!" command, because ":!! -l" needs the space.
     */
    ఒకవేళ (ea.cmdidx == CMD_bang)
	ea.arg = p;
    లేకపోతే
	ea.arg = skipwhite(p);

    // ":file" cannot be run with an argument when "curbuf_lock" is set
    ఒకవేళ (ea.cmdidx == CMD_file && *ea.arg != NUL && curbuf_locked())
	వెళ్ళు doend;

    /*
     * Check for "++opt=val" argument.
     * Must be first, allow ":w ++enc=utf8 !cmd"
     */
    ఒకవేళ (ea.argt & EX_ARGOPT)
	అయితే (ea.arg[0] == '+' && ea.arg[1] == '+')
	    ఒకవేళ (getargopt(&ea) == FAIL && !ni)
	    {
		errormsg = _(e_invalid_వాదం);
		వెళ్ళు doend;
	    }

    ఒకవేళ (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	ఒకవేళ (*ea.arg == '>')			// append
	{
	    ఒకవేళ (*++ea.arg != '>')		// typed wrong
	    {
		errormsg = _(e_use_w_or_w_gt_gt);
		వెళ్ళు doend;
	    }
	    ea.arg = skipwhite(ea.arg + 1);
	    ea.append = TRUE;
	}
	లేకపోతే ఒకవేళ (*ea.arg == '!' && ea.cmdidx == CMD_write)  // :w !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    ఒకవేళ (ea.cmdidx == CMD_read)
    {
	ఒకవేళ (ea.forceit)
	{
	    ea.usefilter = TRUE;		// :r! filter if ea.forceit
	    ea.forceit = FALSE;
	}
	లేకపోతే ఒకవేళ (*ea.arg == '!')		// :r !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    ఒకవేళ (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ea.amount = 1;
	అయితే (*ea.arg == *ea.cmd)		// count number of '>' or '<'
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
    ఒకవేళ ((ea.argt & EX_CMDARG) && !ea.usefilter)
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
    ఒకవేళ ((ea.argt & EX_TRLBAR) && !ea.usefilter)
    {
	separate_nextcmd(&ea, FALSE);
    }
    లేకపోతే ఒకవేళ (ea.cmdidx == CMD_bang
	    || ea.cmdidx == CMD_terminal
	    || ea.cmdidx == CMD_global
	    || ea.cmdidx == CMD_vglobal
	    || ea.usefilter
#నిర్వచితమైతే FEAT_EVAL
	    || inside_ఖండం(&ea)
#అయితే_ముగింపు
	    )
    {
	కొరకు (p = ea.arg; *p; ++p)
	{
	    // Remove one backslash before a newline
	    ఒకవేళ (*p == '\\' && p[1] == '\n')
		STRMOVE(p, p + 1);
	    లేకపోతే ఒకవేళ (*p == '\n' && !(ea.argt & EX_సమాసం_ARG))
	    {
		ea.nextcmd = p + 1;
		*p = NUL;
		ఆపు;
	    }
	}
    }

    ఒకవేళ ((ea.argt & EX_DFLALL) && ea.addr_లెక్క == 0)
	చిరునామా_default_all(&ea);

    // accept numbered register only when no count allowed (:put)
    ఒకవేళ (       (ea.argt & EX_REGSTR)
	    && *ea.arg != NUL
	       // Do not allow register = for user commands
	    && (!IS_USER_CMDIDX(ea.cmdidx) || *ea.arg != '=')
	    && !((ea.argt & EX_లెక్క) && VIM_ISDIGIT(*ea.arg)))
    {
#ఒకవేళ !defined(FEAT_CLIPBOARD) && !defined(FEAT_CLIPBOARD_PROVIDER)
	// check these explicitly for a more specific error message
	ఒకవేళ (*ea.arg == '*' || *ea.arg == '+')
	{
	    errormsg = _(e_invalid_రిజిస్టరు_name);
	    వెళ్ళు doend;
	}
#అయితే_ముగింపు
	ఒకవేళ (valid_yank_reg(*ea.arg, (!IS_USER_CMDIDX(ea.cmdidx)
			    && ea.cmdidx != CMD_put && ea.cmdidx != CMD_iput)))
	{
	    ea.regname = *ea.arg++;
#నిర్వచితమైతే FEAT_EVAL
	    // for '=' register: accept the rest of the line as an expression
	    ఒకవేళ (ea.arg[-1] == '=' && ea.arg[0] != NUL)
	    {
		ఒకవేళ (!ea.skip)
		{
		    సమితి_సమాసం_పంక్తి(vim_strsave(ea.arg), &ea);
		    did_సమితి_సమాసం_పంక్తి = TRUE;
		}
		ea.arg += STRLEN(ea.arg);
	    }
#అయితే_ముగింపు
	    ea.arg = skipwhite(ea.arg);
	}
    }

    /*
     * Check for a count.  When accepting a EX_BUFNAME, don't use "123foo" as a
     * count, it's a buffer name.
     */
    ఒకవేళ ((ea.argt & EX_లెక్క) && VIM_ISDIGIT(*ea.arg)
	    && (!(ea.argt & EX_BUFNAME) || *(p = skipdigits(ea.arg + 1)) == NUL
							  || VIM_ISWHITE(*p)))
    {
	n = getdigits_quoted(&ea.arg);
	ea.arg = skipwhite(ea.arg);
	ఒకవేళ (n <= 0 && !ni && (ea.argt & EX_ZEROR) == 0)
	{
	    errormsg = _(e_ధనాత్మక_లెక్క_required);
	    వెళ్ళు doend;
	}
	ఒకవేళ (ea.addr_type != ADDR_పంక్తులు)	// e.g. :buffer 2, :sleep 3
	{
	    ea.line2 = n;
	    ఒకవేళ (ea.addr_లెక్క == 0)
		ea.addr_లెక్క = 1;
	}
	లేకపోతే
	{
	    ea.line1 = ea.line2;
	    ఒకవేళ (ea.line2 >= LONG_MAX - (n - 1))
		ea.line2 = LONG_MAX;  // avoid overflow
	    లేకపోతే
		ea.line2 += n - 1;
	    ++ea.addr_లెక్క;
	    /*
	     * Be vi compatible: no error message for out of range.
	     */
	    ఒకవేళ (ea.line2 > curbuf->b_ml.ml_పంక్తి_లెక్క)
		ea.line2 = curbuf->b_ml.ml_పంక్తి_లెక్క;
	}
    }

    /*
     * Check for flags: 'l', 'p' and '#'.
     */
    ఒకవేళ (ea.argt & EX_FLAGS)
	get_flags(&ea);
    ఒకవేళ (!ni && !(ea.argt & EX_EXTRA) && *ea.arg != NUL
	    && *ea.arg != '"' && (*ea.arg != '|' || (ea.argt & EX_TRLBAR) == 0))
    {
	// no arguments allowed but there is something
	errormsg = ex_errmsg(e_trailing_characters_str, ea.arg);
	వెళ్ళు doend;
    }

    ఒకవేళ (!ni && (ea.argt & EX_NEEDARG) && *ea.arg == NUL)
    {
	errormsg = _(e_వాదం_required);
	వెళ్ళు doend;
    }

#నిర్వచితమైతే FEAT_EVAL
    /*
     * Skip the command when it's not going to be executed.
     * The commands like :if, :endif, etc. always need to be executed.
     * Also make an exception for commands that handle a trailing command
     * themselves.
     */
    ఒకవేళ (ea.skip)
    {
	ఎంపిక (ea.cmdidx)
	{
	    // commands that need evaluation
	    స్థితి CMD_while:
	    స్థితి CMD_endwhile:
	    స్థితి CMD_for:
	    స్థితి CMD_endfor:
	    స్థితి CMD_if:
	    స్థితి CMD_elseif:
	    స్థితి CMD_else:
	    స్థితి CMD_endif:
	    స్థితి CMD_try:
	    స్థితి CMD_catch:
	    స్థితి CMD_finally:
	    స్థితి CMD_endtry:
	    స్థితి CMD_function:
	    స్థితి CMD_def:
				ఆపు;

	    // commands that read a block of lines
	    స్థితి CMD_abstract:
	    స్థితి CMD_append:
	    స్థితి CMD_autocmd:
	    స్థితి CMD_change:
	    స్థితి CMD_class:
	    స్థితి CMD_command:
	    స్థితి CMD_enum:
	    స్థితి CMD_చొప్పించు:
	    స్థితి CMD_interface:
	    స్థితి CMD_loadkeymap:
				ఆపు;

	    // Commands that handle '|' themselves.  Check: A command should
	    // either have the EX_TRLBAR flag, appear in this list or appear in
	    // the list at ":help :bar".
	    స్థితి CMD_aboveleft:
	    స్థితి CMD_and:
	    స్థితి CMD_belowright:
	    స్థితి CMD_botright:
	    స్థితి CMD_browse:
	    స్థితి CMD_call:
	    స్థితి CMD_confirm:
	    స్థితి CMD_const:
	    స్థితి CMD_delfunction:
	    స్థితి CMD_djump:
	    స్థితి CMD_dlist:
	    స్థితి CMD_dsearch:
	    స్థితి CMD_dsplit:
	    స్థితి CMD_echo:
	    స్థితి CMD_echoerr:
	    స్థితి CMD_echomsg:
	    స్థితి CMD_echon:
	    స్థితి CMD_eval:
	    స్థితి CMD_execute:
	    స్థితి CMD_filter:
	    స్థితి CMD_final:
	    స్థితి CMD_help:
	    స్థితి CMD_hide:
	    స్థితి CMD_horizontal:
	    స్థితి CMD_ijump:
	    స్థితి CMD_ilist:
	    స్థితి CMD_isearch:
	    స్థితి CMD_isplit:
	    స్థితి CMD_keepalt:
	    స్థితి CMD_keepjumps:
	    స్థితి CMD_keepmarks:
	    స్థితి CMD_keeppatterns:
	    స్థితి CMD_leftabove:
	    స్థితి CMD_let:
	    స్థితి CMD_lockmarks:
	    స్థితి CMD_lockvar:
	    స్థితి CMD_lua:
	    స్థితి CMD_సరిపోలిక:
	    స్థితి CMD_mzscheme:
	    స్థితి CMD_noautocmd:
	    స్థితి CMD_noswapfile:
	    స్థితి CMD_perl:
	    స్థితి CMD_psearch:
	    స్థితి CMD_py3:
	    స్థితి CMD_python3:
	    స్థితి CMD_python:
	    స్థితి CMD_return:
	    స్థితి CMD_rightbelow:
	    స్థితి CMD_ruby:
	    స్థితి CMD_silent:
	    స్థితి CMD_smagic:
	    స్థితి CMD_snomagic:
	    స్థితి CMD_substitute:
	    స్థితి CMD_syntax:
	    స్థితి CMD_tab:
	    స్థితి CMD_tcl:
	    స్థితి CMD_throw:
	    స్థితి CMD_tilde:
	    స్థితి CMD_topleft:
	    స్థితి CMD_unlet:
	    స్థితి CMD_unlockvar:
	    స్థితి CMD_var:
	    స్థితి CMD_verbose:
	    స్థితి CMD_vertical:
	    స్థితి CMD_wincmd:
				ఆపు;

	    అప్రమేయ:		వెళ్ళు doend;
	}
    }
#అయితే_ముగింపు

    ఒకవేళ ((ea.argt & EX_XFILE)
	    && expand_filename(&ea, cmdlinep, &errormsg) == FAIL)
	వెళ్ళు doend;

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (is_export && (ea.argt & EX_EXPORT) == 0)
    {
	emsg(_(e_invalid_command_after_export));
	వెళ్ళు doend;
    }
#అయితే_ముగింపు

    /*
     * Accept buffer name.  Cannot be used at the same time with a buffer
     * number.  Don't do this for a user command.
     */
    ఒకవేళ ((ea.argt & EX_BUFNAME) && *ea.arg != NUL && ea.addr_లెక్క == 0
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * :bdelete, :bwipeout and :bunload take several arguments, separated
	 * by spaces: find next space (skipping over escaped characters).
	 * The others take one argument: ignore trailing spaces.
	 */
	ఒకవేళ (ea.cmdidx == CMD_bdelete || ea.cmdidx == CMD_bwipeout
						  || ea.cmdidx == CMD_bunload)
	    p = skiptowhite_esc(ea.arg);
	లేకపోతే
	{
	    p = ea.arg + STRLEN(ea.arg);
	    అయితే (p > ea.arg && VIM_ISWHITE(p[-1]))
		--p;
	}
	ea.line2 = buflist_findpat(ea.arg, p, (ea.argt & EX_BUFUNL) != 0,
								FALSE, FALSE);
	ఒకవేళ (ea.line2 < 0)	    // failed
	    వెళ్ళు doend;
	ea.addr_లెక్క = 1;
	ea.arg = skipwhite(p);
    }

    // The :try command saves the emsg_silent flag, reset it here when
    // ":silent! try" was used, it should only apply to :try itself.
    ఒకవేళ (ea.cmdidx == CMD_try && cmdmod.cmod_did_esilent > 0)
    {
	emsg_silent -= cmdmod.cmod_did_esilent;
	ఒకవేళ (emsg_silent < 0)
	    emsg_silent = 0;
	cmdmod.cmod_did_esilent = 0;
    }

/*
 * 7. Execute the command.
 */

    ఒకవేళ (IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * Execute a user-defined command.
	 */
	do_ucmd(&ea);
    }
    లేకపోతే
    {
	/*
	 * Call the function to execute the builtin command.
	 */
	(cmdnames[ea.cmdidx].cmd_func)(&ea);
	ఒకవేళ (ea.errmsg != ఖాళీ)
	    errormsg = ea.errmsg;
    }

#నిర్వచితమైతే FEAT_EVAL
    // A command will reset "is_export" when exporting an item.  If it is still
    // set something went wrong or the command was never executed.
    ఒకవేళ (!ea.skip && is_export)
    {
	ఒకవేళ (errormsg == ఖాళీ)
	    errormsg = _(e_export_with_invalid_వాదం);
	is_export = FALSE;
    }

    // Set flag that any command was executed, used by ex_vim9script().
    // Not if this was a command that wasn't executed or :endif.
    ఒకవేళ (sourcing_a_script(&ea)
	    && current_sctx.sc_sid > 0
	    && ea.cmdidx != CMD_endif
	    && (cstack->cs_idx < 0
		    || (cstack->cs_flags[cstack->cs_idx] & CSF_చురుకైన)))
	SCRIPT_అంశం(current_sctx.sc_sid)->sn_state = SN_STATE_HAD_COMMAND;

    /*
     * If the command just executed called do_cmdline(), any throw or ":return"
     * or ":finish" encountered there must also check the cstack of the still
     * active do_cmdline() that called this do_one_cmd().  Rethrow an uncaught
     * exception, or reanimate a returned function or finished script file and
     * return or finish it again.
     */
    ఒకవేళ (need_rethrow)
	do_throw(cstack);
    లేకపోతే ఒకవేళ (check_cstack)
    {
	ఒకవేళ (ఆధారం_ముగిసిన(fgetline, cookie))
	    do_ముగింపు(&ea, TRUE);
	లేకపోతే ఒకవేళ (getline_equal(fgetline, cookie, get_func_పంక్తి)
						   && current_func_returned())
	    do_return(&ea, TRUE, FALSE, ఖాళీ);
    }
    need_rethrow = check_cstack = FALSE;
#అయితే_ముగింపు

doend:
    ఒకవేళ (curwin->w_cursor.lnum == 0)	// can happen with zero line number
    {
	curwin->w_cursor.lnum = 1;
	curwin->w_cursor.నిలువు = 0;
    }

    ఒకవేళ (errormsg != ఖాళీ && *errormsg != NUL && !did_emsg)
    {
	ఒకవేళ ((sourcing || !కీలకం_Typed) && !did_append_cmd)
	{
	    ఒకవేళ (errormsg != (అక్షరం *)IObuff)
	    {
		STRCPY(IObuff, errormsg);
		errormsg = (అక్షరం *)IObuff;
	    }
	    append_command(*cmdlinep);
	}
	emsg(errormsg);
    }
#నిర్వచితమైతే FEAT_EVAL
    do_errthrow(cstack,
	    (ea.cmdidx != CMD_కొలత && !IS_USER_CMDIDX(ea.cmdidx))
			? cmdnames[(పూర్ణాంకం)ea.cmdidx].cmd_name : (char_u *)ఖాళీ);

    ఒకవేళ (did_సమితి_సమాసం_పంక్తి)
	సమితి_సమాసం_పంక్తి(ఖాళీ, ఖాళీ);
    is_export = FALSE;
#అయితే_ముగింపు

    undo_cmdmod(&cmdmod);
    cmdmod = save_cmdmod;
    reg_executing = save_reg_executing;
    pending_చివర_reg_executing = save_pending_చివర_reg_executing;

    ఒకవేళ (ea.nextcmd && *ea.nextcmd == NUL)	// not really a next command
	ea.nextcmd = ఖాళీ;

#నిర్వచితమైతే FEAT_EVAL
    --ex_nesting_level;
    vim_free(ea.cmdline_tofree);
#అయితే_ముగింపు

    తిరిగి ea.nextcmd;
}

స్థిర అక్షరం ex_error_buf[సందేశం_BUF_LEN];

/*
 * Return an error message with argument included.
 * Uses a static buffer, only the last error will be kept.
 * "msg" will be translated, caller should use N_().
 */
    అక్షరం *
ex_errmsg(అక్షరం *సందేశం, char_u *arg)
{
    vim_snprintf(ex_error_buf, సందేశం_BUF_LEN, _(సందేశం), arg);
    తిరిగి ex_error_buf;
}

/*
 * The "+" string used in place of an empty command in Ex mode.
 * This string is used in pointer comparison.
 */
స్థిర స్థిరరాశి అక్షరం exmode_plus[] = "+";

/*
 * Handle a range without a command.
 * Returns an error message on failure.
 */
    అక్షరం *
ex_range_without_command(exarg_T *eap)
{
    అక్షరం *errormsg = ఖాళీ;

    ఒకవేళ ((*eap->cmd == '|' ||
		(exmode_చురుకైన && eap->cmd != (char_u *)exmode_plus + 1))
#నిర్వచితమైతే FEAT_EVAL
	    && !లోపల_vim9script()
#అయితే_ముగింపు
       )
    {
	eap->cmdidx = CMD_print;
	eap->argt = EX_RANGE+EX_లెక్క+EX_TRLBAR;
	ఒకవేళ ((errormsg = invalid_range(eap)) == ఖాళీ)
	{
	    correct_range(eap);
	    ex_print(eap);
	}
    }
    లేకపోతే ఒకవేళ (eap->addr_లెక్క != 0)
    {
	ఒకవేళ (eap->line2 > curbuf->b_ml.ml_పంక్తి_లెక్క)
	{
	    // With '-' in 'cpoptions' a line number past the file is an
	    // error, otherwise put it at the end of the file.
	    ఒకవేళ (vim_strchr(p_cpo, CPO_MINUS) != ఖాళీ)
		eap->line2 = -1;
	    లేకపోతే
		eap->line2 = curbuf->b_ml.ml_పంక్తి_లెక్క;
	}

	ఒకవేళ (eap->line2 < 0)
	    errormsg = _(e_invalid_range);
	లేకపోతే
	{
	    ఒకవేళ (eap->line2 == 0)
		curwin->w_cursor.lnum = 1;
	    లేకపోతే
		curwin->w_cursor.lnum = eap->line2;
	    beginline(BL_SOL | BL_FIX);
	}
    }
    తిరిగి errormsg;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 * If "noparen" is TRUE do not recognize the command followed by "(" or ".".
 */
    స్థిర పూర్ణాంకం
checkforcmd_opt(
    char_u	**pp,		// start of command
    అక్షరం	*cmd,		// name of command
    పూర్ణాంకం		len,		// required length
    పూర్ణాంకం		noparen)
{
    పూర్ణాంకం		i;

    కొరకు (i = 0; cmd[i] != NUL; ++i)
	ఒకవేళ (((char_u *)cmd)[i] != (*pp)[i])
	    ఆపు;
    ఒకవేళ (i >= len && !ASCII_ISALPHA((*pp)[i]) && (*pp)[i] != '_'
			 && (!noparen || ((*pp)[i] != '(' && (*pp)[i] != '.')))
    {
	*pp = skipwhite(*pp + i);
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    పూర్ణాంకం
checkforcmd(
    char_u	**pp,		// start of command
    అక్షరం	*cmd,		// name of command
    పూర్ణాంకం		len)		// required length
{
    తిరిగి checkforcmd_opt(pp, cmd, len, FALSE);
}

/*
 * Check for an Ex command with optional tail, not followed by "(" or ".".
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    పూర్ణాంకం
checkforcmd_noparen(
    char_u	**pp,		// start of command
    అక్షరం	*cmd,		// name of command
    పూర్ణాంకం		len)		// required length
{
    తిరిగి checkforcmd_opt(pp, cmd, len, TRUE);
}

/*
 * Find the replacement text of a ":command", the part after the attributes and
 * the command name.  Returns NULL when there is none.  Does not modify "arg"
 * and gives no error messages.
 */
    స్థిర char_u *
కనుగొను_ucmd_repl(char_u *arg)
{
    char_u	*p = arg;

    // Skip over the attributes.
    అయితే (*p == '-')
	p = skipwhite(skiptowhite(p));

    // Skip over the command name.
    ఒకవేళ (!ASCII_ISALPHA(*p))
	తిరిగి ఖాళీ;
    అయితే (ASCII_ISALNUM(*p))
	++p;

    తిరిగి *p == NUL ? ఖాళీ : skipwhite(p);
}

/*
 * Find the "{" in "line" that starts a block for ":command" or ":autocmd".
 * That is the case when the command argument is "{" at the end of the line,
 * also when the command is nested in another ":command" or ":autocmd".
 * Returns NULL when the line does not start such a block.
 */
    char_u *
కనుగొను_cmd_ఖండం_ప్రారంభం(char_u *పంక్తి)
{
    char_u	*p = skipwhite(పంక్తి);

    కొరకు (;;)
    {
	char_u	*arg = p;

	ఒకవేళ (*p == '{' && చివర్లు_excmd2(p, skipwhite(p + 1)))
	    తిరిగి p;

	ఒకవేళ (checkforcmd_noparen(&arg, "autocmd", 2))
	{
	    ఒకవేళ (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = au_కనుగొను_cmd_arg(arg);
	}
	లేకపోతే ఒకవేళ (checkforcmd_noparen(&arg, "command", 3))
	{
	    ఒకవేళ (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = కనుగొను_ucmd_repl(arg);
	}
	లేకపోతే
	    తిరిగి ఖాళీ;

	ఒకవేళ (p == ఖాళీ)
	    తిరిగి ఖాళీ;
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
    పూర్ణాంకం
parse_command_modifiers(
	exarg_T	    *eap,
	అక్షరం	    **errormsg,
	cmdmod_T    *cmod,
	పూర్ణాంకం	    skip_only)
{
    char_u  *orig_cmd = eap->cmd;
    char_u  *cmd_ప్రారంభం = ఖాళీ;
    పూర్ణాంకం	    use_plus_cmd = FALSE;
    పూర్ణాంకం	    starts_with_colon = FALSE;
    పూర్ణాంకం	    vim9script = లోపల_vim9script();
    పూర్ణాంకం	    has_visual_range = FALSE;

    CLEAR_సూచిక(cmod);
    cmod->cmod_flags = sticky_cmdmod_flags;

    ఒకవేళ (STRNCMP(eap->cmd, "'<,'>", 5) == 0)
    {
	// The automatically inserted Visual area range is skipped, so that
	// typing ":cmdmod cmd" in Visual mode works without having to move the
	// range to after the modifiers. The command will be "'<,'>cmdmod cmd",
	// parse "cmdmod cmd" and then put back "'<,'>" before "cmd" below.
	eap->cmd += 5;
	cmd_ప్రారంభం = eap->cmd;
	has_visual_range = TRUE;
    }

    // Repeat until no more command modifiers are found.
    కొరకు (;;)
    {
	char_u  *p;

	అయితే (*eap->cmd == ' ' || *eap->cmd == '\t' || *eap->cmd == ':')
	{
	    ఒకవేళ (*eap->cmd == ':')
		starts_with_colon = TRUE;
	    ++eap->cmd;
	}

	// in ex mode, an empty command (after modifiers) works like :+
	ఒకవేళ (*eap->cmd == NUL && exmode_చురుకైన
		   && (getline_equal(eap->ea_getline, eap->cookie, getexmodeline)
		       || getline_equal(eap->ea_getline, eap->cookie, getexline))
			&& curwin->w_cursor.lnum < curbuf->b_ml.ml_పంక్తి_లెక్క)
	{
	    use_plus_cmd = TRUE;
	    ఒకవేళ (!skip_only)
		ex_pressedreturn = TRUE;
	    ఆపు;  // no modifiers following
	}

	// ignore comment and empty lines
	ఒకవేళ (comment_ప్రారంభం(eap->cmd, starts_with_colon))
	{
	    // a comment ends at a NL
	    eap->nextcmd = vim_strchr(eap->cmd, '\n');
	    ఒకవేళ (eap->nextcmd != ఖాళీ)
		++eap->nextcmd;
	    ఒకవేళ (vim9script)
	    {
		ఒకవేళ (has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
#నిర్వచితమైతే FEAT_EVAL
		ఒకవేళ (eap->cmd[0] == '#' && eap->cmd[1] == '{'
							 && eap->cmd[2] != '{')
		    *errormsg = _(e_cannot_use_hash_curly_గమ్యం_ప్రారంభం_comment);
#అయితే_ముగింపు
	    }
	    తిరిగి FAIL;
	}
	ఒకవేళ (*eap->cmd == '\n')
	{
	    eap->nextcmd = eap->cmd + 1;
	    తిరిగి FAIL;
	}
	ఒకవేళ (*eap->cmd == NUL)
	{
	    ఒకవేళ (!skip_only)
	    {
		ex_pressedreturn = TRUE;
		ఒకవేళ (vim9script && has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
	    }
	    తిరిగి FAIL;
	}

	p = skip_range(eap->cmd, TRUE, ఖాళీ);

	// In Vim9 script a variable can shadow a command modifier:
	//   verbose = 123
	//   verbose += 123
	//   silent! verbose = func()
	//   verbose.member = 2
	//   verbose[expr] = 2
	// But not:
	//   verbose [a, b] = list
	ఒకవేళ (vim9script)
	{
	    char_u *s, *n;

	    కొరకు (s = eap->cmd; ASCII_ISALPHA(*s); ++s)
		;
	    n = skipwhite(s);
	    ఒకవేళ (*n == '.' || *n == '=' || (*n != NUL && n[1] == '=')
		    || *s == '[')
		ఆపు;
	}

	ఎంపిక (*p)
	{
	    // When adding an entry, also modify cmdmod_info_tab[].
	    స్థితి 'a':	ఒకవేళ (!checkforcmd_noparen(&eap->cmd, "aboveleft", 3))
			    ఆపు;
			cmod->cmod_విభజించు |= WSP_ABOVE;
			కొనసాగు;

	    స్థితి 'b':	ఒకవేళ (checkforcmd_noparen(&eap->cmd, "belowright", 3))
			{
			    cmod->cmod_విభజించు |= WSP_క్రింద;
			    కొనసాగు;
			}
			ఒకవేళ (checkforcmd_opt(&eap->cmd, "browse", 3, TRUE))
			{
#నిర్వచితమైతే FEAT_BROWSE_CMD
			    cmod->cmod_flags |= CMOD_BROWSE;
#అయితే_ముగింపు
			    కొనసాగు;
			}
			ఒకవేళ (!checkforcmd_noparen(&eap->cmd, "botright", 2))
			    ఆపు;
			cmod->cmod_విభజించు |= WSP_BOT;
			కొనసాగు;

	    స్థితి 'c':	ఒకవేళ (!checkforcmd_opt(&eap->cmd, "confirm", 4, TRUE))
			    ఆపు;
#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			cmod->cmod_flags |= CMOD_CONFIRM;
#అయితే_ముగింపు
			కొనసాగు;

	    స్థితి 'k':	ఒకవేళ (checkforcmd_noparen(&eap->cmd, "keepmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_KEEPMARKS;
			    కొనసాగు;
			}
			ఒకవేళ (checkforcmd_noparen(&eap->cmd, "keepalt", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPALT;
			    కొనసాగు;
			}
			ఒకవేళ (checkforcmd_noparen(&eap->cmd, "keeppatterns", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPPATTERNS;
			    కొనసాగు;
			}
			ఒకవేళ (!checkforcmd_noparen(&eap->cmd, "keepjumps", 5))
			    ఆపు;
			cmod->cmod_flags |= CMOD_KEEPJUMPS;
			కొనసాగు;

	    స్థితి 'f':	// only accept ":filter {pat} cmd"
			{
			    char_u  *reg_pat;
			    char_u  *nulp = ఖాళీ;
			    పూర్ణాంకం	    c = 0;

			    ఒకవేళ (!checkforcmd_noparen(&p, "filter", 4)
				    || *p == NUL
				    || (చివర్లు_excmd(*p)
#నిర్వచితమైతే FEAT_EVAL
					// in ":filter #pat# cmd" # does not
					// start a comment
				     && (!vim9script || VIM_ISWHITE(p[1]))
#అయితే_ముగింపు
				     ))
				ఆపు;
			    ఒకవేళ (*p == '!')
			    {
				cmod->cmod_filter_force = TRUE;
				p = skipwhite(p + 1);
				ఒకవేళ (*p == NUL || చివర్లు_excmd(*p))
				    ఆపు;
			    }
#నిర్వచితమైతే FEAT_EVAL
			    // Avoid that "filter(arg)" is recognized.
			    ఒకవేళ (vim9script && !VIM_ISWHITE(p[-1]))
				ఆపు;
#అయితే_ముగింపు
			    ఒకవేళ (skip_only)
				p = skip_vimgrep_pat(p, ఖాళీ, ఖాళీ);
			    లేకపోతే
				// NOTE: This puts a NUL after the pattern.
				p = skip_vimgrep_pat_ext(p, &reg_pat, ఖాళీ,
								    &nulp, &c);
			    ఒకవేళ (p == ఖాళీ || *p == NUL)
				ఆపు;
			    ఒకవేళ (!skip_only)
			    {
				cmod->cmod_filter_regmatch.regprog =
						vim_regcomp(reg_pat, RE_MAGIC);
				ఒకవేళ (cmod->cmod_filter_regmatch.regprog == ఖాళీ)
				    ఆపు;
				// restore the character overwritten by NUL
				ఒకవేళ (nulp != ఖాళీ)
				    *nulp = c;
			    }
			    eap->cmd = p;
			    కొనసాగు;
			}

	    స్థితి 'h':	ఒకవేళ (checkforcmd_noparen(&eap->cmd, "horizontal", 3))
			{
			    cmod->cmod_విభజించు |= WSP_HOR;
			    కొనసాగు;
			}
			// ":hide" and ":hide | cmd" are not modifiers
			ఒకవేళ (p != eap->cmd || !checkforcmd_noparen(&p, "hide", 3)
					       || *p == NUL || చివర్లు_excmd(*p))
			    ఆపు;
			eap->cmd = p;
			cmod->cmod_flags |= CMOD_HIDE;
			కొనసాగు;

	    స్థితి 'l':	ఒకవేళ (checkforcmd_noparen(&eap->cmd, "lockmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_LOCKMARKS;
			    కొనసాగు;
			}
			ఒకవేళ (checkforcmd_noparen(&eap->cmd, "legacy", 3))
			{
			    ఒకవేళ (చివర్లు_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_legacy_must_be_followed_by_command);
				తిరిగి FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_VIM9CMD;
			    cmod->cmod_flags |= CMOD_LEGACY;
			    కొనసాగు;
			}

			ఒకవేళ (!checkforcmd_noparen(&eap->cmd, "leftabove", 5))
			    ఆపు;
			cmod->cmod_విభజించు |= WSP_ABOVE;
			కొనసాగు;

	    స్థితి 'n':	ఒకవేళ (checkforcmd_noparen(&eap->cmd, "noautocmd", 3))
			{
			    cmod->cmod_flags |= CMOD_NOAUTOCMD;
			    కొనసాగు;
			}
			ఒకవేళ (!checkforcmd_noparen(&eap->cmd, "noswapfile", 3))
			    ఆపు;
			cmod->cmod_flags |= CMOD_NOSWAPFILE;
			కొనసాగు;

	    స్థితి 'r':	ఒకవేళ (!checkforcmd_noparen(&eap->cmd, "rightbelow", 6))
			    ఆపు;
			cmod->cmod_విభజించు |= WSP_క్రింద;
			కొనసాగు;

	    స్థితి 's':	ఒకవేళ (checkforcmd_noparen(&eap->cmd, "sandbox", 3))
			{
			    cmod->cmod_flags |= CMOD_SANDBOX;
			    కొనసాగు;
			}
			ఒకవేళ (!checkforcmd_noparen(&eap->cmd, "silent", 3))
			    ఆపు;
			cmod->cmod_flags |= CMOD_SILENT;
			ఒకవేళ (*eap->cmd == '!' && !VIM_ISWHITE(eap->cmd[-1]))
			{
			    // ":silent!", but not "silent !cmd"
			    eap->cmd = skipwhite(eap->cmd + 1);
			    cmod->cmod_flags |= CMOD_ERRSILENT;
			}
			కొనసాగు;

	    స్థితి 't':	ఒకవేళ (checkforcmd_noparen(&p, "tab", 3))
			{
			    ఒకవేళ (!skip_only)
			    {
				దీర్ఘ tabnr = get_చిరునామా(eap, &eap->cmd,
						    ADDR_TABS, eap->skip,
						    skip_only, FALSE, 1);
				ఒకవేళ (tabnr == MAXLNUM)
				    cmod->cmod_tab = tabpage_సూచీ(curtab) + 1;
				లేకపోతే
				{
				    ఒకవేళ (tabnr < 0 || tabnr > చివరి_TAB_NR)
				    {
					*errormsg = _(e_invalid_range);
					తిరిగి FAIL;
				    }
				    cmod->cmod_tab = tabnr + 1;
				}
			    }
			    eap->cmd = p;
			    కొనసాగు;
			}
			ఒకవేళ (!checkforcmd_noparen(&eap->cmd, "topleft", 2))
			    ఆపు;
			cmod->cmod_విభజించు |= WSP_శిఖరం;
			కొనసాగు;

	    స్థితి 'u':	ఒకవేళ (!checkforcmd_noparen(&eap->cmd, "unsilent", 3))
			    ఆపు;
			cmod->cmod_flags |= CMOD_UNSILENT;
			కొనసాగు;

	    స్థితి 'v':	ఒకవేళ (checkforcmd_noparen(&eap->cmd, "vertical", 4))
			{
			    cmod->cmod_విభజించు |= WSP_VERT;
			    కొనసాగు;
			}
			ఒకవేళ (checkforcmd_noparen(&eap->cmd, "vim9cmd", 4))
			{
			    ఒకవేళ (చివర్లు_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_vim9cmd_must_be_followed_by_command);
				తిరిగి FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_LEGACY;
			    cmod->cmod_flags |= CMOD_VIM9CMD;
			    కొనసాగు;
			}
			ఒకవేళ (!checkforcmd_noparen(&p, "verbose", 4))
			    ఆపు;
			ఒకవేళ (vim_isdigit(*eap->cmd))
			{
			    // zero means not set, one is verbose == 0, etc.
			    cmod->cmod_verbose = atoi((అక్షరం *)eap->cmd) + 1;
			}
			లేకపోతే
			    cmod->cmod_verbose = 2;  // default: verbose == 1
			eap->cmd = p;
			కొనసాగు;
	}
	ఆపు;
    }

    ఒకవేళ (has_visual_range)
    {
	ఒకవేళ (eap->cmd > cmd_ప్రారంభం)
	{
	    // Move the '<,'> range to after the modifiers and insert a colon.
	    // Since the modifiers have been parsed put the colon on top of the
	    // space: "'<,'>mod cmd" -> "mod:'<,'>cmd
	    // Put eap->cmd after the colon.
	    ఒకవేళ (use_plus_cmd)
	    {
		size_t len = STRLEN(cmd_ప్రారంభం);

		// Special case: empty command uses "+":
		//  "'<,'>mods" -> "mods *+
		//  Use "*" instead of "'<,'>" to avoid the command getting
		//  longer, in case it was allocated.
		mch_memmove(orig_cmd, cmd_ప్రారంభం, len);
		STRCPY(orig_cmd + len, " *+");
	    }
	    లేకపోతే
	    {
		mch_memmove(cmd_ప్రారంభం - 5, cmd_ప్రారంభం, eap->cmd - cmd_ప్రారంభం);
		eap->cmd -= 5;
		mch_memmove(eap->cmd - 1, ":'<,'>", 6);
	    }
	}
	లేకపోతే
	    // No modifiers, move the pointer back.
	    // Special case: change empty command to "+".
	    ఒకవేళ (use_plus_cmd)
		eap->cmd = (char_u *)"'<,'>+";
	    లేకపోతే
		eap->cmd = orig_cmd;
    }
    లేకపోతే ఒకవేళ (use_plus_cmd)
	eap->cmd = (char_u *)exmode_plus;

    తిరిగి OK;
}

/*
 * Return TRUE if "cmod" has anything set.
 */
    పూర్ణాంకం
has_cmdmod(cmdmod_T *cmod, పూర్ణాంకం ignore_silent)
{
    తిరిగి (cmod->cmod_flags != 0 && (!ignore_silent
		|| (cmod->cmod_flags
		      & ~(CMOD_SILENT | CMOD_ERRSILENT | CMOD_UNSILENT)) != 0))
	    || cmod->cmod_విభజించు != 0
	    || cmod->cmod_verbose > 0
	    || cmod->cmod_tab != 0
	    || cmod->cmod_filter_regmatch.regprog != ఖాళీ;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * If Vim9 script and "cmdmod" has anything set give an error and return TRUE.
 */
    పూర్ణాంకం
cmdmod_error(పూర్ణాంకం ignore_silent)
{
    ఒకవేళ (లోపల_vim9script() && has_cmdmod(&cmdmod, ignore_silent))
    {
	emsg(_(e_misplaced_command_modifier));
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}
#అయితే_ముగింపు

/*
 * Apply the command modifiers.  Saves current state in "cmdmod", call
 * undo_cmdmod() later.
 */
    శూన్య
apply_cmdmod(cmdmod_T *cmod)
{
#నిర్వచితమైతే HAVE_SANDBOX
    ఒకవేళ ((cmod->cmod_flags & CMOD_SANDBOX) && !cmod->cmod_did_sandbox)
    {
	++sandbox;
	cmod->cmod_did_sandbox = TRUE;
    }
#అయితే_ముగింపు
    ఒకవేళ (cmod->cmod_verbose > 0)
    {
	ఒకవేళ (cmod->cmod_verbose_save == 0)
	    cmod->cmod_verbose_save = p_verbose + 1;
	p_verbose = cmod->cmod_verbose - 1;
    }

    ఒకవేళ ((cmod->cmod_flags & (CMOD_SILENT | CMOD_UNSILENT))
	    && cmod->cmod_save_సందేశం_silent == 0)
    {
	cmod->cmod_save_సందేశం_silent = సందేశం_silent + 1;
	cmod->cmod_save_సందేశం_scroll = సందేశం_scroll;
    }
    ఒకవేళ (cmod->cmod_flags & CMOD_SILENT)
	++సందేశం_silent;
    ఒకవేళ (cmod->cmod_flags & CMOD_UNSILENT)
	సందేశం_silent = 0;

    ఒకవేళ (cmod->cmod_flags & CMOD_ERRSILENT)
    {
	++emsg_silent;
	++cmod->cmod_did_esilent;
    }

    ఒకవేళ ((cmod->cmod_flags & CMOD_NOAUTOCMD) && cmod->cmod_save_ei == ఖాళీ)
    {
	// Set 'eventignore' to "all".
	// First save the existing option value for restoring it later.
	cmod->cmod_save_ei = vim_strsave(p_ei);
	సమితి_string_option_ప్రత్యక్ష((char_u *)"ei", -1,
					  (char_u *)"all", OPT_FREE, SID_NONE);
    }
}

/*
 * Undo and free contents of "cmod".
 */
    శూన్య
undo_cmdmod(cmdmod_T *cmod)
{
    ఒకవేళ (cmod->cmod_verbose_save > 0)
    {
	p_verbose = cmod->cmod_verbose_save - 1;
	cmod->cmod_verbose_save = 0;
    }

#నిర్వచితమైతే HAVE_SANDBOX
    ఒకవేళ (cmod->cmod_did_sandbox)
    {
	--sandbox;
	cmod->cmod_did_sandbox = FALSE;
    }
#అయితే_ముగింపు

    ఒకవేళ (cmod->cmod_save_ei != ఖాళీ)
    {
	// Restore 'eventignore' to the value before ":noautocmd".
	సమితి_string_option_ప్రత్యక్ష((char_u *)"ei", -1, cmod->cmod_save_ei,
							   OPT_FREE, SID_NONE);
	free_string_option(cmod->cmod_save_ei);
	cmod->cmod_save_ei = ఖాళీ;
    }

    vim_regfree(cmod->cmod_filter_regmatch.regprog);

    ఒకవేళ (cmod->cmod_save_సందేశం_silent > 0)
    {
	// messages could be enabled for a serious error, need to check if the
	// counters don't become negative
	ఒకవేళ (!did_emsg || సందేశం_silent > cmod->cmod_save_సందేశం_silent - 1)
	    సందేశం_silent = cmod->cmod_save_సందేశం_silent - 1;
	emsg_silent -= cmod->cmod_did_esilent;
	ఒకవేళ (emsg_silent < 0)
	    emsg_silent = 0;
	// Restore msg_scroll, it's set by file I/O commands, even when no
	// message is actually displayed.
	సందేశం_scroll = cmod->cmod_save_సందేశం_scroll;

	// "silent reg" or "silent echo x" inside "redir" leaves msg_col
	// somewhere in the line.  Put it back in the first column.
	ఒకవేళ (redirecting())
	    సందేశం_నిలువు = 0;

	cmod->cmod_save_సందేశం_silent = 0;
	cmod->cmod_did_esilent = 0;
    }
}

/*
 * Parse the address range, if any, in "eap".
 * May set the last search pattern, unless "silent" is TRUE.
 * Return FAIL and set "errormsg" or return OK.
 */
    పూర్ణాంకం
parse_cmd_చిరునామా(exarg_T *eap, అక్షరం **errormsg, పూర్ణాంకం silent)
{
    పూర్ణాంకం		చిరునామా_లెక్క = 1;
    linenr_T	lnum;
    పూర్ణాంకం		need_check_cursor = FALSE;
    పూర్ణాంకం		ret = FAIL;

    // Repeat for all ',' or ';' separated addresses.
    కొరకు (;;)
    {
	eap->line1 = eap->line2;
	eap->line2 = default_చిరునామా(eap);
	eap->cmd = skipwhite(eap->cmd);
	lnum = get_చిరునామా(eap, &eap->cmd, eap->addr_type, eap->skip, silent,
					eap->addr_లెక్క == 0, చిరునామా_లెక్క++);
	ఒకవేళ (eap->cmd == ఖాళీ)	// error detected
	    వెళ్ళు theend;
	ఒకవేళ (lnum == MAXLNUM)
	{
	    ఒకవేళ (*eap->cmd == '%')   // '%' - all lines
	    {
		++eap->cmd;
		ఎంపిక (eap->addr_type)
		{
		    స్థితి ADDR_పంక్తులు:
		    స్థితి ADDR_OTHER:
			eap->line1 = 1;
			eap->line2 = curbuf->b_ml.ml_పంక్తి_లెక్క;
			ఆపు;
		    స్థితి ADDR_LOADED_BUFFERS:
			{
			    buf_T	*buf = firstbuf;

			    అయితే (buf->b_తదుపరి != ఖాళీ
						  && buf->b_ml.ml_mfp == ఖాళీ)
				buf = buf->b_తదుపరి;
			    eap->line1 = buf->b_fnum;
			    buf = lastbuf;
			    అయితే (buf->b_prev != ఖాళీ
						  && buf->b_ml.ml_mfp == ఖాళీ)
				buf = buf->b_prev;
			    eap->line2 = buf->b_fnum;
			    ఆపు;
			}
		    స్థితి ADDR_BUFFERS:
			eap->line1 = firstbuf->b_fnum;
			eap->line2 = lastbuf->b_fnum;
			ఆపు;
		    స్థితి ADDR_WINDOWS:
		    స్థితి ADDR_TABS:
			ఒకవేళ (IS_USER_CMDIDX(eap->cmdidx))
			{
			    eap->line1 = 1;
			    eap->line2 = eap->addr_type == ADDR_WINDOWS
						  ? చివరి_WIN_NR : చివరి_TAB_NR;
			}
			లేకపోతే
			{
			    // there is no Vim command which uses '%' and
			    // ADDR_WINDOWS or ADDR_TABS
			    *errormsg = _(e_invalid_range);
			    వెళ్ళు theend;
			}
			ఆపు;
		    స్థితి ADDR_TABS_RELATIVE:
		    స్థితి ADDR_UNSIGNED:
		    స్థితి ADDR_QUICKFIX:
			*errormsg = _(e_invalid_range);
			వెళ్ళు theend;
		    స్థితి ADDR_ARGUMENTS:
			ఒకవేళ (ARGCOUNT == 0)
			    eap->line1 = eap->line2 = 0;
			లేకపోతే
			{
			    eap->line1 = 1;
			    eap->line2 = ARGCOUNT;
			}
			ఆపు;
		    స్థితి ADDR_QUICKFIX_VALID:
#నిర్వచితమైతే FEAT_QUICKFIX
			eap->line1 = 1;
			eap->line2 = qf_get_valid_కొలత(eap);
			ఒకవేళ (eap->line2 == 0)
			    eap->line2 = 1;
#అయితే_ముగింపు
			ఆపు;
		    స్థితి ADDR_NONE:
			// Will give an error later if a range is found.
			ఆపు;
		}
		++eap->addr_లెక్క;
	    }
	    లేకపోతే ఒకవేళ (*eap->cmd == '*' && vim_strchr(p_cpo, CPO_నక్షత్రం) == ఖాళీ)
	    {
		pos_T	    *fp;

		// '*' - visual area
		ఒకవేళ (eap->addr_type != ADDR_పంక్తులు)
		{
		    *errormsg = _(e_invalid_range);
		    వెళ్ళు theend;
		}

		++eap->cmd;
		ఒకవేళ (!eap->skip)
		{
		    fp = getmark('<', FALSE);
		    ఒకవేళ (check_mark(fp) == FAIL)
			వెళ్ళు theend;
		    eap->line1 = fp->lnum;
		    fp = getmark('>', FALSE);
		    ఒకవేళ (check_mark(fp) == FAIL)
			వెళ్ళు theend;
		    eap->line2 = fp->lnum;
		    ++eap->addr_లెక్క;
		}
	    }
	}
	లేకపోతే
	    eap->line2 = lnum;
	eap->addr_లెక్క++;

	ఒకవేళ (*eap->cmd == ';')
	{
	    ఒకవేళ (!eap->skip)
	    {
		curwin->w_cursor.lnum = eap->line2;

		// Don't leave the cursor on an illegal line or column, but do
		// accept zero as address, so 0;/PATTERN/ works correctly
		// (where zero usually means to use the first line).
		// Check the cursor position before returning.
		ఒకవేళ (eap->line2 > 0)
		    check_cursor();
		లేకపోతే
		    check_cursor_నిలువు();
		need_check_cursor = TRUE;
	    }
	}
	లేకపోతే ఒకవేళ (*eap->cmd != ',')
	    ఆపు;
	++eap->cmd;
    }

    // One address given: set start and end lines.
    ఒకవేళ (eap->addr_లెక్క == 1)
    {
	eap->line1 = eap->line2;
	// ... but only implicit: really no address given
	ఒకవేళ (lnum == MAXLNUM)
	    eap->addr_లెక్క = 0;
    }
    ret = OK;

theend:
    ఒకవేళ (need_check_cursor)
	check_cursor();
    తిరిగి ret;
}

/*
 * Append "cmd" to the error message in IObuff.
 * Takes care of limiting the length and handling 0xa0, which would be
 * invisible otherwise.
 */
    స్థిర శూన్య
append_command(char_u *cmd)
{
    size_t  len = STRLEN(IObuff);
    char_u  *s = cmd;
    char_u  *d;

    ఒకవేళ (len > IOSIZE - 100)
    {
	// Not enough space, truncate and put in "...".
	d = IObuff + IOSIZE - 100;
	d -= mb_తల_off(IObuff, d);
	STRCPY(d, "...");
    }
    STRCAT(IObuff, ": ");
    d = IObuff + STRLEN(IObuff);
    అయితే (*s != NUL && d - IObuff + 5 < IOSIZE)
    {
	ఒకవేళ (enc_utf8 ? (s[0] == 0xc2 && s[1] == 0xa0) : *s == 0xa0)
	{
	    s += enc_utf8 ? 2 : 1;
	    STRCPY(d, "<a0>");
	    d += 4;
	}
	లేకపోతే ఒకవేళ (d - IObuff + (*mb_ptr2len)(s) + 1 >= IOSIZE)
	    ఆపు;
	లేకపోతే
	    MB_COPY_CHAR(s, d);
    }
    *d = NUL;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * If "start" points "&opt", "&l:opt", "&g:opt" or "$ENV" return a pointer to
 * the name.  Otherwise just return "start".
 */
    char_u *
skip_option_env_lead(char_u *ప్రారంభం)
{
    char_u *name = ప్రారంభం;

    ఒకవేళ (*ప్రారంభం == '&')
    {
	ఒకవేళ ((ప్రారంభం[1] == 'l' || ప్రారంభం[1] == 'g') && ప్రారంభం[2] == ':')
	    name += 3;
	లేకపోతే
	    name += 1;
    }
    లేకపోతే ఒకవేళ (*ప్రారంభం == '$')
	name += 1;
    తిరిగి name;
}
#అయితే_ముగింపు

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
    స్థిర పూర్ణాంకం
one_letter_cmd(char_u *p, cmdidx_T *idx)
{
    ఒకవేళ (లోపల_vim9script())
	తిరిగి FALSE;
    ఒకవేళ (p[0] == 'k'
	    && (p[1] != 'e' || (p[1] == 'e' && p[2] != 'e')))
    {
	*idx = CMD_k;
	తిరిగి TRUE;
    }
    ఒకవేళ (p[0] == 's'
	    && ((p[1] == 'c' && (p[2] == NUL || (p[2] != 's' && p[2] != 'r'
			&& (p[3] == NUL || (p[3] != 'i' && p[4] != 'p')))))
		|| p[1] == 'g'
		|| (p[1] == 'i' && p[2] != 'm' && p[2] != 'l' && p[2] != 'g')
		|| p[1] == 'I'
		|| (p[1] == 'r' && p[2] != 'e')))
    {
	*idx = CMD_substitute;
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return TRUE if "cmd" starts with "123->", a number followed by a method
 * call.
 */
    పూర్ణాంకం
సంఖ్య_పద్ధతి(char_u *cmd)
{
    char_u *p = skipdigits(cmd);

    తిరిగి p > cmd && (p = skipwhite(p))[0] == '-' && p[1] == '>';
}
#అయితే_ముగింపు

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
కనుగొను_ex_command(
	exarg_T *eap,
	పూర్ణాంకం	*నిండిన వాడని,
	పూర్ణాంకం	(*lookup)(char_u *, size_t, పూర్ణాంకం cmd, cctx_T *) వాడని,
	cctx_T	*cctx వాడని)
{
    పూర్ణాంకం		len;
    char_u	*p;
    పూర్ణాంకం		i;
#నిర్వచితం_కాకపోతే FEAT_EVAL
    పూర్ణాంకం		vim9 = FALSE;
#లేకపోతే
    పూర్ణాంకం		vim9 = లోపల_vim9script();

    /*
     * Recognize a Vim9 script function/method call and assignment:
     * "lvar = value", "lvar(arg)", "[1, 2 3]->Func()"
     */
    p = eap->cmd;
    ఒకవేళ (lookup != ఖాళీ)
    {
	char_u *pskip = skip_option_env_lead(eap->cmd);

	ఒకవేళ (vim_strchr((char_u *)"{('[\"@&$", *p) != ఖాళీ
	       || ((p = గమ్యం_name_const_చివర(pskip)) > eap->cmd && *p != NUL)
	       || (p[0] == '0' && p[1] == 'z'))
	{
	    పూర్ణాంకం	    oplen;
	    పూర్ణాంకం	    heredoc;
	    char_u  *swp;

	    ఒకవేళ (*eap->cmd == '&'
		    || (eap->cmd[0] == '$'
				  && eap->cmd[1] != '\'' && eap->cmd[1] != '"')
		    || (eap->cmd[0] == '@'
					&& (valid_yank_reg(eap->cmd[1], FALSE)
						       || eap->cmd[1] == '@')))
	    {
		ఒకవేళ (*eap->cmd == '&')
		{
		    p = eap->cmd + 1;
		    ఒకవేళ (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
			p += 2;
		    p = గమ్యం_name_చివర(p, FALSE);
		}
		లేకపోతే ఒకవేళ (*eap->cmd == '$')
		    p = గమ్యం_name_చివర(eap->cmd + 1, FALSE);
		లేకపోతే
		    p = eap->cmd + 2;
		ఒకవేళ (చివర్లు_excmd(*skipwhite(p)))
		{
		    // "&option <NL>", "$ENV <NL>" and "@r <NL>" are the start
		    // of an expression.
		    eap->cmdidx = CMD_eval;
		    తిరిగి eap->cmd;
		}
		// "&option" can be followed by "->" or "=", check below
	    }

	    ఒకవేళ (vim9 && *p == '<')
	    {
		// generic function type args
		ఒకవేళ (skip_generic_func_type_వాదనలు(&p) == FAIL)
		{
		    eap->cmdidx = CMD_కొలత;
		    తిరిగి p;
		}
	    }

	    swp = skipwhite(p);

	    ఒకవేళ (
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
		ఒకవేళ (*eap->cmd == '{' && చివర్లు_excmd(*skipwhite(eap->cmd + 1)))
		{
		    // "{" by itself is the start of a block.
		    eap->cmdidx = CMD_ఖండం;
		    తిరిగి eap->cmd + 1;
		}
		eap->cmdidx = CMD_eval;
		తిరిగి eap->cmd;
	    }

	    ఒకవేళ ((p != eap->cmd && (
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
		ఒకవేళ (skip_సమాసం(&after, ఖాళీ) == OK)
		{
		    after = skipwhite(after);
		    ఒకవేళ (*after == '=' || (*after != NUL && after[1] == '=')
					 || (after[0] == '.' && after[1] == '.'
							   && after[2] == '='))
			eap->cmdidx = CMD_var;
		}
		--emsg_off;
		తిరిగి eap->cmd;
	    }

	    // "[...]->Method()" is a list expression, but "[a, b] = Func()" is
	    // an assignment.
	    // If there is no line break inside the "[...]" then "p" is
	    // advanced to after the "]" by to_name_const_end(): check if a "="
	    // follows.
	    // If "[...]" has a line break "p" still points at the "[" and it
	    // can't be an assignment.
	    ఒకవేళ (*eap->cmd == '[')
	    {
		char_u	    *eq;

		p = గమ్యం_name_const_చివర(eap->cmd);
		ఒకవేళ (p == eap->cmd && *p == '[')
		{
		    పూర్ణాంకం లెక్క = 0;
		    పూర్ణాంకం	semicolon = FALSE;

		    p = skip_var_list(eap->cmd, TRUE, &లెక్క, &semicolon, TRUE);
		}
		eq = p;
		ఒకవేళ (eq != ఖాళీ)
		{
		    eq = skipwhite(eq);
		    ఒకవేళ (vim_strchr((char_u *)"+-*/%.", *eq) != ఖాళీ)
		    {
			ఒకవేళ (eq[0] == '.' && eq[1] == '.')
			    ++eq;
			++eq;
		    }
		}
		ఒకవేళ (p == ఖాళీ || p == eap->cmd || *eq != '=')
		{
		    eap->cmdidx = CMD_eval;
		    తిరిగి eap->cmd;
		}
		ఒకవేళ (p > eap->cmd && *eq == '=')
		{
		    eap->cmdidx = CMD_var;
		    తిరిగి eap->cmd;
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
	    ఒకవేళ (oplen > 0)
	    {
		ఒకవేళ (((p - eap->cmd) > 2 && eap->cmd[1] == ':')
			|| *eap->cmd == '&'
			|| *eap->cmd == '$'
			|| *eap->cmd == '@'
			|| (eap->skip && IS_WHITE_OR_NUL(
						      *(skipwhite(p) + oplen)))
			|| lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK)
		{
		    eap->cmdidx = CMD_var;
		    తిరిగి eap->cmd;
		}
	    }

	    // Recognize trying to use a type for a w:, b:, t: or g: variable:
	    // "w:varname: number = 123".
	    ఒకవేళ (eap->cmd[1] == ':' && *p == ':')
	    {
		eap->cmdidx = CMD_var;
		తిరిగి eap->cmd;
	    }
	}

	// 1234->func() is a method call
	ఒకవేళ (సంఖ్య_పద్ధతి(eap->cmd))
	{
	    eap->cmdidx = CMD_eval;
	    తిరిగి eap->cmd;
	}

	// "g:", "s:" and "l:" are always assumed to be a variable, thus start
	// an expression.  A global/substitute/list command needs to use a
	// longer name.
	ఒకవేళ (vim_strchr((char_u *)"gsl", *p) != ఖాళీ && p[1] == ':')
	{
	    eap->cmdidx = CMD_eval;
	    తిరిగి eap->cmd;
	}

	// If it is an ID it might be a variable with an operator on the next
	// line, if the variable exists it can't be an Ex command.
	ఒకవేళ (p > eap->cmd && చివర్లు_excmd(*skipwhite(p))
		&& (lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK
		    || (ASCII_ISALPHA(eap->cmd[0]) && eap->cmd[1] == ':')))
	{
	    eap->cmdidx = CMD_eval;
	    తిరిగి eap->cmd;
	}

	// Check for "++nr" and "--nr".
	ఒకవేళ (p == eap->cmd && p[0] != NUL && p[0] == p[1]
						   && (*p == '+' || *p == '-'))
	{
	    eap->cmdidx = *p == '+' ? CMD_increment : CMD_decrement;
	    తిరిగి eap->cmd + 2;
	}
    }
#అయితే_ముగింపు

    /*
     * Isolate the command and search for it in the command table.
     */
    p = eap->cmd;
    ఒకవేళ (one_letter_cmd(p, &eap->cmdidx))
    {
	++p;
	ఒకవేళ (నిండిన != ఖాళీ)
	    *నిండిన = TRUE;
    }
    లేకపోతే
    {
	అయితే (ASCII_ISALPHA(*p))
	    ++p;
	// for python 3.x support ":py3", ":python3", ":py3file", etc.
	ఒకవేళ (eap->cmd[0] == 'p' && eap->cmd[1] == 'y')
	{
	    అయితే (ASCII_ISALNUM(*p))
		++p;
	}
	లేకపోతే ఒకవేళ (*p == '9' && STRNCMP("vim9", eap->cmd, 4) == 0)
	{
	    // include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	    ++p;
	    అయితే (ASCII_ISALPHA(*p))
		++p;
	}

	// check for non-alpha command
	ఒకవేళ (p == eap->cmd && vim_strchr((char_u *)"@*!=><&~#}", *p) != ఖాళీ)
	    ++p;
	len = (పూర్ణాంకం)(p - eap->cmd);
	// The "d" command can directly be followed by 'l' or 'p' flag, when
	// not in Vim9 script.
	ఒకవేళ (!vim9 && *eap->cmd == 'd' && (p[-1] == 'l' || p[-1] == 'p'))
	{
	    // Check for ":dl", ":dell", etc. to ":deletel": that's
	    // :delete with the 'l' flag.  Same for 'p'.
	    కొరకు (i = 0; i < len; ++i)
		ఒకవేళ (eap->cmd[i] != ((char_u *)"delete")[i])
		    ఆపు;
	    ఒకవేళ (i == len - 1)
	    {
		--len;
		ఒకవేళ (p[-1] == 'l')
		    eap->flags |= EXFLAG_LIST;
		లేకపోతే
		    eap->flags |= EXFLAG_PRINT;
	    }
	}

	ఒకవేళ (ASCII_ISLOWER(eap->cmd[0]))
	{
	    పూర్ణాంకం c1 = eap->cmd[0];
	    పూర్ణాంకం c2 = len == 1 ? NUL : eap->cmd[1];

	    ఒకవేళ (command_లెక్క != (పూర్ణాంకం)CMD_కొలత)
	    {
		iemsg(e_command_పట్టిక_needs_గమ్యం_be_updated_నిరంతరం_make_cmdidxs);
		getout(1);
	    }

	    // Use a precomputed index for fast look-up in cmdnames[]
	    // taking into account the first 2 letters of eap->cmd.
	    eap->cmdidx = cmdidxs1[Char_Ord_దిగువ(c1)];
	    ఒకవేళ (ASCII_ISLOWER(c2))
		eap->cmdidx += cmdidxs2[Char_Ord_దిగువ(c1)][Char_Ord_దిగువ(c2)];
	}
	లేకపోతే ఒకవేళ (ASCII_ISUPPER(eap->cmd[0]))
	    eap->cmdidx = CMD_తదుపరి_3;
	లేకపోతే
	    eap->cmdidx = CMD_bang;

	కొరకు ( ; (పూర్ణాంకం)eap->cmdidx < (పూర్ణాంకం)CMD_కొలత;
			       eap->cmdidx = (cmdidx_T)((పూర్ణాంకం)eap->cmdidx + 1))
	    ఒకవేళ (STRNCMP(cmdnames[(పూర్ణాంకం)eap->cmdidx].cmd_name, (అక్షరం *)eap->cmd,
							    (size_t)len) == 0)
	    {
#నిర్వచితమైతే FEAT_EVAL
		ఒకవేళ (నిండిన != ఖాళీ && cmdnames[eap->cmdidx].cmd_name[len] == NUL)
		    *నిండిన = TRUE;
#అయితే_ముగింపు
		ఆపు;
	    }

	// :Print and :mode are not supported in Vim9 script.
	// Some commands cannot be shortened in Vim9 script.
	ఒకవేళ (vim9 && eap->cmdidx != CMD_కొలత)
	{
	    ఒకవేళ (eap->cmdidx == CMD_బహుళకం || eap->cmdidx == CMD_Print)
		eap->cmdidx = CMD_కొలత;
	    లేకపోతే ఒకవేళ ((cmdnames[eap->cmdidx].cmd_argt & EX_సంపూర్ణ)
			  && len < (పూర్ణాంకం)cmdnames[eap->cmdidx].cmd_namelen)
	    {
		semsg(_(e_command_cannot_be_shortened_str), eap->cmd);
		eap->cmdidx = CMD_కొలత;
	    }
	}

	// Do not recognize ":*" as the star command unless '*' is in
	// 'cpoptions'.
	ఒకవేళ (eap->cmdidx == CMD_నక్షత్రం && vim_strchr(p_cpo, CPO_నక్షత్రం) == ఖాళీ)
	    p = eap->cmd;

	// Look for a user defined command as a last resort.  Let ":Print" be
	// overruled by a user defined command.
	ఒకవేళ ((eap->cmdidx == CMD_కొలత || eap->cmdidx == CMD_Print)
		&& *eap->cmd >= 'A' && *eap->cmd <= 'Z')
	{
	    // User defined commands may contain digits.
	    అయితే (ASCII_ISALNUM(*p))
		++p;
	    p = కనుగొను_ucmd(eap, p, నిండిన, ఖాళీ, ఖాళీ);
	}
	ఒకవేళ (p == ఖాళీ || p == eap->cmd)
	    eap->cmdidx = CMD_కొలత;
    }

    // ":fina" means ":finally" in legacy script, for backwards compatibility.
    ఒకవేళ (eap->cmdidx == CMD_final && p - eap->cmd == 4 && !vim9)
	eap->cmdidx = CMD_finally;

     // Force ":ho" to be unresolved.  Without this, find_ex_command()
     // matches it to CMD_horizontal (the only "ho*" entry), which makes
     // fullcommand("ho") return "horizontal" even though ":ho" cannot be
     // used as the modifier (cmdmods[] requires 3 chars, "hor").
    ఒకవేళ (eap->cmdidx == CMD_horizontal && p - eap->cmd == 2)
	eap->cmdidx = CMD_కొలత;

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (eap->cmdidx < CMD_కొలత
	    && vim9
	    && !IS_WHITE_NL_OR_NUL(*p) && *p != '!' && *p != '|'
	    && (eap->cmdidx < 0 ||
		(cmdnames[eap->cmdidx].cmd_argt & EX_NONWHITE_OK) == 0))
    {
	// A name that goes on with an underscore is not this command with an
	// argument: "ch_log" is not ":change".  Leave it to be reported as an
	// invalid command.
	ఒకవేళ (*p != '_')
	{
	    char_u *cmd = vim_strnsave(eap->cmd, p - eap->cmd);

	    semsg(_(e_command_str_not_followed_by_white_space_str), cmd,
								    eap->cmd);
	    vim_free(cmd);
	}
	eap->cmdidx = CMD_కొలత;
    }
#అయితే_ముగింపు

    తిరిగి p;
}

#ఒకవేళ defined(FEAT_EVAL)
రకనిర్వచనం నిర్మాణం
{
    అక్షరం	*name;
    పూర్ణాంకం		minlen;
    పూర్ణాంకం		has_లెక్క;  // :123verbose  :3tab
} cmdmod_info_T;

స్థిర cmdmod_info_T cmdmod_info_tab[] = {
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
    పూర్ణాంకం
modifier_len(char_u *cmd)
{
    పూర్ణాంకం		i, j;
    char_u	*p = cmd;

    ఒకవేళ (VIM_ISDIGIT(*cmd))
	p = skipwhite(skipdigits(cmd + 1));
    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(cmdmod_info_tab); ++i)
    {
	// cmdmod_info_tab[] is sorted by name: once the first letter is past
	// the command's first letter no later entry can match.
	ఒకవేళ (cmdmod_info_tab[i].name[0] > *p)
	    ఆపు;
	కొరకు (j = 0; p[j] != NUL; ++j)
	    ఒకవేళ (p[j] != cmdmod_info_tab[i].name[j])
		ఆపు;
	ఒకవేళ (!ASCII_ISALPHA(p[j]) && j >= cmdmod_info_tab[i].minlen
					&& (p == cmd || cmdmod_info_tab[i].has_లెక్క))
	    తిరిగి j + (పూర్ణాంకం)(p - cmd);
    }
    తిరిగి 0;
}

/*
 * Return > 0 if an Ex command "name" exists.
 * Return 2 if there is an exact match.
 * Return 3 if there is an ambiguous match.
 */
    పూర్ణాంకం
cmd_exists(char_u *name)
{
    exarg_T	ea;
    పూర్ణాంకం		నిండిన = FALSE;
    పూర్ణాంకం		i;
    పూర్ణాంకం		j;
    char_u	*p;

    // Check command modifiers.
    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(cmdmod_info_tab); ++i)
    {
	కొరకు (j = 0; name[j] != NUL; ++j)
	    ఒకవేళ (name[j] != cmdmod_info_tab[i].name[j])
		ఆపు;
	ఒకవేళ (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	    తిరిగి (cmdmod_info_tab[i].name[j] == NUL ? 2 : 1);
    }

    // Check built-in commands and user defined commands.
    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    p = కనుగొను_ex_command(&ea, &నిండిన, ఖాళీ, ఖాళీ);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి 3;
    ఒకవేళ (vim_isdigit(*name) && ea.cmdidx != CMD_సరిపోలిక)
	తిరిగి 0;
    ఒకవేళ (*skipwhite(p) != NUL)
	తిరిగి 0;	// trailing garbage
    తిరిగి (ea.cmdidx == CMD_కొలత ? 0 : (నిండిన ? 2 : 1));
}

/*
 * Add the attributes of an Ex command to "d" in the terms of ":command":
 * "argt" are the EX_ flags, "addr_type" the address type and "def" the
 * default for a range or count, -1 when there is none.
 */
    శూన్య
ex_command_attrs(dict_T *d, long_u argt, cmd_addr_T addr_type, దీర్ఘ def)
{
    స్థిర అక్షరం *addr_పేర్లు[] = {
	"lines", "windows", "arguments", "loaded_buffers", "buffers", "tabs",
	"tabs_relative", "quickfix_valid", "quickfix", "unsigned", "other",
	"none"
    };
    అక్షరం	*nargs;

    ఒకవేళ (!(argt & EX_EXTRA))
	nargs = "0";
    లేకపోతే ఒకవేళ (!(argt & EX_NOSPC))
	nargs = (argt & EX_NEEDARG) ? "+" : "*";
    లేకపోతే ఒకవేళ (!(argt & EX_NEEDARG))
	nargs = "?";
    లేకపోతే
	nargs = (argt & EX_ARGSPACE) ? "_" : "1";
    dict_కూడు_string(d, "nargs", (char_u *)nargs);

    ఒకవేళ (!(argt & EX_RANGE))
	dict_కూడు_bool(d, "range", FALSE);
    లేకపోతే ఒకవేళ (argt & EX_DFLALL)
	dict_కూడు_string(d, "range", (char_u *)"%");
    లేకపోతే ఒకవేళ (def >= 0 && !(argt & EX_లెక్క))
	dict_కూడు_సంఖ్య(d, "range", def);
    లేకపోతే
	dict_కూడు_string(d, "range", (char_u *)".");
    ఒకవేళ (!(argt & EX_లెక్క))
	dict_కూడు_bool(d, "count", FALSE);
    లేకపోతే
	dict_కూడు_సంఖ్య(d, "count", def < 0 ? 0 : def);
    dict_కూడు_bool(d, "bang", (argt & EX_BANG) != 0);
    dict_కూడు_bool(d, "bar", (argt & EX_TRLBAR) != 0);
    dict_కూడు_bool(d, "register", (argt & EX_REGSTR) != 0);
    dict_కూడు_string(d, "addr", (char_u *)addr_పేర్లు[addr_type]);
}

/*
 * Add what getinfo() reports for the Ex command "name" to "d".  "vim9" is TRUE
 * to find the command as in Vim9 script, FALSE as in legacy script, -1 for the
 * context of the caller.
 * Returns FAIL when there is no such command or the abbreviation is
 * ambiguous.
 */
    పూర్ణాంకం
ex_command_info(char_u *name, పూర్ణాంకం vim9, dict_T *d)
{
    exarg_T	ea;
    char_u	*p;
    పూర్ణాంకం		i;
    పూర్ణాంకం		j;
    పూర్ణాంకం		save_cmod_flags = cmdmod.cmod_flags;
    పూర్ణాంకం		ret = OK;

    ఒకవేళ (vim9 != -1)
    {
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9 ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    // An abbreviation of a command modifier, like ":bo" for ":botright".
    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(cmdmod_info_tab); ++i)
    {
	కొరకు (j = 0; name[j] != NUL; ++j)
	    ఒకవేళ (name[j] != cmdmod_info_tab[i].name[j])
		ఆపు;
	ఒకవేళ (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	{
	    name = (char_u *)cmdmod_info_tab[i].name;
	    ఆపు;
	}
    }

    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    ea.addr_లెక్క = 0;
    // Don't complain about using "en" or ":let" in Vim9 script.
    ++emsg_silent;
    p = కనుగొను_ex_command(&ea, ఖాళీ, ఖాళీ, ఖాళీ);
    ఒకవేళ (p == ఖాళీ || ea.cmdidx == CMD_కొలత
	    || (vim_isdigit(*name) && ea.cmdidx != CMD_సరిపోలిక)
	    || *skipwhite(p) != NUL
	    || not_లోపల_vim9(&ea, true) == FAIL)
	ret = FAIL;
    --emsg_silent;

    ఒకవేళ (ret == FAIL)
	;
    లేకపోతే ఒకవేళ (IS_USER_CMDIDX(ea.cmdidx))
	ret = user_command_info(ea.useridx, ea.cmdidx, d);
    లేకపోతే
    {
	పూర్ణాంకం	notimpl = cmdnames[ea.cmdidx].cmd_func == ex_ni
# ifdef HAVE_EX_SCRIPT_NI
			    || cmdnames[ea.cmdidx].cmd_func == ex_script_ni
# endif
			    ;

	dict_కూడు_string(d, "name", cmdnames[ea.cmdidx].cmd_name);
	dict_కూడు_string(d, "kind", (char_u *)(
		    cmdnames[ea.cmdidx].cmd_func == ex_wrongmodifier
						    ? "modifier" : "builtin"));
	dict_కూడు_bool(d, "available", !notimpl);
	ex_command_attrs(d, cmdnames[ea.cmdidx].cmd_argt,
				       cmdnames[ea.cmdidx].cmd_addr_type, -1);
    }
    cmdmod.cmod_flags = save_cmod_flags;
    తిరిగి ret;
}

/*
 * "fullcommand" function
 */
    శూన్య
f_fullcommand(typval_T *argvars, typval_T *rettv)
{
    exarg_T	ea;
    char_u	*name;
    char_u	*p;
    పూర్ణాంకం		vim9script = లోపల_vim9script();
    పూర్ణాంకం		save_cmod_flags = cmdmod.cmod_flags;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	తిరిగి;

    name = tv_get_string(&argvars[0]);
    ఒకవేళ (name == ఖాళీ)
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	vim9script = tv_get_bool(&argvars[1]);
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9script ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    అయితే (*name == ':')
	name++;
    name = skip_range(name, TRUE, ఖాళీ);

    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.addr_లెక్క = 0;
    ++emsg_silent;  // don't complain about using "en" in Vim9 script
    p = కనుగొను_ex_command(&ea, ఖాళీ, ఖాళీ, ఖాళీ);
    --emsg_silent;
    ఒకవేళ (p == ఖాళీ || ea.cmdidx == CMD_కొలత)
	వెళ్ళు theend;

    ఒకవేళ (vim9script && not_లోపల_vim9(&ea, true) == FAIL)
	వెళ్ళు theend;

    rettv->vval.v_string = vim_strsave(IS_USER_CMDIDX(ea.cmdidx)
				 ? get_user_command_name(ea.useridx, ea.cmdidx)
				 : cmdnames[ea.cmdidx].cmd_name);
theend:
    cmdmod.cmod_flags = save_cmod_flags;
}
#అయితే_ముగింపు

    cmdidx_T
excmd_get_cmdidx(char_u *cmd, పూర్ణాంకం len)
{
    cmdidx_T idx;

    ఒకవేళ (!one_letter_cmd(cmd, &idx))
	కొరకు (idx = (cmdidx_T)0; (పూర్ణాంకం)idx < (పూర్ణాంకం)CMD_కొలత;
		idx = (cmdidx_T)((పూర్ణాంకం)idx + 1))
	    ఒకవేళ (STRNCMP(cmdnames[(పూర్ణాంకం)idx].cmd_name, cmd, (size_t)len) == 0)
		ఆపు;

    తిరిగి idx;
}

    దీర్ఘ
excmd_get_argt(cmdidx_T idx)
{
    తిరిగి (దీర్ఘ)cmdnames[(పూర్ణాంకం)idx].cmd_argt;
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
    char_u	*cmd_ప్రారంభం,
    పూర్ణాంకం		skip_నక్షత్రం,	// skip "*" used for Visual range
    పూర్ణాంకం		*ctx)		// pointer to xp_context or NULL
{
    char_u	*cmd = cmd_ప్రారంభం;
    గుర్తులేని	delim;

    అయితే (vim_strchr((char_u *)" \t0123456789.$%'/?-+,;\\", *cmd) != ఖాళీ)
    {
	ఒకవేళ (*cmd == '\\')
	{
	    ఒకవేళ (cmd[1] == '?' || cmd[1] == '/' || cmd[1] == '&')
		++cmd;
	    లేకపోతే
		ఆపు;
	}
	లేకపోతే ఒకవేళ (*cmd == '\'')
	{
	    char_u *p = cmd;

	    // a quote is only valid at the start or after a separator
	    అయితే (p > cmd_ప్రారంభం)
	    {
		--p;
		ఒకవేళ (!VIM_ISWHITE(*p))
		    ఆపు;
	    }
	    ఒకవేళ (cmd > cmd_ప్రారంభం && !VIM_ISWHITE(*p) && *p != ',' && *p != ';')
		ఆపు;
	    ఒకవేళ (*++cmd == NUL && ctx != ఖాళీ)
		*ctx = EXPAND_NOTHING;
	}
	లేకపోతే ఒకవేళ (*cmd == '/' || *cmd == '?')
	{
	    delim = *cmd++;
	    అయితే (*cmd != NUL && *cmd != delim)
		ఒకవేళ (*cmd++ == '\\' && *cmd != NUL)
		    ++cmd;
	    ఒకవేళ (*cmd == NUL && ctx != ఖాళీ)
		*ctx = EXPAND_NOTHING;
	}
	ఒకవేళ (*cmd != NUL)
	    ++cmd;
    }

    // Skip ":" and white space.
    అయితే (*cmd == ':')
	cmd = skipwhite(cmd + 1);

    // Skip "*" used for Visual range.
    ఒకవేళ (skip_నక్షత్రం && *cmd == '*' && vim_strchr(p_cpo, CPO_నక్షత్రం) == ఖాళీ)
	cmd = skipwhite(cmd + 1);

    తిరిగి cmd;
}

    స్థిర శూన్య
addr_error(cmd_addr_T addr_type)
{
    ఒకవేళ (addr_type == ADDR_NONE)
	emsg(_(e_no_range_allowed));
    లేకపోతే
	emsg(_(e_invalid_range));
}

/*
 * Return the default address for an address type.
 */
    స్థిర linenr_T
default_చిరునామా(exarg_T *eap)
{
    linenr_T lnum = 0;

    ఎంపిక (eap->addr_type)
    {
	స్థితి ADDR_పంక్తులు:
	స్థితి ADDR_OTHER:
	    // Default is the cursor line number.  Avoid using an invalid
	    // line number though.
	    ఒకవేళ (curwin->w_cursor.lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
		lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
	    లేకపోతే
		lnum = curwin->w_cursor.lnum;
	    ఆపు;
	స్థితి ADDR_WINDOWS:
	    lnum = CURRENT_WIN_NR;
	    ఆపు;
	స్థితి ADDR_ARGUMENTS:
	    lnum = curwin->w_arg_idx + 1;
	    ఒకవేళ (lnum > ARGCOUNT)
		lnum = ARGCOUNT;
	    ఆపు;
	స్థితి ADDR_LOADED_BUFFERS:
	స్థితి ADDR_BUFFERS:
	    lnum = curbuf->b_fnum;
	    ఆపు;
	స్థితి ADDR_TABS:
	    lnum = CURRENT_TAB_NR;
	    ఆపు;
	స్థితి ADDR_TABS_RELATIVE:
	స్థితి ADDR_UNSIGNED:
	    lnum = 1;
	    ఆపు;
	స్థితి ADDR_QUICKFIX:
#నిర్వచితమైతే FEAT_QUICKFIX
	    lnum = qf_get_cur_idx(eap);
#అయితే_ముగింపు
	    ఆపు;
	స్థితి ADDR_QUICKFIX_VALID:
#నిర్వచితమైతే FEAT_QUICKFIX
	    lnum = qf_get_cur_valid_idx(eap);
#అయితే_ముగింపు
	    ఆపు;
	స్థితి ADDR_NONE:
	    // Will give an error later if a range is found.
	    ఆపు;
    }
    తిరిగి lnum;
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
get_చిరునామా(
    exarg_T	*eap వాడని,
    char_u	**ptr,
    cmd_addr_T	addr_type,
    పూర్ణాంకం		skip,		// only skip the address, don't use it
    పూర్ణాంకం		silent,		// no errors or side effects
    పూర్ణాంకం		గమ్యం_other_file,  // flag: may jump to other file
    పూర్ణాంకం		చిరునామా_లెక్క వాడని) // 1 for first address, >1 after comma
{
    పూర్ణాంకం		c;
    పూర్ణాంకం		i;
    దీర్ఘ	n;
    char_u	*cmd;
    pos_T	pos;
    pos_T	*fp;
    linenr_T	lnum;
    buf_T	*buf;

    cmd = skipwhite(*ptr);
    lnum = MAXLNUM;
    చేయి
    {
	ఎంపిక (*cmd)
	{
	    స్థితి '.':			    // '.' - Cursor position
		++cmd;
		ఎంపిక (addr_type)
		{
		    స్థితి ADDR_పంక్తులు:
		    స్థితి ADDR_OTHER:
			lnum = curwin->w_cursor.lnum;
			ఆపు;
		    స్థితి ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			ఆపు;
		    స్థితి ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			ఆపు;
		    స్థితి ADDR_LOADED_BUFFERS:
		    స్థితి ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			ఆపు;
		    స్థితి ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			ఆపు;
		    స్థితి ADDR_NONE:
		    స్థితి ADDR_TABS_RELATIVE:
		    స్థితి ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = ఖాళీ;
			వెళ్ళు error;
			ఆపు;
		    స్థితి ADDR_QUICKFIX:
#నిర్వచితమైతే FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#అయితే_ముగింపు
			ఆపు;
		    స్థితి ADDR_QUICKFIX_VALID:
#నిర్వచితమైతే FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#అయితే_ముగింపు
			ఆపు;
		}
		ఆపు;

	    స్థితి '$':			    // '$' - last line
		++cmd;
		ఎంపిక (addr_type)
		{
		    స్థితి ADDR_పంక్తులు:
		    స్థితి ADDR_OTHER:
			lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
			ఆపు;
		    స్థితి ADDR_WINDOWS:
			lnum = చివరి_WIN_NR;
			ఆపు;
		    స్థితి ADDR_ARGUMENTS:
			lnum = ARGCOUNT;
			ఆపు;
		    స్థితి ADDR_LOADED_BUFFERS:
			buf = lastbuf;
			అయితే (buf->b_ml.ml_mfp == ఖాళీ)
			{
			    ఒకవేళ (buf->b_prev == ఖాళీ)
				ఆపు;
			    buf = buf->b_prev;
			}
			lnum = buf->b_fnum;
			ఆపు;
		    స్థితి ADDR_BUFFERS:
			lnum = lastbuf->b_fnum;
			ఆపు;
		    స్థితి ADDR_TABS:
			lnum = చివరి_TAB_NR;
			ఆపు;
		    స్థితి ADDR_NONE:
		    స్థితి ADDR_TABS_RELATIVE:
		    స్థితి ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = ఖాళీ;
			వెళ్ళు error;
			ఆపు;
		    స్థితి ADDR_QUICKFIX:
#నిర్వచితమైతే FEAT_QUICKFIX
			lnum = qf_get_కొలత(eap);
			ఒకవేళ (lnum == 0)
			    lnum = 1;
#అయితే_ముగింపు
			ఆపు;
		    స్థితి ADDR_QUICKFIX_VALID:
#నిర్వచితమైతే FEAT_QUICKFIX
			lnum = qf_get_valid_కొలత(eap);
			ఒకవేళ (lnum == 0)
			    lnum = 1;
#అయితే_ముగింపు
			ఆపు;
		}
		ఆపు;

	    స్థితి '\'':			    // ''' - mark
		ఒకవేళ (*++cmd == NUL)
		{
		    cmd = ఖాళీ;
		    వెళ్ళు error;
		}
		ఒకవేళ (addr_type != ADDR_పంక్తులు)
		{
		    addr_error(addr_type);
		    cmd = ఖాళీ;
		    వెళ్ళు error;
		}
		ఒకవేళ (skip)
		    ++cmd;
		లేకపోతే
		{
		    // Only accept a mark in another file when it is
		    // used by itself: ":'M".
		    fp = getmark(*cmd, గమ్యం_other_file && cmd[1] == NUL);
		    ++cmd;
		    ఒకవేళ (fp == (pos_T *)-1)
			// Jumped to another file.
			lnum = curwin->w_cursor.lnum;
		    లేకపోతే
		    {
			ఒకవేళ (check_mark(fp) == FAIL)
			{
			    cmd = ఖాళీ;
			    వెళ్ళు error;
			}
			lnum = fp->lnum;
		    }
		}
		ఆపు;

	    స్థితి '/':
	    స్థితి '?':			// '/' or '?' - search
		c = *cmd++;
		ఒకవేళ (addr_type != ADDR_పంక్తులు)
		{
		    addr_error(addr_type);
		    cmd = ఖాళీ;
		    వెళ్ళు error;
		}
		ఒకవేళ (skip)	// skip "/pat/"
		{
		    cmd = skip_regexp(cmd, c, magic_isset());
		    ఒకవేళ (*cmd == c)
			++cmd;
		}
		లేకపోతే
		{
		    పూర్ణాంకం flags;

		    pos = curwin->w_cursor; // save curwin->w_cursor

		    // When '/' or '?' follows another address, start from
		    // there.
		    ఒకవేళ (lnum > 0 && lnum != MAXLNUM)
			curwin->w_cursor.lnum =
				lnum > curbuf->b_ml.ml_పంక్తి_లెక్క
					   ? curbuf->b_ml.ml_పంక్తి_లెక్క : lnum;

		    // Start a forward search at the end of the line (unless
		    // before the first line).
		    // Start a backward search at the start of the line.
		    // This makes sure we never match in the current
		    // line, and can match anywhere in the
		    // next/previous line.
		    ఒకవేళ (c == '/' && curwin->w_cursor.lnum > 0)
			curwin->w_cursor.నిలువు = MAXCOL;
		    లేకపోతే
			curwin->w_cursor.నిలువు = 0;
		    searchcmdlen = 0;
		    flags = silent ? శోధన_KEEP : శోధన_HIS | శోధన_సందేశం;
		    ఒకవేళ (!do_శోధన(ఖాళీ, c, c, cmd, STRLEN(cmd), 1L, flags, ఖాళీ))
		    {
			curwin->w_cursor = pos;
			cmd = ఖాళీ;
			వెళ్ళు error;
		    }
		    lnum = curwin->w_cursor.lnum;
		    curwin->w_cursor = pos;
		    // adjust command string pointer
		    cmd += searchcmdlen;
		}
		ఆపు;

	    స్థితి '\\':		    // "\?", "\/" or "\&", repeat search
		++cmd;
		ఒకవేళ (addr_type != ADDR_పంక్తులు)
		{
		    addr_error(addr_type);
		    cmd = ఖాళీ;
		    వెళ్ళు error;
		}
		ఒకవేళ (*cmd == '&')
		    i = RE_SUBST;
		లేకపోతే ఒకవేళ (*cmd == '?' || *cmd == '/')
		    i = RE_శోధన;
		లేకపోతే
		{
		    emsg(_(e_backslash_should_be_followed_by));
		    cmd = ఖాళీ;
		    వెళ్ళు error;
		}

		ఒకవేళ (!skip)
		{
		    /*
		     * When search follows another address, start from
		     * there.
		     */
		    ఒకవేళ (lnum != MAXLNUM)
			pos.lnum = lnum;
		    లేకపోతే
			pos.lnum = curwin->w_cursor.lnum;

		    /*
		     * Start the search just like for the above
		     * do_search().
		     */
		    ఒకవేళ (*cmd != '?')
			pos.నిలువు = MAXCOL;
		    లేకపోతే
			pos.నిలువు = 0;
		    pos.coladd = 0;
		    ఒకవేళ (searchit(curwin, curbuf, &pos, ఖాళీ,
				*cmd == '?' ? BACKWARD : FORWARD,
				(char_u *)"", 0, 1L, శోధన_సందేశం, i, ఖాళీ) != FAIL)
			lnum = pos.lnum;
		    లేకపోతే
		    {
			cmd = ఖాళీ;
			వెళ్ళు error;
		    }
		}
		++cmd;
		ఆపు;

	    అప్రమేయ:
		ఒకవేళ (VIM_ISDIGIT(*cmd))	// absolute line number
		    lnum = getdigits(&cmd);
	}

	కొరకు (;;)
	{
	    cmd = skipwhite(cmd);
	    ఒకవేళ (*cmd != '-' && *cmd != '+' && !VIM_ISDIGIT(*cmd))
		ఆపు;

	    ఒకవేళ (lnum == MAXLNUM)
	    {
		ఎంపిక (addr_type)
		{
		    స్థితి ADDR_పంక్తులు:
		    స్థితి ADDR_OTHER:
			// "+1" is same as ".+1"
			lnum = curwin->w_cursor.lnum;
			ఆపు;
		    స్థితి ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			ఆపు;
		    స్థితి ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			ఆపు;
		    స్థితి ADDR_LOADED_BUFFERS:
		    స్థితి ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			ఆపు;
		    స్థితి ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			ఆపు;
		    స్థితి ADDR_TABS_RELATIVE:
			lnum = 1;
			ఆపు;
		    స్థితి ADDR_QUICKFIX:
#నిర్వచితమైతే FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#అయితే_ముగింపు
			ఆపు;
		    స్థితి ADDR_QUICKFIX_VALID:
#నిర్వచితమైతే FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#అయితే_ముగింపు
			ఆపు;
		    స్థితి ADDR_NONE:
		    స్థితి ADDR_UNSIGNED:
			lnum = 0;
			ఆపు;
		}
	    }

	    ఒకవేళ (VIM_ISDIGIT(*cmd))
		i = '+';		// "number" is same as "+number"
	    లేకపోతే
		i = *cmd++;
	    ఒకవేళ (!VIM_ISDIGIT(*cmd))	// '+' is '+1'
		n = 1;
	    లేకపోతే
	    {
		// "number", "+number" or "-number"
		n = getdigits(&cmd);
		ఒకవేళ (n == MAXLNUM)
		{
		    emsg(_(e_పంక్తి_సంఖ్య_బహిర్గతం_యొక్క_range));
		    cmd = ఖాళీ;
		    వెళ్ళు error;
		}
	    }

	    ఒకవేళ (addr_type == ADDR_TABS_RELATIVE)
	    {
		emsg(_(e_invalid_range));
		cmd = ఖాళీ;
		వెళ్ళు error;
	    }
	    లేకపోతే ఒకవేళ (addr_type == ADDR_LOADED_BUFFERS
		    || addr_type == ADDR_BUFFERS)
		lnum = compute_బఫరు_local_లెక్క(
				    addr_type, lnum, (i == '-') ? -1 * n : n);
	    లేకపోతే
	    {
#నిర్వచితమైతే FEAT_FOLDING
		// Relative line addressing: need to adjust for lines in a
		// closed fold after the first address.
		ఒకవేళ (addr_type == ADDR_పంక్తులు && (i == '-' || i == '+')
							 && చిరునామా_లెక్క >= 2)
		    (శూన్య)hasFolding(lnum, ఖాళీ, &lnum);
#అయితే_ముగింపు
		ఒకవేళ (i == '-')
		    lnum -= n;
		లేకపోతే
		{
		    ఒకవేళ (lnum >= 0 && n >= LONG_MAX - lnum)
		    {
			emsg(_(e_పంక్తి_సంఖ్య_బహిర్గతం_యొక్క_range));
			cmd = ఖాళీ;
			వెళ్ళు error;
		    }
		    lnum += n;
		}
	    }
	}
    } అయితే (*cmd == '/' || *cmd == '?');

error:
    *ptr = cmd;
    తిరిగి lnum;
}

/*
 * Set eap->line1 and eap->line2 to the whole range.
 * Used for commands with the EX_DFLALL flag and no range given.
 */
    స్థిర శూన్య
చిరునామా_default_all(exarg_T *eap)
{
    eap->line1 = 1;
    ఎంపిక (eap->addr_type)
    {
	స్థితి ADDR_పంక్తులు:
	స్థితి ADDR_OTHER:
	    eap->line2 = curbuf->b_ml.ml_పంక్తి_లెక్క;
	    ఆపు;
	స్థితి ADDR_LOADED_BUFFERS:
	    {
		buf_T *buf = firstbuf;

		అయితే (buf->b_తదుపరి != ఖాళీ && buf->b_ml.ml_mfp == ఖాళీ)
		    buf = buf->b_తదుపరి;
		eap->line1 = buf->b_fnum;
		buf = lastbuf;
		అయితే (buf->b_prev != ఖాళీ && buf->b_ml.ml_mfp == ఖాళీ)
		    buf = buf->b_prev;
		eap->line2 = buf->b_fnum;
	    }
	    ఆపు;
	స్థితి ADDR_BUFFERS:
	    eap->line1 = firstbuf->b_fnum;
	    eap->line2 = lastbuf->b_fnum;
	    ఆపు;
	స్థితి ADDR_WINDOWS:
	    eap->line2 = చివరి_WIN_NR;
	    ఆపు;
	స్థితి ADDR_TABS:
	    eap->line2 = చివరి_TAB_NR;
	    ఆపు;
	స్థితి ADDR_TABS_RELATIVE:
	    eap->line2 = 1;
	    ఆపు;
	స్థితి ADDR_ARGUMENTS:
	    ఒకవేళ (ARGCOUNT == 0)
		eap->line1 = eap->line2 = 0;
	    లేకపోతే
		eap->line2 = ARGCOUNT;
	    ఆపు;
	స్థితి ADDR_QUICKFIX_VALID:
#నిర్వచితమైతే FEAT_QUICKFIX
	    eap->line2 = qf_get_valid_కొలత(eap);
	    ఒకవేళ (eap->line2 == 0)
		eap->line2 = 1;
#అయితే_ముగింపు
	    ఆపు;
	స్థితి ADDR_NONE:
	స్థితి ADDR_UNSIGNED:
	స్థితి ADDR_QUICKFIX:
	    iemsg("Cannot use EX_DFLALL with ADDR_NONE, ADDR_UNSIGNED or ADDR_QUICKFIX");
	    ఆపు;
    }
}


/*
 * Get flags from an Ex command argument.
 */
    స్థిర శూన్య
get_flags(exarg_T *eap)
{
    అయితే (vim_strchr((char_u *)"lp#", *eap->arg) != ఖాళీ)
    {
	ఒకవేళ (*eap->arg == 'l')
	    eap->flags |= EXFLAG_LIST;
	లేకపోతే ఒకవేళ (*eap->arg == 'p')
	    eap->flags |= EXFLAG_PRINT;
	లేకపోతే
	    eap->flags |= EXFLAG_NR;
	eap->arg = skipwhite(eap->arg + 1);
    }
}

/*
 * Function called for command which is Not Implemented.  NI!
 */
    శూన్య
ex_ni(exarg_T *eap)
{
    ఒకవేళ (!eap->skip)
	eap->errmsg =
		_(e_sorry_command_is_not_అందుబాటు_లోపల_this_సంస్కరణ);
}

#నిర్వచితమైతే HAVE_EX_SCRIPT_NI
/*
 * Function called for script command which is Not Implemented.  NI!
 * Skips over ":perl <<EOF" constructs.
 */
    స్థిర శూన్య
ex_script_ni(exarg_T *eap)
{
    ఒకవేళ (!eap->skip)
	ex_ni(eap);
    లేకపోతే
	vim_free(script_get(eap, eap->arg));
}
#అయితే_ముగింపు

/*
 * Check range in Ex command for validity.
 * Return NULL when valid, error message when invalid.
 */
    స్థిర అక్షరం *
invalid_range(exarg_T *eap)
{
    buf_T	*buf;

    ఒకవేళ (       eap->line1 < 0
	    || eap->line2 < 0
	    || eap->line1 > eap->line2)
	తిరిగి _(e_invalid_range);

    ఒకవేళ (eap->argt & EX_RANGE)
    {
	ఎంపిక (eap->addr_type)
	{
	    స్థితి ADDR_పంక్తులు:
		ఒకవేళ (eap->line2 > curbuf->b_ml.ml_పంక్తి_లెక్క
#నిర్వచితమైతే FEAT_DIFF
			    + (eap->cmdidx == CMD_diffget ||
				eap->cmdidx == CMD_diffput)
#అయితే_ముగింపు
		   )
		    తిరిగి _(e_invalid_range);
		ఆపు;
	    స్థితి ADDR_ARGUMENTS:
		// add 1 if ARGCOUNT is 0
		ఒకవేళ (eap->line2 > ARGCOUNT + (!ARGCOUNT))
		    తిరిగి _(e_invalid_range);
		ఆపు;
	    స్థితి ADDR_BUFFERS:
		// Only a boundary check, not whether the buffers actually
		// exist.
		ఒకవేళ (eap->line1 < 1 || eap->line2 > get_highest_fnum())
		    తిరిగి _(e_invalid_range);
		ఆపు;
	    స్థితి ADDR_LOADED_BUFFERS:
		buf = firstbuf;
		అయితే (buf->b_ml.ml_mfp == ఖాళీ)
		{
		    ఒకవేళ (buf->b_తదుపరి == ఖాళీ)
			తిరిగి _(e_invalid_range);
		    buf = buf->b_తదుపరి;
		}
		ఒకవేళ (eap->line1 < buf->b_fnum)
		    తిరిగి _(e_invalid_range);
		buf = lastbuf;
		అయితే (buf->b_ml.ml_mfp == ఖాళీ)
		{
		    ఒకవేళ (buf->b_prev == ఖాళీ)
			తిరిగి _(e_invalid_range);
		    buf = buf->b_prev;
		}
		ఒకవేళ (eap->line2 > buf->b_fnum)
		    తిరిగి _(e_invalid_range);
		ఆపు;
	    స్థితి ADDR_WINDOWS:
		ఒకవేళ (eap->line2 > చివరి_WIN_NR)
		    తిరిగి _(e_invalid_range);
		ఆపు;
	    స్థితి ADDR_TABS:
		ఒకవేళ (eap->line2 > చివరి_TAB_NR)
		    తిరిగి _(e_invalid_range);
		ఆపు;
	    స్థితి ADDR_TABS_RELATIVE:
	    స్థితి ADDR_OTHER:
		// Any range is OK.
		ఆపు;
	    స్థితి ADDR_QUICKFIX:
#నిర్వచితమైతే FEAT_QUICKFIX
		// No error for value that is too big, will use the last entry.
		ఒకవేళ (eap->line2 <= 0)
		{
		    ఒకవేళ (eap->addr_లెక్క == 0)
			తిరిగి _(e_no_errors);
		    తిరిగి _(e_invalid_range);
		}
#అయితే_ముగింపు
		ఆపు;
	    స్థితి ADDR_QUICKFIX_VALID:
#నిర్వచితమైతే FEAT_QUICKFIX
		ఒకవేళ ((eap->line2 != 1 && eap->line2 > qf_get_valid_కొలత(eap))
			|| eap->line2 < 0)
		    తిరిగి _(e_invalid_range);
#అయితే_ముగింపు
		ఆపు;
	    స్థితి ADDR_UNSIGNED:
	    స్థితి ADDR_NONE:
		// Will give an error elsewhere.
		ఆపు;
	}
    }
    తిరిగి ఖాళీ;
}

/*
 * Correct the range for zero line number, if required.
 */
    స్థిర శూన్య
correct_range(exarg_T *eap)
{
    ఒకవేళ (!(eap->argt & EX_ZEROR))	    // zero in range not allowed
    {
	ఒకవేళ (eap->line1 == 0)
	    eap->line1 = 1;
	ఒకవేళ (eap->line2 == 0)
	    eap->line2 = 1;
    }
}

#నిర్వచితమైతే FEAT_QUICKFIX
/*
 * For a ":vimgrep" or ":vimgrepadd" command return a pointer past the
 * pattern.  Otherwise return eap->arg.
 */
    స్థిర char_u *
skip_grep_pat(exarg_T *eap)
{
    char_u	*p = eap->arg;

    ఒకవేళ (*p != NUL && (eap->cmdidx == CMD_vimgrep || eap->cmdidx == CMD_lvimgrep
		|| eap->cmdidx == CMD_vimgrepadd
		|| eap->cmdidx == CMD_lvimgrepadd
		|| grep_అంతర్గత(eap->cmdidx)))
    {
	p = skip_vimgrep_pat(p, ఖాళీ, ఖాళీ);
	ఒకవేళ (p == ఖాళీ)
	    p = eap->arg;
    }
    తిరిగి p;
}

/*
 * For the ":make" and ":grep" commands insert the 'makeprg'/'grepprg' option
 * in the command line, so that things like % get expanded.
 */
    స్థిర char_u *
replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep)
{
    char_u	*new_cmdline;
    char_u	*కార్యక్రమం;
    char_u	*pos;
    char_u	*ptr;
    పూర్ణాంకం		len;
    పూర్ణాంకం		i;

    /*
     * Don't do it when ":vimgrep" is used for ":grep".
     */
    ఒకవేళ ((eap->cmdidx == CMD_make || eap->cmdidx == CMD_lmake
		     || eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
		     || eap->cmdidx == CMD_grepadd
		     || eap->cmdidx == CMD_lgrepadd)
	    && !grep_అంతర్గత(eap->cmdidx))
    {
	ఒకవేళ (eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
	    || eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	{
	    ఒకవేళ (*curbuf->b_p_gp == NUL)
		కార్యక్రమం = p_gp;
	    లేకపోతే
		కార్యక్రమం = curbuf->b_p_gp;
	}
	లేకపోతే
	{
	    ఒకవేళ (*curbuf->b_p_mp == NUL)
		కార్యక్రమం = p_mp;
	    లేకపోతే
		కార్యక్రమం = curbuf->b_p_mp;
	}

	p = skipwhite(p);

	ఒకవేళ ((pos = (char_u *)అక్షరశ్రేణి_శోధన((అక్షరం *)కార్యక్రమం, "$*")) != ఖాళీ)
	{
	    // replace $* by given arguments
	    i = 1;
	    అయితే ((pos = (char_u *)అక్షరశ్రేణి_శోధన((అక్షరం *)pos + 2, "$*")) != ఖాళీ)
		++i;
	    len = (పూర్ణాంకం)STRLEN(p);
	    new_cmdline = alloc(STRLEN(కార్యక్రమం) + (size_t)i * (len - 2) + 1);
	    ఒకవేళ (new_cmdline == ఖాళీ)
		తిరిగి ఖాళీ;			// out of memory
	    ptr = new_cmdline;
	    అయితే ((pos = (char_u *)అక్షరశ్రేణి_శోధన((అక్షరం *)కార్యక్రమం, "$*")) != ఖాళీ)
	    {
		i = (పూర్ణాంకం)(pos - కార్యక్రమం);
		STRNCPY(ptr, కార్యక్రమం, i);
		STRCPY(ptr += i, p);
		ptr += len;
		కార్యక్రమం = pos + 2;
	    }
	    STRCPY(ptr, కార్యక్రమం);
	}
	లేకపోతే
	{
	    size_t కార్యక్రమం_len = STRLEN(కార్యక్రమం);

	    new_cmdline = alloc(కార్యక్రమం_len + STRLEN(p) + 2);
	    ఒకవేళ (new_cmdline == ఖాళీ)
		తిరిగి ఖాళీ;			// out of memory
	    STRCPY(new_cmdline, కార్యక్రమం);
	    STRCPY(new_cmdline + కార్యక్రమం_len, " ");
	    STRCPY(new_cmdline + కార్యక్రమం_len + 1, p);
	}
	సందేశం_make(p);

	// 'eap->cmd' is not set here, because it is not used at CMD_make
	vim_free(*cmdlinep);
	*cmdlinep = new_cmdline;
	p = new_cmdline;
    }
    తిరిగి p;
}
#అయితే_ముగింపు

/*
 * Expand file name in Ex command argument.
 * When an error is detected, "errormsgp" is set to a non-NULL pointer.
 * Return FAIL for failure, OK otherwise.
 */
    పూర్ణాంకం
expand_filename(
    exarg_T	*eap,
    char_u	**cmdlinep,
    అక్షరం	**errormsgp)
{
    పూర్ణాంకం		has_wildcards;	// need to expand wildcards
    char_u	*repl;
    size_t	srclen;
    char_u	*p;
    పూర్ణాంకం		n;
    పూర్ణాంకం		escaped;

#నిర్వచితమైతే FEAT_QUICKFIX
    // Skip a regexp pattern for ":vimgrep[add] pat file..."
    p = skip_grep_pat(eap);
#లేకపోతే
    p = eap->arg;
#అయితే_ముగింపు

    /*
     * Decide to expand wildcards *before* replacing '%', '#', etc.  If
     * the file name contains a wildcard it should not cause expanding.
     * (it will be expanded anyway if there is a wildcard before replacing).
     */
    has_wildcards = mch_has_wildcard(p);
    అయితే (*p != NUL)
    {
#నిర్వచితమైతే FEAT_EVAL
	// Skip over `=expr`, wildcards in it are not expanded.
	ఒకవేళ (p[0] == '`' && p[1] == '=')
	{
	    p += 2;
	    (శూన్య)skip_సమాసం(&p, ఖాళీ);
	    ఒకవేళ (*p == '`')
		++p;
	    కొనసాగు;
	}
#అయితే_ముగింపు
	/*
	 * Quick check if this cannot be the start of a special string.
	 * Also removes backslash before '%', '#' and '<'.
	 */
	ఒకవేళ (vim_strchr((char_u *)"%#<", *p) == ఖాళీ)
	{
	    ++p;
	    కొనసాగు;
	}

	/*
	 * Try to find a match at this position.
	 */
	repl = eval_vars(p, eap->arg, &srclen, &(eap->do_ecmd_lnum),
						    errormsgp, &escaped, TRUE);
	ఒకవేళ (*errormsgp != ఖాళీ)		// error detected
	    తిరిగి FAIL;
	ఒకవేళ (repl == ఖాళీ)		// no match found
	{
	    p += srclen;
	    కొనసాగు;
	}

	// Wildcards won't be expanded below, the replacement is taken
	// literally.  But do expand "~/file", "~user/file" and "$HOME/file".
	ఒకవేళ (vim_strchr(repl, '$') != ఖాళీ || vim_strchr(repl, '~') != ఖాళీ)
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
	ఒకవేళ (!eap->usefilter
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
#నిర్వచితం_కాకపోతే UNIX
		&& !(eap->argt & EX_NOSPC)
#అయితే_ముగింపు
		)
	{
	    char_u	*l;
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
	    // Don't escape a backslash here, because rem_backslash() doesn't
	    // remove it later.
	    స్థిర char_u *nobslash = (char_u *)" \t\"|";
# define ESCAPE_CHARS nobslash
#లేకపోతే
# define ESCAPE_CHARS escape_chars
#అయితే_ముగింపు

	    కొరకు (l = repl; *l; ++l)
		ఒకవేళ (vim_strchr(ESCAPE_CHARS, *l) != ఖాళీ)
		{
		    l = vim_strsave_escaped(repl, ESCAPE_CHARS);
		    ఒకవేళ (l != ఖాళీ)
		    {
			vim_free(repl);
			repl = l;
		    }
		    ఆపు;
		}
	}

	// For a shell command a '!' must be escaped.
	ఒకవేళ ((eap->usefilter || eap->cmdidx == CMD_bang
						|| eap->cmdidx == CMD_terminal)
			    && vim_strpbrk(repl, (char_u *)"!") != ఖాళీ)
	{
	    char_u	*l;

	    l = vim_strsave_escaped(repl, (char_u *)"!");
	    ఒకవేళ (l != ఖాళీ)
	    {
		vim_free(repl);
		repl = l;
	    }
	}

	p = repl_cmdline(eap, p, srclen, repl, cmdlinep);
	vim_free(repl);
	ఒకవేళ (p == ఖాళీ)
	    తిరిగి FAIL;
    }

    /*
     * One file argument: Expand wildcards.
     * Don't do this with ":r !command" or ":w !command".
     */
    ఒకవేళ ((eap->argt & EX_NOSPC) && !eap->usefilter)
    {
	/*
	 * May do this twice:
	 * 1. Replace environment variables.
	 * 2. Replace any other wildcards, remove backslashes.
	 */
	కొరకు (n = 1; n <= 2; ++n)
	{
	    ఒకవేళ (n == 2)
	    {
		/*
		 * Halve the number of backslashes (this is Vi compatible).
		 * For Unix and OS/2, when wildcards are expanded, this is
		 * done by ExpandOne() below.
		 */
#ఒకవేళ defined(UNIX)
		ఒకవేళ (!has_wildcards)
#అయితే_ముగింపు
		    backslash_halve(eap->arg);
	    }

	    ఒకవేళ (has_wildcards)
	    {
		ఒకవేళ (n == 1)
		{
		    /*
		     * First loop: May expand environment variables.  This
		     * can be done much faster with expand_env() than with
		     * something else (e.g., calling a shell).
		     * After expanding environment variables, check again
		     * if there are still wildcards present.
		     */
		    ఒకవేళ (vim_strchr(eap->arg, '$') != ఖాళీ
			    || vim_strchr(eap->arg, '~') != ఖాళీ)
		    {
			expand_env_esc(eap->arg, NameBuff, MAXPATHL,
					(char_u *)(" \t" మార్గం_ESC_WILDCARDS),
					TRUE, ఖాళీ);
			has_wildcards = mch_has_wildcard(NameBuff);
			p = NameBuff;
		    }
		    లేకపోతే
			p = ఖాళీ;
		}
		లేకపోతే // n == 2
		{
		    expand_T	xpc;
		    పూర్ణాంకం		options = WILD_LIST_NOTFOUND
					       | WILD_NOERROR | WILD_కూడు_SLASH;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    ఒకవేళ (p_wic)
			options += WILD_ICASE;
		    p = ExpandOne(&xpc, eap->arg, ఖాళీ,
						   options, WILD_EXPAND_FREE);
		    ఒకవేళ (p == ఖాళీ)
			తిరిగి FAIL;
		}
		ఒకవేళ (p != ఖాళీ)
		{
		    (శూన్య)repl_cmdline(eap, eap->arg, STRLEN(eap->arg),
								 p, cmdlinep);
		    ఒకవేళ (n == 2)	// p came from ExpandOne()
			vim_free(p);
		}
	    }
	}
    }
    తిరిగి OK;
}

/*
 * Replace part of the command line, keeping eap->cmd, eap->arg and
 * eap->nextcmd correct.
 * "src" points to the part that is to be replaced, of length "srclen".
 * "repl" is the replacement string.
 * Returns a pointer to the character after the replaced string.
 * Returns NULL for failure.
 */
    స్థిర char_u *
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
    ఒకవేళ (eap->nextcmd != ఖాళీ)
	i += STRLEN(eap->nextcmd);	// add space for next command
    ఒకవేళ ((new_cmdline = alloc(i)) == ఖాళీ)
	తిరిగి ఖాళీ;			// out of memory!

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

    ఒకవేళ (eap->nextcmd != ఖాళీ)		// append next command
    {
	new_cmdlinelen += taillen + 1;
	STRCPY(new_cmdline + new_cmdlinelen, eap->nextcmd);
	eap->nextcmd = new_cmdline + new_cmdlinelen;
    }
    eap->cmd = new_cmdline + (eap->cmd - *cmdlinep);
    eap->arg = new_cmdline + (eap->arg - *cmdlinep);
    ఒకవేళ (eap->do_ecmd_cmd != ఖాళీ && eap->do_ecmd_cmd != dollar_command)
	eap->do_ecmd_cmd = new_cmdline + (eap->do_ecmd_cmd - *cmdlinep);
    vim_free(*cmdlinep);
    *cmdlinep = new_cmdline;

    తిరిగి src;
}

/*
 * Check for '|' to separate commands and '"' to start comments.
 * If "keep_backslash" is TRUE do not remove any backslash.
 */
    శూన్య
separate_nextcmd(exarg_T *eap, పూర్ణాంకం keep_backslash)
{
    పూర్ణాంకం		vim9script = లోపల_vim9script();
    char_u	*p;

#నిర్వచితమైతే FEAT_QUICKFIX
    p = skip_grep_pat(eap);
#లేకపోతే
    p = eap->arg;
#అయితే_ముగింపు

    కొరకు ( ; *p; MB_PTR_ADV(p))
    {
	ఒకవేళ (*p == Ctrl_V)
	{
	    ఒకవేళ ((eap->argt & (EX_CTRLV | EX_XFILE)) || keep_backslash)
		++p;		// skip CTRL-V and next char
	    లేకపోతే
				// remove CTRL-V and skip next char
		STRMOVE(p, p + 1);
	    ఒకవేళ (*p == NUL)		// stop at NUL after CTRL-V
		ఆపు;
	}

#నిర్వచితమైతే FEAT_EVAL
	// Skip over `=expr` when wildcards are expanded.
	లేకపోతే ఒకవేళ (p[0] == '`' && p[1] == '=' && (eap->argt & EX_XFILE))
	{
	    p += 2;
	    (శూన్య)skip_సమాసం(&p, ఖాళీ);
	    ఒకవేళ (*p == NUL)		// stop at NUL after CTRL-V
		ఆపు;
	}
#అయితే_ముగింపు

	// Check for '"'/'#': start of comment or '|': next command
	// :@" and :*" do not start a comment!
	// :redir @" doesn't either.
	లేకపోతే ఒకవేళ ((*p == '"'
		    && !vim9script
		    && !(eap->argt & EX_NOTRLCOM)
		    && ((eap->cmdidx != CMD_at && eap->cmdidx != CMD_నక్షత్రం)
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
		    && eap->cmdidx != CMD_చొప్పించు)
		|| *p == '\n')
	{
	    /*
	     * We remove the '\' before the '|', unless EX_CTRLV is used
	     * AND 'b' is present in 'cpoptions'.
	     */
	    ఒకవేళ ((vim_strchr(p_cpo, CPO_BAR) == ఖాళీ
			      || !(eap->argt & EX_CTRLV)) && *(p - 1) == '\\')
	    {
		ఒకవేళ (!keep_backslash)
		{
		    STRMOVE(p - 1, p);	// remove the '\'
		    --p;
		}
	    }
	    లేకపోతే
	    {
		సమితి_nextcmd(eap, p);
		*p = NUL;
		ఆపు;
	    }
	}
    }

    ఒకవేళ (!(eap->argt & EX_NOTRLCOM))	// remove trailing spaces
	del_trailing_spaces(eap->arg);
}

/*
 * get + command from ex argument
 */
    స్థిర char_u *
getargcmd(char_u **argp)
{
    char_u *arg = *argp;
    char_u *command = ఖాళీ;

    ఒకవేళ (*arg == '+')	    // +[command]
    {
	++arg;
	ఒకవేళ (vim_isspace(*arg) || *arg == NUL)
	    command = dollar_command;
	లేకపోతే
	{
	    command = arg;
	    arg = skip_cmd_arg(command, TRUE);
	    ఒకవేళ (*arg != NUL)
		*arg++ = NUL;		// terminate command with NUL
	}

	arg = skipwhite(arg);	// skip over spaces
	*argp = arg;
    }
    తిరిగి command;
}

/*
 * Find end of "+command" argument.  Skip over "\ " and "\\".
 */
    char_u *
skip_cmd_arg(
    char_u *p,
    పూర్ణాంకం	   rembs)	// TRUE to halve the number of backslashes
{
    అయితే (*p && !vim_isspace(*p))
    {
	ఒకవేళ (*p == '\\' && p[1] != NUL)
	{
	    ఒకవేళ (rembs)
		STRMOVE(p, p + 1);
	    లేకపోతే
		++p;
	}
	MB_PTR_ADV(p);
    }
    తిరిగి p;
}

    పూర్ణాంకం
get_bad_opt(char_u *p, exarg_T *eap)
{
    ఒకవేళ (STRICMP(p, "keep") == 0)
	eap->bad_char = BAD_KEEP;
    లేకపోతే ఒకవేళ (STRICMP(p, "drop") == 0)
	eap->bad_char = BAD_DROP;
    లేకపోతే ఒకవేళ (MB_BYTE2LEN(*p) == 1 && p[1] == NUL)
	eap->bad_char = *p;
    లేకపోతే
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of bad= names.
 */
    స్థిర char_u *
get_bad_name(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    // Note: Keep this in sync with getargopt.
    స్థిర అక్షరం *(p_bad_విలువలు[]) =
    {
	"?",
	"keep",
	"drop",
    };

    ఒకవేళ (idx < 0 || idx >= (పూర్ణాంకం)ARRAY_నిడివి(p_bad_విలువలు))
	తిరిగి ఖాళీ;

    తిరిగి (char_u*)p_bad_విలువలు[idx];
}

/*
 * Get "++opt=arg" argument.
 * Return FAIL or OK.
 */
    స్థిర పూర్ణాంకం
getargopt(exarg_T *eap)
{
    char_u	*arg = eap->arg + 2;
    పూర్ణాంకం		*pp = ఖాళీ;
    పూర్ణాంకం		bad_char_idx;
    char_u	*p;

    // Note: Keep this in sync with get_argopt_name.

    // ":edit ++[no]bin[ary] file"
    ఒకవేళ (STRNCMP(arg, "bin", 3) == 0 || STRNCMP(arg, "nobin", 5) == 0)
    {
	ఒకవేళ (*arg == 'n')
	{
	    arg += 2;
	    eap->force_bin = FORCE_NOBIN;
	}
	లేకపోతే
	    eap->force_bin = FORCE_BIN;
	ఒకవేళ (!checkforcmd(&arg, "binary", 3))
	    తిరిగి FAIL;
	eap->arg = skipwhite(arg);
	తిరిగి OK;
    }

    // ":read ++edit file"
    ఒకవేళ (STRNCMP(arg, "edit", 4) == 0)
    {
	eap->read_edit = TRUE;
	eap->arg = skipwhite(arg + 4);
	తిరిగి OK;
    }

    ఒకవేళ (STRNCMP(arg, "ff", 2) == 0)
    {
	arg += 2;
	pp = &eap->force_ff;
    }
    లేకపోతే ఒకవేళ (STRNCMP(arg, "fileformat", 10) == 0)
    {
	arg += 10;
	pp = &eap->force_ff;
    }
    లేకపోతే ఒకవేళ (STRNCMP(arg, "enc", 3) == 0)
    {
	ఒకవేళ (STRNCMP(arg, "encoding", 8) == 0)
	    arg += 8;
	లేకపోతే
	    arg += 3;
	pp = &eap->force_enc;
    }
    లేకపోతే ఒకవేళ (STRNCMP(arg, "bad", 3) == 0)
    {
	arg += 3;
	pp = &bad_char_idx;
    }

    ఒకవేళ (pp == ఖాళీ || *arg != '=')
	తిరిగి FAIL;

    ++arg;
    *pp = (పూర్ణాంకం)(arg - eap->cmd);
    arg = skip_cmd_arg(arg, FALSE);
    eap->arg = skipwhite(arg);
    *arg = NUL;

    ఒకవేళ (pp == &eap->force_ff)
    {
	ఒకవేళ (check_ff_విలువ(eap->cmd + eap->force_ff) == FAIL)
	    తిరిగి FAIL;
	eap->force_ff = eap->cmd[eap->force_ff];
    }
    లేకపోతే ఒకవేళ (pp == &eap->force_enc)
    {
	// Make 'fileencoding' lower case.
	కొరకు (p = eap->cmd + eap->force_enc; *p != NUL; ++p)
	    *p = TOLOWER_ASC(*p);
    }
    లేకపోతే
    {
	// Check ++bad= argument.  Must be a single-byte character, "keep" or
	// "drop".
	ఒకవేళ (get_bad_opt(eap->cmd + bad_char_idx, eap) == FAIL)
	    తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of ++opt names.
 */
    స్థిర char_u *
get_argopt_name(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    // Note: Keep this in sync with getargopt.
    స్థిర అక్షరం *(p_opt_విలువలు[]) =
    {
	"fileformat=",
	"encoding=",
	"binary",
	"nobinary",
	"bad=",
	"edit",
    };

    ఒకవేళ (idx < 0 || idx >= (పూర్ణాంకం)ARRAY_నిడివి(p_opt_విలువలు))
	తిరిగి ఖాళీ;

    తిరిగి (char_u*)p_opt_విలువలు[idx];
}

/*
 * Command-line expansion for ++opt=name.
 */
    పూర్ణాంకం
expand_argopt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	పూర్ణాంకం	    *అంకె_Matches)
{
    ఒకవేళ (xp->xp_నమూనా > xp->xp_పంక్తి && *(xp->xp_నమూనా-1) == '=')
    {
	char_u *(*cb)(expand_T *, పూర్ణాంకం) = ఖాళీ;

	char_u *name_చివర = xp->xp_నమూనా - 1;
	ఒకవేళ (name_చివర - xp->xp_పంక్తి >= 2
		&& STRNCMP(name_చివర - 2, "ff", 2) == 0)
	    cb = get_fileformat_name;
	లేకపోతే ఒకవేళ (name_చివర - xp->xp_పంక్తి >= 10
		&& STRNCMP(name_చివర - 10, "fileformat", 10) == 0)
	    cb = get_fileformat_name;
	లేకపోతే ఒకవేళ (name_చివర - xp->xp_పంక్తి >= 3
		&& STRNCMP(name_చివర - 3, "enc", 3) == 0)
	    cb = get_encoding_name;
	లేకపోతే ఒకవేళ (name_చివర - xp->xp_పంక్తి >= 8
		&& STRNCMP(name_చివర - 8, "encoding", 8) == 0)
	    cb = get_encoding_name;
	లేకపోతే ఒకవేళ (name_చివర - xp->xp_పంక్తి >= 3
		&& STRNCMP(name_చివర - 3, "bad", 3) == 0)
	    cb = get_bad_name;

	ఒకవేళ (cb != ఖాళీ)
	{
	    తిరిగి ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    అంకె_Matches,
		    cb,
		    FALSE);
	}
	తిరిగి FAIL;
    }

    // Special handling of "ff" which acts as a short form of
    // "fileformat", as "ff" is not a substring of it.
    ఒకవేళ (xp->xp_నమూనా_len == 2
	    && STRNCMP(xp->xp_నమూనా, "ff", xp->xp_నమూనా_len) == 0)
    {
	*matches = ALLOC_MULT(char_u *, 1);
	ఒకవేళ (*matches == ఖాళీ)
	    తిరిగి FAIL;
	*అంకె_Matches = 1;
	(*matches)[0] = vim_strsave((char_u*)"fileformat=");
	తిరిగి OK;
    }

    తిరిగి ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    అంకె_Matches,
	    get_argopt_name,
	    FALSE);
}

    స్థిర శూన్య
ex_autocmd(exarg_T *eap)
{
    ఒకవేళ (eap->skip)
    {
	skip_cmd_ఖండం(eap);
	తిరిగి;
    }

    /*
     * Disallow autocommands from .exrc and .vimrc in current
     * directory for security reasons.
     */
    ఒకవేళ (secure)
    {
	secure = 2;
	eap->errmsg =
	      _(e_command_not_allowed_from_vimrc_లోపల_current_dir_or_ట్యాగ్_శోధన);
    }
    లేకపోతే ఒకవేళ (eap->cmdidx == CMD_autocmd)
	do_autocmd(eap, eap->arg, eap->forceit);
    లేకపోతే
	do_augroup(eap->arg, eap->forceit);
}

/*
 * ":doautocmd": Apply the automatic commands to the current buffer.
 */
    స్థిర శూన్య
ex_doautocmd(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    పూర్ణాంకం		call_do_modelines = check_nomodeline(&arg);
    పూర్ణాంకం		did_aucmd;

    (శూన్య)do_doautocmd(arg, TRUE, &did_aucmd);
    // Only when there is no <nomodeline>.
    ఒకవేళ (call_do_modelines && did_aucmd)
	do_modelines(0);
}

/*
 * :[N]bunload[!] [N] [bufname] unload buffer
 * :[N]bdelete[!] [N] [bufname] delete buffer from buffer list
 * :[N]bwipeout[!] [N] [bufname] delete buffer really
 */
    స్థిర శూన్య
ex_bunload(exarg_T *eap)
{
    ఒకవేళ (ERROR_IF_ANY_POPUP_కిటికీ)
	తిరిగి;
    eap->errmsg = do_bufdel(
	    eap->cmdidx == CMD_bdelete ? DOBUF_DEL
		: eap->cmdidx == CMD_bwipeout ? DOBUF_WIPE
		: DOBUF_UNLOAD, eap->arg,
	    eap->addr_లెక్క, (పూర్ణాంకం)eap->line1, (పూర్ణాంకం)eap->line2, eap->forceit);
}

/*
 * :[N]buffer [N]	to buffer N
 * :[N]sbuffer [N]	to buffer N
 */
    స్థిర శూన్య
ex_బఫరు(exarg_T *eap)
{
    ఒకవేళ (ERROR_IF_ANY_POPUP_కిటికీ)
	తిరిగి;
    do_exbuffer(eap);
}

/*
 * ":buffer" command and alike.
 */
    స్థిర శూన్య
do_exbuffer(exarg_T *eap)
{
    ఒకవేళ (*eap->arg)
	eap->errmsg = ex_errmsg(e_trailing_characters_str, eap->arg);
    లేకపోతే
    {
	ఒకవేళ (eap->addr_లెక్క == 0)	// default is current buffer
	    goto_బఫరు(eap, DOBUF_CURRENT, FORWARD, 0);
	లేకపోతే
	    goto_బఫరు(eap, DOBUF_మొదటి, FORWARD, (పూర్ణాంకం)eap->line2);
	ఒకవేళ (eap->do_ecmd_cmd != ఖాళీ)
	    do_cmd_వాదం(eap->do_ecmd_cmd);
    }
}

/*
 * :[N]bmodified [N]	to next mod. buffer
 * :[N]sbmodified [N]	to next mod. buffer
 */
    స్థిర శూన్య
ex_bmodified(exarg_T *eap)
{
    goto_బఫరు(eap, DOBUF_MOD, FORWARD, (పూర్ణాంకం)eap->line2);
    ఒకవేళ (eap->do_ecmd_cmd != ఖాళీ)
	do_cmd_వాదం(eap->do_ecmd_cmd);
}

/*
 * :[N]bnext [N]	to next buffer
 * :[N]sbnext [N]	split and to next buffer
 */
    స్థిర శూన్య
ex_bnext(exarg_T *eap)
{
    ఒకవేళ (ERROR_IF_ANY_POPUP_కిటికీ)
	తిరిగి;

    goto_బఫరు(eap, DOBUF_CURRENT, FORWARD, (పూర్ణాంకం)eap->line2);
    ఒకవేళ (eap->do_ecmd_cmd != ఖాళీ)
	do_cmd_వాదం(eap->do_ecmd_cmd);
}

/*
 * :[N]bNext [N]	to previous buffer
 * :[N]bprevious [N]	to previous buffer
 * :[N]sbNext [N]	split and to previous buffer
 * :[N]sbprevious [N]	split and to previous buffer
 */
    స్థిర శూన్య
ex_bprevious(exarg_T *eap)
{
    ఒకవేళ (ERROR_IF_ANY_POPUP_కిటికీ)
	తిరిగి;

    goto_బఫరు(eap, DOBUF_CURRENT, BACKWARD, (పూర్ణాంకం)eap->line2);
    ఒకవేళ (eap->do_ecmd_cmd != ఖాళీ)
	do_cmd_వాదం(eap->do_ecmd_cmd);
}

/*
 * :brewind		to first buffer
 * :bfirst		to first buffer
 * :sbrewind		split and to first buffer
 * :sbfirst		split and to first buffer
 */
    స్థిర శూన్య
ex_brewind(exarg_T *eap)
{
    ఒకవేళ (ERROR_IF_ANY_POPUP_కిటికీ)
	తిరిగి;

    goto_బఫరు(eap, DOBUF_మొదటి, FORWARD, 0);
    ఒకవేళ (eap->do_ecmd_cmd != ఖాళీ)
	do_cmd_వాదం(eap->do_ecmd_cmd);
}

/*
 * :blast		to last buffer
 * :sblast		split and to last buffer
 */
    స్థిర శూన్య
ex_blast(exarg_T *eap)
{
    ఒకవేళ (ERROR_IF_ANY_POPUP_కిటికీ)
	తిరిగి;

    goto_బఫరు(eap, DOBUF_చివరి, BACKWARD, 0);
    ఒకవేళ (eap->do_ecmd_cmd != ఖాళీ)
	do_cmd_వాదం(eap->do_ecmd_cmd);
}

/*
 * Check if "c" ends an Ex command.
 * In Vim9 script does not check for white space before #.
 */
    పూర్ణాంకం
చివర్లు_excmd(పూర్ణాంకం c)
{
    పూర్ణాంకం comment_char = '"';

    ఒకవేళ (లోపల_vim9script())
	comment_char = '#';
    తిరిగి (c == NUL || c == '|' || c == comment_char || c == '\n');
}

/*
 * Like ends_excmd() but checks that a # in Vim9 script either has "cmd" equal
 * to "cmd_start" or has a white space character before it.
 */
    పూర్ణాంకం
చివర్లు_excmd2(char_u *cmd_ప్రారంభం వాడని, char_u *cmd)
{
    పూర్ణాంకం c = *cmd;

    ఒకవేళ (c == NUL || c == '|' || c == '\n')
	తిరిగి TRUE;
    ఒకవేళ (లోపల_vim9script())
	//  # starts a comment, #{ might be a mistake, #{{ can start a fold
	తిరిగి c == '#' && (cmd[1] != '{' || cmd[2] == '{')
				 && (cmd == cmd_ప్రారంభం || VIM_ISWHITE(cmd[-1]));
    తిరిగి c == '"';
}

#ఒకవేళ defined(FEAT_SYN_HL) || defined(FEAT_శోధన_EXTRA) || defined(FEAT_EVAL)
/*
 * Return the next command, after the first '|' or '\n'.
 * Return NULL if not found.
 */
    char_u *
కనుగొను_nextcmd(char_u *p)
{
    అయితే (*p != '|' && *p != '\n')
    {
	ఒకవేళ (*p == NUL)
	    తిరిగి ఖాళీ;
	++p;
    }
    తిరిగి (p + 1);
}
#అయితే_ముగింపు

/*
 * Check if *p is a separator between Ex commands, skipping over white space.
 * Return NULL if it isn't, the following character if it is.
 */
    char_u *
check_nextcmd(char_u *p)
{
    char_u *s = skipwhite(p);

    ఒకవేళ (*s == '|' || *s == '\n')
	తిరిగి (s + 1);
    లేకపోతే
	తిరిగి ఖాళీ;
}

/*
 * If "eap->nextcmd" is not set, check for a next command at "p".
 */
    శూన్య
సమితి_nextcmd(exarg_T *eap, char_u *arg)
{
    char_u *p = skipwhite(arg);

    ఒకవేళ (లోపల_vim9script() && *p == '#')
    {
	char_u *nl = vim_strchr(p, NL);
	ఒకవేళ (nl != ఖాళీ)
	    p = nl;
    }

    p = check_nextcmd(p);

    ఒకవేళ (eap->nextcmd == ఖాళీ)
	eap->nextcmd = p;
    లేకపోతే ఒకవేళ (p != ఖాళీ)
	// cannot use "| command" inside a  {} block
	semsg(_(e_cannot_use_bar_గమ్యం_separate_commands_ఇక్కడ_str), arg);
}

/*
 * - if there are more files to edit
 * - and this is the last window
 * - and forceit not used
 * - and not repeated twice on a row
 *    return FAIL and give error message if 'message' TRUE
 * return OK otherwise
 */
    స్థిర పూర్ణాంకం
check_మరిన్ని(
    పూర్ణాంకం సందేశం_2,	    // when FALSE check only, no messages
    పూర్ణాంకం forceit)
{
    పూర్ణాంకం	    n = ARGCOUNT - curwin->w_arg_idx - 1;

    ఒకవేళ (!forceit && only_one_కిటికీ()
	    && ARGCOUNT > 1 && !arg_had_చివరి && n > 0 && quitmore == 0)
    {
	ఒకవేళ (సందేశం_2)
	{
#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ఒకవేళ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						    && curbuf->b_fname != ఖాళీ)
	    {
		char_u	buff[DIALOG_సందేశం_కొలత];

		vim_snprintf((అక్షరం *)buff, DIALOG_సందేశం_కొలత,
			NGETTEXT("%d more file to edit.  Quit anyway?",
			    "%d more files to edit.  Quit anyway?", n), n);
		ఒకవేళ (vim_dialog_yesno(VIM_QUESTION, ఖాళీ, buff, 1) == VIM_YES)
		    తిరిగి OK;
		తిరిగి FAIL;
	    }
#అయితే_ముగింపు
	    semsg(NGETTEXT(e_nr_మరిన్ని_file_గమ్యం_edit,
			   e_nr_మరిన్ని_files_గమ్యం_edit , n), n);
	    quitmore = 2;	    // next try to quit is allowed
	}
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of command names.
 */
    char_u *
get_command_name(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx >= (పూర్ణాంకం)CMD_కొలత)
	తిరిగి expand_user_command_name(idx);
    // the following are no real commands
    ఒకవేళ (STRNCMP(cmdnames[idx].cmd_name, "{", 1) == 0 ||
	STRNCMP(cmdnames[idx].cmd_name, "}", 1) == 0)
	తిరిగి (char_u *)"";
    తిరిగి cmdnames[idx].cmd_name;
}

    స్థిర శూన్య
ex_colorscheme(exarg_T *eap)
{
    ఒకవేళ (*eap->arg == NUL)
    {
#నిర్వచితమైతే FEAT_EVAL
	char_u *సమాసం = vim_strsave((char_u *)"g:colors_name");
	char_u *p = ఖాళీ;

	ఒకవేళ (సమాసం != ఖాళీ)
	{
	    ++emsg_off;
	    p = eval_గమ్యం_string(సమాసం, FALSE, FALSE);
	    --emsg_off;
	    vim_free(సమాసం);
	}
	ఒకవేళ (p != ఖాళీ)
	{
	    సందేశం((అక్షరం *)p);
	    vim_free(p);
	}
	లేకపోతే
	    సందేశం("default");
#లేకపోతే
	సందేశం(_("unknown"));
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ (ఎక్కించు_colors(eap->arg) == FAIL)
	semsg(_(e_cannot_కనుగొను_color_scheme_str), eap->arg);

#నిర్వచితమైతే FEAT_VTP
    లేకపోతే ఒకవేళ (has_vtp_working())
    {
	// background color change requires clear + redraw
	update_screen(UPD_CLEAR);
	redrawcmd();
    }
#అయితే_ముగింపు
}

    స్థిర శూన్య
ex_highlight(exarg_T *eap)
{
    ఒకవేళ (*eap->arg == NUL && eap->cmd[2] == '!')
	సందేశం(_("Greetings, Vim user!"));
    do_highlight(eap->arg, eap->forceit, FALSE);
}


/*
 * Call this function if we thought we were going to exit, but we won't
 * (because of an error).  May need to restore the terminal mode.
 */
    శూన్య
not_exiting(పూర్ణాంకం save_exiting)
{
    exiting = save_exiting;
    settmode(TMODE_RAW);
}

    పూర్ణాంకం
before_quit_autocmds(win_T *wp, పూర్ణాంకం quit_all, పూర్ణాంకం forceit)
{
    apply_autocmds(EVENT_QUITPRE, ఖాళీ, ఖాళీ, FALSE, wp->w_బఫరు);

    // Bail out when autocommands closed the window.
    // Refuse to quit when the buffer in the last window is being closed (can
    // only happen in autocommands).
    ఒకవేళ (!win_valid(wp)
	    || curbuf_locked()
	    || (wp->w_బఫరు->b_nwindows == 1 && wp->w_బఫరు->b_locked > 0))
	తిరిగి TRUE;

    ఒకవేళ (quit_all || (check_మరిన్ని(FALSE, forceit) == OK && only_one_కిటికీ()))
    {
	apply_autocmds(EVENT_EXITPRE, ఖాళీ, ఖాళీ, FALSE, curbuf);
	// Refuse to quit when locked or when the window was closed or the
	// buffer in the last window is being closed (can only happen in
	// autocommands).
	ఒకవేళ (!win_valid(wp) || curbuf_locked()
			  || (curbuf->b_nwindows == 1 && curbuf->b_locked > 0))
	    తిరిగి TRUE;
    }

    తిరిగి FALSE;
}

/*
 * ":quit": quit current window, quit Vim if the last window is closed.
 * ":{nr}quit": quit window {nr}
 * Also used when closing a terminal window that's the last one.
 */
    శూన్య
ex_quit(exarg_T *eap)
{
    win_T	*wp;

    ఒకవేళ (cmdwin_type != 0)
    {
	cmdwin_ఫలితం = Ctrl_C;
	తిరిగి;
    }
    // Don't quit while editing the command line.
    ఒకవేళ (పాఠ్యం_locked())
    {
	పాఠ్యం_locked_సందేశం();
	తిరిగి;
    }
    ఒకవేళ (eap->addr_లెక్క > 0)
    {
	పూర్ణాంకం	wnr = eap->line2;

	కొరకు (wp = firstwin; wp->w_తదుపరి != ఖాళీ; wp = wp->w_తదుపరి)
	    ఒకవేళ (--wnr <= 0)
		ఆపు;
    }
    లేకపోతే
	wp = curwin;

    // Refuse to quit when locked.
    ఒకవేళ (curbuf_locked())
	తిరిగి;

    // Trigger QuitPre and maybe ExitPre
    ఒకవేళ (before_quit_autocmds(wp, FALSE, eap->forceit))
	తిరిగి;

#నిర్వచితమైతే FEAT_NETBEANS_INTG
    netbeansForcedQuit = eap->forceit;
#అయితే_ముగింపు

    పూర్ణాంకం save_exiting = exiting;
    /*
     * If there is only one relevant window we will exit.
     */
    ఒకవేళ (check_మరిన్ని(FALSE, eap->forceit) == OK && only_one_కిటికీ())
	exiting = TRUE;
    ఒకవేళ ((!buf_hide(wp->w_బఫరు)
		&& check_మారింది(wp->w_బఫరు, (p_awa ? CCGD_AW : 0)
				       | (eap->forceit ? CCGD_FORCEIT : 0)
				       | CCGD_EXCMD))
	    || check_మరిన్ని(TRUE, eap->forceit) == FAIL
	    || (only_one_కిటికీ() && check_మారింది_any(eap->forceit, TRUE)))
    {
	not_exiting(save_exiting);
    }
    లేకపోతే
    {
	// quit last window
	// Note: only_one_window() returns true, even so a help window is
	// still open. In that case only quit, if no address has been
	// specified. Example:
	// :h|wincmd w|1q     - don't quit
	// :h|wincmd w|q      - quit
	ఒకవేళ (only_one_కిటికీ() && (ONE_కిటికీ || eap->addr_లెక్క == 0))
	    getout(0);
	not_exiting(save_exiting);
#నిర్వచితమైతే FEAT_GUI
	need_mouse_correct = TRUE;
#అయితే_ముగింపు
	// close window; may free buffer
	win_close(wp, !buf_hide(wp->w_బఫరు) || eap->forceit);
    }
}

/*
 * ":cquit".
 */
    స్థిర శూన్య
ex_cquit(exarg_T *eap వాడని)
{
    // this does not always pass on the exit code to the Manx compiler. why?
    getout(eap->addr_లెక్క > 0 ? (పూర్ణాంకం)eap->line2 : EXIT_FAILURE);
}

/*
 * Do preparations for "qall" and "wqall".
 * Returns FAIL when quitting should be aborted.
 */
    పూర్ణాంకం
before_quit_all(exarg_T *eap)
{
    ఒకవేళ (cmdwin_type != 0)
    {
	ఒకవేళ (eap->forceit)
	    cmdwin_ఫలితం = K_XF1;	// ex_window() takes care of this
	లేకపోతే
	    cmdwin_ఫలితం = K_XF2;
	తిరిగి FAIL;
    }

    // Don't quit while editing the command line.
    ఒకవేళ (పాఠ్యం_locked())
    {
	పాఠ్యం_locked_సందేశం();
	తిరిగి FAIL;
    }

    ఒకవేళ (before_quit_autocmds(curwin, TRUE, eap->forceit))
	తిరిగి FAIL;

    తిరిగి OK;
}

/*
 * ":qall": try to quit all windows
 */
    స్థిర శూన్య
ex_quit_all(exarg_T *eap)
{
    ఒకవేళ (before_quit_all(eap) == FAIL)
	తిరిగి;
    పూర్ణాంకం save_exiting = exiting;
    exiting = TRUE;
    ఒకవేళ (eap->forceit || !check_మారింది_any(FALSE, FALSE))
	getout(0);
    not_exiting(save_exiting);
}

/*
 * ":close": close current window, unless it is the last one
 */
    స్థిర శూన్య
ex_close(exarg_T *eap)
{
    win_T	*win;
    పూర్ణాంకం		winnr = 0;
    ఒకవేళ (cmdwin_type != 0)
	cmdwin_ఫలితం = Ctrl_C;
    లేకపోతే
	ఒకవేళ (!పాఠ్యం_locked() && !curbuf_locked())
	{
	    ఒకవేళ (eap->addr_లెక్క == 0)
		ex_win_close(eap->forceit, curwin, ఖాళీ);
	    లేకపోతే
	    {
		FOR_ALL_WINDOWS(win)
		{
		    winnr++;
		    ఒకవేళ (winnr == eap->line2)
			ఆపు;
		}
		ఒకవేళ (win == ఖాళీ)
		    win = lastwin;
		ex_win_close(eap->forceit, win, ఖాళీ);
	    }
	}
}

#నిర్వచితమైతే FEAT_QUICKFIX
/*
 * ":pclose": Close any preview window.
 */
    స్థిర శూన్య
ex_pclose(exarg_T *eap)
{
    win_T	*win;

    // First close any normal window.
    FOR_ALL_WINDOWS(win)
	ఒకవేళ (win->w_p_pvw)
	{
	    ex_win_close(eap->forceit, win, ఖాళీ);
	    తిరిగి;
	}
# ifdef FEAT_PROP_POPUP
    // Also when 'previewpopup' is empty, it might have been cleared.
    popup_close_preview();
# endif
}
#అయితే_ముగింపు

/*
 * Close window "win" and take care of handling closing the last window for a
 * modified buffer.
 */
    స్థిర శూన్య
ex_win_close(
    పూర్ణాంకం		forceit,
    win_T	*win,
    tabpage_T	*tp)		// NULL or the tab page "win" is in
{
    పూర్ణాంకం		need_hide;
    buf_T	*buf = win->w_బఫరు;

    // Never close the autocommand window.
    ఒకవేళ (is_aucmd_win(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_కిటికీ));
	తిరిగి;
    }
    ఒకవేళ (కిటికీ_layout_locked(CMD_close))
	తిరిగి;

    need_hide = (buf_Is_మారింది(buf) && buf->b_nwindows <= 1);
    ఒకవేళ (need_hide && !buf_hide(buf) && !forceit)
    {
#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ఒకవేళ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    ఒకవేళ (పదం_పని_running(buf->b_పదం))
	    {
		ఒకవేళ (పదం_confirm_stop(buf) == FAIL)
		    తిరిగి;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(buf);
		need_hide = FALSE;
	    }
	    లేకపోతే
# endif
	    {
		bufref_T bufref;

		సమితి_bufref(&bufref, buf);
		dialog_మారింది(buf, FALSE);
		ఒకవేళ (bufref_valid(&bufref) && buf_Is_మారింది(buf))
		    తిరిగి;
		need_hide = FALSE;
	    }
	}
	లేకపోతే
#అయితే_ముగింపు
	{
	    no_write_సందేశం();
	    తిరిగి;
	}
    }

#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు

    // free buffer when not hiding it or when it's a scratch buffer
    ఒకవేళ (tp == ఖాళీ)
	win_close(win, !need_hide && !buf_hide(buf));
    లేకపోతే
	win_close_othertab(win, !need_hide && !buf_hide(buf), tp);
}

/*
 * Handle the argument for a tabpage related ex command.
 * Returns a tabpage number.
 * When an error is encountered then eap->errmsg is set.
 */
    స్థిర పూర్ణాంకం
get_tabpage_arg(exarg_T *eap)
{
    పూర్ణాంకం tab_సంఖ్య;
    పూర్ణాంకం unaccept_arg0 = (eap->cmdidx == CMD_tabmove) ? 0 : 1;

    ఒకవేళ (eap->arg && *eap->arg != NUL)
    {
	char_u *p = eap->arg;
	char_u *p_save;
	పూర్ణాంకం    relative = 0; // argument +N/-N means: go to N places to the
			     // right/left relative to the current position.

	ఒకవేళ (*p == '-')
	{
	    relative = -1;
	    p++;
	}
	లేకపోతే ఒకవేళ (*p == '+')
	{
	    relative = 1;
	    p++;
	}

	p_save = p;
	tab_సంఖ్య = getdigits(&p);

	ఒకవేళ (relative == 0)
	{
	    ఒకవేళ (STRCMP(p, "$") == 0)
		tab_సంఖ్య = చివరి_TAB_NR;
	    లేకపోతే ఒకవేళ (STRCMP(p, "#") == 0)
		ఒకవేళ (valid_tabpage(lastused_tabpage))
		    tab_సంఖ్య = tabpage_సూచీ(lastused_tabpage);
		లేకపోతే
		{
		    eap->errmsg = ex_errmsg(e_invalid_విలువ_for_వాదం_str,
								     eap->arg);
		    tab_సంఖ్య = 0;
		    వెళ్ళు theend;
		}
	    లేకపోతే ఒకవేళ (p == p_save || *p_save == '-' || *p != NUL
		    || tab_సంఖ్య > చివరి_TAB_NR)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_వాదం_str, eap->arg);
		వెళ్ళు theend;
	    }
	}
	లేకపోతే
	{
	    ఒకవేళ (*p_save == NUL)
		tab_సంఖ్య = 1;
	    లేకపోతే ఒకవేళ (p == p_save || *p_save == '-' || *p != NUL
		    || tab_సంఖ్య == 0)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_వాదం_str, eap->arg);
		వెళ్ళు theend;
	    }
	    tab_సంఖ్య = tab_సంఖ్య * relative + tabpage_సూచీ(curtab);
	    ఒకవేళ (!unaccept_arg0 && relative == -1)
		--tab_సంఖ్య;
	}
	ఒకవేళ (tab_సంఖ్య < unaccept_arg0 || tab_సంఖ్య > చివరి_TAB_NR)
	    eap->errmsg = ex_errmsg(e_invalid_వాదం_str, eap->arg);
    }
    లేకపోతే ఒకవేళ (eap->addr_లెక్క > 0)
    {
	ఒకవేళ (unaccept_arg0 && eap->line2 == 0)
	{
	    eap->errmsg = _(e_invalid_range);
	    tab_సంఖ్య = 0;
	}
	లేకపోతే
	{
	    tab_సంఖ్య = eap->line2;
	    ఒకవేళ (!unaccept_arg0)
	    {
		char_u *cmdp = eap->cmd;

		అయితే (--cmdp > *eap->cmdlinep
			&& (VIM_ISWHITE(*cmdp) || VIM_ISDIGIT(*cmdp)))
		    ;
		ఒకవేళ (*cmdp == '-')
		{
		    --tab_సంఖ్య;
		    ఒకవేళ (tab_సంఖ్య < unaccept_arg0)
			eap->errmsg = _(e_invalid_range);
		}
	    }
	}
    }
    లేకపోతే
    {
	ఎంపిక (eap->cmdidx)
	{
	స్థితి CMD_tabnext:
	    tab_సంఖ్య = tabpage_సూచీ(curtab) + 1;
	    ఒకవేళ (tab_సంఖ్య > చివరి_TAB_NR)
		tab_సంఖ్య = 1;
	    ఆపు;
	స్థితి CMD_tabmove:
	    tab_సంఖ్య = చివరి_TAB_NR;
	    ఆపు;
	అప్రమేయ:
	    tab_సంఖ్య = tabpage_సూచీ(curtab);
	}
    }

theend:
    తిరిగి tab_సంఖ్య;
}

/*
 * ":tabclose": close current tab page, unless it is the last one.
 * ":tabclose N": close tab page N.
 */
    స్థిర శూన్య
ex_tabclose(exarg_T *eap)
{
    tabpage_T	*tp;
    పూర్ణాంకం		tab_సంఖ్య;

    ఒకవేళ (cmdwin_type != 0)
    {
	cmdwin_ఫలితం = K_IGNORE;
	తిరిగి;
    }

    ఒకవేళ (మొదటి_tabpage->tp_తదుపరి == ఖాళీ)
    {
	emsg(_(e_cannot_close_చివరి_tab_పుట));
	తిరిగి;
    }

    ఒకవేళ (కిటికీ_layout_locked(CMD_tabclose))
	తిరిగి;

    tab_సంఖ్య = get_tabpage_arg(eap);
    ఒకవేళ (eap->errmsg != ఖాళీ)
	తిరిగి;

    tp = కనుగొను_tabpage(tab_సంఖ్య);
    ఒకవేళ (tp == ఖాళీ)
    {
	beep_flush();
	తిరిగి;
    }
    ఒకవేళ (tp != curtab)
    {
	tabpage_close_other(tp, eap->forceit);
	తిరిగి;
    }
    లేకపోతే ఒకవేళ (!పాఠ్యం_locked() && !curbuf_locked())
	tabpage_close(eap->forceit);
}

/*
 * ":tabonly": close all tab pages except the current one
 */
    స్థిర శూన్య
ex_tabonly(exarg_T *eap)
{
    tabpage_T	*tp;
    పూర్ణాంకం		పూర్తి;
    పూర్ణాంకం		tab_సంఖ్య;

    ఒకవేళ (cmdwin_type != 0)
    {
	cmdwin_ఫలితం = K_IGNORE;
	తిరిగి;
    }

    ఒకవేళ (మొదటి_tabpage->tp_తదుపరి == ఖాళీ)
    {
	సందేశం(_("Already only one tab page"));
	తిరిగి;
    }

    ఒకవేళ (కిటికీ_layout_locked(CMD_tabonly))
	తిరిగి;

    tab_సంఖ్య = get_tabpage_arg(eap);
    ఒకవేళ (eap->errmsg != ఖాళీ)
	తిరిగి;

    goto_tabpage(tab_సంఖ్య);
    // Repeat this up to a 1000 times, because autocommands may
    // mess up the lists.
    కొరకు (పూర్తి = 0; పూర్తి < 1000; ++పూర్తి)
    {
	FOR_ALL_TABPAGES(tp)
	    ఒకవేళ (tp->tp_topframe != topframe)
	    {
		tabpage_close_other(tp, eap->forceit);
		// if we failed to close it quit
		ఒకవేళ (valid_tabpage(tp))
		    పూర్తి = 1000;
		// start over, "tp" is now invalid
		ఆపు;
	    }
	ఒకవేళ (మొదటి_tabpage->tp_తదుపరి == ఖాళీ)
	    ఆపు;
    }
}

/*
 * Close the current tab page.
 */
    శూన్య
tabpage_close(పూర్ణాంకం forceit)
{
    ఒకవేళ (కిటికీ_layout_locked(CMD_tabclose))
	తిరిగి;

    trigger_tabclosedpre(curtab);
    curtab->tp_did_tabclosedpre = TRUE;
    tabpage_T *save_curtab = curtab;

    // First close all the windows but the current one.  If that worked then
    // close the last window in this tab, that will close it.
    ఒకవేళ (!ONE_కిటికీ)
	close_others(TRUE, forceit);
    ఒకవేళ (ONE_కిటికీ)
	ex_win_close(forceit, curwin, ఖాళీ);
    ఒకవేళ (curtab == save_curtab)
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	curtab->tp_did_tabclosedpre = FALSE;
#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు
}

/*
 * Close tab page "tp", which is not the current tab page.
 * Note that autocommands may make "tp" invalid.
 * Also takes care of the tab pages line disappearing when closing the
 * last-but-one tab page.
 */
    శూన్య
tabpage_close_other(tabpage_T *tp, పూర్ణాంకం forceit)
{
    పూర్ణాంకం		పూర్తి = 0;
    win_T	*wp;

    ఒకవేళ (కిటికీ_layout_locked(CMD_కొలత))
	తిరిగి;

    trigger_tabclosedpre(tp);
    tp->tp_did_tabclosedpre = TRUE;

    // Limit to 1000 windows, autocommands may add a window while we close
    // one.  OK, so I'm paranoid...
    అయితే (++పూర్తి < 1000)
    {
	wp = tp->tp_firstwin;
	ex_win_close(forceit, wp, tp);

	// Autocommands may delete the tab page under our fingers.
	ఒకవేళ (!valid_tabpage(tp))
	    ఆపు;
	// We may fail to close a window with a modified buffer.
	ఒకవేళ (tp->tp_firstwin == wp)
	{
	    పూర్తి = 1000;
	    ఆపు;
	}
    }
    ఒకవేళ (పూర్తి >= 1000)
    {
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	tp->tp_did_tabclosedpre = FALSE;
	తిరిగి;
    }

    apply_autocmds(EVENT_TABCLOSED, ఖాళీ, ఖాళీ, FALSE, curbuf);
}

/*
 * ":only".
 */
    స్థిర శూన్య
ex_only(exarg_T *eap)
{
    ఒకవేళ (కిటికీ_layout_locked(CMD_only))
	తిరిగి;
#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు
    ఒకవేళ (eap->addr_లెక్క > 0)
    {
	win_T   *wp;
	పూర్ణాంకం	wnr = eap->line2;
	కొరకు (wp = firstwin; --wnr > 0; )
	{
	    ఒకవేళ (wp->w_తదుపరి == ఖాళీ)
		ఆపు;
	    లేకపోతే
		wp = wp->w_తదుపరి;
	}
	win_goto(wp);
    }
    close_others(TRUE, eap->forceit);
}

    స్థిర శూన్య
ex_hide(exarg_T *eap వాడని)
{
    // ":hide" or ":hide | cmd": hide current window
    ఒకవేళ (eap->skip)
	తిరిగి;

    ఒకవేళ (కిటికీ_layout_locked(CMD_hide))
	తిరిగి;
#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు
    ఒకవేళ (eap->addr_లెక్క == 0)
	win_close(curwin, FALSE);	// don't free buffer
    లేకపోతే
    {
	పూర్ణాంకం	winnr = 0;
	win_T	*win;

	FOR_ALL_WINDOWS(win)
	{
	    winnr++;
	    ఒకవేళ (winnr == eap->line2)
		ఆపు;
	}
	ఒకవేళ (win == ఖాళీ)
	    win = lastwin;
	win_close(win, FALSE);
    }
}

/*
 * ":stop" and ":suspend": Suspend Vim.
 */
    శూన్య
ex_stop(exarg_T *eap)
{
    /*
     * Disallow suspending for "rvim".
     */
    ఒకవేళ (check_restricted())
	తిరిగి;

    ఒకవేళ (!eap->forceit)
	autowrite_all();
    apply_autocmds(EVENT_VIMSUSPEND, ఖాళీ, ఖాళీ, FALSE, ఖాళీ);
    windgoto((పూర్ణాంకం)అడ్డువరుసలు - 1, cmdline_నిలువు_off);
    బహిర్గతం_char('\n');
    బహిర్గతం_flush();
    stoptermcap();
    బహిర్గతం_flush();		// needed for SUN to restore xterm buffer
    mch_restore_title(SAVE_RESTORE_BOTH);	// restore window titles
    ui_suspend();		// call machine specific function
    maketitle();
    resettitle();		// force updating the title
    starttermcap();
    scroll_ప్రారంభం();		// scroll screen before redrawing
    redraw_later_clear();
    shell_resized();	// may have resized window
    apply_autocmds(EVENT_VIMRESUME, ఖాళీ, ఖాళీ, FALSE, ఖాళీ);
}

/*
 * ":exit", ":xit" and ":wq": Write file and quit the current window.
 */
    స్థిర శూన్య
ex_exit(exarg_T *eap)
{
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (not_లోపల_vim9(eap, false) == FAIL)
	తిరిగి;
#అయితే_ముగింపు
    ఒకవేళ (cmdwin_type != 0)
    {
	cmdwin_ఫలితం = Ctrl_C;
	తిరిగి;
    }
    // Don't quit while editing the command line.
    ఒకవేళ (పాఠ్యం_locked())
    {
	పాఠ్యం_locked_సందేశం();
	తిరిగి;
    }

    పూర్ణాంకం save_exiting = exiting;
    /*
     * we plan to exit if there is only one relevant window
     */
    ఒకవేళ (check_మరిన్ని(FALSE, eap->forceit) == OK && only_one_కిటికీ())
	exiting = TRUE;

    // Write the buffer for ":wq" or when it was changed.
    // Trigger QuitPre and ExitPre.
    // Check if we can exit now, after autocommands have changed things.
    ఒకవేళ (((eap->cmdidx == CMD_wq || curbuf_Is_మారింది()) && do_write(eap) == FAIL)
	    || before_quit_autocmds(curwin, FALSE, eap->forceit)
	    || check_మరిన్ని(TRUE, eap->forceit) == FAIL
	    || (only_one_కిటికీ() && check_మారింది_any(eap->forceit, FALSE)))
    {
	not_exiting(save_exiting);
    }
    లేకపోతే
    {
	ఒకవేళ (only_one_కిటికీ())	    // quit last window, exit Vim
	    getout(0);
	not_exiting(save_exiting);
#నిర్వచితమైతే FEAT_GUI
	need_mouse_correct = TRUE;
#అయితే_ముగింపు
	// Quit current window, may free the buffer.
	win_close(curwin, !buf_hide(curwin->w_బఫరు));
    }
}

/*
 * ":print", ":list", ":number".
 */
    స్థిర శూన్య
ex_print(exarg_T *eap)
{
    ఒకవేళ (curbuf->b_ml.ml_flags & ML_ఖాళీలు)
	emsg(_(e_ఖాళీలు_బఫరు));
    లేకపోతే
    {
	కొరకు ( ;!got_int; ui_breakcheck())
	{
	    print_పంక్తి(eap->line1,
		    (eap->cmdidx == CMD_సంఖ్య || eap->cmdidx == CMD_pound
						 || (eap->flags & EXFLAG_NR)),
		    eap->cmdidx == CMD_list || (eap->flags & EXFLAG_LIST));
	    ఒకవేళ (++eap->line1 > eap->line2)
		ఆపు;
	    బహిర్గతం_flush();	    // show one line at a time
	}
	setpcmark();
	// put cursor at last line
	curwin->w_cursor.lnum = eap->line2;
	beginline(BL_SOL | BL_FIX);
    }

    ex_no_reprint = TRUE;
}

#నిర్వచితమైతే FEAT_BYTEOFF
    స్థిర శూన్య
ex_goto(exarg_T *eap)
{
    goto_byte(eap->line2);
}
#అయితే_ముగింపు

/*
 * ":shell".
 */
    స్థిర శూన్య
ex_shell(exarg_T *eap వాడని)
{
    do_shell(ఖాళీ, 0);
}

#ఒకవేళ defined(HAVE_DROP_FILE)

స్థిర పూర్ణాంకం drop_busy = FALSE;
స్థిర పూర్ణాంకం drop_filec;
స్థిర char_u **drop_filev = ఖాళీ;
స్థిర పూర్ణాంకం drop_విభజించు;
స్థిర శూన్య (*drop_callback)(శూన్య *);
స్థిర శూన్య *drop_cookie;

    స్థిర శూన్య
handle_drop_అంతర్గత(శూన్య)
{
    exarg_T	ea;
    పూర్ణాంకం		save_సందేశం_scroll = సందేశం_scroll;

    // Setting the argument list may cause screen updates and being called
    // recursively.  Avoid that by setting drop_busy.
    drop_busy = TRUE;

    // Check whether the current buffer is changed. If so, we will need
    // to split the current window or data could be lost.
    // We don't need to check if the 'hidden' option is set, as in this
    // case the buffer won't be lost.
    ఒకవేళ (!buf_hide(curbuf) && !drop_విభజించు)
    {
	++emsg_off;
	drop_విభజించు = check_మారింది(curbuf, CCGD_AW);
	--emsg_off;
    }
    ఒకవేళ (drop_విభజించు)
    {
	ఒకవేళ (win_విభజించు(0, 0) == FAIL)
	    తిరిగి;
	RESET_BINDING(curwin);

	// When splitting the window, create a new alist.  Otherwise the
	// existing one is overwritten.
	alist_unlink(curwin->w_alist);
	alist_new();
    }

    /*
     * Set up the new argument list.
     */
    alist_సమితి(ALIST(curwin), drop_filec, drop_filev, FALSE, ఖాళీ, 0);

    /*
     * Move to the first file.
     */
    // Fake up a minimal "next" command for do_argfile()
    CLEAR_క్షేత్రం(ea);
    ea.cmd = (char_u *)"next";
    do_argfile(&ea, 0);

    // do_ecmd() may set need_start_insertmode, but since we never left Insert
    // mode that is not needed here.
    need_ప్రారంభం_insertmode = FALSE;

    // Restore msg_scroll, otherwise a following command may cause scrolling
    // unexpectedly.  The screen will be redrawn by the caller, thus
    // msg_scroll being set by displaying a message is irrelevant.
    సందేశం_scroll = save_సందేశం_scroll;

    ఒకవేళ (drop_callback != ఖాళీ)
	drop_callback(drop_cookie);

    drop_filev = ఖాళీ;
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
    శూన్య
handle_drop(
    పూర్ణాంకం		filec,		// the number of files dropped
    char_u	**filev,	// the list of files dropped
    పూర్ణాంకం		విభజించు,		// force splitting the window
    శూన్య	(*callback)(శూన్య *), // to be called after setting the argument
				     // list
    శూన్య	*cookie)	// argument for "callback" (allocated)
{
    // Cannot handle recursive drops, finish the pending one.
    ఒకవేళ (drop_busy)
    {
	FreeWild(filec, filev);
	vim_free(cookie);
	తిరిగి;
    }

    // When calling handle_drop() more than once in a row we only use the last
    // one.
    ఒకవేళ (drop_filev != ఖాళీ)
    {
	FreeWild(drop_filec, drop_filev);
	vim_free(drop_cookie);
    }

    drop_filec = filec;
    drop_filev = filev;
    drop_విభజించు = విభజించు;
    drop_callback = callback;
    drop_cookie = cookie;

    // Postpone this when:
    // - editing the command line
    // - not possible to change the current buffer
    // - updating the screen
    // As it may change buffers and window structures that are in use and cause
    // freed memory to be used.
    ఒకవేళ (పాఠ్యం_locked() || curbuf_locked() || updating_screen)
	తిరిగి;

    handle_drop_అంతర్గత();
}

/*
 * To be called when text is unlocked, curbuf is unlocked or updating_screen is
 * reset: Handle a postponed drop.
 */
    శూన్య
handle_any_postponed_drop(శూన్య)
{
    ఒకవేళ (!drop_busy && drop_filev != ఖాళీ
	     && !పాఠ్యం_locked() && !curbuf_locked() && !updating_screen)
	handle_drop_అంతర్గత();
}
#అయితే_ముగింపు

/*
 * ":preserve".
 */
    స్థిర శూన్య
ex_preserve(exarg_T *eap వాడని)
{
    curbuf->b_flags |= BF_PRESERVED;
    ml_preserve(curbuf, TRUE);
}

/*
 * ":recover".
 */
    స్థిర శూన్య
ex_recover(exarg_T *eap)
{
    // Set recoverymode right away to avoid the ATTENTION prompt.
    recoverymode = TRUE;
    ఒకవేళ (!check_మారింది(curbuf, (p_awa ? CCGD_AW : 0)
			     | CCGD_MULTWIN
			     | (eap->forceit ? CCGD_FORCEIT : 0)
			     | CCGD_EXCMD)

	    && (*eap->arg == NUL
			     || setfname(curbuf, eap->arg, ఖాళీ, TRUE) == OK))
	ml_recover(TRUE);
    recoverymode = FALSE;
}

/*
 * Command modifier used in a wrong way.  Also for other commands that can't
 * appear at the toplevel.
 */
    స్థిర శూన్య
ex_wrongmodifier(exarg_T *eap)
{
    eap->errmsg = ex_errmsg(e_invalid_command_str, eap->cmd);
}

#ఒకవేళ defined(FEAT_EVAL)

// callback function for 'findfunc'
స్థిర callback_T ffu_cb;

    స్థిర callback_T *
get_findfunc_callback(శూన్య)
{
    తిరిగి *curbuf->b_p_ffu != NUL ? &curbuf->b_ffu_cb : &ffu_cb;
}

/*
 * Call 'findfunc' to obtain a list of file names.
 */
    స్థిర list_T *
call_findfunc(char_u *pat, పూర్ణాంకం cmdcomplete)
{
    typval_T	వాదనలు[3];
    callback_T	*cb;
    typval_T	rettv;
    పూర్ణాంకం		retval;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    వాదనలు[0].v_type = VAR_STRING;
    వాదనలు[0].vval.v_string = pat;
    వాదనలు[1].v_type = VAR_BOOL;
    వాదనలు[1].vval.v_సంఖ్య = cmdcomplete;
    వాదనలు[2].v_type = VAR_UNKNOWN;

    // Lock the text to prevent weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;

    ctx = get_option_sctx("findfunc");
    ఒకవేళ (ctx != ఖాళీ)
	current_sctx = *ctx;

    cb = get_findfunc_callback();
    retval = call_callback(cb, -1, &rettv, 2, వాదనలు);

    current_sctx = saved_sctx;

    --textlock;

    list_T *retlist = ఖాళీ;

    ఒకవేళ (retval == OK)
    {
	ఒకవేళ (rettv.v_type == VAR_LIST)
	    retlist = list_copy(rettv.vval.v_list, FALSE, FALSE, get_copyID());
	లేకపోతే
	    emsg(_(e_invalid_return_type_from_findfunc));

	clear_tv(&rettv);
    }

    తిరిగి retlist;
}

/*
 * Find file names matching "pat" using 'findfunc' and return it in "files".
 * Used for expanding the :find, :sfind and :tabfind command argument.
 * Returns OK on success and FAIL otherwise.
 */
    పూర్ణాంకం
expand_findfunc(expand_T *xp, char_u *pat, char_u ***files, పూర్ణాంకం *అంకె_Matches)
{
    list_T	*l;
    పూర్ణాంకం		len;

    *అంకె_Matches = 0;
    *files = ఖాళీ;

    l = call_findfunc(pat, VVAL_TRUE);

    ఒకవేళ (l == ఖాళీ)
	తిరిగి FAIL;

    len = list_len(l);
    ఒకవేళ (len == 0)	    // empty List
    {
	list_free(l);
	తిరిగి FAIL;
    }

    expand_process_user_list(l, files, అంకె_Matches, xp);
    list_free(l);

    తిరిగి OK;
}

/*
 * Use 'findfunc' to find file 'findarg'.  The 'count' argument is used to find
 * the n'th matching file.
 */
    స్థిర char_u *
findfunc_కనుగొను_file(char_u *findarg, పూర్ణాంకం findarg_len, పూర్ణాంకం లెక్క)
{
    list_T	*fname_list;
    char_u	*ret_fname = ఖాళీ;
    char_u	cc;
    పూర్ణాంకం		fname_లెక్క;

    cc = findarg[findarg_len];
    findarg[findarg_len] = NUL;

    fname_list = call_findfunc(findarg, VVAL_FALSE);
    fname_లెక్క = list_len(fname_list);

    ఒకవేళ (fname_లెక్క == 0)
	semsg(_(e_cant_కనుగొను_file_str_లోపల_మార్గం), findarg);
    లేకపోతే
    {
	ఒకవేళ (లెక్క > fname_లెక్క)
	    semsg(_(e_no_మరిన్ని_file_str_దొరికింది_లోపల_మార్గం), findarg);
	లేకపోతే
	{
	    listitem_T *li = list_కనుగొను(fname_list, లెక్క - 1);

	    ఒకవేళ (li != ఖాళీ)
	    {
		typval_T *tv = &li->li_tv;

		ఒకవేళ (tv->v_type == VAR_STRING && tv->vval.v_string != ఖాళీ)
		    ret_fname = vim_strsave(tv->vval.v_string);
		లేకపోతే ఒకవేళ (tv->v_type == VAR_DICT && tv->vval.v_dict != ఖాళీ)
		    ret_fname = dict_get_string(tv->vval.v_dict, "word", TRUE);
	    }
	}
    }

    ఒకవేళ (fname_list != ఖాళీ)
	list_free(fname_list);

    findarg[findarg_len] = cc;

    తిరిగి ret_fname;
}

/*
 * Process the 'findfunc' option value.
 * Returns NULL on success and an error message on failure.
 */
    అక్షరం *
did_సమితి_findfunc(optset_T *వాదనలు వాడని)
{
    పూర్ణాంకం	retval;

    ఒకవేళ (వాదనలు->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_సమితి_callback_func(curbuf->b_p_ffu, &curbuf->b_ffu_cb);
    లేకపోతే
    {
	// global option set
	retval = option_సమితి_callback_func(p_ffu, &ffu_cb);
	// when using :set, free the local callback
	ఒకవేళ (!(వాదనలు->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_ffu_cb);
    }

    ఒకవేళ (retval == FAIL)
	తిరిగి e_invalid_వాదం;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u	**varp = (char_u **)వాదనలు->os_varp;
    char_u	*name = get_scriptlocal_funcname(*varp);
    ఒకవేళ (name != ఖాళీ)
    {
	free_string_option(*varp);
	*varp = name;
    }

    తిరిగి ఖాళీ;
}

# ఒకవేళ defined(EXITFREE)
    శూన్య
free_findfunc_option(శూన్య)
{
    free_callback(&ffu_cb);
}
# endif

/*
 * Mark the global 'findfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_findfunc(పూర్ణాంకం copyID వాడని)
{
    పూర్ణాంకం abort = FALSE;
    abort = సమితి_ref_లోపల_callback(&ffu_cb, copyID);
    తిరిగి abort;
}
#అయితే_ముగింపు

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
    శూన్య
ex_splitview(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    char_u	*fname = ఖాళీ;
#నిర్వచితమైతే FEAT_BROWSE
    char_u	dot_మార్గం[] = ".";
    పూర్ణాంకం		save_cmod_flags = cmdmod.cmod_flags;
#అయితే_ముగింపు
    పూర్ణాంకం		use_tab = eap->cmdidx == CMD_tabedit
		       || eap->cmdidx == CMD_tabfind
		       || eap->cmdidx == CMD_tabnew;

    ఒకవేళ (ERROR_IF_ANY_POPUP_కిటికీ)
	తిరిగి;

#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_QUICKFIX
    // A ":split" in the quickfix window works like ":new".  Don't want two
    // quickfix windows.  But it's OK when doing ":tab split".
    ఒకవేళ (bt_quickfix(curbuf) && cmdmod.cmod_tab == 0)
    {
	ఒకవేళ (eap->cmdidx == CMD_విభజించు)
	    eap->cmdidx = CMD_new;
	ఒకవేళ (eap->cmdidx == CMD_vsplit)
	    eap->cmdidx = CMD_vnew;
    }
#అయితే_ముగింపు

    ఒకవేళ (eap->cmdidx == CMD_sfind || eap->cmdidx == CMD_tabfind)
    {
	char_u	*file_గమ్యం_కనుగొను = ఖాళీ;
	అక్షరం	*శోధన_ctx = ఖాళీ;

	ఒకవేళ (*get_findfunc() != NUL)
	{
#నిర్వచితమైతే FEAT_EVAL
	    fname = findfunc_కనుగొను_file(eap->arg, (పూర్ణాంకం)STRLEN(eap->arg),
				       eap->addr_లెక్క > 0 ? eap->line2 : 1);
#అయితే_ముగింపు
	}
	లేకపోతే
	{
	    fname = కనుగొను_file_లోపల_మార్గం(eap->arg, (పూర్ణాంకం)STRLEN(eap->arg),
					      FNAME_MESS, TRUE, curbuf->b_ffname,
					      &file_గమ్యం_కనుగొను, &శోధన_ctx);
	    vim_free(file_గమ్యం_కనుగొను);
	    vim_findfile_cleanup(శోధన_ctx);
	}
	ఒకవేళ (fname == ఖాళీ)
	    వెళ్ళు theend;
	eap->arg = fname;
    }
#నిర్వచితమైతే FEAT_BROWSE
    లేకపోతే ఒకవేళ ((cmdmod.cmod_flags & CMOD_BROWSE)
	    && eap->cmdidx != CMD_vnew
	    && eap->cmdidx != CMD_new)
    {
	ఒకవేళ (
# ifdef FEAT_GUI
	    !gui.లోపల_use &&
# endif
		au_has_సమూహం((char_u *)"FileExplorer"))
	{
	    // No browsing supported but we do have the file explorer:
	    // Edit the directory.
	    ఒకవేళ (*eap->arg == NUL || !mch_isdir(eap->arg))
		eap->arg = dot_మార్గం;
	}
	లేకపోతే
	{
	    fname = do_browse(0, (char_u *)(use_tab
			? _("Edit File in new tab page")
			: _("Edit File in new window")),
					  eap->arg, ఖాళీ, ఖాళీ, ఖాళీ, curbuf);
	    ఒకవేళ (fname == ఖాళీ)
		వెళ్ళు theend;
	    eap->arg = fname;
	}
    }
    cmdmod.cmod_flags &= ~CMOD_BROWSE;	// Don't browse again in do_ecmd().
#అయితే_ముగింపు

    /*
     * Either open new tab page or split the window.
     */
    ఒకవేళ (use_tab)
    {
	ఒకవేళ (win_new_tabpage(cmdmod.cmod_tab != 0 ? cmdmod.cmod_tab
			 : eap->addr_లెక్క == 0 ? 0
					       : (పూర్ణాంకం)eap->line2 + 1) != FAIL)
	{
	    do_exedit(eap, old_curwin);

	    // set the alternate buffer for the window we came from
	    ఒకవేళ (curwin != old_curwin
		    && win_valid(old_curwin)
		    && old_curwin->w_బఫరు != curbuf
		    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		old_curwin->w_alt_fnum = curbuf->b_fnum;
	}
    }
    లేకపోతే ఒకవేళ (win_విభజించు(eap->addr_లెక్క > 0 ? (పూర్ణాంకం)eap->line2 : 0,
				     *eap->cmd == 'v' ? WSP_VERT : 0) != FAIL)
    {
	// Reset 'scrollbind' when editing another file, but keep it when
	// doing ":split" without arguments.
	ఒకవేళ (*eap->arg != NUL)
	    RESET_BINDING(curwin);
	లేకపోతే
	    do_check_scrollbind(FALSE);
	do_exedit(eap, old_curwin);
    }

#నిర్వచితమైతే FEAT_BROWSE
    cmdmod.cmod_flags = save_cmod_flags;
#అయితే_ముగింపు

theend:
    vim_free(fname);
}

/*
 * Open a new tab page.
 */
    శూన్య
tabpage_new(శూన్య)
{
    exarg_T	ea;

    CLEAR_క్షేత్రం(ea);
    ea.cmdidx = CMD_tabnew;
    ea.cmd = (char_u *)"tabn";
    ea.arg = (char_u *)"";
    ex_splitview(&ea);
}

/*
 * :tabnext command
 */
    స్థిర శూన్య
ex_tabnext(exarg_T *eap)
{
    పూర్ణాంకం tab_సంఖ్య;

    ఒకవేళ (ERROR_IF_POPUP_కిటికీ)
	తిరిగి;
    ఎంపిక (eap->cmdidx)
    {
	స్థితి CMD_tabfirst:
	స్థితి CMD_tabrewind:
	    goto_tabpage(1);
	    ఆపు;
	స్థితి CMD_tablast:
	    goto_tabpage(9999);
	    ఆపు;
	స్థితి CMD_tabprevious:
	స్థితి CMD_tab_తదుపరి:
	    ఒకవేళ (eap->arg && *eap->arg != NUL)
	    {
		char_u *p = eap->arg;
		char_u *p_save = p;

		tab_సంఖ్య = getdigits(&p);
		ఒకవేళ (p == p_save || *p_save == '-' || *p != NUL
			    || tab_సంఖ్య == 0)
		{
		    // No numbers as argument.
		    eap->errmsg = ex_errmsg(e_invalid_వాదం_str, eap->arg);
		    తిరిగి;
		}
	    }
	    లేకపోతే
	    {
		ఒకవేళ (eap->addr_లెక్క == 0)
		    tab_సంఖ్య = 1;
		లేకపోతే
		{
		    tab_సంఖ్య = eap->line2;
		    ఒకవేళ (tab_సంఖ్య < 1)
		    {
			eap->errmsg = _(e_invalid_range);
			తిరిగి;
		    }
		}
	    }
	    goto_tabpage(-tab_సంఖ్య);
	    ఆపు;
	అప్రమేయ: // CMD_tabnext
	    tab_సంఖ్య = get_tabpage_arg(eap);
	    ఒకవేళ (eap->errmsg == ఖాళీ)
		goto_tabpage(tab_సంఖ్య);
	    ఆపు;
    }
}

/*
 * :tabmove command
 */
    స్థిర శూన్య
ex_tabmove(exarg_T *eap)
{
    పూర్ణాంకం tab_సంఖ్య;

    tab_సంఖ్య = get_tabpage_arg(eap);
    ఒకవేళ (eap->errmsg == ఖాళీ)
	tabpage_move(tab_సంఖ్య);
}

/*
 * :tabs command: List tabs and their contents.
 */
    స్థిర శూన్య
ex_tabs(exarg_T *eap వాడని)
{
    tabpage_T	*tp;
    win_T	*wp;
    పూర్ణాంకం		tabcount = 1;

    సందేశం_ప్రారంభం();
    సందేశం_scroll = TRUE;
    కొరకు (tp = మొదటి_tabpage; tp != ఖాళీ && !got_int; tp = tp->tp_తదుపరి)
    {
	సందేశం_putchar('\n');
	vim_snprintf((అక్షరం *)IObuff, IOSIZE, _("Tab page %d"), tabcount++);
	సందేశం_outtrans_attr(IObuff, HL_ATTR(HLF_T));
	బహిర్గతం_flush();	    // output one line at a time
	ui_breakcheck();

	ఒకవేళ (tp  == curtab)
	    wp = firstwin;
	లేకపోతే
	    wp = tp->tp_firstwin;
	కొరకు ( ; wp != ఖాళీ && !got_int; wp = wp->w_తదుపరి)
	{
	    సందేశం_putchar('\n');
	    సందేశం_putchar(wp == curwin ? '>' : ' ');
	    సందేశం_putchar(' ');
	    సందేశం_putchar(buf_Is_మారింది(wp->w_బఫరు) ? '+' : ' ');
	    సందేశం_putchar(' ');
	    ఒకవేళ (buf_spname(wp->w_బఫరు) != ఖాళీ)
		vim_strncpy(IObuff, buf_spname(wp->w_బఫరు), IOSIZE - 1);
	    లేకపోతే
		home_replace(wp->w_బఫరు, wp->w_బఫరు->b_fname,
							IObuff, IOSIZE, TRUE);
	    సందేశం_outtrans(IObuff);
	    బహిర్గతం_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }
}

/*
 * ":mode": Set screen mode.
 * If no argument given, just get the screen size and redraw.
 */
    స్థిర శూన్య
ex_బహుళకం(exarg_T *eap)
{
    ఒకవేళ (*eap->arg == NUL)
	shell_resized();
    లేకపోతే
	emsg(_(e_screen_బహుళకం_setting_not_supported));
}

/*
 * ":resize".
 * set, increment or decrement current window height
 */
    స్థిర శూన్య
ex_resize(exarg_T *eap)
{
    పూర్ణాంకం		n;
    win_T	*wp = curwin;

    ఒకవేళ (eap->addr_లెక్క > 0)
    {
	n = eap->line2;
	కొరకు (wp = firstwin; wp->w_తదుపరి != ఖాళీ && --n > 0; wp = wp->w_తదుపరి)
	    ;
    }

#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు
    n = atol((అక్షరం *)eap->arg);
    ఒకవేళ (cmdmod.cmod_విభజించు & WSP_VERT)
    {
	ఒకవేళ (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_వెడల్పు;
	లేకపోతే ఒకవేళ (n == 0 && eap->arg[0] == NUL)	// default is very wide
	    n = 9999;
	win_setwidth_win(n, wp);
    }
    లేకపోతే
    {
	ఒకవేళ (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_ఎత్తు;
	లేకపోతే ఒకవేళ (n == 0 && eap->arg[0] == NUL)	// default is very high
	    n = 9999;
	win_setheight_win(n, wp);
    }
}

/*
 * ":find [+command] <file>" command.
 */
    స్థిర శూన్య
ex_కనుగొను(exarg_T *eap)
{
    ఒకవేళ (!check_can_సమితి_curbuf_forceit(eap->forceit))
	తిరిగి;

    char_u	*fname = ఖాళీ;
    పూర్ణాంకం		లెక్క;
    char_u	*file_గమ్యం_కనుగొను = ఖాళీ;
    అక్షరం	*శోధన_ctx = ఖాళీ;

    ఒకవేళ (*get_findfunc() != NUL)
    {
#నిర్వచితమైతే FEAT_EVAL
	fname = findfunc_కనుగొను_file(eap->arg, (పూర్ణాంకం)STRLEN(eap->arg),
					eap->addr_లెక్క > 0 ? eap->line2 : 1);
#అయితే_ముగింపు
    }
    లేకపోతే
    {
	fname = కనుగొను_file_లోపల_మార్గం(eap->arg, (పూర్ణాంకం)STRLEN(eap->arg), FNAME_MESS,
			       TRUE, curbuf->b_ffname, &file_గమ్యం_కనుగొను, &శోధన_ctx);
	ఒకవేళ (eap->addr_లెక్క > 0)
	{
	    // Repeat finding the file "count" times.  This matters when it appears
	    // several times in the path.
	    లెక్క = eap->line2;
	    అయితే (fname != ఖాళీ && --లెక్క > 0)
	    {
		vim_free(fname);
		fname = కనుగొను_file_లోపల_మార్గం(ఖాళీ, 0, FNAME_MESS,
			      FALSE, curbuf->b_ffname, &file_గమ్యం_కనుగొను, &శోధన_ctx);
	    }
	}
	VIM_CLEAR(file_గమ్యం_కనుగొను);
	vim_findfile_cleanup(శోధన_ctx);
    }

    ఒకవేళ (fname == ఖాళీ)
	తిరిగి;

    eap->arg = fname;
    do_exedit(eap, ఖాళీ);
    vim_free(fname);
}

/*
 * ":open" simulation: for now works just like ":visual".
 */
    స్థిర శూన్య
ex_open(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*p;

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (not_లోపల_vim9(eap, false) == FAIL)
	తిరిగి;
#అయితే_ముగింపు
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_SOL | BL_FIX);
    ఒకవేళ (*eap->arg == '/')
    {
	// ":open /pattern/": put cursor in column found with pattern
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg, magic_isset() ? RE_MAGIC : 0);
	ఒకవేళ (regmatch.regprog != ఖాళీ)
	{
	    // make a copy of the line, when searching for a mark it might be
	    // flushed
	    char_u *పంక్తి = vim_strsave(ml_get_curline());

	    regmatch.rm_ic = p_ic;
	    ఒకవేళ (vim_regexec(&regmatch, పంక్తి, (colnr_T)0))
		curwin->w_cursor.నిలువు = (colnr_T)(regmatch.startp[0] - పంక్తి);
	    లేకపోతే
		emsg(_(e_no_సరిపోలిక));
	    vim_regfree(regmatch.regprog);
	    vim_free(పంక్తి);
	}
	// Move to the NUL, ignore any other arguments.
	eap->arg += STRLEN(eap->arg);
    }
    check_cursor();

    eap->cmdidx = CMD_visual;
    do_exedit(eap, ఖాళీ);
}

/*
 * ":edit", ":badd", ":balt", ":visual".
 */
    స్థిర శూన్య
ex_edit(exarg_T *eap)
{
    char_u *ffname = eap->cmdidx == CMD_enew ? ఖాళీ : eap->arg;

    // Exclude commands which keep the window's current buffer
    ఒకవేళ (
	    eap->cmdidx != CMD_badd
	    && eap->cmdidx != CMD_balt
	    // All other commands must obey 'winfixbuf' / ! rules
	    && (is_other_file(0, ffname) && !check_can_సమితి_curbuf_forceit(eap->forceit))
    )
	తిరిగి;

    do_exedit(eap, ఖాళీ);
}

/*
 * ":edit <file>" command and alike.
 */
    శూన్య
do_exedit(
    exarg_T	*eap,
    win_T	*old_curwin)	    // curwin before doing a split or NULL
{
    పూర్ణాంకం		n;
    పూర్ణాంకం		need_hide;
    పూర్ణాంకం		exmode_was = exmode_చురుకైన;

    ఒకవేళ ((eap->cmdidx != CMD_pedit && ERROR_IF_POPUP_కిటికీ)
						 || ERROR_IF_పదం_POPUP_కిటికీ)
	తిరిగి;
    /*
     * ":vi" command ends Ex mode.
     */
    ఒకవేళ (exmode_చురుకైన && (eap->cmdidx == CMD_visual
						|| eap->cmdidx == CMD_view))
    {
	exmode_చురుకైన = FALSE;
	ex_pressedreturn = FALSE;
	ఒకవేళ (*eap->arg == NUL)
	{
	    // Special case:  ":global/pat/visual\NLvi-commands"
	    ఒకవేళ (global_busy)
	    {
		ఒకవేళ (eap->nextcmd != ఖాళీ)
		{
		    stuffReadbuff(eap->nextcmd);
		    eap->nextcmd = ఖాళీ;
		}

		ఒకవేళ (exmode_was != EXMODE_VIM)
		    settmode(TMODE_RAW);
		పూర్ణాంకం save_RedrawingDisabled = RedrawingDisabled;
		RedrawingDisabled = 0;
		పూర్ణాంకం save_nwr = no_wait_return;
		no_wait_return = 0;
		need_wait_return = FALSE;
		పూర్ణాంకం save_ms = సందేశం_scroll;
		సందేశం_scroll = 0;
#నిర్వచితమైతే FEAT_GUI
		పూర్ణాంకం save_he = hold_gui_events;
		hold_gui_events = 0;
#అయితే_ముగింపు
		సమితి_must_redraw(UPD_CLEAR);
		pending_exmode_చురుకైన = TRUE;

		main_loop(FALSE, TRUE);

		pending_exmode_చురుకైన = FALSE;
		RedrawingDisabled = save_RedrawingDisabled;
		no_wait_return = save_nwr;
		సందేశం_scroll = save_ms;
#నిర్వచితమైతే FEAT_GUI
		hold_gui_events = save_he;
#అయితే_ముగింపు
	    }
	    తిరిగి;
	}
    }

    ఒకవేళ ((eap->cmdidx == CMD_new
		|| eap->cmdidx == CMD_tabnew
		|| eap->cmdidx == CMD_tabedit
		|| eap->cmdidx == CMD_vnew) && *eap->arg == NUL)
    {
	// ":new" or ":tabnew" without argument: edit a new empty buffer
	setpcmark();
	(శూన్య)do_ecmd(0, ఖాళీ, ఖాళీ, eap, ECMD_ONE,
		      ECMD_HIDE + (eap->forceit ? ECMD_FORCEIT : 0),
		      old_curwin == ఖాళీ ? curwin : ఖాళీ);
    }
    లేకపోతే ఒకవేళ ((eap->cmdidx != CMD_విభజించు && eap->cmdidx != CMD_vsplit)
	    || *eap->arg != NUL
#నిర్వచితమైతే FEAT_BROWSE
	    || (cmdmod.cmod_flags & CMOD_BROWSE)
#అయితే_ముగింపు
	    )
    {
	// Can't edit another file when "textlock" or "curbuf_lock" is set.
	// Only ":edit" or ":script" can bring us here, others are stopped
	// earlier.
	ఒకవేళ (*eap->arg != NUL && పాఠ్యం_or_buf_locked())
	    తిరిగి;

	n = readonlymode;
	ఒకవేళ (eap->cmdidx == CMD_view || eap->cmdidx == CMD_sview)
	    readonlymode = TRUE;
	లేకపోతే ఒకవేళ (eap->cmdidx == CMD_enew)
	    readonlymode = FALSE;   // 'readonly' doesn't make sense in an
				    // empty buffer
	ఒకవేళ (eap->cmdidx != CMD_balt && eap->cmdidx != CMD_badd)
	    setpcmark();
	ఒకవేళ (do_ecmd(0, (eap->cmdidx == CMD_enew ? ఖాళీ : eap->arg),
		    ఖాళీ, eap,
		    // ":edit" goes to first line if Vi compatible
		    (*eap->arg == NUL && eap->do_ecmd_lnum == 0
				      && vim_strchr(p_cpo, CPO_GOTO1) != ఖాళీ)
					       ? ECMD_ONE : eap->do_ecmd_lnum,
		    (buf_hide(curbuf) ? ECMD_HIDE : 0)
		    + (eap->forceit ? ECMD_FORCEIT : 0)
		      // after a split we can use an existing buffer
		    + (old_curwin != ఖాళీ ? ECMD_OLDBUF : 0)
		    + (eap->cmdidx == CMD_badd ? ECMD_ADDBUF : 0)
		    + (eap->cmdidx == CMD_balt ? ECMD_ALTBUF : 0)
		    , old_curwin == ఖాళీ ? curwin : ఖాళీ) == FAIL)
	{
	    // Editing the file failed.  If the window was split, close it.
	    ఒకవేళ (old_curwin != ఖాళీ)
	    {
		need_hide = (curbuf_Is_మారింది() && curbuf->b_nwindows <= 1);
		ఒకవేళ (!need_hide || buf_hide(curbuf))
		{
#ఒకవేళ defined(FEAT_EVAL)
		    cleanup_T   cs;

		    // Reset the error/interrupt/exception state here so that
		    // aborting() returns FALSE when closing a window.
		    enter_cleanup(&cs);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
		    need_mouse_correct = TRUE;
#అయితే_ముగింపు
		    win_close(curwin, !need_hide && !buf_hide(curbuf));

#ఒకవేళ defined(FEAT_EVAL)
		    // Restore the error/interrupt/exception state if not
		    // discarded by a new aborting error, interrupt, or
		    // uncaught exception.
		    leave_cleanup(&cs);
#అయితే_ముగింపు
		}
	    }
	}
	లేకపోతే ఒకవేళ (readonlymode && curbuf->b_nwindows == 1)
	{
	    // When editing an already visited buffer, 'readonly' won't be set
	    // but the previous value is kept.  With ":view" and ":sview" we
	    // want the  file to be readonly, except when another window is
	    // editing the same buffer.
	    curbuf->b_p_ro = TRUE;
	}
	readonlymode = n;
    }
    లేకపోతే
    {
	ఒకవేళ (eap->do_ecmd_cmd != ఖాళీ)
	    do_cmd_వాదం(eap->do_ecmd_cmd);
	n = curwin->w_arg_idx_invalid;
	check_arg_idx(curwin);
	ఒకవేళ (n != curwin->w_arg_idx_invalid)
	    maketitle();
    }

    /*
     * if ":split file" worked, set alternate file name in old window to new
     * file
     */
    ఒకవేళ (old_curwin != ఖాళీ
	    && *eap->arg != NUL
	    && curwin != old_curwin
	    && win_valid(old_curwin)
	    && old_curwin->w_బఫరు != curbuf
	    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	old_curwin->w_alt_fnum = curbuf->b_fnum;

    ex_no_reprint = TRUE;
}

#నిర్వచితం_కాకపోతే FEAT_GUI
/*
 * ":gui" and ":gvim" when there is no GUI.
 */
    స్థిర శూన్య
ex_nogui(exarg_T *eap)
{
    eap->errmsg = _(e_gui_cannot_be_వాడిన_not_enabled_at_compile_సమయం);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
    స్థిర శూన్య
ex_tearoff(exarg_T *eap)
{
    gui_make_tearoff(eap->arg);
}
#అయితే_ముగింపు

#ఒకవేళ (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_పదం_POPUP_MENU)) && defined(FEAT_MENU)
    స్థిర శూన్య
ex_popup(exarg_T *eap)
{
# ఒకవేళ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    ఒకవేళ (gui.లోపల_use)
	gui_make_popup(eap->arg, eap->forceit);
#  ifdef FEAT_పదం_POPUP_MENU
    లేకపోతే
#  endif
# endif
# ifdef FEAT_పదం_POPUP_MENU
	pum_make_popup(eap->arg, eap->forceit);
# endif
}
#అయితే_ముగింపు

    స్థిర శూన్య
ex_swapname(exarg_T *eap వాడని)
{
    ఒకవేళ (curbuf->b_ml.ml_mfp == ఖాళీ || curbuf->b_ml.ml_mfp->mf_fname == ఖాళీ)
	సందేశం(_("No swap file"));
    లేకపోతే
	సందేశం((అక్షరం *)curbuf->b_ml.ml_mfp->mf_fname);
}

/*
 * ":syncbind" forces all 'scrollbind' windows to have the same relative
 * offset.
 * (1998-11-02 16:21:01  R. Edward Ralston <eralston@computer.org>)
 */
    స్థిర శూన్య
ex_syncbind(exarg_T *eap వాడని)
{
    win_T	*wp;
    win_T	*save_curwin = curwin;
    buf_T	*save_curbuf = curbuf;
    దీర్ఘ	topline;
    దీర్ఘ	y;
    linenr_T	old_linenr = curwin->w_cursor.lnum;

    setpcmark();

    /*
     * determine max topline
     */
    ఒకవేళ (curwin->w_p_scb)
    {
	topline = curwin->w_topline;
	FOR_ALL_WINDOWS(wp)
	{
	    ఒకవేళ (wp->w_p_scb && wp->w_బఫరు)
	    {
		y = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క - get_scrolloff_విలువ();
		ఒకవేళ (topline > y)
		    topline = y;
	    }
	}
	ఒకవేళ (topline < 1)
	    topline = 1;
    }
    లేకపోతే
    {
	topline = 1;
    }


    /*
     * Set all scrollbind windows to the same topline.
     */
    FOR_ALL_WINDOWS(curwin)
    {
	ఒకవేళ (curwin->w_p_scb)
	{
	    curbuf = curwin->w_బఫరు;
	    y = topline - curwin->w_topline;
	    ఒకవేళ (y > 0)
		scrollup(y, TRUE);
	    లేకపోతే
		scrolldown(-y, TRUE);
	    curwin->w_scbind_pos = topline;
	    redraw_later(UPD_VALID);
	    cursor_correct();
	    curwin->w_redr_స్థితి = true;
	}
    }
    curwin = save_curwin;
    curbuf = save_curbuf;
    ఒకవేళ (curwin->w_p_scb)
    {
	did_syncbind = TRUE;
	checkpcmark();
	ఒకవేళ (old_linenr != curwin->w_cursor.lnum)
	{
	    char_u ctrl_o[2];

	    ctrl_o[0] = Ctrl_O;
	    ctrl_o[1] = 0;
	    ins_typebuf(ctrl_o, REMAP_NONE, 0, TRUE, FALSE);
	}
    }
}


    స్థిర శూన్య
ex_read(exarg_T *eap)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		ఖాళీలు_2 = (curbuf->b_ml.ml_flags & ML_ఖాళీలు);
    linenr_T	lnum;

    ఒకవేళ (eap->usefilter)			// :r!cmd
    {
	do_bang(1, eap, FALSE, FALSE, TRUE);
	తిరిగి;
    }

    ఒకవేళ (u_save(eap->line2, (linenr_T)(eap->line2 + 1)) == FAIL)
	తిరిగి;

#నిర్వచితమైతే FEAT_BROWSE
    ఒకవేళ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browseFile;

	browseFile = do_browse(0, (char_u *)_("Append File"), eap->arg,
		ఖాళీ, ఖాళీ, ఖాళీ, curbuf);
	ఒకవేళ (browseFile != ఖాళీ)
	{
	    i = readfile(browseFile, ఖాళీ,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	    vim_free(browseFile);
	}
	లేకపోతే
	    i = OK;
    }
    లేకపోతే
#అయితే_ముగింపు
	ఒకవేళ (*eap->arg == NUL)
	{
	    ఒకవేళ (check_fname() == FAIL)	// check for no file name
		తిరిగి;
	    i = readfile(curbuf->b_ffname, curbuf->b_fname,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	}
	లేకపోతే
	{
	    ఒకవేళ (vim_strchr(p_cpo, CPO_ALTREAD) != ఖాళీ)
		(శూన్య)setaltfname(eap->arg, eap->arg, (linenr_T)1);
	    i = readfile(eap->arg, ఖాళీ,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);

	}
    ఒకవేళ (i != OK)
    {
#ఒకవేళ defined(FEAT_EVAL)
	ఒకవేళ (!aborting())
#అయితే_ముగింపు
	    semsg(_(e_cant_open_file_str), eap->arg);
    }
    లేకపోతే
    {
	ఒకవేళ (ఖాళీలు_2 && exmode_చురుకైన)
	{
	    // Delete the empty line that remains.  Historically ex does
	    // this but vi doesn't.
	    ఒకవేళ (eap->line2 == 0)
		lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
	    లేకపోతే
		lnum = 1;
	    ఒకవేళ (*ml_get(lnum) == NUL && u_savedel(lnum, 1L) == OK)
	    {
		ml_delete(lnum);
		ఒకవేళ (curwin->w_cursor.lnum > 1
			&& curwin->w_cursor.lnum >= lnum)
		    --curwin->w_cursor.lnum;
		deleted_పంక్తులు_mark(lnum, 1L);
	    }
	}
	redraw_curbuf_later(UPD_VALID);
    }
}

స్థిర char_u	*prev_dir = ఖాళీ;

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_cd_dir(శూన్య)
{
    VIM_CLEAR(prev_dir);
    VIM_CLEAR(globaldir);
}
#అయితే_ముగింపు

/*
 * Get the previous directory for the given chdir scope.
 */
    స్థిర char_u *
get_prevdir(cdscope_T scope)
{
    ఒకవేళ (scope == CDSCOPE_కిటికీ)
	తిరిగి curwin->w_prevdir;
    లేకపోతే ఒకవేళ (scope == CDSCOPE_TABPAGE)
	తిరిగి curtab->tp_prevdir;
    తిరిగి prev_dir;
}

/*
 * Deal with the side effects of changing the current directory.
 * When 'scope' is CDSCOPE_TABPAGE then this was after an ":tcd" command.
 * When 'scope' is CDSCOPE_WINDOW then this was after an ":lcd" command.
 */
    శూన్య
post_chdir(cdscope_T scope)
{
    ఒకవేళ (scope != CDSCOPE_కిటికీ)
	// Clear tab local directory for both :cd and :tcd
	VIM_CLEAR(curtab->tp_localdir);
    VIM_CLEAR(curwin->w_localdir);
    ఒకవేళ (scope != CDSCOPE_GLOBAL)
    {
	char_u	*pdir = get_prevdir(scope);

	// If still in the global directory, need to remember current
	// directory as the global directory.
	ఒకవేళ (globaldir == ఖాళీ && pdir != ఖాళీ)
	    globaldir = vim_strsave(pdir);

	// Remember this local directory for the window.
	ఒకవేళ (mch_dirname(NameBuff, MAXPATHL) == OK)
	{
	    ఒకవేళ (scope == CDSCOPE_TABPAGE)
		curtab->tp_localdir = vim_strsave(NameBuff);
	    లేకపోతే
		curwin->w_localdir = vim_strsave(NameBuff);
	}
    }
    లేకపోతే
    {
	// We are now in the global directory, no need to remember its name.
	VIM_CLEAR(globaldir);
    }

    చివరి_chdir_reason = ఖాళీ;
    shorten_fnames(vim_strchr(p_cpo, CPO_NOSYMLINKS) == ఖాళీ);
}

/*
 * Trigger DirChangedPre for "acmd_fname" with directory "new_dir".
 */
    శూన్య
trigger_Dir_మారింది_Pre(char_u *acmd_fname, char_u *new_dir)
{
#నిర్వచితమైతే FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;

    v_event = get_v_event(&save_v_event);
    (శూన్య)dict_కూడు_string(v_event, "directory", new_dir);
    dict_సమితి_వస్తువులు_ro(v_event);
#అయితే_ముగింపు
    apply_autocmds(EVENT_DIRCHANGEDPRE, acmd_fname, new_dir, FALSE, curbuf);
#నిర్వచితమైతే FEAT_EVAL
    restore_v_event(v_event, &save_v_event);
#అయితే_ముగింపు
}

/*
 * Change directory function used by :cd/:tcd/:lcd Ex commands and the
 * chdir() function.
 * scope == CDSCOPE_WINDOW: changes the window-local directory
 * scope == CDSCOPE_TABPAGE: changes the tab-local directory
 * Otherwise: changes the global directory
 * Returns TRUE if the directory is successfully changed.
 */
    పూర్ణాంకం
changedir_func(
	char_u		*new_dir,
	పూర్ణాంకం		forceit,
	cdscope_T	scope)
{
    char_u	*pdir = ఖాళీ;
    పూర్ణాంకం		dir_differs;
    char_u	*acmd_fname = ఖాళీ;
    char_u	**pp;
    char_u	*tofree;

    ఒకవేళ (new_dir == ఖాళీ || allbuf_locked())
	తిరిగి FALSE;

    ఒకవేళ (vim_strchr(p_cpo, CPO_CHDIR) != ఖాళీ && curbuf_Is_మారింది() && !forceit)
    {
	emsg(_(e_cannot_change_directory_బఫరు_is_modified_కూడు_bang_గమ్యం_override));
	తిరిగి FALSE;
    }

    // ":cd -": Change to previous directory
    ఒకవేళ (STRCMP(new_dir, "-") == 0)
    {
	pdir = get_prevdir(scope);
	ఒకవేళ (pdir == ఖాళీ)
	{
	    emsg(_(e_no_మునుపటి_directory));
	    తిరిగి FALSE;
	}
	new_dir = pdir;
    }

    // Save current directory for next ":cd -"
    ఒకవేళ (mch_dirname(NameBuff, MAXPATHL) == OK)
	pdir = vim_strsave(NameBuff);
    లేకపోతే
	pdir = ఖాళీ;

    // For UNIX ":cd" means: go to home directory.
    // On other systems too if 'cdhome' is set.
#ఒకవేళ defined(UNIX) || defined(VMS)
    ఒకవేళ (*new_dir == NUL)
#లేకపోతే
    ఒకవేళ (*new_dir == NUL && p_cdh)
#అయితే_ముగింపు
    {
	// use NameBuff for home directory name
#నిర్వచితమైతే VMS
	char_u	*p;

	p = mch_getenv((char_u *)"SYS$LOGIN");
	ఒకవేళ (p == ఖాళీ || *p == NUL)	// empty is the same as not set
	    NameBuff[0] = NUL;
	లేకపోతే
	    vim_strncpy(NameBuff, p, MAXPATHL - 1);
#లేకపోతే
	expand_env((char_u *)"$HOME", NameBuff, MAXPATHL);
#అయితే_ముగింపు
	new_dir = NameBuff;
    }
    dir_differs = pdir == ఖాళీ
			    || pathcmp((అక్షరం *)pdir, (అక్షరం *)new_dir, -1) != 0;
    ఒకవేళ (dir_differs)
    {
	ఒకవేళ (scope == CDSCOPE_కిటికీ)
	    acmd_fname = (char_u *)"window";
	లేకపోతే ఒకవేళ (scope == CDSCOPE_TABPAGE)
	    acmd_fname = (char_u *)"tabpage";
	లేకపోతే
	    acmd_fname = (char_u *)"global";
	trigger_Dir_మారింది_Pre(acmd_fname, new_dir);

	ఒకవేళ (vim_chdir(new_dir))
	{
	    emsg(_(e_command_failed));
	    vim_free(pdir);
	    తిరిగి FALSE;
	}
    }

    ఒకవేళ (scope == CDSCOPE_కిటికీ)
	pp = &curwin->w_prevdir;
    లేకపోతే ఒకవేళ (scope == CDSCOPE_TABPAGE)
	pp = &curtab->tp_prevdir;
    లేకపోతే
	pp = &prev_dir;
    tofree = *pp;  // new_dir may use this
    *pp = pdir;

    post_chdir(scope);

    ఒకవేళ (dir_differs)
	apply_autocmds(EVENT_DIRCHANGED, acmd_fname, new_dir, FALSE, curbuf);
    vim_free(tofree);
    తిరిగి TRUE;
}

/*
 * ":cd", ":tcd", ":lcd", ":chdir" ":tchdir" and ":lchdir".
 */
    శూన్య
ex_cd(exarg_T *eap)
{
    char_u	*new_dir;

    new_dir = eap->arg;
#ఒకవేళ !defined(UNIX) && !defined(VMS)
    // for non-UNIX ":cd" means: print current directory unless 'cdhome' is set
    ఒకవేళ (*new_dir == NUL && !p_cdh)
    {
	ex_pwd(ఖాళీ);
	తిరిగి;
    }
#అయితే_ముగింపు

    cdscope_T	scope = CDSCOPE_GLOBAL;

    ఒకవేళ (eap->cmdidx == CMD_lcd || eap->cmdidx == CMD_lchdir)
	scope = CDSCOPE_కిటికీ;
    లేకపోతే ఒకవేళ (eap->cmdidx == CMD_tcd || eap->cmdidx == CMD_tchdir)
	scope = CDSCOPE_TABPAGE;

    ఒకవేళ (changedir_func(new_dir, eap->forceit, scope))
    {
	// Echo the new current directory if the command was typed.
	ఒకవేళ (కీలకం_Typed || p_verbose >= 5)
	    ex_pwd(eap);
    }
}

/*
 * ":pwd".
 */
    స్థిర శూన్య
ex_pwd(exarg_T *eap వాడని)
{
    ఒకవేళ (mch_dirname(NameBuff, MAXPATHL) == OK)
    {
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
	slash_adjust(NameBuff);
#అయితే_ముగింపు
	ఒకవేళ (p_verbose > 0)
	{
	    అక్షరం *context = "global";

	    ఒకవేళ (చివరి_chdir_reason != ఖాళీ)
		context = చివరి_chdir_reason;
	    లేకపోతే ఒకవేళ (curwin->w_localdir != ఖాళీ)
		context = "window";
	    లేకపోతే ఒకవేళ (curtab->tp_localdir != ఖాళీ)
		context = "tabpage";
	    smsg("[%s] %s", context, (అక్షరం *)NameBuff);
	}
	లేకపోతే
	    సందేశం((అక్షరం *)NameBuff);
    }
    లేకపోతే
	emsg(_(e_directory_unknown));
}

/*
 * ":=".
 */
    స్థిర శూన్య
ex_equal(exarg_T *eap)
{
    smsg("%ld", (దీర్ఘ)eap->line2);
    ex_may_print(eap);
}

    స్థిర శూన్య
ex_sleep(exarg_T *eap)
{
    పూర్ణాంకం		n;
    దీర్ఘ	len;

    ఒకవేళ (cursor_valid())
    {
	n = W_WINROW(curwin) + curwin->w_wrow - సందేశం_scrolled;
	ఒకవేళ (n >= 0)
	    windgoto(n, curwin->w_wincol + curwin->w_wcol);
    }

    len = eap->line2;
    ఎంపిక (*eap->arg)
    {
	స్థితి 'm': ఆపు;
	స్థితి NUL: len *= 1000L; ఆపు;
	అప్రమేయ: semsg(_(e_invalid_వాదం_str), eap->arg); తిరిగి;
    }

    // Hide the cursor if invoked with !
    do_sleep(len, eap->forceit);
}

/*
 * Sleep for "msec" milliseconds, but keep checking for a CTRL-C every second.
 * Hide the cursor if "hide_cursor" is TRUE.
 */
    శూన్య
do_sleep(దీర్ఘ msec, పూర్ణాంకం hide_cursor)
{
    దీర్ఘ	పూర్తి = 0;
    దీర్ఘ	wait_now;
#నిర్వచితమైతే ELAPSED_FUNC
    elapsed_T	ప్రారంభం_tv;

    // Remember at what time we started, so that we know how much longer we
    // should wait after waiting for a bit.
    ELAPSED_INIT(ప్రారంభం_tv);
#అయితే_ముగింపు

    ఒకవేళ (hide_cursor)
	cursor_sleep();
    లేకపోతే
	cursor_on();

    బహిర్గతం_flush_cursor(FALSE, FALSE);
    అయితే (!got_int && పూర్తి < msec)
    {
	wait_now = msec - పూర్తి > 1000L ? 1000L : msec - పూర్తి;
#నిర్వచితమైతే FEAT_TIMERS
	{
	    దీర్ఘ    due_సమయం = check_due_timer();

	    ఒకవేళ (due_సమయం > 0 && due_సమయం < wait_now)
		wait_now = due_సమయం;
	}
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_పని_CHANNEL
	ఒకవేళ (has_any_channel() && wait_now > 20L)
	    wait_now = 20L;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SOUND
	ఒకవేళ (has_any_sound_callback() && wait_now > 20L)
	    wait_now = 20L;
#అయితే_ముగింపు
	ui_delay(wait_now, TRUE);

#నిర్వచితమైతే FEAT_పని_CHANNEL
	ఒకవేళ (has_any_channel())
	    ui_breakcheck_force(TRUE);
	లేకపోతే
#అయితే_ముగింపు
	    ui_breakcheck();
#నిర్వచితమైతే సందేశం_వరుస
	// Process the netbeans and clientserver messages that may have been
	// received in the call to ui_breakcheck() when the GUI is in use. This
	// may occur when running a test case.
	parse_queued_messages();
#అయితే_ముగింపు

#నిర్వచితమైతే ELAPSED_FUNC
	// actual time passed
	పూర్తి = ELAPSED_FUNC(ప్రారంభం_tv);
#లేకపోతే
	// guesstimate time passed (will actually be more)
	పూర్తి += wait_now;
#అయితే_ముగింపు
    }

    // If CTRL-C was typed to interrupt the sleep, drop the CTRL-C from the
    // input buffer, otherwise a following call to input() fails.
    ఒకవేళ (got_int)
	(శూన్య)vpeekc();

    ఒకవేళ (hide_cursor)
	cursor_unsleep();
}

/*
 * ":winsize" command (obsolete).
 */
    స్థిర శూన్య
ex_winsize(exarg_T *eap)
{
    పూర్ణాంకం		w, h;
    char_u	*arg = eap->arg;
    char_u	*p;

    ఒకవేళ (!సురక్షిత_isdigit(*arg))
    {
	semsg(_(e_invalid_వాదం_str), arg);
	తిరిగి;
    }
    w = getdigits(&arg);
    arg = skipwhite(arg);
    p = arg;
    h = getdigits(&arg);
    ఒకవేళ (*p != NUL && *arg == NUL)
	సమితి_shellsize(w, h, TRUE);
    లేకపోతే
	emsg(_(e_winsize_requires_two_సంఖ్య_arguments));
}

    స్థిర శూన్య
ex_wincmd(exarg_T *eap)
{
    పూర్ణాంకం		xchar = NUL;
    char_u	*p;

    ఒకవేళ (*eap->arg == 'g' || *eap->arg == Ctrl_G)
    {
	// CTRL-W g and CTRL-W CTRL-G  have an extra command character
	ఒకవేళ (eap->arg[1] == NUL)
	{
	    emsg(_(e_invalid_వాదం));
	    తిరిగి;
	}
	xchar = eap->arg[1];
	p = eap->arg + 2;
    }
    లేకపోతే
	p = eap->arg + 1;

    సమితి_nextcmd(eap, p);
    p = skipwhite(p);
    ఒకవేళ (*p != NUL && *p != (
#నిర్వచితమైతే FEAT_EVAL
	    లోపల_vim9script() ? '#' :
#అయితే_ముగింపు
		'"')
	    && eap->nextcmd == ఖాళీ)
	emsg(_(e_invalid_వాదం));
    లేకపోతే ఒకవేళ (!eap->skip)
    {
	// Pass flags on for ":vertical wincmd ]".
	postponed_విభజించు_flags = cmdmod.cmod_విభజించు;
	postponed_విభజించు_tab = cmdmod.cmod_tab;
	do_కిటికీ(*eap->arg, eap->addr_లెక్క > 0 ? eap->line2 : 0L, xchar);
	postponed_విభజించు_flags = 0;
	postponed_విభజించు_tab = 0;
    }
}

#ఒకవేళ defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * ":winpos".
 */
    స్థిర శూన్య
ex_winpos(exarg_T *eap)
{
    పూర్ణాంకం		x, y;
    char_u	*arg = eap->arg;
    char_u	*p;

    ఒకవేళ (*arg == NUL)
    {
# ఒకవేళ defined(FEAT_GUI) || defined(MSWIN)
#  ifdef VIMDLL
	ఒకవేళ (gui.లోపల_use ? gui_mch_get_winpos(&x, &y) != FAIL :
		mch_get_winpos(&x, &y) != FAIL)
#  elif defined(FEAT_GUI)
	ఒకవేళ (gui.లోపల_use && gui_mch_get_winpos(&x, &y) != FAIL)
#  లేకపోతే
	ఒకవేళ (mch_get_winpos(&x, &y) != FAIL)
#  endif
	{
	    sprintf((అక్షరం *)IObuff, _("Window position: X %d, Y %d"), x, y);
	    సందేశం((అక్షరం *)IObuff);
	}
	లేకపోతే
# endif
	    emsg(_(e_obtaining_కిటికీ_position_not_implemented_for_this_platform));
    }
    లేకపోతే
    {
	x = getdigits(&arg);
	arg = skipwhite(arg);
	p = arg;
	y = getdigits(&arg);
	ఒకవేళ (*p == NUL || *arg != NUL)
	{
	    emsg(_(e_winpos_requires_two_సంఖ్య_arguments));
	    తిరిగి;
	}
# ifdef FEAT_GUI
	ఒకవేళ (gui.లోపల_use)
	    gui_mch_సమితి_winpos(x, y);
	లేకపోతే ఒకవేళ (gui.starting)
	{
	    // Remember the coordinates for when the window is opened.
	    gui_win_x = x;
	    gui_win_y = y;
	}
#  ఒకవేళ defined(HAVE_TGETENT) || defined(VIMDLL)
	లేకపోతే
#  endif
# endif
# ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	    mch_సమితి_winpos(x, y);
# endif
# ifdef HAVE_TGETENT
	ఒకవేళ (*T_CWP)
	    పదం_సమితి_winpos(x, y);
# endif
    }
}
#అయితే_ముగింపు

/*
 * Handle command that work like operators: ":delete", ":yank", ":>" and ":<".
 */
    స్థిర శూన్య
ex_operators(exarg_T *eap)
{
    oparg_T	oa;

    clear_oparg(&oa);
    oa.regname = eap->regname;
    oa.ప్రారంభం.lnum = eap->line1;
    oa.చివర.lnum = eap->line2;
    oa.పంక్తి_లెక్క = eap->line2 - eap->line1 + 1;
    oa.motion_type = MLINE;
    virtual_op = FALSE;
    ఒకవేళ (eap->cmdidx != CMD_yank)	// position cursor for undo
    {
	setpcmark();
	curwin->w_cursor.lnum = eap->line1;
	beginline(BL_SOL | BL_FIX);
    }

    ఒకవేళ (VIsual_చురుకైన)
	చివర_visual_బహుళకం();

    ఎంపిక (eap->cmdidx)
    {
	స్థితి CMD_delete:
	    oa.op_type = OP_DELETE;
	    op_delete(&oa);
	    ఆపు;

	స్థితి CMD_yank:
	    oa.op_type = OP_YANK;
	    (శూన్య)op_yank(&oa, FALSE, TRUE);
	    ఆపు;

	అప్రమేయ:    // CMD_rshift or CMD_lshift
	    ఒకవేళ (
#నిర్వచితమైతే FEAT_RIGHTLEFT
		(eap->cmdidx == CMD_rshift) ^ curwin->w_p_rl
#లేకపోతే
		eap->cmdidx == CMD_rshift
#అయితే_ముగింపు
						)
		oa.op_type = OP_RSHIFT;
	    లేకపోతే
		oa.op_type = OP_LSHIFT;
	    op_జరుపు(&oa, FALSE, eap->amount);
	    ఆపు;
    }
    virtual_op = MAYBE;
    ex_may_print(eap);
}

/*
 * ":put".
 */
    స్థిర శూన్య
ex_put(exarg_T *eap)
{
    // ":0put" works like ":1put!".
    ఒకవేళ (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_నిలువు();
    do_put(eap->regname, ఖాళీ, eap->forceit ? BACKWARD : FORWARD, 1L,
						       PUT_పంక్తి|PUT_CURSLINE);
}

/*
 * ":iput".
 */
    స్థిర శూన్య
ex_iput(exarg_T *eap)
{
    // ":0iput" works like ":1iput!".
    ఒకవేళ (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_నిలువు();
    do_put(eap->regname, ఖాళీ, eap->forceit ? BACKWARD : FORWARD, 1L,
						      PUT_పంక్తి|PUT_CURSLINE
						      |PUT_FIXINDENT);
}

/*
 * Handle ":copy" and ":move".
 */
    స్థిర శూన్య
ex_copymove(exarg_T *eap)
{
    దీర్ఘ	n;

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (not_లోపల_vim9(eap, false) == FAIL)
	తిరిగి;
#అయితే_ముగింపు
    n = get_చిరునామా(eap, &eap->arg, eap->addr_type, FALSE, FALSE, FALSE, 1);
    ఒకవేళ (eap->arg == ఖాళీ)	    // error detected
    {
	eap->nextcmd = ఖాళీ;
	తిరిగి;
    }
    get_flags(eap);

    /*
     * move or copy lines from 'eap->line1'-'eap->line2' to below line 'n'
     */
    ఒకవేళ (n == MAXLNUM || n < 0 || n > curbuf->b_ml.ml_పంక్తి_లెక్క)
    {
	emsg(_(e_invalid_range));
	తిరిగి;
    }

    ఒకవేళ (eap->cmdidx == CMD_move)
    {
	ఒకవేళ (do_move(eap->line1, eap->line2, n) == FAIL)
	    తిరిగి;
    }
    లేకపోతే
	ex_copy(eap->line1, eap->line2, n);
    u_clearline();
    beginline(BL_SOL | BL_FIX);
    ex_may_print(eap);
}

/*
 * Print the current line if flags were given to the Ex command.
 */
    శూన్య
ex_may_print(exarg_T *eap)
{
    ఒకవేళ (eap->flags != 0)
    {
	print_పంక్తి(curwin->w_cursor.lnum, (eap->flags & EXFLAG_NR),
						  (eap->flags & EXFLAG_LIST));
	ex_no_reprint = TRUE;
    }
}

/*
 * ":smagic" and ":snomagic".
 */
    స్థిర శూన్య
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
    స్థిర శూన్య
ex_join(exarg_T *eap)
{
    curwin->w_cursor.lnum = eap->line1;
    ఒకవేళ (eap->line1 == eap->line2)
    {
	ఒకవేళ (eap->addr_లెక్క >= 2)   // :2,2join does nothing
	    తిరిగి;
	ఒకవేళ (eap->line2 == curbuf->b_ml.ml_పంక్తి_లెక్క)
	{
	    beep_flush();
	    తిరిగి;
	}
	++eap->line2;
    }
    (శూన్య)do_join(eap->line2 - eap->line1 + 1, !eap->forceit, TRUE, TRUE, TRUE);
    beginline(BL_WHITE | BL_FIX);
    ex_may_print(eap);
}

/*
 * ":[addr]@r" or ":[addr]*r": execute register
 */
    స్థిర శూన్య
ex_at(exarg_T *eap)
{
    పూర్ణాంకం		c;
    పూర్ణాంకం		prev_len = typebuf.tb_len;

    curwin->w_cursor.lnum = eap->line2;
    check_cursor_నిలువు();

#నిర్వచితమైతే USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#అయితే_ముగింపు

    // get the register name.  No name means to use the previous one
    c = *eap->arg;
    ఒకవేళ (c == NUL || (c == '*' && *eap->cmd == '*'))
	c = '@';
    // Put the register in the typeahead buffer with the "silent" flag.
    ఒకవేళ (do_execreg(c, TRUE, vim_strchr(p_cpo, CPO_EXECBUF) != ఖాళీ, TRUE)
								      == FAIL)
    {
	beep_flush();
	తిరిగి;
    }

    పూర్ణాంకం	save_efr = exec_from_reg;

    exec_from_reg = TRUE;

    /*
     * Execute from the typeahead buffer.
     * Continue until the stuff buffer is empty and all added characters
     * have been consumed.
     */
    అయితే (!stuff_ఖాళీలు() || typebuf.tb_len > prev_len)
	(శూన్య)do_cmdline(ఖాళీ, getexline, ఖాళీ, DOCMD_NOWAIT|DOCMD_VERBOSE);

    exec_from_reg = save_efr;
}

/*
 * ":!".
 */
    స్థిర శూన్య
ex_bang(exarg_T *eap)
{
    do_bang(eap->addr_లెక్క, eap, eap->forceit, TRUE, TRUE);
}

/*
 * ":undo".
 */
    స్థిర శూన్య
ex_undo(exarg_T *eap)
{
    ఒకవేళ (eap->addr_లెక్క == 1)	    // :undo 123
	undo_సమయం(eap->line2, FALSE, FALSE, TRUE);
    లేకపోతే
	u_undo(1);
}

#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
    స్థిర శూన్య
ex_wundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_కొలత];

    u_compute_hash(hash);
    u_write_undo(eap->arg, eap->forceit, curbuf, hash);
}

    స్థిర శూన్య
ex_rundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_కొలత];

    u_compute_hash(hash);
    u_read_undo(eap->arg, hash, ఖాళీ);
}
#అయితే_ముగింపు

/*
 * ":redo".
 */
    స్థిర శూన్య
ex_redo(exarg_T *eap వాడని)
{
    u_redo(1);
}

/*
 * ":earlier" and ":later".
 */
    స్థిర శూన్య
ex_later(exarg_T *eap)
{
    దీర్ఘ	లెక్క = 0;
    పూర్ణాంకం		sec = FALSE;
    పూర్ణాంకం		file = FALSE;
    char_u	*p = eap->arg;

    ఒకవేళ (*p == NUL)
	లెక్క = 1;
    లేకపోతే ఒకవేళ (సురక్షిత_isdigit(*p))
    {
	లెక్క = getdigits(&p);
	ఎంపిక (*p)
	{
	    స్థితి 's': ++p; sec = TRUE; ఆపు;
	    స్థితి 'm': ++p; sec = TRUE; లెక్క *= 60; ఆపు;
	    స్థితి 'h': ++p; sec = TRUE; లెక్క *= 60 * 60; ఆపు;
	    స్థితి 'd': ++p; sec = TRUE; లెక్క *= 24 * 60 * 60; ఆపు;
	    స్థితి 'f': ++p; file = TRUE; ఆపు;
	}
    }

    ఒకవేళ (*p != NUL)
	semsg(_(e_invalid_వాదం_str), eap->arg);
    లేకపోతే
	undo_సమయం(eap->cmdidx == CMD_earlier ? -లెక్క : లెక్క,
							    sec, file, FALSE);
}

/*
 * ":redir": start/stop redirection.
 */
    స్థిర శూన్య
ex_redir(exarg_T *eap)
{
    అక్షరం	*బహుళకం;
    char_u	*fname;
    char_u	*arg = eap->arg;

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (redir_execute)
    {
	emsg(_(e_cannot_use_redir_inside_execute));
	తిరిగి;
    }
#అయితే_ముగింపు

    ఒకవేళ (STRICMP(eap->arg, "END") == 0)
	close_redir();
    లేకపోతే
    {
	ఒకవేళ (*arg == '>')
	{
	    ++arg;
	    ఒకవేళ (*arg == '>')
	    {
		++arg;
		బహుళకం = "a";
	    }
	    లేకపోతే
		బహుళకం = "w";
	    arg = skipwhite(arg);

	    close_redir();

	    // Expand environment variables and "~/".
	    fname = expand_env_save(arg);
	    ఒకవేళ (fname == ఖాళీ)
		తిరిగి;
#నిర్వచితమైతే FEAT_BROWSE
	    ఒకవేళ (cmdmod.cmod_flags & CMOD_BROWSE)
	    {
		char_u	*browseFile;

		browseFile = do_browse(BROWSE_SAVE,
			(char_u *)_("Save Redirection"),
			fname, ఖాళీ, ఖాళీ,
			(char_u *)_(BROWSE_FILTER_ALL_FILES), curbuf);
		ఒకవేళ (browseFile == ఖాళీ)
		    తిరిగి;		// operation cancelled
		vim_free(fname);
		fname = browseFile;
		eap->forceit = TRUE;	// since dialog already asked
	    }
#అయితే_ముగింపు

	    redir_fd = open_exfile(fname, eap->forceit, బహుళకం);
	    vim_free(fname);
	}
#నిర్వచితమైతే FEAT_EVAL
	లేకపోతే ఒకవేళ (*arg == '@')
	{
	    // redirect to a register a-z (resp. A-Z for appending)
	    close_redir();
	    ++arg;
	    ఒకవేళ (ASCII_ISALPHA(*arg)
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
		ఒకవేళ (*arg == '>' && arg[1] == '>')  // append
		    arg += 2;
		లేకపోతే
		{
		    // Can use both "@a" and "@a>".
		    ఒకవేళ (*arg == '>')
			arg++;
		    // Make register empty when not using @A-@Z and the
		    // command is valid.
		    ఒకవేళ (*arg == NUL && !సురక్షిత_isupper(redir_reg))
			write_reg_contents(redir_reg, (char_u *)"", -1, FALSE);
		}
	    }
	    ఒకవేళ (*arg != NUL)
	    {
		redir_reg = 0;
		semsg(_(e_invalid_వాదం_str), eap->arg);
	    }
	}
	లేకపోతే ఒకవేళ (*arg == '=' && arg[1] == '>')
	{
	    పూర్ణాంకం append;

	    // redirect to a variable
	    close_redir();
	    arg += 2;

	    ఒకవేళ (*arg == '>')
	    {
		++arg;
		append = TRUE;
	    }
	    లేకపోతే
		append = FALSE;

	    ఒకవేళ (var_redir_ప్రారంభం(skipwhite(arg), append) == OK)
		redir_vname = 1;
	}
#అయితే_ముగింపు

	// TODO: redirect to a buffer

	లేకపోతే
	    semsg(_(e_invalid_వాదం_str), eap->arg);
    }

    // Make sure redirection is not off.  Can happen for cmdline completion
    // that indirectly invokes a command to catch its output.
    ఒకవేళ (redir_fd != ఖాళీ
#నిర్వచితమైతే FEAT_EVAL
			  || redir_reg || redir_vname
#అయితే_ముగింపు
							)
	redir_off = FALSE;
}

/*
 * ":redraw": force redraw, with clear for ":redraw!".
 */
    శూన్య
ex_redraw(exarg_T *eap)
{
    redraw_cmd(eap->forceit);
}

/*
 * ":redraw": force redraw, with clear if "clear" is TRUE.
 */
    శూన్య
redraw_cmd(పూర్ణాంకం clear)
{
    పూర్ణాంకం save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    పూర్ణాంకం save_p_lz = p_lz;
    p_lz = FALSE;

    validate_cursor();
    update_topline();
    update_screen(clear ? UPD_CLEAR : VIsual_చురుకైన ? UPD_INVERTED : 0);
    ఒకవేళ ((State & బహుళకం_CMDLINE) == 0)
	setcursor(); // put cursor back where it belongs
    ఒకవేళ (need_maketitle)
	maketitle();
#ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
    ఒకవేళ (!gui.లోపల_use)
# endif
	resize_console_buf();
#అయితే_ముగింపు
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;

    // After drawing the statusline screen_attr may still be set.
    screen_stop_highlight();

    // Reset msg_didout, so that a message that's there is overwritten.
    సందేశం_didout = FALSE;
    సందేశం_నిలువు = 0;

    // No need to wait after an intentional redraw.
    need_wait_return = FALSE;

    // When invoked from a callback or autocmd the command line may be active.
    ఒకవేళ (State & బహుళకం_CMDLINE)
	redrawcmdline();

    బహిర్గతం_flush();
}

/*
 * ":redrawstatus": force redraw of status line(s)
 */
    స్థిర శూన్య
ex_redrawstatus(exarg_T *eap వాడని)
{
    ఒకవేళ (eap->forceit)
	స్థితి_redraw_all();
    లేకపోతే
	స్థితి_redraw_curbuf();
    redraw_vseps = TRUE;
    ఒకవేళ (సందేశం_scrolled && (State & బహుళకం_CMDLINE))
	తిరిగి;  // redraw later

    పూర్ణాంకం save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    పూర్ణాంకం save_p_lz = p_lz;
    p_lz = FALSE;

    ఒకవేళ (State & బహుళకం_CMDLINE)
	redraw_statuslines();
    లేకపోతే
	update_screen(VIsual_చురుకైన ? UPD_INVERTED : 0);
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    బహిర్గతం_flush();
}

/*
 * ":redrawtabline": force redraw of the tabline
 */
    స్థిర శూన్య
ex_redrawtabline(exarg_T *eap వాడని)
{
    పూర్ణాంకం save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    పూర్ణాంకం save_p_lz = p_lz;
    p_lz = FALSE;

    draw_tabline();

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    బహిర్గతం_flush();
}

/*
 * ":redrawtabpanel": force redraw of the tabpanel
 */
    స్థిర శూన్య
ex_redrawtabpanel(exarg_T *eap వాడని)
{
    పూర్ణాంకం save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    పూర్ణాంకం save_p_lz = p_lz;
    p_lz = FALSE;

#ఒకవేళ defined(FEAT_TABPANEL)
    draw_tabpanel();
#లేకపోతే
    emsg(_(e_cannot_not_support_redrawtabpanel));
#అయితే_ముగింపు

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    బహిర్గతం_flush();
}

    స్థిర శూన్య
close_redir(శూన్య)
{
    ఒకవేళ (redir_fd != ఖాళీ)
    {
	fclose(redir_fd);
	redir_fd = ఖాళీ;
    }
#నిర్వచితమైతే FEAT_EVAL
    redir_reg = 0;
    ఒకవేళ (redir_vname)
    {
	var_redir_stop();
	redir_vname = 0;
    }
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_SESSION) || defined(FEAT_EVAL)
    పూర్ణాంకం
vim_mkdir_emsg(char_u *name, పూర్ణాంకం prot వాడని)
{
    ఒకవేళ (vim_mkdir(name, prot) != 0)
    {
	semsg(_(e_cannot_create_directory_str), name);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}
#అయితే_ముగింపు

/*
 * Open a file for writing for an Ex command, with some checks.
 * Return file descriptor, or NULL on failure.
 */
    FILE *
open_exfile(
    char_u	*fname,
    పూర్ణాంకం		forceit,
    అక్షరం	*బహుళకం)	    // "w" for create new file or "a" for append
{
    FILE	*fd;

#నిర్వచితమైతే UNIX
    // with Unix it is possible to open a directory
    ఒకవేళ (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	తిరిగి ఖాళీ;
    }
#అయితే_ముగింపు
    ఒకవేళ (!forceit && *బహుళకం != 'a' && vim_fexists(fname))
    {
	semsg(_(e_str_exists_కూడు_bang_గమ్యం_override), fname);
	తిరిగి ఖాళీ;
    }

    ఒకవేళ ((fd = mch_fopen((అక్షరం *)fname, బహుళకం)) == ఖాళీ)
	semsg(_(e_cannot_open_str_for_writing_2), fname);

    తిరిగి fd;
}

/*
 * ":mark" and ":k".
 */
    స్థిర శూన్య
ex_mark(exarg_T *eap)
{
    pos_T	pos;

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (not_లోపల_vim9(eap, false) == FAIL)
	తిరిగి;
#అయితే_ముగింపు
    ఒకవేళ (*eap->arg == NUL)		// No argument?
    {
	emsg(_(e_వాదం_required));
	తిరిగి;
    }

    ఒకవేళ (eap->arg[1] != NUL)	// more than one character?
    {
	semsg(_(e_trailing_characters_str), eap->arg);
	తిరిగి;
    }

    pos = curwin->w_cursor;		// save curwin->w_cursor
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_WHITE | BL_FIX);
    ఒకవేళ (setmark(*eap->arg) == FAIL)	// set mark
	emsg(_(e_వాదం_must_be_letter_or_forward_backward_quote));
    curwin->w_cursor = pos;		// restore curwin->w_cursor
}

/*
 * Update w_topline, w_leftcol and the cursor position.
 */
    శూన్య
update_topline_cursor(శూన్య)
{
    check_cursor();		// put cursor on valid line
    update_topline();
    ఒకవేళ (!curwin->w_p_wrap)
	validate_cursor();
    update_curswant();
}

/*
 * Save the current State and go to Normal mode.
 * Return TRUE if the typeahead could be saved.
 */
    పూర్ణాంకం
save_current_state(save_state_T *sst)
{
    sst->save_సందేశం_scroll = సందేశం_scroll;
    sst->save_restart_edit = restart_edit;
    sst->save_సందేశం_didout = సందేశం_didout;
    sst->save_State = State;
    sst->save_insertmode = p_im;
    sst->save_ముగింపు_op = ముగింపు_op;
    sst->save_opcount = opcount;
    sst->save_reg_executing = reg_executing;
    sst->save_pending_చివర_reg_executing = pending_చివర_reg_executing;

    సందేశం_scroll = FALSE;		    // no msg scrolling in Normal mode
    restart_edit = 0;		    // don't go to Insert mode
    p_im = FALSE;		    // don't use 'insertmode'

    sst->save_script_సంస్కరణ = current_sctx.sc_సంస్కరణ;
    current_sctx.sc_సంస్కరణ = 1;    // not in Vim9 script

    /*
     * Save the current typeahead.  This is required to allow using ":normal"
     * from an event handler and makes sure we don't hang when the argument
     * ends with half a command.
     */
    save_typeahead(&sst->tabuf);
    తిరిగి sst->tabuf.typebuf_valid;
}

    శూన్య
restore_current_state(save_state_T *sst)
{
    // Restore the previous typeahead.
    restore_typeahead(&sst->tabuf, FALSE);

    సందేశం_scroll = sst->save_సందేశం_scroll;
    restart_edit = sst->save_restart_edit;
    p_im = sst->save_insertmode;
    ముగింపు_op = sst->save_ముగింపు_op;
    opcount = sst->save_opcount;
    reg_executing = sst->save_reg_executing;
    pending_చివర_reg_executing = sst->save_pending_చివర_reg_executing;
    సందేశం_didout |= sst->save_సందేశం_didout;	// don't reset msg_didout now
    current_sctx.sc_సంస్కరణ = sst->save_script_సంస్కరణ;

    // Restore the state (needed when called from a function executed for
    // 'indentexpr'). Update the mouse and cursor, they may have changed.
    State = sst->save_State;
#నిర్వచితమైతే CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#అయితే_ముగింపు
}

/*
 * ":normal[!] {commands}": Execute normal mode commands.
 */
    శూన్య
ex_సాధారణ(exarg_T *eap)
{
    save_state_T save_state;
    char_u	*arg = ఖాళీ;
    పూర్ణాంకం		l;
    char_u	*p;

    ఒకవేళ (ex_సాధారణ_తాళం > 0)
    {
	emsg(_(e_not_allowed_ఇక్కడ));
	తిరిగి;
    }
    ఒకవేళ (ex_సాధారణ_busy >= p_mmd)
    {
	emsg(_(e_recursive_use_యొక్క_సాధారణ_too_deep));
	తిరిగి;
    }

    /*
     * vgetc() expects a CSI and K_SPECIAL to have been escaped.  Don't do
     * this for the K_SPECIAL leading byte, otherwise special keys will not
     * work.
     */
    ఒకవేళ (has_mbyte)
    {
	పూర్ణాంకం	len = 0;

	// Count the number of characters to be escaped.
	కొరకు (p = eap->arg; *p != NUL; ++p)
	{
#నిర్వచితమైతే FEAT_GUI
	    ఒకవేళ (*p == CSI)  // leadbyte CSI
		len += 2;
#అయితే_ముగింపు
	    కొరకు (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		ఒకవేళ (*++p == K_SPECIAL	  // trailbyte K_SPECIAL or CSI
#నిర్వచితమైతే FEAT_GUI
			|| *p == CSI
#అయితే_ముగింపు
			)
		    len += 2;
	}
	ఒకవేళ (len > 0)
	{
	    arg = alloc(STRLEN(eap->arg) + len + 1);
	    ఒకవేళ (arg != ఖాళీ)
	    {
		len = 0;
		కొరకు (p = eap->arg; *p != NUL; ++p)
		{
		    arg[len++] = *p;
#నిర్వచితమైతే FEAT_GUI
		    ఒకవేళ (*p == CSI)
		    {
			arg[len++] = KS_EXTRA;
			arg[len++] = (పూర్ణాంకం)KE_CSI;
		    }
#అయితే_ముగింపు
		    కొరకు (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		    {
			arg[len++] = *++p;
			ఒకవేళ (*p == K_SPECIAL)
			{
			    arg[len++] = KS_SPECIAL;
			    arg[len++] = KE_FILLER;
			}
#నిర్వచితమైతే FEAT_GUI
			లేకపోతే ఒకవేళ (*p == CSI)
			{
			    arg[len++] = KS_EXTRA;
			    arg[len++] = (పూర్ణాంకం)KE_CSI;
			}
#అయితే_ముగింపు
		    }
		    arg[len] = NUL;
		}
	    }
	}
    }

    ++ex_సాధారణ_busy;
    ఒకవేళ (save_current_state(&save_state))
    {
	/*
	 * Repeat the :normal command for each line in the range.  When no
	 * range given, execute it just once, without positioning the cursor
	 * first.
	 */
	చేయి
	{
	    ఒకవేళ (eap->addr_లెక్క != 0)
	    {
		curwin->w_cursor.lnum = eap->line1++;
		curwin->w_cursor.నిలువు = 0;
		check_cursor_moved(curwin);
	    }

	    exec_సాధారణ_cmd(arg != ఖాళీ
		     ? arg
		     : eap->arg, eap->forceit ? REMAP_NONE : REMAP_YES, FALSE);
	}
	అయితే (eap->addr_లెక్క > 0 && eap->line1 <= eap->line2 && !got_int);
    }

    // Might not return to the main loop when in an event handler.
    update_topline_cursor();

    restore_current_state(&save_state);
    --ex_సాధారణ_busy;
    setmouse();
#నిర్వచితమైతే CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#అయితే_ముగింపు

    vim_free(arg);
}

/*
 * ":startinsert", ":startreplace" and ":startgreplace"
 */
    స్థిర శూన్య
ex_startinsert(exarg_T *eap)
{
#నిర్వచితమైతే FEAT_TERMINAL
    // Ignore this when running in an active terminal.
    ఒకవేళ (పదం_పని_running(curbuf->b_పదం))
	తిరిగి;
#అయితే_ముగింపు
    ఒకవేళ (!curbuf->b_p_ma && !p_im)
    {
	// Only give this error when 'insertmode' is off.
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	తిరిగి;
    }
    ఒకవేళ (eap->forceit)
    {
	// cursor line can be zero on startup
	ఒకవేళ (!curwin->w_cursor.lnum)
	    curwin->w_cursor.lnum = 1;
	సమితి_cursor_for_append_గమ్యం_పంక్తి();
    }

    // Ignore the command when already in Insert mode.  Inserting an
    // expression register that invokes a function can do this.
    ఒకవేళ (State & బహుళకం_చొప్పించు)
	తిరిగి;

    ఒకవేళ (eap->cmdidx == CMD_startinsert)
	restart_edit = 'a';
    లేకపోతే ఒకవేళ (eap->cmdidx == CMD_startreplace)
	restart_edit = 'R';
    లేకపోతే
	restart_edit = 'V';

    ఒకవేళ (!eap->forceit)
    {
	ఒకవేళ (eap->cmdidx == CMD_startinsert)
	    restart_edit = 'i';
	curwin->w_curswant = 0;	    // avoid MAXCOL
    }

    ఒకవేళ (VIsual_చురుకైన)
	showmode();
}

/*
 * ":stopinsert"
 */
    స్థిర శూన్య
ex_stopinsert(exarg_T *eap వాడని)
{
    restart_edit = 0;
    stop_చొప్పించు_బహుళకం = TRUE;
#ఒకవేళ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // when called from remote_expr in insert mode, make sure insert mode is
    // ended by adding K_NOP to the typeahead buffer
    ఒకవేళ (vgetc_busy)
	ins_char_typebuf(K_NOP, 0);
#అయితే_ముగింపు
    clearmode();
}

/*
 * Execute normal mode command "cmd".
 * "remap" can be REMAP_NONE or REMAP_YES.
 */
    శూన్య
exec_సాధారణ_cmd(char_u *cmd, పూర్ణాంకం remap, పూర్ణాంకం silent)
{
    // Stuff the argument into the typeahead buffer.
    ins_typebuf(cmd, remap, 0, TRUE, silent);
    exec_సాధారణ(FALSE, FALSE, FALSE);
}

/*
 * Execute normal_cmd() until there is no typeahead left.
 * When "use_vpeekc" is TRUE use vpeekc() to check for available chars.
 */
    శూన్య
exec_సాధారణ(పూర్ణాంకం was_typed, పూర్ణాంకం use_vpeekc, పూర్ణాంకం may_use_terminal_loop వాడని)
{
    oparg_T	oa;
    పూర్ణాంకం		c;

    // When calling vpeekc() from feedkeys() it will return Ctrl_C when there
    // is nothing to get, so also check for Ctrl_C.
    clear_oparg(&oa);
    ముగింపు_op = FALSE;
    అయితే ((!stuff_ఖాళీలు()
		|| ((was_typed || !typebuf_typed()) && typebuf.tb_len > 0)
		|| (use_vpeekc && (c = vpeekc()) != NUL && c != Ctrl_C))
	    && !got_int)
    {
	update_topline_cursor();
#నిర్వచితమైతే FEAT_TERMINAL
	ఒకవేళ (may_use_terminal_loop && పదం_use_loop()
		&& oa.op_type == OP_NOP && oa.regname == NUL
		&& !VIsual_చురుకైన)
	{
	    // If terminal_loop() returns OK we got a key that is handled
	    // in Normal model.  With FAIL we first need to position the
	    // cursor and the screen needs to be redrawn.
	    ఒకవేళ (terminal_loop(TRUE) == OK)
		సాధారణ_cmd(&oa, TRUE);
	}
	లేకపోతే
#అయితే_ముగింపు
	    // execute a Normal mode cmd
	    సాధారణ_cmd(&oa, TRUE);
    }
}

#నిర్వచితమైతే FEAT_కనుగొను_ID
    స్థిర శూన్య
ex_checkpath(exarg_T *eap)
{
    కనుగొను_నమూనా_లోపల_మార్గం(ఖాళీ, 0, 0, FALSE, FALSE, CHECK_మార్గం, 1L,
	    eap->forceit ? ACTION_SHOW_ALL : ACTION_SHOW,
	    (linenr_T)1, (linenr_T)MAXLNUM, eap->forceit, FALSE);
}

# ఒకవేళ defined(FEAT_QUICKFIX)
/*
 * ":psearch"
 */
    స్థిర శూన్య
ex_psearch(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;
    ex_findpat(eap);
    g_do_tagpreview = 0;
}
# endif

    స్థిర శూన్య
ex_findpat(exarg_T *eap)
{
    పూర్ణాంకం		సంపూర్ణ = TRUE;
    దీర్ఘ	n;
    char_u	*p;
    పూర్ణాంకం		action;

    ఎంపిక (cmdnames[eap->cmdidx].cmd_name[2])
    {
	స్థితి 'e':	// ":psearch", ":isearch" and ":dsearch"
		ఒకవేళ (cmdnames[eap->cmdidx].cmd_name[0] == 'p')
		    action = ACTION_GOTO;
		లేకపోతే
		    action = ACTION_SHOW;
		ఆపు;
	స్థితి 'i':	// ":ilist" and ":dlist"
		action = ACTION_SHOW_ALL;
		ఆపు;
	స్థితి 'u':	// ":ijump" and ":djump"
		action = ACTION_GOTO;
		ఆపు;
	అప్రమేయ:	// ":isplit" and ":dsplit"
		action = ACTION_విభజించు;
		ఆపు;
    }

    n = 1;
    ఒకవేళ (vim_isdigit(*eap->arg))	// get count
    {
	n = getdigits(&eap->arg);
	eap->arg = skipwhite(eap->arg);
    }
    ఒకవేళ (*eap->arg == '/')   // Match regexp, not just whole words
    {
	సంపూర్ణ = FALSE;
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	ఒకవేళ (*p)
	{
	    *p++ = NUL;
	    p = skipwhite(p);

	    // Check for trailing illegal characters
	    ఒకవేళ (!చివర్లు_excmd2(eap->arg, p))
		eap->errmsg = ex_errmsg(e_trailing_characters_str, p);
	    లేకపోతే
		సమితి_nextcmd(eap, p);
	}
    }
    ఒకవేళ (!eap->skip)
	కనుగొను_నమూనా_లోపల_మార్గం(eap->arg, 0, (పూర్ణాంకం)STRLEN(eap->arg),
		సంపూర్ణ, !eap->forceit,
		*eap->cmd == 'd' ? కనుగొను_DEFINE : కనుగొను_ANY, n, action,
		eap->line1, eap->line2, eap->forceit, FALSE);
}
#అయితే_ముగింపు


#నిర్వచితమైతే FEAT_QUICKFIX
/*
 * ":ptag", ":ptselect", ":ptjump", ":ptnext", etc.
 */
    స్థిర శూన్య
ex_ptag(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;  // will be reset to 0 in ex_tag_cmd()
    ex_ట్యాగ్_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
}

/*
 * ":pedit"
 */
    స్థిర శూన్య
ex_pedit(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_కిటికీ();

    // Edit the file.
    do_exedit(eap, ఖాళీ);

    వెనుక_గమ్యం_current_కిటికీ(curwin_save);
}

/*
 * ":pbuffer"
 */
    స్థిర శూన్య
ex_pbuffer(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_కిటికీ();

    // Go to the buffer.
    do_exbuffer(eap);

    వెనుక_గమ్యం_current_కిటికీ(curwin_save);
}

    స్థిర శూన్య
prepare_preview_కిటికీ(శూన్య)
{
    ఒకవేళ (ERROR_IF_ANY_POPUP_కిటికీ)
	తిరిగి;

    // Open the preview window or popup and make it the current window.
    g_do_tagpreview = p_pvh;
    prepare_tagpreview(TRUE, TRUE, FALSE);
}

    స్థిర శూన్య
వెనుక_గమ్యం_current_కిటికీ(win_T *curwin_save)
{
    ఒకవేళ (curwin != curwin_save && win_valid(curwin_save))
    {
	// Return cursor to where we were
	validate_cursor();
	redraw_later(UPD_VALID);
	win_enter(curwin_save, TRUE);
    }
# ifdef FEAT_PROP_POPUP
    లేకపోతే ఒకవేళ (WIN_IS_POPUP(curwin))
    {
	// can't keep focus in popup window
	win_enter(firstwin, TRUE);
    }
# endif
    g_do_tagpreview = 0;
}
#అయితే_ముగింపు

/*
 * ":stag", ":stselect" and ":stjump".
 */
    స్థిర శూన్య
ex_stag(exarg_T *eap)
{
    postponed_విభజించు = -1;
    postponed_విభజించు_flags = cmdmod.cmod_విభజించు;
    postponed_విభజించు_tab = cmdmod.cmod_tab;
    ex_ట్యాగ్_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
    postponed_విభజించు_flags = 0;
    postponed_విభజించు_tab = 0;
}

/*
 * ":tag", ":tselect", ":tjump", ":tnext", etc.
 */
    స్థిర శూన్య
ex_ట్యాగ్(exarg_T *eap)
{
    ex_ట్యాగ్_cmd(eap, cmdnames[eap->cmdidx].cmd_name);
}

    స్థిర శూన్య
ex_ట్యాగ్_cmd(exarg_T *eap, char_u *name)
{
    పూర్ణాంకం		cmd;

    ఎంపిక (name[1])
    {
	స్థితి 'j': cmd = DT_JUMP;	// ":tjump"
		  ఆపు;
	స్థితి 's': cmd = DT_SELECT;	// ":tselect"
		  ఆపు;
	స్థితి 'p': cmd = DT_PREV;	// ":tprevious"
		  ఆపు;
	స్థితి 'N': cmd = DT_PREV;	// ":tNext"
		  ఆపు;
	స్థితి 'n': cmd = DT_తదుపరి;	// ":tnext"
		  ఆపు;
	స్థితి 'o': cmd = DT_తీయి;		// ":pop"
		  ఆపు;
	స్థితి 'f':			// ":tfirst"
	స్థితి 'r': cmd = DT_మొదటి;	// ":trewind"
		  ఆపు;
	స్థితి 'l': cmd = DT_చివరి;	// ":tlast"
		  ఆపు;
	అప్రమేయ:			// ":tag"
#నిర్వచితమైతే FEAT_CSCOPE
		  ఒకవేళ (p_cst && *eap->arg != NUL)
		  {
		      ex_cstag(eap);
		      తిరిగి;
		  }
#అయితే_ముగింపు
		  cmd = DT_ట్యాగ్;
		  ఆపు;
    }

    ఒకవేళ (name[0] == 'l')
    {
#నిర్వచితం_కాకపోతే FEAT_QUICKFIX
	ex_ni(eap);
	తిరిగి;
#లేకపోతే
	cmd = DT_LTAG;
#అయితే_ముగింపు
    }

    do_ట్యాగ్(eap->arg, cmd, eap->addr_లెక్క > 0 ? (పూర్ణాంకం)eap->line2 : 1,
							  eap->forceit, TRUE);
}

గణన {
    SPEC_PERC = 0,
    SPEC_HASH,
    SPEC_CWORD,	    // cursor word
    SPEC_CCWORD,    // cursor WORD
    SPEC_CEXPR,	    // expr under cursor
    SPEC_CFILE,	    // cursor path name
    SPEC_SFILE,	    // ":so" file name
    SPEC_SLNUM,	    // ":so" file line number
    SPEC_దొంతర,	    // call stack
    SPEC_SCRIPT,    // script file name
    SPEC_AFILE,	    // autocommand file name
    SPEC_ABUF,	    // autocommand buffer number
    SPEC_AMATCH,    // autocommand match name
    SPEC_SFLNUM,    // script file line number
    SPEC_SID	    // script ID: <SNR>123_
#నిర్వచితమైతే FEAT_CLIENTSERVER
    , SPEC_CLIENT
#అయితే_ముగింపు
};

/*
 * Check "str" for starting with a special cmdline variable.
 * If found return one of the SPEC_ values and set "*usedlen" to the length of
 * the variable.  Otherwise return -1 and "*usedlen" is unchanged.
 */
    పూర్ణాంకం
కనుగొను_cmdline_var(char_u *src, size_t *usedlen)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    స్థిర keyvalue_T spec_str_tab[] = {
	KEYVALUE_నమోదు(SPEC_SID, "SID>"),	    // script ID: <SNR>123_
	KEYVALUE_నమోదు(SPEC_ABUF, "abuf>"),	    // autocommand buffer number
	KEYVALUE_నమోదు(SPEC_AFILE, "afile>"),	    // autocommand file name
	KEYVALUE_నమోదు(SPEC_AMATCH, "amatch>"),	    // autocommand match name
	KEYVALUE_నమోదు(SPEC_CCWORD, "cWORD>"),	    // cursor WORD
	KEYVALUE_నమోదు(SPEC_CEXPR, "cexpr>"),	    // expr under cursor
	KEYVALUE_నమోదు(SPEC_CFILE, "cfile>"),	    // cursor path name
#నిర్వచితమైతే FEAT_CLIENTSERVER
	KEYVALUE_నమోదు(SPEC_CLIENT, "client>"),
#అయితే_ముగింపు
	KEYVALUE_నమోదు(SPEC_CWORD, "cword>"),	    // cursor word
	KEYVALUE_నమోదు(SPEC_SCRIPT, "script>"),	    // script file name
	KEYVALUE_నమోదు(SPEC_SFILE, "sfile>"),	    // ":so" file name
	KEYVALUE_నమోదు(SPEC_SFLNUM, "sflnum>"),	    // script file line number
	KEYVALUE_నమోదు(SPEC_SLNUM, "slnum>"),	    // ":so" file line number
	KEYVALUE_నమోదు(SPEC_దొంతర, "stack>")	    // call stack
    };
    keyvalue_T లక్ష్యం;
    keyvalue_T *నమోదు_2;

    ఎంపిక (*src)
    {
    స్థితి '%':
	*usedlen = 1;
	తిరిగి SPEC_PERC;

    స్థితి '#':
	*usedlen = 1;
	తిరిగి SPEC_HASH;

    స్థితి '<':
	లక్ష్యం.కీలకం = 0;
	లక్ష్యం.విలువ.string = src + 1;	    // skip over '<'
	లక్ష్యం.విలువ.నిడివి = 0;	    // not used, see cmp_keyvalue_value_n()

	నమోదు_2 = (keyvalue_T *)bsearch(&లక్ష్యం, &spec_str_tab,
		ARRAY_నిడివి(spec_str_tab), పరిమాణం(spec_str_tab[0]),
		cmp_keyvalue_విలువ_n);
	ఒకవేళ (నమోదు_2 == ఖాళీ)
	    తిరిగి -1;

	*usedlen = నమోదు_2->విలువ.నిడివి + 1;
	తిరిగి నమోదు_2->కీలకం;

    అప్రమేయ:
	ఆపు;
    }

    తిరిగి -1;
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
    అక్షరం	**errormsg,	// pointer to error message
    పూర్ణాంకం		*escaped,	// return value has escaped white space (can
				// be NULL)
    పూర్ణాంకం		ఖాళీలు_is_error)	// empty result is considered an error
{
    పూర్ణాంకం		i;
    char_u	*s;
    char_u	*ఫలితం = (char_u *)"";
    char_u	*resultbuf = ఖాళీ;
    size_t	resultlen;
    buf_T	*buf;
    పూర్ణాంకం		valid = VALID_తల + VALID_మార్గం;    // assume valid result
    పూర్ణాంకం		spec_idx;
    పూర్ణాంకం		tilde_file = FALSE;
    పూర్ణాంకం		skip_mod = FALSE;
    char_u	strbuf[30];

    *errormsg = ఖాళీ;
    ఒకవేళ (escaped != ఖాళీ)
	*escaped = FALSE;

    /*
     * Check if there is something to do.
     */
    spec_idx = కనుగొను_cmdline_var(src, usedlen);
    ఒకవేళ (spec_idx < 0)	// no match
    {
	*usedlen = 1;
	తిరిగి ఖాళీ;
    }

    /*
     * Skip when preceded with a backslash "\%" and "\#".
     * Note: In "\\%" the % is also not recognized!
     */
    ఒకవేళ (src > srcstart && src[-1] == '\\')
    {
	*usedlen = 0;
	STRMOVE(src - 1, src);	// remove backslash
	తిరిగి ఖాళీ;
    }

    /*
     * word or WORD under cursor
     */
    ఒకవేళ (spec_idx == SPEC_CWORD || spec_idx == SPEC_CCWORD
						     || spec_idx == SPEC_CEXPR)
    {
	resultlen = కనుగొను_ident_under_cursor(&ఫలితం,
		spec_idx == SPEC_CWORD ? (కనుగొను_IDENT | కనుగొను_STRING)
	      : spec_idx == SPEC_CEXPR ? (కనుగొను_IDENT | కనుగొను_STRING | కనుగొను_EVAL)
	      : కనుగొను_STRING);
	ఒకవేళ (resultlen == 0)
	{
	    *errormsg = "";
	    తిరిగి ఖాళీ;
	}
    }

    /*
     * '#': Alternate file name
     * '%': Current file name
     *	    File name under the cursor
     *	    File name for autocommand
     *	and following modifiers
     */
    లేకపోతే
    {
	size_t off = 0;

	ఎంపిక (spec_idx)
	{
	స్థితి SPEC_PERC:
#నిర్వచితమైతే FEAT_EVAL
		ఒకవేళ (!లోపల_vim9script() || src[1] != '%')
#అయితే_ముగింపు
		{
		    // '%': current file
		    ఒకవేళ (curbuf->b_fname == ఖాళీ)
		    {
			ఫలితం = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    లేకపోతే
		    {
			ఫలితం = curbuf->b_fname;
			tilde_file = STRCMP(ఫలితం, "~") == 0;
		    }
		    ఆపు;
		}
#నిర్వచితమైతే FEAT_EVAL
		// "%%" alternate file
		off = 1;
#అయితే_ముగింపు
		// FALLTHROUGH
	స్థితి SPEC_HASH:		// '#' or "#99": alternate file
		ఒకవేళ (off == 0 ? src[1] == '#' : src[2] == '%')
		{
		    // "##" or "%%%": the argument list
		    ఫలితం = arg_all();
		    resultbuf = ఫలితం;
		    *usedlen = off + 2;
		    ఒకవేళ (escaped != ఖాళీ)
			*escaped = TRUE;
		    skip_mod = TRUE;
		    ఆపు;
		}
		s = src + off + 1;
		ఒకవేళ (*s == '<')		// "#<99" uses v:oldfiles
		    ++s;
		i = (పూర్ణాంకం)getdigits(&s);
		ఒకవేళ (s == src + off + 2 && src[off + 1] == '-')
		    // just a minus sign, don't skip over it
		    s--;
		*usedlen = (పూర్ణాంకం)(s - src); // length of what we expand

		ఒకవేళ (src[off + 1] == '<' && i != 0)
		{
		    ఒకవేళ (*usedlen < off + 2)
		    {
			// Should we give an error message for #<text?
			*usedlen = off + 1;
			తిరిగి ఖాళీ;
		    }
#నిర్వచితమైతే FEAT_EVAL
		    ఫలితం = list_కనుగొను_str(get_vim_var_list(VV_OLDFILES),
								     (దీర్ఘ)i);
		    ఒకవేళ (ఫలితం == ఖాళీ)
		    {
			*errormsg = "";
			తిరిగి ఖాళీ;
		    }
#లేకపోతే
		    *errormsg = _(e_hashsmall_is_not_అందుబాటు_without_the_eval_feature);
		    తిరిగి ఖాళీ;
#అయితే_ముగింపు
		}
		లేకపోతే
		{
		    ఒకవేళ (i == 0 && src[off + 1] == '<' && *usedlen > off + 1)
			*usedlen = off + 1;
		    buf = buflist_findnr(i);
		    ఒకవేళ (buf == ఖాళీ)
		    {
			*errormsg = _(e_no_alternate_file_name_గమ్యం_substitute_for_hash);
			తిరిగి ఖాళీ;
		    }
		    ఒకవేళ (lnump != ఖాళీ)
			*lnump = ECMD_చివరి;
		    ఒకవేళ (buf->b_fname == ఖాళీ)
		    {
			ఫలితం = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    లేకపోతే
		    {
			ఫలితం = buf->b_fname;
			tilde_file = STRCMP(ఫలితం, "~") == 0;
		    }
		}
		ఆపు;

	స్థితి SPEC_CFILE:	// file name under cursor
		ఫలితం = file_name_at_cursor(FNAME_MESS|FNAME_HYP, 1L, ఖాళీ);
		ఒకవేళ (ఫలితం == ఖాళీ)
		{
		    *errormsg = "";
		    తిరిగి ఖాళీ;
		}
		resultbuf = ఫలితం;	    // remember allocated string
		ఆపు;

	స్థితి SPEC_AFILE:	// file name for autocommand
		ఫలితం = autocmd_fname;
		ఒకవేళ (ఫలితం != ఖాళీ && !autocmd_fname_నిండిన)
		{
		    // Still need to turn the fname into a full path.  It is
		    // postponed to avoid a delay when <afile> is not used.
		    autocmd_fname_నిండిన = TRUE;
		    ఫలితం = నిండిన_Name_save(autocmd_fname, FALSE);
		    vim_free(autocmd_fname);
		    autocmd_fname = ఫలితం;
		}
		ఒకవేళ (ఫలితం == ఖాళీ)
		{
		    *errormsg = _(e_no_autocommand_file_name_గమ్యం_substitute_for_afile);
		    తిరిగి ఖాళీ;
		}
		ఫలితం = shorten_fname1(ఫలితం);
		ఆపు;

	స్థితి SPEC_ABUF:		// buffer number for autocommand
		ఒకవేళ (autocmd_bufnr <= 0)
		{
		    *errormsg = _(e_no_autocommand_బఫరు_సంఖ్య_గమ్యం_substitute_for_abuf);
		    తిరిగి ఖాళీ;
		}
		sprintf((అక్షరం *)strbuf, "%d", autocmd_bufnr);
		ఫలితం = strbuf;
		ఆపు;

	స్థితి SPEC_AMATCH:	// match name for autocommand
		ఫలితం = autocmd_సరిపోలిక;
		ఒకవేళ (ఫలితం == ఖాళీ)
		{
		    *errormsg = _(e_no_autocommand_సరిపోలిక_name_గమ్యం_substitute_for_amatch);
		    తిరిగి ఖాళీ;
		}
		ఆపు;

	స్థితి SPEC_SFILE:	// file name for ":so" command
		ఫలితం = estack_sfile(ESTACK_SFILE);
		ఒకవేళ (ఫలితం == ఖాళీ)
		{
		    *errormsg = _(e_no_ఆధారం_file_name_గమ్యం_substitute_for_sfile);
		    తిరిగి ఖాళీ;
		}
		resultbuf = ఫలితం;	    // remember allocated string
		ఆపు;
	స్థితి SPEC_దొంతర:	// call stack
		ఫలితం = estack_sfile(ESTACK_దొంతర);
		ఒకవేళ (ఫలితం == ఖాళీ)
		{
		    *errormsg = _(e_no_call_దొంతర_గమ్యం_substitute_for_దొంతర);
		    తిరిగి ఖాళీ;
		}
		resultbuf = ఫలితం;	    // remember allocated string
		ఆపు;
	స్థితి SPEC_SCRIPT:	// script file name
		ఫలితం = estack_sfile(ESTACK_SCRIPT);
		ఒకవేళ (ఫలితం == ఖాళీ)
		{
		    *errormsg = _(e_no_script_file_name_గమ్యం_substitute_for_script);
		    తిరిగి ఖాళీ;
		}
		resultbuf = ఫలితం;	    // remember allocated string
		ఆపు;

	స్థితి SPEC_SLNUM:	// line in file for ":so" command
		ఒకవేళ (SOURCING_NAME == ఖాళీ || SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_పంక్తి_సంఖ్య_గమ్యం_use_for_slnum);
		    తిరిగి ఖాళీ;
		}
		sprintf((అక్షరం *)strbuf, "%ld", SOURCING_LNUM);
		ఫలితం = strbuf;
		ఆపు;

#నిర్వచితమైతే FEAT_EVAL
	స్థితి SPEC_SFLNUM:	// line in script file
		ఒకవేళ (current_sctx.sc_lnum + SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_పంక్తి_సంఖ్య_గమ్యం_use_for_sflnum);
		    తిరిగి ఖాళీ;
		}
		sprintf((అక్షరం *)strbuf, "%ld",
				 (దీర్ఘ)(current_sctx.sc_lnum + SOURCING_LNUM));
		ఫలితం = strbuf;
		ఆపు;

	స్థితి SPEC_SID:
		ఒకవేళ (current_sctx.sc_sid <= 0)
		{
		    *errormsg = _(e_using_sid_not_లోపల_script_context);
		    తిరిగి ఖాళీ;
		}
		sprintf((అక్షరం *)strbuf, "<SNR>%d_", current_sctx.sc_sid);
		ఫలితం = strbuf;
		ఆపు;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_CLIENTSERVER
	స్థితి SPEC_CLIENT:	// Source of last submitted input
# ifdef MSWIN
		sprintf((అక్షరం *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)client_కిటికీ);
		ఫలితం = strbuf;
# లేకపోతే
#  ifdef FEAT_SOCKETSERVER
		ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_SOCKET)
		{
		    ఒకవేళ (client_socket == ఖాళీ)
			ఫలితం = (char_u *)"";
		    లేకపోతే
			ఫలితం = client_socket;
		}
#  endif
#  ifdef FEAT_X11
		ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_X11)
		{
		    sprintf((అక్షరం *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)client_కిటికీ);
		    ఫలితం = strbuf;
		}
#  endif
# endif
		ఆపు;
#అయితే_ముగింపు
	}

	resultlen = STRLEN(ఫలితం);	// length of new string
	ఒకవేళ (src[*usedlen] == '<')	// remove the file name extension
	{
	    ++*usedlen;
	    ఒకవేళ ((s = vim_strrchr(ఫలితం, '.')) != ఖాళీ && s >= gettail(ఫలితం))
		resultlen = s - ఫలితం;
	}
	లేకపోతే ఒకవేళ (!skip_mod)
	{
	    valid |= modify_fname(src, tilde_file, usedlen, &ఫలితం, &resultbuf,
								  &resultlen);
	    ఒకవేళ (ఫలితం == ఖాళీ)
	    {
		*errormsg = "";
		తిరిగి ఖాళీ;
	    }
	}
    }

    ఒకవేళ (resultlen == 0 || valid != VALID_తల + VALID_మార్గం)
    {
	ఒకవేళ (ఖాళీలు_is_error)
	{
	    ఒకవేళ (valid != VALID_తల + VALID_మార్గం)
		*errormsg = _(e_ఖాళీలు_file_name_for_percent_or_hash_only_పనిచేస్తుంది_with_ph);
	    లేకపోతే
		*errormsg = _(e_evaluates_గమ్యం_an_ఖాళీలు_string);
	}
	ఫలితం = ఖాళీ;
    }
    లేకపోతే
	ఫలితం = vim_strnsave(ఫలితం, resultlen);
    vim_free(resultbuf);
    తిరిగి ఫలితం;
}

/*
 * Expand the <sfile> string in "arg".
 *
 * Returns an allocated string, or NULL for any error.
 */
    char_u *
expand_sfile(char_u *arg)
{
    అక్షరం	*errormsg;
    char_u	*ఫలితం;
    size_t	resultlen;
    char_u	*newres;
    size_t	len;
    char_u	*repl;
    size_t	repllen;
    size_t	srclen;
    char_u	*p;

    resultlen = STRLEN(arg);
    ఫలితం = vim_strnsave(arg, resultlen);
    ఒకవేళ (ఫలితం == ఖాళీ)
	తిరిగి ఖాళీ;

    కొరకు (p = ఫలితం; *p; )
    {
	ఒకవేళ (STRNCMP(p, "<sfile>", 7) != 0)
	    ++p;
	లేకపోతే
	{
	    // replace "<sfile>" with the sourced file name, and do ":" stuff
	    repl = eval_vars(p, ఫలితం, &srclen, ఖాళీ, &errormsg, ఖాళీ, TRUE);
	    ఒకవేళ (errormsg != ఖాళీ)
	    {
		ఒకవేళ (*errormsg)
		    emsg(errormsg);
		vim_free(ఫలితం);
		తిరిగి ఖాళీ;
	    }
	    ఒకవేళ (repl == ఖాళీ)		// no match (cannot happen)
	    {
		p += srclen;
		కొనసాగు;
	    }
	    repllen = STRLEN(repl);
	    resultlen += repllen - srclen;
	    newres = alloc(resultlen + 1);
	    ఒకవేళ (newres == ఖాళీ)
	    {
		vim_free(repl);
		vim_free(ఫలితం);
		తిరిగి ఖాళీ;
	    }
	    len = p - ఫలితం;
	    mch_memmove(newres, ఫలితం, len);
	    STRCPY(newres + len, repl);
	    len += repllen;
	    STRCPY(newres + len, p + srclen);
	    vim_free(repl);
	    vim_free(ఫలితం);
	    ఫలితం = newres;
	    p = newres + len;		// continue after the match
	}
    }

    తిరిగి ఫలితం;
}

#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
/*
 * Make a dialog message in "buff[DIALOG_MSG_SIZE]".
 * "format" must contain "%s".
 */
    శూన్య
dialog_సందేశం(char_u *buff, అక్షరం *format, char_u *fname)
{
    ఒకవేళ (fname == ఖాళీ)
	fname = (char_u *)_("Untitled");
    vim_snprintf((అక్షరం *)buff, DIALOG_సందేశం_కొలత, format, fname);
}
#అయితే_ముగింపు

/*
 * ":behave {mswin,xterm}"
 */
    స్థిర శూన్య
ex_behave(exarg_T *eap)
{
    ఒకవేళ (STRCMP(eap->arg, "mswin") == 0)
    {
	సమితి_option_విలువ_give_err((char_u *)"selection",
						 0L, (char_u *)"exclusive", 0);
	సమితి_option_విలువ_give_err((char_u *)"selectmode",
						 0L, (char_u *)"mouse,key", 0);
	సమితి_option_విలువ_give_err((char_u *)"mousemodel",
						     0L, (char_u *)"popup", 0);
	సమితి_option_విలువ_give_err((char_u *)"keymodel",
					  0L, (char_u *)"startsel,stopsel", 0);
    }
    లేకపోతే ఒకవేళ (STRCMP(eap->arg, "xterm") == 0)
    {
	సమితి_option_విలువ_give_err((char_u *)"selection",
						 0L, (char_u *)"inclusive", 0);
	సమితి_option_విలువ_give_err((char_u *)"selectmode", 0L, (char_u *)"", 0);
	సమితి_option_విలువ_give_err((char_u *)"mousemodel",
						    0L, (char_u *)"extend", 0);
	సమితి_option_విలువ_give_err((char_u *)"keymodel", 0L, (char_u *)"", 0);
    }
    లేకపోతే
	semsg(_(e_invalid_వాదం_str), eap->arg);
}

స్థిర పూర్ణాంకం filetype_detect = FALSE;
స్థిర పూర్ణాంకం filetype_plugin = FALSE;
స్థిర పూర్ణాంకం filetype_indent = FALSE;

/*
 * ":filetype [plugin] [indent] {on,off,detect}"
 * on: Load the filetype.vim file to install autocommands for file types.
 * off: Load the ftoff.vim file to remove all autocommands for file types.
 * plugin on: load filetype.vim and ftplugin.vim
 * plugin off: load ftplugof.vim
 * indent on: load filetype.vim and indent.vim
 * indent off: load indoff.vim
 */
    స్థిర శూన్య
ex_filetype(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    పూర్ణాంకం		plugin = FALSE;
    పూర్ణాంకం		indent = FALSE;

    ఒకవేళ (*eap->arg == NUL)
    {
	// Print current status.
	smsg("filetype detection:%s  plugin:%s  indent:%s",
		filetype_detect ? "ON" : "OFF",
		filetype_plugin ? (filetype_detect ? "ON" : "(on)") : "OFF",
		filetype_indent ? (filetype_detect ? "ON" : "(on)") : "OFF");
	తిరిగి;
    }

    // Accept "plugin" and "indent" in any order.
    కొరకు (;;)
    {
	ఒకవేళ (STRNCMP(arg, "plugin", 6) == 0)
	{
	    plugin = TRUE;
	    arg = skipwhite(arg + 6);
	    కొనసాగు;
	}
	ఒకవేళ (STRNCMP(arg, "indent", 6) == 0)
	{
	    indent = TRUE;
	    arg = skipwhite(arg + 6);
	    కొనసాగు;
	}
	ఆపు;
    }
    ఒకవేళ (STRCMP(arg, "on") == 0 || STRCMP(arg, "detect") == 0)
    {
	ఒకవేళ (*arg == 'o' || !filetype_detect)
	{
	    ఆధారం_runtime((char_u *)FILETYPE_FILE, DIP_ALL);
	    filetype_detect = TRUE;
	    ఒకవేళ (plugin)
	    {
		ఆధారం_runtime((char_u *)FTPLUGIN_FILE, DIP_ALL);
		filetype_plugin = TRUE;
	    }
	    ఒకవేళ (indent)
	    {
		ఆధారం_runtime((char_u *)INDENT_FILE, DIP_ALL);
		filetype_indent = TRUE;
	    }
	}
	ఒకవేళ (*arg == 'd')
	{
	    (శూన్య)do_doautocmd((char_u *)"filetypedetect BufRead", TRUE, ఖాళీ);
	    do_modelines(0);
	}
    }
    లేకపోతే ఒకవేళ (STRCMP(arg, "off") == 0)
    {
	ఒకవేళ (plugin || indent)
	{
	    ఒకవేళ (plugin)
	    {
		ఆధారం_runtime((char_u *)FTPLUGOF_FILE, DIP_ALL);
		filetype_plugin = FALSE;
	    }
	    ఒకవేళ (indent)
	    {
		ఆధారం_runtime((char_u *)INDOFF_FILE, DIP_ALL);
		filetype_indent = FALSE;
	    }
	}
	లేకపోతే
	{
	    ఆధారం_runtime((char_u *)FTOFF_FILE, DIP_ALL);
	    filetype_detect = FALSE;
	}
    }
    లేకపోతే
	semsg(_(e_invalid_వాదం_str), arg);
}

/*
 * ":setfiletype [FALLBACK] {name}"
 */
    స్థిర శూన్య
ex_setfiletype(exarg_T *eap)
{
    ఒకవేళ (curbuf->b_did_filetype)
	తిరిగి;

    char_u *arg = eap->arg;
    ఒకవేళ (STRNCMP(arg, "FALLBACK ", 9) == 0)
	arg += 9;

    సమితి_option_విలువ_give_err((char_u *)"filetype", 0L, arg, OPT_LOCAL);
    ఒకవేళ (arg != eap->arg)
	curbuf->b_did_filetype = false;
}

    స్థిర శూన్య
ex_digraphs(exarg_T *eap వాడని)
{
#నిర్వచితమైతే FEAT_DIGRAPHS
    ఒకవేళ (*eap->arg != NUL)
	putdigraph(eap->arg);
    లేకపోతే
	listdigraphs(eap->forceit);
#లేకపోతే
    emsg(_(e_no_digraphs_సంస్కరణ));
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_శోధన_EXTRA)
    శూన్య
సమితి_no_hlsearch(పూర్ణాంకం flag)
{
    no_hlsearch = flag;
# ifdef FEAT_EVAL
    సమితి_vim_var_nr(VV_HLSEARCH, !no_hlsearch && p_hls);
# endif
}

/*
 * ":nohlsearch"
 */
    స్థిర శూన్య
ex_nohlsearch(exarg_T *eap వాడని)
{
    సమితి_no_hlsearch(TRUE);
    redraw_all_later(UPD_SOME_VALID);
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_CRYPT
/*
 * ":X": Get crypt key
 */
    స్థిర శూన్య
ex_X(exarg_T *eap వాడని)
{
    crypt_check_current_పద్ధతి();
    (శూన్య)crypt_get_కీలకం(TRUE, TRUE);
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_FOLDING
    స్థిర శూన్య
ex_fold(exarg_T *eap)
{
    ఒకవేళ (foldManualAllowed(TRUE))
	foldCreate(eap->line1, eap->line2);
}

    స్థిర శూన్య
ex_foldopen(exarg_T *eap)
{
    opFoldRange(eap->line1, eap->line2, eap->cmdidx == CMD_foldopen,
							 eap->forceit, FALSE);
}

    స్థిర శూన్య
ex_folddo(exarg_T *eap)
{
    linenr_T	lnum;

# ifdef FEAT_CLIPBOARD
    ప్రారంభం_global_changes();
# endif
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
# endif

    // First set the marks for all lines closed/open.
    కొరకు (lnum = eap->line1; lnum <= eap->line2; ++lnum)
	ఒకవేళ (hasFolding(lnum, ఖాళీ, ఖాళీ) == (eap->cmdidx == CMD_folddoclosed))
	    ml_setmarked(lnum);

    // Execute the command on the marked lines.
    global_exe(eap->arg);
    ml_clearmarked();	   // clear rest of the marks
# ifdef FEAT_CLIPBOARD
    చివర_global_changes();
# endif
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_QUICKFIX)
/*
 * Returns TRUE if the supplied Ex cmdidx is for a location list command
 * instead of a quickfix command.
 */
    పూర్ణాంకం
is_loclist_cmd(పూర్ణాంకం cmdidx)
{
    ఒకవేళ (cmdidx < 0 || cmdidx >= CMD_కొలత)
	తిరిగి FALSE;
    తిరిగి cmdnames[cmdidx].cmd_name[0] == 'l';
}
#అయితే_ముగింపు

    పూర్ణాంకం
get_pressedreturn(శూన్య)
{
    తిరిగి ex_pressedreturn;
}

    శూన్య
సమితి_pressedreturn(పూర్ణాంకం val)
{
    ex_pressedreturn = val;
}
