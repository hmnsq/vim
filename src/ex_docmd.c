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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	quitmore = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	ex_pressedreturn = FALSE;
#ifndef FEAT_PRINTER
# define ex_hardcopy	ex_ni
#endif

#ifdef FEAT_EVAL
ಸ್ಥಿರ char_u	*do_one_cmd(char_u **, ಪೂರ್ಣಾಙ್ಕ, cstack_T *, char_u *(*fgetline)(ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ, getline_opt_T), ಶೂನ್ಯ *cookie);
#ಅನ್ಯಥಾ
ಸ್ಥಿರ char_u	*do_one_cmd(char_u **, ಪೂರ್ಣಾಙ್ಕ, char_u *(*fgetline)(ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ, getline_opt_T), ಶೂನ್ಯ *cookie);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	if_level = 0;		// depth in :if
#endif
ಸ್ಥಿರ ಶೂನ್ಯ	append_command(char_u *cmd);

#ifndef FEAT_MENU
# define ex_emenu		ex_ni
# define ex_menu		ex_ni
# define ex_menutranslate	ex_ni
#endif
ಸ್ಥಿರ ಶೂನ್ಯ	ex_autocmd(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_doautocmd(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_bunload(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_buffer(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	do_exbuffer(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_bmodified(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_bnext(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_bprevious(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_brewind(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_blast(exarg_T *eap);
ಸ್ಥಿರ char_u	*getargcmd(char_u **);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	getargopt(exarg_T *eap);
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
#ಯದಿ !defined(FEAT_QUICKFIX) || !defined(FEAT_EVAL)
# define ex_cexpr		ex_ni
#endif

ಸ್ಥಿರ linenr_T default_address(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ address_default_all(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	get_flags(exarg_T *eap);
#ಯದಿ !defined(FEAT_PERL) \
	|| !defined(FEAT_PYTHON) || !defined(FEAT_PYTHON3) \
	|| !defined(FEAT_TCL) \
	|| !defined(FEAT_RUBY) \
	|| !defined(FEAT_LUA) \
	|| !defined(FEAT_MZSCHEME)
# define HAVE_EX_SCRIPT_NI
ಸ್ಥಿರ ಶೂನ್ಯ	ex_script_ni(exarg_T *eap);
#endif
ಸ್ಥಿರ ವರ್ಣ	*invalid_range(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	correct_range(exarg_T *eap);
#ifdef FEAT_QUICKFIX
ಸ್ಥಿರ char_u	*replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep);
#endif
ಸ್ಥಿರ char_u	*repl_cmdline(exarg_T *eap, char_u *src, size_t srclen, char_u *repl, char_u **cmdlinep);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_highlight(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_colorscheme(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_cquit(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_quit_all(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_close(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_win_close(ಪೂರ್ಣಾಙ್ಕ forceit, win_T *win, tabpage_T *tp);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_only(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_resize(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_stag(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_tabclose(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_tabonly(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_tabnext(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_tabmove(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_tabs(exarg_T *eap);
#ಯದಿ defined(FEAT_QUICKFIX)
ಸ್ಥಿರ ಶೂನ್ಯ	ex_pclose(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_ptag(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_pedit(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_pbuffer(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	prepare_preview_window(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ	back_to_current_window(win_T *curwin_save);
#ಅನ್ಯಥಾ
# define ex_pclose		ex_ni
# define ex_ptag		ex_ni
# define ex_pedit		ex_ni
# define ex_pbuffer		ex_ni
#endif
ಸ್ಥಿರ ಶೂನ್ಯ	ex_hide(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_exit(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_print(exarg_T *eap);
#ifdef FEAT_BYTEOFF
ಸ್ಥಿರ ಶೂನ್ಯ	ex_goto(exarg_T *eap);
#ಅನ್ಯಥಾ
# define ex_goto		ex_ni
#endif
ಸ್ಥಿರ ಶೂನ್ಯ	ex_shell(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_preserve(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_recover(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_mode(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_wrongmodifier(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_find(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_open(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_edit(exarg_T *eap);
#ifndef FEAT_GUI
# define ex_gui			ex_nogui
ಸ್ಥಿರ ಶೂನ್ಯ	ex_nogui(exarg_T *eap);
#endif
#ಯದಿ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
ಸ್ಥಿರ ಶೂನ್ಯ	ex_tearoff(exarg_T *eap);
#ಅನ್ಯಥಾ
# define ex_tearoff		ex_ni
#endif
#ಯದಿ (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
ಸ್ಥಿರ ಶೂನ್ಯ	ex_popup(exarg_T *eap);
#ಅನ್ಯಥಾ
# define ex_popup		ex_ni
#endif
#ifndef FEAT_GUI_MSWIN
# define ex_simalt		ex_ni
#endif
#ಯದಿ !defined(FEAT_GUI_MSWIN) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
# define gui_mch_find_dialog	ex_ni
# define gui_mch_replace_dialog ex_ni
#endif
#ಯದಿ !defined(FEAT_GUI_GTK)
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
#ಯದಿ !defined(FEAT_SYN_HL) || !defined(FEAT_PROFILE)
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
#ಯದಿ !defined(FEAT_PYTHON) && !defined(FEAT_PYTHON3)
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
ಸ್ಥಿರ ಶೂನ್ಯ	ex_swapname(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_syncbind(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_read(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_pwd(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_equal(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_sleep(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_winsize(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_wincmd(exarg_T *eap);
#ಯದಿ defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
ಸ್ಥಿರ ಶೂನ್ಯ	ex_winpos(exarg_T *eap);
#ಅನ್ಯಥಾ
# define ex_winpos	    ex_ni
#endif
ಸ್ಥಿರ ಶೂನ್ಯ	ex_operators(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_put(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_iput(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_copymove(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_submagic(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_join(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_at(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_bang(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_undo(exarg_T *eap);
#ifdef FEAT_PERSISTENT_UNDO
ಸ್ಥಿರ ಶೂನ್ಯ	ex_wundo(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_rundo(exarg_T *eap);
#endif
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
#ifdef FEAT_FIND_ID
ಸ್ಥಿರ ಶೂನ್ಯ	ex_checkpath(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_findpat(exarg_T *eap);
#ಅನ್ಯಥಾ
# define ex_findpat		ex_ni
# define ex_checkpath		ex_ni
#endif
#ಯದಿ defined(FEAT_FIND_ID) && defined(FEAT_QUICKFIX)
ಸ್ಥಿರ ಶೂನ್ಯ	ex_psearch(exarg_T *eap);
#ಅನ್ಯಥಾ
# define ex_psearch		ex_ni
#endif
ಸ್ಥಿರ ಶೂನ್ಯ	ex_tag(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_tag_cmd(exarg_T *eap, char_u *name);
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
ಸ್ಥಿರ ಶೂನ್ಯ	ex_behave(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_filetype(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_setfiletype(exarg_T *eap);
#ifndef FEAT_DIFF
# define ex_diffoff		ex_ni
# define ex_diffpatch		ex_ni
# define ex_diffgetput		ex_ni
# define ex_diffsplit		ex_ni
# define ex_diffthis		ex_ni
# define ex_diffupdate		ex_ni
#endif
ಸ್ಥಿರ ಶೂನ್ಯ	ex_digraphs(exarg_T *eap);
#ifdef FEAT_ಹುಡುಕು_EXTRA
ಸ್ಥಿರ ಶೂನ್ಯ	ex_nohlsearch(exarg_T *eap);
#ಅನ್ಯಥಾ
# define ex_nohlsearch		ex_ni
# define ex_match		ex_ni
#endif
#ifdef FEAT_CRYPT
ಸ್ಥಿರ ಶೂನ್ಯ	ex_X(exarg_T *eap);
#ಅನ್ಯಥಾ
# define ex_X			ex_ni
#endif
#ifdef FEAT_FOLDING
ಸ್ಥಿರ ಶೂನ್ಯ	ex_fold(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_foldopen(exarg_T *eap);
ಸ್ಥಿರ ಶೂನ್ಯ	ex_folddo(exarg_T *eap);
#ಅನ್ಯಥಾ
# define ex_fold		ex_ni
# define ex_foldopen		ex_ni
# define ex_folddo		ex_ni
#endif
#ಯದಿ !(defined(HAVE_LOCALE_H) || defined(X_LOCALE))
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
#ಯದಿ !defined(FEAT_X11) || !defined(FEAT_XCLIPBOARD)
# define ex_xrestore		ex_ni
#endif
#ಯದಿ !defined(FEAT_WAYLAND)
# define ex_wlrestore		ex_ni
#endif
#ಯದಿ !defined(HAVE_CLIPMETHOD)
# define ex_clipreset		ex_ni
#endif
#ಯದಿ !defined(FEAT_PROP_POPUP)
# define ex_popupclear		ex_ni
#endif

/*
 * Declare cmdnames[].
 */
#define DO_DECLARE_EXCMD
#ಅನ್ತರ್ಭಾವ "ex_cmds.h"
#ಅನ್ತರ್ಭಾವ "ex_cmdidxs.h"

ಸ್ಥಿರ char_u dollar_command[2] = {'$', 0};


#ifdef FEAT_EVAL
// Struct for storing a line inside a while/for loop
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    char_u	*ಸಾಲು;		// command line
    linenr_T	lnum;		// sourcing_lnum of the line
} wcmd_T;

/*
 * Structure used to store info for line position in a while or for loop.
 * This is required, because do_one_cmd() may invoke ex_function(), which
 * reads more lines that may come from the while/for loop.
 */
ಸಂರಚನಾ loop_cookie
{
    garray_T	*ಸಾಲುಗಳು_gap;		// growarray with line info
    ಪೂರ್ಣಾಙ್ಕ		current_ಸಾಲು;		// last read line from growarray
    ಪೂರ್ಣಾಙ್ಕ		repeating;		// TRUE when looping a second time
    // When "repeating" is FALSE use "getline" and "cookie" to get lines
    char_u	*(*lc_getline)(ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ, getline_opt_T);
    ಶೂನ್ಯ	*cookie;
};

ಸ್ಥಿರ char_u	*get_loop_ಸಾಲು(ಪೂರ್ಣಾಙ್ಕ c, ಶೂನ್ಯ *cookie, ಪೂರ್ಣಾಙ್ಕ indent, getline_opt_T options);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	store_loop_ಸಾಲು(garray_T *gap, char_u *ಸಾಲು);
ಸ್ಥಿರ ಶೂನ್ಯ	free_cmdlines(garray_T *gap);

// Struct to save a few things while debugging.  Used in do_cmdline() only.
ಸಂರಚನಾ dbg_stuff
{
    ಪೂರ್ಣಾಙ್ಕ		trylevel;
    ಪೂರ್ಣಾಙ್ಕ		force_abort;
    except_T	*caught_ರಾಶಿ;
    char_u	*vv_exception;
    char_u	*vv_throwpoint;
    ಪೂರ್ಣಾಙ್ಕ		did_emsg;
    ಪೂರ್ಣಾಙ್ಕ		got_int;
    ಪೂರ್ಣಾಙ್ಕ		did_throw;
    ಪೂರ್ಣಾಙ್ಕ		need_rethrow;
    ಪೂರ್ಣಾಙ್ಕ		check_cstack;
    except_T	*current_exception;
};

    ಸ್ಥಿರ ಶೂನ್ಯ
save_dbg_stuff(ಸಂರಚನಾ dbg_stuff *dsp)
{
    dsp->trylevel	= trylevel;		trylevel = 0;
    dsp->force_abort	= force_abort;		force_abort = FALSE;
    dsp->caught_ರಾಶಿ	= caught_ರಾಶಿ;		caught_ರಾಶಿ = ರಿಕ್ತ;
    dsp->vv_exception	= v_exception(ರಿಕ್ತ);
    dsp->vv_throwpoint	= v_throwpoint(ರಿಕ್ತ);

    // Necessary for debugging an inactive ":catch", ":finally", ":endtry"
    dsp->did_emsg	= did_emsg;		did_emsg     = FALSE;
    dsp->got_int	= got_int;		got_int	     = FALSE;
    dsp->did_throw	= did_throw;		did_throw    = FALSE;
    dsp->need_rethrow	= need_rethrow;		need_rethrow = FALSE;
    dsp->check_cstack	= check_cstack;		check_cstack = FALSE;
    dsp->current_exception = current_exception;	current_exception = ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
restore_dbg_stuff(ಸಂರಚನಾ dbg_stuff *dsp)
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
#endif

/*
 * Check if ffname differs from fnum.
 * fnum is a buffer number. 0 == current buffer, 1-or-more must be a valid
 * buffer ID.
 * ffname is a full path to where a buffer lives on-disk or would live on-disk.
 *
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_other_file(ಪೂರ್ಣಾಙ್ಕ fnum, char_u *ffname)
{
    ಯದಿ (fnum != 0)
    {
	ಯದಿ (fnum == curbuf->b_fnum)
	    ಪ್ರತ್ಯಾಗಮ FALSE;

	ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    ಯದಿ (ffname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ಯದಿ (*ffname == NUL)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // TODO: Need a reliable way to know whether a buffer is meant to live
    // on-disk !curbuf->b_dev_valid is not always available (example: missing
    // on Windows)
    ಯದಿ (curbuf->b_sfname != ರಿಕ್ತ
	    && *curbuf->b_sfname != NUL)
	// This occurs with unsaved buffers. In which case `ffname` actually
	// corresponds to curbuf->b_sfname
	ಪ್ರತ್ಯಾಗಮ fnamecmp(ffname, curbuf->b_sfname) != 0;

    ಪ್ರತ್ಯಾಗಮ otherfile(ffname);
}

/*
 * do_exmode(): Repeatedly get commands for the "Ex" mode, until the ":vi"
 * command is given.
 */
    ಶೂನ್ಯ
do_exmode(
    ಪೂರ್ಣಾಙ್ಕ		improved)	    // TRUE for "improved Ex" mode
{
    ಪೂರ್ಣಾಙ್ಕ		save_ಸಂದೇಶ_scroll;
    ಪೂರ್ಣಾಙ್ಕ		prev_ಸಂದೇಶ_row;
    linenr_T	prev_ಸಾಲು;
    varnumber_T	changedtick;

    ಯದಿ (improved)
	exmode_active = EXMODE_VIM;
    ಅನ್ಯಥಾ
	exmode_active = EXMODE_NORMAL;
    State = MODE_NORMAL;
    may_trigger_modechanged();

    // When using ":global /pat/ visual" and then "Q" we return to continue
    // the :global command.
    ಯದಿ (global_busy)
	ಪ್ರತ್ಯಾಗಮ;

    save_ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll;
    ++RedrawingDisabled;	    // don't redisplay the window
    ++no_wait_return;		    // don't wait for return
#ifdef FEAT_GUI
    // Ignore scrollbar and mouse events in Ex mode
    ++hold_gui_events;
#endif

    ಸಂದೇಶ(_("Entering Ex mode.  Type \"visual\" to go to Normal mode."));
    ಯಾವತ್ (exmode_active)
    {
	// Check for a ":normal" command and no more characters left.
	ಯದಿ (ex_normal_busy > 0 && typebuf.tb_len == 0)
	{
	    exmode_active = FALSE;
	    ಭಙ್ಗ;
	}
	ಸಂದೇಶ_scroll = TRUE;
	need_wait_return = FALSE;
	ex_pressedreturn = FALSE;
	ex_no_reprint = FALSE;
	changedtick = CHANGEDTICK(curbuf);
	prev_ಸಂದೇಶ_row = ಸಂದೇಶ_row;
	prev_ಸಾಲು = curwin->w_cursor.lnum;
	ಯದಿ (improved)
	{
	    cmdline_row = ಸಂದೇಶ_row;
	    do_cmdline(ರಿಕ್ತ, getexline, ರಿಕ್ತ, 0);
	}
	ಅನ್ಯಥಾ
	    do_cmdline(ರಿಕ್ತ, getexmodeline, ರಿಕ್ತ, DOCMD_NOWAIT);
	ಸಾಲುಗಳು_ಎಡ = Rows - 1;

	ಯದಿ ((prev_ಸಾಲು != curwin->w_cursor.lnum
		   || changedtick != CHANGEDTICK(curbuf)) && !ex_no_reprint)
	{
	    ಯದಿ (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (ex_pressedreturn)
		{
		    // go up one line, to overwrite the ":<CR>" line, so the
		    // output doesn't contain empty lines.
		    ಸಂದೇಶ_row = prev_ಸಂದೇಶ_row;
		    ಯದಿ (prev_ಸಂದೇಶ_row == Rows - 1)
			ಸಂದೇಶ_row--;
		}
		ಸಂದೇಶ_col = 0;
		print_ಸಾಲು_no_prefix(curwin->w_cursor.lnum, FALSE, FALSE);
		ಸಂದೇಶ_clr_eos();
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (ex_pressedreturn && !ex_no_reprint)	// must be at EOF
	{
	    ಯದಿ (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    ಅನ್ಯಥಾ
		emsg(_(e_at_end_of_file));
	}
    }

#ifdef FEAT_GUI
    --hold_gui_events;
#endif
    ಯದಿ (RedrawingDisabled > 0)
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

    ಯದಿ (lnum == 0)
	smsg(_("Executing: %s"), cmd);
    ಅನ್ಯಥಾ
	smsg(_("line %ld: %s"), (long)lnum, cmd);
    ಯದಿ (ಸಂದೇಶ_silent == 0)
	ಸಂದೇಶ_puts("\n");   // don't overwrite this

    verbose_leave_scroll();
    --no_wait_return;
}

/*
 * Execute a simple command line.  Used for translated commands like "*".
 */
    ಪೂರ್ಣಾಙ್ಕ
do_cmdline_cmd(char_u *cmd)
{
    ಪ್ರತ್ಯಾಗಮ do_cmdline(cmd, ರಿಕ್ತ, ರಿಕ್ತ,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
}

/*
 * Execute the "+cmd" argument of "edit +cmd fname" and the like.
 * This allows for using a range without ":" in Vim9 script.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
do_cmd_argument(char_u *cmd)
{
    ಪ್ರತ್ಯಾಗಮ do_cmdline(cmd, ರಿಕ್ತ, ರಿಕ್ತ,
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
    ಪೂರ್ಣಾಙ್ಕ
do_cmdline(
    char_u	*cmdline,
    char_u	*(*fgetline)(ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ, getline_opt_T),
    ಶೂನ್ಯ	*cookie,		// argument for fgetline()
    ಪೂರ್ಣಾಙ್ಕ		flags)
{
    char_u	*next_cmdline;		// next cmd to execute
    char_u	*cmdline_copy = ರಿಕ್ತ;	// copy of cmd line
    ಪೂರ್ಣಾಙ್ಕ		used_getline = FALSE;	// used "fgetline" to obtain command
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recursive = 0;		// recursive depth
    ಪೂರ್ಣಾಙ್ಕ		ಸಂದೇಶ_didout_before_start = 0;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;		// line number count
    ಪೂರ್ಣಾಙ್ಕ		did_inc_RedrawingDisabled = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;
#ifdef FEAT_EVAL
    cstack_T	cstack;			// conditional stack
    garray_T	ಸಾಲುಗಳು_ga;		// keep lines for ":while"/":for"
    ಪೂರ್ಣಾಙ್ಕ		current_ಸಾಲು = 0;	// active line in lines_ga
    ಪೂರ್ಣಾಙ್ಕ		current_ಸಾಲು_before = 0;
    char_u	*fname = ರಿಕ್ತ;		// function or script name
    linenr_T	*breakpoint = ರಿಕ್ತ;	// ptr to breakpoint field in cookie
    ಪೂರ್ಣಾಙ್ಕ		*dbg_tick = ರಿಕ್ತ;	// ptr to dbg_tick field in cookie
    ಸಂರಚನಾ dbg_stuff debug_saved;	// saved things for debug mode
    ಪೂರ್ಣಾಙ್ಕ		initial_trylevel;
    msglist_T	**saved_ಸಂದೇಶ_list = ರಿಕ್ತ;
    msglist_T	*private_ಸಂದೇಶ_list = ರಿಕ್ತ;

    // "fgetline" and "cookie" passed to do_one_cmd()
    char_u	*(*cmd_getline)(ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ, getline_opt_T);
    ಶೂನ್ಯ	*cmd_cookie;
    ಸಂರಚನಾ loop_cookie cmd_loop_cookie;
    ಶೂನ್ಯ	*real_cookie;
    ಪೂರ್ಣಾಙ್ಕ		getline_is_func;
#ಅನ್ಯಥಾ
# define cmd_getline fgetline
# define cmd_cookie cookie
#endif
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	call_depth = 0;		// recursiveness
#ifdef FEAT_EVAL
    // For every pair of do_cmdline()/do_one_cmd() calls, use an extra memory
    // location for storing error messages to be converted to an exception.
    // This ensures that the do_errthrow() call in do_one_cmd() does not
    // combine the messages stored by an earlier invocation of do_one_cmd()
    // with the command name of the later one.  This would happen when
    // BufWritePost autocommands are executed after a write error.
    saved_ಸಂದೇಶ_list = ಸಂದೇಶ_list;
    ಸಂದೇಶ_list = &private_ಸಂದೇಶ_list;
#endif

    // It's possible to create an endless loop with ":execute", catch that
    // here.  The value of 200 allows nested function calls, ":source", etc.
    // Allow 200 or 'maxfuncdepth', whatever is larger.
    ಯದಿ (call_depth >= 200
#ifdef FEAT_EVAL
	    && call_depth >= p_mfd
#endif
	    )
    {
	emsg(_(e_command_too_recursive));
#ifdef FEAT_EVAL
	// When converting to an exception, we do not include the command name
	// since this is not an error of the specific command.
	do_errthrow((cstack_T *)ರಿಕ್ತ, (char_u *)ರಿಕ್ತ);
	ಸಂದೇಶ_list = saved_ಸಂದೇಶ_list;
#endif
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ++call_depth;

#ifdef FEAT_EVAL
    CLEAR_FIELD(cstack);
    cstack.cs_idx = -1;
    ga_init2(&ಸಾಲುಗಳು_ga, ಪರಿಮಾಣ(wcmd_T), 10);

    real_cookie = getline_cookie(fgetline, cookie);

    // Inside a function use a higher nesting level.
    getline_is_func = getline_equal(fgetline, cookie, get_func_ಸಾಲು);
    ಯದಿ (getline_is_func && ex_nesting_level == func_level(real_cookie))
	++ex_nesting_level;

    // Get the function or script name and the address where the next breakpoint
    // line and the debug tick for a function or script are stored.
    ಯದಿ (getline_is_func)
    {
	fname = func_name(real_cookie);
	breakpoint = func_breakpoint(real_cookie);
	dbg_tick = func_dbg_tick(real_cookie);
    }
    ಅನ್ಯಥಾ ಯದಿ (getline_equal(fgetline, cookie, getsourceline))
    {
	fname = SOURCING_NAME;
	breakpoint = source_breakpoint(real_cookie);
	dbg_tick = source_dbg_tick(real_cookie);
    }

    /*
     * Initialize "force_abort"  and "suppress_errthrow" at the top level.
     */
    ಯದಿ (!recursive)
    {
	force_abort = FALSE;
	suppress_errthrow = FALSE;
    }

    /*
     * If requested, store and reset the global values controlling the
     * exception handling (used when debugging).  Otherwise clear it to avoid
     * a bogus compiler warning when the optimizer uses inline functions...
     */
    ಯದಿ (flags & DOCMD_EXCRESET)
	save_dbg_stuff(&debug_saved);
    ಅನ್ಯಥಾ
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
    ಯದಿ (!(flags & DOCMD_KEYTYPED)
			       && !getline_equal(fgetline, cookie, getexline))
	ಕೀಲಿ_Typed = FALSE;

    /*
     * Continue executing command lines:
     * - when inside an ":if", ":while" or ":for"
     * - for multiple commands on one line, separated with '|'
     * - when repeating until there are no more lines (for ":source")
     */
    next_cmdline = cmdline;
    ಕುರು
    {
#ifdef FEAT_EVAL
	getline_is_func = getline_equal(fgetline, cookie, get_func_ಸಾಲು);
#endif

	// stop skipping cmds for an error msg after all endif/while/for
	ಯದಿ (next_cmdline == ರಿಕ್ತ
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
	ಯದಿ (cstack.cs_looplevel > 0 && current_ಸಾಲು < ಸಾಲುಗಳು_ga.ga_len)
	{
	    // Each '|' separated command is stored separately in lines_ga, to
	    // be able to jump to it.  Don't use next_cmdline now.
	    VIM_CLEAR(cmdline_copy);

	    // Check if a function has returned or, unless it has an unclosed
	    // try conditional, aborted.
	    ಯದಿ (getline_is_func)
	    {
# ifdef FEAT_PROFILE
		ಯದಿ (do_profiling == PROF_YES)
		    func_ಸಾಲು_end(real_cookie);
# endif
		ಯದಿ (func_has_ended(real_cookie))
		{
		    retval = FAIL;
		    ಭಙ್ಗ;
		}
	    }
# ifdef FEAT_PROFILE
	    ಅನ್ಯಥಾ ಯದಿ (do_profiling == PROF_YES
			    && getline_equal(fgetline, cookie, getsourceline))
		script_ಸಾಲು_end();
# endif

	    // Check if a sourced file hit a ":finish" command.
	    ಯದಿ (source_finished(fgetline, cookie))
	    {
		retval = FAIL;
		ಭಙ್ಗ;
	    }

	    // If breakpoints have been added/deleted need to check for it.
	    ಯದಿ (breakpoint != ರಿಕ್ತ && dbg_tick != ರಿಕ್ತ
						   && *dbg_tick != debug_tick)
	    {
		*breakpoint = dbg_find_breakpoint(
				getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }

	    next_cmdline = ((wcmd_T *)(ಸಾಲುಗಳು_ga.ga_data))[current_ಸಾಲು].ಸಾಲು;
	    SOURCING_LNUM = ((wcmd_T *)(ಸಾಲುಗಳು_ga.ga_data))[current_ಸಾಲು].lnum;

	    // Did we encounter a breakpoint?
	    ಯದಿ (breakpoint != ರಿಕ್ತ && *breakpoint != 0
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
	    ಯದಿ (do_profiling == PROF_YES)
	    {
		ಯದಿ (getline_is_func)
		    func_ಸಾಲು_start(real_cookie, SOURCING_LNUM);
		ಅನ್ಯಥಾ ಯದಿ (getline_equal(fgetline, cookie, getsourceline))
		    script_ಸಾಲು_start();
	    }
# endif
	}
#endif

	// 2. If no line given, get an allocated line with fgetline().
	ಯದಿ (next_cmdline == ರಿಕ್ತ)
	{
	    /*
	     * Need to set msg_didout for the first line after an ":if",
	     * otherwise the ":if" will be overwritten.
	     */
	    ಯದಿ (ಎಣಿಕೆ == 1 && getline_equal(fgetline, cookie, getexline))
		ಸಂದೇಶ_didout = TRUE;
	    ಯದಿ (fgetline == ರಿಕ್ತ || (next_cmdline = fgetline(':', cookie,
#ifdef FEAT_EVAL
		    cstack.cs_idx < 0 ? 0 : (cstack.cs_idx + 1) * 2
#ಅನ್ಯಥಾ
		    0
#endif
		    , in_vim9script() ? GETLINE_CONCAT_CONTBAR
					       : GETLINE_CONCAT_CONT)) == ರಿಕ್ತ)
	    {
		// Don't call wait_return() for aborted command line.  The NULL
		// returned for the end of a sourced file or executed function
		// doesn't do this.
		ಯದಿ (ಕೀಲಿ_Typed && !(flags & DOCMD_REPEAT))
		    need_wait_return = FALSE;
		retval = FAIL;
		ಭಙ್ಗ;
	    }
	    used_getline = TRUE;

	    /*
	     * Keep the first typed line.  Clear it when more lines are typed.
	     */
	    ಯದಿ (flags & DOCMD_KEEPLINE)
	    {
		vim_free(repeat_cmdline);
		ಯದಿ (ಎಣಿಕೆ == 0)
		    repeat_cmdline = vim_strsave(next_cmdline);
		ಅನ್ಯಥಾ
		    repeat_cmdline = ರಿಕ್ತ;
	    }
	}

	// 3. Make a copy of the command so we can mess with it.
	ಅನ್ಯಥಾ ಯದಿ (cmdline_copy == ರಿಕ್ತ)
	{
	    next_cmdline = vim_strsave(next_cmdline);
	    ಯದಿ (next_cmdline == ರಿಕ್ತ)
	    {
		emsg(_(e_out_of_memory));
		retval = FAIL;
		ಭಙ್ಗ;
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
	ಯದಿ ((cstack.cs_looplevel > 0 || has_loop_cmd(next_cmdline)))
	{
	    cmd_getline = get_loop_ಸಾಲು;
	    cmd_cookie = (ಶೂನ್ಯ *)&cmd_loop_cookie;
	    cmd_loop_cookie.ಸಾಲುಗಳು_gap = &ಸಾಲುಗಳು_ga;
	    cmd_loop_cookie.current_ಸಾಲು = current_ಸಾಲು;
	    cmd_loop_cookie.lc_getline = fgetline;
	    cmd_loop_cookie.cookie = cookie;
	    cmd_loop_cookie.repeating = (current_ಸಾಲು < ಸಾಲುಗಳು_ga.ga_len);

	    // Save the current line when encountering it the first time.
	    ಯದಿ (current_ಸಾಲು == ಸಾಲುಗಳು_ga.ga_len
		    && store_loop_ಸಾಲು(&ಸಾಲುಗಳು_ga, next_cmdline) == FAIL)
	    {
		retval = FAIL;
		ಭಙ್ಗ;
	    }
	    current_ಸಾಲು_before = current_ಸಾಲು;
	}
	ಅನ್ಯಥಾ
	{
	    cmd_getline = fgetline;
	    cmd_cookie = cookie;
	}

	did_endif = FALSE;
#endif

	ಯದಿ (ಎಣಿಕೆ++ == 0)
	{
	    /*
	     * All output from the commands is put below each other, without
	     * waiting for a return. Don't do this when executing commands
	     * from a script or when being called recursive (e.g. for ":e
	     * +command file").
	     */
	    ಯದಿ (!(flags & DOCMD_NOWAIT) && !recursive)
	    {
		ಸಂದೇಶ_didout_before_start = ಸಂದೇಶ_didout;
		ಸಂದೇಶ_didany = FALSE; // no output yet
		ಸಂದೇಶ_start();
		ಸಂದೇಶ_scroll = TRUE;  // put messages below each other
		++no_wait_return;   // don't wait for return until finished
		++RedrawingDisabled;
		did_inc_RedrawingDisabled = TRUE;
	    }
	}

	ಯದಿ ((p_verbose >= 15 && SOURCING_NAME != ರಿಕ್ತ) || p_verbose >= 16)
	    ಸಂದೇಶ_verbose_cmd(SOURCING_LNUM, cmdline_copy);

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
	ಯದಿ (cmd_cookie == (ಶೂನ್ಯ *)&cmd_loop_cookie)
	    // Use "current_line" from "cmd_loop_cookie", it may have been
	    // incremented when defining a function.
	    current_ಸಾಲು = cmd_loop_cookie.current_ಸಾಲು;
#endif

	ಯದಿ (next_cmdline == ರಿಕ್ತ)
	{
	    VIM_CLEAR(cmdline_copy);

	    /*
	     * If the command was typed, remember it for the ':' register.
	     * Do this AFTER executing the command to make :@: work.
	     */
	    ಯದಿ (getline_equal(fgetline, cookie, getexline)
						  && new_last_cmdline != ರಿಕ್ತ)
	    {
		vim_free(last_cmdline);
		last_cmdline = new_last_cmdline;
		new_last_cmdline = ರಿಕ್ತ;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    // need to copy the command after the '|' to cmdline_copy, for the
	    // next do_one_cmd()
	    STRMOVE(cmdline_copy, next_cmdline);
	    next_cmdline = cmdline_copy;
	}


#ifdef FEAT_EVAL
	// reset did_emsg for a function that is not aborted by an error
	ಯದಿ (did_emsg && !force_abort
		&& getline_equal(fgetline, cookie, get_func_ಸಾಲು)
					      && !func_has_abort(real_cookie))
	{
	    // did_emsg_cumul is not set here
	    did_emsg = FALSE;
	}

	ಯದಿ (cstack.cs_looplevel > 0)
	{
	    ++current_ಸಾಲು;

	    /*
	     * An ":endwhile", ":endfor" and ":continue" is handled here.
	     * If we were executing commands, jump back to the ":while" or
	     * ":for".
	     * If we were not executing commands, decrement cs_looplevel.
	     */
	    ಯದಿ (cstack.cs_lflags & (CSL_HAD_CONT | CSL_HAD_ENDLOOP))
	    {
		cstack.cs_lflags &= ~(CSL_HAD_CONT | CSL_HAD_ENDLOOP);

		// Jump back to the matching ":while" or ":for".  Be careful
		// not to use a cs_line[] from an entry that isn't a ":while"
		// or ":for": It would make "current_line" invalid and can
		// cause a crash.
		ಯದಿ (!did_emsg && !got_int && !did_throw
			&& cstack.cs_idx >= 0
			&& (cstack.cs_flags[cstack.cs_idx]
						      & (CSF_WHILE | CSF_FOR))
			&& cstack.cs_ಸಾಲು[cstack.cs_idx] >= 0
			&& (cstack.cs_flags[cstack.cs_idx] & CSF_ACTIVE))
		{
		    current_ಸಾಲು = cstack.cs_ಸಾಲು[cstack.cs_idx];
						// remember we jumped there
		    cstack.cs_lflags |= CSL_HAD_LOOP;
		    ಸಾಲು_breakcheck();		// check if CTRL-C typed

		    // Check for the next breakpoint at or after the ":while"
		    // or ":for".
		    ಯದಿ (breakpoint != ರಿಕ್ತ && ಸಾಲುಗಳು_ga.ga_len > current_ಸಾಲು)
		    {
			*breakpoint = dbg_find_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
									fname,
			   ((wcmd_T *)ಸಾಲುಗಳು_ga.ga_data)[current_ಸಾಲು].lnum-1);
			*dbg_tick = debug_tick;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    // can only get here with ":endwhile" or ":endfor"
		    ಯದಿ (cstack.cs_idx >= 0)
			rewind_conditionals(&cstack, cstack.cs_idx - 1,
				   CSF_WHILE | CSF_FOR, &cstack.cs_looplevel);
		}
	    }

	    /*
	     * For a ":while" or ":for" we need to remember the line number.
	     */
	    ಅನ್ಯಥಾ ಯದಿ (cstack.cs_lflags & CSL_HAD_LOOP)
	    {
		cstack.cs_lflags &= ~CSL_HAD_LOOP;
		cstack.cs_ಸಾಲು[cstack.cs_idx] = current_ಸಾಲು_before;
	    }
	}

	// Check for the next breakpoint after a watchexpression
	ಯದಿ (breakpoint != ರಿಕ್ತ && has_watchexpr())
	{
	    *breakpoint = dbg_find_breakpoint(FALSE, fname, SOURCING_LNUM);
	    *dbg_tick = debug_tick;
	}

	/*
	 * When not inside any ":while" loop, clear remembered lines.
	 */
	ಯದಿ (cstack.cs_looplevel == 0)
	{
	    ಯದಿ (ಸಾಲುಗಳು_ga.ga_len > 0)
	    {
		SOURCING_LNUM =
		       ((wcmd_T *)ಸಾಲುಗಳು_ga.ga_data)[ಸಾಲುಗಳು_ga.ga_len - 1].lnum;
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
	ಯದಿ (cstack.cs_lflags & CSL_HAD_FINA)
	{
	    cstack.cs_lflags &= ~CSL_HAD_FINA;
	    report_make_pending(cstack.cs_pending[cstack.cs_idx]
		    & (CSTP_ERROR | CSTP_INTERRUPT | CSTP_THROW),
		    did_throw ? (ಶೂನ್ಯ *)current_exception : ರಿಕ್ತ);
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
	ಯದಿ (trylevel == 0 && !did_emsg && !got_int && !did_throw)
	    force_abort = FALSE;

	// Convert an interrupt to an exception if appropriate.
	(ಶೂನ್ಯ)do_intthrow(&cstack);
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
    ಯಾವತ್ (!((got_int
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
	    && (next_cmdline != ರಿಕ್ತ
#ifdef FEAT_EVAL
			|| cstack.cs_idx >= 0
#endif
			|| (flags & DOCMD_REPEAT)));

    vim_free(cmdline_copy);
    did_emsg_syntax = FALSE;
#ifdef FEAT_EVAL
    free_cmdlines(&ಸಾಲುಗಳು_ga);
    ga_clear(&ಸಾಲುಗಳು_ga);

    ಯದಿ (cstack.cs_idx >= 0)
    {
	/*
	 * If a sourced file or executed function ran to its end, report the
	 * unclosed conditional.
	 * In Vim9 script do not give a second error, executing aborts after
	 * the first one.
	 */
	ಯದಿ (!got_int && !did_throw && !aborting()
		&& !(did_emsg && in_vim9script())
		&& ((getline_equal(fgetline, cookie, getsourceline)
			&& !source_finished(fgetline, cookie))
		    || (getline_equal(fgetline, cookie, get_func_ಸಾಲು)
					    && !func_has_ended(real_cookie))))
	{
	    ಯದಿ (cstack.cs_flags[cstack.cs_idx] & CSF_TRY)
		emsg(_(e_missing_endtry));
	    ಅನ್ಯಥಾ ಯದಿ (cstack.cs_flags[cstack.cs_idx] & CSF_WHILE)
		emsg(_(e_missing_endwhile));
	    ಅನ್ಯಥಾ ಯದಿ (cstack.cs_flags[cstack.cs_idx] & CSF_FOR)
		emsg(_(e_missing_endfor));
	    ಅನ್ಯಥಾ
		emsg(_(e_missing_endif));
	}

	/*
	 * Reset "trylevel" in case of a ":finish" or ":return" or a missing
	 * ":endtry" in a sourced file or executed function.  If the try
	 * conditional is in its finally clause, ignore anything pending.
	 * If it is in a catch clause, finish the caught exception.
	 * Also cleanup any "cs_forinfo" structures.
	 */
	ಕುರು
	{
	    ಪೂರ್ಣಾಙ್ಕ idx = cleanup_conditionals(&cstack, 0, TRUE);

	    ಯದಿ (idx >= 0)
		--idx;	    // remove try block not in its finally clause
	    rewind_conditionals(&cstack, idx, CSF_WHILE | CSF_FOR,
							&cstack.cs_looplevel);
	}
	ಯಾವತ್ (cstack.cs_idx >= 0);
	trylevel = initial_trylevel;
    }

    // If a missing ":endtry", ":endwhile", ":endfor", or ":endif" or a memory
    // lack was reported above and the error message is to be converted to an
    // exception, do this now after rewinding the cstack.
    do_errthrow(&cstack, getline_equal(fgetline, cookie, get_func_ಸಾಲು)
				  ? (char_u *)"endfunction" : (char_u *)ರಿಕ್ತ);

    ಯದಿ (trylevel == 0)
    {
	// Just in case did_throw got set but current_exception wasn't.
	ಯದಿ (current_exception == ರಿಕ್ತ)
	    did_throw = FALSE;

	/*
	 * When an exception is being thrown out of the outermost try
	 * conditional, discard the uncaught exception, disable the conversion
	 * of interrupts or errors to exceptions, and ensure that no more
	 * commands are executed.
	 */
	ಯದಿ (did_throw)
	    handle_did_throw();

	/*
	 * On an interrupt or an aborting error not converted to an exception,
	 * disable the conversion of errors to exceptions.  (Interrupts are not
	 * converted anymore, here.) This enables also the interrupt message
	 * when force_abort is set and did_emsg unset in case of an interrupt
	 * from a finally clause after an error.
	 */
	ಅನ್ಯಥಾ ಯದಿ (got_int || (did_emsg && force_abort))
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
    ಯದಿ (did_throw)
	need_rethrow = TRUE;
    ಯದಿ ((getline_equal(fgetline, cookie, getsourceline)
		&& ex_nesting_level > source_level(real_cookie))
	    || (getline_equal(fgetline, cookie, get_func_ಸಾಲು)
		&& ex_nesting_level > func_level(real_cookie) + 1))
    {
	ಯದಿ (!did_throw)
	    check_cstack = TRUE;
    }
    ಅನ್ಯಥಾ
    {
	// When leaving a function, reduce nesting level.
	ಯದಿ (getline_equal(fgetline, cookie, get_func_ಸಾಲು))
	    --ex_nesting_level;
	/*
	 * Go to debug mode when returning from a function in which we are
	 * single-stepping.
	 */
	ಯದಿ ((getline_equal(fgetline, cookie, getsourceline)
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
    ಯದಿ (flags & DOCMD_EXCRESET)
	restore_dbg_stuff(&debug_saved);

    ಸಂದೇಶ_list = saved_ಸಂದೇಶ_list;

    // Cleanup if "cs_emsg_silent_list" remains.
    ಯದಿ (cstack.cs_emsg_silent_list != ರಿಕ್ತ)
    {
	eslist_T *elem, *temp;

	ಪ್ರತಿ (elem = cstack.cs_emsg_silent_list; elem != ರಿಕ್ತ; elem = temp)
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
    ಯದಿ (did_inc_RedrawingDisabled)
    {
	ಯದಿ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	--no_wait_return;
	ಸಂದೇಶ_scroll = FALSE;

	/*
	 * When just finished an ":if"-":else" which was typed, no need to
	 * wait for hit-return.  Also for an error situation.
	 */
	ಯದಿ (retval == FAIL
#ifdef FEAT_EVAL
		|| (did_endif && ಕೀಲಿ_Typed && !did_emsg)
#endif
					    )
	{
	    need_wait_return = FALSE;
	    ಸಂದೇಶ_didany = FALSE;		// don't wait when restarting edit
	}
	ಅನ್ಯಥಾ ಯದಿ (need_wait_return)
	{
	    /*
	     * The msg_start() above clears msg_didout. The wait_return() we do
	     * here should not overwrite the command that may be shown before
	     * doing that.
	     */
	    ಸಂದೇಶ_didout |= ಸಂದೇಶ_didout_before_start;
	    wait_return(FALSE);
	}
    }

#ifdef FEAT_EVAL
    did_endif = FALSE;  // in case do_cmdline used recursively
#ಅನ್ಯಥಾ
    /*
     * Reset if_level, in case a sourced script file contains more ":if" than
     * ":endif" (could be ":if x | foo | endif").
     */
    if_level = 0;
#endif

    --call_depth;
    ಪ್ರತ್ಯಾಗಮ retval;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Handle when "did_throw" is set after executing commands.
 */
    ಶೂನ್ಯ
handle_did_throw(ಶೂನ್ಯ)
{
    ವರ್ಣ	*p = ರಿಕ್ತ;
    msglist_T	*messages = ರಿಕ್ತ;
    ESTACK_CHECK_DECLARATION;

    /*
     * If the uncaught exception is a user exception, report it as an
     * error.  If it is an error exception, display the saved error
     * message now.  For an interrupt exception, do nothing; the
     * interrupt message is given elsewhere.
     */
    ವಿಕಲ್ಪ (current_exception->type)
    {
	ಪಕ್ಷ ET_USER:
	    vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE,
		    _(e_exception_not_caught_str),
		    current_exception->ಮೌಲ್ಯ);
	    p = (ವರ್ಣ *)vim_strsave(IObuff);
	    ಭಙ್ಗ;
	ಪಕ್ಷ ET_ERROR:
	    messages = current_exception->messages;
	    current_exception->messages = ರಿಕ್ತ;
	    ಭಙ್ಗ;
	ಪಕ್ಷ ET_INTERRUPT:
	    ಭಙ್ಗ;
    }

    estack_ತಳ್ಳು(ETYPE_EXCEPT, current_exception->throw_name,
					current_exception->throw_lnum);
    ESTACK_CHECK_SETUP;
    current_exception->throw_name = ರಿಕ್ತ;

    discard_current_exception();	// uses IObuff if 'verbose'

    // If "silent!" is active the uncaught exception is not fatal.
    ಯದಿ (emsg_silent == 0)
    {
	suppress_errthrow = TRUE;
	force_abort = TRUE;
    }

    ಯದಿ (messages != ರಿಕ್ತ)
    {
	ಕುರು
	{
	    msglist_T	*next = messages->next;
	    ಪೂರ್ಣಾಙ್ಕ		save_compiling = estack_compiling;

	    estack_compiling = messages->ಸಂದೇಶ_compiling;
	    emsg(messages->ಸಂದೇಶ);
	    vim_free(messages->ಸಂದೇಶ);
	    vim_free(messages->sfile);
	    vim_free(messages);
	    messages = next;
	    estack_compiling = save_compiling;
	}
	ಯಾವತ್ (messages != ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ ಯದಿ (p != ರಿಕ್ತ)
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
get_loop_ಸಾಲು(ಪೂರ್ಣಾಙ್ಕ c, ಶೂನ್ಯ *cookie, ಪೂರ್ಣಾಙ್ಕ indent, getline_opt_T options)
{
    ಸಂರಚನಾ loop_cookie	*cp = (ಸಂರಚನಾ loop_cookie *)cookie;
    wcmd_T		*wp;
    char_u		*ಸಾಲು;

    ಯದಿ (cp->current_ಸಾಲು + 1 >= cp->ಸಾಲುಗಳು_gap->ga_len)
    {
	ಯದಿ (cp->repeating)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;	// trying to read past ":endwhile"/":endfor"

	// First time inside the ":while"/":for": get line normally.
	ಯದಿ (cp->lc_getline == ರಿಕ್ತ)
	    ಸಾಲು = getcmdline(c, 0L, indent, 0);
	ಅನ್ಯಥಾ
	    ಸಾಲು = cp->lc_getline(c, cp->cookie, indent, options);
	ಯದಿ (ಸಾಲು != ರಿಕ್ತ && store_loop_ಸಾಲು(cp->ಸಾಲುಗಳು_gap, ಸಾಲು) == OK)
	    ++cp->current_ಸಾಲು;

	ಪ್ರತ್ಯಾಗಮ ಸಾಲು;
    }

    ಕೀಲಿ_Typed = FALSE;
    ++cp->current_ಸಾಲು;
    wp = (wcmd_T *)(cp->ಸಾಲುಗಳು_gap->ga_data) + cp->current_ಸಾಲು;
    SOURCING_LNUM = wp->lnum;
    ಪ್ರತ್ಯಾಗಮ vim_strsave(wp->ಸಾಲು);
}

/*
 * Store a line in "gap" so that a ":while" loop can execute it again.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
store_loop_ಸಾಲು(garray_T *gap, char_u *ಸಾಲು)
{
    ಯದಿ (ga_grow(gap, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].ಸಾಲು = vim_strsave(ಸಾಲು);
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].lnum = SOURCING_LNUM;
    ++gap->ga_len;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Free the lines stored for a ":while" or ":for" loop.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_cmdlines(garray_T *gap)
{
    ಯಾವತ್ (gap->ga_len > 0)
    {
	vim_free(((wcmd_T *)(gap->ga_data))[gap->ga_len - 1].ಸಾಲು);
	--gap->ga_len;
    }
}
#endif

/*
 * If "fgetline" is get_loop_line(), return TRUE if the getline it uses equals
 * "func".  * Otherwise return TRUE when "fgetline" equals "func".
 */
    ಪೂರ್ಣಾಙ್ಕ
getline_equal(
    char_u	*(*fgetline)(ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ, getline_opt_T),
    ಶೂನ್ಯ	*cookie UNUSED,		// argument for fgetline()
    char_u	*(*func)(ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ, getline_opt_T))
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ, getline_opt_T);
    ಸಂರಚನಾ loop_cookie *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // function that's originally used to obtain the lines.  This may be
    // nested several levels.
    gp = fgetline;
    cp = (ಸಂರಚನಾ loop_cookie *)cookie;
    ಯಾವತ್ (gp == get_loop_ಸಾಲು)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    ಪ್ರತ್ಯಾಗಮ gp == func;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ fgetline == func;
#endif
}

/*
 * If "fgetline" is get_loop_line(), return the cookie used by the original
 * getline function.  Otherwise return "cookie".
 */
    ಶೂನ್ಯ *
getline_cookie(
    char_u	*(*fgetline)(ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ, getline_opt_T) UNUSED,
    ಶೂನ್ಯ	*cookie)		// argument for fgetline()
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ, getline_opt_T);
    ಸಂರಚನಾ loop_cookie  *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (ಸಂರಚನಾ loop_cookie *)cookie;
    ಯಾವತ್ (gp == get_loop_ಸಾಲು)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    ಪ್ರತ್ಯಾಗಮ cp;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ cookie;
#endif
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Get the next line source line without advancing.
 */
    char_u *
getline_peek(
    char_u	*(*fgetline)(ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ, getline_opt_T) UNUSED,
    ಶೂನ್ಯ	*cookie)		// argument for fgetline()
{
    char_u		*(*gp)(ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ, getline_opt_T);
    ಸಂರಚನಾ loop_cookie  *cp;
    wcmd_T		*wp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (ಸಂರಚನಾ loop_cookie *)cookie;
    ಯಾವತ್ (gp == get_loop_ಸಾಲು)
    {
	ಯದಿ (cp->current_ಸಾಲು + 1 < cp->ಸಾಲುಗಳು_gap->ga_len)
	{
	    // executing lines a second time, use the stored copy
	    wp = (wcmd_T *)(cp->ಸಾಲುಗಳು_gap->ga_data) + cp->current_ಸಾಲು + 1;
	    ಪ್ರತ್ಯಾಗಮ wp->ಸಾಲು;
	}
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    ಯದಿ (gp == getsourceline)
	ಪ್ರತ್ಯಾಗಮ source_nextline(cp);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif


/*
 * Helper function to apply an offset for buffer commands, i.e. ":bdelete",
 * ":bwipeout", etc.
 * Returns the buffer number.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compute_buffer_local_ಎಣಿಕೆ(ಪೂರ್ಣಾಙ್ಕ addr_type, ಪೂರ್ಣಾಙ್ಕ lnum, ಪೂರ್ಣಾಙ್ಕ offset)
{
    buf_T   *buf;
    buf_T   *nextbuf;
    ಪೂರ್ಣಾಙ್ಕ     ಎಣಿಕೆ = offset;

    buf = firstbuf;
    ಯಾವತ್ (buf->b_next != ರಿಕ್ತ && buf->b_fnum < lnum)
	buf = buf->b_next;
    ಯಾವತ್ (ಎಣಿಕೆ != 0)
    {
	ಎಣಿಕೆ += (offset < 0) ? 1 : -1;
	nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
	ಯದಿ (nextbuf == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	buf = nextbuf;
	ಯದಿ (addr_type == ADDR_LOADED_BUFFERS)
	    // skip over unloaded buffers
	    ಯಾವತ್ (buf->b_ml.ml_mfp == ರಿಕ್ತ)
	    {
		nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
		ಯದಿ (nextbuf == ರಿಕ್ತ)
		    ಭಙ್ಗ;
		buf = nextbuf;
	    }
    }
    // we might have gone too far, last buffer is not loadedd
    ಯದಿ (addr_type == ADDR_LOADED_BUFFERS)
	ಯಾವತ್ (buf->b_ml.ml_mfp == ರಿಕ್ತ)
	{
	    nextbuf = (offset >= 0) ? buf->b_prev : buf->b_next;
	    ಯದಿ (nextbuf == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    buf = nextbuf;
	}
    ಪ್ರತ್ಯಾಗಮ buf->b_fnum;
}

/*
 * Return the window number of "win".
 * When "win" is NULL return the number of windows.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
current_win_nr(win_T *win)
{
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		nr = 0;

    FOR_ALL_WINDOWS(wp)
    {
	++nr;
	ಯದಿ (wp == win)
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ nr;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
current_tab_nr(tabpage_T *tab)
{
    tabpage_T	*tp;
    ಪೂರ್ಣಾಙ್ಕ		nr = 0;

    FOR_ALL_TABPAGES(tp)
    {
	++nr;
	ಯದಿ (tp == tab)
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ nr;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
comment_start(char_u *p, ಪೂರ್ಣಾಙ್ಕ starts_with_colon UNUSED)
{
    ಯದಿ (in_vim9script())
	ಪ್ರತ್ಯಾಗಮ p[0] == '#' && !starts_with_colon;
    ಪ್ರತ್ಯಾಗಮ *p == '"';
}

#define CURRENT_WIN_NR current_win_nr(curwin)
#define LAST_WIN_NR current_win_nr(ರಿಕ್ತ)
#define CURRENT_TAB_NR current_tab_nr(curtab)
#define LAST_TAB_NR current_tab_nr(ರಿಕ್ತ)

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
    ಪೂರ್ಣಾಙ್ಕ		flags,
#ifdef FEAT_EVAL
    cstack_T	*cstack,
#endif
    char_u	*(*fgetline)(ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ, getline_opt_T),
    ಶೂನ್ಯ	*cookie)		// argument for fgetline()
{
    char_u	*p;
    linenr_T	lnum;
    long	n;
    ವರ್ಣ	*errormsg = ರಿಕ್ತ;	// error message
    char_u	*after_modifier = ರಿಕ್ತ;
    exarg_T	ea;			// Ex command arguments
    cmdmod_T	save_cmdmod;
    ಪೂರ್ಣಾಙ್ಕ		save_reg_executing = reg_executing;
    ಪೂರ್ಣಾಙ್ಕ		save_pending_end_reg_executing = pending_end_reg_executing;
    ಪೂರ್ಣಾಙ್ಕ		ni;			// set when Not Implemented
    char_u	*cmd;
    ಪೂರ್ಣಾಙ್ಕ		starts_with_colon = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		may_have_range;
#ifdef FEAT_EVAL
    ಪೂರ್ಣಾಙ್ಕ		did_set_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು = FALSE;
#endif
    ಪೂರ್ಣಾಙ್ಕ		sourcing = flags & DOCMD_VERBOSE;
    ಪೂರ್ಣಾಙ್ಕ		did_append_cmd = FALSE;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
#ifdef FEAT_EVAL
    ++ex_nesting_level;
#endif

    // When the last file has not been edited :q has to be typed twice.
    ಯದಿ (quitmore
#ifdef FEAT_EVAL
	    // avoid that a function call in 'statusline' does this
	    && !getline_equal(fgetline, cookie, get_func_ಸಾಲು)
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
    ಯದಿ ((*cmdlinep)[0] == '#' && (*cmdlinep)[1] == '!')
	ಗಚ್ಛ doend;

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
    ಯದಿ (parse_command_modifiers(&ea, &errormsg, &cmdmod, FALSE) == FAIL)
	ಗಚ್ಛ doend;
    apply_cmdmod(&cmdmod);
    after_modifier = ea.cmd;

#ifdef FEAT_EVAL
    ea.skip = did_emsg || got_int || did_throw || (cstack->cs_idx >= 0
			 && !(cstack->cs_flags[cstack->cs_idx] & CSF_ACTIVE));
#ಅನ್ಯಥಾ
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
    ಪೂರ್ಣಾಙ್ಕ vim9script = in_vim9script();
    ಯದಿ (vim9script && (flags & DOCMD_RANGEOK) == 0)
    {
	may_have_range = FALSE;
	ಪ್ರತಿ (p = ea.cmd; p >= *cmdlinep; --p)
	{
	    ಯದಿ (*p == ':')
		may_have_range = TRUE;
	    ಯದಿ (p < ea.cmd && !VIM_ISWHITE(*p))
		ಭಙ್ಗ;
	}
    }
    ಅನ್ಯಥಾ
	may_have_range = TRUE;
    ಯದಿ (may_have_range)
	ea.cmd = skip_range(ea.cmd, TRUE, ರಿಕ್ತ);

#ifdef FEAT_EVAL
    // Handle ":export" - it functions almost like a command modifier.
    // ":export var Name: type"
    // ":export def Name(..."
    // etc.
    ಯದಿ (vim9script && checkforcmd_noparen(&ea.cmd, "export", 6))
	is_export = TRUE;
#endif

    ಯದಿ (vim9script && !may_have_range)
    {
	ಯದಿ (ea.cmd == cmd + 1 && *cmd == '$')
	    // should be "$VAR = val"
	    --ea.cmd;
#ifdef FEAT_EVAL
	p = find_ex_command(&ea, ರಿಕ್ತ, lookup_scriptitem, ರಿಕ್ತ);
#ಅನ್ಯಥಾ
	p = find_ex_command(&ea, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
#endif
	ಯದಿ (ea.cmdidx == CMD_SIZE)
	{
	    char_u *ar = skip_range(ea.cmd, TRUE, ರಿಕ್ತ);

	    // If a ':' before the range is missing, give a clearer error
	    // message.
	    ಯದಿ (ar > ea.cmd && !ea.skip)
	    {
		semsg(_(e_colon_required_before_range_str), ea.cmd);
		ಗಚ್ಛ doend;
	    }
	}
    }
    ಅನ್ಯಥಾ
	p = find_ex_command(&ea, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    // Count this line for profiling if skip is TRUE.
    ಯದಿ (do_profiling == PROF_YES
	    && (!ea.skip || cstack->cs_idx == 0 || (cstack->cs_idx > 0
		     && (cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE))))
    {
	ಪೂರ್ಣಾಙ್ಕ skip = did_emsg || got_int || did_throw;

	ಯದಿ (ea.cmdidx == CMD_catch)
	    skip = !skip && !(cstack->cs_idx >= 0
			  && (cstack->cs_flags[cstack->cs_idx] & CSF_THROWN)
			  && !(cstack->cs_flags[cstack->cs_idx] & CSF_CAUGHT));
	ಅನ್ಯಥಾ ಯದಿ (ea.cmdidx == CMD_else || ea.cmdidx == CMD_elseif)
	    skip = skip || !(cstack->cs_idx >= 0
			  && !(cstack->cs_flags[cstack->cs_idx]
						  & (CSF_ACTIVE | CSF_TRUE)));
	ಅನ್ಯಥಾ ಯದಿ (ea.cmdidx == CMD_finally)
	    skip = FALSE;
	ಅನ್ಯಥಾ ಯದಿ (ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endtry
		&& ea.cmdidx != CMD_endwhile)
	    skip = ea.skip;

	ಯದಿ (!skip)
	{
	    ಯದಿ (getline_equal(fgetline, cookie, get_func_ಸಾಲು))
		func_ಸಾಲು_exec(getline_cookie(fgetline, cookie));
	    ಅನ್ಯಥಾ ಯದಿ (getline_equal(fgetline, cookie, getsourceline))
		script_ಸಾಲು_exec();
	}
    }
# endif
#endif

    ea.cmd = cmd;

#ifdef FEAT_EVAL
    // May go to debug mode.  If this happens and the ">quit" debug command is
    // used, throw an interrupt exception and skip the next command.
    dbg_check_breakpoint(&ea);
    ಯದಿ (!ea.skip && got_int)
    {
	ea.skip = TRUE;
	(ಶೂನ್ಯ)do_intthrow(cstack);
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
    ಯದಿ (!IS_USER_CMDIDX(ea.cmdidx))
    {
	ಯದಿ (ea.cmdidx != CMD_SIZE)
	    ea.addr_type = cmdnames[(ಪೂರ್ಣಾಙ್ಕ)ea.cmdidx].cmd_addr_type;
	ಅನ್ಯಥಾ
	    ea.addr_type = ADDR_ಸಾಲುಗಳು;

	// :wincmd range depends on the argument.
	ಯದಿ (ea.cmdidx == CMD_wincmd && p != ರಿಕ್ತ)
	    get_wincmd_addr_type(skipwhite(p), &ea);
#ifdef FEAT_QUICKFIX
	// :.cc in quickfix window uses line number
	ಯದಿ ((ea.cmdidx == CMD_cc || ea.cmdidx == CMD_ll) && bt_quickfix(curbuf))
	    ea.addr_type = ADDR_OTHER;
#endif
    }

    ಯದಿ (!may_have_range)
	ea.line1 = ea.line2 = default_address(&ea);
    ಅನ್ಯಥಾ ಯದಿ (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
	ಗಚ್ಛ doend;

/*
 * 5. Parse the command.
 */

    /*
     * Skip ':' and any white space
     */
    ea.cmd = skipwhite(ea.cmd);
    ಯಾವತ್ (*ea.cmd == ':')
	ea.cmd = skipwhite(ea.cmd + 1);

    /*
     * If we got a line, but no command, then go to the line.
     * If we find a '|' or '\n' we set ea.nextcmd.
     */
    ಯದಿ (*ea.cmd == NUL || comment_start(ea.cmd, starts_with_colon)
			       || (ea.nextcmd = check_nextcmd(ea.cmd)) != ರಿಕ್ತ)
    {
	/*
	 * strange vi behaviour:
	 * ":3"		jumps to line 3
	 * ":3|..."	prints line 3  (not in Vim9 script)
	 * ":|"		prints current line  (not in Vim9 script)
	 */
	ಯದಿ (ea.skip)	    // skip this if inside :if
	    ಗಚ್ಛ doend;
	errormsg = ex_range_without_command(&ea);
	ಗಚ್ಛ doend;
    }

    // If this looks like an undefined user command and there are CmdUndefined
    // autocommands defined, trigger the matching autocommands.
    ಯದಿ (p != ರಿಕ್ತ && ea.cmdidx == CMD_SIZE && !ea.skip
	    && ASCII_ISUPPER(*ea.cmd)
	    && has_cmdundefined())
    {
	ಪೂರ್ಣಾಙ್ಕ ret;

	p = ea.cmd;
	ಯಾವತ್ (ASCII_ISALNUM(*p))
	    ++p;
	p = vim_strnsave(ea.cmd, p - ea.cmd);
	ret = apply_autocmds(EVENT_CMDUNDEFINED, p, p, TRUE, ರಿಕ್ತ);
	vim_free(p);
	// If the autocommands did something and didn't cause an error, try
	// finding the command again.
	p = (ret
#ifdef FEAT_EVAL
		&& !aborting()
#endif
		) ? find_ex_command(&ea, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ) : ea.cmd;
    }

    ಯದಿ (p == ರಿಕ್ತ)
    {
	ಯದಿ (!ea.skip)
	    errormsg = _(e_ambiguous_use_of_user_defined_command);
	ಗಚ್ಛ doend;
    }
    // Check for wrong commands.
    ಯದಿ (*p == '!' && ea.cmd[1] == 0151 && ea.cmd[0] == 78
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	errormsg = uc_fun_cmd();
	ಗಚ್ಛ doend;
    }

    ಯದಿ (ea.cmdidx == CMD_SIZE)
    {
	ಯದಿ (!ea.skip)
	{
	    STRCPY(IObuff, _(e_not_an_editor_command));
	    ಯದಿ (!sourcing)
	    {
		// If the modifier was parsed OK the error must be in the
		// following command
		ಯದಿ (after_modifier != ರಿಕ್ತ)
		    append_command(after_modifier);
		ಅನ್ಯಥಾ
		    append_command(*cmdlinep);
		did_append_cmd = TRUE;
	    }
	    errormsg = (ವರ್ಣ *)IObuff;
	    did_emsg_syntax = TRUE;
	}
	ಗಚ್ಛ doend;
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
    ಯದಿ (ea.cmdidx == CMD_if)
	++if_level;
    ಯದಿ (if_level)
    {
	ಯದಿ (ea.cmdidx == CMD_endif)
	    --if_level;
	ಗಚ್ಛ doend;
    }

#endif

    // forced commands
    ಯದಿ (*p == '!' && ea.cmdidx != CMD_substitute
	    && ea.cmdidx != CMD_smagic && ea.cmdidx != CMD_snomagic)
    {
	++p;
	ea.forceit = TRUE;
    }
    ಅನ್ಯಥಾ
	ea.forceit = FALSE;

/*
 * 6. Parse arguments.  Then check for errors.
 */
    ಯದಿ (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = (long)cmdnames[(ಪೂರ್ಣಾಙ್ಕ)ea.cmdidx].cmd_argt;

    ಯದಿ (!ea.skip)
    {
#ifdef HAVE_SANDBOX
	ಯದಿ (sandbox != 0 && !(ea.argt & EX_SBOXOK))
	{
	    // Command not allowed in sandbox.
	    errormsg = _(e_not_allowed_in_sandbox);
	    ಗಚ್ಛ doend;
	}
#endif
	ಯದಿ (restricted != 0 && (ea.argt & EX_RESTRICT))
	{
	    errormsg = _(e_command_not_allowed_in_rvim);
	    ಗಚ್ಛ doend;
	}
	ಯದಿ (!curbuf->b_p_ma && (ea.argt & EX_MODIFY))
	{
	    // Command not allowed in non-'modifiable' buffer
	    errormsg = _(e_cannot_make_changes_modifiable_is_off);
	    ಗಚ್ಛ doend;
	}

	ಯದಿ (!IS_USER_CMDIDX(ea.cmdidx))
	{
	    ಯದಿ (cmdwin_type != 0 && !(ea.argt & EX_CMDWIN))
	    {
		// Command not allowed in the command line window
		errormsg = _(e_invalid_in_cmdline_window);
		ಗಚ್ಛ doend;
	    }
	    ಯದಿ (text_locked() && !(ea.argt & EX_LOCK_OK))
	    {
		// Command not allowed when text is locked
		errormsg = _(get_text_locked_ಸಂದೇಶ());
		ಗಚ್ಛ doend;
	    }
	}

	// Disallow editing another buffer when "curbuf_lock" is set.
	// Do allow ":checktime" (it is postponed).
	// Do allow ":edit" (check for an argument later).
	// Do allow ":file" with no arguments (check for an argument later).
	ಯದಿ (!(ea.argt & (EX_CMDWIN | EX_LOCK_OK))
		&& ea.cmdidx != CMD_checktime
		&& ea.cmdidx != CMD_edit
		&& ea.cmdidx != CMD_file
		&& !IS_USER_CMDIDX(ea.cmdidx)
		&& curbuf_locked())
	    ಗಚ್ಛ doend;

	ಯದಿ (!ni && !(ea.argt & EX_RANGE) && ea.addr_ಎಣಿಕೆ > 0)
	{
	    errormsg = _(e_no_range_allowed);
	    ಗಚ್ಛ doend;
	}
    }

    ಯದಿ (!ni && !(ea.argt & EX_BANG) && ea.forceit)
    {
	errormsg = _(e_no_bang_allowed);
	ಗಚ್ಛ doend;
    }

    /*
     * Don't complain about the range if it is not used
     * (could happen if line_count is accidentally set to 0).
     */
    ಯದಿ (!ea.skip && !ni && (ea.argt & EX_RANGE))
    {
	/*
	 * If the range is backwards, ask for confirmation and, if given, swap
	 * ea.line1 & ea.line2 so it's forwards again.
	 * When global command is busy, don't ask, will fail below.
	 */
	ಯದಿ (!global_busy && ea.line1 > ea.line2)
	{
	    ಯದಿ (ಸಂದೇಶ_silent == 0)
	    {
		ಯದಿ (sourcing || exmode_active)
		{
		    errormsg = _(e_backwards_range_given);
		    ಗಚ್ಛ doend;
		}
		ಯದಿ (ask_yesno((char_u *)
			_("Backwards range given, OK to swap"), FALSE) != 'y')
		    ಗಚ್ಛ doend;
	    }
	    lnum = ea.line1;
	    ea.line1 = ea.line2;
	    ea.line2 = lnum;
	}
	ಯದಿ ((errormsg = invalid_range(&ea)) != ರಿಕ್ತ)
	    ಗಚ್ಛ doend;
    }

    ಯದಿ ((ea.addr_type == ADDR_OTHER) && ea.addr_ಎಣಿಕೆ == 0)
	// default is 1, not cursor
	ea.line2 = 1;

    correct_range(&ea);

#ifdef FEAT_FOLDING
    ಯದಿ (((ea.argt & EX_WHOLEFOLD) || ea.addr_ಎಣಿಕೆ >= 2) && !global_busy
	    && ea.addr_type == ADDR_ಸಾಲುಗಳು)
    {
	// Put the first line at the start of a closed fold, put the last line
	// at the end of a closed fold.
	(ಶೂನ್ಯ)hasFolding(ea.line1, &ea.line1, ರಿಕ್ತ);
	(ಶೂನ್ಯ)hasFolding(ea.line2, ರಿಕ್ತ, &ea.line2);
    }
#endif

#ifdef FEAT_QUICKFIX
    /*
     * For the ":make" and ":grep" commands we insert the 'makeprg'/'grepprg'
     * option here, so things like % get expanded.
     */
    p = replace_makeprg(&ea, p, cmdlinep);
    ಯದಿ (p == ರಿಕ್ತ)
	ಗಚ್ಛ doend;
#endif

    /*
     * Skip to start of argument.
     * Don't do this for the ":!" command, because ":!! -l" needs the space.
     */
    ಯದಿ (ea.cmdidx == CMD_bang)
	ea.arg = p;
    ಅನ್ಯಥಾ
	ea.arg = skipwhite(p);

    // ":file" cannot be run with an argument when "curbuf_lock" is set
    ಯದಿ (ea.cmdidx == CMD_file && *ea.arg != NUL && curbuf_locked())
	ಗಚ್ಛ doend;

    /*
     * Check for "++opt=val" argument.
     * Must be first, allow ":w ++enc=utf8 !cmd"
     */
    ಯದಿ (ea.argt & EX_ARGOPT)
	ಯಾವತ್ (ea.arg[0] == '+' && ea.arg[1] == '+')
	    ಯದಿ (getargopt(&ea) == FAIL && !ni)
	    {
		errormsg = _(e_invalid_argument);
		ಗಚ್ಛ doend;
	    }

    ಯದಿ (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	ಯದಿ (*ea.arg == '>')			// append
	{
	    ಯದಿ (*++ea.arg != '>')		// typed wrong
	    {
		errormsg = _(e_use_w_or_w_gt_gt);
		ಗಚ್ಛ doend;
	    }
	    ea.arg = skipwhite(ea.arg + 1);
	    ea.append = TRUE;
	}
	ಅನ್ಯಥಾ ಯದಿ (*ea.arg == '!' && ea.cmdidx == CMD_write)  // :w !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    ಯದಿ (ea.cmdidx == CMD_read)
    {
	ಯದಿ (ea.forceit)
	{
	    ea.usefilter = TRUE;		// :r! filter if ea.forceit
	    ea.forceit = FALSE;
	}
	ಅನ್ಯಥಾ ಯದಿ (*ea.arg == '!')		// :r !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    ಯದಿ (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ea.amount = 1;
	ಯಾವತ್ (*ea.arg == *ea.cmd)		// count number of '>' or '<'
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
    ಯದಿ ((ea.argt & EX_CMDARG) && !ea.usefilter)
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
    ಯದಿ ((ea.argt & EX_TRLBAR) && !ea.usefilter)
    {
	separate_nextcmd(&ea, FALSE);
    }
    ಅನ್ಯಥಾ ಯದಿ (ea.cmdidx == CMD_bang
	    || ea.cmdidx == CMD_terminal
	    || ea.cmdidx == CMD_global
	    || ea.cmdidx == CMD_vglobal
	    || ea.usefilter
#ifdef FEAT_EVAL
	    || inside_block(&ea)
#endif
	    )
    {
	ಪ್ರತಿ (p = ea.arg; *p; ++p)
	{
	    // Remove one backslash before a newline
	    ಯದಿ (*p == '\\' && p[1] == '\n')
		STRMOVE(p, p + 1);
	    ಅನ್ಯಥಾ ಯದಿ (*p == '\n' && !(ea.argt & EX_ಅಭಿವ್ಯಕ್ತಿ_ARG))
	    {
		ea.nextcmd = p + 1;
		*p = NUL;
		ಭಙ್ಗ;
	    }
	}
    }

    ಯದಿ ((ea.argt & EX_DFLALL) && ea.addr_ಎಣಿಕೆ == 0)
	address_default_all(&ea);

    // accept numbered register only when no count allowed (:put)
    ಯದಿ (       (ea.argt & EX_REGSTR)
	    && *ea.arg != NUL
	       // Do not allow register = for user commands
	    && (!IS_USER_CMDIDX(ea.cmdidx) || *ea.arg != '=')
	    && !((ea.argt & EX_ಎಣಿಕೆ) && VIM_ISDIGIT(*ea.arg)))
    {
#ಯದಿ !defined(FEAT_CLIPBOARD) && !defined(FEAT_CLIPBOARD_PROVIDER)
	// check these explicitly for a more specific error message
	ಯದಿ (*ea.arg == '*' || *ea.arg == '+')
	{
	    errormsg = _(e_invalid_register_name);
	    ಗಚ್ಛ doend;
	}
#endif
	ಯದಿ (valid_yank_reg(*ea.arg, (!IS_USER_CMDIDX(ea.cmdidx)
			    && ea.cmdidx != CMD_put && ea.cmdidx != CMD_iput)))
	{
	    ea.regname = *ea.arg++;
#ifdef FEAT_EVAL
	    // for '=' register: accept the rest of the line as an expression
	    ಯದಿ (ea.arg[-1] == '=' && ea.arg[0] != NUL)
	    {
		ಯದಿ (!ea.skip)
		{
		    set_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು(vim_strsave(ea.arg), &ea);
		    did_set_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು = TRUE;
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
    ಯದಿ ((ea.argt & EX_ಎಣಿಕೆ) && VIM_ISDIGIT(*ea.arg)
	    && (!(ea.argt & EX_BUFNAME) || *(p = skipdigits(ea.arg + 1)) == NUL
							  || VIM_ISWHITE(*p)))
    {
	n = getdigits_quoted(&ea.arg);
	ea.arg = skipwhite(ea.arg);
	ಯದಿ (n <= 0 && !ni && (ea.argt & EX_ZEROR) == 0)
	{
	    errormsg = _(e_positive_ಎಣಿಕೆ_required);
	    ಗಚ್ಛ doend;
	}
	ಯದಿ (ea.addr_type != ADDR_ಸಾಲುಗಳು)	// e.g. :buffer 2, :sleep 3
	{
	    ea.line2 = n;
	    ಯದಿ (ea.addr_ಎಣಿಕೆ == 0)
		ea.addr_ಎಣಿಕೆ = 1;
	}
	ಅನ್ಯಥಾ
	{
	    ea.line1 = ea.line2;
	    ಯದಿ (ea.line2 >= LONG_MAX - (n - 1))
		ea.line2 = LONG_MAX;  // avoid overflow
	    ಅನ್ಯಥಾ
		ea.line2 += n - 1;
	    ++ea.addr_ಎಣಿಕೆ;
	    /*
	     * Be vi compatible: no error message for out of range.
	     */
	    ಯದಿ (ea.line2 > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ea.line2 = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	}
    }

    /*
     * Check for flags: 'l', 'p' and '#'.
     */
    ಯದಿ (ea.argt & EX_FLAGS)
	get_flags(&ea);
    ಯದಿ (!ni && !(ea.argt & EX_EXTRA) && *ea.arg != NUL
	    && *ea.arg != '"' && (*ea.arg != '|' || (ea.argt & EX_TRLBAR) == 0))
    {
	// no arguments allowed but there is something
	errormsg = ex_errmsg(e_trailing_characters_str, ea.arg);
	ಗಚ್ಛ doend;
    }

    ಯದಿ (!ni && (ea.argt & EX_NEEDARG) && *ea.arg == NUL)
    {
	errormsg = _(e_argument_required);
	ಗಚ್ಛ doend;
    }

#ifdef FEAT_EVAL
    /*
     * Skip the command when it's not going to be executed.
     * The commands like :if, :endif, etc. always need to be executed.
     * Also make an exception for commands that handle a trailing command
     * themselves.
     */
    ಯದಿ (ea.skip)
    {
	ವಿಕಲ್ಪ (ea.cmdidx)
	{
	    // commands that need evaluation
	    ಪಕ್ಷ CMD_while:
	    ಪಕ್ಷ CMD_endwhile:
	    ಪಕ್ಷ CMD_for:
	    ಪಕ್ಷ CMD_endfor:
	    ಪಕ್ಷ CMD_if:
	    ಪಕ್ಷ CMD_elseif:
	    ಪಕ್ಷ CMD_else:
	    ಪಕ್ಷ CMD_endif:
	    ಪಕ್ಷ CMD_try:
	    ಪಕ್ಷ CMD_catch:
	    ಪಕ್ಷ CMD_finally:
	    ಪಕ್ಷ CMD_endtry:
	    ಪಕ್ಷ CMD_function:
	    ಪಕ್ಷ CMD_def:
				ಭಙ್ಗ;

	    // commands that read a block of lines
	    ಪಕ್ಷ CMD_abstract:
	    ಪಕ್ಷ CMD_append:
	    ಪಕ್ಷ CMD_autocmd:
	    ಪಕ್ಷ CMD_change:
	    ಪಕ್ಷ CMD_class:
	    ಪಕ್ಷ CMD_command:
	    ಪಕ್ಷ CMD_enum:
	    ಪಕ್ಷ CMD_ಸೇರಿಸು:
	    ಪಕ್ಷ CMD_interface:
	    ಪಕ್ಷ CMD_loadkeymap:
				ಭಙ್ಗ;

	    // Commands that handle '|' themselves.  Check: A command should
	    // either have the EX_TRLBAR flag, appear in this list or appear in
	    // the list at ":help :bar".
	    ಪಕ್ಷ CMD_aboveleft:
	    ಪಕ್ಷ CMD_and:
	    ಪಕ್ಷ CMD_belowright:
	    ಪಕ್ಷ CMD_botright:
	    ಪಕ್ಷ CMD_browse:
	    ಪಕ್ಷ CMD_call:
	    ಪಕ್ಷ CMD_confirm:
	    ಪಕ್ಷ CMD_const:
	    ಪಕ್ಷ CMD_delfunction:
	    ಪಕ್ಷ CMD_djump:
	    ಪಕ್ಷ CMD_dlist:
	    ಪಕ್ಷ CMD_dsearch:
	    ಪಕ್ಷ CMD_dsplit:
	    ಪಕ್ಷ CMD_echo:
	    ಪಕ್ಷ CMD_echoerr:
	    ಪಕ್ಷ CMD_echomsg:
	    ಪಕ್ಷ CMD_echon:
	    ಪಕ್ಷ CMD_eval:
	    ಪಕ್ಷ CMD_execute:
	    ಪಕ್ಷ CMD_filter:
	    ಪಕ್ಷ CMD_final:
	    ಪಕ್ಷ CMD_help:
	    ಪಕ್ಷ CMD_hide:
	    ಪಕ್ಷ CMD_horizontal:
	    ಪಕ್ಷ CMD_ijump:
	    ಪಕ್ಷ CMD_ilist:
	    ಪಕ್ಷ CMD_isearch:
	    ಪಕ್ಷ CMD_isplit:
	    ಪಕ್ಷ CMD_keepalt:
	    ಪಕ್ಷ CMD_keepjumps:
	    ಪಕ್ಷ CMD_keepmarks:
	    ಪಕ್ಷ CMD_keeppatterns:
	    ಪಕ್ಷ CMD_leftabove:
	    ಪಕ್ಷ CMD_let:
	    ಪಕ್ಷ CMD_lockmarks:
	    ಪಕ್ಷ CMD_lockvar:
	    ಪಕ್ಷ CMD_lua:
	    ಪಕ್ಷ CMD_match:
	    ಪಕ್ಷ CMD_mzscheme:
	    ಪಕ್ಷ CMD_noautocmd:
	    ಪಕ್ಷ CMD_noswapfile:
	    ಪಕ್ಷ CMD_perl:
	    ಪಕ್ಷ CMD_psearch:
	    ಪಕ್ಷ CMD_py3:
	    ಪಕ್ಷ CMD_python3:
	    ಪಕ್ಷ CMD_python:
	    ಪಕ್ಷ CMD_return:
	    ಪಕ್ಷ CMD_rightbelow:
	    ಪಕ್ಷ CMD_ruby:
	    ಪಕ್ಷ CMD_silent:
	    ಪಕ್ಷ CMD_smagic:
	    ಪಕ್ಷ CMD_snomagic:
	    ಪಕ್ಷ CMD_substitute:
	    ಪಕ್ಷ CMD_syntax:
	    ಪಕ್ಷ CMD_tab:
	    ಪಕ್ಷ CMD_tcl:
	    ಪಕ್ಷ CMD_throw:
	    ಪಕ್ಷ CMD_tilde:
	    ಪಕ್ಷ CMD_topleft:
	    ಪಕ್ಷ CMD_unlet:
	    ಪಕ್ಷ CMD_unlockvar:
	    ಪಕ್ಷ CMD_var:
	    ಪಕ್ಷ CMD_verbose:
	    ಪಕ್ಷ CMD_vertical:
	    ಪಕ್ಷ CMD_wincmd:
				ಭಙ್ಗ;

	    ಸ್ವಭಾವ:		ಗಚ್ಛ doend;
	}
    }
#endif

    ಯದಿ ((ea.argt & EX_XFILE)
	    && expand_filename(&ea, cmdlinep, &errormsg) == FAIL)
	ಗಚ್ಛ doend;

#ifdef FEAT_EVAL
    ಯದಿ (is_export && (ea.argt & EX_EXPORT) == 0)
    {
	emsg(_(e_invalid_command_after_export));
	ಗಚ್ಛ doend;
    }
#endif

    /*
     * Accept buffer name.  Cannot be used at the same time with a buffer
     * number.  Don't do this for a user command.
     */
    ಯದಿ ((ea.argt & EX_BUFNAME) && *ea.arg != NUL && ea.addr_ಎಣಿಕೆ == 0
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * :bdelete, :bwipeout and :bunload take several arguments, separated
	 * by spaces: find next space (skipping over escaped characters).
	 * The others take one argument: ignore trailing spaces.
	 */
	ಯದಿ (ea.cmdidx == CMD_bdelete || ea.cmdidx == CMD_bwipeout
						  || ea.cmdidx == CMD_bunload)
	    p = skiptowhite_esc(ea.arg);
	ಅನ್ಯಥಾ
	{
	    p = ea.arg + STRLEN(ea.arg);
	    ಯಾವತ್ (p > ea.arg && VIM_ISWHITE(p[-1]))
		--p;
	}
	ea.line2 = buflist_findpat(ea.arg, p, (ea.argt & EX_BUFUNL) != 0,
								FALSE, FALSE);
	ಯದಿ (ea.line2 < 0)	    // failed
	    ಗಚ್ಛ doend;
	ea.addr_ಎಣಿಕೆ = 1;
	ea.arg = skipwhite(p);
    }

    // The :try command saves the emsg_silent flag, reset it here when
    // ":silent! try" was used, it should only apply to :try itself.
    ಯದಿ (ea.cmdidx == CMD_try && cmdmod.cmod_did_esilent > 0)
    {
	emsg_silent -= cmdmod.cmod_did_esilent;
	ಯದಿ (emsg_silent < 0)
	    emsg_silent = 0;
	cmdmod.cmod_did_esilent = 0;
    }

/*
 * 7. Execute the command.
 */

    ಯದಿ (IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * Execute a user-defined command.
	 */
	do_ucmd(&ea);
    }
    ಅನ್ಯಥಾ
    {
	/*
	 * Call the function to execute the builtin command.
	 */
	(cmdnames[ea.cmdidx].cmd_func)(&ea);
	ಯದಿ (ea.errmsg != ರಿಕ್ತ)
	    errormsg = ea.errmsg;
    }

#ifdef FEAT_EVAL
    // A command will reset "is_export" when exporting an item.  If it is still
    // set something went wrong or the command was never executed.
    ಯದಿ (!ea.skip && is_export)
    {
	ಯದಿ (errormsg == ರಿಕ್ತ)
	    errormsg = _(e_export_with_invalid_argument);
	is_export = FALSE;
    }

    // Set flag that any command was executed, used by ex_vim9script().
    // Not if this was a command that wasn't executed or :endif.
    ಯದಿ (sourcing_a_script(&ea)
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
    ಯದಿ (need_rethrow)
	do_throw(cstack);
    ಅನ್ಯಥಾ ಯದಿ (check_cstack)
    {
	ಯದಿ (source_finished(fgetline, cookie))
	    do_finish(&ea, TRUE);
	ಅನ್ಯಥಾ ಯದಿ (getline_equal(fgetline, cookie, get_func_ಸಾಲು)
						   && current_func_returned())
	    do_return(&ea, TRUE, FALSE, ರಿಕ್ತ);
    }
    need_rethrow = check_cstack = FALSE;
#endif

doend:
    ಯದಿ (curwin->w_cursor.lnum == 0)	// can happen with zero line number
    {
	curwin->w_cursor.lnum = 1;
	curwin->w_cursor.col = 0;
    }

    ಯದಿ (errormsg != ರಿಕ್ತ && *errormsg != NUL && !did_emsg)
    {
	ಯದಿ ((sourcing || !ಕೀಲಿ_Typed) && !did_append_cmd)
	{
	    ಯದಿ (errormsg != (ವರ್ಣ *)IObuff)
	    {
		STRCPY(IObuff, errormsg);
		errormsg = (ವರ್ಣ *)IObuff;
	    }
	    append_command(*cmdlinep);
	}
	emsg(errormsg);
    }
#ifdef FEAT_EVAL
    do_errthrow(cstack,
	    (ea.cmdidx != CMD_SIZE && !IS_USER_CMDIDX(ea.cmdidx))
			? cmdnames[(ಪೂರ್ಣಾಙ್ಕ)ea.cmdidx].cmd_name : (char_u *)ರಿಕ್ತ);

    ಯದಿ (did_set_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು)
	set_ಅಭಿವ್ಯಕ್ತಿ_ಸಾಲು(ರಿಕ್ತ, ರಿಕ್ತ);
    is_export = FALSE;
#endif

    undo_cmdmod(&cmdmod);
    cmdmod = save_cmdmod;
    reg_executing = save_reg_executing;
    pending_end_reg_executing = save_pending_end_reg_executing;

    ಯದಿ (ea.nextcmd && *ea.nextcmd == NUL)	// not really a next command
	ea.nextcmd = ರಿಕ್ತ;

#ifdef FEAT_EVAL
    --ex_nesting_level;
    vim_free(ea.cmdline_tofree);
#endif

    ಪ್ರತ್ಯಾಗಮ ea.nextcmd;
}

ಸ್ಥಿರ ವರ್ಣ ex_error_buf[ಸಂದೇಶ_BUF_LEN];

/*
 * Return an error message with argument included.
 * Uses a static buffer, only the last error will be kept.
 * "msg" will be translated, caller should use N_().
 */
    ವರ್ಣ *
ex_errmsg(ವರ್ಣ *ಸಂದೇಶ, char_u *arg)
{
    vim_snprintf(ex_error_buf, ಸಂದೇಶ_BUF_LEN, _(ಸಂದೇಶ), arg);
    ಪ್ರತ್ಯಾಗಮ ex_error_buf;
}

/*
 * The "+" string used in place of an empty command in Ex mode.
 * This string is used in pointer comparison.
 */
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ exmode_plus[] = "+";

/*
 * Handle a range without a command.
 * Returns an error message on failure.
 */
    ವರ್ಣ *
ex_range_without_command(exarg_T *eap)
{
    ವರ್ಣ *errormsg = ರಿಕ್ತ;

    ಯದಿ ((*eap->cmd == '|' ||
		(exmode_active && eap->cmd != (char_u *)exmode_plus + 1))
#ifdef FEAT_EVAL
	    && !in_vim9script()
#endif
       )
    {
	eap->cmdidx = CMD_print;
	eap->argt = EX_RANGE+EX_ಎಣಿಕೆ+EX_TRLBAR;
	ಯದಿ ((errormsg = invalid_range(eap)) == ರಿಕ್ತ)
	{
	    correct_range(eap);
	    ex_print(eap);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (eap->addr_ಎಣಿಕೆ != 0)
    {
	ಯದಿ (eap->line2 > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    // With '-' in 'cpoptions' a line number past the file is an
	    // error, otherwise put it at the end of the file.
	    ಯದಿ (vim_strchr(p_cpo, CPO_MINUS) != ರಿಕ್ತ)
		eap->line2 = -1;
	    ಅನ್ಯಥಾ
		eap->line2 = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	}

	ಯದಿ (eap->line2 < 0)
	    errormsg = _(e_invalid_range);
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (eap->line2 == 0)
		curwin->w_cursor.lnum = 1;
	    ಅನ್ಯಥಾ
		curwin->w_cursor.lnum = eap->line2;
	    beginline(BL_SOL | BL_FIX);
	}
    }
    ಪ್ರತ್ಯಾಗಮ errormsg;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 * If "noparen" is TRUE do not recognize the command followed by "(" or ".".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
checkforcmd_opt(
    char_u	**pp,		// start of command
    ವರ್ಣ	*cmd,		// name of command
    ಪೂರ್ಣಾಙ್ಕ		len,		// required length
    ಪೂರ್ಣಾಙ್ಕ		noparen)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; cmd[i] != NUL; ++i)
	ಯದಿ (((char_u *)cmd)[i] != (*pp)[i])
	    ಭಙ್ಗ;
    ಯದಿ (i >= len && !ASCII_ISALPHA((*pp)[i]) && (*pp)[i] != '_'
			 && (!noparen || ((*pp)[i] != '(' && (*pp)[i] != '.')))
    {
	*pp = skipwhite(*pp + i);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    ಪೂರ್ಣಾಙ್ಕ
checkforcmd(
    char_u	**pp,		// start of command
    ವರ್ಣ	*cmd,		// name of command
    ಪೂರ್ಣಾಙ್ಕ		len)		// required length
{
    ಪ್ರತ್ಯಾಗಮ checkforcmd_opt(pp, cmd, len, FALSE);
}

/*
 * Check for an Ex command with optional tail, not followed by "(" or ".".
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    ಪೂರ್ಣಾಙ್ಕ
checkforcmd_noparen(
    char_u	**pp,		// start of command
    ವರ್ಣ	*cmd,		// name of command
    ಪೂರ್ಣಾಙ್ಕ		len)		// required length
{
    ಪ್ರತ್ಯಾಗಮ checkforcmd_opt(pp, cmd, len, TRUE);
}

/*
 * Find the replacement text of a ":command", the part after the attributes and
 * the command name.  Returns NULL when there is none.  Does not modify "arg"
 * and gives no error messages.
 */
    ಸ್ಥಿರ char_u *
find_ucmd_repl(char_u *arg)
{
    char_u	*p = arg;

    // Skip over the attributes.
    ಯಾವತ್ (*p == '-')
	p = skipwhite(skiptowhite(p));

    // Skip over the command name.
    ಯದಿ (!ASCII_ISALPHA(*p))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯಾವತ್ (ASCII_ISALNUM(*p))
	++p;

    ಪ್ರತ್ಯಾಗಮ *p == NUL ? ರಿಕ್ತ : skipwhite(p);
}

/*
 * Find the "{" in "line" that starts a block for ":command" or ":autocmd".
 * That is the case when the command argument is "{" at the end of the line,
 * also when the command is nested in another ":command" or ":autocmd".
 * Returns NULL when the line does not start such a block.
 */
    char_u *
find_cmd_block_start(char_u *ಸಾಲು)
{
    char_u	*p = skipwhite(ಸಾಲು);

    ಪ್ರತಿ (;;)
    {
	char_u	*arg = p;

	ಯದಿ (*p == '{' && ends_excmd2(p, skipwhite(p + 1)))
	    ಪ್ರತ್ಯಾಗಮ p;

	ಯದಿ (checkforcmd_noparen(&arg, "autocmd", 2))
	{
	    ಯದಿ (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = au_find_cmd_arg(arg);
	}
	ಅನ್ಯಥಾ ಯದಿ (checkforcmd_noparen(&arg, "command", 3))
	{
	    ಯದಿ (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = find_ucmd_repl(arg);
	}
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಯದಿ (p == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
    ಪೂರ್ಣಾಙ್ಕ
parse_command_modifiers(
	exarg_T	    *eap,
	ವರ್ಣ	    **errormsg,
	cmdmod_T    *cmod,
	ಪೂರ್ಣಾಙ್ಕ	    skip_only)
{
    char_u  *orig_cmd = eap->cmd;
    char_u  *cmd_start = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ	    use_plus_cmd = FALSE;
    ಪೂರ್ಣಾಙ್ಕ	    starts_with_colon = FALSE;
    ಪೂರ್ಣಾಙ್ಕ	    vim9script = in_vim9script();
    ಪೂರ್ಣಾಙ್ಕ	    has_visual_range = FALSE;

    CLEAR_POINTER(cmod);
    cmod->cmod_flags = sticky_cmdmod_flags;

    ಯದಿ (STRNCMP(eap->cmd, "'<,'>", 5) == 0)
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
    ಪ್ರತಿ (;;)
    {
	char_u  *p;

	ಯಾವತ್ (*eap->cmd == ' ' || *eap->cmd == '\t' || *eap->cmd == ':')
	{
	    ಯದಿ (*eap->cmd == ':')
		starts_with_colon = TRUE;
	    ++eap->cmd;
	}

	// in ex mode, an empty command (after modifiers) works like :+
	ಯದಿ (*eap->cmd == NUL && exmode_active
		   && (getline_equal(eap->ea_getline, eap->cookie, getexmodeline)
		       || getline_equal(eap->ea_getline, eap->cookie, getexline))
			&& curwin->w_cursor.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    use_plus_cmd = TRUE;
	    ಯದಿ (!skip_only)
		ex_pressedreturn = TRUE;
	    ಭಙ್ಗ;  // no modifiers following
	}

	// ignore comment and empty lines
	ಯದಿ (comment_start(eap->cmd, starts_with_colon))
	{
	    // a comment ends at a NL
	    eap->nextcmd = vim_strchr(eap->cmd, '\n');
	    ಯದಿ (eap->nextcmd != ರಿಕ್ತ)
		++eap->nextcmd;
	    ಯದಿ (vim9script)
	    {
		ಯದಿ (has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
#ifdef FEAT_EVAL
		ಯದಿ (eap->cmd[0] == '#' && eap->cmd[1] == '{'
							 && eap->cmd[2] != '{')
		    *errormsg = _(e_cannot_use_hash_curly_to_start_comment);
#endif
	    }
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (*eap->cmd == '\n')
	{
	    eap->nextcmd = eap->cmd + 1;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (*eap->cmd == NUL)
	{
	    ಯದಿ (!skip_only)
	    {
		ex_pressedreturn = TRUE;
		ಯದಿ (vim9script && has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
	    }
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	p = skip_range(eap->cmd, TRUE, ರಿಕ್ತ);

	// In Vim9 script a variable can shadow a command modifier:
	//   verbose = 123
	//   verbose += 123
	//   silent! verbose = func()
	//   verbose.member = 2
	//   verbose[expr] = 2
	// But not:
	//   verbose [a, b] = list
	ಯದಿ (vim9script)
	{
	    char_u *s, *n;

	    ಪ್ರತಿ (s = eap->cmd; ASCII_ISALPHA(*s); ++s)
		;
	    n = skipwhite(s);
	    ಯದಿ (*n == '.' || *n == '=' || (*n != NUL && n[1] == '=')
		    || *s == '[')
		ಭಙ್ಗ;
	}

	ವಿಕಲ್ಪ (*p)
	{
	    // When adding an entry, also modify cmdmod_info_tab[].
	    ಪಕ್ಷ 'a':	ಯದಿ (!checkforcmd_noparen(&eap->cmd, "aboveleft", 3))
			    ಭಙ್ಗ;
			cmod->cmod_split |= WSP_ABOVE;
			ಅನುವರ್ತ;

	    ಪಕ್ಷ 'b':	ಯದಿ (checkforcmd_noparen(&eap->cmd, "belowright", 3))
			{
			    cmod->cmod_split |= WSP_BELOW;
			    ಅನುವರ್ತ;
			}
			ಯದಿ (checkforcmd_opt(&eap->cmd, "browse", 3, TRUE))
			{
#ifdef FEAT_BROWSE_CMD
			    cmod->cmod_flags |= CMOD_BROWSE;
#endif
			    ಅನುವರ್ತ;
			}
			ಯದಿ (!checkforcmd_noparen(&eap->cmd, "botright", 2))
			    ಭಙ್ಗ;
			cmod->cmod_split |= WSP_BOT;
			ಅನುವರ್ತ;

	    ಪಕ್ಷ 'c':	ಯದಿ (!checkforcmd_opt(&eap->cmd, "confirm", 4, TRUE))
			    ಭಙ್ಗ;
#ಯದಿ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			cmod->cmod_flags |= CMOD_CONFIRM;
#endif
			ಅನುವರ್ತ;

	    ಪಕ್ಷ 'k':	ಯದಿ (checkforcmd_noparen(&eap->cmd, "keepmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_KEEPMARKS;
			    ಅನುವರ್ತ;
			}
			ಯದಿ (checkforcmd_noparen(&eap->cmd, "keepalt", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPALT;
			    ಅನುವರ್ತ;
			}
			ಯದಿ (checkforcmd_noparen(&eap->cmd, "keeppatterns", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPPATTERNS;
			    ಅನುವರ್ತ;
			}
			ಯದಿ (!checkforcmd_noparen(&eap->cmd, "keepjumps", 5))
			    ಭಙ್ಗ;
			cmod->cmod_flags |= CMOD_KEEPJUMPS;
			ಅನುವರ್ತ;

	    ಪಕ್ಷ 'f':	// only accept ":filter {pat} cmd"
			{
			    char_u  *reg_pat;
			    char_u  *nulp = ರಿಕ್ತ;
			    ಪೂರ್ಣಾಙ್ಕ	    c = 0;

			    ಯದಿ (!checkforcmd_noparen(&p, "filter", 4)
				    || *p == NUL
				    || (ends_excmd(*p)
#ifdef FEAT_EVAL
					// in ":filter #pat# cmd" # does not
					// start a comment
				     && (!vim9script || VIM_ISWHITE(p[1]))
#endif
				     ))
				ಭಙ್ಗ;
			    ಯದಿ (*p == '!')
			    {
				cmod->cmod_filter_force = TRUE;
				p = skipwhite(p + 1);
				ಯದಿ (*p == NUL || ends_excmd(*p))
				    ಭಙ್ಗ;
			    }
#ifdef FEAT_EVAL
			    // Avoid that "filter(arg)" is recognized.
			    ಯದಿ (vim9script && !VIM_ISWHITE(p[-1]))
				ಭಙ್ಗ;
#endif
			    ಯದಿ (skip_only)
				p = skip_vimgrep_pat(p, ರಿಕ್ತ, ರಿಕ್ತ);
			    ಅನ್ಯಥಾ
				// NOTE: This puts a NUL after the pattern.
				p = skip_vimgrep_pat_ext(p, &reg_pat, ರಿಕ್ತ,
								    &nulp, &c);
			    ಯದಿ (p == ರಿಕ್ತ || *p == NUL)
				ಭಙ್ಗ;
			    ಯದಿ (!skip_only)
			    {
				cmod->cmod_filter_regmatch.regprog =
						vim_regcomp(reg_pat, RE_MAGIC);
				ಯದಿ (cmod->cmod_filter_regmatch.regprog == ರಿಕ್ತ)
				    ಭಙ್ಗ;
				// restore the character overwritten by NUL
				ಯದಿ (nulp != ರಿಕ್ತ)
				    *nulp = c;
			    }
			    eap->cmd = p;
			    ಅನುವರ್ತ;
			}

	    ಪಕ್ಷ 'h':	ಯದಿ (checkforcmd_noparen(&eap->cmd, "horizontal", 3))
			{
			    cmod->cmod_split |= WSP_HOR;
			    ಅನುವರ್ತ;
			}
			// ":hide" and ":hide | cmd" are not modifiers
			ಯದಿ (p != eap->cmd || !checkforcmd_noparen(&p, "hide", 3)
					       || *p == NUL || ends_excmd(*p))
			    ಭಙ್ಗ;
			eap->cmd = p;
			cmod->cmod_flags |= CMOD_HIDE;
			ಅನುವರ್ತ;

	    ಪಕ್ಷ 'l':	ಯದಿ (checkforcmd_noparen(&eap->cmd, "lockmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_LOCKMARKS;
			    ಅನುವರ್ತ;
			}
			ಯದಿ (checkforcmd_noparen(&eap->cmd, "legacy", 3))
			{
			    ಯದಿ (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_legacy_must_be_followed_by_command);
				ಪ್ರತ್ಯಾಗಮ FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_VIM9CMD;
			    cmod->cmod_flags |= CMOD_LEGACY;
			    ಅನುವರ್ತ;
			}

			ಯದಿ (!checkforcmd_noparen(&eap->cmd, "leftabove", 5))
			    ಭಙ್ಗ;
			cmod->cmod_split |= WSP_ABOVE;
			ಅನುವರ್ತ;

	    ಪಕ್ಷ 'n':	ಯದಿ (checkforcmd_noparen(&eap->cmd, "noautocmd", 3))
			{
			    cmod->cmod_flags |= CMOD_NOAUTOCMD;
			    ಅನುವರ್ತ;
			}
			ಯದಿ (!checkforcmd_noparen(&eap->cmd, "noswapfile", 3))
			    ಭಙ್ಗ;
			cmod->cmod_flags |= CMOD_NOSWAPFILE;
			ಅನುವರ್ತ;

	    ಪಕ್ಷ 'r':	ಯದಿ (!checkforcmd_noparen(&eap->cmd, "rightbelow", 6))
			    ಭಙ್ಗ;
			cmod->cmod_split |= WSP_BELOW;
			ಅನುವರ್ತ;

	    ಪಕ್ಷ 's':	ಯದಿ (checkforcmd_noparen(&eap->cmd, "sandbox", 3))
			{
			    cmod->cmod_flags |= CMOD_SANDBOX;
			    ಅನುವರ್ತ;
			}
			ಯದಿ (!checkforcmd_noparen(&eap->cmd, "silent", 3))
			    ಭಙ್ಗ;
			cmod->cmod_flags |= CMOD_SILENT;
			ಯದಿ (*eap->cmd == '!' && !VIM_ISWHITE(eap->cmd[-1]))
			{
			    // ":silent!", but not "silent !cmd"
			    eap->cmd = skipwhite(eap->cmd + 1);
			    cmod->cmod_flags |= CMOD_ERRSILENT;
			}
			ಅನುವರ್ತ;

	    ಪಕ್ಷ 't':	ಯದಿ (checkforcmd_noparen(&p, "tab", 3))
			{
			    ಯದಿ (!skip_only)
			    {
				long tabnr = get_address(eap, &eap->cmd,
						    ADDR_TABS, eap->skip,
						    skip_only, FALSE, 1);
				ಯದಿ (tabnr == MAXLNUM)
				    cmod->cmod_tab = tabpage_index(curtab) + 1;
				ಅನ್ಯಥಾ
				{
				    ಯದಿ (tabnr < 0 || tabnr > LAST_TAB_NR)
				    {
					*errormsg = _(e_invalid_range);
					ಪ್ರತ್ಯಾಗಮ FAIL;
				    }
				    cmod->cmod_tab = tabnr + 1;
				}
			    }
			    eap->cmd = p;
			    ಅನುವರ್ತ;
			}
			ಯದಿ (!checkforcmd_noparen(&eap->cmd, "topleft", 2))
			    ಭಙ್ಗ;
			cmod->cmod_split |= WSP_ತುದಿ;
			ಅನುವರ್ತ;

	    ಪಕ್ಷ 'u':	ಯದಿ (!checkforcmd_noparen(&eap->cmd, "unsilent", 3))
			    ಭಙ್ಗ;
			cmod->cmod_flags |= CMOD_UNSILENT;
			ಅನುವರ್ತ;

	    ಪಕ್ಷ 'v':	ಯದಿ (checkforcmd_noparen(&eap->cmd, "vertical", 4))
			{
			    cmod->cmod_split |= WSP_VERT;
			    ಅನುವರ್ತ;
			}
			ಯದಿ (checkforcmd_noparen(&eap->cmd, "vim9cmd", 4))
			{
			    ಯದಿ (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_vim9cmd_must_be_followed_by_command);
				ಪ್ರತ್ಯಾಗಮ FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_LEGACY;
			    cmod->cmod_flags |= CMOD_VIM9CMD;
			    ಅನುವರ್ತ;
			}
			ಯದಿ (!checkforcmd_noparen(&p, "verbose", 4))
			    ಭಙ್ಗ;
			ಯದಿ (vim_isdigit(*eap->cmd))
			{
			    // zero means not set, one is verbose == 0, etc.
			    cmod->cmod_verbose = atoi((ವರ್ಣ *)eap->cmd) + 1;
			}
			ಅನ್ಯಥಾ
			    cmod->cmod_verbose = 2;  // default: verbose == 1
			eap->cmd = p;
			ಅನುವರ್ತ;
	}
	ಭಙ್ಗ;
    }

    ಯದಿ (has_visual_range)
    {
	ಯದಿ (eap->cmd > cmd_start)
	{
	    // Move the '<,'> range to after the modifiers and insert a colon.
	    // Since the modifiers have been parsed put the colon on top of the
	    // space: "'<,'>mod cmd" -> "mod:'<,'>cmd
	    // Put eap->cmd after the colon.
	    ಯದಿ (use_plus_cmd)
	    {
		size_t len = STRLEN(cmd_start);

		// Special case: empty command uses "+":
		//  "'<,'>mods" -> "mods *+
		//  Use "*" instead of "'<,'>" to avoid the command getting
		//  longer, in case it was allocated.
		mch_memmove(orig_cmd, cmd_start, len);
		STRCPY(orig_cmd + len, " *+");
	    }
	    ಅನ್ಯಥಾ
	    {
		mch_memmove(cmd_start - 5, cmd_start, eap->cmd - cmd_start);
		eap->cmd -= 5;
		mch_memmove(eap->cmd - 1, ":'<,'>", 6);
	    }
	}
	ಅನ್ಯಥಾ
	    // No modifiers, move the pointer back.
	    // Special case: change empty command to "+".
	    ಯದಿ (use_plus_cmd)
		eap->cmd = (char_u *)"'<,'>+";
	    ಅನ್ಯಥಾ
		eap->cmd = orig_cmd;
    }
    ಅನ್ಯಥಾ ಯದಿ (use_plus_cmd)
	eap->cmd = (char_u *)exmode_plus;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return TRUE if "cmod" has anything set.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_cmdmod(cmdmod_T *cmod, ಪೂರ್ಣಾಙ್ಕ ignore_silent)
{
    ಪ್ರತ್ಯಾಗಮ (cmod->cmod_flags != 0 && (!ignore_silent
		|| (cmod->cmod_flags
		      & ~(CMOD_SILENT | CMOD_ERRSILENT | CMOD_UNSILENT)) != 0))
	    || cmod->cmod_split != 0
	    || cmod->cmod_verbose > 0
	    || cmod->cmod_tab != 0
	    || cmod->cmod_filter_regmatch.regprog != ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * If Vim9 script and "cmdmod" has anything set give an error and return TRUE.
 */
    ಪೂರ್ಣಾಙ್ಕ
cmdmod_error(ಪೂರ್ಣಾಙ್ಕ ignore_silent)
{
    ಯದಿ (in_vim9script() && has_cmdmod(&cmdmod, ignore_silent))
    {
	emsg(_(e_misplaced_command_modifier));
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif

/*
 * Apply the command modifiers.  Saves current state in "cmdmod", call
 * undo_cmdmod() later.
 */
    ಶೂನ್ಯ
apply_cmdmod(cmdmod_T *cmod)
{
#ifdef HAVE_SANDBOX
    ಯದಿ ((cmod->cmod_flags & CMOD_SANDBOX) && !cmod->cmod_did_sandbox)
    {
	++sandbox;
	cmod->cmod_did_sandbox = TRUE;
    }
#endif
    ಯದಿ (cmod->cmod_verbose > 0)
    {
	ಯದಿ (cmod->cmod_verbose_save == 0)
	    cmod->cmod_verbose_save = p_verbose + 1;
	p_verbose = cmod->cmod_verbose - 1;
    }

    ಯದಿ ((cmod->cmod_flags & (CMOD_SILENT | CMOD_UNSILENT))
	    && cmod->cmod_save_ಸಂದೇಶ_silent == 0)
    {
	cmod->cmod_save_ಸಂದೇಶ_silent = ಸಂದೇಶ_silent + 1;
	cmod->cmod_save_ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll;
    }
    ಯದಿ (cmod->cmod_flags & CMOD_SILENT)
	++ಸಂದೇಶ_silent;
    ಯದಿ (cmod->cmod_flags & CMOD_UNSILENT)
	ಸಂದೇಶ_silent = 0;

    ಯದಿ (cmod->cmod_flags & CMOD_ERRSILENT)
    {
	++emsg_silent;
	++cmod->cmod_did_esilent;
    }

    ಯದಿ ((cmod->cmod_flags & CMOD_NOAUTOCMD) && cmod->cmod_save_ei == ರಿಕ್ತ)
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
    ಶೂನ್ಯ
undo_cmdmod(cmdmod_T *cmod)
{
    ಯದಿ (cmod->cmod_verbose_save > 0)
    {
	p_verbose = cmod->cmod_verbose_save - 1;
	cmod->cmod_verbose_save = 0;
    }

#ifdef HAVE_SANDBOX
    ಯದಿ (cmod->cmod_did_sandbox)
    {
	--sandbox;
	cmod->cmod_did_sandbox = FALSE;
    }
#endif

    ಯದಿ (cmod->cmod_save_ei != ರಿಕ್ತ)
    {
	// Restore 'eventignore' to the value before ":noautocmd".
	set_string_option_direct((char_u *)"ei", -1, cmod->cmod_save_ei,
							   OPT_FREE, SID_NONE);
	free_string_option(cmod->cmod_save_ei);
	cmod->cmod_save_ei = ರಿಕ್ತ;
    }

    vim_regfree(cmod->cmod_filter_regmatch.regprog);

    ಯದಿ (cmod->cmod_save_ಸಂದೇಶ_silent > 0)
    {
	// messages could be enabled for a serious error, need to check if the
	// counters don't become negative
	ಯದಿ (!did_emsg || ಸಂದೇಶ_silent > cmod->cmod_save_ಸಂದೇಶ_silent - 1)
	    ಸಂದೇಶ_silent = cmod->cmod_save_ಸಂದೇಶ_silent - 1;
	emsg_silent -= cmod->cmod_did_esilent;
	ಯದಿ (emsg_silent < 0)
	    emsg_silent = 0;
	// Restore msg_scroll, it's set by file I/O commands, even when no
	// message is actually displayed.
	ಸಂದೇಶ_scroll = cmod->cmod_save_ಸಂದೇಶ_scroll;

	// "silent reg" or "silent echo x" inside "redir" leaves msg_col
	// somewhere in the line.  Put it back in the first column.
	ಯದಿ (redirecting())
	    ಸಂದೇಶ_col = 0;

	cmod->cmod_save_ಸಂದೇಶ_silent = 0;
	cmod->cmod_did_esilent = 0;
    }
}

/*
 * Parse the address range, if any, in "eap".
 * May set the last search pattern, unless "silent" is TRUE.
 * Return FAIL and set "errormsg" or return OK.
 */
    ಪೂರ್ಣಾಙ್ಕ
parse_cmd_address(exarg_T *eap, ವರ್ಣ **errormsg, ಪೂರ್ಣಾಙ್ಕ silent)
{
    ಪೂರ್ಣಾಙ್ಕ		address_ಎಣಿಕೆ = 1;
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		need_check_cursor = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		ret = FAIL;

    // Repeat for all ',' or ';' separated addresses.
    ಪ್ರತಿ (;;)
    {
	eap->line1 = eap->line2;
	eap->line2 = default_address(eap);
	eap->cmd = skipwhite(eap->cmd);
	lnum = get_address(eap, &eap->cmd, eap->addr_type, eap->skip, silent,
					eap->addr_ಎಣಿಕೆ == 0, address_ಎಣಿಕೆ++);
	ಯದಿ (eap->cmd == ರಿಕ್ತ)	// error detected
	    ಗಚ್ಛ theend;
	ಯದಿ (lnum == MAXLNUM)
	{
	    ಯದಿ (*eap->cmd == '%')   // '%' - all lines
	    {
		++eap->cmd;
		ವಿಕಲ್ಪ (eap->addr_type)
		{
		    ಪಕ್ಷ ADDR_ಸಾಲುಗಳು:
		    ಪಕ್ಷ ADDR_OTHER:
			eap->line1 = 1;
			eap->line2 = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_LOADED_BUFFERS:
			{
			    buf_T	*buf = firstbuf;

			    ಯಾವತ್ (buf->b_next != ರಿಕ್ತ
						  && buf->b_ml.ml_mfp == ರಿಕ್ತ)
				buf = buf->b_next;
			    eap->line1 = buf->b_fnum;
			    buf = lastbuf;
			    ಯಾವತ್ (buf->b_prev != ರಿಕ್ತ
						  && buf->b_ml.ml_mfp == ರಿಕ್ತ)
				buf = buf->b_prev;
			    eap->line2 = buf->b_fnum;
			    ಭಙ್ಗ;
			}
		    ಪಕ್ಷ ADDR_BUFFERS:
			eap->line1 = firstbuf->b_fnum;
			eap->line2 = lastbuf->b_fnum;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_WINDOWS:
		    ಪಕ್ಷ ADDR_TABS:
			ಯದಿ (IS_USER_CMDIDX(eap->cmdidx))
			{
			    eap->line1 = 1;
			    eap->line2 = eap->addr_type == ADDR_WINDOWS
						  ? LAST_WIN_NR : LAST_TAB_NR;
			}
			ಅನ್ಯಥಾ
			{
			    // there is no Vim command which uses '%' and
			    // ADDR_WINDOWS or ADDR_TABS
			    *errormsg = _(e_invalid_range);
			    ಗಚ್ಛ theend;
			}
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_TABS_RELATIVE:
		    ಪಕ್ಷ ADDR_UNSIGNED:
		    ಪಕ್ಷ ADDR_QUICKFIX:
			*errormsg = _(e_invalid_range);
			ಗಚ್ಛ theend;
		    ಪಕ್ಷ ADDR_ARGUMENTS:
			ಯದಿ (ARGCOUNT == 0)
			    eap->line1 = eap->line2 = 0;
			ಅನ್ಯಥಾ
			{
			    eap->line1 = 1;
			    eap->line2 = ARGCOUNT;
			}
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			eap->line1 = 1;
			eap->line2 = qf_get_valid_size(eap);
			ಯದಿ (eap->line2 == 0)
			    eap->line2 = 1;
#endif
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_NONE:
			// Will give an error later if a range is found.
			ಭಙ್ಗ;
		}
		++eap->addr_ಎಣಿಕೆ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*eap->cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == ರಿಕ್ತ)
	    {
		pos_T	    *fp;

		// '*' - visual area
		ಯದಿ (eap->addr_type != ADDR_ಸಾಲುಗಳು)
		{
		    *errormsg = _(e_invalid_range);
		    ಗಚ್ಛ theend;
		}

		++eap->cmd;
		ಯದಿ (!eap->skip)
		{
		    fp = getmark('<', FALSE);
		    ಯದಿ (check_mark(fp) == FAIL)
			ಗಚ್ಛ theend;
		    eap->line1 = fp->lnum;
		    fp = getmark('>', FALSE);
		    ಯದಿ (check_mark(fp) == FAIL)
			ಗಚ್ಛ theend;
		    eap->line2 = fp->lnum;
		    ++eap->addr_ಎಣಿಕೆ;
		}
	    }
	}
	ಅನ್ಯಥಾ
	    eap->line2 = lnum;
	eap->addr_ಎಣಿಕೆ++;

	ಯದಿ (*eap->cmd == ';')
	{
	    ಯದಿ (!eap->skip)
	    {
		curwin->w_cursor.lnum = eap->line2;

		// Don't leave the cursor on an illegal line or column, but do
		// accept zero as address, so 0;/PATTERN/ works correctly
		// (where zero usually means to use the first line).
		// Check the cursor position before returning.
		ಯದಿ (eap->line2 > 0)
		    check_cursor();
		ಅನ್ಯಥಾ
		    check_cursor_col();
		need_check_cursor = TRUE;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (*eap->cmd != ',')
	    ಭಙ್ಗ;
	++eap->cmd;
    }

    // One address given: set start and end lines.
    ಯದಿ (eap->addr_ಎಣಿಕೆ == 1)
    {
	eap->line1 = eap->line2;
	// ... but only implicit: really no address given
	ಯದಿ (lnum == MAXLNUM)
	    eap->addr_ಎಣಿಕೆ = 0;
    }
    ret = OK;

theend:
    ಯದಿ (need_check_cursor)
	check_cursor();
    ಪ್ರತ್ಯಾಗಮ ret;
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

    ಯದಿ (len > IOSIZE - 100)
    {
	// Not enough space, truncate and put in "...".
	d = IObuff + IOSIZE - 100;
	d -= mb_head_off(IObuff, d);
	STRCPY(d, "...");
    }
    STRCAT(IObuff, ": ");
    d = IObuff + STRLEN(IObuff);
    ಯಾವತ್ (*s != NUL && d - IObuff + 5 < IOSIZE)
    {
	ಯದಿ (enc_utf8 ? (s[0] == 0xc2 && s[1] == 0xa0) : *s == 0xa0)
	{
	    s += enc_utf8 ? 2 : 1;
	    STRCPY(d, "<a0>");
	    d += 4;
	}
	ಅನ್ಯಥಾ ಯದಿ (d - IObuff + (*mb_ptr2len)(s) + 1 >= IOSIZE)
	    ಭಙ್ಗ;
	ಅನ್ಯಥಾ
	    MB_COPY_CHAR(s, d);
    }
    *d = NUL;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * If "start" points "&opt", "&l:opt", "&g:opt" or "$ENV" return a pointer to
 * the name.  Otherwise just return "start".
 */
    char_u *
skip_option_env_lead(char_u *start)
{
    char_u *name = start;

    ಯದಿ (*start == '&')
    {
	ಯದಿ ((start[1] == 'l' || start[1] == 'g') && start[2] == ':')
	    name += 3;
	ಅನ್ಯಥಾ
	    name += 1;
    }
    ಅನ್ಯಥಾ ಯದಿ (*start == '$')
	name += 1;
    ಪ್ರತ್ಯಾಗಮ name;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
one_letter_cmd(char_u *p, cmdidx_T *idx)
{
    ಯದಿ (in_vim9script())
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (p[0] == 'k'
	    && (p[1] != 'e' || (p[1] == 'e' && p[2] != 'e')))
    {
	*idx = CMD_k;
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಯದಿ (p[0] == 's'
	    && ((p[1] == 'c' && (p[2] == NUL || (p[2] != 's' && p[2] != 'r'
			&& (p[3] == NUL || (p[3] != 'i' && p[4] != 'p')))))
		|| p[1] == 'g'
		|| (p[1] == 'i' && p[2] != 'm' && p[2] != 'l' && p[2] != 'g')
		|| p[1] == 'I'
		|| (p[1] == 'r' && p[2] != 'e')))
    {
	*idx = CMD_substitute;
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Return TRUE if "cmd" starts with "123->", a number followed by a method
 * call.
 */
    ಪೂರ್ಣಾಙ್ಕ
ಸಂಖ್ಯೆ_method(char_u *cmd)
{
    char_u *p = skipdigits(cmd);

    ಪ್ರತ್ಯಾಗಮ p > cmd && (p = skipwhite(p))[0] == '-' && p[1] == '>';
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
	ಪೂರ್ಣಾಙ್ಕ	*full UNUSED,
	ಪೂರ್ಣಾಙ್ಕ	(*lookup)(char_u *, size_t, ಪೂರ್ಣಾಙ್ಕ cmd, cctx_T *) UNUSED,
	cctx_T	*cctx UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;
#ifndef FEAT_EVAL
    ಪೂರ್ಣಾಙ್ಕ		vim9 = FALSE;
#ಅನ್ಯಥಾ
    ಪೂರ್ಣಾಙ್ಕ		vim9 = in_vim9script();

    /*
     * Recognize a Vim9 script function/method call and assignment:
     * "lvar = value", "lvar(arg)", "[1, 2 3]->Func()"
     */
    p = eap->cmd;
    ಯದಿ (lookup != ರಿಕ್ತ)
    {
	char_u *pskip = skip_option_env_lead(eap->cmd);

	ಯದಿ (vim_strchr((char_u *)"{('[\"@&$", *p) != ರಿಕ್ತ
	       || ((p = to_name_const_end(pskip)) > eap->cmd && *p != NUL)
	       || (p[0] == '0' && p[1] == 'z'))
	{
	    ಪೂರ್ಣಾಙ್ಕ	    oplen;
	    ಪೂರ್ಣಾಙ್ಕ	    heredoc;
	    char_u  *swp;

	    ಯದಿ (*eap->cmd == '&'
		    || (eap->cmd[0] == '$'
				  && eap->cmd[1] != '\'' && eap->cmd[1] != '"')
		    || (eap->cmd[0] == '@'
					&& (valid_yank_reg(eap->cmd[1], FALSE)
						       || eap->cmd[1] == '@')))
	    {
		ಯದಿ (*eap->cmd == '&')
		{
		    p = eap->cmd + 1;
		    ಯದಿ (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
			p += 2;
		    p = to_name_end(p, FALSE);
		}
		ಅನ್ಯಥಾ ಯದಿ (*eap->cmd == '$')
		    p = to_name_end(eap->cmd + 1, FALSE);
		ಅನ್ಯಥಾ
		    p = eap->cmd + 2;
		ಯದಿ (ends_excmd(*skipwhite(p)))
		{
		    // "&option <NL>", "$ENV <NL>" and "@r <NL>" are the start
		    // of an expression.
		    eap->cmdidx = CMD_eval;
		    ಪ್ರತ್ಯಾಗಮ eap->cmd;
		}
		// "&option" can be followed by "->" or "=", check below
	    }

	    ಯದಿ (vim9 && *p == '<')
	    {
		// generic function type args
		ಯದಿ (skip_generic_func_type_args(&p) == FAIL)
		{
		    eap->cmdidx = CMD_SIZE;
		    ಪ್ರತ್ಯಾಗಮ p;
		}
	    }

	    swp = skipwhite(p);

	    ಯದಿ (
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
		ಯದಿ (*eap->cmd == '{' && ends_excmd(*skipwhite(eap->cmd + 1)))
		{
		    // "{" by itself is the start of a block.
		    eap->cmdidx = CMD_block;
		    ಪ್ರತ್ಯಾಗಮ eap->cmd + 1;
		}
		eap->cmdidx = CMD_eval;
		ಪ್ರತ್ಯಾಗಮ eap->cmd;
	    }

	    ಯದಿ ((p != eap->cmd && (
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
		ಯದಿ (skip_ಅಭಿವ್ಯಕ್ತಿ(&after, ರಿಕ್ತ) == OK)
		{
		    after = skipwhite(after);
		    ಯದಿ (*after == '=' || (*after != NUL && after[1] == '=')
					 || (after[0] == '.' && after[1] == '.'
							   && after[2] == '='))
			eap->cmdidx = CMD_var;
		}
		--emsg_off;
		ಪ್ರತ್ಯಾಗಮ eap->cmd;
	    }

	    // "[...]->Method()" is a list expression, but "[a, b] = Func()" is
	    // an assignment.
	    // If there is no line break inside the "[...]" then "p" is
	    // advanced to after the "]" by to_name_const_end(): check if a "="
	    // follows.
	    // If "[...]" has a line break "p" still points at the "[" and it
	    // can't be an assignment.
	    ಯದಿ (*eap->cmd == '[')
	    {
		char_u	    *eq;

		p = to_name_const_end(eap->cmd);
		ಯದಿ (p == eap->cmd && *p == '[')
		{
		    ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = 0;
		    ಪೂರ್ಣಾಙ್ಕ	semicolon = FALSE;

		    p = skip_var_list(eap->cmd, TRUE, &ಎಣಿಕೆ, &semicolon, TRUE);
		}
		eq = p;
		ಯದಿ (eq != ರಿಕ್ತ)
		{
		    eq = skipwhite(eq);
		    ಯದಿ (vim_strchr((char_u *)"+-*/%.", *eq) != ರಿಕ್ತ)
		    {
			ಯದಿ (eq[0] == '.' && eq[1] == '.')
			    ++eq;
			++eq;
		    }
		}
		ಯದಿ (p == ರಿಕ್ತ || p == eap->cmd || *eq != '=')
		{
		    eap->cmdidx = CMD_eval;
		    ಪ್ರತ್ಯಾಗಮ eap->cmd;
		}
		ಯದಿ (p > eap->cmd && *eq == '=')
		{
		    eap->cmdidx = CMD_var;
		    ಪ್ರತ್ಯಾಗಮ eap->cmd;
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
	    ಯದಿ (oplen > 0)
	    {
		ಯದಿ (((p - eap->cmd) > 2 && eap->cmd[1] == ':')
			|| *eap->cmd == '&'
			|| *eap->cmd == '$'
			|| *eap->cmd == '@'
			|| (eap->skip && IS_WHITE_OR_NUL(
						      *(skipwhite(p) + oplen)))
			|| lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK)
		{
		    eap->cmdidx = CMD_var;
		    ಪ್ರತ್ಯಾಗಮ eap->cmd;
		}
	    }

	    // Recognize trying to use a type for a w:, b:, t: or g: variable:
	    // "w:varname: number = 123".
	    ಯದಿ (eap->cmd[1] == ':' && *p == ':')
	    {
		eap->cmdidx = CMD_var;
		ಪ್ರತ್ಯಾಗಮ eap->cmd;
	    }
	}

	// 1234->func() is a method call
	ಯದಿ (ಸಂಖ್ಯೆ_method(eap->cmd))
	{
	    eap->cmdidx = CMD_eval;
	    ಪ್ರತ್ಯಾಗಮ eap->cmd;
	}

	// "g:", "s:" and "l:" are always assumed to be a variable, thus start
	// an expression.  A global/substitute/list command needs to use a
	// longer name.
	ಯದಿ (vim_strchr((char_u *)"gsl", *p) != ರಿಕ್ತ && p[1] == ':')
	{
	    eap->cmdidx = CMD_eval;
	    ಪ್ರತ್ಯಾಗಮ eap->cmd;
	}

	// If it is an ID it might be a variable with an operator on the next
	// line, if the variable exists it can't be an Ex command.
	ಯದಿ (p > eap->cmd && ends_excmd(*skipwhite(p))
		&& (lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK
		    || (ASCII_ISALPHA(eap->cmd[0]) && eap->cmd[1] == ':')))
	{
	    eap->cmdidx = CMD_eval;
	    ಪ್ರತ್ಯಾಗಮ eap->cmd;
	}

	// Check for "++nr" and "--nr".
	ಯದಿ (p == eap->cmd && p[0] != NUL && p[0] == p[1]
						   && (*p == '+' || *p == '-'))
	{
	    eap->cmdidx = *p == '+' ? CMD_increment : CMD_decrement;
	    ಪ್ರತ್ಯಾಗಮ eap->cmd + 2;
	}
    }
#endif

    /*
     * Isolate the command and search for it in the command table.
     */
    p = eap->cmd;
    ಯದಿ (one_letter_cmd(p, &eap->cmdidx))
    {
	++p;
	ಯದಿ (full != ರಿಕ್ತ)
	    *full = TRUE;
    }
    ಅನ್ಯಥಾ
    {
	ಯಾವತ್ (ASCII_ISALPHA(*p))
	    ++p;
	// for python 3.x support ":py3", ":python3", ":py3file", etc.
	ಯದಿ (eap->cmd[0] == 'p' && eap->cmd[1] == 'y')
	{
	    ಯಾವತ್ (ASCII_ISALNUM(*p))
		++p;
	}
	ಅನ್ಯಥಾ ಯದಿ (*p == '9' && STRNCMP("vim9", eap->cmd, 4) == 0)
	{
	    // include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	    ++p;
	    ಯಾವತ್ (ASCII_ISALPHA(*p))
		++p;
	}

	// check for non-alpha command
	ಯದಿ (p == eap->cmd && vim_strchr((char_u *)"@*!=><&~#}", *p) != ರಿಕ್ತ)
	    ++p;
	len = (ಪೂರ್ಣಾಙ್ಕ)(p - eap->cmd);
	// The "d" command can directly be followed by 'l' or 'p' flag, when
	// not in Vim9 script.
	ಯದಿ (!vim9 && *eap->cmd == 'd' && (p[-1] == 'l' || p[-1] == 'p'))
	{
	    // Check for ":dl", ":dell", etc. to ":deletel": that's
	    // :delete with the 'l' flag.  Same for 'p'.
	    ಪ್ರತಿ (i = 0; i < len; ++i)
		ಯದಿ (eap->cmd[i] != ((char_u *)"delete")[i])
		    ಭಙ್ಗ;
	    ಯದಿ (i == len - 1)
	    {
		--len;
		ಯದಿ (p[-1] == 'l')
		    eap->flags |= EXFLAG_LIST;
		ಅನ್ಯಥಾ
		    eap->flags |= EXFLAG_PRINT;
	    }
	}

	ಯದಿ (ASCII_ISLOWER(eap->cmd[0]))
	{
	    ಪೂರ್ಣಾಙ್ಕ c1 = eap->cmd[0];
	    ಪೂರ್ಣಾಙ್ಕ c2 = len == 1 ? NUL : eap->cmd[1];

	    ಯದಿ (command_ಎಣಿಕೆ != (ಪೂರ್ಣಾಙ್ಕ)CMD_SIZE)
	    {
		iemsg(e_command_table_needs_to_be_updated_run_make_cmdidxs);
		getout(1);
	    }

	    // Use a precomputed index for fast look-up in cmdnames[]
	    // taking into account the first 2 letters of eap->cmd.
	    eap->cmdidx = cmdidxs1[Char_Ord_ಕೆಳಗಿನ(c1)];
	    ಯದಿ (ASCII_ISLOWER(c2))
		eap->cmdidx += cmdidxs2[Char_Ord_ಕೆಳಗಿನ(c1)][Char_Ord_ಕೆಳಗಿನ(c2)];
	}
	ಅನ್ಯಥಾ ಯದಿ (ASCII_ISUPPER(eap->cmd[0]))
	    eap->cmdidx = CMD_Next;
	ಅನ್ಯಥಾ
	    eap->cmdidx = CMD_bang;

	ಪ್ರತಿ ( ; (ಪೂರ್ಣಾಙ್ಕ)eap->cmdidx < (ಪೂರ್ಣಾಙ್ಕ)CMD_SIZE;
			       eap->cmdidx = (cmdidx_T)((ಪೂರ್ಣಾಙ್ಕ)eap->cmdidx + 1))
	    ಯದಿ (STRNCMP(cmdnames[(ಪೂರ್ಣಾಙ್ಕ)eap->cmdidx].cmd_name, (ವರ್ಣ *)eap->cmd,
							    (size_t)len) == 0)
	    {
#ifdef FEAT_EVAL
		ಯದಿ (full != ರಿಕ್ತ && cmdnames[eap->cmdidx].cmd_name[len] == NUL)
		    *full = TRUE;
#endif
		ಭಙ್ಗ;
	    }

	// :Print and :mode are not supported in Vim9 script.
	// Some commands cannot be shortened in Vim9 script.
	ಯದಿ (vim9 && eap->cmdidx != CMD_SIZE)
	{
	    ಯದಿ (eap->cmdidx == CMD_mode || eap->cmdidx == CMD_Print)
		eap->cmdidx = CMD_SIZE;
	    ಅನ್ಯಥಾ ಯದಿ ((cmdnames[eap->cmdidx].cmd_argt & EX_WHOLE)
			  && len < (ಪೂರ್ಣಾಙ್ಕ)cmdnames[eap->cmdidx].cmd_namelen)
	    {
		semsg(_(e_command_cannot_be_shortened_str), eap->cmd);
		eap->cmdidx = CMD_SIZE;
	    }
	}

	// Do not recognize ":*" as the star command unless '*' is in
	// 'cpoptions'.
	ಯದಿ (eap->cmdidx == CMD_star && vim_strchr(p_cpo, CPO_STAR) == ರಿಕ್ತ)
	    p = eap->cmd;

	// Look for a user defined command as a last resort.  Let ":Print" be
	// overruled by a user defined command.
	ಯದಿ ((eap->cmdidx == CMD_SIZE || eap->cmdidx == CMD_Print)
		&& *eap->cmd >= 'A' && *eap->cmd <= 'Z')
	{
	    // User defined commands may contain digits.
	    ಯಾವತ್ (ASCII_ISALNUM(*p))
		++p;
	    p = find_ucmd(eap, p, full, ರಿಕ್ತ, ರಿಕ್ತ);
	}
	ಯದಿ (p == ರಿಕ್ತ || p == eap->cmd)
	    eap->cmdidx = CMD_SIZE;
    }

    // ":fina" means ":finally" in legacy script, for backwards compatibility.
    ಯದಿ (eap->cmdidx == CMD_final && p - eap->cmd == 4 && !vim9)
	eap->cmdidx = CMD_finally;

     // Force ":ho" to be unresolved.  Without this, find_ex_command()
     // matches it to CMD_horizontal (the only "ho*" entry), which makes
     // fullcommand("ho") return "horizontal" even though ":ho" cannot be
     // used as the modifier (cmdmods[] requires 3 chars, "hor").
    ಯದಿ (eap->cmdidx == CMD_horizontal && p - eap->cmd == 2)
	eap->cmdidx = CMD_SIZE;

#ifdef FEAT_EVAL
    ಯದಿ (eap->cmdidx < CMD_SIZE
	    && vim9
	    && !IS_WHITE_NL_OR_NUL(*p) && *p != '!' && *p != '|'
	    && (eap->cmdidx < 0 ||
		(cmdnames[eap->cmdidx].cmd_argt & EX_NONWHITE_OK) == 0))
    {
	// A name that goes on with an underscore is not this command with an
	// argument: "ch_log" is not ":change".  Leave it to be reported as an
	// invalid command.
	ಯದಿ (*p != '_')
	{
	    char_u *cmd = vim_strnsave(eap->cmd, p - eap->cmd);

	    semsg(_(e_command_str_not_followed_by_white_space_str), cmd,
								    eap->cmd);
	    vim_free(cmd);
	}
	eap->cmdidx = CMD_SIZE;
    }
#endif

    ಪ್ರತ್ಯಾಗಮ p;
}

#ಯದಿ defined(FEAT_EVAL)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ವರ್ಣ	*name;
    ಪೂರ್ಣಾಙ್ಕ		minlen;
    ಪೂರ್ಣಾಙ್ಕ		has_ಎಣಿಕೆ;  // :123verbose  :3tab
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
    ಪೂರ್ಣಾಙ್ಕ
modifier_len(char_u *cmd)
{
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    char_u	*p = cmd;

    ಯದಿ (VIM_ISDIGIT(*cmd))
	p = skipwhite(skipdigits(cmd + 1));
    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	// cmdmod_info_tab[] is sorted by name: once the first letter is past
	// the command's first letter no later entry can match.
	ಯದಿ (cmdmod_info_tab[i].name[0] > *p)
	    ಭಙ್ಗ;
	ಪ್ರತಿ (j = 0; p[j] != NUL; ++j)
	    ಯದಿ (p[j] != cmdmod_info_tab[i].name[j])
		ಭಙ್ಗ;
	ಯದಿ (!ASCII_ISALPHA(p[j]) && j >= cmdmod_info_tab[i].minlen
					&& (p == cmd || cmdmod_info_tab[i].has_ಎಣಿಕೆ))
	    ಪ್ರತ್ಯಾಗಮ j + (ಪೂರ್ಣಾಙ್ಕ)(p - cmd);
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Return > 0 if an Ex command "name" exists.
 * Return 2 if there is an exact match.
 * Return 3 if there is an ambiguous match.
 */
    ಪೂರ್ಣಾಙ್ಕ
cmd_exists(char_u *name)
{
    exarg_T	ea;
    ಪೂರ್ಣಾಙ್ಕ		full = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		j;
    char_u	*p;

    // Check command modifiers.
    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	ಪ್ರತಿ (j = 0; name[j] != NUL; ++j)
	    ಯದಿ (name[j] != cmdmod_info_tab[i].name[j])
		ಭಙ್ಗ;
	ಯದಿ (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	    ಪ್ರತ್ಯಾಗಮ (cmdmod_info_tab[i].name[j] == NUL ? 2 : 1);
    }

    // Check built-in commands and user defined commands.
    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    p = find_ex_command(&ea, &full, ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 3;
    ಯದಿ (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಯದಿ (*skipwhite(p) != NUL)
	ಪ್ರತ್ಯಾಗಮ 0;	// trailing garbage
    ಪ್ರತ್ಯಾಗಮ (ea.cmdidx == CMD_SIZE ? 0 : (full ? 2 : 1));
}

/*
 * Add the attributes of an Ex command to "d" in the terms of ":command":
 * "argt" are the EX_ flags, "addr_type" the address type and "def" the
 * default for a range or count, -1 when there is none.
 */
    ಶೂನ್ಯ
ex_command_attrs(dict_T *d, long_u argt, cmd_addr_T addr_type, long def)
{
    ಸ್ಥಿರ ವರ್ಣ *addr_names[] = {
	"lines", "windows", "arguments", "loaded_buffers", "buffers", "tabs",
	"tabs_relative", "quickfix_valid", "quickfix", "unsigned", "other",
	"none"
    };
    ವರ್ಣ	*nargs;

    ಯದಿ (!(argt & EX_EXTRA))
	nargs = "0";
    ಅನ್ಯಥಾ ಯದಿ (!(argt & EX_NOSPC))
	nargs = (argt & EX_NEEDARG) ? "+" : "*";
    ಅನ್ಯಥಾ ಯದಿ (!(argt & EX_NEEDARG))
	nargs = "?";
    ಅನ್ಯಥಾ
	nargs = (argt & EX_ARGSPACE) ? "_" : "1";
    dict_add_string(d, "nargs", (char_u *)nargs);

    ಯದಿ (!(argt & EX_RANGE))
	dict_add_bool(d, "range", FALSE);
    ಅನ್ಯಥಾ ಯದಿ (argt & EX_DFLALL)
	dict_add_string(d, "range", (char_u *)"%");
    ಅನ್ಯಥಾ ಯದಿ (def >= 0 && !(argt & EX_ಎಣಿಕೆ))
	dict_add_ಸಂಖ್ಯೆ(d, "range", def);
    ಅನ್ಯಥಾ
	dict_add_string(d, "range", (char_u *)".");
    ಯದಿ (!(argt & EX_ಎಣಿಕೆ))
	dict_add_bool(d, "count", FALSE);
    ಅನ್ಯಥಾ
	dict_add_ಸಂಖ್ಯೆ(d, "count", def < 0 ? 0 : def);
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
    ಪೂರ್ಣಾಙ್ಕ
ex_command_info(char_u *name, ಪೂರ್ಣಾಙ್ಕ vim9, dict_T *d)
{
    exarg_T	ea;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		j;
    ಪೂರ್ಣಾಙ್ಕ		save_cmod_flags = cmdmod.cmod_flags;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;

    ಯದಿ (vim9 != -1)
    {
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9 ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    // An abbreviation of a command modifier, like ":bo" for ":botright".
    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	ಪ್ರತಿ (j = 0; name[j] != NUL; ++j)
	    ಯದಿ (name[j] != cmdmod_info_tab[i].name[j])
		ಭಙ್ಗ;
	ಯದಿ (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	{
	    name = (char_u *)cmdmod_info_tab[i].name;
	    ಭಙ್ಗ;
	}
    }

    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    ea.addr_ಎಣಿಕೆ = 0;
    // Don't complain about using "en" or ":let" in Vim9 script.
    ++emsg_silent;
    p = find_ex_command(&ea, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ (p == ರಿಕ್ತ || ea.cmdidx == CMD_SIZE
	    || (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	    || *skipwhite(p) != NUL
	    || not_in_vim9(&ea, true) == FAIL)
	ret = FAIL;
    --emsg_silent;

    ಯದಿ (ret == FAIL)
	;
    ಅನ್ಯಥಾ ಯದಿ (IS_USER_CMDIDX(ea.cmdidx))
	ret = user_command_info(ea.useridx, ea.cmdidx, d);
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	notimpl = cmdnames[ea.cmdidx].cmd_func == ex_ni
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
    ಪ್ರತ್ಯಾಗಮ ret;
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
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();
    ಪೂರ್ಣಾಙ್ಕ		save_cmod_flags = cmdmod.cmod_flags;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    name = tv_get_string(&argvars[0]);
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	vim9script = tv_get_bool(&argvars[1]);
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9script ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    ಯಾವತ್ (*name == ':')
	name++;
    name = skip_range(name, TRUE, ರಿಕ್ತ);

    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.addr_ಎಣಿಕೆ = 0;
    ++emsg_silent;  // don't complain about using "en" in Vim9 script
    p = find_ex_command(&ea, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
    --emsg_silent;
    ಯದಿ (p == ರಿಕ್ತ || ea.cmdidx == CMD_SIZE)
	ಗಚ್ಛ theend;

    ಯದಿ (vim9script && not_in_vim9(&ea, true) == FAIL)
	ಗಚ್ಛ theend;

    rettv->vval.v_string = vim_strsave(IS_USER_CMDIDX(ea.cmdidx)
				 ? get_user_command_name(ea.useridx, ea.cmdidx)
				 : cmdnames[ea.cmdidx].cmd_name);
theend:
    cmdmod.cmod_flags = save_cmod_flags;
}
#endif

    cmdidx_T
excmd_get_cmdidx(char_u *cmd, ಪೂರ್ಣಾಙ್ಕ len)
{
    cmdidx_T idx;

    ಯದಿ (!one_letter_cmd(cmd, &idx))
	ಪ್ರತಿ (idx = (cmdidx_T)0; (ಪೂರ್ಣಾಙ್ಕ)idx < (ಪೂರ್ಣಾಙ್ಕ)CMD_SIZE;
		idx = (cmdidx_T)((ಪೂರ್ಣಾಙ್ಕ)idx + 1))
	    ಯದಿ (STRNCMP(cmdnames[(ಪೂರ್ಣಾಙ್ಕ)idx].cmd_name, cmd, (size_t)len) == 0)
		ಭಙ್ಗ;

    ಪ್ರತ್ಯಾಗಮ idx;
}

    long
excmd_get_argt(cmdidx_T idx)
{
    ಪ್ರತ್ಯಾಗಮ (long)cmdnames[(ಪೂರ್ಣಾಙ್ಕ)idx].cmd_argt;
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
    ಪೂರ್ಣಾಙ್ಕ		skip_star,	// skip "*" used for Visual range
    ಪೂರ್ಣಾಙ್ಕ		*ctx)		// pointer to xp_context or NULL
{
    char_u	*cmd = cmd_start;
    unsigned	delim;

    ಯಾವತ್ (vim_strchr((char_u *)" \t0123456789.$%'/?-+,;\\", *cmd) != ರಿಕ್ತ)
    {
	ಯದಿ (*cmd == '\\')
	{
	    ಯದಿ (cmd[1] == '?' || cmd[1] == '/' || cmd[1] == '&')
		++cmd;
	    ಅನ್ಯಥಾ
		ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (*cmd == '\'')
	{
	    char_u *p = cmd;

	    // a quote is only valid at the start or after a separator
	    ಯಾವತ್ (p > cmd_start)
	    {
		--p;
		ಯದಿ (!VIM_ISWHITE(*p))
		    ಭಙ್ಗ;
	    }
	    ಯದಿ (cmd > cmd_start && !VIM_ISWHITE(*p) && *p != ',' && *p != ';')
		ಭಙ್ಗ;
	    ಯದಿ (*++cmd == NUL && ctx != ರಿಕ್ತ)
		*ctx = EXPAND_NOTHING;
	}
	ಅನ್ಯಥಾ ಯದಿ (*cmd == '/' || *cmd == '?')
	{
	    delim = *cmd++;
	    ಯಾವತ್ (*cmd != NUL && *cmd != delim)
		ಯದಿ (*cmd++ == '\\' && *cmd != NUL)
		    ++cmd;
	    ಯದಿ (*cmd == NUL && ctx != ರಿಕ್ತ)
		*ctx = EXPAND_NOTHING;
	}
	ಯದಿ (*cmd != NUL)
	    ++cmd;
    }

    // Skip ":" and white space.
    ಯಾವತ್ (*cmd == ':')
	cmd = skipwhite(cmd + 1);

    // Skip "*" used for Visual range.
    ಯದಿ (skip_star && *cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == ರಿಕ್ತ)
	cmd = skipwhite(cmd + 1);

    ಪ್ರತ್ಯಾಗಮ cmd;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
addr_error(cmd_addr_T addr_type)
{
    ಯದಿ (addr_type == ADDR_NONE)
	emsg(_(e_no_range_allowed));
    ಅನ್ಯಥಾ
	emsg(_(e_invalid_range));
}

/*
 * Return the default address for an address type.
 */
    ಸ್ಥಿರ linenr_T
default_address(exarg_T *eap)
{
    linenr_T lnum = 0;

    ವಿಕಲ್ಪ (eap->addr_type)
    {
	ಪಕ್ಷ ADDR_ಸಾಲುಗಳು:
	ಪಕ್ಷ ADDR_OTHER:
	    // Default is the cursor line number.  Avoid using an invalid
	    // line number though.
	    ಯದಿ (curwin->w_cursor.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಅನ್ಯಥಾ
		lnum = curwin->w_cursor.lnum;
	    ಭಙ್ಗ;
	ಪಕ್ಷ ADDR_WINDOWS:
	    lnum = CURRENT_WIN_NR;
	    ಭಙ್ಗ;
	ಪಕ್ಷ ADDR_ARGUMENTS:
	    lnum = curwin->w_arg_idx + 1;
	    ಯದಿ (lnum > ARGCOUNT)
		lnum = ARGCOUNT;
	    ಭಙ್ಗ;
	ಪಕ್ಷ ADDR_LOADED_BUFFERS:
	ಪಕ್ಷ ADDR_BUFFERS:
	    lnum = curbuf->b_fnum;
	    ಭಙ್ಗ;
	ಪಕ್ಷ ADDR_TABS:
	    lnum = CURRENT_TAB_NR;
	    ಭಙ್ಗ;
	ಪಕ್ಷ ADDR_TABS_RELATIVE:
	ಪಕ್ಷ ADDR_UNSIGNED:
	    lnum = 1;
	    ಭಙ್ಗ;
	ಪಕ್ಷ ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_idx(eap);
#endif
	    ಭಙ್ಗ;
	ಪಕ್ಷ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_valid_idx(eap);
#endif
	    ಭಙ್ಗ;
	ಪಕ್ಷ ADDR_NONE:
	    // Will give an error later if a range is found.
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ lnum;
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
    ಪೂರ್ಣಾಙ್ಕ		skip,		// only skip the address, don't use it
    ಪೂರ್ಣಾಙ್ಕ		silent,		// no errors or side effects
    ಪೂರ್ಣಾಙ್ಕ		to_other_file,  // flag: may jump to other file
    ಪೂರ್ಣಾಙ್ಕ		address_ಎಣಿಕೆ UNUSED) // 1 for first address, >1 after comma
{
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		i;
    long	n;
    char_u	*cmd;
    pos_T	pos;
    pos_T	*fp;
    linenr_T	lnum;
    buf_T	*buf;

    cmd = skipwhite(*ptr);
    lnum = MAXLNUM;
    ಕುರು
    {
	ವಿಕಲ್ಪ (*cmd)
	{
	    ಪಕ್ಷ '.':			    // '.' - Cursor position
		++cmd;
		ವಿಕಲ್ಪ (addr_type)
		{
		    ಪಕ್ಷ ADDR_ಸಾಲುಗಳು:
		    ಪಕ್ಷ ADDR_OTHER:
			lnum = curwin->w_cursor.lnum;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_LOADED_BUFFERS:
		    ಪಕ್ಷ ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_NONE:
		    ಪಕ್ಷ ADDR_TABS_RELATIVE:
		    ಪಕ್ಷ ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = ರಿಕ್ತ;
			ಗಚ್ಛ error;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			ಭಙ್ಗ;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ '$':			    // '$' - last line
		++cmd;
		ವಿಕಲ್ಪ (addr_type)
		{
		    ಪಕ್ಷ ADDR_ಸಾಲುಗಳು:
		    ಪಕ್ಷ ADDR_OTHER:
			lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_WINDOWS:
			lnum = LAST_WIN_NR;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_ARGUMENTS:
			lnum = ARGCOUNT;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_LOADED_BUFFERS:
			buf = lastbuf;
			ಯಾವತ್ (buf->b_ml.ml_mfp == ರಿಕ್ತ)
			{
			    ಯದಿ (buf->b_prev == ರಿಕ್ತ)
				ಭಙ್ಗ;
			    buf = buf->b_prev;
			}
			lnum = buf->b_fnum;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_BUFFERS:
			lnum = lastbuf->b_fnum;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_TABS:
			lnum = LAST_TAB_NR;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_NONE:
		    ಪಕ್ಷ ADDR_TABS_RELATIVE:
		    ಪಕ್ಷ ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = ರಿಕ್ತ;
			ಗಚ್ಛ error;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_size(eap);
			ಯದಿ (lnum == 0)
			    lnum = 1;
#endif
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_valid_size(eap);
			ಯದಿ (lnum == 0)
			    lnum = 1;
#endif
			ಭಙ್ಗ;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ '\'':			    // ''' - mark
		ಯದಿ (*++cmd == NUL)
		{
		    cmd = ರಿಕ್ತ;
		    ಗಚ್ಛ error;
		}
		ಯದಿ (addr_type != ADDR_ಸಾಲುಗಳು)
		{
		    addr_error(addr_type);
		    cmd = ರಿಕ್ತ;
		    ಗಚ್ಛ error;
		}
		ಯದಿ (skip)
		    ++cmd;
		ಅನ್ಯಥಾ
		{
		    // Only accept a mark in another file when it is
		    // used by itself: ":'M".
		    fp = getmark(*cmd, to_other_file && cmd[1] == NUL);
		    ++cmd;
		    ಯದಿ (fp == (pos_T *)-1)
			// Jumped to another file.
			lnum = curwin->w_cursor.lnum;
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (check_mark(fp) == FAIL)
			{
			    cmd = ರಿಕ್ತ;
			    ಗಚ್ಛ error;
			}
			lnum = fp->lnum;
		    }
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ '/':
	    ಪಕ್ಷ '?':			// '/' or '?' - search
		c = *cmd++;
		ಯದಿ (addr_type != ADDR_ಸಾಲುಗಳು)
		{
		    addr_error(addr_type);
		    cmd = ರಿಕ್ತ;
		    ಗಚ್ಛ error;
		}
		ಯದಿ (skip)	// skip "/pat/"
		{
		    cmd = skip_regexp(cmd, c, magic_isset());
		    ಯದಿ (*cmd == c)
			++cmd;
		}
		ಅನ್ಯಥಾ
		{
		    ಪೂರ್ಣಾಙ್ಕ flags;

		    pos = curwin->w_cursor; // save curwin->w_cursor

		    // When '/' or '?' follows another address, start from
		    // there.
		    ಯದಿ (lnum > 0 && lnum != MAXLNUM)
			curwin->w_cursor.lnum =
				lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
					   ? curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ : lnum;

		    // Start a forward search at the end of the line (unless
		    // before the first line).
		    // Start a backward search at the start of the line.
		    // This makes sure we never match in the current
		    // line, and can match anywhere in the
		    // next/previous line.
		    ಯದಿ (c == '/' && curwin->w_cursor.lnum > 0)
			curwin->w_cursor.col = MAXCOL;
		    ಅನ್ಯಥಾ
			curwin->w_cursor.col = 0;
		    searchcmdlen = 0;
		    flags = silent ? ಹುಡುಕು_KEEP : ಹುಡುಕು_HIS | ಹುಡುಕು_ಸಂದೇಶ;
		    ಯದಿ (!do_ಹುಡುಕು(ರಿಕ್ತ, c, c, cmd, STRLEN(cmd), 1L, flags, ರಿಕ್ತ))
		    {
			curwin->w_cursor = pos;
			cmd = ರಿಕ್ತ;
			ಗಚ್ಛ error;
		    }
		    lnum = curwin->w_cursor.lnum;
		    curwin->w_cursor = pos;
		    // adjust command string pointer
		    cmd += searchcmdlen;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ '\\':		    // "\?", "\/" or "\&", repeat search
		++cmd;
		ಯದಿ (addr_type != ADDR_ಸಾಲುಗಳು)
		{
		    addr_error(addr_type);
		    cmd = ರಿಕ್ತ;
		    ಗಚ್ಛ error;
		}
		ಯದಿ (*cmd == '&')
		    i = RE_SUBST;
		ಅನ್ಯಥಾ ಯದಿ (*cmd == '?' || *cmd == '/')
		    i = RE_ಹುಡುಕು;
		ಅನ್ಯಥಾ
		{
		    emsg(_(e_backslash_should_be_followed_by));
		    cmd = ರಿಕ್ತ;
		    ಗಚ್ಛ error;
		}

		ಯದಿ (!skip)
		{
		    /*
		     * When search follows another address, start from
		     * there.
		     */
		    ಯದಿ (lnum != MAXLNUM)
			pos.lnum = lnum;
		    ಅನ್ಯಥಾ
			pos.lnum = curwin->w_cursor.lnum;

		    /*
		     * Start the search just like for the above
		     * do_search().
		     */
		    ಯದಿ (*cmd != '?')
			pos.col = MAXCOL;
		    ಅನ್ಯಥಾ
			pos.col = 0;
		    pos.coladd = 0;
		    ಯದಿ (searchit(curwin, curbuf, &pos, ರಿಕ್ತ,
				*cmd == '?' ? BACKWARD : FORWARD,
				(char_u *)"", 0, 1L, ಹುಡುಕು_ಸಂದೇಶ, i, ರಿಕ್ತ) != FAIL)
			lnum = pos.lnum;
		    ಅನ್ಯಥಾ
		    {
			cmd = ರಿಕ್ತ;
			ಗಚ್ಛ error;
		    }
		}
		++cmd;
		ಭಙ್ಗ;

	    ಸ್ವಭಾವ:
		ಯದಿ (VIM_ISDIGIT(*cmd))	// absolute line number
		    lnum = getdigits(&cmd);
	}

	ಪ್ರತಿ (;;)
	{
	    cmd = skipwhite(cmd);
	    ಯದಿ (*cmd != '-' && *cmd != '+' && !VIM_ISDIGIT(*cmd))
		ಭಙ್ಗ;

	    ಯದಿ (lnum == MAXLNUM)
	    {
		ವಿಕಲ್ಪ (addr_type)
		{
		    ಪಕ್ಷ ADDR_ಸಾಲುಗಳು:
		    ಪಕ್ಷ ADDR_OTHER:
			// "+1" is same as ".+1"
			lnum = curwin->w_cursor.lnum;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_LOADED_BUFFERS:
		    ಪಕ್ಷ ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_TABS_RELATIVE:
			lnum = 1;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			ಭಙ್ಗ;
		    ಪಕ್ಷ ADDR_NONE:
		    ಪಕ್ಷ ADDR_UNSIGNED:
			lnum = 0;
			ಭಙ್ಗ;
		}
	    }

	    ಯದಿ (VIM_ISDIGIT(*cmd))
		i = '+';		// "number" is same as "+number"
	    ಅನ್ಯಥಾ
		i = *cmd++;
	    ಯದಿ (!VIM_ISDIGIT(*cmd))	// '+' is '+1'
		n = 1;
	    ಅನ್ಯಥಾ
	    {
		// "number", "+number" or "-number"
		n = getdigits(&cmd);
		ಯದಿ (n == MAXLNUM)
		{
		    emsg(_(e_ಸಾಲು_ಸಂಖ್ಯೆ_out_of_range));
		    cmd = ರಿಕ್ತ;
		    ಗಚ್ಛ error;
		}
	    }

	    ಯದಿ (addr_type == ADDR_TABS_RELATIVE)
	    {
		emsg(_(e_invalid_range));
		cmd = ರಿಕ್ತ;
		ಗಚ್ಛ error;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (addr_type == ADDR_LOADED_BUFFERS
		    || addr_type == ADDR_BUFFERS)
		lnum = compute_buffer_local_ಎಣಿಕೆ(
				    addr_type, lnum, (i == '-') ? -1 * n : n);
	    ಅನ್ಯಥಾ
	    {
#ifdef FEAT_FOLDING
		// Relative line addressing: need to adjust for lines in a
		// closed fold after the first address.
		ಯದಿ (addr_type == ADDR_ಸಾಲುಗಳು && (i == '-' || i == '+')
							 && address_ಎಣಿಕೆ >= 2)
		    (ಶೂನ್ಯ)hasFolding(lnum, ರಿಕ್ತ, &lnum);
#endif
		ಯದಿ (i == '-')
		    lnum -= n;
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (lnum >= 0 && n >= LONG_MAX - lnum)
		    {
			emsg(_(e_ಸಾಲು_ಸಂಖ್ಯೆ_out_of_range));
			cmd = ರಿಕ್ತ;
			ಗಚ್ಛ error;
		    }
		    lnum += n;
		}
	    }
	}
    } ಯಾವತ್ (*cmd == '/' || *cmd == '?');

error:
    *ptr = cmd;
    ಪ್ರತ್ಯಾಗಮ lnum;
}

/*
 * Set eap->line1 and eap->line2 to the whole range.
 * Used for commands with the EX_DFLALL flag and no range given.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
address_default_all(exarg_T *eap)
{
    eap->line1 = 1;
    ವಿಕಲ್ಪ (eap->addr_type)
    {
	ಪಕ್ಷ ADDR_ಸಾಲುಗಳು:
	ಪಕ್ಷ ADDR_OTHER:
	    eap->line2 = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಭಙ್ಗ;
	ಪಕ್ಷ ADDR_LOADED_BUFFERS:
	    {
		buf_T *buf = firstbuf;

		ಯಾವತ್ (buf->b_next != ರಿಕ್ತ && buf->b_ml.ml_mfp == ರಿಕ್ತ)
		    buf = buf->b_next;
		eap->line1 = buf->b_fnum;
		buf = lastbuf;
		ಯಾವತ್ (buf->b_prev != ರಿಕ್ತ && buf->b_ml.ml_mfp == ರಿಕ್ತ)
		    buf = buf->b_prev;
		eap->line2 = buf->b_fnum;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ ADDR_BUFFERS:
	    eap->line1 = firstbuf->b_fnum;
	    eap->line2 = lastbuf->b_fnum;
	    ಭಙ್ಗ;
	ಪಕ್ಷ ADDR_WINDOWS:
	    eap->line2 = LAST_WIN_NR;
	    ಭಙ್ಗ;
	ಪಕ್ಷ ADDR_TABS:
	    eap->line2 = LAST_TAB_NR;
	    ಭಙ್ಗ;
	ಪಕ್ಷ ADDR_TABS_RELATIVE:
	    eap->line2 = 1;
	    ಭಙ್ಗ;
	ಪಕ್ಷ ADDR_ARGUMENTS:
	    ಯದಿ (ARGCOUNT == 0)
		eap->line1 = eap->line2 = 0;
	    ಅನ್ಯಥಾ
		eap->line2 = ARGCOUNT;
	    ಭಙ್ಗ;
	ಪಕ್ಷ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    eap->line2 = qf_get_valid_size(eap);
	    ಯದಿ (eap->line2 == 0)
		eap->line2 = 1;
#endif
	    ಭಙ್ಗ;
	ಪಕ್ಷ ADDR_NONE:
	ಪಕ್ಷ ADDR_UNSIGNED:
	ಪಕ್ಷ ADDR_QUICKFIX:
	    iemsg("Cannot use EX_DFLALL with ADDR_NONE, ADDR_UNSIGNED or ADDR_QUICKFIX");
	    ಭಙ್ಗ;
    }
}


/*
 * Get flags from an Ex command argument.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_flags(exarg_T *eap)
{
    ಯಾವತ್ (vim_strchr((char_u *)"lp#", *eap->arg) != ರಿಕ್ತ)
    {
	ಯದಿ (*eap->arg == 'l')
	    eap->flags |= EXFLAG_LIST;
	ಅನ್ಯಥಾ ಯದಿ (*eap->arg == 'p')
	    eap->flags |= EXFLAG_PRINT;
	ಅನ್ಯಥಾ
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
    ಯದಿ (!eap->skip)
	eap->errmsg =
		_(e_sorry_command_is_not_available_in_this_version);
}

#ifdef HAVE_EX_SCRIPT_NI
/*
 * Function called for script command which is Not Implemented.  NI!
 * Skips over ":perl <<EOF" constructs.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_script_ni(exarg_T *eap)
{
    ಯದಿ (!eap->skip)
	ex_ni(eap);
    ಅನ್ಯಥಾ
	vim_free(script_get(eap, eap->arg));
}
#endif

/*
 * Check range in Ex command for validity.
 * Return NULL when valid, error message when invalid.
 */
    ಸ್ಥಿರ ವರ್ಣ *
invalid_range(exarg_T *eap)
{
    buf_T	*buf;

    ಯದಿ (       eap->line1 < 0
	    || eap->line2 < 0
	    || eap->line1 > eap->line2)
	ಪ್ರತ್ಯಾಗಮ _(e_invalid_range);

    ಯದಿ (eap->argt & EX_RANGE)
    {
	ವಿಕಲ್ಪ (eap->addr_type)
	{
	    ಪಕ್ಷ ADDR_ಸಾಲುಗಳು:
		ಯದಿ (eap->line2 > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
#ifdef FEAT_DIFF
			    + (eap->cmdidx == CMD_diffget ||
				eap->cmdidx == CMD_diffput)
#endif
		   )
		    ಪ್ರತ್ಯಾಗಮ _(e_invalid_range);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ADDR_ARGUMENTS:
		// add 1 if ARGCOUNT is 0
		ಯದಿ (eap->line2 > ARGCOUNT + (!ARGCOUNT))
		    ಪ್ರತ್ಯಾಗಮ _(e_invalid_range);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ADDR_BUFFERS:
		// Only a boundary check, not whether the buffers actually
		// exist.
		ಯದಿ (eap->line1 < 1 || eap->line2 > get_highest_fnum())
		    ಪ್ರತ್ಯಾಗಮ _(e_invalid_range);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ADDR_LOADED_BUFFERS:
		buf = firstbuf;
		ಯಾವತ್ (buf->b_ml.ml_mfp == ರಿಕ್ತ)
		{
		    ಯದಿ (buf->b_next == ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ _(e_invalid_range);
		    buf = buf->b_next;
		}
		ಯದಿ (eap->line1 < buf->b_fnum)
		    ಪ್ರತ್ಯಾಗಮ _(e_invalid_range);
		buf = lastbuf;
		ಯಾವತ್ (buf->b_ml.ml_mfp == ರಿಕ್ತ)
		{
		    ಯದಿ (buf->b_prev == ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ _(e_invalid_range);
		    buf = buf->b_prev;
		}
		ಯದಿ (eap->line2 > buf->b_fnum)
		    ಪ್ರತ್ಯಾಗಮ _(e_invalid_range);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ADDR_WINDOWS:
		ಯದಿ (eap->line2 > LAST_WIN_NR)
		    ಪ್ರತ್ಯಾಗಮ _(e_invalid_range);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ADDR_TABS:
		ಯದಿ (eap->line2 > LAST_TAB_NR)
		    ಪ್ರತ್ಯಾಗಮ _(e_invalid_range);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ADDR_TABS_RELATIVE:
	    ಪಕ್ಷ ADDR_OTHER:
		// Any range is OK.
		ಭಙ್ಗ;
	    ಪಕ್ಷ ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
		// No error for value that is too big, will use the last entry.
		ಯದಿ (eap->line2 <= 0)
		{
		    ಯದಿ (eap->addr_ಎಣಿಕೆ == 0)
			ಪ್ರತ್ಯಾಗಮ _(e_no_errors);
		    ಪ್ರತ್ಯಾಗಮ _(e_invalid_range);
		}
#endif
		ಭಙ್ಗ;
	    ಪಕ್ಷ ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
		ಯದಿ ((eap->line2 != 1 && eap->line2 > qf_get_valid_size(eap))
			|| eap->line2 < 0)
		    ಪ್ರತ್ಯಾಗಮ _(e_invalid_range);
#endif
		ಭಙ್ಗ;
	    ಪಕ್ಷ ADDR_UNSIGNED:
	    ಪಕ್ಷ ADDR_NONE:
		// Will give an error elsewhere.
		ಭಙ್ಗ;
	}
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Correct the range for zero line number, if required.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
correct_range(exarg_T *eap)
{
    ಯದಿ (!(eap->argt & EX_ZEROR))	    // zero in range not allowed
    {
	ಯದಿ (eap->line1 == 0)
	    eap->line1 = 1;
	ಯದಿ (eap->line2 == 0)
	    eap->line2 = 1;
    }
}

#ifdef FEAT_QUICKFIX
/*
 * For a ":vimgrep" or ":vimgrepadd" command return a pointer past the
 * pattern.  Otherwise return eap->arg.
 */
    ಸ್ಥಿರ char_u *
skip_grep_pat(exarg_T *eap)
{
    char_u	*p = eap->arg;

    ಯದಿ (*p != NUL && (eap->cmdidx == CMD_vimgrep || eap->cmdidx == CMD_lvimgrep
		|| eap->cmdidx == CMD_vimgrepadd
		|| eap->cmdidx == CMD_lvimgrepadd
		|| grep_internal(eap->cmdidx)))
    {
	p = skip_vimgrep_pat(p, ರಿಕ್ತ, ರಿಕ್ತ);
	ಯದಿ (p == ರಿಕ್ತ)
	    p = eap->arg;
    }
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * For the ":make" and ":grep" commands insert the 'makeprg'/'grepprg' option
 * in the command line, so that things like % get expanded.
 */
    ಸ್ಥಿರ char_u *
replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep)
{
    char_u	*new_cmdline;
    char_u	*program;
    char_u	*pos;
    char_u	*ptr;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		i;

    /*
     * Don't do it when ":vimgrep" is used for ":grep".
     */
    ಯದಿ ((eap->cmdidx == CMD_make || eap->cmdidx == CMD_lmake
		     || eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
		     || eap->cmdidx == CMD_grepadd
		     || eap->cmdidx == CMD_lgrepadd)
	    && !grep_internal(eap->cmdidx))
    {
	ಯದಿ (eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
	    || eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	{
	    ಯದಿ (*curbuf->b_p_gp == NUL)
		program = p_gp;
	    ಅನ್ಯಥಾ
		program = curbuf->b_p_gp;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (*curbuf->b_p_mp == NUL)
		program = p_mp;
	    ಅನ್ಯಥಾ
		program = curbuf->b_p_mp;
	}

	p = skipwhite(p);

	ಯದಿ ((pos = (char_u *)strstr((ವರ್ಣ *)program, "$*")) != ರಿಕ್ತ)
	{
	    // replace $* by given arguments
	    i = 1;
	    ಯಾವತ್ ((pos = (char_u *)strstr((ವರ್ಣ *)pos + 2, "$*")) != ರಿಕ್ತ)
		++i;
	    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);
	    new_cmdline = alloc(STRLEN(program) + (size_t)i * (len - 2) + 1);
	    ಯದಿ (new_cmdline == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;			// out of memory
	    ptr = new_cmdline;
	    ಯಾವತ್ ((pos = (char_u *)strstr((ವರ್ಣ *)program, "$*")) != ರಿಕ್ತ)
	    {
		i = (ಪೂರ್ಣಾಙ್ಕ)(pos - program);
		STRNCPY(ptr, program, i);
		STRCPY(ptr += i, p);
		ptr += len;
		program = pos + 2;
	    }
	    STRCPY(ptr, program);
	}
	ಅನ್ಯಥಾ
	{
	    size_t program_len = STRLEN(program);

	    new_cmdline = alloc(program_len + STRLEN(p) + 2);
	    ಯದಿ (new_cmdline == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;			// out of memory
	    STRCPY(new_cmdline, program);
	    STRCPY(new_cmdline + program_len, " ");
	    STRCPY(new_cmdline + program_len + 1, p);
	}
	ಸಂದೇಶ_make(p);

	// 'eap->cmd' is not set here, because it is not used at CMD_make
	vim_free(*cmdlinep);
	*cmdlinep = new_cmdline;
	p = new_cmdline;
    }
    ಪ್ರತ್ಯಾಗಮ p;
}
#endif

/*
 * Expand file name in Ex command argument.
 * When an error is detected, "errormsgp" is set to a non-NULL pointer.
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
expand_filename(
    exarg_T	*eap,
    char_u	**cmdlinep,
    ವರ್ಣ	**errormsgp)
{
    ಪೂರ್ಣಾಙ್ಕ		has_wildcards;	// need to expand wildcards
    char_u	*repl;
    size_t	srclen;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		escaped;

#ifdef FEAT_QUICKFIX
    // Skip a regexp pattern for ":vimgrep[add] pat file..."
    p = skip_grep_pat(eap);
#ಅನ್ಯಥಾ
    p = eap->arg;
#endif

    /*
     * Decide to expand wildcards *before* replacing '%', '#', etc.  If
     * the file name contains a wildcard it should not cause expanding.
     * (it will be expanded anyway if there is a wildcard before replacing).
     */
    has_wildcards = mch_has_wildcard(p);
    ಯಾವತ್ (*p != NUL)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`, wildcards in it are not expanded.
	ಯದಿ (p[0] == '`' && p[1] == '=')
	{
	    p += 2;
	    (ಶೂನ್ಯ)skip_ಅಭಿವ್ಯಕ್ತಿ(&p, ರಿಕ್ತ);
	    ಯದಿ (*p == '`')
		++p;
	    ಅನುವರ್ತ;
	}
#endif
	/*
	 * Quick check if this cannot be the start of a special string.
	 * Also removes backslash before '%', '#' and '<'.
	 */
	ಯದಿ (vim_strchr((char_u *)"%#<", *p) == ರಿಕ್ತ)
	{
	    ++p;
	    ಅನುವರ್ತ;
	}

	/*
	 * Try to find a match at this position.
	 */
	repl = eval_vars(p, eap->arg, &srclen, &(eap->do_ecmd_lnum),
						    errormsgp, &escaped, TRUE);
	ಯದಿ (*errormsgp != ರಿಕ್ತ)		// error detected
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (repl == ರಿಕ್ತ)		// no match found
	{
	    p += srclen;
	    ಅನುವರ್ತ;
	}

	// Wildcards won't be expanded below, the replacement is taken
	// literally.  But do expand "~/file", "~user/file" and "$HOME/file".
	ಯದಿ (vim_strchr(repl, '$') != ರಿಕ್ತ || vim_strchr(repl, '~') != ರಿಕ್ತ)
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
	ಯದಿ (!eap->usefilter
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
	    ಸ್ಥಿರ char_u *nobslash = (char_u *)" \t\"|";
# define ESCAPE_CHARS nobslash
#ಅನ್ಯಥಾ
# define ESCAPE_CHARS escape_chars
#endif

	    ಪ್ರತಿ (l = repl; *l; ++l)
		ಯದಿ (vim_strchr(ESCAPE_CHARS, *l) != ರಿಕ್ತ)
		{
		    l = vim_strsave_escaped(repl, ESCAPE_CHARS);
		    ಯದಿ (l != ರಿಕ್ತ)
		    {
			vim_free(repl);
			repl = l;
		    }
		    ಭಙ್ಗ;
		}
	}

	// For a shell command a '!' must be escaped.
	ಯದಿ ((eap->usefilter || eap->cmdidx == CMD_bang
						|| eap->cmdidx == CMD_terminal)
			    && vim_strpbrk(repl, (char_u *)"!") != ರಿಕ್ತ)
	{
	    char_u	*l;

	    l = vim_strsave_escaped(repl, (char_u *)"!");
	    ಯದಿ (l != ರಿಕ್ತ)
	    {
		vim_free(repl);
		repl = l;
	    }
	}

	p = repl_cmdline(eap, p, srclen, repl, cmdlinep);
	vim_free(repl);
	ಯದಿ (p == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    /*
     * One file argument: Expand wildcards.
     * Don't do this with ":r !command" or ":w !command".
     */
    ಯದಿ ((eap->argt & EX_NOSPC) && !eap->usefilter)
    {
	/*
	 * May do this twice:
	 * 1. Replace environment variables.
	 * 2. Replace any other wildcards, remove backslashes.
	 */
	ಪ್ರತಿ (n = 1; n <= 2; ++n)
	{
	    ಯದಿ (n == 2)
	    {
		/*
		 * Halve the number of backslashes (this is Vi compatible).
		 * For Unix and OS/2, when wildcards are expanded, this is
		 * done by ExpandOne() below.
		 */
#ಯದಿ defined(UNIX)
		ಯದಿ (!has_wildcards)
#endif
		    backslash_halve(eap->arg);
	    }

	    ಯದಿ (has_wildcards)
	    {
		ಯದಿ (n == 1)
		{
		    /*
		     * First loop: May expand environment variables.  This
		     * can be done much faster with expand_env() than with
		     * something else (e.g., calling a shell).
		     * After expanding environment variables, check again
		     * if there are still wildcards present.
		     */
		    ಯದಿ (vim_strchr(eap->arg, '$') != ರಿಕ್ತ
			    || vim_strchr(eap->arg, '~') != ರಿಕ್ತ)
		    {
			expand_env_esc(eap->arg, NameBuff, MAXPATHL,
					(char_u *)(" \t" PATH_ESC_WILDCARDS),
					TRUE, ರಿಕ್ತ);
			has_wildcards = mch_has_wildcard(NameBuff);
			p = NameBuff;
		    }
		    ಅನ್ಯಥಾ
			p = ರಿಕ್ತ;
		}
		ಅನ್ಯಥಾ // n == 2
		{
		    expand_T	xpc;
		    ಪೂರ್ಣಾಙ್ಕ		options = WILD_LIST_NOTFOUND
					       | WILD_NOERROR | WILD_ADD_SLASH;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    ಯದಿ (p_wic)
			options += WILD_ICASE;
		    p = ExpandOne(&xpc, eap->arg, ರಿಕ್ತ,
						   options, WILD_EXPAND_FREE);
		    ಯದಿ (p == ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		ಯದಿ (p != ರಿಕ್ತ)
		{
		    (ಶೂನ್ಯ)repl_cmdline(eap, eap->arg, STRLEN(eap->arg),
								 p, cmdlinep);
		    ಯದಿ (n == 2)	// p came from ExpandOne()
			vim_free(p);
		}
	    }
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಯದಿ (eap->nextcmd != ರಿಕ್ತ)
	i += STRLEN(eap->nextcmd);	// add space for next command
    ಯದಿ ((new_cmdline = alloc(i)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;			// out of memory!

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

    ಯದಿ (eap->nextcmd != ರಿಕ್ತ)		// append next command
    {
	new_cmdlinelen += taillen + 1;
	STRCPY(new_cmdline + new_cmdlinelen, eap->nextcmd);
	eap->nextcmd = new_cmdline + new_cmdlinelen;
    }
    eap->cmd = new_cmdline + (eap->cmd - *cmdlinep);
    eap->arg = new_cmdline + (eap->arg - *cmdlinep);
    ಯದಿ (eap->do_ecmd_cmd != ರಿಕ್ತ && eap->do_ecmd_cmd != dollar_command)
	eap->do_ecmd_cmd = new_cmdline + (eap->do_ecmd_cmd - *cmdlinep);
    vim_free(*cmdlinep);
    *cmdlinep = new_cmdline;

    ಪ್ರತ್ಯಾಗಮ src;
}

/*
 * Check for '|' to separate commands and '"' to start comments.
 * If "keep_backslash" is TRUE do not remove any backslash.
 */
    ಶೂನ್ಯ
separate_nextcmd(exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ keep_backslash)
{
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();
    char_u	*p;

#ifdef FEAT_QUICKFIX
    p = skip_grep_pat(eap);
#ಅನ್ಯಥಾ
    p = eap->arg;
#endif

    ಪ್ರತಿ ( ; *p; MB_PTR_ADV(p))
    {
	ಯದಿ (*p == Ctrl_V)
	{
	    ಯದಿ ((eap->argt & (EX_CTRLV | EX_XFILE)) || keep_backslash)
		++p;		// skip CTRL-V and next char
	    ಅನ್ಯಥಾ
				// remove CTRL-V and skip next char
		STRMOVE(p, p + 1);
	    ಯದಿ (*p == NUL)		// stop at NUL after CTRL-V
		ಭಙ್ಗ;
	}

#ifdef FEAT_EVAL
	// Skip over `=expr` when wildcards are expanded.
	ಅನ್ಯಥಾ ಯದಿ (p[0] == '`' && p[1] == '=' && (eap->argt & EX_XFILE))
	{
	    p += 2;
	    (ಶೂನ್ಯ)skip_ಅಭಿವ್ಯಕ್ತಿ(&p, ರಿಕ್ತ);
	    ಯದಿ (*p == NUL)		// stop at NUL after CTRL-V
		ಭಙ್ಗ;
	}
#endif

	// Check for '"'/'#': start of comment or '|': next command
	// :@" and :*" do not start a comment!
	// :redir @" doesn't either.
	ಅನ್ಯಥಾ ಯದಿ ((*p == '"'
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
		    && eap->cmdidx != CMD_ಸೇರಿಸು)
		|| *p == '\n')
	{
	    /*
	     * We remove the '\' before the '|', unless EX_CTRLV is used
	     * AND 'b' is present in 'cpoptions'.
	     */
	    ಯದಿ ((vim_strchr(p_cpo, CPO_BAR) == ರಿಕ್ತ
			      || !(eap->argt & EX_CTRLV)) && *(p - 1) == '\\')
	    {
		ಯದಿ (!keep_backslash)
		{
		    STRMOVE(p - 1, p);	// remove the '\'
		    --p;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		set_nextcmd(eap, p);
		*p = NUL;
		ಭಙ್ಗ;
	    }
	}
    }

    ಯದಿ (!(eap->argt & EX_NOTRLCOM))	// remove trailing spaces
	del_trailing_spaces(eap->arg);
}

/*
 * get + command from ex argument
 */
    ಸ್ಥಿರ char_u *
getargcmd(char_u **argp)
{
    char_u *arg = *argp;
    char_u *command = ರಿಕ್ತ;

    ಯದಿ (*arg == '+')	    // +[command]
    {
	++arg;
	ಯದಿ (vim_isspace(*arg) || *arg == NUL)
	    command = dollar_command;
	ಅನ್ಯಥಾ
	{
	    command = arg;
	    arg = skip_cmd_arg(command, TRUE);
	    ಯದಿ (*arg != NUL)
		*arg++ = NUL;		// terminate command with NUL
	}

	arg = skipwhite(arg);	// skip over spaces
	*argp = arg;
    }
    ಪ್ರತ್ಯಾಗಮ command;
}

/*
 * Find end of "+command" argument.  Skip over "\ " and "\\".
 */
    char_u *
skip_cmd_arg(
    char_u *p,
    ಪೂರ್ಣಾಙ್ಕ	   rembs)	// TRUE to halve the number of backslashes
{
    ಯಾವತ್ (*p && !vim_isspace(*p))
    {
	ಯದಿ (*p == '\\' && p[1] != NUL)
	{
	    ಯದಿ (rembs)
		STRMOVE(p, p + 1);
	    ಅನ್ಯಥಾ
		++p;
	}
	MB_PTR_ADV(p);
    }
    ಪ್ರತ್ಯಾಗಮ p;
}

    ಪೂರ್ಣಾಙ್ಕ
get_bad_opt(char_u *p, exarg_T *eap)
{
    ಯದಿ (STRICMP(p, "keep") == 0)
	eap->bad_char = BAD_KEEP;
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(p, "drop") == 0)
	eap->bad_char = BAD_DROP;
    ಅನ್ಯಥಾ ಯದಿ (MB_BYTE2LEN(*p) == 1 && p[1] == NUL)
	eap->bad_char = *p;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of bad= names.
 */
    ಸ್ಥಿರ char_u *
get_bad_name(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    // Note: Keep this in sync with getargopt.
    ಸ್ಥಿರ ವರ್ಣ *(p_bad_ಮೌಲ್ಯಗಳು[]) =
    {
	"?",
	"keep",
	"drop",
    };

    ಯದಿ (idx < 0 || idx >= (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(p_bad_ಮೌಲ್ಯಗಳು))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ (char_u*)p_bad_ಮೌಲ್ಯಗಳು[idx];
}

/*
 * Get "++opt=arg" argument.
 * Return FAIL or OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
getargopt(exarg_T *eap)
{
    char_u	*arg = eap->arg + 2;
    ಪೂರ್ಣಾಙ್ಕ		*pp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		bad_char_idx;
    char_u	*p;

    // Note: Keep this in sync with get_argopt_name.

    // ":edit ++[no]bin[ary] file"
    ಯದಿ (STRNCMP(arg, "bin", 3) == 0 || STRNCMP(arg, "nobin", 5) == 0)
    {
	ಯದಿ (*arg == 'n')
	{
	    arg += 2;
	    eap->force_bin = FORCE_NOBIN;
	}
	ಅನ್ಯಥಾ
	    eap->force_bin = FORCE_BIN;
	ಯದಿ (!checkforcmd(&arg, "binary", 3))
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	eap->arg = skipwhite(arg);
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    // ":read ++edit file"
    ಯದಿ (STRNCMP(arg, "edit", 4) == 0)
    {
	eap->read_edit = TRUE;
	eap->arg = skipwhite(arg + 4);
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    ಯದಿ (STRNCMP(arg, "ff", 2) == 0)
    {
	arg += 2;
	pp = &eap->force_ff;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "fileformat", 10) == 0)
    {
	arg += 10;
	pp = &eap->force_ff;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "enc", 3) == 0)
    {
	ಯದಿ (STRNCMP(arg, "encoding", 8) == 0)
	    arg += 8;
	ಅನ್ಯಥಾ
	    arg += 3;
	pp = &eap->force_enc;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(arg, "bad", 3) == 0)
    {
	arg += 3;
	pp = &bad_char_idx;
    }

    ಯದಿ (pp == ರಿಕ್ತ || *arg != '=')
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ++arg;
    *pp = (ಪೂರ್ಣಾಙ್ಕ)(arg - eap->cmd);
    arg = skip_cmd_arg(arg, FALSE);
    eap->arg = skipwhite(arg);
    *arg = NUL;

    ಯದಿ (pp == &eap->force_ff)
    {
	ಯದಿ (check_ff_ಮೌಲ್ಯ(eap->cmd + eap->force_ff) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	eap->force_ff = eap->cmd[eap->force_ff];
    }
    ಅನ್ಯಥಾ ಯದಿ (pp == &eap->force_enc)
    {
	// Make 'fileencoding' lower case.
	ಪ್ರತಿ (p = eap->cmd + eap->force_enc; *p != NUL; ++p)
	    *p = TOLOWER_ASC(*p);
    }
    ಅನ್ಯಥಾ
    {
	// Check ++bad= argument.  Must be a single-byte character, "keep" or
	// "drop".
	ಯದಿ (get_bad_opt(eap->cmd + bad_char_idx, eap) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of ++opt names.
 */
    ಸ್ಥಿರ char_u *
get_argopt_name(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    // Note: Keep this in sync with getargopt.
    ಸ್ಥಿರ ವರ್ಣ *(p_opt_ಮೌಲ್ಯಗಳು[]) =
    {
	"fileformat=",
	"encoding=",
	"binary",
	"nobinary",
	"bad=",
	"edit",
    };

    ಯದಿ (idx < 0 || idx >= (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(p_opt_ಮೌಲ್ಯಗಳು))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ (char_u*)p_opt_ಮೌಲ್ಯಗಳು[idx];
}

/*
 * Command-line expansion for ++opt=name.
 */
    ಪೂರ್ಣಾಙ್ಕ
expand_argopt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	ಪೂರ್ಣಾಙ್ಕ	    *ಅಂಕೆ_Matches)
{
    ಯದಿ (xp->xp_pattern > xp->xp_ಸಾಲು && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, ಪೂರ್ಣಾಙ್ಕ) = ರಿಕ್ತ;

	char_u *name_end = xp->xp_pattern - 1;
	ಯದಿ (name_end - xp->xp_ಸಾಲು >= 2
		&& STRNCMP(name_end - 2, "ff", 2) == 0)
	    cb = get_fileformat_name;
	ಅನ್ಯಥಾ ಯದಿ (name_end - xp->xp_ಸಾಲು >= 10
		&& STRNCMP(name_end - 10, "fileformat", 10) == 0)
	    cb = get_fileformat_name;
	ಅನ್ಯಥಾ ಯದಿ (name_end - xp->xp_ಸಾಲು >= 3
		&& STRNCMP(name_end - 3, "enc", 3) == 0)
	    cb = get_encoding_name;
	ಅನ್ಯಥಾ ಯದಿ (name_end - xp->xp_ಸಾಲು >= 8
		&& STRNCMP(name_end - 8, "encoding", 8) == 0)
	    cb = get_encoding_name;
	ಅನ್ಯಥಾ ಯದಿ (name_end - xp->xp_ಸಾಲು >= 3
		&& STRNCMP(name_end - 3, "bad", 3) == 0)
	    cb = get_bad_name;

	ಯದಿ (cb != ರಿಕ್ತ)
	{
	    ಪ್ರತ್ಯಾಗಮ ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    ಅಂಕೆ_Matches,
		    cb,
		    FALSE);
	}
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Special handling of "ff" which acts as a short form of
    // "fileformat", as "ff" is not a substring of it.
    ಯದಿ (xp->xp_pattern_len == 2
	    && STRNCMP(xp->xp_pattern, "ff", xp->xp_pattern_len) == 0)
    {
	*matches = ALLOC_MULT(char_u *, 1);
	ಯದಿ (*matches == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	*ಅಂಕೆ_Matches = 1;
	(*matches)[0] = vim_strsave((char_u*)"fileformat=");
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    ಪ್ರತ್ಯಾಗಮ ExpandGeneric(
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
    ಯದಿ (eap->skip)
    {
	skip_cmd_block(eap);
	ಪ್ರತ್ಯಾಗಮ;
    }

    /*
     * Disallow autocommands from .exrc and .vimrc in current
     * directory for security reasons.
     */
    ಯದಿ (secure)
    {
	secure = 2;
	eap->errmsg =
	      _(e_command_not_allowed_from_vimrc_in_current_dir_or_tag_ಹುಡುಕು);
    }
    ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_autocmd)
	do_autocmd(eap, eap->arg, eap->forceit);
    ಅನ್ಯಥಾ
	do_augroup(eap->arg, eap->forceit);
}

/*
 * ":doautocmd": Apply the automatic commands to the current buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_doautocmd(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಙ್ಕ		call_do_modelines = check_nomodeline(&arg);
    ಪೂರ್ಣಾಙ್ಕ		did_aucmd;

    (ಶೂನ್ಯ)do_doautocmd(arg, TRUE, &did_aucmd);
    // Only when there is no <nomodeline>.
    ಯದಿ (call_do_modelines && did_aucmd)
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
    ಯದಿ (ERROR_IF_ANY_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;
    eap->errmsg = do_bufdel(
	    eap->cmdidx == CMD_bdelete ? DOBUF_DEL
		: eap->cmdidx == CMD_bwipeout ? DOBUF_WIPE
		: DOBUF_UNLOAD, eap->arg,
	    eap->addr_ಎಣಿಕೆ, (ಪೂರ್ಣಾಙ್ಕ)eap->line1, (ಪೂರ್ಣಾಙ್ಕ)eap->line2, eap->forceit);
}

/*
 * :[N]buffer [N]	to buffer N
 * :[N]sbuffer [N]	to buffer N
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_buffer(exarg_T *eap)
{
    ಯದಿ (ERROR_IF_ANY_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;
    do_exbuffer(eap);
}

/*
 * ":buffer" command and alike.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_exbuffer(exarg_T *eap)
{
    ಯದಿ (*eap->arg)
	eap->errmsg = ex_errmsg(e_trailing_characters_str, eap->arg);
    ಅನ್ಯಥಾ
    {
	ಯದಿ (eap->addr_ಎಣಿಕೆ == 0)	// default is current buffer
	    goto_buffer(eap, DOBUF_CURRENT, FORWARD, 0);
	ಅನ್ಯಥಾ
	    goto_buffer(eap, DOBUF_FIRST, FORWARD, (ಪೂರ್ಣಾಙ್ಕ)eap->line2);
	ಯದಿ (eap->do_ecmd_cmd != ರಿಕ್ತ)
	    do_cmd_argument(eap->do_ecmd_cmd);
    }
}

/*
 * :[N]bmodified [N]	to next mod. buffer
 * :[N]sbmodified [N]	to next mod. buffer
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_bmodified(exarg_T *eap)
{
    goto_buffer(eap, DOBUF_MOD, FORWARD, (ಪೂರ್ಣಾಙ್ಕ)eap->line2);
    ಯದಿ (eap->do_ecmd_cmd != ರಿಕ್ತ)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bnext [N]	to next buffer
 * :[N]sbnext [N]	split and to next buffer
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_bnext(exarg_T *eap)
{
    ಯದಿ (ERROR_IF_ANY_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;

    goto_buffer(eap, DOBUF_CURRENT, FORWARD, (ಪೂರ್ಣಾಙ್ಕ)eap->line2);
    ಯದಿ (eap->do_ecmd_cmd != ರಿಕ್ತ)
	do_cmd_argument(eap->do_ecmd_cmd);
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
    ಯದಿ (ERROR_IF_ANY_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;

    goto_buffer(eap, DOBUF_CURRENT, BACKWARD, (ಪೂರ್ಣಾಙ್ಕ)eap->line2);
    ಯದಿ (eap->do_ecmd_cmd != ರಿಕ್ತ)
	do_cmd_argument(eap->do_ecmd_cmd);
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
    ಯದಿ (ERROR_IF_ANY_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;

    goto_buffer(eap, DOBUF_FIRST, FORWARD, 0);
    ಯದಿ (eap->do_ecmd_cmd != ರಿಕ್ತ)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :blast		to last buffer
 * :sblast		split and to last buffer
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_blast(exarg_T *eap)
{
    ಯದಿ (ERROR_IF_ANY_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;

    goto_buffer(eap, DOBUF_LAST, BACKWARD, 0);
    ಯದಿ (eap->do_ecmd_cmd != ರಿಕ್ತ)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * Check if "c" ends an Ex command.
 * In Vim9 script does not check for white space before #.
 */
    ಪೂರ್ಣಾಙ್ಕ
ends_excmd(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪೂರ್ಣಾಙ್ಕ comment_char = '"';

    ಯದಿ (in_vim9script())
	comment_char = '#';
    ಪ್ರತ್ಯಾಗಮ (c == NUL || c == '|' || c == comment_char || c == '\n');
}

/*
 * Like ends_excmd() but checks that a # in Vim9 script either has "cmd" equal
 * to "cmd_start" or has a white space character before it.
 */
    ಪೂರ್ಣಾಙ್ಕ
ends_excmd2(char_u *cmd_start UNUSED, char_u *cmd)
{
    ಪೂರ್ಣಾಙ್ಕ c = *cmd;

    ಯದಿ (c == NUL || c == '|' || c == '\n')
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ (in_vim9script())
	//  # starts a comment, #{ might be a mistake, #{{ can start a fold
	ಪ್ರತ್ಯಾಗಮ c == '#' && (cmd[1] != '{' || cmd[2] == '{')
				 && (cmd == cmd_start || VIM_ISWHITE(cmd[-1]));
    ಪ್ರತ್ಯಾಗಮ c == '"';
}

#ಯದಿ defined(FEAT_SYN_HL) || defined(FEAT_ಹುಡುಕು_EXTRA) || defined(FEAT_EVAL)
/*
 * Return the next command, after the first '|' or '\n'.
 * Return NULL if not found.
 */
    char_u *
find_nextcmd(char_u *p)
{
    ಯಾವತ್ (*p != '|' && *p != '\n')
    {
	ಯದಿ (*p == NUL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	++p;
    }
    ಪ್ರತ್ಯಾಗಮ (p + 1);
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

    ಯದಿ (*s == '|' || *s == '\n')
	ಪ್ರತ್ಯಾಗಮ (s + 1);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * If "eap->nextcmd" is not set, check for a next command at "p".
 */
    ಶೂನ್ಯ
set_nextcmd(exarg_T *eap, char_u *arg)
{
    char_u *p = skipwhite(arg);

    ಯದಿ (in_vim9script() && *p == '#')
    {
	char_u *nl = vim_strchr(p, NL);
	ಯದಿ (nl != ರಿಕ್ತ)
	    p = nl;
    }

    p = check_nextcmd(p);

    ಯದಿ (eap->nextcmd == ರಿಕ್ತ)
	eap->nextcmd = p;
    ಅನ್ಯಥಾ ಯದಿ (p != ರಿಕ್ತ)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_more(
    ಪೂರ್ಣಾಙ್ಕ message,	    // when FALSE check only, no messages
    ಪೂರ್ಣಾಙ್ಕ forceit)
{
    ಪೂರ್ಣಾಙ್ಕ	    n = ARGCOUNT - curwin->w_arg_idx - 1;

    ಯದಿ (!forceit && only_one_window()
	    && ARGCOUNT > 1 && !arg_had_last && n > 0 && quitmore == 0)
    {
	ಯದಿ (message)
	{
#ಯದಿ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ಯದಿ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						    && curbuf->b_fname != ರಿಕ್ತ)
	    {
		char_u	buff[DIALOG_ಸಂದೇಶ_SIZE];

		vim_snprintf((ವರ್ಣ *)buff, DIALOG_ಸಂದೇಶ_SIZE,
			NGETTEXT("%d more file to edit.  Quit anyway?",
			    "%d more files to edit.  Quit anyway?", n), n);
		ಯದಿ (vim_dialog_yesno(VIM_QUESTION, ರಿಕ್ತ, buff, 1) == VIM_YES)
		    ಪ್ರತ್ಯಾಗಮ OK;
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
#endif
	    semsg(NGETTEXT(e_nr_more_file_to_edit,
			   e_nr_more_files_to_edit , n), n);
	    quitmore = 2;	    // next try to quit is allowed
	}
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of command names.
 */
    char_u *
get_command_name(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx >= (ಪೂರ್ಣಾಙ್ಕ)CMD_SIZE)
	ಪ್ರತ್ಯಾಗಮ expand_user_command_name(idx);
    // the following are no real commands
    ಯದಿ (STRNCMP(cmdnames[idx].cmd_name, "{", 1) == 0 ||
	STRNCMP(cmdnames[idx].cmd_name, "}", 1) == 0)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"";
    ಪ್ರತ್ಯಾಗಮ cmdnames[idx].cmd_name;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_colorscheme(exarg_T *eap)
{
    ಯದಿ (*eap->arg == NUL)
    {
#ifdef FEAT_EVAL
	char_u *ಅಭಿವ್ಯಕ್ತಿ = vim_strsave((char_u *)"g:colors_name");
	char_u *p = ರಿಕ್ತ;

	ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ != ರಿಕ್ತ)
	{
	    ++emsg_off;
	    p = eval_to_string(ಅಭಿವ್ಯಕ್ತಿ, FALSE, FALSE);
	    --emsg_off;
	    vim_free(ಅಭಿವ್ಯಕ್ತಿ);
	}
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    ಸಂದೇಶ((ವರ್ಣ *)p);
	    vim_free(p);
	}
	ಅನ್ಯಥಾ
	    ಸಂದೇಶ("default");
#ಅನ್ಯಥಾ
	ಸಂದೇಶ(_("unknown"));
#endif
    }
    ಅನ್ಯಥಾ ಯದಿ (load_colors(eap->arg) == FAIL)
	semsg(_(e_cannot_find_color_scheme_str), eap->arg);

#ifdef FEAT_VTP
    ಅನ್ಯಥಾ ಯದಿ (has_vtp_working())
    {
	// background color change requires clear + redraw
	update_screen(UPD_CLEAR);
	redrawcmd();
    }
#endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_highlight(exarg_T *eap)
{
    ಯದಿ (*eap->arg == NUL && eap->cmd[2] == '!')
	ಸಂದೇಶ(_("Greetings, Vim user!"));
    do_highlight(eap->arg, eap->forceit, FALSE);
}


/*
 * Call this function if we thought we were going to exit, but we won't
 * (because of an error).  May need to restore the terminal mode.
 */
    ಶೂನ್ಯ
not_exiting(ಪೂರ್ಣಾಙ್ಕ save_exiting)
{
    exiting = save_exiting;
    settmode(TMODE_RAW);
}

    ಪೂರ್ಣಾಙ್ಕ
before_quit_autocmds(win_T *wp, ಪೂರ್ಣಾಙ್ಕ quit_all, ಪೂರ್ಣಾಙ್ಕ forceit)
{
    apply_autocmds(EVENT_QUITPRE, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, wp->w_buffer);

    // Bail out when autocommands closed the window.
    // Refuse to quit when the buffer in the last window is being closed (can
    // only happen in autocommands).
    ಯದಿ (!win_valid(wp)
	    || curbuf_locked()
	    || (wp->w_buffer->b_nwindows == 1 && wp->w_buffer->b_locked > 0))
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ಯದಿ (quit_all || (check_more(FALSE, forceit) == OK && only_one_window()))
    {
	apply_autocmds(EVENT_EXITPRE, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	// Refuse to quit when locked or when the window was closed or the
	// buffer in the last window is being closed (can only happen in
	// autocommands).
	ಯದಿ (!win_valid(wp) || curbuf_locked()
			  || (curbuf->b_nwindows == 1 && curbuf->b_locked > 0))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    ಪ್ರತ್ಯಾಗಮ FALSE;
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

    ಯದಿ (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	ಪ್ರತ್ಯಾಗಮ;
    }
    // Don't quit while editing the command line.
    ಯದಿ (text_locked())
    {
	text_locked_ಸಂದೇಶ();
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (eap->addr_ಎಣಿಕೆ > 0)
    {
	ಪೂರ್ಣಾಙ್ಕ	wnr = eap->line2;

	ಪ್ರತಿ (wp = firstwin; wp->w_next != ರಿಕ್ತ; wp = wp->w_next)
	    ಯದಿ (--wnr <= 0)
		ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ
	wp = curwin;

    // Refuse to quit when locked.
    ಯದಿ (curbuf_locked())
	ಪ್ರತ್ಯಾಗಮ;

    // Trigger QuitPre and maybe ExitPre
    ಯದಿ (before_quit_autocmds(wp, FALSE, eap->forceit))
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_NETBEANS_INTG
    netbeansForcedQuit = eap->forceit;
#endif

    ಪೂರ್ಣಾಙ್ಕ save_exiting = exiting;
    /*
     * If there is only one relevant window we will exit.
     */
    ಯದಿ (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;
    ಯದಿ ((!buf_hide(wp->w_buffer)
		&& check_changed(wp->w_buffer, (p_awa ? CCGD_AW : 0)
				       | (eap->forceit ? CCGD_FORCEIT : 0)
				       | CCGD_EXCMD))
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, TRUE)))
    {
	not_exiting(save_exiting);
    }
    ಅನ್ಯಥಾ
    {
	// quit last window
	// Note: only_one_window() returns true, even so a help window is
	// still open. In that case only quit, if no address has been
	// specified. Example:
	// :h|wincmd w|1q     - don't quit
	// :h|wincmd w|q      - quit
	ಯದಿ (only_one_window() && (ONE_WINDOW || eap->addr_ಎಣಿಕೆ == 0))
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_cquit(exarg_T *eap UNUSED)
{
    // this does not always pass on the exit code to the Manx compiler. why?
    getout(eap->addr_ಎಣಿಕೆ > 0 ? (ಪೂರ್ಣಾಙ್ಕ)eap->line2 : EXIT_FAILURE);
}

/*
 * Do preparations for "qall" and "wqall".
 * Returns FAIL when quitting should be aborted.
 */
    ಪೂರ್ಣಾಙ್ಕ
before_quit_all(exarg_T *eap)
{
    ಯದಿ (cmdwin_type != 0)
    {
	ಯದಿ (eap->forceit)
	    cmdwin_result = K_XF1;	// ex_window() takes care of this
	ಅನ್ಯಥಾ
	    cmdwin_result = K_XF2;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Don't quit while editing the command line.
    ಯದಿ (text_locked())
    {
	text_locked_ಸಂದೇಶ();
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (before_quit_autocmds(curwin, TRUE, eap->forceit))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * ":qall": try to quit all windows
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_quit_all(exarg_T *eap)
{
    ಯದಿ (before_quit_all(eap) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಪೂರ್ಣಾಙ್ಕ save_exiting = exiting;
    exiting = TRUE;
    ಯದಿ (eap->forceit || !check_changed_any(FALSE, FALSE))
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
    ಪೂರ್ಣಾಙ್ಕ		winnr = 0;
    ಯದಿ (cmdwin_type != 0)
	cmdwin_result = Ctrl_C;
    ಅನ್ಯಥಾ
	ಯದಿ (!text_locked() && !curbuf_locked())
	{
	    ಯದಿ (eap->addr_ಎಣಿಕೆ == 0)
		ex_win_close(eap->forceit, curwin, ರಿಕ್ತ);
	    ಅನ್ಯಥಾ
	    {
		FOR_ALL_WINDOWS(win)
		{
		    winnr++;
		    ಯದಿ (winnr == eap->line2)
			ಭಙ್ಗ;
		}
		ಯದಿ (win == ರಿಕ್ತ)
		    win = lastwin;
		ex_win_close(eap->forceit, win, ರಿಕ್ತ);
	    }
	}
}

#ifdef FEAT_QUICKFIX
/*
 * ":pclose": Close any preview window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_pclose(exarg_T *eap)
{
    win_T	*win;

    // First close any normal window.
    FOR_ALL_WINDOWS(win)
	ಯದಿ (win->w_p_pvw)
	{
	    ex_win_close(eap->forceit, win, ರಿಕ್ತ);
	    ಪ್ರತ್ಯಾಗಮ;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_win_close(
    ಪೂರ್ಣಾಙ್ಕ		forceit,
    win_T	*win,
    tabpage_T	*tp)		// NULL or the tab page "win" is in
{
    ಪೂರ್ಣಾಙ್ಕ		need_hide;
    buf_T	*buf = win->w_buffer;

    // Never close the autocommand window.
    ಯದಿ (is_aucmd_win(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (window_layout_locked(CMD_close))
	ಪ್ರತ್ಯಾಗಮ;

    need_hide = (bufIsChanged(buf) && buf->b_nwindows <= 1);
    ಯದಿ (need_hide && !buf_hide(buf) && !forceit)
    {
#ಯದಿ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ಯದಿ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    ಯದಿ (term_job_running(buf->b_term))
	    {
		ಯದಿ (term_confirm_stop(buf) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(buf);
		need_hide = FALSE;
	    }
	    ಅನ್ಯಥಾ
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(buf, FALSE);
		ಯದಿ (bufref_valid(&bufref) && bufIsChanged(buf))
		    ಪ್ರತ್ಯಾಗಮ;
		need_hide = FALSE;
	    }
	}
	ಅನ್ಯಥಾ
#endif
	{
	    no_write_message();
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // free buffer when not hiding it or when it's a scratch buffer
    ಯದಿ (tp == ರಿಕ್ತ)
	win_close(win, !need_hide && !buf_hide(buf));
    ಅನ್ಯಥಾ
	win_close_othertab(win, !need_hide && !buf_hide(buf), tp);
}

/*
 * Handle the argument for a tabpage related ex command.
 * Returns a tabpage number.
 * When an error is encountered then eap->errmsg is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_tabpage_arg(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ tab_ಸಂಖ್ಯೆ;
    ಪೂರ್ಣಾಙ್ಕ unaccept_arg0 = (eap->cmdidx == CMD_tabmove) ? 0 : 1;

    ಯದಿ (eap->arg && *eap->arg != NUL)
    {
	char_u *p = eap->arg;
	char_u *p_save;
	ಪೂರ್ಣಾಙ್ಕ    relative = 0; // argument +N/-N means: go to N places to the
			     // right/left relative to the current position.

	ಯದಿ (*p == '-')
	{
	    relative = -1;
	    p++;
	}
	ಅನ್ಯಥಾ ಯದಿ (*p == '+')
	{
	    relative = 1;
	    p++;
	}

	p_save = p;
	tab_ಸಂಖ್ಯೆ = getdigits(&p);

	ಯದಿ (relative == 0)
	{
	    ಯದಿ (STRCMP(p, "$") == 0)
		tab_ಸಂಖ್ಯೆ = LAST_TAB_NR;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(p, "#") == 0)
		ಯದಿ (valid_tabpage(lastused_tabpage))
		    tab_ಸಂಖ್ಯೆ = tabpage_index(lastused_tabpage);
		ಅನ್ಯಥಾ
		{
		    eap->errmsg = ex_errmsg(e_invalid_ಮೌಲ್ಯ_for_argument_str,
								     eap->arg);
		    tab_ಸಂಖ್ಯೆ = 0;
		    ಗಚ್ಛ theend;
		}
	    ಅನ್ಯಥಾ ಯದಿ (p == p_save || *p_save == '-' || *p != NUL
		    || tab_ಸಂಖ್ಯೆ > LAST_TAB_NR)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		ಗಚ್ಛ theend;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (*p_save == NUL)
		tab_ಸಂಖ್ಯೆ = 1;
	    ಅನ್ಯಥಾ ಯದಿ (p == p_save || *p_save == '-' || *p != NUL
		    || tab_ಸಂಖ್ಯೆ == 0)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		ಗಚ್ಛ theend;
	    }
	    tab_ಸಂಖ್ಯೆ = tab_ಸಂಖ್ಯೆ * relative + tabpage_index(curtab);
	    ಯದಿ (!unaccept_arg0 && relative == -1)
		--tab_ಸಂಖ್ಯೆ;
	}
	ಯದಿ (tab_ಸಂಖ್ಯೆ < unaccept_arg0 || tab_ಸಂಖ್ಯೆ > LAST_TAB_NR)
	    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
    }
    ಅನ್ಯಥಾ ಯದಿ (eap->addr_ಎಣಿಕೆ > 0)
    {
	ಯದಿ (unaccept_arg0 && eap->line2 == 0)
	{
	    eap->errmsg = _(e_invalid_range);
	    tab_ಸಂಖ್ಯೆ = 0;
	}
	ಅನ್ಯಥಾ
	{
	    tab_ಸಂಖ್ಯೆ = eap->line2;
	    ಯದಿ (!unaccept_arg0)
	    {
		char_u *cmdp = eap->cmd;

		ಯಾವತ್ (--cmdp > *eap->cmdlinep
			&& (VIM_ISWHITE(*cmdp) || VIM_ISDIGIT(*cmdp)))
		    ;
		ಯದಿ (*cmdp == '-')
		{
		    --tab_ಸಂಖ್ಯೆ;
		    ಯದಿ (tab_ಸಂಖ್ಯೆ < unaccept_arg0)
			eap->errmsg = _(e_invalid_range);
		}
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
	ವಿಕಲ್ಪ (eap->cmdidx)
	{
	ಪಕ್ಷ CMD_tabnext:
	    tab_ಸಂಖ್ಯೆ = tabpage_index(curtab) + 1;
	    ಯದಿ (tab_ಸಂಖ್ಯೆ > LAST_TAB_NR)
		tab_ಸಂಖ್ಯೆ = 1;
	    ಭಙ್ಗ;
	ಪಕ್ಷ CMD_tabmove:
	    tab_ಸಂಖ್ಯೆ = LAST_TAB_NR;
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    tab_ಸಂಖ್ಯೆ = tabpage_index(curtab);
	}
    }

theend:
    ಪ್ರತ್ಯಾಗಮ tab_ಸಂಖ್ಯೆ;
}

/*
 * ":tabclose": close current tab page, unless it is the last one.
 * ":tabclose N": close tab page N.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_tabclose(exarg_T *eap)
{
    tabpage_T	*tp;
    ಪೂರ್ಣಾಙ್ಕ		tab_ಸಂಖ್ಯೆ;

    ಯದಿ (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (first_tabpage->tp_next == ರಿಕ್ತ)
    {
	emsg(_(e_cannot_close_last_tab_page));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (window_layout_locked(CMD_tabclose))
	ಪ್ರತ್ಯಾಗಮ;

    tab_ಸಂಖ್ಯೆ = get_tabpage_arg(eap);
    ಯದಿ (eap->errmsg != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    tp = find_tabpage(tab_ಸಂಖ್ಯೆ);
    ಯದಿ (tp == ರಿಕ್ತ)
    {
	beep_flush();
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (tp != curtab)
    {
	tabpage_close_other(tp, eap->forceit);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಅನ್ಯಥಾ ಯದಿ (!text_locked() && !curbuf_locked())
	tabpage_close(eap->forceit);
}

/*
 * ":tabonly": close all tab pages except the current one
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_tabonly(exarg_T *eap)
{
    tabpage_T	*tp;
    ಪೂರ್ಣಾಙ್ಕ		done;
    ಪೂರ್ಣಾಙ್ಕ		tab_ಸಂಖ್ಯೆ;

    ಯದಿ (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (first_tabpage->tp_next == ರಿಕ್ತ)
    {
	ಸಂದೇಶ(_("Already only one tab page"));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (window_layout_locked(CMD_tabonly))
	ಪ್ರತ್ಯಾಗಮ;

    tab_ಸಂಖ್ಯೆ = get_tabpage_arg(eap);
    ಯದಿ (eap->errmsg != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    goto_tabpage(tab_ಸಂಖ್ಯೆ);
    // Repeat this up to a 1000 times, because autocommands may
    // mess up the lists.
    ಪ್ರತಿ (done = 0; done < 1000; ++done)
    {
	FOR_ALL_TABPAGES(tp)
	    ಯದಿ (tp->tp_topframe != topframe)
	    {
		tabpage_close_other(tp, eap->forceit);
		// if we failed to close it quit
		ಯದಿ (valid_tabpage(tp))
		    done = 1000;
		// start over, "tp" is now invalid
		ಭಙ್ಗ;
	    }
	ಯದಿ (first_tabpage->tp_next == ರಿಕ್ತ)
	    ಭಙ್ಗ;
    }
}

/*
 * Close the current tab page.
 */
    ಶೂನ್ಯ
tabpage_close(ಪೂರ್ಣಾಙ್ಕ forceit)
{
    ಯದಿ (window_layout_locked(CMD_tabclose))
	ಪ್ರತ್ಯಾಗಮ;

    trigger_tabclosedpre(curtab);
    curtab->tp_did_tabclosedpre = TRUE;
    tabpage_T *save_curtab = curtab;

    // First close all the windows but the current one.  If that worked then
    // close the last window in this tab, that will close it.
    ಯದಿ (!ONE_WINDOW)
	close_others(TRUE, forceit);
    ಯದಿ (ONE_WINDOW)
	ex_win_close(forceit, curwin, ರಿಕ್ತ);
    ಯದಿ (curtab == save_curtab)
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
    ಶೂನ್ಯ
tabpage_close_other(tabpage_T *tp, ಪೂರ್ಣಾಙ್ಕ forceit)
{
    ಪೂರ್ಣಾಙ್ಕ		done = 0;
    win_T	*wp;

    ಯದಿ (window_layout_locked(CMD_SIZE))
	ಪ್ರತ್ಯಾಗಮ;

    trigger_tabclosedpre(tp);
    tp->tp_did_tabclosedpre = TRUE;

    // Limit to 1000 windows, autocommands may add a window while we close
    // one.  OK, so I'm paranoid...
    ಯಾವತ್ (++done < 1000)
    {
	wp = tp->tp_firstwin;
	ex_win_close(forceit, wp, tp);

	// Autocommands may delete the tab page under our fingers.
	ಯದಿ (!valid_tabpage(tp))
	    ಭಙ್ಗ;
	// We may fail to close a window with a modified buffer.
	ಯದಿ (tp->tp_firstwin == wp)
	{
	    done = 1000;
	    ಭಙ್ಗ;
	}
    }
    ಯದಿ (done >= 1000)
    {
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	tp->tp_did_tabclosedpre = FALSE;
	ಪ್ರತ್ಯಾಗಮ;
    }

    apply_autocmds(EVENT_TABCLOSED, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
}

/*
 * ":only".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_only(exarg_T *eap)
{
    ಯದಿ (window_layout_locked(CMD_only))
	ಪ್ರತ್ಯಾಗಮ;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    ಯದಿ (eap->addr_ಎಣಿಕೆ > 0)
    {
	win_T   *wp;
	ಪೂರ್ಣಾಙ್ಕ	wnr = eap->line2;
	ಪ್ರತಿ (wp = firstwin; --wnr > 0; )
	{
	    ಯದಿ (wp->w_next == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    ಅನ್ಯಥಾ
		wp = wp->w_next;
	}
	win_goto(wp);
    }
    close_others(TRUE, eap->forceit);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_hide(exarg_T *eap UNUSED)
{
    // ":hide" or ":hide | cmd": hide current window
    ಯದಿ (eap->skip)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (window_layout_locked(CMD_hide))
	ಪ್ರತ್ಯಾಗಮ;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    ಯದಿ (eap->addr_ಎಣಿಕೆ == 0)
	win_close(curwin, FALSE);	// don't free buffer
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	winnr = 0;
	win_T	*win;

	FOR_ALL_WINDOWS(win)
	{
	    winnr++;
	    ಯದಿ (winnr == eap->line2)
		ಭಙ್ಗ;
	}
	ಯದಿ (win == ರಿಕ್ತ)
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
    ಯದಿ (check_restricted())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (!eap->forceit)
	autowrite_all();
    apply_autocmds(EVENT_VIMSUSPEND, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, ರಿಕ್ತ);
    windgoto((ಪೂರ್ಣಾಙ್ಕ)Rows - 1, cmdline_col_off);
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
    apply_autocmds(EVENT_VIMRESUME, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, ರಿಕ್ತ);
}

/*
 * ":exit", ":xit" and ":wq": Write file and quit the current window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_exit(exarg_T *eap)
{
#ifdef FEAT_EVAL
    ಯದಿ (not_in_vim9(eap, false) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
#endif
    ಯದಿ (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	ಪ್ರತ್ಯಾಗಮ;
    }
    // Don't quit while editing the command line.
    ಯದಿ (text_locked())
    {
	text_locked_ಸಂದೇಶ();
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಪೂರ್ಣಾಙ್ಕ save_exiting = exiting;
    /*
     * we plan to exit if there is only one relevant window
     */
    ಯದಿ (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;

    // Write the buffer for ":wq" or when it was changed.
    // Trigger QuitPre and ExitPre.
    // Check if we can exit now, after autocommands have changed things.
    ಯದಿ (((eap->cmdidx == CMD_wq || curbufIsChanged()) && do_write(eap) == FAIL)
	    || before_quit_autocmds(curwin, FALSE, eap->forceit)
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, FALSE)))
    {
	not_exiting(save_exiting);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (only_one_window())	    // quit last window, exit Vim
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_print(exarg_T *eap)
{
    ಯದಿ (curbuf->b_ml.ml_flags & ML_EMPTY)
	emsg(_(e_empty_buffer));
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ ( ;!got_int; ui_breakcheck())
	{
	    print_ಸಾಲು(eap->line1,
		    (eap->cmdidx == CMD_ಸಂಖ್ಯೆ || eap->cmdidx == CMD_pound
						 || (eap->flags & EXFLAG_NR)),
		    eap->cmdidx == CMD_list || (eap->flags & EXFLAG_LIST));
	    ಯದಿ (++eap->line1 > eap->line2)
		ಭಙ್ಗ;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_goto(exarg_T *eap)
{
    goto_byte(eap->line2);
}
#endif

/*
 * ":shell".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_shell(exarg_T *eap UNUSED)
{
    do_shell(ರಿಕ್ತ, 0);
}

#ಯದಿ defined(HAVE_DROP_FILE)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ drop_busy = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ drop_filec;
ಸ್ಥಿರ char_u **drop_filev = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ drop_split;
ಸ್ಥಿರ ಶೂನ್ಯ (*drop_callback)(ಶೂನ್ಯ *);
ಸ್ಥಿರ ಶೂನ್ಯ *drop_cookie;

    ಸ್ಥಿರ ಶೂನ್ಯ
handle_drop_internal(ಶೂನ್ಯ)
{
    exarg_T	ea;
    ಪೂರ್ಣಾಙ್ಕ		save_ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll;

    // Setting the argument list may cause screen updates and being called
    // recursively.  Avoid that by setting drop_busy.
    drop_busy = TRUE;

    // Check whether the current buffer is changed. If so, we will need
    // to split the current window or data could be lost.
    // We don't need to check if the 'hidden' option is set, as in this
    // case the buffer won't be lost.
    ಯದಿ (!buf_hide(curbuf) && !drop_split)
    {
	++emsg_off;
	drop_split = check_changed(curbuf, CCGD_AW);
	--emsg_off;
    }
    ಯದಿ (drop_split)
    {
	ಯದಿ (win_split(0, 0) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
	RESET_BINDING(curwin);

	// When splitting the window, create a new alist.  Otherwise the
	// existing one is overwritten.
	alist_unlink(curwin->w_alist);
	alist_new();
    }

    /*
     * Set up the new argument list.
     */
    alist_set(ALIST(curwin), drop_filec, drop_filev, FALSE, ರಿಕ್ತ, 0);

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
    ಸಂದೇಶ_scroll = save_ಸಂದೇಶ_scroll;

    ಯದಿ (drop_callback != ರಿಕ್ತ)
	drop_callback(drop_cookie);

    drop_filev = ರಿಕ್ತ;
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
    ಪೂರ್ಣಾಙ್ಕ		filec,		// the number of files dropped
    char_u	**filev,	// the list of files dropped
    ಪೂರ್ಣಾಙ್ಕ		split,		// force splitting the window
    ಶೂನ್ಯ	(*callback)(ಶೂನ್ಯ *), // to be called after setting the argument
				     // list
    ಶೂನ್ಯ	*cookie)	// argument for "callback" (allocated)
{
    // Cannot handle recursive drops, finish the pending one.
    ಯದಿ (drop_busy)
    {
	FreeWild(filec, filev);
	vim_free(cookie);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // When calling handle_drop() more than once in a row we only use the last
    // one.
    ಯದಿ (drop_filev != ರಿಕ್ತ)
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
    ಯದಿ (text_locked() || curbuf_locked() || updating_screen)
	ಪ್ರತ್ಯಾಗಮ;

    handle_drop_internal();
}

/*
 * To be called when text is unlocked, curbuf is unlocked or updating_screen is
 * reset: Handle a postponed drop.
 */
    ಶೂನ್ಯ
handle_any_postponed_drop(ಶೂನ್ಯ)
{
    ಯದಿ (!drop_busy && drop_filev != ರಿಕ್ತ
	     && !text_locked() && !curbuf_locked() && !updating_screen)
	handle_drop_internal();
}
#endif

/*
 * ":preserve".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_preserve(exarg_T *eap UNUSED)
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
    ಯದಿ (!check_changed(curbuf, (p_awa ? CCGD_AW : 0)
			     | CCGD_MULTWIN
			     | (eap->forceit ? CCGD_FORCEIT : 0)
			     | CCGD_EXCMD)

	    && (*eap->arg == NUL
			     || setfname(curbuf, eap->arg, ರಿಕ್ತ, TRUE) == OK))
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

#ಯದಿ defined(FEAT_EVAL)

// callback function for 'findfunc'
ಸ್ಥಿರ callback_T ffu_cb;

    ಸ್ಥಿರ callback_T *
get_findfunc_callback(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ *curbuf->b_p_ffu != NUL ? &curbuf->b_ffu_cb : &ffu_cb;
}

/*
 * Call 'findfunc' to obtain a list of file names.
 */
    ಸ್ಥಿರ list_T *
call_findfunc(char_u *pat, ಪೂರ್ಣಾಙ್ಕ cmdcomplete)
{
    typval_T	args[3];
    callback_T	*cb;
    typval_T	rettv;
    ಪೂರ್ಣಾಙ್ಕ		retval;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_BOOL;
    args[1].vval.v_ಸಂಖ್ಯೆ = cmdcomplete;
    args[2].v_type = VAR_UNKNOWN;

    // Lock the text to prevent weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;

    ctx = get_option_sctx("findfunc");
    ಯದಿ (ctx != ರಿಕ್ತ)
	current_sctx = *ctx;

    cb = get_findfunc_callback();
    retval = call_callback(cb, -1, &rettv, 2, args);

    current_sctx = saved_sctx;

    --textlock;

    list_T *retlist = ರಿಕ್ತ;

    ಯದಿ (retval == OK)
    {
	ಯದಿ (rettv.v_type == VAR_LIST)
	    retlist = list_copy(rettv.vval.v_list, FALSE, FALSE, get_copyID());
	ಅನ್ಯಥಾ
	    emsg(_(e_invalid_return_type_from_findfunc));

	clear_tv(&rettv);
    }

    ಪ್ರತ್ಯಾಗಮ retlist;
}

/*
 * Find file names matching "pat" using 'findfunc' and return it in "files".
 * Used for expanding the :find, :sfind and :tabfind command argument.
 * Returns OK on success and FAIL otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
expand_findfunc(expand_T *xp, char_u *pat, char_u ***files, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches)
{
    list_T	*l;
    ಪೂರ್ಣಾಙ್ಕ		len;

    *ಅಂಕೆ_Matches = 0;
    *files = ರಿಕ್ತ;

    l = call_findfunc(pat, VVAL_TRUE);

    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    len = list_len(l);
    ಯದಿ (len == 0)	    // empty List
    {
	list_free(l);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    expand_process_user_list(l, files, ಅಂಕೆ_Matches, xp);
    list_free(l);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Use 'findfunc' to find file 'findarg'.  The 'count' argument is used to find
 * the n'th matching file.
 */
    ಸ್ಥಿರ char_u *
findfunc_find_file(char_u *findarg, ಪೂರ್ಣಾಙ್ಕ findarg_len, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    list_T	*fname_list;
    char_u	*ret_fname = ರಿಕ್ತ;
    char_u	cc;
    ಪೂರ್ಣಾಙ್ಕ		fname_ಎಣಿಕೆ;

    cc = findarg[findarg_len];
    findarg[findarg_len] = NUL;

    fname_list = call_findfunc(findarg, VVAL_FALSE);
    fname_ಎಣಿಕೆ = list_len(fname_list);

    ಯದಿ (fname_ಎಣಿಕೆ == 0)
	semsg(_(e_cant_find_file_str_in_path), findarg);
    ಅನ್ಯಥಾ
    {
	ಯದಿ (ಎಣಿಕೆ > fname_ಎಣಿಕೆ)
	    semsg(_(e_no_more_file_str_found_in_path), findarg);
	ಅನ್ಯಥಾ
	{
	    listitem_T *li = list_find(fname_list, ಎಣಿಕೆ - 1);

	    ಯದಿ (li != ರಿಕ್ತ)
	    {
		typval_T *tv = &li->li_tv;

		ಯದಿ (tv->v_type == VAR_STRING && tv->vval.v_string != ರಿಕ್ತ)
		    ret_fname = vim_strsave(tv->vval.v_string);
		ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_DICT && tv->vval.v_dict != ರಿಕ್ತ)
		    ret_fname = dict_get_string(tv->vval.v_dict, "word", TRUE);
	    }
	}
    }

    ಯದಿ (fname_list != ರಿಕ್ತ)
	list_free(fname_list);

    findarg[findarg_len] = cc;

    ಪ್ರತ್ಯಾಗಮ ret_fname;
}

/*
 * Process the 'findfunc' option value.
 * Returns NULL on success and an error message on failure.
 */
    ವರ್ಣ *
did_set_findfunc(optset_T *args UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ	retval;

    ಯದಿ (args->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_ffu, &curbuf->b_ffu_cb);
    ಅನ್ಯಥಾ
    {
	// global option set
	retval = option_set_callback_func(p_ffu, &ffu_cb);
	// when using :set, free the local callback
	ಯದಿ (!(args->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_ffu_cb);
    }

    ಯದಿ (retval == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*name = get_scriptlocal_funcname(*varp);
    ಯದಿ (name != ರಿಕ್ತ)
    {
	free_string_option(*varp);
	*varp = name;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

# ಯದಿ defined(EXITFREE)
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
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_findfunc(ಪೂರ್ಣಾಙ್ಕ copyID UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ abort = FALSE;
    abort = set_ref_in_callback(&ffu_cb, copyID);
    ಪ್ರತ್ಯಾಗಮ abort;
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
    ಶೂನ್ಯ
ex_splitview(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    char_u	*fname = ರಿಕ್ತ;
#ifdef FEAT_BROWSE
    char_u	dot_path[] = ".";
    ಪೂರ್ಣಾಙ್ಕ		save_cmod_flags = cmdmod.cmod_flags;
#endif
    ಪೂರ್ಣಾಙ್ಕ		use_tab = eap->cmdidx == CMD_tabedit
		       || eap->cmdidx == CMD_tabfind
		       || eap->cmdidx == CMD_tabnew;

    ಯದಿ (ERROR_IF_ANY_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#ifdef FEAT_QUICKFIX
    // A ":split" in the quickfix window works like ":new".  Don't want two
    // quickfix windows.  But it's OK when doing ":tab split".
    ಯದಿ (bt_quickfix(curbuf) && cmdmod.cmod_tab == 0)
    {
	ಯದಿ (eap->cmdidx == CMD_split)
	    eap->cmdidx = CMD_new;
	ಯದಿ (eap->cmdidx == CMD_vsplit)
	    eap->cmdidx = CMD_vnew;
    }
#endif

    ಯದಿ (eap->cmdidx == CMD_sfind || eap->cmdidx == CMD_tabfind)
    {
	char_u	*file_to_find = ರಿಕ್ತ;
	ವರ್ಣ	*ಹುಡುಕು_ctx = ರಿಕ್ತ;

	ಯದಿ (*get_findfunc() != NUL)
	{
#ifdef FEAT_EVAL
	    fname = findfunc_find_file(eap->arg, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(eap->arg),
				       eap->addr_ಎಣಿಕೆ > 0 ? eap->line2 : 1);
#endif
	}
	ಅನ್ಯಥಾ
	{
	    fname = find_file_in_path(eap->arg, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(eap->arg),
					      FNAME_MESS, TRUE, curbuf->b_ffname,
					      &file_to_find, &ಹುಡುಕು_ctx);
	    vim_free(file_to_find);
	    vim_findfile_cleanup(ಹುಡುಕು_ctx);
	}
	ಯದಿ (fname == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
	eap->arg = fname;
    }
#ifdef FEAT_BROWSE
    ಅನ್ಯಥಾ ಯದಿ ((cmdmod.cmod_flags & CMOD_BROWSE)
	    && eap->cmdidx != CMD_vnew
	    && eap->cmdidx != CMD_new)
    {
	ಯದಿ (
# ifdef FEAT_GUI
	    !gui.in_use &&
# endif
		au_has_group((char_u *)"FileExplorer"))
	{
	    // No browsing supported but we do have the file explorer:
	    // Edit the directory.
	    ಯದಿ (*eap->arg == NUL || !mch_isdir(eap->arg))
		eap->arg = dot_path;
	}
	ಅನ್ಯಥಾ
	{
	    fname = do_browse(0, (char_u *)(use_tab
			? _("Edit File in new tab page")
			: _("Edit File in new window")),
					  eap->arg, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, curbuf);
	    ಯದಿ (fname == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    eap->arg = fname;
	}
    }
    cmdmod.cmod_flags &= ~CMOD_BROWSE;	// Don't browse again in do_ecmd().
#endif

    /*
     * Either open new tab page or split the window.
     */
    ಯದಿ (use_tab)
    {
	ಯದಿ (win_new_tabpage(cmdmod.cmod_tab != 0 ? cmdmod.cmod_tab
			 : eap->addr_ಎಣಿಕೆ == 0 ? 0
					       : (ಪೂರ್ಣಾಙ್ಕ)eap->line2 + 1) != FAIL)
	{
	    do_exedit(eap, old_curwin);

	    // set the alternate buffer for the window we came from
	    ಯದಿ (curwin != old_curwin
		    && win_valid(old_curwin)
		    && old_curwin->w_buffer != curbuf
		    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		old_curwin->w_alt_fnum = curbuf->b_fnum;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (win_split(eap->addr_ಎಣಿಕೆ > 0 ? (ಪೂರ್ಣಾಙ್ಕ)eap->line2 : 0,
				     *eap->cmd == 'v' ? WSP_VERT : 0) != FAIL)
    {
	// Reset 'scrollbind' when editing another file, but keep it when
	// doing ":split" without arguments.
	ಯದಿ (*eap->arg != NUL)
	    RESET_BINDING(curwin);
	ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
tabpage_new(ಶೂನ್ಯ)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_tabnext(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ tab_ಸಂಖ್ಯೆ;

    ಯದಿ (ERROR_IF_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;
    ವಿಕಲ್ಪ (eap->cmdidx)
    {
	ಪಕ್ಷ CMD_tabfirst:
	ಪಕ್ಷ CMD_tabrewind:
	    goto_tabpage(1);
	    ಭಙ್ಗ;
	ಪಕ್ಷ CMD_tablast:
	    goto_tabpage(9999);
	    ಭಙ್ಗ;
	ಪಕ್ಷ CMD_tabprevious:
	ಪಕ್ಷ CMD_tabNext:
	    ಯದಿ (eap->arg && *eap->arg != NUL)
	    {
		char_u *p = eap->arg;
		char_u *p_save = p;

		tab_ಸಂಖ್ಯೆ = getdigits(&p);
		ಯದಿ (p == p_save || *p_save == '-' || *p != NUL
			    || tab_ಸಂಖ್ಯೆ == 0)
		{
		    // No numbers as argument.
		    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		    ಪ್ರತ್ಯಾಗಮ;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (eap->addr_ಎಣಿಕೆ == 0)
		    tab_ಸಂಖ್ಯೆ = 1;
		ಅನ್ಯಥಾ
		{
		    tab_ಸಂಖ್ಯೆ = eap->line2;
		    ಯದಿ (tab_ಸಂಖ್ಯೆ < 1)
		    {
			eap->errmsg = _(e_invalid_range);
			ಪ್ರತ್ಯಾಗಮ;
		    }
		}
	    }
	    goto_tabpage(-tab_ಸಂಖ್ಯೆ);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ: // CMD_tabnext
	    tab_ಸಂಖ್ಯೆ = get_tabpage_arg(eap);
	    ಯದಿ (eap->errmsg == ರಿಕ್ತ)
		goto_tabpage(tab_ಸಂಖ್ಯೆ);
	    ಭಙ್ಗ;
    }
}

/*
 * :tabmove command
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_tabmove(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ tab_ಸಂಖ್ಯೆ;

    tab_ಸಂಖ್ಯೆ = get_tabpage_arg(eap);
    ಯದಿ (eap->errmsg == ರಿಕ್ತ)
	tabpage_move(tab_ಸಂಖ್ಯೆ);
}

/*
 * :tabs command: List tabs and their contents.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_tabs(exarg_T *eap UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		tabcount = 1;

    ಸಂದೇಶ_start();
    ಸಂದೇಶ_scroll = TRUE;
    ಪ್ರತಿ (tp = first_tabpage; tp != ರಿಕ್ತ && !got_int; tp = tp->tp_next)
    {
	ಸಂದೇಶ_putchar('\n');
	vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, _("Tab page %d"), tabcount++);
	ಸಂದೇಶ_outtrans_attr(IObuff, HL_ATTR(HLF_T));
	out_flush();	    // output one line at a time
	ui_breakcheck();

	ಯದಿ (tp  == curtab)
	    wp = firstwin;
	ಅನ್ಯಥಾ
	    wp = tp->tp_firstwin;
	ಪ್ರತಿ ( ; wp != ರಿಕ್ತ && !got_int; wp = wp->w_next)
	{
	    ಸಂದೇಶ_putchar('\n');
	    ಸಂದೇಶ_putchar(wp == curwin ? '>' : ' ');
	    ಸಂದೇಶ_putchar(' ');
	    ಸಂದೇಶ_putchar(bufIsChanged(wp->w_buffer) ? '+' : ' ');
	    ಸಂದೇಶ_putchar(' ');
	    ಯದಿ (buf_spname(wp->w_buffer) != ರಿಕ್ತ)
		vim_strncpy(IObuff, buf_spname(wp->w_buffer), IOSIZE - 1);
	    ಅನ್ಯಥಾ
		home_replace(wp->w_buffer, wp->w_buffer->b_fname,
							IObuff, IOSIZE, TRUE);
	    ಸಂದೇಶ_outtrans(IObuff);
	    out_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }
}

/*
 * ":mode": Set screen mode.
 * If no argument given, just get the screen size and redraw.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_mode(exarg_T *eap)
{
    ಯದಿ (*eap->arg == NUL)
	shell_resized();
    ಅನ್ಯಥಾ
	emsg(_(e_screen_mode_setting_not_supported));
}

/*
 * ":resize".
 * set, increment or decrement current window height
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_resize(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ		n;
    win_T	*wp = curwin;

    ಯದಿ (eap->addr_ಎಣಿಕೆ > 0)
    {
	n = eap->line2;
	ಪ್ರತಿ (wp = firstwin; wp->w_next != ರಿಕ್ತ && --n > 0; wp = wp->w_next)
	    ;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    n = atol((ವರ್ಣ *)eap->arg);
    ಯದಿ (cmdmod.cmod_split & WSP_VERT)
    {
	ಯದಿ (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_width;
	ಅನ್ಯಥಾ ಯದಿ (n == 0 && eap->arg[0] == NUL)	// default is very wide
	    n = 9999;
	win_setwidth_win(n, wp);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_height;
	ಅನ್ಯಥಾ ಯದಿ (n == 0 && eap->arg[0] == NUL)	// default is very high
	    n = 9999;
	win_setheight_win(n, wp);
    }
}

/*
 * ":find [+command] <file>" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_find(exarg_T *eap)
{
    ಯದಿ (!check_can_set_curbuf_forceit(eap->forceit))
	ಪ್ರತ್ಯಾಗಮ;

    char_u	*fname = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;
    char_u	*file_to_find = ರಿಕ್ತ;
    ವರ್ಣ	*ಹುಡುಕು_ctx = ರಿಕ್ತ;

    ಯದಿ (*get_findfunc() != NUL)
    {
#ifdef FEAT_EVAL
	fname = findfunc_find_file(eap->arg, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(eap->arg),
					eap->addr_ಎಣಿಕೆ > 0 ? eap->line2 : 1);
#endif
    }
    ಅನ್ಯಥಾ
    {
	fname = find_file_in_path(eap->arg, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(eap->arg), FNAME_MESS,
			       TRUE, curbuf->b_ffname, &file_to_find, &ಹುಡುಕು_ctx);
	ಯದಿ (eap->addr_ಎಣಿಕೆ > 0)
	{
	    // Repeat finding the file "count" times.  This matters when it appears
	    // several times in the path.
	    ಎಣಿಕೆ = eap->line2;
	    ಯಾವತ್ (fname != ರಿಕ್ತ && --ಎಣಿಕೆ > 0)
	    {
		vim_free(fname);
		fname = find_file_in_path(ರಿಕ್ತ, 0, FNAME_MESS,
			      FALSE, curbuf->b_ffname, &file_to_find, &ಹುಡುಕು_ctx);
	    }
	}
	VIM_CLEAR(file_to_find);
	vim_findfile_cleanup(ಹುಡುಕು_ctx);
    }

    ಯದಿ (fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    eap->arg = fname;
    do_exedit(eap, ರಿಕ್ತ);
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

#ifdef FEAT_EVAL
    ಯದಿ (not_in_vim9(eap, false) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
#endif
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_SOL | BL_FIX);
    ಯದಿ (*eap->arg == '/')
    {
	// ":open /pattern/": put cursor in column found with pattern
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg, magic_isset() ? RE_MAGIC : 0);
	ಯದಿ (regmatch.regprog != ರಿಕ್ತ)
	{
	    // make a copy of the line, when searching for a mark it might be
	    // flushed
	    char_u *ಸಾಲು = vim_strsave(ml_get_curline());

	    regmatch.rm_ic = p_ic;
	    ಯದಿ (vim_regexec(&regmatch, ಸಾಲು, (colnr_T)0))
		curwin->w_cursor.col = (colnr_T)(regmatch.startp[0] - ಸಾಲು);
	    ಅನ್ಯಥಾ
		emsg(_(e_no_match));
	    vim_regfree(regmatch.regprog);
	    vim_free(ಸಾಲು);
	}
	// Move to the NUL, ignore any other arguments.
	eap->arg += STRLEN(eap->arg);
    }
    check_cursor();

    eap->cmdidx = CMD_visual;
    do_exedit(eap, ರಿಕ್ತ);
}

/*
 * ":edit", ":badd", ":balt", ":visual".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_edit(exarg_T *eap)
{
    char_u *ffname = eap->cmdidx == CMD_enew ? ರಿಕ್ತ : eap->arg;

    // Exclude commands which keep the window's current buffer
    ಯದಿ (
	    eap->cmdidx != CMD_badd
	    && eap->cmdidx != CMD_balt
	    // All other commands must obey 'winfixbuf' / ! rules
	    && (is_other_file(0, ffname) && !check_can_set_curbuf_forceit(eap->forceit))
    )
	ಪ್ರತ್ಯಾಗಮ;

    do_exedit(eap, ರಿಕ್ತ);
}

/*
 * ":edit <file>" command and alike.
 */
    ಶೂನ್ಯ
do_exedit(
    exarg_T	*eap,
    win_T	*old_curwin)	    // curwin before doing a split or NULL
{
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		need_hide;
    ಪೂರ್ಣಾಙ್ಕ		exmode_was = exmode_active;

    ಯದಿ ((eap->cmdidx != CMD_pedit && ERROR_IF_POPUP_WINDOW)
						 || ERROR_IF_TERM_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;
    /*
     * ":vi" command ends Ex mode.
     */
    ಯದಿ (exmode_active && (eap->cmdidx == CMD_visual
						|| eap->cmdidx == CMD_view))
    {
	exmode_active = FALSE;
	ex_pressedreturn = FALSE;
	ಯದಿ (*eap->arg == NUL)
	{
	    // Special case:  ":global/pat/visual\NLvi-commands"
	    ಯದಿ (global_busy)
	    {
		ಯದಿ (eap->nextcmd != ರಿಕ್ತ)
		{
		    stuffReadbuff(eap->nextcmd);
		    eap->nextcmd = ರಿಕ್ತ;
		}

		ಯದಿ (exmode_was != EXMODE_VIM)
		    settmode(TMODE_RAW);
		ಪೂರ್ಣಾಙ್ಕ save_RedrawingDisabled = RedrawingDisabled;
		RedrawingDisabled = 0;
		ಪೂರ್ಣಾಙ್ಕ save_nwr = no_wait_return;
		no_wait_return = 0;
		need_wait_return = FALSE;
		ಪೂರ್ಣಾಙ್ಕ save_ms = ಸಂದೇಶ_scroll;
		ಸಂದೇಶ_scroll = 0;
#ifdef FEAT_GUI
		ಪೂರ್ಣಾಙ್ಕ save_he = hold_gui_events;
		hold_gui_events = 0;
#endif
		set_must_redraw(UPD_CLEAR);
		pending_exmode_active = TRUE;

		main_loop(FALSE, TRUE);

		pending_exmode_active = FALSE;
		RedrawingDisabled = save_RedrawingDisabled;
		no_wait_return = save_nwr;
		ಸಂದೇಶ_scroll = save_ms;
#ifdef FEAT_GUI
		hold_gui_events = save_he;
#endif
	    }
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    ಯದಿ ((eap->cmdidx == CMD_new
		|| eap->cmdidx == CMD_tabnew
		|| eap->cmdidx == CMD_tabedit
		|| eap->cmdidx == CMD_vnew) && *eap->arg == NUL)
    {
	// ":new" or ":tabnew" without argument: edit a new empty buffer
	setpcmark();
	(ಶೂನ್ಯ)do_ecmd(0, ರಿಕ್ತ, ರಿಕ್ತ, eap, ECMD_ONE,
		      ECMD_HIDE + (eap->forceit ? ECMD_FORCEIT : 0),
		      old_curwin == ರಿಕ್ತ ? curwin : ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ ಯದಿ ((eap->cmdidx != CMD_split && eap->cmdidx != CMD_vsplit)
	    || *eap->arg != NUL
#ifdef FEAT_BROWSE
	    || (cmdmod.cmod_flags & CMOD_BROWSE)
#endif
	    )
    {
	// Can't edit another file when "textlock" or "curbuf_lock" is set.
	// Only ":edit" or ":script" can bring us here, others are stopped
	// earlier.
	ಯದಿ (*eap->arg != NUL && text_or_buf_locked())
	    ಪ್ರತ್ಯಾಗಮ;

	n = readonlymode;
	ಯದಿ (eap->cmdidx == CMD_view || eap->cmdidx == CMD_sview)
	    readonlymode = TRUE;
	ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_enew)
	    readonlymode = FALSE;   // 'readonly' doesn't make sense in an
				    // empty buffer
	ಯದಿ (eap->cmdidx != CMD_balt && eap->cmdidx != CMD_badd)
	    setpcmark();
	ಯದಿ (do_ecmd(0, (eap->cmdidx == CMD_enew ? ರಿಕ್ತ : eap->arg),
		    ರಿಕ್ತ, eap,
		    // ":edit" goes to first line if Vi compatible
		    (*eap->arg == NUL && eap->do_ecmd_lnum == 0
				      && vim_strchr(p_cpo, CPO_GOTO1) != ರಿಕ್ತ)
					       ? ECMD_ONE : eap->do_ecmd_lnum,
		    (buf_hide(curbuf) ? ECMD_HIDE : 0)
		    + (eap->forceit ? ECMD_FORCEIT : 0)
		      // after a split we can use an existing buffer
		    + (old_curwin != ರಿಕ್ತ ? ECMD_OLDBUF : 0)
		    + (eap->cmdidx == CMD_badd ? ECMD_ADDBUF : 0)
		    + (eap->cmdidx == CMD_balt ? ECMD_ALTBUF : 0)
		    , old_curwin == ರಿಕ್ತ ? curwin : ರಿಕ್ತ) == FAIL)
	{
	    // Editing the file failed.  If the window was split, close it.
	    ಯದಿ (old_curwin != ರಿಕ್ತ)
	    {
		need_hide = (curbufIsChanged() && curbuf->b_nwindows <= 1);
		ಯದಿ (!need_hide || buf_hide(curbuf))
		{
#ಯದಿ defined(FEAT_EVAL)
		    cleanup_T   cs;

		    // Reset the error/interrupt/exception state here so that
		    // aborting() returns FALSE when closing a window.
		    enter_cleanup(&cs);
#endif
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_close(curwin, !need_hide && !buf_hide(curbuf));

#ಯದಿ defined(FEAT_EVAL)
		    // Restore the error/interrupt/exception state if not
		    // discarded by a new aborting error, interrupt, or
		    // uncaught exception.
		    leave_cleanup(&cs);
#endif
		}
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (readonlymode && curbuf->b_nwindows == 1)
	{
	    // When editing an already visited buffer, 'readonly' won't be set
	    // but the previous value is kept.  With ":view" and ":sview" we
	    // want the  file to be readonly, except when another window is
	    // editing the same buffer.
	    curbuf->b_p_ro = TRUE;
	}
	readonlymode = n;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (eap->do_ecmd_cmd != ರಿಕ್ತ)
	    do_cmd_argument(eap->do_ecmd_cmd);
	n = curwin->w_arg_idx_invalid;
	check_arg_idx(curwin);
	ಯದಿ (n != curwin->w_arg_idx_invalid)
	    maketitle();
    }

    /*
     * if ":split file" worked, set alternate file name in old window to new
     * file
     */
    ಯದಿ (old_curwin != ರಿಕ್ತ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_nogui(exarg_T *eap)
{
    eap->errmsg = _(e_gui_cannot_be_used_not_enabled_at_compile_time);
}
#endif

#ಯದಿ defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_tearoff(exarg_T *eap)
{
    gui_make_tearoff(eap->arg);
}
#endif

#ಯದಿ (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_popup(exarg_T *eap)
{
# ಯದಿ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    ಯದಿ (gui.in_use)
	gui_make_popup(eap->arg, eap->forceit);
#  ifdef FEAT_TERM_POPUP_MENU
    ಅನ್ಯಥಾ
#  endif
# endif
# ifdef FEAT_TERM_POPUP_MENU
	pum_make_popup(eap->arg, eap->forceit);
# endif
}
#endif

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_swapname(exarg_T *eap UNUSED)
{
    ಯದಿ (curbuf->b_ml.ml_mfp == ರಿಕ್ತ || curbuf->b_ml.ml_mfp->mf_fname == ರಿಕ್ತ)
	ಸಂದೇಶ(_("No swap file"));
    ಅನ್ಯಥಾ
	ಸಂದೇಶ((ವರ್ಣ *)curbuf->b_ml.ml_mfp->mf_fname);
}

/*
 * ":syncbind" forces all 'scrollbind' windows to have the same relative
 * offset.
 * (1998-11-02 16:21:01  R. Edward Ralston <eralston@computer.org>)
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
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
    ಯದಿ (curwin->w_p_scb)
    {
	topline = curwin->w_topline;
	FOR_ALL_WINDOWS(wp)
	{
	    ಯದಿ (wp->w_p_scb && wp->w_buffer)
	    {
		y = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - get_scrolloff_ಮೌಲ್ಯ();
		ಯದಿ (topline > y)
		    topline = y;
	    }
	}
	ಯದಿ (topline < 1)
	    topline = 1;
    }
    ಅನ್ಯಥಾ
    {
	topline = 1;
    }


    /*
     * Set all scrollbind windows to the same topline.
     */
    FOR_ALL_WINDOWS(curwin)
    {
	ಯದಿ (curwin->w_p_scb)
	{
	    curbuf = curwin->w_buffer;
	    y = topline - curwin->w_topline;
	    ಯದಿ (y > 0)
		scrollup(y, TRUE);
	    ಅನ್ಯಥಾ
		scrolldown(-y, TRUE);
	    curwin->w_scbind_pos = topline;
	    redraw_later(UPD_VALID);
	    cursor_correct();
	    curwin->w_redr_status = true;
	}
    }
    curwin = save_curwin;
    curbuf = save_curbuf;
    ಯದಿ (curwin->w_p_scb)
    {
	did_syncbind = TRUE;
	checkpcmark();
	ಯದಿ (old_linenr != curwin->w_cursor.lnum)
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
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum;

    ಯದಿ (eap->usefilter)			// :r!cmd
    {
	do_bang(1, eap, FALSE, FALSE, TRUE);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (u_save(eap->line2, (linenr_T)(eap->line2 + 1)) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_BROWSE
    ಯದಿ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browseFile;

	browseFile = do_browse(0, (char_u *)_("Append File"), eap->arg,
		ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, curbuf);
	ಯದಿ (browseFile != ರಿಕ್ತ)
	{
	    i = readfile(browseFile, ರಿಕ್ತ,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	    vim_free(browseFile);
	}
	ಅನ್ಯಥಾ
	    i = OK;
    }
    ಅನ್ಯಥಾ
#endif
	ಯದಿ (*eap->arg == NUL)
	{
	    ಯದಿ (check_fname() == FAIL)	// check for no file name
		ಪ್ರತ್ಯಾಗಮ;
	    i = readfile(curbuf->b_ffname, curbuf->b_fname,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (vim_strchr(p_cpo, CPO_ALTREAD) != ರಿಕ್ತ)
		(ಶೂನ್ಯ)setaltfname(eap->arg, eap->arg, (linenr_T)1);
	    i = readfile(eap->arg, ರಿಕ್ತ,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);

	}
    ಯದಿ (i != OK)
    {
#ಯದಿ defined(FEAT_EVAL)
	ಯದಿ (!aborting())
#endif
	    semsg(_(e_cant_open_file_str), eap->arg);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (empty && exmode_active)
	{
	    // Delete the empty line that remains.  Historically ex does
	    // this but vi doesn't.
	    ಯದಿ (eap->line2 == 0)
		lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಅನ್ಯಥಾ
		lnum = 1;
	    ಯದಿ (*ml_get(lnum) == NUL && u_savedel(lnum, 1L) == OK)
	    {
		ml_delete(lnum);
		ಯದಿ (curwin->w_cursor.lnum > 1
			&& curwin->w_cursor.lnum >= lnum)
		    --curwin->w_cursor.lnum;
		deleted_ಸಾಲುಗಳು_mark(lnum, 1L);
	    }
	}
	redraw_curbuf_later(UPD_VALID);
    }
}

ಸ್ಥಿರ char_u	*prev_dir = ರಿಕ್ತ;

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
free_cd_dir(ಶೂನ್ಯ)
{
    VIM_CLEAR(prev_dir);
    VIM_CLEAR(globaldir);
}
#endif

/*
 * Get the previous directory for the given chdir scope.
 */
    ಸ್ಥಿರ char_u *
get_prevdir(cdscope_T scope)
{
    ಯದಿ (scope == CDSCOPE_WINDOW)
	ಪ್ರತ್ಯಾಗಮ curwin->w_prevdir;
    ಅನ್ಯಥಾ ಯದಿ (scope == CDSCOPE_TABPAGE)
	ಪ್ರತ್ಯಾಗಮ curtab->tp_prevdir;
    ಪ್ರತ್ಯಾಗಮ prev_dir;
}

/*
 * Deal with the side effects of changing the current directory.
 * When 'scope' is CDSCOPE_TABPAGE then this was after an ":tcd" command.
 * When 'scope' is CDSCOPE_WINDOW then this was after an ":lcd" command.
 */
    ಶೂನ್ಯ
post_chdir(cdscope_T scope)
{
    ಯದಿ (scope != CDSCOPE_WINDOW)
	// Clear tab local directory for both :cd and :tcd
	VIM_CLEAR(curtab->tp_localdir);
    VIM_CLEAR(curwin->w_localdir);
    ಯದಿ (scope != CDSCOPE_GLOBAL)
    {
	char_u	*pdir = get_prevdir(scope);

	// If still in the global directory, need to remember current
	// directory as the global directory.
	ಯದಿ (globaldir == ರಿಕ್ತ && pdir != ರಿಕ್ತ)
	    globaldir = vim_strsave(pdir);

	// Remember this local directory for the window.
	ಯದಿ (mch_dirname(NameBuff, MAXPATHL) == OK)
	{
	    ಯದಿ (scope == CDSCOPE_TABPAGE)
		curtab->tp_localdir = vim_strsave(NameBuff);
	    ಅನ್ಯಥಾ
		curwin->w_localdir = vim_strsave(NameBuff);
	}
    }
    ಅನ್ಯಥಾ
    {
	// We are now in the global directory, no need to remember its name.
	VIM_CLEAR(globaldir);
    }

    last_chdir_reason = ರಿಕ್ತ;
    shorten_fnames(vim_strchr(p_cpo, CPO_NOSYMLINKS) == ರಿಕ್ತ);
}

/*
 * Trigger DirChangedPre for "acmd_fname" with directory "new_dir".
 */
    ಶೂನ್ಯ
trigger_DirChangedPre(char_u *acmd_fname, char_u *new_dir)
{
#ifdef FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;

    v_event = get_v_event(&save_v_event);
    (ಶೂನ್ಯ)dict_add_string(v_event, "directory", new_dir);
    dict_set_ವಸ್ತುಗಳು_ro(v_event);
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
    ಪೂರ್ಣಾಙ್ಕ
changedir_func(
	char_u		*new_dir,
	ಪೂರ್ಣಾಙ್ಕ		forceit,
	cdscope_T	scope)
{
    char_u	*pdir = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		dir_differs;
    char_u	*acmd_fname = ರಿಕ್ತ;
    char_u	**pp;
    char_u	*tofree;

    ಯದಿ (new_dir == ರಿಕ್ತ || allbuf_locked())
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (vim_strchr(p_cpo, CPO_CHDIR) != ರಿಕ್ತ && curbufIsChanged() && !forceit)
    {
	emsg(_(e_cannot_change_directory_buffer_is_modified_add_bang_to_override));
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // ":cd -": Change to previous directory
    ಯದಿ (STRCMP(new_dir, "-") == 0)
    {
	pdir = get_prevdir(scope);
	ಯದಿ (pdir == ರಿಕ್ತ)
	{
	    emsg(_(e_no_previous_directory));
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
	new_dir = pdir;
    }

    // Save current directory for next ":cd -"
    ಯದಿ (mch_dirname(NameBuff, MAXPATHL) == OK)
	pdir = vim_strsave(NameBuff);
    ಅನ್ಯಥಾ
	pdir = ರಿಕ್ತ;

    // For UNIX ":cd" means: go to home directory.
    // On other systems too if 'cdhome' is set.
#ಯದಿ defined(UNIX) || defined(VMS)
    ಯದಿ (*new_dir == NUL)
#ಅನ್ಯಥಾ
    ಯದಿ (*new_dir == NUL && p_cdh)
#endif
    {
	// use NameBuff for home directory name
#ifdef VMS
	char_u	*p;

	p = mch_getenv((char_u *)"SYS$LOGIN");
	ಯದಿ (p == ರಿಕ್ತ || *p == NUL)	// empty is the same as not set
	    NameBuff[0] = NUL;
	ಅನ್ಯಥಾ
	    vim_strncpy(NameBuff, p, MAXPATHL - 1);
#ಅನ್ಯಥಾ
	expand_env((char_u *)"$HOME", NameBuff, MAXPATHL);
#endif
	new_dir = NameBuff;
    }
    dir_differs = pdir == ರಿಕ್ತ
			    || pathcmp((ವರ್ಣ *)pdir, (ವರ್ಣ *)new_dir, -1) != 0;
    ಯದಿ (dir_differs)
    {
	ಯದಿ (scope == CDSCOPE_WINDOW)
	    acmd_fname = (char_u *)"window";
	ಅನ್ಯಥಾ ಯದಿ (scope == CDSCOPE_TABPAGE)
	    acmd_fname = (char_u *)"tabpage";
	ಅನ್ಯಥಾ
	    acmd_fname = (char_u *)"global";
	trigger_DirChangedPre(acmd_fname, new_dir);

	ಯದಿ (vim_chdir(new_dir))
	{
	    emsg(_(e_command_failed));
	    vim_free(pdir);
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }

    ಯದಿ (scope == CDSCOPE_WINDOW)
	pp = &curwin->w_prevdir;
    ಅನ್ಯಥಾ ಯದಿ (scope == CDSCOPE_TABPAGE)
	pp = &curtab->tp_prevdir;
    ಅನ್ಯಥಾ
	pp = &prev_dir;
    tofree = *pp;  // new_dir may use this
    *pp = pdir;

    post_chdir(scope);

    ಯದಿ (dir_differs)
	apply_autocmds(EVENT_DIRCHANGED, acmd_fname, new_dir, FALSE, curbuf);
    vim_free(tofree);
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * ":cd", ":tcd", ":lcd", ":chdir" ":tchdir" and ":lchdir".
 */
    ಶೂನ್ಯ
ex_cd(exarg_T *eap)
{
    char_u	*new_dir;

    new_dir = eap->arg;
#ಯದಿ !defined(UNIX) && !defined(VMS)
    // for non-UNIX ":cd" means: print current directory unless 'cdhome' is set
    ಯದಿ (*new_dir == NUL && !p_cdh)
    {
	ex_pwd(ರಿಕ್ತ);
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

    cdscope_T	scope = CDSCOPE_GLOBAL;

    ಯದಿ (eap->cmdidx == CMD_lcd || eap->cmdidx == CMD_lchdir)
	scope = CDSCOPE_WINDOW;
    ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_tcd || eap->cmdidx == CMD_tchdir)
	scope = CDSCOPE_TABPAGE;

    ಯದಿ (changedir_func(new_dir, eap->forceit, scope))
    {
	// Echo the new current directory if the command was typed.
	ಯದಿ (ಕೀಲಿ_Typed || p_verbose >= 5)
	    ex_pwd(eap);
    }
}

/*
 * ":pwd".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_pwd(exarg_T *eap UNUSED)
{
    ಯದಿ (mch_dirname(NameBuff, MAXPATHL) == OK)
    {
#ifdef BACKSLASH_IN_FILENAME
	slash_adjust(NameBuff);
#endif
	ಯದಿ (p_verbose > 0)
	{
	    ವರ್ಣ *context = "global";

	    ಯದಿ (last_chdir_reason != ರಿಕ್ತ)
		context = last_chdir_reason;
	    ಅನ್ಯಥಾ ಯದಿ (curwin->w_localdir != ರಿಕ್ತ)
		context = "window";
	    ಅನ್ಯಥಾ ಯದಿ (curtab->tp_localdir != ರಿಕ್ತ)
		context = "tabpage";
	    smsg("[%s] %s", context, (ವರ್ಣ *)NameBuff);
	}
	ಅನ್ಯಥಾ
	    ಸಂದೇಶ((ವರ್ಣ *)NameBuff);
    }
    ಅನ್ಯಥಾ
	emsg(_(e_directory_unknown));
}

/*
 * ":=".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_equal(exarg_T *eap)
{
    smsg("%ld", (long)eap->line2);
    ex_may_print(eap);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_sleep(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ		n;
    long	len;

    ಯದಿ (cursor_valid())
    {
	n = W_WINROW(curwin) + curwin->w_wrow - ಸಂದೇಶ_scrolled;
	ಯದಿ (n >= 0)
	    windgoto(n, curwin->w_wincol + curwin->w_wcol);
    }

    len = eap->line2;
    ವಿಕಲ್ಪ (*eap->arg)
    {
	ಪಕ್ಷ 'm': ಭಙ್ಗ;
	ಪಕ್ಷ NUL: len *= 1000L; ಭಙ್ಗ;
	ಸ್ವಭಾವ: semsg(_(e_invalid_argument_str), eap->arg); ಪ್ರತ್ಯಾಗಮ;
    }

    // Hide the cursor if invoked with !
    do_sleep(len, eap->forceit);
}

/*
 * Sleep for "msec" milliseconds, but keep checking for a CTRL-C every second.
 * Hide the cursor if "hide_cursor" is TRUE.
 */
    ಶೂನ್ಯ
do_sleep(long msec, ಪೂರ್ಣಾಙ್ಕ hide_cursor)
{
    long	done = 0;
    long	wait_now;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    // Remember at what time we started, so that we know how much longer we
    // should wait after waiting for a bit.
    ELAPSED_INIT(start_tv);
#endif

    ಯದಿ (hide_cursor)
	cursor_sleep();
    ಅನ್ಯಥಾ
	cursor_on();

    out_flush_cursor(FALSE, FALSE);
    ಯಾವತ್ (!got_int && done < msec)
    {
	wait_now = msec - done > 1000L ? 1000L : msec - done;
#ifdef FEAT_TIMERS
	{
	    long    due_time = check_due_timer();

	    ಯದಿ (due_time > 0 && due_time < wait_now)
		wait_now = due_time;
	}
#endif
#ifdef FEAT_JOB_CHANNEL
	ಯದಿ (has_any_channel() && wait_now > 20L)
	    wait_now = 20L;
#endif
#ifdef FEAT_SOUND
	ಯದಿ (has_any_sound_callback() && wait_now > 20L)
	    wait_now = 20L;
#endif
	ui_delay(wait_now, TRUE);

#ifdef FEAT_JOB_CHANNEL
	ಯದಿ (has_any_channel())
	    ui_breakcheck_force(TRUE);
	ಅನ್ಯಥಾ
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
#ಅನ್ಯಥಾ
	// guesstimate time passed (will actually be more)
	done += wait_now;
#endif
    }

    // If CTRL-C was typed to interrupt the sleep, drop the CTRL-C from the
    // input buffer, otherwise a following call to input() fails.
    ಯದಿ (got_int)
	(ಶೂನ್ಯ)vpeekc();

    ಯದಿ (hide_cursor)
	cursor_unsleep();
}

/*
 * ":winsize" command (obsolete).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_winsize(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ		w, h;
    char_u	*arg = eap->arg;
    char_u	*p;

    ಯದಿ (!SAFE_isdigit(*arg))
    {
	semsg(_(e_invalid_argument_str), arg);
	ಪ್ರತ್ಯಾಗಮ;
    }
    w = getdigits(&arg);
    arg = skipwhite(arg);
    p = arg;
    h = getdigits(&arg);
    ಯದಿ (*p != NUL && *arg == NUL)
	set_shellsize(w, h, TRUE);
    ಅನ್ಯಥಾ
	emsg(_(e_winsize_requires_two_ಸಂಖ್ಯೆ_arguments));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_wincmd(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ		xchar = NUL;
    char_u	*p;

    ಯದಿ (*eap->arg == 'g' || *eap->arg == Ctrl_G)
    {
	// CTRL-W g and CTRL-W CTRL-G  have an extra command character
	ಯದಿ (eap->arg[1] == NUL)
	{
	    emsg(_(e_invalid_argument));
	    ಪ್ರತ್ಯಾಗಮ;
	}
	xchar = eap->arg[1];
	p = eap->arg + 2;
    }
    ಅನ್ಯಥಾ
	p = eap->arg + 1;

    set_nextcmd(eap, p);
    p = skipwhite(p);
    ಯದಿ (*p != NUL && *p != (
#ifdef FEAT_EVAL
	    in_vim9script() ? '#' :
#endif
		'"')
	    && eap->nextcmd == ರಿಕ್ತ)
	emsg(_(e_invalid_argument));
    ಅನ್ಯಥಾ ಯದಿ (!eap->skip)
    {
	// Pass flags on for ":vertical wincmd ]".
	postponed_split_flags = cmdmod.cmod_split;
	postponed_split_tab = cmdmod.cmod_tab;
	do_window(*eap->arg, eap->addr_ಎಣಿಕೆ > 0 ? eap->line2 : 0L, xchar);
	postponed_split_flags = 0;
	postponed_split_tab = 0;
    }
}

#ಯದಿ defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * ":winpos".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_winpos(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ		x, y;
    char_u	*arg = eap->arg;
    char_u	*p;

    ಯದಿ (*arg == NUL)
    {
# ಯದಿ defined(FEAT_GUI) || defined(MSWIN)
#  ifdef VIMDLL
	ಯದಿ (gui.in_use ? gui_mch_get_winpos(&x, &y) != FAIL :
		mch_get_winpos(&x, &y) != FAIL)
#  elif defined(FEAT_GUI)
	ಯದಿ (gui.in_use && gui_mch_get_winpos(&x, &y) != FAIL)
#  ಅನ್ಯಥಾ
	ಯದಿ (mch_get_winpos(&x, &y) != FAIL)
#  endif
	{
	    sprintf((ವರ್ಣ *)IObuff, _("Window position: X %d, Y %d"), x, y);
	    ಸಂದೇಶ((ವರ್ಣ *)IObuff);
	}
	ಅನ್ಯಥಾ
# endif
	    emsg(_(e_obtaining_window_position_not_implemented_for_this_platform));
    }
    ಅನ್ಯಥಾ
    {
	x = getdigits(&arg);
	arg = skipwhite(arg);
	p = arg;
	y = getdigits(&arg);
	ಯದಿ (*p == NUL || *arg != NUL)
	{
	    emsg(_(e_winpos_requires_two_ಸಂಖ್ಯೆ_arguments));
	    ಪ್ರತ್ಯಾಗಮ;
	}
# ifdef FEAT_GUI
	ಯದಿ (gui.in_use)
	    gui_mch_set_winpos(x, y);
	ಅನ್ಯಥಾ ಯದಿ (gui.starting)
	{
	    // Remember the coordinates for when the window is opened.
	    gui_win_x = x;
	    gui_win_y = y;
	}
#  ಯದಿ defined(HAVE_TGETENT) || defined(VIMDLL)
	ಅನ್ಯಥಾ
#  endif
# endif
# ಯದಿ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	    mch_set_winpos(x, y);
# endif
# ifdef HAVE_TGETENT
	ಯದಿ (*T_CWP)
	    term_set_winpos(x, y);
# endif
    }
}
#endif

/*
 * Handle command that work like operators: ":delete", ":yank", ":>" and ":<".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_operators(exarg_T *eap)
{
    oparg_T	oa;

    clear_oparg(&oa);
    oa.regname = eap->regname;
    oa.start.lnum = eap->line1;
    oa.end.lnum = eap->line2;
    oa.ಸಾಲು_ಎಣಿಕೆ = eap->line2 - eap->line1 + 1;
    oa.motion_type = MLINE;
    virtual_op = FALSE;
    ಯದಿ (eap->cmdidx != CMD_yank)	// position cursor for undo
    {
	setpcmark();
	curwin->w_cursor.lnum = eap->line1;
	beginline(BL_SOL | BL_FIX);
    }

    ಯದಿ (VIsual_active)
	end_visual_mode();

    ವಿಕಲ್ಪ (eap->cmdidx)
    {
	ಪಕ್ಷ CMD_delete:
	    oa.op_type = OP_DELETE;
	    op_delete(&oa);
	    ಭಙ್ಗ;

	ಪಕ್ಷ CMD_yank:
	    oa.op_type = OP_YANK;
	    (ಶೂನ್ಯ)op_yank(&oa, FALSE, TRUE);
	    ಭಙ್ಗ;

	ಸ್ವಭಾವ:    // CMD_rshift or CMD_lshift
	    ಯದಿ (
#ifdef FEAT_RIGHTLEFT
		(eap->cmdidx == CMD_rshift) ^ curwin->w_p_rl
#ಅನ್ಯಥಾ
		eap->cmdidx == CMD_rshift
#endif
						)
		oa.op_type = OP_RSHIFT;
	    ಅನ್ಯಥಾ
		oa.op_type = OP_LSHIFT;
	    op_shift(&oa, FALSE, eap->amount);
	    ಭಙ್ಗ;
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
    ಯದಿ (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, ರಿಕ್ತ, eap->forceit ? BACKWARD : FORWARD, 1L,
						       PUT_ಸಾಲು|PUT_CURSLINE);
}

/*
 * ":iput".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_iput(exarg_T *eap)
{
    // ":0iput" works like ":1iput!".
    ಯದಿ (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, ರಿಕ್ತ, eap->forceit ? BACKWARD : FORWARD, 1L,
						      PUT_ಸಾಲು|PUT_CURSLINE
						      |PUT_FIXINDENT);
}

/*
 * Handle ":copy" and ":move".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_copymove(exarg_T *eap)
{
    long	n;

#ifdef FEAT_EVAL
    ಯದಿ (not_in_vim9(eap, false) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
#endif
    n = get_address(eap, &eap->arg, eap->addr_type, FALSE, FALSE, FALSE, 1);
    ಯದಿ (eap->arg == ರಿಕ್ತ)	    // error detected
    {
	eap->nextcmd = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ;
    }
    get_flags(eap);

    /*
     * move or copy lines from 'eap->line1'-'eap->line2' to below line 'n'
     */
    ಯದಿ (n == MAXLNUM || n < 0 || n > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	emsg(_(e_invalid_range));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (eap->cmdidx == CMD_move)
    {
	ಯದಿ (do_move(eap->line1, eap->line2, n) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
    }
    ಅನ್ಯಥಾ
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
    ಯದಿ (eap->flags != 0)
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
    ಯದಿ (eap->line1 == eap->line2)
    {
	ಯದಿ (eap->addr_ಎಣಿಕೆ >= 2)   // :2,2join does nothing
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (eap->line2 == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    beep_flush();
	    ಪ್ರತ್ಯಾಗಮ;
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
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		prev_len = typebuf.tb_len;

    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif

    // get the register name.  No name means to use the previous one
    c = *eap->arg;
    ಯದಿ (c == NUL || (c == '*' && *eap->cmd == '*'))
	c = '@';
    // Put the register in the typeahead buffer with the "silent" flag.
    ಯದಿ (do_execreg(c, TRUE, vim_strchr(p_cpo, CPO_EXECBUF) != ರಿಕ್ತ, TRUE)
								      == FAIL)
    {
	beep_flush();
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಪೂರ್ಣಾಙ್ಕ	save_efr = exec_from_reg;

    exec_from_reg = TRUE;

    /*
     * Execute from the typeahead buffer.
     * Continue until the stuff buffer is empty and all added characters
     * have been consumed.
     */
    ಯಾವತ್ (!stuff_empty() || typebuf.tb_len > prev_len)
	(ಶೂನ್ಯ)do_cmdline(ರಿಕ್ತ, getexline, ರಿಕ್ತ, DOCMD_NOWAIT|DOCMD_VERBOSE);

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
    ಯದಿ (eap->addr_ಎಣಿಕೆ == 1)	    // :undo 123
	undo_time(eap->line2, FALSE, FALSE, TRUE);
    ಅನ್ಯಥಾ
	u_undo(1);
}

#ifdef FEAT_PERSISTENT_UNDO
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_wundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_SIZE];

    u_compute_hash(hash);
    u_write_undo(eap->arg, eap->forceit, curbuf, hash);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_rundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_SIZE];

    u_compute_hash(hash);
    u_read_undo(eap->arg, hash, ರಿಕ್ತ);
}
#endif

/*
 * ":redo".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_redo(exarg_T *eap UNUSED)
{
    u_redo(1);
}

/*
 * ":earlier" and ":later".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_later(exarg_T *eap)
{
    long	ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		sec = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		file = FALSE;
    char_u	*p = eap->arg;

    ಯದಿ (*p == NUL)
	ಎಣಿಕೆ = 1;
    ಅನ್ಯಥಾ ಯದಿ (SAFE_isdigit(*p))
    {
	ಎಣಿಕೆ = getdigits(&p);
	ವಿಕಲ್ಪ (*p)
	{
	    ಪಕ್ಷ 's': ++p; sec = TRUE; ಭಙ್ಗ;
	    ಪಕ್ಷ 'm': ++p; sec = TRUE; ಎಣಿಕೆ *= 60; ಭಙ್ಗ;
	    ಪಕ್ಷ 'h': ++p; sec = TRUE; ಎಣಿಕೆ *= 60 * 60; ಭಙ್ಗ;
	    ಪಕ್ಷ 'd': ++p; sec = TRUE; ಎಣಿಕೆ *= 24 * 60 * 60; ಭಙ್ಗ;
	    ಪಕ್ಷ 'f': ++p; file = TRUE; ಭಙ್ಗ;
	}
    }

    ಯದಿ (*p != NUL)
	semsg(_(e_invalid_argument_str), eap->arg);
    ಅನ್ಯಥಾ
	undo_time(eap->cmdidx == CMD_earlier ? -ಎಣಿಕೆ : ಎಣಿಕೆ,
							    sec, file, FALSE);
}

/*
 * ":redir": start/stop redirection.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_redir(exarg_T *eap)
{
    ವರ್ಣ	*mode;
    char_u	*fname;
    char_u	*arg = eap->arg;

#ifdef FEAT_EVAL
    ಯದಿ (redir_execute)
    {
	emsg(_(e_cannot_use_redir_inside_execute));
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

    ಯದಿ (STRICMP(eap->arg, "END") == 0)
	close_redir();
    ಅನ್ಯಥಾ
    {
	ಯದಿ (*arg == '>')
	{
	    ++arg;
	    ಯದಿ (*arg == '>')
	    {
		++arg;
		mode = "a";
	    }
	    ಅನ್ಯಥಾ
		mode = "w";
	    arg = skipwhite(arg);

	    close_redir();

	    // Expand environment variables and "~/".
	    fname = expand_env_save(arg);
	    ಯದಿ (fname == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
#ifdef FEAT_BROWSE
	    ಯದಿ (cmdmod.cmod_flags & CMOD_BROWSE)
	    {
		char_u	*browseFile;

		browseFile = do_browse(BROWSE_SAVE,
			(char_u *)_("Save Redirection"),
			fname, ರಿಕ್ತ, ರಿಕ್ತ,
			(char_u *)_(BROWSE_FILTER_ALL_FILES), curbuf);
		ಯದಿ (browseFile == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ;		// operation cancelled
		vim_free(fname);
		fname = browseFile;
		eap->forceit = TRUE;	// since dialog already asked
	    }
#endif

	    redir_fd = open_exfile(fname, eap->forceit, mode);
	    vim_free(fname);
	}
#ifdef FEAT_EVAL
	ಅನ್ಯಥಾ ಯದಿ (*arg == '@')
	{
	    // redirect to a register a-z (resp. A-Z for appending)
	    close_redir();
	    ++arg;
	    ಯದಿ (ASCII_ISALPHA(*arg)
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
		ಯದಿ (*arg == '>' && arg[1] == '>')  // append
		    arg += 2;
		ಅನ್ಯಥಾ
		{
		    // Can use both "@a" and "@a>".
		    ಯದಿ (*arg == '>')
			arg++;
		    // Make register empty when not using @A-@Z and the
		    // command is valid.
		    ಯದಿ (*arg == NUL && !SAFE_isupper(redir_reg))
			write_reg_contents(redir_reg, (char_u *)"", -1, FALSE);
		}
	    }
	    ಯದಿ (*arg != NUL)
	    {
		redir_reg = 0;
		semsg(_(e_invalid_argument_str), eap->arg);
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (*arg == '=' && arg[1] == '>')
	{
	    ಪೂರ್ಣಾಙ್ಕ append;

	    // redirect to a variable
	    close_redir();
	    arg += 2;

	    ಯದಿ (*arg == '>')
	    {
		++arg;
		append = TRUE;
	    }
	    ಅನ್ಯಥಾ
		append = FALSE;

	    ಯದಿ (var_redir_start(skipwhite(arg), append) == OK)
		redir_vname = 1;
	}
#endif

	// TODO: redirect to a buffer

	ಅನ್ಯಥಾ
	    semsg(_(e_invalid_argument_str), eap->arg);
    }

    // Make sure redirection is not off.  Can happen for cmdline completion
    // that indirectly invokes a command to catch its output.
    ಯದಿ (redir_fd != ರಿಕ್ತ
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname
#endif
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
redraw_cmd(ಪೂರ್ಣಾಙ್ಕ clear)
{
    ಪೂರ್ಣಾಙ್ಕ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ಪೂರ್ಣಾಙ್ಕ save_p_lz = p_lz;
    p_lz = FALSE;

    validate_cursor();
    update_topline();
    update_screen(clear ? UPD_CLEAR : VIsual_active ? UPD_INVERTED : 0);
    ಯದಿ ((State & MODE_CMDLINE) == 0)
	setcursor(); // put cursor back where it belongs
    ಯದಿ (need_maketitle)
	maketitle();
#ಯದಿ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
    ಯದಿ (!gui.in_use)
# endif
	resize_console_buf();
#endif
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;

    // After drawing the statusline screen_attr may still be set.
    screen_stop_highlight();

    // Reset msg_didout, so that a message that's there is overwritten.
    ಸಂದೇಶ_didout = FALSE;
    ಸಂದೇಶ_col = 0;

    // No need to wait after an intentional redraw.
    need_wait_return = FALSE;

    // When invoked from a callback or autocmd the command line may be active.
    ಯದಿ (State & MODE_CMDLINE)
	redrawcmdline();

    out_flush();
}

/*
 * ":redrawstatus": force redraw of status line(s)
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_redrawstatus(exarg_T *eap UNUSED)
{
    ಯದಿ (eap->forceit)
	status_redraw_all();
    ಅನ್ಯಥಾ
	status_redraw_curbuf();
    redraw_vseps = TRUE;
    ಯದಿ (ಸಂದೇಶ_scrolled && (State & MODE_CMDLINE))
	ಪ್ರತ್ಯಾಗಮ;  // redraw later

    ಪೂರ್ಣಾಙ್ಕ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ಪೂರ್ಣಾಙ್ಕ save_p_lz = p_lz;
    p_lz = FALSE;

    ಯದಿ (State & MODE_CMDLINE)
	redraw_statuslines();
    ಅನ್ಯಥಾ
	update_screen(VIsual_active ? UPD_INVERTED : 0);
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabline": force redraw of the tabline
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_redrawtabline(exarg_T *eap UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ಪೂರ್ಣಾಙ್ಕ save_p_lz = p_lz;
    p_lz = FALSE;

    draw_tabline();

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabpanel": force redraw of the tabpanel
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_redrawtabpanel(exarg_T *eap UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ಪೂರ್ಣಾಙ್ಕ save_p_lz = p_lz;
    p_lz = FALSE;

#ಯದಿ defined(FEAT_TABPANEL)
    draw_tabpanel();
#ಅನ್ಯಥಾ
    emsg(_(e_cannot_not_support_redrawtabpanel));
#endif

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
close_redir(ಶೂನ್ಯ)
{
    ಯದಿ (redir_fd != ರಿಕ್ತ)
    {
	fclose(redir_fd);
	redir_fd = ರಿಕ್ತ;
    }
#ifdef FEAT_EVAL
    redir_reg = 0;
    ಯದಿ (redir_vname)
    {
	var_redir_stop();
	redir_vname = 0;
    }
#endif
}

#ಯದಿ defined(FEAT_SESSION) || defined(FEAT_EVAL)
    ಪೂರ್ಣಾಙ್ಕ
vim_mkdir_emsg(char_u *name, ಪೂರ್ಣಾಙ್ಕ prot UNUSED)
{
    ಯದಿ (vim_mkdir(name, prot) != 0)
    {
	semsg(_(e_cannot_create_directory_str), name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

/*
 * Open a file for writing for an Ex command, with some checks.
 * Return file descriptor, or NULL on failure.
 */
    FILE *
open_exfile(
    char_u	*fname,
    ಪೂರ್ಣಾಙ್ಕ		forceit,
    ವರ್ಣ	*mode)	    // "w" for create new file or "a" for append
{
    FILE	*fd;

#ifdef UNIX
    // with Unix it is possible to open a directory
    ಯದಿ (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
#endif
    ಯದಿ (!forceit && *mode != 'a' && vim_fexists(fname))
    {
	semsg(_(e_str_exists_add_bang_to_override), fname);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ ((fd = mch_fopen((ವರ್ಣ *)fname, mode)) == ರಿಕ್ತ)
	semsg(_(e_cannot_open_str_for_writing_2), fname);

    ಪ್ರತ್ಯಾಗಮ fd;
}

/*
 * ":mark" and ":k".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_mark(exarg_T *eap)
{
    pos_T	pos;

#ifdef FEAT_EVAL
    ಯದಿ (not_in_vim9(eap, false) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
#endif
    ಯದಿ (*eap->arg == NUL)		// No argument?
    {
	emsg(_(e_argument_required));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (eap->arg[1] != NUL)	// more than one character?
    {
	semsg(_(e_trailing_characters_str), eap->arg);
	ಪ್ರತ್ಯಾಗಮ;
    }

    pos = curwin->w_cursor;		// save curwin->w_cursor
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_WHITE | BL_FIX);
    ಯದಿ (setmark(*eap->arg) == FAIL)	// set mark
	emsg(_(e_argument_must_be_letter_or_forward_backward_quote));
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
    ಯದಿ (!curwin->w_p_wrap)
	validate_cursor();
    update_curswant();
}

/*
 * Save the current State and go to Normal mode.
 * Return TRUE if the typeahead could be saved.
 */
    ಪೂರ್ಣಾಙ್ಕ
save_current_state(save_state_T *sst)
{
    sst->save_ಸಂದೇಶ_scroll = ಸಂದೇಶ_scroll;
    sst->save_restart_edit = restart_edit;
    sst->save_ಸಂದೇಶ_didout = ಸಂದೇಶ_didout;
    sst->save_State = State;
    sst->save_insertmode = p_im;
    sst->save_finish_op = finish_op;
    sst->save_opcount = opcount;
    sst->save_reg_executing = reg_executing;
    sst->save_pending_end_reg_executing = pending_end_reg_executing;

    ಸಂದೇಶ_scroll = FALSE;		    // no msg scrolling in Normal mode
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
    ಪ್ರತ್ಯಾಗಮ sst->tabuf.typebuf_valid;
}

    ಶೂನ್ಯ
restore_current_state(save_state_T *sst)
{
    // Restore the previous typeahead.
    restore_typeahead(&sst->tabuf, FALSE);

    ಸಂದೇಶ_scroll = sst->save_ಸಂದೇಶ_scroll;
    restart_edit = sst->save_restart_edit;
    p_im = sst->save_insertmode;
    finish_op = sst->save_finish_op;
    opcount = sst->save_opcount;
    reg_executing = sst->save_reg_executing;
    pending_end_reg_executing = sst->save_pending_end_reg_executing;
    ಸಂದೇಶ_didout |= sst->save_ಸಂದೇಶ_didout;	// don't reset msg_didout now
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
    ಶೂನ್ಯ
ex_normal(exarg_T *eap)
{
    save_state_T save_state;
    char_u	*arg = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		l;
    char_u	*p;

    ಯದಿ (ex_normal_lock > 0)
    {
	emsg(_(e_not_allowed_here));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (ex_normal_busy >= p_mmd)
    {
	emsg(_(e_recursive_use_of_normal_too_deep));
	ಪ್ರತ್ಯಾಗಮ;
    }

    /*
     * vgetc() expects a CSI and K_SPECIAL to have been escaped.  Don't do
     * this for the K_SPECIAL leading byte, otherwise special keys will not
     * work.
     */
    ಯದಿ (has_mbyte)
    {
	ಪೂರ್ಣಾಙ್ಕ	len = 0;

	// Count the number of characters to be escaped.
	ಪ್ರತಿ (p = eap->arg; *p != NUL; ++p)
	{
#ifdef FEAT_GUI
	    ಯದಿ (*p == CSI)  // leadbyte CSI
		len += 2;
#endif
	    ಪ್ರತಿ (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		ಯದಿ (*++p == K_SPECIAL	  // trailbyte K_SPECIAL or CSI
#ifdef FEAT_GUI
			|| *p == CSI
#endif
			)
		    len += 2;
	}
	ಯದಿ (len > 0)
	{
	    arg = alloc(STRLEN(eap->arg) + len + 1);
	    ಯದಿ (arg != ರಿಕ್ತ)
	    {
		len = 0;
		ಪ್ರತಿ (p = eap->arg; *p != NUL; ++p)
		{
		    arg[len++] = *p;
#ifdef FEAT_GUI
		    ಯದಿ (*p == CSI)
		    {
			arg[len++] = KS_EXTRA;
			arg[len++] = (ಪೂರ್ಣಾಙ್ಕ)KE_CSI;
		    }
#endif
		    ಪ್ರತಿ (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		    {
			arg[len++] = *++p;
			ಯದಿ (*p == K_SPECIAL)
			{
			    arg[len++] = KS_SPECIAL;
			    arg[len++] = KE_FILLER;
			}
#ifdef FEAT_GUI
			ಅನ್ಯಥಾ ಯದಿ (*p == CSI)
			{
			    arg[len++] = KS_EXTRA;
			    arg[len++] = (ಪೂರ್ಣಾಙ್ಕ)KE_CSI;
			}
#endif
		    }
		    arg[len] = NUL;
		}
	    }
	}
    }

    ++ex_normal_busy;
    ಯದಿ (save_current_state(&save_state))
    {
	/*
	 * Repeat the :normal command for each line in the range.  When no
	 * range given, execute it just once, without positioning the cursor
	 * first.
	 */
	ಕುರು
	{
	    ಯದಿ (eap->addr_ಎಣಿಕೆ != 0)
	    {
		curwin->w_cursor.lnum = eap->line1++;
		curwin->w_cursor.col = 0;
		check_cursor_moved(curwin);
	    }

	    exec_normal_cmd(arg != ರಿಕ್ತ
		     ? arg
		     : eap->arg, eap->forceit ? REMAP_NONE : REMAP_YES, FALSE);
	}
	ಯಾವತ್ (eap->addr_ಎಣಿಕೆ > 0 && eap->line1 <= eap->line2 && !got_int);
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_startinsert(exarg_T *eap)
{
#ifdef FEAT_TERMINAL
    // Ignore this when running in an active terminal.
    ಯದಿ (term_job_running(curbuf->b_term))
	ಪ್ರತ್ಯಾಗಮ;
#endif
    ಯದಿ (!curbuf->b_p_ma && !p_im)
    {
	// Only give this error when 'insertmode' is off.
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (eap->forceit)
    {
	// cursor line can be zero on startup
	ಯದಿ (!curwin->w_cursor.lnum)
	    curwin->w_cursor.lnum = 1;
	set_cursor_for_append_to_ಸಾಲು();
    }

    // Ignore the command when already in Insert mode.  Inserting an
    // expression register that invokes a function can do this.
    ಯದಿ (State & MODE_ಸೇರಿಸು)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (eap->cmdidx == CMD_startinsert)
	restart_edit = 'a';
    ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_startreplace)
	restart_edit = 'R';
    ಅನ್ಯಥಾ
	restart_edit = 'V';

    ಯದಿ (!eap->forceit)
    {
	ಯದಿ (eap->cmdidx == CMD_startinsert)
	    restart_edit = 'i';
	curwin->w_curswant = 0;	    // avoid MAXCOL
    }

    ಯದಿ (VIsual_active)
	showmode();
}

/*
 * ":stopinsert"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_stopinsert(exarg_T *eap UNUSED)
{
    restart_edit = 0;
    stop_ಸೇರಿಸು_mode = TRUE;
#ಯದಿ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // when called from remote_expr in insert mode, make sure insert mode is
    // ended by adding K_NOP to the typeahead buffer
    ಯದಿ (vgetc_busy)
	ins_char_typebuf(K_NOP, 0);
#endif
    clearmode();
}

/*
 * Execute normal mode command "cmd".
 * "remap" can be REMAP_NONE or REMAP_YES.
 */
    ಶೂನ್ಯ
exec_normal_cmd(char_u *cmd, ಪೂರ್ಣಾಙ್ಕ remap, ಪೂರ್ಣಾಙ್ಕ silent)
{
    // Stuff the argument into the typeahead buffer.
    ins_typebuf(cmd, remap, 0, TRUE, silent);
    exec_normal(FALSE, FALSE, FALSE);
}

/*
 * Execute normal_cmd() until there is no typeahead left.
 * When "use_vpeekc" is TRUE use vpeekc() to check for available chars.
 */
    ಶೂನ್ಯ
exec_normal(ಪೂರ್ಣಾಙ್ಕ was_typed, ಪೂರ್ಣಾಙ್ಕ use_vpeekc, ಪೂರ್ಣಾಙ್ಕ may_use_terminal_loop UNUSED)
{
    oparg_T	oa;
    ಪೂರ್ಣಾಙ್ಕ		c;

    // When calling vpeekc() from feedkeys() it will return Ctrl_C when there
    // is nothing to get, so also check for Ctrl_C.
    clear_oparg(&oa);
    finish_op = FALSE;
    ಯಾವತ್ ((!stuff_empty()
		|| ((was_typed || !typebuf_typed()) && typebuf.tb_len > 0)
		|| (use_vpeekc && (c = vpeekc()) != NUL && c != Ctrl_C))
	    && !got_int)
    {
	update_topline_cursor();
#ifdef FEAT_TERMINAL
	ಯದಿ (may_use_terminal_loop && term_use_loop()
		&& oa.op_type == OP_NOP && oa.regname == NUL
		&& !VIsual_active)
	{
	    // If terminal_loop() returns OK we got a key that is handled
	    // in Normal model.  With FAIL we first need to position the
	    // cursor and the screen needs to be redrawn.
	    ಯದಿ (terminal_loop(TRUE) == OK)
		normal_cmd(&oa, TRUE);
	}
	ಅನ್ಯಥಾ
#endif
	    // execute a Normal mode cmd
	    normal_cmd(&oa, TRUE);
    }
}

#ifdef FEAT_FIND_ID
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_checkpath(exarg_T *eap)
{
    find_pattern_in_path(ರಿಕ್ತ, 0, 0, FALSE, FALSE, CHECK_PATH, 1L,
	    eap->forceit ? ACTION_SHOW_ALL : ACTION_SHOW,
	    (linenr_T)1, (linenr_T)MAXLNUM, eap->forceit, FALSE);
}

# ಯದಿ defined(FEAT_QUICKFIX)
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
    ಪೂರ್ಣಾಙ್ಕ		whole = TRUE;
    long	n;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		action;

    ವಿಕಲ್ಪ (cmdnames[eap->cmdidx].cmd_name[2])
    {
	ಪಕ್ಷ 'e':	// ":psearch", ":isearch" and ":dsearch"
		ಯದಿ (cmdnames[eap->cmdidx].cmd_name[0] == 'p')
		    action = ACTION_GOTO;
		ಅನ್ಯಥಾ
		    action = ACTION_SHOW;
		ಭಙ್ಗ;
	ಪಕ್ಷ 'i':	// ":ilist" and ":dlist"
		action = ACTION_SHOW_ALL;
		ಭಙ್ಗ;
	ಪಕ್ಷ 'u':	// ":ijump" and ":djump"
		action = ACTION_GOTO;
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:	// ":isplit" and ":dsplit"
		action = ACTION_SPLIT;
		ಭಙ್ಗ;
    }

    n = 1;
    ಯದಿ (vim_isdigit(*eap->arg))	// get count
    {
	n = getdigits(&eap->arg);
	eap->arg = skipwhite(eap->arg);
    }
    ಯದಿ (*eap->arg == '/')   // Match regexp, not just whole words
    {
	whole = FALSE;
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	ಯದಿ (*p)
	{
	    *p++ = NUL;
	    p = skipwhite(p);

	    // Check for trailing illegal characters
	    ಯದಿ (!ends_excmd2(eap->arg, p))
		eap->errmsg = ex_errmsg(e_trailing_characters_str, p);
	    ಅನ್ಯಥಾ
		set_nextcmd(eap, p);
	}
    }
    ಯದಿ (!eap->skip)
	find_pattern_in_path(eap->arg, 0, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(eap->arg),
		whole, !eap->forceit,
		*eap->cmd == 'd' ? FIND_DEFINE : FIND_ANY, n, action,
		eap->line1, eap->line2, eap->forceit, FALSE);
}
#endif


#ifdef FEAT_QUICKFIX
/*
 * ":ptag", ":ptselect", ":ptjump", ":ptnext", etc.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_ptag(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;  // will be reset to 0 in ex_tag_cmd()
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
}

/*
 * ":pedit"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_pedit(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Edit the file.
    do_exedit(eap, ರಿಕ್ತ);

    back_to_current_window(curwin_save);
}

/*
 * ":pbuffer"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_pbuffer(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Go to the buffer.
    do_exbuffer(eap);

    back_to_current_window(curwin_save);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prepare_preview_window(ಶೂನ್ಯ)
{
    ಯದಿ (ERROR_IF_ANY_POPUP_WINDOW)
	ಪ್ರತ್ಯಾಗಮ;

    // Open the preview window or popup and make it the current window.
    g_do_tagpreview = p_pvh;
    prepare_tagpreview(TRUE, TRUE, FALSE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
back_to_current_window(win_T *curwin_save)
{
    ಯದಿ (curwin != curwin_save && win_valid(curwin_save))
    {
	// Return cursor to where we were
	validate_cursor();
	redraw_later(UPD_VALID);
	win_enter(curwin_save, TRUE);
    }
# ifdef FEAT_PROP_POPUP
    ಅನ್ಯಥಾ ಯದಿ (WIN_IS_POPUP(curwin))
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
    ಸ್ಥಿರ ಶೂನ್ಯ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_tag(exarg_T *eap)
{
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_tag_cmd(exarg_T *eap, char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ		cmd;

    ವಿಕಲ್ಪ (name[1])
    {
	ಪಕ್ಷ 'j': cmd = DT_JUMP;	// ":tjump"
		  ಭಙ್ಗ;
	ಪಕ್ಷ 's': cmd = DT_SELECT;	// ":tselect"
		  ಭಙ್ಗ;
	ಪಕ್ಷ 'p': cmd = DT_PREV;	// ":tprevious"
		  ಭಙ್ಗ;
	ಪಕ್ಷ 'N': cmd = DT_PREV;	// ":tNext"
		  ಭಙ್ಗ;
	ಪಕ್ಷ 'n': cmd = DT_NEXT;	// ":tnext"
		  ಭಙ್ಗ;
	ಪಕ್ಷ 'o': cmd = DT_ತೆಗೆ;		// ":pop"
		  ಭಙ್ಗ;
	ಪಕ್ಷ 'f':			// ":tfirst"
	ಪಕ್ಷ 'r': cmd = DT_FIRST;	// ":trewind"
		  ಭಙ್ಗ;
	ಪಕ್ಷ 'l': cmd = DT_LAST;	// ":tlast"
		  ಭಙ್ಗ;
	ಸ್ವಭಾವ:			// ":tag"
#ifdef FEAT_CSCOPE
		  ಯದಿ (p_cst && *eap->arg != NUL)
		  {
		      ex_cstag(eap);
		      ಪ್ರತ್ಯಾಗಮ;
		  }
#endif
		  cmd = DT_TAG;
		  ಭಙ್ಗ;
    }

    ಯದಿ (name[0] == 'l')
    {
#ifndef FEAT_QUICKFIX
	ex_ni(eap);
	ಪ್ರತ್ಯಾಗಮ;
#ಅನ್ಯಥಾ
	cmd = DT_LTAG;
#endif
    }

    do_tag(eap->arg, cmd, eap->addr_ಎಣಿಕೆ > 0 ? (ಪೂರ್ಣಾಙ್ಕ)eap->line2 : 1,
							  eap->forceit, TRUE);
}

ಗಣನಾ {
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
#ifdef FEAT_CLIENTSERVER
    , SPEC_CLIENT
#endif
};

/*
 * Check "str" for starting with a special cmdline variable.
 * If found return one of the SPEC_ values and set "*usedlen" to the length of
 * the variable.  Otherwise return -1 and "*usedlen" is unchanged.
 */
    ಪೂರ್ಣಾಙ್ಕ
find_cmdline_var(char_u *src, size_t *usedlen)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    ಸ್ಥಿರ keyvalue_T spec_str_tab[] = {
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
	KEYVALUE_ENTRY(SPEC_ರಾಶಿ, "stack>")	    // call stack
    };
    keyvalue_T target;
    keyvalue_T *entry;

    ವಿಕಲ್ಪ (*src)
    {
    ಪಕ್ಷ '%':
	*usedlen = 1;
	ಪ್ರತ್ಯಾಗಮ SPEC_PERC;

    ಪಕ್ಷ '#':
	*usedlen = 1;
	ಪ್ರತ್ಯಾಗಮ SPEC_HASH;

    ಪಕ್ಷ '<':
	target.ಕೀಲಿ = 0;
	target.ಮೌಲ್ಯ.string = src + 1;	    // skip over '<'
	target.ಮೌಲ್ಯ.length = 0;	    // not used, see cmp_keyvalue_value_n()

	entry = (keyvalue_T *)bsearch(&target, &spec_str_tab,
		ARRAY_LENGTH(spec_str_tab), ಪರಿಮಾಣ(spec_str_tab[0]),
		cmp_keyvalue_ಮೌಲ್ಯ_n);
	ಯದಿ (entry == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;

	*usedlen = entry->ಮೌಲ್ಯ.length + 1;
	ಪ್ರತ್ಯಾಗಮ entry->ಕೀಲಿ;

    ಸ್ವಭಾವ:
	ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ -1;
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
    ವರ್ಣ	**errormsg,	// pointer to error message
    ಪೂರ್ಣಾಙ್ಕ		*escaped,	// return value has escaped white space (can
				// be NULL)
    ಪೂರ್ಣಾಙ್ಕ		empty_is_error)	// empty result is considered an error
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*s;
    char_u	*result = (char_u *)"";
    char_u	*resultbuf = ರಿಕ್ತ;
    size_t	resultlen;
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		valid = VALID_HEAD + VALID_PATH;    // assume valid result
    ಪೂರ್ಣಾಙ್ಕ		spec_idx;
    ಪೂರ್ಣಾಙ್ಕ		tilde_file = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		skip_mod = FALSE;
    char_u	strbuf[30];

    *errormsg = ರಿಕ್ತ;
    ಯದಿ (escaped != ರಿಕ್ತ)
	*escaped = FALSE;

    /*
     * Check if there is something to do.
     */
    spec_idx = find_cmdline_var(src, usedlen);
    ಯದಿ (spec_idx < 0)	// no match
    {
	*usedlen = 1;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    /*
     * Skip when preceded with a backslash "\%" and "\#".
     * Note: In "\\%" the % is also not recognized!
     */
    ಯದಿ (src > srcstart && src[-1] == '\\')
    {
	*usedlen = 0;
	STRMOVE(src - 1, src);	// remove backslash
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    /*
     * word or WORD under cursor
     */
    ಯದಿ (spec_idx == SPEC_CWORD || spec_idx == SPEC_CCWORD
						     || spec_idx == SPEC_CEXPR)
    {
	resultlen = find_ident_under_cursor(&result,
		spec_idx == SPEC_CWORD ? (FIND_IDENT | FIND_STRING)
	      : spec_idx == SPEC_CEXPR ? (FIND_IDENT | FIND_STRING | FIND_EVAL)
	      : FIND_STRING);
	ಯದಿ (resultlen == 0)
	{
	    *errormsg = "";
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }

    /*
     * '#': Alternate file name
     * '%': Current file name
     *	    File name under the cursor
     *	    File name for autocommand
     *	and following modifiers
     */
    ಅನ್ಯಥಾ
    {
	size_t off = 0;

	ವಿಕಲ್ಪ (spec_idx)
	{
	ಪಕ್ಷ SPEC_PERC:
#ifdef FEAT_EVAL
		ಯದಿ (!in_vim9script() || src[1] != '%')
#endif
		{
		    // '%': current file
		    ಯದಿ (curbuf->b_fname == ರಿಕ್ತ)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    ಅನ್ಯಥಾ
		    {
			result = curbuf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		    ಭಙ್ಗ;
		}
#ifdef FEAT_EVAL
		// "%%" alternate file
		off = 1;
#endif
		// FALLTHROUGH
	ಪಕ್ಷ SPEC_HASH:		// '#' or "#99": alternate file
		ಯದಿ (off == 0 ? src[1] == '#' : src[2] == '%')
		{
		    // "##" or "%%%": the argument list
		    result = arg_all();
		    resultbuf = result;
		    *usedlen = off + 2;
		    ಯದಿ (escaped != ರಿಕ್ತ)
			*escaped = TRUE;
		    skip_mod = TRUE;
		    ಭಙ್ಗ;
		}
		s = src + off + 1;
		ಯದಿ (*s == '<')		// "#<99" uses v:oldfiles
		    ++s;
		i = (ಪೂರ್ಣಾಙ್ಕ)getdigits(&s);
		ಯದಿ (s == src + off + 2 && src[off + 1] == '-')
		    // just a minus sign, don't skip over it
		    s--;
		*usedlen = (ಪೂರ್ಣಾಙ್ಕ)(s - src); // length of what we expand

		ಯದಿ (src[off + 1] == '<' && i != 0)
		{
		    ಯದಿ (*usedlen < off + 2)
		    {
			// Should we give an error message for #<text?
			*usedlen = off + 1;
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		    }
#ifdef FEAT_EVAL
		    result = list_find_str(get_vim_var_list(VV_OLDFILES),
								     (long)i);
		    ಯದಿ (result == ರಿಕ್ತ)
		    {
			*errormsg = "";
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		    }
#ಅನ್ಯಥಾ
		    *errormsg = _(e_hashsmall_is_not_available_without_the_eval_feature);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#endif
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (i == 0 && src[off + 1] == '<' && *usedlen > off + 1)
			*usedlen = off + 1;
		    buf = buflist_findnr(i);
		    ಯದಿ (buf == ರಿಕ್ತ)
		    {
			*errormsg = _(e_no_alternate_file_name_to_substitute_for_hash);
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		    }
		    ಯದಿ (lnump != ರಿಕ್ತ)
			*lnump = ECMD_LAST;
		    ಯದಿ (buf->b_fname == ರಿಕ್ತ)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    ಅನ್ಯಥಾ
		    {
			result = buf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		}
		ಭಙ್ಗ;

	ಪಕ್ಷ SPEC_CFILE:	// file name under cursor
		result = file_name_at_cursor(FNAME_MESS|FNAME_HYP, 1L, ರಿಕ್ತ);
		ಯದಿ (result == ರಿಕ್ತ)
		{
		    *errormsg = "";
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		resultbuf = result;	    // remember allocated string
		ಭಙ್ಗ;

	ಪಕ್ಷ SPEC_AFILE:	// file name for autocommand
		result = autocmd_fname;
		ಯದಿ (result != ರಿಕ್ತ && !autocmd_fname_full)
		{
		    // Still need to turn the fname into a full path.  It is
		    // postponed to avoid a delay when <afile> is not used.
		    autocmd_fname_full = TRUE;
		    result = FullName_save(autocmd_fname, FALSE);
		    vim_free(autocmd_fname);
		    autocmd_fname = result;
		}
		ಯದಿ (result == ರಿಕ್ತ)
		{
		    *errormsg = _(e_no_autocommand_file_name_to_substitute_for_afile);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		result = shorten_fname1(result);
		ಭಙ್ಗ;

	ಪಕ್ಷ SPEC_ABUF:		// buffer number for autocommand
		ಯದಿ (autocmd_bufnr <= 0)
		{
		    *errormsg = _(e_no_autocommand_buffer_ಸಂಖ್ಯೆ_to_substitute_for_abuf);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		sprintf((ವರ್ಣ *)strbuf, "%d", autocmd_bufnr);
		result = strbuf;
		ಭಙ್ಗ;

	ಪಕ್ಷ SPEC_AMATCH:	// match name for autocommand
		result = autocmd_match;
		ಯದಿ (result == ರಿಕ್ತ)
		{
		    *errormsg = _(e_no_autocommand_match_name_to_substitute_for_amatch);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		ಭಙ್ಗ;

	ಪಕ್ಷ SPEC_SFILE:	// file name for ":so" command
		result = estack_sfile(ESTACK_SFILE);
		ಯದಿ (result == ರಿಕ್ತ)
		{
		    *errormsg = _(e_no_source_file_name_to_substitute_for_sfile);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		resultbuf = result;	    // remember allocated string
		ಭಙ್ಗ;
	ಪಕ್ಷ SPEC_ರಾಶಿ:	// call stack
		result = estack_sfile(ESTACK_ರಾಶಿ);
		ಯದಿ (result == ರಿಕ್ತ)
		{
		    *errormsg = _(e_no_call_ರಾಶಿ_to_substitute_for_ರಾಶಿ);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		resultbuf = result;	    // remember allocated string
		ಭಙ್ಗ;
	ಪಕ್ಷ SPEC_SCRIPT:	// script file name
		result = estack_sfile(ESTACK_SCRIPT);
		ಯದಿ (result == ರಿಕ್ತ)
		{
		    *errormsg = _(e_no_script_file_name_to_substitute_for_script);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		resultbuf = result;	    // remember allocated string
		ಭಙ್ಗ;

	ಪಕ್ಷ SPEC_SLNUM:	// line in file for ":so" command
		ಯದಿ (SOURCING_NAME == ರಿಕ್ತ || SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_ಸಾಲು_ಸಂಖ್ಯೆ_to_use_for_slnum);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		sprintf((ವರ್ಣ *)strbuf, "%ld", SOURCING_LNUM);
		result = strbuf;
		ಭಙ್ಗ;

#ifdef FEAT_EVAL
	ಪಕ್ಷ SPEC_SFLNUM:	// line in script file
		ಯದಿ (current_sctx.sc_lnum + SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_ಸಾಲು_ಸಂಖ್ಯೆ_to_use_for_sflnum);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		sprintf((ವರ್ಣ *)strbuf, "%ld",
				 (long)(current_sctx.sc_lnum + SOURCING_LNUM));
		result = strbuf;
		ಭಙ್ಗ;

	ಪಕ್ಷ SPEC_SID:
		ಯದಿ (current_sctx.sc_sid <= 0)
		{
		    *errormsg = _(e_using_sid_not_in_script_context);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		sprintf((ವರ್ಣ *)strbuf, "<SNR>%d_", current_sctx.sc_sid);
		result = strbuf;
		ಭಙ್ಗ;
#endif

#ifdef FEAT_CLIENTSERVER
	ಪಕ್ಷ SPEC_CLIENT:	// Source of last submitted input
# ifdef MSWIN
		sprintf((ವರ್ಣ *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		result = strbuf;
# ಅನ್ಯಥಾ
#  ifdef FEAT_SOCKETSERVER
		ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		{
		    ಯದಿ (client_socket == ರಿಕ್ತ)
			result = (char_u *)"";
		    ಅನ್ಯಥಾ
			result = client_socket;
		}
#  endif
#  ifdef FEAT_X11
		ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_X11)
		{
		    sprintf((ವರ್ಣ *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		    result = strbuf;
		}
#  endif
# endif
		ಭಙ್ಗ;
#endif
	}

	resultlen = STRLEN(result);	// length of new string
	ಯದಿ (src[*usedlen] == '<')	// remove the file name extension
	{
	    ++*usedlen;
	    ಯದಿ ((s = vim_strrchr(result, '.')) != ರಿಕ್ತ && s >= gettail(result))
		resultlen = s - result;
	}
	ಅನ್ಯಥಾ ಯದಿ (!skip_mod)
	{
	    valid |= modify_fname(src, tilde_file, usedlen, &result, &resultbuf,
								  &resultlen);
	    ಯದಿ (result == ರಿಕ್ತ)
	    {
		*errormsg = "";
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	}
    }

    ಯದಿ (resultlen == 0 || valid != VALID_HEAD + VALID_PATH)
    {
	ಯದಿ (empty_is_error)
	{
	    ಯದಿ (valid != VALID_HEAD + VALID_PATH)
		*errormsg = _(e_empty_file_name_for_percent_or_hash_only_works_with_ph);
	    ಅನ್ಯಥಾ
		*errormsg = _(e_evaluates_to_an_empty_string);
	}
	result = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
	result = vim_strnsave(result, resultlen);
    vim_free(resultbuf);
    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Expand the <sfile> string in "arg".
 *
 * Returns an allocated string, or NULL for any error.
 */
    char_u *
expand_sfile(char_u *arg)
{
    ವರ್ಣ	*errormsg;
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
    ಯದಿ (result == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (p = result; *p; )
    {
	ಯದಿ (STRNCMP(p, "<sfile>", 7) != 0)
	    ++p;
	ಅನ್ಯಥಾ
	{
	    // replace "<sfile>" with the sourced file name, and do ":" stuff
	    repl = eval_vars(p, result, &srclen, ರಿಕ್ತ, &errormsg, ರಿಕ್ತ, TRUE);
	    ಯದಿ (errormsg != ರಿಕ್ತ)
	    {
		ಯದಿ (*errormsg)
		    emsg(errormsg);
		vim_free(result);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    ಯದಿ (repl == ರಿಕ್ತ)		// no match (cannot happen)
	    {
		p += srclen;
		ಅನುವರ್ತ;
	    }
	    repllen = STRLEN(repl);
	    resultlen += repllen - srclen;
	    newres = alloc(resultlen + 1);
	    ಯದಿ (newres == ರಿಕ್ತ)
	    {
		vim_free(repl);
		vim_free(result);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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

    ಪ್ರತ್ಯಾಗಮ result;
}

#ಯದಿ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
/*
 * Make a dialog message in "buff[DIALOG_MSG_SIZE]".
 * "format" must contain "%s".
 */
    ಶೂನ್ಯ
dialog_ಸಂದೇಶ(char_u *buff, ವರ್ಣ *format, char_u *fname)
{
    ಯದಿ (fname == ರಿಕ್ತ)
	fname = (char_u *)_("Untitled");
    vim_snprintf((ವರ್ಣ *)buff, DIALOG_ಸಂದೇಶ_SIZE, format, fname);
}
#endif

/*
 * ":behave {mswin,xterm}"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_behave(exarg_T *eap)
{
    ಯದಿ (STRCMP(eap->arg, "mswin") == 0)
    {
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"selection",
						 0L, (char_u *)"exclusive", 0);
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"selectmode",
						 0L, (char_u *)"mouse,key", 0);
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"mousemodel",
						     0L, (char_u *)"popup", 0);
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"keymodel",
					  0L, (char_u *)"startsel,stopsel", 0);
    }
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(eap->arg, "xterm") == 0)
    {
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"selection",
						 0L, (char_u *)"inclusive", 0);
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"selectmode", 0L, (char_u *)"", 0);
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"mousemodel",
						    0L, (char_u *)"extend", 0);
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"keymodel", 0L, (char_u *)"", 0);
    }
    ಅನ್ಯಥಾ
	semsg(_(e_invalid_argument_str), eap->arg);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ filetype_detect = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ filetype_plugin = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ filetype_indent = FALSE;

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
    ಪೂರ್ಣಾಙ್ಕ		plugin = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		indent = FALSE;

    ಯದಿ (*eap->arg == NUL)
    {
	// Print current status.
	smsg("filetype detection:%s  plugin:%s  indent:%s",
		filetype_detect ? "ON" : "OFF",
		filetype_plugin ? (filetype_detect ? "ON" : "(on)") : "OFF",
		filetype_indent ? (filetype_detect ? "ON" : "(on)") : "OFF");
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Accept "plugin" and "indent" in any order.
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (STRNCMP(arg, "plugin", 6) == 0)
	{
	    plugin = TRUE;
	    arg = skipwhite(arg + 6);
	    ಅನುವರ್ತ;
	}
	ಯದಿ (STRNCMP(arg, "indent", 6) == 0)
	{
	    indent = TRUE;
	    arg = skipwhite(arg + 6);
	    ಅನುವರ್ತ;
	}
	ಭಙ್ಗ;
    }
    ಯದಿ (STRCMP(arg, "on") == 0 || STRCMP(arg, "detect") == 0)
    {
	ಯದಿ (*arg == 'o' || !filetype_detect)
	{
	    source_runtime((char_u *)FILETYPE_FILE, DIP_ALL);
	    filetype_detect = TRUE;
	    ಯದಿ (plugin)
	    {
		source_runtime((char_u *)FTPLUGIN_FILE, DIP_ALL);
		filetype_plugin = TRUE;
	    }
	    ಯದಿ (indent)
	    {
		source_runtime((char_u *)INDENT_FILE, DIP_ALL);
		filetype_indent = TRUE;
	    }
	}
	ಯದಿ (*arg == 'd')
	{
	    (ಶೂನ್ಯ)do_doautocmd((char_u *)"filetypedetect BufRead", TRUE, ರಿಕ್ತ);
	    do_modelines(0);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(arg, "off") == 0)
    {
	ಯದಿ (plugin || indent)
	{
	    ಯದಿ (plugin)
	    {
		source_runtime((char_u *)FTPLUGOF_FILE, DIP_ALL);
		filetype_plugin = FALSE;
	    }
	    ಯದಿ (indent)
	    {
		source_runtime((char_u *)INDOFF_FILE, DIP_ALL);
		filetype_indent = FALSE;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    source_runtime((char_u *)FTOFF_FILE, DIP_ALL);
	    filetype_detect = FALSE;
	}
    }
    ಅನ್ಯಥಾ
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * ":setfiletype [FALLBACK] {name}"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_setfiletype(exarg_T *eap)
{
    ಯದಿ (curbuf->b_did_filetype)
	ಪ್ರತ್ಯಾಗಮ;

    char_u *arg = eap->arg;
    ಯದಿ (STRNCMP(arg, "FALLBACK ", 9) == 0)
	arg += 9;

    set_option_ಮೌಲ್ಯ_give_err((char_u *)"filetype", 0L, arg, OPT_LOCAL);
    ಯದಿ (arg != eap->arg)
	curbuf->b_did_filetype = false;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ex_digraphs(exarg_T *eap UNUSED)
{
#ifdef FEAT_DIGRAPHS
    ಯದಿ (*eap->arg != NUL)
	putdigraph(eap->arg);
    ಅನ್ಯಥಾ
	listdigraphs(eap->forceit);
#ಅನ್ಯಥಾ
    emsg(_(e_no_digraphs_version));
#endif
}

#ಯದಿ defined(FEAT_ಹುಡುಕು_EXTRA)
    ಶೂನ್ಯ
set_no_hlsearch(ಪೂರ್ಣಾಙ್ಕ flag)
{
    no_hlsearch = flag;
# ifdef FEAT_EVAL
    set_vim_var_nr(VV_HLSEARCH, !no_hlsearch && p_hls);
# endif
}

/*
 * ":nohlsearch"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_X(exarg_T *eap UNUSED)
{
    crypt_check_current_method();
    (ಶೂನ್ಯ)crypt_get_ಕೀಲಿ(TRUE, TRUE);
}
#endif

#ifdef FEAT_FOLDING
    ಸ್ಥಿರ ಶೂನ್ಯ
ex_fold(exarg_T *eap)
{
    ಯದಿ (foldManualAllowed(TRUE))
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
    start_global_changes();
# endif
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
# endif

    // First set the marks for all lines closed/open.
    ಪ್ರತಿ (lnum = eap->line1; lnum <= eap->line2; ++lnum)
	ಯದಿ (hasFolding(lnum, ರಿಕ್ತ, ರಿಕ್ತ) == (eap->cmdidx == CMD_folddoclosed))
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

#ಯದಿ defined(FEAT_QUICKFIX)
/*
 * Returns TRUE if the supplied Ex cmdidx is for a location list command
 * instead of a quickfix command.
 */
    ಪೂರ್ಣಾಙ್ಕ
is_loclist_cmd(ಪೂರ್ಣಾಙ್ಕ cmdidx)
{
    ಯದಿ (cmdidx < 0 || cmdidx >= CMD_SIZE)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ cmdnames[cmdidx].cmd_name[0] == 'l';
}
#endif

    ಪೂರ್ಣಾಙ್ಕ
get_pressedreturn(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ ex_pressedreturn;
}

    ಶೂನ್ಯ
set_pressedreturn(ಪೂರ್ಣಾಙ್ಕ val)
{
    ex_pressedreturn = val;
}
