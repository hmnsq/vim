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

#ଅନ୍ତର୍ଭାଵ "vim.h"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	quitmore = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ex_pressedreturn = FALSE;
#ifndef FEAT_PRINTER
# define ex_hardcopy	ex_ni
#endif

#ifdef FEAT_EVAL
ସ୍ଥିର char_u	*do_one_cmd(char_u **, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, cstack_T *, char_u *(*fgetline)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, getline_opt_T), ଶୂନ୍ୟ *cookie);
#ଅନ୍ୟଥା
ସ୍ଥିର char_u	*do_one_cmd(char_u **, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, char_u *(*fgetline)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, getline_opt_T), ଶୂନ୍ୟ *cookie);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	if_level = 0;		// depth in :if
#endif
ସ୍ଥିର ଶୂନ୍ୟ	append_command(char_u *cmd);

#ifndef FEAT_MENU
# define ex_emenu		ex_ni
# define ex_menu		ex_ni
# define ex_menutranslate	ex_ni
#endif
ସ୍ଥିର ଶୂନ୍ୟ	ex_autocmd(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_doautocmd(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_bunload(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_buffer(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	do_exbuffer(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_bmodified(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_bnext(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_bprevious(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_brewind(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_blast(exarg_T *eap);
ସ୍ଥିର char_u	*getargcmd(char_u **);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	getargopt(exarg_T *eap);
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
#ଯଦି !defined(FEAT_QUICKFIX) || !defined(FEAT_EVAL)
# define ex_cexpr		ex_ni
#endif

ସ୍ଥିର linenr_T default_address(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ address_default_all(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	get_flags(exarg_T *eap);
#ଯଦି !defined(FEAT_PERL) \
	|| !defined(FEAT_PYTHON) || !defined(FEAT_PYTHON3) \
	|| !defined(FEAT_TCL) \
	|| !defined(FEAT_RUBY) \
	|| !defined(FEAT_LUA) \
	|| !defined(FEAT_MZSCHEME)
# define HAVE_EX_SCRIPT_NI
ସ୍ଥିର ଶୂନ୍ୟ	ex_script_ni(exarg_T *eap);
#endif
ସ୍ଥିର ଅକ୍ଷର	*invalid_range(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	correct_range(exarg_T *eap);
#ifdef FEAT_QUICKFIX
ସ୍ଥିର char_u	*replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep);
#endif
ସ୍ଥିର char_u	*repl_cmdline(exarg_T *eap, char_u *src, size_t srclen, char_u *repl, char_u **cmdlinep);
ସ୍ଥିର ଶୂନ୍ୟ	ex_highlight(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_colorscheme(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_cquit(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_quit_all(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_close(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_win_close(ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit, win_T *win, tabpage_T *tp);
ସ୍ଥିର ଶୂନ୍ୟ	ex_only(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_resize(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_stag(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_tabclose(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_tabonly(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_tabnext(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_tabmove(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_tabs(exarg_T *eap);
#ଯଦି defined(FEAT_QUICKFIX)
ସ୍ଥିର ଶୂନ୍ୟ	ex_pclose(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_ptag(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_pedit(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_pbuffer(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	prepare_preview_window(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ	back_to_current_window(win_T *curwin_save);
#ଅନ୍ୟଥା
# define ex_pclose		ex_ni
# define ex_ptag		ex_ni
# define ex_pedit		ex_ni
# define ex_pbuffer		ex_ni
#endif
ସ୍ଥିର ଶୂନ୍ୟ	ex_hide(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_exit(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_print(exarg_T *eap);
#ifdef FEAT_BYTEOFF
ସ୍ଥିର ଶୂନ୍ୟ	ex_goto(exarg_T *eap);
#ଅନ୍ୟଥା
# define ex_goto		ex_ni
#endif
ସ୍ଥିର ଶୂନ୍ୟ	ex_shell(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_preserve(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_recover(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_mode(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_wrongmodifier(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_find(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_open(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_edit(exarg_T *eap);
#ifndef FEAT_GUI
# define ex_gui			ex_nogui
ସ୍ଥିର ଶୂନ୍ୟ	ex_nogui(exarg_T *eap);
#endif
#ଯଦି defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
ସ୍ଥିର ଶୂନ୍ୟ	ex_tearoff(exarg_T *eap);
#ଅନ୍ୟଥା
# define ex_tearoff		ex_ni
#endif
#ଯଦି (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
ସ୍ଥିର ଶୂନ୍ୟ	ex_popup(exarg_T *eap);
#ଅନ୍ୟଥା
# define ex_popup		ex_ni
#endif
#ifndef FEAT_GUI_MSWIN
# define ex_simalt		ex_ni
#endif
#ଯଦି !defined(FEAT_GUI_MSWIN) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
# define gui_mch_find_dialog	ex_ni
# define gui_mch_replace_dialog ex_ni
#endif
#ଯଦି !defined(FEAT_GUI_GTK)
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
#ଯଦି !defined(FEAT_SYN_HL) || !defined(FEAT_PROFILE)
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
#ଯଦି !defined(FEAT_PYTHON) && !defined(FEAT_PYTHON3)
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
ସ୍ଥିର ଶୂନ୍ୟ	ex_swapname(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_syncbind(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_read(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_pwd(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_equal(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_sleep(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_winsize(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_wincmd(exarg_T *eap);
#ଯଦି defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
ସ୍ଥିର ଶୂନ୍ୟ	ex_winpos(exarg_T *eap);
#ଅନ୍ୟଥା
# define ex_winpos	    ex_ni
#endif
ସ୍ଥିର ଶୂନ୍ୟ	ex_operators(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_put(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_iput(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_copymove(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_submagic(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_join(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_at(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_bang(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_undo(exarg_T *eap);
#ifdef FEAT_PERSISTENT_UNDO
ସ୍ଥିର ଶୂନ୍ୟ	ex_wundo(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_rundo(exarg_T *eap);
#endif
ସ୍ଥିର ଶୂନ୍ୟ	ex_redo(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_later(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_redir(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_redrawstatus(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_redrawtabline(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_redrawtabpanel(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	close_redir(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ	ex_mark(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_startinsert(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_stopinsert(exarg_T *eap);
#ifdef FEAT_FIND_ID
ସ୍ଥିର ଶୂନ୍ୟ	ex_checkpath(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_findpat(exarg_T *eap);
#ଅନ୍ୟଥା
# define ex_findpat		ex_ni
# define ex_checkpath		ex_ni
#endif
#ଯଦି defined(FEAT_FIND_ID) && defined(FEAT_QUICKFIX)
ସ୍ଥିର ଶୂନ୍ୟ	ex_psearch(exarg_T *eap);
#ଅନ୍ୟଥା
# define ex_psearch		ex_ni
#endif
ସ୍ଥିର ଶୂନ୍ୟ	ex_tag(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_tag_cmd(exarg_T *eap, char_u *name);
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
ସ୍ଥିର ଶୂନ୍ୟ	ex_behave(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_filetype(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_setfiletype(exarg_T *eap);
#ifndef FEAT_DIFF
# define ex_diffoff		ex_ni
# define ex_diffpatch		ex_ni
# define ex_diffgetput		ex_ni
# define ex_diffsplit		ex_ni
# define ex_diffthis		ex_ni
# define ex_diffupdate		ex_ni
#endif
ସ୍ଥିର ଶୂନ୍ୟ	ex_digraphs(exarg_T *eap);
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
ସ୍ଥିର ଶୂନ୍ୟ	ex_nohlsearch(exarg_T *eap);
#ଅନ୍ୟଥା
# define ex_nohlsearch		ex_ni
# define ex_match		ex_ni
#endif
#ifdef FEAT_CRYPT
ସ୍ଥିର ଶୂନ୍ୟ	ex_X(exarg_T *eap);
#ଅନ୍ୟଥା
# define ex_X			ex_ni
#endif
#ifdef FEAT_FOLDING
ସ୍ଥିର ଶୂନ୍ୟ	ex_fold(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_foldopen(exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ	ex_folddo(exarg_T *eap);
#ଅନ୍ୟଥା
# define ex_fold		ex_ni
# define ex_foldopen		ex_ni
# define ex_folddo		ex_ni
#endif
#ଯଦି !(defined(HAVE_LOCALE_H) || defined(X_LOCALE))
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
#ଯଦି !defined(FEAT_X11) || !defined(FEAT_XCLIPBOARD)
# define ex_xrestore		ex_ni
#endif
#ଯଦି !defined(FEAT_WAYLAND)
# define ex_wlrestore		ex_ni
#endif
#ଯଦି !defined(HAVE_CLIPMETHOD)
# define ex_clipreset		ex_ni
#endif
#ଯଦି !defined(FEAT_PROP_POPUP)
# define ex_popupclear		ex_ni
#endif

/*
 * Declare cmdnames[].
 */
#define DO_DECLARE_EXCMD
#ଅନ୍ତର୍ଭାଵ "ex_cmds.h"
#ଅନ୍ତର୍ଭାଵ "ex_cmdidxs.h"

ସ୍ଥିର char_u dollar_command[2] = {'$', 0};


#ifdef FEAT_EVAL
// Struct for storing a line inside a while/for loop
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    char_u	*ଧାଡ଼ି;		// command line
    linenr_T	lnum;		// sourcing_lnum of the line
} wcmd_T;

/*
 * Structure used to store info for line position in a while or for loop.
 * This is required, because do_one_cmd() may invoke ex_function(), which
 * reads more lines that may come from the while/for loop.
 */
ସଂରଚନା loop_cookie
{
    garray_T	*ଧାଡ଼ିଗୁଡ଼ିକ_gap;		// growarray with line info
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		current_ଧାଡ଼ି;		// last read line from growarray
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		repeating;		// TRUE when looping a second time
    // When "repeating" is FALSE use "getline" and "cookie" to get lines
    char_u	*(*lc_getline)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, getline_opt_T);
    ଶୂନ୍ୟ	*cookie;
};

ସ୍ଥିର char_u	*get_loop_ଧାଡ଼ି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ଶୂନ୍ୟ *cookie, ପୂର୍ଣ୍ଣସଂଖ୍ୟା indent, getline_opt_T options);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	store_loop_ଧାଡ଼ି(garray_T *gap, char_u *ଧାଡ଼ି);
ସ୍ଥିର ଶୂନ୍ୟ	free_cmdlines(garray_T *gap);

// Struct to save a few things while debugging.  Used in do_cmdline() only.
ସଂରଚନା dbg_stuff
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		trylevel;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		force_abort;
    except_T	*caught_ଗଦା;
    char_u	*vv_exception;
    char_u	*vv_throwpoint;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		got_int;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_throw;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_rethrow;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		check_cstack;
    except_T	*current_exception;
};

    ସ୍ଥିର ଶୂନ୍ୟ
save_dbg_stuff(ସଂରଚନା dbg_stuff *dsp)
{
    dsp->trylevel	= trylevel;		trylevel = 0;
    dsp->force_abort	= force_abort;		force_abort = FALSE;
    dsp->caught_ଗଦା	= caught_ଗଦା;		caught_ଗଦା = ଖାଲି;
    dsp->vv_exception	= v_exception(ଖାଲି);
    dsp->vv_throwpoint	= v_throwpoint(ଖାଲି);

    // Necessary for debugging an inactive ":catch", ":finally", ":endtry"
    dsp->did_emsg	= did_emsg;		did_emsg     = FALSE;
    dsp->got_int	= got_int;		got_int	     = FALSE;
    dsp->did_throw	= did_throw;		did_throw    = FALSE;
    dsp->need_rethrow	= need_rethrow;		need_rethrow = FALSE;
    dsp->check_cstack	= check_cstack;		check_cstack = FALSE;
    dsp->current_exception = current_exception;	current_exception = ଖାଲି;
}

    ସ୍ଥିର ଶୂନ୍ୟ
restore_dbg_stuff(ସଂରଚନା dbg_stuff *dsp)
{
    suppress_errthrow = FALSE;
    trylevel = dsp->trylevel;
    force_abort = dsp->force_abort;
    caught_ଗଦା = dsp->caught_ଗଦା;
    (ଶୂନ୍ୟ)v_exception(dsp->vv_exception);
    (ଶୂନ୍ୟ)v_throwpoint(dsp->vv_throwpoint);
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_other_file(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fnum, char_u *ffname)
{
    ଯଦି (fnum != 0)
    {
	ଯଦି (fnum == curbuf->b_fnum)
	    ଫେରନ୍ତୁ FALSE;

	ଫେରନ୍ତୁ TRUE;
    }

    ଯଦି (ffname == ଖାଲି)
	ଫେରନ୍ତୁ TRUE;

    ଯଦି (*ffname == NUL)
	ଫେରନ୍ତୁ FALSE;

    // TODO: Need a reliable way to know whether a buffer is meant to live
    // on-disk !curbuf->b_dev_valid is not always available (example: missing
    // on Windows)
    ଯଦି (curbuf->b_sfname != ଖାଲି
	    && *curbuf->b_sfname != NUL)
	// This occurs with unsaved buffers. In which case `ffname` actually
	// corresponds to curbuf->b_sfname
	ଫେରନ୍ତୁ fnamecmp(ffname, curbuf->b_sfname) != 0;

    ଫେରନ୍ତୁ otherfile(ffname);
}

/*
 * do_exmode(): Repeatedly get commands for the "Ex" mode, until the ":vi"
 * command is given.
 */
    ଶୂନ୍ୟ
do_exmode(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		improved)	    // TRUE for "improved Ex" mode
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ସନ୍ଦେଶ_scroll;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_ସନ୍ଦେଶ_row;
    linenr_T	prev_ଧାଡ଼ି;
    varnumber_T	changedtick;

    ଯଦି (improved)
	exmode_active = EXMODE_VIM;
    ଅନ୍ୟଥା
	exmode_active = EXMODE_NORMAL;
    State = MODE_NORMAL;
    may_trigger_modechanged();

    // When using ":global /pat/ visual" and then "Q" we return to continue
    // the :global command.
    ଯଦି (global_busy)
	ଫେରନ୍ତୁ;

    save_ସନ୍ଦେଶ_scroll = ସନ୍ଦେଶ_scroll;
    ++RedrawingDisabled;	    // don't redisplay the window
    ++no_wait_return;		    // don't wait for return
#ifdef FEAT_GUI
    // Ignore scrollbar and mouse events in Ex mode
    ++hold_gui_events;
#endif

    ସନ୍ଦେଶ(_("Entering Ex mode.  Type \"visual\" to go to Normal mode."));
    ଯେତେବେଳେ (exmode_active)
    {
	// Check for a ":normal" command and no more characters left.
	ଯଦି (ex_normal_busy > 0 && typebuf.tb_len == 0)
	{
	    exmode_active = FALSE;
	    ଭାଙ୍ଗ;
	}
	ସନ୍ଦେଶ_scroll = TRUE;
	need_wait_return = FALSE;
	ex_pressedreturn = FALSE;
	ex_no_reprint = FALSE;
	changedtick = CHANGEDTICK(curbuf);
	prev_ସନ୍ଦେଶ_row = ସନ୍ଦେଶ_row;
	prev_ଧାଡ଼ି = curwin->w_cursor.lnum;
	ଯଦି (improved)
	{
	    cmdline_row = ସନ୍ଦେଶ_row;
	    do_cmdline(ଖାଲି, getexline, ଖାଲି, 0);
	}
	ଅନ୍ୟଥା
	    do_cmdline(ଖାଲି, getexmodeline, ଖାଲି, DOCMD_NOWAIT);
	ଧାଡ଼ିଗୁଡ଼ିକ_ବାମ = Rows - 1;

	ଯଦି ((prev_ଧାଡ଼ି != curwin->w_cursor.lnum
		   || changedtick != CHANGEDTICK(curbuf)) && !ex_no_reprint)
	{
	    ଯଦି (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    ଅନ୍ୟଥା
	    {
		ଯଦି (ex_pressedreturn)
		{
		    // go up one line, to overwrite the ":<CR>" line, so the
		    // output doesn't contain empty lines.
		    ସନ୍ଦେଶ_row = prev_ସନ୍ଦେଶ_row;
		    ଯଦି (prev_ସନ୍ଦେଶ_row == Rows - 1)
			ସନ୍ଦେଶ_row--;
		}
		ସନ୍ଦେଶ_col = 0;
		print_ଧାଡ଼ି_no_prefix(curwin->w_cursor.lnum, FALSE, FALSE);
		ସନ୍ଦେଶ_clr_eos();
	    }
	}
	ଅନ୍ୟଥା ଯଦି (ex_pressedreturn && !ex_no_reprint)	// must be at EOF
	{
	    ଯଦି (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    ଅନ୍ୟଥା
		emsg(_(e_at_end_of_file));
	}
    }

#ifdef FEAT_GUI
    --hold_gui_events;
#endif
    ଯଦି (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    update_screen(UPD_CLEAR);
    need_wait_return = FALSE;
    ସନ୍ଦେଶ_scroll = save_ସନ୍ଦେଶ_scroll;
}

/*
 * Print the executed command for when 'verbose' is set.
 * When "lnum" is 0 only print the command.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ସନ୍ଦେଶ_verbose_cmd(linenr_T lnum, char_u *cmd)
{
    ++no_wait_return;
    verbose_enter_scroll();

    ଯଦି (lnum == 0)
	smsg(_("Executing: %s"), cmd);
    ଅନ୍ୟଥା
	smsg(_("line %ld: %s"), (long)lnum, cmd);
    ଯଦି (ସନ୍ଦେଶ_silent == 0)
	ସନ୍ଦେଶ_puts("\n");   // don't overwrite this

    verbose_leave_scroll();
    --no_wait_return;
}

/*
 * Execute a simple command line.  Used for translated commands like "*".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_cmdline_cmd(char_u *cmd)
{
    ଫେରନ୍ତୁ do_cmdline(cmd, ଖାଲି, ଖାଲି,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
}

/*
 * Execute the "+cmd" argument of "edit +cmd fname" and the like.
 * This allows for using a range without ":" in Vim9 script.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_cmd_argument(char_u *cmd)
{
    ଫେରନ୍ତୁ do_cmdline(cmd, ଖାଲି, ଖାଲି,
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_cmdline(
    char_u	*cmdline,
    char_u	*(*fgetline)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, getline_opt_T),
    ଶୂନ୍ୟ	*cookie,		// argument for fgetline()
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    char_u	*next_cmdline;		// next cmd to execute
    char_u	*cmdline_copy = ଖାଲି;	// copy of cmd line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		used_getline = FALSE;	// used "fgetline" to obtain command
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recursive = 0;		// recursive depth
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ସନ୍ଦେଶ_didout_before_start = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;		// line number count
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_inc_RedrawingDisabled = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;
#ifdef FEAT_EVAL
    cstack_T	cstack;			// conditional stack
    garray_T	ଧାଡ଼ିଗୁଡ଼ିକ_ga;		// keep lines for ":while"/":for"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		current_ଧାଡ଼ି = 0;	// active line in lines_ga
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		current_ଧାଡ଼ି_before = 0;
    char_u	*fname = ଖାଲି;		// function or script name
    linenr_T	*breakpoint = ଖାଲି;	// ptr to breakpoint field in cookie
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*dbg_tick = ଖାଲି;	// ptr to dbg_tick field in cookie
    ସଂରଚନା dbg_stuff debug_saved;	// saved things for debug mode
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		initial_trylevel;
    msglist_T	**saved_ସନ୍ଦେଶ_list = ଖାଲି;
    msglist_T	*private_ସନ୍ଦେଶ_list = ଖାଲି;

    // "fgetline" and "cookie" passed to do_one_cmd()
    char_u	*(*cmd_getline)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, getline_opt_T);
    ଶୂନ୍ୟ	*cmd_cookie;
    ସଂରଚନା loop_cookie cmd_loop_cookie;
    ଶୂନ୍ୟ	*real_cookie;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		getline_is_func;
#ଅନ୍ୟଥା
# define cmd_getline fgetline
# define cmd_cookie cookie
#endif
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	call_depth = 0;		// recursiveness
#ifdef FEAT_EVAL
    // For every pair of do_cmdline()/do_one_cmd() calls, use an extra memory
    // location for storing error messages to be converted to an exception.
    // This ensures that the do_errthrow() call in do_one_cmd() does not
    // combine the messages stored by an earlier invocation of do_one_cmd()
    // with the command name of the later one.  This would happen when
    // BufWritePost autocommands are executed after a write error.
    saved_ସନ୍ଦେଶ_list = ସନ୍ଦେଶ_list;
    ସନ୍ଦେଶ_list = &private_ସନ୍ଦେଶ_list;
#endif

    // It's possible to create an endless loop with ":execute", catch that
    // here.  The value of 200 allows nested function calls, ":source", etc.
    // Allow 200 or 'maxfuncdepth', whatever is larger.
    ଯଦି (call_depth >= 200
#ifdef FEAT_EVAL
	    && call_depth >= p_mfd
#endif
	    )
    {
	emsg(_(e_command_too_recursive));
#ifdef FEAT_EVAL
	// When converting to an exception, we do not include the command name
	// since this is not an error of the specific command.
	do_errthrow((cstack_T *)ଖାଲି, (char_u *)ଖାଲି);
	ସନ୍ଦେଶ_list = saved_ସନ୍ଦେଶ_list;
#endif
	ଫେରନ୍ତୁ FAIL;
    }
    ++call_depth;

#ifdef FEAT_EVAL
    CLEAR_FIELD(cstack);
    cstack.cs_idx = -1;
    ga_init2(&ଧାଡ଼ିଗୁଡ଼ିକ_ga, ଆକାର(wcmd_T), 10);

    real_cookie = getline_cookie(fgetline, cookie);

    // Inside a function use a higher nesting level.
    getline_is_func = getline_equal(fgetline, cookie, get_func_ଧାଡ଼ି);
    ଯଦି (getline_is_func && ex_nesting_level == func_level(real_cookie))
	++ex_nesting_level;

    // Get the function or script name and the address where the next breakpoint
    // line and the debug tick for a function or script are stored.
    ଯଦି (getline_is_func)
    {
	fname = func_name(real_cookie);
	breakpoint = func_breakpoint(real_cookie);
	dbg_tick = func_dbg_tick(real_cookie);
    }
    ଅନ୍ୟଥା ଯଦି (getline_equal(fgetline, cookie, getsourceline))
    {
	fname = SOURCING_NAME;
	breakpoint = source_breakpoint(real_cookie);
	dbg_tick = source_dbg_tick(real_cookie);
    }

    /*
     * Initialize "force_abort"  and "suppress_errthrow" at the top level.
     */
    ଯଦି (!recursive)
    {
	force_abort = FALSE;
	suppress_errthrow = FALSE;
    }

    /*
     * If requested, store and reset the global values controlling the
     * exception handling (used when debugging).  Otherwise clear it to avoid
     * a bogus compiler warning when the optimizer uses inline functions...
     */
    ଯଦି (flags & DOCMD_EXCRESET)
	save_dbg_stuff(&debug_saved);
    ଅନ୍ୟଥା
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
    ଯଦି (!(flags & DOCMD_KEYTYPED)
			       && !getline_equal(fgetline, cookie, getexline))
	ଚାବି_Typed = FALSE;

    /*
     * Continue executing command lines:
     * - when inside an ":if", ":while" or ":for"
     * - for multiple commands on one line, separated with '|'
     * - when repeating until there are no more lines (for ":source")
     */
    next_cmdline = cmdline;
    କର
    {
#ifdef FEAT_EVAL
	getline_is_func = getline_equal(fgetline, cookie, get_func_ଧାଡ଼ି);
#endif

	// stop skipping cmds for an error msg after all endif/while/for
	ଯଦି (next_cmdline == ଖାଲି
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
	ଯଦି (cstack.cs_looplevel > 0 && current_ଧାଡ଼ି < ଧାଡ଼ିଗୁଡ଼ିକ_ga.ga_len)
	{
	    // Each '|' separated command is stored separately in lines_ga, to
	    // be able to jump to it.  Don't use next_cmdline now.
	    VIM_CLEAR(cmdline_copy);

	    // Check if a function has returned or, unless it has an unclosed
	    // try conditional, aborted.
	    ଯଦି (getline_is_func)
	    {
# ifdef FEAT_PROFILE
		ଯଦି (do_profiling == PROF_YES)
		    func_ଧାଡ଼ି_end(real_cookie);
# endif
		ଯଦି (func_has_ended(real_cookie))
		{
		    retval = FAIL;
		    ଭାଙ୍ଗ;
		}
	    }
# ifdef FEAT_PROFILE
	    ଅନ୍ୟଥା ଯଦି (do_profiling == PROF_YES
			    && getline_equal(fgetline, cookie, getsourceline))
		script_ଧାଡ଼ି_end();
# endif

	    // Check if a sourced file hit a ":finish" command.
	    ଯଦି (source_finished(fgetline, cookie))
	    {
		retval = FAIL;
		ଭାଙ୍ଗ;
	    }

	    // If breakpoints have been added/deleted need to check for it.
	    ଯଦି (breakpoint != ଖାଲି && dbg_tick != ଖାଲି
						   && *dbg_tick != debug_tick)
	    {
		*breakpoint = dbg_find_breakpoint(
				getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }

	    next_cmdline = ((wcmd_T *)(ଧାଡ଼ିଗୁଡ଼ିକ_ga.ga_data))[current_ଧାଡ଼ି].ଧାଡ଼ି;
	    SOURCING_LNUM = ((wcmd_T *)(ଧାଡ଼ିଗୁଡ଼ିକ_ga.ga_data))[current_ଧାଡ଼ି].lnum;

	    // Did we encounter a breakpoint?
	    ଯଦି (breakpoint != ଖାଲି && *breakpoint != 0
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
	    ଯଦି (do_profiling == PROF_YES)
	    {
		ଯଦି (getline_is_func)
		    func_ଧାଡ଼ି_start(real_cookie, SOURCING_LNUM);
		ଅନ୍ୟଥା ଯଦି (getline_equal(fgetline, cookie, getsourceline))
		    script_ଧାଡ଼ି_start();
	    }
# endif
	}
#endif

	// 2. If no line given, get an allocated line with fgetline().
	ଯଦି (next_cmdline == ଖାଲି)
	{
	    /*
	     * Need to set msg_didout for the first line after an ":if",
	     * otherwise the ":if" will be overwritten.
	     */
	    ଯଦି (ଗଣନା_2 == 1 && getline_equal(fgetline, cookie, getexline))
		ସନ୍ଦେଶ_didout = TRUE;
	    ଯଦି (fgetline == ଖାଲି || (next_cmdline = fgetline(':', cookie,
#ifdef FEAT_EVAL
		    cstack.cs_idx < 0 ? 0 : (cstack.cs_idx + 1) * 2
#ଅନ୍ୟଥା
		    0
#endif
		    , in_vim9script() ? GETLINE_CONCAT_CONTBAR
					       : GETLINE_CONCAT_CONT)) == ଖାଲି)
	    {
		// Don't call wait_return() for aborted command line.  The NULL
		// returned for the end of a sourced file or executed function
		// doesn't do this.
		ଯଦି (ଚାବି_Typed && !(flags & DOCMD_REPEAT))
		    need_wait_return = FALSE;
		retval = FAIL;
		ଭାଙ୍ଗ;
	    }
	    used_getline = TRUE;

	    /*
	     * Keep the first typed line.  Clear it when more lines are typed.
	     */
	    ଯଦି (flags & DOCMD_KEEPLINE)
	    {
		vim_free(repeat_cmdline);
		ଯଦି (ଗଣନା_2 == 0)
		    repeat_cmdline = vim_strsave(next_cmdline);
		ଅନ୍ୟଥା
		    repeat_cmdline = ଖାଲି;
	    }
	}

	// 3. Make a copy of the command so we can mess with it.
	ଅନ୍ୟଥା ଯଦି (cmdline_copy == ଖାଲି)
	{
	    next_cmdline = vim_strsave(next_cmdline);
	    ଯଦି (next_cmdline == ଖାଲି)
	    {
		emsg(_(e_out_of_memory));
		retval = FAIL;
		ଭାଙ୍ଗ;
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
	ଯଦି ((cstack.cs_looplevel > 0 || has_loop_cmd(next_cmdline)))
	{
	    cmd_getline = get_loop_ଧାଡ଼ି;
	    cmd_cookie = (ଶୂନ୍ୟ *)&cmd_loop_cookie;
	    cmd_loop_cookie.ଧାଡ଼ିଗୁଡ଼ିକ_gap = &ଧାଡ଼ିଗୁଡ଼ିକ_ga;
	    cmd_loop_cookie.current_ଧାଡ଼ି = current_ଧାଡ଼ି;
	    cmd_loop_cookie.lc_getline = fgetline;
	    cmd_loop_cookie.cookie = cookie;
	    cmd_loop_cookie.repeating = (current_ଧାଡ଼ି < ଧାଡ଼ିଗୁଡ଼ିକ_ga.ga_len);

	    // Save the current line when encountering it the first time.
	    ଯଦି (current_ଧାଡ଼ି == ଧାଡ଼ିଗୁଡ଼ିକ_ga.ga_len
		    && store_loop_ଧାଡ଼ି(&ଧାଡ଼ିଗୁଡ଼ିକ_ga, next_cmdline) == FAIL)
	    {
		retval = FAIL;
		ଭାଙ୍ଗ;
	    }
	    current_ଧାଡ଼ି_before = current_ଧାଡ଼ି;
	}
	ଅନ୍ୟଥା
	{
	    cmd_getline = fgetline;
	    cmd_cookie = cookie;
	}

	did_endif = FALSE;
#endif

	ଯଦି (ଗଣନା_2++ == 0)
	{
	    /*
	     * All output from the commands is put below each other, without
	     * waiting for a return. Don't do this when executing commands
	     * from a script or when being called recursive (e.g. for ":e
	     * +command file").
	     */
	    ଯଦି (!(flags & DOCMD_NOWAIT) && !recursive)
	    {
		ସନ୍ଦେଶ_didout_before_start = ସନ୍ଦେଶ_didout;
		ସନ୍ଦେଶ_didany = FALSE; // no output yet
		ସନ୍ଦେଶ_start();
		ସନ୍ଦେଶ_scroll = TRUE;  // put messages below each other
		++no_wait_return;   // don't wait for return until finished
		++RedrawingDisabled;
		did_inc_RedrawingDisabled = TRUE;
	    }
	}

	ଯଦି ((p_verbose >= 15 && SOURCING_NAME != ଖାଲି) || p_verbose >= 16)
	    ସନ୍ଦେଶ_verbose_cmd(SOURCING_LNUM, cmdline_copy);

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
	ଯଦି (cmd_cookie == (ଶୂନ୍ୟ *)&cmd_loop_cookie)
	    // Use "current_line" from "cmd_loop_cookie", it may have been
	    // incremented when defining a function.
	    current_ଧାଡ଼ି = cmd_loop_cookie.current_ଧାଡ଼ି;
#endif

	ଯଦି (next_cmdline == ଖାଲି)
	{
	    VIM_CLEAR(cmdline_copy);

	    /*
	     * If the command was typed, remember it for the ':' register.
	     * Do this AFTER executing the command to make :@: work.
	     */
	    ଯଦି (getline_equal(fgetline, cookie, getexline)
						  && new_last_cmdline != ଖାଲି)
	    {
		vim_free(last_cmdline);
		last_cmdline = new_last_cmdline;
		new_last_cmdline = ଖାଲି;
	    }
	}
	ଅନ୍ୟଥା
	{
	    // need to copy the command after the '|' to cmdline_copy, for the
	    // next do_one_cmd()
	    STRMOVE(cmdline_copy, next_cmdline);
	    next_cmdline = cmdline_copy;
	}


#ifdef FEAT_EVAL
	// reset did_emsg for a function that is not aborted by an error
	ଯଦି (did_emsg && !force_abort
		&& getline_equal(fgetline, cookie, get_func_ଧାଡ଼ି)
					      && !func_has_abort(real_cookie))
	{
	    // did_emsg_cumul is not set here
	    did_emsg = FALSE;
	}

	ଯଦି (cstack.cs_looplevel > 0)
	{
	    ++current_ଧାଡ଼ି;

	    /*
	     * An ":endwhile", ":endfor" and ":continue" is handled here.
	     * If we were executing commands, jump back to the ":while" or
	     * ":for".
	     * If we were not executing commands, decrement cs_looplevel.
	     */
	    ଯଦି (cstack.cs_lflags & (CSL_HAD_CONT | CSL_HAD_ENDLOOP))
	    {
		cstack.cs_lflags &= ~(CSL_HAD_CONT | CSL_HAD_ENDLOOP);

		// Jump back to the matching ":while" or ":for".  Be careful
		// not to use a cs_line[] from an entry that isn't a ":while"
		// or ":for": It would make "current_line" invalid and can
		// cause a crash.
		ଯଦି (!did_emsg && !got_int && !did_throw
			&& cstack.cs_idx >= 0
			&& (cstack.cs_flags[cstack.cs_idx]
						      & (CSF_WHILE | CSF_FOR))
			&& cstack.cs_ଧାଡ଼ି[cstack.cs_idx] >= 0
			&& (cstack.cs_flags[cstack.cs_idx] & CSF_ACTIVE))
		{
		    current_ଧାଡ଼ି = cstack.cs_ଧାଡ଼ି[cstack.cs_idx];
						// remember we jumped there
		    cstack.cs_lflags |= CSL_HAD_LOOP;
		    ଧାଡ଼ି_breakcheck();		// check if CTRL-C typed

		    // Check for the next breakpoint at or after the ":while"
		    // or ":for".
		    ଯଦି (breakpoint != ଖାଲି && ଧାଡ଼ିଗୁଡ଼ିକ_ga.ga_len > current_ଧାଡ଼ି)
		    {
			*breakpoint = dbg_find_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
									fname,
			   ((wcmd_T *)ଧାଡ଼ିଗୁଡ଼ିକ_ga.ga_data)[current_ଧାଡ଼ି].lnum-1);
			*dbg_tick = debug_tick;
		    }
		}
		ଅନ୍ୟଥା
		{
		    // can only get here with ":endwhile" or ":endfor"
		    ଯଦି (cstack.cs_idx >= 0)
			rewind_conditionals(&cstack, cstack.cs_idx - 1,
				   CSF_WHILE | CSF_FOR, &cstack.cs_looplevel);
		}
	    }

	    /*
	     * For a ":while" or ":for" we need to remember the line number.
	     */
	    ଅନ୍ୟଥା ଯଦି (cstack.cs_lflags & CSL_HAD_LOOP)
	    {
		cstack.cs_lflags &= ~CSL_HAD_LOOP;
		cstack.cs_ଧାଡ଼ି[cstack.cs_idx] = current_ଧାଡ଼ି_before;
	    }
	}

	// Check for the next breakpoint after a watchexpression
	ଯଦି (breakpoint != ଖାଲି && has_watchexpr())
	{
	    *breakpoint = dbg_find_breakpoint(FALSE, fname, SOURCING_LNUM);
	    *dbg_tick = debug_tick;
	}

	/*
	 * When not inside any ":while" loop, clear remembered lines.
	 */
	ଯଦି (cstack.cs_looplevel == 0)
	{
	    ଯଦି (ଧାଡ଼ିଗୁଡ଼ିକ_ga.ga_len > 0)
	    {
		SOURCING_LNUM =
		       ((wcmd_T *)ଧାଡ଼ିଗୁଡ଼ିକ_ga.ga_data)[ଧାଡ଼ିଗୁଡ଼ିକ_ga.ga_len - 1].lnum;
		free_cmdlines(&ଧାଡ଼ିଗୁଡ଼ିକ_ga);
	    }
	    current_ଧାଡ଼ି = 0;
	}

	/*
	 * A ":finally" makes did_emsg, got_int, and did_throw pending for
	 * being restored at the ":endtry".  Reset them here and set the
	 * ACTIVE and FINALLY flags, so that the finally clause gets executed.
	 * This includes the case where a missing ":endif", ":endwhile" or
	 * ":endfor" was detected by the ":finally" itself.
	 */
	ଯଦି (cstack.cs_lflags & CSL_HAD_FINA)
	{
	    cstack.cs_lflags &= ~CSL_HAD_FINA;
	    report_make_pending(cstack.cs_pending[cstack.cs_idx]
		    & (CSTP_ERROR | CSTP_INTERRUPT | CSTP_THROW),
		    did_throw ? (ଶୂନ୍ୟ *)current_exception : ଖାଲି);
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
	ଯଦି (trylevel == 0 && !did_emsg && !got_int && !did_throw)
	    force_abort = FALSE;

	// Convert an interrupt to an exception if appropriate.
	(ଶୂନ୍ୟ)do_intthrow(&cstack);
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
    ଯେତେବେଳେ (!((got_int
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
	    && (next_cmdline != ଖାଲି
#ifdef FEAT_EVAL
			|| cstack.cs_idx >= 0
#endif
			|| (flags & DOCMD_REPEAT)));

    vim_free(cmdline_copy);
    did_emsg_syntax = FALSE;
#ifdef FEAT_EVAL
    free_cmdlines(&ଧାଡ଼ିଗୁଡ଼ିକ_ga);
    ga_clear(&ଧାଡ଼ିଗୁଡ଼ିକ_ga);

    ଯଦି (cstack.cs_idx >= 0)
    {
	/*
	 * If a sourced file or executed function ran to its end, report the
	 * unclosed conditional.
	 * In Vim9 script do not give a second error, executing aborts after
	 * the first one.
	 */
	ଯଦି (!got_int && !did_throw && !aborting()
		&& !(did_emsg && in_vim9script())
		&& ((getline_equal(fgetline, cookie, getsourceline)
			&& !source_finished(fgetline, cookie))
		    || (getline_equal(fgetline, cookie, get_func_ଧାଡ଼ି)
					    && !func_has_ended(real_cookie))))
	{
	    ଯଦି (cstack.cs_flags[cstack.cs_idx] & CSF_TRY)
		emsg(_(e_missing_endtry));
	    ଅନ୍ୟଥା ଯଦି (cstack.cs_flags[cstack.cs_idx] & CSF_WHILE)
		emsg(_(e_missing_endwhile));
	    ଅନ୍ୟଥା ଯଦି (cstack.cs_flags[cstack.cs_idx] & CSF_FOR)
		emsg(_(e_missing_endfor));
	    ଅନ୍ୟଥା
		emsg(_(e_missing_endif));
	}

	/*
	 * Reset "trylevel" in case of a ":finish" or ":return" or a missing
	 * ":endtry" in a sourced file or executed function.  If the try
	 * conditional is in its finally clause, ignore anything pending.
	 * If it is in a catch clause, finish the caught exception.
	 * Also cleanup any "cs_forinfo" structures.
	 */
	କର
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = cleanup_conditionals(&cstack, 0, TRUE);

	    ଯଦି (idx >= 0)
		--idx;	    // remove try block not in its finally clause
	    rewind_conditionals(&cstack, idx, CSF_WHILE | CSF_FOR,
							&cstack.cs_looplevel);
	}
	ଯେତେବେଳେ (cstack.cs_idx >= 0);
	trylevel = initial_trylevel;
    }

    // If a missing ":endtry", ":endwhile", ":endfor", or ":endif" or a memory
    // lack was reported above and the error message is to be converted to an
    // exception, do this now after rewinding the cstack.
    do_errthrow(&cstack, getline_equal(fgetline, cookie, get_func_ଧାଡ଼ି)
				  ? (char_u *)"endfunction" : (char_u *)ଖାଲି);

    ଯଦି (trylevel == 0)
    {
	// Just in case did_throw got set but current_exception wasn't.
	ଯଦି (current_exception == ଖାଲି)
	    did_throw = FALSE;

	/*
	 * When an exception is being thrown out of the outermost try
	 * conditional, discard the uncaught exception, disable the conversion
	 * of interrupts or errors to exceptions, and ensure that no more
	 * commands are executed.
	 */
	ଯଦି (did_throw)
	    handle_did_throw();

	/*
	 * On an interrupt or an aborting error not converted to an exception,
	 * disable the conversion of errors to exceptions.  (Interrupts are not
	 * converted anymore, here.) This enables also the interrupt message
	 * when force_abort is set and did_emsg unset in case of an interrupt
	 * from a finally clause after an error.
	 */
	ଅନ୍ୟଥା ଯଦି (got_int || (did_emsg && force_abort))
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
    ଯଦି (did_throw)
	need_rethrow = TRUE;
    ଯଦି ((getline_equal(fgetline, cookie, getsourceline)
		&& ex_nesting_level > source_level(real_cookie))
	    || (getline_equal(fgetline, cookie, get_func_ଧାଡ଼ି)
		&& ex_nesting_level > func_level(real_cookie) + 1))
    {
	ଯଦି (!did_throw)
	    check_cstack = TRUE;
    }
    ଅନ୍ୟଥା
    {
	// When leaving a function, reduce nesting level.
	ଯଦି (getline_equal(fgetline, cookie, get_func_ଧାଡ଼ି))
	    --ex_nesting_level;
	/*
	 * Go to debug mode when returning from a function in which we are
	 * single-stepping.
	 */
	ଯଦି ((getline_equal(fgetline, cookie, getsourceline)
		    || getline_equal(fgetline, cookie, get_func_ଧାଡ଼ି))
		&& ex_nesting_level + 1 <= debug_break_level)
	    do_debug(getline_equal(fgetline, cookie, getsourceline)
		    ? (char_u *)_("End of sourced file")
		    : (char_u *)_("End of function"));
    }

    /*
     * Restore the exception environment (done after returning from the
     * debugger).
     */
    ଯଦି (flags & DOCMD_EXCRESET)
	restore_dbg_stuff(&debug_saved);

    ସନ୍ଦେଶ_list = saved_ସନ୍ଦେଶ_list;

    // Cleanup if "cs_emsg_silent_list" remains.
    ଯଦି (cstack.cs_emsg_silent_list != ଖାଲି)
    {
	eslist_T *elem, *temp;

	ପାଇଁ (elem = cstack.cs_emsg_silent_list; elem != ଖାଲି; elem = temp)
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
    ଯଦି (did_inc_RedrawingDisabled)
    {
	ଯଦି (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	--no_wait_return;
	ସନ୍ଦେଶ_scroll = FALSE;

	/*
	 * When just finished an ":if"-":else" which was typed, no need to
	 * wait for hit-return.  Also for an error situation.
	 */
	ଯଦି (retval == FAIL
#ifdef FEAT_EVAL
		|| (did_endif && ଚାବି_Typed && !did_emsg)
#endif
					    )
	{
	    need_wait_return = FALSE;
	    ସନ୍ଦେଶ_didany = FALSE;		// don't wait when restarting edit
	}
	ଅନ୍ୟଥା ଯଦି (need_wait_return)
	{
	    /*
	     * The msg_start() above clears msg_didout. The wait_return() we do
	     * here should not overwrite the command that may be shown before
	     * doing that.
	     */
	    ସନ୍ଦେଶ_didout |= ସନ୍ଦେଶ_didout_before_start;
	    wait_return(FALSE);
	}
    }

#ifdef FEAT_EVAL
    did_endif = FALSE;  // in case do_cmdline used recursively
#ଅନ୍ୟଥା
    /*
     * Reset if_level, in case a sourced script file contains more ":if" than
     * ":endif" (could be ":if x | foo | endif").
     */
    if_level = 0;
#endif

    --call_depth;
    ଫେରନ୍ତୁ retval;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Handle when "did_throw" is set after executing commands.
 */
    ଶୂନ୍ୟ
handle_did_throw(ଶୂନ୍ୟ)
{
    ଅକ୍ଷର	*p = ଖାଲି;
    msglist_T	*messages = ଖାଲି;
    ESTACK_CHECK_DECLARATION;

    /*
     * If the uncaught exception is a user exception, report it as an
     * error.  If it is an error exception, display the saved error
     * message now.  For an interrupt exception, do nothing; the
     * interrupt message is given elsewhere.
     */
    ବାଛ (current_exception->type)
    {
	ସ୍ଥିତି ET_USER:
	    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE,
		    _(e_exception_not_caught_str),
		    current_exception->ମୂଲ୍ୟ);
	    p = (ଅକ୍ଷର *)vim_strsave(IObuff);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ET_ERROR:
	    messages = current_exception->messages;
	    current_exception->messages = ଖାଲି;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ET_INTERRUPT:
	    ଭାଙ୍ଗ;
    }

    estack_ଠେଲ(ETYPE_EXCEPT, current_exception->throw_name,
					current_exception->throw_lnum);
    ESTACK_CHECK_SETUP;
    current_exception->throw_name = ଖାଲି;

    discard_current_exception();	// uses IObuff if 'verbose'

    // If "silent!" is active the uncaught exception is not fatal.
    ଯଦି (emsg_silent == 0)
    {
	suppress_errthrow = TRUE;
	force_abort = TRUE;
    }

    ଯଦି (messages != ଖାଲି)
    {
	କର
	{
	    msglist_T	*next = messages->next;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_compiling = estack_compiling;

	    estack_compiling = messages->ସନ୍ଦେଶ_compiling;
	    emsg(messages->ସନ୍ଦେଶ);
	    vim_free(messages->ସନ୍ଦେଶ);
	    vim_free(messages->sfile);
	    vim_free(messages);
	    messages = next;
	    estack_compiling = save_compiling;
	}
	ଯେତେବେଳେ (messages != ଖାଲି);
    }
    ଅନ୍ୟଥା ଯଦି (p != ଖାଲି)
    {
	emsg(p);
	vim_free(p);
    }
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_କାଢ଼();
}

/*
 * Obtain a line when inside a ":while" or ":for" loop.
 */
    ସ୍ଥିର char_u *
get_loop_ଧାଡ଼ି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ଶୂନ୍ୟ *cookie, ପୂର୍ଣ୍ଣସଂଖ୍ୟା indent, getline_opt_T options)
{
    ସଂରଚନା loop_cookie	*cp = (ସଂରଚନା loop_cookie *)cookie;
    wcmd_T		*wp;
    char_u		*ଧାଡ଼ି;

    ଯଦି (cp->current_ଧାଡ଼ି + 1 >= cp->ଧାଡ଼ିଗୁଡ଼ିକ_gap->ga_len)
    {
	ଯଦି (cp->repeating)
	    ଫେରନ୍ତୁ ଖାଲି;	// trying to read past ":endwhile"/":endfor"

	// First time inside the ":while"/":for": get line normally.
	ଯଦି (cp->lc_getline == ଖାଲି)
	    ଧାଡ଼ି = getcmdline(c, 0L, indent, 0);
	ଅନ୍ୟଥା
	    ଧାଡ଼ି = cp->lc_getline(c, cp->cookie, indent, options);
	ଯଦି (ଧାଡ଼ି != ଖାଲି && store_loop_ଧାଡ଼ି(cp->ଧାଡ଼ିଗୁଡ଼ିକ_gap, ଧାଡ଼ି) == OK)
	    ++cp->current_ଧାଡ଼ି;

	ଫେରନ୍ତୁ ଧାଡ଼ି;
    }

    ଚାବି_Typed = FALSE;
    ++cp->current_ଧାଡ଼ି;
    wp = (wcmd_T *)(cp->ଧାଡ଼ିଗୁଡ଼ିକ_gap->ga_data) + cp->current_ଧାଡ଼ି;
    SOURCING_LNUM = wp->lnum;
    ଫେରନ୍ତୁ vim_strsave(wp->ଧାଡ଼ି);
}

/*
 * Store a line in "gap" so that a ":while" loop can execute it again.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
store_loop_ଧାଡ଼ି(garray_T *gap, char_u *ଧାଡ଼ି)
{
    ଯଦି (ga_grow(gap, 1) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].ଧାଡ଼ି = vim_strsave(ଧାଡ଼ି);
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].lnum = SOURCING_LNUM;
    ++gap->ga_len;
    ଫେରନ୍ତୁ OK;
}

/*
 * Free the lines stored for a ":while" or ":for" loop.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
free_cmdlines(garray_T *gap)
{
    ଯେତେବେଳେ (gap->ga_len > 0)
    {
	vim_free(((wcmd_T *)(gap->ga_data))[gap->ga_len - 1].ଧାଡ଼ି);
	--gap->ga_len;
    }
}
#endif

/*
 * If "fgetline" is get_loop_line(), return TRUE if the getline it uses equals
 * "func".  * Otherwise return TRUE when "fgetline" equals "func".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
getline_equal(
    char_u	*(*fgetline)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, getline_opt_T),
    ଶୂନ୍ୟ	*cookie UNUSED,		// argument for fgetline()
    char_u	*(*func)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, getline_opt_T))
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, getline_opt_T);
    ସଂରଚନା loop_cookie *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // function that's originally used to obtain the lines.  This may be
    // nested several levels.
    gp = fgetline;
    cp = (ସଂରଚନା loop_cookie *)cookie;
    ଯେତେବେଳେ (gp == get_loop_ଧାଡ଼ି)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    ଫେରନ୍ତୁ gp == func;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ fgetline == func;
#endif
}

/*
 * If "fgetline" is get_loop_line(), return the cookie used by the original
 * getline function.  Otherwise return "cookie".
 */
    ଶୂନ୍ୟ *
getline_cookie(
    char_u	*(*fgetline)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, getline_opt_T) UNUSED,
    ଶୂନ୍ୟ	*cookie)		// argument for fgetline()
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, getline_opt_T);
    ସଂରଚନା loop_cookie  *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (ସଂରଚନା loop_cookie *)cookie;
    ଯେତେବେଳେ (gp == get_loop_ଧାଡ଼ି)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    ଫେରନ୍ତୁ cp;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ cookie;
#endif
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Get the next line source line without advancing.
 */
    char_u *
getline_peek(
    char_u	*(*fgetline)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, getline_opt_T) UNUSED,
    ଶୂନ୍ୟ	*cookie)		// argument for fgetline()
{
    char_u		*(*gp)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, getline_opt_T);
    ସଂରଚନା loop_cookie  *cp;
    wcmd_T		*wp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (ସଂରଚନା loop_cookie *)cookie;
    ଯେତେବେଳେ (gp == get_loop_ଧାଡ଼ି)
    {
	ଯଦି (cp->current_ଧାଡ଼ି + 1 < cp->ଧାଡ଼ିଗୁଡ଼ିକ_gap->ga_len)
	{
	    // executing lines a second time, use the stored copy
	    wp = (wcmd_T *)(cp->ଧାଡ଼ିଗୁଡ଼ିକ_gap->ga_data) + cp->current_ଧାଡ଼ି + 1;
	    ଫେରନ୍ତୁ wp->ଧାଡ଼ି;
	}
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    ଯଦି (gp == getsourceline)
	ଫେରନ୍ତୁ source_nextline(cp);
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif


/*
 * Helper function to apply an offset for buffer commands, i.e. ":bdelete",
 * ":bwipeout", etc.
 * Returns the buffer number.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compute_buffer_local_ଗଣନା(ପୂର୍ଣ୍ଣସଂଖ୍ୟା addr_type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
    buf_T   *buf;
    buf_T   *nextbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     ଗଣନା_2 = offset;

    buf = firstbuf;
    ଯେତେବେଳେ (buf->b_next != ଖାଲି && buf->b_fnum < lnum)
	buf = buf->b_next;
    ଯେତେବେଳେ (ଗଣନା_2 != 0)
    {
	ଗଣନା_2 += (offset < 0) ? 1 : -1;
	nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
	ଯଦି (nextbuf == ଖାଲି)
	    ଭାଙ୍ଗ;
	buf = nextbuf;
	ଯଦି (addr_type == ADDR_LOADED_BUFFERS)
	    // skip over unloaded buffers
	    ଯେତେବେଳେ (buf->b_ml.ml_mfp == ଖାଲି)
	    {
		nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
		ଯଦି (nextbuf == ଖାଲି)
		    ଭାଙ୍ଗ;
		buf = nextbuf;
	    }
    }
    // we might have gone too far, last buffer is not loadedd
    ଯଦି (addr_type == ADDR_LOADED_BUFFERS)
	ଯେତେବେଳେ (buf->b_ml.ml_mfp == ଖାଲି)
	{
	    nextbuf = (offset >= 0) ? buf->b_prev : buf->b_next;
	    ଯଦି (nextbuf == ଖାଲି)
		ଭାଙ୍ଗ;
	    buf = nextbuf;
	}
    ଫେରନ୍ତୁ buf->b_fnum;
}

/*
 * Return the window number of "win".
 * When "win" is NULL return the number of windows.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
current_win_nr(win_T *win)
{
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr = 0;

    FOR_ALL_WINDOWS(wp)
    {
	++nr;
	ଯଦି (wp == win)
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ nr;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
current_tab_nr(tabpage_T *tab)
{
    tabpage_T	*tp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr = 0;

    FOR_ALL_TABPAGES(tp)
    {
	++nr;
	ଯଦି (tp == tab)
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ nr;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
comment_start(char_u *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା starts_with_colon UNUSED)
{
    ଯଦି (in_vim9script())
	ଫେରନ୍ତୁ p[0] == '#' && !starts_with_colon;
    ଫେରନ୍ତୁ *p == '"';
}

#define CURRENT_WIN_NR current_win_nr(curwin)
#define LAST_WIN_NR current_win_nr(ଖାଲି)
#define CURRENT_TAB_NR current_tab_nr(curtab)
#define LAST_TAB_NR current_tab_nr(ଖାଲି)

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
    ସ୍ଥିର char_u *
do_one_cmd(
    char_u	**cmdlinep,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
#ifdef FEAT_EVAL
    cstack_T	*cstack,
#endif
    char_u	*(*fgetline)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, getline_opt_T),
    ଶୂନ୍ୟ	*cookie)		// argument for fgetline()
{
    char_u	*p;
    linenr_T	lnum;
    long	n;
    ଅକ୍ଷର	*errormsg = ଖାଲି;	// error message
    char_u	*after_modifier = ଖାଲି;
    exarg_T	ea;			// Ex command arguments
    cmdmod_T	save_cmdmod;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_reg_executing = reg_executing;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_pending_end_reg_executing = pending_end_reg_executing;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ni;			// set when Not Implemented
    char_u	*cmd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		starts_with_colon = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		may_have_range;
#ifdef FEAT_EVAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_set_ପଦସମଷ୍ଟି_ଧାଡ଼ି = FALSE;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sourcing = flags & DOCMD_VERBOSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_append_cmd = FALSE;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
#ifdef FEAT_EVAL
    ++ex_nesting_level;
#endif

    // When the last file has not been edited :q has to be typed twice.
    ଯଦି (quitmore
#ifdef FEAT_EVAL
	    // avoid that a function call in 'statusline' does this
	    && !getline_equal(fgetline, cookie, get_func_ଧାଡ଼ି)
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
    ଯଦି ((*cmdlinep)[0] == '#' && (*cmdlinep)[1] == '!')
	ଯାଅ doend;

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
    ଯଦି (parse_command_modifiers(&ea, &errormsg, &cmdmod, FALSE) == FAIL)
	ଯାଅ doend;
    apply_cmdmod(&cmdmod);
    after_modifier = ea.cmd;

#ifdef FEAT_EVAL
    ea.skip = did_emsg || got_int || did_throw || (cstack->cs_idx >= 0
			 && !(cstack->cs_flags[cstack->cs_idx] & CSF_ACTIVE));
#ଅନ୍ୟଥା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା vim9script = in_vim9script();
    ଯଦି (vim9script && (flags & DOCMD_RANGEOK) == 0)
    {
	may_have_range = FALSE;
	ପାଇଁ (p = ea.cmd; p >= *cmdlinep; --p)
	{
	    ଯଦି (*p == ':')
		may_have_range = TRUE;
	    ଯଦି (p < ea.cmd && !VIM_ISWHITE(*p))
		ଭାଙ୍ଗ;
	}
    }
    ଅନ୍ୟଥା
	may_have_range = TRUE;
    ଯଦି (may_have_range)
	ea.cmd = skip_range(ea.cmd, TRUE, ଖାଲି);

#ifdef FEAT_EVAL
    // Handle ":export" - it functions almost like a command modifier.
    // ":export var Name: type"
    // ":export def Name(..."
    // etc.
    ଯଦି (vim9script && checkforcmd_noparen(&ea.cmd, "export", 6))
	is_export = TRUE;
#endif

    ଯଦି (vim9script && !may_have_range)
    {
	ଯଦି (ea.cmd == cmd + 1 && *cmd == '$')
	    // should be "$VAR = val"
	    --ea.cmd;
#ifdef FEAT_EVAL
	p = find_ex_command(&ea, ଖାଲି, lookup_scriptitem, ଖାଲି);
#ଅନ୍ୟଥା
	p = find_ex_command(&ea, ଖାଲି, ଖାଲି, ଖାଲି);
#endif
	ଯଦି (ea.cmdidx == CMD_SIZE)
	{
	    char_u *ar = skip_range(ea.cmd, TRUE, ଖାଲି);

	    // If a ':' before the range is missing, give a clearer error
	    // message.
	    ଯଦି (ar > ea.cmd && !ea.skip)
	    {
		semsg(_(e_colon_required_before_range_str), ea.cmd);
		ଯାଅ doend;
	    }
	}
    }
    ଅନ୍ୟଥା
	p = find_ex_command(&ea, ଖାଲି, ଖାଲି, ଖାଲି);

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    // Count this line for profiling if skip is TRUE.
    ଯଦି (do_profiling == PROF_YES
	    && (!ea.skip || cstack->cs_idx == 0 || (cstack->cs_idx > 0
		     && (cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE))))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା skip = did_emsg || got_int || did_throw;

	ଯଦି (ea.cmdidx == CMD_catch)
	    skip = !skip && !(cstack->cs_idx >= 0
			  && (cstack->cs_flags[cstack->cs_idx] & CSF_THROWN)
			  && !(cstack->cs_flags[cstack->cs_idx] & CSF_CAUGHT));
	ଅନ୍ୟଥା ଯଦି (ea.cmdidx == CMD_else || ea.cmdidx == CMD_elseif)
	    skip = skip || !(cstack->cs_idx >= 0
			  && !(cstack->cs_flags[cstack->cs_idx]
						  & (CSF_ACTIVE | CSF_TRUE)));
	ଅନ୍ୟଥା ଯଦି (ea.cmdidx == CMD_finally)
	    skip = FALSE;
	ଅନ୍ୟଥା ଯଦି (ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endtry
		&& ea.cmdidx != CMD_endwhile)
	    skip = ea.skip;

	ଯଦି (!skip)
	{
	    ଯଦି (getline_equal(fgetline, cookie, get_func_ଧାଡ଼ି))
		func_ଧାଡ଼ି_exec(getline_cookie(fgetline, cookie));
	    ଅନ୍ୟଥା ଯଦି (getline_equal(fgetline, cookie, getsourceline))
		script_ଧାଡ଼ି_exec();
	}
    }
# endif
#endif

    ea.cmd = cmd;

#ifdef FEAT_EVAL
    // May go to debug mode.  If this happens and the ">quit" debug command is
    // used, throw an interrupt exception and skip the next command.
    dbg_check_breakpoint(&ea);
    ଯଦି (!ea.skip && got_int)
    {
	ea.skip = TRUE;
	(ଶୂନ୍ୟ)do_intthrow(cstack);
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
    ଯଦି (!IS_USER_CMDIDX(ea.cmdidx))
    {
	ଯଦି (ea.cmdidx != CMD_SIZE)
	    ea.addr_type = cmdnames[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ea.cmdidx].cmd_addr_type;
	ଅନ୍ୟଥା
	    ea.addr_type = ADDR_ଧାଡ଼ିଗୁଡ଼ିକ;

	// :wincmd range depends on the argument.
	ଯଦି (ea.cmdidx == CMD_wincmd && p != ଖାଲି)
	    get_wincmd_addr_type(skipwhite(p), &ea);
#ifdef FEAT_QUICKFIX
	// :.cc in quickfix window uses line number
	ଯଦି ((ea.cmdidx == CMD_cc || ea.cmdidx == CMD_ll) && bt_quickfix(curbuf))
	    ea.addr_type = ADDR_OTHER;
#endif
    }

    ଯଦି (!may_have_range)
	ea.line1 = ea.line2 = default_address(&ea);
    ଅନ୍ୟଥା ଯଦି (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
	ଯାଅ doend;

/*
 * 5. Parse the command.
 */

    /*
     * Skip ':' and any white space
     */
    ea.cmd = skipwhite(ea.cmd);
    ଯେତେବେଳେ (*ea.cmd == ':')
	ea.cmd = skipwhite(ea.cmd + 1);

    /*
     * If we got a line, but no command, then go to the line.
     * If we find a '|' or '\n' we set ea.nextcmd.
     */
    ଯଦି (*ea.cmd == NUL || comment_start(ea.cmd, starts_with_colon)
			       || (ea.nextcmd = check_nextcmd(ea.cmd)) != ଖାଲି)
    {
	/*
	 * strange vi behaviour:
	 * ":3"		jumps to line 3
	 * ":3|..."	prints line 3  (not in Vim9 script)
	 * ":|"		prints current line  (not in Vim9 script)
	 */
	ଯଦି (ea.skip)	    // skip this if inside :if
	    ଯାଅ doend;
	errormsg = ex_range_without_command(&ea);
	ଯାଅ doend;
    }

    // If this looks like an undefined user command and there are CmdUndefined
    // autocommands defined, trigger the matching autocommands.
    ଯଦି (p != ଖାଲି && ea.cmdidx == CMD_SIZE && !ea.skip
	    && ASCII_ISUPPER(*ea.cmd)
	    && has_cmdundefined())
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	p = ea.cmd;
	ଯେତେବେଳେ (ASCII_ISALNUM(*p))
	    ++p;
	p = vim_strnsave(ea.cmd, p - ea.cmd);
	ret = apply_autocmds(EVENT_CMDUNDEFINED, p, p, TRUE, ଖାଲି);
	vim_free(p);
	// If the autocommands did something and didn't cause an error, try
	// finding the command again.
	p = (ret
#ifdef FEAT_EVAL
		&& !aborting()
#endif
		) ? find_ex_command(&ea, ଖାଲି, ଖାଲି, ଖାଲି) : ea.cmd;
    }

    ଯଦି (p == ଖାଲି)
    {
	ଯଦି (!ea.skip)
	    errormsg = _(e_ambiguous_use_of_user_defined_command);
	ଯାଅ doend;
    }
    // Check for wrong commands.
    ଯଦି (*p == '!' && ea.cmd[1] == 0151 && ea.cmd[0] == 78
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	errormsg = uc_fun_cmd();
	ଯାଅ doend;
    }

    ଯଦି (ea.cmdidx == CMD_SIZE)
    {
	ଯଦି (!ea.skip)
	{
	    STRCPY(IObuff, _(e_not_an_editor_command));
	    ଯଦି (!sourcing)
	    {
		// If the modifier was parsed OK the error must be in the
		// following command
		ଯଦି (after_modifier != ଖାଲି)
		    append_command(after_modifier);
		ଅନ୍ୟଥା
		    append_command(*cmdlinep);
		did_append_cmd = TRUE;
	    }
	    errormsg = (ଅକ୍ଷର *)IObuff;
	    did_emsg_syntax = TRUE;
	}
	ଯାଅ doend;
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
    ଯଦି (ea.cmdidx == CMD_if)
	++if_level;
    ଯଦି (if_level)
    {
	ଯଦି (ea.cmdidx == CMD_endif)
	    --if_level;
	ଯାଅ doend;
    }

#endif

    // forced commands
    ଯଦି (*p == '!' && ea.cmdidx != CMD_substitute
	    && ea.cmdidx != CMD_smagic && ea.cmdidx != CMD_snomagic)
    {
	++p;
	ea.forceit = TRUE;
    }
    ଅନ୍ୟଥା
	ea.forceit = FALSE;

/*
 * 6. Parse arguments.  Then check for errors.
 */
    ଯଦି (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = (long)cmdnames[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ea.cmdidx].cmd_argt;

    ଯଦି (!ea.skip)
    {
#ifdef HAVE_SANDBOX
	ଯଦି (sandbox != 0 && !(ea.argt & EX_SBOXOK))
	{
	    // Command not allowed in sandbox.
	    errormsg = _(e_not_allowed_in_sandbox);
	    ଯାଅ doend;
	}
#endif
	ଯଦି (restricted != 0 && (ea.argt & EX_RESTRICT))
	{
	    errormsg = _(e_command_not_allowed_in_rvim);
	    ଯାଅ doend;
	}
	ଯଦି (!curbuf->b_p_ma && (ea.argt & EX_MODIFY))
	{
	    // Command not allowed in non-'modifiable' buffer
	    errormsg = _(e_cannot_make_changes_modifiable_is_off);
	    ଯାଅ doend;
	}

	ଯଦି (!IS_USER_CMDIDX(ea.cmdidx))
	{
	    ଯଦି (cmdwin_type != 0 && !(ea.argt & EX_CMDWIN))
	    {
		// Command not allowed in the command line window
		errormsg = _(e_invalid_in_cmdline_window);
		ଯାଅ doend;
	    }
	    ଯଦି (text_locked() && !(ea.argt & EX_LOCK_OK))
	    {
		// Command not allowed when text is locked
		errormsg = _(get_text_locked_ସନ୍ଦେଶ());
		ଯାଅ doend;
	    }
	}

	// Disallow editing another buffer when "curbuf_lock" is set.
	// Do allow ":checktime" (it is postponed).
	// Do allow ":edit" (check for an argument later).
	// Do allow ":file" with no arguments (check for an argument later).
	ଯଦି (!(ea.argt & (EX_CMDWIN | EX_LOCK_OK))
		&& ea.cmdidx != CMD_checktime
		&& ea.cmdidx != CMD_edit
		&& ea.cmdidx != CMD_file
		&& !IS_USER_CMDIDX(ea.cmdidx)
		&& curbuf_locked())
	    ଯାଅ doend;

	ଯଦି (!ni && !(ea.argt & EX_RANGE) && ea.addr_ଗଣନା > 0)
	{
	    errormsg = _(e_no_range_allowed);
	    ଯାଅ doend;
	}
    }

    ଯଦି (!ni && !(ea.argt & EX_BANG) && ea.forceit)
    {
	errormsg = _(e_no_bang_allowed);
	ଯାଅ doend;
    }

    /*
     * Don't complain about the range if it is not used
     * (could happen if line_count is accidentally set to 0).
     */
    ଯଦି (!ea.skip && !ni && (ea.argt & EX_RANGE))
    {
	/*
	 * If the range is backwards, ask for confirmation and, if given, swap
	 * ea.line1 & ea.line2 so it's forwards again.
	 * When global command is busy, don't ask, will fail below.
	 */
	ଯଦି (!global_busy && ea.line1 > ea.line2)
	{
	    ଯଦି (ସନ୍ଦେଶ_silent == 0)
	    {
		ଯଦି (sourcing || exmode_active)
		{
		    errormsg = _(e_backwards_range_given);
		    ଯାଅ doend;
		}
		ଯଦି (ask_yesno((char_u *)
			_("Backwards range given, OK to swap"), FALSE) != 'y')
		    ଯାଅ doend;
	    }
	    lnum = ea.line1;
	    ea.line1 = ea.line2;
	    ea.line2 = lnum;
	}
	ଯଦି ((errormsg = invalid_range(&ea)) != ଖାଲି)
	    ଯାଅ doend;
    }

    ଯଦି ((ea.addr_type == ADDR_OTHER) && ea.addr_ଗଣନା == 0)
	// default is 1, not cursor
	ea.line2 = 1;

    correct_range(&ea);

#ifdef FEAT_FOLDING
    ଯଦି (((ea.argt & EX_WHOLEFOLD) || ea.addr_ଗଣନା >= 2) && !global_busy
	    && ea.addr_type == ADDR_ଧାଡ଼ିଗୁଡ଼ିକ)
    {
	// Put the first line at the start of a closed fold, put the last line
	// at the end of a closed fold.
	(ଶୂନ୍ୟ)hasFolding(ea.line1, &ea.line1, ଖାଲି);
	(ଶୂନ୍ୟ)hasFolding(ea.line2, ଖାଲି, &ea.line2);
    }
#endif

#ifdef FEAT_QUICKFIX
    /*
     * For the ":make" and ":grep" commands we insert the 'makeprg'/'grepprg'
     * option here, so things like % get expanded.
     */
    p = replace_makeprg(&ea, p, cmdlinep);
    ଯଦି (p == ଖାଲି)
	ଯାଅ doend;
#endif

    /*
     * Skip to start of argument.
     * Don't do this for the ":!" command, because ":!! -l" needs the space.
     */
    ଯଦି (ea.cmdidx == CMD_bang)
	ea.arg = p;
    ଅନ୍ୟଥା
	ea.arg = skipwhite(p);

    // ":file" cannot be run with an argument when "curbuf_lock" is set
    ଯଦି (ea.cmdidx == CMD_file && *ea.arg != NUL && curbuf_locked())
	ଯାଅ doend;

    /*
     * Check for "++opt=val" argument.
     * Must be first, allow ":w ++enc=utf8 !cmd"
     */
    ଯଦି (ea.argt & EX_ARGOPT)
	ଯେତେବେଳେ (ea.arg[0] == '+' && ea.arg[1] == '+')
	    ଯଦି (getargopt(&ea) == FAIL && !ni)
	    {
		errormsg = _(e_invalid_argument);
		ଯାଅ doend;
	    }

    ଯଦି (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	ଯଦି (*ea.arg == '>')			// append
	{
	    ଯଦି (*++ea.arg != '>')		// typed wrong
	    {
		errormsg = _(e_use_w_or_w_gt_gt);
		ଯାଅ doend;
	    }
	    ea.arg = skipwhite(ea.arg + 1);
	    ea.append = TRUE;
	}
	ଅନ୍ୟଥା ଯଦି (*ea.arg == '!' && ea.cmdidx == CMD_write)  // :w !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    ଯଦି (ea.cmdidx == CMD_read)
    {
	ଯଦି (ea.forceit)
	{
	    ea.usefilter = TRUE;		// :r! filter if ea.forceit
	    ea.forceit = FALSE;
	}
	ଅନ୍ୟଥା ଯଦି (*ea.arg == '!')		// :r !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    ଯଦି (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ea.amount = 1;
	ଯେତେବେଳେ (*ea.arg == *ea.cmd)		// count number of '>' or '<'
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
    ଯଦି ((ea.argt & EX_CMDARG) && !ea.usefilter)
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
    ଯଦି ((ea.argt & EX_TRLBAR) && !ea.usefilter)
    {
	separate_nextcmd(&ea, FALSE);
    }
    ଅନ୍ୟଥା ଯଦି (ea.cmdidx == CMD_bang
	    || ea.cmdidx == CMD_terminal
	    || ea.cmdidx == CMD_global
	    || ea.cmdidx == CMD_vglobal
	    || ea.usefilter
#ifdef FEAT_EVAL
	    || inside_block(&ea)
#endif
	    )
    {
	ପାଇଁ (p = ea.arg; *p; ++p)
	{
	    // Remove one backslash before a newline
	    ଯଦି (*p == '\\' && p[1] == '\n')
		STRMOVE(p, p + 1);
	    ଅନ୍ୟଥା ଯଦି (*p == '\n' && !(ea.argt & EX_ପଦସମଷ୍ଟି_ARG))
	    {
		ea.nextcmd = p + 1;
		*p = NUL;
		ଭାଙ୍ଗ;
	    }
	}
    }

    ଯଦି ((ea.argt & EX_DFLALL) && ea.addr_ଗଣନା == 0)
	address_default_all(&ea);

    // accept numbered register only when no count allowed (:put)
    ଯଦି (       (ea.argt & EX_REGSTR)
	    && *ea.arg != NUL
	       // Do not allow register = for user commands
	    && (!IS_USER_CMDIDX(ea.cmdidx) || *ea.arg != '=')
	    && !((ea.argt & EX_ଗଣନା) && VIM_ISDIGIT(*ea.arg)))
    {
#ଯଦି !defined(FEAT_CLIPBOARD) && !defined(FEAT_CLIPBOARD_PROVIDER)
	// check these explicitly for a more specific error message
	ଯଦି (*ea.arg == '*' || *ea.arg == '+')
	{
	    errormsg = _(e_invalid_register_name);
	    ଯାଅ doend;
	}
#endif
	ଯଦି (valid_yank_reg(*ea.arg, (!IS_USER_CMDIDX(ea.cmdidx)
			    && ea.cmdidx != CMD_put && ea.cmdidx != CMD_iput)))
	{
	    ea.regname = *ea.arg++;
#ifdef FEAT_EVAL
	    // for '=' register: accept the rest of the line as an expression
	    ଯଦି (ea.arg[-1] == '=' && ea.arg[0] != NUL)
	    {
		ଯଦି (!ea.skip)
		{
		    set_ପଦସମଷ୍ଟି_ଧାଡ଼ି(vim_strsave(ea.arg), &ea);
		    did_set_ପଦସମଷ୍ଟି_ଧାଡ଼ି = TRUE;
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
    ଯଦି ((ea.argt & EX_ଗଣନା) && VIM_ISDIGIT(*ea.arg)
	    && (!(ea.argt & EX_BUFNAME) || *(p = skipdigits(ea.arg + 1)) == NUL
							  || VIM_ISWHITE(*p)))
    {
	n = getdigits_quoted(&ea.arg);
	ea.arg = skipwhite(ea.arg);
	ଯଦି (n <= 0 && !ni && (ea.argt & EX_ZEROR) == 0)
	{
	    errormsg = _(e_positive_ଗଣନା_required);
	    ଯାଅ doend;
	}
	ଯଦି (ea.addr_type != ADDR_ଧାଡ଼ିଗୁଡ଼ିକ)	// e.g. :buffer 2, :sleep 3
	{
	    ea.line2 = n;
	    ଯଦି (ea.addr_ଗଣନା == 0)
		ea.addr_ଗଣନା = 1;
	}
	ଅନ୍ୟଥା
	{
	    ea.line1 = ea.line2;
	    ଯଦି (ea.line2 >= LONG_MAX - (n - 1))
		ea.line2 = LONG_MAX;  // avoid overflow
	    ଅନ୍ୟଥା
		ea.line2 += n - 1;
	    ++ea.addr_ଗଣନା;
	    /*
	     * Be vi compatible: no error message for out of range.
	     */
	    ଯଦି (ea.line2 > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		ea.line2 = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	}
    }

    /*
     * Check for flags: 'l', 'p' and '#'.
     */
    ଯଦି (ea.argt & EX_FLAGS)
	get_flags(&ea);
    ଯଦି (!ni && !(ea.argt & EX_EXTRA) && *ea.arg != NUL
	    && *ea.arg != '"' && (*ea.arg != '|' || (ea.argt & EX_TRLBAR) == 0))
    {
	// no arguments allowed but there is something
	errormsg = ex_errmsg(e_trailing_characters_str, ea.arg);
	ଯାଅ doend;
    }

    ଯଦି (!ni && (ea.argt & EX_NEEDARG) && *ea.arg == NUL)
    {
	errormsg = _(e_argument_required);
	ଯାଅ doend;
    }

#ifdef FEAT_EVAL
    /*
     * Skip the command when it's not going to be executed.
     * The commands like :if, :endif, etc. always need to be executed.
     * Also make an exception for commands that handle a trailing command
     * themselves.
     */
    ଯଦି (ea.skip)
    {
	ବାଛ (ea.cmdidx)
	{
	    // commands that need evaluation
	    ସ୍ଥିତି CMD_while:
	    ସ୍ଥିତି CMD_endwhile:
	    ସ୍ଥିତି CMD_for:
	    ସ୍ଥିତି CMD_endfor:
	    ସ୍ଥିତି CMD_if:
	    ସ୍ଥିତି CMD_elseif:
	    ସ୍ଥିତି CMD_else:
	    ସ୍ଥିତି CMD_endif:
	    ସ୍ଥିତି CMD_try:
	    ସ୍ଥିତି CMD_catch:
	    ସ୍ଥିତି CMD_finally:
	    ସ୍ଥିତି CMD_endtry:
	    ସ୍ଥିତି CMD_function:
	    ସ୍ଥିତି CMD_def:
				ଭାଙ୍ଗ;

	    // commands that read a block of lines
	    ସ୍ଥିତି CMD_abstract:
	    ସ୍ଥିତି CMD_append:
	    ସ୍ଥିତି CMD_autocmd:
	    ସ୍ଥିତି CMD_change:
	    ସ୍ଥିତି CMD_class:
	    ସ୍ଥିତି CMD_command:
	    ସ୍ଥିତି CMD_enum:
	    ସ୍ଥିତି CMD_ଭର୍ତ୍ତି:
	    ସ୍ଥିତି CMD_interface:
	    ସ୍ଥିତି CMD_loadkeymap:
				ଭାଙ୍ଗ;

	    // Commands that handle '|' themselves.  Check: A command should
	    // either have the EX_TRLBAR flag, appear in this list or appear in
	    // the list at ":help :bar".
	    ସ୍ଥିତି CMD_aboveleft:
	    ସ୍ଥିତି CMD_and:
	    ସ୍ଥିତି CMD_belowright:
	    ସ୍ଥିତି CMD_botright:
	    ସ୍ଥିତି CMD_browse:
	    ସ୍ଥିତି CMD_call:
	    ସ୍ଥିତି CMD_confirm:
	    ସ୍ଥିତି CMD_const:
	    ସ୍ଥିତି CMD_delfunction:
	    ସ୍ଥିତି CMD_djump:
	    ସ୍ଥିତି CMD_dlist:
	    ସ୍ଥିତି CMD_dsearch:
	    ସ୍ଥିତି CMD_dsplit:
	    ସ୍ଥିତି CMD_echo:
	    ସ୍ଥିତି CMD_echoerr:
	    ସ୍ଥିତି CMD_echomsg:
	    ସ୍ଥିତି CMD_echon:
	    ସ୍ଥିତି CMD_eval:
	    ସ୍ଥିତି CMD_execute:
	    ସ୍ଥିତି CMD_filter:
	    ସ୍ଥିତି CMD_final:
	    ସ୍ଥିତି CMD_help:
	    ସ୍ଥିତି CMD_hide:
	    ସ୍ଥିତି CMD_horizontal:
	    ସ୍ଥିତି CMD_ijump:
	    ସ୍ଥିତି CMD_ilist:
	    ସ୍ଥିତି CMD_isearch:
	    ସ୍ଥିତି CMD_isplit:
	    ସ୍ଥିତି CMD_keepalt:
	    ସ୍ଥିତି CMD_keepjumps:
	    ସ୍ଥିତି CMD_keepmarks:
	    ସ୍ଥିତି CMD_keeppatterns:
	    ସ୍ଥିତି CMD_leftabove:
	    ସ୍ଥିତି CMD_let:
	    ସ୍ଥିତି CMD_lockmarks:
	    ସ୍ଥିତି CMD_lockvar:
	    ସ୍ଥିତି CMD_lua:
	    ସ୍ଥିତି CMD_match:
	    ସ୍ଥିତି CMD_mzscheme:
	    ସ୍ଥିତି CMD_noautocmd:
	    ସ୍ଥିତି CMD_noswapfile:
	    ସ୍ଥିତି CMD_perl:
	    ସ୍ଥିତି CMD_psearch:
	    ସ୍ଥିତି CMD_py3:
	    ସ୍ଥିତି CMD_python3:
	    ସ୍ଥିତି CMD_python:
	    ସ୍ଥିତି CMD_return:
	    ସ୍ଥିତି CMD_rightbelow:
	    ସ୍ଥିତି CMD_ruby:
	    ସ୍ଥିତି CMD_silent:
	    ସ୍ଥିତି CMD_smagic:
	    ସ୍ଥିତି CMD_snomagic:
	    ସ୍ଥିତି CMD_substitute:
	    ସ୍ଥିତି CMD_syntax:
	    ସ୍ଥିତି CMD_tab:
	    ସ୍ଥିତି CMD_tcl:
	    ସ୍ଥିତି CMD_throw:
	    ସ୍ଥିତି CMD_tilde:
	    ସ୍ଥିତି CMD_topleft:
	    ସ୍ଥିତି CMD_unlet:
	    ସ୍ଥିତି CMD_unlockvar:
	    ସ୍ଥିତି CMD_var:
	    ସ୍ଥିତି CMD_verbose:
	    ସ୍ଥିତି CMD_vertical:
	    ସ୍ଥିତି CMD_wincmd:
				ଭାଙ୍ଗ;

	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:		ଯାଅ doend;
	}
    }
#endif

    ଯଦି ((ea.argt & EX_XFILE)
	    && expand_filename(&ea, cmdlinep, &errormsg) == FAIL)
	ଯାଅ doend;

#ifdef FEAT_EVAL
    ଯଦି (is_export && (ea.argt & EX_EXPORT) == 0)
    {
	emsg(_(e_invalid_command_after_export));
	ଯାଅ doend;
    }
#endif

    /*
     * Accept buffer name.  Cannot be used at the same time with a buffer
     * number.  Don't do this for a user command.
     */
    ଯଦି ((ea.argt & EX_BUFNAME) && *ea.arg != NUL && ea.addr_ଗଣନା == 0
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * :bdelete, :bwipeout and :bunload take several arguments, separated
	 * by spaces: find next space (skipping over escaped characters).
	 * The others take one argument: ignore trailing spaces.
	 */
	ଯଦି (ea.cmdidx == CMD_bdelete || ea.cmdidx == CMD_bwipeout
						  || ea.cmdidx == CMD_bunload)
	    p = skiptowhite_esc(ea.arg);
	ଅନ୍ୟଥା
	{
	    p = ea.arg + STRLEN(ea.arg);
	    ଯେତେବେଳେ (p > ea.arg && VIM_ISWHITE(p[-1]))
		--p;
	}
	ea.line2 = buflist_findpat(ea.arg, p, (ea.argt & EX_BUFUNL) != 0,
								FALSE, FALSE);
	ଯଦି (ea.line2 < 0)	    // failed
	    ଯାଅ doend;
	ea.addr_ଗଣନା = 1;
	ea.arg = skipwhite(p);
    }

    // The :try command saves the emsg_silent flag, reset it here when
    // ":silent! try" was used, it should only apply to :try itself.
    ଯଦି (ea.cmdidx == CMD_try && cmdmod.cmod_did_esilent > 0)
    {
	emsg_silent -= cmdmod.cmod_did_esilent;
	ଯଦି (emsg_silent < 0)
	    emsg_silent = 0;
	cmdmod.cmod_did_esilent = 0;
    }

/*
 * 7. Execute the command.
 */

    ଯଦି (IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * Execute a user-defined command.
	 */
	do_ucmd(&ea);
    }
    ଅନ୍ୟଥା
    {
	/*
	 * Call the function to execute the builtin command.
	 */
	(cmdnames[ea.cmdidx].cmd_func)(&ea);
	ଯଦି (ea.errmsg != ଖାଲି)
	    errormsg = ea.errmsg;
    }

#ifdef FEAT_EVAL
    // A command will reset "is_export" when exporting an item.  If it is still
    // set something went wrong or the command was never executed.
    ଯଦି (!ea.skip && is_export)
    {
	ଯଦି (errormsg == ଖାଲି)
	    errormsg = _(e_export_with_invalid_argument);
	is_export = FALSE;
    }

    // Set flag that any command was executed, used by ex_vim9script().
    // Not if this was a command that wasn't executed or :endif.
    ଯଦି (sourcing_a_script(&ea)
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
    ଯଦି (need_rethrow)
	do_throw(cstack);
    ଅନ୍ୟଥା ଯଦି (check_cstack)
    {
	ଯଦି (source_finished(fgetline, cookie))
	    do_finish(&ea, TRUE);
	ଅନ୍ୟଥା ଯଦି (getline_equal(fgetline, cookie, get_func_ଧାଡ଼ି)
						   && current_func_returned())
	    do_return(&ea, TRUE, FALSE, ଖାଲି);
    }
    need_rethrow = check_cstack = FALSE;
#endif

doend:
    ଯଦି (curwin->w_cursor.lnum == 0)	// can happen with zero line number
    {
	curwin->w_cursor.lnum = 1;
	curwin->w_cursor.col = 0;
    }

    ଯଦି (errormsg != ଖାଲି && *errormsg != NUL && !did_emsg)
    {
	ଯଦି ((sourcing || !ଚାବି_Typed) && !did_append_cmd)
	{
	    ଯଦି (errormsg != (ଅକ୍ଷର *)IObuff)
	    {
		STRCPY(IObuff, errormsg);
		errormsg = (ଅକ୍ଷର *)IObuff;
	    }
	    append_command(*cmdlinep);
	}
	emsg(errormsg);
    }
#ifdef FEAT_EVAL
    do_errthrow(cstack,
	    (ea.cmdidx != CMD_SIZE && !IS_USER_CMDIDX(ea.cmdidx))
			? cmdnames[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ea.cmdidx].cmd_name : (char_u *)ଖାଲି);

    ଯଦି (did_set_ପଦସମଷ୍ଟି_ଧାଡ଼ି)
	set_ପଦସମଷ୍ଟି_ଧାଡ଼ି(ଖାଲି, ଖାଲି);
    is_export = FALSE;
#endif

    undo_cmdmod(&cmdmod);
    cmdmod = save_cmdmod;
    reg_executing = save_reg_executing;
    pending_end_reg_executing = save_pending_end_reg_executing;

    ଯଦି (ea.nextcmd && *ea.nextcmd == NUL)	// not really a next command
	ea.nextcmd = ଖାଲି;

#ifdef FEAT_EVAL
    --ex_nesting_level;
    vim_free(ea.cmdline_tofree);
#endif

    ଫେରନ୍ତୁ ea.nextcmd;
}

ସ୍ଥିର ଅକ୍ଷର ex_error_buf[ସନ୍ଦେଶ_BUF_LEN];

/*
 * Return an error message with argument included.
 * Uses a static buffer, only the last error will be kept.
 * "msg" will be translated, caller should use N_().
 */
    ଅକ୍ଷର *
ex_errmsg(ଅକ୍ଷର *ସନ୍ଦେଶ, char_u *arg)
{
    vim_snprintf(ex_error_buf, ସନ୍ଦେଶ_BUF_LEN, _(ସନ୍ଦେଶ), arg);
    ଫେରନ୍ତୁ ex_error_buf;
}

/*
 * The "+" string used in place of an empty command in Ex mode.
 * This string is used in pointer comparison.
 */
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର exmode_plus[] = "+";

/*
 * Handle a range without a command.
 * Returns an error message on failure.
 */
    ଅକ୍ଷର *
ex_range_without_command(exarg_T *eap)
{
    ଅକ୍ଷର *errormsg = ଖାଲି;

    ଯଦି ((*eap->cmd == '|' ||
		(exmode_active && eap->cmd != (char_u *)exmode_plus + 1))
#ifdef FEAT_EVAL
	    && !in_vim9script()
#endif
       )
    {
	eap->cmdidx = CMD_print;
	eap->argt = EX_RANGE+EX_ଗଣନା+EX_TRLBAR;
	ଯଦି ((errormsg = invalid_range(eap)) == ଖାଲି)
	{
	    correct_range(eap);
	    ex_print(eap);
	}
    }
    ଅନ୍ୟଥା ଯଦି (eap->addr_ଗଣନା != 0)
    {
	ଯଦି (eap->line2 > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    // With '-' in 'cpoptions' a line number past the file is an
	    // error, otherwise put it at the end of the file.
	    ଯଦି (vim_strchr(p_cpo, CPO_MINUS) != ଖାଲି)
		eap->line2 = -1;
	    ଅନ୍ୟଥା
		eap->line2 = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	}

	ଯଦି (eap->line2 < 0)
	    errormsg = _(e_invalid_range);
	ଅନ୍ୟଥା
	{
	    ଯଦି (eap->line2 == 0)
		curwin->w_cursor.lnum = 1;
	    ଅନ୍ୟଥା
		curwin->w_cursor.lnum = eap->line2;
	    beginline(BL_SOL | BL_FIX);
	}
    }
    ଫେରନ୍ତୁ errormsg;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 * If "noparen" is TRUE do not recognize the command followed by "(" or ".".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
checkforcmd_opt(
    char_u	**pp,		// start of command
    ଅକ୍ଷର	*cmd,		// name of command
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,		// required length
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		noparen)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; cmd[i] != NUL; ++i)
	ଯଦି (((char_u *)cmd)[i] != (*pp)[i])
	    ଭାଙ୍ଗ;
    ଯଦି (i >= len && !ASCII_ISALPHA((*pp)[i]) && (*pp)[i] != '_'
			 && (!noparen || ((*pp)[i] != '(' && (*pp)[i] != '.')))
    {
	*pp = skipwhite(*pp + i);
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
checkforcmd(
    char_u	**pp,		// start of command
    ଅକ୍ଷର	*cmd,		// name of command
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len)		// required length
{
    ଫେରନ୍ତୁ checkforcmd_opt(pp, cmd, len, FALSE);
}

/*
 * Check for an Ex command with optional tail, not followed by "(" or ".".
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
checkforcmd_noparen(
    char_u	**pp,		// start of command
    ଅକ୍ଷର	*cmd,		// name of command
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len)		// required length
{
    ଫେରନ୍ତୁ checkforcmd_opt(pp, cmd, len, TRUE);
}

/*
 * Find the replacement text of a ":command", the part after the attributes and
 * the command name.  Returns NULL when there is none.  Does not modify "arg"
 * and gives no error messages.
 */
    ସ୍ଥିର char_u *
find_ucmd_repl(char_u *arg)
{
    char_u	*p = arg;

    // Skip over the attributes.
    ଯେତେବେଳେ (*p == '-')
	p = skipwhite(skiptowhite(p));

    // Skip over the command name.
    ଯଦି (!ASCII_ISALPHA(*p))
	ଫେରନ୍ତୁ ଖାଲି;
    ଯେତେବେଳେ (ASCII_ISALNUM(*p))
	++p;

    ଫେରନ୍ତୁ *p == NUL ? ଖାଲି : skipwhite(p);
}

/*
 * Find the "{" in "line" that starts a block for ":command" or ":autocmd".
 * That is the case when the command argument is "{" at the end of the line,
 * also when the command is nested in another ":command" or ":autocmd".
 * Returns NULL when the line does not start such a block.
 */
    char_u *
find_cmd_block_start(char_u *ଧାଡ଼ି)
{
    char_u	*p = skipwhite(ଧାଡ଼ି);

    ପାଇଁ (;;)
    {
	char_u	*arg = p;

	ଯଦି (*p == '{' && ends_excmd2(p, skipwhite(p + 1)))
	    ଫେରନ୍ତୁ p;

	ଯଦି (checkforcmd_noparen(&arg, "autocmd", 2))
	{
	    ଯଦି (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = au_find_cmd_arg(arg);
	}
	ଅନ୍ୟଥା ଯଦି (checkforcmd_noparen(&arg, "command", 3))
	{
	    ଯଦି (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = find_ucmd_repl(arg);
	}
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ ଖାଲି;

	ଯଦି (p == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_command_modifiers(
	exarg_T	    *eap,
	ଅକ୍ଷର	    **errormsg,
	cmdmod_T    *cmod,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    skip_only)
{
    char_u  *orig_cmd = eap->cmd;
    char_u  *cmd_start = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    use_plus_cmd = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    starts_with_colon = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    vim9script = in_vim9script();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    has_visual_range = FALSE;

    CLEAR_POINTER(cmod);
    cmod->cmod_flags = sticky_cmdmod_flags;

    ଯଦି (STRNCMP(eap->cmd, "'<,'>", 5) == 0)
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
    ପାଇଁ (;;)
    {
	char_u  *p;

	ଯେତେବେଳେ (*eap->cmd == ' ' || *eap->cmd == '\t' || *eap->cmd == ':')
	{
	    ଯଦି (*eap->cmd == ':')
		starts_with_colon = TRUE;
	    ++eap->cmd;
	}

	// in ex mode, an empty command (after modifiers) works like :+
	ଯଦି (*eap->cmd == NUL && exmode_active
		   && (getline_equal(eap->ea_getline, eap->cookie, getexmodeline)
		       || getline_equal(eap->ea_getline, eap->cookie, getexline))
			&& curwin->w_cursor.lnum < curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    use_plus_cmd = TRUE;
	    ଯଦି (!skip_only)
		ex_pressedreturn = TRUE;
	    ଭାଙ୍ଗ;  // no modifiers following
	}

	// ignore comment and empty lines
	ଯଦି (comment_start(eap->cmd, starts_with_colon))
	{
	    // a comment ends at a NL
	    eap->nextcmd = vim_strchr(eap->cmd, '\n');
	    ଯଦି (eap->nextcmd != ଖାଲି)
		++eap->nextcmd;
	    ଯଦି (vim9script)
	    {
		ଯଦି (has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
#ifdef FEAT_EVAL
		ଯଦି (eap->cmd[0] == '#' && eap->cmd[1] == '{'
							 && eap->cmd[2] != '{')
		    *errormsg = _(e_cannot_use_hash_curly_to_start_comment);
#endif
	    }
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (*eap->cmd == '\n')
	{
	    eap->nextcmd = eap->cmd + 1;
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (*eap->cmd == NUL)
	{
	    ଯଦି (!skip_only)
	    {
		ex_pressedreturn = TRUE;
		ଯଦି (vim9script && has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
	    }
	    ଫେରନ୍ତୁ FAIL;
	}

	p = skip_range(eap->cmd, TRUE, ଖାଲି);

	// In Vim9 script a variable can shadow a command modifier:
	//   verbose = 123
	//   verbose += 123
	//   silent! verbose = func()
	//   verbose.member = 2
	//   verbose[expr] = 2
	// But not:
	//   verbose [a, b] = list
	ଯଦି (vim9script)
	{
	    char_u *s, *n;

	    ପାଇଁ (s = eap->cmd; ASCII_ISALPHA(*s); ++s)
		;
	    n = skipwhite(s);
	    ଯଦି (*n == '.' || *n == '=' || (*n != NUL && n[1] == '=')
		    || *s == '[')
		ଭାଙ୍ଗ;
	}

	ବାଛ (*p)
	{
	    // When adding an entry, also modify cmdmod_info_tab[].
	    ସ୍ଥିତି 'a':	ଯଦି (!checkforcmd_noparen(&eap->cmd, "aboveleft", 3))
			    ଭାଙ୍ଗ;
			cmod->cmod_split |= WSP_ABOVE;
			ଜାରି;

	    ସ୍ଥିତି 'b':	ଯଦି (checkforcmd_noparen(&eap->cmd, "belowright", 3))
			{
			    cmod->cmod_split |= WSP_BELOW;
			    ଜାରି;
			}
			ଯଦି (checkforcmd_opt(&eap->cmd, "browse", 3, TRUE))
			{
#ifdef FEAT_BROWSE_CMD
			    cmod->cmod_flags |= CMOD_BROWSE;
#endif
			    ଜାରି;
			}
			ଯଦି (!checkforcmd_noparen(&eap->cmd, "botright", 2))
			    ଭାଙ୍ଗ;
			cmod->cmod_split |= WSP_BOT;
			ଜାରି;

	    ସ୍ଥିତି 'c':	ଯଦି (!checkforcmd_opt(&eap->cmd, "confirm", 4, TRUE))
			    ଭାଙ୍ଗ;
#ଯଦି defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			cmod->cmod_flags |= CMOD_CONFIRM;
#endif
			ଜାରି;

	    ସ୍ଥିତି 'k':	ଯଦି (checkforcmd_noparen(&eap->cmd, "keepmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_KEEPMARKS;
			    ଜାରି;
			}
			ଯଦି (checkforcmd_noparen(&eap->cmd, "keepalt", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPALT;
			    ଜାରି;
			}
			ଯଦି (checkforcmd_noparen(&eap->cmd, "keeppatterns", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPPATTERNS;
			    ଜାରି;
			}
			ଯଦି (!checkforcmd_noparen(&eap->cmd, "keepjumps", 5))
			    ଭାଙ୍ଗ;
			cmod->cmod_flags |= CMOD_KEEPJUMPS;
			ଜାରି;

	    ସ୍ଥିତି 'f':	// only accept ":filter {pat} cmd"
			{
			    char_u  *reg_pat;
			    char_u  *nulp = ଖାଲି;
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c = 0;

			    ଯଦି (!checkforcmd_noparen(&p, "filter", 4)
				    || *p == NUL
				    || (ends_excmd(*p)
#ifdef FEAT_EVAL
					// in ":filter #pat# cmd" # does not
					// start a comment
				     && (!vim9script || VIM_ISWHITE(p[1]))
#endif
				     ))
				ଭାଙ୍ଗ;
			    ଯଦି (*p == '!')
			    {
				cmod->cmod_filter_force = TRUE;
				p = skipwhite(p + 1);
				ଯଦି (*p == NUL || ends_excmd(*p))
				    ଭାଙ୍ଗ;
			    }
#ifdef FEAT_EVAL
			    // Avoid that "filter(arg)" is recognized.
			    ଯଦି (vim9script && !VIM_ISWHITE(p[-1]))
				ଭାଙ୍ଗ;
#endif
			    ଯଦି (skip_only)
				p = skip_vimgrep_pat(p, ଖାଲି, ଖାଲି);
			    ଅନ୍ୟଥା
				// NOTE: This puts a NUL after the pattern.
				p = skip_vimgrep_pat_ext(p, &reg_pat, ଖାଲି,
								    &nulp, &c);
			    ଯଦି (p == ଖାଲି || *p == NUL)
				ଭାଙ୍ଗ;
			    ଯଦି (!skip_only)
			    {
				cmod->cmod_filter_regmatch.regprog =
						vim_regcomp(reg_pat, RE_MAGIC);
				ଯଦି (cmod->cmod_filter_regmatch.regprog == ଖାଲି)
				    ଭାଙ୍ଗ;
				// restore the character overwritten by NUL
				ଯଦି (nulp != ଖାଲି)
				    *nulp = c;
			    }
			    eap->cmd = p;
			    ଜାରି;
			}

	    ସ୍ଥିତି 'h':	ଯଦି (checkforcmd_noparen(&eap->cmd, "horizontal", 3))
			{
			    cmod->cmod_split |= WSP_HOR;
			    ଜାରି;
			}
			// ":hide" and ":hide | cmd" are not modifiers
			ଯଦି (p != eap->cmd || !checkforcmd_noparen(&p, "hide", 3)
					       || *p == NUL || ends_excmd(*p))
			    ଭାଙ୍ଗ;
			eap->cmd = p;
			cmod->cmod_flags |= CMOD_HIDE;
			ଜାରି;

	    ସ୍ଥିତି 'l':	ଯଦି (checkforcmd_noparen(&eap->cmd, "lockmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_LOCKMARKS;
			    ଜାରି;
			}
			ଯଦି (checkforcmd_noparen(&eap->cmd, "legacy", 3))
			{
			    ଯଦି (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_legacy_must_be_followed_by_command);
				ଫେରନ୍ତୁ FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_VIM9CMD;
			    cmod->cmod_flags |= CMOD_LEGACY;
			    ଜାରି;
			}

			ଯଦି (!checkforcmd_noparen(&eap->cmd, "leftabove", 5))
			    ଭାଙ୍ଗ;
			cmod->cmod_split |= WSP_ABOVE;
			ଜାରି;

	    ସ୍ଥିତି 'n':	ଯଦି (checkforcmd_noparen(&eap->cmd, "noautocmd", 3))
			{
			    cmod->cmod_flags |= CMOD_NOAUTOCMD;
			    ଜାରି;
			}
			ଯଦି (!checkforcmd_noparen(&eap->cmd, "noswapfile", 3))
			    ଭାଙ୍ଗ;
			cmod->cmod_flags |= CMOD_NOSWAPFILE;
			ଜାରି;

	    ସ୍ଥିତି 'r':	ଯଦି (!checkforcmd_noparen(&eap->cmd, "rightbelow", 6))
			    ଭାଙ୍ଗ;
			cmod->cmod_split |= WSP_BELOW;
			ଜାରି;

	    ସ୍ଥିତି 's':	ଯଦି (checkforcmd_noparen(&eap->cmd, "sandbox", 3))
			{
			    cmod->cmod_flags |= CMOD_SANDBOX;
			    ଜାରି;
			}
			ଯଦି (!checkforcmd_noparen(&eap->cmd, "silent", 3))
			    ଭାଙ୍ଗ;
			cmod->cmod_flags |= CMOD_SILENT;
			ଯଦି (*eap->cmd == '!' && !VIM_ISWHITE(eap->cmd[-1]))
			{
			    // ":silent!", but not "silent !cmd"
			    eap->cmd = skipwhite(eap->cmd + 1);
			    cmod->cmod_flags |= CMOD_ERRSILENT;
			}
			ଜାରି;

	    ସ୍ଥିତି 't':	ଯଦି (checkforcmd_noparen(&p, "tab", 3))
			{
			    ଯଦି (!skip_only)
			    {
				long tabnr = get_address(eap, &eap->cmd,
						    ADDR_TABS, eap->skip,
						    skip_only, FALSE, 1);
				ଯଦି (tabnr == MAXLNUM)
				    cmod->cmod_tab = tabpage_index(curtab) + 1;
				ଅନ୍ୟଥା
				{
				    ଯଦି (tabnr < 0 || tabnr > LAST_TAB_NR)
				    {
					*errormsg = _(e_invalid_range);
					ଫେରନ୍ତୁ FAIL;
				    }
				    cmod->cmod_tab = tabnr + 1;
				}
			    }
			    eap->cmd = p;
			    ଜାରି;
			}
			ଯଦି (!checkforcmd_noparen(&eap->cmd, "topleft", 2))
			    ଭାଙ୍ଗ;
			cmod->cmod_split |= WSP_ଶୀର୍ଷ;
			ଜାରି;

	    ସ୍ଥିତି 'u':	ଯଦି (!checkforcmd_noparen(&eap->cmd, "unsilent", 3))
			    ଭାଙ୍ଗ;
			cmod->cmod_flags |= CMOD_UNSILENT;
			ଜାରି;

	    ସ୍ଥିତି 'v':	ଯଦି (checkforcmd_noparen(&eap->cmd, "vertical", 4))
			{
			    cmod->cmod_split |= WSP_VERT;
			    ଜାରି;
			}
			ଯଦି (checkforcmd_noparen(&eap->cmd, "vim9cmd", 4))
			{
			    ଯଦି (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_vim9cmd_must_be_followed_by_command);
				ଫେରନ୍ତୁ FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_LEGACY;
			    cmod->cmod_flags |= CMOD_VIM9CMD;
			    ଜାରି;
			}
			ଯଦି (!checkforcmd_noparen(&p, "verbose", 4))
			    ଭାଙ୍ଗ;
			ଯଦି (vim_isdigit(*eap->cmd))
			{
			    // zero means not set, one is verbose == 0, etc.
			    cmod->cmod_verbose = atoi((ଅକ୍ଷର *)eap->cmd) + 1;
			}
			ଅନ୍ୟଥା
			    cmod->cmod_verbose = 2;  // default: verbose == 1
			eap->cmd = p;
			ଜାରି;
	}
	ଭାଙ୍ଗ;
    }

    ଯଦି (has_visual_range)
    {
	ଯଦି (eap->cmd > cmd_start)
	{
	    // Move the '<,'> range to after the modifiers and insert a colon.
	    // Since the modifiers have been parsed put the colon on top of the
	    // space: "'<,'>mod cmd" -> "mod:'<,'>cmd
	    // Put eap->cmd after the colon.
	    ଯଦି (use_plus_cmd)
	    {
		size_t len = STRLEN(cmd_start);

		// Special case: empty command uses "+":
		//  "'<,'>mods" -> "mods *+
		//  Use "*" instead of "'<,'>" to avoid the command getting
		//  longer, in case it was allocated.
		mch_memmove(orig_cmd, cmd_start, len);
		STRCPY(orig_cmd + len, " *+");
	    }
	    ଅନ୍ୟଥା
	    {
		mch_memmove(cmd_start - 5, cmd_start, eap->cmd - cmd_start);
		eap->cmd -= 5;
		mch_memmove(eap->cmd - 1, ":'<,'>", 6);
	    }
	}
	ଅନ୍ୟଥା
	    // No modifiers, move the pointer back.
	    // Special case: change empty command to "+".
	    ଯଦି (use_plus_cmd)
		eap->cmd = (char_u *)"'<,'>+";
	    ଅନ୍ୟଥା
		eap->cmd = orig_cmd;
    }
    ଅନ୍ୟଥା ଯଦି (use_plus_cmd)
	eap->cmd = (char_u *)exmode_plus;

    ଫେରନ୍ତୁ OK;
}

/*
 * Return TRUE if "cmod" has anything set.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_cmdmod(cmdmod_T *cmod, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ignore_silent)
{
    ଫେରନ୍ତୁ (cmod->cmod_flags != 0 && (!ignore_silent
		|| (cmod->cmod_flags
		      & ~(CMOD_SILENT | CMOD_ERRSILENT | CMOD_UNSILENT)) != 0))
	    || cmod->cmod_split != 0
	    || cmod->cmod_verbose > 0
	    || cmod->cmod_tab != 0
	    || cmod->cmod_filter_regmatch.regprog != ଖାଲି;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * If Vim9 script and "cmdmod" has anything set give an error and return TRUE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdmod_error(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ignore_silent)
{
    ଯଦି (in_vim9script() && has_cmdmod(&cmdmod, ignore_silent))
    {
	emsg(_(e_misplaced_command_modifier));
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}
#endif

/*
 * Apply the command modifiers.  Saves current state in "cmdmod", call
 * undo_cmdmod() later.
 */
    ଶୂନ୍ୟ
apply_cmdmod(cmdmod_T *cmod)
{
#ifdef HAVE_SANDBOX
    ଯଦି ((cmod->cmod_flags & CMOD_SANDBOX) && !cmod->cmod_did_sandbox)
    {
	++sandbox;
	cmod->cmod_did_sandbox = TRUE;
    }
#endif
    ଯଦି (cmod->cmod_verbose > 0)
    {
	ଯଦି (cmod->cmod_verbose_save == 0)
	    cmod->cmod_verbose_save = p_verbose + 1;
	p_verbose = cmod->cmod_verbose - 1;
    }

    ଯଦି ((cmod->cmod_flags & (CMOD_SILENT | CMOD_UNSILENT))
	    && cmod->cmod_save_ସନ୍ଦେଶ_silent == 0)
    {
	cmod->cmod_save_ସନ୍ଦେଶ_silent = ସନ୍ଦେଶ_silent + 1;
	cmod->cmod_save_ସନ୍ଦେଶ_scroll = ସନ୍ଦେଶ_scroll;
    }
    ଯଦି (cmod->cmod_flags & CMOD_SILENT)
	++ସନ୍ଦେଶ_silent;
    ଯଦି (cmod->cmod_flags & CMOD_UNSILENT)
	ସନ୍ଦେଶ_silent = 0;

    ଯଦି (cmod->cmod_flags & CMOD_ERRSILENT)
    {
	++emsg_silent;
	++cmod->cmod_did_esilent;
    }

    ଯଦି ((cmod->cmod_flags & CMOD_NOAUTOCMD) && cmod->cmod_save_ei == ଖାଲି)
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
    ଶୂନ୍ୟ
undo_cmdmod(cmdmod_T *cmod)
{
    ଯଦି (cmod->cmod_verbose_save > 0)
    {
	p_verbose = cmod->cmod_verbose_save - 1;
	cmod->cmod_verbose_save = 0;
    }

#ifdef HAVE_SANDBOX
    ଯଦି (cmod->cmod_did_sandbox)
    {
	--sandbox;
	cmod->cmod_did_sandbox = FALSE;
    }
#endif

    ଯଦି (cmod->cmod_save_ei != ଖାଲି)
    {
	// Restore 'eventignore' to the value before ":noautocmd".
	set_string_option_direct((char_u *)"ei", -1, cmod->cmod_save_ei,
							   OPT_FREE, SID_NONE);
	free_string_option(cmod->cmod_save_ei);
	cmod->cmod_save_ei = ଖାଲି;
    }

    vim_regfree(cmod->cmod_filter_regmatch.regprog);

    ଯଦି (cmod->cmod_save_ସନ୍ଦେଶ_silent > 0)
    {
	// messages could be enabled for a serious error, need to check if the
	// counters don't become negative
	ଯଦି (!did_emsg || ସନ୍ଦେଶ_silent > cmod->cmod_save_ସନ୍ଦେଶ_silent - 1)
	    ସନ୍ଦେଶ_silent = cmod->cmod_save_ସନ୍ଦେଶ_silent - 1;
	emsg_silent -= cmod->cmod_did_esilent;
	ଯଦି (emsg_silent < 0)
	    emsg_silent = 0;
	// Restore msg_scroll, it's set by file I/O commands, even when no
	// message is actually displayed.
	ସନ୍ଦେଶ_scroll = cmod->cmod_save_ସନ୍ଦେଶ_scroll;

	// "silent reg" or "silent echo x" inside "redir" leaves msg_col
	// somewhere in the line.  Put it back in the first column.
	ଯଦି (redirecting())
	    ସନ୍ଦେଶ_col = 0;

	cmod->cmod_save_ସନ୍ଦେଶ_silent = 0;
	cmod->cmod_did_esilent = 0;
    }
}

/*
 * Parse the address range, if any, in "eap".
 * May set the last search pattern, unless "silent" is TRUE.
 * Return FAIL and set "errormsg" or return OK.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_cmd_address(exarg_T *eap, ଅକ୍ଷର **errormsg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା silent)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		address_ଗଣନା = 1;
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_check_cursor = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FAIL;

    // Repeat for all ',' or ';' separated addresses.
    ପାଇଁ (;;)
    {
	eap->line1 = eap->line2;
	eap->line2 = default_address(eap);
	eap->cmd = skipwhite(eap->cmd);
	lnum = get_address(eap, &eap->cmd, eap->addr_type, eap->skip, silent,
					eap->addr_ଗଣନା == 0, address_ଗଣନା++);
	ଯଦି (eap->cmd == ଖାଲି)	// error detected
	    ଯାଅ theend;
	ଯଦି (lnum == MAXLNUM)
	{
	    ଯଦି (*eap->cmd == '%')   // '%' - all lines
	    {
		++eap->cmd;
		ବାଛ (eap->addr_type)
		{
		    ସ୍ଥିତି ADDR_ଧାଡ଼ିଗୁଡ଼ିକ:
		    ସ୍ଥିତି ADDR_OTHER:
			eap->line1 = 1;
			eap->line2 = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_LOADED_BUFFERS:
			{
			    buf_T	*buf = firstbuf;

			    ଯେତେବେଳେ (buf->b_next != ଖାଲି
						  && buf->b_ml.ml_mfp == ଖାଲି)
				buf = buf->b_next;
			    eap->line1 = buf->b_fnum;
			    buf = lastbuf;
			    ଯେତେବେଳେ (buf->b_prev != ଖାଲି
						  && buf->b_ml.ml_mfp == ଖାଲି)
				buf = buf->b_prev;
			    eap->line2 = buf->b_fnum;
			    ଭାଙ୍ଗ;
			}
		    ସ୍ଥିତି ADDR_BUFFERS:
			eap->line1 = firstbuf->b_fnum;
			eap->line2 = lastbuf->b_fnum;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_WINDOWS:
		    ସ୍ଥିତି ADDR_TABS:
			ଯଦି (IS_USER_CMDIDX(eap->cmdidx))
			{
			    eap->line1 = 1;
			    eap->line2 = eap->addr_type == ADDR_WINDOWS
						  ? LAST_WIN_NR : LAST_TAB_NR;
			}
			ଅନ୍ୟଥା
			{
			    // there is no Vim command which uses '%' and
			    // ADDR_WINDOWS or ADDR_TABS
			    *errormsg = _(e_invalid_range);
			    ଯାଅ theend;
			}
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_TABS_RELATIVE:
		    ସ୍ଥିତି ADDR_UNSIGNED:
		    ସ୍ଥିତି ADDR_QUICKFIX:
			*errormsg = _(e_invalid_range);
			ଯାଅ theend;
		    ସ୍ଥିତି ADDR_ARGUMENTS:
			ଯଦି (ARGCOUNT == 0)
			    eap->line1 = eap->line2 = 0;
			ଅନ୍ୟଥା
			{
			    eap->line1 = 1;
			    eap->line2 = ARGCOUNT;
			}
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			eap->line1 = 1;
			eap->line2 = qf_get_valid_size(eap);
			ଯଦି (eap->line2 == 0)
			    eap->line2 = 1;
#endif
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_NONE:
			// Will give an error later if a range is found.
			ଭାଙ୍ଗ;
		}
		++eap->addr_ଗଣନା;
	    }
	    ଅନ୍ୟଥା ଯଦି (*eap->cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == ଖାଲି)
	    {
		pos_T	    *fp;

		// '*' - visual area
		ଯଦି (eap->addr_type != ADDR_ଧାଡ଼ିଗୁଡ଼ିକ)
		{
		    *errormsg = _(e_invalid_range);
		    ଯାଅ theend;
		}

		++eap->cmd;
		ଯଦି (!eap->skip)
		{
		    fp = getmark('<', FALSE);
		    ଯଦି (check_mark(fp) == FAIL)
			ଯାଅ theend;
		    eap->line1 = fp->lnum;
		    fp = getmark('>', FALSE);
		    ଯଦି (check_mark(fp) == FAIL)
			ଯାଅ theend;
		    eap->line2 = fp->lnum;
		    ++eap->addr_ଗଣନା;
		}
	    }
	}
	ଅନ୍ୟଥା
	    eap->line2 = lnum;
	eap->addr_ଗଣନା++;

	ଯଦି (*eap->cmd == ';')
	{
	    ଯଦି (!eap->skip)
	    {
		curwin->w_cursor.lnum = eap->line2;

		// Don't leave the cursor on an illegal line or column, but do
		// accept zero as address, so 0;/PATTERN/ works correctly
		// (where zero usually means to use the first line).
		// Check the cursor position before returning.
		ଯଦି (eap->line2 > 0)
		    check_cursor();
		ଅନ୍ୟଥା
		    check_cursor_col();
		need_check_cursor = TRUE;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (*eap->cmd != ',')
	    ଭାଙ୍ଗ;
	++eap->cmd;
    }

    // One address given: set start and end lines.
    ଯଦି (eap->addr_ଗଣନା == 1)
    {
	eap->line1 = eap->line2;
	// ... but only implicit: really no address given
	ଯଦି (lnum == MAXLNUM)
	    eap->addr_ଗଣନା = 0;
    }
    ret = OK;

theend:
    ଯଦି (need_check_cursor)
	check_cursor();
    ଫେରନ୍ତୁ ret;
}

/*
 * Append "cmd" to the error message in IObuff.
 * Takes care of limiting the length and handling 0xa0, which would be
 * invisible otherwise.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
append_command(char_u *cmd)
{
    size_t  len = STRLEN(IObuff);
    char_u  *s = cmd;
    char_u  *d;

    ଯଦି (len > IOSIZE - 100)
    {
	// Not enough space, truncate and put in "...".
	d = IObuff + IOSIZE - 100;
	d -= mb_head_off(IObuff, d);
	STRCPY(d, "...");
    }
    STRCAT(IObuff, ": ");
    d = IObuff + STRLEN(IObuff);
    ଯେତେବେଳେ (*s != NUL && d - IObuff + 5 < IOSIZE)
    {
	ଯଦି (enc_utf8 ? (s[0] == 0xc2 && s[1] == 0xa0) : *s == 0xa0)
	{
	    s += enc_utf8 ? 2 : 1;
	    STRCPY(d, "<a0>");
	    d += 4;
	}
	ଅନ୍ୟଥା ଯଦି (d - IObuff + (*mb_ptr2len)(s) + 1 >= IOSIZE)
	    ଭାଙ୍ଗ;
	ଅନ୍ୟଥା
	    MB_COPY_CHAR(s, d);
    }
    *d = NUL;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * If "start" points "&opt", "&l:opt", "&g:opt" or "$ENV" return a pointer to
 * the name.  Otherwise just return "start".
 */
    char_u *
skip_option_env_lead(char_u *start)
{
    char_u *name = start;

    ଯଦି (*start == '&')
    {
	ଯଦି ((start[1] == 'l' || start[1] == 'g') && start[2] == ':')
	    name += 3;
	ଅନ୍ୟଥା
	    name += 1;
    }
    ଅନ୍ୟଥା ଯଦି (*start == '$')
	name += 1;
    ଫେରନ୍ତୁ name;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
one_letter_cmd(char_u *p, cmdidx_T *idx)
{
    ଯଦି (in_vim9script())
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (p[0] == 'k'
	    && (p[1] != 'e' || (p[1] == 'e' && p[2] != 'e')))
    {
	*idx = CMD_k;
	ଫେରନ୍ତୁ TRUE;
    }
    ଯଦି (p[0] == 's'
	    && ((p[1] == 'c' && (p[2] == NUL || (p[2] != 's' && p[2] != 'r'
			&& (p[3] == NUL || (p[3] != 'i' && p[4] != 'p')))))
		|| p[1] == 'g'
		|| (p[1] == 'i' && p[2] != 'm' && p[2] != 'l' && p[2] != 'g')
		|| p[1] == 'I'
		|| (p[1] == 'r' && p[2] != 'e')))
    {
	*idx = CMD_substitute;
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Return TRUE if "cmd" starts with "123->", a number followed by a method
 * call.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ସଂଖ୍ୟା_method(char_u *cmd)
{
    char_u *p = skipdigits(cmd);

    ଫେରନ୍ତୁ p > cmd && (p = skipwhite(p))[0] == '-' && p[1] == '>';
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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*full UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	(*lookup)(char_u *, size_t, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmd, cctx_T *) UNUSED,
	cctx_T	*cctx UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
#ifndef FEAT_EVAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9 = FALSE;
#ଅନ୍ୟଥା
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9 = in_vim9script();

    /*
     * Recognize a Vim9 script function/method call and assignment:
     * "lvar = value", "lvar(arg)", "[1, 2 3]->Func()"
     */
    p = eap->cmd;
    ଯଦି (lookup != ଖାଲି)
    {
	char_u *pskip = skip_option_env_lead(eap->cmd);

	ଯଦି (vim_strchr((char_u *)"{('[\"@&$", *p) != ଖାଲି
	       || ((p = to_name_const_end(pskip)) > eap->cmd && *p != NUL)
	       || (p[0] == '0' && p[1] == 'z'))
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    oplen;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    heredoc;
	    char_u  *swp;

	    ଯଦି (*eap->cmd == '&'
		    || (eap->cmd[0] == '$'
				  && eap->cmd[1] != '\'' && eap->cmd[1] != '"')
		    || (eap->cmd[0] == '@'
					&& (valid_yank_reg(eap->cmd[1], FALSE)
						       || eap->cmd[1] == '@')))
	    {
		ଯଦି (*eap->cmd == '&')
		{
		    p = eap->cmd + 1;
		    ଯଦି (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
			p += 2;
		    p = to_name_end(p, FALSE);
		}
		ଅନ୍ୟଥା ଯଦି (*eap->cmd == '$')
		    p = to_name_end(eap->cmd + 1, FALSE);
		ଅନ୍ୟଥା
		    p = eap->cmd + 2;
		ଯଦି (ends_excmd(*skipwhite(p)))
		{
		    // "&option <NL>", "$ENV <NL>" and "@r <NL>" are the start
		    // of an expression.
		    eap->cmdidx = CMD_eval;
		    ଫେରନ୍ତୁ eap->cmd;
		}
		// "&option" can be followed by "->" or "=", check below
	    }

	    ଯଦି (vim9 && *p == '<')
	    {
		// generic function type args
		ଯଦି (skip_generic_func_type_args(&p) == FAIL)
		{
		    eap->cmdidx = CMD_SIZE;
		    ଫେରନ୍ତୁ p;
		}
	    }

	    swp = skipwhite(p);

	    ଯଦି (
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
		ଯଦି (*eap->cmd == '{' && ends_excmd(*skipwhite(eap->cmd + 1)))
		{
		    // "{" by itself is the start of a block.
		    eap->cmdidx = CMD_block;
		    ଫେରନ୍ତୁ eap->cmd + 1;
		}
		eap->cmdidx = CMD_eval;
		ଫେରନ୍ତୁ eap->cmd;
	    }

	    ଯଦି ((p != eap->cmd && (
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
		ଯଦି (skip_ପଦସମଷ୍ଟି(&after, ଖାଲି) == OK)
		{
		    after = skipwhite(after);
		    ଯଦି (*after == '=' || (*after != NUL && after[1] == '=')
					 || (after[0] == '.' && after[1] == '.'
							   && after[2] == '='))
			eap->cmdidx = CMD_var;
		}
		--emsg_off;
		ଫେରନ୍ତୁ eap->cmd;
	    }

	    // "[...]->Method()" is a list expression, but "[a, b] = Func()" is
	    // an assignment.
	    // If there is no line break inside the "[...]" then "p" is
	    // advanced to after the "]" by to_name_const_end(): check if a "="
	    // follows.
	    // If "[...]" has a line break "p" still points at the "[" and it
	    // can't be an assignment.
	    ଯଦି (*eap->cmd == '[')
	    {
		char_u	    *eq;

		p = to_name_const_end(eap->cmd);
		ଯଦି (p == eap->cmd && *p == '[')
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = 0;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	semicolon = FALSE;

		    p = skip_var_list(eap->cmd, TRUE, &ଗଣନା_2, &semicolon, TRUE);
		}
		eq = p;
		ଯଦି (eq != ଖାଲି)
		{
		    eq = skipwhite(eq);
		    ଯଦି (vim_strchr((char_u *)"+-*/%.", *eq) != ଖାଲି)
		    {
			ଯଦି (eq[0] == '.' && eq[1] == '.')
			    ++eq;
			++eq;
		    }
		}
		ଯଦି (p == ଖାଲି || p == eap->cmd || *eq != '=')
		{
		    eap->cmdidx = CMD_eval;
		    ଫେରନ୍ତୁ eap->cmd;
		}
		ଯଦି (p > eap->cmd && *eq == '=')
		{
		    eap->cmdidx = CMD_var;
		    ଫେରନ୍ତୁ eap->cmd;
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
	    ଯଦି (oplen > 0)
	    {
		ଯଦି (((p - eap->cmd) > 2 && eap->cmd[1] == ':')
			|| *eap->cmd == '&'
			|| *eap->cmd == '$'
			|| *eap->cmd == '@'
			|| (eap->skip && IS_WHITE_OR_NUL(
						      *(skipwhite(p) + oplen)))
			|| lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK)
		{
		    eap->cmdidx = CMD_var;
		    ଫେରନ୍ତୁ eap->cmd;
		}
	    }

	    // Recognize trying to use a type for a w:, b:, t: or g: variable:
	    // "w:varname: number = 123".
	    ଯଦି (eap->cmd[1] == ':' && *p == ':')
	    {
		eap->cmdidx = CMD_var;
		ଫେରନ୍ତୁ eap->cmd;
	    }
	}

	// 1234->func() is a method call
	ଯଦି (ସଂଖ୍ୟା_method(eap->cmd))
	{
	    eap->cmdidx = CMD_eval;
	    ଫେରନ୍ତୁ eap->cmd;
	}

	// "g:", "s:" and "l:" are always assumed to be a variable, thus start
	// an expression.  A global/substitute/list command needs to use a
	// longer name.
	ଯଦି (vim_strchr((char_u *)"gsl", *p) != ଖାଲି && p[1] == ':')
	{
	    eap->cmdidx = CMD_eval;
	    ଫେରନ୍ତୁ eap->cmd;
	}

	// If it is an ID it might be a variable with an operator on the next
	// line, if the variable exists it can't be an Ex command.
	ଯଦି (p > eap->cmd && ends_excmd(*skipwhite(p))
		&& (lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK
		    || (ASCII_ISALPHA(eap->cmd[0]) && eap->cmd[1] == ':')))
	{
	    eap->cmdidx = CMD_eval;
	    ଫେରନ୍ତୁ eap->cmd;
	}

	// Check for "++nr" and "--nr".
	ଯଦି (p == eap->cmd && p[0] != NUL && p[0] == p[1]
						   && (*p == '+' || *p == '-'))
	{
	    eap->cmdidx = *p == '+' ? CMD_increment : CMD_decrement;
	    ଫେରନ୍ତୁ eap->cmd + 2;
	}
    }
#endif

    /*
     * Isolate the command and search for it in the command table.
     */
    p = eap->cmd;
    ଯଦି (one_letter_cmd(p, &eap->cmdidx))
    {
	++p;
	ଯଦି (full != ଖାଲି)
	    *full = TRUE;
    }
    ଅନ୍ୟଥା
    {
	ଯେତେବେଳେ (ASCII_ISALPHA(*p))
	    ++p;
	// for python 3.x support ":py3", ":python3", ":py3file", etc.
	ଯଦି (eap->cmd[0] == 'p' && eap->cmd[1] == 'y')
	{
	    ଯେତେବେଳେ (ASCII_ISALNUM(*p))
		++p;
	}
	ଅନ୍ୟଥା ଯଦି (*p == '9' && STRNCMP("vim9", eap->cmd, 4) == 0)
	{
	    // include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	    ++p;
	    ଯେତେବେଳେ (ASCII_ISALPHA(*p))
		++p;
	}

	// check for non-alpha command
	ଯଦି (p == eap->cmd && vim_strchr((char_u *)"@*!=><&~#}", *p) != ଖାଲି)
	    ++p;
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - eap->cmd);
	// The "d" command can directly be followed by 'l' or 'p' flag, when
	// not in Vim9 script.
	ଯଦି (!vim9 && *eap->cmd == 'd' && (p[-1] == 'l' || p[-1] == 'p'))
	{
	    // Check for ":dl", ":dell", etc. to ":deletel": that's
	    // :delete with the 'l' flag.  Same for 'p'.
	    ପାଇଁ (i = 0; i < len; ++i)
		ଯଦି (eap->cmd[i] != ((char_u *)"delete")[i])
		    ଭାଙ୍ଗ;
	    ଯଦି (i == len - 1)
	    {
		--len;
		ଯଦି (p[-1] == 'l')
		    eap->flags |= EXFLAG_LIST;
		ଅନ୍ୟଥା
		    eap->flags |= EXFLAG_PRINT;
	    }
	}

	ଯଦି (ASCII_ISLOWER(eap->cmd[0]))
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c1 = eap->cmd[0];
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c2 = len == 1 ? NUL : eap->cmd[1];

	    ଯଦି (command_ଗଣନା != (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)CMD_SIZE)
	    {
		iemsg(e_command_table_needs_to_be_updated_run_make_cmdidxs);
		getout(1);
	    }

	    // Use a precomputed index for fast look-up in cmdnames[]
	    // taking into account the first 2 letters of eap->cmd.
	    eap->cmdidx = cmdidxs1[Char_Ord_ତଳ(c1)];
	    ଯଦି (ASCII_ISLOWER(c2))
		eap->cmdidx += cmdidxs2[Char_Ord_ତଳ(c1)][Char_Ord_ତଳ(c2)];
	}
	ଅନ୍ୟଥା ଯଦି (ASCII_ISUPPER(eap->cmd[0]))
	    eap->cmdidx = CMD_Next;
	ଅନ୍ୟଥା
	    eap->cmdidx = CMD_bang;

	ପାଇଁ ( ; (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->cmdidx < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)CMD_SIZE;
			       eap->cmdidx = (cmdidx_T)((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->cmdidx + 1))
	    ଯଦି (STRNCMP(cmdnames[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->cmdidx].cmd_name, (ଅକ୍ଷର *)eap->cmd,
							    (size_t)len) == 0)
	    {
#ifdef FEAT_EVAL
		ଯଦି (full != ଖାଲି && cmdnames[eap->cmdidx].cmd_name[len] == NUL)
		    *full = TRUE;
#endif
		ଭାଙ୍ଗ;
	    }

	// :Print and :mode are not supported in Vim9 script.
	// Some commands cannot be shortened in Vim9 script.
	ଯଦି (vim9 && eap->cmdidx != CMD_SIZE)
	{
	    ଯଦି (eap->cmdidx == CMD_mode || eap->cmdidx == CMD_Print)
		eap->cmdidx = CMD_SIZE;
	    ଅନ୍ୟଥା ଯଦି ((cmdnames[eap->cmdidx].cmd_argt & EX_WHOLE)
			  && len < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cmdnames[eap->cmdidx].cmd_namelen)
	    {
		semsg(_(e_command_cannot_be_shortened_str), eap->cmd);
		eap->cmdidx = CMD_SIZE;
	    }
	}

	// Do not recognize ":*" as the star command unless '*' is in
	// 'cpoptions'.
	ଯଦି (eap->cmdidx == CMD_star && vim_strchr(p_cpo, CPO_STAR) == ଖାଲି)
	    p = eap->cmd;

	// Look for a user defined command as a last resort.  Let ":Print" be
	// overruled by a user defined command.
	ଯଦି ((eap->cmdidx == CMD_SIZE || eap->cmdidx == CMD_Print)
		&& *eap->cmd >= 'A' && *eap->cmd <= 'Z')
	{
	    // User defined commands may contain digits.
	    ଯେତେବେଳେ (ASCII_ISALNUM(*p))
		++p;
	    p = find_ucmd(eap, p, full, ଖାଲି, ଖାଲି);
	}
	ଯଦି (p == ଖାଲି || p == eap->cmd)
	    eap->cmdidx = CMD_SIZE;
    }

    // ":fina" means ":finally" in legacy script, for backwards compatibility.
    ଯଦି (eap->cmdidx == CMD_final && p - eap->cmd == 4 && !vim9)
	eap->cmdidx = CMD_finally;

     // Force ":ho" to be unresolved.  Without this, find_ex_command()
     // matches it to CMD_horizontal (the only "ho*" entry), which makes
     // fullcommand("ho") return "horizontal" even though ":ho" cannot be
     // used as the modifier (cmdmods[] requires 3 chars, "hor").
    ଯଦି (eap->cmdidx == CMD_horizontal && p - eap->cmd == 2)
	eap->cmdidx = CMD_SIZE;

#ifdef FEAT_EVAL
    ଯଦି (eap->cmdidx < CMD_SIZE
	    && vim9
	    && !IS_WHITE_NL_OR_NUL(*p) && *p != '!' && *p != '|'
	    && (eap->cmdidx < 0 ||
		(cmdnames[eap->cmdidx].cmd_argt & EX_NONWHITE_OK) == 0))
    {
	// A name that goes on with an underscore is not this command with an
	// argument: "ch_log" is not ":change".  Leave it to be reported as an
	// invalid command.
	ଯଦି (*p != '_')
	{
	    char_u *cmd = vim_strnsave(eap->cmd, p - eap->cmd);

	    semsg(_(e_command_str_not_followed_by_white_space_str), cmd,
								    eap->cmd);
	    vim_free(cmd);
	}
	eap->cmdidx = CMD_SIZE;
    }
#endif

    ଫେରନ୍ତୁ p;
}

#ଯଦି defined(FEAT_EVAL)
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ଅକ୍ଷର	*name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		minlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_ଗଣନା;  // :123verbose  :3tab
} cmdmod_info_T;

ସ୍ଥିର cmdmod_info_T cmdmod_info_tab[] = {
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
modifier_len(char_u *cmd)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j;
    char_u	*p = cmd;

    ଯଦି (VIM_ISDIGIT(*cmd))
	p = skipwhite(skipdigits(cmd + 1));
    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	// cmdmod_info_tab[] is sorted by name: once the first letter is past
	// the command's first letter no later entry can match.
	ଯଦି (cmdmod_info_tab[i].name[0] > *p)
	    ଭାଙ୍ଗ;
	ପାଇଁ (j = 0; p[j] != NUL; ++j)
	    ଯଦି (p[j] != cmdmod_info_tab[i].name[j])
		ଭାଙ୍ଗ;
	ଯଦି (!ASCII_ISALPHA(p[j]) && j >= cmdmod_info_tab[i].minlen
					&& (p == cmd || cmdmod_info_tab[i].has_ଗଣନା))
	    ଫେରନ୍ତୁ j + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - cmd);
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * Return > 0 if an Ex command "name" exists.
 * Return 2 if there is an exact match.
 * Return 3 if there is an ambiguous match.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmd_exists(char_u *name)
{
    exarg_T	ea;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		full = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;
    char_u	*p;

    // Check command modifiers.
    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	ପାଇଁ (j = 0; name[j] != NUL; ++j)
	    ଯଦି (name[j] != cmdmod_info_tab[i].name[j])
		ଭାଙ୍ଗ;
	ଯଦି (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	    ଫେରନ୍ତୁ (cmdmod_info_tab[i].name[j] == NUL ? 2 : 1);
    }

    // Check built-in commands and user defined commands.
    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    p = find_ex_command(&ea, &full, ଖାଲି, ଖାଲି);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ 3;
    ଯଦି (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	ଫେରନ୍ତୁ 0;
    ଯଦି (*skipwhite(p) != NUL)
	ଫେରନ୍ତୁ 0;	// trailing garbage
    ଫେରନ୍ତୁ (ea.cmdidx == CMD_SIZE ? 0 : (full ? 2 : 1));
}

/*
 * Add the attributes of an Ex command to "d" in the terms of ":command":
 * "argt" are the EX_ flags, "addr_type" the address type and "def" the
 * default for a range or count, -1 when there is none.
 */
    ଶୂନ୍ୟ
ex_command_attrs(dict_T *d, long_u argt, cmd_addr_T addr_type, long def)
{
    ସ୍ଥିର ଅକ୍ଷର *addr_names[] = {
	"lines", "windows", "arguments", "loaded_buffers", "buffers", "tabs",
	"tabs_relative", "quickfix_valid", "quickfix", "unsigned", "other",
	"none"
    };
    ଅକ୍ଷର	*nargs;

    ଯଦି (!(argt & EX_EXTRA))
	nargs = "0";
    ଅନ୍ୟଥା ଯଦି (!(argt & EX_NOSPC))
	nargs = (argt & EX_NEEDARG) ? "+" : "*";
    ଅନ୍ୟଥା ଯଦି (!(argt & EX_NEEDARG))
	nargs = "?";
    ଅନ୍ୟଥା
	nargs = (argt & EX_ARGSPACE) ? "_" : "1";
    dict_add_string(d, "nargs", (char_u *)nargs);

    ଯଦି (!(argt & EX_RANGE))
	dict_add_bool(d, "range", FALSE);
    ଅନ୍ୟଥା ଯଦି (argt & EX_DFLALL)
	dict_add_string(d, "range", (char_u *)"%");
    ଅନ୍ୟଥା ଯଦି (def >= 0 && !(argt & EX_ଗଣନା))
	dict_add_ସଂଖ୍ୟା(d, "range", def);
    ଅନ୍ୟଥା
	dict_add_string(d, "range", (char_u *)".");
    ଯଦି (!(argt & EX_ଗଣନା))
	dict_add_bool(d, "count", FALSE);
    ଅନ୍ୟଥା
	dict_add_ସଂଖ୍ୟା(d, "count", def < 0 ? 0 : def);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ex_command_info(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା vim9, dict_T *d)
{
    exarg_T	ea;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_cmod_flags = cmdmod.cmod_flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;

    ଯଦି (vim9 != -1)
    {
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9 ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    // An abbreviation of a command modifier, like ":bo" for ":botright".
    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	ପାଇଁ (j = 0; name[j] != NUL; ++j)
	    ଯଦି (name[j] != cmdmod_info_tab[i].name[j])
		ଭାଙ୍ଗ;
	ଯଦି (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	{
	    name = (char_u *)cmdmod_info_tab[i].name;
	    ଭାଙ୍ଗ;
	}
    }

    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    ea.addr_ଗଣନା = 0;
    // Don't complain about using "en" or ":let" in Vim9 script.
    ++emsg_silent;
    p = find_ex_command(&ea, ଖାଲି, ଖାଲି, ଖାଲି);
    ଯଦି (p == ଖାଲି || ea.cmdidx == CMD_SIZE
	    || (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	    || *skipwhite(p) != NUL
	    || not_in_vim9(&ea, true) == FAIL)
	ret = FAIL;
    --emsg_silent;

    ଯଦି (ret == FAIL)
	;
    ଅନ୍ୟଥା ଯଦି (IS_USER_CMDIDX(ea.cmdidx))
	ret = user_command_info(ea.useridx, ea.cmdidx, d);
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	notimpl = cmdnames[ea.cmdidx].cmd_func == ex_ni
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
    ଫେରନ୍ତୁ ret;
}

/*
 * "fullcommand" function
 */
    ଶୂନ୍ୟ
f_fullcommand(typval_T *argvars, typval_T *rettv)
{
    exarg_T	ea;
    char_u	*name;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_cmod_flags = cmdmod.cmod_flags;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    name = tv_get_string(&argvars[0]);
    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	vim9script = tv_get_bool(&argvars[1]);
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9script ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    ଯେତେବେଳେ (*name == ':')
	name++;
    name = skip_range(name, TRUE, ଖାଲି);

    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.addr_ଗଣନା = 0;
    ++emsg_silent;  // don't complain about using "en" in Vim9 script
    p = find_ex_command(&ea, ଖାଲି, ଖାଲି, ଖାଲି);
    --emsg_silent;
    ଯଦି (p == ଖାଲି || ea.cmdidx == CMD_SIZE)
	ଯାଅ theend;

    ଯଦି (vim9script && not_in_vim9(&ea, true) == FAIL)
	ଯାଅ theend;

    rettv->vval.v_string = vim_strsave(IS_USER_CMDIDX(ea.cmdidx)
				 ? get_user_command_name(ea.useridx, ea.cmdidx)
				 : cmdnames[ea.cmdidx].cmd_name);
theend:
    cmdmod.cmod_flags = save_cmod_flags;
}
#endif

    cmdidx_T
excmd_get_cmdidx(char_u *cmd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    cmdidx_T idx;

    ଯଦି (!one_letter_cmd(cmd, &idx))
	ପାଇଁ (idx = (cmdidx_T)0; (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)idx < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)CMD_SIZE;
		idx = (cmdidx_T)((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)idx + 1))
	    ଯଦି (STRNCMP(cmdnames[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)idx].cmd_name, cmd, (size_t)len) == 0)
		ଭାଙ୍ଗ;

    ଫେରନ୍ତୁ idx;
}

    long
excmd_get_argt(cmdidx_T idx)
{
    ଫେରନ୍ତୁ (long)cmdnames[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)idx].cmd_argt;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip_star,	// skip "*" used for Visual range
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ctx)		// pointer to xp_context or NULL
{
    char_u	*cmd = cmd_start;
    unsigned	delim;

    ଯେତେବେଳେ (vim_strchr((char_u *)" \t0123456789.$%'/?-+,;\\", *cmd) != ଖାଲି)
    {
	ଯଦି (*cmd == '\\')
	{
	    ଯଦି (cmd[1] == '?' || cmd[1] == '/' || cmd[1] == '&')
		++cmd;
	    ଅନ୍ୟଥା
		ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (*cmd == '\'')
	{
	    char_u *p = cmd;

	    // a quote is only valid at the start or after a separator
	    ଯେତେବେଳେ (p > cmd_start)
	    {
		--p;
		ଯଦି (!VIM_ISWHITE(*p))
		    ଭାଙ୍ଗ;
	    }
	    ଯଦି (cmd > cmd_start && !VIM_ISWHITE(*p) && *p != ',' && *p != ';')
		ଭାଙ୍ଗ;
	    ଯଦି (*++cmd == NUL && ctx != ଖାଲି)
		*ctx = EXPAND_NOTHING;
	}
	ଅନ୍ୟଥା ଯଦି (*cmd == '/' || *cmd == '?')
	{
	    delim = *cmd++;
	    ଯେତେବେଳେ (*cmd != NUL && *cmd != delim)
		ଯଦି (*cmd++ == '\\' && *cmd != NUL)
		    ++cmd;
	    ଯଦି (*cmd == NUL && ctx != ଖାଲି)
		*ctx = EXPAND_NOTHING;
	}
	ଯଦି (*cmd != NUL)
	    ++cmd;
    }

    // Skip ":" and white space.
    ଯେତେବେଳେ (*cmd == ':')
	cmd = skipwhite(cmd + 1);

    // Skip "*" used for Visual range.
    ଯଦି (skip_star && *cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == ଖାଲି)
	cmd = skipwhite(cmd + 1);

    ଫେରନ୍ତୁ cmd;
}

    ସ୍ଥିର ଶୂନ୍ୟ
addr_error(cmd_addr_T addr_type)
{
    ଯଦି (addr_type == ADDR_NONE)
	emsg(_(e_no_range_allowed));
    ଅନ୍ୟଥା
	emsg(_(e_invalid_range));
}

/*
 * Return the default address for an address type.
 */
    ସ୍ଥିର linenr_T
default_address(exarg_T *eap)
{
    linenr_T lnum = 0;

    ବାଛ (eap->addr_type)
    {
	ସ୍ଥିତି ADDR_ଧାଡ଼ିଗୁଡ଼ିକ:
	ସ୍ଥିତି ADDR_OTHER:
	    // Default is the cursor line number.  Avoid using an invalid
	    // line number though.
	    ଯଦି (curwin->w_cursor.lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	    ଅନ୍ୟଥା
		lnum = curwin->w_cursor.lnum;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ADDR_WINDOWS:
	    lnum = CURRENT_WIN_NR;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ADDR_ARGUMENTS:
	    lnum = curwin->w_arg_idx + 1;
	    ଯଦି (lnum > ARGCOUNT)
		lnum = ARGCOUNT;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ADDR_LOADED_BUFFERS:
	ସ୍ଥିତି ADDR_BUFFERS:
	    lnum = curbuf->b_fnum;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ADDR_TABS:
	    lnum = CURRENT_TAB_NR;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ADDR_TABS_RELATIVE:
	ସ୍ଥିତି ADDR_UNSIGNED:
	    lnum = 1;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_idx(eap);
#endif
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_valid_idx(eap);
#endif
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ADDR_NONE:
	    // Will give an error later if a range is found.
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ lnum;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip,		// only skip the address, don't use it
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		silent,		// no errors or side effects
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		to_other_file,  // flag: may jump to other file
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		address_ଗଣନା UNUSED) // 1 for first address, >1 after comma
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    long	n;
    char_u	*cmd;
    pos_T	pos;
    pos_T	*fp;
    linenr_T	lnum;
    buf_T	*buf;

    cmd = skipwhite(*ptr);
    lnum = MAXLNUM;
    କର
    {
	ବାଛ (*cmd)
	{
	    ସ୍ଥିତି '.':			    // '.' - Cursor position
		++cmd;
		ବାଛ (addr_type)
		{
		    ସ୍ଥିତି ADDR_ଧାଡ଼ିଗୁଡ଼ିକ:
		    ସ୍ଥିତି ADDR_OTHER:
			lnum = curwin->w_cursor.lnum;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_LOADED_BUFFERS:
		    ସ୍ଥିତି ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_NONE:
		    ସ୍ଥିତି ADDR_TABS_RELATIVE:
		    ସ୍ଥିତି ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = ଖାଲି;
			ଯାଅ error;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			ଭାଙ୍ଗ;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି '$':			    // '$' - last line
		++cmd;
		ବାଛ (addr_type)
		{
		    ସ୍ଥିତି ADDR_ଧାଡ଼ିଗୁଡ଼ିକ:
		    ସ୍ଥିତି ADDR_OTHER:
			lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_WINDOWS:
			lnum = LAST_WIN_NR;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_ARGUMENTS:
			lnum = ARGCOUNT;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_LOADED_BUFFERS:
			buf = lastbuf;
			ଯେତେବେଳେ (buf->b_ml.ml_mfp == ଖାଲି)
			{
			    ଯଦି (buf->b_prev == ଖାଲି)
				ଭାଙ୍ଗ;
			    buf = buf->b_prev;
			}
			lnum = buf->b_fnum;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_BUFFERS:
			lnum = lastbuf->b_fnum;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_TABS:
			lnum = LAST_TAB_NR;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_NONE:
		    ସ୍ଥିତି ADDR_TABS_RELATIVE:
		    ସ୍ଥିତି ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = ଖାଲି;
			ଯାଅ error;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_size(eap);
			ଯଦି (lnum == 0)
			    lnum = 1;
#endif
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_valid_size(eap);
			ଯଦି (lnum == 0)
			    lnum = 1;
#endif
			ଭାଙ୍ଗ;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି '\'':			    // ''' - mark
		ଯଦି (*++cmd == NUL)
		{
		    cmd = ଖାଲି;
		    ଯାଅ error;
		}
		ଯଦି (addr_type != ADDR_ଧାଡ଼ିଗୁଡ଼ିକ)
		{
		    addr_error(addr_type);
		    cmd = ଖାଲି;
		    ଯାଅ error;
		}
		ଯଦି (skip)
		    ++cmd;
		ଅନ୍ୟଥା
		{
		    // Only accept a mark in another file when it is
		    // used by itself: ":'M".
		    fp = getmark(*cmd, to_other_file && cmd[1] == NUL);
		    ++cmd;
		    ଯଦି (fp == (pos_T *)-1)
			// Jumped to another file.
			lnum = curwin->w_cursor.lnum;
		    ଅନ୍ୟଥା
		    {
			ଯଦି (check_mark(fp) == FAIL)
			{
			    cmd = ଖାଲି;
			    ଯାଅ error;
			}
			lnum = fp->lnum;
		    }
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି '/':
	    ସ୍ଥିତି '?':			// '/' or '?' - search
		c = *cmd++;
		ଯଦି (addr_type != ADDR_ଧାଡ଼ିଗୁଡ଼ିକ)
		{
		    addr_error(addr_type);
		    cmd = ଖାଲି;
		    ଯାଅ error;
		}
		ଯଦି (skip)	// skip "/pat/"
		{
		    cmd = skip_regexp(cmd, c, magic_isset());
		    ଯଦି (*cmd == c)
			++cmd;
		}
		ଅନ୍ୟଥା
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags;

		    pos = curwin->w_cursor; // save curwin->w_cursor

		    // When '/' or '?' follows another address, start from
		    // there.
		    ଯଦି (lnum > 0 && lnum != MAXLNUM)
			curwin->w_cursor.lnum =
				lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
					   ? curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା : lnum;

		    // Start a forward search at the end of the line (unless
		    // before the first line).
		    // Start a backward search at the start of the line.
		    // This makes sure we never match in the current
		    // line, and can match anywhere in the
		    // next/previous line.
		    ଯଦି (c == '/' && curwin->w_cursor.lnum > 0)
			curwin->w_cursor.col = MAXCOL;
		    ଅନ୍ୟଥା
			curwin->w_cursor.col = 0;
		    searchcmdlen = 0;
		    flags = silent ? ସନ୍ଧାନ_KEEP : ସନ୍ଧାନ_HIS | ସନ୍ଧାନ_ସନ୍ଦେଶ;
		    ଯଦି (!do_ସନ୍ଧାନ(ଖାଲି, c, c, cmd, STRLEN(cmd), 1L, flags, ଖାଲି))
		    {
			curwin->w_cursor = pos;
			cmd = ଖାଲି;
			ଯାଅ error;
		    }
		    lnum = curwin->w_cursor.lnum;
		    curwin->w_cursor = pos;
		    // adjust command string pointer
		    cmd += searchcmdlen;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି '\\':		    // "\?", "\/" or "\&", repeat search
		++cmd;
		ଯଦି (addr_type != ADDR_ଧାଡ଼ିଗୁଡ଼ିକ)
		{
		    addr_error(addr_type);
		    cmd = ଖାଲି;
		    ଯାଅ error;
		}
		ଯଦି (*cmd == '&')
		    i = RE_SUBST;
		ଅନ୍ୟଥା ଯଦି (*cmd == '?' || *cmd == '/')
		    i = RE_ସନ୍ଧାନ;
		ଅନ୍ୟଥା
		{
		    emsg(_(e_backslash_should_be_followed_by));
		    cmd = ଖାଲି;
		    ଯାଅ error;
		}

		ଯଦି (!skip)
		{
		    /*
		     * When search follows another address, start from
		     * there.
		     */
		    ଯଦି (lnum != MAXLNUM)
			pos.lnum = lnum;
		    ଅନ୍ୟଥା
			pos.lnum = curwin->w_cursor.lnum;

		    /*
		     * Start the search just like for the above
		     * do_search().
		     */
		    ଯଦି (*cmd != '?')
			pos.col = MAXCOL;
		    ଅନ୍ୟଥା
			pos.col = 0;
		    pos.coladd = 0;
		    ଯଦି (searchit(curwin, curbuf, &pos, ଖାଲି,
				*cmd == '?' ? BACKWARD : FORWARD,
				(char_u *)"", 0, 1L, ସନ୍ଧାନ_ସନ୍ଦେଶ, i, ଖାଲି) != FAIL)
			lnum = pos.lnum;
		    ଅନ୍ୟଥା
		    {
			cmd = ଖାଲି;
			ଯାଅ error;
		    }
		}
		++cmd;
		ଭାଙ୍ଗ;

	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଯଦି (VIM_ISDIGIT(*cmd))	// absolute line number
		    lnum = getdigits(&cmd);
	}

	ପାଇଁ (;;)
	{
	    cmd = skipwhite(cmd);
	    ଯଦି (*cmd != '-' && *cmd != '+' && !VIM_ISDIGIT(*cmd))
		ଭାଙ୍ଗ;

	    ଯଦି (lnum == MAXLNUM)
	    {
		ବାଛ (addr_type)
		{
		    ସ୍ଥିତି ADDR_ଧାଡ଼ିଗୁଡ଼ିକ:
		    ସ୍ଥିତି ADDR_OTHER:
			// "+1" is same as ".+1"
			lnum = curwin->w_cursor.lnum;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_LOADED_BUFFERS:
		    ସ୍ଥିତି ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_TABS_RELATIVE:
			lnum = 1;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ADDR_NONE:
		    ସ୍ଥିତି ADDR_UNSIGNED:
			lnum = 0;
			ଭାଙ୍ଗ;
		}
	    }

	    ଯଦି (VIM_ISDIGIT(*cmd))
		i = '+';		// "number" is same as "+number"
	    ଅନ୍ୟଥା
		i = *cmd++;
	    ଯଦି (!VIM_ISDIGIT(*cmd))	// '+' is '+1'
		n = 1;
	    ଅନ୍ୟଥା
	    {
		// "number", "+number" or "-number"
		n = getdigits(&cmd);
		ଯଦି (n == MAXLNUM)
		{
		    emsg(_(e_ଧାଡ଼ି_ସଂଖ୍ୟା_out_of_range));
		    cmd = ଖାଲି;
		    ଯାଅ error;
		}
	    }

	    ଯଦି (addr_type == ADDR_TABS_RELATIVE)
	    {
		emsg(_(e_invalid_range));
		cmd = ଖାଲି;
		ଯାଅ error;
	    }
	    ଅନ୍ୟଥା ଯଦି (addr_type == ADDR_LOADED_BUFFERS
		    || addr_type == ADDR_BUFFERS)
		lnum = compute_buffer_local_ଗଣନା(
				    addr_type, lnum, (i == '-') ? -1 * n : n);
	    ଅନ୍ୟଥା
	    {
#ifdef FEAT_FOLDING
		// Relative line addressing: need to adjust for lines in a
		// closed fold after the first address.
		ଯଦି (addr_type == ADDR_ଧାଡ଼ିଗୁଡ଼ିକ && (i == '-' || i == '+')
							 && address_ଗଣନା >= 2)
		    (ଶୂନ୍ୟ)hasFolding(lnum, ଖାଲି, &lnum);
#endif
		ଯଦି (i == '-')
		    lnum -= n;
		ଅନ୍ୟଥା
		{
		    ଯଦି (lnum >= 0 && n >= LONG_MAX - lnum)
		    {
			emsg(_(e_ଧାଡ଼ି_ସଂଖ୍ୟା_out_of_range));
			cmd = ଖାଲି;
			ଯାଅ error;
		    }
		    lnum += n;
		}
	    }
	}
    } ଯେତେବେଳେ (*cmd == '/' || *cmd == '?');

error:
    *ptr = cmd;
    ଫେରନ୍ତୁ lnum;
}

/*
 * Set eap->line1 and eap->line2 to the whole range.
 * Used for commands with the EX_DFLALL flag and no range given.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
address_default_all(exarg_T *eap)
{
    eap->line1 = 1;
    ବାଛ (eap->addr_type)
    {
	ସ୍ଥିତି ADDR_ଧାଡ଼ିଗୁଡ଼ିକ:
	ସ୍ଥିତି ADDR_OTHER:
	    eap->line2 = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ADDR_LOADED_BUFFERS:
	    {
		buf_T *buf = firstbuf;

		ଯେତେବେଳେ (buf->b_next != ଖାଲି && buf->b_ml.ml_mfp == ଖାଲି)
		    buf = buf->b_next;
		eap->line1 = buf->b_fnum;
		buf = lastbuf;
		ଯେତେବେଳେ (buf->b_prev != ଖାଲି && buf->b_ml.ml_mfp == ଖାଲି)
		    buf = buf->b_prev;
		eap->line2 = buf->b_fnum;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ADDR_BUFFERS:
	    eap->line1 = firstbuf->b_fnum;
	    eap->line2 = lastbuf->b_fnum;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ADDR_WINDOWS:
	    eap->line2 = LAST_WIN_NR;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ADDR_TABS:
	    eap->line2 = LAST_TAB_NR;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ADDR_TABS_RELATIVE:
	    eap->line2 = 1;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ADDR_ARGUMENTS:
	    ଯଦି (ARGCOUNT == 0)
		eap->line1 = eap->line2 = 0;
	    ଅନ୍ୟଥା
		eap->line2 = ARGCOUNT;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    eap->line2 = qf_get_valid_size(eap);
	    ଯଦି (eap->line2 == 0)
		eap->line2 = 1;
#endif
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ADDR_NONE:
	ସ୍ଥିତି ADDR_UNSIGNED:
	ସ୍ଥିତି ADDR_QUICKFIX:
	    iemsg("Cannot use EX_DFLALL with ADDR_NONE, ADDR_UNSIGNED or ADDR_QUICKFIX");
	    ଭାଙ୍ଗ;
    }
}


/*
 * Get flags from an Ex command argument.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_flags(exarg_T *eap)
{
    ଯେତେବେଳେ (vim_strchr((char_u *)"lp#", *eap->arg) != ଖାଲି)
    {
	ଯଦି (*eap->arg == 'l')
	    eap->flags |= EXFLAG_LIST;
	ଅନ୍ୟଥା ଯଦି (*eap->arg == 'p')
	    eap->flags |= EXFLAG_PRINT;
	ଅନ୍ୟଥା
	    eap->flags |= EXFLAG_NR;
	eap->arg = skipwhite(eap->arg + 1);
    }
}

/*
 * Function called for command which is Not Implemented.  NI!
 */
    ଶୂନ୍ୟ
ex_ni(exarg_T *eap)
{
    ଯଦି (!eap->skip)
	eap->errmsg =
		_(e_sorry_command_is_not_available_in_this_version);
}

#ifdef HAVE_EX_SCRIPT_NI
/*
 * Function called for script command which is Not Implemented.  NI!
 * Skips over ":perl <<EOF" constructs.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_script_ni(exarg_T *eap)
{
    ଯଦି (!eap->skip)
	ex_ni(eap);
    ଅନ୍ୟଥା
	vim_free(script_get(eap, eap->arg));
}
#endif

/*
 * Check range in Ex command for validity.
 * Return NULL when valid, error message when invalid.
 */
    ସ୍ଥିର ଅକ୍ଷର *
invalid_range(exarg_T *eap)
{
    buf_T	*buf;

    ଯଦି (       eap->line1 < 0
	    || eap->line2 < 0
	    || eap->line1 > eap->line2)
	ଫେରନ୍ତୁ _(e_invalid_range);

    ଯଦି (eap->argt & EX_RANGE)
    {
	ବାଛ (eap->addr_type)
	{
	    ସ୍ଥିତି ADDR_ଧାଡ଼ିଗୁଡ଼ିକ:
		ଯଦି (eap->line2 > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
#ifdef FEAT_DIFF
			    + (eap->cmdidx == CMD_diffget ||
				eap->cmdidx == CMD_diffput)
#endif
		   )
		    ଫେରନ୍ତୁ _(e_invalid_range);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ADDR_ARGUMENTS:
		// add 1 if ARGCOUNT is 0
		ଯଦି (eap->line2 > ARGCOUNT + (!ARGCOUNT))
		    ଫେରନ୍ତୁ _(e_invalid_range);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ADDR_BUFFERS:
		// Only a boundary check, not whether the buffers actually
		// exist.
		ଯଦି (eap->line1 < 1 || eap->line2 > get_highest_fnum())
		    ଫେରନ୍ତୁ _(e_invalid_range);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ADDR_LOADED_BUFFERS:
		buf = firstbuf;
		ଯେତେବେଳେ (buf->b_ml.ml_mfp == ଖାଲି)
		{
		    ଯଦି (buf->b_next == ଖାଲି)
			ଫେରନ୍ତୁ _(e_invalid_range);
		    buf = buf->b_next;
		}
		ଯଦି (eap->line1 < buf->b_fnum)
		    ଫେରନ୍ତୁ _(e_invalid_range);
		buf = lastbuf;
		ଯେତେବେଳେ (buf->b_ml.ml_mfp == ଖାଲି)
		{
		    ଯଦି (buf->b_prev == ଖାଲି)
			ଫେରନ୍ତୁ _(e_invalid_range);
		    buf = buf->b_prev;
		}
		ଯଦି (eap->line2 > buf->b_fnum)
		    ଫେରନ୍ତୁ _(e_invalid_range);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ADDR_WINDOWS:
		ଯଦି (eap->line2 > LAST_WIN_NR)
		    ଫେରନ୍ତୁ _(e_invalid_range);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ADDR_TABS:
		ଯଦି (eap->line2 > LAST_TAB_NR)
		    ଫେରନ୍ତୁ _(e_invalid_range);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ADDR_TABS_RELATIVE:
	    ସ୍ଥିତି ADDR_OTHER:
		// Any range is OK.
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
		// No error for value that is too big, will use the last entry.
		ଯଦି (eap->line2 <= 0)
		{
		    ଯଦି (eap->addr_ଗଣନା == 0)
			ଫେରନ୍ତୁ _(e_no_errors);
		    ଫେରନ୍ତୁ _(e_invalid_range);
		}
#endif
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
		ଯଦି ((eap->line2 != 1 && eap->line2 > qf_get_valid_size(eap))
			|| eap->line2 < 0)
		    ଫେରନ୍ତୁ _(e_invalid_range);
#endif
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ADDR_UNSIGNED:
	    ସ୍ଥିତି ADDR_NONE:
		// Will give an error elsewhere.
		ଭାଙ୍ଗ;
	}
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Correct the range for zero line number, if required.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
correct_range(exarg_T *eap)
{
    ଯଦି (!(eap->argt & EX_ZEROR))	    // zero in range not allowed
    {
	ଯଦି (eap->line1 == 0)
	    eap->line1 = 1;
	ଯଦି (eap->line2 == 0)
	    eap->line2 = 1;
    }
}

#ifdef FEAT_QUICKFIX
/*
 * For a ":vimgrep" or ":vimgrepadd" command return a pointer past the
 * pattern.  Otherwise return eap->arg.
 */
    ସ୍ଥିର char_u *
skip_grep_pat(exarg_T *eap)
{
    char_u	*p = eap->arg;

    ଯଦି (*p != NUL && (eap->cmdidx == CMD_vimgrep || eap->cmdidx == CMD_lvimgrep
		|| eap->cmdidx == CMD_vimgrepadd
		|| eap->cmdidx == CMD_lvimgrepadd
		|| grep_internal(eap->cmdidx)))
    {
	p = skip_vimgrep_pat(p, ଖାଲି, ଖାଲି);
	ଯଦି (p == ଖାଲି)
	    p = eap->arg;
    }
    ଫେରନ୍ତୁ p;
}

/*
 * For the ":make" and ":grep" commands insert the 'makeprg'/'grepprg' option
 * in the command line, so that things like % get expanded.
 */
    ସ୍ଥିର char_u *
replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep)
{
    char_u	*new_cmdline;
    char_u	*program;
    char_u	*pos;
    char_u	*ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    /*
     * Don't do it when ":vimgrep" is used for ":grep".
     */
    ଯଦି ((eap->cmdidx == CMD_make || eap->cmdidx == CMD_lmake
		     || eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
		     || eap->cmdidx == CMD_grepadd
		     || eap->cmdidx == CMD_lgrepadd)
	    && !grep_internal(eap->cmdidx))
    {
	ଯଦି (eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
	    || eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	{
	    ଯଦି (*curbuf->b_p_gp == NUL)
		program = p_gp;
	    ଅନ୍ୟଥା
		program = curbuf->b_p_gp;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (*curbuf->b_p_mp == NUL)
		program = p_mp;
	    ଅନ୍ୟଥା
		program = curbuf->b_p_mp;
	}

	p = skipwhite(p);

	ଯଦି ((pos = (char_u *)strstr((ଅକ୍ଷର *)program, "$*")) != ଖାଲି)
	{
	    // replace $* by given arguments
	    i = 1;
	    ଯେତେବେଳେ ((pos = (char_u *)strstr((ଅକ୍ଷର *)pos + 2, "$*")) != ଖାଲି)
		++i;
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);
	    new_cmdline = alloc(STRLEN(program) + (size_t)i * (len - 2) + 1);
	    ଯଦି (new_cmdline == ଖାଲି)
		ଫେରନ୍ତୁ ଖାଲି;			// out of memory
	    ptr = new_cmdline;
	    ଯେତେବେଳେ ((pos = (char_u *)strstr((ଅକ୍ଷର *)program, "$*")) != ଖାଲି)
	    {
		i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(pos - program);
		STRNCPY(ptr, program, i);
		STRCPY(ptr += i, p);
		ptr += len;
		program = pos + 2;
	    }
	    STRCPY(ptr, program);
	}
	ଅନ୍ୟଥା
	{
	    size_t program_len = STRLEN(program);

	    new_cmdline = alloc(program_len + STRLEN(p) + 2);
	    ଯଦି (new_cmdline == ଖାଲି)
		ଫେରନ୍ତୁ ଖାଲି;			// out of memory
	    STRCPY(new_cmdline, program);
	    STRCPY(new_cmdline + program_len, " ");
	    STRCPY(new_cmdline + program_len + 1, p);
	}
	ସନ୍ଦେଶ_make(p);

	// 'eap->cmd' is not set here, because it is not used at CMD_make
	vim_free(*cmdlinep);
	*cmdlinep = new_cmdline;
	p = new_cmdline;
    }
    ଫେରନ୍ତୁ p;
}
#endif

/*
 * Expand file name in Ex command argument.
 * When an error is detected, "errormsgp" is set to a non-NULL pointer.
 * Return FAIL for failure, OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_filename(
    exarg_T	*eap,
    char_u	**cmdlinep,
    ଅକ୍ଷର	**errormsgp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_wildcards;	// need to expand wildcards
    char_u	*repl;
    size_t	srclen;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		escaped;

#ifdef FEAT_QUICKFIX
    // Skip a regexp pattern for ":vimgrep[add] pat file..."
    p = skip_grep_pat(eap);
#ଅନ୍ୟଥା
    p = eap->arg;
#endif

    /*
     * Decide to expand wildcards *before* replacing '%', '#', etc.  If
     * the file name contains a wildcard it should not cause expanding.
     * (it will be expanded anyway if there is a wildcard before replacing).
     */
    has_wildcards = mch_has_wildcard(p);
    ଯେତେବେଳେ (*p != NUL)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`, wildcards in it are not expanded.
	ଯଦି (p[0] == '`' && p[1] == '=')
	{
	    p += 2;
	    (ଶୂନ୍ୟ)skip_ପଦସମଷ୍ଟି(&p, ଖାଲି);
	    ଯଦି (*p == '`')
		++p;
	    ଜାରି;
	}
#endif
	/*
	 * Quick check if this cannot be the start of a special string.
	 * Also removes backslash before '%', '#' and '<'.
	 */
	ଯଦି (vim_strchr((char_u *)"%#<", *p) == ଖାଲି)
	{
	    ++p;
	    ଜାରି;
	}

	/*
	 * Try to find a match at this position.
	 */
	repl = eval_vars(p, eap->arg, &srclen, &(eap->do_ecmd_lnum),
						    errormsgp, &escaped, TRUE);
	ଯଦି (*errormsgp != ଖାଲି)		// error detected
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (repl == ଖାଲି)		// no match found
	{
	    p += srclen;
	    ଜାରି;
	}

	// Wildcards won't be expanded below, the replacement is taken
	// literally.  But do expand "~/file", "~user/file" and "$HOME/file".
	ଯଦି (vim_strchr(repl, '$') != ଖାଲି || vim_strchr(repl, '~') != ଖାଲି)
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
	ଯଦି (!eap->usefilter
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
	    ସ୍ଥିର char_u *nobslash = (char_u *)" \t\"|";
# define ESCAPE_CHARS nobslash
#ଅନ୍ୟଥା
# define ESCAPE_CHARS escape_chars
#endif

	    ପାଇଁ (l = repl; *l; ++l)
		ଯଦି (vim_strchr(ESCAPE_CHARS, *l) != ଖାଲି)
		{
		    l = vim_strsave_escaped(repl, ESCAPE_CHARS);
		    ଯଦି (l != ଖାଲି)
		    {
			vim_free(repl);
			repl = l;
		    }
		    ଭାଙ୍ଗ;
		}
	}

	// For a shell command a '!' must be escaped.
	ଯଦି ((eap->usefilter || eap->cmdidx == CMD_bang
						|| eap->cmdidx == CMD_terminal)
			    && vim_strpbrk(repl, (char_u *)"!") != ଖାଲି)
	{
	    char_u	*l;

	    l = vim_strsave_escaped(repl, (char_u *)"!");
	    ଯଦି (l != ଖାଲି)
	    {
		vim_free(repl);
		repl = l;
	    }
	}

	p = repl_cmdline(eap, p, srclen, repl, cmdlinep);
	vim_free(repl);
	ଯଦି (p == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
    }

    /*
     * One file argument: Expand wildcards.
     * Don't do this with ":r !command" or ":w !command".
     */
    ଯଦି ((eap->argt & EX_NOSPC) && !eap->usefilter)
    {
	/*
	 * May do this twice:
	 * 1. Replace environment variables.
	 * 2. Replace any other wildcards, remove backslashes.
	 */
	ପାଇଁ (n = 1; n <= 2; ++n)
	{
	    ଯଦି (n == 2)
	    {
		/*
		 * Halve the number of backslashes (this is Vi compatible).
		 * For Unix and OS/2, when wildcards are expanded, this is
		 * done by ExpandOne() below.
		 */
#ଯଦି defined(UNIX)
		ଯଦି (!has_wildcards)
#endif
		    backslash_halve(eap->arg);
	    }

	    ଯଦି (has_wildcards)
	    {
		ଯଦି (n == 1)
		{
		    /*
		     * First loop: May expand environment variables.  This
		     * can be done much faster with expand_env() than with
		     * something else (e.g., calling a shell).
		     * After expanding environment variables, check again
		     * if there are still wildcards present.
		     */
		    ଯଦି (vim_strchr(eap->arg, '$') != ଖାଲି
			    || vim_strchr(eap->arg, '~') != ଖାଲି)
		    {
			expand_env_esc(eap->arg, NameBuff, MAXPATHL,
					(char_u *)(" \t" PATH_ESC_WILDCARDS),
					TRUE, ଖାଲି);
			has_wildcards = mch_has_wildcard(NameBuff);
			p = NameBuff;
		    }
		    ଅନ୍ୟଥା
			p = ଖାଲି;
		}
		ଅନ୍ୟଥା // n == 2
		{
		    expand_T	xpc;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options = WILD_LIST_NOTFOUND
					       | WILD_NOERROR | WILD_ADD_SLASH;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    ଯଦି (p_wic)
			options += WILD_ICASE;
		    p = ExpandOne(&xpc, eap->arg, ଖାଲି,
						   options, WILD_EXPAND_FREE);
		    ଯଦି (p == ଖାଲି)
			ଫେରନ୍ତୁ FAIL;
		}
		ଯଦି (p != ଖାଲି)
		{
		    (ଶୂନ୍ୟ)repl_cmdline(eap, eap->arg, STRLEN(eap->arg),
								 p, cmdlinep);
		    ଯଦି (n == 2)	// p came from ExpandOne()
			vim_free(p);
		}
	    }
	}
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Replace part of the command line, keeping eap->cmd, eap->arg and
 * eap->nextcmd correct.
 * "src" points to the part that is to be replaced, of length "srclen".
 * "repl" is the replacement string.
 * Returns a pointer to the character after the replaced string.
 * Returns NULL for failure.
 */
    ସ୍ଥିର char_u *
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
    ଯଦି (eap->nextcmd != ଖାଲି)
	i += STRLEN(eap->nextcmd);	// add space for next command
    ଯଦି ((new_cmdline = alloc(i)) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;			// out of memory!

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

    ଯଦି (eap->nextcmd != ଖାଲି)		// append next command
    {
	new_cmdlinelen += taillen + 1;
	STRCPY(new_cmdline + new_cmdlinelen, eap->nextcmd);
	eap->nextcmd = new_cmdline + new_cmdlinelen;
    }
    eap->cmd = new_cmdline + (eap->cmd - *cmdlinep);
    eap->arg = new_cmdline + (eap->arg - *cmdlinep);
    ଯଦି (eap->do_ecmd_cmd != ଖାଲି && eap->do_ecmd_cmd != dollar_command)
	eap->do_ecmd_cmd = new_cmdline + (eap->do_ecmd_cmd - *cmdlinep);
    vim_free(*cmdlinep);
    *cmdlinep = new_cmdline;

    ଫେରନ୍ତୁ src;
}

/*
 * Check for '|' to separate commands and '"' to start comments.
 * If "keep_backslash" is TRUE do not remove any backslash.
 */
    ଶୂନ୍ୟ
separate_nextcmd(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା keep_backslash)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();
    char_u	*p;

#ifdef FEAT_QUICKFIX
    p = skip_grep_pat(eap);
#ଅନ୍ୟଥା
    p = eap->arg;
#endif

    ପାଇଁ ( ; *p; MB_PTR_ADV(p))
    {
	ଯଦି (*p == Ctrl_V)
	{
	    ଯଦି ((eap->argt & (EX_CTRLV | EX_XFILE)) || keep_backslash)
		++p;		// skip CTRL-V and next char
	    ଅନ୍ୟଥା
				// remove CTRL-V and skip next char
		STRMOVE(p, p + 1);
	    ଯଦି (*p == NUL)		// stop at NUL after CTRL-V
		ଭାଙ୍ଗ;
	}

#ifdef FEAT_EVAL
	// Skip over `=expr` when wildcards are expanded.
	ଅନ୍ୟଥା ଯଦି (p[0] == '`' && p[1] == '=' && (eap->argt & EX_XFILE))
	{
	    p += 2;
	    (ଶୂନ୍ୟ)skip_ପଦସମଷ୍ଟି(&p, ଖାଲି);
	    ଯଦି (*p == NUL)		// stop at NUL after CTRL-V
		ଭାଙ୍ଗ;
	}
#endif

	// Check for '"'/'#': start of comment or '|': next command
	// :@" and :*" do not start a comment!
	// :redir @" doesn't either.
	ଅନ୍ୟଥା ଯଦି ((*p == '"'
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
		    && eap->cmdidx != CMD_ଭର୍ତ୍ତି)
		|| *p == '\n')
	{
	    /*
	     * We remove the '\' before the '|', unless EX_CTRLV is used
	     * AND 'b' is present in 'cpoptions'.
	     */
	    ଯଦି ((vim_strchr(p_cpo, CPO_BAR) == ଖାଲି
			      || !(eap->argt & EX_CTRLV)) && *(p - 1) == '\\')
	    {
		ଯଦି (!keep_backslash)
		{
		    STRMOVE(p - 1, p);	// remove the '\'
		    --p;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		set_nextcmd(eap, p);
		*p = NUL;
		ଭାଙ୍ଗ;
	    }
	}
    }

    ଯଦି (!(eap->argt & EX_NOTRLCOM))	// remove trailing spaces
	del_trailing_spaces(eap->arg);
}

/*
 * get + command from ex argument
 */
    ସ୍ଥିର char_u *
getargcmd(char_u **argp)
{
    char_u *arg = *argp;
    char_u *command = ଖାଲି;

    ଯଦି (*arg == '+')	    // +[command]
    {
	++arg;
	ଯଦି (vim_isspace(*arg) || *arg == NUL)
	    command = dollar_command;
	ଅନ୍ୟଥା
	{
	    command = arg;
	    arg = skip_cmd_arg(command, TRUE);
	    ଯଦି (*arg != NUL)
		*arg++ = NUL;		// terminate command with NUL
	}

	arg = skipwhite(arg);	// skip over spaces
	*argp = arg;
    }
    ଫେରନ୍ତୁ command;
}

/*
 * Find end of "+command" argument.  Skip over "\ " and "\\".
 */
    char_u *
skip_cmd_arg(
    char_u *p,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	   rembs)	// TRUE to halve the number of backslashes
{
    ଯେତେବେଳେ (*p && !vim_isspace(*p))
    {
	ଯଦି (*p == '\\' && p[1] != NUL)
	{
	    ଯଦି (rembs)
		STRMOVE(p, p + 1);
	    ଅନ୍ୟଥା
		++p;
	}
	MB_PTR_ADV(p);
    }
    ଫେରନ୍ତୁ p;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_bad_opt(char_u *p, exarg_T *eap)
{
    ଯଦି (STRICMP(p, "keep") == 0)
	eap->bad_char = BAD_KEEP;
    ଅନ୍ୟଥା ଯଦି (STRICMP(p, "drop") == 0)
	eap->bad_char = BAD_DROP;
    ଅନ୍ୟଥା ଯଦି (MB_BYTE2LEN(*p) == 1 && p[1] == NUL)
	eap->bad_char = *p;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of bad= names.
 */
    ସ୍ଥିର char_u *
get_bad_name(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    // Note: Keep this in sync with getargopt.
    ସ୍ଥିର ଅକ୍ଷର *(p_bad_ମୂଲ୍ୟଗୁଡ଼ିକ[]) =
    {
	"?",
	"keep",
	"drop",
    };

    ଯଦି (idx < 0 || idx >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(p_bad_ମୂଲ୍ୟଗୁଡ଼ିକ))
	ଫେରନ୍ତୁ ଖାଲି;

    ଫେରନ୍ତୁ (char_u*)p_bad_ମୂଲ୍ୟଗୁଡ଼ିକ[idx];
}

/*
 * Get "++opt=arg" argument.
 * Return FAIL or OK.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
getargopt(exarg_T *eap)
{
    char_u	*arg = eap->arg + 2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*pp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bad_char_idx;
    char_u	*p;

    // Note: Keep this in sync with get_argopt_name.

    // ":edit ++[no]bin[ary] file"
    ଯଦି (STRNCMP(arg, "bin", 3) == 0 || STRNCMP(arg, "nobin", 5) == 0)
    {
	ଯଦି (*arg == 'n')
	{
	    arg += 2;
	    eap->force_bin = FORCE_NOBIN;
	}
	ଅନ୍ୟଥା
	    eap->force_bin = FORCE_BIN;
	ଯଦି (!checkforcmd(&arg, "binary", 3))
	    ଫେରନ୍ତୁ FAIL;
	eap->arg = skipwhite(arg);
	ଫେରନ୍ତୁ OK;
    }

    // ":read ++edit file"
    ଯଦି (STRNCMP(arg, "edit", 4) == 0)
    {
	eap->read_edit = TRUE;
	eap->arg = skipwhite(arg + 4);
	ଫେରନ୍ତୁ OK;
    }

    ଯଦି (STRNCMP(arg, "ff", 2) == 0)
    {
	arg += 2;
	pp = &eap->force_ff;
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "fileformat", 10) == 0)
    {
	arg += 10;
	pp = &eap->force_ff;
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "enc", 3) == 0)
    {
	ଯଦି (STRNCMP(arg, "encoding", 8) == 0)
	    arg += 8;
	ଅନ୍ୟଥା
	    arg += 3;
	pp = &eap->force_enc;
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "bad", 3) == 0)
    {
	arg += 3;
	pp = &bad_char_idx;
    }

    ଯଦି (pp == ଖାଲି || *arg != '=')
	ଫେରନ୍ତୁ FAIL;

    ++arg;
    *pp = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(arg - eap->cmd);
    arg = skip_cmd_arg(arg, FALSE);
    eap->arg = skipwhite(arg);
    *arg = NUL;

    ଯଦି (pp == &eap->force_ff)
    {
	ଯଦି (check_ff_ମୂଲ୍ୟ(eap->cmd + eap->force_ff) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	eap->force_ff = eap->cmd[eap->force_ff];
    }
    ଅନ୍ୟଥା ଯଦି (pp == &eap->force_enc)
    {
	// Make 'fileencoding' lower case.
	ପାଇଁ (p = eap->cmd + eap->force_enc; *p != NUL; ++p)
	    *p = TOLOWER_ASC(*p);
    }
    ଅନ୍ୟଥା
    {
	// Check ++bad= argument.  Must be a single-byte character, "keep" or
	// "drop".
	ଯଦି (get_bad_opt(eap->cmd + bad_char_idx, eap) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of ++opt names.
 */
    ସ୍ଥିର char_u *
get_argopt_name(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    // Note: Keep this in sync with getargopt.
    ସ୍ଥିର ଅକ୍ଷର *(p_opt_ମୂଲ୍ୟଗୁଡ଼ିକ[]) =
    {
	"fileformat=",
	"encoding=",
	"binary",
	"nobinary",
	"bad=",
	"edit",
    };

    ଯଦି (idx < 0 || idx >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(p_opt_ମୂଲ୍ୟଗୁଡ଼ିକ))
	ଫେରନ୍ତୁ ଖାଲି;

    ଫେରନ୍ତୁ (char_u*)p_opt_ମୂଲ୍ୟଗୁଡ଼ିକ[idx];
}

/*
 * Command-line expansion for ++opt=name.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_argopt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *ଅଙ୍କ_Matches)
{
    ଯଦି (xp->xp_pattern > xp->xp_ଧାଡ଼ି && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା) = ଖାଲି;

	char_u *name_end = xp->xp_pattern - 1;
	ଯଦି (name_end - xp->xp_ଧାଡ଼ି >= 2
		&& STRNCMP(name_end - 2, "ff", 2) == 0)
	    cb = get_fileformat_name;
	ଅନ୍ୟଥା ଯଦି (name_end - xp->xp_ଧାଡ଼ି >= 10
		&& STRNCMP(name_end - 10, "fileformat", 10) == 0)
	    cb = get_fileformat_name;
	ଅନ୍ୟଥା ଯଦି (name_end - xp->xp_ଧାଡ଼ି >= 3
		&& STRNCMP(name_end - 3, "enc", 3) == 0)
	    cb = get_encoding_name;
	ଅନ୍ୟଥା ଯଦି (name_end - xp->xp_ଧାଡ଼ି >= 8
		&& STRNCMP(name_end - 8, "encoding", 8) == 0)
	    cb = get_encoding_name;
	ଅନ୍ୟଥା ଯଦି (name_end - xp->xp_ଧାଡ଼ି >= 3
		&& STRNCMP(name_end - 3, "bad", 3) == 0)
	    cb = get_bad_name;

	ଯଦି (cb != ଖାଲି)
	{
	    ଫେରନ୍ତୁ ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    ଅଙ୍କ_Matches,
		    cb,
		    FALSE);
	}
	ଫେରନ୍ତୁ FAIL;
    }

    // Special handling of "ff" which acts as a short form of
    // "fileformat", as "ff" is not a substring of it.
    ଯଦି (xp->xp_pattern_len == 2
	    && STRNCMP(xp->xp_pattern, "ff", xp->xp_pattern_len) == 0)
    {
	*matches = ALLOC_MULT(char_u *, 1);
	ଯଦି (*matches == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	*ଅଙ୍କ_Matches = 1;
	(*matches)[0] = vim_strsave((char_u*)"fileformat=");
	ଫେରନ୍ତୁ OK;
    }

    ଫେରନ୍ତୁ ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    ଅଙ୍କ_Matches,
	    get_argopt_name,
	    FALSE);
}

    ସ୍ଥିର ଶୂନ୍ୟ
ex_autocmd(exarg_T *eap)
{
    ଯଦି (eap->skip)
    {
	skip_cmd_block(eap);
	ଫେରନ୍ତୁ;
    }

    /*
     * Disallow autocommands from .exrc and .vimrc in current
     * directory for security reasons.
     */
    ଯଦି (secure)
    {
	secure = 2;
	eap->errmsg =
	      _(e_command_not_allowed_from_vimrc_in_current_dir_or_tag_ସନ୍ଧାନ);
    }
    ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_autocmd)
	do_autocmd(eap, eap->arg, eap->forceit);
    ଅନ୍ୟଥା
	do_augroup(eap->arg, eap->forceit);
}

/*
 * ":doautocmd": Apply the automatic commands to the current buffer.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_doautocmd(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		call_do_modelines = check_nomodeline(&arg);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_aucmd;

    (ଶୂନ୍ୟ)do_doautocmd(arg, TRUE, &did_aucmd);
    // Only when there is no <nomodeline>.
    ଯଦି (call_do_modelines && did_aucmd)
	do_modelines(0);
}

/*
 * :[N]bunload[!] [N] [bufname] unload buffer
 * :[N]bdelete[!] [N] [bufname] delete buffer from buffer list
 * :[N]bwipeout[!] [N] [bufname] delete buffer really
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_bunload(exarg_T *eap)
{
    ଯଦି (ERROR_IF_ANY_POPUP_WINDOW)
	ଫେରନ୍ତୁ;
    eap->errmsg = do_bufdel(
	    eap->cmdidx == CMD_bdelete ? DOBUF_DEL
		: eap->cmdidx == CMD_bwipeout ? DOBUF_WIPE
		: DOBUF_UNLOAD, eap->arg,
	    eap->addr_ଗଣନା, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->line1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->line2, eap->forceit);
}

/*
 * :[N]buffer [N]	to buffer N
 * :[N]sbuffer [N]	to buffer N
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_buffer(exarg_T *eap)
{
    ଯଦି (ERROR_IF_ANY_POPUP_WINDOW)
	ଫେରନ୍ତୁ;
    do_exbuffer(eap);
}

/*
 * ":buffer" command and alike.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
do_exbuffer(exarg_T *eap)
{
    ଯଦି (*eap->arg)
	eap->errmsg = ex_errmsg(e_trailing_characters_str, eap->arg);
    ଅନ୍ୟଥା
    {
	ଯଦି (eap->addr_ଗଣନା == 0)	// default is current buffer
	    goto_buffer(eap, DOBUF_CURRENT, FORWARD, 0);
	ଅନ୍ୟଥା
	    goto_buffer(eap, DOBUF_FIRST, FORWARD, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->line2);
	ଯଦି (eap->do_ecmd_cmd != ଖାଲି)
	    do_cmd_argument(eap->do_ecmd_cmd);
    }
}

/*
 * :[N]bmodified [N]	to next mod. buffer
 * :[N]sbmodified [N]	to next mod. buffer
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_bmodified(exarg_T *eap)
{
    goto_buffer(eap, DOBUF_MOD, FORWARD, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->line2);
    ଯଦି (eap->do_ecmd_cmd != ଖାଲି)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bnext [N]	to next buffer
 * :[N]sbnext [N]	split and to next buffer
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_bnext(exarg_T *eap)
{
    ଯଦି (ERROR_IF_ANY_POPUP_WINDOW)
	ଫେରନ୍ତୁ;

    goto_buffer(eap, DOBUF_CURRENT, FORWARD, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->line2);
    ଯଦି (eap->do_ecmd_cmd != ଖାଲି)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bNext [N]	to previous buffer
 * :[N]bprevious [N]	to previous buffer
 * :[N]sbNext [N]	split and to previous buffer
 * :[N]sbprevious [N]	split and to previous buffer
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_bprevious(exarg_T *eap)
{
    ଯଦି (ERROR_IF_ANY_POPUP_WINDOW)
	ଫେରନ୍ତୁ;

    goto_buffer(eap, DOBUF_CURRENT, BACKWARD, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->line2);
    ଯଦି (eap->do_ecmd_cmd != ଖାଲି)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :brewind		to first buffer
 * :bfirst		to first buffer
 * :sbrewind		split and to first buffer
 * :sbfirst		split and to first buffer
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_brewind(exarg_T *eap)
{
    ଯଦି (ERROR_IF_ANY_POPUP_WINDOW)
	ଫେରନ୍ତୁ;

    goto_buffer(eap, DOBUF_FIRST, FORWARD, 0);
    ଯଦି (eap->do_ecmd_cmd != ଖାଲି)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :blast		to last buffer
 * :sblast		split and to last buffer
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_blast(exarg_T *eap)
{
    ଯଦି (ERROR_IF_ANY_POPUP_WINDOW)
	ଫେରନ୍ତୁ;

    goto_buffer(eap, DOBUF_LAST, BACKWARD, 0);
    ଯଦି (eap->do_ecmd_cmd != ଖାଲି)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * Check if "c" ends an Ex command.
 * In Vim9 script does not check for white space before #.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ends_excmd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା comment_char = '"';

    ଯଦି (in_vim9script())
	comment_char = '#';
    ଫେରନ୍ତୁ (c == NUL || c == '|' || c == comment_char || c == '\n');
}

/*
 * Like ends_excmd() but checks that a # in Vim9 script either has "cmd" equal
 * to "cmd_start" or has a white space character before it.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ends_excmd2(char_u *cmd_start UNUSED, char_u *cmd)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = *cmd;

    ଯଦି (c == NUL || c == '|' || c == '\n')
	ଫେରନ୍ତୁ TRUE;
    ଯଦି (in_vim9script())
	//  # starts a comment, #{ might be a mistake, #{{ can start a fold
	ଫେରନ୍ତୁ c == '#' && (cmd[1] != '{' || cmd[2] == '{')
				 && (cmd == cmd_start || VIM_ISWHITE(cmd[-1]));
    ଫେରନ୍ତୁ c == '"';
}

#ଯଦି defined(FEAT_SYN_HL) || defined(FEAT_ସନ୍ଧାନ_EXTRA) || defined(FEAT_EVAL)
/*
 * Return the next command, after the first '|' or '\n'.
 * Return NULL if not found.
 */
    char_u *
find_nextcmd(char_u *p)
{
    ଯେତେବେଳେ (*p != '|' && *p != '\n')
    {
	ଯଦି (*p == NUL)
	    ଫେରନ୍ତୁ ଖାଲି;
	++p;
    }
    ଫେରନ୍ତୁ (p + 1);
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

    ଯଦି (*s == '|' || *s == '\n')
	ଫେରନ୍ତୁ (s + 1);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * If "eap->nextcmd" is not set, check for a next command at "p".
 */
    ଶୂନ୍ୟ
set_nextcmd(exarg_T *eap, char_u *arg)
{
    char_u *p = skipwhite(arg);

    ଯଦି (in_vim9script() && *p == '#')
    {
	char_u *nl = vim_strchr(p, NL);
	ଯଦି (nl != ଖାଲି)
	    p = nl;
    }

    p = check_nextcmd(p);

    ଯଦି (eap->nextcmd == ଖାଲି)
	eap->nextcmd = p;
    ଅନ୍ୟଥା ଯଦି (p != ଖାଲି)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_more(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା message,	    // when FALSE check only, no messages
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    n = ARGCOUNT - curwin->w_arg_idx - 1;

    ଯଦି (!forceit && only_one_window()
	    && ARGCOUNT > 1 && !arg_had_last && n > 0 && quitmore == 0)
    {
	ଯଦି (message)
	{
#ଯଦି defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ଯଦି ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						    && curbuf->b_fname != ଖାଲି)
	    {
		char_u	buff[DIALOG_ସନ୍ଦେଶ_SIZE];

		vim_snprintf((ଅକ୍ଷର *)buff, DIALOG_ସନ୍ଦେଶ_SIZE,
			NGETTEXT("%d more file to edit.  Quit anyway?",
			    "%d more files to edit.  Quit anyway?", n), n);
		ଯଦି (vim_dialog_yesno(VIM_QUESTION, ଖାଲି, buff, 1) == VIM_YES)
		    ଫେରନ୍ତୁ OK;
		ଫେରନ୍ତୁ FAIL;
	    }
#endif
	    semsg(NGETTEXT(e_nr_more_file_to_edit,
			   e_nr_more_files_to_edit , n), n);
	    quitmore = 2;	    // next try to quit is allowed
	}
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of command names.
 */
    char_u *
get_command_name(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)CMD_SIZE)
	ଫେରନ୍ତୁ expand_user_command_name(idx);
    // the following are no real commands
    ଯଦି (STRNCMP(cmdnames[idx].cmd_name, "{", 1) == 0 ||
	STRNCMP(cmdnames[idx].cmd_name, "}", 1) == 0)
	ଫେରନ୍ତୁ (char_u *)"";
    ଫେରନ୍ତୁ cmdnames[idx].cmd_name;
}

    ସ୍ଥିର ଶୂନ୍ୟ
ex_colorscheme(exarg_T *eap)
{
    ଯଦି (*eap->arg == NUL)
    {
#ifdef FEAT_EVAL
	char_u *ପଦସମଷ୍ଟି = vim_strsave((char_u *)"g:colors_name");
	char_u *p = ଖାଲି;

	ଯଦି (ପଦସମଷ୍ଟି != ଖାଲି)
	{
	    ++emsg_off;
	    p = eval_to_string(ପଦସମଷ୍ଟି, FALSE, FALSE);
	    --emsg_off;
	    vim_free(ପଦସମଷ୍ଟି);
	}
	ଯଦି (p != ଖାଲି)
	{
	    ସନ୍ଦେଶ((ଅକ୍ଷର *)p);
	    vim_free(p);
	}
	ଅନ୍ୟଥା
	    ସନ୍ଦେଶ("default");
#ଅନ୍ୟଥା
	ସନ୍ଦେଶ(_("unknown"));
#endif
    }
    ଅନ୍ୟଥା ଯଦି (load_colors(eap->arg) == FAIL)
	semsg(_(e_cannot_find_color_scheme_str), eap->arg);

#ifdef FEAT_VTP
    ଅନ୍ୟଥା ଯଦି (has_vtp_working())
    {
	// background color change requires clear + redraw
	update_screen(UPD_CLEAR);
	redrawcmd();
    }
#endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
ex_highlight(exarg_T *eap)
{
    ଯଦି (*eap->arg == NUL && eap->cmd[2] == '!')
	ସନ୍ଦେଶ(_("Greetings, Vim user!"));
    do_highlight(eap->arg, eap->forceit, FALSE);
}


/*
 * Call this function if we thought we were going to exit, but we won't
 * (because of an error).  May need to restore the terminal mode.
 */
    ଶୂନ୍ୟ
not_exiting(ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_exiting)
{
    exiting = save_exiting;
    settmode(TMODE_RAW);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
before_quit_autocmds(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା quit_all, ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit)
{
    apply_autocmds(EVENT_QUITPRE, ଖାଲି, ଖାଲି, FALSE, wp->w_buffer);

    // Bail out when autocommands closed the window.
    // Refuse to quit when the buffer in the last window is being closed (can
    // only happen in autocommands).
    ଯଦି (!win_valid(wp)
	    || curbuf_locked()
	    || (wp->w_buffer->b_nwindows == 1 && wp->w_buffer->b_locked > 0))
	ଫେରନ୍ତୁ TRUE;

    ଯଦି (quit_all || (check_more(FALSE, forceit) == OK && only_one_window()))
    {
	apply_autocmds(EVENT_EXITPRE, ଖାଲି, ଖାଲି, FALSE, curbuf);
	// Refuse to quit when locked or when the window was closed or the
	// buffer in the last window is being closed (can only happen in
	// autocommands).
	ଯଦି (!win_valid(wp) || curbuf_locked()
			  || (curbuf->b_nwindows == 1 && curbuf->b_locked > 0))
	    ଫେରନ୍ତୁ TRUE;
    }

    ଫେରନ୍ତୁ FALSE;
}

/*
 * ":quit": quit current window, quit Vim if the last window is closed.
 * ":{nr}quit": quit window {nr}
 * Also used when closing a terminal window that's the last one.
 */
    ଶୂନ୍ୟ
ex_quit(exarg_T *eap)
{
    win_T	*wp;

    ଯଦି (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	ଫେରନ୍ତୁ;
    }
    // Don't quit while editing the command line.
    ଯଦି (text_locked())
    {
	text_locked_ସନ୍ଦେଶ();
	ଫେରନ୍ତୁ;
    }
    ଯଦି (eap->addr_ଗଣନା > 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	wnr = eap->line2;

	ପାଇଁ (wp = firstwin; wp->w_next != ଖାଲି; wp = wp->w_next)
	    ଯଦି (--wnr <= 0)
		ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା
	wp = curwin;

    // Refuse to quit when locked.
    ଯଦି (curbuf_locked())
	ଫେରନ୍ତୁ;

    // Trigger QuitPre and maybe ExitPre
    ଯଦି (before_quit_autocmds(wp, FALSE, eap->forceit))
	ଫେରନ୍ତୁ;

#ifdef FEAT_NETBEANS_INTG
    netbeansForcedQuit = eap->forceit;
#endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_exiting = exiting;
    /*
     * If there is only one relevant window we will exit.
     */
    ଯଦି (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;
    ଯଦି ((!buf_hide(wp->w_buffer)
		&& check_changed(wp->w_buffer, (p_awa ? CCGD_AW : 0)
				       | (eap->forceit ? CCGD_FORCEIT : 0)
				       | CCGD_EXCMD))
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, TRUE)))
    {
	not_exiting(save_exiting);
    }
    ଅନ୍ୟଥା
    {
	// quit last window
	// Note: only_one_window() returns true, even so a help window is
	// still open. In that case only quit, if no address has been
	// specified. Example:
	// :h|wincmd w|1q     - don't quit
	// :h|wincmd w|q      - quit
	ଯଦି (only_one_window() && (ONE_WINDOW || eap->addr_ଗଣନା == 0))
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
    ସ୍ଥିର ଶୂନ୍ୟ
ex_cquit(exarg_T *eap UNUSED)
{
    // this does not always pass on the exit code to the Manx compiler. why?
    getout(eap->addr_ଗଣନା > 0 ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->line2 : EXIT_FAILURE);
}

/*
 * Do preparations for "qall" and "wqall".
 * Returns FAIL when quitting should be aborted.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
before_quit_all(exarg_T *eap)
{
    ଯଦି (cmdwin_type != 0)
    {
	ଯଦି (eap->forceit)
	    cmdwin_result = K_XF1;	// ex_window() takes care of this
	ଅନ୍ୟଥା
	    cmdwin_result = K_XF2;
	ଫେରନ୍ତୁ FAIL;
    }

    // Don't quit while editing the command line.
    ଯଦି (text_locked())
    {
	text_locked_ସନ୍ଦେଶ();
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (before_quit_autocmds(curwin, TRUE, eap->forceit))
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ OK;
}

/*
 * ":qall": try to quit all windows
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_quit_all(exarg_T *eap)
{
    ଯଦି (before_quit_all(eap) == FAIL)
	ଫେରନ୍ତୁ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_exiting = exiting;
    exiting = TRUE;
    ଯଦି (eap->forceit || !check_changed_any(FALSE, FALSE))
	getout(0);
    not_exiting(save_exiting);
}

/*
 * ":close": close current window, unless it is the last one
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_close(exarg_T *eap)
{
    win_T	*win;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		winnr = 0;
    ଯଦି (cmdwin_type != 0)
	cmdwin_result = Ctrl_C;
    ଅନ୍ୟଥା
	ଯଦି (!text_locked() && !curbuf_locked())
	{
	    ଯଦି (eap->addr_ଗଣନା == 0)
		ex_win_close(eap->forceit, curwin, ଖାଲି);
	    ଅନ୍ୟଥା
	    {
		FOR_ALL_WINDOWS(win)
		{
		    winnr++;
		    ଯଦି (winnr == eap->line2)
			ଭାଙ୍ଗ;
		}
		ଯଦି (win == ଖାଲି)
		    win = lastwin;
		ex_win_close(eap->forceit, win, ଖାଲି);
	    }
	}
}

#ifdef FEAT_QUICKFIX
/*
 * ":pclose": Close any preview window.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_pclose(exarg_T *eap)
{
    win_T	*win;

    // First close any normal window.
    FOR_ALL_WINDOWS(win)
	ଯଦି (win->w_p_pvw)
	{
	    ex_win_close(eap->forceit, win, ଖାଲି);
	    ଫେରନ୍ତୁ;
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
    ସ୍ଥିର ଶୂନ୍ୟ
ex_win_close(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit,
    win_T	*win,
    tabpage_T	*tp)		// NULL or the tab page "win" is in
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_hide;
    buf_T	*buf = win->w_buffer;

    // Never close the autocommand window.
    ଯଦି (is_aucmd_win(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	ଫେରନ୍ତୁ;
    }
    ଯଦି (window_layout_locked(CMD_close))
	ଫେରନ୍ତୁ;

    need_hide = (bufIsChanged(buf) && buf->b_nwindows <= 1);
    ଯଦି (need_hide && !buf_hide(buf) && !forceit)
    {
#ଯଦି defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ଯଦି ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    ଯଦି (term_job_running(buf->b_term))
	    {
		ଯଦି (term_confirm_stop(buf) == FAIL)
		    ଫେରନ୍ତୁ;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(buf);
		need_hide = FALSE;
	    }
	    ଅନ୍ୟଥା
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(buf, FALSE);
		ଯଦି (bufref_valid(&bufref) && bufIsChanged(buf))
		    ଫେରନ୍ତୁ;
		need_hide = FALSE;
	    }
	}
	ଅନ୍ୟଥା
#endif
	{
	    no_write_message();
	    ଫେରନ୍ତୁ;
	}
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // free buffer when not hiding it or when it's a scratch buffer
    ଯଦି (tp == ଖାଲି)
	win_close(win, !need_hide && !buf_hide(buf));
    ଅନ୍ୟଥା
	win_close_othertab(win, !need_hide && !buf_hide(buf), tp);
}

/*
 * Handle the argument for a tabpage related ex command.
 * Returns a tabpage number.
 * When an error is encountered then eap->errmsg is set.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_tabpage_arg(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tab_ସଂଖ୍ୟା;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା unaccept_arg0 = (eap->cmdidx == CMD_tabmove) ? 0 : 1;

    ଯଦି (eap->arg && *eap->arg != NUL)
    {
	char_u *p = eap->arg;
	char_u *p_save;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା    relative = 0; // argument +N/-N means: go to N places to the
			     // right/left relative to the current position.

	ଯଦି (*p == '-')
	{
	    relative = -1;
	    p++;
	}
	ଅନ୍ୟଥା ଯଦି (*p == '+')
	{
	    relative = 1;
	    p++;
	}

	p_save = p;
	tab_ସଂଖ୍ୟା = getdigits(&p);

	ଯଦି (relative == 0)
	{
	    ଯଦି (STRCMP(p, "$") == 0)
		tab_ସଂଖ୍ୟା = LAST_TAB_NR;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(p, "#") == 0)
		ଯଦି (valid_tabpage(lastused_tabpage))
		    tab_ସଂଖ୍ୟା = tabpage_index(lastused_tabpage);
		ଅନ୍ୟଥା
		{
		    eap->errmsg = ex_errmsg(e_invalid_ମୂଲ୍ୟ_for_argument_str,
								     eap->arg);
		    tab_ସଂଖ୍ୟା = 0;
		    ଯାଅ theend;
		}
	    ଅନ୍ୟଥା ଯଦି (p == p_save || *p_save == '-' || *p != NUL
		    || tab_ସଂଖ୍ୟା > LAST_TAB_NR)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		ଯାଅ theend;
	    }
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (*p_save == NUL)
		tab_ସଂଖ୍ୟା = 1;
	    ଅନ୍ୟଥା ଯଦି (p == p_save || *p_save == '-' || *p != NUL
		    || tab_ସଂଖ୍ୟା == 0)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		ଯାଅ theend;
	    }
	    tab_ସଂଖ୍ୟା = tab_ସଂଖ୍ୟା * relative + tabpage_index(curtab);
	    ଯଦି (!unaccept_arg0 && relative == -1)
		--tab_ସଂଖ୍ୟା;
	}
	ଯଦି (tab_ସଂଖ୍ୟା < unaccept_arg0 || tab_ସଂଖ୍ୟା > LAST_TAB_NR)
	    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
    }
    ଅନ୍ୟଥା ଯଦି (eap->addr_ଗଣନା > 0)
    {
	ଯଦି (unaccept_arg0 && eap->line2 == 0)
	{
	    eap->errmsg = _(e_invalid_range);
	    tab_ସଂଖ୍ୟା = 0;
	}
	ଅନ୍ୟଥା
	{
	    tab_ସଂଖ୍ୟା = eap->line2;
	    ଯଦି (!unaccept_arg0)
	    {
		char_u *cmdp = eap->cmd;

		ଯେତେବେଳେ (--cmdp > *eap->cmdlinep
			&& (VIM_ISWHITE(*cmdp) || VIM_ISDIGIT(*cmdp)))
		    ;
		ଯଦି (*cmdp == '-')
		{
		    --tab_ସଂଖ୍ୟା;
		    ଯଦି (tab_ସଂଖ୍ୟା < unaccept_arg0)
			eap->errmsg = _(e_invalid_range);
		}
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	ବାଛ (eap->cmdidx)
	{
	ସ୍ଥିତି CMD_tabnext:
	    tab_ସଂଖ୍ୟା = tabpage_index(curtab) + 1;
	    ଯଦି (tab_ସଂଖ୍ୟା > LAST_TAB_NR)
		tab_ସଂଖ୍ୟା = 1;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CMD_tabmove:
	    tab_ସଂଖ୍ୟା = LAST_TAB_NR;
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    tab_ସଂଖ୍ୟା = tabpage_index(curtab);
	}
    }

theend:
    ଫେରନ୍ତୁ tab_ସଂଖ୍ୟା;
}

/*
 * ":tabclose": close current tab page, unless it is the last one.
 * ":tabclose N": close tab page N.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_tabclose(exarg_T *eap)
{
    tabpage_T	*tp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tab_ସଂଖ୍ୟା;

    ଯଦି (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	ଫେରନ୍ତୁ;
    }

    ଯଦି (first_tabpage->tp_next == ଖାଲି)
    {
	emsg(_(e_cannot_close_last_tab_page));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (window_layout_locked(CMD_tabclose))
	ଫେରନ୍ତୁ;

    tab_ସଂଖ୍ୟା = get_tabpage_arg(eap);
    ଯଦି (eap->errmsg != ଖାଲି)
	ଫେରନ୍ତୁ;

    tp = find_tabpage(tab_ସଂଖ୍ୟା);
    ଯଦି (tp == ଖାଲି)
    {
	beep_flush();
	ଫେରନ୍ତୁ;
    }
    ଯଦି (tp != curtab)
    {
	tabpage_close_other(tp, eap->forceit);
	ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା ଯଦି (!text_locked() && !curbuf_locked())
	tabpage_close(eap->forceit);
}

/*
 * ":tabonly": close all tab pages except the current one
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_tabonly(exarg_T *eap)
{
    tabpage_T	*tp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		done;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tab_ସଂଖ୍ୟା;

    ଯଦି (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	ଫେରନ୍ତୁ;
    }

    ଯଦି (first_tabpage->tp_next == ଖାଲି)
    {
	ସନ୍ଦେଶ(_("Already only one tab page"));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (window_layout_locked(CMD_tabonly))
	ଫେରନ୍ତୁ;

    tab_ସଂଖ୍ୟା = get_tabpage_arg(eap);
    ଯଦି (eap->errmsg != ଖାଲି)
	ଫେରନ୍ତୁ;

    goto_tabpage(tab_ସଂଖ୍ୟା);
    // Repeat this up to a 1000 times, because autocommands may
    // mess up the lists.
    ପାଇଁ (done = 0; done < 1000; ++done)
    {
	FOR_ALL_TABPAGES(tp)
	    ଯଦି (tp->tp_topframe != topframe)
	    {
		tabpage_close_other(tp, eap->forceit);
		// if we failed to close it quit
		ଯଦି (valid_tabpage(tp))
		    done = 1000;
		// start over, "tp" is now invalid
		ଭାଙ୍ଗ;
	    }
	ଯଦି (first_tabpage->tp_next == ଖାଲି)
	    ଭାଙ୍ଗ;
    }
}

/*
 * Close the current tab page.
 */
    ଶୂନ୍ୟ
tabpage_close(ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit)
{
    ଯଦି (window_layout_locked(CMD_tabclose))
	ଫେରନ୍ତୁ;

    trigger_tabclosedpre(curtab);
    curtab->tp_did_tabclosedpre = TRUE;
    tabpage_T *save_curtab = curtab;

    // First close all the windows but the current one.  If that worked then
    // close the last window in this tab, that will close it.
    ଯଦି (!ONE_WINDOW)
	close_others(TRUE, forceit);
    ଯଦି (ONE_WINDOW)
	ex_win_close(forceit, curwin, ଖାଲି);
    ଯଦି (curtab == save_curtab)
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
    ଶୂନ୍ୟ
tabpage_close_other(tabpage_T *tp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		done = 0;
    win_T	*wp;

    ଯଦି (window_layout_locked(CMD_SIZE))
	ଫେରନ୍ତୁ;

    trigger_tabclosedpre(tp);
    tp->tp_did_tabclosedpre = TRUE;

    // Limit to 1000 windows, autocommands may add a window while we close
    // one.  OK, so I'm paranoid...
    ଯେତେବେଳେ (++done < 1000)
    {
	wp = tp->tp_firstwin;
	ex_win_close(forceit, wp, tp);

	// Autocommands may delete the tab page under our fingers.
	ଯଦି (!valid_tabpage(tp))
	    ଭାଙ୍ଗ;
	// We may fail to close a window with a modified buffer.
	ଯଦି (tp->tp_firstwin == wp)
	{
	    done = 1000;
	    ଭାଙ୍ଗ;
	}
    }
    ଯଦି (done >= 1000)
    {
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	tp->tp_did_tabclosedpre = FALSE;
	ଫେରନ୍ତୁ;
    }

    apply_autocmds(EVENT_TABCLOSED, ଖାଲି, ଖାଲି, FALSE, curbuf);
}

/*
 * ":only".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_only(exarg_T *eap)
{
    ଯଦି (window_layout_locked(CMD_only))
	ଫେରନ୍ତୁ;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    ଯଦି (eap->addr_ଗଣନା > 0)
    {
	win_T   *wp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	wnr = eap->line2;
	ପାଇଁ (wp = firstwin; --wnr > 0; )
	{
	    ଯଦି (wp->w_next == ଖାଲି)
		ଭାଙ୍ଗ;
	    ଅନ୍ୟଥା
		wp = wp->w_next;
	}
	win_goto(wp);
    }
    close_others(TRUE, eap->forceit);
}

    ସ୍ଥିର ଶୂନ୍ୟ
ex_hide(exarg_T *eap UNUSED)
{
    // ":hide" or ":hide | cmd": hide current window
    ଯଦି (eap->skip)
	ଫେରନ୍ତୁ;

    ଯଦି (window_layout_locked(CMD_hide))
	ଫେରନ୍ତୁ;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    ଯଦି (eap->addr_ଗଣନା == 0)
	win_close(curwin, FALSE);	// don't free buffer
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	winnr = 0;
	win_T	*win;

	FOR_ALL_WINDOWS(win)
	{
	    winnr++;
	    ଯଦି (winnr == eap->line2)
		ଭାଙ୍ଗ;
	}
	ଯଦି (win == ଖାଲି)
	    win = lastwin;
	win_close(win, FALSE);
    }
}

/*
 * ":stop" and ":suspend": Suspend Vim.
 */
    ଶୂନ୍ୟ
ex_stop(exarg_T *eap)
{
    /*
     * Disallow suspending for "rvim".
     */
    ଯଦି (check_restricted())
	ଫେରନ୍ତୁ;

    ଯଦି (!eap->forceit)
	autowrite_all();
    apply_autocmds(EVENT_VIMSUSPEND, ଖାଲି, ଖାଲି, FALSE, ଖାଲି);
    windgoto((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 1, cmdline_col_off);
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
    apply_autocmds(EVENT_VIMRESUME, ଖାଲି, ଖାଲି, FALSE, ଖାଲି);
}

/*
 * ":exit", ":xit" and ":wq": Write file and quit the current window.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_exit(exarg_T *eap)
{
#ifdef FEAT_EVAL
    ଯଦି (not_in_vim9(eap, false) == FAIL)
	ଫେରନ୍ତୁ;
#endif
    ଯଦି (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	ଫେରନ୍ତୁ;
    }
    // Don't quit while editing the command line.
    ଯଦି (text_locked())
    {
	text_locked_ସନ୍ଦେଶ();
	ଫେରନ୍ତୁ;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_exiting = exiting;
    /*
     * we plan to exit if there is only one relevant window
     */
    ଯଦି (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;

    // Write the buffer for ":wq" or when it was changed.
    // Trigger QuitPre and ExitPre.
    // Check if we can exit now, after autocommands have changed things.
    ଯଦି (((eap->cmdidx == CMD_wq || curbufIsChanged()) && do_write(eap) == FAIL)
	    || before_quit_autocmds(curwin, FALSE, eap->forceit)
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, FALSE)))
    {
	not_exiting(save_exiting);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (only_one_window())	    // quit last window, exit Vim
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
    ସ୍ଥିର ଶୂନ୍ୟ
ex_print(exarg_T *eap)
{
    ଯଦି (curbuf->b_ml.ml_flags & ML_EMPTY)
	emsg(_(e_empty_buffer));
    ଅନ୍ୟଥା
    {
	ପାଇଁ ( ;!got_int; ui_breakcheck())
	{
	    print_ଧାଡ଼ି(eap->line1,
		    (eap->cmdidx == CMD_ସଂଖ୍ୟା || eap->cmdidx == CMD_pound
						 || (eap->flags & EXFLAG_NR)),
		    eap->cmdidx == CMD_list || (eap->flags & EXFLAG_LIST));
	    ଯଦି (++eap->line1 > eap->line2)
		ଭାଙ୍ଗ;
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
    ସ୍ଥିର ଶୂନ୍ୟ
ex_goto(exarg_T *eap)
{
    goto_byte(eap->line2);
}
#endif

/*
 * ":shell".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_shell(exarg_T *eap UNUSED)
{
    do_shell(ଖାଲି, 0);
}

#ଯଦି defined(HAVE_DROP_FILE)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା drop_busy = FALSE;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା drop_filec;
ସ୍ଥିର char_u **drop_filev = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା drop_split;
ସ୍ଥିର ଶୂନ୍ୟ (*drop_callback)(ଶୂନ୍ୟ *);
ସ୍ଥିର ଶୂନ୍ୟ *drop_cookie;

    ସ୍ଥିର ଶୂନ୍ୟ
handle_drop_internal(ଶୂନ୍ୟ)
{
    exarg_T	ea;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ସନ୍ଦେଶ_scroll = ସନ୍ଦେଶ_scroll;

    // Setting the argument list may cause screen updates and being called
    // recursively.  Avoid that by setting drop_busy.
    drop_busy = TRUE;

    // Check whether the current buffer is changed. If so, we will need
    // to split the current window or data could be lost.
    // We don't need to check if the 'hidden' option is set, as in this
    // case the buffer won't be lost.
    ଯଦି (!buf_hide(curbuf) && !drop_split)
    {
	++emsg_off;
	drop_split = check_changed(curbuf, CCGD_AW);
	--emsg_off;
    }
    ଯଦି (drop_split)
    {
	ଯଦି (win_split(0, 0) == FAIL)
	    ଫେରନ୍ତୁ;
	RESET_BINDING(curwin);

	// When splitting the window, create a new alist.  Otherwise the
	// existing one is overwritten.
	alist_unlink(curwin->w_alist);
	alist_new();
    }

    /*
     * Set up the new argument list.
     */
    alist_set(ALIST(curwin), drop_filec, drop_filev, FALSE, ଖାଲି, 0);

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
    ସନ୍ଦେଶ_scroll = save_ସନ୍ଦେଶ_scroll;

    ଯଦି (drop_callback != ଖାଲି)
	drop_callback(drop_cookie);

    drop_filev = ଖାଲି;
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
    ଶୂନ୍ୟ
handle_drop(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		filec,		// the number of files dropped
    char_u	**filev,	// the list of files dropped
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		split,		// force splitting the window
    ଶୂନ୍ୟ	(*callback)(ଶୂନ୍ୟ *), // to be called after setting the argument
				     // list
    ଶୂନ୍ୟ	*cookie)	// argument for "callback" (allocated)
{
    // Cannot handle recursive drops, finish the pending one.
    ଯଦି (drop_busy)
    {
	FreeWild(filec, filev);
	vim_free(cookie);
	ଫେରନ୍ତୁ;
    }

    // When calling handle_drop() more than once in a row we only use the last
    // one.
    ଯଦି (drop_filev != ଖାଲି)
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
    ଯଦି (text_locked() || curbuf_locked() || updating_screen)
	ଫେରନ୍ତୁ;

    handle_drop_internal();
}

/*
 * To be called when text is unlocked, curbuf is unlocked or updating_screen is
 * reset: Handle a postponed drop.
 */
    ଶୂନ୍ୟ
handle_any_postponed_drop(ଶୂନ୍ୟ)
{
    ଯଦି (!drop_busy && drop_filev != ଖାଲି
	     && !text_locked() && !curbuf_locked() && !updating_screen)
	handle_drop_internal();
}
#endif

/*
 * ":preserve".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_preserve(exarg_T *eap UNUSED)
{
    curbuf->b_flags |= BF_PRESERVED;
    ml_preserve(curbuf, TRUE);
}

/*
 * ":recover".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_recover(exarg_T *eap)
{
    // Set recoverymode right away to avoid the ATTENTION prompt.
    recoverymode = TRUE;
    ଯଦି (!check_changed(curbuf, (p_awa ? CCGD_AW : 0)
			     | CCGD_MULTWIN
			     | (eap->forceit ? CCGD_FORCEIT : 0)
			     | CCGD_EXCMD)

	    && (*eap->arg == NUL
			     || setfname(curbuf, eap->arg, ଖାଲି, TRUE) == OK))
	ml_recover(TRUE);
    recoverymode = FALSE;
}

/*
 * Command modifier used in a wrong way.  Also for other commands that can't
 * appear at the toplevel.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_wrongmodifier(exarg_T *eap)
{
    eap->errmsg = ex_errmsg(e_invalid_command_str, eap->cmd);
}

#ଯଦି defined(FEAT_EVAL)

// callback function for 'findfunc'
ସ୍ଥିର callback_T ffu_cb;

    ସ୍ଥିର callback_T *
get_findfunc_callback(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ *curbuf->b_p_ffu != NUL ? &curbuf->b_ffu_cb : &ffu_cb;
}

/*
 * Call 'findfunc' to obtain a list of file names.
 */
    ସ୍ଥିର list_T *
call_findfunc(char_u *pat, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmdcomplete)
{
    typval_T	args[3];
    callback_T	*cb;
    typval_T	rettv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_BOOL;
    args[1].vval.v_ସଂଖ୍ୟା = cmdcomplete;
    args[2].v_type = VAR_UNKNOWN;

    // Lock the text to prevent weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;

    ctx = get_option_sctx("findfunc");
    ଯଦି (ctx != ଖାଲି)
	current_sctx = *ctx;

    cb = get_findfunc_callback();
    retval = call_callback(cb, -1, &rettv, 2, args);

    current_sctx = saved_sctx;

    --textlock;

    list_T *retlist = ଖାଲି;

    ଯଦି (retval == OK)
    {
	ଯଦି (rettv.v_type == VAR_LIST)
	    retlist = list_copy(rettv.vval.v_list, FALSE, FALSE, get_copyID());
	ଅନ୍ୟଥା
	    emsg(_(e_invalid_return_type_from_findfunc));

	clear_tv(&rettv);
    }

    ଫେରନ୍ତୁ retlist;
}

/*
 * Find file names matching "pat" using 'findfunc' and return it in "files".
 * Used for expanding the :find, :sfind and :tabfind command argument.
 * Returns OK on success and FAIL otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_findfunc(expand_T *xp, char_u *pat, char_u ***files, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches)
{
    list_T	*l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    *ଅଙ୍କ_Matches = 0;
    *files = ଖାଲି;

    l = call_findfunc(pat, VVAL_TRUE);

    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    len = list_len(l);
    ଯଦି (len == 0)	    // empty List
    {
	list_free(l);
	ଫେରନ୍ତୁ FAIL;
    }

    expand_process_user_list(l, files, ଅଙ୍କ_Matches, xp);
    list_free(l);

    ଫେରନ୍ତୁ OK;
}

/*
 * Use 'findfunc' to find file 'findarg'.  The 'count' argument is used to find
 * the n'th matching file.
 */
    ସ୍ଥିର char_u *
findfunc_find_file(char_u *findarg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା findarg_len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    list_T	*fname_list;
    char_u	*ret_fname = ଖାଲି;
    char_u	cc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fname_ଗଣନା;

    cc = findarg[findarg_len];
    findarg[findarg_len] = NUL;

    fname_list = call_findfunc(findarg, VVAL_FALSE);
    fname_ଗଣନା = list_len(fname_list);

    ଯଦି (fname_ଗଣନା == 0)
	semsg(_(e_cant_find_file_str_in_path), findarg);
    ଅନ୍ୟଥା
    {
	ଯଦି (ଗଣନା_2 > fname_ଗଣନା)
	    semsg(_(e_no_more_file_str_found_in_path), findarg);
	ଅନ୍ୟଥା
	{
	    listitem_T *li = list_find(fname_list, ଗଣନା_2 - 1);

	    ଯଦି (li != ଖାଲି)
	    {
		typval_T *tv = &li->li_tv;

		ଯଦି (tv->v_type == VAR_STRING && tv->vval.v_string != ଖାଲି)
		    ret_fname = vim_strsave(tv->vval.v_string);
		ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_DICT && tv->vval.v_dict != ଖାଲି)
		    ret_fname = dict_get_string(tv->vval.v_dict, "word", TRUE);
	    }
	}
    }

    ଯଦି (fname_list != ଖାଲି)
	list_free(fname_list);

    findarg[findarg_len] = cc;

    ଫେରନ୍ତୁ ret_fname;
}

/*
 * Process the 'findfunc' option value.
 * Returns NULL on success and an error message on failure.
 */
    ଅକ୍ଷର *
did_set_findfunc(optset_T *args UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	retval;

    ଯଦି (args->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_ffu, &curbuf->b_ffu_cb);
    ଅନ୍ୟଥା
    {
	// global option set
	retval = option_set_callback_func(p_ffu, &ffu_cb);
	// when using :set, free the local callback
	ଯଦି (!(args->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_ffu_cb);
    }

    ଯଦି (retval == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*name = get_scriptlocal_funcname(*varp);
    ଯଦି (name != ଖାଲି)
    {
	free_string_option(*varp);
	*varp = name;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

# ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_findfunc_option(ଶୂନ୍ୟ)
{
    free_callback(&ffu_cb);
}
# endif

/*
 * Mark the global 'findfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_findfunc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା abort = FALSE;
    abort = set_ref_in_callback(&ffu_cb, copyID);
    ଫେରନ୍ତୁ abort;
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
    ଶୂନ୍ୟ
ex_splitview(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    char_u	*fname = ଖାଲି;
#ifdef FEAT_BROWSE
    char_u	dot_path[] = ".";
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_cmod_flags = cmdmod.cmod_flags;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_tab = eap->cmdidx == CMD_tabedit
		       || eap->cmdidx == CMD_tabfind
		       || eap->cmdidx == CMD_tabnew;

    ଯଦି (ERROR_IF_ANY_POPUP_WINDOW)
	ଫେରନ୍ତୁ;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#ifdef FEAT_QUICKFIX
    // A ":split" in the quickfix window works like ":new".  Don't want two
    // quickfix windows.  But it's OK when doing ":tab split".
    ଯଦି (bt_quickfix(curbuf) && cmdmod.cmod_tab == 0)
    {
	ଯଦି (eap->cmdidx == CMD_split)
	    eap->cmdidx = CMD_new;
	ଯଦି (eap->cmdidx == CMD_vsplit)
	    eap->cmdidx = CMD_vnew;
    }
#endif

    ଯଦି (eap->cmdidx == CMD_sfind || eap->cmdidx == CMD_tabfind)
    {
	char_u	*file_to_find = ଖାଲି;
	ଅକ୍ଷର	*ସନ୍ଧାନ_ctx = ଖାଲି;

	ଯଦି (*get_findfunc() != NUL)
	{
#ifdef FEAT_EVAL
	    fname = findfunc_find_file(eap->arg, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(eap->arg),
				       eap->addr_ଗଣନା > 0 ? eap->line2 : 1);
#endif
	}
	ଅନ୍ୟଥା
	{
	    fname = find_file_in_path(eap->arg, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(eap->arg),
					      FNAME_MESS, TRUE, curbuf->b_ffname,
					      &file_to_find, &ସନ୍ଧାନ_ctx);
	    vim_free(file_to_find);
	    vim_findfile_cleanup(ସନ୍ଧାନ_ctx);
	}
	ଯଦି (fname == ଖାଲି)
	    ଯାଅ theend;
	eap->arg = fname;
    }
#ifdef FEAT_BROWSE
    ଅନ୍ୟଥା ଯଦି ((cmdmod.cmod_flags & CMOD_BROWSE)
	    && eap->cmdidx != CMD_vnew
	    && eap->cmdidx != CMD_new)
    {
	ଯଦି (
# ifdef FEAT_GUI
	    !gui.in_use &&
# endif
		au_has_group((char_u *)"FileExplorer"))
	{
	    // No browsing supported but we do have the file explorer:
	    // Edit the directory.
	    ଯଦି (*eap->arg == NUL || !mch_isdir(eap->arg))
		eap->arg = dot_path;
	}
	ଅନ୍ୟଥା
	{
	    fname = do_browse(0, (char_u *)(use_tab
			? _("Edit File in new tab page")
			: _("Edit File in new window")),
					  eap->arg, ଖାଲି, ଖାଲି, ଖାଲି, curbuf);
	    ଯଦି (fname == ଖାଲି)
		ଯାଅ theend;
	    eap->arg = fname;
	}
    }
    cmdmod.cmod_flags &= ~CMOD_BROWSE;	// Don't browse again in do_ecmd().
#endif

    /*
     * Either open new tab page or split the window.
     */
    ଯଦି (use_tab)
    {
	ଯଦି (win_new_tabpage(cmdmod.cmod_tab != 0 ? cmdmod.cmod_tab
			 : eap->addr_ଗଣନା == 0 ? 0
					       : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->line2 + 1) != FAIL)
	{
	    do_exedit(eap, old_curwin);

	    // set the alternate buffer for the window we came from
	    ଯଦି (curwin != old_curwin
		    && win_valid(old_curwin)
		    && old_curwin->w_buffer != curbuf
		    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		old_curwin->w_alt_fnum = curbuf->b_fnum;
	}
    }
    ଅନ୍ୟଥା ଯଦି (win_split(eap->addr_ଗଣନା > 0 ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->line2 : 0,
				     *eap->cmd == 'v' ? WSP_VERT : 0) != FAIL)
    {
	// Reset 'scrollbind' when editing another file, but keep it when
	// doing ":split" without arguments.
	ଯଦି (*eap->arg != NUL)
	    RESET_BINDING(curwin);
	ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
tabpage_new(ଶୂନ୍ୟ)
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
    ସ୍ଥିର ଶୂନ୍ୟ
ex_tabnext(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tab_ସଂଖ୍ୟା;

    ଯଦି (ERROR_IF_POPUP_WINDOW)
	ଫେରନ୍ତୁ;
    ବାଛ (eap->cmdidx)
    {
	ସ୍ଥିତି CMD_tabfirst:
	ସ୍ଥିତି CMD_tabrewind:
	    goto_tabpage(1);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CMD_tablast:
	    goto_tabpage(9999);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CMD_tabprevious:
	ସ୍ଥିତି CMD_tabNext:
	    ଯଦି (eap->arg && *eap->arg != NUL)
	    {
		char_u *p = eap->arg;
		char_u *p_save = p;

		tab_ସଂଖ୍ୟା = getdigits(&p);
		ଯଦି (p == p_save || *p_save == '-' || *p != NUL
			    || tab_ସଂଖ୍ୟା == 0)
		{
		    // No numbers as argument.
		    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		    ଫେରନ୍ତୁ;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (eap->addr_ଗଣନା == 0)
		    tab_ସଂଖ୍ୟା = 1;
		ଅନ୍ୟଥା
		{
		    tab_ସଂଖ୍ୟା = eap->line2;
		    ଯଦି (tab_ସଂଖ୍ୟା < 1)
		    {
			eap->errmsg = _(e_invalid_range);
			ଫେରନ୍ତୁ;
		    }
		}
	    }
	    goto_tabpage(-tab_ସଂଖ୍ୟା);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ: // CMD_tabnext
	    tab_ସଂଖ୍ୟା = get_tabpage_arg(eap);
	    ଯଦି (eap->errmsg == ଖାଲି)
		goto_tabpage(tab_ସଂଖ୍ୟା);
	    ଭାଙ୍ଗ;
    }
}

/*
 * :tabmove command
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_tabmove(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tab_ସଂଖ୍ୟା;

    tab_ସଂଖ୍ୟା = get_tabpage_arg(eap);
    ଯଦି (eap->errmsg == ଖାଲି)
	tabpage_move(tab_ସଂଖ୍ୟା);
}

/*
 * :tabs command: List tabs and their contents.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_tabs(exarg_T *eap UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tabcount = 1;

    ସନ୍ଦେଶ_start();
    ସନ୍ଦେଶ_scroll = TRUE;
    ପାଇଁ (tp = first_tabpage; tp != ଖାଲି && !got_int; tp = tp->tp_next)
    {
	ସନ୍ଦେଶ_putchar('\n');
	vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, _("Tab page %d"), tabcount++);
	ସନ୍ଦେଶ_outtrans_attr(IObuff, HL_ATTR(HLF_T));
	out_flush();	    // output one line at a time
	ui_breakcheck();

	ଯଦି (tp  == curtab)
	    wp = firstwin;
	ଅନ୍ୟଥା
	    wp = tp->tp_firstwin;
	ପାଇଁ ( ; wp != ଖାଲି && !got_int; wp = wp->w_next)
	{
	    ସନ୍ଦେଶ_putchar('\n');
	    ସନ୍ଦେଶ_putchar(wp == curwin ? '>' : ' ');
	    ସନ୍ଦେଶ_putchar(' ');
	    ସନ୍ଦେଶ_putchar(bufIsChanged(wp->w_buffer) ? '+' : ' ');
	    ସନ୍ଦେଶ_putchar(' ');
	    ଯଦି (buf_spname(wp->w_buffer) != ଖାଲି)
		vim_strncpy(IObuff, buf_spname(wp->w_buffer), IOSIZE - 1);
	    ଅନ୍ୟଥା
		home_replace(wp->w_buffer, wp->w_buffer->b_fname,
							IObuff, IOSIZE, TRUE);
	    ସନ୍ଦେଶ_outtrans(IObuff);
	    out_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }
}

/*
 * ":mode": Set screen mode.
 * If no argument given, just get the screen size and redraw.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_mode(exarg_T *eap)
{
    ଯଦି (*eap->arg == NUL)
	shell_resized();
    ଅନ୍ୟଥା
	emsg(_(e_screen_mode_setting_not_supported));
}

/*
 * ":resize".
 * set, increment or decrement current window height
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_resize(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    win_T	*wp = curwin;

    ଯଦି (eap->addr_ଗଣନା > 0)
    {
	n = eap->line2;
	ପାଇଁ (wp = firstwin; wp->w_next != ଖାଲି && --n > 0; wp = wp->w_next)
	    ;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    n = atol((ଅକ୍ଷର *)eap->arg);
    ଯଦି (cmdmod.cmod_split & WSP_VERT)
    {
	ଯଦି (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_width;
	ଅନ୍ୟଥା ଯଦି (n == 0 && eap->arg[0] == NUL)	// default is very wide
	    n = 9999;
	win_setwidth_win(n, wp);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_height;
	ଅନ୍ୟଥା ଯଦି (n == 0 && eap->arg[0] == NUL)	// default is very high
	    n = 9999;
	win_setheight_win(n, wp);
    }
}

/*
 * ":find [+command] <file>" command.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_find(exarg_T *eap)
{
    ଯଦି (!check_can_set_curbuf_forceit(eap->forceit))
	ଫେରନ୍ତୁ;

    char_u	*fname = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;
    char_u	*file_to_find = ଖାଲି;
    ଅକ୍ଷର	*ସନ୍ଧାନ_ctx = ଖାଲି;

    ଯଦି (*get_findfunc() != NUL)
    {
#ifdef FEAT_EVAL
	fname = findfunc_find_file(eap->arg, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(eap->arg),
					eap->addr_ଗଣନା > 0 ? eap->line2 : 1);
#endif
    }
    ଅନ୍ୟଥା
    {
	fname = find_file_in_path(eap->arg, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(eap->arg), FNAME_MESS,
			       TRUE, curbuf->b_ffname, &file_to_find, &ସନ୍ଧାନ_ctx);
	ଯଦି (eap->addr_ଗଣନା > 0)
	{
	    // Repeat finding the file "count" times.  This matters when it appears
	    // several times in the path.
	    ଗଣନା_2 = eap->line2;
	    ଯେତେବେଳେ (fname != ଖାଲି && --ଗଣନା_2 > 0)
	    {
		vim_free(fname);
		fname = find_file_in_path(ଖାଲି, 0, FNAME_MESS,
			      FALSE, curbuf->b_ffname, &file_to_find, &ସନ୍ଧାନ_ctx);
	    }
	}
	VIM_CLEAR(file_to_find);
	vim_findfile_cleanup(ସନ୍ଧାନ_ctx);
    }

    ଯଦି (fname == ଖାଲି)
	ଫେରନ୍ତୁ;

    eap->arg = fname;
    do_exedit(eap, ଖାଲି);
    vim_free(fname);
}

/*
 * ":open" simulation: for now works just like ":visual".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_open(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*p;

#ifdef FEAT_EVAL
    ଯଦି (not_in_vim9(eap, false) == FAIL)
	ଫେରନ୍ତୁ;
#endif
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_SOL | BL_FIX);
    ଯଦି (*eap->arg == '/')
    {
	// ":open /pattern/": put cursor in column found with pattern
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg, magic_isset() ? RE_MAGIC : 0);
	ଯଦି (regmatch.regprog != ଖାଲି)
	{
	    // make a copy of the line, when searching for a mark it might be
	    // flushed
	    char_u *ଧାଡ଼ି = vim_strsave(ml_get_curline());

	    regmatch.rm_ic = p_ic;
	    ଯଦି (vim_regexec(&regmatch, ଧାଡ଼ି, (colnr_T)0))
		curwin->w_cursor.col = (colnr_T)(regmatch.startp[0] - ଧାଡ଼ି);
	    ଅନ୍ୟଥା
		emsg(_(e_no_match));
	    vim_regfree(regmatch.regprog);
	    vim_free(ଧାଡ଼ି);
	}
	// Move to the NUL, ignore any other arguments.
	eap->arg += STRLEN(eap->arg);
    }
    check_cursor();

    eap->cmdidx = CMD_visual;
    do_exedit(eap, ଖାଲି);
}

/*
 * ":edit", ":badd", ":balt", ":visual".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_edit(exarg_T *eap)
{
    char_u *ffname = eap->cmdidx == CMD_enew ? ଖାଲି : eap->arg;

    // Exclude commands which keep the window's current buffer
    ଯଦି (
	    eap->cmdidx != CMD_badd
	    && eap->cmdidx != CMD_balt
	    // All other commands must obey 'winfixbuf' / ! rules
	    && (is_other_file(0, ffname) && !check_can_set_curbuf_forceit(eap->forceit))
    )
	ଫେରନ୍ତୁ;

    do_exedit(eap, ଖାଲି);
}

/*
 * ":edit <file>" command and alike.
 */
    ଶୂନ୍ୟ
do_exedit(
    exarg_T	*eap,
    win_T	*old_curwin)	    // curwin before doing a split or NULL
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_hide;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		exmode_was = exmode_active;

    ଯଦି ((eap->cmdidx != CMD_pedit && ERROR_IF_POPUP_WINDOW)
						 || ERROR_IF_TERM_POPUP_WINDOW)
	ଫେରନ୍ତୁ;
    /*
     * ":vi" command ends Ex mode.
     */
    ଯଦି (exmode_active && (eap->cmdidx == CMD_visual
						|| eap->cmdidx == CMD_view))
    {
	exmode_active = FALSE;
	ex_pressedreturn = FALSE;
	ଯଦି (*eap->arg == NUL)
	{
	    // Special case:  ":global/pat/visual\NLvi-commands"
	    ଯଦି (global_busy)
	    {
		ଯଦି (eap->nextcmd != ଖାଲି)
		{
		    stuffReadbuff(eap->nextcmd);
		    eap->nextcmd = ଖାଲି;
		}

		ଯଦି (exmode_was != EXMODE_VIM)
		    settmode(TMODE_RAW);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_RedrawingDisabled = RedrawingDisabled;
		RedrawingDisabled = 0;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_nwr = no_wait_return;
		no_wait_return = 0;
		need_wait_return = FALSE;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_ms = ସନ୍ଦେଶ_scroll;
		ସନ୍ଦେଶ_scroll = 0;
#ifdef FEAT_GUI
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_he = hold_gui_events;
		hold_gui_events = 0;
#endif
		set_must_redraw(UPD_CLEAR);
		pending_exmode_active = TRUE;

		main_loop(FALSE, TRUE);

		pending_exmode_active = FALSE;
		RedrawingDisabled = save_RedrawingDisabled;
		no_wait_return = save_nwr;
		ସନ୍ଦେଶ_scroll = save_ms;
#ifdef FEAT_GUI
		hold_gui_events = save_he;
#endif
	    }
	    ଫେରନ୍ତୁ;
	}
    }

    ଯଦି ((eap->cmdidx == CMD_new
		|| eap->cmdidx == CMD_tabnew
		|| eap->cmdidx == CMD_tabedit
		|| eap->cmdidx == CMD_vnew) && *eap->arg == NUL)
    {
	// ":new" or ":tabnew" without argument: edit a new empty buffer
	setpcmark();
	(ଶୂନ୍ୟ)do_ecmd(0, ଖାଲି, ଖାଲି, eap, ECMD_ONE,
		      ECMD_HIDE + (eap->forceit ? ECMD_FORCEIT : 0),
		      old_curwin == ଖାଲି ? curwin : ଖାଲି);
    }
    ଅନ୍ୟଥା ଯଦି ((eap->cmdidx != CMD_split && eap->cmdidx != CMD_vsplit)
	    || *eap->arg != NUL
#ifdef FEAT_BROWSE
	    || (cmdmod.cmod_flags & CMOD_BROWSE)
#endif
	    )
    {
	// Can't edit another file when "textlock" or "curbuf_lock" is set.
	// Only ":edit" or ":script" can bring us here, others are stopped
	// earlier.
	ଯଦି (*eap->arg != NUL && text_or_buf_locked())
	    ଫେରନ୍ତୁ;

	n = readonlymode;
	ଯଦି (eap->cmdidx == CMD_view || eap->cmdidx == CMD_sview)
	    readonlymode = TRUE;
	ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_enew)
	    readonlymode = FALSE;   // 'readonly' doesn't make sense in an
				    // empty buffer
	ଯଦି (eap->cmdidx != CMD_balt && eap->cmdidx != CMD_badd)
	    setpcmark();
	ଯଦି (do_ecmd(0, (eap->cmdidx == CMD_enew ? ଖାଲି : eap->arg),
		    ଖାଲି, eap,
		    // ":edit" goes to first line if Vi compatible
		    (*eap->arg == NUL && eap->do_ecmd_lnum == 0
				      && vim_strchr(p_cpo, CPO_GOTO1) != ଖାଲି)
					       ? ECMD_ONE : eap->do_ecmd_lnum,
		    (buf_hide(curbuf) ? ECMD_HIDE : 0)
		    + (eap->forceit ? ECMD_FORCEIT : 0)
		      // after a split we can use an existing buffer
		    + (old_curwin != ଖାଲି ? ECMD_OLDBUF : 0)
		    + (eap->cmdidx == CMD_badd ? ECMD_ADDBUF : 0)
		    + (eap->cmdidx == CMD_balt ? ECMD_ALTBUF : 0)
		    , old_curwin == ଖାଲି ? curwin : ଖାଲି) == FAIL)
	{
	    // Editing the file failed.  If the window was split, close it.
	    ଯଦି (old_curwin != ଖାଲି)
	    {
		need_hide = (curbufIsChanged() && curbuf->b_nwindows <= 1);
		ଯଦି (!need_hide || buf_hide(curbuf))
		{
#ଯଦି defined(FEAT_EVAL)
		    cleanup_T   cs;

		    // Reset the error/interrupt/exception state here so that
		    // aborting() returns FALSE when closing a window.
		    enter_cleanup(&cs);
#endif
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_close(curwin, !need_hide && !buf_hide(curbuf));

#ଯଦି defined(FEAT_EVAL)
		    // Restore the error/interrupt/exception state if not
		    // discarded by a new aborting error, interrupt, or
		    // uncaught exception.
		    leave_cleanup(&cs);
#endif
		}
	    }
	}
	ଅନ୍ୟଥା ଯଦି (readonlymode && curbuf->b_nwindows == 1)
	{
	    // When editing an already visited buffer, 'readonly' won't be set
	    // but the previous value is kept.  With ":view" and ":sview" we
	    // want the  file to be readonly, except when another window is
	    // editing the same buffer.
	    curbuf->b_p_ro = TRUE;
	}
	readonlymode = n;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (eap->do_ecmd_cmd != ଖାଲି)
	    do_cmd_argument(eap->do_ecmd_cmd);
	n = curwin->w_arg_idx_invalid;
	check_arg_idx(curwin);
	ଯଦି (n != curwin->w_arg_idx_invalid)
	    maketitle();
    }

    /*
     * if ":split file" worked, set alternate file name in old window to new
     * file
     */
    ଯଦି (old_curwin != ଖାଲି
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
    ସ୍ଥିର ଶୂନ୍ୟ
ex_nogui(exarg_T *eap)
{
    eap->errmsg = _(e_gui_cannot_be_used_not_enabled_at_compile_time);
}
#endif

#ଯଦି defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
    ସ୍ଥିର ଶୂନ୍ୟ
ex_tearoff(exarg_T *eap)
{
    gui_make_tearoff(eap->arg);
}
#endif

#ଯଦି (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
    ସ୍ଥିର ଶୂନ୍ୟ
ex_popup(exarg_T *eap)
{
# ଯଦି defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    ଯଦି (gui.in_use)
	gui_make_popup(eap->arg, eap->forceit);
#  ifdef FEAT_TERM_POPUP_MENU
    ଅନ୍ୟଥା
#  endif
# endif
# ifdef FEAT_TERM_POPUP_MENU
	pum_make_popup(eap->arg, eap->forceit);
# endif
}
#endif

    ସ୍ଥିର ଶୂନ୍ୟ
ex_swapname(exarg_T *eap UNUSED)
{
    ଯଦି (curbuf->b_ml.ml_mfp == ଖାଲି || curbuf->b_ml.ml_mfp->mf_fname == ଖାଲି)
	ସନ୍ଦେଶ(_("No swap file"));
    ଅନ୍ୟଥା
	ସନ୍ଦେଶ((ଅକ୍ଷର *)curbuf->b_ml.ml_mfp->mf_fname);
}

/*
 * ":syncbind" forces all 'scrollbind' windows to have the same relative
 * offset.
 * (1998-11-02 16:21:01  R. Edward Ralston <eralston@computer.org>)
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ଯଦି (curwin->w_p_scb)
    {
	topline = curwin->w_topline;
	FOR_ALL_WINDOWS(wp)
	{
	    ଯଦି (wp->w_p_scb && wp->w_buffer)
	    {
		y = wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା - get_scrolloff_ମୂଲ୍ୟ();
		ଯଦି (topline > y)
		    topline = y;
	    }
	}
	ଯଦି (topline < 1)
	    topline = 1;
    }
    ଅନ୍ୟଥା
    {
	topline = 1;
    }


    /*
     * Set all scrollbind windows to the same topline.
     */
    FOR_ALL_WINDOWS(curwin)
    {
	ଯଦି (curwin->w_p_scb)
	{
	    curbuf = curwin->w_buffer;
	    y = topline - curwin->w_topline;
	    ଯଦି (y > 0)
		scrollup(y, TRUE);
	    ଅନ୍ୟଥା
		scrolldown(-y, TRUE);
	    curwin->w_scbind_pos = topline;
	    redraw_later(UPD_VALID);
	    cursor_correct();
	    curwin->w_redr_status = true;
	}
    }
    curwin = save_curwin;
    curbuf = save_curbuf;
    ଯଦି (curwin->w_p_scb)
    {
	did_syncbind = TRUE;
	checkpcmark();
	ଯଦି (old_linenr != curwin->w_cursor.lnum)
	{
	    char_u ctrl_o[2];

	    ctrl_o[0] = Ctrl_O;
	    ctrl_o[1] = 0;
	    ins_typebuf(ctrl_o, REMAP_NONE, 0, TRUE, FALSE);
	}
    }
}


    ସ୍ଥିର ଶୂନ୍ୟ
ex_read(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum;

    ଯଦି (eap->usefilter)			// :r!cmd
    {
	do_bang(1, eap, FALSE, FALSE, TRUE);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (u_save(eap->line2, (linenr_T)(eap->line2 + 1)) == FAIL)
	ଫେରନ୍ତୁ;

#ifdef FEAT_BROWSE
    ଯଦି (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browseFile;

	browseFile = do_browse(0, (char_u *)_("Append File"), eap->arg,
		ଖାଲି, ଖାଲି, ଖାଲି, curbuf);
	ଯଦି (browseFile != ଖାଲି)
	{
	    i = readfile(browseFile, ଖାଲି,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	    vim_free(browseFile);
	}
	ଅନ୍ୟଥା
	    i = OK;
    }
    ଅନ୍ୟଥା
#endif
	ଯଦି (*eap->arg == NUL)
	{
	    ଯଦି (check_fname() == FAIL)	// check for no file name
		ଫେରନ୍ତୁ;
	    i = readfile(curbuf->b_ffname, curbuf->b_fname,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (vim_strchr(p_cpo, CPO_ALTREAD) != ଖାଲି)
		(ଶୂନ୍ୟ)setaltfname(eap->arg, eap->arg, (linenr_T)1);
	    i = readfile(eap->arg, ଖାଲି,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);

	}
    ଯଦି (i != OK)
    {
#ଯଦି defined(FEAT_EVAL)
	ଯଦି (!aborting())
#endif
	    semsg(_(e_cant_open_file_str), eap->arg);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (empty && exmode_active)
	{
	    // Delete the empty line that remains.  Historically ex does
	    // this but vi doesn't.
	    ଯଦି (eap->line2 == 0)
		lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	    ଅନ୍ୟଥା
		lnum = 1;
	    ଯଦି (*ml_get(lnum) == NUL && u_savedel(lnum, 1L) == OK)
	    {
		ml_delete(lnum);
		ଯଦି (curwin->w_cursor.lnum > 1
			&& curwin->w_cursor.lnum >= lnum)
		    --curwin->w_cursor.lnum;
		deleted_ଧାଡ଼ିଗୁଡ଼ିକ_mark(lnum, 1L);
	    }
	}
	redraw_curbuf_later(UPD_VALID);
    }
}

ସ୍ଥିର char_u	*prev_dir = ଖାଲି;

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_cd_dir(ଶୂନ୍ୟ)
{
    VIM_CLEAR(prev_dir);
    VIM_CLEAR(globaldir);
}
#endif

/*
 * Get the previous directory for the given chdir scope.
 */
    ସ୍ଥିର char_u *
get_prevdir(cdscope_T scope)
{
    ଯଦି (scope == CDSCOPE_WINDOW)
	ଫେରନ୍ତୁ curwin->w_prevdir;
    ଅନ୍ୟଥା ଯଦି (scope == CDSCOPE_TABPAGE)
	ଫେରନ୍ତୁ curtab->tp_prevdir;
    ଫେରନ୍ତୁ prev_dir;
}

/*
 * Deal with the side effects of changing the current directory.
 * When 'scope' is CDSCOPE_TABPAGE then this was after an ":tcd" command.
 * When 'scope' is CDSCOPE_WINDOW then this was after an ":lcd" command.
 */
    ଶୂନ୍ୟ
post_chdir(cdscope_T scope)
{
    ଯଦି (scope != CDSCOPE_WINDOW)
	// Clear tab local directory for both :cd and :tcd
	VIM_CLEAR(curtab->tp_localdir);
    VIM_CLEAR(curwin->w_localdir);
    ଯଦି (scope != CDSCOPE_GLOBAL)
    {
	char_u	*pdir = get_prevdir(scope);

	// If still in the global directory, need to remember current
	// directory as the global directory.
	ଯଦି (globaldir == ଖାଲି && pdir != ଖାଲି)
	    globaldir = vim_strsave(pdir);

	// Remember this local directory for the window.
	ଯଦି (mch_dirname(NameBuff, MAXPATHL) == OK)
	{
	    ଯଦି (scope == CDSCOPE_TABPAGE)
		curtab->tp_localdir = vim_strsave(NameBuff);
	    ଅନ୍ୟଥା
		curwin->w_localdir = vim_strsave(NameBuff);
	}
    }
    ଅନ୍ୟଥା
    {
	// We are now in the global directory, no need to remember its name.
	VIM_CLEAR(globaldir);
    }

    last_chdir_reason = ଖାଲି;
    shorten_fnames(vim_strchr(p_cpo, CPO_NOSYMLINKS) == ଖାଲି);
}

/*
 * Trigger DirChangedPre for "acmd_fname" with directory "new_dir".
 */
    ଶୂନ୍ୟ
trigger_DirChangedPre(char_u *acmd_fname, char_u *new_dir)
{
#ifdef FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;

    v_event = get_v_event(&save_v_event);
    (ଶୂନ୍ୟ)dict_add_string(v_event, "directory", new_dir);
    dict_set_ବସ୍ତୁଗୁଡ଼ିକ_ro(v_event);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
changedir_func(
	char_u		*new_dir,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit,
	cdscope_T	scope)
{
    char_u	*pdir = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir_differs;
    char_u	*acmd_fname = ଖାଲି;
    char_u	**pp;
    char_u	*tofree;

    ଯଦି (new_dir == ଖାଲି || allbuf_locked())
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (vim_strchr(p_cpo, CPO_CHDIR) != ଖାଲି && curbufIsChanged() && !forceit)
    {
	emsg(_(e_cannot_change_directory_buffer_is_modified_add_bang_to_override));
	ଫେରନ୍ତୁ FALSE;
    }

    // ":cd -": Change to previous directory
    ଯଦି (STRCMP(new_dir, "-") == 0)
    {
	pdir = get_prevdir(scope);
	ଯଦି (pdir == ଖାଲି)
	{
	    emsg(_(e_no_previous_directory));
	    ଫେରନ୍ତୁ FALSE;
	}
	new_dir = pdir;
    }

    // Save current directory for next ":cd -"
    ଯଦି (mch_dirname(NameBuff, MAXPATHL) == OK)
	pdir = vim_strsave(NameBuff);
    ଅନ୍ୟଥା
	pdir = ଖାଲି;

    // For UNIX ":cd" means: go to home directory.
    // On other systems too if 'cdhome' is set.
#ଯଦି defined(UNIX) || defined(VMS)
    ଯଦି (*new_dir == NUL)
#ଅନ୍ୟଥା
    ଯଦି (*new_dir == NUL && p_cdh)
#endif
    {
	// use NameBuff for home directory name
#ifdef VMS
	char_u	*p;

	p = mch_getenv((char_u *)"SYS$LOGIN");
	ଯଦି (p == ଖାଲି || *p == NUL)	// empty is the same as not set
	    NameBuff[0] = NUL;
	ଅନ୍ୟଥା
	    vim_strncpy(NameBuff, p, MAXPATHL - 1);
#ଅନ୍ୟଥା
	expand_env((char_u *)"$HOME", NameBuff, MAXPATHL);
#endif
	new_dir = NameBuff;
    }
    dir_differs = pdir == ଖାଲି
			    || pathcmp((ଅକ୍ଷର *)pdir, (ଅକ୍ଷର *)new_dir, -1) != 0;
    ଯଦି (dir_differs)
    {
	ଯଦି (scope == CDSCOPE_WINDOW)
	    acmd_fname = (char_u *)"window";
	ଅନ୍ୟଥା ଯଦି (scope == CDSCOPE_TABPAGE)
	    acmd_fname = (char_u *)"tabpage";
	ଅନ୍ୟଥା
	    acmd_fname = (char_u *)"global";
	trigger_DirChangedPre(acmd_fname, new_dir);

	ଯଦି (vim_chdir(new_dir))
	{
	    emsg(_(e_command_failed));
	    vim_free(pdir);
	    ଫେରନ୍ତୁ FALSE;
	}
    }

    ଯଦି (scope == CDSCOPE_WINDOW)
	pp = &curwin->w_prevdir;
    ଅନ୍ୟଥା ଯଦି (scope == CDSCOPE_TABPAGE)
	pp = &curtab->tp_prevdir;
    ଅନ୍ୟଥା
	pp = &prev_dir;
    tofree = *pp;  // new_dir may use this
    *pp = pdir;

    post_chdir(scope);

    ଯଦି (dir_differs)
	apply_autocmds(EVENT_DIRCHANGED, acmd_fname, new_dir, FALSE, curbuf);
    vim_free(tofree);
    ଫେରନ୍ତୁ TRUE;
}

/*
 * ":cd", ":tcd", ":lcd", ":chdir" ":tchdir" and ":lchdir".
 */
    ଶୂନ୍ୟ
ex_cd(exarg_T *eap)
{
    char_u	*new_dir;

    new_dir = eap->arg;
#ଯଦି !defined(UNIX) && !defined(VMS)
    // for non-UNIX ":cd" means: print current directory unless 'cdhome' is set
    ଯଦି (*new_dir == NUL && !p_cdh)
    {
	ex_pwd(ଖାଲି);
	ଫେରନ୍ତୁ;
    }
#endif

    cdscope_T	scope = CDSCOPE_GLOBAL;

    ଯଦି (eap->cmdidx == CMD_lcd || eap->cmdidx == CMD_lchdir)
	scope = CDSCOPE_WINDOW;
    ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_tcd || eap->cmdidx == CMD_tchdir)
	scope = CDSCOPE_TABPAGE;

    ଯଦି (changedir_func(new_dir, eap->forceit, scope))
    {
	// Echo the new current directory if the command was typed.
	ଯଦି (ଚାବି_Typed || p_verbose >= 5)
	    ex_pwd(eap);
    }
}

/*
 * ":pwd".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_pwd(exarg_T *eap UNUSED)
{
    ଯଦି (mch_dirname(NameBuff, MAXPATHL) == OK)
    {
#ifdef BACKSLASH_IN_FILENAME
	slash_adjust(NameBuff);
#endif
	ଯଦି (p_verbose > 0)
	{
	    ଅକ୍ଷର *context = "global";

	    ଯଦି (last_chdir_reason != ଖାଲି)
		context = last_chdir_reason;
	    ଅନ୍ୟଥା ଯଦି (curwin->w_localdir != ଖାଲି)
		context = "window";
	    ଅନ୍ୟଥା ଯଦି (curtab->tp_localdir != ଖାଲି)
		context = "tabpage";
	    smsg("[%s] %s", context, (ଅକ୍ଷର *)NameBuff);
	}
	ଅନ୍ୟଥା
	    ସନ୍ଦେଶ((ଅକ୍ଷର *)NameBuff);
    }
    ଅନ୍ୟଥା
	emsg(_(e_directory_unknown));
}

/*
 * ":=".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_equal(exarg_T *eap)
{
    smsg("%ld", (long)eap->line2);
    ex_may_print(eap);
}

    ସ୍ଥିର ଶୂନ୍ୟ
ex_sleep(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    long	len;

    ଯଦି (cursor_valid())
    {
	n = W_WINROW(curwin) + curwin->w_wrow - ସନ୍ଦେଶ_scrolled;
	ଯଦି (n >= 0)
	    windgoto(n, curwin->w_wincol + curwin->w_wcol);
    }

    len = eap->line2;
    ବାଛ (*eap->arg)
    {
	ସ୍ଥିତି 'm': ଭାଙ୍ଗ;
	ସ୍ଥିତି NUL: len *= 1000L; ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ: semsg(_(e_invalid_argument_str), eap->arg); ଫେରନ୍ତୁ;
    }

    // Hide the cursor if invoked with !
    do_sleep(len, eap->forceit);
}

/*
 * Sleep for "msec" milliseconds, but keep checking for a CTRL-C every second.
 * Hide the cursor if "hide_cursor" is TRUE.
 */
    ଶୂନ୍ୟ
do_sleep(long msec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hide_cursor)
{
    long	done = 0;
    long	wait_now;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    // Remember at what time we started, so that we know how much longer we
    // should wait after waiting for a bit.
    ELAPSED_INIT(start_tv);
#endif

    ଯଦି (hide_cursor)
	cursor_sleep();
    ଅନ୍ୟଥା
	cursor_on();

    out_flush_cursor(FALSE, FALSE);
    ଯେତେବେଳେ (!got_int && done < msec)
    {
	wait_now = msec - done > 1000L ? 1000L : msec - done;
#ifdef FEAT_TIMERS
	{
	    long    due_time = check_due_timer();

	    ଯଦି (due_time > 0 && due_time < wait_now)
		wait_now = due_time;
	}
#endif
#ifdef FEAT_JOB_CHANNEL
	ଯଦି (has_any_channel() && wait_now > 20L)
	    wait_now = 20L;
#endif
#ifdef FEAT_SOUND
	ଯଦି (has_any_sound_callback() && wait_now > 20L)
	    wait_now = 20L;
#endif
	ui_delay(wait_now, TRUE);

#ifdef FEAT_JOB_CHANNEL
	ଯଦି (has_any_channel())
	    ui_breakcheck_force(TRUE);
	ଅନ୍ୟଥା
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
#ଅନ୍ୟଥା
	// guesstimate time passed (will actually be more)
	done += wait_now;
#endif
    }

    // If CTRL-C was typed to interrupt the sleep, drop the CTRL-C from the
    // input buffer, otherwise a following call to input() fails.
    ଯଦି (got_int)
	(ଶୂନ୍ୟ)vpeekc();

    ଯଦି (hide_cursor)
	cursor_unsleep();
}

/*
 * ":winsize" command (obsolete).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_winsize(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		w, h;
    char_u	*arg = eap->arg;
    char_u	*p;

    ଯଦି (!SAFE_isdigit(*arg))
    {
	semsg(_(e_invalid_argument_str), arg);
	ଫେରନ୍ତୁ;
    }
    w = getdigits(&arg);
    arg = skipwhite(arg);
    p = arg;
    h = getdigits(&arg);
    ଯଦି (*p != NUL && *arg == NUL)
	set_shellsize(w, h, TRUE);
    ଅନ୍ୟଥା
	emsg(_(e_winsize_requires_two_ସଂଖ୍ୟା_arguments));
}

    ସ୍ଥିର ଶୂନ୍ୟ
ex_wincmd(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		xchar = NUL;
    char_u	*p;

    ଯଦି (*eap->arg == 'g' || *eap->arg == Ctrl_G)
    {
	// CTRL-W g and CTRL-W CTRL-G  have an extra command character
	ଯଦି (eap->arg[1] == NUL)
	{
	    emsg(_(e_invalid_argument));
	    ଫେରନ୍ତୁ;
	}
	xchar = eap->arg[1];
	p = eap->arg + 2;
    }
    ଅନ୍ୟଥା
	p = eap->arg + 1;

    set_nextcmd(eap, p);
    p = skipwhite(p);
    ଯଦି (*p != NUL && *p != (
#ifdef FEAT_EVAL
	    in_vim9script() ? '#' :
#endif
		'"')
	    && eap->nextcmd == ଖାଲି)
	emsg(_(e_invalid_argument));
    ଅନ୍ୟଥା ଯଦି (!eap->skip)
    {
	// Pass flags on for ":vertical wincmd ]".
	postponed_split_flags = cmdmod.cmod_split;
	postponed_split_tab = cmdmod.cmod_tab;
	do_window(*eap->arg, eap->addr_ଗଣନା > 0 ? eap->line2 : 0L, xchar);
	postponed_split_flags = 0;
	postponed_split_tab = 0;
    }
}

#ଯଦି defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * ":winpos".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_winpos(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x, y;
    char_u	*arg = eap->arg;
    char_u	*p;

    ଯଦି (*arg == NUL)
    {
# ଯଦି defined(FEAT_GUI) || defined(MSWIN)
#  ifdef VIMDLL
	ଯଦି (gui.in_use ? gui_mch_get_winpos(&x, &y) != FAIL :
		mch_get_winpos(&x, &y) != FAIL)
#  elif defined(FEAT_GUI)
	ଯଦି (gui.in_use && gui_mch_get_winpos(&x, &y) != FAIL)
#  ଅନ୍ୟଥା
	ଯଦି (mch_get_winpos(&x, &y) != FAIL)
#  endif
	{
	    sprintf((ଅକ୍ଷର *)IObuff, _("Window position: X %d, Y %d"), x, y);
	    ସନ୍ଦେଶ((ଅକ୍ଷର *)IObuff);
	}
	ଅନ୍ୟଥା
# endif
	    emsg(_(e_obtaining_window_position_not_implemented_for_this_platform));
    }
    ଅନ୍ୟଥା
    {
	x = getdigits(&arg);
	arg = skipwhite(arg);
	p = arg;
	y = getdigits(&arg);
	ଯଦି (*p == NUL || *arg != NUL)
	{
	    emsg(_(e_winpos_requires_two_ସଂଖ୍ୟା_arguments));
	    ଫେରନ୍ତୁ;
	}
# ifdef FEAT_GUI
	ଯଦି (gui.in_use)
	    gui_mch_set_winpos(x, y);
	ଅନ୍ୟଥା ଯଦି (gui.starting)
	{
	    // Remember the coordinates for when the window is opened.
	    gui_win_x = x;
	    gui_win_y = y;
	}
#  ଯଦି defined(HAVE_TGETENT) || defined(VIMDLL)
	ଅନ୍ୟଥା
#  endif
# endif
# ଯଦି defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	    mch_set_winpos(x, y);
# endif
# ifdef HAVE_TGETENT
	ଯଦି (*T_CWP)
	    term_set_winpos(x, y);
# endif
    }
}
#endif

/*
 * Handle command that work like operators: ":delete", ":yank", ":>" and ":<".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_operators(exarg_T *eap)
{
    oparg_T	oa;

    clear_oparg(&oa);
    oa.regname = eap->regname;
    oa.start.lnum = eap->line1;
    oa.end.lnum = eap->line2;
    oa.ଧାଡ଼ି_ଗଣନା = eap->line2 - eap->line1 + 1;
    oa.motion_type = MLINE;
    virtual_op = FALSE;
    ଯଦି (eap->cmdidx != CMD_yank)	// position cursor for undo
    {
	setpcmark();
	curwin->w_cursor.lnum = eap->line1;
	beginline(BL_SOL | BL_FIX);
    }

    ଯଦି (VIsual_active)
	end_visual_mode();

    ବାଛ (eap->cmdidx)
    {
	ସ୍ଥିତି CMD_delete:
	    oa.op_type = OP_DELETE;
	    op_delete(&oa);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି CMD_yank:
	    oa.op_type = OP_YANK;
	    (ଶୂନ୍ୟ)op_yank(&oa, FALSE, TRUE);
	    ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:    // CMD_rshift or CMD_lshift
	    ଯଦି (
#ifdef FEAT_RIGHTLEFT
		(eap->cmdidx == CMD_rshift) ^ curwin->w_p_rl
#ଅନ୍ୟଥା
		eap->cmdidx == CMD_rshift
#endif
						)
		oa.op_type = OP_RSHIFT;
	    ଅନ୍ୟଥା
		oa.op_type = OP_LSHIFT;
	    op_shift(&oa, FALSE, eap->amount);
	    ଭାଙ୍ଗ;
    }
    virtual_op = MAYBE;
    ex_may_print(eap);
}

/*
 * ":put".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_put(exarg_T *eap)
{
    // ":0put" works like ":1put!".
    ଯଦି (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, ଖାଲି, eap->forceit ? BACKWARD : FORWARD, 1L,
						       PUT_ଧାଡ଼ି|PUT_CURSLINE);
}

/*
 * ":iput".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_iput(exarg_T *eap)
{
    // ":0iput" works like ":1iput!".
    ଯଦି (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, ଖାଲି, eap->forceit ? BACKWARD : FORWARD, 1L,
						      PUT_ଧାଡ଼ି|PUT_CURSLINE
						      |PUT_FIXINDENT);
}

/*
 * Handle ":copy" and ":move".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_copymove(exarg_T *eap)
{
    long	n;

#ifdef FEAT_EVAL
    ଯଦି (not_in_vim9(eap, false) == FAIL)
	ଫେରନ୍ତୁ;
#endif
    n = get_address(eap, &eap->arg, eap->addr_type, FALSE, FALSE, FALSE, 1);
    ଯଦି (eap->arg == ଖାଲି)	    // error detected
    {
	eap->nextcmd = ଖାଲି;
	ଫେରନ୍ତୁ;
    }
    get_flags(eap);

    /*
     * move or copy lines from 'eap->line1'-'eap->line2' to below line 'n'
     */
    ଯଦି (n == MAXLNUM || n < 0 || n > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	emsg(_(e_invalid_range));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (eap->cmdidx == CMD_move)
    {
	ଯଦି (do_move(eap->line1, eap->line2, n) == FAIL)
	    ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା
	ex_copy(eap->line1, eap->line2, n);
    u_clearline();
    beginline(BL_SOL | BL_FIX);
    ex_may_print(eap);
}

/*
 * Print the current line if flags were given to the Ex command.
 */
    ଶୂନ୍ୟ
ex_may_print(exarg_T *eap)
{
    ଯଦି (eap->flags != 0)
    {
	print_ଧାଡ଼ି(curwin->w_cursor.lnum, (eap->flags & EXFLAG_NR),
						  (eap->flags & EXFLAG_LIST));
	ex_no_reprint = TRUE;
    }
}

/*
 * ":smagic" and ":snomagic".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ଶୂନ୍ୟ
ex_join(exarg_T *eap)
{
    curwin->w_cursor.lnum = eap->line1;
    ଯଦି (eap->line1 == eap->line2)
    {
	ଯଦି (eap->addr_ଗଣନା >= 2)   // :2,2join does nothing
	    ଫେରନ୍ତୁ;
	ଯଦି (eap->line2 == curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    beep_flush();
	    ଫେରନ୍ତୁ;
	}
	++eap->line2;
    }
    (ଶୂନ୍ୟ)do_join(eap->line2 - eap->line1 + 1, !eap->forceit, TRUE, TRUE, TRUE);
    beginline(BL_WHITE | BL_FIX);
    ex_may_print(eap);
}

/*
 * ":[addr]@r" or ":[addr]*r": execute register
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_at(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_len = typebuf.tb_len;

    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif

    // get the register name.  No name means to use the previous one
    c = *eap->arg;
    ଯଦି (c == NUL || (c == '*' && *eap->cmd == '*'))
	c = '@';
    // Put the register in the typeahead buffer with the "silent" flag.
    ଯଦି (do_execreg(c, TRUE, vim_strchr(p_cpo, CPO_EXECBUF) != ଖାଲି, TRUE)
								      == FAIL)
    {
	beep_flush();
	ଫେରନ୍ତୁ;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_efr = exec_from_reg;

    exec_from_reg = TRUE;

    /*
     * Execute from the typeahead buffer.
     * Continue until the stuff buffer is empty and all added characters
     * have been consumed.
     */
    ଯେତେବେଳେ (!stuff_empty() || typebuf.tb_len > prev_len)
	(ଶୂନ୍ୟ)do_cmdline(ଖାଲି, getexline, ଖାଲି, DOCMD_NOWAIT|DOCMD_VERBOSE);

    exec_from_reg = save_efr;
}

/*
 * ":!".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_bang(exarg_T *eap)
{
    do_bang(eap->addr_ଗଣନା, eap, eap->forceit, TRUE, TRUE);
}

/*
 * ":undo".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_undo(exarg_T *eap)
{
    ଯଦି (eap->addr_ଗଣନା == 1)	    // :undo 123
	undo_time(eap->line2, FALSE, FALSE, TRUE);
    ଅନ୍ୟଥା
	u_undo(1);
}

#ifdef FEAT_PERSISTENT_UNDO
    ସ୍ଥିର ଶୂନ୍ୟ
ex_wundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_SIZE];

    u_compute_hash(hash);
    u_write_undo(eap->arg, eap->forceit, curbuf, hash);
}

    ସ୍ଥିର ଶୂନ୍ୟ
ex_rundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_SIZE];

    u_compute_hash(hash);
    u_read_undo(eap->arg, hash, ଖାଲି);
}
#endif

/*
 * ":redo".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_redo(exarg_T *eap UNUSED)
{
    u_redo(1);
}

/*
 * ":earlier" and ":later".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_later(exarg_T *eap)
{
    long	ଗଣନା_2 = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sec = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		file = FALSE;
    char_u	*p = eap->arg;

    ଯଦି (*p == NUL)
	ଗଣନା_2 = 1;
    ଅନ୍ୟଥା ଯଦି (SAFE_isdigit(*p))
    {
	ଗଣନା_2 = getdigits(&p);
	ବାଛ (*p)
	{
	    ସ୍ଥିତି 's': ++p; sec = TRUE; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'm': ++p; sec = TRUE; ଗଣନା_2 *= 60; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'h': ++p; sec = TRUE; ଗଣନା_2 *= 60 * 60; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'd': ++p; sec = TRUE; ଗଣନା_2 *= 24 * 60 * 60; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'f': ++p; file = TRUE; ଭାଙ୍ଗ;
	}
    }

    ଯଦି (*p != NUL)
	semsg(_(e_invalid_argument_str), eap->arg);
    ଅନ୍ୟଥା
	undo_time(eap->cmdidx == CMD_earlier ? -ଗଣନା_2 : ଗଣନା_2,
							    sec, file, FALSE);
}

/*
 * ":redir": start/stop redirection.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_redir(exarg_T *eap)
{
    ଅକ୍ଷର	*mode;
    char_u	*fname;
    char_u	*arg = eap->arg;

#ifdef FEAT_EVAL
    ଯଦି (redir_execute)
    {
	emsg(_(e_cannot_use_redir_inside_execute));
	ଫେରନ୍ତୁ;
    }
#endif

    ଯଦି (STRICMP(eap->arg, "END") == 0)
	close_redir();
    ଅନ୍ୟଥା
    {
	ଯଦି (*arg == '>')
	{
	    ++arg;
	    ଯଦି (*arg == '>')
	    {
		++arg;
		mode = "a";
	    }
	    ଅନ୍ୟଥା
		mode = "w";
	    arg = skipwhite(arg);

	    close_redir();

	    // Expand environment variables and "~/".
	    fname = expand_env_save(arg);
	    ଯଦି (fname == ଖାଲି)
		ଫେରନ୍ତୁ;
#ifdef FEAT_BROWSE
	    ଯଦି (cmdmod.cmod_flags & CMOD_BROWSE)
	    {
		char_u	*browseFile;

		browseFile = do_browse(BROWSE_SAVE,
			(char_u *)_("Save Redirection"),
			fname, ଖାଲି, ଖାଲି,
			(char_u *)_(BROWSE_FILTER_ALL_FILES), curbuf);
		ଯଦି (browseFile == ଖାଲି)
		    ଫେରନ୍ତୁ;		// operation cancelled
		vim_free(fname);
		fname = browseFile;
		eap->forceit = TRUE;	// since dialog already asked
	    }
#endif

	    redir_fd = open_exfile(fname, eap->forceit, mode);
	    vim_free(fname);
	}
#ifdef FEAT_EVAL
	ଅନ୍ୟଥା ଯଦି (*arg == '@')
	{
	    // redirect to a register a-z (resp. A-Z for appending)
	    close_redir();
	    ++arg;
	    ଯଦି (ASCII_ISALPHA(*arg)
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
		ଯଦି (*arg == '>' && arg[1] == '>')  // append
		    arg += 2;
		ଅନ୍ୟଥା
		{
		    // Can use both "@a" and "@a>".
		    ଯଦି (*arg == '>')
			arg++;
		    // Make register empty when not using @A-@Z and the
		    // command is valid.
		    ଯଦି (*arg == NUL && !SAFE_isupper(redir_reg))
			write_reg_contents(redir_reg, (char_u *)"", -1, FALSE);
		}
	    }
	    ଯଦି (*arg != NUL)
	    {
		redir_reg = 0;
		semsg(_(e_invalid_argument_str), eap->arg);
	    }
	}
	ଅନ୍ୟଥା ଯଦି (*arg == '=' && arg[1] == '>')
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା append;

	    // redirect to a variable
	    close_redir();
	    arg += 2;

	    ଯଦି (*arg == '>')
	    {
		++arg;
		append = TRUE;
	    }
	    ଅନ୍ୟଥା
		append = FALSE;

	    ଯଦି (var_redir_start(skipwhite(arg), append) == OK)
		redir_vname = 1;
	}
#endif

	// TODO: redirect to a buffer

	ଅନ୍ୟଥା
	    semsg(_(e_invalid_argument_str), eap->arg);
    }

    // Make sure redirection is not off.  Can happen for cmdline completion
    // that indirectly invokes a command to catch its output.
    ଯଦି (redir_fd != ଖାଲି
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname
#endif
							)
	redir_off = FALSE;
}

/*
 * ":redraw": force redraw, with clear for ":redraw!".
 */
    ଶୂନ୍ୟ
ex_redraw(exarg_T *eap)
{
    redraw_cmd(eap->forceit);
}

/*
 * ":redraw": force redraw, with clear if "clear" is TRUE.
 */
    ଶୂନ୍ୟ
redraw_cmd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା clear)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_p_lz = p_lz;
    p_lz = FALSE;

    validate_cursor();
    update_topline();
    update_screen(clear ? UPD_CLEAR : VIsual_active ? UPD_INVERTED : 0);
    ଯଦି ((State & MODE_CMDLINE) == 0)
	setcursor(); // put cursor back where it belongs
    ଯଦି (need_maketitle)
	maketitle();
#ଯଦି defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
    ଯଦି (!gui.in_use)
# endif
	resize_console_buf();
#endif
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;

    // After drawing the statusline screen_attr may still be set.
    screen_stop_highlight();

    // Reset msg_didout, so that a message that's there is overwritten.
    ସନ୍ଦେଶ_didout = FALSE;
    ସନ୍ଦେଶ_col = 0;

    // No need to wait after an intentional redraw.
    need_wait_return = FALSE;

    // When invoked from a callback or autocmd the command line may be active.
    ଯଦି (State & MODE_CMDLINE)
	redrawcmdline();

    out_flush();
}

/*
 * ":redrawstatus": force redraw of status line(s)
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_redrawstatus(exarg_T *eap UNUSED)
{
    ଯଦି (eap->forceit)
	status_redraw_all();
    ଅନ୍ୟଥା
	status_redraw_curbuf();
    redraw_vseps = TRUE;
    ଯଦି (ସନ୍ଦେଶ_scrolled && (State & MODE_CMDLINE))
	ଫେରନ୍ତୁ;  // redraw later

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_p_lz = p_lz;
    p_lz = FALSE;

    ଯଦି (State & MODE_CMDLINE)
	redraw_statuslines();
    ଅନ୍ୟଥା
	update_screen(VIsual_active ? UPD_INVERTED : 0);
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabline": force redraw of the tabline
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_redrawtabline(exarg_T *eap UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_p_lz = p_lz;
    p_lz = FALSE;

    draw_tabline();

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabpanel": force redraw of the tabpanel
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_redrawtabpanel(exarg_T *eap UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_p_lz = p_lz;
    p_lz = FALSE;

#ଯଦି defined(FEAT_TABPANEL)
    draw_tabpanel();
#ଅନ୍ୟଥା
    emsg(_(e_cannot_not_support_redrawtabpanel));
#endif

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

    ସ୍ଥିର ଶୂନ୍ୟ
close_redir(ଶୂନ୍ୟ)
{
    ଯଦି (redir_fd != ଖାଲି)
    {
	fclose(redir_fd);
	redir_fd = ଖାଲି;
    }
#ifdef FEAT_EVAL
    redir_reg = 0;
    ଯଦି (redir_vname)
    {
	var_redir_stop();
	redir_vname = 0;
    }
#endif
}

#ଯଦି defined(FEAT_SESSION) || defined(FEAT_EVAL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_mkdir_emsg(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା prot UNUSED)
{
    ଯଦି (vim_mkdir(name, prot) != 0)
    {
	semsg(_(e_cannot_create_directory_str), name);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}
#endif

/*
 * Open a file for writing for an Ex command, with some checks.
 * Return file descriptor, or NULL on failure.
 */
    FILE *
open_exfile(
    char_u	*fname,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit,
    ଅକ୍ଷର	*mode)	    // "w" for create new file or "a" for append
{
    FILE	*fd;

#ifdef UNIX
    // with Unix it is possible to open a directory
    ଯଦି (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	ଫେରନ୍ତୁ ଖାଲି;
    }
#endif
    ଯଦି (!forceit && *mode != 'a' && vim_fexists(fname))
    {
	semsg(_(e_str_exists_add_bang_to_override), fname);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି ((fd = mch_fopen((ଅକ୍ଷର *)fname, mode)) == ଖାଲି)
	semsg(_(e_cannot_open_str_for_writing_2), fname);

    ଫେରନ୍ତୁ fd;
}

/*
 * ":mark" and ":k".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_mark(exarg_T *eap)
{
    pos_T	pos;

#ifdef FEAT_EVAL
    ଯଦି (not_in_vim9(eap, false) == FAIL)
	ଫେରନ୍ତୁ;
#endif
    ଯଦି (*eap->arg == NUL)		// No argument?
    {
	emsg(_(e_argument_required));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (eap->arg[1] != NUL)	// more than one character?
    {
	semsg(_(e_trailing_characters_str), eap->arg);
	ଫେରନ୍ତୁ;
    }

    pos = curwin->w_cursor;		// save curwin->w_cursor
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_WHITE | BL_FIX);
    ଯଦି (setmark(*eap->arg) == FAIL)	// set mark
	emsg(_(e_argument_must_be_letter_or_forward_backward_quote));
    curwin->w_cursor = pos;		// restore curwin->w_cursor
}

/*
 * Update w_topline, w_leftcol and the cursor position.
 */
    ଶୂନ୍ୟ
update_topline_cursor(ଶୂନ୍ୟ)
{
    check_cursor();		// put cursor on valid line
    update_topline();
    ଯଦି (!curwin->w_p_wrap)
	validate_cursor();
    update_curswant();
}

/*
 * Save the current State and go to Normal mode.
 * Return TRUE if the typeahead could be saved.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
save_current_state(save_state_T *sst)
{
    sst->save_ସନ୍ଦେଶ_scroll = ସନ୍ଦେଶ_scroll;
    sst->save_restart_edit = restart_edit;
    sst->save_ସନ୍ଦେଶ_didout = ସନ୍ଦେଶ_didout;
    sst->save_State = State;
    sst->save_insertmode = p_im;
    sst->save_finish_op = finish_op;
    sst->save_opcount = opcount;
    sst->save_reg_executing = reg_executing;
    sst->save_pending_end_reg_executing = pending_end_reg_executing;

    ସନ୍ଦେଶ_scroll = FALSE;		    // no msg scrolling in Normal mode
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
    ଫେରନ୍ତୁ sst->tabuf.typebuf_valid;
}

    ଶୂନ୍ୟ
restore_current_state(save_state_T *sst)
{
    // Restore the previous typeahead.
    restore_typeahead(&sst->tabuf, FALSE);

    ସନ୍ଦେଶ_scroll = sst->save_ସନ୍ଦେଶ_scroll;
    restart_edit = sst->save_restart_edit;
    p_im = sst->save_insertmode;
    finish_op = sst->save_finish_op;
    opcount = sst->save_opcount;
    reg_executing = sst->save_reg_executing;
    pending_end_reg_executing = sst->save_pending_end_reg_executing;
    ସନ୍ଦେଶ_didout |= sst->save_ସନ୍ଦେଶ_didout;	// don't reset msg_didout now
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
    ଶୂନ୍ୟ
ex_normal(exarg_T *eap)
{
    save_state_T save_state;
    char_u	*arg = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;
    char_u	*p;

    ଯଦି (ex_normal_lock > 0)
    {
	emsg(_(e_not_allowed_here));
	ଫେରନ୍ତୁ;
    }
    ଯଦି (ex_normal_busy >= p_mmd)
    {
	emsg(_(e_recursive_use_of_normal_too_deep));
	ଫେରନ୍ତୁ;
    }

    /*
     * vgetc() expects a CSI and K_SPECIAL to have been escaped.  Don't do
     * this for the K_SPECIAL leading byte, otherwise special keys will not
     * work.
     */
    ଯଦି (has_mbyte)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len = 0;

	// Count the number of characters to be escaped.
	ପାଇଁ (p = eap->arg; *p != NUL; ++p)
	{
#ifdef FEAT_GUI
	    ଯଦି (*p == CSI)  // leadbyte CSI
		len += 2;
#endif
	    ପାଇଁ (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		ଯଦି (*++p == K_SPECIAL	  // trailbyte K_SPECIAL or CSI
#ifdef FEAT_GUI
			|| *p == CSI
#endif
			)
		    len += 2;
	}
	ଯଦି (len > 0)
	{
	    arg = alloc(STRLEN(eap->arg) + len + 1);
	    ଯଦି (arg != ଖାଲି)
	    {
		len = 0;
		ପାଇଁ (p = eap->arg; *p != NUL; ++p)
		{
		    arg[len++] = *p;
#ifdef FEAT_GUI
		    ଯଦି (*p == CSI)
		    {
			arg[len++] = KS_EXTRA;
			arg[len++] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_CSI;
		    }
#endif
		    ପାଇଁ (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		    {
			arg[len++] = *++p;
			ଯଦି (*p == K_SPECIAL)
			{
			    arg[len++] = KS_SPECIAL;
			    arg[len++] = KE_FILLER;
			}
#ifdef FEAT_GUI
			ଅନ୍ୟଥା ଯଦି (*p == CSI)
			{
			    arg[len++] = KS_EXTRA;
			    arg[len++] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_CSI;
			}
#endif
		    }
		    arg[len] = NUL;
		}
	    }
	}
    }

    ++ex_normal_busy;
    ଯଦି (save_current_state(&save_state))
    {
	/*
	 * Repeat the :normal command for each line in the range.  When no
	 * range given, execute it just once, without positioning the cursor
	 * first.
	 */
	କର
	{
	    ଯଦି (eap->addr_ଗଣନା != 0)
	    {
		curwin->w_cursor.lnum = eap->line1++;
		curwin->w_cursor.col = 0;
		check_cursor_moved(curwin);
	    }

	    exec_normal_cmd(arg != ଖାଲି
		     ? arg
		     : eap->arg, eap->forceit ? REMAP_NONE : REMAP_YES, FALSE);
	}
	ଯେତେବେଳେ (eap->addr_ଗଣନା > 0 && eap->line1 <= eap->line2 && !got_int);
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
    ସ୍ଥିର ଶୂନ୍ୟ
ex_startinsert(exarg_T *eap)
{
#ifdef FEAT_TERMINAL
    // Ignore this when running in an active terminal.
    ଯଦି (term_job_running(curbuf->b_term))
	ଫେରନ୍ତୁ;
#endif
    ଯଦି (!curbuf->b_p_ma && !p_im)
    {
	// Only give this error when 'insertmode' is off.
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ଫେରନ୍ତୁ;
    }
    ଯଦି (eap->forceit)
    {
	// cursor line can be zero on startup
	ଯଦି (!curwin->w_cursor.lnum)
	    curwin->w_cursor.lnum = 1;
	set_cursor_for_append_to_ଧାଡ଼ି();
    }

    // Ignore the command when already in Insert mode.  Inserting an
    // expression register that invokes a function can do this.
    ଯଦି (State & MODE_ଭର୍ତ୍ତି)
	ଫେରନ୍ତୁ;

    ଯଦି (eap->cmdidx == CMD_startinsert)
	restart_edit = 'a';
    ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_startreplace)
	restart_edit = 'R';
    ଅନ୍ୟଥା
	restart_edit = 'V';

    ଯଦି (!eap->forceit)
    {
	ଯଦି (eap->cmdidx == CMD_startinsert)
	    restart_edit = 'i';
	curwin->w_curswant = 0;	    // avoid MAXCOL
    }

    ଯଦି (VIsual_active)
	showmode();
}

/*
 * ":stopinsert"
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_stopinsert(exarg_T *eap UNUSED)
{
    restart_edit = 0;
    stop_ଭର୍ତ୍ତି_mode = TRUE;
#ଯଦି defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // when called from remote_expr in insert mode, make sure insert mode is
    // ended by adding K_NOP to the typeahead buffer
    ଯଦି (vgetc_busy)
	ins_char_typebuf(K_NOP, 0);
#endif
    clearmode();
}

/*
 * Execute normal mode command "cmd".
 * "remap" can be REMAP_NONE or REMAP_YES.
 */
    ଶୂନ୍ୟ
exec_normal_cmd(char_u *cmd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା remap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା silent)
{
    // Stuff the argument into the typeahead buffer.
    ins_typebuf(cmd, remap, 0, TRUE, silent);
    exec_normal(FALSE, FALSE, FALSE);
}

/*
 * Execute normal_cmd() until there is no typeahead left.
 * When "use_vpeekc" is TRUE use vpeekc() to check for available chars.
 */
    ଶୂନ୍ୟ
exec_normal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା was_typed, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_vpeekc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା may_use_terminal_loop UNUSED)
{
    oparg_T	oa;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    // When calling vpeekc() from feedkeys() it will return Ctrl_C when there
    // is nothing to get, so also check for Ctrl_C.
    clear_oparg(&oa);
    finish_op = FALSE;
    ଯେତେବେଳେ ((!stuff_empty()
		|| ((was_typed || !typebuf_typed()) && typebuf.tb_len > 0)
		|| (use_vpeekc && (c = vpeekc()) != NUL && c != Ctrl_C))
	    && !got_int)
    {
	update_topline_cursor();
#ifdef FEAT_TERMINAL
	ଯଦି (may_use_terminal_loop && term_use_loop()
		&& oa.op_type == OP_NOP && oa.regname == NUL
		&& !VIsual_active)
	{
	    // If terminal_loop() returns OK we got a key that is handled
	    // in Normal model.  With FAIL we first need to position the
	    // cursor and the screen needs to be redrawn.
	    ଯଦି (terminal_loop(TRUE) == OK)
		normal_cmd(&oa, TRUE);
	}
	ଅନ୍ୟଥା
#endif
	    // execute a Normal mode cmd
	    normal_cmd(&oa, TRUE);
    }
}

#ifdef FEAT_FIND_ID
    ସ୍ଥିର ଶୂନ୍ୟ
ex_checkpath(exarg_T *eap)
{
    find_pattern_in_path(ଖାଲି, 0, 0, FALSE, FALSE, CHECK_PATH, 1L,
	    eap->forceit ? ACTION_SHOW_ALL : ACTION_SHOW,
	    (linenr_T)1, (linenr_T)MAXLNUM, eap->forceit, FALSE);
}

# ଯଦି defined(FEAT_QUICKFIX)
/*
 * ":psearch"
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_psearch(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;
    ex_findpat(eap);
    g_do_tagpreview = 0;
}
# endif

    ସ୍ଥିର ଶୂନ୍ୟ
ex_findpat(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		whole = TRUE;
    long	n;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		action;

    ବାଛ (cmdnames[eap->cmdidx].cmd_name[2])
    {
	ସ୍ଥିତି 'e':	// ":psearch", ":isearch" and ":dsearch"
		ଯଦି (cmdnames[eap->cmdidx].cmd_name[0] == 'p')
		    action = ACTION_GOTO;
		ଅନ୍ୟଥା
		    action = ACTION_SHOW;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 'i':	// ":ilist" and ":dlist"
		action = ACTION_SHOW_ALL;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 'u':	// ":ijump" and ":djump"
		action = ACTION_GOTO;
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	// ":isplit" and ":dsplit"
		action = ACTION_SPLIT;
		ଭାଙ୍ଗ;
    }

    n = 1;
    ଯଦି (vim_isdigit(*eap->arg))	// get count
    {
	n = getdigits(&eap->arg);
	eap->arg = skipwhite(eap->arg);
    }
    ଯଦି (*eap->arg == '/')   // Match regexp, not just whole words
    {
	whole = FALSE;
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	ଯଦି (*p)
	{
	    *p++ = NUL;
	    p = skipwhite(p);

	    // Check for trailing illegal characters
	    ଯଦି (!ends_excmd2(eap->arg, p))
		eap->errmsg = ex_errmsg(e_trailing_characters_str, p);
	    ଅନ୍ୟଥା
		set_nextcmd(eap, p);
	}
    }
    ଯଦି (!eap->skip)
	find_pattern_in_path(eap->arg, 0, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(eap->arg),
		whole, !eap->forceit,
		*eap->cmd == 'd' ? FIND_DEFINE : FIND_ANY, n, action,
		eap->line1, eap->line2, eap->forceit, FALSE);
}
#endif


#ifdef FEAT_QUICKFIX
/*
 * ":ptag", ":ptselect", ":ptjump", ":ptnext", etc.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_ptag(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;  // will be reset to 0 in ex_tag_cmd()
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
}

/*
 * ":pedit"
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_pedit(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Edit the file.
    do_exedit(eap, ଖାଲି);

    back_to_current_window(curwin_save);
}

/*
 * ":pbuffer"
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_pbuffer(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Go to the buffer.
    do_exbuffer(eap);

    back_to_current_window(curwin_save);
}

    ସ୍ଥିର ଶୂନ୍ୟ
prepare_preview_window(ଶୂନ୍ୟ)
{
    ଯଦି (ERROR_IF_ANY_POPUP_WINDOW)
	ଫେରନ୍ତୁ;

    // Open the preview window or popup and make it the current window.
    g_do_tagpreview = p_pvh;
    prepare_tagpreview(TRUE, TRUE, FALSE);
}

    ସ୍ଥିର ଶୂନ୍ୟ
back_to_current_window(win_T *curwin_save)
{
    ଯଦି (curwin != curwin_save && win_valid(curwin_save))
    {
	// Return cursor to where we were
	validate_cursor();
	redraw_later(UPD_VALID);
	win_enter(curwin_save, TRUE);
    }
# ifdef FEAT_PROP_POPUP
    ଅନ୍ୟଥା ଯଦି (WIN_IS_POPUP(curwin))
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
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ଶୂନ୍ୟ
ex_tag(exarg_T *eap)
{
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name);
}

    ସ୍ଥିର ଶୂନ୍ୟ
ex_tag_cmd(exarg_T *eap, char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmd;

    ବାଛ (name[1])
    {
	ସ୍ଥିତି 'j': cmd = DT_JUMP;	// ":tjump"
		  ଭାଙ୍ଗ;
	ସ୍ଥିତି 's': cmd = DT_SELECT;	// ":tselect"
		  ଭାଙ୍ଗ;
	ସ୍ଥିତି 'p': cmd = DT_PREV;	// ":tprevious"
		  ଭାଙ୍ଗ;
	ସ୍ଥିତି 'N': cmd = DT_PREV;	// ":tNext"
		  ଭାଙ୍ଗ;
	ସ୍ଥିତି 'n': cmd = DT_NEXT;	// ":tnext"
		  ଭାଙ୍ଗ;
	ସ୍ଥିତି 'o': cmd = DT_କାଢ଼;		// ":pop"
		  ଭାଙ୍ଗ;
	ସ୍ଥିତି 'f':			// ":tfirst"
	ସ୍ଥିତି 'r': cmd = DT_FIRST;	// ":trewind"
		  ଭାଙ୍ଗ;
	ସ୍ଥିତି 'l': cmd = DT_LAST;	// ":tlast"
		  ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:			// ":tag"
#ifdef FEAT_CSCOPE
		  ଯଦି (p_cst && *eap->arg != NUL)
		  {
		      ex_cstag(eap);
		      ଫେରନ୍ତୁ;
		  }
#endif
		  cmd = DT_TAG;
		  ଭାଙ୍ଗ;
    }

    ଯଦି (name[0] == 'l')
    {
#ifndef FEAT_QUICKFIX
	ex_ni(eap);
	ଫେରନ୍ତୁ;
#ଅନ୍ୟଥା
	cmd = DT_LTAG;
#endif
    }

    do_tag(eap->arg, cmd, eap->addr_ଗଣନା > 0 ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->line2 : 1,
							  eap->forceit, TRUE);
}

ଗଣନା {
    SPEC_PERC = 0,
    SPEC_HASH,
    SPEC_CWORD,	    // cursor word
    SPEC_CCWORD,    // cursor WORD
    SPEC_CEXPR,	    // expr under cursor
    SPEC_CFILE,	    // cursor path name
    SPEC_SFILE,	    // ":so" file name
    SPEC_SLNUM,	    // ":so" file line number
    SPEC_ଗଦା,	    // call stack
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_cmdline_var(char_u *src, size_t *usedlen)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    ସ୍ଥିର keyvalue_T spec_str_tab[] = {
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
	KEYVALUE_ENTRY(SPEC_ଗଦା, "stack>")	    // call stack
    };
    keyvalue_T target;
    keyvalue_T *entry;

    ବାଛ (*src)
    {
    ସ୍ଥିତି '%':
	*usedlen = 1;
	ଫେରନ୍ତୁ SPEC_PERC;

    ସ୍ଥିତି '#':
	*usedlen = 1;
	ଫେରନ୍ତୁ SPEC_HASH;

    ସ୍ଥିତି '<':
	target.ଚାବି = 0;
	target.ମୂଲ୍ୟ.string = src + 1;	    // skip over '<'
	target.ମୂଲ୍ୟ.length = 0;	    // not used, see cmp_keyvalue_value_n()

	entry = (keyvalue_T *)bsearch(&target, &spec_str_tab,
		ARRAY_LENGTH(spec_str_tab), ଆକାର(spec_str_tab[0]),
		cmp_keyvalue_ମୂଲ୍ୟ_n);
	ଯଦି (entry == ଖାଲି)
	    ଫେରନ୍ତୁ -1;

	*usedlen = entry->ମୂଲ୍ୟ.length + 1;
	ଫେରନ୍ତୁ entry->ଚାବି;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ -1;
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
    ଅକ୍ଷର	**errormsg,	// pointer to error message
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*escaped,	// return value has escaped white space (can
				// be NULL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		empty_is_error)	// empty result is considered an error
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*s;
    char_u	*result = (char_u *)"";
    char_u	*resultbuf = ଖାଲି;
    size_t	resultlen;
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		valid = VALID_HEAD + VALID_PATH;    // assume valid result
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		spec_idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tilde_file = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip_mod = FALSE;
    char_u	strbuf[30];

    *errormsg = ଖାଲି;
    ଯଦି (escaped != ଖାଲି)
	*escaped = FALSE;

    /*
     * Check if there is something to do.
     */
    spec_idx = find_cmdline_var(src, usedlen);
    ଯଦି (spec_idx < 0)	// no match
    {
	*usedlen = 1;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    /*
     * Skip when preceded with a backslash "\%" and "\#".
     * Note: In "\\%" the % is also not recognized!
     */
    ଯଦି (src > srcstart && src[-1] == '\\')
    {
	*usedlen = 0;
	STRMOVE(src - 1, src);	// remove backslash
	ଫେରନ୍ତୁ ଖାଲି;
    }

    /*
     * word or WORD under cursor
     */
    ଯଦି (spec_idx == SPEC_CWORD || spec_idx == SPEC_CCWORD
						     || spec_idx == SPEC_CEXPR)
    {
	resultlen = find_ident_under_cursor(&result,
		spec_idx == SPEC_CWORD ? (FIND_IDENT | FIND_STRING)
	      : spec_idx == SPEC_CEXPR ? (FIND_IDENT | FIND_STRING | FIND_EVAL)
	      : FIND_STRING);
	ଯଦି (resultlen == 0)
	{
	    *errormsg = "";
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }

    /*
     * '#': Alternate file name
     * '%': Current file name
     *	    File name under the cursor
     *	    File name for autocommand
     *	and following modifiers
     */
    ଅନ୍ୟଥା
    {
	size_t off = 0;

	ବାଛ (spec_idx)
	{
	ସ୍ଥିତି SPEC_PERC:
#ifdef FEAT_EVAL
		ଯଦି (!in_vim9script() || src[1] != '%')
#endif
		{
		    // '%': current file
		    ଯଦି (curbuf->b_fname == ଖାଲି)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    ଅନ୍ୟଥା
		    {
			result = curbuf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		    ଭାଙ୍ଗ;
		}
#ifdef FEAT_EVAL
		// "%%" alternate file
		off = 1;
#endif
		// FALLTHROUGH
	ସ୍ଥିତି SPEC_HASH:		// '#' or "#99": alternate file
		ଯଦି (off == 0 ? src[1] == '#' : src[2] == '%')
		{
		    // "##" or "%%%": the argument list
		    result = arg_all();
		    resultbuf = result;
		    *usedlen = off + 2;
		    ଯଦି (escaped != ଖାଲି)
			*escaped = TRUE;
		    skip_mod = TRUE;
		    ଭାଙ୍ଗ;
		}
		s = src + off + 1;
		ଯଦି (*s == '<')		// "#<99" uses v:oldfiles
		    ++s;
		i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)getdigits(&s);
		ଯଦି (s == src + off + 2 && src[off + 1] == '-')
		    // just a minus sign, don't skip over it
		    s--;
		*usedlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(s - src); // length of what we expand

		ଯଦି (src[off + 1] == '<' && i != 0)
		{
		    ଯଦି (*usedlen < off + 2)
		    {
			// Should we give an error message for #<text?
			*usedlen = off + 1;
			ଫେରନ୍ତୁ ଖାଲି;
		    }
#ifdef FEAT_EVAL
		    result = list_find_str(get_vim_var_list(VV_OLDFILES),
								     (long)i);
		    ଯଦି (result == ଖାଲି)
		    {
			*errormsg = "";
			ଫେରନ୍ତୁ ଖାଲି;
		    }
#ଅନ୍ୟଥା
		    *errormsg = _(e_hashsmall_is_not_available_without_the_eval_feature);
		    ଫେରନ୍ତୁ ଖାଲି;
#endif
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (i == 0 && src[off + 1] == '<' && *usedlen > off + 1)
			*usedlen = off + 1;
		    buf = buflist_findnr(i);
		    ଯଦି (buf == ଖାଲି)
		    {
			*errormsg = _(e_no_alternate_file_name_to_substitute_for_hash);
			ଫେରନ୍ତୁ ଖାଲି;
		    }
		    ଯଦି (lnump != ଖାଲି)
			*lnump = ECMD_LAST;
		    ଯଦି (buf->b_fname == ଖାଲି)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    ଅନ୍ୟଥା
		    {
			result = buf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି SPEC_CFILE:	// file name under cursor
		result = file_name_at_cursor(FNAME_MESS|FNAME_HYP, 1L, ଖାଲି);
		ଯଦି (result == ଖାଲି)
		{
		    *errormsg = "";
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		resultbuf = result;	    // remember allocated string
		ଭାଙ୍ଗ;

	ସ୍ଥିତି SPEC_AFILE:	// file name for autocommand
		result = autocmd_fname;
		ଯଦି (result != ଖାଲି && !autocmd_fname_full)
		{
		    // Still need to turn the fname into a full path.  It is
		    // postponed to avoid a delay when <afile> is not used.
		    autocmd_fname_full = TRUE;
		    result = FullName_save(autocmd_fname, FALSE);
		    vim_free(autocmd_fname);
		    autocmd_fname = result;
		}
		ଯଦି (result == ଖାଲି)
		{
		    *errormsg = _(e_no_autocommand_file_name_to_substitute_for_afile);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		result = shorten_fname1(result);
		ଭାଙ୍ଗ;

	ସ୍ଥିତି SPEC_ABUF:		// buffer number for autocommand
		ଯଦି (autocmd_bufnr <= 0)
		{
		    *errormsg = _(e_no_autocommand_buffer_ସଂଖ୍ୟା_to_substitute_for_abuf);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		sprintf((ଅକ୍ଷର *)strbuf, "%d", autocmd_bufnr);
		result = strbuf;
		ଭାଙ୍ଗ;

	ସ୍ଥିତି SPEC_AMATCH:	// match name for autocommand
		result = autocmd_match;
		ଯଦି (result == ଖାଲି)
		{
		    *errormsg = _(e_no_autocommand_match_name_to_substitute_for_amatch);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି SPEC_SFILE:	// file name for ":so" command
		result = estack_sfile(ESTACK_SFILE);
		ଯଦି (result == ଖାଲି)
		{
		    *errormsg = _(e_no_source_file_name_to_substitute_for_sfile);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		resultbuf = result;	    // remember allocated string
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SPEC_ଗଦା:	// call stack
		result = estack_sfile(ESTACK_ଗଦା);
		ଯଦି (result == ଖାଲି)
		{
		    *errormsg = _(e_no_call_ଗଦା_to_substitute_for_ଗଦା);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		resultbuf = result;	    // remember allocated string
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SPEC_SCRIPT:	// script file name
		result = estack_sfile(ESTACK_SCRIPT);
		ଯଦି (result == ଖାଲି)
		{
		    *errormsg = _(e_no_script_file_name_to_substitute_for_script);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		resultbuf = result;	    // remember allocated string
		ଭାଙ୍ଗ;

	ସ୍ଥିତି SPEC_SLNUM:	// line in file for ":so" command
		ଯଦି (SOURCING_NAME == ଖାଲି || SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_ଧାଡ଼ି_ସଂଖ୍ୟା_to_use_for_slnum);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		sprintf((ଅକ୍ଷର *)strbuf, "%ld", SOURCING_LNUM);
		result = strbuf;
		ଭାଙ୍ଗ;

#ifdef FEAT_EVAL
	ସ୍ଥିତି SPEC_SFLNUM:	// line in script file
		ଯଦି (current_sctx.sc_lnum + SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_ଧାଡ଼ି_ସଂଖ୍ୟା_to_use_for_sflnum);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		sprintf((ଅକ୍ଷର *)strbuf, "%ld",
				 (long)(current_sctx.sc_lnum + SOURCING_LNUM));
		result = strbuf;
		ଭାଙ୍ଗ;

	ସ୍ଥିତି SPEC_SID:
		ଯଦି (current_sctx.sc_sid <= 0)
		{
		    *errormsg = _(e_using_sid_not_in_script_context);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		sprintf((ଅକ୍ଷର *)strbuf, "<SNR>%d_", current_sctx.sc_sid);
		result = strbuf;
		ଭାଙ୍ଗ;
#endif

#ifdef FEAT_CLIENTSERVER
	ସ୍ଥିତି SPEC_CLIENT:	// Source of last submitted input
# ifdef MSWIN
		sprintf((ଅକ୍ଷର *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		result = strbuf;
# ଅନ୍ୟଥା
#  ifdef FEAT_SOCKETSERVER
		ଯଦି (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		{
		    ଯଦି (client_socket == ଖାଲି)
			result = (char_u *)"";
		    ଅନ୍ୟଥା
			result = client_socket;
		}
#  endif
#  ifdef FEAT_X11
		ଯଦି (clientserver_method == CLIENTSERVER_METHOD_X11)
		{
		    sprintf((ଅକ୍ଷର *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		    result = strbuf;
		}
#  endif
# endif
		ଭାଙ୍ଗ;
#endif
	}

	resultlen = STRLEN(result);	// length of new string
	ଯଦି (src[*usedlen] == '<')	// remove the file name extension
	{
	    ++*usedlen;
	    ଯଦି ((s = vim_strrchr(result, '.')) != ଖାଲି && s >= gettail(result))
		resultlen = s - result;
	}
	ଅନ୍ୟଥା ଯଦି (!skip_mod)
	{
	    valid |= modify_fname(src, tilde_file, usedlen, &result, &resultbuf,
								  &resultlen);
	    ଯଦି (result == ଖାଲି)
	    {
		*errormsg = "";
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	}
    }

    ଯଦି (resultlen == 0 || valid != VALID_HEAD + VALID_PATH)
    {
	ଯଦି (empty_is_error)
	{
	    ଯଦି (valid != VALID_HEAD + VALID_PATH)
		*errormsg = _(e_empty_file_name_for_percent_or_hash_only_works_with_ph);
	    ଅନ୍ୟଥା
		*errormsg = _(e_evaluates_to_an_empty_string);
	}
	result = ଖାଲି;
    }
    ଅନ୍ୟଥା
	result = vim_strnsave(result, resultlen);
    vim_free(resultbuf);
    ଫେରନ୍ତୁ result;
}

/*
 * Expand the <sfile> string in "arg".
 *
 * Returns an allocated string, or NULL for any error.
 */
    char_u *
expand_sfile(char_u *arg)
{
    ଅକ୍ଷର	*errormsg;
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
    ଯଦି (result == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (p = result; *p; )
    {
	ଯଦି (STRNCMP(p, "<sfile>", 7) != 0)
	    ++p;
	ଅନ୍ୟଥା
	{
	    // replace "<sfile>" with the sourced file name, and do ":" stuff
	    repl = eval_vars(p, result, &srclen, ଖାଲି, &errormsg, ଖାଲି, TRUE);
	    ଯଦି (errormsg != ଖାଲି)
	    {
		ଯଦି (*errormsg)
		    emsg(errormsg);
		vim_free(result);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    ଯଦି (repl == ଖାଲି)		// no match (cannot happen)
	    {
		p += srclen;
		ଜାରି;
	    }
	    repllen = STRLEN(repl);
	    resultlen += repllen - srclen;
	    newres = alloc(resultlen + 1);
	    ଯଦି (newres == ଖାଲି)
	    {
		vim_free(repl);
		vim_free(result);
		ଫେରନ୍ତୁ ଖାଲି;
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

    ଫେରନ୍ତୁ result;
}

#ଯଦି defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
/*
 * Make a dialog message in "buff[DIALOG_MSG_SIZE]".
 * "format" must contain "%s".
 */
    ଶୂନ୍ୟ
dialog_ସନ୍ଦେଶ(char_u *buff, ଅକ୍ଷର *format, char_u *fname)
{
    ଯଦି (fname == ଖାଲି)
	fname = (char_u *)_("Untitled");
    vim_snprintf((ଅକ୍ଷର *)buff, DIALOG_ସନ୍ଦେଶ_SIZE, format, fname);
}
#endif

/*
 * ":behave {mswin,xterm}"
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_behave(exarg_T *eap)
{
    ଯଦି (STRCMP(eap->arg, "mswin") == 0)
    {
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"selection",
						 0L, (char_u *)"exclusive", 0);
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"selectmode",
						 0L, (char_u *)"mouse,key", 0);
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"mousemodel",
						     0L, (char_u *)"popup", 0);
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"keymodel",
					  0L, (char_u *)"startsel,stopsel", 0);
    }
    ଅନ୍ୟଥା ଯଦି (STRCMP(eap->arg, "xterm") == 0)
    {
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"selection",
						 0L, (char_u *)"inclusive", 0);
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"selectmode", 0L, (char_u *)"", 0);
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"mousemodel",
						    0L, (char_u *)"extend", 0);
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"keymodel", 0L, (char_u *)"", 0);
    }
    ଅନ୍ୟଥା
	semsg(_(e_invalid_argument_str), eap->arg);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା filetype_detect = FALSE;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା filetype_plugin = FALSE;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା filetype_indent = FALSE;

/*
 * ":filetype [plugin] [indent] {on,off,detect}"
 * on: Load the filetype.vim file to install autocommands for file types.
 * off: Load the ftoff.vim file to remove all autocommands for file types.
 * plugin on: load filetype.vim and ftplugin.vim
 * plugin off: load ftplugof.vim
 * indent on: load filetype.vim and indent.vim
 * indent off: load indoff.vim
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_filetype(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		plugin = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		indent = FALSE;

    ଯଦି (*eap->arg == NUL)
    {
	// Print current status.
	smsg("filetype detection:%s  plugin:%s  indent:%s",
		filetype_detect ? "ON" : "OFF",
		filetype_plugin ? (filetype_detect ? "ON" : "(on)") : "OFF",
		filetype_indent ? (filetype_detect ? "ON" : "(on)") : "OFF");
	ଫେରନ୍ତୁ;
    }

    // Accept "plugin" and "indent" in any order.
    ପାଇଁ (;;)
    {
	ଯଦି (STRNCMP(arg, "plugin", 6) == 0)
	{
	    plugin = TRUE;
	    arg = skipwhite(arg + 6);
	    ଜାରି;
	}
	ଯଦି (STRNCMP(arg, "indent", 6) == 0)
	{
	    indent = TRUE;
	    arg = skipwhite(arg + 6);
	    ଜାରି;
	}
	ଭାଙ୍ଗ;
    }
    ଯଦି (STRCMP(arg, "on") == 0 || STRCMP(arg, "detect") == 0)
    {
	ଯଦି (*arg == 'o' || !filetype_detect)
	{
	    source_runtime((char_u *)FILETYPE_FILE, DIP_ALL);
	    filetype_detect = TRUE;
	    ଯଦି (plugin)
	    {
		source_runtime((char_u *)FTPLUGIN_FILE, DIP_ALL);
		filetype_plugin = TRUE;
	    }
	    ଯଦି (indent)
	    {
		source_runtime((char_u *)INDENT_FILE, DIP_ALL);
		filetype_indent = TRUE;
	    }
	}
	ଯଦି (*arg == 'd')
	{
	    (ଶୂନ୍ୟ)do_doautocmd((char_u *)"filetypedetect BufRead", TRUE, ଖାଲି);
	    do_modelines(0);
	}
    }
    ଅନ୍ୟଥା ଯଦି (STRCMP(arg, "off") == 0)
    {
	ଯଦି (plugin || indent)
	{
	    ଯଦି (plugin)
	    {
		source_runtime((char_u *)FTPLUGOF_FILE, DIP_ALL);
		filetype_plugin = FALSE;
	    }
	    ଯଦି (indent)
	    {
		source_runtime((char_u *)INDOFF_FILE, DIP_ALL);
		filetype_indent = FALSE;
	    }
	}
	ଅନ୍ୟଥା
	{
	    source_runtime((char_u *)FTOFF_FILE, DIP_ALL);
	    filetype_detect = FALSE;
	}
    }
    ଅନ୍ୟଥା
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * ":setfiletype [FALLBACK] {name}"
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ex_setfiletype(exarg_T *eap)
{
    ଯଦି (curbuf->b_did_filetype)
	ଫେରନ୍ତୁ;

    char_u *arg = eap->arg;
    ଯଦି (STRNCMP(arg, "FALLBACK ", 9) == 0)
	arg += 9;

    set_option_ମୂଲ୍ୟ_give_err((char_u *)"filetype", 0L, arg, OPT_LOCAL);
    ଯଦି (arg != eap->arg)
	curbuf->b_did_filetype = false;
}

    ସ୍ଥିର ଶୂନ୍ୟ
ex_digraphs(exarg_T *eap UNUSED)
{
#ifdef FEAT_DIGRAPHS
    ଯଦି (*eap->arg != NUL)
	putdigraph(eap->arg);
    ଅନ୍ୟଥା
	listdigraphs(eap->forceit);
#ଅନ୍ୟଥା
    emsg(_(e_no_digraphs_version));
#endif
}

#ଯଦି defined(FEAT_ସନ୍ଧାନ_EXTRA)
    ଶୂନ୍ୟ
set_no_hlsearch(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag)
{
    no_hlsearch = flag;
# ifdef FEAT_EVAL
    set_vim_var_nr(VV_HLSEARCH, !no_hlsearch && p_hls);
# endif
}

/*
 * ":nohlsearch"
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ଶୂନ୍ୟ
ex_X(exarg_T *eap UNUSED)
{
    crypt_check_current_method();
    (ଶୂନ୍ୟ)crypt_get_ଚାବି(TRUE, TRUE);
}
#endif

#ifdef FEAT_FOLDING
    ସ୍ଥିର ଶୂନ୍ୟ
ex_fold(exarg_T *eap)
{
    ଯଦି (foldManualAllowed(TRUE))
	foldCreate(eap->line1, eap->line2);
}

    ସ୍ଥିର ଶୂନ୍ୟ
ex_foldopen(exarg_T *eap)
{
    opFoldRange(eap->line1, eap->line2, eap->cmdidx == CMD_foldopen,
							 eap->forceit, FALSE);
}

    ସ୍ଥିର ଶୂନ୍ୟ
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
    ପାଇଁ (lnum = eap->line1; lnum <= eap->line2; ++lnum)
	ଯଦି (hasFolding(lnum, ଖାଲି, ଖାଲି) == (eap->cmdidx == CMD_folddoclosed))
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

#ଯଦି defined(FEAT_QUICKFIX)
/*
 * Returns TRUE if the supplied Ex cmdidx is for a location list command
 * instead of a quickfix command.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_loclist_cmd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmdidx)
{
    ଯଦି (cmdidx < 0 || cmdidx >= CMD_SIZE)
	ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ cmdnames[cmdidx].cmd_name[0] == 'l';
}
#endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_pressedreturn(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ ex_pressedreturn;
}

    ଶୂନ୍ୟ
set_pressedreturn(ପୂର୍ଣ୍ଣସଂଖ୍ୟା val)
{
    ex_pressedreturn = val;
}
