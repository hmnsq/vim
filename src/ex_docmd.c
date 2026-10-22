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

#അന്തര്ഭാവ "vim.h"

സ്ഥിതം പൂർണ്ണസംഖ്യ	quitmore = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ	ex_pressedreturn = FALSE;
#ifndef FEAT_PRINTER
# define ex_hardcopy	ex_ni
#endif

#ifdef FEAT_EVAL
സ്ഥിതം char_u	*do_one_cmd(char_u **, പൂർണ്ണസംഖ്യ, cstack_T *, char_u *(*fgetline)(പൂർണ്ണസംഖ്യ, ശൂന്യം *, പൂർണ്ണസംഖ്യ, getline_opt_T), ശൂന്യം *cookie);
#അല്ലെങ്കിൽ
സ്ഥിതം char_u	*do_one_cmd(char_u **, പൂർണ്ണസംഖ്യ, char_u *(*fgetline)(പൂർണ്ണസംഖ്യ, ശൂന്യം *, പൂർണ്ണസംഖ്യ, getline_opt_T), ശൂന്യം *cookie);
സ്ഥിതം പൂർണ്ണസംഖ്യ	if_level = 0;		// depth in :if
#endif
സ്ഥിതം ശൂന്യം	append_command(char_u *cmd);

#ifndef FEAT_MENU
# define ex_emenu		ex_ni
# define ex_menu		ex_ni
# define ex_menutranslate	ex_ni
#endif
സ്ഥിതം ശൂന്യം	ex_autocmd(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_doautocmd(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_bunload(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_buffer(exarg_T *eap);
സ്ഥിതം ശൂന്യം	do_exbuffer(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_bmodified(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_bnext(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_bprevious(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_brewind(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_blast(exarg_T *eap);
സ്ഥിതം char_u	*getargcmd(char_u **);
സ്ഥിതം പൂർണ്ണസംഖ്യ	getargopt(exarg_T *eap);
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
#എങ്കിൽ !defined(FEAT_QUICKFIX) || !defined(FEAT_EVAL)
# define ex_cexpr		ex_ni
#endif

സ്ഥിതം linenr_T default_address(exarg_T *eap);
സ്ഥിതം ശൂന്യം address_default_all(exarg_T *eap);
സ്ഥിതം ശൂന്യം	get_flags(exarg_T *eap);
#എങ്കിൽ !defined(FEAT_PERL) \
	|| !defined(FEAT_PYTHON) || !defined(FEAT_PYTHON3) \
	|| !defined(FEAT_TCL) \
	|| !defined(FEAT_RUBY) \
	|| !defined(FEAT_LUA) \
	|| !defined(FEAT_MZSCHEME)
# define HAVE_EX_SCRIPT_NI
സ്ഥിതം ശൂന്യം	ex_script_ni(exarg_T *eap);
#endif
സ്ഥിതം അക്ഷരം	*invalid_range(exarg_T *eap);
സ്ഥിതം ശൂന്യം	correct_range(exarg_T *eap);
#ifdef FEAT_QUICKFIX
സ്ഥിതം char_u	*replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep);
#endif
സ്ഥിതം char_u	*repl_cmdline(exarg_T *eap, char_u *src, size_t srclen, char_u *repl, char_u **cmdlinep);
സ്ഥിതം ശൂന്യം	ex_highlight(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_colorscheme(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_cquit(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_quit_all(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_close(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_win_close(പൂർണ്ണസംഖ്യ forceit, win_T *win, tabpage_T *tp);
സ്ഥിതം ശൂന്യം	ex_only(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_resize(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_stag(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_tabclose(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_tabonly(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_tabnext(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_tabmove(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_tabs(exarg_T *eap);
#എങ്കിൽ defined(FEAT_QUICKFIX)
സ്ഥിതം ശൂന്യം	ex_pclose(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_ptag(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_pedit(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_pbuffer(exarg_T *eap);
സ്ഥിതം ശൂന്യം	prepare_preview_window(ശൂന്യം);
സ്ഥിതം ശൂന്യം	back_to_current_window(win_T *curwin_save);
#അല്ലെങ്കിൽ
# define ex_pclose		ex_ni
# define ex_ptag		ex_ni
# define ex_pedit		ex_ni
# define ex_pbuffer		ex_ni
#endif
സ്ഥിതം ശൂന്യം	ex_hide(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_exit(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_print(exarg_T *eap);
#ifdef FEAT_BYTEOFF
സ്ഥിതം ശൂന്യം	ex_goto(exarg_T *eap);
#അല്ലെങ്കിൽ
# define ex_goto		ex_ni
#endif
സ്ഥിതം ശൂന്യം	ex_shell(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_preserve(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_recover(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_mode(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_wrongmodifier(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_find(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_open(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_edit(exarg_T *eap);
#ifndef FEAT_GUI
# define ex_gui			ex_nogui
സ്ഥിതം ശൂന്യം	ex_nogui(exarg_T *eap);
#endif
#എങ്കിൽ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
സ്ഥിതം ശൂന്യം	ex_tearoff(exarg_T *eap);
#അല്ലെങ്കിൽ
# define ex_tearoff		ex_ni
#endif
#എങ്കിൽ (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
സ്ഥിതം ശൂന്യം	ex_popup(exarg_T *eap);
#അല്ലെങ്കിൽ
# define ex_popup		ex_ni
#endif
#ifndef FEAT_GUI_MSWIN
# define ex_simalt		ex_ni
#endif
#എങ്കിൽ !defined(FEAT_GUI_MSWIN) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
# define gui_mch_find_dialog	ex_ni
# define gui_mch_replace_dialog ex_ni
#endif
#എങ്കിൽ !defined(FEAT_GUI_GTK)
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
#എങ്കിൽ !defined(FEAT_SYN_HL) || !defined(FEAT_PROFILE)
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
#എങ്കിൽ !defined(FEAT_PYTHON) && !defined(FEAT_PYTHON3)
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
സ്ഥിതം ശൂന്യം	ex_swapname(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_syncbind(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_read(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_pwd(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_equal(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_sleep(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_winsize(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_wincmd(exarg_T *eap);
#എങ്കിൽ defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
സ്ഥിതം ശൂന്യം	ex_winpos(exarg_T *eap);
#അല്ലെങ്കിൽ
# define ex_winpos	    ex_ni
#endif
സ്ഥിതം ശൂന്യം	ex_operators(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_put(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_iput(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_copymove(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_submagic(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_join(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_at(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_bang(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_undo(exarg_T *eap);
#ifdef FEAT_PERSISTENT_UNDO
സ്ഥിതം ശൂന്യം	ex_wundo(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_rundo(exarg_T *eap);
#endif
സ്ഥിതം ശൂന്യം	ex_redo(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_later(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_redir(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_redrawstatus(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_redrawtabline(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_redrawtabpanel(exarg_T *eap);
സ്ഥിതം ശൂന്യം	close_redir(ശൂന്യം);
സ്ഥിതം ശൂന്യം	ex_mark(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_startinsert(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_stopinsert(exarg_T *eap);
#ifdef FEAT_FIND_ID
സ്ഥിതം ശൂന്യം	ex_checkpath(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_findpat(exarg_T *eap);
#അല്ലെങ്കിൽ
# define ex_findpat		ex_ni
# define ex_checkpath		ex_ni
#endif
#എങ്കിൽ defined(FEAT_FIND_ID) && defined(FEAT_QUICKFIX)
സ്ഥിതം ശൂന്യം	ex_psearch(exarg_T *eap);
#അല്ലെങ്കിൽ
# define ex_psearch		ex_ni
#endif
സ്ഥിതം ശൂന്യം	ex_tag(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_tag_cmd(exarg_T *eap, char_u *name);
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
സ്ഥിതം ശൂന്യം	ex_behave(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_filetype(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_setfiletype(exarg_T *eap);
#ifndef FEAT_DIFF
# define ex_diffoff		ex_ni
# define ex_diffpatch		ex_ni
# define ex_diffgetput		ex_ni
# define ex_diffsplit		ex_ni
# define ex_diffthis		ex_ni
# define ex_diffupdate		ex_ni
#endif
സ്ഥിതം ശൂന്യം	ex_digraphs(exarg_T *eap);
#ifdef FEAT_തിരയൽ_EXTRA
സ്ഥിതം ശൂന്യം	ex_nohlsearch(exarg_T *eap);
#അല്ലെങ്കിൽ
# define ex_nohlsearch		ex_ni
# define ex_match		ex_ni
#endif
#ifdef FEAT_CRYPT
സ്ഥിതം ശൂന്യം	ex_X(exarg_T *eap);
#അല്ലെങ്കിൽ
# define ex_X			ex_ni
#endif
#ifdef FEAT_FOLDING
സ്ഥിതം ശൂന്യം	ex_fold(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_foldopen(exarg_T *eap);
സ്ഥിതം ശൂന്യം	ex_folddo(exarg_T *eap);
#അല്ലെങ്കിൽ
# define ex_fold		ex_ni
# define ex_foldopen		ex_ni
# define ex_folddo		ex_ni
#endif
#എങ്കിൽ !(defined(HAVE_LOCALE_H) || defined(X_LOCALE))
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
#എങ്കിൽ !defined(FEAT_X11) || !defined(FEAT_XCLIPBOARD)
# define ex_xrestore		ex_ni
#endif
#എങ്കിൽ !defined(FEAT_WAYLAND)
# define ex_wlrestore		ex_ni
#endif
#എങ്കിൽ !defined(HAVE_CLIPMETHOD)
# define ex_clipreset		ex_ni
#endif
#എങ്കിൽ !defined(FEAT_PROP_POPUP)
# define ex_popupclear		ex_ni
#endif

/*
 * Declare cmdnames[].
 */
#define DO_DECLARE_EXCMD
#അന്തര്ഭാവ "ex_cmds.h"
#അന്തര്ഭാവ "ex_cmdidxs.h"

സ്ഥിതം char_u dollar_command[2] = {'$', 0};


#ifdef FEAT_EVAL
// Struct for storing a line inside a while/for loop
തരനിർവചനം ഘടന
{
    char_u	*വരി;		// command line
    linenr_T	lnum;		// sourcing_lnum of the line
} wcmd_T;

/*
 * Structure used to store info for line position in a while or for loop.
 * This is required, because do_one_cmd() may invoke ex_function(), which
 * reads more lines that may come from the while/for loop.
 */
ഘടന loop_cookie
{
    garray_T	*വരികൾ_gap;		// growarray with line info
    പൂർണ്ണസംഖ്യ		current_വരി;		// last read line from growarray
    പൂർണ്ണസംഖ്യ		repeating;		// TRUE when looping a second time
    // When "repeating" is FALSE use "getline" and "cookie" to get lines
    char_u	*(*lc_getline)(പൂർണ്ണസംഖ്യ, ശൂന്യം *, പൂർണ്ണസംഖ്യ, getline_opt_T);
    ശൂന്യം	*cookie;
};

സ്ഥിതം char_u	*get_loop_വരി(പൂർണ്ണസംഖ്യ c, ശൂന്യം *cookie, പൂർണ്ണസംഖ്യ indent, getline_opt_T options);
സ്ഥിതം പൂർണ്ണസംഖ്യ	store_loop_വരി(garray_T *gap, char_u *വരി);
സ്ഥിതം ശൂന്യം	free_cmdlines(garray_T *gap);

// Struct to save a few things while debugging.  Used in do_cmdline() only.
ഘടന dbg_stuff
{
    പൂർണ്ണസംഖ്യ		trylevel;
    പൂർണ്ണസംഖ്യ		force_abort;
    except_T	*caught_അട്ടി;
    char_u	*vv_exception;
    char_u	*vv_throwpoint;
    പൂർണ്ണസംഖ്യ		did_emsg;
    പൂർണ്ണസംഖ്യ		got_int;
    പൂർണ്ണസംഖ്യ		did_throw;
    പൂർണ്ണസംഖ്യ		need_rethrow;
    പൂർണ്ണസംഖ്യ		check_cstack;
    except_T	*current_exception;
};

    സ്ഥിതം ശൂന്യം
save_dbg_stuff(ഘടന dbg_stuff *dsp)
{
    dsp->trylevel	= trylevel;		trylevel = 0;
    dsp->force_abort	= force_abort;		force_abort = FALSE;
    dsp->caught_അട്ടി	= caught_അട്ടി;		caught_അട്ടി = ശൂന്യ;
    dsp->vv_exception	= v_exception(ശൂന്യ);
    dsp->vv_throwpoint	= v_throwpoint(ശൂന്യ);

    // Necessary for debugging an inactive ":catch", ":finally", ":endtry"
    dsp->did_emsg	= did_emsg;		did_emsg     = FALSE;
    dsp->got_int	= got_int;		got_int	     = FALSE;
    dsp->did_throw	= did_throw;		did_throw    = FALSE;
    dsp->need_rethrow	= need_rethrow;		need_rethrow = FALSE;
    dsp->check_cstack	= check_cstack;		check_cstack = FALSE;
    dsp->current_exception = current_exception;	current_exception = ശൂന്യ;
}

    സ്ഥിതം ശൂന്യം
restore_dbg_stuff(ഘടന dbg_stuff *dsp)
{
    suppress_errthrow = FALSE;
    trylevel = dsp->trylevel;
    force_abort = dsp->force_abort;
    caught_അട്ടി = dsp->caught_അട്ടി;
    (ശൂന്യം)v_exception(dsp->vv_exception);
    (ശൂന്യം)v_throwpoint(dsp->vv_throwpoint);
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_other_file(പൂർണ്ണസംഖ്യ fnum, char_u *ffname)
{
    എങ്കിൽ (fnum != 0)
    {
	എങ്കിൽ (fnum == curbuf->b_fnum)
	    തിരികെ FALSE;

	തിരികെ TRUE;
    }

    എങ്കിൽ (ffname == ശൂന്യ)
	തിരികെ TRUE;

    എങ്കിൽ (*ffname == NUL)
	തിരികെ FALSE;

    // TODO: Need a reliable way to know whether a buffer is meant to live
    // on-disk !curbuf->b_dev_valid is not always available (example: missing
    // on Windows)
    എങ്കിൽ (curbuf->b_sfname != ശൂന്യ
	    && *curbuf->b_sfname != NUL)
	// This occurs with unsaved buffers. In which case `ffname` actually
	// corresponds to curbuf->b_sfname
	തിരികെ fnamecmp(ffname, curbuf->b_sfname) != 0;

    തിരികെ otherfile(ffname);
}

/*
 * do_exmode(): Repeatedly get commands for the "Ex" mode, until the ":vi"
 * command is given.
 */
    ശൂന്യം
do_exmode(
    പൂർണ്ണസംഖ്യ		improved)	    // TRUE for "improved Ex" mode
{
    പൂർണ്ണസംഖ്യ		save_സന്ദേശം_scroll;
    പൂർണ്ണസംഖ്യ		prev_സന്ദേശം_row;
    linenr_T	prev_വരി;
    varnumber_T	changedtick;

    എങ്കിൽ (improved)
	exmode_active = EXMODE_VIM;
    അല്ലെങ്കിൽ
	exmode_active = EXMODE_NORMAL;
    State = MODE_NORMAL;
    may_trigger_modechanged();

    // When using ":global /pat/ visual" and then "Q" we return to continue
    // the :global command.
    എങ്കിൽ (global_busy)
	തിരികെ;

    save_സന്ദേശം_scroll = സന്ദേശം_scroll;
    ++RedrawingDisabled;	    // don't redisplay the window
    ++no_wait_return;		    // don't wait for return
#ifdef FEAT_GUI
    // Ignore scrollbar and mouse events in Ex mode
    ++hold_gui_events;
#endif

    സന്ദേശം(_("Entering Ex mode.  Type \"visual\" to go to Normal mode."));
    ആകുമ്പോൾ (exmode_active)
    {
	// Check for a ":normal" command and no more characters left.
	എങ്കിൽ (ex_normal_busy > 0 && typebuf.tb_len == 0)
	{
	    exmode_active = FALSE;
	    നിർത്തുക;
	}
	സന്ദേശം_scroll = TRUE;
	need_wait_return = FALSE;
	ex_pressedreturn = FALSE;
	ex_no_reprint = FALSE;
	changedtick = CHANGEDTICK(curbuf);
	prev_സന്ദേശം_row = സന്ദേശം_row;
	prev_വരി = curwin->w_cursor.lnum;
	എങ്കിൽ (improved)
	{
	    cmdline_row = സന്ദേശം_row;
	    do_cmdline(ശൂന്യ, getexline, ശൂന്യ, 0);
	}
	അല്ലെങ്കിൽ
	    do_cmdline(ശൂന്യ, getexmodeline, ശൂന്യ, DOCMD_NOWAIT);
	വരികൾ_ഇടത് = Rows - 1;

	എങ്കിൽ ((prev_വരി != curwin->w_cursor.lnum
		   || changedtick != CHANGEDTICK(curbuf)) && !ex_no_reprint)
	{
	    എങ്കിൽ (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (ex_pressedreturn)
		{
		    // go up one line, to overwrite the ":<CR>" line, so the
		    // output doesn't contain empty lines.
		    സന്ദേശം_row = prev_സന്ദേശം_row;
		    എങ്കിൽ (prev_സന്ദേശം_row == Rows - 1)
			സന്ദേശം_row--;
		}
		സന്ദേശം_col = 0;
		print_വരി_no_prefix(curwin->w_cursor.lnum, FALSE, FALSE);
		സന്ദേശം_clr_eos();
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (ex_pressedreturn && !ex_no_reprint)	// must be at EOF
	{
	    എങ്കിൽ (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    അല്ലെങ്കിൽ
		emsg(_(e_at_end_of_file));
	}
    }

#ifdef FEAT_GUI
    --hold_gui_events;
#endif
    എങ്കിൽ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    update_screen(UPD_CLEAR);
    need_wait_return = FALSE;
    സന്ദേശം_scroll = save_സന്ദേശം_scroll;
}

/*
 * Print the executed command for when 'verbose' is set.
 * When "lnum" is 0 only print the command.
 */
    സ്ഥിതം ശൂന്യം
സന്ദേശം_verbose_cmd(linenr_T lnum, char_u *cmd)
{
    ++no_wait_return;
    verbose_enter_scroll();

    എങ്കിൽ (lnum == 0)
	smsg(_("Executing: %s"), cmd);
    അല്ലെങ്കിൽ
	smsg(_("line %ld: %s"), (long)lnum, cmd);
    എങ്കിൽ (സന്ദേശം_silent == 0)
	സന്ദേശം_puts("\n");   // don't overwrite this

    verbose_leave_scroll();
    --no_wait_return;
}

/*
 * Execute a simple command line.  Used for translated commands like "*".
 */
    പൂർണ്ണസംഖ്യ
do_cmdline_cmd(char_u *cmd)
{
    തിരികെ do_cmdline(cmd, ശൂന്യ, ശൂന്യ,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
}

/*
 * Execute the "+cmd" argument of "edit +cmd fname" and the like.
 * This allows for using a range without ":" in Vim9 script.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
do_cmd_argument(char_u *cmd)
{
    തിരികെ do_cmdline(cmd, ശൂന്യ, ശൂന്യ,
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
    പൂർണ്ണസംഖ്യ
do_cmdline(
    char_u	*cmdline,
    char_u	*(*fgetline)(പൂർണ്ണസംഖ്യ, ശൂന്യം *, പൂർണ്ണസംഖ്യ, getline_opt_T),
    ശൂന്യം	*cookie,		// argument for fgetline()
    പൂർണ്ണസംഖ്യ		flags)
{
    char_u	*next_cmdline;		// next cmd to execute
    char_u	*cmdline_copy = ശൂന്യ;	// copy of cmd line
    പൂർണ്ണസംഖ്യ		used_getline = FALSE;	// used "fgetline" to obtain command
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recursive = 0;		// recursive depth
    പൂർണ്ണസംഖ്യ		സന്ദേശം_didout_before_start = 0;
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;		// line number count
    പൂർണ്ണസംഖ്യ		did_inc_RedrawingDisabled = FALSE;
    പൂർണ്ണസംഖ്യ		retval = OK;
#ifdef FEAT_EVAL
    cstack_T	cstack;			// conditional stack
    garray_T	വരികൾ_ga;		// keep lines for ":while"/":for"
    പൂർണ്ണസംഖ്യ		current_വരി = 0;	// active line in lines_ga
    പൂർണ്ണസംഖ്യ		current_വരി_before = 0;
    char_u	*fname = ശൂന്യ;		// function or script name
    linenr_T	*breakpoint = ശൂന്യ;	// ptr to breakpoint field in cookie
    പൂർണ്ണസംഖ്യ		*dbg_tick = ശൂന്യ;	// ptr to dbg_tick field in cookie
    ഘടന dbg_stuff debug_saved;	// saved things for debug mode
    പൂർണ്ണസംഖ്യ		initial_trylevel;
    msglist_T	**saved_സന്ദേശം_list = ശൂന്യ;
    msglist_T	*private_സന്ദേശം_list = ശൂന്യ;

    // "fgetline" and "cookie" passed to do_one_cmd()
    char_u	*(*cmd_getline)(പൂർണ്ണസംഖ്യ, ശൂന്യം *, പൂർണ്ണസംഖ്യ, getline_opt_T);
    ശൂന്യം	*cmd_cookie;
    ഘടന loop_cookie cmd_loop_cookie;
    ശൂന്യം	*real_cookie;
    പൂർണ്ണസംഖ്യ		getline_is_func;
#അല്ലെങ്കിൽ
# define cmd_getline fgetline
# define cmd_cookie cookie
#endif
    സ്ഥിതം പൂർണ്ണസംഖ്യ	call_depth = 0;		// recursiveness
#ifdef FEAT_EVAL
    // For every pair of do_cmdline()/do_one_cmd() calls, use an extra memory
    // location for storing error messages to be converted to an exception.
    // This ensures that the do_errthrow() call in do_one_cmd() does not
    // combine the messages stored by an earlier invocation of do_one_cmd()
    // with the command name of the later one.  This would happen when
    // BufWritePost autocommands are executed after a write error.
    saved_സന്ദേശം_list = സന്ദേശം_list;
    സന്ദേശം_list = &private_സന്ദേശം_list;
#endif

    // It's possible to create an endless loop with ":execute", catch that
    // here.  The value of 200 allows nested function calls, ":source", etc.
    // Allow 200 or 'maxfuncdepth', whatever is larger.
    എങ്കിൽ (call_depth >= 200
#ifdef FEAT_EVAL
	    && call_depth >= p_mfd
#endif
	    )
    {
	emsg(_(e_command_too_recursive));
#ifdef FEAT_EVAL
	// When converting to an exception, we do not include the command name
	// since this is not an error of the specific command.
	do_errthrow((cstack_T *)ശൂന്യ, (char_u *)ശൂന്യ);
	സന്ദേശം_list = saved_സന്ദേശം_list;
#endif
	തിരികെ FAIL;
    }
    ++call_depth;

#ifdef FEAT_EVAL
    CLEAR_FIELD(cstack);
    cstack.cs_idx = -1;
    ga_init2(&വരികൾ_ga, വലുപ്പം(wcmd_T), 10);

    real_cookie = getline_cookie(fgetline, cookie);

    // Inside a function use a higher nesting level.
    getline_is_func = getline_equal(fgetline, cookie, get_func_വരി);
    എങ്കിൽ (getline_is_func && ex_nesting_level == func_level(real_cookie))
	++ex_nesting_level;

    // Get the function or script name and the address where the next breakpoint
    // line and the debug tick for a function or script are stored.
    എങ്കിൽ (getline_is_func)
    {
	fname = func_name(real_cookie);
	breakpoint = func_breakpoint(real_cookie);
	dbg_tick = func_dbg_tick(real_cookie);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (getline_equal(fgetline, cookie, getsourceline))
    {
	fname = SOURCING_NAME;
	breakpoint = source_breakpoint(real_cookie);
	dbg_tick = source_dbg_tick(real_cookie);
    }

    /*
     * Initialize "force_abort"  and "suppress_errthrow" at the top level.
     */
    എങ്കിൽ (!recursive)
    {
	force_abort = FALSE;
	suppress_errthrow = FALSE;
    }

    /*
     * If requested, store and reset the global values controlling the
     * exception handling (used when debugging).  Otherwise clear it to avoid
     * a bogus compiler warning when the optimizer uses inline functions...
     */
    എങ്കിൽ (flags & DOCMD_EXCRESET)
	save_dbg_stuff(&debug_saved);
    അല്ലെങ്കിൽ
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
    എങ്കിൽ (!(flags & DOCMD_KEYTYPED)
			       && !getline_equal(fgetline, cookie, getexline))
	താക്കോൽ_Typed = FALSE;

    /*
     * Continue executing command lines:
     * - when inside an ":if", ":while" or ":for"
     * - for multiple commands on one line, separated with '|'
     * - when repeating until there are no more lines (for ":source")
     */
    next_cmdline = cmdline;
    ചെയ്യുക
    {
#ifdef FEAT_EVAL
	getline_is_func = getline_equal(fgetline, cookie, get_func_വരി);
#endif

	// stop skipping cmds for an error msg after all endif/while/for
	എങ്കിൽ (next_cmdline == ശൂന്യ
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
	എങ്കിൽ (cstack.cs_looplevel > 0 && current_വരി < വരികൾ_ga.ga_len)
	{
	    // Each '|' separated command is stored separately in lines_ga, to
	    // be able to jump to it.  Don't use next_cmdline now.
	    VIM_CLEAR(cmdline_copy);

	    // Check if a function has returned or, unless it has an unclosed
	    // try conditional, aborted.
	    എങ്കിൽ (getline_is_func)
	    {
# ifdef FEAT_PROFILE
		എങ്കിൽ (do_profiling == PROF_YES)
		    func_വരി_end(real_cookie);
# endif
		എങ്കിൽ (func_has_ended(real_cookie))
		{
		    retval = FAIL;
		    നിർത്തുക;
		}
	    }
# ifdef FEAT_PROFILE
	    അല്ലെങ്കിൽ എങ്കിൽ (do_profiling == PROF_YES
			    && getline_equal(fgetline, cookie, getsourceline))
		script_വരി_end();
# endif

	    // Check if a sourced file hit a ":finish" command.
	    എങ്കിൽ (source_finished(fgetline, cookie))
	    {
		retval = FAIL;
		നിർത്തുക;
	    }

	    // If breakpoints have been added/deleted need to check for it.
	    എങ്കിൽ (breakpoint != ശൂന്യ && dbg_tick != ശൂന്യ
						   && *dbg_tick != debug_tick)
	    {
		*breakpoint = dbg_find_breakpoint(
				getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }

	    next_cmdline = ((wcmd_T *)(വരികൾ_ga.ga_data))[current_വരി].വരി;
	    SOURCING_LNUM = ((wcmd_T *)(വരികൾ_ga.ga_data))[current_വരി].lnum;

	    // Did we encounter a breakpoint?
	    എങ്കിൽ (breakpoint != ശൂന്യ && *breakpoint != 0
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
	    എങ്കിൽ (do_profiling == PROF_YES)
	    {
		എങ്കിൽ (getline_is_func)
		    func_വരി_start(real_cookie, SOURCING_LNUM);
		അല്ലെങ്കിൽ എങ്കിൽ (getline_equal(fgetline, cookie, getsourceline))
		    script_വരി_start();
	    }
# endif
	}
#endif

	// 2. If no line given, get an allocated line with fgetline().
	എങ്കിൽ (next_cmdline == ശൂന്യ)
	{
	    /*
	     * Need to set msg_didout for the first line after an ":if",
	     * otherwise the ":if" will be overwritten.
	     */
	    എങ്കിൽ (എണ്ണം == 1 && getline_equal(fgetline, cookie, getexline))
		സന്ദേശം_didout = TRUE;
	    എങ്കിൽ (fgetline == ശൂന്യ || (next_cmdline = fgetline(':', cookie,
#ifdef FEAT_EVAL
		    cstack.cs_idx < 0 ? 0 : (cstack.cs_idx + 1) * 2
#അല്ലെങ്കിൽ
		    0
#endif
		    , in_vim9script() ? GETLINE_CONCAT_CONTBAR
					       : GETLINE_CONCAT_CONT)) == ശൂന്യ)
	    {
		// Don't call wait_return() for aborted command line.  The NULL
		// returned for the end of a sourced file or executed function
		// doesn't do this.
		എങ്കിൽ (താക്കോൽ_Typed && !(flags & DOCMD_REPEAT))
		    need_wait_return = FALSE;
		retval = FAIL;
		നിർത്തുക;
	    }
	    used_getline = TRUE;

	    /*
	     * Keep the first typed line.  Clear it when more lines are typed.
	     */
	    എങ്കിൽ (flags & DOCMD_KEEPLINE)
	    {
		vim_free(repeat_cmdline);
		എങ്കിൽ (എണ്ണം == 0)
		    repeat_cmdline = vim_strsave(next_cmdline);
		അല്ലെങ്കിൽ
		    repeat_cmdline = ശൂന്യ;
	    }
	}

	// 3. Make a copy of the command so we can mess with it.
	അല്ലെങ്കിൽ എങ്കിൽ (cmdline_copy == ശൂന്യ)
	{
	    next_cmdline = vim_strsave(next_cmdline);
	    എങ്കിൽ (next_cmdline == ശൂന്യ)
	    {
		emsg(_(e_out_of_memory));
		retval = FAIL;
		നിർത്തുക;
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
	എങ്കിൽ ((cstack.cs_looplevel > 0 || has_loop_cmd(next_cmdline)))
	{
	    cmd_getline = get_loop_വരി;
	    cmd_cookie = (ശൂന്യം *)&cmd_loop_cookie;
	    cmd_loop_cookie.വരികൾ_gap = &വരികൾ_ga;
	    cmd_loop_cookie.current_വരി = current_വരി;
	    cmd_loop_cookie.lc_getline = fgetline;
	    cmd_loop_cookie.cookie = cookie;
	    cmd_loop_cookie.repeating = (current_വരി < വരികൾ_ga.ga_len);

	    // Save the current line when encountering it the first time.
	    എങ്കിൽ (current_വരി == വരികൾ_ga.ga_len
		    && store_loop_വരി(&വരികൾ_ga, next_cmdline) == FAIL)
	    {
		retval = FAIL;
		നിർത്തുക;
	    }
	    current_വരി_before = current_വരി;
	}
	അല്ലെങ്കിൽ
	{
	    cmd_getline = fgetline;
	    cmd_cookie = cookie;
	}

	did_endif = FALSE;
#endif

	എങ്കിൽ (എണ്ണം++ == 0)
	{
	    /*
	     * All output from the commands is put below each other, without
	     * waiting for a return. Don't do this when executing commands
	     * from a script or when being called recursive (e.g. for ":e
	     * +command file").
	     */
	    എങ്കിൽ (!(flags & DOCMD_NOWAIT) && !recursive)
	    {
		സന്ദേശം_didout_before_start = സന്ദേശം_didout;
		സന്ദേശം_didany = FALSE; // no output yet
		സന്ദേശം_start();
		സന്ദേശം_scroll = TRUE;  // put messages below each other
		++no_wait_return;   // don't wait for return until finished
		++RedrawingDisabled;
		did_inc_RedrawingDisabled = TRUE;
	    }
	}

	എങ്കിൽ ((p_verbose >= 15 && SOURCING_NAME != ശൂന്യ) || p_verbose >= 16)
	    സന്ദേശം_verbose_cmd(SOURCING_LNUM, cmdline_copy);

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
	എങ്കിൽ (cmd_cookie == (ശൂന്യം *)&cmd_loop_cookie)
	    // Use "current_line" from "cmd_loop_cookie", it may have been
	    // incremented when defining a function.
	    current_വരി = cmd_loop_cookie.current_വരി;
#endif

	എങ്കിൽ (next_cmdline == ശൂന്യ)
	{
	    VIM_CLEAR(cmdline_copy);

	    /*
	     * If the command was typed, remember it for the ':' register.
	     * Do this AFTER executing the command to make :@: work.
	     */
	    എങ്കിൽ (getline_equal(fgetline, cookie, getexline)
						  && new_last_cmdline != ശൂന്യ)
	    {
		vim_free(last_cmdline);
		last_cmdline = new_last_cmdline;
		new_last_cmdline = ശൂന്യ;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    // need to copy the command after the '|' to cmdline_copy, for the
	    // next do_one_cmd()
	    STRMOVE(cmdline_copy, next_cmdline);
	    next_cmdline = cmdline_copy;
	}


#ifdef FEAT_EVAL
	// reset did_emsg for a function that is not aborted by an error
	എങ്കിൽ (did_emsg && !force_abort
		&& getline_equal(fgetline, cookie, get_func_വരി)
					      && !func_has_abort(real_cookie))
	{
	    // did_emsg_cumul is not set here
	    did_emsg = FALSE;
	}

	എങ്കിൽ (cstack.cs_looplevel > 0)
	{
	    ++current_വരി;

	    /*
	     * An ":endwhile", ":endfor" and ":continue" is handled here.
	     * If we were executing commands, jump back to the ":while" or
	     * ":for".
	     * If we were not executing commands, decrement cs_looplevel.
	     */
	    എങ്കിൽ (cstack.cs_lflags & (CSL_HAD_CONT | CSL_HAD_ENDLOOP))
	    {
		cstack.cs_lflags &= ~(CSL_HAD_CONT | CSL_HAD_ENDLOOP);

		// Jump back to the matching ":while" or ":for".  Be careful
		// not to use a cs_line[] from an entry that isn't a ":while"
		// or ":for": It would make "current_line" invalid and can
		// cause a crash.
		എങ്കിൽ (!did_emsg && !got_int && !did_throw
			&& cstack.cs_idx >= 0
			&& (cstack.cs_flags[cstack.cs_idx]
						      & (CSF_WHILE | CSF_FOR))
			&& cstack.cs_വരി[cstack.cs_idx] >= 0
			&& (cstack.cs_flags[cstack.cs_idx] & CSF_ACTIVE))
		{
		    current_വരി = cstack.cs_വരി[cstack.cs_idx];
						// remember we jumped there
		    cstack.cs_lflags |= CSL_HAD_LOOP;
		    വരി_breakcheck();		// check if CTRL-C typed

		    // Check for the next breakpoint at or after the ":while"
		    // or ":for".
		    എങ്കിൽ (breakpoint != ശൂന്യ && വരികൾ_ga.ga_len > current_വരി)
		    {
			*breakpoint = dbg_find_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
									fname,
			   ((wcmd_T *)വരികൾ_ga.ga_data)[current_വരി].lnum-1);
			*dbg_tick = debug_tick;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    // can only get here with ":endwhile" or ":endfor"
		    എങ്കിൽ (cstack.cs_idx >= 0)
			rewind_conditionals(&cstack, cstack.cs_idx - 1,
				   CSF_WHILE | CSF_FOR, &cstack.cs_looplevel);
		}
	    }

	    /*
	     * For a ":while" or ":for" we need to remember the line number.
	     */
	    അല്ലെങ്കിൽ എങ്കിൽ (cstack.cs_lflags & CSL_HAD_LOOP)
	    {
		cstack.cs_lflags &= ~CSL_HAD_LOOP;
		cstack.cs_വരി[cstack.cs_idx] = current_വരി_before;
	    }
	}

	// Check for the next breakpoint after a watchexpression
	എങ്കിൽ (breakpoint != ശൂന്യ && has_watchexpr())
	{
	    *breakpoint = dbg_find_breakpoint(FALSE, fname, SOURCING_LNUM);
	    *dbg_tick = debug_tick;
	}

	/*
	 * When not inside any ":while" loop, clear remembered lines.
	 */
	എങ്കിൽ (cstack.cs_looplevel == 0)
	{
	    എങ്കിൽ (വരികൾ_ga.ga_len > 0)
	    {
		SOURCING_LNUM =
		       ((wcmd_T *)വരികൾ_ga.ga_data)[വരികൾ_ga.ga_len - 1].lnum;
		free_cmdlines(&വരികൾ_ga);
	    }
	    current_വരി = 0;
	}

	/*
	 * A ":finally" makes did_emsg, got_int, and did_throw pending for
	 * being restored at the ":endtry".  Reset them here and set the
	 * ACTIVE and FINALLY flags, so that the finally clause gets executed.
	 * This includes the case where a missing ":endif", ":endwhile" or
	 * ":endfor" was detected by the ":finally" itself.
	 */
	എങ്കിൽ (cstack.cs_lflags & CSL_HAD_FINA)
	{
	    cstack.cs_lflags &= ~CSL_HAD_FINA;
	    report_make_pending(cstack.cs_pending[cstack.cs_idx]
		    & (CSTP_ERROR | CSTP_INTERRUPT | CSTP_THROW),
		    did_throw ? (ശൂന്യം *)current_exception : ശൂന്യ);
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
	എങ്കിൽ (trylevel == 0 && !did_emsg && !got_int && !did_throw)
	    force_abort = FALSE;

	// Convert an interrupt to an exception if appropriate.
	(ശൂന്യം)do_intthrow(&cstack);
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
    ആകുമ്പോൾ (!((got_int
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
	    && (next_cmdline != ശൂന്യ
#ifdef FEAT_EVAL
			|| cstack.cs_idx >= 0
#endif
			|| (flags & DOCMD_REPEAT)));

    vim_free(cmdline_copy);
    did_emsg_syntax = FALSE;
#ifdef FEAT_EVAL
    free_cmdlines(&വരികൾ_ga);
    ga_clear(&വരികൾ_ga);

    എങ്കിൽ (cstack.cs_idx >= 0)
    {
	/*
	 * If a sourced file or executed function ran to its end, report the
	 * unclosed conditional.
	 * In Vim9 script do not give a second error, executing aborts after
	 * the first one.
	 */
	എങ്കിൽ (!got_int && !did_throw && !aborting()
		&& !(did_emsg && in_vim9script())
		&& ((getline_equal(fgetline, cookie, getsourceline)
			&& !source_finished(fgetline, cookie))
		    || (getline_equal(fgetline, cookie, get_func_വരി)
					    && !func_has_ended(real_cookie))))
	{
	    എങ്കിൽ (cstack.cs_flags[cstack.cs_idx] & CSF_TRY)
		emsg(_(e_missing_endtry));
	    അല്ലെങ്കിൽ എങ്കിൽ (cstack.cs_flags[cstack.cs_idx] & CSF_WHILE)
		emsg(_(e_missing_endwhile));
	    അല്ലെങ്കിൽ എങ്കിൽ (cstack.cs_flags[cstack.cs_idx] & CSF_FOR)
		emsg(_(e_missing_endfor));
	    അല്ലെങ്കിൽ
		emsg(_(e_missing_endif));
	}

	/*
	 * Reset "trylevel" in case of a ":finish" or ":return" or a missing
	 * ":endtry" in a sourced file or executed function.  If the try
	 * conditional is in its finally clause, ignore anything pending.
	 * If it is in a catch clause, finish the caught exception.
	 * Also cleanup any "cs_forinfo" structures.
	 */
	ചെയ്യുക
	{
	    പൂർണ്ണസംഖ്യ idx = cleanup_conditionals(&cstack, 0, TRUE);

	    എങ്കിൽ (idx >= 0)
		--idx;	    // remove try block not in its finally clause
	    rewind_conditionals(&cstack, idx, CSF_WHILE | CSF_FOR,
							&cstack.cs_looplevel);
	}
	ആകുമ്പോൾ (cstack.cs_idx >= 0);
	trylevel = initial_trylevel;
    }

    // If a missing ":endtry", ":endwhile", ":endfor", or ":endif" or a memory
    // lack was reported above and the error message is to be converted to an
    // exception, do this now after rewinding the cstack.
    do_errthrow(&cstack, getline_equal(fgetline, cookie, get_func_വരി)
				  ? (char_u *)"endfunction" : (char_u *)ശൂന്യ);

    എങ്കിൽ (trylevel == 0)
    {
	// Just in case did_throw got set but current_exception wasn't.
	എങ്കിൽ (current_exception == ശൂന്യ)
	    did_throw = FALSE;

	/*
	 * When an exception is being thrown out of the outermost try
	 * conditional, discard the uncaught exception, disable the conversion
	 * of interrupts or errors to exceptions, and ensure that no more
	 * commands are executed.
	 */
	എങ്കിൽ (did_throw)
	    handle_did_throw();

	/*
	 * On an interrupt or an aborting error not converted to an exception,
	 * disable the conversion of errors to exceptions.  (Interrupts are not
	 * converted anymore, here.) This enables also the interrupt message
	 * when force_abort is set and did_emsg unset in case of an interrupt
	 * from a finally clause after an error.
	 */
	അല്ലെങ്കിൽ എങ്കിൽ (got_int || (did_emsg && force_abort))
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
    എങ്കിൽ (did_throw)
	need_rethrow = TRUE;
    എങ്കിൽ ((getline_equal(fgetline, cookie, getsourceline)
		&& ex_nesting_level > source_level(real_cookie))
	    || (getline_equal(fgetline, cookie, get_func_വരി)
		&& ex_nesting_level > func_level(real_cookie) + 1))
    {
	എങ്കിൽ (!did_throw)
	    check_cstack = TRUE;
    }
    അല്ലെങ്കിൽ
    {
	// When leaving a function, reduce nesting level.
	എങ്കിൽ (getline_equal(fgetline, cookie, get_func_വരി))
	    --ex_nesting_level;
	/*
	 * Go to debug mode when returning from a function in which we are
	 * single-stepping.
	 */
	എങ്കിൽ ((getline_equal(fgetline, cookie, getsourceline)
		    || getline_equal(fgetline, cookie, get_func_വരി))
		&& ex_nesting_level + 1 <= debug_break_level)
	    do_debug(getline_equal(fgetline, cookie, getsourceline)
		    ? (char_u *)_("End of sourced file")
		    : (char_u *)_("End of function"));
    }

    /*
     * Restore the exception environment (done after returning from the
     * debugger).
     */
    എങ്കിൽ (flags & DOCMD_EXCRESET)
	restore_dbg_stuff(&debug_saved);

    സന്ദേശം_list = saved_സന്ദേശം_list;

    // Cleanup if "cs_emsg_silent_list" remains.
    എങ്കിൽ (cstack.cs_emsg_silent_list != ശൂന്യ)
    {
	eslist_T *elem, *temp;

	വേണ്ടി (elem = cstack.cs_emsg_silent_list; elem != ശൂന്യ; elem = temp)
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
    എങ്കിൽ (did_inc_RedrawingDisabled)
    {
	എങ്കിൽ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	--no_wait_return;
	സന്ദേശം_scroll = FALSE;

	/*
	 * When just finished an ":if"-":else" which was typed, no need to
	 * wait for hit-return.  Also for an error situation.
	 */
	എങ്കിൽ (retval == FAIL
#ifdef FEAT_EVAL
		|| (did_endif && താക്കോൽ_Typed && !did_emsg)
#endif
					    )
	{
	    need_wait_return = FALSE;
	    സന്ദേശം_didany = FALSE;		// don't wait when restarting edit
	}
	അല്ലെങ്കിൽ എങ്കിൽ (need_wait_return)
	{
	    /*
	     * The msg_start() above clears msg_didout. The wait_return() we do
	     * here should not overwrite the command that may be shown before
	     * doing that.
	     */
	    സന്ദേശം_didout |= സന്ദേശം_didout_before_start;
	    wait_return(FALSE);
	}
    }

#ifdef FEAT_EVAL
    did_endif = FALSE;  // in case do_cmdline used recursively
#അല്ലെങ്കിൽ
    /*
     * Reset if_level, in case a sourced script file contains more ":if" than
     * ":endif" (could be ":if x | foo | endif").
     */
    if_level = 0;
#endif

    --call_depth;
    തിരികെ retval;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Handle when "did_throw" is set after executing commands.
 */
    ശൂന്യം
handle_did_throw(ശൂന്യം)
{
    അക്ഷരം	*p = ശൂന്യ;
    msglist_T	*messages = ശൂന്യ;
    ESTACK_CHECK_DECLARATION;

    /*
     * If the uncaught exception is a user exception, report it as an
     * error.  If it is an error exception, display the saved error
     * message now.  For an interrupt exception, do nothing; the
     * interrupt message is given elsewhere.
     */
    തിരഞ്ഞെടുപ്പ് (current_exception->type)
    {
	അവസ്ഥ ET_USER:
	    vim_snprintf((അക്ഷരം *)IObuff, IOSIZE,
		    _(e_exception_not_caught_str),
		    current_exception->മൂല്യം);
	    p = (അക്ഷരം *)vim_strsave(IObuff);
	    നിർത്തുക;
	അവസ്ഥ ET_ERROR:
	    messages = current_exception->messages;
	    current_exception->messages = ശൂന്യ;
	    നിർത്തുക;
	അവസ്ഥ ET_INTERRUPT:
	    നിർത്തുക;
    }

    estack_തള്ളുക(ETYPE_EXCEPT, current_exception->throw_name,
					current_exception->throw_lnum);
    ESTACK_CHECK_SETUP;
    current_exception->throw_name = ശൂന്യ;

    discard_current_exception();	// uses IObuff if 'verbose'

    // If "silent!" is active the uncaught exception is not fatal.
    എങ്കിൽ (emsg_silent == 0)
    {
	suppress_errthrow = TRUE;
	force_abort = TRUE;
    }

    എങ്കിൽ (messages != ശൂന്യ)
    {
	ചെയ്യുക
	{
	    msglist_T	*next = messages->next;
	    പൂർണ്ണസംഖ്യ		save_compiling = estack_compiling;

	    estack_compiling = messages->സന്ദേശം_compiling;
	    emsg(messages->സന്ദേശം);
	    vim_free(messages->സന്ദേശം);
	    vim_free(messages->sfile);
	    vim_free(messages);
	    messages = next;
	    estack_compiling = save_compiling;
	}
	ആകുമ്പോൾ (messages != ശൂന്യ);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (p != ശൂന്യ)
    {
	emsg(p);
	vim_free(p);
    }
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_എടുക്കുക();
}

/*
 * Obtain a line when inside a ":while" or ":for" loop.
 */
    സ്ഥിതം char_u *
get_loop_വരി(പൂർണ്ണസംഖ്യ c, ശൂന്യം *cookie, പൂർണ്ണസംഖ്യ indent, getline_opt_T options)
{
    ഘടന loop_cookie	*cp = (ഘടന loop_cookie *)cookie;
    wcmd_T		*wp;
    char_u		*വരി;

    എങ്കിൽ (cp->current_വരി + 1 >= cp->വരികൾ_gap->ga_len)
    {
	എങ്കിൽ (cp->repeating)
	    തിരികെ ശൂന്യ;	// trying to read past ":endwhile"/":endfor"

	// First time inside the ":while"/":for": get line normally.
	എങ്കിൽ (cp->lc_getline == ശൂന്യ)
	    വരി = getcmdline(c, 0L, indent, 0);
	അല്ലെങ്കിൽ
	    വരി = cp->lc_getline(c, cp->cookie, indent, options);
	എങ്കിൽ (വരി != ശൂന്യ && store_loop_വരി(cp->വരികൾ_gap, വരി) == OK)
	    ++cp->current_വരി;

	തിരികെ വരി;
    }

    താക്കോൽ_Typed = FALSE;
    ++cp->current_വരി;
    wp = (wcmd_T *)(cp->വരികൾ_gap->ga_data) + cp->current_വരി;
    SOURCING_LNUM = wp->lnum;
    തിരികെ vim_strsave(wp->വരി);
}

/*
 * Store a line in "gap" so that a ":while" loop can execute it again.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
store_loop_വരി(garray_T *gap, char_u *വരി)
{
    എങ്കിൽ (ga_grow(gap, 1) == FAIL)
	തിരികെ FAIL;
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].വരി = vim_strsave(വരി);
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].lnum = SOURCING_LNUM;
    ++gap->ga_len;
    തിരികെ OK;
}

/*
 * Free the lines stored for a ":while" or ":for" loop.
 */
    സ്ഥിതം ശൂന്യം
free_cmdlines(garray_T *gap)
{
    ആകുമ്പോൾ (gap->ga_len > 0)
    {
	vim_free(((wcmd_T *)(gap->ga_data))[gap->ga_len - 1].വരി);
	--gap->ga_len;
    }
}
#endif

/*
 * If "fgetline" is get_loop_line(), return TRUE if the getline it uses equals
 * "func".  * Otherwise return TRUE when "fgetline" equals "func".
 */
    പൂർണ്ണസംഖ്യ
getline_equal(
    char_u	*(*fgetline)(പൂർണ്ണസംഖ്യ, ശൂന്യം *, പൂർണ്ണസംഖ്യ, getline_opt_T),
    ശൂന്യം	*cookie UNUSED,		// argument for fgetline()
    char_u	*(*func)(പൂർണ്ണസംഖ്യ, ശൂന്യം *, പൂർണ്ണസംഖ്യ, getline_opt_T))
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(പൂർണ്ണസംഖ്യ, ശൂന്യം *, പൂർണ്ണസംഖ്യ, getline_opt_T);
    ഘടന loop_cookie *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // function that's originally used to obtain the lines.  This may be
    // nested several levels.
    gp = fgetline;
    cp = (ഘടന loop_cookie *)cookie;
    ആകുമ്പോൾ (gp == get_loop_വരി)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    തിരികെ gp == func;
#അല്ലെങ്കിൽ
    തിരികെ fgetline == func;
#endif
}

/*
 * If "fgetline" is get_loop_line(), return the cookie used by the original
 * getline function.  Otherwise return "cookie".
 */
    ശൂന്യം *
getline_cookie(
    char_u	*(*fgetline)(പൂർണ്ണസംഖ്യ, ശൂന്യം *, പൂർണ്ണസംഖ്യ, getline_opt_T) UNUSED,
    ശൂന്യം	*cookie)		// argument for fgetline()
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(പൂർണ്ണസംഖ്യ, ശൂന്യം *, പൂർണ്ണസംഖ്യ, getline_opt_T);
    ഘടന loop_cookie  *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (ഘടന loop_cookie *)cookie;
    ആകുമ്പോൾ (gp == get_loop_വരി)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    തിരികെ cp;
#അല്ലെങ്കിൽ
    തിരികെ cookie;
#endif
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Get the next line source line without advancing.
 */
    char_u *
getline_peek(
    char_u	*(*fgetline)(പൂർണ്ണസംഖ്യ, ശൂന്യം *, പൂർണ്ണസംഖ്യ, getline_opt_T) UNUSED,
    ശൂന്യം	*cookie)		// argument for fgetline()
{
    char_u		*(*gp)(പൂർണ്ണസംഖ്യ, ശൂന്യം *, പൂർണ്ണസംഖ്യ, getline_opt_T);
    ഘടന loop_cookie  *cp;
    wcmd_T		*wp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (ഘടന loop_cookie *)cookie;
    ആകുമ്പോൾ (gp == get_loop_വരി)
    {
	എങ്കിൽ (cp->current_വരി + 1 < cp->വരികൾ_gap->ga_len)
	{
	    // executing lines a second time, use the stored copy
	    wp = (wcmd_T *)(cp->വരികൾ_gap->ga_data) + cp->current_വരി + 1;
	    തിരികെ wp->വരി;
	}
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    എങ്കിൽ (gp == getsourceline)
	തിരികെ source_nextline(cp);
    തിരികെ ശൂന്യ;
}
#endif


/*
 * Helper function to apply an offset for buffer commands, i.e. ":bdelete",
 * ":bwipeout", etc.
 * Returns the buffer number.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compute_buffer_local_എണ്ണം(പൂർണ്ണസംഖ്യ addr_type, പൂർണ്ണസംഖ്യ lnum, പൂർണ്ണസംഖ്യ offset)
{
    buf_T   *buf;
    buf_T   *nextbuf;
    പൂർണ്ണസംഖ്യ     എണ്ണം = offset;

    buf = firstbuf;
    ആകുമ്പോൾ (buf->b_next != ശൂന്യ && buf->b_fnum < lnum)
	buf = buf->b_next;
    ആകുമ്പോൾ (എണ്ണം != 0)
    {
	എണ്ണം += (offset < 0) ? 1 : -1;
	nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
	എങ്കിൽ (nextbuf == ശൂന്യ)
	    നിർത്തുക;
	buf = nextbuf;
	എങ്കിൽ (addr_type == ADDR_LOADED_BUFFERS)
	    // skip over unloaded buffers
	    ആകുമ്പോൾ (buf->b_ml.ml_mfp == ശൂന്യ)
	    {
		nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
		എങ്കിൽ (nextbuf == ശൂന്യ)
		    നിർത്തുക;
		buf = nextbuf;
	    }
    }
    // we might have gone too far, last buffer is not loadedd
    എങ്കിൽ (addr_type == ADDR_LOADED_BUFFERS)
	ആകുമ്പോൾ (buf->b_ml.ml_mfp == ശൂന്യ)
	{
	    nextbuf = (offset >= 0) ? buf->b_prev : buf->b_next;
	    എങ്കിൽ (nextbuf == ശൂന്യ)
		നിർത്തുക;
	    buf = nextbuf;
	}
    തിരികെ buf->b_fnum;
}

/*
 * Return the window number of "win".
 * When "win" is NULL return the number of windows.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
current_win_nr(win_T *win)
{
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		nr = 0;

    FOR_ALL_WINDOWS(wp)
    {
	++nr;
	എങ്കിൽ (wp == win)
	    നിർത്തുക;
    }
    തിരികെ nr;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
current_tab_nr(tabpage_T *tab)
{
    tabpage_T	*tp;
    പൂർണ്ണസംഖ്യ		nr = 0;

    FOR_ALL_TABPAGES(tp)
    {
	++nr;
	എങ്കിൽ (tp == tab)
	    നിർത്തുക;
    }
    തിരികെ nr;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
comment_start(char_u *p, പൂർണ്ണസംഖ്യ starts_with_colon UNUSED)
{
    എങ്കിൽ (in_vim9script())
	തിരികെ p[0] == '#' && !starts_with_colon;
    തിരികെ *p == '"';
}

#define CURRENT_WIN_NR current_win_nr(curwin)
#define LAST_WIN_NR current_win_nr(ശൂന്യ)
#define CURRENT_TAB_NR current_tab_nr(curtab)
#define LAST_TAB_NR current_tab_nr(ശൂന്യ)

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
    സ്ഥിതം char_u *
do_one_cmd(
    char_u	**cmdlinep,
    പൂർണ്ണസംഖ്യ		flags,
#ifdef FEAT_EVAL
    cstack_T	*cstack,
#endif
    char_u	*(*fgetline)(പൂർണ്ണസംഖ്യ, ശൂന്യം *, പൂർണ്ണസംഖ്യ, getline_opt_T),
    ശൂന്യം	*cookie)		// argument for fgetline()
{
    char_u	*p;
    linenr_T	lnum;
    long	n;
    അക്ഷരം	*errormsg = ശൂന്യ;	// error message
    char_u	*after_modifier = ശൂന്യ;
    exarg_T	ea;			// Ex command arguments
    cmdmod_T	save_cmdmod;
    പൂർണ്ണസംഖ്യ		save_reg_executing = reg_executing;
    പൂർണ്ണസംഖ്യ		save_pending_end_reg_executing = pending_end_reg_executing;
    പൂർണ്ണസംഖ്യ		ni;			// set when Not Implemented
    char_u	*cmd;
    പൂർണ്ണസംഖ്യ		starts_with_colon = FALSE;
    പൂർണ്ണസംഖ്യ		may_have_range;
#ifdef FEAT_EVAL
    പൂർണ്ണസംഖ്യ		did_set_വ്യഞ്ജകം_വരി = FALSE;
#endif
    പൂർണ്ണസംഖ്യ		sourcing = flags & DOCMD_VERBOSE;
    പൂർണ്ണസംഖ്യ		did_append_cmd = FALSE;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
#ifdef FEAT_EVAL
    ++ex_nesting_level;
#endif

    // When the last file has not been edited :q has to be typed twice.
    എങ്കിൽ (quitmore
#ifdef FEAT_EVAL
	    // avoid that a function call in 'statusline' does this
	    && !getline_equal(fgetline, cookie, get_func_വരി)
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
    എങ്കിൽ ((*cmdlinep)[0] == '#' && (*cmdlinep)[1] == '!')
	പോകുക doend;

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
    എങ്കിൽ (parse_command_modifiers(&ea, &errormsg, &cmdmod, FALSE) == FAIL)
	പോകുക doend;
    apply_cmdmod(&cmdmod);
    after_modifier = ea.cmd;

#ifdef FEAT_EVAL
    ea.skip = did_emsg || got_int || did_throw || (cstack->cs_idx >= 0
			 && !(cstack->cs_flags[cstack->cs_idx] & CSF_ACTIVE));
#അല്ലെങ്കിൽ
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
    പൂർണ്ണസംഖ്യ vim9script = in_vim9script();
    എങ്കിൽ (vim9script && (flags & DOCMD_RANGEOK) == 0)
    {
	may_have_range = FALSE;
	വേണ്ടി (p = ea.cmd; p >= *cmdlinep; --p)
	{
	    എങ്കിൽ (*p == ':')
		may_have_range = TRUE;
	    എങ്കിൽ (p < ea.cmd && !VIM_ISWHITE(*p))
		നിർത്തുക;
	}
    }
    അല്ലെങ്കിൽ
	may_have_range = TRUE;
    എങ്കിൽ (may_have_range)
	ea.cmd = skip_range(ea.cmd, TRUE, ശൂന്യ);

#ifdef FEAT_EVAL
    // Handle ":export" - it functions almost like a command modifier.
    // ":export var Name: type"
    // ":export def Name(..."
    // etc.
    എങ്കിൽ (vim9script && checkforcmd_noparen(&ea.cmd, "export", 6))
	is_export = TRUE;
#endif

    എങ്കിൽ (vim9script && !may_have_range)
    {
	എങ്കിൽ (ea.cmd == cmd + 1 && *cmd == '$')
	    // should be "$VAR = val"
	    --ea.cmd;
#ifdef FEAT_EVAL
	p = find_ex_command(&ea, ശൂന്യ, lookup_scriptitem, ശൂന്യ);
#അല്ലെങ്കിൽ
	p = find_ex_command(&ea, ശൂന്യ, ശൂന്യ, ശൂന്യ);
#endif
	എങ്കിൽ (ea.cmdidx == CMD_SIZE)
	{
	    char_u *ar = skip_range(ea.cmd, TRUE, ശൂന്യ);

	    // If a ':' before the range is missing, give a clearer error
	    // message.
	    എങ്കിൽ (ar > ea.cmd && !ea.skip)
	    {
		semsg(_(e_colon_required_before_range_str), ea.cmd);
		പോകുക doend;
	    }
	}
    }
    അല്ലെങ്കിൽ
	p = find_ex_command(&ea, ശൂന്യ, ശൂന്യ, ശൂന്യ);

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    // Count this line for profiling if skip is TRUE.
    എങ്കിൽ (do_profiling == PROF_YES
	    && (!ea.skip || cstack->cs_idx == 0 || (cstack->cs_idx > 0
		     && (cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE))))
    {
	പൂർണ്ണസംഖ്യ skip = did_emsg || got_int || did_throw;

	എങ്കിൽ (ea.cmdidx == CMD_catch)
	    skip = !skip && !(cstack->cs_idx >= 0
			  && (cstack->cs_flags[cstack->cs_idx] & CSF_THROWN)
			  && !(cstack->cs_flags[cstack->cs_idx] & CSF_CAUGHT));
	അല്ലെങ്കിൽ എങ്കിൽ (ea.cmdidx == CMD_else || ea.cmdidx == CMD_elseif)
	    skip = skip || !(cstack->cs_idx >= 0
			  && !(cstack->cs_flags[cstack->cs_idx]
						  & (CSF_ACTIVE | CSF_TRUE)));
	അല്ലെങ്കിൽ എങ്കിൽ (ea.cmdidx == CMD_finally)
	    skip = FALSE;
	അല്ലെങ്കിൽ എങ്കിൽ (ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endtry
		&& ea.cmdidx != CMD_endwhile)
	    skip = ea.skip;

	എങ്കിൽ (!skip)
	{
	    എങ്കിൽ (getline_equal(fgetline, cookie, get_func_വരി))
		func_വരി_exec(getline_cookie(fgetline, cookie));
	    അല്ലെങ്കിൽ എങ്കിൽ (getline_equal(fgetline, cookie, getsourceline))
		script_വരി_exec();
	}
    }
# endif
#endif

    ea.cmd = cmd;

#ifdef FEAT_EVAL
    // May go to debug mode.  If this happens and the ">quit" debug command is
    // used, throw an interrupt exception and skip the next command.
    dbg_check_breakpoint(&ea);
    എങ്കിൽ (!ea.skip && got_int)
    {
	ea.skip = TRUE;
	(ശൂന്യം)do_intthrow(cstack);
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
    എങ്കിൽ (!IS_USER_CMDIDX(ea.cmdidx))
    {
	എങ്കിൽ (ea.cmdidx != CMD_SIZE)
	    ea.addr_type = cmdnames[(പൂർണ്ണസംഖ്യ)ea.cmdidx].cmd_addr_type;
	അല്ലെങ്കിൽ
	    ea.addr_type = ADDR_വരികൾ;

	// :wincmd range depends on the argument.
	എങ്കിൽ (ea.cmdidx == CMD_wincmd && p != ശൂന്യ)
	    get_wincmd_addr_type(skipwhite(p), &ea);
#ifdef FEAT_QUICKFIX
	// :.cc in quickfix window uses line number
	എങ്കിൽ ((ea.cmdidx == CMD_cc || ea.cmdidx == CMD_ll) && bt_quickfix(curbuf))
	    ea.addr_type = ADDR_OTHER;
#endif
    }

    എങ്കിൽ (!may_have_range)
	ea.line1 = ea.line2 = default_address(&ea);
    അല്ലെങ്കിൽ എങ്കിൽ (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
	പോകുക doend;

/*
 * 5. Parse the command.
 */

    /*
     * Skip ':' and any white space
     */
    ea.cmd = skipwhite(ea.cmd);
    ആകുമ്പോൾ (*ea.cmd == ':')
	ea.cmd = skipwhite(ea.cmd + 1);

    /*
     * If we got a line, but no command, then go to the line.
     * If we find a '|' or '\n' we set ea.nextcmd.
     */
    എങ്കിൽ (*ea.cmd == NUL || comment_start(ea.cmd, starts_with_colon)
			       || (ea.nextcmd = check_nextcmd(ea.cmd)) != ശൂന്യ)
    {
	/*
	 * strange vi behaviour:
	 * ":3"		jumps to line 3
	 * ":3|..."	prints line 3  (not in Vim9 script)
	 * ":|"		prints current line  (not in Vim9 script)
	 */
	എങ്കിൽ (ea.skip)	    // skip this if inside :if
	    പോകുക doend;
	errormsg = ex_range_without_command(&ea);
	പോകുക doend;
    }

    // If this looks like an undefined user command and there are CmdUndefined
    // autocommands defined, trigger the matching autocommands.
    എങ്കിൽ (p != ശൂന്യ && ea.cmdidx == CMD_SIZE && !ea.skip
	    && ASCII_ISUPPER(*ea.cmd)
	    && has_cmdundefined())
    {
	പൂർണ്ണസംഖ്യ ret;

	p = ea.cmd;
	ആകുമ്പോൾ (ASCII_ISALNUM(*p))
	    ++p;
	p = vim_strnsave(ea.cmd, p - ea.cmd);
	ret = apply_autocmds(EVENT_CMDUNDEFINED, p, p, TRUE, ശൂന്യ);
	vim_free(p);
	// If the autocommands did something and didn't cause an error, try
	// finding the command again.
	p = (ret
#ifdef FEAT_EVAL
		&& !aborting()
#endif
		) ? find_ex_command(&ea, ശൂന്യ, ശൂന്യ, ശൂന്യ) : ea.cmd;
    }

    എങ്കിൽ (p == ശൂന്യ)
    {
	എങ്കിൽ (!ea.skip)
	    errormsg = _(e_ambiguous_use_of_user_defined_command);
	പോകുക doend;
    }
    // Check for wrong commands.
    എങ്കിൽ (*p == '!' && ea.cmd[1] == 0151 && ea.cmd[0] == 78
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	errormsg = uc_fun_cmd();
	പോകുക doend;
    }

    എങ്കിൽ (ea.cmdidx == CMD_SIZE)
    {
	എങ്കിൽ (!ea.skip)
	{
	    STRCPY(IObuff, _(e_not_an_editor_command));
	    എങ്കിൽ (!sourcing)
	    {
		// If the modifier was parsed OK the error must be in the
		// following command
		എങ്കിൽ (after_modifier != ശൂന്യ)
		    append_command(after_modifier);
		അല്ലെങ്കിൽ
		    append_command(*cmdlinep);
		did_append_cmd = TRUE;
	    }
	    errormsg = (അക്ഷരം *)IObuff;
	    did_emsg_syntax = TRUE;
	}
	പോകുക doend;
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
    എങ്കിൽ (ea.cmdidx == CMD_if)
	++if_level;
    എങ്കിൽ (if_level)
    {
	എങ്കിൽ (ea.cmdidx == CMD_endif)
	    --if_level;
	പോകുക doend;
    }

#endif

    // forced commands
    എങ്കിൽ (*p == '!' && ea.cmdidx != CMD_substitute
	    && ea.cmdidx != CMD_smagic && ea.cmdidx != CMD_snomagic)
    {
	++p;
	ea.forceit = TRUE;
    }
    അല്ലെങ്കിൽ
	ea.forceit = FALSE;

/*
 * 6. Parse arguments.  Then check for errors.
 */
    എങ്കിൽ (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = (long)cmdnames[(പൂർണ്ണസംഖ്യ)ea.cmdidx].cmd_argt;

    എങ്കിൽ (!ea.skip)
    {
#ifdef HAVE_SANDBOX
	എങ്കിൽ (sandbox != 0 && !(ea.argt & EX_SBOXOK))
	{
	    // Command not allowed in sandbox.
	    errormsg = _(e_not_allowed_in_sandbox);
	    പോകുക doend;
	}
#endif
	എങ്കിൽ (restricted != 0 && (ea.argt & EX_RESTRICT))
	{
	    errormsg = _(e_command_not_allowed_in_rvim);
	    പോകുക doend;
	}
	എങ്കിൽ (!curbuf->b_p_ma && (ea.argt & EX_MODIFY))
	{
	    // Command not allowed in non-'modifiable' buffer
	    errormsg = _(e_cannot_make_changes_modifiable_is_off);
	    പോകുക doend;
	}

	എങ്കിൽ (!IS_USER_CMDIDX(ea.cmdidx))
	{
	    എങ്കിൽ (cmdwin_type != 0 && !(ea.argt & EX_CMDWIN))
	    {
		// Command not allowed in the command line window
		errormsg = _(e_invalid_in_cmdline_window);
		പോകുക doend;
	    }
	    എങ്കിൽ (text_locked() && !(ea.argt & EX_LOCK_OK))
	    {
		// Command not allowed when text is locked
		errormsg = _(get_text_locked_സന്ദേശം());
		പോകുക doend;
	    }
	}

	// Disallow editing another buffer when "curbuf_lock" is set.
	// Do allow ":checktime" (it is postponed).
	// Do allow ":edit" (check for an argument later).
	// Do allow ":file" with no arguments (check for an argument later).
	എങ്കിൽ (!(ea.argt & (EX_CMDWIN | EX_LOCK_OK))
		&& ea.cmdidx != CMD_checktime
		&& ea.cmdidx != CMD_edit
		&& ea.cmdidx != CMD_file
		&& !IS_USER_CMDIDX(ea.cmdidx)
		&& curbuf_locked())
	    പോകുക doend;

	എങ്കിൽ (!ni && !(ea.argt & EX_RANGE) && ea.addr_എണ്ണം > 0)
	{
	    errormsg = _(e_no_range_allowed);
	    പോകുക doend;
	}
    }

    എങ്കിൽ (!ni && !(ea.argt & EX_BANG) && ea.forceit)
    {
	errormsg = _(e_no_bang_allowed);
	പോകുക doend;
    }

    /*
     * Don't complain about the range if it is not used
     * (could happen if line_count is accidentally set to 0).
     */
    എങ്കിൽ (!ea.skip && !ni && (ea.argt & EX_RANGE))
    {
	/*
	 * If the range is backwards, ask for confirmation and, if given, swap
	 * ea.line1 & ea.line2 so it's forwards again.
	 * When global command is busy, don't ask, will fail below.
	 */
	എങ്കിൽ (!global_busy && ea.line1 > ea.line2)
	{
	    എങ്കിൽ (സന്ദേശം_silent == 0)
	    {
		എങ്കിൽ (sourcing || exmode_active)
		{
		    errormsg = _(e_backwards_range_given);
		    പോകുക doend;
		}
		എങ്കിൽ (ask_yesno((char_u *)
			_("Backwards range given, OK to swap"), FALSE) != 'y')
		    പോകുക doend;
	    }
	    lnum = ea.line1;
	    ea.line1 = ea.line2;
	    ea.line2 = lnum;
	}
	എങ്കിൽ ((errormsg = invalid_range(&ea)) != ശൂന്യ)
	    പോകുക doend;
    }

    എങ്കിൽ ((ea.addr_type == ADDR_OTHER) && ea.addr_എണ്ണം == 0)
	// default is 1, not cursor
	ea.line2 = 1;

    correct_range(&ea);

#ifdef FEAT_FOLDING
    എങ്കിൽ (((ea.argt & EX_WHOLEFOLD) || ea.addr_എണ്ണം >= 2) && !global_busy
	    && ea.addr_type == ADDR_വരികൾ)
    {
	// Put the first line at the start of a closed fold, put the last line
	// at the end of a closed fold.
	(ശൂന്യം)hasFolding(ea.line1, &ea.line1, ശൂന്യ);
	(ശൂന്യം)hasFolding(ea.line2, ശൂന്യ, &ea.line2);
    }
#endif

#ifdef FEAT_QUICKFIX
    /*
     * For the ":make" and ":grep" commands we insert the 'makeprg'/'grepprg'
     * option here, so things like % get expanded.
     */
    p = replace_makeprg(&ea, p, cmdlinep);
    എങ്കിൽ (p == ശൂന്യ)
	പോകുക doend;
#endif

    /*
     * Skip to start of argument.
     * Don't do this for the ":!" command, because ":!! -l" needs the space.
     */
    എങ്കിൽ (ea.cmdidx == CMD_bang)
	ea.arg = p;
    അല്ലെങ്കിൽ
	ea.arg = skipwhite(p);

    // ":file" cannot be run with an argument when "curbuf_lock" is set
    എങ്കിൽ (ea.cmdidx == CMD_file && *ea.arg != NUL && curbuf_locked())
	പോകുക doend;

    /*
     * Check for "++opt=val" argument.
     * Must be first, allow ":w ++enc=utf8 !cmd"
     */
    എങ്കിൽ (ea.argt & EX_ARGOPT)
	ആകുമ്പോൾ (ea.arg[0] == '+' && ea.arg[1] == '+')
	    എങ്കിൽ (getargopt(&ea) == FAIL && !ni)
	    {
		errormsg = _(e_invalid_argument);
		പോകുക doend;
	    }

    എങ്കിൽ (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	എങ്കിൽ (*ea.arg == '>')			// append
	{
	    എങ്കിൽ (*++ea.arg != '>')		// typed wrong
	    {
		errormsg = _(e_use_w_or_w_gt_gt);
		പോകുക doend;
	    }
	    ea.arg = skipwhite(ea.arg + 1);
	    ea.append = TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*ea.arg == '!' && ea.cmdidx == CMD_write)  // :w !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    എങ്കിൽ (ea.cmdidx == CMD_read)
    {
	എങ്കിൽ (ea.forceit)
	{
	    ea.usefilter = TRUE;		// :r! filter if ea.forceit
	    ea.forceit = FALSE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*ea.arg == '!')		// :r !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    എങ്കിൽ (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ea.amount = 1;
	ആകുമ്പോൾ (*ea.arg == *ea.cmd)		// count number of '>' or '<'
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
    എങ്കിൽ ((ea.argt & EX_CMDARG) && !ea.usefilter)
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
    എങ്കിൽ ((ea.argt & EX_TRLBAR) && !ea.usefilter)
    {
	separate_nextcmd(&ea, FALSE);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ea.cmdidx == CMD_bang
	    || ea.cmdidx == CMD_terminal
	    || ea.cmdidx == CMD_global
	    || ea.cmdidx == CMD_vglobal
	    || ea.usefilter
#ifdef FEAT_EVAL
	    || inside_block(&ea)
#endif
	    )
    {
	വേണ്ടി (p = ea.arg; *p; ++p)
	{
	    // Remove one backslash before a newline
	    എങ്കിൽ (*p == '\\' && p[1] == '\n')
		STRMOVE(p, p + 1);
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == '\n' && !(ea.argt & EX_വ്യഞ്ജകം_ARG))
	    {
		ea.nextcmd = p + 1;
		*p = NUL;
		നിർത്തുക;
	    }
	}
    }

    എങ്കിൽ ((ea.argt & EX_DFLALL) && ea.addr_എണ്ണം == 0)
	address_default_all(&ea);

    // accept numbered register only when no count allowed (:put)
    എങ്കിൽ (       (ea.argt & EX_REGSTR)
	    && *ea.arg != NUL
	       // Do not allow register = for user commands
	    && (!IS_USER_CMDIDX(ea.cmdidx) || *ea.arg != '=')
	    && !((ea.argt & EX_എണ്ണം) && VIM_ISDIGIT(*ea.arg)))
    {
#എങ്കിൽ !defined(FEAT_CLIPBOARD) && !defined(FEAT_CLIPBOARD_PROVIDER)
	// check these explicitly for a more specific error message
	എങ്കിൽ (*ea.arg == '*' || *ea.arg == '+')
	{
	    errormsg = _(e_invalid_register_name);
	    പോകുക doend;
	}
#endif
	എങ്കിൽ (valid_yank_reg(*ea.arg, (!IS_USER_CMDIDX(ea.cmdidx)
			    && ea.cmdidx != CMD_put && ea.cmdidx != CMD_iput)))
	{
	    ea.regname = *ea.arg++;
#ifdef FEAT_EVAL
	    // for '=' register: accept the rest of the line as an expression
	    എങ്കിൽ (ea.arg[-1] == '=' && ea.arg[0] != NUL)
	    {
		എങ്കിൽ (!ea.skip)
		{
		    set_വ്യഞ്ജകം_വരി(vim_strsave(ea.arg), &ea);
		    did_set_വ്യഞ്ജകം_വരി = TRUE;
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
    എങ്കിൽ ((ea.argt & EX_എണ്ണം) && VIM_ISDIGIT(*ea.arg)
	    && (!(ea.argt & EX_BUFNAME) || *(p = skipdigits(ea.arg + 1)) == NUL
							  || VIM_ISWHITE(*p)))
    {
	n = getdigits_quoted(&ea.arg);
	ea.arg = skipwhite(ea.arg);
	എങ്കിൽ (n <= 0 && !ni && (ea.argt & EX_ZEROR) == 0)
	{
	    errormsg = _(e_positive_എണ്ണം_required);
	    പോകുക doend;
	}
	എങ്കിൽ (ea.addr_type != ADDR_വരികൾ)	// e.g. :buffer 2, :sleep 3
	{
	    ea.line2 = n;
	    എങ്കിൽ (ea.addr_എണ്ണം == 0)
		ea.addr_എണ്ണം = 1;
	}
	അല്ലെങ്കിൽ
	{
	    ea.line1 = ea.line2;
	    എങ്കിൽ (ea.line2 >= LONG_MAX - (n - 1))
		ea.line2 = LONG_MAX;  // avoid overflow
	    അല്ലെങ്കിൽ
		ea.line2 += n - 1;
	    ++ea.addr_എണ്ണം;
	    /*
	     * Be vi compatible: no error message for out of range.
	     */
	    എങ്കിൽ (ea.line2 > curbuf->b_ml.ml_വരി_എണ്ണം)
		ea.line2 = curbuf->b_ml.ml_വരി_എണ്ണം;
	}
    }

    /*
     * Check for flags: 'l', 'p' and '#'.
     */
    എങ്കിൽ (ea.argt & EX_FLAGS)
	get_flags(&ea);
    എങ്കിൽ (!ni && !(ea.argt & EX_EXTRA) && *ea.arg != NUL
	    && *ea.arg != '"' && (*ea.arg != '|' || (ea.argt & EX_TRLBAR) == 0))
    {
	// no arguments allowed but there is something
	errormsg = ex_errmsg(e_trailing_characters_str, ea.arg);
	പോകുക doend;
    }

    എങ്കിൽ (!ni && (ea.argt & EX_NEEDARG) && *ea.arg == NUL)
    {
	errormsg = _(e_argument_required);
	പോകുക doend;
    }

#ifdef FEAT_EVAL
    /*
     * Skip the command when it's not going to be executed.
     * The commands like :if, :endif, etc. always need to be executed.
     * Also make an exception for commands that handle a trailing command
     * themselves.
     */
    എങ്കിൽ (ea.skip)
    {
	തിരഞ്ഞെടുപ്പ് (ea.cmdidx)
	{
	    // commands that need evaluation
	    അവസ്ഥ CMD_while:
	    അവസ്ഥ CMD_endwhile:
	    അവസ്ഥ CMD_for:
	    അവസ്ഥ CMD_endfor:
	    അവസ്ഥ CMD_if:
	    അവസ്ഥ CMD_elseif:
	    അവസ്ഥ CMD_else:
	    അവസ്ഥ CMD_endif:
	    അവസ്ഥ CMD_try:
	    അവസ്ഥ CMD_catch:
	    അവസ്ഥ CMD_finally:
	    അവസ്ഥ CMD_endtry:
	    അവസ്ഥ CMD_function:
	    അവസ്ഥ CMD_def:
				നിർത്തുക;

	    // commands that read a block of lines
	    അവസ്ഥ CMD_abstract:
	    അവസ്ഥ CMD_append:
	    അവസ്ഥ CMD_autocmd:
	    അവസ്ഥ CMD_change:
	    അവസ്ഥ CMD_class:
	    അവസ്ഥ CMD_command:
	    അവസ്ഥ CMD_enum:
	    അവസ്ഥ CMD_തിരുകുക:
	    അവസ്ഥ CMD_interface:
	    അവസ്ഥ CMD_loadkeymap:
				നിർത്തുക;

	    // Commands that handle '|' themselves.  Check: A command should
	    // either have the EX_TRLBAR flag, appear in this list or appear in
	    // the list at ":help :bar".
	    അവസ്ഥ CMD_aboveleft:
	    അവസ്ഥ CMD_and:
	    അവസ്ഥ CMD_belowright:
	    അവസ്ഥ CMD_botright:
	    അവസ്ഥ CMD_browse:
	    അവസ്ഥ CMD_call:
	    അവസ്ഥ CMD_confirm:
	    അവസ്ഥ CMD_const:
	    അവസ്ഥ CMD_delfunction:
	    അവസ്ഥ CMD_djump:
	    അവസ്ഥ CMD_dlist:
	    അവസ്ഥ CMD_dsearch:
	    അവസ്ഥ CMD_dsplit:
	    അവസ്ഥ CMD_echo:
	    അവസ്ഥ CMD_echoerr:
	    അവസ്ഥ CMD_echomsg:
	    അവസ്ഥ CMD_echon:
	    അവസ്ഥ CMD_eval:
	    അവസ്ഥ CMD_execute:
	    അവസ്ഥ CMD_filter:
	    അവസ്ഥ CMD_final:
	    അവസ്ഥ CMD_help:
	    അവസ്ഥ CMD_hide:
	    അവസ്ഥ CMD_horizontal:
	    അവസ്ഥ CMD_ijump:
	    അവസ്ഥ CMD_ilist:
	    അവസ്ഥ CMD_isearch:
	    അവസ്ഥ CMD_isplit:
	    അവസ്ഥ CMD_keepalt:
	    അവസ്ഥ CMD_keepjumps:
	    അവസ്ഥ CMD_keepmarks:
	    അവസ്ഥ CMD_keeppatterns:
	    അവസ്ഥ CMD_leftabove:
	    അവസ്ഥ CMD_let:
	    അവസ്ഥ CMD_lockmarks:
	    അവസ്ഥ CMD_lockvar:
	    അവസ്ഥ CMD_lua:
	    അവസ്ഥ CMD_match:
	    അവസ്ഥ CMD_mzscheme:
	    അവസ്ഥ CMD_noautocmd:
	    അവസ്ഥ CMD_noswapfile:
	    അവസ്ഥ CMD_perl:
	    അവസ്ഥ CMD_psearch:
	    അവസ്ഥ CMD_py3:
	    അവസ്ഥ CMD_python3:
	    അവസ്ഥ CMD_python:
	    അവസ്ഥ CMD_return:
	    അവസ്ഥ CMD_rightbelow:
	    അവസ്ഥ CMD_ruby:
	    അവസ്ഥ CMD_silent:
	    അവസ്ഥ CMD_smagic:
	    അവസ്ഥ CMD_snomagic:
	    അവസ്ഥ CMD_substitute:
	    അവസ്ഥ CMD_syntax:
	    അവസ്ഥ CMD_tab:
	    അവസ്ഥ CMD_tcl:
	    അവസ്ഥ CMD_throw:
	    അവസ്ഥ CMD_tilde:
	    അവസ്ഥ CMD_topleft:
	    അവസ്ഥ CMD_unlet:
	    അവസ്ഥ CMD_unlockvar:
	    അവസ്ഥ CMD_var:
	    അവസ്ഥ CMD_verbose:
	    അവസ്ഥ CMD_vertical:
	    അവസ്ഥ CMD_wincmd:
				നിർത്തുക;

	    സ്ഥിരസ്ഥിതി:		പോകുക doend;
	}
    }
#endif

    എങ്കിൽ ((ea.argt & EX_XFILE)
	    && expand_filename(&ea, cmdlinep, &errormsg) == FAIL)
	പോകുക doend;

#ifdef FEAT_EVAL
    എങ്കിൽ (is_export && (ea.argt & EX_EXPORT) == 0)
    {
	emsg(_(e_invalid_command_after_export));
	പോകുക doend;
    }
#endif

    /*
     * Accept buffer name.  Cannot be used at the same time with a buffer
     * number.  Don't do this for a user command.
     */
    എങ്കിൽ ((ea.argt & EX_BUFNAME) && *ea.arg != NUL && ea.addr_എണ്ണം == 0
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * :bdelete, :bwipeout and :bunload take several arguments, separated
	 * by spaces: find next space (skipping over escaped characters).
	 * The others take one argument: ignore trailing spaces.
	 */
	എങ്കിൽ (ea.cmdidx == CMD_bdelete || ea.cmdidx == CMD_bwipeout
						  || ea.cmdidx == CMD_bunload)
	    p = skiptowhite_esc(ea.arg);
	അല്ലെങ്കിൽ
	{
	    p = ea.arg + STRLEN(ea.arg);
	    ആകുമ്പോൾ (p > ea.arg && VIM_ISWHITE(p[-1]))
		--p;
	}
	ea.line2 = buflist_findpat(ea.arg, p, (ea.argt & EX_BUFUNL) != 0,
								FALSE, FALSE);
	എങ്കിൽ (ea.line2 < 0)	    // failed
	    പോകുക doend;
	ea.addr_എണ്ണം = 1;
	ea.arg = skipwhite(p);
    }

    // The :try command saves the emsg_silent flag, reset it here when
    // ":silent! try" was used, it should only apply to :try itself.
    എങ്കിൽ (ea.cmdidx == CMD_try && cmdmod.cmod_did_esilent > 0)
    {
	emsg_silent -= cmdmod.cmod_did_esilent;
	എങ്കിൽ (emsg_silent < 0)
	    emsg_silent = 0;
	cmdmod.cmod_did_esilent = 0;
    }

/*
 * 7. Execute the command.
 */

    എങ്കിൽ (IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * Execute a user-defined command.
	 */
	do_ucmd(&ea);
    }
    അല്ലെങ്കിൽ
    {
	/*
	 * Call the function to execute the builtin command.
	 */
	(cmdnames[ea.cmdidx].cmd_func)(&ea);
	എങ്കിൽ (ea.errmsg != ശൂന്യ)
	    errormsg = ea.errmsg;
    }

#ifdef FEAT_EVAL
    // A command will reset "is_export" when exporting an item.  If it is still
    // set something went wrong or the command was never executed.
    എങ്കിൽ (!ea.skip && is_export)
    {
	എങ്കിൽ (errormsg == ശൂന്യ)
	    errormsg = _(e_export_with_invalid_argument);
	is_export = FALSE;
    }

    // Set flag that any command was executed, used by ex_vim9script().
    // Not if this was a command that wasn't executed or :endif.
    എങ്കിൽ (sourcing_a_script(&ea)
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
    എങ്കിൽ (need_rethrow)
	do_throw(cstack);
    അല്ലെങ്കിൽ എങ്കിൽ (check_cstack)
    {
	എങ്കിൽ (source_finished(fgetline, cookie))
	    do_finish(&ea, TRUE);
	അല്ലെങ്കിൽ എങ്കിൽ (getline_equal(fgetline, cookie, get_func_വരി)
						   && current_func_returned())
	    do_return(&ea, TRUE, FALSE, ശൂന്യ);
    }
    need_rethrow = check_cstack = FALSE;
#endif

doend:
    എങ്കിൽ (curwin->w_cursor.lnum == 0)	// can happen with zero line number
    {
	curwin->w_cursor.lnum = 1;
	curwin->w_cursor.col = 0;
    }

    എങ്കിൽ (errormsg != ശൂന്യ && *errormsg != NUL && !did_emsg)
    {
	എങ്കിൽ ((sourcing || !താക്കോൽ_Typed) && !did_append_cmd)
	{
	    എങ്കിൽ (errormsg != (അക്ഷരം *)IObuff)
	    {
		STRCPY(IObuff, errormsg);
		errormsg = (അക്ഷരം *)IObuff;
	    }
	    append_command(*cmdlinep);
	}
	emsg(errormsg);
    }
#ifdef FEAT_EVAL
    do_errthrow(cstack,
	    (ea.cmdidx != CMD_SIZE && !IS_USER_CMDIDX(ea.cmdidx))
			? cmdnames[(പൂർണ്ണസംഖ്യ)ea.cmdidx].cmd_name : (char_u *)ശൂന്യ);

    എങ്കിൽ (did_set_വ്യഞ്ജകം_വരി)
	set_വ്യഞ്ജകം_വരി(ശൂന്യ, ശൂന്യ);
    is_export = FALSE;
#endif

    undo_cmdmod(&cmdmod);
    cmdmod = save_cmdmod;
    reg_executing = save_reg_executing;
    pending_end_reg_executing = save_pending_end_reg_executing;

    എങ്കിൽ (ea.nextcmd && *ea.nextcmd == NUL)	// not really a next command
	ea.nextcmd = ശൂന്യ;

#ifdef FEAT_EVAL
    --ex_nesting_level;
    vim_free(ea.cmdline_tofree);
#endif

    തിരികെ ea.nextcmd;
}

സ്ഥിതം അക്ഷരം ex_error_buf[സന്ദേശം_BUF_LEN];

/*
 * Return an error message with argument included.
 * Uses a static buffer, only the last error will be kept.
 * "msg" will be translated, caller should use N_().
 */
    അക്ഷരം *
ex_errmsg(അക്ഷരം *സന്ദേശം, char_u *arg)
{
    vim_snprintf(ex_error_buf, സന്ദേശം_BUF_LEN, _(സന്ദേശം), arg);
    തിരികെ ex_error_buf;
}

/*
 * The "+" string used in place of an empty command in Ex mode.
 * This string is used in pointer comparison.
 */
സ്ഥിതം സ്ഥിരം അക്ഷരം exmode_plus[] = "+";

/*
 * Handle a range without a command.
 * Returns an error message on failure.
 */
    അക്ഷരം *
ex_range_without_command(exarg_T *eap)
{
    അക്ഷരം *errormsg = ശൂന്യ;

    എങ്കിൽ ((*eap->cmd == '|' ||
		(exmode_active && eap->cmd != (char_u *)exmode_plus + 1))
#ifdef FEAT_EVAL
	    && !in_vim9script()
#endif
       )
    {
	eap->cmdidx = CMD_print;
	eap->argt = EX_RANGE+EX_എണ്ണം+EX_TRLBAR;
	എങ്കിൽ ((errormsg = invalid_range(eap)) == ശൂന്യ)
	{
	    correct_range(eap);
	    ex_print(eap);
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (eap->addr_എണ്ണം != 0)
    {
	എങ്കിൽ (eap->line2 > curbuf->b_ml.ml_വരി_എണ്ണം)
	{
	    // With '-' in 'cpoptions' a line number past the file is an
	    // error, otherwise put it at the end of the file.
	    എങ്കിൽ (vim_strchr(p_cpo, CPO_MINUS) != ശൂന്യ)
		eap->line2 = -1;
	    അല്ലെങ്കിൽ
		eap->line2 = curbuf->b_ml.ml_വരി_എണ്ണം;
	}

	എങ്കിൽ (eap->line2 < 0)
	    errormsg = _(e_invalid_range);
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (eap->line2 == 0)
		curwin->w_cursor.lnum = 1;
	    അല്ലെങ്കിൽ
		curwin->w_cursor.lnum = eap->line2;
	    beginline(BL_SOL | BL_FIX);
	}
    }
    തിരികെ errormsg;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 * If "noparen" is TRUE do not recognize the command followed by "(" or ".".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
checkforcmd_opt(
    char_u	**pp,		// start of command
    അക്ഷരം	*cmd,		// name of command
    പൂർണ്ണസംഖ്യ		len,		// required length
    പൂർണ്ണസംഖ്യ		noparen)
{
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; cmd[i] != NUL; ++i)
	എങ്കിൽ (((char_u *)cmd)[i] != (*pp)[i])
	    നിർത്തുക;
    എങ്കിൽ (i >= len && !ASCII_ISALPHA((*pp)[i]) && (*pp)[i] != '_'
			 && (!noparen || ((*pp)[i] != '(' && (*pp)[i] != '.')))
    {
	*pp = skipwhite(*pp + i);
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    പൂർണ്ണസംഖ്യ
checkforcmd(
    char_u	**pp,		// start of command
    അക്ഷരം	*cmd,		// name of command
    പൂർണ്ണസംഖ്യ		len)		// required length
{
    തിരികെ checkforcmd_opt(pp, cmd, len, FALSE);
}

/*
 * Check for an Ex command with optional tail, not followed by "(" or ".".
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    പൂർണ്ണസംഖ്യ
checkforcmd_noparen(
    char_u	**pp,		// start of command
    അക്ഷരം	*cmd,		// name of command
    പൂർണ്ണസംഖ്യ		len)		// required length
{
    തിരികെ checkforcmd_opt(pp, cmd, len, TRUE);
}

/*
 * Find the replacement text of a ":command", the part after the attributes and
 * the command name.  Returns NULL when there is none.  Does not modify "arg"
 * and gives no error messages.
 */
    സ്ഥിതം char_u *
find_ucmd_repl(char_u *arg)
{
    char_u	*p = arg;

    // Skip over the attributes.
    ആകുമ്പോൾ (*p == '-')
	p = skipwhite(skiptowhite(p));

    // Skip over the command name.
    എങ്കിൽ (!ASCII_ISALPHA(*p))
	തിരികെ ശൂന്യ;
    ആകുമ്പോൾ (ASCII_ISALNUM(*p))
	++p;

    തിരികെ *p == NUL ? ശൂന്യ : skipwhite(p);
}

/*
 * Find the "{" in "line" that starts a block for ":command" or ":autocmd".
 * That is the case when the command argument is "{" at the end of the line,
 * also when the command is nested in another ":command" or ":autocmd".
 * Returns NULL when the line does not start such a block.
 */
    char_u *
find_cmd_block_start(char_u *വരി)
{
    char_u	*p = skipwhite(വരി);

    വേണ്ടി (;;)
    {
	char_u	*arg = p;

	എങ്കിൽ (*p == '{' && ends_excmd2(p, skipwhite(p + 1)))
	    തിരികെ p;

	എങ്കിൽ (checkforcmd_noparen(&arg, "autocmd", 2))
	{
	    എങ്കിൽ (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = au_find_cmd_arg(arg);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (checkforcmd_noparen(&arg, "command", 3))
	{
	    എങ്കിൽ (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = find_ucmd_repl(arg);
	}
	അല്ലെങ്കിൽ
	    തിരികെ ശൂന്യ;

	എങ്കിൽ (p == ശൂന്യ)
	    തിരികെ ശൂന്യ;
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
    പൂർണ്ണസംഖ്യ
parse_command_modifiers(
	exarg_T	    *eap,
	അക്ഷരം	    **errormsg,
	cmdmod_T    *cmod,
	പൂർണ്ണസംഖ്യ	    skip_only)
{
    char_u  *orig_cmd = eap->cmd;
    char_u  *cmd_start = ശൂന്യ;
    പൂർണ്ണസംഖ്യ	    use_plus_cmd = FALSE;
    പൂർണ്ണസംഖ്യ	    starts_with_colon = FALSE;
    പൂർണ്ണസംഖ്യ	    vim9script = in_vim9script();
    പൂർണ്ണസംഖ്യ	    has_visual_range = FALSE;

    CLEAR_POINTER(cmod);
    cmod->cmod_flags = sticky_cmdmod_flags;

    എങ്കിൽ (STRNCMP(eap->cmd, "'<,'>", 5) == 0)
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
    വേണ്ടി (;;)
    {
	char_u  *p;

	ആകുമ്പോൾ (*eap->cmd == ' ' || *eap->cmd == '\t' || *eap->cmd == ':')
	{
	    എങ്കിൽ (*eap->cmd == ':')
		starts_with_colon = TRUE;
	    ++eap->cmd;
	}

	// in ex mode, an empty command (after modifiers) works like :+
	എങ്കിൽ (*eap->cmd == NUL && exmode_active
		   && (getline_equal(eap->ea_getline, eap->cookie, getexmodeline)
		       || getline_equal(eap->ea_getline, eap->cookie, getexline))
			&& curwin->w_cursor.lnum < curbuf->b_ml.ml_വരി_എണ്ണം)
	{
	    use_plus_cmd = TRUE;
	    എങ്കിൽ (!skip_only)
		ex_pressedreturn = TRUE;
	    നിർത്തുക;  // no modifiers following
	}

	// ignore comment and empty lines
	എങ്കിൽ (comment_start(eap->cmd, starts_with_colon))
	{
	    // a comment ends at a NL
	    eap->nextcmd = vim_strchr(eap->cmd, '\n');
	    എങ്കിൽ (eap->nextcmd != ശൂന്യ)
		++eap->nextcmd;
	    എങ്കിൽ (vim9script)
	    {
		എങ്കിൽ (has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
#ifdef FEAT_EVAL
		എങ്കിൽ (eap->cmd[0] == '#' && eap->cmd[1] == '{'
							 && eap->cmd[2] != '{')
		    *errormsg = _(e_cannot_use_hash_curly_to_start_comment);
#endif
	    }
	    തിരികെ FAIL;
	}
	എങ്കിൽ (*eap->cmd == '\n')
	{
	    eap->nextcmd = eap->cmd + 1;
	    തിരികെ FAIL;
	}
	എങ്കിൽ (*eap->cmd == NUL)
	{
	    എങ്കിൽ (!skip_only)
	    {
		ex_pressedreturn = TRUE;
		എങ്കിൽ (vim9script && has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
	    }
	    തിരികെ FAIL;
	}

	p = skip_range(eap->cmd, TRUE, ശൂന്യ);

	// In Vim9 script a variable can shadow a command modifier:
	//   verbose = 123
	//   verbose += 123
	//   silent! verbose = func()
	//   verbose.member = 2
	//   verbose[expr] = 2
	// But not:
	//   verbose [a, b] = list
	എങ്കിൽ (vim9script)
	{
	    char_u *s, *n;

	    വേണ്ടി (s = eap->cmd; ASCII_ISALPHA(*s); ++s)
		;
	    n = skipwhite(s);
	    എങ്കിൽ (*n == '.' || *n == '=' || (*n != NUL && n[1] == '=')
		    || *s == '[')
		നിർത്തുക;
	}

	തിരഞ്ഞെടുപ്പ് (*p)
	{
	    // When adding an entry, also modify cmdmod_info_tab[].
	    അവസ്ഥ 'a':	എങ്കിൽ (!checkforcmd_noparen(&eap->cmd, "aboveleft", 3))
			    നിർത്തുക;
			cmod->cmod_split |= WSP_ABOVE;
			തുടരുക;

	    അവസ്ഥ 'b':	എങ്കിൽ (checkforcmd_noparen(&eap->cmd, "belowright", 3))
			{
			    cmod->cmod_split |= WSP_BELOW;
			    തുടരുക;
			}
			എങ്കിൽ (checkforcmd_opt(&eap->cmd, "browse", 3, TRUE))
			{
#ifdef FEAT_BROWSE_CMD
			    cmod->cmod_flags |= CMOD_BROWSE;
#endif
			    തുടരുക;
			}
			എങ്കിൽ (!checkforcmd_noparen(&eap->cmd, "botright", 2))
			    നിർത്തുക;
			cmod->cmod_split |= WSP_BOT;
			തുടരുക;

	    അവസ്ഥ 'c':	എങ്കിൽ (!checkforcmd_opt(&eap->cmd, "confirm", 4, TRUE))
			    നിർത്തുക;
#എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			cmod->cmod_flags |= CMOD_CONFIRM;
#endif
			തുടരുക;

	    അവസ്ഥ 'k':	എങ്കിൽ (checkforcmd_noparen(&eap->cmd, "keepmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_KEEPMARKS;
			    തുടരുക;
			}
			എങ്കിൽ (checkforcmd_noparen(&eap->cmd, "keepalt", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPALT;
			    തുടരുക;
			}
			എങ്കിൽ (checkforcmd_noparen(&eap->cmd, "keeppatterns", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPPATTERNS;
			    തുടരുക;
			}
			എങ്കിൽ (!checkforcmd_noparen(&eap->cmd, "keepjumps", 5))
			    നിർത്തുക;
			cmod->cmod_flags |= CMOD_KEEPJUMPS;
			തുടരുക;

	    അവസ്ഥ 'f':	// only accept ":filter {pat} cmd"
			{
			    char_u  *reg_pat;
			    char_u  *nulp = ശൂന്യ;
			    പൂർണ്ണസംഖ്യ	    c = 0;

			    എങ്കിൽ (!checkforcmd_noparen(&p, "filter", 4)
				    || *p == NUL
				    || (ends_excmd(*p)
#ifdef FEAT_EVAL
					// in ":filter #pat# cmd" # does not
					// start a comment
				     && (!vim9script || VIM_ISWHITE(p[1]))
#endif
				     ))
				നിർത്തുക;
			    എങ്കിൽ (*p == '!')
			    {
				cmod->cmod_filter_force = TRUE;
				p = skipwhite(p + 1);
				എങ്കിൽ (*p == NUL || ends_excmd(*p))
				    നിർത്തുക;
			    }
#ifdef FEAT_EVAL
			    // Avoid that "filter(arg)" is recognized.
			    എങ്കിൽ (vim9script && !VIM_ISWHITE(p[-1]))
				നിർത്തുക;
#endif
			    എങ്കിൽ (skip_only)
				p = skip_vimgrep_pat(p, ശൂന്യ, ശൂന്യ);
			    അല്ലെങ്കിൽ
				// NOTE: This puts a NUL after the pattern.
				p = skip_vimgrep_pat_ext(p, &reg_pat, ശൂന്യ,
								    &nulp, &c);
			    എങ്കിൽ (p == ശൂന്യ || *p == NUL)
				നിർത്തുക;
			    എങ്കിൽ (!skip_only)
			    {
				cmod->cmod_filter_regmatch.regprog =
						vim_regcomp(reg_pat, RE_MAGIC);
				എങ്കിൽ (cmod->cmod_filter_regmatch.regprog == ശൂന്യ)
				    നിർത്തുക;
				// restore the character overwritten by NUL
				എങ്കിൽ (nulp != ശൂന്യ)
				    *nulp = c;
			    }
			    eap->cmd = p;
			    തുടരുക;
			}

	    അവസ്ഥ 'h':	എങ്കിൽ (checkforcmd_noparen(&eap->cmd, "horizontal", 3))
			{
			    cmod->cmod_split |= WSP_HOR;
			    തുടരുക;
			}
			// ":hide" and ":hide | cmd" are not modifiers
			എങ്കിൽ (p != eap->cmd || !checkforcmd_noparen(&p, "hide", 3)
					       || *p == NUL || ends_excmd(*p))
			    നിർത്തുക;
			eap->cmd = p;
			cmod->cmod_flags |= CMOD_HIDE;
			തുടരുക;

	    അവസ്ഥ 'l':	എങ്കിൽ (checkforcmd_noparen(&eap->cmd, "lockmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_LOCKMARKS;
			    തുടരുക;
			}
			എങ്കിൽ (checkforcmd_noparen(&eap->cmd, "legacy", 3))
			{
			    എങ്കിൽ (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_legacy_must_be_followed_by_command);
				തിരികെ FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_VIM9CMD;
			    cmod->cmod_flags |= CMOD_LEGACY;
			    തുടരുക;
			}

			എങ്കിൽ (!checkforcmd_noparen(&eap->cmd, "leftabove", 5))
			    നിർത്തുക;
			cmod->cmod_split |= WSP_ABOVE;
			തുടരുക;

	    അവസ്ഥ 'n':	എങ്കിൽ (checkforcmd_noparen(&eap->cmd, "noautocmd", 3))
			{
			    cmod->cmod_flags |= CMOD_NOAUTOCMD;
			    തുടരുക;
			}
			എങ്കിൽ (!checkforcmd_noparen(&eap->cmd, "noswapfile", 3))
			    നിർത്തുക;
			cmod->cmod_flags |= CMOD_NOSWAPFILE;
			തുടരുക;

	    അവസ്ഥ 'r':	എങ്കിൽ (!checkforcmd_noparen(&eap->cmd, "rightbelow", 6))
			    നിർത്തുക;
			cmod->cmod_split |= WSP_BELOW;
			തുടരുക;

	    അവസ്ഥ 's':	എങ്കിൽ (checkforcmd_noparen(&eap->cmd, "sandbox", 3))
			{
			    cmod->cmod_flags |= CMOD_SANDBOX;
			    തുടരുക;
			}
			എങ്കിൽ (!checkforcmd_noparen(&eap->cmd, "silent", 3))
			    നിർത്തുക;
			cmod->cmod_flags |= CMOD_SILENT;
			എങ്കിൽ (*eap->cmd == '!' && !VIM_ISWHITE(eap->cmd[-1]))
			{
			    // ":silent!", but not "silent !cmd"
			    eap->cmd = skipwhite(eap->cmd + 1);
			    cmod->cmod_flags |= CMOD_ERRSILENT;
			}
			തുടരുക;

	    അവസ്ഥ 't':	എങ്കിൽ (checkforcmd_noparen(&p, "tab", 3))
			{
			    എങ്കിൽ (!skip_only)
			    {
				long tabnr = get_address(eap, &eap->cmd,
						    ADDR_TABS, eap->skip,
						    skip_only, FALSE, 1);
				എങ്കിൽ (tabnr == MAXLNUM)
				    cmod->cmod_tab = tabpage_index(curtab) + 1;
				അല്ലെങ്കിൽ
				{
				    എങ്കിൽ (tabnr < 0 || tabnr > LAST_TAB_NR)
				    {
					*errormsg = _(e_invalid_range);
					തിരികെ FAIL;
				    }
				    cmod->cmod_tab = tabnr + 1;
				}
			    }
			    eap->cmd = p;
			    തുടരുക;
			}
			എങ്കിൽ (!checkforcmd_noparen(&eap->cmd, "topleft", 2))
			    നിർത്തുക;
			cmod->cmod_split |= WSP_മുകൾ;
			തുടരുക;

	    അവസ്ഥ 'u':	എങ്കിൽ (!checkforcmd_noparen(&eap->cmd, "unsilent", 3))
			    നിർത്തുക;
			cmod->cmod_flags |= CMOD_UNSILENT;
			തുടരുക;

	    അവസ്ഥ 'v':	എങ്കിൽ (checkforcmd_noparen(&eap->cmd, "vertical", 4))
			{
			    cmod->cmod_split |= WSP_VERT;
			    തുടരുക;
			}
			എങ്കിൽ (checkforcmd_noparen(&eap->cmd, "vim9cmd", 4))
			{
			    എങ്കിൽ (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_vim9cmd_must_be_followed_by_command);
				തിരികെ FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_LEGACY;
			    cmod->cmod_flags |= CMOD_VIM9CMD;
			    തുടരുക;
			}
			എങ്കിൽ (!checkforcmd_noparen(&p, "verbose", 4))
			    നിർത്തുക;
			എങ്കിൽ (vim_isdigit(*eap->cmd))
			{
			    // zero means not set, one is verbose == 0, etc.
			    cmod->cmod_verbose = atoi((അക്ഷരം *)eap->cmd) + 1;
			}
			അല്ലെങ്കിൽ
			    cmod->cmod_verbose = 2;  // default: verbose == 1
			eap->cmd = p;
			തുടരുക;
	}
	നിർത്തുക;
    }

    എങ്കിൽ (has_visual_range)
    {
	എങ്കിൽ (eap->cmd > cmd_start)
	{
	    // Move the '<,'> range to after the modifiers and insert a colon.
	    // Since the modifiers have been parsed put the colon on top of the
	    // space: "'<,'>mod cmd" -> "mod:'<,'>cmd
	    // Put eap->cmd after the colon.
	    എങ്കിൽ (use_plus_cmd)
	    {
		size_t len = STRLEN(cmd_start);

		// Special case: empty command uses "+":
		//  "'<,'>mods" -> "mods *+
		//  Use "*" instead of "'<,'>" to avoid the command getting
		//  longer, in case it was allocated.
		mch_memmove(orig_cmd, cmd_start, len);
		STRCPY(orig_cmd + len, " *+");
	    }
	    അല്ലെങ്കിൽ
	    {
		mch_memmove(cmd_start - 5, cmd_start, eap->cmd - cmd_start);
		eap->cmd -= 5;
		mch_memmove(eap->cmd - 1, ":'<,'>", 6);
	    }
	}
	അല്ലെങ്കിൽ
	    // No modifiers, move the pointer back.
	    // Special case: change empty command to "+".
	    എങ്കിൽ (use_plus_cmd)
		eap->cmd = (char_u *)"'<,'>+";
	    അല്ലെങ്കിൽ
		eap->cmd = orig_cmd;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (use_plus_cmd)
	eap->cmd = (char_u *)exmode_plus;

    തിരികെ OK;
}

/*
 * Return TRUE if "cmod" has anything set.
 */
    പൂർണ്ണസംഖ്യ
has_cmdmod(cmdmod_T *cmod, പൂർണ്ണസംഖ്യ ignore_silent)
{
    തിരികെ (cmod->cmod_flags != 0 && (!ignore_silent
		|| (cmod->cmod_flags
		      & ~(CMOD_SILENT | CMOD_ERRSILENT | CMOD_UNSILENT)) != 0))
	    || cmod->cmod_split != 0
	    || cmod->cmod_verbose > 0
	    || cmod->cmod_tab != 0
	    || cmod->cmod_filter_regmatch.regprog != ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * If Vim9 script and "cmdmod" has anything set give an error and return TRUE.
 */
    പൂർണ്ണസംഖ്യ
cmdmod_error(പൂർണ്ണസംഖ്യ ignore_silent)
{
    എങ്കിൽ (in_vim9script() && has_cmdmod(&cmdmod, ignore_silent))
    {
	emsg(_(e_misplaced_command_modifier));
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}
#endif

/*
 * Apply the command modifiers.  Saves current state in "cmdmod", call
 * undo_cmdmod() later.
 */
    ശൂന്യം
apply_cmdmod(cmdmod_T *cmod)
{
#ifdef HAVE_SANDBOX
    എങ്കിൽ ((cmod->cmod_flags & CMOD_SANDBOX) && !cmod->cmod_did_sandbox)
    {
	++sandbox;
	cmod->cmod_did_sandbox = TRUE;
    }
#endif
    എങ്കിൽ (cmod->cmod_verbose > 0)
    {
	എങ്കിൽ (cmod->cmod_verbose_save == 0)
	    cmod->cmod_verbose_save = p_verbose + 1;
	p_verbose = cmod->cmod_verbose - 1;
    }

    എങ്കിൽ ((cmod->cmod_flags & (CMOD_SILENT | CMOD_UNSILENT))
	    && cmod->cmod_save_സന്ദേശം_silent == 0)
    {
	cmod->cmod_save_സന്ദേശം_silent = സന്ദേശം_silent + 1;
	cmod->cmod_save_സന്ദേശം_scroll = സന്ദേശം_scroll;
    }
    എങ്കിൽ (cmod->cmod_flags & CMOD_SILENT)
	++സന്ദേശം_silent;
    എങ്കിൽ (cmod->cmod_flags & CMOD_UNSILENT)
	സന്ദേശം_silent = 0;

    എങ്കിൽ (cmod->cmod_flags & CMOD_ERRSILENT)
    {
	++emsg_silent;
	++cmod->cmod_did_esilent;
    }

    എങ്കിൽ ((cmod->cmod_flags & CMOD_NOAUTOCMD) && cmod->cmod_save_ei == ശൂന്യ)
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
    ശൂന്യം
undo_cmdmod(cmdmod_T *cmod)
{
    എങ്കിൽ (cmod->cmod_verbose_save > 0)
    {
	p_verbose = cmod->cmod_verbose_save - 1;
	cmod->cmod_verbose_save = 0;
    }

#ifdef HAVE_SANDBOX
    എങ്കിൽ (cmod->cmod_did_sandbox)
    {
	--sandbox;
	cmod->cmod_did_sandbox = FALSE;
    }
#endif

    എങ്കിൽ (cmod->cmod_save_ei != ശൂന്യ)
    {
	// Restore 'eventignore' to the value before ":noautocmd".
	set_string_option_direct((char_u *)"ei", -1, cmod->cmod_save_ei,
							   OPT_FREE, SID_NONE);
	free_string_option(cmod->cmod_save_ei);
	cmod->cmod_save_ei = ശൂന്യ;
    }

    vim_regfree(cmod->cmod_filter_regmatch.regprog);

    എങ്കിൽ (cmod->cmod_save_സന്ദേശം_silent > 0)
    {
	// messages could be enabled for a serious error, need to check if the
	// counters don't become negative
	എങ്കിൽ (!did_emsg || സന്ദേശം_silent > cmod->cmod_save_സന്ദേശം_silent - 1)
	    സന്ദേശം_silent = cmod->cmod_save_സന്ദേശം_silent - 1;
	emsg_silent -= cmod->cmod_did_esilent;
	എങ്കിൽ (emsg_silent < 0)
	    emsg_silent = 0;
	// Restore msg_scroll, it's set by file I/O commands, even when no
	// message is actually displayed.
	സന്ദേശം_scroll = cmod->cmod_save_സന്ദേശം_scroll;

	// "silent reg" or "silent echo x" inside "redir" leaves msg_col
	// somewhere in the line.  Put it back in the first column.
	എങ്കിൽ (redirecting())
	    സന്ദേശം_col = 0;

	cmod->cmod_save_സന്ദേശം_silent = 0;
	cmod->cmod_did_esilent = 0;
    }
}

/*
 * Parse the address range, if any, in "eap".
 * May set the last search pattern, unless "silent" is TRUE.
 * Return FAIL and set "errormsg" or return OK.
 */
    പൂർണ്ണസംഖ്യ
parse_cmd_address(exarg_T *eap, അക്ഷരം **errormsg, പൂർണ്ണസംഖ്യ silent)
{
    പൂർണ്ണസംഖ്യ		address_എണ്ണം = 1;
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		need_check_cursor = FALSE;
    പൂർണ്ണസംഖ്യ		ret = FAIL;

    // Repeat for all ',' or ';' separated addresses.
    വേണ്ടി (;;)
    {
	eap->line1 = eap->line2;
	eap->line2 = default_address(eap);
	eap->cmd = skipwhite(eap->cmd);
	lnum = get_address(eap, &eap->cmd, eap->addr_type, eap->skip, silent,
					eap->addr_എണ്ണം == 0, address_എണ്ണം++);
	എങ്കിൽ (eap->cmd == ശൂന്യ)	// error detected
	    പോകുക theend;
	എങ്കിൽ (lnum == MAXLNUM)
	{
	    എങ്കിൽ (*eap->cmd == '%')   // '%' - all lines
	    {
		++eap->cmd;
		തിരഞ്ഞെടുപ്പ് (eap->addr_type)
		{
		    അവസ്ഥ ADDR_വരികൾ:
		    അവസ്ഥ ADDR_OTHER:
			eap->line1 = 1;
			eap->line2 = curbuf->b_ml.ml_വരി_എണ്ണം;
			നിർത്തുക;
		    അവസ്ഥ ADDR_LOADED_BUFFERS:
			{
			    buf_T	*buf = firstbuf;

			    ആകുമ്പോൾ (buf->b_next != ശൂന്യ
						  && buf->b_ml.ml_mfp == ശൂന്യ)
				buf = buf->b_next;
			    eap->line1 = buf->b_fnum;
			    buf = lastbuf;
			    ആകുമ്പോൾ (buf->b_prev != ശൂന്യ
						  && buf->b_ml.ml_mfp == ശൂന്യ)
				buf = buf->b_prev;
			    eap->line2 = buf->b_fnum;
			    നിർത്തുക;
			}
		    അവസ്ഥ ADDR_BUFFERS:
			eap->line1 = firstbuf->b_fnum;
			eap->line2 = lastbuf->b_fnum;
			നിർത്തുക;
		    അവസ്ഥ ADDR_WINDOWS:
		    അവസ്ഥ ADDR_TABS:
			എങ്കിൽ (IS_USER_CMDIDX(eap->cmdidx))
			{
			    eap->line1 = 1;
			    eap->line2 = eap->addr_type == ADDR_WINDOWS
						  ? LAST_WIN_NR : LAST_TAB_NR;
			}
			അല്ലെങ്കിൽ
			{
			    // there is no Vim command which uses '%' and
			    // ADDR_WINDOWS or ADDR_TABS
			    *errormsg = _(e_invalid_range);
			    പോകുക theend;
			}
			നിർത്തുക;
		    അവസ്ഥ ADDR_TABS_RELATIVE:
		    അവസ്ഥ ADDR_UNSIGNED:
		    അവസ്ഥ ADDR_QUICKFIX:
			*errormsg = _(e_invalid_range);
			പോകുക theend;
		    അവസ്ഥ ADDR_ARGUMENTS:
			എങ്കിൽ (ARGCOUNT == 0)
			    eap->line1 = eap->line2 = 0;
			അല്ലെങ്കിൽ
			{
			    eap->line1 = 1;
			    eap->line2 = ARGCOUNT;
			}
			നിർത്തുക;
		    അവസ്ഥ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			eap->line1 = 1;
			eap->line2 = qf_get_valid_size(eap);
			എങ്കിൽ (eap->line2 == 0)
			    eap->line2 = 1;
#endif
			നിർത്തുക;
		    അവസ്ഥ ADDR_NONE:
			// Will give an error later if a range is found.
			നിർത്തുക;
		}
		++eap->addr_എണ്ണം;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*eap->cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == ശൂന്യ)
	    {
		pos_T	    *fp;

		// '*' - visual area
		എങ്കിൽ (eap->addr_type != ADDR_വരികൾ)
		{
		    *errormsg = _(e_invalid_range);
		    പോകുക theend;
		}

		++eap->cmd;
		എങ്കിൽ (!eap->skip)
		{
		    fp = getmark('<', FALSE);
		    എങ്കിൽ (check_mark(fp) == FAIL)
			പോകുക theend;
		    eap->line1 = fp->lnum;
		    fp = getmark('>', FALSE);
		    എങ്കിൽ (check_mark(fp) == FAIL)
			പോകുക theend;
		    eap->line2 = fp->lnum;
		    ++eap->addr_എണ്ണം;
		}
	    }
	}
	അല്ലെങ്കിൽ
	    eap->line2 = lnum;
	eap->addr_എണ്ണം++;

	എങ്കിൽ (*eap->cmd == ';')
	{
	    എങ്കിൽ (!eap->skip)
	    {
		curwin->w_cursor.lnum = eap->line2;

		// Don't leave the cursor on an illegal line or column, but do
		// accept zero as address, so 0;/PATTERN/ works correctly
		// (where zero usually means to use the first line).
		// Check the cursor position before returning.
		എങ്കിൽ (eap->line2 > 0)
		    check_cursor();
		അല്ലെങ്കിൽ
		    check_cursor_col();
		need_check_cursor = TRUE;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*eap->cmd != ',')
	    നിർത്തുക;
	++eap->cmd;
    }

    // One address given: set start and end lines.
    എങ്കിൽ (eap->addr_എണ്ണം == 1)
    {
	eap->line1 = eap->line2;
	// ... but only implicit: really no address given
	എങ്കിൽ (lnum == MAXLNUM)
	    eap->addr_എണ്ണം = 0;
    }
    ret = OK;

theend:
    എങ്കിൽ (need_check_cursor)
	check_cursor();
    തിരികെ ret;
}

/*
 * Append "cmd" to the error message in IObuff.
 * Takes care of limiting the length and handling 0xa0, which would be
 * invisible otherwise.
 */
    സ്ഥിതം ശൂന്യം
append_command(char_u *cmd)
{
    size_t  len = STRLEN(IObuff);
    char_u  *s = cmd;
    char_u  *d;

    എങ്കിൽ (len > IOSIZE - 100)
    {
	// Not enough space, truncate and put in "...".
	d = IObuff + IOSIZE - 100;
	d -= mb_head_off(IObuff, d);
	STRCPY(d, "...");
    }
    STRCAT(IObuff, ": ");
    d = IObuff + STRLEN(IObuff);
    ആകുമ്പോൾ (*s != NUL && d - IObuff + 5 < IOSIZE)
    {
	എങ്കിൽ (enc_utf8 ? (s[0] == 0xc2 && s[1] == 0xa0) : *s == 0xa0)
	{
	    s += enc_utf8 ? 2 : 1;
	    STRCPY(d, "<a0>");
	    d += 4;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (d - IObuff + (*mb_ptr2len)(s) + 1 >= IOSIZE)
	    നിർത്തുക;
	അല്ലെങ്കിൽ
	    MB_COPY_CHAR(s, d);
    }
    *d = NUL;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * If "start" points "&opt", "&l:opt", "&g:opt" or "$ENV" return a pointer to
 * the name.  Otherwise just return "start".
 */
    char_u *
skip_option_env_lead(char_u *start)
{
    char_u *name = start;

    എങ്കിൽ (*start == '&')
    {
	എങ്കിൽ ((start[1] == 'l' || start[1] == 'g') && start[2] == ':')
	    name += 3;
	അല്ലെങ്കിൽ
	    name += 1;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*start == '$')
	name += 1;
    തിരികെ name;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
one_letter_cmd(char_u *p, cmdidx_T *idx)
{
    എങ്കിൽ (in_vim9script())
	തിരികെ FALSE;
    എങ്കിൽ (p[0] == 'k'
	    && (p[1] != 'e' || (p[1] == 'e' && p[2] != 'e')))
    {
	*idx = CMD_k;
	തിരികെ TRUE;
    }
    എങ്കിൽ (p[0] == 's'
	    && ((p[1] == 'c' && (p[2] == NUL || (p[2] != 's' && p[2] != 'r'
			&& (p[3] == NUL || (p[3] != 'i' && p[4] != 'p')))))
		|| p[1] == 'g'
		|| (p[1] == 'i' && p[2] != 'm' && p[2] != 'l' && p[2] != 'g')
		|| p[1] == 'I'
		|| (p[1] == 'r' && p[2] != 'e')))
    {
	*idx = CMD_substitute;
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Return TRUE if "cmd" starts with "123->", a number followed by a method
 * call.
 */
    പൂർണ്ണസംഖ്യ
സംഖ്യ_method(char_u *cmd)
{
    char_u *p = skipdigits(cmd);

    തിരികെ p > cmd && (p = skipwhite(p))[0] == '-' && p[1] == '>';
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
	പൂർണ്ണസംഖ്യ	*full UNUSED,
	പൂർണ്ണസംഖ്യ	(*lookup)(char_u *, size_t, പൂർണ്ണസംഖ്യ cmd, cctx_T *) UNUSED,
	cctx_T	*cctx UNUSED)
{
    പൂർണ്ണസംഖ്യ		len;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		i;
#ifndef FEAT_EVAL
    പൂർണ്ണസംഖ്യ		vim9 = FALSE;
#അല്ലെങ്കിൽ
    പൂർണ്ണസംഖ്യ		vim9 = in_vim9script();

    /*
     * Recognize a Vim9 script function/method call and assignment:
     * "lvar = value", "lvar(arg)", "[1, 2 3]->Func()"
     */
    p = eap->cmd;
    എങ്കിൽ (lookup != ശൂന്യ)
    {
	char_u *pskip = skip_option_env_lead(eap->cmd);

	എങ്കിൽ (vim_strchr((char_u *)"{('[\"@&$", *p) != ശൂന്യ
	       || ((p = to_name_const_end(pskip)) > eap->cmd && *p != NUL)
	       || (p[0] == '0' && p[1] == 'z'))
	{
	    പൂർണ്ണസംഖ്യ	    oplen;
	    പൂർണ്ണസംഖ്യ	    heredoc;
	    char_u  *swp;

	    എങ്കിൽ (*eap->cmd == '&'
		    || (eap->cmd[0] == '$'
				  && eap->cmd[1] != '\'' && eap->cmd[1] != '"')
		    || (eap->cmd[0] == '@'
					&& (valid_yank_reg(eap->cmd[1], FALSE)
						       || eap->cmd[1] == '@')))
	    {
		എങ്കിൽ (*eap->cmd == '&')
		{
		    p = eap->cmd + 1;
		    എങ്കിൽ (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
			p += 2;
		    p = to_name_end(p, FALSE);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (*eap->cmd == '$')
		    p = to_name_end(eap->cmd + 1, FALSE);
		അല്ലെങ്കിൽ
		    p = eap->cmd + 2;
		എങ്കിൽ (ends_excmd(*skipwhite(p)))
		{
		    // "&option <NL>", "$ENV <NL>" and "@r <NL>" are the start
		    // of an expression.
		    eap->cmdidx = CMD_eval;
		    തിരികെ eap->cmd;
		}
		// "&option" can be followed by "->" or "=", check below
	    }

	    എങ്കിൽ (vim9 && *p == '<')
	    {
		// generic function type args
		എങ്കിൽ (skip_generic_func_type_args(&p) == FAIL)
		{
		    eap->cmdidx = CMD_SIZE;
		    തിരികെ p;
		}
	    }

	    swp = skipwhite(p);

	    എങ്കിൽ (
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
		എങ്കിൽ (*eap->cmd == '{' && ends_excmd(*skipwhite(eap->cmd + 1)))
		{
		    // "{" by itself is the start of a block.
		    eap->cmdidx = CMD_block;
		    തിരികെ eap->cmd + 1;
		}
		eap->cmdidx = CMD_eval;
		തിരികെ eap->cmd;
	    }

	    എങ്കിൽ ((p != eap->cmd && (
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
		എങ്കിൽ (skip_വ്യഞ്ജകം(&after, ശൂന്യ) == OK)
		{
		    after = skipwhite(after);
		    എങ്കിൽ (*after == '=' || (*after != NUL && after[1] == '=')
					 || (after[0] == '.' && after[1] == '.'
							   && after[2] == '='))
			eap->cmdidx = CMD_var;
		}
		--emsg_off;
		തിരികെ eap->cmd;
	    }

	    // "[...]->Method()" is a list expression, but "[a, b] = Func()" is
	    // an assignment.
	    // If there is no line break inside the "[...]" then "p" is
	    // advanced to after the "]" by to_name_const_end(): check if a "="
	    // follows.
	    // If "[...]" has a line break "p" still points at the "[" and it
	    // can't be an assignment.
	    എങ്കിൽ (*eap->cmd == '[')
	    {
		char_u	    *eq;

		p = to_name_const_end(eap->cmd);
		എങ്കിൽ (p == eap->cmd && *p == '[')
		{
		    പൂർണ്ണസംഖ്യ എണ്ണം = 0;
		    പൂർണ്ണസംഖ്യ	semicolon = FALSE;

		    p = skip_var_list(eap->cmd, TRUE, &എണ്ണം, &semicolon, TRUE);
		}
		eq = p;
		എങ്കിൽ (eq != ശൂന്യ)
		{
		    eq = skipwhite(eq);
		    എങ്കിൽ (vim_strchr((char_u *)"+-*/%.", *eq) != ശൂന്യ)
		    {
			എങ്കിൽ (eq[0] == '.' && eq[1] == '.')
			    ++eq;
			++eq;
		    }
		}
		എങ്കിൽ (p == ശൂന്യ || p == eap->cmd || *eq != '=')
		{
		    eap->cmdidx = CMD_eval;
		    തിരികെ eap->cmd;
		}
		എങ്കിൽ (p > eap->cmd && *eq == '=')
		{
		    eap->cmdidx = CMD_var;
		    തിരികെ eap->cmd;
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
	    എങ്കിൽ (oplen > 0)
	    {
		എങ്കിൽ (((p - eap->cmd) > 2 && eap->cmd[1] == ':')
			|| *eap->cmd == '&'
			|| *eap->cmd == '$'
			|| *eap->cmd == '@'
			|| (eap->skip && IS_WHITE_OR_NUL(
						      *(skipwhite(p) + oplen)))
			|| lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK)
		{
		    eap->cmdidx = CMD_var;
		    തിരികെ eap->cmd;
		}
	    }

	    // Recognize trying to use a type for a w:, b:, t: or g: variable:
	    // "w:varname: number = 123".
	    എങ്കിൽ (eap->cmd[1] == ':' && *p == ':')
	    {
		eap->cmdidx = CMD_var;
		തിരികെ eap->cmd;
	    }
	}

	// 1234->func() is a method call
	എങ്കിൽ (സംഖ്യ_method(eap->cmd))
	{
	    eap->cmdidx = CMD_eval;
	    തിരികെ eap->cmd;
	}

	// "g:", "s:" and "l:" are always assumed to be a variable, thus start
	// an expression.  A global/substitute/list command needs to use a
	// longer name.
	എങ്കിൽ (vim_strchr((char_u *)"gsl", *p) != ശൂന്യ && p[1] == ':')
	{
	    eap->cmdidx = CMD_eval;
	    തിരികെ eap->cmd;
	}

	// If it is an ID it might be a variable with an operator on the next
	// line, if the variable exists it can't be an Ex command.
	എങ്കിൽ (p > eap->cmd && ends_excmd(*skipwhite(p))
		&& (lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK
		    || (ASCII_ISALPHA(eap->cmd[0]) && eap->cmd[1] == ':')))
	{
	    eap->cmdidx = CMD_eval;
	    തിരികെ eap->cmd;
	}

	// Check for "++nr" and "--nr".
	എങ്കിൽ (p == eap->cmd && p[0] != NUL && p[0] == p[1]
						   && (*p == '+' || *p == '-'))
	{
	    eap->cmdidx = *p == '+' ? CMD_increment : CMD_decrement;
	    തിരികെ eap->cmd + 2;
	}
    }
#endif

    /*
     * Isolate the command and search for it in the command table.
     */
    p = eap->cmd;
    എങ്കിൽ (one_letter_cmd(p, &eap->cmdidx))
    {
	++p;
	എങ്കിൽ (full != ശൂന്യ)
	    *full = TRUE;
    }
    അല്ലെങ്കിൽ
    {
	ആകുമ്പോൾ (ASCII_ISALPHA(*p))
	    ++p;
	// for python 3.x support ":py3", ":python3", ":py3file", etc.
	എങ്കിൽ (eap->cmd[0] == 'p' && eap->cmd[1] == 'y')
	{
	    ആകുമ്പോൾ (ASCII_ISALNUM(*p))
		++p;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '9' && STRNCMP("vim9", eap->cmd, 4) == 0)
	{
	    // include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	    ++p;
	    ആകുമ്പോൾ (ASCII_ISALPHA(*p))
		++p;
	}

	// check for non-alpha command
	എങ്കിൽ (p == eap->cmd && vim_strchr((char_u *)"@*!=><&~#}", *p) != ശൂന്യ)
	    ++p;
	len = (പൂർണ്ണസംഖ്യ)(p - eap->cmd);
	// The "d" command can directly be followed by 'l' or 'p' flag, when
	// not in Vim9 script.
	എങ്കിൽ (!vim9 && *eap->cmd == 'd' && (p[-1] == 'l' || p[-1] == 'p'))
	{
	    // Check for ":dl", ":dell", etc. to ":deletel": that's
	    // :delete with the 'l' flag.  Same for 'p'.
	    വേണ്ടി (i = 0; i < len; ++i)
		എങ്കിൽ (eap->cmd[i] != ((char_u *)"delete")[i])
		    നിർത്തുക;
	    എങ്കിൽ (i == len - 1)
	    {
		--len;
		എങ്കിൽ (p[-1] == 'l')
		    eap->flags |= EXFLAG_LIST;
		അല്ലെങ്കിൽ
		    eap->flags |= EXFLAG_PRINT;
	    }
	}

	എങ്കിൽ (ASCII_ISLOWER(eap->cmd[0]))
	{
	    പൂർണ്ണസംഖ്യ c1 = eap->cmd[0];
	    പൂർണ്ണസംഖ്യ c2 = len == 1 ? NUL : eap->cmd[1];

	    എങ്കിൽ (command_എണ്ണം != (പൂർണ്ണസംഖ്യ)CMD_SIZE)
	    {
		iemsg(e_command_table_needs_to_be_updated_run_make_cmdidxs);
		getout(1);
	    }

	    // Use a precomputed index for fast look-up in cmdnames[]
	    // taking into account the first 2 letters of eap->cmd.
	    eap->cmdidx = cmdidxs1[Char_Ord_താഴെയുള്ള(c1)];
	    എങ്കിൽ (ASCII_ISLOWER(c2))
		eap->cmdidx += cmdidxs2[Char_Ord_താഴെയുള്ള(c1)][Char_Ord_താഴെയുള്ള(c2)];
	}
	അല്ലെങ്കിൽ എങ്കിൽ (ASCII_ISUPPER(eap->cmd[0]))
	    eap->cmdidx = CMD_Next;
	അല്ലെങ്കിൽ
	    eap->cmdidx = CMD_bang;

	വേണ്ടി ( ; (പൂർണ്ണസംഖ്യ)eap->cmdidx < (പൂർണ്ണസംഖ്യ)CMD_SIZE;
			       eap->cmdidx = (cmdidx_T)((പൂർണ്ണസംഖ്യ)eap->cmdidx + 1))
	    എങ്കിൽ (STRNCMP(cmdnames[(പൂർണ്ണസംഖ്യ)eap->cmdidx].cmd_name, (അക്ഷരം *)eap->cmd,
							    (size_t)len) == 0)
	    {
#ifdef FEAT_EVAL
		എങ്കിൽ (full != ശൂന്യ && cmdnames[eap->cmdidx].cmd_name[len] == NUL)
		    *full = TRUE;
#endif
		നിർത്തുക;
	    }

	// :Print and :mode are not supported in Vim9 script.
	// Some commands cannot be shortened in Vim9 script.
	എങ്കിൽ (vim9 && eap->cmdidx != CMD_SIZE)
	{
	    എങ്കിൽ (eap->cmdidx == CMD_mode || eap->cmdidx == CMD_Print)
		eap->cmdidx = CMD_SIZE;
	    അല്ലെങ്കിൽ എങ്കിൽ ((cmdnames[eap->cmdidx].cmd_argt & EX_WHOLE)
			  && len < (പൂർണ്ണസംഖ്യ)cmdnames[eap->cmdidx].cmd_namelen)
	    {
		semsg(_(e_command_cannot_be_shortened_str), eap->cmd);
		eap->cmdidx = CMD_SIZE;
	    }
	}

	// Do not recognize ":*" as the star command unless '*' is in
	// 'cpoptions'.
	എങ്കിൽ (eap->cmdidx == CMD_star && vim_strchr(p_cpo, CPO_STAR) == ശൂന്യ)
	    p = eap->cmd;

	// Look for a user defined command as a last resort.  Let ":Print" be
	// overruled by a user defined command.
	എങ്കിൽ ((eap->cmdidx == CMD_SIZE || eap->cmdidx == CMD_Print)
		&& *eap->cmd >= 'A' && *eap->cmd <= 'Z')
	{
	    // User defined commands may contain digits.
	    ആകുമ്പോൾ (ASCII_ISALNUM(*p))
		++p;
	    p = find_ucmd(eap, p, full, ശൂന്യ, ശൂന്യ);
	}
	എങ്കിൽ (p == ശൂന്യ || p == eap->cmd)
	    eap->cmdidx = CMD_SIZE;
    }

    // ":fina" means ":finally" in legacy script, for backwards compatibility.
    എങ്കിൽ (eap->cmdidx == CMD_final && p - eap->cmd == 4 && !vim9)
	eap->cmdidx = CMD_finally;

     // Force ":ho" to be unresolved.  Without this, find_ex_command()
     // matches it to CMD_horizontal (the only "ho*" entry), which makes
     // fullcommand("ho") return "horizontal" even though ":ho" cannot be
     // used as the modifier (cmdmods[] requires 3 chars, "hor").
    എങ്കിൽ (eap->cmdidx == CMD_horizontal && p - eap->cmd == 2)
	eap->cmdidx = CMD_SIZE;

#ifdef FEAT_EVAL
    എങ്കിൽ (eap->cmdidx < CMD_SIZE
	    && vim9
	    && !IS_WHITE_NL_OR_NUL(*p) && *p != '!' && *p != '|'
	    && (eap->cmdidx < 0 ||
		(cmdnames[eap->cmdidx].cmd_argt & EX_NONWHITE_OK) == 0))
    {
	// A name that goes on with an underscore is not this command with an
	// argument: "ch_log" is not ":change".  Leave it to be reported as an
	// invalid command.
	എങ്കിൽ (*p != '_')
	{
	    char_u *cmd = vim_strnsave(eap->cmd, p - eap->cmd);

	    semsg(_(e_command_str_not_followed_by_white_space_str), cmd,
								    eap->cmd);
	    vim_free(cmd);
	}
	eap->cmdidx = CMD_SIZE;
    }
#endif

    തിരികെ p;
}

#എങ്കിൽ defined(FEAT_EVAL)
തരനിർവചനം ഘടന
{
    അക്ഷരം	*name;
    പൂർണ്ണസംഖ്യ		minlen;
    പൂർണ്ണസംഖ്യ		has_എണ്ണം;  // :123verbose  :3tab
} cmdmod_info_T;

സ്ഥിതം cmdmod_info_T cmdmod_info_tab[] = {
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
    പൂർണ്ണസംഖ്യ
modifier_len(char_u *cmd)
{
    പൂർണ്ണസംഖ്യ		i, j;
    char_u	*p = cmd;

    എങ്കിൽ (VIM_ISDIGIT(*cmd))
	p = skipwhite(skipdigits(cmd + 1));
    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	// cmdmod_info_tab[] is sorted by name: once the first letter is past
	// the command's first letter no later entry can match.
	എങ്കിൽ (cmdmod_info_tab[i].name[0] > *p)
	    നിർത്തുക;
	വേണ്ടി (j = 0; p[j] != NUL; ++j)
	    എങ്കിൽ (p[j] != cmdmod_info_tab[i].name[j])
		നിർത്തുക;
	എങ്കിൽ (!ASCII_ISALPHA(p[j]) && j >= cmdmod_info_tab[i].minlen
					&& (p == cmd || cmdmod_info_tab[i].has_എണ്ണം))
	    തിരികെ j + (പൂർണ്ണസംഖ്യ)(p - cmd);
    }
    തിരികെ 0;
}

/*
 * Return > 0 if an Ex command "name" exists.
 * Return 2 if there is an exact match.
 * Return 3 if there is an ambiguous match.
 */
    പൂർണ്ണസംഖ്യ
cmd_exists(char_u *name)
{
    exarg_T	ea;
    പൂർണ്ണസംഖ്യ		full = FALSE;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		j;
    char_u	*p;

    // Check command modifiers.
    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	വേണ്ടി (j = 0; name[j] != NUL; ++j)
	    എങ്കിൽ (name[j] != cmdmod_info_tab[i].name[j])
		നിർത്തുക;
	എങ്കിൽ (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	    തിരികെ (cmdmod_info_tab[i].name[j] == NUL ? 2 : 1);
    }

    // Check built-in commands and user defined commands.
    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    p = find_ex_command(&ea, &full, ശൂന്യ, ശൂന്യ);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ 3;
    എങ്കിൽ (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	തിരികെ 0;
    എങ്കിൽ (*skipwhite(p) != NUL)
	തിരികെ 0;	// trailing garbage
    തിരികെ (ea.cmdidx == CMD_SIZE ? 0 : (full ? 2 : 1));
}

/*
 * Add the attributes of an Ex command to "d" in the terms of ":command":
 * "argt" are the EX_ flags, "addr_type" the address type and "def" the
 * default for a range or count, -1 when there is none.
 */
    ശൂന്യം
ex_command_attrs(dict_T *d, long_u argt, cmd_addr_T addr_type, long def)
{
    സ്ഥിതം അക്ഷരം *addr_names[] = {
	"lines", "windows", "arguments", "loaded_buffers", "buffers", "tabs",
	"tabs_relative", "quickfix_valid", "quickfix", "unsigned", "other",
	"none"
    };
    അക്ഷരം	*nargs;

    എങ്കിൽ (!(argt & EX_EXTRA))
	nargs = "0";
    അല്ലെങ്കിൽ എങ്കിൽ (!(argt & EX_NOSPC))
	nargs = (argt & EX_NEEDARG) ? "+" : "*";
    അല്ലെങ്കിൽ എങ്കിൽ (!(argt & EX_NEEDARG))
	nargs = "?";
    അല്ലെങ്കിൽ
	nargs = (argt & EX_ARGSPACE) ? "_" : "1";
    dict_add_string(d, "nargs", (char_u *)nargs);

    എങ്കിൽ (!(argt & EX_RANGE))
	dict_add_bool(d, "range", FALSE);
    അല്ലെങ്കിൽ എങ്കിൽ (argt & EX_DFLALL)
	dict_add_string(d, "range", (char_u *)"%");
    അല്ലെങ്കിൽ എങ്കിൽ (def >= 0 && !(argt & EX_എണ്ണം))
	dict_add_സംഖ്യ(d, "range", def);
    അല്ലെങ്കിൽ
	dict_add_string(d, "range", (char_u *)".");
    എങ്കിൽ (!(argt & EX_എണ്ണം))
	dict_add_bool(d, "count", FALSE);
    അല്ലെങ്കിൽ
	dict_add_സംഖ്യ(d, "count", def < 0 ? 0 : def);
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
    പൂർണ്ണസംഖ്യ
ex_command_info(char_u *name, പൂർണ്ണസംഖ്യ vim9, dict_T *d)
{
    exarg_T	ea;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		j;
    പൂർണ്ണസംഖ്യ		save_cmod_flags = cmdmod.cmod_flags;
    പൂർണ്ണസംഖ്യ		ret = OK;

    എങ്കിൽ (vim9 != -1)
    {
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9 ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    // An abbreviation of a command modifier, like ":bo" for ":botright".
    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	വേണ്ടി (j = 0; name[j] != NUL; ++j)
	    എങ്കിൽ (name[j] != cmdmod_info_tab[i].name[j])
		നിർത്തുക;
	എങ്കിൽ (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	{
	    name = (char_u *)cmdmod_info_tab[i].name;
	    നിർത്തുക;
	}
    }

    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    ea.addr_എണ്ണം = 0;
    // Don't complain about using "en" or ":let" in Vim9 script.
    ++emsg_silent;
    p = find_ex_command(&ea, ശൂന്യ, ശൂന്യ, ശൂന്യ);
    എങ്കിൽ (p == ശൂന്യ || ea.cmdidx == CMD_SIZE
	    || (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	    || *skipwhite(p) != NUL
	    || not_in_vim9(&ea, true) == FAIL)
	ret = FAIL;
    --emsg_silent;

    എങ്കിൽ (ret == FAIL)
	;
    അല്ലെങ്കിൽ എങ്കിൽ (IS_USER_CMDIDX(ea.cmdidx))
	ret = user_command_info(ea.useridx, ea.cmdidx, d);
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	notimpl = cmdnames[ea.cmdidx].cmd_func == ex_ni
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
    തിരികെ ret;
}

/*
 * "fullcommand" function
 */
    ശൂന്യം
f_fullcommand(typval_T *argvars, typval_T *rettv)
{
    exarg_T	ea;
    char_u	*name;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();
    പൂർണ്ണസംഖ്യ		save_cmod_flags = cmdmod.cmod_flags;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	തിരികെ;

    name = tv_get_string(&argvars[0]);
    എങ്കിൽ (name == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	vim9script = tv_get_bool(&argvars[1]);
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9script ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    ആകുമ്പോൾ (*name == ':')
	name++;
    name = skip_range(name, TRUE, ശൂന്യ);

    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.addr_എണ്ണം = 0;
    ++emsg_silent;  // don't complain about using "en" in Vim9 script
    p = find_ex_command(&ea, ശൂന്യ, ശൂന്യ, ശൂന്യ);
    --emsg_silent;
    എങ്കിൽ (p == ശൂന്യ || ea.cmdidx == CMD_SIZE)
	പോകുക theend;

    എങ്കിൽ (vim9script && not_in_vim9(&ea, true) == FAIL)
	പോകുക theend;

    rettv->vval.v_string = vim_strsave(IS_USER_CMDIDX(ea.cmdidx)
				 ? get_user_command_name(ea.useridx, ea.cmdidx)
				 : cmdnames[ea.cmdidx].cmd_name);
theend:
    cmdmod.cmod_flags = save_cmod_flags;
}
#endif

    cmdidx_T
excmd_get_cmdidx(char_u *cmd, പൂർണ്ണസംഖ്യ len)
{
    cmdidx_T idx;

    എങ്കിൽ (!one_letter_cmd(cmd, &idx))
	വേണ്ടി (idx = (cmdidx_T)0; (പൂർണ്ണസംഖ്യ)idx < (പൂർണ്ണസംഖ്യ)CMD_SIZE;
		idx = (cmdidx_T)((പൂർണ്ണസംഖ്യ)idx + 1))
	    എങ്കിൽ (STRNCMP(cmdnames[(പൂർണ്ണസംഖ്യ)idx].cmd_name, cmd, (size_t)len) == 0)
		നിർത്തുക;

    തിരികെ idx;
}

    long
excmd_get_argt(cmdidx_T idx)
{
    തിരികെ (long)cmdnames[(പൂർണ്ണസംഖ്യ)idx].cmd_argt;
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
    പൂർണ്ണസംഖ്യ		skip_star,	// skip "*" used for Visual range
    പൂർണ്ണസംഖ്യ		*ctx)		// pointer to xp_context or NULL
{
    char_u	*cmd = cmd_start;
    unsigned	delim;

    ആകുമ്പോൾ (vim_strchr((char_u *)" \t0123456789.$%'/?-+,;\\", *cmd) != ശൂന്യ)
    {
	എങ്കിൽ (*cmd == '\\')
	{
	    എങ്കിൽ (cmd[1] == '?' || cmd[1] == '/' || cmd[1] == '&')
		++cmd;
	    അല്ലെങ്കിൽ
		നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*cmd == '\'')
	{
	    char_u *p = cmd;

	    // a quote is only valid at the start or after a separator
	    ആകുമ്പോൾ (p > cmd_start)
	    {
		--p;
		എങ്കിൽ (!VIM_ISWHITE(*p))
		    നിർത്തുക;
	    }
	    എങ്കിൽ (cmd > cmd_start && !VIM_ISWHITE(*p) && *p != ',' && *p != ';')
		നിർത്തുക;
	    എങ്കിൽ (*++cmd == NUL && ctx != ശൂന്യ)
		*ctx = EXPAND_NOTHING;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*cmd == '/' || *cmd == '?')
	{
	    delim = *cmd++;
	    ആകുമ്പോൾ (*cmd != NUL && *cmd != delim)
		എങ്കിൽ (*cmd++ == '\\' && *cmd != NUL)
		    ++cmd;
	    എങ്കിൽ (*cmd == NUL && ctx != ശൂന്യ)
		*ctx = EXPAND_NOTHING;
	}
	എങ്കിൽ (*cmd != NUL)
	    ++cmd;
    }

    // Skip ":" and white space.
    ആകുമ്പോൾ (*cmd == ':')
	cmd = skipwhite(cmd + 1);

    // Skip "*" used for Visual range.
    എങ്കിൽ (skip_star && *cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == ശൂന്യ)
	cmd = skipwhite(cmd + 1);

    തിരികെ cmd;
}

    സ്ഥിതം ശൂന്യം
addr_error(cmd_addr_T addr_type)
{
    എങ്കിൽ (addr_type == ADDR_NONE)
	emsg(_(e_no_range_allowed));
    അല്ലെങ്കിൽ
	emsg(_(e_invalid_range));
}

/*
 * Return the default address for an address type.
 */
    സ്ഥിതം linenr_T
default_address(exarg_T *eap)
{
    linenr_T lnum = 0;

    തിരഞ്ഞെടുപ്പ് (eap->addr_type)
    {
	അവസ്ഥ ADDR_വരികൾ:
	അവസ്ഥ ADDR_OTHER:
	    // Default is the cursor line number.  Avoid using an invalid
	    // line number though.
	    എങ്കിൽ (curwin->w_cursor.lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
		lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
	    അല്ലെങ്കിൽ
		lnum = curwin->w_cursor.lnum;
	    നിർത്തുക;
	അവസ്ഥ ADDR_WINDOWS:
	    lnum = CURRENT_WIN_NR;
	    നിർത്തുക;
	അവസ്ഥ ADDR_ARGUMENTS:
	    lnum = curwin->w_arg_idx + 1;
	    എങ്കിൽ (lnum > ARGCOUNT)
		lnum = ARGCOUNT;
	    നിർത്തുക;
	അവസ്ഥ ADDR_LOADED_BUFFERS:
	അവസ്ഥ ADDR_BUFFERS:
	    lnum = curbuf->b_fnum;
	    നിർത്തുക;
	അവസ്ഥ ADDR_TABS:
	    lnum = CURRENT_TAB_NR;
	    നിർത്തുക;
	അവസ്ഥ ADDR_TABS_RELATIVE:
	അവസ്ഥ ADDR_UNSIGNED:
	    lnum = 1;
	    നിർത്തുക;
	അവസ്ഥ ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_idx(eap);
#endif
	    നിർത്തുക;
	അവസ്ഥ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_valid_idx(eap);
#endif
	    നിർത്തുക;
	അവസ്ഥ ADDR_NONE:
	    // Will give an error later if a range is found.
	    നിർത്തുക;
    }
    തിരികെ lnum;
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
    പൂർണ്ണസംഖ്യ		skip,		// only skip the address, don't use it
    പൂർണ്ണസംഖ്യ		silent,		// no errors or side effects
    പൂർണ്ണസംഖ്യ		to_other_file,  // flag: may jump to other file
    പൂർണ്ണസംഖ്യ		address_എണ്ണം UNUSED) // 1 for first address, >1 after comma
{
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		i;
    long	n;
    char_u	*cmd;
    pos_T	pos;
    pos_T	*fp;
    linenr_T	lnum;
    buf_T	*buf;

    cmd = skipwhite(*ptr);
    lnum = MAXLNUM;
    ചെയ്യുക
    {
	തിരഞ്ഞെടുപ്പ് (*cmd)
	{
	    അവസ്ഥ '.':			    // '.' - Cursor position
		++cmd;
		തിരഞ്ഞെടുപ്പ് (addr_type)
		{
		    അവസ്ഥ ADDR_വരികൾ:
		    അവസ്ഥ ADDR_OTHER:
			lnum = curwin->w_cursor.lnum;
			നിർത്തുക;
		    അവസ്ഥ ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			നിർത്തുക;
		    അവസ്ഥ ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			നിർത്തുക;
		    അവസ്ഥ ADDR_LOADED_BUFFERS:
		    അവസ്ഥ ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			നിർത്തുക;
		    അവസ്ഥ ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			നിർത്തുക;
		    അവസ്ഥ ADDR_NONE:
		    അവസ്ഥ ADDR_TABS_RELATIVE:
		    അവസ്ഥ ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = ശൂന്യ;
			പോകുക error;
			നിർത്തുക;
		    അവസ്ഥ ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			നിർത്തുക;
		    അവസ്ഥ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			നിർത്തുക;
		}
		നിർത്തുക;

	    അവസ്ഥ '$':			    // '$' - last line
		++cmd;
		തിരഞ്ഞെടുപ്പ് (addr_type)
		{
		    അവസ്ഥ ADDR_വരികൾ:
		    അവസ്ഥ ADDR_OTHER:
			lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
			നിർത്തുക;
		    അവസ്ഥ ADDR_WINDOWS:
			lnum = LAST_WIN_NR;
			നിർത്തുക;
		    അവസ്ഥ ADDR_ARGUMENTS:
			lnum = ARGCOUNT;
			നിർത്തുക;
		    അവസ്ഥ ADDR_LOADED_BUFFERS:
			buf = lastbuf;
			ആകുമ്പോൾ (buf->b_ml.ml_mfp == ശൂന്യ)
			{
			    എങ്കിൽ (buf->b_prev == ശൂന്യ)
				നിർത്തുക;
			    buf = buf->b_prev;
			}
			lnum = buf->b_fnum;
			നിർത്തുക;
		    അവസ്ഥ ADDR_BUFFERS:
			lnum = lastbuf->b_fnum;
			നിർത്തുക;
		    അവസ്ഥ ADDR_TABS:
			lnum = LAST_TAB_NR;
			നിർത്തുക;
		    അവസ്ഥ ADDR_NONE:
		    അവസ്ഥ ADDR_TABS_RELATIVE:
		    അവസ്ഥ ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = ശൂന്യ;
			പോകുക error;
			നിർത്തുക;
		    അവസ്ഥ ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_size(eap);
			എങ്കിൽ (lnum == 0)
			    lnum = 1;
#endif
			നിർത്തുക;
		    അവസ്ഥ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_valid_size(eap);
			എങ്കിൽ (lnum == 0)
			    lnum = 1;
#endif
			നിർത്തുക;
		}
		നിർത്തുക;

	    അവസ്ഥ '\'':			    // ''' - mark
		എങ്കിൽ (*++cmd == NUL)
		{
		    cmd = ശൂന്യ;
		    പോകുക error;
		}
		എങ്കിൽ (addr_type != ADDR_വരികൾ)
		{
		    addr_error(addr_type);
		    cmd = ശൂന്യ;
		    പോകുക error;
		}
		എങ്കിൽ (skip)
		    ++cmd;
		അല്ലെങ്കിൽ
		{
		    // Only accept a mark in another file when it is
		    // used by itself: ":'M".
		    fp = getmark(*cmd, to_other_file && cmd[1] == NUL);
		    ++cmd;
		    എങ്കിൽ (fp == (pos_T *)-1)
			// Jumped to another file.
			lnum = curwin->w_cursor.lnum;
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (check_mark(fp) == FAIL)
			{
			    cmd = ശൂന്യ;
			    പോകുക error;
			}
			lnum = fp->lnum;
		    }
		}
		നിർത്തുക;

	    അവസ്ഥ '/':
	    അവസ്ഥ '?':			// '/' or '?' - search
		c = *cmd++;
		എങ്കിൽ (addr_type != ADDR_വരികൾ)
		{
		    addr_error(addr_type);
		    cmd = ശൂന്യ;
		    പോകുക error;
		}
		എങ്കിൽ (skip)	// skip "/pat/"
		{
		    cmd = skip_regexp(cmd, c, magic_isset());
		    എങ്കിൽ (*cmd == c)
			++cmd;
		}
		അല്ലെങ്കിൽ
		{
		    പൂർണ്ണസംഖ്യ flags;

		    pos = curwin->w_cursor; // save curwin->w_cursor

		    // When '/' or '?' follows another address, start from
		    // there.
		    എങ്കിൽ (lnum > 0 && lnum != MAXLNUM)
			curwin->w_cursor.lnum =
				lnum > curbuf->b_ml.ml_വരി_എണ്ണം
					   ? curbuf->b_ml.ml_വരി_എണ്ണം : lnum;

		    // Start a forward search at the end of the line (unless
		    // before the first line).
		    // Start a backward search at the start of the line.
		    // This makes sure we never match in the current
		    // line, and can match anywhere in the
		    // next/previous line.
		    എങ്കിൽ (c == '/' && curwin->w_cursor.lnum > 0)
			curwin->w_cursor.col = MAXCOL;
		    അല്ലെങ്കിൽ
			curwin->w_cursor.col = 0;
		    searchcmdlen = 0;
		    flags = silent ? തിരയൽ_KEEP : തിരയൽ_HIS | തിരയൽ_സന്ദേശം;
		    എങ്കിൽ (!do_തിരയൽ(ശൂന്യ, c, c, cmd, STRLEN(cmd), 1L, flags, ശൂന്യ))
		    {
			curwin->w_cursor = pos;
			cmd = ശൂന്യ;
			പോകുക error;
		    }
		    lnum = curwin->w_cursor.lnum;
		    curwin->w_cursor = pos;
		    // adjust command string pointer
		    cmd += searchcmdlen;
		}
		നിർത്തുക;

	    അവസ്ഥ '\\':		    // "\?", "\/" or "\&", repeat search
		++cmd;
		എങ്കിൽ (addr_type != ADDR_വരികൾ)
		{
		    addr_error(addr_type);
		    cmd = ശൂന്യ;
		    പോകുക error;
		}
		എങ്കിൽ (*cmd == '&')
		    i = RE_SUBST;
		അല്ലെങ്കിൽ എങ്കിൽ (*cmd == '?' || *cmd == '/')
		    i = RE_തിരയൽ;
		അല്ലെങ്കിൽ
		{
		    emsg(_(e_backslash_should_be_followed_by));
		    cmd = ശൂന്യ;
		    പോകുക error;
		}

		എങ്കിൽ (!skip)
		{
		    /*
		     * When search follows another address, start from
		     * there.
		     */
		    എങ്കിൽ (lnum != MAXLNUM)
			pos.lnum = lnum;
		    അല്ലെങ്കിൽ
			pos.lnum = curwin->w_cursor.lnum;

		    /*
		     * Start the search just like for the above
		     * do_search().
		     */
		    എങ്കിൽ (*cmd != '?')
			pos.col = MAXCOL;
		    അല്ലെങ്കിൽ
			pos.col = 0;
		    pos.coladd = 0;
		    എങ്കിൽ (searchit(curwin, curbuf, &pos, ശൂന്യ,
				*cmd == '?' ? BACKWARD : FORWARD,
				(char_u *)"", 0, 1L, തിരയൽ_സന്ദേശം, i, ശൂന്യ) != FAIL)
			lnum = pos.lnum;
		    അല്ലെങ്കിൽ
		    {
			cmd = ശൂന്യ;
			പോകുക error;
		    }
		}
		++cmd;
		നിർത്തുക;

	    സ്ഥിരസ്ഥിതി:
		എങ്കിൽ (VIM_ISDIGIT(*cmd))	// absolute line number
		    lnum = getdigits(&cmd);
	}

	വേണ്ടി (;;)
	{
	    cmd = skipwhite(cmd);
	    എങ്കിൽ (*cmd != '-' && *cmd != '+' && !VIM_ISDIGIT(*cmd))
		നിർത്തുക;

	    എങ്കിൽ (lnum == MAXLNUM)
	    {
		തിരഞ്ഞെടുപ്പ് (addr_type)
		{
		    അവസ്ഥ ADDR_വരികൾ:
		    അവസ്ഥ ADDR_OTHER:
			// "+1" is same as ".+1"
			lnum = curwin->w_cursor.lnum;
			നിർത്തുക;
		    അവസ്ഥ ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			നിർത്തുക;
		    അവസ്ഥ ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			നിർത്തുക;
		    അവസ്ഥ ADDR_LOADED_BUFFERS:
		    അവസ്ഥ ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			നിർത്തുക;
		    അവസ്ഥ ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			നിർത്തുക;
		    അവസ്ഥ ADDR_TABS_RELATIVE:
			lnum = 1;
			നിർത്തുക;
		    അവസ്ഥ ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			നിർത്തുക;
		    അവസ്ഥ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			നിർത്തുക;
		    അവസ്ഥ ADDR_NONE:
		    അവസ്ഥ ADDR_UNSIGNED:
			lnum = 0;
			നിർത്തുക;
		}
	    }

	    എങ്കിൽ (VIM_ISDIGIT(*cmd))
		i = '+';		// "number" is same as "+number"
	    അല്ലെങ്കിൽ
		i = *cmd++;
	    എങ്കിൽ (!VIM_ISDIGIT(*cmd))	// '+' is '+1'
		n = 1;
	    അല്ലെങ്കിൽ
	    {
		// "number", "+number" or "-number"
		n = getdigits(&cmd);
		എങ്കിൽ (n == MAXLNUM)
		{
		    emsg(_(e_വരി_സംഖ്യ_out_of_range));
		    cmd = ശൂന്യ;
		    പോകുക error;
		}
	    }

	    എങ്കിൽ (addr_type == ADDR_TABS_RELATIVE)
	    {
		emsg(_(e_invalid_range));
		cmd = ശൂന്യ;
		പോകുക error;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (addr_type == ADDR_LOADED_BUFFERS
		    || addr_type == ADDR_BUFFERS)
		lnum = compute_buffer_local_എണ്ണം(
				    addr_type, lnum, (i == '-') ? -1 * n : n);
	    അല്ലെങ്കിൽ
	    {
#ifdef FEAT_FOLDING
		// Relative line addressing: need to adjust for lines in a
		// closed fold after the first address.
		എങ്കിൽ (addr_type == ADDR_വരികൾ && (i == '-' || i == '+')
							 && address_എണ്ണം >= 2)
		    (ശൂന്യം)hasFolding(lnum, ശൂന്യ, &lnum);
#endif
		എങ്കിൽ (i == '-')
		    lnum -= n;
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (lnum >= 0 && n >= LONG_MAX - lnum)
		    {
			emsg(_(e_വരി_സംഖ്യ_out_of_range));
			cmd = ശൂന്യ;
			പോകുക error;
		    }
		    lnum += n;
		}
	    }
	}
    } ആകുമ്പോൾ (*cmd == '/' || *cmd == '?');

error:
    *ptr = cmd;
    തിരികെ lnum;
}

/*
 * Set eap->line1 and eap->line2 to the whole range.
 * Used for commands with the EX_DFLALL flag and no range given.
 */
    സ്ഥിതം ശൂന്യം
address_default_all(exarg_T *eap)
{
    eap->line1 = 1;
    തിരഞ്ഞെടുപ്പ് (eap->addr_type)
    {
	അവസ്ഥ ADDR_വരികൾ:
	അവസ്ഥ ADDR_OTHER:
	    eap->line2 = curbuf->b_ml.ml_വരി_എണ്ണം;
	    നിർത്തുക;
	അവസ്ഥ ADDR_LOADED_BUFFERS:
	    {
		buf_T *buf = firstbuf;

		ആകുമ്പോൾ (buf->b_next != ശൂന്യ && buf->b_ml.ml_mfp == ശൂന്യ)
		    buf = buf->b_next;
		eap->line1 = buf->b_fnum;
		buf = lastbuf;
		ആകുമ്പോൾ (buf->b_prev != ശൂന്യ && buf->b_ml.ml_mfp == ശൂന്യ)
		    buf = buf->b_prev;
		eap->line2 = buf->b_fnum;
	    }
	    നിർത്തുക;
	അവസ്ഥ ADDR_BUFFERS:
	    eap->line1 = firstbuf->b_fnum;
	    eap->line2 = lastbuf->b_fnum;
	    നിർത്തുക;
	അവസ്ഥ ADDR_WINDOWS:
	    eap->line2 = LAST_WIN_NR;
	    നിർത്തുക;
	അവസ്ഥ ADDR_TABS:
	    eap->line2 = LAST_TAB_NR;
	    നിർത്തുക;
	അവസ്ഥ ADDR_TABS_RELATIVE:
	    eap->line2 = 1;
	    നിർത്തുക;
	അവസ്ഥ ADDR_ARGUMENTS:
	    എങ്കിൽ (ARGCOUNT == 0)
		eap->line1 = eap->line2 = 0;
	    അല്ലെങ്കിൽ
		eap->line2 = ARGCOUNT;
	    നിർത്തുക;
	അവസ്ഥ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    eap->line2 = qf_get_valid_size(eap);
	    എങ്കിൽ (eap->line2 == 0)
		eap->line2 = 1;
#endif
	    നിർത്തുക;
	അവസ്ഥ ADDR_NONE:
	അവസ്ഥ ADDR_UNSIGNED:
	അവസ്ഥ ADDR_QUICKFIX:
	    iemsg("Cannot use EX_DFLALL with ADDR_NONE, ADDR_UNSIGNED or ADDR_QUICKFIX");
	    നിർത്തുക;
    }
}


/*
 * Get flags from an Ex command argument.
 */
    സ്ഥിതം ശൂന്യം
get_flags(exarg_T *eap)
{
    ആകുമ്പോൾ (vim_strchr((char_u *)"lp#", *eap->arg) != ശൂന്യ)
    {
	എങ്കിൽ (*eap->arg == 'l')
	    eap->flags |= EXFLAG_LIST;
	അല്ലെങ്കിൽ എങ്കിൽ (*eap->arg == 'p')
	    eap->flags |= EXFLAG_PRINT;
	അല്ലെങ്കിൽ
	    eap->flags |= EXFLAG_NR;
	eap->arg = skipwhite(eap->arg + 1);
    }
}

/*
 * Function called for command which is Not Implemented.  NI!
 */
    ശൂന്യം
ex_ni(exarg_T *eap)
{
    എങ്കിൽ (!eap->skip)
	eap->errmsg =
		_(e_sorry_command_is_not_available_in_this_version);
}

#ifdef HAVE_EX_SCRIPT_NI
/*
 * Function called for script command which is Not Implemented.  NI!
 * Skips over ":perl <<EOF" constructs.
 */
    സ്ഥിതം ശൂന്യം
ex_script_ni(exarg_T *eap)
{
    എങ്കിൽ (!eap->skip)
	ex_ni(eap);
    അല്ലെങ്കിൽ
	vim_free(script_get(eap, eap->arg));
}
#endif

/*
 * Check range in Ex command for validity.
 * Return NULL when valid, error message when invalid.
 */
    സ്ഥിതം അക്ഷരം *
invalid_range(exarg_T *eap)
{
    buf_T	*buf;

    എങ്കിൽ (       eap->line1 < 0
	    || eap->line2 < 0
	    || eap->line1 > eap->line2)
	തിരികെ _(e_invalid_range);

    എങ്കിൽ (eap->argt & EX_RANGE)
    {
	തിരഞ്ഞെടുപ്പ് (eap->addr_type)
	{
	    അവസ്ഥ ADDR_വരികൾ:
		എങ്കിൽ (eap->line2 > curbuf->b_ml.ml_വരി_എണ്ണം
#ifdef FEAT_DIFF
			    + (eap->cmdidx == CMD_diffget ||
				eap->cmdidx == CMD_diffput)
#endif
		   )
		    തിരികെ _(e_invalid_range);
		നിർത്തുക;
	    അവസ്ഥ ADDR_ARGUMENTS:
		// add 1 if ARGCOUNT is 0
		എങ്കിൽ (eap->line2 > ARGCOUNT + (!ARGCOUNT))
		    തിരികെ _(e_invalid_range);
		നിർത്തുക;
	    അവസ്ഥ ADDR_BUFFERS:
		// Only a boundary check, not whether the buffers actually
		// exist.
		എങ്കിൽ (eap->line1 < 1 || eap->line2 > get_highest_fnum())
		    തിരികെ _(e_invalid_range);
		നിർത്തുക;
	    അവസ്ഥ ADDR_LOADED_BUFFERS:
		buf = firstbuf;
		ആകുമ്പോൾ (buf->b_ml.ml_mfp == ശൂന്യ)
		{
		    എങ്കിൽ (buf->b_next == ശൂന്യ)
			തിരികെ _(e_invalid_range);
		    buf = buf->b_next;
		}
		എങ്കിൽ (eap->line1 < buf->b_fnum)
		    തിരികെ _(e_invalid_range);
		buf = lastbuf;
		ആകുമ്പോൾ (buf->b_ml.ml_mfp == ശൂന്യ)
		{
		    എങ്കിൽ (buf->b_prev == ശൂന്യ)
			തിരികെ _(e_invalid_range);
		    buf = buf->b_prev;
		}
		എങ്കിൽ (eap->line2 > buf->b_fnum)
		    തിരികെ _(e_invalid_range);
		നിർത്തുക;
	    അവസ്ഥ ADDR_WINDOWS:
		എങ്കിൽ (eap->line2 > LAST_WIN_NR)
		    തിരികെ _(e_invalid_range);
		നിർത്തുക;
	    അവസ്ഥ ADDR_TABS:
		എങ്കിൽ (eap->line2 > LAST_TAB_NR)
		    തിരികെ _(e_invalid_range);
		നിർത്തുക;
	    അവസ്ഥ ADDR_TABS_RELATIVE:
	    അവസ്ഥ ADDR_OTHER:
		// Any range is OK.
		നിർത്തുക;
	    അവസ്ഥ ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
		// No error for value that is too big, will use the last entry.
		എങ്കിൽ (eap->line2 <= 0)
		{
		    എങ്കിൽ (eap->addr_എണ്ണം == 0)
			തിരികെ _(e_no_errors);
		    തിരികെ _(e_invalid_range);
		}
#endif
		നിർത്തുക;
	    അവസ്ഥ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
		എങ്കിൽ ((eap->line2 != 1 && eap->line2 > qf_get_valid_size(eap))
			|| eap->line2 < 0)
		    തിരികെ _(e_invalid_range);
#endif
		നിർത്തുക;
	    അവസ്ഥ ADDR_UNSIGNED:
	    അവസ്ഥ ADDR_NONE:
		// Will give an error elsewhere.
		നിർത്തുക;
	}
    }
    തിരികെ ശൂന്യ;
}

/*
 * Correct the range for zero line number, if required.
 */
    സ്ഥിതം ശൂന്യം
correct_range(exarg_T *eap)
{
    എങ്കിൽ (!(eap->argt & EX_ZEROR))	    // zero in range not allowed
    {
	എങ്കിൽ (eap->line1 == 0)
	    eap->line1 = 1;
	എങ്കിൽ (eap->line2 == 0)
	    eap->line2 = 1;
    }
}

#ifdef FEAT_QUICKFIX
/*
 * For a ":vimgrep" or ":vimgrepadd" command return a pointer past the
 * pattern.  Otherwise return eap->arg.
 */
    സ്ഥിതം char_u *
skip_grep_pat(exarg_T *eap)
{
    char_u	*p = eap->arg;

    എങ്കിൽ (*p != NUL && (eap->cmdidx == CMD_vimgrep || eap->cmdidx == CMD_lvimgrep
		|| eap->cmdidx == CMD_vimgrepadd
		|| eap->cmdidx == CMD_lvimgrepadd
		|| grep_internal(eap->cmdidx)))
    {
	p = skip_vimgrep_pat(p, ശൂന്യ, ശൂന്യ);
	എങ്കിൽ (p == ശൂന്യ)
	    p = eap->arg;
    }
    തിരികെ p;
}

/*
 * For the ":make" and ":grep" commands insert the 'makeprg'/'grepprg' option
 * in the command line, so that things like % get expanded.
 */
    സ്ഥിതം char_u *
replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep)
{
    char_u	*new_cmdline;
    char_u	*program;
    char_u	*pos;
    char_u	*ptr;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		i;

    /*
     * Don't do it when ":vimgrep" is used for ":grep".
     */
    എങ്കിൽ ((eap->cmdidx == CMD_make || eap->cmdidx == CMD_lmake
		     || eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
		     || eap->cmdidx == CMD_grepadd
		     || eap->cmdidx == CMD_lgrepadd)
	    && !grep_internal(eap->cmdidx))
    {
	എങ്കിൽ (eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
	    || eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	{
	    എങ്കിൽ (*curbuf->b_p_gp == NUL)
		program = p_gp;
	    അല്ലെങ്കിൽ
		program = curbuf->b_p_gp;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (*curbuf->b_p_mp == NUL)
		program = p_mp;
	    അല്ലെങ്കിൽ
		program = curbuf->b_p_mp;
	}

	p = skipwhite(p);

	എങ്കിൽ ((pos = (char_u *)strstr((അക്ഷരം *)program, "$*")) != ശൂന്യ)
	{
	    // replace $* by given arguments
	    i = 1;
	    ആകുമ്പോൾ ((pos = (char_u *)strstr((അക്ഷരം *)pos + 2, "$*")) != ശൂന്യ)
		++i;
	    len = (പൂർണ്ണസംഖ്യ)STRLEN(p);
	    new_cmdline = alloc(STRLEN(program) + (size_t)i * (len - 2) + 1);
	    എങ്കിൽ (new_cmdline == ശൂന്യ)
		തിരികെ ശൂന്യ;			// out of memory
	    ptr = new_cmdline;
	    ആകുമ്പോൾ ((pos = (char_u *)strstr((അക്ഷരം *)program, "$*")) != ശൂന്യ)
	    {
		i = (പൂർണ്ണസംഖ്യ)(pos - program);
		STRNCPY(ptr, program, i);
		STRCPY(ptr += i, p);
		ptr += len;
		program = pos + 2;
	    }
	    STRCPY(ptr, program);
	}
	അല്ലെങ്കിൽ
	{
	    size_t program_len = STRLEN(program);

	    new_cmdline = alloc(program_len + STRLEN(p) + 2);
	    എങ്കിൽ (new_cmdline == ശൂന്യ)
		തിരികെ ശൂന്യ;			// out of memory
	    STRCPY(new_cmdline, program);
	    STRCPY(new_cmdline + program_len, " ");
	    STRCPY(new_cmdline + program_len + 1, p);
	}
	സന്ദേശം_make(p);

	// 'eap->cmd' is not set here, because it is not used at CMD_make
	vim_free(*cmdlinep);
	*cmdlinep = new_cmdline;
	p = new_cmdline;
    }
    തിരികെ p;
}
#endif

/*
 * Expand file name in Ex command argument.
 * When an error is detected, "errormsgp" is set to a non-NULL pointer.
 * Return FAIL for failure, OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
expand_filename(
    exarg_T	*eap,
    char_u	**cmdlinep,
    അക്ഷരം	**errormsgp)
{
    പൂർണ്ണസംഖ്യ		has_wildcards;	// need to expand wildcards
    char_u	*repl;
    size_t	srclen;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		escaped;

#ifdef FEAT_QUICKFIX
    // Skip a regexp pattern for ":vimgrep[add] pat file..."
    p = skip_grep_pat(eap);
#അല്ലെങ്കിൽ
    p = eap->arg;
#endif

    /*
     * Decide to expand wildcards *before* replacing '%', '#', etc.  If
     * the file name contains a wildcard it should not cause expanding.
     * (it will be expanded anyway if there is a wildcard before replacing).
     */
    has_wildcards = mch_has_wildcard(p);
    ആകുമ്പോൾ (*p != NUL)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`, wildcards in it are not expanded.
	എങ്കിൽ (p[0] == '`' && p[1] == '=')
	{
	    p += 2;
	    (ശൂന്യം)skip_വ്യഞ്ജകം(&p, ശൂന്യ);
	    എങ്കിൽ (*p == '`')
		++p;
	    തുടരുക;
	}
#endif
	/*
	 * Quick check if this cannot be the start of a special string.
	 * Also removes backslash before '%', '#' and '<'.
	 */
	എങ്കിൽ (vim_strchr((char_u *)"%#<", *p) == ശൂന്യ)
	{
	    ++p;
	    തുടരുക;
	}

	/*
	 * Try to find a match at this position.
	 */
	repl = eval_vars(p, eap->arg, &srclen, &(eap->do_ecmd_lnum),
						    errormsgp, &escaped, TRUE);
	എങ്കിൽ (*errormsgp != ശൂന്യ)		// error detected
	    തിരികെ FAIL;
	എങ്കിൽ (repl == ശൂന്യ)		// no match found
	{
	    p += srclen;
	    തുടരുക;
	}

	// Wildcards won't be expanded below, the replacement is taken
	// literally.  But do expand "~/file", "~user/file" and "$HOME/file".
	എങ്കിൽ (vim_strchr(repl, '$') != ശൂന്യ || vim_strchr(repl, '~') != ശൂന്യ)
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
	എങ്കിൽ (!eap->usefilter
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
	    സ്ഥിതം char_u *nobslash = (char_u *)" \t\"|";
# define ESCAPE_CHARS nobslash
#അല്ലെങ്കിൽ
# define ESCAPE_CHARS escape_chars
#endif

	    വേണ്ടി (l = repl; *l; ++l)
		എങ്കിൽ (vim_strchr(ESCAPE_CHARS, *l) != ശൂന്യ)
		{
		    l = vim_strsave_escaped(repl, ESCAPE_CHARS);
		    എങ്കിൽ (l != ശൂന്യ)
		    {
			vim_free(repl);
			repl = l;
		    }
		    നിർത്തുക;
		}
	}

	// For a shell command a '!' must be escaped.
	എങ്കിൽ ((eap->usefilter || eap->cmdidx == CMD_bang
						|| eap->cmdidx == CMD_terminal)
			    && vim_strpbrk(repl, (char_u *)"!") != ശൂന്യ)
	{
	    char_u	*l;

	    l = vim_strsave_escaped(repl, (char_u *)"!");
	    എങ്കിൽ (l != ശൂന്യ)
	    {
		vim_free(repl);
		repl = l;
	    }
	}

	p = repl_cmdline(eap, p, srclen, repl, cmdlinep);
	vim_free(repl);
	എങ്കിൽ (p == ശൂന്യ)
	    തിരികെ FAIL;
    }

    /*
     * One file argument: Expand wildcards.
     * Don't do this with ":r !command" or ":w !command".
     */
    എങ്കിൽ ((eap->argt & EX_NOSPC) && !eap->usefilter)
    {
	/*
	 * May do this twice:
	 * 1. Replace environment variables.
	 * 2. Replace any other wildcards, remove backslashes.
	 */
	വേണ്ടി (n = 1; n <= 2; ++n)
	{
	    എങ്കിൽ (n == 2)
	    {
		/*
		 * Halve the number of backslashes (this is Vi compatible).
		 * For Unix and OS/2, when wildcards are expanded, this is
		 * done by ExpandOne() below.
		 */
#എങ്കിൽ defined(UNIX)
		എങ്കിൽ (!has_wildcards)
#endif
		    backslash_halve(eap->arg);
	    }

	    എങ്കിൽ (has_wildcards)
	    {
		എങ്കിൽ (n == 1)
		{
		    /*
		     * First loop: May expand environment variables.  This
		     * can be done much faster with expand_env() than with
		     * something else (e.g., calling a shell).
		     * After expanding environment variables, check again
		     * if there are still wildcards present.
		     */
		    എങ്കിൽ (vim_strchr(eap->arg, '$') != ശൂന്യ
			    || vim_strchr(eap->arg, '~') != ശൂന്യ)
		    {
			expand_env_esc(eap->arg, NameBuff, MAXPATHL,
					(char_u *)(" \t" PATH_ESC_WILDCARDS),
					TRUE, ശൂന്യ);
			has_wildcards = mch_has_wildcard(NameBuff);
			p = NameBuff;
		    }
		    അല്ലെങ്കിൽ
			p = ശൂന്യ;
		}
		അല്ലെങ്കിൽ // n == 2
		{
		    expand_T	xpc;
		    പൂർണ്ണസംഖ്യ		options = WILD_LIST_NOTFOUND
					       | WILD_NOERROR | WILD_ADD_SLASH;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    എങ്കിൽ (p_wic)
			options += WILD_ICASE;
		    p = ExpandOne(&xpc, eap->arg, ശൂന്യ,
						   options, WILD_EXPAND_FREE);
		    എങ്കിൽ (p == ശൂന്യ)
			തിരികെ FAIL;
		}
		എങ്കിൽ (p != ശൂന്യ)
		{
		    (ശൂന്യം)repl_cmdline(eap, eap->arg, STRLEN(eap->arg),
								 p, cmdlinep);
		    എങ്കിൽ (n == 2)	// p came from ExpandOne()
			vim_free(p);
		}
	    }
	}
    }
    തിരികെ OK;
}

/*
 * Replace part of the command line, keeping eap->cmd, eap->arg and
 * eap->nextcmd correct.
 * "src" points to the part that is to be replaced, of length "srclen".
 * "repl" is the replacement string.
 * Returns a pointer to the character after the replaced string.
 * Returns NULL for failure.
 */
    സ്ഥിതം char_u *
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
    എങ്കിൽ (eap->nextcmd != ശൂന്യ)
	i += STRLEN(eap->nextcmd);	// add space for next command
    എങ്കിൽ ((new_cmdline = alloc(i)) == ശൂന്യ)
	തിരികെ ശൂന്യ;			// out of memory!

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

    എങ്കിൽ (eap->nextcmd != ശൂന്യ)		// append next command
    {
	new_cmdlinelen += taillen + 1;
	STRCPY(new_cmdline + new_cmdlinelen, eap->nextcmd);
	eap->nextcmd = new_cmdline + new_cmdlinelen;
    }
    eap->cmd = new_cmdline + (eap->cmd - *cmdlinep);
    eap->arg = new_cmdline + (eap->arg - *cmdlinep);
    എങ്കിൽ (eap->do_ecmd_cmd != ശൂന്യ && eap->do_ecmd_cmd != dollar_command)
	eap->do_ecmd_cmd = new_cmdline + (eap->do_ecmd_cmd - *cmdlinep);
    vim_free(*cmdlinep);
    *cmdlinep = new_cmdline;

    തിരികെ src;
}

/*
 * Check for '|' to separate commands and '"' to start comments.
 * If "keep_backslash" is TRUE do not remove any backslash.
 */
    ശൂന്യം
separate_nextcmd(exarg_T *eap, പൂർണ്ണസംഖ്യ keep_backslash)
{
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();
    char_u	*p;

#ifdef FEAT_QUICKFIX
    p = skip_grep_pat(eap);
#അല്ലെങ്കിൽ
    p = eap->arg;
#endif

    വേണ്ടി ( ; *p; MB_PTR_ADV(p))
    {
	എങ്കിൽ (*p == Ctrl_V)
	{
	    എങ്കിൽ ((eap->argt & (EX_CTRLV | EX_XFILE)) || keep_backslash)
		++p;		// skip CTRL-V and next char
	    അല്ലെങ്കിൽ
				// remove CTRL-V and skip next char
		STRMOVE(p, p + 1);
	    എങ്കിൽ (*p == NUL)		// stop at NUL after CTRL-V
		നിർത്തുക;
	}

#ifdef FEAT_EVAL
	// Skip over `=expr` when wildcards are expanded.
	അല്ലെങ്കിൽ എങ്കിൽ (p[0] == '`' && p[1] == '=' && (eap->argt & EX_XFILE))
	{
	    p += 2;
	    (ശൂന്യം)skip_വ്യഞ്ജകം(&p, ശൂന്യ);
	    എങ്കിൽ (*p == NUL)		// stop at NUL after CTRL-V
		നിർത്തുക;
	}
#endif

	// Check for '"'/'#': start of comment or '|': next command
	// :@" and :*" do not start a comment!
	// :redir @" doesn't either.
	അല്ലെങ്കിൽ എങ്കിൽ ((*p == '"'
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
		    && eap->cmdidx != CMD_തിരുകുക)
		|| *p == '\n')
	{
	    /*
	     * We remove the '\' before the '|', unless EX_CTRLV is used
	     * AND 'b' is present in 'cpoptions'.
	     */
	    എങ്കിൽ ((vim_strchr(p_cpo, CPO_BAR) == ശൂന്യ
			      || !(eap->argt & EX_CTRLV)) && *(p - 1) == '\\')
	    {
		എങ്കിൽ (!keep_backslash)
		{
		    STRMOVE(p - 1, p);	// remove the '\'
		    --p;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		set_nextcmd(eap, p);
		*p = NUL;
		നിർത്തുക;
	    }
	}
    }

    എങ്കിൽ (!(eap->argt & EX_NOTRLCOM))	// remove trailing spaces
	del_trailing_spaces(eap->arg);
}

/*
 * get + command from ex argument
 */
    സ്ഥിതം char_u *
getargcmd(char_u **argp)
{
    char_u *arg = *argp;
    char_u *command = ശൂന്യ;

    എങ്കിൽ (*arg == '+')	    // +[command]
    {
	++arg;
	എങ്കിൽ (vim_isspace(*arg) || *arg == NUL)
	    command = dollar_command;
	അല്ലെങ്കിൽ
	{
	    command = arg;
	    arg = skip_cmd_arg(command, TRUE);
	    എങ്കിൽ (*arg != NUL)
		*arg++ = NUL;		// terminate command with NUL
	}

	arg = skipwhite(arg);	// skip over spaces
	*argp = arg;
    }
    തിരികെ command;
}

/*
 * Find end of "+command" argument.  Skip over "\ " and "\\".
 */
    char_u *
skip_cmd_arg(
    char_u *p,
    പൂർണ്ണസംഖ്യ	   rembs)	// TRUE to halve the number of backslashes
{
    ആകുമ്പോൾ (*p && !vim_isspace(*p))
    {
	എങ്കിൽ (*p == '\\' && p[1] != NUL)
	{
	    എങ്കിൽ (rembs)
		STRMOVE(p, p + 1);
	    അല്ലെങ്കിൽ
		++p;
	}
	MB_PTR_ADV(p);
    }
    തിരികെ p;
}

    പൂർണ്ണസംഖ്യ
get_bad_opt(char_u *p, exarg_T *eap)
{
    എങ്കിൽ (STRICMP(p, "keep") == 0)
	eap->bad_char = BAD_KEEP;
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(p, "drop") == 0)
	eap->bad_char = BAD_DROP;
    അല്ലെങ്കിൽ എങ്കിൽ (MB_BYTE2LEN(*p) == 1 && p[1] == NUL)
	eap->bad_char = *p;
    അല്ലെങ്കിൽ
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of bad= names.
 */
    സ്ഥിതം char_u *
get_bad_name(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    // Note: Keep this in sync with getargopt.
    സ്ഥിതം അക്ഷരം *(p_bad_മൂല്യങ്ങൾ[]) =
    {
	"?",
	"keep",
	"drop",
    };

    എങ്കിൽ (idx < 0 || idx >= (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(p_bad_മൂല്യങ്ങൾ))
	തിരികെ ശൂന്യ;

    തിരികെ (char_u*)p_bad_മൂല്യങ്ങൾ[idx];
}

/*
 * Get "++opt=arg" argument.
 * Return FAIL or OK.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
getargopt(exarg_T *eap)
{
    char_u	*arg = eap->arg + 2;
    പൂർണ്ണസംഖ്യ		*pp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		bad_char_idx;
    char_u	*p;

    // Note: Keep this in sync with get_argopt_name.

    // ":edit ++[no]bin[ary] file"
    എങ്കിൽ (STRNCMP(arg, "bin", 3) == 0 || STRNCMP(arg, "nobin", 5) == 0)
    {
	എങ്കിൽ (*arg == 'n')
	{
	    arg += 2;
	    eap->force_bin = FORCE_NOBIN;
	}
	അല്ലെങ്കിൽ
	    eap->force_bin = FORCE_BIN;
	എങ്കിൽ (!checkforcmd(&arg, "binary", 3))
	    തിരികെ FAIL;
	eap->arg = skipwhite(arg);
	തിരികെ OK;
    }

    // ":read ++edit file"
    എങ്കിൽ (STRNCMP(arg, "edit", 4) == 0)
    {
	eap->read_edit = TRUE;
	eap->arg = skipwhite(arg + 4);
	തിരികെ OK;
    }

    എങ്കിൽ (STRNCMP(arg, "ff", 2) == 0)
    {
	arg += 2;
	pp = &eap->force_ff;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "fileformat", 10) == 0)
    {
	arg += 10;
	pp = &eap->force_ff;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "enc", 3) == 0)
    {
	എങ്കിൽ (STRNCMP(arg, "encoding", 8) == 0)
	    arg += 8;
	അല്ലെങ്കിൽ
	    arg += 3;
	pp = &eap->force_enc;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "bad", 3) == 0)
    {
	arg += 3;
	pp = &bad_char_idx;
    }

    എങ്കിൽ (pp == ശൂന്യ || *arg != '=')
	തിരികെ FAIL;

    ++arg;
    *pp = (പൂർണ്ണസംഖ്യ)(arg - eap->cmd);
    arg = skip_cmd_arg(arg, FALSE);
    eap->arg = skipwhite(arg);
    *arg = NUL;

    എങ്കിൽ (pp == &eap->force_ff)
    {
	എങ്കിൽ (check_ff_മൂല്യം(eap->cmd + eap->force_ff) == FAIL)
	    തിരികെ FAIL;
	eap->force_ff = eap->cmd[eap->force_ff];
    }
    അല്ലെങ്കിൽ എങ്കിൽ (pp == &eap->force_enc)
    {
	// Make 'fileencoding' lower case.
	വേണ്ടി (p = eap->cmd + eap->force_enc; *p != NUL; ++p)
	    *p = TOLOWER_ASC(*p);
    }
    അല്ലെങ്കിൽ
    {
	// Check ++bad= argument.  Must be a single-byte character, "keep" or
	// "drop".
	എങ്കിൽ (get_bad_opt(eap->cmd + bad_char_idx, eap) == FAIL)
	    തിരികെ FAIL;
    }

    തിരികെ OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of ++opt names.
 */
    സ്ഥിതം char_u *
get_argopt_name(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    // Note: Keep this in sync with getargopt.
    സ്ഥിതം അക്ഷരം *(p_opt_മൂല്യങ്ങൾ[]) =
    {
	"fileformat=",
	"encoding=",
	"binary",
	"nobinary",
	"bad=",
	"edit",
    };

    എങ്കിൽ (idx < 0 || idx >= (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(p_opt_മൂല്യങ്ങൾ))
	തിരികെ ശൂന്യ;

    തിരികെ (char_u*)p_opt_മൂല്യങ്ങൾ[idx];
}

/*
 * Command-line expansion for ++opt=name.
 */
    പൂർണ്ണസംഖ്യ
expand_argopt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	പൂർണ്ണസംഖ്യ	    *അക്കം_Matches)
{
    എങ്കിൽ (xp->xp_pattern > xp->xp_വരി && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, പൂർണ്ണസംഖ്യ) = ശൂന്യ;

	char_u *name_end = xp->xp_pattern - 1;
	എങ്കിൽ (name_end - xp->xp_വരി >= 2
		&& STRNCMP(name_end - 2, "ff", 2) == 0)
	    cb = get_fileformat_name;
	അല്ലെങ്കിൽ എങ്കിൽ (name_end - xp->xp_വരി >= 10
		&& STRNCMP(name_end - 10, "fileformat", 10) == 0)
	    cb = get_fileformat_name;
	അല്ലെങ്കിൽ എങ്കിൽ (name_end - xp->xp_വരി >= 3
		&& STRNCMP(name_end - 3, "enc", 3) == 0)
	    cb = get_encoding_name;
	അല്ലെങ്കിൽ എങ്കിൽ (name_end - xp->xp_വരി >= 8
		&& STRNCMP(name_end - 8, "encoding", 8) == 0)
	    cb = get_encoding_name;
	അല്ലെങ്കിൽ എങ്കിൽ (name_end - xp->xp_വരി >= 3
		&& STRNCMP(name_end - 3, "bad", 3) == 0)
	    cb = get_bad_name;

	എങ്കിൽ (cb != ശൂന്യ)
	{
	    തിരികെ ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    അക്കം_Matches,
		    cb,
		    FALSE);
	}
	തിരികെ FAIL;
    }

    // Special handling of "ff" which acts as a short form of
    // "fileformat", as "ff" is not a substring of it.
    എങ്കിൽ (xp->xp_pattern_len == 2
	    && STRNCMP(xp->xp_pattern, "ff", xp->xp_pattern_len) == 0)
    {
	*matches = ALLOC_MULT(char_u *, 1);
	എങ്കിൽ (*matches == ശൂന്യ)
	    തിരികെ FAIL;
	*അക്കം_Matches = 1;
	(*matches)[0] = vim_strsave((char_u*)"fileformat=");
	തിരികെ OK;
    }

    തിരികെ ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    അക്കം_Matches,
	    get_argopt_name,
	    FALSE);
}

    സ്ഥിതം ശൂന്യം
ex_autocmd(exarg_T *eap)
{
    എങ്കിൽ (eap->skip)
    {
	skip_cmd_block(eap);
	തിരികെ;
    }

    /*
     * Disallow autocommands from .exrc and .vimrc in current
     * directory for security reasons.
     */
    എങ്കിൽ (secure)
    {
	secure = 2;
	eap->errmsg =
	      _(e_command_not_allowed_from_vimrc_in_current_dir_or_tag_തിരയൽ);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_autocmd)
	do_autocmd(eap, eap->arg, eap->forceit);
    അല്ലെങ്കിൽ
	do_augroup(eap->arg, eap->forceit);
}

/*
 * ":doautocmd": Apply the automatic commands to the current buffer.
 */
    സ്ഥിതം ശൂന്യം
ex_doautocmd(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    പൂർണ്ണസംഖ്യ		call_do_modelines = check_nomodeline(&arg);
    പൂർണ്ണസംഖ്യ		did_aucmd;

    (ശൂന്യം)do_doautocmd(arg, TRUE, &did_aucmd);
    // Only when there is no <nomodeline>.
    എങ്കിൽ (call_do_modelines && did_aucmd)
	do_modelines(0);
}

/*
 * :[N]bunload[!] [N] [bufname] unload buffer
 * :[N]bdelete[!] [N] [bufname] delete buffer from buffer list
 * :[N]bwipeout[!] [N] [bufname] delete buffer really
 */
    സ്ഥിതം ശൂന്യം
ex_bunload(exarg_T *eap)
{
    എങ്കിൽ (ERROR_IF_ANY_POPUP_WINDOW)
	തിരികെ;
    eap->errmsg = do_bufdel(
	    eap->cmdidx == CMD_bdelete ? DOBUF_DEL
		: eap->cmdidx == CMD_bwipeout ? DOBUF_WIPE
		: DOBUF_UNLOAD, eap->arg,
	    eap->addr_എണ്ണം, (പൂർണ്ണസംഖ്യ)eap->line1, (പൂർണ്ണസംഖ്യ)eap->line2, eap->forceit);
}

/*
 * :[N]buffer [N]	to buffer N
 * :[N]sbuffer [N]	to buffer N
 */
    സ്ഥിതം ശൂന്യം
ex_buffer(exarg_T *eap)
{
    എങ്കിൽ (ERROR_IF_ANY_POPUP_WINDOW)
	തിരികെ;
    do_exbuffer(eap);
}

/*
 * ":buffer" command and alike.
 */
    സ്ഥിതം ശൂന്യം
do_exbuffer(exarg_T *eap)
{
    എങ്കിൽ (*eap->arg)
	eap->errmsg = ex_errmsg(e_trailing_characters_str, eap->arg);
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (eap->addr_എണ്ണം == 0)	// default is current buffer
	    goto_buffer(eap, DOBUF_CURRENT, FORWARD, 0);
	അല്ലെങ്കിൽ
	    goto_buffer(eap, DOBUF_FIRST, FORWARD, (പൂർണ്ണസംഖ്യ)eap->line2);
	എങ്കിൽ (eap->do_ecmd_cmd != ശൂന്യ)
	    do_cmd_argument(eap->do_ecmd_cmd);
    }
}

/*
 * :[N]bmodified [N]	to next mod. buffer
 * :[N]sbmodified [N]	to next mod. buffer
 */
    സ്ഥിതം ശൂന്യം
ex_bmodified(exarg_T *eap)
{
    goto_buffer(eap, DOBUF_MOD, FORWARD, (പൂർണ്ണസംഖ്യ)eap->line2);
    എങ്കിൽ (eap->do_ecmd_cmd != ശൂന്യ)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bnext [N]	to next buffer
 * :[N]sbnext [N]	split and to next buffer
 */
    സ്ഥിതം ശൂന്യം
ex_bnext(exarg_T *eap)
{
    എങ്കിൽ (ERROR_IF_ANY_POPUP_WINDOW)
	തിരികെ;

    goto_buffer(eap, DOBUF_CURRENT, FORWARD, (പൂർണ്ണസംഖ്യ)eap->line2);
    എങ്കിൽ (eap->do_ecmd_cmd != ശൂന്യ)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bNext [N]	to previous buffer
 * :[N]bprevious [N]	to previous buffer
 * :[N]sbNext [N]	split and to previous buffer
 * :[N]sbprevious [N]	split and to previous buffer
 */
    സ്ഥിതം ശൂന്യം
ex_bprevious(exarg_T *eap)
{
    എങ്കിൽ (ERROR_IF_ANY_POPUP_WINDOW)
	തിരികെ;

    goto_buffer(eap, DOBUF_CURRENT, BACKWARD, (പൂർണ്ണസംഖ്യ)eap->line2);
    എങ്കിൽ (eap->do_ecmd_cmd != ശൂന്യ)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :brewind		to first buffer
 * :bfirst		to first buffer
 * :sbrewind		split and to first buffer
 * :sbfirst		split and to first buffer
 */
    സ്ഥിതം ശൂന്യം
ex_brewind(exarg_T *eap)
{
    എങ്കിൽ (ERROR_IF_ANY_POPUP_WINDOW)
	തിരികെ;

    goto_buffer(eap, DOBUF_FIRST, FORWARD, 0);
    എങ്കിൽ (eap->do_ecmd_cmd != ശൂന്യ)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :blast		to last buffer
 * :sblast		split and to last buffer
 */
    സ്ഥിതം ശൂന്യം
ex_blast(exarg_T *eap)
{
    എങ്കിൽ (ERROR_IF_ANY_POPUP_WINDOW)
	തിരികെ;

    goto_buffer(eap, DOBUF_LAST, BACKWARD, 0);
    എങ്കിൽ (eap->do_ecmd_cmd != ശൂന്യ)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * Check if "c" ends an Ex command.
 * In Vim9 script does not check for white space before #.
 */
    പൂർണ്ണസംഖ്യ
ends_excmd(പൂർണ്ണസംഖ്യ c)
{
    പൂർണ്ണസംഖ്യ comment_char = '"';

    എങ്കിൽ (in_vim9script())
	comment_char = '#';
    തിരികെ (c == NUL || c == '|' || c == comment_char || c == '\n');
}

/*
 * Like ends_excmd() but checks that a # in Vim9 script either has "cmd" equal
 * to "cmd_start" or has a white space character before it.
 */
    പൂർണ്ണസംഖ്യ
ends_excmd2(char_u *cmd_start UNUSED, char_u *cmd)
{
    പൂർണ്ണസംഖ്യ c = *cmd;

    എങ്കിൽ (c == NUL || c == '|' || c == '\n')
	തിരികെ TRUE;
    എങ്കിൽ (in_vim9script())
	//  # starts a comment, #{ might be a mistake, #{{ can start a fold
	തിരികെ c == '#' && (cmd[1] != '{' || cmd[2] == '{')
				 && (cmd == cmd_start || VIM_ISWHITE(cmd[-1]));
    തിരികെ c == '"';
}

#എങ്കിൽ defined(FEAT_SYN_HL) || defined(FEAT_തിരയൽ_EXTRA) || defined(FEAT_EVAL)
/*
 * Return the next command, after the first '|' or '\n'.
 * Return NULL if not found.
 */
    char_u *
find_nextcmd(char_u *p)
{
    ആകുമ്പോൾ (*p != '|' && *p != '\n')
    {
	എങ്കിൽ (*p == NUL)
	    തിരികെ ശൂന്യ;
	++p;
    }
    തിരികെ (p + 1);
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

    എങ്കിൽ (*s == '|' || *s == '\n')
	തിരികെ (s + 1);
    അല്ലെങ്കിൽ
	തിരികെ ശൂന്യ;
}

/*
 * If "eap->nextcmd" is not set, check for a next command at "p".
 */
    ശൂന്യം
set_nextcmd(exarg_T *eap, char_u *arg)
{
    char_u *p = skipwhite(arg);

    എങ്കിൽ (in_vim9script() && *p == '#')
    {
	char_u *nl = vim_strchr(p, NL);
	എങ്കിൽ (nl != ശൂന്യ)
	    p = nl;
    }

    p = check_nextcmd(p);

    എങ്കിൽ (eap->nextcmd == ശൂന്യ)
	eap->nextcmd = p;
    അല്ലെങ്കിൽ എങ്കിൽ (p != ശൂന്യ)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_more(
    പൂർണ്ണസംഖ്യ message,	    // when FALSE check only, no messages
    പൂർണ്ണസംഖ്യ forceit)
{
    പൂർണ്ണസംഖ്യ	    n = ARGCOUNT - curwin->w_arg_idx - 1;

    എങ്കിൽ (!forceit && only_one_window()
	    && ARGCOUNT > 1 && !arg_had_last && n > 0 && quitmore == 0)
    {
	എങ്കിൽ (message)
	{
#എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    എങ്കിൽ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						    && curbuf->b_fname != ശൂന്യ)
	    {
		char_u	buff[DIALOG_സന്ദേശം_SIZE];

		vim_snprintf((അക്ഷരം *)buff, DIALOG_സന്ദേശം_SIZE,
			NGETTEXT("%d more file to edit.  Quit anyway?",
			    "%d more files to edit.  Quit anyway?", n), n);
		എങ്കിൽ (vim_dialog_yesno(VIM_QUESTION, ശൂന്യ, buff, 1) == VIM_YES)
		    തിരികെ OK;
		തിരികെ FAIL;
	    }
#endif
	    semsg(NGETTEXT(e_nr_more_file_to_edit,
			   e_nr_more_files_to_edit , n), n);
	    quitmore = 2;	    // next try to quit is allowed
	}
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of command names.
 */
    char_u *
get_command_name(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (idx >= (പൂർണ്ണസംഖ്യ)CMD_SIZE)
	തിരികെ expand_user_command_name(idx);
    // the following are no real commands
    എങ്കിൽ (STRNCMP(cmdnames[idx].cmd_name, "{", 1) == 0 ||
	STRNCMP(cmdnames[idx].cmd_name, "}", 1) == 0)
	തിരികെ (char_u *)"";
    തിരികെ cmdnames[idx].cmd_name;
}

    സ്ഥിതം ശൂന്യം
ex_colorscheme(exarg_T *eap)
{
    എങ്കിൽ (*eap->arg == NUL)
    {
#ifdef FEAT_EVAL
	char_u *വ്യഞ്ജകം = vim_strsave((char_u *)"g:colors_name");
	char_u *p = ശൂന്യ;

	എങ്കിൽ (വ്യഞ്ജകം != ശൂന്യ)
	{
	    ++emsg_off;
	    p = eval_to_string(വ്യഞ്ജകം, FALSE, FALSE);
	    --emsg_off;
	    vim_free(വ്യഞ്ജകം);
	}
	എങ്കിൽ (p != ശൂന്യ)
	{
	    സന്ദേശം((അക്ഷരം *)p);
	    vim_free(p);
	}
	അല്ലെങ്കിൽ
	    സന്ദേശം("default");
#അല്ലെങ്കിൽ
	സന്ദേശം(_("unknown"));
#endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ (load_colors(eap->arg) == FAIL)
	semsg(_(e_cannot_find_color_scheme_str), eap->arg);

#ifdef FEAT_VTP
    അല്ലെങ്കിൽ എങ്കിൽ (has_vtp_working())
    {
	// background color change requires clear + redraw
	update_screen(UPD_CLEAR);
	redrawcmd();
    }
#endif
}

    സ്ഥിതം ശൂന്യം
ex_highlight(exarg_T *eap)
{
    എങ്കിൽ (*eap->arg == NUL && eap->cmd[2] == '!')
	സന്ദേശം(_("Greetings, Vim user!"));
    do_highlight(eap->arg, eap->forceit, FALSE);
}


/*
 * Call this function if we thought we were going to exit, but we won't
 * (because of an error).  May need to restore the terminal mode.
 */
    ശൂന്യം
not_exiting(പൂർണ്ണസംഖ്യ save_exiting)
{
    exiting = save_exiting;
    settmode(TMODE_RAW);
}

    പൂർണ്ണസംഖ്യ
before_quit_autocmds(win_T *wp, പൂർണ്ണസംഖ്യ quit_all, പൂർണ്ണസംഖ്യ forceit)
{
    apply_autocmds(EVENT_QUITPRE, ശൂന്യ, ശൂന്യ, FALSE, wp->w_buffer);

    // Bail out when autocommands closed the window.
    // Refuse to quit when the buffer in the last window is being closed (can
    // only happen in autocommands).
    എങ്കിൽ (!win_valid(wp)
	    || curbuf_locked()
	    || (wp->w_buffer->b_nwindows == 1 && wp->w_buffer->b_locked > 0))
	തിരികെ TRUE;

    എങ്കിൽ (quit_all || (check_more(FALSE, forceit) == OK && only_one_window()))
    {
	apply_autocmds(EVENT_EXITPRE, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	// Refuse to quit when locked or when the window was closed or the
	// buffer in the last window is being closed (can only happen in
	// autocommands).
	എങ്കിൽ (!win_valid(wp) || curbuf_locked()
			  || (curbuf->b_nwindows == 1 && curbuf->b_locked > 0))
	    തിരികെ TRUE;
    }

    തിരികെ FALSE;
}

/*
 * ":quit": quit current window, quit Vim if the last window is closed.
 * ":{nr}quit": quit window {nr}
 * Also used when closing a terminal window that's the last one.
 */
    ശൂന്യം
ex_quit(exarg_T *eap)
{
    win_T	*wp;

    എങ്കിൽ (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	തിരികെ;
    }
    // Don't quit while editing the command line.
    എങ്കിൽ (text_locked())
    {
	text_locked_സന്ദേശം();
	തിരികെ;
    }
    എങ്കിൽ (eap->addr_എണ്ണം > 0)
    {
	പൂർണ്ണസംഖ്യ	wnr = eap->line2;

	വേണ്ടി (wp = firstwin; wp->w_next != ശൂന്യ; wp = wp->w_next)
	    എങ്കിൽ (--wnr <= 0)
		നിർത്തുക;
    }
    അല്ലെങ്കിൽ
	wp = curwin;

    // Refuse to quit when locked.
    എങ്കിൽ (curbuf_locked())
	തിരികെ;

    // Trigger QuitPre and maybe ExitPre
    എങ്കിൽ (before_quit_autocmds(wp, FALSE, eap->forceit))
	തിരികെ;

#ifdef FEAT_NETBEANS_INTG
    netbeansForcedQuit = eap->forceit;
#endif

    പൂർണ്ണസംഖ്യ save_exiting = exiting;
    /*
     * If there is only one relevant window we will exit.
     */
    എങ്കിൽ (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;
    എങ്കിൽ ((!buf_hide(wp->w_buffer)
		&& check_changed(wp->w_buffer, (p_awa ? CCGD_AW : 0)
				       | (eap->forceit ? CCGD_FORCEIT : 0)
				       | CCGD_EXCMD))
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, TRUE)))
    {
	not_exiting(save_exiting);
    }
    അല്ലെങ്കിൽ
    {
	// quit last window
	// Note: only_one_window() returns true, even so a help window is
	// still open. In that case only quit, if no address has been
	// specified. Example:
	// :h|wincmd w|1q     - don't quit
	// :h|wincmd w|q      - quit
	എങ്കിൽ (only_one_window() && (ONE_WINDOW || eap->addr_എണ്ണം == 0))
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
    സ്ഥിതം ശൂന്യം
ex_cquit(exarg_T *eap UNUSED)
{
    // this does not always pass on the exit code to the Manx compiler. why?
    getout(eap->addr_എണ്ണം > 0 ? (പൂർണ്ണസംഖ്യ)eap->line2 : EXIT_FAILURE);
}

/*
 * Do preparations for "qall" and "wqall".
 * Returns FAIL when quitting should be aborted.
 */
    പൂർണ്ണസംഖ്യ
before_quit_all(exarg_T *eap)
{
    എങ്കിൽ (cmdwin_type != 0)
    {
	എങ്കിൽ (eap->forceit)
	    cmdwin_result = K_XF1;	// ex_window() takes care of this
	അല്ലെങ്കിൽ
	    cmdwin_result = K_XF2;
	തിരികെ FAIL;
    }

    // Don't quit while editing the command line.
    എങ്കിൽ (text_locked())
    {
	text_locked_സന്ദേശം();
	തിരികെ FAIL;
    }

    എങ്കിൽ (before_quit_autocmds(curwin, TRUE, eap->forceit))
	തിരികെ FAIL;

    തിരികെ OK;
}

/*
 * ":qall": try to quit all windows
 */
    സ്ഥിതം ശൂന്യം
ex_quit_all(exarg_T *eap)
{
    എങ്കിൽ (before_quit_all(eap) == FAIL)
	തിരികെ;
    പൂർണ്ണസംഖ്യ save_exiting = exiting;
    exiting = TRUE;
    എങ്കിൽ (eap->forceit || !check_changed_any(FALSE, FALSE))
	getout(0);
    not_exiting(save_exiting);
}

/*
 * ":close": close current window, unless it is the last one
 */
    സ്ഥിതം ശൂന്യം
ex_close(exarg_T *eap)
{
    win_T	*win;
    പൂർണ്ണസംഖ്യ		winnr = 0;
    എങ്കിൽ (cmdwin_type != 0)
	cmdwin_result = Ctrl_C;
    അല്ലെങ്കിൽ
	എങ്കിൽ (!text_locked() && !curbuf_locked())
	{
	    എങ്കിൽ (eap->addr_എണ്ണം == 0)
		ex_win_close(eap->forceit, curwin, ശൂന്യ);
	    അല്ലെങ്കിൽ
	    {
		FOR_ALL_WINDOWS(win)
		{
		    winnr++;
		    എങ്കിൽ (winnr == eap->line2)
			നിർത്തുക;
		}
		എങ്കിൽ (win == ശൂന്യ)
		    win = lastwin;
		ex_win_close(eap->forceit, win, ശൂന്യ);
	    }
	}
}

#ifdef FEAT_QUICKFIX
/*
 * ":pclose": Close any preview window.
 */
    സ്ഥിതം ശൂന്യം
ex_pclose(exarg_T *eap)
{
    win_T	*win;

    // First close any normal window.
    FOR_ALL_WINDOWS(win)
	എങ്കിൽ (win->w_p_pvw)
	{
	    ex_win_close(eap->forceit, win, ശൂന്യ);
	    തിരികെ;
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
    സ്ഥിതം ശൂന്യം
ex_win_close(
    പൂർണ്ണസംഖ്യ		forceit,
    win_T	*win,
    tabpage_T	*tp)		// NULL or the tab page "win" is in
{
    പൂർണ്ണസംഖ്യ		need_hide;
    buf_T	*buf = win->w_buffer;

    // Never close the autocommand window.
    എങ്കിൽ (is_aucmd_win(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	തിരികെ;
    }
    എങ്കിൽ (window_layout_locked(CMD_close))
	തിരികെ;

    need_hide = (bufIsChanged(buf) && buf->b_nwindows <= 1);
    എങ്കിൽ (need_hide && !buf_hide(buf) && !forceit)
    {
#എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	എങ്കിൽ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    എങ്കിൽ (term_job_running(buf->b_term))
	    {
		എങ്കിൽ (term_confirm_stop(buf) == FAIL)
		    തിരികെ;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(buf);
		need_hide = FALSE;
	    }
	    അല്ലെങ്കിൽ
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(buf, FALSE);
		എങ്കിൽ (bufref_valid(&bufref) && bufIsChanged(buf))
		    തിരികെ;
		need_hide = FALSE;
	    }
	}
	അല്ലെങ്കിൽ
#endif
	{
	    no_write_message();
	    തിരികെ;
	}
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // free buffer when not hiding it or when it's a scratch buffer
    എങ്കിൽ (tp == ശൂന്യ)
	win_close(win, !need_hide && !buf_hide(buf));
    അല്ലെങ്കിൽ
	win_close_othertab(win, !need_hide && !buf_hide(buf), tp);
}

/*
 * Handle the argument for a tabpage related ex command.
 * Returns a tabpage number.
 * When an error is encountered then eap->errmsg is set.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_tabpage_arg(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ tab_സംഖ്യ;
    പൂർണ്ണസംഖ്യ unaccept_arg0 = (eap->cmdidx == CMD_tabmove) ? 0 : 1;

    എങ്കിൽ (eap->arg && *eap->arg != NUL)
    {
	char_u *p = eap->arg;
	char_u *p_save;
	പൂർണ്ണസംഖ്യ    relative = 0; // argument +N/-N means: go to N places to the
			     // right/left relative to the current position.

	എങ്കിൽ (*p == '-')
	{
	    relative = -1;
	    p++;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '+')
	{
	    relative = 1;
	    p++;
	}

	p_save = p;
	tab_സംഖ്യ = getdigits(&p);

	എങ്കിൽ (relative == 0)
	{
	    എങ്കിൽ (STRCMP(p, "$") == 0)
		tab_സംഖ്യ = LAST_TAB_NR;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(p, "#") == 0)
		എങ്കിൽ (valid_tabpage(lastused_tabpage))
		    tab_സംഖ്യ = tabpage_index(lastused_tabpage);
		അല്ലെങ്കിൽ
		{
		    eap->errmsg = ex_errmsg(e_invalid_മൂല്യം_for_argument_str,
								     eap->arg);
		    tab_സംഖ്യ = 0;
		    പോകുക theend;
		}
	    അല്ലെങ്കിൽ എങ്കിൽ (p == p_save || *p_save == '-' || *p != NUL
		    || tab_സംഖ്യ > LAST_TAB_NR)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		പോകുക theend;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (*p_save == NUL)
		tab_സംഖ്യ = 1;
	    അല്ലെങ്കിൽ എങ്കിൽ (p == p_save || *p_save == '-' || *p != NUL
		    || tab_സംഖ്യ == 0)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		പോകുക theend;
	    }
	    tab_സംഖ്യ = tab_സംഖ്യ * relative + tabpage_index(curtab);
	    എങ്കിൽ (!unaccept_arg0 && relative == -1)
		--tab_സംഖ്യ;
	}
	എങ്കിൽ (tab_സംഖ്യ < unaccept_arg0 || tab_സംഖ്യ > LAST_TAB_NR)
	    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (eap->addr_എണ്ണം > 0)
    {
	എങ്കിൽ (unaccept_arg0 && eap->line2 == 0)
	{
	    eap->errmsg = _(e_invalid_range);
	    tab_സംഖ്യ = 0;
	}
	അല്ലെങ്കിൽ
	{
	    tab_സംഖ്യ = eap->line2;
	    എങ്കിൽ (!unaccept_arg0)
	    {
		char_u *cmdp = eap->cmd;

		ആകുമ്പോൾ (--cmdp > *eap->cmdlinep
			&& (VIM_ISWHITE(*cmdp) || VIM_ISDIGIT(*cmdp)))
		    ;
		എങ്കിൽ (*cmdp == '-')
		{
		    --tab_സംഖ്യ;
		    എങ്കിൽ (tab_സംഖ്യ < unaccept_arg0)
			eap->errmsg = _(e_invalid_range);
		}
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	തിരഞ്ഞെടുപ്പ് (eap->cmdidx)
	{
	അവസ്ഥ CMD_tabnext:
	    tab_സംഖ്യ = tabpage_index(curtab) + 1;
	    എങ്കിൽ (tab_സംഖ്യ > LAST_TAB_NR)
		tab_സംഖ്യ = 1;
	    നിർത്തുക;
	അവസ്ഥ CMD_tabmove:
	    tab_സംഖ്യ = LAST_TAB_NR;
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    tab_സംഖ്യ = tabpage_index(curtab);
	}
    }

theend:
    തിരികെ tab_സംഖ്യ;
}

/*
 * ":tabclose": close current tab page, unless it is the last one.
 * ":tabclose N": close tab page N.
 */
    സ്ഥിതം ശൂന്യം
ex_tabclose(exarg_T *eap)
{
    tabpage_T	*tp;
    പൂർണ്ണസംഖ്യ		tab_സംഖ്യ;

    എങ്കിൽ (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	തിരികെ;
    }

    എങ്കിൽ (first_tabpage->tp_next == ശൂന്യ)
    {
	emsg(_(e_cannot_close_last_tab_page));
	തിരികെ;
    }

    എങ്കിൽ (window_layout_locked(CMD_tabclose))
	തിരികെ;

    tab_സംഖ്യ = get_tabpage_arg(eap);
    എങ്കിൽ (eap->errmsg != ശൂന്യ)
	തിരികെ;

    tp = find_tabpage(tab_സംഖ്യ);
    എങ്കിൽ (tp == ശൂന്യ)
    {
	beep_flush();
	തിരികെ;
    }
    എങ്കിൽ (tp != curtab)
    {
	tabpage_close_other(tp, eap->forceit);
	തിരികെ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!text_locked() && !curbuf_locked())
	tabpage_close(eap->forceit);
}

/*
 * ":tabonly": close all tab pages except the current one
 */
    സ്ഥിതം ശൂന്യം
ex_tabonly(exarg_T *eap)
{
    tabpage_T	*tp;
    പൂർണ്ണസംഖ്യ		done;
    പൂർണ്ണസംഖ്യ		tab_സംഖ്യ;

    എങ്കിൽ (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	തിരികെ;
    }

    എങ്കിൽ (first_tabpage->tp_next == ശൂന്യ)
    {
	സന്ദേശം(_("Already only one tab page"));
	തിരികെ;
    }

    എങ്കിൽ (window_layout_locked(CMD_tabonly))
	തിരികെ;

    tab_സംഖ്യ = get_tabpage_arg(eap);
    എങ്കിൽ (eap->errmsg != ശൂന്യ)
	തിരികെ;

    goto_tabpage(tab_സംഖ്യ);
    // Repeat this up to a 1000 times, because autocommands may
    // mess up the lists.
    വേണ്ടി (done = 0; done < 1000; ++done)
    {
	FOR_ALL_TABPAGES(tp)
	    എങ്കിൽ (tp->tp_topframe != topframe)
	    {
		tabpage_close_other(tp, eap->forceit);
		// if we failed to close it quit
		എങ്കിൽ (valid_tabpage(tp))
		    done = 1000;
		// start over, "tp" is now invalid
		നിർത്തുക;
	    }
	എങ്കിൽ (first_tabpage->tp_next == ശൂന്യ)
	    നിർത്തുക;
    }
}

/*
 * Close the current tab page.
 */
    ശൂന്യം
tabpage_close(പൂർണ്ണസംഖ്യ forceit)
{
    എങ്കിൽ (window_layout_locked(CMD_tabclose))
	തിരികെ;

    trigger_tabclosedpre(curtab);
    curtab->tp_did_tabclosedpre = TRUE;
    tabpage_T *save_curtab = curtab;

    // First close all the windows but the current one.  If that worked then
    // close the last window in this tab, that will close it.
    എങ്കിൽ (!ONE_WINDOW)
	close_others(TRUE, forceit);
    എങ്കിൽ (ONE_WINDOW)
	ex_win_close(forceit, curwin, ശൂന്യ);
    എങ്കിൽ (curtab == save_curtab)
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
    ശൂന്യം
tabpage_close_other(tabpage_T *tp, പൂർണ്ണസംഖ്യ forceit)
{
    പൂർണ്ണസംഖ്യ		done = 0;
    win_T	*wp;

    എങ്കിൽ (window_layout_locked(CMD_SIZE))
	തിരികെ;

    trigger_tabclosedpre(tp);
    tp->tp_did_tabclosedpre = TRUE;

    // Limit to 1000 windows, autocommands may add a window while we close
    // one.  OK, so I'm paranoid...
    ആകുമ്പോൾ (++done < 1000)
    {
	wp = tp->tp_firstwin;
	ex_win_close(forceit, wp, tp);

	// Autocommands may delete the tab page under our fingers.
	എങ്കിൽ (!valid_tabpage(tp))
	    നിർത്തുക;
	// We may fail to close a window with a modified buffer.
	എങ്കിൽ (tp->tp_firstwin == wp)
	{
	    done = 1000;
	    നിർത്തുക;
	}
    }
    എങ്കിൽ (done >= 1000)
    {
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	tp->tp_did_tabclosedpre = FALSE;
	തിരികെ;
    }

    apply_autocmds(EVENT_TABCLOSED, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
}

/*
 * ":only".
 */
    സ്ഥിതം ശൂന്യം
ex_only(exarg_T *eap)
{
    എങ്കിൽ (window_layout_locked(CMD_only))
	തിരികെ;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    എങ്കിൽ (eap->addr_എണ്ണം > 0)
    {
	win_T   *wp;
	പൂർണ്ണസംഖ്യ	wnr = eap->line2;
	വേണ്ടി (wp = firstwin; --wnr > 0; )
	{
	    എങ്കിൽ (wp->w_next == ശൂന്യ)
		നിർത്തുക;
	    അല്ലെങ്കിൽ
		wp = wp->w_next;
	}
	win_goto(wp);
    }
    close_others(TRUE, eap->forceit);
}

    സ്ഥിതം ശൂന്യം
ex_hide(exarg_T *eap UNUSED)
{
    // ":hide" or ":hide | cmd": hide current window
    എങ്കിൽ (eap->skip)
	തിരികെ;

    എങ്കിൽ (window_layout_locked(CMD_hide))
	തിരികെ;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    എങ്കിൽ (eap->addr_എണ്ണം == 0)
	win_close(curwin, FALSE);	// don't free buffer
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	winnr = 0;
	win_T	*win;

	FOR_ALL_WINDOWS(win)
	{
	    winnr++;
	    എങ്കിൽ (winnr == eap->line2)
		നിർത്തുക;
	}
	എങ്കിൽ (win == ശൂന്യ)
	    win = lastwin;
	win_close(win, FALSE);
    }
}

/*
 * ":stop" and ":suspend": Suspend Vim.
 */
    ശൂന്യം
ex_stop(exarg_T *eap)
{
    /*
     * Disallow suspending for "rvim".
     */
    എങ്കിൽ (check_restricted())
	തിരികെ;

    എങ്കിൽ (!eap->forceit)
	autowrite_all();
    apply_autocmds(EVENT_VIMSUSPEND, ശൂന്യ, ശൂന്യ, FALSE, ശൂന്യ);
    windgoto((പൂർണ്ണസംഖ്യ)Rows - 1, cmdline_col_off);
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
    apply_autocmds(EVENT_VIMRESUME, ശൂന്യ, ശൂന്യ, FALSE, ശൂന്യ);
}

/*
 * ":exit", ":xit" and ":wq": Write file and quit the current window.
 */
    സ്ഥിതം ശൂന്യം
ex_exit(exarg_T *eap)
{
#ifdef FEAT_EVAL
    എങ്കിൽ (not_in_vim9(eap, false) == FAIL)
	തിരികെ;
#endif
    എങ്കിൽ (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	തിരികെ;
    }
    // Don't quit while editing the command line.
    എങ്കിൽ (text_locked())
    {
	text_locked_സന്ദേശം();
	തിരികെ;
    }

    പൂർണ്ണസംഖ്യ save_exiting = exiting;
    /*
     * we plan to exit if there is only one relevant window
     */
    എങ്കിൽ (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;

    // Write the buffer for ":wq" or when it was changed.
    // Trigger QuitPre and ExitPre.
    // Check if we can exit now, after autocommands have changed things.
    എങ്കിൽ (((eap->cmdidx == CMD_wq || curbufIsChanged()) && do_write(eap) == FAIL)
	    || before_quit_autocmds(curwin, FALSE, eap->forceit)
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, FALSE)))
    {
	not_exiting(save_exiting);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (only_one_window())	    // quit last window, exit Vim
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
    സ്ഥിതം ശൂന്യം
ex_print(exarg_T *eap)
{
    എങ്കിൽ (curbuf->b_ml.ml_flags & ML_EMPTY)
	emsg(_(e_empty_buffer));
    അല്ലെങ്കിൽ
    {
	വേണ്ടി ( ;!got_int; ui_breakcheck())
	{
	    print_വരി(eap->line1,
		    (eap->cmdidx == CMD_സംഖ്യ || eap->cmdidx == CMD_pound
						 || (eap->flags & EXFLAG_NR)),
		    eap->cmdidx == CMD_list || (eap->flags & EXFLAG_LIST));
	    എങ്കിൽ (++eap->line1 > eap->line2)
		നിർത്തുക;
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
    സ്ഥിതം ശൂന്യം
ex_goto(exarg_T *eap)
{
    goto_byte(eap->line2);
}
#endif

/*
 * ":shell".
 */
    സ്ഥിതം ശൂന്യം
ex_shell(exarg_T *eap UNUSED)
{
    do_shell(ശൂന്യ, 0);
}

#എങ്കിൽ defined(HAVE_DROP_FILE)

സ്ഥിതം പൂർണ്ണസംഖ്യ drop_busy = FALSE;
സ്ഥിതം പൂർണ്ണസംഖ്യ drop_filec;
സ്ഥിതം char_u **drop_filev = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ drop_split;
സ്ഥിതം ശൂന്യം (*drop_callback)(ശൂന്യം *);
സ്ഥിതം ശൂന്യം *drop_cookie;

    സ്ഥിതം ശൂന്യം
handle_drop_internal(ശൂന്യം)
{
    exarg_T	ea;
    പൂർണ്ണസംഖ്യ		save_സന്ദേശം_scroll = സന്ദേശം_scroll;

    // Setting the argument list may cause screen updates and being called
    // recursively.  Avoid that by setting drop_busy.
    drop_busy = TRUE;

    // Check whether the current buffer is changed. If so, we will need
    // to split the current window or data could be lost.
    // We don't need to check if the 'hidden' option is set, as in this
    // case the buffer won't be lost.
    എങ്കിൽ (!buf_hide(curbuf) && !drop_split)
    {
	++emsg_off;
	drop_split = check_changed(curbuf, CCGD_AW);
	--emsg_off;
    }
    എങ്കിൽ (drop_split)
    {
	എങ്കിൽ (win_split(0, 0) == FAIL)
	    തിരികെ;
	RESET_BINDING(curwin);

	// When splitting the window, create a new alist.  Otherwise the
	// existing one is overwritten.
	alist_unlink(curwin->w_alist);
	alist_new();
    }

    /*
     * Set up the new argument list.
     */
    alist_set(ALIST(curwin), drop_filec, drop_filev, FALSE, ശൂന്യ, 0);

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
    സന്ദേശം_scroll = save_സന്ദേശം_scroll;

    എങ്കിൽ (drop_callback != ശൂന്യ)
	drop_callback(drop_cookie);

    drop_filev = ശൂന്യ;
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
    ശൂന്യം
handle_drop(
    പൂർണ്ണസംഖ്യ		filec,		// the number of files dropped
    char_u	**filev,	// the list of files dropped
    പൂർണ്ണസംഖ്യ		split,		// force splitting the window
    ശൂന്യം	(*callback)(ശൂന്യം *), // to be called after setting the argument
				     // list
    ശൂന്യം	*cookie)	// argument for "callback" (allocated)
{
    // Cannot handle recursive drops, finish the pending one.
    എങ്കിൽ (drop_busy)
    {
	FreeWild(filec, filev);
	vim_free(cookie);
	തിരികെ;
    }

    // When calling handle_drop() more than once in a row we only use the last
    // one.
    എങ്കിൽ (drop_filev != ശൂന്യ)
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
    എങ്കിൽ (text_locked() || curbuf_locked() || updating_screen)
	തിരികെ;

    handle_drop_internal();
}

/*
 * To be called when text is unlocked, curbuf is unlocked or updating_screen is
 * reset: Handle a postponed drop.
 */
    ശൂന്യം
handle_any_postponed_drop(ശൂന്യം)
{
    എങ്കിൽ (!drop_busy && drop_filev != ശൂന്യ
	     && !text_locked() && !curbuf_locked() && !updating_screen)
	handle_drop_internal();
}
#endif

/*
 * ":preserve".
 */
    സ്ഥിതം ശൂന്യം
ex_preserve(exarg_T *eap UNUSED)
{
    curbuf->b_flags |= BF_PRESERVED;
    ml_preserve(curbuf, TRUE);
}

/*
 * ":recover".
 */
    സ്ഥിതം ശൂന്യം
ex_recover(exarg_T *eap)
{
    // Set recoverymode right away to avoid the ATTENTION prompt.
    recoverymode = TRUE;
    എങ്കിൽ (!check_changed(curbuf, (p_awa ? CCGD_AW : 0)
			     | CCGD_MULTWIN
			     | (eap->forceit ? CCGD_FORCEIT : 0)
			     | CCGD_EXCMD)

	    && (*eap->arg == NUL
			     || setfname(curbuf, eap->arg, ശൂന്യ, TRUE) == OK))
	ml_recover(TRUE);
    recoverymode = FALSE;
}

/*
 * Command modifier used in a wrong way.  Also for other commands that can't
 * appear at the toplevel.
 */
    സ്ഥിതം ശൂന്യം
ex_wrongmodifier(exarg_T *eap)
{
    eap->errmsg = ex_errmsg(e_invalid_command_str, eap->cmd);
}

#എങ്കിൽ defined(FEAT_EVAL)

// callback function for 'findfunc'
സ്ഥിതം callback_T ffu_cb;

    സ്ഥിതം callback_T *
get_findfunc_callback(ശൂന്യം)
{
    തിരികെ *curbuf->b_p_ffu != NUL ? &curbuf->b_ffu_cb : &ffu_cb;
}

/*
 * Call 'findfunc' to obtain a list of file names.
 */
    സ്ഥിതം list_T *
call_findfunc(char_u *pat, പൂർണ്ണസംഖ്യ cmdcomplete)
{
    typval_T	args[3];
    callback_T	*cb;
    typval_T	rettv;
    പൂർണ്ണസംഖ്യ		retval;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_BOOL;
    args[1].vval.v_സംഖ്യ = cmdcomplete;
    args[2].v_type = VAR_UNKNOWN;

    // Lock the text to prevent weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;

    ctx = get_option_sctx("findfunc");
    എങ്കിൽ (ctx != ശൂന്യ)
	current_sctx = *ctx;

    cb = get_findfunc_callback();
    retval = call_callback(cb, -1, &rettv, 2, args);

    current_sctx = saved_sctx;

    --textlock;

    list_T *retlist = ശൂന്യ;

    എങ്കിൽ (retval == OK)
    {
	എങ്കിൽ (rettv.v_type == VAR_LIST)
	    retlist = list_copy(rettv.vval.v_list, FALSE, FALSE, get_copyID());
	അല്ലെങ്കിൽ
	    emsg(_(e_invalid_return_type_from_findfunc));

	clear_tv(&rettv);
    }

    തിരികെ retlist;
}

/*
 * Find file names matching "pat" using 'findfunc' and return it in "files".
 * Used for expanding the :find, :sfind and :tabfind command argument.
 * Returns OK on success and FAIL otherwise.
 */
    പൂർണ്ണസംഖ്യ
expand_findfunc(expand_T *xp, char_u *pat, char_u ***files, പൂർണ്ണസംഖ്യ *അക്കം_Matches)
{
    list_T	*l;
    പൂർണ്ണസംഖ്യ		len;

    *അക്കം_Matches = 0;
    *files = ശൂന്യ;

    l = call_findfunc(pat, VVAL_TRUE);

    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ FAIL;

    len = list_len(l);
    എങ്കിൽ (len == 0)	    // empty List
    {
	list_free(l);
	തിരികെ FAIL;
    }

    expand_process_user_list(l, files, അക്കം_Matches, xp);
    list_free(l);

    തിരികെ OK;
}

/*
 * Use 'findfunc' to find file 'findarg'.  The 'count' argument is used to find
 * the n'th matching file.
 */
    സ്ഥിതം char_u *
findfunc_find_file(char_u *findarg, പൂർണ്ണസംഖ്യ findarg_len, പൂർണ്ണസംഖ്യ എണ്ണം)
{
    list_T	*fname_list;
    char_u	*ret_fname = ശൂന്യ;
    char_u	cc;
    പൂർണ്ണസംഖ്യ		fname_എണ്ണം;

    cc = findarg[findarg_len];
    findarg[findarg_len] = NUL;

    fname_list = call_findfunc(findarg, VVAL_FALSE);
    fname_എണ്ണം = list_len(fname_list);

    എങ്കിൽ (fname_എണ്ണം == 0)
	semsg(_(e_cant_find_file_str_in_path), findarg);
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (എണ്ണം > fname_എണ്ണം)
	    semsg(_(e_no_more_file_str_found_in_path), findarg);
	അല്ലെങ്കിൽ
	{
	    listitem_T *li = list_find(fname_list, എണ്ണം - 1);

	    എങ്കിൽ (li != ശൂന്യ)
	    {
		typval_T *tv = &li->li_tv;

		എങ്കിൽ (tv->v_type == VAR_STRING && tv->vval.v_string != ശൂന്യ)
		    ret_fname = vim_strsave(tv->vval.v_string);
		അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_DICT && tv->vval.v_dict != ശൂന്യ)
		    ret_fname = dict_get_string(tv->vval.v_dict, "word", TRUE);
	    }
	}
    }

    എങ്കിൽ (fname_list != ശൂന്യ)
	list_free(fname_list);

    findarg[findarg_len] = cc;

    തിരികെ ret_fname;
}

/*
 * Process the 'findfunc' option value.
 * Returns NULL on success and an error message on failure.
 */
    അക്ഷരം *
did_set_findfunc(optset_T *args UNUSED)
{
    പൂർണ്ണസംഖ്യ	retval;

    എങ്കിൽ (args->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_ffu, &curbuf->b_ffu_cb);
    അല്ലെങ്കിൽ
    {
	// global option set
	retval = option_set_callback_func(p_ffu, &ffu_cb);
	// when using :set, free the local callback
	എങ്കിൽ (!(args->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_ffu_cb);
    }

    എങ്കിൽ (retval == FAIL)
	തിരികെ e_invalid_argument;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*name = get_scriptlocal_funcname(*varp);
    എങ്കിൽ (name != ശൂന്യ)
    {
	free_string_option(*varp);
	*varp = name;
    }

    തിരികെ ശൂന്യ;
}

# എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_findfunc_option(ശൂന്യം)
{
    free_callback(&ffu_cb);
}
# endif

/*
 * Mark the global 'findfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_findfunc(പൂർണ്ണസംഖ്യ copyID UNUSED)
{
    പൂർണ്ണസംഖ്യ abort = FALSE;
    abort = set_ref_in_callback(&ffu_cb, copyID);
    തിരികെ abort;
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
    ശൂന്യം
ex_splitview(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    char_u	*fname = ശൂന്യ;
#ifdef FEAT_BROWSE
    char_u	dot_path[] = ".";
    പൂർണ്ണസംഖ്യ		save_cmod_flags = cmdmod.cmod_flags;
#endif
    പൂർണ്ണസംഖ്യ		use_tab = eap->cmdidx == CMD_tabedit
		       || eap->cmdidx == CMD_tabfind
		       || eap->cmdidx == CMD_tabnew;

    എങ്കിൽ (ERROR_IF_ANY_POPUP_WINDOW)
	തിരികെ;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#ifdef FEAT_QUICKFIX
    // A ":split" in the quickfix window works like ":new".  Don't want two
    // quickfix windows.  But it's OK when doing ":tab split".
    എങ്കിൽ (bt_quickfix(curbuf) && cmdmod.cmod_tab == 0)
    {
	എങ്കിൽ (eap->cmdidx == CMD_split)
	    eap->cmdidx = CMD_new;
	എങ്കിൽ (eap->cmdidx == CMD_vsplit)
	    eap->cmdidx = CMD_vnew;
    }
#endif

    എങ്കിൽ (eap->cmdidx == CMD_sfind || eap->cmdidx == CMD_tabfind)
    {
	char_u	*file_to_find = ശൂന്യ;
	അക്ഷരം	*തിരയൽ_ctx = ശൂന്യ;

	എങ്കിൽ (*get_findfunc() != NUL)
	{
#ifdef FEAT_EVAL
	    fname = findfunc_find_file(eap->arg, (പൂർണ്ണസംഖ്യ)STRLEN(eap->arg),
				       eap->addr_എണ്ണം > 0 ? eap->line2 : 1);
#endif
	}
	അല്ലെങ്കിൽ
	{
	    fname = find_file_in_path(eap->arg, (പൂർണ്ണസംഖ്യ)STRLEN(eap->arg),
					      FNAME_MESS, TRUE, curbuf->b_ffname,
					      &file_to_find, &തിരയൽ_ctx);
	    vim_free(file_to_find);
	    vim_findfile_cleanup(തിരയൽ_ctx);
	}
	എങ്കിൽ (fname == ശൂന്യ)
	    പോകുക theend;
	eap->arg = fname;
    }
#ifdef FEAT_BROWSE
    അല്ലെങ്കിൽ എങ്കിൽ ((cmdmod.cmod_flags & CMOD_BROWSE)
	    && eap->cmdidx != CMD_vnew
	    && eap->cmdidx != CMD_new)
    {
	എങ്കിൽ (
# ifdef FEAT_GUI
	    !gui.in_use &&
# endif
		au_has_group((char_u *)"FileExplorer"))
	{
	    // No browsing supported but we do have the file explorer:
	    // Edit the directory.
	    എങ്കിൽ (*eap->arg == NUL || !mch_isdir(eap->arg))
		eap->arg = dot_path;
	}
	അല്ലെങ്കിൽ
	{
	    fname = do_browse(0, (char_u *)(use_tab
			? _("Edit File in new tab page")
			: _("Edit File in new window")),
					  eap->arg, ശൂന്യ, ശൂന്യ, ശൂന്യ, curbuf);
	    എങ്കിൽ (fname == ശൂന്യ)
		പോകുക theend;
	    eap->arg = fname;
	}
    }
    cmdmod.cmod_flags &= ~CMOD_BROWSE;	// Don't browse again in do_ecmd().
#endif

    /*
     * Either open new tab page or split the window.
     */
    എങ്കിൽ (use_tab)
    {
	എങ്കിൽ (win_new_tabpage(cmdmod.cmod_tab != 0 ? cmdmod.cmod_tab
			 : eap->addr_എണ്ണം == 0 ? 0
					       : (പൂർണ്ണസംഖ്യ)eap->line2 + 1) != FAIL)
	{
	    do_exedit(eap, old_curwin);

	    // set the alternate buffer for the window we came from
	    എങ്കിൽ (curwin != old_curwin
		    && win_valid(old_curwin)
		    && old_curwin->w_buffer != curbuf
		    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		old_curwin->w_alt_fnum = curbuf->b_fnum;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (win_split(eap->addr_എണ്ണം > 0 ? (പൂർണ്ണസംഖ്യ)eap->line2 : 0,
				     *eap->cmd == 'v' ? WSP_VERT : 0) != FAIL)
    {
	// Reset 'scrollbind' when editing another file, but keep it when
	// doing ":split" without arguments.
	എങ്കിൽ (*eap->arg != NUL)
	    RESET_BINDING(curwin);
	അല്ലെങ്കിൽ
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
    ശൂന്യം
tabpage_new(ശൂന്യം)
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
    സ്ഥിതം ശൂന്യം
ex_tabnext(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ tab_സംഖ്യ;

    എങ്കിൽ (ERROR_IF_POPUP_WINDOW)
	തിരികെ;
    തിരഞ്ഞെടുപ്പ് (eap->cmdidx)
    {
	അവസ്ഥ CMD_tabfirst:
	അവസ്ഥ CMD_tabrewind:
	    goto_tabpage(1);
	    നിർത്തുക;
	അവസ്ഥ CMD_tablast:
	    goto_tabpage(9999);
	    നിർത്തുക;
	അവസ്ഥ CMD_tabprevious:
	അവസ്ഥ CMD_tabNext:
	    എങ്കിൽ (eap->arg && *eap->arg != NUL)
	    {
		char_u *p = eap->arg;
		char_u *p_save = p;

		tab_സംഖ്യ = getdigits(&p);
		എങ്കിൽ (p == p_save || *p_save == '-' || *p != NUL
			    || tab_സംഖ്യ == 0)
		{
		    // No numbers as argument.
		    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		    തിരികെ;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (eap->addr_എണ്ണം == 0)
		    tab_സംഖ്യ = 1;
		അല്ലെങ്കിൽ
		{
		    tab_സംഖ്യ = eap->line2;
		    എങ്കിൽ (tab_സംഖ്യ < 1)
		    {
			eap->errmsg = _(e_invalid_range);
			തിരികെ;
		    }
		}
	    }
	    goto_tabpage(-tab_സംഖ്യ);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി: // CMD_tabnext
	    tab_സംഖ്യ = get_tabpage_arg(eap);
	    എങ്കിൽ (eap->errmsg == ശൂന്യ)
		goto_tabpage(tab_സംഖ്യ);
	    നിർത്തുക;
    }
}

/*
 * :tabmove command
 */
    സ്ഥിതം ശൂന്യം
ex_tabmove(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ tab_സംഖ്യ;

    tab_സംഖ്യ = get_tabpage_arg(eap);
    എങ്കിൽ (eap->errmsg == ശൂന്യ)
	tabpage_move(tab_സംഖ്യ);
}

/*
 * :tabs command: List tabs and their contents.
 */
    സ്ഥിതം ശൂന്യം
ex_tabs(exarg_T *eap UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		tabcount = 1;

    സന്ദേശം_start();
    സന്ദേശം_scroll = TRUE;
    വേണ്ടി (tp = first_tabpage; tp != ശൂന്യ && !got_int; tp = tp->tp_next)
    {
	സന്ദേശം_putchar('\n');
	vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, _("Tab page %d"), tabcount++);
	സന്ദേശം_outtrans_attr(IObuff, HL_ATTR(HLF_T));
	out_flush();	    // output one line at a time
	ui_breakcheck();

	എങ്കിൽ (tp  == curtab)
	    wp = firstwin;
	അല്ലെങ്കിൽ
	    wp = tp->tp_firstwin;
	വേണ്ടി ( ; wp != ശൂന്യ && !got_int; wp = wp->w_next)
	{
	    സന്ദേശം_putchar('\n');
	    സന്ദേശം_putchar(wp == curwin ? '>' : ' ');
	    സന്ദേശം_putchar(' ');
	    സന്ദേശം_putchar(bufIsChanged(wp->w_buffer) ? '+' : ' ');
	    സന്ദേശം_putchar(' ');
	    എങ്കിൽ (buf_spname(wp->w_buffer) != ശൂന്യ)
		vim_strncpy(IObuff, buf_spname(wp->w_buffer), IOSIZE - 1);
	    അല്ലെങ്കിൽ
		home_replace(wp->w_buffer, wp->w_buffer->b_fname,
							IObuff, IOSIZE, TRUE);
	    സന്ദേശം_outtrans(IObuff);
	    out_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }
}

/*
 * ":mode": Set screen mode.
 * If no argument given, just get the screen size and redraw.
 */
    സ്ഥിതം ശൂന്യം
ex_mode(exarg_T *eap)
{
    എങ്കിൽ (*eap->arg == NUL)
	shell_resized();
    അല്ലെങ്കിൽ
	emsg(_(e_screen_mode_setting_not_supported));
}

/*
 * ":resize".
 * set, increment or decrement current window height
 */
    സ്ഥിതം ശൂന്യം
ex_resize(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ		n;
    win_T	*wp = curwin;

    എങ്കിൽ (eap->addr_എണ്ണം > 0)
    {
	n = eap->line2;
	വേണ്ടി (wp = firstwin; wp->w_next != ശൂന്യ && --n > 0; wp = wp->w_next)
	    ;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    n = atol((അക്ഷരം *)eap->arg);
    എങ്കിൽ (cmdmod.cmod_split & WSP_VERT)
    {
	എങ്കിൽ (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_width;
	അല്ലെങ്കിൽ എങ്കിൽ (n == 0 && eap->arg[0] == NUL)	// default is very wide
	    n = 9999;
	win_setwidth_win(n, wp);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_height;
	അല്ലെങ്കിൽ എങ്കിൽ (n == 0 && eap->arg[0] == NUL)	// default is very high
	    n = 9999;
	win_setheight_win(n, wp);
    }
}

/*
 * ":find [+command] <file>" command.
 */
    സ്ഥിതം ശൂന്യം
ex_find(exarg_T *eap)
{
    എങ്കിൽ (!check_can_set_curbuf_forceit(eap->forceit))
	തിരികെ;

    char_u	*fname = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		എണ്ണം;
    char_u	*file_to_find = ശൂന്യ;
    അക്ഷരം	*തിരയൽ_ctx = ശൂന്യ;

    എങ്കിൽ (*get_findfunc() != NUL)
    {
#ifdef FEAT_EVAL
	fname = findfunc_find_file(eap->arg, (പൂർണ്ണസംഖ്യ)STRLEN(eap->arg),
					eap->addr_എണ്ണം > 0 ? eap->line2 : 1);
#endif
    }
    അല്ലെങ്കിൽ
    {
	fname = find_file_in_path(eap->arg, (പൂർണ്ണസംഖ്യ)STRLEN(eap->arg), FNAME_MESS,
			       TRUE, curbuf->b_ffname, &file_to_find, &തിരയൽ_ctx);
	എങ്കിൽ (eap->addr_എണ്ണം > 0)
	{
	    // Repeat finding the file "count" times.  This matters when it appears
	    // several times in the path.
	    എണ്ണം = eap->line2;
	    ആകുമ്പോൾ (fname != ശൂന്യ && --എണ്ണം > 0)
	    {
		vim_free(fname);
		fname = find_file_in_path(ശൂന്യ, 0, FNAME_MESS,
			      FALSE, curbuf->b_ffname, &file_to_find, &തിരയൽ_ctx);
	    }
	}
	VIM_CLEAR(file_to_find);
	vim_findfile_cleanup(തിരയൽ_ctx);
    }

    എങ്കിൽ (fname == ശൂന്യ)
	തിരികെ;

    eap->arg = fname;
    do_exedit(eap, ശൂന്യ);
    vim_free(fname);
}

/*
 * ":open" simulation: for now works just like ":visual".
 */
    സ്ഥിതം ശൂന്യം
ex_open(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*p;

#ifdef FEAT_EVAL
    എങ്കിൽ (not_in_vim9(eap, false) == FAIL)
	തിരികെ;
#endif
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_SOL | BL_FIX);
    എങ്കിൽ (*eap->arg == '/')
    {
	// ":open /pattern/": put cursor in column found with pattern
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg, magic_isset() ? RE_MAGIC : 0);
	എങ്കിൽ (regmatch.regprog != ശൂന്യ)
	{
	    // make a copy of the line, when searching for a mark it might be
	    // flushed
	    char_u *വരി = vim_strsave(ml_get_curline());

	    regmatch.rm_ic = p_ic;
	    എങ്കിൽ (vim_regexec(&regmatch, വരി, (colnr_T)0))
		curwin->w_cursor.col = (colnr_T)(regmatch.startp[0] - വരി);
	    അല്ലെങ്കിൽ
		emsg(_(e_no_match));
	    vim_regfree(regmatch.regprog);
	    vim_free(വരി);
	}
	// Move to the NUL, ignore any other arguments.
	eap->arg += STRLEN(eap->arg);
    }
    check_cursor();

    eap->cmdidx = CMD_visual;
    do_exedit(eap, ശൂന്യ);
}

/*
 * ":edit", ":badd", ":balt", ":visual".
 */
    സ്ഥിതം ശൂന്യം
ex_edit(exarg_T *eap)
{
    char_u *ffname = eap->cmdidx == CMD_enew ? ശൂന്യ : eap->arg;

    // Exclude commands which keep the window's current buffer
    എങ്കിൽ (
	    eap->cmdidx != CMD_badd
	    && eap->cmdidx != CMD_balt
	    // All other commands must obey 'winfixbuf' / ! rules
	    && (is_other_file(0, ffname) && !check_can_set_curbuf_forceit(eap->forceit))
    )
	തിരികെ;

    do_exedit(eap, ശൂന്യ);
}

/*
 * ":edit <file>" command and alike.
 */
    ശൂന്യം
do_exedit(
    exarg_T	*eap,
    win_T	*old_curwin)	    // curwin before doing a split or NULL
{
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		need_hide;
    പൂർണ്ണസംഖ്യ		exmode_was = exmode_active;

    എങ്കിൽ ((eap->cmdidx != CMD_pedit && ERROR_IF_POPUP_WINDOW)
						 || ERROR_IF_TERM_POPUP_WINDOW)
	തിരികെ;
    /*
     * ":vi" command ends Ex mode.
     */
    എങ്കിൽ (exmode_active && (eap->cmdidx == CMD_visual
						|| eap->cmdidx == CMD_view))
    {
	exmode_active = FALSE;
	ex_pressedreturn = FALSE;
	എങ്കിൽ (*eap->arg == NUL)
	{
	    // Special case:  ":global/pat/visual\NLvi-commands"
	    എങ്കിൽ (global_busy)
	    {
		എങ്കിൽ (eap->nextcmd != ശൂന്യ)
		{
		    stuffReadbuff(eap->nextcmd);
		    eap->nextcmd = ശൂന്യ;
		}

		എങ്കിൽ (exmode_was != EXMODE_VIM)
		    settmode(TMODE_RAW);
		പൂർണ്ണസംഖ്യ save_RedrawingDisabled = RedrawingDisabled;
		RedrawingDisabled = 0;
		പൂർണ്ണസംഖ്യ save_nwr = no_wait_return;
		no_wait_return = 0;
		need_wait_return = FALSE;
		പൂർണ്ണസംഖ്യ save_ms = സന്ദേശം_scroll;
		സന്ദേശം_scroll = 0;
#ifdef FEAT_GUI
		പൂർണ്ണസംഖ്യ save_he = hold_gui_events;
		hold_gui_events = 0;
#endif
		set_must_redraw(UPD_CLEAR);
		pending_exmode_active = TRUE;

		main_loop(FALSE, TRUE);

		pending_exmode_active = FALSE;
		RedrawingDisabled = save_RedrawingDisabled;
		no_wait_return = save_nwr;
		സന്ദേശം_scroll = save_ms;
#ifdef FEAT_GUI
		hold_gui_events = save_he;
#endif
	    }
	    തിരികെ;
	}
    }

    എങ്കിൽ ((eap->cmdidx == CMD_new
		|| eap->cmdidx == CMD_tabnew
		|| eap->cmdidx == CMD_tabedit
		|| eap->cmdidx == CMD_vnew) && *eap->arg == NUL)
    {
	// ":new" or ":tabnew" without argument: edit a new empty buffer
	setpcmark();
	(ശൂന്യം)do_ecmd(0, ശൂന്യ, ശൂന്യ, eap, ECMD_ONE,
		      ECMD_HIDE + (eap->forceit ? ECMD_FORCEIT : 0),
		      old_curwin == ശൂന്യ ? curwin : ശൂന്യ);
    }
    അല്ലെങ്കിൽ എങ്കിൽ ((eap->cmdidx != CMD_split && eap->cmdidx != CMD_vsplit)
	    || *eap->arg != NUL
#ifdef FEAT_BROWSE
	    || (cmdmod.cmod_flags & CMOD_BROWSE)
#endif
	    )
    {
	// Can't edit another file when "textlock" or "curbuf_lock" is set.
	// Only ":edit" or ":script" can bring us here, others are stopped
	// earlier.
	എങ്കിൽ (*eap->arg != NUL && text_or_buf_locked())
	    തിരികെ;

	n = readonlymode;
	എങ്കിൽ (eap->cmdidx == CMD_view || eap->cmdidx == CMD_sview)
	    readonlymode = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_enew)
	    readonlymode = FALSE;   // 'readonly' doesn't make sense in an
				    // empty buffer
	എങ്കിൽ (eap->cmdidx != CMD_balt && eap->cmdidx != CMD_badd)
	    setpcmark();
	എങ്കിൽ (do_ecmd(0, (eap->cmdidx == CMD_enew ? ശൂന്യ : eap->arg),
		    ശൂന്യ, eap,
		    // ":edit" goes to first line if Vi compatible
		    (*eap->arg == NUL && eap->do_ecmd_lnum == 0
				      && vim_strchr(p_cpo, CPO_GOTO1) != ശൂന്യ)
					       ? ECMD_ONE : eap->do_ecmd_lnum,
		    (buf_hide(curbuf) ? ECMD_HIDE : 0)
		    + (eap->forceit ? ECMD_FORCEIT : 0)
		      // after a split we can use an existing buffer
		    + (old_curwin != ശൂന്യ ? ECMD_OLDBUF : 0)
		    + (eap->cmdidx == CMD_badd ? ECMD_ADDBUF : 0)
		    + (eap->cmdidx == CMD_balt ? ECMD_ALTBUF : 0)
		    , old_curwin == ശൂന്യ ? curwin : ശൂന്യ) == FAIL)
	{
	    // Editing the file failed.  If the window was split, close it.
	    എങ്കിൽ (old_curwin != ശൂന്യ)
	    {
		need_hide = (curbufIsChanged() && curbuf->b_nwindows <= 1);
		എങ്കിൽ (!need_hide || buf_hide(curbuf))
		{
#എങ്കിൽ defined(FEAT_EVAL)
		    cleanup_T   cs;

		    // Reset the error/interrupt/exception state here so that
		    // aborting() returns FALSE when closing a window.
		    enter_cleanup(&cs);
#endif
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_close(curwin, !need_hide && !buf_hide(curbuf));

#എങ്കിൽ defined(FEAT_EVAL)
		    // Restore the error/interrupt/exception state if not
		    // discarded by a new aborting error, interrupt, or
		    // uncaught exception.
		    leave_cleanup(&cs);
#endif
		}
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (readonlymode && curbuf->b_nwindows == 1)
	{
	    // When editing an already visited buffer, 'readonly' won't be set
	    // but the previous value is kept.  With ":view" and ":sview" we
	    // want the  file to be readonly, except when another window is
	    // editing the same buffer.
	    curbuf->b_p_ro = TRUE;
	}
	readonlymode = n;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (eap->do_ecmd_cmd != ശൂന്യ)
	    do_cmd_argument(eap->do_ecmd_cmd);
	n = curwin->w_arg_idx_invalid;
	check_arg_idx(curwin);
	എങ്കിൽ (n != curwin->w_arg_idx_invalid)
	    maketitle();
    }

    /*
     * if ":split file" worked, set alternate file name in old window to new
     * file
     */
    എങ്കിൽ (old_curwin != ശൂന്യ
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
    സ്ഥിതം ശൂന്യം
ex_nogui(exarg_T *eap)
{
    eap->errmsg = _(e_gui_cannot_be_used_not_enabled_at_compile_time);
}
#endif

#എങ്കിൽ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
    സ്ഥിതം ശൂന്യം
ex_tearoff(exarg_T *eap)
{
    gui_make_tearoff(eap->arg);
}
#endif

#എങ്കിൽ (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
    സ്ഥിതം ശൂന്യം
ex_popup(exarg_T *eap)
{
# എങ്കിൽ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    എങ്കിൽ (gui.in_use)
	gui_make_popup(eap->arg, eap->forceit);
#  ifdef FEAT_TERM_POPUP_MENU
    അല്ലെങ്കിൽ
#  endif
# endif
# ifdef FEAT_TERM_POPUP_MENU
	pum_make_popup(eap->arg, eap->forceit);
# endif
}
#endif

    സ്ഥിതം ശൂന്യം
ex_swapname(exarg_T *eap UNUSED)
{
    എങ്കിൽ (curbuf->b_ml.ml_mfp == ശൂന്യ || curbuf->b_ml.ml_mfp->mf_fname == ശൂന്യ)
	സന്ദേശം(_("No swap file"));
    അല്ലെങ്കിൽ
	സന്ദേശം((അക്ഷരം *)curbuf->b_ml.ml_mfp->mf_fname);
}

/*
 * ":syncbind" forces all 'scrollbind' windows to have the same relative
 * offset.
 * (1998-11-02 16:21:01  R. Edward Ralston <eralston@computer.org>)
 */
    സ്ഥിതം ശൂന്യം
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
    എങ്കിൽ (curwin->w_p_scb)
    {
	topline = curwin->w_topline;
	FOR_ALL_WINDOWS(wp)
	{
	    എങ്കിൽ (wp->w_p_scb && wp->w_buffer)
	    {
		y = wp->w_buffer->b_ml.ml_വരി_എണ്ണം - get_scrolloff_മൂല്യം();
		എങ്കിൽ (topline > y)
		    topline = y;
	    }
	}
	എങ്കിൽ (topline < 1)
	    topline = 1;
    }
    അല്ലെങ്കിൽ
    {
	topline = 1;
    }


    /*
     * Set all scrollbind windows to the same topline.
     */
    FOR_ALL_WINDOWS(curwin)
    {
	എങ്കിൽ (curwin->w_p_scb)
	{
	    curbuf = curwin->w_buffer;
	    y = topline - curwin->w_topline;
	    എങ്കിൽ (y > 0)
		scrollup(y, TRUE);
	    അല്ലെങ്കിൽ
		scrolldown(-y, TRUE);
	    curwin->w_scbind_pos = topline;
	    redraw_later(UPD_VALID);
	    cursor_correct();
	    curwin->w_redr_status = true;
	}
    }
    curwin = save_curwin;
    curbuf = save_curbuf;
    എങ്കിൽ (curwin->w_p_scb)
    {
	did_syncbind = TRUE;
	checkpcmark();
	എങ്കിൽ (old_linenr != curwin->w_cursor.lnum)
	{
	    char_u ctrl_o[2];

	    ctrl_o[0] = Ctrl_O;
	    ctrl_o[1] = 0;
	    ins_typebuf(ctrl_o, REMAP_NONE, 0, TRUE, FALSE);
	}
    }
}


    സ്ഥിതം ശൂന്യം
ex_read(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum;

    എങ്കിൽ (eap->usefilter)			// :r!cmd
    {
	do_bang(1, eap, FALSE, FALSE, TRUE);
	തിരികെ;
    }

    എങ്കിൽ (u_save(eap->line2, (linenr_T)(eap->line2 + 1)) == FAIL)
	തിരികെ;

#ifdef FEAT_BROWSE
    എങ്കിൽ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browseFile;

	browseFile = do_browse(0, (char_u *)_("Append File"), eap->arg,
		ശൂന്യ, ശൂന്യ, ശൂന്യ, curbuf);
	എങ്കിൽ (browseFile != ശൂന്യ)
	{
	    i = readfile(browseFile, ശൂന്യ,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	    vim_free(browseFile);
	}
	അല്ലെങ്കിൽ
	    i = OK;
    }
    അല്ലെങ്കിൽ
#endif
	എങ്കിൽ (*eap->arg == NUL)
	{
	    എങ്കിൽ (check_fname() == FAIL)	// check for no file name
		തിരികെ;
	    i = readfile(curbuf->b_ffname, curbuf->b_fname,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (vim_strchr(p_cpo, CPO_ALTREAD) != ശൂന്യ)
		(ശൂന്യം)setaltfname(eap->arg, eap->arg, (linenr_T)1);
	    i = readfile(eap->arg, ശൂന്യ,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);

	}
    എങ്കിൽ (i != OK)
    {
#എങ്കിൽ defined(FEAT_EVAL)
	എങ്കിൽ (!aborting())
#endif
	    semsg(_(e_cant_open_file_str), eap->arg);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (empty && exmode_active)
	{
	    // Delete the empty line that remains.  Historically ex does
	    // this but vi doesn't.
	    എങ്കിൽ (eap->line2 == 0)
		lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
	    അല്ലെങ്കിൽ
		lnum = 1;
	    എങ്കിൽ (*ml_get(lnum) == NUL && u_savedel(lnum, 1L) == OK)
	    {
		ml_delete(lnum);
		എങ്കിൽ (curwin->w_cursor.lnum > 1
			&& curwin->w_cursor.lnum >= lnum)
		    --curwin->w_cursor.lnum;
		deleted_വരികൾ_mark(lnum, 1L);
	    }
	}
	redraw_curbuf_later(UPD_VALID);
    }
}

സ്ഥിതം char_u	*prev_dir = ശൂന്യ;

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_cd_dir(ശൂന്യം)
{
    VIM_CLEAR(prev_dir);
    VIM_CLEAR(globaldir);
}
#endif

/*
 * Get the previous directory for the given chdir scope.
 */
    സ്ഥിതം char_u *
get_prevdir(cdscope_T scope)
{
    എങ്കിൽ (scope == CDSCOPE_WINDOW)
	തിരികെ curwin->w_prevdir;
    അല്ലെങ്കിൽ എങ്കിൽ (scope == CDSCOPE_TABPAGE)
	തിരികെ curtab->tp_prevdir;
    തിരികെ prev_dir;
}

/*
 * Deal with the side effects of changing the current directory.
 * When 'scope' is CDSCOPE_TABPAGE then this was after an ":tcd" command.
 * When 'scope' is CDSCOPE_WINDOW then this was after an ":lcd" command.
 */
    ശൂന്യം
post_chdir(cdscope_T scope)
{
    എങ്കിൽ (scope != CDSCOPE_WINDOW)
	// Clear tab local directory for both :cd and :tcd
	VIM_CLEAR(curtab->tp_localdir);
    VIM_CLEAR(curwin->w_localdir);
    എങ്കിൽ (scope != CDSCOPE_GLOBAL)
    {
	char_u	*pdir = get_prevdir(scope);

	// If still in the global directory, need to remember current
	// directory as the global directory.
	എങ്കിൽ (globaldir == ശൂന്യ && pdir != ശൂന്യ)
	    globaldir = vim_strsave(pdir);

	// Remember this local directory for the window.
	എങ്കിൽ (mch_dirname(NameBuff, MAXPATHL) == OK)
	{
	    എങ്കിൽ (scope == CDSCOPE_TABPAGE)
		curtab->tp_localdir = vim_strsave(NameBuff);
	    അല്ലെങ്കിൽ
		curwin->w_localdir = vim_strsave(NameBuff);
	}
    }
    അല്ലെങ്കിൽ
    {
	// We are now in the global directory, no need to remember its name.
	VIM_CLEAR(globaldir);
    }

    last_chdir_reason = ശൂന്യ;
    shorten_fnames(vim_strchr(p_cpo, CPO_NOSYMLINKS) == ശൂന്യ);
}

/*
 * Trigger DirChangedPre for "acmd_fname" with directory "new_dir".
 */
    ശൂന്യം
trigger_DirChangedPre(char_u *acmd_fname, char_u *new_dir)
{
#ifdef FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;

    v_event = get_v_event(&save_v_event);
    (ശൂന്യം)dict_add_string(v_event, "directory", new_dir);
    dict_set_ഇനങ്ങൾ_ro(v_event);
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
    പൂർണ്ണസംഖ്യ
changedir_func(
	char_u		*new_dir,
	പൂർണ്ണസംഖ്യ		forceit,
	cdscope_T	scope)
{
    char_u	*pdir = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		dir_differs;
    char_u	*acmd_fname = ശൂന്യ;
    char_u	**pp;
    char_u	*tofree;

    എങ്കിൽ (new_dir == ശൂന്യ || allbuf_locked())
	തിരികെ FALSE;

    എങ്കിൽ (vim_strchr(p_cpo, CPO_CHDIR) != ശൂന്യ && curbufIsChanged() && !forceit)
    {
	emsg(_(e_cannot_change_directory_buffer_is_modified_add_bang_to_override));
	തിരികെ FALSE;
    }

    // ":cd -": Change to previous directory
    എങ്കിൽ (STRCMP(new_dir, "-") == 0)
    {
	pdir = get_prevdir(scope);
	എങ്കിൽ (pdir == ശൂന്യ)
	{
	    emsg(_(e_no_previous_directory));
	    തിരികെ FALSE;
	}
	new_dir = pdir;
    }

    // Save current directory for next ":cd -"
    എങ്കിൽ (mch_dirname(NameBuff, MAXPATHL) == OK)
	pdir = vim_strsave(NameBuff);
    അല്ലെങ്കിൽ
	pdir = ശൂന്യ;

    // For UNIX ":cd" means: go to home directory.
    // On other systems too if 'cdhome' is set.
#എങ്കിൽ defined(UNIX) || defined(VMS)
    എങ്കിൽ (*new_dir == NUL)
#അല്ലെങ്കിൽ
    എങ്കിൽ (*new_dir == NUL && p_cdh)
#endif
    {
	// use NameBuff for home directory name
#ifdef VMS
	char_u	*p;

	p = mch_getenv((char_u *)"SYS$LOGIN");
	എങ്കിൽ (p == ശൂന്യ || *p == NUL)	// empty is the same as not set
	    NameBuff[0] = NUL;
	അല്ലെങ്കിൽ
	    vim_strncpy(NameBuff, p, MAXPATHL - 1);
#അല്ലെങ്കിൽ
	expand_env((char_u *)"$HOME", NameBuff, MAXPATHL);
#endif
	new_dir = NameBuff;
    }
    dir_differs = pdir == ശൂന്യ
			    || pathcmp((അക്ഷരം *)pdir, (അക്ഷരം *)new_dir, -1) != 0;
    എങ്കിൽ (dir_differs)
    {
	എങ്കിൽ (scope == CDSCOPE_WINDOW)
	    acmd_fname = (char_u *)"window";
	അല്ലെങ്കിൽ എങ്കിൽ (scope == CDSCOPE_TABPAGE)
	    acmd_fname = (char_u *)"tabpage";
	അല്ലെങ്കിൽ
	    acmd_fname = (char_u *)"global";
	trigger_DirChangedPre(acmd_fname, new_dir);

	എങ്കിൽ (vim_chdir(new_dir))
	{
	    emsg(_(e_command_failed));
	    vim_free(pdir);
	    തിരികെ FALSE;
	}
    }

    എങ്കിൽ (scope == CDSCOPE_WINDOW)
	pp = &curwin->w_prevdir;
    അല്ലെങ്കിൽ എങ്കിൽ (scope == CDSCOPE_TABPAGE)
	pp = &curtab->tp_prevdir;
    അല്ലെങ്കിൽ
	pp = &prev_dir;
    tofree = *pp;  // new_dir may use this
    *pp = pdir;

    post_chdir(scope);

    എങ്കിൽ (dir_differs)
	apply_autocmds(EVENT_DIRCHANGED, acmd_fname, new_dir, FALSE, curbuf);
    vim_free(tofree);
    തിരികെ TRUE;
}

/*
 * ":cd", ":tcd", ":lcd", ":chdir" ":tchdir" and ":lchdir".
 */
    ശൂന്യം
ex_cd(exarg_T *eap)
{
    char_u	*new_dir;

    new_dir = eap->arg;
#എങ്കിൽ !defined(UNIX) && !defined(VMS)
    // for non-UNIX ":cd" means: print current directory unless 'cdhome' is set
    എങ്കിൽ (*new_dir == NUL && !p_cdh)
    {
	ex_pwd(ശൂന്യ);
	തിരികെ;
    }
#endif

    cdscope_T	scope = CDSCOPE_GLOBAL;

    എങ്കിൽ (eap->cmdidx == CMD_lcd || eap->cmdidx == CMD_lchdir)
	scope = CDSCOPE_WINDOW;
    അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_tcd || eap->cmdidx == CMD_tchdir)
	scope = CDSCOPE_TABPAGE;

    എങ്കിൽ (changedir_func(new_dir, eap->forceit, scope))
    {
	// Echo the new current directory if the command was typed.
	എങ്കിൽ (താക്കോൽ_Typed || p_verbose >= 5)
	    ex_pwd(eap);
    }
}

/*
 * ":pwd".
 */
    സ്ഥിതം ശൂന്യം
ex_pwd(exarg_T *eap UNUSED)
{
    എങ്കിൽ (mch_dirname(NameBuff, MAXPATHL) == OK)
    {
#ifdef BACKSLASH_IN_FILENAME
	slash_adjust(NameBuff);
#endif
	എങ്കിൽ (p_verbose > 0)
	{
	    അക്ഷരം *context = "global";

	    എങ്കിൽ (last_chdir_reason != ശൂന്യ)
		context = last_chdir_reason;
	    അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_localdir != ശൂന്യ)
		context = "window";
	    അല്ലെങ്കിൽ എങ്കിൽ (curtab->tp_localdir != ശൂന്യ)
		context = "tabpage";
	    smsg("[%s] %s", context, (അക്ഷരം *)NameBuff);
	}
	അല്ലെങ്കിൽ
	    സന്ദേശം((അക്ഷരം *)NameBuff);
    }
    അല്ലെങ്കിൽ
	emsg(_(e_directory_unknown));
}

/*
 * ":=".
 */
    സ്ഥിതം ശൂന്യം
ex_equal(exarg_T *eap)
{
    smsg("%ld", (long)eap->line2);
    ex_may_print(eap);
}

    സ്ഥിതം ശൂന്യം
ex_sleep(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ		n;
    long	len;

    എങ്കിൽ (cursor_valid())
    {
	n = W_WINROW(curwin) + curwin->w_wrow - സന്ദേശം_scrolled;
	എങ്കിൽ (n >= 0)
	    windgoto(n, curwin->w_wincol + curwin->w_wcol);
    }

    len = eap->line2;
    തിരഞ്ഞെടുപ്പ് (*eap->arg)
    {
	അവസ്ഥ 'm': നിർത്തുക;
	അവസ്ഥ NUL: len *= 1000L; നിർത്തുക;
	സ്ഥിരസ്ഥിതി: semsg(_(e_invalid_argument_str), eap->arg); തിരികെ;
    }

    // Hide the cursor if invoked with !
    do_sleep(len, eap->forceit);
}

/*
 * Sleep for "msec" milliseconds, but keep checking for a CTRL-C every second.
 * Hide the cursor if "hide_cursor" is TRUE.
 */
    ശൂന്യം
do_sleep(long msec, പൂർണ്ണസംഖ്യ hide_cursor)
{
    long	done = 0;
    long	wait_now;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    // Remember at what time we started, so that we know how much longer we
    // should wait after waiting for a bit.
    ELAPSED_INIT(start_tv);
#endif

    എങ്കിൽ (hide_cursor)
	cursor_sleep();
    അല്ലെങ്കിൽ
	cursor_on();

    out_flush_cursor(FALSE, FALSE);
    ആകുമ്പോൾ (!got_int && done < msec)
    {
	wait_now = msec - done > 1000L ? 1000L : msec - done;
#ifdef FEAT_TIMERS
	{
	    long    due_time = check_due_timer();

	    എങ്കിൽ (due_time > 0 && due_time < wait_now)
		wait_now = due_time;
	}
#endif
#ifdef FEAT_JOB_CHANNEL
	എങ്കിൽ (has_any_channel() && wait_now > 20L)
	    wait_now = 20L;
#endif
#ifdef FEAT_SOUND
	എങ്കിൽ (has_any_sound_callback() && wait_now > 20L)
	    wait_now = 20L;
#endif
	ui_delay(wait_now, TRUE);

#ifdef FEAT_JOB_CHANNEL
	എങ്കിൽ (has_any_channel())
	    ui_breakcheck_force(TRUE);
	അല്ലെങ്കിൽ
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
#അല്ലെങ്കിൽ
	// guesstimate time passed (will actually be more)
	done += wait_now;
#endif
    }

    // If CTRL-C was typed to interrupt the sleep, drop the CTRL-C from the
    // input buffer, otherwise a following call to input() fails.
    എങ്കിൽ (got_int)
	(ശൂന്യം)vpeekc();

    എങ്കിൽ (hide_cursor)
	cursor_unsleep();
}

/*
 * ":winsize" command (obsolete).
 */
    സ്ഥിതം ശൂന്യം
ex_winsize(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ		w, h;
    char_u	*arg = eap->arg;
    char_u	*p;

    എങ്കിൽ (!SAFE_isdigit(*arg))
    {
	semsg(_(e_invalid_argument_str), arg);
	തിരികെ;
    }
    w = getdigits(&arg);
    arg = skipwhite(arg);
    p = arg;
    h = getdigits(&arg);
    എങ്കിൽ (*p != NUL && *arg == NUL)
	set_shellsize(w, h, TRUE);
    അല്ലെങ്കിൽ
	emsg(_(e_winsize_requires_two_സംഖ്യ_arguments));
}

    സ്ഥിതം ശൂന്യം
ex_wincmd(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ		xchar = NUL;
    char_u	*p;

    എങ്കിൽ (*eap->arg == 'g' || *eap->arg == Ctrl_G)
    {
	// CTRL-W g and CTRL-W CTRL-G  have an extra command character
	എങ്കിൽ (eap->arg[1] == NUL)
	{
	    emsg(_(e_invalid_argument));
	    തിരികെ;
	}
	xchar = eap->arg[1];
	p = eap->arg + 2;
    }
    അല്ലെങ്കിൽ
	p = eap->arg + 1;

    set_nextcmd(eap, p);
    p = skipwhite(p);
    എങ്കിൽ (*p != NUL && *p != (
#ifdef FEAT_EVAL
	    in_vim9script() ? '#' :
#endif
		'"')
	    && eap->nextcmd == ശൂന്യ)
	emsg(_(e_invalid_argument));
    അല്ലെങ്കിൽ എങ്കിൽ (!eap->skip)
    {
	// Pass flags on for ":vertical wincmd ]".
	postponed_split_flags = cmdmod.cmod_split;
	postponed_split_tab = cmdmod.cmod_tab;
	do_window(*eap->arg, eap->addr_എണ്ണം > 0 ? eap->line2 : 0L, xchar);
	postponed_split_flags = 0;
	postponed_split_tab = 0;
    }
}

#എങ്കിൽ defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * ":winpos".
 */
    സ്ഥിതം ശൂന്യം
ex_winpos(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ		x, y;
    char_u	*arg = eap->arg;
    char_u	*p;

    എങ്കിൽ (*arg == NUL)
    {
# എങ്കിൽ defined(FEAT_GUI) || defined(MSWIN)
#  ifdef VIMDLL
	എങ്കിൽ (gui.in_use ? gui_mch_get_winpos(&x, &y) != FAIL :
		mch_get_winpos(&x, &y) != FAIL)
#  elif defined(FEAT_GUI)
	എങ്കിൽ (gui.in_use && gui_mch_get_winpos(&x, &y) != FAIL)
#  അല്ലെങ്കിൽ
	എങ്കിൽ (mch_get_winpos(&x, &y) != FAIL)
#  endif
	{
	    sprintf((അക്ഷരം *)IObuff, _("Window position: X %d, Y %d"), x, y);
	    സന്ദേശം((അക്ഷരം *)IObuff);
	}
	അല്ലെങ്കിൽ
# endif
	    emsg(_(e_obtaining_window_position_not_implemented_for_this_platform));
    }
    അല്ലെങ്കിൽ
    {
	x = getdigits(&arg);
	arg = skipwhite(arg);
	p = arg;
	y = getdigits(&arg);
	എങ്കിൽ (*p == NUL || *arg != NUL)
	{
	    emsg(_(e_winpos_requires_two_സംഖ്യ_arguments));
	    തിരികെ;
	}
# ifdef FEAT_GUI
	എങ്കിൽ (gui.in_use)
	    gui_mch_set_winpos(x, y);
	അല്ലെങ്കിൽ എങ്കിൽ (gui.starting)
	{
	    // Remember the coordinates for when the window is opened.
	    gui_win_x = x;
	    gui_win_y = y;
	}
#  എങ്കിൽ defined(HAVE_TGETENT) || defined(VIMDLL)
	അല്ലെങ്കിൽ
#  endif
# endif
# എങ്കിൽ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	    mch_set_winpos(x, y);
# endif
# ifdef HAVE_TGETENT
	എങ്കിൽ (*T_CWP)
	    term_set_winpos(x, y);
# endif
    }
}
#endif

/*
 * Handle command that work like operators: ":delete", ":yank", ":>" and ":<".
 */
    സ്ഥിതം ശൂന്യം
ex_operators(exarg_T *eap)
{
    oparg_T	oa;

    clear_oparg(&oa);
    oa.regname = eap->regname;
    oa.start.lnum = eap->line1;
    oa.end.lnum = eap->line2;
    oa.വരി_എണ്ണം = eap->line2 - eap->line1 + 1;
    oa.motion_type = MLINE;
    virtual_op = FALSE;
    എങ്കിൽ (eap->cmdidx != CMD_yank)	// position cursor for undo
    {
	setpcmark();
	curwin->w_cursor.lnum = eap->line1;
	beginline(BL_SOL | BL_FIX);
    }

    എങ്കിൽ (VIsual_active)
	end_visual_mode();

    തിരഞ്ഞെടുപ്പ് (eap->cmdidx)
    {
	അവസ്ഥ CMD_delete:
	    oa.op_type = OP_DELETE;
	    op_delete(&oa);
	    നിർത്തുക;

	അവസ്ഥ CMD_yank:
	    oa.op_type = OP_YANK;
	    (ശൂന്യം)op_yank(&oa, FALSE, TRUE);
	    നിർത്തുക;

	സ്ഥിരസ്ഥിതി:    // CMD_rshift or CMD_lshift
	    എങ്കിൽ (
#ifdef FEAT_RIGHTLEFT
		(eap->cmdidx == CMD_rshift) ^ curwin->w_p_rl
#അല്ലെങ്കിൽ
		eap->cmdidx == CMD_rshift
#endif
						)
		oa.op_type = OP_RSHIFT;
	    അല്ലെങ്കിൽ
		oa.op_type = OP_LSHIFT;
	    op_shift(&oa, FALSE, eap->amount);
	    നിർത്തുക;
    }
    virtual_op = MAYBE;
    ex_may_print(eap);
}

/*
 * ":put".
 */
    സ്ഥിതം ശൂന്യം
ex_put(exarg_T *eap)
{
    // ":0put" works like ":1put!".
    എങ്കിൽ (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, ശൂന്യ, eap->forceit ? BACKWARD : FORWARD, 1L,
						       PUT_വരി|PUT_CURSLINE);
}

/*
 * ":iput".
 */
    സ്ഥിതം ശൂന്യം
ex_iput(exarg_T *eap)
{
    // ":0iput" works like ":1iput!".
    എങ്കിൽ (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, ശൂന്യ, eap->forceit ? BACKWARD : FORWARD, 1L,
						      PUT_വരി|PUT_CURSLINE
						      |PUT_FIXINDENT);
}

/*
 * Handle ":copy" and ":move".
 */
    സ്ഥിതം ശൂന്യം
ex_copymove(exarg_T *eap)
{
    long	n;

#ifdef FEAT_EVAL
    എങ്കിൽ (not_in_vim9(eap, false) == FAIL)
	തിരികെ;
#endif
    n = get_address(eap, &eap->arg, eap->addr_type, FALSE, FALSE, FALSE, 1);
    എങ്കിൽ (eap->arg == ശൂന്യ)	    // error detected
    {
	eap->nextcmd = ശൂന്യ;
	തിരികെ;
    }
    get_flags(eap);

    /*
     * move or copy lines from 'eap->line1'-'eap->line2' to below line 'n'
     */
    എങ്കിൽ (n == MAXLNUM || n < 0 || n > curbuf->b_ml.ml_വരി_എണ്ണം)
    {
	emsg(_(e_invalid_range));
	തിരികെ;
    }

    എങ്കിൽ (eap->cmdidx == CMD_move)
    {
	എങ്കിൽ (do_move(eap->line1, eap->line2, n) == FAIL)
	    തിരികെ;
    }
    അല്ലെങ്കിൽ
	ex_copy(eap->line1, eap->line2, n);
    u_clearline();
    beginline(BL_SOL | BL_FIX);
    ex_may_print(eap);
}

/*
 * Print the current line if flags were given to the Ex command.
 */
    ശൂന്യം
ex_may_print(exarg_T *eap)
{
    എങ്കിൽ (eap->flags != 0)
    {
	print_വരി(curwin->w_cursor.lnum, (eap->flags & EXFLAG_NR),
						  (eap->flags & EXFLAG_LIST));
	ex_no_reprint = TRUE;
    }
}

/*
 * ":smagic" and ":snomagic".
 */
    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം ശൂന്യം
ex_join(exarg_T *eap)
{
    curwin->w_cursor.lnum = eap->line1;
    എങ്കിൽ (eap->line1 == eap->line2)
    {
	എങ്കിൽ (eap->addr_എണ്ണം >= 2)   // :2,2join does nothing
	    തിരികെ;
	എങ്കിൽ (eap->line2 == curbuf->b_ml.ml_വരി_എണ്ണം)
	{
	    beep_flush();
	    തിരികെ;
	}
	++eap->line2;
    }
    (ശൂന്യം)do_join(eap->line2 - eap->line1 + 1, !eap->forceit, TRUE, TRUE, TRUE);
    beginline(BL_WHITE | BL_FIX);
    ex_may_print(eap);
}

/*
 * ":[addr]@r" or ":[addr]*r": execute register
 */
    സ്ഥിതം ശൂന്യം
ex_at(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		prev_len = typebuf.tb_len;

    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif

    // get the register name.  No name means to use the previous one
    c = *eap->arg;
    എങ്കിൽ (c == NUL || (c == '*' && *eap->cmd == '*'))
	c = '@';
    // Put the register in the typeahead buffer with the "silent" flag.
    എങ്കിൽ (do_execreg(c, TRUE, vim_strchr(p_cpo, CPO_EXECBUF) != ശൂന്യ, TRUE)
								      == FAIL)
    {
	beep_flush();
	തിരികെ;
    }

    പൂർണ്ണസംഖ്യ	save_efr = exec_from_reg;

    exec_from_reg = TRUE;

    /*
     * Execute from the typeahead buffer.
     * Continue until the stuff buffer is empty and all added characters
     * have been consumed.
     */
    ആകുമ്പോൾ (!stuff_empty() || typebuf.tb_len > prev_len)
	(ശൂന്യം)do_cmdline(ശൂന്യ, getexline, ശൂന്യ, DOCMD_NOWAIT|DOCMD_VERBOSE);

    exec_from_reg = save_efr;
}

/*
 * ":!".
 */
    സ്ഥിതം ശൂന്യം
ex_bang(exarg_T *eap)
{
    do_bang(eap->addr_എണ്ണം, eap, eap->forceit, TRUE, TRUE);
}

/*
 * ":undo".
 */
    സ്ഥിതം ശൂന്യം
ex_undo(exarg_T *eap)
{
    എങ്കിൽ (eap->addr_എണ്ണം == 1)	    // :undo 123
	undo_time(eap->line2, FALSE, FALSE, TRUE);
    അല്ലെങ്കിൽ
	u_undo(1);
}

#ifdef FEAT_PERSISTENT_UNDO
    സ്ഥിതം ശൂന്യം
ex_wundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_SIZE];

    u_compute_hash(hash);
    u_write_undo(eap->arg, eap->forceit, curbuf, hash);
}

    സ്ഥിതം ശൂന്യം
ex_rundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_SIZE];

    u_compute_hash(hash);
    u_read_undo(eap->arg, hash, ശൂന്യ);
}
#endif

/*
 * ":redo".
 */
    സ്ഥിതം ശൂന്യം
ex_redo(exarg_T *eap UNUSED)
{
    u_redo(1);
}

/*
 * ":earlier" and ":later".
 */
    സ്ഥിതം ശൂന്യം
ex_later(exarg_T *eap)
{
    long	എണ്ണം = 0;
    പൂർണ്ണസംഖ്യ		sec = FALSE;
    പൂർണ്ണസംഖ്യ		file = FALSE;
    char_u	*p = eap->arg;

    എങ്കിൽ (*p == NUL)
	എണ്ണം = 1;
    അല്ലെങ്കിൽ എങ്കിൽ (SAFE_isdigit(*p))
    {
	എണ്ണം = getdigits(&p);
	തിരഞ്ഞെടുപ്പ് (*p)
	{
	    അവസ്ഥ 's': ++p; sec = TRUE; നിർത്തുക;
	    അവസ്ഥ 'm': ++p; sec = TRUE; എണ്ണം *= 60; നിർത്തുക;
	    അവസ്ഥ 'h': ++p; sec = TRUE; എണ്ണം *= 60 * 60; നിർത്തുക;
	    അവസ്ഥ 'd': ++p; sec = TRUE; എണ്ണം *= 24 * 60 * 60; നിർത്തുക;
	    അവസ്ഥ 'f': ++p; file = TRUE; നിർത്തുക;
	}
    }

    എങ്കിൽ (*p != NUL)
	semsg(_(e_invalid_argument_str), eap->arg);
    അല്ലെങ്കിൽ
	undo_time(eap->cmdidx == CMD_earlier ? -എണ്ണം : എണ്ണം,
							    sec, file, FALSE);
}

/*
 * ":redir": start/stop redirection.
 */
    സ്ഥിതം ശൂന്യം
ex_redir(exarg_T *eap)
{
    അക്ഷരം	*mode;
    char_u	*fname;
    char_u	*arg = eap->arg;

#ifdef FEAT_EVAL
    എങ്കിൽ (redir_execute)
    {
	emsg(_(e_cannot_use_redir_inside_execute));
	തിരികെ;
    }
#endif

    എങ്കിൽ (STRICMP(eap->arg, "END") == 0)
	close_redir();
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (*arg == '>')
	{
	    ++arg;
	    എങ്കിൽ (*arg == '>')
	    {
		++arg;
		mode = "a";
	    }
	    അല്ലെങ്കിൽ
		mode = "w";
	    arg = skipwhite(arg);

	    close_redir();

	    // Expand environment variables and "~/".
	    fname = expand_env_save(arg);
	    എങ്കിൽ (fname == ശൂന്യ)
		തിരികെ;
#ifdef FEAT_BROWSE
	    എങ്കിൽ (cmdmod.cmod_flags & CMOD_BROWSE)
	    {
		char_u	*browseFile;

		browseFile = do_browse(BROWSE_SAVE,
			(char_u *)_("Save Redirection"),
			fname, ശൂന്യ, ശൂന്യ,
			(char_u *)_(BROWSE_FILTER_ALL_FILES), curbuf);
		എങ്കിൽ (browseFile == ശൂന്യ)
		    തിരികെ;		// operation cancelled
		vim_free(fname);
		fname = browseFile;
		eap->forceit = TRUE;	// since dialog already asked
	    }
#endif

	    redir_fd = open_exfile(fname, eap->forceit, mode);
	    vim_free(fname);
	}
#ifdef FEAT_EVAL
	അല്ലെങ്കിൽ എങ്കിൽ (*arg == '@')
	{
	    // redirect to a register a-z (resp. A-Z for appending)
	    close_redir();
	    ++arg;
	    എങ്കിൽ (ASCII_ISALPHA(*arg)
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
		എങ്കിൽ (*arg == '>' && arg[1] == '>')  // append
		    arg += 2;
		അല്ലെങ്കിൽ
		{
		    // Can use both "@a" and "@a>".
		    എങ്കിൽ (*arg == '>')
			arg++;
		    // Make register empty when not using @A-@Z and the
		    // command is valid.
		    എങ്കിൽ (*arg == NUL && !SAFE_isupper(redir_reg))
			write_reg_contents(redir_reg, (char_u *)"", -1, FALSE);
		}
	    }
	    എങ്കിൽ (*arg != NUL)
	    {
		redir_reg = 0;
		semsg(_(e_invalid_argument_str), eap->arg);
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*arg == '=' && arg[1] == '>')
	{
	    പൂർണ്ണസംഖ്യ append;

	    // redirect to a variable
	    close_redir();
	    arg += 2;

	    എങ്കിൽ (*arg == '>')
	    {
		++arg;
		append = TRUE;
	    }
	    അല്ലെങ്കിൽ
		append = FALSE;

	    എങ്കിൽ (var_redir_start(skipwhite(arg), append) == OK)
		redir_vname = 1;
	}
#endif

	// TODO: redirect to a buffer

	അല്ലെങ്കിൽ
	    semsg(_(e_invalid_argument_str), eap->arg);
    }

    // Make sure redirection is not off.  Can happen for cmdline completion
    // that indirectly invokes a command to catch its output.
    എങ്കിൽ (redir_fd != ശൂന്യ
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname
#endif
							)
	redir_off = FALSE;
}

/*
 * ":redraw": force redraw, with clear for ":redraw!".
 */
    ശൂന്യം
ex_redraw(exarg_T *eap)
{
    redraw_cmd(eap->forceit);
}

/*
 * ":redraw": force redraw, with clear if "clear" is TRUE.
 */
    ശൂന്യം
redraw_cmd(പൂർണ്ണസംഖ്യ clear)
{
    പൂർണ്ണസംഖ്യ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    പൂർണ്ണസംഖ്യ save_p_lz = p_lz;
    p_lz = FALSE;

    validate_cursor();
    update_topline();
    update_screen(clear ? UPD_CLEAR : VIsual_active ? UPD_INVERTED : 0);
    എങ്കിൽ ((State & MODE_CMDLINE) == 0)
	setcursor(); // put cursor back where it belongs
    എങ്കിൽ (need_maketitle)
	maketitle();
#എങ്കിൽ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
    എങ്കിൽ (!gui.in_use)
# endif
	resize_console_buf();
#endif
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;

    // After drawing the statusline screen_attr may still be set.
    screen_stop_highlight();

    // Reset msg_didout, so that a message that's there is overwritten.
    സന്ദേശം_didout = FALSE;
    സന്ദേശം_col = 0;

    // No need to wait after an intentional redraw.
    need_wait_return = FALSE;

    // When invoked from a callback or autocmd the command line may be active.
    എങ്കിൽ (State & MODE_CMDLINE)
	redrawcmdline();

    out_flush();
}

/*
 * ":redrawstatus": force redraw of status line(s)
 */
    സ്ഥിതം ശൂന്യം
ex_redrawstatus(exarg_T *eap UNUSED)
{
    എങ്കിൽ (eap->forceit)
	status_redraw_all();
    അല്ലെങ്കിൽ
	status_redraw_curbuf();
    redraw_vseps = TRUE;
    എങ്കിൽ (സന്ദേശം_scrolled && (State & MODE_CMDLINE))
	തിരികെ;  // redraw later

    പൂർണ്ണസംഖ്യ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    പൂർണ്ണസംഖ്യ save_p_lz = p_lz;
    p_lz = FALSE;

    എങ്കിൽ (State & MODE_CMDLINE)
	redraw_statuslines();
    അല്ലെങ്കിൽ
	update_screen(VIsual_active ? UPD_INVERTED : 0);
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabline": force redraw of the tabline
 */
    സ്ഥിതം ശൂന്യം
ex_redrawtabline(exarg_T *eap UNUSED)
{
    പൂർണ്ണസംഖ്യ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    പൂർണ്ണസംഖ്യ save_p_lz = p_lz;
    p_lz = FALSE;

    draw_tabline();

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabpanel": force redraw of the tabpanel
 */
    സ്ഥിതം ശൂന്യം
ex_redrawtabpanel(exarg_T *eap UNUSED)
{
    പൂർണ്ണസംഖ്യ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    പൂർണ്ണസംഖ്യ save_p_lz = p_lz;
    p_lz = FALSE;

#എങ്കിൽ defined(FEAT_TABPANEL)
    draw_tabpanel();
#അല്ലെങ്കിൽ
    emsg(_(e_cannot_not_support_redrawtabpanel));
#endif

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

    സ്ഥിതം ശൂന്യം
close_redir(ശൂന്യം)
{
    എങ്കിൽ (redir_fd != ശൂന്യ)
    {
	fclose(redir_fd);
	redir_fd = ശൂന്യ;
    }
#ifdef FEAT_EVAL
    redir_reg = 0;
    എങ്കിൽ (redir_vname)
    {
	var_redir_stop();
	redir_vname = 0;
    }
#endif
}

#എങ്കിൽ defined(FEAT_SESSION) || defined(FEAT_EVAL)
    പൂർണ്ണസംഖ്യ
vim_mkdir_emsg(char_u *name, പൂർണ്ണസംഖ്യ prot UNUSED)
{
    എങ്കിൽ (vim_mkdir(name, prot) != 0)
    {
	semsg(_(e_cannot_create_directory_str), name);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}
#endif

/*
 * Open a file for writing for an Ex command, with some checks.
 * Return file descriptor, or NULL on failure.
 */
    FILE *
open_exfile(
    char_u	*fname,
    പൂർണ്ണസംഖ്യ		forceit,
    അക്ഷരം	*mode)	    // "w" for create new file or "a" for append
{
    FILE	*fd;

#ifdef UNIX
    // with Unix it is possible to open a directory
    എങ്കിൽ (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	തിരികെ ശൂന്യ;
    }
#endif
    എങ്കിൽ (!forceit && *mode != 'a' && vim_fexists(fname))
    {
	semsg(_(e_str_exists_add_bang_to_override), fname);
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ ((fd = mch_fopen((അക്ഷരം *)fname, mode)) == ശൂന്യ)
	semsg(_(e_cannot_open_str_for_writing_2), fname);

    തിരികെ fd;
}

/*
 * ":mark" and ":k".
 */
    സ്ഥിതം ശൂന്യം
ex_mark(exarg_T *eap)
{
    pos_T	pos;

#ifdef FEAT_EVAL
    എങ്കിൽ (not_in_vim9(eap, false) == FAIL)
	തിരികെ;
#endif
    എങ്കിൽ (*eap->arg == NUL)		// No argument?
    {
	emsg(_(e_argument_required));
	തിരികെ;
    }

    എങ്കിൽ (eap->arg[1] != NUL)	// more than one character?
    {
	semsg(_(e_trailing_characters_str), eap->arg);
	തിരികെ;
    }

    pos = curwin->w_cursor;		// save curwin->w_cursor
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_WHITE | BL_FIX);
    എങ്കിൽ (setmark(*eap->arg) == FAIL)	// set mark
	emsg(_(e_argument_must_be_letter_or_forward_backward_quote));
    curwin->w_cursor = pos;		// restore curwin->w_cursor
}

/*
 * Update w_topline, w_leftcol and the cursor position.
 */
    ശൂന്യം
update_topline_cursor(ശൂന്യം)
{
    check_cursor();		// put cursor on valid line
    update_topline();
    എങ്കിൽ (!curwin->w_p_wrap)
	validate_cursor();
    update_curswant();
}

/*
 * Save the current State and go to Normal mode.
 * Return TRUE if the typeahead could be saved.
 */
    പൂർണ്ണസംഖ്യ
save_current_state(save_state_T *sst)
{
    sst->save_സന്ദേശം_scroll = സന്ദേശം_scroll;
    sst->save_restart_edit = restart_edit;
    sst->save_സന്ദേശം_didout = സന്ദേശം_didout;
    sst->save_State = State;
    sst->save_insertmode = p_im;
    sst->save_finish_op = finish_op;
    sst->save_opcount = opcount;
    sst->save_reg_executing = reg_executing;
    sst->save_pending_end_reg_executing = pending_end_reg_executing;

    സന്ദേശം_scroll = FALSE;		    // no msg scrolling in Normal mode
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
    തിരികെ sst->tabuf.typebuf_valid;
}

    ശൂന്യം
restore_current_state(save_state_T *sst)
{
    // Restore the previous typeahead.
    restore_typeahead(&sst->tabuf, FALSE);

    സന്ദേശം_scroll = sst->save_സന്ദേശം_scroll;
    restart_edit = sst->save_restart_edit;
    p_im = sst->save_insertmode;
    finish_op = sst->save_finish_op;
    opcount = sst->save_opcount;
    reg_executing = sst->save_reg_executing;
    pending_end_reg_executing = sst->save_pending_end_reg_executing;
    സന്ദേശം_didout |= sst->save_സന്ദേശം_didout;	// don't reset msg_didout now
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
    ശൂന്യം
ex_normal(exarg_T *eap)
{
    save_state_T save_state;
    char_u	*arg = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		l;
    char_u	*p;

    എങ്കിൽ (ex_normal_lock > 0)
    {
	emsg(_(e_not_allowed_here));
	തിരികെ;
    }
    എങ്കിൽ (ex_normal_busy >= p_mmd)
    {
	emsg(_(e_recursive_use_of_normal_too_deep));
	തിരികെ;
    }

    /*
     * vgetc() expects a CSI and K_SPECIAL to have been escaped.  Don't do
     * this for the K_SPECIAL leading byte, otherwise special keys will not
     * work.
     */
    എങ്കിൽ (has_mbyte)
    {
	പൂർണ്ണസംഖ്യ	len = 0;

	// Count the number of characters to be escaped.
	വേണ്ടി (p = eap->arg; *p != NUL; ++p)
	{
#ifdef FEAT_GUI
	    എങ്കിൽ (*p == CSI)  // leadbyte CSI
		len += 2;
#endif
	    വേണ്ടി (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		എങ്കിൽ (*++p == K_SPECIAL	  // trailbyte K_SPECIAL or CSI
#ifdef FEAT_GUI
			|| *p == CSI
#endif
			)
		    len += 2;
	}
	എങ്കിൽ (len > 0)
	{
	    arg = alloc(STRLEN(eap->arg) + len + 1);
	    എങ്കിൽ (arg != ശൂന്യ)
	    {
		len = 0;
		വേണ്ടി (p = eap->arg; *p != NUL; ++p)
		{
		    arg[len++] = *p;
#ifdef FEAT_GUI
		    എങ്കിൽ (*p == CSI)
		    {
			arg[len++] = KS_EXTRA;
			arg[len++] = (പൂർണ്ണസംഖ്യ)KE_CSI;
		    }
#endif
		    വേണ്ടി (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		    {
			arg[len++] = *++p;
			എങ്കിൽ (*p == K_SPECIAL)
			{
			    arg[len++] = KS_SPECIAL;
			    arg[len++] = KE_FILLER;
			}
#ifdef FEAT_GUI
			അല്ലെങ്കിൽ എങ്കിൽ (*p == CSI)
			{
			    arg[len++] = KS_EXTRA;
			    arg[len++] = (പൂർണ്ണസംഖ്യ)KE_CSI;
			}
#endif
		    }
		    arg[len] = NUL;
		}
	    }
	}
    }

    ++ex_normal_busy;
    എങ്കിൽ (save_current_state(&save_state))
    {
	/*
	 * Repeat the :normal command for each line in the range.  When no
	 * range given, execute it just once, without positioning the cursor
	 * first.
	 */
	ചെയ്യുക
	{
	    എങ്കിൽ (eap->addr_എണ്ണം != 0)
	    {
		curwin->w_cursor.lnum = eap->line1++;
		curwin->w_cursor.col = 0;
		check_cursor_moved(curwin);
	    }

	    exec_normal_cmd(arg != ശൂന്യ
		     ? arg
		     : eap->arg, eap->forceit ? REMAP_NONE : REMAP_YES, FALSE);
	}
	ആകുമ്പോൾ (eap->addr_എണ്ണം > 0 && eap->line1 <= eap->line2 && !got_int);
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
    സ്ഥിതം ശൂന്യം
ex_startinsert(exarg_T *eap)
{
#ifdef FEAT_TERMINAL
    // Ignore this when running in an active terminal.
    എങ്കിൽ (term_job_running(curbuf->b_term))
	തിരികെ;
#endif
    എങ്കിൽ (!curbuf->b_p_ma && !p_im)
    {
	// Only give this error when 'insertmode' is off.
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	തിരികെ;
    }
    എങ്കിൽ (eap->forceit)
    {
	// cursor line can be zero on startup
	എങ്കിൽ (!curwin->w_cursor.lnum)
	    curwin->w_cursor.lnum = 1;
	set_cursor_for_append_to_വരി();
    }

    // Ignore the command when already in Insert mode.  Inserting an
    // expression register that invokes a function can do this.
    എങ്കിൽ (State & MODE_തിരുകുക)
	തിരികെ;

    എങ്കിൽ (eap->cmdidx == CMD_startinsert)
	restart_edit = 'a';
    അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_startreplace)
	restart_edit = 'R';
    അല്ലെങ്കിൽ
	restart_edit = 'V';

    എങ്കിൽ (!eap->forceit)
    {
	എങ്കിൽ (eap->cmdidx == CMD_startinsert)
	    restart_edit = 'i';
	curwin->w_curswant = 0;	    // avoid MAXCOL
    }

    എങ്കിൽ (VIsual_active)
	showmode();
}

/*
 * ":stopinsert"
 */
    സ്ഥിതം ശൂന്യം
ex_stopinsert(exarg_T *eap UNUSED)
{
    restart_edit = 0;
    stop_തിരുകുക_mode = TRUE;
#എങ്കിൽ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // when called from remote_expr in insert mode, make sure insert mode is
    // ended by adding K_NOP to the typeahead buffer
    എങ്കിൽ (vgetc_busy)
	ins_char_typebuf(K_NOP, 0);
#endif
    clearmode();
}

/*
 * Execute normal mode command "cmd".
 * "remap" can be REMAP_NONE or REMAP_YES.
 */
    ശൂന്യം
exec_normal_cmd(char_u *cmd, പൂർണ്ണസംഖ്യ remap, പൂർണ്ണസംഖ്യ silent)
{
    // Stuff the argument into the typeahead buffer.
    ins_typebuf(cmd, remap, 0, TRUE, silent);
    exec_normal(FALSE, FALSE, FALSE);
}

/*
 * Execute normal_cmd() until there is no typeahead left.
 * When "use_vpeekc" is TRUE use vpeekc() to check for available chars.
 */
    ശൂന്യം
exec_normal(പൂർണ്ണസംഖ്യ was_typed, പൂർണ്ണസംഖ്യ use_vpeekc, പൂർണ്ണസംഖ്യ may_use_terminal_loop UNUSED)
{
    oparg_T	oa;
    പൂർണ്ണസംഖ്യ		c;

    // When calling vpeekc() from feedkeys() it will return Ctrl_C when there
    // is nothing to get, so also check for Ctrl_C.
    clear_oparg(&oa);
    finish_op = FALSE;
    ആകുമ്പോൾ ((!stuff_empty()
		|| ((was_typed || !typebuf_typed()) && typebuf.tb_len > 0)
		|| (use_vpeekc && (c = vpeekc()) != NUL && c != Ctrl_C))
	    && !got_int)
    {
	update_topline_cursor();
#ifdef FEAT_TERMINAL
	എങ്കിൽ (may_use_terminal_loop && term_use_loop()
		&& oa.op_type == OP_NOP && oa.regname == NUL
		&& !VIsual_active)
	{
	    // If terminal_loop() returns OK we got a key that is handled
	    // in Normal model.  With FAIL we first need to position the
	    // cursor and the screen needs to be redrawn.
	    എങ്കിൽ (terminal_loop(TRUE) == OK)
		normal_cmd(&oa, TRUE);
	}
	അല്ലെങ്കിൽ
#endif
	    // execute a Normal mode cmd
	    normal_cmd(&oa, TRUE);
    }
}

#ifdef FEAT_FIND_ID
    സ്ഥിതം ശൂന്യം
ex_checkpath(exarg_T *eap)
{
    find_pattern_in_path(ശൂന്യ, 0, 0, FALSE, FALSE, CHECK_PATH, 1L,
	    eap->forceit ? ACTION_SHOW_ALL : ACTION_SHOW,
	    (linenr_T)1, (linenr_T)MAXLNUM, eap->forceit, FALSE);
}

# എങ്കിൽ defined(FEAT_QUICKFIX)
/*
 * ":psearch"
 */
    സ്ഥിതം ശൂന്യം
ex_psearch(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;
    ex_findpat(eap);
    g_do_tagpreview = 0;
}
# endif

    സ്ഥിതം ശൂന്യം
ex_findpat(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ		whole = TRUE;
    long	n;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		action;

    തിരഞ്ഞെടുപ്പ് (cmdnames[eap->cmdidx].cmd_name[2])
    {
	അവസ്ഥ 'e':	// ":psearch", ":isearch" and ":dsearch"
		എങ്കിൽ (cmdnames[eap->cmdidx].cmd_name[0] == 'p')
		    action = ACTION_GOTO;
		അല്ലെങ്കിൽ
		    action = ACTION_SHOW;
		നിർത്തുക;
	അവസ്ഥ 'i':	// ":ilist" and ":dlist"
		action = ACTION_SHOW_ALL;
		നിർത്തുക;
	അവസ്ഥ 'u':	// ":ijump" and ":djump"
		action = ACTION_GOTO;
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:	// ":isplit" and ":dsplit"
		action = ACTION_SPLIT;
		നിർത്തുക;
    }

    n = 1;
    എങ്കിൽ (vim_isdigit(*eap->arg))	// get count
    {
	n = getdigits(&eap->arg);
	eap->arg = skipwhite(eap->arg);
    }
    എങ്കിൽ (*eap->arg == '/')   // Match regexp, not just whole words
    {
	whole = FALSE;
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	എങ്കിൽ (*p)
	{
	    *p++ = NUL;
	    p = skipwhite(p);

	    // Check for trailing illegal characters
	    എങ്കിൽ (!ends_excmd2(eap->arg, p))
		eap->errmsg = ex_errmsg(e_trailing_characters_str, p);
	    അല്ലെങ്കിൽ
		set_nextcmd(eap, p);
	}
    }
    എങ്കിൽ (!eap->skip)
	find_pattern_in_path(eap->arg, 0, (പൂർണ്ണസംഖ്യ)STRLEN(eap->arg),
		whole, !eap->forceit,
		*eap->cmd == 'd' ? FIND_DEFINE : FIND_ANY, n, action,
		eap->line1, eap->line2, eap->forceit, FALSE);
}
#endif


#ifdef FEAT_QUICKFIX
/*
 * ":ptag", ":ptselect", ":ptjump", ":ptnext", etc.
 */
    സ്ഥിതം ശൂന്യം
ex_ptag(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;  // will be reset to 0 in ex_tag_cmd()
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
}

/*
 * ":pedit"
 */
    സ്ഥിതം ശൂന്യം
ex_pedit(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Edit the file.
    do_exedit(eap, ശൂന്യ);

    back_to_current_window(curwin_save);
}

/*
 * ":pbuffer"
 */
    സ്ഥിതം ശൂന്യം
ex_pbuffer(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Go to the buffer.
    do_exbuffer(eap);

    back_to_current_window(curwin_save);
}

    സ്ഥിതം ശൂന്യം
prepare_preview_window(ശൂന്യം)
{
    എങ്കിൽ (ERROR_IF_ANY_POPUP_WINDOW)
	തിരികെ;

    // Open the preview window or popup and make it the current window.
    g_do_tagpreview = p_pvh;
    prepare_tagpreview(TRUE, TRUE, FALSE);
}

    സ്ഥിതം ശൂന്യം
back_to_current_window(win_T *curwin_save)
{
    എങ്കിൽ (curwin != curwin_save && win_valid(curwin_save))
    {
	// Return cursor to where we were
	validate_cursor();
	redraw_later(UPD_VALID);
	win_enter(curwin_save, TRUE);
    }
# ifdef FEAT_PROP_POPUP
    അല്ലെങ്കിൽ എങ്കിൽ (WIN_IS_POPUP(curwin))
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
    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം ശൂന്യം
ex_tag(exarg_T *eap)
{
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name);
}

    സ്ഥിതം ശൂന്യം
ex_tag_cmd(exarg_T *eap, char_u *name)
{
    പൂർണ്ണസംഖ്യ		cmd;

    തിരഞ്ഞെടുപ്പ് (name[1])
    {
	അവസ്ഥ 'j': cmd = DT_JUMP;	// ":tjump"
		  നിർത്തുക;
	അവസ്ഥ 's': cmd = DT_SELECT;	// ":tselect"
		  നിർത്തുക;
	അവസ്ഥ 'p': cmd = DT_PREV;	// ":tprevious"
		  നിർത്തുക;
	അവസ്ഥ 'N': cmd = DT_PREV;	// ":tNext"
		  നിർത്തുക;
	അവസ്ഥ 'n': cmd = DT_NEXT;	// ":tnext"
		  നിർത്തുക;
	അവസ്ഥ 'o': cmd = DT_എടുക്കുക;		// ":pop"
		  നിർത്തുക;
	അവസ്ഥ 'f':			// ":tfirst"
	അവസ്ഥ 'r': cmd = DT_FIRST;	// ":trewind"
		  നിർത്തുക;
	അവസ്ഥ 'l': cmd = DT_LAST;	// ":tlast"
		  നിർത്തുക;
	സ്ഥിരസ്ഥിതി:			// ":tag"
#ifdef FEAT_CSCOPE
		  എങ്കിൽ (p_cst && *eap->arg != NUL)
		  {
		      ex_cstag(eap);
		      തിരികെ;
		  }
#endif
		  cmd = DT_TAG;
		  നിർത്തുക;
    }

    എങ്കിൽ (name[0] == 'l')
    {
#ifndef FEAT_QUICKFIX
	ex_ni(eap);
	തിരികെ;
#അല്ലെങ്കിൽ
	cmd = DT_LTAG;
#endif
    }

    do_tag(eap->arg, cmd, eap->addr_എണ്ണം > 0 ? (പൂർണ്ണസംഖ്യ)eap->line2 : 1,
							  eap->forceit, TRUE);
}

എണ്ണൽ {
    SPEC_PERC = 0,
    SPEC_HASH,
    SPEC_CWORD,	    // cursor word
    SPEC_CCWORD,    // cursor WORD
    SPEC_CEXPR,	    // expr under cursor
    SPEC_CFILE,	    // cursor path name
    SPEC_SFILE,	    // ":so" file name
    SPEC_SLNUM,	    // ":so" file line number
    SPEC_അട്ടി,	    // call stack
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
    പൂർണ്ണസംഖ്യ
find_cmdline_var(char_u *src, size_t *usedlen)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    സ്ഥിതം keyvalue_T spec_str_tab[] = {
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
	KEYVALUE_ENTRY(SPEC_അട്ടി, "stack>")	    // call stack
    };
    keyvalue_T target;
    keyvalue_T *entry;

    തിരഞ്ഞെടുപ്പ് (*src)
    {
    അവസ്ഥ '%':
	*usedlen = 1;
	തിരികെ SPEC_PERC;

    അവസ്ഥ '#':
	*usedlen = 1;
	തിരികെ SPEC_HASH;

    അവസ്ഥ '<':
	target.താക്കോൽ = 0;
	target.മൂല്യം.string = src + 1;	    // skip over '<'
	target.മൂല്യം.length = 0;	    // not used, see cmp_keyvalue_value_n()

	entry = (keyvalue_T *)bsearch(&target, &spec_str_tab,
		ARRAY_LENGTH(spec_str_tab), വലുപ്പം(spec_str_tab[0]),
		cmp_keyvalue_മൂല്യം_n);
	എങ്കിൽ (entry == ശൂന്യ)
	    തിരികെ -1;

	*usedlen = entry->മൂല്യം.length + 1;
	തിരികെ entry->താക്കോൽ;

    സ്ഥിരസ്ഥിതി:
	നിർത്തുക;
    }

    തിരികെ -1;
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
    അക്ഷരം	**errormsg,	// pointer to error message
    പൂർണ്ണസംഖ്യ		*escaped,	// return value has escaped white space (can
				// be NULL)
    പൂർണ്ണസംഖ്യ		empty_is_error)	// empty result is considered an error
{
    പൂർണ്ണസംഖ്യ		i;
    char_u	*s;
    char_u	*result = (char_u *)"";
    char_u	*resultbuf = ശൂന്യ;
    size_t	resultlen;
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		valid = VALID_HEAD + VALID_PATH;    // assume valid result
    പൂർണ്ണസംഖ്യ		spec_idx;
    പൂർണ്ണസംഖ്യ		tilde_file = FALSE;
    പൂർണ്ണസംഖ്യ		skip_mod = FALSE;
    char_u	strbuf[30];

    *errormsg = ശൂന്യ;
    എങ്കിൽ (escaped != ശൂന്യ)
	*escaped = FALSE;

    /*
     * Check if there is something to do.
     */
    spec_idx = find_cmdline_var(src, usedlen);
    എങ്കിൽ (spec_idx < 0)	// no match
    {
	*usedlen = 1;
	തിരികെ ശൂന്യ;
    }

    /*
     * Skip when preceded with a backslash "\%" and "\#".
     * Note: In "\\%" the % is also not recognized!
     */
    എങ്കിൽ (src > srcstart && src[-1] == '\\')
    {
	*usedlen = 0;
	STRMOVE(src - 1, src);	// remove backslash
	തിരികെ ശൂന്യ;
    }

    /*
     * word or WORD under cursor
     */
    എങ്കിൽ (spec_idx == SPEC_CWORD || spec_idx == SPEC_CCWORD
						     || spec_idx == SPEC_CEXPR)
    {
	resultlen = find_ident_under_cursor(&result,
		spec_idx == SPEC_CWORD ? (FIND_IDENT | FIND_STRING)
	      : spec_idx == SPEC_CEXPR ? (FIND_IDENT | FIND_STRING | FIND_EVAL)
	      : FIND_STRING);
	എങ്കിൽ (resultlen == 0)
	{
	    *errormsg = "";
	    തിരികെ ശൂന്യ;
	}
    }

    /*
     * '#': Alternate file name
     * '%': Current file name
     *	    File name under the cursor
     *	    File name for autocommand
     *	and following modifiers
     */
    അല്ലെങ്കിൽ
    {
	size_t off = 0;

	തിരഞ്ഞെടുപ്പ് (spec_idx)
	{
	അവസ്ഥ SPEC_PERC:
#ifdef FEAT_EVAL
		എങ്കിൽ (!in_vim9script() || src[1] != '%')
#endif
		{
		    // '%': current file
		    എങ്കിൽ (curbuf->b_fname == ശൂന്യ)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    അല്ലെങ്കിൽ
		    {
			result = curbuf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		    നിർത്തുക;
		}
#ifdef FEAT_EVAL
		// "%%" alternate file
		off = 1;
#endif
		// FALLTHROUGH
	അവസ്ഥ SPEC_HASH:		// '#' or "#99": alternate file
		എങ്കിൽ (off == 0 ? src[1] == '#' : src[2] == '%')
		{
		    // "##" or "%%%": the argument list
		    result = arg_all();
		    resultbuf = result;
		    *usedlen = off + 2;
		    എങ്കിൽ (escaped != ശൂന്യ)
			*escaped = TRUE;
		    skip_mod = TRUE;
		    നിർത്തുക;
		}
		s = src + off + 1;
		എങ്കിൽ (*s == '<')		// "#<99" uses v:oldfiles
		    ++s;
		i = (പൂർണ്ണസംഖ്യ)getdigits(&s);
		എങ്കിൽ (s == src + off + 2 && src[off + 1] == '-')
		    // just a minus sign, don't skip over it
		    s--;
		*usedlen = (പൂർണ്ണസംഖ്യ)(s - src); // length of what we expand

		എങ്കിൽ (src[off + 1] == '<' && i != 0)
		{
		    എങ്കിൽ (*usedlen < off + 2)
		    {
			// Should we give an error message for #<text?
			*usedlen = off + 1;
			തിരികെ ശൂന്യ;
		    }
#ifdef FEAT_EVAL
		    result = list_find_str(get_vim_var_list(VV_OLDFILES),
								     (long)i);
		    എങ്കിൽ (result == ശൂന്യ)
		    {
			*errormsg = "";
			തിരികെ ശൂന്യ;
		    }
#അല്ലെങ്കിൽ
		    *errormsg = _(e_hashsmall_is_not_available_without_the_eval_feature);
		    തിരികെ ശൂന്യ;
#endif
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (i == 0 && src[off + 1] == '<' && *usedlen > off + 1)
			*usedlen = off + 1;
		    buf = buflist_findnr(i);
		    എങ്കിൽ (buf == ശൂന്യ)
		    {
			*errormsg = _(e_no_alternate_file_name_to_substitute_for_hash);
			തിരികെ ശൂന്യ;
		    }
		    എങ്കിൽ (lnump != ശൂന്യ)
			*lnump = ECMD_LAST;
		    എങ്കിൽ (buf->b_fname == ശൂന്യ)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    അല്ലെങ്കിൽ
		    {
			result = buf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		}
		നിർത്തുക;

	അവസ്ഥ SPEC_CFILE:	// file name under cursor
		result = file_name_at_cursor(FNAME_MESS|FNAME_HYP, 1L, ശൂന്യ);
		എങ്കിൽ (result == ശൂന്യ)
		{
		    *errormsg = "";
		    തിരികെ ശൂന്യ;
		}
		resultbuf = result;	    // remember allocated string
		നിർത്തുക;

	അവസ്ഥ SPEC_AFILE:	// file name for autocommand
		result = autocmd_fname;
		എങ്കിൽ (result != ശൂന്യ && !autocmd_fname_full)
		{
		    // Still need to turn the fname into a full path.  It is
		    // postponed to avoid a delay when <afile> is not used.
		    autocmd_fname_full = TRUE;
		    result = FullName_save(autocmd_fname, FALSE);
		    vim_free(autocmd_fname);
		    autocmd_fname = result;
		}
		എങ്കിൽ (result == ശൂന്യ)
		{
		    *errormsg = _(e_no_autocommand_file_name_to_substitute_for_afile);
		    തിരികെ ശൂന്യ;
		}
		result = shorten_fname1(result);
		നിർത്തുക;

	അവസ്ഥ SPEC_ABUF:		// buffer number for autocommand
		എങ്കിൽ (autocmd_bufnr <= 0)
		{
		    *errormsg = _(e_no_autocommand_buffer_സംഖ്യ_to_substitute_for_abuf);
		    തിരികെ ശൂന്യ;
		}
		sprintf((അക്ഷരം *)strbuf, "%d", autocmd_bufnr);
		result = strbuf;
		നിർത്തുക;

	അവസ്ഥ SPEC_AMATCH:	// match name for autocommand
		result = autocmd_match;
		എങ്കിൽ (result == ശൂന്യ)
		{
		    *errormsg = _(e_no_autocommand_match_name_to_substitute_for_amatch);
		    തിരികെ ശൂന്യ;
		}
		നിർത്തുക;

	അവസ്ഥ SPEC_SFILE:	// file name for ":so" command
		result = estack_sfile(ESTACK_SFILE);
		എങ്കിൽ (result == ശൂന്യ)
		{
		    *errormsg = _(e_no_source_file_name_to_substitute_for_sfile);
		    തിരികെ ശൂന്യ;
		}
		resultbuf = result;	    // remember allocated string
		നിർത്തുക;
	അവസ്ഥ SPEC_അട്ടി:	// call stack
		result = estack_sfile(ESTACK_അട്ടി);
		എങ്കിൽ (result == ശൂന്യ)
		{
		    *errormsg = _(e_no_call_അട്ടി_to_substitute_for_അട്ടി);
		    തിരികെ ശൂന്യ;
		}
		resultbuf = result;	    // remember allocated string
		നിർത്തുക;
	അവസ്ഥ SPEC_SCRIPT:	// script file name
		result = estack_sfile(ESTACK_SCRIPT);
		എങ്കിൽ (result == ശൂന്യ)
		{
		    *errormsg = _(e_no_script_file_name_to_substitute_for_script);
		    തിരികെ ശൂന്യ;
		}
		resultbuf = result;	    // remember allocated string
		നിർത്തുക;

	അവസ്ഥ SPEC_SLNUM:	// line in file for ":so" command
		എങ്കിൽ (SOURCING_NAME == ശൂന്യ || SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_വരി_സംഖ്യ_to_use_for_slnum);
		    തിരികെ ശൂന്യ;
		}
		sprintf((അക്ഷരം *)strbuf, "%ld", SOURCING_LNUM);
		result = strbuf;
		നിർത്തുക;

#ifdef FEAT_EVAL
	അവസ്ഥ SPEC_SFLNUM:	// line in script file
		എങ്കിൽ (current_sctx.sc_lnum + SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_വരി_സംഖ്യ_to_use_for_sflnum);
		    തിരികെ ശൂന്യ;
		}
		sprintf((അക്ഷരം *)strbuf, "%ld",
				 (long)(current_sctx.sc_lnum + SOURCING_LNUM));
		result = strbuf;
		നിർത്തുക;

	അവസ്ഥ SPEC_SID:
		എങ്കിൽ (current_sctx.sc_sid <= 0)
		{
		    *errormsg = _(e_using_sid_not_in_script_context);
		    തിരികെ ശൂന്യ;
		}
		sprintf((അക്ഷരം *)strbuf, "<SNR>%d_", current_sctx.sc_sid);
		result = strbuf;
		നിർത്തുക;
#endif

#ifdef FEAT_CLIENTSERVER
	അവസ്ഥ SPEC_CLIENT:	// Source of last submitted input
# ifdef MSWIN
		sprintf((അക്ഷരം *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		result = strbuf;
# അല്ലെങ്കിൽ
#  ifdef FEAT_SOCKETSERVER
		എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		{
		    എങ്കിൽ (client_socket == ശൂന്യ)
			result = (char_u *)"";
		    അല്ലെങ്കിൽ
			result = client_socket;
		}
#  endif
#  ifdef FEAT_X11
		എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_X11)
		{
		    sprintf((അക്ഷരം *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		    result = strbuf;
		}
#  endif
# endif
		നിർത്തുക;
#endif
	}

	resultlen = STRLEN(result);	// length of new string
	എങ്കിൽ (src[*usedlen] == '<')	// remove the file name extension
	{
	    ++*usedlen;
	    എങ്കിൽ ((s = vim_strrchr(result, '.')) != ശൂന്യ && s >= gettail(result))
		resultlen = s - result;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!skip_mod)
	{
	    valid |= modify_fname(src, tilde_file, usedlen, &result, &resultbuf,
								  &resultlen);
	    എങ്കിൽ (result == ശൂന്യ)
	    {
		*errormsg = "";
		തിരികെ ശൂന്യ;
	    }
	}
    }

    എങ്കിൽ (resultlen == 0 || valid != VALID_HEAD + VALID_PATH)
    {
	എങ്കിൽ (empty_is_error)
	{
	    എങ്കിൽ (valid != VALID_HEAD + VALID_PATH)
		*errormsg = _(e_empty_file_name_for_percent_or_hash_only_works_with_ph);
	    അല്ലെങ്കിൽ
		*errormsg = _(e_evaluates_to_an_empty_string);
	}
	result = ശൂന്യ;
    }
    അല്ലെങ്കിൽ
	result = vim_strnsave(result, resultlen);
    vim_free(resultbuf);
    തിരികെ result;
}

/*
 * Expand the <sfile> string in "arg".
 *
 * Returns an allocated string, or NULL for any error.
 */
    char_u *
expand_sfile(char_u *arg)
{
    അക്ഷരം	*errormsg;
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
    എങ്കിൽ (result == ശൂന്യ)
	തിരികെ ശൂന്യ;

    വേണ്ടി (p = result; *p; )
    {
	എങ്കിൽ (STRNCMP(p, "<sfile>", 7) != 0)
	    ++p;
	അല്ലെങ്കിൽ
	{
	    // replace "<sfile>" with the sourced file name, and do ":" stuff
	    repl = eval_vars(p, result, &srclen, ശൂന്യ, &errormsg, ശൂന്യ, TRUE);
	    എങ്കിൽ (errormsg != ശൂന്യ)
	    {
		എങ്കിൽ (*errormsg)
		    emsg(errormsg);
		vim_free(result);
		തിരികെ ശൂന്യ;
	    }
	    എങ്കിൽ (repl == ശൂന്യ)		// no match (cannot happen)
	    {
		p += srclen;
		തുടരുക;
	    }
	    repllen = STRLEN(repl);
	    resultlen += repllen - srclen;
	    newres = alloc(resultlen + 1);
	    എങ്കിൽ (newres == ശൂന്യ)
	    {
		vim_free(repl);
		vim_free(result);
		തിരികെ ശൂന്യ;
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

    തിരികെ result;
}

#എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
/*
 * Make a dialog message in "buff[DIALOG_MSG_SIZE]".
 * "format" must contain "%s".
 */
    ശൂന്യം
dialog_സന്ദേശം(char_u *buff, അക്ഷരം *format, char_u *fname)
{
    എങ്കിൽ (fname == ശൂന്യ)
	fname = (char_u *)_("Untitled");
    vim_snprintf((അക്ഷരം *)buff, DIALOG_സന്ദേശം_SIZE, format, fname);
}
#endif

/*
 * ":behave {mswin,xterm}"
 */
    സ്ഥിതം ശൂന്യം
ex_behave(exarg_T *eap)
{
    എങ്കിൽ (STRCMP(eap->arg, "mswin") == 0)
    {
	set_option_മൂല്യം_give_err((char_u *)"selection",
						 0L, (char_u *)"exclusive", 0);
	set_option_മൂല്യം_give_err((char_u *)"selectmode",
						 0L, (char_u *)"mouse,key", 0);
	set_option_മൂല്യം_give_err((char_u *)"mousemodel",
						     0L, (char_u *)"popup", 0);
	set_option_മൂല്യം_give_err((char_u *)"keymodel",
					  0L, (char_u *)"startsel,stopsel", 0);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(eap->arg, "xterm") == 0)
    {
	set_option_മൂല്യം_give_err((char_u *)"selection",
						 0L, (char_u *)"inclusive", 0);
	set_option_മൂല്യം_give_err((char_u *)"selectmode", 0L, (char_u *)"", 0);
	set_option_മൂല്യം_give_err((char_u *)"mousemodel",
						    0L, (char_u *)"extend", 0);
	set_option_മൂല്യം_give_err((char_u *)"keymodel", 0L, (char_u *)"", 0);
    }
    അല്ലെങ്കിൽ
	semsg(_(e_invalid_argument_str), eap->arg);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ filetype_detect = FALSE;
സ്ഥിതം പൂർണ്ണസംഖ്യ filetype_plugin = FALSE;
സ്ഥിതം പൂർണ്ണസംഖ്യ filetype_indent = FALSE;

/*
 * ":filetype [plugin] [indent] {on,off,detect}"
 * on: Load the filetype.vim file to install autocommands for file types.
 * off: Load the ftoff.vim file to remove all autocommands for file types.
 * plugin on: load filetype.vim and ftplugin.vim
 * plugin off: load ftplugof.vim
 * indent on: load filetype.vim and indent.vim
 * indent off: load indoff.vim
 */
    സ്ഥിതം ശൂന്യം
ex_filetype(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    പൂർണ്ണസംഖ്യ		plugin = FALSE;
    പൂർണ്ണസംഖ്യ		indent = FALSE;

    എങ്കിൽ (*eap->arg == NUL)
    {
	// Print current status.
	smsg("filetype detection:%s  plugin:%s  indent:%s",
		filetype_detect ? "ON" : "OFF",
		filetype_plugin ? (filetype_detect ? "ON" : "(on)") : "OFF",
		filetype_indent ? (filetype_detect ? "ON" : "(on)") : "OFF");
	തിരികെ;
    }

    // Accept "plugin" and "indent" in any order.
    വേണ്ടി (;;)
    {
	എങ്കിൽ (STRNCMP(arg, "plugin", 6) == 0)
	{
	    plugin = TRUE;
	    arg = skipwhite(arg + 6);
	    തുടരുക;
	}
	എങ്കിൽ (STRNCMP(arg, "indent", 6) == 0)
	{
	    indent = TRUE;
	    arg = skipwhite(arg + 6);
	    തുടരുക;
	}
	നിർത്തുക;
    }
    എങ്കിൽ (STRCMP(arg, "on") == 0 || STRCMP(arg, "detect") == 0)
    {
	എങ്കിൽ (*arg == 'o' || !filetype_detect)
	{
	    source_runtime((char_u *)FILETYPE_FILE, DIP_ALL);
	    filetype_detect = TRUE;
	    എങ്കിൽ (plugin)
	    {
		source_runtime((char_u *)FTPLUGIN_FILE, DIP_ALL);
		filetype_plugin = TRUE;
	    }
	    എങ്കിൽ (indent)
	    {
		source_runtime((char_u *)INDENT_FILE, DIP_ALL);
		filetype_indent = TRUE;
	    }
	}
	എങ്കിൽ (*arg == 'd')
	{
	    (ശൂന്യം)do_doautocmd((char_u *)"filetypedetect BufRead", TRUE, ശൂന്യ);
	    do_modelines(0);
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(arg, "off") == 0)
    {
	എങ്കിൽ (plugin || indent)
	{
	    എങ്കിൽ (plugin)
	    {
		source_runtime((char_u *)FTPLUGOF_FILE, DIP_ALL);
		filetype_plugin = FALSE;
	    }
	    എങ്കിൽ (indent)
	    {
		source_runtime((char_u *)INDOFF_FILE, DIP_ALL);
		filetype_indent = FALSE;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    source_runtime((char_u *)FTOFF_FILE, DIP_ALL);
	    filetype_detect = FALSE;
	}
    }
    അല്ലെങ്കിൽ
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * ":setfiletype [FALLBACK] {name}"
 */
    സ്ഥിതം ശൂന്യം
ex_setfiletype(exarg_T *eap)
{
    എങ്കിൽ (curbuf->b_did_filetype)
	തിരികെ;

    char_u *arg = eap->arg;
    എങ്കിൽ (STRNCMP(arg, "FALLBACK ", 9) == 0)
	arg += 9;

    set_option_മൂല്യം_give_err((char_u *)"filetype", 0L, arg, OPT_LOCAL);
    എങ്കിൽ (arg != eap->arg)
	curbuf->b_did_filetype = false;
}

    സ്ഥിതം ശൂന്യം
ex_digraphs(exarg_T *eap UNUSED)
{
#ifdef FEAT_DIGRAPHS
    എങ്കിൽ (*eap->arg != NUL)
	putdigraph(eap->arg);
    അല്ലെങ്കിൽ
	listdigraphs(eap->forceit);
#അല്ലെങ്കിൽ
    emsg(_(e_no_digraphs_version));
#endif
}

#എങ്കിൽ defined(FEAT_തിരയൽ_EXTRA)
    ശൂന്യം
set_no_hlsearch(പൂർണ്ണസംഖ്യ flag)
{
    no_hlsearch = flag;
# ifdef FEAT_EVAL
    set_vim_var_nr(VV_HLSEARCH, !no_hlsearch && p_hls);
# endif
}

/*
 * ":nohlsearch"
 */
    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം ശൂന്യം
ex_X(exarg_T *eap UNUSED)
{
    crypt_check_current_method();
    (ശൂന്യം)crypt_get_താക്കോൽ(TRUE, TRUE);
}
#endif

#ifdef FEAT_FOLDING
    സ്ഥിതം ശൂന്യം
ex_fold(exarg_T *eap)
{
    എങ്കിൽ (foldManualAllowed(TRUE))
	foldCreate(eap->line1, eap->line2);
}

    സ്ഥിതം ശൂന്യം
ex_foldopen(exarg_T *eap)
{
    opFoldRange(eap->line1, eap->line2, eap->cmdidx == CMD_foldopen,
							 eap->forceit, FALSE);
}

    സ്ഥിതം ശൂന്യം
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
    വേണ്ടി (lnum = eap->line1; lnum <= eap->line2; ++lnum)
	എങ്കിൽ (hasFolding(lnum, ശൂന്യ, ശൂന്യ) == (eap->cmdidx == CMD_folddoclosed))
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

#എങ്കിൽ defined(FEAT_QUICKFIX)
/*
 * Returns TRUE if the supplied Ex cmdidx is for a location list command
 * instead of a quickfix command.
 */
    പൂർണ്ണസംഖ്യ
is_loclist_cmd(പൂർണ്ണസംഖ്യ cmdidx)
{
    എങ്കിൽ (cmdidx < 0 || cmdidx >= CMD_SIZE)
	തിരികെ FALSE;
    തിരികെ cmdnames[cmdidx].cmd_name[0] == 'l';
}
#endif

    പൂർണ്ണസംഖ്യ
get_pressedreturn(ശൂന്യം)
{
    തിരികെ ex_pressedreturn;
}

    ശൂന്യം
set_pressedreturn(പൂർണ്ണസംഖ്യ val)
{
    ex_pressedreturn = val;
}
