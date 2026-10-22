/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

// Structure containing all the GUI information
gui_T gui;

#ಆದರೆ defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
# define USE_ಗಣ_GUIFONTWIDE
ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_guifontwide(char_u *font_name);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ gui_check_pos(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ gui_reset_scroll_region(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ gui_outstr(char_u *, ಪೂರ್ಣಾಂಕ);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_GTK4
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ gui_screenchar(ಪೂರ್ಣಾಂಕ off, ಪೂರ್ಣಾಂಕ flags, guicolor_T fg, guicolor_T bg, ಪೂರ್ಣಾಂಕ ಹಿಂಭಾಗ);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ gui_outstr_nowrap(char_u *s, ಪೂರ್ಣಾಂಕ len, ಪೂರ್ಣಾಂಕ flags, guicolor_T fg, guicolor_T bg, ಪೂರ್ಣಾಂಕ ಹಿಂಭಾಗ);
ಸ್ಥಿರ ಶೂನ್ಯ gui_delete_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2);
ಸ್ಥಿರ ಶೂನ್ಯ gui_ಸೇರಿಸು_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ gui_xy2colrow(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ *colp);
#ಆದರೆ defined(FEAT_GUI_TABLINE)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ gui_has_tabline(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ gui_do_scrollbar(win_T *wp, ಪೂರ್ಣಾಂಕ which, ಪೂರ್ಣಾಂಕ enable);
ಸ್ಥಿರ ಶೂನ್ಯ gui_update_horiz_scrollbar(ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಶೂನ್ಯ gui_ಗಣ_fg_color(char_u *name);
ಸ್ಥಿರ ಶೂನ್ಯ gui_ಗಣ_bg_color(char_u *name);
ಸ್ಥಿರ ಶೂನ್ಯ init_gui_options(ಶೂನ್ಯ);
ಸ್ಥಿರ win_T *xy2win(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, mouse_ಕಂಡುಹಿಡಿ_T popup);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GUI_MAY_FORK
ಸ್ಥಿರ ಶೂನ್ಯ gui_do_fork(ಶೂನ್ಯ);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ gui_read_ಮಗು_pipe(ಪೂರ್ಣಾಂಕ fd);

// Return values for gui_read_child_pipe
ಎಣಿಕೆ {
    GUI_ಮಗು_IO_ERROR,
    GUI_ಮಗು_OK,
    GUI_ಮಗು_FAILED
};
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ gui_attempt_ಆರಂಭ(ಶೂನ್ಯ);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ can_update_cursor = TRUE; // can display the cursor
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ disable_flush = 0;	// If > 0, gui_mch_flush() is disabled.

/*
 * gui_start -- Called when user wants to start the GUI.
 *
 * Careful: This function can be called recursively when there is a ":gui"
 * command in the .gvimrc file.  Only the first call should fork, not the
 * recursive call.
 */
    ಶೂನ್ಯ
gui_ಆರಂಭ(char_u *arg ಬಳಸದ)
{
    char_u	*old_ಪದ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GUI_MAY_FORK
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = 0;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
    ಅಕ್ಷರ	*ಸಂದೇಶ = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

    old_ಪದ = vim_strsave(T_NAME);

    settmode(TMODE_COOK);		// stop RAW mode
    ಆದರೆ (ತುಂಬಿದ_screen)
	cursor_on();			// needed for ":gui" in .vimrc
    ತುಂಬಿದ_screen = FALSE;

    // If GUI fails to start, then we will recover afterwards
    ಪದ_disable_dec();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GUI_MAY_FORK
    ++recursive;
    /*
     * Quit the current process and continue in the child.
     * Makes "gvim file" disconnect from the shell it was started in.
     * Don't do this when Vim was started with "-f" or the 'f' flag is present
     * in 'guioptions'.
     * Don't do this when there is a running job, we can only get the status
     * of a child from the parent.
     */
    ಆದರೆ (gui.dofork && !vim_strchr(p_go, GO_FORG) && recursive <= 1
# ifdef FEAT_ಕೆಲಸ_CHANNEL
	    && !ಕೆಲಸ_any_running()
# endif
	    )
    {
	gui_do_fork();
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GUI_MAY_SPAWN
    ಆದರೆ (gui.dospawn
# ifdef EXPERIMENTAL_GUI_CMD
	    && gui.dofork
# endif
	    && !vim_strchr(p_go, GO_FORG)
	    && !any_Buf_Is_ಬದಲಾಯಿತು()
# ifdef FEAT_ಕೆಲಸ_CHANNEL
	    && !ಕೆಲಸ_any_running()
# endif
	    )
    {
# ifdef EXPERIMENTAL_GUI_CMD
	ಸಂದೇಶ =
# endif
	    gui_mch_do_spawn(arg);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
	// If there is 'f' in 'guioptions' and specify -g argument,
	// gui_mch_init_check() was not called yet.
	ಆದರೆ (gui_mch_init_check() != OK)
	    getout_preserve_modified(1);
#ಆದರೆ_ಅಂತ್ಯ
	gui_attempt_ಆರಂಭ();
    }

    ಆದರೆ (!gui.ಒಳ_use)			// failed to start GUI
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
	termcapinit(old_ಪದ);
	settmode(TMODE_RAW);		// restart RAW mode
	ಗಣ_title_defaults();		// set 'title' and 'icon' again
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	ಪದ_ಗಣ_win_resize(true);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
	ಆದರೆ (ಸಂದೇಶ != ಖಾಲಿ)
	    emsg(ಸಂದೇಶ);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLIPMETHOD
	// Reset clipmethod to CLIPMETHOD_NONE
	ಆರಿಸು_clipmethod();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
	update_cell_ಗಾತ್ರ();
	update_image_backend();
#ಆದರೆ_ಅಂತ್ಯ
    }

#ಆದರೆ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    // Enable fullscreen mode
    ಆದರೆ (vim_strchr(p_go, GO_FULLSCREEN) != ಖಾಲಿ)
       gui_mch_ಗಣ_fullscreen(TRUE);
#ಆದರೆ_ಅಂತ್ಯ

    vim_free(old_ಪದ);

    // If the GUI started successfully, trigger the GUIEnter event, otherwise
    // the GUIFailed event.
    gui_mch_update();
    apply_autocmds(gui.ಒಳ_use ? EVENT_GUIENTER : EVENT_GUIFAILED,
						   ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GUI_MAY_FORK
    --recursive;
#ಆದರೆ_ಅಂತ್ಯ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_attempt_ಆರಂಭ(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ recursive = 0;

    ++recursive;
    gui.starting = true;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
    gui.event_ಸಮಯ = GDK_CURRENT_ಸಮಯ;
#ಆದರೆ_ಅಂತ್ಯ

    termcapinit((char_u *)"builtin_gui");
    gui.starting = recursive - 1;

#ಆದರೆ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
    ಆದರೆ (gui.ಒಳ_use)
    {
# ifdef FEAT_EVAL
	ಕಿಟಕಿ	x11_ಕಿಟಕಿ;
	Display	*x11_display;

	ಆದರೆ (gui_get_x11_windis(&x11_ಕಿಟಕಿ, &x11_display) == OK)
	    ಗಣ_vim_var_nr(VV_WINDOWID, (ದೀರ್ಘ)x11_ಕಿಟಕಿ);
# endif
	// Display error messages in a dialog now.
	display_errors();
    }
#ಆದರೆ_ಅಂತ್ಯ
    --recursive;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GUI_MAY_FORK

// for waitpid()
# ಆದರೆ defined(HAVE_SYS_WAIT_H)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_do_fork(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		pipefd[2];	// pipe between parent and child
    ಪೂರ್ಣಾಂಕ		pipe_error;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2;
    ಪೂರ್ಣಾಂಕ		exit_ಸ್ಥಿತಿ;
    pid_t	pid = -1;

# ಆದರೆ defined(FEAT_RELTIME) && defined(PROF_NSEC)
    // a timer is not carried forward
    delete_timer();
# endif

    // Setup a pipe between the child and the parent, so that the parent
    // knows when the child has done the setsid() call and is allowed to
    // exit.
    pipe_error = (pipe(pipefd) < 0);
    pid = fork();
    ಆದರೆ (pid < 0)	    // Fork error
    {
	emsg(_(e_failed_ಗಮ್ಯ_create_new_process_for_GUI));
	ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pid > 0)	    //ಪೋಷಕ
    {
	// Give the child some time to do the setsid(), otherwise the
	// exit() may kill the child too (when starting gvim from inside a
	// gvim).
	ಆದರೆ (!pipe_error)
	{
	    // The read returns when the child closes the pipe (or when
	    // the child dies for some reason).
	    close(pipefd[1]);
	    ಸ್ಥಿತಿ_2 = gui_read_ಮಗು_pipe(pipefd[0]);
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == GUI_ಮಗು_FAILED)
	    {
		// The child failed to start the GUI, so the caller must
		// continue. There may be more error information written
		// to stderr by the child.
		waitpid(pid, &exit_ಸ್ಥಿತಿ, 0);
		emsg(_(e_the_ಮಗು_process_failed_ಗಮ್ಯ_ಆರಂಭ_GUI));
		ಹಿಂತಿರುಗಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸ್ಥಿತಿ_2 == GUI_ಮಗು_IO_ERROR)
	    {
		pipe_error = TRUE;
	    }
	    // else GUI_CHILD_OK: parent exit
	}

	ಆದರೆ (pipe_error)
	    ui_delay(301L, TRUE);

	// When swapping screens we may need to go to the next line, e.g.,
	// after a hit-enter prompt and using ":gui".
	ಆದರೆ (newline_on_exit)
	    mch_errmsg("\r\n");

	/*
	 * The parent must skip the normal exit() processing, the child
	 * will do it.  For example, GTK messes up signals when exiting.
	 */
	_exit(0);
    }
    //ಮಗು

# ifdef FEAT_GUI_GTK
    // Call gtk_init_check() here after fork(). See gui_init_check().
    ಆದರೆ (gui_mch_init_check() != OK)
	getout_preserve_modified(1);
# endif

# ಆದರೆ defined(HAVE_SETSID) || defined(HAVE_SETPGID)
    /*
     * Change our process group.  On some systems/shells a CTRL-C in the
     * shell where Vim was started would otherwise kill gvim!
     */
#  ಆದರೆ defined(HAVE_SETSID)
    (ಶೂನ್ಯ)setsid();
#  ಇಲ್ಲದಿದ್ದರೆ
    (ಶೂನ್ಯ)setpgid(0, 0);
#  endif
# endif
    ಆದರೆ (!pipe_error)
	close(pipefd[0]);

# ಆದರೆ defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
    // Tell the session manager our new PID
    gui_mch_forked();
# endif

    // Try to start the GUI
    gui_attempt_ಆರಂಭ();

    // Notify the parent
    ಆದರೆ (!pipe_error)
    {
	ಆದರೆ (gui.ಒಳ_use)
	    write_eintr(pipefd[1], "ok", 3);
	ಇಲ್ಲದಿದ್ದರೆ
	    write_eintr(pipefd[1], "fail", 5);
	close(pipefd[1]);
    }

    // If we failed to start the GUI, exit now.
    ಆದರೆ (!gui.ಒಳ_use)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_read_ಮಗು_pipe(ಪೂರ್ಣಾಂಕ fd)
{
    ದೀರ್ಘ	ಬೈಟುಗಳು_read;
# define READ_ಬಫರ್_ಗಾತ್ರ 10
    ಅಕ್ಷರ	ಬಫರ್[READ_ಬಫರ್_ಗಾತ್ರ];

    ಬೈಟುಗಳು_read = read_eintr(fd, ಬಫರ್, READ_ಬಫರ್_ಗಾತ್ರ - 1);
# undef READ_ಬಫರ್_ಗಾತ್ರ
    close(fd);
    ಆದರೆ (ಬೈಟುಗಳು_read < 0)
	ಹಿಂತಿರುಗಿ GUI_ಮಗು_IO_ERROR;
    ಬಫರ್[ಬೈಟುಗಳು_read] = NUL;
    ಆದರೆ (STRCMP(ಬಫರ್, "ok") == 0)
	ಹಿಂತಿರುಗಿ GUI_ಮಗು_OK;
    ಹಿಂತಿರುಗಿ GUI_ಮಗು_FAILED;
}

#ಆದರೆ_ಅಂತ್ಯ // GUI_MAY_FORK

/*
 * Call this when vim starts up, whether or not the GUI is started
 */
    ಶೂನ್ಯ
gui_prepare(ಪೂರ್ಣಾಂಕ *argc, ಅಕ್ಷರ **argv)
{
    gui.ಒಳ_use = false;		    // No GUI yet (maybe later)
    gui.starting = false;	    // No GUI yet (maybe later)
    gui_mch_prepare(argc, argv);
}

/*
 * Try initializing the GUI and check if it can be started.
 * Used from main() to check early if "vim -g" can start the GUI.
 * Used from gui_init() to prepare for starting the GUI.
 * Returns FAIL or OK.
 */
    ಪೂರ್ಣಾಂಕ
gui_init_check(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = MAYBE;

    ಆದರೆ (ಫಲಿತಾಂಶ != MAYBE)
    {
	ಆದರೆ (ಫಲಿತಾಂಶ == FAIL)
	    emsg(_(e_cannot_ಆರಂಭ_the_GUI));
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    gui.shell_created = false;
    gui.dying = false;
    gui.ಒಳ_focus = true;		// so the guicursor setting works
    gui.dragged_sb = SBAR_NONE;
    gui.dragged_wp = ಖಾಲಿ;
    gui.ಸೂಚಕ_hidden = false;
    gui.ಲಂಬಸಾಲು = 0;
    gui.ಅಡ್ಡಸಾಲು = 0;
    gui.ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = Columns;
    gui.ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು;

    gui.cursor_is_valid = false;
    gui.scroll_region_ತುದಿ = 0;
    gui.scroll_region_bot = ಅಡ್ಡಸಾಲುಗಳು - 1;
    gui.scroll_region_ಎಡ = 0;
    gui.scroll_region_ಬಲ = Columns - 1;
    gui.highlight_ಮುಖವಾಡ = HL_ಸಾಮಾನ್ಯ;
    gui.char_ಅಗಲ = 1;
    gui.char_ಎತ್ತರ = 1;
    gui.char_ascent = 0;
    gui.border_ಅಗಲ = 0;

    gui.norm_font = NOFONT;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_GUI_GTK
    gui.bold_font = NOFONT;
    gui.ital_font = NOFONT;
    gui.boldital_font = NOFONT;
# ifdef FEAT_XFONTSET
    gui.fontset = NOFONTSET;
# endif
#ಆದರೆ_ಅಂತ್ಯ
    gui.wide_font = NOFONT;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_GUI_GTK
    gui.wide_bold_font = NOFONT;
    gui.wide_ital_font = NOFONT;
    gui.wide_boldital_font = NOFONT;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
# ifndef FEAT_GUI_GTK
#  ifdef FONTSET_ALWAYS
    gui.menu_fontset = NOFONTSET;
#  ಇಲ್ಲದಿದ್ದರೆ
    gui.menu_font = NOFONT;
#  endif
# endif
    gui.menu_is_ಸಕ್ರಿಯ = true;	    // default: include menu
# ifndef FEAT_GUI_GTK
    gui.menu_ಎತ್ತರ = MENU_DEFAULT_ಎತ್ತರ;
    gui.menu_ಅಗಲ = 0;
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    gui.toolbar_ಎತ್ತರ = 0;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_TIP
    gui.tooltip_fontset = NOFONTSET;
#ಆದರೆ_ಅಂತ್ಯ

    gui.scrollbar_ಅಗಲ = gui.scrollbar_ಎತ್ತರ = SB_DEFAULT_ಅಗಲ;
    gui.prev_wrap = -1;

#ಆದರೆ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // Note: gui_set_ligatures() might already have been called e.g. from .vimrc,
    // and in that case we don't want to overwrite ligatures map that has already
    // been correctly populated (as that would lead to a cleared ligatures maps).
    ಆದರೆ (*p_guiligatures == NUL)
	CLEAR_ಕ್ಷೇತ್ರ(gui.ligatures_map);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(ALWAYS_USE_GUI) || defined(VIMDLL)
    ಫಲಿತಾಂಶ = OK;
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef FEAT_GUI_GTK
    gui.is_x11 = false;
#  ifdef FEAT_GUI_DIALOG
    gui.dialogs_ಸಕ್ರಿಯ = 0;
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
    ಫಲಿತಾಂಶ = gui.dofork ? gui_mch_early_init_check(TRUE) : gui_mch_init_check();
# ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = gui_mch_init_check();
# endif
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * This is the call which starts the GUI.
 */
    ಶೂನ್ಯ
gui_init(ಶೂನ್ಯ)
{
    win_T	*wp;
    tabpage_T	*tp;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = 0;

    /*
     * It's possible to use ":gui" in a .gvimrc file.  The first half of this
     * function will then be executed at the first call, the rest by the
     * recursive call.  This allow the shell to be opened halfway reading a
     * gvimrc file.
     */
    ಆದರೆ (!recursive)
    {
	++recursive;

	clip_init(TRUE);

	// If can't initialize, don't try doing the rest
	ಆದರೆ (gui_init_check() == FAIL)
	{
	    --recursive;
	    clip_init(FALSE);
	    ಹಿಂತಿರುಗಿ;
	}

	/*
	 * Reset 'paste'.  It's useful in the terminal, but not in the GUI.  It
	 * breaks the Paste toolbar button.
	 */
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"paste", 0L, ಖಾಲಿ, 0);

	// Set t_Co to the number of colors: RGB.
	ಗಣ_color_ಎಣಿಕೆ(256 * 256 * 256);

	/*
	 * Set up system-wide default menus.
	 */
#ಆದರೆ defined(SYS_MENU_FILE) && defined(FEAT_MENU)
	ಆದರೆ (vim_strchr(p_go, GO_NOSYSMENU) == ಖಾಲಿ)
	{
	    sys_menu = TRUE;
	    do_ಮೂಲ((char_u *)SYS_MENU_FILE, FALSE, DOSO_NONE, ಖಾಲಿ);
	    sys_menu = FALSE;
	}
#ಆದರೆ_ಅಂತ್ಯ

	/*
	 * Switch on the mouse by default, unless the user changed it already.
	 * This can then be changed in the .gvimrc.
	 */
	ಆದರೆ (!option_was_ಗಣ((char_u *)"mouse"))
	    ಗಣ_string_option_ನೇರ((char_u *)"mouse", -1,
					   (char_u *)"a", OPT_FREE, SID_NONE);

	/*
	 * If -U option given, use only the initializations from that file and
	 * nothing else.  Skip all initializations for "-U NONE" or "-u NORC".
	 */
	ಆದರೆ (use_gvimrc != ಖಾಲಿ)
	{
	    ಆದರೆ (STRCMP(use_gvimrc, "NONE") != 0
		    && STRCMP(use_gvimrc, "NORC") != 0
		    && do_ಮೂಲ(use_gvimrc, FALSE, DOSO_NONE, ಖಾಲಿ) != OK)
		semsg(_(e_cannot_read_from_str), use_gvimrc);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    /*
	     * Get system wide defaults for gvim, only when file name defined.
	     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SYS_GVIMRC_FILE
	    do_ಮೂಲ((char_u *)SYS_GVIMRC_FILE, FALSE, DOSO_NONE, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

	    /*
	     * Try to read GUI initialization commands from the following
	     * places:
	     * - environment variable GVIMINIT
	     * - the user gvimrc file (~/.gvimrc)
	     * - the second user gvimrc file ($VIM/.gvimrc for Dos)
	     * - the third user gvimrc file ($VIM/.gvimrc for Amiga)
	     * The first that exists is used, the rest is ignored.
	     */
	    ಆದರೆ (process_env((char_u *)"GVIMINIT", FALSE) == FAIL
		 && do_ಮೂಲ((char_u *)USR_GVIMRC_FILE, TRUE,
						     DOSO_GVIMRC, ಖಾಲಿ) == FAIL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USR_GVIMRC_FILE2
		 && do_ಮೂಲ((char_u *)USR_GVIMRC_FILE2, TRUE,
						     DOSO_GVIMRC, ಖಾಲಿ) == FAIL
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USR_GVIMRC_FILE3
		 && do_ಮೂಲ((char_u *)USR_GVIMRC_FILE3, TRUE,
						     DOSO_GVIMRC, ಖಾಲಿ) == FAIL
#ಆದರೆ_ಅಂತ್ಯ
				)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USR_GVIMRC_FILE4
		(ಶೂನ್ಯ)do_ಮೂಲ((char_u *)USR_GVIMRC_FILE4, TRUE,
							    DOSO_GVIMRC, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
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
	    ಆದರೆ (p_exrc)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		{
		    stat_T s;

		    // if ".gvimrc" file is not owned by user, set 'secure'
		    //ಬಹುಲಕ
		    ಆದರೆ (mch_stat(GVIMRC_FILE, &s) || s.st_uid != getuid())
			secure = p_secure;
		}
#ಇಲ್ಲದಿದ್ದರೆ
		secure = p_secure;
#ಆದರೆ_ಅಂತ್ಯ

		ಆದರೆ (       fullpathcmp((char_u *)USR_GVIMRC_FILE,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SYS_GVIMRC_FILE
			&& fullpathcmp((char_u *)SYS_GVIMRC_FILE,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USR_GVIMRC_FILE2
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE2,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USR_GVIMRC_FILE3
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE3,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USR_GVIMRC_FILE4
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE4,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#ಆದರೆ_ಅಂತ್ಯ
			)
		    do_ಮೂಲ((char_u *)GVIMRC_FILE, TRUE, DOSO_GVIMRC, ಖಾಲಿ);

		ಆದರೆ (secure == 2)
		    need_wait_return = TRUE;
		secure = 0;
	    }
	}

	ಆದರೆ (need_wait_return || ಸಂದೇಶ_didany)
	    wait_return(TRUE);

	--recursive;
    }

    // If recursive call opened the shell, return here from the first call
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;

    /*
     * Create the GUI shell.
     */
    gui.ಒಳ_use = true;		// Must be set after menus have been set up
#ಆದರೆ defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
    gui.scale = 1.0; // Default value
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (gui_mch_init() == FAIL)
	ಹೋಗು error;

    // Avoid a delay for an error message that was printed in the terminal
    // where Vim was started.
    emsg_on_display = FALSE;
    ಸಂದೇಶ_scrolled = 0;
    clear_sb_ಪಠ್ಯ(TRUE);
    need_wait_return = FALSE;
    ಸಂದೇಶ_didany = FALSE;

    /*
     * Check validity of any generic resources that may have been loaded.
     */
    ಆದರೆ (gui.border_ಅಗಲ < 0)
	gui.border_ಅಗಲ = 0;

    /*
     * Set up the fonts.  First use a font specified with "-fn" or "-font".
     */
    ಆದರೆ (font_ವಾದ != ಖಾಲಿ)
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"gfn",
					       0L, (char_u *)font_ವಾದ, 0);
    ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
	    (*p_guifontset == NUL
	     || gui_init_font(p_guifontset, TRUE) == FAIL) &&
#ಆದರೆ_ಅಂತ್ಯ
	    gui_init_font(*p_guifont == NUL ? hl_get_font_name()
						  : p_guifont, FALSE) == FAIL)
    {
	emsg(_(e_cannot_ಆರಂಭ_gui_no_valid_font_ಸಿಕ್ಕಿತು));
	ಹೋಗು error2;
    }
    ಆದರೆ (gui_get_wide_font() == FAIL)
	emsg(_(e_guifontwide_invalid));

    gui.ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = Columns;
    gui.ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು;
    gui_reset_scroll_region();

    // Create initial scrollbars
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_ಎಡ], SBAR_ಎಡ, wp);
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_ಬಲ], SBAR_ಬಲ, wp);
    }
    gui_create_scrollbar(&gui.bottom_sbar, SBAR_BOTTOM, ಖಾಲಿ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    gui_create_initial_menus(ಬೇರು_menu);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGN_ICONS
    sign_gui_started();
#ಆದರೆ_ಅಂತ್ಯ

    // Configure the desired menu and scrollbars
    gui_init_which_components(ಖಾಲಿ);

    // All components of the GUI have been created now
    gui.shell_created = true;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
    // Set the shell size, adjusted for the screen size.  For GTK this only
    // works after the shell has been opened, thus it is further down.
    // If the window is already maximized (e.g. when --windowid is passed in),
    // we want to use the system-provided dimensions by passing FALSE to
    // mustset. Otherwise, we want to initialize with the default rows/columns.
    ಆದರೆ (gui_mch_maximized())
	gui_ಗಣ_shellsize(FALSE, TRUE, RESIZE_BOTH);
    ಇಲ್ಲದಿದ್ದರೆ
	gui_ಗಣ_shellsize(TRUE, TRUE, RESIZE_BOTH);
#ಇಲ್ಲದಿದ್ದರೆ
# ifndef FEAT_GUI_GTK
    gui_ಗಣ_shellsize(FALSE, TRUE, RESIZE_BOTH);
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    // Need to set the size of the menubar after all the menus have been
    // created.
    gui_mch_compute_menu_ಎತ್ತರ((Widget)0);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Actually open the GUI shell.
     */
    ಆದರೆ (gui_mch_open() != FAIL)
    {
	maketitle();
	resettitle();

	init_gui_options();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
	// Our GUI can't do bidi.
	p_tbidi = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI_GTK)
	// Give GTK+ a chance to put all widget's into place.
	gui_mch_update();

# ifdef FEAT_MENU
	// If there is no 'm' in 'guioptions' we need to remove the menu now.
	// It was still there to make F10 work.
	ಆದರೆ (vim_strchr(p_go, GO_MENUS) == ಖಾಲಿ)
	{
	    bool save_starting = gui.starting;

	    gui.starting = false;
	    gui_mch_enable_menu(FALSE);
	    gui.starting = save_starting;
	    gui_mch_update();
	}
# endif

	// Now make sure the shell fits on the screen.
	ಆದರೆ (gui_mch_maximized())
	    gui_ಗಣ_shellsize(FALSE, TRUE, RESIZE_BOTH);
	ಇಲ್ಲದಿದ್ದರೆ
	    gui_ಗಣ_shellsize(TRUE, TRUE, RESIZE_BOTH);
#ಆದರೆ_ಅಂತ್ಯ
	// When 'lines' was set while starting up the topframe may have to be
	// resized.
	win_new_shellsize();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_GUI
	// Always create the Balloon Evaluation area, but disable it when
	// 'ballooneval' is off.
	ಆದರೆ (balloonEval != ಖಾಲಿ)
	{
# ifdef FEAT_VARTABS
	    vim_free(balloonEval->vts);
# endif
	    vim_free(balloonEval);
	}
	balloon_Eval_For_ಪದ = FALSE;
# ifdef FEAT_GUI_GTK
	balloonEval = gui_mch_create_beval_area(gui.drawarea, ಖಾಲಿ,
						     &general_beval_cb, ಖಾಲಿ);
# ಇಲ್ಲದಿದ್ದರೆ
#  ಆದರೆ defined(FEAT_GUI_MOTIF)
	{
	    ಬಾಹ್ಯ Widget	ಪಠ್ಯ_Area;
	    balloonEval = gui_mch_create_beval_area(ಪಠ್ಯ_Area, ಖಾಲಿ,
						     &general_beval_cb, ಖಾಲಿ);
	}
#  ಇಲ್ಲದಿದ್ದರೆ
#   ifdef FEAT_GUI_MSWIN
	balloonEval = gui_mch_create_beval_area(ಖಾಲಿ, ಖಾಲಿ,
						     &general_beval_cb, ಖಾಲಿ);
#   endif
#  endif
# endif
	ಆದರೆ (!p_beval)
	    gui_mch_disable_beval_area(balloonEval);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_GUI_MSWIN
	// In the GUI modifiers are prepended to keys.
	// Don't do this for MS-Windows yet, it sends CTRL-K without the
	// modifier.
	ನೋಡಿದ_Modify_Other_ಕೀಲಿಗಳು = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	ಆದರೆ (!im_xim_isvalid_imactivate())
	    emsg(_(e_ಮೌಲ್ಯ_ದ_imactivatekey_is_invalid));
#ಆದರೆ_ಅಂತ್ಯ
	// When 'cmdheight' was set during startup it may not have taken
	// effect yet.
	ಆದರೆ (p_ch != 1L)
	    command_ಎತ್ತರ();

	ಹಿಂತಿರುಗಿ;
    }

error2:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_X11
    // undo gui_mch_init()
    gui_mch_uninit();
#ಆದರೆ_ಅಂತ್ಯ

error:
    gui.ಒಳ_use = false;
    clip_init(FALSE);
}


    ಶೂನ್ಯ
gui_exit(ಪೂರ್ಣಾಂಕ rc)
{
    // don't free the fonts, it leads to a BUS error
    // richard@whitequeen.com Jul 99
    free_highlight_fonts();
    gui.ಒಳ_use = false;
    gui_mch_exit(rc);
}

#ಆದರೆ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_PHOTON)
# define NEED_GUI_UPDATE_SCREEN 1
/*
 * Called when the GUI shell is closed by the user.  If there are no changed
 * files Vim exits, otherwise there will be a dialog to ask the user what to
 * do.
 * When this function returns, Vim should NOT exit!
 */
    ಶೂನ್ಯ
gui_shell_closed(ಶೂನ್ಯ)
{
    cmdmod_T	    save_cmdmod = cmdmod;

    ಆದರೆ (before_quit_autocmds(curwin, TRUE, FALSE))
	ಹಿಂತಿರುಗಿ;

    // Only exit when there are no changed files
    exiting = TRUE;
# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    cmdmod.cmod_flags |= CMOD_CONFIRM;
# endif
    // If there are changed buffers, present the user with a dialog if
    // possible, otherwise give an error message.
    ಆದರೆ (!check_ಬದಲಾಯಿತು_any(FALSE, FALSE))
	getout(0);

    exiting = FALSE;
    cmdmod = save_cmdmod;
    gui_update_screen();	// redraw, window may show changed buffer
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set the font.  "font_list" is a comma separated list of font names.  The
 * first font name that works is used.  If none is found, use the default
 * font.
 * If "fontset" is TRUE, the "font_list" is used as one name for the fontset.
 * Return OK when able to set the font.  When it failed FAIL is returned and
 * the fonts are unchanged.
 */
    ಪೂರ್ಣಾಂಕ
gui_init_font(char_u *font_list, ಪೂರ್ಣಾಂಕ fontset ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿಸು FONTLEN 320
    char_u	font_name[FONTLEN];
    ಪೂರ್ಣಾಂಕ		font_list_ಖಾಲಿಗಳು = FALSE;
    ಪೂರ್ಣಾಂಕ		ret = FAIL;

    ಆದರೆ (!gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ FAIL;

    font_name[0] = NUL;
    ಆದರೆ (*font_list == NUL)
	font_list_ಖಾಲಿಗಳು = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
	// When using a fontset, the whole list of fonts is one name.
	ಆದರೆ (fontset)
	    ret = gui_mch_init_font(font_list, TRUE);
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ಆಗ (*font_list != NUL)
	    {
		// Isolate one comma separated font name.
		(ಶೂನ್ಯ)copy_option_part(&font_list, font_name, FONTLEN, ",");

		// Careful!!!  The Win32 version of gui_mch_init_font(), when
		// called with "*" will change p_guifont to the selected font
		// name, which frees the old value.  This makes font_list
		// invalid.  Thus when OK is returned here, font_list must no
		// longer be used!
		ಆದರೆ (gui_mch_init_font(font_name, FALSE) == OK)
		{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ಗಣ_GUIFONTWIDE
		    // If it's a Unicode font, try setting 'guifontwide' to a
		    // similar double-width font.
		    ಆದರೆ ((p_guifontwide == ಖಾಲಿ || *p_guifontwide == NUL)
				&& ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)font_name, "10646") != ಖಾಲಿ)
			ಗಣ_guifontwide(font_name);
#ಆದರೆ_ಅಂತ್ಯ
		    ret = OK;
		    ನಿಲ್ಲಿಸು;
		}
	    }
    }

    ಆದರೆ (ret != OK
	    && STRCMP(font_list, "*") != 0
	    && (font_list_ಖಾಲಿಗಳು || gui.norm_font == NOFONT))
    {
	/*
	 * Couldn't load any font in 'font_list', keep the current font if
	 * there is one.  If 'font_list' is empty, or if there is no current
	 * font, tell gui_mch_init_font() to try to find a font we can load.
	 */
	ret = gui_mch_init_font(ಖಾಲಿ, FALSE);
    }

    ಆದರೆ (ret == OK)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_GUI_GTK
	// Set normal font as current font
# ifdef FEAT_XFONTSET
	ಆದರೆ (gui.fontset != NOFONTSET)
	    gui_mch_ಗಣ_fontset(gui.fontset);
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	    gui_mch_ಗಣ_font(gui.norm_font);
#ಆದರೆ_ಅಂತ್ಯ
	gui_ಗಣ_shellsize(FALSE, TRUE, RESIZE_BOTH);
    }

    ಹಿಂತಿರುಗಿ ret;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ಗಣ_GUIFONTWIDE
/*
 * Try setting 'guifontwide' to a font twice as wide as "name".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_guifontwide(char_u *name)
{
    ಪೂರ್ಣಾಂಕ		i = 0;
    char_u	wide_name[FONTLEN + 10]; // room for 2 * width and '*'
    char_u	*wp = ಖಾಲಿ;
    char_u	*p;
    GuiFont	font;

    wp = wide_name;
    ಗಾಗಿ (p = name; *p != NUL; ++p)
    {
	*wp++ = *p;
	ಆದರೆ (*p == '-')
	{
	    ++i;
	    ಆದರೆ (i == 6)		// font type: change "--" to "-*-"
	    {
		ಆದರೆ (p[1] == '-')
		    *wp++ = '*';
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == 12)	// found the width
	    {
		++p;
		i = getdigits(&p);
		ಆದರೆ (i != 0)
		{
		    // Double the width specification.
		    sprintf((ಅಕ್ಷರ *)wp, "%d%s", i * 2, p);
		    font = gui_mch_get_font(wide_name, FALSE);
		    ಆದರೆ (font != NOFONT)
		    {
			gui_mch_free_font(gui.wide_font);
			gui.wide_font = font;
			ಗಣ_string_option_ನೇರ((char_u *)"gfw", -1,
						      wide_name, OPT_FREE, 0);
		    }
		}
		ನಿಲ್ಲಿಸು;
	    }
	}
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get the font for 'guifontwide'.
 * Return FAIL for an invalid font name.
 */
    ಪೂರ್ಣಾಂಕ
gui_get_wide_font(ಶೂನ್ಯ)
{
    GuiFont	font = NOFONT;
    char_u	font_name[FONTLEN];
    char_u	*p;

    ಆದರೆ (!gui.ಒಳ_use)	    // Can't allocate font yet, assume it's OK.
	ಹಿಂತಿರುಗಿ OK;	    // Will give an error message later.

    ಆದರೆ (p_guifontwide != ಖಾಲಿ && *p_guifontwide != NUL)
    {
	ಗಾಗಿ (p = p_guifontwide; *p != NUL; )
	{
	    // Isolate one comma separated font name.
	    (ಶೂನ್ಯ)copy_option_part(&p, font_name, FONTLEN, ",");
	    font = gui_mch_get_font(font_name, FALSE);
	    ಆದರೆ (font != NOFONT)
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (font == NOFONT)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    gui_mch_free_font(gui.wide_font);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    ಆದರೆ (font != NOFONT && gui.norm_font != NOFONT
			 && pango_font_description_equal(font, gui.norm_font))
    {
	gui.wide_font = NOFONT;
	gui_mch_free_font(font);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	gui.wide_font = font;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
    gui_mch_wide_font_ಬದಲಾಯಿತು();
#ಇಲ್ಲದಿದ್ದರೆ
    /*
     * TODO: setup wide_bold_font, wide_ital_font and wide_boldital_font to
     * support those fonts for 'guifontwide'.
     */
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * Set list of ascii characters that combined can create ligature.
 * Store them in char map for quick access from gui_gtk_draw_string.
 */
    ಶೂನ್ಯ
gui_ಗಣ_ligatures(ಶೂನ್ಯ)
{
    char_u	*p;

    ಆದರೆ (*p_guiligatures != NUL)
    {
	// check for invalid characters
	ಗಾಗಿ (p = p_guiligatures; *p != NUL; ++p)
	    ಆದರೆ (*p < 32 || *p > 127)
	    {
		emsg(_(e_ascii_ಸಂಕೇತ_not_ಒಳ_range));
		ಹಿಂತಿರುಗಿ;
	    }

	// store valid setting into ligatures_map
	CLEAR_ಕ್ಷೇತ್ರ(gui.ligatures_map);
	ಗಾಗಿ (p = p_guiligatures; *p != NUL; ++p)
	    gui.ligatures_map[*p] = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	CLEAR_ಕ್ಷೇತ್ರ(gui.ligatures_map);
}

/*
 * Adjust the columns to undraw for when the cursor is on ligatures.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_adjust_undraw_cursor_for_ligatures(ಪೂರ್ಣಾಂಕ *startcol, ಪೂರ್ಣಾಂಕ *endcol)
{
    ಪೂರ್ಣಾಂಕ off;

    ಆದರೆ (Screen_ಸಾಲುಗಳು == ಖಾಲಿ || *p_guiligatures == NUL)
	ಹಿಂತಿರುಗಿ;

    // expand before the cursor for all the chars in gui.ligatures_map
    off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[gui.cursor_ಅಡ್ಡಸಾಲು] + *startcol;
    ಆದರೆ (gui.ligatures_map[Screen_ಸಾಲುಗಳು[off]])
	ಆಗ (*startcol > 0 && gui.ligatures_map[Screen_ಸಾಲುಗಳು[--off]])
	    (*startcol)--;

    // expand after the cursor for all the chars in gui.ligatures_map
    off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[gui.cursor_ಅಡ್ಡಸಾಲು] + *endcol;
    ಆದರೆ (gui.ligatures_map[Screen_ಸಾಲುಗಳು[off]])
	ಆಗ (*endcol < ((ಪೂರ್ಣಾಂಕ)screen_Columns - 1)
				      && gui.ligatures_map[Screen_ಸಾಲುಗಳು[++off]])
	   (*endcol)++;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ಗಣ_cursor(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    gui.ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
    gui.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
}

/*
 * gui_check_pos - check if the cursor is on the screen.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_check_pos(ಶೂನ್ಯ)
{
    ಆದರೆ (gui.ಅಡ್ಡಸಾಲು >= screen_ಅಡ್ಡಸಾಲುಗಳು)
	gui.ಅಡ್ಡಸಾಲು = screen_ಅಡ್ಡಸಾಲುಗಳು - 1;
    ಆದರೆ (gui.ಲಂಬಸಾಲು >= screen_Columns)
	gui.ಲಂಬಸಾಲು = screen_Columns - 1;
    ಆದರೆ (gui.cursor_ಅಡ್ಡಸಾಲು >= screen_ಅಡ್ಡಸಾಲುಗಳು || gui.cursor_ಲಂಬಸಾಲು >= screen_Columns)
	gui.cursor_is_valid = false;
}

/*
 * Redraw the cursor if necessary or when forced.
 * Careful: The contents of ScreenLines[] must match what is on the screen,
 * otherwise this goes wrong.  May need to call out_flush() first.
 */
    ಶೂನ್ಯ
gui_update_cursor(
    ಪೂರ್ಣಾಂಕ		force,		 // when TRUE, update even when not moved
    ಪೂರ್ಣಾಂಕ		clear_selection) // clear selection under cursor
{
    ಪೂರ್ಣಾಂಕ		cur_ಅಗಲ = 0;
    ಪೂರ್ಣಾಂಕ		cur_ಎತ್ತರ = 0;
    ಪೂರ್ಣಾಂಕ		old_hl_ಮುಖವಾಡ;
    cursorentry_T *shape;
    ಪೂರ್ಣಾಂಕ		id;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    guicolor_T	shape_fg = INVALCOLOR;
    guicolor_T	shape_bg = INVALCOLOR;
#ಆದರೆ_ಅಂತ್ಯ
    guicolor_T	cfg, cbg, cc;	// cursor fore-/background color
    ಪೂರ್ಣಾಂಕ		cattr;		// cursor attributes
    ಪೂರ್ಣಾಂಕ		attr;
    attrentry_T *aep = ಖಾಲಿ;
#ಆದರೆ (defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)) && !defined(USE_GTK4)
    bool	lig_ಎಡ = false, lig_ಬಲ = false;
#ಆದರೆ_ಅಂತ್ಯ

    // Don't update the cursor when halfway busy scrolling or the screen size
    // doesn't match 'columns' and 'lines.  ScreenLines[] isn't valid then.
    ಆದರೆ (!can_update_cursor || screen_Columns != gui.ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ
					       || screen_ಅಡ್ಡಸಾಲುಗಳು != gui.ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು)
	ಹಿಂತಿರುಗಿ;

    gui_check_pos();

    ಆದರೆ (gui.cursor_is_valid && !force
		&& gui.ಅಡ್ಡಸಾಲು == gui.cursor_ಅಡ್ಡಸಾಲು && gui.ಲಂಬಸಾಲು == gui.cursor_ಲಂಬಸಾಲು)
	ಹಿಂತಿರುಗಿ;

    gui_undraw_cursor();

    // If a cursor-less sleep is ongoing, leave the cursor invisible
    ಆದರೆ (cursor_is_sleeping())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (gui.ಅಡ್ಡಸಾಲು < 0)
	ಹಿಂತಿರುಗಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ಒಳಹರಿವು_ವಿಧಾನ
    ಆದರೆ (gui.ಅಡ್ಡಸಾಲು != gui.cursor_ಅಡ್ಡಸಾಲು || gui.ಲಂಬಸಾಲು != gui.cursor_ಲಂಬಸಾಲು)
	im_ಗಣ_position(gui.ಅಡ್ಡಸಾಲು, gui.ಲಂಬಸಾಲು);
#ಆದರೆ_ಅಂತ್ಯ
    gui.cursor_ಅಡ್ಡಸಾಲು = gui.ಅಡ್ಡಸಾಲು;
    gui.cursor_ಲಂಬಸಾಲು = gui.ಲಂಬಸಾಲು;

    // Only write to the screen after ScreenLines[] has been initialized
    ಆದರೆ (!screen_cleared || Screen_ಸಾಲುಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Clear the selection if we are about to write over it
    ಆದರೆ (clear_selection)
	clip_may_clear_selection(gui.ಅಡ್ಡಸಾಲು, gui.ಅಡ್ಡಸಾಲು);
    // Check that the cursor is inside the shell (resizing may have made
    // it invalid)
    ಆದರೆ (gui.ಅಡ್ಡಸಾಲು >= screen_ಅಡ್ಡಸಾಲುಗಳು || gui.ಲಂಬಸಾಲು >= screen_Columns)
	ಹಿಂತಿರುಗಿ;

    gui.cursor_is_valid = true;

    /*
     * How the cursor is drawn depends on the current mode.
     * When in a terminal window use the shape/color specified there.
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಆದರೆ (terminal_is_ಸಕ್ರಿಯ())
	shape = ಪದ_get_cursor_shape(&shape_fg, &shape_bg);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	shape = &shape_ಕೋಷ್ಟಕ[get_shape_idx(FALSE)];
    ಆದರೆ (State & ಬಹುಲಕ_LANGMAP)
	id = shape->id_lm;
    ಇಲ್ಲದಿದ್ದರೆ
	id = shape->id;

    // get the colors and attributes for the cursor.  Default is inverted
    cfg = INVALCOLOR;
    cbg = INVALCOLOR;
    cattr = HL_INVERSE;
    gui_mch_ಗಣ_blinking(shape->blinkwait,
	    shape->blinkon,
	    shape->blinkoff);
    ಆದರೆ (shape->blinkwait == 0 || shape->blinkon == 0
	    || shape->blinkoff == 0)
	gui_mch_stop_blink(FALSE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಆದರೆ (shape_bg != INVALCOLOR)
    {
	cattr = 0;
	cfg = shape_fg;
	cbg = shape_bg;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (id > 0)
	{
	    cattr = syn_id2colors(id, &cfg, &cbg);
#ಆದರೆ defined(HAVE_ಒಳಹರಿವು_ವಿಧಾನ)
	    {
		ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ iid;
		guicolor_T fg, bg;

		ಆದರೆ (
# ಆದರೆ defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
			preedit_get_ಸ್ಥಿತಿ()
# ಇಲ್ಲದಿದ್ದರೆ
			im_get_ಸ್ಥಿತಿ()
# endif
		   )
		{
		    iid = syn_name2id((char_u *)"CursorIM");
		    ಆದರೆ (iid > 0)
		    {
			syn_id2colors(iid, &fg, &bg);
			ಆದರೆ (bg != INVALCOLOR)
			    cbg = bg;
			ಆದರೆ (fg != INVALCOLOR)
			    cfg = fg;
		    }
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}

    /*
     * Get the attributes for the character under the cursor.
     * When no cursor color was given, use the character color.
     */
    attr = ScreenAttrs[ಸಾಲು_ಸ್ಥಾನಾಂತರ[gui.ಅಡ್ಡಸಾಲು] + gui.ಲಂಬಸಾಲು];
    ಆದರೆ (attr > HL_ALL)
	aep = syn_gui_attr2entry(attr);
    ಆದರೆ (aep != ಖಾಲಿ)
    {
	attr = aep->ae_attr;
	ಆದರೆ (cfg == INVALCOLOR)
	    cfg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.bg_color
		    : aep->ae_u.gui.fg_color);
	ಆದರೆ (cbg == INVALCOLOR)
	    cbg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.fg_color
		    : aep->ae_u.gui.bg_color);
    }
    ಆದರೆ (cfg == INVALCOLOR)
	cfg = (attr & HL_INVERSE) ? gui.ಹಿಂಭಾಗ_pixel : gui.norm_pixel;
    ಆದರೆ (cbg == INVALCOLOR)
	cbg = (attr & HL_INVERSE) ? gui.norm_pixel : gui.ಹಿಂಭಾಗ_pixel;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
    ಆದರೆ (aep != ಖಾಲಿ)
    {
	xim_bg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.fg_color
		: aep->ae_u.gui.bg_color);
	xim_fg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.bg_color
		: aep->ae_u.gui.fg_color);
	ಆದರೆ (xim_bg_color == INVALCOLOR)
	    xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
		: gui.ಹಿಂಭಾಗ_pixel;
	ಆದರೆ (xim_fg_color == INVALCOLOR)
	    xim_fg_color = (attr & HL_INVERSE) ? gui.ಹಿಂಭಾಗ_pixel
		: gui.norm_pixel;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
	    : gui.ಹಿಂಭಾಗ_pixel;
	xim_fg_color = (attr & HL_INVERSE) ? gui.ಹಿಂಭಾಗ_pixel
	    : gui.norm_pixel;
    }
#ಆದರೆ_ಅಂತ್ಯ

    attr &= ~HL_INVERSE;
    ಆದರೆ (cattr & HL_INVERSE)
    {
	cc = cbg;
	cbg = cfg;
	cfg = cc;
    }
    cattr &= ~HL_INVERSE;

    /*
     * When we don't have window focus, draw a hollow cursor.
     */
    ಆದರೆ (!gui.ಒಳ_focus)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, -1, -1);
#ಇಲ್ಲದಿದ್ದರೆ
	gui_mch_draw_hollow_cursor(cbg);
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK4
    // Make sure that character underneath is drawn again in case it is part of
    // a ligature.
    gui_redraw_ಖಂಡ(gui.ಅಡ್ಡಸಾಲು, gui.ಲಂಬಸಾಲು, gui.ಅಡ್ಡಸಾಲು, gui.ಲಂಬಸಾಲು, GUI_MON_NOCLEAR);
    // gui_redraw_block() will invalidate the cursor
    gui.cursor_is_valid = true;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // If cursor is in the middle of a ligature, then split the ligature at
    // the cursor boundaries by redrawing those cells again.
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ c = gui.ಲಂಬಸಾಲು - 1; c >= 0; c--)
	ಆದರೆ (!gui.ligatures_map[Screen_ಸಾಲುಗಳು[ಸಾಲು_ಸ್ಥಾನಾಂತರ[gui.ಅಡ್ಡಸಾಲು] + c]])
	{
	    ಆದರೆ (c < gui.ಲಂಬಸಾಲು - 1)
	    {
		gui_redraw_ಖಂಡ(gui.ಅಡ್ಡಸಾಲು, c + 1, gui.ಅಡ್ಡಸಾಲು, gui.ಲಂಬಸಾಲು - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_ಎಡ = true;
# endif
	    }
	    ನಿಲ್ಲಿಸು;
	}

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ c = gui.ಲಂಬಸಾಲು + 1; c < screen_Columns - 1; c++)
	ಆದರೆ (!gui.ligatures_map[Screen_ಸಾಲುಗಳು[ಸಾಲು_ಸ್ಥಾನಾಂತರ[gui.ಅಡ್ಡಸಾಲು] + c]])
	{
	    ಆದರೆ (c > gui.ಲಂಬಸಾಲು + 1)
	    {
		gui_redraw_ಖಂಡ(gui.ಅಡ್ಡಸಾಲು, gui.ಲಂಬಸಾಲು + 1, gui.ಅಡ್ಡಸಾಲು, c - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_ಬಲ = true;
# endif
	    }
	    ನಿಲ್ಲಿಸು;
	}

# ifndef USE_GTK4
    ಆದರೆ ((lig_ಎಡ || lig_ಬಲ) && shape->shape != SHAPE_ಖಂಡ)
    {
	// Because the cursor is not drawn with gui_screenchar(), must blit the
	// cell again (with its background), so that old ligature does not
	// remain on screen. Not needed for GtkSnapshot, because char beneath is
	// always rerendered (see above ifdef).
	ಪೂರ್ಣಾಂಕ old = gui.ಲಂಬಸಾಲು;
	gui.highlight_ಮುಖವಾಡ = ScreenAttrs[ಸಾಲು_ಸ್ಥಾನಾಂತರ[gui.ಅಡ್ಡಸಾಲು] + gui.ಲಂಬಸಾಲು];
	(ಶೂನ್ಯ)gui_screenchar(ಸಾಲು_ಸ್ಥಾನಾಂತರ[gui.ಅಡ್ಡಸಾಲು] + gui.ಲಂಬಸಾಲು,
		GUI_MON_NOCLEAR, (guicolor_T)0, (guicolor_T)0, 0);
	gui.ಲಂಬಸಾಲು = old;
    }
# endif
    // gui_redraw_block()/gui_screenchar() may invalidate the cursor, make sure
    // to validate it again.
    gui.cursor_is_valid = true;
#ಆದರೆ_ಅಂತ್ಯ

    old_hl_ಮುಖವಾಡ = gui.highlight_ಮುಖವಾಡ;
    ಆದರೆ (shape->shape == SHAPE_ಖಂಡ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, 0, 0);
#ಇಲ್ಲದಿದ್ದರೆ
	/*
	 * Draw the text character with the cursor colors.	Use the
	 * character attributes plus the cursor attributes.
	 */
	gui.highlight_ಮುಖವಾಡ = (cattr | attr);
	(ಶೂನ್ಯ)gui_screenchar(ಸಾಲು_ಸ್ಥಾನಾಂತರ[gui.ಅಡ್ಡಸಾಲು] + gui.ಲಂಬಸಾಲು,
		GUI_MON_IS_CURSOR | GUI_MON_NOCLEAR, cfg, cbg, 0);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ಆದರೆ defined(FEAT_RIGHTLEFT)
	ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು_off = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
	/*
	 * First draw the partial cursor, then overwrite with the text
	 * character, using a transparent background.
	 */
	ಆದರೆ (shape->shape == SHAPE_VER)
	{
	    cur_ಎತ್ತರ = gui.char_ಎತ್ತರ;
	    cur_ಅಗಲ = (gui.char_ಅಗಲ * shape->percentage + 99) / 100;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    cur_ಎತ್ತರ = (gui.char_ಎತ್ತರ * shape->percentage + 99) / 100;
	    cur_ಅಗಲ = gui.char_ಅಗಲ;
	}
	ಆದರೆ (has_mbyte && (*mb_off2cells)(ಸಾಲು_ಸ್ಥಾನಾಂತರ[gui.ಅಡ್ಡಸಾಲು] + gui.ಲಂಬಸಾಲು,
		    ಸಾಲು_ಸ್ಥಾನಾಂತರ[gui.ಅಡ್ಡಸಾಲು] + screen_Columns) > 1)
	{
	    // Double wide character.
	    ಆದರೆ (shape->shape != SHAPE_VER)
		cur_ಅಗಲ += gui.char_ಅಗಲ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	    ಆದರೆ (CURSOR_BAR_ಬಲ)
	    {
		// gui.col points to the left half of the character but
		// the vertical line needs to be on the right half.
		// A double-wide horizontal line is also drawn from the
		// right half in gui_mch_draw_part_cursor().
		ಲಂಬಸಾಲು_off = TRUE;
		++gui.ಲಂಬಸಾಲು;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, cur_ಅಗಲ, cur_ಎತ್ತರ);
#ಇಲ್ಲದಿದ್ದರೆ
	gui_mch_draw_part_cursor(cur_ಅಗಲ, cur_ಎತ್ತರ, cbg);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_RIGHTLEFT)
	ಆದರೆ (ಲಂಬಸಾಲು_off)
	    --gui.ಲಂಬಸಾಲು;
#ಆದರೆ_ಅಂತ್ಯ

	// Doesn't seem to work for MSWindows. We call gui_redraw_block() above
	// for GtkSnapshot.
#ಆದರೆ !defined(FEAT_GUI_MSWIN) && !defined(USE_GTK4)
	gui.highlight_ಮುಖವಾಡ = ScreenAttrs[ಸಾಲು_ಸ್ಥಾನಾಂತರ[gui.ಅಡ್ಡಸಾಲು] + gui.ಲಂಬಸಾಲು];
	(ಶೂನ್ಯ)gui_screenchar(ಸಾಲು_ಸ್ಥಾನಾಂತರ[gui.ಅಡ್ಡಸಾಲು] + gui.ಲಂಬಸಾಲು,
		GUI_MON_TRS_CURSOR | GUI_MON_NOCLEAR,
		(guicolor_T)0, (guicolor_T)0, 0);
#ಆದರೆ_ಅಂತ್ಯ
    }
    gui.highlight_ಮುಖವಾಡ = old_hl_ಮುಖವಾಡ;
}

#ಆದರೆ defined(FEAT_MENU)
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_position_menu(ಶೂನ್ಯ)
{
# ಆದರೆ !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
    ಆದರೆ (gui.menu_is_ಸಕ್ರಿಯ && gui.ಒಳ_use)
	gui_mch_ಗಣ_menu_pos(0, 0, gui.menu_ಅಗಲ, gui.menu_ಎತ್ತರ);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Position the various GUI components (text area, menu).  The vertical
 * scrollbars are NOT handled here.  See gui_update_scrollbars().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_position_components(ಪೂರ್ಣಾಂಕ ಒಟ್ಟು_ಅಗಲ ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ	    ಪಠ್ಯ_area_x;
    ಪೂರ್ಣಾಂಕ	    ಪಠ್ಯ_area_y;
    ಪೂರ್ಣಾಂಕ	    ಪಠ್ಯ_area_ಅಗಲ;
    ಪೂರ್ಣಾಂಕ	    ಪಠ್ಯ_area_ಎತ್ತರ;

    // avoid that moving components around generates events
    ++hold_gui_events;

    ಪಠ್ಯ_area_x = 0;
    ಆದರೆ (gui.which_scrollbars[SBAR_ಎಡ])
	ಪಠ್ಯ_area_x += gui.scrollbar_ಅಗಲ;

    ಪಠ್ಯ_area_y = 0;
#ಆದರೆ defined(FEAT_MENU) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    gui.menu_ಅಗಲ = ಒಟ್ಟು_ಅಗಲ;
    ಆದರೆ (gui.menu_is_ಸಕ್ರಿಯ)
	ಪಠ್ಯ_area_y += gui.menu_ಎತ್ತರ;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
    ಆದರೆ (gui_has_tabline())
	ಪಠ್ಯ_area_y += gui.tabline_ಎತ್ತರ;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    ಆದರೆ (vim_strchr(p_go, GO_TOOLBAR) != ಖಾಲಿ)
    {
# ಆದರೆ defined(FEAT_GUI_HAIKU)
	gui_mch_ಗಣ_toolbar_pos(0, ಪಠ್ಯ_area_y,
				gui.menu_ಅಗಲ, gui.toolbar_ಎತ್ತರ);
# endif
	ಪಠ್ಯ_area_y += gui.toolbar_ಎತ್ತರ;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_HAIKU)
    gui_mch_ಗಣ_tabline_pos(0, ಪಠ್ಯ_area_y,
    gui.menu_ಅಗಲ, gui.tabline_ಎತ್ತರ);
    ಆದರೆ (gui_has_tabline())
	ಪಠ್ಯ_area_y += gui.tabline_ಎತ್ತರ;
#ಆದರೆ_ಅಂತ್ಯ

    ಪಠ್ಯ_area_ಅಗಲ = gui.ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * gui.char_ಅಗಲ + gui.border_ಸ್ಥಾನಾಂತರ * 2;
    ಪಠ್ಯ_area_ಎತ್ತರ = gui.ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು * gui.char_ಎತ್ತರ + gui.border_ಸ್ಥಾನಾಂತರ * 2;

    gui_mch_ಗಣ_ಪಠ್ಯ_area_pos(ಪಠ್ಯ_area_x,
			      ಪಠ್ಯ_area_y,
			      ಪಠ್ಯ_area_ಅಗಲ,
			      ಪಠ್ಯ_area_ಎತ್ತರ
#ಆದರೆ defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
				  + xim_get_ಸ್ಥಿತಿ_area_ಎತ್ತರ()
#ಆದರೆ_ಅಂತ್ಯ
			      );
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    gui_position_menu();
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (gui.which_scrollbars[SBAR_BOTTOM])
	gui_mch_ಗಣ_scrollbar_pos(&gui.bottom_sbar,
				  ಪಠ್ಯ_area_x,
				  ಪಠ್ಯ_area_y + ಪಠ್ಯ_area_ಎತ್ತರ
					+ gui_mch_get_scrollbar_ypadding(),
				  ಪಠ್ಯ_area_ಅಗಲ,
				  gui.scrollbar_ಎತ್ತರ);
    gui.ಎಡ_sbar_x = 0;
    gui.ಬಲ_sbar_x = ಪಠ್ಯ_area_x + ಪಠ್ಯ_area_ಅಗಲ
					+ gui_mch_get_scrollbar_xpadding();

    --hold_gui_events;
}

/*
 * Get the width of the widgets and decorations to the side of the text area.
 */
    ಪೂರ್ಣಾಂಕ
gui_get_ತಳ_ಅಗಲ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    ತಳ_ಅಗಲ;

    ತಳ_ಅಗಲ = 2 * gui.border_ಸ್ಥಾನಾಂತರ;
    ಆದರೆ (gui.which_scrollbars[SBAR_ಎಡ])
	ತಳ_ಅಗಲ += gui.scrollbar_ಅಗಲ;
    ಆದರೆ (gui.which_scrollbars[SBAR_ಬಲ])
	ತಳ_ಅಗಲ += gui.scrollbar_ಅಗಲ;
    ಹಿಂತಿರುಗಿ ತಳ_ಅಗಲ;
}

/*
 * Get the height of the widgets and decorations above and below the text area.
 */
    ಪೂರ್ಣಾಂಕ
gui_get_ತಳ_ಎತ್ತರ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    ತಳ_ಎತ್ತರ;

    ತಳ_ಎತ್ತರ = 2 * gui.border_ಸ್ಥಾನಾಂತರ;
    ಆದರೆ (gui.which_scrollbars[SBAR_BOTTOM])
	ತಳ_ಎತ್ತರ += gui.scrollbar_ಎತ್ತರ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
    // We can't take the sizes properly into account until anything is
    // realized.  Therefore we recalculate all the values here just before
    // setting the size. (--mdcki)
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef FEAT_MENU
    ಆದರೆ (gui.menu_is_ಸಕ್ರಿಯ)
	ತಳ_ಎತ್ತರ += gui.menu_ಎತ್ತರ;
# endif
# ifdef FEAT_TOOLBAR
    ಆದರೆ (vim_strchr(p_go, GO_TOOLBAR) != ಖಾಲಿ)
	ತಳ_ಎತ್ತರ += gui.toolbar_ಎತ್ತರ;
# endif
# ಆದರೆ defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    ಆದರೆ (gui_has_tabline())
	ತಳ_ಎತ್ತರ += gui.tabline_ಎತ್ತರ;
# endif
# ಆದರೆ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    ತಳ_ಎತ್ತರ += gui_mch_ಪಠ್ಯ_area_extra_ಎತ್ತರ();
# endif
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ತಳ_ಎತ್ತರ;
}

/*
 * Should be called after the GUI shell has been resized.  Its arguments are
 * the new width and height of the shell in pixels.
 */
    ಶೂನ್ಯ
gui_resize_shell(ಪೂರ್ಣಾಂಕ pixel_ಅಗಲ, ಪೂರ್ಣಾಂಕ pixel_ಎತ್ತರ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	busy = FALSE;

    ಆದರೆ (!gui.shell_created)	    // ignore when still initializing
	ಹಿಂತಿರುಗಿ;

    /*
     * Can't resize the screen while it is being redrawn.  Remember the new
     * size and handle it later.
     */
    ಆದರೆ (updating_screen || busy)
    {
	new_pixel_ಅಗಲ = pixel_ಅಗಲ;
	new_pixel_ಎತ್ತರ = pixel_ಎತ್ತರ;
	ಹಿಂತಿರುಗಿ;
    }

again:
    new_pixel_ಅಗಲ = 0;
    new_pixel_ಎತ್ತರ = 0;
    busy = TRUE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_HAIKU
    vim_ಬೀಗ_screen();
#ಆದರೆ_ಅಂತ್ಯ

    // Flush pending output before redrawing
    ಹೊರಹೋಗು_flush();
#ಆದರೆ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk_init_decor_ಎತ್ತರ();
#ಆದರೆ_ಅಂತ್ಯ

    gui.ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = (pixel_ಅಗಲ - gui_get_ತಳ_ಅಗಲ()) / gui.char_ಅಗಲ;
    gui.ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು = (pixel_ಎತ್ತರ - gui_get_ತಳ_ಎತ್ತರ()) / gui.char_ಎತ್ತರ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK4
    gui_gtk4_update_ಗಾತ್ರ();
#ಆದರೆ_ಅಂತ್ಯ

    gui_position_components(pixel_ಅಗಲ);
    gui_reset_scroll_region();

#ಆದರೆ defined(FEAT_GUI_GTK) && defined(USE_GTK4) && !defined(USE_GTK4)
    // We do not resize the draw area via the "resize" signal. This is because
    // when the window is resized, the form widget is the one that is resized,
    // so let that call gui_resize_shell() which will allocate the surface and
    // allocate the drawing area size/position.
    gui_gtk4_resize(pixel_ಅಗಲ, pixel_ಎತ್ತರ);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * At the "more" and ":confirm" prompt there is no redraw, put the cursor
     * at the last line here (why does it have to be one row too low?).
     */
    ಆದರೆ (State == ಬಹುಲಕ_ASKMORE || State == ಬಹುಲಕ_CONFIRM)
	gui.ಅಡ್ಡಸಾಲು = gui.ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು;

    // Only comparing Rows and Columns may be sufficient, but let's stay on
    // the safe side.
    ಆದರೆ (gui.ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು != screen_ಅಡ್ಡಸಾಲುಗಳು || gui.ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ != screen_Columns
	    || gui.ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು != ಅಡ್ಡಸಾಲುಗಳು || gui.ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ != Columns || gui.force_redraw)
    {
	shell_resized();
	gui.force_redraw = 0;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_HAIKU
    vim_unlock_screen();
#ಆದರೆ_ಅಂತ್ಯ

    gui_update_scrollbars(TRUE);
    gui_update_cursor(FALSE, TRUE);
#ಆದರೆ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(pixel_ಅಗಲ, pixel_ಎತ್ತರ);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
    xim_ಗಣ_ಸ್ಥಿತಿ_area();
#ಆದರೆ_ಅಂತ್ಯ

    busy = FALSE;

    // We may have been called again while redrawing the screen.
    // Need to do it all again with the latest size then.  But only if the size
    // actually changed.
    ಆದರೆ (new_pixel_ಎತ್ತರ)
    {
	ಆದರೆ (pixel_ಅಗಲ == new_pixel_ಅಗಲ && pixel_ಎತ್ತರ == new_pixel_ಎತ್ತರ)
	{
	    new_pixel_ಅಗಲ = 0;
	    new_pixel_ಎತ್ತರ = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    pixel_ಅಗಲ = new_pixel_ಅಗಲ;
	    pixel_ಎತ್ತರ = new_pixel_ಎತ್ತರ;
	    ಹೋಗು again;
	}
    }
}

/*
 * Check if gui_resize_shell() must be called.
 */
    ಶೂನ್ಯ
gui_may_resize_shell(ಶೂನ್ಯ)
{
    ಆದರೆ (new_pixel_ಎತ್ತರ)
    {
	// careful: gui_resize_shell() may postpone the resize again if we
	// were called indirectly by it
	gui_resize_shell(new_pixel_ಅಗಲ, new_pixel_ಎತ್ತರ);
    }
}

    ಪೂರ್ಣಾಂಕ
gui_get_shellsize(ಶೂನ್ಯ)
{
    ಅಡ್ಡಸಾಲುಗಳು = gui.ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು;
    Columns = gui.ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set the size of the Vim shell according to Rows and Columns.
 * If "fit_to_display" is TRUE then the size may be reduced to fit the window
 * on the screen.
 * When "mustset" is TRUE the size was set by the user. When FALSE a UI
 * component was added or removed (e.g., a scrollbar).
 */
    ಶೂನ್ಯ
gui_ಗಣ_shellsize(
    ಪೂರ್ಣಾಂಕ		mustset ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		fit_ಗಮ್ಯ_display,
    ಪೂರ್ಣಾಂಕ		direction)		// RESIZE_HOR, RESIZE_VER
{
    ಪೂರ್ಣಾಂಕ		ತಳ_ಅಗಲ;
    ಪೂರ್ಣಾಂಕ		ತಳ_ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ		ಅಗಲ;
    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ		min_ಅಗಲ;
    ಪೂರ್ಣಾಂಕ		min_ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ		screen_w;
    ಪೂರ್ಣಾಂಕ		screen_h;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
    ಪೂರ್ಣಾಂಕ		un_maximize = mustset;
    ಪೂರ್ಣಾಂಕ		did_adjust = 0;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		x = -1, y = -1;

    ಆದರೆ (!gui.shell_created)
	ಹಿಂತಿರುಗಿ;

#ಆದರೆ defined(MSWIN) || defined(FEAT_GUI_GTK)
    // If not setting to a user specified size and maximized, calculate the
    // number of characters that fit in the maximized window.
    // FIXME: gui_mch_newfont() is called here even when the font hasn't
    // changed at all.  For example, ":set guioptions=k" triggers this path
    // via gui_init_which_components() -> gui_set_shellsize(FALSE, ...).
    // The intent is to keep the window size and recalculate Rows/Columns,
    // which has nothing to do with fonts.  This should be a separate
    // function with a more descriptive name.
    ಆದರೆ (!mustset && (vim_strchr(p_go, GO_KEEPWINSIZE) != ಖಾಲಿ
						       || gui_mch_maximized()))
    {
	gui_mch_newfont();
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ತಳ_ಅಗಲ = gui_get_ತಳ_ಅಗಲ();
    ತಳ_ಎತ್ತರ = gui_get_ತಳ_ಎತ್ತರ();
    ಆದರೆ (fit_ಗಮ್ಯ_display)
	// Remember the original window position.
	(ಶೂನ್ಯ)gui_mch_get_winpos(&x, &y);

    ಅಗಲ = Columns * gui.char_ಅಗಲ + ತಳ_ಅಗಲ;
    ಎತ್ತರ = ಅಡ್ಡಸಾಲುಗಳು * gui.char_ಎತ್ತರ + ತಳ_ಎತ್ತರ;

    ಆದರೆ (fit_ಗಮ್ಯ_display)
    {
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	ಆದರೆ ((direction & RESIZE_HOR) && ಅಗಲ > screen_w)
	{
	    Columns = (screen_w - ತಳ_ಅಗಲ) / gui.char_ಅಗಲ;
	    ಆದರೆ (Columns < MIN_COLUMNS)
		Columns = MIN_COLUMNS;
	    ಅಗಲ = Columns * gui.char_ಅಗಲ + ತಳ_ಅಗಲ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
	    ++did_adjust;
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಆದರೆ ((direction & RESIZE_VERT) && ಎತ್ತರ > screen_h)
	{
	    ಅಡ್ಡಸಾಲುಗಳು = (screen_h - ತಳ_ಎತ್ತರ) / gui.char_ಎತ್ತರ;
	    check_shellsize();
	    ಎತ್ತರ = ಅಡ್ಡಸಾಲುಗಳು * gui.char_ಎತ್ತರ + ತಳ_ಎತ್ತರ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
	    ++did_adjust;
#ಆದರೆ_ಅಂತ್ಯ
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
	ಆದರೆ (did_adjust == 2 || (ಅಗಲ + gui.char_ಅಗಲ >= screen_w
				     && ಎತ್ತರ + gui.char_ಎತ್ತರ >= screen_h))
	    // don't unmaximize if at maximum size
	    un_maximize = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಎಲ್ಲೆ_screen_ಗಾತ್ರ();
    gui.ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = Columns;
    gui.ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK4
    // Keep the drawing area in sync with the size Vim is going to draw.
    gui_gtk4_update_ಗಾತ್ರ();
#ಆದರೆ_ಅಂತ್ಯ

    min_ಅಗಲ = ತಳ_ಅಗಲ + MIN_COLUMNS * gui.char_ಅಗಲ;
    min_ಎತ್ತರ = ತಳ_ಎತ್ತರ + MIN_ಸಾಲುಗಳು * gui.char_ಎತ್ತರ;
    min_ಎತ್ತರ += tabline_ಎತ್ತರ() * gui.char_ಎತ್ತರ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
    ಆದರೆ (un_maximize)
    {
	// If the window size is smaller than the screen unmaximize the
	// window, otherwise resizing won't work.
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	ಆದರೆ ((ಅಗಲ + gui.char_ಅಗಲ < screen_w
				   || ಎತ್ತರ + gui.char_ಎತ್ತರ * 2 < screen_h)
		&& gui_mch_maximized())
	    gui_mch_unmaximize();
    }
#ಆದರೆ_ಅಂತ್ಯ

    gui_mch_ಗಣ_shellsize(ಅಗಲ, ಎತ್ತರ, min_ಅಗಲ, min_ಎತ್ತರ,
					  ತಳ_ಅಗಲ, ತಳ_ಎತ್ತರ, direction);

    ಆದರೆ (fit_ಗಮ್ಯ_display && x >= 0 && y >= 0)
    {
	// Some window managers put the Vim window left of/above the screen.
	// Only change the position if it wasn't already negative before
	// (happens on MS-Windows with a secondary monitor).
	gui_mch_update();
	ಆದರೆ (gui_mch_get_winpos(&x, &y) == OK && (x < 0 || y < 0))
	    gui_mch_ಗಣ_winpos(x < 0 ? 0 : x, y < 0 ? 0 : y);
    }

    gui_position_components(ಅಗಲ);
    gui_update_scrollbars(TRUE);
    gui_reset_scroll_region();

#ಆದರೆ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(ಅಗಲ, ಎತ್ತರ);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Called when Rows and/or Columns has changed.
 */
    ಶೂನ್ಯ
gui_new_shellsize(ಶೂನ್ಯ)
{
    gui_reset_scroll_region();
}

/*
 * Make scroll region cover whole screen.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_reset_scroll_region(ಶೂನ್ಯ)
{
    gui.scroll_region_ತುದಿ = 0;
    gui.scroll_region_bot = gui.ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು - 1;
    gui.scroll_region_ಎಡ = 0;
    gui.scroll_region_ಬಲ = gui.ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ - 1;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ಆರಂಭ_highlight(ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ)
{
    ಆದರೆ (ಮುಖವಾಡ > HL_ALL)		    // highlight code
	gui.highlight_ಮುಖವಾಡ = ಮುಖವಾಡ;
    ಇಲ್ಲದಿದ್ದರೆ			    //ಮುಖವಾಡ
	gui.highlight_ಮುಖವಾಡ |= ಮುಖವಾಡ;
}

    ಶೂನ್ಯ
gui_stop_highlight(ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ)
{
    ಆದರೆ (ಮುಖವಾಡ > HL_ALL)		    // highlight code
	gui.highlight_ಮುಖವಾಡ = HL_ಸಾಮಾನ್ಯ;
    ಇಲ್ಲದಿದ್ದರೆ			    //ಮುಖವಾಡ
	gui.highlight_ಮುಖವಾಡ &= ~ಮುಖವಾಡ;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    ಶೂನ್ಯ
gui_clear_ಖಂಡ(
    ಪೂರ್ಣಾಂಕ	    row1,
    ಪೂರ್ಣಾಂಕ	    col1,
    ಪೂರ್ಣಾಂಕ	    row2,
    ಪೂರ್ಣಾಂಕ	    col2)
{
    // Clear the selection if we are about to write over it
    clip_may_clear_selection(row1, row2);

    gui_mch_clear_ಖಂಡ(row1, col1, row2, col2);

    // Invalidate cursor if it was in this block
    ಆದರೆ (       gui.cursor_ಅಡ್ಡಸಾಲು >= row1 && gui.cursor_ಅಡ್ಡಸಾಲು <= row2
	    && gui.cursor_ಲಂಬಸಾಲು >= col1 && gui.cursor_ಲಂಬಸಾಲು <= col2)
	gui.cursor_is_valid = false;
}

/*
 * Write code to update the cursor later.  This avoids the need to flush the
 * output buffer before calling gui_update_cursor().
 */
    ಶೂನ್ಯ
gui_update_cursor_later(ಶೂನ್ಯ)
{
    ಹೊರಹೋಗು_STR("\033|s");
}

    ಶೂನ್ಯ
gui_write(
    char_u	*s,
    ಪೂರ್ಣಾಂಕ		len)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		arg1 = 0, arg2 = 0;
    ಪೂರ್ಣಾಂಕ		force_cursor = FALSE;	// force cursor update
    ಪೂರ್ಣಾಂಕ		force_scrollbar = FALSE;
    ಸ್ಥಿರ win_T	*old_curwin = ಖಾಲಿ;

// #define DEBUG_GUI_WRITE
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_GUI_WRITE
    {
	ಪೂರ್ಣಾಂಕ i;
	char_u *str;

	ಬರೆ("gui_write(%d):\n    ", len);
	ಗಾಗಿ (i = 0; i < len; i++)
	    ಆದರೆ (s[i] == ESC)
	    {
		ಆದರೆ (i != 0)
		    ಬರೆ("\n    ");
		ಬರೆ("<ESC>");
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		str = transchar_byte(s[i]);
		ಆದರೆ (str[0] && str[1])
		    ಬರೆ("<%s>", (ಅಕ್ಷರ *)str);
		ಇಲ್ಲದಿದ್ದರೆ
		    ಬರೆ("%s", (ಅಕ್ಷರ *)str);
	    }
	ಬರೆ("\n");
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆಗ (len)
    {
	ಆದರೆ (s[0] == ESC && s[1] == '|')
	{
	    p = s + 2;
	    ಆದರೆ (VIM_ISDIGIT(*p) || (*p == '-' && VIM_ISDIGIT(*(p + 1))))
	    {
		arg1 = getdigits(&p);
		ಆದರೆ (p > s + len)
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (*p == ';')
		{
		    ++p;
		    arg2 = getdigits(&p);
		    ಆದರೆ (p > s + len)
			ನಿಲ್ಲಿಸು;
		}
	    }
	    ಆಯ್ಕೆ (*p)
	    {
		ಸ್ಥಿತಿ 'C':	// Clear screen
		    clip_scroll_selection(9999);
		    gui_mch_clear_all();
		    gui.cursor_is_valid = false;
		    force_scrollbar = TRUE;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'M':	// Move cursor
		    gui_ಗಣ_cursor(arg1, arg2);
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 's':	// force cursor (shape) update
		    force_cursor = TRUE;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'R':	// Set scroll region
		    ಆದರೆ (arg1 < arg2)
		    {
			gui.scroll_region_ತುದಿ = arg1;
			gui.scroll_region_bot = arg2;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			gui.scroll_region_ತುದಿ = arg2;
			gui.scroll_region_bot = arg1;
		    }
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'V':	// Set vertical scroll region
		    ಆದರೆ (arg1 < arg2)
		    {
			gui.scroll_region_ಎಡ = arg1;
			gui.scroll_region_ಬಲ = arg2;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			gui.scroll_region_ಎಡ = arg2;
			gui.scroll_region_ಬಲ = arg1;
		    }
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'd':	// Delete line
		    gui_delete_ಸಾಲುಗಳು(gui.ಅಡ್ಡಸಾಲು, 1);
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'D':	// Delete lines
		    gui_delete_ಸಾಲುಗಳು(gui.ಅಡ್ಡಸಾಲು, arg1);
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'i':	// Insert line
		    gui_ಸೇರಿಸು_ಸಾಲುಗಳು(gui.ಅಡ್ಡಸಾಲು, 1);
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'I':	// Insert lines
		    gui_ಸೇರಿಸು_ಸಾಲುಗಳು(gui.ಅಡ್ಡಸಾಲು, arg1);
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '$':	// Clear to end-of-line
		    gui_clear_ಖಂಡ(gui.ಅಡ್ಡಸಾಲು, gui.ಲಂಬಸಾಲು, gui.ಅಡ್ಡಸಾಲು,
							    (ಪೂರ್ಣಾಂಕ)Columns - 1);
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'h':	// Turn on highlighting
		    gui_ಆರಂಭ_highlight(arg1);
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'H':	// Turn off highlighting
		    gui_stop_highlight(arg1);
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'f':	// flash the window (visual bell)
		    gui_mch_flash(arg1 == 0 ? 20 : arg1);
		    ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		    p = s + 1;	// Skip the ESC
		    ನಿಲ್ಲಿಸು;
	    }
	    len -= (ಪೂರ್ಣಾಂಕ)(++p - s);
	    s = p;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s[0] < 0x20		// Ctrl character
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGN_ICONS
		&& s[0] != SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
		&& s[0] != MULTISIGN_BYTE
# endif
#ಆದರೆ_ಅಂತ್ಯ
		)
	{
	    ಆದರೆ (s[0] == '\n')		// NL
	    {
		gui.ಲಂಬಸಾಲು = 0;
		ಆದರೆ (gui.ಅಡ್ಡಸಾಲು < gui.scroll_region_bot)
		    gui.ಅಡ್ಡಸಾಲು++;
		ಇಲ್ಲದಿದ್ದರೆ
		    gui_delete_ಸಾಲುಗಳು(gui.scroll_region_ತುದಿ, 1);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s[0] == '\r')	// CR
	    {
		gui.ಲಂಬಸಾಲು = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s[0] == '\b')	// Backspace
	    {
		ಆದರೆ (gui.ಲಂಬಸಾಲು)
		    --gui.ಲಂಬಸಾಲು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s[0] == Ctrl_L)	// cursor-right
	    {
		++gui.ಲಂಬಸಾಲು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s[0] == Ctrl_G)	// Beep
	    {
		gui_mch_beep();
	    }
	    // Other Ctrl character: shouldn't happen!

	    --len;	// Skip this char
	    ++s;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    p = s;
	    ಆಗ (len > 0 && (
			*p >= 0x20
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGN_ICONS
			|| *p == SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
			|| *p == MULTISIGN_BYTE
# endif
#ಆದರೆ_ಅಂತ್ಯ
			))
	    {
		len--;
		p++;
	    }
	    gui_outstr(s, (ಪೂರ್ಣಾಂಕ)(p - s));
	    s = p;
	}
    }

    // Postponed update of the cursor (won't work if "can_update_cursor" isn't
    // set).
    ಆದರೆ (force_cursor)
	gui_update_cursor(TRUE, TRUE);

    // When switching to another window the dragging must have stopped.
    // Required for GTK, dragged_sb isn't reset.
    ಆದರೆ (old_curwin != curwin)
	gui.dragged_sb = SBAR_NONE;

    // Update the scrollbars after clearing the screen or when switched
    // to another window.
    // Update the horizontal scrollbar always, it's difficult to check all
    // situations where it might change.
    ಆದರೆ (force_scrollbar || old_curwin != curwin)
	gui_update_scrollbars(force_scrollbar);
    ಇಲ್ಲದಿದ್ದರೆ
	gui_update_horiz_scrollbar(FALSE);
    old_curwin = curwin;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#ಆದರೆ defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#ಆದರೆ_ಅಂತ್ಯ

    gui_may_flush();		    // In case vim decides to take a nap
}

/*
 * When ScreenLines[] is invalid, updating the cursor should not be done, it
 * produces wrong results.  Call gui_dont_update_cursor() before that code and
 * gui_can_update_cursor() afterwards.
 */
    ಶೂನ್ಯ
gui_dont_update_cursor(ಪೂರ್ಣಾಂಕ undraw)
{
    ಆದರೆ (!gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;

    // Undraw the cursor now, we probably can't do it after the change.
    ಆದರೆ (undraw)
	gui_undraw_cursor();
    can_update_cursor = FALSE;
}

    ಶೂನ್ಯ
gui_can_update_cursor(ಶೂನ್ಯ)
{
    can_update_cursor = TRUE;
    // No need to update the cursor right now, there is always more output
    // after scrolling.
}

/*
 * Disable issuing gui_mch_flush().
 */
    ಶೂನ್ಯ
gui_disable_flush(ಶೂನ್ಯ)
{
    ++disable_flush;
}

/*
 * Enable issuing gui_mch_flush().
 */
    ಶೂನ್ಯ
gui_enable_flush(ಶೂನ್ಯ)
{
    --disable_flush;
}

/*
 * Issue gui_mch_flush() if it is not disabled.
 */
    ಶೂನ್ಯ
gui_may_flush(ಶೂನ್ಯ)
{
    ಆದರೆ (disable_flush == 0)
	gui_mch_flush();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_outstr(char_u *s, ಪೂರ್ಣಾಂಕ len)
{
    ಪೂರ್ಣಾಂಕ	    this_len;
    ಪೂರ್ಣಾಂಕ	    cells;

    ಆದರೆ (len == 0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (len < 0)
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(s);

    ಆಗ (len > 0)
    {
	ಆದರೆ (has_mbyte)
	{
	    // Find out how many chars fit in the current line.
	    cells = 0;
	    ಗಾಗಿ (this_len = 0; this_len < len; )
	    {
		cells += (*mb_ptr2cells)(s + this_len);
		ಆದರೆ (gui.ಲಂಬಸಾಲು + cells > Columns)
		    ನಿಲ್ಲಿಸು;
		this_len += (*mb_ptr2len)(s + this_len);
	    }
	    ಆದರೆ (this_len > len)
		this_len = len;	    // don't include following composing char
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (gui.ಲಂಬಸಾಲು + len > Columns)
	    this_len = Columns - gui.ಲಂಬಸಾಲು;
	ಇಲ್ಲದಿದ್ದರೆ
	    this_len = len;

	(ಶೂನ್ಯ)gui_outstr_nowrap(s, this_len,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	s += this_len;
	len -= this_len;
	// fill up for a double-width char that doesn't fit.
	ಆದರೆ (len > 0 && gui.ಲಂಬಸಾಲು < Columns)
	    (ಶೂನ್ಯ)gui_outstr_nowrap((char_u *)" ", 1,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	// The cursor may wrap to the next line.
	ಆದರೆ (gui.ಲಂಬಸಾಲು >= Columns)
	{
	    gui.ಲಂಬಸಾಲು = 0;
	    gui.ಅಡ್ಡಸಾಲು++;
	}
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_GTK4
/*
 * Output one character (may be one or two display cells).
 * Caller must check for valid "off".
 * Returns FAIL or OK, just like gui_outstr_nowrap().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_screenchar(
    ಪೂರ್ಣಾಂಕ		off,	    // Offset from start of screen
    ಪೂರ್ಣಾಂಕ		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    ಪೂರ್ಣಾಂಕ		ಹಿಂಭಾಗ)	    // backup this many chars when using bold trick
{
    char_u	buf[MB_MAXBYTES + 1];

    // Don't draw right half of a double-width UTF-8 char. "cannot happen"
    ಆದರೆ (enc_utf8 && Screen_ಸಾಲುಗಳು[off] == 0)
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0)
	// Draw UTF-8 multi-byte character.
	ಹಿಂತಿರುಗಿ gui_outstr_nowrap(buf, utfc_char2bytes(off, buf),
							 flags, fg, bg, ಹಿಂಭಾಗ);

    ಆದರೆ (enc_dbcs == DBCS_JPNU && Screen_ಸಾಲುಗಳು[off] == 0x8e)
    {
	buf[0] = Screen_ಸಾಲುಗಳು[off];
	buf[1] = ScreenLines2[off];
	ಹಿಂತಿರುಗಿ gui_outstr_nowrap(buf, 2, flags, fg, bg, ಹಿಂಭಾಗ);
    }

    // Draw non-multi-byte character or DBCS character.
    ಹಿಂತಿರುಗಿ gui_outstr_nowrap(Screen_ಸಾಲುಗಳು + off,
	    enc_dbcs ? (*mb_ptr2len)(Screen_ಸಾಲುಗಳು + off) : 1,
							 flags, fg, bg, ಹಿಂಭಾಗ);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
/*
 * Output the string at the given screen position.  This is used in place
 * of gui_screenchar() where possible because Pango needs as much context
 * as possible to work nicely.  It's a lot faster as well.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_screenstr(
    ಪೂರ್ಣಾಂಕ		off,	    // Offset from start of screen
    ಪೂರ್ಣಾಂಕ		len,	    // string length in screen cells
    ಪೂರ್ಣಾಂಕ		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    ಪೂರ್ಣಾಂಕ		ಹಿಂಭಾಗ)	    // backup this many chars when using bold trick
{
    char_u  *buf;
    ಪೂರ್ಣಾಂಕ	    outlen = 0;
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    retval;

    ಆದರೆ (len <= 0) // "cannot happen"?
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (enc_utf8)
    {
	buf = alloc(len * MB_MAXBYTES + 1);
	ಆದರೆ (buf == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ OK; // not much we could do here...

	ಗಾಗಿ (i = off; i < off + len; ++i)
	{
	    ಆದರೆ (Screen_ಸಾಲುಗಳು[i] == 0)
		ಮುಂದುವರಿ; // skip second half of double-width char

	    ಆದರೆ (Screen_ಸಾಲುಗಳು_UC[i] == 0)
		buf[outlen++] = Screen_ಸಾಲುಗಳು[i];
	    ಇಲ್ಲದಿದ್ದರೆ
		outlen += utfc_char2bytes(i, buf + outlen);
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, ಹಿಂಭಾಗ);
	vim_free(buf);

	ಹಿಂತಿರುಗಿ retval;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_dbcs == DBCS_JPNU)
    {
	buf = alloc(len * 2 + 1);
	ಆದರೆ (buf == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ OK; // not much we could do here...

	ಗಾಗಿ (i = off; i < off + len; ++i)
	{
	    buf[outlen++] = Screen_ಸಾಲುಗಳು[i];

	    // handle double-byte single-width char
	    ಆದರೆ (Screen_ಸಾಲುಗಳು[i] == 0x8e)
		buf[outlen++] = ScreenLines2[i];
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (MB_BYTE2LEN(Screen_ಸಾಲುಗಳು[i]) == 2)
		buf[outlen++] = Screen_ಸಾಲುಗಳು[++i];
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, ಹಿಂಭಾಗ);
	vim_free(buf);

	ಹಿಂತಿರುಗಿ retval;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಹಿಂತಿರುಗಿ gui_outstr_nowrap(&Screen_ಸಾಲುಗಳು[off], len,
				 flags, fg, bg, ಹಿಂಭಾಗ);
    }
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_GTK

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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_outstr_nowrap(
    char_u	*s,
    ಪೂರ್ಣಾಂಕ		len,
    ಪೂರ್ಣಾಂಕ		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    ಪೂರ್ಣಾಂಕ		ಹಿಂಭಾಗ)	    // backup this many chars when using bold trick
{
    long_u	highlight_ಮುಖವಾಡ;
    long_u	hl_ಮುಖವಾಡ_todo;
    guicolor_T	fg_color;
    guicolor_T	bg_color;
    guicolor_T	sp_color;
#ಆದರೆ !defined(FEAT_GUI_GTK)
    GuiFont	font = NOFONT;
    GuiFont	wide_font = NOFONT;
# ifdef FEAT_XFONTSET
    GuiFontset	fontset = NOFONTSET;
# endif
#ಆದರೆ_ಅಂತ್ಯ
    attrentry_T	*aep = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		draw_flags;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = gui.ಲಂಬಸಾಲು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGN_ICONS
    ಪೂರ್ಣಾಂಕ		draw_sign = FALSE;
    ಪೂರ್ಣಾಂಕ		signcol = 0;
    char_u	extra[18];
# ifdef FEAT_NETBEANS_INTG
    ಪೂರ್ಣಾಂಕ		multi_sign = FALSE;
# endif
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (len < 0)
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(s);
    ಆದರೆ (len == 0)
	ಹಿಂತಿರುಗಿ OK;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGN_ICONS
    ಆದರೆ (*s == SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
	  || *s == MULTISIGN_BYTE
# endif
       )
    {
	size_t	slen = 2;		    // 2 spaces by default

# ifdef FEAT_NETBEANS_INTG
	ಆದರೆ (*s == MULTISIGN_BYTE)
	    multi_sign = TRUE;
# endif
	// draw spaces instead
	ಆದರೆ (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u' &&
		(curwin->w_p_nu || curwin->w_p_rnu))
	{
	    ಪೂರ್ಣಾಂಕ	n = ಸಂಖ್ಯೆ_ಅಗಲ(curwin);

	    slen = MIN(n, (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(extra) - 1);
	}
	vim_memset(extra, ' ', slen);
	extra[slen] = NUL;
	s = extra;
	ಆದರೆ (len == 1 && ಲಂಬಸಾಲು > 0)
	    --ಲಂಬಸಾಲು;
	len = (ಪೂರ್ಣಾಂಕ)slen;
	ಆದರೆ (len > 2)
	    // right align sign icon in the number column
	    signcol = ಲಂಬಸಾಲು + len - 3;
	ಇಲ್ಲದಿದ್ದರೆ
	    signcol = ಲಂಬಸಾಲು;
	draw_sign = TRUE;
	highlight_ಮುಖವಾಡ = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (gui.highlight_ಮುಖವಾಡ > HL_ALL)
    {
	aep = syn_gui_attr2entry(gui.highlight_ಮುಖವಾಡ);
	ಆದರೆ (aep == ಖಾಲಿ)	    // highlighting not set
	    highlight_ಮುಖವಾಡ = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    highlight_ಮುಖವಾಡ = aep->ae_attr;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	highlight_ಮುಖವಾಡ = gui.highlight_ಮುಖವಾಡ;
    hl_ಮುಖವಾಡ_todo = highlight_ಮುಖವಾಡ;

#ಆದರೆ !defined(FEAT_GUI_GTK)
    // Set the font
    ಆದರೆ (aep != ಖಾಲಿ && aep->ae_u.gui.font != NOFONT)
	font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (aep != ಖಾಲಿ && aep->ae_u.gui.fontset != NOFONTSET)
	fontset = aep->ae_u.gui.fontset;
# endif
    ಇಲ್ಲದಿದ್ದರೆ
    {
# ifdef FEAT_XFONTSET
	ಆದರೆ (gui.fontset != NOFONTSET)
	    fontset = gui.fontset;
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	    ಆದರೆ (hl_ಮುಖವಾಡ_todo & (HL_BOLD | HL_STANDOUT))
	{
	    ಆದರೆ ((hl_ಮುಖವಾಡ_todo & HL_ITALIC) && gui.boldital_font != NOFONT)
	    {
		font = gui.boldital_font;
		hl_ಮುಖವಾಡ_todo &= ~(HL_BOLD | HL_STANDOUT | HL_ITALIC);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (gui.bold_font != NOFONT)
	    {
		font = gui.bold_font;
		hl_ಮುಖವಾಡ_todo &= ~(HL_BOLD | HL_STANDOUT);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		font = gui.norm_font;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((hl_ಮುಖವಾಡ_todo & HL_ITALIC) && gui.ital_font != NOFONT)
	{
	    font = gui.ital_font;
	    hl_ಮುಖವಾಡ_todo &= ~HL_ITALIC;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    font = gui.norm_font;

	/*
	 * Choose correct wide_font by font.  wide_font should be set with font
	 * at same time in above block.  But it will make many "ifdef" nasty
	 * blocks.  So we do it here.
	 */
	ಆದರೆ (font == gui.boldital_font && gui.wide_boldital_font)
	    wide_font = gui.wide_boldital_font;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (font == gui.bold_font && gui.wide_bold_font)
	    wide_font = gui.wide_bold_font;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (font == gui.ital_font && gui.wide_ital_font)
	    wide_font = gui.wide_ital_font;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (font == gui.norm_font && gui.wide_font)
	    wide_font = gui.wide_font;
    }
# ifdef FEAT_XFONTSET
    ಆದರೆ (fontset != NOFONTSET)
	gui_mch_ಗಣ_fontset(fontset);
    ಇಲ್ಲದಿದ್ದರೆ
# endif
	gui_mch_ಗಣ_font(font);
#ಆದರೆ_ಅಂತ್ಯ

    draw_flags = 0;

    // Set the color
    bg_color = gui.ಹಿಂಭಾಗ_pixel;
    ಆದರೆ ((flags & GUI_MON_IS_CURSOR) && gui.ಒಳ_focus)
    {
	draw_flags |= DRAW_CURSOR;
	fg_color = fg;
	bg_color = bg;
	sp_color = fg;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (aep != ಖಾಲಿ)
    {
	fg_color = aep->ae_u.gui.fg_color;
	ಆದರೆ (fg_color == INVALCOLOR)
	    fg_color = gui.norm_pixel;
	bg_color = aep->ae_u.gui.bg_color;
	ಆದರೆ (bg_color == INVALCOLOR)
	    bg_color = gui.ಹಿಂಭಾಗ_pixel;
	sp_color = aep->ae_u.gui.sp_color;
	ಆದರೆ (sp_color == INVALCOLOR)
	    sp_color = fg_color;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	fg_color = gui.norm_pixel;
	sp_color = fg_color;
    }

    ಆದರೆ (highlight_ಮುಖವಾಡ & (HL_INVERSE | HL_STANDOUT))
    {
	gui_mch_ಗಣ_fg_color(bg_color);
	gui_mch_ಗಣ_bg_color(fg_color);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	gui_mch_ಗಣ_fg_color(fg_color);
	gui_mch_ಗಣ_bg_color(bg_color);
    }
    gui_mch_ಗಣ_sp_color(sp_color);

    // Clear the selection if we are about to write over it
    ಆದರೆ (!(flags & GUI_MON_NOCLEAR))
	clip_may_clear_selection(gui.ಅಡ್ಡಸಾಲು, gui.ಅಡ್ಡಸಾಲು);


    // If there's no bold font, then fake it
    ಆದರೆ (hl_ಮುಖವಾಡ_todo & (HL_BOLD | HL_STANDOUT))
	draw_flags |= DRAW_BOLD;

    /*
     * When drawing bold or italic characters the spill-over from the left
     * neighbor may be destroyed.  Let the caller backup to start redrawing
     * just after a blank.
     */
    ಆದರೆ (ಹಿಂಭಾಗ != 0 && ((draw_flags & DRAW_BOLD) || (highlight_ಮುಖವಾಡ & HL_ITALIC)))
	ಹಿಂತಿರುಗಿ FAIL;

#ಆದರೆ defined(FEAT_GUI_GTK)
    // If there's no italic font, then fake it.
    // For GTK2, we don't need a different font for italic style.
    ಆದರೆ (hl_ಮುಖವಾಡ_todo & HL_ITALIC)
	draw_flags |= DRAW_ITALIC;

    // Do we underline the text?
    ಆದರೆ (hl_ಮುಖವಾಡ_todo & HL_UNDERLINE)
	draw_flags |= DRAW_UNDERL;

#ಇಲ್ಲದಿದ್ದರೆ
    // Do we underline the text?
    ಆದರೆ ((hl_ಮುಖವಾಡ_todo & HL_UNDERLINE) || (hl_ಮುಖವಾಡ_todo & HL_ITALIC))
	draw_flags |= DRAW_UNDERL;
#ಆದರೆ_ಅಂತ್ಯ
    // Do we undercurl the text?
    ಆದರೆ (hl_ಮುಖವಾಡ_todo & HL_UNDERCURL)
	draw_flags |= DRAW_UNDERC;

    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

    // Do we strikethrough the text?
    ಆದರೆ (hl_ಮುಖವಾಡ_todo & HL_STRIKETHROUGH)
	draw_flags |= DRAW_STRIKE;

    // Do we draw transparently?
    ಆದರೆ (flags & GUI_MON_TRS_CURSOR)
	draw_flags |= DRAW_TRANSP;

    /*
     * Draw the text.
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
    // The value returned is the length in display cells
    len = gui_gtk_draw_string(gui.ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, s, len, draw_flags);
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (enc_utf8)
    {
	ಪೂರ್ಣಾಂಕ	ಆರಂಭ;		// index of bytes to be drawn
	ಪೂರ್ಣಾಂಕ	cells;		// cellwidth of bytes to be drawn
	ಪೂರ್ಣಾಂಕ	thislen;	// length of bytes to be drawn
	ಪೂರ್ಣಾಂಕ	cn;		// cellwidth of current char
	ಪೂರ್ಣಾಂಕ	i;		// index of current char
	ಪೂರ್ಣಾಂಕ	c;		// current char value
	ಪೂರ್ಣಾಂಕ	cl;		// byte length of current char
	ಪೂರ್ಣಾಂಕ	comping;	// current char is composing
	ಪೂರ್ಣಾಂಕ	scol = ಲಂಬಸಾಲು;	// screen column
	ಪೂರ್ಣಾಂಕ	curr_wide = FALSE;  // use 'guifontwide'
	ಪೂರ್ಣಾಂಕ	prev_wide = FALSE;
	ಪೂರ್ಣಾಂಕ	wide_ಬದಲಾಯಿತು;
# ifdef MSWIN
	ಪೂರ್ಣಾಂಕ	sep_comp = FALSE;   // Don't separate composing chars.
# ಇಲ್ಲದಿದ್ದರೆ
	ಪೂರ್ಣಾಂಕ	sep_comp = TRUE;    // Separate composing chars.
# endif

	// Break the string at a composing character, it has to be drawn on
	// top of the previous character.
	ಆರಂಭ = 0;
	cells = 0;
	ಗಾಗಿ (i = 0; i < len; i += cl)
	{
	    c = utf_ptr2char(s + i);
	    cn = utf_char2cells(c);
	    comping = utf_iscomposing(c);
	    ಆದರೆ (!comping)	// count cells from non-composing chars
		cells += cn;
	    ಆದರೆ (!comping || sep_comp)
	    {
		ಆದರೆ (cn > 1
# ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
# endif
			&& wide_font != NOFONT)
		    curr_wide = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		    curr_wide = FALSE;
	    }
	    cl = utf_ptr2len(s + i);
	    ಆದರೆ (cl == 0)	// hit end of string
		len = i + cl;	// len must be wrong "cannot happen"

	    wide_ಬದಲಾಯಿತು = curr_wide != prev_wide;

	    // Print the string so far if it's the last character or there is
	    // a composing character.
	    ಆದರೆ (i + cl >= len || (comping && sep_comp && i > ಆರಂಭ)
		    || wide_ಬದಲಾಯಿತು
# ಆದರೆ defined(FEAT_GUI_X11)
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
		ಆದರೆ ((comping && sep_comp) || wide_ಬದಲಾಯಿತು)
		    thislen = i - ಆರಂಭ;
		ಇಲ್ಲದಿದ್ದರೆ
		    thislen = i - ಆರಂಭ + cl;
		ಆದರೆ (thislen > 0)
		{
		    ಆದರೆ (prev_wide)
			gui_mch_ಗಣ_font(wide_font);
		    gui_mch_draw_string(gui.ಅಡ್ಡಸಾಲು, scol, s + ಆರಂಭ, thislen,
								  draw_flags);
		    ಆದರೆ (prev_wide)
			gui_mch_ಗಣ_font(font);
		    ಆರಂಭ += thislen;
		}
		scol += cells;
		cells = 0;
		// Adjust to not draw a character which width is changed
		// against with last one.
		ಆದರೆ (wide_ಬದಲಾಯಿತು && !(comping && sep_comp))
		{
		    scol -= cn;
		    cl = 0;
		}

# ಆದರೆ defined(FEAT_GUI_X11)
		// No fontset: draw a space to fill the gap after a wide char
		//
		ಆದರೆ (cn > 1 && (draw_flags & DRAW_TRANSP) == 0
#  ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
#  endif
			&& !wide_ಬದಲಾಯಿತು)
		    gui_mch_draw_string(gui.ಅಡ್ಡಸಾಲು, scol - 1, (char_u *)" ",
							       1, draw_flags);
# endif
	    }
	    // Draw a composing char on top of the previous char.
	    ಆದರೆ (comping && sep_comp)
	    {
# ಆದರೆ defined(__APPLE_CC__) && ಗುರಿ_API_MAC_CARBON
		// Carbon ATSUI autodraws composing char over previous char
		gui_mch_draw_string(gui.ಅಡ್ಡಸಾಲು, scol, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# ಇಲ್ಲದಿದ್ದರೆ
		gui_mch_draw_string(gui.ಅಡ್ಡಸಾಲು, scol - cn, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# endif
		ಆರಂಭ = i + cl;
	    }
	    prev_wide = curr_wide;
	}
	// The stuff below assumes "len" is the length in screen columns.
	len = scol - ಲಂಬಸಾಲು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	gui_mch_draw_string(gui.ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, s, len, draw_flags);
	ಆದರೆ (enc_dbcs == DBCS_JPNU)
	{
	    // Get the length in display cells, this can be different from the
	    // number of bytes for "euc-jp".
	    len = mb_string2cells(s, len);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ // !FEAT_GUI_GTK

    ಆದರೆ (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
	gui.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು + len;

    // May need to invert it when it's part of the selection.
    ಆದರೆ (flags & GUI_MON_NOCLEAR)
	clip_may_redraw_selection(gui.ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, len);

    ಆದರೆ (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
    {
	// Invalidate the old physical cursor position if we wrote over it
	ಆದರೆ (gui.cursor_ಅಡ್ಡಸಾಲು == gui.ಅಡ್ಡಸಾಲು
		&& gui.cursor_ಲಂಬಸಾಲು >= ಲಂಬಸಾಲು
		&& gui.cursor_ಲಂಬಸಾಲು < ಲಂಬಸಾಲು + len)
	    gui.cursor_is_valid = false;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGN_ICONS
    ಆದರೆ (draw_sign)
	// Draw the sign on top of the spaces.
	gui_mch_drawsign(gui.ಅಡ್ಡಸಾಲು, signcol, gui.highlight_ಮುಖವಾಡ);
# ಆದರೆ defined(FEAT_NETBEANS_INTG) && (defined(FEAT_GUI_X11) \
	|| defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN))
    ಆದರೆ (multi_sign)
	netbeans_draw_multisign_indicator(gui.ಅಡ್ಡಸಾಲು);
# endif
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Undraw the cursor.  This actually redraws the character at the cursor
 * position, plus some more characters when needed.
 */
    ಶೂನ್ಯ
gui_undraw_cursor(ಶೂನ್ಯ)
{
    ಆದರೆ (!gui.cursor_is_valid)
	ಹಿಂತಿರುಗಿ;

    // Always redraw the character just before if there is one, because
    // with some fonts and characters there can be a one pixel overlap.
    ಪೂರ್ಣಾಂಕ startcol = gui.cursor_ಲಂಬಸಾಲು > 0 ? gui.cursor_ಲಂಬಸಾಲು - 1 : gui.cursor_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ endcol = gui.cursor_ಲಂಬಸಾಲು;

#ಆದರೆ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    gui_adjust_undraw_cursor_for_ligatures(&startcol, &endcol);
#ಆದರೆ_ಅಂತ್ಯ
    gui_redraw_ಖಂಡ(gui.cursor_ಅಡ್ಡಸಾಲು, startcol,
	    gui.cursor_ಅಡ್ಡಸಾಲು, endcol, GUI_MON_NOCLEAR);

    // Cursor_is_valid is reset when the cursor is undrawn, also reset it
    // here in case it wasn't needed to undraw it.
    gui.cursor_is_valid = false;
}

    ಶೂನ್ಯ
gui_redraw(
    ಪೂರ್ಣಾಂಕ		x,
    ಪೂರ್ಣಾಂಕ		y,
    ಪೂರ್ಣಾಂಕ		w,
    ಪೂರ್ಣಾಂಕ		h)
{
    ಪೂರ್ಣಾಂಕ		row1, col1, row2, col2;

    row1 = Y_2_ಅಡ್ಡಸಾಲು(y);
    col1 = X_2_ಲಂಬಸಾಲು(x);
    row2 = Y_2_ಅಡ್ಡಸಾಲು(y + h - 1);
    col2 = X_2_ಲಂಬಸಾಲು(x + w - 1);

    gui_redraw_ಖಂಡ(row1, col1, row2, col2, GUI_MON_NOCLEAR);

    /*
     * We may need to redraw the cursor, but don't take it upon us to change
     * its location after a scroll.
     * (maybe be more strict even and test col too?)
     * These things may be outside the update/clipping region and reality may
     * not reflect Vims internal ideas if these operations are clipped away.
     */
    ಆದರೆ (gui.ಅಡ್ಡಸಾಲು == gui.cursor_ಅಡ್ಡಸಾಲು)
	gui_update_cursor(TRUE, TRUE);
}

/*
 * Draw a rectangular block of characters, from row1 to row2 (inclusive) and
 * from col1 to col2 (inclusive).
 */
    ಶೂನ್ಯ
gui_redraw_ಖಂಡ(
    ಪೂರ್ಣಾಂಕ		row1,
    ಪೂರ್ಣಾಂಕ		col1,
    ಪೂರ್ಣಾಂಕ		row2,
    ಪೂರ್ಣಾಂಕ		col2,
    ಪೂರ್ಣಾಂಕ		flags)	// flags for gui_outstr_nowrap()
{
    ಪೂರ್ಣಾಂಕ		old_ಅಡ್ಡಸಾಲು, old_ಲಂಬಸಾಲು;
    long_u	old_hl_ಮುಖವಾಡ;
    ಪೂರ್ಣಾಂಕ		off;
    sattr_T	ಮೊದಲ_attr;
    ಪೂರ್ಣಾಂಕ		idx, len;
    ಪೂರ್ಣಾಂಕ		ಹಿಂಭಾಗ, nback;
    ಪೂರ್ಣಾಂಕ		orig_col1, orig_col2;

    // Don't try to update when ScreenLines is not valid
    ಆದರೆ (!screen_cleared || Screen_ಸಾಲುಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Don't try to draw outside the shell!
    // Check everything, strange values may be caused by a big border width
    col1 = check_ಲಂಬಸಾಲು(col1);
    col2 = check_ಲಂಬಸಾಲು(col2);
    row1 = check_ಅಡ್ಡಸಾಲು(row1);
    row2 = check_ಅಡ್ಡಸಾಲು(row2);

    // Remember where our cursor was
    old_ಅಡ್ಡಸಾಲು = gui.ಅಡ್ಡಸಾಲು;
    old_ಲಂಬಸಾಲು = gui.ಲಂಬಸಾಲು;
    old_hl_ಮುಖವಾಡ = gui.highlight_ಮುಖವಾಡ;
    orig_col1 = col1;
    orig_col2 = col2;

    ಗಾಗಿ (gui.ಅಡ್ಡಸಾಲು = row1; gui.ಅಡ್ಡಸಾಲು <= row2; gui.ಅಡ್ಡಸಾಲು++)
    {
	// When only half of a double-wide character is in the block, include
	// the other half.
	col1 = orig_col1;
	col2 = orig_col2;
	off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[gui.ಅಡ್ಡಸಾಲು];
	ಆದರೆ (enc_dbcs != 0)
	{
	    ಆದರೆ (col1 > 0)
		col1 -= dbcs_screen_ತಲೆ_off(Screen_ಸಾಲುಗಳು + off,
						    Screen_ಸಾಲುಗಳು + off + col1);
	    col2 += dbcs_screen_ಬಾಲ_off(Screen_ಸಾಲುಗಳು + off,
						    Screen_ಸಾಲುಗಳು + off + col2);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_utf8)
	{
	    ಆದರೆ (Screen_ಸಾಲುಗಳು[off + col1] == 0)
	    {
		ಆದರೆ (col1 > 0)
		    --col1;
		ಇಲ್ಲದಿದ್ದರೆ
		    // FIXME: how can the first character ever be zero?
		    siemsg("NUL in ScreenLines in row %ld", (ದೀರ್ಘ)gui.ಅಡ್ಡಸಾಲು);
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
	    ಆದರೆ (col2 + 1 < Columns && Screen_ಸಾಲುಗಳು[off + col2 + 1] == 0)
		++col2;
#ಆದರೆ_ಅಂತ್ಯ
	}
	gui.ಲಂಬಸಾಲು = col1;
	off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[gui.ಅಡ್ಡಸಾಲು] + gui.ಲಂಬಸಾಲು;
	len = col2 - col1 + 1;

	// Find how many chars back this highlighting starts, or where a space
	// is.  Needed for when the bold trick is used
	ಗಾಗಿ (ಹಿಂಭಾಗ = 0; ಹಿಂಭಾಗ < col1; ++ಹಿಂಭಾಗ)
	    ಆದರೆ (ScreenAttrs[off - 1 - ಹಿಂಭಾಗ] != ScreenAttrs[off]
		    || Screen_ಸಾಲುಗಳು[off - 1 - ಹಿಂಭಾಗ] == ' ')
		ನಿಲ್ಲಿಸು;

	// Break it up in strings of characters with the same attributes.
	// Print UTF-8 characters individually.
	ಆಗ (len > 0)
	{
	    ಮೊದಲ_attr = ScreenAttrs[off];
	    gui.highlight_ಮುಖವಾಡ = ಮೊದಲ_attr;
#ಆದರೆ !defined(FEAT_GUI_GTK)
	    ಆದರೆ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0)
	    {
		// output multi-byte character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, ಹಿಂಭಾಗ);
		ಆದರೆ (gui.ಲಂಬಸಾಲು < Columns && Screen_ಸಾಲುಗಳು[off + 1] == 0)
		    idx = 2;
		ಇಲ್ಲದಿದ್ದರೆ
		    idx = 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_dbcs == DBCS_JPNU && Screen_ಸಾಲುಗಳು[off] == 0x8e)
	    {
		// output double-byte, single-width character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, ಹಿಂಭಾಗ);
		idx = 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
		ಗಾಗಿ (idx = 0; idx < len; ++idx)
		{
		    ಆದರೆ (enc_utf8 && Screen_ಸಾಲುಗಳು[off + idx] == 0)
			ಮುಂದುವರಿ; // skip second half of double-width char
		    ಆದರೆ (ScreenAttrs[off + idx] != ಮೊದಲ_attr)
			ನಿಲ್ಲಿಸು;
		}
		// gui_screenstr() takes care of multibyte chars
		nback = gui_screenstr(off, idx, flags,
				      (guicolor_T)0, (guicolor_T)0, ಹಿಂಭಾಗ);
#ಇಲ್ಲದಿದ್ದರೆ
		ಗಾಗಿ (idx = 0; idx < len && ScreenAttrs[off + idx] == ಮೊದಲ_attr;
									idx++)
		{
		    // Stop at a multi-byte Unicode character.
		    ಆದರೆ (enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off + idx] != 0)
			ನಿಲ್ಲಿಸು;
		    ಆದರೆ (enc_dbcs == DBCS_JPNU)
		    {
			// Stop at a double-byte single-width char.
			ಆದರೆ (Screen_ಸಾಲುಗಳು[off + idx] == 0x8e)
			    ನಿಲ್ಲಿಸು;
			ಆದರೆ (len > 1 && (*mb_ptr2len)(Screen_ಸಾಲುಗಳು
							    + off + idx) == 2)
			    ++idx;  // skip second byte of double-byte char
		    }
		}
		nback = gui_outstr_nowrap(Screen_ಸಾಲುಗಳು + off, idx, flags,
					  (guicolor_T)0, (guicolor_T)0, ಹಿಂಭಾಗ);
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಆದರೆ (nback == FAIL)
	    {
		// Must back up to start drawing where a bold or italic word
		// starts.
		off -= ಹಿಂಭಾಗ;
		len += ಹಿಂಭಾಗ;
		gui.ಲಂಬಸಾಲು -= ಹಿಂಭಾಗ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		off += idx;
		len -= idx;
	    }
	    ಹಿಂಭಾಗ = 0;
	}
    }

    // Put the cursor back where it was
    gui.ಅಡ್ಡಸಾಲು = old_ಅಡ್ಡಸಾಲು;
    gui.ಲಂಬಸಾಲು = old_ಲಂಬಸಾಲು;
    gui.highlight_ಮುಖವಾಡ = (ಪೂರ್ಣಾಂಕ)old_hl_ಮುಖವಾಡ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_delete_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    ಆದರೆ (ಎಣಿಕೆ_2 <= 0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಅಡ್ಡಸಾಲು + ಎಣಿಕೆ_2 > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_ಖಂಡ(ಅಡ್ಡಸಾಲು, gui.scroll_region_ಎಡ,
			      gui.scroll_region_bot, gui.scroll_region_ಬಲ);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	gui_mch_delete_ಸಾಲುಗಳು(ಅಡ್ಡಸಾಲು, ಎಣಿಕೆ_2);

	// If the cursor was in the deleted lines it's now gone.  If the
	// cursor was in the scrolled lines adjust its position.
	ಆದರೆ (gui.cursor_ಅಡ್ಡಸಾಲು >= ಅಡ್ಡಸಾಲು
		&& gui.cursor_ಲಂಬಸಾಲು >= gui.scroll_region_ಎಡ
		&& gui.cursor_ಲಂಬಸಾಲು <= gui.scroll_region_ಬಲ)
	{
	    ಆದರೆ (gui.cursor_ಅಡ್ಡಸಾಲು < ಅಡ್ಡಸಾಲು + ಎಣಿಕೆ_2)
		gui.cursor_is_valid = false;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (gui.cursor_ಅಡ್ಡಸಾಲು <= gui.scroll_region_bot)
		gui.cursor_ಅಡ್ಡಸಾಲು -= ಎಣಿಕೆ_2;
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ಸೇರಿಸು_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    ಆದರೆ (ಎಣಿಕೆ_2 <= 0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಅಡ್ಡಸಾಲು + ಎಣಿಕೆ_2 > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_ಖಂಡ(ಅಡ್ಡಸಾಲು, gui.scroll_region_ಎಡ,
			      gui.scroll_region_bot, gui.scroll_region_ಬಲ);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	gui_mch_ಸೇರಿಸು_ಸಾಲುಗಳು(ಅಡ್ಡಸಾಲು, ಎಣಿಕೆ_2);

	ಆದರೆ (gui.cursor_ಅಡ್ಡಸಾಲು >= gui.ಅಡ್ಡಸಾಲು
		&& gui.cursor_ಲಂಬಸಾಲು >= gui.scroll_region_ಎಡ
		&& gui.cursor_ಲಂಬಸಾಲು <= gui.scroll_region_ಬಲ)
	{
	    ಆದರೆ (gui.cursor_ಅಡ್ಡಸಾಲು <= gui.scroll_region_bot - ಎಣಿಕೆ_2)
		gui.cursor_ಅಡ್ಡಸಾಲು += ಎಣಿಕೆ_2;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (gui.cursor_ಅಡ್ಡಸಾಲು <= gui.scroll_region_bot)
		gui.cursor_is_valid = false;
	}
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
/*
 * Passed to ui_wait_for_chars_or_timer(), ignoring extra arguments.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_wait_for_chars_3(
    ದೀರ್ಘ wtime,
    ಪೂರ್ಣಾಂಕ *interrupted ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ ignore_ಒಳಹರಿವು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ gui_mch_wait_for_chars(wtime);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_wait_for_chars_or_timer(
	ದೀರ್ಘ wtime,
	ಪೂರ್ಣಾಂಕ *interrupted ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ ignore_ಒಳಹರಿವು ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
    ಹಿಂತಿರುಗಿ ui_wait_for_chars_or_timer(wtime, gui_wait_for_chars_3,
						    interrupted, ignore_ಒಳಹರಿವು);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ gui_mch_wait_for_chars(wtime);
#ಆದರೆ_ಅಂತ್ಯ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_wait_for_chars_buf(
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		maxlen,
    ದೀರ್ಘ	wtime,	    // don't use "time", MIPS cannot handle it
    ಪೂರ್ಣಾಂಕ		tb_change_cnt)
{
    ಪೂರ್ಣಾಂಕ	    retval;
    ಪೂರ್ಣಾಂಕ	    keep_blinking; // Guard against restarting blink cycle on CursorHold

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    // If we're going to wait a bit, update the menus and mouse shape for the
    // current State.
    ಆದರೆ (wtime != 0)
	gui_update_menus(0);
#ಆದರೆ_ಅಂತ್ಯ

    gui_mch_update();
    ಆದರೆ (ಒಳಹರಿವು_ಲಭ್ಯ())	// Got char, return immediately
    {
	ಆದರೆ (gui_mch_is_blinking())
	    gui_mch_stop_blink(TRUE);
	ಆದರೆ (buf != ಖಾಲಿ && !typebuf_ಬದಲಾಯಿತು(tb_change_cnt))
	    ಹಿಂತಿರುಗಿ read_from_ಒಳಹರಿವು_buf(buf, (ದೀರ್ಘ)maxlen);
	ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (wtime == 0)		// Don't wait for char
	ಹಿಂತಿರುಗಿ FAIL;

    // Before waiting, flush any output to the screen.
    gui_mch_flush();

    // Blink while waiting for a character.
    ಆದರೆ (!gui_mch_is_blinking())
	gui_mch_ಆರಂಭ_blink();

    // Common function to loop until "wtime" is met, while handling timers and
    // other callbacks.
    retval = inchar_loop(buf, maxlen, wtime, tb_change_cnt,
			 gui_wait_for_chars_or_timer, ಖಾಲಿ);

    // Keep blinking when CursorHold wakes the input loop. (See PR #21115)
    keep_blinking = retval == 3 && buf != ಖಾಲಿ
	&& buf[0] == K_SPECIAL && buf[1] == KS_EXTRA
	&& buf[2] == (ಪೂರ್ಣಾಂಕ)KE_CURSORHOLD;

    ಆದರೆ (!keep_blinking)
	gui_mch_stop_blink(TRUE);

    ಹಿಂತಿರುಗಿ retval;
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
    ಪೂರ್ಣಾಂಕ
gui_wait_for_chars(ದೀರ್ಘ wtime, ಪೂರ್ಣಾಂಕ tb_change_cnt)
{
    ಹಿಂತಿರುಗಿ gui_wait_for_chars_buf(ಖಾಲಿ, 0, wtime, tb_change_cnt);
}

/*
 * Equivalent of mch_inchar() for the GUI.
 */
    ಪೂರ್ಣಾಂಕ
gui_inchar(
    char_u  *buf,
    ಪೂರ್ಣಾಂಕ	    maxlen,
    ದೀರ್ಘ    wtime,		// milliseconds
    ಪೂರ್ಣಾಂಕ	    tb_change_cnt)
{
    ಹಿಂತಿರುಗಿ gui_wait_for_chars_buf(buf, maxlen, wtime, tb_change_cnt);
}

/*
 * Fill p[4] with mouse coordinates encoded for check_termcode().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fill_mouse_coord(char_u *p, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು)
{
    p[0] = (char_u)(ಲಂಬಸಾಲು / 128 + ' ' + 1);
    p[1] = (char_u)(ಲಂಬಸಾಲು % 128 + ' ' + 1);
    p[2] = (char_u)(ಅಡ್ಡಸಾಲು / 128 + ' ' + 1);
    p[3] = (char_u)(ಅಡ್ಡಸಾಲು % 128 + ' ' + 1);
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
    ಶೂನ್ಯ
gui_send_mouse_event(
    ಪೂರ್ಣಾಂಕ	    button,
    ಪೂರ್ಣಾಂಕ	    x,
    ಪೂರ್ಣಾಂಕ	    y,
    ಪೂರ್ಣಾಂಕ	    repeated_click,
    int_u   modifiers)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    prev_ಅಡ್ಡಸಾಲು = 0, prev_ಲಂಬಸಾಲು = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    prev_button = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    ಅಂಕೆ_clicks = 1;
    char_u	    string[10];
    ಎಣಿಕೆ ಕೀಲಿ_extra  button_char;
    ಪೂರ್ಣಾಂಕ		    ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    ಪೂರ್ಣಾಂಕ		    checkfor;
    ಪೂರ್ಣಾಂಕ		    did_clip = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Scrolling may happen at any time, also while a selection is present.
     */
    ಆಯ್ಕೆ (button)
    {
	ಸ್ಥಿತಿ MOUSE_MOVE:
	    button_char = KE_MOUSEMOVE_XY;
	    ಹೋಗು button_ಗಣ;
	ಸ್ಥಿತಿ MOUSE_X1:
	    button_char = KE_X1MOUSE;
	    ಹೋಗು button_ಗಣ;
	ಸ್ಥಿತಿ MOUSE_X2:
	    button_char = KE_X2MOUSE;
	    ಹೋಗು button_ಗಣ;
	ಸ್ಥಿತಿ MOUSE_4:
	    button_char = KE_MOUSEDOWN;
	    ಹೋಗು button_ಗಣ;
	ಸ್ಥಿತಿ MOUSE_5:
	    button_char = KE_MOUSEUP;
	    ಹೋಗು button_ಗಣ;
	ಸ್ಥಿತಿ MOUSE_6:
	    button_char = KE_MOUSELEFT;
	    ಹೋಗು button_ಗಣ;
	ಸ್ಥಿತಿ MOUSE_7:
	    button_char = KE_MOUSERIGHT;
button_ಗಣ:
	    {
		// Don't put events in the input queue now.
		ಆದರೆ (hold_gui_events)
		    ಹಿಂತಿರುಗಿ;

		ಅಡ್ಡಸಾಲು = gui_xy2colrow(x, y, &ಲಂಬಸಾಲು);
		// Don't report a mouse move unless moved to a
		// different character position.
		ಆದರೆ (button == MOUSE_MOVE)
		{
		    ಆದರೆ (ಅಡ್ಡಸಾಲು == prev_ಅಡ್ಡಸಾಲು && ಲಂಬಸಾಲು == prev_ಲಂಬಸಾಲು)
			ಹಿಂತಿರುಗಿ;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			prev_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು >= 0 ? ಅಡ್ಡಸಾಲು : 0;
			prev_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
		    }
		}

		string[3] = CSI;
		string[4] = KS_EXTRA;
		string[5] = (ಪೂರ್ಣಾಂಕ)button_char;

		// Pass the pointer coordinates of the scroll event so that we
		// know which window to scroll.
		string[6] = (char_u)(ಲಂಬಸಾಲು / 128 + ' ' + 1);
		string[7] = (char_u)(ಲಂಬಸಾಲು % 128 + ' ' + 1);
		string[8] = (char_u)(ಅಡ್ಡಸಾಲು / 128 + ' ' + 1);
		string[9] = (char_u)(ಅಡ್ಡಸಾಲು % 128 + ' ' + 1);

		ಆದರೆ (modifiers == 0)
		    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string + 3, 7);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = 0;
		    ಆದರೆ (modifiers & MOUSE_ಸರಿಸು)
			string[2] |= MOD_ಮುಖವಾಡ_ಸರಿಸು;
		    ಆದರೆ (modifiers & MOUSE_CTRL)
			string[2] |= MOD_ಮುಖವಾಡ_CTRL;
		    ಆದರೆ (modifiers & MOUSE_ALT)
			string[2] |= MOD_ಮುಖವಾಡ_ALT;
		    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, 10);
		}
		ಹಿಂತಿರುಗಿ;
	    }
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    // If a clipboard selection is in progress, handle it
    ಆದರೆ (clip_ನಕ್ಷತ್ರ.state == SELECT_ಒಳ_ಪ್ರಗತಿ)
    {
	clip_process_selection(button, X_2_ಲಂಬಸಾಲು(x), Y_2_ಅಡ್ಡಸಾಲು(y), repeated_click);

	// A release event may still need to be sent if the position is equal.
	ಅಡ್ಡಸಾಲು = gui_xy2colrow(x, y, &ಲಂಬಸಾಲು);
	ಆದರೆ (button != MOUSE_RELEASE || ಅಡ್ಡಸಾಲು != prev_ಅಡ್ಡಸಾಲು || ಲಂಬಸಾಲು != prev_ಲಂಬಸಾಲು)
	    ಹಿಂತಿರುಗಿ;
    }

    // Determine which mouse settings to look for based on the current mode
    ಆಯ್ಕೆ (get_real_state())
    {
	ಸ್ಥಿತಿ ಬಹುಲಕ_ಸಾಮಾನ್ಯ_BUSY:
	ಸ್ಥಿತಿ ಬಹುಲಕ_OP_PENDING:
# ifdef FEAT_TERMINAL
	ಸ್ಥಿತಿ ಬಹುಲಕ_TERMINAL:
# endif
	ಸ್ಥಿತಿ ಬಹುಲಕ_ಸಾಮಾನ್ಯ:	checkfor = MOUSE_ಸಾಮಾನ್ಯ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ಬಹುಲಕ_VISUAL:	checkfor = MOUSE_VISUAL; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ಬಹುಲಕ_SELECT:	checkfor = MOUSE_VISUAL; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ಬಹುಲಕ_REPLACE:
	ಸ್ಥಿತಿ ಬಹುಲಕ_REPLACE | ಬಹುಲಕ_LANGMAP:
	ಸ್ಥಿತಿ ಬಹುಲಕ_VREPLACE:
	ಸ್ಥಿತಿ ಬಹುಲಕ_VREPLACE | ಬಹುಲಕ_LANGMAP:
	ಸ್ಥಿತಿ ಬಹುಲಕ_ಸೇರಿಸು:
	ಸ್ಥಿತಿ ಬಹುಲಕ_ಸೇರಿಸು | ಬಹುಲಕ_LANGMAP:
				checkfor = MOUSE_ಸೇರಿಸು; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ಬಹುಲಕ_ASKMORE:
	ಸ್ಥಿತಿ ಬಹುಲಕ_HITRETURN:	// At the more- and hit-enter prompt pass the
				// mouse event for a click on or below the
				// message line.
				ಆದರೆ (Y_2_ಅಡ್ಡಸಾಲು(y) >= ಸಂದೇಶ_ಅಡ್ಡಸಾಲು)
				    checkfor = MOUSE_ಸಾಮಾನ್ಯ;
				ಇಲ್ಲದಿದ್ದರೆ
				    checkfor = MOUSE_RETURN;
				ನಿಲ್ಲಿಸು;

	    /*
	     * On the command line, use the clipboard selection on all lines
	     * but the command line.  But not when pasting.
	     */
	ಸ್ಥಿತಿ ಬಹುಲಕ_CMDLINE:
	ಸ್ಥಿತಿ ಬಹುಲಕ_CMDLINE | ಬಹುಲಕ_LANGMAP:
	    ಆದರೆ (Y_2_ಅಡ್ಡಸಾಲು(y) < cmdline_ಅಡ್ಡಸಾಲು && button != MOUSE_ಮಧ್ಯ)
		checkfor = MOUSE_NONE;
	    ಇಲ್ಲದಿದ್ದರೆ
		checkfor = MOUSE_COMMAND;
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    checkfor = MOUSE_NONE;
	    ನಿಲ್ಲಿಸು;
    };

    /*
     * Allow clipboard selection of text on the command line in "normal"
     * modes.  Don't do this when dragging the status line, or extending a
     * Visual selection.
     */
    ಆದರೆ ((State == ಬಹುಲಕ_ಸಾಮಾನ್ಯ || State == ಬಹುಲಕ_ಸಾಮಾನ್ಯ_BUSY
						      || (State & ಬಹುಲಕ_ಸೇರಿಸು))
	    && X_2_ಲಂಬಸಾಲು(x) >= firstwin->w_wincol
	    && X_2_ಲಂಬಸಾಲು(x) < firstwin->w_wincol + topframe->fr_ಅಗಲ
	    && Y_2_ಅಡ್ಡಸಾಲು(y) >= topframe->fr_ಎತ್ತರ + firstwin->w_winrow
	    && button != MOUSE_DRAG
# ifdef FEAT_MOUSESHAPE
	    && !drag_ಸ್ಥಿತಿ_ಸಾಲು
	    && !drag_sep_ಸಾಲು
# endif
	    )
	checkfor = MOUSE_NONE;

    /*
     * Use modeless selection when holding CTRL and SHIFT pressed.
     */
    ಆದರೆ ((modifiers & MOUSE_CTRL) && (modifiers & MOUSE_ಸರಿಸು))
	checkfor = MOUSE_NONEF;

    /*
     * In Ex mode, always use modeless selection.
     */
    ಆದರೆ (exmode_ಸಕ್ರಿಯ)
	checkfor = MOUSE_NONE;

    /*
     * If the mouse settings say to not use the mouse, use the modeless
     * selection.  But if Visual is active, assume that only the Visual area
     * will be selected.
     * Exception: On the command line, both the selection is used and a mouse
     * key is sent.
     */
    ಆದರೆ (!mouse_has(checkfor) || checkfor == MOUSE_COMMAND)
    {
	// Don't do modeless selection in Visual mode.
	ಆದರೆ (checkfor != MOUSE_NONEF && VIsual_ಸಕ್ರಿಯ && (State & ಬಹುಲಕ_ಸಾಮಾನ್ಯ))
	    ಹಿಂತಿರುಗಿ;

	/*
	 * When 'mousemodel' is "popup", shift-left is translated to right.
	 * But not when also using Ctrl.
	 */
	ಆದರೆ (mouse_model_popup() && button == MOUSE_ಎಡ
		&& (modifiers & MOUSE_ಸರಿಸು) && !(modifiers & MOUSE_CTRL))
	{
	    button = MOUSE_ಬಲ;
	    modifiers &= ~ MOUSE_ಸರಿಸು;
	}

	// If the selection is done, allow the right button to extend it.
	// If the selection is cleared, allow the right button to start it
	// from the cursor position.
	ಆದರೆ (button == MOUSE_ಬಲ)
	{
	    ಆದರೆ (clip_ನಕ್ಷತ್ರ.state == SELECT_CLEARED)
	    {
		ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
		{
		    ಲಂಬಸಾಲು = ಸಂದೇಶ_ಲಂಬಸಾಲು;
		    ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಲಂಬಸಾಲು = curwin->w_wcol;
		    ಅಡ್ಡಸಾಲು = curwin->w_wrow + W_WINROW(curwin);
		}
		clip_ಆರಂಭ_selection(ಲಂಬಸಾಲು, ಅಡ್ಡಸಾಲು, FALSE);
	    }
	    clip_process_selection(button, X_2_ಲಂಬಸಾಲು(x), Y_2_ಅಡ್ಡಸಾಲು(y),
							      repeated_click);
	    did_clip = TRUE;
	}
	// Allow the left button to start the selection
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (button == MOUSE_ಎಡ)
	{
	    clip_ಆರಂಭ_selection(X_2_ಲಂಬಸಾಲು(x), Y_2_ಅಡ್ಡಸಾಲು(y), repeated_click);
	    did_clip = TRUE;
	}

	// Always allow pasting
	ಆದರೆ (button != MOUSE_ಮಧ್ಯ)
	{
	    ಆದರೆ (!mouse_has(checkfor) || button == MOUSE_RELEASE)
		ಹಿಂತಿರುಗಿ;
	    ಆದರೆ (checkfor != MOUSE_COMMAND)
		button = MOUSE_ಎಡ;
	}
	repeated_click = FALSE;
    }

    ಆದರೆ (clip_ನಕ್ಷತ್ರ.state != SELECT_CLEARED && !did_clip)
	clip_clear_selection(&clip_ನಕ್ಷತ್ರ);
#ಆದರೆ_ಅಂತ್ಯ

    // Don't put events in the input queue now.
    ಆದರೆ (hold_gui_events)
	ಹಿಂತಿರುಗಿ;

    ಅಡ್ಡಸಾಲು = gui_xy2colrow(x, y, &ಲಂಬಸಾಲು);

    /*
     * If we are dragging and the mouse hasn't moved far enough to be on a
     * different character, then don't send an event to vim.
     */
    ಆದರೆ (button == MOUSE_DRAG)
    {
	ಆದರೆ (ಅಡ್ಡಸಾಲು == prev_ಅಡ್ಡಸಾಲು && ಲಂಬಸಾಲು == prev_ಲಂಬಸಾಲು)
	    ಹಿಂತಿರುಗಿ;
	// Dragging above the window, set "row" to -1 to cause a scroll.
	ಆದರೆ (y < 0)
	    ಅಡ್ಡಸಾಲು = -1;
    }

    /*
     * If topline has changed (window scrolled) since the last click, reset
     * repeated_click, because we don't want starting Visual mode when
     * clicking on a different character in the text.
     */
    ಆದರೆ (curwin->w_topline != gui_prev_topline
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    || curwin->w_topfill != gui_prev_topfill
#ಆದರೆ_ಅಂತ್ಯ
	    )
	repeated_click = FALSE;

    string[0] = CSI;	// this sequence is recognized by check_termcode()
    string[1] = KS_MOUSE;
    string[2] = KE_FILLER;
    ಆದರೆ (button != MOUSE_DRAG && button != MOUSE_RELEASE)
    {
	ಆದರೆ (repeated_click)
	{
	    /*
	     * Handle multiple clicks.	They only count if the mouse is still
	     * pointing at the same character.
	     */
	    ಆದರೆ (button != prev_button || ಅಡ್ಡಸಾಲು != prev_ಅಡ್ಡಸಾಲು || ಲಂಬಸಾಲು != prev_ಲಂಬಸಾಲು)
		ಅಂಕೆ_clicks = 1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (++ಅಂಕೆ_clicks > 4)
		ಅಂಕೆ_clicks = 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಅಂಕೆ_clicks = 1;
	prev_button = button;
	gui_prev_topline = curwin->w_topline;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	gui_prev_topfill = curwin->w_topfill;
#ಆದರೆ_ಅಂತ್ಯ

	string[3] = (char_u)(button | 0x20);
	ಗಣ_ಅಂಕೆ_MOUSE_CLICKS(string[3], ಅಂಕೆ_clicks);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	string[3] = (char_u)button;

    string[3] |= modifiers;
    fill_mouse_coord(string + 4, ಲಂಬಸಾಲು, ಅಡ್ಡಸಾಲು);
    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, 8);

    ಆದರೆ (ಅಡ್ಡಸಾಲು < 0)
	prev_ಅಡ್ಡಸಾಲು = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	prev_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
    prev_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#ಆದರೆ defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Convert x and y coordinate to column and row in text window.
 * Corrects for multi-byte character.
 * returns column in "*colp" and row as return value;
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_xy2colrow(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ *colp)
{
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = check_ಲಂಬಸಾಲು(X_2_ಲಂಬಸಾಲು(x));
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು = check_ಅಡ್ಡಸಾಲು(Y_2_ಅಡ್ಡಸಾಲು(y));

    *colp = mb_fix_ಲಂಬಸಾಲು(ಲಂಬಸಾಲು, ಅಡ್ಡಸಾಲು);
    ಹಿಂತಿರುಗಿ ಅಡ್ಡಸಾಲು;
}

#ಆದರೆ defined(FEAT_MENU)
/*
 * Callback function for when a menu entry has been selected.
 */
    ಶೂನ್ಯ
gui_menu_cb(vimmenu_T *menu)
{
    char_u  ಬೈಟುಗಳು[ಗಾತ್ರ(long_u)];

    // Don't put events in the input queue now.
    ಆದರೆ (hold_gui_events)
	ಹಿಂತಿರುಗಿ;

    ಬೈಟುಗಳು[0] = CSI;
    ಬೈಟುಗಳು[1] = KS_MENU;
    ಬೈಟುಗಳು[2] = KE_FILLER;
    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(ಬೈಟುಗಳು, 3);
    ಕೂಡಿಸು_long_ಗಮ್ಯ_buf((long_u)menu, ಬೈಟುಗಳು);
    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf_csi(ಬೈಟುಗಳು, ಗಾತ್ರ(long_u));
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	prev_which_scrollbars[3];

/*
 * Set which components are present.
 * If "oldval" is not NULL, "oldval" is the previous value, the new value is
 * in p_go.
 */
    ಶೂನ್ಯ
gui_init_which_components(char_u *oldval ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_DARKTHEME
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	prev_dark_theme = FALSE;
    ಪೂರ್ಣಾಂಕ		using_dark_theme = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	prev_menu_is_ಸಕ್ರಿಯ = -1;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	prev_toolbar = -1;
    ಪೂರ್ಣಾಂಕ		using_toolbar = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    ಪೂರ್ಣಾಂಕ		using_tabline;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	prev_titlebar = FALSE;
    ಪೂರ್ಣಾಂಕ		using_titlebar = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	prev_fullscreen = FALSE;
    ಪೂರ್ಣಾಂಕ		using_fullscreen = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_MENU)
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	prev_tearoff = -1;
    ಪೂರ್ಣಾಂಕ		using_tearoff = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    ಪೂರ್ಣಾಂಕ		grey_old, grey_new;
    char_u	*ತಾತ್ಕಾಲಿಕ;
#ಆದರೆ_ಅಂತ್ಯ
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		need_ಗಣ_ಗಾತ್ರ;
    ಪೂರ್ಣಾಂಕ		fix_ಗಾತ್ರ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    ಆದರೆ (oldval != ಖಾಲಿ && gui.ಒಳ_use)
    {
	/*
	 * Check if the menus go from grey to non-grey or vice versa.
	 */
	grey_old = (vim_strchr(oldval, GO_GREY) != ಖಾಲಿ);
	grey_new = (vim_strchr(p_go, GO_GREY) != ಖಾಲಿ);
	ಆದರೆ (grey_old != grey_new)
	{
	    ತಾತ್ಕಾಲಿಕ = p_go;
	    p_go = oldval;
	    gui_update_menus(MENU_ALL_MODES);
	    p_go = ತಾತ್ಕಾಲಿಕ;
	}
    }
    gui.menu_is_ಸಕ್ರಿಯ = false;
#ಆದರೆ_ಅಂತ್ಯ

    ಗಾಗಿ (i = 0; i < 3; i++)
	gui.which_scrollbars[i] = false;
    ಗಾಗಿ (p = p_go; *p; p++)
	ಆಯ್ಕೆ (*p)
	{
	    ಸ್ಥಿತಿ GO_ಎಡ:
		gui.which_scrollbars[SBAR_ಎಡ] = true;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GO_ಬಲ:
		gui.which_scrollbars[SBAR_ಬಲ] = true;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GO_VLEFT:
		ಆದರೆ (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_ಎಡ] = true;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GO_VRIGHT:
		ಆದರೆ (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_ಬಲ] = true;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GO_BOT:
		gui.which_scrollbars[SBAR_BOTTOM] = true;
		ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_DARKTHEME
	    ಸ್ಥಿತಿ GO_DARKTHEME:
		using_dark_theme = TRUE;
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	    ಸ್ಥಿತಿ GO_MENUS:
		gui.menu_is_ಸಕ್ರಿಯ = true;
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	    ಸ್ಥಿತಿ GO_GREY:
		// make menu's have grey items, ignored here
		ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
	    ಸ್ಥಿತಿ GO_TITLEBAR:
		using_titlebar = TRUE;
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
	    ಸ್ಥಿತಿ GO_FULLSCREEN:
		using_fullscreen = TRUE;
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
	    ಸ್ಥಿತಿ GO_TOOLBAR:
		using_toolbar = TRUE;
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	    ಸ್ಥಿತಿ GO_TEAROFF:
#ಆದರೆ defined(FEAT_MENU)
		using_tearoff = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		// Ignore options that are not supported
		ನಿಲ್ಲಿಸು;
	}

    ಆದರೆ (!gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;

    need_ಗಣ_ಗಾತ್ರ = 0;
    fix_ಗಾತ್ರ = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
    ಆದರೆ (using_titlebar != prev_titlebar)
    {
	gui_mch_ಗಣ_titlebar_colors();
	prev_titlebar = using_titlebar;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    ಆದರೆ (using_fullscreen != prev_fullscreen)
    {
	gui_mch_ಗಣ_fullscreen(using_fullscreen);
	prev_fullscreen = using_fullscreen;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_DARKTHEME
    ಆದರೆ (using_dark_theme != prev_dark_theme)
    {
	gui_mch_ಗಣ_dark_theme(using_dark_theme);
	prev_dark_theme = using_dark_theme;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    // Update the GUI tab line, it may appear or disappear.  This may
    // cause the non-GUI tab line to disappear or appear.
    using_tabline = gui_has_tabline();
    ಆದರೆ (!gui_mch_showing_tabline() != !using_tabline)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = ಅಡ್ಡಸಾಲುಗಳು;
	gui_update_tabline();
	ಅಡ್ಡಸಾಲುಗಳು = i;
	need_ಗಣ_ಗಾತ್ರ |= RESIZE_VERT;
	ಆದರೆ (using_tabline)
	    fix_ಗಾತ್ರ = TRUE;
	ಆದರೆ (!gui_use_tabline())
	    redraw_tabline = TRUE;    // may draw non-GUI tab line
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಗಾಗಿ (i = 0; i < 3; i++)
    {
	// The scrollbar needs to be updated when it is shown/unshown and
	// when switching tab pages.  But the size only changes when it's
	// shown/unshown.  Thus we need two places to remember whether a
	// scrollbar is there or not.
	ಆದರೆ (gui.which_scrollbars[i] != prev_which_scrollbars[i]
		|| gui.which_scrollbars[i]
		!= curtab->tp_prev_which_scrollbars[i])
	{
	    ಆದರೆ (i == SBAR_BOTTOM)
		gui_mch_enable_scrollbar(&gui.bottom_sbar,
			gui.which_scrollbars[i]);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		FOR_ALL_WINDOWS(wp)
		    gui_do_scrollbar(wp, i, gui.which_scrollbars[i]);
	    }
	    ಆದರೆ (gui.which_scrollbars[i] != prev_which_scrollbars[i])
	    {
		ಆದರೆ (i == SBAR_BOTTOM)
		    need_ಗಣ_ಗಾತ್ರ |= RESIZE_VERT;
		ಇಲ್ಲದಿದ್ದರೆ
		    need_ಗಣ_ಗಾತ್ರ |= RESIZE_HOR;
		ಆದರೆ (gui.which_scrollbars[i])
		    fix_ಗಾತ್ರ = TRUE;
	    }
	}
	curtab->tp_prev_which_scrollbars[i] = gui.which_scrollbars[i];
	prev_which_scrollbars[i] = gui.which_scrollbars[i];
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    ಆದರೆ (gui.menu_is_ಸಕ್ರಿಯ != prev_menu_is_ಸಕ್ರಿಯ)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = ಅಡ್ಡಸಾಲುಗಳು;
	gui_mch_enable_menu(gui.menu_is_ಸಕ್ರಿಯ);
	ಅಡ್ಡಸಾಲುಗಳು = i;
	prev_menu_is_ಸಕ್ರಿಯ = gui.menu_is_ಸಕ್ರಿಯ;
	need_ಗಣ_ಗಾತ್ರ |= RESIZE_VERT;
	ಆದರೆ (gui.menu_is_ಸಕ್ರಿಯ)
	    fix_ಗಾತ್ರ = TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    ಆದರೆ (using_toolbar != prev_toolbar)
    {
	gui_mch_show_toolbar(using_toolbar);
	prev_toolbar = using_toolbar;
	need_ಗಣ_ಗಾತ್ರ |= RESIZE_VERT;
	ಆದರೆ (using_toolbar)
	    fix_ಗಾತ್ರ = TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_MENU) && !(defined(MSWIN) && !defined(FEAT_TEAROFF))
    ಆದರೆ (using_tearoff != prev_tearoff)
    {
	gui_mch_toggle_tearoffs(using_tearoff);
	prev_tearoff = using_tearoff;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (need_ಗಣ_ಗಾತ್ರ != 0)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
	ದೀರ್ಘ    prev_Columns = Columns;
	ದೀರ್ಘ    prev_ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು;
#ಆದರೆ_ಅಂತ್ಯ
	// Adjust the size of the window to make the text area keep the
	// same size and to avoid that part of our window is off-screen
	// and a scrollbar can't be used, for example.
	gui_ಗಣ_shellsize(FALSE, fix_ಗಾತ್ರ, need_ಗಣ_ಗಾತ್ರ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
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
	    ದೀರ್ಘ    post_ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು;
	    ದೀರ್ಘ    post_Columns = Columns;

	    ಆದರೆ (!gui.starting && need_ಗಣ_ಗಾತ್ರ != (RESIZE_VERT | RESIZE_HOR))
		(ಶೂನ್ಯ)char_avail();
	    ಆದರೆ ((need_ಗಣ_ಗಾತ್ರ & RESIZE_VERT) == 0)
		ಅಡ್ಡಸಾಲುಗಳು = prev_ಅಡ್ಡಸಾಲುಗಳು;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಅಡ್ಡಸಾಲುಗಳು = post_ಅಡ್ಡಸಾಲುಗಳು;
	    ಆದರೆ ((need_ಗಣ_ಗಾತ್ರ & RESIZE_HOR) == 0)
		Columns = prev_Columns;
	    ಇಲ್ಲದಿದ್ದರೆ
		Columns = post_Columns;
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    // When the console tabline appears or disappears the window positions
    // change.
    ಆದರೆ (firstwin->w_winrow != tabline_ಎತ್ತರ())
	shell_new_ಅಡ್ಡಸಾಲುಗಳು();	// recompute window positions and heights
}

#ಆದರೆ defined(FEAT_GUI_TABLINE)
/*
 * Return TRUE if the GUI is taking care of the tabline.
 * It may still be hidden if 'showtabline' is zero.
 */
    ಪೂರ್ಣಾಂಕ
gui_use_tabline(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ gui.ಒಳ_use && vim_strchr(p_go, GO_TABLINE) != ಖಾಲಿ;
}

/*
 * Return TRUE if the GUI is showing the tabline.
 * This uses 'showtabline'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_has_tabline(ಶೂನ್ಯ)
{
    ಆದರೆ (!gui_use_tabline()
	    || p_stal == 0
	    || (p_stal == 1 && ಮೊದಲ_tabpage->tp_ಮುಂದಿನ == ಖಾಲಿ))
	ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Update the tabline.
 * This may display/undisplay the tabline and update the labels.
 */
    ಶೂನ್ಯ
gui_update_tabline(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    showit = gui_has_tabline();
    ಪೂರ್ಣಾಂಕ	    shown = gui_mch_showing_tabline();

    ಆದರೆ (!gui.starting && starting == 0)
    {
	// Updating the tabline uses direct GUI commands, flush
	// outstanding instructions first. (esp. clear screen)
	ಹೊರಹೋಗು_flush();

	ಆದರೆ (!showit != !shown)
	{
	    // Save Rows/Columns before showing/hiding the tabline.
	    // gui_mch_show_tabline() processes GTK events (via
	    // gui_mch_update) which may trigger a spurious configure event
	    // that temporarily corrupts Rows.  Restore the original values so
	    // that gui_set_shellsize() uses the correct row/column count.
	    ಪೂರ್ಣಾಂಕ save_ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು;
	    ಪೂರ್ಣಾಂಕ save_Columns = Columns;

	    gui_mch_show_tabline(showit);
	    ಅಡ್ಡಸಾಲುಗಳು = save_ಅಡ್ಡಸಾಲುಗಳು;
	    Columns = save_Columns;

	    // Resize the outer window to compensate for the tabline height
	    // change.  This is also needed when called from draw_tabline()
	    // (e.g. after :tabclose), where no caller will do the resize.
	    // When called from gui_init_which_components() the subsequent
	    // gui_set_shellsize() call will redo this to the same dimensions,
	    // which is harmless.
	    gui_ಗಣ_shellsize(FALSE, showit, RESIZE_VERT);
	}
	ಆದರೆ (showit != 0)
	    gui_mch_update_tabline();
    }
}

/*
 * Get the label or tooltip for tab page "tp" into NameBuff[].
 */
    ಶೂನ್ಯ
get_tabline_label(
    tabpage_T	*tp,
    ಪೂರ್ಣಾಂಕ		tooltip)	// TRUE: get tooltip
{
    char_u	**opt;

    // Use 'guitablabel' or 'guitabtooltip' if it's set.
    opt = (tooltip ? &p_gtt : &p_gtl);
    ಆದರೆ (**opt != NUL)
    {
	char_u	res[MAXPATHL];
	tabpage_T *save_curtab;
	char_u	*opt_name = (char_u *)(tooltip ? "guitabtooltip"
							     : "guitablabel");

	printer_ಪುಟ_ಅಂಕೆ = tabpage_ಸೂಚ್ಯಂಕ(tp);
# ifdef FEAT_EVAL
	ಗಣ_vim_var_nr(VV_LNUM, printer_ಪುಟ_ಅಂಕೆ);
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
	curbuf = curwin->w_ಬಫರ್;

	// Can't use NameBuff directly, build_stl_str_hl() uses it.
	build_stl_str_hl(curwin, res, MAXPATHL, *opt, opt_name, 0,
					   0, (ಪೂರ್ಣಾಂಕ)Columns, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
	STRCPY(NameBuff, res);

	// Back to the original curtab.
	curtab = save_curtab;
	topframe = curtab->tp_topframe;
	firstwin = curtab->tp_firstwin;
	lastwin = curtab->tp_lastwin;
	curwin = curtab->tp_curwin;
	curbuf = curwin->w_ಬಫರ್;
    }

    // If 'guitablabel'/'guitabtooltip' is not set or the result is empty then
    // use a default label.
    ಆದರೆ (**opt == NUL || *NameBuff == NUL)
    {
	win_T	*wp;
	ಪೂರ್ಣಾಂಕ	wincount = 0;
	ಪೂರ್ಣಾಂಕ	modified = FALSE;

	// Get the buffer name into NameBuff[] and shorten it.
	get_trans_bufname(tp == curtab ? curbuf : tp->tp_curwin->w_ಬಫರ್);
	ಆದರೆ (!tooltip)
	    shorten_dir(NameBuff);

	FOR_ALL_WINDOWS_ಒಳ_TAB(tp, wp)
	{
	    ++wincount;
	    ಆದರೆ (buf_Is_ಬದಲಾಯಿತು(wp->w_ಬಫರ್))
		modified = TRUE;
	}

	ಆದರೆ (modified || wincount > 1)
	{
	    char_u  buf[40] = "+ ";		// Tentatively assume modified only
	    size_t  buflen = 2;
	    size_t  NameBufflen = STRLEN(NameBuff);

	    ಆದರೆ (wincount > 1)
		buflen = vim_snprintf_safelen(
		    (ಅಕ್ಷರ *)buf,
		    ಗಾತ್ರ(buf),
		    "%d%s ",
		    wincount,
		    modified ? "+" : "");

	    // Make sure resulting NameBuff will not exceed its bounds.
	    ಆದರೆ (NameBufflen + buflen < MAXPATHL)
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
    ಪೂರ್ಣಾಂಕ
send_tabline_event(ಪೂರ್ಣಾಂಕ nr)
{
    char_u string[3];

    ಆದರೆ (nr == tabpage_ಸೂಚ್ಯಂಕ(curtab))
	ಹಿಂತಿರುಗಿ FALSE;

    // Don't put events in the input queue now.
    ಆದರೆ (hold_gui_events || cmdwin_type != 0)
    {
	// Set it back to the current tab page.
	gui_mch_ಗಣ_curtab(tabpage_ಸೂಚ್ಯಂಕ(curtab));
	ಹಿಂತಿರುಗಿ FALSE;
    }

    string[0] = CSI;
    string[1] = KS_TABLINE;
    string[2] = KE_FILLER;
    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, 3);
    string[0] = nr;
    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf_csi(string, 1);
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Send a tabline menu event
 */
    ಶೂನ್ಯ
send_tabline_menu_event(ಪೂರ್ಣಾಂಕ tabidx, ಪೂರ್ಣಾಂಕ event)
{
    char_u	    string[3];

    // Don't put events in the input queue now.
    ಆದರೆ (hold_gui_events)
	ಹಿಂತಿರುಗಿ;

    // Cannot close the last tabpage.
    ಆದರೆ (event == TABLINE_MENU_CLOSE && ಮೊದಲ_tabpage->tp_ಮುಂದಿನ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    string[0] = CSI;
    string[1] = KS_TABMENU;
    string[2] = KE_FILLER;
    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, 3);
    string[0] = tabidx;
    string[1] = (char_u)(ದೀರ್ಘ)event;
    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf_csi(string, 2);
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * Scrollbar stuff:
 */

/*
 * Remove all scrollbars.  Used before switching to another tab page.
 */
    ಶೂನ್ಯ
gui_remove_scrollbars(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    i;
    win_T   *wp;

    ಗಾಗಿ (i = 0; i < 3; i++)
    {
	ಆದರೆ (i == SBAR_BOTTOM)
	    gui_mch_enable_scrollbar(&gui.bottom_sbar, FALSE);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    FOR_ALL_WINDOWS(wp)
		gui_do_scrollbar(wp, i, FALSE);
	}
	curtab->tp_prev_which_scrollbars[i] = -1;
    }
}

    ಶೂನ್ಯ
gui_create_scrollbar(scrollbar_T *sb, ಪೂರ್ಣಾಂಕ type, win_T *wp)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	sbar_ident = 0;

    sb->ident = sbar_ident++;	// No check for too big, but would it happen?
    sb->wp = wp;
    sb->type = type;
    sb->ಮೌಲ್ಯ = 0;
    sb->ಗಾತ್ರ_2 = 1;
    sb->max = 1;
    sb->ತುದಿ = 0;
    sb->ಎತ್ತರ = 0;
    sb->ಅಗಲ = 0;
    sb->ಸ್ಥಿತಿ_ಎತ್ತರ = 0;
    gui_mch_create_scrollbar(sb, (wp == ಖಾಲಿ) ? SBAR_HORIZ : SBAR_VERT);
}

/*
 * Find the scrollbar with the given index.
 */
    scrollbar_T *
gui_ಕಂಡುಹಿಡಿ_scrollbar(ದೀರ್ಘ ident)
{
    win_T	*wp;

    ಆದರೆ (gui.bottom_sbar.ident == ident)
	ಹಿಂತಿರುಗಿ &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	ಆದರೆ (wp->w_scrollbars[SBAR_ಎಡ].ident == ident)
	    ಹಿಂತಿರುಗಿ &wp->w_scrollbars[SBAR_ಎಡ];
	ಆದರೆ (wp->w_scrollbars[SBAR_ಬಲ].ident == ident)
	    ಹಿಂತಿರುಗಿ &wp->w_scrollbars[SBAR_ಬಲ];
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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
    ಶೂನ್ಯ
gui_drag_scrollbar(scrollbar_T *sb, ದೀರ್ಘ ಮೌಲ್ಯ, ಪೂರ್ಣಾಂಕ still_dragging)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		sb_ಅಂಕೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ON_FLY_SCROLL
    colnr_T	old_leftcol = curwin->w_leftcol;
    linenr_T	old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		old_topfill = curwin->w_topfill;
# endif
#ಇಲ್ಲದಿದ್ದರೆ
    char_u	ಬೈಟುಗಳು[ಗಾತ್ರ(long_u)];
    ಪೂರ್ಣಾಂಕ		byte_ಎಣಿಕೆ;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (sb == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Don't put events in the input queue now.
    ಆದರೆ (hold_gui_events)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (cmdwin_type != 0 && sb->wp != cmdwin_win)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (still_dragging)
    {
	ಆದರೆ (sb->wp == ಖಾಲಿ)
	    gui.dragged_sb = SBAR_BOTTOM;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sb == &sb->wp->w_scrollbars[SBAR_ಎಡ])
	    gui.dragged_sb = SBAR_ಎಡ;
	ಇಲ್ಲದಿದ್ದರೆ
	    gui.dragged_sb = SBAR_ಬಲ;
	gui.dragged_wp = sb->wp;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	gui.dragged_sb = SBAR_NONE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
	// Keep the "dragged_wp" value until after the scrolling, for when the
	// mouse button is released.  GTK2 doesn't send the button-up event.
	gui.dragged_wp = ಖಾಲಿ;
	// WinScrolled event
	gui_focus_change(TRUE);
#ಆದರೆ_ಅಂತ್ಯ
    }

    // Vertical sbar info is kept in the first sbar (the left one)
    ಆದರೆ (sb->wp != ಖಾಲಿ)
	sb = &sb->wp->w_scrollbars[0];

    /*
     * Check validity of value
     */
    ಆದರೆ (ಮೌಲ್ಯ < 0)
	ಮೌಲ್ಯ = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SCROLL_PAST_ಕೊನೆ
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೌಲ್ಯ > sb->max)
	ಮೌಲ್ಯ = sb->max;
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (ಮೌಲ್ಯ > sb->max - sb->ಗಾತ್ರ_2 + 1)
	ಮೌಲ್ಯ = sb->max - sb->ಗಾತ್ರ_2 + 1;
#ಆದರೆ_ಅಂತ್ಯ

    sb->ಮೌಲ್ಯ = ಮೌಲ್ಯ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ON_FLY_SCROLL
    // When not allowed to do the scrolling right now, return.
    // This also checked input_available(), but that causes the first click in
    // a scrollbar to be ignored when Vim doesn't have focus.
    ಆದರೆ (dont_scroll)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    // Disallow scrolling the current window when the completion popup menu is
    // visible.
    ಆದರೆ ((sb->wp == ಖಾಲಿ || sb->wp == curwin) && pum_visible())
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (sb->wp == ಖಾಲಿ && curwin->w_p_rl)
    {
	ಮೌಲ್ಯ = sb->max + 1 - sb->ಗಾತ್ರ_2 - ಮೌಲ್ಯ;
	ಆದರೆ (ಮೌಲ್ಯ < 0)
	    ಮೌಲ್ಯ = 0;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (sb->wp != ಖಾಲಿ)		// vertical scrollbar
    {
	sb_ಅಂಕೆ = 0;
	ಗಾಗಿ (wp = firstwin; wp != sb->wp && wp != ಖಾಲಿ; wp = wp->w_ಮುಂದಿನ)
	    sb_ಅಂಕೆ++;
	ಆದರೆ (wp == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ON_FLY_SCROLL
	current_scrollbar = sb_ಅಂಕೆ;
	scrollbar_ಮೌಲ್ಯ = ಮೌಲ್ಯ;
	ಆದರೆ (State & ಬಹುಲಕ_ಸಾಮಾನ್ಯ)
	{
	    gui_do_scroll();
	    setcursor();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು)
	{
	    ins_scroll();
	    setcursor();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
	{
	    ಆದರೆ (ಸಂದೇಶ_scrolled == 0)
	    {
		gui_do_scroll();
		redrawcmdline();
	    }
	}
# ifdef FEAT_FOLDING
	// Value may have been changed for closed fold.
	sb->ಮೌಲ್ಯ = sb->wp->w_topline - 1;
# endif

	// When dragging one scrollbar and there is another one at the other
	// side move the thumb of that one too.
	ಆದರೆ (gui.which_scrollbars[SBAR_ಬಲ] && gui.which_scrollbars[SBAR_ಎಡ])
	    gui_mch_ಗಣ_scrollbar_thumb(
		    &sb->wp->w_scrollbars[
			    sb == &sb->wp->w_scrollbars[SBAR_ಬಲ]
						    ? SBAR_ಎಡ : SBAR_ಬಲ],
		    sb->ಮೌಲ್ಯ, sb->ಗಾತ್ರ_2, sb->max);

#ಇಲ್ಲದಿದ್ದರೆ
	ಬೈಟುಗಳು[0] = CSI;
	ಬೈಟುಗಳು[1] = KS_VER_SCROLLBAR;
	ಬೈಟುಗಳು[2] = KE_FILLER;
	ಬೈಟುಗಳು[3] = (char_u)sb_ಅಂಕೆ;
	byte_ಎಣಿಕೆ = 4;
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ON_FLY_SCROLL
	scrollbar_ಮೌಲ್ಯ = ಮೌಲ್ಯ;

	ಆದರೆ (State & ಬಹುಲಕ_ಸಾಮಾನ್ಯ)
	    do_mousescroll_horiz(scrollbar_ಮೌಲ್ಯ);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು)
	    ins_horscroll();
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
	{
	    ಆದರೆ (ಸಂದೇಶ_scrolled == 0)
	    {
		do_mousescroll_horiz(scrollbar_ಮೌಲ್ಯ);
		redrawcmdline();
	    }
	}
	ಆದರೆ (old_leftcol != curwin->w_leftcol)
	{
	    update_ಕಿಟಕಿ(curwin);   // update window, status and cmdline
	    setcursor();
	}
#ಇಲ್ಲದಿದ್ದರೆ
	ಬೈಟುಗಳು[0] = CSI;
	ಬೈಟುಗಳು[1] = KS_HOR_SCROLLBAR;
	ಬೈಟುಗಳು[2] = KE_FILLER;
	byte_ಎಣಿಕೆ = 3;
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ON_FLY_SCROLL
    /*
     * synchronize other windows, as necessary according to 'scrollbind'
     */
    ಆದರೆ (curwin->w_p_scb
	    && ((sb->wp == ಖಾಲಿ && curwin->w_leftcol != old_leftcol)
		|| (sb->wp == curwin && (curwin->w_topline != old_topline
# ifdef FEAT_DIFF
					   || curwin->w_topfill != old_topfill
# endif
			))))
    {
	do_check_scrollbind(TRUE);
	// need to update the window right here
	FOR_ALL_WINDOWS(wp)
	    ಆದರೆ (wp->w_redr_type > 0)
		update_ಕಿಟಕಿ(wp);
	setcursor();
    }
    ಹೊರಹೋಗು_flush_cursor(FALSE, TRUE);
#ಇಲ್ಲದಿದ್ದರೆ
    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(ಬೈಟುಗಳು, byte_ಎಣಿಕೆ);
    ಕೂಡಿಸು_long_ಗಮ್ಯ_buf((long_u)ಮೌಲ್ಯ, ಬೈಟುಗಳು);
    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf_csi(ಬೈಟುಗಳು, ಗಾತ್ರ(long_u));
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Scrollbar stuff:
 */

/*
 * Called when something in the window layout has changed.
 */
    ಶೂನ್ಯ
gui_may_update_scrollbars(ಶೂನ್ಯ)
{
    ಆದರೆ (gui.ಒಳ_use && starting == 0)
    {
	ಹೊರಹೋಗು_flush();
	gui_init_which_components(ಖಾಲಿ);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
}

    ಶೂನ್ಯ
gui_update_scrollbars(
    ಪೂರ್ಣಾಂಕ		force)	    // Force all scrollbars to get updated
{
    win_T	*wp;
    scrollbar_T	*sb;
    ದೀರ್ಘ	val, ಗಾತ್ರ_2, max;		// need 32 bits here
    ಪೂರ್ಣಾಂಕ		which_sb;
    ಪೂರ್ಣಾಂಕ		h, y;
    ಸ್ಥಿರ win_T *prev_curwin = ಖಾಲಿ;

    // Update the horizontal scrollbar
    gui_update_horiz_scrollbar(force);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MSWIN
    // Return straight away if there is neither a left nor right scrollbar.
    // On MS-Windows this is required anyway for scrollwheel messages.
    ಆದರೆ (!gui.which_scrollbars[SBAR_ಎಡ] && !gui.which_scrollbars[SBAR_ಬಲ])
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Don't want to update a scrollbar while we're dragging it.  But if we
     * have both a left and right scrollbar, and we drag one of them, we still
     * need to update the other one.
     */
    ಆದರೆ (!force && (gui.dragged_sb == SBAR_ಎಡ || gui.dragged_sb == SBAR_ಬಲ)
	    && gui.which_scrollbars[SBAR_ಎಡ]
	    && gui.which_scrollbars[SBAR_ಬಲ])
    {
	/*
	 * If we have two scrollbars and one of them is being dragged, just
	 * copy the scrollbar position from the dragged one to the other one.
	 */
	which_sb = SBAR_ಎಡ + SBAR_ಬಲ - gui.dragged_sb;
	ಆದರೆ (gui.dragged_wp != ಖಾಲಿ)
	    gui_mch_ಗಣ_scrollbar_thumb(
		    &gui.dragged_wp->w_scrollbars[which_sb],
		    gui.dragged_wp->w_scrollbars[0].ಮೌಲ್ಯ,
		    gui.dragged_wp->w_scrollbars[0].ಗಾತ್ರ_2,
		    gui.dragged_wp->w_scrollbars[0].max);
    }

    // avoid that moving components around generates events
    ++hold_gui_events;

    FOR_ALL_WINDOWS(wp)
    {
	ಆದರೆ (wp->w_ಬಫರ್ == ಖಾಲಿ)	// just in case
	    ಮುಂದುವರಿ;
	// Skip a scrollbar that is being dragged.
	ಆದರೆ (!force && (gui.dragged_sb == SBAR_ಎಡ
					     || gui.dragged_sb == SBAR_ಬಲ)
		&& gui.dragged_wp == wp)
	    ಮುಂದುವರಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SCROLL_PAST_ಕೊನೆ
	max = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - 1;
#ಇಲ್ಲದಿದ್ದರೆ
	max = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + wp->w_ಎತ್ತರ - 2;
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (max < 0)			// empty buffer
	    max = 0;
	val = wp->w_topline - 1;
	ಗಾತ್ರ_2 = wp->w_ಎತ್ತರ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SCROLL_PAST_ಕೊನೆ
	ಆದರೆ (val > max)			// just in case
	    val = max;
#ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (ಗಾತ್ರ_2 > max + 1)		// just in case
	    ಗಾತ್ರ_2 = max + 1;
	ಆದರೆ (val > max - ಗಾತ್ರ_2 + 1)
	    val = max - ಗಾತ್ರ_2 + 1;
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (val < 0)			// minimal value is 0
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
	ಆದರೆ (ಗಾತ್ರ_2 < 1 || wp->w_botline - 2 > max)
	{
	    /*
	     * This can happen during changing files.  Just don't update the
	     * scrollbar for now.
	     */
	    sb->ಎತ್ತರ = 0;	    // Force update next time
	    ಆದರೆ (gui.which_scrollbars[SBAR_ಎಡ])
		gui_do_scrollbar(wp, SBAR_ಎಡ, FALSE);
	    ಆದರೆ (gui.which_scrollbars[SBAR_ಬಲ])
		gui_do_scrollbar(wp, SBAR_ಬಲ, FALSE);
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (force || sb->ಎತ್ತರ != wp->w_ಎತ್ತರ
	    || sb->ತುದಿ != wp->w_winrow
	    || sb->ಸ್ಥಿತಿ_ಎತ್ತರ != wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ
	    || sb->ಅಗಲ != wp->w_ಅಗಲ
	    || prev_curwin != curwin)
	{
	    // Height, width or position of scrollbar has changed.  For
	    // vertical split: curwin changed.
	    sb->ಎತ್ತರ = wp->w_ಎತ್ತರ;
	    sb->ತುದಿ = wp->w_winrow;
	    sb->ಸ್ಥಿತಿ_ಎತ್ತರ = wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ;
	    sb->ಅಗಲ = wp->w_ಅಗಲ;

	    // Calculate height and position in pixels
	    h = (sb->ಎತ್ತರ + sb->ಸ್ಥಿತಿ_ಎತ್ತರ) * gui.char_ಎತ್ತರ;
	    y = sb->ತುದಿ * gui.char_ಎತ್ತರ + gui.border_ಸ್ಥಾನಾಂತರ;
#ಆದರೆ defined(FEAT_MENU) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF) && !defined(FEAT_GUI_PHOTON)
	    ಆದರೆ (gui.menu_is_ಸಕ್ರಿಯ)
		y += gui.menu_ಎತ್ತರ;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU))
	    ಆದರೆ (vim_strchr(p_go, GO_TOOLBAR) != ಖಾಲಿ)
		y += gui.toolbar_ಎತ್ತರ;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
	    ಆದರೆ (gui_has_tabline())
		y += gui.tabline_ಎತ್ತರ;
#ಆದರೆ_ಅಂತ್ಯ

	    ಆದರೆ (wp->w_winrow == 0)
	    {
		// Height of top scrollbar includes width of top border
		h += gui.border_ಸ್ಥಾನಾಂತರ;
		y -= gui.border_ಸ್ಥಾನಾಂತರ;
	    }
	    ಆದರೆ (gui.which_scrollbars[SBAR_ಎಡ])
	    {
		gui_mch_ಗಣ_scrollbar_pos(&wp->w_scrollbars[SBAR_ಎಡ],
					  gui.ಎಡ_sbar_x, y,
					  gui.scrollbar_ಅಗಲ, h);
		gui_do_scrollbar(wp, SBAR_ಎಡ, TRUE);
	    }
	    ಆದರೆ (gui.which_scrollbars[SBAR_ಬಲ])
	    {
		gui_mch_ಗಣ_scrollbar_pos(&wp->w_scrollbars[SBAR_ಬಲ],
					  gui.ಬಲ_sbar_x, y,
					  gui.scrollbar_ಅಗಲ, h);
		gui_do_scrollbar(wp, SBAR_ಬಲ, TRUE);
	    }
	}

	ಆದರೆ (force || sb->ಮೌಲ್ಯ != val || sb->ಗಾತ್ರ_2 != ಗಾತ್ರ_2 || sb->max != max)
	{
	    // Thumb of scrollbar has moved
	    sb->ಮೌಲ್ಯ = val;
	    sb->ಗಾತ್ರ_2 = ಗಾತ್ರ_2;
	    sb->max = max;
	    ಆದರೆ (gui.which_scrollbars[SBAR_ಎಡ]
		    && (gui.dragged_sb != SBAR_ಎಡ || gui.dragged_wp != wp))
		gui_mch_ಗಣ_scrollbar_thumb(&wp->w_scrollbars[SBAR_ಎಡ],
					    val, ಗಾತ್ರ_2, max);
	    ಆದರೆ (gui.which_scrollbars[SBAR_ಬಲ]
		    && (gui.dragged_sb != SBAR_ಬಲ || gui.dragged_wp != wp))
		gui_mch_ಗಣ_scrollbar_thumb(&wp->w_scrollbars[SBAR_ಬಲ],
					    val, ಗಾತ್ರ_2, max);
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
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_do_scrollbar(
    win_T	*wp,
    ಪೂರ್ಣಾಂಕ		which,	    // SBAR_LEFT or SBAR_RIGHT
    ಪೂರ್ಣಾಂಕ		enable)	    // TRUE to enable scrollbar
{
    ಪೂರ್ಣಾಂಕ		midcol = curwin->w_wincol + curwin->w_ಅಗಲ / 2;
    ಪೂರ್ಣಾಂಕ		has_midcol = (wp->w_wincol <= midcol
				     && wp->w_wincol + wp->w_ಅಗಲ >= midcol);

    // Only enable scrollbars that contain the middle column of the current
    // window.
    ಆದರೆ (gui.which_scrollbars[SBAR_ಬಲ] != gui.which_scrollbars[SBAR_ಎಡ])
    {
	// Scrollbars only on one side.  Don't enable scrollbars that don't
	// contain the middle column of the current window.
	ಆದರೆ (!has_midcol)
	    enable = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Scrollbars on both sides.  Don't enable scrollbars that neither
	// contain the middle column of the current window nor are on the far
	// side.
	ಆದರೆ (midcol > Columns / 2)
	{
	    ಆದರೆ (which == SBAR_ಎಡ ? wp->w_wincol != 0 : !has_midcol)
		enable = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (which == SBAR_ಬಲ ? wp->w_wincol + wp->w_ಅಗಲ != Columns
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
    ಪೂರ್ಣಾಂಕ
gui_do_scroll(ಶೂನ್ಯ)
{
    win_T	*wp, *save_wp;
    ಪೂರ್ಣಾಂಕ		i;
    ದೀರ್ಘ	nlines;
    pos_T	old_cursor;
    linenr_T	old_topline;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		old_topfill;
#ಆದರೆ_ಅಂತ್ಯ

    ಗಾಗಿ (wp = firstwin, i = 0; i < current_scrollbar; wp = W_ಮುಂದಿನ(wp), i++)
	ಆದರೆ (wp == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (wp == ಖಾಲಿ)
	// Couldn't find window
	ಹಿಂತಿರುಗಿ FALSE;
    // don't redraw, LineOffset and similar are not valid!
    ಆದರೆ (exmode_ಸಕ್ರಿಯ)
	ಹಿಂತಿರುಗಿ FALSE;

    /*
     * Compute number of lines to scroll.  If zero, nothing to do.
     */
    nlines = (ದೀರ್ಘ)scrollbar_ಮೌಲ್ಯ + 1 - (ದೀರ್ಘ)wp->w_topline;
    ಆದರೆ (nlines == 0)
	ಹಿಂತಿರುಗಿ FALSE;

    save_wp = curwin;
    old_topline = wp->w_topline;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    old_topfill = wp->w_topfill;
#ಆದರೆ_ಅಂತ್ಯ
    old_cursor = wp->w_cursor;
    curwin = wp;
    curbuf = wp->w_ಬಫರ್;
    ಆದರೆ (nlines < 0)
	scrolldown(-nlines, gui.dragged_wp == ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ
	scrollup(nlines, gui.dragged_wp == ಖಾಲಿ);
    // Reset dragged_wp after using it.  "dragged_sb" will have been reset for
    // the mouse-up event already, but we still want it to behave like when
    // dragging.  But not the next click in an arrow.
    ಆದರೆ (gui.dragged_sb == SBAR_NONE)
	gui.dragged_wp = ಖಾಲಿ;

    ಆದರೆ (old_topline != wp->w_topline
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    || old_topfill != wp->w_topfill
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
	ಆದರೆ (get_scrolloff_ಮೌಲ್ಯ() != 0)
	{
	    cursor_correct();		// fix window for 'so'
	    update_topline();		// avoid up/down jump
	}
	ಆದರೆ (old_cursor.lnum != wp->w_cursor.lnum)
	    coladvance(wp->w_curswant);
	wp->w_scbind_pos = wp->w_topline;
    }

    // Make sure wp->w_leftcol and wp->w_skipcol are correct.
    validate_cursor();

    curwin = save_wp;
    curbuf = save_wp->w_ಬಫರ್;

    /*
     * Don't call updateWindow() when nothing has changed (it will overwrite
     * the status line!).
     *
     * Check for ScreenLines, because in ex-mode, we don't have a valid display.
     */
    ಆದರೆ (Screen_ಸಾಲುಗಳು != ಖಾಲಿ && (old_topline != wp->w_topline
	    || wp->w_redr_type != 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    || old_topfill != wp->w_topfill
#ಆದರೆ_ಅಂತ್ಯ
	    ))
    {
	ಪೂರ್ಣಾಂಕ type = UPD_VALID;

	ಆದರೆ (pum_visible())
	{
	    type = UPD_NOT_VALID;
	    wp->w_ಸಾಲುಗಳು_valid = 0;
	}

	// Don't set must_redraw here, it may cause the popup menu to
	// disappear when losing focus after a scrollbar drag.
	ಆದರೆ (wp->w_redr_type < type)
	    wp->w_redr_type = type;
	mch_disable_flush();
	update_ಕಿಟಕಿ(wp);   // update window, status line, and cmdline
	mch_enable_flush();
    }

    // May need to redraw the popup menu.
    ಆದರೆ (pum_visible())
	pum_redraw();

    ಹಿಂತಿರುಗಿ (wp == curwin && !EQUAL_POS(curwin->w_cursor, old_cursor));
}

/*
 * Horizontal scrollbar stuff:
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_update_horiz_scrollbar(ಪೂರ್ಣಾಂಕ force)
{
    ದೀರ್ಘ	ಮೌಲ್ಯ, ಗಾತ್ರ_2, max;

    ಆದರೆ (!gui.which_scrollbars[SBAR_BOTTOM])
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (!force && gui.dragged_sb == SBAR_BOTTOM)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (!force && curwin->w_p_wrap && gui.prev_wrap)
	ಹಿಂತಿರುಗಿ;

    /*
     * It is possible for the cursor to be invalid if we're in the middle of
     * something (like changing files).  If so, don't do anything for now.
     */
    ಆದರೆ (curwin->w_cursor.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	gui.bottom_sbar.ಮೌಲ್ಯ = -1;
	ಹಿಂತಿರುಗಿ;
    }

    ಗಾತ್ರ_2 = curwin->w_ಅಗಲ;
    ಆದರೆ (curwin->w_p_wrap)
    {
	ಮೌಲ್ಯ = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SCROLL_PAST_ಕೊನೆ
	max = 0;
#ಇಲ್ಲದಿದ್ದರೆ
	max = curwin->w_ಅಗಲ - 1;
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಮೌಲ್ಯ = curwin->w_leftcol;
	max = scroll_ಸಾಲು_len(ui_ಕಂಡುಹಿಡಿ_longest_lnum());

	ಆದರೆ (virtual_ಸಕ್ರಿಯ())
	{
	    // May move the cursor even further to the right.
	    ಆದರೆ (curwin->w_virtcol >= (colnr_T)max)
		max = curwin->w_virtcol;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SCROLL_PAST_ಕೊನೆ
	max += curwin->w_ಅಗಲ - 1;
#ಆದರೆ_ಅಂತ್ಯ
	// The line number isn't scrolled, thus there is less space when
	// 'number' or 'relativenumber' is set (also for 'foldcolumn').
	ಗಾತ್ರ_2 -= curwin_ಲಂಬಸಾಲು_off();
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SCROLL_PAST_ಕೊನೆ
	max -= curwin_ಲಂಬಸಾಲು_off();
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SCROLL_PAST_ಕೊನೆ
    ಆದರೆ (ಮೌಲ್ಯ > max - ಗಾತ್ರ_2 + 1)
	ಮೌಲ್ಯ = max - ಗಾತ್ರ_2 + 1;	    // limit the value to allowable range
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (curwin->w_p_rl)
    {
	ಮೌಲ್ಯ = max + 1 - ಗಾತ್ರ_2 - ಮೌಲ್ಯ;
	ಆದರೆ (ಮೌಲ್ಯ < 0)
	{
	    ಗಾತ್ರ_2 += ಮೌಲ್ಯ;
	    ಮೌಲ್ಯ = 0;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (!force && ಮೌಲ್ಯ == gui.bottom_sbar.ಮೌಲ್ಯ && ಗಾತ್ರ_2 == gui.bottom_sbar.ಗಾತ್ರ_2
						&& max == gui.bottom_sbar.max)
	ಹಿಂತಿರುಗಿ;

    gui.bottom_sbar.ಮೌಲ್ಯ = ಮೌಲ್ಯ;
    gui.bottom_sbar.ಗಾತ್ರ_2 = ಗಾತ್ರ_2;
    gui.bottom_sbar.max = max;
    gui.prev_wrap = curwin->w_p_wrap;

    gui_mch_ಗಣ_scrollbar_thumb(&gui.bottom_sbar, ಮೌಲ್ಯ, ಗಾತ್ರ_2, max);
}

/*
 * Check that none of the colors are the same as the background color
 */
    ಶೂನ್ಯ
gui_check_colors(ಶೂನ್ಯ)
{
    ಆದರೆ (gui.norm_pixel == gui.ಹಿಂಭಾಗ_pixel || gui.norm_pixel == INVALCOLOR)
    {
	gui_ಗಣ_bg_color((char_u *)"White");
	ಆದರೆ (gui.norm_pixel == gui.ಹಿಂಭಾಗ_pixel || gui.norm_pixel == INVALCOLOR)
	    gui_ಗಣ_fg_color((char_u *)"Black");
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ಗಣ_fg_color(char_u *name)
{
    gui.norm_pixel = gui_get_color(name);
    hl_ಗಣ_fg_color_name(vim_strsave(name));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ಗಣ_bg_color(char_u *name)
{
    gui.ಹಿಂಭಾಗ_pixel = gui_get_color(name);
    hl_ಗಣ_bg_color_name(vim_strsave(name));
}

/*
 * Allocate a color by name.
 * Returns INVALCOLOR and gives an error message when failed.
 */
    guicolor_T
gui_get_color(char_u *name)
{
    guicolor_T	t;

    ಆದರೆ (*name == NUL)
	ಹಿಂತಿರುಗಿ INVALCOLOR;
    t = gui_mch_get_color(name);

    ಪೂರ್ಣಾಂಕ is_none = STRCMP(name, "none") == 0;
    ಆದರೆ (t == INVALCOLOR
#ಆದರೆ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
	    && (gui.ಒಳ_use || is_none)
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
	ಆದರೆ (is_none)
	    emsg(_(e_cannot_use_color_none_did_you_ಸರಾಸರಿ_none));
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_cannot_allocate_color_str), name);
    }
    ಹಿಂತಿರುಗಿ t;
}

/*
 * Return the grey value of a color (range 0-255).
 */
    ಪೂರ್ಣಾಂಕ
gui_get_lightness(guicolor_T pixel)
{
    long_u	rgb = (long_u)gui_mch_get_rgb(pixel);

    ಹಿಂತಿರುಗಿ  (ಪೂರ್ಣಾಂಕ)(  (((rgb >> 16) & 0xff) * 299)
		   + (((rgb >> 8) & 0xff) * 587)
		   +  ((rgb	  & 0xff) * 114)) / 1000;
}

    char_u *
gui_bg_default(ಶೂನ್ಯ)
{
    ಆದರೆ (gui_get_lightness(gui.ಹಿಂಭಾಗ_pixel) < 127)
	ಹಿಂತಿರುಗಿ (char_u *)"dark";
    ಹಿಂತಿರುಗಿ (char_u *)"light";
}

/*
 * Option initializations that can only be done after opening the GUI window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_gui_options(ಶೂನ್ಯ)
{
    // Set the 'background' option according to the lightness of the
    // background color, unless the user has set it already.
    ಆದರೆ (!option_was_ಗಣ((char_u *)"bg") && STRCMP(p_bg, gui_bg_default()) != 0)
    {
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"bg", 0L, gui_bg_default(), 0);
	highlight_ಬದಲಾಯಿತು();
    }
}

#ಆದರೆ defined(FEAT_GUI_X11)
    ಶೂನ್ಯ
gui_new_scrollbar_colors(ಶೂನ್ಯ)
{
    win_T	*wp;

    // Nothing to do if GUI hasn't started yet.
    ಆದರೆ (!gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;

    FOR_ALL_WINDOWS(wp)
    {
	gui_mch_ಗಣ_scrollbar_colors(&(wp->w_scrollbars[SBAR_ಎಡ]));
	gui_mch_ಗಣ_scrollbar_colors(&(wp->w_scrollbars[SBAR_ಬಲ]));
    }
    gui_mch_ಗಣ_scrollbar_colors(&gui.bottom_sbar);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Call this when focus has changed.
 */
    ಶೂನ್ಯ
gui_focus_change(ಪೂರ್ಣಾಂಕ ಒಳ_focus)
{
/*
 * Skip this code to avoid drawing the cursor when debugging and switching
 * between the debugger window and gvim.
 */
#ಆದರೆ 1
    gui.ಒಳ_focus = ಒಳ_focus;
    ಹೊರಹೋಗು_flush_cursor(TRUE, FALSE);

# ifdef FEAT_GUI_MSWIN
    gui_mch_ಗಣ_titlebar_colors();
# endif

# ifdef FEAT_XIM
    xim_ಗಣ_focus(ಒಳ_focus);
# endif

    // Put events in the input queue only when allowed.
    // ui_focus_change() isn't called directly, because it invokes
    // autocommands and that must not happen asynchronously.
    ಆದರೆ (!hold_gui_events
# ಆದರೆ defined(FEAT_GUI_GTK) && defined(FEAT_GUI_DIALOG)
	    && gui.dialogs_ಸಕ್ರಿಯ == 0
# endif
       )
    {
	char_u  ಬೈಟುಗಳು[3];

	ಬೈಟುಗಳು[0] = CSI;
	ಬೈಟುಗಳು[1] = KS_EXTRA;
	ಬೈಟುಗಳು[2] = ಒಳ_focus ? (ಪೂರ್ಣಾಂಕ)KE_FOCUSGAINED : (ಪೂರ್ಣಾಂಕ)KE_FOCUSLOST;
	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(ಬೈಟುಗಳು, 3);
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * When mouse moved: apply 'mousefocus'.
 * Also updates the mouse pointer shape.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_mouse_focus(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    win_T	*wp;
    char_u	st[8];

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
    // Get window pointer, and update mouse shape as well.
    wp = xy2win(x, y, IGNORE_POPUP);
#ಆದರೆ_ಅಂತ್ಯ

    // Only handle this when 'mousefocus' set and ...
    ಆದರೆ (p_mousef
	    && !hold_gui_events		// not holding events
	    && (State & (ಬಹುಲಕ_ಸಾಮಾನ್ಯ | ಬಹುಲಕ_ಸೇರಿಸು))// Normal/Visual/Insert mode
	    && State != ಬಹುಲಕ_HITRETURN	// but not hit-return prompt
	    && ಸಂದೇಶ_scrolled == 0	// no scrolled message
	    && !need_mouse_correct	// not moving the pointer
	    && gui.ಒಳ_focus)		// gvim in focus
    {
	// Don't move the mouse when it's left or right of the Vim window
	ಆದರೆ (x < 0 || x > Columns * gui.char_ಅಗಲ)
	    ಹಿಂತಿರುಗಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_MOUSESHAPE
	wp = xy2win(x, y, IGNORE_POPUP);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (wp == curwin || wp == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;	// still in the same old window, or none at all

	// Ignore position in the tab pages line.
	ಆದರೆ (Y_2_ಅಡ್ಡಸಾಲು(y) < tabline_ಎತ್ತರ())
	    ಹಿಂತಿರುಗಿ;

	/*
	 * Format a mouse click on status line input,
	 * ala gui_send_mouse_event(0, x, y, 0, 0);
	 * Trick: Use a column number -1, so that get_pseudo_mouse_code() will
	 * generate a K_LEFTMOUSE_NM key code.
	 */
	ಆದರೆ (ಮುಕ್ತಾಯ_op)
	{
	    // abort the current operator first
	    st[0] = ESC;
	    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(st, 1);
	}
	st[0] = CSI;
	st[1] = KS_MOUSE;
	st[2] = KE_FILLER;
	st[3] = (char_u)MOUSE_ಎಡ;
	fill_mouse_coord(st + 4,
		wp->w_wincol == 0 ? -1 : wp->w_wincol + MOUSE_COLOFF,
		wp->w_ಎತ್ತರ + W_WINROW(wp));

	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(st, 8);
	st[3] = (char_u)MOUSE_RELEASE;
	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(st, 8);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
	// Need to wake up the main loop
	ಆದರೆ (gtk_main_level() > 0)
	    gtk_main_quit();
#ಆದರೆ_ಅಂತ್ಯ
    }
}

/*
 * Called when the mouse moved (but not when dragging).
 */
    ಶೂನ್ಯ
gui_mouse_moved(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    // Ignore this while still starting up.
    ಆದರೆ (!gui.ಒಳ_use || gui.starting)
	ಹಿಂತಿರುಗಿ;

    // apply 'mousefocus' and pointer shape
    gui_mouse_focus(x, y);

    ಆದರೆ (p_mousemev
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	|| popup_ಬಳಸುತ್ತದೆ_mouse_move
#ಆದರೆ_ಅಂತ್ಯ
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
gui_mouse_ಕಿಟಕಿ(mouse_ಕಂಡುಹಿಡಿ_T popup)
{
    ಪೂರ್ಣಾಂಕ		x, y;

    ಆದರೆ (!(gui.ಒಳ_use && (p_mousef || popup == ಕಂಡುಹಿಡಿ_POPUP)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    gui_mch_getmouse(&x, &y);

    // Only use the mouse when it's on the Vim window
    ಆದರೆ (x >= 0 && x <= Columns * gui.char_ಅಗಲ
	    && y >= 0 && Y_2_ಅಡ್ಡಸಾಲು(y) >= tabline_ಎತ್ತರ())
	ಹಿಂತಿರುಗಿ xy2win(x, y, popup);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Called when mouse should be moved to window with focus.
 */
    ಶೂನ್ಯ
gui_mouse_correct(ಶೂನ್ಯ)
{
    win_T	*wp = ಖಾಲಿ;

    need_mouse_correct = FALSE;

    wp = gui_mouse_ಕಿಟಕಿ(IGNORE_POPUP);
    ಆದರೆ (wp == curwin || wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // If in other than current window
    validate_cline_ಅಡ್ಡಸಾಲು();
    gui_mch_setmouse((ಪೂರ್ಣಾಂಕ)W_ENDCOL(curwin) * gui.char_ಅಗಲ - 3,
	    (W_WINROW(curwin) + curwin->w_wrow) * gui.char_ಎತ್ತರ
	    + (gui.char_ಎತ್ತರ) / 2);
}

/*
 * Find window where the mouse pointer "x" / "y" coordinate is in.
 * As a side effect update the shape of the mouse pointer.
 */
    ಸ್ಥಿರ win_T *
xy2win(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, mouse_ಕಂಡುಹಿಡಿ_T popup)
{
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    win_T	*wp;

    ಅಡ್ಡಸಾಲು = Y_2_ಅಡ್ಡಸಾಲು(y);
    ಲಂಬಸಾಲು = X_2_ಲಂಬಸಾಲು(x);

    ಆದರೆ (ಅಡ್ಡಸಾಲು < 0 || ಲಂಬಸಾಲು < 0)		// before first window
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    wp = mouse_ಕಂಡುಹಿಡಿ_win(&ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, popup);
    ಆದರೆ (wp == ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
	update_mouseshape(-2);
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
    ಆದರೆ (State == ಬಹುಲಕ_HITRETURN || State == ಬಹುಲಕ_ASKMORE)
    {
	ಆದರೆ (Y_2_ಅಡ್ಡಸಾಲು(y) >= ಸಂದೇಶ_ಅಡ್ಡಸಾಲು)
	    update_mouseshape(SHAPE_IDX_MOREL);
	ಇಲ್ಲದಿದ್ದರೆ
	    update_mouseshape(SHAPE_IDX_ಇನ್ನಷ್ಟು);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಡ್ಡಸಾಲು >= wp->w_ಎತ್ತರ + wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ)	// below status line
	update_mouseshape(SHAPE_IDX_CLINE);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(State & ಬಹುಲಕ_CMDLINE) && wp->w_vsep_ಅಗಲ > 0 && ಲಂಬಸಾಲು == wp->w_ಅಗಲ
	    && (!(ಅಡ್ಡಸಾಲು >= wp->w_ಎತ್ತರ && ಅಡ್ಡಸಾಲು < wp->w_ಎತ್ತರ
	    + wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ) || !stl_connected(wp)) && ಸಂದೇಶ_scrolled == 0)
	update_mouseshape(SHAPE_IDX_VSEP);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(State & ಬಹುಲಕ_CMDLINE) && wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ > 0
	    && ಅಡ್ಡಸಾಲು >= wp->w_ಎತ್ತರ && ಅಡ್ಡಸಾಲು < wp->w_ಎತ್ತರ + wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ
	    && ಸಂದೇಶ_scrolled == 0)
	update_mouseshape(SHAPE_IDX_ಸ್ಥಿತಿ);
    ಇಲ್ಲದಿದ್ದರೆ
	update_mouseshape(-2);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ wp;
}

/*
 * ":gui" and ":gvim": Change from the terminal version to the GUI version.
 * File names may be given to redefine the args list.
 */
    ಶೂನ್ಯ
ex_gui(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    /*
     * Check for "-f" argument: foreground, don't fork.
     * Also don't fork when started with "gvim -f".
     * Do fork when using "gui -b".
     */
    ಆದರೆ (arg[0] == '-'
	    && (arg[1] == 'f' || arg[1] == 'b')
	    && (arg[2] == NUL || VIM_ISWHITE(arg[2])))
    {
	gui.dofork = (arg[1] == 'b');
	eap->arg = skipwhite(eap->arg + 2);
    }
    ಆದರೆ (!gui.ಒಳ_use)
    {
#ಆದರೆ defined(VIMDLL) && !defined(EXPERIMENTAL_GUI_CMD)
	ಆದರೆ (!gui.starting)
	{
	    emsg(_(e_gui_cannot_be_ಬಳಸಿದ_not_enabled_at_compile_ಸಮಯ));
	    ಹಿಂತಿರುಗಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ
	// Clear the command.  Needed for when forking+exiting, to avoid part
	// of the argument ending up after the shell prompt.
	ಸಂದೇಶ_clr_eos_force();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GUI_MAY_SPAWN
	ಆದರೆ (!ತುದಿಗಳು_excmd2(eap->cmd, eap->arg))
	    gui_ಆರಂಭ(eap->arg);
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    gui_ಆರಂಭ(ಖಾಲಿ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	channel_gui_ರಿಜಿಸ್ಟರ್_all();
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಆದರೆ (!ತುದಿಗಳು_excmd2(eap->cmd, eap->arg))
	ex_ಮುಂದಿನ(eap);
}

#ಆದರೆ (defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	    || defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON) \
	    || defined(FEAT_GUI_HAIKU)) \
	    && defined(FEAT_TOOLBAR)
/*
 * This is shared between Haiku, Motif, and GTK.
 */

/*
 * Callback function for do_in_runtimepath().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gfp_setname(char_u *fname, ಶೂನ್ಯ *cookie)
{
    char_u	*gfp_ಬಫರ್ = cookie;

    ಆದರೆ (STRLEN(fname) >= MAXPATHL)
	*gfp_ಬಫರ್ = NUL;
    ಇಲ್ಲದಿದ್ದರೆ
	STRCPY(gfp_ಬಫರ್, fname);
}

/*
 * Find the path of bitmap "name" with extension "ext" in 'runtimepath'.
 * Return FAIL for failure and OK if buffer[MAXPATHL] contains the result.
 */
    ಪೂರ್ಣಾಂಕ
gui_ಕಂಡುಹಿಡಿ_bitmap(char_u *name, char_u *ಬಫರ್, ಅಕ್ಷರ *ext)
{
    ಆದರೆ (STRLEN(name) > MAXPATHL - 14)
	ಹಿಂತಿರುಗಿ FAIL;
    vim_snprintf((ಅಕ್ಷರ *)ಬಫರ್, MAXPATHL, "bitmaps/%s.%s", name, ext);
    ಆದರೆ (do_ಒಳ_runtimepath(ಬಫರ್, 0, gfp_setname, ಬಫರ್) == FAIL
							    || *ಬಫರ್ == NUL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

# ಆದರೆ !defined(FEAT_GUI_GTK)
/*
 * Given the name of the "icon=" argument, try finding the bitmap file for the
 * icon.  If it is an absolute path name, use it as it is.  Otherwise append
 * "ext" and search for it in 'runtimepath'.
 * The result is put in "buffer[MAXPATHL]".  If something fails "buffer"
 * contains "name".
 */
    ಶೂನ್ಯ
gui_ಕಂಡುಹಿಡಿ_iconfile(char_u *name, char_u *ಬಫರ್, ಅಕ್ಷರ *ext)
{
    char_u	buf[MAXPATHL + 1];

    expand_env(name, ಬಫರ್, MAXPATHL);
    ಆದರೆ (!mch_is_ತುಂಬಿದ_Name(ಬಫರ್) && gui_ಕಂಡುಹಿಡಿ_bitmap(ಬಫರ್, buf, ext) == OK)
	STRCPY(ಬಫರ್, buf);
}
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)|| defined(FEAT_GUI_HAIKU)
    ಶೂನ್ಯ
display_errors(ಶೂನ್ಯ)
{
    char_u	*p;

    ಆದರೆ (isatty(2))
    {
	fflush(stderr);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (error_ga.ga_ದತ್ತಾಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // avoid putting up a message box with blanks only
    ಗಾಗಿ (p = (char_u *)error_ga.ga_ದತ್ತಾಂಶ; *p != NUL; ++p)
	ಆದರೆ (!ಸುರಕ್ಷಿತ_isspace(*p))
	{
	    // Truncate a very long message, it will go off-screen.
	    ಆದರೆ (STRLEN(p) > 2000)
		STRCPY(p + 2000 - 14, "...(truncated)");
	    (ಶೂನ್ಯ)do_dialog(VIM_ERROR, (char_u *)_("Error"),
		    p, (char_u *)_("&Ok"), 1, ಖಾಲಿ, FALSE);
	    ನಿಲ್ಲಿಸು;
	}
    ga_clear(&error_ga);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(NO_CONSOLE_ಒಳಹರಿವು)
/*
 * Return TRUE if still starting up and there is no place to enter text.
 * For GTK and X11 we check if stderr is not a tty, which means we were
 * (probably) started from the desktop.  Also check stdin, "vim >& file" does
 * allow typing on stdin.
 */
    ಪೂರ್ಣಾಂಕ
no_console_ಒಳಹರಿವು(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ((!gui.ಒಳ_use || gui.starting)
# ifndef NO_CONSOLE
	    && !isatty(0) && !isatty(2)
# endif
	    );
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(ಕಂಡುಹಿಡಿ_REPLACE_DIALOG) \
	|| defined(NEED_GUI_UPDATE_SCREEN)
/*
 * Update the current window and the screen.
 */
    ಶೂನ್ಯ
gui_update_screen(ಶೂನ್ಯ)
{
# ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_ಸಾಲು = 0;
    linenr_T	conceal_new_cursor_ಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		conceal_update_ಸಾಲುಗಳು = FALSE;
# endif

    update_topline();
    validate_cursor();

    // Trigger CursorMoved if the cursor moved.
    ಆದರೆ (!ಮುಕ್ತಾಯ_op && (has_cursormoved()
# ifdef FEAT_PROP_POPUP
		|| popup_visible
# endif
# ifdef FEAT_CONCEAL
		|| curwin->w_p_cole > 0
# endif
		) && !EQUAL_POS(ಕೊನೆಯ_cursormoved, curwin->w_cursor))
    {
	ಆದರೆ (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
# ifdef FEAT_PROP_POPUP
	ಆದರೆ (popup_visible)
	    popup_check_cursor_pos();
# endif
# ifdef FEAT_CONCEAL
	ಆದರೆ (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_ಸಾಲು = ಕೊನೆಯ_cursormoved.lnum;
	    conceal_new_cursor_ಸಾಲು = curwin->w_cursor.lnum;
	    conceal_update_ಸಾಲುಗಳು = TRUE;
	}
# endif
	ಕೊನೆಯ_cursormoved = curwin->w_cursor;
    }

    ಆದರೆ (!ಮುಕ್ತಾಯ_op)
	may_trigger_win_scrolled_resized();

# ifdef FEAT_CONCEAL
    ಆದರೆ (conceal_update_ಸಾಲುಗಳು
	    && (conceal_old_cursor_ಸಾಲು != conceal_new_cursor_ಸಾಲು
		|| conceal_cursor_ಸಾಲು(curwin)
		|| need_cursor_ಸಾಲು_redraw))
    {
	ಆದರೆ (conceal_old_cursor_ಸಾಲು != conceal_new_cursor_ಸಾಲು)
	    redrawWinline(curwin, conceal_old_cursor_ಸಾಲು);
	redrawWinline(curwin, conceal_new_cursor_ಸಾಲು);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_ಸಾಲು_redraw = FALSE;
    }
# endif
    update_screen(0);	// may need to update the screen
    setcursor();
    ಹೊರಹೋಗು_flush_cursor(TRUE, FALSE);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(ಕಂಡುಹಿಡಿ_REPLACE_DIALOG)
/*
 * Get the text to use in a find/replace dialog.  Uses the last search pattern
 * if the argument is empty.
 * Returns an allocated string.
 */
    char_u *
get_ಕಂಡುಹಿಡಿ_dialog_ಪಠ್ಯ(
    char_u	*arg,
    ಪೂರ್ಣಾಂಕ		*wwordp,	// return: TRUE if \< \> found
    ಪೂರ್ಣಾಂಕ		*mcasep)	// return: TRUE if \C found
{
    char_u	*ಪಠ್ಯ;

    ಆದರೆ (*arg == NUL)
	ಪಠ್ಯ = ಕೊನೆಯ_ಹುಡುಕು_pat();
    ಇಲ್ಲದಿದ್ದರೆ
	ಪಠ್ಯ = arg;
    ಆದರೆ (ಪಠ್ಯ != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ len = (ಪೂರ್ಣಾಂಕ)STRLEN(ಪಠ್ಯ);

	ಪಠ್ಯ = vim_strnsave(ಪಠ್ಯ, len);
	ಆದರೆ (ಪಠ್ಯ != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ i;

	    // Remove "\V"
	    ಆದರೆ (len >= 2 && STRNCMP(ಪಠ್ಯ, "\\V", 2) == 0)
	    {
		mch_memmove(ಪಠ್ಯ, ಪಠ್ಯ + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\c" and "\C" and remove.
	    ಆದರೆ (len >= 2 && *ಪಠ್ಯ == '\\' && (ಪಠ್ಯ[1] == 'c' || ಪಠ್ಯ[1] == 'C'))
	    {
		*mcasep = (ಪಠ್ಯ[1] == 'C');
		mch_memmove(ಪಠ್ಯ, ಪಠ್ಯ + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\<text\>" and remove.
	    ಆದರೆ (len >= 4
		    && STRNCMP(ಪಠ್ಯ, "\\<", 2) == 0
		    && STRNCMP(ಪಠ್ಯ + len - 2, "\\>", 2) == 0)
	    {
		*wwordp = TRUE;
		mch_memmove(ಪಠ್ಯ, ಪಠ್ಯ + 2, (size_t)(len - 4));
		ಪಠ್ಯ[len - 4] = NUL;
	    }

	    // Recognize "\/" or "\?" and remove.
	    ಗಾಗಿ (i = 0; i + 1 < len; ++i)
		ಆದರೆ (ಪಠ್ಯ[i] == '\\' && (ಪಠ್ಯ[i + 1] == '/'
						       || ಪಠ್ಯ[i + 1] == '?'))
		{
		    mch_memmove(ಪಠ್ಯ + i, ಪಠ್ಯ + i + 1, (size_t)(len - i));
		    --len;
		}
	}
    }
    ಹಿಂತಿರುಗಿ ಪಠ್ಯ;
}

/*
 * Handle the press of a button in the find-replace dialog.
 * Return TRUE when something was added to the input buffer.
 */
    ಪೂರ್ಣಾಂಕ
gui_do_findrepl(
    ಪೂರ್ಣಾಂಕ		flags,		// one of FRD_REPLACE, FRD_FINDNEXT, etc.
    char_u	*ಕಂಡುಹಿಡಿ_ಪಠ್ಯ,
    char_u	*repl_ಪಠ್ಯ,
    ಪೂರ್ಣಾಂಕ		ಕೆಳಕ್ಕೆ)		// Search downwards.
{
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		type = (flags & FRD_TYPE_ಮುಖವಾಡ);
    char_u	*p;
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಂಕ		save_did_emsg = did_emsg;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  busy = FALSE;

    // When the screen is being updated we should not change buffers and
    // windows structures, it may cause freed memory to be used.  Also don't
    // do this recursively (pressing "Find" quickly several times).
    ಆದರೆ (updating_screen || busy)
	ಹಿಂತಿರುಗಿ FALSE;

    // refuse replace when text cannot be changed
    ಆದರೆ ((type == FRD_REPLACE || type == FRD_REPLACEALL) && ಪಠ್ಯ_locked())
	ಹಿಂತಿರುಗಿ FALSE;

    busy = TRUE;

    ga_init2(&ga, 1, 100);
    ಆದರೆ (type == FRD_REPLACEALL)
	GA_CONCAT_LITERAL(&ga, "%s/");

    GA_CONCAT_LITERAL(&ga, "\\V");
    ಆದರೆ (flags & FRD_ಹೊಂದಿಕೆ_CASE)
	GA_CONCAT_LITERAL(&ga, "\\C");
    ಇಲ್ಲದಿದ್ದರೆ
	GA_CONCAT_LITERAL(&ga, "\\c");
    ಆದರೆ (flags & FRD_ಸಂಪೂರ್ಣ_ಶಬ್ದ)
	GA_CONCAT_LITERAL(&ga, "\\<");
    // escape slash and backslash
    p = vim_strsave_escaped(ಕಂಡುಹಿಡಿ_ಪಠ್ಯ, (char_u *)"/\\");
    ಆದರೆ (p != ಖಾಲಿ)
	ga_concat(&ga, p);
    vim_free(p);
    ಆದರೆ (flags & FRD_ಸಂಪೂರ್ಣ_ಶಬ್ದ)
	GA_CONCAT_LITERAL(&ga, "\\>");

    ಆದರೆ (type == FRD_REPLACEALL)
    {
	GA_CONCAT_LITERAL(&ga, "/");
	// Escape slash and backslash.
	// Also escape tilde and ampersand if 'magic' is set.
	p = vim_strsave_escaped(repl_ಪಠ್ಯ,
				p_magic ? (char_u *)"/\\~&" : (char_u *)"/\\");
	ಆದರೆ (p != ಖಾಲಿ)
	    ga_concat(&ga, p);
	vim_free(p);
	GA_CONCAT_LITERAL(&ga, "/g");
    }
    ga_append(&ga, NUL);

    ಆದರೆ (type == FRD_REPLACE)
    {
	// Do the replacement when the text at the cursor matches.  Thus no
	// replacement is done if the cursor was moved!
	regmatch.regprog = vim_regcomp(ga.ga_ದತ್ತಾಂಶ, RE_MAGIC + RE_STRING);
	regmatch.rm_ic = 0;
	ಆದರೆ (regmatch.regprog != ಖಾಲಿ)
	{
	    p = ml_get_cursor();
	    ಆದರೆ (vim_regexec_nl(&regmatch, p, (colnr_T)0)
						   && regmatch.startp[0] == p)
	    {
		// Clear the command line to remove any old "No match"
		// error.
		ಸಂದೇಶ_ಕೊನೆ_prompt();

		ಆದರೆ (u_save_cursor() == OK)
		{
		    // A button was pressed thus undo should be synced.
		    u_sync(FALSE);

		    del_ಬೈಟುಗಳು((ದೀರ್ಘ)(regmatch.endp[0] - regmatch.startp[0]),
								FALSE, FALSE);
		    ins_str(repl_ಪಠ್ಯ, STRLEN(repl_ಪಠ್ಯ));
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಸಂದೇಶ(_("No match at cursor, finding next"));
	    vim_regfree(regmatch.regprog);
	}
    }

    ಆದರೆ (type == FRD_REPLACEALL)
    {
	// A button was pressed, thus undo should be synced.
	u_sync(FALSE);
	do_cmdline_cmd(ga.ga_ದತ್ತಾಂಶ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ searchflags = ಹುಡುಕು_ಸಂದೇಶ + ಹುಡುಕು_MARK;

	// Search for the next match.
	// Don't skip text under cursor for single replace.
	ಆದರೆ (type == FRD_REPLACE)
	    searchflags += ಹುಡುಕು_ಆರಂಭ;
	i = ಸಂದೇಶ_scroll;
	ಆದರೆ (ಕೆಳಕ್ಕೆ)
	{
	    (ಶೂನ್ಯ)do_ಹುಡುಕು(ಖಾಲಿ, '/', '/', ga.ga_ದತ್ತಾಂಶ, STRLEN(ga.ga_ದತ್ತಾಂಶ), 1L, searchflags, ಖಾಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // We need to escape '?' if and only if we are searching in the up
	    // direction
	    p = vim_strsave_escaped(ga.ga_ದತ್ತಾಂಶ, (char_u *)"?");
	    ಆದರೆ (p != ಖಾಲಿ)
		(ಶೂನ್ಯ)do_ಹುಡುಕು(ಖಾಲಿ, '?', '?', p, STRLEN(p), 1L, searchflags, ಖಾಲಿ);
	    vim_free(p);
	}

	ಸಂದೇಶ_scroll = i;	    // don't let an error message set msg_scroll
    }

    // Don't want to pass did_emsg to other code, it may cause disabling
    // syntax HL if we were busy redrawing.
    did_emsg = save_did_emsg;

    ಆದರೆ (State & (ಬಹುಲಕ_ಸಾಮಾನ್ಯ | ಬಹುಲಕ_ಸೇರಿಸು))
    {
	gui_update_screen();		// update the screen
	ಸಂದೇಶ_didout = 0;			// overwrite any message
	need_wait_return = FALSE;	// don't wait for return
    }

    vim_free(ga.ga_ದತ್ತಾಂಶ);
    busy = FALSE;
    ಹಿಂತಿರುಗಿ (ga.ga_len > 0);
}

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_DROP_FILE)
/*
 * Jump to the window at specified point (x, y).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_wingoto_xy(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು = Y_2_ಅಡ್ಡಸಾಲು(y);
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = X_2_ಲಂಬಸಾಲು(x);
    win_T	*wp;

    ಆದರೆ (ಅಡ್ಡಸಾಲು < 0 || ಲಂಬಸಾಲು < 0)
	ಹಿಂತಿರುಗಿ;

    wp = mouse_ಕಂಡುಹಿಡಿ_win(&ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, FAIL_POPUP);
    ಆದರೆ (wp != ಖಾಲಿ && wp != curwin)
	win_goto(wp);
}

/*
 * Function passed to handle_drop() for the actions to be done after the
 * argument list has been updated.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
drop_callback(ಶೂನ್ಯ *cookie)
{
    char_u	*p = cookie;
    ಪೂರ್ಣಾಂಕ		do_shorten = FALSE;

    // If Shift held down, change to first file's directory.  If the first
    // item is a directory, change to that directory (and let the explorer
    // plugin show the contents).
    ಆದರೆ (p != ಖಾಲಿ)
    {
	ಆದರೆ (mch_isdir(p))
	{
	    ಆದರೆ (mch_chdir((ಅಕ್ಷರ *)p) == 0)
		do_shorten = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_chdirfile(p, "drop") == OK)
	    do_shorten = TRUE;
	vim_free(p);
	ಆದರೆ (do_shorten)
	{
	    shorten_fnames(TRUE);
	    ಕೊನೆಯ_chdir_reason = "drop";
	}
    }

    // Update the screen display
    update_screen(UPD_NOT_VALID);
# ifdef FEAT_MENU
    gui_update_menus(0);
# endif
    maketitle();
    setcursor();
    ಹೊರಹೋಗು_flush_cursor(FALSE, FALSE);
}

/*
 * Process file drop.  Mouse cursor position, key modifiers, name of files
 * and count of files are given.  Argument "fnames[count]" has full pathnames
 * of dropped files, they will be freed in this function, and caller can't use
 * fnames after call this function.
 */
    ಶೂನ್ಯ
gui_handle_drop(
    ಪೂರ್ಣಾಂಕ		x ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		y ಬಳಸದ,
    int_u	modifiers,
    char_u	**fnames,
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*p;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	entered = FALSE;

    /*
     * This function is called by event handlers.  Just in case we get a
     * second event before the first one is handled, ignore the second one.
     * Not sure if this can ever happen, just in case.
     */
    ಆದರೆ (entered)
	ಹಿಂತಿರುಗಿ;
    entered = TRUE;

    /*
     * When the cursor is at the command line, add the file names to the
     * command line, don't edit the files.
     */
    ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
    {
	shorten_filenames(fnames, ಎಣಿಕೆ_2);
	ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
	{
	    ಆದರೆ (fnames[i] != ಖಾಲಿ)
	    {
		ಆದರೆ (i > 0)
		    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf((char_u*)" ", 1);

		// We don't know what command is used thus we can't be sure
		// about which characters need to be escaped.  Only escape the
		// most common ones.
# ifdef BACKSLASH_ಒಳ_FILENAME
		p = vim_strsave_escaped(fnames[i], (char_u *)" \t\"|");
# ಇಲ್ಲದಿದ್ದರೆ
		p = vim_strsave_escaped(fnames[i], (char_u *)"\\ \t\"|");
# endif
		ಆದರೆ (p != ಖಾಲಿ)
		    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf_csi(p, (ಪೂರ್ಣಾಂಕ)STRLEN(p));
		vim_free(p);
		vim_free(fnames[i]);
	    }
	}
	vim_free(fnames);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Go to the window under mouse cursor, then shorten given "fnames" by
	// current window, because a window can have local current dir.
	gui_wingoto_xy(x, y);
	shorten_filenames(fnames, ಎಣಿಕೆ_2);

	// If Shift held down, remember the first item.
	ಆದರೆ ((modifiers & MOUSE_ಸರಿಸು) != 0)
	    p = vim_strsave(fnames[0]);
	ಇಲ್ಲದಿದ್ದರೆ
	    p = ಖಾಲಿ;

	// Handle the drop, :edit or :split to get to the file.  This also
	// frees fnames[].  Skip this if there is only one item, it's a
	// directory and Shift is held down.
	ಆದರೆ (ಎಣಿಕೆ_2 == 1 && (modifiers & MOUSE_ಸರಿಸು) != 0
						     && mch_isdir(fnames[0]))
	{
	    vim_free(fnames[0]);
	    vim_free(fnames);
	    vim_free(p);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    handle_drop(ಎಣಿಕೆ_2, fnames, (modifiers & MOUSE_CTRL) != 0,
						     drop_callback, (ಶೂನ್ಯ *)p);
    }

    entered = FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Check if "key" is to interrupt us.  Handles a key that has not had modifiers
 * applied yet.
 * Return the key with modifiers applied if so, NUL if not.
 */
    ಪೂರ್ಣಾಂಕ
check_for_ಅಡಚಣೆ(ಪೂರ್ಣಾಂಕ ಕೀಲಿ, ಪೂರ್ಣಾಂಕ modifiers_arg)
{
    ಪೂರ್ಣಾಂಕ modifiers = modifiers_arg;
    ಪೂರ್ಣಾಂಕ c = ವಿಲೀನ_modify_Other_ಕೀಲಿಗಳು(ಕೀಲಿ, &modifiers);

    ಆದರೆ ((c == Ctrl_C && ctrl_c_interrupts)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	    || (intr_char != Ctrl_C && c == intr_char)
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
	got_int = TRUE;
	ಹಿಂತಿರುಗಿ c;
    }
    ಹಿಂತಿರುಗಿ NUL;
}

/*
 * If the "--gui-log-file fname" argument is given write the dialog title and
 * message to a file and return TRUE.  Otherwise return FALSE.
 * When there is any problem opening the file or writing to the file this is
 * ignored, showing the dialog might get the test to get stuck.
 */
    ಪೂರ್ಣಾಂಕ
gui_dialog_log(char_u *title, char_u *ಸಂದೇಶ_2)
{
    char_u  *fname = get_gui_dialog_file();
    FILE    *fd;

    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    fd = mch_fopen((ಅಕ್ಷರ *)fname, "a");
    ಆದರೆ (fd != ಖಾಲಿ)
    {
	fprintf(fd, "%s: %s\n", title, ಸಂದೇಶ_2);
	fclose(fd);
    }
    ಹಿಂತಿರುಗಿ TRUE;
}
