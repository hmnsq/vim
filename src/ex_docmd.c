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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

ਸਥਿਰ ਪੂਰਨ	quitmore = 0;
ਸਥਿਰ ਪੂਰਨ	ex_pressedreturn = FALSE;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_PRINTER
# define ex_hardcopy	ex_ni
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ char_u	*do_one_cmd(char_u **, ਪੂਰਨ, cstack_T *, char_u *(*fgetline)(ਪੂਰਨ, ਖਾਲੀ *, ਪੂਰਨ, getline_opt_T), ਖਾਲੀ *cookie);
#ਨਹੀਂਤਾਂ
ਸਥਿਰ char_u	*do_one_cmd(char_u **, ਪੂਰਨ, char_u *(*fgetline)(ਪੂਰਨ, ਖਾਲੀ *, ਪੂਰਨ, getline_opt_T), ਖਾਲੀ *cookie);
ਸਥਿਰ ਪੂਰਨ	if_level = 0;		// depth in :if
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ	append_command(char_u *cmd);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
# define ex_emenu		ex_ni
# define ex_menu		ex_ni
# define ex_menutranslate	ex_ni
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ	ex_autocmd(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_doautocmd(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_bunload(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_ਬਫਰ(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	do_exbuffer(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_bmodified(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_bnext(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_bprevious(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_brewind(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_blast(exarg_T *eap);
ਸਥਿਰ char_u	*getargcmd(char_u **);
ਸਥਿਰ ਪੂਰਨ	getargopt(exarg_T *eap);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
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
#ਜੇ_ਅੰਤ
#ਜੇ !defined(FEAT_QUICKFIX) || !defined(FEAT_EVAL)
# define ex_cexpr		ex_ni
#ਜੇ_ਅੰਤ

ਸਥਿਰ linenr_T default_ਪਤਾ(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ ਪਤਾ_default_all(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	get_flags(exarg_T *eap);
#ਜੇ !defined(FEAT_PERL) \
	|| !defined(FEAT_PYTHON) || !defined(FEAT_PYTHON3) \
	|| !defined(FEAT_TCL) \
	|| !defined(FEAT_RUBY) \
	|| !defined(FEAT_LUA) \
	|| !defined(FEAT_MZSCHEME)
# define HAVE_EX_SCRIPT_NI
ਸਥਿਰ ਖਾਲੀ	ex_script_ni(exarg_T *eap);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਅੱਖਰ	*invalid_range(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	correct_range(exarg_T *eap);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
ਸਥਿਰ char_u	*replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep);
#ਜੇ_ਅੰਤ
ਸਥਿਰ char_u	*repl_cmdline(exarg_T *eap, char_u *src, size_t srclen, char_u *repl, char_u **cmdlinep);
ਸਥਿਰ ਖਾਲੀ	ex_highlight(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_colorscheme(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_cquit(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_quit_all(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_close(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_win_close(ਪੂਰਨ forceit, win_T *win, tabpage_T *tp);
ਸਥਿਰ ਖਾਲੀ	ex_only(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_resize(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_stag(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_tabclose(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_tabonly(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_tabnext(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_tabmove(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_tabs(exarg_T *eap);
#ਜੇ defined(FEAT_QUICKFIX)
ਸਥਿਰ ਖਾਲੀ	ex_pclose(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_ptag(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_pedit(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_pbuffer(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	prepare_preview_ਖਿੜਕੀ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ	ਪਿੱਠ_ਮੰਜ਼ਿਲ_current_ਖਿੜਕੀ(win_T *curwin_save);
#ਨਹੀਂਤਾਂ
# define ex_pclose		ex_ni
# define ex_ptag		ex_ni
# define ex_pedit		ex_ni
# define ex_pbuffer		ex_ni
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ	ex_hide(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_exit(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_print(exarg_T *eap);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BYTEOFF
ਸਥਿਰ ਖਾਲੀ	ex_goto(exarg_T *eap);
#ਨਹੀਂਤਾਂ
# define ex_goto		ex_ni
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ	ex_shell(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_preserve(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_recover(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_ਬਹੁਲਕ(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_wrongmodifier(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_ਲੱਭੋ(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_open(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_edit(exarg_T *eap);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
# define ex_gui			ex_nogui
ਸਥਿਰ ਖਾਲੀ	ex_nogui(exarg_T *eap);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
ਸਥਿਰ ਖਾਲੀ	ex_tearoff(exarg_T *eap);
#ਨਹੀਂਤਾਂ
# define ex_tearoff		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_ਪਦ_POPUP_MENU)) && defined(FEAT_MENU)
ਸਥਿਰ ਖਾਲੀ	ex_popup(exarg_T *eap);
#ਨਹੀਂਤਾਂ
# define ex_popup		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
# define ex_simalt		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ !defined(FEAT_GUI_MSWIN) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
# define gui_mch_ਲੱਭੋ_dialog	ex_ni
# define gui_mch_replace_dialog ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ !defined(FEAT_GUI_GTK)
# define ex_helpfind		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
# define ex_cscope		ex_ni
# define ex_scscope		ex_ni
# define ex_cstag		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
# define ex_syntax		ex_ni
# define ex_ownsyntax		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ !defined(FEAT_SYN_HL) || !defined(FEAT_PROFILE)
# define ex_syntime		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
# define ex_spell		ex_ni
# define ex_mkspell		ex_ni
# define ex_spelldump		ex_ni
# define ex_spellinfo		ex_ni
# define ex_spellrepall		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
# define ex_rundo		ex_ni
# define ex_wundo		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_LUA
# define ex_lua			ex_script_ni
# define ex_luado		ex_ni
# define ex_luafile		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_MZSCHEME
# define ex_mzscheme		ex_script_ni
# define ex_mzfile		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_PERL
# define ex_perl		ex_script_ni
# define ex_perldo		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON
# define ex_python		ex_script_ni
# define ex_pydo		ex_ni
# define ex_pyfile		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON3
# define ex_py3			ex_script_ni
# define ex_py3do		ex_ni
# define ex_py3file		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ !defined(FEAT_PYTHON) && !defined(FEAT_PYTHON3)
# define ex_pyx			ex_script_ni
# define ex_pyxdo		ex_ni
# define ex_pyxfile		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_TCL
# define ex_tcl			ex_script_ni
# define ex_tcldo		ex_ni
# define ex_tclfile		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_RUBY
# define ex_ruby		ex_script_ni
# define ex_rubydo		ex_ni
# define ex_rubyfile		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
# define ex_loadkeymap		ex_ni
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ	ex_swapname(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_syncbind(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_read(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_pwd(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_equal(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_sleep(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_winsize(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_wincmd(exarg_T *eap);
#ਜੇ defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
ਸਥਿਰ ਖਾਲੀ	ex_winpos(exarg_T *eap);
#ਨਹੀਂਤਾਂ
# define ex_winpos	    ex_ni
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ	ex_operators(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_put(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_iput(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_copymove(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_submagic(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_join(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_at(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_bang(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_undo(exarg_T *eap);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
ਸਥਿਰ ਖਾਲੀ	ex_wundo(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_rundo(exarg_T *eap);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ	ex_redo(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_later(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_redir(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_redrawstatus(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_redrawtabline(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_redrawtabpanel(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	close_redir(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ	ex_mark(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_startinsert(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_stopinsert(exarg_T *eap);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
ਸਥਿਰ ਖਾਲੀ	ex_checkpath(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_findpat(exarg_T *eap);
#ਨਹੀਂਤਾਂ
# define ex_findpat		ex_ni
# define ex_checkpath		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_ਲੱਭੋ_ID) && defined(FEAT_QUICKFIX)
ਸਥਿਰ ਖਾਲੀ	ex_psearch(exarg_T *eap);
#ਨਹੀਂਤਾਂ
# define ex_psearch		ex_ni
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ	ex_ਟੈਗ(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_ਟੈਗ_cmd(exarg_T *eap, char_u *name);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
# define ex_ਖੰਡ		ex_ni
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
# define ex_ਅੰਤ		ex_ni
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
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
# define ex_loadview		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
# define ex_viminfo		ex_ni
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ	ex_behave(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_filetype(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_setfiletype(exarg_T *eap);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
# define ex_diffoff		ex_ni
# define ex_diffpatch		ex_ni
# define ex_diffgetput		ex_ni
# define ex_diffsplit		ex_ni
# define ex_diffthis		ex_ni
# define ex_diffupdate		ex_ni
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ	ex_digraphs(exarg_T *eap);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
ਸਥਿਰ ਖਾਲੀ	ex_nohlsearch(exarg_T *eap);
#ਨਹੀਂਤਾਂ
# define ex_nohlsearch		ex_ni
# define ex_ਮੇਲ		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
ਸਥਿਰ ਖਾਲੀ	ex_X(exarg_T *eap);
#ਨਹੀਂਤਾਂ
# define ex_X			ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
ਸਥਿਰ ਖਾਲੀ	ex_fold(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_foldopen(exarg_T *eap);
ਸਥਿਰ ਖਾਲੀ	ex_folddo(exarg_T *eap);
#ਨਹੀਂਤਾਂ
# define ex_fold		ex_ni
# define ex_foldopen		ex_ni
# define ex_folddo		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ !(defined(HAVE_LOCALE_H) || defined(X_LOCALE))
# define ex_language		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
# define ex_sign		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
# define ex_nbclose		ex_ni
# define ex_nbkey		ex_ni
# define ex_nbstart		ex_ni
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
# define ex_profile		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
# define ex_terminal		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ !defined(FEAT_X11) || !defined(FEAT_XCLIPBOARD)
# define ex_xrestore		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ !defined(FEAT_WAYLAND)
# define ex_wlrestore		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ !defined(HAVE_CLIPMETHOD)
# define ex_clipreset		ex_ni
#ਜੇ_ਅੰਤ
#ਜੇ !defined(FEAT_PROP_POPUP)
# define ex_popupclear		ex_ni
#ਜੇ_ਅੰਤ

/*
 * Declare cmdnames[].
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DO_DECLARE_EXCMD
#ਅਨ੍ਤਰ੍ਭਾਵ "ex_cmds.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ex_cmdidxs.h"

ਸਥਿਰ char_u dollar_command[2] = {'$', 0};


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
// Struct for storing a line inside a while/for loop
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u	*ਸਤਰ;		// command line
    linenr_T	lnum;		// sourcing_lnum of the line
} wcmd_T;

/*
 * Structure used to store info for line position in a while or for loop.
 * This is required, because do_one_cmd() may invoke ex_function(), which
 * reads more lines that may come from the while/for loop.
 */
ਬਣਤਰ loop_cookie
{
    garray_T	*ਸਤਰਾਂ_gap;		// growarray with line info
    ਪੂਰਨ		current_ਸਤਰ;		// last read line from growarray
    ਪੂਰਨ		repeating;		// TRUE when looping a second time
    // When "repeating" is FALSE use "getline" and "cookie" to get lines
    char_u	*(*lc_getline)(ਪੂਰਨ, ਖਾਲੀ *, ਪੂਰਨ, getline_opt_T);
    ਖਾਲੀ	*cookie;
};

ਸਥਿਰ char_u	*get_loop_ਸਤਰ(ਪੂਰਨ c, ਖਾਲੀ *cookie, ਪੂਰਨ indent, getline_opt_T options);
ਸਥਿਰ ਪੂਰਨ	store_loop_ਸਤਰ(garray_T *gap, char_u *ਸਤਰ);
ਸਥਿਰ ਖਾਲੀ	free_cmdlines(garray_T *gap);

// Struct to save a few things while debugging.  Used in do_cmdline() only.
ਬਣਤਰ dbg_stuff
{
    ਪੂਰਨ		trylevel;
    ਪੂਰਨ		force_abort;
    except_T	*caught_ਢੇਰ;
    char_u	*vv_exception;
    char_u	*vv_throwpoint;
    ਪੂਰਨ		did_emsg;
    ਪੂਰਨ		got_int;
    ਪੂਰਨ		did_throw;
    ਪੂਰਨ		need_rethrow;
    ਪੂਰਨ		check_cstack;
    except_T	*current_exception;
};

    ਸਥਿਰ ਖਾਲੀ
save_dbg_stuff(ਬਣਤਰ dbg_stuff *dsp)
{
    dsp->trylevel	= trylevel;		trylevel = 0;
    dsp->force_abort	= force_abort;		force_abort = FALSE;
    dsp->caught_ਢੇਰ	= caught_ਢੇਰ;		caught_ਢੇਰ = ਨਿਰਾ;
    dsp->vv_exception	= v_exception(ਨਿਰਾ);
    dsp->vv_throwpoint	= v_throwpoint(ਨਿਰਾ);

    // Necessary for debugging an inactive ":catch", ":finally", ":endtry"
    dsp->did_emsg	= did_emsg;		did_emsg     = FALSE;
    dsp->got_int	= got_int;		got_int	     = FALSE;
    dsp->did_throw	= did_throw;		did_throw    = FALSE;
    dsp->need_rethrow	= need_rethrow;		need_rethrow = FALSE;
    dsp->check_cstack	= check_cstack;		check_cstack = FALSE;
    dsp->current_exception = current_exception;	current_exception = ਨਿਰਾ;
}

    ਸਥਿਰ ਖਾਲੀ
restore_dbg_stuff(ਬਣਤਰ dbg_stuff *dsp)
{
    suppress_errthrow = FALSE;
    trylevel = dsp->trylevel;
    force_abort = dsp->force_abort;
    caught_ਢੇਰ = dsp->caught_ਢੇਰ;
    (ਖਾਲੀ)v_exception(dsp->vv_exception);
    (ਖਾਲੀ)v_throwpoint(dsp->vv_throwpoint);
    did_emsg = dsp->did_emsg;
    got_int = dsp->got_int;
    did_throw = dsp->did_throw;
    need_rethrow = dsp->need_rethrow;
    check_cstack = dsp->check_cstack;
    current_exception = dsp->current_exception;
}
#ਜੇ_ਅੰਤ

/*
 * Check if ffname differs from fnum.
 * fnum is a buffer number. 0 == current buffer, 1-or-more must be a valid
 * buffer ID.
 * ffname is a full path to where a buffer lives on-disk or would live on-disk.
 *
 */
    ਸਥਿਰ ਪੂਰਨ
is_other_file(ਪੂਰਨ fnum, char_u *ffname)
{
    ਜੇ (fnum != 0)
    {
	ਜੇ (fnum == curbuf->b_fnum)
	    ਵਾਪਸ FALSE;

	ਵਾਪਸ TRUE;
    }

    ਜੇ (ffname == ਨਿਰਾ)
	ਵਾਪਸ TRUE;

    ਜੇ (*ffname == NUL)
	ਵਾਪਸ FALSE;

    // TODO: Need a reliable way to know whether a buffer is meant to live
    // on-disk !curbuf->b_dev_valid is not always available (example: missing
    // on Windows)
    ਜੇ (curbuf->b_sfname != ਨਿਰਾ
	    && *curbuf->b_sfname != NUL)
	// This occurs with unsaved buffers. In which case `ffname` actually
	// corresponds to curbuf->b_sfname
	ਵਾਪਸ fnamecmp(ffname, curbuf->b_sfname) != 0;

    ਵਾਪਸ otherfile(ffname);
}

/*
 * do_exmode(): Repeatedly get commands for the "Ex" mode, until the ":vi"
 * command is given.
 */
    ਖਾਲੀ
do_exmode(
    ਪੂਰਨ		improved)	    // TRUE for "improved Ex" mode
{
    ਪੂਰਨ		save_ਸੁਨੇਹਾ_scroll;
    ਪੂਰਨ		prev_ਸੁਨੇਹਾ_ਪੰਕਤੀ;
    linenr_T	prev_ਸਤਰ;
    varnumber_T	changedtick;

    ਜੇ (improved)
	exmode_ਸਰਗਰਮ = EXMODE_VIM;
    ਨਹੀਂਤਾਂ
	exmode_ਸਰਗਰਮ = EXMODE_ਸਾਧਾਰਨ;
    State = ਬਹੁਲਕ_ਸਾਧਾਰਨ;
    may_trigger_modechanged();

    // When using ":global /pat/ visual" and then "Q" we return to continue
    // the :global command.
    ਜੇ (global_busy)
	ਵਾਪਸ;

    save_ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_scroll;
    ++RedrawingDisabled;	    // don't redisplay the window
    ++no_wait_return;		    // don't wait for return
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // Ignore scrollbar and mouse events in Ex mode
    ++hold_gui_events;
#ਜੇ_ਅੰਤ

    ਸੁਨੇਹਾ(_("Entering Ex mode.  Type \"visual\" to go to Normal mode."));
    ਜਦੋਂ (exmode_ਸਰਗਰਮ)
    {
	// Check for a ":normal" command and no more characters left.
	ਜੇ (ex_ਸਾਧਾਰਨ_busy > 0 && typebuf.tb_len == 0)
	{
	    exmode_ਸਰਗਰਮ = FALSE;
	    ਰੋਕੋ;
	}
	ਸੁਨੇਹਾ_scroll = TRUE;
	need_wait_return = FALSE;
	ex_pressedreturn = FALSE;
	ex_no_reprint = FALSE;
	changedtick = CHANGEDTICK(curbuf);
	prev_ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
	prev_ਸਤਰ = curwin->w_cursor.lnum;
	ਜੇ (improved)
	{
	    cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
	    do_cmdline(ਨਿਰਾ, getexline, ਨਿਰਾ, 0);
	}
	ਨਹੀਂਤਾਂ
	    do_cmdline(ਨਿਰਾ, getexmodeline, ਨਿਰਾ, DOCMD_NOWAIT);
	ਸਤਰਾਂ_ਖੱਬਾ = ਪੰਕਤੀਆਂ - 1;

	ਜੇ ((prev_ਸਤਰ != curwin->w_cursor.lnum
		   || changedtick != CHANGEDTICK(curbuf)) && !ex_no_reprint)
	{
	    ਜੇ (curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)
		emsg(_(e_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ));
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (ex_pressedreturn)
		{
		    // go up one line, to overwrite the ":<CR>" line, so the
		    // output doesn't contain empty lines.
		    ਸੁਨੇਹਾ_ਪੰਕਤੀ = prev_ਸੁਨੇਹਾ_ਪੰਕਤੀ;
		    ਜੇ (prev_ਸੁਨੇਹਾ_ਪੰਕਤੀ == ਪੰਕਤੀਆਂ - 1)
			ਸੁਨੇਹਾ_ਪੰਕਤੀ--;
		}
		ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
		print_ਸਤਰ_no_ਅਗੇਤਰ(curwin->w_cursor.lnum, FALSE, FALSE);
		ਸੁਨੇਹਾ_clr_eos();
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (ex_pressedreturn && !ex_no_reprint)	// must be at EOF
	{
	    ਜੇ (curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)
		emsg(_(e_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ));
	    ਨਹੀਂਤਾਂ
		emsg(_(e_at_ਸਿਰਾ_ਦਾ_file));
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    --hold_gui_events;
#ਜੇ_ਅੰਤ
    ਜੇ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    update_screen(UPD_CLEAR);
    need_wait_return = FALSE;
    ਸੁਨੇਹਾ_scroll = save_ਸੁਨੇਹਾ_scroll;
}

/*
 * Print the executed command for when 'verbose' is set.
 * When "lnum" is 0 only print the command.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੁਨੇਹਾ_verbose_cmd(linenr_T lnum, char_u *cmd)
{
    ++no_wait_return;
    verbose_enter_scroll();

    ਜੇ (lnum == 0)
	smsg(_("Executing: %s"), cmd);
    ਨਹੀਂਤਾਂ
	smsg(_("line %ld: %s"), (ਲੰਮਾ)lnum, cmd);
    ਜੇ (ਸੁਨੇਹਾ_silent == 0)
	ਸੁਨੇਹਾ_puts("\n");   // don't overwrite this

    verbose_leave_scroll();
    --no_wait_return;
}

/*
 * Execute a simple command line.  Used for translated commands like "*".
 */
    ਪੂਰਨ
do_cmdline_cmd(char_u *cmd)
{
    ਵਾਪਸ do_cmdline(cmd, ਨਿਰਾ, ਨਿਰਾ,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
}

/*
 * Execute the "+cmd" argument of "edit +cmd fname" and the like.
 * This allows for using a range without ":" in Vim9 script.
 */
    ਸਥਿਰ ਪੂਰਨ
do_cmd_ਦਲੀਲ(char_u *cmd)
{
    ਵਾਪਸ do_cmdline(cmd, ਨਿਰਾ, ਨਿਰਾ,
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
    ਪੂਰਨ
do_cmdline(
    char_u	*cmdline,
    char_u	*(*fgetline)(ਪੂਰਨ, ਖਾਲੀ *, ਪੂਰਨ, getline_opt_T),
    ਖਾਲੀ	*cookie,		// argument for fgetline()
    ਪੂਰਨ		flags)
{
    char_u	*ਅਗਲਾ_cmdline;		// next cmd to execute
    char_u	*cmdline_copy = ਨਿਰਾ;	// copy of cmd line
    ਪੂਰਨ		ਵਰਤਿਆ_getline = FALSE;	// used "fgetline" to obtain command
    ਸਥਿਰ ਪੂਰਨ	recursive = 0;		// recursive depth
    ਪੂਰਨ		ਸੁਨੇਹਾ_didout_before_ਸ਼ੁਰੂ = 0;
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;		// line number count
    ਪੂਰਨ		did_inc_RedrawingDisabled = FALSE;
    ਪੂਰਨ		retval = OK;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    cstack_T	cstack;			// conditional stack
    garray_T	ਸਤਰਾਂ_ga;		// keep lines for ":while"/":for"
    ਪੂਰਨ		current_ਸਤਰ = 0;	// active line in lines_ga
    ਪੂਰਨ		current_ਸਤਰ_before = 0;
    char_u	*fname = ਨਿਰਾ;		// function or script name
    linenr_T	*breakpoint = ਨਿਰਾ;	// ptr to breakpoint field in cookie
    ਪੂਰਨ		*dbg_tick = ਨਿਰਾ;	// ptr to dbg_tick field in cookie
    ਬਣਤਰ dbg_stuff debug_saved;	// saved things for debug mode
    ਪੂਰਨ		initial_trylevel;
    msglist_T	**saved_ਸੁਨੇਹਾ_list = ਨਿਰਾ;
    msglist_T	*ਨਿੱਜੀ_ਸੁਨੇਹਾ_list = ਨਿਰਾ;

    // "fgetline" and "cookie" passed to do_one_cmd()
    char_u	*(*cmd_getline)(ਪੂਰਨ, ਖਾਲੀ *, ਪੂਰਨ, getline_opt_T);
    ਖਾਲੀ	*cmd_cookie;
    ਬਣਤਰ loop_cookie cmd_loop_cookie;
    ਖਾਲੀ	*real_cookie;
    ਪੂਰਨ		getline_is_func;
#ਨਹੀਂਤਾਂ
# define cmd_getline fgetline
# define cmd_cookie cookie
#ਜੇ_ਅੰਤ
    ਸਥਿਰ ਪੂਰਨ	call_ਡੂੰਘਾਈ = 0;		// recursiveness
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // For every pair of do_cmdline()/do_one_cmd() calls, use an extra memory
    // location for storing error messages to be converted to an exception.
    // This ensures that the do_errthrow() call in do_one_cmd() does not
    // combine the messages stored by an earlier invocation of do_one_cmd()
    // with the command name of the later one.  This would happen when
    // BufWritePost autocommands are executed after a write error.
    saved_ਸੁਨੇਹਾ_list = ਸੁਨੇਹਾ_list;
    ਸੁਨੇਹਾ_list = &ਨਿੱਜੀ_ਸੁਨੇਹਾ_list;
#ਜੇ_ਅੰਤ

    // It's possible to create an endless loop with ":execute", catch that
    // here.  The value of 200 allows nested function calls, ":source", etc.
    // Allow 200 or 'maxfuncdepth', whatever is larger.
    ਜੇ (call_ਡੂੰਘਾਈ >= 200
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    && call_ਡੂੰਘਾਈ >= p_mfd
#ਜੇ_ਅੰਤ
	    )
    {
	emsg(_(e_command_too_recursive));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// When converting to an exception, we do not include the command name
	// since this is not an error of the specific command.
	do_errthrow((cstack_T *)ਨਿਰਾ, (char_u *)ਨਿਰਾ);
	ਸੁਨੇਹਾ_list = saved_ਸੁਨੇਹਾ_list;
#ਜੇ_ਅੰਤ
	ਵਾਪਸ FAIL;
    }
    ++call_ਡੂੰਘਾਈ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    CLEAR_ਖੇਤਰ(cstack);
    cstack.cs_idx = -1;
    ga_init2(&ਸਤਰਾਂ_ga, ਆਕਾਰ(wcmd_T), 10);

    real_cookie = getline_cookie(fgetline, cookie);

    // Inside a function use a higher nesting level.
    getline_is_func = getline_equal(fgetline, cookie, get_func_ਸਤਰ);
    ਜੇ (getline_is_func && ex_nesting_level == func_level(real_cookie))
	++ex_nesting_level;

    // Get the function or script name and the address where the next breakpoint
    // line and the debug tick for a function or script are stored.
    ਜੇ (getline_is_func)
    {
	fname = func_name(real_cookie);
	breakpoint = func_breakpoint(real_cookie);
	dbg_tick = func_dbg_tick(real_cookie);
    }
    ਨਹੀਂਤਾਂ ਜੇ (getline_equal(fgetline, cookie, getsourceline))
    {
	fname = SOURCING_NAME;
	breakpoint = ਸਰੋਤ_breakpoint(real_cookie);
	dbg_tick = ਸਰੋਤ_dbg_tick(real_cookie);
    }

    /*
     * Initialize "force_abort"  and "suppress_errthrow" at the top level.
     */
    ਜੇ (!recursive)
    {
	force_abort = FALSE;
	suppress_errthrow = FALSE;
    }

    /*
     * If requested, store and reset the global values controlling the
     * exception handling (used when debugging).  Otherwise clear it to avoid
     * a bogus compiler warning when the optimizer uses inline functions...
     */
    ਜੇ (flags & DOCMD_EXCRESET)
	save_dbg_stuff(&debug_saved);
    ਨਹੀਂਤਾਂ
	CLEAR_ਖੇਤਰ(debug_saved);

    initial_trylevel = trylevel;

    /*
     * "did_throw" will be set to TRUE when an exception is being thrown.
     */
    did_throw = FALSE;
#ਜੇ_ਅੰਤ
    /*
     * "did_emsg" will be set to TRUE when emsg() is used, in which case we
     * cancel the whole command line, and any if/endif or loop.
     * If force_abort is set, we cancel everything.
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    did_emsg_cumul += did_emsg;
#ਜੇ_ਅੰਤ
    did_emsg = FALSE;

    /*
     * KeyTyped is only set when calling vgetc().  Reset it here when not
     * calling vgetc() (sourced command lines).
     */
    ਜੇ (!(flags & DOCMD_KEYTYPED)
			       && !getline_equal(fgetline, cookie, getexline))
	ਕੁੰਜੀ_Typed = FALSE;

    /*
     * Continue executing command lines:
     * - when inside an ":if", ":while" or ":for"
     * - for multiple commands on one line, separated with '|'
     * - when repeating until there are no more lines (for ":source")
     */
    ਅਗਲਾ_cmdline = cmdline;
    ਕਰੋ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	getline_is_func = getline_equal(fgetline, cookie, get_func_ਸਤਰ);
#ਜੇ_ਅੰਤ

	// stop skipping cmds for an error msg after all endif/while/for
	ਜੇ (ਅਗਲਾ_cmdline == ਨਿਰਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		&& !force_abort
		&& cstack.cs_idx < 0
		&& !(getline_is_func && func_has_abort(real_cookie))
#ਜੇ_ਅੰਤ
							)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    did_emsg_cumul += did_emsg;
#ਜੇ_ਅੰਤ
	    did_emsg = FALSE;
	}

	/*
	 * 1. If repeating a line in a loop, get a line from lines_ga.
	 * 2. If no line given: Get an allocated line with fgetline().
	 * 3. If a line is given: Make a copy, so we can mess with it.
	 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// 1. If repeating, get a previous line from lines_ga.
	ਜੇ (cstack.cs_looplevel > 0 && current_ਸਤਰ < ਸਤਰਾਂ_ga.ga_len)
	{
	    // Each '|' separated command is stored separately in lines_ga, to
	    // be able to jump to it.  Don't use next_cmdline now.
	    VIM_CLEAR(cmdline_copy);

	    // Check if a function has returned or, unless it has an unclosed
	    // try conditional, aborted.
	    ਜੇ (getline_is_func)
	    {
# ifdef FEAT_PROFILE
		ਜੇ (do_profiling == PROF_YES)
		    func_ਸਤਰ_ਸਿਰਾ(real_cookie);
# endif
		ਜੇ (func_has_ended(real_cookie))
		{
		    retval = FAIL;
		    ਰੋਕੋ;
		}
	    }
# ifdef FEAT_PROFILE
	    ਨਹੀਂਤਾਂ ਜੇ (do_profiling == PROF_YES
			    && getline_equal(fgetline, cookie, getsourceline))
		script_ਸਤਰ_ਸਿਰਾ();
# endif

	    // Check if a sourced file hit a ":finish" command.
	    ਜੇ (ਸਰੋਤ_ਮੁਕੰਮਲ(fgetline, cookie))
	    {
		retval = FAIL;
		ਰੋਕੋ;
	    }

	    // If breakpoints have been added/deleted need to check for it.
	    ਜੇ (breakpoint != ਨਿਰਾ && dbg_tick != ਨਿਰਾ
						   && *dbg_tick != debug_tick)
	    {
		*breakpoint = dbg_ਲੱਭੋ_breakpoint(
				getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }

	    ਅਗਲਾ_cmdline = ((wcmd_T *)(ਸਤਰਾਂ_ga.ga_ਅੰਕੜੇ))[current_ਸਤਰ].ਸਤਰ;
	    SOURCING_LNUM = ((wcmd_T *)(ਸਤਰਾਂ_ga.ga_ਅੰਕੜੇ))[current_ਸਤਰ].lnum;

	    // Did we encounter a breakpoint?
	    ਜੇ (breakpoint != ਨਿਰਾ && *breakpoint != 0
					      && *breakpoint <= SOURCING_LNUM)
	    {
		dbg_breakpoint(fname, SOURCING_LNUM);
		// Find next breakpoint.
		*breakpoint = dbg_ਲੱਭੋ_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }
# ifdef FEAT_PROFILE
	    ਜੇ (do_profiling == PROF_YES)
	    {
		ਜੇ (getline_is_func)
		    func_ਸਤਰ_ਸ਼ੁਰੂ(real_cookie, SOURCING_LNUM);
		ਨਹੀਂਤਾਂ ਜੇ (getline_equal(fgetline, cookie, getsourceline))
		    script_ਸਤਰ_ਸ਼ੁਰੂ();
	    }
# endif
	}
#ਜੇ_ਅੰਤ

	// 2. If no line given, get an allocated line with fgetline().
	ਜੇ (ਅਗਲਾ_cmdline == ਨਿਰਾ)
	{
	    /*
	     * Need to set msg_didout for the first line after an ":if",
	     * otherwise the ":if" will be overwritten.
	     */
	    ਜੇ (ਗਿਣਤੀ_2 == 1 && getline_equal(fgetline, cookie, getexline))
		ਸੁਨੇਹਾ_didout = TRUE;
	    ਜੇ (fgetline == ਨਿਰਾ || (ਅਗਲਾ_cmdline = fgetline(':', cookie,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    cstack.cs_idx < 0 ? 0 : (cstack.cs_idx + 1) * 2
#ਨਹੀਂਤਾਂ
		    0
#ਜੇ_ਅੰਤ
		    , ਵਿੱਚ_vim9script() ? GETLINE_CONCAT_CONTBAR
					       : GETLINE_CONCAT_CONT)) == ਨਿਰਾ)
	    {
		// Don't call wait_return() for aborted command line.  The NULL
		// returned for the end of a sourced file or executed function
		// doesn't do this.
		ਜੇ (ਕੁੰਜੀ_Typed && !(flags & DOCMD_REPEAT))
		    need_wait_return = FALSE;
		retval = FAIL;
		ਰੋਕੋ;
	    }
	    ਵਰਤਿਆ_getline = TRUE;

	    /*
	     * Keep the first typed line.  Clear it when more lines are typed.
	     */
	    ਜੇ (flags & DOCMD_KEEPLINE)
	    {
		vim_free(repeat_cmdline);
		ਜੇ (ਗਿਣਤੀ_2 == 0)
		    repeat_cmdline = vim_strsave(ਅਗਲਾ_cmdline);
		ਨਹੀਂਤਾਂ
		    repeat_cmdline = ਨਿਰਾ;
	    }
	}

	// 3. Make a copy of the command so we can mess with it.
	ਨਹੀਂਤਾਂ ਜੇ (cmdline_copy == ਨਿਰਾ)
	{
	    ਅਗਲਾ_cmdline = vim_strsave(ਅਗਲਾ_cmdline);
	    ਜੇ (ਅਗਲਾ_cmdline == ਨਿਰਾ)
	    {
		emsg(_(e_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ));
		retval = FAIL;
		ਰੋਕੋ;
	    }
	}
	cmdline_copy = ਅਗਲਾ_cmdline;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
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
	ਜੇ ((cstack.cs_looplevel > 0 || has_loop_cmd(ਅਗਲਾ_cmdline)))
	{
	    cmd_getline = get_loop_ਸਤਰ;
	    cmd_cookie = (ਖਾਲੀ *)&cmd_loop_cookie;
	    cmd_loop_cookie.ਸਤਰਾਂ_gap = &ਸਤਰਾਂ_ga;
	    cmd_loop_cookie.current_ਸਤਰ = current_ਸਤਰ;
	    cmd_loop_cookie.lc_getline = fgetline;
	    cmd_loop_cookie.cookie = cookie;
	    cmd_loop_cookie.repeating = (current_ਸਤਰ < ਸਤਰਾਂ_ga.ga_len);

	    // Save the current line when encountering it the first time.
	    ਜੇ (current_ਸਤਰ == ਸਤਰਾਂ_ga.ga_len
		    && store_loop_ਸਤਰ(&ਸਤਰਾਂ_ga, ਅਗਲਾ_cmdline) == FAIL)
	    {
		retval = FAIL;
		ਰੋਕੋ;
	    }
	    current_ਸਤਰ_before = current_ਸਤਰ;
	}
	ਨਹੀਂਤਾਂ
	{
	    cmd_getline = fgetline;
	    cmd_cookie = cookie;
	}

	did_endif = FALSE;
#ਜੇ_ਅੰਤ

	ਜੇ (ਗਿਣਤੀ_2++ == 0)
	{
	    /*
	     * All output from the commands is put below each other, without
	     * waiting for a return. Don't do this when executing commands
	     * from a script or when being called recursive (e.g. for ":e
	     * +command file").
	     */
	    ਜੇ (!(flags & DOCMD_NOWAIT) && !recursive)
	    {
		ਸੁਨੇਹਾ_didout_before_ਸ਼ੁਰੂ = ਸੁਨੇਹਾ_didout;
		ਸੁਨੇਹਾ_didany = FALSE; // no output yet
		ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
		ਸੁਨੇਹਾ_scroll = TRUE;  // put messages below each other
		++no_wait_return;   // don't wait for return until finished
		++RedrawingDisabled;
		did_inc_RedrawingDisabled = TRUE;
	    }
	}

	ਜੇ ((p_verbose >= 15 && SOURCING_NAME != ਨਿਰਾ) || p_verbose >= 16)
	    ਸੁਨੇਹਾ_verbose_cmd(SOURCING_LNUM, cmdline_copy);

	/*
	 * 2. Execute one '|' separated command.
	 *    do_one_cmd() will return NULL if there is no trailing '|'.
	 *    "cmdline_copy" can change, e.g. for '%' and '#' expansion.
	 */
	++recursive;
	ਅਗਲਾ_cmdline = do_one_cmd(&cmdline_copy, flags,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
				&cstack,
#ਜੇ_ਅੰਤ
				cmd_getline, cmd_cookie);
	--recursive;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (cmd_cookie == (ਖਾਲੀ *)&cmd_loop_cookie)
	    // Use "current_line" from "cmd_loop_cookie", it may have been
	    // incremented when defining a function.
	    current_ਸਤਰ = cmd_loop_cookie.current_ਸਤਰ;
#ਜੇ_ਅੰਤ

	ਜੇ (ਅਗਲਾ_cmdline == ਨਿਰਾ)
	{
	    VIM_CLEAR(cmdline_copy);

	    /*
	     * If the command was typed, remember it for the ':' register.
	     * Do this AFTER executing the command to make :@: work.
	     */
	    ਜੇ (getline_equal(fgetline, cookie, getexline)
						  && new_ਆਖਰੀ_cmdline != ਨਿਰਾ)
	    {
		vim_free(ਆਖਰੀ_cmdline);
		ਆਖਰੀ_cmdline = new_ਆਖਰੀ_cmdline;
		new_ਆਖਰੀ_cmdline = ਨਿਰਾ;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    // need to copy the command after the '|' to cmdline_copy, for the
	    // next do_one_cmd()
	    STRMOVE(cmdline_copy, ਅਗਲਾ_cmdline);
	    ਅਗਲਾ_cmdline = cmdline_copy;
	}


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// reset did_emsg for a function that is not aborted by an error
	ਜੇ (did_emsg && !force_abort
		&& getline_equal(fgetline, cookie, get_func_ਸਤਰ)
					      && !func_has_abort(real_cookie))
	{
	    // did_emsg_cumul is not set here
	    did_emsg = FALSE;
	}

	ਜੇ (cstack.cs_looplevel > 0)
	{
	    ++current_ਸਤਰ;

	    /*
	     * An ":endwhile", ":endfor" and ":continue" is handled here.
	     * If we were executing commands, jump back to the ":while" or
	     * ":for".
	     * If we were not executing commands, decrement cs_looplevel.
	     */
	    ਜੇ (cstack.cs_lflags & (CSL_HAD_CONT | CSL_HAD_ENDLOOP))
	    {
		cstack.cs_lflags &= ~(CSL_HAD_CONT | CSL_HAD_ENDLOOP);

		// Jump back to the matching ":while" or ":for".  Be careful
		// not to use a cs_line[] from an entry that isn't a ":while"
		// or ":for": It would make "current_line" invalid and can
		// cause a crash.
		ਜੇ (!did_emsg && !got_int && !did_throw
			&& cstack.cs_idx >= 0
			&& (cstack.cs_flags[cstack.cs_idx]
						      & (CSF_WHILE | CSF_FOR))
			&& cstack.cs_ਸਤਰ[cstack.cs_idx] >= 0
			&& (cstack.cs_flags[cstack.cs_idx] & CSF_ਸਰਗਰਮ))
		{
		    current_ਸਤਰ = cstack.cs_ਸਤਰ[cstack.cs_idx];
						// remember we jumped there
		    cstack.cs_lflags |= CSL_HAD_LOOP;
		    ਸਤਰ_breakcheck();		// check if CTRL-C typed

		    // Check for the next breakpoint at or after the ":while"
		    // or ":for".
		    ਜੇ (breakpoint != ਨਿਰਾ && ਸਤਰਾਂ_ga.ga_len > current_ਸਤਰ)
		    {
			*breakpoint = dbg_ਲੱਭੋ_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
									fname,
			   ((wcmd_T *)ਸਤਰਾਂ_ga.ga_ਅੰਕੜੇ)[current_ਸਤਰ].lnum-1);
			*dbg_tick = debug_tick;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    // can only get here with ":endwhile" or ":endfor"
		    ਜੇ (cstack.cs_idx >= 0)
			rewind_conditionals(&cstack, cstack.cs_idx - 1,
				   CSF_WHILE | CSF_FOR, &cstack.cs_looplevel);
		}
	    }

	    /*
	     * For a ":while" or ":for" we need to remember the line number.
	     */
	    ਨਹੀਂਤਾਂ ਜੇ (cstack.cs_lflags & CSL_HAD_LOOP)
	    {
		cstack.cs_lflags &= ~CSL_HAD_LOOP;
		cstack.cs_ਸਤਰ[cstack.cs_idx] = current_ਸਤਰ_before;
	    }
	}

	// Check for the next breakpoint after a watchexpression
	ਜੇ (breakpoint != ਨਿਰਾ && has_watchexpr())
	{
	    *breakpoint = dbg_ਲੱਭੋ_breakpoint(FALSE, fname, SOURCING_LNUM);
	    *dbg_tick = debug_tick;
	}

	/*
	 * When not inside any ":while" loop, clear remembered lines.
	 */
	ਜੇ (cstack.cs_looplevel == 0)
	{
	    ਜੇ (ਸਤਰਾਂ_ga.ga_len > 0)
	    {
		SOURCING_LNUM =
		       ((wcmd_T *)ਸਤਰਾਂ_ga.ga_ਅੰਕੜੇ)[ਸਤਰਾਂ_ga.ga_len - 1].lnum;
		free_cmdlines(&ਸਤਰਾਂ_ga);
	    }
	    current_ਸਤਰ = 0;
	}

	/*
	 * A ":finally" makes did_emsg, got_int, and did_throw pending for
	 * being restored at the ":endtry".  Reset them here and set the
	 * ACTIVE and FINALLY flags, so that the finally clause gets executed.
	 * This includes the case where a missing ":endif", ":endwhile" or
	 * ":endfor" was detected by the ":finally" itself.
	 */
	ਜੇ (cstack.cs_lflags & CSL_HAD_FINA)
	{
	    cstack.cs_lflags &= ~CSL_HAD_FINA;
	    report_make_pending(cstack.cs_pending[cstack.cs_idx]
		    & (CSTP_ERROR | CSTP_ਰੁਕਾਵਟ | CSTP_THROW),
		    did_throw ? (ਖਾਲੀ *)current_exception : ਨਿਰਾ);
	    did_emsg = got_int = did_throw = FALSE;
	    cstack.cs_flags[cstack.cs_idx] |= CSF_ਸਰਗਰਮ | CSF_FINALLY;
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
	ਜੇ (trylevel == 0 && !did_emsg && !got_int && !did_throw)
	    force_abort = FALSE;

	// Convert an interrupt to an exception if appropriate.
	(ਖਾਲੀ)do_intthrow(&cstack);
#ਜੇ_ਅੰਤ // FEAT_EVAL

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
    ਜਦੋਂ (!((got_int
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    || (did_emsg && (force_abort || ਵਿੱਚ_vim9script()))
		    || did_throw
#ਜੇ_ਅੰਤ
	     )
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		&& cstack.cs_trylevel == 0
#ਜੇ_ਅੰਤ
	    )
	    && !(did_emsg
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		// Keep going when inside try/catch, so that the error can be
		// deal with, except when it is a syntax error, it may cause
		// the :endtry to be missed.
		&& (cstack.cs_trylevel == 0 || did_emsg_syntax)
#ਜੇ_ਅੰਤ
		&& ਵਰਤਿਆ_getline
			    && (getline_equal(fgetline, cookie, getexmodeline)
			       || getline_equal(fgetline, cookie, getexline)))
	    && (ਅਗਲਾ_cmdline != ਨਿਰਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
			|| cstack.cs_idx >= 0
#ਜੇ_ਅੰਤ
			|| (flags & DOCMD_REPEAT)));

    vim_free(cmdline_copy);
    did_emsg_syntax = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    free_cmdlines(&ਸਤਰਾਂ_ga);
    ga_clear(&ਸਤਰਾਂ_ga);

    ਜੇ (cstack.cs_idx >= 0)
    {
	/*
	 * If a sourced file or executed function ran to its end, report the
	 * unclosed conditional.
	 * In Vim9 script do not give a second error, executing aborts after
	 * the first one.
	 */
	ਜੇ (!got_int && !did_throw && !aborting()
		&& !(did_emsg && ਵਿੱਚ_vim9script())
		&& ((getline_equal(fgetline, cookie, getsourceline)
			&& !ਸਰੋਤ_ਮੁਕੰਮਲ(fgetline, cookie))
		    || (getline_equal(fgetline, cookie, get_func_ਸਤਰ)
					    && !func_has_ended(real_cookie))))
	{
	    ਜੇ (cstack.cs_flags[cstack.cs_idx] & CSF_TRY)
		emsg(_(e_missing_endtry));
	    ਨਹੀਂਤਾਂ ਜੇ (cstack.cs_flags[cstack.cs_idx] & CSF_WHILE)
		emsg(_(e_missing_endwhile));
	    ਨਹੀਂਤਾਂ ਜੇ (cstack.cs_flags[cstack.cs_idx] & CSF_FOR)
		emsg(_(e_missing_endfor));
	    ਨਹੀਂਤਾਂ
		emsg(_(e_missing_endif));
	}

	/*
	 * Reset "trylevel" in case of a ":finish" or ":return" or a missing
	 * ":endtry" in a sourced file or executed function.  If the try
	 * conditional is in its finally clause, ignore anything pending.
	 * If it is in a catch clause, finish the caught exception.
	 * Also cleanup any "cs_forinfo" structures.
	 */
	ਕਰੋ
	{
	    ਪੂਰਨ idx = cleanup_conditionals(&cstack, 0, TRUE);

	    ਜੇ (idx >= 0)
		--idx;	    // remove try block not in its finally clause
	    rewind_conditionals(&cstack, idx, CSF_WHILE | CSF_FOR,
							&cstack.cs_looplevel);
	}
	ਜਦੋਂ (cstack.cs_idx >= 0);
	trylevel = initial_trylevel;
    }

    // If a missing ":endtry", ":endwhile", ":endfor", or ":endif" or a memory
    // lack was reported above and the error message is to be converted to an
    // exception, do this now after rewinding the cstack.
    do_errthrow(&cstack, getline_equal(fgetline, cookie, get_func_ਸਤਰ)
				  ? (char_u *)"endfunction" : (char_u *)ਨਿਰਾ);

    ਜੇ (trylevel == 0)
    {
	// Just in case did_throw got set but current_exception wasn't.
	ਜੇ (current_exception == ਨਿਰਾ)
	    did_throw = FALSE;

	/*
	 * When an exception is being thrown out of the outermost try
	 * conditional, discard the uncaught exception, disable the conversion
	 * of interrupts or errors to exceptions, and ensure that no more
	 * commands are executed.
	 */
	ਜੇ (did_throw)
	    handle_did_throw();

	/*
	 * On an interrupt or an aborting error not converted to an exception,
	 * disable the conversion of errors to exceptions.  (Interrupts are not
	 * converted anymore, here.) This enables also the interrupt message
	 * when force_abort is set and did_emsg unset in case of an interrupt
	 * from a finally clause after an error.
	 */
	ਨਹੀਂਤਾਂ ਜੇ (got_int || (did_emsg && force_abort))
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
    ਜੇ (did_throw)
	need_rethrow = TRUE;
    ਜੇ ((getline_equal(fgetline, cookie, getsourceline)
		&& ex_nesting_level > ਸਰੋਤ_level(real_cookie))
	    || (getline_equal(fgetline, cookie, get_func_ਸਤਰ)
		&& ex_nesting_level > func_level(real_cookie) + 1))
    {
	ਜੇ (!did_throw)
	    check_cstack = TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
	// When leaving a function, reduce nesting level.
	ਜੇ (getline_equal(fgetline, cookie, get_func_ਸਤਰ))
	    --ex_nesting_level;
	/*
	 * Go to debug mode when returning from a function in which we are
	 * single-stepping.
	 */
	ਜੇ ((getline_equal(fgetline, cookie, getsourceline)
		    || getline_equal(fgetline, cookie, get_func_ਸਤਰ))
		&& ex_nesting_level + 1 <= debug_break_level)
	    do_debug(getline_equal(fgetline, cookie, getsourceline)
		    ? (char_u *)_("End of sourced file")
		    : (char_u *)_("End of function"));
    }

    /*
     * Restore the exception environment (done after returning from the
     * debugger).
     */
    ਜੇ (flags & DOCMD_EXCRESET)
	restore_dbg_stuff(&debug_saved);

    ਸੁਨੇਹਾ_list = saved_ਸੁਨੇਹਾ_list;

    // Cleanup if "cs_emsg_silent_list" remains.
    ਜੇ (cstack.cs_emsg_silent_list != ਨਿਰਾ)
    {
	eslist_T *elem, *ਆਰਜ਼ੀ;

	ਲਈ (elem = cstack.cs_emsg_silent_list; elem != ਨਿਰਾ; elem = ਆਰਜ਼ੀ)
	{
	    ਆਰਜ਼ੀ = elem->ਅਗਲਾ;
	    vim_free(elem);
	}
    }
#ਜੇ_ਅੰਤ // FEAT_EVAL

    /*
     * If there was too much output to fit on the command line, ask the user to
     * hit return before redrawing the screen. With the ":global" command we do
     * this only once after the command is finished.
     */
    ਜੇ (did_inc_RedrawingDisabled)
    {
	ਜੇ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	--no_wait_return;
	ਸੁਨੇਹਾ_scroll = FALSE;

	/*
	 * When just finished an ":if"-":else" which was typed, no need to
	 * wait for hit-return.  Also for an error situation.
	 */
	ਜੇ (retval == FAIL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		|| (did_endif && ਕੁੰਜੀ_Typed && !did_emsg)
#ਜੇ_ਅੰਤ
					    )
	{
	    need_wait_return = FALSE;
	    ਸੁਨੇਹਾ_didany = FALSE;		// don't wait when restarting edit
	}
	ਨਹੀਂਤਾਂ ਜੇ (need_wait_return)
	{
	    /*
	     * The msg_start() above clears msg_didout. The wait_return() we do
	     * here should not overwrite the command that may be shown before
	     * doing that.
	     */
	    ਸੁਨੇਹਾ_didout |= ਸੁਨੇਹਾ_didout_before_ਸ਼ੁਰੂ;
	    wait_return(FALSE);
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    did_endif = FALSE;  // in case do_cmdline used recursively
#ਨਹੀਂਤਾਂ
    /*
     * Reset if_level, in case a sourced script file contains more ":if" than
     * ":endif" (could be ":if x | foo | endif").
     */
    if_level = 0;
#ਜੇ_ਅੰਤ

    --call_ਡੂੰਘਾਈ;
    ਵਾਪਸ retval;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Handle when "did_throw" is set after executing commands.
 */
    ਖਾਲੀ
handle_did_throw(ਖਾਲੀ)
{
    ਅੱਖਰ	*p = ਨਿਰਾ;
    msglist_T	*messages = ਨਿਰਾ;
    ESTACK_CHECK_DECLARATION;

    /*
     * If the uncaught exception is a user exception, report it as an
     * error.  If it is an error exception, display the saved error
     * message now.  For an interrupt exception, do nothing; the
     * interrupt message is given elsewhere.
     */
    ਚੋਣ (current_exception->type)
    {
	ਹਾਲਤ ET_USER:
	    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
		    _(e_exception_not_caught_str),
		    current_exception->ਮੁੱਲ);
	    p = (ਅੱਖਰ *)vim_strsave(IObuff);
	    ਰੋਕੋ;
	ਹਾਲਤ ET_ERROR:
	    messages = current_exception->messages;
	    current_exception->messages = ਨਿਰਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ ET_ਰੁਕਾਵਟ:
	    ਰੋਕੋ;
    }

    estack_ਧੱਕੋ(ETYPE_EXCEPT, current_exception->throw_name,
					current_exception->throw_lnum);
    ESTACK_CHECK_ਤਿਆਰੀ;
    current_exception->throw_name = ਨਿਰਾ;

    discard_current_exception();	// uses IObuff if 'verbose'

    // If "silent!" is active the uncaught exception is not fatal.
    ਜੇ (emsg_silent == 0)
    {
	suppress_errthrow = TRUE;
	force_abort = TRUE;
    }

    ਜੇ (messages != ਨਿਰਾ)
    {
	ਕਰੋ
	{
	    msglist_T	*ਅਗਲਾ = messages->ਅਗਲਾ;
	    ਪੂਰਨ		save_compiling = estack_compiling;

	    estack_compiling = messages->ਸੁਨੇਹਾ_compiling;
	    emsg(messages->ਸੁਨੇਹਾ);
	    vim_free(messages->ਸੁਨੇਹਾ);
	    vim_free(messages->sfile);
	    vim_free(messages);
	    messages = ਅਗਲਾ;
	    estack_compiling = save_compiling;
	}
	ਜਦੋਂ (messages != ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (p != ਨਿਰਾ)
    {
	emsg(p);
	vim_free(p);
    }
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_ਕੱਢੋ();
}

/*
 * Obtain a line when inside a ":while" or ":for" loop.
 */
    ਸਥਿਰ char_u *
get_loop_ਸਤਰ(ਪੂਰਨ c, ਖਾਲੀ *cookie, ਪੂਰਨ indent, getline_opt_T options)
{
    ਬਣਤਰ loop_cookie	*cp = (ਬਣਤਰ loop_cookie *)cookie;
    wcmd_T		*wp;
    char_u		*ਸਤਰ;

    ਜੇ (cp->current_ਸਤਰ + 1 >= cp->ਸਤਰਾਂ_gap->ga_len)
    {
	ਜੇ (cp->repeating)
	    ਵਾਪਸ ਨਿਰਾ;	// trying to read past ":endwhile"/":endfor"

	// First time inside the ":while"/":for": get line normally.
	ਜੇ (cp->lc_getline == ਨਿਰਾ)
	    ਸਤਰ = getcmdline(c, 0L, indent, 0);
	ਨਹੀਂਤਾਂ
	    ਸਤਰ = cp->lc_getline(c, cp->cookie, indent, options);
	ਜੇ (ਸਤਰ != ਨਿਰਾ && store_loop_ਸਤਰ(cp->ਸਤਰਾਂ_gap, ਸਤਰ) == OK)
	    ++cp->current_ਸਤਰ;

	ਵਾਪਸ ਸਤਰ;
    }

    ਕੁੰਜੀ_Typed = FALSE;
    ++cp->current_ਸਤਰ;
    wp = (wcmd_T *)(cp->ਸਤਰਾਂ_gap->ga_ਅੰਕੜੇ) + cp->current_ਸਤਰ;
    SOURCING_LNUM = wp->lnum;
    ਵਾਪਸ vim_strsave(wp->ਸਤਰ);
}

/*
 * Store a line in "gap" so that a ":while" loop can execute it again.
 */
    ਸਥਿਰ ਪੂਰਨ
store_loop_ਸਤਰ(garray_T *gap, char_u *ਸਤਰ)
{
    ਜੇ (ga_grow(gap, 1) == FAIL)
	ਵਾਪਸ FAIL;
    ((wcmd_T *)(gap->ga_ਅੰਕੜੇ))[gap->ga_len].ਸਤਰ = vim_strsave(ਸਤਰ);
    ((wcmd_T *)(gap->ga_ਅੰਕੜੇ))[gap->ga_len].lnum = SOURCING_LNUM;
    ++gap->ga_len;
    ਵਾਪਸ OK;
}

/*
 * Free the lines stored for a ":while" or ":for" loop.
 */
    ਸਥਿਰ ਖਾਲੀ
free_cmdlines(garray_T *gap)
{
    ਜਦੋਂ (gap->ga_len > 0)
    {
	vim_free(((wcmd_T *)(gap->ga_ਅੰਕੜੇ))[gap->ga_len - 1].ਸਤਰ);
	--gap->ga_len;
    }
}
#ਜੇ_ਅੰਤ

/*
 * If "fgetline" is get_loop_line(), return TRUE if the getline it uses equals
 * "func".  * Otherwise return TRUE when "fgetline" equals "func".
 */
    ਪੂਰਨ
getline_equal(
    char_u	*(*fgetline)(ਪੂਰਨ, ਖਾਲੀ *, ਪੂਰਨ, getline_opt_T),
    ਖਾਲੀ	*cookie ਅਣਵਰਤਿਆ,		// argument for fgetline()
    char_u	*(*func)(ਪੂਰਨ, ਖਾਲੀ *, ਪੂਰਨ, getline_opt_T))
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    char_u		*(*gp)(ਪੂਰਨ, ਖਾਲੀ *, ਪੂਰਨ, getline_opt_T);
    ਬਣਤਰ loop_cookie *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // function that's originally used to obtain the lines.  This may be
    // nested several levels.
    gp = fgetline;
    cp = (ਬਣਤਰ loop_cookie *)cookie;
    ਜਦੋਂ (gp == get_loop_ਸਤਰ)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    ਵਾਪਸ gp == func;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ fgetline == func;
#ਜੇ_ਅੰਤ
}

/*
 * If "fgetline" is get_loop_line(), return the cookie used by the original
 * getline function.  Otherwise return "cookie".
 */
    ਖਾਲੀ *
getline_cookie(
    char_u	*(*fgetline)(ਪੂਰਨ, ਖਾਲੀ *, ਪੂਰਨ, getline_opt_T) ਅਣਵਰਤਿਆ,
    ਖਾਲੀ	*cookie)		// argument for fgetline()
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    char_u		*(*gp)(ਪੂਰਨ, ਖਾਲੀ *, ਪੂਰਨ, getline_opt_T);
    ਬਣਤਰ loop_cookie  *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (ਬਣਤਰ loop_cookie *)cookie;
    ਜਦੋਂ (gp == get_loop_ਸਤਰ)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    ਵਾਪਸ cp;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ cookie;
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Get the next line source line without advancing.
 */
    char_u *
getline_peek(
    char_u	*(*fgetline)(ਪੂਰਨ, ਖਾਲੀ *, ਪੂਰਨ, getline_opt_T) ਅਣਵਰਤਿਆ,
    ਖਾਲੀ	*cookie)		// argument for fgetline()
{
    char_u		*(*gp)(ਪੂਰਨ, ਖਾਲੀ *, ਪੂਰਨ, getline_opt_T);
    ਬਣਤਰ loop_cookie  *cp;
    wcmd_T		*wp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (ਬਣਤਰ loop_cookie *)cookie;
    ਜਦੋਂ (gp == get_loop_ਸਤਰ)
    {
	ਜੇ (cp->current_ਸਤਰ + 1 < cp->ਸਤਰਾਂ_gap->ga_len)
	{
	    // executing lines a second time, use the stored copy
	    wp = (wcmd_T *)(cp->ਸਤਰਾਂ_gap->ga_ਅੰਕੜੇ) + cp->current_ਸਤਰ + 1;
	    ਵਾਪਸ wp->ਸਤਰ;
	}
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    ਜੇ (gp == getsourceline)
	ਵਾਪਸ ਸਰੋਤ_nextline(cp);
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ


/*
 * Helper function to apply an offset for buffer commands, i.e. ":bdelete",
 * ":bwipeout", etc.
 * Returns the buffer number.
 */
    ਸਥਿਰ ਪੂਰਨ
compute_ਬਫਰ_local_ਗਿਣਤੀ(ਪੂਰਨ addr_type, ਪੂਰਨ lnum, ਪੂਰਨ ਵਿੱਥ)
{
    buf_T   *buf;
    buf_T   *nextbuf;
    ਪੂਰਨ     ਗਿਣਤੀ_2 = ਵਿੱਥ;

    buf = firstbuf;
    ਜਦੋਂ (buf->b_ਅਗਲਾ != ਨਿਰਾ && buf->b_fnum < lnum)
	buf = buf->b_ਅਗਲਾ;
    ਜਦੋਂ (ਗਿਣਤੀ_2 != 0)
    {
	ਗਿਣਤੀ_2 += (ਵਿੱਥ < 0) ? 1 : -1;
	nextbuf = (ਵਿੱਥ < 0) ? buf->b_prev : buf->b_ਅਗਲਾ;
	ਜੇ (nextbuf == ਨਿਰਾ)
	    ਰੋਕੋ;
	buf = nextbuf;
	ਜੇ (addr_type == ADDR_LOADED_BUFFERS)
	    // skip over unloaded buffers
	    ਜਦੋਂ (buf->b_ml.ml_mfp == ਨਿਰਾ)
	    {
		nextbuf = (ਵਿੱਥ < 0) ? buf->b_prev : buf->b_ਅਗਲਾ;
		ਜੇ (nextbuf == ਨਿਰਾ)
		    ਰੋਕੋ;
		buf = nextbuf;
	    }
    }
    // we might have gone too far, last buffer is not loadedd
    ਜੇ (addr_type == ADDR_LOADED_BUFFERS)
	ਜਦੋਂ (buf->b_ml.ml_mfp == ਨਿਰਾ)
	{
	    nextbuf = (ਵਿੱਥ >= 0) ? buf->b_prev : buf->b_ਅਗਲਾ;
	    ਜੇ (nextbuf == ਨਿਰਾ)
		ਰੋਕੋ;
	    buf = nextbuf;
	}
    ਵਾਪਸ buf->b_fnum;
}

/*
 * Return the window number of "win".
 * When "win" is NULL return the number of windows.
 */
    ਸਥਿਰ ਪੂਰਨ
current_win_nr(win_T *win)
{
    win_T	*wp;
    ਪੂਰਨ		nr = 0;

    FOR_ALL_WINDOWS(wp)
    {
	++nr;
	ਜੇ (wp == win)
	    ਰੋਕੋ;
    }
    ਵਾਪਸ nr;
}

    ਸਥਿਰ ਪੂਰਨ
current_tab_nr(tabpage_T *tab)
{
    tabpage_T	*tp;
    ਪੂਰਨ		nr = 0;

    FOR_ALL_TABPAGES(tp)
    {
	++nr;
	ਜੇ (tp == tab)
	    ਰੋਕੋ;
    }
    ਵਾਪਸ nr;
}

    ਸਥਿਰ ਪੂਰਨ
comment_ਸ਼ੁਰੂ(char_u *p, ਪੂਰਨ starts_with_colon ਅਣਵਰਤਿਆ)
{
    ਜੇ (ਵਿੱਚ_vim9script())
	ਵਾਪਸ p[0] == '#' && !starts_with_colon;
    ਵਾਪਸ *p == '"';
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURRENT_WIN_NR current_win_nr(curwin)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਆਖਰੀ_WIN_NR current_win_nr(ਨਿਰਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURRENT_TAB_NR current_tab_nr(curtab)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਆਖਰੀ_TAB_NR current_tab_nr(ਨਿਰਾ)

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
    ਸਥਿਰ char_u *
do_one_cmd(
    char_u	**cmdlinep,
    ਪੂਰਨ		flags,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    cstack_T	*cstack,
#ਜੇ_ਅੰਤ
    char_u	*(*fgetline)(ਪੂਰਨ, ਖਾਲੀ *, ਪੂਰਨ, getline_opt_T),
    ਖਾਲੀ	*cookie)		// argument for fgetline()
{
    char_u	*p;
    linenr_T	lnum;
    ਲੰਮਾ	n;
    ਅੱਖਰ	*errormsg = ਨਿਰਾ;	// error message
    char_u	*after_modifier = ਨਿਰਾ;
    exarg_T	ea;			// Ex command arguments
    cmdmod_T	save_cmdmod;
    ਪੂਰਨ		save_reg_executing = reg_executing;
    ਪੂਰਨ		save_pending_ਸਿਰਾ_reg_executing = pending_ਸਿਰਾ_reg_executing;
    ਪੂਰਨ		ni;			// set when Not Implemented
    char_u	*cmd;
    ਪੂਰਨ		starts_with_colon = FALSE;
    ਪੂਰਨ		may_have_range;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਪੂਰਨ		did_ਸੈੱਟ_ਵਿਅੰਜਕ_ਸਤਰ = FALSE;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		sourcing = flags & DOCMD_VERBOSE;
    ਪੂਰਨ		did_append_cmd = FALSE;

    CLEAR_ਖੇਤਰ(ea);
    ea.line1 = 1;
    ea.line2 = 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ++ex_nesting_level;
#ਜੇ_ਅੰਤ

    // When the last file has not been edited :q has to be typed twice.
    ਜੇ (quitmore
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    // avoid that a function call in 'statusline' does this
	    && !getline_equal(fgetline, cookie, get_func_ਸਤਰ)
#ਜੇ_ਅੰਤ
	    // avoid that an autocommand, e.g. QuitPre, does this
	    && !getline_equal(fgetline, cookie, getnextac))
	--quitmore;

    /*
     * Reset browse, confirm, etc..  They are restored when returning, for
     * recursive calls.
     */
    save_cmdmod = cmdmod;

    // "#!anything" is handled like a comment.
    ਜੇ ((*cmdlinep)[0] == '#' && (*cmdlinep)[1] == '!')
	ਜਾਓ doend;

/*
 * 1. Skip comment lines and leading white space and colons.
 * 2. Handle command modifiers.
 */
    // The "ea" structure holds the arguments that can be used.
    ea.cmd = *cmdlinep;
    ea.cmdlinep = cmdlinep;
    ea.ea_getline = fgetline;
    ea.cookie = cookie;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ea.cstack = cstack;
    starts_with_colon = *skipwhite(ea.cmd) == ':';
#ਜੇ_ਅੰਤ
    ਜੇ (parse_command_modifiers(&ea, &errormsg, &cmdmod, FALSE) == FAIL)
	ਜਾਓ doend;
    apply_cmdmod(&cmdmod);
    after_modifier = ea.cmd;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ea.skip = did_emsg || got_int || did_throw || (cstack->cs_idx >= 0
			 && !(cstack->cs_flags[cstack->cs_idx] & CSF_ਸਰਗਰਮ));
#ਨਹੀਂਤਾਂ
    ea.skip = (if_level > 0);
#ਜੇ_ਅੰਤ

/*
 * 3. Skip over the range to find the command.  Let "p" point to after it.
 *
 * We need the command to know what kind of range it uses.
 */
    cmd = ea.cmd;

    // In Vim9 script a colon is required before the range.  This may also be
    // after command modifiers.
    ਪੂਰਨ vim9script = ਵਿੱਚ_vim9script();
    ਜੇ (vim9script && (flags & DOCMD_RANGEOK) == 0)
    {
	may_have_range = FALSE;
	ਲਈ (p = ea.cmd; p >= *cmdlinep; --p)
	{
	    ਜੇ (*p == ':')
		may_have_range = TRUE;
	    ਜੇ (p < ea.cmd && !VIM_ISWHITE(*p))
		ਰੋਕੋ;
	}
    }
    ਨਹੀਂਤਾਂ
	may_have_range = TRUE;
    ਜੇ (may_have_range)
	ea.cmd = skip_range(ea.cmd, TRUE, ਨਿਰਾ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // Handle ":export" - it functions almost like a command modifier.
    // ":export var Name: type"
    // ":export def Name(..."
    // etc.
    ਜੇ (vim9script && checkforcmd_noparen(&ea.cmd, "export", 6))
	is_export = TRUE;
#ਜੇ_ਅੰਤ

    ਜੇ (vim9script && !may_have_range)
    {
	ਜੇ (ea.cmd == cmd + 1 && *cmd == '$')
	    // should be "$VAR = val"
	    --ea.cmd;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	p = ਲੱਭੋ_ex_command(&ea, ਨਿਰਾ, lookup_scriptitem, ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
	p = ਲੱਭੋ_ex_command(&ea, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
	ਜੇ (ea.cmdidx == CMD_ਮਾਪ)
	{
	    char_u *ar = skip_range(ea.cmd, TRUE, ਨਿਰਾ);

	    // If a ':' before the range is missing, give a clearer error
	    // message.
	    ਜੇ (ar > ea.cmd && !ea.skip)
	    {
		semsg(_(e_colon_required_before_range_str), ea.cmd);
		ਜਾਓ doend;
	    }
	}
    }
    ਨਹੀਂਤਾਂ
	p = ਲੱਭੋ_ex_command(&ea, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
# ifdef FEAT_PROFILE
    // Count this line for profiling if skip is TRUE.
    ਜੇ (do_profiling == PROF_YES
	    && (!ea.skip || cstack->cs_idx == 0 || (cstack->cs_idx > 0
		     && (cstack->cs_flags[cstack->cs_idx - 1] & CSF_ਸਰਗਰਮ))))
    {
	ਪੂਰਨ skip = did_emsg || got_int || did_throw;

	ਜੇ (ea.cmdidx == CMD_catch)
	    skip = !skip && !(cstack->cs_idx >= 0
			  && (cstack->cs_flags[cstack->cs_idx] & CSF_THROWN)
			  && !(cstack->cs_flags[cstack->cs_idx] & CSF_CAUGHT));
	ਨਹੀਂਤਾਂ ਜੇ (ea.cmdidx == CMD_else || ea.cmdidx == CMD_elseif)
	    skip = skip || !(cstack->cs_idx >= 0
			  && !(cstack->cs_flags[cstack->cs_idx]
						  & (CSF_ਸਰਗਰਮ | CSF_TRUE)));
	ਨਹੀਂਤਾਂ ਜੇ (ea.cmdidx == CMD_finally)
	    skip = FALSE;
	ਨਹੀਂਤਾਂ ਜੇ (ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endtry
		&& ea.cmdidx != CMD_endwhile)
	    skip = ea.skip;

	ਜੇ (!skip)
	{
	    ਜੇ (getline_equal(fgetline, cookie, get_func_ਸਤਰ))
		func_ਸਤਰ_exec(getline_cookie(fgetline, cookie));
	    ਨਹੀਂਤਾਂ ਜੇ (getline_equal(fgetline, cookie, getsourceline))
		script_ਸਤਰ_exec();
	}
    }
# endif
#ਜੇ_ਅੰਤ

    ea.cmd = cmd;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // May go to debug mode.  If this happens and the ">quit" debug command is
    // used, throw an interrupt exception and skip the next command.
    dbg_check_breakpoint(&ea);
    ਜੇ (!ea.skip && got_int)
    {
	ea.skip = TRUE;
	(ਖਾਲੀ)do_intthrow(cstack);
    }
#ਜੇ_ਅੰਤ

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
    ਜੇ (!IS_USER_CMDIDX(ea.cmdidx))
    {
	ਜੇ (ea.cmdidx != CMD_ਮਾਪ)
	    ea.addr_type = cmdnames[(ਪੂਰਨ)ea.cmdidx].cmd_addr_type;
	ਨਹੀਂਤਾਂ
	    ea.addr_type = ADDR_ਸਤਰਾਂ;

	// :wincmd range depends on the argument.
	ਜੇ (ea.cmdidx == CMD_wincmd && p != ਨਿਰਾ)
	    get_wincmd_addr_type(skipwhite(p), &ea);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
	// :.cc in quickfix window uses line number
	ਜੇ ((ea.cmdidx == CMD_cc || ea.cmdidx == CMD_ll) && bt_quickfix(curbuf))
	    ea.addr_type = ADDR_OTHER;
#ਜੇ_ਅੰਤ
    }

    ਜੇ (!may_have_range)
	ea.line1 = ea.line2 = default_ਪਤਾ(&ea);
    ਨਹੀਂਤਾਂ ਜੇ (parse_cmd_ਪਤਾ(&ea, &errormsg, FALSE) == FAIL)
	ਜਾਓ doend;

/*
 * 5. Parse the command.
 */

    /*
     * Skip ':' and any white space
     */
    ea.cmd = skipwhite(ea.cmd);
    ਜਦੋਂ (*ea.cmd == ':')
	ea.cmd = skipwhite(ea.cmd + 1);

    /*
     * If we got a line, but no command, then go to the line.
     * If we find a '|' or '\n' we set ea.nextcmd.
     */
    ਜੇ (*ea.cmd == NUL || comment_ਸ਼ੁਰੂ(ea.cmd, starts_with_colon)
			       || (ea.nextcmd = check_nextcmd(ea.cmd)) != ਨਿਰਾ)
    {
	/*
	 * strange vi behaviour:
	 * ":3"		jumps to line 3
	 * ":3|..."	prints line 3  (not in Vim9 script)
	 * ":|"		prints current line  (not in Vim9 script)
	 */
	ਜੇ (ea.skip)	    // skip this if inside :if
	    ਜਾਓ doend;
	errormsg = ex_range_without_command(&ea);
	ਜਾਓ doend;
    }

    // If this looks like an undefined user command and there are CmdUndefined
    // autocommands defined, trigger the matching autocommands.
    ਜੇ (p != ਨਿਰਾ && ea.cmdidx == CMD_ਮਾਪ && !ea.skip
	    && ASCII_ISUPPER(*ea.cmd)
	    && has_cmdundefined())
    {
	ਪੂਰਨ ret;

	p = ea.cmd;
	ਜਦੋਂ (ASCII_ISALNUM(*p))
	    ++p;
	p = vim_strnsave(ea.cmd, p - ea.cmd);
	ret = apply_autocmds(EVENT_CMDUNDEFINED, p, p, TRUE, ਨਿਰਾ);
	vim_free(p);
	// If the autocommands did something and didn't cause an error, try
	// finding the command again.
	p = (ret
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		&& !aborting()
#ਜੇ_ਅੰਤ
		) ? ਲੱਭੋ_ex_command(&ea, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ) : ea.cmd;
    }

    ਜੇ (p == ਨਿਰਾ)
    {
	ਜੇ (!ea.skip)
	    errormsg = _(e_ambiguous_use_ਦਾ_user_defined_command);
	ਜਾਓ doend;
    }
    // Check for wrong commands.
    ਜੇ (*p == '!' && ea.cmd[1] == 0151 && ea.cmd[0] == 78
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	errormsg = uc_fun_cmd();
	ਜਾਓ doend;
    }

    ਜੇ (ea.cmdidx == CMD_ਮਾਪ)
    {
	ਜੇ (!ea.skip)
	{
	    STRCPY(IObuff, _(e_not_an_editor_command));
	    ਜੇ (!sourcing)
	    {
		// If the modifier was parsed OK the error must be in the
		// following command
		ਜੇ (after_modifier != ਨਿਰਾ)
		    append_command(after_modifier);
		ਨਹੀਂਤਾਂ
		    append_command(*cmdlinep);
		did_append_cmd = TRUE;
	    }
	    errormsg = (ਅੱਖਰ *)IObuff;
	    did_emsg_syntax = TRUE;
	}
	ਜਾਓ doend;
    }

    ni = (!IS_USER_CMDIDX(ea.cmdidx)
	    && (cmdnames[ea.cmdidx].cmd_func == ex_ni
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_EX_SCRIPT_NI
	     || cmdnames[ea.cmdidx].cmd_func == ex_script_ni
#ਜੇ_ਅੰਤ
	     ));

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    /*
     * When the expression evaluation is disabled, recognize the ":if" and
     * ":endif" commands and ignore everything in between it.
     */
    ਜੇ (ea.cmdidx == CMD_if)
	++if_level;
    ਜੇ (if_level)
    {
	ਜੇ (ea.cmdidx == CMD_endif)
	    --if_level;
	ਜਾਓ doend;
    }

#ਜੇ_ਅੰਤ

    // forced commands
    ਜੇ (*p == '!' && ea.cmdidx != CMD_substitute
	    && ea.cmdidx != CMD_smagic && ea.cmdidx != CMD_snomagic)
    {
	++p;
	ea.forceit = TRUE;
    }
    ਨਹੀਂਤਾਂ
	ea.forceit = FALSE;

/*
 * 6. Parse arguments.  Then check for errors.
 */
    ਜੇ (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = (ਲੰਮਾ)cmdnames[(ਪੂਰਨ)ea.cmdidx].cmd_argt;

    ਜੇ (!ea.skip)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
	ਜੇ (sandbox != 0 && !(ea.argt & EX_SBOXOK))
	{
	    // Command not allowed in sandbox.
	    errormsg = _(e_not_allowed_ਵਿੱਚ_sandbox);
	    ਜਾਓ doend;
	}
#ਜੇ_ਅੰਤ
	ਜੇ (restricted != 0 && (ea.argt & EX_RESTRICT))
	{
	    errormsg = _(e_command_not_allowed_ਵਿੱਚ_rvim);
	    ਜਾਓ doend;
	}
	ਜੇ (!curbuf->b_p_ma && (ea.argt & EX_MODIFY))
	{
	    // Command not allowed in non-'modifiable' buffer
	    errormsg = _(e_cannot_make_changes_modifiable_is_off);
	    ਜਾਓ doend;
	}

	ਜੇ (!IS_USER_CMDIDX(ea.cmdidx))
	{
	    ਜੇ (cmdwin_type != 0 && !(ea.argt & EX_CMDWIN))
	    {
		// Command not allowed in the command line window
		errormsg = _(e_invalid_ਵਿੱਚ_cmdline_ਖਿੜਕੀ);
		ਜਾਓ doend;
	    }
	    ਜੇ (ਪਾਠ_locked() && !(ea.argt & EX_ਤਾਲਾ_OK))
	    {
		// Command not allowed when text is locked
		errormsg = _(get_ਪਾਠ_locked_ਸੁਨੇਹਾ());
		ਜਾਓ doend;
	    }
	}

	// Disallow editing another buffer when "curbuf_lock" is set.
	// Do allow ":checktime" (it is postponed).
	// Do allow ":edit" (check for an argument later).
	// Do allow ":file" with no arguments (check for an argument later).
	ਜੇ (!(ea.argt & (EX_CMDWIN | EX_ਤਾਲਾ_OK))
		&& ea.cmdidx != CMD_checktime
		&& ea.cmdidx != CMD_edit
		&& ea.cmdidx != CMD_file
		&& !IS_USER_CMDIDX(ea.cmdidx)
		&& curbuf_locked())
	    ਜਾਓ doend;

	ਜੇ (!ni && !(ea.argt & EX_RANGE) && ea.addr_ਗਿਣਤੀ > 0)
	{
	    errormsg = _(e_no_range_allowed);
	    ਜਾਓ doend;
	}
    }

    ਜੇ (!ni && !(ea.argt & EX_BANG) && ea.forceit)
    {
	errormsg = _(e_no_bang_allowed);
	ਜਾਓ doend;
    }

    /*
     * Don't complain about the range if it is not used
     * (could happen if line_count is accidentally set to 0).
     */
    ਜੇ (!ea.skip && !ni && (ea.argt & EX_RANGE))
    {
	/*
	 * If the range is backwards, ask for confirmation and, if given, swap
	 * ea.line1 & ea.line2 so it's forwards again.
	 * When global command is busy, don't ask, will fail below.
	 */
	ਜੇ (!global_busy && ea.line1 > ea.line2)
	{
	    ਜੇ (ਸੁਨੇਹਾ_silent == 0)
	    {
		ਜੇ (sourcing || exmode_ਸਰਗਰਮ)
		{
		    errormsg = _(e_backwards_range_given);
		    ਜਾਓ doend;
		}
		ਜੇ (ask_yesno((char_u *)
			_("Backwards range given, OK to swap"), FALSE) != 'y')
		    ਜਾਓ doend;
	    }
	    lnum = ea.line1;
	    ea.line1 = ea.line2;
	    ea.line2 = lnum;
	}
	ਜੇ ((errormsg = invalid_range(&ea)) != ਨਿਰਾ)
	    ਜਾਓ doend;
    }

    ਜੇ ((ea.addr_type == ADDR_OTHER) && ea.addr_ਗਿਣਤੀ == 0)
	// default is 1, not cursor
	ea.line2 = 1;

    correct_range(&ea);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ (((ea.argt & EX_WHOLEFOLD) || ea.addr_ਗਿਣਤੀ >= 2) && !global_busy
	    && ea.addr_type == ADDR_ਸਤਰਾਂ)
    {
	// Put the first line at the start of a closed fold, put the last line
	// at the end of a closed fold.
	(ਖਾਲੀ)hasFolding(ea.line1, &ea.line1, ਨਿਰਾ);
	(ਖਾਲੀ)hasFolding(ea.line2, ਨਿਰਾ, &ea.line2);
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    /*
     * For the ":make" and ":grep" commands we insert the 'makeprg'/'grepprg'
     * option here, so things like % get expanded.
     */
    p = replace_makeprg(&ea, p, cmdlinep);
    ਜੇ (p == ਨਿਰਾ)
	ਜਾਓ doend;
#ਜੇ_ਅੰਤ

    /*
     * Skip to start of argument.
     * Don't do this for the ":!" command, because ":!! -l" needs the space.
     */
    ਜੇ (ea.cmdidx == CMD_bang)
	ea.arg = p;
    ਨਹੀਂਤਾਂ
	ea.arg = skipwhite(p);

    // ":file" cannot be run with an argument when "curbuf_lock" is set
    ਜੇ (ea.cmdidx == CMD_file && *ea.arg != NUL && curbuf_locked())
	ਜਾਓ doend;

    /*
     * Check for "++opt=val" argument.
     * Must be first, allow ":w ++enc=utf8 !cmd"
     */
    ਜੇ (ea.argt & EX_ARGOPT)
	ਜਦੋਂ (ea.arg[0] == '+' && ea.arg[1] == '+')
	    ਜੇ (getargopt(&ea) == FAIL && !ni)
	    {
		errormsg = _(e_invalid_ਦਲੀਲ);
		ਜਾਓ doend;
	    }

    ਜੇ (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	ਜੇ (*ea.arg == '>')			// append
	{
	    ਜੇ (*++ea.arg != '>')		// typed wrong
	    {
		errormsg = _(e_use_w_or_w_gt_gt);
		ਜਾਓ doend;
	    }
	    ea.arg = skipwhite(ea.arg + 1);
	    ea.append = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*ea.arg == '!' && ea.cmdidx == CMD_write)  // :w !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    ਜੇ (ea.cmdidx == CMD_read)
    {
	ਜੇ (ea.forceit)
	{
	    ea.usefilter = TRUE;		// :r! filter if ea.forceit
	    ea.forceit = FALSE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*ea.arg == '!')		// :r !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    ਜੇ (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ea.amount = 1;
	ਜਦੋਂ (*ea.arg == *ea.cmd)		// count number of '>' or '<'
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
    ਜੇ ((ea.argt & EX_CMDARG) && !ea.usefilter)
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
    ਜੇ ((ea.argt & EX_TRLBAR) && !ea.usefilter)
    {
	separate_nextcmd(&ea, FALSE);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ea.cmdidx == CMD_bang
	    || ea.cmdidx == CMD_terminal
	    || ea.cmdidx == CMD_global
	    || ea.cmdidx == CMD_vglobal
	    || ea.usefilter
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    || inside_ਖੰਡ(&ea)
#ਜੇ_ਅੰਤ
	    )
    {
	ਲਈ (p = ea.arg; *p; ++p)
	{
	    // Remove one backslash before a newline
	    ਜੇ (*p == '\\' && p[1] == '\n')
		STRMOVE(p, p + 1);
	    ਨਹੀਂਤਾਂ ਜੇ (*p == '\n' && !(ea.argt & EX_ਵਿਅੰਜਕ_ARG))
	    {
		ea.nextcmd = p + 1;
		*p = NUL;
		ਰੋਕੋ;
	    }
	}
    }

    ਜੇ ((ea.argt & EX_DFLALL) && ea.addr_ਗਿਣਤੀ == 0)
	ਪਤਾ_default_all(&ea);

    // accept numbered register only when no count allowed (:put)
    ਜੇ (       (ea.argt & EX_REGSTR)
	    && *ea.arg != NUL
	       // Do not allow register = for user commands
	    && (!IS_USER_CMDIDX(ea.cmdidx) || *ea.arg != '=')
	    && !((ea.argt & EX_ਗਿਣਤੀ) && VIM_ISDIGIT(*ea.arg)))
    {
#ਜੇ !defined(FEAT_CLIPBOARD) && !defined(FEAT_CLIPBOARD_PROVIDER)
	// check these explicitly for a more specific error message
	ਜੇ (*ea.arg == '*' || *ea.arg == '+')
	{
	    errormsg = _(e_invalid_ਰਜਿਸਟਰ_name);
	    ਜਾਓ doend;
	}
#ਜੇ_ਅੰਤ
	ਜੇ (valid_yank_reg(*ea.arg, (!IS_USER_CMDIDX(ea.cmdidx)
			    && ea.cmdidx != CMD_put && ea.cmdidx != CMD_iput)))
	{
	    ea.regname = *ea.arg++;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    // for '=' register: accept the rest of the line as an expression
	    ਜੇ (ea.arg[-1] == '=' && ea.arg[0] != NUL)
	    {
		ਜੇ (!ea.skip)
		{
		    ਸੈੱਟ_ਵਿਅੰਜਕ_ਸਤਰ(vim_strsave(ea.arg), &ea);
		    did_ਸੈੱਟ_ਵਿਅੰਜਕ_ਸਤਰ = TRUE;
		}
		ea.arg += STRLEN(ea.arg);
	    }
#ਜੇ_ਅੰਤ
	    ea.arg = skipwhite(ea.arg);
	}
    }

    /*
     * Check for a count.  When accepting a EX_BUFNAME, don't use "123foo" as a
     * count, it's a buffer name.
     */
    ਜੇ ((ea.argt & EX_ਗਿਣਤੀ) && VIM_ISDIGIT(*ea.arg)
	    && (!(ea.argt & EX_BUFNAME) || *(p = skipdigits(ea.arg + 1)) == NUL
							  || VIM_ISWHITE(*p)))
    {
	n = getdigits_quoted(&ea.arg);
	ea.arg = skipwhite(ea.arg);
	ਜੇ (n <= 0 && !ni && (ea.argt & EX_ZEROR) == 0)
	{
	    errormsg = _(e_ਸਕਾਰਾਤਮਕ_ਗਿਣਤੀ_required);
	    ਜਾਓ doend;
	}
	ਜੇ (ea.addr_type != ADDR_ਸਤਰਾਂ)	// e.g. :buffer 2, :sleep 3
	{
	    ea.line2 = n;
	    ਜੇ (ea.addr_ਗਿਣਤੀ == 0)
		ea.addr_ਗਿਣਤੀ = 1;
	}
	ਨਹੀਂਤਾਂ
	{
	    ea.line1 = ea.line2;
	    ਜੇ (ea.line2 >= LONG_MAX - (n - 1))
		ea.line2 = LONG_MAX;  // avoid overflow
	    ਨਹੀਂਤਾਂ
		ea.line2 += n - 1;
	    ++ea.addr_ਗਿਣਤੀ;
	    /*
	     * Be vi compatible: no error message for out of range.
	     */
	    ਜੇ (ea.line2 > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		ea.line2 = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	}
    }

    /*
     * Check for flags: 'l', 'p' and '#'.
     */
    ਜੇ (ea.argt & EX_FLAGS)
	get_flags(&ea);
    ਜੇ (!ni && !(ea.argt & EX_EXTRA) && *ea.arg != NUL
	    && *ea.arg != '"' && (*ea.arg != '|' || (ea.argt & EX_TRLBAR) == 0))
    {
	// no arguments allowed but there is something
	errormsg = ex_errmsg(e_trailing_characters_str, ea.arg);
	ਜਾਓ doend;
    }

    ਜੇ (!ni && (ea.argt & EX_NEEDARG) && *ea.arg == NUL)
    {
	errormsg = _(e_ਦਲੀਲ_required);
	ਜਾਓ doend;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    /*
     * Skip the command when it's not going to be executed.
     * The commands like :if, :endif, etc. always need to be executed.
     * Also make an exception for commands that handle a trailing command
     * themselves.
     */
    ਜੇ (ea.skip)
    {
	ਚੋਣ (ea.cmdidx)
	{
	    // commands that need evaluation
	    ਹਾਲਤ CMD_while:
	    ਹਾਲਤ CMD_endwhile:
	    ਹਾਲਤ CMD_for:
	    ਹਾਲਤ CMD_endfor:
	    ਹਾਲਤ CMD_if:
	    ਹਾਲਤ CMD_elseif:
	    ਹਾਲਤ CMD_else:
	    ਹਾਲਤ CMD_endif:
	    ਹਾਲਤ CMD_try:
	    ਹਾਲਤ CMD_catch:
	    ਹਾਲਤ CMD_finally:
	    ਹਾਲਤ CMD_endtry:
	    ਹਾਲਤ CMD_function:
	    ਹਾਲਤ CMD_def:
				ਰੋਕੋ;

	    // commands that read a block of lines
	    ਹਾਲਤ CMD_abstract:
	    ਹਾਲਤ CMD_append:
	    ਹਾਲਤ CMD_autocmd:
	    ਹਾਲਤ CMD_change:
	    ਹਾਲਤ CMD_class:
	    ਹਾਲਤ CMD_command:
	    ਹਾਲਤ CMD_enum:
	    ਹਾਲਤ CMD_ਪਾਓ:
	    ਹਾਲਤ CMD_interface:
	    ਹਾਲਤ CMD_loadkeymap:
				ਰੋਕੋ;

	    // Commands that handle '|' themselves.  Check: A command should
	    // either have the EX_TRLBAR flag, appear in this list or appear in
	    // the list at ":help :bar".
	    ਹਾਲਤ CMD_aboveleft:
	    ਹਾਲਤ CMD_and:
	    ਹਾਲਤ CMD_belowright:
	    ਹਾਲਤ CMD_botright:
	    ਹਾਲਤ CMD_browse:
	    ਹਾਲਤ CMD_call:
	    ਹਾਲਤ CMD_confirm:
	    ਹਾਲਤ CMD_const:
	    ਹਾਲਤ CMD_delfunction:
	    ਹਾਲਤ CMD_djump:
	    ਹਾਲਤ CMD_dlist:
	    ਹਾਲਤ CMD_dsearch:
	    ਹਾਲਤ CMD_dsplit:
	    ਹਾਲਤ CMD_echo:
	    ਹਾਲਤ CMD_echoerr:
	    ਹਾਲਤ CMD_echomsg:
	    ਹਾਲਤ CMD_echon:
	    ਹਾਲਤ CMD_eval:
	    ਹਾਲਤ CMD_execute:
	    ਹਾਲਤ CMD_filter:
	    ਹਾਲਤ CMD_final:
	    ਹਾਲਤ CMD_help:
	    ਹਾਲਤ CMD_hide:
	    ਹਾਲਤ CMD_horizontal:
	    ਹਾਲਤ CMD_ijump:
	    ਹਾਲਤ CMD_ilist:
	    ਹਾਲਤ CMD_isearch:
	    ਹਾਲਤ CMD_isplit:
	    ਹਾਲਤ CMD_keepalt:
	    ਹਾਲਤ CMD_keepjumps:
	    ਹਾਲਤ CMD_keepmarks:
	    ਹਾਲਤ CMD_keeppatterns:
	    ਹਾਲਤ CMD_leftabove:
	    ਹਾਲਤ CMD_let:
	    ਹਾਲਤ CMD_lockmarks:
	    ਹਾਲਤ CMD_lockvar:
	    ਹਾਲਤ CMD_lua:
	    ਹਾਲਤ CMD_ਮੇਲ:
	    ਹਾਲਤ CMD_mzscheme:
	    ਹਾਲਤ CMD_noautocmd:
	    ਹਾਲਤ CMD_noswapfile:
	    ਹਾਲਤ CMD_perl:
	    ਹਾਲਤ CMD_psearch:
	    ਹਾਲਤ CMD_py3:
	    ਹਾਲਤ CMD_python3:
	    ਹਾਲਤ CMD_python:
	    ਹਾਲਤ CMD_return:
	    ਹਾਲਤ CMD_rightbelow:
	    ਹਾਲਤ CMD_ruby:
	    ਹਾਲਤ CMD_silent:
	    ਹਾਲਤ CMD_smagic:
	    ਹਾਲਤ CMD_snomagic:
	    ਹਾਲਤ CMD_substitute:
	    ਹਾਲਤ CMD_syntax:
	    ਹਾਲਤ CMD_tab:
	    ਹਾਲਤ CMD_tcl:
	    ਹਾਲਤ CMD_throw:
	    ਹਾਲਤ CMD_tilde:
	    ਹਾਲਤ CMD_topleft:
	    ਹਾਲਤ CMD_unlet:
	    ਹਾਲਤ CMD_unlockvar:
	    ਹਾਲਤ CMD_var:
	    ਹਾਲਤ CMD_verbose:
	    ਹਾਲਤ CMD_vertical:
	    ਹਾਲਤ CMD_wincmd:
				ਰੋਕੋ;

	    ਮੂਲ:		ਜਾਓ doend;
	}
    }
#ਜੇ_ਅੰਤ

    ਜੇ ((ea.argt & EX_XFILE)
	    && expand_filename(&ea, cmdlinep, &errormsg) == FAIL)
	ਜਾਓ doend;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (is_export && (ea.argt & EX_EXPORT) == 0)
    {
	emsg(_(e_invalid_command_after_export));
	ਜਾਓ doend;
    }
#ਜੇ_ਅੰਤ

    /*
     * Accept buffer name.  Cannot be used at the same time with a buffer
     * number.  Don't do this for a user command.
     */
    ਜੇ ((ea.argt & EX_BUFNAME) && *ea.arg != NUL && ea.addr_ਗਿਣਤੀ == 0
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * :bdelete, :bwipeout and :bunload take several arguments, separated
	 * by spaces: find next space (skipping over escaped characters).
	 * The others take one argument: ignore trailing spaces.
	 */
	ਜੇ (ea.cmdidx == CMD_bdelete || ea.cmdidx == CMD_bwipeout
						  || ea.cmdidx == CMD_bunload)
	    p = skiptowhite_esc(ea.arg);
	ਨਹੀਂਤਾਂ
	{
	    p = ea.arg + STRLEN(ea.arg);
	    ਜਦੋਂ (p > ea.arg && VIM_ISWHITE(p[-1]))
		--p;
	}
	ea.line2 = buflist_findpat(ea.arg, p, (ea.argt & EX_BUFUNL) != 0,
								FALSE, FALSE);
	ਜੇ (ea.line2 < 0)	    // failed
	    ਜਾਓ doend;
	ea.addr_ਗਿਣਤੀ = 1;
	ea.arg = skipwhite(p);
    }

    // The :try command saves the emsg_silent flag, reset it here when
    // ":silent! try" was used, it should only apply to :try itself.
    ਜੇ (ea.cmdidx == CMD_try && cmdmod.cmod_did_esilent > 0)
    {
	emsg_silent -= cmdmod.cmod_did_esilent;
	ਜੇ (emsg_silent < 0)
	    emsg_silent = 0;
	cmdmod.cmod_did_esilent = 0;
    }

/*
 * 7. Execute the command.
 */

    ਜੇ (IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * Execute a user-defined command.
	 */
	do_ucmd(&ea);
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * Call the function to execute the builtin command.
	 */
	(cmdnames[ea.cmdidx].cmd_func)(&ea);
	ਜੇ (ea.errmsg != ਨਿਰਾ)
	    errormsg = ea.errmsg;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // A command will reset "is_export" when exporting an item.  If it is still
    // set something went wrong or the command was never executed.
    ਜੇ (!ea.skip && is_export)
    {
	ਜੇ (errormsg == ਨਿਰਾ)
	    errormsg = _(e_export_with_invalid_ਦਲੀਲ);
	is_export = FALSE;
    }

    // Set flag that any command was executed, used by ex_vim9script().
    // Not if this was a command that wasn't executed or :endif.
    ਜੇ (sourcing_a_script(&ea)
	    && current_sctx.sc_sid > 0
	    && ea.cmdidx != CMD_endif
	    && (cstack->cs_idx < 0
		    || (cstack->cs_flags[cstack->cs_idx] & CSF_ਸਰਗਰਮ)))
	SCRIPT_ਵਸਤੂ(current_sctx.sc_sid)->sn_state = SN_STATE_HAD_COMMAND;

    /*
     * If the command just executed called do_cmdline(), any throw or ":return"
     * or ":finish" encountered there must also check the cstack of the still
     * active do_cmdline() that called this do_one_cmd().  Rethrow an uncaught
     * exception, or reanimate a returned function or finished script file and
     * return or finish it again.
     */
    ਜੇ (need_rethrow)
	do_throw(cstack);
    ਨਹੀਂਤਾਂ ਜੇ (check_cstack)
    {
	ਜੇ (ਸਰੋਤ_ਮੁਕੰਮਲ(fgetline, cookie))
	    do_ਅੰਤ(&ea, TRUE);
	ਨਹੀਂਤਾਂ ਜੇ (getline_equal(fgetline, cookie, get_func_ਸਤਰ)
						   && current_func_returned())
	    do_return(&ea, TRUE, FALSE, ਨਿਰਾ);
    }
    need_rethrow = check_cstack = FALSE;
#ਜੇ_ਅੰਤ

doend:
    ਜੇ (curwin->w_cursor.lnum == 0)	// can happen with zero line number
    {
	curwin->w_cursor.lnum = 1;
	curwin->w_cursor.ਖਾਨਾ = 0;
    }

    ਜੇ (errormsg != ਨਿਰਾ && *errormsg != NUL && !did_emsg)
    {
	ਜੇ ((sourcing || !ਕੁੰਜੀ_Typed) && !did_append_cmd)
	{
	    ਜੇ (errormsg != (ਅੱਖਰ *)IObuff)
	    {
		STRCPY(IObuff, errormsg);
		errormsg = (ਅੱਖਰ *)IObuff;
	    }
	    append_command(*cmdlinep);
	}
	emsg(errormsg);
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    do_errthrow(cstack,
	    (ea.cmdidx != CMD_ਮਾਪ && !IS_USER_CMDIDX(ea.cmdidx))
			? cmdnames[(ਪੂਰਨ)ea.cmdidx].cmd_name : (char_u *)ਨਿਰਾ);

    ਜੇ (did_ਸੈੱਟ_ਵਿਅੰਜਕ_ਸਤਰ)
	ਸੈੱਟ_ਵਿਅੰਜਕ_ਸਤਰ(ਨਿਰਾ, ਨਿਰਾ);
    is_export = FALSE;
#ਜੇ_ਅੰਤ

    undo_cmdmod(&cmdmod);
    cmdmod = save_cmdmod;
    reg_executing = save_reg_executing;
    pending_ਸਿਰਾ_reg_executing = save_pending_ਸਿਰਾ_reg_executing;

    ਜੇ (ea.nextcmd && *ea.nextcmd == NUL)	// not really a next command
	ea.nextcmd = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    --ex_nesting_level;
    vim_free(ea.cmdline_tofree);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ea.nextcmd;
}

ਸਥਿਰ ਅੱਖਰ ex_error_buf[ਸੁਨੇਹਾ_BUF_LEN];

/*
 * Return an error message with argument included.
 * Uses a static buffer, only the last error will be kept.
 * "msg" will be translated, caller should use N_().
 */
    ਅੱਖਰ *
ex_errmsg(ਅੱਖਰ *ਸੁਨੇਹਾ, char_u *arg)
{
    vim_snprintf(ex_error_buf, ਸੁਨੇਹਾ_BUF_LEN, _(ਸੁਨੇਹਾ), arg);
    ਵਾਪਸ ex_error_buf;
}

/*
 * The "+" string used in place of an empty command in Ex mode.
 * This string is used in pointer comparison.
 */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ exmode_plus[] = "+";

/*
 * Handle a range without a command.
 * Returns an error message on failure.
 */
    ਅੱਖਰ *
ex_range_without_command(exarg_T *eap)
{
    ਅੱਖਰ *errormsg = ਨਿਰਾ;

    ਜੇ ((*eap->cmd == '|' ||
		(exmode_ਸਰਗਰਮ && eap->cmd != (char_u *)exmode_plus + 1))
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    && !ਵਿੱਚ_vim9script()
#ਜੇ_ਅੰਤ
       )
    {
	eap->cmdidx = CMD_print;
	eap->argt = EX_RANGE+EX_ਗਿਣਤੀ+EX_TRLBAR;
	ਜੇ ((errormsg = invalid_range(eap)) == ਨਿਰਾ)
	{
	    correct_range(eap);
	    ex_print(eap);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (eap->addr_ਗਿਣਤੀ != 0)
    {
	ਜੇ (eap->line2 > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    // With '-' in 'cpoptions' a line number past the file is an
	    // error, otherwise put it at the end of the file.
	    ਜੇ (vim_strchr(p_cpo, CPO_MINUS) != ਨਿਰਾ)
		eap->line2 = -1;
	    ਨਹੀਂਤਾਂ
		eap->line2 = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	}

	ਜੇ (eap->line2 < 0)
	    errormsg = _(e_invalid_range);
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (eap->line2 == 0)
		curwin->w_cursor.lnum = 1;
	    ਨਹੀਂਤਾਂ
		curwin->w_cursor.lnum = eap->line2;
	    beginline(BL_SOL | BL_FIX);
	}
    }
    ਵਾਪਸ errormsg;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 * If "noparen" is TRUE do not recognize the command followed by "(" or ".".
 */
    ਸਥਿਰ ਪੂਰਨ
checkforcmd_opt(
    char_u	**pp,		// start of command
    ਅੱਖਰ	*cmd,		// name of command
    ਪੂਰਨ		len,		// required length
    ਪੂਰਨ		noparen)
{
    ਪੂਰਨ		i;

    ਲਈ (i = 0; cmd[i] != NUL; ++i)
	ਜੇ (((char_u *)cmd)[i] != (*pp)[i])
	    ਰੋਕੋ;
    ਜੇ (i >= len && !ASCII_ISALPHA((*pp)[i]) && (*pp)[i] != '_'
			 && (!noparen || ((*pp)[i] != '(' && (*pp)[i] != '.')))
    {
	*pp = skipwhite(*pp + i);
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    ਪੂਰਨ
checkforcmd(
    char_u	**pp,		// start of command
    ਅੱਖਰ	*cmd,		// name of command
    ਪੂਰਨ		len)		// required length
{
    ਵਾਪਸ checkforcmd_opt(pp, cmd, len, FALSE);
}

/*
 * Check for an Ex command with optional tail, not followed by "(" or ".".
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    ਪੂਰਨ
checkforcmd_noparen(
    char_u	**pp,		// start of command
    ਅੱਖਰ	*cmd,		// name of command
    ਪੂਰਨ		len)		// required length
{
    ਵਾਪਸ checkforcmd_opt(pp, cmd, len, TRUE);
}

/*
 * Find the replacement text of a ":command", the part after the attributes and
 * the command name.  Returns NULL when there is none.  Does not modify "arg"
 * and gives no error messages.
 */
    ਸਥਿਰ char_u *
ਲੱਭੋ_ucmd_repl(char_u *arg)
{
    char_u	*p = arg;

    // Skip over the attributes.
    ਜਦੋਂ (*p == '-')
	p = skipwhite(skiptowhite(p));

    // Skip over the command name.
    ਜੇ (!ASCII_ISALPHA(*p))
	ਵਾਪਸ ਨਿਰਾ;
    ਜਦੋਂ (ASCII_ISALNUM(*p))
	++p;

    ਵਾਪਸ *p == NUL ? ਨਿਰਾ : skipwhite(p);
}

/*
 * Find the "{" in "line" that starts a block for ":command" or ":autocmd".
 * That is the case when the command argument is "{" at the end of the line,
 * also when the command is nested in another ":command" or ":autocmd".
 * Returns NULL when the line does not start such a block.
 */
    char_u *
ਲੱਭੋ_cmd_ਖੰਡ_ਸ਼ੁਰੂ(char_u *ਸਤਰ)
{
    char_u	*p = skipwhite(ਸਤਰ);

    ਲਈ (;;)
    {
	char_u	*arg = p;

	ਜੇ (*p == '{' && ਸਿਰੇ_excmd2(p, skipwhite(p + 1)))
	    ਵਾਪਸ p;

	ਜੇ (checkforcmd_noparen(&arg, "autocmd", 2))
	{
	    ਜੇ (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = au_ਲੱਭੋ_cmd_arg(arg);
	}
	ਨਹੀਂਤਾਂ ਜੇ (checkforcmd_noparen(&arg, "command", 3))
	{
	    ਜੇ (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = ਲੱਭੋ_ucmd_repl(arg);
	}
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ ਨਿਰਾ;

	ਜੇ (p == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
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
    ਪੂਰਨ
parse_command_modifiers(
	exarg_T	    *eap,
	ਅੱਖਰ	    **errormsg,
	cmdmod_T    *cmod,
	ਪੂਰਨ	    skip_only)
{
    char_u  *orig_cmd = eap->cmd;
    char_u  *cmd_ਸ਼ੁਰੂ = ਨਿਰਾ;
    ਪੂਰਨ	    use_plus_cmd = FALSE;
    ਪੂਰਨ	    starts_with_colon = FALSE;
    ਪੂਰਨ	    vim9script = ਵਿੱਚ_vim9script();
    ਪੂਰਨ	    has_visual_range = FALSE;

    CLEAR_ਸੂਚਕ(cmod);
    cmod->cmod_flags = sticky_cmdmod_flags;

    ਜੇ (STRNCMP(eap->cmd, "'<,'>", 5) == 0)
    {
	// The automatically inserted Visual area range is skipped, so that
	// typing ":cmdmod cmd" in Visual mode works without having to move the
	// range to after the modifiers. The command will be "'<,'>cmdmod cmd",
	// parse "cmdmod cmd" and then put back "'<,'>" before "cmd" below.
	eap->cmd += 5;
	cmd_ਸ਼ੁਰੂ = eap->cmd;
	has_visual_range = TRUE;
    }

    // Repeat until no more command modifiers are found.
    ਲਈ (;;)
    {
	char_u  *p;

	ਜਦੋਂ (*eap->cmd == ' ' || *eap->cmd == '\t' || *eap->cmd == ':')
	{
	    ਜੇ (*eap->cmd == ':')
		starts_with_colon = TRUE;
	    ++eap->cmd;
	}

	// in ex mode, an empty command (after modifiers) works like :+
	ਜੇ (*eap->cmd == NUL && exmode_ਸਰਗਰਮ
		   && (getline_equal(eap->ea_getline, eap->cookie, getexmodeline)
		       || getline_equal(eap->ea_getline, eap->cookie, getexline))
			&& curwin->w_cursor.lnum < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    use_plus_cmd = TRUE;
	    ਜੇ (!skip_only)
		ex_pressedreturn = TRUE;
	    ਰੋਕੋ;  // no modifiers following
	}

	// ignore comment and empty lines
	ਜੇ (comment_ਸ਼ੁਰੂ(eap->cmd, starts_with_colon))
	{
	    // a comment ends at a NL
	    eap->nextcmd = vim_strchr(eap->cmd, '\n');
	    ਜੇ (eap->nextcmd != ਨਿਰਾ)
		++eap->nextcmd;
	    ਜੇ (vim9script)
	    {
		ਜੇ (has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		ਜੇ (eap->cmd[0] == '#' && eap->cmd[1] == '{'
							 && eap->cmd[2] != '{')
		    *errormsg = _(e_cannot_use_hash_curly_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ_comment);
#ਜੇ_ਅੰਤ
	    }
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (*eap->cmd == '\n')
	{
	    eap->nextcmd = eap->cmd + 1;
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (*eap->cmd == NUL)
	{
	    ਜੇ (!skip_only)
	    {
		ex_pressedreturn = TRUE;
		ਜੇ (vim9script && has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
	    }
	    ਵਾਪਸ FAIL;
	}

	p = skip_range(eap->cmd, TRUE, ਨਿਰਾ);

	// In Vim9 script a variable can shadow a command modifier:
	//   verbose = 123
	//   verbose += 123
	//   silent! verbose = func()
	//   verbose.member = 2
	//   verbose[expr] = 2
	// But not:
	//   verbose [a, b] = list
	ਜੇ (vim9script)
	{
	    char_u *s, *n;

	    ਲਈ (s = eap->cmd; ASCII_ISALPHA(*s); ++s)
		;
	    n = skipwhite(s);
	    ਜੇ (*n == '.' || *n == '=' || (*n != NUL && n[1] == '=')
		    || *s == '[')
		ਰੋਕੋ;
	}

	ਚੋਣ (*p)
	{
	    // When adding an entry, also modify cmdmod_info_tab[].
	    ਹਾਲਤ 'a':	ਜੇ (!checkforcmd_noparen(&eap->cmd, "aboveleft", 3))
			    ਰੋਕੋ;
			cmod->cmod_ਵੰਡੋ |= WSP_ABOVE;
			ਜਾਰੀ;

	    ਹਾਲਤ 'b':	ਜੇ (checkforcmd_noparen(&eap->cmd, "belowright", 3))
			{
			    cmod->cmod_ਵੰਡੋ |= WSP_ਹੇਠਾਂ;
			    ਜਾਰੀ;
			}
			ਜੇ (checkforcmd_opt(&eap->cmd, "browse", 3, TRUE))
			{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE_CMD
			    cmod->cmod_flags |= CMOD_BROWSE;
#ਜੇ_ਅੰਤ
			    ਜਾਰੀ;
			}
			ਜੇ (!checkforcmd_noparen(&eap->cmd, "botright", 2))
			    ਰੋਕੋ;
			cmod->cmod_ਵੰਡੋ |= WSP_BOT;
			ਜਾਰੀ;

	    ਹਾਲਤ 'c':	ਜੇ (!checkforcmd_opt(&eap->cmd, "confirm", 4, TRUE))
			    ਰੋਕੋ;
#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			cmod->cmod_flags |= CMOD_CONFIRM;
#ਜੇ_ਅੰਤ
			ਜਾਰੀ;

	    ਹਾਲਤ 'k':	ਜੇ (checkforcmd_noparen(&eap->cmd, "keepmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_KEEPMARKS;
			    ਜਾਰੀ;
			}
			ਜੇ (checkforcmd_noparen(&eap->cmd, "keepalt", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPALT;
			    ਜਾਰੀ;
			}
			ਜੇ (checkforcmd_noparen(&eap->cmd, "keeppatterns", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPPATTERNS;
			    ਜਾਰੀ;
			}
			ਜੇ (!checkforcmd_noparen(&eap->cmd, "keepjumps", 5))
			    ਰੋਕੋ;
			cmod->cmod_flags |= CMOD_KEEPJUMPS;
			ਜਾਰੀ;

	    ਹਾਲਤ 'f':	// only accept ":filter {pat} cmd"
			{
			    char_u  *reg_pat;
			    char_u  *nulp = ਨਿਰਾ;
			    ਪੂਰਨ	    c = 0;

			    ਜੇ (!checkforcmd_noparen(&p, "filter", 4)
				    || *p == NUL
				    || (ਸਿਰੇ_excmd(*p)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
					// in ":filter #pat# cmd" # does not
					// start a comment
				     && (!vim9script || VIM_ISWHITE(p[1]))
#ਜੇ_ਅੰਤ
				     ))
				ਰੋਕੋ;
			    ਜੇ (*p == '!')
			    {
				cmod->cmod_filter_force = TRUE;
				p = skipwhite(p + 1);
				ਜੇ (*p == NUL || ਸਿਰੇ_excmd(*p))
				    ਰੋਕੋ;
			    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
			    // Avoid that "filter(arg)" is recognized.
			    ਜੇ (vim9script && !VIM_ISWHITE(p[-1]))
				ਰੋਕੋ;
#ਜੇ_ਅੰਤ
			    ਜੇ (skip_only)
				p = skip_vimgrep_pat(p, ਨਿਰਾ, ਨਿਰਾ);
			    ਨਹੀਂਤਾਂ
				// NOTE: This puts a NUL after the pattern.
				p = skip_vimgrep_pat_ext(p, &reg_pat, ਨਿਰਾ,
								    &nulp, &c);
			    ਜੇ (p == ਨਿਰਾ || *p == NUL)
				ਰੋਕੋ;
			    ਜੇ (!skip_only)
			    {
				cmod->cmod_filter_regmatch.regprog =
						vim_regcomp(reg_pat, RE_MAGIC);
				ਜੇ (cmod->cmod_filter_regmatch.regprog == ਨਿਰਾ)
				    ਰੋਕੋ;
				// restore the character overwritten by NUL
				ਜੇ (nulp != ਨਿਰਾ)
				    *nulp = c;
			    }
			    eap->cmd = p;
			    ਜਾਰੀ;
			}

	    ਹਾਲਤ 'h':	ਜੇ (checkforcmd_noparen(&eap->cmd, "horizontal", 3))
			{
			    cmod->cmod_ਵੰਡੋ |= WSP_HOR;
			    ਜਾਰੀ;
			}
			// ":hide" and ":hide | cmd" are not modifiers
			ਜੇ (p != eap->cmd || !checkforcmd_noparen(&p, "hide", 3)
					       || *p == NUL || ਸਿਰੇ_excmd(*p))
			    ਰੋਕੋ;
			eap->cmd = p;
			cmod->cmod_flags |= CMOD_HIDE;
			ਜਾਰੀ;

	    ਹਾਲਤ 'l':	ਜੇ (checkforcmd_noparen(&eap->cmd, "lockmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_LOCKMARKS;
			    ਜਾਰੀ;
			}
			ਜੇ (checkforcmd_noparen(&eap->cmd, "legacy", 3))
			{
			    ਜੇ (ਸਿਰੇ_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_legacy_must_be_followed_by_command);
				ਵਾਪਸ FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_VIM9CMD;
			    cmod->cmod_flags |= CMOD_LEGACY;
			    ਜਾਰੀ;
			}

			ਜੇ (!checkforcmd_noparen(&eap->cmd, "leftabove", 5))
			    ਰੋਕੋ;
			cmod->cmod_ਵੰਡੋ |= WSP_ABOVE;
			ਜਾਰੀ;

	    ਹਾਲਤ 'n':	ਜੇ (checkforcmd_noparen(&eap->cmd, "noautocmd", 3))
			{
			    cmod->cmod_flags |= CMOD_NOAUTOCMD;
			    ਜਾਰੀ;
			}
			ਜੇ (!checkforcmd_noparen(&eap->cmd, "noswapfile", 3))
			    ਰੋਕੋ;
			cmod->cmod_flags |= CMOD_NOSWAPFILE;
			ਜਾਰੀ;

	    ਹਾਲਤ 'r':	ਜੇ (!checkforcmd_noparen(&eap->cmd, "rightbelow", 6))
			    ਰੋਕੋ;
			cmod->cmod_ਵੰਡੋ |= WSP_ਹੇਠਾਂ;
			ਜਾਰੀ;

	    ਹਾਲਤ 's':	ਜੇ (checkforcmd_noparen(&eap->cmd, "sandbox", 3))
			{
			    cmod->cmod_flags |= CMOD_SANDBOX;
			    ਜਾਰੀ;
			}
			ਜੇ (!checkforcmd_noparen(&eap->cmd, "silent", 3))
			    ਰੋਕੋ;
			cmod->cmod_flags |= CMOD_SILENT;
			ਜੇ (*eap->cmd == '!' && !VIM_ISWHITE(eap->cmd[-1]))
			{
			    // ":silent!", but not "silent !cmd"
			    eap->cmd = skipwhite(eap->cmd + 1);
			    cmod->cmod_flags |= CMOD_ERRSILENT;
			}
			ਜਾਰੀ;

	    ਹਾਲਤ 't':	ਜੇ (checkforcmd_noparen(&p, "tab", 3))
			{
			    ਜੇ (!skip_only)
			    {
				ਲੰਮਾ tabnr = get_ਪਤਾ(eap, &eap->cmd,
						    ADDR_TABS, eap->skip,
						    skip_only, FALSE, 1);
				ਜੇ (tabnr == MAXLNUM)
				    cmod->cmod_tab = tabpage_ਸੂਚਕਾਂਕ(curtab) + 1;
				ਨਹੀਂਤਾਂ
				{
				    ਜੇ (tabnr < 0 || tabnr > ਆਖਰੀ_TAB_NR)
				    {
					*errormsg = _(e_invalid_range);
					ਵਾਪਸ FAIL;
				    }
				    cmod->cmod_tab = tabnr + 1;
				}
			    }
			    eap->cmd = p;
			    ਜਾਰੀ;
			}
			ਜੇ (!checkforcmd_noparen(&eap->cmd, "topleft", 2))
			    ਰੋਕੋ;
			cmod->cmod_ਵੰਡੋ |= WSP_ਸਿਖਰ;
			ਜਾਰੀ;

	    ਹਾਲਤ 'u':	ਜੇ (!checkforcmd_noparen(&eap->cmd, "unsilent", 3))
			    ਰੋਕੋ;
			cmod->cmod_flags |= CMOD_UNSILENT;
			ਜਾਰੀ;

	    ਹਾਲਤ 'v':	ਜੇ (checkforcmd_noparen(&eap->cmd, "vertical", 4))
			{
			    cmod->cmod_ਵੰਡੋ |= WSP_VERT;
			    ਜਾਰੀ;
			}
			ਜੇ (checkforcmd_noparen(&eap->cmd, "vim9cmd", 4))
			{
			    ਜੇ (ਸਿਰੇ_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_vim9cmd_must_be_followed_by_command);
				ਵਾਪਸ FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_LEGACY;
			    cmod->cmod_flags |= CMOD_VIM9CMD;
			    ਜਾਰੀ;
			}
			ਜੇ (!checkforcmd_noparen(&p, "verbose", 4))
			    ਰੋਕੋ;
			ਜੇ (vim_isdigit(*eap->cmd))
			{
			    // zero means not set, one is verbose == 0, etc.
			    cmod->cmod_verbose = atoi((ਅੱਖਰ *)eap->cmd) + 1;
			}
			ਨਹੀਂਤਾਂ
			    cmod->cmod_verbose = 2;  // default: verbose == 1
			eap->cmd = p;
			ਜਾਰੀ;
	}
	ਰੋਕੋ;
    }

    ਜੇ (has_visual_range)
    {
	ਜੇ (eap->cmd > cmd_ਸ਼ੁਰੂ)
	{
	    // Move the '<,'> range to after the modifiers and insert a colon.
	    // Since the modifiers have been parsed put the colon on top of the
	    // space: "'<,'>mod cmd" -> "mod:'<,'>cmd
	    // Put eap->cmd after the colon.
	    ਜੇ (use_plus_cmd)
	    {
		size_t len = STRLEN(cmd_ਸ਼ੁਰੂ);

		// Special case: empty command uses "+":
		//  "'<,'>mods" -> "mods *+
		//  Use "*" instead of "'<,'>" to avoid the command getting
		//  longer, in case it was allocated.
		mch_memmove(orig_cmd, cmd_ਸ਼ੁਰੂ, len);
		STRCPY(orig_cmd + len, " *+");
	    }
	    ਨਹੀਂਤਾਂ
	    {
		mch_memmove(cmd_ਸ਼ੁਰੂ - 5, cmd_ਸ਼ੁਰੂ, eap->cmd - cmd_ਸ਼ੁਰੂ);
		eap->cmd -= 5;
		mch_memmove(eap->cmd - 1, ":'<,'>", 6);
	    }
	}
	ਨਹੀਂਤਾਂ
	    // No modifiers, move the pointer back.
	    // Special case: change empty command to "+".
	    ਜੇ (use_plus_cmd)
		eap->cmd = (char_u *)"'<,'>+";
	    ਨਹੀਂਤਾਂ
		eap->cmd = orig_cmd;
    }
    ਨਹੀਂਤਾਂ ਜੇ (use_plus_cmd)
	eap->cmd = (char_u *)exmode_plus;

    ਵਾਪਸ OK;
}

/*
 * Return TRUE if "cmod" has anything set.
 */
    ਪੂਰਨ
has_cmdmod(cmdmod_T *cmod, ਪੂਰਨ ignore_silent)
{
    ਵਾਪਸ (cmod->cmod_flags != 0 && (!ignore_silent
		|| (cmod->cmod_flags
		      & ~(CMOD_SILENT | CMOD_ERRSILENT | CMOD_UNSILENT)) != 0))
	    || cmod->cmod_ਵੰਡੋ != 0
	    || cmod->cmod_verbose > 0
	    || cmod->cmod_tab != 0
	    || cmod->cmod_filter_regmatch.regprog != ਨਿਰਾ;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * If Vim9 script and "cmdmod" has anything set give an error and return TRUE.
 */
    ਪੂਰਨ
cmdmod_error(ਪੂਰਨ ignore_silent)
{
    ਜੇ (ਵਿੱਚ_vim9script() && has_cmdmod(&cmdmod, ignore_silent))
    {
	emsg(_(e_misplaced_command_modifier));
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

/*
 * Apply the command modifiers.  Saves current state in "cmdmod", call
 * undo_cmdmod() later.
 */
    ਖਾਲੀ
apply_cmdmod(cmdmod_T *cmod)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
    ਜੇ ((cmod->cmod_flags & CMOD_SANDBOX) && !cmod->cmod_did_sandbox)
    {
	++sandbox;
	cmod->cmod_did_sandbox = TRUE;
    }
#ਜੇ_ਅੰਤ
    ਜੇ (cmod->cmod_verbose > 0)
    {
	ਜੇ (cmod->cmod_verbose_save == 0)
	    cmod->cmod_verbose_save = p_verbose + 1;
	p_verbose = cmod->cmod_verbose - 1;
    }

    ਜੇ ((cmod->cmod_flags & (CMOD_SILENT | CMOD_UNSILENT))
	    && cmod->cmod_save_ਸੁਨੇਹਾ_silent == 0)
    {
	cmod->cmod_save_ਸੁਨੇਹਾ_silent = ਸੁਨੇਹਾ_silent + 1;
	cmod->cmod_save_ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_scroll;
    }
    ਜੇ (cmod->cmod_flags & CMOD_SILENT)
	++ਸੁਨੇਹਾ_silent;
    ਜੇ (cmod->cmod_flags & CMOD_UNSILENT)
	ਸੁਨੇਹਾ_silent = 0;

    ਜੇ (cmod->cmod_flags & CMOD_ERRSILENT)
    {
	++emsg_silent;
	++cmod->cmod_did_esilent;
    }

    ਜੇ ((cmod->cmod_flags & CMOD_NOAUTOCMD) && cmod->cmod_save_ei == ਨਿਰਾ)
    {
	// Set 'eventignore' to "all".
	// First save the existing option value for restoring it later.
	cmod->cmod_save_ei = vim_strsave(p_ei);
	ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"ei", -1,
					  (char_u *)"all", OPT_FREE, SID_NONE);
    }
}

/*
 * Undo and free contents of "cmod".
 */
    ਖਾਲੀ
undo_cmdmod(cmdmod_T *cmod)
{
    ਜੇ (cmod->cmod_verbose_save > 0)
    {
	p_verbose = cmod->cmod_verbose_save - 1;
	cmod->cmod_verbose_save = 0;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
    ਜੇ (cmod->cmod_did_sandbox)
    {
	--sandbox;
	cmod->cmod_did_sandbox = FALSE;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (cmod->cmod_save_ei != ਨਿਰਾ)
    {
	// Restore 'eventignore' to the value before ":noautocmd".
	ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"ei", -1, cmod->cmod_save_ei,
							   OPT_FREE, SID_NONE);
	free_string_option(cmod->cmod_save_ei);
	cmod->cmod_save_ei = ਨਿਰਾ;
    }

    vim_regfree(cmod->cmod_filter_regmatch.regprog);

    ਜੇ (cmod->cmod_save_ਸੁਨੇਹਾ_silent > 0)
    {
	// messages could be enabled for a serious error, need to check if the
	// counters don't become negative
	ਜੇ (!did_emsg || ਸੁਨੇਹਾ_silent > cmod->cmod_save_ਸੁਨੇਹਾ_silent - 1)
	    ਸੁਨੇਹਾ_silent = cmod->cmod_save_ਸੁਨੇਹਾ_silent - 1;
	emsg_silent -= cmod->cmod_did_esilent;
	ਜੇ (emsg_silent < 0)
	    emsg_silent = 0;
	// Restore msg_scroll, it's set by file I/O commands, even when no
	// message is actually displayed.
	ਸੁਨੇਹਾ_scroll = cmod->cmod_save_ਸੁਨੇਹਾ_scroll;

	// "silent reg" or "silent echo x" inside "redir" leaves msg_col
	// somewhere in the line.  Put it back in the first column.
	ਜੇ (redirecting())
	    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;

	cmod->cmod_save_ਸੁਨੇਹਾ_silent = 0;
	cmod->cmod_did_esilent = 0;
    }
}

/*
 * Parse the address range, if any, in "eap".
 * May set the last search pattern, unless "silent" is TRUE.
 * Return FAIL and set "errormsg" or return OK.
 */
    ਪੂਰਨ
parse_cmd_ਪਤਾ(exarg_T *eap, ਅੱਖਰ **errormsg, ਪੂਰਨ silent)
{
    ਪੂਰਨ		ਪਤਾ_ਗਿਣਤੀ = 1;
    linenr_T	lnum;
    ਪੂਰਨ		need_check_cursor = FALSE;
    ਪੂਰਨ		ret = FAIL;

    // Repeat for all ',' or ';' separated addresses.
    ਲਈ (;;)
    {
	eap->line1 = eap->line2;
	eap->line2 = default_ਪਤਾ(eap);
	eap->cmd = skipwhite(eap->cmd);
	lnum = get_ਪਤਾ(eap, &eap->cmd, eap->addr_type, eap->skip, silent,
					eap->addr_ਗਿਣਤੀ == 0, ਪਤਾ_ਗਿਣਤੀ++);
	ਜੇ (eap->cmd == ਨਿਰਾ)	// error detected
	    ਜਾਓ theend;
	ਜੇ (lnum == MAXLNUM)
	{
	    ਜੇ (*eap->cmd == '%')   // '%' - all lines
	    {
		++eap->cmd;
		ਚੋਣ (eap->addr_type)
		{
		    ਹਾਲਤ ADDR_ਸਤਰਾਂ:
		    ਹਾਲਤ ADDR_OTHER:
			eap->line1 = 1;
			eap->line2 = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_LOADED_BUFFERS:
			{
			    buf_T	*buf = firstbuf;

			    ਜਦੋਂ (buf->b_ਅਗਲਾ != ਨਿਰਾ
						  && buf->b_ml.ml_mfp == ਨਿਰਾ)
				buf = buf->b_ਅਗਲਾ;
			    eap->line1 = buf->b_fnum;
			    buf = lastbuf;
			    ਜਦੋਂ (buf->b_prev != ਨਿਰਾ
						  && buf->b_ml.ml_mfp == ਨਿਰਾ)
				buf = buf->b_prev;
			    eap->line2 = buf->b_fnum;
			    ਰੋਕੋ;
			}
		    ਹਾਲਤ ADDR_BUFFERS:
			eap->line1 = firstbuf->b_fnum;
			eap->line2 = lastbuf->b_fnum;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_WINDOWS:
		    ਹਾਲਤ ADDR_TABS:
			ਜੇ (IS_USER_CMDIDX(eap->cmdidx))
			{
			    eap->line1 = 1;
			    eap->line2 = eap->addr_type == ADDR_WINDOWS
						  ? ਆਖਰੀ_WIN_NR : ਆਖਰੀ_TAB_NR;
			}
			ਨਹੀਂਤਾਂ
			{
			    // there is no Vim command which uses '%' and
			    // ADDR_WINDOWS or ADDR_TABS
			    *errormsg = _(e_invalid_range);
			    ਜਾਓ theend;
			}
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_TABS_RELATIVE:
		    ਹਾਲਤ ADDR_UNSIGNED:
		    ਹਾਲਤ ADDR_QUICKFIX:
			*errormsg = _(e_invalid_range);
			ਜਾਓ theend;
		    ਹਾਲਤ ADDR_ARGUMENTS:
			ਜੇ (ARGCOUNT == 0)
			    eap->line1 = eap->line2 = 0;
			ਨਹੀਂਤਾਂ
			{
			    eap->line1 = 1;
			    eap->line2 = ARGCOUNT;
			}
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_QUICKFIX_VALID:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
			eap->line1 = 1;
			eap->line2 = qf_get_valid_ਮਾਪ(eap);
			ਜੇ (eap->line2 == 0)
			    eap->line2 = 1;
#ਜੇ_ਅੰਤ
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_NONE:
			// Will give an error later if a range is found.
			ਰੋਕੋ;
		}
		++eap->addr_ਗਿਣਤੀ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*eap->cmd == '*' && vim_strchr(p_cpo, CPO_ਤਾਰਾ) == ਨਿਰਾ)
	    {
		pos_T	    *fp;

		// '*' - visual area
		ਜੇ (eap->addr_type != ADDR_ਸਤਰਾਂ)
		{
		    *errormsg = _(e_invalid_range);
		    ਜਾਓ theend;
		}

		++eap->cmd;
		ਜੇ (!eap->skip)
		{
		    fp = getmark('<', FALSE);
		    ਜੇ (check_mark(fp) == FAIL)
			ਜਾਓ theend;
		    eap->line1 = fp->lnum;
		    fp = getmark('>', FALSE);
		    ਜੇ (check_mark(fp) == FAIL)
			ਜਾਓ theend;
		    eap->line2 = fp->lnum;
		    ++eap->addr_ਗਿਣਤੀ;
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	    eap->line2 = lnum;
	eap->addr_ਗਿਣਤੀ++;

	ਜੇ (*eap->cmd == ';')
	{
	    ਜੇ (!eap->skip)
	    {
		curwin->w_cursor.lnum = eap->line2;

		// Don't leave the cursor on an illegal line or column, but do
		// accept zero as address, so 0;/PATTERN/ works correctly
		// (where zero usually means to use the first line).
		// Check the cursor position before returning.
		ਜੇ (eap->line2 > 0)
		    check_cursor();
		ਨਹੀਂਤਾਂ
		    check_cursor_ਖਾਨਾ();
		need_check_cursor = TRUE;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (*eap->cmd != ',')
	    ਰੋਕੋ;
	++eap->cmd;
    }

    // One address given: set start and end lines.
    ਜੇ (eap->addr_ਗਿਣਤੀ == 1)
    {
	eap->line1 = eap->line2;
	// ... but only implicit: really no address given
	ਜੇ (lnum == MAXLNUM)
	    eap->addr_ਗਿਣਤੀ = 0;
    }
    ret = OK;

theend:
    ਜੇ (need_check_cursor)
	check_cursor();
    ਵਾਪਸ ret;
}

/*
 * Append "cmd" to the error message in IObuff.
 * Takes care of limiting the length and handling 0xa0, which would be
 * invisible otherwise.
 */
    ਸਥਿਰ ਖਾਲੀ
append_command(char_u *cmd)
{
    size_t  len = STRLEN(IObuff);
    char_u  *s = cmd;
    char_u  *d;

    ਜੇ (len > IOSIZE - 100)
    {
	// Not enough space, truncate and put in "...".
	d = IObuff + IOSIZE - 100;
	d -= mb_ਸਿਰ_off(IObuff, d);
	STRCPY(d, "...");
    }
    STRCAT(IObuff, ": ");
    d = IObuff + STRLEN(IObuff);
    ਜਦੋਂ (*s != NUL && d - IObuff + 5 < IOSIZE)
    {
	ਜੇ (enc_utf8 ? (s[0] == 0xc2 && s[1] == 0xa0) : *s == 0xa0)
	{
	    s += enc_utf8 ? 2 : 1;
	    STRCPY(d, "<a0>");
	    d += 4;
	}
	ਨਹੀਂਤਾਂ ਜੇ (d - IObuff + (*mb_ptr2len)(s) + 1 >= IOSIZE)
	    ਰੋਕੋ;
	ਨਹੀਂਤਾਂ
	    MB_COPY_CHAR(s, d);
    }
    *d = NUL;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * If "start" points "&opt", "&l:opt", "&g:opt" or "$ENV" return a pointer to
 * the name.  Otherwise just return "start".
 */
    char_u *
skip_option_env_lead(char_u *ਸ਼ੁਰੂ)
{
    char_u *name = ਸ਼ੁਰੂ;

    ਜੇ (*ਸ਼ੁਰੂ == '&')
    {
	ਜੇ ((ਸ਼ੁਰੂ[1] == 'l' || ਸ਼ੁਰੂ[1] == 'g') && ਸ਼ੁਰੂ[2] == ':')
	    name += 3;
	ਨਹੀਂਤਾਂ
	    name += 1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*ਸ਼ੁਰੂ == '$')
	name += 1;
    ਵਾਪਸ name;
}
#ਜੇ_ਅੰਤ

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
    ਸਥਿਰ ਪੂਰਨ
one_letter_cmd(char_u *p, cmdidx_T *idx)
{
    ਜੇ (ਵਿੱਚ_vim9script())
	ਵਾਪਸ FALSE;
    ਜੇ (p[0] == 'k'
	    && (p[1] != 'e' || (p[1] == 'e' && p[2] != 'e')))
    {
	*idx = CMD_k;
	ਵਾਪਸ TRUE;
    }
    ਜੇ (p[0] == 's'
	    && ((p[1] == 'c' && (p[2] == NUL || (p[2] != 's' && p[2] != 'r'
			&& (p[3] == NUL || (p[3] != 'i' && p[4] != 'p')))))
		|| p[1] == 'g'
		|| (p[1] == 'i' && p[2] != 'm' && p[2] != 'l' && p[2] != 'g')
		|| p[1] == 'I'
		|| (p[1] == 'r' && p[2] != 'e')))
    {
	*idx = CMD_substitute;
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Return TRUE if "cmd" starts with "123->", a number followed by a method
 * call.
 */
    ਪੂਰਨ
ਸੰਖਿਆ_ਵਿਧੀ(char_u *cmd)
{
    char_u *p = skipdigits(cmd);

    ਵਾਪਸ p > cmd && (p = skipwhite(p))[0] == '-' && p[1] == '>';
}
#ਜੇ_ਅੰਤ

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
ਲੱਭੋ_ex_command(
	exarg_T *eap,
	ਪੂਰਨ	*ਭਰਿਆ ਅਣਵਰਤਿਆ,
	ਪੂਰਨ	(*lookup)(char_u *, size_t, ਪੂਰਨ cmd, cctx_T *) ਅਣਵਰਤਿਆ,
	cctx_T	*cctx ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		len;
    char_u	*p;
    ਪੂਰਨ		i;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਪੂਰਨ		vim9 = FALSE;
#ਨਹੀਂਤਾਂ
    ਪੂਰਨ		vim9 = ਵਿੱਚ_vim9script();

    /*
     * Recognize a Vim9 script function/method call and assignment:
     * "lvar = value", "lvar(arg)", "[1, 2 3]->Func()"
     */
    p = eap->cmd;
    ਜੇ (lookup != ਨਿਰਾ)
    {
	char_u *pskip = skip_option_env_lead(eap->cmd);

	ਜੇ (vim_strchr((char_u *)"{('[\"@&$", *p) != ਨਿਰਾ
	       || ((p = ਮੰਜ਼ਿਲ_name_const_ਸਿਰਾ(pskip)) > eap->cmd && *p != NUL)
	       || (p[0] == '0' && p[1] == 'z'))
	{
	    ਪੂਰਨ	    oplen;
	    ਪੂਰਨ	    heredoc;
	    char_u  *swp;

	    ਜੇ (*eap->cmd == '&'
		    || (eap->cmd[0] == '$'
				  && eap->cmd[1] != '\'' && eap->cmd[1] != '"')
		    || (eap->cmd[0] == '@'
					&& (valid_yank_reg(eap->cmd[1], FALSE)
						       || eap->cmd[1] == '@')))
	    {
		ਜੇ (*eap->cmd == '&')
		{
		    p = eap->cmd + 1;
		    ਜੇ (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
			p += 2;
		    p = ਮੰਜ਼ਿਲ_name_ਸਿਰਾ(p, FALSE);
		}
		ਨਹੀਂਤਾਂ ਜੇ (*eap->cmd == '$')
		    p = ਮੰਜ਼ਿਲ_name_ਸਿਰਾ(eap->cmd + 1, FALSE);
		ਨਹੀਂਤਾਂ
		    p = eap->cmd + 2;
		ਜੇ (ਸਿਰੇ_excmd(*skipwhite(p)))
		{
		    // "&option <NL>", "$ENV <NL>" and "@r <NL>" are the start
		    // of an expression.
		    eap->cmdidx = CMD_eval;
		    ਵਾਪਸ eap->cmd;
		}
		// "&option" can be followed by "->" or "=", check below
	    }

	    ਜੇ (vim9 && *p == '<')
	    {
		// generic function type args
		ਜੇ (skip_generic_func_type_ਦਲੀਲਾਂ(&p) == FAIL)
		{
		    eap->cmdidx = CMD_ਮਾਪ;
		    ਵਾਪਸ p;
		}
	    }

	    swp = skipwhite(p);

	    ਜੇ (
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
		ਜੇ (*eap->cmd == '{' && ਸਿਰੇ_excmd(*skipwhite(eap->cmd + 1)))
		{
		    // "{" by itself is the start of a block.
		    eap->cmdidx = CMD_ਖੰਡ;
		    ਵਾਪਸ eap->cmd + 1;
		}
		eap->cmdidx = CMD_eval;
		ਵਾਪਸ eap->cmd;
	    }

	    ਜੇ ((p != eap->cmd && (
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
		ਜੇ (skip_ਵਿਅੰਜਕ(&after, ਨਿਰਾ) == OK)
		{
		    after = skipwhite(after);
		    ਜੇ (*after == '=' || (*after != NUL && after[1] == '=')
					 || (after[0] == '.' && after[1] == '.'
							   && after[2] == '='))
			eap->cmdidx = CMD_var;
		}
		--emsg_off;
		ਵਾਪਸ eap->cmd;
	    }

	    // "[...]->Method()" is a list expression, but "[a, b] = Func()" is
	    // an assignment.
	    // If there is no line break inside the "[...]" then "p" is
	    // advanced to after the "]" by to_name_const_end(): check if a "="
	    // follows.
	    // If "[...]" has a line break "p" still points at the "[" and it
	    // can't be an assignment.
	    ਜੇ (*eap->cmd == '[')
	    {
		char_u	    *eq;

		p = ਮੰਜ਼ਿਲ_name_const_ਸਿਰਾ(eap->cmd);
		ਜੇ (p == eap->cmd && *p == '[')
		{
		    ਪੂਰਨ ਗਿਣਤੀ_2 = 0;
		    ਪੂਰਨ	semicolon = FALSE;

		    p = skip_var_list(eap->cmd, TRUE, &ਗਿਣਤੀ_2, &semicolon, TRUE);
		}
		eq = p;
		ਜੇ (eq != ਨਿਰਾ)
		{
		    eq = skipwhite(eq);
		    ਜੇ (vim_strchr((char_u *)"+-*/%.", *eq) != ਨਿਰਾ)
		    {
			ਜੇ (eq[0] == '.' && eq[1] == '.')
			    ++eq;
			++eq;
		    }
		}
		ਜੇ (p == ਨਿਰਾ || p == eap->cmd || *eq != '=')
		{
		    eap->cmdidx = CMD_eval;
		    ਵਾਪਸ eap->cmd;
		}
		ਜੇ (p > eap->cmd && *eq == '=')
		{
		    eap->cmdidx = CMD_var;
		    ਵਾਪਸ eap->cmd;
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
	    ਜੇ (oplen > 0)
	    {
		ਜੇ (((p - eap->cmd) > 2 && eap->cmd[1] == ':')
			|| *eap->cmd == '&'
			|| *eap->cmd == '$'
			|| *eap->cmd == '@'
			|| (eap->skip && IS_WHITE_OR_NUL(
						      *(skipwhite(p) + oplen)))
			|| lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK)
		{
		    eap->cmdidx = CMD_var;
		    ਵਾਪਸ eap->cmd;
		}
	    }

	    // Recognize trying to use a type for a w:, b:, t: or g: variable:
	    // "w:varname: number = 123".
	    ਜੇ (eap->cmd[1] == ':' && *p == ':')
	    {
		eap->cmdidx = CMD_var;
		ਵਾਪਸ eap->cmd;
	    }
	}

	// 1234->func() is a method call
	ਜੇ (ਸੰਖਿਆ_ਵਿਧੀ(eap->cmd))
	{
	    eap->cmdidx = CMD_eval;
	    ਵਾਪਸ eap->cmd;
	}

	// "g:", "s:" and "l:" are always assumed to be a variable, thus start
	// an expression.  A global/substitute/list command needs to use a
	// longer name.
	ਜੇ (vim_strchr((char_u *)"gsl", *p) != ਨਿਰਾ && p[1] == ':')
	{
	    eap->cmdidx = CMD_eval;
	    ਵਾਪਸ eap->cmd;
	}

	// If it is an ID it might be a variable with an operator on the next
	// line, if the variable exists it can't be an Ex command.
	ਜੇ (p > eap->cmd && ਸਿਰੇ_excmd(*skipwhite(p))
		&& (lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK
		    || (ASCII_ISALPHA(eap->cmd[0]) && eap->cmd[1] == ':')))
	{
	    eap->cmdidx = CMD_eval;
	    ਵਾਪਸ eap->cmd;
	}

	// Check for "++nr" and "--nr".
	ਜੇ (p == eap->cmd && p[0] != NUL && p[0] == p[1]
						   && (*p == '+' || *p == '-'))
	{
	    eap->cmdidx = *p == '+' ? CMD_increment : CMD_decrement;
	    ਵਾਪਸ eap->cmd + 2;
	}
    }
#ਜੇ_ਅੰਤ

    /*
     * Isolate the command and search for it in the command table.
     */
    p = eap->cmd;
    ਜੇ (one_letter_cmd(p, &eap->cmdidx))
    {
	++p;
	ਜੇ (ਭਰਿਆ != ਨਿਰਾ)
	    *ਭਰਿਆ = TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜਦੋਂ (ASCII_ISALPHA(*p))
	    ++p;
	// for python 3.x support ":py3", ":python3", ":py3file", etc.
	ਜੇ (eap->cmd[0] == 'p' && eap->cmd[1] == 'y')
	{
	    ਜਦੋਂ (ASCII_ISALNUM(*p))
		++p;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == '9' && STRNCMP("vim9", eap->cmd, 4) == 0)
	{
	    // include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	    ++p;
	    ਜਦੋਂ (ASCII_ISALPHA(*p))
		++p;
	}

	// check for non-alpha command
	ਜੇ (p == eap->cmd && vim_strchr((char_u *)"@*!=><&~#}", *p) != ਨਿਰਾ)
	    ++p;
	len = (ਪੂਰਨ)(p - eap->cmd);
	// The "d" command can directly be followed by 'l' or 'p' flag, when
	// not in Vim9 script.
	ਜੇ (!vim9 && *eap->cmd == 'd' && (p[-1] == 'l' || p[-1] == 'p'))
	{
	    // Check for ":dl", ":dell", etc. to ":deletel": that's
	    // :delete with the 'l' flag.  Same for 'p'.
	    ਲਈ (i = 0; i < len; ++i)
		ਜੇ (eap->cmd[i] != ((char_u *)"delete")[i])
		    ਰੋਕੋ;
	    ਜੇ (i == len - 1)
	    {
		--len;
		ਜੇ (p[-1] == 'l')
		    eap->flags |= EXFLAG_LIST;
		ਨਹੀਂਤਾਂ
		    eap->flags |= EXFLAG_PRINT;
	    }
	}

	ਜੇ (ASCII_ISLOWER(eap->cmd[0]))
	{
	    ਪੂਰਨ c1 = eap->cmd[0];
	    ਪੂਰਨ c2 = len == 1 ? NUL : eap->cmd[1];

	    ਜੇ (command_ਗਿਣਤੀ != (ਪੂਰਨ)CMD_ਮਾਪ)
	    {
		iemsg(e_command_ਸਾਰਣੀ_needs_ਮੰਜ਼ਿਲ_be_updated_ਲੜੀਵਾਰ_make_cmdidxs);
		getout(1);
	    }

	    // Use a precomputed index for fast look-up in cmdnames[]
	    // taking into account the first 2 letters of eap->cmd.
	    eap->cmdidx = cmdidxs1[Char_Ord_ਨੀਵਾਂ(c1)];
	    ਜੇ (ASCII_ISLOWER(c2))
		eap->cmdidx += cmdidxs2[Char_Ord_ਨੀਵਾਂ(c1)][Char_Ord_ਨੀਵਾਂ(c2)];
	}
	ਨਹੀਂਤਾਂ ਜੇ (ASCII_ISUPPER(eap->cmd[0]))
	    eap->cmdidx = CMD_ਅਗਲਾ_3;
	ਨਹੀਂਤਾਂ
	    eap->cmdidx = CMD_bang;

	ਲਈ ( ; (ਪੂਰਨ)eap->cmdidx < (ਪੂਰਨ)CMD_ਮਾਪ;
			       eap->cmdidx = (cmdidx_T)((ਪੂਰਨ)eap->cmdidx + 1))
	    ਜੇ (STRNCMP(cmdnames[(ਪੂਰਨ)eap->cmdidx].cmd_name, (ਅੱਖਰ *)eap->cmd,
							    (size_t)len) == 0)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		ਜੇ (ਭਰਿਆ != ਨਿਰਾ && cmdnames[eap->cmdidx].cmd_name[len] == NUL)
		    *ਭਰਿਆ = TRUE;
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	    }

	// :Print and :mode are not supported in Vim9 script.
	// Some commands cannot be shortened in Vim9 script.
	ਜੇ (vim9 && eap->cmdidx != CMD_ਮਾਪ)
	{
	    ਜੇ (eap->cmdidx == CMD_ਬਹੁਲਕ || eap->cmdidx == CMD_Print)
		eap->cmdidx = CMD_ਮਾਪ;
	    ਨਹੀਂਤਾਂ ਜੇ ((cmdnames[eap->cmdidx].cmd_argt & EX_ਸਾਰਾ)
			  && len < (ਪੂਰਨ)cmdnames[eap->cmdidx].cmd_namelen)
	    {
		semsg(_(e_command_cannot_be_shortened_str), eap->cmd);
		eap->cmdidx = CMD_ਮਾਪ;
	    }
	}

	// Do not recognize ":*" as the star command unless '*' is in
	// 'cpoptions'.
	ਜੇ (eap->cmdidx == CMD_ਤਾਰਾ && vim_strchr(p_cpo, CPO_ਤਾਰਾ) == ਨਿਰਾ)
	    p = eap->cmd;

	// Look for a user defined command as a last resort.  Let ":Print" be
	// overruled by a user defined command.
	ਜੇ ((eap->cmdidx == CMD_ਮਾਪ || eap->cmdidx == CMD_Print)
		&& *eap->cmd >= 'A' && *eap->cmd <= 'Z')
	{
	    // User defined commands may contain digits.
	    ਜਦੋਂ (ASCII_ISALNUM(*p))
		++p;
	    p = ਲੱਭੋ_ucmd(eap, p, ਭਰਿਆ, ਨਿਰਾ, ਨਿਰਾ);
	}
	ਜੇ (p == ਨਿਰਾ || p == eap->cmd)
	    eap->cmdidx = CMD_ਮਾਪ;
    }

    // ":fina" means ":finally" in legacy script, for backwards compatibility.
    ਜੇ (eap->cmdidx == CMD_final && p - eap->cmd == 4 && !vim9)
	eap->cmdidx = CMD_finally;

     // Force ":ho" to be unresolved.  Without this, find_ex_command()
     // matches it to CMD_horizontal (the only "ho*" entry), which makes
     // fullcommand("ho") return "horizontal" even though ":ho" cannot be
     // used as the modifier (cmdmods[] requires 3 chars, "hor").
    ਜੇ (eap->cmdidx == CMD_horizontal && p - eap->cmd == 2)
	eap->cmdidx = CMD_ਮਾਪ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (eap->cmdidx < CMD_ਮਾਪ
	    && vim9
	    && !IS_WHITE_NL_OR_NUL(*p) && *p != '!' && *p != '|'
	    && (eap->cmdidx < 0 ||
		(cmdnames[eap->cmdidx].cmd_argt & EX_NONWHITE_OK) == 0))
    {
	// A name that goes on with an underscore is not this command with an
	// argument: "ch_log" is not ":change".  Leave it to be reported as an
	// invalid command.
	ਜੇ (*p != '_')
	{
	    char_u *cmd = vim_strnsave(eap->cmd, p - eap->cmd);

	    semsg(_(e_command_str_not_followed_by_white_space_str), cmd,
								    eap->cmd);
	    vim_free(cmd);
	}
	eap->cmdidx = CMD_ਮਾਪ;
    }
#ਜੇ_ਅੰਤ

    ਵਾਪਸ p;
}

#ਜੇ defined(FEAT_EVAL)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਅੱਖਰ	*name;
    ਪੂਰਨ		minlen;
    ਪੂਰਨ		has_ਗਿਣਤੀ;  // :123verbose  :3tab
} cmdmod_info_T;

ਸਥਿਰ cmdmod_info_T cmdmod_info_tab[] = {
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
    ਪੂਰਨ
modifier_len(char_u *cmd)
{
    ਪੂਰਨ		i, j;
    char_u	*p = cmd;

    ਜੇ (VIM_ISDIGIT(*cmd))
	p = skipwhite(skipdigits(cmd + 1));
    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(cmdmod_info_tab); ++i)
    {
	// cmdmod_info_tab[] is sorted by name: once the first letter is past
	// the command's first letter no later entry can match.
	ਜੇ (cmdmod_info_tab[i].name[0] > *p)
	    ਰੋਕੋ;
	ਲਈ (j = 0; p[j] != NUL; ++j)
	    ਜੇ (p[j] != cmdmod_info_tab[i].name[j])
		ਰੋਕੋ;
	ਜੇ (!ASCII_ISALPHA(p[j]) && j >= cmdmod_info_tab[i].minlen
					&& (p == cmd || cmdmod_info_tab[i].has_ਗਿਣਤੀ))
	    ਵਾਪਸ j + (ਪੂਰਨ)(p - cmd);
    }
    ਵਾਪਸ 0;
}

/*
 * Return > 0 if an Ex command "name" exists.
 * Return 2 if there is an exact match.
 * Return 3 if there is an ambiguous match.
 */
    ਪੂਰਨ
cmd_exists(char_u *name)
{
    exarg_T	ea;
    ਪੂਰਨ		ਭਰਿਆ = FALSE;
    ਪੂਰਨ		i;
    ਪੂਰਨ		j;
    char_u	*p;

    // Check command modifiers.
    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(cmdmod_info_tab); ++i)
    {
	ਲਈ (j = 0; name[j] != NUL; ++j)
	    ਜੇ (name[j] != cmdmod_info_tab[i].name[j])
		ਰੋਕੋ;
	ਜੇ (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	    ਵਾਪਸ (cmdmod_info_tab[i].name[j] == NUL ? 2 : 1);
    }

    // Check built-in commands and user defined commands.
    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    p = ਲੱਭੋ_ex_command(&ea, &ਭਰਿਆ, ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ 3;
    ਜੇ (vim_isdigit(*name) && ea.cmdidx != CMD_ਮੇਲ)
	ਵਾਪਸ 0;
    ਜੇ (*skipwhite(p) != NUL)
	ਵਾਪਸ 0;	// trailing garbage
    ਵਾਪਸ (ea.cmdidx == CMD_ਮਾਪ ? 0 : (ਭਰਿਆ ? 2 : 1));
}

/*
 * Add the attributes of an Ex command to "d" in the terms of ":command":
 * "argt" are the EX_ flags, "addr_type" the address type and "def" the
 * default for a range or count, -1 when there is none.
 */
    ਖਾਲੀ
ex_command_attrs(dict_T *d, long_u argt, cmd_addr_T addr_type, ਲੰਮਾ def)
{
    ਸਥਿਰ ਅੱਖਰ *addr_ਨਾਂ[] = {
	"lines", "windows", "arguments", "loaded_buffers", "buffers", "tabs",
	"tabs_relative", "quickfix_valid", "quickfix", "unsigned", "other",
	"none"
    };
    ਅੱਖਰ	*nargs;

    ਜੇ (!(argt & EX_EXTRA))
	nargs = "0";
    ਨਹੀਂਤਾਂ ਜੇ (!(argt & EX_NOSPC))
	nargs = (argt & EX_NEEDARG) ? "+" : "*";
    ਨਹੀਂਤਾਂ ਜੇ (!(argt & EX_NEEDARG))
	nargs = "?";
    ਨਹੀਂਤਾਂ
	nargs = (argt & EX_ARGSPACE) ? "_" : "1";
    dict_ਜੋੜੋ_string(d, "nargs", (char_u *)nargs);

    ਜੇ (!(argt & EX_RANGE))
	dict_ਜੋੜੋ_bool(d, "range", FALSE);
    ਨਹੀਂਤਾਂ ਜੇ (argt & EX_DFLALL)
	dict_ਜੋੜੋ_string(d, "range", (char_u *)"%");
    ਨਹੀਂਤਾਂ ਜੇ (def >= 0 && !(argt & EX_ਗਿਣਤੀ))
	dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "range", def);
    ਨਹੀਂਤਾਂ
	dict_ਜੋੜੋ_string(d, "range", (char_u *)".");
    ਜੇ (!(argt & EX_ਗਿਣਤੀ))
	dict_ਜੋੜੋ_bool(d, "count", FALSE);
    ਨਹੀਂਤਾਂ
	dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "count", def < 0 ? 0 : def);
    dict_ਜੋੜੋ_bool(d, "bang", (argt & EX_BANG) != 0);
    dict_ਜੋੜੋ_bool(d, "bar", (argt & EX_TRLBAR) != 0);
    dict_ਜੋੜੋ_bool(d, "register", (argt & EX_REGSTR) != 0);
    dict_ਜੋੜੋ_string(d, "addr", (char_u *)addr_ਨਾਂ[addr_type]);
}

/*
 * Add what getinfo() reports for the Ex command "name" to "d".  "vim9" is TRUE
 * to find the command as in Vim9 script, FALSE as in legacy script, -1 for the
 * context of the caller.
 * Returns FAIL when there is no such command or the abbreviation is
 * ambiguous.
 */
    ਪੂਰਨ
ex_command_info(char_u *name, ਪੂਰਨ vim9, dict_T *d)
{
    exarg_T	ea;
    char_u	*p;
    ਪੂਰਨ		i;
    ਪੂਰਨ		j;
    ਪੂਰਨ		save_cmod_flags = cmdmod.cmod_flags;
    ਪੂਰਨ		ret = OK;

    ਜੇ (vim9 != -1)
    {
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9 ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    // An abbreviation of a command modifier, like ":bo" for ":botright".
    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(cmdmod_info_tab); ++i)
    {
	ਲਈ (j = 0; name[j] != NUL; ++j)
	    ਜੇ (name[j] != cmdmod_info_tab[i].name[j])
		ਰੋਕੋ;
	ਜੇ (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	{
	    name = (char_u *)cmdmod_info_tab[i].name;
	    ਰੋਕੋ;
	}
    }

    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    ea.addr_ਗਿਣਤੀ = 0;
    // Don't complain about using "en" or ":let" in Vim9 script.
    ++emsg_silent;
    p = ਲੱਭੋ_ex_command(&ea, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ (p == ਨਿਰਾ || ea.cmdidx == CMD_ਮਾਪ
	    || (vim_isdigit(*name) && ea.cmdidx != CMD_ਮੇਲ)
	    || *skipwhite(p) != NUL
	    || not_ਵਿੱਚ_vim9(&ea, true) == FAIL)
	ret = FAIL;
    --emsg_silent;

    ਜੇ (ret == FAIL)
	;
    ਨਹੀਂਤਾਂ ਜੇ (IS_USER_CMDIDX(ea.cmdidx))
	ret = user_command_info(ea.useridx, ea.cmdidx, d);
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	notimpl = cmdnames[ea.cmdidx].cmd_func == ex_ni
# ifdef HAVE_EX_SCRIPT_NI
			    || cmdnames[ea.cmdidx].cmd_func == ex_script_ni
# endif
			    ;

	dict_ਜੋੜੋ_string(d, "name", cmdnames[ea.cmdidx].cmd_name);
	dict_ਜੋੜੋ_string(d, "kind", (char_u *)(
		    cmdnames[ea.cmdidx].cmd_func == ex_wrongmodifier
						    ? "modifier" : "builtin"));
	dict_ਜੋੜੋ_bool(d, "available", !notimpl);
	ex_command_attrs(d, cmdnames[ea.cmdidx].cmd_argt,
				       cmdnames[ea.cmdidx].cmd_addr_type, -1);
    }
    cmdmod.cmod_flags = save_cmod_flags;
    ਵਾਪਸ ret;
}

/*
 * "fullcommand" function
 */
    ਖਾਲੀ
f_fullcommand(typval_T *argvars, typval_T *rettv)
{
    exarg_T	ea;
    char_u	*name;
    char_u	*p;
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();
    ਪੂਰਨ		save_cmod_flags = cmdmod.cmod_flags;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    name = tv_get_string(&argvars[0]);
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	vim9script = tv_get_bool(&argvars[1]);
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9script ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    ਜਦੋਂ (*name == ':')
	name++;
    name = skip_range(name, TRUE, ਨਿਰਾ);

    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.addr_ਗਿਣਤੀ = 0;
    ++emsg_silent;  // don't complain about using "en" in Vim9 script
    p = ਲੱਭੋ_ex_command(&ea, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    --emsg_silent;
    ਜੇ (p == ਨਿਰਾ || ea.cmdidx == CMD_ਮਾਪ)
	ਜਾਓ theend;

    ਜੇ (vim9script && not_ਵਿੱਚ_vim9(&ea, true) == FAIL)
	ਜਾਓ theend;

    rettv->vval.v_string = vim_strsave(IS_USER_CMDIDX(ea.cmdidx)
				 ? get_user_command_name(ea.useridx, ea.cmdidx)
				 : cmdnames[ea.cmdidx].cmd_name);
theend:
    cmdmod.cmod_flags = save_cmod_flags;
}
#ਜੇ_ਅੰਤ

    cmdidx_T
excmd_get_cmdidx(char_u *cmd, ਪੂਰਨ len)
{
    cmdidx_T idx;

    ਜੇ (!one_letter_cmd(cmd, &idx))
	ਲਈ (idx = (cmdidx_T)0; (ਪੂਰਨ)idx < (ਪੂਰਨ)CMD_ਮਾਪ;
		idx = (cmdidx_T)((ਪੂਰਨ)idx + 1))
	    ਜੇ (STRNCMP(cmdnames[(ਪੂਰਨ)idx].cmd_name, cmd, (size_t)len) == 0)
		ਰੋਕੋ;

    ਵਾਪਸ idx;
}

    ਲੰਮਾ
excmd_get_argt(cmdidx_T idx)
{
    ਵਾਪਸ (ਲੰਮਾ)cmdnames[(ਪੂਰਨ)idx].cmd_argt;
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
    char_u	*cmd_ਸ਼ੁਰੂ,
    ਪੂਰਨ		skip_ਤਾਰਾ,	// skip "*" used for Visual range
    ਪੂਰਨ		*ctx)		// pointer to xp_context or NULL
{
    char_u	*cmd = cmd_ਸ਼ੁਰੂ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	delim;

    ਜਦੋਂ (vim_strchr((char_u *)" \t0123456789.$%'/?-+,;\\", *cmd) != ਨਿਰਾ)
    {
	ਜੇ (*cmd == '\\')
	{
	    ਜੇ (cmd[1] == '?' || cmd[1] == '/' || cmd[1] == '&')
		++cmd;
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*cmd == '\'')
	{
	    char_u *p = cmd;

	    // a quote is only valid at the start or after a separator
	    ਜਦੋਂ (p > cmd_ਸ਼ੁਰੂ)
	    {
		--p;
		ਜੇ (!VIM_ISWHITE(*p))
		    ਰੋਕੋ;
	    }
	    ਜੇ (cmd > cmd_ਸ਼ੁਰੂ && !VIM_ISWHITE(*p) && *p != ',' && *p != ';')
		ਰੋਕੋ;
	    ਜੇ (*++cmd == NUL && ctx != ਨਿਰਾ)
		*ctx = EXPAND_NOTHING;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*cmd == '/' || *cmd == '?')
	{
	    delim = *cmd++;
	    ਜਦੋਂ (*cmd != NUL && *cmd != delim)
		ਜੇ (*cmd++ == '\\' && *cmd != NUL)
		    ++cmd;
	    ਜੇ (*cmd == NUL && ctx != ਨਿਰਾ)
		*ctx = EXPAND_NOTHING;
	}
	ਜੇ (*cmd != NUL)
	    ++cmd;
    }

    // Skip ":" and white space.
    ਜਦੋਂ (*cmd == ':')
	cmd = skipwhite(cmd + 1);

    // Skip "*" used for Visual range.
    ਜੇ (skip_ਤਾਰਾ && *cmd == '*' && vim_strchr(p_cpo, CPO_ਤਾਰਾ) == ਨਿਰਾ)
	cmd = skipwhite(cmd + 1);

    ਵਾਪਸ cmd;
}

    ਸਥਿਰ ਖਾਲੀ
addr_error(cmd_addr_T addr_type)
{
    ਜੇ (addr_type == ADDR_NONE)
	emsg(_(e_no_range_allowed));
    ਨਹੀਂਤਾਂ
	emsg(_(e_invalid_range));
}

/*
 * Return the default address for an address type.
 */
    ਸਥਿਰ linenr_T
default_ਪਤਾ(exarg_T *eap)
{
    linenr_T lnum = 0;

    ਚੋਣ (eap->addr_type)
    {
	ਹਾਲਤ ADDR_ਸਤਰਾਂ:
	ਹਾਲਤ ADDR_OTHER:
	    // Default is the cursor line number.  Avoid using an invalid
	    // line number though.
	    ਜੇ (curwin->w_cursor.lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    ਨਹੀਂਤਾਂ
		lnum = curwin->w_cursor.lnum;
	    ਰੋਕੋ;
	ਹਾਲਤ ADDR_WINDOWS:
	    lnum = CURRENT_WIN_NR;
	    ਰੋਕੋ;
	ਹਾਲਤ ADDR_ARGUMENTS:
	    lnum = curwin->w_arg_idx + 1;
	    ਜੇ (lnum > ARGCOUNT)
		lnum = ARGCOUNT;
	    ਰੋਕੋ;
	ਹਾਲਤ ADDR_LOADED_BUFFERS:
	ਹਾਲਤ ADDR_BUFFERS:
	    lnum = curbuf->b_fnum;
	    ਰੋਕੋ;
	ਹਾਲਤ ADDR_TABS:
	    lnum = CURRENT_TAB_NR;
	    ਰੋਕੋ;
	ਹਾਲਤ ADDR_TABS_RELATIVE:
	ਹਾਲਤ ADDR_UNSIGNED:
	    lnum = 1;
	    ਰੋਕੋ;
	ਹਾਲਤ ADDR_QUICKFIX:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
	    lnum = qf_get_cur_idx(eap);
#ਜੇ_ਅੰਤ
	    ਰੋਕੋ;
	ਹਾਲਤ ADDR_QUICKFIX_VALID:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
	    lnum = qf_get_cur_valid_idx(eap);
#ਜੇ_ਅੰਤ
	    ਰੋਕੋ;
	ਹਾਲਤ ADDR_NONE:
	    // Will give an error later if a range is found.
	    ਰੋਕੋ;
    }
    ਵਾਪਸ lnum;
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
get_ਪਤਾ(
    exarg_T	*eap ਅਣਵਰਤਿਆ,
    char_u	**ptr,
    cmd_addr_T	addr_type,
    ਪੂਰਨ		skip,		// only skip the address, don't use it
    ਪੂਰਨ		silent,		// no errors or side effects
    ਪੂਰਨ		ਮੰਜ਼ਿਲ_other_file,  // flag: may jump to other file
    ਪੂਰਨ		ਪਤਾ_ਗਿਣਤੀ ਅਣਵਰਤਿਆ) // 1 for first address, >1 after comma
{
    ਪੂਰਨ		c;
    ਪੂਰਨ		i;
    ਲੰਮਾ	n;
    char_u	*cmd;
    pos_T	pos;
    pos_T	*fp;
    linenr_T	lnum;
    buf_T	*buf;

    cmd = skipwhite(*ptr);
    lnum = MAXLNUM;
    ਕਰੋ
    {
	ਚੋਣ (*cmd)
	{
	    ਹਾਲਤ '.':			    // '.' - Cursor position
		++cmd;
		ਚੋਣ (addr_type)
		{
		    ਹਾਲਤ ADDR_ਸਤਰਾਂ:
		    ਹਾਲਤ ADDR_OTHER:
			lnum = curwin->w_cursor.lnum;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_LOADED_BUFFERS:
		    ਹਾਲਤ ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_NONE:
		    ਹਾਲਤ ADDR_TABS_RELATIVE:
		    ਹਾਲਤ ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = ਨਿਰਾ;
			ਜਾਓ error;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_QUICKFIX:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#ਜੇ_ਅੰਤ
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_QUICKFIX_VALID:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#ਜੇ_ਅੰਤ
			ਰੋਕੋ;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ '$':			    // '$' - last line
		++cmd;
		ਚੋਣ (addr_type)
		{
		    ਹਾਲਤ ADDR_ਸਤਰਾਂ:
		    ਹਾਲਤ ADDR_OTHER:
			lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_WINDOWS:
			lnum = ਆਖਰੀ_WIN_NR;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_ARGUMENTS:
			lnum = ARGCOUNT;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_LOADED_BUFFERS:
			buf = lastbuf;
			ਜਦੋਂ (buf->b_ml.ml_mfp == ਨਿਰਾ)
			{
			    ਜੇ (buf->b_prev == ਨਿਰਾ)
				ਰੋਕੋ;
			    buf = buf->b_prev;
			}
			lnum = buf->b_fnum;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_BUFFERS:
			lnum = lastbuf->b_fnum;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_TABS:
			lnum = ਆਖਰੀ_TAB_NR;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_NONE:
		    ਹਾਲਤ ADDR_TABS_RELATIVE:
		    ਹਾਲਤ ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = ਨਿਰਾ;
			ਜਾਓ error;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_QUICKFIX:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
			lnum = qf_get_ਮਾਪ(eap);
			ਜੇ (lnum == 0)
			    lnum = 1;
#ਜੇ_ਅੰਤ
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_QUICKFIX_VALID:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
			lnum = qf_get_valid_ਮਾਪ(eap);
			ਜੇ (lnum == 0)
			    lnum = 1;
#ਜੇ_ਅੰਤ
			ਰੋਕੋ;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ '\'':			    // ''' - mark
		ਜੇ (*++cmd == NUL)
		{
		    cmd = ਨਿਰਾ;
		    ਜਾਓ error;
		}
		ਜੇ (addr_type != ADDR_ਸਤਰਾਂ)
		{
		    addr_error(addr_type);
		    cmd = ਨਿਰਾ;
		    ਜਾਓ error;
		}
		ਜੇ (skip)
		    ++cmd;
		ਨਹੀਂਤਾਂ
		{
		    // Only accept a mark in another file when it is
		    // used by itself: ":'M".
		    fp = getmark(*cmd, ਮੰਜ਼ਿਲ_other_file && cmd[1] == NUL);
		    ++cmd;
		    ਜੇ (fp == (pos_T *)-1)
			// Jumped to another file.
			lnum = curwin->w_cursor.lnum;
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (check_mark(fp) == FAIL)
			{
			    cmd = ਨਿਰਾ;
			    ਜਾਓ error;
			}
			lnum = fp->lnum;
		    }
		}
		ਰੋਕੋ;

	    ਹਾਲਤ '/':
	    ਹਾਲਤ '?':			// '/' or '?' - search
		c = *cmd++;
		ਜੇ (addr_type != ADDR_ਸਤਰਾਂ)
		{
		    addr_error(addr_type);
		    cmd = ਨਿਰਾ;
		    ਜਾਓ error;
		}
		ਜੇ (skip)	// skip "/pat/"
		{
		    cmd = skip_regexp(cmd, c, magic_isset());
		    ਜੇ (*cmd == c)
			++cmd;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਪੂਰਨ flags;

		    pos = curwin->w_cursor; // save curwin->w_cursor

		    // When '/' or '?' follows another address, start from
		    // there.
		    ਜੇ (lnum > 0 && lnum != MAXLNUM)
			curwin->w_cursor.lnum =
				lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
					   ? curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ : lnum;

		    // Start a forward search at the end of the line (unless
		    // before the first line).
		    // Start a backward search at the start of the line.
		    // This makes sure we never match in the current
		    // line, and can match anywhere in the
		    // next/previous line.
		    ਜੇ (c == '/' && curwin->w_cursor.lnum > 0)
			curwin->w_cursor.ਖਾਨਾ = MAXCOL;
		    ਨਹੀਂਤਾਂ
			curwin->w_cursor.ਖਾਨਾ = 0;
		    searchcmdlen = 0;
		    flags = silent ? ਖੋਜ_KEEP : ਖੋਜ_HIS | ਖੋਜ_ਸੁਨੇਹਾ;
		    ਜੇ (!do_ਖੋਜ(ਨਿਰਾ, c, c, cmd, STRLEN(cmd), 1L, flags, ਨਿਰਾ))
		    {
			curwin->w_cursor = pos;
			cmd = ਨਿਰਾ;
			ਜਾਓ error;
		    }
		    lnum = curwin->w_cursor.lnum;
		    curwin->w_cursor = pos;
		    // adjust command string pointer
		    cmd += searchcmdlen;
		}
		ਰੋਕੋ;

	    ਹਾਲਤ '\\':		    // "\?", "\/" or "\&", repeat search
		++cmd;
		ਜੇ (addr_type != ADDR_ਸਤਰਾਂ)
		{
		    addr_error(addr_type);
		    cmd = ਨਿਰਾ;
		    ਜਾਓ error;
		}
		ਜੇ (*cmd == '&')
		    i = RE_SUBST;
		ਨਹੀਂਤਾਂ ਜੇ (*cmd == '?' || *cmd == '/')
		    i = RE_ਖੋਜ;
		ਨਹੀਂਤਾਂ
		{
		    emsg(_(e_backslash_should_be_followed_by));
		    cmd = ਨਿਰਾ;
		    ਜਾਓ error;
		}

		ਜੇ (!skip)
		{
		    /*
		     * When search follows another address, start from
		     * there.
		     */
		    ਜੇ (lnum != MAXLNUM)
			pos.lnum = lnum;
		    ਨਹੀਂਤਾਂ
			pos.lnum = curwin->w_cursor.lnum;

		    /*
		     * Start the search just like for the above
		     * do_search().
		     */
		    ਜੇ (*cmd != '?')
			pos.ਖਾਨਾ = MAXCOL;
		    ਨਹੀਂਤਾਂ
			pos.ਖਾਨਾ = 0;
		    pos.coladd = 0;
		    ਜੇ (searchit(curwin, curbuf, &pos, ਨਿਰਾ,
				*cmd == '?' ? BACKWARD : FORWARD,
				(char_u *)"", 0, 1L, ਖੋਜ_ਸੁਨੇਹਾ, i, ਨਿਰਾ) != FAIL)
			lnum = pos.lnum;
		    ਨਹੀਂਤਾਂ
		    {
			cmd = ਨਿਰਾ;
			ਜਾਓ error;
		    }
		}
		++cmd;
		ਰੋਕੋ;

	    ਮੂਲ:
		ਜੇ (VIM_ISDIGIT(*cmd))	// absolute line number
		    lnum = getdigits(&cmd);
	}

	ਲਈ (;;)
	{
	    cmd = skipwhite(cmd);
	    ਜੇ (*cmd != '-' && *cmd != '+' && !VIM_ISDIGIT(*cmd))
		ਰੋਕੋ;

	    ਜੇ (lnum == MAXLNUM)
	    {
		ਚੋਣ (addr_type)
		{
		    ਹਾਲਤ ADDR_ਸਤਰਾਂ:
		    ਹਾਲਤ ADDR_OTHER:
			// "+1" is same as ".+1"
			lnum = curwin->w_cursor.lnum;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_LOADED_BUFFERS:
		    ਹਾਲਤ ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_TABS_RELATIVE:
			lnum = 1;
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_QUICKFIX:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#ਜੇ_ਅੰਤ
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_QUICKFIX_VALID:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#ਜੇ_ਅੰਤ
			ਰੋਕੋ;
		    ਹਾਲਤ ADDR_NONE:
		    ਹਾਲਤ ADDR_UNSIGNED:
			lnum = 0;
			ਰੋਕੋ;
		}
	    }

	    ਜੇ (VIM_ISDIGIT(*cmd))
		i = '+';		// "number" is same as "+number"
	    ਨਹੀਂਤਾਂ
		i = *cmd++;
	    ਜੇ (!VIM_ISDIGIT(*cmd))	// '+' is '+1'
		n = 1;
	    ਨਹੀਂਤਾਂ
	    {
		// "number", "+number" or "-number"
		n = getdigits(&cmd);
		ਜੇ (n == MAXLNUM)
		{
		    emsg(_(e_ਸਤਰ_ਸੰਖਿਆ_ਬਾਹਰਲਾ_ਦਾ_range));
		    cmd = ਨਿਰਾ;
		    ਜਾਓ error;
		}
	    }

	    ਜੇ (addr_type == ADDR_TABS_RELATIVE)
	    {
		emsg(_(e_invalid_range));
		cmd = ਨਿਰਾ;
		ਜਾਓ error;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (addr_type == ADDR_LOADED_BUFFERS
		    || addr_type == ADDR_BUFFERS)
		lnum = compute_ਬਫਰ_local_ਗਿਣਤੀ(
				    addr_type, lnum, (i == '-') ? -1 * n : n);
	    ਨਹੀਂਤਾਂ
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		// Relative line addressing: need to adjust for lines in a
		// closed fold after the first address.
		ਜੇ (addr_type == ADDR_ਸਤਰਾਂ && (i == '-' || i == '+')
							 && ਪਤਾ_ਗਿਣਤੀ >= 2)
		    (ਖਾਲੀ)hasFolding(lnum, ਨਿਰਾ, &lnum);
#ਜੇ_ਅੰਤ
		ਜੇ (i == '-')
		    lnum -= n;
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (lnum >= 0 && n >= LONG_MAX - lnum)
		    {
			emsg(_(e_ਸਤਰ_ਸੰਖਿਆ_ਬਾਹਰਲਾ_ਦਾ_range));
			cmd = ਨਿਰਾ;
			ਜਾਓ error;
		    }
		    lnum += n;
		}
	    }
	}
    } ਜਦੋਂ (*cmd == '/' || *cmd == '?');

error:
    *ptr = cmd;
    ਵਾਪਸ lnum;
}

/*
 * Set eap->line1 and eap->line2 to the whole range.
 * Used for commands with the EX_DFLALL flag and no range given.
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਤਾ_default_all(exarg_T *eap)
{
    eap->line1 = 1;
    ਚੋਣ (eap->addr_type)
    {
	ਹਾਲਤ ADDR_ਸਤਰਾਂ:
	ਹਾਲਤ ADDR_OTHER:
	    eap->line2 = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    ਰੋਕੋ;
	ਹਾਲਤ ADDR_LOADED_BUFFERS:
	    {
		buf_T *buf = firstbuf;

		ਜਦੋਂ (buf->b_ਅਗਲਾ != ਨਿਰਾ && buf->b_ml.ml_mfp == ਨਿਰਾ)
		    buf = buf->b_ਅਗਲਾ;
		eap->line1 = buf->b_fnum;
		buf = lastbuf;
		ਜਦੋਂ (buf->b_prev != ਨਿਰਾ && buf->b_ml.ml_mfp == ਨਿਰਾ)
		    buf = buf->b_prev;
		eap->line2 = buf->b_fnum;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ ADDR_BUFFERS:
	    eap->line1 = firstbuf->b_fnum;
	    eap->line2 = lastbuf->b_fnum;
	    ਰੋਕੋ;
	ਹਾਲਤ ADDR_WINDOWS:
	    eap->line2 = ਆਖਰੀ_WIN_NR;
	    ਰੋਕੋ;
	ਹਾਲਤ ADDR_TABS:
	    eap->line2 = ਆਖਰੀ_TAB_NR;
	    ਰੋਕੋ;
	ਹਾਲਤ ADDR_TABS_RELATIVE:
	    eap->line2 = 1;
	    ਰੋਕੋ;
	ਹਾਲਤ ADDR_ARGUMENTS:
	    ਜੇ (ARGCOUNT == 0)
		eap->line1 = eap->line2 = 0;
	    ਨਹੀਂਤਾਂ
		eap->line2 = ARGCOUNT;
	    ਰੋਕੋ;
	ਹਾਲਤ ADDR_QUICKFIX_VALID:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
	    eap->line2 = qf_get_valid_ਮਾਪ(eap);
	    ਜੇ (eap->line2 == 0)
		eap->line2 = 1;
#ਜੇ_ਅੰਤ
	    ਰੋਕੋ;
	ਹਾਲਤ ADDR_NONE:
	ਹਾਲਤ ADDR_UNSIGNED:
	ਹਾਲਤ ADDR_QUICKFIX:
	    iemsg("Cannot use EX_DFLALL with ADDR_NONE, ADDR_UNSIGNED or ADDR_QUICKFIX");
	    ਰੋਕੋ;
    }
}


/*
 * Get flags from an Ex command argument.
 */
    ਸਥਿਰ ਖਾਲੀ
get_flags(exarg_T *eap)
{
    ਜਦੋਂ (vim_strchr((char_u *)"lp#", *eap->arg) != ਨਿਰਾ)
    {
	ਜੇ (*eap->arg == 'l')
	    eap->flags |= EXFLAG_LIST;
	ਨਹੀਂਤਾਂ ਜੇ (*eap->arg == 'p')
	    eap->flags |= EXFLAG_PRINT;
	ਨਹੀਂਤਾਂ
	    eap->flags |= EXFLAG_NR;
	eap->arg = skipwhite(eap->arg + 1);
    }
}

/*
 * Function called for command which is Not Implemented.  NI!
 */
    ਖਾਲੀ
ex_ni(exarg_T *eap)
{
    ਜੇ (!eap->skip)
	eap->errmsg =
		_(e_sorry_command_is_not_ਉਪਲਬਧ_ਵਿੱਚ_this_ਸੰਸਕਰਣ);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_EX_SCRIPT_NI
/*
 * Function called for script command which is Not Implemented.  NI!
 * Skips over ":perl <<EOF" constructs.
 */
    ਸਥਿਰ ਖਾਲੀ
ex_script_ni(exarg_T *eap)
{
    ਜੇ (!eap->skip)
	ex_ni(eap);
    ਨਹੀਂਤਾਂ
	vim_free(script_get(eap, eap->arg));
}
#ਜੇ_ਅੰਤ

/*
 * Check range in Ex command for validity.
 * Return NULL when valid, error message when invalid.
 */
    ਸਥਿਰ ਅੱਖਰ *
invalid_range(exarg_T *eap)
{
    buf_T	*buf;

    ਜੇ (       eap->line1 < 0
	    || eap->line2 < 0
	    || eap->line1 > eap->line2)
	ਵਾਪਸ _(e_invalid_range);

    ਜੇ (eap->argt & EX_RANGE)
    {
	ਚੋਣ (eap->addr_type)
	{
	    ਹਾਲਤ ADDR_ਸਤਰਾਂ:
		ਜੇ (eap->line2 > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
			    + (eap->cmdidx == CMD_diffget ||
				eap->cmdidx == CMD_diffput)
#ਜੇ_ਅੰਤ
		   )
		    ਵਾਪਸ _(e_invalid_range);
		ਰੋਕੋ;
	    ਹਾਲਤ ADDR_ARGUMENTS:
		// add 1 if ARGCOUNT is 0
		ਜੇ (eap->line2 > ARGCOUNT + (!ARGCOUNT))
		    ਵਾਪਸ _(e_invalid_range);
		ਰੋਕੋ;
	    ਹਾਲਤ ADDR_BUFFERS:
		// Only a boundary check, not whether the buffers actually
		// exist.
		ਜੇ (eap->line1 < 1 || eap->line2 > get_highest_fnum())
		    ਵਾਪਸ _(e_invalid_range);
		ਰੋਕੋ;
	    ਹਾਲਤ ADDR_LOADED_BUFFERS:
		buf = firstbuf;
		ਜਦੋਂ (buf->b_ml.ml_mfp == ਨਿਰਾ)
		{
		    ਜੇ (buf->b_ਅਗਲਾ == ਨਿਰਾ)
			ਵਾਪਸ _(e_invalid_range);
		    buf = buf->b_ਅਗਲਾ;
		}
		ਜੇ (eap->line1 < buf->b_fnum)
		    ਵਾਪਸ _(e_invalid_range);
		buf = lastbuf;
		ਜਦੋਂ (buf->b_ml.ml_mfp == ਨਿਰਾ)
		{
		    ਜੇ (buf->b_prev == ਨਿਰਾ)
			ਵਾਪਸ _(e_invalid_range);
		    buf = buf->b_prev;
		}
		ਜੇ (eap->line2 > buf->b_fnum)
		    ਵਾਪਸ _(e_invalid_range);
		ਰੋਕੋ;
	    ਹਾਲਤ ADDR_WINDOWS:
		ਜੇ (eap->line2 > ਆਖਰੀ_WIN_NR)
		    ਵਾਪਸ _(e_invalid_range);
		ਰੋਕੋ;
	    ਹਾਲਤ ADDR_TABS:
		ਜੇ (eap->line2 > ਆਖਰੀ_TAB_NR)
		    ਵਾਪਸ _(e_invalid_range);
		ਰੋਕੋ;
	    ਹਾਲਤ ADDR_TABS_RELATIVE:
	    ਹਾਲਤ ADDR_OTHER:
		// Any range is OK.
		ਰੋਕੋ;
	    ਹਾਲਤ ADDR_QUICKFIX:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
		// No error for value that is too big, will use the last entry.
		ਜੇ (eap->line2 <= 0)
		{
		    ਜੇ (eap->addr_ਗਿਣਤੀ == 0)
			ਵਾਪਸ _(e_no_errors);
		    ਵਾਪਸ _(e_invalid_range);
		}
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	    ਹਾਲਤ ADDR_QUICKFIX_VALID:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
		ਜੇ ((eap->line2 != 1 && eap->line2 > qf_get_valid_ਮਾਪ(eap))
			|| eap->line2 < 0)
		    ਵਾਪਸ _(e_invalid_range);
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	    ਹਾਲਤ ADDR_UNSIGNED:
	    ਹਾਲਤ ADDR_NONE:
		// Will give an error elsewhere.
		ਰੋਕੋ;
	}
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Correct the range for zero line number, if required.
 */
    ਸਥਿਰ ਖਾਲੀ
correct_range(exarg_T *eap)
{
    ਜੇ (!(eap->argt & EX_ZEROR))	    // zero in range not allowed
    {
	ਜੇ (eap->line1 == 0)
	    eap->line1 = 1;
	ਜੇ (eap->line2 == 0)
	    eap->line2 = 1;
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
/*
 * For a ":vimgrep" or ":vimgrepadd" command return a pointer past the
 * pattern.  Otherwise return eap->arg.
 */
    ਸਥਿਰ char_u *
skip_grep_pat(exarg_T *eap)
{
    char_u	*p = eap->arg;

    ਜੇ (*p != NUL && (eap->cmdidx == CMD_vimgrep || eap->cmdidx == CMD_lvimgrep
		|| eap->cmdidx == CMD_vimgrepadd
		|| eap->cmdidx == CMD_lvimgrepadd
		|| grep_ਅੰਦਰੂਨੀ(eap->cmdidx)))
    {
	p = skip_vimgrep_pat(p, ਨਿਰਾ, ਨਿਰਾ);
	ਜੇ (p == ਨਿਰਾ)
	    p = eap->arg;
    }
    ਵਾਪਸ p;
}

/*
 * For the ":make" and ":grep" commands insert the 'makeprg'/'grepprg' option
 * in the command line, so that things like % get expanded.
 */
    ਸਥਿਰ char_u *
replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep)
{
    char_u	*new_cmdline;
    char_u	*ਪ੍ਰੋਗਰਾਮ;
    char_u	*pos;
    char_u	*ptr;
    ਪੂਰਨ		len;
    ਪੂਰਨ		i;

    /*
     * Don't do it when ":vimgrep" is used for ":grep".
     */
    ਜੇ ((eap->cmdidx == CMD_make || eap->cmdidx == CMD_lmake
		     || eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
		     || eap->cmdidx == CMD_grepadd
		     || eap->cmdidx == CMD_lgrepadd)
	    && !grep_ਅੰਦਰੂਨੀ(eap->cmdidx))
    {
	ਜੇ (eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
	    || eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	{
	    ਜੇ (*curbuf->b_p_gp == NUL)
		ਪ੍ਰੋਗਰਾਮ = p_gp;
	    ਨਹੀਂਤਾਂ
		ਪ੍ਰੋਗਰਾਮ = curbuf->b_p_gp;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (*curbuf->b_p_mp == NUL)
		ਪ੍ਰੋਗਰਾਮ = p_mp;
	    ਨਹੀਂਤਾਂ
		ਪ੍ਰੋਗਰਾਮ = curbuf->b_p_mp;
	}

	p = skipwhite(p);

	ਜੇ ((pos = (char_u *)ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)ਪ੍ਰੋਗਰਾਮ, "$*")) != ਨਿਰਾ)
	{
	    // replace $* by given arguments
	    i = 1;
	    ਜਦੋਂ ((pos = (char_u *)ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)pos + 2, "$*")) != ਨਿਰਾ)
		++i;
	    len = (ਪੂਰਨ)STRLEN(p);
	    new_cmdline = alloc(STRLEN(ਪ੍ਰੋਗਰਾਮ) + (size_t)i * (len - 2) + 1);
	    ਜੇ (new_cmdline == ਨਿਰਾ)
		ਵਾਪਸ ਨਿਰਾ;			// out of memory
	    ptr = new_cmdline;
	    ਜਦੋਂ ((pos = (char_u *)ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)ਪ੍ਰੋਗਰਾਮ, "$*")) != ਨਿਰਾ)
	    {
		i = (ਪੂਰਨ)(pos - ਪ੍ਰੋਗਰਾਮ);
		STRNCPY(ptr, ਪ੍ਰੋਗਰਾਮ, i);
		STRCPY(ptr += i, p);
		ptr += len;
		ਪ੍ਰੋਗਰਾਮ = pos + 2;
	    }
	    STRCPY(ptr, ਪ੍ਰੋਗਰਾਮ);
	}
	ਨਹੀਂਤਾਂ
	{
	    size_t ਪ੍ਰੋਗਰਾਮ_len = STRLEN(ਪ੍ਰੋਗਰਾਮ);

	    new_cmdline = alloc(ਪ੍ਰੋਗਰਾਮ_len + STRLEN(p) + 2);
	    ਜੇ (new_cmdline == ਨਿਰਾ)
		ਵਾਪਸ ਨਿਰਾ;			// out of memory
	    STRCPY(new_cmdline, ਪ੍ਰੋਗਰਾਮ);
	    STRCPY(new_cmdline + ਪ੍ਰੋਗਰਾਮ_len, " ");
	    STRCPY(new_cmdline + ਪ੍ਰੋਗਰਾਮ_len + 1, p);
	}
	ਸੁਨੇਹਾ_make(p);

	// 'eap->cmd' is not set here, because it is not used at CMD_make
	vim_free(*cmdlinep);
	*cmdlinep = new_cmdline;
	p = new_cmdline;
    }
    ਵਾਪਸ p;
}
#ਜੇ_ਅੰਤ

/*
 * Expand file name in Ex command argument.
 * When an error is detected, "errormsgp" is set to a non-NULL pointer.
 * Return FAIL for failure, OK otherwise.
 */
    ਪੂਰਨ
expand_filename(
    exarg_T	*eap,
    char_u	**cmdlinep,
    ਅੱਖਰ	**errormsgp)
{
    ਪੂਰਨ		has_wildcards;	// need to expand wildcards
    char_u	*repl;
    size_t	srclen;
    char_u	*p;
    ਪੂਰਨ		n;
    ਪੂਰਨ		escaped;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    // Skip a regexp pattern for ":vimgrep[add] pat file..."
    p = skip_grep_pat(eap);
#ਨਹੀਂਤਾਂ
    p = eap->arg;
#ਜੇ_ਅੰਤ

    /*
     * Decide to expand wildcards *before* replacing '%', '#', etc.  If
     * the file name contains a wildcard it should not cause expanding.
     * (it will be expanded anyway if there is a wildcard before replacing).
     */
    has_wildcards = mch_has_wildcard(p);
    ਜਦੋਂ (*p != NUL)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// Skip over `=expr`, wildcards in it are not expanded.
	ਜੇ (p[0] == '`' && p[1] == '=')
	{
	    p += 2;
	    (ਖਾਲੀ)skip_ਵਿਅੰਜਕ(&p, ਨਿਰਾ);
	    ਜੇ (*p == '`')
		++p;
	    ਜਾਰੀ;
	}
#ਜੇ_ਅੰਤ
	/*
	 * Quick check if this cannot be the start of a special string.
	 * Also removes backslash before '%', '#' and '<'.
	 */
	ਜੇ (vim_strchr((char_u *)"%#<", *p) == ਨਿਰਾ)
	{
	    ++p;
	    ਜਾਰੀ;
	}

	/*
	 * Try to find a match at this position.
	 */
	repl = eval_vars(p, eap->arg, &srclen, &(eap->do_ecmd_lnum),
						    errormsgp, &escaped, TRUE);
	ਜੇ (*errormsgp != ਨਿਰਾ)		// error detected
	    ਵਾਪਸ FAIL;
	ਜੇ (repl == ਨਿਰਾ)		// no match found
	{
	    p += srclen;
	    ਜਾਰੀ;
	}

	// Wildcards won't be expanded below, the replacement is taken
	// literally.  But do expand "~/file", "~user/file" and "$HOME/file".
	ਜੇ (vim_strchr(repl, '$') != ਨਿਰਾ || vim_strchr(repl, '~') != ਨਿਰਾ)
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
	ਜੇ (!eap->usefilter
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
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UNIX
		&& !(eap->argt & EX_NOSPC)
#ਜੇ_ਅੰਤ
		)
	{
	    char_u	*l;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	    // Don't escape a backslash here, because rem_backslash() doesn't
	    // remove it later.
	    ਸਥਿਰ char_u *nobslash = (char_u *)" \t\"|";
# define ESCAPE_CHARS nobslash
#ਨਹੀਂਤਾਂ
# define ESCAPE_CHARS escape_chars
#ਜੇ_ਅੰਤ

	    ਲਈ (l = repl; *l; ++l)
		ਜੇ (vim_strchr(ESCAPE_CHARS, *l) != ਨਿਰਾ)
		{
		    l = vim_strsave_escaped(repl, ESCAPE_CHARS);
		    ਜੇ (l != ਨਿਰਾ)
		    {
			vim_free(repl);
			repl = l;
		    }
		    ਰੋਕੋ;
		}
	}

	// For a shell command a '!' must be escaped.
	ਜੇ ((eap->usefilter || eap->cmdidx == CMD_bang
						|| eap->cmdidx == CMD_terminal)
			    && vim_strpbrk(repl, (char_u *)"!") != ਨਿਰਾ)
	{
	    char_u	*l;

	    l = vim_strsave_escaped(repl, (char_u *)"!");
	    ਜੇ (l != ਨਿਰਾ)
	    {
		vim_free(repl);
		repl = l;
	    }
	}

	p = repl_cmdline(eap, p, srclen, repl, cmdlinep);
	vim_free(repl);
	ਜੇ (p == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
    }

    /*
     * One file argument: Expand wildcards.
     * Don't do this with ":r !command" or ":w !command".
     */
    ਜੇ ((eap->argt & EX_NOSPC) && !eap->usefilter)
    {
	/*
	 * May do this twice:
	 * 1. Replace environment variables.
	 * 2. Replace any other wildcards, remove backslashes.
	 */
	ਲਈ (n = 1; n <= 2; ++n)
	{
	    ਜੇ (n == 2)
	    {
		/*
		 * Halve the number of backslashes (this is Vi compatible).
		 * For Unix and OS/2, when wildcards are expanded, this is
		 * done by ExpandOne() below.
		 */
#ਜੇ defined(UNIX)
		ਜੇ (!has_wildcards)
#ਜੇ_ਅੰਤ
		    backslash_halve(eap->arg);
	    }

	    ਜੇ (has_wildcards)
	    {
		ਜੇ (n == 1)
		{
		    /*
		     * First loop: May expand environment variables.  This
		     * can be done much faster with expand_env() than with
		     * something else (e.g., calling a shell).
		     * After expanding environment variables, check again
		     * if there are still wildcards present.
		     */
		    ਜੇ (vim_strchr(eap->arg, '$') != ਨਿਰਾ
			    || vim_strchr(eap->arg, '~') != ਨਿਰਾ)
		    {
			expand_env_esc(eap->arg, NameBuff, MAXPATHL,
					(char_u *)(" \t" ਰਸਤਾ_ESC_WILDCARDS),
					TRUE, ਨਿਰਾ);
			has_wildcards = mch_has_wildcard(NameBuff);
			p = NameBuff;
		    }
		    ਨਹੀਂਤਾਂ
			p = ਨਿਰਾ;
		}
		ਨਹੀਂਤਾਂ // n == 2
		{
		    expand_T	xpc;
		    ਪੂਰਨ		options = WILD_LIST_NOTFOUND
					       | WILD_NOERROR | WILD_ਜੋੜੋ_SLASH;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    ਜੇ (p_wic)
			options += WILD_ICASE;
		    p = ExpandOne(&xpc, eap->arg, ਨਿਰਾ,
						   options, WILD_EXPAND_FREE);
		    ਜੇ (p == ਨਿਰਾ)
			ਵਾਪਸ FAIL;
		}
		ਜੇ (p != ਨਿਰਾ)
		{
		    (ਖਾਲੀ)repl_cmdline(eap, eap->arg, STRLEN(eap->arg),
								 p, cmdlinep);
		    ਜੇ (n == 2)	// p came from ExpandOne()
			vim_free(p);
		}
	    }
	}
    }
    ਵਾਪਸ OK;
}

/*
 * Replace part of the command line, keeping eap->cmd, eap->arg and
 * eap->nextcmd correct.
 * "src" points to the part that is to be replaced, of length "srclen".
 * "repl" is the replacement string.
 * Returns a pointer to the character after the replaced string.
 * Returns NULL for failure.
 */
    ਸਥਿਰ char_u *
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
    ਜੇ (eap->nextcmd != ਨਿਰਾ)
	i += STRLEN(eap->nextcmd);	// add space for next command
    ਜੇ ((new_cmdline = alloc(i)) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;			// out of memory!

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

    ਜੇ (eap->nextcmd != ਨਿਰਾ)		// append next command
    {
	new_cmdlinelen += taillen + 1;
	STRCPY(new_cmdline + new_cmdlinelen, eap->nextcmd);
	eap->nextcmd = new_cmdline + new_cmdlinelen;
    }
    eap->cmd = new_cmdline + (eap->cmd - *cmdlinep);
    eap->arg = new_cmdline + (eap->arg - *cmdlinep);
    ਜੇ (eap->do_ecmd_cmd != ਨਿਰਾ && eap->do_ecmd_cmd != dollar_command)
	eap->do_ecmd_cmd = new_cmdline + (eap->do_ecmd_cmd - *cmdlinep);
    vim_free(*cmdlinep);
    *cmdlinep = new_cmdline;

    ਵਾਪਸ src;
}

/*
 * Check for '|' to separate commands and '"' to start comments.
 * If "keep_backslash" is TRUE do not remove any backslash.
 */
    ਖਾਲੀ
separate_nextcmd(exarg_T *eap, ਪੂਰਨ keep_backslash)
{
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();
    char_u	*p;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    p = skip_grep_pat(eap);
#ਨਹੀਂਤਾਂ
    p = eap->arg;
#ਜੇ_ਅੰਤ

    ਲਈ ( ; *p; MB_PTR_ADV(p))
    {
	ਜੇ (*p == Ctrl_V)
	{
	    ਜੇ ((eap->argt & (EX_CTRLV | EX_XFILE)) || keep_backslash)
		++p;		// skip CTRL-V and next char
	    ਨਹੀਂਤਾਂ
				// remove CTRL-V and skip next char
		STRMOVE(p, p + 1);
	    ਜੇ (*p == NUL)		// stop at NUL after CTRL-V
		ਰੋਕੋ;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// Skip over `=expr` when wildcards are expanded.
	ਨਹੀਂਤਾਂ ਜੇ (p[0] == '`' && p[1] == '=' && (eap->argt & EX_XFILE))
	{
	    p += 2;
	    (ਖਾਲੀ)skip_ਵਿਅੰਜਕ(&p, ਨਿਰਾ);
	    ਜੇ (*p == NUL)		// stop at NUL after CTRL-V
		ਰੋਕੋ;
	}
#ਜੇ_ਅੰਤ

	// Check for '"'/'#': start of comment or '|': next command
	// :@" and :*" do not start a comment!
	// :redir @" doesn't either.
	ਨਹੀਂਤਾਂ ਜੇ ((*p == '"'
		    && !vim9script
		    && !(eap->argt & EX_NOTRLCOM)
		    && ((eap->cmdidx != CMD_at && eap->cmdidx != CMD_ਤਾਰਾ)
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
		    && eap->cmdidx != CMD_ਪਾਓ)
		|| *p == '\n')
	{
	    /*
	     * We remove the '\' before the '|', unless EX_CTRLV is used
	     * AND 'b' is present in 'cpoptions'.
	     */
	    ਜੇ ((vim_strchr(p_cpo, CPO_BAR) == ਨਿਰਾ
			      || !(eap->argt & EX_CTRLV)) && *(p - 1) == '\\')
	    {
		ਜੇ (!keep_backslash)
		{
		    STRMOVE(p - 1, p);	// remove the '\'
		    --p;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਸੈੱਟ_nextcmd(eap, p);
		*p = NUL;
		ਰੋਕੋ;
	    }
	}
    }

    ਜੇ (!(eap->argt & EX_NOTRLCOM))	// remove trailing spaces
	del_trailing_spaces(eap->arg);
}

/*
 * get + command from ex argument
 */
    ਸਥਿਰ char_u *
getargcmd(char_u **argp)
{
    char_u *arg = *argp;
    char_u *command = ਨਿਰਾ;

    ਜੇ (*arg == '+')	    // +[command]
    {
	++arg;
	ਜੇ (vim_isspace(*arg) || *arg == NUL)
	    command = dollar_command;
	ਨਹੀਂਤਾਂ
	{
	    command = arg;
	    arg = skip_cmd_arg(command, TRUE);
	    ਜੇ (*arg != NUL)
		*arg++ = NUL;		// terminate command with NUL
	}

	arg = skipwhite(arg);	// skip over spaces
	*argp = arg;
    }
    ਵਾਪਸ command;
}

/*
 * Find end of "+command" argument.  Skip over "\ " and "\\".
 */
    char_u *
skip_cmd_arg(
    char_u *p,
    ਪੂਰਨ	   rembs)	// TRUE to halve the number of backslashes
{
    ਜਦੋਂ (*p && !vim_isspace(*p))
    {
	ਜੇ (*p == '\\' && p[1] != NUL)
	{
	    ਜੇ (rembs)
		STRMOVE(p, p + 1);
	    ਨਹੀਂਤਾਂ
		++p;
	}
	MB_PTR_ADV(p);
    }
    ਵਾਪਸ p;
}

    ਪੂਰਨ
get_bad_opt(char_u *p, exarg_T *eap)
{
    ਜੇ (STRICMP(p, "keep") == 0)
	eap->bad_char = BAD_KEEP;
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(p, "drop") == 0)
	eap->bad_char = BAD_DROP;
    ਨਹੀਂਤਾਂ ਜੇ (MB_BYTE2LEN(*p) == 1 && p[1] == NUL)
	eap->bad_char = *p;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of bad= names.
 */
    ਸਥਿਰ char_u *
get_bad_name(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    // Note: Keep this in sync with getargopt.
    ਸਥਿਰ ਅੱਖਰ *(p_bad_ਮੁੱਲਾਂ[]) =
    {
	"?",
	"keep",
	"drop",
    };

    ਜੇ (idx < 0 || idx >= (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(p_bad_ਮੁੱਲਾਂ))
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ (char_u*)p_bad_ਮੁੱਲਾਂ[idx];
}

/*
 * Get "++opt=arg" argument.
 * Return FAIL or OK.
 */
    ਸਥਿਰ ਪੂਰਨ
getargopt(exarg_T *eap)
{
    char_u	*arg = eap->arg + 2;
    ਪੂਰਨ		*pp = ਨਿਰਾ;
    ਪੂਰਨ		bad_char_idx;
    char_u	*p;

    // Note: Keep this in sync with get_argopt_name.

    // ":edit ++[no]bin[ary] file"
    ਜੇ (STRNCMP(arg, "bin", 3) == 0 || STRNCMP(arg, "nobin", 5) == 0)
    {
	ਜੇ (*arg == 'n')
	{
	    arg += 2;
	    eap->force_bin = FORCE_NOBIN;
	}
	ਨਹੀਂਤਾਂ
	    eap->force_bin = FORCE_BIN;
	ਜੇ (!checkforcmd(&arg, "binary", 3))
	    ਵਾਪਸ FAIL;
	eap->arg = skipwhite(arg);
	ਵਾਪਸ OK;
    }

    // ":read ++edit file"
    ਜੇ (STRNCMP(arg, "edit", 4) == 0)
    {
	eap->read_edit = TRUE;
	eap->arg = skipwhite(arg + 4);
	ਵਾਪਸ OK;
    }

    ਜੇ (STRNCMP(arg, "ff", 2) == 0)
    {
	arg += 2;
	pp = &eap->force_ff;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "fileformat", 10) == 0)
    {
	arg += 10;
	pp = &eap->force_ff;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "enc", 3) == 0)
    {
	ਜੇ (STRNCMP(arg, "encoding", 8) == 0)
	    arg += 8;
	ਨਹੀਂਤਾਂ
	    arg += 3;
	pp = &eap->force_enc;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "bad", 3) == 0)
    {
	arg += 3;
	pp = &bad_char_idx;
    }

    ਜੇ (pp == ਨਿਰਾ || *arg != '=')
	ਵਾਪਸ FAIL;

    ++arg;
    *pp = (ਪੂਰਨ)(arg - eap->cmd);
    arg = skip_cmd_arg(arg, FALSE);
    eap->arg = skipwhite(arg);
    *arg = NUL;

    ਜੇ (pp == &eap->force_ff)
    {
	ਜੇ (check_ff_ਮੁੱਲ(eap->cmd + eap->force_ff) == FAIL)
	    ਵਾਪਸ FAIL;
	eap->force_ff = eap->cmd[eap->force_ff];
    }
    ਨਹੀਂਤਾਂ ਜੇ (pp == &eap->force_enc)
    {
	// Make 'fileencoding' lower case.
	ਲਈ (p = eap->cmd + eap->force_enc; *p != NUL; ++p)
	    *p = TOLOWER_ASC(*p);
    }
    ਨਹੀਂਤਾਂ
    {
	// Check ++bad= argument.  Must be a single-byte character, "keep" or
	// "drop".
	ਜੇ (get_bad_opt(eap->cmd + bad_char_idx, eap) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of ++opt names.
 */
    ਸਥਿਰ char_u *
get_argopt_name(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    // Note: Keep this in sync with getargopt.
    ਸਥਿਰ ਅੱਖਰ *(p_opt_ਮੁੱਲਾਂ[]) =
    {
	"fileformat=",
	"encoding=",
	"binary",
	"nobinary",
	"bad=",
	"edit",
    };

    ਜੇ (idx < 0 || idx >= (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(p_opt_ਮੁੱਲਾਂ))
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ (char_u*)p_opt_ਮੁੱਲਾਂ[idx];
}

/*
 * Command-line expansion for ++opt=name.
 */
    ਪੂਰਨ
expand_argopt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	ਪੂਰਨ	    *ਅੰਕ_Matches)
{
    ਜੇ (xp->xp_ਨਮੂਨਾ > xp->xp_ਸਤਰ && *(xp->xp_ਨਮੂਨਾ-1) == '=')
    {
	char_u *(*cb)(expand_T *, ਪੂਰਨ) = ਨਿਰਾ;

	char_u *name_ਸਿਰਾ = xp->xp_ਨਮੂਨਾ - 1;
	ਜੇ (name_ਸਿਰਾ - xp->xp_ਸਤਰ >= 2
		&& STRNCMP(name_ਸਿਰਾ - 2, "ff", 2) == 0)
	    cb = get_fileformat_name;
	ਨਹੀਂਤਾਂ ਜੇ (name_ਸਿਰਾ - xp->xp_ਸਤਰ >= 10
		&& STRNCMP(name_ਸਿਰਾ - 10, "fileformat", 10) == 0)
	    cb = get_fileformat_name;
	ਨਹੀਂਤਾਂ ਜੇ (name_ਸਿਰਾ - xp->xp_ਸਤਰ >= 3
		&& STRNCMP(name_ਸਿਰਾ - 3, "enc", 3) == 0)
	    cb = get_encoding_name;
	ਨਹੀਂਤਾਂ ਜੇ (name_ਸਿਰਾ - xp->xp_ਸਤਰ >= 8
		&& STRNCMP(name_ਸਿਰਾ - 8, "encoding", 8) == 0)
	    cb = get_encoding_name;
	ਨਹੀਂਤਾਂ ਜੇ (name_ਸਿਰਾ - xp->xp_ਸਤਰ >= 3
		&& STRNCMP(name_ਸਿਰਾ - 3, "bad", 3) == 0)
	    cb = get_bad_name;

	ਜੇ (cb != ਨਿਰਾ)
	{
	    ਵਾਪਸ ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    ਅੰਕ_Matches,
		    cb,
		    FALSE);
	}
	ਵਾਪਸ FAIL;
    }

    // Special handling of "ff" which acts as a short form of
    // "fileformat", as "ff" is not a substring of it.
    ਜੇ (xp->xp_ਨਮੂਨਾ_len == 2
	    && STRNCMP(xp->xp_ਨਮੂਨਾ, "ff", xp->xp_ਨਮੂਨਾ_len) == 0)
    {
	*matches = ALLOC_MULT(char_u *, 1);
	ਜੇ (*matches == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	*ਅੰਕ_Matches = 1;
	(*matches)[0] = vim_strsave((char_u*)"fileformat=");
	ਵਾਪਸ OK;
    }

    ਵਾਪਸ ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    ਅੰਕ_Matches,
	    get_argopt_name,
	    FALSE);
}

    ਸਥਿਰ ਖਾਲੀ
ex_autocmd(exarg_T *eap)
{
    ਜੇ (eap->skip)
    {
	skip_cmd_ਖੰਡ(eap);
	ਵਾਪਸ;
    }

    /*
     * Disallow autocommands from .exrc and .vimrc in current
     * directory for security reasons.
     */
    ਜੇ (secure)
    {
	secure = 2;
	eap->errmsg =
	      _(e_command_not_allowed_from_vimrc_ਵਿੱਚ_current_dir_or_ਟੈਗ_ਖੋਜ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_autocmd)
	do_autocmd(eap, eap->arg, eap->forceit);
    ਨਹੀਂਤਾਂ
	do_augroup(eap->arg, eap->forceit);
}

/*
 * ":doautocmd": Apply the automatic commands to the current buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
ex_doautocmd(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ਪੂਰਨ		call_do_modelines = check_nomodeline(&arg);
    ਪੂਰਨ		did_aucmd;

    (ਖਾਲੀ)do_doautocmd(arg, TRUE, &did_aucmd);
    // Only when there is no <nomodeline>.
    ਜੇ (call_do_modelines && did_aucmd)
	do_modelines(0);
}

/*
 * :[N]bunload[!] [N] [bufname] unload buffer
 * :[N]bdelete[!] [N] [bufname] delete buffer from buffer list
 * :[N]bwipeout[!] [N] [bufname] delete buffer really
 */
    ਸਥਿਰ ਖਾਲੀ
ex_bunload(exarg_T *eap)
{
    ਜੇ (ERROR_IF_ANY_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;
    eap->errmsg = do_bufdel(
	    eap->cmdidx == CMD_bdelete ? DOBUF_DEL
		: eap->cmdidx == CMD_bwipeout ? DOBUF_WIPE
		: DOBUF_UNLOAD, eap->arg,
	    eap->addr_ਗਿਣਤੀ, (ਪੂਰਨ)eap->line1, (ਪੂਰਨ)eap->line2, eap->forceit);
}

/*
 * :[N]buffer [N]	to buffer N
 * :[N]sbuffer [N]	to buffer N
 */
    ਸਥਿਰ ਖਾਲੀ
ex_ਬਫਰ(exarg_T *eap)
{
    ਜੇ (ERROR_IF_ANY_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;
    do_exbuffer(eap);
}

/*
 * ":buffer" command and alike.
 */
    ਸਥਿਰ ਖਾਲੀ
do_exbuffer(exarg_T *eap)
{
    ਜੇ (*eap->arg)
	eap->errmsg = ex_errmsg(e_trailing_characters_str, eap->arg);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (eap->addr_ਗਿਣਤੀ == 0)	// default is current buffer
	    goto_ਬਫਰ(eap, DOBUF_CURRENT, FORWARD, 0);
	ਨਹੀਂਤਾਂ
	    goto_ਬਫਰ(eap, DOBUF_ਪਹਿਲਾ, FORWARD, (ਪੂਰਨ)eap->line2);
	ਜੇ (eap->do_ecmd_cmd != ਨਿਰਾ)
	    do_cmd_ਦਲੀਲ(eap->do_ecmd_cmd);
    }
}

/*
 * :[N]bmodified [N]	to next mod. buffer
 * :[N]sbmodified [N]	to next mod. buffer
 */
    ਸਥਿਰ ਖਾਲੀ
ex_bmodified(exarg_T *eap)
{
    goto_ਬਫਰ(eap, DOBUF_MOD, FORWARD, (ਪੂਰਨ)eap->line2);
    ਜੇ (eap->do_ecmd_cmd != ਨਿਰਾ)
	do_cmd_ਦਲੀਲ(eap->do_ecmd_cmd);
}

/*
 * :[N]bnext [N]	to next buffer
 * :[N]sbnext [N]	split and to next buffer
 */
    ਸਥਿਰ ਖਾਲੀ
ex_bnext(exarg_T *eap)
{
    ਜੇ (ERROR_IF_ANY_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;

    goto_ਬਫਰ(eap, DOBUF_CURRENT, FORWARD, (ਪੂਰਨ)eap->line2);
    ਜੇ (eap->do_ecmd_cmd != ਨਿਰਾ)
	do_cmd_ਦਲੀਲ(eap->do_ecmd_cmd);
}

/*
 * :[N]bNext [N]	to previous buffer
 * :[N]bprevious [N]	to previous buffer
 * :[N]sbNext [N]	split and to previous buffer
 * :[N]sbprevious [N]	split and to previous buffer
 */
    ਸਥਿਰ ਖਾਲੀ
ex_bprevious(exarg_T *eap)
{
    ਜੇ (ERROR_IF_ANY_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;

    goto_ਬਫਰ(eap, DOBUF_CURRENT, BACKWARD, (ਪੂਰਨ)eap->line2);
    ਜੇ (eap->do_ecmd_cmd != ਨਿਰਾ)
	do_cmd_ਦਲੀਲ(eap->do_ecmd_cmd);
}

/*
 * :brewind		to first buffer
 * :bfirst		to first buffer
 * :sbrewind		split and to first buffer
 * :sbfirst		split and to first buffer
 */
    ਸਥਿਰ ਖਾਲੀ
ex_brewind(exarg_T *eap)
{
    ਜੇ (ERROR_IF_ANY_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;

    goto_ਬਫਰ(eap, DOBUF_ਪਹਿਲਾ, FORWARD, 0);
    ਜੇ (eap->do_ecmd_cmd != ਨਿਰਾ)
	do_cmd_ਦਲੀਲ(eap->do_ecmd_cmd);
}

/*
 * :blast		to last buffer
 * :sblast		split and to last buffer
 */
    ਸਥਿਰ ਖਾਲੀ
ex_blast(exarg_T *eap)
{
    ਜੇ (ERROR_IF_ANY_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;

    goto_ਬਫਰ(eap, DOBUF_ਆਖਰੀ, BACKWARD, 0);
    ਜੇ (eap->do_ecmd_cmd != ਨਿਰਾ)
	do_cmd_ਦਲੀਲ(eap->do_ecmd_cmd);
}

/*
 * Check if "c" ends an Ex command.
 * In Vim9 script does not check for white space before #.
 */
    ਪੂਰਨ
ਸਿਰੇ_excmd(ਪੂਰਨ c)
{
    ਪੂਰਨ comment_char = '"';

    ਜੇ (ਵਿੱਚ_vim9script())
	comment_char = '#';
    ਵਾਪਸ (c == NUL || c == '|' || c == comment_char || c == '\n');
}

/*
 * Like ends_excmd() but checks that a # in Vim9 script either has "cmd" equal
 * to "cmd_start" or has a white space character before it.
 */
    ਪੂਰਨ
ਸਿਰੇ_excmd2(char_u *cmd_ਸ਼ੁਰੂ ਅਣਵਰਤਿਆ, char_u *cmd)
{
    ਪੂਰਨ c = *cmd;

    ਜੇ (c == NUL || c == '|' || c == '\n')
	ਵਾਪਸ TRUE;
    ਜੇ (ਵਿੱਚ_vim9script())
	//  # starts a comment, #{ might be a mistake, #{{ can start a fold
	ਵਾਪਸ c == '#' && (cmd[1] != '{' || cmd[2] == '{')
				 && (cmd == cmd_ਸ਼ੁਰੂ || VIM_ISWHITE(cmd[-1]));
    ਵਾਪਸ c == '"';
}

#ਜੇ defined(FEAT_SYN_HL) || defined(FEAT_ਖੋਜ_EXTRA) || defined(FEAT_EVAL)
/*
 * Return the next command, after the first '|' or '\n'.
 * Return NULL if not found.
 */
    char_u *
ਲੱਭੋ_nextcmd(char_u *p)
{
    ਜਦੋਂ (*p != '|' && *p != '\n')
    {
	ਜੇ (*p == NUL)
	    ਵਾਪਸ ਨਿਰਾ;
	++p;
    }
    ਵਾਪਸ (p + 1);
}
#ਜੇ_ਅੰਤ

/*
 * Check if *p is a separator between Ex commands, skipping over white space.
 * Return NULL if it isn't, the following character if it is.
 */
    char_u *
check_nextcmd(char_u *p)
{
    char_u *s = skipwhite(p);

    ਜੇ (*s == '|' || *s == '\n')
	ਵਾਪਸ (s + 1);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ ਨਿਰਾ;
}

/*
 * If "eap->nextcmd" is not set, check for a next command at "p".
 */
    ਖਾਲੀ
ਸੈੱਟ_nextcmd(exarg_T *eap, char_u *arg)
{
    char_u *p = skipwhite(arg);

    ਜੇ (ਵਿੱਚ_vim9script() && *p == '#')
    {
	char_u *nl = vim_strchr(p, NL);
	ਜੇ (nl != ਨਿਰਾ)
	    p = nl;
    }

    p = check_nextcmd(p);

    ਜੇ (eap->nextcmd == ਨਿਰਾ)
	eap->nextcmd = p;
    ਨਹੀਂਤਾਂ ਜੇ (p != ਨਿਰਾ)
	// cannot use "| command" inside a  {} block
	semsg(_(e_cannot_use_bar_ਮੰਜ਼ਿਲ_separate_commands_ਇੱਥੇ_str), arg);
}

/*
 * - if there are more files to edit
 * - and this is the last window
 * - and forceit not used
 * - and not repeated twice on a row
 *    return FAIL and give error message if 'message' TRUE
 * return OK otherwise
 */
    ਸਥਿਰ ਪੂਰਨ
check_ਹੋਰ(
    ਪੂਰਨ ਸੁਨੇਹਾ_2,	    // when FALSE check only, no messages
    ਪੂਰਨ forceit)
{
    ਪੂਰਨ	    n = ARGCOUNT - curwin->w_arg_idx - 1;

    ਜੇ (!forceit && only_one_ਖਿੜਕੀ()
	    && ARGCOUNT > 1 && !arg_had_ਆਖਰੀ && n > 0 && quitmore == 0)
    {
	ਜੇ (ਸੁਨੇਹਾ_2)
	{
#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ਜੇ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						    && curbuf->b_fname != ਨਿਰਾ)
	    {
		char_u	buff[DIALOG_ਸੁਨੇਹਾ_ਮਾਪ];

		vim_snprintf((ਅੱਖਰ *)buff, DIALOG_ਸੁਨੇਹਾ_ਮਾਪ,
			NGETTEXT("%d more file to edit.  Quit anyway?",
			    "%d more files to edit.  Quit anyway?", n), n);
		ਜੇ (vim_dialog_yesno(VIM_QUESTION, ਨਿਰਾ, buff, 1) == VIM_YES)
		    ਵਾਪਸ OK;
		ਵਾਪਸ FAIL;
	    }
#ਜੇ_ਅੰਤ
	    semsg(NGETTEXT(e_nr_ਹੋਰ_file_ਮੰਜ਼ਿਲ_edit,
			   e_nr_ਹੋਰ_files_ਮੰਜ਼ਿਲ_edit , n), n);
	    quitmore = 2;	    // next try to quit is allowed
	}
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of command names.
 */
    char_u *
get_command_name(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx >= (ਪੂਰਨ)CMD_ਮਾਪ)
	ਵਾਪਸ expand_user_command_name(idx);
    // the following are no real commands
    ਜੇ (STRNCMP(cmdnames[idx].cmd_name, "{", 1) == 0 ||
	STRNCMP(cmdnames[idx].cmd_name, "}", 1) == 0)
	ਵਾਪਸ (char_u *)"";
    ਵਾਪਸ cmdnames[idx].cmd_name;
}

    ਸਥਿਰ ਖਾਲੀ
ex_colorscheme(exarg_T *eap)
{
    ਜੇ (*eap->arg == NUL)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	char_u *ਵਿਅੰਜਕ = vim_strsave((char_u *)"g:colors_name");
	char_u *p = ਨਿਰਾ;

	ਜੇ (ਵਿਅੰਜਕ != ਨਿਰਾ)
	{
	    ++emsg_off;
	    p = eval_ਮੰਜ਼ਿਲ_string(ਵਿਅੰਜਕ, FALSE, FALSE);
	    --emsg_off;
	    vim_free(ਵਿਅੰਜਕ);
	}
	ਜੇ (p != ਨਿਰਾ)
	{
	    ਸੁਨੇਹਾ((ਅੱਖਰ *)p);
	    vim_free(p);
	}
	ਨਹੀਂਤਾਂ
	    ਸੁਨੇਹਾ("default");
#ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ(_("unknown"));
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਲੱਦੋ_colors(eap->arg) == FAIL)
	semsg(_(e_cannot_ਲੱਭੋ_color_scheme_str), eap->arg);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VTP
    ਨਹੀਂਤਾਂ ਜੇ (has_vtp_working())
    {
	// background color change requires clear + redraw
	update_screen(UPD_CLEAR);
	redrawcmd();
    }
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ ਖਾਲੀ
ex_highlight(exarg_T *eap)
{
    ਜੇ (*eap->arg == NUL && eap->cmd[2] == '!')
	ਸੁਨੇਹਾ(_("Greetings, Vim user!"));
    do_highlight(eap->arg, eap->forceit, FALSE);
}


/*
 * Call this function if we thought we were going to exit, but we won't
 * (because of an error).  May need to restore the terminal mode.
 */
    ਖਾਲੀ
not_exiting(ਪੂਰਨ save_exiting)
{
    exiting = save_exiting;
    settmode(TMODE_RAW);
}

    ਪੂਰਨ
before_quit_autocmds(win_T *wp, ਪੂਰਨ quit_all, ਪੂਰਨ forceit)
{
    apply_autocmds(EVENT_QUITPRE, ਨਿਰਾ, ਨਿਰਾ, FALSE, wp->w_ਬਫਰ);

    // Bail out when autocommands closed the window.
    // Refuse to quit when the buffer in the last window is being closed (can
    // only happen in autocommands).
    ਜੇ (!win_valid(wp)
	    || curbuf_locked()
	    || (wp->w_ਬਫਰ->b_nwindows == 1 && wp->w_ਬਫਰ->b_locked > 0))
	ਵਾਪਸ TRUE;

    ਜੇ (quit_all || (check_ਹੋਰ(FALSE, forceit) == OK && only_one_ਖਿੜਕੀ()))
    {
	apply_autocmds(EVENT_EXITPRE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	// Refuse to quit when locked or when the window was closed or the
	// buffer in the last window is being closed (can only happen in
	// autocommands).
	ਜੇ (!win_valid(wp) || curbuf_locked()
			  || (curbuf->b_nwindows == 1 && curbuf->b_locked > 0))
	    ਵਾਪਸ TRUE;
    }

    ਵਾਪਸ FALSE;
}

/*
 * ":quit": quit current window, quit Vim if the last window is closed.
 * ":{nr}quit": quit window {nr}
 * Also used when closing a terminal window that's the last one.
 */
    ਖਾਲੀ
ex_quit(exarg_T *eap)
{
    win_T	*wp;

    ਜੇ (cmdwin_type != 0)
    {
	cmdwin_ਨਤੀਜਾ = Ctrl_C;
	ਵਾਪਸ;
    }
    // Don't quit while editing the command line.
    ਜੇ (ਪਾਠ_locked())
    {
	ਪਾਠ_locked_ਸੁਨੇਹਾ();
	ਵਾਪਸ;
    }
    ਜੇ (eap->addr_ਗਿਣਤੀ > 0)
    {
	ਪੂਰਨ	wnr = eap->line2;

	ਲਈ (wp = firstwin; wp->w_ਅਗਲਾ != ਨਿਰਾ; wp = wp->w_ਅਗਲਾ)
	    ਜੇ (--wnr <= 0)
		ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ
	wp = curwin;

    // Refuse to quit when locked.
    ਜੇ (curbuf_locked())
	ਵਾਪਸ;

    // Trigger QuitPre and maybe ExitPre
    ਜੇ (before_quit_autocmds(wp, FALSE, eap->forceit))
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    netbeansForcedQuit = eap->forceit;
#ਜੇ_ਅੰਤ

    ਪੂਰਨ save_exiting = exiting;
    /*
     * If there is only one relevant window we will exit.
     */
    ਜੇ (check_ਹੋਰ(FALSE, eap->forceit) == OK && only_one_ਖਿੜਕੀ())
	exiting = TRUE;
    ਜੇ ((!buf_hide(wp->w_ਬਫਰ)
		&& check_ਬਦਲਿਆ(wp->w_ਬਫਰ, (p_awa ? CCGD_AW : 0)
				       | (eap->forceit ? CCGD_FORCEIT : 0)
				       | CCGD_EXCMD))
	    || check_ਹੋਰ(TRUE, eap->forceit) == FAIL
	    || (only_one_ਖਿੜਕੀ() && check_ਬਦਲਿਆ_any(eap->forceit, TRUE)))
    {
	not_exiting(save_exiting);
    }
    ਨਹੀਂਤਾਂ
    {
	// quit last window
	// Note: only_one_window() returns true, even so a help window is
	// still open. In that case only quit, if no address has been
	// specified. Example:
	// :h|wincmd w|1q     - don't quit
	// :h|wincmd w|q      - quit
	ਜੇ (only_one_ਖਿੜਕੀ() && (ONE_ਖਿੜਕੀ || eap->addr_ਗਿਣਤੀ == 0))
	    getout(0);
	not_exiting(save_exiting);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
	// close window; may free buffer
	win_close(wp, !buf_hide(wp->w_ਬਫਰ) || eap->forceit);
    }
}

/*
 * ":cquit".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_cquit(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    // this does not always pass on the exit code to the Manx compiler. why?
    getout(eap->addr_ਗਿਣਤੀ > 0 ? (ਪੂਰਨ)eap->line2 : EXIT_FAILURE);
}

/*
 * Do preparations for "qall" and "wqall".
 * Returns FAIL when quitting should be aborted.
 */
    ਪੂਰਨ
before_quit_all(exarg_T *eap)
{
    ਜੇ (cmdwin_type != 0)
    {
	ਜੇ (eap->forceit)
	    cmdwin_ਨਤੀਜਾ = K_XF1;	// ex_window() takes care of this
	ਨਹੀਂਤਾਂ
	    cmdwin_ਨਤੀਜਾ = K_XF2;
	ਵਾਪਸ FAIL;
    }

    // Don't quit while editing the command line.
    ਜੇ (ਪਾਠ_locked())
    {
	ਪਾਠ_locked_ਸੁਨੇਹਾ();
	ਵਾਪਸ FAIL;
    }

    ਜੇ (before_quit_autocmds(curwin, TRUE, eap->forceit))
	ਵਾਪਸ FAIL;

    ਵਾਪਸ OK;
}

/*
 * ":qall": try to quit all windows
 */
    ਸਥਿਰ ਖਾਲੀ
ex_quit_all(exarg_T *eap)
{
    ਜੇ (before_quit_all(eap) == FAIL)
	ਵਾਪਸ;
    ਪੂਰਨ save_exiting = exiting;
    exiting = TRUE;
    ਜੇ (eap->forceit || !check_ਬਦਲਿਆ_any(FALSE, FALSE))
	getout(0);
    not_exiting(save_exiting);
}

/*
 * ":close": close current window, unless it is the last one
 */
    ਸਥਿਰ ਖਾਲੀ
ex_close(exarg_T *eap)
{
    win_T	*win;
    ਪੂਰਨ		winnr = 0;
    ਜੇ (cmdwin_type != 0)
	cmdwin_ਨਤੀਜਾ = Ctrl_C;
    ਨਹੀਂਤਾਂ
	ਜੇ (!ਪਾਠ_locked() && !curbuf_locked())
	{
	    ਜੇ (eap->addr_ਗਿਣਤੀ == 0)
		ex_win_close(eap->forceit, curwin, ਨਿਰਾ);
	    ਨਹੀਂਤਾਂ
	    {
		FOR_ALL_WINDOWS(win)
		{
		    winnr++;
		    ਜੇ (winnr == eap->line2)
			ਰੋਕੋ;
		}
		ਜੇ (win == ਨਿਰਾ)
		    win = lastwin;
		ex_win_close(eap->forceit, win, ਨਿਰਾ);
	    }
	}
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
/*
 * ":pclose": Close any preview window.
 */
    ਸਥਿਰ ਖਾਲੀ
ex_pclose(exarg_T *eap)
{
    win_T	*win;

    // First close any normal window.
    FOR_ALL_WINDOWS(win)
	ਜੇ (win->w_p_pvw)
	{
	    ex_win_close(eap->forceit, win, ਨਿਰਾ);
	    ਵਾਪਸ;
	}
# ifdef FEAT_PROP_POPUP
    // Also when 'previewpopup' is empty, it might have been cleared.
    popup_close_preview();
# endif
}
#ਜੇ_ਅੰਤ

/*
 * Close window "win" and take care of handling closing the last window for a
 * modified buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
ex_win_close(
    ਪੂਰਨ		forceit,
    win_T	*win,
    tabpage_T	*tp)		// NULL or the tab page "win" is in
{
    ਪੂਰਨ		need_hide;
    buf_T	*buf = win->w_ਬਫਰ;

    // Never close the autocommand window.
    ਜੇ (is_aucmd_win(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_ਖਿੜਕੀ));
	ਵਾਪਸ;
    }
    ਜੇ (ਖਿੜਕੀ_layout_locked(CMD_close))
	ਵਾਪਸ;

    need_hide = (buf_Is_ਬਦਲਿਆ(buf) && buf->b_nwindows <= 1);
    ਜੇ (need_hide && !buf_hide(buf) && !forceit)
    {
#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ਜੇ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    ਜੇ (ਪਦ_ਕੰਮ_running(buf->b_ਪਦ))
	    {
		ਜੇ (ਪਦ_confirm_stop(buf) == FAIL)
		    ਵਾਪਸ;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(buf);
		need_hide = FALSE;
	    }
	    ਨਹੀਂਤਾਂ
# endif
	    {
		bufref_T bufref;

		ਸੈੱਟ_bufref(&bufref, buf);
		dialog_ਬਦਲਿਆ(buf, FALSE);
		ਜੇ (bufref_valid(&bufref) && buf_Is_ਬਦਲਿਆ(buf))
		    ਵਾਪਸ;
		need_hide = FALSE;
	    }
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	    no_write_ਸੁਨੇਹਾ();
	    ਵਾਪਸ;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ

    // free buffer when not hiding it or when it's a scratch buffer
    ਜੇ (tp == ਨਿਰਾ)
	win_close(win, !need_hide && !buf_hide(buf));
    ਨਹੀਂਤਾਂ
	win_close_othertab(win, !need_hide && !buf_hide(buf), tp);
}

/*
 * Handle the argument for a tabpage related ex command.
 * Returns a tabpage number.
 * When an error is encountered then eap->errmsg is set.
 */
    ਸਥਿਰ ਪੂਰਨ
get_tabpage_arg(exarg_T *eap)
{
    ਪੂਰਨ tab_ਸੰਖਿਆ;
    ਪੂਰਨ unaccept_arg0 = (eap->cmdidx == CMD_tabmove) ? 0 : 1;

    ਜੇ (eap->arg && *eap->arg != NUL)
    {
	char_u *p = eap->arg;
	char_u *p_save;
	ਪੂਰਨ    relative = 0; // argument +N/-N means: go to N places to the
			     // right/left relative to the current position.

	ਜੇ (*p == '-')
	{
	    relative = -1;
	    p++;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == '+')
	{
	    relative = 1;
	    p++;
	}

	p_save = p;
	tab_ਸੰਖਿਆ = getdigits(&p);

	ਜੇ (relative == 0)
	{
	    ਜੇ (STRCMP(p, "$") == 0)
		tab_ਸੰਖਿਆ = ਆਖਰੀ_TAB_NR;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(p, "#") == 0)
		ਜੇ (valid_tabpage(lastused_tabpage))
		    tab_ਸੰਖਿਆ = tabpage_ਸੂਚਕਾਂਕ(lastused_tabpage);
		ਨਹੀਂਤਾਂ
		{
		    eap->errmsg = ex_errmsg(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str,
								     eap->arg);
		    tab_ਸੰਖਿਆ = 0;
		    ਜਾਓ theend;
		}
	    ਨਹੀਂਤਾਂ ਜੇ (p == p_save || *p_save == '-' || *p != NUL
		    || tab_ਸੰਖਿਆ > ਆਖਰੀ_TAB_NR)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_ਦਲੀਲ_str, eap->arg);
		ਜਾਓ theend;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (*p_save == NUL)
		tab_ਸੰਖਿਆ = 1;
	    ਨਹੀਂਤਾਂ ਜੇ (p == p_save || *p_save == '-' || *p != NUL
		    || tab_ਸੰਖਿਆ == 0)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_ਦਲੀਲ_str, eap->arg);
		ਜਾਓ theend;
	    }
	    tab_ਸੰਖਿਆ = tab_ਸੰਖਿਆ * relative + tabpage_ਸੂਚਕਾਂਕ(curtab);
	    ਜੇ (!unaccept_arg0 && relative == -1)
		--tab_ਸੰਖਿਆ;
	}
	ਜੇ (tab_ਸੰਖਿਆ < unaccept_arg0 || tab_ਸੰਖਿਆ > ਆਖਰੀ_TAB_NR)
	    eap->errmsg = ex_errmsg(e_invalid_ਦਲੀਲ_str, eap->arg);
    }
    ਨਹੀਂਤਾਂ ਜੇ (eap->addr_ਗਿਣਤੀ > 0)
    {
	ਜੇ (unaccept_arg0 && eap->line2 == 0)
	{
	    eap->errmsg = _(e_invalid_range);
	    tab_ਸੰਖਿਆ = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    tab_ਸੰਖਿਆ = eap->line2;
	    ਜੇ (!unaccept_arg0)
	    {
		char_u *cmdp = eap->cmd;

		ਜਦੋਂ (--cmdp > *eap->cmdlinep
			&& (VIM_ISWHITE(*cmdp) || VIM_ISDIGIT(*cmdp)))
		    ;
		ਜੇ (*cmdp == '-')
		{
		    --tab_ਸੰਖਿਆ;
		    ਜੇ (tab_ਸੰਖਿਆ < unaccept_arg0)
			eap->errmsg = _(e_invalid_range);
		}
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਚੋਣ (eap->cmdidx)
	{
	ਹਾਲਤ CMD_tabnext:
	    tab_ਸੰਖਿਆ = tabpage_ਸੂਚਕਾਂਕ(curtab) + 1;
	    ਜੇ (tab_ਸੰਖਿਆ > ਆਖਰੀ_TAB_NR)
		tab_ਸੰਖਿਆ = 1;
	    ਰੋਕੋ;
	ਹਾਲਤ CMD_tabmove:
	    tab_ਸੰਖਿਆ = ਆਖਰੀ_TAB_NR;
	    ਰੋਕੋ;
	ਮੂਲ:
	    tab_ਸੰਖਿਆ = tabpage_ਸੂਚਕਾਂਕ(curtab);
	}
    }

theend:
    ਵਾਪਸ tab_ਸੰਖਿਆ;
}

/*
 * ":tabclose": close current tab page, unless it is the last one.
 * ":tabclose N": close tab page N.
 */
    ਸਥਿਰ ਖਾਲੀ
ex_tabclose(exarg_T *eap)
{
    tabpage_T	*tp;
    ਪੂਰਨ		tab_ਸੰਖਿਆ;

    ਜੇ (cmdwin_type != 0)
    {
	cmdwin_ਨਤੀਜਾ = K_IGNORE;
	ਵਾਪਸ;
    }

    ਜੇ (ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ == ਨਿਰਾ)
    {
	emsg(_(e_cannot_close_ਆਖਰੀ_tab_ਪੰਨਾ));
	ਵਾਪਸ;
    }

    ਜੇ (ਖਿੜਕੀ_layout_locked(CMD_tabclose))
	ਵਾਪਸ;

    tab_ਸੰਖਿਆ = get_tabpage_arg(eap);
    ਜੇ (eap->errmsg != ਨਿਰਾ)
	ਵਾਪਸ;

    tp = ਲੱਭੋ_tabpage(tab_ਸੰਖਿਆ);
    ਜੇ (tp == ਨਿਰਾ)
    {
	beep_flush();
	ਵਾਪਸ;
    }
    ਜੇ (tp != curtab)
    {
	tabpage_close_other(tp, eap->forceit);
	ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (!ਪਾਠ_locked() && !curbuf_locked())
	tabpage_close(eap->forceit);
}

/*
 * ":tabonly": close all tab pages except the current one
 */
    ਸਥਿਰ ਖਾਲੀ
ex_tabonly(exarg_T *eap)
{
    tabpage_T	*tp;
    ਪੂਰਨ		ਪੂਰਾ;
    ਪੂਰਨ		tab_ਸੰਖਿਆ;

    ਜੇ (cmdwin_type != 0)
    {
	cmdwin_ਨਤੀਜਾ = K_IGNORE;
	ਵਾਪਸ;
    }

    ਜੇ (ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ == ਨਿਰਾ)
    {
	ਸੁਨੇਹਾ(_("Already only one tab page"));
	ਵਾਪਸ;
    }

    ਜੇ (ਖਿੜਕੀ_layout_locked(CMD_tabonly))
	ਵਾਪਸ;

    tab_ਸੰਖਿਆ = get_tabpage_arg(eap);
    ਜੇ (eap->errmsg != ਨਿਰਾ)
	ਵਾਪਸ;

    goto_tabpage(tab_ਸੰਖਿਆ);
    // Repeat this up to a 1000 times, because autocommands may
    // mess up the lists.
    ਲਈ (ਪੂਰਾ = 0; ਪੂਰਾ < 1000; ++ਪੂਰਾ)
    {
	FOR_ALL_TABPAGES(tp)
	    ਜੇ (tp->tp_topframe != topframe)
	    {
		tabpage_close_other(tp, eap->forceit);
		// if we failed to close it quit
		ਜੇ (valid_tabpage(tp))
		    ਪੂਰਾ = 1000;
		// start over, "tp" is now invalid
		ਰੋਕੋ;
	    }
	ਜੇ (ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ == ਨਿਰਾ)
	    ਰੋਕੋ;
    }
}

/*
 * Close the current tab page.
 */
    ਖਾਲੀ
tabpage_close(ਪੂਰਨ forceit)
{
    ਜੇ (ਖਿੜਕੀ_layout_locked(CMD_tabclose))
	ਵਾਪਸ;

    trigger_tabclosedpre(curtab);
    curtab->tp_did_tabclosedpre = TRUE;
    tabpage_T *save_curtab = curtab;

    // First close all the windows but the current one.  If that worked then
    // close the last window in this tab, that will close it.
    ਜੇ (!ONE_ਖਿੜਕੀ)
	close_others(TRUE, forceit);
    ਜੇ (ONE_ਖਿੜਕੀ)
	ex_win_close(forceit, curwin, ਨਿਰਾ);
    ਜੇ (curtab == save_curtab)
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	curtab->tp_did_tabclosedpre = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
}

/*
 * Close tab page "tp", which is not the current tab page.
 * Note that autocommands may make "tp" invalid.
 * Also takes care of the tab pages line disappearing when closing the
 * last-but-one tab page.
 */
    ਖਾਲੀ
tabpage_close_other(tabpage_T *tp, ਪੂਰਨ forceit)
{
    ਪੂਰਨ		ਪੂਰਾ = 0;
    win_T	*wp;

    ਜੇ (ਖਿੜਕੀ_layout_locked(CMD_ਮਾਪ))
	ਵਾਪਸ;

    trigger_tabclosedpre(tp);
    tp->tp_did_tabclosedpre = TRUE;

    // Limit to 1000 windows, autocommands may add a window while we close
    // one.  OK, so I'm paranoid...
    ਜਦੋਂ (++ਪੂਰਾ < 1000)
    {
	wp = tp->tp_firstwin;
	ex_win_close(forceit, wp, tp);

	// Autocommands may delete the tab page under our fingers.
	ਜੇ (!valid_tabpage(tp))
	    ਰੋਕੋ;
	// We may fail to close a window with a modified buffer.
	ਜੇ (tp->tp_firstwin == wp)
	{
	    ਪੂਰਾ = 1000;
	    ਰੋਕੋ;
	}
    }
    ਜੇ (ਪੂਰਾ >= 1000)
    {
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	tp->tp_did_tabclosedpre = FALSE;
	ਵਾਪਸ;
    }

    apply_autocmds(EVENT_TABCLOSED, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
}

/*
 * ":only".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_only(exarg_T *eap)
{
    ਜੇ (ਖਿੜਕੀ_layout_locked(CMD_only))
	ਵਾਪਸ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
    ਜੇ (eap->addr_ਗਿਣਤੀ > 0)
    {
	win_T   *wp;
	ਪੂਰਨ	wnr = eap->line2;
	ਲਈ (wp = firstwin; --wnr > 0; )
	{
	    ਜੇ (wp->w_ਅਗਲਾ == ਨਿਰਾ)
		ਰੋਕੋ;
	    ਨਹੀਂਤਾਂ
		wp = wp->w_ਅਗਲਾ;
	}
	win_goto(wp);
    }
    close_others(TRUE, eap->forceit);
}

    ਸਥਿਰ ਖਾਲੀ
ex_hide(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    // ":hide" or ":hide | cmd": hide current window
    ਜੇ (eap->skip)
	ਵਾਪਸ;

    ਜੇ (ਖਿੜਕੀ_layout_locked(CMD_hide))
	ਵਾਪਸ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
    ਜੇ (eap->addr_ਗਿਣਤੀ == 0)
	win_close(curwin, FALSE);	// don't free buffer
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	winnr = 0;
	win_T	*win;

	FOR_ALL_WINDOWS(win)
	{
	    winnr++;
	    ਜੇ (winnr == eap->line2)
		ਰੋਕੋ;
	}
	ਜੇ (win == ਨਿਰਾ)
	    win = lastwin;
	win_close(win, FALSE);
    }
}

/*
 * ":stop" and ":suspend": Suspend Vim.
 */
    ਖਾਲੀ
ex_stop(exarg_T *eap)
{
    /*
     * Disallow suspending for "rvim".
     */
    ਜੇ (check_restricted())
	ਵਾਪਸ;

    ਜੇ (!eap->forceit)
	autowrite_all();
    apply_autocmds(EVENT_VIMSUSPEND, ਨਿਰਾ, ਨਿਰਾ, FALSE, ਨਿਰਾ);
    windgoto((ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1, cmdline_ਖਾਨਾ_off);
    ਬਾਹਰਲਾ_char('\n');
    ਬਾਹਰਲਾ_flush();
    stoptermcap();
    ਬਾਹਰਲਾ_flush();		// needed for SUN to restore xterm buffer
    mch_restore_title(SAVE_RESTORE_BOTH);	// restore window titles
    ui_suspend();		// call machine specific function
    maketitle();
    resettitle();		// force updating the title
    starttermcap();
    scroll_ਸ਼ੁਰੂ();		// scroll screen before redrawing
    redraw_later_clear();
    shell_resized();	// may have resized window
    apply_autocmds(EVENT_VIMRESUME, ਨਿਰਾ, ਨਿਰਾ, FALSE, ਨਿਰਾ);
}

/*
 * ":exit", ":xit" and ":wq": Write file and quit the current window.
 */
    ਸਥਿਰ ਖਾਲੀ
ex_exit(exarg_T *eap)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (not_ਵਿੱਚ_vim9(eap, false) == FAIL)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    ਜੇ (cmdwin_type != 0)
    {
	cmdwin_ਨਤੀਜਾ = Ctrl_C;
	ਵਾਪਸ;
    }
    // Don't quit while editing the command line.
    ਜੇ (ਪਾਠ_locked())
    {
	ਪਾਠ_locked_ਸੁਨੇਹਾ();
	ਵਾਪਸ;
    }

    ਪੂਰਨ save_exiting = exiting;
    /*
     * we plan to exit if there is only one relevant window
     */
    ਜੇ (check_ਹੋਰ(FALSE, eap->forceit) == OK && only_one_ਖਿੜਕੀ())
	exiting = TRUE;

    // Write the buffer for ":wq" or when it was changed.
    // Trigger QuitPre and ExitPre.
    // Check if we can exit now, after autocommands have changed things.
    ਜੇ (((eap->cmdidx == CMD_wq || curbuf_Is_ਬਦਲਿਆ()) && do_write(eap) == FAIL)
	    || before_quit_autocmds(curwin, FALSE, eap->forceit)
	    || check_ਹੋਰ(TRUE, eap->forceit) == FAIL
	    || (only_one_ਖਿੜਕੀ() && check_ਬਦਲਿਆ_any(eap->forceit, FALSE)))
    {
	not_exiting(save_exiting);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (only_one_ਖਿੜਕੀ())	    // quit last window, exit Vim
	    getout(0);
	not_exiting(save_exiting);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
	// Quit current window, may free the buffer.
	win_close(curwin, !buf_hide(curwin->w_ਬਫਰ));
    }
}

/*
 * ":print", ":list", ":number".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_print(exarg_T *eap)
{
    ਜੇ (curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)
	emsg(_(e_ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ));
    ਨਹੀਂਤਾਂ
    {
	ਲਈ ( ;!got_int; ui_breakcheck())
	{
	    print_ਸਤਰ(eap->line1,
		    (eap->cmdidx == CMD_ਸੰਖਿਆ || eap->cmdidx == CMD_pound
						 || (eap->flags & EXFLAG_NR)),
		    eap->cmdidx == CMD_list || (eap->flags & EXFLAG_LIST));
	    ਜੇ (++eap->line1 > eap->line2)
		ਰੋਕੋ;
	    ਬਾਹਰਲਾ_flush();	    // show one line at a time
	}
	setpcmark();
	// put cursor at last line
	curwin->w_cursor.lnum = eap->line2;
	beginline(BL_SOL | BL_FIX);
    }

    ex_no_reprint = TRUE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BYTEOFF
    ਸਥਿਰ ਖਾਲੀ
ex_goto(exarg_T *eap)
{
    goto_byte(eap->line2);
}
#ਜੇ_ਅੰਤ

/*
 * ":shell".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_shell(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    do_shell(ਨਿਰਾ, 0);
}

#ਜੇ defined(HAVE_DROP_FILE)

ਸਥਿਰ ਪੂਰਨ drop_busy = FALSE;
ਸਥਿਰ ਪੂਰਨ drop_filec;
ਸਥਿਰ char_u **drop_filev = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ drop_ਵੰਡੋ;
ਸਥਿਰ ਖਾਲੀ (*drop_callback)(ਖਾਲੀ *);
ਸਥਿਰ ਖਾਲੀ *drop_cookie;

    ਸਥਿਰ ਖਾਲੀ
handle_drop_ਅੰਦਰੂਨੀ(ਖਾਲੀ)
{
    exarg_T	ea;
    ਪੂਰਨ		save_ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_scroll;

    // Setting the argument list may cause screen updates and being called
    // recursively.  Avoid that by setting drop_busy.
    drop_busy = TRUE;

    // Check whether the current buffer is changed. If so, we will need
    // to split the current window or data could be lost.
    // We don't need to check if the 'hidden' option is set, as in this
    // case the buffer won't be lost.
    ਜੇ (!buf_hide(curbuf) && !drop_ਵੰਡੋ)
    {
	++emsg_off;
	drop_ਵੰਡੋ = check_ਬਦਲਿਆ(curbuf, CCGD_AW);
	--emsg_off;
    }
    ਜੇ (drop_ਵੰਡੋ)
    {
	ਜੇ (win_ਵੰਡੋ(0, 0) == FAIL)
	    ਵਾਪਸ;
	RESET_BINDING(curwin);

	// When splitting the window, create a new alist.  Otherwise the
	// existing one is overwritten.
	alist_unlink(curwin->w_alist);
	alist_new();
    }

    /*
     * Set up the new argument list.
     */
    alist_ਸੈੱਟ(ALIST(curwin), drop_filec, drop_filev, FALSE, ਨਿਰਾ, 0);

    /*
     * Move to the first file.
     */
    // Fake up a minimal "next" command for do_argfile()
    CLEAR_ਖੇਤਰ(ea);
    ea.cmd = (char_u *)"next";
    do_argfile(&ea, 0);

    // do_ecmd() may set need_start_insertmode, but since we never left Insert
    // mode that is not needed here.
    need_ਸ਼ੁਰੂ_insertmode = FALSE;

    // Restore msg_scroll, otherwise a following command may cause scrolling
    // unexpectedly.  The screen will be redrawn by the caller, thus
    // msg_scroll being set by displaying a message is irrelevant.
    ਸੁਨੇਹਾ_scroll = save_ਸੁਨੇਹਾ_scroll;

    ਜੇ (drop_callback != ਨਿਰਾ)
	drop_callback(drop_cookie);

    drop_filev = ਨਿਰਾ;
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
    ਖਾਲੀ
handle_drop(
    ਪੂਰਨ		filec,		// the number of files dropped
    char_u	**filev,	// the list of files dropped
    ਪੂਰਨ		ਵੰਡੋ,		// force splitting the window
    ਖਾਲੀ	(*callback)(ਖਾਲੀ *), // to be called after setting the argument
				     // list
    ਖਾਲੀ	*cookie)	// argument for "callback" (allocated)
{
    // Cannot handle recursive drops, finish the pending one.
    ਜੇ (drop_busy)
    {
	FreeWild(filec, filev);
	vim_free(cookie);
	ਵਾਪਸ;
    }

    // When calling handle_drop() more than once in a row we only use the last
    // one.
    ਜੇ (drop_filev != ਨਿਰਾ)
    {
	FreeWild(drop_filec, drop_filev);
	vim_free(drop_cookie);
    }

    drop_filec = filec;
    drop_filev = filev;
    drop_ਵੰਡੋ = ਵੰਡੋ;
    drop_callback = callback;
    drop_cookie = cookie;

    // Postpone this when:
    // - editing the command line
    // - not possible to change the current buffer
    // - updating the screen
    // As it may change buffers and window structures that are in use and cause
    // freed memory to be used.
    ਜੇ (ਪਾਠ_locked() || curbuf_locked() || updating_screen)
	ਵਾਪਸ;

    handle_drop_ਅੰਦਰੂਨੀ();
}

/*
 * To be called when text is unlocked, curbuf is unlocked or updating_screen is
 * reset: Handle a postponed drop.
 */
    ਖਾਲੀ
handle_any_postponed_drop(ਖਾਲੀ)
{
    ਜੇ (!drop_busy && drop_filev != ਨਿਰਾ
	     && !ਪਾਠ_locked() && !curbuf_locked() && !updating_screen)
	handle_drop_ਅੰਦਰੂਨੀ();
}
#ਜੇ_ਅੰਤ

/*
 * ":preserve".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_preserve(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    curbuf->b_flags |= BF_PRESERVED;
    ml_preserve(curbuf, TRUE);
}

/*
 * ":recover".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_recover(exarg_T *eap)
{
    // Set recoverymode right away to avoid the ATTENTION prompt.
    recoverymode = TRUE;
    ਜੇ (!check_ਬਦਲਿਆ(curbuf, (p_awa ? CCGD_AW : 0)
			     | CCGD_MULTWIN
			     | (eap->forceit ? CCGD_FORCEIT : 0)
			     | CCGD_EXCMD)

	    && (*eap->arg == NUL
			     || setfname(curbuf, eap->arg, ਨਿਰਾ, TRUE) == OK))
	ml_recover(TRUE);
    recoverymode = FALSE;
}

/*
 * Command modifier used in a wrong way.  Also for other commands that can't
 * appear at the toplevel.
 */
    ਸਥਿਰ ਖਾਲੀ
ex_wrongmodifier(exarg_T *eap)
{
    eap->errmsg = ex_errmsg(e_invalid_command_str, eap->cmd);
}

#ਜੇ defined(FEAT_EVAL)

// callback function for 'findfunc'
ਸਥਿਰ callback_T ffu_cb;

    ਸਥਿਰ callback_T *
get_findfunc_callback(ਖਾਲੀ)
{
    ਵਾਪਸ *curbuf->b_p_ffu != NUL ? &curbuf->b_ffu_cb : &ffu_cb;
}

/*
 * Call 'findfunc' to obtain a list of file names.
 */
    ਸਥਿਰ list_T *
call_findfunc(char_u *pat, ਪੂਰਨ cmdcomplete)
{
    typval_T	ਦਲੀਲਾਂ[3];
    callback_T	*cb;
    typval_T	rettv;
    ਪੂਰਨ		retval;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    ਦਲੀਲਾਂ[0].v_type = VAR_STRING;
    ਦਲੀਲਾਂ[0].vval.v_string = pat;
    ਦਲੀਲਾਂ[1].v_type = VAR_BOOL;
    ਦਲੀਲਾਂ[1].vval.v_ਸੰਖਿਆ = cmdcomplete;
    ਦਲੀਲਾਂ[2].v_type = VAR_UNKNOWN;

    // Lock the text to prevent weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;

    ctx = get_option_sctx("findfunc");
    ਜੇ (ctx != ਨਿਰਾ)
	current_sctx = *ctx;

    cb = get_findfunc_callback();
    retval = call_callback(cb, -1, &rettv, 2, ਦਲੀਲਾਂ);

    current_sctx = saved_sctx;

    --textlock;

    list_T *retlist = ਨਿਰਾ;

    ਜੇ (retval == OK)
    {
	ਜੇ (rettv.v_type == VAR_LIST)
	    retlist = list_copy(rettv.vval.v_list, FALSE, FALSE, get_copyID());
	ਨਹੀਂਤਾਂ
	    emsg(_(e_invalid_return_type_from_findfunc));

	clear_tv(&rettv);
    }

    ਵਾਪਸ retlist;
}

/*
 * Find file names matching "pat" using 'findfunc' and return it in "files".
 * Used for expanding the :find, :sfind and :tabfind command argument.
 * Returns OK on success and FAIL otherwise.
 */
    ਪੂਰਨ
expand_findfunc(expand_T *xp, char_u *pat, char_u ***files, ਪੂਰਨ *ਅੰਕ_Matches)
{
    list_T	*l;
    ਪੂਰਨ		len;

    *ਅੰਕ_Matches = 0;
    *files = ਨਿਰਾ;

    l = call_findfunc(pat, VVAL_TRUE);

    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    len = list_len(l);
    ਜੇ (len == 0)	    // empty List
    {
	list_free(l);
	ਵਾਪਸ FAIL;
    }

    expand_process_user_list(l, files, ਅੰਕ_Matches, xp);
    list_free(l);

    ਵਾਪਸ OK;
}

/*
 * Use 'findfunc' to find file 'findarg'.  The 'count' argument is used to find
 * the n'th matching file.
 */
    ਸਥਿਰ char_u *
findfunc_ਲੱਭੋ_file(char_u *findarg, ਪੂਰਨ findarg_len, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    list_T	*fname_list;
    char_u	*ret_fname = ਨਿਰਾ;
    char_u	cc;
    ਪੂਰਨ		fname_ਗਿਣਤੀ;

    cc = findarg[findarg_len];
    findarg[findarg_len] = NUL;

    fname_list = call_findfunc(findarg, VVAL_FALSE);
    fname_ਗਿਣਤੀ = list_len(fname_list);

    ਜੇ (fname_ਗਿਣਤੀ == 0)
	semsg(_(e_cant_ਲੱਭੋ_file_str_ਵਿੱਚ_ਰਸਤਾ), findarg);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ਗਿਣਤੀ_2 > fname_ਗਿਣਤੀ)
	    semsg(_(e_no_ਹੋਰ_file_str_ਮਿਲਿਆ_ਵਿੱਚ_ਰਸਤਾ), findarg);
	ਨਹੀਂਤਾਂ
	{
	    listitem_T *li = list_ਲੱਭੋ(fname_list, ਗਿਣਤੀ_2 - 1);

	    ਜੇ (li != ਨਿਰਾ)
	    {
		typval_T *tv = &li->li_tv;

		ਜੇ (tv->v_type == VAR_STRING && tv->vval.v_string != ਨਿਰਾ)
		    ret_fname = vim_strsave(tv->vval.v_string);
		ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_DICT && tv->vval.v_dict != ਨਿਰਾ)
		    ret_fname = dict_get_string(tv->vval.v_dict, "word", TRUE);
	    }
	}
    }

    ਜੇ (fname_list != ਨਿਰਾ)
	list_free(fname_list);

    findarg[findarg_len] = cc;

    ਵਾਪਸ ret_fname;
}

/*
 * Process the 'findfunc' option value.
 * Returns NULL on success and an error message on failure.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_findfunc(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ	retval;

    ਜੇ (ਦਲੀਲਾਂ->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_ਸੈੱਟ_callback_func(curbuf->b_p_ffu, &curbuf->b_ffu_cb);
    ਨਹੀਂਤਾਂ
    {
	// global option set
	retval = option_ਸੈੱਟ_callback_func(p_ffu, &ffu_cb);
	// when using :set, free the local callback
	ਜੇ (!(ਦਲੀਲਾਂ->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_ffu_cb);
    }

    ਜੇ (retval == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    char_u	*name = get_scriptlocal_funcname(*varp);
    ਜੇ (name != ਨਿਰਾ)
    {
	free_string_option(*varp);
	*varp = name;
    }

    ਵਾਪਸ ਨਿਰਾ;
}

# ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_findfunc_option(ਖਾਲੀ)
{
    free_callback(&ffu_cb);
}
# endif

/*
 * Mark the global 'findfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_findfunc(ਪੂਰਨ copyID ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ abort = FALSE;
    abort = ਸੈੱਟ_ref_ਵਿੱਚ_callback(&ffu_cb, copyID);
    ਵਾਪਸ abort;
}
#ਜੇ_ਅੰਤ

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
    ਖਾਲੀ
ex_splitview(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    char_u	*fname = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
    char_u	dot_ਰਸਤਾ[] = ".";
    ਪੂਰਨ		save_cmod_flags = cmdmod.cmod_flags;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		use_tab = eap->cmdidx == CMD_tabedit
		       || eap->cmdidx == CMD_tabfind
		       || eap->cmdidx == CMD_tabnew;

    ਜੇ (ERROR_IF_ANY_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    // A ":split" in the quickfix window works like ":new".  Don't want two
    // quickfix windows.  But it's OK when doing ":tab split".
    ਜੇ (bt_quickfix(curbuf) && cmdmod.cmod_tab == 0)
    {
	ਜੇ (eap->cmdidx == CMD_ਵੰਡੋ)
	    eap->cmdidx = CMD_new;
	ਜੇ (eap->cmdidx == CMD_vsplit)
	    eap->cmdidx = CMD_vnew;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (eap->cmdidx == CMD_sfind || eap->cmdidx == CMD_tabfind)
    {
	char_u	*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ = ਨਿਰਾ;
	ਅੱਖਰ	*ਖੋਜ_ctx = ਨਿਰਾ;

	ਜੇ (*get_findfunc() != NUL)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    fname = findfunc_ਲੱਭੋ_file(eap->arg, (ਪੂਰਨ)STRLEN(eap->arg),
				       eap->addr_ਗਿਣਤੀ > 0 ? eap->line2 : 1);
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ
	{
	    fname = ਲੱਭੋ_file_ਵਿੱਚ_ਰਸਤਾ(eap->arg, (ਪੂਰਨ)STRLEN(eap->arg),
					      FNAME_MESS, TRUE, curbuf->b_ffname,
					      &file_ਮੰਜ਼ਿਲ_ਲੱਭੋ, &ਖੋਜ_ctx);
	    vim_free(file_ਮੰਜ਼ਿਲ_ਲੱਭੋ);
	    vim_findfile_cleanup(ਖੋਜ_ctx);
	}
	ਜੇ (fname == ਨਿਰਾ)
	    ਜਾਓ theend;
	eap->arg = fname;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
    ਨਹੀਂਤਾਂ ਜੇ ((cmdmod.cmod_flags & CMOD_BROWSE)
	    && eap->cmdidx != CMD_vnew
	    && eap->cmdidx != CMD_new)
    {
	ਜੇ (
# ifdef FEAT_GUI
	    !gui.ਵਿੱਚ_use &&
# endif
		au_has_ਸਮੂਹ((char_u *)"FileExplorer"))
	{
	    // No browsing supported but we do have the file explorer:
	    // Edit the directory.
	    ਜੇ (*eap->arg == NUL || !mch_isdir(eap->arg))
		eap->arg = dot_ਰਸਤਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    fname = do_browse(0, (char_u *)(use_tab
			? _("Edit File in new tab page")
			: _("Edit File in new window")),
					  eap->arg, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, curbuf);
	    ਜੇ (fname == ਨਿਰਾ)
		ਜਾਓ theend;
	    eap->arg = fname;
	}
    }
    cmdmod.cmod_flags &= ~CMOD_BROWSE;	// Don't browse again in do_ecmd().
#ਜੇ_ਅੰਤ

    /*
     * Either open new tab page or split the window.
     */
    ਜੇ (use_tab)
    {
	ਜੇ (win_new_tabpage(cmdmod.cmod_tab != 0 ? cmdmod.cmod_tab
			 : eap->addr_ਗਿਣਤੀ == 0 ? 0
					       : (ਪੂਰਨ)eap->line2 + 1) != FAIL)
	{
	    do_exedit(eap, old_curwin);

	    // set the alternate buffer for the window we came from
	    ਜੇ (curwin != old_curwin
		    && win_valid(old_curwin)
		    && old_curwin->w_ਬਫਰ != curbuf
		    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		old_curwin->w_alt_fnum = curbuf->b_fnum;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (win_ਵੰਡੋ(eap->addr_ਗਿਣਤੀ > 0 ? (ਪੂਰਨ)eap->line2 : 0,
				     *eap->cmd == 'v' ? WSP_VERT : 0) != FAIL)
    {
	// Reset 'scrollbind' when editing another file, but keep it when
	// doing ":split" without arguments.
	ਜੇ (*eap->arg != NUL)
	    RESET_BINDING(curwin);
	ਨਹੀਂਤਾਂ
	    do_check_scrollbind(FALSE);
	do_exedit(eap, old_curwin);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
    cmdmod.cmod_flags = save_cmod_flags;
#ਜੇ_ਅੰਤ

theend:
    vim_free(fname);
}

/*
 * Open a new tab page.
 */
    ਖਾਲੀ
tabpage_new(ਖਾਲੀ)
{
    exarg_T	ea;

    CLEAR_ਖੇਤਰ(ea);
    ea.cmdidx = CMD_tabnew;
    ea.cmd = (char_u *)"tabn";
    ea.arg = (char_u *)"";
    ex_splitview(&ea);
}

/*
 * :tabnext command
 */
    ਸਥਿਰ ਖਾਲੀ
ex_tabnext(exarg_T *eap)
{
    ਪੂਰਨ tab_ਸੰਖਿਆ;

    ਜੇ (ERROR_IF_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;
    ਚੋਣ (eap->cmdidx)
    {
	ਹਾਲਤ CMD_tabfirst:
	ਹਾਲਤ CMD_tabrewind:
	    goto_tabpage(1);
	    ਰੋਕੋ;
	ਹਾਲਤ CMD_tablast:
	    goto_tabpage(9999);
	    ਰੋਕੋ;
	ਹਾਲਤ CMD_tabprevious:
	ਹਾਲਤ CMD_tab_ਅਗਲਾ:
	    ਜੇ (eap->arg && *eap->arg != NUL)
	    {
		char_u *p = eap->arg;
		char_u *p_save = p;

		tab_ਸੰਖਿਆ = getdigits(&p);
		ਜੇ (p == p_save || *p_save == '-' || *p != NUL
			    || tab_ਸੰਖਿਆ == 0)
		{
		    // No numbers as argument.
		    eap->errmsg = ex_errmsg(e_invalid_ਦਲੀਲ_str, eap->arg);
		    ਵਾਪਸ;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (eap->addr_ਗਿਣਤੀ == 0)
		    tab_ਸੰਖਿਆ = 1;
		ਨਹੀਂਤਾਂ
		{
		    tab_ਸੰਖਿਆ = eap->line2;
		    ਜੇ (tab_ਸੰਖਿਆ < 1)
		    {
			eap->errmsg = _(e_invalid_range);
			ਵਾਪਸ;
		    }
		}
	    }
	    goto_tabpage(-tab_ਸੰਖਿਆ);
	    ਰੋਕੋ;
	ਮੂਲ: // CMD_tabnext
	    tab_ਸੰਖਿਆ = get_tabpage_arg(eap);
	    ਜੇ (eap->errmsg == ਨਿਰਾ)
		goto_tabpage(tab_ਸੰਖਿਆ);
	    ਰੋਕੋ;
    }
}

/*
 * :tabmove command
 */
    ਸਥਿਰ ਖਾਲੀ
ex_tabmove(exarg_T *eap)
{
    ਪੂਰਨ tab_ਸੰਖਿਆ;

    tab_ਸੰਖਿਆ = get_tabpage_arg(eap);
    ਜੇ (eap->errmsg == ਨਿਰਾ)
	tabpage_move(tab_ਸੰਖਿਆ);
}

/*
 * :tabs command: List tabs and their contents.
 */
    ਸਥਿਰ ਖਾਲੀ
ex_tabs(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    tabpage_T	*tp;
    win_T	*wp;
    ਪੂਰਨ		tabcount = 1;

    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
    ਸੁਨੇਹਾ_scroll = TRUE;
    ਲਈ (tp = ਪਹਿਲਾ_tabpage; tp != ਨਿਰਾ && !got_int; tp = tp->tp_ਅਗਲਾ)
    {
	ਸੁਨੇਹਾ_putchar('\n');
	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, _("Tab page %d"), tabcount++);
	ਸੁਨੇਹਾ_outtrans_attr(IObuff, HL_ATTR(HLF_T));
	ਬਾਹਰਲਾ_flush();	    // output one line at a time
	ui_breakcheck();

	ਜੇ (tp  == curtab)
	    wp = firstwin;
	ਨਹੀਂਤਾਂ
	    wp = tp->tp_firstwin;
	ਲਈ ( ; wp != ਨਿਰਾ && !got_int; wp = wp->w_ਅਗਲਾ)
	{
	    ਸੁਨੇਹਾ_putchar('\n');
	    ਸੁਨੇਹਾ_putchar(wp == curwin ? '>' : ' ');
	    ਸੁਨੇਹਾ_putchar(' ');
	    ਸੁਨੇਹਾ_putchar(buf_Is_ਬਦਲਿਆ(wp->w_ਬਫਰ) ? '+' : ' ');
	    ਸੁਨੇਹਾ_putchar(' ');
	    ਜੇ (buf_spname(wp->w_ਬਫਰ) != ਨਿਰਾ)
		vim_strncpy(IObuff, buf_spname(wp->w_ਬਫਰ), IOSIZE - 1);
	    ਨਹੀਂਤਾਂ
		home_replace(wp->w_ਬਫਰ, wp->w_ਬਫਰ->b_fname,
							IObuff, IOSIZE, TRUE);
	    ਸੁਨੇਹਾ_outtrans(IObuff);
	    ਬਾਹਰਲਾ_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }
}

/*
 * ":mode": Set screen mode.
 * If no argument given, just get the screen size and redraw.
 */
    ਸਥਿਰ ਖਾਲੀ
ex_ਬਹੁਲਕ(exarg_T *eap)
{
    ਜੇ (*eap->arg == NUL)
	shell_resized();
    ਨਹੀਂਤਾਂ
	emsg(_(e_screen_ਬਹੁਲਕ_setting_not_supported));
}

/*
 * ":resize".
 * set, increment or decrement current window height
 */
    ਸਥਿਰ ਖਾਲੀ
ex_resize(exarg_T *eap)
{
    ਪੂਰਨ		n;
    win_T	*wp = curwin;

    ਜੇ (eap->addr_ਗਿਣਤੀ > 0)
    {
	n = eap->line2;
	ਲਈ (wp = firstwin; wp->w_ਅਗਲਾ != ਨਿਰਾ && --n > 0; wp = wp->w_ਅਗਲਾ)
	    ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
    n = atol((ਅੱਖਰ *)eap->arg);
    ਜੇ (cmdmod.cmod_ਵੰਡੋ & WSP_VERT)
    {
	ਜੇ (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_ਚੌੜਾਈ;
	ਨਹੀਂਤਾਂ ਜੇ (n == 0 && eap->arg[0] == NUL)	// default is very wide
	    n = 9999;
	win_setwidth_win(n, wp);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_ਉਚਾਈ;
	ਨਹੀਂਤਾਂ ਜੇ (n == 0 && eap->arg[0] == NUL)	// default is very high
	    n = 9999;
	win_setheight_win(n, wp);
    }
}

/*
 * ":find [+command] <file>" command.
 */
    ਸਥਿਰ ਖਾਲੀ
ex_ਲੱਭੋ(exarg_T *eap)
{
    ਜੇ (!check_can_ਸੈੱਟ_curbuf_forceit(eap->forceit))
	ਵਾਪਸ;

    char_u	*fname = ਨਿਰਾ;
    ਪੂਰਨ		ਗਿਣਤੀ_2;
    char_u	*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ = ਨਿਰਾ;
    ਅੱਖਰ	*ਖੋਜ_ctx = ਨਿਰਾ;

    ਜੇ (*get_findfunc() != NUL)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	fname = findfunc_ਲੱਭੋ_file(eap->arg, (ਪੂਰਨ)STRLEN(eap->arg),
					eap->addr_ਗਿਣਤੀ > 0 ? eap->line2 : 1);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
	fname = ਲੱਭੋ_file_ਵਿੱਚ_ਰਸਤਾ(eap->arg, (ਪੂਰਨ)STRLEN(eap->arg), FNAME_MESS,
			       TRUE, curbuf->b_ffname, &file_ਮੰਜ਼ਿਲ_ਲੱਭੋ, &ਖੋਜ_ctx);
	ਜੇ (eap->addr_ਗਿਣਤੀ > 0)
	{
	    // Repeat finding the file "count" times.  This matters when it appears
	    // several times in the path.
	    ਗਿਣਤੀ_2 = eap->line2;
	    ਜਦੋਂ (fname != ਨਿਰਾ && --ਗਿਣਤੀ_2 > 0)
	    {
		vim_free(fname);
		fname = ਲੱਭੋ_file_ਵਿੱਚ_ਰਸਤਾ(ਨਿਰਾ, 0, FNAME_MESS,
			      FALSE, curbuf->b_ffname, &file_ਮੰਜ਼ਿਲ_ਲੱਭੋ, &ਖੋਜ_ctx);
	    }
	}
	VIM_CLEAR(file_ਮੰਜ਼ਿਲ_ਲੱਭੋ);
	vim_findfile_cleanup(ਖੋਜ_ctx);
    }

    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ;

    eap->arg = fname;
    do_exedit(eap, ਨਿਰਾ);
    vim_free(fname);
}

/*
 * ":open" simulation: for now works just like ":visual".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_open(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*p;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (not_ਵਿੱਚ_vim9(eap, false) == FAIL)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_SOL | BL_FIX);
    ਜੇ (*eap->arg == '/')
    {
	// ":open /pattern/": put cursor in column found with pattern
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg, magic_isset() ? RE_MAGIC : 0);
	ਜੇ (regmatch.regprog != ਨਿਰਾ)
	{
	    // make a copy of the line, when searching for a mark it might be
	    // flushed
	    char_u *ਸਤਰ = vim_strsave(ml_get_curline());

	    regmatch.rm_ic = p_ic;
	    ਜੇ (vim_regexec(&regmatch, ਸਤਰ, (colnr_T)0))
		curwin->w_cursor.ਖਾਨਾ = (colnr_T)(regmatch.startp[0] - ਸਤਰ);
	    ਨਹੀਂਤਾਂ
		emsg(_(e_no_ਮੇਲ));
	    vim_regfree(regmatch.regprog);
	    vim_free(ਸਤਰ);
	}
	// Move to the NUL, ignore any other arguments.
	eap->arg += STRLEN(eap->arg);
    }
    check_cursor();

    eap->cmdidx = CMD_visual;
    do_exedit(eap, ਨਿਰਾ);
}

/*
 * ":edit", ":badd", ":balt", ":visual".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_edit(exarg_T *eap)
{
    char_u *ffname = eap->cmdidx == CMD_enew ? ਨਿਰਾ : eap->arg;

    // Exclude commands which keep the window's current buffer
    ਜੇ (
	    eap->cmdidx != CMD_badd
	    && eap->cmdidx != CMD_balt
	    // All other commands must obey 'winfixbuf' / ! rules
	    && (is_other_file(0, ffname) && !check_can_ਸੈੱਟ_curbuf_forceit(eap->forceit))
    )
	ਵਾਪਸ;

    do_exedit(eap, ਨਿਰਾ);
}

/*
 * ":edit <file>" command and alike.
 */
    ਖਾਲੀ
do_exedit(
    exarg_T	*eap,
    win_T	*old_curwin)	    // curwin before doing a split or NULL
{
    ਪੂਰਨ		n;
    ਪੂਰਨ		need_hide;
    ਪੂਰਨ		exmode_was = exmode_ਸਰਗਰਮ;

    ਜੇ ((eap->cmdidx != CMD_pedit && ERROR_IF_POPUP_ਖਿੜਕੀ)
						 || ERROR_IF_ਪਦ_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;
    /*
     * ":vi" command ends Ex mode.
     */
    ਜੇ (exmode_ਸਰਗਰਮ && (eap->cmdidx == CMD_visual
						|| eap->cmdidx == CMD_view))
    {
	exmode_ਸਰਗਰਮ = FALSE;
	ex_pressedreturn = FALSE;
	ਜੇ (*eap->arg == NUL)
	{
	    // Special case:  ":global/pat/visual\NLvi-commands"
	    ਜੇ (global_busy)
	    {
		ਜੇ (eap->nextcmd != ਨਿਰਾ)
		{
		    stuffReadbuff(eap->nextcmd);
		    eap->nextcmd = ਨਿਰਾ;
		}

		ਜੇ (exmode_was != EXMODE_VIM)
		    settmode(TMODE_RAW);
		ਪੂਰਨ save_RedrawingDisabled = RedrawingDisabled;
		RedrawingDisabled = 0;
		ਪੂਰਨ save_nwr = no_wait_return;
		no_wait_return = 0;
		need_wait_return = FALSE;
		ਪੂਰਨ save_ms = ਸੁਨੇਹਾ_scroll;
		ਸੁਨੇਹਾ_scroll = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		ਪੂਰਨ save_he = hold_gui_events;
		hold_gui_events = 0;
#ਜੇ_ਅੰਤ
		ਸੈੱਟ_must_redraw(UPD_CLEAR);
		pending_exmode_ਸਰਗਰਮ = TRUE;

		main_loop(FALSE, TRUE);

		pending_exmode_ਸਰਗਰਮ = FALSE;
		RedrawingDisabled = save_RedrawingDisabled;
		no_wait_return = save_nwr;
		ਸੁਨੇਹਾ_scroll = save_ms;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		hold_gui_events = save_he;
#ਜੇ_ਅੰਤ
	    }
	    ਵਾਪਸ;
	}
    }

    ਜੇ ((eap->cmdidx == CMD_new
		|| eap->cmdidx == CMD_tabnew
		|| eap->cmdidx == CMD_tabedit
		|| eap->cmdidx == CMD_vnew) && *eap->arg == NUL)
    {
	// ":new" or ":tabnew" without argument: edit a new empty buffer
	setpcmark();
	(ਖਾਲੀ)do_ecmd(0, ਨਿਰਾ, ਨਿਰਾ, eap, ECMD_ONE,
		      ECMD_HIDE + (eap->forceit ? ECMD_FORCEIT : 0),
		      old_curwin == ਨਿਰਾ ? curwin : ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ ਜੇ ((eap->cmdidx != CMD_ਵੰਡੋ && eap->cmdidx != CMD_vsplit)
	    || *eap->arg != NUL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
	    || (cmdmod.cmod_flags & CMOD_BROWSE)
#ਜੇ_ਅੰਤ
	    )
    {
	// Can't edit another file when "textlock" or "curbuf_lock" is set.
	// Only ":edit" or ":script" can bring us here, others are stopped
	// earlier.
	ਜੇ (*eap->arg != NUL && ਪਾਠ_or_buf_locked())
	    ਵਾਪਸ;

	n = readonlymode;
	ਜੇ (eap->cmdidx == CMD_view || eap->cmdidx == CMD_sview)
	    readonlymode = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_enew)
	    readonlymode = FALSE;   // 'readonly' doesn't make sense in an
				    // empty buffer
	ਜੇ (eap->cmdidx != CMD_balt && eap->cmdidx != CMD_badd)
	    setpcmark();
	ਜੇ (do_ecmd(0, (eap->cmdidx == CMD_enew ? ਨਿਰਾ : eap->arg),
		    ਨਿਰਾ, eap,
		    // ":edit" goes to first line if Vi compatible
		    (*eap->arg == NUL && eap->do_ecmd_lnum == 0
				      && vim_strchr(p_cpo, CPO_GOTO1) != ਨਿਰਾ)
					       ? ECMD_ONE : eap->do_ecmd_lnum,
		    (buf_hide(curbuf) ? ECMD_HIDE : 0)
		    + (eap->forceit ? ECMD_FORCEIT : 0)
		      // after a split we can use an existing buffer
		    + (old_curwin != ਨਿਰਾ ? ECMD_OLDBUF : 0)
		    + (eap->cmdidx == CMD_badd ? ECMD_ADDBUF : 0)
		    + (eap->cmdidx == CMD_balt ? ECMD_ALTBUF : 0)
		    , old_curwin == ਨਿਰਾ ? curwin : ਨਿਰਾ) == FAIL)
	{
	    // Editing the file failed.  If the window was split, close it.
	    ਜੇ (old_curwin != ਨਿਰਾ)
	    {
		need_hide = (curbuf_Is_ਬਦਲਿਆ() && curbuf->b_nwindows <= 1);
		ਜੇ (!need_hide || buf_hide(curbuf))
		{
#ਜੇ defined(FEAT_EVAL)
		    cleanup_T   cs;

		    // Reset the error/interrupt/exception state here so that
		    // aborting() returns FALSE when closing a window.
		    enter_cleanup(&cs);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ
		    win_close(curwin, !need_hide && !buf_hide(curbuf));

#ਜੇ defined(FEAT_EVAL)
		    // Restore the error/interrupt/exception state if not
		    // discarded by a new aborting error, interrupt, or
		    // uncaught exception.
		    leave_cleanup(&cs);
#ਜੇ_ਅੰਤ
		}
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (readonlymode && curbuf->b_nwindows == 1)
	{
	    // When editing an already visited buffer, 'readonly' won't be set
	    // but the previous value is kept.  With ":view" and ":sview" we
	    // want the  file to be readonly, except when another window is
	    // editing the same buffer.
	    curbuf->b_p_ro = TRUE;
	}
	readonlymode = n;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (eap->do_ecmd_cmd != ਨਿਰਾ)
	    do_cmd_ਦਲੀਲ(eap->do_ecmd_cmd);
	n = curwin->w_arg_idx_invalid;
	check_arg_idx(curwin);
	ਜੇ (n != curwin->w_arg_idx_invalid)
	    maketitle();
    }

    /*
     * if ":split file" worked, set alternate file name in old window to new
     * file
     */
    ਜੇ (old_curwin != ਨਿਰਾ
	    && *eap->arg != NUL
	    && curwin != old_curwin
	    && win_valid(old_curwin)
	    && old_curwin->w_ਬਫਰ != curbuf
	    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	old_curwin->w_alt_fnum = curbuf->b_fnum;

    ex_no_reprint = TRUE;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
/*
 * ":gui" and ":gvim" when there is no GUI.
 */
    ਸਥਿਰ ਖਾਲੀ
ex_nogui(exarg_T *eap)
{
    eap->errmsg = _(e_gui_cannot_be_ਵਰਤਿਆ_not_enabled_at_compile_ਸਮਾਂ);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
    ਸਥਿਰ ਖਾਲੀ
ex_tearoff(exarg_T *eap)
{
    gui_make_tearoff(eap->arg);
}
#ਜੇ_ਅੰਤ

#ਜੇ (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_ਪਦ_POPUP_MENU)) && defined(FEAT_MENU)
    ਸਥਿਰ ਖਾਲੀ
ex_popup(exarg_T *eap)
{
# ਜੇ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    ਜੇ (gui.ਵਿੱਚ_use)
	gui_make_popup(eap->arg, eap->forceit);
#  ifdef FEAT_ਪਦ_POPUP_MENU
    ਨਹੀਂਤਾਂ
#  endif
# endif
# ifdef FEAT_ਪਦ_POPUP_MENU
	pum_make_popup(eap->arg, eap->forceit);
# endif
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਖਾਲੀ
ex_swapname(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਜੇ (curbuf->b_ml.ml_mfp == ਨਿਰਾ || curbuf->b_ml.ml_mfp->mf_fname == ਨਿਰਾ)
	ਸੁਨੇਹਾ(_("No swap file"));
    ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ((ਅੱਖਰ *)curbuf->b_ml.ml_mfp->mf_fname);
}

/*
 * ":syncbind" forces all 'scrollbind' windows to have the same relative
 * offset.
 * (1998-11-02 16:21:01  R. Edward Ralston <eralston@computer.org>)
 */
    ਸਥਿਰ ਖਾਲੀ
ex_syncbind(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    win_T	*wp;
    win_T	*save_curwin = curwin;
    buf_T	*save_curbuf = curbuf;
    ਲੰਮਾ	topline;
    ਲੰਮਾ	y;
    linenr_T	old_linenr = curwin->w_cursor.lnum;

    setpcmark();

    /*
     * determine max topline
     */
    ਜੇ (curwin->w_p_scb)
    {
	topline = curwin->w_topline;
	FOR_ALL_WINDOWS(wp)
	{
	    ਜੇ (wp->w_p_scb && wp->w_ਬਫਰ)
	    {
		y = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - get_scrolloff_ਮੁੱਲ();
		ਜੇ (topline > y)
		    topline = y;
	    }
	}
	ਜੇ (topline < 1)
	    topline = 1;
    }
    ਨਹੀਂਤਾਂ
    {
	topline = 1;
    }


    /*
     * Set all scrollbind windows to the same topline.
     */
    FOR_ALL_WINDOWS(curwin)
    {
	ਜੇ (curwin->w_p_scb)
	{
	    curbuf = curwin->w_ਬਫਰ;
	    y = topline - curwin->w_topline;
	    ਜੇ (y > 0)
		scrollup(y, TRUE);
	    ਨਹੀਂਤਾਂ
		scrolldown(-y, TRUE);
	    curwin->w_scbind_pos = topline;
	    redraw_later(UPD_VALID);
	    cursor_correct();
	    curwin->w_redr_ਸਥਿਤੀ = true;
	}
    }
    curwin = save_curwin;
    curbuf = save_curbuf;
    ਜੇ (curwin->w_p_scb)
    {
	did_syncbind = TRUE;
	checkpcmark();
	ਜੇ (old_linenr != curwin->w_cursor.lnum)
	{
	    char_u ctrl_o[2];

	    ctrl_o[0] = Ctrl_O;
	    ctrl_o[1] = 0;
	    ins_typebuf(ctrl_o, REMAP_NONE, 0, TRUE, FALSE);
	}
    }
}


    ਸਥਿਰ ਖਾਲੀ
ex_read(exarg_T *eap)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਖਾਲੀ_ਥਾਵਾਂ_2 = (curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ);
    linenr_T	lnum;

    ਜੇ (eap->usefilter)			// :r!cmd
    {
	do_bang(1, eap, FALSE, FALSE, TRUE);
	ਵਾਪਸ;
    }

    ਜੇ (u_save(eap->line2, (linenr_T)(eap->line2 + 1)) == FAIL)
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
    ਜੇ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browseFile;

	browseFile = do_browse(0, (char_u *)_("Append File"), eap->arg,
		ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, curbuf);
	ਜੇ (browseFile != ਨਿਰਾ)
	{
	    i = readfile(browseFile, ਨਿਰਾ,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	    vim_free(browseFile);
	}
	ਨਹੀਂਤਾਂ
	    i = OK;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਜੇ (*eap->arg == NUL)
	{
	    ਜੇ (check_fname() == FAIL)	// check for no file name
		ਵਾਪਸ;
	    i = readfile(curbuf->b_ffname, curbuf->b_fname,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (vim_strchr(p_cpo, CPO_ALTREAD) != ਨਿਰਾ)
		(ਖਾਲੀ)setaltfname(eap->arg, eap->arg, (linenr_T)1);
	    i = readfile(eap->arg, ਨਿਰਾ,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);

	}
    ਜੇ (i != OK)
    {
#ਜੇ defined(FEAT_EVAL)
	ਜੇ (!aborting())
#ਜੇ_ਅੰਤ
	    semsg(_(e_cant_open_file_str), eap->arg);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_2 && exmode_ਸਰਗਰਮ)
	{
	    // Delete the empty line that remains.  Historically ex does
	    // this but vi doesn't.
	    ਜੇ (eap->line2 == 0)
		lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    ਨਹੀਂਤਾਂ
		lnum = 1;
	    ਜੇ (*ml_get(lnum) == NUL && u_savedel(lnum, 1L) == OK)
	    {
		ml_delete(lnum);
		ਜੇ (curwin->w_cursor.lnum > 1
			&& curwin->w_cursor.lnum >= lnum)
		    --curwin->w_cursor.lnum;
		deleted_ਸਤਰਾਂ_mark(lnum, 1L);
	    }
	}
	redraw_curbuf_later(UPD_VALID);
    }
}

ਸਥਿਰ char_u	*prev_dir = ਨਿਰਾ;

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_cd_dir(ਖਾਲੀ)
{
    VIM_CLEAR(prev_dir);
    VIM_CLEAR(globaldir);
}
#ਜੇ_ਅੰਤ

/*
 * Get the previous directory for the given chdir scope.
 */
    ਸਥਿਰ char_u *
get_prevdir(cdscope_T scope)
{
    ਜੇ (scope == CDSCOPE_ਖਿੜਕੀ)
	ਵਾਪਸ curwin->w_prevdir;
    ਨਹੀਂਤਾਂ ਜੇ (scope == CDSCOPE_TABPAGE)
	ਵਾਪਸ curtab->tp_prevdir;
    ਵਾਪਸ prev_dir;
}

/*
 * Deal with the side effects of changing the current directory.
 * When 'scope' is CDSCOPE_TABPAGE then this was after an ":tcd" command.
 * When 'scope' is CDSCOPE_WINDOW then this was after an ":lcd" command.
 */
    ਖਾਲੀ
post_chdir(cdscope_T scope)
{
    ਜੇ (scope != CDSCOPE_ਖਿੜਕੀ)
	// Clear tab local directory for both :cd and :tcd
	VIM_CLEAR(curtab->tp_localdir);
    VIM_CLEAR(curwin->w_localdir);
    ਜੇ (scope != CDSCOPE_GLOBAL)
    {
	char_u	*pdir = get_prevdir(scope);

	// If still in the global directory, need to remember current
	// directory as the global directory.
	ਜੇ (globaldir == ਨਿਰਾ && pdir != ਨਿਰਾ)
	    globaldir = vim_strsave(pdir);

	// Remember this local directory for the window.
	ਜੇ (mch_dirname(NameBuff, MAXPATHL) == OK)
	{
	    ਜੇ (scope == CDSCOPE_TABPAGE)
		curtab->tp_localdir = vim_strsave(NameBuff);
	    ਨਹੀਂਤਾਂ
		curwin->w_localdir = vim_strsave(NameBuff);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// We are now in the global directory, no need to remember its name.
	VIM_CLEAR(globaldir);
    }

    ਆਖਰੀ_chdir_reason = ਨਿਰਾ;
    shorten_fnames(vim_strchr(p_cpo, CPO_NOSYMLINKS) == ਨਿਰਾ);
}

/*
 * Trigger DirChangedPre for "acmd_fname" with directory "new_dir".
 */
    ਖਾਲੀ
trigger_Dir_ਬਦਲਿਆ_Pre(char_u *acmd_fname, char_u *new_dir)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;

    v_event = get_v_event(&save_v_event);
    (ਖਾਲੀ)dict_ਜੋੜੋ_string(v_event, "directory", new_dir);
    dict_ਸੈੱਟ_ਚੀਜ਼ਾਂ_ro(v_event);
#ਜੇ_ਅੰਤ
    apply_autocmds(EVENT_DIRCHANGEDPRE, acmd_fname, new_dir, FALSE, curbuf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    restore_v_event(v_event, &save_v_event);
#ਜੇ_ਅੰਤ
}

/*
 * Change directory function used by :cd/:tcd/:lcd Ex commands and the
 * chdir() function.
 * scope == CDSCOPE_WINDOW: changes the window-local directory
 * scope == CDSCOPE_TABPAGE: changes the tab-local directory
 * Otherwise: changes the global directory
 * Returns TRUE if the directory is successfully changed.
 */
    ਪੂਰਨ
changedir_func(
	char_u		*new_dir,
	ਪੂਰਨ		forceit,
	cdscope_T	scope)
{
    char_u	*pdir = ਨਿਰਾ;
    ਪੂਰਨ		dir_differs;
    char_u	*acmd_fname = ਨਿਰਾ;
    char_u	**pp;
    char_u	*tofree;

    ਜੇ (new_dir == ਨਿਰਾ || allbuf_locked())
	ਵਾਪਸ FALSE;

    ਜੇ (vim_strchr(p_cpo, CPO_CHDIR) != ਨਿਰਾ && curbuf_Is_ਬਦਲਿਆ() && !forceit)
    {
	emsg(_(e_cannot_change_directory_ਬਫਰ_is_modified_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_override));
	ਵਾਪਸ FALSE;
    }

    // ":cd -": Change to previous directory
    ਜੇ (STRCMP(new_dir, "-") == 0)
    {
	pdir = get_prevdir(scope);
	ਜੇ (pdir == ਨਿਰਾ)
	{
	    emsg(_(e_no_ਪਿਛਲਾ_directory));
	    ਵਾਪਸ FALSE;
	}
	new_dir = pdir;
    }

    // Save current directory for next ":cd -"
    ਜੇ (mch_dirname(NameBuff, MAXPATHL) == OK)
	pdir = vim_strsave(NameBuff);
    ਨਹੀਂਤਾਂ
	pdir = ਨਿਰਾ;

    // For UNIX ":cd" means: go to home directory.
    // On other systems too if 'cdhome' is set.
#ਜੇ defined(UNIX) || defined(VMS)
    ਜੇ (*new_dir == NUL)
#ਨਹੀਂਤਾਂ
    ਜੇ (*new_dir == NUL && p_cdh)
#ਜੇ_ਅੰਤ
    {
	// use NameBuff for home directory name
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
	char_u	*p;

	p = mch_getenv((char_u *)"SYS$LOGIN");
	ਜੇ (p == ਨਿਰਾ || *p == NUL)	// empty is the same as not set
	    NameBuff[0] = NUL;
	ਨਹੀਂਤਾਂ
	    vim_strncpy(NameBuff, p, MAXPATHL - 1);
#ਨਹੀਂਤਾਂ
	expand_env((char_u *)"$HOME", NameBuff, MAXPATHL);
#ਜੇ_ਅੰਤ
	new_dir = NameBuff;
    }
    dir_differs = pdir == ਨਿਰਾ
			    || pathcmp((ਅੱਖਰ *)pdir, (ਅੱਖਰ *)new_dir, -1) != 0;
    ਜੇ (dir_differs)
    {
	ਜੇ (scope == CDSCOPE_ਖਿੜਕੀ)
	    acmd_fname = (char_u *)"window";
	ਨਹੀਂਤਾਂ ਜੇ (scope == CDSCOPE_TABPAGE)
	    acmd_fname = (char_u *)"tabpage";
	ਨਹੀਂਤਾਂ
	    acmd_fname = (char_u *)"global";
	trigger_Dir_ਬਦਲਿਆ_Pre(acmd_fname, new_dir);

	ਜੇ (vim_chdir(new_dir))
	{
	    emsg(_(e_command_failed));
	    vim_free(pdir);
	    ਵਾਪਸ FALSE;
	}
    }

    ਜੇ (scope == CDSCOPE_ਖਿੜਕੀ)
	pp = &curwin->w_prevdir;
    ਨਹੀਂਤਾਂ ਜੇ (scope == CDSCOPE_TABPAGE)
	pp = &curtab->tp_prevdir;
    ਨਹੀਂਤਾਂ
	pp = &prev_dir;
    tofree = *pp;  // new_dir may use this
    *pp = pdir;

    post_chdir(scope);

    ਜੇ (dir_differs)
	apply_autocmds(EVENT_DIRCHANGED, acmd_fname, new_dir, FALSE, curbuf);
    vim_free(tofree);
    ਵਾਪਸ TRUE;
}

/*
 * ":cd", ":tcd", ":lcd", ":chdir" ":tchdir" and ":lchdir".
 */
    ਖਾਲੀ
ex_cd(exarg_T *eap)
{
    char_u	*new_dir;

    new_dir = eap->arg;
#ਜੇ !defined(UNIX) && !defined(VMS)
    // for non-UNIX ":cd" means: print current directory unless 'cdhome' is set
    ਜੇ (*new_dir == NUL && !p_cdh)
    {
	ex_pwd(ਨਿਰਾ);
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    cdscope_T	scope = CDSCOPE_GLOBAL;

    ਜੇ (eap->cmdidx == CMD_lcd || eap->cmdidx == CMD_lchdir)
	scope = CDSCOPE_ਖਿੜਕੀ;
    ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_tcd || eap->cmdidx == CMD_tchdir)
	scope = CDSCOPE_TABPAGE;

    ਜੇ (changedir_func(new_dir, eap->forceit, scope))
    {
	// Echo the new current directory if the command was typed.
	ਜੇ (ਕੁੰਜੀ_Typed || p_verbose >= 5)
	    ex_pwd(eap);
    }
}

/*
 * ":pwd".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_pwd(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਜੇ (mch_dirname(NameBuff, MAXPATHL) == OK)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	slash_adjust(NameBuff);
#ਜੇ_ਅੰਤ
	ਜੇ (p_verbose > 0)
	{
	    ਅੱਖਰ *context = "global";

	    ਜੇ (ਆਖਰੀ_chdir_reason != ਨਿਰਾ)
		context = ਆਖਰੀ_chdir_reason;
	    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_localdir != ਨਿਰਾ)
		context = "window";
	    ਨਹੀਂਤਾਂ ਜੇ (curtab->tp_localdir != ਨਿਰਾ)
		context = "tabpage";
	    smsg("[%s] %s", context, (ਅੱਖਰ *)NameBuff);
	}
	ਨਹੀਂਤਾਂ
	    ਸੁਨੇਹਾ((ਅੱਖਰ *)NameBuff);
    }
    ਨਹੀਂਤਾਂ
	emsg(_(e_directory_unknown));
}

/*
 * ":=".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_equal(exarg_T *eap)
{
    smsg("%ld", (ਲੰਮਾ)eap->line2);
    ex_may_print(eap);
}

    ਸਥਿਰ ਖਾਲੀ
ex_sleep(exarg_T *eap)
{
    ਪੂਰਨ		n;
    ਲੰਮਾ	len;

    ਜੇ (cursor_valid())
    {
	n = W_WINROW(curwin) + curwin->w_wrow - ਸੁਨੇਹਾ_scrolled;
	ਜੇ (n >= 0)
	    windgoto(n, curwin->w_wincol + curwin->w_wcol);
    }

    len = eap->line2;
    ਚੋਣ (*eap->arg)
    {
	ਹਾਲਤ 'm': ਰੋਕੋ;
	ਹਾਲਤ NUL: len *= 1000L; ਰੋਕੋ;
	ਮੂਲ: semsg(_(e_invalid_ਦਲੀਲ_str), eap->arg); ਵਾਪਸ;
    }

    // Hide the cursor if invoked with !
    do_sleep(len, eap->forceit);
}

/*
 * Sleep for "msec" milliseconds, but keep checking for a CTRL-C every second.
 * Hide the cursor if "hide_cursor" is TRUE.
 */
    ਖਾਲੀ
do_sleep(ਲੰਮਾ msec, ਪੂਰਨ hide_cursor)
{
    ਲੰਮਾ	ਪੂਰਾ = 0;
    ਲੰਮਾ	wait_now;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
    elapsed_T	ਸ਼ੁਰੂ_tv;

    // Remember at what time we started, so that we know how much longer we
    // should wait after waiting for a bit.
    ELAPSED_INIT(ਸ਼ੁਰੂ_tv);
#ਜੇ_ਅੰਤ

    ਜੇ (hide_cursor)
	cursor_sleep();
    ਨਹੀਂਤਾਂ
	cursor_on();

    ਬਾਹਰਲਾ_flush_cursor(FALSE, FALSE);
    ਜਦੋਂ (!got_int && ਪੂਰਾ < msec)
    {
	wait_now = msec - ਪੂਰਾ > 1000L ? 1000L : msec - ਪੂਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
	{
	    ਲੰਮਾ    due_ਸਮਾਂ = check_due_timer();

	    ਜੇ (due_ਸਮਾਂ > 0 && due_ਸਮਾਂ < wait_now)
		wait_now = due_ਸਮਾਂ;
	}
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	ਜੇ (has_any_channel() && wait_now > 20L)
	    wait_now = 20L;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SOUND
	ਜੇ (has_any_sound_callback() && wait_now > 20L)
	    wait_now = 20L;
#ਜੇ_ਅੰਤ
	ui_delay(wait_now, TRUE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	ਜੇ (has_any_channel())
	    ui_breakcheck_force(TRUE);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ui_breakcheck();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸੁਨੇਹਾ_ਕਤਾਰ
	// Process the netbeans and clientserver messages that may have been
	// received in the call to ui_breakcheck() when the GUI is in use. This
	// may occur when running a test case.
	parse_queued_messages();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
	// actual time passed
	ਪੂਰਾ = ELAPSED_FUNC(ਸ਼ੁਰੂ_tv);
#ਨਹੀਂਤਾਂ
	// guesstimate time passed (will actually be more)
	ਪੂਰਾ += wait_now;
#ਜੇ_ਅੰਤ
    }

    // If CTRL-C was typed to interrupt the sleep, drop the CTRL-C from the
    // input buffer, otherwise a following call to input() fails.
    ਜੇ (got_int)
	(ਖਾਲੀ)vpeekc();

    ਜੇ (hide_cursor)
	cursor_unsleep();
}

/*
 * ":winsize" command (obsolete).
 */
    ਸਥਿਰ ਖਾਲੀ
ex_winsize(exarg_T *eap)
{
    ਪੂਰਨ		w, h;
    char_u	*arg = eap->arg;
    char_u	*p;

    ਜੇ (!ਸੁਰੱਖਿਅਤ_isdigit(*arg))
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), arg);
	ਵਾਪਸ;
    }
    w = getdigits(&arg);
    arg = skipwhite(arg);
    p = arg;
    h = getdigits(&arg);
    ਜੇ (*p != NUL && *arg == NUL)
	ਸੈੱਟ_shellsize(w, h, TRUE);
    ਨਹੀਂਤਾਂ
	emsg(_(e_winsize_requires_two_ਸੰਖਿਆ_arguments));
}

    ਸਥਿਰ ਖਾਲੀ
ex_wincmd(exarg_T *eap)
{
    ਪੂਰਨ		xchar = NUL;
    char_u	*p;

    ਜੇ (*eap->arg == 'g' || *eap->arg == Ctrl_G)
    {
	// CTRL-W g and CTRL-W CTRL-G  have an extra command character
	ਜੇ (eap->arg[1] == NUL)
	{
	    emsg(_(e_invalid_ਦਲੀਲ));
	    ਵਾਪਸ;
	}
	xchar = eap->arg[1];
	p = eap->arg + 2;
    }
    ਨਹੀਂਤਾਂ
	p = eap->arg + 1;

    ਸੈੱਟ_nextcmd(eap, p);
    p = skipwhite(p);
    ਜੇ (*p != NUL && *p != (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਵਿੱਚ_vim9script() ? '#' :
#ਜੇ_ਅੰਤ
		'"')
	    && eap->nextcmd == ਨਿਰਾ)
	emsg(_(e_invalid_ਦਲੀਲ));
    ਨਹੀਂਤਾਂ ਜੇ (!eap->skip)
    {
	// Pass flags on for ":vertical wincmd ]".
	postponed_ਵੰਡੋ_flags = cmdmod.cmod_ਵੰਡੋ;
	postponed_ਵੰਡੋ_tab = cmdmod.cmod_tab;
	do_ਖਿੜਕੀ(*eap->arg, eap->addr_ਗਿਣਤੀ > 0 ? eap->line2 : 0L, xchar);
	postponed_ਵੰਡੋ_flags = 0;
	postponed_ਵੰਡੋ_tab = 0;
    }
}

#ਜੇ defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * ":winpos".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_winpos(exarg_T *eap)
{
    ਪੂਰਨ		x, y;
    char_u	*arg = eap->arg;
    char_u	*p;

    ਜੇ (*arg == NUL)
    {
# ਜੇ defined(FEAT_GUI) || defined(MSWIN)
#  ifdef VIMDLL
	ਜੇ (gui.ਵਿੱਚ_use ? gui_mch_get_winpos(&x, &y) != FAIL :
		mch_get_winpos(&x, &y) != FAIL)
#  elif defined(FEAT_GUI)
	ਜੇ (gui.ਵਿੱਚ_use && gui_mch_get_winpos(&x, &y) != FAIL)
#  ਨਹੀਂਤਾਂ
	ਜੇ (mch_get_winpos(&x, &y) != FAIL)
#  endif
	{
	    sprintf((ਅੱਖਰ *)IObuff, _("Window position: X %d, Y %d"), x, y);
	    ਸੁਨੇਹਾ((ਅੱਖਰ *)IObuff);
	}
	ਨਹੀਂਤਾਂ
# endif
	    emsg(_(e_obtaining_ਖਿੜਕੀ_position_not_implemented_for_this_platform));
    }
    ਨਹੀਂਤਾਂ
    {
	x = getdigits(&arg);
	arg = skipwhite(arg);
	p = arg;
	y = getdigits(&arg);
	ਜੇ (*p == NUL || *arg != NUL)
	{
	    emsg(_(e_winpos_requires_two_ਸੰਖਿਆ_arguments));
	    ਵਾਪਸ;
	}
# ifdef FEAT_GUI
	ਜੇ (gui.ਵਿੱਚ_use)
	    gui_mch_ਸੈੱਟ_winpos(x, y);
	ਨਹੀਂਤਾਂ ਜੇ (gui.starting)
	{
	    // Remember the coordinates for when the window is opened.
	    gui_win_x = x;
	    gui_win_y = y;
	}
#  ਜੇ defined(HAVE_TGETENT) || defined(VIMDLL)
	ਨਹੀਂਤਾਂ
#  endif
# endif
# ਜੇ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	    mch_ਸੈੱਟ_winpos(x, y);
# endif
# ifdef HAVE_TGETENT
	ਜੇ (*T_CWP)
	    ਪਦ_ਸੈੱਟ_winpos(x, y);
# endif
    }
}
#ਜੇ_ਅੰਤ

/*
 * Handle command that work like operators: ":delete", ":yank", ":>" and ":<".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_operators(exarg_T *eap)
{
    oparg_T	oa;

    clear_oparg(&oa);
    oa.regname = eap->regname;
    oa.ਸ਼ੁਰੂ.lnum = eap->line1;
    oa.ਸਿਰਾ.lnum = eap->line2;
    oa.ਸਤਰ_ਗਿਣਤੀ = eap->line2 - eap->line1 + 1;
    oa.motion_type = MLINE;
    virtual_op = FALSE;
    ਜੇ (eap->cmdidx != CMD_yank)	// position cursor for undo
    {
	setpcmark();
	curwin->w_cursor.lnum = eap->line1;
	beginline(BL_SOL | BL_FIX);
    }

    ਜੇ (VIsual_ਸਰਗਰਮ)
	ਸਿਰਾ_visual_ਬਹੁਲਕ();

    ਚੋਣ (eap->cmdidx)
    {
	ਹਾਲਤ CMD_delete:
	    oa.op_type = OP_DELETE;
	    op_delete(&oa);
	    ਰੋਕੋ;

	ਹਾਲਤ CMD_yank:
	    oa.op_type = OP_YANK;
	    (ਖਾਲੀ)op_yank(&oa, FALSE, TRUE);
	    ਰੋਕੋ;

	ਮੂਲ:    // CMD_rshift or CMD_lshift
	    ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		(eap->cmdidx == CMD_rshift) ^ curwin->w_p_rl
#ਨਹੀਂਤਾਂ
		eap->cmdidx == CMD_rshift
#ਜੇ_ਅੰਤ
						)
		oa.op_type = OP_RSHIFT;
	    ਨਹੀਂਤਾਂ
		oa.op_type = OP_LSHIFT;
	    op_ਖਿਸਕਾਓ(&oa, FALSE, eap->amount);
	    ਰੋਕੋ;
    }
    virtual_op = MAYBE;
    ex_may_print(eap);
}

/*
 * ":put".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_put(exarg_T *eap)
{
    // ":0put" works like ":1put!".
    ਜੇ (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_ਖਾਨਾ();
    do_put(eap->regname, ਨਿਰਾ, eap->forceit ? BACKWARD : FORWARD, 1L,
						       PUT_ਸਤਰ|PUT_CURSLINE);
}

/*
 * ":iput".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_iput(exarg_T *eap)
{
    // ":0iput" works like ":1iput!".
    ਜੇ (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_ਖਾਨਾ();
    do_put(eap->regname, ਨਿਰਾ, eap->forceit ? BACKWARD : FORWARD, 1L,
						      PUT_ਸਤਰ|PUT_CURSLINE
						      |PUT_FIXINDENT);
}

/*
 * Handle ":copy" and ":move".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_copymove(exarg_T *eap)
{
    ਲੰਮਾ	n;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (not_ਵਿੱਚ_vim9(eap, false) == FAIL)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    n = get_ਪਤਾ(eap, &eap->arg, eap->addr_type, FALSE, FALSE, FALSE, 1);
    ਜੇ (eap->arg == ਨਿਰਾ)	    // error detected
    {
	eap->nextcmd = ਨਿਰਾ;
	ਵਾਪਸ;
    }
    get_flags(eap);

    /*
     * move or copy lines from 'eap->line1'-'eap->line2' to below line 'n'
     */
    ਜੇ (n == MAXLNUM || n < 0 || n > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	emsg(_(e_invalid_range));
	ਵਾਪਸ;
    }

    ਜੇ (eap->cmdidx == CMD_move)
    {
	ਜੇ (do_move(eap->line1, eap->line2, n) == FAIL)
	    ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ
	ex_copy(eap->line1, eap->line2, n);
    u_clearline();
    beginline(BL_SOL | BL_FIX);
    ex_may_print(eap);
}

/*
 * Print the current line if flags were given to the Ex command.
 */
    ਖਾਲੀ
ex_may_print(exarg_T *eap)
{
    ਜੇ (eap->flags != 0)
    {
	print_ਸਤਰ(curwin->w_cursor.lnum, (eap->flags & EXFLAG_NR),
						  (eap->flags & EXFLAG_LIST));
	ex_no_reprint = TRUE;
    }
}

/*
 * ":smagic" and ":snomagic".
 */
    ਸਥਿਰ ਖਾਲੀ
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
    ਸਥਿਰ ਖਾਲੀ
ex_join(exarg_T *eap)
{
    curwin->w_cursor.lnum = eap->line1;
    ਜੇ (eap->line1 == eap->line2)
    {
	ਜੇ (eap->addr_ਗਿਣਤੀ >= 2)   // :2,2join does nothing
	    ਵਾਪਸ;
	ਜੇ (eap->line2 == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    beep_flush();
	    ਵਾਪਸ;
	}
	++eap->line2;
    }
    (ਖਾਲੀ)do_join(eap->line2 - eap->line1 + 1, !eap->forceit, TRUE, TRUE, TRUE);
    beginline(BL_WHITE | BL_FIX);
    ex_may_print(eap);
}

/*
 * ":[addr]@r" or ":[addr]*r": execute register
 */
    ਸਥਿਰ ਖਾਲੀ
ex_at(exarg_T *eap)
{
    ਪੂਰਨ		c;
    ਪੂਰਨ		prev_len = typebuf.tb_len;

    curwin->w_cursor.lnum = eap->line2;
    check_cursor_ਖਾਨਾ();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#ਜੇ_ਅੰਤ

    // get the register name.  No name means to use the previous one
    c = *eap->arg;
    ਜੇ (c == NUL || (c == '*' && *eap->cmd == '*'))
	c = '@';
    // Put the register in the typeahead buffer with the "silent" flag.
    ਜੇ (do_execreg(c, TRUE, vim_strchr(p_cpo, CPO_EXECBUF) != ਨਿਰਾ, TRUE)
								      == FAIL)
    {
	beep_flush();
	ਵਾਪਸ;
    }

    ਪੂਰਨ	save_efr = exec_from_reg;

    exec_from_reg = TRUE;

    /*
     * Execute from the typeahead buffer.
     * Continue until the stuff buffer is empty and all added characters
     * have been consumed.
     */
    ਜਦੋਂ (!stuff_ਖਾਲੀ_ਥਾਵਾਂ() || typebuf.tb_len > prev_len)
	(ਖਾਲੀ)do_cmdline(ਨਿਰਾ, getexline, ਨਿਰਾ, DOCMD_NOWAIT|DOCMD_VERBOSE);

    exec_from_reg = save_efr;
}

/*
 * ":!".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_bang(exarg_T *eap)
{
    do_bang(eap->addr_ਗਿਣਤੀ, eap, eap->forceit, TRUE, TRUE);
}

/*
 * ":undo".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_undo(exarg_T *eap)
{
    ਜੇ (eap->addr_ਗਿਣਤੀ == 1)	    // :undo 123
	undo_ਸਮਾਂ(eap->line2, FALSE, FALSE, TRUE);
    ਨਹੀਂਤਾਂ
	u_undo(1);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
    ਸਥਿਰ ਖਾਲੀ
ex_wundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_ਮਾਪ];

    u_compute_hash(hash);
    u_write_undo(eap->arg, eap->forceit, curbuf, hash);
}

    ਸਥਿਰ ਖਾਲੀ
ex_rundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_ਮਾਪ];

    u_compute_hash(hash);
    u_read_undo(eap->arg, hash, ਨਿਰਾ);
}
#ਜੇ_ਅੰਤ

/*
 * ":redo".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_redo(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    u_redo(1);
}

/*
 * ":earlier" and ":later".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_later(exarg_T *eap)
{
    ਲੰਮਾ	ਗਿਣਤੀ_2 = 0;
    ਪੂਰਨ		sec = FALSE;
    ਪੂਰਨ		file = FALSE;
    char_u	*p = eap->arg;

    ਜੇ (*p == NUL)
	ਗਿਣਤੀ_2 = 1;
    ਨਹੀਂਤਾਂ ਜੇ (ਸੁਰੱਖਿਅਤ_isdigit(*p))
    {
	ਗਿਣਤੀ_2 = getdigits(&p);
	ਚੋਣ (*p)
	{
	    ਹਾਲਤ 's': ++p; sec = TRUE; ਰੋਕੋ;
	    ਹਾਲਤ 'm': ++p; sec = TRUE; ਗਿਣਤੀ_2 *= 60; ਰੋਕੋ;
	    ਹਾਲਤ 'h': ++p; sec = TRUE; ਗਿਣਤੀ_2 *= 60 * 60; ਰੋਕੋ;
	    ਹਾਲਤ 'd': ++p; sec = TRUE; ਗਿਣਤੀ_2 *= 24 * 60 * 60; ਰੋਕੋ;
	    ਹਾਲਤ 'f': ++p; file = TRUE; ਰੋਕੋ;
	}
    }

    ਜੇ (*p != NUL)
	semsg(_(e_invalid_ਦਲੀਲ_str), eap->arg);
    ਨਹੀਂਤਾਂ
	undo_ਸਮਾਂ(eap->cmdidx == CMD_earlier ? -ਗਿਣਤੀ_2 : ਗਿਣਤੀ_2,
							    sec, file, FALSE);
}

/*
 * ":redir": start/stop redirection.
 */
    ਸਥਿਰ ਖਾਲੀ
ex_redir(exarg_T *eap)
{
    ਅੱਖਰ	*ਬਹੁਲਕ;
    char_u	*fname;
    char_u	*arg = eap->arg;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (redir_execute)
    {
	emsg(_(e_cannot_use_redir_inside_execute));
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (STRICMP(eap->arg, "END") == 0)
	close_redir();
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (*arg == '>')
	{
	    ++arg;
	    ਜੇ (*arg == '>')
	    {
		++arg;
		ਬਹੁਲਕ = "a";
	    }
	    ਨਹੀਂਤਾਂ
		ਬਹੁਲਕ = "w";
	    arg = skipwhite(arg);

	    close_redir();

	    // Expand environment variables and "~/".
	    fname = expand_env_save(arg);
	    ਜੇ (fname == ਨਿਰਾ)
		ਵਾਪਸ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
	    ਜੇ (cmdmod.cmod_flags & CMOD_BROWSE)
	    {
		char_u	*browseFile;

		browseFile = do_browse(BROWSE_SAVE,
			(char_u *)_("Save Redirection"),
			fname, ਨਿਰਾ, ਨਿਰਾ,
			(char_u *)_(BROWSE_FILTER_ALL_FILES), curbuf);
		ਜੇ (browseFile == ਨਿਰਾ)
		    ਵਾਪਸ;		// operation cancelled
		vim_free(fname);
		fname = browseFile;
		eap->forceit = TRUE;	// since dialog already asked
	    }
#ਜੇ_ਅੰਤ

	    redir_fd = open_exfile(fname, eap->forceit, ਬਹੁਲਕ);
	    vim_free(fname);
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਨਹੀਂਤਾਂ ਜੇ (*arg == '@')
	{
	    // redirect to a register a-z (resp. A-Z for appending)
	    close_redir();
	    ++arg;
	    ਜੇ (ASCII_ISALPHA(*arg)
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
		ਜੇ (*arg == '>' && arg[1] == '>')  // append
		    arg += 2;
		ਨਹੀਂਤਾਂ
		{
		    // Can use both "@a" and "@a>".
		    ਜੇ (*arg == '>')
			arg++;
		    // Make register empty when not using @A-@Z and the
		    // command is valid.
		    ਜੇ (*arg == NUL && !ਸੁਰੱਖਿਅਤ_isupper(redir_reg))
			write_reg_contents(redir_reg, (char_u *)"", -1, FALSE);
		}
	    }
	    ਜੇ (*arg != NUL)
	    {
		redir_reg = 0;
		semsg(_(e_invalid_ਦਲੀਲ_str), eap->arg);
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (*arg == '=' && arg[1] == '>')
	{
	    ਪੂਰਨ append;

	    // redirect to a variable
	    close_redir();
	    arg += 2;

	    ਜੇ (*arg == '>')
	    {
		++arg;
		append = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
		append = FALSE;

	    ਜੇ (var_redir_ਸ਼ੁਰੂ(skipwhite(arg), append) == OK)
		redir_vname = 1;
	}
#ਜੇ_ਅੰਤ

	// TODO: redirect to a buffer

	ਨਹੀਂਤਾਂ
	    semsg(_(e_invalid_ਦਲੀਲ_str), eap->arg);
    }

    // Make sure redirection is not off.  Can happen for cmdline completion
    // that indirectly invokes a command to catch its output.
    ਜੇ (redir_fd != ਨਿਰਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
			  || redir_reg || redir_vname
#ਜੇ_ਅੰਤ
							)
	redir_off = FALSE;
}

/*
 * ":redraw": force redraw, with clear for ":redraw!".
 */
    ਖਾਲੀ
ex_redraw(exarg_T *eap)
{
    redraw_cmd(eap->forceit);
}

/*
 * ":redraw": force redraw, with clear if "clear" is TRUE.
 */
    ਖਾਲੀ
redraw_cmd(ਪੂਰਨ clear)
{
    ਪੂਰਨ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ਪੂਰਨ save_p_lz = p_lz;
    p_lz = FALSE;

    validate_cursor();
    update_topline();
    update_screen(clear ? UPD_CLEAR : VIsual_ਸਰਗਰਮ ? UPD_INVERTED : 0);
    ਜੇ ((State & ਬਹੁਲਕ_CMDLINE) == 0)
	setcursor(); // put cursor back where it belongs
    ਜੇ (need_maketitle)
	maketitle();
#ਜੇ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
    ਜੇ (!gui.ਵਿੱਚ_use)
# endif
	resize_console_buf();
#ਜੇ_ਅੰਤ
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;

    // After drawing the statusline screen_attr may still be set.
    screen_stop_highlight();

    // Reset msg_didout, so that a message that's there is overwritten.
    ਸੁਨੇਹਾ_didout = FALSE;
    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;

    // No need to wait after an intentional redraw.
    need_wait_return = FALSE;

    // When invoked from a callback or autocmd the command line may be active.
    ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
	redrawcmdline();

    ਬਾਹਰਲਾ_flush();
}

/*
 * ":redrawstatus": force redraw of status line(s)
 */
    ਸਥਿਰ ਖਾਲੀ
ex_redrawstatus(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਜੇ (eap->forceit)
	ਸਥਿਤੀ_redraw_all();
    ਨਹੀਂਤਾਂ
	ਸਥਿਤੀ_redraw_curbuf();
    redraw_vseps = TRUE;
    ਜੇ (ਸੁਨੇਹਾ_scrolled && (State & ਬਹੁਲਕ_CMDLINE))
	ਵਾਪਸ;  // redraw later

    ਪੂਰਨ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ਪੂਰਨ save_p_lz = p_lz;
    p_lz = FALSE;

    ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
	redraw_statuslines();
    ਨਹੀਂਤਾਂ
	update_screen(VIsual_ਸਰਗਰਮ ? UPD_INVERTED : 0);
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    ਬਾਹਰਲਾ_flush();
}

/*
 * ":redrawtabline": force redraw of the tabline
 */
    ਸਥਿਰ ਖਾਲੀ
ex_redrawtabline(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ਪੂਰਨ save_p_lz = p_lz;
    p_lz = FALSE;

    draw_tabline();

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    ਬਾਹਰਲਾ_flush();
}

/*
 * ":redrawtabpanel": force redraw of the tabpanel
 */
    ਸਥਿਰ ਖਾਲੀ
ex_redrawtabpanel(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ਪੂਰਨ save_p_lz = p_lz;
    p_lz = FALSE;

#ਜੇ defined(FEAT_TABPANEL)
    draw_tabpanel();
#ਨਹੀਂਤਾਂ
    emsg(_(e_cannot_not_support_redrawtabpanel));
#ਜੇ_ਅੰਤ

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    ਬਾਹਰਲਾ_flush();
}

    ਸਥਿਰ ਖਾਲੀ
close_redir(ਖਾਲੀ)
{
    ਜੇ (redir_fd != ਨਿਰਾ)
    {
	fclose(redir_fd);
	redir_fd = ਨਿਰਾ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    redir_reg = 0;
    ਜੇ (redir_vname)
    {
	var_redir_stop();
	redir_vname = 0;
    }
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_SESSION) || defined(FEAT_EVAL)
    ਪੂਰਨ
vim_mkdir_emsg(char_u *name, ਪੂਰਨ prot ਅਣਵਰਤਿਆ)
{
    ਜੇ (vim_mkdir(name, prot) != 0)
    {
	semsg(_(e_cannot_create_directory_str), name);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

/*
 * Open a file for writing for an Ex command, with some checks.
 * Return file descriptor, or NULL on failure.
 */
    FILE *
open_exfile(
    char_u	*fname,
    ਪੂਰਨ		forceit,
    ਅੱਖਰ	*ਬਹੁਲਕ)	    // "w" for create new file or "a" for append
{
    FILE	*fd;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    // with Unix it is possible to open a directory
    ਜੇ (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	ਵਾਪਸ ਨਿਰਾ;
    }
#ਜੇ_ਅੰਤ
    ਜੇ (!forceit && *ਬਹੁਲਕ != 'a' && vim_fexists(fname))
    {
	semsg(_(e_str_exists_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_override), fname);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ ((fd = mch_fopen((ਅੱਖਰ *)fname, ਬਹੁਲਕ)) == ਨਿਰਾ)
	semsg(_(e_cannot_open_str_for_writing_2), fname);

    ਵਾਪਸ fd;
}

/*
 * ":mark" and ":k".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_mark(exarg_T *eap)
{
    pos_T	pos;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (not_ਵਿੱਚ_vim9(eap, false) == FAIL)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    ਜੇ (*eap->arg == NUL)		// No argument?
    {
	emsg(_(e_ਦਲੀਲ_required));
	ਵਾਪਸ;
    }

    ਜੇ (eap->arg[1] != NUL)	// more than one character?
    {
	semsg(_(e_trailing_characters_str), eap->arg);
	ਵਾਪਸ;
    }

    pos = curwin->w_cursor;		// save curwin->w_cursor
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_WHITE | BL_FIX);
    ਜੇ (setmark(*eap->arg) == FAIL)	// set mark
	emsg(_(e_ਦਲੀਲ_must_be_letter_or_forward_backward_quote));
    curwin->w_cursor = pos;		// restore curwin->w_cursor
}

/*
 * Update w_topline, w_leftcol and the cursor position.
 */
    ਖਾਲੀ
update_topline_cursor(ਖਾਲੀ)
{
    check_cursor();		// put cursor on valid line
    update_topline();
    ਜੇ (!curwin->w_p_wrap)
	validate_cursor();
    update_curswant();
}

/*
 * Save the current State and go to Normal mode.
 * Return TRUE if the typeahead could be saved.
 */
    ਪੂਰਨ
save_current_state(save_state_T *sst)
{
    sst->save_ਸੁਨੇਹਾ_scroll = ਸੁਨੇਹਾ_scroll;
    sst->save_restart_edit = restart_edit;
    sst->save_ਸੁਨੇਹਾ_didout = ਸੁਨੇਹਾ_didout;
    sst->save_State = State;
    sst->save_insertmode = p_im;
    sst->save_ਅੰਤ_op = ਅੰਤ_op;
    sst->save_opcount = opcount;
    sst->save_reg_executing = reg_executing;
    sst->save_pending_ਸਿਰਾ_reg_executing = pending_ਸਿਰਾ_reg_executing;

    ਸੁਨੇਹਾ_scroll = FALSE;		    // no msg scrolling in Normal mode
    restart_edit = 0;		    // don't go to Insert mode
    p_im = FALSE;		    // don't use 'insertmode'

    sst->save_script_ਸੰਸਕਰਣ = current_sctx.sc_ਸੰਸਕਰਣ;
    current_sctx.sc_ਸੰਸਕਰਣ = 1;    // not in Vim9 script

    /*
     * Save the current typeahead.  This is required to allow using ":normal"
     * from an event handler and makes sure we don't hang when the argument
     * ends with half a command.
     */
    save_typeahead(&sst->tabuf);
    ਵਾਪਸ sst->tabuf.typebuf_valid;
}

    ਖਾਲੀ
restore_current_state(save_state_T *sst)
{
    // Restore the previous typeahead.
    restore_typeahead(&sst->tabuf, FALSE);

    ਸੁਨੇਹਾ_scroll = sst->save_ਸੁਨੇਹਾ_scroll;
    restart_edit = sst->save_restart_edit;
    p_im = sst->save_insertmode;
    ਅੰਤ_op = sst->save_ਅੰਤ_op;
    opcount = sst->save_opcount;
    reg_executing = sst->save_reg_executing;
    pending_ਸਿਰਾ_reg_executing = sst->save_pending_ਸਿਰਾ_reg_executing;
    ਸੁਨੇਹਾ_didout |= sst->save_ਸੁਨੇਹਾ_didout;	// don't reset msg_didout now
    current_sctx.sc_ਸੰਸਕਰਣ = sst->save_script_ਸੰਸਕਰਣ;

    // Restore the state (needed when called from a function executed for
    // 'indentexpr'). Update the mouse and cursor, they may have changed.
    State = sst->save_State;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#ਜੇ_ਅੰਤ
}

/*
 * ":normal[!] {commands}": Execute normal mode commands.
 */
    ਖਾਲੀ
ex_ਸਾਧਾਰਨ(exarg_T *eap)
{
    save_state_T save_state;
    char_u	*arg = ਨਿਰਾ;
    ਪੂਰਨ		l;
    char_u	*p;

    ਜੇ (ex_ਸਾਧਾਰਨ_ਤਾਲਾ > 0)
    {
	emsg(_(e_not_allowed_ਇੱਥੇ));
	ਵਾਪਸ;
    }
    ਜੇ (ex_ਸਾਧਾਰਨ_busy >= p_mmd)
    {
	emsg(_(e_recursive_use_ਦਾ_ਸਾਧਾਰਨ_too_deep));
	ਵਾਪਸ;
    }

    /*
     * vgetc() expects a CSI and K_SPECIAL to have been escaped.  Don't do
     * this for the K_SPECIAL leading byte, otherwise special keys will not
     * work.
     */
    ਜੇ (has_mbyte)
    {
	ਪੂਰਨ	len = 0;

	// Count the number of characters to be escaped.
	ਲਈ (p = eap->arg; *p != NUL; ++p)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    ਜੇ (*p == CSI)  // leadbyte CSI
		len += 2;
#ਜੇ_ਅੰਤ
	    ਲਈ (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		ਜੇ (*++p == K_SPECIAL	  // trailbyte K_SPECIAL or CSI
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
			|| *p == CSI
#ਜੇ_ਅੰਤ
			)
		    len += 2;
	}
	ਜੇ (len > 0)
	{
	    arg = alloc(STRLEN(eap->arg) + len + 1);
	    ਜੇ (arg != ਨਿਰਾ)
	    {
		len = 0;
		ਲਈ (p = eap->arg; *p != NUL; ++p)
		{
		    arg[len++] = *p;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		    ਜੇ (*p == CSI)
		    {
			arg[len++] = KS_EXTRA;
			arg[len++] = (ਪੂਰਨ)KE_CSI;
		    }
#ਜੇ_ਅੰਤ
		    ਲਈ (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		    {
			arg[len++] = *++p;
			ਜੇ (*p == K_SPECIAL)
			{
			    arg[len++] = KS_SPECIAL;
			    arg[len++] = KE_FILLER;
			}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
			ਨਹੀਂਤਾਂ ਜੇ (*p == CSI)
			{
			    arg[len++] = KS_EXTRA;
			    arg[len++] = (ਪੂਰਨ)KE_CSI;
			}
#ਜੇ_ਅੰਤ
		    }
		    arg[len] = NUL;
		}
	    }
	}
    }

    ++ex_ਸਾਧਾਰਨ_busy;
    ਜੇ (save_current_state(&save_state))
    {
	/*
	 * Repeat the :normal command for each line in the range.  When no
	 * range given, execute it just once, without positioning the cursor
	 * first.
	 */
	ਕਰੋ
	{
	    ਜੇ (eap->addr_ਗਿਣਤੀ != 0)
	    {
		curwin->w_cursor.lnum = eap->line1++;
		curwin->w_cursor.ਖਾਨਾ = 0;
		check_cursor_moved(curwin);
	    }

	    exec_ਸਾਧਾਰਨ_cmd(arg != ਨਿਰਾ
		     ? arg
		     : eap->arg, eap->forceit ? REMAP_NONE : REMAP_YES, FALSE);
	}
	ਜਦੋਂ (eap->addr_ਗਿਣਤੀ > 0 && eap->line1 <= eap->line2 && !got_int);
    }

    // Might not return to the main loop when in an event handler.
    update_topline_cursor();

    restore_current_state(&save_state);
    --ex_ਸਾਧਾਰਨ_busy;
    setmouse();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#ਜੇ_ਅੰਤ

    vim_free(arg);
}

/*
 * ":startinsert", ":startreplace" and ":startgreplace"
 */
    ਸਥਿਰ ਖਾਲੀ
ex_startinsert(exarg_T *eap)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    // Ignore this when running in an active terminal.
    ਜੇ (ਪਦ_ਕੰਮ_running(curbuf->b_ਪਦ))
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    ਜੇ (!curbuf->b_p_ma && !p_im)
    {
	// Only give this error when 'insertmode' is off.
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ਵਾਪਸ;
    }
    ਜੇ (eap->forceit)
    {
	// cursor line can be zero on startup
	ਜੇ (!curwin->w_cursor.lnum)
	    curwin->w_cursor.lnum = 1;
	ਸੈੱਟ_cursor_for_append_ਮੰਜ਼ਿਲ_ਸਤਰ();
    }

    // Ignore the command when already in Insert mode.  Inserting an
    // expression register that invokes a function can do this.
    ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ)
	ਵਾਪਸ;

    ਜੇ (eap->cmdidx == CMD_startinsert)
	restart_edit = 'a';
    ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_startreplace)
	restart_edit = 'R';
    ਨਹੀਂਤਾਂ
	restart_edit = 'V';

    ਜੇ (!eap->forceit)
    {
	ਜੇ (eap->cmdidx == CMD_startinsert)
	    restart_edit = 'i';
	curwin->w_curswant = 0;	    // avoid MAXCOL
    }

    ਜੇ (VIsual_ਸਰਗਰਮ)
	showmode();
}

/*
 * ":stopinsert"
 */
    ਸਥਿਰ ਖਾਲੀ
ex_stopinsert(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    restart_edit = 0;
    stop_ਪਾਓ_ਬਹੁਲਕ = TRUE;
#ਜੇ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // when called from remote_expr in insert mode, make sure insert mode is
    // ended by adding K_NOP to the typeahead buffer
    ਜੇ (vgetc_busy)
	ins_char_typebuf(K_NOP, 0);
#ਜੇ_ਅੰਤ
    clearmode();
}

/*
 * Execute normal mode command "cmd".
 * "remap" can be REMAP_NONE or REMAP_YES.
 */
    ਖਾਲੀ
exec_ਸਾਧਾਰਨ_cmd(char_u *cmd, ਪੂਰਨ remap, ਪੂਰਨ silent)
{
    // Stuff the argument into the typeahead buffer.
    ins_typebuf(cmd, remap, 0, TRUE, silent);
    exec_ਸਾਧਾਰਨ(FALSE, FALSE, FALSE);
}

/*
 * Execute normal_cmd() until there is no typeahead left.
 * When "use_vpeekc" is TRUE use vpeekc() to check for available chars.
 */
    ਖਾਲੀ
exec_ਸਾਧਾਰਨ(ਪੂਰਨ was_typed, ਪੂਰਨ use_vpeekc, ਪੂਰਨ may_use_terminal_loop ਅਣਵਰਤਿਆ)
{
    oparg_T	oa;
    ਪੂਰਨ		c;

    // When calling vpeekc() from feedkeys() it will return Ctrl_C when there
    // is nothing to get, so also check for Ctrl_C.
    clear_oparg(&oa);
    ਅੰਤ_op = FALSE;
    ਜਦੋਂ ((!stuff_ਖਾਲੀ_ਥਾਵਾਂ()
		|| ((was_typed || !typebuf_typed()) && typebuf.tb_len > 0)
		|| (use_vpeekc && (c = vpeekc()) != NUL && c != Ctrl_C))
	    && !got_int)
    {
	update_topline_cursor();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	ਜੇ (may_use_terminal_loop && ਪਦ_use_loop()
		&& oa.op_type == OP_NOP && oa.regname == NUL
		&& !VIsual_ਸਰਗਰਮ)
	{
	    // If terminal_loop() returns OK we got a key that is handled
	    // in Normal model.  With FAIL we first need to position the
	    // cursor and the screen needs to be redrawn.
	    ਜੇ (terminal_loop(TRUE) == OK)
		ਸਾਧਾਰਨ_cmd(&oa, TRUE);
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    // execute a Normal mode cmd
	    ਸਾਧਾਰਨ_cmd(&oa, TRUE);
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
    ਸਥਿਰ ਖਾਲੀ
ex_checkpath(exarg_T *eap)
{
    ਲੱਭੋ_ਨਮੂਨਾ_ਵਿੱਚ_ਰਸਤਾ(ਨਿਰਾ, 0, 0, FALSE, FALSE, CHECK_ਰਸਤਾ, 1L,
	    eap->forceit ? ACTION_SHOW_ALL : ACTION_SHOW,
	    (linenr_T)1, (linenr_T)MAXLNUM, eap->forceit, FALSE);
}

# ਜੇ defined(FEAT_QUICKFIX)
/*
 * ":psearch"
 */
    ਸਥਿਰ ਖਾਲੀ
ex_psearch(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;
    ex_findpat(eap);
    g_do_tagpreview = 0;
}
# endif

    ਸਥਿਰ ਖਾਲੀ
ex_findpat(exarg_T *eap)
{
    ਪੂਰਨ		ਸਾਰਾ = TRUE;
    ਲੰਮਾ	n;
    char_u	*p;
    ਪੂਰਨ		action;

    ਚੋਣ (cmdnames[eap->cmdidx].cmd_name[2])
    {
	ਹਾਲਤ 'e':	// ":psearch", ":isearch" and ":dsearch"
		ਜੇ (cmdnames[eap->cmdidx].cmd_name[0] == 'p')
		    action = ACTION_GOTO;
		ਨਹੀਂਤਾਂ
		    action = ACTION_SHOW;
		ਰੋਕੋ;
	ਹਾਲਤ 'i':	// ":ilist" and ":dlist"
		action = ACTION_SHOW_ALL;
		ਰੋਕੋ;
	ਹਾਲਤ 'u':	// ":ijump" and ":djump"
		action = ACTION_GOTO;
		ਰੋਕੋ;
	ਮੂਲ:	// ":isplit" and ":dsplit"
		action = ACTION_ਵੰਡੋ;
		ਰੋਕੋ;
    }

    n = 1;
    ਜੇ (vim_isdigit(*eap->arg))	// get count
    {
	n = getdigits(&eap->arg);
	eap->arg = skipwhite(eap->arg);
    }
    ਜੇ (*eap->arg == '/')   // Match regexp, not just whole words
    {
	ਸਾਰਾ = FALSE;
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	ਜੇ (*p)
	{
	    *p++ = NUL;
	    p = skipwhite(p);

	    // Check for trailing illegal characters
	    ਜੇ (!ਸਿਰੇ_excmd2(eap->arg, p))
		eap->errmsg = ex_errmsg(e_trailing_characters_str, p);
	    ਨਹੀਂਤਾਂ
		ਸੈੱਟ_nextcmd(eap, p);
	}
    }
    ਜੇ (!eap->skip)
	ਲੱਭੋ_ਨਮੂਨਾ_ਵਿੱਚ_ਰਸਤਾ(eap->arg, 0, (ਪੂਰਨ)STRLEN(eap->arg),
		ਸਾਰਾ, !eap->forceit,
		*eap->cmd == 'd' ? ਲੱਭੋ_DEFINE : ਲੱਭੋ_ANY, n, action,
		eap->line1, eap->line2, eap->forceit, FALSE);
}
#ਜੇ_ਅੰਤ


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
/*
 * ":ptag", ":ptselect", ":ptjump", ":ptnext", etc.
 */
    ਸਥਿਰ ਖਾਲੀ
ex_ptag(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;  // will be reset to 0 in ex_tag_cmd()
    ex_ਟੈਗ_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
}

/*
 * ":pedit"
 */
    ਸਥਿਰ ਖਾਲੀ
ex_pedit(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_ਖਿੜਕੀ();

    // Edit the file.
    do_exedit(eap, ਨਿਰਾ);

    ਪਿੱਠ_ਮੰਜ਼ਿਲ_current_ਖਿੜਕੀ(curwin_save);
}

/*
 * ":pbuffer"
 */
    ਸਥਿਰ ਖਾਲੀ
ex_pbuffer(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_ਖਿੜਕੀ();

    // Go to the buffer.
    do_exbuffer(eap);

    ਪਿੱਠ_ਮੰਜ਼ਿਲ_current_ਖਿੜਕੀ(curwin_save);
}

    ਸਥਿਰ ਖਾਲੀ
prepare_preview_ਖਿੜਕੀ(ਖਾਲੀ)
{
    ਜੇ (ERROR_IF_ANY_POPUP_ਖਿੜਕੀ)
	ਵਾਪਸ;

    // Open the preview window or popup and make it the current window.
    g_do_tagpreview = p_pvh;
    prepare_tagpreview(TRUE, TRUE, FALSE);
}

    ਸਥਿਰ ਖਾਲੀ
ਪਿੱਠ_ਮੰਜ਼ਿਲ_current_ਖਿੜਕੀ(win_T *curwin_save)
{
    ਜੇ (curwin != curwin_save && win_valid(curwin_save))
    {
	// Return cursor to where we were
	validate_cursor();
	redraw_later(UPD_VALID);
	win_enter(curwin_save, TRUE);
    }
# ifdef FEAT_PROP_POPUP
    ਨਹੀਂਤਾਂ ਜੇ (WIN_IS_POPUP(curwin))
    {
	// can't keep focus in popup window
	win_enter(firstwin, TRUE);
    }
# endif
    g_do_tagpreview = 0;
}
#ਜੇ_ਅੰਤ

/*
 * ":stag", ":stselect" and ":stjump".
 */
    ਸਥਿਰ ਖਾਲੀ
ex_stag(exarg_T *eap)
{
    postponed_ਵੰਡੋ = -1;
    postponed_ਵੰਡੋ_flags = cmdmod.cmod_ਵੰਡੋ;
    postponed_ਵੰਡੋ_tab = cmdmod.cmod_tab;
    ex_ਟੈਗ_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
    postponed_ਵੰਡੋ_flags = 0;
    postponed_ਵੰਡੋ_tab = 0;
}

/*
 * ":tag", ":tselect", ":tjump", ":tnext", etc.
 */
    ਸਥਿਰ ਖਾਲੀ
ex_ਟੈਗ(exarg_T *eap)
{
    ex_ਟੈਗ_cmd(eap, cmdnames[eap->cmdidx].cmd_name);
}

    ਸਥਿਰ ਖਾਲੀ
ex_ਟੈਗ_cmd(exarg_T *eap, char_u *name)
{
    ਪੂਰਨ		cmd;

    ਚੋਣ (name[1])
    {
	ਹਾਲਤ 'j': cmd = DT_JUMP;	// ":tjump"
		  ਰੋਕੋ;
	ਹਾਲਤ 's': cmd = DT_SELECT;	// ":tselect"
		  ਰੋਕੋ;
	ਹਾਲਤ 'p': cmd = DT_PREV;	// ":tprevious"
		  ਰੋਕੋ;
	ਹਾਲਤ 'N': cmd = DT_PREV;	// ":tNext"
		  ਰੋਕੋ;
	ਹਾਲਤ 'n': cmd = DT_ਅਗਲਾ;	// ":tnext"
		  ਰੋਕੋ;
	ਹਾਲਤ 'o': cmd = DT_ਕੱਢੋ;		// ":pop"
		  ਰੋਕੋ;
	ਹਾਲਤ 'f':			// ":tfirst"
	ਹਾਲਤ 'r': cmd = DT_ਪਹਿਲਾ;	// ":trewind"
		  ਰੋਕੋ;
	ਹਾਲਤ 'l': cmd = DT_ਆਖਰੀ;	// ":tlast"
		  ਰੋਕੋ;
	ਮੂਲ:			// ":tag"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
		  ਜੇ (p_cst && *eap->arg != NUL)
		  {
		      ex_cstag(eap);
		      ਵਾਪਸ;
		  }
#ਜੇ_ਅੰਤ
		  cmd = DT_ਟੈਗ;
		  ਰੋਕੋ;
    }

    ਜੇ (name[0] == 'l')
    {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
	ex_ni(eap);
	ਵਾਪਸ;
#ਨਹੀਂਤਾਂ
	cmd = DT_LTAG;
#ਜੇ_ਅੰਤ
    }

    do_ਟੈਗ(eap->arg, cmd, eap->addr_ਗਿਣਤੀ > 0 ? (ਪੂਰਨ)eap->line2 : 1,
							  eap->forceit, TRUE);
}

ਗਿਣਤੀ {
    SPEC_PERC = 0,
    SPEC_HASH,
    SPEC_CWORD,	    // cursor word
    SPEC_CCWORD,    // cursor WORD
    SPEC_CEXPR,	    // expr under cursor
    SPEC_CFILE,	    // cursor path name
    SPEC_SFILE,	    // ":so" file name
    SPEC_SLNUM,	    // ":so" file line number
    SPEC_ਢੇਰ,	    // call stack
    SPEC_SCRIPT,    // script file name
    SPEC_AFILE,	    // autocommand file name
    SPEC_ABUF,	    // autocommand buffer number
    SPEC_AMATCH,    // autocommand match name
    SPEC_SFLNUM,    // script file line number
    SPEC_SID	    // script ID: <SNR>123_
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIENTSERVER
    , SPEC_CLIENT
#ਜੇ_ਅੰਤ
};

/*
 * Check "str" for starting with a special cmdline variable.
 * If found return one of the SPEC_ values and set "*usedlen" to the length of
 * the variable.  Otherwise return -1 and "*usedlen" is unchanged.
 */
    ਪੂਰਨ
ਲੱਭੋ_cmdline_var(char_u *src, size_t *usedlen)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    ਸਥਿਰ keyvalue_T spec_str_tab[] = {
	KEYVALUE_ਇੰਦਰਾਜ(SPEC_SID, "SID>"),	    // script ID: <SNR>123_
	KEYVALUE_ਇੰਦਰਾਜ(SPEC_ABUF, "abuf>"),	    // autocommand buffer number
	KEYVALUE_ਇੰਦਰਾਜ(SPEC_AFILE, "afile>"),	    // autocommand file name
	KEYVALUE_ਇੰਦਰਾਜ(SPEC_AMATCH, "amatch>"),	    // autocommand match name
	KEYVALUE_ਇੰਦਰਾਜ(SPEC_CCWORD, "cWORD>"),	    // cursor WORD
	KEYVALUE_ਇੰਦਰਾਜ(SPEC_CEXPR, "cexpr>"),	    // expr under cursor
	KEYVALUE_ਇੰਦਰਾਜ(SPEC_CFILE, "cfile>"),	    // cursor path name
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIENTSERVER
	KEYVALUE_ਇੰਦਰਾਜ(SPEC_CLIENT, "client>"),
#ਜੇ_ਅੰਤ
	KEYVALUE_ਇੰਦਰਾਜ(SPEC_CWORD, "cword>"),	    // cursor word
	KEYVALUE_ਇੰਦਰਾਜ(SPEC_SCRIPT, "script>"),	    // script file name
	KEYVALUE_ਇੰਦਰਾਜ(SPEC_SFILE, "sfile>"),	    // ":so" file name
	KEYVALUE_ਇੰਦਰਾਜ(SPEC_SFLNUM, "sflnum>"),	    // script file line number
	KEYVALUE_ਇੰਦਰਾਜ(SPEC_SLNUM, "slnum>"),	    // ":so" file line number
	KEYVALUE_ਇੰਦਰਾਜ(SPEC_ਢੇਰ, "stack>")	    // call stack
    };
    keyvalue_T ਨਿਸ਼ਾਨਾ;
    keyvalue_T *ਇੰਦਰਾਜ;

    ਚੋਣ (*src)
    {
    ਹਾਲਤ '%':
	*usedlen = 1;
	ਵਾਪਸ SPEC_PERC;

    ਹਾਲਤ '#':
	*usedlen = 1;
	ਵਾਪਸ SPEC_HASH;

    ਹਾਲਤ '<':
	ਨਿਸ਼ਾਨਾ.ਕੁੰਜੀ = 0;
	ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.string = src + 1;	    // skip over '<'
	ਨਿਸ਼ਾਨਾ.ਮੁੱਲ.ਲੰਮਾਈ = 0;	    // not used, see cmp_keyvalue_value_n()

	ਇੰਦਰਾਜ = (keyvalue_T *)bsearch(&ਨਿਸ਼ਾਨਾ, &spec_str_tab,
		ARRAY_ਲੰਮਾਈ(spec_str_tab), ਆਕਾਰ(spec_str_tab[0]),
		cmp_keyvalue_ਮੁੱਲ_n);
	ਜੇ (ਇੰਦਰਾਜ == ਨਿਰਾ)
	    ਵਾਪਸ -1;

	*usedlen = ਇੰਦਰਾਜ->ਮੁੱਲ.ਲੰਮਾਈ + 1;
	ਵਾਪਸ ਇੰਦਰਾਜ->ਕੁੰਜੀ;

    ਮੂਲ:
	ਰੋਕੋ;
    }

    ਵਾਪਸ -1;
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
    ਅੱਖਰ	**errormsg,	// pointer to error message
    ਪੂਰਨ		*escaped,	// return value has escaped white space (can
				// be NULL)
    ਪੂਰਨ		ਖਾਲੀ_ਥਾਵਾਂ_is_error)	// empty result is considered an error
{
    ਪੂਰਨ		i;
    char_u	*s;
    char_u	*ਨਤੀਜਾ = (char_u *)"";
    char_u	*resultbuf = ਨਿਰਾ;
    size_t	resultlen;
    buf_T	*buf;
    ਪੂਰਨ		valid = VALID_ਸਿਰ + VALID_ਰਸਤਾ;    // assume valid result
    ਪੂਰਨ		spec_idx;
    ਪੂਰਨ		tilde_file = FALSE;
    ਪੂਰਨ		skip_mod = FALSE;
    char_u	strbuf[30];

    *errormsg = ਨਿਰਾ;
    ਜੇ (escaped != ਨਿਰਾ)
	*escaped = FALSE;

    /*
     * Check if there is something to do.
     */
    spec_idx = ਲੱਭੋ_cmdline_var(src, usedlen);
    ਜੇ (spec_idx < 0)	// no match
    {
	*usedlen = 1;
	ਵਾਪਸ ਨਿਰਾ;
    }

    /*
     * Skip when preceded with a backslash "\%" and "\#".
     * Note: In "\\%" the % is also not recognized!
     */
    ਜੇ (src > srcstart && src[-1] == '\\')
    {
	*usedlen = 0;
	STRMOVE(src - 1, src);	// remove backslash
	ਵਾਪਸ ਨਿਰਾ;
    }

    /*
     * word or WORD under cursor
     */
    ਜੇ (spec_idx == SPEC_CWORD || spec_idx == SPEC_CCWORD
						     || spec_idx == SPEC_CEXPR)
    {
	resultlen = ਲੱਭੋ_ident_under_cursor(&ਨਤੀਜਾ,
		spec_idx == SPEC_CWORD ? (ਲੱਭੋ_IDENT | ਲੱਭੋ_STRING)
	      : spec_idx == SPEC_CEXPR ? (ਲੱਭੋ_IDENT | ਲੱਭੋ_STRING | ਲੱਭੋ_EVAL)
	      : ਲੱਭੋ_STRING);
	ਜੇ (resultlen == 0)
	{
	    *errormsg = "";
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }

    /*
     * '#': Alternate file name
     * '%': Current file name
     *	    File name under the cursor
     *	    File name for autocommand
     *	and following modifiers
     */
    ਨਹੀਂਤਾਂ
    {
	size_t off = 0;

	ਚੋਣ (spec_idx)
	{
	ਹਾਲਤ SPEC_PERC:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		ਜੇ (!ਵਿੱਚ_vim9script() || src[1] != '%')
#ਜੇ_ਅੰਤ
		{
		    // '%': current file
		    ਜੇ (curbuf->b_fname == ਨਿਰਾ)
		    {
			ਨਤੀਜਾ = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਨਤੀਜਾ = curbuf->b_fname;
			tilde_file = STRCMP(ਨਤੀਜਾ, "~") == 0;
		    }
		    ਰੋਕੋ;
		}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		// "%%" alternate file
		off = 1;
#ਜੇ_ਅੰਤ
		// FALLTHROUGH
	ਹਾਲਤ SPEC_HASH:		// '#' or "#99": alternate file
		ਜੇ (off == 0 ? src[1] == '#' : src[2] == '%')
		{
		    // "##" or "%%%": the argument list
		    ਨਤੀਜਾ = arg_all();
		    resultbuf = ਨਤੀਜਾ;
		    *usedlen = off + 2;
		    ਜੇ (escaped != ਨਿਰਾ)
			*escaped = TRUE;
		    skip_mod = TRUE;
		    ਰੋਕੋ;
		}
		s = src + off + 1;
		ਜੇ (*s == '<')		// "#<99" uses v:oldfiles
		    ++s;
		i = (ਪੂਰਨ)getdigits(&s);
		ਜੇ (s == src + off + 2 && src[off + 1] == '-')
		    // just a minus sign, don't skip over it
		    s--;
		*usedlen = (ਪੂਰਨ)(s - src); // length of what we expand

		ਜੇ (src[off + 1] == '<' && i != 0)
		{
		    ਜੇ (*usedlen < off + 2)
		    {
			// Should we give an error message for #<text?
			*usedlen = off + 1;
			ਵਾਪਸ ਨਿਰਾ;
		    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    ਨਤੀਜਾ = list_ਲੱਭੋ_str(get_vim_var_list(VV_OLDFILES),
								     (ਲੰਮਾ)i);
		    ਜੇ (ਨਤੀਜਾ == ਨਿਰਾ)
		    {
			*errormsg = "";
			ਵਾਪਸ ਨਿਰਾ;
		    }
#ਨਹੀਂਤਾਂ
		    *errormsg = _(e_hashsmall_is_not_ਉਪਲਬਧ_without_the_eval_feature);
		    ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਅੰਤ
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (i == 0 && src[off + 1] == '<' && *usedlen > off + 1)
			*usedlen = off + 1;
		    buf = buflist_findnr(i);
		    ਜੇ (buf == ਨਿਰਾ)
		    {
			*errormsg = _(e_no_alternate_file_name_ਮੰਜ਼ਿਲ_substitute_for_hash);
			ਵਾਪਸ ਨਿਰਾ;
		    }
		    ਜੇ (lnump != ਨਿਰਾ)
			*lnump = ECMD_ਆਖਰੀ;
		    ਜੇ (buf->b_fname == ਨਿਰਾ)
		    {
			ਨਤੀਜਾ = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਨਤੀਜਾ = buf->b_fname;
			tilde_file = STRCMP(ਨਤੀਜਾ, "~") == 0;
		    }
		}
		ਰੋਕੋ;

	ਹਾਲਤ SPEC_CFILE:	// file name under cursor
		ਨਤੀਜਾ = file_name_at_cursor(FNAME_MESS|FNAME_HYP, 1L, ਨਿਰਾ);
		ਜੇ (ਨਤੀਜਾ == ਨਿਰਾ)
		{
		    *errormsg = "";
		    ਵਾਪਸ ਨਿਰਾ;
		}
		resultbuf = ਨਤੀਜਾ;	    // remember allocated string
		ਰੋਕੋ;

	ਹਾਲਤ SPEC_AFILE:	// file name for autocommand
		ਨਤੀਜਾ = autocmd_fname;
		ਜੇ (ਨਤੀਜਾ != ਨਿਰਾ && !autocmd_fname_ਭਰਿਆ)
		{
		    // Still need to turn the fname into a full path.  It is
		    // postponed to avoid a delay when <afile> is not used.
		    autocmd_fname_ਭਰਿਆ = TRUE;
		    ਨਤੀਜਾ = ਭਰਿਆ_Name_save(autocmd_fname, FALSE);
		    vim_free(autocmd_fname);
		    autocmd_fname = ਨਤੀਜਾ;
		}
		ਜੇ (ਨਤੀਜਾ == ਨਿਰਾ)
		{
		    *errormsg = _(e_no_autocommand_file_name_ਮੰਜ਼ਿਲ_substitute_for_afile);
		    ਵਾਪਸ ਨਿਰਾ;
		}
		ਨਤੀਜਾ = shorten_fname1(ਨਤੀਜਾ);
		ਰੋਕੋ;

	ਹਾਲਤ SPEC_ABUF:		// buffer number for autocommand
		ਜੇ (autocmd_bufnr <= 0)
		{
		    *errormsg = _(e_no_autocommand_ਬਫਰ_ਸੰਖਿਆ_ਮੰਜ਼ਿਲ_substitute_for_abuf);
		    ਵਾਪਸ ਨਿਰਾ;
		}
		sprintf((ਅੱਖਰ *)strbuf, "%d", autocmd_bufnr);
		ਨਤੀਜਾ = strbuf;
		ਰੋਕੋ;

	ਹਾਲਤ SPEC_AMATCH:	// match name for autocommand
		ਨਤੀਜਾ = autocmd_ਮੇਲ;
		ਜੇ (ਨਤੀਜਾ == ਨਿਰਾ)
		{
		    *errormsg = _(e_no_autocommand_ਮੇਲ_name_ਮੰਜ਼ਿਲ_substitute_for_amatch);
		    ਵਾਪਸ ਨਿਰਾ;
		}
		ਰੋਕੋ;

	ਹਾਲਤ SPEC_SFILE:	// file name for ":so" command
		ਨਤੀਜਾ = estack_sfile(ESTACK_SFILE);
		ਜੇ (ਨਤੀਜਾ == ਨਿਰਾ)
		{
		    *errormsg = _(e_no_ਸਰੋਤ_file_name_ਮੰਜ਼ਿਲ_substitute_for_sfile);
		    ਵਾਪਸ ਨਿਰਾ;
		}
		resultbuf = ਨਤੀਜਾ;	    // remember allocated string
		ਰੋਕੋ;
	ਹਾਲਤ SPEC_ਢੇਰ:	// call stack
		ਨਤੀਜਾ = estack_sfile(ESTACK_ਢੇਰ);
		ਜੇ (ਨਤੀਜਾ == ਨਿਰਾ)
		{
		    *errormsg = _(e_no_call_ਢੇਰ_ਮੰਜ਼ਿਲ_substitute_for_ਢੇਰ);
		    ਵਾਪਸ ਨਿਰਾ;
		}
		resultbuf = ਨਤੀਜਾ;	    // remember allocated string
		ਰੋਕੋ;
	ਹਾਲਤ SPEC_SCRIPT:	// script file name
		ਨਤੀਜਾ = estack_sfile(ESTACK_SCRIPT);
		ਜੇ (ਨਤੀਜਾ == ਨਿਰਾ)
		{
		    *errormsg = _(e_no_script_file_name_ਮੰਜ਼ਿਲ_substitute_for_script);
		    ਵਾਪਸ ਨਿਰਾ;
		}
		resultbuf = ਨਤੀਜਾ;	    // remember allocated string
		ਰੋਕੋ;

	ਹਾਲਤ SPEC_SLNUM:	// line in file for ":so" command
		ਜੇ (SOURCING_NAME == ਨਿਰਾ || SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_ਸਤਰ_ਸੰਖਿਆ_ਮੰਜ਼ਿਲ_use_for_slnum);
		    ਵਾਪਸ ਨਿਰਾ;
		}
		sprintf((ਅੱਖਰ *)strbuf, "%ld", SOURCING_LNUM);
		ਨਤੀਜਾ = strbuf;
		ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਹਾਲਤ SPEC_SFLNUM:	// line in script file
		ਜੇ (current_sctx.sc_lnum + SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_ਸਤਰ_ਸੰਖਿਆ_ਮੰਜ਼ਿਲ_use_for_sflnum);
		    ਵਾਪਸ ਨਿਰਾ;
		}
		sprintf((ਅੱਖਰ *)strbuf, "%ld",
				 (ਲੰਮਾ)(current_sctx.sc_lnum + SOURCING_LNUM));
		ਨਤੀਜਾ = strbuf;
		ਰੋਕੋ;

	ਹਾਲਤ SPEC_SID:
		ਜੇ (current_sctx.sc_sid <= 0)
		{
		    *errormsg = _(e_using_sid_not_ਵਿੱਚ_script_context);
		    ਵਾਪਸ ਨਿਰਾ;
		}
		sprintf((ਅੱਖਰ *)strbuf, "<SNR>%d_", current_sctx.sc_sid);
		ਨਤੀਜਾ = strbuf;
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIENTSERVER
	ਹਾਲਤ SPEC_CLIENT:	// Source of last submitted input
# ifdef MSWIN
		sprintf((ਅੱਖਰ *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)client_ਖਿੜਕੀ);
		ਨਤੀਜਾ = strbuf;
# ਨਹੀਂਤਾਂ
#  ifdef FEAT_SOCKETSERVER
		ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_SOCKET)
		{
		    ਜੇ (client_socket == ਨਿਰਾ)
			ਨਤੀਜਾ = (char_u *)"";
		    ਨਹੀਂਤਾਂ
			ਨਤੀਜਾ = client_socket;
		}
#  endif
#  ifdef FEAT_X11
		ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_X11)
		{
		    sprintf((ਅੱਖਰ *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)client_ਖਿੜਕੀ);
		    ਨਤੀਜਾ = strbuf;
		}
#  endif
# endif
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	}

	resultlen = STRLEN(ਨਤੀਜਾ);	// length of new string
	ਜੇ (src[*usedlen] == '<')	// remove the file name extension
	{
	    ++*usedlen;
	    ਜੇ ((s = vim_strrchr(ਨਤੀਜਾ, '.')) != ਨਿਰਾ && s >= gettail(ਨਤੀਜਾ))
		resultlen = s - ਨਤੀਜਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (!skip_mod)
	{
	    valid |= modify_fname(src, tilde_file, usedlen, &ਨਤੀਜਾ, &resultbuf,
								  &resultlen);
	    ਜੇ (ਨਤੀਜਾ == ਨਿਰਾ)
	    {
		*errormsg = "";
		ਵਾਪਸ ਨਿਰਾ;
	    }
	}
    }

    ਜੇ (resultlen == 0 || valid != VALID_ਸਿਰ + VALID_ਰਸਤਾ)
    {
	ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_is_error)
	{
	    ਜੇ (valid != VALID_ਸਿਰ + VALID_ਰਸਤਾ)
		*errormsg = _(e_ਖਾਲੀ_ਥਾਵਾਂ_file_name_for_percent_or_hash_only_ਚੱਲਦਾ_with_ph);
	    ਨਹੀਂਤਾਂ
		*errormsg = _(e_evaluates_ਮੰਜ਼ਿਲ_an_ਖਾਲੀ_ਥਾਵਾਂ_string);
	}
	ਨਤੀਜਾ = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
	ਨਤੀਜਾ = vim_strnsave(ਨਤੀਜਾ, resultlen);
    vim_free(resultbuf);
    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Expand the <sfile> string in "arg".
 *
 * Returns an allocated string, or NULL for any error.
 */
    char_u *
expand_sfile(char_u *arg)
{
    ਅੱਖਰ	*errormsg;
    char_u	*ਨਤੀਜਾ;
    size_t	resultlen;
    char_u	*newres;
    size_t	len;
    char_u	*repl;
    size_t	repllen;
    size_t	srclen;
    char_u	*p;

    resultlen = STRLEN(arg);
    ਨਤੀਜਾ = vim_strnsave(arg, resultlen);
    ਜੇ (ਨਤੀਜਾ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (p = ਨਤੀਜਾ; *p; )
    {
	ਜੇ (STRNCMP(p, "<sfile>", 7) != 0)
	    ++p;
	ਨਹੀਂਤਾਂ
	{
	    // replace "<sfile>" with the sourced file name, and do ":" stuff
	    repl = eval_vars(p, ਨਤੀਜਾ, &srclen, ਨਿਰਾ, &errormsg, ਨਿਰਾ, TRUE);
	    ਜੇ (errormsg != ਨਿਰਾ)
	    {
		ਜੇ (*errormsg)
		    emsg(errormsg);
		vim_free(ਨਤੀਜਾ);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    ਜੇ (repl == ਨਿਰਾ)		// no match (cannot happen)
	    {
		p += srclen;
		ਜਾਰੀ;
	    }
	    repllen = STRLEN(repl);
	    resultlen += repllen - srclen;
	    newres = alloc(resultlen + 1);
	    ਜੇ (newres == ਨਿਰਾ)
	    {
		vim_free(repl);
		vim_free(ਨਤੀਜਾ);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    len = p - ਨਤੀਜਾ;
	    mch_memmove(newres, ਨਤੀਜਾ, len);
	    STRCPY(newres + len, repl);
	    len += repllen;
	    STRCPY(newres + len, p + srclen);
	    vim_free(repl);
	    vim_free(ਨਤੀਜਾ);
	    ਨਤੀਜਾ = newres;
	    p = newres + len;		// continue after the match
	}
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
/*
 * Make a dialog message in "buff[DIALOG_MSG_SIZE]".
 * "format" must contain "%s".
 */
    ਖਾਲੀ
dialog_ਸੁਨੇਹਾ(char_u *buff, ਅੱਖਰ *format, char_u *fname)
{
    ਜੇ (fname == ਨਿਰਾ)
	fname = (char_u *)_("Untitled");
    vim_snprintf((ਅੱਖਰ *)buff, DIALOG_ਸੁਨੇਹਾ_ਮਾਪ, format, fname);
}
#ਜੇ_ਅੰਤ

/*
 * ":behave {mswin,xterm}"
 */
    ਸਥਿਰ ਖਾਲੀ
ex_behave(exarg_T *eap)
{
    ਜੇ (STRCMP(eap->arg, "mswin") == 0)
    {
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"selection",
						 0L, (char_u *)"exclusive", 0);
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"selectmode",
						 0L, (char_u *)"mouse,key", 0);
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"mousemodel",
						     0L, (char_u *)"popup", 0);
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"keymodel",
					  0L, (char_u *)"startsel,stopsel", 0);
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(eap->arg, "xterm") == 0)
    {
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"selection",
						 0L, (char_u *)"inclusive", 0);
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"selectmode", 0L, (char_u *)"", 0);
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"mousemodel",
						    0L, (char_u *)"extend", 0);
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"keymodel", 0L, (char_u *)"", 0);
    }
    ਨਹੀਂਤਾਂ
	semsg(_(e_invalid_ਦਲੀਲ_str), eap->arg);
}

ਸਥਿਰ ਪੂਰਨ filetype_detect = FALSE;
ਸਥਿਰ ਪੂਰਨ filetype_plugin = FALSE;
ਸਥਿਰ ਪੂਰਨ filetype_indent = FALSE;

/*
 * ":filetype [plugin] [indent] {on,off,detect}"
 * on: Load the filetype.vim file to install autocommands for file types.
 * off: Load the ftoff.vim file to remove all autocommands for file types.
 * plugin on: load filetype.vim and ftplugin.vim
 * plugin off: load ftplugof.vim
 * indent on: load filetype.vim and indent.vim
 * indent off: load indoff.vim
 */
    ਸਥਿਰ ਖਾਲੀ
ex_filetype(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ਪੂਰਨ		plugin = FALSE;
    ਪੂਰਨ		indent = FALSE;

    ਜੇ (*eap->arg == NUL)
    {
	// Print current status.
	smsg("filetype detection:%s  plugin:%s  indent:%s",
		filetype_detect ? "ON" : "OFF",
		filetype_plugin ? (filetype_detect ? "ON" : "(on)") : "OFF",
		filetype_indent ? (filetype_detect ? "ON" : "(on)") : "OFF");
	ਵਾਪਸ;
    }

    // Accept "plugin" and "indent" in any order.
    ਲਈ (;;)
    {
	ਜੇ (STRNCMP(arg, "plugin", 6) == 0)
	{
	    plugin = TRUE;
	    arg = skipwhite(arg + 6);
	    ਜਾਰੀ;
	}
	ਜੇ (STRNCMP(arg, "indent", 6) == 0)
	{
	    indent = TRUE;
	    arg = skipwhite(arg + 6);
	    ਜਾਰੀ;
	}
	ਰੋਕੋ;
    }
    ਜੇ (STRCMP(arg, "on") == 0 || STRCMP(arg, "detect") == 0)
    {
	ਜੇ (*arg == 'o' || !filetype_detect)
	{
	    ਸਰੋਤ_runtime((char_u *)FILETYPE_FILE, DIP_ALL);
	    filetype_detect = TRUE;
	    ਜੇ (plugin)
	    {
		ਸਰੋਤ_runtime((char_u *)FTPLUGIN_FILE, DIP_ALL);
		filetype_plugin = TRUE;
	    }
	    ਜੇ (indent)
	    {
		ਸਰੋਤ_runtime((char_u *)INDENT_FILE, DIP_ALL);
		filetype_indent = TRUE;
	    }
	}
	ਜੇ (*arg == 'd')
	{
	    (ਖਾਲੀ)do_doautocmd((char_u *)"filetypedetect BufRead", TRUE, ਨਿਰਾ);
	    do_modelines(0);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(arg, "off") == 0)
    {
	ਜੇ (plugin || indent)
	{
	    ਜੇ (plugin)
	    {
		ਸਰੋਤ_runtime((char_u *)FTPLUGOF_FILE, DIP_ALL);
		filetype_plugin = FALSE;
	    }
	    ਜੇ (indent)
	    {
		ਸਰੋਤ_runtime((char_u *)INDOFF_FILE, DIP_ALL);
		filetype_indent = FALSE;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਸਰੋਤ_runtime((char_u *)FTOFF_FILE, DIP_ALL);
	    filetype_detect = FALSE;
	}
    }
    ਨਹੀਂਤਾਂ
	semsg(_(e_invalid_ਦਲੀਲ_str), arg);
}

/*
 * ":setfiletype [FALLBACK] {name}"
 */
    ਸਥਿਰ ਖਾਲੀ
ex_setfiletype(exarg_T *eap)
{
    ਜੇ (curbuf->b_did_filetype)
	ਵਾਪਸ;

    char_u *arg = eap->arg;
    ਜੇ (STRNCMP(arg, "FALLBACK ", 9) == 0)
	arg += 9;

    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"filetype", 0L, arg, OPT_LOCAL);
    ਜੇ (arg != eap->arg)
	curbuf->b_did_filetype = false;
}

    ਸਥਿਰ ਖਾਲੀ
ex_digraphs(exarg_T *eap ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIGRAPHS
    ਜੇ (*eap->arg != NUL)
	putdigraph(eap->arg);
    ਨਹੀਂਤਾਂ
	listdigraphs(eap->forceit);
#ਨਹੀਂਤਾਂ
    emsg(_(e_no_digraphs_ਸੰਸਕਰਣ));
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_ਖੋਜ_EXTRA)
    ਖਾਲੀ
ਸੈੱਟ_no_hlsearch(ਪੂਰਨ flag)
{
    no_hlsearch = flag;
# ifdef FEAT_EVAL
    ਸੈੱਟ_vim_var_nr(VV_HLSEARCH, !no_hlsearch && p_hls);
# endif
}

/*
 * ":nohlsearch"
 */
    ਸਥਿਰ ਖਾਲੀ
ex_nohlsearch(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਸੈੱਟ_no_hlsearch(TRUE);
    redraw_all_later(UPD_SOME_VALID);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
/*
 * ":X": Get crypt key
 */
    ਸਥਿਰ ਖਾਲੀ
ex_X(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    crypt_check_current_ਵਿਧੀ();
    (ਖਾਲੀ)crypt_get_ਕੁੰਜੀ(TRUE, TRUE);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਸਥਿਰ ਖਾਲੀ
ex_fold(exarg_T *eap)
{
    ਜੇ (foldManualAllowed(TRUE))
	foldCreate(eap->line1, eap->line2);
}

    ਸਥਿਰ ਖਾਲੀ
ex_foldopen(exarg_T *eap)
{
    opFoldRange(eap->line1, eap->line2, eap->cmdidx == CMD_foldopen,
							 eap->forceit, FALSE);
}

    ਸਥਿਰ ਖਾਲੀ
ex_folddo(exarg_T *eap)
{
    linenr_T	lnum;

# ifdef FEAT_CLIPBOARD
    ਸ਼ੁਰੂ_global_changes();
# endif
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
# endif

    // First set the marks for all lines closed/open.
    ਲਈ (lnum = eap->line1; lnum <= eap->line2; ++lnum)
	ਜੇ (hasFolding(lnum, ਨਿਰਾ, ਨਿਰਾ) == (eap->cmdidx == CMD_folddoclosed))
	    ml_setmarked(lnum);

    // Execute the command on the marked lines.
    global_exe(eap->arg);
    ml_clearmarked();	   // clear rest of the marks
# ifdef FEAT_CLIPBOARD
    ਸਿਰਾ_global_changes();
# endif
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_QUICKFIX)
/*
 * Returns TRUE if the supplied Ex cmdidx is for a location list command
 * instead of a quickfix command.
 */
    ਪੂਰਨ
is_loclist_cmd(ਪੂਰਨ cmdidx)
{
    ਜੇ (cmdidx < 0 || cmdidx >= CMD_ਮਾਪ)
	ਵਾਪਸ FALSE;
    ਵਾਪਸ cmdnames[cmdidx].cmd_name[0] == 'l';
}
#ਜੇ_ਅੰਤ

    ਪੂਰਨ
get_pressedreturn(ਖਾਲੀ)
{
    ਵਾਪਸ ex_pressedreturn;
}

    ਖਾਲੀ
ਸੈੱਟ_pressedreturn(ਪੂਰਨ val)
{
    ex_pressedreturn = val;
}
