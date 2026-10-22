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

#ಅನ್ತರ್ಭಾವ "vim.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	quitmore = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ex_pressedreturn = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_PRINTER
# define ex_hardcopy	ex_ni
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ char_u	*do_one_cmd(char_u **, ಪೂರ್ಣಾಂಕ, cstack_T *, char_u *(*fgetline)(ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, getline_opt_T), ಶೂನ್ಯ *cookie);
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ char_u	*do_one_cmd(char_u **, ಪೂರ್ಣಾಂಕ, char_u *(*fgetline)(ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, getline_opt_T), ಶೂನ್ಯ *cookie);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	if_level = 0;		// depth in :if
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ	append_command(char_u *cmd);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_MENU
# define ex_emenu		ex_ni
# define ex_menu		ex_ni
# define ex_menutranslate	ex_ni
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ	ex_autocmd(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_doautocmd(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_bunload(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_ಬಫರ್(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	do_exbuffer(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_bmodified(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_bnext(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_bprevious(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_brewind(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_blast(exarg_T *eap);
ಸ್ಥಿರ char_u	*getargcmd(char_u **);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	getargopt(exarg_T *eap);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_QUICKFIX
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
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(FEAT_QUICKFIX) || !defined(FEAT_EVAL)
# define ex_cexpr		ex_ni
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ linenr_T default_ವಿಳಾಸ(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ ವಿಳಾಸ_default_all(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	get_flags(exarg_T *eap);
#ಆದರೆ !defined(FEAT_PERL) \
	|| !defined(FEAT_PYTHON) || !defined(FEAT_PYTHON3) \
	|| !defined(FEAT_TCL) \
	|| !defined(FEAT_RUBY) \
	|| !defined(FEAT_LUA) \
	|| !defined(FEAT_MZSCHEME)
# define HAVE_EX_SCRIPT_NI
ಸ್ಥಿರ ಶೂನ್ಯ	ex_script_ni(exarg_T *eap);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಅಕ್ಷರ	*invalid_range(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	correct_range(exarg_T *eap);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
ಸ್ಥಿರ char_u	*replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ char_u	*repl_cmdline(exarg_T *eap, char_u *src, size_t srclen, char_u *repl, char_u **cmdlinep);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_highlight(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_colorscheme(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_cquit(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_quit_all(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_close(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_win_close(ಪೂರ್ಣಾಂಕ forceit, win_T *win, tabpage_T *tp);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_only(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_resize(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_stag(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_tabclose(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_tabonly(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_tabnext(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_tabmove(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_tabs(exarg_T *eap);
#ಆದರೆ defined(FEAT_QUICKFIX)
ಸ್ಥಿರ ಶೂನ್ಯ	ex_pclose(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_ptag(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_pedit(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_pbuffer(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	prepare_preview_ಕಿಟಕಿ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ	ಹಿಂಭಾಗ_ಗಮ್ಯ_current_ಕಿಟಕಿ(win_T *curwin_save);
#ಇಲ್ಲದಿದ್ದರೆ
# define ex_pclose		ex_ni
# define ex_ptag		ex_ni
# define ex_pedit		ex_ni
# define ex_pbuffer		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ	ex_hide(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_exit(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_print(exarg_T *eap);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BYTEOFF
ಸ್ಥಿರ ಶೂನ್ಯ	ex_goto(exarg_T *eap);
#ಇಲ್ಲದಿದ್ದರೆ
# define ex_goto		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ	ex_shell(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_preserve(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_recover(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_ಬಹುಲಕ(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_wrongmodifier(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_ಕಂಡುಹಿಡಿ(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_open(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_edit(exarg_T *eap);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_GUI
# define ex_gui			ex_nogui
ಸ್ಥಿರ ಶೂನ್ಯ	ex_nogui(exarg_T *eap);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
ಸ್ಥಿರ ಶೂನ್ಯ	ex_tearoff(exarg_T *eap);
#ಇಲ್ಲದಿದ್ದರೆ
# define ex_tearoff		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_ಪದ_POPUP_MENU)) && defined(FEAT_MENU)
ಸ್ಥಿರ ಶೂನ್ಯ	ex_popup(exarg_T *eap);
#ಇಲ್ಲದಿದ್ದರೆ
# define ex_popup		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_GUI_MSWIN
# define ex_simalt		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(FEAT_GUI_MSWIN) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
# define gui_mch_ಕಂಡುಹಿಡಿ_dialog	ex_ni
# define gui_mch_replace_dialog ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(FEAT_GUI_GTK)
# define ex_helpfind		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_CSCOPE
# define ex_cscope		ex_ni
# define ex_scscope		ex_ni
# define ex_cstag		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_SYN_HL
# define ex_syntax		ex_ni
# define ex_ownsyntax		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(FEAT_SYN_HL) || !defined(FEAT_PROFILE)
# define ex_syntime		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_SPELL
# define ex_spell		ex_ni
# define ex_mkspell		ex_ni
# define ex_spelldump		ex_ni
# define ex_spellinfo		ex_ni
# define ex_spellrepall		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_PERSISTENT_UNDO
# define ex_rundo		ex_ni
# define ex_wundo		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_LUA
# define ex_lua			ex_script_ni
# define ex_luado		ex_ni
# define ex_luafile		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_MZSCHEME
# define ex_mzscheme		ex_script_ni
# define ex_mzfile		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_PERL
# define ex_perl		ex_script_ni
# define ex_perldo		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_PYTHON
# define ex_python		ex_script_ni
# define ex_pydo		ex_ni
# define ex_pyfile		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_PYTHON3
# define ex_py3			ex_script_ni
# define ex_py3do		ex_ni
# define ex_py3file		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(FEAT_PYTHON) && !defined(FEAT_PYTHON3)
# define ex_pyx			ex_script_ni
# define ex_pyxdo		ex_ni
# define ex_pyxfile		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_TCL
# define ex_tcl			ex_script_ni
# define ex_tcldo		ex_ni
# define ex_tclfile		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_RUBY
# define ex_ruby		ex_script_ni
# define ex_rubydo		ex_ni
# define ex_rubyfile		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_KEYMAP
# define ex_loadkeymap		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ	ex_swapname(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_syncbind(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_read(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_pwd(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_equal(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_sleep(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_winsize(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_wincmd(exarg_T *eap);
#ಆದರೆ defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
ಸ್ಥಿರ ಶೂನ್ಯ	ex_winpos(exarg_T *eap);
#ಇಲ್ಲದಿದ್ದರೆ
# define ex_winpos	    ex_ni
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ	ex_operators(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_put(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_iput(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_copymove(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_submagic(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_join(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_at(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_bang(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_undo(exarg_T *eap);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
ಸ್ಥಿರ ಶೂನ್ಯ	ex_wundo(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_rundo(exarg_T *eap);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ	ex_redo(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_later(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_redir(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_redrawstatus(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_redrawtabline(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_redrawtabpanel(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	close_redir(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_mark(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_startinsert(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_stopinsert(exarg_T *eap);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
ಸ್ಥಿರ ಶೂನ್ಯ	ex_checkpath(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_findpat(exarg_T *eap);
#ಇಲ್ಲದಿದ್ದರೆ
# define ex_findpat		ex_ni
# define ex_checkpath		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_ಕಂಡುಹಿಡಿ_ID) && defined(FEAT_QUICKFIX)
ಸ್ಥಿರ ಶೂನ್ಯ	ex_psearch(exarg_T *eap);
#ಇಲ್ಲದಿದ್ದರೆ
# define ex_psearch		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ	ex_ಟ್ಯಾಗ್(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_ಟ್ಯಾಗ್_cmd(exarg_T *eap, char_u *name);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_EVAL
# define ex_ಖಂಡ		ex_ni
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
# define ex_ಮುಕ್ತಾಯ		ex_ni
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
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_SESSION
# define ex_loadview		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_VIMINFO
# define ex_viminfo		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ	ex_behave(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_filetype(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_setfiletype(exarg_T *eap);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_DIFF
# define ex_diffoff		ex_ni
# define ex_diffpatch		ex_ni
# define ex_diffgetput		ex_ni
# define ex_diffsplit		ex_ni
# define ex_diffthis		ex_ni
# define ex_diffupdate		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ	ex_digraphs(exarg_T *eap);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
ಸ್ಥಿರ ಶೂನ್ಯ	ex_nohlsearch(exarg_T *eap);
#ಇಲ್ಲದಿದ್ದರೆ
# define ex_nohlsearch		ex_ni
# define ex_ಹೊಂದಿಕೆ		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
ಸ್ಥಿರ ಶೂನ್ಯ	ex_X(exarg_T *eap);
#ಇಲ್ಲದಿದ್ದರೆ
# define ex_X			ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
ಸ್ಥಿರ ಶೂನ್ಯ	ex_fold(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_foldopen(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_folddo(exarg_T *eap);
#ಇಲ್ಲದಿದ್ದರೆ
# define ex_fold		ex_ni
# define ex_foldopen		ex_ni
# define ex_folddo		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !(defined(HAVE_LOCALE_H) || defined(X_LOCALE))
# define ex_language		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_SIGNS
# define ex_sign		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_NETBEANS_INTG
# define ex_nbclose		ex_ni
# define ex_nbkey		ex_ni
# define ex_nbstart		ex_ni
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_PROFILE
# define ex_profile		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_TERMINAL
# define ex_terminal		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(FEAT_X11) || !defined(FEAT_XCLIPBOARD)
# define ex_xrestore		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(FEAT_WAYLAND)
# define ex_wlrestore		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(HAVE_CLIPMETHOD)
# define ex_clipreset		ex_ni
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(FEAT_PROP_POPUP)
# define ex_popupclear		ex_ni
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Declare cmdnames[].
 */
#ವ್ಯಾಖ್ಯಾನಿಸು DO_DECLARE_EXCMD
#ಅನ್ತರ್ಭಾವ "ex_cmds.h"
#ಅನ್ತರ್ಭಾವ "ex_cmdidxs.h"

ಸ್ಥಿರ char_u dollar_command[2] = {'$', 0};


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
// Struct for storing a line inside a while/for loop
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u	*ಸಾಲು;		// command line
    linenr_T	lnum;		// sourcing_lnum of the line
} wcmd_T;

/*
 * Structure used to store info for line position in a while or for loop.
 * This is required, because do_one_cmd() may invoke ex_function(), which
 * reads more lines that may come from the while/for loop.
 */
ರಚನೆ loop_cookie
{
    garray_T	*ಸಾಲುಗಳು_gap;		// growarray with line info
    ಪೂರ್ಣಾಂಕ		current_ಸಾಲು;		// last read line from growarray
    ಪೂರ್ಣಾಂಕ		repeating;		// TRUE when looping a second time
    // When "repeating" is FALSE use "getline" and "cookie" to get lines
    char_u	*(*lc_getline)(ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, getline_opt_T);
    ಶೂನ್ಯ	*cookie;
};

ಸ್ಥಿರ char_u	*get_loop_ಸಾಲು(ಪೂರ್ಣಾಂಕ c, ಶೂನ್ಯ *cookie, ಪೂರ್ಣಾಂಕ indent, getline_opt_T options);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	store_loop_ಸಾಲು(garray_T *gap, char_u *ಸಾಲು);
ಸ್ಥಿರ ಶೂನ್ಯ	free_cmdlines(garray_T *gap);

// Struct to save a few things while debugging.  Used in do_cmdline() only.
ರಚನೆ dbg_stuff
{
    ಪೂರ್ಣಾಂಕ		trylevel;
    ಪೂರ್ಣಾಂಕ		force_abort;
    except_T	*caught_ರಾಶಿ;
    char_u	*vv_exception;
    char_u	*vv_throwpoint;
    ಪೂರ್ಣಾಂಕ		did_emsg;
    ಪೂರ್ಣಾಂಕ		got_int;
    ಪೂರ್ಣಾಂಕ		did_throw;
    ಪೂರ್ಣಾಂಕ		need_rethrow;
    ಪೂರ್ಣಾಂಕ		check_cstack;
    except_T	*current_exception;
};

    ಸ್ಥಿರ ಶೂನ್ಯ
save_dbg_stuff(ರಚನೆ dbg_stuff *dsp)
{
    dsp->trylevel	= trylevel;		trylevel = 0;
    dsp->force_abort	= force_abort;		force_abort = FALSE;
    dsp->caught_ರಾಶಿ	= caught_ರಾಶಿ;		caught_ರಾಶಿ = ಖಾಲಿ;
    dsp->vv_exception	= v_exception(ಖಾಲಿ);
    dsp->vv_throwpoint	= v_throwpoint(ಖಾಲಿ);

    // Necessary for debugging an inactive ":catch", ":finally", ":endtry"
    dsp->did_emsg	= did_emsg;		did_emsg     = FALSE;
    dsp->got_int	= got_int;		got_int	     = FALSE;
    dsp->did_throw	= did_throw;		did_throw    = FALSE;
    dsp->need_rethrow	= need_rethrow;		need_rethrow = FALSE;
    dsp->check_cstack	= check_cstack;		check_cstack = FALSE;
    dsp->current_exception = current_exception;	current_exception = ಖಾಲಿ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
restore_dbg_stuff(ರಚನೆ dbg_stuff *dsp)
{
    suppress_errthrow = FALSE;
    trylevel = dsp->trylevel;
    force_abort = dsp->force_abort;
    caught_ರಾಶಿ = dsp->caught_ರಾಶಿ;
    (ಶೂನ್ಯ)v_exception(dsp->vv_exception);
    (ಶೂನ್ಯ)v_throwpoint(dsp->vv_throwpoint);
    did_emsg = dsp->did_emsg;
    got_int = dsp->got_int;
    did_throw = dsp->did_throw;
    need_rethrow = dsp->need_rethrow;
    check_cstack = dsp->check_cstack;
    current_exception = dsp->current_exception;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Check if ffname differs from fnum.
 * fnum is a buffer number. 0 == current buffer, 1-or-more must be a valid
 * buffer ID.
 * ffname is a full path to where a buffer lives on-disk or would live on-disk.
 *
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_other_file(ಪೂರ್ಣಾಂಕ fnum, char_u *ffname)
{
    ಆದರೆ (fnum != 0)
    {
	ಆದರೆ (fnum == curbuf->b_fnum)
	    ಹಿಂತಿರುಗಿ FALSE;

	ಹಿಂತಿರುಗಿ TRUE;
    }

    ಆದರೆ (ffname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ TRUE;

    ಆದರೆ (*ffname == NUL)
	ಹಿಂತಿರುಗಿ FALSE;

    // TODO: Need a reliable way to know whether a buffer is meant to live
    // on-disk !curbuf->b_dev_valid is not always available (example: missing
    // on Windows)
    ಆದರೆ (curbuf->b_sfname != ಖಾಲಿ
	    && *curbuf->b_sfname != NUL)
	// This occurs with unsaved buffers. In which case `ffname` actually
	// corresponds to curbuf->b_sfname
	ಹಿಂತಿರುಗಿ fnamecmp(ffname, curbuf->b_sfname) != 0;

    ಹಿಂತಿರುಗಿ otherfile(ffname);
}

/*
 * do_exmode(): Repeatedly get commands for the "Ex" mode, until the ":vi"
 * command is given.
 */
    ಶೂನ್ಯ
do_exmode(
    ಪೂರ್ಣಾಂಕ		improved)	    // TRUE for "improved Ex" mode
{
    ಪೂರ್ಣಾಂಕ		save_ಸಂದೇಶ_scroll;
    ಪೂರ್ಣಾಂಕ		prev_ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
    linenr_T	prev_ಸಾಲು;
    varnumber_T	changedtick;

    ಆದರೆ (improved)
	exmode_ಸಕ್ರಿಯ = EXMODE_VIM;
    ಇಲ್ಲದಿದ್ದರೆ
	exmode_ಸಕ್ರಿಯ = EXMODE_ಸಾಮಾನ್ಯ;
    State = ಬಹುಲಕ_ಸಾಮಾನ್ಯ;
    may_trigger_modechanged();

    // When using ":global /pat/ visual" and then "Q" we return to continue
    // the :global command.
    ಆದರೆ (global_busy)
	ಹಿಂತಿರುಗಿ;

    save_ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll;
    ++RedrawingDisabled;	    // don't redisplay the window
    ++no_wait_return;		    // don't wait for return
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // Ignore scrollbar and mouse events in Ex mode
    ++hold_gui_events;
#ಆದರೆ_ಅಂತ್ಯ

    ಸಂದೇಶ(_("Entering Ex mode.  Type \"visual\" to go to Normal mode."));
    ಆಗ (exmode_ಸಕ್ರಿಯ)
    {
	// Check for a ":normal" command and no more characters left.
	ಆದರೆ (ex_ಸಾಮಾನ್ಯ_busy > 0 && typebuf.tb_len == 0)
	{
	    exmode_ಸಕ್ರಿಯ = FALSE;
	    ನಿಲ್ಲಿಸು;
	}
	ಸಂದೇಶ_scroll = TRUE;
	need_wait_return = FALSE;
	ex_pressedreturn = FALSE;
	ex_no_reprint = FALSE;
	changedtick = CHANGEDTICK(curbuf);
	prev_ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
	prev_ಸಾಲು = curwin->w_cursor.lnum;
	ಆದರೆ (improved)
	{
	    cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
	    do_cmdline(ಖಾಲಿ, getexline, ಖಾಲಿ, 0);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    do_cmdline(ಖಾಲಿ, getexmodeline, ಖಾಲಿ, DOCMD_NOWAIT);
	ಸಾಲುಗಳು_ಎಡ = ಅಡ್ಡಸಾಲುಗಳು - 1;

	ಆದರೆ ((prev_ಸಾಲು != curwin->w_cursor.lnum
		   || changedtick != CHANGEDTICK(curbuf)) && !ex_no_reprint)
	{
	    ಆದರೆ (curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)
		emsg(_(e_ಖಾಲಿಗಳು_ಬಫರ್));
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (ex_pressedreturn)
		{
		    // go up one line, to overwrite the ":<CR>" line, so the
		    // output doesn't contain empty lines.
		    ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = prev_ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
		    ಆದರೆ (prev_ಸಂದೇಶ_ಅಡ್ಡಸಾಲು == ಅಡ್ಡಸಾಲುಗಳು - 1)
			ಸಂದೇಶ_ಅಡ್ಡಸಾಲು--;
		}
		ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
		print_ಸಾಲು_no_ಪೂರ್ವಪ್ರತ್ಯಯ(curwin->w_cursor.lnum, FALSE, FALSE);
		ಸಂದೇಶ_clr_eos();
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ex_pressedreturn && !ex_no_reprint)	// must be at EOF
	{
	    ಆದರೆ (curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)
		emsg(_(e_ಖಾಲಿಗಳು_ಬಫರ್));
	    ಇಲ್ಲದಿದ್ದರೆ
		emsg(_(e_at_ಕೊನೆ_ದ_file));
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    --hold_gui_events;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    update_screen(UPD_CLEAR);
    need_wait_return = FALSE;
    ಸಂದೇಶ_scroll = save_ಸಂದೇಶ_scroll;
}

/*
 * Print the executed command for when 'verbose' is set.
 * When "lnum" is 0 only print the command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_verbose_cmd(linenr_T lnum, char_u *cmd)
{
    ++no_wait_return;
    verbose_enter_scroll();

    ಆದರೆ (lnum == 0)
	smsg(_("Executing: %s"), cmd);
    ಇಲ್ಲದಿದ್ದರೆ
	smsg(_("line %ld: %s"), (ದೀರ್ಘ)lnum, cmd);
    ಆದರೆ (ಸಂದೇಶ_silent == 0)
	ಸಂದೇಶ_puts("\n");   // don't overwrite this

    verbose_leave_scroll();
    --no_wait_return;
}

/*
 * Execute a simple command line.  Used for translated commands like "*".
 */
    ಪೂರ್ಣಾಂಕ
do_cmdline_cmd(char_u *cmd)
{
    ಹಿಂತಿರುಗಿ do_cmdline(cmd, ಖಾಲಿ, ಖಾಲಿ,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
}

/*
 * Execute the "+cmd" argument of "edit +cmd fname" and the like.
 * This allows for using a range without ":" in Vim9 script.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
do_cmd_ವಾದ(char_u *cmd)
{
    ಹಿಂತಿರುಗಿ do_cmdline(cmd, ಖಾಲಿ, ಖಾಲಿ,
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
    ಪೂರ್ಣಾಂಕ
do_cmdline(
    char_u	*cmdline,
    char_u	*(*fgetline)(ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, getline_opt_T),
    ಶೂನ್ಯ	*cookie,		// argument for fgetline()
    ಪೂರ್ಣಾಂಕ		flags)
{
    char_u	*ಮುಂದಿನ_cmdline;		// next cmd to execute
    char_u	*cmdline_copy = ಖಾಲಿ;	// copy of cmd line
    ಪೂರ್ಣಾಂಕ		ಬಳಸಿದ_getline = FALSE;	// used "fgetline" to obtain command
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = 0;		// recursive depth
    ಪೂರ್ಣಾಂಕ		ಸಂದೇಶ_didout_before_ಆರಂಭ = 0;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;		// line number count
    ಪೂರ್ಣಾಂಕ		did_inc_RedrawingDisabled = FALSE;
    ಪೂರ್ಣಾಂಕ		retval = OK;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    cstack_T	cstack;			// conditional stack
    garray_T	ಸಾಲುಗಳು_ga;		// keep lines for ":while"/":for"
    ಪೂರ್ಣಾಂಕ		current_ಸಾಲು = 0;	// active line in lines_ga
    ಪೂರ್ಣಾಂಕ		current_ಸಾಲು_before = 0;
    char_u	*fname = ಖಾಲಿ;		// function or script name
    linenr_T	*breakpoint = ಖಾಲಿ;	// ptr to breakpoint field in cookie
    ಪೂರ್ಣಾಂಕ		*dbg_tick = ಖಾಲಿ;	// ptr to dbg_tick field in cookie
    ರಚನೆ dbg_stuff debug_saved;	// saved things for debug mode
    ಪೂರ್ಣಾಂಕ		initial_trylevel;
    msglist_T	**saved_ಸಂದೇಶ_list = ಖಾಲಿ;
    msglist_T	*ಖಾಸಗಿ_ಸಂದೇಶ_list = ಖಾಲಿ;

    // "fgetline" and "cookie" passed to do_one_cmd()
    char_u	*(*cmd_getline)(ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, getline_opt_T);
    ಶೂನ್ಯ	*cmd_cookie;
    ರಚನೆ loop_cookie cmd_loop_cookie;
    ಶೂನ್ಯ	*real_cookie;
    ಪೂರ್ಣಾಂಕ		getline_is_func;
#ಇಲ್ಲದಿದ್ದರೆ
# define cmd_getline fgetline
# define cmd_cookie cookie
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	call_ಆಳ = 0;		// recursiveness
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // For every pair of do_cmdline()/do_one_cmd() calls, use an extra memory
    // location for storing error messages to be converted to an exception.
    // This ensures that the do_errthrow() call in do_one_cmd() does not
    // combine the messages stored by an earlier invocation of do_one_cmd()
    // with the command name of the later one.  This would happen when
    // BufWritePost autocommands are executed after a write error.
    saved_ಸಂದೇಶ_list = ಸಂದೇಶ_list;
    ಸಂದೇಶ_list = &ಖಾಸಗಿ_ಸಂದೇಶ_list;
#ಆದರೆ_ಅಂತ್ಯ

    // It's possible to create an endless loop with ":execute", catch that
    // here.  The value of 200 allows nested function calls, ":source", etc.
    // Allow 200 or 'maxfuncdepth', whatever is larger.
    ಆದರೆ (call_ಆಳ >= 200
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    && call_ಆಳ >= p_mfd
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
	emsg(_(e_command_too_recursive));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// When converting to an exception, we do not include the command name
	// since this is not an error of the specific command.
	do_errthrow((cstack_T *)ಖಾಲಿ, (char_u *)ಖಾಲಿ);
	ಸಂದೇಶ_list = saved_ಸಂದೇಶ_list;
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ++call_ಆಳ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    CLEAR_ಕ್ಷೇತ್ರ(cstack);
    cstack.cs_idx = -1;
    ga_init2(&ಸಾಲುಗಳು_ga, ಗಾತ್ರ(wcmd_T), 10);

    real_cookie = getline_cookie(fgetline, cookie);

    // Inside a function use a higher nesting level.
    getline_is_func = getline_equal(fgetline, cookie, get_func_ಸಾಲು);
    ಆದರೆ (getline_is_func && ex_nesting_level == func_level(real_cookie))
	++ex_nesting_level;

    // Get the function or script name and the address where the next breakpoint
    // line and the debug tick for a function or script are stored.
    ಆದರೆ (getline_is_func)
    {
	fname = func_name(real_cookie);
	breakpoint = func_breakpoint(real_cookie);
	dbg_tick = func_dbg_tick(real_cookie);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (getline_equal(fgetline, cookie, getsourceline))
    {
	fname = SOURCING_NAME;
	breakpoint = ಮೂಲ_breakpoint(real_cookie);
	dbg_tick = ಮೂಲ_dbg_tick(real_cookie);
    }

    /*
     * Initialize "force_abort"  and "suppress_errthrow" at the top level.
     */
    ಆದರೆ (!recursive)
    {
	force_abort = FALSE;
	suppress_errthrow = FALSE;
    }

    /*
     * If requested, store and reset the global values controlling the
     * exception handling (used when debugging).  Otherwise clear it to avoid
     * a bogus compiler warning when the optimizer uses inline functions...
     */
    ಆದರೆ (flags & DOCMD_EXCRESET)
	save_dbg_stuff(&debug_saved);
    ಇಲ್ಲದಿದ್ದರೆ
	CLEAR_ಕ್ಷೇತ್ರ(debug_saved);

    initial_trylevel = trylevel;

    /*
     * "did_throw" will be set to TRUE when an exception is being thrown.
     */
    did_throw = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    /*
     * "did_emsg" will be set to TRUE when emsg() is used, in which case we
     * cancel the whole command line, and any if/endif or loop.
     * If force_abort is set, we cancel everything.
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    did_emsg_cumul += did_emsg;
#ಆದರೆ_ಅಂತ್ಯ
    did_emsg = FALSE;

    /*
     * KeyTyped is only set when calling vgetc().  Reset it here when not
     * calling vgetc() (sourced command lines).
     */
    ಆದರೆ (!(flags & DOCMD_KEYTYPED)
			       && !getline_equal(fgetline, cookie, getexline))
	ಕೀಲಿ_Typed = FALSE;

    /*
     * Continue executing command lines:
     * - when inside an ":if", ":while" or ":for"
     * - for multiple commands on one line, separated with '|'
     * - when repeating until there are no more lines (for ":source")
     */
    ಮುಂದಿನ_cmdline = cmdline;
    ಮಾಡು
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	getline_is_func = getline_equal(fgetline, cookie, get_func_ಸಾಲು);
#ಆದರೆ_ಅಂತ್ಯ

	// stop skipping cmds for an error msg after all endif/while/for
	ಆದರೆ (ಮುಂದಿನ_cmdline == ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		&& !force_abort
		&& cstack.cs_idx < 0
		&& !(getline_is_func && func_has_abort(real_cookie))
#ಆದರೆ_ಅಂತ್ಯ
							)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    did_emsg_cumul += did_emsg;
#ಆದರೆ_ಅಂತ್ಯ
	    did_emsg = FALSE;
	}

	/*
	 * 1. If repeating a line in a loop, get a line from lines_ga.
	 * 2. If no line given: Get an allocated line with fgetline().
	 * 3. If a line is given: Make a copy, so we can mess with it.
	 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// 1. If repeating, get a previous line from lines_ga.
	ಆದರೆ (cstack.cs_looplevel > 0 && current_ಸಾಲು < ಸಾಲುಗಳು_ga.ga_len)
	{
	    // Each '|' separated command is stored separately in lines_ga, to
	    // be able to jump to it.  Don't use next_cmdline now.
	    VIM_CLEAR(cmdline_copy);

	    // Check if a function has returned or, unless it has an unclosed
	    // try conditional, aborted.
	    ಆದರೆ (getline_is_func)
	    {
# ifdef FEAT_PROFILE
		ಆದರೆ (do_profiling == PROF_YES)
		    func_ಸಾಲು_ಕೊನೆ(real_cookie);
# endif
		ಆದರೆ (func_has_ended(real_cookie))
		{
		    retval = FAIL;
		    ನಿಲ್ಲಿಸು;
		}
	    }
# ifdef FEAT_PROFILE
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (do_profiling == PROF_YES
			    && getline_equal(fgetline, cookie, getsourceline))
		script_ಸಾಲು_ಕೊನೆ();
# endif

	    // Check if a sourced file hit a ":finish" command.
	    ಆದರೆ (ಮೂಲ_ಪೂರ್ಣಗೊಂಡ(fgetline, cookie))
	    {
		retval = FAIL;
		ನಿಲ್ಲಿಸು;
	    }

	    // If breakpoints have been added/deleted need to check for it.
	    ಆದರೆ (breakpoint != ಖಾಲಿ && dbg_tick != ಖಾಲಿ
						   && *dbg_tick != debug_tick)
	    {
		*breakpoint = dbg_ಕಂಡುಹಿಡಿ_breakpoint(
				getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }

	    ಮುಂದಿನ_cmdline = ((wcmd_T *)(ಸಾಲುಗಳು_ga.ga_ದತ್ತಾಂಶ))[current_ಸಾಲು].ಸಾಲು;
	    SOURCING_LNUM = ((wcmd_T *)(ಸಾಲುಗಳು_ga.ga_ದತ್ತಾಂಶ))[current_ಸಾಲು].lnum;

	    // Did we encounter a breakpoint?
	    ಆದರೆ (breakpoint != ಖಾಲಿ && *breakpoint != 0
					      && *breakpoint <= SOURCING_LNUM)
	    {
		dbg_breakpoint(fname, SOURCING_LNUM);
		// Find next breakpoint.
		*breakpoint = dbg_ಕಂಡುಹಿಡಿ_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }
# ifdef FEAT_PROFILE
	    ಆದರೆ (do_profiling == PROF_YES)
	    {
		ಆದರೆ (getline_is_func)
		    func_ಸಾಲು_ಆರಂಭ(real_cookie, SOURCING_LNUM);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (getline_equal(fgetline, cookie, getsourceline))
		    script_ಸಾಲು_ಆರಂಭ();
	    }
# endif
	}
#ಆದರೆ_ಅಂತ್ಯ

	// 2. If no line given, get an allocated line with fgetline().
	ಆದರೆ (ಮುಂದಿನ_cmdline == ಖಾಲಿ)
	{
	    /*
	     * Need to set msg_didout for the first line after an ":if",
	     * otherwise the ":if" will be overwritten.
	     */
	    ಆದರೆ (ಎಣಿಕೆ_2 == 1 && getline_equal(fgetline, cookie, getexline))
		ಸಂದೇಶ_didout = TRUE;
	    ಆದರೆ (fgetline == ಖಾಲಿ || (ಮುಂದಿನ_cmdline = fgetline(':', cookie,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    cstack.cs_idx < 0 ? 0 : (cstack.cs_idx + 1) * 2
#ಇಲ್ಲದಿದ್ದರೆ
		    0
#ಆದರೆ_ಅಂತ್ಯ
		    , ಒಳ_vim9script() ? GETLINE_CONCAT_CONTBAR
					       : GETLINE_CONCAT_CONT)) == ಖಾಲಿ)
	    {
		// Don't call wait_return() for aborted command line.  The NULL
		// returned for the end of a sourced file or executed function
		// doesn't do this.
		ಆದರೆ (ಕೀಲಿ_Typed && !(flags & DOCMD_REPEAT))
		    need_wait_return = FALSE;
		retval = FAIL;
		ನಿಲ್ಲಿಸು;
	    }
	    ಬಳಸಿದ_getline = TRUE;

	    /*
	     * Keep the first typed line.  Clear it when more lines are typed.
	     */
	    ಆದರೆ (flags & DOCMD_KEEPLINE)
	    {
		vim_free(repeat_cmdline);
		ಆದರೆ (ಎಣಿಕೆ_2 == 0)
		    repeat_cmdline = vim_strsave(ಮುಂದಿನ_cmdline);
		ಇಲ್ಲದಿದ್ದರೆ
		    repeat_cmdline = ಖಾಲಿ;
	    }
	}

	// 3. Make a copy of the command so we can mess with it.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmdline_copy == ಖಾಲಿ)
	{
	    ಮುಂದಿನ_cmdline = vim_strsave(ಮುಂದಿನ_cmdline);
	    ಆದರೆ (ಮುಂದಿನ_cmdline == ಖಾಲಿ)
	    {
		emsg(_(e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ));
		retval = FAIL;
		ನಿಲ್ಲಿಸು;
	    }
	}
	cmdline_copy = ಮುಂದಿನ_cmdline;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
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
	ಆದರೆ ((cstack.cs_looplevel > 0 || has_loop_cmd(ಮುಂದಿನ_cmdline)))
	{
	    cmd_getline = get_loop_ಸಾಲು;
	    cmd_cookie = (ಶೂನ್ಯ *)&cmd_loop_cookie;
	    cmd_loop_cookie.ಸಾಲುಗಳು_gap = &ಸಾಲುಗಳು_ga;
	    cmd_loop_cookie.current_ಸಾಲು = current_ಸಾಲು;
	    cmd_loop_cookie.lc_getline = fgetline;
	    cmd_loop_cookie.cookie = cookie;
	    cmd_loop_cookie.repeating = (current_ಸಾಲು < ಸಾಲುಗಳು_ga.ga_len);

	    // Save the current line when encountering it the first time.
	    ಆದರೆ (current_ಸಾಲು == ಸಾಲುಗಳು_ga.ga_len
		    && store_loop_ಸಾಲು(&ಸಾಲುಗಳು_ga, ಮುಂದಿನ_cmdline) == FAIL)
	    {
		retval = FAIL;
		ನಿಲ್ಲಿಸು;
	    }
	    current_ಸಾಲು_before = current_ಸಾಲು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    cmd_getline = fgetline;
	    cmd_cookie = cookie;
	}

	did_endif = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (ಎಣಿಕೆ_2++ == 0)
	{
	    /*
	     * All output from the commands is put below each other, without
	     * waiting for a return. Don't do this when executing commands
	     * from a script or when being called recursive (e.g. for ":e
	     * +command file").
	     */
	    ಆದರೆ (!(flags & DOCMD_NOWAIT) && !recursive)
	    {
		ಸಂದೇಶ_didout_before_ಆರಂಭ = ಸಂದೇಶ_didout;
		ಸಂದೇಶ_didany = FALSE; // no output yet
		ಸಂದೇಶ_ಆರಂಭ();
		ಸಂದೇಶ_scroll = TRUE;  // put messages below each other
		++no_wait_return;   // don't wait for return until finished
		++RedrawingDisabled;
		did_inc_RedrawingDisabled = TRUE;
	    }
	}

	ಆದರೆ ((p_verbose >= 15 && SOURCING_NAME != ಖಾಲಿ) || p_verbose >= 16)
	    ಸಂದೇಶ_verbose_cmd(SOURCING_LNUM, cmdline_copy);

	/*
	 * 2. Execute one '|' separated command.
	 *    do_one_cmd() will return NULL if there is no trailing '|'.
	 *    "cmdline_copy" can change, e.g. for '%' and '#' expansion.
	 */
	++recursive;
	ಮುಂದಿನ_cmdline = do_one_cmd(&cmdline_copy, flags,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
				&cstack,
#ಆದರೆ_ಅಂತ್ಯ
				cmd_getline, cmd_cookie);
	--recursive;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (cmd_cookie == (ಶೂನ್ಯ *)&cmd_loop_cookie)
	    // Use "current_line" from "cmd_loop_cookie", it may have been
	    // incremented when defining a function.
	    current_ಸಾಲು = cmd_loop_cookie.current_ಸಾಲು;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (ಮುಂದಿನ_cmdline == ಖಾಲಿ)
	{
	    VIM_CLEAR(cmdline_copy);

	    /*
	     * If the command was typed, remember it for the ':' register.
	     * Do this AFTER executing the command to make :@: work.
	     */
	    ಆದರೆ (getline_equal(fgetline, cookie, getexline)
						  && new_ಕೊನೆಯ_cmdline != ಖಾಲಿ)
	    {
		vim_free(ಕೊನೆಯ_cmdline);
		ಕೊನೆಯ_cmdline = new_ಕೊನೆಯ_cmdline;
		new_ಕೊನೆಯ_cmdline = ಖಾಲಿ;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // need to copy the command after the '|' to cmdline_copy, for the
	    // next do_one_cmd()
	    STRMOVE(cmdline_copy, ಮುಂದಿನ_cmdline);
	    ಮುಂದಿನ_cmdline = cmdline_copy;
	}


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// reset did_emsg for a function that is not aborted by an error
	ಆದರೆ (did_emsg && !force_abort
		&& getline_equal(fgetline, cookie, get_func_ಸಾಲು)
					      && !func_has_abort(real_cookie))
	{
	    // did_emsg_cumul is not set here
	    did_emsg = FALSE;
	}

	ಆದರೆ (cstack.cs_looplevel > 0)
	{
	    ++current_ಸಾಲು;

	    /*
	     * An ":endwhile", ":endfor" and ":continue" is handled here.
	     * If we were executing commands, jump back to the ":while" or
	     * ":for".
	     * If we were not executing commands, decrement cs_looplevel.
	     */
	    ಆದರೆ (cstack.cs_lflags & (CSL_HAD_CONT | CSL_HAD_ENDLOOP))
	    {
		cstack.cs_lflags &= ~(CSL_HAD_CONT | CSL_HAD_ENDLOOP);

		// Jump back to the matching ":while" or ":for".  Be careful
		// not to use a cs_line[] from an entry that isn't a ":while"
		// or ":for": It would make "current_line" invalid and can
		// cause a crash.
		ಆದರೆ (!did_emsg && !got_int && !did_throw
			&& cstack.cs_idx >= 0
			&& (cstack.cs_flags[cstack.cs_idx]
						      & (CSF_WHILE | CSF_FOR))
			&& cstack.cs_ಸಾಲು[cstack.cs_idx] >= 0
			&& (cstack.cs_flags[cstack.cs_idx] & CSF_ಸಕ್ರಿಯ))
		{
		    current_ಸಾಲು = cstack.cs_ಸಾಲು[cstack.cs_idx];
						// remember we jumped there
		    cstack.cs_lflags |= CSL_HAD_LOOP;
		    ಸಾಲು_breakcheck();		// check if CTRL-C typed

		    // Check for the next breakpoint at or after the ":while"
		    // or ":for".
		    ಆದರೆ (breakpoint != ಖಾಲಿ && ಸಾಲುಗಳು_ga.ga_len > current_ಸಾಲು)
		    {
			*breakpoint = dbg_ಕಂಡುಹಿಡಿ_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
									fname,
			   ((wcmd_T *)ಸಾಲುಗಳು_ga.ga_ದತ್ತಾಂಶ)[current_ಸಾಲು].lnum-1);
			*dbg_tick = debug_tick;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // can only get here with ":endwhile" or ":endfor"
		    ಆದರೆ (cstack.cs_idx >= 0)
			rewind_conditionals(&cstack, cstack.cs_idx - 1,
				   CSF_WHILE | CSF_FOR, &cstack.cs_looplevel);
		}
	    }

	    /*
	     * For a ":while" or ":for" we need to remember the line number.
	     */
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cstack.cs_lflags & CSL_HAD_LOOP)
	    {
		cstack.cs_lflags &= ~CSL_HAD_LOOP;
		cstack.cs_ಸಾಲು[cstack.cs_idx] = current_ಸಾಲು_before;
	    }
	}

	// Check for the next breakpoint after a watchexpression
	ಆದರೆ (breakpoint != ಖಾಲಿ && has_watchexpr())
	{
	    *breakpoint = dbg_ಕಂಡುಹಿಡಿ_breakpoint(FALSE, fname, SOURCING_LNUM);
	    *dbg_tick = debug_tick;
	}

	/*
	 * When not inside any ":while" loop, clear remembered lines.
	 */
	ಆದರೆ (cstack.cs_looplevel == 0)
	{
	    ಆದರೆ (ಸಾಲುಗಳು_ga.ga_len > 0)
	    {
		SOURCING_LNUM =
		       ((wcmd_T *)ಸಾಲುಗಳು_ga.ga_ದತ್ತಾಂಶ)[ಸಾಲುಗಳು_ga.ga_len - 1].lnum;
		free_cmdlines(&ಸಾಲುಗಳು_ga);
	    }
	    current_ಸಾಲು = 0;
	}

	/*
	 * A ":finally" makes did_emsg, got_int, and did_throw pending for
	 * being restored at the ":endtry".  Reset them here and set the
	 * ACTIVE and FINALLY flags, so that the finally clause gets executed.
	 * This includes the case where a missing ":endif", ":endwhile" or
	 * ":endfor" was detected by the ":finally" itself.
	 */
	ಆದರೆ (cstack.cs_lflags & CSL_HAD_FINA)
	{
	    cstack.cs_lflags &= ~CSL_HAD_FINA;
	    report_make_pending(cstack.cs_pending[cstack.cs_idx]
		    & (CSTP_ERROR | CSTP_ಅಡಚಣೆ | CSTP_THROW),
		    did_throw ? (ಶೂನ್ಯ *)current_exception : ಖಾಲಿ);
	    did_emsg = got_int = did_throw = FALSE;
	    cstack.cs_flags[cstack.cs_idx] |= CSF_ಸಕ್ರಿಯ | CSF_FINALLY;
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
	ಆದರೆ (trylevel == 0 && !did_emsg && !got_int && !did_throw)
	    force_abort = FALSE;

	// Convert an interrupt to an exception if appropriate.
	(ಶೂನ್ಯ)do_intthrow(&cstack);
#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL

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
    ಆಗ (!((got_int
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    || (did_emsg && (force_abort || ಒಳ_vim9script()))
		    || did_throw
#ಆದರೆ_ಅಂತ್ಯ
	     )
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		&& cstack.cs_trylevel == 0
#ಆದರೆ_ಅಂತ್ಯ
	    )
	    && !(did_emsg
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		// Keep going when inside try/catch, so that the error can be
		// deal with, except when it is a syntax error, it may cause
		// the :endtry to be missed.
		&& (cstack.cs_trylevel == 0 || did_emsg_syntax)
#ಆದರೆ_ಅಂತ್ಯ
		&& ಬಳಸಿದ_getline
			    && (getline_equal(fgetline, cookie, getexmodeline)
			       || getline_equal(fgetline, cookie, getexline)))
	    && (ಮುಂದಿನ_cmdline != ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
			|| cstack.cs_idx >= 0
#ಆದರೆ_ಅಂತ್ಯ
			|| (flags & DOCMD_REPEAT)));

    vim_free(cmdline_copy);
    did_emsg_syntax = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    free_cmdlines(&ಸಾಲುಗಳು_ga);
    ga_clear(&ಸಾಲುಗಳು_ga);

    ಆದರೆ (cstack.cs_idx >= 0)
    {
	/*
	 * If a sourced file or executed function ran to its end, report the
	 * unclosed conditional.
	 * In Vim9 script do not give a second error, executing aborts after
	 * the first one.
	 */
	ಆದರೆ (!got_int && !did_throw && !aborting()
		&& !(did_emsg && ಒಳ_vim9script())
		&& ((getline_equal(fgetline, cookie, getsourceline)
			&& !ಮೂಲ_ಪೂರ್ಣಗೊಂಡ(fgetline, cookie))
		    || (getline_equal(fgetline, cookie, get_func_ಸಾಲು)
					    && !func_has_ended(real_cookie))))
	{
	    ಆದರೆ (cstack.cs_flags[cstack.cs_idx] & CSF_TRY)
		emsg(_(e_missing_endtry));
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cstack.cs_flags[cstack.cs_idx] & CSF_WHILE)
		emsg(_(e_missing_endwhile));
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cstack.cs_flags[cstack.cs_idx] & CSF_FOR)
		emsg(_(e_missing_endfor));
	    ಇಲ್ಲದಿದ್ದರೆ
		emsg(_(e_missing_endif));
	}

	/*
	 * Reset "trylevel" in case of a ":finish" or ":return" or a missing
	 * ":endtry" in a sourced file or executed function.  If the try
	 * conditional is in its finally clause, ignore anything pending.
	 * If it is in a catch clause, finish the caught exception.
	 * Also cleanup any "cs_forinfo" structures.
	 */
	ಮಾಡು
	{
	    ಪೂರ್ಣಾಂಕ idx = cleanup_conditionals(&cstack, 0, TRUE);

	    ಆದರೆ (idx >= 0)
		--idx;	    // remove try block not in its finally clause
	    rewind_conditionals(&cstack, idx, CSF_WHILE | CSF_FOR,
							&cstack.cs_looplevel);
	}
	ಆಗ (cstack.cs_idx >= 0);
	trylevel = initial_trylevel;
    }

    // If a missing ":endtry", ":endwhile", ":endfor", or ":endif" or a memory
    // lack was reported above and the error message is to be converted to an
    // exception, do this now after rewinding the cstack.
    do_errthrow(&cstack, getline_equal(fgetline, cookie, get_func_ಸಾಲು)
				  ? (char_u *)"endfunction" : (char_u *)ಖಾಲಿ);

    ಆದರೆ (trylevel == 0)
    {
	// Just in case did_throw got set but current_exception wasn't.
	ಆದರೆ (current_exception == ಖಾಲಿ)
	    did_throw = FALSE;

	/*
	 * When an exception is being thrown out of the outermost try
	 * conditional, discard the uncaught exception, disable the conversion
	 * of interrupts or errors to exceptions, and ensure that no more
	 * commands are executed.
	 */
	ಆದರೆ (did_throw)
	    handle_did_throw();

	/*
	 * On an interrupt or an aborting error not converted to an exception,
	 * disable the conversion of errors to exceptions.  (Interrupts are not
	 * converted anymore, here.) This enables also the interrupt message
	 * when force_abort is set and did_emsg unset in case of an interrupt
	 * from a finally clause after an error.
	 */
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (got_int || (did_emsg && force_abort))
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
    ಆದರೆ (did_throw)
	need_rethrow = TRUE;
    ಆದರೆ ((getline_equal(fgetline, cookie, getsourceline)
		&& ex_nesting_level > ಮೂಲ_level(real_cookie))
	    || (getline_equal(fgetline, cookie, get_func_ಸಾಲು)
		&& ex_nesting_level > func_level(real_cookie) + 1))
    {
	ಆದರೆ (!did_throw)
	    check_cstack = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// When leaving a function, reduce nesting level.
	ಆದರೆ (getline_equal(fgetline, cookie, get_func_ಸಾಲು))
	    --ex_nesting_level;
	/*
	 * Go to debug mode when returning from a function in which we are
	 * single-stepping.
	 */
	ಆದರೆ ((getline_equal(fgetline, cookie, getsourceline)
		    || getline_equal(fgetline, cookie, get_func_ಸಾಲು))
		&& ex_nesting_level + 1 <= debug_break_level)
	    do_debug(getline_equal(fgetline, cookie, getsourceline)
		    ? (char_u *)_("End of sourced file")
		    : (char_u *)_("End of function"));
    }

    /*
     * Restore the exception environment (done after returning from the
     * debugger).
     */
    ಆದರೆ (flags & DOCMD_EXCRESET)
	restore_dbg_stuff(&debug_saved);

    ಸಂದೇಶ_list = saved_ಸಂದೇಶ_list;

    // Cleanup if "cs_emsg_silent_list" remains.
    ಆದರೆ (cstack.cs_emsg_silent_list != ಖಾಲಿ)
    {
	eslist_T *elem, *ತಾತ್ಕಾಲಿಕ;

	ಗಾಗಿ (elem = cstack.cs_emsg_silent_list; elem != ಖಾಲಿ; elem = ತಾತ್ಕಾಲಿಕ)
	{
	    ತಾತ್ಕಾಲಿಕ = elem->ಮುಂದಿನ;
	    vim_free(elem);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL

    /*
     * If there was too much output to fit on the command line, ask the user to
     * hit return before redrawing the screen. With the ":global" command we do
     * this only once after the command is finished.
     */
    ಆದರೆ (did_inc_RedrawingDisabled)
    {
	ಆದರೆ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	--no_wait_return;
	ಸಂದೇಶ_scroll = FALSE;

	/*
	 * When just finished an ":if"-":else" which was typed, no need to
	 * wait for hit-return.  Also for an error situation.
	 */
	ಆದರೆ (retval == FAIL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		|| (did_endif && ಕೀಲಿ_Typed && !did_emsg)
#ಆದರೆ_ಅಂತ್ಯ
					    )
	{
	    need_wait_return = FALSE;
	    ಸಂದೇಶ_didany = FALSE;		// don't wait when restarting edit
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (need_wait_return)
	{
	    /*
	     * The msg_start() above clears msg_didout. The wait_return() we do
	     * here should not overwrite the command that may be shown before
	     * doing that.
	     */
	    ಸಂದೇಶ_didout |= ಸಂದೇಶ_didout_before_ಆರಂಭ;
	    wait_return(FALSE);
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    did_endif = FALSE;  // in case do_cmdline used recursively
#ಇಲ್ಲದಿದ್ದರೆ
    /*
     * Reset if_level, in case a sourced script file contains more ":if" than
     * ":endif" (could be ":if x | foo | endif").
     */
    if_level = 0;
#ಆದರೆ_ಅಂತ್ಯ

    --call_ಆಳ;
    ಹಿಂತಿರುಗಿ retval;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Handle when "did_throw" is set after executing commands.
 */
    ಶೂನ್ಯ
handle_did_throw(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ	*p = ಖಾಲಿ;
    msglist_T	*messages = ಖಾಲಿ;
    ESTACK_CHECK_DECLARATION;

    /*
     * If the uncaught exception is a user exception, report it as an
     * error.  If it is an error exception, display the saved error
     * message now.  For an interrupt exception, do nothing; the
     * interrupt message is given elsewhere.
     */
    ಆಯ್ಕೆ (current_exception->type)
    {
	ಸ್ಥಿತಿ ET_USER:
	    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE,
		    _(e_exception_not_caught_str),
		    current_exception->ಮೌಲ್ಯ);
	    p = (ಅಕ್ಷರ *)vim_strsave(IObuff);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ET_ERROR:
	    messages = current_exception->messages;
	    current_exception->messages = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ET_ಅಡಚಣೆ:
	    ನಿಲ್ಲಿಸು;
    }

    estack_ತಳ್ಳು(ETYPE_EXCEPT, current_exception->throw_name,
					current_exception->throw_lnum);
    ESTACK_CHECK_ಸಿದ್ಧತೆ;
    current_exception->throw_name = ಖಾಲಿ;

    discard_current_exception();	// uses IObuff if 'verbose'

    // If "silent!" is active the uncaught exception is not fatal.
    ಆದರೆ (emsg_silent == 0)
    {
	suppress_errthrow = TRUE;
	force_abort = TRUE;
    }

    ಆದರೆ (messages != ಖಾಲಿ)
    {
	ಮಾಡು
	{
	    msglist_T	*ಮುಂದಿನ = messages->ಮುಂದಿನ;
	    ಪೂರ್ಣಾಂಕ		save_compiling = estack_compiling;

	    estack_compiling = messages->ಸಂದೇಶ_compiling;
	    emsg(messages->ಸಂದೇಶ);
	    vim_free(messages->ಸಂದೇಶ);
	    vim_free(messages->sfile);
	    vim_free(messages);
	    messages = ಮುಂದಿನ;
	    estack_compiling = save_compiling;
	}
	ಆಗ (messages != ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p != ಖಾಲಿ)
    {
	emsg(p);
	vim_free(p);
    }
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_ತೆಗೆ();
}

/*
 * Obtain a line when inside a ":while" or ":for" loop.
 */
    ಸ್ಥಿರ char_u *
get_loop_ಸಾಲು(ಪೂರ್ಣಾಂಕ c, ಶೂನ್ಯ *cookie, ಪೂರ್ಣಾಂಕ indent, getline_opt_T options)
{
    ರಚನೆ loop_cookie	*cp = (ರಚನೆ loop_cookie *)cookie;
    wcmd_T		*wp;
    char_u		*ಸಾಲು;

    ಆದರೆ (cp->current_ಸಾಲು + 1 >= cp->ಸಾಲುಗಳು_gap->ga_len)
    {
	ಆದರೆ (cp->repeating)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;	// trying to read past ":endwhile"/":endfor"

	// First time inside the ":while"/":for": get line normally.
	ಆದರೆ (cp->lc_getline == ಖಾಲಿ)
	    ಸಾಲು = getcmdline(c, 0L, indent, 0);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಾಲು = cp->lc_getline(c, cp->cookie, indent, options);
	ಆದರೆ (ಸಾಲು != ಖಾಲಿ && store_loop_ಸಾಲು(cp->ಸಾಲುಗಳು_gap, ಸಾಲು) == OK)
	    ++cp->current_ಸಾಲು;

	ಹಿಂತಿರುಗಿ ಸಾಲು;
    }

    ಕೀಲಿ_Typed = FALSE;
    ++cp->current_ಸಾಲು;
    wp = (wcmd_T *)(cp->ಸಾಲುಗಳು_gap->ga_ದತ್ತಾಂಶ) + cp->current_ಸಾಲು;
    SOURCING_LNUM = wp->lnum;
    ಹಿಂತಿರುಗಿ vim_strsave(wp->ಸಾಲು);
}

/*
 * Store a line in "gap" so that a ":while" loop can execute it again.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
store_loop_ಸಾಲು(garray_T *gap, char_u *ಸಾಲು)
{
    ಆದರೆ (ga_grow(gap, 1) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ((wcmd_T *)(gap->ga_ದತ್ತಾಂಶ))[gap->ga_len].ಸಾಲು = vim_strsave(ಸಾಲು);
    ((wcmd_T *)(gap->ga_ದತ್ತಾಂಶ))[gap->ga_len].lnum = SOURCING_LNUM;
    ++gap->ga_len;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Free the lines stored for a ":while" or ":for" loop.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_cmdlines(garray_T *gap)
{
    ಆಗ (gap->ga_len > 0)
    {
	vim_free(((wcmd_T *)(gap->ga_ದತ್ತಾಂಶ))[gap->ga_len - 1].ಸಾಲು);
	--gap->ga_len;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * If "fgetline" is get_loop_line(), return TRUE if the getline it uses equals
 * "func".  * Otherwise return TRUE when "fgetline" equals "func".
 */
    ಪೂರ್ಣಾಂಕ
getline_equal(
    char_u	*(*fgetline)(ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, getline_opt_T),
    ಶೂನ್ಯ	*cookie ಬಳಸದ,		// argument for fgetline()
    char_u	*(*func)(ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, getline_opt_T))
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    char_u		*(*gp)(ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, getline_opt_T);
    ರಚನೆ loop_cookie *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // function that's originally used to obtain the lines.  This may be
    // nested several levels.
    gp = fgetline;
    cp = (ರಚನೆ loop_cookie *)cookie;
    ಆಗ (gp == get_loop_ಸಾಲು)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    ಹಿಂತಿರುಗಿ gp == func;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ fgetline == func;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * If "fgetline" is get_loop_line(), return the cookie used by the original
 * getline function.  Otherwise return "cookie".
 */
    ಶೂನ್ಯ *
getline_cookie(
    char_u	*(*fgetline)(ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, getline_opt_T) ಬಳಸದ,
    ಶೂನ್ಯ	*cookie)		// argument for fgetline()
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    char_u		*(*gp)(ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, getline_opt_T);
    ರಚನೆ loop_cookie  *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (ರಚನೆ loop_cookie *)cookie;
    ಆಗ (gp == get_loop_ಸಾಲು)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    ಹಿಂತಿರುಗಿ cp;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ cookie;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Get the next line source line without advancing.
 */
    char_u *
getline_peek(
    char_u	*(*fgetline)(ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, getline_opt_T) ಬಳಸದ,
    ಶೂನ್ಯ	*cookie)		// argument for fgetline()
{
    char_u		*(*gp)(ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, getline_opt_T);
    ರಚನೆ loop_cookie  *cp;
    wcmd_T		*wp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (ರಚನೆ loop_cookie *)cookie;
    ಆಗ (gp == get_loop_ಸಾಲು)
    {
	ಆದರೆ (cp->current_ಸಾಲು + 1 < cp->ಸಾಲುಗಳು_gap->ga_len)
	{
	    // executing lines a second time, use the stored copy
	    wp = (wcmd_T *)(cp->ಸಾಲುಗಳು_gap->ga_ದತ್ತಾಂಶ) + cp->current_ಸಾಲು + 1;
	    ಹಿಂತಿರುಗಿ wp->ಸಾಲು;
	}
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    ಆದರೆ (gp == getsourceline)
	ಹಿಂತಿರುಗಿ ಮೂಲ_nextline(cp);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ


/*
 * Helper function to apply an offset for buffer commands, i.e. ":bdelete",
 * ":bwipeout", etc.
 * Returns the buffer number.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compute_ಬಫರ್_local_ಎಣಿಕೆ(ಪೂರ್ಣಾಂಕ addr_type, ಪೂರ್ಣಾಂಕ lnum, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
    buf_T   *buf;
    buf_T   *nextbuf;
    ಪೂರ್ಣಾಂಕ     ಎಣಿಕೆ_2 = ಸ್ಥಾನಾಂತರ;

    buf = firstbuf;
    ಆಗ (buf->b_ಮುಂದಿನ != ಖಾಲಿ && buf->b_fnum < lnum)
	buf = buf->b_ಮುಂದಿನ;
    ಆಗ (ಎಣಿಕೆ_2 != 0)
    {
	ಎಣಿಕೆ_2 += (ಸ್ಥಾನಾಂತರ < 0) ? 1 : -1;
	nextbuf = (ಸ್ಥಾನಾಂತರ < 0) ? buf->b_prev : buf->b_ಮುಂದಿನ;
	ಆದರೆ (nextbuf == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	buf = nextbuf;
	ಆದರೆ (addr_type == ADDR_LOADED_BUFFERS)
	    // skip over unloaded buffers
	    ಆಗ (buf->b_ml.ml_mfp == ಖಾಲಿ)
	    {
		nextbuf = (ಸ್ಥಾನಾಂತರ < 0) ? buf->b_prev : buf->b_ಮುಂದಿನ;
		ಆದರೆ (nextbuf == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
		buf = nextbuf;
	    }
    }
    // we might have gone too far, last buffer is not loadedd
    ಆದರೆ (addr_type == ADDR_LOADED_BUFFERS)
	ಆಗ (buf->b_ml.ml_mfp == ಖಾಲಿ)
	{
	    nextbuf = (ಸ್ಥಾನಾಂತರ >= 0) ? buf->b_prev : buf->b_ಮುಂದಿನ;
	    ಆದರೆ (nextbuf == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    buf = nextbuf;
	}
    ಹಿಂತಿರುಗಿ buf->b_fnum;
}

/*
 * Return the window number of "win".
 * When "win" is NULL return the number of windows.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
current_win_nr(win_T *win)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		nr = 0;

    FOR_ALL_WINDOWS(wp)
    {
	++nr;
	ಆದರೆ (wp == win)
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ nr;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
current_tab_nr(tabpage_T *tab)
{
    tabpage_T	*tp;
    ಪೂರ್ಣಾಂಕ		nr = 0;

    FOR_ALL_TABPAGES(tp)
    {
	++nr;
	ಆದರೆ (tp == tab)
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ nr;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
comment_ಆರಂಭ(char_u *p, ಪೂರ್ಣಾಂಕ starts_with_colon ಬಳಸದ)
{
    ಆದರೆ (ಒಳ_vim9script())
	ಹಿಂತಿರುಗಿ p[0] == '#' && !starts_with_colon;
    ಹಿಂತಿರುಗಿ *p == '"';
}

#ವ್ಯಾಖ್ಯಾನಿಸು CURRENT_WIN_NR current_win_nr(curwin)
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೊನೆಯ_WIN_NR current_win_nr(ಖಾಲಿ)
#ವ್ಯಾಖ್ಯಾನಿಸು CURRENT_TAB_NR current_tab_nr(curtab)
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೊನೆಯ_TAB_NR current_tab_nr(ಖಾಲಿ)

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
    ಸ್ಥಿರ char_u *
do_one_cmd(
    char_u	**cmdlinep,
    ಪೂರ್ಣಾಂಕ		flags,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    cstack_T	*cstack,
#ಆದರೆ_ಅಂತ್ಯ
    char_u	*(*fgetline)(ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, getline_opt_T),
    ಶೂನ್ಯ	*cookie)		// argument for fgetline()
{
    char_u	*p;
    linenr_T	lnum;
    ದೀರ್ಘ	n;
    ಅಕ್ಷರ	*errormsg = ಖಾಲಿ;	// error message
    char_u	*after_modifier = ಖಾಲಿ;
    exarg_T	ea;			// Ex command arguments
    cmdmod_T	save_cmdmod;
    ಪೂರ್ಣಾಂಕ		save_reg_executing = reg_executing;
    ಪೂರ್ಣಾಂಕ		save_pending_ಕೊನೆ_reg_executing = pending_ಕೊನೆ_reg_executing;
    ಪೂರ್ಣಾಂಕ		ni;			// set when Not Implemented
    char_u	*cmd;
    ಪೂರ್ಣಾಂಕ		starts_with_colon = FALSE;
    ಪೂರ್ಣಾಂಕ		may_have_range;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಪೂರ್ಣಾಂಕ		did_ಗಣ_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		sourcing = flags & DOCMD_VERBOSE;
    ಪೂರ್ಣಾಂಕ		did_append_cmd = FALSE;

    CLEAR_ಕ್ಷೇತ್ರ(ea);
    ea.line1 = 1;
    ea.line2 = 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ++ex_nesting_level;
#ಆದರೆ_ಅಂತ್ಯ

    // When the last file has not been edited :q has to be typed twice.
    ಆದರೆ (quitmore
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    // avoid that a function call in 'statusline' does this
	    && !getline_equal(fgetline, cookie, get_func_ಸಾಲು)
#ಆದರೆ_ಅಂತ್ಯ
	    // avoid that an autocommand, e.g. QuitPre, does this
	    && !getline_equal(fgetline, cookie, getnextac))
	--quitmore;

    /*
     * Reset browse, confirm, etc..  They are restored when returning, for
     * recursive calls.
     */
    save_cmdmod = cmdmod;

    // "#!anything" is handled like a comment.
    ಆದರೆ ((*cmdlinep)[0] == '#' && (*cmdlinep)[1] == '!')
	ಹೋಗು doend;

/*
 * 1. Skip comment lines and leading white space and colons.
 * 2. Handle command modifiers.
 */
    // The "ea" structure holds the arguments that can be used.
    ea.cmd = *cmdlinep;
    ea.cmdlinep = cmdlinep;
    ea.ea_getline = fgetline;
    ea.cookie = cookie;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ea.cstack = cstack;
    starts_with_colon = *skipwhite(ea.cmd) == ':';
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (parse_command_modifiers(&ea, &errormsg, &cmdmod, FALSE) == FAIL)
	ಹೋಗು doend;
    apply_cmdmod(&cmdmod);
    after_modifier = ea.cmd;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ea.skip = did_emsg || got_int || did_throw || (cstack->cs_idx >= 0
			 && !(cstack->cs_flags[cstack->cs_idx] & CSF_ಸಕ್ರಿಯ));
#ಇಲ್ಲದಿದ್ದರೆ
    ea.skip = (if_level > 0);
#ಆದರೆ_ಅಂತ್ಯ

/*
 * 3. Skip over the range to find the command.  Let "p" point to after it.
 *
 * We need the command to know what kind of range it uses.
 */
    cmd = ea.cmd;

    // In Vim9 script a colon is required before the range.  This may also be
    // after command modifiers.
    ಪೂರ್ಣಾಂಕ vim9script = ಒಳ_vim9script();
    ಆದರೆ (vim9script && (flags & DOCMD_RANGEOK) == 0)
    {
	may_have_range = FALSE;
	ಗಾಗಿ (p = ea.cmd; p >= *cmdlinep; --p)
	{
	    ಆದರೆ (*p == ':')
		may_have_range = TRUE;
	    ಆದರೆ (p < ea.cmd && !VIM_ISWHITE(*p))
		ನಿಲ್ಲಿಸು;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	may_have_range = TRUE;
    ಆದರೆ (may_have_range)
	ea.cmd = skip_range(ea.cmd, TRUE, ಖಾಲಿ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // Handle ":export" - it functions almost like a command modifier.
    // ":export var Name: type"
    // ":export def Name(..."
    // etc.
    ಆದರೆ (vim9script && checkforcmd_noparen(&ea.cmd, "export", 6))
	is_export = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (vim9script && !may_have_range)
    {
	ಆದರೆ (ea.cmd == cmd + 1 && *cmd == '$')
	    // should be "$VAR = val"
	    --ea.cmd;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	p = ಕಂಡುಹಿಡಿ_ex_command(&ea, ಖಾಲಿ, lookup_scriptitem, ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
	p = ಕಂಡುಹಿಡಿ_ex_command(&ea, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (ea.cmdidx == CMD_ಗಾತ್ರ)
	{
	    char_u *ar = skip_range(ea.cmd, TRUE, ಖಾಲಿ);

	    // If a ':' before the range is missing, give a clearer error
	    // message.
	    ಆದರೆ (ar > ea.cmd && !ea.skip)
	    {
		semsg(_(e_colon_required_before_range_str), ea.cmd);
		ಹೋಗು doend;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	p = ಕಂಡುಹಿಡಿ_ex_command(&ea, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
# ifdef FEAT_PROFILE
    // Count this line for profiling if skip is TRUE.
    ಆದರೆ (do_profiling == PROF_YES
	    && (!ea.skip || cstack->cs_idx == 0 || (cstack->cs_idx > 0
		     && (cstack->cs_flags[cstack->cs_idx - 1] & CSF_ಸಕ್ರಿಯ))))
    {
	ಪೂರ್ಣಾಂಕ skip = did_emsg || got_int || did_throw;

	ಆದರೆ (ea.cmdidx == CMD_catch)
	    skip = !skip && !(cstack->cs_idx >= 0
			  && (cstack->cs_flags[cstack->cs_idx] & CSF_THROWN)
			  && !(cstack->cs_flags[cstack->cs_idx] & CSF_CAUGHT));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ea.cmdidx == CMD_else || ea.cmdidx == CMD_elseif)
	    skip = skip || !(cstack->cs_idx >= 0
			  && !(cstack->cs_flags[cstack->cs_idx]
						  & (CSF_ಸಕ್ರಿಯ | CSF_TRUE)));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ea.cmdidx == CMD_finally)
	    skip = FALSE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endtry
		&& ea.cmdidx != CMD_endwhile)
	    skip = ea.skip;

	ಆದರೆ (!skip)
	{
	    ಆದರೆ (getline_equal(fgetline, cookie, get_func_ಸಾಲು))
		func_ಸಾಲು_exec(getline_cookie(fgetline, cookie));
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (getline_equal(fgetline, cookie, getsourceline))
		script_ಸಾಲು_exec();
	}
    }
# endif
#ಆದರೆ_ಅಂತ್ಯ

    ea.cmd = cmd;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // May go to debug mode.  If this happens and the ">quit" debug command is
    // used, throw an interrupt exception and skip the next command.
    dbg_check_breakpoint(&ea);
    ಆದರೆ (!ea.skip && got_int)
    {
	ea.skip = TRUE;
	(ಶೂನ್ಯ)do_intthrow(cstack);
    }
#ಆದರೆ_ಅಂತ್ಯ

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
    ಆದರೆ (!IS_USER_CMDIDX(ea.cmdidx))
    {
	ಆದರೆ (ea.cmdidx != CMD_ಗಾತ್ರ)
	    ea.addr_type = cmdnames[(ಪೂರ್ಣಾಂಕ)ea.cmdidx].cmd_addr_type;
	ಇಲ್ಲದಿದ್ದರೆ
	    ea.addr_type = ADDR_ಸಾಲುಗಳು;

	// :wincmd range depends on the argument.
	ಆದರೆ (ea.cmdidx == CMD_wincmd && p != ಖಾಲಿ)
	    get_wincmd_addr_type(skipwhite(p), &ea);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
	// :.cc in quickfix window uses line number
	ಆದರೆ ((ea.cmdidx == CMD_cc || ea.cmdidx == CMD_ll) && bt_quickfix(curbuf))
	    ea.addr_type = ADDR_OTHER;
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ (!may_have_range)
	ea.line1 = ea.line2 = default_ವಿಳಾಸ(&ea);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (parse_cmd_ವಿಳಾಸ(&ea, &errormsg, FALSE) == FAIL)
	ಹೋಗು doend;

/*
 * 5. Parse the command.
 */

    /*
     * Skip ':' and any white space
     */
    ea.cmd = skipwhite(ea.cmd);
    ಆಗ (*ea.cmd == ':')
	ea.cmd = skipwhite(ea.cmd + 1);

    /*
     * If we got a line, but no command, then go to the line.
     * If we find a '|' or '\n' we set ea.nextcmd.
     */
    ಆದರೆ (*ea.cmd == NUL || comment_ಆರಂಭ(ea.cmd, starts_with_colon)
			       || (ea.nextcmd = check_nextcmd(ea.cmd)) != ಖಾಲಿ)
    {
	/*
	 * strange vi behaviour:
	 * ":3"		jumps to line 3
	 * ":3|..."	prints line 3  (not in Vim9 script)
	 * ":|"		prints current line  (not in Vim9 script)
	 */
	ಆದರೆ (ea.skip)	    // skip this if inside :if
	    ಹೋಗು doend;
	errormsg = ex_range_without_command(&ea);
	ಹೋಗು doend;
    }

    // If this looks like an undefined user command and there are CmdUndefined
    // autocommands defined, trigger the matching autocommands.
    ಆದರೆ (p != ಖಾಲಿ && ea.cmdidx == CMD_ಗಾತ್ರ && !ea.skip
	    && ASCII_ISUPPER(*ea.cmd)
	    && has_cmdundefined())
    {
	ಪೂರ್ಣಾಂಕ ret;

	p = ea.cmd;
	ಆಗ (ASCII_ISALNUM(*p))
	    ++p;
	p = vim_strnsave(ea.cmd, p - ea.cmd);
	ret = apply_autocmds(EVENT_CMDUNDEFINED, p, p, TRUE, ಖಾಲಿ);
	vim_free(p);
	// If the autocommands did something and didn't cause an error, try
	// finding the command again.
	p = (ret
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		&& !aborting()
#ಆದರೆ_ಅಂತ್ಯ
		) ? ಕಂಡುಹಿಡಿ_ex_command(&ea, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ) : ea.cmd;
    }

    ಆದರೆ (p == ಖಾಲಿ)
    {
	ಆದರೆ (!ea.skip)
	    errormsg = _(e_ambiguous_use_ದ_user_defined_command);
	ಹೋಗು doend;
    }
    // Check for wrong commands.
    ಆದರೆ (*p == '!' && ea.cmd[1] == 0151 && ea.cmd[0] == 78
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	errormsg = uc_fun_cmd();
	ಹೋಗು doend;
    }

    ಆದರೆ (ea.cmdidx == CMD_ಗಾತ್ರ)
    {
	ಆದರೆ (!ea.skip)
	{
	    STRCPY(IObuff, _(e_not_an_editor_command));
	    ಆದರೆ (!sourcing)
	    {
		// If the modifier was parsed OK the error must be in the
		// following command
		ಆದರೆ (after_modifier != ಖಾಲಿ)
		    append_command(after_modifier);
		ಇಲ್ಲದಿದ್ದರೆ
		    append_command(*cmdlinep);
		did_append_cmd = TRUE;
	    }
	    errormsg = (ಅಕ್ಷರ *)IObuff;
	    did_emsg_syntax = TRUE;
	}
	ಹೋಗು doend;
    }

    ni = (!IS_USER_CMDIDX(ea.cmdidx)
	    && (cmdnames[ea.cmdidx].cmd_func == ex_ni
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_EX_SCRIPT_NI
	     || cmdnames[ea.cmdidx].cmd_func == ex_script_ni
#ಆದರೆ_ಅಂತ್ಯ
	     ));

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_EVAL
    /*
     * When the expression evaluation is disabled, recognize the ":if" and
     * ":endif" commands and ignore everything in between it.
     */
    ಆದರೆ (ea.cmdidx == CMD_if)
	++if_level;
    ಆದರೆ (if_level)
    {
	ಆದರೆ (ea.cmdidx == CMD_endif)
	    --if_level;
	ಹೋಗು doend;
    }

#ಆದರೆ_ಅಂತ್ಯ

    // forced commands
    ಆದರೆ (*p == '!' && ea.cmdidx != CMD_substitute
	    && ea.cmdidx != CMD_smagic && ea.cmdidx != CMD_snomagic)
    {
	++p;
	ea.forceit = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ea.forceit = FALSE;

/*
 * 6. Parse arguments.  Then check for errors.
 */
    ಆದರೆ (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = (ದೀರ್ಘ)cmdnames[(ಪೂರ್ಣಾಂಕ)ea.cmdidx].cmd_argt;

    ಆದರೆ (!ea.skip)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
	ಆದರೆ (sandbox != 0 && !(ea.argt & EX_SBOXOK))
	{
	    // Command not allowed in sandbox.
	    errormsg = _(e_not_allowed_ಒಳ_sandbox);
	    ಹೋಗು doend;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (restricted != 0 && (ea.argt & EX_RESTRICT))
	{
	    errormsg = _(e_command_not_allowed_ಒಳ_rvim);
	    ಹೋಗು doend;
	}
	ಆದರೆ (!curbuf->b_p_ma && (ea.argt & EX_MODIFY))
	{
	    // Command not allowed in non-'modifiable' buffer
	    errormsg = _(e_cannot_make_changes_modifiable_is_off);
	    ಹೋಗು doend;
	}

	ಆದರೆ (!IS_USER_CMDIDX(ea.cmdidx))
	{
	    ಆದರೆ (cmdwin_type != 0 && !(ea.argt & EX_CMDWIN))
	    {
		// Command not allowed in the command line window
		errormsg = _(e_invalid_ಒಳ_cmdline_ಕಿಟಕಿ);
		ಹೋಗು doend;
	    }
	    ಆದರೆ (ಪಠ್ಯ_locked() && !(ea.argt & EX_ಬೀಗ_OK))
	    {
		// Command not allowed when text is locked
		errormsg = _(get_ಪಠ್ಯ_locked_ಸಂದೇಶ());
		ಹೋಗು doend;
	    }
	}

	// Disallow editing another buffer when "curbuf_lock" is set.
	// Do allow ":checktime" (it is postponed).
	// Do allow ":edit" (check for an argument later).
	// Do allow ":file" with no arguments (check for an argument later).
	ಆದರೆ (!(ea.argt & (EX_CMDWIN | EX_ಬೀಗ_OK))
		&& ea.cmdidx != CMD_checktime
		&& ea.cmdidx != CMD_edit
		&& ea.cmdidx != CMD_file
		&& !IS_USER_CMDIDX(ea.cmdidx)
		&& curbuf_locked())
	    ಹೋಗು doend;

	ಆದರೆ (!ni && !(ea.argt & EX_RANGE) && ea.addr_ಎಣಿಕೆ > 0)
	{
	    errormsg = _(e_no_range_allowed);
	    ಹೋಗು doend;
	}
    }

    ಆದರೆ (!ni && !(ea.argt & EX_BANG) && ea.forceit)
    {
	errormsg = _(e_no_bang_allowed);
	ಹೋಗು doend;
    }

    /*
     * Don't complain about the range if it is not used
     * (could happen if line_count is accidentally set to 0).
     */
    ಆದರೆ (!ea.skip && !ni && (ea.argt & EX_RANGE))
    {
	/*
	 * If the range is backwards, ask for confirmation and, if given, swap
	 * ea.line1 & ea.line2 so it's forwards again.
	 * When global command is busy, don't ask, will fail below.
	 */
	ಆದರೆ (!global_busy && ea.line1 > ea.line2)
	{
	    ಆದರೆ (ಸಂದೇಶ_silent == 0)
	    {
		ಆದರೆ (sourcing || exmode_ಸಕ್ರಿಯ)
		{
		    errormsg = _(e_backwards_range_given);
		    ಹೋಗು doend;
		}
		ಆದರೆ (ask_yesno((char_u *)
			_("Backwards range given, OK to swap"), FALSE) != 'y')
		    ಹೋಗು doend;
	    }
	    lnum = ea.line1;
	    ea.line1 = ea.line2;
	    ea.line2 = lnum;
	}
	ಆದರೆ ((errormsg = invalid_range(&ea)) != ಖಾಲಿ)
	    ಹೋಗು doend;
    }

    ಆದರೆ ((ea.addr_type == ADDR_OTHER) && ea.addr_ಎಣಿಕೆ == 0)
	// default is 1, not cursor
	ea.line2 = 1;

    correct_range(&ea);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ (((ea.argt & EX_WHOLEFOLD) || ea.addr_ಎಣಿಕೆ >= 2) && !global_busy
	    && ea.addr_type == ADDR_ಸಾಲುಗಳು)
    {
	// Put the first line at the start of a closed fold, put the last line
	// at the end of a closed fold.
	(ಶೂನ್ಯ)hasFolding(ea.line1, &ea.line1, ಖಾಲಿ);
	(ಶೂನ್ಯ)hasFolding(ea.line2, ಖಾಲಿ, &ea.line2);
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    /*
     * For the ":make" and ":grep" commands we insert the 'makeprg'/'grepprg'
     * option here, so things like % get expanded.
     */
    p = replace_makeprg(&ea, p, cmdlinep);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹೋಗು doend;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Skip to start of argument.
     * Don't do this for the ":!" command, because ":!! -l" needs the space.
     */
    ಆದರೆ (ea.cmdidx == CMD_bang)
	ea.arg = p;
    ಇಲ್ಲದಿದ್ದರೆ
	ea.arg = skipwhite(p);

    // ":file" cannot be run with an argument when "curbuf_lock" is set
    ಆದರೆ (ea.cmdidx == CMD_file && *ea.arg != NUL && curbuf_locked())
	ಹೋಗು doend;

    /*
     * Check for "++opt=val" argument.
     * Must be first, allow ":w ++enc=utf8 !cmd"
     */
    ಆದರೆ (ea.argt & EX_ARGOPT)
	ಆಗ (ea.arg[0] == '+' && ea.arg[1] == '+')
	    ಆದರೆ (getargopt(&ea) == FAIL && !ni)
	    {
		errormsg = _(e_invalid_ವಾದ);
		ಹೋಗು doend;
	    }

    ಆದರೆ (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	ಆದರೆ (*ea.arg == '>')			// append
	{
	    ಆದರೆ (*++ea.arg != '>')		// typed wrong
	    {
		errormsg = _(e_use_w_or_w_gt_gt);
		ಹೋಗು doend;
	    }
	    ea.arg = skipwhite(ea.arg + 1);
	    ea.append = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ea.arg == '!' && ea.cmdidx == CMD_write)  // :w !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    ಆದರೆ (ea.cmdidx == CMD_read)
    {
	ಆದರೆ (ea.forceit)
	{
	    ea.usefilter = TRUE;		// :r! filter if ea.forceit
	    ea.forceit = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ea.arg == '!')		// :r !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    ಆದರೆ (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ea.amount = 1;
	ಆಗ (*ea.arg == *ea.cmd)		// count number of '>' or '<'
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
    ಆದರೆ ((ea.argt & EX_CMDARG) && !ea.usefilter)
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
    ಆದರೆ ((ea.argt & EX_TRLBAR) && !ea.usefilter)
    {
	separate_nextcmd(&ea, FALSE);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ea.cmdidx == CMD_bang
	    || ea.cmdidx == CMD_terminal
	    || ea.cmdidx == CMD_global
	    || ea.cmdidx == CMD_vglobal
	    || ea.usefilter
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    || inside_ಖಂಡ(&ea)
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
	ಗಾಗಿ (p = ea.arg; *p; ++p)
	{
	    // Remove one backslash before a newline
	    ಆದರೆ (*p == '\\' && p[1] == '\n')
		STRMOVE(p, p + 1);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '\n' && !(ea.argt & EX_ಅಭಿವ್ಯಕ್ತಿ_ARG))
	    {
		ea.nextcmd = p + 1;
		*p = NUL;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    ಆದರೆ ((ea.argt & EX_DFLALL) && ea.addr_ಎಣಿಕೆ == 0)
	ವಿಳಾಸ_default_all(&ea);

    // accept numbered register only when no count allowed (:put)
    ಆದರೆ (       (ea.argt & EX_REGSTR)
	    && *ea.arg != NUL
	       // Do not allow register = for user commands
	    && (!IS_USER_CMDIDX(ea.cmdidx) || *ea.arg != '=')
	    && !((ea.argt & EX_ಎಣಿಕೆ) && VIM_ISDIGIT(*ea.arg)))
    {
#ಆದರೆ !defined(FEAT_CLIPBOARD) && !defined(FEAT_CLIPBOARD_PROVIDER)
	// check these explicitly for a more specific error message
	ಆದರೆ (*ea.arg == '*' || *ea.arg == '+')
	{
	    errormsg = _(e_invalid_ರಿಜಿಸ್ಟರ್_name);
	    ಹೋಗು doend;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (valid_yank_reg(*ea.arg, (!IS_USER_CMDIDX(ea.cmdidx)
			    && ea.cmdidx != CMD_put && ea.cmdidx != CMD_iput)))
	{
	    ea.regname = *ea.arg++;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    // for '=' register: accept the rest of the line as an expression
	    ಆದರೆ (ea.arg[-1] == '=' && ea.arg[0] != NUL)
	    {
		ಆದರೆ (!ea.skip)
		{
		    ಗಣ_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು(vim_strsave(ea.arg), &ea);
		    did_ಗಣ_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು = TRUE;
		}
		ea.arg += STRLEN(ea.arg);
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ea.arg = skipwhite(ea.arg);
	}
    }

    /*
     * Check for a count.  When accepting a EX_BUFNAME, don't use "123foo" as a
     * count, it's a buffer name.
     */
    ಆದರೆ ((ea.argt & EX_ಎಣಿಕೆ) && VIM_ISDIGIT(*ea.arg)
	    && (!(ea.argt & EX_BUFNAME) || *(p = skipdigits(ea.arg + 1)) == NUL
							  || VIM_ISWHITE(*p)))
    {
	n = getdigits_quoted(&ea.arg);
	ea.arg = skipwhite(ea.arg);
	ಆದರೆ (n <= 0 && !ni && (ea.argt & EX_ZEROR) == 0)
	{
	    errormsg = _(e_ಧನಾತ್ಮಕ_ಎಣಿಕೆ_required);
	    ಹೋಗು doend;
	}
	ಆದರೆ (ea.addr_type != ADDR_ಸಾಲುಗಳು)	// e.g. :buffer 2, :sleep 3
	{
	    ea.line2 = n;
	    ಆದರೆ (ea.addr_ಎಣಿಕೆ == 0)
		ea.addr_ಎಣಿಕೆ = 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ea.line1 = ea.line2;
	    ಆದರೆ (ea.line2 >= LONG_MAX - (n - 1))
		ea.line2 = LONG_MAX;  // avoid overflow
	    ಇಲ್ಲದಿದ್ದರೆ
		ea.line2 += n - 1;
	    ++ea.addr_ಎಣಿಕೆ;
	    /*
	     * Be vi compatible: no error message for out of range.
	     */
	    ಆದರೆ (ea.line2 > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ea.line2 = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	}
    }

    /*
     * Check for flags: 'l', 'p' and '#'.
     */
    ಆದರೆ (ea.argt & EX_FLAGS)
	get_flags(&ea);
    ಆದರೆ (!ni && !(ea.argt & EX_EXTRA) && *ea.arg != NUL
	    && *ea.arg != '"' && (*ea.arg != '|' || (ea.argt & EX_TRLBAR) == 0))
    {
	// no arguments allowed but there is something
	errormsg = ex_errmsg(e_trailing_characters_str, ea.arg);
	ಹೋಗು doend;
    }

    ಆದರೆ (!ni && (ea.argt & EX_NEEDARG) && *ea.arg == NUL)
    {
	errormsg = _(e_ವಾದ_required);
	ಹೋಗು doend;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    /*
     * Skip the command when it's not going to be executed.
     * The commands like :if, :endif, etc. always need to be executed.
     * Also make an exception for commands that handle a trailing command
     * themselves.
     */
    ಆದರೆ (ea.skip)
    {
	ಆಯ್ಕೆ (ea.cmdidx)
	{
	    // commands that need evaluation
	    ಸ್ಥಿತಿ CMD_while:
	    ಸ್ಥಿತಿ CMD_endwhile:
	    ಸ್ಥಿತಿ CMD_for:
	    ಸ್ಥಿತಿ CMD_endfor:
	    ಸ್ಥಿತಿ CMD_if:
	    ಸ್ಥಿತಿ CMD_elseif:
	    ಸ್ಥಿತಿ CMD_else:
	    ಸ್ಥಿತಿ CMD_endif:
	    ಸ್ಥಿತಿ CMD_try:
	    ಸ್ಥಿತಿ CMD_catch:
	    ಸ್ಥಿತಿ CMD_finally:
	    ಸ್ಥಿತಿ CMD_endtry:
	    ಸ್ಥಿತಿ CMD_function:
	    ಸ್ಥಿತಿ CMD_def:
				ನಿಲ್ಲಿಸು;

	    // commands that read a block of lines
	    ಸ್ಥಿತಿ CMD_abstract:
	    ಸ್ಥಿತಿ CMD_append:
	    ಸ್ಥಿತಿ CMD_autocmd:
	    ಸ್ಥಿತಿ CMD_change:
	    ಸ್ಥಿತಿ CMD_class:
	    ಸ್ಥಿತಿ CMD_command:
	    ಸ್ಥಿತಿ CMD_enum:
	    ಸ್ಥಿತಿ CMD_ಸೇರಿಸು:
	    ಸ್ಥಿತಿ CMD_interface:
	    ಸ್ಥಿತಿ CMD_loadkeymap:
				ನಿಲ್ಲಿಸು;

	    // Commands that handle '|' themselves.  Check: A command should
	    // either have the EX_TRLBAR flag, appear in this list or appear in
	    // the list at ":help :bar".
	    ಸ್ಥಿತಿ CMD_aboveleft:
	    ಸ್ಥಿತಿ CMD_and:
	    ಸ್ಥಿತಿ CMD_belowright:
	    ಸ್ಥಿತಿ CMD_botright:
	    ಸ್ಥಿತಿ CMD_browse:
	    ಸ್ಥಿತಿ CMD_call:
	    ಸ್ಥಿತಿ CMD_confirm:
	    ಸ್ಥಿತಿ CMD_const:
	    ಸ್ಥಿತಿ CMD_delfunction:
	    ಸ್ಥಿತಿ CMD_djump:
	    ಸ್ಥಿತಿ CMD_dlist:
	    ಸ್ಥಿತಿ CMD_dsearch:
	    ಸ್ಥಿತಿ CMD_dsplit:
	    ಸ್ಥಿತಿ CMD_echo:
	    ಸ್ಥಿತಿ CMD_echoerr:
	    ಸ್ಥಿತಿ CMD_echomsg:
	    ಸ್ಥಿತಿ CMD_echon:
	    ಸ್ಥಿತಿ CMD_eval:
	    ಸ್ಥಿತಿ CMD_execute:
	    ಸ್ಥಿತಿ CMD_filter:
	    ಸ್ಥಿತಿ CMD_final:
	    ಸ್ಥಿತಿ CMD_help:
	    ಸ್ಥಿತಿ CMD_hide:
	    ಸ್ಥಿತಿ CMD_horizontal:
	    ಸ್ಥಿತಿ CMD_ijump:
	    ಸ್ಥಿತಿ CMD_ilist:
	    ಸ್ಥಿತಿ CMD_isearch:
	    ಸ್ಥಿತಿ CMD_isplit:
	    ಸ್ಥಿತಿ CMD_keepalt:
	    ಸ್ಥಿತಿ CMD_keepjumps:
	    ಸ್ಥಿತಿ CMD_keepmarks:
	    ಸ್ಥಿತಿ CMD_keeppatterns:
	    ಸ್ಥಿತಿ CMD_leftabove:
	    ಸ್ಥಿತಿ CMD_let:
	    ಸ್ಥಿತಿ CMD_lockmarks:
	    ಸ್ಥಿತಿ CMD_lockvar:
	    ಸ್ಥಿತಿ CMD_lua:
	    ಸ್ಥಿತಿ CMD_ಹೊಂದಿಕೆ:
	    ಸ್ಥಿತಿ CMD_mzscheme:
	    ಸ್ಥಿತಿ CMD_noautocmd:
	    ಸ್ಥಿತಿ CMD_noswapfile:
	    ಸ್ಥಿತಿ CMD_perl:
	    ಸ್ಥಿತಿ CMD_psearch:
	    ಸ್ಥಿತಿ CMD_py3:
	    ಸ್ಥಿತಿ CMD_python3:
	    ಸ್ಥಿತಿ CMD_python:
	    ಸ್ಥಿತಿ CMD_return:
	    ಸ್ಥಿತಿ CMD_rightbelow:
	    ಸ್ಥಿತಿ CMD_ruby:
	    ಸ್ಥಿತಿ CMD_silent:
	    ಸ್ಥಿತಿ CMD_smagic:
	    ಸ್ಥಿತಿ CMD_snomagic:
	    ಸ್ಥಿತಿ CMD_substitute:
	    ಸ್ಥಿತಿ CMD_syntax:
	    ಸ್ಥಿತಿ CMD_tab:
	    ಸ್ಥಿತಿ CMD_tcl:
	    ಸ್ಥಿತಿ CMD_throw:
	    ಸ್ಥಿತಿ CMD_tilde:
	    ಸ್ಥಿತಿ CMD_topleft:
	    ಸ್ಥಿತಿ CMD_unlet:
	    ಸ್ಥಿತಿ CMD_unlockvar:
	    ಸ್ಥಿತಿ CMD_var:
	    ಸ್ಥಿತಿ CMD_verbose:
	    ಸ್ಥಿತಿ CMD_vertical:
	    ಸ್ಥಿತಿ CMD_wincmd:
				ನಿಲ್ಲಿಸು;

	    ಪೂರ್ವನಿಯೋಜಿತ:		ಹೋಗು doend;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ ((ea.argt & EX_XFILE)
	    && expand_filename(&ea, cmdlinep, &errormsg) == FAIL)
	ಹೋಗು doend;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (is_export && (ea.argt & EX_EXPORT) == 0)
    {
	emsg(_(e_invalid_command_after_export));
	ಹೋಗು doend;
    }
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Accept buffer name.  Cannot be used at the same time with a buffer
     * number.  Don't do this for a user command.
     */
    ಆದರೆ ((ea.argt & EX_BUFNAME) && *ea.arg != NUL && ea.addr_ಎಣಿಕೆ == 0
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * :bdelete, :bwipeout and :bunload take several arguments, separated
	 * by spaces: find next space (skipping over escaped characters).
	 * The others take one argument: ignore trailing spaces.
	 */
	ಆದರೆ (ea.cmdidx == CMD_bdelete || ea.cmdidx == CMD_bwipeout
						  || ea.cmdidx == CMD_bunload)
	    p = skiptowhite_esc(ea.arg);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    p = ea.arg + STRLEN(ea.arg);
	    ಆಗ (p > ea.arg && VIM_ISWHITE(p[-1]))
		--p;
	}
	ea.line2 = buflist_findpat(ea.arg, p, (ea.argt & EX_BUFUNL) != 0,
								FALSE, FALSE);
	ಆದರೆ (ea.line2 < 0)	    // failed
	    ಹೋಗು doend;
	ea.addr_ಎಣಿಕೆ = 1;
	ea.arg = skipwhite(p);
    }

    // The :try command saves the emsg_silent flag, reset it here when
    // ":silent! try" was used, it should only apply to :try itself.
    ಆದರೆ (ea.cmdidx == CMD_try && cmdmod.cmod_did_esilent > 0)
    {
	emsg_silent -= cmdmod.cmod_did_esilent;
	ಆದರೆ (emsg_silent < 0)
	    emsg_silent = 0;
	cmdmod.cmod_did_esilent = 0;
    }

/*
 * 7. Execute the command.
 */

    ಆದರೆ (IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * Execute a user-defined command.
	 */
	do_ucmd(&ea);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * Call the function to execute the builtin command.
	 */
	(cmdnames[ea.cmdidx].cmd_func)(&ea);
	ಆದರೆ (ea.errmsg != ಖಾಲಿ)
	    errormsg = ea.errmsg;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // A command will reset "is_export" when exporting an item.  If it is still
    // set something went wrong or the command was never executed.
    ಆದರೆ (!ea.skip && is_export)
    {
	ಆದರೆ (errormsg == ಖಾಲಿ)
	    errormsg = _(e_export_with_invalid_ವಾದ);
	is_export = FALSE;
    }

    // Set flag that any command was executed, used by ex_vim9script().
    // Not if this was a command that wasn't executed or :endif.
    ಆದರೆ (sourcing_a_script(&ea)
	    && current_sctx.sc_sid > 0
	    && ea.cmdidx != CMD_endif
	    && (cstack->cs_idx < 0
		    || (cstack->cs_flags[cstack->cs_idx] & CSF_ಸಕ್ರಿಯ)))
	SCRIPT_ಅಂಶ(current_sctx.sc_sid)->sn_state = SN_STATE_HAD_COMMAND;

    /*
     * If the command just executed called do_cmdline(), any throw or ":return"
     * or ":finish" encountered there must also check the cstack of the still
     * active do_cmdline() that called this do_one_cmd().  Rethrow an uncaught
     * exception, or reanimate a returned function or finished script file and
     * return or finish it again.
     */
    ಆದರೆ (need_rethrow)
	do_throw(cstack);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (check_cstack)
    {
	ಆದರೆ (ಮೂಲ_ಪೂರ್ಣಗೊಂಡ(fgetline, cookie))
	    do_ಮುಕ್ತಾಯ(&ea, TRUE);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (getline_equal(fgetline, cookie, get_func_ಸಾಲು)
						   && current_func_returned())
	    do_return(&ea, TRUE, FALSE, ಖಾಲಿ);
    }
    need_rethrow = check_cstack = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

doend:
    ಆದರೆ (curwin->w_cursor.lnum == 0)	// can happen with zero line number
    {
	curwin->w_cursor.lnum = 1;
	curwin->w_cursor.ಲಂಬಸಾಲು = 0;
    }

    ಆದರೆ (errormsg != ಖಾಲಿ && *errormsg != NUL && !did_emsg)
    {
	ಆದರೆ ((sourcing || !ಕೀಲಿ_Typed) && !did_append_cmd)
	{
	    ಆದರೆ (errormsg != (ಅಕ್ಷರ *)IObuff)
	    {
		STRCPY(IObuff, errormsg);
		errormsg = (ಅಕ್ಷರ *)IObuff;
	    }
	    append_command(*cmdlinep);
	}
	emsg(errormsg);
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    do_errthrow(cstack,
	    (ea.cmdidx != CMD_ಗಾತ್ರ && !IS_USER_CMDIDX(ea.cmdidx))
			? cmdnames[(ಪೂರ್ಣಾಂಕ)ea.cmdidx].cmd_name : (char_u *)ಖಾಲಿ);

    ಆದರೆ (did_ಗಣ_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು)
	ಗಣ_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು(ಖಾಲಿ, ಖಾಲಿ);
    is_export = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    undo_cmdmod(&cmdmod);
    cmdmod = save_cmdmod;
    reg_executing = save_reg_executing;
    pending_ಕೊನೆ_reg_executing = save_pending_ಕೊನೆ_reg_executing;

    ಆದರೆ (ea.nextcmd && *ea.nextcmd == NUL)	// not really a next command
	ea.nextcmd = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    --ex_nesting_level;
    vim_free(ea.cmdline_tofree);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ea.nextcmd;
}

ಸ್ಥಿರ ಅಕ್ಷರ ex_error_buf[ಸಂದೇಶ_BUF_LEN];

/*
 * Return an error message with argument included.
 * Uses a static buffer, only the last error will be kept.
 * "msg" will be translated, caller should use N_().
 */
    ಅಕ್ಷರ *
ex_errmsg(ಅಕ್ಷರ *ಸಂದೇಶ, char_u *arg)
{
    vim_snprintf(ex_error_buf, ಸಂದೇಶ_BUF_LEN, _(ಸಂದೇಶ), arg);
    ಹಿಂತಿರುಗಿ ex_error_buf;
}

/*
 * The "+" string used in place of an empty command in Ex mode.
 * This string is used in pointer comparison.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ exmode_plus[] = "+";

/*
 * Handle a range without a command.
 * Returns an error message on failure.
 */
    ಅಕ್ಷರ *
ex_range_without_command(exarg_T *eap)
{
    ಅಕ್ಷರ *errormsg = ಖಾಲಿ;

    ಆದರೆ ((*eap->cmd == '|' ||
		(exmode_ಸಕ್ರಿಯ && eap->cmd != (char_u *)exmode_plus + 1))
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    && !ಒಳ_vim9script()
#ಆದರೆ_ಅಂತ್ಯ
       )
    {
	eap->cmdidx = CMD_print;
	eap->argt = EX_RANGE+EX_ಎಣಿಕೆ+EX_TRLBAR;
	ಆದರೆ ((errormsg = invalid_range(eap)) == ಖಾಲಿ)
	{
	    correct_range(eap);
	    ex_print(eap);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->addr_ಎಣಿಕೆ != 0)
    {
	ಆದರೆ (eap->line2 > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    // With '-' in 'cpoptions' a line number past the file is an
	    // error, otherwise put it at the end of the file.
	    ಆದರೆ (vim_strchr(p_cpo, CPO_MINUS) != ಖಾಲಿ)
		eap->line2 = -1;
	    ಇಲ್ಲದಿದ್ದರೆ
		eap->line2 = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	}

	ಆದರೆ (eap->line2 < 0)
	    errormsg = _(e_invalid_range);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (eap->line2 == 0)
		curwin->w_cursor.lnum = 1;
	    ಇಲ್ಲದಿದ್ದರೆ
		curwin->w_cursor.lnum = eap->line2;
	    beginline(BL_SOL | BL_FIX);
	}
    }
    ಹಿಂತಿರುಗಿ errormsg;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 * If "noparen" is TRUE do not recognize the command followed by "(" or ".".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
checkforcmd_opt(
    char_u	**pp,		// start of command
    ಅಕ್ಷರ	*cmd,		// name of command
    ಪೂರ್ಣಾಂಕ		len,		// required length
    ಪೂರ್ಣಾಂಕ		noparen)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; cmd[i] != NUL; ++i)
	ಆದರೆ (((char_u *)cmd)[i] != (*pp)[i])
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (i >= len && !ASCII_ISALPHA((*pp)[i]) && (*pp)[i] != '_'
			 && (!noparen || ((*pp)[i] != '(' && (*pp)[i] != '.')))
    {
	*pp = skipwhite(*pp + i);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    ಪೂರ್ಣಾಂಕ
checkforcmd(
    char_u	**pp,		// start of command
    ಅಕ್ಷರ	*cmd,		// name of command
    ಪೂರ್ಣಾಂಕ		len)		// required length
{
    ಹಿಂತಿರುಗಿ checkforcmd_opt(pp, cmd, len, FALSE);
}

/*
 * Check for an Ex command with optional tail, not followed by "(" or ".".
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    ಪೂರ್ಣಾಂಕ
checkforcmd_noparen(
    char_u	**pp,		// start of command
    ಅಕ್ಷರ	*cmd,		// name of command
    ಪೂರ್ಣಾಂಕ		len)		// required length
{
    ಹಿಂತಿರುಗಿ checkforcmd_opt(pp, cmd, len, TRUE);
}

/*
 * Find the replacement text of a ":command", the part after the attributes and
 * the command name.  Returns NULL when there is none.  Does not modify "arg"
 * and gives no error messages.
 */
    ಸ್ಥಿರ char_u *
ಕಂಡುಹಿಡಿ_ucmd_repl(char_u *arg)
{
    char_u	*p = arg;

    // Skip over the attributes.
    ಆಗ (*p == '-')
	p = skipwhite(skiptowhite(p));

    // Skip over the command name.
    ಆದರೆ (!ASCII_ISALPHA(*p))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆಗ (ASCII_ISALNUM(*p))
	++p;

    ಹಿಂತಿರುಗಿ *p == NUL ? ಖಾಲಿ : skipwhite(p);
}

/*
 * Find the "{" in "line" that starts a block for ":command" or ":autocmd".
 * That is the case when the command argument is "{" at the end of the line,
 * also when the command is nested in another ":command" or ":autocmd".
 * Returns NULL when the line does not start such a block.
 */
    char_u *
ಕಂಡುಹಿಡಿ_cmd_ಖಂಡ_ಆರಂಭ(char_u *ಸಾಲು)
{
    char_u	*p = skipwhite(ಸಾಲು);

    ಗಾಗಿ (;;)
    {
	char_u	*arg = p;

	ಆದರೆ (*p == '{' && ತುದಿಗಳು_excmd2(p, skipwhite(p + 1)))
	    ಹಿಂತಿರುಗಿ p;

	ಆದರೆ (checkforcmd_noparen(&arg, "autocmd", 2))
	{
	    ಆದರೆ (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = au_ಕಂಡುಹಿಡಿ_cmd_arg(arg);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (checkforcmd_noparen(&arg, "command", 3))
	{
	    ಆದರೆ (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = ಕಂಡುಹಿಡಿ_ucmd_repl(arg);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಆದರೆ (p == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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
    ಪೂರ್ಣಾಂಕ
parse_command_modifiers(
	exarg_T	    *eap,
	ಅಕ್ಷರ	    **errormsg,
	cmdmod_T    *cmod,
	ಪೂರ್ಣಾಂಕ	    skip_only)
{
    char_u  *orig_cmd = eap->cmd;
    char_u  *cmd_ಆರಂಭ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ	    use_plus_cmd = FALSE;
    ಪೂರ್ಣಾಂಕ	    starts_with_colon = FALSE;
    ಪೂರ್ಣಾಂಕ	    vim9script = ಒಳ_vim9script();
    ಪೂರ್ಣಾಂಕ	    has_visual_range = FALSE;

    CLEAR_ಸೂಚಕ(cmod);
    cmod->cmod_flags = sticky_cmdmod_flags;

    ಆದರೆ (STRNCMP(eap->cmd, "'<,'>", 5) == 0)
    {
	// The automatically inserted Visual area range is skipped, so that
	// typing ":cmdmod cmd" in Visual mode works without having to move the
	// range to after the modifiers. The command will be "'<,'>cmdmod cmd",
	// parse "cmdmod cmd" and then put back "'<,'>" before "cmd" below.
	eap->cmd += 5;
	cmd_ಆರಂಭ = eap->cmd;
	has_visual_range = TRUE;
    }

    // Repeat until no more command modifiers are found.
    ಗಾಗಿ (;;)
    {
	char_u  *p;

	ಆಗ (*eap->cmd == ' ' || *eap->cmd == '\t' || *eap->cmd == ':')
	{
	    ಆದರೆ (*eap->cmd == ':')
		starts_with_colon = TRUE;
	    ++eap->cmd;
	}

	// in ex mode, an empty command (after modifiers) works like :+
	ಆದರೆ (*eap->cmd == NUL && exmode_ಸಕ್ರಿಯ
		   && (getline_equal(eap->ea_getline, eap->cookie, getexmodeline)
		       || getline_equal(eap->ea_getline, eap->cookie, getexline))
			&& curwin->w_cursor.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    use_plus_cmd = TRUE;
	    ಆದರೆ (!skip_only)
		ex_pressedreturn = TRUE;
	    ನಿಲ್ಲಿಸು;  // no modifiers following
	}

	// ignore comment and empty lines
	ಆದರೆ (comment_ಆರಂಭ(eap->cmd, starts_with_colon))
	{
	    // a comment ends at a NL
	    eap->nextcmd = vim_strchr(eap->cmd, '\n');
	    ಆದರೆ (eap->nextcmd != ಖಾಲಿ)
		++eap->nextcmd;
	    ಆದರೆ (vim9script)
	    {
		ಆದರೆ (has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		ಆದರೆ (eap->cmd[0] == '#' && eap->cmd[1] == '{'
							 && eap->cmd[2] != '{')
		    *errormsg = _(e_cannot_use_hash_curly_ಗಮ್ಯ_ಆರಂಭ_comment);
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (*eap->cmd == '\n')
	{
	    eap->nextcmd = eap->cmd + 1;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (*eap->cmd == NUL)
	{
	    ಆದರೆ (!skip_only)
	    {
		ex_pressedreturn = TRUE;
		ಆದರೆ (vim9script && has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
	    }
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	p = skip_range(eap->cmd, TRUE, ಖಾಲಿ);

	// In Vim9 script a variable can shadow a command modifier:
	//   verbose = 123
	//   verbose += 123
	//   silent! verbose = func()
	//   verbose.member = 2
	//   verbose[expr] = 2
	// But not:
	//   verbose [a, b] = list
	ಆದರೆ (vim9script)
	{
	    char_u *s, *n;

	    ಗಾಗಿ (s = eap->cmd; ASCII_ISALPHA(*s); ++s)
		;
	    n = skipwhite(s);
	    ಆದರೆ (*n == '.' || *n == '=' || (*n != NUL && n[1] == '=')
		    || *s == '[')
		ನಿಲ್ಲಿಸು;
	}

	ಆಯ್ಕೆ (*p)
	{
	    // When adding an entry, also modify cmdmod_info_tab[].
	    ಸ್ಥಿತಿ 'a':	ಆದರೆ (!checkforcmd_noparen(&eap->cmd, "aboveleft", 3))
			    ನಿಲ್ಲಿಸು;
			cmod->cmod_ವಿಭಜಿಸು |= WSP_ABOVE;
			ಮುಂದುವರಿ;

	    ಸ್ಥಿತಿ 'b':	ಆದರೆ (checkforcmd_noparen(&eap->cmd, "belowright", 3))
			{
			    cmod->cmod_ವಿಭಜಿಸು |= WSP_ಕೆಳಗೆ;
			    ಮುಂದುವರಿ;
			}
			ಆದರೆ (checkforcmd_opt(&eap->cmd, "browse", 3, TRUE))
			{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE_CMD
			    cmod->cmod_flags |= CMOD_BROWSE;
#ಆದರೆ_ಅಂತ್ಯ
			    ಮುಂದುವರಿ;
			}
			ಆದರೆ (!checkforcmd_noparen(&eap->cmd, "botright", 2))
			    ನಿಲ್ಲಿಸು;
			cmod->cmod_ವಿಭಜಿಸು |= WSP_BOT;
			ಮುಂದುವರಿ;

	    ಸ್ಥಿತಿ 'c':	ಆದರೆ (!checkforcmd_opt(&eap->cmd, "confirm", 4, TRUE))
			    ನಿಲ್ಲಿಸು;
#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			cmod->cmod_flags |= CMOD_CONFIRM;
#ಆದರೆ_ಅಂತ್ಯ
			ಮುಂದುವರಿ;

	    ಸ್ಥಿತಿ 'k':	ಆದರೆ (checkforcmd_noparen(&eap->cmd, "keepmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_KEEPMARKS;
			    ಮುಂದುವರಿ;
			}
			ಆದರೆ (checkforcmd_noparen(&eap->cmd, "keepalt", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPALT;
			    ಮುಂದುವರಿ;
			}
			ಆದರೆ (checkforcmd_noparen(&eap->cmd, "keeppatterns", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPPATTERNS;
			    ಮುಂದುವರಿ;
			}
			ಆದರೆ (!checkforcmd_noparen(&eap->cmd, "keepjumps", 5))
			    ನಿಲ್ಲಿಸು;
			cmod->cmod_flags |= CMOD_KEEPJUMPS;
			ಮುಂದುವರಿ;

	    ಸ್ಥಿತಿ 'f':	// only accept ":filter {pat} cmd"
			{
			    char_u  *reg_pat;
			    char_u  *nulp = ಖಾಲಿ;
			    ಪೂರ್ಣಾಂಕ	    c = 0;

			    ಆದರೆ (!checkforcmd_noparen(&p, "filter", 4)
				    || *p == NUL
				    || (ತುದಿಗಳು_excmd(*p)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
					// in ":filter #pat# cmd" # does not
					// start a comment
				     && (!vim9script || VIM_ISWHITE(p[1]))
#ಆದರೆ_ಅಂತ್ಯ
				     ))
				ನಿಲ್ಲಿಸು;
			    ಆದರೆ (*p == '!')
			    {
				cmod->cmod_filter_force = TRUE;
				p = skipwhite(p + 1);
				ಆದರೆ (*p == NUL || ತುದಿಗಳು_excmd(*p))
				    ನಿಲ್ಲಿಸು;
			    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
			    // Avoid that "filter(arg)" is recognized.
			    ಆದರೆ (vim9script && !VIM_ISWHITE(p[-1]))
				ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
			    ಆದರೆ (skip_only)
				p = skip_vimgrep_pat(p, ಖಾಲಿ, ಖಾಲಿ);
			    ಇಲ್ಲದಿದ್ದರೆ
				// NOTE: This puts a NUL after the pattern.
				p = skip_vimgrep_pat_ext(p, &reg_pat, ಖಾಲಿ,
								    &nulp, &c);
			    ಆದರೆ (p == ಖಾಲಿ || *p == NUL)
				ನಿಲ್ಲಿಸು;
			    ಆದರೆ (!skip_only)
			    {
				cmod->cmod_filter_regmatch.regprog =
						vim_regcomp(reg_pat, RE_MAGIC);
				ಆದರೆ (cmod->cmod_filter_regmatch.regprog == ಖಾಲಿ)
				    ನಿಲ್ಲಿಸು;
				// restore the character overwritten by NUL
				ಆದರೆ (nulp != ಖಾಲಿ)
				    *nulp = c;
			    }
			    eap->cmd = p;
			    ಮುಂದುವರಿ;
			}

	    ಸ್ಥಿತಿ 'h':	ಆದರೆ (checkforcmd_noparen(&eap->cmd, "horizontal", 3))
			{
			    cmod->cmod_ವಿಭಜಿಸು |= WSP_HOR;
			    ಮುಂದುವರಿ;
			}
			// ":hide" and ":hide | cmd" are not modifiers
			ಆದರೆ (p != eap->cmd || !checkforcmd_noparen(&p, "hide", 3)
					       || *p == NUL || ತುದಿಗಳು_excmd(*p))
			    ನಿಲ್ಲಿಸು;
			eap->cmd = p;
			cmod->cmod_flags |= CMOD_HIDE;
			ಮುಂದುವರಿ;

	    ಸ್ಥಿತಿ 'l':	ಆದರೆ (checkforcmd_noparen(&eap->cmd, "lockmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_LOCKMARKS;
			    ಮುಂದುವರಿ;
			}
			ಆದರೆ (checkforcmd_noparen(&eap->cmd, "legacy", 3))
			{
			    ಆದರೆ (ತುದಿಗಳು_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_legacy_must_be_followed_by_command);
				ಹಿಂತಿರುಗಿ FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_VIM9CMD;
			    cmod->cmod_flags |= CMOD_LEGACY;
			    ಮುಂದುವರಿ;
			}

			ಆದರೆ (!checkforcmd_noparen(&eap->cmd, "leftabove", 5))
			    ನಿಲ್ಲಿಸು;
			cmod->cmod_ವಿಭಜಿಸು |= WSP_ABOVE;
			ಮುಂದುವರಿ;

	    ಸ್ಥಿತಿ 'n':	ಆದರೆ (checkforcmd_noparen(&eap->cmd, "noautocmd", 3))
			{
			    cmod->cmod_flags |= CMOD_NOAUTOCMD;
			    ಮುಂದುವರಿ;
			}
			ಆದರೆ (!checkforcmd_noparen(&eap->cmd, "noswapfile", 3))
			    ನಿಲ್ಲಿಸು;
			cmod->cmod_flags |= CMOD_NOSWAPFILE;
			ಮುಂದುವರಿ;

	    ಸ್ಥಿತಿ 'r':	ಆದರೆ (!checkforcmd_noparen(&eap->cmd, "rightbelow", 6))
			    ನಿಲ್ಲಿಸು;
			cmod->cmod_ವಿಭಜಿಸು |= WSP_ಕೆಳಗೆ;
			ಮುಂದುವರಿ;

	    ಸ್ಥಿತಿ 's':	ಆದರೆ (checkforcmd_noparen(&eap->cmd, "sandbox", 3))
			{
			    cmod->cmod_flags |= CMOD_SANDBOX;
			    ಮುಂದುವರಿ;
			}
			ಆದರೆ (!checkforcmd_noparen(&eap->cmd, "silent", 3))
			    ನಿಲ್ಲಿಸು;
			cmod->cmod_flags |= CMOD_SILENT;
			ಆದರೆ (*eap->cmd == '!' && !VIM_ISWHITE(eap->cmd[-1]))
			{
			    // ":silent!", but not "silent !cmd"
			    eap->cmd = skipwhite(eap->cmd + 1);
			    cmod->cmod_flags |= CMOD_ERRSILENT;
			}
			ಮುಂದುವರಿ;

	    ಸ್ಥಿತಿ 't':	ಆದರೆ (checkforcmd_noparen(&p, "tab", 3))
			{
			    ಆದರೆ (!skip_only)
			    {
				ದೀರ್ಘ tabnr = get_ವಿಳಾಸ(eap, &eap->cmd,
						    ADDR_TABS, eap->skip,
						    skip_only, FALSE, 1);
				ಆದರೆ (tabnr == MAXLNUM)
				    cmod->cmod_tab = tabpage_ಸೂಚ್ಯಂಕ(curtab) + 1;
				ಇಲ್ಲದಿದ್ದರೆ
				{
				    ಆದರೆ (tabnr < 0 || tabnr > ಕೊನೆಯ_TAB_NR)
				    {
					*errormsg = _(e_invalid_range);
					ಹಿಂತಿರುಗಿ FAIL;
				    }
				    cmod->cmod_tab = tabnr + 1;
				}
			    }
			    eap->cmd = p;
			    ಮುಂದುವರಿ;
			}
			ಆದರೆ (!checkforcmd_noparen(&eap->cmd, "topleft", 2))
			    ನಿಲ್ಲಿಸು;
			cmod->cmod_ವಿಭಜಿಸು |= WSP_ತುದಿ;
			ಮುಂದುವರಿ;

	    ಸ್ಥಿತಿ 'u':	ಆದರೆ (!checkforcmd_noparen(&eap->cmd, "unsilent", 3))
			    ನಿಲ್ಲಿಸು;
			cmod->cmod_flags |= CMOD_UNSILENT;
			ಮುಂದುವರಿ;

	    ಸ್ಥಿತಿ 'v':	ಆದರೆ (checkforcmd_noparen(&eap->cmd, "vertical", 4))
			{
			    cmod->cmod_ವಿಭಜಿಸು |= WSP_VERT;
			    ಮುಂದುವರಿ;
			}
			ಆದರೆ (checkforcmd_noparen(&eap->cmd, "vim9cmd", 4))
			{
			    ಆದರೆ (ತುದಿಗಳು_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_vim9cmd_must_be_followed_by_command);
				ಹಿಂತಿರುಗಿ FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_LEGACY;
			    cmod->cmod_flags |= CMOD_VIM9CMD;
			    ಮುಂದುವರಿ;
			}
			ಆದರೆ (!checkforcmd_noparen(&p, "verbose", 4))
			    ನಿಲ್ಲಿಸು;
			ಆದರೆ (vim_isdigit(*eap->cmd))
			{
			    // zero means not set, one is verbose == 0, etc.
			    cmod->cmod_verbose = atoi((ಅಕ್ಷರ *)eap->cmd) + 1;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    cmod->cmod_verbose = 2;  // default: verbose == 1
			eap->cmd = p;
			ಮುಂದುವರಿ;
	}
	ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (has_visual_range)
    {
	ಆದರೆ (eap->cmd > cmd_ಆರಂಭ)
	{
	    // Move the '<,'> range to after the modifiers and insert a colon.
	    // Since the modifiers have been parsed put the colon on top of the
	    // space: "'<,'>mod cmd" -> "mod:'<,'>cmd
	    // Put eap->cmd after the colon.
	    ಆದರೆ (use_plus_cmd)
	    {
		size_t len = STRLEN(cmd_ಆರಂಭ);

		// Special case: empty command uses "+":
		//  "'<,'>mods" -> "mods *+
		//  Use "*" instead of "'<,'>" to avoid the command getting
		//  longer, in case it was allocated.
		mch_memmove(orig_cmd, cmd_ಆರಂಭ, len);
		STRCPY(orig_cmd + len, " *+");
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		mch_memmove(cmd_ಆರಂಭ - 5, cmd_ಆರಂಭ, eap->cmd - cmd_ಆರಂಭ);
		eap->cmd -= 5;
		mch_memmove(eap->cmd - 1, ":'<,'>", 6);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // No modifiers, move the pointer back.
	    // Special case: change empty command to "+".
	    ಆದರೆ (use_plus_cmd)
		eap->cmd = (char_u *)"'<,'>+";
	    ಇಲ್ಲದಿದ್ದರೆ
		eap->cmd = orig_cmd;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (use_plus_cmd)
	eap->cmd = (char_u *)exmode_plus;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return TRUE if "cmod" has anything set.
 */
    ಪೂರ್ಣಾಂಕ
has_cmdmod(cmdmod_T *cmod, ಪೂರ್ಣಾಂಕ ignore_silent)
{
    ಹಿಂತಿರುಗಿ (cmod->cmod_flags != 0 && (!ignore_silent
		|| (cmod->cmod_flags
		      & ~(CMOD_SILENT | CMOD_ERRSILENT | CMOD_UNSILENT)) != 0))
	    || cmod->cmod_ವಿಭಜಿಸು != 0
	    || cmod->cmod_verbose > 0
	    || cmod->cmod_tab != 0
	    || cmod->cmod_filter_regmatch.regprog != ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * If Vim9 script and "cmdmod" has anything set give an error and return TRUE.
 */
    ಪೂರ್ಣಾಂಕ
cmdmod_error(ಪೂರ್ಣಾಂಕ ignore_silent)
{
    ಆದರೆ (ಒಳ_vim9script() && has_cmdmod(&cmdmod, ignore_silent))
    {
	emsg(_(e_misplaced_command_modifier));
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Apply the command modifiers.  Saves current state in "cmdmod", call
 * undo_cmdmod() later.
 */
    ಶೂನ್ಯ
apply_cmdmod(cmdmod_T *cmod)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
    ಆದರೆ ((cmod->cmod_flags & CMOD_SANDBOX) && !cmod->cmod_did_sandbox)
    {
	++sandbox;
	cmod->cmod_did_sandbox = TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (cmod->cmod_verbose > 0)
    {
	ಆದರೆ (cmod->cmod_verbose_save == 0)
	    cmod->cmod_verbose_save = p_verbose + 1;
	p_verbose = cmod->cmod_verbose - 1;
    }

    ಆದರೆ ((cmod->cmod_flags & (CMOD_SILENT | CMOD_UNSILENT))
	    && cmod->cmod_save_ಸಂದೇಶ_silent == 0)
    {
	cmod->cmod_save_ಸಂದೇಶ_silent = ಸಂದೇಶ_silent + 1;
	cmod->cmod_save_ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll;
    }
    ಆದರೆ (cmod->cmod_flags & CMOD_SILENT)
	++ಸಂದೇಶ_silent;
    ಆದರೆ (cmod->cmod_flags & CMOD_UNSILENT)
	ಸಂದೇಶ_silent = 0;

    ಆದರೆ (cmod->cmod_flags & CMOD_ERRSILENT)
    {
	++emsg_silent;
	++cmod->cmod_did_esilent;
    }

    ಆದರೆ ((cmod->cmod_flags & CMOD_NOAUTOCMD) && cmod->cmod_save_ei == ಖಾಲಿ)
    {
	// Set 'eventignore' to "all".
	// First save the existing option value for restoring it later.
	cmod->cmod_save_ei = vim_strsave(p_ei);
	ಗಣ_string_option_ನೇರ((char_u *)"ei", -1,
					  (char_u *)"all", OPT_FREE, SID_NONE);
    }
}

/*
 * Undo and free contents of "cmod".
 */
    ಶೂನ್ಯ
undo_cmdmod(cmdmod_T *cmod)
{
    ಆದರೆ (cmod->cmod_verbose_save > 0)
    {
	p_verbose = cmod->cmod_verbose_save - 1;
	cmod->cmod_verbose_save = 0;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
    ಆದರೆ (cmod->cmod_did_sandbox)
    {
	--sandbox;
	cmod->cmod_did_sandbox = FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (cmod->cmod_save_ei != ಖಾಲಿ)
    {
	// Restore 'eventignore' to the value before ":noautocmd".
	ಗಣ_string_option_ನೇರ((char_u *)"ei", -1, cmod->cmod_save_ei,
							   OPT_FREE, SID_NONE);
	free_string_option(cmod->cmod_save_ei);
	cmod->cmod_save_ei = ಖಾಲಿ;
    }

    vim_regfree(cmod->cmod_filter_regmatch.regprog);

    ಆದರೆ (cmod->cmod_save_ಸಂದೇಶ_silent > 0)
    {
	// messages could be enabled for a serious error, need to check if the
	// counters don't become negative
	ಆದರೆ (!did_emsg || ಸಂದೇಶ_silent > cmod->cmod_save_ಸಂದೇಶ_silent - 1)
	    ಸಂದೇಶ_silent = cmod->cmod_save_ಸಂದೇಶ_silent - 1;
	emsg_silent -= cmod->cmod_did_esilent;
	ಆದರೆ (emsg_silent < 0)
	    emsg_silent = 0;
	// Restore msg_scroll, it's set by file I/O commands, even when no
	// message is actually displayed.
	ಸಂದೇಶ_scroll = cmod->cmod_save_ಸಂದೇಶ_scroll;

	// "silent reg" or "silent echo x" inside "redir" leaves msg_col
	// somewhere in the line.  Put it back in the first column.
	ಆದರೆ (redirecting())
	    ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;

	cmod->cmod_save_ಸಂದೇಶ_silent = 0;
	cmod->cmod_did_esilent = 0;
    }
}

/*
 * Parse the address range, if any, in "eap".
 * May set the last search pattern, unless "silent" is TRUE.
 * Return FAIL and set "errormsg" or return OK.
 */
    ಪೂರ್ಣಾಂಕ
parse_cmd_ವಿಳಾಸ(exarg_T *eap, ಅಕ್ಷರ **errormsg, ಪೂರ್ಣಾಂಕ silent)
{
    ಪೂರ್ಣಾಂಕ		ವಿಳಾಸ_ಎಣಿಕೆ = 1;
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		need_check_cursor = FALSE;
    ಪೂರ್ಣಾಂಕ		ret = FAIL;

    // Repeat for all ',' or ';' separated addresses.
    ಗಾಗಿ (;;)
    {
	eap->line1 = eap->line2;
	eap->line2 = default_ವಿಳಾಸ(eap);
	eap->cmd = skipwhite(eap->cmd);
	lnum = get_ವಿಳಾಸ(eap, &eap->cmd, eap->addr_type, eap->skip, silent,
					eap->addr_ಎಣಿಕೆ == 0, ವಿಳಾಸ_ಎಣಿಕೆ++);
	ಆದರೆ (eap->cmd == ಖಾಲಿ)	// error detected
	    ಹೋಗು theend;
	ಆದರೆ (lnum == MAXLNUM)
	{
	    ಆದರೆ (*eap->cmd == '%')   // '%' - all lines
	    {
		++eap->cmd;
		ಆಯ್ಕೆ (eap->addr_type)
		{
		    ಸ್ಥಿತಿ ADDR_ಸಾಲುಗಳು:
		    ಸ್ಥಿತಿ ADDR_OTHER:
			eap->line1 = 1;
			eap->line2 = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_LOADED_BUFFERS:
			{
			    buf_T	*buf = firstbuf;

			    ಆಗ (buf->b_ಮುಂದಿನ != ಖಾಲಿ
						  && buf->b_ml.ml_mfp == ಖಾಲಿ)
				buf = buf->b_ಮುಂದಿನ;
			    eap->line1 = buf->b_fnum;
			    buf = lastbuf;
			    ಆಗ (buf->b_prev != ಖಾಲಿ
						  && buf->b_ml.ml_mfp == ಖಾಲಿ)
				buf = buf->b_prev;
			    eap->line2 = buf->b_fnum;
			    ನಿಲ್ಲಿಸು;
			}
		    ಸ್ಥಿತಿ ADDR_BUFFERS:
			eap->line1 = firstbuf->b_fnum;
			eap->line2 = lastbuf->b_fnum;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_WINDOWS:
		    ಸ್ಥಿತಿ ADDR_TABS:
			ಆದರೆ (IS_USER_CMDIDX(eap->cmdidx))
			{
			    eap->line1 = 1;
			    eap->line2 = eap->addr_type == ADDR_WINDOWS
						  ? ಕೊನೆಯ_WIN_NR : ಕೊನೆಯ_TAB_NR;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // there is no Vim command which uses '%' and
			    // ADDR_WINDOWS or ADDR_TABS
			    *errormsg = _(e_invalid_range);
			    ಹೋಗು theend;
			}
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_TABS_RELATIVE:
		    ಸ್ಥಿತಿ ADDR_UNSIGNED:
		    ಸ್ಥಿತಿ ADDR_QUICKFIX:
			*errormsg = _(e_invalid_range);
			ಹೋಗು theend;
		    ಸ್ಥಿತಿ ADDR_ARGUMENTS:
			ಆದರೆ (ARGCOUNT == 0)
			    eap->line1 = eap->line2 = 0;
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    eap->line1 = 1;
			    eap->line2 = ARGCOUNT;
			}
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_QUICKFIX_VALID:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
			eap->line1 = 1;
			eap->line2 = qf_get_valid_ಗಾತ್ರ(eap);
			ಆದರೆ (eap->line2 == 0)
			    eap->line2 = 1;
#ಆದರೆ_ಅಂತ್ಯ
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_NONE:
			// Will give an error later if a range is found.
			ನಿಲ್ಲಿಸು;
		}
		++eap->addr_ಎಣಿಕೆ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*eap->cmd == '*' && vim_strchr(p_cpo, CPO_ನಕ್ಷತ್ರ) == ಖಾಲಿ)
	    {
		pos_T	    *fp;

		// '*' - visual area
		ಆದರೆ (eap->addr_type != ADDR_ಸಾಲುಗಳು)
		{
		    *errormsg = _(e_invalid_range);
		    ಹೋಗು theend;
		}

		++eap->cmd;
		ಆದರೆ (!eap->skip)
		{
		    fp = getmark('<', FALSE);
		    ಆದರೆ (check_mark(fp) == FAIL)
			ಹೋಗು theend;
		    eap->line1 = fp->lnum;
		    fp = getmark('>', FALSE);
		    ಆದರೆ (check_mark(fp) == FAIL)
			ಹೋಗು theend;
		    eap->line2 = fp->lnum;
		    ++eap->addr_ಎಣಿಕೆ;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    eap->line2 = lnum;
	eap->addr_ಎಣಿಕೆ++;

	ಆದರೆ (*eap->cmd == ';')
	{
	    ಆದರೆ (!eap->skip)
	    {
		curwin->w_cursor.lnum = eap->line2;

		// Don't leave the cursor on an illegal line or column, but do
		// accept zero as address, so 0;/PATTERN/ works correctly
		// (where zero usually means to use the first line).
		// Check the cursor position before returning.
		ಆದರೆ (eap->line2 > 0)
		    check_cursor();
		ಇಲ್ಲದಿದ್ದರೆ
		    check_cursor_ಲಂಬಸಾಲು();
		need_check_cursor = TRUE;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*eap->cmd != ',')
	    ನಿಲ್ಲಿಸು;
	++eap->cmd;
    }

    // One address given: set start and end lines.
    ಆದರೆ (eap->addr_ಎಣಿಕೆ == 1)
    {
	eap->line1 = eap->line2;
	// ... but only implicit: really no address given
	ಆದರೆ (lnum == MAXLNUM)
	    eap->addr_ಎಣಿಕೆ = 0;
    }
    ret = OK;

theend:
    ಆದರೆ (need_check_cursor)
	check_cursor();
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Append "cmd" to the error message in IObuff.
 * Takes care of limiting the length and handling 0xa0, which would be
 * invisible otherwise.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
append_command(char_u *cmd)
{
    size_t  len = STRLEN(IObuff);
    char_u  *s = cmd;
    char_u  *d;

    ಆದರೆ (len > IOSIZE - 100)
    {
	// Not enough space, truncate and put in "...".
	d = IObuff + IOSIZE - 100;
	d -= mb_ತಲೆ_off(IObuff, d);
	STRCPY(d, "...");
    }
    STRCAT(IObuff, ": ");
    d = IObuff + STRLEN(IObuff);
    ಆಗ (*s != NUL && d - IObuff + 5 < IOSIZE)
    {
	ಆದರೆ (enc_utf8 ? (s[0] == 0xc2 && s[1] == 0xa0) : *s == 0xa0)
	{
	    s += enc_utf8 ? 2 : 1;
	    STRCPY(d, "<a0>");
	    d += 4;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (d - IObuff + (*mb_ptr2len)(s) + 1 >= IOSIZE)
	    ನಿಲ್ಲಿಸು;
	ಇಲ್ಲದಿದ್ದರೆ
	    MB_COPY_CHAR(s, d);
    }
    *d = NUL;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * If "start" points "&opt", "&l:opt", "&g:opt" or "$ENV" return a pointer to
 * the name.  Otherwise just return "start".
 */
    char_u *
skip_option_env_lead(char_u *ಆರಂಭ)
{
    char_u *name = ಆರಂಭ;

    ಆದರೆ (*ಆರಂಭ == '&')
    {
	ಆದರೆ ((ಆರಂಭ[1] == 'l' || ಆರಂಭ[1] == 'g') && ಆರಂಭ[2] == ':')
	    name += 3;
	ಇಲ್ಲದಿದ್ದರೆ
	    name += 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ಆರಂಭ == '$')
	name += 1;
    ಹಿಂತಿರುಗಿ name;
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
one_letter_cmd(char_u *p, cmdidx_T *idx)
{
    ಆದರೆ (ಒಳ_vim9script())
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (p[0] == 'k'
	    && (p[1] != 'e' || (p[1] == 'e' && p[2] != 'e')))
    {
	*idx = CMD_k;
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಆದರೆ (p[0] == 's'
	    && ((p[1] == 'c' && (p[2] == NUL || (p[2] != 's' && p[2] != 'r'
			&& (p[3] == NUL || (p[3] != 'i' && p[4] != 'p')))))
		|| p[1] == 'g'
		|| (p[1] == 'i' && p[2] != 'm' && p[2] != 'l' && p[2] != 'g')
		|| p[1] == 'I'
		|| (p[1] == 'r' && p[2] != 'e')))
    {
	*idx = CMD_substitute;
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return TRUE if "cmd" starts with "123->", a number followed by a method
 * call.
 */
    ಪೂರ್ಣಾಂಕ
ಸಂಖ್ಯೆ_ವಿಧಾನ(char_u *cmd)
{
    char_u *p = skipdigits(cmd);

    ಹಿಂತಿರುಗಿ p > cmd && (p = skipwhite(p))[0] == '-' && p[1] == '>';
}
#ಆದರೆ_ಅಂತ್ಯ

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
ಕಂಡುಹಿಡಿ_ex_command(
	exarg_T *eap,
	ಪೂರ್ಣಾಂಕ	*ತುಂಬಿದ ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ	(*lookup)(char_u *, size_t, ಪೂರ್ಣಾಂಕ cmd, cctx_T *) ಬಳಸದ,
	cctx_T	*cctx ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_EVAL
    ಪೂರ್ಣಾಂಕ		vim9 = FALSE;
#ಇಲ್ಲದಿದ್ದರೆ
    ಪೂರ್ಣಾಂಕ		vim9 = ಒಳ_vim9script();

    /*
     * Recognize a Vim9 script function/method call and assignment:
     * "lvar = value", "lvar(arg)", "[1, 2 3]->Func()"
     */
    p = eap->cmd;
    ಆದರೆ (lookup != ಖಾಲಿ)
    {
	char_u *pskip = skip_option_env_lead(eap->cmd);

	ಆದರೆ (vim_strchr((char_u *)"{('[\"@&$", *p) != ಖಾಲಿ
	       || ((p = ಗಮ್ಯ_name_const_ಕೊನೆ(pskip)) > eap->cmd && *p != NUL)
	       || (p[0] == '0' && p[1] == 'z'))
	{
	    ಪೂರ್ಣಾಂಕ	    oplen;
	    ಪೂರ್ಣಾಂಕ	    heredoc;
	    char_u  *swp;

	    ಆದರೆ (*eap->cmd == '&'
		    || (eap->cmd[0] == '$'
				  && eap->cmd[1] != '\'' && eap->cmd[1] != '"')
		    || (eap->cmd[0] == '@'
					&& (valid_yank_reg(eap->cmd[1], FALSE)
						       || eap->cmd[1] == '@')))
	    {
		ಆದರೆ (*eap->cmd == '&')
		{
		    p = eap->cmd + 1;
		    ಆದರೆ (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
			p += 2;
		    p = ಗಮ್ಯ_name_ಕೊನೆ(p, FALSE);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*eap->cmd == '$')
		    p = ಗಮ್ಯ_name_ಕೊನೆ(eap->cmd + 1, FALSE);
		ಇಲ್ಲದಿದ್ದರೆ
		    p = eap->cmd + 2;
		ಆದರೆ (ತುದಿಗಳು_excmd(*skipwhite(p)))
		{
		    // "&option <NL>", "$ENV <NL>" and "@r <NL>" are the start
		    // of an expression.
		    eap->cmdidx = CMD_eval;
		    ಹಿಂತಿರುಗಿ eap->cmd;
		}
		// "&option" can be followed by "->" or "=", check below
	    }

	    ಆದರೆ (vim9 && *p == '<')
	    {
		// generic function type args
		ಆದರೆ (skip_generic_func_type_ವಾದಗಳು(&p) == FAIL)
		{
		    eap->cmdidx = CMD_ಗಾತ್ರ;
		    ಹಿಂತಿರುಗಿ p;
		}
	    }

	    swp = skipwhite(p);

	    ಆದರೆ (
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
		ಆದರೆ (*eap->cmd == '{' && ತುದಿಗಳು_excmd(*skipwhite(eap->cmd + 1)))
		{
		    // "{" by itself is the start of a block.
		    eap->cmdidx = CMD_ಖಂಡ;
		    ಹಿಂತಿರುಗಿ eap->cmd + 1;
		}
		eap->cmdidx = CMD_eval;
		ಹಿಂತಿರುಗಿ eap->cmd;
	    }

	    ಆದರೆ ((p != eap->cmd && (
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
		ಆದರೆ (skip_ಅಭಿವ್ಯಕ್ತಿ(&after, ಖಾಲಿ) == OK)
		{
		    after = skipwhite(after);
		    ಆದರೆ (*after == '=' || (*after != NUL && after[1] == '=')
					 || (after[0] == '.' && after[1] == '.'
							   && after[2] == '='))
			eap->cmdidx = CMD_var;
		}
		--emsg_off;
		ಹಿಂತಿರುಗಿ eap->cmd;
	    }

	    // "[...]->Method()" is a list expression, but "[a, b] = Func()" is
	    // an assignment.
	    // If there is no line break inside the "[...]" then "p" is
	    // advanced to after the "]" by to_name_const_end(): check if a "="
	    // follows.
	    // If "[...]" has a line break "p" still points at the "[" and it
	    // can't be an assignment.
	    ಆದರೆ (*eap->cmd == '[')
	    {
		char_u	    *eq;

		p = ಗಮ್ಯ_name_const_ಕೊನೆ(eap->cmd);
		ಆದರೆ (p == eap->cmd && *p == '[')
		{
		    ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0;
		    ಪೂರ್ಣಾಂಕ	semicolon = FALSE;

		    p = skip_var_list(eap->cmd, TRUE, &ಎಣಿಕೆ_2, &semicolon, TRUE);
		}
		eq = p;
		ಆದರೆ (eq != ಖಾಲಿ)
		{
		    eq = skipwhite(eq);
		    ಆದರೆ (vim_strchr((char_u *)"+-*/%.", *eq) != ಖಾಲಿ)
		    {
			ಆದರೆ (eq[0] == '.' && eq[1] == '.')
			    ++eq;
			++eq;
		    }
		}
		ಆದರೆ (p == ಖಾಲಿ || p == eap->cmd || *eq != '=')
		{
		    eap->cmdidx = CMD_eval;
		    ಹಿಂತಿರುಗಿ eap->cmd;
		}
		ಆದರೆ (p > eap->cmd && *eq == '=')
		{
		    eap->cmdidx = CMD_var;
		    ಹಿಂತಿರುಗಿ eap->cmd;
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
	    ಆದರೆ (oplen > 0)
	    {
		ಆದರೆ (((p - eap->cmd) > 2 && eap->cmd[1] == ':')
			|| *eap->cmd == '&'
			|| *eap->cmd == '$'
			|| *eap->cmd == '@'
			|| (eap->skip && IS_WHITE_OR_NUL(
						      *(skipwhite(p) + oplen)))
			|| lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK)
		{
		    eap->cmdidx = CMD_var;
		    ಹಿಂತಿರುಗಿ eap->cmd;
		}
	    }

	    // Recognize trying to use a type for a w:, b:, t: or g: variable:
	    // "w:varname: number = 123".
	    ಆದರೆ (eap->cmd[1] == ':' && *p == ':')
	    {
		eap->cmdidx = CMD_var;
		ಹಿಂತಿರುಗಿ eap->cmd;
	    }
	}

	// 1234->func() is a method call
	ಆದರೆ (ಸಂಖ್ಯೆ_ವಿಧಾನ(eap->cmd))
	{
	    eap->cmdidx = CMD_eval;
	    ಹಿಂತಿರುಗಿ eap->cmd;
	}

	// "g:", "s:" and "l:" are always assumed to be a variable, thus start
	// an expression.  A global/substitute/list command needs to use a
	// longer name.
	ಆದರೆ (vim_strchr((char_u *)"gsl", *p) != ಖಾಲಿ && p[1] == ':')
	{
	    eap->cmdidx = CMD_eval;
	    ಹಿಂತಿರುಗಿ eap->cmd;
	}

	// If it is an ID it might be a variable with an operator on the next
	// line, if the variable exists it can't be an Ex command.
	ಆದರೆ (p > eap->cmd && ತುದಿಗಳು_excmd(*skipwhite(p))
		&& (lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK
		    || (ASCII_ISALPHA(eap->cmd[0]) && eap->cmd[1] == ':')))
	{
	    eap->cmdidx = CMD_eval;
	    ಹಿಂತಿರುಗಿ eap->cmd;
	}

	// Check for "++nr" and "--nr".
	ಆದರೆ (p == eap->cmd && p[0] != NUL && p[0] == p[1]
						   && (*p == '+' || *p == '-'))
	{
	    eap->cmdidx = *p == '+' ? CMD_increment : CMD_decrement;
	    ಹಿಂತಿರುಗಿ eap->cmd + 2;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Isolate the command and search for it in the command table.
     */
    p = eap->cmd;
    ಆದರೆ (one_letter_cmd(p, &eap->cmdidx))
    {
	++p;
	ಆದರೆ (ತುಂಬಿದ != ಖಾಲಿ)
	    *ತುಂಬಿದ = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆಗ (ASCII_ISALPHA(*p))
	    ++p;
	// for python 3.x support ":py3", ":python3", ":py3file", etc.
	ಆದರೆ (eap->cmd[0] == 'p' && eap->cmd[1] == 'y')
	{
	    ಆಗ (ASCII_ISALNUM(*p))
		++p;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '9' && STRNCMP("vim9", eap->cmd, 4) == 0)
	{
	    // include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	    ++p;
	    ಆಗ (ASCII_ISALPHA(*p))
		++p;
	}

	// check for non-alpha command
	ಆದರೆ (p == eap->cmd && vim_strchr((char_u *)"@*!=><&~#}", *p) != ಖಾಲಿ)
	    ++p;
	len = (ಪೂರ್ಣಾಂಕ)(p - eap->cmd);
	// The "d" command can directly be followed by 'l' or 'p' flag, when
	// not in Vim9 script.
	ಆದರೆ (!vim9 && *eap->cmd == 'd' && (p[-1] == 'l' || p[-1] == 'p'))
	{
	    // Check for ":dl", ":dell", etc. to ":deletel": that's
	    // :delete with the 'l' flag.  Same for 'p'.
	    ಗಾಗಿ (i = 0; i < len; ++i)
		ಆದರೆ (eap->cmd[i] != ((char_u *)"delete")[i])
		    ನಿಲ್ಲಿಸು;
	    ಆದರೆ (i == len - 1)
	    {
		--len;
		ಆದರೆ (p[-1] == 'l')
		    eap->flags |= EXFLAG_LIST;
		ಇಲ್ಲದಿದ್ದರೆ
		    eap->flags |= EXFLAG_PRINT;
	    }
	}

	ಆದರೆ (ASCII_ISLOWER(eap->cmd[0]))
	{
	    ಪೂರ್ಣಾಂಕ c1 = eap->cmd[0];
	    ಪೂರ್ಣಾಂಕ c2 = len == 1 ? NUL : eap->cmd[1];

	    ಆದರೆ (command_ಎಣಿಕೆ != (ಪೂರ್ಣಾಂಕ)CMD_ಗಾತ್ರ)
	    {
		iemsg(e_command_ಕೋಷ್ಟಕ_needs_ಗಮ್ಯ_be_updated_ಸತತ_make_cmdidxs);
		getout(1);
	    }

	    // Use a precomputed index for fast look-up in cmdnames[]
	    // taking into account the first 2 letters of eap->cmd.
	    eap->cmdidx = cmdidxs1[Char_Ord_ಕೆಳಗಿನ(c1)];
	    ಆದರೆ (ASCII_ISLOWER(c2))
		eap->cmdidx += cmdidxs2[Char_Ord_ಕೆಳಗಿನ(c1)][Char_Ord_ಕೆಳಗಿನ(c2)];
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ASCII_ISUPPER(eap->cmd[0]))
	    eap->cmdidx = CMD_ಮುಂದಿನ_3;
	ಇಲ್ಲದಿದ್ದರೆ
	    eap->cmdidx = CMD_bang;

	ಗಾಗಿ ( ; (ಪೂರ್ಣಾಂಕ)eap->cmdidx < (ಪೂರ್ಣಾಂಕ)CMD_ಗಾತ್ರ;
			       eap->cmdidx = (cmdidx_T)((ಪೂರ್ಣಾಂಕ)eap->cmdidx + 1))
	    ಆದರೆ (STRNCMP(cmdnames[(ಪೂರ್ಣಾಂಕ)eap->cmdidx].cmd_name, (ಅಕ್ಷರ *)eap->cmd,
							    (size_t)len) == 0)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		ಆದರೆ (ತುಂಬಿದ != ಖಾಲಿ && cmdnames[eap->cmdidx].cmd_name[len] == NUL)
		    *ತುಂಬಿದ = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	    }

	// :Print and :mode are not supported in Vim9 script.
	// Some commands cannot be shortened in Vim9 script.
	ಆದರೆ (vim9 && eap->cmdidx != CMD_ಗಾತ್ರ)
	{
	    ಆದರೆ (eap->cmdidx == CMD_ಬಹುಲಕ || eap->cmdidx == CMD_Print)
		eap->cmdidx = CMD_ಗಾತ್ರ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((cmdnames[eap->cmdidx].cmd_argt & EX_ಸಂಪೂರ್ಣ)
			  && len < (ಪೂರ್ಣಾಂಕ)cmdnames[eap->cmdidx].cmd_namelen)
	    {
		semsg(_(e_command_cannot_be_shortened_str), eap->cmd);
		eap->cmdidx = CMD_ಗಾತ್ರ;
	    }
	}

	// Do not recognize ":*" as the star command unless '*' is in
	// 'cpoptions'.
	ಆದರೆ (eap->cmdidx == CMD_ನಕ್ಷತ್ರ && vim_strchr(p_cpo, CPO_ನಕ್ಷತ್ರ) == ಖಾಲಿ)
	    p = eap->cmd;

	// Look for a user defined command as a last resort.  Let ":Print" be
	// overruled by a user defined command.
	ಆದರೆ ((eap->cmdidx == CMD_ಗಾತ್ರ || eap->cmdidx == CMD_Print)
		&& *eap->cmd >= 'A' && *eap->cmd <= 'Z')
	{
	    // User defined commands may contain digits.
	    ಆಗ (ASCII_ISALNUM(*p))
		++p;
	    p = ಕಂಡುಹಿಡಿ_ucmd(eap, p, ತುಂಬಿದ, ಖಾಲಿ, ಖಾಲಿ);
	}
	ಆದರೆ (p == ಖಾಲಿ || p == eap->cmd)
	    eap->cmdidx = CMD_ಗಾತ್ರ;
    }

    // ":fina" means ":finally" in legacy script, for backwards compatibility.
    ಆದರೆ (eap->cmdidx == CMD_final && p - eap->cmd == 4 && !vim9)
	eap->cmdidx = CMD_finally;

     // Force ":ho" to be unresolved.  Without this, find_ex_command()
     // matches it to CMD_horizontal (the only "ho*" entry), which makes
     // fullcommand("ho") return "horizontal" even though ":ho" cannot be
     // used as the modifier (cmdmods[] requires 3 chars, "hor").
    ಆದರೆ (eap->cmdidx == CMD_horizontal && p - eap->cmd == 2)
	eap->cmdidx = CMD_ಗಾತ್ರ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (eap->cmdidx < CMD_ಗಾತ್ರ
	    && vim9
	    && !IS_WHITE_NL_OR_NUL(*p) && *p != '!' && *p != '|'
	    && (eap->cmdidx < 0 ||
		(cmdnames[eap->cmdidx].cmd_argt & EX_NONWHITE_OK) == 0))
    {
	// A name that goes on with an underscore is not this command with an
	// argument: "ch_log" is not ":change".  Leave it to be reported as an
	// invalid command.
	ಆದರೆ (*p != '_')
	{
	    char_u *cmd = vim_strnsave(eap->cmd, p - eap->cmd);

	    semsg(_(e_command_str_not_followed_by_white_space_str), cmd,
								    eap->cmd);
	    vim_free(cmd);
	}
	eap->cmdidx = CMD_ಗಾತ್ರ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ p;
}

#ಆದರೆ defined(FEAT_EVAL)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಅಕ್ಷರ	*name;
    ಪೂರ್ಣಾಂಕ		minlen;
    ಪೂರ್ಣಾಂಕ		has_ಎಣಿಕೆ;  // :123verbose  :3tab
} cmdmod_info_T;

ಸ್ಥಿರ cmdmod_info_T cmdmod_info_tab[] = {
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
    ಪೂರ್ಣಾಂಕ
modifier_len(char_u *cmd)
{
    ಪೂರ್ಣಾಂಕ		i, j;
    char_u	*p = cmd;

    ಆದರೆ (VIM_ISDIGIT(*cmd))
	p = skipwhite(skipdigits(cmd + 1));
    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(cmdmod_info_tab); ++i)
    {
	// cmdmod_info_tab[] is sorted by name: once the first letter is past
	// the command's first letter no later entry can match.
	ಆದರೆ (cmdmod_info_tab[i].name[0] > *p)
	    ನಿಲ್ಲಿಸು;
	ಗಾಗಿ (j = 0; p[j] != NUL; ++j)
	    ಆದರೆ (p[j] != cmdmod_info_tab[i].name[j])
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (!ASCII_ISALPHA(p[j]) && j >= cmdmod_info_tab[i].minlen
					&& (p == cmd || cmdmod_info_tab[i].has_ಎಣಿಕೆ))
	    ಹಿಂತಿರುಗಿ j + (ಪೂರ್ಣಾಂಕ)(p - cmd);
    }
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Return > 0 if an Ex command "name" exists.
 * Return 2 if there is an exact match.
 * Return 3 if there is an ambiguous match.
 */
    ಪೂರ್ಣಾಂಕ
cmd_exists(char_u *name)
{
    exarg_T	ea;
    ಪೂರ್ಣಾಂಕ		ತುಂಬಿದ = FALSE;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		j;
    char_u	*p;

    // Check command modifiers.
    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(cmdmod_info_tab); ++i)
    {
	ಗಾಗಿ (j = 0; name[j] != NUL; ++j)
	    ಆದರೆ (name[j] != cmdmod_info_tab[i].name[j])
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	    ಹಿಂತಿರುಗಿ (cmdmod_info_tab[i].name[j] == NUL ? 2 : 1);
    }

    // Check built-in commands and user defined commands.
    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    p = ಕಂಡುಹಿಡಿ_ex_command(&ea, &ತುಂಬಿದ, ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 3;
    ಆದರೆ (vim_isdigit(*name) && ea.cmdidx != CMD_ಹೊಂದಿಕೆ)
	ಹಿಂತಿರುಗಿ 0;
    ಆದರೆ (*skipwhite(p) != NUL)
	ಹಿಂತಿರುಗಿ 0;	// trailing garbage
    ಹಿಂತಿರುಗಿ (ea.cmdidx == CMD_ಗಾತ್ರ ? 0 : (ತುಂಬಿದ ? 2 : 1));
}

/*
 * Add the attributes of an Ex command to "d" in the terms of ":command":
 * "argt" are the EX_ flags, "addr_type" the address type and "def" the
 * default for a range or count, -1 when there is none.
 */
    ಶೂನ್ಯ
ex_command_attrs(dict_T *d, long_u argt, cmd_addr_T addr_type, ದೀರ್ಘ def)
{
    ಸ್ಥಿರ ಅಕ್ಷರ *addr_ಹೆಸರುಗಳು[] = {
	"lines", "windows", "arguments", "loaded_buffers", "buffers", "tabs",
	"tabs_relative", "quickfix_valid", "quickfix", "unsigned", "other",
	"none"
    };
    ಅಕ್ಷರ	*nargs;

    ಆದರೆ (!(argt & EX_EXTRA))
	nargs = "0";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(argt & EX_NOSPC))
	nargs = (argt & EX_NEEDARG) ? "+" : "*";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(argt & EX_NEEDARG))
	nargs = "?";
    ಇಲ್ಲದಿದ್ದರೆ
	nargs = (argt & EX_ARGSPACE) ? "_" : "1";
    dict_ಕೂಡಿಸು_string(d, "nargs", (char_u *)nargs);

    ಆದರೆ (!(argt & EX_RANGE))
	dict_ಕೂಡಿಸು_bool(d, "range", FALSE);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argt & EX_DFLALL)
	dict_ಕೂಡಿಸು_string(d, "range", (char_u *)"%");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (def >= 0 && !(argt & EX_ಎಣಿಕೆ))
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "range", def);
    ಇಲ್ಲದಿದ್ದರೆ
	dict_ಕೂಡಿಸು_string(d, "range", (char_u *)".");
    ಆದರೆ (!(argt & EX_ಎಣಿಕೆ))
	dict_ಕೂಡಿಸು_bool(d, "count", FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "count", def < 0 ? 0 : def);
    dict_ಕೂಡಿಸು_bool(d, "bang", (argt & EX_BANG) != 0);
    dict_ಕೂಡಿಸು_bool(d, "bar", (argt & EX_TRLBAR) != 0);
    dict_ಕೂಡಿಸು_bool(d, "register", (argt & EX_REGSTR) != 0);
    dict_ಕೂಡಿಸು_string(d, "addr", (char_u *)addr_ಹೆಸರುಗಳು[addr_type]);
}

/*
 * Add what getinfo() reports for the Ex command "name" to "d".  "vim9" is TRUE
 * to find the command as in Vim9 script, FALSE as in legacy script, -1 for the
 * context of the caller.
 * Returns FAIL when there is no such command or the abbreviation is
 * ambiguous.
 */
    ಪೂರ್ಣಾಂಕ
ex_command_info(char_u *name, ಪೂರ್ಣಾಂಕ vim9, dict_T *d)
{
    exarg_T	ea;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		j;
    ಪೂರ್ಣಾಂಕ		save_cmod_flags = cmdmod.cmod_flags;
    ಪೂರ್ಣಾಂಕ		ret = OK;

    ಆದರೆ (vim9 != -1)
    {
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9 ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    // An abbreviation of a command modifier, like ":bo" for ":botright".
    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(cmdmod_info_tab); ++i)
    {
	ಗಾಗಿ (j = 0; name[j] != NUL; ++j)
	    ಆದರೆ (name[j] != cmdmod_info_tab[i].name[j])
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	{
	    name = (char_u *)cmdmod_info_tab[i].name;
	    ನಿಲ್ಲಿಸು;
	}
    }

    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    ea.addr_ಎಣಿಕೆ = 0;
    // Don't complain about using "en" or ":let" in Vim9 script.
    ++emsg_silent;
    p = ಕಂಡುಹಿಡಿ_ex_command(&ea, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ (p == ಖಾಲಿ || ea.cmdidx == CMD_ಗಾತ್ರ
	    || (vim_isdigit(*name) && ea.cmdidx != CMD_ಹೊಂದಿಕೆ)
	    || *skipwhite(p) != NUL
	    || not_ಒಳ_vim9(&ea, true) == FAIL)
	ret = FAIL;
    --emsg_silent;

    ಆದರೆ (ret == FAIL)
	;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (IS_USER_CMDIDX(ea.cmdidx))
	ret = user_command_info(ea.useridx, ea.cmdidx, d);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	notimpl = cmdnames[ea.cmdidx].cmd_func == ex_ni
# ifdef HAVE_EX_SCRIPT_NI
			    || cmdnames[ea.cmdidx].cmd_func == ex_script_ni
# endif
			    ;

	dict_ಕೂಡಿಸು_string(d, "name", cmdnames[ea.cmdidx].cmd_name);
	dict_ಕೂಡಿಸು_string(d, "kind", (char_u *)(
		    cmdnames[ea.cmdidx].cmd_func == ex_wrongmodifier
						    ? "modifier" : "builtin"));
	dict_ಕೂಡಿಸು_bool(d, "available", !notimpl);
	ex_command_attrs(d, cmdnames[ea.cmdidx].cmd_argt,
				       cmdnames[ea.cmdidx].cmd_addr_type, -1);
    }
    cmdmod.cmod_flags = save_cmod_flags;
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * "fullcommand" function
 */
    ಶೂನ್ಯ
f_fullcommand(typval_T *argvars, typval_T *rettv)
{
    exarg_T	ea;
    char_u	*name;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();
    ಪೂರ್ಣಾಂಕ		save_cmod_flags = cmdmod.cmod_flags;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    name = tv_get_string(&argvars[0]);
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	vim9script = tv_get_bool(&argvars[1]);
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9script ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    ಆಗ (*name == ':')
	name++;
    name = skip_range(name, TRUE, ಖಾಲಿ);

    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.addr_ಎಣಿಕೆ = 0;
    ++emsg_silent;  // don't complain about using "en" in Vim9 script
    p = ಕಂಡುಹಿಡಿ_ex_command(&ea, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
    --emsg_silent;
    ಆದರೆ (p == ಖಾಲಿ || ea.cmdidx == CMD_ಗಾತ್ರ)
	ಹೋಗು theend;

    ಆದರೆ (vim9script && not_ಒಳ_vim9(&ea, true) == FAIL)
	ಹೋಗು theend;

    rettv->vval.v_string = vim_strsave(IS_USER_CMDIDX(ea.cmdidx)
				 ? get_user_command_name(ea.useridx, ea.cmdidx)
				 : cmdnames[ea.cmdidx].cmd_name);
theend:
    cmdmod.cmod_flags = save_cmod_flags;
}
#ಆದರೆ_ಅಂತ್ಯ

    cmdidx_T
excmd_get_cmdidx(char_u *cmd, ಪೂರ್ಣಾಂಕ len)
{
    cmdidx_T idx;

    ಆದರೆ (!one_letter_cmd(cmd, &idx))
	ಗಾಗಿ (idx = (cmdidx_T)0; (ಪೂರ್ಣಾಂಕ)idx < (ಪೂರ್ಣಾಂಕ)CMD_ಗಾತ್ರ;
		idx = (cmdidx_T)((ಪೂರ್ಣಾಂಕ)idx + 1))
	    ಆದರೆ (STRNCMP(cmdnames[(ಪೂರ್ಣಾಂಕ)idx].cmd_name, cmd, (size_t)len) == 0)
		ನಿಲ್ಲಿಸು;

    ಹಿಂತಿರುಗಿ idx;
}

    ದೀರ್ಘ
excmd_get_argt(cmdidx_T idx)
{
    ಹಿಂತಿರುಗಿ (ದೀರ್ಘ)cmdnames[(ಪೂರ್ಣಾಂಕ)idx].cmd_argt;
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
    char_u	*cmd_ಆರಂಭ,
    ಪೂರ್ಣಾಂಕ		skip_ನಕ್ಷತ್ರ,	// skip "*" used for Visual range
    ಪೂರ್ಣಾಂಕ		*ctx)		// pointer to xp_context or NULL
{
    char_u	*cmd = cmd_ಆರಂಭ;
    ಚಿಹ್ನೆರಹಿತ	delim;

    ಆಗ (vim_strchr((char_u *)" \t0123456789.$%'/?-+,;\\", *cmd) != ಖಾಲಿ)
    {
	ಆದರೆ (*cmd == '\\')
	{
	    ಆದರೆ (cmd[1] == '?' || cmd[1] == '/' || cmd[1] == '&')
		++cmd;
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cmd == '\'')
	{
	    char_u *p = cmd;

	    // a quote is only valid at the start or after a separator
	    ಆಗ (p > cmd_ಆರಂಭ)
	    {
		--p;
		ಆದರೆ (!VIM_ISWHITE(*p))
		    ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (cmd > cmd_ಆರಂಭ && !VIM_ISWHITE(*p) && *p != ',' && *p != ';')
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (*++cmd == NUL && ctx != ಖಾಲಿ)
		*ctx = EXPAND_NOTHING;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cmd == '/' || *cmd == '?')
	{
	    delim = *cmd++;
	    ಆಗ (*cmd != NUL && *cmd != delim)
		ಆದರೆ (*cmd++ == '\\' && *cmd != NUL)
		    ++cmd;
	    ಆದರೆ (*cmd == NUL && ctx != ಖಾಲಿ)
		*ctx = EXPAND_NOTHING;
	}
	ಆದರೆ (*cmd != NUL)
	    ++cmd;
    }

    // Skip ":" and white space.
    ಆಗ (*cmd == ':')
	cmd = skipwhite(cmd + 1);

    // Skip "*" used for Visual range.
    ಆದರೆ (skip_ನಕ್ಷತ್ರ && *cmd == '*' && vim_strchr(p_cpo, CPO_ನಕ್ಷತ್ರ) == ಖಾಲಿ)
	cmd = skipwhite(cmd + 1);

    ಹಿಂತಿರುಗಿ cmd;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
addr_error(cmd_addr_T addr_type)
{
    ಆದರೆ (addr_type == ADDR_NONE)
	emsg(_(e_no_range_allowed));
    ಇಲ್ಲದಿದ್ದರೆ
	emsg(_(e_invalid_range));
}

/*
 * Return the default address for an address type.
 */
    ಸ್ಥಿರ linenr_T
default_ವಿಳಾಸ(exarg_T *eap)
{
    linenr_T lnum = 0;

    ಆಯ್ಕೆ (eap->addr_type)
    {
	ಸ್ಥಿತಿ ADDR_ಸಾಲುಗಳು:
	ಸ್ಥಿತಿ ADDR_OTHER:
	    // Default is the cursor line number.  Avoid using an invalid
	    // line number though.
	    ಆದರೆ (curwin->w_cursor.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಇಲ್ಲದಿದ್ದರೆ
		lnum = curwin->w_cursor.lnum;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ADDR_WINDOWS:
	    lnum = CURRENT_WIN_NR;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ADDR_ARGUMENTS:
	    lnum = curwin->w_arg_idx + 1;
	    ಆದರೆ (lnum > ARGCOUNT)
		lnum = ARGCOUNT;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ADDR_LOADED_BUFFERS:
	ಸ್ಥಿತಿ ADDR_BUFFERS:
	    lnum = curbuf->b_fnum;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ADDR_TABS:
	    lnum = CURRENT_TAB_NR;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ADDR_TABS_RELATIVE:
	ಸ್ಥಿತಿ ADDR_UNSIGNED:
	    lnum = 1;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ADDR_QUICKFIX:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
	    lnum = qf_get_cur_idx(eap);
#ಆದರೆ_ಅಂತ್ಯ
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ADDR_QUICKFIX_VALID:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
	    lnum = qf_get_cur_valid_idx(eap);
#ಆದರೆ_ಅಂತ್ಯ
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ADDR_NONE:
	    // Will give an error later if a range is found.
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ lnum;
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
get_ವಿಳಾಸ(
    exarg_T	*eap ಬಳಸದ,
    char_u	**ptr,
    cmd_addr_T	addr_type,
    ಪೂರ್ಣಾಂಕ		skip,		// only skip the address, don't use it
    ಪೂರ್ಣಾಂಕ		silent,		// no errors or side effects
    ಪೂರ್ಣಾಂಕ		ಗಮ್ಯ_other_file,  // flag: may jump to other file
    ಪೂರ್ಣಾಂಕ		ವಿಳಾಸ_ಎಣಿಕೆ ಬಳಸದ) // 1 for first address, >1 after comma
{
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		i;
    ದೀರ್ಘ	n;
    char_u	*cmd;
    pos_T	pos;
    pos_T	*fp;
    linenr_T	lnum;
    buf_T	*buf;

    cmd = skipwhite(*ptr);
    lnum = MAXLNUM;
    ಮಾಡು
    {
	ಆಯ್ಕೆ (*cmd)
	{
	    ಸ್ಥಿತಿ '.':			    // '.' - Cursor position
		++cmd;
		ಆಯ್ಕೆ (addr_type)
		{
		    ಸ್ಥಿತಿ ADDR_ಸಾಲುಗಳು:
		    ಸ್ಥಿತಿ ADDR_OTHER:
			lnum = curwin->w_cursor.lnum;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_LOADED_BUFFERS:
		    ಸ್ಥಿತಿ ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_NONE:
		    ಸ್ಥಿತಿ ADDR_TABS_RELATIVE:
		    ಸ್ಥಿತಿ ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = ಖಾಲಿ;
			ಹೋಗು error;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_QUICKFIX:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#ಆದರೆ_ಅಂತ್ಯ
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_QUICKFIX_VALID:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#ಆದರೆ_ಅಂತ್ಯ
			ನಿಲ್ಲಿಸು;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ '$':			    // '$' - last line
		++cmd;
		ಆಯ್ಕೆ (addr_type)
		{
		    ಸ್ಥಿತಿ ADDR_ಸಾಲುಗಳು:
		    ಸ್ಥಿತಿ ADDR_OTHER:
			lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_WINDOWS:
			lnum = ಕೊನೆಯ_WIN_NR;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_ARGUMENTS:
			lnum = ARGCOUNT;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_LOADED_BUFFERS:
			buf = lastbuf;
			ಆಗ (buf->b_ml.ml_mfp == ಖಾಲಿ)
			{
			    ಆದರೆ (buf->b_prev == ಖಾಲಿ)
				ನಿಲ್ಲಿಸು;
			    buf = buf->b_prev;
			}
			lnum = buf->b_fnum;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_BUFFERS:
			lnum = lastbuf->b_fnum;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_TABS:
			lnum = ಕೊನೆಯ_TAB_NR;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_NONE:
		    ಸ್ಥಿತಿ ADDR_TABS_RELATIVE:
		    ಸ್ಥಿತಿ ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = ಖಾಲಿ;
			ಹೋಗು error;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_QUICKFIX:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
			lnum = qf_get_ಗಾತ್ರ(eap);
			ಆದರೆ (lnum == 0)
			    lnum = 1;
#ಆದರೆ_ಅಂತ್ಯ
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_QUICKFIX_VALID:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
			lnum = qf_get_valid_ಗಾತ್ರ(eap);
			ಆದರೆ (lnum == 0)
			    lnum = 1;
#ಆದರೆ_ಅಂತ್ಯ
			ನಿಲ್ಲಿಸು;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ '\'':			    // ''' - mark
		ಆದರೆ (*++cmd == NUL)
		{
		    cmd = ಖಾಲಿ;
		    ಹೋಗು error;
		}
		ಆದರೆ (addr_type != ADDR_ಸಾಲುಗಳು)
		{
		    addr_error(addr_type);
		    cmd = ಖಾಲಿ;
		    ಹೋಗು error;
		}
		ಆದರೆ (skip)
		    ++cmd;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Only accept a mark in another file when it is
		    // used by itself: ":'M".
		    fp = getmark(*cmd, ಗಮ್ಯ_other_file && cmd[1] == NUL);
		    ++cmd;
		    ಆದರೆ (fp == (pos_T *)-1)
			// Jumped to another file.
			lnum = curwin->w_cursor.lnum;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (check_mark(fp) == FAIL)
			{
			    cmd = ಖಾಲಿ;
			    ಹೋಗು error;
			}
			lnum = fp->lnum;
		    }
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ '/':
	    ಸ್ಥಿತಿ '?':			// '/' or '?' - search
		c = *cmd++;
		ಆದರೆ (addr_type != ADDR_ಸಾಲುಗಳು)
		{
		    addr_error(addr_type);
		    cmd = ಖಾಲಿ;
		    ಹೋಗು error;
		}
		ಆದರೆ (skip)	// skip "/pat/"
		{
		    cmd = skip_regexp(cmd, c, magic_isset());
		    ಆದರೆ (*cmd == c)
			++cmd;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಪೂರ್ಣಾಂಕ flags;

		    pos = curwin->w_cursor; // save curwin->w_cursor

		    // When '/' or '?' follows another address, start from
		    // there.
		    ಆದರೆ (lnum > 0 && lnum != MAXLNUM)
			curwin->w_cursor.lnum =
				lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
					   ? curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ : lnum;

		    // Start a forward search at the end of the line (unless
		    // before the first line).
		    // Start a backward search at the start of the line.
		    // This makes sure we never match in the current
		    // line, and can match anywhere in the
		    // next/previous line.
		    ಆದರೆ (c == '/' && curwin->w_cursor.lnum > 0)
			curwin->w_cursor.ಲಂಬಸಾಲು = MAXCOL;
		    ಇಲ್ಲದಿದ್ದರೆ
			curwin->w_cursor.ಲಂಬಸಾಲು = 0;
		    searchcmdlen = 0;
		    flags = silent ? ಹುಡುಕು_KEEP : ಹುಡುಕು_HIS | ಹುಡುಕು_ಸಂದೇಶ;
		    ಆದರೆ (!do_ಹುಡುಕು(ಖಾಲಿ, c, c, cmd, STRLEN(cmd), 1L, flags, ಖಾಲಿ))
		    {
			curwin->w_cursor = pos;
			cmd = ಖಾಲಿ;
			ಹೋಗು error;
		    }
		    lnum = curwin->w_cursor.lnum;
		    curwin->w_cursor = pos;
		    // adjust command string pointer
		    cmd += searchcmdlen;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ '\\':		    // "\?", "\/" or "\&", repeat search
		++cmd;
		ಆದರೆ (addr_type != ADDR_ಸಾಲುಗಳು)
		{
		    addr_error(addr_type);
		    cmd = ಖಾಲಿ;
		    ಹೋಗು error;
		}
		ಆದರೆ (*cmd == '&')
		    i = RE_SUBST;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cmd == '?' || *cmd == '/')
		    i = RE_ಹುಡುಕು;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    emsg(_(e_backslash_should_be_followed_by));
		    cmd = ಖಾಲಿ;
		    ಹೋಗು error;
		}

		ಆದರೆ (!skip)
		{
		    /*
		     * When search follows another address, start from
		     * there.
		     */
		    ಆದರೆ (lnum != MAXLNUM)
			pos.lnum = lnum;
		    ಇಲ್ಲದಿದ್ದರೆ
			pos.lnum = curwin->w_cursor.lnum;

		    /*
		     * Start the search just like for the above
		     * do_search().
		     */
		    ಆದರೆ (*cmd != '?')
			pos.ಲಂಬಸಾಲು = MAXCOL;
		    ಇಲ್ಲದಿದ್ದರೆ
			pos.ಲಂಬಸಾಲು = 0;
		    pos.coladd = 0;
		    ಆದರೆ (searchit(curwin, curbuf, &pos, ಖಾಲಿ,
				*cmd == '?' ? BACKWARD : FORWARD,
				(char_u *)"", 0, 1L, ಹುಡುಕು_ಸಂದೇಶ, i, ಖಾಲಿ) != FAIL)
			lnum = pos.lnum;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			cmd = ಖಾಲಿ;
			ಹೋಗು error;
		    }
		}
		++cmd;
		ನಿಲ್ಲಿಸು;

	    ಪೂರ್ವನಿಯೋಜಿತ:
		ಆದರೆ (VIM_ISDIGIT(*cmd))	// absolute line number
		    lnum = getdigits(&cmd);
	}

	ಗಾಗಿ (;;)
	{
	    cmd = skipwhite(cmd);
	    ಆದರೆ (*cmd != '-' && *cmd != '+' && !VIM_ISDIGIT(*cmd))
		ನಿಲ್ಲಿಸು;

	    ಆದರೆ (lnum == MAXLNUM)
	    {
		ಆಯ್ಕೆ (addr_type)
		{
		    ಸ್ಥಿತಿ ADDR_ಸಾಲುಗಳು:
		    ಸ್ಥಿತಿ ADDR_OTHER:
			// "+1" is same as ".+1"
			lnum = curwin->w_cursor.lnum;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_LOADED_BUFFERS:
		    ಸ್ಥಿತಿ ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_TABS_RELATIVE:
			lnum = 1;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_QUICKFIX:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#ಆದರೆ_ಅಂತ್ಯ
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_QUICKFIX_VALID:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#ಆದರೆ_ಅಂತ್ಯ
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ADDR_NONE:
		    ಸ್ಥಿತಿ ADDR_UNSIGNED:
			lnum = 0;
			ನಿಲ್ಲಿಸು;
		}
	    }

	    ಆದರೆ (VIM_ISDIGIT(*cmd))
		i = '+';		// "number" is same as "+number"
	    ಇಲ್ಲದಿದ್ದರೆ
		i = *cmd++;
	    ಆದರೆ (!VIM_ISDIGIT(*cmd))	// '+' is '+1'
		n = 1;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// "number", "+number" or "-number"
		n = getdigits(&cmd);
		ಆದರೆ (n == MAXLNUM)
		{
		    emsg(_(e_ಸಾಲು_ಸಂಖ್ಯೆ_ಹೊರಹೋಗು_ದ_range));
		    cmd = ಖಾಲಿ;
		    ಹೋಗು error;
		}
	    }

	    ಆದರೆ (addr_type == ADDR_TABS_RELATIVE)
	    {
		emsg(_(e_invalid_range));
		cmd = ಖಾಲಿ;
		ಹೋಗು error;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (addr_type == ADDR_LOADED_BUFFERS
		    || addr_type == ADDR_BUFFERS)
		lnum = compute_ಬಫರ್_local_ಎಣಿಕೆ(
				    addr_type, lnum, (i == '-') ? -1 * n : n);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		// Relative line addressing: need to adjust for lines in a
		// closed fold after the first address.
		ಆದರೆ (addr_type == ADDR_ಸಾಲುಗಳು && (i == '-' || i == '+')
							 && ವಿಳಾಸ_ಎಣಿಕೆ >= 2)
		    (ಶೂನ್ಯ)hasFolding(lnum, ಖಾಲಿ, &lnum);
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (i == '-')
		    lnum -= n;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (lnum >= 0 && n >= LONG_MAX - lnum)
		    {
			emsg(_(e_ಸಾಲು_ಸಂಖ್ಯೆ_ಹೊರಹೋಗು_ದ_range));
			cmd = ಖಾಲಿ;
			ಹೋಗು error;
		    }
		    lnum += n;
		}
	    }
	}
    } ಆಗ (*cmd == '/' || *cmd == '?');

error:
    *ptr = cmd;
    ಹಿಂತಿರುಗಿ lnum;
}

/*
 * Set eap->line1 and eap->line2 to the whole range.
 * Used for commands with the EX_DFLALL flag and no range given.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ವಿಳಾಸ_default_all(exarg_T *eap)
{
    eap->line1 = 1;
    ಆಯ್ಕೆ (eap->addr_type)
    {
	ಸ್ಥಿತಿ ADDR_ಸಾಲುಗಳು:
	ಸ್ಥಿತಿ ADDR_OTHER:
	    eap->line2 = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ADDR_LOADED_BUFFERS:
	    {
		buf_T *buf = firstbuf;

		ಆಗ (buf->b_ಮುಂದಿನ != ಖಾಲಿ && buf->b_ml.ml_mfp == ಖಾಲಿ)
		    buf = buf->b_ಮುಂದಿನ;
		eap->line1 = buf->b_fnum;
		buf = lastbuf;
		ಆಗ (buf->b_prev != ಖಾಲಿ && buf->b_ml.ml_mfp == ಖಾಲಿ)
		    buf = buf->b_prev;
		eap->line2 = buf->b_fnum;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ADDR_BUFFERS:
	    eap->line1 = firstbuf->b_fnum;
	    eap->line2 = lastbuf->b_fnum;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ADDR_WINDOWS:
	    eap->line2 = ಕೊನೆಯ_WIN_NR;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ADDR_TABS:
	    eap->line2 = ಕೊನೆಯ_TAB_NR;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ADDR_TABS_RELATIVE:
	    eap->line2 = 1;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ADDR_ARGUMENTS:
	    ಆದರೆ (ARGCOUNT == 0)
		eap->line1 = eap->line2 = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		eap->line2 = ARGCOUNT;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ADDR_QUICKFIX_VALID:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
	    eap->line2 = qf_get_valid_ಗಾತ್ರ(eap);
	    ಆದರೆ (eap->line2 == 0)
		eap->line2 = 1;
#ಆದರೆ_ಅಂತ್ಯ
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ADDR_NONE:
	ಸ್ಥಿತಿ ADDR_UNSIGNED:
	ಸ್ಥಿತಿ ADDR_QUICKFIX:
	    iemsg("Cannot use EX_DFLALL with ADDR_NONE, ADDR_UNSIGNED or ADDR_QUICKFIX");
	    ನಿಲ್ಲಿಸು;
    }
}


/*
 * Get flags from an Ex command argument.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_flags(exarg_T *eap)
{
    ಆಗ (vim_strchr((char_u *)"lp#", *eap->arg) != ಖಾಲಿ)
    {
	ಆದರೆ (*eap->arg == 'l')
	    eap->flags |= EXFLAG_LIST;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*eap->arg == 'p')
	    eap->flags |= EXFLAG_PRINT;
	ಇಲ್ಲದಿದ್ದರೆ
	    eap->flags |= EXFLAG_NR;
	eap->arg = skipwhite(eap->arg + 1);
    }
}

/*
 * Function called for command which is Not Implemented.  NI!
 */
    ಶೂನ್ಯ
ex_ni(exarg_T *eap)
{
    ಆದರೆ (!eap->skip)
	eap->errmsg =
		_(e_sorry_command_is_not_ಲಭ್ಯ_ಒಳ_this_ಆವೃತ್ತಿ);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_EX_SCRIPT_NI
/*
 * Function called for script command which is Not Implemented.  NI!
 * Skips over ":perl <<EOF" constructs.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_script_ni(exarg_T *eap)
{
    ಆದರೆ (!eap->skip)
	ex_ni(eap);
    ಇಲ್ಲದಿದ್ದರೆ
	vim_free(script_get(eap, eap->arg));
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Check range in Ex command for validity.
 * Return NULL when valid, error message when invalid.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
invalid_range(exarg_T *eap)
{
    buf_T	*buf;

    ಆದರೆ (       eap->line1 < 0
	    || eap->line2 < 0
	    || eap->line1 > eap->line2)
	ಹಿಂತಿರುಗಿ _(e_invalid_range);

    ಆದರೆ (eap->argt & EX_RANGE)
    {
	ಆಯ್ಕೆ (eap->addr_type)
	{
	    ಸ್ಥಿತಿ ADDR_ಸಾಲುಗಳು:
		ಆದರೆ (eap->line2 > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
			    + (eap->cmdidx == CMD_diffget ||
				eap->cmdidx == CMD_diffput)
#ಆದರೆ_ಅಂತ್ಯ
		   )
		    ಹಿಂತಿರುಗಿ _(e_invalid_range);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ADDR_ARGUMENTS:
		// add 1 if ARGCOUNT is 0
		ಆದರೆ (eap->line2 > ARGCOUNT + (!ARGCOUNT))
		    ಹಿಂತಿರುಗಿ _(e_invalid_range);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ADDR_BUFFERS:
		// Only a boundary check, not whether the buffers actually
		// exist.
		ಆದರೆ (eap->line1 < 1 || eap->line2 > get_highest_fnum())
		    ಹಿಂತಿರುಗಿ _(e_invalid_range);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ADDR_LOADED_BUFFERS:
		buf = firstbuf;
		ಆಗ (buf->b_ml.ml_mfp == ಖಾಲಿ)
		{
		    ಆದರೆ (buf->b_ಮುಂದಿನ == ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ _(e_invalid_range);
		    buf = buf->b_ಮುಂದಿನ;
		}
		ಆದರೆ (eap->line1 < buf->b_fnum)
		    ಹಿಂತಿರುಗಿ _(e_invalid_range);
		buf = lastbuf;
		ಆಗ (buf->b_ml.ml_mfp == ಖಾಲಿ)
		{
		    ಆದರೆ (buf->b_prev == ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ _(e_invalid_range);
		    buf = buf->b_prev;
		}
		ಆದರೆ (eap->line2 > buf->b_fnum)
		    ಹಿಂತಿರುಗಿ _(e_invalid_range);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ADDR_WINDOWS:
		ಆದರೆ (eap->line2 > ಕೊನೆಯ_WIN_NR)
		    ಹಿಂತಿರುಗಿ _(e_invalid_range);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ADDR_TABS:
		ಆದರೆ (eap->line2 > ಕೊನೆಯ_TAB_NR)
		    ಹಿಂತಿರುಗಿ _(e_invalid_range);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ADDR_TABS_RELATIVE:
	    ಸ್ಥಿತಿ ADDR_OTHER:
		// Any range is OK.
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ADDR_QUICKFIX:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
		// No error for value that is too big, will use the last entry.
		ಆದರೆ (eap->line2 <= 0)
		{
		    ಆದರೆ (eap->addr_ಎಣಿಕೆ == 0)
			ಹಿಂತಿರುಗಿ _(e_no_errors);
		    ಹಿಂತಿರುಗಿ _(e_invalid_range);
		}
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ADDR_QUICKFIX_VALID:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
		ಆದರೆ ((eap->line2 != 1 && eap->line2 > qf_get_valid_ಗಾತ್ರ(eap))
			|| eap->line2 < 0)
		    ಹಿಂತಿರುಗಿ _(e_invalid_range);
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ADDR_UNSIGNED:
	    ಸ್ಥಿತಿ ADDR_NONE:
		// Will give an error elsewhere.
		ನಿಲ್ಲಿಸು;
	}
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Correct the range for zero line number, if required.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
correct_range(exarg_T *eap)
{
    ಆದರೆ (!(eap->argt & EX_ZEROR))	    // zero in range not allowed
    {
	ಆದರೆ (eap->line1 == 0)
	    eap->line1 = 1;
	ಆದರೆ (eap->line2 == 0)
	    eap->line2 = 1;
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
/*
 * For a ":vimgrep" or ":vimgrepadd" command return a pointer past the
 * pattern.  Otherwise return eap->arg.
 */
    ಸ್ಥಿರ char_u *
skip_grep_pat(exarg_T *eap)
{
    char_u	*p = eap->arg;

    ಆದರೆ (*p != NUL && (eap->cmdidx == CMD_vimgrep || eap->cmdidx == CMD_lvimgrep
		|| eap->cmdidx == CMD_vimgrepadd
		|| eap->cmdidx == CMD_lvimgrepadd
		|| grep_ಆಂತರಿಕ(eap->cmdidx)))
    {
	p = skip_vimgrep_pat(p, ಖಾಲಿ, ಖಾಲಿ);
	ಆದರೆ (p == ಖಾಲಿ)
	    p = eap->arg;
    }
    ಹಿಂತಿರುಗಿ p;
}

/*
 * For the ":make" and ":grep" commands insert the 'makeprg'/'grepprg' option
 * in the command line, so that things like % get expanded.
 */
    ಸ್ಥಿರ char_u *
replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep)
{
    char_u	*new_cmdline;
    char_u	*ಕಾರ್ಯಕ್ರಮ;
    char_u	*pos;
    char_u	*ptr;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		i;

    /*
     * Don't do it when ":vimgrep" is used for ":grep".
     */
    ಆದರೆ ((eap->cmdidx == CMD_make || eap->cmdidx == CMD_lmake
		     || eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
		     || eap->cmdidx == CMD_grepadd
		     || eap->cmdidx == CMD_lgrepadd)
	    && !grep_ಆಂತರಿಕ(eap->cmdidx))
    {
	ಆದರೆ (eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
	    || eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	{
	    ಆದರೆ (*curbuf->b_p_gp == NUL)
		ಕಾರ್ಯಕ್ರಮ = p_gp;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಕಾರ್ಯಕ್ರಮ = curbuf->b_p_gp;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (*curbuf->b_p_mp == NUL)
		ಕಾರ್ಯಕ್ರಮ = p_mp;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಕಾರ್ಯಕ್ರಮ = curbuf->b_p_mp;
	}

	p = skipwhite(p);

	ಆದರೆ ((pos = (char_u *)ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)ಕಾರ್ಯಕ್ರಮ, "$*")) != ಖಾಲಿ)
	{
	    // replace $* by given arguments
	    i = 1;
	    ಆಗ ((pos = (char_u *)ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)pos + 2, "$*")) != ಖಾಲಿ)
		++i;
	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(p);
	    new_cmdline = alloc(STRLEN(ಕಾರ್ಯಕ್ರಮ) + (size_t)i * (len - 2) + 1);
	    ಆದರೆ (new_cmdline == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;			// out of memory
	    ptr = new_cmdline;
	    ಆಗ ((pos = (char_u *)ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)ಕಾರ್ಯಕ್ರಮ, "$*")) != ಖಾಲಿ)
	    {
		i = (ಪೂರ್ಣಾಂಕ)(pos - ಕಾರ್ಯಕ್ರಮ);
		STRNCPY(ptr, ಕಾರ್ಯಕ್ರಮ, i);
		STRCPY(ptr += i, p);
		ptr += len;
		ಕಾರ್ಯಕ್ರಮ = pos + 2;
	    }
	    STRCPY(ptr, ಕಾರ್ಯಕ್ರಮ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    size_t ಕಾರ್ಯಕ್ರಮ_len = STRLEN(ಕಾರ್ಯಕ್ರಮ);

	    new_cmdline = alloc(ಕಾರ್ಯಕ್ರಮ_len + STRLEN(p) + 2);
	    ಆದರೆ (new_cmdline == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;			// out of memory
	    STRCPY(new_cmdline, ಕಾರ್ಯಕ್ರಮ);
	    STRCPY(new_cmdline + ಕಾರ್ಯಕ್ರಮ_len, " ");
	    STRCPY(new_cmdline + ಕಾರ್ಯಕ್ರಮ_len + 1, p);
	}
	ಸಂದೇಶ_make(p);

	// 'eap->cmd' is not set here, because it is not used at CMD_make
	vim_free(*cmdlinep);
	*cmdlinep = new_cmdline;
	p = new_cmdline;
    }
    ಹಿಂತಿರುಗಿ p;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Expand file name in Ex command argument.
 * When an error is detected, "errormsgp" is set to a non-NULL pointer.
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
expand_filename(
    exarg_T	*eap,
    char_u	**cmdlinep,
    ಅಕ್ಷರ	**errormsgp)
{
    ಪೂರ್ಣಾಂಕ		has_wildcards;	// need to expand wildcards
    char_u	*repl;
    size_t	srclen;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		escaped;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    // Skip a regexp pattern for ":vimgrep[add] pat file..."
    p = skip_grep_pat(eap);
#ಇಲ್ಲದಿದ್ದರೆ
    p = eap->arg;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Decide to expand wildcards *before* replacing '%', '#', etc.  If
     * the file name contains a wildcard it should not cause expanding.
     * (it will be expanded anyway if there is a wildcard before replacing).
     */
    has_wildcards = mch_has_wildcard(p);
    ಆಗ (*p != NUL)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// Skip over `=expr`, wildcards in it are not expanded.
	ಆದರೆ (p[0] == '`' && p[1] == '=')
	{
	    p += 2;
	    (ಶೂನ್ಯ)skip_ಅಭಿವ್ಯಕ್ತಿ(&p, ಖಾಲಿ);
	    ಆದರೆ (*p == '`')
		++p;
	    ಮುಂದುವರಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ
	/*
	 * Quick check if this cannot be the start of a special string.
	 * Also removes backslash before '%', '#' and '<'.
	 */
	ಆದರೆ (vim_strchr((char_u *)"%#<", *p) == ಖಾಲಿ)
	{
	    ++p;
	    ಮುಂದುವರಿ;
	}

	/*
	 * Try to find a match at this position.
	 */
	repl = eval_vars(p, eap->arg, &srclen, &(eap->do_ecmd_lnum),
						    errormsgp, &escaped, TRUE);
	ಆದರೆ (*errormsgp != ಖಾಲಿ)		// error detected
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (repl == ಖಾಲಿ)		// no match found
	{
	    p += srclen;
	    ಮುಂದುವರಿ;
	}

	// Wildcards won't be expanded below, the replacement is taken
	// literally.  But do expand "~/file", "~user/file" and "$HOME/file".
	ಆದರೆ (vim_strchr(repl, '$') != ಖಾಲಿ || vim_strchr(repl, '~') != ಖಾಲಿ)
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
	ಆದರೆ (!eap->usefilter
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
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UNIX
		&& !(eap->argt & EX_NOSPC)
#ಆದರೆ_ಅಂತ್ಯ
		)
	{
	    char_u	*l;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
	    // Don't escape a backslash here, because rem_backslash() doesn't
	    // remove it later.
	    ಸ್ಥಿರ char_u *nobslash = (char_u *)" \t\"|";
# define ESCAPE_CHARS nobslash
#ಇಲ್ಲದಿದ್ದರೆ
# define ESCAPE_CHARS escape_chars
#ಆದರೆ_ಅಂತ್ಯ

	    ಗಾಗಿ (l = repl; *l; ++l)
		ಆದರೆ (vim_strchr(ESCAPE_CHARS, *l) != ಖಾಲಿ)
		{
		    l = vim_strsave_escaped(repl, ESCAPE_CHARS);
		    ಆದರೆ (l != ಖಾಲಿ)
		    {
			vim_free(repl);
			repl = l;
		    }
		    ನಿಲ್ಲಿಸು;
		}
	}

	// For a shell command a '!' must be escaped.
	ಆದರೆ ((eap->usefilter || eap->cmdidx == CMD_bang
						|| eap->cmdidx == CMD_terminal)
			    && vim_strpbrk(repl, (char_u *)"!") != ಖಾಲಿ)
	{
	    char_u	*l;

	    l = vim_strsave_escaped(repl, (char_u *)"!");
	    ಆದರೆ (l != ಖಾಲಿ)
	    {
		vim_free(repl);
		repl = l;
	    }
	}

	p = repl_cmdline(eap, p, srclen, repl, cmdlinep);
	vim_free(repl);
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    /*
     * One file argument: Expand wildcards.
     * Don't do this with ":r !command" or ":w !command".
     */
    ಆದರೆ ((eap->argt & EX_NOSPC) && !eap->usefilter)
    {
	/*
	 * May do this twice:
	 * 1. Replace environment variables.
	 * 2. Replace any other wildcards, remove backslashes.
	 */
	ಗಾಗಿ (n = 1; n <= 2; ++n)
	{
	    ಆದರೆ (n == 2)
	    {
		/*
		 * Halve the number of backslashes (this is Vi compatible).
		 * For Unix and OS/2, when wildcards are expanded, this is
		 * done by ExpandOne() below.
		 */
#ಆದರೆ defined(UNIX)
		ಆದರೆ (!has_wildcards)
#ಆದರೆ_ಅಂತ್ಯ
		    backslash_halve(eap->arg);
	    }

	    ಆದರೆ (has_wildcards)
	    {
		ಆದರೆ (n == 1)
		{
		    /*
		     * First loop: May expand environment variables.  This
		     * can be done much faster with expand_env() than with
		     * something else (e.g., calling a shell).
		     * After expanding environment variables, check again
		     * if there are still wildcards present.
		     */
		    ಆದರೆ (vim_strchr(eap->arg, '$') != ಖಾಲಿ
			    || vim_strchr(eap->arg, '~') != ಖಾಲಿ)
		    {
			expand_env_esc(eap->arg, NameBuff, MAXPATHL,
					(char_u *)(" \t" ಮಾರ್ಗ_ESC_WILDCARDS),
					TRUE, ಖಾಲಿ);
			has_wildcards = mch_has_wildcard(NameBuff);
			p = NameBuff;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			p = ಖಾಲಿ;
		}
		ಇಲ್ಲದಿದ್ದರೆ // n == 2
		{
		    expand_T	xpc;
		    ಪೂರ್ಣಾಂಕ		options = WILD_LIST_NOTFOUND
					       | WILD_NOERROR | WILD_ಕೂಡಿಸು_SLASH;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    ಆದರೆ (p_wic)
			options += WILD_ICASE;
		    p = ExpandOne(&xpc, eap->arg, ಖಾಲಿ,
						   options, WILD_EXPAND_FREE);
		    ಆದರೆ (p == ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ FAIL;
		}
		ಆದರೆ (p != ಖಾಲಿ)
		{
		    (ಶೂನ್ಯ)repl_cmdline(eap, eap->arg, STRLEN(eap->arg),
								 p, cmdlinep);
		    ಆದರೆ (n == 2)	// p came from ExpandOne()
			vim_free(p);
		}
	    }
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Replace part of the command line, keeping eap->cmd, eap->arg and
 * eap->nextcmd correct.
 * "src" points to the part that is to be replaced, of length "srclen".
 * "repl" is the replacement string.
 * Returns a pointer to the character after the replaced string.
 * Returns NULL for failure.
 */
    ಸ್ಥಿರ char_u *
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
    ಆದರೆ (eap->nextcmd != ಖಾಲಿ)
	i += STRLEN(eap->nextcmd);	// add space for next command
    ಆದರೆ ((new_cmdline = alloc(i)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;			// out of memory!

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

    ಆದರೆ (eap->nextcmd != ಖಾಲಿ)		// append next command
    {
	new_cmdlinelen += taillen + 1;
	STRCPY(new_cmdline + new_cmdlinelen, eap->nextcmd);
	eap->nextcmd = new_cmdline + new_cmdlinelen;
    }
    eap->cmd = new_cmdline + (eap->cmd - *cmdlinep);
    eap->arg = new_cmdline + (eap->arg - *cmdlinep);
    ಆದರೆ (eap->do_ecmd_cmd != ಖಾಲಿ && eap->do_ecmd_cmd != dollar_command)
	eap->do_ecmd_cmd = new_cmdline + (eap->do_ecmd_cmd - *cmdlinep);
    vim_free(*cmdlinep);
    *cmdlinep = new_cmdline;

    ಹಿಂತಿರುಗಿ src;
}

/*
 * Check for '|' to separate commands and '"' to start comments.
 * If "keep_backslash" is TRUE do not remove any backslash.
 */
    ಶೂನ್ಯ
separate_nextcmd(exarg_T *eap, ಪೂರ್ಣಾಂಕ keep_backslash)
{
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();
    char_u	*p;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    p = skip_grep_pat(eap);
#ಇಲ್ಲದಿದ್ದರೆ
    p = eap->arg;
#ಆದರೆ_ಅಂತ್ಯ

    ಗಾಗಿ ( ; *p; MB_PTR_ADV(p))
    {
	ಆದರೆ (*p == Ctrl_V)
	{
	    ಆದರೆ ((eap->argt & (EX_CTRLV | EX_XFILE)) || keep_backslash)
		++p;		// skip CTRL-V and next char
	    ಇಲ್ಲದಿದ್ದರೆ
				// remove CTRL-V and skip next char
		STRMOVE(p, p + 1);
	    ಆದರೆ (*p == NUL)		// stop at NUL after CTRL-V
		ನಿಲ್ಲಿಸು;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// Skip over `=expr` when wildcards are expanded.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == '`' && p[1] == '=' && (eap->argt & EX_XFILE))
	{
	    p += 2;
	    (ಶೂನ್ಯ)skip_ಅಭಿವ್ಯಕ್ತಿ(&p, ಖಾಲಿ);
	    ಆದರೆ (*p == NUL)		// stop at NUL after CTRL-V
		ನಿಲ್ಲಿಸು;
	}
#ಆದರೆ_ಅಂತ್ಯ

	// Check for '"'/'#': start of comment or '|': next command
	// :@" and :*" do not start a comment!
	// :redir @" doesn't either.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((*p == '"'
		    && !vim9script
		    && !(eap->argt & EX_NOTRLCOM)
		    && ((eap->cmdidx != CMD_at && eap->cmdidx != CMD_ನಕ್ಷತ್ರ)
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
		    && eap->cmdidx != CMD_ಸೇರಿಸು)
		|| *p == '\n')
	{
	    /*
	     * We remove the '\' before the '|', unless EX_CTRLV is used
	     * AND 'b' is present in 'cpoptions'.
	     */
	    ಆದರೆ ((vim_strchr(p_cpo, CPO_BAR) == ಖಾಲಿ
			      || !(eap->argt & EX_CTRLV)) && *(p - 1) == '\\')
	    {
		ಆದರೆ (!keep_backslash)
		{
		    STRMOVE(p - 1, p);	// remove the '\'
		    --p;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಣ_nextcmd(eap, p);
		*p = NUL;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    ಆದರೆ (!(eap->argt & EX_NOTRLCOM))	// remove trailing spaces
	del_trailing_spaces(eap->arg);
}

/*
 * get + command from ex argument
 */
    ಸ್ಥಿರ char_u *
getargcmd(char_u **argp)
{
    char_u *arg = *argp;
    char_u *command = ಖಾಲಿ;

    ಆದರೆ (*arg == '+')	    // +[command]
    {
	++arg;
	ಆದರೆ (vim_isspace(*arg) || *arg == NUL)
	    command = dollar_command;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    command = arg;
	    arg = skip_cmd_arg(command, TRUE);
	    ಆದರೆ (*arg != NUL)
		*arg++ = NUL;		// terminate command with NUL
	}

	arg = skipwhite(arg);	// skip over spaces
	*argp = arg;
    }
    ಹಿಂತಿರುಗಿ command;
}

/*
 * Find end of "+command" argument.  Skip over "\ " and "\\".
 */
    char_u *
skip_cmd_arg(
    char_u *p,
    ಪೂರ್ಣಾಂಕ	   rembs)	// TRUE to halve the number of backslashes
{
    ಆಗ (*p && !vim_isspace(*p))
    {
	ಆದರೆ (*p == '\\' && p[1] != NUL)
	{
	    ಆದರೆ (rembs)
		STRMOVE(p, p + 1);
	    ಇಲ್ಲದಿದ್ದರೆ
		++p;
	}
	MB_PTR_ADV(p);
    }
    ಹಿಂತಿರುಗಿ p;
}

    ಪೂರ್ಣಾಂಕ
get_bad_opt(char_u *p, exarg_T *eap)
{
    ಆದರೆ (STRICMP(p, "keep") == 0)
	eap->bad_char = BAD_KEEP;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(p, "drop") == 0)
	eap->bad_char = BAD_DROP;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (MB_BYTE2LEN(*p) == 1 && p[1] == NUL)
	eap->bad_char = *p;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of bad= names.
 */
    ಸ್ಥಿರ char_u *
get_bad_name(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    // Note: Keep this in sync with getargopt.
    ಸ್ಥಿರ ಅಕ್ಷರ *(p_bad_ಮೌಲ್ಯಗಳು[]) =
    {
	"?",
	"keep",
	"drop",
    };

    ಆದರೆ (idx < 0 || idx >= (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(p_bad_ಮೌಲ್ಯಗಳು))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ (char_u*)p_bad_ಮೌಲ್ಯಗಳು[idx];
}

/*
 * Get "++opt=arg" argument.
 * Return FAIL or OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
getargopt(exarg_T *eap)
{
    char_u	*arg = eap->arg + 2;
    ಪೂರ್ಣಾಂಕ		*pp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		bad_char_idx;
    char_u	*p;

    // Note: Keep this in sync with get_argopt_name.

    // ":edit ++[no]bin[ary] file"
    ಆದರೆ (STRNCMP(arg, "bin", 3) == 0 || STRNCMP(arg, "nobin", 5) == 0)
    {
	ಆದರೆ (*arg == 'n')
	{
	    arg += 2;
	    eap->force_bin = FORCE_NOBIN;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    eap->force_bin = FORCE_BIN;
	ಆದರೆ (!checkforcmd(&arg, "binary", 3))
	    ಹಿಂತಿರುಗಿ FAIL;
	eap->arg = skipwhite(arg);
	ಹಿಂತಿರುಗಿ OK;
    }

    // ":read ++edit file"
    ಆದರೆ (STRNCMP(arg, "edit", 4) == 0)
    {
	eap->read_edit = TRUE;
	eap->arg = skipwhite(arg + 4);
	ಹಿಂತಿರುಗಿ OK;
    }

    ಆದರೆ (STRNCMP(arg, "ff", 2) == 0)
    {
	arg += 2;
	pp = &eap->force_ff;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "fileformat", 10) == 0)
    {
	arg += 10;
	pp = &eap->force_ff;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "enc", 3) == 0)
    {
	ಆದರೆ (STRNCMP(arg, "encoding", 8) == 0)
	    arg += 8;
	ಇಲ್ಲದಿದ್ದರೆ
	    arg += 3;
	pp = &eap->force_enc;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "bad", 3) == 0)
    {
	arg += 3;
	pp = &bad_char_idx;
    }

    ಆದರೆ (pp == ಖಾಲಿ || *arg != '=')
	ಹಿಂತಿರುಗಿ FAIL;

    ++arg;
    *pp = (ಪೂರ್ಣಾಂಕ)(arg - eap->cmd);
    arg = skip_cmd_arg(arg, FALSE);
    eap->arg = skipwhite(arg);
    *arg = NUL;

    ಆದರೆ (pp == &eap->force_ff)
    {
	ಆದರೆ (check_ff_ಮೌಲ್ಯ(eap->cmd + eap->force_ff) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	eap->force_ff = eap->cmd[eap->force_ff];
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pp == &eap->force_enc)
    {
	// Make 'fileencoding' lower case.
	ಗಾಗಿ (p = eap->cmd + eap->force_enc; *p != NUL; ++p)
	    *p = TOLOWER_ASC(*p);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Check ++bad= argument.  Must be a single-byte character, "keep" or
	// "drop".
	ಆದರೆ (get_bad_opt(eap->cmd + bad_char_idx, eap) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of ++opt names.
 */
    ಸ್ಥಿರ char_u *
get_argopt_name(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    // Note: Keep this in sync with getargopt.
    ಸ್ಥಿರ ಅಕ್ಷರ *(p_opt_ಮೌಲ್ಯಗಳು[]) =
    {
	"fileformat=",
	"encoding=",
	"binary",
	"nobinary",
	"bad=",
	"edit",
    };

    ಆದರೆ (idx < 0 || idx >= (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(p_opt_ಮೌಲ್ಯಗಳು))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ (char_u*)p_opt_ಮೌಲ್ಯಗಳು[idx];
}

/*
 * Command-line expansion for ++opt=name.
 */
    ಪೂರ್ಣಾಂಕ
expand_argopt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	ಪೂರ್ಣಾಂಕ	    *ಅಂಕೆ_Matches)
{
    ಆದರೆ (xp->xp_ನಮೂನೆ > xp->xp_ಸಾಲು && *(xp->xp_ನಮೂನೆ-1) == '=')
    {
	char_u *(*cb)(expand_T *, ಪೂರ್ಣಾಂಕ) = ಖಾಲಿ;

	char_u *name_ಕೊನೆ = xp->xp_ನಮೂನೆ - 1;
	ಆದರೆ (name_ಕೊನೆ - xp->xp_ಸಾಲು >= 2
		&& STRNCMP(name_ಕೊನೆ - 2, "ff", 2) == 0)
	    cb = get_fileformat_name;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name_ಕೊನೆ - xp->xp_ಸಾಲು >= 10
		&& STRNCMP(name_ಕೊನೆ - 10, "fileformat", 10) == 0)
	    cb = get_fileformat_name;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name_ಕೊನೆ - xp->xp_ಸಾಲು >= 3
		&& STRNCMP(name_ಕೊನೆ - 3, "enc", 3) == 0)
	    cb = get_encoding_name;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name_ಕೊನೆ - xp->xp_ಸಾಲು >= 8
		&& STRNCMP(name_ಕೊನೆ - 8, "encoding", 8) == 0)
	    cb = get_encoding_name;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name_ಕೊನೆ - xp->xp_ಸಾಲು >= 3
		&& STRNCMP(name_ಕೊನೆ - 3, "bad", 3) == 0)
	    cb = get_bad_name;

	ಆದರೆ (cb != ಖಾಲಿ)
	{
	    ಹಿಂತಿರುಗಿ ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    ಅಂಕೆ_Matches,
		    cb,
		    FALSE);
	}
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Special handling of "ff" which acts as a short form of
    // "fileformat", as "ff" is not a substring of it.
    ಆದರೆ (xp->xp_ನಮೂನೆ_len == 2
	    && STRNCMP(xp->xp_ನಮೂನೆ, "ff", xp->xp_ನಮೂನೆ_len) == 0)
    {
	*matches = ALLOC_MULT(char_u *, 1);
	ಆದರೆ (*matches == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	*ಅಂಕೆ_Matches = 1;
	(*matches)[0] = vim_strsave((char_u*)"fileformat=");
	ಹಿಂತಿರುಗಿ OK;
    }

    ಹಿಂತಿರುಗಿ ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    ಅಂಕೆ_Matches,
	    get_argopt_name,
	    FALSE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_autocmd(exarg_T *eap)
{
    ಆದರೆ (eap->skip)
    {
	skip_cmd_ಖಂಡ(eap);
	ಹಿಂತಿರುಗಿ;
    }

    /*
     * Disallow autocommands from .exrc and .vimrc in current
     * directory for security reasons.
     */
    ಆದರೆ (secure)
    {
	secure = 2;
	eap->errmsg =
	      _(e_command_not_allowed_from_vimrc_ಒಳ_current_dir_or_ಟ್ಯಾಗ್_ಹುಡುಕು);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_autocmd)
	do_autocmd(eap, eap->arg, eap->forceit);
    ಇಲ್ಲದಿದ್ದರೆ
	do_augroup(eap->arg, eap->forceit);
}

/*
 * ":doautocmd": Apply the automatic commands to the current buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_doautocmd(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಂಕ		call_do_modelines = check_nomodeline(&arg);
    ಪೂರ್ಣಾಂಕ		did_aucmd;

    (ಶೂನ್ಯ)do_doautocmd(arg, TRUE, &did_aucmd);
    // Only when there is no <nomodeline>.
    ಆದರೆ (call_do_modelines && did_aucmd)
	do_modelines(0);
}

/*
 * :[N]bunload[!] [N] [bufname] unload buffer
 * :[N]bdelete[!] [N] [bufname] delete buffer from buffer list
 * :[N]bwipeout[!] [N] [bufname] delete buffer really
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_bunload(exarg_T *eap)
{
    ಆದರೆ (ERROR_IF_ANY_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;
    eap->errmsg = do_bufdel(
	    eap->cmdidx == CMD_bdelete ? DOBUF_DEL
		: eap->cmdidx == CMD_bwipeout ? DOBUF_WIPE
		: DOBUF_UNLOAD, eap->arg,
	    eap->addr_ಎಣಿಕೆ, (ಪೂರ್ಣಾಂಕ)eap->line1, (ಪೂರ್ಣಾಂಕ)eap->line2, eap->forceit);
}

/*
 * :[N]buffer [N]	to buffer N
 * :[N]sbuffer [N]	to buffer N
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_ಬಫರ್(exarg_T *eap)
{
    ಆದರೆ (ERROR_IF_ANY_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;
    do_exbuffer(eap);
}

/*
 * ":buffer" command and alike.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_exbuffer(exarg_T *eap)
{
    ಆದರೆ (*eap->arg)
	eap->errmsg = ex_errmsg(e_trailing_characters_str, eap->arg);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (eap->addr_ಎಣಿಕೆ == 0)	// default is current buffer
	    goto_ಬಫರ್(eap, DOBUF_CURRENT, FORWARD, 0);
	ಇಲ್ಲದಿದ್ದರೆ
	    goto_ಬಫರ್(eap, DOBUF_ಮೊದಲ, FORWARD, (ಪೂರ್ಣಾಂಕ)eap->line2);
	ಆದರೆ (eap->do_ecmd_cmd != ಖಾಲಿ)
	    do_cmd_ವಾದ(eap->do_ecmd_cmd);
    }
}

/*
 * :[N]bmodified [N]	to next mod. buffer
 * :[N]sbmodified [N]	to next mod. buffer
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_bmodified(exarg_T *eap)
{
    goto_ಬಫರ್(eap, DOBUF_MOD, FORWARD, (ಪೂರ್ಣಾಂಕ)eap->line2);
    ಆದರೆ (eap->do_ecmd_cmd != ಖಾಲಿ)
	do_cmd_ವಾದ(eap->do_ecmd_cmd);
}

/*
 * :[N]bnext [N]	to next buffer
 * :[N]sbnext [N]	split and to next buffer
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_bnext(exarg_T *eap)
{
    ಆದರೆ (ERROR_IF_ANY_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;

    goto_ಬಫರ್(eap, DOBUF_CURRENT, FORWARD, (ಪೂರ್ಣಾಂಕ)eap->line2);
    ಆದರೆ (eap->do_ecmd_cmd != ಖಾಲಿ)
	do_cmd_ವಾದ(eap->do_ecmd_cmd);
}

/*
 * :[N]bNext [N]	to previous buffer
 * :[N]bprevious [N]	to previous buffer
 * :[N]sbNext [N]	split and to previous buffer
 * :[N]sbprevious [N]	split and to previous buffer
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_bprevious(exarg_T *eap)
{
    ಆದರೆ (ERROR_IF_ANY_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;

    goto_ಬಫರ್(eap, DOBUF_CURRENT, BACKWARD, (ಪೂರ್ಣಾಂಕ)eap->line2);
    ಆದರೆ (eap->do_ecmd_cmd != ಖಾಲಿ)
	do_cmd_ವಾದ(eap->do_ecmd_cmd);
}

/*
 * :brewind		to first buffer
 * :bfirst		to first buffer
 * :sbrewind		split and to first buffer
 * :sbfirst		split and to first buffer
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_brewind(exarg_T *eap)
{
    ಆದರೆ (ERROR_IF_ANY_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;

    goto_ಬಫರ್(eap, DOBUF_ಮೊದಲ, FORWARD, 0);
    ಆದರೆ (eap->do_ecmd_cmd != ಖಾಲಿ)
	do_cmd_ವಾದ(eap->do_ecmd_cmd);
}

/*
 * :blast		to last buffer
 * :sblast		split and to last buffer
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_blast(exarg_T *eap)
{
    ಆದರೆ (ERROR_IF_ANY_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;

    goto_ಬಫರ್(eap, DOBUF_ಕೊನೆಯ, BACKWARD, 0);
    ಆದರೆ (eap->do_ecmd_cmd != ಖಾಲಿ)
	do_cmd_ವಾದ(eap->do_ecmd_cmd);
}

/*
 * Check if "c" ends an Ex command.
 * In Vim9 script does not check for white space before #.
 */
    ಪೂರ್ಣಾಂಕ
ತುದಿಗಳು_excmd(ಪೂರ್ಣಾಂಕ c)
{
    ಪೂರ್ಣಾಂಕ comment_char = '"';

    ಆದರೆ (ಒಳ_vim9script())
	comment_char = '#';
    ಹಿಂತಿರುಗಿ (c == NUL || c == '|' || c == comment_char || c == '\n');
}

/*
 * Like ends_excmd() but checks that a # in Vim9 script either has "cmd" equal
 * to "cmd_start" or has a white space character before it.
 */
    ಪೂರ್ಣಾಂಕ
ತುದಿಗಳು_excmd2(char_u *cmd_ಆರಂಭ ಬಳಸದ, char_u *cmd)
{
    ಪೂರ್ಣಾಂಕ c = *cmd;

    ಆದರೆ (c == NUL || c == '|' || c == '\n')
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (ಒಳ_vim9script())
	//  # starts a comment, #{ might be a mistake, #{{ can start a fold
	ಹಿಂತಿರುಗಿ c == '#' && (cmd[1] != '{' || cmd[2] == '{')
				 && (cmd == cmd_ಆರಂಭ || VIM_ISWHITE(cmd[-1]));
    ಹಿಂತಿರುಗಿ c == '"';
}

#ಆದರೆ defined(FEAT_SYN_HL) || defined(FEAT_ಹುಡುಕು_EXTRA) || defined(FEAT_EVAL)
/*
 * Return the next command, after the first '|' or '\n'.
 * Return NULL if not found.
 */
    char_u *
ಕಂಡುಹಿಡಿ_nextcmd(char_u *p)
{
    ಆಗ (*p != '|' && *p != '\n')
    {
	ಆದರೆ (*p == NUL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	++p;
    }
    ಹಿಂತಿರುಗಿ (p + 1);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Check if *p is a separator between Ex commands, skipping over white space.
 * Return NULL if it isn't, the following character if it is.
 */
    char_u *
check_nextcmd(char_u *p)
{
    char_u *s = skipwhite(p);

    ಆದರೆ (*s == '|' || *s == '\n')
	ಹಿಂತಿರುಗಿ (s + 1);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * If "eap->nextcmd" is not set, check for a next command at "p".
 */
    ಶೂನ್ಯ
ಗಣ_nextcmd(exarg_T *eap, char_u *arg)
{
    char_u *p = skipwhite(arg);

    ಆದರೆ (ಒಳ_vim9script() && *p == '#')
    {
	char_u *nl = vim_strchr(p, NL);
	ಆದರೆ (nl != ಖಾಲಿ)
	    p = nl;
    }

    p = check_nextcmd(p);

    ಆದರೆ (eap->nextcmd == ಖಾಲಿ)
	eap->nextcmd = p;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p != ಖಾಲಿ)
	// cannot use "| command" inside a  {} block
	semsg(_(e_cannot_use_bar_ಗಮ್ಯ_separate_commands_ಇಲ್ಲಿ_str), arg);
}

/*
 * - if there are more files to edit
 * - and this is the last window
 * - and forceit not used
 * - and not repeated twice on a row
 *    return FAIL and give error message if 'message' TRUE
 * return OK otherwise
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_ಇನ್ನಷ್ಟು(
    ಪೂರ್ಣಾಂಕ ಸಂದೇಶ_2,	    // when FALSE check only, no messages
    ಪೂರ್ಣಾಂಕ forceit)
{
    ಪೂರ್ಣಾಂಕ	    n = ARGCOUNT - curwin->w_arg_idx - 1;

    ಆದರೆ (!forceit && only_one_ಕಿಟಕಿ()
	    && ARGCOUNT > 1 && !arg_had_ಕೊನೆಯ && n > 0 && quitmore == 0)
    {
	ಆದರೆ (ಸಂದೇಶ_2)
	{
#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ಆದರೆ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						    && curbuf->b_fname != ಖಾಲಿ)
	    {
		char_u	buff[DIALOG_ಸಂದೇಶ_ಗಾತ್ರ];

		vim_snprintf((ಅಕ್ಷರ *)buff, DIALOG_ಸಂದೇಶ_ಗಾತ್ರ,
			NGETTEXT("%d more file to edit.  Quit anyway?",
			    "%d more files to edit.  Quit anyway?", n), n);
		ಆದರೆ (vim_dialog_yesno(VIM_QUESTION, ಖಾಲಿ, buff, 1) == VIM_YES)
		    ಹಿಂತಿರುಗಿ OK;
		ಹಿಂತಿರುಗಿ FAIL;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    semsg(NGETTEXT(e_nr_ಇನ್ನಷ್ಟು_file_ಗಮ್ಯ_edit,
			   e_nr_ಇನ್ನಷ್ಟು_files_ಗಮ್ಯ_edit , n), n);
	    quitmore = 2;	    // next try to quit is allowed
	}
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of command names.
 */
    char_u *
get_command_name(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx >= (ಪೂರ್ಣಾಂಕ)CMD_ಗಾತ್ರ)
	ಹಿಂತಿರುಗಿ expand_user_command_name(idx);
    // the following are no real commands
    ಆದರೆ (STRNCMP(cmdnames[idx].cmd_name, "{", 1) == 0 ||
	STRNCMP(cmdnames[idx].cmd_name, "}", 1) == 0)
	ಹಿಂತಿರುಗಿ (char_u *)"";
    ಹಿಂತಿರುಗಿ cmdnames[idx].cmd_name;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_colorscheme(exarg_T *eap)
{
    ಆದರೆ (*eap->arg == NUL)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	char_u *ಅಭಿವ್ಯಕ್ತಿ = vim_strsave((char_u *)"g:colors_name");
	char_u *p = ಖಾಲಿ;

	ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ != ಖಾಲಿ)
	{
	    ++emsg_off;
	    p = eval_ಗಮ್ಯ_string(ಅಭಿವ್ಯಕ್ತಿ, FALSE, FALSE);
	    --emsg_off;
	    vim_free(ಅಭಿವ್ಯಕ್ತಿ);
	}
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    ಸಂದೇಶ((ಅಕ್ಷರ *)p);
	    vim_free(p);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಂದೇಶ("default");
#ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ(_("unknown"));
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ತುಂಬು_colors(eap->arg) == FAIL)
	semsg(_(e_cannot_ಕಂಡುಹಿಡಿ_color_scheme_str), eap->arg);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VTP
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_vtp_working())
    {
	// background color change requires clear + redraw
	update_screen(UPD_CLEAR);
	redrawcmd();
    }
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_highlight(exarg_T *eap)
{
    ಆದರೆ (*eap->arg == NUL && eap->cmd[2] == '!')
	ಸಂದೇಶ(_("Greetings, Vim user!"));
    do_highlight(eap->arg, eap->forceit, FALSE);
}


/*
 * Call this function if we thought we were going to exit, but we won't
 * (because of an error).  May need to restore the terminal mode.
 */
    ಶೂನ್ಯ
not_exiting(ಪೂರ್ಣಾಂಕ save_exiting)
{
    exiting = save_exiting;
    settmode(TMODE_RAW);
}

    ಪೂರ್ಣಾಂಕ
before_quit_autocmds(win_T *wp, ಪೂರ್ಣಾಂಕ quit_all, ಪೂರ್ಣಾಂಕ forceit)
{
    apply_autocmds(EVENT_QUITPRE, ಖಾಲಿ, ಖಾಲಿ, FALSE, wp->w_ಬಫರ್);

    // Bail out when autocommands closed the window.
    // Refuse to quit when the buffer in the last window is being closed (can
    // only happen in autocommands).
    ಆದರೆ (!win_valid(wp)
	    || curbuf_locked()
	    || (wp->w_ಬಫರ್->b_nwindows == 1 && wp->w_ಬಫರ್->b_locked > 0))
	ಹಿಂತಿರುಗಿ TRUE;

    ಆದರೆ (quit_all || (check_ಇನ್ನಷ್ಟು(FALSE, forceit) == OK && only_one_ಕಿಟಕಿ()))
    {
	apply_autocmds(EVENT_EXITPRE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	// Refuse to quit when locked or when the window was closed or the
	// buffer in the last window is being closed (can only happen in
	// autocommands).
	ಆದರೆ (!win_valid(wp) || curbuf_locked()
			  || (curbuf->b_nwindows == 1 && curbuf->b_locked > 0))
	    ಹಿಂತಿರುಗಿ TRUE;
    }

    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * ":quit": quit current window, quit Vim if the last window is closed.
 * ":{nr}quit": quit window {nr}
 * Also used when closing a terminal window that's the last one.
 */
    ಶೂನ್ಯ
ex_quit(exarg_T *eap)
{
    win_T	*wp;

    ಆದರೆ (cmdwin_type != 0)
    {
	cmdwin_ಫಲಿತಾಂಶ = Ctrl_C;
	ಹಿಂತಿರುಗಿ;
    }
    // Don't quit while editing the command line.
    ಆದರೆ (ಪಠ್ಯ_locked())
    {
	ಪಠ್ಯ_locked_ಸಂದೇಶ();
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0)
    {
	ಪೂರ್ಣಾಂಕ	wnr = eap->line2;

	ಗಾಗಿ (wp = firstwin; wp->w_ಮುಂದಿನ != ಖಾಲಿ; wp = wp->w_ಮುಂದಿನ)
	    ಆದರೆ (--wnr <= 0)
		ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	wp = curwin;

    // Refuse to quit when locked.
    ಆದರೆ (curbuf_locked())
	ಹಿಂತಿರುಗಿ;

    // Trigger QuitPre and maybe ExitPre
    ಆದರೆ (before_quit_autocmds(wp, FALSE, eap->forceit))
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    netbeansForcedQuit = eap->forceit;
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ save_exiting = exiting;
    /*
     * If there is only one relevant window we will exit.
     */
    ಆದರೆ (check_ಇನ್ನಷ್ಟು(FALSE, eap->forceit) == OK && only_one_ಕಿಟಕಿ())
	exiting = TRUE;
    ಆದರೆ ((!buf_hide(wp->w_ಬಫರ್)
		&& check_ಬದಲಾಯಿತು(wp->w_ಬಫರ್, (p_awa ? CCGD_AW : 0)
				       | (eap->forceit ? CCGD_FORCEIT : 0)
				       | CCGD_EXCMD))
	    || check_ಇನ್ನಷ್ಟು(TRUE, eap->forceit) == FAIL
	    || (only_one_ಕಿಟಕಿ() && check_ಬದಲಾಯಿತು_any(eap->forceit, TRUE)))
    {
	not_exiting(save_exiting);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// quit last window
	// Note: only_one_window() returns true, even so a help window is
	// still open. In that case only quit, if no address has been
	// specified. Example:
	// :h|wincmd w|1q     - don't quit
	// :h|wincmd w|q      - quit
	ಆದರೆ (only_one_ಕಿಟಕಿ() && (ONE_ಕಿಟಕಿ || eap->addr_ಎಣಿಕೆ == 0))
	    getout(0);
	not_exiting(save_exiting);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	// close window; may free buffer
	win_close(wp, !buf_hide(wp->w_ಬಫರ್) || eap->forceit);
    }
}

/*
 * ":cquit".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_cquit(exarg_T *eap ಬಳಸದ)
{
    // this does not always pass on the exit code to the Manx compiler. why?
    getout(eap->addr_ಎಣಿಕೆ > 0 ? (ಪೂರ್ಣಾಂಕ)eap->line2 : EXIT_FAILURE);
}

/*
 * Do preparations for "qall" and "wqall".
 * Returns FAIL when quitting should be aborted.
 */
    ಪೂರ್ಣಾಂಕ
before_quit_all(exarg_T *eap)
{
    ಆದರೆ (cmdwin_type != 0)
    {
	ಆದರೆ (eap->forceit)
	    cmdwin_ಫಲಿತಾಂಶ = K_XF1;	// ex_window() takes care of this
	ಇಲ್ಲದಿದ್ದರೆ
	    cmdwin_ಫಲಿತಾಂಶ = K_XF2;
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Don't quit while editing the command line.
    ಆದರೆ (ಪಠ್ಯ_locked())
    {
	ಪಠ್ಯ_locked_ಸಂದೇಶ();
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (before_quit_autocmds(curwin, TRUE, eap->forceit))
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * ":qall": try to quit all windows
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_quit_all(exarg_T *eap)
{
    ಆದರೆ (before_quit_all(eap) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಪೂರ್ಣಾಂಕ save_exiting = exiting;
    exiting = TRUE;
    ಆದರೆ (eap->forceit || !check_ಬದಲಾಯಿತು_any(FALSE, FALSE))
	getout(0);
    not_exiting(save_exiting);
}

/*
 * ":close": close current window, unless it is the last one
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_close(exarg_T *eap)
{
    win_T	*win;
    ಪೂರ್ಣಾಂಕ		winnr = 0;
    ಆದರೆ (cmdwin_type != 0)
	cmdwin_ಫಲಿತಾಂಶ = Ctrl_C;
    ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (!ಪಠ್ಯ_locked() && !curbuf_locked())
	{
	    ಆದರೆ (eap->addr_ಎಣಿಕೆ == 0)
		ex_win_close(eap->forceit, curwin, ಖಾಲಿ);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		FOR_ALL_WINDOWS(win)
		{
		    winnr++;
		    ಆದರೆ (winnr == eap->line2)
			ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (win == ಖಾಲಿ)
		    win = lastwin;
		ex_win_close(eap->forceit, win, ಖಾಲಿ);
	    }
	}
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
/*
 * ":pclose": Close any preview window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_pclose(exarg_T *eap)
{
    win_T	*win;

    // First close any normal window.
    FOR_ALL_WINDOWS(win)
	ಆದರೆ (win->w_p_pvw)
	{
	    ex_win_close(eap->forceit, win, ಖಾಲಿ);
	    ಹಿಂತಿರುಗಿ;
	}
# ifdef FEAT_PROP_POPUP
    // Also when 'previewpopup' is empty, it might have been cleared.
    popup_close_preview();
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Close window "win" and take care of handling closing the last window for a
 * modified buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_win_close(
    ಪೂರ್ಣಾಂಕ		forceit,
    win_T	*win,
    tabpage_T	*tp)		// NULL or the tab page "win" is in
{
    ಪೂರ್ಣಾಂಕ		need_hide;
    buf_T	*buf = win->w_ಬಫರ್;

    // Never close the autocommand window.
    ಆದರೆ (is_aucmd_win(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_ಕಿಟಕಿ));
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (ಕಿಟಕಿ_layout_locked(CMD_close))
	ಹಿಂತಿರುಗಿ;

    need_hide = (buf_Is_ಬದಲಾಯಿತು(buf) && buf->b_nwindows <= 1);
    ಆದರೆ (need_hide && !buf_hide(buf) && !forceit)
    {
#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ಆದರೆ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    ಆದರೆ (ಪದ_ಕೆಲಸ_running(buf->b_ಪದ))
	    {
		ಆದರೆ (ಪದ_confirm_stop(buf) == FAIL)
		    ಹಿಂತಿರುಗಿ;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(buf);
		need_hide = FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
	    {
		bufref_T bufref;

		ಗಣ_bufref(&bufref, buf);
		dialog_ಬದಲಾಯಿತು(buf, FALSE);
		ಆದರೆ (bufref_valid(&bufref) && buf_Is_ಬದಲಾಯಿತು(buf))
		    ಹಿಂತಿರುಗಿ;
		need_hide = FALSE;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	    no_write_ಸಂದೇಶ();
	    ಹಿಂತಿರುಗಿ;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    // free buffer when not hiding it or when it's a scratch buffer
    ಆದರೆ (tp == ಖಾಲಿ)
	win_close(win, !need_hide && !buf_hide(buf));
    ಇಲ್ಲದಿದ್ದರೆ
	win_close_othertab(win, !need_hide && !buf_hide(buf), tp);
}

/*
 * Handle the argument for a tabpage related ex command.
 * Returns a tabpage number.
 * When an error is encountered then eap->errmsg is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_tabpage_arg(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ tab_ಸಂಖ್ಯೆ;
    ಪೂರ್ಣಾಂಕ unaccept_arg0 = (eap->cmdidx == CMD_tabmove) ? 0 : 1;

    ಆದರೆ (eap->arg && *eap->arg != NUL)
    {
	char_u *p = eap->arg;
	char_u *p_save;
	ಪೂರ್ಣಾಂಕ    relative = 0; // argument +N/-N means: go to N places to the
			     // right/left relative to the current position.

	ಆದರೆ (*p == '-')
	{
	    relative = -1;
	    p++;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '+')
	{
	    relative = 1;
	    p++;
	}

	p_save = p;
	tab_ಸಂಖ್ಯೆ = getdigits(&p);

	ಆದರೆ (relative == 0)
	{
	    ಆದರೆ (STRCMP(p, "$") == 0)
		tab_ಸಂಖ್ಯೆ = ಕೊನೆಯ_TAB_NR;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(p, "#") == 0)
		ಆದರೆ (valid_tabpage(lastused_tabpage))
		    tab_ಸಂಖ್ಯೆ = tabpage_ಸೂಚ್ಯಂಕ(lastused_tabpage);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    eap->errmsg = ex_errmsg(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str,
								     eap->arg);
		    tab_ಸಂಖ್ಯೆ = 0;
		    ಹೋಗು theend;
		}
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p == p_save || *p_save == '-' || *p != NUL
		    || tab_ಸಂಖ್ಯೆ > ಕೊನೆಯ_TAB_NR)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_ವಾದ_str, eap->arg);
		ಹೋಗು theend;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (*p_save == NUL)
		tab_ಸಂಖ್ಯೆ = 1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p == p_save || *p_save == '-' || *p != NUL
		    || tab_ಸಂಖ್ಯೆ == 0)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_ವಾದ_str, eap->arg);
		ಹೋಗು theend;
	    }
	    tab_ಸಂಖ್ಯೆ = tab_ಸಂಖ್ಯೆ * relative + tabpage_ಸೂಚ್ಯಂಕ(curtab);
	    ಆದರೆ (!unaccept_arg0 && relative == -1)
		--tab_ಸಂಖ್ಯೆ;
	}
	ಆದರೆ (tab_ಸಂಖ್ಯೆ < unaccept_arg0 || tab_ಸಂಖ್ಯೆ > ಕೊನೆಯ_TAB_NR)
	    eap->errmsg = ex_errmsg(e_invalid_ವಾದ_str, eap->arg);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0)
    {
	ಆದರೆ (unaccept_arg0 && eap->line2 == 0)
	{
	    eap->errmsg = _(e_invalid_range);
	    tab_ಸಂಖ್ಯೆ = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    tab_ಸಂಖ್ಯೆ = eap->line2;
	    ಆದರೆ (!unaccept_arg0)
	    {
		char_u *cmdp = eap->cmd;

		ಆಗ (--cmdp > *eap->cmdlinep
			&& (VIM_ISWHITE(*cmdp) || VIM_ISDIGIT(*cmdp)))
		    ;
		ಆದರೆ (*cmdp == '-')
		{
		    --tab_ಸಂಖ್ಯೆ;
		    ಆದರೆ (tab_ಸಂಖ್ಯೆ < unaccept_arg0)
			eap->errmsg = _(e_invalid_range);
		}
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆಯ್ಕೆ (eap->cmdidx)
	{
	ಸ್ಥಿತಿ CMD_tabnext:
	    tab_ಸಂಖ್ಯೆ = tabpage_ಸೂಚ್ಯಂಕ(curtab) + 1;
	    ಆದರೆ (tab_ಸಂಖ್ಯೆ > ಕೊನೆಯ_TAB_NR)
		tab_ಸಂಖ್ಯೆ = 1;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CMD_tabmove:
	    tab_ಸಂಖ್ಯೆ = ಕೊನೆಯ_TAB_NR;
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    tab_ಸಂಖ್ಯೆ = tabpage_ಸೂಚ್ಯಂಕ(curtab);
	}
    }

theend:
    ಹಿಂತಿರುಗಿ tab_ಸಂಖ್ಯೆ;
}

/*
 * ":tabclose": close current tab page, unless it is the last one.
 * ":tabclose N": close tab page N.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_tabclose(exarg_T *eap)
{
    tabpage_T	*tp;
    ಪೂರ್ಣಾಂಕ		tab_ಸಂಖ್ಯೆ;

    ಆದರೆ (cmdwin_type != 0)
    {
	cmdwin_ಫಲಿತಾಂಶ = K_IGNORE;
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ಮೊದಲ_tabpage->tp_ಮುಂದಿನ == ಖಾಲಿ)
    {
	emsg(_(e_cannot_close_ಕೊನೆಯ_tab_ಪುಟ));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ಕಿಟಕಿ_layout_locked(CMD_tabclose))
	ಹಿಂತಿರುಗಿ;

    tab_ಸಂಖ್ಯೆ = get_tabpage_arg(eap);
    ಆದರೆ (eap->errmsg != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    tp = ಕಂಡುಹಿಡಿ_tabpage(tab_ಸಂಖ್ಯೆ);
    ಆದರೆ (tp == ಖಾಲಿ)
    {
	beep_flush();
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (tp != curtab)
    {
	tabpage_close_other(tp, eap->forceit);
	ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ಪಠ್ಯ_locked() && !curbuf_locked())
	tabpage_close(eap->forceit);
}

/*
 * ":tabonly": close all tab pages except the current one
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_tabonly(exarg_T *eap)
{
    tabpage_T	*tp;
    ಪೂರ್ಣಾಂಕ		ಮುಗಿದ;
    ಪೂರ್ಣಾಂಕ		tab_ಸಂಖ್ಯೆ;

    ಆದರೆ (cmdwin_type != 0)
    {
	cmdwin_ಫಲಿತಾಂಶ = K_IGNORE;
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ಮೊದಲ_tabpage->tp_ಮುಂದಿನ == ಖಾಲಿ)
    {
	ಸಂದೇಶ(_("Already only one tab page"));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ಕಿಟಕಿ_layout_locked(CMD_tabonly))
	ಹಿಂತಿರುಗಿ;

    tab_ಸಂಖ್ಯೆ = get_tabpage_arg(eap);
    ಆದರೆ (eap->errmsg != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    goto_tabpage(tab_ಸಂಖ್ಯೆ);
    // Repeat this up to a 1000 times, because autocommands may
    // mess up the lists.
    ಗಾಗಿ (ಮುಗಿದ = 0; ಮುಗಿದ < 1000; ++ಮುಗಿದ)
    {
	FOR_ALL_TABPAGES(tp)
	    ಆದರೆ (tp->tp_topframe != topframe)
	    {
		tabpage_close_other(tp, eap->forceit);
		// if we failed to close it quit
		ಆದರೆ (valid_tabpage(tp))
		    ಮುಗಿದ = 1000;
		// start over, "tp" is now invalid
		ನಿಲ್ಲಿಸು;
	    }
	ಆದರೆ (ಮೊದಲ_tabpage->tp_ಮುಂದಿನ == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
    }
}

/*
 * Close the current tab page.
 */
    ಶೂನ್ಯ
tabpage_close(ಪೂರ್ಣಾಂಕ forceit)
{
    ಆದರೆ (ಕಿಟಕಿ_layout_locked(CMD_tabclose))
	ಹಿಂತಿರುಗಿ;

    trigger_tabclosedpre(curtab);
    curtab->tp_did_tabclosedpre = TRUE;
    tabpage_T *save_curtab = curtab;

    // First close all the windows but the current one.  If that worked then
    // close the last window in this tab, that will close it.
    ಆದರೆ (!ONE_ಕಿಟಕಿ)
	close_others(TRUE, forceit);
    ಆದರೆ (ONE_ಕಿಟಕಿ)
	ex_win_close(forceit, curwin, ಖಾಲಿ);
    ಆದರೆ (curtab == save_curtab)
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	curtab->tp_did_tabclosedpre = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Close tab page "tp", which is not the current tab page.
 * Note that autocommands may make "tp" invalid.
 * Also takes care of the tab pages line disappearing when closing the
 * last-but-one tab page.
 */
    ಶೂನ್ಯ
tabpage_close_other(tabpage_T *tp, ಪೂರ್ಣಾಂಕ forceit)
{
    ಪೂರ್ಣಾಂಕ		ಮುಗಿದ = 0;
    win_T	*wp;

    ಆದರೆ (ಕಿಟಕಿ_layout_locked(CMD_ಗಾತ್ರ))
	ಹಿಂತಿರುಗಿ;

    trigger_tabclosedpre(tp);
    tp->tp_did_tabclosedpre = TRUE;

    // Limit to 1000 windows, autocommands may add a window while we close
    // one.  OK, so I'm paranoid...
    ಆಗ (++ಮುಗಿದ < 1000)
    {
	wp = tp->tp_firstwin;
	ex_win_close(forceit, wp, tp);

	// Autocommands may delete the tab page under our fingers.
	ಆದರೆ (!valid_tabpage(tp))
	    ನಿಲ್ಲಿಸು;
	// We may fail to close a window with a modified buffer.
	ಆದರೆ (tp->tp_firstwin == wp)
	{
	    ಮುಗಿದ = 1000;
	    ನಿಲ್ಲಿಸು;
	}
    }
    ಆದರೆ (ಮುಗಿದ >= 1000)
    {
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	tp->tp_did_tabclosedpre = FALSE;
	ಹಿಂತಿರುಗಿ;
    }

    apply_autocmds(EVENT_TABCLOSED, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
}

/*
 * ":only".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_only(exarg_T *eap)
{
    ಆದರೆ (ಕಿಟಕಿ_layout_locked(CMD_only))
	ಹಿಂತಿರುಗಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0)
    {
	win_T   *wp;
	ಪೂರ್ಣಾಂಕ	wnr = eap->line2;
	ಗಾಗಿ (wp = firstwin; --wnr > 0; )
	{
	    ಆದರೆ (wp->w_ಮುಂದಿನ == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    ಇಲ್ಲದಿದ್ದರೆ
		wp = wp->w_ಮುಂದಿನ;
	}
	win_goto(wp);
    }
    close_others(TRUE, eap->forceit);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_hide(exarg_T *eap ಬಳಸದ)
{
    // ":hide" or ":hide | cmd": hide current window
    ಆದರೆ (eap->skip)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಕಿಟಕಿ_layout_locked(CMD_hide))
	ಹಿಂತಿರುಗಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (eap->addr_ಎಣಿಕೆ == 0)
	win_close(curwin, FALSE);	// don't free buffer
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	winnr = 0;
	win_T	*win;

	FOR_ALL_WINDOWS(win)
	{
	    winnr++;
	    ಆದರೆ (winnr == eap->line2)
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (win == ಖಾಲಿ)
	    win = lastwin;
	win_close(win, FALSE);
    }
}

/*
 * ":stop" and ":suspend": Suspend Vim.
 */
    ಶೂನ್ಯ
ex_stop(exarg_T *eap)
{
    /*
     * Disallow suspending for "rvim".
     */
    ಆದರೆ (check_restricted())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (!eap->forceit)
	autowrite_all();
    apply_autocmds(EVENT_VIMSUSPEND, ಖಾಲಿ, ಖಾಲಿ, FALSE, ಖಾಲಿ);
    windgoto((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1, cmdline_ಲಂಬಸಾಲು_off);
    ಹೊರಹೋಗು_char('\n');
    ಹೊರಹೋಗು_flush();
    stoptermcap();
    ಹೊರಹೋಗು_flush();		// needed for SUN to restore xterm buffer
    mch_restore_title(SAVE_RESTORE_BOTH);	// restore window titles
    ui_suspend();		// call machine specific function
    maketitle();
    resettitle();		// force updating the title
    starttermcap();
    scroll_ಆರಂಭ();		// scroll screen before redrawing
    redraw_later_clear();
    shell_resized();	// may have resized window
    apply_autocmds(EVENT_VIMRESUME, ಖಾಲಿ, ಖಾಲಿ, FALSE, ಖಾಲಿ);
}

/*
 * ":exit", ":xit" and ":wq": Write file and quit the current window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_exit(exarg_T *eap)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (not_ಒಳ_vim9(eap, false) == FAIL)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (cmdwin_type != 0)
    {
	cmdwin_ಫಲಿತಾಂಶ = Ctrl_C;
	ಹಿಂತಿರುಗಿ;
    }
    // Don't quit while editing the command line.
    ಆದರೆ (ಪಠ್ಯ_locked())
    {
	ಪಠ್ಯ_locked_ಸಂದೇಶ();
	ಹಿಂತಿರುಗಿ;
    }

    ಪೂರ್ಣಾಂಕ save_exiting = exiting;
    /*
     * we plan to exit if there is only one relevant window
     */
    ಆದರೆ (check_ಇನ್ನಷ್ಟು(FALSE, eap->forceit) == OK && only_one_ಕಿಟಕಿ())
	exiting = TRUE;

    // Write the buffer for ":wq" or when it was changed.
    // Trigger QuitPre and ExitPre.
    // Check if we can exit now, after autocommands have changed things.
    ಆದರೆ (((eap->cmdidx == CMD_wq || curbuf_Is_ಬದಲಾಯಿತು()) && do_write(eap) == FAIL)
	    || before_quit_autocmds(curwin, FALSE, eap->forceit)
	    || check_ಇನ್ನಷ್ಟು(TRUE, eap->forceit) == FAIL
	    || (only_one_ಕಿಟಕಿ() && check_ಬದಲಾಯಿತು_any(eap->forceit, FALSE)))
    {
	not_exiting(save_exiting);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (only_one_ಕಿಟಕಿ())	    // quit last window, exit Vim
	    getout(0);
	not_exiting(save_exiting);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	// Quit current window, may free the buffer.
	win_close(curwin, !buf_hide(curwin->w_ಬಫರ್));
    }
}

/*
 * ":print", ":list", ":number".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_print(exarg_T *eap)
{
    ಆದರೆ (curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)
	emsg(_(e_ಖಾಲಿಗಳು_ಬಫರ್));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ ( ;!got_int; ui_breakcheck())
	{
	    print_ಸಾಲು(eap->line1,
		    (eap->cmdidx == CMD_ಸಂಖ್ಯೆ || eap->cmdidx == CMD_pound
						 || (eap->flags & EXFLAG_NR)),
		    eap->cmdidx == CMD_list || (eap->flags & EXFLAG_LIST));
	    ಆದರೆ (++eap->line1 > eap->line2)
		ನಿಲ್ಲಿಸು;
	    ಹೊರಹೋಗು_flush();	    // show one line at a time
	}
	setpcmark();
	// put cursor at last line
	curwin->w_cursor.lnum = eap->line2;
	beginline(BL_SOL | BL_FIX);
    }

    ex_no_reprint = TRUE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BYTEOFF
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_goto(exarg_T *eap)
{
    goto_byte(eap->line2);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * ":shell".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_shell(exarg_T *eap ಬಳಸದ)
{
    do_shell(ಖಾಲಿ, 0);
}

#ಆದರೆ defined(HAVE_DROP_FILE)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ drop_busy = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ drop_filec;
ಸ್ಥಿರ char_u **drop_filev = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ drop_ವಿಭಜಿಸು;
ಸ್ಥಿರ ಶೂನ್ಯ (*drop_callback)(ಶೂನ್ಯ *);
ಸ್ಥಿರ ಶೂನ್ಯ *drop_cookie;

    ಸ್ಥಿರ ಶೂನ್ಯ
handle_drop_ಆಂತರಿಕ(ಶೂನ್ಯ)
{
    exarg_T	ea;
    ಪೂರ್ಣಾಂಕ		save_ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll;

    // Setting the argument list may cause screen updates and being called
    // recursively.  Avoid that by setting drop_busy.
    drop_busy = TRUE;

    // Check whether the current buffer is changed. If so, we will need
    // to split the current window or data could be lost.
    // We don't need to check if the 'hidden' option is set, as in this
    // case the buffer won't be lost.
    ಆದರೆ (!buf_hide(curbuf) && !drop_ವಿಭಜಿಸು)
    {
	++emsg_off;
	drop_ವಿಭಜಿಸು = check_ಬದಲಾಯಿತು(curbuf, CCGD_AW);
	--emsg_off;
    }
    ಆದರೆ (drop_ವಿಭಜಿಸು)
    {
	ಆದರೆ (win_ವಿಭಜಿಸು(0, 0) == FAIL)
	    ಹಿಂತಿರುಗಿ;
	RESET_BINDING(curwin);

	// When splitting the window, create a new alist.  Otherwise the
	// existing one is overwritten.
	alist_unlink(curwin->w_alist);
	alist_new();
    }

    /*
     * Set up the new argument list.
     */
    alist_ಗಣ(ALIST(curwin), drop_filec, drop_filev, FALSE, ಖಾಲಿ, 0);

    /*
     * Move to the first file.
     */
    // Fake up a minimal "next" command for do_argfile()
    CLEAR_ಕ್ಷೇತ್ರ(ea);
    ea.cmd = (char_u *)"next";
    do_argfile(&ea, 0);

    // do_ecmd() may set need_start_insertmode, but since we never left Insert
    // mode that is not needed here.
    need_ಆರಂಭ_insertmode = FALSE;

    // Restore msg_scroll, otherwise a following command may cause scrolling
    // unexpectedly.  The screen will be redrawn by the caller, thus
    // msg_scroll being set by displaying a message is irrelevant.
    ಸಂದೇಶ_scroll = save_ಸಂದೇಶ_scroll;

    ಆದರೆ (drop_callback != ಖಾಲಿ)
	drop_callback(drop_cookie);

    drop_filev = ಖಾಲಿ;
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
    ಶೂನ್ಯ
handle_drop(
    ಪೂರ್ಣಾಂಕ		filec,		// the number of files dropped
    char_u	**filev,	// the list of files dropped
    ಪೂರ್ಣಾಂಕ		ವಿಭಜಿಸು,		// force splitting the window
    ಶೂನ್ಯ	(*callback)(ಶೂನ್ಯ *), // to be called after setting the argument
				     // list
    ಶೂನ್ಯ	*cookie)	// argument for "callback" (allocated)
{
    // Cannot handle recursive drops, finish the pending one.
    ಆದರೆ (drop_busy)
    {
	FreeWild(filec, filev);
	vim_free(cookie);
	ಹಿಂತಿರುಗಿ;
    }

    // When calling handle_drop() more than once in a row we only use the last
    // one.
    ಆದರೆ (drop_filev != ಖಾಲಿ)
    {
	FreeWild(drop_filec, drop_filev);
	vim_free(drop_cookie);
    }

    drop_filec = filec;
    drop_filev = filev;
    drop_ವಿಭಜಿಸು = ವಿಭಜಿಸು;
    drop_callback = callback;
    drop_cookie = cookie;

    // Postpone this when:
    // - editing the command line
    // - not possible to change the current buffer
    // - updating the screen
    // As it may change buffers and window structures that are in use and cause
    // freed memory to be used.
    ಆದರೆ (ಪಠ್ಯ_locked() || curbuf_locked() || updating_screen)
	ಹಿಂತಿರುಗಿ;

    handle_drop_ಆಂತರಿಕ();
}

/*
 * To be called when text is unlocked, curbuf is unlocked or updating_screen is
 * reset: Handle a postponed drop.
 */
    ಶೂನ್ಯ
handle_any_postponed_drop(ಶೂನ್ಯ)
{
    ಆದರೆ (!drop_busy && drop_filev != ಖಾಲಿ
	     && !ಪಠ್ಯ_locked() && !curbuf_locked() && !updating_screen)
	handle_drop_ಆಂತರಿಕ();
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * ":preserve".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_preserve(exarg_T *eap ಬಳಸದ)
{
    curbuf->b_flags |= BF_PRESERVED;
    ml_preserve(curbuf, TRUE);
}

/*
 * ":recover".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_recover(exarg_T *eap)
{
    // Set recoverymode right away to avoid the ATTENTION prompt.
    recoverymode = TRUE;
    ಆದರೆ (!check_ಬದಲಾಯಿತು(curbuf, (p_awa ? CCGD_AW : 0)
			     | CCGD_MULTWIN
			     | (eap->forceit ? CCGD_FORCEIT : 0)
			     | CCGD_EXCMD)

	    && (*eap->arg == NUL
			     || setfname(curbuf, eap->arg, ಖಾಲಿ, TRUE) == OK))
	ml_recover(TRUE);
    recoverymode = FALSE;
}

/*
 * Command modifier used in a wrong way.  Also for other commands that can't
 * appear at the toplevel.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_wrongmodifier(exarg_T *eap)
{
    eap->errmsg = ex_errmsg(e_invalid_command_str, eap->cmd);
}

#ಆದರೆ defined(FEAT_EVAL)

// callback function for 'findfunc'
ಸ್ಥಿರ callback_T ffu_cb;

    ಸ್ಥಿರ callback_T *
get_findfunc_callback(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ *curbuf->b_p_ffu != NUL ? &curbuf->b_ffu_cb : &ffu_cb;
}

/*
 * Call 'findfunc' to obtain a list of file names.
 */
    ಸ್ಥಿರ list_T *
call_findfunc(char_u *pat, ಪೂರ್ಣಾಂಕ cmdcomplete)
{
    typval_T	ವಾದಗಳು[3];
    callback_T	*cb;
    typval_T	rettv;
    ಪೂರ್ಣಾಂಕ		retval;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    ವಾದಗಳು[0].v_type = VAR_STRING;
    ವಾದಗಳು[0].vval.v_string = pat;
    ವಾದಗಳು[1].v_type = VAR_BOOL;
    ವಾದಗಳು[1].vval.v_ಸಂಖ್ಯೆ = cmdcomplete;
    ವಾದಗಳು[2].v_type = VAR_UNKNOWN;

    // Lock the text to prevent weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;

    ctx = get_option_sctx("findfunc");
    ಆದರೆ (ctx != ಖಾಲಿ)
	current_sctx = *ctx;

    cb = get_findfunc_callback();
    retval = call_callback(cb, -1, &rettv, 2, ವಾದಗಳು);

    current_sctx = saved_sctx;

    --textlock;

    list_T *retlist = ಖಾಲಿ;

    ಆದರೆ (retval == OK)
    {
	ಆದರೆ (rettv.v_type == VAR_LIST)
	    retlist = list_copy(rettv.vval.v_list, FALSE, FALSE, get_copyID());
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_invalid_return_type_from_findfunc));

	clear_tv(&rettv);
    }

    ಹಿಂತಿರುಗಿ retlist;
}

/*
 * Find file names matching "pat" using 'findfunc' and return it in "files".
 * Used for expanding the :find, :sfind and :tabfind command argument.
 * Returns OK on success and FAIL otherwise.
 */
    ಪೂರ್ಣಾಂಕ
expand_findfunc(expand_T *xp, char_u *pat, char_u ***files, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches)
{
    list_T	*l;
    ಪೂರ್ಣಾಂಕ		len;

    *ಅಂಕೆ_Matches = 0;
    *files = ಖಾಲಿ;

    l = call_findfunc(pat, VVAL_TRUE);

    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    len = list_len(l);
    ಆದರೆ (len == 0)	    // empty List
    {
	list_free(l);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    expand_process_user_list(l, files, ಅಂಕೆ_Matches, xp);
    list_free(l);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Use 'findfunc' to find file 'findarg'.  The 'count' argument is used to find
 * the n'th matching file.
 */
    ಸ್ಥಿರ char_u *
findfunc_ಕಂಡುಹಿಡಿ_file(char_u *findarg, ಪೂರ್ಣಾಂಕ findarg_len, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    list_T	*fname_list;
    char_u	*ret_fname = ಖಾಲಿ;
    char_u	cc;
    ಪೂರ್ಣಾಂಕ		fname_ಎಣಿಕೆ;

    cc = findarg[findarg_len];
    findarg[findarg_len] = NUL;

    fname_list = call_findfunc(findarg, VVAL_FALSE);
    fname_ಎಣಿಕೆ = list_len(fname_list);

    ಆದರೆ (fname_ಎಣಿಕೆ == 0)
	semsg(_(e_cant_ಕಂಡುಹಿಡಿ_file_str_ಒಳ_ಮಾರ್ಗ), findarg);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ಎಣಿಕೆ_2 > fname_ಎಣಿಕೆ)
	    semsg(_(e_no_ಇನ್ನಷ್ಟು_file_str_ಸಿಕ್ಕಿತು_ಒಳ_ಮಾರ್ಗ), findarg);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    listitem_T *li = list_ಕಂಡುಹಿಡಿ(fname_list, ಎಣಿಕೆ_2 - 1);

	    ಆದರೆ (li != ಖಾಲಿ)
	    {
		typval_T *tv = &li->li_tv;

		ಆದರೆ (tv->v_type == VAR_STRING && tv->vval.v_string != ಖಾಲಿ)
		    ret_fname = vim_strsave(tv->vval.v_string);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_DICT && tv->vval.v_dict != ಖಾಲಿ)
		    ret_fname = dict_get_string(tv->vval.v_dict, "word", TRUE);
	    }
	}
    }

    ಆದರೆ (fname_list != ಖಾಲಿ)
	list_free(fname_list);

    findarg[findarg_len] = cc;

    ಹಿಂತಿರುಗಿ ret_fname;
}

/*
 * Process the 'findfunc' option value.
 * Returns NULL on success and an error message on failure.
 */
    ಅಕ್ಷರ *
did_ಗಣ_findfunc(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ	retval;

    ಆದರೆ (ವಾದಗಳು->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_ಗಣ_callback_func(curbuf->b_p_ffu, &curbuf->b_ffu_cb);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// global option set
	retval = option_ಗಣ_callback_func(p_ffu, &ffu_cb);
	// when using :set, free the local callback
	ಆದರೆ (!(ವಾದಗಳು->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_ffu_cb);
    }

    ಆದರೆ (retval == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;
    char_u	*name = get_scriptlocal_funcname(*varp);
    ಆದರೆ (name != ಖಾಲಿ)
    {
	free_string_option(*varp);
	*varp = name;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

# ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_findfunc_option(ಶೂನ್ಯ)
{
    free_callback(&ffu_cb);
}
# endif

/*
 * Mark the global 'findfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_findfunc(ಪೂರ್ಣಾಂಕ copyID ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ abort = FALSE;
    abort = ಗಣ_ref_ಒಳ_callback(&ffu_cb, copyID);
    ಹಿಂತಿರುಗಿ abort;
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಶೂನ್ಯ
ex_splitview(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    char_u	*fname = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
    char_u	dot_ಮಾರ್ಗ[] = ".";
    ಪೂರ್ಣಾಂಕ		save_cmod_flags = cmdmod.cmod_flags;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		use_tab = eap->cmdidx == CMD_tabedit
		       || eap->cmdidx == CMD_tabfind
		       || eap->cmdidx == CMD_tabnew;

    ಆದರೆ (ERROR_IF_ANY_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    // A ":split" in the quickfix window works like ":new".  Don't want two
    // quickfix windows.  But it's OK when doing ":tab split".
    ಆದರೆ (bt_quickfix(curbuf) && cmdmod.cmod_tab == 0)
    {
	ಆದರೆ (eap->cmdidx == CMD_ವಿಭಜಿಸು)
	    eap->cmdidx = CMD_new;
	ಆದರೆ (eap->cmdidx == CMD_vsplit)
	    eap->cmdidx = CMD_vnew;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (eap->cmdidx == CMD_sfind || eap->cmdidx == CMD_tabfind)
    {
	char_u	*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ = ಖಾಲಿ;
	ಅಕ್ಷರ	*ಹುಡುಕು_ctx = ಖಾಲಿ;

	ಆದರೆ (*get_findfunc() != NUL)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    fname = findfunc_ಕಂಡುಹಿಡಿ_file(eap->arg, (ಪೂರ್ಣಾಂಕ)STRLEN(eap->arg),
				       eap->addr_ಎಣಿಕೆ > 0 ? eap->line2 : 1);
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    fname = ಕಂಡುಹಿಡಿ_file_ಒಳ_ಮಾರ್ಗ(eap->arg, (ಪೂರ್ಣಾಂಕ)STRLEN(eap->arg),
					      FNAME_MESS, TRUE, curbuf->b_ffname,
					      &file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ, &ಹುಡುಕು_ctx);
	    vim_free(file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ);
	    vim_findfile_cleanup(ಹುಡುಕು_ctx);
	}
	ಆದರೆ (fname == ಖಾಲಿ)
	    ಹೋಗು theend;
	eap->arg = fname;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((cmdmod.cmod_flags & CMOD_BROWSE)
	    && eap->cmdidx != CMD_vnew
	    && eap->cmdidx != CMD_new)
    {
	ಆದರೆ (
# ifdef FEAT_GUI
	    !gui.ಒಳ_use &&
# endif
		au_has_ಗುಂಪು((char_u *)"FileExplorer"))
	{
	    // No browsing supported but we do have the file explorer:
	    // Edit the directory.
	    ಆದರೆ (*eap->arg == NUL || !mch_isdir(eap->arg))
		eap->arg = dot_ಮಾರ್ಗ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    fname = do_browse(0, (char_u *)(use_tab
			? _("Edit File in new tab page")
			: _("Edit File in new window")),
					  eap->arg, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, curbuf);
	    ಆದರೆ (fname == ಖಾಲಿ)
		ಹೋಗು theend;
	    eap->arg = fname;
	}
    }
    cmdmod.cmod_flags &= ~CMOD_BROWSE;	// Don't browse again in do_ecmd().
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Either open new tab page or split the window.
     */
    ಆದರೆ (use_tab)
    {
	ಆದರೆ (win_new_tabpage(cmdmod.cmod_tab != 0 ? cmdmod.cmod_tab
			 : eap->addr_ಎಣಿಕೆ == 0 ? 0
					       : (ಪೂರ್ಣಾಂಕ)eap->line2 + 1) != FAIL)
	{
	    do_exedit(eap, old_curwin);

	    // set the alternate buffer for the window we came from
	    ಆದರೆ (curwin != old_curwin
		    && win_valid(old_curwin)
		    && old_curwin->w_ಬಫರ್ != curbuf
		    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		old_curwin->w_alt_fnum = curbuf->b_fnum;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (win_ವಿಭಜಿಸು(eap->addr_ಎಣಿಕೆ > 0 ? (ಪೂರ್ಣಾಂಕ)eap->line2 : 0,
				     *eap->cmd == 'v' ? WSP_VERT : 0) != FAIL)
    {
	// Reset 'scrollbind' when editing another file, but keep it when
	// doing ":split" without arguments.
	ಆದರೆ (*eap->arg != NUL)
	    RESET_BINDING(curwin);
	ಇಲ್ಲದಿದ್ದರೆ
	    do_check_scrollbind(FALSE);
	do_exedit(eap, old_curwin);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
    cmdmod.cmod_flags = save_cmod_flags;
#ಆದರೆ_ಅಂತ್ಯ

theend:
    vim_free(fname);
}

/*
 * Open a new tab page.
 */
    ಶೂನ್ಯ
tabpage_new(ಶೂನ್ಯ)
{
    exarg_T	ea;

    CLEAR_ಕ್ಷೇತ್ರ(ea);
    ea.cmdidx = CMD_tabnew;
    ea.cmd = (char_u *)"tabn";
    ea.arg = (char_u *)"";
    ex_splitview(&ea);
}

/*
 * :tabnext command
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_tabnext(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ tab_ಸಂಖ್ಯೆ;

    ಆದರೆ (ERROR_IF_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;
    ಆಯ್ಕೆ (eap->cmdidx)
    {
	ಸ್ಥಿತಿ CMD_tabfirst:
	ಸ್ಥಿತಿ CMD_tabrewind:
	    goto_tabpage(1);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CMD_tablast:
	    goto_tabpage(9999);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CMD_tabprevious:
	ಸ್ಥಿತಿ CMD_tab_ಮುಂದಿನ:
	    ಆದರೆ (eap->arg && *eap->arg != NUL)
	    {
		char_u *p = eap->arg;
		char_u *p_save = p;

		tab_ಸಂಖ್ಯೆ = getdigits(&p);
		ಆದರೆ (p == p_save || *p_save == '-' || *p != NUL
			    || tab_ಸಂಖ್ಯೆ == 0)
		{
		    // No numbers as argument.
		    eap->errmsg = ex_errmsg(e_invalid_ವಾದ_str, eap->arg);
		    ಹಿಂತಿರುಗಿ;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (eap->addr_ಎಣಿಕೆ == 0)
		    tab_ಸಂಖ್ಯೆ = 1;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    tab_ಸಂಖ್ಯೆ = eap->line2;
		    ಆದರೆ (tab_ಸಂಖ್ಯೆ < 1)
		    {
			eap->errmsg = _(e_invalid_range);
			ಹಿಂತಿರುಗಿ;
		    }
		}
	    }
	    goto_tabpage(-tab_ಸಂಖ್ಯೆ);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ: // CMD_tabnext
	    tab_ಸಂಖ್ಯೆ = get_tabpage_arg(eap);
	    ಆದರೆ (eap->errmsg == ಖಾಲಿ)
		goto_tabpage(tab_ಸಂಖ್ಯೆ);
	    ನಿಲ್ಲಿಸು;
    }
}

/*
 * :tabmove command
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_tabmove(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ tab_ಸಂಖ್ಯೆ;

    tab_ಸಂಖ್ಯೆ = get_tabpage_arg(eap);
    ಆದರೆ (eap->errmsg == ಖಾಲಿ)
	tabpage_move(tab_ಸಂಖ್ಯೆ);
}

/*
 * :tabs command: List tabs and their contents.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_tabs(exarg_T *eap ಬಳಸದ)
{
    tabpage_T	*tp;
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		tabcount = 1;

    ಸಂದೇಶ_ಆರಂಭ();
    ಸಂದೇಶ_scroll = TRUE;
    ಗಾಗಿ (tp = ಮೊದಲ_tabpage; tp != ಖಾಲಿ && !got_int; tp = tp->tp_ಮುಂದಿನ)
    {
	ಸಂದೇಶ_putchar('\n');
	vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, _("Tab page %d"), tabcount++);
	ಸಂದೇಶ_outtrans_attr(IObuff, HL_ATTR(HLF_T));
	ಹೊರಹೋಗು_flush();	    // output one line at a time
	ui_breakcheck();

	ಆದರೆ (tp  == curtab)
	    wp = firstwin;
	ಇಲ್ಲದಿದ್ದರೆ
	    wp = tp->tp_firstwin;
	ಗಾಗಿ ( ; wp != ಖಾಲಿ && !got_int; wp = wp->w_ಮುಂದಿನ)
	{
	    ಸಂದೇಶ_putchar('\n');
	    ಸಂದೇಶ_putchar(wp == curwin ? '>' : ' ');
	    ಸಂದೇಶ_putchar(' ');
	    ಸಂದೇಶ_putchar(buf_Is_ಬದಲಾಯಿತು(wp->w_ಬಫರ್) ? '+' : ' ');
	    ಸಂದೇಶ_putchar(' ');
	    ಆದರೆ (buf_spname(wp->w_ಬಫರ್) != ಖಾಲಿ)
		vim_strncpy(IObuff, buf_spname(wp->w_ಬಫರ್), IOSIZE - 1);
	    ಇಲ್ಲದಿದ್ದರೆ
		home_replace(wp->w_ಬಫರ್, wp->w_ಬಫರ್->b_fname,
							IObuff, IOSIZE, TRUE);
	    ಸಂದೇಶ_outtrans(IObuff);
	    ಹೊರಹೋಗು_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }
}

/*
 * ":mode": Set screen mode.
 * If no argument given, just get the screen size and redraw.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_ಬಹುಲಕ(exarg_T *eap)
{
    ಆದರೆ (*eap->arg == NUL)
	shell_resized();
    ಇಲ್ಲದಿದ್ದರೆ
	emsg(_(e_screen_ಬಹುಲಕ_setting_not_supported));
}

/*
 * ":resize".
 * set, increment or decrement current window height
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_resize(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		n;
    win_T	*wp = curwin;

    ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0)
    {
	n = eap->line2;
	ಗಾಗಿ (wp = firstwin; wp->w_ಮುಂದಿನ != ಖಾಲಿ && --n > 0; wp = wp->w_ಮುಂದಿನ)
	    ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    n = atol((ಅಕ್ಷರ *)eap->arg);
    ಆದರೆ (cmdmod.cmod_ವಿಭಜಿಸು & WSP_VERT)
    {
	ಆದರೆ (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_ಅಗಲ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n == 0 && eap->arg[0] == NUL)	// default is very wide
	    n = 9999;
	win_setwidth_win(n, wp);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_ಎತ್ತರ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n == 0 && eap->arg[0] == NUL)	// default is very high
	    n = 9999;
	win_setheight_win(n, wp);
    }
}

/*
 * ":find [+command] <file>" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_ಕಂಡುಹಿಡಿ(exarg_T *eap)
{
    ಆದರೆ (!check_can_ಗಣ_curbuf_forceit(eap->forceit))
	ಹಿಂತಿರುಗಿ;

    char_u	*fname = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;
    char_u	*file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ = ಖಾಲಿ;
    ಅಕ್ಷರ	*ಹುಡುಕು_ctx = ಖಾಲಿ;

    ಆದರೆ (*get_findfunc() != NUL)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	fname = findfunc_ಕಂಡುಹಿಡಿ_file(eap->arg, (ಪೂರ್ಣಾಂಕ)STRLEN(eap->arg),
					eap->addr_ಎಣಿಕೆ > 0 ? eap->line2 : 1);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	fname = ಕಂಡುಹಿಡಿ_file_ಒಳ_ಮಾರ್ಗ(eap->arg, (ಪೂರ್ಣಾಂಕ)STRLEN(eap->arg), FNAME_MESS,
			       TRUE, curbuf->b_ffname, &file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ, &ಹುಡುಕು_ctx);
	ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0)
	{
	    // Repeat finding the file "count" times.  This matters when it appears
	    // several times in the path.
	    ಎಣಿಕೆ_2 = eap->line2;
	    ಆಗ (fname != ಖಾಲಿ && --ಎಣಿಕೆ_2 > 0)
	    {
		vim_free(fname);
		fname = ಕಂಡುಹಿಡಿ_file_ಒಳ_ಮಾರ್ಗ(ಖಾಲಿ, 0, FNAME_MESS,
			      FALSE, curbuf->b_ffname, &file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ, &ಹುಡುಕು_ctx);
	    }
	}
	VIM_CLEAR(file_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ);
	vim_findfile_cleanup(ಹುಡುಕು_ctx);
    }

    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    eap->arg = fname;
    do_exedit(eap, ಖಾಲಿ);
    vim_free(fname);
}

/*
 * ":open" simulation: for now works just like ":visual".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_open(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*p;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (not_ಒಳ_vim9(eap, false) == FAIL)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_SOL | BL_FIX);
    ಆದರೆ (*eap->arg == '/')
    {
	// ":open /pattern/": put cursor in column found with pattern
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg, magic_isset() ? RE_MAGIC : 0);
	ಆದರೆ (regmatch.regprog != ಖಾಲಿ)
	{
	    // make a copy of the line, when searching for a mark it might be
	    // flushed
	    char_u *ಸಾಲು = vim_strsave(ml_get_curline());

	    regmatch.rm_ic = p_ic;
	    ಆದರೆ (vim_regexec(&regmatch, ಸಾಲು, (colnr_T)0))
		curwin->w_cursor.ಲಂಬಸಾಲು = (colnr_T)(regmatch.startp[0] - ಸಾಲು);
	    ಇಲ್ಲದಿದ್ದರೆ
		emsg(_(e_no_ಹೊಂದಿಕೆ));
	    vim_regfree(regmatch.regprog);
	    vim_free(ಸಾಲು);
	}
	// Move to the NUL, ignore any other arguments.
	eap->arg += STRLEN(eap->arg);
    }
    check_cursor();

    eap->cmdidx = CMD_visual;
    do_exedit(eap, ಖಾಲಿ);
}

/*
 * ":edit", ":badd", ":balt", ":visual".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_edit(exarg_T *eap)
{
    char_u *ffname = eap->cmdidx == CMD_enew ? ಖಾಲಿ : eap->arg;

    // Exclude commands which keep the window's current buffer
    ಆದರೆ (
	    eap->cmdidx != CMD_badd
	    && eap->cmdidx != CMD_balt
	    // All other commands must obey 'winfixbuf' / ! rules
	    && (is_other_file(0, ffname) && !check_can_ಗಣ_curbuf_forceit(eap->forceit))
    )
	ಹಿಂತಿರುಗಿ;

    do_exedit(eap, ಖಾಲಿ);
}

/*
 * ":edit <file>" command and alike.
 */
    ಶೂನ್ಯ
do_exedit(
    exarg_T	*eap,
    win_T	*old_curwin)	    // curwin before doing a split or NULL
{
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		need_hide;
    ಪೂರ್ಣಾಂಕ		exmode_was = exmode_ಸಕ್ರಿಯ;

    ಆದರೆ ((eap->cmdidx != CMD_pedit && ERROR_IF_POPUP_ಕಿಟಕಿ)
						 || ERROR_IF_ಪದ_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;
    /*
     * ":vi" command ends Ex mode.
     */
    ಆದರೆ (exmode_ಸಕ್ರಿಯ && (eap->cmdidx == CMD_visual
						|| eap->cmdidx == CMD_view))
    {
	exmode_ಸಕ್ರಿಯ = FALSE;
	ex_pressedreturn = FALSE;
	ಆದರೆ (*eap->arg == NUL)
	{
	    // Special case:  ":global/pat/visual\NLvi-commands"
	    ಆದರೆ (global_busy)
	    {
		ಆದರೆ (eap->nextcmd != ಖಾಲಿ)
		{
		    stuffReadbuff(eap->nextcmd);
		    eap->nextcmd = ಖಾಲಿ;
		}

		ಆದರೆ (exmode_was != EXMODE_VIM)
		    settmode(TMODE_RAW);
		ಪೂರ್ಣಾಂಕ save_RedrawingDisabled = RedrawingDisabled;
		RedrawingDisabled = 0;
		ಪೂರ್ಣಾಂಕ save_nwr = no_wait_return;
		no_wait_return = 0;
		need_wait_return = FALSE;
		ಪೂರ್ಣಾಂಕ save_ms = ಸಂದೇಶ_scroll;
		ಸಂದೇಶ_scroll = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		ಪೂರ್ಣಾಂಕ save_he = hold_gui_events;
		hold_gui_events = 0;
#ಆದರೆ_ಅಂತ್ಯ
		ಗಣ_must_redraw(UPD_CLEAR);
		pending_exmode_ಸಕ್ರಿಯ = TRUE;

		main_loop(FALSE, TRUE);

		pending_exmode_ಸಕ್ರಿಯ = FALSE;
		RedrawingDisabled = save_RedrawingDisabled;
		no_wait_return = save_nwr;
		ಸಂದೇಶ_scroll = save_ms;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		hold_gui_events = save_he;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಹಿಂತಿರುಗಿ;
	}
    }

    ಆದರೆ ((eap->cmdidx == CMD_new
		|| eap->cmdidx == CMD_tabnew
		|| eap->cmdidx == CMD_tabedit
		|| eap->cmdidx == CMD_vnew) && *eap->arg == NUL)
    {
	// ":new" or ":tabnew" without argument: edit a new empty buffer
	setpcmark();
	(ಶೂನ್ಯ)do_ecmd(0, ಖಾಲಿ, ಖಾಲಿ, eap, ECMD_ONE,
		      ECMD_HIDE + (eap->forceit ? ECMD_FORCEIT : 0),
		      old_curwin == ಖಾಲಿ ? curwin : ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((eap->cmdidx != CMD_ವಿಭಜಿಸು && eap->cmdidx != CMD_vsplit)
	    || *eap->arg != NUL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
	    || (cmdmod.cmod_flags & CMOD_BROWSE)
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
	// Can't edit another file when "textlock" or "curbuf_lock" is set.
	// Only ":edit" or ":script" can bring us here, others are stopped
	// earlier.
	ಆದರೆ (*eap->arg != NUL && ಪಠ್ಯ_or_buf_locked())
	    ಹಿಂತಿರುಗಿ;

	n = readonlymode;
	ಆದರೆ (eap->cmdidx == CMD_view || eap->cmdidx == CMD_sview)
	    readonlymode = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_enew)
	    readonlymode = FALSE;   // 'readonly' doesn't make sense in an
				    // empty buffer
	ಆದರೆ (eap->cmdidx != CMD_balt && eap->cmdidx != CMD_badd)
	    setpcmark();
	ಆದರೆ (do_ecmd(0, (eap->cmdidx == CMD_enew ? ಖಾಲಿ : eap->arg),
		    ಖಾಲಿ, eap,
		    // ":edit" goes to first line if Vi compatible
		    (*eap->arg == NUL && eap->do_ecmd_lnum == 0
				      && vim_strchr(p_cpo, CPO_GOTO1) != ಖಾಲಿ)
					       ? ECMD_ONE : eap->do_ecmd_lnum,
		    (buf_hide(curbuf) ? ECMD_HIDE : 0)
		    + (eap->forceit ? ECMD_FORCEIT : 0)
		      // after a split we can use an existing buffer
		    + (old_curwin != ಖಾಲಿ ? ECMD_OLDBUF : 0)
		    + (eap->cmdidx == CMD_badd ? ECMD_ADDBUF : 0)
		    + (eap->cmdidx == CMD_balt ? ECMD_ALTBUF : 0)
		    , old_curwin == ಖಾಲಿ ? curwin : ಖಾಲಿ) == FAIL)
	{
	    // Editing the file failed.  If the window was split, close it.
	    ಆದರೆ (old_curwin != ಖಾಲಿ)
	    {
		need_hide = (curbuf_Is_ಬದಲಾಯಿತು() && curbuf->b_nwindows <= 1);
		ಆದರೆ (!need_hide || buf_hide(curbuf))
		{
#ಆದರೆ defined(FEAT_EVAL)
		    cleanup_T   cs;

		    // Reset the error/interrupt/exception state here so that
		    // aborting() returns FALSE when closing a window.
		    enter_cleanup(&cs);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		    win_close(curwin, !need_hide && !buf_hide(curbuf));

#ಆದರೆ defined(FEAT_EVAL)
		    // Restore the error/interrupt/exception state if not
		    // discarded by a new aborting error, interrupt, or
		    // uncaught exception.
		    leave_cleanup(&cs);
#ಆದರೆ_ಅಂತ್ಯ
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (readonlymode && curbuf->b_nwindows == 1)
	{
	    // When editing an already visited buffer, 'readonly' won't be set
	    // but the previous value is kept.  With ":view" and ":sview" we
	    // want the  file to be readonly, except when another window is
	    // editing the same buffer.
	    curbuf->b_p_ro = TRUE;
	}
	readonlymode = n;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (eap->do_ecmd_cmd != ಖಾಲಿ)
	    do_cmd_ವಾದ(eap->do_ecmd_cmd);
	n = curwin->w_arg_idx_invalid;
	check_arg_idx(curwin);
	ಆದರೆ (n != curwin->w_arg_idx_invalid)
	    maketitle();
    }

    /*
     * if ":split file" worked, set alternate file name in old window to new
     * file
     */
    ಆದರೆ (old_curwin != ಖಾಲಿ
	    && *eap->arg != NUL
	    && curwin != old_curwin
	    && win_valid(old_curwin)
	    && old_curwin->w_ಬಫರ್ != curbuf
	    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	old_curwin->w_alt_fnum = curbuf->b_fnum;

    ex_no_reprint = TRUE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_GUI
/*
 * ":gui" and ":gvim" when there is no GUI.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_nogui(exarg_T *eap)
{
    eap->errmsg = _(e_gui_cannot_be_ಬಳಸಿದ_not_enabled_at_compile_ಸಮಯ);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_tearoff(exarg_T *eap)
{
    gui_make_tearoff(eap->arg);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_ಪದ_POPUP_MENU)) && defined(FEAT_MENU)
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_popup(exarg_T *eap)
{
# ಆದರೆ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    ಆದರೆ (gui.ಒಳ_use)
	gui_make_popup(eap->arg, eap->forceit);
#  ifdef FEAT_ಪದ_POPUP_MENU
    ಇಲ್ಲದಿದ್ದರೆ
#  endif
# endif
# ifdef FEAT_ಪದ_POPUP_MENU
	pum_make_popup(eap->arg, eap->forceit);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_swapname(exarg_T *eap ಬಳಸದ)
{
    ಆದರೆ (curbuf->b_ml.ml_mfp == ಖಾಲಿ || curbuf->b_ml.ml_mfp->mf_fname == ಖಾಲಿ)
	ಸಂದೇಶ(_("No swap file"));
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ((ಅಕ್ಷರ *)curbuf->b_ml.ml_mfp->mf_fname);
}

/*
 * ":syncbind" forces all 'scrollbind' windows to have the same relative
 * offset.
 * (1998-11-02 16:21:01  R. Edward Ralston <eralston@computer.org>)
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_syncbind(exarg_T *eap ಬಳಸದ)
{
    win_T	*wp;
    win_T	*save_curwin = curwin;
    buf_T	*save_curbuf = curbuf;
    ದೀರ್ಘ	topline;
    ದೀರ್ಘ	y;
    linenr_T	old_linenr = curwin->w_cursor.lnum;

    setpcmark();

    /*
     * determine max topline
     */
    ಆದರೆ (curwin->w_p_scb)
    {
	topline = curwin->w_topline;
	FOR_ALL_WINDOWS(wp)
	{
	    ಆದರೆ (wp->w_p_scb && wp->w_ಬಫರ್)
	    {
		y = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - get_scrolloff_ಮೌಲ್ಯ();
		ಆದರೆ (topline > y)
		    topline = y;
	    }
	}
	ಆದರೆ (topline < 1)
	    topline = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	topline = 1;
    }


    /*
     * Set all scrollbind windows to the same topline.
     */
    FOR_ALL_WINDOWS(curwin)
    {
	ಆದರೆ (curwin->w_p_scb)
	{
	    curbuf = curwin->w_ಬಫರ್;
	    y = topline - curwin->w_topline;
	    ಆದರೆ (y > 0)
		scrollup(y, TRUE);
	    ಇಲ್ಲದಿದ್ದರೆ
		scrolldown(-y, TRUE);
	    curwin->w_scbind_pos = topline;
	    redraw_later(UPD_VALID);
	    cursor_correct();
	    curwin->w_redr_ಸ್ಥಿತಿ = true;
	}
    }
    curwin = save_curwin;
    curbuf = save_curbuf;
    ಆದರೆ (curwin->w_p_scb)
    {
	did_syncbind = TRUE;
	checkpcmark();
	ಆದರೆ (old_linenr != curwin->w_cursor.lnum)
	{
	    char_u ctrl_o[2];

	    ctrl_o[0] = Ctrl_O;
	    ctrl_o[1] = 0;
	    ins_typebuf(ctrl_o, REMAP_NONE, 0, TRUE, FALSE);
	}
    }
}


    ಸ್ಥಿರ ಶೂನ್ಯ
ex_read(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ಖಾಲಿಗಳು_2 = (curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು);
    linenr_T	lnum;

    ಆದರೆ (eap->usefilter)			// :r!cmd
    {
	do_bang(1, eap, FALSE, FALSE, TRUE);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (u_save(eap->line2, (linenr_T)(eap->line2 + 1)) == FAIL)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
    ಆದರೆ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browseFile;

	browseFile = do_browse(0, (char_u *)_("Append File"), eap->arg,
		ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, curbuf);
	ಆದರೆ (browseFile != ಖಾಲಿ)
	{
	    i = readfile(browseFile, ಖಾಲಿ,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	    vim_free(browseFile);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    i = OK;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (*eap->arg == NUL)
	{
	    ಆದರೆ (check_fname() == FAIL)	// check for no file name
		ಹಿಂತಿರುಗಿ;
	    i = readfile(curbuf->b_ffname, curbuf->b_fname,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (vim_strchr(p_cpo, CPO_ALTREAD) != ಖಾಲಿ)
		(ಶೂನ್ಯ)setaltfname(eap->arg, eap->arg, (linenr_T)1);
	    i = readfile(eap->arg, ಖಾಲಿ,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);

	}
    ಆದರೆ (i != OK)
    {
#ಆದರೆ defined(FEAT_EVAL)
	ಆದರೆ (!aborting())
#ಆದರೆ_ಅಂತ್ಯ
	    semsg(_(e_cant_open_file_str), eap->arg);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ಖಾಲಿಗಳು_2 && exmode_ಸಕ್ರಿಯ)
	{
	    // Delete the empty line that remains.  Historically ex does
	    // this but vi doesn't.
	    ಆದರೆ (eap->line2 == 0)
		lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಇಲ್ಲದಿದ್ದರೆ
		lnum = 1;
	    ಆದರೆ (*ml_get(lnum) == NUL && u_savedel(lnum, 1L) == OK)
	    {
		ml_delete(lnum);
		ಆದರೆ (curwin->w_cursor.lnum > 1
			&& curwin->w_cursor.lnum >= lnum)
		    --curwin->w_cursor.lnum;
		deleted_ಸಾಲುಗಳು_mark(lnum, 1L);
	    }
	}
	redraw_curbuf_later(UPD_VALID);
    }
}

ಸ್ಥಿರ char_u	*prev_dir = ಖಾಲಿ;

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_cd_dir(ಶೂನ್ಯ)
{
    VIM_CLEAR(prev_dir);
    VIM_CLEAR(globaldir);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get the previous directory for the given chdir scope.
 */
    ಸ್ಥಿರ char_u *
get_prevdir(cdscope_T scope)
{
    ಆದರೆ (scope == CDSCOPE_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ curwin->w_prevdir;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (scope == CDSCOPE_TABPAGE)
	ಹಿಂತಿರುಗಿ curtab->tp_prevdir;
    ಹಿಂತಿರುಗಿ prev_dir;
}

/*
 * Deal with the side effects of changing the current directory.
 * When 'scope' is CDSCOPE_TABPAGE then this was after an ":tcd" command.
 * When 'scope' is CDSCOPE_WINDOW then this was after an ":lcd" command.
 */
    ಶೂನ್ಯ
post_chdir(cdscope_T scope)
{
    ಆದರೆ (scope != CDSCOPE_ಕಿಟಕಿ)
	// Clear tab local directory for both :cd and :tcd
	VIM_CLEAR(curtab->tp_localdir);
    VIM_CLEAR(curwin->w_localdir);
    ಆದರೆ (scope != CDSCOPE_GLOBAL)
    {
	char_u	*pdir = get_prevdir(scope);

	// If still in the global directory, need to remember current
	// directory as the global directory.
	ಆದರೆ (globaldir == ಖಾಲಿ && pdir != ಖಾಲಿ)
	    globaldir = vim_strsave(pdir);

	// Remember this local directory for the window.
	ಆದರೆ (mch_dirname(NameBuff, MAXPATHL) == OK)
	{
	    ಆದರೆ (scope == CDSCOPE_TABPAGE)
		curtab->tp_localdir = vim_strsave(NameBuff);
	    ಇಲ್ಲದಿದ್ದರೆ
		curwin->w_localdir = vim_strsave(NameBuff);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// We are now in the global directory, no need to remember its name.
	VIM_CLEAR(globaldir);
    }

    ಕೊನೆಯ_chdir_reason = ಖಾಲಿ;
    shorten_fnames(vim_strchr(p_cpo, CPO_NOSYMLINKS) == ಖಾಲಿ);
}

/*
 * Trigger DirChangedPre for "acmd_fname" with directory "new_dir".
 */
    ಶೂನ್ಯ
trigger_Dir_ಬದಲಾಯಿತು_Pre(char_u *acmd_fname, char_u *new_dir)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;

    v_event = get_v_event(&save_v_event);
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_string(v_event, "directory", new_dir);
    dict_ಗಣ_ವಸ್ತುಗಳು_ro(v_event);
#ಆದರೆ_ಅಂತ್ಯ
    apply_autocmds(EVENT_DIRCHANGEDPRE, acmd_fname, new_dir, FALSE, curbuf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    restore_v_event(v_event, &save_v_event);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Change directory function used by :cd/:tcd/:lcd Ex commands and the
 * chdir() function.
 * scope == CDSCOPE_WINDOW: changes the window-local directory
 * scope == CDSCOPE_TABPAGE: changes the tab-local directory
 * Otherwise: changes the global directory
 * Returns TRUE if the directory is successfully changed.
 */
    ಪೂರ್ಣಾಂಕ
changedir_func(
	char_u		*new_dir,
	ಪೂರ್ಣಾಂಕ		forceit,
	cdscope_T	scope)
{
    char_u	*pdir = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		dir_differs;
    char_u	*acmd_fname = ಖಾಲಿ;
    char_u	**pp;
    char_u	*tofree;

    ಆದರೆ (new_dir == ಖಾಲಿ || allbuf_locked())
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (vim_strchr(p_cpo, CPO_CHDIR) != ಖಾಲಿ && curbuf_Is_ಬದಲಾಯಿತು() && !forceit)
    {
	emsg(_(e_cannot_change_directory_ಬಫರ್_is_modified_ಕೂಡಿಸು_bang_ಗಮ್ಯ_override));
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // ":cd -": Change to previous directory
    ಆದರೆ (STRCMP(new_dir, "-") == 0)
    {
	pdir = get_prevdir(scope);
	ಆದರೆ (pdir == ಖಾಲಿ)
	{
	    emsg(_(e_no_ಹಿಂದಿನ_directory));
	    ಹಿಂತಿರುಗಿ FALSE;
	}
	new_dir = pdir;
    }

    // Save current directory for next ":cd -"
    ಆದರೆ (mch_dirname(NameBuff, MAXPATHL) == OK)
	pdir = vim_strsave(NameBuff);
    ಇಲ್ಲದಿದ್ದರೆ
	pdir = ಖಾಲಿ;

    // For UNIX ":cd" means: go to home directory.
    // On other systems too if 'cdhome' is set.
#ಆದರೆ defined(UNIX) || defined(VMS)
    ಆದರೆ (*new_dir == NUL)
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (*new_dir == NUL && p_cdh)
#ಆದರೆ_ಅಂತ್ಯ
    {
	// use NameBuff for home directory name
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
	char_u	*p;

	p = mch_getenv((char_u *)"SYS$LOGIN");
	ಆದರೆ (p == ಖಾಲಿ || *p == NUL)	// empty is the same as not set
	    NameBuff[0] = NUL;
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_strncpy(NameBuff, p, MAXPATHL - 1);
#ಇಲ್ಲದಿದ್ದರೆ
	expand_env((char_u *)"$HOME", NameBuff, MAXPATHL);
#ಆದರೆ_ಅಂತ್ಯ
	new_dir = NameBuff;
    }
    dir_differs = pdir == ಖಾಲಿ
			    || pathcmp((ಅಕ್ಷರ *)pdir, (ಅಕ್ಷರ *)new_dir, -1) != 0;
    ಆದರೆ (dir_differs)
    {
	ಆದರೆ (scope == CDSCOPE_ಕಿಟಕಿ)
	    acmd_fname = (char_u *)"window";
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (scope == CDSCOPE_TABPAGE)
	    acmd_fname = (char_u *)"tabpage";
	ಇಲ್ಲದಿದ್ದರೆ
	    acmd_fname = (char_u *)"global";
	trigger_Dir_ಬದಲಾಯಿತು_Pre(acmd_fname, new_dir);

	ಆದರೆ (vim_chdir(new_dir))
	{
	    emsg(_(e_command_failed));
	    vim_free(pdir);
	    ಹಿಂತಿರುಗಿ FALSE;
	}
    }

    ಆದರೆ (scope == CDSCOPE_ಕಿಟಕಿ)
	pp = &curwin->w_prevdir;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (scope == CDSCOPE_TABPAGE)
	pp = &curtab->tp_prevdir;
    ಇಲ್ಲದಿದ್ದರೆ
	pp = &prev_dir;
    tofree = *pp;  // new_dir may use this
    *pp = pdir;

    post_chdir(scope);

    ಆದರೆ (dir_differs)
	apply_autocmds(EVENT_DIRCHANGED, acmd_fname, new_dir, FALSE, curbuf);
    vim_free(tofree);
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * ":cd", ":tcd", ":lcd", ":chdir" ":tchdir" and ":lchdir".
 */
    ಶೂನ್ಯ
ex_cd(exarg_T *eap)
{
    char_u	*new_dir;

    new_dir = eap->arg;
#ಆದರೆ !defined(UNIX) && !defined(VMS)
    // for non-UNIX ":cd" means: print current directory unless 'cdhome' is set
    ಆದರೆ (*new_dir == NUL && !p_cdh)
    {
	ex_pwd(ಖಾಲಿ);
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    cdscope_T	scope = CDSCOPE_GLOBAL;

    ಆದರೆ (eap->cmdidx == CMD_lcd || eap->cmdidx == CMD_lchdir)
	scope = CDSCOPE_ಕಿಟಕಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_tcd || eap->cmdidx == CMD_tchdir)
	scope = CDSCOPE_TABPAGE;

    ಆದರೆ (changedir_func(new_dir, eap->forceit, scope))
    {
	// Echo the new current directory if the command was typed.
	ಆದರೆ (ಕೀಲಿ_Typed || p_verbose >= 5)
	    ex_pwd(eap);
    }
}

/*
 * ":pwd".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_pwd(exarg_T *eap ಬಳಸದ)
{
    ಆದರೆ (mch_dirname(NameBuff, MAXPATHL) == OK)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
	slash_adjust(NameBuff);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (p_verbose > 0)
	{
	    ಅಕ್ಷರ *context = "global";

	    ಆದರೆ (ಕೊನೆಯ_chdir_reason != ಖಾಲಿ)
		context = ಕೊನೆಯ_chdir_reason;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_localdir != ಖಾಲಿ)
		context = "window";
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curtab->tp_localdir != ಖಾಲಿ)
		context = "tabpage";
	    smsg("[%s] %s", context, (ಅಕ್ಷರ *)NameBuff);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಂದೇಶ((ಅಕ್ಷರ *)NameBuff);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	emsg(_(e_directory_unknown));
}

/*
 * ":=".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_equal(exarg_T *eap)
{
    smsg("%ld", (ದೀರ್ಘ)eap->line2);
    ex_may_print(eap);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_sleep(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		n;
    ದೀರ್ಘ	len;

    ಆದರೆ (cursor_valid())
    {
	n = W_WINROW(curwin) + curwin->w_wrow - ಸಂದೇಶ_scrolled;
	ಆದರೆ (n >= 0)
	    windgoto(n, curwin->w_wincol + curwin->w_wcol);
    }

    len = eap->line2;
    ಆಯ್ಕೆ (*eap->arg)
    {
	ಸ್ಥಿತಿ 'm': ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ NUL: len *= 1000L; ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ: semsg(_(e_invalid_ವಾದ_str), eap->arg); ಹಿಂತಿರುಗಿ;
    }

    // Hide the cursor if invoked with !
    do_sleep(len, eap->forceit);
}

/*
 * Sleep for "msec" milliseconds, but keep checking for a CTRL-C every second.
 * Hide the cursor if "hide_cursor" is TRUE.
 */
    ಶೂನ್ಯ
do_sleep(ದೀರ್ಘ msec, ಪೂರ್ಣಾಂಕ hide_cursor)
{
    ದೀರ್ಘ	ಮುಗಿದ = 0;
    ದೀರ್ಘ	wait_now;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
    elapsed_T	ಆರಂಭ_tv;

    // Remember at what time we started, so that we know how much longer we
    // should wait after waiting for a bit.
    ELAPSED_INIT(ಆರಂಭ_tv);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (hide_cursor)
	cursor_sleep();
    ಇಲ್ಲದಿದ್ದರೆ
	cursor_on();

    ಹೊರಹೋಗು_flush_cursor(FALSE, FALSE);
    ಆಗ (!got_int && ಮುಗಿದ < msec)
    {
	wait_now = msec - ಮುಗಿದ > 1000L ? 1000L : msec - ಮುಗಿದ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
	{
	    ದೀರ್ಘ    due_ಸಮಯ = check_due_timer();

	    ಆದರೆ (due_ಸಮಯ > 0 && due_ಸಮಯ < wait_now)
		wait_now = due_ಸಮಯ;
	}
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	ಆದರೆ (has_any_channel() && wait_now > 20L)
	    wait_now = 20L;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SOUND
	ಆದರೆ (has_any_sound_callback() && wait_now > 20L)
	    wait_now = 20L;
#ಆದರೆ_ಅಂತ್ಯ
	ui_delay(wait_now, TRUE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	ಆದರೆ (has_any_channel())
	    ui_breakcheck_force(TRUE);
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ui_breakcheck();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಸಂದೇಶ_ಸರತಿ
	// Process the netbeans and clientserver messages that may have been
	// received in the call to ui_breakcheck() when the GUI is in use. This
	// may occur when running a test case.
	parse_queued_messages();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
	// actual time passed
	ಮುಗಿದ = ELAPSED_FUNC(ಆರಂಭ_tv);
#ಇಲ್ಲದಿದ್ದರೆ
	// guesstimate time passed (will actually be more)
	ಮುಗಿದ += wait_now;
#ಆದರೆ_ಅಂತ್ಯ
    }

    // If CTRL-C was typed to interrupt the sleep, drop the CTRL-C from the
    // input buffer, otherwise a following call to input() fails.
    ಆದರೆ (got_int)
	(ಶೂನ್ಯ)vpeekc();

    ಆದರೆ (hide_cursor)
	cursor_unsleep();
}

/*
 * ":winsize" command (obsolete).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_winsize(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		w, h;
    char_u	*arg = eap->arg;
    char_u	*p;

    ಆದರೆ (!ಸುರಕ್ಷಿತ_isdigit(*arg))
    {
	semsg(_(e_invalid_ವಾದ_str), arg);
	ಹಿಂತಿರುಗಿ;
    }
    w = getdigits(&arg);
    arg = skipwhite(arg);
    p = arg;
    h = getdigits(&arg);
    ಆದರೆ (*p != NUL && *arg == NUL)
	ಗಣ_shellsize(w, h, TRUE);
    ಇಲ್ಲದಿದ್ದರೆ
	emsg(_(e_winsize_requires_two_ಸಂಖ್ಯೆ_arguments));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_wincmd(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		xchar = NUL;
    char_u	*p;

    ಆದರೆ (*eap->arg == 'g' || *eap->arg == Ctrl_G)
    {
	// CTRL-W g and CTRL-W CTRL-G  have an extra command character
	ಆದರೆ (eap->arg[1] == NUL)
	{
	    emsg(_(e_invalid_ವಾದ));
	    ಹಿಂತಿರುಗಿ;
	}
	xchar = eap->arg[1];
	p = eap->arg + 2;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	p = eap->arg + 1;

    ಗಣ_nextcmd(eap, p);
    p = skipwhite(p);
    ಆದರೆ (*p != NUL && *p != (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಒಳ_vim9script() ? '#' :
#ಆದರೆ_ಅಂತ್ಯ
		'"')
	    && eap->nextcmd == ಖಾಲಿ)
	emsg(_(e_invalid_ವಾದ));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!eap->skip)
    {
	// Pass flags on for ":vertical wincmd ]".
	postponed_ವಿಭಜಿಸು_flags = cmdmod.cmod_ವಿಭಜಿಸು;
	postponed_ವಿಭಜಿಸು_tab = cmdmod.cmod_tab;
	do_ಕಿಟಕಿ(*eap->arg, eap->addr_ಎಣಿಕೆ > 0 ? eap->line2 : 0L, xchar);
	postponed_ವಿಭಜಿಸು_flags = 0;
	postponed_ವಿಭಜಿಸು_tab = 0;
    }
}

#ಆದರೆ defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * ":winpos".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_winpos(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		x, y;
    char_u	*arg = eap->arg;
    char_u	*p;

    ಆದರೆ (*arg == NUL)
    {
# ಆದರೆ defined(FEAT_GUI) || defined(MSWIN)
#  ifdef VIMDLL
	ಆದರೆ (gui.ಒಳ_use ? gui_mch_get_winpos(&x, &y) != FAIL :
		mch_get_winpos(&x, &y) != FAIL)
#  elif defined(FEAT_GUI)
	ಆದರೆ (gui.ಒಳ_use && gui_mch_get_winpos(&x, &y) != FAIL)
#  ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (mch_get_winpos(&x, &y) != FAIL)
#  endif
	{
	    sprintf((ಅಕ್ಷರ *)IObuff, _("Window position: X %d, Y %d"), x, y);
	    ಸಂದೇಶ((ಅಕ್ಷರ *)IObuff);
	}
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	    emsg(_(e_obtaining_ಕಿಟಕಿ_position_not_implemented_for_this_platform));
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	x = getdigits(&arg);
	arg = skipwhite(arg);
	p = arg;
	y = getdigits(&arg);
	ಆದರೆ (*p == NUL || *arg != NUL)
	{
	    emsg(_(e_winpos_requires_two_ಸಂಖ್ಯೆ_arguments));
	    ಹಿಂತಿರುಗಿ;
	}
# ifdef FEAT_GUI
	ಆದರೆ (gui.ಒಳ_use)
	    gui_mch_ಗಣ_winpos(x, y);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (gui.starting)
	{
	    // Remember the coordinates for when the window is opened.
	    gui_win_x = x;
	    gui_win_y = y;
	}
#  ಆದರೆ defined(HAVE_TGETENT) || defined(VIMDLL)
	ಇಲ್ಲದಿದ್ದರೆ
#  endif
# endif
# ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	    mch_ಗಣ_winpos(x, y);
# endif
# ifdef HAVE_TGETENT
	ಆದರೆ (*T_CWP)
	    ಪದ_ಗಣ_winpos(x, y);
# endif
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Handle command that work like operators: ":delete", ":yank", ":>" and ":<".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_operators(exarg_T *eap)
{
    oparg_T	oa;

    clear_oparg(&oa);
    oa.regname = eap->regname;
    oa.ಆರಂಭ.lnum = eap->line1;
    oa.ಕೊನೆ.lnum = eap->line2;
    oa.ಸಾಲು_ಎಣಿಕೆ = eap->line2 - eap->line1 + 1;
    oa.motion_type = MLINE;
    virtual_op = FALSE;
    ಆದರೆ (eap->cmdidx != CMD_yank)	// position cursor for undo
    {
	setpcmark();
	curwin->w_cursor.lnum = eap->line1;
	beginline(BL_SOL | BL_FIX);
    }

    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	ಕೊನೆ_visual_ಬಹುಲಕ();

    ಆಯ್ಕೆ (eap->cmdidx)
    {
	ಸ್ಥಿತಿ CMD_delete:
	    oa.op_type = OP_DELETE;
	    op_delete(&oa);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ CMD_yank:
	    oa.op_type = OP_YANK;
	    (ಶೂನ್ಯ)op_yank(&oa, FALSE, TRUE);
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:    // CMD_rshift or CMD_lshift
	    ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		(eap->cmdidx == CMD_rshift) ^ curwin->w_p_rl
#ಇಲ್ಲದಿದ್ದರೆ
		eap->cmdidx == CMD_rshift
#ಆದರೆ_ಅಂತ್ಯ
						)
		oa.op_type = OP_RSHIFT;
	    ಇಲ್ಲದಿದ್ದರೆ
		oa.op_type = OP_LSHIFT;
	    op_ಸರಿಸು(&oa, FALSE, eap->amount);
	    ನಿಲ್ಲಿಸು;
    }
    virtual_op = MAYBE;
    ex_may_print(eap);
}

/*
 * ":put".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_put(exarg_T *eap)
{
    // ":0put" works like ":1put!".
    ಆದರೆ (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_ಲಂಬಸಾಲು();
    do_put(eap->regname, ಖಾಲಿ, eap->forceit ? BACKWARD : FORWARD, 1L,
						       PUT_ಸಾಲು|PUT_CURSLINE);
}

/*
 * ":iput".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_iput(exarg_T *eap)
{
    // ":0iput" works like ":1iput!".
    ಆದರೆ (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_ಲಂಬಸಾಲು();
    do_put(eap->regname, ಖಾಲಿ, eap->forceit ? BACKWARD : FORWARD, 1L,
						      PUT_ಸಾಲು|PUT_CURSLINE
						      |PUT_FIXINDENT);
}

/*
 * Handle ":copy" and ":move".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_copymove(exarg_T *eap)
{
    ದೀರ್ಘ	n;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (not_ಒಳ_vim9(eap, false) == FAIL)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    n = get_ವಿಳಾಸ(eap, &eap->arg, eap->addr_type, FALSE, FALSE, FALSE, 1);
    ಆದರೆ (eap->arg == ಖಾಲಿ)	    // error detected
    {
	eap->nextcmd = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ;
    }
    get_flags(eap);

    /*
     * move or copy lines from 'eap->line1'-'eap->line2' to below line 'n'
     */
    ಆದರೆ (n == MAXLNUM || n < 0 || n > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	emsg(_(e_invalid_range));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (eap->cmdidx == CMD_move)
    {
	ಆದರೆ (do_move(eap->line1, eap->line2, n) == FAIL)
	    ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ex_copy(eap->line1, eap->line2, n);
    u_clearline();
    beginline(BL_SOL | BL_FIX);
    ex_may_print(eap);
}

/*
 * Print the current line if flags were given to the Ex command.
 */
    ಶೂನ್ಯ
ex_may_print(exarg_T *eap)
{
    ಆದರೆ (eap->flags != 0)
    {
	print_ಸಾಲು(curwin->w_cursor.lnum, (eap->flags & EXFLAG_NR),
						  (eap->flags & EXFLAG_LIST));
	ex_no_reprint = TRUE;
    }
}

/*
 * ":smagic" and ":snomagic".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_join(exarg_T *eap)
{
    curwin->w_cursor.lnum = eap->line1;
    ಆದರೆ (eap->line1 == eap->line2)
    {
	ಆದರೆ (eap->addr_ಎಣಿಕೆ >= 2)   // :2,2join does nothing
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (eap->line2 == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    beep_flush();
	    ಹಿಂತಿರುಗಿ;
	}
	++eap->line2;
    }
    (ಶೂನ್ಯ)do_join(eap->line2 - eap->line1 + 1, !eap->forceit, TRUE, TRUE, TRUE);
    beginline(BL_WHITE | BL_FIX);
    ex_may_print(eap);
}

/*
 * ":[addr]@r" or ":[addr]*r": execute register
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_at(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		prev_len = typebuf.tb_len;

    curwin->w_cursor.lnum = eap->line2;
    check_cursor_ಲಂಬಸಾಲು();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#ಆದರೆ_ಅಂತ್ಯ

    // get the register name.  No name means to use the previous one
    c = *eap->arg;
    ಆದರೆ (c == NUL || (c == '*' && *eap->cmd == '*'))
	c = '@';
    // Put the register in the typeahead buffer with the "silent" flag.
    ಆದರೆ (do_execreg(c, TRUE, vim_strchr(p_cpo, CPO_EXECBUF) != ಖಾಲಿ, TRUE)
								      == FAIL)
    {
	beep_flush();
	ಹಿಂತಿರುಗಿ;
    }

    ಪೂರ್ಣಾಂಕ	save_efr = exec_from_reg;

    exec_from_reg = TRUE;

    /*
     * Execute from the typeahead buffer.
     * Continue until the stuff buffer is empty and all added characters
     * have been consumed.
     */
    ಆಗ (!stuff_ಖಾಲಿಗಳು() || typebuf.tb_len > prev_len)
	(ಶೂನ್ಯ)do_cmdline(ಖಾಲಿ, getexline, ಖಾಲಿ, DOCMD_NOWAIT|DOCMD_VERBOSE);

    exec_from_reg = save_efr;
}

/*
 * ":!".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_bang(exarg_T *eap)
{
    do_bang(eap->addr_ಎಣಿಕೆ, eap, eap->forceit, TRUE, TRUE);
}

/*
 * ":undo".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_undo(exarg_T *eap)
{
    ಆದರೆ (eap->addr_ಎಣಿಕೆ == 1)	    // :undo 123
	undo_ಸಮಯ(eap->line2, FALSE, FALSE, TRUE);
    ಇಲ್ಲದಿದ್ದರೆ
	u_undo(1);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_wundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_ಗಾತ್ರ];

    u_compute_hash(hash);
    u_write_undo(eap->arg, eap->forceit, curbuf, hash);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_rundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_ಗಾತ್ರ];

    u_compute_hash(hash);
    u_read_undo(eap->arg, hash, ಖಾಲಿ);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * ":redo".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_redo(exarg_T *eap ಬಳಸದ)
{
    u_redo(1);
}

/*
 * ":earlier" and ":later".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_later(exarg_T *eap)
{
    ದೀರ್ಘ	ಎಣಿಕೆ_2 = 0;
    ಪೂರ್ಣಾಂಕ		sec = FALSE;
    ಪೂರ್ಣಾಂಕ		file = FALSE;
    char_u	*p = eap->arg;

    ಆದರೆ (*p == NUL)
	ಎಣಿಕೆ_2 = 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸುರಕ್ಷಿತ_isdigit(*p))
    {
	ಎಣಿಕೆ_2 = getdigits(&p);
	ಆಯ್ಕೆ (*p)
	{
	    ಸ್ಥಿತಿ 's': ++p; sec = TRUE; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'm': ++p; sec = TRUE; ಎಣಿಕೆ_2 *= 60; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'h': ++p; sec = TRUE; ಎಣಿಕೆ_2 *= 60 * 60; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'd': ++p; sec = TRUE; ಎಣಿಕೆ_2 *= 24 * 60 * 60; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'f': ++p; file = TRUE; ನಿಲ್ಲಿಸು;
	}
    }

    ಆದರೆ (*p != NUL)
	semsg(_(e_invalid_ವಾದ_str), eap->arg);
    ಇಲ್ಲದಿದ್ದರೆ
	undo_ಸಮಯ(eap->cmdidx == CMD_earlier ? -ಎಣಿಕೆ_2 : ಎಣಿಕೆ_2,
							    sec, file, FALSE);
}

/*
 * ":redir": start/stop redirection.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_redir(exarg_T *eap)
{
    ಅಕ್ಷರ	*ಬಹುಲಕ;
    char_u	*fname;
    char_u	*arg = eap->arg;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (redir_execute)
    {
	emsg(_(e_cannot_use_redir_inside_execute));
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (STRICMP(eap->arg, "END") == 0)
	close_redir();
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (*arg == '>')
	{
	    ++arg;
	    ಆದರೆ (*arg == '>')
	    {
		++arg;
		ಬಹುಲಕ = "a";
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಬಹುಲಕ = "w";
	    arg = skipwhite(arg);

	    close_redir();

	    // Expand environment variables and "~/".
	    fname = expand_env_save(arg);
	    ಆದರೆ (fname == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
	    ಆದರೆ (cmdmod.cmod_flags & CMOD_BROWSE)
	    {
		char_u	*browseFile;

		browseFile = do_browse(BROWSE_SAVE,
			(char_u *)_("Save Redirection"),
			fname, ಖಾಲಿ, ಖಾಲಿ,
			(char_u *)_(BROWSE_FILTER_ALL_FILES), curbuf);
		ಆದರೆ (browseFile == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ;		// operation cancelled
		vim_free(fname);
		fname = browseFile;
		eap->forceit = TRUE;	// since dialog already asked
	    }
#ಆದರೆ_ಅಂತ್ಯ

	    redir_fd = open_exfile(fname, eap->forceit, ಬಹುಲಕ);
	    vim_free(fname);
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*arg == '@')
	{
	    // redirect to a register a-z (resp. A-Z for appending)
	    close_redir();
	    ++arg;
	    ಆದರೆ (ASCII_ISALPHA(*arg)
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
		ಆದರೆ (*arg == '>' && arg[1] == '>')  // append
		    arg += 2;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Can use both "@a" and "@a>".
		    ಆದರೆ (*arg == '>')
			arg++;
		    // Make register empty when not using @A-@Z and the
		    // command is valid.
		    ಆದರೆ (*arg == NUL && !ಸುರಕ್ಷಿತ_isupper(redir_reg))
			write_reg_contents(redir_reg, (char_u *)"", -1, FALSE);
		}
	    }
	    ಆದರೆ (*arg != NUL)
	    {
		redir_reg = 0;
		semsg(_(e_invalid_ವಾದ_str), eap->arg);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*arg == '=' && arg[1] == '>')
	{
	    ಪೂರ್ಣಾಂಕ append;

	    // redirect to a variable
	    close_redir();
	    arg += 2;

	    ಆದರೆ (*arg == '>')
	    {
		++arg;
		append = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		append = FALSE;

	    ಆದರೆ (var_redir_ಆರಂಭ(skipwhite(arg), append) == OK)
		redir_vname = 1;
	}
#ಆದರೆ_ಅಂತ್ಯ

	// TODO: redirect to a buffer

	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_invalid_ವಾದ_str), eap->arg);
    }

    // Make sure redirection is not off.  Can happen for cmdline completion
    // that indirectly invokes a command to catch its output.
    ಆದರೆ (redir_fd != ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
			  || redir_reg || redir_vname
#ಆದರೆ_ಅಂತ್ಯ
							)
	redir_off = FALSE;
}

/*
 * ":redraw": force redraw, with clear for ":redraw!".
 */
    ಶೂನ್ಯ
ex_redraw(exarg_T *eap)
{
    redraw_cmd(eap->forceit);
}

/*
 * ":redraw": force redraw, with clear if "clear" is TRUE.
 */
    ಶೂನ್ಯ
redraw_cmd(ಪೂರ್ಣಾಂಕ clear)
{
    ಪೂರ್ಣಾಂಕ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ಪೂರ್ಣಾಂಕ save_p_lz = p_lz;
    p_lz = FALSE;

    validate_cursor();
    update_topline();
    update_screen(clear ? UPD_CLEAR : VIsual_ಸಕ್ರಿಯ ? UPD_INVERTED : 0);
    ಆದರೆ ((State & ಬಹುಲಕ_CMDLINE) == 0)
	setcursor(); // put cursor back where it belongs
    ಆದರೆ (need_maketitle)
	maketitle();
#ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
    ಆದರೆ (!gui.ಒಳ_use)
# endif
	resize_console_buf();
#ಆದರೆ_ಅಂತ್ಯ
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;

    // After drawing the statusline screen_attr may still be set.
    screen_stop_highlight();

    // Reset msg_didout, so that a message that's there is overwritten.
    ಸಂದೇಶ_didout = FALSE;
    ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;

    // No need to wait after an intentional redraw.
    need_wait_return = FALSE;

    // When invoked from a callback or autocmd the command line may be active.
    ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
	redrawcmdline();

    ಹೊರಹೋಗು_flush();
}

/*
 * ":redrawstatus": force redraw of status line(s)
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_redrawstatus(exarg_T *eap ಬಳಸದ)
{
    ಆದರೆ (eap->forceit)
	ಸ್ಥಿತಿ_redraw_all();
    ಇಲ್ಲದಿದ್ದರೆ
	ಸ್ಥಿತಿ_redraw_curbuf();
    redraw_vseps = TRUE;
    ಆದರೆ (ಸಂದೇಶ_scrolled && (State & ಬಹುಲಕ_CMDLINE))
	ಹಿಂತಿರುಗಿ;  // redraw later

    ಪೂರ್ಣಾಂಕ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ಪೂರ್ಣಾಂಕ save_p_lz = p_lz;
    p_lz = FALSE;

    ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
	redraw_statuslines();
    ಇಲ್ಲದಿದ್ದರೆ
	update_screen(VIsual_ಸಕ್ರಿಯ ? UPD_INVERTED : 0);
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    ಹೊರಹೋಗು_flush();
}

/*
 * ":redrawtabline": force redraw of the tabline
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_redrawtabline(exarg_T *eap ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ಪೂರ್ಣಾಂಕ save_p_lz = p_lz;
    p_lz = FALSE;

    draw_tabline();

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    ಹೊರಹೋಗು_flush();
}

/*
 * ":redrawtabpanel": force redraw of the tabpanel
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_redrawtabpanel(exarg_T *eap ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ಪೂರ್ಣಾಂಕ save_p_lz = p_lz;
    p_lz = FALSE;

#ಆದರೆ defined(FEAT_TABPANEL)
    draw_tabpanel();
#ಇಲ್ಲದಿದ್ದರೆ
    emsg(_(e_cannot_not_support_redrawtabpanel));
#ಆದರೆ_ಅಂತ್ಯ

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    ಹೊರಹೋಗು_flush();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
close_redir(ಶೂನ್ಯ)
{
    ಆದರೆ (redir_fd != ಖಾಲಿ)
    {
	fclose(redir_fd);
	redir_fd = ಖಾಲಿ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    redir_reg = 0;
    ಆದರೆ (redir_vname)
    {
	var_redir_stop();
	redir_vname = 0;
    }
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_SESSION) || defined(FEAT_EVAL)
    ಪೂರ್ಣಾಂಕ
vim_mkdir_emsg(char_u *name, ಪೂರ್ಣಾಂಕ prot ಬಳಸದ)
{
    ಆದರೆ (vim_mkdir(name, prot) != 0)
    {
	semsg(_(e_cannot_create_directory_str), name);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Open a file for writing for an Ex command, with some checks.
 * Return file descriptor, or NULL on failure.
 */
    FILE *
open_exfile(
    char_u	*fname,
    ಪೂರ್ಣಾಂಕ		forceit,
    ಅಕ್ಷರ	*ಬಹುಲಕ)	    // "w" for create new file or "a" for append
{
    FILE	*fd;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    // with Unix it is possible to open a directory
    ಆದರೆ (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (!forceit && *ಬಹುಲಕ != 'a' && vim_fexists(fname))
    {
	semsg(_(e_str_exists_ಕೂಡಿಸು_bang_ಗಮ್ಯ_override), fname);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ ((fd = mch_fopen((ಅಕ್ಷರ *)fname, ಬಹುಲಕ)) == ಖಾಲಿ)
	semsg(_(e_cannot_open_str_for_writing_2), fname);

    ಹಿಂತಿರುಗಿ fd;
}

/*
 * ":mark" and ":k".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_mark(exarg_T *eap)
{
    pos_T	pos;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (not_ಒಳ_vim9(eap, false) == FAIL)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (*eap->arg == NUL)		// No argument?
    {
	emsg(_(e_ವಾದ_required));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (eap->arg[1] != NUL)	// more than one character?
    {
	semsg(_(e_trailing_characters_str), eap->arg);
	ಹಿಂತಿರುಗಿ;
    }

    pos = curwin->w_cursor;		// save curwin->w_cursor
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_WHITE | BL_FIX);
    ಆದರೆ (setmark(*eap->arg) == FAIL)	// set mark
	emsg(_(e_ವಾದ_must_be_letter_or_forward_backward_quote));
    curwin->w_cursor = pos;		// restore curwin->w_cursor
}

/*
 * Update w_topline, w_leftcol and the cursor position.
 */
    ಶೂನ್ಯ
update_topline_cursor(ಶೂನ್ಯ)
{
    check_cursor();		// put cursor on valid line
    update_topline();
    ಆದರೆ (!curwin->w_p_wrap)
	validate_cursor();
    update_curswant();
}

/*
 * Save the current State and go to Normal mode.
 * Return TRUE if the typeahead could be saved.
 */
    ಪೂರ್ಣಾಂಕ
save_current_state(save_state_T *sst)
{
    sst->save_ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll;
    sst->save_restart_edit = restart_edit;
    sst->save_ಸಂದೇಶ_didout = ಸಂದೇಶ_didout;
    sst->save_State = State;
    sst->save_insertmode = p_im;
    sst->save_ಮುಕ್ತಾಯ_op = ಮುಕ್ತಾಯ_op;
    sst->save_opcount = opcount;
    sst->save_reg_executing = reg_executing;
    sst->save_pending_ಕೊನೆ_reg_executing = pending_ಕೊನೆ_reg_executing;

    ಸಂದೇಶ_scroll = FALSE;		    // no msg scrolling in Normal mode
    restart_edit = 0;		    // don't go to Insert mode
    p_im = FALSE;		    // don't use 'insertmode'

    sst->save_script_ಆವೃತ್ತಿ = current_sctx.sc_ಆವೃತ್ತಿ;
    current_sctx.sc_ಆವೃತ್ತಿ = 1;    // not in Vim9 script

    /*
     * Save the current typeahead.  This is required to allow using ":normal"
     * from an event handler and makes sure we don't hang when the argument
     * ends with half a command.
     */
    save_typeahead(&sst->tabuf);
    ಹಿಂತಿರುಗಿ sst->tabuf.typebuf_valid;
}

    ಶೂನ್ಯ
restore_current_state(save_state_T *sst)
{
    // Restore the previous typeahead.
    restore_typeahead(&sst->tabuf, FALSE);

    ಸಂದೇಶ_scroll = sst->save_ಸಂದೇಶ_scroll;
    restart_edit = sst->save_restart_edit;
    p_im = sst->save_insertmode;
    ಮುಕ್ತಾಯ_op = sst->save_ಮುಕ್ತಾಯ_op;
    opcount = sst->save_opcount;
    reg_executing = sst->save_reg_executing;
    pending_ಕೊನೆ_reg_executing = sst->save_pending_ಕೊನೆ_reg_executing;
    ಸಂದೇಶ_didout |= sst->save_ಸಂದೇಶ_didout;	// don't reset msg_didout now
    current_sctx.sc_ಆವೃತ್ತಿ = sst->save_script_ಆವೃತ್ತಿ;

    // Restore the state (needed when called from a function executed for
    // 'indentexpr'). Update the mouse and cursor, they may have changed.
    State = sst->save_State;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * ":normal[!] {commands}": Execute normal mode commands.
 */
    ಶೂನ್ಯ
ex_ಸಾಮಾನ್ಯ(exarg_T *eap)
{
    save_state_T save_state;
    char_u	*arg = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		l;
    char_u	*p;

    ಆದರೆ (ex_ಸಾಮಾನ್ಯ_ಬೀಗ > 0)
    {
	emsg(_(e_not_allowed_ಇಲ್ಲಿ));
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (ex_ಸಾಮಾನ್ಯ_busy >= p_mmd)
    {
	emsg(_(e_recursive_use_ದ_ಸಾಮಾನ್ಯ_too_deep));
	ಹಿಂತಿರುಗಿ;
    }

    /*
     * vgetc() expects a CSI and K_SPECIAL to have been escaped.  Don't do
     * this for the K_SPECIAL leading byte, otherwise special keys will not
     * work.
     */
    ಆದರೆ (has_mbyte)
    {
	ಪೂರ್ಣಾಂಕ	len = 0;

	// Count the number of characters to be escaped.
	ಗಾಗಿ (p = eap->arg; *p != NUL; ++p)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    ಆದರೆ (*p == CSI)  // leadbyte CSI
		len += 2;
#ಆದರೆ_ಅಂತ್ಯ
	    ಗಾಗಿ (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		ಆದರೆ (*++p == K_SPECIAL	  // trailbyte K_SPECIAL or CSI
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
			|| *p == CSI
#ಆದರೆ_ಅಂತ್ಯ
			)
		    len += 2;
	}
	ಆದರೆ (len > 0)
	{
	    arg = alloc(STRLEN(eap->arg) + len + 1);
	    ಆದರೆ (arg != ಖಾಲಿ)
	    {
		len = 0;
		ಗಾಗಿ (p = eap->arg; *p != NUL; ++p)
		{
		    arg[len++] = *p;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		    ಆದರೆ (*p == CSI)
		    {
			arg[len++] = KS_EXTRA;
			arg[len++] = (ಪೂರ್ಣಾಂಕ)KE_CSI;
		    }
#ಆದರೆ_ಅಂತ್ಯ
		    ಗಾಗಿ (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		    {
			arg[len++] = *++p;
			ಆದರೆ (*p == K_SPECIAL)
			{
			    arg[len++] = KS_SPECIAL;
			    arg[len++] = KE_FILLER;
			}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == CSI)
			{
			    arg[len++] = KS_EXTRA;
			    arg[len++] = (ಪೂರ್ಣಾಂಕ)KE_CSI;
			}
#ಆದರೆ_ಅಂತ್ಯ
		    }
		    arg[len] = NUL;
		}
	    }
	}
    }

    ++ex_ಸಾಮಾನ್ಯ_busy;
    ಆದರೆ (save_current_state(&save_state))
    {
	/*
	 * Repeat the :normal command for each line in the range.  When no
	 * range given, execute it just once, without positioning the cursor
	 * first.
	 */
	ಮಾಡು
	{
	    ಆದರೆ (eap->addr_ಎಣಿಕೆ != 0)
	    {
		curwin->w_cursor.lnum = eap->line1++;
		curwin->w_cursor.ಲಂಬಸಾಲು = 0;
		check_cursor_moved(curwin);
	    }

	    exec_ಸಾಮಾನ್ಯ_cmd(arg != ಖಾಲಿ
		     ? arg
		     : eap->arg, eap->forceit ? REMAP_NONE : REMAP_YES, FALSE);
	}
	ಆಗ (eap->addr_ಎಣಿಕೆ > 0 && eap->line1 <= eap->line2 && !got_int);
    }

    // Might not return to the main loop when in an event handler.
    update_topline_cursor();

    restore_current_state(&save_state);
    --ex_ಸಾಮಾನ್ಯ_busy;
    setmouse();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#ಆದರೆ_ಅಂತ್ಯ

    vim_free(arg);
}

/*
 * ":startinsert", ":startreplace" and ":startgreplace"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_startinsert(exarg_T *eap)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    // Ignore this when running in an active terminal.
    ಆದರೆ (ಪದ_ಕೆಲಸ_running(curbuf->b_ಪದ))
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (!curbuf->b_p_ma && !p_im)
    {
	// Only give this error when 'insertmode' is off.
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (eap->forceit)
    {
	// cursor line can be zero on startup
	ಆದರೆ (!curwin->w_cursor.lnum)
	    curwin->w_cursor.lnum = 1;
	ಗಣ_cursor_for_append_ಗಮ್ಯ_ಸಾಲು();
    }

    // Ignore the command when already in Insert mode.  Inserting an
    // expression register that invokes a function can do this.
    ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (eap->cmdidx == CMD_startinsert)
	restart_edit = 'a';
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_startreplace)
	restart_edit = 'R';
    ಇಲ್ಲದಿದ್ದರೆ
	restart_edit = 'V';

    ಆದರೆ (!eap->forceit)
    {
	ಆದರೆ (eap->cmdidx == CMD_startinsert)
	    restart_edit = 'i';
	curwin->w_curswant = 0;	    // avoid MAXCOL
    }

    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	showmode();
}

/*
 * ":stopinsert"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_stopinsert(exarg_T *eap ಬಳಸದ)
{
    restart_edit = 0;
    stop_ಸೇರಿಸು_ಬಹುಲಕ = TRUE;
#ಆದರೆ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // when called from remote_expr in insert mode, make sure insert mode is
    // ended by adding K_NOP to the typeahead buffer
    ಆದರೆ (vgetc_busy)
	ins_char_typebuf(K_NOP, 0);
#ಆದರೆ_ಅಂತ್ಯ
    clearmode();
}

/*
 * Execute normal mode command "cmd".
 * "remap" can be REMAP_NONE or REMAP_YES.
 */
    ಶೂನ್ಯ
exec_ಸಾಮಾನ್ಯ_cmd(char_u *cmd, ಪೂರ್ಣಾಂಕ remap, ಪೂರ್ಣಾಂಕ silent)
{
    // Stuff the argument into the typeahead buffer.
    ins_typebuf(cmd, remap, 0, TRUE, silent);
    exec_ಸಾಮಾನ್ಯ(FALSE, FALSE, FALSE);
}

/*
 * Execute normal_cmd() until there is no typeahead left.
 * When "use_vpeekc" is TRUE use vpeekc() to check for available chars.
 */
    ಶೂನ್ಯ
exec_ಸಾಮಾನ್ಯ(ಪೂರ್ಣಾಂಕ was_typed, ಪೂರ್ಣಾಂಕ use_vpeekc, ಪೂರ್ಣಾಂಕ may_use_terminal_loop ಬಳಸದ)
{
    oparg_T	oa;
    ಪೂರ್ಣಾಂಕ		c;

    // When calling vpeekc() from feedkeys() it will return Ctrl_C when there
    // is nothing to get, so also check for Ctrl_C.
    clear_oparg(&oa);
    ಮುಕ್ತಾಯ_op = FALSE;
    ಆಗ ((!stuff_ಖಾಲಿಗಳು()
		|| ((was_typed || !typebuf_typed()) && typebuf.tb_len > 0)
		|| (use_vpeekc && (c = vpeekc()) != NUL && c != Ctrl_C))
	    && !got_int)
    {
	update_topline_cursor();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	ಆದರೆ (may_use_terminal_loop && ಪದ_use_loop()
		&& oa.op_type == OP_NOP && oa.regname == NUL
		&& !VIsual_ಸಕ್ರಿಯ)
	{
	    // If terminal_loop() returns OK we got a key that is handled
	    // in Normal model.  With FAIL we first need to position the
	    // cursor and the screen needs to be redrawn.
	    ಆದರೆ (terminal_loop(TRUE) == OK)
		ಸಾಮಾನ್ಯ_cmd(&oa, TRUE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    // execute a Normal mode cmd
	    ಸಾಮಾನ್ಯ_cmd(&oa, TRUE);
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_checkpath(exarg_T *eap)
{
    ಕಂಡುಹಿಡಿ_ನಮೂನೆ_ಒಳ_ಮಾರ್ಗ(ಖಾಲಿ, 0, 0, FALSE, FALSE, CHECK_ಮಾರ್ಗ, 1L,
	    eap->forceit ? ACTION_SHOW_ALL : ACTION_SHOW,
	    (linenr_T)1, (linenr_T)MAXLNUM, eap->forceit, FALSE);
}

# ಆದರೆ defined(FEAT_QUICKFIX)
/*
 * ":psearch"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_psearch(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;
    ex_findpat(eap);
    g_do_tagpreview = 0;
}
# endif

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_findpat(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		ಸಂಪೂರ್ಣ = TRUE;
    ದೀರ್ಘ	n;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		action;

    ಆಯ್ಕೆ (cmdnames[eap->cmdidx].cmd_name[2])
    {
	ಸ್ಥಿತಿ 'e':	// ":psearch", ":isearch" and ":dsearch"
		ಆದರೆ (cmdnames[eap->cmdidx].cmd_name[0] == 'p')
		    action = ACTION_GOTO;
		ಇಲ್ಲದಿದ್ದರೆ
		    action = ACTION_SHOW;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'i':	// ":ilist" and ":dlist"
		action = ACTION_SHOW_ALL;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'u':	// ":ijump" and ":djump"
		action = ACTION_GOTO;
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:	// ":isplit" and ":dsplit"
		action = ACTION_ವಿಭಜಿಸು;
		ನಿಲ್ಲಿಸು;
    }

    n = 1;
    ಆದರೆ (vim_isdigit(*eap->arg))	// get count
    {
	n = getdigits(&eap->arg);
	eap->arg = skipwhite(eap->arg);
    }
    ಆದರೆ (*eap->arg == '/')   // Match regexp, not just whole words
    {
	ಸಂಪೂರ್ಣ = FALSE;
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	ಆದರೆ (*p)
	{
	    *p++ = NUL;
	    p = skipwhite(p);

	    // Check for trailing illegal characters
	    ಆದರೆ (!ತುದಿಗಳು_excmd2(eap->arg, p))
		eap->errmsg = ex_errmsg(e_trailing_characters_str, p);
	    ಇಲ್ಲದಿದ್ದರೆ
		ಗಣ_nextcmd(eap, p);
	}
    }
    ಆದರೆ (!eap->skip)
	ಕಂಡುಹಿಡಿ_ನಮೂನೆ_ಒಳ_ಮಾರ್ಗ(eap->arg, 0, (ಪೂರ್ಣಾಂಕ)STRLEN(eap->arg),
		ಸಂಪೂರ್ಣ, !eap->forceit,
		*eap->cmd == 'd' ? ಕಂಡುಹಿಡಿ_DEFINE : ಕಂಡುಹಿಡಿ_ANY, n, action,
		eap->line1, eap->line2, eap->forceit, FALSE);
}
#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
/*
 * ":ptag", ":ptselect", ":ptjump", ":ptnext", etc.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_ptag(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;  // will be reset to 0 in ex_tag_cmd()
    ex_ಟ್ಯಾಗ್_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
}

/*
 * ":pedit"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_pedit(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_ಕಿಟಕಿ();

    // Edit the file.
    do_exedit(eap, ಖಾಲಿ);

    ಹಿಂಭಾಗ_ಗಮ್ಯ_current_ಕಿಟಕಿ(curwin_save);
}

/*
 * ":pbuffer"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_pbuffer(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_ಕಿಟಕಿ();

    // Go to the buffer.
    do_exbuffer(eap);

    ಹಿಂಭಾಗ_ಗಮ್ಯ_current_ಕಿಟಕಿ(curwin_save);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prepare_preview_ಕಿಟಕಿ(ಶೂನ್ಯ)
{
    ಆದರೆ (ERROR_IF_ANY_POPUP_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ;

    // Open the preview window or popup and make it the current window.
    g_do_tagpreview = p_pvh;
    prepare_tagpreview(TRUE, TRUE, FALSE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಹಿಂಭಾಗ_ಗಮ್ಯ_current_ಕಿಟಕಿ(win_T *curwin_save)
{
    ಆದರೆ (curwin != curwin_save && win_valid(curwin_save))
    {
	// Return cursor to where we were
	validate_cursor();
	redraw_later(UPD_VALID);
	win_enter(curwin_save, TRUE);
    }
# ifdef FEAT_PROP_POPUP
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (WIN_IS_POPUP(curwin))
    {
	// can't keep focus in popup window
	win_enter(firstwin, TRUE);
    }
# endif
    g_do_tagpreview = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * ":stag", ":stselect" and ":stjump".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_stag(exarg_T *eap)
{
    postponed_ವಿಭಜಿಸು = -1;
    postponed_ವಿಭಜಿಸು_flags = cmdmod.cmod_ವಿಭಜಿಸು;
    postponed_ವಿಭಜಿಸು_tab = cmdmod.cmod_tab;
    ex_ಟ್ಯಾಗ್_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
    postponed_ವಿಭಜಿಸು_flags = 0;
    postponed_ವಿಭಜಿಸು_tab = 0;
}

/*
 * ":tag", ":tselect", ":tjump", ":tnext", etc.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_ಟ್ಯಾಗ್(exarg_T *eap)
{
    ex_ಟ್ಯಾಗ್_cmd(eap, cmdnames[eap->cmdidx].cmd_name);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_ಟ್ಯಾಗ್_cmd(exarg_T *eap, char_u *name)
{
    ಪೂರ್ಣಾಂಕ		cmd;

    ಆಯ್ಕೆ (name[1])
    {
	ಸ್ಥಿತಿ 'j': cmd = DT_JUMP;	// ":tjump"
		  ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 's': cmd = DT_SELECT;	// ":tselect"
		  ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'p': cmd = DT_PREV;	// ":tprevious"
		  ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'N': cmd = DT_PREV;	// ":tNext"
		  ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'n': cmd = DT_ಮುಂದಿನ;	// ":tnext"
		  ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'o': cmd = DT_ತೆಗೆ;		// ":pop"
		  ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'f':			// ":tfirst"
	ಸ್ಥಿತಿ 'r': cmd = DT_ಮೊದಲ;	// ":trewind"
		  ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'l': cmd = DT_ಕೊನೆಯ;	// ":tlast"
		  ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:			// ":tag"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
		  ಆದರೆ (p_cst && *eap->arg != NUL)
		  {
		      ex_cstag(eap);
		      ಹಿಂತಿರುಗಿ;
		  }
#ಆದರೆ_ಅಂತ್ಯ
		  cmd = DT_ಟ್ಯಾಗ್;
		  ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (name[0] == 'l')
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_QUICKFIX
	ex_ni(eap);
	ಹಿಂತಿರುಗಿ;
#ಇಲ್ಲದಿದ್ದರೆ
	cmd = DT_LTAG;
#ಆದರೆ_ಅಂತ್ಯ
    }

    do_ಟ್ಯಾಗ್(eap->arg, cmd, eap->addr_ಎಣಿಕೆ > 0 ? (ಪೂರ್ಣಾಂಕ)eap->line2 : 1,
							  eap->forceit, TRUE);
}

ಎಣಿಕೆ {
    SPEC_PERC = 0,
    SPEC_HASH,
    SPEC_CWORD,	    // cursor word
    SPEC_CCWORD,    // cursor WORD
    SPEC_CEXPR,	    // expr under cursor
    SPEC_CFILE,	    // cursor path name
    SPEC_SFILE,	    // ":so" file name
    SPEC_SLNUM,	    // ":so" file line number
    SPEC_ರಾಶಿ,	    // call stack
    SPEC_SCRIPT,    // script file name
    SPEC_AFILE,	    // autocommand file name
    SPEC_ABUF,	    // autocommand buffer number
    SPEC_AMATCH,    // autocommand match name
    SPEC_SFLNUM,    // script file line number
    SPEC_SID	    // script ID: <SNR>123_
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
    , SPEC_CLIENT
#ಆದರೆ_ಅಂತ್ಯ
};

/*
 * Check "str" for starting with a special cmdline variable.
 * If found return one of the SPEC_ values and set "*usedlen" to the length of
 * the variable.  Otherwise return -1 and "*usedlen" is unchanged.
 */
    ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_cmdline_var(char_u *src, size_t *usedlen)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    ಸ್ಥಿರ keyvalue_T spec_str_tab[] = {
	KEYVALUE_ನಮೂದು(SPEC_SID, "SID>"),	    // script ID: <SNR>123_
	KEYVALUE_ನಮೂದು(SPEC_ABUF, "abuf>"),	    // autocommand buffer number
	KEYVALUE_ನಮೂದು(SPEC_AFILE, "afile>"),	    // autocommand file name
	KEYVALUE_ನಮೂದು(SPEC_AMATCH, "amatch>"),	    // autocommand match name
	KEYVALUE_ನಮೂದು(SPEC_CCWORD, "cWORD>"),	    // cursor WORD
	KEYVALUE_ನಮೂದು(SPEC_CEXPR, "cexpr>"),	    // expr under cursor
	KEYVALUE_ನಮೂದು(SPEC_CFILE, "cfile>"),	    // cursor path name
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
	KEYVALUE_ನಮೂದು(SPEC_CLIENT, "client>"),
#ಆದರೆ_ಅಂತ್ಯ
	KEYVALUE_ನಮೂದು(SPEC_CWORD, "cword>"),	    // cursor word
	KEYVALUE_ನಮೂದು(SPEC_SCRIPT, "script>"),	    // script file name
	KEYVALUE_ನಮೂದು(SPEC_SFILE, "sfile>"),	    // ":so" file name
	KEYVALUE_ನಮೂದು(SPEC_SFLNUM, "sflnum>"),	    // script file line number
	KEYVALUE_ನಮೂದು(SPEC_SLNUM, "slnum>"),	    // ":so" file line number
	KEYVALUE_ನಮೂದು(SPEC_ರಾಶಿ, "stack>")	    // call stack
    };
    keyvalue_T ಗುರಿ;
    keyvalue_T *ನಮೂದು;

    ಆಯ್ಕೆ (*src)
    {
    ಸ್ಥಿತಿ '%':
	*usedlen = 1;
	ಹಿಂತಿರುಗಿ SPEC_PERC;

    ಸ್ಥಿತಿ '#':
	*usedlen = 1;
	ಹಿಂತಿರುಗಿ SPEC_HASH;

    ಸ್ಥಿತಿ '<':
	ಗುರಿ.ಕೀಲಿ = 0;
	ಗುರಿ.ಮೌಲ್ಯ.string = src + 1;	    // skip over '<'
	ಗುರಿ.ಮೌಲ್ಯ.ಉದ್ದಳತೆ = 0;	    // not used, see cmp_keyvalue_value_n()

	ನಮೂದು = (keyvalue_T *)bsearch(&ಗುರಿ, &spec_str_tab,
		ARRAY_ಉದ್ದಳತೆ(spec_str_tab), ಗಾತ್ರ(spec_str_tab[0]),
		cmp_keyvalue_ಮೌಲ್ಯ_n);
	ಆದರೆ (ನಮೂದು == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;

	*usedlen = ನಮೂದು->ಮೌಲ್ಯ.ಉದ್ದಳತೆ + 1;
	ಹಿಂತಿರುಗಿ ನಮೂದು->ಕೀಲಿ;

    ಪೂರ್ವನಿಯೋಜಿತ:
	ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ -1;
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
    ಅಕ್ಷರ	**errormsg,	// pointer to error message
    ಪೂರ್ಣಾಂಕ		*escaped,	// return value has escaped white space (can
				// be NULL)
    ಪೂರ್ಣಾಂಕ		ಖಾಲಿಗಳು_is_error)	// empty result is considered an error
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*s;
    char_u	*ಫಲಿತಾಂಶ = (char_u *)"";
    char_u	*resultbuf = ಖಾಲಿ;
    size_t	resultlen;
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		valid = VALID_ತಲೆ + VALID_ಮಾರ್ಗ;    // assume valid result
    ಪೂರ್ಣಾಂಕ		spec_idx;
    ಪೂರ್ಣಾಂಕ		tilde_file = FALSE;
    ಪೂರ್ಣಾಂಕ		skip_mod = FALSE;
    char_u	strbuf[30];

    *errormsg = ಖಾಲಿ;
    ಆದರೆ (escaped != ಖಾಲಿ)
	*escaped = FALSE;

    /*
     * Check if there is something to do.
     */
    spec_idx = ಕಂಡುಹಿಡಿ_cmdline_var(src, usedlen);
    ಆದರೆ (spec_idx < 0)	// no match
    {
	*usedlen = 1;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    /*
     * Skip when preceded with a backslash "\%" and "\#".
     * Note: In "\\%" the % is also not recognized!
     */
    ಆದರೆ (src > srcstart && src[-1] == '\\')
    {
	*usedlen = 0;
	STRMOVE(src - 1, src);	// remove backslash
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    /*
     * word or WORD under cursor
     */
    ಆದರೆ (spec_idx == SPEC_CWORD || spec_idx == SPEC_CCWORD
						     || spec_idx == SPEC_CEXPR)
    {
	resultlen = ಕಂಡುಹಿಡಿ_ident_under_cursor(&ಫಲಿತಾಂಶ,
		spec_idx == SPEC_CWORD ? (ಕಂಡುಹಿಡಿ_IDENT | ಕಂಡುಹಿಡಿ_STRING)
	      : spec_idx == SPEC_CEXPR ? (ಕಂಡುಹಿಡಿ_IDENT | ಕಂಡುಹಿಡಿ_STRING | ಕಂಡುಹಿಡಿ_EVAL)
	      : ಕಂಡುಹಿಡಿ_STRING);
	ಆದರೆ (resultlen == 0)
	{
	    *errormsg = "";
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }

    /*
     * '#': Alternate file name
     * '%': Current file name
     *	    File name under the cursor
     *	    File name for autocommand
     *	and following modifiers
     */
    ಇಲ್ಲದಿದ್ದರೆ
    {
	size_t off = 0;

	ಆಯ್ಕೆ (spec_idx)
	{
	ಸ್ಥಿತಿ SPEC_PERC:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		ಆದರೆ (!ಒಳ_vim9script() || src[1] != '%')
#ಆದರೆ_ಅಂತ್ಯ
		{
		    // '%': current file
		    ಆದರೆ (curbuf->b_fname == ಖಾಲಿ)
		    {
			ಫಲಿತಾಂಶ = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಫಲಿತಾಂಶ = curbuf->b_fname;
			tilde_file = STRCMP(ಫಲಿತಾಂಶ, "~") == 0;
		    }
		    ನಿಲ್ಲಿಸು;
		}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		// "%%" alternate file
		off = 1;
#ಆದರೆ_ಅಂತ್ಯ
		// FALLTHROUGH
	ಸ್ಥಿತಿ SPEC_HASH:		// '#' or "#99": alternate file
		ಆದರೆ (off == 0 ? src[1] == '#' : src[2] == '%')
		{
		    // "##" or "%%%": the argument list
		    ಫಲಿತಾಂಶ = arg_all();
		    resultbuf = ಫಲಿತಾಂಶ;
		    *usedlen = off + 2;
		    ಆದರೆ (escaped != ಖಾಲಿ)
			*escaped = TRUE;
		    skip_mod = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
		s = src + off + 1;
		ಆದರೆ (*s == '<')		// "#<99" uses v:oldfiles
		    ++s;
		i = (ಪೂರ್ಣಾಂಕ)getdigits(&s);
		ಆದರೆ (s == src + off + 2 && src[off + 1] == '-')
		    // just a minus sign, don't skip over it
		    s--;
		*usedlen = (ಪೂರ್ಣಾಂಕ)(s - src); // length of what we expand

		ಆದರೆ (src[off + 1] == '<' && i != 0)
		{
		    ಆದರೆ (*usedlen < off + 2)
		    {
			// Should we give an error message for #<text?
			*usedlen = off + 1;
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    ಫಲಿತಾಂಶ = list_ಕಂಡುಹಿಡಿ_str(get_vim_var_list(VV_OLDFILES),
								     (ದೀರ್ಘ)i);
		    ಆದರೆ (ಫಲಿತಾಂಶ == ಖಾಲಿ)
		    {
			*errormsg = "";
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		    }
#ಇಲ್ಲದಿದ್ದರೆ
		    *errormsg = _(e_hashsmall_is_not_ಲಭ್ಯ_without_the_eval_feature);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (i == 0 && src[off + 1] == '<' && *usedlen > off + 1)
			*usedlen = off + 1;
		    buf = buflist_findnr(i);
		    ಆದರೆ (buf == ಖಾಲಿ)
		    {
			*errormsg = _(e_no_alternate_file_name_ಗಮ್ಯ_substitute_for_hash);
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		    }
		    ಆದರೆ (lnump != ಖಾಲಿ)
			*lnump = ECMD_ಕೊನೆಯ;
		    ಆದರೆ (buf->b_fname == ಖಾಲಿ)
		    {
			ಫಲಿತಾಂಶ = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಫಲಿತಾಂಶ = buf->b_fname;
			tilde_file = STRCMP(ಫಲಿತಾಂಶ, "~") == 0;
		    }
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SPEC_CFILE:	// file name under cursor
		ಫಲಿತಾಂಶ = file_name_at_cursor(FNAME_MESS|FNAME_HYP, 1L, ಖಾಲಿ);
		ಆದರೆ (ಫಲಿತಾಂಶ == ಖಾಲಿ)
		{
		    *errormsg = "";
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		resultbuf = ಫಲಿತಾಂಶ;	    // remember allocated string
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SPEC_AFILE:	// file name for autocommand
		ಫಲಿತಾಂಶ = autocmd_fname;
		ಆದರೆ (ಫಲಿತಾಂಶ != ಖಾಲಿ && !autocmd_fname_ತುಂಬಿದ)
		{
		    // Still need to turn the fname into a full path.  It is
		    // postponed to avoid a delay when <afile> is not used.
		    autocmd_fname_ತುಂಬಿದ = TRUE;
		    ಫಲಿತಾಂಶ = ತುಂಬಿದ_Name_save(autocmd_fname, FALSE);
		    vim_free(autocmd_fname);
		    autocmd_fname = ಫಲಿತಾಂಶ;
		}
		ಆದರೆ (ಫಲಿತಾಂಶ == ಖಾಲಿ)
		{
		    *errormsg = _(e_no_autocommand_file_name_ಗಮ್ಯ_substitute_for_afile);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		ಫಲಿತಾಂಶ = shorten_fname1(ಫಲಿತಾಂಶ);
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SPEC_ABUF:		// buffer number for autocommand
		ಆದರೆ (autocmd_bufnr <= 0)
		{
		    *errormsg = _(e_no_autocommand_ಬಫರ್_ಸಂಖ್ಯೆ_ಗಮ್ಯ_substitute_for_abuf);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		sprintf((ಅಕ್ಷರ *)strbuf, "%d", autocmd_bufnr);
		ಫಲಿತಾಂಶ = strbuf;
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SPEC_AMATCH:	// match name for autocommand
		ಫಲಿತಾಂಶ = autocmd_ಹೊಂದಿಕೆ;
		ಆದರೆ (ಫಲಿತಾಂಶ == ಖಾಲಿ)
		{
		    *errormsg = _(e_no_autocommand_ಹೊಂದಿಕೆ_name_ಗಮ್ಯ_substitute_for_amatch);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SPEC_SFILE:	// file name for ":so" command
		ಫಲಿತಾಂಶ = estack_sfile(ESTACK_SFILE);
		ಆದರೆ (ಫಲಿತಾಂಶ == ಖಾಲಿ)
		{
		    *errormsg = _(e_no_ಮೂಲ_file_name_ಗಮ್ಯ_substitute_for_sfile);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		resultbuf = ಫಲಿತಾಂಶ;	    // remember allocated string
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SPEC_ರಾಶಿ:	// call stack
		ಫಲಿತಾಂಶ = estack_sfile(ESTACK_ರಾಶಿ);
		ಆದರೆ (ಫಲಿತಾಂಶ == ಖಾಲಿ)
		{
		    *errormsg = _(e_no_call_ರಾಶಿ_ಗಮ್ಯ_substitute_for_ರಾಶಿ);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		resultbuf = ಫಲಿತಾಂಶ;	    // remember allocated string
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SPEC_SCRIPT:	// script file name
		ಫಲಿತಾಂಶ = estack_sfile(ESTACK_SCRIPT);
		ಆದರೆ (ಫಲಿತಾಂಶ == ಖಾಲಿ)
		{
		    *errormsg = _(e_no_script_file_name_ಗಮ್ಯ_substitute_for_script);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		resultbuf = ಫಲಿತಾಂಶ;	    // remember allocated string
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SPEC_SLNUM:	// line in file for ":so" command
		ಆದರೆ (SOURCING_NAME == ಖಾಲಿ || SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_ಸಾಲು_ಸಂಖ್ಯೆ_ಗಮ್ಯ_use_for_slnum);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		sprintf((ಅಕ್ಷರ *)strbuf, "%ld", SOURCING_LNUM);
		ಫಲಿತಾಂಶ = strbuf;
		ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಸ್ಥಿತಿ SPEC_SFLNUM:	// line in script file
		ಆದರೆ (current_sctx.sc_lnum + SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_ಸಾಲು_ಸಂಖ್ಯೆ_ಗಮ್ಯ_use_for_sflnum);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		sprintf((ಅಕ್ಷರ *)strbuf, "%ld",
				 (ದೀರ್ಘ)(current_sctx.sc_lnum + SOURCING_LNUM));
		ಫಲಿತಾಂಶ = strbuf;
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SPEC_SID:
		ಆದರೆ (current_sctx.sc_sid <= 0)
		{
		    *errormsg = _(e_using_sid_not_ಒಳ_script_context);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		sprintf((ಅಕ್ಷರ *)strbuf, "<SNR>%d_", current_sctx.sc_sid);
		ಫಲಿತಾಂಶ = strbuf;
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
	ಸ್ಥಿತಿ SPEC_CLIENT:	// Source of last submitted input
# ifdef MSWIN
		sprintf((ಅಕ್ಷರ *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)client_ಕಿಟಕಿ);
		ಫಲಿತಾಂಶ = strbuf;
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef FEAT_SOCKETSERVER
		ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_SOCKET)
		{
		    ಆದರೆ (client_socket == ಖಾಲಿ)
			ಫಲಿತಾಂಶ = (char_u *)"";
		    ಇಲ್ಲದಿದ್ದರೆ
			ಫಲಿತಾಂಶ = client_socket;
		}
#  endif
#  ifdef FEAT_X11
		ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_X11)
		{
		    sprintf((ಅಕ್ಷರ *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)client_ಕಿಟಕಿ);
		    ಫಲಿತಾಂಶ = strbuf;
		}
#  endif
# endif
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	}

	resultlen = STRLEN(ಫಲಿತಾಂಶ);	// length of new string
	ಆದರೆ (src[*usedlen] == '<')	// remove the file name extension
	{
	    ++*usedlen;
	    ಆದರೆ ((s = vim_strrchr(ಫಲಿತಾಂಶ, '.')) != ಖಾಲಿ && s >= gettail(ಫಲಿತಾಂಶ))
		resultlen = s - ಫಲಿತಾಂಶ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!skip_mod)
	{
	    valid |= modify_fname(src, tilde_file, usedlen, &ಫಲಿತಾಂಶ, &resultbuf,
								  &resultlen);
	    ಆದರೆ (ಫಲಿತಾಂಶ == ಖಾಲಿ)
	    {
		*errormsg = "";
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	}
    }

    ಆದರೆ (resultlen == 0 || valid != VALID_ತಲೆ + VALID_ಮಾರ್ಗ)
    {
	ಆದರೆ (ಖಾಲಿಗಳು_is_error)
	{
	    ಆದರೆ (valid != VALID_ತಲೆ + VALID_ಮಾರ್ಗ)
		*errormsg = _(e_ಖಾಲಿಗಳು_file_name_for_percent_or_hash_only_ಕೆಲಸಮಾಡುತ್ತದೆ_with_ph);
	    ಇಲ್ಲದಿದ್ದರೆ
		*errormsg = _(e_evaluates_ಗಮ್ಯ_an_ಖಾಲಿಗಳು_string);
	}
	ಫಲಿತಾಂಶ = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಫಲಿತಾಂಶ = vim_strnsave(ಫಲಿತಾಂಶ, resultlen);
    vim_free(resultbuf);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Expand the <sfile> string in "arg".
 *
 * Returns an allocated string, or NULL for any error.
 */
    char_u *
expand_sfile(char_u *arg)
{
    ಅಕ್ಷರ	*errormsg;
    char_u	*ಫಲಿತಾಂಶ;
    size_t	resultlen;
    char_u	*newres;
    size_t	len;
    char_u	*repl;
    size_t	repllen;
    size_t	srclen;
    char_u	*p;

    resultlen = STRLEN(arg);
    ಫಲಿತಾಂಶ = vim_strnsave(arg, resultlen);
    ಆದರೆ (ಫಲಿತಾಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (p = ಫಲಿತಾಂಶ; *p; )
    {
	ಆದರೆ (STRNCMP(p, "<sfile>", 7) != 0)
	    ++p;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // replace "<sfile>" with the sourced file name, and do ":" stuff
	    repl = eval_vars(p, ಫಲಿತಾಂಶ, &srclen, ಖಾಲಿ, &errormsg, ಖಾಲಿ, TRUE);
	    ಆದರೆ (errormsg != ಖಾಲಿ)
	    {
		ಆದರೆ (*errormsg)
		    emsg(errormsg);
		vim_free(ಫಲಿತಾಂಶ);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    ಆದರೆ (repl == ಖಾಲಿ)		// no match (cannot happen)
	    {
		p += srclen;
		ಮುಂದುವರಿ;
	    }
	    repllen = STRLEN(repl);
	    resultlen += repllen - srclen;
	    newres = alloc(resultlen + 1);
	    ಆದರೆ (newres == ಖಾಲಿ)
	    {
		vim_free(repl);
		vim_free(ಫಲಿತಾಂಶ);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    len = p - ಫಲಿತಾಂಶ;
	    mch_memmove(newres, ಫಲಿತಾಂಶ, len);
	    STRCPY(newres + len, repl);
	    len += repllen;
	    STRCPY(newres + len, p + srclen);
	    vim_free(repl);
	    vim_free(ಫಲಿತಾಂಶ);
	    ಫಲಿತಾಂಶ = newres;
	    p = newres + len;		// continue after the match
	}
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
/*
 * Make a dialog message in "buff[DIALOG_MSG_SIZE]".
 * "format" must contain "%s".
 */
    ಶೂನ್ಯ
dialog_ಸಂದೇಶ(char_u *buff, ಅಕ್ಷರ *format, char_u *fname)
{
    ಆದರೆ (fname == ಖಾಲಿ)
	fname = (char_u *)_("Untitled");
    vim_snprintf((ಅಕ್ಷರ *)buff, DIALOG_ಸಂದೇಶ_ಗಾತ್ರ, format, fname);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * ":behave {mswin,xterm}"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_behave(exarg_T *eap)
{
    ಆದರೆ (STRCMP(eap->arg, "mswin") == 0)
    {
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"selection",
						 0L, (char_u *)"exclusive", 0);
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"selectmode",
						 0L, (char_u *)"mouse,key", 0);
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"mousemodel",
						     0L, (char_u *)"popup", 0);
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"keymodel",
					  0L, (char_u *)"startsel,stopsel", 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(eap->arg, "xterm") == 0)
    {
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"selection",
						 0L, (char_u *)"inclusive", 0);
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"selectmode", 0L, (char_u *)"", 0);
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"mousemodel",
						    0L, (char_u *)"extend", 0);
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"keymodel", 0L, (char_u *)"", 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_invalid_ವಾದ_str), eap->arg);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ filetype_detect = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ filetype_plugin = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ filetype_indent = FALSE;

/*
 * ":filetype [plugin] [indent] {on,off,detect}"
 * on: Load the filetype.vim file to install autocommands for file types.
 * off: Load the ftoff.vim file to remove all autocommands for file types.
 * plugin on: load filetype.vim and ftplugin.vim
 * plugin off: load ftplugof.vim
 * indent on: load filetype.vim and indent.vim
 * indent off: load indoff.vim
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_filetype(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಂಕ		plugin = FALSE;
    ಪೂರ್ಣಾಂಕ		indent = FALSE;

    ಆದರೆ (*eap->arg == NUL)
    {
	// Print current status.
	smsg("filetype detection:%s  plugin:%s  indent:%s",
		filetype_detect ? "ON" : "OFF",
		filetype_plugin ? (filetype_detect ? "ON" : "(on)") : "OFF",
		filetype_indent ? (filetype_detect ? "ON" : "(on)") : "OFF");
	ಹಿಂತಿರುಗಿ;
    }

    // Accept "plugin" and "indent" in any order.
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (STRNCMP(arg, "plugin", 6) == 0)
	{
	    plugin = TRUE;
	    arg = skipwhite(arg + 6);
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (STRNCMP(arg, "indent", 6) == 0)
	{
	    indent = TRUE;
	    arg = skipwhite(arg + 6);
	    ಮುಂದುವರಿ;
	}
	ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (STRCMP(arg, "on") == 0 || STRCMP(arg, "detect") == 0)
    {
	ಆದರೆ (*arg == 'o' || !filetype_detect)
	{
	    ಮೂಲ_runtime((char_u *)FILETYPE_FILE, DIP_ALL);
	    filetype_detect = TRUE;
	    ಆದರೆ (plugin)
	    {
		ಮೂಲ_runtime((char_u *)FTPLUGIN_FILE, DIP_ALL);
		filetype_plugin = TRUE;
	    }
	    ಆದರೆ (indent)
	    {
		ಮೂಲ_runtime((char_u *)INDENT_FILE, DIP_ALL);
		filetype_indent = TRUE;
	    }
	}
	ಆದರೆ (*arg == 'd')
	{
	    (ಶೂನ್ಯ)do_doautocmd((char_u *)"filetypedetect BufRead", TRUE, ಖಾಲಿ);
	    do_modelines(0);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(arg, "off") == 0)
    {
	ಆದರೆ (plugin || indent)
	{
	    ಆದರೆ (plugin)
	    {
		ಮೂಲ_runtime((char_u *)FTPLUGOF_FILE, DIP_ALL);
		filetype_plugin = FALSE;
	    }
	    ಆದರೆ (indent)
	    {
		ಮೂಲ_runtime((char_u *)INDOFF_FILE, DIP_ALL);
		filetype_indent = FALSE;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಮೂಲ_runtime((char_u *)FTOFF_FILE, DIP_ALL);
	    filetype_detect = FALSE;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_invalid_ವಾದ_str), arg);
}

/*
 * ":setfiletype [FALLBACK] {name}"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_setfiletype(exarg_T *eap)
{
    ಆದರೆ (curbuf->b_did_filetype)
	ಹಿಂತಿರುಗಿ;

    char_u *arg = eap->arg;
    ಆದರೆ (STRNCMP(arg, "FALLBACK ", 9) == 0)
	arg += 9;

    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"filetype", 0L, arg, OPT_LOCAL);
    ಆದರೆ (arg != eap->arg)
	curbuf->b_did_filetype = false;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_digraphs(exarg_T *eap ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIGRAPHS
    ಆದರೆ (*eap->arg != NUL)
	putdigraph(eap->arg);
    ಇಲ್ಲದಿದ್ದರೆ
	listdigraphs(eap->forceit);
#ಇಲ್ಲದಿದ್ದರೆ
    emsg(_(e_no_digraphs_ಆವೃತ್ತಿ));
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_ಹುಡುಕು_EXTRA)
    ಶೂನ್ಯ
ಗಣ_no_hlsearch(ಪೂರ್ಣಾಂಕ flag)
{
    no_hlsearch = flag;
# ifdef FEAT_EVAL
    ಗಣ_vim_var_nr(VV_HLSEARCH, !no_hlsearch && p_hls);
# endif
}

/*
 * ":nohlsearch"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_nohlsearch(exarg_T *eap ಬಳಸದ)
{
    ಗಣ_no_hlsearch(TRUE);
    redraw_all_later(UPD_SOME_VALID);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
/*
 * ":X": Get crypt key
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_X(exarg_T *eap ಬಳಸದ)
{
    crypt_check_current_ವಿಧಾನ();
    (ಶೂನ್ಯ)crypt_get_ಕೀಲಿ(TRUE, TRUE);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_fold(exarg_T *eap)
{
    ಆದರೆ (foldManualAllowed(TRUE))
	foldCreate(eap->line1, eap->line2);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_foldopen(exarg_T *eap)
{
    opFoldRange(eap->line1, eap->line2, eap->cmdidx == CMD_foldopen,
							 eap->forceit, FALSE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_folddo(exarg_T *eap)
{
    linenr_T	lnum;

# ifdef FEAT_CLIPBOARD
    ಆರಂಭ_global_changes();
# endif
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
# endif

    // First set the marks for all lines closed/open.
    ಗಾಗಿ (lnum = eap->line1; lnum <= eap->line2; ++lnum)
	ಆದರೆ (hasFolding(lnum, ಖಾಲಿ, ಖಾಲಿ) == (eap->cmdidx == CMD_folddoclosed))
	    ml_setmarked(lnum);

    // Execute the command on the marked lines.
    global_exe(eap->arg);
    ml_clearmarked();	   // clear rest of the marks
# ifdef FEAT_CLIPBOARD
    ಕೊನೆ_global_changes();
# endif
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_QUICKFIX)
/*
 * Returns TRUE if the supplied Ex cmdidx is for a location list command
 * instead of a quickfix command.
 */
    ಪೂರ್ಣಾಂಕ
is_loclist_cmd(ಪೂರ್ಣಾಂಕ cmdidx)
{
    ಆದರೆ (cmdidx < 0 || cmdidx >= CMD_ಗಾತ್ರ)
	ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ cmdnames[cmdidx].cmd_name[0] == 'l';
}
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ
get_pressedreturn(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ex_pressedreturn;
}

    ಶೂನ್ಯ
ಗಣ_pressedreturn(ಪೂರ್ಣಾಂಕ val)
{
    ex_pressedreturn = val;
}
