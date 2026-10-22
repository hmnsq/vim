/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

// Structure containing all the GUI information
gui_T gui;

#ਜੇ defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
# define USE_ਸੈੱਟ_GUIFONTWIDE
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_guifontwide(char_u *font_name);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ gui_check_pos(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ gui_reset_scroll_region(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ gui_outstr(char_u *, ਪੂਰਨ);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_GTK4
ਸਥਿਰ ਪੂਰਨ gui_screenchar(ਪੂਰਨ off, ਪੂਰਨ flags, guicolor_T fg, guicolor_T bg, ਪੂਰਨ ਪਿੱਠ);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ gui_outstr_nowrap(char_u *s, ਪੂਰਨ len, ਪੂਰਨ flags, guicolor_T fg, guicolor_T bg, ਪੂਰਨ ਪਿੱਠ);
ਸਥਿਰ ਖਾਲੀ gui_delete_ਸਤਰਾਂ(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਗਿਣਤੀ_2);
ਸਥਿਰ ਖਾਲੀ gui_ਪਾਓ_ਸਤਰਾਂ(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਗਿਣਤੀ_2);
ਸਥਿਰ ਪੂਰਨ gui_xy2colrow(ਪੂਰਨ x, ਪੂਰਨ y, ਪੂਰਨ *colp);
#ਜੇ defined(FEAT_GUI_TABLINE)
ਸਥਿਰ ਪੂਰਨ gui_has_tabline(ਖਾਲੀ);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ gui_do_scrollbar(win_T *wp, ਪੂਰਨ which, ਪੂਰਨ enable);
ਸਥਿਰ ਖਾਲੀ gui_update_horiz_scrollbar(ਪੂਰਨ);
ਸਥਿਰ ਖਾਲੀ gui_ਸੈੱਟ_fg_color(char_u *name);
ਸਥਿਰ ਖਾਲੀ gui_ਸੈੱਟ_bg_color(char_u *name);
ਸਥਿਰ ਖਾਲੀ init_gui_options(ਖਾਲੀ);
ਸਥਿਰ win_T *xy2win(ਪੂਰਨ x, ਪੂਰਨ y, mouse_ਲੱਭੋ_T popup);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GUI_MAY_FORK
ਸਥਿਰ ਖਾਲੀ gui_do_fork(ਖਾਲੀ);

ਸਥਿਰ ਪੂਰਨ gui_read_ਬੱਚਾ_pipe(ਪੂਰਨ fd);

// Return values for gui_read_child_pipe
ਗਿਣਤੀ {
    GUI_ਬੱਚਾ_IO_ERROR,
    GUI_ਬੱਚਾ_OK,
    GUI_ਬੱਚਾ_FAILED
};
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ gui_attempt_ਸ਼ੁਰੂ(ਖਾਲੀ);

ਸਥਿਰ ਪੂਰਨ can_update_cursor = TRUE; // can display the cursor
ਸਥਿਰ ਪੂਰਨ disable_flush = 0;	// If > 0, gui_mch_flush() is disabled.

/*
 * gui_start -- Called when user wants to start the GUI.
 *
 * Careful: This function can be called recursively when there is a ":gui"
 * command in the .gvimrc file.  Only the first call should fork, not the
 * recursive call.
 */
    ਖਾਲੀ
gui_ਸ਼ੁਰੂ(char_u *arg ਅਣਵਰਤਿਆ)
{
    char_u	*old_ਪਦ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GUI_MAY_FORK
    ਸਥਿਰ ਪੂਰਨ	recursive = 0;
#ਜੇ_ਅੰਤ
#ਜੇ defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
    ਅੱਖਰ	*ਸੁਨੇਹਾ = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

    old_ਪਦ = vim_strsave(T_NAME);

    settmode(TMODE_COOK);		// stop RAW mode
    ਜੇ (ਭਰਿਆ_screen)
	cursor_on();			// needed for ":gui" in .vimrc
    ਭਰਿਆ_screen = FALSE;

    // If GUI fails to start, then we will recover afterwards
    ਪਦ_disable_dec();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GUI_MAY_FORK
    ++recursive;
    /*
     * Quit the current process and continue in the child.
     * Makes "gvim file" disconnect from the shell it was started in.
     * Don't do this when Vim was started with "-f" or the 'f' flag is present
     * in 'guioptions'.
     * Don't do this when there is a running job, we can only get the status
     * of a child from the parent.
     */
    ਜੇ (gui.dofork && !vim_strchr(p_go, GO_FORG) && recursive <= 1
# ifdef FEAT_ਕੰਮ_CHANNEL
	    && !ਕੰਮ_any_running()
# endif
	    )
    {
	gui_do_fork();
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GUI_MAY_SPAWN
    ਜੇ (gui.dospawn
# ifdef EXPERIMENTAL_GUI_CMD
	    && gui.dofork
# endif
	    && !vim_strchr(p_go, GO_FORG)
	    && !any_Buf_Is_ਬਦਲਿਆ()
# ifdef FEAT_ਕੰਮ_CHANNEL
	    && !ਕੰਮ_any_running()
# endif
	    )
    {
# ifdef EXPERIMENTAL_GUI_CMD
	ਸੁਨੇਹਾ =
# endif
	    gui_mch_do_spawn(arg);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
	// If there is 'f' in 'guioptions' and specify -g argument,
	// gui_mch_init_check() was not called yet.
	ਜੇ (gui_mch_init_check() != OK)
	    getout_preserve_modified(1);
#ਜੇ_ਅੰਤ
	gui_attempt_ਸ਼ੁਰੂ();
    }

    ਜੇ (!gui.ਵਿੱਚ_use)			// failed to start GUI
    {
	// Back to old term settings
	//
	// FIXME: If we got here because a child process failed and flagged to
	// the parent to resume, and X11 is enabled, this will
	// hit an X11 I/O error and do a longjmp(), leaving recursive
	// permanently set to 1. This is probably not as big a problem as it
	// sounds, because gui_mch_init() in both gui_x11.c and gui_gtk_x11.c
	// return "OK" unconditionally, so it would be very difficult to
	// actually hit this case.
	termcapinit(old_ਪਦ);
	settmode(TMODE_RAW);		// restart RAW mode
	ਸੈੱਟ_title_defaults();		// set 'title' and 'icon' again
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	ਪਦ_ਸੈੱਟ_win_resize(true);
#ਜੇ_ਅੰਤ
#ਜੇ defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
	ਜੇ (ਸੁਨੇਹਾ != ਨਿਰਾ)
	    emsg(ਸੁਨੇਹਾ);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLIPMETHOD
	// Reset clipmethod to CLIPMETHOD_NONE
	ਚੁਣੋ_clipmethod();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE
	update_cell_ਮਾਪ();
	update_image_backend();
#ਜੇ_ਅੰਤ
    }

#ਜੇ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    // Enable fullscreen mode
    ਜੇ (vim_strchr(p_go, GO_FULLSCREEN) != ਨਿਰਾ)
       gui_mch_ਸੈੱਟ_fullscreen(TRUE);
#ਜੇ_ਅੰਤ

    vim_free(old_ਪਦ);

    // If the GUI started successfully, trigger the GUIEnter event, otherwise
    // the GUIFailed event.
    gui_mch_update();
    apply_autocmds(gui.ਵਿੱਚ_use ? EVENT_GUIENTER : EVENT_GUIFAILED,
						   ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GUI_MAY_FORK
    --recursive;
#ਜੇ_ਅੰਤ
}

/*
 * Set_termname() will call gui_init() to start the GUI.
 * Set the "starting" flag, to indicate that the GUI will start.
 *
 * We don't want to open the GUI shell until after we've read .gvimrc,
 * otherwise we don't know what font we will use, and hence we don't know
 * what size the shell should be.  So if there are errors in the .gvimrc
 * file, they will have to go to the terminal: Set full_screen to FALSE.
 * full_screen will be set to TRUE again by a successful termcapinit().
 */
    ਸਥਿਰ ਖਾਲੀ
gui_attempt_ਸ਼ੁਰੂ(ਖਾਲੀ)
{
    ਸਥਿਰ ਪੂਰਨ recursive = 0;

    ++recursive;
    gui.starting = true;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
    gui.event_ਸਮਾਂ = GDK_CURRENT_ਸਮਾਂ;
#ਜੇ_ਅੰਤ

    termcapinit((char_u *)"builtin_gui");
    gui.starting = recursive - 1;

#ਜੇ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
    ਜੇ (gui.ਵਿੱਚ_use)
    {
# ifdef FEAT_EVAL
	ਖਿੜਕੀ	x11_ਖਿੜਕੀ;
	Display	*x11_display;

	ਜੇ (gui_get_x11_windis(&x11_ਖਿੜਕੀ, &x11_display) == OK)
	    ਸੈੱਟ_vim_var_nr(VV_WINDOWID, (ਲੰਮਾ)x11_ਖਿੜਕੀ);
# endif
	// Display error messages in a dialog now.
	display_errors();
    }
#ਜੇ_ਅੰਤ
    --recursive;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GUI_MAY_FORK

// for waitpid()
# ਜੇ defined(HAVE_SYS_WAIT_H)
#  include <sys/wait.h>
# endif

/*
 * Create a new process, by forking. In the child, start the GUI, and in
 * the parent, exit.
 *
 * If something goes wrong, this will return with gui.in_use still set
 * to FALSE, in which case the caller should continue execution without
 * the GUI.
 *
 * If the child fails to start the GUI, then the child will exit and the
 * parent will return. If the child succeeds, then the parent will exit
 * and the child will return.
 */
    ਸਥਿਰ ਖਾਲੀ
gui_do_fork(ਖਾਲੀ)
{
    ਪੂਰਨ		pipefd[2];	// pipe between parent and child
    ਪੂਰਨ		pipe_error;
    ਪੂਰਨ		ਸਥਿਤੀ;
    ਪੂਰਨ		exit_ਸਥਿਤੀ;
    pid_t	pid = -1;

# ਜੇ defined(FEAT_RELTIME) && defined(PROF_NSEC)
    // a timer is not carried forward
    delete_timer();
# endif

    // Setup a pipe between the child and the parent, so that the parent
    // knows when the child has done the setsid() call and is allowed to
    // exit.
    pipe_error = (pipe(pipefd) < 0);
    pid = fork();
    ਜੇ (pid < 0)	    // Fork error
    {
	emsg(_(e_failed_ਮੰਜ਼ਿਲ_create_new_process_for_GUI));
	ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (pid > 0)	    //ਮਾਪਾ
    {
	// Give the child some time to do the setsid(), otherwise the
	// exit() may kill the child too (when starting gvim from inside a
	// gvim).
	ਜੇ (!pipe_error)
	{
	    // The read returns when the child closes the pipe (or when
	    // the child dies for some reason).
	    close(pipefd[1]);
	    ਸਥਿਤੀ = gui_read_ਬੱਚਾ_pipe(pipefd[0]);
	    ਜੇ (ਸਥਿਤੀ == GUI_ਬੱਚਾ_FAILED)
	    {
		// The child failed to start the GUI, so the caller must
		// continue. There may be more error information written
		// to stderr by the child.
		waitpid(pid, &exit_ਸਥਿਤੀ, 0);
		emsg(_(e_the_ਬੱਚਾ_process_failed_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ_GUI));
		ਵਾਪਸ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਸਥਿਤੀ == GUI_ਬੱਚਾ_IO_ERROR)
	    {
		pipe_error = TRUE;
	    }
	    // else GUI_CHILD_OK: parent exit
	}

	ਜੇ (pipe_error)
	    ui_delay(301L, TRUE);

	// When swapping screens we may need to go to the next line, e.g.,
	// after a hit-enter prompt and using ":gui".
	ਜੇ (newline_on_exit)
	    mch_errmsg("\r\n");

	/*
	 * The parent must skip the normal exit() processing, the child
	 * will do it.  For example, GTK messes up signals when exiting.
	 */
	_exit(0);
    }
    //ਬੱਚਾ

# ifdef FEAT_GUI_GTK
    // Call gtk_init_check() here after fork(). See gui_init_check().
    ਜੇ (gui_mch_init_check() != OK)
	getout_preserve_modified(1);
# endif

# ਜੇ defined(HAVE_SETSID) || defined(HAVE_SETPGID)
    /*
     * Change our process group.  On some systems/shells a CTRL-C in the
     * shell where Vim was started would otherwise kill gvim!
     */
#  ਜੇ defined(HAVE_SETSID)
    (ਖਾਲੀ)setsid();
#  ਨਹੀਂਤਾਂ
    (ਖਾਲੀ)setpgid(0, 0);
#  endif
# endif
    ਜੇ (!pipe_error)
	close(pipefd[0]);

# ਜੇ defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
    // Tell the session manager our new PID
    gui_mch_forked();
# endif

    // Try to start the GUI
    gui_attempt_ਸ਼ੁਰੂ();

    // Notify the parent
    ਜੇ (!pipe_error)
    {
	ਜੇ (gui.ਵਿੱਚ_use)
	    write_eintr(pipefd[1], "ok", 3);
	ਨਹੀਂਤਾਂ
	    write_eintr(pipefd[1], "fail", 5);
	close(pipefd[1]);
    }

    // If we failed to start the GUI, exit now.
    ਜੇ (!gui.ਵਿੱਚ_use)
	getout_preserve_modified(1);
}

/*
 * Read from a pipe assumed to be connected to the child process (this
 * function is called from the parent).
 * Return GUI_CHILD_OK if the child successfully started the GUI,
 * GUY_CHILD_FAILED if the child failed, or GUI_CHILD_IO_ERROR if there was
 * some other error.
 *
 * The file descriptor will be closed before the function returns.
 */
    ਸਥਿਰ ਪੂਰਨ
gui_read_ਬੱਚਾ_pipe(ਪੂਰਨ fd)
{
    ਲੰਮਾ	ਬਾਈਟਾਂ_read;
# define READ_ਬਫਰ_ਮਾਪ 10
    ਅੱਖਰ	ਬਫਰ[READ_ਬਫਰ_ਮਾਪ];

    ਬਾਈਟਾਂ_read = read_eintr(fd, ਬਫਰ, READ_ਬਫਰ_ਮਾਪ - 1);
# undef READ_ਬਫਰ_ਮਾਪ
    close(fd);
    ਜੇ (ਬਾਈਟਾਂ_read < 0)
	ਵਾਪਸ GUI_ਬੱਚਾ_IO_ERROR;
    ਬਫਰ[ਬਾਈਟਾਂ_read] = NUL;
    ਜੇ (STRCMP(ਬਫਰ, "ok") == 0)
	ਵਾਪਸ GUI_ਬੱਚਾ_OK;
    ਵਾਪਸ GUI_ਬੱਚਾ_FAILED;
}

#ਜੇ_ਅੰਤ // GUI_MAY_FORK

/*
 * Call this when vim starts up, whether or not the GUI is started
 */
    ਖਾਲੀ
gui_prepare(ਪੂਰਨ *argc, ਅੱਖਰ **argv)
{
    gui.ਵਿੱਚ_use = false;		    // No GUI yet (maybe later)
    gui.starting = false;	    // No GUI yet (maybe later)
    gui_mch_prepare(argc, argv);
}

/*
 * Try initializing the GUI and check if it can be started.
 * Used from main() to check early if "vim -g" can start the GUI.
 * Used from gui_init() to prepare for starting the GUI.
 * Returns FAIL or OK.
 */
    ਪੂਰਨ
gui_init_check(ਖਾਲੀ)
{
    ਸਥਿਰ ਪੂਰਨ ਨਤੀਜਾ = MAYBE;

    ਜੇ (ਨਤੀਜਾ != MAYBE)
    {
	ਜੇ (ਨਤੀਜਾ == FAIL)
	    emsg(_(e_cannot_ਸ਼ੁਰੂ_the_GUI));
	ਵਾਪਸ ਨਤੀਜਾ;
    }

    gui.shell_created = false;
    gui.dying = false;
    gui.ਵਿੱਚ_focus = true;		// so the guicursor setting works
    gui.dragged_sb = SBAR_NONE;
    gui.dragged_wp = ਨਿਰਾ;
    gui.ਸੂਚਕ_hidden = false;
    gui.ਖਾਨਾ = 0;
    gui.ਪੰਕਤੀ = 0;
    gui.ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ = Columns;
    gui.ਅੰਕ_ਪੰਕਤੀਆਂ = ਪੰਕਤੀਆਂ;

    gui.cursor_is_valid = false;
    gui.scroll_region_ਸਿਖਰ = 0;
    gui.scroll_region_bot = ਪੰਕਤੀਆਂ - 1;
    gui.scroll_region_ਖੱਬਾ = 0;
    gui.scroll_region_ਸੱਜਾ = Columns - 1;
    gui.highlight_ਨਕਾਬ = HL_ਸਾਧਾਰਨ;
    gui.char_ਚੌੜਾਈ = 1;
    gui.char_ਉਚਾਈ = 1;
    gui.char_ascent = 0;
    gui.border_ਚੌੜਾਈ = 0;

    gui.norm_font = NOFONT;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
    gui.bold_font = NOFONT;
    gui.ital_font = NOFONT;
    gui.boldital_font = NOFONT;
# ifdef FEAT_XFONTSET
    gui.fontset = NOFONTSET;
# endif
#ਜੇ_ਅੰਤ
    gui.wide_font = NOFONT;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
    gui.wide_bold_font = NOFONT;
    gui.wide_ital_font = NOFONT;
    gui.wide_boldital_font = NOFONT;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
# ifndef FEAT_GUI_GTK
#  ifdef FONTSET_ALWAYS
    gui.menu_fontset = NOFONTSET;
#  ਨਹੀਂਤਾਂ
    gui.menu_font = NOFONT;
#  endif
# endif
    gui.menu_is_ਸਰਗਰਮ = true;	    // default: include menu
# ifndef FEAT_GUI_GTK
    gui.menu_ਉਚਾਈ = MENU_DEFAULT_ਉਚਾਈ;
    gui.menu_ਚੌੜਾਈ = 0;
# endif
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    gui.toolbar_ਉਚਾਈ = 0;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_TIP
    gui.tooltip_fontset = NOFONTSET;
#ਜੇ_ਅੰਤ

    gui.scrollbar_ਚੌੜਾਈ = gui.scrollbar_ਉਚਾਈ = SB_DEFAULT_ਚੌੜਾਈ;
    gui.prev_wrap = -1;

#ਜੇ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // Note: gui_set_ligatures() might already have been called e.g. from .vimrc,
    // and in that case we don't want to overwrite ligatures map that has already
    // been correctly populated (as that would lead to a cleared ligatures maps).
    ਜੇ (*p_guiligatures == NUL)
	CLEAR_ਖੇਤਰ(gui.ligatures_map);
#ਜੇ_ਅੰਤ

#ਜੇ defined(ALWAYS_USE_GUI) || defined(VIMDLL)
    ਨਤੀਜਾ = OK;
#ਨਹੀਂਤਾਂ
# ifdef FEAT_GUI_GTK
    gui.is_x11 = false;
#  ifdef FEAT_GUI_DIALOG
    gui.dialogs_ਸਰਗਰਮ = 0;
    gui.dialog_focus_pending = 0;
#  endif
#  ifdef GDK_WINDOWING_WAYLAND
    gui.is_wayland = false;
#  endif
    /*
     * Note: Don't call gtk_init_check() before fork, it will be called after
     * the fork. When calling it before fork, it make vim hang for a while.
     * See gui_do_fork().
     * Use a simpler check if the GUI window can probably be opened.
     */
    ਨਤੀਜਾ = gui.dofork ? gui_mch_early_init_check(TRUE) : gui_mch_init_check();
# ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = gui_mch_init_check();
# endif
#ਜੇ_ਅੰਤ
    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * This is the call which starts the GUI.
 */
    ਖਾਲੀ
gui_init(ਖਾਲੀ)
{
    win_T	*wp;
    tabpage_T	*tp;
    ਸਥਿਰ ਪੂਰਨ	recursive = 0;

    /*
     * It's possible to use ":gui" in a .gvimrc file.  The first half of this
     * function will then be executed at the first call, the rest by the
     * recursive call.  This allow the shell to be opened halfway reading a
     * gvimrc file.
     */
    ਜੇ (!recursive)
    {
	++recursive;

	clip_init(TRUE);

	// If can't initialize, don't try doing the rest
	ਜੇ (gui_init_check() == FAIL)
	{
	    --recursive;
	    clip_init(FALSE);
	    ਵਾਪਸ;
	}

	/*
	 * Reset 'paste'.  It's useful in the terminal, but not in the GUI.  It
	 * breaks the Paste toolbar button.
	 */
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"paste", 0L, ਨਿਰਾ, 0);

	// Set t_Co to the number of colors: RGB.
	ਸੈੱਟ_color_ਗਿਣਤੀ(256 * 256 * 256);

	/*
	 * Set up system-wide default menus.
	 */
#ਜੇ defined(SYS_MENU_FILE) && defined(FEAT_MENU)
	ਜੇ (vim_strchr(p_go, GO_NOSYSMENU) == ਨਿਰਾ)
	{
	    sys_menu = TRUE;
	    do_ਸਰੋਤ((char_u *)SYS_MENU_FILE, FALSE, DOSO_NONE, ਨਿਰਾ);
	    sys_menu = FALSE;
	}
#ਜੇ_ਅੰਤ

	/*
	 * Switch on the mouse by default, unless the user changed it already.
	 * This can then be changed in the .gvimrc.
	 */
	ਜੇ (!option_was_ਸੈੱਟ((char_u *)"mouse"))
	    ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"mouse", -1,
					   (char_u *)"a", OPT_FREE, SID_NONE);

	/*
	 * If -U option given, use only the initializations from that file and
	 * nothing else.  Skip all initializations for "-U NONE" or "-u NORC".
	 */
	ਜੇ (use_gvimrc != ਨਿਰਾ)
	{
	    ਜੇ (STRCMP(use_gvimrc, "NONE") != 0
		    && STRCMP(use_gvimrc, "NORC") != 0
		    && do_ਸਰੋਤ(use_gvimrc, FALSE, DOSO_NONE, ਨਿਰਾ) != OK)
		semsg(_(e_cannot_read_from_str), use_gvimrc);
	}
	ਨਹੀਂਤਾਂ
	{
	    /*
	     * Get system wide defaults for gvim, only when file name defined.
	     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SYS_GVIMRC_FILE
	    do_ਸਰੋਤ((char_u *)SYS_GVIMRC_FILE, FALSE, DOSO_NONE, ਨਿਰਾ);
#ਜੇ_ਅੰਤ

	    /*
	     * Try to read GUI initialization commands from the following
	     * places:
	     * - environment variable GVIMINIT
	     * - the user gvimrc file (~/.gvimrc)
	     * - the second user gvimrc file ($VIM/.gvimrc for Dos)
	     * - the third user gvimrc file ($VIM/.gvimrc for Amiga)
	     * The first that exists is used, the rest is ignored.
	     */
	    ਜੇ (process_env((char_u *)"GVIMINIT", FALSE) == FAIL
		 && do_ਸਰੋਤ((char_u *)USR_GVIMRC_FILE, TRUE,
						     DOSO_GVIMRC, ਨਿਰਾ) == FAIL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USR_GVIMRC_FILE2
		 && do_ਸਰੋਤ((char_u *)USR_GVIMRC_FILE2, TRUE,
						     DOSO_GVIMRC, ਨਿਰਾ) == FAIL
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USR_GVIMRC_FILE3
		 && do_ਸਰੋਤ((char_u *)USR_GVIMRC_FILE3, TRUE,
						     DOSO_GVIMRC, ਨਿਰਾ) == FAIL
#ਜੇ_ਅੰਤ
				)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USR_GVIMRC_FILE4
		(ਖਾਲੀ)do_ਸਰੋਤ((char_u *)USR_GVIMRC_FILE4, TRUE,
							    DOSO_GVIMRC, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
	    }

	    /*
	     * Read initialization commands from ".gvimrc" in current
	     * directory.  This is only done if the 'exrc' option is set.
	     * Because of security reasons we disallow shell and write
	     * commands now, except for unix if the file is owned by the user
	     * or 'secure' option has been reset in environment of global
	     * ".gvimrc".
	     * Only do this if GVIMRC_FILE is not the same as USR_GVIMRC_FILE,
	     * USR_GVIMRC_FILE2, USR_GVIMRC_FILE3 or SYS_GVIMRC_FILE.
	     */
	    ਜੇ (p_exrc)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		{
		    stat_T s;

		    // if ".gvimrc" file is not owned by user, set 'secure'
		    //ਬਹੁਲਕ
		    ਜੇ (mch_stat(GVIMRC_FILE, &s) || s.st_uid != getuid())
			secure = p_secure;
		}
#ਨਹੀਂਤਾਂ
		secure = p_secure;
#ਜੇ_ਅੰਤ

		ਜੇ (       fullpathcmp((char_u *)USR_GVIMRC_FILE,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SYS_GVIMRC_FILE
			&& fullpathcmp((char_u *)SYS_GVIMRC_FILE,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USR_GVIMRC_FILE2
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE2,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USR_GVIMRC_FILE3
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE3,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USR_GVIMRC_FILE4
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE4,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#ਜੇ_ਅੰਤ
			)
		    do_ਸਰੋਤ((char_u *)GVIMRC_FILE, TRUE, DOSO_GVIMRC, ਨਿਰਾ);

		ਜੇ (secure == 2)
		    need_wait_return = TRUE;
		secure = 0;
	    }
	}

	ਜੇ (need_wait_return || ਸੁਨੇਹਾ_didany)
	    wait_return(TRUE);

	--recursive;
    }

    // If recursive call opened the shell, return here from the first call
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ;

    /*
     * Create the GUI shell.
     */
    gui.ਵਿੱਚ_use = true;		// Must be set after menus have been set up
#ਜੇ defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
    gui.scale = 1.0; // Default value
#ਜੇ_ਅੰਤ
    ਜੇ (gui_mch_init() == FAIL)
	ਜਾਓ error;

    // Avoid a delay for an error message that was printed in the terminal
    // where Vim was started.
    emsg_on_display = FALSE;
    ਸੁਨੇਹਾ_scrolled = 0;
    clear_sb_ਪਾਠ(TRUE);
    need_wait_return = FALSE;
    ਸੁਨੇਹਾ_didany = FALSE;

    /*
     * Check validity of any generic resources that may have been loaded.
     */
    ਜੇ (gui.border_ਚੌੜਾਈ < 0)
	gui.border_ਚੌੜਾਈ = 0;

    /*
     * Set up the fonts.  First use a font specified with "-fn" or "-font".
     */
    ਜੇ (font_ਦਲੀਲ != ਨਿਰਾ)
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"gfn",
					       0L, (char_u *)font_ਦਲੀਲ, 0);
    ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XFONTSET
	    (*p_guifontset == NUL
	     || gui_init_font(p_guifontset, TRUE) == FAIL) &&
#ਜੇ_ਅੰਤ
	    gui_init_font(*p_guifont == NUL ? hl_get_font_name()
						  : p_guifont, FALSE) == FAIL)
    {
	emsg(_(e_cannot_ਸ਼ੁਰੂ_gui_no_valid_font_ਮਿਲਿਆ));
	ਜਾਓ error2;
    }
    ਜੇ (gui_get_wide_font() == FAIL)
	emsg(_(e_guifontwide_invalid));

    gui.ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ = Columns;
    gui.ਅੰਕ_ਪੰਕਤੀਆਂ = ਪੰਕਤੀਆਂ;
    gui_reset_scroll_region();

    // Create initial scrollbars
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_ਖੱਬਾ], SBAR_ਖੱਬਾ, wp);
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_ਸੱਜਾ], SBAR_ਸੱਜਾ, wp);
    }
    gui_create_scrollbar(&gui.bottom_sbar, SBAR_BOTTOM, ਨਿਰਾ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    gui_create_initial_menus(ਜੜ੍ਹ_menu);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGN_ICONS
    sign_gui_started();
#ਜੇ_ਅੰਤ

    // Configure the desired menu and scrollbars
    gui_init_which_components(ਨਿਰਾ);

    // All components of the GUI have been created now
    gui.shell_created = true;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
    // Set the shell size, adjusted for the screen size.  For GTK this only
    // works after the shell has been opened, thus it is further down.
    // If the window is already maximized (e.g. when --windowid is passed in),
    // we want to use the system-provided dimensions by passing FALSE to
    // mustset. Otherwise, we want to initialize with the default rows/columns.
    ਜੇ (gui_mch_maximized())
	gui_ਸੈੱਟ_shellsize(FALSE, TRUE, RESIZE_BOTH);
    ਨਹੀਂਤਾਂ
	gui_ਸੈੱਟ_shellsize(TRUE, TRUE, RESIZE_BOTH);
#ਨਹੀਂਤਾਂ
# ifndef FEAT_GUI_GTK
    gui_ਸੈੱਟ_shellsize(FALSE, TRUE, RESIZE_BOTH);
# endif
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    // Need to set the size of the menubar after all the menus have been
    // created.
    gui_mch_compute_menu_ਉਚਾਈ((Widget)0);
#ਜੇ_ਅੰਤ

    /*
     * Actually open the GUI shell.
     */
    ਜੇ (gui_mch_open() != FAIL)
    {
	maketitle();
	resettitle();

	init_gui_options();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
	// Our GUI can't do bidi.
	p_tbidi = FALSE;
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI_GTK)
	// Give GTK+ a chance to put all widget's into place.
	gui_mch_update();

# ifdef FEAT_MENU
	// If there is no 'm' in 'guioptions' we need to remove the menu now.
	// It was still there to make F10 work.
	ਜੇ (vim_strchr(p_go, GO_MENUS) == ਨਿਰਾ)
	{
	    bool save_starting = gui.starting;

	    gui.starting = false;
	    gui_mch_enable_menu(FALSE);
	    gui.starting = save_starting;
	    gui_mch_update();
	}
# endif

	// Now make sure the shell fits on the screen.
	ਜੇ (gui_mch_maximized())
	    gui_ਸੈੱਟ_shellsize(FALSE, TRUE, RESIZE_BOTH);
	ਨਹੀਂਤਾਂ
	    gui_ਸੈੱਟ_shellsize(TRUE, TRUE, RESIZE_BOTH);
#ਜੇ_ਅੰਤ
	// When 'lines' was set while starting up the topframe may have to be
	// resized.
	win_new_shellsize();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_GUI
	// Always create the Balloon Evaluation area, but disable it when
	// 'ballooneval' is off.
	ਜੇ (balloonEval != ਨਿਰਾ)
	{
# ifdef FEAT_VARTABS
	    vim_free(balloonEval->vts);
# endif
	    vim_free(balloonEval);
	}
	balloon_Eval_For_ਪਦ = FALSE;
# ifdef FEAT_GUI_GTK
	balloonEval = gui_mch_create_beval_area(gui.drawarea, ਨਿਰਾ,
						     &general_beval_cb, ਨਿਰਾ);
# ਨਹੀਂਤਾਂ
#  ਜੇ defined(FEAT_GUI_MOTIF)
	{
	    ਬਾਹਰੀ Widget	ਪਾਠ_Area;
	    balloonEval = gui_mch_create_beval_area(ਪਾਠ_Area, ਨਿਰਾ,
						     &general_beval_cb, ਨਿਰਾ);
	}
#  ਨਹੀਂਤਾਂ
#   ifdef FEAT_GUI_MSWIN
	balloonEval = gui_mch_create_beval_area(ਨਿਰਾ, ਨਿਰਾ,
						     &general_beval_cb, ਨਿਰਾ);
#   endif
#  endif
# endif
	ਜੇ (!p_beval)
	    gui_mch_disable_beval_area(balloonEval);
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
	// In the GUI modifiers are prepended to keys.
	// Don't do this for MS-Windows yet, it sends CTRL-K without the
	// modifier.
	ਵੇਖਿਆ_Modify_Other_ਕੁੰਜੀਆਂ = TRUE;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	ਜੇ (!im_xim_isvalid_imactivate())
	    emsg(_(e_ਮੁੱਲ_ਦਾ_imactivatekey_is_invalid));
#ਜੇ_ਅੰਤ
	// When 'cmdheight' was set during startup it may not have taken
	// effect yet.
	ਜੇ (p_ch != 1L)
	    command_ਉਚਾਈ();

	ਵਾਪਸ;
    }

error2:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_X11
    // undo gui_mch_init()
    gui_mch_uninit();
#ਜੇ_ਅੰਤ

error:
    gui.ਵਿੱਚ_use = false;
    clip_init(FALSE);
}


    ਖਾਲੀ
gui_exit(ਪੂਰਨ rc)
{
    // don't free the fonts, it leads to a BUS error
    // richard@whitequeen.com Jul 99
    free_highlight_fonts();
    gui.ਵਿੱਚ_use = false;
    gui_mch_exit(rc);
}

#ਜੇ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_PHOTON)
# define NEED_GUI_UPDATE_SCREEN 1
/*
 * Called when the GUI shell is closed by the user.  If there are no changed
 * files Vim exits, otherwise there will be a dialog to ask the user what to
 * do.
 * When this function returns, Vim should NOT exit!
 */
    ਖਾਲੀ
gui_shell_closed(ਖਾਲੀ)
{
    cmdmod_T	    save_cmdmod = cmdmod;

    ਜੇ (before_quit_autocmds(curwin, TRUE, FALSE))
	ਵਾਪਸ;

    // Only exit when there are no changed files
    exiting = TRUE;
# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    cmdmod.cmod_flags |= CMOD_CONFIRM;
# endif
    // If there are changed buffers, present the user with a dialog if
    // possible, otherwise give an error message.
    ਜੇ (!check_ਬਦਲਿਆ_any(FALSE, FALSE))
	getout(0);

    exiting = FALSE;
    cmdmod = save_cmdmod;
    gui_update_screen();	// redraw, window may show changed buffer
}
#ਜੇ_ਅੰਤ

/*
 * Set the font.  "font_list" is a comma separated list of font names.  The
 * first font name that works is used.  If none is found, use the default
 * font.
 * If "fontset" is TRUE, the "font_list" is used as one name for the fontset.
 * Return OK when able to set the font.  When it failed FAIL is returned and
 * the fonts are unchanged.
 */
    ਪੂਰਨ
gui_init_font(char_u *font_list, ਪੂਰਨ fontset ਅਣਵਰਤਿਆ)
{
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FONTLEN 320
    char_u	font_name[FONTLEN];
    ਪੂਰਨ		font_list_ਖਾਲੀ_ਥਾਵਾਂ = FALSE;
    ਪੂਰਨ		ret = FAIL;

    ਜੇ (!gui.ਵਿੱਚ_use)
	ਵਾਪਸ FAIL;

    font_name[0] = NUL;
    ਜੇ (*font_list == NUL)
	font_list_ਖਾਲੀ_ਥਾਵਾਂ = TRUE;
    ਨਹੀਂਤਾਂ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XFONTSET
	// When using a fontset, the whole list of fonts is one name.
	ਜੇ (fontset)
	    ret = gui_mch_init_font(font_list, TRUE);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ਜਦੋਂ (*font_list != NUL)
	    {
		// Isolate one comma separated font name.
		(ਖਾਲੀ)copy_option_part(&font_list, font_name, FONTLEN, ",");

		// Careful!!!  The Win32 version of gui_mch_init_font(), when
		// called with "*" will change p_guifont to the selected font
		// name, which frees the old value.  This makes font_list
		// invalid.  Thus when OK is returned here, font_list must no
		// longer be used!
		ਜੇ (gui_mch_init_font(font_name, FALSE) == OK)
		{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ਸੈੱਟ_GUIFONTWIDE
		    // If it's a Unicode font, try setting 'guifontwide' to a
		    // similar double-width font.
		    ਜੇ ((p_guifontwide == ਨਿਰਾ || *p_guifontwide == NUL)
				&& ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)font_name, "10646") != ਨਿਰਾ)
			ਸੈੱਟ_guifontwide(font_name);
#ਜੇ_ਅੰਤ
		    ret = OK;
		    ਰੋਕੋ;
		}
	    }
    }

    ਜੇ (ret != OK
	    && STRCMP(font_list, "*") != 0
	    && (font_list_ਖਾਲੀ_ਥਾਵਾਂ || gui.norm_font == NOFONT))
    {
	/*
	 * Couldn't load any font in 'font_list', keep the current font if
	 * there is one.  If 'font_list' is empty, or if there is no current
	 * font, tell gui_mch_init_font() to try to find a font we can load.
	 */
	ret = gui_mch_init_font(ਨਿਰਾ, FALSE);
    }

    ਜੇ (ret == OK)
    {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
	// Set normal font as current font
# ifdef FEAT_XFONTSET
	ਜੇ (gui.fontset != NOFONTSET)
	    gui_mch_ਸੈੱਟ_fontset(gui.fontset);
	ਨਹੀਂਤਾਂ
# endif
	    gui_mch_ਸੈੱਟ_font(gui.norm_font);
#ਜੇ_ਅੰਤ
	gui_ਸੈੱਟ_shellsize(FALSE, TRUE, RESIZE_BOTH);
    }

    ਵਾਪਸ ret;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ਸੈੱਟ_GUIFONTWIDE
/*
 * Try setting 'guifontwide' to a font twice as wide as "name".
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_guifontwide(char_u *name)
{
    ਪੂਰਨ		i = 0;
    char_u	wide_name[FONTLEN + 10]; // room for 2 * width and '*'
    char_u	*wp = ਨਿਰਾ;
    char_u	*p;
    GuiFont	font;

    wp = wide_name;
    ਲਈ (p = name; *p != NUL; ++p)
    {
	*wp++ = *p;
	ਜੇ (*p == '-')
	{
	    ++i;
	    ਜੇ (i == 6)		// font type: change "--" to "-*-"
	    {
		ਜੇ (p[1] == '-')
		    *wp++ = '*';
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (i == 12)	// found the width
	    {
		++p;
		i = getdigits(&p);
		ਜੇ (i != 0)
		{
		    // Double the width specification.
		    sprintf((ਅੱਖਰ *)wp, "%d%s", i * 2, p);
		    font = gui_mch_get_font(wide_name, FALSE);
		    ਜੇ (font != NOFONT)
		    {
			gui_mch_free_font(gui.wide_font);
			gui.wide_font = font;
			ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"gfw", -1,
						      wide_name, OPT_FREE, 0);
		    }
		}
		ਰੋਕੋ;
	    }
	}
    }
}
#ਜੇ_ਅੰਤ

/*
 * Get the font for 'guifontwide'.
 * Return FAIL for an invalid font name.
 */
    ਪੂਰਨ
gui_get_wide_font(ਖਾਲੀ)
{
    GuiFont	font = NOFONT;
    char_u	font_name[FONTLEN];
    char_u	*p;

    ਜੇ (!gui.ਵਿੱਚ_use)	    // Can't allocate font yet, assume it's OK.
	ਵਾਪਸ OK;	    // Will give an error message later.

    ਜੇ (p_guifontwide != ਨਿਰਾ && *p_guifontwide != NUL)
    {
	ਲਈ (p = p_guifontwide; *p != NUL; )
	{
	    // Isolate one comma separated font name.
	    (ਖਾਲੀ)copy_option_part(&p, font_name, FONTLEN, ",");
	    font = gui_mch_get_font(font_name, FALSE);
	    ਜੇ (font != NOFONT)
		ਰੋਕੋ;
	}
	ਜੇ (font == NOFONT)
	    ਵਾਪਸ FAIL;
    }

    gui_mch_free_font(gui.wide_font);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    ਜੇ (font != NOFONT && gui.norm_font != NOFONT
			 && pango_font_description_equal(font, gui.norm_font))
    {
	gui.wide_font = NOFONT;
	gui_mch_free_font(font);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	gui.wide_font = font;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
    gui_mch_wide_font_ਬਦਲਿਆ();
#ਨਹੀਂਤਾਂ
    /*
     * TODO: setup wide_bold_font, wide_ital_font and wide_boldital_font to
     * support those fonts for 'guifontwide'.
     */
#ਜੇ_ਅੰਤ
    ਵਾਪਸ OK;
}

#ਜੇ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * Set list of ascii characters that combined can create ligature.
 * Store them in char map for quick access from gui_gtk_draw_string.
 */
    ਖਾਲੀ
gui_ਸੈੱਟ_ligatures(ਖਾਲੀ)
{
    char_u	*p;

    ਜੇ (*p_guiligatures != NUL)
    {
	// check for invalid characters
	ਲਈ (p = p_guiligatures; *p != NUL; ++p)
	    ਜੇ (*p < 32 || *p > 127)
	    {
		emsg(_(e_ascii_ਕੋਡ_not_ਵਿੱਚ_range));
		ਵਾਪਸ;
	    }

	// store valid setting into ligatures_map
	CLEAR_ਖੇਤਰ(gui.ligatures_map);
	ਲਈ (p = p_guiligatures; *p != NUL; ++p)
	    gui.ligatures_map[*p] = 1;
    }
    ਨਹੀਂਤਾਂ
	CLEAR_ਖੇਤਰ(gui.ligatures_map);
}

/*
 * Adjust the columns to undraw for when the cursor is on ligatures.
 */
    ਸਥਿਰ ਖਾਲੀ
gui_adjust_undraw_cursor_for_ligatures(ਪੂਰਨ *startcol, ਪੂਰਨ *endcol)
{
    ਪੂਰਨ off;

    ਜੇ (Screen_ਸਤਰਾਂ == ਨਿਰਾ || *p_guiligatures == NUL)
	ਵਾਪਸ;

    // expand before the cursor for all the chars in gui.ligatures_map
    off = ਸਤਰ_ਵਿੱਥ[gui.cursor_ਪੰਕਤੀ] + *startcol;
    ਜੇ (gui.ligatures_map[Screen_ਸਤਰਾਂ[off]])
	ਜਦੋਂ (*startcol > 0 && gui.ligatures_map[Screen_ਸਤਰਾਂ[--off]])
	    (*startcol)--;

    // expand after the cursor for all the chars in gui.ligatures_map
    off = ਸਤਰ_ਵਿੱਥ[gui.cursor_ਪੰਕਤੀ] + *endcol;
    ਜੇ (gui.ligatures_map[Screen_ਸਤਰਾਂ[off]])
	ਜਦੋਂ (*endcol < ((ਪੂਰਨ)screen_Columns - 1)
				      && gui.ligatures_map[Screen_ਸਤਰਾਂ[++off]])
	   (*endcol)++;
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਖਾਲੀ
gui_ਸੈੱਟ_cursor(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    gui.ਪੰਕਤੀ = ਪੰਕਤੀ;
    gui.ਖਾਨਾ = ਖਾਨਾ;
}

/*
 * gui_check_pos - check if the cursor is on the screen.
 */
    ਸਥਿਰ ਖਾਲੀ
gui_check_pos(ਖਾਲੀ)
{
    ਜੇ (gui.ਪੰਕਤੀ >= screen_ਪੰਕਤੀਆਂ)
	gui.ਪੰਕਤੀ = screen_ਪੰਕਤੀਆਂ - 1;
    ਜੇ (gui.ਖਾਨਾ >= screen_Columns)
	gui.ਖਾਨਾ = screen_Columns - 1;
    ਜੇ (gui.cursor_ਪੰਕਤੀ >= screen_ਪੰਕਤੀਆਂ || gui.cursor_ਖਾਨਾ >= screen_Columns)
	gui.cursor_is_valid = false;
}

/*
 * Redraw the cursor if necessary or when forced.
 * Careful: The contents of ScreenLines[] must match what is on the screen,
 * otherwise this goes wrong.  May need to call out_flush() first.
 */
    ਖਾਲੀ
gui_update_cursor(
    ਪੂਰਨ		force,		 // when TRUE, update even when not moved
    ਪੂਰਨ		clear_selection) // clear selection under cursor
{
    ਪੂਰਨ		cur_ਚੌੜਾਈ = 0;
    ਪੂਰਨ		cur_ਉਚਾਈ = 0;
    ਪੂਰਨ		old_hl_ਨਕਾਬ;
    cursorentry_T *shape;
    ਪੂਰਨ		id;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    guicolor_T	shape_fg = INVALCOLOR;
    guicolor_T	shape_bg = INVALCOLOR;
#ਜੇ_ਅੰਤ
    guicolor_T	cfg, cbg, cc;	// cursor fore-/background color
    ਪੂਰਨ		cattr;		// cursor attributes
    ਪੂਰਨ		attr;
    attrentry_T *aep = ਨਿਰਾ;
#ਜੇ (defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)) && !defined(USE_GTK4)
    bool	lig_ਖੱਬਾ = false, lig_ਸੱਜਾ = false;
#ਜੇ_ਅੰਤ

    // Don't update the cursor when halfway busy scrolling or the screen size
    // doesn't match 'columns' and 'lines.  ScreenLines[] isn't valid then.
    ਜੇ (!can_update_cursor || screen_Columns != gui.ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ
					       || screen_ਪੰਕਤੀਆਂ != gui.ਅੰਕ_ਪੰਕਤੀਆਂ)
	ਵਾਪਸ;

    gui_check_pos();

    ਜੇ (gui.cursor_is_valid && !force
		&& gui.ਪੰਕਤੀ == gui.cursor_ਪੰਕਤੀ && gui.ਖਾਨਾ == gui.cursor_ਖਾਨਾ)
	ਵਾਪਸ;

    gui_undraw_cursor();

    // If a cursor-less sleep is ongoing, leave the cursor invisible
    ਜੇ (cursor_is_sleeping())
	ਵਾਪਸ;

    ਜੇ (gui.ਪੰਕਤੀ < 0)
	ਵਾਪਸ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ
    ਜੇ (gui.ਪੰਕਤੀ != gui.cursor_ਪੰਕਤੀ || gui.ਖਾਨਾ != gui.cursor_ਖਾਨਾ)
	im_ਸੈੱਟ_position(gui.ਪੰਕਤੀ, gui.ਖਾਨਾ);
#ਜੇ_ਅੰਤ
    gui.cursor_ਪੰਕਤੀ = gui.ਪੰਕਤੀ;
    gui.cursor_ਖਾਨਾ = gui.ਖਾਨਾ;

    // Only write to the screen after ScreenLines[] has been initialized
    ਜੇ (!screen_cleared || Screen_ਸਤਰਾਂ == ਨਿਰਾ)
	ਵਾਪਸ;

    // Clear the selection if we are about to write over it
    ਜੇ (clear_selection)
	clip_may_clear_selection(gui.ਪੰਕਤੀ, gui.ਪੰਕਤੀ);
    // Check that the cursor is inside the shell (resizing may have made
    // it invalid)
    ਜੇ (gui.ਪੰਕਤੀ >= screen_ਪੰਕਤੀਆਂ || gui.ਖਾਨਾ >= screen_Columns)
	ਵਾਪਸ;

    gui.cursor_is_valid = true;

    /*
     * How the cursor is drawn depends on the current mode.
     * When in a terminal window use the shape/color specified there.
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਜੇ (terminal_is_ਸਰਗਰਮ())
	shape = ਪਦ_get_cursor_shape(&shape_fg, &shape_bg);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	shape = &shape_ਸਾਰਣੀ[get_shape_idx(FALSE)];
    ਜੇ (State & ਬਹੁਲਕ_LANGMAP)
	id = shape->id_lm;
    ਨਹੀਂਤਾਂ
	id = shape->id;

    // get the colors and attributes for the cursor.  Default is inverted
    cfg = INVALCOLOR;
    cbg = INVALCOLOR;
    cattr = HL_INVERSE;
    gui_mch_ਸੈੱਟ_blinking(shape->blinkwait,
	    shape->blinkon,
	    shape->blinkoff);
    ਜੇ (shape->blinkwait == 0 || shape->blinkon == 0
	    || shape->blinkoff == 0)
	gui_mch_stop_blink(FALSE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਜੇ (shape_bg != INVALCOLOR)
    {
	cattr = 0;
	cfg = shape_fg;
	cbg = shape_bg;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਜੇ (id > 0)
	{
	    cattr = syn_id2colors(id, &cfg, &cbg);
#ਜੇ defined(HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ)
	    {
		ਸਥਿਰ ਪੂਰਨ iid;
		guicolor_T fg, bg;

		ਜੇ (
# ਜੇ defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
			preedit_get_ਸਥਿਤੀ()
# ਨਹੀਂਤਾਂ
			im_get_ਸਥਿਤੀ()
# endif
		   )
		{
		    iid = syn_name2id((char_u *)"CursorIM");
		    ਜੇ (iid > 0)
		    {
			syn_id2colors(iid, &fg, &bg);
			ਜੇ (bg != INVALCOLOR)
			    cbg = bg;
			ਜੇ (fg != INVALCOLOR)
			    cfg = fg;
		    }
		}
	    }
#ਜੇ_ਅੰਤ
	}

    /*
     * Get the attributes for the character under the cursor.
     * When no cursor color was given, use the character color.
     */
    attr = ScreenAttrs[ਸਤਰ_ਵਿੱਥ[gui.ਪੰਕਤੀ] + gui.ਖਾਨਾ];
    ਜੇ (attr > HL_ALL)
	aep = syn_gui_attr2entry(attr);
    ਜੇ (aep != ਨਿਰਾ)
    {
	attr = aep->ae_attr;
	ਜੇ (cfg == INVALCOLOR)
	    cfg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.bg_color
		    : aep->ae_u.gui.fg_color);
	ਜੇ (cbg == INVALCOLOR)
	    cbg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.fg_color
		    : aep->ae_u.gui.bg_color);
    }
    ਜੇ (cfg == INVALCOLOR)
	cfg = (attr & HL_INVERSE) ? gui.ਪਿੱਠ_pixel : gui.norm_pixel;
    ਜੇ (cbg == INVALCOLOR)
	cbg = (attr & HL_INVERSE) ? gui.norm_pixel : gui.ਪਿੱਠ_pixel;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
    ਜੇ (aep != ਨਿਰਾ)
    {
	xim_bg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.fg_color
		: aep->ae_u.gui.bg_color);
	xim_fg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.bg_color
		: aep->ae_u.gui.fg_color);
	ਜੇ (xim_bg_color == INVALCOLOR)
	    xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
		: gui.ਪਿੱਠ_pixel;
	ਜੇ (xim_fg_color == INVALCOLOR)
	    xim_fg_color = (attr & HL_INVERSE) ? gui.ਪਿੱਠ_pixel
		: gui.norm_pixel;
    }
    ਨਹੀਂਤਾਂ
    {
	xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
	    : gui.ਪਿੱਠ_pixel;
	xim_fg_color = (attr & HL_INVERSE) ? gui.ਪਿੱਠ_pixel
	    : gui.norm_pixel;
    }
#ਜੇ_ਅੰਤ

    attr &= ~HL_INVERSE;
    ਜੇ (cattr & HL_INVERSE)
    {
	cc = cbg;
	cbg = cfg;
	cfg = cc;
    }
    cattr &= ~HL_INVERSE;

    /*
     * When we don't have window focus, draw a hollow cursor.
     */
    ਜੇ (!gui.ਵਿੱਚ_focus)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, -1, -1);
#ਨਹੀਂਤਾਂ
	gui_mch_draw_hollow_cursor(cbg);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK4
    // Make sure that character underneath is drawn again in case it is part of
    // a ligature.
    gui_redraw_ਖੰਡ(gui.ਪੰਕਤੀ, gui.ਖਾਨਾ, gui.ਪੰਕਤੀ, gui.ਖਾਨਾ, GUI_MON_NOCLEAR);
    // gui_redraw_block() will invalidate the cursor
    gui.cursor_is_valid = true;
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // If cursor is in the middle of a ligature, then split the ligature at
    // the cursor boundaries by redrawing those cells again.
    ਲਈ (ਪੂਰਨ c = gui.ਖਾਨਾ - 1; c >= 0; c--)
	ਜੇ (!gui.ligatures_map[Screen_ਸਤਰਾਂ[ਸਤਰ_ਵਿੱਥ[gui.ਪੰਕਤੀ] + c]])
	{
	    ਜੇ (c < gui.ਖਾਨਾ - 1)
	    {
		gui_redraw_ਖੰਡ(gui.ਪੰਕਤੀ, c + 1, gui.ਪੰਕਤੀ, gui.ਖਾਨਾ - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_ਖੱਬਾ = true;
# endif
	    }
	    ਰੋਕੋ;
	}

    ਲਈ (ਪੂਰਨ c = gui.ਖਾਨਾ + 1; c < screen_Columns - 1; c++)
	ਜੇ (!gui.ligatures_map[Screen_ਸਤਰਾਂ[ਸਤਰ_ਵਿੱਥ[gui.ਪੰਕਤੀ] + c]])
	{
	    ਜੇ (c > gui.ਖਾਨਾ + 1)
	    {
		gui_redraw_ਖੰਡ(gui.ਪੰਕਤੀ, gui.ਖਾਨਾ + 1, gui.ਪੰਕਤੀ, c - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_ਸੱਜਾ = true;
# endif
	    }
	    ਰੋਕੋ;
	}

# ifndef USE_GTK4
    ਜੇ ((lig_ਖੱਬਾ || lig_ਸੱਜਾ) && shape->shape != SHAPE_ਖੰਡ)
    {
	// Because the cursor is not drawn with gui_screenchar(), must blit the
	// cell again (with its background), so that old ligature does not
	// remain on screen. Not needed for GtkSnapshot, because char beneath is
	// always rerendered (see above ifdef).
	ਪੂਰਨ old = gui.ਖਾਨਾ;
	gui.highlight_ਨਕਾਬ = ScreenAttrs[ਸਤਰ_ਵਿੱਥ[gui.ਪੰਕਤੀ] + gui.ਖਾਨਾ];
	(ਖਾਲੀ)gui_screenchar(ਸਤਰ_ਵਿੱਥ[gui.ਪੰਕਤੀ] + gui.ਖਾਨਾ,
		GUI_MON_NOCLEAR, (guicolor_T)0, (guicolor_T)0, 0);
	gui.ਖਾਨਾ = old;
    }
# endif
    // gui_redraw_block()/gui_screenchar() may invalidate the cursor, make sure
    // to validate it again.
    gui.cursor_is_valid = true;
#ਜੇ_ਅੰਤ

    old_hl_ਨਕਾਬ = gui.highlight_ਨਕਾਬ;
    ਜੇ (shape->shape == SHAPE_ਖੰਡ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, 0, 0);
#ਨਹੀਂਤਾਂ
	/*
	 * Draw the text character with the cursor colors.	Use the
	 * character attributes plus the cursor attributes.
	 */
	gui.highlight_ਨਕਾਬ = (cattr | attr);
	(ਖਾਲੀ)gui_screenchar(ਸਤਰ_ਵਿੱਥ[gui.ਪੰਕਤੀ] + gui.ਖਾਨਾ,
		GUI_MON_IS_CURSOR | GUI_MON_NOCLEAR, cfg, cbg, 0);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ defined(FEAT_RIGHTLEFT)
	ਪੂਰਨ	    ਖਾਨਾ_off = FALSE;
#ਜੇ_ਅੰਤ
	/*
	 * First draw the partial cursor, then overwrite with the text
	 * character, using a transparent background.
	 */
	ਜੇ (shape->shape == SHAPE_VER)
	{
	    cur_ਉਚਾਈ = gui.char_ਉਚਾਈ;
	    cur_ਚੌੜਾਈ = (gui.char_ਚੌੜਾਈ * shape->percentage + 99) / 100;
	}
	ਨਹੀਂਤਾਂ
	{
	    cur_ਉਚਾਈ = (gui.char_ਉਚਾਈ * shape->percentage + 99) / 100;
	    cur_ਚੌੜਾਈ = gui.char_ਚੌੜਾਈ;
	}
	ਜੇ (has_mbyte && (*mb_off2cells)(ਸਤਰ_ਵਿੱਥ[gui.ਪੰਕਤੀ] + gui.ਖਾਨਾ,
		    ਸਤਰ_ਵਿੱਥ[gui.ਪੰਕਤੀ] + screen_Columns) > 1)
	{
	    // Double wide character.
	    ਜੇ (shape->shape != SHAPE_VER)
		cur_ਚੌੜਾਈ += gui.char_ਚੌੜਾਈ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	    ਜੇ (CURSOR_BAR_ਸੱਜਾ)
	    {
		// gui.col points to the left half of the character but
		// the vertical line needs to be on the right half.
		// A double-wide horizontal line is also drawn from the
		// right half in gui_mch_draw_part_cursor().
		ਖਾਨਾ_off = TRUE;
		++gui.ਖਾਨਾ;
	    }
#ਜੇ_ਅੰਤ
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, cur_ਚੌੜਾਈ, cur_ਉਚਾਈ);
#ਨਹੀਂਤਾਂ
	gui_mch_draw_part_cursor(cur_ਚੌੜਾਈ, cur_ਉਚਾਈ, cbg);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_RIGHTLEFT)
	ਜੇ (ਖਾਨਾ_off)
	    --gui.ਖਾਨਾ;
#ਜੇ_ਅੰਤ

	// Doesn't seem to work for MSWindows. We call gui_redraw_block() above
	// for GtkSnapshot.
#ਜੇ !defined(FEAT_GUI_MSWIN) && !defined(USE_GTK4)
	gui.highlight_ਨਕਾਬ = ScreenAttrs[ਸਤਰ_ਵਿੱਥ[gui.ਪੰਕਤੀ] + gui.ਖਾਨਾ];
	(ਖਾਲੀ)gui_screenchar(ਸਤਰ_ਵਿੱਥ[gui.ਪੰਕਤੀ] + gui.ਖਾਨਾ,
		GUI_MON_TRS_CURSOR | GUI_MON_NOCLEAR,
		(guicolor_T)0, (guicolor_T)0, 0);
#ਜੇ_ਅੰਤ
    }
    gui.highlight_ਨਕਾਬ = old_hl_ਨਕਾਬ;
}

#ਜੇ defined(FEAT_MENU)
    ਸਥਿਰ ਖਾਲੀ
gui_position_menu(ਖਾਲੀ)
{
# ਜੇ !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
    ਜੇ (gui.menu_is_ਸਰਗਰਮ && gui.ਵਿੱਚ_use)
	gui_mch_ਸੈੱਟ_menu_pos(0, 0, gui.menu_ਚੌੜਾਈ, gui.menu_ਉਚਾਈ);
# endif
}
#ਜੇ_ਅੰਤ

/*
 * Position the various GUI components (text area, menu).  The vertical
 * scrollbars are NOT handled here.  See gui_update_scrollbars().
 */
    ਸਥਿਰ ਖਾਲੀ
gui_position_components(ਪੂਰਨ ਕੁੱਲ_ਚੌੜਾਈ ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ	    ਪਾਠ_area_x;
    ਪੂਰਨ	    ਪਾਠ_area_y;
    ਪੂਰਨ	    ਪਾਠ_area_ਚੌੜਾਈ;
    ਪੂਰਨ	    ਪਾਠ_area_ਉਚਾਈ;

    // avoid that moving components around generates events
    ++hold_gui_events;

    ਪਾਠ_area_x = 0;
    ਜੇ (gui.which_scrollbars[SBAR_ਖੱਬਾ])
	ਪਾਠ_area_x += gui.scrollbar_ਚੌੜਾਈ;

    ਪਾਠ_area_y = 0;
#ਜੇ defined(FEAT_MENU) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    gui.menu_ਚੌੜਾਈ = ਕੁੱਲ_ਚੌੜਾਈ;
    ਜੇ (gui.menu_is_ਸਰਗਰਮ)
	ਪਾਠ_area_y += gui.menu_ਉਚਾਈ;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
    ਜੇ (gui_has_tabline())
	ਪਾਠ_area_y += gui.tabline_ਉਚਾਈ;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    ਜੇ (vim_strchr(p_go, GO_TOOLBAR) != ਨਿਰਾ)
    {
# ਜੇ defined(FEAT_GUI_HAIKU)
	gui_mch_ਸੈੱਟ_toolbar_pos(0, ਪਾਠ_area_y,
				gui.menu_ਚੌੜਾਈ, gui.toolbar_ਉਚਾਈ);
# endif
	ਪਾਠ_area_y += gui.toolbar_ਉਚਾਈ;
    }
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_HAIKU)
    gui_mch_ਸੈੱਟ_tabline_pos(0, ਪਾਠ_area_y,
    gui.menu_ਚੌੜਾਈ, gui.tabline_ਉਚਾਈ);
    ਜੇ (gui_has_tabline())
	ਪਾਠ_area_y += gui.tabline_ਉਚਾਈ;
#ਜੇ_ਅੰਤ

    ਪਾਠ_area_ਚੌੜਾਈ = gui.ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ * gui.char_ਚੌੜਾਈ + gui.border_ਵਿੱਥ * 2;
    ਪਾਠ_area_ਉਚਾਈ = gui.ਅੰਕ_ਪੰਕਤੀਆਂ * gui.char_ਉਚਾਈ + gui.border_ਵਿੱਥ * 2;

    gui_mch_ਸੈੱਟ_ਪਾਠ_area_pos(ਪਾਠ_area_x,
			      ਪਾਠ_area_y,
			      ਪਾਠ_area_ਚੌੜਾਈ,
			      ਪਾਠ_area_ਉਚਾਈ
#ਜੇ defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
				  + xim_get_ਸਥਿਤੀ_area_ਉਚਾਈ()
#ਜੇ_ਅੰਤ
			      );
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    gui_position_menu();
#ਜੇ_ਅੰਤ
    ਜੇ (gui.which_scrollbars[SBAR_BOTTOM])
	gui_mch_ਸੈੱਟ_scrollbar_pos(&gui.bottom_sbar,
				  ਪਾਠ_area_x,
				  ਪਾਠ_area_y + ਪਾਠ_area_ਉਚਾਈ
					+ gui_mch_get_scrollbar_ypadding(),
				  ਪਾਠ_area_ਚੌੜਾਈ,
				  gui.scrollbar_ਉਚਾਈ);
    gui.ਖੱਬਾ_sbar_x = 0;
    gui.ਸੱਜਾ_sbar_x = ਪਾਠ_area_x + ਪਾਠ_area_ਚੌੜਾਈ
					+ gui_mch_get_scrollbar_xpadding();

    --hold_gui_events;
}

/*
 * Get the width of the widgets and decorations to the side of the text area.
 */
    ਪੂਰਨ
gui_get_ਅਧਾਰ_ਚੌੜਾਈ(ਖਾਲੀ)
{
    ਪੂਰਨ	    ਅਧਾਰ_ਚੌੜਾਈ;

    ਅਧਾਰ_ਚੌੜਾਈ = 2 * gui.border_ਵਿੱਥ;
    ਜੇ (gui.which_scrollbars[SBAR_ਖੱਬਾ])
	ਅਧਾਰ_ਚੌੜਾਈ += gui.scrollbar_ਚੌੜਾਈ;
    ਜੇ (gui.which_scrollbars[SBAR_ਸੱਜਾ])
	ਅਧਾਰ_ਚੌੜਾਈ += gui.scrollbar_ਚੌੜਾਈ;
    ਵਾਪਸ ਅਧਾਰ_ਚੌੜਾਈ;
}

/*
 * Get the height of the widgets and decorations above and below the text area.
 */
    ਪੂਰਨ
gui_get_ਅਧਾਰ_ਉਚਾਈ(ਖਾਲੀ)
{
    ਪੂਰਨ	    ਅਧਾਰ_ਉਚਾਈ;

    ਅਧਾਰ_ਉਚਾਈ = 2 * gui.border_ਵਿੱਥ;
    ਜੇ (gui.which_scrollbars[SBAR_BOTTOM])
	ਅਧਾਰ_ਉਚਾਈ += gui.scrollbar_ਉਚਾਈ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
    // We can't take the sizes properly into account until anything is
    // realized.  Therefore we recalculate all the values here just before
    // setting the size. (--mdcki)
#ਨਹੀਂਤਾਂ
# ifdef FEAT_MENU
    ਜੇ (gui.menu_is_ਸਰਗਰਮ)
	ਅਧਾਰ_ਉਚਾਈ += gui.menu_ਉਚਾਈ;
# endif
# ifdef FEAT_TOOLBAR
    ਜੇ (vim_strchr(p_go, GO_TOOLBAR) != ਨਿਰਾ)
	ਅਧਾਰ_ਉਚਾਈ += gui.toolbar_ਉਚਾਈ;
# endif
# ਜੇ defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    ਜੇ (gui_has_tabline())
	ਅਧਾਰ_ਉਚਾਈ += gui.tabline_ਉਚਾਈ;
# endif
# ਜੇ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    ਅਧਾਰ_ਉਚਾਈ += gui_mch_ਪਾਠ_area_extra_ਉਚਾਈ();
# endif
#ਜੇ_ਅੰਤ
    ਵਾਪਸ ਅਧਾਰ_ਉਚਾਈ;
}

/*
 * Should be called after the GUI shell has been resized.  Its arguments are
 * the new width and height of the shell in pixels.
 */
    ਖਾਲੀ
gui_resize_shell(ਪੂਰਨ pixel_ਚੌੜਾਈ, ਪੂਰਨ pixel_ਉਚਾਈ)
{
    ਸਥਿਰ ਪੂਰਨ	busy = FALSE;

    ਜੇ (!gui.shell_created)	    // ignore when still initializing
	ਵਾਪਸ;

    /*
     * Can't resize the screen while it is being redrawn.  Remember the new
     * size and handle it later.
     */
    ਜੇ (updating_screen || busy)
    {
	new_pixel_ਚੌੜਾਈ = pixel_ਚੌੜਾਈ;
	new_pixel_ਉਚਾਈ = pixel_ਉਚਾਈ;
	ਵਾਪਸ;
    }

again:
    new_pixel_ਚੌੜਾਈ = 0;
    new_pixel_ਉਚਾਈ = 0;
    busy = TRUE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_HAIKU
    vim_ਤਾਲਾ_screen();
#ਜੇ_ਅੰਤ

    // Flush pending output before redrawing
    ਬਾਹਰਲਾ_flush();
#ਜੇ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk_init_decor_ਉਚਾਈ();
#ਜੇ_ਅੰਤ

    gui.ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ = (pixel_ਚੌੜਾਈ - gui_get_ਅਧਾਰ_ਚੌੜਾਈ()) / gui.char_ਚੌੜਾਈ;
    gui.ਅੰਕ_ਪੰਕਤੀਆਂ = (pixel_ਉਚਾਈ - gui_get_ਅਧਾਰ_ਉਚਾਈ()) / gui.char_ਉਚਾਈ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK4
    gui_gtk4_update_ਮਾਪ();
#ਜੇ_ਅੰਤ

    gui_position_components(pixel_ਚੌੜਾਈ);
    gui_reset_scroll_region();

#ਜੇ defined(FEAT_GUI_GTK) && defined(USE_GTK4) && !defined(USE_GTK4)
    // We do not resize the draw area via the "resize" signal. This is because
    // when the window is resized, the form widget is the one that is resized,
    // so let that call gui_resize_shell() which will allocate the surface and
    // allocate the drawing area size/position.
    gui_gtk4_resize(pixel_ਚੌੜਾਈ, pixel_ਉਚਾਈ);
#ਜੇ_ਅੰਤ

    /*
     * At the "more" and ":confirm" prompt there is no redraw, put the cursor
     * at the last line here (why does it have to be one row too low?).
     */
    ਜੇ (State == ਬਹੁਲਕ_ASKMORE || State == ਬਹੁਲਕ_CONFIRM)
	gui.ਪੰਕਤੀ = gui.ਅੰਕ_ਪੰਕਤੀਆਂ;

    // Only comparing Rows and Columns may be sufficient, but let's stay on
    // the safe side.
    ਜੇ (gui.ਅੰਕ_ਪੰਕਤੀਆਂ != screen_ਪੰਕਤੀਆਂ || gui.ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ != screen_Columns
	    || gui.ਅੰਕ_ਪੰਕਤੀਆਂ != ਪੰਕਤੀਆਂ || gui.ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ != Columns || gui.force_redraw)
    {
	shell_resized();
	gui.force_redraw = 0;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_HAIKU
    vim_unlock_screen();
#ਜੇ_ਅੰਤ

    gui_update_scrollbars(TRUE);
    gui_update_cursor(FALSE, TRUE);
#ਜੇ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(pixel_ਚੌੜਾਈ, pixel_ਉਚਾਈ);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
    xim_ਸੈੱਟ_ਸਥਿਤੀ_area();
#ਜੇ_ਅੰਤ

    busy = FALSE;

    // We may have been called again while redrawing the screen.
    // Need to do it all again with the latest size then.  But only if the size
    // actually changed.
    ਜੇ (new_pixel_ਉਚਾਈ)
    {
	ਜੇ (pixel_ਚੌੜਾਈ == new_pixel_ਚੌੜਾਈ && pixel_ਉਚਾਈ == new_pixel_ਉਚਾਈ)
	{
	    new_pixel_ਚੌੜਾਈ = 0;
	    new_pixel_ਉਚਾਈ = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    pixel_ਚੌੜਾਈ = new_pixel_ਚੌੜਾਈ;
	    pixel_ਉਚਾਈ = new_pixel_ਉਚਾਈ;
	    ਜਾਓ again;
	}
    }
}

/*
 * Check if gui_resize_shell() must be called.
 */
    ਖਾਲੀ
gui_may_resize_shell(ਖਾਲੀ)
{
    ਜੇ (new_pixel_ਉਚਾਈ)
    {
	// careful: gui_resize_shell() may postpone the resize again if we
	// were called indirectly by it
	gui_resize_shell(new_pixel_ਚੌੜਾਈ, new_pixel_ਉਚਾਈ);
    }
}

    ਪੂਰਨ
gui_get_shellsize(ਖਾਲੀ)
{
    ਪੰਕਤੀਆਂ = gui.ਅੰਕ_ਪੰਕਤੀਆਂ;
    Columns = gui.ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ;
    ਵਾਪਸ OK;
}

/*
 * Set the size of the Vim shell according to Rows and Columns.
 * If "fit_to_display" is TRUE then the size may be reduced to fit the window
 * on the screen.
 * When "mustset" is TRUE the size was set by the user. When FALSE a UI
 * component was added or removed (e.g., a scrollbar).
 */
    ਖਾਲੀ
gui_ਸੈੱਟ_shellsize(
    ਪੂਰਨ		mustset ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		fit_ਮੰਜ਼ਿਲ_display,
    ਪੂਰਨ		direction)		// RESIZE_HOR, RESIZE_VER
{
    ਪੂਰਨ		ਅਧਾਰ_ਚੌੜਾਈ;
    ਪੂਰਨ		ਅਧਾਰ_ਉਚਾਈ;
    ਪੂਰਨ		ਚੌੜਾਈ;
    ਪੂਰਨ		ਉਚਾਈ;
    ਪੂਰਨ		min_ਚੌੜਾਈ;
    ਪੂਰਨ		min_ਉਚਾਈ;
    ਪੂਰਨ		screen_w;
    ਪੂਰਨ		screen_h;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
    ਪੂਰਨ		un_maximize = mustset;
    ਪੂਰਨ		did_adjust = 0;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		x = -1, y = -1;

    ਜੇ (!gui.shell_created)
	ਵਾਪਸ;

#ਜੇ defined(MSWIN) || defined(FEAT_GUI_GTK)
    // If not setting to a user specified size and maximized, calculate the
    // number of characters that fit in the maximized window.
    // FIXME: gui_mch_newfont() is called here even when the font hasn't
    // changed at all.  For example, ":set guioptions=k" triggers this path
    // via gui_init_which_components() -> gui_set_shellsize(FALSE, ...).
    // The intent is to keep the window size and recalculate Rows/Columns,
    // which has nothing to do with fonts.  This should be a separate
    // function with a more descriptive name.
    ਜੇ (!mustset && (vim_strchr(p_go, GO_KEEPWINSIZE) != ਨਿਰਾ
						       || gui_mch_maximized()))
    {
	gui_mch_newfont();
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    ਅਧਾਰ_ਚੌੜਾਈ = gui_get_ਅਧਾਰ_ਚੌੜਾਈ();
    ਅਧਾਰ_ਉਚਾਈ = gui_get_ਅਧਾਰ_ਉਚਾਈ();
    ਜੇ (fit_ਮੰਜ਼ਿਲ_display)
	// Remember the original window position.
	(ਖਾਲੀ)gui_mch_get_winpos(&x, &y);

    ਚੌੜਾਈ = Columns * gui.char_ਚੌੜਾਈ + ਅਧਾਰ_ਚੌੜਾਈ;
    ਉਚਾਈ = ਪੰਕਤੀਆਂ * gui.char_ਉਚਾਈ + ਅਧਾਰ_ਉਚਾਈ;

    ਜੇ (fit_ਮੰਜ਼ਿਲ_display)
    {
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	ਜੇ ((direction & RESIZE_HOR) && ਚੌੜਾਈ > screen_w)
	{
	    Columns = (screen_w - ਅਧਾਰ_ਚੌੜਾਈ) / gui.char_ਚੌੜਾਈ;
	    ਜੇ (Columns < MIN_COLUMNS)
		Columns = MIN_COLUMNS;
	    ਚੌੜਾਈ = Columns * gui.char_ਚੌੜਾਈ + ਅਧਾਰ_ਚੌੜਾਈ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
	    ++did_adjust;
#ਜੇ_ਅੰਤ
	}
	ਜੇ ((direction & RESIZE_VERT) && ਉਚਾਈ > screen_h)
	{
	    ਪੰਕਤੀਆਂ = (screen_h - ਅਧਾਰ_ਉਚਾਈ) / gui.char_ਉਚਾਈ;
	    check_shellsize();
	    ਉਚਾਈ = ਪੰਕਤੀਆਂ * gui.char_ਉਚਾਈ + ਅਧਾਰ_ਉਚਾਈ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
	    ++did_adjust;
#ਜੇ_ਅੰਤ
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
	ਜੇ (did_adjust == 2 || (ਚੌੜਾਈ + gui.char_ਚੌੜਾਈ >= screen_w
				     && ਉਚਾਈ + gui.char_ਉਚਾਈ >= screen_h))
	    // don't unmaximize if at maximum size
	    un_maximize = FALSE;
#ਜੇ_ਅੰਤ
    }
    ਹੱਦਬੰਦੀ_screen_ਮਾਪ();
    gui.ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ = Columns;
    gui.ਅੰਕ_ਪੰਕਤੀਆਂ = ਪੰਕਤੀਆਂ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK4
    // Keep the drawing area in sync with the size Vim is going to draw.
    gui_gtk4_update_ਮਾਪ();
#ਜੇ_ਅੰਤ

    min_ਚੌੜਾਈ = ਅਧਾਰ_ਚੌੜਾਈ + MIN_COLUMNS * gui.char_ਚੌੜਾਈ;
    min_ਉਚਾਈ = ਅਧਾਰ_ਉਚਾਈ + MIN_ਸਤਰਾਂ * gui.char_ਉਚਾਈ;
    min_ਉਚਾਈ += tabline_ਉਚਾਈ() * gui.char_ਉਚਾਈ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
    ਜੇ (un_maximize)
    {
	// If the window size is smaller than the screen unmaximize the
	// window, otherwise resizing won't work.
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	ਜੇ ((ਚੌੜਾਈ + gui.char_ਚੌੜਾਈ < screen_w
				   || ਉਚਾਈ + gui.char_ਉਚਾਈ * 2 < screen_h)
		&& gui_mch_maximized())
	    gui_mch_unmaximize();
    }
#ਜੇ_ਅੰਤ

    gui_mch_ਸੈੱਟ_shellsize(ਚੌੜਾਈ, ਉਚਾਈ, min_ਚੌੜਾਈ, min_ਉਚਾਈ,
					  ਅਧਾਰ_ਚੌੜਾਈ, ਅਧਾਰ_ਉਚਾਈ, direction);

    ਜੇ (fit_ਮੰਜ਼ਿਲ_display && x >= 0 && y >= 0)
    {
	// Some window managers put the Vim window left of/above the screen.
	// Only change the position if it wasn't already negative before
	// (happens on MS-Windows with a secondary monitor).
	gui_mch_update();
	ਜੇ (gui_mch_get_winpos(&x, &y) == OK && (x < 0 || y < 0))
	    gui_mch_ਸੈੱਟ_winpos(x < 0 ? 0 : x, y < 0 ? 0 : y);
    }

    gui_position_components(ਚੌੜਾਈ);
    gui_update_scrollbars(TRUE);
    gui_reset_scroll_region();

#ਜੇ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(ਚੌੜਾਈ, ਉਚਾਈ);
#ਜੇ_ਅੰਤ
}

/*
 * Called when Rows and/or Columns has changed.
 */
    ਖਾਲੀ
gui_new_shellsize(ਖਾਲੀ)
{
    gui_reset_scroll_region();
}

/*
 * Make scroll region cover whole screen.
 */
    ਸਥਿਰ ਖਾਲੀ
gui_reset_scroll_region(ਖਾਲੀ)
{
    gui.scroll_region_ਸਿਖਰ = 0;
    gui.scroll_region_bot = gui.ਅੰਕ_ਪੰਕਤੀਆਂ - 1;
    gui.scroll_region_ਖੱਬਾ = 0;
    gui.scroll_region_ਸੱਜਾ = gui.ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ - 1;
}

    ਸਥਿਰ ਖਾਲੀ
gui_ਸ਼ੁਰੂ_highlight(ਪੂਰਨ ਨਕਾਬ)
{
    ਜੇ (ਨਕਾਬ > HL_ALL)		    // highlight code
	gui.highlight_ਨਕਾਬ = ਨਕਾਬ;
    ਨਹੀਂਤਾਂ			    //ਨਕਾਬ
	gui.highlight_ਨਕਾਬ |= ਨਕਾਬ;
}

    ਖਾਲੀ
gui_stop_highlight(ਪੂਰਨ ਨਕਾਬ)
{
    ਜੇ (ਨਕਾਬ > HL_ALL)		    // highlight code
	gui.highlight_ਨਕਾਬ = HL_ਸਾਧਾਰਨ;
    ਨਹੀਂਤਾਂ			    //ਨਕਾਬ
	gui.highlight_ਨਕਾਬ &= ~ਨਕਾਬ;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    ਖਾਲੀ
gui_clear_ਖੰਡ(
    ਪੂਰਨ	    row1,
    ਪੂਰਨ	    col1,
    ਪੂਰਨ	    row2,
    ਪੂਰਨ	    col2)
{
    // Clear the selection if we are about to write over it
    clip_may_clear_selection(row1, row2);

    gui_mch_clear_ਖੰਡ(row1, col1, row2, col2);

    // Invalidate cursor if it was in this block
    ਜੇ (       gui.cursor_ਪੰਕਤੀ >= row1 && gui.cursor_ਪੰਕਤੀ <= row2
	    && gui.cursor_ਖਾਨਾ >= col1 && gui.cursor_ਖਾਨਾ <= col2)
	gui.cursor_is_valid = false;
}

/*
 * Write code to update the cursor later.  This avoids the need to flush the
 * output buffer before calling gui_update_cursor().
 */
    ਖਾਲੀ
gui_update_cursor_later(ਖਾਲੀ)
{
    ਬਾਹਰਲਾ_STR("\033|s");
}

    ਖਾਲੀ
gui_write(
    char_u	*s,
    ਪੂਰਨ		len)
{
    char_u	*p;
    ਪੂਰਨ		arg1 = 0, arg2 = 0;
    ਪੂਰਨ		force_cursor = FALSE;	// force cursor update
    ਪੂਰਨ		force_scrollbar = FALSE;
    ਸਥਿਰ win_T	*old_curwin = ਨਿਰਾ;

// #define DEBUG_GUI_WRITE
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_GUI_WRITE
    {
	ਪੂਰਨ i;
	char_u *str;

	ਲਿਖੋ("gui_write(%d):\n    ", len);
	ਲਈ (i = 0; i < len; i++)
	    ਜੇ (s[i] == ESC)
	    {
		ਜੇ (i != 0)
		    ਲਿਖੋ("\n    ");
		ਲਿਖੋ("<ESC>");
	    }
	    ਨਹੀਂਤਾਂ
	    {
		str = transchar_byte(s[i]);
		ਜੇ (str[0] && str[1])
		    ਲਿਖੋ("<%s>", (ਅੱਖਰ *)str);
		ਨਹੀਂਤਾਂ
		    ਲਿਖੋ("%s", (ਅੱਖਰ *)str);
	    }
	ਲਿਖੋ("\n");
    }
#ਜੇ_ਅੰਤ
    ਜਦੋਂ (len)
    {
	ਜੇ (s[0] == ESC && s[1] == '|')
	{
	    p = s + 2;
	    ਜੇ (VIM_ISDIGIT(*p) || (*p == '-' && VIM_ISDIGIT(*(p + 1))))
	    {
		arg1 = getdigits(&p);
		ਜੇ (p > s + len)
		    ਰੋਕੋ;
		ਜੇ (*p == ';')
		{
		    ++p;
		    arg2 = getdigits(&p);
		    ਜੇ (p > s + len)
			ਰੋਕੋ;
		}
	    }
	    ਚੋਣ (*p)
	    {
		ਹਾਲਤ 'C':	// Clear screen
		    clip_scroll_selection(9999);
		    gui_mch_clear_all();
		    gui.cursor_is_valid = false;
		    force_scrollbar = TRUE;
		    ਰੋਕੋ;
		ਹਾਲਤ 'M':	// Move cursor
		    gui_ਸੈੱਟ_cursor(arg1, arg2);
		    ਰੋਕੋ;
		ਹਾਲਤ 's':	// force cursor (shape) update
		    force_cursor = TRUE;
		    ਰੋਕੋ;
		ਹਾਲਤ 'R':	// Set scroll region
		    ਜੇ (arg1 < arg2)
		    {
			gui.scroll_region_ਸਿਖਰ = arg1;
			gui.scroll_region_bot = arg2;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			gui.scroll_region_ਸਿਖਰ = arg2;
			gui.scroll_region_bot = arg1;
		    }
		    ਰੋਕੋ;
		ਹਾਲਤ 'V':	// Set vertical scroll region
		    ਜੇ (arg1 < arg2)
		    {
			gui.scroll_region_ਖੱਬਾ = arg1;
			gui.scroll_region_ਸੱਜਾ = arg2;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			gui.scroll_region_ਖੱਬਾ = arg2;
			gui.scroll_region_ਸੱਜਾ = arg1;
		    }
		    ਰੋਕੋ;
		ਹਾਲਤ 'd':	// Delete line
		    gui_delete_ਸਤਰਾਂ(gui.ਪੰਕਤੀ, 1);
		    ਰੋਕੋ;
		ਹਾਲਤ 'D':	// Delete lines
		    gui_delete_ਸਤਰਾਂ(gui.ਪੰਕਤੀ, arg1);
		    ਰੋਕੋ;
		ਹਾਲਤ 'i':	// Insert line
		    gui_ਪਾਓ_ਸਤਰਾਂ(gui.ਪੰਕਤੀ, 1);
		    ਰੋਕੋ;
		ਹਾਲਤ 'I':	// Insert lines
		    gui_ਪਾਓ_ਸਤਰਾਂ(gui.ਪੰਕਤੀ, arg1);
		    ਰੋਕੋ;
		ਹਾਲਤ '$':	// Clear to end-of-line
		    gui_clear_ਖੰਡ(gui.ਪੰਕਤੀ, gui.ਖਾਨਾ, gui.ਪੰਕਤੀ,
							    (ਪੂਰਨ)Columns - 1);
		    ਰੋਕੋ;
		ਹਾਲਤ 'h':	// Turn on highlighting
		    gui_ਸ਼ੁਰੂ_highlight(arg1);
		    ਰੋਕੋ;
		ਹਾਲਤ 'H':	// Turn off highlighting
		    gui_stop_highlight(arg1);
		    ਰੋਕੋ;
		ਹਾਲਤ 'f':	// flash the window (visual bell)
		    gui_mch_flash(arg1 == 0 ? 20 : arg1);
		    ਰੋਕੋ;
		ਮੂਲ:
		    p = s + 1;	// Skip the ESC
		    ਰੋਕੋ;
	    }
	    len -= (ਪੂਰਨ)(++p - s);
	    s = p;
	}
	ਨਹੀਂਤਾਂ ਜੇ (s[0] < 0x20		// Ctrl character
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGN_ICONS
		&& s[0] != SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
		&& s[0] != MULTISIGN_BYTE
# endif
#ਜੇ_ਅੰਤ
		)
	{
	    ਜੇ (s[0] == '\n')		// NL
	    {
		gui.ਖਾਨਾ = 0;
		ਜੇ (gui.ਪੰਕਤੀ < gui.scroll_region_bot)
		    gui.ਪੰਕਤੀ++;
		ਨਹੀਂਤਾਂ
		    gui_delete_ਸਤਰਾਂ(gui.scroll_region_ਸਿਖਰ, 1);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (s[0] == '\r')	// CR
	    {
		gui.ਖਾਨਾ = 0;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (s[0] == '\b')	// Backspace
	    {
		ਜੇ (gui.ਖਾਨਾ)
		    --gui.ਖਾਨਾ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (s[0] == Ctrl_L)	// cursor-right
	    {
		++gui.ਖਾਨਾ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (s[0] == Ctrl_G)	// Beep
	    {
		gui_mch_beep();
	    }
	    // Other Ctrl character: shouldn't happen!

	    --len;	// Skip this char
	    ++s;
	}
	ਨਹੀਂਤਾਂ
	{
	    p = s;
	    ਜਦੋਂ (len > 0 && (
			*p >= 0x20
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGN_ICONS
			|| *p == SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
			|| *p == MULTISIGN_BYTE
# endif
#ਜੇ_ਅੰਤ
			))
	    {
		len--;
		p++;
	    }
	    gui_outstr(s, (ਪੂਰਨ)(p - s));
	    s = p;
	}
    }

    // Postponed update of the cursor (won't work if "can_update_cursor" isn't
    // set).
    ਜੇ (force_cursor)
	gui_update_cursor(TRUE, TRUE);

    // When switching to another window the dragging must have stopped.
    // Required for GTK, dragged_sb isn't reset.
    ਜੇ (old_curwin != curwin)
	gui.dragged_sb = SBAR_NONE;

    // Update the scrollbars after clearing the screen or when switched
    // to another window.
    // Update the horizontal scrollbar always, it's difficult to check all
    // situations where it might change.
    ਜੇ (force_scrollbar || old_curwin != curwin)
	gui_update_scrollbars(force_scrollbar);
    ਨਹੀਂਤਾਂ
	gui_update_horiz_scrollbar(FALSE);
    old_curwin = curwin;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#ਜੇ defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#ਜੇ_ਅੰਤ

    gui_may_flush();		    // In case vim decides to take a nap
}

/*
 * When ScreenLines[] is invalid, updating the cursor should not be done, it
 * produces wrong results.  Call gui_dont_update_cursor() before that code and
 * gui_can_update_cursor() afterwards.
 */
    ਖਾਲੀ
gui_dont_update_cursor(ਪੂਰਨ undraw)
{
    ਜੇ (!gui.ਵਿੱਚ_use)
	ਵਾਪਸ;

    // Undraw the cursor now, we probably can't do it after the change.
    ਜੇ (undraw)
	gui_undraw_cursor();
    can_update_cursor = FALSE;
}

    ਖਾਲੀ
gui_can_update_cursor(ਖਾਲੀ)
{
    can_update_cursor = TRUE;
    // No need to update the cursor right now, there is always more output
    // after scrolling.
}

/*
 * Disable issuing gui_mch_flush().
 */
    ਖਾਲੀ
gui_disable_flush(ਖਾਲੀ)
{
    ++disable_flush;
}

/*
 * Enable issuing gui_mch_flush().
 */
    ਖਾਲੀ
gui_enable_flush(ਖਾਲੀ)
{
    --disable_flush;
}

/*
 * Issue gui_mch_flush() if it is not disabled.
 */
    ਖਾਲੀ
gui_may_flush(ਖਾਲੀ)
{
    ਜੇ (disable_flush == 0)
	gui_mch_flush();
}

    ਸਥਿਰ ਖਾਲੀ
gui_outstr(char_u *s, ਪੂਰਨ len)
{
    ਪੂਰਨ	    this_len;
    ਪੂਰਨ	    cells;

    ਜੇ (len == 0)
	ਵਾਪਸ;

    ਜੇ (len < 0)
	len = (ਪੂਰਨ)STRLEN(s);

    ਜਦੋਂ (len > 0)
    {
	ਜੇ (has_mbyte)
	{
	    // Find out how many chars fit in the current line.
	    cells = 0;
	    ਲਈ (this_len = 0; this_len < len; )
	    {
		cells += (*mb_ptr2cells)(s + this_len);
		ਜੇ (gui.ਖਾਨਾ + cells > Columns)
		    ਰੋਕੋ;
		this_len += (*mb_ptr2len)(s + this_len);
	    }
	    ਜੇ (this_len > len)
		this_len = len;	    // don't include following composing char
	}
	ਨਹੀਂਤਾਂ ਜੇ (gui.ਖਾਨਾ + len > Columns)
	    this_len = Columns - gui.ਖਾਨਾ;
	ਨਹੀਂਤਾਂ
	    this_len = len;

	(ਖਾਲੀ)gui_outstr_nowrap(s, this_len,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	s += this_len;
	len -= this_len;
	// fill up for a double-width char that doesn't fit.
	ਜੇ (len > 0 && gui.ਖਾਨਾ < Columns)
	    (ਖਾਲੀ)gui_outstr_nowrap((char_u *)" ", 1,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	// The cursor may wrap to the next line.
	ਜੇ (gui.ਖਾਨਾ >= Columns)
	{
	    gui.ਖਾਨਾ = 0;
	    gui.ਪੰਕਤੀ++;
	}
    }
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_GTK4
/*
 * Output one character (may be one or two display cells).
 * Caller must check for valid "off".
 * Returns FAIL or OK, just like gui_outstr_nowrap().
 */
    ਸਥਿਰ ਪੂਰਨ
gui_screenchar(
    ਪੂਰਨ		off,	    // Offset from start of screen
    ਪੂਰਨ		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    ਪੂਰਨ		ਪਿੱਠ)	    // backup this many chars when using bold trick
{
    char_u	buf[MB_MAXBYTES + 1];

    // Don't draw right half of a double-width UTF-8 char. "cannot happen"
    ਜੇ (enc_utf8 && Screen_ਸਤਰਾਂ[off] == 0)
	ਵਾਪਸ OK;

    ਜੇ (enc_utf8 && Screen_ਸਤਰਾਂ_UC[off] != 0)
	// Draw UTF-8 multi-byte character.
	ਵਾਪਸ gui_outstr_nowrap(buf, utfc_char2bytes(off, buf),
							 flags, fg, bg, ਪਿੱਠ);

    ਜੇ (enc_dbcs == DBCS_JPNU && Screen_ਸਤਰਾਂ[off] == 0x8e)
    {
	buf[0] = Screen_ਸਤਰਾਂ[off];
	buf[1] = ScreenLines2[off];
	ਵਾਪਸ gui_outstr_nowrap(buf, 2, flags, fg, bg, ਪਿੱਠ);
    }

    // Draw non-multi-byte character or DBCS character.
    ਵਾਪਸ gui_outstr_nowrap(Screen_ਸਤਰਾਂ + off,
	    enc_dbcs ? (*mb_ptr2len)(Screen_ਸਤਰਾਂ + off) : 1,
							 flags, fg, bg, ਪਿੱਠ);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
/*
 * Output the string at the given screen position.  This is used in place
 * of gui_screenchar() where possible because Pango needs as much context
 * as possible to work nicely.  It's a lot faster as well.
 */
    ਸਥਿਰ ਪੂਰਨ
gui_screenstr(
    ਪੂਰਨ		off,	    // Offset from start of screen
    ਪੂਰਨ		len,	    // string length in screen cells
    ਪੂਰਨ		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    ਪੂਰਨ		ਪਿੱਠ)	    // backup this many chars when using bold trick
{
    char_u  *buf;
    ਪੂਰਨ	    outlen = 0;
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    retval;

    ਜੇ (len <= 0) // "cannot happen"?
	ਵਾਪਸ OK;

    ਜੇ (enc_utf8)
    {
	buf = alloc(len * MB_MAXBYTES + 1);
	ਜੇ (buf == ਨਿਰਾ)
	    ਵਾਪਸ OK; // not much we could do here...

	ਲਈ (i = off; i < off + len; ++i)
	{
	    ਜੇ (Screen_ਸਤਰਾਂ[i] == 0)
		ਜਾਰੀ; // skip second half of double-width char

	    ਜੇ (Screen_ਸਤਰਾਂ_UC[i] == 0)
		buf[outlen++] = Screen_ਸਤਰਾਂ[i];
	    ਨਹੀਂਤਾਂ
		outlen += utfc_char2bytes(i, buf + outlen);
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, ਪਿੱਠ);
	vim_free(buf);

	ਵਾਪਸ retval;
    }
    ਨਹੀਂਤਾਂ ਜੇ (enc_dbcs == DBCS_JPNU)
    {
	buf = alloc(len * 2 + 1);
	ਜੇ (buf == ਨਿਰਾ)
	    ਵਾਪਸ OK; // not much we could do here...

	ਲਈ (i = off; i < off + len; ++i)
	{
	    buf[outlen++] = Screen_ਸਤਰਾਂ[i];

	    // handle double-byte single-width char
	    ਜੇ (Screen_ਸਤਰਾਂ[i] == 0x8e)
		buf[outlen++] = ScreenLines2[i];
	    ਨਹੀਂਤਾਂ ਜੇ (MB_BYTE2LEN(Screen_ਸਤਰਾਂ[i]) == 2)
		buf[outlen++] = Screen_ਸਤਰਾਂ[++i];
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, ਪਿੱਠ);
	vim_free(buf);

	ਵਾਪਸ retval;
    }
    ਨਹੀਂਤਾਂ
    {
	ਵਾਪਸ gui_outstr_nowrap(&Screen_ਸਤਰਾਂ[off], len,
				 flags, fg, bg, ਪਿੱਠ);
    }
}
#ਜੇ_ਅੰਤ // FEAT_GUI_GTK

/*
 * Output the given string at the current cursor position.  If the string is
 * too long to fit on the line, then it is truncated.
 * "flags":
 * GUI_MON_IS_CURSOR should only be used when this function is being called to
 * actually draw (an inverted) cursor.
 * GUI_MON_TRS_CURSOR is used to draw the cursor text with a transparent
 * background.
 * GUI_MON_NOCLEAR is used to avoid clearing the selection when drawing over
 * it.
 * Returns OK, unless "back" is non-zero and using the bold trick, then return
 * FAIL (the caller should start drawing "back" chars back).
 */
    ਸਥਿਰ ਪੂਰਨ
gui_outstr_nowrap(
    char_u	*s,
    ਪੂਰਨ		len,
    ਪੂਰਨ		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    ਪੂਰਨ		ਪਿੱਠ)	    // backup this many chars when using bold trick
{
    long_u	highlight_ਨਕਾਬ;
    long_u	hl_ਨਕਾਬ_todo;
    guicolor_T	fg_color;
    guicolor_T	bg_color;
    guicolor_T	sp_color;
#ਜੇ !defined(FEAT_GUI_GTK)
    GuiFont	font = NOFONT;
    GuiFont	wide_font = NOFONT;
# ifdef FEAT_XFONTSET
    GuiFontset	fontset = NOFONTSET;
# endif
#ਜੇ_ਅੰਤ
    attrentry_T	*aep = ਨਿਰਾ;
    ਪੂਰਨ		draw_flags;
    ਪੂਰਨ		ਖਾਨਾ = gui.ਖਾਨਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGN_ICONS
    ਪੂਰਨ		draw_sign = FALSE;
    ਪੂਰਨ		signcol = 0;
    char_u	extra[18];
# ifdef FEAT_NETBEANS_INTG
    ਪੂਰਨ		multi_sign = FALSE;
# endif
#ਜੇ_ਅੰਤ

    ਜੇ (len < 0)
	len = (ਪੂਰਨ)STRLEN(s);
    ਜੇ (len == 0)
	ਵਾਪਸ OK;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGN_ICONS
    ਜੇ (*s == SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
	  || *s == MULTISIGN_BYTE
# endif
       )
    {
	size_t	slen = 2;		    // 2 spaces by default

# ifdef FEAT_NETBEANS_INTG
	ਜੇ (*s == MULTISIGN_BYTE)
	    multi_sign = TRUE;
# endif
	// draw spaces instead
	ਜੇ (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u' &&
		(curwin->w_p_nu || curwin->w_p_rnu))
	{
	    ਪੂਰਨ	n = ਸੰਖਿਆ_ਚੌੜਾਈ(curwin);

	    slen = MIN(n, (ਪੂਰਨ)ਆਕਾਰ(extra) - 1);
	}
	vim_memset(extra, ' ', slen);
	extra[slen] = NUL;
	s = extra;
	ਜੇ (len == 1 && ਖਾਨਾ > 0)
	    --ਖਾਨਾ;
	len = (ਪੂਰਨ)slen;
	ਜੇ (len > 2)
	    // right align sign icon in the number column
	    signcol = ਖਾਨਾ + len - 3;
	ਨਹੀਂਤਾਂ
	    signcol = ਖਾਨਾ;
	draw_sign = TRUE;
	highlight_ਨਕਾਬ = 0;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ਜੇ (gui.highlight_ਨਕਾਬ > HL_ALL)
    {
	aep = syn_gui_attr2entry(gui.highlight_ਨਕਾਬ);
	ਜੇ (aep == ਨਿਰਾ)	    // highlighting not set
	    highlight_ਨਕਾਬ = 0;
	ਨਹੀਂਤਾਂ
	    highlight_ਨਕਾਬ = aep->ae_attr;
    }
    ਨਹੀਂਤਾਂ
	highlight_ਨਕਾਬ = gui.highlight_ਨਕਾਬ;
    hl_ਨਕਾਬ_todo = highlight_ਨਕਾਬ;

#ਜੇ !defined(FEAT_GUI_GTK)
    // Set the font
    ਜੇ (aep != ਨਿਰਾ && aep->ae_u.gui.font != NOFONT)
	font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
    ਨਹੀਂਤਾਂ ਜੇ (aep != ਨਿਰਾ && aep->ae_u.gui.fontset != NOFONTSET)
	fontset = aep->ae_u.gui.fontset;
# endif
    ਨਹੀਂਤਾਂ
    {
# ifdef FEAT_XFONTSET
	ਜੇ (gui.fontset != NOFONTSET)
	    fontset = gui.fontset;
	ਨਹੀਂਤਾਂ
# endif
	    ਜੇ (hl_ਨਕਾਬ_todo & (HL_BOLD | HL_STANDOUT))
	{
	    ਜੇ ((hl_ਨਕਾਬ_todo & HL_ITALIC) && gui.boldital_font != NOFONT)
	    {
		font = gui.boldital_font;
		hl_ਨਕਾਬ_todo &= ~(HL_BOLD | HL_STANDOUT | HL_ITALIC);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (gui.bold_font != NOFONT)
	    {
		font = gui.bold_font;
		hl_ਨਕਾਬ_todo &= ~(HL_BOLD | HL_STANDOUT);
	    }
	    ਨਹੀਂਤਾਂ
		font = gui.norm_font;
	}
	ਨਹੀਂਤਾਂ ਜੇ ((hl_ਨਕਾਬ_todo & HL_ITALIC) && gui.ital_font != NOFONT)
	{
	    font = gui.ital_font;
	    hl_ਨਕਾਬ_todo &= ~HL_ITALIC;
	}
	ਨਹੀਂਤਾਂ
	    font = gui.norm_font;

	/*
	 * Choose correct wide_font by font.  wide_font should be set with font
	 * at same time in above block.  But it will make many "ifdef" nasty
	 * blocks.  So we do it here.
	 */
	ਜੇ (font == gui.boldital_font && gui.wide_boldital_font)
	    wide_font = gui.wide_boldital_font;
	ਨਹੀਂਤਾਂ ਜੇ (font == gui.bold_font && gui.wide_bold_font)
	    wide_font = gui.wide_bold_font;
	ਨਹੀਂਤਾਂ ਜੇ (font == gui.ital_font && gui.wide_ital_font)
	    wide_font = gui.wide_ital_font;
	ਨਹੀਂਤਾਂ ਜੇ (font == gui.norm_font && gui.wide_font)
	    wide_font = gui.wide_font;
    }
# ifdef FEAT_XFONTSET
    ਜੇ (fontset != NOFONTSET)
	gui_mch_ਸੈੱਟ_fontset(fontset);
    ਨਹੀਂਤਾਂ
# endif
	gui_mch_ਸੈੱਟ_font(font);
#ਜੇ_ਅੰਤ

    draw_flags = 0;

    // Set the color
    bg_color = gui.ਪਿੱਠ_pixel;
    ਜੇ ((flags & GUI_MON_IS_CURSOR) && gui.ਵਿੱਚ_focus)
    {
	draw_flags |= DRAW_CURSOR;
	fg_color = fg;
	bg_color = bg;
	sp_color = fg;
    }
    ਨਹੀਂਤਾਂ ਜੇ (aep != ਨਿਰਾ)
    {
	fg_color = aep->ae_u.gui.fg_color;
	ਜੇ (fg_color == INVALCOLOR)
	    fg_color = gui.norm_pixel;
	bg_color = aep->ae_u.gui.bg_color;
	ਜੇ (bg_color == INVALCOLOR)
	    bg_color = gui.ਪਿੱਠ_pixel;
	sp_color = aep->ae_u.gui.sp_color;
	ਜੇ (sp_color == INVALCOLOR)
	    sp_color = fg_color;
    }
    ਨਹੀਂਤਾਂ
    {
	fg_color = gui.norm_pixel;
	sp_color = fg_color;
    }

    ਜੇ (highlight_ਨਕਾਬ & (HL_INVERSE | HL_STANDOUT))
    {
	gui_mch_ਸੈੱਟ_fg_color(bg_color);
	gui_mch_ਸੈੱਟ_bg_color(fg_color);
    }
    ਨਹੀਂਤਾਂ
    {
	gui_mch_ਸੈੱਟ_fg_color(fg_color);
	gui_mch_ਸੈੱਟ_bg_color(bg_color);
    }
    gui_mch_ਸੈੱਟ_sp_color(sp_color);

    // Clear the selection if we are about to write over it
    ਜੇ (!(flags & GUI_MON_NOCLEAR))
	clip_may_clear_selection(gui.ਪੰਕਤੀ, gui.ਪੰਕਤੀ);


    // If there's no bold font, then fake it
    ਜੇ (hl_ਨਕਾਬ_todo & (HL_BOLD | HL_STANDOUT))
	draw_flags |= DRAW_BOLD;

    /*
     * When drawing bold or italic characters the spill-over from the left
     * neighbor may be destroyed.  Let the caller backup to start redrawing
     * just after a blank.
     */
    ਜੇ (ਪਿੱਠ != 0 && ((draw_flags & DRAW_BOLD) || (highlight_ਨਕਾਬ & HL_ITALIC)))
	ਵਾਪਸ FAIL;

#ਜੇ defined(FEAT_GUI_GTK)
    // If there's no italic font, then fake it.
    // For GTK2, we don't need a different font for italic style.
    ਜੇ (hl_ਨਕਾਬ_todo & HL_ITALIC)
	draw_flags |= DRAW_ITALIC;

    // Do we underline the text?
    ਜੇ (hl_ਨਕਾਬ_todo & HL_UNDERLINE)
	draw_flags |= DRAW_UNDERL;

#ਨਹੀਂਤਾਂ
    // Do we underline the text?
    ਜੇ ((hl_ਨਕਾਬ_todo & HL_UNDERLINE) || (hl_ਨਕਾਬ_todo & HL_ITALIC))
	draw_flags |= DRAW_UNDERL;
#ਜੇ_ਅੰਤ
    // Do we undercurl the text?
    ਜੇ (hl_ਨਕਾਬ_todo & HL_UNDERCURL)
	draw_flags |= DRAW_UNDERC;

    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

    // Do we strikethrough the text?
    ਜੇ (hl_ਨਕਾਬ_todo & HL_STRIKETHROUGH)
	draw_flags |= DRAW_STRIKE;

    // Do we draw transparently?
    ਜੇ (flags & GUI_MON_TRS_CURSOR)
	draw_flags |= DRAW_TRANSP;

    /*
     * Draw the text.
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
    // The value returned is the length in display cells
    len = gui_gtk_draw_string(gui.ਪੰਕਤੀ, ਖਾਨਾ, s, len, draw_flags);
#ਨਹੀਂਤਾਂ
    ਜੇ (enc_utf8)
    {
	ਪੂਰਨ	ਸ਼ੁਰੂ;		// index of bytes to be drawn
	ਪੂਰਨ	cells;		// cellwidth of bytes to be drawn
	ਪੂਰਨ	thislen;	// length of bytes to be drawn
	ਪੂਰਨ	cn;		// cellwidth of current char
	ਪੂਰਨ	i;		// index of current char
	ਪੂਰਨ	c;		// current char value
	ਪੂਰਨ	cl;		// byte length of current char
	ਪੂਰਨ	comping;	// current char is composing
	ਪੂਰਨ	scol = ਖਾਨਾ;	// screen column
	ਪੂਰਨ	curr_wide = FALSE;  // use 'guifontwide'
	ਪੂਰਨ	prev_wide = FALSE;
	ਪੂਰਨ	wide_ਬਦਲਿਆ;
# ifdef MSWIN
	ਪੂਰਨ	sep_comp = FALSE;   // Don't separate composing chars.
# ਨਹੀਂਤਾਂ
	ਪੂਰਨ	sep_comp = TRUE;    // Separate composing chars.
# endif

	// Break the string at a composing character, it has to be drawn on
	// top of the previous character.
	ਸ਼ੁਰੂ = 0;
	cells = 0;
	ਲਈ (i = 0; i < len; i += cl)
	{
	    c = utf_ptr2char(s + i);
	    cn = utf_char2cells(c);
	    comping = utf_iscomposing(c);
	    ਜੇ (!comping)	// count cells from non-composing chars
		cells += cn;
	    ਜੇ (!comping || sep_comp)
	    {
		ਜੇ (cn > 1
# ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
# endif
			&& wide_font != NOFONT)
		    curr_wide = TRUE;
		ਨਹੀਂਤਾਂ
		    curr_wide = FALSE;
	    }
	    cl = utf_ptr2len(s + i);
	    ਜੇ (cl == 0)	// hit end of string
		len = i + cl;	// len must be wrong "cannot happen"

	    wide_ਬਦਲਿਆ = curr_wide != prev_wide;

	    // Print the string so far if it's the last character or there is
	    // a composing character.
	    ਜੇ (i + cl >= len || (comping && sep_comp && i > ਸ਼ੁਰੂ)
		    || wide_ਬਦਲਿਆ
# ਜੇ defined(FEAT_GUI_X11)
		    || (cn > 1
#  ifdef FEAT_XFONTSET
			// No fontset: At least draw char after wide char at
			// right position.
			&& fontset == NOFONTSET
#  endif
		       )
# endif
	       )
	    {
		ਜੇ ((comping && sep_comp) || wide_ਬਦਲਿਆ)
		    thislen = i - ਸ਼ੁਰੂ;
		ਨਹੀਂਤਾਂ
		    thislen = i - ਸ਼ੁਰੂ + cl;
		ਜੇ (thislen > 0)
		{
		    ਜੇ (prev_wide)
			gui_mch_ਸੈੱਟ_font(wide_font);
		    gui_mch_draw_string(gui.ਪੰਕਤੀ, scol, s + ਸ਼ੁਰੂ, thislen,
								  draw_flags);
		    ਜੇ (prev_wide)
			gui_mch_ਸੈੱਟ_font(font);
		    ਸ਼ੁਰੂ += thislen;
		}
		scol += cells;
		cells = 0;
		// Adjust to not draw a character which width is changed
		// against with last one.
		ਜੇ (wide_ਬਦਲਿਆ && !(comping && sep_comp))
		{
		    scol -= cn;
		    cl = 0;
		}

# ਜੇ defined(FEAT_GUI_X11)
		// No fontset: draw a space to fill the gap after a wide char
		//
		ਜੇ (cn > 1 && (draw_flags & DRAW_TRANSP) == 0
#  ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
#  endif
			&& !wide_ਬਦਲਿਆ)
		    gui_mch_draw_string(gui.ਪੰਕਤੀ, scol - 1, (char_u *)" ",
							       1, draw_flags);
# endif
	    }
	    // Draw a composing char on top of the previous char.
	    ਜੇ (comping && sep_comp)
	    {
# ਜੇ defined(__APPLE_CC__) && ਨਿਸ਼ਾਨਾ_API_MAC_CARBON
		// Carbon ATSUI autodraws composing char over previous char
		gui_mch_draw_string(gui.ਪੰਕਤੀ, scol, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# ਨਹੀਂਤਾਂ
		gui_mch_draw_string(gui.ਪੰਕਤੀ, scol - cn, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# endif
		ਸ਼ੁਰੂ = i + cl;
	    }
	    prev_wide = curr_wide;
	}
	// The stuff below assumes "len" is the length in screen columns.
	len = scol - ਖਾਨਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	gui_mch_draw_string(gui.ਪੰਕਤੀ, ਖਾਨਾ, s, len, draw_flags);
	ਜੇ (enc_dbcs == DBCS_JPNU)
	{
	    // Get the length in display cells, this can be different from the
	    // number of bytes for "euc-jp".
	    len = mb_string2cells(s, len);
	}
    }
#ਜੇ_ਅੰਤ // !FEAT_GUI_GTK

    ਜੇ (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
	gui.ਖਾਨਾ = ਖਾਨਾ + len;

    // May need to invert it when it's part of the selection.
    ਜੇ (flags & GUI_MON_NOCLEAR)
	clip_may_redraw_selection(gui.ਪੰਕਤੀ, ਖਾਨਾ, len);

    ਜੇ (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
    {
	// Invalidate the old physical cursor position if we wrote over it
	ਜੇ (gui.cursor_ਪੰਕਤੀ == gui.ਪੰਕਤੀ
		&& gui.cursor_ਖਾਨਾ >= ਖਾਨਾ
		&& gui.cursor_ਖਾਨਾ < ਖਾਨਾ + len)
	    gui.cursor_is_valid = false;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGN_ICONS
    ਜੇ (draw_sign)
	// Draw the sign on top of the spaces.
	gui_mch_drawsign(gui.ਪੰਕਤੀ, signcol, gui.highlight_ਨਕਾਬ);
# ਜੇ defined(FEAT_NETBEANS_INTG) && (defined(FEAT_GUI_X11) \
	|| defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN))
    ਜੇ (multi_sign)
	netbeans_draw_multisign_indicator(gui.ਪੰਕਤੀ);
# endif
#ਜੇ_ਅੰਤ

    ਵਾਪਸ OK;
}

/*
 * Undraw the cursor.  This actually redraws the character at the cursor
 * position, plus some more characters when needed.
 */
    ਖਾਲੀ
gui_undraw_cursor(ਖਾਲੀ)
{
    ਜੇ (!gui.cursor_is_valid)
	ਵਾਪਸ;

    // Always redraw the character just before if there is one, because
    // with some fonts and characters there can be a one pixel overlap.
    ਪੂਰਨ startcol = gui.cursor_ਖਾਨਾ > 0 ? gui.cursor_ਖਾਨਾ - 1 : gui.cursor_ਖਾਨਾ;
    ਪੂਰਨ endcol = gui.cursor_ਖਾਨਾ;

#ਜੇ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    gui_adjust_undraw_cursor_for_ligatures(&startcol, &endcol);
#ਜੇ_ਅੰਤ
    gui_redraw_ਖੰਡ(gui.cursor_ਪੰਕਤੀ, startcol,
	    gui.cursor_ਪੰਕਤੀ, endcol, GUI_MON_NOCLEAR);

    // Cursor_is_valid is reset when the cursor is undrawn, also reset it
    // here in case it wasn't needed to undraw it.
    gui.cursor_is_valid = false;
}

    ਖਾਲੀ
gui_redraw(
    ਪੂਰਨ		x,
    ਪੂਰਨ		y,
    ਪੂਰਨ		w,
    ਪੂਰਨ		h)
{
    ਪੂਰਨ		row1, col1, row2, col2;

    row1 = Y_2_ਪੰਕਤੀ(y);
    col1 = X_2_ਖਾਨਾ(x);
    row2 = Y_2_ਪੰਕਤੀ(y + h - 1);
    col2 = X_2_ਖਾਨਾ(x + w - 1);

    gui_redraw_ਖੰਡ(row1, col1, row2, col2, GUI_MON_NOCLEAR);

    /*
     * We may need to redraw the cursor, but don't take it upon us to change
     * its location after a scroll.
     * (maybe be more strict even and test col too?)
     * These things may be outside the update/clipping region and reality may
     * not reflect Vims internal ideas if these operations are clipped away.
     */
    ਜੇ (gui.ਪੰਕਤੀ == gui.cursor_ਪੰਕਤੀ)
	gui_update_cursor(TRUE, TRUE);
}

/*
 * Draw a rectangular block of characters, from row1 to row2 (inclusive) and
 * from col1 to col2 (inclusive).
 */
    ਖਾਲੀ
gui_redraw_ਖੰਡ(
    ਪੂਰਨ		row1,
    ਪੂਰਨ		col1,
    ਪੂਰਨ		row2,
    ਪੂਰਨ		col2,
    ਪੂਰਨ		flags)	// flags for gui_outstr_nowrap()
{
    ਪੂਰਨ		old_ਪੰਕਤੀ, old_ਖਾਨਾ;
    long_u	old_hl_ਨਕਾਬ;
    ਪੂਰਨ		off;
    sattr_T	ਪਹਿਲਾ_attr;
    ਪੂਰਨ		idx, len;
    ਪੂਰਨ		ਪਿੱਠ, nback;
    ਪੂਰਨ		orig_col1, orig_col2;

    // Don't try to update when ScreenLines is not valid
    ਜੇ (!screen_cleared || Screen_ਸਤਰਾਂ == ਨਿਰਾ)
	ਵਾਪਸ;

    // Don't try to draw outside the shell!
    // Check everything, strange values may be caused by a big border width
    col1 = check_ਖਾਨਾ(col1);
    col2 = check_ਖਾਨਾ(col2);
    row1 = check_ਪੰਕਤੀ(row1);
    row2 = check_ਪੰਕਤੀ(row2);

    // Remember where our cursor was
    old_ਪੰਕਤੀ = gui.ਪੰਕਤੀ;
    old_ਖਾਨਾ = gui.ਖਾਨਾ;
    old_hl_ਨਕਾਬ = gui.highlight_ਨਕਾਬ;
    orig_col1 = col1;
    orig_col2 = col2;

    ਲਈ (gui.ਪੰਕਤੀ = row1; gui.ਪੰਕਤੀ <= row2; gui.ਪੰਕਤੀ++)
    {
	// When only half of a double-wide character is in the block, include
	// the other half.
	col1 = orig_col1;
	col2 = orig_col2;
	off = ਸਤਰ_ਵਿੱਥ[gui.ਪੰਕਤੀ];
	ਜੇ (enc_dbcs != 0)
	{
	    ਜੇ (col1 > 0)
		col1 -= dbcs_screen_ਸਿਰ_off(Screen_ਸਤਰਾਂ + off,
						    Screen_ਸਤਰਾਂ + off + col1);
	    col2 += dbcs_screen_ਪੂਛ_off(Screen_ਸਤਰਾਂ + off,
						    Screen_ਸਤਰਾਂ + off + col2);
	}
	ਨਹੀਂਤਾਂ ਜੇ (enc_utf8)
	{
	    ਜੇ (Screen_ਸਤਰਾਂ[off + col1] == 0)
	    {
		ਜੇ (col1 > 0)
		    --col1;
		ਨਹੀਂਤਾਂ
		    // FIXME: how can the first character ever be zero?
		    siemsg("NUL in ScreenLines in row %ld", (ਲੰਮਾ)gui.ਪੰਕਤੀ);
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
	    ਜੇ (col2 + 1 < Columns && Screen_ਸਤਰਾਂ[off + col2 + 1] == 0)
		++col2;
#ਜੇ_ਅੰਤ
	}
	gui.ਖਾਨਾ = col1;
	off = ਸਤਰ_ਵਿੱਥ[gui.ਪੰਕਤੀ] + gui.ਖਾਨਾ;
	len = col2 - col1 + 1;

	// Find how many chars back this highlighting starts, or where a space
	// is.  Needed for when the bold trick is used
	ਲਈ (ਪਿੱਠ = 0; ਪਿੱਠ < col1; ++ਪਿੱਠ)
	    ਜੇ (ScreenAttrs[off - 1 - ਪਿੱਠ] != ScreenAttrs[off]
		    || Screen_ਸਤਰਾਂ[off - 1 - ਪਿੱਠ] == ' ')
		ਰੋਕੋ;

	// Break it up in strings of characters with the same attributes.
	// Print UTF-8 characters individually.
	ਜਦੋਂ (len > 0)
	{
	    ਪਹਿਲਾ_attr = ScreenAttrs[off];
	    gui.highlight_ਨਕਾਬ = ਪਹਿਲਾ_attr;
#ਜੇ !defined(FEAT_GUI_GTK)
	    ਜੇ (enc_utf8 && Screen_ਸਤਰਾਂ_UC[off] != 0)
	    {
		// output multi-byte character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, ਪਿੱਠ);
		ਜੇ (gui.ਖਾਨਾ < Columns && Screen_ਸਤਰਾਂ[off + 1] == 0)
		    idx = 2;
		ਨਹੀਂਤਾਂ
		    idx = 1;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (enc_dbcs == DBCS_JPNU && Screen_ਸਤਰਾਂ[off] == 0x8e)
	    {
		// output double-byte, single-width character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, ਪਿੱਠ);
		idx = 1;
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
		ਲਈ (idx = 0; idx < len; ++idx)
		{
		    ਜੇ (enc_utf8 && Screen_ਸਤਰਾਂ[off + idx] == 0)
			ਜਾਰੀ; // skip second half of double-width char
		    ਜੇ (ScreenAttrs[off + idx] != ਪਹਿਲਾ_attr)
			ਰੋਕੋ;
		}
		// gui_screenstr() takes care of multibyte chars
		nback = gui_screenstr(off, idx, flags,
				      (guicolor_T)0, (guicolor_T)0, ਪਿੱਠ);
#ਨਹੀਂਤਾਂ
		ਲਈ (idx = 0; idx < len && ScreenAttrs[off + idx] == ਪਹਿਲਾ_attr;
									idx++)
		{
		    // Stop at a multi-byte Unicode character.
		    ਜੇ (enc_utf8 && Screen_ਸਤਰਾਂ_UC[off + idx] != 0)
			ਰੋਕੋ;
		    ਜੇ (enc_dbcs == DBCS_JPNU)
		    {
			// Stop at a double-byte single-width char.
			ਜੇ (Screen_ਸਤਰਾਂ[off + idx] == 0x8e)
			    ਰੋਕੋ;
			ਜੇ (len > 1 && (*mb_ptr2len)(Screen_ਸਤਰਾਂ
							    + off + idx) == 2)
			    ++idx;  // skip second byte of double-byte char
		    }
		}
		nback = gui_outstr_nowrap(Screen_ਸਤਰਾਂ + off, idx, flags,
					  (guicolor_T)0, (guicolor_T)0, ਪਿੱਠ);
#ਜੇ_ਅੰਤ
	    }
	    ਜੇ (nback == FAIL)
	    {
		// Must back up to start drawing where a bold or italic word
		// starts.
		off -= ਪਿੱਠ;
		len += ਪਿੱਠ;
		gui.ਖਾਨਾ -= ਪਿੱਠ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		off += idx;
		len -= idx;
	    }
	    ਪਿੱਠ = 0;
	}
    }

    // Put the cursor back where it was
    gui.ਪੰਕਤੀ = old_ਪੰਕਤੀ;
    gui.ਖਾਨਾ = old_ਖਾਨਾ;
    gui.highlight_ਨਕਾਬ = (ਪੂਰਨ)old_hl_ਨਕਾਬ;
}

    ਸਥਿਰ ਖਾਲੀ
gui_delete_ਸਤਰਾਂ(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    ਜੇ (ਗਿਣਤੀ_2 <= 0)
	ਵਾਪਸ;

    ਜੇ (ਪੰਕਤੀ + ਗਿਣਤੀ_2 > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_ਖੰਡ(ਪੰਕਤੀ, gui.scroll_region_ਖੱਬਾ,
			      gui.scroll_region_bot, gui.scroll_region_ਸੱਜਾ);
    ਨਹੀਂਤਾਂ
    {
	gui_mch_delete_ਸਤਰਾਂ(ਪੰਕਤੀ, ਗਿਣਤੀ_2);

	// If the cursor was in the deleted lines it's now gone.  If the
	// cursor was in the scrolled lines adjust its position.
	ਜੇ (gui.cursor_ਪੰਕਤੀ >= ਪੰਕਤੀ
		&& gui.cursor_ਖਾਨਾ >= gui.scroll_region_ਖੱਬਾ
		&& gui.cursor_ਖਾਨਾ <= gui.scroll_region_ਸੱਜਾ)
	{
	    ਜੇ (gui.cursor_ਪੰਕਤੀ < ਪੰਕਤੀ + ਗਿਣਤੀ_2)
		gui.cursor_is_valid = false;
	    ਨਹੀਂਤਾਂ ਜੇ (gui.cursor_ਪੰਕਤੀ <= gui.scroll_region_bot)
		gui.cursor_ਪੰਕਤੀ -= ਗਿਣਤੀ_2;
	}
    }
}

    ਸਥਿਰ ਖਾਲੀ
gui_ਪਾਓ_ਸਤਰਾਂ(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    ਜੇ (ਗਿਣਤੀ_2 <= 0)
	ਵਾਪਸ;

    ਜੇ (ਪੰਕਤੀ + ਗਿਣਤੀ_2 > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_ਖੰਡ(ਪੰਕਤੀ, gui.scroll_region_ਖੱਬਾ,
			      gui.scroll_region_bot, gui.scroll_region_ਸੱਜਾ);
    ਨਹੀਂਤਾਂ
    {
	gui_mch_ਪਾਓ_ਸਤਰਾਂ(ਪੰਕਤੀ, ਗਿਣਤੀ_2);

	ਜੇ (gui.cursor_ਪੰਕਤੀ >= gui.ਪੰਕਤੀ
		&& gui.cursor_ਖਾਨਾ >= gui.scroll_region_ਖੱਬਾ
		&& gui.cursor_ਖਾਨਾ <= gui.scroll_region_ਸੱਜਾ)
	{
	    ਜੇ (gui.cursor_ਪੰਕਤੀ <= gui.scroll_region_bot - ਗਿਣਤੀ_2)
		gui.cursor_ਪੰਕਤੀ += ਗਿਣਤੀ_2;
	    ਨਹੀਂਤਾਂ ਜੇ (gui.cursor_ਪੰਕਤੀ <= gui.scroll_region_bot)
		gui.cursor_is_valid = false;
	}
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
/*
 * Passed to ui_wait_for_chars_or_timer(), ignoring extra arguments.
 */
    ਸਥਿਰ ਪੂਰਨ
gui_wait_for_chars_3(
    ਲੰਮਾ wtime,
    ਪੂਰਨ *interrupted ਅਣਵਰਤਿਆ,
    ਪੂਰਨ ignore_ਨਿਵੇਸ਼ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ gui_mch_wait_for_chars(wtime);
}
#ਜੇ_ਅੰਤ

/*
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
gui_wait_for_chars_or_timer(
	ਲੰਮਾ wtime,
	ਪੂਰਨ *interrupted ਅਣਵਰਤਿਆ,
	ਪੂਰਨ ignore_ਨਿਵੇਸ਼ ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
    ਵਾਪਸ ui_wait_for_chars_or_timer(wtime, gui_wait_for_chars_3,
						    interrupted, ignore_ਨਿਵੇਸ਼);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ gui_mch_wait_for_chars(wtime);
#ਜੇ_ਅੰਤ
}

/*
 * The main GUI input routine.	Waits for a character from the keyboard.
 * "wtime" == -1    Wait forever.
 * "wtime" == 0	    Don't wait.
 * "wtime" > 0	    Wait wtime milliseconds for a character.
 *
 * Returns the number of characters read or zero when timed out or interrupted.
 * "buf" may be NULL, in which case a non-zero number is returned if characters
 * are available.
 */
    ਸਥਿਰ ਪੂਰਨ
gui_wait_for_chars_buf(
    char_u	*buf,
    ਪੂਰਨ		maxlen,
    ਲੰਮਾ	wtime,	    // don't use "time", MIPS cannot handle it
    ਪੂਰਨ		tb_change_cnt)
{
    ਪੂਰਨ	    retval;
    ਪੂਰਨ	    keep_blinking; // Guard against restarting blink cycle on CursorHold

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    // If we're going to wait a bit, update the menus and mouse shape for the
    // current State.
    ਜੇ (wtime != 0)
	gui_update_menus(0);
#ਜੇ_ਅੰਤ

    gui_mch_update();
    ਜੇ (ਨਿਵੇਸ਼_ਉਪਲਬਧ())	// Got char, return immediately
    {
	ਜੇ (gui_mch_is_blinking())
	    gui_mch_stop_blink(TRUE);
	ਜੇ (buf != ਨਿਰਾ && !typebuf_ਬਦਲਿਆ(tb_change_cnt))
	    ਵਾਪਸ read_from_ਨਿਵੇਸ਼_buf(buf, (ਲੰਮਾ)maxlen);
	ਵਾਪਸ 0;
    }
    ਜੇ (wtime == 0)		// Don't wait for char
	ਵਾਪਸ FAIL;

    // Before waiting, flush any output to the screen.
    gui_mch_flush();

    // Blink while waiting for a character.
    ਜੇ (!gui_mch_is_blinking())
	gui_mch_ਸ਼ੁਰੂ_blink();

    // Common function to loop until "wtime" is met, while handling timers and
    // other callbacks.
    retval = inchar_loop(buf, maxlen, wtime, tb_change_cnt,
			 gui_wait_for_chars_or_timer, ਨਿਰਾ);

    // Keep blinking when CursorHold wakes the input loop. (See PR #21115)
    keep_blinking = retval == 3 && buf != ਨਿਰਾ
	&& buf[0] == K_SPECIAL && buf[1] == KS_EXTRA
	&& buf[2] == (ਪੂਰਨ)KE_CURSORHOLD;

    ਜੇ (!keep_blinking)
	gui_mch_stop_blink(TRUE);

    ਵਾਪਸ retval;
}

/*
 * Wait for a character from the keyboard without actually reading it.
 * Also deals with timers.
 * wtime == -1	    Wait forever.
 * wtime == 0	    Don't wait.
 * wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    ਪੂਰਨ
gui_wait_for_chars(ਲੰਮਾ wtime, ਪੂਰਨ tb_change_cnt)
{
    ਵਾਪਸ gui_wait_for_chars_buf(ਨਿਰਾ, 0, wtime, tb_change_cnt);
}

/*
 * Equivalent of mch_inchar() for the GUI.
 */
    ਪੂਰਨ
gui_inchar(
    char_u  *buf,
    ਪੂਰਨ	    maxlen,
    ਲੰਮਾ    wtime,		// milliseconds
    ਪੂਰਨ	    tb_change_cnt)
{
    ਵਾਪਸ gui_wait_for_chars_buf(buf, maxlen, wtime, tb_change_cnt);
}

/*
 * Fill p[4] with mouse coordinates encoded for check_termcode().
 */
    ਸਥਿਰ ਖਾਲੀ
fill_mouse_coord(char_u *p, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ ਪੰਕਤੀ)
{
    p[0] = (char_u)(ਖਾਨਾ / 128 + ' ' + 1);
    p[1] = (char_u)(ਖਾਨਾ % 128 + ' ' + 1);
    p[2] = (char_u)(ਪੰਕਤੀ / 128 + ' ' + 1);
    p[3] = (char_u)(ਪੰਕਤੀ % 128 + ' ' + 1);
}

/*
 * Generic mouse support function.  Add a mouse event to the input buffer with
 * the given properties.
 *  button	    --- may be any of MOUSE_LEFT, MOUSE_MIDDLE, MOUSE_RIGHT,
 *			MOUSE_X1, MOUSE_X2
 *			MOUSE_DRAG, or MOUSE_RELEASE.
 *			MOUSE_4 and MOUSE_5 are used for vertical scroll wheel,
 *			MOUSE_6 and MOUSE_7 for horizontal scroll wheel.
 *  x, y	    --- Coordinates of mouse in pixels.
 *  repeated_click  --- TRUE if this click comes only a short time after a
 *			previous click.
 *  modifiers	    --- Bit field which may be any of the following modifiers
 *			or'ed together: MOUSE_SHIFT | MOUSE_CTRL | MOUSE_ALT.
 * This function will ignore drag events where the mouse has not moved to a new
 * character.
 */
    ਖਾਲੀ
gui_send_mouse_event(
    ਪੂਰਨ	    button,
    ਪੂਰਨ	    x,
    ਪੂਰਨ	    y,
    ਪੂਰਨ	    repeated_click,
    int_u   modifiers)
{
    ਸਥਿਰ ਪੂਰਨ	    prev_ਪੰਕਤੀ = 0, prev_ਖਾਨਾ = 0;
    ਸਥਿਰ ਪੂਰਨ	    prev_button = -1;
    ਸਥਿਰ ਪੂਰਨ	    ਅੰਕ_clicks = 1;
    char_u	    string[10];
    ਗਿਣਤੀ ਕੁੰਜੀ_extra  button_char;
    ਪੂਰਨ		    ਪੰਕਤੀ, ਖਾਨਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    ਪੂਰਨ		    checkfor;
    ਪੂਰਨ		    did_clip = FALSE;
#ਜੇ_ਅੰਤ

    /*
     * Scrolling may happen at any time, also while a selection is present.
     */
    ਚੋਣ (button)
    {
	ਹਾਲਤ MOUSE_MOVE:
	    button_char = KE_MOUSEMOVE_XY;
	    ਜਾਓ button_ਸੈੱਟ;
	ਹਾਲਤ MOUSE_X1:
	    button_char = KE_X1MOUSE;
	    ਜਾਓ button_ਸੈੱਟ;
	ਹਾਲਤ MOUSE_X2:
	    button_char = KE_X2MOUSE;
	    ਜਾਓ button_ਸੈੱਟ;
	ਹਾਲਤ MOUSE_4:
	    button_char = KE_MOUSEDOWN;
	    ਜਾਓ button_ਸੈੱਟ;
	ਹਾਲਤ MOUSE_5:
	    button_char = KE_MOUSEUP;
	    ਜਾਓ button_ਸੈੱਟ;
	ਹਾਲਤ MOUSE_6:
	    button_char = KE_MOUSELEFT;
	    ਜਾਓ button_ਸੈੱਟ;
	ਹਾਲਤ MOUSE_7:
	    button_char = KE_MOUSERIGHT;
button_ਸੈੱਟ:
	    {
		// Don't put events in the input queue now.
		ਜੇ (hold_gui_events)
		    ਵਾਪਸ;

		ਪੰਕਤੀ = gui_xy2colrow(x, y, &ਖਾਨਾ);
		// Don't report a mouse move unless moved to a
		// different character position.
		ਜੇ (button == MOUSE_MOVE)
		{
		    ਜੇ (ਪੰਕਤੀ == prev_ਪੰਕਤੀ && ਖਾਨਾ == prev_ਖਾਨਾ)
			ਵਾਪਸ;
		    ਨਹੀਂਤਾਂ
		    {
			prev_ਪੰਕਤੀ = ਪੰਕਤੀ >= 0 ? ਪੰਕਤੀ : 0;
			prev_ਖਾਨਾ = ਖਾਨਾ;
		    }
		}

		string[3] = CSI;
		string[4] = KS_EXTRA;
		string[5] = (ਪੂਰਨ)button_char;

		// Pass the pointer coordinates of the scroll event so that we
		// know which window to scroll.
		string[6] = (char_u)(ਖਾਨਾ / 128 + ' ' + 1);
		string[7] = (char_u)(ਖਾਨਾ % 128 + ' ' + 1);
		string[8] = (char_u)(ਪੰਕਤੀ / 128 + ' ' + 1);
		string[9] = (char_u)(ਪੰਕਤੀ % 128 + ' ' + 1);

		ਜੇ (modifiers == 0)
		    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string + 3, 7);
		ਨਹੀਂਤਾਂ
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = 0;
		    ਜੇ (modifiers & MOUSE_ਖਿਸਕਾਓ)
			string[2] |= MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;
		    ਜੇ (modifiers & MOUSE_CTRL)
			string[2] |= MOD_ਨਕਾਬ_CTRL;
		    ਜੇ (modifiers & MOUSE_ALT)
			string[2] |= MOD_ਨਕਾਬ_ALT;
		    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, 10);
		}
		ਵਾਪਸ;
	    }
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    // If a clipboard selection is in progress, handle it
    ਜੇ (clip_ਤਾਰਾ.state == SELECT_ਵਿੱਚ_ਤਰੱਕੀ)
    {
	clip_process_selection(button, X_2_ਖਾਨਾ(x), Y_2_ਪੰਕਤੀ(y), repeated_click);

	// A release event may still need to be sent if the position is equal.
	ਪੰਕਤੀ = gui_xy2colrow(x, y, &ਖਾਨਾ);
	ਜੇ (button != MOUSE_RELEASE || ਪੰਕਤੀ != prev_ਪੰਕਤੀ || ਖਾਨਾ != prev_ਖਾਨਾ)
	    ਵਾਪਸ;
    }

    // Determine which mouse settings to look for based on the current mode
    ਚੋਣ (get_real_state())
    {
	ਹਾਲਤ ਬਹੁਲਕ_ਸਾਧਾਰਨ_BUSY:
	ਹਾਲਤ ਬਹੁਲਕ_OP_PENDING:
# ifdef FEAT_TERMINAL
	ਹਾਲਤ ਬਹੁਲਕ_TERMINAL:
# endif
	ਹਾਲਤ ਬਹੁਲਕ_ਸਾਧਾਰਨ:	checkfor = MOUSE_ਸਾਧਾਰਨ; ਰੋਕੋ;
	ਹਾਲਤ ਬਹੁਲਕ_VISUAL:	checkfor = MOUSE_VISUAL; ਰੋਕੋ;
	ਹਾਲਤ ਬਹੁਲਕ_SELECT:	checkfor = MOUSE_VISUAL; ਰੋਕੋ;
	ਹਾਲਤ ਬਹੁਲਕ_REPLACE:
	ਹਾਲਤ ਬਹੁਲਕ_REPLACE | ਬਹੁਲਕ_LANGMAP:
	ਹਾਲਤ ਬਹੁਲਕ_VREPLACE:
	ਹਾਲਤ ਬਹੁਲਕ_VREPLACE | ਬਹੁਲਕ_LANGMAP:
	ਹਾਲਤ ਬਹੁਲਕ_ਪਾਓ:
	ਹਾਲਤ ਬਹੁਲਕ_ਪਾਓ | ਬਹੁਲਕ_LANGMAP:
				checkfor = MOUSE_ਪਾਓ; ਰੋਕੋ;
	ਹਾਲਤ ਬਹੁਲਕ_ASKMORE:
	ਹਾਲਤ ਬਹੁਲਕ_HITRETURN:	// At the more- and hit-enter prompt pass the
				// mouse event for a click on or below the
				// message line.
				ਜੇ (Y_2_ਪੰਕਤੀ(y) >= ਸੁਨੇਹਾ_ਪੰਕਤੀ)
				    checkfor = MOUSE_ਸਾਧਾਰਨ;
				ਨਹੀਂਤਾਂ
				    checkfor = MOUSE_RETURN;
				ਰੋਕੋ;

	    /*
	     * On the command line, use the clipboard selection on all lines
	     * but the command line.  But not when pasting.
	     */
	ਹਾਲਤ ਬਹੁਲਕ_CMDLINE:
	ਹਾਲਤ ਬਹੁਲਕ_CMDLINE | ਬਹੁਲਕ_LANGMAP:
	    ਜੇ (Y_2_ਪੰਕਤੀ(y) < cmdline_ਪੰਕਤੀ && button != MOUSE_ਵਿਚਕਾਰ)
		checkfor = MOUSE_NONE;
	    ਨਹੀਂਤਾਂ
		checkfor = MOUSE_COMMAND;
	    ਰੋਕੋ;

	ਮੂਲ:
	    checkfor = MOUSE_NONE;
	    ਰੋਕੋ;
    };

    /*
     * Allow clipboard selection of text on the command line in "normal"
     * modes.  Don't do this when dragging the status line, or extending a
     * Visual selection.
     */
    ਜੇ ((State == ਬਹੁਲਕ_ਸਾਧਾਰਨ || State == ਬਹੁਲਕ_ਸਾਧਾਰਨ_BUSY
						      || (State & ਬਹੁਲਕ_ਪਾਓ))
	    && X_2_ਖਾਨਾ(x) >= firstwin->w_wincol
	    && X_2_ਖਾਨਾ(x) < firstwin->w_wincol + topframe->fr_ਚੌੜਾਈ
	    && Y_2_ਪੰਕਤੀ(y) >= topframe->fr_ਉਚਾਈ + firstwin->w_winrow
	    && button != MOUSE_DRAG
# ifdef FEAT_MOUSESHAPE
	    && !drag_ਸਥਿਤੀ_ਸਤਰ
	    && !drag_sep_ਸਤਰ
# endif
	    )
	checkfor = MOUSE_NONE;

    /*
     * Use modeless selection when holding CTRL and SHIFT pressed.
     */
    ਜੇ ((modifiers & MOUSE_CTRL) && (modifiers & MOUSE_ਖਿਸਕਾਓ))
	checkfor = MOUSE_NONEF;

    /*
     * In Ex mode, always use modeless selection.
     */
    ਜੇ (exmode_ਸਰਗਰਮ)
	checkfor = MOUSE_NONE;

    /*
     * If the mouse settings say to not use the mouse, use the modeless
     * selection.  But if Visual is active, assume that only the Visual area
     * will be selected.
     * Exception: On the command line, both the selection is used and a mouse
     * key is sent.
     */
    ਜੇ (!mouse_has(checkfor) || checkfor == MOUSE_COMMAND)
    {
	// Don't do modeless selection in Visual mode.
	ਜੇ (checkfor != MOUSE_NONEF && VIsual_ਸਰਗਰਮ && (State & ਬਹੁਲਕ_ਸਾਧਾਰਨ))
	    ਵਾਪਸ;

	/*
	 * When 'mousemodel' is "popup", shift-left is translated to right.
	 * But not when also using Ctrl.
	 */
	ਜੇ (mouse_model_popup() && button == MOUSE_ਖੱਬਾ
		&& (modifiers & MOUSE_ਖਿਸਕਾਓ) && !(modifiers & MOUSE_CTRL))
	{
	    button = MOUSE_ਸੱਜਾ;
	    modifiers &= ~ MOUSE_ਖਿਸਕਾਓ;
	}

	// If the selection is done, allow the right button to extend it.
	// If the selection is cleared, allow the right button to start it
	// from the cursor position.
	ਜੇ (button == MOUSE_ਸੱਜਾ)
	{
	    ਜੇ (clip_ਤਾਰਾ.state == SELECT_CLEARED)
	    {
		ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
		{
		    ਖਾਨਾ = ਸੁਨੇਹਾ_ਖਾਨਾ;
		    ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਖਾਨਾ = curwin->w_wcol;
		    ਪੰਕਤੀ = curwin->w_wrow + W_WINROW(curwin);
		}
		clip_ਸ਼ੁਰੂ_selection(ਖਾਨਾ, ਪੰਕਤੀ, FALSE);
	    }
	    clip_process_selection(button, X_2_ਖਾਨਾ(x), Y_2_ਪੰਕਤੀ(y),
							      repeated_click);
	    did_clip = TRUE;
	}
	// Allow the left button to start the selection
	ਨਹੀਂਤਾਂ ਜੇ (button == MOUSE_ਖੱਬਾ)
	{
	    clip_ਸ਼ੁਰੂ_selection(X_2_ਖਾਨਾ(x), Y_2_ਪੰਕਤੀ(y), repeated_click);
	    did_clip = TRUE;
	}

	// Always allow pasting
	ਜੇ (button != MOUSE_ਵਿਚਕਾਰ)
	{
	    ਜੇ (!mouse_has(checkfor) || button == MOUSE_RELEASE)
		ਵਾਪਸ;
	    ਜੇ (checkfor != MOUSE_COMMAND)
		button = MOUSE_ਖੱਬਾ;
	}
	repeated_click = FALSE;
    }

    ਜੇ (clip_ਤਾਰਾ.state != SELECT_CLEARED && !did_clip)
	clip_clear_selection(&clip_ਤਾਰਾ);
#ਜੇ_ਅੰਤ

    // Don't put events in the input queue now.
    ਜੇ (hold_gui_events)
	ਵਾਪਸ;

    ਪੰਕਤੀ = gui_xy2colrow(x, y, &ਖਾਨਾ);

    /*
     * If we are dragging and the mouse hasn't moved far enough to be on a
     * different character, then don't send an event to vim.
     */
    ਜੇ (button == MOUSE_DRAG)
    {
	ਜੇ (ਪੰਕਤੀ == prev_ਪੰਕਤੀ && ਖਾਨਾ == prev_ਖਾਨਾ)
	    ਵਾਪਸ;
	// Dragging above the window, set "row" to -1 to cause a scroll.
	ਜੇ (y < 0)
	    ਪੰਕਤੀ = -1;
    }

    /*
     * If topline has changed (window scrolled) since the last click, reset
     * repeated_click, because we don't want starting Visual mode when
     * clicking on a different character in the text.
     */
    ਜੇ (curwin->w_topline != gui_prev_topline
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    || curwin->w_topfill != gui_prev_topfill
#ਜੇ_ਅੰਤ
	    )
	repeated_click = FALSE;

    string[0] = CSI;	// this sequence is recognized by check_termcode()
    string[1] = KS_MOUSE;
    string[2] = KE_FILLER;
    ਜੇ (button != MOUSE_DRAG && button != MOUSE_RELEASE)
    {
	ਜੇ (repeated_click)
	{
	    /*
	     * Handle multiple clicks.	They only count if the mouse is still
	     * pointing at the same character.
	     */
	    ਜੇ (button != prev_button || ਪੰਕਤੀ != prev_ਪੰਕਤੀ || ਖਾਨਾ != prev_ਖਾਨਾ)
		ਅੰਕ_clicks = 1;
	    ਨਹੀਂਤਾਂ ਜੇ (++ਅੰਕ_clicks > 4)
		ਅੰਕ_clicks = 1;
	}
	ਨਹੀਂਤਾਂ
	    ਅੰਕ_clicks = 1;
	prev_button = button;
	gui_prev_topline = curwin->w_topline;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	gui_prev_topfill = curwin->w_topfill;
#ਜੇ_ਅੰਤ

	string[3] = (char_u)(button | 0x20);
	ਸੈੱਟ_ਅੰਕ_MOUSE_CLICKS(string[3], ਅੰਕ_clicks);
    }
    ਨਹੀਂਤਾਂ
	string[3] = (char_u)button;

    string[3] |= modifiers;
    fill_mouse_coord(string + 4, ਖਾਨਾ, ਪੰਕਤੀ);
    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, 8);

    ਜੇ (ਪੰਕਤੀ < 0)
	prev_ਪੰਕਤੀ = 0;
    ਨਹੀਂਤਾਂ
	prev_ਪੰਕਤੀ = ਪੰਕਤੀ;
    prev_ਖਾਨਾ = ਖਾਨਾ;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#ਜੇ defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#ਜੇ_ਅੰਤ
}

/*
 * Convert x and y coordinate to column and row in text window.
 * Corrects for multi-byte character.
 * returns column in "*colp" and row as return value;
 */
    ਸਥਿਰ ਪੂਰਨ
gui_xy2colrow(ਪੂਰਨ x, ਪੂਰਨ y, ਪੂਰਨ *colp)
{
    ਪੂਰਨ		ਖਾਨਾ = check_ਖਾਨਾ(X_2_ਖਾਨਾ(x));
    ਪੂਰਨ		ਪੰਕਤੀ = check_ਪੰਕਤੀ(Y_2_ਪੰਕਤੀ(y));

    *colp = mb_fix_ਖਾਨਾ(ਖਾਨਾ, ਪੰਕਤੀ);
    ਵਾਪਸ ਪੰਕਤੀ;
}

#ਜੇ defined(FEAT_MENU)
/*
 * Callback function for when a menu entry has been selected.
 */
    ਖਾਲੀ
gui_menu_cb(vimmenu_T *menu)
{
    char_u  ਬਾਈਟਾਂ[ਆਕਾਰ(long_u)];

    // Don't put events in the input queue now.
    ਜੇ (hold_gui_events)
	ਵਾਪਸ;

    ਬਾਈਟਾਂ[0] = CSI;
    ਬਾਈਟਾਂ[1] = KS_MENU;
    ਬਾਈਟਾਂ[2] = KE_FILLER;
    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(ਬਾਈਟਾਂ, 3);
    ਜੋੜੋ_long_ਮੰਜ਼ਿਲ_buf((long_u)menu, ਬਾਈਟਾਂ);
    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf_csi(ਬਾਈਟਾਂ, ਆਕਾਰ(long_u));
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ	prev_which_scrollbars[3];

/*
 * Set which components are present.
 * If "oldval" is not NULL, "oldval" is the previous value, the new value is
 * in p_go.
 */
    ਖਾਲੀ
gui_init_which_components(char_u *oldval ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_DARKTHEME
    ਸਥਿਰ ਪੂਰਨ	prev_dark_theme = FALSE;
    ਪੂਰਨ		using_dark_theme = FALSE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    ਸਥਿਰ ਪੂਰਨ	prev_menu_is_ਸਰਗਰਮ = -1;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    ਸਥਿਰ ਪੂਰਨ	prev_toolbar = -1;
    ਪੂਰਨ		using_toolbar = FALSE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    ਪੂਰਨ		using_tabline;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
    ਸਥਿਰ ਪੂਰਨ	prev_titlebar = FALSE;
    ਪੂਰਨ		using_titlebar = FALSE;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    ਸਥਿਰ ਪੂਰਨ	prev_fullscreen = FALSE;
    ਪੂਰਨ		using_fullscreen = FALSE;
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_MENU)
    ਸਥਿਰ ਪੂਰਨ	prev_tearoff = -1;
    ਪੂਰਨ		using_tearoff = FALSE;
#ਜੇ_ਅੰਤ

    char_u	*p;
    ਪੂਰਨ		i;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    ਪੂਰਨ		grey_old, grey_new;
    char_u	*ਆਰਜ਼ੀ;
#ਜੇ_ਅੰਤ
    win_T	*wp;
    ਪੂਰਨ		need_ਸੈੱਟ_ਮਾਪ;
    ਪੂਰਨ		fix_ਮਾਪ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    ਜੇ (oldval != ਨਿਰਾ && gui.ਵਿੱਚ_use)
    {
	/*
	 * Check if the menus go from grey to non-grey or vice versa.
	 */
	grey_old = (vim_strchr(oldval, GO_GREY) != ਨਿਰਾ);
	grey_new = (vim_strchr(p_go, GO_GREY) != ਨਿਰਾ);
	ਜੇ (grey_old != grey_new)
	{
	    ਆਰਜ਼ੀ = p_go;
	    p_go = oldval;
	    gui_update_menus(MENU_ALL_MODES);
	    p_go = ਆਰਜ਼ੀ;
	}
    }
    gui.menu_is_ਸਰਗਰਮ = false;
#ਜੇ_ਅੰਤ

    ਲਈ (i = 0; i < 3; i++)
	gui.which_scrollbars[i] = false;
    ਲਈ (p = p_go; *p; p++)
	ਚੋਣ (*p)
	{
	    ਹਾਲਤ GO_ਖੱਬਾ:
		gui.which_scrollbars[SBAR_ਖੱਬਾ] = true;
		ਰੋਕੋ;
	    ਹਾਲਤ GO_ਸੱਜਾ:
		gui.which_scrollbars[SBAR_ਸੱਜਾ] = true;
		ਰੋਕੋ;
	    ਹਾਲਤ GO_VLEFT:
		ਜੇ (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_ਖੱਬਾ] = true;
		ਰੋਕੋ;
	    ਹਾਲਤ GO_VRIGHT:
		ਜੇ (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_ਸੱਜਾ] = true;
		ਰੋਕੋ;
	    ਹਾਲਤ GO_BOT:
		gui.which_scrollbars[SBAR_BOTTOM] = true;
		ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_DARKTHEME
	    ਹਾਲਤ GO_DARKTHEME:
		using_dark_theme = TRUE;
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	    ਹਾਲਤ GO_MENUS:
		gui.menu_is_ਸਰਗਰਮ = true;
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	    ਹਾਲਤ GO_GREY:
		// make menu's have grey items, ignored here
		ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
	    ਹਾਲਤ GO_TITLEBAR:
		using_titlebar = TRUE;
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
	    ਹਾਲਤ GO_FULLSCREEN:
		using_fullscreen = TRUE;
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
	    ਹਾਲਤ GO_TOOLBAR:
		using_toolbar = TRUE;
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	    ਹਾਲਤ GO_TEAROFF:
#ਜੇ defined(FEAT_MENU)
		using_tearoff = TRUE;
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	    ਮੂਲ:
		// Ignore options that are not supported
		ਰੋਕੋ;
	}

    ਜੇ (!gui.ਵਿੱਚ_use)
	ਵਾਪਸ;

    need_ਸੈੱਟ_ਮਾਪ = 0;
    fix_ਮਾਪ = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
    ਜੇ (using_titlebar != prev_titlebar)
    {
	gui_mch_ਸੈੱਟ_titlebar_colors();
	prev_titlebar = using_titlebar;
    }
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    ਜੇ (using_fullscreen != prev_fullscreen)
    {
	gui_mch_ਸੈੱਟ_fullscreen(using_fullscreen);
	prev_fullscreen = using_fullscreen;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_DARKTHEME
    ਜੇ (using_dark_theme != prev_dark_theme)
    {
	gui_mch_ਸੈੱਟ_dark_theme(using_dark_theme);
	prev_dark_theme = using_dark_theme;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    // Update the GUI tab line, it may appear or disappear.  This may
    // cause the non-GUI tab line to disappear or appear.
    using_tabline = gui_has_tabline();
    ਜੇ (!gui_mch_showing_tabline() != !using_tabline)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = ਪੰਕਤੀਆਂ;
	gui_update_tabline();
	ਪੰਕਤੀਆਂ = i;
	need_ਸੈੱਟ_ਮਾਪ |= RESIZE_VERT;
	ਜੇ (using_tabline)
	    fix_ਮਾਪ = TRUE;
	ਜੇ (!gui_use_tabline())
	    redraw_tabline = TRUE;    // may draw non-GUI tab line
    }
#ਜੇ_ਅੰਤ

    ਲਈ (i = 0; i < 3; i++)
    {
	// The scrollbar needs to be updated when it is shown/unshown and
	// when switching tab pages.  But the size only changes when it's
	// shown/unshown.  Thus we need two places to remember whether a
	// scrollbar is there or not.
	ਜੇ (gui.which_scrollbars[i] != prev_which_scrollbars[i]
		|| gui.which_scrollbars[i]
		!= curtab->tp_prev_which_scrollbars[i])
	{
	    ਜੇ (i == SBAR_BOTTOM)
		gui_mch_enable_scrollbar(&gui.bottom_sbar,
			gui.which_scrollbars[i]);
	    ਨਹੀਂਤਾਂ
	    {
		FOR_ALL_WINDOWS(wp)
		    gui_do_scrollbar(wp, i, gui.which_scrollbars[i]);
	    }
	    ਜੇ (gui.which_scrollbars[i] != prev_which_scrollbars[i])
	    {
		ਜੇ (i == SBAR_BOTTOM)
		    need_ਸੈੱਟ_ਮਾਪ |= RESIZE_VERT;
		ਨਹੀਂਤਾਂ
		    need_ਸੈੱਟ_ਮਾਪ |= RESIZE_HOR;
		ਜੇ (gui.which_scrollbars[i])
		    fix_ਮਾਪ = TRUE;
	    }
	}
	curtab->tp_prev_which_scrollbars[i] = gui.which_scrollbars[i];
	prev_which_scrollbars[i] = gui.which_scrollbars[i];
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    ਜੇ (gui.menu_is_ਸਰਗਰਮ != prev_menu_is_ਸਰਗਰਮ)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = ਪੰਕਤੀਆਂ;
	gui_mch_enable_menu(gui.menu_is_ਸਰਗਰਮ);
	ਪੰਕਤੀਆਂ = i;
	prev_menu_is_ਸਰਗਰਮ = gui.menu_is_ਸਰਗਰਮ;
	need_ਸੈੱਟ_ਮਾਪ |= RESIZE_VERT;
	ਜੇ (gui.menu_is_ਸਰਗਰਮ)
	    fix_ਮਾਪ = TRUE;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    ਜੇ (using_toolbar != prev_toolbar)
    {
	gui_mch_show_toolbar(using_toolbar);
	prev_toolbar = using_toolbar;
	need_ਸੈੱਟ_ਮਾਪ |= RESIZE_VERT;
	ਜੇ (using_toolbar)
	    fix_ਮਾਪ = TRUE;
    }
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_MENU) && !(defined(MSWIN) && !defined(FEAT_TEAROFF))
    ਜੇ (using_tearoff != prev_tearoff)
    {
	gui_mch_toggle_tearoffs(using_tearoff);
	prev_tearoff = using_tearoff;
    }
#ਜੇ_ਅੰਤ
    ਜੇ (need_ਸੈੱਟ_ਮਾਪ != 0)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
	ਲੰਮਾ    prev_Columns = Columns;
	ਲੰਮਾ    prev_ਪੰਕਤੀਆਂ = ਪੰਕਤੀਆਂ;
#ਜੇ_ਅੰਤ
	// Adjust the size of the window to make the text area keep the
	// same size and to avoid that part of our window is off-screen
	// and a scrollbar can't be used, for example.
	gui_ਸੈੱਟ_shellsize(FALSE, fix_ਮਾਪ, need_ਸੈੱਟ_ਮਾਪ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
	// GTK has the annoying habit of sending us resize events when
	// changing the window size ourselves.  This mostly happens when
	// waiting for a character to arrive, quite unpredictably, and may
	// change Columns and Rows when we don't want it.  Wait for a
	// character here to avoid this effect.
	// If you remove this, please test this command for resizing
	// effects (with optional left scrollbar): ":vsp|q|vsp|q|vsp|q".
	// Don't do this while starting up though.
	// Don't change Rows when adding menu/toolbar/tabline.
	// Don't change Columns when adding vertical toolbar.
	{
	    // Save the size gui_set_shellsize() determined before
	    // char_avail() processes spurious configure events that may
	    // corrupt Rows/Columns.
	    ਲੰਮਾ    post_ਪੰਕਤੀਆਂ = ਪੰਕਤੀਆਂ;
	    ਲੰਮਾ    post_Columns = Columns;

	    ਜੇ (!gui.starting && need_ਸੈੱਟ_ਮਾਪ != (RESIZE_VERT | RESIZE_HOR))
		(ਖਾਲੀ)char_avail();
	    ਜੇ ((need_ਸੈੱਟ_ਮਾਪ & RESIZE_VERT) == 0)
		ਪੰਕਤੀਆਂ = prev_ਪੰਕਤੀਆਂ;
	    ਨਹੀਂਤਾਂ
		ਪੰਕਤੀਆਂ = post_ਪੰਕਤੀਆਂ;
	    ਜੇ ((need_ਸੈੱਟ_ਮਾਪ & RESIZE_HOR) == 0)
		Columns = prev_Columns;
	    ਨਹੀਂਤਾਂ
		Columns = post_Columns;
	}
#ਜੇ_ਅੰਤ
    }
    // When the console tabline appears or disappears the window positions
    // change.
    ਜੇ (firstwin->w_winrow != tabline_ਉਚਾਈ())
	shell_new_ਪੰਕਤੀਆਂ();	// recompute window positions and heights
}

#ਜੇ defined(FEAT_GUI_TABLINE)
/*
 * Return TRUE if the GUI is taking care of the tabline.
 * It may still be hidden if 'showtabline' is zero.
 */
    ਪੂਰਨ
gui_use_tabline(ਖਾਲੀ)
{
    ਵਾਪਸ gui.ਵਿੱਚ_use && vim_strchr(p_go, GO_TABLINE) != ਨਿਰਾ;
}

/*
 * Return TRUE if the GUI is showing the tabline.
 * This uses 'showtabline'.
 */
    ਸਥਿਰ ਪੂਰਨ
gui_has_tabline(ਖਾਲੀ)
{
    ਜੇ (!gui_use_tabline()
	    || p_stal == 0
	    || (p_stal == 1 && ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ == ਨਿਰਾ))
	ਵਾਪਸ FALSE;
    ਵਾਪਸ TRUE;
}

/*
 * Update the tabline.
 * This may display/undisplay the tabline and update the labels.
 */
    ਖਾਲੀ
gui_update_tabline(ਖਾਲੀ)
{
    ਪੂਰਨ	    showit = gui_has_tabline();
    ਪੂਰਨ	    shown = gui_mch_showing_tabline();

    ਜੇ (!gui.starting && starting == 0)
    {
	// Updating the tabline uses direct GUI commands, flush
	// outstanding instructions first. (esp. clear screen)
	ਬਾਹਰਲਾ_flush();

	ਜੇ (!showit != !shown)
	{
	    // Save Rows/Columns before showing/hiding the tabline.
	    // gui_mch_show_tabline() processes GTK events (via
	    // gui_mch_update) which may trigger a spurious configure event
	    // that temporarily corrupts Rows.  Restore the original values so
	    // that gui_set_shellsize() uses the correct row/column count.
	    ਪੂਰਨ save_ਪੰਕਤੀਆਂ = ਪੰਕਤੀਆਂ;
	    ਪੂਰਨ save_Columns = Columns;

	    gui_mch_show_tabline(showit);
	    ਪੰਕਤੀਆਂ = save_ਪੰਕਤੀਆਂ;
	    Columns = save_Columns;

	    // Resize the outer window to compensate for the tabline height
	    // change.  This is also needed when called from draw_tabline()
	    // (e.g. after :tabclose), where no caller will do the resize.
	    // When called from gui_init_which_components() the subsequent
	    // gui_set_shellsize() call will redo this to the same dimensions,
	    // which is harmless.
	    gui_ਸੈੱਟ_shellsize(FALSE, showit, RESIZE_VERT);
	}
	ਜੇ (showit != 0)
	    gui_mch_update_tabline();
    }
}

/*
 * Get the label or tooltip for tab page "tp" into NameBuff[].
 */
    ਖਾਲੀ
get_tabline_label(
    tabpage_T	*tp,
    ਪੂਰਨ		tooltip)	// TRUE: get tooltip
{
    char_u	**opt;

    // Use 'guitablabel' or 'guitabtooltip' if it's set.
    opt = (tooltip ? &p_gtt : &p_gtl);
    ਜੇ (**opt != NUL)
    {
	char_u	res[MAXPATHL];
	tabpage_T *save_curtab;
	char_u	*opt_name = (char_u *)(tooltip ? "guitabtooltip"
							     : "guitablabel");

	printer_ਪੰਨਾ_ਅੰਕ = tabpage_ਸੂਚਕਾਂਕ(tp);
# ifdef FEAT_EVAL
	ਸੈੱਟ_vim_var_nr(VV_LNUM, printer_ਪੰਨਾ_ਅੰਕ);
# endif
	// It's almost as going to the tabpage, but without autocommands.
	curtab->tp_firstwin = firstwin;
	curtab->tp_lastwin = lastwin;
	curtab->tp_curwin = curwin;
	save_curtab = curtab;
	curtab = tp;
	topframe = curtab->tp_topframe;
	firstwin = curtab->tp_firstwin;
	lastwin = curtab->tp_lastwin;
	curwin = curtab->tp_curwin;
	curbuf = curwin->w_ਬਫਰ;

	// Can't use NameBuff directly, build_stl_str_hl() uses it.
	build_stl_str_hl(curwin, res, MAXPATHL, *opt, opt_name, 0,
					   0, (ਪੂਰਨ)Columns, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
	STRCPY(NameBuff, res);

	// Back to the original curtab.
	curtab = save_curtab;
	topframe = curtab->tp_topframe;
	firstwin = curtab->tp_firstwin;
	lastwin = curtab->tp_lastwin;
	curwin = curtab->tp_curwin;
	curbuf = curwin->w_ਬਫਰ;
    }

    // If 'guitablabel'/'guitabtooltip' is not set or the result is empty then
    // use a default label.
    ਜੇ (**opt == NUL || *NameBuff == NUL)
    {
	win_T	*wp;
	ਪੂਰਨ	wincount = 0;
	ਪੂਰਨ	modified = FALSE;

	// Get the buffer name into NameBuff[] and shorten it.
	get_trans_bufname(tp == curtab ? curbuf : tp->tp_curwin->w_ਬਫਰ);
	ਜੇ (!tooltip)
	    shorten_dir(NameBuff);

	FOR_ALL_WINDOWS_ਵਿੱਚ_TAB(tp, wp)
	{
	    ++wincount;
	    ਜੇ (buf_Is_ਬਦਲਿਆ(wp->w_ਬਫਰ))
		modified = TRUE;
	}

	ਜੇ (modified || wincount > 1)
	{
	    char_u  buf[40] = "+ ";		// Tentatively assume modified only
	    size_t  buflen = 2;
	    size_t  NameBufflen = STRLEN(NameBuff);

	    ਜੇ (wincount > 1)
		buflen = vim_snprintf_safelen(
		    (ਅੱਖਰ *)buf,
		    ਆਕਾਰ(buf),
		    "%d%s ",
		    wincount,
		    modified ? "+" : "");

	    // Make sure resulting NameBuff will not exceed its bounds.
	    ਜੇ (NameBufflen + buflen < MAXPATHL)
	    {
		mch_memmove(NameBuff + buflen, NameBuff, NameBufflen + 1);	// +1 for NUL
		mch_memmove(NameBuff, buf, buflen);
	    }
	}
    }
}

/*
 * Send the event for clicking to select tab page "nr".
 * Returns TRUE if it was done, FALSE when skipped because we are already at
 * that tab page or the cmdline window is open.
 */
    ਪੂਰਨ
send_tabline_event(ਪੂਰਨ nr)
{
    char_u string[3];

    ਜੇ (nr == tabpage_ਸੂਚਕਾਂਕ(curtab))
	ਵਾਪਸ FALSE;

    // Don't put events in the input queue now.
    ਜੇ (hold_gui_events || cmdwin_type != 0)
    {
	// Set it back to the current tab page.
	gui_mch_ਸੈੱਟ_curtab(tabpage_ਸੂਚਕਾਂਕ(curtab));
	ਵਾਪਸ FALSE;
    }

    string[0] = CSI;
    string[1] = KS_TABLINE;
    string[2] = KE_FILLER;
    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, 3);
    string[0] = nr;
    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf_csi(string, 1);
    ਵਾਪਸ TRUE;
}

/*
 * Send a tabline menu event
 */
    ਖਾਲੀ
send_tabline_menu_event(ਪੂਰਨ tabidx, ਪੂਰਨ event)
{
    char_u	    string[3];

    // Don't put events in the input queue now.
    ਜੇ (hold_gui_events)
	ਵਾਪਸ;

    // Cannot close the last tabpage.
    ਜੇ (event == TABLINE_MENU_CLOSE && ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ == ਨਿਰਾ)
	ਵਾਪਸ;

    string[0] = CSI;
    string[1] = KS_TABMENU;
    string[2] = KE_FILLER;
    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, 3);
    string[0] = tabidx;
    string[1] = (char_u)(ਲੰਮਾ)event;
    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf_csi(string, 2);
}

#ਜੇ_ਅੰਤ

/*
 * Scrollbar stuff:
 */

/*
 * Remove all scrollbars.  Used before switching to another tab page.
 */
    ਖਾਲੀ
gui_remove_scrollbars(ਖਾਲੀ)
{
    ਪੂਰਨ	    i;
    win_T   *wp;

    ਲਈ (i = 0; i < 3; i++)
    {
	ਜੇ (i == SBAR_BOTTOM)
	    gui_mch_enable_scrollbar(&gui.bottom_sbar, FALSE);
	ਨਹੀਂਤਾਂ
	{
	    FOR_ALL_WINDOWS(wp)
		gui_do_scrollbar(wp, i, FALSE);
	}
	curtab->tp_prev_which_scrollbars[i] = -1;
    }
}

    ਖਾਲੀ
gui_create_scrollbar(scrollbar_T *sb, ਪੂਰਨ type, win_T *wp)
{
    ਸਥਿਰ ਪੂਰਨ	sbar_ident = 0;

    sb->ident = sbar_ident++;	// No check for too big, but would it happen?
    sb->wp = wp;
    sb->type = type;
    sb->ਮੁੱਲ = 0;
    sb->ਮਾਪ = 1;
    sb->max = 1;
    sb->ਸਿਖਰ = 0;
    sb->ਉਚਾਈ = 0;
    sb->ਚੌੜਾਈ = 0;
    sb->ਸਥਿਤੀ_ਉਚਾਈ = 0;
    gui_mch_create_scrollbar(sb, (wp == ਨਿਰਾ) ? SBAR_HORIZ : SBAR_VERT);
}

/*
 * Find the scrollbar with the given index.
 */
    scrollbar_T *
gui_ਲੱਭੋ_scrollbar(ਲੰਮਾ ident)
{
    win_T	*wp;

    ਜੇ (gui.bottom_sbar.ident == ident)
	ਵਾਪਸ &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	ਜੇ (wp->w_scrollbars[SBAR_ਖੱਬਾ].ident == ident)
	    ਵਾਪਸ &wp->w_scrollbars[SBAR_ਖੱਬਾ];
	ਜੇ (wp->w_scrollbars[SBAR_ਸੱਜਾ].ident == ident)
	    ਵਾਪਸ &wp->w_scrollbars[SBAR_ਸੱਜਾ];
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * For most systems: Put a code in the input buffer for a dragged scrollbar.
 *
 * For Win32, Macintosh and GTK+ 2:
 * Scrollbars seem to grab focus and vim doesn't read the input queue until
 * you stop dragging the scrollbar.  We get here each time the scrollbar is
 * dragged another pixel, but as far as the rest of vim goes, it thinks
 * we're just hanging in the call to DispatchMessage() in
 * process_message().  The DispatchMessage() call that hangs was passed a
 * mouse button click event in the scrollbar window. -- webb.
 *
 * Solution: Do the scrolling right here.  But only when allowed.
 * Ignore the scrollbars while executing an external command or when there
 * are still characters to be processed.
 */
    ਖਾਲੀ
gui_drag_scrollbar(scrollbar_T *sb, ਲੰਮਾ ਮੁੱਲ, ਪੂਰਨ still_dragging)
{
    win_T	*wp;
    ਪੂਰਨ		sb_ਅੰਕ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ON_FLY_SCROLL
    colnr_T	old_leftcol = curwin->w_leftcol;
    linenr_T	old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    ਪੂਰਨ		old_topfill = curwin->w_topfill;
# endif
#ਨਹੀਂਤਾਂ
    char_u	ਬਾਈਟਾਂ[ਆਕਾਰ(long_u)];
    ਪੂਰਨ		byte_ਗਿਣਤੀ;
#ਜੇ_ਅੰਤ

    ਜੇ (sb == ਨਿਰਾ)
	ਵਾਪਸ;

    // Don't put events in the input queue now.
    ਜੇ (hold_gui_events)
	ਵਾਪਸ;

    ਜੇ (cmdwin_type != 0 && sb->wp != cmdwin_win)
	ਵਾਪਸ;

    ਜੇ (still_dragging)
    {
	ਜੇ (sb->wp == ਨਿਰਾ)
	    gui.dragged_sb = SBAR_BOTTOM;
	ਨਹੀਂਤਾਂ ਜੇ (sb == &sb->wp->w_scrollbars[SBAR_ਖੱਬਾ])
	    gui.dragged_sb = SBAR_ਖੱਬਾ;
	ਨਹੀਂਤਾਂ
	    gui.dragged_sb = SBAR_ਸੱਜਾ;
	gui.dragged_wp = sb->wp;
    }
    ਨਹੀਂਤਾਂ
    {
	gui.dragged_sb = SBAR_NONE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
	// Keep the "dragged_wp" value until after the scrolling, for when the
	// mouse button is released.  GTK2 doesn't send the button-up event.
	gui.dragged_wp = ਨਿਰਾ;
	// WinScrolled event
	gui_focus_change(TRUE);
#ਜੇ_ਅੰਤ
    }

    // Vertical sbar info is kept in the first sbar (the left one)
    ਜੇ (sb->wp != ਨਿਰਾ)
	sb = &sb->wp->w_scrollbars[0];

    /*
     * Check validity of value
     */
    ਜੇ (ਮੁੱਲ < 0)
	ਮੁੱਲ = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SCROLL_PAST_ਸਿਰਾ
    ਨਹੀਂਤਾਂ ਜੇ (ਮੁੱਲ > sb->max)
	ਮੁੱਲ = sb->max;
#ਨਹੀਂਤਾਂ
    ਜੇ (ਮੁੱਲ > sb->max - sb->ਮਾਪ + 1)
	ਮੁੱਲ = sb->max - sb->ਮਾਪ + 1;
#ਜੇ_ਅੰਤ

    sb->ਮੁੱਲ = ਮੁੱਲ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ON_FLY_SCROLL
    // When not allowed to do the scrolling right now, return.
    // This also checked input_available(), but that causes the first click in
    // a scrollbar to be ignored when Vim doesn't have focus.
    ਜੇ (dont_scroll)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    // Disallow scrolling the current window when the completion popup menu is
    // visible.
    ਜੇ ((sb->wp == ਨਿਰਾ || sb->wp == curwin) && pum_visible())
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (sb->wp == ਨਿਰਾ && curwin->w_p_rl)
    {
	ਮੁੱਲ = sb->max + 1 - sb->ਮਾਪ - ਮੁੱਲ;
	ਜੇ (ਮੁੱਲ < 0)
	    ਮੁੱਲ = 0;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (sb->wp != ਨਿਰਾ)		// vertical scrollbar
    {
	sb_ਅੰਕ = 0;
	ਲਈ (wp = firstwin; wp != sb->wp && wp != ਨਿਰਾ; wp = wp->w_ਅਗਲਾ)
	    sb_ਅੰਕ++;
	ਜੇ (wp == ਨਿਰਾ)
	    ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ON_FLY_SCROLL
	current_scrollbar = sb_ਅੰਕ;
	scrollbar_ਮੁੱਲ = ਮੁੱਲ;
	ਜੇ (State & ਬਹੁਲਕ_ਸਾਧਾਰਨ)
	{
	    gui_do_scroll();
	    setcursor();
	}
	ਨਹੀਂਤਾਂ ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ)
	{
	    ins_scroll();
	    setcursor();
	}
	ਨਹੀਂਤਾਂ ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
	{
	    ਜੇ (ਸੁਨੇਹਾ_scrolled == 0)
	    {
		gui_do_scroll();
		redrawcmdline();
	    }
	}
# ifdef FEAT_FOLDING
	// Value may have been changed for closed fold.
	sb->ਮੁੱਲ = sb->wp->w_topline - 1;
# endif

	// When dragging one scrollbar and there is another one at the other
	// side move the thumb of that one too.
	ਜੇ (gui.which_scrollbars[SBAR_ਸੱਜਾ] && gui.which_scrollbars[SBAR_ਖੱਬਾ])
	    gui_mch_ਸੈੱਟ_scrollbar_thumb(
		    &sb->wp->w_scrollbars[
			    sb == &sb->wp->w_scrollbars[SBAR_ਸੱਜਾ]
						    ? SBAR_ਖੱਬਾ : SBAR_ਸੱਜਾ],
		    sb->ਮੁੱਲ, sb->ਮਾਪ, sb->max);

#ਨਹੀਂਤਾਂ
	ਬਾਈਟਾਂ[0] = CSI;
	ਬਾਈਟਾਂ[1] = KS_VER_SCROLLBAR;
	ਬਾਈਟਾਂ[2] = KE_FILLER;
	ਬਾਈਟਾਂ[3] = (char_u)sb_ਅੰਕ;
	byte_ਗਿਣਤੀ = 4;
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ON_FLY_SCROLL
	scrollbar_ਮੁੱਲ = ਮੁੱਲ;

	ਜੇ (State & ਬਹੁਲਕ_ਸਾਧਾਰਨ)
	    do_mousescroll_horiz(scrollbar_ਮੁੱਲ);
	ਨਹੀਂਤਾਂ ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ)
	    ins_horscroll();
	ਨਹੀਂਤਾਂ ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
	{
	    ਜੇ (ਸੁਨੇਹਾ_scrolled == 0)
	    {
		do_mousescroll_horiz(scrollbar_ਮੁੱਲ);
		redrawcmdline();
	    }
	}
	ਜੇ (old_leftcol != curwin->w_leftcol)
	{
	    update_ਖਿੜਕੀ(curwin);   // update window, status and cmdline
	    setcursor();
	}
#ਨਹੀਂਤਾਂ
	ਬਾਈਟਾਂ[0] = CSI;
	ਬਾਈਟਾਂ[1] = KS_HOR_SCROLLBAR;
	ਬਾਈਟਾਂ[2] = KE_FILLER;
	byte_ਗਿਣਤੀ = 3;
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ON_FLY_SCROLL
    /*
     * synchronize other windows, as necessary according to 'scrollbind'
     */
    ਜੇ (curwin->w_p_scb
	    && ((sb->wp == ਨਿਰਾ && curwin->w_leftcol != old_leftcol)
		|| (sb->wp == curwin && (curwin->w_topline != old_topline
# ifdef FEAT_DIFF
					   || curwin->w_topfill != old_topfill
# endif
			))))
    {
	do_check_scrollbind(TRUE);
	// need to update the window right here
	FOR_ALL_WINDOWS(wp)
	    ਜੇ (wp->w_redr_type > 0)
		update_ਖਿੜਕੀ(wp);
	setcursor();
    }
    ਬਾਹਰਲਾ_flush_cursor(FALSE, TRUE);
#ਨਹੀਂਤਾਂ
    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(ਬਾਈਟਾਂ, byte_ਗਿਣਤੀ);
    ਜੋੜੋ_long_ਮੰਜ਼ਿਲ_buf((long_u)ਮੁੱਲ, ਬਾਈਟਾਂ);
    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf_csi(ਬਾਈਟਾਂ, ਆਕਾਰ(long_u));
#ਜੇ_ਅੰਤ
}

/*
 * Scrollbar stuff:
 */

/*
 * Called when something in the window layout has changed.
 */
    ਖਾਲੀ
gui_may_update_scrollbars(ਖਾਲੀ)
{
    ਜੇ (gui.ਵਿੱਚ_use && starting == 0)
    {
	ਬਾਹਰਲਾ_flush();
	gui_init_which_components(ਨਿਰਾ);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
}

    ਖਾਲੀ
gui_update_scrollbars(
    ਪੂਰਨ		force)	    // Force all scrollbars to get updated
{
    win_T	*wp;
    scrollbar_T	*sb;
    ਲੰਮਾ	val, ਮਾਪ, max;		// need 32 bits here
    ਪੂਰਨ		which_sb;
    ਪੂਰਨ		h, y;
    ਸਥਿਰ win_T *prev_curwin = ਨਿਰਾ;

    // Update the horizontal scrollbar
    gui_update_horiz_scrollbar(force);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MSWIN
    // Return straight away if there is neither a left nor right scrollbar.
    // On MS-Windows this is required anyway for scrollwheel messages.
    ਜੇ (!gui.which_scrollbars[SBAR_ਖੱਬਾ] && !gui.which_scrollbars[SBAR_ਸੱਜਾ])
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ

    /*
     * Don't want to update a scrollbar while we're dragging it.  But if we
     * have both a left and right scrollbar, and we drag one of them, we still
     * need to update the other one.
     */
    ਜੇ (!force && (gui.dragged_sb == SBAR_ਖੱਬਾ || gui.dragged_sb == SBAR_ਸੱਜਾ)
	    && gui.which_scrollbars[SBAR_ਖੱਬਾ]
	    && gui.which_scrollbars[SBAR_ਸੱਜਾ])
    {
	/*
	 * If we have two scrollbars and one of them is being dragged, just
	 * copy the scrollbar position from the dragged one to the other one.
	 */
	which_sb = SBAR_ਖੱਬਾ + SBAR_ਸੱਜਾ - gui.dragged_sb;
	ਜੇ (gui.dragged_wp != ਨਿਰਾ)
	    gui_mch_ਸੈੱਟ_scrollbar_thumb(
		    &gui.dragged_wp->w_scrollbars[which_sb],
		    gui.dragged_wp->w_scrollbars[0].ਮੁੱਲ,
		    gui.dragged_wp->w_scrollbars[0].ਮਾਪ,
		    gui.dragged_wp->w_scrollbars[0].max);
    }

    // avoid that moving components around generates events
    ++hold_gui_events;

    FOR_ALL_WINDOWS(wp)
    {
	ਜੇ (wp->w_ਬਫਰ == ਨਿਰਾ)	// just in case
	    ਜਾਰੀ;
	// Skip a scrollbar that is being dragged.
	ਜੇ (!force && (gui.dragged_sb == SBAR_ਖੱਬਾ
					     || gui.dragged_sb == SBAR_ਸੱਜਾ)
		&& gui.dragged_wp == wp)
	    ਜਾਰੀ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SCROLL_PAST_ਸਿਰਾ
	max = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - 1;
#ਨਹੀਂਤਾਂ
	max = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + wp->w_ਉਚਾਈ - 2;
#ਜੇ_ਅੰਤ
	ਜੇ (max < 0)			// empty buffer
	    max = 0;
	val = wp->w_topline - 1;
	ਮਾਪ = wp->w_ਉਚਾਈ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SCROLL_PAST_ਸਿਰਾ
	ਜੇ (val > max)			// just in case
	    val = max;
#ਨਹੀਂਤਾਂ
	ਜੇ (ਮਾਪ > max + 1)		// just in case
	    ਮਾਪ = max + 1;
	ਜੇ (val > max - ਮਾਪ + 1)
	    val = max - ਮਾਪ + 1;
#ਜੇ_ਅੰਤ
	ਜੇ (val < 0)			// minimal value is 0
	    val = 0;

	/*
	 * Scrollbar at index 0 (the left one) contains all the information.
	 * It would be the same info for left and right so we just store it for
	 * one of them.
	 */
	sb = &wp->w_scrollbars[0];

	/*
	 * Note: no check for valid w_botline.	If it's not valid the
	 * scrollbars will be updated later anyway.
	 */
	ਜੇ (ਮਾਪ < 1 || wp->w_botline - 2 > max)
	{
	    /*
	     * This can happen during changing files.  Just don't update the
	     * scrollbar for now.
	     */
	    sb->ਉਚਾਈ = 0;	    // Force update next time
	    ਜੇ (gui.which_scrollbars[SBAR_ਖੱਬਾ])
		gui_do_scrollbar(wp, SBAR_ਖੱਬਾ, FALSE);
	    ਜੇ (gui.which_scrollbars[SBAR_ਸੱਜਾ])
		gui_do_scrollbar(wp, SBAR_ਸੱਜਾ, FALSE);
	    ਜਾਰੀ;
	}
	ਜੇ (force || sb->ਉਚਾਈ != wp->w_ਉਚਾਈ
	    || sb->ਸਿਖਰ != wp->w_winrow
	    || sb->ਸਥਿਤੀ_ਉਚਾਈ != wp->w_ਸਥਿਤੀ_ਉਚਾਈ
	    || sb->ਚੌੜਾਈ != wp->w_ਚੌੜਾਈ
	    || prev_curwin != curwin)
	{
	    // Height, width or position of scrollbar has changed.  For
	    // vertical split: curwin changed.
	    sb->ਉਚਾਈ = wp->w_ਉਚਾਈ;
	    sb->ਸਿਖਰ = wp->w_winrow;
	    sb->ਸਥਿਤੀ_ਉਚਾਈ = wp->w_ਸਥਿਤੀ_ਉਚਾਈ;
	    sb->ਚੌੜਾਈ = wp->w_ਚੌੜਾਈ;

	    // Calculate height and position in pixels
	    h = (sb->ਉਚਾਈ + sb->ਸਥਿਤੀ_ਉਚਾਈ) * gui.char_ਉਚਾਈ;
	    y = sb->ਸਿਖਰ * gui.char_ਉਚਾਈ + gui.border_ਵਿੱਥ;
#ਜੇ defined(FEAT_MENU) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF) && !defined(FEAT_GUI_PHOTON)
	    ਜੇ (gui.menu_is_ਸਰਗਰਮ)
		y += gui.menu_ਉਚਾਈ;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU))
	    ਜੇ (vim_strchr(p_go, GO_TOOLBAR) != ਨਿਰਾ)
		y += gui.toolbar_ਉਚਾਈ;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
	    ਜੇ (gui_has_tabline())
		y += gui.tabline_ਉਚਾਈ;
#ਜੇ_ਅੰਤ

	    ਜੇ (wp->w_winrow == 0)
	    {
		// Height of top scrollbar includes width of top border
		h += gui.border_ਵਿੱਥ;
		y -= gui.border_ਵਿੱਥ;
	    }
	    ਜੇ (gui.which_scrollbars[SBAR_ਖੱਬਾ])
	    {
		gui_mch_ਸੈੱਟ_scrollbar_pos(&wp->w_scrollbars[SBAR_ਖੱਬਾ],
					  gui.ਖੱਬਾ_sbar_x, y,
					  gui.scrollbar_ਚੌੜਾਈ, h);
		gui_do_scrollbar(wp, SBAR_ਖੱਬਾ, TRUE);
	    }
	    ਜੇ (gui.which_scrollbars[SBAR_ਸੱਜਾ])
	    {
		gui_mch_ਸੈੱਟ_scrollbar_pos(&wp->w_scrollbars[SBAR_ਸੱਜਾ],
					  gui.ਸੱਜਾ_sbar_x, y,
					  gui.scrollbar_ਚੌੜਾਈ, h);
		gui_do_scrollbar(wp, SBAR_ਸੱਜਾ, TRUE);
	    }
	}

	ਜੇ (force || sb->ਮੁੱਲ != val || sb->ਮਾਪ != ਮਾਪ || sb->max != max)
	{
	    // Thumb of scrollbar has moved
	    sb->ਮੁੱਲ = val;
	    sb->ਮਾਪ = ਮਾਪ;
	    sb->max = max;
	    ਜੇ (gui.which_scrollbars[SBAR_ਖੱਬਾ]
		    && (gui.dragged_sb != SBAR_ਖੱਬਾ || gui.dragged_wp != wp))
		gui_mch_ਸੈੱਟ_scrollbar_thumb(&wp->w_scrollbars[SBAR_ਖੱਬਾ],
					    val, ਮਾਪ, max);
	    ਜੇ (gui.which_scrollbars[SBAR_ਸੱਜਾ]
		    && (gui.dragged_sb != SBAR_ਸੱਜਾ || gui.dragged_wp != wp))
		gui_mch_ਸੈੱਟ_scrollbar_thumb(&wp->w_scrollbars[SBAR_ਸੱਜਾ],
					    val, ਮਾਪ, max);
	}
    }

    // update the title, it may show the scroll position
    maketitle();

    prev_curwin = curwin;
    --hold_gui_events;
}

/*
 * Enable or disable a scrollbar.
 * Check for scrollbars for vertically split windows which are not enabled
 * sometimes.
 */
    ਸਥਿਰ ਖਾਲੀ
gui_do_scrollbar(
    win_T	*wp,
    ਪੂਰਨ		which,	    // SBAR_LEFT or SBAR_RIGHT
    ਪੂਰਨ		enable)	    // TRUE to enable scrollbar
{
    ਪੂਰਨ		midcol = curwin->w_wincol + curwin->w_ਚੌੜਾਈ / 2;
    ਪੂਰਨ		has_midcol = (wp->w_wincol <= midcol
				     && wp->w_wincol + wp->w_ਚੌੜਾਈ >= midcol);

    // Only enable scrollbars that contain the middle column of the current
    // window.
    ਜੇ (gui.which_scrollbars[SBAR_ਸੱਜਾ] != gui.which_scrollbars[SBAR_ਖੱਬਾ])
    {
	// Scrollbars only on one side.  Don't enable scrollbars that don't
	// contain the middle column of the current window.
	ਜੇ (!has_midcol)
	    enable = FALSE;
    }
    ਨਹੀਂਤਾਂ
    {
	// Scrollbars on both sides.  Don't enable scrollbars that neither
	// contain the middle column of the current window nor are on the far
	// side.
	ਜੇ (midcol > Columns / 2)
	{
	    ਜੇ (which == SBAR_ਖੱਬਾ ? wp->w_wincol != 0 : !has_midcol)
		enable = FALSE;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (which == SBAR_ਸੱਜਾ ? wp->w_wincol + wp->w_ਚੌੜਾਈ != Columns
								: !has_midcol)
		enable = FALSE;
	}
    }
    gui_mch_enable_scrollbar(&wp->w_scrollbars[which], enable);
}

/*
 * Scroll a window according to the values set in the globals
 * "current_scrollbar" and "scrollbar_value".
 * Return TRUE if the cursor in the current window moved or FALSE otherwise.
 * may eventually cause a redraw using updateWindow
 */
    ਪੂਰਨ
gui_do_scroll(ਖਾਲੀ)
{
    win_T	*wp, *save_wp;
    ਪੂਰਨ		i;
    ਲੰਮਾ	nlines;
    pos_T	old_cursor;
    linenr_T	old_topline;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		old_topfill;
#ਜੇ_ਅੰਤ

    ਲਈ (wp = firstwin, i = 0; i < current_scrollbar; wp = W_ਅਗਲਾ(wp), i++)
	ਜੇ (wp == ਨਿਰਾ)
	    ਰੋਕੋ;
    ਜੇ (wp == ਨਿਰਾ)
	// Couldn't find window
	ਵਾਪਸ FALSE;
    // don't redraw, LineOffset and similar are not valid!
    ਜੇ (exmode_ਸਰਗਰਮ)
	ਵਾਪਸ FALSE;

    /*
     * Compute number of lines to scroll.  If zero, nothing to do.
     */
    nlines = (ਲੰਮਾ)scrollbar_ਮੁੱਲ + 1 - (ਲੰਮਾ)wp->w_topline;
    ਜੇ (nlines == 0)
	ਵਾਪਸ FALSE;

    save_wp = curwin;
    old_topline = wp->w_topline;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    old_topfill = wp->w_topfill;
#ਜੇ_ਅੰਤ
    old_cursor = wp->w_cursor;
    curwin = wp;
    curbuf = wp->w_ਬਫਰ;
    ਜੇ (nlines < 0)
	scrolldown(-nlines, gui.dragged_wp == ਨਿਰਾ);
    ਨਹੀਂਤਾਂ
	scrollup(nlines, gui.dragged_wp == ਨਿਰਾ);
    // Reset dragged_wp after using it.  "dragged_sb" will have been reset for
    // the mouse-up event already, but we still want it to behave like when
    // dragging.  But not the next click in an arrow.
    ਜੇ (gui.dragged_sb == SBAR_NONE)
	gui.dragged_wp = ਨਿਰਾ;

    ਜੇ (old_topline != wp->w_topline
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    || old_topfill != wp->w_topfill
#ਜੇ_ਅੰਤ
	    )
    {
	ਜੇ (get_scrolloff_ਮੁੱਲ() != 0)
	{
	    cursor_correct();		// fix window for 'so'
	    update_topline();		// avoid up/down jump
	}
	ਜੇ (old_cursor.lnum != wp->w_cursor.lnum)
	    coladvance(wp->w_curswant);
	wp->w_scbind_pos = wp->w_topline;
    }

    // Make sure wp->w_leftcol and wp->w_skipcol are correct.
    validate_cursor();

    curwin = save_wp;
    curbuf = save_wp->w_ਬਫਰ;

    /*
     * Don't call updateWindow() when nothing has changed (it will overwrite
     * the status line!).
     *
     * Check for ScreenLines, because in ex-mode, we don't have a valid display.
     */
    ਜੇ (Screen_ਸਤਰਾਂ != ਨਿਰਾ && (old_topline != wp->w_topline
	    || wp->w_redr_type != 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    || old_topfill != wp->w_topfill
#ਜੇ_ਅੰਤ
	    ))
    {
	ਪੂਰਨ type = UPD_VALID;

	ਜੇ (pum_visible())
	{
	    type = UPD_NOT_VALID;
	    wp->w_ਸਤਰਾਂ_valid = 0;
	}

	// Don't set must_redraw here, it may cause the popup menu to
	// disappear when losing focus after a scrollbar drag.
	ਜੇ (wp->w_redr_type < type)
	    wp->w_redr_type = type;
	mch_disable_flush();
	update_ਖਿੜਕੀ(wp);   // update window, status line, and cmdline
	mch_enable_flush();
    }

    // May need to redraw the popup menu.
    ਜੇ (pum_visible())
	pum_redraw();

    ਵਾਪਸ (wp == curwin && !EQUAL_POS(curwin->w_cursor, old_cursor));
}

/*
 * Horizontal scrollbar stuff:
 */
    ਸਥਿਰ ਖਾਲੀ
gui_update_horiz_scrollbar(ਪੂਰਨ force)
{
    ਲੰਮਾ	ਮੁੱਲ, ਮਾਪ, max;

    ਜੇ (!gui.which_scrollbars[SBAR_BOTTOM])
	ਵਾਪਸ;

    ਜੇ (!force && gui.dragged_sb == SBAR_BOTTOM)
	ਵਾਪਸ;

    ਜੇ (!force && curwin->w_p_wrap && gui.prev_wrap)
	ਵਾਪਸ;

    /*
     * It is possible for the cursor to be invalid if we're in the middle of
     * something (like changing files).  If so, don't do anything for now.
     */
    ਜੇ (curwin->w_cursor.lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	gui.bottom_sbar.ਮੁੱਲ = -1;
	ਵਾਪਸ;
    }

    ਮਾਪ = curwin->w_ਚੌੜਾਈ;
    ਜੇ (curwin->w_p_wrap)
    {
	ਮੁੱਲ = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SCROLL_PAST_ਸਿਰਾ
	max = 0;
#ਨਹੀਂਤਾਂ
	max = curwin->w_ਚੌੜਾਈ - 1;
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
	ਮੁੱਲ = curwin->w_leftcol;
	max = scroll_ਸਤਰ_len(ui_ਲੱਭੋ_longest_lnum());

	ਜੇ (virtual_ਸਰਗਰਮ())
	{
	    // May move the cursor even further to the right.
	    ਜੇ (curwin->w_virtcol >= (colnr_T)max)
		max = curwin->w_virtcol;
	}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SCROLL_PAST_ਸਿਰਾ
	max += curwin->w_ਚੌੜਾਈ - 1;
#ਜੇ_ਅੰਤ
	// The line number isn't scrolled, thus there is less space when
	// 'number' or 'relativenumber' is set (also for 'foldcolumn').
	ਮਾਪ -= curwin_ਖਾਨਾ_off();
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SCROLL_PAST_ਸਿਰਾ
	max -= curwin_ਖਾਨਾ_off();
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SCROLL_PAST_ਸਿਰਾ
    ਜੇ (ਮੁੱਲ > max - ਮਾਪ + 1)
	ਮੁੱਲ = max - ਮਾਪ + 1;	    // limit the value to allowable range
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (curwin->w_p_rl)
    {
	ਮੁੱਲ = max + 1 - ਮਾਪ - ਮੁੱਲ;
	ਜੇ (ਮੁੱਲ < 0)
	{
	    ਮਾਪ += ਮੁੱਲ;
	    ਮੁੱਲ = 0;
	}
    }
#ਜੇ_ਅੰਤ
    ਜੇ (!force && ਮੁੱਲ == gui.bottom_sbar.ਮੁੱਲ && ਮਾਪ == gui.bottom_sbar.ਮਾਪ
						&& max == gui.bottom_sbar.max)
	ਵਾਪਸ;

    gui.bottom_sbar.ਮੁੱਲ = ਮੁੱਲ;
    gui.bottom_sbar.ਮਾਪ = ਮਾਪ;
    gui.bottom_sbar.max = max;
    gui.prev_wrap = curwin->w_p_wrap;

    gui_mch_ਸੈੱਟ_scrollbar_thumb(&gui.bottom_sbar, ਮੁੱਲ, ਮਾਪ, max);
}

/*
 * Check that none of the colors are the same as the background color
 */
    ਖਾਲੀ
gui_check_colors(ਖਾਲੀ)
{
    ਜੇ (gui.norm_pixel == gui.ਪਿੱਠ_pixel || gui.norm_pixel == INVALCOLOR)
    {
	gui_ਸੈੱਟ_bg_color((char_u *)"White");
	ਜੇ (gui.norm_pixel == gui.ਪਿੱਠ_pixel || gui.norm_pixel == INVALCOLOR)
	    gui_ਸੈੱਟ_fg_color((char_u *)"Black");
    }
}

    ਸਥਿਰ ਖਾਲੀ
gui_ਸੈੱਟ_fg_color(char_u *name)
{
    gui.norm_pixel = gui_get_color(name);
    hl_ਸੈੱਟ_fg_color_name(vim_strsave(name));
}

    ਸਥਿਰ ਖਾਲੀ
gui_ਸੈੱਟ_bg_color(char_u *name)
{
    gui.ਪਿੱਠ_pixel = gui_get_color(name);
    hl_ਸੈੱਟ_bg_color_name(vim_strsave(name));
}

/*
 * Allocate a color by name.
 * Returns INVALCOLOR and gives an error message when failed.
 */
    guicolor_T
gui_get_color(char_u *name)
{
    guicolor_T	t;

    ਜੇ (*name == NUL)
	ਵਾਪਸ INVALCOLOR;
    t = gui_mch_get_color(name);

    ਪੂਰਨ is_none = STRCMP(name, "none") == 0;
    ਜੇ (t == INVALCOLOR
#ਜੇ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
	    && (gui.ਵਿੱਚ_use || is_none)
#ਜੇ_ਅੰਤ
	    )
    {
	ਜੇ (is_none)
	    emsg(_(e_cannot_use_color_none_did_you_ਔਸਤ_none));
	ਨਹੀਂਤਾਂ
	    semsg(_(e_cannot_allocate_color_str), name);
    }
    ਵਾਪਸ t;
}

/*
 * Return the grey value of a color (range 0-255).
 */
    ਪੂਰਨ
gui_get_lightness(guicolor_T pixel)
{
    long_u	rgb = (long_u)gui_mch_get_rgb(pixel);

    ਵਾਪਸ  (ਪੂਰਨ)(  (((rgb >> 16) & 0xff) * 299)
		   + (((rgb >> 8) & 0xff) * 587)
		   +  ((rgb	  & 0xff) * 114)) / 1000;
}

    char_u *
gui_bg_default(ਖਾਲੀ)
{
    ਜੇ (gui_get_lightness(gui.ਪਿੱਠ_pixel) < 127)
	ਵਾਪਸ (char_u *)"dark";
    ਵਾਪਸ (char_u *)"light";
}

/*
 * Option initializations that can only be done after opening the GUI window.
 */
    ਸਥਿਰ ਖਾਲੀ
init_gui_options(ਖਾਲੀ)
{
    // Set the 'background' option according to the lightness of the
    // background color, unless the user has set it already.
    ਜੇ (!option_was_ਸੈੱਟ((char_u *)"bg") && STRCMP(p_bg, gui_bg_default()) != 0)
    {
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"bg", 0L, gui_bg_default(), 0);
	highlight_ਬਦਲਿਆ();
    }
}

#ਜੇ defined(FEAT_GUI_X11)
    ਖਾਲੀ
gui_new_scrollbar_colors(ਖਾਲੀ)
{
    win_T	*wp;

    // Nothing to do if GUI hasn't started yet.
    ਜੇ (!gui.ਵਿੱਚ_use)
	ਵਾਪਸ;

    FOR_ALL_WINDOWS(wp)
    {
	gui_mch_ਸੈੱਟ_scrollbar_colors(&(wp->w_scrollbars[SBAR_ਖੱਬਾ]));
	gui_mch_ਸੈੱਟ_scrollbar_colors(&(wp->w_scrollbars[SBAR_ਸੱਜਾ]));
    }
    gui_mch_ਸੈੱਟ_scrollbar_colors(&gui.bottom_sbar);
}
#ਜੇ_ਅੰਤ

/*
 * Call this when focus has changed.
 */
    ਖਾਲੀ
gui_focus_change(ਪੂਰਨ ਵਿੱਚ_focus)
{
/*
 * Skip this code to avoid drawing the cursor when debugging and switching
 * between the debugger window and gvim.
 */
#ਜੇ 1
    gui.ਵਿੱਚ_focus = ਵਿੱਚ_focus;
    ਬਾਹਰਲਾ_flush_cursor(TRUE, FALSE);

# ifdef FEAT_GUI_MSWIN
    gui_mch_ਸੈੱਟ_titlebar_colors();
# endif

# ifdef FEAT_XIM
    xim_ਸੈੱਟ_focus(ਵਿੱਚ_focus);
# endif

    // Put events in the input queue only when allowed.
    // ui_focus_change() isn't called directly, because it invokes
    // autocommands and that must not happen asynchronously.
    ਜੇ (!hold_gui_events
# ਜੇ defined(FEAT_GUI_GTK) && defined(FEAT_GUI_DIALOG)
	    && gui.dialogs_ਸਰਗਰਮ == 0
# endif
       )
    {
	char_u  ਬਾਈਟਾਂ[3];

	ਬਾਈਟਾਂ[0] = CSI;
	ਬਾਈਟਾਂ[1] = KS_EXTRA;
	ਬਾਈਟਾਂ[2] = ਵਿੱਚ_focus ? (ਪੂਰਨ)KE_FOCUSGAINED : (ਪੂਰਨ)KE_FOCUSLOST;
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(ਬਾਈਟਾਂ, 3);
    }
#ਜੇ_ਅੰਤ
}

/*
 * When mouse moved: apply 'mousefocus'.
 * Also updates the mouse pointer shape.
 */
    ਸਥਿਰ ਖਾਲੀ
gui_mouse_focus(ਪੂਰਨ x, ਪੂਰਨ y)
{
    win_T	*wp;
    char_u	st[8];

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
    // Get window pointer, and update mouse shape as well.
    wp = xy2win(x, y, IGNORE_POPUP);
#ਜੇ_ਅੰਤ

    // Only handle this when 'mousefocus' set and ...
    ਜੇ (p_mousef
	    && !hold_gui_events		// not holding events
	    && (State & (ਬਹੁਲਕ_ਸਾਧਾਰਨ | ਬਹੁਲਕ_ਪਾਓ))// Normal/Visual/Insert mode
	    && State != ਬਹੁਲਕ_HITRETURN	// but not hit-return prompt
	    && ਸੁਨੇਹਾ_scrolled == 0	// no scrolled message
	    && !need_mouse_correct	// not moving the pointer
	    && gui.ਵਿੱਚ_focus)		// gvim in focus
    {
	// Don't move the mouse when it's left or right of the Vim window
	ਜੇ (x < 0 || x > Columns * gui.char_ਚੌੜਾਈ)
	    ਵਾਪਸ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
	wp = xy2win(x, y, IGNORE_POPUP);
#ਜੇ_ਅੰਤ
	ਜੇ (wp == curwin || wp == ਨਿਰਾ)
	    ਵਾਪਸ;	// still in the same old window, or none at all

	// Ignore position in the tab pages line.
	ਜੇ (Y_2_ਪੰਕਤੀ(y) < tabline_ਉਚਾਈ())
	    ਵਾਪਸ;

	/*
	 * Format a mouse click on status line input,
	 * ala gui_send_mouse_event(0, x, y, 0, 0);
	 * Trick: Use a column number -1, so that get_pseudo_mouse_code() will
	 * generate a K_LEFTMOUSE_NM key code.
	 */
	ਜੇ (ਅੰਤ_op)
	{
	    // abort the current operator first
	    st[0] = ESC;
	    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(st, 1);
	}
	st[0] = CSI;
	st[1] = KS_MOUSE;
	st[2] = KE_FILLER;
	st[3] = (char_u)MOUSE_ਖੱਬਾ;
	fill_mouse_coord(st + 4,
		wp->w_wincol == 0 ? -1 : wp->w_wincol + MOUSE_COLOFF,
		wp->w_ਉਚਾਈ + W_WINROW(wp));

	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(st, 8);
	st[3] = (char_u)MOUSE_RELEASE;
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(st, 8);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
	// Need to wake up the main loop
	ਜੇ (gtk_main_level() > 0)
	    gtk_main_quit();
#ਜੇ_ਅੰਤ
    }
}

/*
 * Called when the mouse moved (but not when dragging).
 */
    ਖਾਲੀ
gui_mouse_moved(ਪੂਰਨ x, ਪੂਰਨ y)
{
    // Ignore this while still starting up.
    ਜੇ (!gui.ਵਿੱਚ_use || gui.starting)
	ਵਾਪਸ;

    // apply 'mousefocus' and pointer shape
    gui_mouse_focus(x, y);

    ਜੇ (p_mousemev
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	|| popup_ਵਰਤਦਾ_mouse_move
#ਜੇ_ਅੰਤ
   )
	// Generate a mouse-moved event. For a <MouseMove> mapping. Or so the
	// popup can perhaps be closed, just like in the terminal.
	gui_send_mouse_event(MOUSE_MOVE, x, y, FALSE, 0);
}

/*
 * Get the window where the mouse pointer is on.
 * Returns NULL if not found.
 */
    win_T *
gui_mouse_ਖਿੜਕੀ(mouse_ਲੱਭੋ_T popup)
{
    ਪੂਰਨ		x, y;

    ਜੇ (!(gui.ਵਿੱਚ_use && (p_mousef || popup == ਲੱਭੋ_POPUP)))
	ਵਾਪਸ ਨਿਰਾ;
    gui_mch_getmouse(&x, &y);

    // Only use the mouse when it's on the Vim window
    ਜੇ (x >= 0 && x <= Columns * gui.char_ਚੌੜਾਈ
	    && y >= 0 && Y_2_ਪੰਕਤੀ(y) >= tabline_ਉਚਾਈ())
	ਵਾਪਸ xy2win(x, y, popup);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Called when mouse should be moved to window with focus.
 */
    ਖਾਲੀ
gui_mouse_correct(ਖਾਲੀ)
{
    win_T	*wp = ਨਿਰਾ;

    need_mouse_correct = FALSE;

    wp = gui_mouse_ਖਿੜਕੀ(IGNORE_POPUP);
    ਜੇ (wp == curwin || wp == ਨਿਰਾ)
	ਵਾਪਸ;

    // If in other than current window
    validate_cline_ਪੰਕਤੀ();
    gui_mch_setmouse((ਪੂਰਨ)W_ENDCOL(curwin) * gui.char_ਚੌੜਾਈ - 3,
	    (W_WINROW(curwin) + curwin->w_wrow) * gui.char_ਉਚਾਈ
	    + (gui.char_ਉਚਾਈ) / 2);
}

/*
 * Find window where the mouse pointer "x" / "y" coordinate is in.
 * As a side effect update the shape of the mouse pointer.
 */
    ਸਥਿਰ win_T *
xy2win(ਪੂਰਨ x, ਪੂਰਨ y, mouse_ਲੱਭੋ_T popup)
{
    ਪੂਰਨ		ਪੰਕਤੀ;
    ਪੂਰਨ		ਖਾਨਾ;
    win_T	*wp;

    ਪੰਕਤੀ = Y_2_ਪੰਕਤੀ(y);
    ਖਾਨਾ = X_2_ਖਾਨਾ(x);

    ਜੇ (ਪੰਕਤੀ < 0 || ਖਾਨਾ < 0)		// before first window
	ਵਾਪਸ ਨਿਰਾ;
    wp = mouse_ਲੱਭੋ_win(&ਪੰਕਤੀ, &ਖਾਨਾ, popup);
    ਜੇ (wp == ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
	update_mouseshape(-2);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ ਨਿਰਾ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
    ਜੇ (State == ਬਹੁਲਕ_HITRETURN || State == ਬਹੁਲਕ_ASKMORE)
    {
	ਜੇ (Y_2_ਪੰਕਤੀ(y) >= ਸੁਨੇਹਾ_ਪੰਕਤੀ)
	    update_mouseshape(SHAPE_IDX_MOREL);
	ਨਹੀਂਤਾਂ
	    update_mouseshape(SHAPE_IDX_ਹੋਰ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਪੰਕਤੀ >= wp->w_ਉਚਾਈ + wp->w_ਸਥਿਤੀ_ਉਚਾਈ)	// below status line
	update_mouseshape(SHAPE_IDX_CLINE);
    ਨਹੀਂਤਾਂ ਜੇ (!(State & ਬਹੁਲਕ_CMDLINE) && wp->w_vsep_ਚੌੜਾਈ > 0 && ਖਾਨਾ == wp->w_ਚੌੜਾਈ
	    && (!(ਪੰਕਤੀ >= wp->w_ਉਚਾਈ && ਪੰਕਤੀ < wp->w_ਉਚਾਈ
	    + wp->w_ਸਥਿਤੀ_ਉਚਾਈ) || !stl_connected(wp)) && ਸੁਨੇਹਾ_scrolled == 0)
	update_mouseshape(SHAPE_IDX_VSEP);
    ਨਹੀਂਤਾਂ ਜੇ (!(State & ਬਹੁਲਕ_CMDLINE) && wp->w_ਸਥਿਤੀ_ਉਚਾਈ > 0
	    && ਪੰਕਤੀ >= wp->w_ਉਚਾਈ && ਪੰਕਤੀ < wp->w_ਉਚਾਈ + wp->w_ਸਥਿਤੀ_ਉਚਾਈ
	    && ਸੁਨੇਹਾ_scrolled == 0)
	update_mouseshape(SHAPE_IDX_ਸਥਿਤੀ);
    ਨਹੀਂਤਾਂ
	update_mouseshape(-2);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ wp;
}

/*
 * ":gui" and ":gvim": Change from the terminal version to the GUI version.
 * File names may be given to redefine the args list.
 */
    ਖਾਲੀ
ex_gui(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    /*
     * Check for "-f" argument: foreground, don't fork.
     * Also don't fork when started with "gvim -f".
     * Do fork when using "gui -b".
     */
    ਜੇ (arg[0] == '-'
	    && (arg[1] == 'f' || arg[1] == 'b')
	    && (arg[2] == NUL || VIM_ISWHITE(arg[2])))
    {
	gui.dofork = (arg[1] == 'b');
	eap->arg = skipwhite(eap->arg + 2);
    }
    ਜੇ (!gui.ਵਿੱਚ_use)
    {
#ਜੇ defined(VIMDLL) && !defined(EXPERIMENTAL_GUI_CMD)
	ਜੇ (!gui.starting)
	{
	    emsg(_(e_gui_cannot_be_ਵਰਤਿਆ_not_enabled_at_compile_ਸਮਾਂ));
	    ਵਾਪਸ;
	}
#ਜੇ_ਅੰਤ
	// Clear the command.  Needed for when forking+exiting, to avoid part
	// of the argument ending up after the shell prompt.
	ਸੁਨੇਹਾ_clr_eos_force();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GUI_MAY_SPAWN
	ਜੇ (!ਸਿਰੇ_excmd2(eap->cmd, eap->arg))
	    gui_ਸ਼ੁਰੂ(eap->arg);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    gui_ਸ਼ੁਰੂ(ਨਿਰਾ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	channel_gui_ਰਜਿਸਟਰ_all();
#ਜੇ_ਅੰਤ
    }
    ਜੇ (!ਸਿਰੇ_excmd2(eap->cmd, eap->arg))
	ex_ਅਗਲਾ(eap);
}

#ਜੇ (defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	    || defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON) \
	    || defined(FEAT_GUI_HAIKU)) \
	    && defined(FEAT_TOOLBAR)
/*
 * This is shared between Haiku, Motif, and GTK.
 */

/*
 * Callback function for do_in_runtimepath().
 */
    ਸਥਿਰ ਖਾਲੀ
gfp_setname(char_u *fname, ਖਾਲੀ *cookie)
{
    char_u	*gfp_ਬਫਰ = cookie;

    ਜੇ (STRLEN(fname) >= MAXPATHL)
	*gfp_ਬਫਰ = NUL;
    ਨਹੀਂਤਾਂ
	STRCPY(gfp_ਬਫਰ, fname);
}

/*
 * Find the path of bitmap "name" with extension "ext" in 'runtimepath'.
 * Return FAIL for failure and OK if buffer[MAXPATHL] contains the result.
 */
    ਪੂਰਨ
gui_ਲੱਭੋ_bitmap(char_u *name, char_u *ਬਫਰ, ਅੱਖਰ *ext)
{
    ਜੇ (STRLEN(name) > MAXPATHL - 14)
	ਵਾਪਸ FAIL;
    vim_snprintf((ਅੱਖਰ *)ਬਫਰ, MAXPATHL, "bitmaps/%s.%s", name, ext);
    ਜੇ (do_ਵਿੱਚ_runtimepath(ਬਫਰ, 0, gfp_setname, ਬਫਰ) == FAIL
							    || *ਬਫਰ == NUL)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

# ਜੇ !defined(FEAT_GUI_GTK)
/*
 * Given the name of the "icon=" argument, try finding the bitmap file for the
 * icon.  If it is an absolute path name, use it as it is.  Otherwise append
 * "ext" and search for it in 'runtimepath'.
 * The result is put in "buffer[MAXPATHL]".  If something fails "buffer"
 * contains "name".
 */
    ਖਾਲੀ
gui_ਲੱਭੋ_iconfile(char_u *name, char_u *ਬਫਰ, ਅੱਖਰ *ext)
{
    char_u	buf[MAXPATHL + 1];

    expand_env(name, ਬਫਰ, MAXPATHL);
    ਜੇ (!mch_is_ਭਰਿਆ_Name(ਬਫਰ) && gui_ਲੱਭੋ_bitmap(ਬਫਰ, buf, ext) == OK)
	STRCPY(ਬਫਰ, buf);
}
# endif
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)|| defined(FEAT_GUI_HAIKU)
    ਖਾਲੀ
display_errors(ਖਾਲੀ)
{
    char_u	*p;

    ਜੇ (isatty(2))
    {
	fflush(stderr);
	ਵਾਪਸ;
    }

    ਜੇ (error_ga.ga_ਅੰਕੜੇ == ਨਿਰਾ)
	ਵਾਪਸ;

    // avoid putting up a message box with blanks only
    ਲਈ (p = (char_u *)error_ga.ga_ਅੰਕੜੇ; *p != NUL; ++p)
	ਜੇ (!ਸੁਰੱਖਿਅਤ_isspace(*p))
	{
	    // Truncate a very long message, it will go off-screen.
	    ਜੇ (STRLEN(p) > 2000)
		STRCPY(p + 2000 - 14, "...(truncated)");
	    (ਖਾਲੀ)do_dialog(VIM_ERROR, (char_u *)_("Error"),
		    p, (char_u *)_("&Ok"), 1, ਨਿਰਾ, FALSE);
	    ਰੋਕੋ;
	}
    ga_clear(&error_ga);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(NO_CONSOLE_ਨਿਵੇਸ਼)
/*
 * Return TRUE if still starting up and there is no place to enter text.
 * For GTK and X11 we check if stderr is not a tty, which means we were
 * (probably) started from the desktop.  Also check stdin, "vim >& file" does
 * allow typing on stdin.
 */
    ਪੂਰਨ
no_console_ਨਿਵੇਸ਼(ਖਾਲੀ)
{
    ਵਾਪਸ ((!gui.ਵਿੱਚ_use || gui.starting)
# ifndef NO_CONSOLE
	    && !isatty(0) && !isatty(2)
# endif
	    );
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(ਲੱਭੋ_REPLACE_DIALOG) \
	|| defined(NEED_GUI_UPDATE_SCREEN)
/*
 * Update the current window and the screen.
 */
    ਖਾਲੀ
gui_update_screen(ਖਾਲੀ)
{
# ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_ਸਤਰ = 0;
    linenr_T	conceal_new_cursor_ਸਤਰ = 0;
    ਪੂਰਨ		conceal_update_ਸਤਰਾਂ = FALSE;
# endif

    update_topline();
    validate_cursor();

    // Trigger CursorMoved if the cursor moved.
    ਜੇ (!ਅੰਤ_op && (has_cursormoved()
# ifdef FEAT_PROP_POPUP
		|| popup_visible
# endif
# ifdef FEAT_CONCEAL
		|| curwin->w_p_cole > 0
# endif
		) && !EQUAL_POS(ਆਖਰੀ_cursormoved, curwin->w_cursor))
    {
	ਜੇ (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
# ifdef FEAT_PROP_POPUP
	ਜੇ (popup_visible)
	    popup_check_cursor_pos();
# endif
# ifdef FEAT_CONCEAL
	ਜੇ (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_ਸਤਰ = ਆਖਰੀ_cursormoved.lnum;
	    conceal_new_cursor_ਸਤਰ = curwin->w_cursor.lnum;
	    conceal_update_ਸਤਰਾਂ = TRUE;
	}
# endif
	ਆਖਰੀ_cursormoved = curwin->w_cursor;
    }

    ਜੇ (!ਅੰਤ_op)
	may_trigger_win_scrolled_resized();

# ifdef FEAT_CONCEAL
    ਜੇ (conceal_update_ਸਤਰਾਂ
	    && (conceal_old_cursor_ਸਤਰ != conceal_new_cursor_ਸਤਰ
		|| conceal_cursor_ਸਤਰ(curwin)
		|| need_cursor_ਸਤਰ_redraw))
    {
	ਜੇ (conceal_old_cursor_ਸਤਰ != conceal_new_cursor_ਸਤਰ)
	    redrawWinline(curwin, conceal_old_cursor_ਸਤਰ);
	redrawWinline(curwin, conceal_new_cursor_ਸਤਰ);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_ਸਤਰ_redraw = FALSE;
    }
# endif
    update_screen(0);	// may need to update the screen
    setcursor();
    ਬਾਹਰਲਾ_flush_cursor(TRUE, FALSE);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(ਲੱਭੋ_REPLACE_DIALOG)
/*
 * Get the text to use in a find/replace dialog.  Uses the last search pattern
 * if the argument is empty.
 * Returns an allocated string.
 */
    char_u *
get_ਲੱਭੋ_dialog_ਪਾਠ(
    char_u	*arg,
    ਪੂਰਨ		*wwordp,	// return: TRUE if \< \> found
    ਪੂਰਨ		*mcasep)	// return: TRUE if \C found
{
    char_u	*ਪਾਠ;

    ਜੇ (*arg == NUL)
	ਪਾਠ = ਆਖਰੀ_ਖੋਜ_pat();
    ਨਹੀਂਤਾਂ
	ਪਾਠ = arg;
    ਜੇ (ਪਾਠ != ਨਿਰਾ)
    {
	ਪੂਰਨ len = (ਪੂਰਨ)STRLEN(ਪਾਠ);

	ਪਾਠ = vim_strnsave(ਪਾਠ, len);
	ਜੇ (ਪਾਠ != ਨਿਰਾ)
	{
	    ਪੂਰਨ i;

	    // Remove "\V"
	    ਜੇ (len >= 2 && STRNCMP(ਪਾਠ, "\\V", 2) == 0)
	    {
		mch_memmove(ਪਾਠ, ਪਾਠ + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\c" and "\C" and remove.
	    ਜੇ (len >= 2 && *ਪਾਠ == '\\' && (ਪਾਠ[1] == 'c' || ਪਾਠ[1] == 'C'))
	    {
		*mcasep = (ਪਾਠ[1] == 'C');
		mch_memmove(ਪਾਠ, ਪਾਠ + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\<text\>" and remove.
	    ਜੇ (len >= 4
		    && STRNCMP(ਪਾਠ, "\\<", 2) == 0
		    && STRNCMP(ਪਾਠ + len - 2, "\\>", 2) == 0)
	    {
		*wwordp = TRUE;
		mch_memmove(ਪਾਠ, ਪਾਠ + 2, (size_t)(len - 4));
		ਪਾਠ[len - 4] = NUL;
	    }

	    // Recognize "\/" or "\?" and remove.
	    ਲਈ (i = 0; i + 1 < len; ++i)
		ਜੇ (ਪਾਠ[i] == '\\' && (ਪਾਠ[i + 1] == '/'
						       || ਪਾਠ[i + 1] == '?'))
		{
		    mch_memmove(ਪਾਠ + i, ਪਾਠ + i + 1, (size_t)(len - i));
		    --len;
		}
	}
    }
    ਵਾਪਸ ਪਾਠ;
}

/*
 * Handle the press of a button in the find-replace dialog.
 * Return TRUE when something was added to the input buffer.
 */
    ਪੂਰਨ
gui_do_findrepl(
    ਪੂਰਨ		flags,		// one of FRD_REPLACE, FRD_FINDNEXT, etc.
    char_u	*ਲੱਭੋ_ਪਾਠ,
    char_u	*repl_ਪਾਠ,
    ਪੂਰਨ		ਥੱਲੇ)		// Search downwards.
{
    garray_T	ga;
    ਪੂਰਨ		i;
    ਪੂਰਨ		type = (flags & FRD_TYPE_ਨਕਾਬ);
    char_u	*p;
    regmatch_T	regmatch;
    ਪੂਰਨ		save_did_emsg = did_emsg;
    ਸਥਿਰ ਪੂਰਨ  busy = FALSE;

    // When the screen is being updated we should not change buffers and
    // windows structures, it may cause freed memory to be used.  Also don't
    // do this recursively (pressing "Find" quickly several times).
    ਜੇ (updating_screen || busy)
	ਵਾਪਸ FALSE;

    // refuse replace when text cannot be changed
    ਜੇ ((type == FRD_REPLACE || type == FRD_REPLACEALL) && ਪਾਠ_locked())
	ਵਾਪਸ FALSE;

    busy = TRUE;

    ga_init2(&ga, 1, 100);
    ਜੇ (type == FRD_REPLACEALL)
	GA_CONCAT_LITERAL(&ga, "%s/");

    GA_CONCAT_LITERAL(&ga, "\\V");
    ਜੇ (flags & FRD_ਮੇਲ_CASE)
	GA_CONCAT_LITERAL(&ga, "\\C");
    ਨਹੀਂਤਾਂ
	GA_CONCAT_LITERAL(&ga, "\\c");
    ਜੇ (flags & FRD_ਸਾਰਾ_ਸ਼ਬਦ)
	GA_CONCAT_LITERAL(&ga, "\\<");
    // escape slash and backslash
    p = vim_strsave_escaped(ਲੱਭੋ_ਪਾਠ, (char_u *)"/\\");
    ਜੇ (p != ਨਿਰਾ)
	ga_concat(&ga, p);
    vim_free(p);
    ਜੇ (flags & FRD_ਸਾਰਾ_ਸ਼ਬਦ)
	GA_CONCAT_LITERAL(&ga, "\\>");

    ਜੇ (type == FRD_REPLACEALL)
    {
	GA_CONCAT_LITERAL(&ga, "/");
	// Escape slash and backslash.
	// Also escape tilde and ampersand if 'magic' is set.
	p = vim_strsave_escaped(repl_ਪਾਠ,
				p_magic ? (char_u *)"/\\~&" : (char_u *)"/\\");
	ਜੇ (p != ਨਿਰਾ)
	    ga_concat(&ga, p);
	vim_free(p);
	GA_CONCAT_LITERAL(&ga, "/g");
    }
    ga_append(&ga, NUL);

    ਜੇ (type == FRD_REPLACE)
    {
	// Do the replacement when the text at the cursor matches.  Thus no
	// replacement is done if the cursor was moved!
	regmatch.regprog = vim_regcomp(ga.ga_ਅੰਕੜੇ, RE_MAGIC + RE_STRING);
	regmatch.rm_ic = 0;
	ਜੇ (regmatch.regprog != ਨਿਰਾ)
	{
	    p = ml_get_cursor();
	    ਜੇ (vim_regexec_nl(&regmatch, p, (colnr_T)0)
						   && regmatch.startp[0] == p)
	    {
		// Clear the command line to remove any old "No match"
		// error.
		ਸੁਨੇਹਾ_ਸਿਰਾ_prompt();

		ਜੇ (u_save_cursor() == OK)
		{
		    // A button was pressed thus undo should be synced.
		    u_sync(FALSE);

		    del_ਬਾਈਟਾਂ((ਲੰਮਾ)(regmatch.endp[0] - regmatch.startp[0]),
								FALSE, FALSE);
		    ins_str(repl_ਪਾਠ, STRLEN(repl_ਪਾਠ));
		}
	    }
	    ਨਹੀਂਤਾਂ
		ਸੁਨੇਹਾ(_("No match at cursor, finding next"));
	    vim_regfree(regmatch.regprog);
	}
    }

    ਜੇ (type == FRD_REPLACEALL)
    {
	// A button was pressed, thus undo should be synced.
	u_sync(FALSE);
	do_cmdline_cmd(ga.ga_ਅੰਕੜੇ);
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ searchflags = ਖੋਜ_ਸੁਨੇਹਾ + ਖੋਜ_MARK;

	// Search for the next match.
	// Don't skip text under cursor for single replace.
	ਜੇ (type == FRD_REPLACE)
	    searchflags += ਖੋਜ_ਸ਼ੁਰੂ;
	i = ਸੁਨੇਹਾ_scroll;
	ਜੇ (ਥੱਲੇ)
	{
	    (ਖਾਲੀ)do_ਖੋਜ(ਨਿਰਾ, '/', '/', ga.ga_ਅੰਕੜੇ, STRLEN(ga.ga_ਅੰਕੜੇ), 1L, searchflags, ਨਿਰਾ);
	}
	ਨਹੀਂਤਾਂ
	{
	    // We need to escape '?' if and only if we are searching in the up
	    // direction
	    p = vim_strsave_escaped(ga.ga_ਅੰਕੜੇ, (char_u *)"?");
	    ਜੇ (p != ਨਿਰਾ)
		(ਖਾਲੀ)do_ਖੋਜ(ਨਿਰਾ, '?', '?', p, STRLEN(p), 1L, searchflags, ਨਿਰਾ);
	    vim_free(p);
	}

	ਸੁਨੇਹਾ_scroll = i;	    // don't let an error message set msg_scroll
    }

    // Don't want to pass did_emsg to other code, it may cause disabling
    // syntax HL if we were busy redrawing.
    did_emsg = save_did_emsg;

    ਜੇ (State & (ਬਹੁਲਕ_ਸਾਧਾਰਨ | ਬਹੁਲਕ_ਪਾਓ))
    {
	gui_update_screen();		// update the screen
	ਸੁਨੇਹਾ_didout = 0;			// overwrite any message
	need_wait_return = FALSE;	// don't wait for return
    }

    vim_free(ga.ga_ਅੰਕੜੇ);
    busy = FALSE;
    ਵਾਪਸ (ga.ga_len > 0);
}

#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_DROP_FILE)
/*
 * Jump to the window at specified point (x, y).
 */
    ਸਥਿਰ ਖਾਲੀ
gui_wingoto_xy(ਪੂਰਨ x, ਪੂਰਨ y)
{
    ਪੂਰਨ		ਪੰਕਤੀ = Y_2_ਪੰਕਤੀ(y);
    ਪੂਰਨ		ਖਾਨਾ = X_2_ਖਾਨਾ(x);
    win_T	*wp;

    ਜੇ (ਪੰਕਤੀ < 0 || ਖਾਨਾ < 0)
	ਵਾਪਸ;

    wp = mouse_ਲੱਭੋ_win(&ਪੰਕਤੀ, &ਖਾਨਾ, FAIL_POPUP);
    ਜੇ (wp != ਨਿਰਾ && wp != curwin)
	win_goto(wp);
}

/*
 * Function passed to handle_drop() for the actions to be done after the
 * argument list has been updated.
 */
    ਸਥਿਰ ਖਾਲੀ
drop_callback(ਖਾਲੀ *cookie)
{
    char_u	*p = cookie;
    ਪੂਰਨ		do_shorten = FALSE;

    // If Shift held down, change to first file's directory.  If the first
    // item is a directory, change to that directory (and let the explorer
    // plugin show the contents).
    ਜੇ (p != ਨਿਰਾ)
    {
	ਜੇ (mch_isdir(p))
	{
	    ਜੇ (mch_chdir((ਅੱਖਰ *)p) == 0)
		do_shorten = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (vim_chdirfile(p, "drop") == OK)
	    do_shorten = TRUE;
	vim_free(p);
	ਜੇ (do_shorten)
	{
	    shorten_fnames(TRUE);
	    ਆਖਰੀ_chdir_reason = "drop";
	}
    }

    // Update the screen display
    update_screen(UPD_NOT_VALID);
# ifdef FEAT_MENU
    gui_update_menus(0);
# endif
    maketitle();
    setcursor();
    ਬਾਹਰਲਾ_flush_cursor(FALSE, FALSE);
}

/*
 * Process file drop.  Mouse cursor position, key modifiers, name of files
 * and count of files are given.  Argument "fnames[count]" has full pathnames
 * of dropped files, they will be freed in this function, and caller can't use
 * fnames after call this function.
 */
    ਖਾਲੀ
gui_handle_drop(
    ਪੂਰਨ		x ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		y ਅਣਵਰਤਿਆ,
    int_u	modifiers,
    char_u	**fnames,
    ਪੂਰਨ		ਗਿਣਤੀ_2)
{
    ਪੂਰਨ		i;
    char_u	*p;
    ਸਥਿਰ ਪੂਰਨ	entered = FALSE;

    /*
     * This function is called by event handlers.  Just in case we get a
     * second event before the first one is handled, ignore the second one.
     * Not sure if this can ever happen, just in case.
     */
    ਜੇ (entered)
	ਵਾਪਸ;
    entered = TRUE;

    /*
     * When the cursor is at the command line, add the file names to the
     * command line, don't edit the files.
     */
    ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
    {
	shorten_filenames(fnames, ਗਿਣਤੀ_2);
	ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
	{
	    ਜੇ (fnames[i] != ਨਿਰਾ)
	    {
		ਜੇ (i > 0)
		    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf((char_u*)" ", 1);

		// We don't know what command is used thus we can't be sure
		// about which characters need to be escaped.  Only escape the
		// most common ones.
# ifdef BACKSLASH_ਵਿੱਚ_FILENAME
		p = vim_strsave_escaped(fnames[i], (char_u *)" \t\"|");
# ਨਹੀਂਤਾਂ
		p = vim_strsave_escaped(fnames[i], (char_u *)"\\ \t\"|");
# endif
		ਜੇ (p != ਨਿਰਾ)
		    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf_csi(p, (ਪੂਰਨ)STRLEN(p));
		vim_free(p);
		vim_free(fnames[i]);
	    }
	}
	vim_free(fnames);
    }
    ਨਹੀਂਤਾਂ
    {
	// Go to the window under mouse cursor, then shorten given "fnames" by
	// current window, because a window can have local current dir.
	gui_wingoto_xy(x, y);
	shorten_filenames(fnames, ਗਿਣਤੀ_2);

	// If Shift held down, remember the first item.
	ਜੇ ((modifiers & MOUSE_ਖਿਸਕਾਓ) != 0)
	    p = vim_strsave(fnames[0]);
	ਨਹੀਂਤਾਂ
	    p = ਨਿਰਾ;

	// Handle the drop, :edit or :split to get to the file.  This also
	// frees fnames[].  Skip this if there is only one item, it's a
	// directory and Shift is held down.
	ਜੇ (ਗਿਣਤੀ_2 == 1 && (modifiers & MOUSE_ਖਿਸਕਾਓ) != 0
						     && mch_isdir(fnames[0]))
	{
	    vim_free(fnames[0]);
	    vim_free(fnames);
	    vim_free(p);
	}
	ਨਹੀਂਤਾਂ
	    handle_drop(ਗਿਣਤੀ_2, fnames, (modifiers & MOUSE_CTRL) != 0,
						     drop_callback, (ਖਾਲੀ *)p);
    }

    entered = FALSE;
}
#ਜੇ_ਅੰਤ

/*
 * Check if "key" is to interrupt us.  Handles a key that has not had modifiers
 * applied yet.
 * Return the key with modifiers applied if so, NUL if not.
 */
    ਪੂਰਨ
check_for_ਰੁਕਾਵਟ(ਪੂਰਨ ਕੁੰਜੀ, ਪੂਰਨ modifiers_arg)
{
    ਪੂਰਨ modifiers = modifiers_arg;
    ਪੂਰਨ c = ਮਿਲਾਓ_modify_Other_ਕੁੰਜੀਆਂ(ਕੁੰਜੀ, &modifiers);

    ਜੇ ((c == Ctrl_C && ctrl_c_interrupts)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	    || (intr_char != Ctrl_C && c == intr_char)
#ਜੇ_ਅੰਤ
	    )
    {
	got_int = TRUE;
	ਵਾਪਸ c;
    }
    ਵਾਪਸ NUL;
}

/*
 * If the "--gui-log-file fname" argument is given write the dialog title and
 * message to a file and return TRUE.  Otherwise return FALSE.
 * When there is any problem opening the file or writing to the file this is
 * ignored, showing the dialog might get the test to get stuck.
 */
    ਪੂਰਨ
gui_dialog_log(char_u *title, char_u *ਸੁਨੇਹਾ_2)
{
    char_u  *fname = get_gui_dialog_file();
    FILE    *fd;

    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    fd = mch_fopen((ਅੱਖਰ *)fname, "a");
    ਜੇ (fd != ਨਿਰਾ)
    {
	fprintf(fd, "%s: %s\n", title, ਸੁਨੇਹਾ_2);
	fclose(fd);
    }
    ਵਾਪਸ TRUE;
}
