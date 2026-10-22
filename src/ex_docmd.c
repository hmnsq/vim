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

#شامل "vim.h"

ساکن صحیح	quitmore = 0;
ساکن صحیح	ex_pressedreturn = FALSE;
#اگر_غیر_تعریف_شدہ FEAT_PRINTER
# define ex_hardcopy	ex_ni
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
ساکن char_u	*do_one_cmd(char_u **, صحیح, cstack_T *, char_u *(*fgetline)(صحیح, خالی *, صحیح, getline_opt_T), خالی *cookie);
#ورنہ
ساکن char_u	*do_one_cmd(char_u **, صحیح, char_u *(*fgetline)(صحیح, خالی *, صحیح, getline_opt_T), خالی *cookie);
ساکن صحیح	if_level = 0;		// depth in :if
#اگر_ختم
ساکن خالی	append_command(char_u *cmd);

#اگر_غیر_تعریف_شدہ FEAT_MENU
# define ex_emenu		ex_ni
# define ex_menu		ex_ni
# define ex_menutranslate	ex_ni
#اگر_ختم
ساکن خالی	ex_autocmd(exarg_T *eap);
ساکن خالی	ex_doautocmd(exarg_T *eap);
ساکن خالی	ex_bunload(exarg_T *eap);
ساکن خالی	ex_بفر(exarg_T *eap);
ساکن خالی	do_exbuffer(exarg_T *eap);
ساکن خالی	ex_bmodified(exarg_T *eap);
ساکن خالی	ex_bnext(exarg_T *eap);
ساکن خالی	ex_bprevious(exarg_T *eap);
ساکن خالی	ex_brewind(exarg_T *eap);
ساکن خالی	ex_blast(exarg_T *eap);
ساکن char_u	*getargcmd(char_u **);
ساکن صحیح	getargopt(exarg_T *eap);
#اگر_غیر_تعریف_شدہ FEAT_QUICKFIX
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
#اگر_ختم
#اگر !defined(FEAT_QUICKFIX) || !defined(FEAT_EVAL)
# define ex_cexpr		ex_ni
#اگر_ختم

ساکن linenr_T default_پتہ(exarg_T *eap);
ساکن خالی پتہ_default_all(exarg_T *eap);
ساکن خالی	get_flags(exarg_T *eap);
#اگر !defined(FEAT_PERL) \
	|| !defined(FEAT_PYTHON) || !defined(FEAT_PYTHON3) \
	|| !defined(FEAT_TCL) \
	|| !defined(FEAT_RUBY) \
	|| !defined(FEAT_LUA) \
	|| !defined(FEAT_MZSCHEME)
# define HAVE_EX_SCRIPT_NI
ساکن خالی	ex_script_ni(exarg_T *eap);
#اگر_ختم
ساکن حرف	*invalid_range(exarg_T *eap);
ساکن خالی	correct_range(exarg_T *eap);
#اگر_تعریف_شدہ FEAT_QUICKFIX
ساکن char_u	*replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep);
#اگر_ختم
ساکن char_u	*repl_cmdline(exarg_T *eap, char_u *src, size_t srclen, char_u *repl, char_u **cmdlinep);
ساکن خالی	ex_highlight(exarg_T *eap);
ساکن خالی	ex_colorscheme(exarg_T *eap);
ساکن خالی	ex_cquit(exarg_T *eap);
ساکن خالی	ex_quit_all(exarg_T *eap);
ساکن خالی	ex_close(exarg_T *eap);
ساکن خالی	ex_win_close(صحیح forceit, win_T *win, tabpage_T *tp);
ساکن خالی	ex_only(exarg_T *eap);
ساکن خالی	ex_resize(exarg_T *eap);
ساکن خالی	ex_stag(exarg_T *eap);
ساکن خالی	ex_tabclose(exarg_T *eap);
ساکن خالی	ex_tabonly(exarg_T *eap);
ساکن خالی	ex_tabnext(exarg_T *eap);
ساکن خالی	ex_tabmove(exarg_T *eap);
ساکن خالی	ex_tabs(exarg_T *eap);
#اگر defined(FEAT_QUICKFIX)
ساکن خالی	ex_pclose(exarg_T *eap);
ساکن خالی	ex_ptag(exarg_T *eap);
ساکن خالی	ex_pedit(exarg_T *eap);
ساکن خالی	ex_pbuffer(exarg_T *eap);
ساکن خالی	prepare_preview_کھڑکی(خالی);
ساکن خالی	عقب_مقصد_current_کھڑکی(win_T *curwin_save);
#ورنہ
# define ex_pclose		ex_ni
# define ex_ptag		ex_ni
# define ex_pedit		ex_ni
# define ex_pbuffer		ex_ni
#اگر_ختم
ساکن خالی	ex_hide(exarg_T *eap);
ساکن خالی	ex_exit(exarg_T *eap);
ساکن خالی	ex_print(exarg_T *eap);
#اگر_تعریف_شدہ FEAT_BYTEOFF
ساکن خالی	ex_goto(exarg_T *eap);
#ورنہ
# define ex_goto		ex_ni
#اگر_ختم
ساکن خالی	ex_shell(exarg_T *eap);
ساکن خالی	ex_preserve(exarg_T *eap);
ساکن خالی	ex_recover(exarg_T *eap);
ساکن خالی	ex_منوال(exarg_T *eap);
ساکن خالی	ex_wrongmodifier(exarg_T *eap);
ساکن خالی	ex_ڈھونڈو(exarg_T *eap);
ساکن خالی	ex_open(exarg_T *eap);
ساکن خالی	ex_edit(exarg_T *eap);
#اگر_غیر_تعریف_شدہ FEAT_GUI
# define ex_gui			ex_nogui
ساکن خالی	ex_nogui(exarg_T *eap);
#اگر_ختم
#اگر defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
ساکن خالی	ex_tearoff(exarg_T *eap);
#ورنہ
# define ex_tearoff		ex_ni
#اگر_ختم
#اگر (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_رکن_POPUP_MENU)) && defined(FEAT_MENU)
ساکن خالی	ex_popup(exarg_T *eap);
#ورنہ
# define ex_popup		ex_ni
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_GUI_MSWIN
# define ex_simalt		ex_ni
#اگر_ختم
#اگر !defined(FEAT_GUI_MSWIN) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
# define gui_mch_ڈھونڈو_dialog	ex_ni
# define gui_mch_replace_dialog ex_ni
#اگر_ختم
#اگر !defined(FEAT_GUI_GTK)
# define ex_helpfind		ex_ni
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_CSCOPE
# define ex_cscope		ex_ni
# define ex_scscope		ex_ni
# define ex_cstag		ex_ni
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_SYN_HL
# define ex_syntax		ex_ni
# define ex_ownsyntax		ex_ni
#اگر_ختم
#اگر !defined(FEAT_SYN_HL) || !defined(FEAT_PROFILE)
# define ex_syntime		ex_ni
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_SPELL
# define ex_spell		ex_ni
# define ex_mkspell		ex_ni
# define ex_spelldump		ex_ni
# define ex_spellinfo		ex_ni
# define ex_spellrepall		ex_ni
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_PERSISTENT_UNDO
# define ex_rundo		ex_ni
# define ex_wundo		ex_ni
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_LUA
# define ex_lua			ex_script_ni
# define ex_luado		ex_ni
# define ex_luafile		ex_ni
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_MZSCHEME
# define ex_mzscheme		ex_script_ni
# define ex_mzfile		ex_ni
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_PERL
# define ex_perl		ex_script_ni
# define ex_perldo		ex_ni
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_PYTHON
# define ex_python		ex_script_ni
# define ex_pydo		ex_ni
# define ex_pyfile		ex_ni
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_PYTHON3
# define ex_py3			ex_script_ni
# define ex_py3do		ex_ni
# define ex_py3file		ex_ni
#اگر_ختم
#اگر !defined(FEAT_PYTHON) && !defined(FEAT_PYTHON3)
# define ex_pyx			ex_script_ni
# define ex_pyxdo		ex_ni
# define ex_pyxfile		ex_ni
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_TCL
# define ex_tcl			ex_script_ni
# define ex_tcldo		ex_ni
# define ex_tclfile		ex_ni
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_RUBY
# define ex_ruby		ex_script_ni
# define ex_rubydo		ex_ni
# define ex_rubyfile		ex_ni
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_KEYMAP
# define ex_loadkeymap		ex_ni
#اگر_ختم
ساکن خالی	ex_swapname(exarg_T *eap);
ساکن خالی	ex_syncbind(exarg_T *eap);
ساکن خالی	ex_read(exarg_T *eap);
ساکن خالی	ex_pwd(exarg_T *eap);
ساکن خالی	ex_equal(exarg_T *eap);
ساکن خالی	ex_sleep(exarg_T *eap);
ساکن خالی	ex_winsize(exarg_T *eap);
ساکن خالی	ex_wincmd(exarg_T *eap);
#اگر defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
ساکن خالی	ex_winpos(exarg_T *eap);
#ورنہ
# define ex_winpos	    ex_ni
#اگر_ختم
ساکن خالی	ex_operators(exarg_T *eap);
ساکن خالی	ex_put(exarg_T *eap);
ساکن خالی	ex_iput(exarg_T *eap);
ساکن خالی	ex_copymove(exarg_T *eap);
ساکن خالی	ex_submagic(exarg_T *eap);
ساکن خالی	ex_join(exarg_T *eap);
ساکن خالی	ex_at(exarg_T *eap);
ساکن خالی	ex_bang(exarg_T *eap);
ساکن خالی	ex_undo(exarg_T *eap);
#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
ساکن خالی	ex_wundo(exarg_T *eap);
ساکن خالی	ex_rundo(exarg_T *eap);
#اگر_ختم
ساکن خالی	ex_redo(exarg_T *eap);
ساکن خالی	ex_later(exarg_T *eap);
ساکن خالی	ex_redir(exarg_T *eap);
ساکن خالی	ex_redrawstatus(exarg_T *eap);
ساکن خالی	ex_redrawtabline(exarg_T *eap);
ساکن خالی	ex_redrawtabpanel(exarg_T *eap);
ساکن خالی	close_redir(خالی);
ساکن خالی	ex_mark(exarg_T *eap);
ساکن خالی	ex_startinsert(exarg_T *eap);
ساکن خالی	ex_stopinsert(exarg_T *eap);
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
ساکن خالی	ex_checkpath(exarg_T *eap);
ساکن خالی	ex_findpat(exarg_T *eap);
#ورنہ
# define ex_findpat		ex_ni
# define ex_checkpath		ex_ni
#اگر_ختم
#اگر defined(FEAT_ڈھونڈو_ID) && defined(FEAT_QUICKFIX)
ساکن خالی	ex_psearch(exarg_T *eap);
#ورنہ
# define ex_psearch		ex_ni
#اگر_ختم
ساکن خالی	ex_لیبل(exarg_T *eap);
ساکن خالی	ex_لیبل_cmd(exarg_T *eap, char_u *name);
#اگر_غیر_تعریف_شدہ FEAT_EVAL
# define ex_بلاک		ex_ni
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
# define ex_اختتام		ex_ni
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
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_SESSION
# define ex_loadview		ex_ni
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_VIMINFO
# define ex_viminfo		ex_ni
#اگر_ختم
ساکن خالی	ex_behave(exarg_T *eap);
ساکن خالی	ex_filetype(exarg_T *eap);
ساکن خالی	ex_setfiletype(exarg_T *eap);
#اگر_غیر_تعریف_شدہ FEAT_DIFF
# define ex_diffoff		ex_ni
# define ex_diffpatch		ex_ni
# define ex_diffgetput		ex_ni
# define ex_diffsplit		ex_ni
# define ex_diffthis		ex_ni
# define ex_diffupdate		ex_ni
#اگر_ختم
ساکن خالی	ex_digraphs(exarg_T *eap);
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
ساکن خالی	ex_nohlsearch(exarg_T *eap);
#ورنہ
# define ex_nohlsearch		ex_ni
# define ex_مطابقت		ex_ni
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CRYPT
ساکن خالی	ex_X(exarg_T *eap);
#ورنہ
# define ex_X			ex_ni
#اگر_ختم
#اگر_تعریف_شدہ FEAT_FOLDING
ساکن خالی	ex_fold(exarg_T *eap);
ساکن خالی	ex_foldopen(exarg_T *eap);
ساکن خالی	ex_folddo(exarg_T *eap);
#ورنہ
# define ex_fold		ex_ni
# define ex_foldopen		ex_ni
# define ex_folddo		ex_ni
#اگر_ختم
#اگر !(defined(HAVE_LOCALE_H) || defined(X_LOCALE))
# define ex_language		ex_ni
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_SIGNS
# define ex_sign		ex_ni
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_NETBEANS_INTG
# define ex_nbclose		ex_ni
# define ex_nbkey		ex_ni
# define ex_nbstart		ex_ni
#اگر_ختم

#اگر_غیر_تعریف_شدہ FEAT_PROFILE
# define ex_profile		ex_ni
#اگر_ختم
#اگر_غیر_تعریف_شدہ FEAT_TERMINAL
# define ex_terminal		ex_ni
#اگر_ختم
#اگر !defined(FEAT_X11) || !defined(FEAT_XCLIPBOARD)
# define ex_xrestore		ex_ni
#اگر_ختم
#اگر !defined(FEAT_WAYLAND)
# define ex_wlrestore		ex_ni
#اگر_ختم
#اگر !defined(HAVE_CLIPMETHOD)
# define ex_clipreset		ex_ni
#اگر_ختم
#اگر !defined(FEAT_PROP_POPUP)
# define ex_popupclear		ex_ni
#اگر_ختم

/*
 * Declare cmdnames[].
 */
#تعریف_کرو DO_DECLARE_EXCMD
#شامل "ex_cmds.h"
#شامل "ex_cmdidxs.h"

ساکن char_u dollar_command[2] = {'$', 0};


#اگر_تعریف_شدہ FEAT_EVAL
// Struct for storing a line inside a while/for loop
قسمتعریف ساخت
{
    char_u	*سطر;		// command line
    linenr_T	lnum;		// sourcing_lnum of the line
} wcmd_T;

/*
 * Structure used to store info for line position in a while or for loop.
 * This is required, because do_one_cmd() may invoke ex_function(), which
 * reads more lines that may come from the while/for loop.
 */
ساخت loop_cookie
{
    garray_T	*سطریں_gap;		// growarray with line info
    صحیح		current_سطر;		// last read line from growarray
    صحیح		repeating;		// TRUE when looping a second time
    // When "repeating" is FALSE use "getline" and "cookie" to get lines
    char_u	*(*lc_getline)(صحیح, خالی *, صحیح, getline_opt_T);
    خالی	*cookie;
};

ساکن char_u	*get_loop_سطر(صحیح c, خالی *cookie, صحیح indent, getline_opt_T options);
ساکن صحیح	store_loop_سطر(garray_T *gap, char_u *سطر);
ساکن خالی	free_cmdlines(garray_T *gap);

// Struct to save a few things while debugging.  Used in do_cmdline() only.
ساخت dbg_stuff
{
    صحیح		trylevel;
    صحیح		force_abort;
    except_T	*caught_ڈھیر;
    char_u	*vv_exception;
    char_u	*vv_throwpoint;
    صحیح		did_emsg;
    صحیح		got_int;
    صحیح		did_throw;
    صحیح		need_rethrow;
    صحیح		check_cstack;
    except_T	*current_exception;
};

    ساکن خالی
save_dbg_stuff(ساخت dbg_stuff *dsp)
{
    dsp->trylevel	= trylevel;		trylevel = 0;
    dsp->force_abort	= force_abort;		force_abort = FALSE;
    dsp->caught_ڈھیر	= caught_ڈھیر;		caught_ڈھیر = تہی;
    dsp->vv_exception	= v_exception(تہی);
    dsp->vv_throwpoint	= v_throwpoint(تہی);

    // Necessary for debugging an inactive ":catch", ":finally", ":endtry"
    dsp->did_emsg	= did_emsg;		did_emsg     = FALSE;
    dsp->got_int	= got_int;		got_int	     = FALSE;
    dsp->did_throw	= did_throw;		did_throw    = FALSE;
    dsp->need_rethrow	= need_rethrow;		need_rethrow = FALSE;
    dsp->check_cstack	= check_cstack;		check_cstack = FALSE;
    dsp->current_exception = current_exception;	current_exception = تہی;
}

    ساکن خالی
restore_dbg_stuff(ساخت dbg_stuff *dsp)
{
    suppress_errthrow = FALSE;
    trylevel = dsp->trylevel;
    force_abort = dsp->force_abort;
    caught_ڈھیر = dsp->caught_ڈھیر;
    (خالی)v_exception(dsp->vv_exception);
    (خالی)v_throwpoint(dsp->vv_throwpoint);
    did_emsg = dsp->did_emsg;
    got_int = dsp->got_int;
    did_throw = dsp->did_throw;
    need_rethrow = dsp->need_rethrow;
    check_cstack = dsp->check_cstack;
    current_exception = dsp->current_exception;
}
#اگر_ختم

/*
 * Check if ffname differs from fnum.
 * fnum is a buffer number. 0 == current buffer, 1-or-more must be a valid
 * buffer ID.
 * ffname is a full path to where a buffer lives on-disk or would live on-disk.
 *
 */
    ساکن صحیح
is_other_file(صحیح fnum, char_u *ffname)
{
    اگر (fnum != 0)
    {
	اگر (fnum == curbuf->b_fnum)
	    واپس FALSE;

	واپس TRUE;
    }

    اگر (ffname == تہی)
	واپس TRUE;

    اگر (*ffname == NUL)
	واپس FALSE;

    // TODO: Need a reliable way to know whether a buffer is meant to live
    // on-disk !curbuf->b_dev_valid is not always available (example: missing
    // on Windows)
    اگر (curbuf->b_sfname != تہی
	    && *curbuf->b_sfname != NUL)
	// This occurs with unsaved buffers. In which case `ffname` actually
	// corresponds to curbuf->b_sfname
	واپس fnamecmp(ffname, curbuf->b_sfname) != 0;

    واپس otherfile(ffname);
}

/*
 * do_exmode(): Repeatedly get commands for the "Ex" mode, until the ":vi"
 * command is given.
 */
    خالی
do_exmode(
    صحیح		improved)	    // TRUE for "improved Ex" mode
{
    صحیح		save_پیغام_scroll;
    صحیح		پچھلا_پیغام_صف;
    linenr_T	پچھلا_سطر;
    varnumber_T	changedtick;

    اگر (improved)
	exmode_فعال = EXMODE_VIM;
    ورنہ
	exmode_فعال = EXMODE_معمول;
    State = منوال_معمول;
    may_trigger_modechanged();

    // When using ":global /pat/ visual" and then "Q" we return to continue
    // the :global command.
    اگر (global_busy)
	واپس;

    save_پیغام_scroll = پیغام_scroll;
    ++RedrawingDisabled;	    // don't redisplay the window
    ++no_wait_return;		    // don't wait for return
#اگر_تعریف_شدہ FEAT_GUI
    // Ignore scrollbar and mouse events in Ex mode
    ++hold_gui_events;
#اگر_ختم

    پیغام(_("Entering Ex mode.  Type \"visual\" to go to Normal mode."));
    جب (exmode_فعال)
    {
	// Check for a ":normal" command and no more characters left.
	اگر (ex_معمول_busy > 0 && typebuf.tb_len == 0)
	{
	    exmode_فعال = FALSE;
	    توڑو;
	}
	پیغام_scroll = TRUE;
	need_wait_return = FALSE;
	ex_pressedreturn = FALSE;
	ex_no_reprint = FALSE;
	changedtick = CHANGEDTICK(curbuf);
	پچھلا_پیغام_صف = پیغام_صف;
	پچھلا_سطر = curwin->w_cursor.lnum;
	اگر (improved)
	{
	    cmdline_صف = پیغام_صف;
	    do_cmdline(تہی, getexline, تہی, 0);
	}
	ورنہ
	    do_cmdline(تہی, getexmodeline, تہی, DOCMD_NOWAIT);
	سطریں_بایاں = صفیں - 1;

	اگر ((پچھلا_سطر != curwin->w_cursor.lnum
		   || changedtick != CHANGEDTICK(curbuf)) && !ex_no_reprint)
	{
	    اگر (curbuf->b_ml.ml_flags & ML_خالی_خانے)
		emsg(_(e_خالی_خانے_بفر));
	    ورنہ
	    {
		اگر (ex_pressedreturn)
		{
		    // go up one line, to overwrite the ":<CR>" line, so the
		    // output doesn't contain empty lines.
		    پیغام_صف = پچھلا_پیغام_صف;
		    اگر (پچھلا_پیغام_صف == صفیں - 1)
			پیغام_صف--;
		}
		پیغام_ستون = 0;
		print_سطر_no_سابقہ(curwin->w_cursor.lnum, FALSE, FALSE);
		پیغام_clr_eos();
	    }
	}
	ورنہ اگر (ex_pressedreturn && !ex_no_reprint)	// must be at EOF
	{
	    اگر (curbuf->b_ml.ml_flags & ML_خالی_خانے)
		emsg(_(e_خالی_خانے_بفر));
	    ورنہ
		emsg(_(e_at_انتہا_کا_file));
	}
    }

#اگر_تعریف_شدہ FEAT_GUI
    --hold_gui_events;
#اگر_ختم
    اگر (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    update_screen(UPD_CLEAR);
    need_wait_return = FALSE;
    پیغام_scroll = save_پیغام_scroll;
}

/*
 * Print the executed command for when 'verbose' is set.
 * When "lnum" is 0 only print the command.
 */
    ساکن خالی
پیغام_verbose_cmd(linenr_T lnum, char_u *cmd)
{
    ++no_wait_return;
    verbose_enter_scroll();

    اگر (lnum == 0)
	smsg(_("Executing: %s"), cmd);
    ورنہ
	smsg(_("line %ld: %s"), (طویل)lnum, cmd);
    اگر (پیغام_silent == 0)
	پیغام_puts("\n");   // don't overwrite this

    verbose_leave_scroll();
    --no_wait_return;
}

/*
 * Execute a simple command line.  Used for translated commands like "*".
 */
    صحیح
do_cmdline_cmd(char_u *cmd)
{
    واپس do_cmdline(cmd, تہی, تہی,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
}

/*
 * Execute the "+cmd" argument of "edit +cmd fname" and the like.
 * This allows for using a range without ":" in Vim9 script.
 */
    ساکن صحیح
do_cmd_دلیل(char_u *cmd)
{
    واپس do_cmdline(cmd, تہی, تہی,
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
    صحیح
do_cmdline(
    char_u	*cmdline,
    char_u	*(*fgetline)(صحیح, خالی *, صحیح, getline_opt_T),
    خالی	*cookie,		// argument for fgetline()
    صحیح		flags)
{
    char_u	*اگلا_cmdline;		// next cmd to execute
    char_u	*cmdline_copy = تہی;	// copy of cmd line
    صحیح		مستعمل_getline = FALSE;	// used "fgetline" to obtain command
    ساکن صحیح	recursive = 0;		// recursive depth
    صحیح		پیغام_didout_before_آغاز = 0;
    صحیح		گنتی = 0;		// line number count
    صحیح		did_inc_RedrawingDisabled = FALSE;
    صحیح		retval = OK;
#اگر_تعریف_شدہ FEAT_EVAL
    cstack_T	cstack;			// conditional stack
    garray_T	سطریں_ga;		// keep lines for ":while"/":for"
    صحیح		current_سطر = 0;	// active line in lines_ga
    صحیح		current_سطر_before = 0;
    char_u	*fname = تہی;		// function or script name
    linenr_T	*breakpoint = تہی;	// ptr to breakpoint field in cookie
    صحیح		*dbg_tick = تہی;	// ptr to dbg_tick field in cookie
    ساخت dbg_stuff debug_saved;	// saved things for debug mode
    صحیح		initial_trylevel;
    msglist_T	**saved_پیغام_list = تہی;
    msglist_T	*نجی_پیغام_list = تہی;

    // "fgetline" and "cookie" passed to do_one_cmd()
    char_u	*(*cmd_getline)(صحیح, خالی *, صحیح, getline_opt_T);
    خالی	*cmd_cookie;
    ساخت loop_cookie cmd_loop_cookie;
    خالی	*real_cookie;
    صحیح		getline_is_func;
#ورنہ
# define cmd_getline fgetline
# define cmd_cookie cookie
#اگر_ختم
    ساکن صحیح	call_گہرائی = 0;		// recursiveness
#اگر_تعریف_شدہ FEAT_EVAL
    // For every pair of do_cmdline()/do_one_cmd() calls, use an extra memory
    // location for storing error messages to be converted to an exception.
    // This ensures that the do_errthrow() call in do_one_cmd() does not
    // combine the messages stored by an earlier invocation of do_one_cmd()
    // with the command name of the later one.  This would happen when
    // BufWritePost autocommands are executed after a write error.
    saved_پیغام_list = پیغام_list;
    پیغام_list = &نجی_پیغام_list;
#اگر_ختم

    // It's possible to create an endless loop with ":execute", catch that
    // here.  The value of 200 allows nested function calls, ":source", etc.
    // Allow 200 or 'maxfuncdepth', whatever is larger.
    اگر (call_گہرائی >= 200
#اگر_تعریف_شدہ FEAT_EVAL
	    && call_گہرائی >= p_mfd
#اگر_ختم
	    )
    {
	emsg(_(e_command_too_recursive));
#اگر_تعریف_شدہ FEAT_EVAL
	// When converting to an exception, we do not include the command name
	// since this is not an error of the specific command.
	do_errthrow((cstack_T *)تہی, (char_u *)تہی);
	پیغام_list = saved_پیغام_list;
#اگر_ختم
	واپس FAIL;
    }
    ++call_گہرائی;

#اگر_تعریف_شدہ FEAT_EVAL
    CLEAR_میدان(cstack);
    cstack.cs_idx = -1;
    ga_init2(&سطریں_ga, حجم(wcmd_T), 10);

    real_cookie = getline_cookie(fgetline, cookie);

    // Inside a function use a higher nesting level.
    getline_is_func = getline_equal(fgetline, cookie, get_func_سطر);
    اگر (getline_is_func && ex_nesting_level == func_level(real_cookie))
	++ex_nesting_level;

    // Get the function or script name and the address where the next breakpoint
    // line and the debug tick for a function or script are stored.
    اگر (getline_is_func)
    {
	fname = func_name(real_cookie);
	breakpoint = func_breakpoint(real_cookie);
	dbg_tick = func_dbg_tick(real_cookie);
    }
    ورنہ اگر (getline_equal(fgetline, cookie, getsourceline))
    {
	fname = SOURCING_NAME;
	breakpoint = ماخذ_breakpoint(real_cookie);
	dbg_tick = ماخذ_dbg_tick(real_cookie);
    }

    /*
     * Initialize "force_abort"  and "suppress_errthrow" at the top level.
     */
    اگر (!recursive)
    {
	force_abort = FALSE;
	suppress_errthrow = FALSE;
    }

    /*
     * If requested, store and reset the global values controlling the
     * exception handling (used when debugging).  Otherwise clear it to avoid
     * a bogus compiler warning when the optimizer uses inline functions...
     */
    اگر (flags & DOCMD_EXCRESET)
	save_dbg_stuff(&debug_saved);
    ورنہ
	CLEAR_میدان(debug_saved);

    initial_trylevel = trylevel;

    /*
     * "did_throw" will be set to TRUE when an exception is being thrown.
     */
    did_throw = FALSE;
#اگر_ختم
    /*
     * "did_emsg" will be set to TRUE when emsg() is used, in which case we
     * cancel the whole command line, and any if/endif or loop.
     * If force_abort is set, we cancel everything.
     */
#اگر_تعریف_شدہ FEAT_EVAL
    did_emsg_cumul += did_emsg;
#اگر_ختم
    did_emsg = FALSE;

    /*
     * KeyTyped is only set when calling vgetc().  Reset it here when not
     * calling vgetc() (sourced command lines).
     */
    اگر (!(flags & DOCMD_KEYTYPED)
			       && !getline_equal(fgetline, cookie, getexline))
	کلید_Typed = FALSE;

    /*
     * Continue executing command lines:
     * - when inside an ":if", ":while" or ":for"
     * - for multiple commands on one line, separated with '|'
     * - when repeating until there are no more lines (for ":source")
     */
    اگلا_cmdline = cmdline;
    کرو
    {
#اگر_تعریف_شدہ FEAT_EVAL
	getline_is_func = getline_equal(fgetline, cookie, get_func_سطر);
#اگر_ختم

	// stop skipping cmds for an error msg after all endif/while/for
	اگر (اگلا_cmdline == تہی
#اگر_تعریف_شدہ FEAT_EVAL
		&& !force_abort
		&& cstack.cs_idx < 0
		&& !(getline_is_func && func_has_abort(real_cookie))
#اگر_ختم
							)
	{
#اگر_تعریف_شدہ FEAT_EVAL
	    did_emsg_cumul += did_emsg;
#اگر_ختم
	    did_emsg = FALSE;
	}

	/*
	 * 1. If repeating a line in a loop, get a line from lines_ga.
	 * 2. If no line given: Get an allocated line with fgetline().
	 * 3. If a line is given: Make a copy, so we can mess with it.
	 */

#اگر_تعریف_شدہ FEAT_EVAL
	// 1. If repeating, get a previous line from lines_ga.
	اگر (cstack.cs_looplevel > 0 && current_سطر < سطریں_ga.ga_len)
	{
	    // Each '|' separated command is stored separately in lines_ga, to
	    // be able to jump to it.  Don't use next_cmdline now.
	    VIM_CLEAR(cmdline_copy);

	    // Check if a function has returned or, unless it has an unclosed
	    // try conditional, aborted.
	    اگر (getline_is_func)
	    {
# ifdef FEAT_PROFILE
		اگر (do_profiling == PROF_YES)
		    func_سطر_انتہا(real_cookie);
# endif
		اگر (func_has_ended(real_cookie))
		{
		    retval = FAIL;
		    توڑو;
		}
	    }
# ifdef FEAT_PROFILE
	    ورنہ اگر (do_profiling == PROF_YES
			    && getline_equal(fgetline, cookie, getsourceline))
		script_سطر_انتہا();
# endif

	    // Check if a sourced file hit a ":finish" command.
	    اگر (ماخذ_ختم_شدہ(fgetline, cookie))
	    {
		retval = FAIL;
		توڑو;
	    }

	    // If breakpoints have been added/deleted need to check for it.
	    اگر (breakpoint != تہی && dbg_tick != تہی
						   && *dbg_tick != debug_tick)
	    {
		*breakpoint = dbg_ڈھونڈو_breakpoint(
				getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }

	    اگلا_cmdline = ((wcmd_T *)(سطریں_ga.ga_ڈیٹا))[current_سطر].سطر;
	    SOURCING_LNUM = ((wcmd_T *)(سطریں_ga.ga_ڈیٹا))[current_سطر].lnum;

	    // Did we encounter a breakpoint?
	    اگر (breakpoint != تہی && *breakpoint != 0
					      && *breakpoint <= SOURCING_LNUM)
	    {
		dbg_breakpoint(fname, SOURCING_LNUM);
		// Find next breakpoint.
		*breakpoint = dbg_ڈھونڈو_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }
# ifdef FEAT_PROFILE
	    اگر (do_profiling == PROF_YES)
	    {
		اگر (getline_is_func)
		    func_سطر_آغاز(real_cookie, SOURCING_LNUM);
		ورنہ اگر (getline_equal(fgetline, cookie, getsourceline))
		    script_سطر_آغاز();
	    }
# endif
	}
#اگر_ختم

	// 2. If no line given, get an allocated line with fgetline().
	اگر (اگلا_cmdline == تہی)
	{
	    /*
	     * Need to set msg_didout for the first line after an ":if",
	     * otherwise the ":if" will be overwritten.
	     */
	    اگر (گنتی == 1 && getline_equal(fgetline, cookie, getexline))
		پیغام_didout = TRUE;
	    اگر (fgetline == تہی || (اگلا_cmdline = fgetline(':', cookie,
#اگر_تعریف_شدہ FEAT_EVAL
		    cstack.cs_idx < 0 ? 0 : (cstack.cs_idx + 1) * 2
#ورنہ
		    0
#اگر_ختم
		    , اندر_vim9script() ? GETLINE_CONCAT_CONTBAR
					       : GETLINE_CONCAT_CONT)) == تہی)
	    {
		// Don't call wait_return() for aborted command line.  The NULL
		// returned for the end of a sourced file or executed function
		// doesn't do this.
		اگر (کلید_Typed && !(flags & DOCMD_REPEAT))
		    need_wait_return = FALSE;
		retval = FAIL;
		توڑو;
	    }
	    مستعمل_getline = TRUE;

	    /*
	     * Keep the first typed line.  Clear it when more lines are typed.
	     */
	    اگر (flags & DOCMD_KEEPLINE)
	    {
		vim_free(repeat_cmdline);
		اگر (گنتی == 0)
		    repeat_cmdline = vim_strsave(اگلا_cmdline);
		ورنہ
		    repeat_cmdline = تہی;
	    }
	}

	// 3. Make a copy of the command so we can mess with it.
	ورنہ اگر (cmdline_copy == تہی)
	{
	    اگلا_cmdline = vim_strsave(اگلا_cmdline);
	    اگر (اگلا_cmdline == تہی)
	    {
		emsg(_(e_برآمد_کا_حافظہ));
		retval = FAIL;
		توڑو;
	    }
	}
	cmdline_copy = اگلا_cmdline;

#اگر_تعریف_شدہ FEAT_EVAL
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
	اگر ((cstack.cs_looplevel > 0 || has_loop_cmd(اگلا_cmdline)))
	{
	    cmd_getline = get_loop_سطر;
	    cmd_cookie = (خالی *)&cmd_loop_cookie;
	    cmd_loop_cookie.سطریں_gap = &سطریں_ga;
	    cmd_loop_cookie.current_سطر = current_سطر;
	    cmd_loop_cookie.lc_getline = fgetline;
	    cmd_loop_cookie.cookie = cookie;
	    cmd_loop_cookie.repeating = (current_سطر < سطریں_ga.ga_len);

	    // Save the current line when encountering it the first time.
	    اگر (current_سطر == سطریں_ga.ga_len
		    && store_loop_سطر(&سطریں_ga, اگلا_cmdline) == FAIL)
	    {
		retval = FAIL;
		توڑو;
	    }
	    current_سطر_before = current_سطر;
	}
	ورنہ
	{
	    cmd_getline = fgetline;
	    cmd_cookie = cookie;
	}

	did_endif = FALSE;
#اگر_ختم

	اگر (گنتی++ == 0)
	{
	    /*
	     * All output from the commands is put below each other, without
	     * waiting for a return. Don't do this when executing commands
	     * from a script or when being called recursive (e.g. for ":e
	     * +command file").
	     */
	    اگر (!(flags & DOCMD_NOWAIT) && !recursive)
	    {
		پیغام_didout_before_آغاز = پیغام_didout;
		پیغام_didany = FALSE; // no output yet
		پیغام_آغاز();
		پیغام_scroll = TRUE;  // put messages below each other
		++no_wait_return;   // don't wait for return until finished
		++RedrawingDisabled;
		did_inc_RedrawingDisabled = TRUE;
	    }
	}

	اگر ((p_verbose >= 15 && SOURCING_NAME != تہی) || p_verbose >= 16)
	    پیغام_verbose_cmd(SOURCING_LNUM, cmdline_copy);

	/*
	 * 2. Execute one '|' separated command.
	 *    do_one_cmd() will return NULL if there is no trailing '|'.
	 *    "cmdline_copy" can change, e.g. for '%' and '#' expansion.
	 */
	++recursive;
	اگلا_cmdline = do_one_cmd(&cmdline_copy, flags,
#اگر_تعریف_شدہ FEAT_EVAL
				&cstack,
#اگر_ختم
				cmd_getline, cmd_cookie);
	--recursive;

#اگر_تعریف_شدہ FEAT_EVAL
	اگر (cmd_cookie == (خالی *)&cmd_loop_cookie)
	    // Use "current_line" from "cmd_loop_cookie", it may have been
	    // incremented when defining a function.
	    current_سطر = cmd_loop_cookie.current_سطر;
#اگر_ختم

	اگر (اگلا_cmdline == تہی)
	{
	    VIM_CLEAR(cmdline_copy);

	    /*
	     * If the command was typed, remember it for the ':' register.
	     * Do this AFTER executing the command to make :@: work.
	     */
	    اگر (getline_equal(fgetline, cookie, getexline)
						  && new_آخری_cmdline != تہی)
	    {
		vim_free(آخری_cmdline);
		آخری_cmdline = new_آخری_cmdline;
		new_آخری_cmdline = تہی;
	    }
	}
	ورنہ
	{
	    // need to copy the command after the '|' to cmdline_copy, for the
	    // next do_one_cmd()
	    STRMOVE(cmdline_copy, اگلا_cmdline);
	    اگلا_cmdline = cmdline_copy;
	}


#اگر_تعریف_شدہ FEAT_EVAL
	// reset did_emsg for a function that is not aborted by an error
	اگر (did_emsg && !force_abort
		&& getline_equal(fgetline, cookie, get_func_سطر)
					      && !func_has_abort(real_cookie))
	{
	    // did_emsg_cumul is not set here
	    did_emsg = FALSE;
	}

	اگر (cstack.cs_looplevel > 0)
	{
	    ++current_سطر;

	    /*
	     * An ":endwhile", ":endfor" and ":continue" is handled here.
	     * If we were executing commands, jump back to the ":while" or
	     * ":for".
	     * If we were not executing commands, decrement cs_looplevel.
	     */
	    اگر (cstack.cs_lflags & (CSL_HAD_CONT | CSL_HAD_ENDLOOP))
	    {
		cstack.cs_lflags &= ~(CSL_HAD_CONT | CSL_HAD_ENDLOOP);

		// Jump back to the matching ":while" or ":for".  Be careful
		// not to use a cs_line[] from an entry that isn't a ":while"
		// or ":for": It would make "current_line" invalid and can
		// cause a crash.
		اگر (!did_emsg && !got_int && !did_throw
			&& cstack.cs_idx >= 0
			&& (cstack.cs_flags[cstack.cs_idx]
						      & (CSF_WHILE | CSF_FOR))
			&& cstack.cs_سطر[cstack.cs_idx] >= 0
			&& (cstack.cs_flags[cstack.cs_idx] & CSF_فعال))
		{
		    current_سطر = cstack.cs_سطر[cstack.cs_idx];
						// remember we jumped there
		    cstack.cs_lflags |= CSL_HAD_LOOP;
		    سطر_breakcheck();		// check if CTRL-C typed

		    // Check for the next breakpoint at or after the ":while"
		    // or ":for".
		    اگر (breakpoint != تہی && سطریں_ga.ga_len > current_سطر)
		    {
			*breakpoint = dbg_ڈھونڈو_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
									fname,
			   ((wcmd_T *)سطریں_ga.ga_ڈیٹا)[current_سطر].lnum-1);
			*dbg_tick = debug_tick;
		    }
		}
		ورنہ
		{
		    // can only get here with ":endwhile" or ":endfor"
		    اگر (cstack.cs_idx >= 0)
			rewind_conditionals(&cstack, cstack.cs_idx - 1,
				   CSF_WHILE | CSF_FOR, &cstack.cs_looplevel);
		}
	    }

	    /*
	     * For a ":while" or ":for" we need to remember the line number.
	     */
	    ورنہ اگر (cstack.cs_lflags & CSL_HAD_LOOP)
	    {
		cstack.cs_lflags &= ~CSL_HAD_LOOP;
		cstack.cs_سطر[cstack.cs_idx] = current_سطر_before;
	    }
	}

	// Check for the next breakpoint after a watchexpression
	اگر (breakpoint != تہی && has_watchexpr())
	{
	    *breakpoint = dbg_ڈھونڈو_breakpoint(FALSE, fname, SOURCING_LNUM);
	    *dbg_tick = debug_tick;
	}

	/*
	 * When not inside any ":while" loop, clear remembered lines.
	 */
	اگر (cstack.cs_looplevel == 0)
	{
	    اگر (سطریں_ga.ga_len > 0)
	    {
		SOURCING_LNUM =
		       ((wcmd_T *)سطریں_ga.ga_ڈیٹا)[سطریں_ga.ga_len - 1].lnum;
		free_cmdlines(&سطریں_ga);
	    }
	    current_سطر = 0;
	}

	/*
	 * A ":finally" makes did_emsg, got_int, and did_throw pending for
	 * being restored at the ":endtry".  Reset them here and set the
	 * ACTIVE and FINALLY flags, so that the finally clause gets executed.
	 * This includes the case where a missing ":endif", ":endwhile" or
	 * ":endfor" was detected by the ":finally" itself.
	 */
	اگر (cstack.cs_lflags & CSL_HAD_FINA)
	{
	    cstack.cs_lflags &= ~CSL_HAD_FINA;
	    report_make_pending(cstack.cs_pending[cstack.cs_idx]
		    & (CSTP_ERROR | CSTP_مداخلت | CSTP_THROW),
		    did_throw ? (خالی *)current_exception : تہی);
	    did_emsg = got_int = did_throw = FALSE;
	    cstack.cs_flags[cstack.cs_idx] |= CSF_فعال | CSF_FINALLY;
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
	اگر (trylevel == 0 && !did_emsg && !got_int && !did_throw)
	    force_abort = FALSE;

	// Convert an interrupt to an exception if appropriate.
	(خالی)do_intthrow(&cstack);
#اگر_ختم // FEAT_EVAL

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
    جب (!((got_int
#اگر_تعریف_شدہ FEAT_EVAL
		    || (did_emsg && (force_abort || اندر_vim9script()))
		    || did_throw
#اگر_ختم
	     )
#اگر_تعریف_شدہ FEAT_EVAL
		&& cstack.cs_trylevel == 0
#اگر_ختم
	    )
	    && !(did_emsg
#اگر_تعریف_شدہ FEAT_EVAL
		// Keep going when inside try/catch, so that the error can be
		// deal with, except when it is a syntax error, it may cause
		// the :endtry to be missed.
		&& (cstack.cs_trylevel == 0 || did_emsg_syntax)
#اگر_ختم
		&& مستعمل_getline
			    && (getline_equal(fgetline, cookie, getexmodeline)
			       || getline_equal(fgetline, cookie, getexline)))
	    && (اگلا_cmdline != تہی
#اگر_تعریف_شدہ FEAT_EVAL
			|| cstack.cs_idx >= 0
#اگر_ختم
			|| (flags & DOCMD_REPEAT)));

    vim_free(cmdline_copy);
    did_emsg_syntax = FALSE;
#اگر_تعریف_شدہ FEAT_EVAL
    free_cmdlines(&سطریں_ga);
    ga_clear(&سطریں_ga);

    اگر (cstack.cs_idx >= 0)
    {
	/*
	 * If a sourced file or executed function ran to its end, report the
	 * unclosed conditional.
	 * In Vim9 script do not give a second error, executing aborts after
	 * the first one.
	 */
	اگر (!got_int && !did_throw && !aborting()
		&& !(did_emsg && اندر_vim9script())
		&& ((getline_equal(fgetline, cookie, getsourceline)
			&& !ماخذ_ختم_شدہ(fgetline, cookie))
		    || (getline_equal(fgetline, cookie, get_func_سطر)
					    && !func_has_ended(real_cookie))))
	{
	    اگر (cstack.cs_flags[cstack.cs_idx] & CSF_TRY)
		emsg(_(e_missing_endtry));
	    ورنہ اگر (cstack.cs_flags[cstack.cs_idx] & CSF_WHILE)
		emsg(_(e_missing_endwhile));
	    ورنہ اگر (cstack.cs_flags[cstack.cs_idx] & CSF_FOR)
		emsg(_(e_missing_endfor));
	    ورنہ
		emsg(_(e_missing_endif));
	}

	/*
	 * Reset "trylevel" in case of a ":finish" or ":return" or a missing
	 * ":endtry" in a sourced file or executed function.  If the try
	 * conditional is in its finally clause, ignore anything pending.
	 * If it is in a catch clause, finish the caught exception.
	 * Also cleanup any "cs_forinfo" structures.
	 */
	کرو
	{
	    صحیح idx = cleanup_conditionals(&cstack, 0, TRUE);

	    اگر (idx >= 0)
		--idx;	    // remove try block not in its finally clause
	    rewind_conditionals(&cstack, idx, CSF_WHILE | CSF_FOR,
							&cstack.cs_looplevel);
	}
	جب (cstack.cs_idx >= 0);
	trylevel = initial_trylevel;
    }

    // If a missing ":endtry", ":endwhile", ":endfor", or ":endif" or a memory
    // lack was reported above and the error message is to be converted to an
    // exception, do this now after rewinding the cstack.
    do_errthrow(&cstack, getline_equal(fgetline, cookie, get_func_سطر)
				  ? (char_u *)"endfunction" : (char_u *)تہی);

    اگر (trylevel == 0)
    {
	// Just in case did_throw got set but current_exception wasn't.
	اگر (current_exception == تہی)
	    did_throw = FALSE;

	/*
	 * When an exception is being thrown out of the outermost try
	 * conditional, discard the uncaught exception, disable the conversion
	 * of interrupts or errors to exceptions, and ensure that no more
	 * commands are executed.
	 */
	اگر (did_throw)
	    handle_did_throw();

	/*
	 * On an interrupt or an aborting error not converted to an exception,
	 * disable the conversion of errors to exceptions.  (Interrupts are not
	 * converted anymore, here.) This enables also the interrupt message
	 * when force_abort is set and did_emsg unset in case of an interrupt
	 * from a finally clause after an error.
	 */
	ورنہ اگر (got_int || (did_emsg && force_abort))
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
    اگر (did_throw)
	need_rethrow = TRUE;
    اگر ((getline_equal(fgetline, cookie, getsourceline)
		&& ex_nesting_level > ماخذ_level(real_cookie))
	    || (getline_equal(fgetline, cookie, get_func_سطر)
		&& ex_nesting_level > func_level(real_cookie) + 1))
    {
	اگر (!did_throw)
	    check_cstack = TRUE;
    }
    ورنہ
    {
	// When leaving a function, reduce nesting level.
	اگر (getline_equal(fgetline, cookie, get_func_سطر))
	    --ex_nesting_level;
	/*
	 * Go to debug mode when returning from a function in which we are
	 * single-stepping.
	 */
	اگر ((getline_equal(fgetline, cookie, getsourceline)
		    || getline_equal(fgetline, cookie, get_func_سطر))
		&& ex_nesting_level + 1 <= debug_break_level)
	    do_debug(getline_equal(fgetline, cookie, getsourceline)
		    ? (char_u *)_("End of sourced file")
		    : (char_u *)_("End of function"));
    }

    /*
     * Restore the exception environment (done after returning from the
     * debugger).
     */
    اگر (flags & DOCMD_EXCRESET)
	restore_dbg_stuff(&debug_saved);

    پیغام_list = saved_پیغام_list;

    // Cleanup if "cs_emsg_silent_list" remains.
    اگر (cstack.cs_emsg_silent_list != تہی)
    {
	eslist_T *elem, *عارضی;

	برائے (elem = cstack.cs_emsg_silent_list; elem != تہی; elem = عارضی)
	{
	    عارضی = elem->اگلا;
	    vim_free(elem);
	}
    }
#اگر_ختم // FEAT_EVAL

    /*
     * If there was too much output to fit on the command line, ask the user to
     * hit return before redrawing the screen. With the ":global" command we do
     * this only once after the command is finished.
     */
    اگر (did_inc_RedrawingDisabled)
    {
	اگر (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	--no_wait_return;
	پیغام_scroll = FALSE;

	/*
	 * When just finished an ":if"-":else" which was typed, no need to
	 * wait for hit-return.  Also for an error situation.
	 */
	اگر (retval == FAIL
#اگر_تعریف_شدہ FEAT_EVAL
		|| (did_endif && کلید_Typed && !did_emsg)
#اگر_ختم
					    )
	{
	    need_wait_return = FALSE;
	    پیغام_didany = FALSE;		// don't wait when restarting edit
	}
	ورنہ اگر (need_wait_return)
	{
	    /*
	     * The msg_start() above clears msg_didout. The wait_return() we do
	     * here should not overwrite the command that may be shown before
	     * doing that.
	     */
	    پیغام_didout |= پیغام_didout_before_آغاز;
	    wait_return(FALSE);
	}
    }

#اگر_تعریف_شدہ FEAT_EVAL
    did_endif = FALSE;  // in case do_cmdline used recursively
#ورنہ
    /*
     * Reset if_level, in case a sourced script file contains more ":if" than
     * ":endif" (could be ":if x | foo | endif").
     */
    if_level = 0;
#اگر_ختم

    --call_گہرائی;
    واپس retval;
}

#اگر defined(FEAT_EVAL)
/*
 * Handle when "did_throw" is set after executing commands.
 */
    خالی
handle_did_throw(خالی)
{
    حرف	*p = تہی;
    msglist_T	*messages = تہی;
    ESTACK_CHECK_DECLARATION;

    /*
     * If the uncaught exception is a user exception, report it as an
     * error.  If it is an error exception, display the saved error
     * message now.  For an interrupt exception, do nothing; the
     * interrupt message is given elsewhere.
     */
    انتخاب (current_exception->type)
    {
	حالت ET_USER:
	    vim_snprintf((حرف *)IObuff, IOSIZE,
		    _(e_exception_not_caught_str),
		    current_exception->قدر);
	    p = (حرف *)vim_strsave(IObuff);
	    توڑو;
	حالت ET_ERROR:
	    messages = current_exception->messages;
	    current_exception->messages = تہی;
	    توڑو;
	حالت ET_مداخلت:
	    توڑو;
    }

    estack_ڈالو(ETYPE_EXCEPT, current_exception->throw_name,
					current_exception->throw_lnum);
    ESTACK_CHECK_تیاری;
    current_exception->throw_name = تہی;

    discard_current_exception();	// uses IObuff if 'verbose'

    // If "silent!" is active the uncaught exception is not fatal.
    اگر (emsg_silent == 0)
    {
	suppress_errthrow = TRUE;
	force_abort = TRUE;
    }

    اگر (messages != تہی)
    {
	کرو
	{
	    msglist_T	*اگلا = messages->اگلا;
	    صحیح		save_compiling = estack_compiling;

	    estack_compiling = messages->پیغام_compiling;
	    emsg(messages->پیغام);
	    vim_free(messages->پیغام);
	    vim_free(messages->sfile);
	    vim_free(messages);
	    messages = اگلا;
	    estack_compiling = save_compiling;
	}
	جب (messages != تہی);
    }
    ورنہ اگر (p != تہی)
    {
	emsg(p);
	vim_free(p);
    }
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_نکالو();
}

/*
 * Obtain a line when inside a ":while" or ":for" loop.
 */
    ساکن char_u *
get_loop_سطر(صحیح c, خالی *cookie, صحیح indent, getline_opt_T options)
{
    ساخت loop_cookie	*cp = (ساخت loop_cookie *)cookie;
    wcmd_T		*wp;
    char_u		*سطر;

    اگر (cp->current_سطر + 1 >= cp->سطریں_gap->ga_len)
    {
	اگر (cp->repeating)
	    واپس تہی;	// trying to read past ":endwhile"/":endfor"

	// First time inside the ":while"/":for": get line normally.
	اگر (cp->lc_getline == تہی)
	    سطر = getcmdline(c, 0L, indent, 0);
	ورنہ
	    سطر = cp->lc_getline(c, cp->cookie, indent, options);
	اگر (سطر != تہی && store_loop_سطر(cp->سطریں_gap, سطر) == OK)
	    ++cp->current_سطر;

	واپس سطر;
    }

    کلید_Typed = FALSE;
    ++cp->current_سطر;
    wp = (wcmd_T *)(cp->سطریں_gap->ga_ڈیٹا) + cp->current_سطر;
    SOURCING_LNUM = wp->lnum;
    واپس vim_strsave(wp->سطر);
}

/*
 * Store a line in "gap" so that a ":while" loop can execute it again.
 */
    ساکن صحیح
store_loop_سطر(garray_T *gap, char_u *سطر)
{
    اگر (ga_grow(gap, 1) == FAIL)
	واپس FAIL;
    ((wcmd_T *)(gap->ga_ڈیٹا))[gap->ga_len].سطر = vim_strsave(سطر);
    ((wcmd_T *)(gap->ga_ڈیٹا))[gap->ga_len].lnum = SOURCING_LNUM;
    ++gap->ga_len;
    واپس OK;
}

/*
 * Free the lines stored for a ":while" or ":for" loop.
 */
    ساکن خالی
free_cmdlines(garray_T *gap)
{
    جب (gap->ga_len > 0)
    {
	vim_free(((wcmd_T *)(gap->ga_ڈیٹا))[gap->ga_len - 1].سطر);
	--gap->ga_len;
    }
}
#اگر_ختم

/*
 * If "fgetline" is get_loop_line(), return TRUE if the getline it uses equals
 * "func".  * Otherwise return TRUE when "fgetline" equals "func".
 */
    صحیح
getline_equal(
    char_u	*(*fgetline)(صحیح, خالی *, صحیح, getline_opt_T),
    خالی	*cookie غیر_مستعمل,		// argument for fgetline()
    char_u	*(*func)(صحیح, خالی *, صحیح, getline_opt_T))
{
#اگر_تعریف_شدہ FEAT_EVAL
    char_u		*(*gp)(صحیح, خالی *, صحیح, getline_opt_T);
    ساخت loop_cookie *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // function that's originally used to obtain the lines.  This may be
    // nested several levels.
    gp = fgetline;
    cp = (ساخت loop_cookie *)cookie;
    جب (gp == get_loop_سطر)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    واپس gp == func;
#ورنہ
    واپس fgetline == func;
#اگر_ختم
}

/*
 * If "fgetline" is get_loop_line(), return the cookie used by the original
 * getline function.  Otherwise return "cookie".
 */
    خالی *
getline_cookie(
    char_u	*(*fgetline)(صحیح, خالی *, صحیح, getline_opt_T) غیر_مستعمل,
    خالی	*cookie)		// argument for fgetline()
{
#اگر_تعریف_شدہ FEAT_EVAL
    char_u		*(*gp)(صحیح, خالی *, صحیح, getline_opt_T);
    ساخت loop_cookie  *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (ساخت loop_cookie *)cookie;
    جب (gp == get_loop_سطر)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    واپس cp;
#ورنہ
    واپس cookie;
#اگر_ختم
}

#اگر defined(FEAT_EVAL)
/*
 * Get the next line source line without advancing.
 */
    char_u *
getline_peek(
    char_u	*(*fgetline)(صحیح, خالی *, صحیح, getline_opt_T) غیر_مستعمل,
    خالی	*cookie)		// argument for fgetline()
{
    char_u		*(*gp)(صحیح, خالی *, صحیح, getline_opt_T);
    ساخت loop_cookie  *cp;
    wcmd_T		*wp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (ساخت loop_cookie *)cookie;
    جب (gp == get_loop_سطر)
    {
	اگر (cp->current_سطر + 1 < cp->سطریں_gap->ga_len)
	{
	    // executing lines a second time, use the stored copy
	    wp = (wcmd_T *)(cp->سطریں_gap->ga_ڈیٹا) + cp->current_سطر + 1;
	    واپس wp->سطر;
	}
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    اگر (gp == getsourceline)
	واپس ماخذ_nextline(cp);
    واپس تہی;
}
#اگر_ختم


/*
 * Helper function to apply an offset for buffer commands, i.e. ":bdelete",
 * ":bwipeout", etc.
 * Returns the buffer number.
 */
    ساکن صحیح
compute_بفر_local_گنتی(صحیح addr_type, صحیح lnum, صحیح انحراف)
{
    buf_T   *buf;
    buf_T   *nextbuf;
    صحیح     گنتی = انحراف;

    buf = firstbuf;
    جب (buf->b_اگلا != تہی && buf->b_fnum < lnum)
	buf = buf->b_اگلا;
    جب (گنتی != 0)
    {
	گنتی += (انحراف < 0) ? 1 : -1;
	nextbuf = (انحراف < 0) ? buf->b_پچھلا : buf->b_اگلا;
	اگر (nextbuf == تہی)
	    توڑو;
	buf = nextbuf;
	اگر (addr_type == ADDR_LOADED_BUFFERS)
	    // skip over unloaded buffers
	    جب (buf->b_ml.ml_mfp == تہی)
	    {
		nextbuf = (انحراف < 0) ? buf->b_پچھلا : buf->b_اگلا;
		اگر (nextbuf == تہی)
		    توڑو;
		buf = nextbuf;
	    }
    }
    // we might have gone too far, last buffer is not loadedd
    اگر (addr_type == ADDR_LOADED_BUFFERS)
	جب (buf->b_ml.ml_mfp == تہی)
	{
	    nextbuf = (انحراف >= 0) ? buf->b_پچھلا : buf->b_اگلا;
	    اگر (nextbuf == تہی)
		توڑو;
	    buf = nextbuf;
	}
    واپس buf->b_fnum;
}

/*
 * Return the window number of "win".
 * When "win" is NULL return the number of windows.
 */
    ساکن صحیح
current_win_nr(win_T *win)
{
    win_T	*wp;
    صحیح		nr = 0;

    FOR_ALL_WINDOWS(wp)
    {
	++nr;
	اگر (wp == win)
	    توڑو;
    }
    واپس nr;
}

    ساکن صحیح
current_tab_nr(tabpage_T *tab)
{
    tabpage_T	*tp;
    صحیح		nr = 0;

    FOR_ALL_TABPAGES(tp)
    {
	++nr;
	اگر (tp == tab)
	    توڑو;
    }
    واپس nr;
}

    ساکن صحیح
comment_آغاز(char_u *p, صحیح starts_with_colon غیر_مستعمل)
{
    اگر (اندر_vim9script())
	واپس p[0] == '#' && !starts_with_colon;
    واپس *p == '"';
}

#تعریف_کرو CURRENT_WIN_NR current_win_nr(curwin)
#تعریف_کرو آخری_WIN_NR current_win_nr(تہی)
#تعریف_کرو CURRENT_TAB_NR current_tab_nr(curtab)
#تعریف_کرو آخری_TAB_NR current_tab_nr(تہی)

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
    ساکن char_u *
do_one_cmd(
    char_u	**cmdlinep,
    صحیح		flags,
#اگر_تعریف_شدہ FEAT_EVAL
    cstack_T	*cstack,
#اگر_ختم
    char_u	*(*fgetline)(صحیح, خالی *, صحیح, getline_opt_T),
    خالی	*cookie)		// argument for fgetline()
{
    char_u	*p;
    linenr_T	lnum;
    طویل	n;
    حرف	*errormsg = تہی;	// error message
    char_u	*after_modifier = تہی;
    exarg_T	ea;			// Ex command arguments
    cmdmod_T	save_cmdmod;
    صحیح		save_reg_executing = reg_executing;
    صحیح		save_pending_انتہا_reg_executing = pending_انتہا_reg_executing;
    صحیح		ni;			// set when Not Implemented
    char_u	*cmd;
    صحیح		starts_with_colon = FALSE;
    صحیح		may_have_range;
#اگر_تعریف_شدہ FEAT_EVAL
    صحیح		did_سیٹ_اظہاریہ_سطر = FALSE;
#اگر_ختم
    صحیح		sourcing = flags & DOCMD_VERBOSE;
    صحیح		did_append_cmd = FALSE;

    CLEAR_میدان(ea);
    ea.line1 = 1;
    ea.line2 = 1;
#اگر_تعریف_شدہ FEAT_EVAL
    ++ex_nesting_level;
#اگر_ختم

    // When the last file has not been edited :q has to be typed twice.
    اگر (quitmore
#اگر_تعریف_شدہ FEAT_EVAL
	    // avoid that a function call in 'statusline' does this
	    && !getline_equal(fgetline, cookie, get_func_سطر)
#اگر_ختم
	    // avoid that an autocommand, e.g. QuitPre, does this
	    && !getline_equal(fgetline, cookie, getnextac))
	--quitmore;

    /*
     * Reset browse, confirm, etc..  They are restored when returning, for
     * recursive calls.
     */
    save_cmdmod = cmdmod;

    // "#!anything" is handled like a comment.
    اگر ((*cmdlinep)[0] == '#' && (*cmdlinep)[1] == '!')
	جاؤ doend;

/*
 * 1. Skip comment lines and leading white space and colons.
 * 2. Handle command modifiers.
 */
    // The "ea" structure holds the arguments that can be used.
    ea.cmd = *cmdlinep;
    ea.cmdlinep = cmdlinep;
    ea.ea_getline = fgetline;
    ea.cookie = cookie;
#اگر_تعریف_شدہ FEAT_EVAL
    ea.cstack = cstack;
    starts_with_colon = *skipwhite(ea.cmd) == ':';
#اگر_ختم
    اگر (parse_command_modifiers(&ea, &errormsg, &cmdmod, FALSE) == FAIL)
	جاؤ doend;
    apply_cmdmod(&cmdmod);
    after_modifier = ea.cmd;

#اگر_تعریف_شدہ FEAT_EVAL
    ea.skip = did_emsg || got_int || did_throw || (cstack->cs_idx >= 0
			 && !(cstack->cs_flags[cstack->cs_idx] & CSF_فعال));
#ورنہ
    ea.skip = (if_level > 0);
#اگر_ختم

/*
 * 3. Skip over the range to find the command.  Let "p" point to after it.
 *
 * We need the command to know what kind of range it uses.
 */
    cmd = ea.cmd;

    // In Vim9 script a colon is required before the range.  This may also be
    // after command modifiers.
    صحیح vim9script = اندر_vim9script();
    اگر (vim9script && (flags & DOCMD_RANGEOK) == 0)
    {
	may_have_range = FALSE;
	برائے (p = ea.cmd; p >= *cmdlinep; --p)
	{
	    اگر (*p == ':')
		may_have_range = TRUE;
	    اگر (p < ea.cmd && !VIM_ISWHITE(*p))
		توڑو;
	}
    }
    ورنہ
	may_have_range = TRUE;
    اگر (may_have_range)
	ea.cmd = skip_range(ea.cmd, TRUE, تہی);

#اگر_تعریف_شدہ FEAT_EVAL
    // Handle ":export" - it functions almost like a command modifier.
    // ":export var Name: type"
    // ":export def Name(..."
    // etc.
    اگر (vim9script && checkforcmd_noparen(&ea.cmd, "export", 6))
	is_export = TRUE;
#اگر_ختم

    اگر (vim9script && !may_have_range)
    {
	اگر (ea.cmd == cmd + 1 && *cmd == '$')
	    // should be "$VAR = val"
	    --ea.cmd;
#اگر_تعریف_شدہ FEAT_EVAL
	p = ڈھونڈو_ex_command(&ea, تہی, lookup_scriptitem, تہی);
#ورنہ
	p = ڈھونڈو_ex_command(&ea, تہی, تہی, تہی);
#اگر_ختم
	اگر (ea.cmdidx == CMD_سائز)
	{
	    char_u *ar = skip_range(ea.cmd, TRUE, تہی);

	    // If a ':' before the range is missing, give a clearer error
	    // message.
	    اگر (ar > ea.cmd && !ea.skip)
	    {
		semsg(_(e_colon_required_before_range_str), ea.cmd);
		جاؤ doend;
	    }
	}
    }
    ورنہ
	p = ڈھونڈو_ex_command(&ea, تہی, تہی, تہی);

#اگر_تعریف_شدہ FEAT_EVAL
# ifdef FEAT_PROFILE
    // Count this line for profiling if skip is TRUE.
    اگر (do_profiling == PROF_YES
	    && (!ea.skip || cstack->cs_idx == 0 || (cstack->cs_idx > 0
		     && (cstack->cs_flags[cstack->cs_idx - 1] & CSF_فعال))))
    {
	صحیح skip = did_emsg || got_int || did_throw;

	اگر (ea.cmdidx == CMD_catch)
	    skip = !skip && !(cstack->cs_idx >= 0
			  && (cstack->cs_flags[cstack->cs_idx] & CSF_THROWN)
			  && !(cstack->cs_flags[cstack->cs_idx] & CSF_CAUGHT));
	ورنہ اگر (ea.cmdidx == CMD_else || ea.cmdidx == CMD_elseif)
	    skip = skip || !(cstack->cs_idx >= 0
			  && !(cstack->cs_flags[cstack->cs_idx]
						  & (CSF_فعال | CSF_TRUE)));
	ورنہ اگر (ea.cmdidx == CMD_finally)
	    skip = FALSE;
	ورنہ اگر (ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endtry
		&& ea.cmdidx != CMD_endwhile)
	    skip = ea.skip;

	اگر (!skip)
	{
	    اگر (getline_equal(fgetline, cookie, get_func_سطر))
		func_سطر_exec(getline_cookie(fgetline, cookie));
	    ورنہ اگر (getline_equal(fgetline, cookie, getsourceline))
		script_سطر_exec();
	}
    }
# endif
#اگر_ختم

    ea.cmd = cmd;

#اگر_تعریف_شدہ FEAT_EVAL
    // May go to debug mode.  If this happens and the ">quit" debug command is
    // used, throw an interrupt exception and skip the next command.
    dbg_check_breakpoint(&ea);
    اگر (!ea.skip && got_int)
    {
	ea.skip = TRUE;
	(خالی)do_intthrow(cstack);
    }
#اگر_ختم

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
    اگر (!IS_USER_CMDIDX(ea.cmdidx))
    {
	اگر (ea.cmdidx != CMD_سائز)
	    ea.addr_type = cmdnames[(صحیح)ea.cmdidx].cmd_addr_type;
	ورنہ
	    ea.addr_type = ADDR_سطریں;

	// :wincmd range depends on the argument.
	اگر (ea.cmdidx == CMD_wincmd && p != تہی)
	    get_wincmd_addr_type(skipwhite(p), &ea);
#اگر_تعریف_شدہ FEAT_QUICKFIX
	// :.cc in quickfix window uses line number
	اگر ((ea.cmdidx == CMD_cc || ea.cmdidx == CMD_ll) && bt_quickfix(curbuf))
	    ea.addr_type = ADDR_OTHER;
#اگر_ختم
    }

    اگر (!may_have_range)
	ea.line1 = ea.line2 = default_پتہ(&ea);
    ورنہ اگر (parse_cmd_پتہ(&ea, &errormsg, FALSE) == FAIL)
	جاؤ doend;

/*
 * 5. Parse the command.
 */

    /*
     * Skip ':' and any white space
     */
    ea.cmd = skipwhite(ea.cmd);
    جب (*ea.cmd == ':')
	ea.cmd = skipwhite(ea.cmd + 1);

    /*
     * If we got a line, but no command, then go to the line.
     * If we find a '|' or '\n' we set ea.nextcmd.
     */
    اگر (*ea.cmd == NUL || comment_آغاز(ea.cmd, starts_with_colon)
			       || (ea.nextcmd = check_nextcmd(ea.cmd)) != تہی)
    {
	/*
	 * strange vi behaviour:
	 * ":3"		jumps to line 3
	 * ":3|..."	prints line 3  (not in Vim9 script)
	 * ":|"		prints current line  (not in Vim9 script)
	 */
	اگر (ea.skip)	    // skip this if inside :if
	    جاؤ doend;
	errormsg = ex_range_without_command(&ea);
	جاؤ doend;
    }

    // If this looks like an undefined user command and there are CmdUndefined
    // autocommands defined, trigger the matching autocommands.
    اگر (p != تہی && ea.cmdidx == CMD_سائز && !ea.skip
	    && ASCII_ISUPPER(*ea.cmd)
	    && has_cmdundefined())
    {
	صحیح ret;

	p = ea.cmd;
	جب (ASCII_ISALNUM(*p))
	    ++p;
	p = vim_strnsave(ea.cmd, p - ea.cmd);
	ret = apply_autocmds(EVENT_CMDUNDEFINED, p, p, TRUE, تہی);
	vim_free(p);
	// If the autocommands did something and didn't cause an error, try
	// finding the command again.
	p = (ret
#اگر_تعریف_شدہ FEAT_EVAL
		&& !aborting()
#اگر_ختم
		) ? ڈھونڈو_ex_command(&ea, تہی, تہی, تہی) : ea.cmd;
    }

    اگر (p == تہی)
    {
	اگر (!ea.skip)
	    errormsg = _(e_ambiguous_use_کا_user_defined_command);
	جاؤ doend;
    }
    // Check for wrong commands.
    اگر (*p == '!' && ea.cmd[1] == 0151 && ea.cmd[0] == 78
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	errormsg = uc_fun_cmd();
	جاؤ doend;
    }

    اگر (ea.cmdidx == CMD_سائز)
    {
	اگر (!ea.skip)
	{
	    STRCPY(IObuff, _(e_not_an_editor_command));
	    اگر (!sourcing)
	    {
		// If the modifier was parsed OK the error must be in the
		// following command
		اگر (after_modifier != تہی)
		    append_command(after_modifier);
		ورنہ
		    append_command(*cmdlinep);
		did_append_cmd = TRUE;
	    }
	    errormsg = (حرف *)IObuff;
	    did_emsg_syntax = TRUE;
	}
	جاؤ doend;
    }

    ni = (!IS_USER_CMDIDX(ea.cmdidx)
	    && (cmdnames[ea.cmdidx].cmd_func == ex_ni
#اگر_تعریف_شدہ HAVE_EX_SCRIPT_NI
	     || cmdnames[ea.cmdidx].cmd_func == ex_script_ni
#اگر_ختم
	     ));

#اگر_غیر_تعریف_شدہ FEAT_EVAL
    /*
     * When the expression evaluation is disabled, recognize the ":if" and
     * ":endif" commands and ignore everything in between it.
     */
    اگر (ea.cmdidx == CMD_if)
	++if_level;
    اگر (if_level)
    {
	اگر (ea.cmdidx == CMD_endif)
	    --if_level;
	جاؤ doend;
    }

#اگر_ختم

    // forced commands
    اگر (*p == '!' && ea.cmdidx != CMD_substitute
	    && ea.cmdidx != CMD_smagic && ea.cmdidx != CMD_snomagic)
    {
	++p;
	ea.forceit = TRUE;
    }
    ورنہ
	ea.forceit = FALSE;

/*
 * 6. Parse arguments.  Then check for errors.
 */
    اگر (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = (طویل)cmdnames[(صحیح)ea.cmdidx].cmd_argt;

    اگر (!ea.skip)
    {
#اگر_تعریف_شدہ HAVE_SANDBOX
	اگر (sandbox != 0 && !(ea.argt & EX_SBOXOK))
	{
	    // Command not allowed in sandbox.
	    errormsg = _(e_not_allowed_اندر_sandbox);
	    جاؤ doend;
	}
#اگر_ختم
	اگر (restricted != 0 && (ea.argt & EX_RESTRICT))
	{
	    errormsg = _(e_command_not_allowed_اندر_rvim);
	    جاؤ doend;
	}
	اگر (!curbuf->b_p_ma && (ea.argt & EX_MODIFY))
	{
	    // Command not allowed in non-'modifiable' buffer
	    errormsg = _(e_cannot_make_changes_modifiable_is_off);
	    جاؤ doend;
	}

	اگر (!IS_USER_CMDIDX(ea.cmdidx))
	{
	    اگر (cmdwin_type != 0 && !(ea.argt & EX_CMDWIN))
	    {
		// Command not allowed in the command line window
		errormsg = _(e_invalid_اندر_cmdline_کھڑکی);
		جاؤ doend;
	    }
	    اگر (متن_locked() && !(ea.argt & EX_تالا_OK))
	    {
		// Command not allowed when text is locked
		errormsg = _(get_متن_locked_پیغام());
		جاؤ doend;
	    }
	}

	// Disallow editing another buffer when "curbuf_lock" is set.
	// Do allow ":checktime" (it is postponed).
	// Do allow ":edit" (check for an argument later).
	// Do allow ":file" with no arguments (check for an argument later).
	اگر (!(ea.argt & (EX_CMDWIN | EX_تالا_OK))
		&& ea.cmdidx != CMD_checktime
		&& ea.cmdidx != CMD_edit
		&& ea.cmdidx != CMD_file
		&& !IS_USER_CMDIDX(ea.cmdidx)
		&& curbuf_locked())
	    جاؤ doend;

	اگر (!ni && !(ea.argt & EX_RANGE) && ea.addr_گنتی > 0)
	{
	    errormsg = _(e_no_range_allowed);
	    جاؤ doend;
	}
    }

    اگر (!ni && !(ea.argt & EX_BANG) && ea.forceit)
    {
	errormsg = _(e_no_bang_allowed);
	جاؤ doend;
    }

    /*
     * Don't complain about the range if it is not used
     * (could happen if line_count is accidentally set to 0).
     */
    اگر (!ea.skip && !ni && (ea.argt & EX_RANGE))
    {
	/*
	 * If the range is backwards, ask for confirmation and, if given, swap
	 * ea.line1 & ea.line2 so it's forwards again.
	 * When global command is busy, don't ask, will fail below.
	 */
	اگر (!global_busy && ea.line1 > ea.line2)
	{
	    اگر (پیغام_silent == 0)
	    {
		اگر (sourcing || exmode_فعال)
		{
		    errormsg = _(e_backwards_range_given);
		    جاؤ doend;
		}
		اگر (ask_yesno((char_u *)
			_("Backwards range given, OK to swap"), FALSE) != 'y')
		    جاؤ doend;
	    }
	    lnum = ea.line1;
	    ea.line1 = ea.line2;
	    ea.line2 = lnum;
	}
	اگر ((errormsg = invalid_range(&ea)) != تہی)
	    جاؤ doend;
    }

    اگر ((ea.addr_type == ADDR_OTHER) && ea.addr_گنتی == 0)
	// default is 1, not cursor
	ea.line2 = 1;

    correct_range(&ea);

#اگر_تعریف_شدہ FEAT_FOLDING
    اگر (((ea.argt & EX_WHOLEFOLD) || ea.addr_گنتی >= 2) && !global_busy
	    && ea.addr_type == ADDR_سطریں)
    {
	// Put the first line at the start of a closed fold, put the last line
	// at the end of a closed fold.
	(خالی)hasFolding(ea.line1, &ea.line1, تہی);
	(خالی)hasFolding(ea.line2, تہی, &ea.line2);
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_QUICKFIX
    /*
     * For the ":make" and ":grep" commands we insert the 'makeprg'/'grepprg'
     * option here, so things like % get expanded.
     */
    p = replace_makeprg(&ea, p, cmdlinep);
    اگر (p == تہی)
	جاؤ doend;
#اگر_ختم

    /*
     * Skip to start of argument.
     * Don't do this for the ":!" command, because ":!! -l" needs the space.
     */
    اگر (ea.cmdidx == CMD_bang)
	ea.arg = p;
    ورنہ
	ea.arg = skipwhite(p);

    // ":file" cannot be run with an argument when "curbuf_lock" is set
    اگر (ea.cmdidx == CMD_file && *ea.arg != NUL && curbuf_locked())
	جاؤ doend;

    /*
     * Check for "++opt=val" argument.
     * Must be first, allow ":w ++enc=utf8 !cmd"
     */
    اگر (ea.argt & EX_ARGOPT)
	جب (ea.arg[0] == '+' && ea.arg[1] == '+')
	    اگر (getargopt(&ea) == FAIL && !ni)
	    {
		errormsg = _(e_invalid_دلیل);
		جاؤ doend;
	    }

    اگر (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	اگر (*ea.arg == '>')			// append
	{
	    اگر (*++ea.arg != '>')		// typed wrong
	    {
		errormsg = _(e_use_w_or_w_gt_gt);
		جاؤ doend;
	    }
	    ea.arg = skipwhite(ea.arg + 1);
	    ea.append = TRUE;
	}
	ورنہ اگر (*ea.arg == '!' && ea.cmdidx == CMD_write)  // :w !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    اگر (ea.cmdidx == CMD_read)
    {
	اگر (ea.forceit)
	{
	    ea.usefilter = TRUE;		// :r! filter if ea.forceit
	    ea.forceit = FALSE;
	}
	ورنہ اگر (*ea.arg == '!')		// :r !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    اگر (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ea.amount = 1;
	جب (*ea.arg == *ea.cmd)		// count number of '>' or '<'
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
    اگر ((ea.argt & EX_CMDARG) && !ea.usefilter)
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
    اگر ((ea.argt & EX_TRLBAR) && !ea.usefilter)
    {
	separate_nextcmd(&ea, FALSE);
    }
    ورنہ اگر (ea.cmdidx == CMD_bang
	    || ea.cmdidx == CMD_terminal
	    || ea.cmdidx == CMD_global
	    || ea.cmdidx == CMD_vglobal
	    || ea.usefilter
#اگر_تعریف_شدہ FEAT_EVAL
	    || inside_بلاک(&ea)
#اگر_ختم
	    )
    {
	برائے (p = ea.arg; *p; ++p)
	{
	    // Remove one backslash before a newline
	    اگر (*p == '\\' && p[1] == '\n')
		STRMOVE(p, p + 1);
	    ورنہ اگر (*p == '\n' && !(ea.argt & EX_اظہاریہ_ARG))
	    {
		ea.nextcmd = p + 1;
		*p = NUL;
		توڑو;
	    }
	}
    }

    اگر ((ea.argt & EX_DFLALL) && ea.addr_گنتی == 0)
	پتہ_default_all(&ea);

    // accept numbered register only when no count allowed (:put)
    اگر (       (ea.argt & EX_REGSTR)
	    && *ea.arg != NUL
	       // Do not allow register = for user commands
	    && (!IS_USER_CMDIDX(ea.cmdidx) || *ea.arg != '=')
	    && !((ea.argt & EX_گنتی) && VIM_ISDIGIT(*ea.arg)))
    {
#اگر !defined(FEAT_CLIPBOARD) && !defined(FEAT_CLIPBOARD_PROVIDER)
	// check these explicitly for a more specific error message
	اگر (*ea.arg == '*' || *ea.arg == '+')
	{
	    errormsg = _(e_invalid_رجسٹر_name);
	    جاؤ doend;
	}
#اگر_ختم
	اگر (valid_yank_reg(*ea.arg, (!IS_USER_CMDIDX(ea.cmdidx)
			    && ea.cmdidx != CMD_put && ea.cmdidx != CMD_iput)))
	{
	    ea.regname = *ea.arg++;
#اگر_تعریف_شدہ FEAT_EVAL
	    // for '=' register: accept the rest of the line as an expression
	    اگر (ea.arg[-1] == '=' && ea.arg[0] != NUL)
	    {
		اگر (!ea.skip)
		{
		    سیٹ_اظہاریہ_سطر(vim_strsave(ea.arg), &ea);
		    did_سیٹ_اظہاریہ_سطر = TRUE;
		}
		ea.arg += STRLEN(ea.arg);
	    }
#اگر_ختم
	    ea.arg = skipwhite(ea.arg);
	}
    }

    /*
     * Check for a count.  When accepting a EX_BUFNAME, don't use "123foo" as a
     * count, it's a buffer name.
     */
    اگر ((ea.argt & EX_گنتی) && VIM_ISDIGIT(*ea.arg)
	    && (!(ea.argt & EX_BUFNAME) || *(p = skipdigits(ea.arg + 1)) == NUL
							  || VIM_ISWHITE(*p)))
    {
	n = getdigits_quoted(&ea.arg);
	ea.arg = skipwhite(ea.arg);
	اگر (n <= 0 && !ni && (ea.argt & EX_ZEROR) == 0)
	{
	    errormsg = _(e_مثبت_گنتی_required);
	    جاؤ doend;
	}
	اگر (ea.addr_type != ADDR_سطریں)	// e.g. :buffer 2, :sleep 3
	{
	    ea.line2 = n;
	    اگر (ea.addr_گنتی == 0)
		ea.addr_گنتی = 1;
	}
	ورنہ
	{
	    ea.line1 = ea.line2;
	    اگر (ea.line2 >= LONG_MAX - (n - 1))
		ea.line2 = LONG_MAX;  // avoid overflow
	    ورنہ
		ea.line2 += n - 1;
	    ++ea.addr_گنتی;
	    /*
	     * Be vi compatible: no error message for out of range.
	     */
	    اگر (ea.line2 > curbuf->b_ml.ml_سطر_گنتی)
		ea.line2 = curbuf->b_ml.ml_سطر_گنتی;
	}
    }

    /*
     * Check for flags: 'l', 'p' and '#'.
     */
    اگر (ea.argt & EX_FLAGS)
	get_flags(&ea);
    اگر (!ni && !(ea.argt & EX_EXTRA) && *ea.arg != NUL
	    && *ea.arg != '"' && (*ea.arg != '|' || (ea.argt & EX_TRLBAR) == 0))
    {
	// no arguments allowed but there is something
	errormsg = ex_errmsg(e_trailing_characters_str, ea.arg);
	جاؤ doend;
    }

    اگر (!ni && (ea.argt & EX_NEEDARG) && *ea.arg == NUL)
    {
	errormsg = _(e_دلیل_required);
	جاؤ doend;
    }

#اگر_تعریف_شدہ FEAT_EVAL
    /*
     * Skip the command when it's not going to be executed.
     * The commands like :if, :endif, etc. always need to be executed.
     * Also make an exception for commands that handle a trailing command
     * themselves.
     */
    اگر (ea.skip)
    {
	انتخاب (ea.cmdidx)
	{
	    // commands that need evaluation
	    حالت CMD_while:
	    حالت CMD_endwhile:
	    حالت CMD_for:
	    حالت CMD_endfor:
	    حالت CMD_if:
	    حالت CMD_elseif:
	    حالت CMD_else:
	    حالت CMD_endif:
	    حالت CMD_try:
	    حالت CMD_catch:
	    حالت CMD_finally:
	    حالت CMD_endtry:
	    حالت CMD_function:
	    حالت CMD_def:
				توڑو;

	    // commands that read a block of lines
	    حالت CMD_abstract:
	    حالت CMD_append:
	    حالت CMD_autocmd:
	    حالت CMD_change:
	    حالت CMD_class:
	    حالت CMD_command:
	    حالت CMD_enum:
	    حالت CMD_داخل_کرو:
	    حالت CMD_interface:
	    حالت CMD_loadkeymap:
				توڑو;

	    // Commands that handle '|' themselves.  Check: A command should
	    // either have the EX_TRLBAR flag, appear in this list or appear in
	    // the list at ":help :bar".
	    حالت CMD_aboveleft:
	    حالت CMD_and:
	    حالت CMD_belowright:
	    حالت CMD_botright:
	    حالت CMD_browse:
	    حالت CMD_call:
	    حالت CMD_confirm:
	    حالت CMD_const:
	    حالت CMD_delfunction:
	    حالت CMD_djump:
	    حالت CMD_dlist:
	    حالت CMD_dsearch:
	    حالت CMD_dsplit:
	    حالت CMD_echo:
	    حالت CMD_echoerr:
	    حالت CMD_echomsg:
	    حالت CMD_echon:
	    حالت CMD_eval:
	    حالت CMD_execute:
	    حالت CMD_filter:
	    حالت CMD_final:
	    حالت CMD_help:
	    حالت CMD_hide:
	    حالت CMD_horizontal:
	    حالت CMD_ijump:
	    حالت CMD_ilist:
	    حالت CMD_isearch:
	    حالت CMD_isplit:
	    حالت CMD_keepalt:
	    حالت CMD_keepjumps:
	    حالت CMD_keepmarks:
	    حالت CMD_keeppatterns:
	    حالت CMD_leftabove:
	    حالت CMD_let:
	    حالت CMD_lockmarks:
	    حالت CMD_lockvar:
	    حالت CMD_lua:
	    حالت CMD_مطابقت:
	    حالت CMD_mzscheme:
	    حالت CMD_noautocmd:
	    حالت CMD_noswapfile:
	    حالت CMD_perl:
	    حالت CMD_psearch:
	    حالت CMD_py3:
	    حالت CMD_python3:
	    حالت CMD_python:
	    حالت CMD_return:
	    حالت CMD_rightbelow:
	    حالت CMD_ruby:
	    حالت CMD_silent:
	    حالت CMD_smagic:
	    حالت CMD_snomagic:
	    حالت CMD_substitute:
	    حالت CMD_syntax:
	    حالت CMD_tab:
	    حالت CMD_tcl:
	    حالت CMD_throw:
	    حالت CMD_tilde:
	    حالت CMD_topleft:
	    حالت CMD_unlet:
	    حالت CMD_unlockvar:
	    حالت CMD_var:
	    حالت CMD_verbose:
	    حالت CMD_vertical:
	    حالت CMD_wincmd:
				توڑو;

	    طےشدہ:		جاؤ doend;
	}
    }
#اگر_ختم

    اگر ((ea.argt & EX_XFILE)
	    && expand_filename(&ea, cmdlinep, &errormsg) == FAIL)
	جاؤ doend;

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (is_export && (ea.argt & EX_EXPORT) == 0)
    {
	emsg(_(e_invalid_command_after_export));
	جاؤ doend;
    }
#اگر_ختم

    /*
     * Accept buffer name.  Cannot be used at the same time with a buffer
     * number.  Don't do this for a user command.
     */
    اگر ((ea.argt & EX_BUFNAME) && *ea.arg != NUL && ea.addr_گنتی == 0
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * :bdelete, :bwipeout and :bunload take several arguments, separated
	 * by spaces: find next space (skipping over escaped characters).
	 * The others take one argument: ignore trailing spaces.
	 */
	اگر (ea.cmdidx == CMD_bdelete || ea.cmdidx == CMD_bwipeout
						  || ea.cmdidx == CMD_bunload)
	    p = skiptowhite_esc(ea.arg);
	ورنہ
	{
	    p = ea.arg + STRLEN(ea.arg);
	    جب (p > ea.arg && VIM_ISWHITE(p[-1]))
		--p;
	}
	ea.line2 = buflist_findpat(ea.arg, p, (ea.argt & EX_BUFUNL) != 0,
								FALSE, FALSE);
	اگر (ea.line2 < 0)	    // failed
	    جاؤ doend;
	ea.addr_گنتی = 1;
	ea.arg = skipwhite(p);
    }

    // The :try command saves the emsg_silent flag, reset it here when
    // ":silent! try" was used, it should only apply to :try itself.
    اگر (ea.cmdidx == CMD_try && cmdmod.cmod_did_esilent > 0)
    {
	emsg_silent -= cmdmod.cmod_did_esilent;
	اگر (emsg_silent < 0)
	    emsg_silent = 0;
	cmdmod.cmod_did_esilent = 0;
    }

/*
 * 7. Execute the command.
 */

    اگر (IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * Execute a user-defined command.
	 */
	do_ucmd(&ea);
    }
    ورنہ
    {
	/*
	 * Call the function to execute the builtin command.
	 */
	(cmdnames[ea.cmdidx].cmd_func)(&ea);
	اگر (ea.errmsg != تہی)
	    errormsg = ea.errmsg;
    }

#اگر_تعریف_شدہ FEAT_EVAL
    // A command will reset "is_export" when exporting an item.  If it is still
    // set something went wrong or the command was never executed.
    اگر (!ea.skip && is_export)
    {
	اگر (errormsg == تہی)
	    errormsg = _(e_export_with_invalid_دلیل);
	is_export = FALSE;
    }

    // Set flag that any command was executed, used by ex_vim9script().
    // Not if this was a command that wasn't executed or :endif.
    اگر (sourcing_a_script(&ea)
	    && current_sctx.sc_sid > 0
	    && ea.cmdidx != CMD_endif
	    && (cstack->cs_idx < 0
		    || (cstack->cs_flags[cstack->cs_idx] & CSF_فعال)))
	SCRIPT_شے(current_sctx.sc_sid)->sn_state = SN_STATE_HAD_COMMAND;

    /*
     * If the command just executed called do_cmdline(), any throw or ":return"
     * or ":finish" encountered there must also check the cstack of the still
     * active do_cmdline() that called this do_one_cmd().  Rethrow an uncaught
     * exception, or reanimate a returned function or finished script file and
     * return or finish it again.
     */
    اگر (need_rethrow)
	do_throw(cstack);
    ورنہ اگر (check_cstack)
    {
	اگر (ماخذ_ختم_شدہ(fgetline, cookie))
	    do_اختتام(&ea, TRUE);
	ورنہ اگر (getline_equal(fgetline, cookie, get_func_سطر)
						   && current_func_returned())
	    do_return(&ea, TRUE, FALSE, تہی);
    }
    need_rethrow = check_cstack = FALSE;
#اگر_ختم

doend:
    اگر (curwin->w_cursor.lnum == 0)	// can happen with zero line number
    {
	curwin->w_cursor.lnum = 1;
	curwin->w_cursor.ستون = 0;
    }

    اگر (errormsg != تہی && *errormsg != NUL && !did_emsg)
    {
	اگر ((sourcing || !کلید_Typed) && !did_append_cmd)
	{
	    اگر (errormsg != (حرف *)IObuff)
	    {
		STRCPY(IObuff, errormsg);
		errormsg = (حرف *)IObuff;
	    }
	    append_command(*cmdlinep);
	}
	emsg(errormsg);
    }
#اگر_تعریف_شدہ FEAT_EVAL
    do_errthrow(cstack,
	    (ea.cmdidx != CMD_سائز && !IS_USER_CMDIDX(ea.cmdidx))
			? cmdnames[(صحیح)ea.cmdidx].cmd_name : (char_u *)تہی);

    اگر (did_سیٹ_اظہاریہ_سطر)
	سیٹ_اظہاریہ_سطر(تہی, تہی);
    is_export = FALSE;
#اگر_ختم

    undo_cmdmod(&cmdmod);
    cmdmod = save_cmdmod;
    reg_executing = save_reg_executing;
    pending_انتہا_reg_executing = save_pending_انتہا_reg_executing;

    اگر (ea.nextcmd && *ea.nextcmd == NUL)	// not really a next command
	ea.nextcmd = تہی;

#اگر_تعریف_شدہ FEAT_EVAL
    --ex_nesting_level;
    vim_free(ea.cmdline_tofree);
#اگر_ختم

    واپس ea.nextcmd;
}

ساکن حرف ex_error_buf[پیغام_BUF_LEN];

/*
 * Return an error message with argument included.
 * Uses a static buffer, only the last error will be kept.
 * "msg" will be translated, caller should use N_().
 */
    حرف *
ex_errmsg(حرف *پیغام, char_u *arg)
{
    vim_snprintf(ex_error_buf, پیغام_BUF_LEN, _(پیغام), arg);
    واپس ex_error_buf;
}

/*
 * The "+" string used in place of an empty command in Ex mode.
 * This string is used in pointer comparison.
 */
ساکن ثابت حرف exmode_plus[] = "+";

/*
 * Handle a range without a command.
 * Returns an error message on failure.
 */
    حرف *
ex_range_without_command(exarg_T *eap)
{
    حرف *errormsg = تہی;

    اگر ((*eap->cmd == '|' ||
		(exmode_فعال && eap->cmd != (char_u *)exmode_plus + 1))
#اگر_تعریف_شدہ FEAT_EVAL
	    && !اندر_vim9script()
#اگر_ختم
       )
    {
	eap->cmdidx = CMD_print;
	eap->argt = EX_RANGE+EX_گنتی+EX_TRLBAR;
	اگر ((errormsg = invalid_range(eap)) == تہی)
	{
	    correct_range(eap);
	    ex_print(eap);
	}
    }
    ورنہ اگر (eap->addr_گنتی != 0)
    {
	اگر (eap->line2 > curbuf->b_ml.ml_سطر_گنتی)
	{
	    // With '-' in 'cpoptions' a line number past the file is an
	    // error, otherwise put it at the end of the file.
	    اگر (vim_strchr(p_cpo, CPO_MINUS) != تہی)
		eap->line2 = -1;
	    ورنہ
		eap->line2 = curbuf->b_ml.ml_سطر_گنتی;
	}

	اگر (eap->line2 < 0)
	    errormsg = _(e_invalid_range);
	ورنہ
	{
	    اگر (eap->line2 == 0)
		curwin->w_cursor.lnum = 1;
	    ورنہ
		curwin->w_cursor.lnum = eap->line2;
	    beginline(BL_SOL | BL_FIX);
	}
    }
    واپس errormsg;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 * If "noparen" is TRUE do not recognize the command followed by "(" or ".".
 */
    ساکن صحیح
checkforcmd_opt(
    char_u	**pp,		// start of command
    حرف	*cmd,		// name of command
    صحیح		len,		// required length
    صحیح		noparen)
{
    صحیح		i;

    برائے (i = 0; cmd[i] != NUL; ++i)
	اگر (((char_u *)cmd)[i] != (*pp)[i])
	    توڑو;
    اگر (i >= len && !ASCII_ISALPHA((*pp)[i]) && (*pp)[i] != '_'
			 && (!noparen || ((*pp)[i] != '(' && (*pp)[i] != '.')))
    {
	*pp = skipwhite(*pp + i);
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    صحیح
checkforcmd(
    char_u	**pp,		// start of command
    حرف	*cmd,		// name of command
    صحیح		len)		// required length
{
    واپس checkforcmd_opt(pp, cmd, len, FALSE);
}

/*
 * Check for an Ex command with optional tail, not followed by "(" or ".".
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    صحیح
checkforcmd_noparen(
    char_u	**pp,		// start of command
    حرف	*cmd,		// name of command
    صحیح		len)		// required length
{
    واپس checkforcmd_opt(pp, cmd, len, TRUE);
}

/*
 * Find the replacement text of a ":command", the part after the attributes and
 * the command name.  Returns NULL when there is none.  Does not modify "arg"
 * and gives no error messages.
 */
    ساکن char_u *
ڈھونڈو_ucmd_repl(char_u *arg)
{
    char_u	*p = arg;

    // Skip over the attributes.
    جب (*p == '-')
	p = skipwhite(skiptowhite(p));

    // Skip over the command name.
    اگر (!ASCII_ISALPHA(*p))
	واپس تہی;
    جب (ASCII_ISALNUM(*p))
	++p;

    واپس *p == NUL ? تہی : skipwhite(p);
}

/*
 * Find the "{" in "line" that starts a block for ":command" or ":autocmd".
 * That is the case when the command argument is "{" at the end of the line,
 * also when the command is nested in another ":command" or ":autocmd".
 * Returns NULL when the line does not start such a block.
 */
    char_u *
ڈھونڈو_cmd_بلاک_آغاز(char_u *سطر)
{
    char_u	*p = skipwhite(سطر);

    برائے (;;)
    {
	char_u	*arg = p;

	اگر (*p == '{' && سرے_excmd2(p, skipwhite(p + 1)))
	    واپس p;

	اگر (checkforcmd_noparen(&arg, "autocmd", 2))
	{
	    اگر (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = au_ڈھونڈو_cmd_arg(arg);
	}
	ورنہ اگر (checkforcmd_noparen(&arg, "command", 3))
	{
	    اگر (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = ڈھونڈو_ucmd_repl(arg);
	}
	ورنہ
	    واپس تہی;

	اگر (p == تہی)
	    واپس تہی;
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
    صحیح
parse_command_modifiers(
	exarg_T	    *eap,
	حرف	    **errormsg,
	cmdmod_T    *cmod,
	صحیح	    skip_only)
{
    char_u  *orig_cmd = eap->cmd;
    char_u  *cmd_آغاز = تہی;
    صحیح	    use_plus_cmd = FALSE;
    صحیح	    starts_with_colon = FALSE;
    صحیح	    vim9script = اندر_vim9script();
    صحیح	    has_visual_range = FALSE;

    CLEAR_اشارہ_گر(cmod);
    cmod->cmod_flags = sticky_cmdmod_flags;

    اگر (STRNCMP(eap->cmd, "'<,'>", 5) == 0)
    {
	// The automatically inserted Visual area range is skipped, so that
	// typing ":cmdmod cmd" in Visual mode works without having to move the
	// range to after the modifiers. The command will be "'<,'>cmdmod cmd",
	// parse "cmdmod cmd" and then put back "'<,'>" before "cmd" below.
	eap->cmd += 5;
	cmd_آغاز = eap->cmd;
	has_visual_range = TRUE;
    }

    // Repeat until no more command modifiers are found.
    برائے (;;)
    {
	char_u  *p;

	جب (*eap->cmd == ' ' || *eap->cmd == '\t' || *eap->cmd == ':')
	{
	    اگر (*eap->cmd == ':')
		starts_with_colon = TRUE;
	    ++eap->cmd;
	}

	// in ex mode, an empty command (after modifiers) works like :+
	اگر (*eap->cmd == NUL && exmode_فعال
		   && (getline_equal(eap->ea_getline, eap->cookie, getexmodeline)
		       || getline_equal(eap->ea_getline, eap->cookie, getexline))
			&& curwin->w_cursor.lnum < curbuf->b_ml.ml_سطر_گنتی)
	{
	    use_plus_cmd = TRUE;
	    اگر (!skip_only)
		ex_pressedreturn = TRUE;
	    توڑو;  // no modifiers following
	}

	// ignore comment and empty lines
	اگر (comment_آغاز(eap->cmd, starts_with_colon))
	{
	    // a comment ends at a NL
	    eap->nextcmd = vim_strchr(eap->cmd, '\n');
	    اگر (eap->nextcmd != تہی)
		++eap->nextcmd;
	    اگر (vim9script)
	    {
		اگر (has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
#اگر_تعریف_شدہ FEAT_EVAL
		اگر (eap->cmd[0] == '#' && eap->cmd[1] == '{'
							 && eap->cmd[2] != '{')
		    *errormsg = _(e_cannot_use_hash_curly_مقصد_آغاز_comment);
#اگر_ختم
	    }
	    واپس FAIL;
	}
	اگر (*eap->cmd == '\n')
	{
	    eap->nextcmd = eap->cmd + 1;
	    واپس FAIL;
	}
	اگر (*eap->cmd == NUL)
	{
	    اگر (!skip_only)
	    {
		ex_pressedreturn = TRUE;
		اگر (vim9script && has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
	    }
	    واپس FAIL;
	}

	p = skip_range(eap->cmd, TRUE, تہی);

	// In Vim9 script a variable can shadow a command modifier:
	//   verbose = 123
	//   verbose += 123
	//   silent! verbose = func()
	//   verbose.member = 2
	//   verbose[expr] = 2
	// But not:
	//   verbose [a, b] = list
	اگر (vim9script)
	{
	    char_u *s, *n;

	    برائے (s = eap->cmd; ASCII_ISALPHA(*s); ++s)
		;
	    n = skipwhite(s);
	    اگر (*n == '.' || *n == '=' || (*n != NUL && n[1] == '=')
		    || *s == '[')
		توڑو;
	}

	انتخاب (*p)
	{
	    // When adding an entry, also modify cmdmod_info_tab[].
	    حالت 'a':	اگر (!checkforcmd_noparen(&eap->cmd, "aboveleft", 3))
			    توڑو;
			cmod->cmod_بانٹو |= WSP_ABOVE;
			جاری;

	    حالت 'b':	اگر (checkforcmd_noparen(&eap->cmd, "belowright", 3))
			{
			    cmod->cmod_بانٹو |= WSP_نیچے;
			    جاری;
			}
			اگر (checkforcmd_opt(&eap->cmd, "browse", 3, TRUE))
			{
#اگر_تعریف_شدہ FEAT_BROWSE_CMD
			    cmod->cmod_flags |= CMOD_BROWSE;
#اگر_ختم
			    جاری;
			}
			اگر (!checkforcmd_noparen(&eap->cmd, "botright", 2))
			    توڑو;
			cmod->cmod_بانٹو |= WSP_BOT;
			جاری;

	    حالت 'c':	اگر (!checkforcmd_opt(&eap->cmd, "confirm", 4, TRUE))
			    توڑو;
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			cmod->cmod_flags |= CMOD_CONFIRM;
#اگر_ختم
			جاری;

	    حالت 'k':	اگر (checkforcmd_noparen(&eap->cmd, "keepmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_KEEPMARKS;
			    جاری;
			}
			اگر (checkforcmd_noparen(&eap->cmd, "keepalt", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPALT;
			    جاری;
			}
			اگر (checkforcmd_noparen(&eap->cmd, "keeppatterns", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPPATTERNS;
			    جاری;
			}
			اگر (!checkforcmd_noparen(&eap->cmd, "keepjumps", 5))
			    توڑو;
			cmod->cmod_flags |= CMOD_KEEPJUMPS;
			جاری;

	    حالت 'f':	// only accept ":filter {pat} cmd"
			{
			    char_u  *reg_pat;
			    char_u  *nulp = تہی;
			    صحیح	    c = 0;

			    اگر (!checkforcmd_noparen(&p, "filter", 4)
				    || *p == NUL
				    || (سرے_excmd(*p)
#اگر_تعریف_شدہ FEAT_EVAL
					// in ":filter #pat# cmd" # does not
					// start a comment
				     && (!vim9script || VIM_ISWHITE(p[1]))
#اگر_ختم
				     ))
				توڑو;
			    اگر (*p == '!')
			    {
				cmod->cmod_filter_force = TRUE;
				p = skipwhite(p + 1);
				اگر (*p == NUL || سرے_excmd(*p))
				    توڑو;
			    }
#اگر_تعریف_شدہ FEAT_EVAL
			    // Avoid that "filter(arg)" is recognized.
			    اگر (vim9script && !VIM_ISWHITE(p[-1]))
				توڑو;
#اگر_ختم
			    اگر (skip_only)
				p = skip_vimgrep_pat(p, تہی, تہی);
			    ورنہ
				// NOTE: This puts a NUL after the pattern.
				p = skip_vimgrep_pat_ext(p, &reg_pat, تہی,
								    &nulp, &c);
			    اگر (p == تہی || *p == NUL)
				توڑو;
			    اگر (!skip_only)
			    {
				cmod->cmod_filter_regmatch.regprog =
						vim_regcomp(reg_pat, RE_MAGIC);
				اگر (cmod->cmod_filter_regmatch.regprog == تہی)
				    توڑو;
				// restore the character overwritten by NUL
				اگر (nulp != تہی)
				    *nulp = c;
			    }
			    eap->cmd = p;
			    جاری;
			}

	    حالت 'h':	اگر (checkforcmd_noparen(&eap->cmd, "horizontal", 3))
			{
			    cmod->cmod_بانٹو |= WSP_HOR;
			    جاری;
			}
			// ":hide" and ":hide | cmd" are not modifiers
			اگر (p != eap->cmd || !checkforcmd_noparen(&p, "hide", 3)
					       || *p == NUL || سرے_excmd(*p))
			    توڑو;
			eap->cmd = p;
			cmod->cmod_flags |= CMOD_HIDE;
			جاری;

	    حالت 'l':	اگر (checkforcmd_noparen(&eap->cmd, "lockmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_LOCKMARKS;
			    جاری;
			}
			اگر (checkforcmd_noparen(&eap->cmd, "legacy", 3))
			{
			    اگر (سرے_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_legacy_must_be_followed_by_command);
				واپس FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_VIM9CMD;
			    cmod->cmod_flags |= CMOD_LEGACY;
			    جاری;
			}

			اگر (!checkforcmd_noparen(&eap->cmd, "leftabove", 5))
			    توڑو;
			cmod->cmod_بانٹو |= WSP_ABOVE;
			جاری;

	    حالت 'n':	اگر (checkforcmd_noparen(&eap->cmd, "noautocmd", 3))
			{
			    cmod->cmod_flags |= CMOD_NOAUTOCMD;
			    جاری;
			}
			اگر (!checkforcmd_noparen(&eap->cmd, "noswapfile", 3))
			    توڑو;
			cmod->cmod_flags |= CMOD_NOSWAPFILE;
			جاری;

	    حالت 'r':	اگر (!checkforcmd_noparen(&eap->cmd, "rightbelow", 6))
			    توڑو;
			cmod->cmod_بانٹو |= WSP_نیچے;
			جاری;

	    حالت 's':	اگر (checkforcmd_noparen(&eap->cmd, "sandbox", 3))
			{
			    cmod->cmod_flags |= CMOD_SANDBOX;
			    جاری;
			}
			اگر (!checkforcmd_noparen(&eap->cmd, "silent", 3))
			    توڑو;
			cmod->cmod_flags |= CMOD_SILENT;
			اگر (*eap->cmd == '!' && !VIM_ISWHITE(eap->cmd[-1]))
			{
			    // ":silent!", but not "silent !cmd"
			    eap->cmd = skipwhite(eap->cmd + 1);
			    cmod->cmod_flags |= CMOD_ERRSILENT;
			}
			جاری;

	    حالت 't':	اگر (checkforcmd_noparen(&p, "tab", 3))
			{
			    اگر (!skip_only)
			    {
				طویل tabnr = get_پتہ(eap, &eap->cmd,
						    ADDR_TABS, eap->skip,
						    skip_only, FALSE, 1);
				اگر (tabnr == MAXLNUM)
				    cmod->cmod_tab = tabpage_اشاریہ(curtab) + 1;
				ورنہ
				{
				    اگر (tabnr < 0 || tabnr > آخری_TAB_NR)
				    {
					*errormsg = _(e_invalid_range);
					واپس FAIL;
				    }
				    cmod->cmod_tab = tabnr + 1;
				}
			    }
			    eap->cmd = p;
			    جاری;
			}
			اگر (!checkforcmd_noparen(&eap->cmd, "topleft", 2))
			    توڑو;
			cmod->cmod_بانٹو |= WSP_چوٹی;
			جاری;

	    حالت 'u':	اگر (!checkforcmd_noparen(&eap->cmd, "unsilent", 3))
			    توڑو;
			cmod->cmod_flags |= CMOD_UNSILENT;
			جاری;

	    حالت 'v':	اگر (checkforcmd_noparen(&eap->cmd, "vertical", 4))
			{
			    cmod->cmod_بانٹو |= WSP_VERT;
			    جاری;
			}
			اگر (checkforcmd_noparen(&eap->cmd, "vim9cmd", 4))
			{
			    اگر (سرے_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_vim9cmd_must_be_followed_by_command);
				واپس FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_LEGACY;
			    cmod->cmod_flags |= CMOD_VIM9CMD;
			    جاری;
			}
			اگر (!checkforcmd_noparen(&p, "verbose", 4))
			    توڑو;
			اگر (vim_isdigit(*eap->cmd))
			{
			    // zero means not set, one is verbose == 0, etc.
			    cmod->cmod_verbose = atoi((حرف *)eap->cmd) + 1;
			}
			ورنہ
			    cmod->cmod_verbose = 2;  // default: verbose == 1
			eap->cmd = p;
			جاری;
	}
	توڑو;
    }

    اگر (has_visual_range)
    {
	اگر (eap->cmd > cmd_آغاز)
	{
	    // Move the '<,'> range to after the modifiers and insert a colon.
	    // Since the modifiers have been parsed put the colon on top of the
	    // space: "'<,'>mod cmd" -> "mod:'<,'>cmd
	    // Put eap->cmd after the colon.
	    اگر (use_plus_cmd)
	    {
		size_t len = STRLEN(cmd_آغاز);

		// Special case: empty command uses "+":
		//  "'<,'>mods" -> "mods *+
		//  Use "*" instead of "'<,'>" to avoid the command getting
		//  longer, in case it was allocated.
		mch_memmove(orig_cmd, cmd_آغاز, len);
		STRCPY(orig_cmd + len, " *+");
	    }
	    ورنہ
	    {
		mch_memmove(cmd_آغاز - 5, cmd_آغاز, eap->cmd - cmd_آغاز);
		eap->cmd -= 5;
		mch_memmove(eap->cmd - 1, ":'<,'>", 6);
	    }
	}
	ورنہ
	    // No modifiers, move the pointer back.
	    // Special case: change empty command to "+".
	    اگر (use_plus_cmd)
		eap->cmd = (char_u *)"'<,'>+";
	    ورنہ
		eap->cmd = orig_cmd;
    }
    ورنہ اگر (use_plus_cmd)
	eap->cmd = (char_u *)exmode_plus;

    واپس OK;
}

/*
 * Return TRUE if "cmod" has anything set.
 */
    صحیح
has_cmdmod(cmdmod_T *cmod, صحیح ignore_silent)
{
    واپس (cmod->cmod_flags != 0 && (!ignore_silent
		|| (cmod->cmod_flags
		      & ~(CMOD_SILENT | CMOD_ERRSILENT | CMOD_UNSILENT)) != 0))
	    || cmod->cmod_بانٹو != 0
	    || cmod->cmod_verbose > 0
	    || cmod->cmod_tab != 0
	    || cmod->cmod_filter_regmatch.regprog != تہی;
}

#اگر defined(FEAT_EVAL)
/*
 * If Vim9 script and "cmdmod" has anything set give an error and return TRUE.
 */
    صحیح
cmdmod_error(صحیح ignore_silent)
{
    اگر (اندر_vim9script() && has_cmdmod(&cmdmod, ignore_silent))
    {
	emsg(_(e_misplaced_command_modifier));
	واپس TRUE;
    }
    واپس FALSE;
}
#اگر_ختم

/*
 * Apply the command modifiers.  Saves current state in "cmdmod", call
 * undo_cmdmod() later.
 */
    خالی
apply_cmdmod(cmdmod_T *cmod)
{
#اگر_تعریف_شدہ HAVE_SANDBOX
    اگر ((cmod->cmod_flags & CMOD_SANDBOX) && !cmod->cmod_did_sandbox)
    {
	++sandbox;
	cmod->cmod_did_sandbox = TRUE;
    }
#اگر_ختم
    اگر (cmod->cmod_verbose > 0)
    {
	اگر (cmod->cmod_verbose_save == 0)
	    cmod->cmod_verbose_save = p_verbose + 1;
	p_verbose = cmod->cmod_verbose - 1;
    }

    اگر ((cmod->cmod_flags & (CMOD_SILENT | CMOD_UNSILENT))
	    && cmod->cmod_save_پیغام_silent == 0)
    {
	cmod->cmod_save_پیغام_silent = پیغام_silent + 1;
	cmod->cmod_save_پیغام_scroll = پیغام_scroll;
    }
    اگر (cmod->cmod_flags & CMOD_SILENT)
	++پیغام_silent;
    اگر (cmod->cmod_flags & CMOD_UNSILENT)
	پیغام_silent = 0;

    اگر (cmod->cmod_flags & CMOD_ERRSILENT)
    {
	++emsg_silent;
	++cmod->cmod_did_esilent;
    }

    اگر ((cmod->cmod_flags & CMOD_NOAUTOCMD) && cmod->cmod_save_ei == تہی)
    {
	// Set 'eventignore' to "all".
	// First save the existing option value for restoring it later.
	cmod->cmod_save_ei = vim_strsave(p_ei);
	سیٹ_string_option_براہ_راست((char_u *)"ei", -1,
					  (char_u *)"all", OPT_FREE, SID_NONE);
    }
}

/*
 * Undo and free contents of "cmod".
 */
    خالی
undo_cmdmod(cmdmod_T *cmod)
{
    اگر (cmod->cmod_verbose_save > 0)
    {
	p_verbose = cmod->cmod_verbose_save - 1;
	cmod->cmod_verbose_save = 0;
    }

#اگر_تعریف_شدہ HAVE_SANDBOX
    اگر (cmod->cmod_did_sandbox)
    {
	--sandbox;
	cmod->cmod_did_sandbox = FALSE;
    }
#اگر_ختم

    اگر (cmod->cmod_save_ei != تہی)
    {
	// Restore 'eventignore' to the value before ":noautocmd".
	سیٹ_string_option_براہ_راست((char_u *)"ei", -1, cmod->cmod_save_ei,
							   OPT_FREE, SID_NONE);
	free_string_option(cmod->cmod_save_ei);
	cmod->cmod_save_ei = تہی;
    }

    vim_regfree(cmod->cmod_filter_regmatch.regprog);

    اگر (cmod->cmod_save_پیغام_silent > 0)
    {
	// messages could be enabled for a serious error, need to check if the
	// counters don't become negative
	اگر (!did_emsg || پیغام_silent > cmod->cmod_save_پیغام_silent - 1)
	    پیغام_silent = cmod->cmod_save_پیغام_silent - 1;
	emsg_silent -= cmod->cmod_did_esilent;
	اگر (emsg_silent < 0)
	    emsg_silent = 0;
	// Restore msg_scroll, it's set by file I/O commands, even when no
	// message is actually displayed.
	پیغام_scroll = cmod->cmod_save_پیغام_scroll;

	// "silent reg" or "silent echo x" inside "redir" leaves msg_col
	// somewhere in the line.  Put it back in the first column.
	اگر (redirecting())
	    پیغام_ستون = 0;

	cmod->cmod_save_پیغام_silent = 0;
	cmod->cmod_did_esilent = 0;
    }
}

/*
 * Parse the address range, if any, in "eap".
 * May set the last search pattern, unless "silent" is TRUE.
 * Return FAIL and set "errormsg" or return OK.
 */
    صحیح
parse_cmd_پتہ(exarg_T *eap, حرف **errormsg, صحیح silent)
{
    صحیح		پتہ_گنتی = 1;
    linenr_T	lnum;
    صحیح		need_check_cursor = FALSE;
    صحیح		ret = FAIL;

    // Repeat for all ',' or ';' separated addresses.
    برائے (;;)
    {
	eap->line1 = eap->line2;
	eap->line2 = default_پتہ(eap);
	eap->cmd = skipwhite(eap->cmd);
	lnum = get_پتہ(eap, &eap->cmd, eap->addr_type, eap->skip, silent,
					eap->addr_گنتی == 0, پتہ_گنتی++);
	اگر (eap->cmd == تہی)	// error detected
	    جاؤ theend;
	اگر (lnum == MAXLNUM)
	{
	    اگر (*eap->cmd == '%')   // '%' - all lines
	    {
		++eap->cmd;
		انتخاب (eap->addr_type)
		{
		    حالت ADDR_سطریں:
		    حالت ADDR_OTHER:
			eap->line1 = 1;
			eap->line2 = curbuf->b_ml.ml_سطر_گنتی;
			توڑو;
		    حالت ADDR_LOADED_BUFFERS:
			{
			    buf_T	*buf = firstbuf;

			    جب (buf->b_اگلا != تہی
						  && buf->b_ml.ml_mfp == تہی)
				buf = buf->b_اگلا;
			    eap->line1 = buf->b_fnum;
			    buf = lastbuf;
			    جب (buf->b_پچھلا != تہی
						  && buf->b_ml.ml_mfp == تہی)
				buf = buf->b_پچھلا;
			    eap->line2 = buf->b_fnum;
			    توڑو;
			}
		    حالت ADDR_BUFFERS:
			eap->line1 = firstbuf->b_fnum;
			eap->line2 = lastbuf->b_fnum;
			توڑو;
		    حالت ADDR_WINDOWS:
		    حالت ADDR_TABS:
			اگر (IS_USER_CMDIDX(eap->cmdidx))
			{
			    eap->line1 = 1;
			    eap->line2 = eap->addr_type == ADDR_WINDOWS
						  ? آخری_WIN_NR : آخری_TAB_NR;
			}
			ورنہ
			{
			    // there is no Vim command which uses '%' and
			    // ADDR_WINDOWS or ADDR_TABS
			    *errormsg = _(e_invalid_range);
			    جاؤ theend;
			}
			توڑو;
		    حالت ADDR_TABS_RELATIVE:
		    حالت ADDR_UNSIGNED:
		    حالت ADDR_QUICKFIX:
			*errormsg = _(e_invalid_range);
			جاؤ theend;
		    حالت ADDR_ARGUMENTS:
			اگر (ARGCOUNT == 0)
			    eap->line1 = eap->line2 = 0;
			ورنہ
			{
			    eap->line1 = 1;
			    eap->line2 = ARGCOUNT;
			}
			توڑو;
		    حالت ADDR_QUICKFIX_VALID:
#اگر_تعریف_شدہ FEAT_QUICKFIX
			eap->line1 = 1;
			eap->line2 = qf_get_valid_سائز(eap);
			اگر (eap->line2 == 0)
			    eap->line2 = 1;
#اگر_ختم
			توڑو;
		    حالت ADDR_NONE:
			// Will give an error later if a range is found.
			توڑو;
		}
		++eap->addr_گنتی;
	    }
	    ورنہ اگر (*eap->cmd == '*' && vim_strchr(p_cpo, CPO_ستارہ) == تہی)
	    {
		pos_T	    *fp;

		// '*' - visual area
		اگر (eap->addr_type != ADDR_سطریں)
		{
		    *errormsg = _(e_invalid_range);
		    جاؤ theend;
		}

		++eap->cmd;
		اگر (!eap->skip)
		{
		    fp = getmark('<', FALSE);
		    اگر (check_mark(fp) == FAIL)
			جاؤ theend;
		    eap->line1 = fp->lnum;
		    fp = getmark('>', FALSE);
		    اگر (check_mark(fp) == FAIL)
			جاؤ theend;
		    eap->line2 = fp->lnum;
		    ++eap->addr_گنتی;
		}
	    }
	}
	ورنہ
	    eap->line2 = lnum;
	eap->addr_گنتی++;

	اگر (*eap->cmd == ';')
	{
	    اگر (!eap->skip)
	    {
		curwin->w_cursor.lnum = eap->line2;

		// Don't leave the cursor on an illegal line or column, but do
		// accept zero as address, so 0;/PATTERN/ works correctly
		// (where zero usually means to use the first line).
		// Check the cursor position before returning.
		اگر (eap->line2 > 0)
		    check_cursor();
		ورنہ
		    check_cursor_ستون();
		need_check_cursor = TRUE;
	    }
	}
	ورنہ اگر (*eap->cmd != ',')
	    توڑو;
	++eap->cmd;
    }

    // One address given: set start and end lines.
    اگر (eap->addr_گنتی == 1)
    {
	eap->line1 = eap->line2;
	// ... but only implicit: really no address given
	اگر (lnum == MAXLNUM)
	    eap->addr_گنتی = 0;
    }
    ret = OK;

theend:
    اگر (need_check_cursor)
	check_cursor();
    واپس ret;
}

/*
 * Append "cmd" to the error message in IObuff.
 * Takes care of limiting the length and handling 0xa0, which would be
 * invisible otherwise.
 */
    ساکن خالی
append_command(char_u *cmd)
{
    size_t  len = STRLEN(IObuff);
    char_u  *s = cmd;
    char_u  *d;

    اگر (len > IOSIZE - 100)
    {
	// Not enough space, truncate and put in "...".
	d = IObuff + IOSIZE - 100;
	d -= mb_سر_off(IObuff, d);
	STRCPY(d, "...");
    }
    STRCAT(IObuff, ": ");
    d = IObuff + STRLEN(IObuff);
    جب (*s != NUL && d - IObuff + 5 < IOSIZE)
    {
	اگر (enc_utf8 ? (s[0] == 0xc2 && s[1] == 0xa0) : *s == 0xa0)
	{
	    s += enc_utf8 ? 2 : 1;
	    STRCPY(d, "<a0>");
	    d += 4;
	}
	ورنہ اگر (d - IObuff + (*mb_ptr2len)(s) + 1 >= IOSIZE)
	    توڑو;
	ورنہ
	    MB_COPY_CHAR(s, d);
    }
    *d = NUL;
}

#اگر defined(FEAT_EVAL)
/*
 * If "start" points "&opt", "&l:opt", "&g:opt" or "$ENV" return a pointer to
 * the name.  Otherwise just return "start".
 */
    char_u *
skip_option_env_lead(char_u *آغاز)
{
    char_u *name = آغاز;

    اگر (*آغاز == '&')
    {
	اگر ((آغاز[1] == 'l' || آغاز[1] == 'g') && آغاز[2] == ':')
	    name += 3;
	ورنہ
	    name += 1;
    }
    ورنہ اگر (*آغاز == '$')
	name += 1;
    واپس name;
}
#اگر_ختم

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
    ساکن صحیح
one_letter_cmd(char_u *p, cmdidx_T *idx)
{
    اگر (اندر_vim9script())
	واپس FALSE;
    اگر (p[0] == 'k'
	    && (p[1] != 'e' || (p[1] == 'e' && p[2] != 'e')))
    {
	*idx = CMD_k;
	واپس TRUE;
    }
    اگر (p[0] == 's'
	    && ((p[1] == 'c' && (p[2] == NUL || (p[2] != 's' && p[2] != 'r'
			&& (p[3] == NUL || (p[3] != 'i' && p[4] != 'p')))))
		|| p[1] == 'g'
		|| (p[1] == 'i' && p[2] != 'm' && p[2] != 'l' && p[2] != 'g')
		|| p[1] == 'I'
		|| (p[1] == 'r' && p[2] != 'e')))
    {
	*idx = CMD_substitute;
	واپس TRUE;
    }
    واپس FALSE;
}

#اگر defined(FEAT_EVAL)
/*
 * Return TRUE if "cmd" starts with "123->", a number followed by a method
 * call.
 */
    صحیح
نمبر_طریقہ(char_u *cmd)
{
    char_u *p = skipdigits(cmd);

    واپس p > cmd && (p = skipwhite(p))[0] == '-' && p[1] == '>';
}
#اگر_ختم

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
ڈھونڈو_ex_command(
	exarg_T *eap,
	صحیح	*بھرا غیر_مستعمل,
	صحیح	(*lookup)(char_u *, size_t, صحیح cmd, cctx_T *) غیر_مستعمل,
	cctx_T	*cctx غیر_مستعمل)
{
    صحیح		len;
    char_u	*p;
    صحیح		i;
#اگر_غیر_تعریف_شدہ FEAT_EVAL
    صحیح		vim9 = FALSE;
#ورنہ
    صحیح		vim9 = اندر_vim9script();

    /*
     * Recognize a Vim9 script function/method call and assignment:
     * "lvar = value", "lvar(arg)", "[1, 2 3]->Func()"
     */
    p = eap->cmd;
    اگر (lookup != تہی)
    {
	char_u *pskip = skip_option_env_lead(eap->cmd);

	اگر (vim_strchr((char_u *)"{('[\"@&$", *p) != تہی
	       || ((p = مقصد_name_const_انتہا(pskip)) > eap->cmd && *p != NUL)
	       || (p[0] == '0' && p[1] == 'z'))
	{
	    صحیح	    oplen;
	    صحیح	    heredoc;
	    char_u  *swp;

	    اگر (*eap->cmd == '&'
		    || (eap->cmd[0] == '$'
				  && eap->cmd[1] != '\'' && eap->cmd[1] != '"')
		    || (eap->cmd[0] == '@'
					&& (valid_yank_reg(eap->cmd[1], FALSE)
						       || eap->cmd[1] == '@')))
	    {
		اگر (*eap->cmd == '&')
		{
		    p = eap->cmd + 1;
		    اگر (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
			p += 2;
		    p = مقصد_name_انتہا(p, FALSE);
		}
		ورنہ اگر (*eap->cmd == '$')
		    p = مقصد_name_انتہا(eap->cmd + 1, FALSE);
		ورنہ
		    p = eap->cmd + 2;
		اگر (سرے_excmd(*skipwhite(p)))
		{
		    // "&option <NL>", "$ENV <NL>" and "@r <NL>" are the start
		    // of an expression.
		    eap->cmdidx = CMD_eval;
		    واپس eap->cmd;
		}
		// "&option" can be followed by "->" or "=", check below
	    }

	    اگر (vim9 && *p == '<')
	    {
		// generic function type args
		اگر (skip_generic_func_type_دلائل(&p) == FAIL)
		{
		    eap->cmdidx = CMD_سائز;
		    واپس p;
		}
	    }

	    swp = skipwhite(p);

	    اگر (
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
		اگر (*eap->cmd == '{' && سرے_excmd(*skipwhite(eap->cmd + 1)))
		{
		    // "{" by itself is the start of a block.
		    eap->cmdidx = CMD_بلاک;
		    واپس eap->cmd + 1;
		}
		eap->cmdidx = CMD_eval;
		واپس eap->cmd;
	    }

	    اگر ((p != eap->cmd && (
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
		اگر (skip_اظہاریہ(&after, تہی) == OK)
		{
		    after = skipwhite(after);
		    اگر (*after == '=' || (*after != NUL && after[1] == '=')
					 || (after[0] == '.' && after[1] == '.'
							   && after[2] == '='))
			eap->cmdidx = CMD_var;
		}
		--emsg_off;
		واپس eap->cmd;
	    }

	    // "[...]->Method()" is a list expression, but "[a, b] = Func()" is
	    // an assignment.
	    // If there is no line break inside the "[...]" then "p" is
	    // advanced to after the "]" by to_name_const_end(): check if a "="
	    // follows.
	    // If "[...]" has a line break "p" still points at the "[" and it
	    // can't be an assignment.
	    اگر (*eap->cmd == '[')
	    {
		char_u	    *eq;

		p = مقصد_name_const_انتہا(eap->cmd);
		اگر (p == eap->cmd && *p == '[')
		{
		    صحیح گنتی = 0;
		    صحیح	semicolon = FALSE;

		    p = skip_var_list(eap->cmd, TRUE, &گنتی, &semicolon, TRUE);
		}
		eq = p;
		اگر (eq != تہی)
		{
		    eq = skipwhite(eq);
		    اگر (vim_strchr((char_u *)"+-*/%.", *eq) != تہی)
		    {
			اگر (eq[0] == '.' && eq[1] == '.')
			    ++eq;
			++eq;
		    }
		}
		اگر (p == تہی || p == eap->cmd || *eq != '=')
		{
		    eap->cmdidx = CMD_eval;
		    واپس eap->cmd;
		}
		اگر (p > eap->cmd && *eq == '=')
		{
		    eap->cmdidx = CMD_var;
		    واپس eap->cmd;
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
	    اگر (oplen > 0)
	    {
		اگر (((p - eap->cmd) > 2 && eap->cmd[1] == ':')
			|| *eap->cmd == '&'
			|| *eap->cmd == '$'
			|| *eap->cmd == '@'
			|| (eap->skip && IS_WHITE_OR_NUL(
						      *(skipwhite(p) + oplen)))
			|| lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK)
		{
		    eap->cmdidx = CMD_var;
		    واپس eap->cmd;
		}
	    }

	    // Recognize trying to use a type for a w:, b:, t: or g: variable:
	    // "w:varname: number = 123".
	    اگر (eap->cmd[1] == ':' && *p == ':')
	    {
		eap->cmdidx = CMD_var;
		واپس eap->cmd;
	    }
	}

	// 1234->func() is a method call
	اگر (نمبر_طریقہ(eap->cmd))
	{
	    eap->cmdidx = CMD_eval;
	    واپس eap->cmd;
	}

	// "g:", "s:" and "l:" are always assumed to be a variable, thus start
	// an expression.  A global/substitute/list command needs to use a
	// longer name.
	اگر (vim_strchr((char_u *)"gsl", *p) != تہی && p[1] == ':')
	{
	    eap->cmdidx = CMD_eval;
	    واپس eap->cmd;
	}

	// If it is an ID it might be a variable with an operator on the next
	// line, if the variable exists it can't be an Ex command.
	اگر (p > eap->cmd && سرے_excmd(*skipwhite(p))
		&& (lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK
		    || (ASCII_ISALPHA(eap->cmd[0]) && eap->cmd[1] == ':')))
	{
	    eap->cmdidx = CMD_eval;
	    واپس eap->cmd;
	}

	// Check for "++nr" and "--nr".
	اگر (p == eap->cmd && p[0] != NUL && p[0] == p[1]
						   && (*p == '+' || *p == '-'))
	{
	    eap->cmdidx = *p == '+' ? CMD_increment : CMD_decrement;
	    واپس eap->cmd + 2;
	}
    }
#اگر_ختم

    /*
     * Isolate the command and search for it in the command table.
     */
    p = eap->cmd;
    اگر (one_letter_cmd(p, &eap->cmdidx))
    {
	++p;
	اگر (بھرا != تہی)
	    *بھرا = TRUE;
    }
    ورنہ
    {
	جب (ASCII_ISALPHA(*p))
	    ++p;
	// for python 3.x support ":py3", ":python3", ":py3file", etc.
	اگر (eap->cmd[0] == 'p' && eap->cmd[1] == 'y')
	{
	    جب (ASCII_ISALNUM(*p))
		++p;
	}
	ورنہ اگر (*p == '9' && STRNCMP("vim9", eap->cmd, 4) == 0)
	{
	    // include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	    ++p;
	    جب (ASCII_ISALPHA(*p))
		++p;
	}

	// check for non-alpha command
	اگر (p == eap->cmd && vim_strchr((char_u *)"@*!=><&~#}", *p) != تہی)
	    ++p;
	len = (صحیح)(p - eap->cmd);
	// The "d" command can directly be followed by 'l' or 'p' flag, when
	// not in Vim9 script.
	اگر (!vim9 && *eap->cmd == 'd' && (p[-1] == 'l' || p[-1] == 'p'))
	{
	    // Check for ":dl", ":dell", etc. to ":deletel": that's
	    // :delete with the 'l' flag.  Same for 'p'.
	    برائے (i = 0; i < len; ++i)
		اگر (eap->cmd[i] != ((char_u *)"delete")[i])
		    توڑو;
	    اگر (i == len - 1)
	    {
		--len;
		اگر (p[-1] == 'l')
		    eap->flags |= EXFLAG_LIST;
		ورنہ
		    eap->flags |= EXFLAG_PRINT;
	    }
	}

	اگر (ASCII_ISLOWER(eap->cmd[0]))
	{
	    صحیح c1 = eap->cmd[0];
	    صحیح c2 = len == 1 ? NUL : eap->cmd[1];

	    اگر (command_گنتی != (صحیح)CMD_سائز)
	    {
		iemsg(e_command_جدول_needs_مقصد_be_updated_تسلسل_make_cmdidxs);
		getout(1);
	    }

	    // Use a precomputed index for fast look-up in cmdnames[]
	    // taking into account the first 2 letters of eap->cmd.
	    eap->cmdidx = cmdidxs1[Char_Ord_کم(c1)];
	    اگر (ASCII_ISLOWER(c2))
		eap->cmdidx += cmdidxs2[Char_Ord_کم(c1)][Char_Ord_کم(c2)];
	}
	ورنہ اگر (ASCII_ISUPPER(eap->cmd[0]))
	    eap->cmdidx = CMD_اگلا_3;
	ورنہ
	    eap->cmdidx = CMD_bang;

	برائے ( ; (صحیح)eap->cmdidx < (صحیح)CMD_سائز;
			       eap->cmdidx = (cmdidx_T)((صحیح)eap->cmdidx + 1))
	    اگر (STRNCMP(cmdnames[(صحیح)eap->cmdidx].cmd_name, (حرف *)eap->cmd,
							    (size_t)len) == 0)
	    {
#اگر_تعریف_شدہ FEAT_EVAL
		اگر (بھرا != تہی && cmdnames[eap->cmdidx].cmd_name[len] == NUL)
		    *بھرا = TRUE;
#اگر_ختم
		توڑو;
	    }

	// :Print and :mode are not supported in Vim9 script.
	// Some commands cannot be shortened in Vim9 script.
	اگر (vim9 && eap->cmdidx != CMD_سائز)
	{
	    اگر (eap->cmdidx == CMD_منوال || eap->cmdidx == CMD_Print)
		eap->cmdidx = CMD_سائز;
	    ورنہ اگر ((cmdnames[eap->cmdidx].cmd_argt & EX_پورا)
			  && len < (صحیح)cmdnames[eap->cmdidx].cmd_namelen)
	    {
		semsg(_(e_command_cannot_be_shortened_str), eap->cmd);
		eap->cmdidx = CMD_سائز;
	    }
	}

	// Do not recognize ":*" as the star command unless '*' is in
	// 'cpoptions'.
	اگر (eap->cmdidx == CMD_ستارہ && vim_strchr(p_cpo, CPO_ستارہ) == تہی)
	    p = eap->cmd;

	// Look for a user defined command as a last resort.  Let ":Print" be
	// overruled by a user defined command.
	اگر ((eap->cmdidx == CMD_سائز || eap->cmdidx == CMD_Print)
		&& *eap->cmd >= 'A' && *eap->cmd <= 'Z')
	{
	    // User defined commands may contain digits.
	    جب (ASCII_ISALNUM(*p))
		++p;
	    p = ڈھونڈو_ucmd(eap, p, بھرا, تہی, تہی);
	}
	اگر (p == تہی || p == eap->cmd)
	    eap->cmdidx = CMD_سائز;
    }

    // ":fina" means ":finally" in legacy script, for backwards compatibility.
    اگر (eap->cmdidx == CMD_final && p - eap->cmd == 4 && !vim9)
	eap->cmdidx = CMD_finally;

     // Force ":ho" to be unresolved.  Without this, find_ex_command()
     // matches it to CMD_horizontal (the only "ho*" entry), which makes
     // fullcommand("ho") return "horizontal" even though ":ho" cannot be
     // used as the modifier (cmdmods[] requires 3 chars, "hor").
    اگر (eap->cmdidx == CMD_horizontal && p - eap->cmd == 2)
	eap->cmdidx = CMD_سائز;

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (eap->cmdidx < CMD_سائز
	    && vim9
	    && !IS_WHITE_NL_OR_NUL(*p) && *p != '!' && *p != '|'
	    && (eap->cmdidx < 0 ||
		(cmdnames[eap->cmdidx].cmd_argt & EX_NONWHITE_OK) == 0))
    {
	// A name that goes on with an underscore is not this command with an
	// argument: "ch_log" is not ":change".  Leave it to be reported as an
	// invalid command.
	اگر (*p != '_')
	{
	    char_u *cmd = vim_strnsave(eap->cmd, p - eap->cmd);

	    semsg(_(e_command_str_not_followed_by_white_space_str), cmd,
								    eap->cmd);
	    vim_free(cmd);
	}
	eap->cmdidx = CMD_سائز;
    }
#اگر_ختم

    واپس p;
}

#اگر defined(FEAT_EVAL)
قسمتعریف ساخت
{
    حرف	*name;
    صحیح		minlen;
    صحیح		has_گنتی;  // :123verbose  :3tab
} cmdmod_info_T;

ساکن cmdmod_info_T cmdmod_info_tab[] = {
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
    صحیح
modifier_len(char_u *cmd)
{
    صحیح		i, j;
    char_u	*p = cmd;

    اگر (VIM_ISDIGIT(*cmd))
	p = skipwhite(skipdigits(cmd + 1));
    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(cmdmod_info_tab); ++i)
    {
	// cmdmod_info_tab[] is sorted by name: once the first letter is past
	// the command's first letter no later entry can match.
	اگر (cmdmod_info_tab[i].name[0] > *p)
	    توڑو;
	برائے (j = 0; p[j] != NUL; ++j)
	    اگر (p[j] != cmdmod_info_tab[i].name[j])
		توڑو;
	اگر (!ASCII_ISALPHA(p[j]) && j >= cmdmod_info_tab[i].minlen
					&& (p == cmd || cmdmod_info_tab[i].has_گنتی))
	    واپس j + (صحیح)(p - cmd);
    }
    واپس 0;
}

/*
 * Return > 0 if an Ex command "name" exists.
 * Return 2 if there is an exact match.
 * Return 3 if there is an ambiguous match.
 */
    صحیح
cmd_exists(char_u *name)
{
    exarg_T	ea;
    صحیح		بھرا = FALSE;
    صحیح		i;
    صحیح		j;
    char_u	*p;

    // Check command modifiers.
    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(cmdmod_info_tab); ++i)
    {
	برائے (j = 0; name[j] != NUL; ++j)
	    اگر (name[j] != cmdmod_info_tab[i].name[j])
		توڑو;
	اگر (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	    واپس (cmdmod_info_tab[i].name[j] == NUL ? 2 : 1);
    }

    // Check built-in commands and user defined commands.
    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    p = ڈھونڈو_ex_command(&ea, &بھرا, تہی, تہی);
    اگر (p == تہی)
	واپس 3;
    اگر (vim_isdigit(*name) && ea.cmdidx != CMD_مطابقت)
	واپس 0;
    اگر (*skipwhite(p) != NUL)
	واپس 0;	// trailing garbage
    واپس (ea.cmdidx == CMD_سائز ? 0 : (بھرا ? 2 : 1));
}

/*
 * Add the attributes of an Ex command to "d" in the terms of ":command":
 * "argt" are the EX_ flags, "addr_type" the address type and "def" the
 * default for a range or count, -1 when there is none.
 */
    خالی
ex_command_attrs(dict_T *d, long_u argt, cmd_addr_T addr_type, طویل def)
{
    ساکن حرف *addr_نام[] = {
	"lines", "windows", "arguments", "loaded_buffers", "buffers", "tabs",
	"tabs_relative", "quickfix_valid", "quickfix", "unsigned", "other",
	"none"
    };
    حرف	*nargs;

    اگر (!(argt & EX_EXTRA))
	nargs = "0";
    ورنہ اگر (!(argt & EX_NOSPC))
	nargs = (argt & EX_NEEDARG) ? "+" : "*";
    ورنہ اگر (!(argt & EX_NEEDARG))
	nargs = "?";
    ورنہ
	nargs = (argt & EX_ARGSPACE) ? "_" : "1";
    dict_جوڑو_string(d, "nargs", (char_u *)nargs);

    اگر (!(argt & EX_RANGE))
	dict_جوڑو_bool(d, "range", FALSE);
    ورنہ اگر (argt & EX_DFLALL)
	dict_جوڑو_string(d, "range", (char_u *)"%");
    ورنہ اگر (def >= 0 && !(argt & EX_گنتی))
	dict_جوڑو_نمبر(d, "range", def);
    ورنہ
	dict_جوڑو_string(d, "range", (char_u *)".");
    اگر (!(argt & EX_گنتی))
	dict_جوڑو_bool(d, "count", FALSE);
    ورنہ
	dict_جوڑو_نمبر(d, "count", def < 0 ? 0 : def);
    dict_جوڑو_bool(d, "bang", (argt & EX_BANG) != 0);
    dict_جوڑو_bool(d, "bar", (argt & EX_TRLBAR) != 0);
    dict_جوڑو_bool(d, "register", (argt & EX_REGSTR) != 0);
    dict_جوڑو_string(d, "addr", (char_u *)addr_نام[addr_type]);
}

/*
 * Add what getinfo() reports for the Ex command "name" to "d".  "vim9" is TRUE
 * to find the command as in Vim9 script, FALSE as in legacy script, -1 for the
 * context of the caller.
 * Returns FAIL when there is no such command or the abbreviation is
 * ambiguous.
 */
    صحیح
ex_command_info(char_u *name, صحیح vim9, dict_T *d)
{
    exarg_T	ea;
    char_u	*p;
    صحیح		i;
    صحیح		j;
    صحیح		save_cmod_flags = cmdmod.cmod_flags;
    صحیح		ret = OK;

    اگر (vim9 != -1)
    {
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9 ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    // An abbreviation of a command modifier, like ":bo" for ":botright".
    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(cmdmod_info_tab); ++i)
    {
	برائے (j = 0; name[j] != NUL; ++j)
	    اگر (name[j] != cmdmod_info_tab[i].name[j])
		توڑو;
	اگر (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	{
	    name = (char_u *)cmdmod_info_tab[i].name;
	    توڑو;
	}
    }

    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    ea.addr_گنتی = 0;
    // Don't complain about using "en" or ":let" in Vim9 script.
    ++emsg_silent;
    p = ڈھونڈو_ex_command(&ea, تہی, تہی, تہی);
    اگر (p == تہی || ea.cmdidx == CMD_سائز
	    || (vim_isdigit(*name) && ea.cmdidx != CMD_مطابقت)
	    || *skipwhite(p) != NUL
	    || not_اندر_vim9(&ea, true) == FAIL)
	ret = FAIL;
    --emsg_silent;

    اگر (ret == FAIL)
	;
    ورنہ اگر (IS_USER_CMDIDX(ea.cmdidx))
	ret = user_command_info(ea.useridx, ea.cmdidx, d);
    ورنہ
    {
	صحیح	notimpl = cmdnames[ea.cmdidx].cmd_func == ex_ni
# ifdef HAVE_EX_SCRIPT_NI
			    || cmdnames[ea.cmdidx].cmd_func == ex_script_ni
# endif
			    ;

	dict_جوڑو_string(d, "name", cmdnames[ea.cmdidx].cmd_name);
	dict_جوڑو_string(d, "kind", (char_u *)(
		    cmdnames[ea.cmdidx].cmd_func == ex_wrongmodifier
						    ? "modifier" : "builtin"));
	dict_جوڑو_bool(d, "available", !notimpl);
	ex_command_attrs(d, cmdnames[ea.cmdidx].cmd_argt,
				       cmdnames[ea.cmdidx].cmd_addr_type, -1);
    }
    cmdmod.cmod_flags = save_cmod_flags;
    واپس ret;
}

/*
 * "fullcommand" function
 */
    خالی
f_fullcommand(typval_T *argvars, typval_T *rettv)
{
    exarg_T	ea;
    char_u	*name;
    char_u	*p;
    صحیح		vim9script = اندر_vim9script();
    صحیح		save_cmod_flags = cmdmod.cmod_flags;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	واپس;

    name = tv_get_string(&argvars[0]);
    اگر (name == تہی)
	واپس;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	vim9script = tv_get_bool(&argvars[1]);
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9script ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    جب (*name == ':')
	name++;
    name = skip_range(name, TRUE, تہی);

    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.addr_گنتی = 0;
    ++emsg_silent;  // don't complain about using "en" in Vim9 script
    p = ڈھونڈو_ex_command(&ea, تہی, تہی, تہی);
    --emsg_silent;
    اگر (p == تہی || ea.cmdidx == CMD_سائز)
	جاؤ theend;

    اگر (vim9script && not_اندر_vim9(&ea, true) == FAIL)
	جاؤ theend;

    rettv->vval.v_string = vim_strsave(IS_USER_CMDIDX(ea.cmdidx)
				 ? get_user_command_name(ea.useridx, ea.cmdidx)
				 : cmdnames[ea.cmdidx].cmd_name);
theend:
    cmdmod.cmod_flags = save_cmod_flags;
}
#اگر_ختم

    cmdidx_T
excmd_get_cmdidx(char_u *cmd, صحیح len)
{
    cmdidx_T idx;

    اگر (!one_letter_cmd(cmd, &idx))
	برائے (idx = (cmdidx_T)0; (صحیح)idx < (صحیح)CMD_سائز;
		idx = (cmdidx_T)((صحیح)idx + 1))
	    اگر (STRNCMP(cmdnames[(صحیح)idx].cmd_name, cmd, (size_t)len) == 0)
		توڑو;

    واپس idx;
}

    طویل
excmd_get_argt(cmdidx_T idx)
{
    واپس (طویل)cmdnames[(صحیح)idx].cmd_argt;
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
    char_u	*cmd_آغاز,
    صحیح		skip_ستارہ,	// skip "*" used for Visual range
    صحیح		*ctx)		// pointer to xp_context or NULL
{
    char_u	*cmd = cmd_آغاز;
    غیرعلامتی	delim;

    جب (vim_strchr((char_u *)" \t0123456789.$%'/?-+,;\\", *cmd) != تہی)
    {
	اگر (*cmd == '\\')
	{
	    اگر (cmd[1] == '?' || cmd[1] == '/' || cmd[1] == '&')
		++cmd;
	    ورنہ
		توڑو;
	}
	ورنہ اگر (*cmd == '\'')
	{
	    char_u *p = cmd;

	    // a quote is only valid at the start or after a separator
	    جب (p > cmd_آغاز)
	    {
		--p;
		اگر (!VIM_ISWHITE(*p))
		    توڑو;
	    }
	    اگر (cmd > cmd_آغاز && !VIM_ISWHITE(*p) && *p != ',' && *p != ';')
		توڑو;
	    اگر (*++cmd == NUL && ctx != تہی)
		*ctx = EXPAND_NOTHING;
	}
	ورنہ اگر (*cmd == '/' || *cmd == '?')
	{
	    delim = *cmd++;
	    جب (*cmd != NUL && *cmd != delim)
		اگر (*cmd++ == '\\' && *cmd != NUL)
		    ++cmd;
	    اگر (*cmd == NUL && ctx != تہی)
		*ctx = EXPAND_NOTHING;
	}
	اگر (*cmd != NUL)
	    ++cmd;
    }

    // Skip ":" and white space.
    جب (*cmd == ':')
	cmd = skipwhite(cmd + 1);

    // Skip "*" used for Visual range.
    اگر (skip_ستارہ && *cmd == '*' && vim_strchr(p_cpo, CPO_ستارہ) == تہی)
	cmd = skipwhite(cmd + 1);

    واپس cmd;
}

    ساکن خالی
addr_error(cmd_addr_T addr_type)
{
    اگر (addr_type == ADDR_NONE)
	emsg(_(e_no_range_allowed));
    ورنہ
	emsg(_(e_invalid_range));
}

/*
 * Return the default address for an address type.
 */
    ساکن linenr_T
default_پتہ(exarg_T *eap)
{
    linenr_T lnum = 0;

    انتخاب (eap->addr_type)
    {
	حالت ADDR_سطریں:
	حالت ADDR_OTHER:
	    // Default is the cursor line number.  Avoid using an invalid
	    // line number though.
	    اگر (curwin->w_cursor.lnum > curbuf->b_ml.ml_سطر_گنتی)
		lnum = curbuf->b_ml.ml_سطر_گنتی;
	    ورنہ
		lnum = curwin->w_cursor.lnum;
	    توڑو;
	حالت ADDR_WINDOWS:
	    lnum = CURRENT_WIN_NR;
	    توڑو;
	حالت ADDR_ARGUMENTS:
	    lnum = curwin->w_arg_idx + 1;
	    اگر (lnum > ARGCOUNT)
		lnum = ARGCOUNT;
	    توڑو;
	حالت ADDR_LOADED_BUFFERS:
	حالت ADDR_BUFFERS:
	    lnum = curbuf->b_fnum;
	    توڑو;
	حالت ADDR_TABS:
	    lnum = CURRENT_TAB_NR;
	    توڑو;
	حالت ADDR_TABS_RELATIVE:
	حالت ADDR_UNSIGNED:
	    lnum = 1;
	    توڑو;
	حالت ADDR_QUICKFIX:
#اگر_تعریف_شدہ FEAT_QUICKFIX
	    lnum = qf_get_cur_idx(eap);
#اگر_ختم
	    توڑو;
	حالت ADDR_QUICKFIX_VALID:
#اگر_تعریف_شدہ FEAT_QUICKFIX
	    lnum = qf_get_cur_valid_idx(eap);
#اگر_ختم
	    توڑو;
	حالت ADDR_NONE:
	    // Will give an error later if a range is found.
	    توڑو;
    }
    واپس lnum;
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
get_پتہ(
    exarg_T	*eap غیر_مستعمل,
    char_u	**ptr,
    cmd_addr_T	addr_type,
    صحیح		skip,		// only skip the address, don't use it
    صحیح		silent,		// no errors or side effects
    صحیح		مقصد_other_file,  // flag: may jump to other file
    صحیح		پتہ_گنتی غیر_مستعمل) // 1 for first address, >1 after comma
{
    صحیح		c;
    صحیح		i;
    طویل	n;
    char_u	*cmd;
    pos_T	pos;
    pos_T	*fp;
    linenr_T	lnum;
    buf_T	*buf;

    cmd = skipwhite(*ptr);
    lnum = MAXLNUM;
    کرو
    {
	انتخاب (*cmd)
	{
	    حالت '.':			    // '.' - Cursor position
		++cmd;
		انتخاب (addr_type)
		{
		    حالت ADDR_سطریں:
		    حالت ADDR_OTHER:
			lnum = curwin->w_cursor.lnum;
			توڑو;
		    حالت ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			توڑو;
		    حالت ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			توڑو;
		    حالت ADDR_LOADED_BUFFERS:
		    حالت ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			توڑو;
		    حالت ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			توڑو;
		    حالت ADDR_NONE:
		    حالت ADDR_TABS_RELATIVE:
		    حالت ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = تہی;
			جاؤ error;
			توڑو;
		    حالت ADDR_QUICKFIX:
#اگر_تعریف_شدہ FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#اگر_ختم
			توڑو;
		    حالت ADDR_QUICKFIX_VALID:
#اگر_تعریف_شدہ FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#اگر_ختم
			توڑو;
		}
		توڑو;

	    حالت '$':			    // '$' - last line
		++cmd;
		انتخاب (addr_type)
		{
		    حالت ADDR_سطریں:
		    حالت ADDR_OTHER:
			lnum = curbuf->b_ml.ml_سطر_گنتی;
			توڑو;
		    حالت ADDR_WINDOWS:
			lnum = آخری_WIN_NR;
			توڑو;
		    حالت ADDR_ARGUMENTS:
			lnum = ARGCOUNT;
			توڑو;
		    حالت ADDR_LOADED_BUFFERS:
			buf = lastbuf;
			جب (buf->b_ml.ml_mfp == تہی)
			{
			    اگر (buf->b_پچھلا == تہی)
				توڑو;
			    buf = buf->b_پچھلا;
			}
			lnum = buf->b_fnum;
			توڑو;
		    حالت ADDR_BUFFERS:
			lnum = lastbuf->b_fnum;
			توڑو;
		    حالت ADDR_TABS:
			lnum = آخری_TAB_NR;
			توڑو;
		    حالت ADDR_NONE:
		    حالت ADDR_TABS_RELATIVE:
		    حالت ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = تہی;
			جاؤ error;
			توڑو;
		    حالت ADDR_QUICKFIX:
#اگر_تعریف_شدہ FEAT_QUICKFIX
			lnum = qf_get_سائز(eap);
			اگر (lnum == 0)
			    lnum = 1;
#اگر_ختم
			توڑو;
		    حالت ADDR_QUICKFIX_VALID:
#اگر_تعریف_شدہ FEAT_QUICKFIX
			lnum = qf_get_valid_سائز(eap);
			اگر (lnum == 0)
			    lnum = 1;
#اگر_ختم
			توڑو;
		}
		توڑو;

	    حالت '\'':			    // ''' - mark
		اگر (*++cmd == NUL)
		{
		    cmd = تہی;
		    جاؤ error;
		}
		اگر (addr_type != ADDR_سطریں)
		{
		    addr_error(addr_type);
		    cmd = تہی;
		    جاؤ error;
		}
		اگر (skip)
		    ++cmd;
		ورنہ
		{
		    // Only accept a mark in another file when it is
		    // used by itself: ":'M".
		    fp = getmark(*cmd, مقصد_other_file && cmd[1] == NUL);
		    ++cmd;
		    اگر (fp == (pos_T *)-1)
			// Jumped to another file.
			lnum = curwin->w_cursor.lnum;
		    ورنہ
		    {
			اگر (check_mark(fp) == FAIL)
			{
			    cmd = تہی;
			    جاؤ error;
			}
			lnum = fp->lnum;
		    }
		}
		توڑو;

	    حالت '/':
	    حالت '?':			// '/' or '?' - search
		c = *cmd++;
		اگر (addr_type != ADDR_سطریں)
		{
		    addr_error(addr_type);
		    cmd = تہی;
		    جاؤ error;
		}
		اگر (skip)	// skip "/pat/"
		{
		    cmd = skip_regexp(cmd, c, magic_isset());
		    اگر (*cmd == c)
			++cmd;
		}
		ورنہ
		{
		    صحیح flags;

		    pos = curwin->w_cursor; // save curwin->w_cursor

		    // When '/' or '?' follows another address, start from
		    // there.
		    اگر (lnum > 0 && lnum != MAXLNUM)
			curwin->w_cursor.lnum =
				lnum > curbuf->b_ml.ml_سطر_گنتی
					   ? curbuf->b_ml.ml_سطر_گنتی : lnum;

		    // Start a forward search at the end of the line (unless
		    // before the first line).
		    // Start a backward search at the start of the line.
		    // This makes sure we never match in the current
		    // line, and can match anywhere in the
		    // next/previous line.
		    اگر (c == '/' && curwin->w_cursor.lnum > 0)
			curwin->w_cursor.ستون = MAXCOL;
		    ورنہ
			curwin->w_cursor.ستون = 0;
		    searchcmdlen = 0;
		    flags = silent ? تلاش_KEEP : تلاش_HIS | تلاش_پیغام;
		    اگر (!do_تلاش(تہی, c, c, cmd, STRLEN(cmd), 1L, flags, تہی))
		    {
			curwin->w_cursor = pos;
			cmd = تہی;
			جاؤ error;
		    }
		    lnum = curwin->w_cursor.lnum;
		    curwin->w_cursor = pos;
		    // adjust command string pointer
		    cmd += searchcmdlen;
		}
		توڑو;

	    حالت '\\':		    // "\?", "\/" or "\&", repeat search
		++cmd;
		اگر (addr_type != ADDR_سطریں)
		{
		    addr_error(addr_type);
		    cmd = تہی;
		    جاؤ error;
		}
		اگر (*cmd == '&')
		    i = RE_SUBST;
		ورنہ اگر (*cmd == '?' || *cmd == '/')
		    i = RE_تلاش;
		ورنہ
		{
		    emsg(_(e_backslash_should_be_followed_by));
		    cmd = تہی;
		    جاؤ error;
		}

		اگر (!skip)
		{
		    /*
		     * When search follows another address, start from
		     * there.
		     */
		    اگر (lnum != MAXLNUM)
			pos.lnum = lnum;
		    ورنہ
			pos.lnum = curwin->w_cursor.lnum;

		    /*
		     * Start the search just like for the above
		     * do_search().
		     */
		    اگر (*cmd != '?')
			pos.ستون = MAXCOL;
		    ورنہ
			pos.ستون = 0;
		    pos.coladd = 0;
		    اگر (searchit(curwin, curbuf, &pos, تہی,
				*cmd == '?' ? BACKWARD : FORWARD,
				(char_u *)"", 0, 1L, تلاش_پیغام, i, تہی) != FAIL)
			lnum = pos.lnum;
		    ورنہ
		    {
			cmd = تہی;
			جاؤ error;
		    }
		}
		++cmd;
		توڑو;

	    طےشدہ:
		اگر (VIM_ISDIGIT(*cmd))	// absolute line number
		    lnum = getdigits(&cmd);
	}

	برائے (;;)
	{
	    cmd = skipwhite(cmd);
	    اگر (*cmd != '-' && *cmd != '+' && !VIM_ISDIGIT(*cmd))
		توڑو;

	    اگر (lnum == MAXLNUM)
	    {
		انتخاب (addr_type)
		{
		    حالت ADDR_سطریں:
		    حالت ADDR_OTHER:
			// "+1" is same as ".+1"
			lnum = curwin->w_cursor.lnum;
			توڑو;
		    حالت ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			توڑو;
		    حالت ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			توڑو;
		    حالت ADDR_LOADED_BUFFERS:
		    حالت ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			توڑو;
		    حالت ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			توڑو;
		    حالت ADDR_TABS_RELATIVE:
			lnum = 1;
			توڑو;
		    حالت ADDR_QUICKFIX:
#اگر_تعریف_شدہ FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#اگر_ختم
			توڑو;
		    حالت ADDR_QUICKFIX_VALID:
#اگر_تعریف_شدہ FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#اگر_ختم
			توڑو;
		    حالت ADDR_NONE:
		    حالت ADDR_UNSIGNED:
			lnum = 0;
			توڑو;
		}
	    }

	    اگر (VIM_ISDIGIT(*cmd))
		i = '+';		// "number" is same as "+number"
	    ورنہ
		i = *cmd++;
	    اگر (!VIM_ISDIGIT(*cmd))	// '+' is '+1'
		n = 1;
	    ورنہ
	    {
		// "number", "+number" or "-number"
		n = getdigits(&cmd);
		اگر (n == MAXLNUM)
		{
		    emsg(_(e_سطر_نمبر_برآمد_کا_range));
		    cmd = تہی;
		    جاؤ error;
		}
	    }

	    اگر (addr_type == ADDR_TABS_RELATIVE)
	    {
		emsg(_(e_invalid_range));
		cmd = تہی;
		جاؤ error;
	    }
	    ورنہ اگر (addr_type == ADDR_LOADED_BUFFERS
		    || addr_type == ADDR_BUFFERS)
		lnum = compute_بفر_local_گنتی(
				    addr_type, lnum, (i == '-') ? -1 * n : n);
	    ورنہ
	    {
#اگر_تعریف_شدہ FEAT_FOLDING
		// Relative line addressing: need to adjust for lines in a
		// closed fold after the first address.
		اگر (addr_type == ADDR_سطریں && (i == '-' || i == '+')
							 && پتہ_گنتی >= 2)
		    (خالی)hasFolding(lnum, تہی, &lnum);
#اگر_ختم
		اگر (i == '-')
		    lnum -= n;
		ورنہ
		{
		    اگر (lnum >= 0 && n >= LONG_MAX - lnum)
		    {
			emsg(_(e_سطر_نمبر_برآمد_کا_range));
			cmd = تہی;
			جاؤ error;
		    }
		    lnum += n;
		}
	    }
	}
    } جب (*cmd == '/' || *cmd == '?');

error:
    *ptr = cmd;
    واپس lnum;
}

/*
 * Set eap->line1 and eap->line2 to the whole range.
 * Used for commands with the EX_DFLALL flag and no range given.
 */
    ساکن خالی
پتہ_default_all(exarg_T *eap)
{
    eap->line1 = 1;
    انتخاب (eap->addr_type)
    {
	حالت ADDR_سطریں:
	حالت ADDR_OTHER:
	    eap->line2 = curbuf->b_ml.ml_سطر_گنتی;
	    توڑو;
	حالت ADDR_LOADED_BUFFERS:
	    {
		buf_T *buf = firstbuf;

		جب (buf->b_اگلا != تہی && buf->b_ml.ml_mfp == تہی)
		    buf = buf->b_اگلا;
		eap->line1 = buf->b_fnum;
		buf = lastbuf;
		جب (buf->b_پچھلا != تہی && buf->b_ml.ml_mfp == تہی)
		    buf = buf->b_پچھلا;
		eap->line2 = buf->b_fnum;
	    }
	    توڑو;
	حالت ADDR_BUFFERS:
	    eap->line1 = firstbuf->b_fnum;
	    eap->line2 = lastbuf->b_fnum;
	    توڑو;
	حالت ADDR_WINDOWS:
	    eap->line2 = آخری_WIN_NR;
	    توڑو;
	حالت ADDR_TABS:
	    eap->line2 = آخری_TAB_NR;
	    توڑو;
	حالت ADDR_TABS_RELATIVE:
	    eap->line2 = 1;
	    توڑو;
	حالت ADDR_ARGUMENTS:
	    اگر (ARGCOUNT == 0)
		eap->line1 = eap->line2 = 0;
	    ورنہ
		eap->line2 = ARGCOUNT;
	    توڑو;
	حالت ADDR_QUICKFIX_VALID:
#اگر_تعریف_شدہ FEAT_QUICKFIX
	    eap->line2 = qf_get_valid_سائز(eap);
	    اگر (eap->line2 == 0)
		eap->line2 = 1;
#اگر_ختم
	    توڑو;
	حالت ADDR_NONE:
	حالت ADDR_UNSIGNED:
	حالت ADDR_QUICKFIX:
	    iemsg("Cannot use EX_DFLALL with ADDR_NONE, ADDR_UNSIGNED or ADDR_QUICKFIX");
	    توڑو;
    }
}


/*
 * Get flags from an Ex command argument.
 */
    ساکن خالی
get_flags(exarg_T *eap)
{
    جب (vim_strchr((char_u *)"lp#", *eap->arg) != تہی)
    {
	اگر (*eap->arg == 'l')
	    eap->flags |= EXFLAG_LIST;
	ورنہ اگر (*eap->arg == 'p')
	    eap->flags |= EXFLAG_PRINT;
	ورنہ
	    eap->flags |= EXFLAG_NR;
	eap->arg = skipwhite(eap->arg + 1);
    }
}

/*
 * Function called for command which is Not Implemented.  NI!
 */
    خالی
ex_ni(exarg_T *eap)
{
    اگر (!eap->skip)
	eap->errmsg =
		_(e_sorry_command_is_not_دستیاب_اندر_this_نسخہ);
}

#اگر_تعریف_شدہ HAVE_EX_SCRIPT_NI
/*
 * Function called for script command which is Not Implemented.  NI!
 * Skips over ":perl <<EOF" constructs.
 */
    ساکن خالی
ex_script_ni(exarg_T *eap)
{
    اگر (!eap->skip)
	ex_ni(eap);
    ورنہ
	vim_free(script_get(eap, eap->arg));
}
#اگر_ختم

/*
 * Check range in Ex command for validity.
 * Return NULL when valid, error message when invalid.
 */
    ساکن حرف *
invalid_range(exarg_T *eap)
{
    buf_T	*buf;

    اگر (       eap->line1 < 0
	    || eap->line2 < 0
	    || eap->line1 > eap->line2)
	واپس _(e_invalid_range);

    اگر (eap->argt & EX_RANGE)
    {
	انتخاب (eap->addr_type)
	{
	    حالت ADDR_سطریں:
		اگر (eap->line2 > curbuf->b_ml.ml_سطر_گنتی
#اگر_تعریف_شدہ FEAT_DIFF
			    + (eap->cmdidx == CMD_diffget ||
				eap->cmdidx == CMD_diffput)
#اگر_ختم
		   )
		    واپس _(e_invalid_range);
		توڑو;
	    حالت ADDR_ARGUMENTS:
		// add 1 if ARGCOUNT is 0
		اگر (eap->line2 > ARGCOUNT + (!ARGCOUNT))
		    واپس _(e_invalid_range);
		توڑو;
	    حالت ADDR_BUFFERS:
		// Only a boundary check, not whether the buffers actually
		// exist.
		اگر (eap->line1 < 1 || eap->line2 > get_highest_fnum())
		    واپس _(e_invalid_range);
		توڑو;
	    حالت ADDR_LOADED_BUFFERS:
		buf = firstbuf;
		جب (buf->b_ml.ml_mfp == تہی)
		{
		    اگر (buf->b_اگلا == تہی)
			واپس _(e_invalid_range);
		    buf = buf->b_اگلا;
		}
		اگر (eap->line1 < buf->b_fnum)
		    واپس _(e_invalid_range);
		buf = lastbuf;
		جب (buf->b_ml.ml_mfp == تہی)
		{
		    اگر (buf->b_پچھلا == تہی)
			واپس _(e_invalid_range);
		    buf = buf->b_پچھلا;
		}
		اگر (eap->line2 > buf->b_fnum)
		    واپس _(e_invalid_range);
		توڑو;
	    حالت ADDR_WINDOWS:
		اگر (eap->line2 > آخری_WIN_NR)
		    واپس _(e_invalid_range);
		توڑو;
	    حالت ADDR_TABS:
		اگر (eap->line2 > آخری_TAB_NR)
		    واپس _(e_invalid_range);
		توڑو;
	    حالت ADDR_TABS_RELATIVE:
	    حالت ADDR_OTHER:
		// Any range is OK.
		توڑو;
	    حالت ADDR_QUICKFIX:
#اگر_تعریف_شدہ FEAT_QUICKFIX
		// No error for value that is too big, will use the last entry.
		اگر (eap->line2 <= 0)
		{
		    اگر (eap->addr_گنتی == 0)
			واپس _(e_no_errors);
		    واپس _(e_invalid_range);
		}
#اگر_ختم
		توڑو;
	    حالت ADDR_QUICKFIX_VALID:
#اگر_تعریف_شدہ FEAT_QUICKFIX
		اگر ((eap->line2 != 1 && eap->line2 > qf_get_valid_سائز(eap))
			|| eap->line2 < 0)
		    واپس _(e_invalid_range);
#اگر_ختم
		توڑو;
	    حالت ADDR_UNSIGNED:
	    حالت ADDR_NONE:
		// Will give an error elsewhere.
		توڑو;
	}
    }
    واپس تہی;
}

/*
 * Correct the range for zero line number, if required.
 */
    ساکن خالی
correct_range(exarg_T *eap)
{
    اگر (!(eap->argt & EX_ZEROR))	    // zero in range not allowed
    {
	اگر (eap->line1 == 0)
	    eap->line1 = 1;
	اگر (eap->line2 == 0)
	    eap->line2 = 1;
    }
}

#اگر_تعریف_شدہ FEAT_QUICKFIX
/*
 * For a ":vimgrep" or ":vimgrepadd" command return a pointer past the
 * pattern.  Otherwise return eap->arg.
 */
    ساکن char_u *
skip_grep_pat(exarg_T *eap)
{
    char_u	*p = eap->arg;

    اگر (*p != NUL && (eap->cmdidx == CMD_vimgrep || eap->cmdidx == CMD_lvimgrep
		|| eap->cmdidx == CMD_vimgrepadd
		|| eap->cmdidx == CMD_lvimgrepadd
		|| grep_اندرونی(eap->cmdidx)))
    {
	p = skip_vimgrep_pat(p, تہی, تہی);
	اگر (p == تہی)
	    p = eap->arg;
    }
    واپس p;
}

/*
 * For the ":make" and ":grep" commands insert the 'makeprg'/'grepprg' option
 * in the command line, so that things like % get expanded.
 */
    ساکن char_u *
replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep)
{
    char_u	*new_cmdline;
    char_u	*پروگرام;
    char_u	*pos;
    char_u	*ptr;
    صحیح		len;
    صحیح		i;

    /*
     * Don't do it when ":vimgrep" is used for ":grep".
     */
    اگر ((eap->cmdidx == CMD_make || eap->cmdidx == CMD_lmake
		     || eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
		     || eap->cmdidx == CMD_grepadd
		     || eap->cmdidx == CMD_lgrepadd)
	    && !grep_اندرونی(eap->cmdidx))
    {
	اگر (eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
	    || eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	{
	    اگر (*curbuf->b_p_gp == NUL)
		پروگرام = p_gp;
	    ورنہ
		پروگرام = curbuf->b_p_gp;
	}
	ورنہ
	{
	    اگر (*curbuf->b_p_mp == NUL)
		پروگرام = p_mp;
	    ورنہ
		پروگرام = curbuf->b_p_mp;
	}

	p = skipwhite(p);

	اگر ((pos = (char_u *)لڑی_تلاش((حرف *)پروگرام, "$*")) != تہی)
	{
	    // replace $* by given arguments
	    i = 1;
	    جب ((pos = (char_u *)لڑی_تلاش((حرف *)pos + 2, "$*")) != تہی)
		++i;
	    len = (صحیح)STRLEN(p);
	    new_cmdline = alloc(STRLEN(پروگرام) + (size_t)i * (len - 2) + 1);
	    اگر (new_cmdline == تہی)
		واپس تہی;			// out of memory
	    ptr = new_cmdline;
	    جب ((pos = (char_u *)لڑی_تلاش((حرف *)پروگرام, "$*")) != تہی)
	    {
		i = (صحیح)(pos - پروگرام);
		STRNCPY(ptr, پروگرام, i);
		STRCPY(ptr += i, p);
		ptr += len;
		پروگرام = pos + 2;
	    }
	    STRCPY(ptr, پروگرام);
	}
	ورنہ
	{
	    size_t پروگرام_len = STRLEN(پروگرام);

	    new_cmdline = alloc(پروگرام_len + STRLEN(p) + 2);
	    اگر (new_cmdline == تہی)
		واپس تہی;			// out of memory
	    STRCPY(new_cmdline, پروگرام);
	    STRCPY(new_cmdline + پروگرام_len, " ");
	    STRCPY(new_cmdline + پروگرام_len + 1, p);
	}
	پیغام_make(p);

	// 'eap->cmd' is not set here, because it is not used at CMD_make
	vim_free(*cmdlinep);
	*cmdlinep = new_cmdline;
	p = new_cmdline;
    }
    واپس p;
}
#اگر_ختم

/*
 * Expand file name in Ex command argument.
 * When an error is detected, "errormsgp" is set to a non-NULL pointer.
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
expand_filename(
    exarg_T	*eap,
    char_u	**cmdlinep,
    حرف	**errormsgp)
{
    صحیح		has_wildcards;	// need to expand wildcards
    char_u	*repl;
    size_t	srclen;
    char_u	*p;
    صحیح		n;
    صحیح		escaped;

#اگر_تعریف_شدہ FEAT_QUICKFIX
    // Skip a regexp pattern for ":vimgrep[add] pat file..."
    p = skip_grep_pat(eap);
#ورنہ
    p = eap->arg;
#اگر_ختم

    /*
     * Decide to expand wildcards *before* replacing '%', '#', etc.  If
     * the file name contains a wildcard it should not cause expanding.
     * (it will be expanded anyway if there is a wildcard before replacing).
     */
    has_wildcards = mch_has_wildcard(p);
    جب (*p != NUL)
    {
#اگر_تعریف_شدہ FEAT_EVAL
	// Skip over `=expr`, wildcards in it are not expanded.
	اگر (p[0] == '`' && p[1] == '=')
	{
	    p += 2;
	    (خالی)skip_اظہاریہ(&p, تہی);
	    اگر (*p == '`')
		++p;
	    جاری;
	}
#اگر_ختم
	/*
	 * Quick check if this cannot be the start of a special string.
	 * Also removes backslash before '%', '#' and '<'.
	 */
	اگر (vim_strchr((char_u *)"%#<", *p) == تہی)
	{
	    ++p;
	    جاری;
	}

	/*
	 * Try to find a match at this position.
	 */
	repl = eval_vars(p, eap->arg, &srclen, &(eap->do_ecmd_lnum),
						    errormsgp, &escaped, TRUE);
	اگر (*errormsgp != تہی)		// error detected
	    واپس FAIL;
	اگر (repl == تہی)		// no match found
	{
	    p += srclen;
	    جاری;
	}

	// Wildcards won't be expanded below, the replacement is taken
	// literally.  But do expand "~/file", "~user/file" and "$HOME/file".
	اگر (vim_strchr(repl, '$') != تہی || vim_strchr(repl, '~') != تہی)
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
	اگر (!eap->usefilter
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
#اگر_غیر_تعریف_شدہ UNIX
		&& !(eap->argt & EX_NOSPC)
#اگر_ختم
		)
	{
	    char_u	*l;
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	    // Don't escape a backslash here, because rem_backslash() doesn't
	    // remove it later.
	    ساکن char_u *nobslash = (char_u *)" \t\"|";
# define ESCAPE_CHARS nobslash
#ورنہ
# define ESCAPE_CHARS escape_chars
#اگر_ختم

	    برائے (l = repl; *l; ++l)
		اگر (vim_strchr(ESCAPE_CHARS, *l) != تہی)
		{
		    l = vim_strsave_escaped(repl, ESCAPE_CHARS);
		    اگر (l != تہی)
		    {
			vim_free(repl);
			repl = l;
		    }
		    توڑو;
		}
	}

	// For a shell command a '!' must be escaped.
	اگر ((eap->usefilter || eap->cmdidx == CMD_bang
						|| eap->cmdidx == CMD_terminal)
			    && vim_strpbrk(repl, (char_u *)"!") != تہی)
	{
	    char_u	*l;

	    l = vim_strsave_escaped(repl, (char_u *)"!");
	    اگر (l != تہی)
	    {
		vim_free(repl);
		repl = l;
	    }
	}

	p = repl_cmdline(eap, p, srclen, repl, cmdlinep);
	vim_free(repl);
	اگر (p == تہی)
	    واپس FAIL;
    }

    /*
     * One file argument: Expand wildcards.
     * Don't do this with ":r !command" or ":w !command".
     */
    اگر ((eap->argt & EX_NOSPC) && !eap->usefilter)
    {
	/*
	 * May do this twice:
	 * 1. Replace environment variables.
	 * 2. Replace any other wildcards, remove backslashes.
	 */
	برائے (n = 1; n <= 2; ++n)
	{
	    اگر (n == 2)
	    {
		/*
		 * Halve the number of backslashes (this is Vi compatible).
		 * For Unix and OS/2, when wildcards are expanded, this is
		 * done by ExpandOne() below.
		 */
#اگر defined(UNIX)
		اگر (!has_wildcards)
#اگر_ختم
		    backslash_halve(eap->arg);
	    }

	    اگر (has_wildcards)
	    {
		اگر (n == 1)
		{
		    /*
		     * First loop: May expand environment variables.  This
		     * can be done much faster with expand_env() than with
		     * something else (e.g., calling a shell).
		     * After expanding environment variables, check again
		     * if there are still wildcards present.
		     */
		    اگر (vim_strchr(eap->arg, '$') != تہی
			    || vim_strchr(eap->arg, '~') != تہی)
		    {
			expand_env_esc(eap->arg, NameBuff, MAXPATHL,
					(char_u *)(" \t" راستہ_ESC_WILDCARDS),
					TRUE, تہی);
			has_wildcards = mch_has_wildcard(NameBuff);
			p = NameBuff;
		    }
		    ورنہ
			p = تہی;
		}
		ورنہ // n == 2
		{
		    expand_T	xpc;
		    صحیح		options = WILD_LIST_NOTFOUND
					       | WILD_NOERROR | WILD_جوڑو_SLASH;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    اگر (p_wic)
			options += WILD_ICASE;
		    p = ExpandOne(&xpc, eap->arg, تہی,
						   options, WILD_EXPAND_FREE);
		    اگر (p == تہی)
			واپس FAIL;
		}
		اگر (p != تہی)
		{
		    (خالی)repl_cmdline(eap, eap->arg, STRLEN(eap->arg),
								 p, cmdlinep);
		    اگر (n == 2)	// p came from ExpandOne()
			vim_free(p);
		}
	    }
	}
    }
    واپس OK;
}

/*
 * Replace part of the command line, keeping eap->cmd, eap->arg and
 * eap->nextcmd correct.
 * "src" points to the part that is to be replaced, of length "srclen".
 * "repl" is the replacement string.
 * Returns a pointer to the character after the replaced string.
 * Returns NULL for failure.
 */
    ساکن char_u *
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
    اگر (eap->nextcmd != تہی)
	i += STRLEN(eap->nextcmd);	// add space for next command
    اگر ((new_cmdline = alloc(i)) == تہی)
	واپس تہی;			// out of memory!

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

    اگر (eap->nextcmd != تہی)		// append next command
    {
	new_cmdlinelen += taillen + 1;
	STRCPY(new_cmdline + new_cmdlinelen, eap->nextcmd);
	eap->nextcmd = new_cmdline + new_cmdlinelen;
    }
    eap->cmd = new_cmdline + (eap->cmd - *cmdlinep);
    eap->arg = new_cmdline + (eap->arg - *cmdlinep);
    اگر (eap->do_ecmd_cmd != تہی && eap->do_ecmd_cmd != dollar_command)
	eap->do_ecmd_cmd = new_cmdline + (eap->do_ecmd_cmd - *cmdlinep);
    vim_free(*cmdlinep);
    *cmdlinep = new_cmdline;

    واپس src;
}

/*
 * Check for '|' to separate commands and '"' to start comments.
 * If "keep_backslash" is TRUE do not remove any backslash.
 */
    خالی
separate_nextcmd(exarg_T *eap, صحیح keep_backslash)
{
    صحیح		vim9script = اندر_vim9script();
    char_u	*p;

#اگر_تعریف_شدہ FEAT_QUICKFIX
    p = skip_grep_pat(eap);
#ورنہ
    p = eap->arg;
#اگر_ختم

    برائے ( ; *p; MB_PTR_ADV(p))
    {
	اگر (*p == Ctrl_V)
	{
	    اگر ((eap->argt & (EX_CTRLV | EX_XFILE)) || keep_backslash)
		++p;		// skip CTRL-V and next char
	    ورنہ
				// remove CTRL-V and skip next char
		STRMOVE(p, p + 1);
	    اگر (*p == NUL)		// stop at NUL after CTRL-V
		توڑو;
	}

#اگر_تعریف_شدہ FEAT_EVAL
	// Skip over `=expr` when wildcards are expanded.
	ورنہ اگر (p[0] == '`' && p[1] == '=' && (eap->argt & EX_XFILE))
	{
	    p += 2;
	    (خالی)skip_اظہاریہ(&p, تہی);
	    اگر (*p == NUL)		// stop at NUL after CTRL-V
		توڑو;
	}
#اگر_ختم

	// Check for '"'/'#': start of comment or '|': next command
	// :@" and :*" do not start a comment!
	// :redir @" doesn't either.
	ورنہ اگر ((*p == '"'
		    && !vim9script
		    && !(eap->argt & EX_NOTRLCOM)
		    && ((eap->cmdidx != CMD_at && eap->cmdidx != CMD_ستارہ)
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
		    && eap->cmdidx != CMD_داخل_کرو)
		|| *p == '\n')
	{
	    /*
	     * We remove the '\' before the '|', unless EX_CTRLV is used
	     * AND 'b' is present in 'cpoptions'.
	     */
	    اگر ((vim_strchr(p_cpo, CPO_BAR) == تہی
			      || !(eap->argt & EX_CTRLV)) && *(p - 1) == '\\')
	    {
		اگر (!keep_backslash)
		{
		    STRMOVE(p - 1, p);	// remove the '\'
		    --p;
		}
	    }
	    ورنہ
	    {
		سیٹ_nextcmd(eap, p);
		*p = NUL;
		توڑو;
	    }
	}
    }

    اگر (!(eap->argt & EX_NOTRLCOM))	// remove trailing spaces
	del_trailing_spaces(eap->arg);
}

/*
 * get + command from ex argument
 */
    ساکن char_u *
getargcmd(char_u **argp)
{
    char_u *arg = *argp;
    char_u *command = تہی;

    اگر (*arg == '+')	    // +[command]
    {
	++arg;
	اگر (vim_isspace(*arg) || *arg == NUL)
	    command = dollar_command;
	ورنہ
	{
	    command = arg;
	    arg = skip_cmd_arg(command, TRUE);
	    اگر (*arg != NUL)
		*arg++ = NUL;		// terminate command with NUL
	}

	arg = skipwhite(arg);	// skip over spaces
	*argp = arg;
    }
    واپس command;
}

/*
 * Find end of "+command" argument.  Skip over "\ " and "\\".
 */
    char_u *
skip_cmd_arg(
    char_u *p,
    صحیح	   rembs)	// TRUE to halve the number of backslashes
{
    جب (*p && !vim_isspace(*p))
    {
	اگر (*p == '\\' && p[1] != NUL)
	{
	    اگر (rembs)
		STRMOVE(p, p + 1);
	    ورنہ
		++p;
	}
	MB_PTR_ADV(p);
    }
    واپس p;
}

    صحیح
get_bad_opt(char_u *p, exarg_T *eap)
{
    اگر (STRICMP(p, "keep") == 0)
	eap->bad_char = BAD_KEEP;
    ورنہ اگر (STRICMP(p, "drop") == 0)
	eap->bad_char = BAD_DROP;
    ورنہ اگر (MB_BYTE2LEN(*p) == 1 && p[1] == NUL)
	eap->bad_char = *p;
    ورنہ
	واپس FAIL;
    واپس OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of bad= names.
 */
    ساکن char_u *
get_bad_name(expand_T *xp غیر_مستعمل, صحیح idx)
{
    // Note: Keep this in sync with getargopt.
    ساکن حرف *(p_bad_اقدار[]) =
    {
	"?",
	"keep",
	"drop",
    };

    اگر (idx < 0 || idx >= (صحیح)ARRAY_لمبائی(p_bad_اقدار))
	واپس تہی;

    واپس (char_u*)p_bad_اقدار[idx];
}

/*
 * Get "++opt=arg" argument.
 * Return FAIL or OK.
 */
    ساکن صحیح
getargopt(exarg_T *eap)
{
    char_u	*arg = eap->arg + 2;
    صحیح		*pp = تہی;
    صحیح		bad_char_idx;
    char_u	*p;

    // Note: Keep this in sync with get_argopt_name.

    // ":edit ++[no]bin[ary] file"
    اگر (STRNCMP(arg, "bin", 3) == 0 || STRNCMP(arg, "nobin", 5) == 0)
    {
	اگر (*arg == 'n')
	{
	    arg += 2;
	    eap->force_bin = FORCE_NOBIN;
	}
	ورنہ
	    eap->force_bin = FORCE_BIN;
	اگر (!checkforcmd(&arg, "binary", 3))
	    واپس FAIL;
	eap->arg = skipwhite(arg);
	واپس OK;
    }

    // ":read ++edit file"
    اگر (STRNCMP(arg, "edit", 4) == 0)
    {
	eap->read_edit = TRUE;
	eap->arg = skipwhite(arg + 4);
	واپس OK;
    }

    اگر (STRNCMP(arg, "ff", 2) == 0)
    {
	arg += 2;
	pp = &eap->force_ff;
    }
    ورنہ اگر (STRNCMP(arg, "fileformat", 10) == 0)
    {
	arg += 10;
	pp = &eap->force_ff;
    }
    ورنہ اگر (STRNCMP(arg, "enc", 3) == 0)
    {
	اگر (STRNCMP(arg, "encoding", 8) == 0)
	    arg += 8;
	ورنہ
	    arg += 3;
	pp = &eap->force_enc;
    }
    ورنہ اگر (STRNCMP(arg, "bad", 3) == 0)
    {
	arg += 3;
	pp = &bad_char_idx;
    }

    اگر (pp == تہی || *arg != '=')
	واپس FAIL;

    ++arg;
    *pp = (صحیح)(arg - eap->cmd);
    arg = skip_cmd_arg(arg, FALSE);
    eap->arg = skipwhite(arg);
    *arg = NUL;

    اگر (pp == &eap->force_ff)
    {
	اگر (check_ff_قدر(eap->cmd + eap->force_ff) == FAIL)
	    واپس FAIL;
	eap->force_ff = eap->cmd[eap->force_ff];
    }
    ورنہ اگر (pp == &eap->force_enc)
    {
	// Make 'fileencoding' lower case.
	برائے (p = eap->cmd + eap->force_enc; *p != NUL; ++p)
	    *p = TOLOWER_ASC(*p);
    }
    ورنہ
    {
	// Check ++bad= argument.  Must be a single-byte character, "keep" or
	// "drop".
	اگر (get_bad_opt(eap->cmd + bad_char_idx, eap) == FAIL)
	    واپس FAIL;
    }

    واپس OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of ++opt names.
 */
    ساکن char_u *
get_argopt_name(expand_T *xp غیر_مستعمل, صحیح idx)
{
    // Note: Keep this in sync with getargopt.
    ساکن حرف *(p_opt_اقدار[]) =
    {
	"fileformat=",
	"encoding=",
	"binary",
	"nobinary",
	"bad=",
	"edit",
    };

    اگر (idx < 0 || idx >= (صحیح)ARRAY_لمبائی(p_opt_اقدار))
	واپس تہی;

    واپس (char_u*)p_opt_اقدار[idx];
}

/*
 * Command-line expansion for ++opt=name.
 */
    صحیح
expand_argopt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	صحیح	    *عدد_Matches)
{
    اگر (xp->xp_نمونہ > xp->xp_سطر && *(xp->xp_نمونہ-1) == '=')
    {
	char_u *(*cb)(expand_T *, صحیح) = تہی;

	char_u *name_انتہا = xp->xp_نمونہ - 1;
	اگر (name_انتہا - xp->xp_سطر >= 2
		&& STRNCMP(name_انتہا - 2, "ff", 2) == 0)
	    cb = get_fileformat_name;
	ورنہ اگر (name_انتہا - xp->xp_سطر >= 10
		&& STRNCMP(name_انتہا - 10, "fileformat", 10) == 0)
	    cb = get_fileformat_name;
	ورنہ اگر (name_انتہا - xp->xp_سطر >= 3
		&& STRNCMP(name_انتہا - 3, "enc", 3) == 0)
	    cb = get_encoding_name;
	ورنہ اگر (name_انتہا - xp->xp_سطر >= 8
		&& STRNCMP(name_انتہا - 8, "encoding", 8) == 0)
	    cb = get_encoding_name;
	ورنہ اگر (name_انتہا - xp->xp_سطر >= 3
		&& STRNCMP(name_انتہا - 3, "bad", 3) == 0)
	    cb = get_bad_name;

	اگر (cb != تہی)
	{
	    واپس ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    عدد_Matches,
		    cb,
		    FALSE);
	}
	واپس FAIL;
    }

    // Special handling of "ff" which acts as a short form of
    // "fileformat", as "ff" is not a substring of it.
    اگر (xp->xp_نمونہ_len == 2
	    && STRNCMP(xp->xp_نمونہ, "ff", xp->xp_نمونہ_len) == 0)
    {
	*matches = ALLOC_MULT(char_u *, 1);
	اگر (*matches == تہی)
	    واپس FAIL;
	*عدد_Matches = 1;
	(*matches)[0] = vim_strsave((char_u*)"fileformat=");
	واپس OK;
    }

    واپس ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    عدد_Matches,
	    get_argopt_name,
	    FALSE);
}

    ساکن خالی
ex_autocmd(exarg_T *eap)
{
    اگر (eap->skip)
    {
	skip_cmd_بلاک(eap);
	واپس;
    }

    /*
     * Disallow autocommands from .exrc and .vimrc in current
     * directory for security reasons.
     */
    اگر (secure)
    {
	secure = 2;
	eap->errmsg =
	      _(e_command_not_allowed_from_vimrc_اندر_current_dir_or_لیبل_تلاش);
    }
    ورنہ اگر (eap->cmdidx == CMD_autocmd)
	do_autocmd(eap, eap->arg, eap->forceit);
    ورنہ
	do_augroup(eap->arg, eap->forceit);
}

/*
 * ":doautocmd": Apply the automatic commands to the current buffer.
 */
    ساکن خالی
ex_doautocmd(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    صحیح		call_do_modelines = check_nomodeline(&arg);
    صحیح		did_aucmd;

    (خالی)do_doautocmd(arg, TRUE, &did_aucmd);
    // Only when there is no <nomodeline>.
    اگر (call_do_modelines && did_aucmd)
	do_modelines(0);
}

/*
 * :[N]bunload[!] [N] [bufname] unload buffer
 * :[N]bdelete[!] [N] [bufname] delete buffer from buffer list
 * :[N]bwipeout[!] [N] [bufname] delete buffer really
 */
    ساکن خالی
ex_bunload(exarg_T *eap)
{
    اگر (ERROR_IF_ANY_POPUP_کھڑکی)
	واپس;
    eap->errmsg = do_bufdel(
	    eap->cmdidx == CMD_bdelete ? DOBUF_DEL
		: eap->cmdidx == CMD_bwipeout ? DOBUF_WIPE
		: DOBUF_UNLOAD, eap->arg,
	    eap->addr_گنتی, (صحیح)eap->line1, (صحیح)eap->line2, eap->forceit);
}

/*
 * :[N]buffer [N]	to buffer N
 * :[N]sbuffer [N]	to buffer N
 */
    ساکن خالی
ex_بفر(exarg_T *eap)
{
    اگر (ERROR_IF_ANY_POPUP_کھڑکی)
	واپس;
    do_exbuffer(eap);
}

/*
 * ":buffer" command and alike.
 */
    ساکن خالی
do_exbuffer(exarg_T *eap)
{
    اگر (*eap->arg)
	eap->errmsg = ex_errmsg(e_trailing_characters_str, eap->arg);
    ورنہ
    {
	اگر (eap->addr_گنتی == 0)	// default is current buffer
	    goto_بفر(eap, DOBUF_CURRENT, FORWARD, 0);
	ورنہ
	    goto_بفر(eap, DOBUF_پہلا, FORWARD, (صحیح)eap->line2);
	اگر (eap->do_ecmd_cmd != تہی)
	    do_cmd_دلیل(eap->do_ecmd_cmd);
    }
}

/*
 * :[N]bmodified [N]	to next mod. buffer
 * :[N]sbmodified [N]	to next mod. buffer
 */
    ساکن خالی
ex_bmodified(exarg_T *eap)
{
    goto_بفر(eap, DOBUF_MOD, FORWARD, (صحیح)eap->line2);
    اگر (eap->do_ecmd_cmd != تہی)
	do_cmd_دلیل(eap->do_ecmd_cmd);
}

/*
 * :[N]bnext [N]	to next buffer
 * :[N]sbnext [N]	split and to next buffer
 */
    ساکن خالی
ex_bnext(exarg_T *eap)
{
    اگر (ERROR_IF_ANY_POPUP_کھڑکی)
	واپس;

    goto_بفر(eap, DOBUF_CURRENT, FORWARD, (صحیح)eap->line2);
    اگر (eap->do_ecmd_cmd != تہی)
	do_cmd_دلیل(eap->do_ecmd_cmd);
}

/*
 * :[N]bNext [N]	to previous buffer
 * :[N]bprevious [N]	to previous buffer
 * :[N]sbNext [N]	split and to previous buffer
 * :[N]sbprevious [N]	split and to previous buffer
 */
    ساکن خالی
ex_bprevious(exarg_T *eap)
{
    اگر (ERROR_IF_ANY_POPUP_کھڑکی)
	واپس;

    goto_بفر(eap, DOBUF_CURRENT, BACKWARD, (صحیح)eap->line2);
    اگر (eap->do_ecmd_cmd != تہی)
	do_cmd_دلیل(eap->do_ecmd_cmd);
}

/*
 * :brewind		to first buffer
 * :bfirst		to first buffer
 * :sbrewind		split and to first buffer
 * :sbfirst		split and to first buffer
 */
    ساکن خالی
ex_brewind(exarg_T *eap)
{
    اگر (ERROR_IF_ANY_POPUP_کھڑکی)
	واپس;

    goto_بفر(eap, DOBUF_پہلا, FORWARD, 0);
    اگر (eap->do_ecmd_cmd != تہی)
	do_cmd_دلیل(eap->do_ecmd_cmd);
}

/*
 * :blast		to last buffer
 * :sblast		split and to last buffer
 */
    ساکن خالی
ex_blast(exarg_T *eap)
{
    اگر (ERROR_IF_ANY_POPUP_کھڑکی)
	واپس;

    goto_بفر(eap, DOBUF_آخری, BACKWARD, 0);
    اگر (eap->do_ecmd_cmd != تہی)
	do_cmd_دلیل(eap->do_ecmd_cmd);
}

/*
 * Check if "c" ends an Ex command.
 * In Vim9 script does not check for white space before #.
 */
    صحیح
سرے_excmd(صحیح c)
{
    صحیح comment_char = '"';

    اگر (اندر_vim9script())
	comment_char = '#';
    واپس (c == NUL || c == '|' || c == comment_char || c == '\n');
}

/*
 * Like ends_excmd() but checks that a # in Vim9 script either has "cmd" equal
 * to "cmd_start" or has a white space character before it.
 */
    صحیح
سرے_excmd2(char_u *cmd_آغاز غیر_مستعمل, char_u *cmd)
{
    صحیح c = *cmd;

    اگر (c == NUL || c == '|' || c == '\n')
	واپس TRUE;
    اگر (اندر_vim9script())
	//  # starts a comment, #{ might be a mistake, #{{ can start a fold
	واپس c == '#' && (cmd[1] != '{' || cmd[2] == '{')
				 && (cmd == cmd_آغاز || VIM_ISWHITE(cmd[-1]));
    واپس c == '"';
}

#اگر defined(FEAT_SYN_HL) || defined(FEAT_تلاش_EXTRA) || defined(FEAT_EVAL)
/*
 * Return the next command, after the first '|' or '\n'.
 * Return NULL if not found.
 */
    char_u *
ڈھونڈو_nextcmd(char_u *p)
{
    جب (*p != '|' && *p != '\n')
    {
	اگر (*p == NUL)
	    واپس تہی;
	++p;
    }
    واپس (p + 1);
}
#اگر_ختم

/*
 * Check if *p is a separator between Ex commands, skipping over white space.
 * Return NULL if it isn't, the following character if it is.
 */
    char_u *
check_nextcmd(char_u *p)
{
    char_u *s = skipwhite(p);

    اگر (*s == '|' || *s == '\n')
	واپس (s + 1);
    ورنہ
	واپس تہی;
}

/*
 * If "eap->nextcmd" is not set, check for a next command at "p".
 */
    خالی
سیٹ_nextcmd(exarg_T *eap, char_u *arg)
{
    char_u *p = skipwhite(arg);

    اگر (اندر_vim9script() && *p == '#')
    {
	char_u *nl = vim_strchr(p, NL);
	اگر (nl != تہی)
	    p = nl;
    }

    p = check_nextcmd(p);

    اگر (eap->nextcmd == تہی)
	eap->nextcmd = p;
    ورنہ اگر (p != تہی)
	// cannot use "| command" inside a  {} block
	semsg(_(e_cannot_use_bar_مقصد_separate_commands_یہاں_str), arg);
}

/*
 * - if there are more files to edit
 * - and this is the last window
 * - and forceit not used
 * - and not repeated twice on a row
 *    return FAIL and give error message if 'message' TRUE
 * return OK otherwise
 */
    ساکن صحیح
check_مزید(
    صحیح پیغام_2,	    // when FALSE check only, no messages
    صحیح forceit)
{
    صحیح	    n = ARGCOUNT - curwin->w_arg_idx - 1;

    اگر (!forceit && only_one_کھڑکی()
	    && ARGCOUNT > 1 && !arg_had_آخری && n > 0 && quitmore == 0)
    {
	اگر (پیغام_2)
	{
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    اگر ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						    && curbuf->b_fname != تہی)
	    {
		char_u	buff[DIALOG_پیغام_سائز];

		vim_snprintf((حرف *)buff, DIALOG_پیغام_سائز,
			NGETTEXT("%d more file to edit.  Quit anyway?",
			    "%d more files to edit.  Quit anyway?", n), n);
		اگر (vim_dialog_yesno(VIM_QUESTION, تہی, buff, 1) == VIM_YES)
		    واپس OK;
		واپس FAIL;
	    }
#اگر_ختم
	    semsg(NGETTEXT(e_nr_مزید_file_مقصد_edit,
			   e_nr_مزید_files_مقصد_edit , n), n);
	    quitmore = 2;	    // next try to quit is allowed
	}
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of command names.
 */
    char_u *
get_command_name(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx >= (صحیح)CMD_سائز)
	واپس expand_user_command_name(idx);
    // the following are no real commands
    اگر (STRNCMP(cmdnames[idx].cmd_name, "{", 1) == 0 ||
	STRNCMP(cmdnames[idx].cmd_name, "}", 1) == 0)
	واپس (char_u *)"";
    واپس cmdnames[idx].cmd_name;
}

    ساکن خالی
ex_colorscheme(exarg_T *eap)
{
    اگر (*eap->arg == NUL)
    {
#اگر_تعریف_شدہ FEAT_EVAL
	char_u *اظہاریہ = vim_strsave((char_u *)"g:colors_name");
	char_u *p = تہی;

	اگر (اظہاریہ != تہی)
	{
	    ++emsg_off;
	    p = eval_مقصد_string(اظہاریہ, FALSE, FALSE);
	    --emsg_off;
	    vim_free(اظہاریہ);
	}
	اگر (p != تہی)
	{
	    پیغام((حرف *)p);
	    vim_free(p);
	}
	ورنہ
	    پیغام("default");
#ورنہ
	پیغام(_("unknown"));
#اگر_ختم
    }
    ورنہ اگر (بھرو_colors(eap->arg) == FAIL)
	semsg(_(e_cannot_ڈھونڈو_color_scheme_str), eap->arg);

#اگر_تعریف_شدہ FEAT_VTP
    ورنہ اگر (has_vtp_working())
    {
	// background color change requires clear + redraw
	update_screen(UPD_CLEAR);
	redrawcmd();
    }
#اگر_ختم
}

    ساکن خالی
ex_highlight(exarg_T *eap)
{
    اگر (*eap->arg == NUL && eap->cmd[2] == '!')
	پیغام(_("Greetings, Vim user!"));
    do_highlight(eap->arg, eap->forceit, FALSE);
}


/*
 * Call this function if we thought we were going to exit, but we won't
 * (because of an error).  May need to restore the terminal mode.
 */
    خالی
not_exiting(صحیح save_exiting)
{
    exiting = save_exiting;
    settmode(TMODE_RAW);
}

    صحیح
before_quit_autocmds(win_T *wp, صحیح quit_all, صحیح forceit)
{
    apply_autocmds(EVENT_QUITPRE, تہی, تہی, FALSE, wp->w_بفر);

    // Bail out when autocommands closed the window.
    // Refuse to quit when the buffer in the last window is being closed (can
    // only happen in autocommands).
    اگر (!win_valid(wp)
	    || curbuf_locked()
	    || (wp->w_بفر->b_nwindows == 1 && wp->w_بفر->b_locked > 0))
	واپس TRUE;

    اگر (quit_all || (check_مزید(FALSE, forceit) == OK && only_one_کھڑکی()))
    {
	apply_autocmds(EVENT_EXITPRE, تہی, تہی, FALSE, curbuf);
	// Refuse to quit when locked or when the window was closed or the
	// buffer in the last window is being closed (can only happen in
	// autocommands).
	اگر (!win_valid(wp) || curbuf_locked()
			  || (curbuf->b_nwindows == 1 && curbuf->b_locked > 0))
	    واپس TRUE;
    }

    واپس FALSE;
}

/*
 * ":quit": quit current window, quit Vim if the last window is closed.
 * ":{nr}quit": quit window {nr}
 * Also used when closing a terminal window that's the last one.
 */
    خالی
ex_quit(exarg_T *eap)
{
    win_T	*wp;

    اگر (cmdwin_type != 0)
    {
	cmdwin_نتیجہ = Ctrl_C;
	واپس;
    }
    // Don't quit while editing the command line.
    اگر (متن_locked())
    {
	متن_locked_پیغام();
	واپس;
    }
    اگر (eap->addr_گنتی > 0)
    {
	صحیح	wnr = eap->line2;

	برائے (wp = firstwin; wp->w_اگلا != تہی; wp = wp->w_اگلا)
	    اگر (--wnr <= 0)
		توڑو;
    }
    ورنہ
	wp = curwin;

    // Refuse to quit when locked.
    اگر (curbuf_locked())
	واپس;

    // Trigger QuitPre and maybe ExitPre
    اگر (before_quit_autocmds(wp, FALSE, eap->forceit))
	واپس;

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    netbeansForcedQuit = eap->forceit;
#اگر_ختم

    صحیح save_exiting = exiting;
    /*
     * If there is only one relevant window we will exit.
     */
    اگر (check_مزید(FALSE, eap->forceit) == OK && only_one_کھڑکی())
	exiting = TRUE;
    اگر ((!buf_hide(wp->w_بفر)
		&& check_بدلا(wp->w_بفر, (p_awa ? CCGD_AW : 0)
				       | (eap->forceit ? CCGD_FORCEIT : 0)
				       | CCGD_EXCMD))
	    || check_مزید(TRUE, eap->forceit) == FAIL
	    || (only_one_کھڑکی() && check_بدلا_any(eap->forceit, TRUE)))
    {
	not_exiting(save_exiting);
    }
    ورنہ
    {
	// quit last window
	// Note: only_one_window() returns true, even so a help window is
	// still open. In that case only quit, if no address has been
	// specified. Example:
	// :h|wincmd w|1q     - don't quit
	// :h|wincmd w|q      - quit
	اگر (only_one_کھڑکی() && (ONE_کھڑکی || eap->addr_گنتی == 0))
	    getout(0);
	not_exiting(save_exiting);
#اگر_تعریف_شدہ FEAT_GUI
	need_mouse_correct = TRUE;
#اگر_ختم
	// close window; may free buffer
	win_close(wp, !buf_hide(wp->w_بفر) || eap->forceit);
    }
}

/*
 * ":cquit".
 */
    ساکن خالی
ex_cquit(exarg_T *eap غیر_مستعمل)
{
    // this does not always pass on the exit code to the Manx compiler. why?
    getout(eap->addr_گنتی > 0 ? (صحیح)eap->line2 : EXIT_FAILURE);
}

/*
 * Do preparations for "qall" and "wqall".
 * Returns FAIL when quitting should be aborted.
 */
    صحیح
before_quit_all(exarg_T *eap)
{
    اگر (cmdwin_type != 0)
    {
	اگر (eap->forceit)
	    cmdwin_نتیجہ = K_XF1;	// ex_window() takes care of this
	ورنہ
	    cmdwin_نتیجہ = K_XF2;
	واپس FAIL;
    }

    // Don't quit while editing the command line.
    اگر (متن_locked())
    {
	متن_locked_پیغام();
	واپس FAIL;
    }

    اگر (before_quit_autocmds(curwin, TRUE, eap->forceit))
	واپس FAIL;

    واپس OK;
}

/*
 * ":qall": try to quit all windows
 */
    ساکن خالی
ex_quit_all(exarg_T *eap)
{
    اگر (before_quit_all(eap) == FAIL)
	واپس;
    صحیح save_exiting = exiting;
    exiting = TRUE;
    اگر (eap->forceit || !check_بدلا_any(FALSE, FALSE))
	getout(0);
    not_exiting(save_exiting);
}

/*
 * ":close": close current window, unless it is the last one
 */
    ساکن خالی
ex_close(exarg_T *eap)
{
    win_T	*win;
    صحیح		winnr = 0;
    اگر (cmdwin_type != 0)
	cmdwin_نتیجہ = Ctrl_C;
    ورنہ
	اگر (!متن_locked() && !curbuf_locked())
	{
	    اگر (eap->addr_گنتی == 0)
		ex_win_close(eap->forceit, curwin, تہی);
	    ورنہ
	    {
		FOR_ALL_WINDOWS(win)
		{
		    winnr++;
		    اگر (winnr == eap->line2)
			توڑو;
		}
		اگر (win == تہی)
		    win = lastwin;
		ex_win_close(eap->forceit, win, تہی);
	    }
	}
}

#اگر_تعریف_شدہ FEAT_QUICKFIX
/*
 * ":pclose": Close any preview window.
 */
    ساکن خالی
ex_pclose(exarg_T *eap)
{
    win_T	*win;

    // First close any normal window.
    FOR_ALL_WINDOWS(win)
	اگر (win->w_p_pvw)
	{
	    ex_win_close(eap->forceit, win, تہی);
	    واپس;
	}
# ifdef FEAT_PROP_POPUP
    // Also when 'previewpopup' is empty, it might have been cleared.
    popup_close_preview();
# endif
}
#اگر_ختم

/*
 * Close window "win" and take care of handling closing the last window for a
 * modified buffer.
 */
    ساکن خالی
ex_win_close(
    صحیح		forceit,
    win_T	*win,
    tabpage_T	*tp)		// NULL or the tab page "win" is in
{
    صحیح		need_hide;
    buf_T	*buf = win->w_بفر;

    // Never close the autocommand window.
    اگر (is_aucmd_win(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_کھڑکی));
	واپس;
    }
    اگر (کھڑکی_layout_locked(CMD_close))
	واپس;

    need_hide = (buf_Is_بدلا(buf) && buf->b_nwindows <= 1);
    اگر (need_hide && !buf_hide(buf) && !forceit)
    {
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	اگر ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    اگر (رکن_کام_running(buf->b_رکن))
	    {
		اگر (رکن_confirm_stop(buf) == FAIL)
		    واپس;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(buf);
		need_hide = FALSE;
	    }
	    ورنہ
# endif
	    {
		bufref_T bufref;

		سیٹ_bufref(&bufref, buf);
		dialog_بدلا(buf, FALSE);
		اگر (bufref_valid(&bufref) && buf_Is_بدلا(buf))
		    واپس;
		need_hide = FALSE;
	    }
	}
	ورنہ
#اگر_ختم
	{
	    no_write_پیغام();
	    واپس;
	}
    }

#اگر_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#اگر_ختم

    // free buffer when not hiding it or when it's a scratch buffer
    اگر (tp == تہی)
	win_close(win, !need_hide && !buf_hide(buf));
    ورنہ
	win_close_othertab(win, !need_hide && !buf_hide(buf), tp);
}

/*
 * Handle the argument for a tabpage related ex command.
 * Returns a tabpage number.
 * When an error is encountered then eap->errmsg is set.
 */
    ساکن صحیح
get_tabpage_arg(exarg_T *eap)
{
    صحیح tab_نمبر;
    صحیح unaccept_arg0 = (eap->cmdidx == CMD_tabmove) ? 0 : 1;

    اگر (eap->arg && *eap->arg != NUL)
    {
	char_u *p = eap->arg;
	char_u *p_save;
	صحیح    relative = 0; // argument +N/-N means: go to N places to the
			     // right/left relative to the current position.

	اگر (*p == '-')
	{
	    relative = -1;
	    p++;
	}
	ورنہ اگر (*p == '+')
	{
	    relative = 1;
	    p++;
	}

	p_save = p;
	tab_نمبر = getdigits(&p);

	اگر (relative == 0)
	{
	    اگر (STRCMP(p, "$") == 0)
		tab_نمبر = آخری_TAB_NR;
	    ورنہ اگر (STRCMP(p, "#") == 0)
		اگر (valid_tabpage(lastused_tabpage))
		    tab_نمبر = tabpage_اشاریہ(lastused_tabpage);
		ورنہ
		{
		    eap->errmsg = ex_errmsg(e_invalid_قدر_for_دلیل_str,
								     eap->arg);
		    tab_نمبر = 0;
		    جاؤ theend;
		}
	    ورنہ اگر (p == p_save || *p_save == '-' || *p != NUL
		    || tab_نمبر > آخری_TAB_NR)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_دلیل_str, eap->arg);
		جاؤ theend;
	    }
	}
	ورنہ
	{
	    اگر (*p_save == NUL)
		tab_نمبر = 1;
	    ورنہ اگر (p == p_save || *p_save == '-' || *p != NUL
		    || tab_نمبر == 0)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_دلیل_str, eap->arg);
		جاؤ theend;
	    }
	    tab_نمبر = tab_نمبر * relative + tabpage_اشاریہ(curtab);
	    اگر (!unaccept_arg0 && relative == -1)
		--tab_نمبر;
	}
	اگر (tab_نمبر < unaccept_arg0 || tab_نمبر > آخری_TAB_NR)
	    eap->errmsg = ex_errmsg(e_invalid_دلیل_str, eap->arg);
    }
    ورنہ اگر (eap->addr_گنتی > 0)
    {
	اگر (unaccept_arg0 && eap->line2 == 0)
	{
	    eap->errmsg = _(e_invalid_range);
	    tab_نمبر = 0;
	}
	ورنہ
	{
	    tab_نمبر = eap->line2;
	    اگر (!unaccept_arg0)
	    {
		char_u *cmdp = eap->cmd;

		جب (--cmdp > *eap->cmdlinep
			&& (VIM_ISWHITE(*cmdp) || VIM_ISDIGIT(*cmdp)))
		    ;
		اگر (*cmdp == '-')
		{
		    --tab_نمبر;
		    اگر (tab_نمبر < unaccept_arg0)
			eap->errmsg = _(e_invalid_range);
		}
	    }
	}
    }
    ورنہ
    {
	انتخاب (eap->cmdidx)
	{
	حالت CMD_tabnext:
	    tab_نمبر = tabpage_اشاریہ(curtab) + 1;
	    اگر (tab_نمبر > آخری_TAB_NR)
		tab_نمبر = 1;
	    توڑو;
	حالت CMD_tabmove:
	    tab_نمبر = آخری_TAB_NR;
	    توڑو;
	طےشدہ:
	    tab_نمبر = tabpage_اشاریہ(curtab);
	}
    }

theend:
    واپس tab_نمبر;
}

/*
 * ":tabclose": close current tab page, unless it is the last one.
 * ":tabclose N": close tab page N.
 */
    ساکن خالی
ex_tabclose(exarg_T *eap)
{
    tabpage_T	*tp;
    صحیح		tab_نمبر;

    اگر (cmdwin_type != 0)
    {
	cmdwin_نتیجہ = K_IGNORE;
	واپس;
    }

    اگر (پہلا_tabpage->tp_اگلا == تہی)
    {
	emsg(_(e_cannot_close_آخری_tab_صفحہ));
	واپس;
    }

    اگر (کھڑکی_layout_locked(CMD_tabclose))
	واپس;

    tab_نمبر = get_tabpage_arg(eap);
    اگر (eap->errmsg != تہی)
	واپس;

    tp = ڈھونڈو_tabpage(tab_نمبر);
    اگر (tp == تہی)
    {
	beep_flush();
	واپس;
    }
    اگر (tp != curtab)
    {
	tabpage_close_other(tp, eap->forceit);
	واپس;
    }
    ورنہ اگر (!متن_locked() && !curbuf_locked())
	tabpage_close(eap->forceit);
}

/*
 * ":tabonly": close all tab pages except the current one
 */
    ساکن خالی
ex_tabonly(exarg_T *eap)
{
    tabpage_T	*tp;
    صحیح		مکمل;
    صحیح		tab_نمبر;

    اگر (cmdwin_type != 0)
    {
	cmdwin_نتیجہ = K_IGNORE;
	واپس;
    }

    اگر (پہلا_tabpage->tp_اگلا == تہی)
    {
	پیغام(_("Already only one tab page"));
	واپس;
    }

    اگر (کھڑکی_layout_locked(CMD_tabonly))
	واپس;

    tab_نمبر = get_tabpage_arg(eap);
    اگر (eap->errmsg != تہی)
	واپس;

    goto_tabpage(tab_نمبر);
    // Repeat this up to a 1000 times, because autocommands may
    // mess up the lists.
    برائے (مکمل = 0; مکمل < 1000; ++مکمل)
    {
	FOR_ALL_TABPAGES(tp)
	    اگر (tp->tp_topframe != topframe)
	    {
		tabpage_close_other(tp, eap->forceit);
		// if we failed to close it quit
		اگر (valid_tabpage(tp))
		    مکمل = 1000;
		// start over, "tp" is now invalid
		توڑو;
	    }
	اگر (پہلا_tabpage->tp_اگلا == تہی)
	    توڑو;
    }
}

/*
 * Close the current tab page.
 */
    خالی
tabpage_close(صحیح forceit)
{
    اگر (کھڑکی_layout_locked(CMD_tabclose))
	واپس;

    trigger_tabclosedpre(curtab);
    curtab->tp_did_tabclosedpre = TRUE;
    tabpage_T *save_curtab = curtab;

    // First close all the windows but the current one.  If that worked then
    // close the last window in this tab, that will close it.
    اگر (!ONE_کھڑکی)
	close_others(TRUE, forceit);
    اگر (ONE_کھڑکی)
	ex_win_close(forceit, curwin, تہی);
    اگر (curtab == save_curtab)
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	curtab->tp_did_tabclosedpre = FALSE;
#اگر_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#اگر_ختم
}

/*
 * Close tab page "tp", which is not the current tab page.
 * Note that autocommands may make "tp" invalid.
 * Also takes care of the tab pages line disappearing when closing the
 * last-but-one tab page.
 */
    خالی
tabpage_close_other(tabpage_T *tp, صحیح forceit)
{
    صحیح		مکمل = 0;
    win_T	*wp;

    اگر (کھڑکی_layout_locked(CMD_سائز))
	واپس;

    trigger_tabclosedpre(tp);
    tp->tp_did_tabclosedpre = TRUE;

    // Limit to 1000 windows, autocommands may add a window while we close
    // one.  OK, so I'm paranoid...
    جب (++مکمل < 1000)
    {
	wp = tp->tp_firstwin;
	ex_win_close(forceit, wp, tp);

	// Autocommands may delete the tab page under our fingers.
	اگر (!valid_tabpage(tp))
	    توڑو;
	// We may fail to close a window with a modified buffer.
	اگر (tp->tp_firstwin == wp)
	{
	    مکمل = 1000;
	    توڑو;
	}
    }
    اگر (مکمل >= 1000)
    {
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	tp->tp_did_tabclosedpre = FALSE;
	واپس;
    }

    apply_autocmds(EVENT_TABCLOSED, تہی, تہی, FALSE, curbuf);
}

/*
 * ":only".
 */
    ساکن خالی
ex_only(exarg_T *eap)
{
    اگر (کھڑکی_layout_locked(CMD_only))
	واپس;
#اگر_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#اگر_ختم
    اگر (eap->addr_گنتی > 0)
    {
	win_T   *wp;
	صحیح	wnr = eap->line2;
	برائے (wp = firstwin; --wnr > 0; )
	{
	    اگر (wp->w_اگلا == تہی)
		توڑو;
	    ورنہ
		wp = wp->w_اگلا;
	}
	win_goto(wp);
    }
    close_others(TRUE, eap->forceit);
}

    ساکن خالی
ex_hide(exarg_T *eap غیر_مستعمل)
{
    // ":hide" or ":hide | cmd": hide current window
    اگر (eap->skip)
	واپس;

    اگر (کھڑکی_layout_locked(CMD_hide))
	واپس;
#اگر_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#اگر_ختم
    اگر (eap->addr_گنتی == 0)
	win_close(curwin, FALSE);	// don't free buffer
    ورنہ
    {
	صحیح	winnr = 0;
	win_T	*win;

	FOR_ALL_WINDOWS(win)
	{
	    winnr++;
	    اگر (winnr == eap->line2)
		توڑو;
	}
	اگر (win == تہی)
	    win = lastwin;
	win_close(win, FALSE);
    }
}

/*
 * ":stop" and ":suspend": Suspend Vim.
 */
    خالی
ex_stop(exarg_T *eap)
{
    /*
     * Disallow suspending for "rvim".
     */
    اگر (check_restricted())
	واپس;

    اگر (!eap->forceit)
	autowrite_all();
    apply_autocmds(EVENT_VIMSUSPEND, تہی, تہی, FALSE, تہی);
    windgoto((صحیح)صفیں - 1, cmdline_ستون_off);
    برآمد_char('\n');
    برآمد_flush();
    stoptermcap();
    برآمد_flush();		// needed for SUN to restore xterm buffer
    mch_restore_title(SAVE_RESTORE_BOTH);	// restore window titles
    ui_suspend();		// call machine specific function
    maketitle();
    resettitle();		// force updating the title
    starttermcap();
    scroll_آغاز();		// scroll screen before redrawing
    redraw_later_clear();
    shell_resized();	// may have resized window
    apply_autocmds(EVENT_VIMRESUME, تہی, تہی, FALSE, تہی);
}

/*
 * ":exit", ":xit" and ":wq": Write file and quit the current window.
 */
    ساکن خالی
ex_exit(exarg_T *eap)
{
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (not_اندر_vim9(eap, false) == FAIL)
	واپس;
#اگر_ختم
    اگر (cmdwin_type != 0)
    {
	cmdwin_نتیجہ = Ctrl_C;
	واپس;
    }
    // Don't quit while editing the command line.
    اگر (متن_locked())
    {
	متن_locked_پیغام();
	واپس;
    }

    صحیح save_exiting = exiting;
    /*
     * we plan to exit if there is only one relevant window
     */
    اگر (check_مزید(FALSE, eap->forceit) == OK && only_one_کھڑکی())
	exiting = TRUE;

    // Write the buffer for ":wq" or when it was changed.
    // Trigger QuitPre and ExitPre.
    // Check if we can exit now, after autocommands have changed things.
    اگر (((eap->cmdidx == CMD_wq || curbuf_Is_بدلا()) && do_write(eap) == FAIL)
	    || before_quit_autocmds(curwin, FALSE, eap->forceit)
	    || check_مزید(TRUE, eap->forceit) == FAIL
	    || (only_one_کھڑکی() && check_بدلا_any(eap->forceit, FALSE)))
    {
	not_exiting(save_exiting);
    }
    ورنہ
    {
	اگر (only_one_کھڑکی())	    // quit last window, exit Vim
	    getout(0);
	not_exiting(save_exiting);
#اگر_تعریف_شدہ FEAT_GUI
	need_mouse_correct = TRUE;
#اگر_ختم
	// Quit current window, may free the buffer.
	win_close(curwin, !buf_hide(curwin->w_بفر));
    }
}

/*
 * ":print", ":list", ":number".
 */
    ساکن خالی
ex_print(exarg_T *eap)
{
    اگر (curbuf->b_ml.ml_flags & ML_خالی_خانے)
	emsg(_(e_خالی_خانے_بفر));
    ورنہ
    {
	برائے ( ;!got_int; ui_breakcheck())
	{
	    print_سطر(eap->line1,
		    (eap->cmdidx == CMD_نمبر || eap->cmdidx == CMD_pound
						 || (eap->flags & EXFLAG_NR)),
		    eap->cmdidx == CMD_list || (eap->flags & EXFLAG_LIST));
	    اگر (++eap->line1 > eap->line2)
		توڑو;
	    برآمد_flush();	    // show one line at a time
	}
	setpcmark();
	// put cursor at last line
	curwin->w_cursor.lnum = eap->line2;
	beginline(BL_SOL | BL_FIX);
    }

    ex_no_reprint = TRUE;
}

#اگر_تعریف_شدہ FEAT_BYTEOFF
    ساکن خالی
ex_goto(exarg_T *eap)
{
    goto_byte(eap->line2);
}
#اگر_ختم

/*
 * ":shell".
 */
    ساکن خالی
ex_shell(exarg_T *eap غیر_مستعمل)
{
    do_shell(تہی, 0);
}

#اگر defined(HAVE_DROP_FILE)

ساکن صحیح drop_busy = FALSE;
ساکن صحیح drop_filec;
ساکن char_u **drop_filev = تہی;
ساکن صحیح drop_بانٹو;
ساکن خالی (*drop_callback)(خالی *);
ساکن خالی *drop_cookie;

    ساکن خالی
handle_drop_اندرونی(خالی)
{
    exarg_T	ea;
    صحیح		save_پیغام_scroll = پیغام_scroll;

    // Setting the argument list may cause screen updates and being called
    // recursively.  Avoid that by setting drop_busy.
    drop_busy = TRUE;

    // Check whether the current buffer is changed. If so, we will need
    // to split the current window or data could be lost.
    // We don't need to check if the 'hidden' option is set, as in this
    // case the buffer won't be lost.
    اگر (!buf_hide(curbuf) && !drop_بانٹو)
    {
	++emsg_off;
	drop_بانٹو = check_بدلا(curbuf, CCGD_AW);
	--emsg_off;
    }
    اگر (drop_بانٹو)
    {
	اگر (win_بانٹو(0, 0) == FAIL)
	    واپس;
	RESET_BINDING(curwin);

	// When splitting the window, create a new alist.  Otherwise the
	// existing one is overwritten.
	alist_unlink(curwin->w_alist);
	alist_new();
    }

    /*
     * Set up the new argument list.
     */
    alist_سیٹ(ALIST(curwin), drop_filec, drop_filev, FALSE, تہی, 0);

    /*
     * Move to the first file.
     */
    // Fake up a minimal "next" command for do_argfile()
    CLEAR_میدان(ea);
    ea.cmd = (char_u *)"next";
    do_argfile(&ea, 0);

    // do_ecmd() may set need_start_insertmode, but since we never left Insert
    // mode that is not needed here.
    need_آغاز_insertmode = FALSE;

    // Restore msg_scroll, otherwise a following command may cause scrolling
    // unexpectedly.  The screen will be redrawn by the caller, thus
    // msg_scroll being set by displaying a message is irrelevant.
    پیغام_scroll = save_پیغام_scroll;

    اگر (drop_callback != تہی)
	drop_callback(drop_cookie);

    drop_filev = تہی;
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
    خالی
handle_drop(
    صحیح		filec,		// the number of files dropped
    char_u	**filev,	// the list of files dropped
    صحیح		بانٹو,		// force splitting the window
    خالی	(*callback)(خالی *), // to be called after setting the argument
				     // list
    خالی	*cookie)	// argument for "callback" (allocated)
{
    // Cannot handle recursive drops, finish the pending one.
    اگر (drop_busy)
    {
	FreeWild(filec, filev);
	vim_free(cookie);
	واپس;
    }

    // When calling handle_drop() more than once in a row we only use the last
    // one.
    اگر (drop_filev != تہی)
    {
	FreeWild(drop_filec, drop_filev);
	vim_free(drop_cookie);
    }

    drop_filec = filec;
    drop_filev = filev;
    drop_بانٹو = بانٹو;
    drop_callback = callback;
    drop_cookie = cookie;

    // Postpone this when:
    // - editing the command line
    // - not possible to change the current buffer
    // - updating the screen
    // As it may change buffers and window structures that are in use and cause
    // freed memory to be used.
    اگر (متن_locked() || curbuf_locked() || updating_screen)
	واپس;

    handle_drop_اندرونی();
}

/*
 * To be called when text is unlocked, curbuf is unlocked or updating_screen is
 * reset: Handle a postponed drop.
 */
    خالی
handle_any_postponed_drop(خالی)
{
    اگر (!drop_busy && drop_filev != تہی
	     && !متن_locked() && !curbuf_locked() && !updating_screen)
	handle_drop_اندرونی();
}
#اگر_ختم

/*
 * ":preserve".
 */
    ساکن خالی
ex_preserve(exarg_T *eap غیر_مستعمل)
{
    curbuf->b_flags |= BF_PRESERVED;
    ml_preserve(curbuf, TRUE);
}

/*
 * ":recover".
 */
    ساکن خالی
ex_recover(exarg_T *eap)
{
    // Set recoverymode right away to avoid the ATTENTION prompt.
    recoverymode = TRUE;
    اگر (!check_بدلا(curbuf, (p_awa ? CCGD_AW : 0)
			     | CCGD_MULTWIN
			     | (eap->forceit ? CCGD_FORCEIT : 0)
			     | CCGD_EXCMD)

	    && (*eap->arg == NUL
			     || setfname(curbuf, eap->arg, تہی, TRUE) == OK))
	ml_recover(TRUE);
    recoverymode = FALSE;
}

/*
 * Command modifier used in a wrong way.  Also for other commands that can't
 * appear at the toplevel.
 */
    ساکن خالی
ex_wrongmodifier(exarg_T *eap)
{
    eap->errmsg = ex_errmsg(e_invalid_command_str, eap->cmd);
}

#اگر defined(FEAT_EVAL)

// callback function for 'findfunc'
ساکن callback_T ffu_cb;

    ساکن callback_T *
get_findfunc_callback(خالی)
{
    واپس *curbuf->b_p_ffu != NUL ? &curbuf->b_ffu_cb : &ffu_cb;
}

/*
 * Call 'findfunc' to obtain a list of file names.
 */
    ساکن list_T *
call_findfunc(char_u *pat, صحیح cmdcomplete)
{
    typval_T	دلائل[3];
    callback_T	*cb;
    typval_T	rettv;
    صحیح		retval;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    دلائل[0].v_type = VAR_STRING;
    دلائل[0].vval.v_string = pat;
    دلائل[1].v_type = VAR_BOOL;
    دلائل[1].vval.v_نمبر = cmdcomplete;
    دلائل[2].v_type = VAR_UNKNOWN;

    // Lock the text to prevent weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;

    ctx = get_option_sctx("findfunc");
    اگر (ctx != تہی)
	current_sctx = *ctx;

    cb = get_findfunc_callback();
    retval = call_callback(cb, -1, &rettv, 2, دلائل);

    current_sctx = saved_sctx;

    --textlock;

    list_T *retlist = تہی;

    اگر (retval == OK)
    {
	اگر (rettv.v_type == VAR_LIST)
	    retlist = list_copy(rettv.vval.v_list, FALSE, FALSE, get_copyID());
	ورنہ
	    emsg(_(e_invalid_return_type_from_findfunc));

	clear_tv(&rettv);
    }

    واپس retlist;
}

/*
 * Find file names matching "pat" using 'findfunc' and return it in "files".
 * Used for expanding the :find, :sfind and :tabfind command argument.
 * Returns OK on success and FAIL otherwise.
 */
    صحیح
expand_findfunc(expand_T *xp, char_u *pat, char_u ***files, صحیح *عدد_Matches)
{
    list_T	*l;
    صحیح		len;

    *عدد_Matches = 0;
    *files = تہی;

    l = call_findfunc(pat, VVAL_TRUE);

    اگر (l == تہی)
	واپس FAIL;

    len = list_len(l);
    اگر (len == 0)	    // empty List
    {
	list_free(l);
	واپس FAIL;
    }

    expand_process_user_list(l, files, عدد_Matches, xp);
    list_free(l);

    واپس OK;
}

/*
 * Use 'findfunc' to find file 'findarg'.  The 'count' argument is used to find
 * the n'th matching file.
 */
    ساکن char_u *
findfunc_ڈھونڈو_file(char_u *findarg, صحیح findarg_len, صحیح گنتی)
{
    list_T	*fname_list;
    char_u	*ret_fname = تہی;
    char_u	cc;
    صحیح		fname_گنتی;

    cc = findarg[findarg_len];
    findarg[findarg_len] = NUL;

    fname_list = call_findfunc(findarg, VVAL_FALSE);
    fname_گنتی = list_len(fname_list);

    اگر (fname_گنتی == 0)
	semsg(_(e_cant_ڈھونڈو_file_str_اندر_راستہ), findarg);
    ورنہ
    {
	اگر (گنتی > fname_گنتی)
	    semsg(_(e_no_مزید_file_str_ملا_اندر_راستہ), findarg);
	ورنہ
	{
	    listitem_T *li = list_ڈھونڈو(fname_list, گنتی - 1);

	    اگر (li != تہی)
	    {
		typval_T *tv = &li->li_tv;

		اگر (tv->v_type == VAR_STRING && tv->vval.v_string != تہی)
		    ret_fname = vim_strsave(tv->vval.v_string);
		ورنہ اگر (tv->v_type == VAR_DICT && tv->vval.v_dict != تہی)
		    ret_fname = dict_get_string(tv->vval.v_dict, "word", TRUE);
	    }
	}
    }

    اگر (fname_list != تہی)
	list_free(fname_list);

    findarg[findarg_len] = cc;

    واپس ret_fname;
}

/*
 * Process the 'findfunc' option value.
 * Returns NULL on success and an error message on failure.
 */
    حرف *
did_سیٹ_findfunc(optset_T *دلائل غیر_مستعمل)
{
    صحیح	retval;

    اگر (دلائل->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_سیٹ_callback_func(curbuf->b_p_ffu, &curbuf->b_ffu_cb);
    ورنہ
    {
	// global option set
	retval = option_سیٹ_callback_func(p_ffu, &ffu_cb);
	// when using :set, free the local callback
	اگر (!(دلائل->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_ffu_cb);
    }

    اگر (retval == FAIL)
	واپس e_invalid_دلیل;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u	**varp = (char_u **)دلائل->os_varp;
    char_u	*name = get_scriptlocal_funcname(*varp);
    اگر (name != تہی)
    {
	free_string_option(*varp);
	*varp = name;
    }

    واپس تہی;
}

# اگر defined(EXITFREE)
    خالی
free_findfunc_option(خالی)
{
    free_callback(&ffu_cb);
}
# endif

/*
 * Mark the global 'findfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    صحیح
سیٹ_ref_اندر_findfunc(صحیح copyID غیر_مستعمل)
{
    صحیح abort = FALSE;
    abort = سیٹ_ref_اندر_callback(&ffu_cb, copyID);
    واپس abort;
}
#اگر_ختم

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
    خالی
ex_splitview(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    char_u	*fname = تہی;
#اگر_تعریف_شدہ FEAT_BROWSE
    char_u	dot_راستہ[] = ".";
    صحیح		save_cmod_flags = cmdmod.cmod_flags;
#اگر_ختم
    صحیح		use_tab = eap->cmdidx == CMD_tabedit
		       || eap->cmdidx == CMD_tabfind
		       || eap->cmdidx == CMD_tabnew;

    اگر (ERROR_IF_ANY_POPUP_کھڑکی)
	واپس;

#اگر_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_QUICKFIX
    // A ":split" in the quickfix window works like ":new".  Don't want two
    // quickfix windows.  But it's OK when doing ":tab split".
    اگر (bt_quickfix(curbuf) && cmdmod.cmod_tab == 0)
    {
	اگر (eap->cmdidx == CMD_بانٹو)
	    eap->cmdidx = CMD_new;
	اگر (eap->cmdidx == CMD_vsplit)
	    eap->cmdidx = CMD_vnew;
    }
#اگر_ختم

    اگر (eap->cmdidx == CMD_sfind || eap->cmdidx == CMD_tabfind)
    {
	char_u	*file_مقصد_ڈھونڈو = تہی;
	حرف	*تلاش_ctx = تہی;

	اگر (*get_findfunc() != NUL)
	{
#اگر_تعریف_شدہ FEAT_EVAL
	    fname = findfunc_ڈھونڈو_file(eap->arg, (صحیح)STRLEN(eap->arg),
				       eap->addr_گنتی > 0 ? eap->line2 : 1);
#اگر_ختم
	}
	ورنہ
	{
	    fname = ڈھونڈو_file_اندر_راستہ(eap->arg, (صحیح)STRLEN(eap->arg),
					      FNAME_MESS, TRUE, curbuf->b_ffname,
					      &file_مقصد_ڈھونڈو, &تلاش_ctx);
	    vim_free(file_مقصد_ڈھونڈو);
	    vim_findfile_cleanup(تلاش_ctx);
	}
	اگر (fname == تہی)
	    جاؤ theend;
	eap->arg = fname;
    }
#اگر_تعریف_شدہ FEAT_BROWSE
    ورنہ اگر ((cmdmod.cmod_flags & CMOD_BROWSE)
	    && eap->cmdidx != CMD_vnew
	    && eap->cmdidx != CMD_new)
    {
	اگر (
# ifdef FEAT_GUI
	    !gui.اندر_use &&
# endif
		au_has_گروہ((char_u *)"FileExplorer"))
	{
	    // No browsing supported but we do have the file explorer:
	    // Edit the directory.
	    اگر (*eap->arg == NUL || !mch_isdir(eap->arg))
		eap->arg = dot_راستہ;
	}
	ورنہ
	{
	    fname = do_browse(0, (char_u *)(use_tab
			? _("Edit File in new tab page")
			: _("Edit File in new window")),
					  eap->arg, تہی, تہی, تہی, curbuf);
	    اگر (fname == تہی)
		جاؤ theend;
	    eap->arg = fname;
	}
    }
    cmdmod.cmod_flags &= ~CMOD_BROWSE;	// Don't browse again in do_ecmd().
#اگر_ختم

    /*
     * Either open new tab page or split the window.
     */
    اگر (use_tab)
    {
	اگر (win_new_tabpage(cmdmod.cmod_tab != 0 ? cmdmod.cmod_tab
			 : eap->addr_گنتی == 0 ? 0
					       : (صحیح)eap->line2 + 1) != FAIL)
	{
	    do_exedit(eap, old_curwin);

	    // set the alternate buffer for the window we came from
	    اگر (curwin != old_curwin
		    && win_valid(old_curwin)
		    && old_curwin->w_بفر != curbuf
		    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		old_curwin->w_alt_fnum = curbuf->b_fnum;
	}
    }
    ورنہ اگر (win_بانٹو(eap->addr_گنتی > 0 ? (صحیح)eap->line2 : 0,
				     *eap->cmd == 'v' ? WSP_VERT : 0) != FAIL)
    {
	// Reset 'scrollbind' when editing another file, but keep it when
	// doing ":split" without arguments.
	اگر (*eap->arg != NUL)
	    RESET_BINDING(curwin);
	ورنہ
	    do_check_scrollbind(FALSE);
	do_exedit(eap, old_curwin);
    }

#اگر_تعریف_شدہ FEAT_BROWSE
    cmdmod.cmod_flags = save_cmod_flags;
#اگر_ختم

theend:
    vim_free(fname);
}

/*
 * Open a new tab page.
 */
    خالی
tabpage_new(خالی)
{
    exarg_T	ea;

    CLEAR_میدان(ea);
    ea.cmdidx = CMD_tabnew;
    ea.cmd = (char_u *)"tabn";
    ea.arg = (char_u *)"";
    ex_splitview(&ea);
}

/*
 * :tabnext command
 */
    ساکن خالی
ex_tabnext(exarg_T *eap)
{
    صحیح tab_نمبر;

    اگر (ERROR_IF_POPUP_کھڑکی)
	واپس;
    انتخاب (eap->cmdidx)
    {
	حالت CMD_tabfirst:
	حالت CMD_tabrewind:
	    goto_tabpage(1);
	    توڑو;
	حالت CMD_tablast:
	    goto_tabpage(9999);
	    توڑو;
	حالت CMD_tabprevious:
	حالت CMD_tab_اگلا:
	    اگر (eap->arg && *eap->arg != NUL)
	    {
		char_u *p = eap->arg;
		char_u *p_save = p;

		tab_نمبر = getdigits(&p);
		اگر (p == p_save || *p_save == '-' || *p != NUL
			    || tab_نمبر == 0)
		{
		    // No numbers as argument.
		    eap->errmsg = ex_errmsg(e_invalid_دلیل_str, eap->arg);
		    واپس;
		}
	    }
	    ورنہ
	    {
		اگر (eap->addr_گنتی == 0)
		    tab_نمبر = 1;
		ورنہ
		{
		    tab_نمبر = eap->line2;
		    اگر (tab_نمبر < 1)
		    {
			eap->errmsg = _(e_invalid_range);
			واپس;
		    }
		}
	    }
	    goto_tabpage(-tab_نمبر);
	    توڑو;
	طےشدہ: // CMD_tabnext
	    tab_نمبر = get_tabpage_arg(eap);
	    اگر (eap->errmsg == تہی)
		goto_tabpage(tab_نمبر);
	    توڑو;
    }
}

/*
 * :tabmove command
 */
    ساکن خالی
ex_tabmove(exarg_T *eap)
{
    صحیح tab_نمبر;

    tab_نمبر = get_tabpage_arg(eap);
    اگر (eap->errmsg == تہی)
	tabpage_move(tab_نمبر);
}

/*
 * :tabs command: List tabs and their contents.
 */
    ساکن خالی
ex_tabs(exarg_T *eap غیر_مستعمل)
{
    tabpage_T	*tp;
    win_T	*wp;
    صحیح		tabcount = 1;

    پیغام_آغاز();
    پیغام_scroll = TRUE;
    برائے (tp = پہلا_tabpage; tp != تہی && !got_int; tp = tp->tp_اگلا)
    {
	پیغام_putchar('\n');
	vim_snprintf((حرف *)IObuff, IOSIZE, _("Tab page %d"), tabcount++);
	پیغام_outtrans_attr(IObuff, HL_ATTR(HLF_T));
	برآمد_flush();	    // output one line at a time
	ui_breakcheck();

	اگر (tp  == curtab)
	    wp = firstwin;
	ورنہ
	    wp = tp->tp_firstwin;
	برائے ( ; wp != تہی && !got_int; wp = wp->w_اگلا)
	{
	    پیغام_putchar('\n');
	    پیغام_putchar(wp == curwin ? '>' : ' ');
	    پیغام_putchar(' ');
	    پیغام_putchar(buf_Is_بدلا(wp->w_بفر) ? '+' : ' ');
	    پیغام_putchar(' ');
	    اگر (buf_spname(wp->w_بفر) != تہی)
		vim_strncpy(IObuff, buf_spname(wp->w_بفر), IOSIZE - 1);
	    ورنہ
		home_replace(wp->w_بفر, wp->w_بفر->b_fname,
							IObuff, IOSIZE, TRUE);
	    پیغام_outtrans(IObuff);
	    برآمد_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }
}

/*
 * ":mode": Set screen mode.
 * If no argument given, just get the screen size and redraw.
 */
    ساکن خالی
ex_منوال(exarg_T *eap)
{
    اگر (*eap->arg == NUL)
	shell_resized();
    ورنہ
	emsg(_(e_screen_منوال_setting_not_supported));
}

/*
 * ":resize".
 * set, increment or decrement current window height
 */
    ساکن خالی
ex_resize(exarg_T *eap)
{
    صحیح		n;
    win_T	*wp = curwin;

    اگر (eap->addr_گنتی > 0)
    {
	n = eap->line2;
	برائے (wp = firstwin; wp->w_اگلا != تہی && --n > 0; wp = wp->w_اگلا)
	    ;
    }

#اگر_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#اگر_ختم
    n = atol((حرف *)eap->arg);
    اگر (cmdmod.cmod_بانٹو & WSP_VERT)
    {
	اگر (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_چوڑائی;
	ورنہ اگر (n == 0 && eap->arg[0] == NUL)	// default is very wide
	    n = 9999;
	win_setwidth_win(n, wp);
    }
    ورنہ
    {
	اگر (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_اونچائی;
	ورنہ اگر (n == 0 && eap->arg[0] == NUL)	// default is very high
	    n = 9999;
	win_setheight_win(n, wp);
    }
}

/*
 * ":find [+command] <file>" command.
 */
    ساکن خالی
ex_ڈھونڈو(exarg_T *eap)
{
    اگر (!check_can_سیٹ_curbuf_forceit(eap->forceit))
	واپس;

    char_u	*fname = تہی;
    صحیح		گنتی;
    char_u	*file_مقصد_ڈھونڈو = تہی;
    حرف	*تلاش_ctx = تہی;

    اگر (*get_findfunc() != NUL)
    {
#اگر_تعریف_شدہ FEAT_EVAL
	fname = findfunc_ڈھونڈو_file(eap->arg, (صحیح)STRLEN(eap->arg),
					eap->addr_گنتی > 0 ? eap->line2 : 1);
#اگر_ختم
    }
    ورنہ
    {
	fname = ڈھونڈو_file_اندر_راستہ(eap->arg, (صحیح)STRLEN(eap->arg), FNAME_MESS,
			       TRUE, curbuf->b_ffname, &file_مقصد_ڈھونڈو, &تلاش_ctx);
	اگر (eap->addr_گنتی > 0)
	{
	    // Repeat finding the file "count" times.  This matters when it appears
	    // several times in the path.
	    گنتی = eap->line2;
	    جب (fname != تہی && --گنتی > 0)
	    {
		vim_free(fname);
		fname = ڈھونڈو_file_اندر_راستہ(تہی, 0, FNAME_MESS,
			      FALSE, curbuf->b_ffname, &file_مقصد_ڈھونڈو, &تلاش_ctx);
	    }
	}
	VIM_CLEAR(file_مقصد_ڈھونڈو);
	vim_findfile_cleanup(تلاش_ctx);
    }

    اگر (fname == تہی)
	واپس;

    eap->arg = fname;
    do_exedit(eap, تہی);
    vim_free(fname);
}

/*
 * ":open" simulation: for now works just like ":visual".
 */
    ساکن خالی
ex_open(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*p;

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (not_اندر_vim9(eap, false) == FAIL)
	واپس;
#اگر_ختم
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_SOL | BL_FIX);
    اگر (*eap->arg == '/')
    {
	// ":open /pattern/": put cursor in column found with pattern
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg, magic_isset() ? RE_MAGIC : 0);
	اگر (regmatch.regprog != تہی)
	{
	    // make a copy of the line, when searching for a mark it might be
	    // flushed
	    char_u *سطر = vim_strsave(ml_get_curline());

	    regmatch.rm_ic = p_ic;
	    اگر (vim_regexec(&regmatch, سطر, (colnr_T)0))
		curwin->w_cursor.ستون = (colnr_T)(regmatch.startp[0] - سطر);
	    ورنہ
		emsg(_(e_no_مطابقت));
	    vim_regfree(regmatch.regprog);
	    vim_free(سطر);
	}
	// Move to the NUL, ignore any other arguments.
	eap->arg += STRLEN(eap->arg);
    }
    check_cursor();

    eap->cmdidx = CMD_visual;
    do_exedit(eap, تہی);
}

/*
 * ":edit", ":badd", ":balt", ":visual".
 */
    ساکن خالی
ex_edit(exarg_T *eap)
{
    char_u *ffname = eap->cmdidx == CMD_enew ? تہی : eap->arg;

    // Exclude commands which keep the window's current buffer
    اگر (
	    eap->cmdidx != CMD_badd
	    && eap->cmdidx != CMD_balt
	    // All other commands must obey 'winfixbuf' / ! rules
	    && (is_other_file(0, ffname) && !check_can_سیٹ_curbuf_forceit(eap->forceit))
    )
	واپس;

    do_exedit(eap, تہی);
}

/*
 * ":edit <file>" command and alike.
 */
    خالی
do_exedit(
    exarg_T	*eap,
    win_T	*old_curwin)	    // curwin before doing a split or NULL
{
    صحیح		n;
    صحیح		need_hide;
    صحیح		exmode_was = exmode_فعال;

    اگر ((eap->cmdidx != CMD_pedit && ERROR_IF_POPUP_کھڑکی)
						 || ERROR_IF_رکن_POPUP_کھڑکی)
	واپس;
    /*
     * ":vi" command ends Ex mode.
     */
    اگر (exmode_فعال && (eap->cmdidx == CMD_visual
						|| eap->cmdidx == CMD_view))
    {
	exmode_فعال = FALSE;
	ex_pressedreturn = FALSE;
	اگر (*eap->arg == NUL)
	{
	    // Special case:  ":global/pat/visual\NLvi-commands"
	    اگر (global_busy)
	    {
		اگر (eap->nextcmd != تہی)
		{
		    stuffReadbuff(eap->nextcmd);
		    eap->nextcmd = تہی;
		}

		اگر (exmode_was != EXMODE_VIM)
		    settmode(TMODE_RAW);
		صحیح save_RedrawingDisabled = RedrawingDisabled;
		RedrawingDisabled = 0;
		صحیح save_nwr = no_wait_return;
		no_wait_return = 0;
		need_wait_return = FALSE;
		صحیح save_ms = پیغام_scroll;
		پیغام_scroll = 0;
#اگر_تعریف_شدہ FEAT_GUI
		صحیح save_he = hold_gui_events;
		hold_gui_events = 0;
#اگر_ختم
		سیٹ_must_redraw(UPD_CLEAR);
		pending_exmode_فعال = TRUE;

		main_loop(FALSE, TRUE);

		pending_exmode_فعال = FALSE;
		RedrawingDisabled = save_RedrawingDisabled;
		no_wait_return = save_nwr;
		پیغام_scroll = save_ms;
#اگر_تعریف_شدہ FEAT_GUI
		hold_gui_events = save_he;
#اگر_ختم
	    }
	    واپس;
	}
    }

    اگر ((eap->cmdidx == CMD_new
		|| eap->cmdidx == CMD_tabnew
		|| eap->cmdidx == CMD_tabedit
		|| eap->cmdidx == CMD_vnew) && *eap->arg == NUL)
    {
	// ":new" or ":tabnew" without argument: edit a new empty buffer
	setpcmark();
	(خالی)do_ecmd(0, تہی, تہی, eap, ECMD_ONE,
		      ECMD_HIDE + (eap->forceit ? ECMD_FORCEIT : 0),
		      old_curwin == تہی ? curwin : تہی);
    }
    ورنہ اگر ((eap->cmdidx != CMD_بانٹو && eap->cmdidx != CMD_vsplit)
	    || *eap->arg != NUL
#اگر_تعریف_شدہ FEAT_BROWSE
	    || (cmdmod.cmod_flags & CMOD_BROWSE)
#اگر_ختم
	    )
    {
	// Can't edit another file when "textlock" or "curbuf_lock" is set.
	// Only ":edit" or ":script" can bring us here, others are stopped
	// earlier.
	اگر (*eap->arg != NUL && متن_or_buf_locked())
	    واپس;

	n = readonlymode;
	اگر (eap->cmdidx == CMD_view || eap->cmdidx == CMD_sview)
	    readonlymode = TRUE;
	ورنہ اگر (eap->cmdidx == CMD_enew)
	    readonlymode = FALSE;   // 'readonly' doesn't make sense in an
				    // empty buffer
	اگر (eap->cmdidx != CMD_balt && eap->cmdidx != CMD_badd)
	    setpcmark();
	اگر (do_ecmd(0, (eap->cmdidx == CMD_enew ? تہی : eap->arg),
		    تہی, eap,
		    // ":edit" goes to first line if Vi compatible
		    (*eap->arg == NUL && eap->do_ecmd_lnum == 0
				      && vim_strchr(p_cpo, CPO_GOTO1) != تہی)
					       ? ECMD_ONE : eap->do_ecmd_lnum,
		    (buf_hide(curbuf) ? ECMD_HIDE : 0)
		    + (eap->forceit ? ECMD_FORCEIT : 0)
		      // after a split we can use an existing buffer
		    + (old_curwin != تہی ? ECMD_OLDBUF : 0)
		    + (eap->cmdidx == CMD_badd ? ECMD_ADDBUF : 0)
		    + (eap->cmdidx == CMD_balt ? ECMD_ALTBUF : 0)
		    , old_curwin == تہی ? curwin : تہی) == FAIL)
	{
	    // Editing the file failed.  If the window was split, close it.
	    اگر (old_curwin != تہی)
	    {
		need_hide = (curbuf_Is_بدلا() && curbuf->b_nwindows <= 1);
		اگر (!need_hide || buf_hide(curbuf))
		{
#اگر defined(FEAT_EVAL)
		    cleanup_T   cs;

		    // Reset the error/interrupt/exception state here so that
		    // aborting() returns FALSE when closing a window.
		    enter_cleanup(&cs);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI
		    need_mouse_correct = TRUE;
#اگر_ختم
		    win_close(curwin, !need_hide && !buf_hide(curbuf));

#اگر defined(FEAT_EVAL)
		    // Restore the error/interrupt/exception state if not
		    // discarded by a new aborting error, interrupt, or
		    // uncaught exception.
		    leave_cleanup(&cs);
#اگر_ختم
		}
	    }
	}
	ورنہ اگر (readonlymode && curbuf->b_nwindows == 1)
	{
	    // When editing an already visited buffer, 'readonly' won't be set
	    // but the previous value is kept.  With ":view" and ":sview" we
	    // want the  file to be readonly, except when another window is
	    // editing the same buffer.
	    curbuf->b_p_ro = TRUE;
	}
	readonlymode = n;
    }
    ورنہ
    {
	اگر (eap->do_ecmd_cmd != تہی)
	    do_cmd_دلیل(eap->do_ecmd_cmd);
	n = curwin->w_arg_idx_invalid;
	check_arg_idx(curwin);
	اگر (n != curwin->w_arg_idx_invalid)
	    maketitle();
    }

    /*
     * if ":split file" worked, set alternate file name in old window to new
     * file
     */
    اگر (old_curwin != تہی
	    && *eap->arg != NUL
	    && curwin != old_curwin
	    && win_valid(old_curwin)
	    && old_curwin->w_بفر != curbuf
	    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	old_curwin->w_alt_fnum = curbuf->b_fnum;

    ex_no_reprint = TRUE;
}

#اگر_غیر_تعریف_شدہ FEAT_GUI
/*
 * ":gui" and ":gvim" when there is no GUI.
 */
    ساکن خالی
ex_nogui(exarg_T *eap)
{
    eap->errmsg = _(e_gui_cannot_be_مستعمل_not_enabled_at_compile_وقت);
}
#اگر_ختم

#اگر defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
    ساکن خالی
ex_tearoff(exarg_T *eap)
{
    gui_make_tearoff(eap->arg);
}
#اگر_ختم

#اگر (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_رکن_POPUP_MENU)) && defined(FEAT_MENU)
    ساکن خالی
ex_popup(exarg_T *eap)
{
# اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    اگر (gui.اندر_use)
	gui_make_popup(eap->arg, eap->forceit);
#  ifdef FEAT_رکن_POPUP_MENU
    ورنہ
#  endif
# endif
# ifdef FEAT_رکن_POPUP_MENU
	pum_make_popup(eap->arg, eap->forceit);
# endif
}
#اگر_ختم

    ساکن خالی
ex_swapname(exarg_T *eap غیر_مستعمل)
{
    اگر (curbuf->b_ml.ml_mfp == تہی || curbuf->b_ml.ml_mfp->mf_fname == تہی)
	پیغام(_("No swap file"));
    ورنہ
	پیغام((حرف *)curbuf->b_ml.ml_mfp->mf_fname);
}

/*
 * ":syncbind" forces all 'scrollbind' windows to have the same relative
 * offset.
 * (1998-11-02 16:21:01  R. Edward Ralston <eralston@computer.org>)
 */
    ساکن خالی
ex_syncbind(exarg_T *eap غیر_مستعمل)
{
    win_T	*wp;
    win_T	*save_curwin = curwin;
    buf_T	*save_curbuf = curbuf;
    طویل	topline;
    طویل	y;
    linenr_T	old_linenr = curwin->w_cursor.lnum;

    setpcmark();

    /*
     * determine max topline
     */
    اگر (curwin->w_p_scb)
    {
	topline = curwin->w_topline;
	FOR_ALL_WINDOWS(wp)
	{
	    اگر (wp->w_p_scb && wp->w_بفر)
	    {
		y = wp->w_بفر->b_ml.ml_سطر_گنتی - get_scrolloff_قدر();
		اگر (topline > y)
		    topline = y;
	    }
	}
	اگر (topline < 1)
	    topline = 1;
    }
    ورنہ
    {
	topline = 1;
    }


    /*
     * Set all scrollbind windows to the same topline.
     */
    FOR_ALL_WINDOWS(curwin)
    {
	اگر (curwin->w_p_scb)
	{
	    curbuf = curwin->w_بفر;
	    y = topline - curwin->w_topline;
	    اگر (y > 0)
		scrollup(y, TRUE);
	    ورنہ
		scrolldown(-y, TRUE);
	    curwin->w_scbind_pos = topline;
	    redraw_later(UPD_VALID);
	    cursor_correct();
	    curwin->w_redr_حالت = true;
	}
    }
    curwin = save_curwin;
    curbuf = save_curbuf;
    اگر (curwin->w_p_scb)
    {
	did_syncbind = TRUE;
	checkpcmark();
	اگر (old_linenr != curwin->w_cursor.lnum)
	{
	    char_u ctrl_o[2];

	    ctrl_o[0] = Ctrl_O;
	    ctrl_o[1] = 0;
	    ins_typebuf(ctrl_o, REMAP_NONE, 0, TRUE, FALSE);
	}
    }
}


    ساکن خالی
ex_read(exarg_T *eap)
{
    صحیح		i;
    صحیح		خالی_خانے_2 = (curbuf->b_ml.ml_flags & ML_خالی_خانے);
    linenr_T	lnum;

    اگر (eap->usefilter)			// :r!cmd
    {
	do_bang(1, eap, FALSE, FALSE, TRUE);
	واپس;
    }

    اگر (u_save(eap->line2, (linenr_T)(eap->line2 + 1)) == FAIL)
	واپس;

#اگر_تعریف_شدہ FEAT_BROWSE
    اگر (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browseFile;

	browseFile = do_browse(0, (char_u *)_("Append File"), eap->arg,
		تہی, تہی, تہی, curbuf);
	اگر (browseFile != تہی)
	{
	    i = readfile(browseFile, تہی,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	    vim_free(browseFile);
	}
	ورنہ
	    i = OK;
    }
    ورنہ
#اگر_ختم
	اگر (*eap->arg == NUL)
	{
	    اگر (check_fname() == FAIL)	// check for no file name
		واپس;
	    i = readfile(curbuf->b_ffname, curbuf->b_fname,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	}
	ورنہ
	{
	    اگر (vim_strchr(p_cpo, CPO_ALTREAD) != تہی)
		(خالی)setaltfname(eap->arg, eap->arg, (linenr_T)1);
	    i = readfile(eap->arg, تہی,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);

	}
    اگر (i != OK)
    {
#اگر defined(FEAT_EVAL)
	اگر (!aborting())
#اگر_ختم
	    semsg(_(e_cant_open_file_str), eap->arg);
    }
    ورنہ
    {
	اگر (خالی_خانے_2 && exmode_فعال)
	{
	    // Delete the empty line that remains.  Historically ex does
	    // this but vi doesn't.
	    اگر (eap->line2 == 0)
		lnum = curbuf->b_ml.ml_سطر_گنتی;
	    ورنہ
		lnum = 1;
	    اگر (*ml_get(lnum) == NUL && u_savedel(lnum, 1L) == OK)
	    {
		ml_delete(lnum);
		اگر (curwin->w_cursor.lnum > 1
			&& curwin->w_cursor.lnum >= lnum)
		    --curwin->w_cursor.lnum;
		deleted_سطریں_mark(lnum, 1L);
	    }
	}
	redraw_curbuf_later(UPD_VALID);
    }
}

ساکن char_u	*پچھلا_dir = تہی;

#اگر defined(EXITFREE)
    خالی
free_cd_dir(خالی)
{
    VIM_CLEAR(پچھلا_dir);
    VIM_CLEAR(globaldir);
}
#اگر_ختم

/*
 * Get the previous directory for the given chdir scope.
 */
    ساکن char_u *
get_prevdir(cdscope_T scope)
{
    اگر (scope == CDSCOPE_کھڑکی)
	واپس curwin->w_prevdir;
    ورنہ اگر (scope == CDSCOPE_TABPAGE)
	واپس curtab->tp_prevdir;
    واپس پچھلا_dir;
}

/*
 * Deal with the side effects of changing the current directory.
 * When 'scope' is CDSCOPE_TABPAGE then this was after an ":tcd" command.
 * When 'scope' is CDSCOPE_WINDOW then this was after an ":lcd" command.
 */
    خالی
post_chdir(cdscope_T scope)
{
    اگر (scope != CDSCOPE_کھڑکی)
	// Clear tab local directory for both :cd and :tcd
	VIM_CLEAR(curtab->tp_localdir);
    VIM_CLEAR(curwin->w_localdir);
    اگر (scope != CDSCOPE_GLOBAL)
    {
	char_u	*pdir = get_prevdir(scope);

	// If still in the global directory, need to remember current
	// directory as the global directory.
	اگر (globaldir == تہی && pdir != تہی)
	    globaldir = vim_strsave(pdir);

	// Remember this local directory for the window.
	اگر (mch_dirname(NameBuff, MAXPATHL) == OK)
	{
	    اگر (scope == CDSCOPE_TABPAGE)
		curtab->tp_localdir = vim_strsave(NameBuff);
	    ورنہ
		curwin->w_localdir = vim_strsave(NameBuff);
	}
    }
    ورنہ
    {
	// We are now in the global directory, no need to remember its name.
	VIM_CLEAR(globaldir);
    }

    آخری_chdir_reason = تہی;
    shorten_fnames(vim_strchr(p_cpo, CPO_NOSYMLINKS) == تہی);
}

/*
 * Trigger DirChangedPre for "acmd_fname" with directory "new_dir".
 */
    خالی
trigger_Dir_بدلا_Pre(char_u *acmd_fname, char_u *new_dir)
{
#اگر_تعریف_شدہ FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;

    v_event = get_v_event(&save_v_event);
    (خالی)dict_جوڑو_string(v_event, "directory", new_dir);
    dict_سیٹ_اشیاء_ro(v_event);
#اگر_ختم
    apply_autocmds(EVENT_DIRCHANGEDPRE, acmd_fname, new_dir, FALSE, curbuf);
#اگر_تعریف_شدہ FEAT_EVAL
    restore_v_event(v_event, &save_v_event);
#اگر_ختم
}

/*
 * Change directory function used by :cd/:tcd/:lcd Ex commands and the
 * chdir() function.
 * scope == CDSCOPE_WINDOW: changes the window-local directory
 * scope == CDSCOPE_TABPAGE: changes the tab-local directory
 * Otherwise: changes the global directory
 * Returns TRUE if the directory is successfully changed.
 */
    صحیح
changedir_func(
	char_u		*new_dir,
	صحیح		forceit,
	cdscope_T	scope)
{
    char_u	*pdir = تہی;
    صحیح		dir_differs;
    char_u	*acmd_fname = تہی;
    char_u	**pp;
    char_u	*tofree;

    اگر (new_dir == تہی || allbuf_locked())
	واپس FALSE;

    اگر (vim_strchr(p_cpo, CPO_CHDIR) != تہی && curbuf_Is_بدلا() && !forceit)
    {
	emsg(_(e_cannot_change_directory_بفر_is_modified_جوڑو_bang_مقصد_override));
	واپس FALSE;
    }

    // ":cd -": Change to previous directory
    اگر (STRCMP(new_dir, "-") == 0)
    {
	pdir = get_prevdir(scope);
	اگر (pdir == تہی)
	{
	    emsg(_(e_no_پچھلا_directory));
	    واپس FALSE;
	}
	new_dir = pdir;
    }

    // Save current directory for next ":cd -"
    اگر (mch_dirname(NameBuff, MAXPATHL) == OK)
	pdir = vim_strsave(NameBuff);
    ورنہ
	pdir = تہی;

    // For UNIX ":cd" means: go to home directory.
    // On other systems too if 'cdhome' is set.
#اگر defined(UNIX) || defined(VMS)
    اگر (*new_dir == NUL)
#ورنہ
    اگر (*new_dir == NUL && p_cdh)
#اگر_ختم
    {
	// use NameBuff for home directory name
#اگر_تعریف_شدہ VMS
	char_u	*p;

	p = mch_getenv((char_u *)"SYS$LOGIN");
	اگر (p == تہی || *p == NUL)	// empty is the same as not set
	    NameBuff[0] = NUL;
	ورنہ
	    vim_strncpy(NameBuff, p, MAXPATHL - 1);
#ورنہ
	expand_env((char_u *)"$HOME", NameBuff, MAXPATHL);
#اگر_ختم
	new_dir = NameBuff;
    }
    dir_differs = pdir == تہی
			    || pathcmp((حرف *)pdir, (حرف *)new_dir, -1) != 0;
    اگر (dir_differs)
    {
	اگر (scope == CDSCOPE_کھڑکی)
	    acmd_fname = (char_u *)"window";
	ورنہ اگر (scope == CDSCOPE_TABPAGE)
	    acmd_fname = (char_u *)"tabpage";
	ورنہ
	    acmd_fname = (char_u *)"global";
	trigger_Dir_بدلا_Pre(acmd_fname, new_dir);

	اگر (vim_chdir(new_dir))
	{
	    emsg(_(e_command_failed));
	    vim_free(pdir);
	    واپس FALSE;
	}
    }

    اگر (scope == CDSCOPE_کھڑکی)
	pp = &curwin->w_prevdir;
    ورنہ اگر (scope == CDSCOPE_TABPAGE)
	pp = &curtab->tp_prevdir;
    ورنہ
	pp = &پچھلا_dir;
    tofree = *pp;  // new_dir may use this
    *pp = pdir;

    post_chdir(scope);

    اگر (dir_differs)
	apply_autocmds(EVENT_DIRCHANGED, acmd_fname, new_dir, FALSE, curbuf);
    vim_free(tofree);
    واپس TRUE;
}

/*
 * ":cd", ":tcd", ":lcd", ":chdir" ":tchdir" and ":lchdir".
 */
    خالی
ex_cd(exarg_T *eap)
{
    char_u	*new_dir;

    new_dir = eap->arg;
#اگر !defined(UNIX) && !defined(VMS)
    // for non-UNIX ":cd" means: print current directory unless 'cdhome' is set
    اگر (*new_dir == NUL && !p_cdh)
    {
	ex_pwd(تہی);
	واپس;
    }
#اگر_ختم

    cdscope_T	scope = CDSCOPE_GLOBAL;

    اگر (eap->cmdidx == CMD_lcd || eap->cmdidx == CMD_lchdir)
	scope = CDSCOPE_کھڑکی;
    ورنہ اگر (eap->cmdidx == CMD_tcd || eap->cmdidx == CMD_tchdir)
	scope = CDSCOPE_TABPAGE;

    اگر (changedir_func(new_dir, eap->forceit, scope))
    {
	// Echo the new current directory if the command was typed.
	اگر (کلید_Typed || p_verbose >= 5)
	    ex_pwd(eap);
    }
}

/*
 * ":pwd".
 */
    ساکن خالی
ex_pwd(exarg_T *eap غیر_مستعمل)
{
    اگر (mch_dirname(NameBuff, MAXPATHL) == OK)
    {
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	slash_adjust(NameBuff);
#اگر_ختم
	اگر (p_verbose > 0)
	{
	    حرف *context = "global";

	    اگر (آخری_chdir_reason != تہی)
		context = آخری_chdir_reason;
	    ورنہ اگر (curwin->w_localdir != تہی)
		context = "window";
	    ورنہ اگر (curtab->tp_localdir != تہی)
		context = "tabpage";
	    smsg("[%s] %s", context, (حرف *)NameBuff);
	}
	ورنہ
	    پیغام((حرف *)NameBuff);
    }
    ورنہ
	emsg(_(e_directory_unknown));
}

/*
 * ":=".
 */
    ساکن خالی
ex_equal(exarg_T *eap)
{
    smsg("%ld", (طویل)eap->line2);
    ex_may_print(eap);
}

    ساکن خالی
ex_sleep(exarg_T *eap)
{
    صحیح		n;
    طویل	len;

    اگر (cursor_valid())
    {
	n = W_WINROW(curwin) + curwin->w_wrow - پیغام_scrolled;
	اگر (n >= 0)
	    windgoto(n, curwin->w_wincol + curwin->w_wcol);
    }

    len = eap->line2;
    انتخاب (*eap->arg)
    {
	حالت 'm': توڑو;
	حالت NUL: len *= 1000L; توڑو;
	طےشدہ: semsg(_(e_invalid_دلیل_str), eap->arg); واپس;
    }

    // Hide the cursor if invoked with !
    do_sleep(len, eap->forceit);
}

/*
 * Sleep for "msec" milliseconds, but keep checking for a CTRL-C every second.
 * Hide the cursor if "hide_cursor" is TRUE.
 */
    خالی
do_sleep(طویل msec, صحیح hide_cursor)
{
    طویل	مکمل = 0;
    طویل	wait_now;
#اگر_تعریف_شدہ ELAPSED_FUNC
    elapsed_T	آغاز_tv;

    // Remember at what time we started, so that we know how much longer we
    // should wait after waiting for a bit.
    ELAPSED_INIT(آغاز_tv);
#اگر_ختم

    اگر (hide_cursor)
	cursor_sleep();
    ورنہ
	cursor_on();

    برآمد_flush_cursor(FALSE, FALSE);
    جب (!got_int && مکمل < msec)
    {
	wait_now = msec - مکمل > 1000L ? 1000L : msec - مکمل;
#اگر_تعریف_شدہ FEAT_TIMERS
	{
	    طویل    due_وقت = check_due_timer();

	    اگر (due_وقت > 0 && due_وقت < wait_now)
		wait_now = due_وقت;
	}
#اگر_ختم
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	اگر (has_any_channel() && wait_now > 20L)
	    wait_now = 20L;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SOUND
	اگر (has_any_sound_callback() && wait_now > 20L)
	    wait_now = 20L;
#اگر_ختم
	ui_delay(wait_now, TRUE);

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	اگر (has_any_channel())
	    ui_breakcheck_force(TRUE);
	ورنہ
#اگر_ختم
	    ui_breakcheck();
#اگر_تعریف_شدہ پیغام_قطار
	// Process the netbeans and clientserver messages that may have been
	// received in the call to ui_breakcheck() when the GUI is in use. This
	// may occur when running a test case.
	parse_queued_messages();
#اگر_ختم

#اگر_تعریف_شدہ ELAPSED_FUNC
	// actual time passed
	مکمل = ELAPSED_FUNC(آغاز_tv);
#ورنہ
	// guesstimate time passed (will actually be more)
	مکمل += wait_now;
#اگر_ختم
    }

    // If CTRL-C was typed to interrupt the sleep, drop the CTRL-C from the
    // input buffer, otherwise a following call to input() fails.
    اگر (got_int)
	(خالی)vpeekc();

    اگر (hide_cursor)
	cursor_unsleep();
}

/*
 * ":winsize" command (obsolete).
 */
    ساکن خالی
ex_winsize(exarg_T *eap)
{
    صحیح		w, h;
    char_u	*arg = eap->arg;
    char_u	*p;

    اگر (!محفوظ_isdigit(*arg))
    {
	semsg(_(e_invalid_دلیل_str), arg);
	واپس;
    }
    w = getdigits(&arg);
    arg = skipwhite(arg);
    p = arg;
    h = getdigits(&arg);
    اگر (*p != NUL && *arg == NUL)
	سیٹ_shellsize(w, h, TRUE);
    ورنہ
	emsg(_(e_winsize_requires_two_نمبر_arguments));
}

    ساکن خالی
ex_wincmd(exarg_T *eap)
{
    صحیح		xchar = NUL;
    char_u	*p;

    اگر (*eap->arg == 'g' || *eap->arg == Ctrl_G)
    {
	// CTRL-W g and CTRL-W CTRL-G  have an extra command character
	اگر (eap->arg[1] == NUL)
	{
	    emsg(_(e_invalid_دلیل));
	    واپس;
	}
	xchar = eap->arg[1];
	p = eap->arg + 2;
    }
    ورنہ
	p = eap->arg + 1;

    سیٹ_nextcmd(eap, p);
    p = skipwhite(p);
    اگر (*p != NUL && *p != (
#اگر_تعریف_شدہ FEAT_EVAL
	    اندر_vim9script() ? '#' :
#اگر_ختم
		'"')
	    && eap->nextcmd == تہی)
	emsg(_(e_invalid_دلیل));
    ورنہ اگر (!eap->skip)
    {
	// Pass flags on for ":vertical wincmd ]".
	postponed_بانٹو_flags = cmdmod.cmod_بانٹو;
	postponed_بانٹو_tab = cmdmod.cmod_tab;
	do_کھڑکی(*eap->arg, eap->addr_گنتی > 0 ? eap->line2 : 0L, xchar);
	postponed_بانٹو_flags = 0;
	postponed_بانٹو_tab = 0;
    }
}

#اگر defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * ":winpos".
 */
    ساکن خالی
ex_winpos(exarg_T *eap)
{
    صحیح		x, y;
    char_u	*arg = eap->arg;
    char_u	*p;

    اگر (*arg == NUL)
    {
# اگر defined(FEAT_GUI) || defined(MSWIN)
#  ifdef VIMDLL
	اگر (gui.اندر_use ? gui_mch_get_winpos(&x, &y) != FAIL :
		mch_get_winpos(&x, &y) != FAIL)
#  elif defined(FEAT_GUI)
	اگر (gui.اندر_use && gui_mch_get_winpos(&x, &y) != FAIL)
#  ورنہ
	اگر (mch_get_winpos(&x, &y) != FAIL)
#  endif
	{
	    sprintf((حرف *)IObuff, _("Window position: X %d, Y %d"), x, y);
	    پیغام((حرف *)IObuff);
	}
	ورنہ
# endif
	    emsg(_(e_obtaining_کھڑکی_position_not_implemented_for_this_platform));
    }
    ورنہ
    {
	x = getdigits(&arg);
	arg = skipwhite(arg);
	p = arg;
	y = getdigits(&arg);
	اگر (*p == NUL || *arg != NUL)
	{
	    emsg(_(e_winpos_requires_two_نمبر_arguments));
	    واپس;
	}
# ifdef FEAT_GUI
	اگر (gui.اندر_use)
	    gui_mch_سیٹ_winpos(x, y);
	ورنہ اگر (gui.starting)
	{
	    // Remember the coordinates for when the window is opened.
	    gui_win_x = x;
	    gui_win_y = y;
	}
#  اگر defined(HAVE_TGETENT) || defined(VIMDLL)
	ورنہ
#  endif
# endif
# اگر defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	    mch_سیٹ_winpos(x, y);
# endif
# ifdef HAVE_TGETENT
	اگر (*T_CWP)
	    رکن_سیٹ_winpos(x, y);
# endif
    }
}
#اگر_ختم

/*
 * Handle command that work like operators: ":delete", ":yank", ":>" and ":<".
 */
    ساکن خالی
ex_operators(exarg_T *eap)
{
    oparg_T	oa;

    clear_oparg(&oa);
    oa.regname = eap->regname;
    oa.آغاز.lnum = eap->line1;
    oa.انتہا.lnum = eap->line2;
    oa.سطر_گنتی = eap->line2 - eap->line1 + 1;
    oa.motion_type = MLINE;
    virtual_op = FALSE;
    اگر (eap->cmdidx != CMD_yank)	// position cursor for undo
    {
	setpcmark();
	curwin->w_cursor.lnum = eap->line1;
	beginline(BL_SOL | BL_FIX);
    }

    اگر (VIsual_فعال)
	انتہا_visual_منوال();

    انتخاب (eap->cmdidx)
    {
	حالت CMD_delete:
	    oa.op_type = OP_DELETE;
	    op_delete(&oa);
	    توڑو;

	حالت CMD_yank:
	    oa.op_type = OP_YANK;
	    (خالی)op_yank(&oa, FALSE, TRUE);
	    توڑو;

	طےشدہ:    // CMD_rshift or CMD_lshift
	    اگر (
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		(eap->cmdidx == CMD_rshift) ^ curwin->w_p_rl
#ورنہ
		eap->cmdidx == CMD_rshift
#اگر_ختم
						)
		oa.op_type = OP_RSHIFT;
	    ورنہ
		oa.op_type = OP_LSHIFT;
	    op_سرکاؤ(&oa, FALSE, eap->amount);
	    توڑو;
    }
    virtual_op = MAYBE;
    ex_may_print(eap);
}

/*
 * ":put".
 */
    ساکن خالی
ex_put(exarg_T *eap)
{
    // ":0put" works like ":1put!".
    اگر (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_ستون();
    do_put(eap->regname, تہی, eap->forceit ? BACKWARD : FORWARD, 1L,
						       PUT_سطر|PUT_CURSLINE);
}

/*
 * ":iput".
 */
    ساکن خالی
ex_iput(exarg_T *eap)
{
    // ":0iput" works like ":1iput!".
    اگر (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_ستون();
    do_put(eap->regname, تہی, eap->forceit ? BACKWARD : FORWARD, 1L,
						      PUT_سطر|PUT_CURSLINE
						      |PUT_FIXINDENT);
}

/*
 * Handle ":copy" and ":move".
 */
    ساکن خالی
ex_copymove(exarg_T *eap)
{
    طویل	n;

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (not_اندر_vim9(eap, false) == FAIL)
	واپس;
#اگر_ختم
    n = get_پتہ(eap, &eap->arg, eap->addr_type, FALSE, FALSE, FALSE, 1);
    اگر (eap->arg == تہی)	    // error detected
    {
	eap->nextcmd = تہی;
	واپس;
    }
    get_flags(eap);

    /*
     * move or copy lines from 'eap->line1'-'eap->line2' to below line 'n'
     */
    اگر (n == MAXLNUM || n < 0 || n > curbuf->b_ml.ml_سطر_گنتی)
    {
	emsg(_(e_invalid_range));
	واپس;
    }

    اگر (eap->cmdidx == CMD_move)
    {
	اگر (do_move(eap->line1, eap->line2, n) == FAIL)
	    واپس;
    }
    ورنہ
	ex_copy(eap->line1, eap->line2, n);
    u_clearline();
    beginline(BL_SOL | BL_FIX);
    ex_may_print(eap);
}

/*
 * Print the current line if flags were given to the Ex command.
 */
    خالی
ex_may_print(exarg_T *eap)
{
    اگر (eap->flags != 0)
    {
	print_سطر(curwin->w_cursor.lnum, (eap->flags & EXFLAG_NR),
						  (eap->flags & EXFLAG_LIST));
	ex_no_reprint = TRUE;
    }
}

/*
 * ":smagic" and ":snomagic".
 */
    ساکن خالی
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
    ساکن خالی
ex_join(exarg_T *eap)
{
    curwin->w_cursor.lnum = eap->line1;
    اگر (eap->line1 == eap->line2)
    {
	اگر (eap->addr_گنتی >= 2)   // :2,2join does nothing
	    واپس;
	اگر (eap->line2 == curbuf->b_ml.ml_سطر_گنتی)
	{
	    beep_flush();
	    واپس;
	}
	++eap->line2;
    }
    (خالی)do_join(eap->line2 - eap->line1 + 1, !eap->forceit, TRUE, TRUE, TRUE);
    beginline(BL_WHITE | BL_FIX);
    ex_may_print(eap);
}

/*
 * ":[addr]@r" or ":[addr]*r": execute register
 */
    ساکن خالی
ex_at(exarg_T *eap)
{
    صحیح		c;
    صحیح		پچھلا_len = typebuf.tb_len;

    curwin->w_cursor.lnum = eap->line2;
    check_cursor_ستون();

#اگر_تعریف_شدہ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#اگر_ختم

    // get the register name.  No name means to use the previous one
    c = *eap->arg;
    اگر (c == NUL || (c == '*' && *eap->cmd == '*'))
	c = '@';
    // Put the register in the typeahead buffer with the "silent" flag.
    اگر (do_execreg(c, TRUE, vim_strchr(p_cpo, CPO_EXECBUF) != تہی, TRUE)
								      == FAIL)
    {
	beep_flush();
	واپس;
    }

    صحیح	save_efr = exec_from_reg;

    exec_from_reg = TRUE;

    /*
     * Execute from the typeahead buffer.
     * Continue until the stuff buffer is empty and all added characters
     * have been consumed.
     */
    جب (!stuff_خالی_خانے() || typebuf.tb_len > پچھلا_len)
	(خالی)do_cmdline(تہی, getexline, تہی, DOCMD_NOWAIT|DOCMD_VERBOSE);

    exec_from_reg = save_efr;
}

/*
 * ":!".
 */
    ساکن خالی
ex_bang(exarg_T *eap)
{
    do_bang(eap->addr_گنتی, eap, eap->forceit, TRUE, TRUE);
}

/*
 * ":undo".
 */
    ساکن خالی
ex_undo(exarg_T *eap)
{
    اگر (eap->addr_گنتی == 1)	    // :undo 123
	undo_وقت(eap->line2, FALSE, FALSE, TRUE);
    ورنہ
	u_undo(1);
}

#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
    ساکن خالی
ex_wundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_سائز];

    u_compute_hash(hash);
    u_write_undo(eap->arg, eap->forceit, curbuf, hash);
}

    ساکن خالی
ex_rundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_سائز];

    u_compute_hash(hash);
    u_read_undo(eap->arg, hash, تہی);
}
#اگر_ختم

/*
 * ":redo".
 */
    ساکن خالی
ex_redo(exarg_T *eap غیر_مستعمل)
{
    u_redo(1);
}

/*
 * ":earlier" and ":later".
 */
    ساکن خالی
ex_later(exarg_T *eap)
{
    طویل	گنتی = 0;
    صحیح		sec = FALSE;
    صحیح		file = FALSE;
    char_u	*p = eap->arg;

    اگر (*p == NUL)
	گنتی = 1;
    ورنہ اگر (محفوظ_isdigit(*p))
    {
	گنتی = getdigits(&p);
	انتخاب (*p)
	{
	    حالت 's': ++p; sec = TRUE; توڑو;
	    حالت 'm': ++p; sec = TRUE; گنتی *= 60; توڑو;
	    حالت 'h': ++p; sec = TRUE; گنتی *= 60 * 60; توڑو;
	    حالت 'd': ++p; sec = TRUE; گنتی *= 24 * 60 * 60; توڑو;
	    حالت 'f': ++p; file = TRUE; توڑو;
	}
    }

    اگر (*p != NUL)
	semsg(_(e_invalid_دلیل_str), eap->arg);
    ورنہ
	undo_وقت(eap->cmdidx == CMD_earlier ? -گنتی : گنتی,
							    sec, file, FALSE);
}

/*
 * ":redir": start/stop redirection.
 */
    ساکن خالی
ex_redir(exarg_T *eap)
{
    حرف	*منوال;
    char_u	*fname;
    char_u	*arg = eap->arg;

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (redir_execute)
    {
	emsg(_(e_cannot_use_redir_inside_execute));
	واپس;
    }
#اگر_ختم

    اگر (STRICMP(eap->arg, "END") == 0)
	close_redir();
    ورنہ
    {
	اگر (*arg == '>')
	{
	    ++arg;
	    اگر (*arg == '>')
	    {
		++arg;
		منوال = "a";
	    }
	    ورنہ
		منوال = "w";
	    arg = skipwhite(arg);

	    close_redir();

	    // Expand environment variables and "~/".
	    fname = expand_env_save(arg);
	    اگر (fname == تہی)
		واپس;
#اگر_تعریف_شدہ FEAT_BROWSE
	    اگر (cmdmod.cmod_flags & CMOD_BROWSE)
	    {
		char_u	*browseFile;

		browseFile = do_browse(BROWSE_SAVE,
			(char_u *)_("Save Redirection"),
			fname, تہی, تہی,
			(char_u *)_(BROWSE_FILTER_ALL_FILES), curbuf);
		اگر (browseFile == تہی)
		    واپس;		// operation cancelled
		vim_free(fname);
		fname = browseFile;
		eap->forceit = TRUE;	// since dialog already asked
	    }
#اگر_ختم

	    redir_fd = open_exfile(fname, eap->forceit, منوال);
	    vim_free(fname);
	}
#اگر_تعریف_شدہ FEAT_EVAL
	ورنہ اگر (*arg == '@')
	{
	    // redirect to a register a-z (resp. A-Z for appending)
	    close_redir();
	    ++arg;
	    اگر (ASCII_ISALPHA(*arg)
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
		اگر (*arg == '>' && arg[1] == '>')  // append
		    arg += 2;
		ورنہ
		{
		    // Can use both "@a" and "@a>".
		    اگر (*arg == '>')
			arg++;
		    // Make register empty when not using @A-@Z and the
		    // command is valid.
		    اگر (*arg == NUL && !محفوظ_isupper(redir_reg))
			write_reg_contents(redir_reg, (char_u *)"", -1, FALSE);
		}
	    }
	    اگر (*arg != NUL)
	    {
		redir_reg = 0;
		semsg(_(e_invalid_دلیل_str), eap->arg);
	    }
	}
	ورنہ اگر (*arg == '=' && arg[1] == '>')
	{
	    صحیح append;

	    // redirect to a variable
	    close_redir();
	    arg += 2;

	    اگر (*arg == '>')
	    {
		++arg;
		append = TRUE;
	    }
	    ورنہ
		append = FALSE;

	    اگر (var_redir_آغاز(skipwhite(arg), append) == OK)
		redir_vname = 1;
	}
#اگر_ختم

	// TODO: redirect to a buffer

	ورنہ
	    semsg(_(e_invalid_دلیل_str), eap->arg);
    }

    // Make sure redirection is not off.  Can happen for cmdline completion
    // that indirectly invokes a command to catch its output.
    اگر (redir_fd != تہی
#اگر_تعریف_شدہ FEAT_EVAL
			  || redir_reg || redir_vname
#اگر_ختم
							)
	redir_off = FALSE;
}

/*
 * ":redraw": force redraw, with clear for ":redraw!".
 */
    خالی
ex_redraw(exarg_T *eap)
{
    redraw_cmd(eap->forceit);
}

/*
 * ":redraw": force redraw, with clear if "clear" is TRUE.
 */
    خالی
redraw_cmd(صحیح clear)
{
    صحیح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    صحیح save_p_lz = p_lz;
    p_lz = FALSE;

    validate_cursor();
    update_topline();
    update_screen(clear ? UPD_CLEAR : VIsual_فعال ? UPD_INVERTED : 0);
    اگر ((State & منوال_CMDLINE) == 0)
	setcursor(); // put cursor back where it belongs
    اگر (need_maketitle)
	maketitle();
#اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
    اگر (!gui.اندر_use)
# endif
	resize_console_buf();
#اگر_ختم
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;

    // After drawing the statusline screen_attr may still be set.
    screen_stop_highlight();

    // Reset msg_didout, so that a message that's there is overwritten.
    پیغام_didout = FALSE;
    پیغام_ستون = 0;

    // No need to wait after an intentional redraw.
    need_wait_return = FALSE;

    // When invoked from a callback or autocmd the command line may be active.
    اگر (State & منوال_CMDLINE)
	redrawcmdline();

    برآمد_flush();
}

/*
 * ":redrawstatus": force redraw of status line(s)
 */
    ساکن خالی
ex_redrawstatus(exarg_T *eap غیر_مستعمل)
{
    اگر (eap->forceit)
	حالت_redraw_all();
    ورنہ
	حالت_redraw_curbuf();
    redraw_vseps = TRUE;
    اگر (پیغام_scrolled && (State & منوال_CMDLINE))
	واپس;  // redraw later

    صحیح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    صحیح save_p_lz = p_lz;
    p_lz = FALSE;

    اگر (State & منوال_CMDLINE)
	redraw_statuslines();
    ورنہ
	update_screen(VIsual_فعال ? UPD_INVERTED : 0);
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    برآمد_flush();
}

/*
 * ":redrawtabline": force redraw of the tabline
 */
    ساکن خالی
ex_redrawtabline(exarg_T *eap غیر_مستعمل)
{
    صحیح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    صحیح save_p_lz = p_lz;
    p_lz = FALSE;

    draw_tabline();

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    برآمد_flush();
}

/*
 * ":redrawtabpanel": force redraw of the tabpanel
 */
    ساکن خالی
ex_redrawtabpanel(exarg_T *eap غیر_مستعمل)
{
    صحیح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    صحیح save_p_lz = p_lz;
    p_lz = FALSE;

#اگر defined(FEAT_TABPANEL)
    draw_tabpanel();
#ورنہ
    emsg(_(e_cannot_not_support_redrawtabpanel));
#اگر_ختم

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    برآمد_flush();
}

    ساکن خالی
close_redir(خالی)
{
    اگر (redir_fd != تہی)
    {
	fclose(redir_fd);
	redir_fd = تہی;
    }
#اگر_تعریف_شدہ FEAT_EVAL
    redir_reg = 0;
    اگر (redir_vname)
    {
	var_redir_stop();
	redir_vname = 0;
    }
#اگر_ختم
}

#اگر defined(FEAT_SESSION) || defined(FEAT_EVAL)
    صحیح
vim_mkdir_emsg(char_u *name, صحیح prot غیر_مستعمل)
{
    اگر (vim_mkdir(name, prot) != 0)
    {
	semsg(_(e_cannot_create_directory_str), name);
	واپس FAIL;
    }
    واپس OK;
}
#اگر_ختم

/*
 * Open a file for writing for an Ex command, with some checks.
 * Return file descriptor, or NULL on failure.
 */
    FILE *
open_exfile(
    char_u	*fname,
    صحیح		forceit,
    حرف	*منوال)	    // "w" for create new file or "a" for append
{
    FILE	*fd;

#اگر_تعریف_شدہ UNIX
    // with Unix it is possible to open a directory
    اگر (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	واپس تہی;
    }
#اگر_ختم
    اگر (!forceit && *منوال != 'a' && vim_fexists(fname))
    {
	semsg(_(e_str_exists_جوڑو_bang_مقصد_override), fname);
	واپس تہی;
    }

    اگر ((fd = mch_fopen((حرف *)fname, منوال)) == تہی)
	semsg(_(e_cannot_open_str_for_writing_2), fname);

    واپس fd;
}

/*
 * ":mark" and ":k".
 */
    ساکن خالی
ex_mark(exarg_T *eap)
{
    pos_T	pos;

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (not_اندر_vim9(eap, false) == FAIL)
	واپس;
#اگر_ختم
    اگر (*eap->arg == NUL)		// No argument?
    {
	emsg(_(e_دلیل_required));
	واپس;
    }

    اگر (eap->arg[1] != NUL)	// more than one character?
    {
	semsg(_(e_trailing_characters_str), eap->arg);
	واپس;
    }

    pos = curwin->w_cursor;		// save curwin->w_cursor
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_WHITE | BL_FIX);
    اگر (setmark(*eap->arg) == FAIL)	// set mark
	emsg(_(e_دلیل_must_be_letter_or_forward_backward_quote));
    curwin->w_cursor = pos;		// restore curwin->w_cursor
}

/*
 * Update w_topline, w_leftcol and the cursor position.
 */
    خالی
update_topline_cursor(خالی)
{
    check_cursor();		// put cursor on valid line
    update_topline();
    اگر (!curwin->w_p_wrap)
	validate_cursor();
    update_curswant();
}

/*
 * Save the current State and go to Normal mode.
 * Return TRUE if the typeahead could be saved.
 */
    صحیح
save_current_state(save_state_T *sst)
{
    sst->save_پیغام_scroll = پیغام_scroll;
    sst->save_restart_edit = restart_edit;
    sst->save_پیغام_didout = پیغام_didout;
    sst->save_State = State;
    sst->save_insertmode = p_im;
    sst->save_اختتام_op = اختتام_op;
    sst->save_opcount = opcount;
    sst->save_reg_executing = reg_executing;
    sst->save_pending_انتہا_reg_executing = pending_انتہا_reg_executing;

    پیغام_scroll = FALSE;		    // no msg scrolling in Normal mode
    restart_edit = 0;		    // don't go to Insert mode
    p_im = FALSE;		    // don't use 'insertmode'

    sst->save_script_نسخہ = current_sctx.sc_نسخہ;
    current_sctx.sc_نسخہ = 1;    // not in Vim9 script

    /*
     * Save the current typeahead.  This is required to allow using ":normal"
     * from an event handler and makes sure we don't hang when the argument
     * ends with half a command.
     */
    save_typeahead(&sst->tabuf);
    واپس sst->tabuf.typebuf_valid;
}

    خالی
restore_current_state(save_state_T *sst)
{
    // Restore the previous typeahead.
    restore_typeahead(&sst->tabuf, FALSE);

    پیغام_scroll = sst->save_پیغام_scroll;
    restart_edit = sst->save_restart_edit;
    p_im = sst->save_insertmode;
    اختتام_op = sst->save_اختتام_op;
    opcount = sst->save_opcount;
    reg_executing = sst->save_reg_executing;
    pending_انتہا_reg_executing = sst->save_pending_انتہا_reg_executing;
    پیغام_didout |= sst->save_پیغام_didout;	// don't reset msg_didout now
    current_sctx.sc_نسخہ = sst->save_script_نسخہ;

    // Restore the state (needed when called from a function executed for
    // 'indentexpr'). Update the mouse and cursor, they may have changed.
    State = sst->save_State;
#اگر_تعریف_شدہ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#اگر_ختم
}

/*
 * ":normal[!] {commands}": Execute normal mode commands.
 */
    خالی
ex_معمول(exarg_T *eap)
{
    save_state_T save_state;
    char_u	*arg = تہی;
    صحیح		l;
    char_u	*p;

    اگر (ex_معمول_تالا > 0)
    {
	emsg(_(e_not_allowed_یہاں));
	واپس;
    }
    اگر (ex_معمول_busy >= p_mmd)
    {
	emsg(_(e_recursive_use_کا_معمول_too_deep));
	واپس;
    }

    /*
     * vgetc() expects a CSI and K_SPECIAL to have been escaped.  Don't do
     * this for the K_SPECIAL leading byte, otherwise special keys will not
     * work.
     */
    اگر (has_mbyte)
    {
	صحیح	len = 0;

	// Count the number of characters to be escaped.
	برائے (p = eap->arg; *p != NUL; ++p)
	{
#اگر_تعریف_شدہ FEAT_GUI
	    اگر (*p == CSI)  // leadbyte CSI
		len += 2;
#اگر_ختم
	    برائے (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		اگر (*++p == K_SPECIAL	  // trailbyte K_SPECIAL or CSI
#اگر_تعریف_شدہ FEAT_GUI
			|| *p == CSI
#اگر_ختم
			)
		    len += 2;
	}
	اگر (len > 0)
	{
	    arg = alloc(STRLEN(eap->arg) + len + 1);
	    اگر (arg != تہی)
	    {
		len = 0;
		برائے (p = eap->arg; *p != NUL; ++p)
		{
		    arg[len++] = *p;
#اگر_تعریف_شدہ FEAT_GUI
		    اگر (*p == CSI)
		    {
			arg[len++] = KS_EXTRA;
			arg[len++] = (صحیح)KE_CSI;
		    }
#اگر_ختم
		    برائے (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		    {
			arg[len++] = *++p;
			اگر (*p == K_SPECIAL)
			{
			    arg[len++] = KS_SPECIAL;
			    arg[len++] = KE_FILLER;
			}
#اگر_تعریف_شدہ FEAT_GUI
			ورنہ اگر (*p == CSI)
			{
			    arg[len++] = KS_EXTRA;
			    arg[len++] = (صحیح)KE_CSI;
			}
#اگر_ختم
		    }
		    arg[len] = NUL;
		}
	    }
	}
    }

    ++ex_معمول_busy;
    اگر (save_current_state(&save_state))
    {
	/*
	 * Repeat the :normal command for each line in the range.  When no
	 * range given, execute it just once, without positioning the cursor
	 * first.
	 */
	کرو
	{
	    اگر (eap->addr_گنتی != 0)
	    {
		curwin->w_cursor.lnum = eap->line1++;
		curwin->w_cursor.ستون = 0;
		check_cursor_moved(curwin);
	    }

	    exec_معمول_cmd(arg != تہی
		     ? arg
		     : eap->arg, eap->forceit ? REMAP_NONE : REMAP_YES, FALSE);
	}
	جب (eap->addr_گنتی > 0 && eap->line1 <= eap->line2 && !got_int);
    }

    // Might not return to the main loop when in an event handler.
    update_topline_cursor();

    restore_current_state(&save_state);
    --ex_معمول_busy;
    setmouse();
#اگر_تعریف_شدہ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#اگر_ختم

    vim_free(arg);
}

/*
 * ":startinsert", ":startreplace" and ":startgreplace"
 */
    ساکن خالی
ex_startinsert(exarg_T *eap)
{
#اگر_تعریف_شدہ FEAT_TERMINAL
    // Ignore this when running in an active terminal.
    اگر (رکن_کام_running(curbuf->b_رکن))
	واپس;
#اگر_ختم
    اگر (!curbuf->b_p_ma && !p_im)
    {
	// Only give this error when 'insertmode' is off.
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	واپس;
    }
    اگر (eap->forceit)
    {
	// cursor line can be zero on startup
	اگر (!curwin->w_cursor.lnum)
	    curwin->w_cursor.lnum = 1;
	سیٹ_cursor_for_append_مقصد_سطر();
    }

    // Ignore the command when already in Insert mode.  Inserting an
    // expression register that invokes a function can do this.
    اگر (State & منوال_داخل_کرو)
	واپس;

    اگر (eap->cmdidx == CMD_startinsert)
	restart_edit = 'a';
    ورنہ اگر (eap->cmdidx == CMD_startreplace)
	restart_edit = 'R';
    ورنہ
	restart_edit = 'V';

    اگر (!eap->forceit)
    {
	اگر (eap->cmdidx == CMD_startinsert)
	    restart_edit = 'i';
	curwin->w_curswant = 0;	    // avoid MAXCOL
    }

    اگر (VIsual_فعال)
	showmode();
}

/*
 * ":stopinsert"
 */
    ساکن خالی
ex_stopinsert(exarg_T *eap غیر_مستعمل)
{
    restart_edit = 0;
    stop_داخل_کرو_منوال = TRUE;
#اگر defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // when called from remote_expr in insert mode, make sure insert mode is
    // ended by adding K_NOP to the typeahead buffer
    اگر (vgetc_busy)
	ins_char_typebuf(K_NOP, 0);
#اگر_ختم
    clearmode();
}

/*
 * Execute normal mode command "cmd".
 * "remap" can be REMAP_NONE or REMAP_YES.
 */
    خالی
exec_معمول_cmd(char_u *cmd, صحیح remap, صحیح silent)
{
    // Stuff the argument into the typeahead buffer.
    ins_typebuf(cmd, remap, 0, TRUE, silent);
    exec_معمول(FALSE, FALSE, FALSE);
}

/*
 * Execute normal_cmd() until there is no typeahead left.
 * When "use_vpeekc" is TRUE use vpeekc() to check for available chars.
 */
    خالی
exec_معمول(صحیح was_typed, صحیح use_vpeekc, صحیح may_use_terminal_loop غیر_مستعمل)
{
    oparg_T	oa;
    صحیح		c;

    // When calling vpeekc() from feedkeys() it will return Ctrl_C when there
    // is nothing to get, so also check for Ctrl_C.
    clear_oparg(&oa);
    اختتام_op = FALSE;
    جب ((!stuff_خالی_خانے()
		|| ((was_typed || !typebuf_typed()) && typebuf.tb_len > 0)
		|| (use_vpeekc && (c = vpeekc()) != NUL && c != Ctrl_C))
	    && !got_int)
    {
	update_topline_cursor();
#اگر_تعریف_شدہ FEAT_TERMINAL
	اگر (may_use_terminal_loop && رکن_use_loop()
		&& oa.op_type == OP_NOP && oa.regname == NUL
		&& !VIsual_فعال)
	{
	    // If terminal_loop() returns OK we got a key that is handled
	    // in Normal model.  With FAIL we first need to position the
	    // cursor and the screen needs to be redrawn.
	    اگر (terminal_loop(TRUE) == OK)
		معمول_cmd(&oa, TRUE);
	}
	ورنہ
#اگر_ختم
	    // execute a Normal mode cmd
	    معمول_cmd(&oa, TRUE);
    }
}

#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
    ساکن خالی
ex_checkpath(exarg_T *eap)
{
    ڈھونڈو_نمونہ_اندر_راستہ(تہی, 0, 0, FALSE, FALSE, CHECK_راستہ, 1L,
	    eap->forceit ? ACTION_SHOW_ALL : ACTION_SHOW,
	    (linenr_T)1, (linenr_T)MAXLNUM, eap->forceit, FALSE);
}

# اگر defined(FEAT_QUICKFIX)
/*
 * ":psearch"
 */
    ساکن خالی
ex_psearch(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;
    ex_findpat(eap);
    g_do_tagpreview = 0;
}
# endif

    ساکن خالی
ex_findpat(exarg_T *eap)
{
    صحیح		پورا = TRUE;
    طویل	n;
    char_u	*p;
    صحیح		action;

    انتخاب (cmdnames[eap->cmdidx].cmd_name[2])
    {
	حالت 'e':	// ":psearch", ":isearch" and ":dsearch"
		اگر (cmdnames[eap->cmdidx].cmd_name[0] == 'p')
		    action = ACTION_GOTO;
		ورنہ
		    action = ACTION_SHOW;
		توڑو;
	حالت 'i':	// ":ilist" and ":dlist"
		action = ACTION_SHOW_ALL;
		توڑو;
	حالت 'u':	// ":ijump" and ":djump"
		action = ACTION_GOTO;
		توڑو;
	طےشدہ:	// ":isplit" and ":dsplit"
		action = ACTION_بانٹو;
		توڑو;
    }

    n = 1;
    اگر (vim_isdigit(*eap->arg))	// get count
    {
	n = getdigits(&eap->arg);
	eap->arg = skipwhite(eap->arg);
    }
    اگر (*eap->arg == '/')   // Match regexp, not just whole words
    {
	پورا = FALSE;
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	اگر (*p)
	{
	    *p++ = NUL;
	    p = skipwhite(p);

	    // Check for trailing illegal characters
	    اگر (!سرے_excmd2(eap->arg, p))
		eap->errmsg = ex_errmsg(e_trailing_characters_str, p);
	    ورنہ
		سیٹ_nextcmd(eap, p);
	}
    }
    اگر (!eap->skip)
	ڈھونڈو_نمونہ_اندر_راستہ(eap->arg, 0, (صحیح)STRLEN(eap->arg),
		پورا, !eap->forceit,
		*eap->cmd == 'd' ? ڈھونڈو_DEFINE : ڈھونڈو_ANY, n, action,
		eap->line1, eap->line2, eap->forceit, FALSE);
}
#اگر_ختم


#اگر_تعریف_شدہ FEAT_QUICKFIX
/*
 * ":ptag", ":ptselect", ":ptjump", ":ptnext", etc.
 */
    ساکن خالی
ex_ptag(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;  // will be reset to 0 in ex_tag_cmd()
    ex_لیبل_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
}

/*
 * ":pedit"
 */
    ساکن خالی
ex_pedit(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_کھڑکی();

    // Edit the file.
    do_exedit(eap, تہی);

    عقب_مقصد_current_کھڑکی(curwin_save);
}

/*
 * ":pbuffer"
 */
    ساکن خالی
ex_pbuffer(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_کھڑکی();

    // Go to the buffer.
    do_exbuffer(eap);

    عقب_مقصد_current_کھڑکی(curwin_save);
}

    ساکن خالی
prepare_preview_کھڑکی(خالی)
{
    اگر (ERROR_IF_ANY_POPUP_کھڑکی)
	واپس;

    // Open the preview window or popup and make it the current window.
    g_do_tagpreview = p_pvh;
    prepare_tagpreview(TRUE, TRUE, FALSE);
}

    ساکن خالی
عقب_مقصد_current_کھڑکی(win_T *curwin_save)
{
    اگر (curwin != curwin_save && win_valid(curwin_save))
    {
	// Return cursor to where we were
	validate_cursor();
	redraw_later(UPD_VALID);
	win_enter(curwin_save, TRUE);
    }
# ifdef FEAT_PROP_POPUP
    ورنہ اگر (WIN_IS_POPUP(curwin))
    {
	// can't keep focus in popup window
	win_enter(firstwin, TRUE);
    }
# endif
    g_do_tagpreview = 0;
}
#اگر_ختم

/*
 * ":stag", ":stselect" and ":stjump".
 */
    ساکن خالی
ex_stag(exarg_T *eap)
{
    postponed_بانٹو = -1;
    postponed_بانٹو_flags = cmdmod.cmod_بانٹو;
    postponed_بانٹو_tab = cmdmod.cmod_tab;
    ex_لیبل_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
    postponed_بانٹو_flags = 0;
    postponed_بانٹو_tab = 0;
}

/*
 * ":tag", ":tselect", ":tjump", ":tnext", etc.
 */
    ساکن خالی
ex_لیبل(exarg_T *eap)
{
    ex_لیبل_cmd(eap, cmdnames[eap->cmdidx].cmd_name);
}

    ساکن خالی
ex_لیبل_cmd(exarg_T *eap, char_u *name)
{
    صحیح		cmd;

    انتخاب (name[1])
    {
	حالت 'j': cmd = DT_JUMP;	// ":tjump"
		  توڑو;
	حالت 's': cmd = DT_SELECT;	// ":tselect"
		  توڑو;
	حالت 'p': cmd = DT_پچھلا;	// ":tprevious"
		  توڑو;
	حالت 'N': cmd = DT_پچھلا;	// ":tNext"
		  توڑو;
	حالت 'n': cmd = DT_اگلا;	// ":tnext"
		  توڑو;
	حالت 'o': cmd = DT_نکالو;		// ":pop"
		  توڑو;
	حالت 'f':			// ":tfirst"
	حالت 'r': cmd = DT_پہلا;	// ":trewind"
		  توڑو;
	حالت 'l': cmd = DT_آخری;	// ":tlast"
		  توڑو;
	طےشدہ:			// ":tag"
#اگر_تعریف_شدہ FEAT_CSCOPE
		  اگر (p_cst && *eap->arg != NUL)
		  {
		      ex_cstag(eap);
		      واپس;
		  }
#اگر_ختم
		  cmd = DT_لیبل;
		  توڑو;
    }

    اگر (name[0] == 'l')
    {
#اگر_غیر_تعریف_شدہ FEAT_QUICKFIX
	ex_ni(eap);
	واپس;
#ورنہ
	cmd = DT_LTAG;
#اگر_ختم
    }

    do_لیبل(eap->arg, cmd, eap->addr_گنتی > 0 ? (صحیح)eap->line2 : 1,
							  eap->forceit, TRUE);
}

شمار {
    SPEC_PERC = 0,
    SPEC_HASH,
    SPEC_CWORD,	    // cursor word
    SPEC_CCWORD,    // cursor WORD
    SPEC_CEXPR,	    // expr under cursor
    SPEC_CFILE,	    // cursor path name
    SPEC_SFILE,	    // ":so" file name
    SPEC_SLNUM,	    // ":so" file line number
    SPEC_ڈھیر,	    // call stack
    SPEC_SCRIPT,    // script file name
    SPEC_AFILE,	    // autocommand file name
    SPEC_ABUF,	    // autocommand buffer number
    SPEC_AMATCH,    // autocommand match name
    SPEC_SFLNUM,    // script file line number
    SPEC_SID	    // script ID: <SNR>123_
#اگر_تعریف_شدہ FEAT_CLIENTSERVER
    , SPEC_CLIENT
#اگر_ختم
};

/*
 * Check "str" for starting with a special cmdline variable.
 * If found return one of the SPEC_ values and set "*usedlen" to the length of
 * the variable.  Otherwise return -1 and "*usedlen" is unchanged.
 */
    صحیح
ڈھونڈو_cmdline_var(char_u *src, size_t *usedlen)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    ساکن keyvalue_T spec_str_tab[] = {
	KEYVALUE_اندراج(SPEC_SID, "SID>"),	    // script ID: <SNR>123_
	KEYVALUE_اندراج(SPEC_ABUF, "abuf>"),	    // autocommand buffer number
	KEYVALUE_اندراج(SPEC_AFILE, "afile>"),	    // autocommand file name
	KEYVALUE_اندراج(SPEC_AMATCH, "amatch>"),	    // autocommand match name
	KEYVALUE_اندراج(SPEC_CCWORD, "cWORD>"),	    // cursor WORD
	KEYVALUE_اندراج(SPEC_CEXPR, "cexpr>"),	    // expr under cursor
	KEYVALUE_اندراج(SPEC_CFILE, "cfile>"),	    // cursor path name
#اگر_تعریف_شدہ FEAT_CLIENTSERVER
	KEYVALUE_اندراج(SPEC_CLIENT, "client>"),
#اگر_ختم
	KEYVALUE_اندراج(SPEC_CWORD, "cword>"),	    // cursor word
	KEYVALUE_اندراج(SPEC_SCRIPT, "script>"),	    // script file name
	KEYVALUE_اندراج(SPEC_SFILE, "sfile>"),	    // ":so" file name
	KEYVALUE_اندراج(SPEC_SFLNUM, "sflnum>"),	    // script file line number
	KEYVALUE_اندراج(SPEC_SLNUM, "slnum>"),	    // ":so" file line number
	KEYVALUE_اندراج(SPEC_ڈھیر, "stack>")	    // call stack
    };
    keyvalue_T ہدف;
    keyvalue_T *اندراج_2;

    انتخاب (*src)
    {
    حالت '%':
	*usedlen = 1;
	واپس SPEC_PERC;

    حالت '#':
	*usedlen = 1;
	واپس SPEC_HASH;

    حالت '<':
	ہدف.کلید = 0;
	ہدف.قدر.string = src + 1;	    // skip over '<'
	ہدف.قدر.لمبائی = 0;	    // not used, see cmp_keyvalue_value_n()

	اندراج_2 = (keyvalue_T *)bsearch(&ہدف, &spec_str_tab,
		ARRAY_لمبائی(spec_str_tab), حجم(spec_str_tab[0]),
		cmp_keyvalue_قدر_n);
	اگر (اندراج_2 == تہی)
	    واپس -1;

	*usedlen = اندراج_2->قدر.لمبائی + 1;
	واپس اندراج_2->کلید;

    طےشدہ:
	توڑو;
    }

    واپس -1;
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
    حرف	**errormsg,	// pointer to error message
    صحیح		*escaped,	// return value has escaped white space (can
				// be NULL)
    صحیح		خالی_خانے_is_error)	// empty result is considered an error
{
    صحیح		i;
    char_u	*s;
    char_u	*نتیجہ = (char_u *)"";
    char_u	*resultbuf = تہی;
    size_t	resultlen;
    buf_T	*buf;
    صحیح		valid = VALID_سر + VALID_راستہ;    // assume valid result
    صحیح		spec_idx;
    صحیح		tilde_file = FALSE;
    صحیح		skip_mod = FALSE;
    char_u	strbuf[30];

    *errormsg = تہی;
    اگر (escaped != تہی)
	*escaped = FALSE;

    /*
     * Check if there is something to do.
     */
    spec_idx = ڈھونڈو_cmdline_var(src, usedlen);
    اگر (spec_idx < 0)	// no match
    {
	*usedlen = 1;
	واپس تہی;
    }

    /*
     * Skip when preceded with a backslash "\%" and "\#".
     * Note: In "\\%" the % is also not recognized!
     */
    اگر (src > srcstart && src[-1] == '\\')
    {
	*usedlen = 0;
	STRMOVE(src - 1, src);	// remove backslash
	واپس تہی;
    }

    /*
     * word or WORD under cursor
     */
    اگر (spec_idx == SPEC_CWORD || spec_idx == SPEC_CCWORD
						     || spec_idx == SPEC_CEXPR)
    {
	resultlen = ڈھونڈو_ident_under_cursor(&نتیجہ,
		spec_idx == SPEC_CWORD ? (ڈھونڈو_IDENT | ڈھونڈو_STRING)
	      : spec_idx == SPEC_CEXPR ? (ڈھونڈو_IDENT | ڈھونڈو_STRING | ڈھونڈو_EVAL)
	      : ڈھونڈو_STRING);
	اگر (resultlen == 0)
	{
	    *errormsg = "";
	    واپس تہی;
	}
    }

    /*
     * '#': Alternate file name
     * '%': Current file name
     *	    File name under the cursor
     *	    File name for autocommand
     *	and following modifiers
     */
    ورنہ
    {
	size_t off = 0;

	انتخاب (spec_idx)
	{
	حالت SPEC_PERC:
#اگر_تعریف_شدہ FEAT_EVAL
		اگر (!اندر_vim9script() || src[1] != '%')
#اگر_ختم
		{
		    // '%': current file
		    اگر (curbuf->b_fname == تہی)
		    {
			نتیجہ = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    ورنہ
		    {
			نتیجہ = curbuf->b_fname;
			tilde_file = STRCMP(نتیجہ, "~") == 0;
		    }
		    توڑو;
		}
#اگر_تعریف_شدہ FEAT_EVAL
		// "%%" alternate file
		off = 1;
#اگر_ختم
		// FALLTHROUGH
	حالت SPEC_HASH:		// '#' or "#99": alternate file
		اگر (off == 0 ? src[1] == '#' : src[2] == '%')
		{
		    // "##" or "%%%": the argument list
		    نتیجہ = arg_all();
		    resultbuf = نتیجہ;
		    *usedlen = off + 2;
		    اگر (escaped != تہی)
			*escaped = TRUE;
		    skip_mod = TRUE;
		    توڑو;
		}
		s = src + off + 1;
		اگر (*s == '<')		// "#<99" uses v:oldfiles
		    ++s;
		i = (صحیح)getdigits(&s);
		اگر (s == src + off + 2 && src[off + 1] == '-')
		    // just a minus sign, don't skip over it
		    s--;
		*usedlen = (صحیح)(s - src); // length of what we expand

		اگر (src[off + 1] == '<' && i != 0)
		{
		    اگر (*usedlen < off + 2)
		    {
			// Should we give an error message for #<text?
			*usedlen = off + 1;
			واپس تہی;
		    }
#اگر_تعریف_شدہ FEAT_EVAL
		    نتیجہ = list_ڈھونڈو_str(get_vim_var_list(VV_OLDFILES),
								     (طویل)i);
		    اگر (نتیجہ == تہی)
		    {
			*errormsg = "";
			واپس تہی;
		    }
#ورنہ
		    *errormsg = _(e_hashsmall_is_not_دستیاب_without_the_eval_feature);
		    واپس تہی;
#اگر_ختم
		}
		ورنہ
		{
		    اگر (i == 0 && src[off + 1] == '<' && *usedlen > off + 1)
			*usedlen = off + 1;
		    buf = buflist_findnr(i);
		    اگر (buf == تہی)
		    {
			*errormsg = _(e_no_alternate_file_name_مقصد_substitute_for_hash);
			واپس تہی;
		    }
		    اگر (lnump != تہی)
			*lnump = ECMD_آخری;
		    اگر (buf->b_fname == تہی)
		    {
			نتیجہ = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    ورنہ
		    {
			نتیجہ = buf->b_fname;
			tilde_file = STRCMP(نتیجہ, "~") == 0;
		    }
		}
		توڑو;

	حالت SPEC_CFILE:	// file name under cursor
		نتیجہ = file_name_at_cursor(FNAME_MESS|FNAME_HYP, 1L, تہی);
		اگر (نتیجہ == تہی)
		{
		    *errormsg = "";
		    واپس تہی;
		}
		resultbuf = نتیجہ;	    // remember allocated string
		توڑو;

	حالت SPEC_AFILE:	// file name for autocommand
		نتیجہ = autocmd_fname;
		اگر (نتیجہ != تہی && !autocmd_fname_بھرا)
		{
		    // Still need to turn the fname into a full path.  It is
		    // postponed to avoid a delay when <afile> is not used.
		    autocmd_fname_بھرا = TRUE;
		    نتیجہ = بھرا_Name_save(autocmd_fname, FALSE);
		    vim_free(autocmd_fname);
		    autocmd_fname = نتیجہ;
		}
		اگر (نتیجہ == تہی)
		{
		    *errormsg = _(e_no_autocommand_file_name_مقصد_substitute_for_afile);
		    واپس تہی;
		}
		نتیجہ = shorten_fname1(نتیجہ);
		توڑو;

	حالت SPEC_ABUF:		// buffer number for autocommand
		اگر (autocmd_bufnr <= 0)
		{
		    *errormsg = _(e_no_autocommand_بفر_نمبر_مقصد_substitute_for_abuf);
		    واپس تہی;
		}
		sprintf((حرف *)strbuf, "%d", autocmd_bufnr);
		نتیجہ = strbuf;
		توڑو;

	حالت SPEC_AMATCH:	// match name for autocommand
		نتیجہ = autocmd_مطابقت;
		اگر (نتیجہ == تہی)
		{
		    *errormsg = _(e_no_autocommand_مطابقت_name_مقصد_substitute_for_amatch);
		    واپس تہی;
		}
		توڑو;

	حالت SPEC_SFILE:	// file name for ":so" command
		نتیجہ = estack_sfile(ESTACK_SFILE);
		اگر (نتیجہ == تہی)
		{
		    *errormsg = _(e_no_ماخذ_file_name_مقصد_substitute_for_sfile);
		    واپس تہی;
		}
		resultbuf = نتیجہ;	    // remember allocated string
		توڑو;
	حالت SPEC_ڈھیر:	// call stack
		نتیجہ = estack_sfile(ESTACK_ڈھیر);
		اگر (نتیجہ == تہی)
		{
		    *errormsg = _(e_no_call_ڈھیر_مقصد_substitute_for_ڈھیر);
		    واپس تہی;
		}
		resultbuf = نتیجہ;	    // remember allocated string
		توڑو;
	حالت SPEC_SCRIPT:	// script file name
		نتیجہ = estack_sfile(ESTACK_SCRIPT);
		اگر (نتیجہ == تہی)
		{
		    *errormsg = _(e_no_script_file_name_مقصد_substitute_for_script);
		    واپس تہی;
		}
		resultbuf = نتیجہ;	    // remember allocated string
		توڑو;

	حالت SPEC_SLNUM:	// line in file for ":so" command
		اگر (SOURCING_NAME == تہی || SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_سطر_نمبر_مقصد_use_for_slnum);
		    واپس تہی;
		}
		sprintf((حرف *)strbuf, "%ld", SOURCING_LNUM);
		نتیجہ = strbuf;
		توڑو;

#اگر_تعریف_شدہ FEAT_EVAL
	حالت SPEC_SFLNUM:	// line in script file
		اگر (current_sctx.sc_lnum + SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_سطر_نمبر_مقصد_use_for_sflnum);
		    واپس تہی;
		}
		sprintf((حرف *)strbuf, "%ld",
				 (طویل)(current_sctx.sc_lnum + SOURCING_LNUM));
		نتیجہ = strbuf;
		توڑو;

	حالت SPEC_SID:
		اگر (current_sctx.sc_sid <= 0)
		{
		    *errormsg = _(e_using_sid_not_اندر_script_context);
		    واپس تہی;
		}
		sprintf((حرف *)strbuf, "<SNR>%d_", current_sctx.sc_sid);
		نتیجہ = strbuf;
		توڑو;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_CLIENTSERVER
	حالت SPEC_CLIENT:	// Source of last submitted input
# ifdef MSWIN
		sprintf((حرف *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)client_کھڑکی);
		نتیجہ = strbuf;
# ورنہ
#  ifdef FEAT_SOCKETSERVER
		اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_SOCKET)
		{
		    اگر (client_socket == تہی)
			نتیجہ = (char_u *)"";
		    ورنہ
			نتیجہ = client_socket;
		}
#  endif
#  ifdef FEAT_X11
		اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_X11)
		{
		    sprintf((حرف *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)client_کھڑکی);
		    نتیجہ = strbuf;
		}
#  endif
# endif
		توڑو;
#اگر_ختم
	}

	resultlen = STRLEN(نتیجہ);	// length of new string
	اگر (src[*usedlen] == '<')	// remove the file name extension
	{
	    ++*usedlen;
	    اگر ((s = vim_strrchr(نتیجہ, '.')) != تہی && s >= gettail(نتیجہ))
		resultlen = s - نتیجہ;
	}
	ورنہ اگر (!skip_mod)
	{
	    valid |= modify_fname(src, tilde_file, usedlen, &نتیجہ, &resultbuf,
								  &resultlen);
	    اگر (نتیجہ == تہی)
	    {
		*errormsg = "";
		واپس تہی;
	    }
	}
    }

    اگر (resultlen == 0 || valid != VALID_سر + VALID_راستہ)
    {
	اگر (خالی_خانے_is_error)
	{
	    اگر (valid != VALID_سر + VALID_راستہ)
		*errormsg = _(e_خالی_خانے_file_name_for_percent_or_hash_only_چلتا_with_ph);
	    ورنہ
		*errormsg = _(e_evaluates_مقصد_an_خالی_خانے_string);
	}
	نتیجہ = تہی;
    }
    ورنہ
	نتیجہ = vim_strnsave(نتیجہ, resultlen);
    vim_free(resultbuf);
    واپس نتیجہ;
}

/*
 * Expand the <sfile> string in "arg".
 *
 * Returns an allocated string, or NULL for any error.
 */
    char_u *
expand_sfile(char_u *arg)
{
    حرف	*errormsg;
    char_u	*نتیجہ;
    size_t	resultlen;
    char_u	*newres;
    size_t	len;
    char_u	*repl;
    size_t	repllen;
    size_t	srclen;
    char_u	*p;

    resultlen = STRLEN(arg);
    نتیجہ = vim_strnsave(arg, resultlen);
    اگر (نتیجہ == تہی)
	واپس تہی;

    برائے (p = نتیجہ; *p; )
    {
	اگر (STRNCMP(p, "<sfile>", 7) != 0)
	    ++p;
	ورنہ
	{
	    // replace "<sfile>" with the sourced file name, and do ":" stuff
	    repl = eval_vars(p, نتیجہ, &srclen, تہی, &errormsg, تہی, TRUE);
	    اگر (errormsg != تہی)
	    {
		اگر (*errormsg)
		    emsg(errormsg);
		vim_free(نتیجہ);
		واپس تہی;
	    }
	    اگر (repl == تہی)		// no match (cannot happen)
	    {
		p += srclen;
		جاری;
	    }
	    repllen = STRLEN(repl);
	    resultlen += repllen - srclen;
	    newres = alloc(resultlen + 1);
	    اگر (newres == تہی)
	    {
		vim_free(repl);
		vim_free(نتیجہ);
		واپس تہی;
	    }
	    len = p - نتیجہ;
	    mch_memmove(newres, نتیجہ, len);
	    STRCPY(newres + len, repl);
	    len += repllen;
	    STRCPY(newres + len, p + srclen);
	    vim_free(repl);
	    vim_free(نتیجہ);
	    نتیجہ = newres;
	    p = newres + len;		// continue after the match
	}
    }

    واپس نتیجہ;
}

#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
/*
 * Make a dialog message in "buff[DIALOG_MSG_SIZE]".
 * "format" must contain "%s".
 */
    خالی
dialog_پیغام(char_u *buff, حرف *format, char_u *fname)
{
    اگر (fname == تہی)
	fname = (char_u *)_("Untitled");
    vim_snprintf((حرف *)buff, DIALOG_پیغام_سائز, format, fname);
}
#اگر_ختم

/*
 * ":behave {mswin,xterm}"
 */
    ساکن خالی
ex_behave(exarg_T *eap)
{
    اگر (STRCMP(eap->arg, "mswin") == 0)
    {
	سیٹ_option_قدر_give_err((char_u *)"selection",
						 0L, (char_u *)"exclusive", 0);
	سیٹ_option_قدر_give_err((char_u *)"selectmode",
						 0L, (char_u *)"mouse,key", 0);
	سیٹ_option_قدر_give_err((char_u *)"mousemodel",
						     0L, (char_u *)"popup", 0);
	سیٹ_option_قدر_give_err((char_u *)"keymodel",
					  0L, (char_u *)"startsel,stopsel", 0);
    }
    ورنہ اگر (STRCMP(eap->arg, "xterm") == 0)
    {
	سیٹ_option_قدر_give_err((char_u *)"selection",
						 0L, (char_u *)"inclusive", 0);
	سیٹ_option_قدر_give_err((char_u *)"selectmode", 0L, (char_u *)"", 0);
	سیٹ_option_قدر_give_err((char_u *)"mousemodel",
						    0L, (char_u *)"extend", 0);
	سیٹ_option_قدر_give_err((char_u *)"keymodel", 0L, (char_u *)"", 0);
    }
    ورنہ
	semsg(_(e_invalid_دلیل_str), eap->arg);
}

ساکن صحیح filetype_detect = FALSE;
ساکن صحیح filetype_plugin = FALSE;
ساکن صحیح filetype_indent = FALSE;

/*
 * ":filetype [plugin] [indent] {on,off,detect}"
 * on: Load the filetype.vim file to install autocommands for file types.
 * off: Load the ftoff.vim file to remove all autocommands for file types.
 * plugin on: load filetype.vim and ftplugin.vim
 * plugin off: load ftplugof.vim
 * indent on: load filetype.vim and indent.vim
 * indent off: load indoff.vim
 */
    ساکن خالی
ex_filetype(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    صحیح		plugin = FALSE;
    صحیح		indent = FALSE;

    اگر (*eap->arg == NUL)
    {
	// Print current status.
	smsg("filetype detection:%s  plugin:%s  indent:%s",
		filetype_detect ? "ON" : "OFF",
		filetype_plugin ? (filetype_detect ? "ON" : "(on)") : "OFF",
		filetype_indent ? (filetype_detect ? "ON" : "(on)") : "OFF");
	واپس;
    }

    // Accept "plugin" and "indent" in any order.
    برائے (;;)
    {
	اگر (STRNCMP(arg, "plugin", 6) == 0)
	{
	    plugin = TRUE;
	    arg = skipwhite(arg + 6);
	    جاری;
	}
	اگر (STRNCMP(arg, "indent", 6) == 0)
	{
	    indent = TRUE;
	    arg = skipwhite(arg + 6);
	    جاری;
	}
	توڑو;
    }
    اگر (STRCMP(arg, "on") == 0 || STRCMP(arg, "detect") == 0)
    {
	اگر (*arg == 'o' || !filetype_detect)
	{
	    ماخذ_runtime((char_u *)FILETYPE_FILE, DIP_ALL);
	    filetype_detect = TRUE;
	    اگر (plugin)
	    {
		ماخذ_runtime((char_u *)FTPLUGIN_FILE, DIP_ALL);
		filetype_plugin = TRUE;
	    }
	    اگر (indent)
	    {
		ماخذ_runtime((char_u *)INDENT_FILE, DIP_ALL);
		filetype_indent = TRUE;
	    }
	}
	اگر (*arg == 'd')
	{
	    (خالی)do_doautocmd((char_u *)"filetypedetect BufRead", TRUE, تہی);
	    do_modelines(0);
	}
    }
    ورنہ اگر (STRCMP(arg, "off") == 0)
    {
	اگر (plugin || indent)
	{
	    اگر (plugin)
	    {
		ماخذ_runtime((char_u *)FTPLUGOF_FILE, DIP_ALL);
		filetype_plugin = FALSE;
	    }
	    اگر (indent)
	    {
		ماخذ_runtime((char_u *)INDOFF_FILE, DIP_ALL);
		filetype_indent = FALSE;
	    }
	}
	ورنہ
	{
	    ماخذ_runtime((char_u *)FTOFF_FILE, DIP_ALL);
	    filetype_detect = FALSE;
	}
    }
    ورنہ
	semsg(_(e_invalid_دلیل_str), arg);
}

/*
 * ":setfiletype [FALLBACK] {name}"
 */
    ساکن خالی
ex_setfiletype(exarg_T *eap)
{
    اگر (curbuf->b_did_filetype)
	واپس;

    char_u *arg = eap->arg;
    اگر (STRNCMP(arg, "FALLBACK ", 9) == 0)
	arg += 9;

    سیٹ_option_قدر_give_err((char_u *)"filetype", 0L, arg, OPT_LOCAL);
    اگر (arg != eap->arg)
	curbuf->b_did_filetype = false;
}

    ساکن خالی
ex_digraphs(exarg_T *eap غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_DIGRAPHS
    اگر (*eap->arg != NUL)
	putdigraph(eap->arg);
    ورنہ
	listdigraphs(eap->forceit);
#ورنہ
    emsg(_(e_no_digraphs_نسخہ));
#اگر_ختم
}

#اگر defined(FEAT_تلاش_EXTRA)
    خالی
سیٹ_no_hlsearch(صحیح flag)
{
    no_hlsearch = flag;
# ifdef FEAT_EVAL
    سیٹ_vim_var_nr(VV_HLSEARCH, !no_hlsearch && p_hls);
# endif
}

/*
 * ":nohlsearch"
 */
    ساکن خالی
ex_nohlsearch(exarg_T *eap غیر_مستعمل)
{
    سیٹ_no_hlsearch(TRUE);
    redraw_all_later(UPD_SOME_VALID);
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_CRYPT
/*
 * ":X": Get crypt key
 */
    ساکن خالی
ex_X(exarg_T *eap غیر_مستعمل)
{
    crypt_check_current_طریقہ();
    (خالی)crypt_get_کلید(TRUE, TRUE);
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_FOLDING
    ساکن خالی
ex_fold(exarg_T *eap)
{
    اگر (foldManualAllowed(TRUE))
	foldCreate(eap->line1, eap->line2);
}

    ساکن خالی
ex_foldopen(exarg_T *eap)
{
    opFoldRange(eap->line1, eap->line2, eap->cmdidx == CMD_foldopen,
							 eap->forceit, FALSE);
}

    ساکن خالی
ex_folddo(exarg_T *eap)
{
    linenr_T	lnum;

# ifdef FEAT_CLIPBOARD
    آغاز_global_changes();
# endif
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
# endif

    // First set the marks for all lines closed/open.
    برائے (lnum = eap->line1; lnum <= eap->line2; ++lnum)
	اگر (hasFolding(lnum, تہی, تہی) == (eap->cmdidx == CMD_folddoclosed))
	    ml_setmarked(lnum);

    // Execute the command on the marked lines.
    global_exe(eap->arg);
    ml_clearmarked();	   // clear rest of the marks
# ifdef FEAT_CLIPBOARD
    انتہا_global_changes();
# endif
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif
}
#اگر_ختم

#اگر defined(FEAT_QUICKFIX)
/*
 * Returns TRUE if the supplied Ex cmdidx is for a location list command
 * instead of a quickfix command.
 */
    صحیح
is_loclist_cmd(صحیح cmdidx)
{
    اگر (cmdidx < 0 || cmdidx >= CMD_سائز)
	واپس FALSE;
    واپس cmdnames[cmdidx].cmd_name[0] == 'l';
}
#اگر_ختم

    صحیح
get_pressedreturn(خالی)
{
    واپس ex_pressedreturn;
}

    خالی
سیٹ_pressedreturn(صحیح val)
{
    ex_pressedreturn = val;
}
