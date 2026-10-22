/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * Icons used by the toolbar code.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_new.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_open.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_close.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_save.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_print.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_cut.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_copy.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_paste.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_find.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_find_next.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_find_prev.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_find_help.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_exit.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_undo.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_redo.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_help.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_macro.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_make.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_save_all.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_jump.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_ctags.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_load_session.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_save_session.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_new_session.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_blank.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_maximize.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_split.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_minimize.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_shell.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_replace.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_vsplit.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_maxwidth.xpm"
#ਅਨ੍ਤਰ੍ਭਾਵ "../pixmaps/tb_minwidth.xpm"

/*
 * Those are the pixmaps used for the default buttons.
 */
ਸਥਿਰ ਅੱਖਰ **(built_ਵਿੱਚ_pixmaps[]) =
{
    tb_new_xpm,
    tb_open_xpm,
    tb_save_xpm,
    tb_undo_xpm,
    tb_redo_xpm,
    tb_cut_xpm,
    tb_copy_xpm,
    tb_paste_xpm,
    tb_print_xpm,
    tb_help_xpm,
    tb_ਲੱਭੋ_xpm,
    tb_save_all_xpm,
    tb_save_session_xpm,
    tb_new_session_xpm,
    tb_ਲੱਦੋ_session_xpm,
    tb_macro_xpm,
    tb_replace_xpm,
    tb_close_xpm,
    tb_maximize_xpm,
    tb_minimize_xpm,
    tb_ਵੰਡੋ_xpm,
    tb_shell_xpm,
    tb_ਲੱਭੋ_prev_xpm,
    tb_ਲੱਭੋ_ਅਗਲਾ_xpm,
    tb_ਲੱਭੋ_help_xpm,
    tb_make_xpm,
    tb_jump_xpm,
    tb_ctags_xpm,
    tb_vsplit_xpm,
    tb_maxwidth_xpm,
    tb_minwidth_xpm,
    tb_exit_xpm
};

// Indices for named colors
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BACKGROUND	0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOREGROUND	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOTTOM_SHADOW	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਖਰ_SHADOW	3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HIGHLIGHT	4
