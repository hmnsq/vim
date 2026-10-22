/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#അന്തര്ഭാവ "vim.h"

// Structure containing all the GUI information
gui_T gui;

#എങ്കിൽ defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
# define USE_SET_GUIFONTWIDE
സ്ഥിതം ശൂന്യം set_guifontwide(char_u *font_name);
#endif
സ്ഥിതം ശൂന്യം gui_check_pos(ശൂന്യം);
സ്ഥിതം ശൂന്യം gui_reset_scroll_region(ശൂന്യം);
സ്ഥിതം ശൂന്യം gui_outstr(char_u *, പൂർണ്ണസംഖ്യ);
#ifndef USE_GTK4
സ്ഥിതം പൂർണ്ണസംഖ്യ gui_screenchar(പൂർണ്ണസംഖ്യ off, പൂർണ്ണസംഖ്യ flags, guicolor_T fg, guicolor_T bg, പൂർണ്ണസംഖ്യ back);
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ gui_outstr_nowrap(char_u *s, പൂർണ്ണസംഖ്യ len, പൂർണ്ണസംഖ്യ flags, guicolor_T fg, guicolor_T bg, പൂർണ്ണസംഖ്യ back);
സ്ഥിതം ശൂന്യം gui_delete_വരികൾ(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ എണ്ണം);
സ്ഥിതം ശൂന്യം gui_തിരുകുക_വരികൾ(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ എണ്ണം);
സ്ഥിതം പൂർണ്ണസംഖ്യ gui_xy2colrow(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, പൂർണ്ണസംഖ്യ *colp);
#എങ്കിൽ defined(FEAT_GUI_TABLINE)
സ്ഥിതം പൂർണ്ണസംഖ്യ gui_has_tabline(ശൂന്യം);
#endif
സ്ഥിതം ശൂന്യം gui_do_scrollbar(win_T *wp, പൂർണ്ണസംഖ്യ which, പൂർണ്ണസംഖ്യ enable);
സ്ഥിതം ശൂന്യം gui_update_horiz_scrollbar(പൂർണ്ണസംഖ്യ);
സ്ഥിതം ശൂന്യം gui_set_fg_color(char_u *name);
സ്ഥിതം ശൂന്യം gui_set_bg_color(char_u *name);
സ്ഥിതം ശൂന്യം init_gui_options(ശൂന്യം);
സ്ഥിതം win_T *xy2win(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, mouse_find_T popup);

#ifdef GUI_MAY_FORK
സ്ഥിതം ശൂന്യം gui_do_fork(ശൂന്യം);

സ്ഥിതം പൂർണ്ണസംഖ്യ gui_read_child_pipe(പൂർണ്ണസംഖ്യ fd);

// Return values for gui_read_child_pipe
എണ്ണൽ {
    GUI_CHILD_IO_ERROR,
    GUI_CHILD_OK,
    GUI_CHILD_FAILED
};
#endif

സ്ഥിതം ശൂന്യം gui_attempt_start(ശൂന്യം);

സ്ഥിതം പൂർണ്ണസംഖ്യ can_update_cursor = TRUE; // can display the cursor
സ്ഥിതം പൂർണ്ണസംഖ്യ disable_flush = 0;	// If > 0, gui_mch_flush() is disabled.

/*
 * gui_start -- Called when user wants to start the GUI.
 *
 * Careful: This function can be called recursively when there is a ":gui"
 * command in the .gvimrc file.  Only the first call should fork, not the
 * recursive call.
 */
    ശൂന്യം
gui_start(char_u *arg UNUSED)
{
    char_u	*old_term;
#ifdef GUI_MAY_FORK
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recursive = 0;
#endif
#എങ്കിൽ defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
    അക്ഷരം	*സന്ദേശം = ശൂന്യ;
#endif

    old_term = vim_strsave(T_NAME);

    settmode(TMODE_COOK);		// stop RAW mode
    എങ്കിൽ (full_screen)
	cursor_on();			// needed for ":gui" in .vimrc
    full_screen = FALSE;

    // If GUI fails to start, then we will recover afterwards
    term_disable_dec();

#ifdef GUI_MAY_FORK
    ++recursive;
    /*
     * Quit the current process and continue in the child.
     * Makes "gvim file" disconnect from the shell it was started in.
     * Don't do this when Vim was started with "-f" or the 'f' flag is present
     * in 'guioptions'.
     * Don't do this when there is a running job, we can only get the status
     * of a child from the parent.
     */
    എങ്കിൽ (gui.dofork && !vim_strchr(p_go, GO_FORG) && recursive <= 1
# ifdef FEAT_JOB_CHANNEL
	    && !job_any_running()
# endif
	    )
    {
	gui_do_fork();
    }
    അല്ലെങ്കിൽ
#endif
#ifdef GUI_MAY_SPAWN
    എങ്കിൽ (gui.dospawn
# ifdef EXPERIMENTAL_GUI_CMD
	    && gui.dofork
# endif
	    && !vim_strchr(p_go, GO_FORG)
	    && !anyBufIsChanged()
# ifdef FEAT_JOB_CHANNEL
	    && !job_any_running()
# endif
	    )
    {
# ifdef EXPERIMENTAL_GUI_CMD
	സന്ദേശം =
# endif
	    gui_mch_do_spawn(arg);
    }
    അല്ലെങ്കിൽ
#endif
    {
#ifdef FEAT_GUI_GTK
	// If there is 'f' in 'guioptions' and specify -g argument,
	// gui_mch_init_check() was not called yet.
	എങ്കിൽ (gui_mch_init_check() != OK)
	    getout_preserve_modified(1);
#endif
	gui_attempt_start();
    }

    എങ്കിൽ (!gui.in_use)			// failed to start GUI
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
	termcapinit(old_term);
	settmode(TMODE_RAW);		// restart RAW mode
	set_title_defaults();		// set 'title' and 'icon' again
#ifdef UNIX
	term_set_win_resize(true);
#endif
#എങ്കിൽ defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
	എങ്കിൽ (സന്ദേശം != ശൂന്യ)
	    emsg(സന്ദേശം);
#endif
    }
    അല്ലെങ്കിൽ
    {
#ifdef HAVE_CLIPMETHOD
	// Reset clipmethod to CLIPMETHOD_NONE
	choose_clipmethod();
#endif
#ifdef FEAT_IMAGE
	update_cell_size();
	update_image_backend();
#endif
    }

#എങ്കിൽ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    // Enable fullscreen mode
    എങ്കിൽ (vim_strchr(p_go, GO_FULLSCREEN) != ശൂന്യ)
       gui_mch_set_fullscreen(TRUE);
#endif

    vim_free(old_term);

    // If the GUI started successfully, trigger the GUIEnter event, otherwise
    // the GUIFailed event.
    gui_mch_update();
    apply_autocmds(gui.in_use ? EVENT_GUIENTER : EVENT_GUIFAILED,
						   ശൂന്യ, ശൂന്യ, FALSE, curbuf);
#ifdef GUI_MAY_FORK
    --recursive;
#endif
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
    സ്ഥിതം ശൂന്യം
gui_attempt_start(ശൂന്യം)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ recursive = 0;

    ++recursive;
    gui.starting = true;

#ifdef FEAT_GUI_GTK
    gui.event_time = GDK_CURRENT_TIME;
#endif

    termcapinit((char_u *)"builtin_gui");
    gui.starting = recursive - 1;

#എങ്കിൽ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
    എങ്കിൽ (gui.in_use)
    {
# ifdef FEAT_EVAL
	Window	x11_window;
	Display	*x11_display;

	എങ്കിൽ (gui_get_x11_windis(&x11_window, &x11_display) == OK)
	    set_vim_var_nr(VV_WINDOWID, (long)x11_window);
# endif
	// Display error messages in a dialog now.
	display_errors();
    }
#endif
    --recursive;
}

#ifdef GUI_MAY_FORK

// for waitpid()
# എങ്കിൽ defined(HAVE_SYS_WAIT_H)
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
    സ്ഥിതം ശൂന്യം
gui_do_fork(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		pipefd[2];	// pipe between parent and child
    പൂർണ്ണസംഖ്യ		pipe_error;
    പൂർണ്ണസംഖ്യ		status;
    പൂർണ്ണസംഖ്യ		exit_status;
    pid_t	pid = -1;

# എങ്കിൽ defined(FEAT_RELTIME) && defined(PROF_NSEC)
    // a timer is not carried forward
    delete_timer();
# endif

    // Setup a pipe between the child and the parent, so that the parent
    // knows when the child has done the setsid() call and is allowed to
    // exit.
    pipe_error = (pipe(pipefd) < 0);
    pid = fork();
    എങ്കിൽ (pid < 0)	    // Fork error
    {
	emsg(_(e_failed_to_create_new_process_for_GUI));
	തിരികെ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (pid > 0)	    // Parent
    {
	// Give the child some time to do the setsid(), otherwise the
	// exit() may kill the child too (when starting gvim from inside a
	// gvim).
	എങ്കിൽ (!pipe_error)
	{
	    // The read returns when the child closes the pipe (or when
	    // the child dies for some reason).
	    close(pipefd[1]);
	    status = gui_read_child_pipe(pipefd[0]);
	    എങ്കിൽ (status == GUI_CHILD_FAILED)
	    {
		// The child failed to start the GUI, so the caller must
		// continue. There may be more error information written
		// to stderr by the child.
		waitpid(pid, &exit_status, 0);
		emsg(_(e_the_child_process_failed_to_start_GUI));
		തിരികെ;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (status == GUI_CHILD_IO_ERROR)
	    {
		pipe_error = TRUE;
	    }
	    // else GUI_CHILD_OK: parent exit
	}

	എങ്കിൽ (pipe_error)
	    ui_delay(301L, TRUE);

	// When swapping screens we may need to go to the next line, e.g.,
	// after a hit-enter prompt and using ":gui".
	എങ്കിൽ (newline_on_exit)
	    mch_errmsg("\r\n");

	/*
	 * The parent must skip the normal exit() processing, the child
	 * will do it.  For example, GTK messes up signals when exiting.
	 */
	_exit(0);
    }
    // Child

# ifdef FEAT_GUI_GTK
    // Call gtk_init_check() here after fork(). See gui_init_check().
    എങ്കിൽ (gui_mch_init_check() != OK)
	getout_preserve_modified(1);
# endif

# എങ്കിൽ defined(HAVE_SETSID) || defined(HAVE_SETPGID)
    /*
     * Change our process group.  On some systems/shells a CTRL-C in the
     * shell where Vim was started would otherwise kill gvim!
     */
#  എങ്കിൽ defined(HAVE_SETSID)
    (ശൂന്യം)setsid();
#  അല്ലെങ്കിൽ
    (ശൂന്യം)setpgid(0, 0);
#  endif
# endif
    എങ്കിൽ (!pipe_error)
	close(pipefd[0]);

# എങ്കിൽ defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
    // Tell the session manager our new PID
    gui_mch_forked();
# endif

    // Try to start the GUI
    gui_attempt_start();

    // Notify the parent
    എങ്കിൽ (!pipe_error)
    {
	എങ്കിൽ (gui.in_use)
	    write_eintr(pipefd[1], "ok", 3);
	അല്ലെങ്കിൽ
	    write_eintr(pipefd[1], "fail", 5);
	close(pipefd[1]);
    }

    // If we failed to start the GUI, exit now.
    എങ്കിൽ (!gui.in_use)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_read_child_pipe(പൂർണ്ണസംഖ്യ fd)
{
    long	bytes_read;
# define READ_BUFFER_SIZE 10
    അക്ഷരം	buffer[READ_BUFFER_SIZE];

    bytes_read = read_eintr(fd, buffer, READ_BUFFER_SIZE - 1);
# undef READ_BUFFER_SIZE
    close(fd);
    എങ്കിൽ (bytes_read < 0)
	തിരികെ GUI_CHILD_IO_ERROR;
    buffer[bytes_read] = NUL;
    എങ്കിൽ (STRCMP(buffer, "ok") == 0)
	തിരികെ GUI_CHILD_OK;
    തിരികെ GUI_CHILD_FAILED;
}

#endif // GUI_MAY_FORK

/*
 * Call this when vim starts up, whether or not the GUI is started
 */
    ശൂന്യം
gui_prepare(പൂർണ്ണസംഖ്യ *argc, അക്ഷരം **argv)
{
    gui.in_use = false;		    // No GUI yet (maybe later)
    gui.starting = false;	    // No GUI yet (maybe later)
    gui_mch_prepare(argc, argv);
}

/*
 * Try initializing the GUI and check if it can be started.
 * Used from main() to check early if "vim -g" can start the GUI.
 * Used from gui_init() to prepare for starting the GUI.
 * Returns FAIL or OK.
 */
    പൂർണ്ണസംഖ്യ
gui_init_check(ശൂന്യം)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ result = MAYBE;

    എങ്കിൽ (result != MAYBE)
    {
	എങ്കിൽ (result == FAIL)
	    emsg(_(e_cannot_start_the_GUI));
	തിരികെ result;
    }

    gui.shell_created = false;
    gui.dying = false;
    gui.in_focus = true;		// so the guicursor setting works
    gui.dragged_sb = SBAR_NONE;
    gui.dragged_wp = ശൂന്യ;
    gui.pointer_hidden = false;
    gui.col = 0;
    gui.row = 0;
    gui.അക്കം_cols = Columns;
    gui.അക്കം_rows = Rows;

    gui.cursor_is_valid = false;
    gui.scroll_region_മുകൾ = 0;
    gui.scroll_region_bot = Rows - 1;
    gui.scroll_region_ഇടത് = 0;
    gui.scroll_region_വലത് = Columns - 1;
    gui.highlight_mask = HL_NORMAL;
    gui.char_width = 1;
    gui.char_height = 1;
    gui.char_ascent = 0;
    gui.border_width = 0;

    gui.norm_font = NOFONT;
#ifndef FEAT_GUI_GTK
    gui.bold_font = NOFONT;
    gui.ital_font = NOFONT;
    gui.boldital_font = NOFONT;
# ifdef FEAT_XFONTSET
    gui.fontset = NOFONTSET;
# endif
#endif
    gui.wide_font = NOFONT;
#ifndef FEAT_GUI_GTK
    gui.wide_bold_font = NOFONT;
    gui.wide_ital_font = NOFONT;
    gui.wide_boldital_font = NOFONT;
#endif

#ifdef FEAT_MENU
# ifndef FEAT_GUI_GTK
#  ifdef FONTSET_ALWAYS
    gui.menu_fontset = NOFONTSET;
#  അല്ലെങ്കിൽ
    gui.menu_font = NOFONT;
#  endif
# endif
    gui.menu_is_active = true;	    // default: include menu
# ifndef FEAT_GUI_GTK
    gui.menu_height = MENU_DEFAULT_HEIGHT;
    gui.menu_width = 0;
# endif
#endif
#എങ്കിൽ defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    gui.toolbar_height = 0;
#endif
#ifdef FEAT_BEVAL_TIP
    gui.tooltip_fontset = NOFONTSET;
#endif

    gui.scrollbar_width = gui.scrollbar_height = SB_DEFAULT_WIDTH;
    gui.prev_wrap = -1;

#എങ്കിൽ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // Note: gui_set_ligatures() might already have been called e.g. from .vimrc,
    // and in that case we don't want to overwrite ligatures map that has already
    // been correctly populated (as that would lead to a cleared ligatures maps).
    എങ്കിൽ (*p_guiligatures == NUL)
	CLEAR_FIELD(gui.ligatures_map);
#endif

#എങ്കിൽ defined(ALWAYS_USE_GUI) || defined(VIMDLL)
    result = OK;
#അല്ലെങ്കിൽ
# ifdef FEAT_GUI_GTK
    gui.is_x11 = false;
#  ifdef FEAT_GUI_DIALOG
    gui.dialogs_active = 0;
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
    result = gui.dofork ? gui_mch_early_init_check(TRUE) : gui_mch_init_check();
# അല്ലെങ്കിൽ
    result = gui_mch_init_check();
# endif
#endif
    തിരികെ result;
}

/*
 * This is the call which starts the GUI.
 */
    ശൂന്യം
gui_init(ശൂന്യം)
{
    win_T	*wp;
    tabpage_T	*tp;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recursive = 0;

    /*
     * It's possible to use ":gui" in a .gvimrc file.  The first half of this
     * function will then be executed at the first call, the rest by the
     * recursive call.  This allow the shell to be opened halfway reading a
     * gvimrc file.
     */
    എങ്കിൽ (!recursive)
    {
	++recursive;

	clip_init(TRUE);

	// If can't initialize, don't try doing the rest
	എങ്കിൽ (gui_init_check() == FAIL)
	{
	    --recursive;
	    clip_init(FALSE);
	    തിരികെ;
	}

	/*
	 * Reset 'paste'.  It's useful in the terminal, but not in the GUI.  It
	 * breaks the Paste toolbar button.
	 */
	set_option_മൂല്യം_give_err((char_u *)"paste", 0L, ശൂന്യ, 0);

	// Set t_Co to the number of colors: RGB.
	set_color_എണ്ണം(256 * 256 * 256);

	/*
	 * Set up system-wide default menus.
	 */
#എങ്കിൽ defined(SYS_MENU_FILE) && defined(FEAT_MENU)
	എങ്കിൽ (vim_strchr(p_go, GO_NOSYSMENU) == ശൂന്യ)
	{
	    sys_menu = TRUE;
	    do_source((char_u *)SYS_MENU_FILE, FALSE, DOSO_NONE, ശൂന്യ);
	    sys_menu = FALSE;
	}
#endif

	/*
	 * Switch on the mouse by default, unless the user changed it already.
	 * This can then be changed in the .gvimrc.
	 */
	എങ്കിൽ (!option_was_set((char_u *)"mouse"))
	    set_string_option_direct((char_u *)"mouse", -1,
					   (char_u *)"a", OPT_FREE, SID_NONE);

	/*
	 * If -U option given, use only the initializations from that file and
	 * nothing else.  Skip all initializations for "-U NONE" or "-u NORC".
	 */
	എങ്കിൽ (use_gvimrc != ശൂന്യ)
	{
	    എങ്കിൽ (STRCMP(use_gvimrc, "NONE") != 0
		    && STRCMP(use_gvimrc, "NORC") != 0
		    && do_source(use_gvimrc, FALSE, DOSO_NONE, ശൂന്യ) != OK)
		semsg(_(e_cannot_read_from_str), use_gvimrc);
	}
	അല്ലെങ്കിൽ
	{
	    /*
	     * Get system wide defaults for gvim, only when file name defined.
	     */
#ifdef SYS_GVIMRC_FILE
	    do_source((char_u *)SYS_GVIMRC_FILE, FALSE, DOSO_NONE, ശൂന്യ);
#endif

	    /*
	     * Try to read GUI initialization commands from the following
	     * places:
	     * - environment variable GVIMINIT
	     * - the user gvimrc file (~/.gvimrc)
	     * - the second user gvimrc file ($VIM/.gvimrc for Dos)
	     * - the third user gvimrc file ($VIM/.gvimrc for Amiga)
	     * The first that exists is used, the rest is ignored.
	     */
	    എങ്കിൽ (process_env((char_u *)"GVIMINIT", FALSE) == FAIL
		 && do_source((char_u *)USR_GVIMRC_FILE, TRUE,
						     DOSO_GVIMRC, ശൂന്യ) == FAIL
#ifdef USR_GVIMRC_FILE2
		 && do_source((char_u *)USR_GVIMRC_FILE2, TRUE,
						     DOSO_GVIMRC, ശൂന്യ) == FAIL
#endif
#ifdef USR_GVIMRC_FILE3
		 && do_source((char_u *)USR_GVIMRC_FILE3, TRUE,
						     DOSO_GVIMRC, ശൂന്യ) == FAIL
#endif
				)
	    {
#ifdef USR_GVIMRC_FILE4
		(ശൂന്യം)do_source((char_u *)USR_GVIMRC_FILE4, TRUE,
							    DOSO_GVIMRC, ശൂന്യ);
#endif
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
	    എങ്കിൽ (p_exrc)
	    {
#ifdef UNIX
		{
		    stat_T s;

		    // if ".gvimrc" file is not owned by user, set 'secure'
		    // mode
		    എങ്കിൽ (mch_stat(GVIMRC_FILE, &s) || s.st_uid != getuid())
			secure = p_secure;
		}
#അല്ലെങ്കിൽ
		secure = p_secure;
#endif

		എങ്കിൽ (       fullpathcmp((char_u *)USR_GVIMRC_FILE,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#ifdef SYS_GVIMRC_FILE
			&& fullpathcmp((char_u *)SYS_GVIMRC_FILE,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#endif
#ifdef USR_GVIMRC_FILE2
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE2,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#endif
#ifdef USR_GVIMRC_FILE3
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE3,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#endif
#ifdef USR_GVIMRC_FILE4
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE4,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#endif
			)
		    do_source((char_u *)GVIMRC_FILE, TRUE, DOSO_GVIMRC, ശൂന്യ);

		എങ്കിൽ (secure == 2)
		    need_wait_return = TRUE;
		secure = 0;
	    }
	}

	എങ്കിൽ (need_wait_return || സന്ദേശം_didany)
	    wait_return(TRUE);

	--recursive;
    }

    // If recursive call opened the shell, return here from the first call
    എങ്കിൽ (gui.in_use)
	തിരികെ;

    /*
     * Create the GUI shell.
     */
    gui.in_use = true;		// Must be set after menus have been set up
#എങ്കിൽ defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
    gui.scale = 1.0; // Default value
#endif
    എങ്കിൽ (gui_mch_init() == FAIL)
	പോകുക error;

    // Avoid a delay for an error message that was printed in the terminal
    // where Vim was started.
    emsg_on_display = FALSE;
    സന്ദേശം_scrolled = 0;
    clear_sb_text(TRUE);
    need_wait_return = FALSE;
    സന്ദേശം_didany = FALSE;

    /*
     * Check validity of any generic resources that may have been loaded.
     */
    എങ്കിൽ (gui.border_width < 0)
	gui.border_width = 0;

    /*
     * Set up the fonts.  First use a font specified with "-fn" or "-font".
     */
    എങ്കിൽ (font_argument != ശൂന്യ)
	set_option_മൂല്യം_give_err((char_u *)"gfn",
					       0L, (char_u *)font_argument, 0);
    എങ്കിൽ (
#ifdef FEAT_XFONTSET
	    (*p_guifontset == NUL
	     || gui_init_font(p_guifontset, TRUE) == FAIL) &&
#endif
	    gui_init_font(*p_guifont == NUL ? hl_get_font_name()
						  : p_guifont, FALSE) == FAIL)
    {
	emsg(_(e_cannot_start_gui_no_valid_font_found));
	പോകുക error2;
    }
    എങ്കിൽ (gui_get_wide_font() == FAIL)
	emsg(_(e_guifontwide_invalid));

    gui.അക്കം_cols = Columns;
    gui.അക്കം_rows = Rows;
    gui_reset_scroll_region();

    // Create initial scrollbars
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_ഇടത്], SBAR_ഇടത്, wp);
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_വലത്], SBAR_വലത്, wp);
    }
    gui_create_scrollbar(&gui.bottom_sbar, SBAR_BOTTOM, ശൂന്യ);

#ifdef FEAT_MENU
    gui_create_initial_menus(വേര്_menu);
#endif
#ifdef FEAT_SIGN_ICONS
    sign_gui_started();
#endif

    // Configure the desired menu and scrollbars
    gui_init_which_components(ശൂന്യ);

    // All components of the GUI have been created now
    gui.shell_created = true;

#ifdef FEAT_GUI_MSWIN
    // Set the shell size, adjusted for the screen size.  For GTK this only
    // works after the shell has been opened, thus it is further down.
    // If the window is already maximized (e.g. when --windowid is passed in),
    // we want to use the system-provided dimensions by passing FALSE to
    // mustset. Otherwise, we want to initialize with the default rows/columns.
    എങ്കിൽ (gui_mch_maximized())
	gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    അല്ലെങ്കിൽ
	gui_set_shellsize(TRUE, TRUE, RESIZE_BOTH);
#അല്ലെങ്കിൽ
# ifndef FEAT_GUI_GTK
    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
# endif
#endif
#എങ്കിൽ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    // Need to set the size of the menubar after all the menus have been
    // created.
    gui_mch_compute_menu_height((Widget)0);
#endif

    /*
     * Actually open the GUI shell.
     */
    എങ്കിൽ (gui_mch_open() != FAIL)
    {
	maketitle();
	resettitle();

	init_gui_options();
#ifdef FEAT_ARABIC
	// Our GUI can't do bidi.
	p_tbidi = FALSE;
#endif
#എങ്കിൽ defined(FEAT_GUI_GTK)
	// Give GTK+ a chance to put all widget's into place.
	gui_mch_update();

# ifdef FEAT_MENU
	// If there is no 'm' in 'guioptions' we need to remove the menu now.
	// It was still there to make F10 work.
	എങ്കിൽ (vim_strchr(p_go, GO_MENUS) == ശൂന്യ)
	{
	    bool save_starting = gui.starting;

	    gui.starting = false;
	    gui_mch_enable_menu(FALSE);
	    gui.starting = save_starting;
	    gui_mch_update();
	}
# endif

	// Now make sure the shell fits on the screen.
	എങ്കിൽ (gui_mch_maximized())
	    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
	അല്ലെങ്കിൽ
	    gui_set_shellsize(TRUE, TRUE, RESIZE_BOTH);
#endif
	// When 'lines' was set while starting up the topframe may have to be
	// resized.
	win_new_shellsize();

#ifdef FEAT_BEVAL_GUI
	// Always create the Balloon Evaluation area, but disable it when
	// 'ballooneval' is off.
	എങ്കിൽ (balloonEval != ശൂന്യ)
	{
# ifdef FEAT_VARTABS
	    vim_free(balloonEval->vts);
# endif
	    vim_free(balloonEval);
	}
	balloonEvalForTerm = FALSE;
# ifdef FEAT_GUI_GTK
	balloonEval = gui_mch_create_beval_area(gui.drawarea, ശൂന്യ,
						     &general_beval_cb, ശൂന്യ);
# അല്ലെങ്കിൽ
#  എങ്കിൽ defined(FEAT_GUI_MOTIF)
	{
	    ബാഹ്യ Widget	textArea;
	    balloonEval = gui_mch_create_beval_area(textArea, ശൂന്യ,
						     &general_beval_cb, ശൂന്യ);
	}
#  അല്ലെങ്കിൽ
#   ifdef FEAT_GUI_MSWIN
	balloonEval = gui_mch_create_beval_area(ശൂന്യ, ശൂന്യ,
						     &general_beval_cb, ശൂന്യ);
#   endif
#  endif
# endif
	എങ്കിൽ (!p_beval)
	    gui_mch_disable_beval_area(balloonEval);
#endif

#ifndef FEAT_GUI_MSWIN
	// In the GUI modifiers are prepended to keys.
	// Don't do this for MS-Windows yet, it sends CTRL-K without the
	// modifier.
	seen_Modify_Other_താക്കോലുകൾ = TRUE;
#endif

#എങ്കിൽ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	എങ്കിൽ (!im_xim_isvalid_imactivate())
	    emsg(_(e_മൂല്യം_of_imactivatekey_is_invalid));
#endif
	// When 'cmdheight' was set during startup it may not have taken
	// effect yet.
	എങ്കിൽ (p_ch != 1L)
	    command_height();

	തിരികെ;
    }

error2:
#ifdef FEAT_GUI_X11
    // undo gui_mch_init()
    gui_mch_uninit();
#endif

error:
    gui.in_use = false;
    clip_init(FALSE);
}


    ശൂന്യം
gui_exit(പൂർണ്ണസംഖ്യ rc)
{
    // don't free the fonts, it leads to a BUS error
    // richard@whitequeen.com Jul 99
    free_highlight_fonts();
    gui.in_use = false;
    gui_mch_exit(rc);
}

#എങ്കിൽ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_PHOTON)
# define NEED_GUI_UPDATE_SCREEN 1
/*
 * Called when the GUI shell is closed by the user.  If there are no changed
 * files Vim exits, otherwise there will be a dialog to ask the user what to
 * do.
 * When this function returns, Vim should NOT exit!
 */
    ശൂന്യം
gui_shell_closed(ശൂന്യം)
{
    cmdmod_T	    save_cmdmod = cmdmod;

    എങ്കിൽ (before_quit_autocmds(curwin, TRUE, FALSE))
	തിരികെ;

    // Only exit when there are no changed files
    exiting = TRUE;
# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    cmdmod.cmod_flags |= CMOD_CONFIRM;
# endif
    // If there are changed buffers, present the user with a dialog if
    // possible, otherwise give an error message.
    എങ്കിൽ (!check_changed_any(FALSE, FALSE))
	getout(0);

    exiting = FALSE;
    cmdmod = save_cmdmod;
    gui_update_screen();	// redraw, window may show changed buffer
}
#endif

/*
 * Set the font.  "font_list" is a comma separated list of font names.  The
 * first font name that works is used.  If none is found, use the default
 * font.
 * If "fontset" is TRUE, the "font_list" is used as one name for the fontset.
 * Return OK when able to set the font.  When it failed FAIL is returned and
 * the fonts are unchanged.
 */
    പൂർണ്ണസംഖ്യ
gui_init_font(char_u *font_list, പൂർണ്ണസംഖ്യ fontset UNUSED)
{
#define FONTLEN 320
    char_u	font_name[FONTLEN];
    പൂർണ്ണസംഖ്യ		font_list_empty = FALSE;
    പൂർണ്ണസംഖ്യ		ret = FAIL;

    എങ്കിൽ (!gui.in_use)
	തിരികെ FAIL;

    font_name[0] = NUL;
    എങ്കിൽ (*font_list == NUL)
	font_list_empty = TRUE;
    അല്ലെങ്കിൽ
    {
#ifdef FEAT_XFONTSET
	// When using a fontset, the whole list of fonts is one name.
	എങ്കിൽ (fontset)
	    ret = gui_mch_init_font(font_list, TRUE);
	അല്ലെങ്കിൽ
#endif
	    ആകുമ്പോൾ (*font_list != NUL)
	    {
		// Isolate one comma separated font name.
		(ശൂന്യം)copy_option_part(&font_list, font_name, FONTLEN, ",");

		// Careful!!!  The Win32 version of gui_mch_init_font(), when
		// called with "*" will change p_guifont to the selected font
		// name, which frees the old value.  This makes font_list
		// invalid.  Thus when OK is returned here, font_list must no
		// longer be used!
		എങ്കിൽ (gui_mch_init_font(font_name, FALSE) == OK)
		{
#ifdef USE_SET_GUIFONTWIDE
		    // If it's a Unicode font, try setting 'guifontwide' to a
		    // similar double-width font.
		    എങ്കിൽ ((p_guifontwide == ശൂന്യ || *p_guifontwide == NUL)
				&& strstr((അക്ഷരം *)font_name, "10646") != ശൂന്യ)
			set_guifontwide(font_name);
#endif
		    ret = OK;
		    നിർത്തുക;
		}
	    }
    }

    എങ്കിൽ (ret != OK
	    && STRCMP(font_list, "*") != 0
	    && (font_list_empty || gui.norm_font == NOFONT))
    {
	/*
	 * Couldn't load any font in 'font_list', keep the current font if
	 * there is one.  If 'font_list' is empty, or if there is no current
	 * font, tell gui_mch_init_font() to try to find a font we can load.
	 */
	ret = gui_mch_init_font(ശൂന്യ, FALSE);
    }

    എങ്കിൽ (ret == OK)
    {
#ifndef FEAT_GUI_GTK
	// Set normal font as current font
# ifdef FEAT_XFONTSET
	എങ്കിൽ (gui.fontset != NOFONTSET)
	    gui_mch_set_fontset(gui.fontset);
	അല്ലെങ്കിൽ
# endif
	    gui_mch_set_font(gui.norm_font);
#endif
	gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    }

    തിരികെ ret;
}

#ifdef USE_SET_GUIFONTWIDE
/*
 * Try setting 'guifontwide' to a font twice as wide as "name".
 */
    സ്ഥിതം ശൂന്യം
set_guifontwide(char_u *name)
{
    പൂർണ്ണസംഖ്യ		i = 0;
    char_u	wide_name[FONTLEN + 10]; // room for 2 * width and '*'
    char_u	*wp = ശൂന്യ;
    char_u	*p;
    GuiFont	font;

    wp = wide_name;
    വേണ്ടി (p = name; *p != NUL; ++p)
    {
	*wp++ = *p;
	എങ്കിൽ (*p == '-')
	{
	    ++i;
	    എങ്കിൽ (i == 6)		// font type: change "--" to "-*-"
	    {
		എങ്കിൽ (p[1] == '-')
		    *wp++ = '*';
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (i == 12)	// found the width
	    {
		++p;
		i = getdigits(&p);
		എങ്കിൽ (i != 0)
		{
		    // Double the width specification.
		    sprintf((അക്ഷരം *)wp, "%d%s", i * 2, p);
		    font = gui_mch_get_font(wide_name, FALSE);
		    എങ്കിൽ (font != NOFONT)
		    {
			gui_mch_free_font(gui.wide_font);
			gui.wide_font = font;
			set_string_option_direct((char_u *)"gfw", -1,
						      wide_name, OPT_FREE, 0);
		    }
		}
		നിർത്തുക;
	    }
	}
    }
}
#endif

/*
 * Get the font for 'guifontwide'.
 * Return FAIL for an invalid font name.
 */
    പൂർണ്ണസംഖ്യ
gui_get_wide_font(ശൂന്യം)
{
    GuiFont	font = NOFONT;
    char_u	font_name[FONTLEN];
    char_u	*p;

    എങ്കിൽ (!gui.in_use)	    // Can't allocate font yet, assume it's OK.
	തിരികെ OK;	    // Will give an error message later.

    എങ്കിൽ (p_guifontwide != ശൂന്യ && *p_guifontwide != NUL)
    {
	വേണ്ടി (p = p_guifontwide; *p != NUL; )
	{
	    // Isolate one comma separated font name.
	    (ശൂന്യം)copy_option_part(&p, font_name, FONTLEN, ",");
	    font = gui_mch_get_font(font_name, FALSE);
	    എങ്കിൽ (font != NOFONT)
		നിർത്തുക;
	}
	എങ്കിൽ (font == NOFONT)
	    തിരികെ FAIL;
    }

    gui_mch_free_font(gui.wide_font);
#ifdef FEAT_GUI_GTK
    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    എങ്കിൽ (font != NOFONT && gui.norm_font != NOFONT
			 && pango_font_description_equal(font, gui.norm_font))
    {
	gui.wide_font = NOFONT;
	gui_mch_free_font(font);
    }
    അല്ലെങ്കിൽ
#endif
	gui.wide_font = font;
#ifdef FEAT_GUI_MSWIN
    gui_mch_wide_font_changed();
#അല്ലെങ്കിൽ
    /*
     * TODO: setup wide_bold_font, wide_ital_font and wide_boldital_font to
     * support those fonts for 'guifontwide'.
     */
#endif
    തിരികെ OK;
}

#എങ്കിൽ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * Set list of ascii characters that combined can create ligature.
 * Store them in char map for quick access from gui_gtk_draw_string.
 */
    ശൂന്യം
gui_set_ligatures(ശൂന്യം)
{
    char_u	*p;

    എങ്കിൽ (*p_guiligatures != NUL)
    {
	// check for invalid characters
	വേണ്ടി (p = p_guiligatures; *p != NUL; ++p)
	    എങ്കിൽ (*p < 32 || *p > 127)
	    {
		emsg(_(e_ascii_code_not_in_range));
		തിരികെ;
	    }

	// store valid setting into ligatures_map
	CLEAR_FIELD(gui.ligatures_map);
	വേണ്ടി (p = p_guiligatures; *p != NUL; ++p)
	    gui.ligatures_map[*p] = 1;
    }
    അല്ലെങ്കിൽ
	CLEAR_FIELD(gui.ligatures_map);
}

/*
 * Adjust the columns to undraw for when the cursor is on ligatures.
 */
    സ്ഥിതം ശൂന്യം
gui_adjust_undraw_cursor_for_ligatures(പൂർണ്ണസംഖ്യ *startcol, പൂർണ്ണസംഖ്യ *endcol)
{
    പൂർണ്ണസംഖ്യ off;

    എങ്കിൽ (Screen_വരികൾ == ശൂന്യ || *p_guiligatures == NUL)
	തിരികെ;

    // expand before the cursor for all the chars in gui.ligatures_map
    off = വരി_Offset[gui.cursor_row] + *startcol;
    എങ്കിൽ (gui.ligatures_map[Screen_വരികൾ[off]])
	ആകുമ്പോൾ (*startcol > 0 && gui.ligatures_map[Screen_വരികൾ[--off]])
	    (*startcol)--;

    // expand after the cursor for all the chars in gui.ligatures_map
    off = വരി_Offset[gui.cursor_row] + *endcol;
    എങ്കിൽ (gui.ligatures_map[Screen_വരികൾ[off]])
	ആകുമ്പോൾ (*endcol < ((പൂർണ്ണസംഖ്യ)screen_Columns - 1)
				      && gui.ligatures_map[Screen_വരികൾ[++off]])
	   (*endcol)++;
}
#endif

    സ്ഥിതം ശൂന്യം
gui_set_cursor(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col)
{
    gui.row = row;
    gui.col = col;
}

/*
 * gui_check_pos - check if the cursor is on the screen.
 */
    സ്ഥിതം ശൂന്യം
gui_check_pos(ശൂന്യം)
{
    എങ്കിൽ (gui.row >= screen_Rows)
	gui.row = screen_Rows - 1;
    എങ്കിൽ (gui.col >= screen_Columns)
	gui.col = screen_Columns - 1;
    എങ്കിൽ (gui.cursor_row >= screen_Rows || gui.cursor_col >= screen_Columns)
	gui.cursor_is_valid = false;
}

/*
 * Redraw the cursor if necessary or when forced.
 * Careful: The contents of ScreenLines[] must match what is on the screen,
 * otherwise this goes wrong.  May need to call out_flush() first.
 */
    ശൂന്യം
gui_update_cursor(
    പൂർണ്ണസംഖ്യ		force,		 // when TRUE, update even when not moved
    പൂർണ്ണസംഖ്യ		clear_selection) // clear selection under cursor
{
    പൂർണ്ണസംഖ്യ		cur_width = 0;
    പൂർണ്ണസംഖ്യ		cur_height = 0;
    പൂർണ്ണസംഖ്യ		old_hl_mask;
    cursorentry_T *shape;
    പൂർണ്ണസംഖ്യ		id;
#ifdef FEAT_TERMINAL
    guicolor_T	shape_fg = INVALCOLOR;
    guicolor_T	shape_bg = INVALCOLOR;
#endif
    guicolor_T	cfg, cbg, cc;	// cursor fore-/background color
    പൂർണ്ണസംഖ്യ		cattr;		// cursor attributes
    പൂർണ്ണസംഖ്യ		attr;
    attrentry_T *aep = ശൂന്യ;
#എങ്കിൽ (defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)) && !defined(USE_GTK4)
    bool	lig_ഇടത് = false, lig_വലത് = false;
#endif

    // Don't update the cursor when halfway busy scrolling or the screen size
    // doesn't match 'columns' and 'lines.  ScreenLines[] isn't valid then.
    എങ്കിൽ (!can_update_cursor || screen_Columns != gui.അക്കം_cols
					       || screen_Rows != gui.അക്കം_rows)
	തിരികെ;

    gui_check_pos();

    എങ്കിൽ (gui.cursor_is_valid && !force
		&& gui.row == gui.cursor_row && gui.col == gui.cursor_col)
	തിരികെ;

    gui_undraw_cursor();

    // If a cursor-less sleep is ongoing, leave the cursor invisible
    എങ്കിൽ (cursor_is_sleeping())
	തിരികെ;

    എങ്കിൽ (gui.row < 0)
	തിരികെ;
#ifdef HAVE_നിവേശം_METHOD
    എങ്കിൽ (gui.row != gui.cursor_row || gui.col != gui.cursor_col)
	im_set_position(gui.row, gui.col);
#endif
    gui.cursor_row = gui.row;
    gui.cursor_col = gui.col;

    // Only write to the screen after ScreenLines[] has been initialized
    എങ്കിൽ (!screen_cleared || Screen_വരികൾ == ശൂന്യ)
	തിരികെ;

    // Clear the selection if we are about to write over it
    എങ്കിൽ (clear_selection)
	clip_may_clear_selection(gui.row, gui.row);
    // Check that the cursor is inside the shell (resizing may have made
    // it invalid)
    എങ്കിൽ (gui.row >= screen_Rows || gui.col >= screen_Columns)
	തിരികെ;

    gui.cursor_is_valid = true;

    /*
     * How the cursor is drawn depends on the current mode.
     * When in a terminal window use the shape/color specified there.
     */
#ifdef FEAT_TERMINAL
    എങ്കിൽ (terminal_is_active())
	shape = term_get_cursor_shape(&shape_fg, &shape_bg);
    അല്ലെങ്കിൽ
#endif
	shape = &shape_table[get_shape_idx(FALSE)];
    എങ്കിൽ (State & MODE_LANGMAP)
	id = shape->id_lm;
    അല്ലെങ്കിൽ
	id = shape->id;

    // get the colors and attributes for the cursor.  Default is inverted
    cfg = INVALCOLOR;
    cbg = INVALCOLOR;
    cattr = HL_INVERSE;
    gui_mch_set_blinking(shape->blinkwait,
	    shape->blinkon,
	    shape->blinkoff);
    എങ്കിൽ (shape->blinkwait == 0 || shape->blinkon == 0
	    || shape->blinkoff == 0)
	gui_mch_stop_blink(FALSE);
#ifdef FEAT_TERMINAL
    എങ്കിൽ (shape_bg != INVALCOLOR)
    {
	cattr = 0;
	cfg = shape_fg;
	cbg = shape_bg;
    }
    അല്ലെങ്കിൽ
#endif
	എങ്കിൽ (id > 0)
	{
	    cattr = syn_id2colors(id, &cfg, &cbg);
#എങ്കിൽ defined(HAVE_നിവേശം_METHOD)
	    {
		സ്ഥിതം പൂർണ്ണസംഖ്യ iid;
		guicolor_T fg, bg;

		എങ്കിൽ (
# എങ്കിൽ defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
			preedit_get_status()
# അല്ലെങ്കിൽ
			im_get_status()
# endif
		   )
		{
		    iid = syn_name2id((char_u *)"CursorIM");
		    എങ്കിൽ (iid > 0)
		    {
			syn_id2colors(iid, &fg, &bg);
			എങ്കിൽ (bg != INVALCOLOR)
			    cbg = bg;
			എങ്കിൽ (fg != INVALCOLOR)
			    cfg = fg;
		    }
		}
	    }
#endif
	}

    /*
     * Get the attributes for the character under the cursor.
     * When no cursor color was given, use the character color.
     */
    attr = ScreenAttrs[വരി_Offset[gui.row] + gui.col];
    എങ്കിൽ (attr > HL_ALL)
	aep = syn_gui_attr2entry(attr);
    എങ്കിൽ (aep != ശൂന്യ)
    {
	attr = aep->ae_attr;
	എങ്കിൽ (cfg == INVALCOLOR)
	    cfg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.bg_color
		    : aep->ae_u.gui.fg_color);
	എങ്കിൽ (cbg == INVALCOLOR)
	    cbg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.fg_color
		    : aep->ae_u.gui.bg_color);
    }
    എങ്കിൽ (cfg == INVALCOLOR)
	cfg = (attr & HL_INVERSE) ? gui.back_pixel : gui.norm_pixel;
    എങ്കിൽ (cbg == INVALCOLOR)
	cbg = (attr & HL_INVERSE) ? gui.norm_pixel : gui.back_pixel;

#ifdef FEAT_XIM
    എങ്കിൽ (aep != ശൂന്യ)
    {
	xim_bg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.fg_color
		: aep->ae_u.gui.bg_color);
	xim_fg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.bg_color
		: aep->ae_u.gui.fg_color);
	എങ്കിൽ (xim_bg_color == INVALCOLOR)
	    xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
		: gui.back_pixel;
	എങ്കിൽ (xim_fg_color == INVALCOLOR)
	    xim_fg_color = (attr & HL_INVERSE) ? gui.back_pixel
		: gui.norm_pixel;
    }
    അല്ലെങ്കിൽ
    {
	xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
	    : gui.back_pixel;
	xim_fg_color = (attr & HL_INVERSE) ? gui.back_pixel
	    : gui.norm_pixel;
    }
#endif

    attr &= ~HL_INVERSE;
    എങ്കിൽ (cattr & HL_INVERSE)
    {
	cc = cbg;
	cbg = cfg;
	cfg = cc;
    }
    cattr &= ~HL_INVERSE;

    /*
     * When we don't have window focus, draw a hollow cursor.
     */
    എങ്കിൽ (!gui.in_focus)
    {
#ifdef USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, -1, -1);
#അല്ലെങ്കിൽ
	gui_mch_draw_hollow_cursor(cbg);
#endif
	തിരികെ;
    }

#ifdef USE_GTK4
    // Make sure that character underneath is drawn again in case it is part of
    // a ligature.
    gui_redraw_block(gui.row, gui.col, gui.row, gui.col, GUI_MON_NOCLEAR);
    // gui_redraw_block() will invalidate the cursor
    gui.cursor_is_valid = true;
#endif
#എങ്കിൽ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // If cursor is in the middle of a ligature, then split the ligature at
    // the cursor boundaries by redrawing those cells again.
    വേണ്ടി (പൂർണ്ണസംഖ്യ c = gui.col - 1; c >= 0; c--)
	എങ്കിൽ (!gui.ligatures_map[Screen_വരികൾ[വരി_Offset[gui.row] + c]])
	{
	    എങ്കിൽ (c < gui.col - 1)
	    {
		gui_redraw_block(gui.row, c + 1, gui.row, gui.col - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_ഇടത് = true;
# endif
	    }
	    നിർത്തുക;
	}

    വേണ്ടി (പൂർണ്ണസംഖ്യ c = gui.col + 1; c < screen_Columns - 1; c++)
	എങ്കിൽ (!gui.ligatures_map[Screen_വരികൾ[വരി_Offset[gui.row] + c]])
	{
	    എങ്കിൽ (c > gui.col + 1)
	    {
		gui_redraw_block(gui.row, gui.col + 1, gui.row, c - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_വലത് = true;
# endif
	    }
	    നിർത്തുക;
	}

# ifndef USE_GTK4
    എങ്കിൽ ((lig_ഇടത് || lig_വലത്) && shape->shape != SHAPE_BLOCK)
    {
	// Because the cursor is not drawn with gui_screenchar(), must blit the
	// cell again (with its background), so that old ligature does not
	// remain on screen. Not needed for GtkSnapshot, because char beneath is
	// always rerendered (see above ifdef).
	പൂർണ്ണസംഖ്യ old = gui.col;
	gui.highlight_mask = ScreenAttrs[വരി_Offset[gui.row] + gui.col];
	(ശൂന്യം)gui_screenchar(വരി_Offset[gui.row] + gui.col,
		GUI_MON_NOCLEAR, (guicolor_T)0, (guicolor_T)0, 0);
	gui.col = old;
    }
# endif
    // gui_redraw_block()/gui_screenchar() may invalidate the cursor, make sure
    // to validate it again.
    gui.cursor_is_valid = true;
#endif

    old_hl_mask = gui.highlight_mask;
    എങ്കിൽ (shape->shape == SHAPE_BLOCK)
    {
#ifdef USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, 0, 0);
#അല്ലെങ്കിൽ
	/*
	 * Draw the text character with the cursor colors.	Use the
	 * character attributes plus the cursor attributes.
	 */
	gui.highlight_mask = (cattr | attr);
	(ശൂന്യം)gui_screenchar(വരി_Offset[gui.row] + gui.col,
		GUI_MON_IS_CURSOR | GUI_MON_NOCLEAR, cfg, cbg, 0);
#endif
    }
    അല്ലെങ്കിൽ
    {
#എങ്കിൽ defined(FEAT_RIGHTLEFT)
	പൂർണ്ണസംഖ്യ	    col_off = FALSE;
#endif
	/*
	 * First draw the partial cursor, then overwrite with the text
	 * character, using a transparent background.
	 */
	എങ്കിൽ (shape->shape == SHAPE_VER)
	{
	    cur_height = gui.char_height;
	    cur_width = (gui.char_width * shape->percentage + 99) / 100;
	}
	അല്ലെങ്കിൽ
	{
	    cur_height = (gui.char_height * shape->percentage + 99) / 100;
	    cur_width = gui.char_width;
	}
	എങ്കിൽ (has_mbyte && (*mb_off2cells)(വരി_Offset[gui.row] + gui.col,
		    വരി_Offset[gui.row] + screen_Columns) > 1)
	{
	    // Double wide character.
	    എങ്കിൽ (shape->shape != SHAPE_VER)
		cur_width += gui.char_width;
#ifdef FEAT_RIGHTLEFT
	    എങ്കിൽ (CURSOR_BAR_വലത്)
	    {
		// gui.col points to the left half of the character but
		// the vertical line needs to be on the right half.
		// A double-wide horizontal line is also drawn from the
		// right half in gui_mch_draw_part_cursor().
		col_off = TRUE;
		++gui.col;
	    }
#endif
	}
#ifdef USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, cur_width, cur_height);
#അല്ലെങ്കിൽ
	gui_mch_draw_part_cursor(cur_width, cur_height, cbg);
#endif
#എങ്കിൽ defined(FEAT_RIGHTLEFT)
	എങ്കിൽ (col_off)
	    --gui.col;
#endif

	// Doesn't seem to work for MSWindows. We call gui_redraw_block() above
	// for GtkSnapshot.
#എങ്കിൽ !defined(FEAT_GUI_MSWIN) && !defined(USE_GTK4)
	gui.highlight_mask = ScreenAttrs[വരി_Offset[gui.row] + gui.col];
	(ശൂന്യം)gui_screenchar(വരി_Offset[gui.row] + gui.col,
		GUI_MON_TRS_CURSOR | GUI_MON_NOCLEAR,
		(guicolor_T)0, (guicolor_T)0, 0);
#endif
    }
    gui.highlight_mask = old_hl_mask;
}

#എങ്കിൽ defined(FEAT_MENU)
    സ്ഥിതം ശൂന്യം
gui_position_menu(ശൂന്യം)
{
# എങ്കിൽ !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
    എങ്കിൽ (gui.menu_is_active && gui.in_use)
	gui_mch_set_menu_pos(0, 0, gui.menu_width, gui.menu_height);
# endif
}
#endif

/*
 * Position the various GUI components (text area, menu).  The vertical
 * scrollbars are NOT handled here.  See gui_update_scrollbars().
 */
    സ്ഥിതം ശൂന്യം
gui_position_components(പൂർണ്ണസംഖ്യ total_width UNUSED)
{
    പൂർണ്ണസംഖ്യ	    text_area_x;
    പൂർണ്ണസംഖ്യ	    text_area_y;
    പൂർണ്ണസംഖ്യ	    text_area_width;
    പൂർണ്ണസംഖ്യ	    text_area_height;

    // avoid that moving components around generates events
    ++hold_gui_events;

    text_area_x = 0;
    എങ്കിൽ (gui.which_scrollbars[SBAR_ഇടത്])
	text_area_x += gui.scrollbar_width;

    text_area_y = 0;
#എങ്കിൽ defined(FEAT_MENU) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    gui.menu_width = total_width;
    എങ്കിൽ (gui.menu_is_active)
	text_area_y += gui.menu_height;
#endif

#എങ്കിൽ defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
    എങ്കിൽ (gui_has_tabline())
	text_area_y += gui.tabline_height;
#endif

#എങ്കിൽ defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    എങ്കിൽ (vim_strchr(p_go, GO_TOOLBAR) != ശൂന്യ)
    {
# എങ്കിൽ defined(FEAT_GUI_HAIKU)
	gui_mch_set_toolbar_pos(0, text_area_y,
				gui.menu_width, gui.toolbar_height);
# endif
	text_area_y += gui.toolbar_height;
    }
#endif

#എങ്കിൽ defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_HAIKU)
    gui_mch_set_tabline_pos(0, text_area_y,
    gui.menu_width, gui.tabline_height);
    എങ്കിൽ (gui_has_tabline())
	text_area_y += gui.tabline_height;
#endif

    text_area_width = gui.അക്കം_cols * gui.char_width + gui.border_offset * 2;
    text_area_height = gui.അക്കം_rows * gui.char_height + gui.border_offset * 2;

    gui_mch_set_text_area_pos(text_area_x,
			      text_area_y,
			      text_area_width,
			      text_area_height
#എങ്കിൽ defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
				  + xim_get_status_area_height()
#endif
			      );
#ifdef FEAT_MENU
    gui_position_menu();
#endif
    എങ്കിൽ (gui.which_scrollbars[SBAR_BOTTOM])
	gui_mch_set_scrollbar_pos(&gui.bottom_sbar,
				  text_area_x,
				  text_area_y + text_area_height
					+ gui_mch_get_scrollbar_ypadding(),
				  text_area_width,
				  gui.scrollbar_height);
    gui.ഇടത്_sbar_x = 0;
    gui.വലത്_sbar_x = text_area_x + text_area_width
					+ gui_mch_get_scrollbar_xpadding();

    --hold_gui_events;
}

/*
 * Get the width of the widgets and decorations to the side of the text area.
 */
    പൂർണ്ണസംഖ്യ
gui_get_base_width(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    base_width;

    base_width = 2 * gui.border_offset;
    എങ്കിൽ (gui.which_scrollbars[SBAR_ഇടത്])
	base_width += gui.scrollbar_width;
    എങ്കിൽ (gui.which_scrollbars[SBAR_വലത്])
	base_width += gui.scrollbar_width;
    തിരികെ base_width;
}

/*
 * Get the height of the widgets and decorations above and below the text area.
 */
    പൂർണ്ണസംഖ്യ
gui_get_base_height(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    base_height;

    base_height = 2 * gui.border_offset;
    എങ്കിൽ (gui.which_scrollbars[SBAR_BOTTOM])
	base_height += gui.scrollbar_height;
#ifdef FEAT_GUI_GTK
    // We can't take the sizes properly into account until anything is
    // realized.  Therefore we recalculate all the values here just before
    // setting the size. (--mdcki)
#അല്ലെങ്കിൽ
# ifdef FEAT_MENU
    എങ്കിൽ (gui.menu_is_active)
	base_height += gui.menu_height;
# endif
# ifdef FEAT_TOOLBAR
    എങ്കിൽ (vim_strchr(p_go, GO_TOOLBAR) != ശൂന്യ)
	base_height += gui.toolbar_height;
# endif
# എങ്കിൽ defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    എങ്കിൽ (gui_has_tabline())
	base_height += gui.tabline_height;
# endif
# എങ്കിൽ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    base_height += gui_mch_text_area_extra_height();
# endif
#endif
    തിരികെ base_height;
}

/*
 * Should be called after the GUI shell has been resized.  Its arguments are
 * the new width and height of the shell in pixels.
 */
    ശൂന്യം
gui_resize_shell(പൂർണ്ണസംഖ്യ pixel_width, പൂർണ്ണസംഖ്യ pixel_height)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	busy = FALSE;

    എങ്കിൽ (!gui.shell_created)	    // ignore when still initializing
	തിരികെ;

    /*
     * Can't resize the screen while it is being redrawn.  Remember the new
     * size and handle it later.
     */
    എങ്കിൽ (updating_screen || busy)
    {
	new_pixel_width = pixel_width;
	new_pixel_height = pixel_height;
	തിരികെ;
    }

again:
    new_pixel_width = 0;
    new_pixel_height = 0;
    busy = TRUE;

#ifdef FEAT_GUI_HAIKU
    vim_lock_screen();
#endif

    // Flush pending output before redrawing
    out_flush();
#എങ്കിൽ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk_init_decor_height();
#endif

    gui.അക്കം_cols = (pixel_width - gui_get_base_width()) / gui.char_width;
    gui.അക്കം_rows = (pixel_height - gui_get_base_height()) / gui.char_height;

#ifdef USE_GTK4
    gui_gtk4_update_size();
#endif

    gui_position_components(pixel_width);
    gui_reset_scroll_region();

#എങ്കിൽ defined(FEAT_GUI_GTK) && defined(USE_GTK4) && !defined(USE_GTK4)
    // We do not resize the draw area via the "resize" signal. This is because
    // when the window is resized, the form widget is the one that is resized,
    // so let that call gui_resize_shell() which will allocate the surface and
    // allocate the drawing area size/position.
    gui_gtk4_resize(pixel_width, pixel_height);
#endif

    /*
     * At the "more" and ":confirm" prompt there is no redraw, put the cursor
     * at the last line here (why does it have to be one row too low?).
     */
    എങ്കിൽ (State == MODE_ASKMORE || State == MODE_CONFIRM)
	gui.row = gui.അക്കം_rows;

    // Only comparing Rows and Columns may be sufficient, but let's stay on
    // the safe side.
    എങ്കിൽ (gui.അക്കം_rows != screen_Rows || gui.അക്കം_cols != screen_Columns
	    || gui.അക്കം_rows != Rows || gui.അക്കം_cols != Columns || gui.force_redraw)
    {
	shell_resized();
	gui.force_redraw = 0;
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    gui_update_scrollbars(TRUE);
    gui_update_cursor(FALSE, TRUE);
#എങ്കിൽ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(pixel_width, pixel_height);
#endif
#എങ്കിൽ defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
    xim_set_status_area();
#endif

    busy = FALSE;

    // We may have been called again while redrawing the screen.
    // Need to do it all again with the latest size then.  But only if the size
    // actually changed.
    എങ്കിൽ (new_pixel_height)
    {
	എങ്കിൽ (pixel_width == new_pixel_width && pixel_height == new_pixel_height)
	{
	    new_pixel_width = 0;
	    new_pixel_height = 0;
	}
	അല്ലെങ്കിൽ
	{
	    pixel_width = new_pixel_width;
	    pixel_height = new_pixel_height;
	    പോകുക again;
	}
    }
}

/*
 * Check if gui_resize_shell() must be called.
 */
    ശൂന്യം
gui_may_resize_shell(ശൂന്യം)
{
    എങ്കിൽ (new_pixel_height)
    {
	// careful: gui_resize_shell() may postpone the resize again if we
	// were called indirectly by it
	gui_resize_shell(new_pixel_width, new_pixel_height);
    }
}

    പൂർണ്ണസംഖ്യ
gui_get_shellsize(ശൂന്യം)
{
    Rows = gui.അക്കം_rows;
    Columns = gui.അക്കം_cols;
    തിരികെ OK;
}

/*
 * Set the size of the Vim shell according to Rows and Columns.
 * If "fit_to_display" is TRUE then the size may be reduced to fit the window
 * on the screen.
 * When "mustset" is TRUE the size was set by the user. When FALSE a UI
 * component was added or removed (e.g., a scrollbar).
 */
    ശൂന്യം
gui_set_shellsize(
    പൂർണ്ണസംഖ്യ		mustset UNUSED,
    പൂർണ്ണസംഖ്യ		fit_to_display,
    പൂർണ്ണസംഖ്യ		direction)		// RESIZE_HOR, RESIZE_VER
{
    പൂർണ്ണസംഖ്യ		base_width;
    പൂർണ്ണസംഖ്യ		base_height;
    പൂർണ്ണസംഖ്യ		width;
    പൂർണ്ണസംഖ്യ		height;
    പൂർണ്ണസംഖ്യ		min_width;
    പൂർണ്ണസംഖ്യ		min_height;
    പൂർണ്ണസംഖ്യ		screen_w;
    പൂർണ്ണസംഖ്യ		screen_h;
#ifdef FEAT_GUI_GTK
    പൂർണ്ണസംഖ്യ		un_maximize = mustset;
    പൂർണ്ണസംഖ്യ		did_adjust = 0;
#endif
    പൂർണ്ണസംഖ്യ		x = -1, y = -1;

    എങ്കിൽ (!gui.shell_created)
	തിരികെ;

#എങ്കിൽ defined(MSWIN) || defined(FEAT_GUI_GTK)
    // If not setting to a user specified size and maximized, calculate the
    // number of characters that fit in the maximized window.
    // FIXME: gui_mch_newfont() is called here even when the font hasn't
    // changed at all.  For example, ":set guioptions=k" triggers this path
    // via gui_init_which_components() -> gui_set_shellsize(FALSE, ...).
    // The intent is to keep the window size and recalculate Rows/Columns,
    // which has nothing to do with fonts.  This should be a separate
    // function with a more descriptive name.
    എങ്കിൽ (!mustset && (vim_strchr(p_go, GO_KEEPWINSIZE) != ശൂന്യ
						       || gui_mch_maximized()))
    {
	gui_mch_newfont();
	തിരികെ;
    }
#endif

    base_width = gui_get_base_width();
    base_height = gui_get_base_height();
    എങ്കിൽ (fit_to_display)
	// Remember the original window position.
	(ശൂന്യം)gui_mch_get_winpos(&x, &y);

    width = Columns * gui.char_width + base_width;
    height = Rows * gui.char_height + base_height;

    എങ്കിൽ (fit_to_display)
    {
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	എങ്കിൽ ((direction & RESIZE_HOR) && width > screen_w)
	{
	    Columns = (screen_w - base_width) / gui.char_width;
	    എങ്കിൽ (Columns < MIN_COLUMNS)
		Columns = MIN_COLUMNS;
	    width = Columns * gui.char_width + base_width;
#ifdef FEAT_GUI_GTK
	    ++did_adjust;
#endif
	}
	എങ്കിൽ ((direction & RESIZE_VERT) && height > screen_h)
	{
	    Rows = (screen_h - base_height) / gui.char_height;
	    check_shellsize();
	    height = Rows * gui.char_height + base_height;
#ifdef FEAT_GUI_GTK
	    ++did_adjust;
#endif
	}
#ifdef FEAT_GUI_GTK
	എങ്കിൽ (did_adjust == 2 || (width + gui.char_width >= screen_w
				     && height + gui.char_height >= screen_h))
	    // don't unmaximize if at maximum size
	    un_maximize = FALSE;
#endif
    }
    limit_screen_size();
    gui.അക്കം_cols = Columns;
    gui.അക്കം_rows = Rows;
#ifdef USE_GTK4
    // Keep the drawing area in sync with the size Vim is going to draw.
    gui_gtk4_update_size();
#endif

    min_width = base_width + MIN_COLUMNS * gui.char_width;
    min_height = base_height + MIN_വരികൾ * gui.char_height;
    min_height += tabline_height() * gui.char_height;

#ifdef FEAT_GUI_GTK
    എങ്കിൽ (un_maximize)
    {
	// If the window size is smaller than the screen unmaximize the
	// window, otherwise resizing won't work.
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	എങ്കിൽ ((width + gui.char_width < screen_w
				   || height + gui.char_height * 2 < screen_h)
		&& gui_mch_maximized())
	    gui_mch_unmaximize();
    }
#endif

    gui_mch_set_shellsize(width, height, min_width, min_height,
					  base_width, base_height, direction);

    എങ്കിൽ (fit_to_display && x >= 0 && y >= 0)
    {
	// Some window managers put the Vim window left of/above the screen.
	// Only change the position if it wasn't already negative before
	// (happens on MS-Windows with a secondary monitor).
	gui_mch_update();
	എങ്കിൽ (gui_mch_get_winpos(&x, &y) == OK && (x < 0 || y < 0))
	    gui_mch_set_winpos(x < 0 ? 0 : x, y < 0 ? 0 : y);
    }

    gui_position_components(width);
    gui_update_scrollbars(TRUE);
    gui_reset_scroll_region();

#എങ്കിൽ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(width, height);
#endif
}

/*
 * Called when Rows and/or Columns has changed.
 */
    ശൂന്യം
gui_new_shellsize(ശൂന്യം)
{
    gui_reset_scroll_region();
}

/*
 * Make scroll region cover whole screen.
 */
    സ്ഥിതം ശൂന്യം
gui_reset_scroll_region(ശൂന്യം)
{
    gui.scroll_region_മുകൾ = 0;
    gui.scroll_region_bot = gui.അക്കം_rows - 1;
    gui.scroll_region_ഇടത് = 0;
    gui.scroll_region_വലത് = gui.അക്കം_cols - 1;
}

    സ്ഥിതം ശൂന്യം
gui_start_highlight(പൂർണ്ണസംഖ്യ mask)
{
    എങ്കിൽ (mask > HL_ALL)		    // highlight code
	gui.highlight_mask = mask;
    അല്ലെങ്കിൽ			    // mask
	gui.highlight_mask |= mask;
}

    ശൂന്യം
gui_stop_highlight(പൂർണ്ണസംഖ്യ mask)
{
    എങ്കിൽ (mask > HL_ALL)		    // highlight code
	gui.highlight_mask = HL_NORMAL;
    അല്ലെങ്കിൽ			    // mask
	gui.highlight_mask &= ~mask;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    ശൂന്യം
gui_clear_block(
    പൂർണ്ണസംഖ്യ	    row1,
    പൂർണ്ണസംഖ്യ	    col1,
    പൂർണ്ണസംഖ്യ	    row2,
    പൂർണ്ണസംഖ്യ	    col2)
{
    // Clear the selection if we are about to write over it
    clip_may_clear_selection(row1, row2);

    gui_mch_clear_block(row1, col1, row2, col2);

    // Invalidate cursor if it was in this block
    എങ്കിൽ (       gui.cursor_row >= row1 && gui.cursor_row <= row2
	    && gui.cursor_col >= col1 && gui.cursor_col <= col2)
	gui.cursor_is_valid = false;
}

/*
 * Write code to update the cursor later.  This avoids the need to flush the
 * output buffer before calling gui_update_cursor().
 */
    ശൂന്യം
gui_update_cursor_later(ശൂന്യം)
{
    OUT_STR("\033|s");
}

    ശൂന്യം
gui_write(
    char_u	*s,
    പൂർണ്ണസംഖ്യ		len)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		arg1 = 0, arg2 = 0;
    പൂർണ്ണസംഖ്യ		force_cursor = FALSE;	// force cursor update
    പൂർണ്ണസംഖ്യ		force_scrollbar = FALSE;
    സ്ഥിതം win_T	*old_curwin = ശൂന്യ;

// #define DEBUG_GUI_WRITE
#ifdef DEBUG_GUI_WRITE
    {
	പൂർണ്ണസംഖ്യ i;
	char_u *str;

	printf("gui_write(%d):\n    ", len);
	വേണ്ടി (i = 0; i < len; i++)
	    എങ്കിൽ (s[i] == ESC)
	    {
		എങ്കിൽ (i != 0)
		    printf("\n    ");
		printf("<ESC>");
	    }
	    അല്ലെങ്കിൽ
	    {
		str = transchar_byte(s[i]);
		എങ്കിൽ (str[0] && str[1])
		    printf("<%s>", (അക്ഷരം *)str);
		അല്ലെങ്കിൽ
		    printf("%s", (അക്ഷരം *)str);
	    }
	printf("\n");
    }
#endif
    ആകുമ്പോൾ (len)
    {
	എങ്കിൽ (s[0] == ESC && s[1] == '|')
	{
	    p = s + 2;
	    എങ്കിൽ (VIM_ISDIGIT(*p) || (*p == '-' && VIM_ISDIGIT(*(p + 1))))
	    {
		arg1 = getdigits(&p);
		എങ്കിൽ (p > s + len)
		    നിർത്തുക;
		എങ്കിൽ (*p == ';')
		{
		    ++p;
		    arg2 = getdigits(&p);
		    എങ്കിൽ (p > s + len)
			നിർത്തുക;
		}
	    }
	    തിരഞ്ഞെടുപ്പ് (*p)
	    {
		അവസ്ഥ 'C':	// Clear screen
		    clip_scroll_selection(9999);
		    gui_mch_clear_all();
		    gui.cursor_is_valid = false;
		    force_scrollbar = TRUE;
		    നിർത്തുക;
		അവസ്ഥ 'M':	// Move cursor
		    gui_set_cursor(arg1, arg2);
		    നിർത്തുക;
		അവസ്ഥ 's':	// force cursor (shape) update
		    force_cursor = TRUE;
		    നിർത്തുക;
		അവസ്ഥ 'R':	// Set scroll region
		    എങ്കിൽ (arg1 < arg2)
		    {
			gui.scroll_region_മുകൾ = arg1;
			gui.scroll_region_bot = arg2;
		    }
		    അല്ലെങ്കിൽ
		    {
			gui.scroll_region_മുകൾ = arg2;
			gui.scroll_region_bot = arg1;
		    }
		    നിർത്തുക;
		അവസ്ഥ 'V':	// Set vertical scroll region
		    എങ്കിൽ (arg1 < arg2)
		    {
			gui.scroll_region_ഇടത് = arg1;
			gui.scroll_region_വലത് = arg2;
		    }
		    അല്ലെങ്കിൽ
		    {
			gui.scroll_region_ഇടത് = arg2;
			gui.scroll_region_വലത് = arg1;
		    }
		    നിർത്തുക;
		അവസ്ഥ 'd':	// Delete line
		    gui_delete_വരികൾ(gui.row, 1);
		    നിർത്തുക;
		അവസ്ഥ 'D':	// Delete lines
		    gui_delete_വരികൾ(gui.row, arg1);
		    നിർത്തുക;
		അവസ്ഥ 'i':	// Insert line
		    gui_തിരുകുക_വരികൾ(gui.row, 1);
		    നിർത്തുക;
		അവസ്ഥ 'I':	// Insert lines
		    gui_തിരുകുക_വരികൾ(gui.row, arg1);
		    നിർത്തുക;
		അവസ്ഥ '$':	// Clear to end-of-line
		    gui_clear_block(gui.row, gui.col, gui.row,
							    (പൂർണ്ണസംഖ്യ)Columns - 1);
		    നിർത്തുക;
		അവസ്ഥ 'h':	// Turn on highlighting
		    gui_start_highlight(arg1);
		    നിർത്തുക;
		അവസ്ഥ 'H':	// Turn off highlighting
		    gui_stop_highlight(arg1);
		    നിർത്തുക;
		അവസ്ഥ 'f':	// flash the window (visual bell)
		    gui_mch_flash(arg1 == 0 ? 20 : arg1);
		    നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		    p = s + 1;	// Skip the ESC
		    നിർത്തുക;
	    }
	    len -= (പൂർണ്ണസംഖ്യ)(++p - s);
	    s = p;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (s[0] < 0x20		// Ctrl character
#ifdef FEAT_SIGN_ICONS
		&& s[0] != SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
		&& s[0] != MULTISIGN_BYTE
# endif
#endif
		)
	{
	    എങ്കിൽ (s[0] == '\n')		// NL
	    {
		gui.col = 0;
		എങ്കിൽ (gui.row < gui.scroll_region_bot)
		    gui.row++;
		അല്ലെങ്കിൽ
		    gui_delete_വരികൾ(gui.scroll_region_മുകൾ, 1);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (s[0] == '\r')	// CR
	    {
		gui.col = 0;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (s[0] == '\b')	// Backspace
	    {
		എങ്കിൽ (gui.col)
		    --gui.col;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (s[0] == Ctrl_L)	// cursor-right
	    {
		++gui.col;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (s[0] == Ctrl_G)	// Beep
	    {
		gui_mch_beep();
	    }
	    // Other Ctrl character: shouldn't happen!

	    --len;	// Skip this char
	    ++s;
	}
	അല്ലെങ്കിൽ
	{
	    p = s;
	    ആകുമ്പോൾ (len > 0 && (
			*p >= 0x20
#ifdef FEAT_SIGN_ICONS
			|| *p == SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
			|| *p == MULTISIGN_BYTE
# endif
#endif
			))
	    {
		len--;
		p++;
	    }
	    gui_outstr(s, (പൂർണ്ണസംഖ്യ)(p - s));
	    s = p;
	}
    }

    // Postponed update of the cursor (won't work if "can_update_cursor" isn't
    // set).
    എങ്കിൽ (force_cursor)
	gui_update_cursor(TRUE, TRUE);

    // When switching to another window the dragging must have stopped.
    // Required for GTK, dragged_sb isn't reset.
    എങ്കിൽ (old_curwin != curwin)
	gui.dragged_sb = SBAR_NONE;

    // Update the scrollbars after clearing the screen or when switched
    // to another window.
    // Update the horizontal scrollbar always, it's difficult to check all
    // situations where it might change.
    എങ്കിൽ (force_scrollbar || old_curwin != curwin)
	gui_update_scrollbars(force_scrollbar);
    അല്ലെങ്കിൽ
	gui_update_horiz_scrollbar(FALSE);
    old_curwin = curwin;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#എങ്കിൽ defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#endif

    gui_may_flush();		    // In case vim decides to take a nap
}

/*
 * When ScreenLines[] is invalid, updating the cursor should not be done, it
 * produces wrong results.  Call gui_dont_update_cursor() before that code and
 * gui_can_update_cursor() afterwards.
 */
    ശൂന്യം
gui_dont_update_cursor(പൂർണ്ണസംഖ്യ undraw)
{
    എങ്കിൽ (!gui.in_use)
	തിരികെ;

    // Undraw the cursor now, we probably can't do it after the change.
    എങ്കിൽ (undraw)
	gui_undraw_cursor();
    can_update_cursor = FALSE;
}

    ശൂന്യം
gui_can_update_cursor(ശൂന്യം)
{
    can_update_cursor = TRUE;
    // No need to update the cursor right now, there is always more output
    // after scrolling.
}

/*
 * Disable issuing gui_mch_flush().
 */
    ശൂന്യം
gui_disable_flush(ശൂന്യം)
{
    ++disable_flush;
}

/*
 * Enable issuing gui_mch_flush().
 */
    ശൂന്യം
gui_enable_flush(ശൂന്യം)
{
    --disable_flush;
}

/*
 * Issue gui_mch_flush() if it is not disabled.
 */
    ശൂന്യം
gui_may_flush(ശൂന്യം)
{
    എങ്കിൽ (disable_flush == 0)
	gui_mch_flush();
}

    സ്ഥിതം ശൂന്യം
gui_outstr(char_u *s, പൂർണ്ണസംഖ്യ len)
{
    പൂർണ്ണസംഖ്യ	    this_len;
    പൂർണ്ണസംഖ്യ	    cells;

    എങ്കിൽ (len == 0)
	തിരികെ;

    എങ്കിൽ (len < 0)
	len = (പൂർണ്ണസംഖ്യ)STRLEN(s);

    ആകുമ്പോൾ (len > 0)
    {
	എങ്കിൽ (has_mbyte)
	{
	    // Find out how many chars fit in the current line.
	    cells = 0;
	    വേണ്ടി (this_len = 0; this_len < len; )
	    {
		cells += (*mb_ptr2cells)(s + this_len);
		എങ്കിൽ (gui.col + cells > Columns)
		    നിർത്തുക;
		this_len += (*mb_ptr2len)(s + this_len);
	    }
	    എങ്കിൽ (this_len > len)
		this_len = len;	    // don't include following composing char
	}
	അല്ലെങ്കിൽ എങ്കിൽ (gui.col + len > Columns)
	    this_len = Columns - gui.col;
	അല്ലെങ്കിൽ
	    this_len = len;

	(ശൂന്യം)gui_outstr_nowrap(s, this_len,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	s += this_len;
	len -= this_len;
	// fill up for a double-width char that doesn't fit.
	എങ്കിൽ (len > 0 && gui.col < Columns)
	    (ശൂന്യം)gui_outstr_nowrap((char_u *)" ", 1,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	// The cursor may wrap to the next line.
	എങ്കിൽ (gui.col >= Columns)
	{
	    gui.col = 0;
	    gui.row++;
	}
    }
}

#ifndef USE_GTK4
/*
 * Output one character (may be one or two display cells).
 * Caller must check for valid "off".
 * Returns FAIL or OK, just like gui_outstr_nowrap().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_screenchar(
    പൂർണ്ണസംഖ്യ		off,	    // Offset from start of screen
    പൂർണ്ണസംഖ്യ		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    പൂർണ്ണസംഖ്യ		back)	    // backup this many chars when using bold trick
{
    char_u	buf[MB_MAXBYTES + 1];

    // Don't draw right half of a double-width UTF-8 char. "cannot happen"
    എങ്കിൽ (enc_utf8 && Screen_വരികൾ[off] == 0)
	തിരികെ OK;

    എങ്കിൽ (enc_utf8 && Screen_വരികൾ_UC[off] != 0)
	// Draw UTF-8 multi-byte character.
	തിരികെ gui_outstr_nowrap(buf, utfc_char2bytes(off, buf),
							 flags, fg, bg, back);

    എങ്കിൽ (enc_dbcs == DBCS_JPNU && Screen_വരികൾ[off] == 0x8e)
    {
	buf[0] = Screen_വരികൾ[off];
	buf[1] = ScreenLines2[off];
	തിരികെ gui_outstr_nowrap(buf, 2, flags, fg, bg, back);
    }

    // Draw non-multi-byte character or DBCS character.
    തിരികെ gui_outstr_nowrap(Screen_വരികൾ + off,
	    enc_dbcs ? (*mb_ptr2len)(Screen_വരികൾ + off) : 1,
							 flags, fg, bg, back);
}
#endif

#ifdef FEAT_GUI_GTK
/*
 * Output the string at the given screen position.  This is used in place
 * of gui_screenchar() where possible because Pango needs as much context
 * as possible to work nicely.  It's a lot faster as well.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_screenstr(
    പൂർണ്ണസംഖ്യ		off,	    // Offset from start of screen
    പൂർണ്ണസംഖ്യ		len,	    // string length in screen cells
    പൂർണ്ണസംഖ്യ		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    പൂർണ്ണസംഖ്യ		back)	    // backup this many chars when using bold trick
{
    char_u  *buf;
    പൂർണ്ണസംഖ്യ	    outlen = 0;
    പൂർണ്ണസംഖ്യ	    i;
    പൂർണ്ണസംഖ്യ	    retval;

    എങ്കിൽ (len <= 0) // "cannot happen"?
	തിരികെ OK;

    എങ്കിൽ (enc_utf8)
    {
	buf = alloc(len * MB_MAXBYTES + 1);
	എങ്കിൽ (buf == ശൂന്യ)
	    തിരികെ OK; // not much we could do here...

	വേണ്ടി (i = off; i < off + len; ++i)
	{
	    എങ്കിൽ (Screen_വരികൾ[i] == 0)
		തുടരുക; // skip second half of double-width char

	    എങ്കിൽ (Screen_വരികൾ_UC[i] == 0)
		buf[outlen++] = Screen_വരികൾ[i];
	    അല്ലെങ്കിൽ
		outlen += utfc_char2bytes(i, buf + outlen);
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, back);
	vim_free(buf);

	തിരികെ retval;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (enc_dbcs == DBCS_JPNU)
    {
	buf = alloc(len * 2 + 1);
	എങ്കിൽ (buf == ശൂന്യ)
	    തിരികെ OK; // not much we could do here...

	വേണ്ടി (i = off; i < off + len; ++i)
	{
	    buf[outlen++] = Screen_വരികൾ[i];

	    // handle double-byte single-width char
	    എങ്കിൽ (Screen_വരികൾ[i] == 0x8e)
		buf[outlen++] = ScreenLines2[i];
	    അല്ലെങ്കിൽ എങ്കിൽ (MB_BYTE2LEN(Screen_വരികൾ[i]) == 2)
		buf[outlen++] = Screen_വരികൾ[++i];
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, back);
	vim_free(buf);

	തിരികെ retval;
    }
    അല്ലെങ്കിൽ
    {
	തിരികെ gui_outstr_nowrap(&Screen_വരികൾ[off], len,
				 flags, fg, bg, back);
    }
}
#endif // FEAT_GUI_GTK

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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_outstr_nowrap(
    char_u	*s,
    പൂർണ്ണസംഖ്യ		len,
    പൂർണ്ണസംഖ്യ		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    പൂർണ്ണസംഖ്യ		back)	    // backup this many chars when using bold trick
{
    long_u	highlight_mask;
    long_u	hl_mask_todo;
    guicolor_T	fg_color;
    guicolor_T	bg_color;
    guicolor_T	sp_color;
#എങ്കിൽ !defined(FEAT_GUI_GTK)
    GuiFont	font = NOFONT;
    GuiFont	wide_font = NOFONT;
# ifdef FEAT_XFONTSET
    GuiFontset	fontset = NOFONTSET;
# endif
#endif
    attrentry_T	*aep = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		draw_flags;
    പൂർണ്ണസംഖ്യ		col = gui.col;
#ifdef FEAT_SIGN_ICONS
    പൂർണ്ണസംഖ്യ		draw_sign = FALSE;
    പൂർണ്ണസംഖ്യ		signcol = 0;
    char_u	extra[18];
# ifdef FEAT_NETBEANS_INTG
    പൂർണ്ണസംഖ്യ		multi_sign = FALSE;
# endif
#endif

    എങ്കിൽ (len < 0)
	len = (പൂർണ്ണസംഖ്യ)STRLEN(s);
    എങ്കിൽ (len == 0)
	തിരികെ OK;

#ifdef FEAT_SIGN_ICONS
    എങ്കിൽ (*s == SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
	  || *s == MULTISIGN_BYTE
# endif
       )
    {
	size_t	slen = 2;		    // 2 spaces by default

# ifdef FEAT_NETBEANS_INTG
	എങ്കിൽ (*s == MULTISIGN_BYTE)
	    multi_sign = TRUE;
# endif
	// draw spaces instead
	എങ്കിൽ (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u' &&
		(curwin->w_p_nu || curwin->w_p_rnu))
	{
	    പൂർണ്ണസംഖ്യ	n = സംഖ്യ_width(curwin);

	    slen = MIN(n, (പൂർണ്ണസംഖ്യ)വലുപ്പം(extra) - 1);
	}
	vim_memset(extra, ' ', slen);
	extra[slen] = NUL;
	s = extra;
	എങ്കിൽ (len == 1 && col > 0)
	    --col;
	len = (പൂർണ്ണസംഖ്യ)slen;
	എങ്കിൽ (len > 2)
	    // right align sign icon in the number column
	    signcol = col + len - 3;
	അല്ലെങ്കിൽ
	    signcol = col;
	draw_sign = TRUE;
	highlight_mask = 0;
    }
    അല്ലെങ്കിൽ
#endif
    എങ്കിൽ (gui.highlight_mask > HL_ALL)
    {
	aep = syn_gui_attr2entry(gui.highlight_mask);
	എങ്കിൽ (aep == ശൂന്യ)	    // highlighting not set
	    highlight_mask = 0;
	അല്ലെങ്കിൽ
	    highlight_mask = aep->ae_attr;
    }
    അല്ലെങ്കിൽ
	highlight_mask = gui.highlight_mask;
    hl_mask_todo = highlight_mask;

#എങ്കിൽ !defined(FEAT_GUI_GTK)
    // Set the font
    എങ്കിൽ (aep != ശൂന്യ && aep->ae_u.gui.font != NOFONT)
	font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
    അല്ലെങ്കിൽ എങ്കിൽ (aep != ശൂന്യ && aep->ae_u.gui.fontset != NOFONTSET)
	fontset = aep->ae_u.gui.fontset;
# endif
    അല്ലെങ്കിൽ
    {
# ifdef FEAT_XFONTSET
	എങ്കിൽ (gui.fontset != NOFONTSET)
	    fontset = gui.fontset;
	അല്ലെങ്കിൽ
# endif
	    എങ്കിൽ (hl_mask_todo & (HL_BOLD | HL_STANDOUT))
	{
	    എങ്കിൽ ((hl_mask_todo & HL_ITALIC) && gui.boldital_font != NOFONT)
	    {
		font = gui.boldital_font;
		hl_mask_todo &= ~(HL_BOLD | HL_STANDOUT | HL_ITALIC);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (gui.bold_font != NOFONT)
	    {
		font = gui.bold_font;
		hl_mask_todo &= ~(HL_BOLD | HL_STANDOUT);
	    }
	    അല്ലെങ്കിൽ
		font = gui.norm_font;
	}
	അല്ലെങ്കിൽ എങ്കിൽ ((hl_mask_todo & HL_ITALIC) && gui.ital_font != NOFONT)
	{
	    font = gui.ital_font;
	    hl_mask_todo &= ~HL_ITALIC;
	}
	അല്ലെങ്കിൽ
	    font = gui.norm_font;

	/*
	 * Choose correct wide_font by font.  wide_font should be set with font
	 * at same time in above block.  But it will make many "ifdef" nasty
	 * blocks.  So we do it here.
	 */
	എങ്കിൽ (font == gui.boldital_font && gui.wide_boldital_font)
	    wide_font = gui.wide_boldital_font;
	അല്ലെങ്കിൽ എങ്കിൽ (font == gui.bold_font && gui.wide_bold_font)
	    wide_font = gui.wide_bold_font;
	അല്ലെങ്കിൽ എങ്കിൽ (font == gui.ital_font && gui.wide_ital_font)
	    wide_font = gui.wide_ital_font;
	അല്ലെങ്കിൽ എങ്കിൽ (font == gui.norm_font && gui.wide_font)
	    wide_font = gui.wide_font;
    }
# ifdef FEAT_XFONTSET
    എങ്കിൽ (fontset != NOFONTSET)
	gui_mch_set_fontset(fontset);
    അല്ലെങ്കിൽ
# endif
	gui_mch_set_font(font);
#endif

    draw_flags = 0;

    // Set the color
    bg_color = gui.back_pixel;
    എങ്കിൽ ((flags & GUI_MON_IS_CURSOR) && gui.in_focus)
    {
	draw_flags |= DRAW_CURSOR;
	fg_color = fg;
	bg_color = bg;
	sp_color = fg;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (aep != ശൂന്യ)
    {
	fg_color = aep->ae_u.gui.fg_color;
	എങ്കിൽ (fg_color == INVALCOLOR)
	    fg_color = gui.norm_pixel;
	bg_color = aep->ae_u.gui.bg_color;
	എങ്കിൽ (bg_color == INVALCOLOR)
	    bg_color = gui.back_pixel;
	sp_color = aep->ae_u.gui.sp_color;
	എങ്കിൽ (sp_color == INVALCOLOR)
	    sp_color = fg_color;
    }
    അല്ലെങ്കിൽ
    {
	fg_color = gui.norm_pixel;
	sp_color = fg_color;
    }

    എങ്കിൽ (highlight_mask & (HL_INVERSE | HL_STANDOUT))
    {
	gui_mch_set_fg_color(bg_color);
	gui_mch_set_bg_color(fg_color);
    }
    അല്ലെങ്കിൽ
    {
	gui_mch_set_fg_color(fg_color);
	gui_mch_set_bg_color(bg_color);
    }
    gui_mch_set_sp_color(sp_color);

    // Clear the selection if we are about to write over it
    എങ്കിൽ (!(flags & GUI_MON_NOCLEAR))
	clip_may_clear_selection(gui.row, gui.row);


    // If there's no bold font, then fake it
    എങ്കിൽ (hl_mask_todo & (HL_BOLD | HL_STANDOUT))
	draw_flags |= DRAW_BOLD;

    /*
     * When drawing bold or italic characters the spill-over from the left
     * neighbor may be destroyed.  Let the caller backup to start redrawing
     * just after a blank.
     */
    എങ്കിൽ (back != 0 && ((draw_flags & DRAW_BOLD) || (highlight_mask & HL_ITALIC)))
	തിരികെ FAIL;

#എങ്കിൽ defined(FEAT_GUI_GTK)
    // If there's no italic font, then fake it.
    // For GTK2, we don't need a different font for italic style.
    എങ്കിൽ (hl_mask_todo & HL_ITALIC)
	draw_flags |= DRAW_ITALIC;

    // Do we underline the text?
    എങ്കിൽ (hl_mask_todo & HL_UNDERLINE)
	draw_flags |= DRAW_UNDERL;

#അല്ലെങ്കിൽ
    // Do we underline the text?
    എങ്കിൽ ((hl_mask_todo & HL_UNDERLINE) || (hl_mask_todo & HL_ITALIC))
	draw_flags |= DRAW_UNDERL;
#endif
    // Do we undercurl the text?
    എങ്കിൽ (hl_mask_todo & HL_UNDERCURL)
	draw_flags |= DRAW_UNDERC;

    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

    // Do we strikethrough the text?
    എങ്കിൽ (hl_mask_todo & HL_STRIKETHROUGH)
	draw_flags |= DRAW_STRIKE;

    // Do we draw transparently?
    എങ്കിൽ (flags & GUI_MON_TRS_CURSOR)
	draw_flags |= DRAW_TRANSP;

    /*
     * Draw the text.
     */
#ifdef FEAT_GUI_GTK
    // The value returned is the length in display cells
    len = gui_gtk_draw_string(gui.row, col, s, len, draw_flags);
#അല്ലെങ്കിൽ
    എങ്കിൽ (enc_utf8)
    {
	പൂർണ്ണസംഖ്യ	start;		// index of bytes to be drawn
	പൂർണ്ണസംഖ്യ	cells;		// cellwidth of bytes to be drawn
	പൂർണ്ണസംഖ്യ	thislen;	// length of bytes to be drawn
	പൂർണ്ണസംഖ്യ	cn;		// cellwidth of current char
	പൂർണ്ണസംഖ്യ	i;		// index of current char
	പൂർണ്ണസംഖ്യ	c;		// current char value
	പൂർണ്ണസംഖ്യ	cl;		// byte length of current char
	പൂർണ്ണസംഖ്യ	comping;	// current char is composing
	പൂർണ്ണസംഖ്യ	scol = col;	// screen column
	പൂർണ്ണസംഖ്യ	curr_wide = FALSE;  // use 'guifontwide'
	പൂർണ്ണസംഖ്യ	prev_wide = FALSE;
	പൂർണ്ണസംഖ്യ	wide_changed;
# ifdef MSWIN
	പൂർണ്ണസംഖ്യ	sep_comp = FALSE;   // Don't separate composing chars.
# അല്ലെങ്കിൽ
	പൂർണ്ണസംഖ്യ	sep_comp = TRUE;    // Separate composing chars.
# endif

	// Break the string at a composing character, it has to be drawn on
	// top of the previous character.
	start = 0;
	cells = 0;
	വേണ്ടി (i = 0; i < len; i += cl)
	{
	    c = utf_ptr2char(s + i);
	    cn = utf_char2cells(c);
	    comping = utf_iscomposing(c);
	    എങ്കിൽ (!comping)	// count cells from non-composing chars
		cells += cn;
	    എങ്കിൽ (!comping || sep_comp)
	    {
		എങ്കിൽ (cn > 1
# ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
# endif
			&& wide_font != NOFONT)
		    curr_wide = TRUE;
		അല്ലെങ്കിൽ
		    curr_wide = FALSE;
	    }
	    cl = utf_ptr2len(s + i);
	    എങ്കിൽ (cl == 0)	// hit end of string
		len = i + cl;	// len must be wrong "cannot happen"

	    wide_changed = curr_wide != prev_wide;

	    // Print the string so far if it's the last character or there is
	    // a composing character.
	    എങ്കിൽ (i + cl >= len || (comping && sep_comp && i > start)
		    || wide_changed
# എങ്കിൽ defined(FEAT_GUI_X11)
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
		എങ്കിൽ ((comping && sep_comp) || wide_changed)
		    thislen = i - start;
		അല്ലെങ്കിൽ
		    thislen = i - start + cl;
		എങ്കിൽ (thislen > 0)
		{
		    എങ്കിൽ (prev_wide)
			gui_mch_set_font(wide_font);
		    gui_mch_draw_string(gui.row, scol, s + start, thislen,
								  draw_flags);
		    എങ്കിൽ (prev_wide)
			gui_mch_set_font(font);
		    start += thislen;
		}
		scol += cells;
		cells = 0;
		// Adjust to not draw a character which width is changed
		// against with last one.
		എങ്കിൽ (wide_changed && !(comping && sep_comp))
		{
		    scol -= cn;
		    cl = 0;
		}

# എങ്കിൽ defined(FEAT_GUI_X11)
		// No fontset: draw a space to fill the gap after a wide char
		//
		എങ്കിൽ (cn > 1 && (draw_flags & DRAW_TRANSP) == 0
#  ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
#  endif
			&& !wide_changed)
		    gui_mch_draw_string(gui.row, scol - 1, (char_u *)" ",
							       1, draw_flags);
# endif
	    }
	    // Draw a composing char on top of the previous char.
	    എങ്കിൽ (comping && sep_comp)
	    {
# എങ്കിൽ defined(__APPLE_CC__) && TARGET_API_MAC_CARBON
		// Carbon ATSUI autodraws composing char over previous char
		gui_mch_draw_string(gui.row, scol, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# അല്ലെങ്കിൽ
		gui_mch_draw_string(gui.row, scol - cn, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# endif
		start = i + cl;
	    }
	    prev_wide = curr_wide;
	}
	// The stuff below assumes "len" is the length in screen columns.
	len = scol - col;
    }
    അല്ലെങ്കിൽ
    {
	gui_mch_draw_string(gui.row, col, s, len, draw_flags);
	എങ്കിൽ (enc_dbcs == DBCS_JPNU)
	{
	    // Get the length in display cells, this can be different from the
	    // number of bytes for "euc-jp".
	    len = mb_string2cells(s, len);
	}
    }
#endif // !FEAT_GUI_GTK

    എങ്കിൽ (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
	gui.col = col + len;

    // May need to invert it when it's part of the selection.
    എങ്കിൽ (flags & GUI_MON_NOCLEAR)
	clip_may_redraw_selection(gui.row, col, len);

    എങ്കിൽ (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
    {
	// Invalidate the old physical cursor position if we wrote over it
	എങ്കിൽ (gui.cursor_row == gui.row
		&& gui.cursor_col >= col
		&& gui.cursor_col < col + len)
	    gui.cursor_is_valid = false;
    }

#ifdef FEAT_SIGN_ICONS
    എങ്കിൽ (draw_sign)
	// Draw the sign on top of the spaces.
	gui_mch_drawsign(gui.row, signcol, gui.highlight_mask);
# എങ്കിൽ defined(FEAT_NETBEANS_INTG) && (defined(FEAT_GUI_X11) \
	|| defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN))
    എങ്കിൽ (multi_sign)
	netbeans_draw_multisign_indicator(gui.row);
# endif
#endif

    തിരികെ OK;
}

/*
 * Undraw the cursor.  This actually redraws the character at the cursor
 * position, plus some more characters when needed.
 */
    ശൂന്യം
gui_undraw_cursor(ശൂന്യം)
{
    എങ്കിൽ (!gui.cursor_is_valid)
	തിരികെ;

    // Always redraw the character just before if there is one, because
    // with some fonts and characters there can be a one pixel overlap.
    പൂർണ്ണസംഖ്യ startcol = gui.cursor_col > 0 ? gui.cursor_col - 1 : gui.cursor_col;
    പൂർണ്ണസംഖ്യ endcol = gui.cursor_col;

#എങ്കിൽ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    gui_adjust_undraw_cursor_for_ligatures(&startcol, &endcol);
#endif
    gui_redraw_block(gui.cursor_row, startcol,
	    gui.cursor_row, endcol, GUI_MON_NOCLEAR);

    // Cursor_is_valid is reset when the cursor is undrawn, also reset it
    // here in case it wasn't needed to undraw it.
    gui.cursor_is_valid = false;
}

    ശൂന്യം
gui_redraw(
    പൂർണ്ണസംഖ്യ		x,
    പൂർണ്ണസംഖ്യ		y,
    പൂർണ്ണസംഖ്യ		w,
    പൂർണ്ണസംഖ്യ		h)
{
    പൂർണ്ണസംഖ്യ		row1, col1, row2, col2;

    row1 = Y_2_ROW(y);
    col1 = X_2_COL(x);
    row2 = Y_2_ROW(y + h - 1);
    col2 = X_2_COL(x + w - 1);

    gui_redraw_block(row1, col1, row2, col2, GUI_MON_NOCLEAR);

    /*
     * We may need to redraw the cursor, but don't take it upon us to change
     * its location after a scroll.
     * (maybe be more strict even and test col too?)
     * These things may be outside the update/clipping region and reality may
     * not reflect Vims internal ideas if these operations are clipped away.
     */
    എങ്കിൽ (gui.row == gui.cursor_row)
	gui_update_cursor(TRUE, TRUE);
}

/*
 * Draw a rectangular block of characters, from row1 to row2 (inclusive) and
 * from col1 to col2 (inclusive).
 */
    ശൂന്യം
gui_redraw_block(
    പൂർണ്ണസംഖ്യ		row1,
    പൂർണ്ണസംഖ്യ		col1,
    പൂർണ്ണസംഖ്യ		row2,
    പൂർണ്ണസംഖ്യ		col2,
    പൂർണ്ണസംഖ്യ		flags)	// flags for gui_outstr_nowrap()
{
    പൂർണ്ണസംഖ്യ		old_row, old_col;
    long_u	old_hl_mask;
    പൂർണ്ണസംഖ്യ		off;
    sattr_T	first_attr;
    പൂർണ്ണസംഖ്യ		idx, len;
    പൂർണ്ണസംഖ്യ		back, nback;
    പൂർണ്ണസംഖ്യ		orig_col1, orig_col2;

    // Don't try to update when ScreenLines is not valid
    എങ്കിൽ (!screen_cleared || Screen_വരികൾ == ശൂന്യ)
	തിരികെ;

    // Don't try to draw outside the shell!
    // Check everything, strange values may be caused by a big border width
    col1 = check_col(col1);
    col2 = check_col(col2);
    row1 = check_row(row1);
    row2 = check_row(row2);

    // Remember where our cursor was
    old_row = gui.row;
    old_col = gui.col;
    old_hl_mask = gui.highlight_mask;
    orig_col1 = col1;
    orig_col2 = col2;

    വേണ്ടി (gui.row = row1; gui.row <= row2; gui.row++)
    {
	// When only half of a double-wide character is in the block, include
	// the other half.
	col1 = orig_col1;
	col2 = orig_col2;
	off = വരി_Offset[gui.row];
	എങ്കിൽ (enc_dbcs != 0)
	{
	    എങ്കിൽ (col1 > 0)
		col1 -= dbcs_screen_head_off(Screen_വരികൾ + off,
						    Screen_വരികൾ + off + col1);
	    col2 += dbcs_screen_tail_off(Screen_വരികൾ + off,
						    Screen_വരികൾ + off + col2);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (enc_utf8)
	{
	    എങ്കിൽ (Screen_വരികൾ[off + col1] == 0)
	    {
		എങ്കിൽ (col1 > 0)
		    --col1;
		അല്ലെങ്കിൽ
		    // FIXME: how can the first character ever be zero?
		    siemsg("NUL in ScreenLines in row %ld", (long)gui.row);
	    }
#ifdef FEAT_GUI_GTK
	    എങ്കിൽ (col2 + 1 < Columns && Screen_വരികൾ[off + col2 + 1] == 0)
		++col2;
#endif
	}
	gui.col = col1;
	off = വരി_Offset[gui.row] + gui.col;
	len = col2 - col1 + 1;

	// Find how many chars back this highlighting starts, or where a space
	// is.  Needed for when the bold trick is used
	വേണ്ടി (back = 0; back < col1; ++back)
	    എങ്കിൽ (ScreenAttrs[off - 1 - back] != ScreenAttrs[off]
		    || Screen_വരികൾ[off - 1 - back] == ' ')
		നിർത്തുക;

	// Break it up in strings of characters with the same attributes.
	// Print UTF-8 characters individually.
	ആകുമ്പോൾ (len > 0)
	{
	    first_attr = ScreenAttrs[off];
	    gui.highlight_mask = first_attr;
#എങ്കിൽ !defined(FEAT_GUI_GTK)
	    എങ്കിൽ (enc_utf8 && Screen_വരികൾ_UC[off] != 0)
	    {
		// output multi-byte character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
		എങ്കിൽ (gui.col < Columns && Screen_വരികൾ[off + 1] == 0)
		    idx = 2;
		അല്ലെങ്കിൽ
		    idx = 1;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (enc_dbcs == DBCS_JPNU && Screen_വരികൾ[off] == 0x8e)
	    {
		// output double-byte, single-width character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
		idx = 1;
	    }
	    അല്ലെങ്കിൽ
#endif
	    {
#ifdef FEAT_GUI_GTK
		വേണ്ടി (idx = 0; idx < len; ++idx)
		{
		    എങ്കിൽ (enc_utf8 && Screen_വരികൾ[off + idx] == 0)
			തുടരുക; // skip second half of double-width char
		    എങ്കിൽ (ScreenAttrs[off + idx] != first_attr)
			നിർത്തുക;
		}
		// gui_screenstr() takes care of multibyte chars
		nback = gui_screenstr(off, idx, flags,
				      (guicolor_T)0, (guicolor_T)0, back);
#അല്ലെങ്കിൽ
		വേണ്ടി (idx = 0; idx < len && ScreenAttrs[off + idx] == first_attr;
									idx++)
		{
		    // Stop at a multi-byte Unicode character.
		    എങ്കിൽ (enc_utf8 && Screen_വരികൾ_UC[off + idx] != 0)
			നിർത്തുക;
		    എങ്കിൽ (enc_dbcs == DBCS_JPNU)
		    {
			// Stop at a double-byte single-width char.
			എങ്കിൽ (Screen_വരികൾ[off + idx] == 0x8e)
			    നിർത്തുക;
			എങ്കിൽ (len > 1 && (*mb_ptr2len)(Screen_വരികൾ
							    + off + idx) == 2)
			    ++idx;  // skip second byte of double-byte char
		    }
		}
		nback = gui_outstr_nowrap(Screen_വരികൾ + off, idx, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
#endif
	    }
	    എങ്കിൽ (nback == FAIL)
	    {
		// Must back up to start drawing where a bold or italic word
		// starts.
		off -= back;
		len += back;
		gui.col -= back;
	    }
	    അല്ലെങ്കിൽ
	    {
		off += idx;
		len -= idx;
	    }
	    back = 0;
	}
    }

    // Put the cursor back where it was
    gui.row = old_row;
    gui.col = old_col;
    gui.highlight_mask = (പൂർണ്ണസംഖ്യ)old_hl_mask;
}

    സ്ഥിതം ശൂന്യം
gui_delete_വരികൾ(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ എണ്ണം)
{
    എങ്കിൽ (എണ്ണം <= 0)
	തിരികെ;

    എങ്കിൽ (row + എണ്ണം > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_block(row, gui.scroll_region_ഇടത്,
			      gui.scroll_region_bot, gui.scroll_region_വലത്);
    അല്ലെങ്കിൽ
    {
	gui_mch_delete_വരികൾ(row, എണ്ണം);

	// If the cursor was in the deleted lines it's now gone.  If the
	// cursor was in the scrolled lines adjust its position.
	എങ്കിൽ (gui.cursor_row >= row
		&& gui.cursor_col >= gui.scroll_region_ഇടത്
		&& gui.cursor_col <= gui.scroll_region_വലത്)
	{
	    എങ്കിൽ (gui.cursor_row < row + എണ്ണം)
		gui.cursor_is_valid = false;
	    അല്ലെങ്കിൽ എങ്കിൽ (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_row -= എണ്ണം;
	}
    }
}

    സ്ഥിതം ശൂന്യം
gui_തിരുകുക_വരികൾ(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ എണ്ണം)
{
    എങ്കിൽ (എണ്ണം <= 0)
	തിരികെ;

    എങ്കിൽ (row + എണ്ണം > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_block(row, gui.scroll_region_ഇടത്,
			      gui.scroll_region_bot, gui.scroll_region_വലത്);
    അല്ലെങ്കിൽ
    {
	gui_mch_തിരുകുക_വരികൾ(row, എണ്ണം);

	എങ്കിൽ (gui.cursor_row >= gui.row
		&& gui.cursor_col >= gui.scroll_region_ഇടത്
		&& gui.cursor_col <= gui.scroll_region_വലത്)
	{
	    എങ്കിൽ (gui.cursor_row <= gui.scroll_region_bot - എണ്ണം)
		gui.cursor_row += എണ്ണം;
	    അല്ലെങ്കിൽ എങ്കിൽ (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_is_valid = false;
	}
    }
}

#ifdef FEAT_TIMERS
/*
 * Passed to ui_wait_for_chars_or_timer(), ignoring extra arguments.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_wait_for_chars_3(
    long wtime,
    പൂർണ്ണസംഖ്യ *interrupted UNUSED,
    പൂർണ്ണസംഖ്യ ignore_നിവേശം UNUSED)
{
    തിരികെ gui_mch_wait_for_chars(wtime);
}
#endif

/*
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_wait_for_chars_or_timer(
	long wtime,
	പൂർണ്ണസംഖ്യ *interrupted UNUSED,
	പൂർണ്ണസംഖ്യ ignore_നിവേശം UNUSED)
{
#ifdef FEAT_TIMERS
    തിരികെ ui_wait_for_chars_or_timer(wtime, gui_wait_for_chars_3,
						    interrupted, ignore_നിവേശം);
#അല്ലെങ്കിൽ
    തിരികെ gui_mch_wait_for_chars(wtime);
#endif
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_wait_for_chars_buf(
    char_u	*buf,
    പൂർണ്ണസംഖ്യ		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    പൂർണ്ണസംഖ്യ		tb_change_cnt)
{
    പൂർണ്ണസംഖ്യ	    retval;
    പൂർണ്ണസംഖ്യ	    keep_blinking; // Guard against restarting blink cycle on CursorHold

#ifdef FEAT_MENU
    // If we're going to wait a bit, update the menus and mouse shape for the
    // current State.
    എങ്കിൽ (wtime != 0)
	gui_update_menus(0);
#endif

    gui_mch_update();
    എങ്കിൽ (നിവേശം_available())	// Got char, return immediately
    {
	എങ്കിൽ (gui_mch_is_blinking())
	    gui_mch_stop_blink(TRUE);
	എങ്കിൽ (buf != ശൂന്യ && !typebuf_changed(tb_change_cnt))
	    തിരികെ read_from_നിവേശം_buf(buf, (long)maxlen);
	തിരികെ 0;
    }
    എങ്കിൽ (wtime == 0)		// Don't wait for char
	തിരികെ FAIL;

    // Before waiting, flush any output to the screen.
    gui_mch_flush();

    // Blink while waiting for a character.
    എങ്കിൽ (!gui_mch_is_blinking())
	gui_mch_start_blink();

    // Common function to loop until "wtime" is met, while handling timers and
    // other callbacks.
    retval = inchar_loop(buf, maxlen, wtime, tb_change_cnt,
			 gui_wait_for_chars_or_timer, ശൂന്യ);

    // Keep blinking when CursorHold wakes the input loop. (See PR #21115)
    keep_blinking = retval == 3 && buf != ശൂന്യ
	&& buf[0] == K_SPECIAL && buf[1] == KS_EXTRA
	&& buf[2] == (പൂർണ്ണസംഖ്യ)KE_CURSORHOLD;

    എങ്കിൽ (!keep_blinking)
	gui_mch_stop_blink(TRUE);

    തിരികെ retval;
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
    പൂർണ്ണസംഖ്യ
gui_wait_for_chars(long wtime, പൂർണ്ണസംഖ്യ tb_change_cnt)
{
    തിരികെ gui_wait_for_chars_buf(ശൂന്യ, 0, wtime, tb_change_cnt);
}

/*
 * Equivalent of mch_inchar() for the GUI.
 */
    പൂർണ്ണസംഖ്യ
gui_inchar(
    char_u  *buf,
    പൂർണ്ണസംഖ്യ	    maxlen,
    long    wtime,		// milliseconds
    പൂർണ്ണസംഖ്യ	    tb_change_cnt)
{
    തിരികെ gui_wait_for_chars_buf(buf, maxlen, wtime, tb_change_cnt);
}

/*
 * Fill p[4] with mouse coordinates encoded for check_termcode().
 */
    സ്ഥിതം ശൂന്യം
fill_mouse_coord(char_u *p, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ row)
{
    p[0] = (char_u)(col / 128 + ' ' + 1);
    p[1] = (char_u)(col % 128 + ' ' + 1);
    p[2] = (char_u)(row / 128 + ' ' + 1);
    p[3] = (char_u)(row % 128 + ' ' + 1);
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
    ശൂന്യം
gui_send_mouse_event(
    പൂർണ്ണസംഖ്യ	    button,
    പൂർണ്ണസംഖ്യ	    x,
    പൂർണ്ണസംഖ്യ	    y,
    പൂർണ്ണസംഖ്യ	    repeated_click,
    int_u   modifiers)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    prev_row = 0, prev_col = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    prev_button = -1;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    അക്കം_clicks = 1;
    char_u	    string[10];
    എണ്ണൽ താക്കോൽ_extra  button_char;
    പൂർണ്ണസംഖ്യ		    row, col;
#ifdef FEAT_CLIPBOARD
    പൂർണ്ണസംഖ്യ		    checkfor;
    പൂർണ്ണസംഖ്യ		    did_clip = FALSE;
#endif

    /*
     * Scrolling may happen at any time, also while a selection is present.
     */
    തിരഞ്ഞെടുപ്പ് (button)
    {
	അവസ്ഥ MOUSE_MOVE:
	    button_char = KE_MOUSEMOVE_XY;
	    പോകുക button_set;
	അവസ്ഥ MOUSE_X1:
	    button_char = KE_X1MOUSE;
	    പോകുക button_set;
	അവസ്ഥ MOUSE_X2:
	    button_char = KE_X2MOUSE;
	    പോകുക button_set;
	അവസ്ഥ MOUSE_4:
	    button_char = KE_MOUSEDOWN;
	    പോകുക button_set;
	അവസ്ഥ MOUSE_5:
	    button_char = KE_MOUSEUP;
	    പോകുക button_set;
	അവസ്ഥ MOUSE_6:
	    button_char = KE_MOUSELEFT;
	    പോകുക button_set;
	അവസ്ഥ MOUSE_7:
	    button_char = KE_MOUSERIGHT;
button_set:
	    {
		// Don't put events in the input queue now.
		എങ്കിൽ (hold_gui_events)
		    തിരികെ;

		row = gui_xy2colrow(x, y, &col);
		// Don't report a mouse move unless moved to a
		// different character position.
		എങ്കിൽ (button == MOUSE_MOVE)
		{
		    എങ്കിൽ (row == prev_row && col == prev_col)
			തിരികെ;
		    അല്ലെങ്കിൽ
		    {
			prev_row = row >= 0 ? row : 0;
			prev_col = col;
		    }
		}

		string[3] = CSI;
		string[4] = KS_EXTRA;
		string[5] = (പൂർണ്ണസംഖ്യ)button_char;

		// Pass the pointer coordinates of the scroll event so that we
		// know which window to scroll.
		string[6] = (char_u)(col / 128 + ' ' + 1);
		string[7] = (char_u)(col % 128 + ' ' + 1);
		string[8] = (char_u)(row / 128 + ' ' + 1);
		string[9] = (char_u)(row % 128 + ' ' + 1);

		എങ്കിൽ (modifiers == 0)
		    add_to_നിവേശം_buf(string + 3, 7);
		അല്ലെങ്കിൽ
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = 0;
		    എങ്കിൽ (modifiers & MOUSE_SHIFT)
			string[2] |= MOD_MASK_SHIFT;
		    എങ്കിൽ (modifiers & MOUSE_CTRL)
			string[2] |= MOD_MASK_CTRL;
		    എങ്കിൽ (modifiers & MOUSE_ALT)
			string[2] |= MOD_MASK_ALT;
		    add_to_നിവേശം_buf(string, 10);
		}
		തിരികെ;
	    }
    }

#ifdef FEAT_CLIPBOARD
    // If a clipboard selection is in progress, handle it
    എങ്കിൽ (clip_star.state == SELECT_IN_PROGRESS)
    {
	clip_process_selection(button, X_2_COL(x), Y_2_ROW(y), repeated_click);

	// A release event may still need to be sent if the position is equal.
	row = gui_xy2colrow(x, y, &col);
	എങ്കിൽ (button != MOUSE_RELEASE || row != prev_row || col != prev_col)
	    തിരികെ;
    }

    // Determine which mouse settings to look for based on the current mode
    തിരഞ്ഞെടുപ്പ് (get_real_state())
    {
	അവസ്ഥ MODE_NORMAL_BUSY:
	അവസ്ഥ MODE_OP_PENDING:
# ifdef FEAT_TERMINAL
	അവസ്ഥ MODE_TERMINAL:
# endif
	അവസ്ഥ MODE_NORMAL:	checkfor = MOUSE_NORMAL; നിർത്തുക;
	അവസ്ഥ MODE_VISUAL:	checkfor = MOUSE_VISUAL; നിർത്തുക;
	അവസ്ഥ MODE_SELECT:	checkfor = MOUSE_VISUAL; നിർത്തുക;
	അവസ്ഥ MODE_REPLACE:
	അവസ്ഥ MODE_REPLACE | MODE_LANGMAP:
	അവസ്ഥ MODE_VREPLACE:
	അവസ്ഥ MODE_VREPLACE | MODE_LANGMAP:
	അവസ്ഥ MODE_തിരുകുക:
	അവസ്ഥ MODE_തിരുകുക | MODE_LANGMAP:
				checkfor = MOUSE_തിരുകുക; നിർത്തുക;
	അവസ്ഥ MODE_ASKMORE:
	അവസ്ഥ MODE_HITRETURN:	// At the more- and hit-enter prompt pass the
				// mouse event for a click on or below the
				// message line.
				എങ്കിൽ (Y_2_ROW(y) >= സന്ദേശം_row)
				    checkfor = MOUSE_NORMAL;
				അല്ലെങ്കിൽ
				    checkfor = MOUSE_RETURN;
				നിർത്തുക;

	    /*
	     * On the command line, use the clipboard selection on all lines
	     * but the command line.  But not when pasting.
	     */
	അവസ്ഥ MODE_CMDLINE:
	അവസ്ഥ MODE_CMDLINE | MODE_LANGMAP:
	    എങ്കിൽ (Y_2_ROW(y) < cmdline_row && button != MOUSE_നടുവ്)
		checkfor = MOUSE_NONE;
	    അല്ലെങ്കിൽ
		checkfor = MOUSE_COMMAND;
	    നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
	    checkfor = MOUSE_NONE;
	    നിർത്തുക;
    };

    /*
     * Allow clipboard selection of text on the command line in "normal"
     * modes.  Don't do this when dragging the status line, or extending a
     * Visual selection.
     */
    എങ്കിൽ ((State == MODE_NORMAL || State == MODE_NORMAL_BUSY
						      || (State & MODE_തിരുകുക))
	    && X_2_COL(x) >= firstwin->w_wincol
	    && X_2_COL(x) < firstwin->w_wincol + topframe->fr_width
	    && Y_2_ROW(y) >= topframe->fr_height + firstwin->w_winrow
	    && button != MOUSE_DRAG
# ifdef FEAT_MOUSESHAPE
	    && !drag_status_വരി
	    && !drag_sep_വരി
# endif
	    )
	checkfor = MOUSE_NONE;

    /*
     * Use modeless selection when holding CTRL and SHIFT pressed.
     */
    എങ്കിൽ ((modifiers & MOUSE_CTRL) && (modifiers & MOUSE_SHIFT))
	checkfor = MOUSE_NONEF;

    /*
     * In Ex mode, always use modeless selection.
     */
    എങ്കിൽ (exmode_active)
	checkfor = MOUSE_NONE;

    /*
     * If the mouse settings say to not use the mouse, use the modeless
     * selection.  But if Visual is active, assume that only the Visual area
     * will be selected.
     * Exception: On the command line, both the selection is used and a mouse
     * key is sent.
     */
    എങ്കിൽ (!mouse_has(checkfor) || checkfor == MOUSE_COMMAND)
    {
	// Don't do modeless selection in Visual mode.
	എങ്കിൽ (checkfor != MOUSE_NONEF && VIsual_active && (State & MODE_NORMAL))
	    തിരികെ;

	/*
	 * When 'mousemodel' is "popup", shift-left is translated to right.
	 * But not when also using Ctrl.
	 */
	എങ്കിൽ (mouse_model_popup() && button == MOUSE_ഇടത്
		&& (modifiers & MOUSE_SHIFT) && !(modifiers & MOUSE_CTRL))
	{
	    button = MOUSE_വലത്;
	    modifiers &= ~ MOUSE_SHIFT;
	}

	// If the selection is done, allow the right button to extend it.
	// If the selection is cleared, allow the right button to start it
	// from the cursor position.
	എങ്കിൽ (button == MOUSE_വലത്)
	{
	    എങ്കിൽ (clip_star.state == SELECT_CLEARED)
	    {
		എങ്കിൽ (State & MODE_CMDLINE)
		{
		    col = സന്ദേശം_col;
		    row = സന്ദേശം_row;
		}
		അല്ലെങ്കിൽ
		{
		    col = curwin->w_wcol;
		    row = curwin->w_wrow + W_WINROW(curwin);
		}
		clip_start_selection(col, row, FALSE);
	    }
	    clip_process_selection(button, X_2_COL(x), Y_2_ROW(y),
							      repeated_click);
	    did_clip = TRUE;
	}
	// Allow the left button to start the selection
	അല്ലെങ്കിൽ എങ്കിൽ (button == MOUSE_ഇടത്)
	{
	    clip_start_selection(X_2_COL(x), Y_2_ROW(y), repeated_click);
	    did_clip = TRUE;
	}

	// Always allow pasting
	എങ്കിൽ (button != MOUSE_നടുവ്)
	{
	    എങ്കിൽ (!mouse_has(checkfor) || button == MOUSE_RELEASE)
		തിരികെ;
	    എങ്കിൽ (checkfor != MOUSE_COMMAND)
		button = MOUSE_ഇടത്;
	}
	repeated_click = FALSE;
    }

    എങ്കിൽ (clip_star.state != SELECT_CLEARED && !did_clip)
	clip_clear_selection(&clip_star);
#endif

    // Don't put events in the input queue now.
    എങ്കിൽ (hold_gui_events)
	തിരികെ;

    row = gui_xy2colrow(x, y, &col);

    /*
     * If we are dragging and the mouse hasn't moved far enough to be on a
     * different character, then don't send an event to vim.
     */
    എങ്കിൽ (button == MOUSE_DRAG)
    {
	എങ്കിൽ (row == prev_row && col == prev_col)
	    തിരികെ;
	// Dragging above the window, set "row" to -1 to cause a scroll.
	എങ്കിൽ (y < 0)
	    row = -1;
    }

    /*
     * If topline has changed (window scrolled) since the last click, reset
     * repeated_click, because we don't want starting Visual mode when
     * clicking on a different character in the text.
     */
    എങ്കിൽ (curwin->w_topline != gui_prev_topline
#ifdef FEAT_DIFF
	    || curwin->w_topfill != gui_prev_topfill
#endif
	    )
	repeated_click = FALSE;

    string[0] = CSI;	// this sequence is recognized by check_termcode()
    string[1] = KS_MOUSE;
    string[2] = KE_FILLER;
    എങ്കിൽ (button != MOUSE_DRAG && button != MOUSE_RELEASE)
    {
	എങ്കിൽ (repeated_click)
	{
	    /*
	     * Handle multiple clicks.	They only count if the mouse is still
	     * pointing at the same character.
	     */
	    എങ്കിൽ (button != prev_button || row != prev_row || col != prev_col)
		അക്കം_clicks = 1;
	    അല്ലെങ്കിൽ എങ്കിൽ (++അക്കം_clicks > 4)
		അക്കം_clicks = 1;
	}
	അല്ലെങ്കിൽ
	    അക്കം_clicks = 1;
	prev_button = button;
	gui_prev_topline = curwin->w_topline;
#ifdef FEAT_DIFF
	gui_prev_topfill = curwin->w_topfill;
#endif

	string[3] = (char_u)(button | 0x20);
	SET_അക്കം_MOUSE_CLICKS(string[3], അക്കം_clicks);
    }
    അല്ലെങ്കിൽ
	string[3] = (char_u)button;

    string[3] |= modifiers;
    fill_mouse_coord(string + 4, col, row);
    add_to_നിവേശം_buf(string, 8);

    എങ്കിൽ (row < 0)
	prev_row = 0;
    അല്ലെങ്കിൽ
	prev_row = row;
    prev_col = col;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#എങ്കിൽ defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#endif
}

/*
 * Convert x and y coordinate to column and row in text window.
 * Corrects for multi-byte character.
 * returns column in "*colp" and row as return value;
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_xy2colrow(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, പൂർണ്ണസംഖ്യ *colp)
{
    പൂർണ്ണസംഖ്യ		col = check_col(X_2_COL(x));
    പൂർണ്ണസംഖ്യ		row = check_row(Y_2_ROW(y));

    *colp = mb_fix_col(col, row);
    തിരികെ row;
}

#എങ്കിൽ defined(FEAT_MENU)
/*
 * Callback function for when a menu entry has been selected.
 */
    ശൂന്യം
gui_menu_cb(vimmenu_T *menu)
{
    char_u  bytes[വലുപ്പം(long_u)];

    // Don't put events in the input queue now.
    എങ്കിൽ (hold_gui_events)
	തിരികെ;

    bytes[0] = CSI;
    bytes[1] = KS_MENU;
    bytes[2] = KE_FILLER;
    add_to_നിവേശം_buf(bytes, 3);
    add_long_to_buf((long_u)menu, bytes);
    add_to_നിവേശം_buf_csi(bytes, വലുപ്പം(long_u));
}
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ	prev_which_scrollbars[3];

/*
 * Set which components are present.
 * If "oldval" is not NULL, "oldval" is the previous value, the new value is
 * in p_go.
 */
    ശൂന്യം
gui_init_which_components(char_u *oldval UNUSED)
{
#ifdef FEAT_GUI_DARKTHEME
    സ്ഥിതം പൂർണ്ണസംഖ്യ	prev_dark_theme = FALSE;
    പൂർണ്ണസംഖ്യ		using_dark_theme = FALSE;
#endif
#ifdef FEAT_MENU
    സ്ഥിതം പൂർണ്ണസംഖ്യ	prev_menu_is_active = -1;
#endif
#ifdef FEAT_TOOLBAR
    സ്ഥിതം പൂർണ്ണസംഖ്യ	prev_toolbar = -1;
    പൂർണ്ണസംഖ്യ		using_toolbar = FALSE;
#endif
#ifdef FEAT_GUI_TABLINE
    പൂർണ്ണസംഖ്യ		using_tabline;
#endif
#ifdef FEAT_GUI_MSWIN
    സ്ഥിതം പൂർണ്ണസംഖ്യ	prev_titlebar = FALSE;
    പൂർണ്ണസംഖ്യ		using_titlebar = FALSE;
#endif

#എങ്കിൽ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    സ്ഥിതം പൂർണ്ണസംഖ്യ	prev_fullscreen = FALSE;
    പൂർണ്ണസംഖ്യ		using_fullscreen = FALSE;
#endif
#എങ്കിൽ defined(FEAT_MENU)
    സ്ഥിതം പൂർണ്ണസംഖ്യ	prev_tearoff = -1;
    പൂർണ്ണസംഖ്യ		using_tearoff = FALSE;
#endif

    char_u	*p;
    പൂർണ്ണസംഖ്യ		i;
#ifdef FEAT_MENU
    പൂർണ്ണസംഖ്യ		grey_old, grey_new;
    char_u	*temp;
#endif
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		need_set_size;
    പൂർണ്ണസംഖ്യ		fix_size;

#ifdef FEAT_MENU
    എങ്കിൽ (oldval != ശൂന്യ && gui.in_use)
    {
	/*
	 * Check if the menus go from grey to non-grey or vice versa.
	 */
	grey_old = (vim_strchr(oldval, GO_GREY) != ശൂന്യ);
	grey_new = (vim_strchr(p_go, GO_GREY) != ശൂന്യ);
	എങ്കിൽ (grey_old != grey_new)
	{
	    temp = p_go;
	    p_go = oldval;
	    gui_update_menus(MENU_ALL_MODES);
	    p_go = temp;
	}
    }
    gui.menu_is_active = false;
#endif

    വേണ്ടി (i = 0; i < 3; i++)
	gui.which_scrollbars[i] = false;
    വേണ്ടി (p = p_go; *p; p++)
	തിരഞ്ഞെടുപ്പ് (*p)
	{
	    അവസ്ഥ GO_ഇടത്:
		gui.which_scrollbars[SBAR_ഇടത്] = true;
		നിർത്തുക;
	    അവസ്ഥ GO_വലത്:
		gui.which_scrollbars[SBAR_വലത്] = true;
		നിർത്തുക;
	    അവസ്ഥ GO_VLEFT:
		എങ്കിൽ (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_ഇടത്] = true;
		നിർത്തുക;
	    അവസ്ഥ GO_VRIGHT:
		എങ്കിൽ (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_വലത്] = true;
		നിർത്തുക;
	    അവസ്ഥ GO_BOT:
		gui.which_scrollbars[SBAR_BOTTOM] = true;
		നിർത്തുക;
#ifdef FEAT_GUI_DARKTHEME
	    അവസ്ഥ GO_DARKTHEME:
		using_dark_theme = TRUE;
		നിർത്തുക;
#endif
#ifdef FEAT_MENU
	    അവസ്ഥ GO_MENUS:
		gui.menu_is_active = true;
		നിർത്തുക;
#endif
	    അവസ്ഥ GO_GREY:
		// make menu's have grey items, ignored here
		നിർത്തുക;
#ifdef FEAT_GUI_MSWIN
	    അവസ്ഥ GO_TITLEBAR:
		using_titlebar = TRUE;
		നിർത്തുക;
#endif
#എങ്കിൽ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
	    അവസ്ഥ GO_FULLSCREEN:
		using_fullscreen = TRUE;
		നിർത്തുക;
#endif
#ifdef FEAT_TOOLBAR
	    അവസ്ഥ GO_TOOLBAR:
		using_toolbar = TRUE;
		നിർത്തുക;
#endif
	    അവസ്ഥ GO_TEAROFF:
#എങ്കിൽ defined(FEAT_MENU)
		using_tearoff = TRUE;
#endif
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		// Ignore options that are not supported
		നിർത്തുക;
	}

    എങ്കിൽ (!gui.in_use)
	തിരികെ;

    need_set_size = 0;
    fix_size = FALSE;

#ifdef FEAT_GUI_MSWIN
    എങ്കിൽ (using_titlebar != prev_titlebar)
    {
	gui_mch_set_titlebar_colors();
	prev_titlebar = using_titlebar;
    }
#endif

#എങ്കിൽ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    എങ്കിൽ (using_fullscreen != prev_fullscreen)
    {
	gui_mch_set_fullscreen(using_fullscreen);
	prev_fullscreen = using_fullscreen;
    }
#endif

#ifdef FEAT_GUI_DARKTHEME
    എങ്കിൽ (using_dark_theme != prev_dark_theme)
    {
	gui_mch_set_dark_theme(using_dark_theme);
	prev_dark_theme = using_dark_theme;
    }
#endif

#ifdef FEAT_GUI_TABLINE
    // Update the GUI tab line, it may appear or disappear.  This may
    // cause the non-GUI tab line to disappear or appear.
    using_tabline = gui_has_tabline();
    എങ്കിൽ (!gui_mch_showing_tabline() != !using_tabline)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = Rows;
	gui_update_tabline();
	Rows = i;
	need_set_size |= RESIZE_VERT;
	എങ്കിൽ (using_tabline)
	    fix_size = TRUE;
	എങ്കിൽ (!gui_use_tabline())
	    redraw_tabline = TRUE;    // may draw non-GUI tab line
    }
#endif

    വേണ്ടി (i = 0; i < 3; i++)
    {
	// The scrollbar needs to be updated when it is shown/unshown and
	// when switching tab pages.  But the size only changes when it's
	// shown/unshown.  Thus we need two places to remember whether a
	// scrollbar is there or not.
	എങ്കിൽ (gui.which_scrollbars[i] != prev_which_scrollbars[i]
		|| gui.which_scrollbars[i]
		!= curtab->tp_prev_which_scrollbars[i])
	{
	    എങ്കിൽ (i == SBAR_BOTTOM)
		gui_mch_enable_scrollbar(&gui.bottom_sbar,
			gui.which_scrollbars[i]);
	    അല്ലെങ്കിൽ
	    {
		FOR_ALL_WINDOWS(wp)
		    gui_do_scrollbar(wp, i, gui.which_scrollbars[i]);
	    }
	    എങ്കിൽ (gui.which_scrollbars[i] != prev_which_scrollbars[i])
	    {
		എങ്കിൽ (i == SBAR_BOTTOM)
		    need_set_size |= RESIZE_VERT;
		അല്ലെങ്കിൽ
		    need_set_size |= RESIZE_HOR;
		എങ്കിൽ (gui.which_scrollbars[i])
		    fix_size = TRUE;
	    }
	}
	curtab->tp_prev_which_scrollbars[i] = gui.which_scrollbars[i];
	prev_which_scrollbars[i] = gui.which_scrollbars[i];
    }

#ifdef FEAT_MENU
    എങ്കിൽ (gui.menu_is_active != prev_menu_is_active)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = Rows;
	gui_mch_enable_menu(gui.menu_is_active);
	Rows = i;
	prev_menu_is_active = gui.menu_is_active;
	need_set_size |= RESIZE_VERT;
	എങ്കിൽ (gui.menu_is_active)
	    fix_size = TRUE;
    }
#endif

#ifdef FEAT_TOOLBAR
    എങ്കിൽ (using_toolbar != prev_toolbar)
    {
	gui_mch_show_toolbar(using_toolbar);
	prev_toolbar = using_toolbar;
	need_set_size |= RESIZE_VERT;
	എങ്കിൽ (using_toolbar)
	    fix_size = TRUE;
    }
#endif
#എങ്കിൽ defined(FEAT_MENU) && !(defined(MSWIN) && !defined(FEAT_TEAROFF))
    എങ്കിൽ (using_tearoff != prev_tearoff)
    {
	gui_mch_toggle_tearoffs(using_tearoff);
	prev_tearoff = using_tearoff;
    }
#endif
    എങ്കിൽ (need_set_size != 0)
    {
#ifdef FEAT_GUI_GTK
	long    prev_Columns = Columns;
	long    prev_Rows = Rows;
#endif
	// Adjust the size of the window to make the text area keep the
	// same size and to avoid that part of our window is off-screen
	// and a scrollbar can't be used, for example.
	gui_set_shellsize(FALSE, fix_size, need_set_size);

#ifdef FEAT_GUI_GTK
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
	    long    post_Rows = Rows;
	    long    post_Columns = Columns;

	    എങ്കിൽ (!gui.starting && need_set_size != (RESIZE_VERT | RESIZE_HOR))
		(ശൂന്യം)char_avail();
	    എങ്കിൽ ((need_set_size & RESIZE_VERT) == 0)
		Rows = prev_Rows;
	    അല്ലെങ്കിൽ
		Rows = post_Rows;
	    എങ്കിൽ ((need_set_size & RESIZE_HOR) == 0)
		Columns = prev_Columns;
	    അല്ലെങ്കിൽ
		Columns = post_Columns;
	}
#endif
    }
    // When the console tabline appears or disappears the window positions
    // change.
    എങ്കിൽ (firstwin->w_winrow != tabline_height())
	shell_new_rows();	// recompute window positions and heights
}

#എങ്കിൽ defined(FEAT_GUI_TABLINE)
/*
 * Return TRUE if the GUI is taking care of the tabline.
 * It may still be hidden if 'showtabline' is zero.
 */
    പൂർണ്ണസംഖ്യ
gui_use_tabline(ശൂന്യം)
{
    തിരികെ gui.in_use && vim_strchr(p_go, GO_TABLINE) != ശൂന്യ;
}

/*
 * Return TRUE if the GUI is showing the tabline.
 * This uses 'showtabline'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_has_tabline(ശൂന്യം)
{
    എങ്കിൽ (!gui_use_tabline()
	    || p_stal == 0
	    || (p_stal == 1 && first_tabpage->tp_next == ശൂന്യ))
	തിരികെ FALSE;
    തിരികെ TRUE;
}

/*
 * Update the tabline.
 * This may display/undisplay the tabline and update the labels.
 */
    ശൂന്യം
gui_update_tabline(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    showit = gui_has_tabline();
    പൂർണ്ണസംഖ്യ	    shown = gui_mch_showing_tabline();

    എങ്കിൽ (!gui.starting && starting == 0)
    {
	// Updating the tabline uses direct GUI commands, flush
	// outstanding instructions first. (esp. clear screen)
	out_flush();

	എങ്കിൽ (!showit != !shown)
	{
	    // Save Rows/Columns before showing/hiding the tabline.
	    // gui_mch_show_tabline() processes GTK events (via
	    // gui_mch_update) which may trigger a spurious configure event
	    // that temporarily corrupts Rows.  Restore the original values so
	    // that gui_set_shellsize() uses the correct row/column count.
	    പൂർണ്ണസംഖ്യ save_Rows = Rows;
	    പൂർണ്ണസംഖ്യ save_Columns = Columns;

	    gui_mch_show_tabline(showit);
	    Rows = save_Rows;
	    Columns = save_Columns;

	    // Resize the outer window to compensate for the tabline height
	    // change.  This is also needed when called from draw_tabline()
	    // (e.g. after :tabclose), where no caller will do the resize.
	    // When called from gui_init_which_components() the subsequent
	    // gui_set_shellsize() call will redo this to the same dimensions,
	    // which is harmless.
	    gui_set_shellsize(FALSE, showit, RESIZE_VERT);
	}
	എങ്കിൽ (showit != 0)
	    gui_mch_update_tabline();
    }
}

/*
 * Get the label or tooltip for tab page "tp" into NameBuff[].
 */
    ശൂന്യം
get_tabline_label(
    tabpage_T	*tp,
    പൂർണ്ണസംഖ്യ		tooltip)	// TRUE: get tooltip
{
    char_u	**opt;

    // Use 'guitablabel' or 'guitabtooltip' if it's set.
    opt = (tooltip ? &p_gtt : &p_gtl);
    എങ്കിൽ (**opt != NUL)
    {
	char_u	res[MAXPATHL];
	tabpage_T *save_curtab;
	char_u	*opt_name = (char_u *)(tooltip ? "guitabtooltip"
							     : "guitablabel");

	printer_page_അക്കം = tabpage_index(tp);
# ifdef FEAT_EVAL
	set_vim_var_nr(VV_LNUM, printer_page_അക്കം);
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
	curbuf = curwin->w_buffer;

	// Can't use NameBuff directly, build_stl_str_hl() uses it.
	build_stl_str_hl(curwin, res, MAXPATHL, *opt, opt_name, 0,
					   0, (പൂർണ്ണസംഖ്യ)Columns, ശൂന്യ, ശൂന്യ, ശൂന്യ);
	STRCPY(NameBuff, res);

	// Back to the original curtab.
	curtab = save_curtab;
	topframe = curtab->tp_topframe;
	firstwin = curtab->tp_firstwin;
	lastwin = curtab->tp_lastwin;
	curwin = curtab->tp_curwin;
	curbuf = curwin->w_buffer;
    }

    // If 'guitablabel'/'guitabtooltip' is not set or the result is empty then
    // use a default label.
    എങ്കിൽ (**opt == NUL || *NameBuff == NUL)
    {
	win_T	*wp;
	പൂർണ്ണസംഖ്യ	wincount = 0;
	പൂർണ്ണസംഖ്യ	modified = FALSE;

	// Get the buffer name into NameBuff[] and shorten it.
	get_trans_bufname(tp == curtab ? curbuf : tp->tp_curwin->w_buffer);
	എങ്കിൽ (!tooltip)
	    shorten_dir(NameBuff);

	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    ++wincount;
	    എങ്കിൽ (bufIsChanged(wp->w_buffer))
		modified = TRUE;
	}

	എങ്കിൽ (modified || wincount > 1)
	{
	    char_u  buf[40] = "+ ";		// Tentatively assume modified only
	    size_t  buflen = 2;
	    size_t  NameBufflen = STRLEN(NameBuff);

	    എങ്കിൽ (wincount > 1)
		buflen = vim_snprintf_safelen(
		    (അക്ഷരം *)buf,
		    വലുപ്പം(buf),
		    "%d%s ",
		    wincount,
		    modified ? "+" : "");

	    // Make sure resulting NameBuff will not exceed its bounds.
	    എങ്കിൽ (NameBufflen + buflen < MAXPATHL)
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
    പൂർണ്ണസംഖ്യ
send_tabline_event(പൂർണ്ണസംഖ്യ nr)
{
    char_u string[3];

    എങ്കിൽ (nr == tabpage_index(curtab))
	തിരികെ FALSE;

    // Don't put events in the input queue now.
    എങ്കിൽ (hold_gui_events || cmdwin_type != 0)
    {
	// Set it back to the current tab page.
	gui_mch_set_curtab(tabpage_index(curtab));
	തിരികെ FALSE;
    }

    string[0] = CSI;
    string[1] = KS_TABLINE;
    string[2] = KE_FILLER;
    add_to_നിവേശം_buf(string, 3);
    string[0] = nr;
    add_to_നിവേശം_buf_csi(string, 1);
    തിരികെ TRUE;
}

/*
 * Send a tabline menu event
 */
    ശൂന്യം
send_tabline_menu_event(പൂർണ്ണസംഖ്യ tabidx, പൂർണ്ണസംഖ്യ event)
{
    char_u	    string[3];

    // Don't put events in the input queue now.
    എങ്കിൽ (hold_gui_events)
	തിരികെ;

    // Cannot close the last tabpage.
    എങ്കിൽ (event == TABLINE_MENU_CLOSE && first_tabpage->tp_next == ശൂന്യ)
	തിരികെ;

    string[0] = CSI;
    string[1] = KS_TABMENU;
    string[2] = KE_FILLER;
    add_to_നിവേശം_buf(string, 3);
    string[0] = tabidx;
    string[1] = (char_u)(long)event;
    add_to_നിവേശം_buf_csi(string, 2);
}

#endif

/*
 * Scrollbar stuff:
 */

/*
 * Remove all scrollbars.  Used before switching to another tab page.
 */
    ശൂന്യം
gui_remove_scrollbars(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    i;
    win_T   *wp;

    വേണ്ടി (i = 0; i < 3; i++)
    {
	എങ്കിൽ (i == SBAR_BOTTOM)
	    gui_mch_enable_scrollbar(&gui.bottom_sbar, FALSE);
	അല്ലെങ്കിൽ
	{
	    FOR_ALL_WINDOWS(wp)
		gui_do_scrollbar(wp, i, FALSE);
	}
	curtab->tp_prev_which_scrollbars[i] = -1;
    }
}

    ശൂന്യം
gui_create_scrollbar(scrollbar_T *sb, പൂർണ്ണസംഖ്യ type, win_T *wp)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	sbar_ident = 0;

    sb->ident = sbar_ident++;	// No check for too big, but would it happen?
    sb->wp = wp;
    sb->type = type;
    sb->മൂല്യം = 0;
    sb->size = 1;
    sb->max = 1;
    sb->മുകൾ = 0;
    sb->height = 0;
    sb->width = 0;
    sb->status_height = 0;
    gui_mch_create_scrollbar(sb, (wp == ശൂന്യ) ? SBAR_HORIZ : SBAR_VERT);
}

/*
 * Find the scrollbar with the given index.
 */
    scrollbar_T *
gui_find_scrollbar(long ident)
{
    win_T	*wp;

    എങ്കിൽ (gui.bottom_sbar.ident == ident)
	തിരികെ &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	എങ്കിൽ (wp->w_scrollbars[SBAR_ഇടത്].ident == ident)
	    തിരികെ &wp->w_scrollbars[SBAR_ഇടത്];
	എങ്കിൽ (wp->w_scrollbars[SBAR_വലത്].ident == ident)
	    തിരികെ &wp->w_scrollbars[SBAR_വലത്];
    }
    തിരികെ ശൂന്യ;
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
    ശൂന്യം
gui_drag_scrollbar(scrollbar_T *sb, long മൂല്യം, പൂർണ്ണസംഖ്യ still_dragging)
{
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		sb_അക്കം;
#ifdef USE_ON_FLY_SCROLL
    colnr_T	old_leftcol = curwin->w_leftcol;
    linenr_T	old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    പൂർണ്ണസംഖ്യ		old_topfill = curwin->w_topfill;
# endif
#അല്ലെങ്കിൽ
    char_u	bytes[വലുപ്പം(long_u)];
    പൂർണ്ണസംഖ്യ		byte_എണ്ണം;
#endif

    എങ്കിൽ (sb == ശൂന്യ)
	തിരികെ;

    // Don't put events in the input queue now.
    എങ്കിൽ (hold_gui_events)
	തിരികെ;

    എങ്കിൽ (cmdwin_type != 0 && sb->wp != cmdwin_win)
	തിരികെ;

    എങ്കിൽ (still_dragging)
    {
	എങ്കിൽ (sb->wp == ശൂന്യ)
	    gui.dragged_sb = SBAR_BOTTOM;
	അല്ലെങ്കിൽ എങ്കിൽ (sb == &sb->wp->w_scrollbars[SBAR_ഇടത്])
	    gui.dragged_sb = SBAR_ഇടത്;
	അല്ലെങ്കിൽ
	    gui.dragged_sb = SBAR_വലത്;
	gui.dragged_wp = sb->wp;
    }
    അല്ലെങ്കിൽ
    {
	gui.dragged_sb = SBAR_NONE;
#ifdef FEAT_GUI_GTK
	// Keep the "dragged_wp" value until after the scrolling, for when the
	// mouse button is released.  GTK2 doesn't send the button-up event.
	gui.dragged_wp = ശൂന്യ;
	// WinScrolled event
	gui_focus_change(TRUE);
#endif
    }

    // Vertical sbar info is kept in the first sbar (the left one)
    എങ്കിൽ (sb->wp != ശൂന്യ)
	sb = &sb->wp->w_scrollbars[0];

    /*
     * Check validity of value
     */
    എങ്കിൽ (മൂല്യം < 0)
	മൂല്യം = 0;
#ifdef SCROLL_PAST_END
    അല്ലെങ്കിൽ എങ്കിൽ (മൂല്യം > sb->max)
	മൂല്യം = sb->max;
#അല്ലെങ്കിൽ
    എങ്കിൽ (മൂല്യം > sb->max - sb->size + 1)
	മൂല്യം = sb->max - sb->size + 1;
#endif

    sb->മൂല്യം = മൂല്യം;

#ifdef USE_ON_FLY_SCROLL
    // When not allowed to do the scrolling right now, return.
    // This also checked input_available(), but that causes the first click in
    // a scrollbar to be ignored when Vim doesn't have focus.
    എങ്കിൽ (dont_scroll)
	തിരികെ;
#endif
    // Disallow scrolling the current window when the completion popup menu is
    // visible.
    എങ്കിൽ ((sb->wp == ശൂന്യ || sb->wp == curwin) && pum_visible())
	തിരികെ;

#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (sb->wp == ശൂന്യ && curwin->w_p_rl)
    {
	മൂല്യം = sb->max + 1 - sb->size - മൂല്യം;
	എങ്കിൽ (മൂല്യം < 0)
	    മൂല്യം = 0;
    }
#endif

    എങ്കിൽ (sb->wp != ശൂന്യ)		// vertical scrollbar
    {
	sb_അക്കം = 0;
	വേണ്ടി (wp = firstwin; wp != sb->wp && wp != ശൂന്യ; wp = wp->w_next)
	    sb_അക്കം++;
	എങ്കിൽ (wp == ശൂന്യ)
	    തിരികെ;

#ifdef USE_ON_FLY_SCROLL
	current_scrollbar = sb_അക്കം;
	scrollbar_മൂല്യം = മൂല്യം;
	എങ്കിൽ (State & MODE_NORMAL)
	{
	    gui_do_scroll();
	    setcursor();
	}
	അല്ലെങ്കിൽ എങ്കിൽ (State & MODE_തിരുകുക)
	{
	    ins_scroll();
	    setcursor();
	}
	അല്ലെങ്കിൽ എങ്കിൽ (State & MODE_CMDLINE)
	{
	    എങ്കിൽ (സന്ദേശം_scrolled == 0)
	    {
		gui_do_scroll();
		redrawcmdline();
	    }
	}
# ifdef FEAT_FOLDING
	// Value may have been changed for closed fold.
	sb->മൂല്യം = sb->wp->w_topline - 1;
# endif

	// When dragging one scrollbar and there is another one at the other
	// side move the thumb of that one too.
	എങ്കിൽ (gui.which_scrollbars[SBAR_വലത്] && gui.which_scrollbars[SBAR_ഇടത്])
	    gui_mch_set_scrollbar_thumb(
		    &sb->wp->w_scrollbars[
			    sb == &sb->wp->w_scrollbars[SBAR_വലത്]
						    ? SBAR_ഇടത് : SBAR_വലത്],
		    sb->മൂല്യം, sb->size, sb->max);

#അല്ലെങ്കിൽ
	bytes[0] = CSI;
	bytes[1] = KS_VER_SCROLLBAR;
	bytes[2] = KE_FILLER;
	bytes[3] = (char_u)sb_അക്കം;
	byte_എണ്ണം = 4;
#endif
    }
    അല്ലെങ്കിൽ
    {
#ifdef USE_ON_FLY_SCROLL
	scrollbar_മൂല്യം = മൂല്യം;

	എങ്കിൽ (State & MODE_NORMAL)
	    do_mousescroll_horiz(scrollbar_മൂല്യം);
	അല്ലെങ്കിൽ എങ്കിൽ (State & MODE_തിരുകുക)
	    ins_horscroll();
	അല്ലെങ്കിൽ എങ്കിൽ (State & MODE_CMDLINE)
	{
	    എങ്കിൽ (സന്ദേശം_scrolled == 0)
	    {
		do_mousescroll_horiz(scrollbar_മൂല്യം);
		redrawcmdline();
	    }
	}
	എങ്കിൽ (old_leftcol != curwin->w_leftcol)
	{
	    updateWindow(curwin);   // update window, status and cmdline
	    setcursor();
	}
#അല്ലെങ്കിൽ
	bytes[0] = CSI;
	bytes[1] = KS_HOR_SCROLLBAR;
	bytes[2] = KE_FILLER;
	byte_എണ്ണം = 3;
#endif
    }

#ifdef USE_ON_FLY_SCROLL
    /*
     * synchronize other windows, as necessary according to 'scrollbind'
     */
    എങ്കിൽ (curwin->w_p_scb
	    && ((sb->wp == ശൂന്യ && curwin->w_leftcol != old_leftcol)
		|| (sb->wp == curwin && (curwin->w_topline != old_topline
# ifdef FEAT_DIFF
					   || curwin->w_topfill != old_topfill
# endif
			))))
    {
	do_check_scrollbind(TRUE);
	// need to update the window right here
	FOR_ALL_WINDOWS(wp)
	    എങ്കിൽ (wp->w_redr_type > 0)
		updateWindow(wp);
	setcursor();
    }
    out_flush_cursor(FALSE, TRUE);
#അല്ലെങ്കിൽ
    add_to_നിവേശം_buf(bytes, byte_എണ്ണം);
    add_long_to_buf((long_u)മൂല്യം, bytes);
    add_to_നിവേശം_buf_csi(bytes, വലുപ്പം(long_u));
#endif
}

/*
 * Scrollbar stuff:
 */

/*
 * Called when something in the window layout has changed.
 */
    ശൂന്യം
gui_may_update_scrollbars(ശൂന്യം)
{
    എങ്കിൽ (gui.in_use && starting == 0)
    {
	out_flush();
	gui_init_which_components(ശൂന്യ);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
}

    ശൂന്യം
gui_update_scrollbars(
    പൂർണ്ണസംഖ്യ		force)	    // Force all scrollbars to get updated
{
    win_T	*wp;
    scrollbar_T	*sb;
    long	val, size, max;		// need 32 bits here
    പൂർണ്ണസംഖ്യ		which_sb;
    പൂർണ്ണസംഖ്യ		h, y;
    സ്ഥിതം win_T *prev_curwin = ശൂന്യ;

    // Update the horizontal scrollbar
    gui_update_horiz_scrollbar(force);

#ifndef MSWIN
    // Return straight away if there is neither a left nor right scrollbar.
    // On MS-Windows this is required anyway for scrollwheel messages.
    എങ്കിൽ (!gui.which_scrollbars[SBAR_ഇടത്] && !gui.which_scrollbars[SBAR_വലത്])
	തിരികെ;
#endif

    /*
     * Don't want to update a scrollbar while we're dragging it.  But if we
     * have both a left and right scrollbar, and we drag one of them, we still
     * need to update the other one.
     */
    എങ്കിൽ (!force && (gui.dragged_sb == SBAR_ഇടത് || gui.dragged_sb == SBAR_വലത്)
	    && gui.which_scrollbars[SBAR_ഇടത്]
	    && gui.which_scrollbars[SBAR_വലത്])
    {
	/*
	 * If we have two scrollbars and one of them is being dragged, just
	 * copy the scrollbar position from the dragged one to the other one.
	 */
	which_sb = SBAR_ഇടത് + SBAR_വലത് - gui.dragged_sb;
	എങ്കിൽ (gui.dragged_wp != ശൂന്യ)
	    gui_mch_set_scrollbar_thumb(
		    &gui.dragged_wp->w_scrollbars[which_sb],
		    gui.dragged_wp->w_scrollbars[0].മൂല്യം,
		    gui.dragged_wp->w_scrollbars[0].size,
		    gui.dragged_wp->w_scrollbars[0].max);
    }

    // avoid that moving components around generates events
    ++hold_gui_events;

    FOR_ALL_WINDOWS(wp)
    {
	എങ്കിൽ (wp->w_buffer == ശൂന്യ)	// just in case
	    തുടരുക;
	// Skip a scrollbar that is being dragged.
	എങ്കിൽ (!force && (gui.dragged_sb == SBAR_ഇടത്
					     || gui.dragged_sb == SBAR_വലത്)
		&& gui.dragged_wp == wp)
	    തുടരുക;

#ifdef SCROLL_PAST_END
	max = wp->w_buffer->b_ml.ml_വരി_എണ്ണം - 1;
#അല്ലെങ്കിൽ
	max = wp->w_buffer->b_ml.ml_വരി_എണ്ണം + wp->w_height - 2;
#endif
	എങ്കിൽ (max < 0)			// empty buffer
	    max = 0;
	val = wp->w_topline - 1;
	size = wp->w_height;
#ifdef SCROLL_PAST_END
	എങ്കിൽ (val > max)			// just in case
	    val = max;
#അല്ലെങ്കിൽ
	എങ്കിൽ (size > max + 1)		// just in case
	    size = max + 1;
	എങ്കിൽ (val > max - size + 1)
	    val = max - size + 1;
#endif
	എങ്കിൽ (val < 0)			// minimal value is 0
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
	എങ്കിൽ (size < 1 || wp->w_botline - 2 > max)
	{
	    /*
	     * This can happen during changing files.  Just don't update the
	     * scrollbar for now.
	     */
	    sb->height = 0;	    // Force update next time
	    എങ്കിൽ (gui.which_scrollbars[SBAR_ഇടത്])
		gui_do_scrollbar(wp, SBAR_ഇടത്, FALSE);
	    എങ്കിൽ (gui.which_scrollbars[SBAR_വലത്])
		gui_do_scrollbar(wp, SBAR_വലത്, FALSE);
	    തുടരുക;
	}
	എങ്കിൽ (force || sb->height != wp->w_height
	    || sb->മുകൾ != wp->w_winrow
	    || sb->status_height != wp->w_status_height
	    || sb->width != wp->w_width
	    || prev_curwin != curwin)
	{
	    // Height, width or position of scrollbar has changed.  For
	    // vertical split: curwin changed.
	    sb->height = wp->w_height;
	    sb->മുകൾ = wp->w_winrow;
	    sb->status_height = wp->w_status_height;
	    sb->width = wp->w_width;

	    // Calculate height and position in pixels
	    h = (sb->height + sb->status_height) * gui.char_height;
	    y = sb->മുകൾ * gui.char_height + gui.border_offset;
#എങ്കിൽ defined(FEAT_MENU) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF) && !defined(FEAT_GUI_PHOTON)
	    എങ്കിൽ (gui.menu_is_active)
		y += gui.menu_height;
#endif

#എങ്കിൽ defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU))
	    എങ്കിൽ (vim_strchr(p_go, GO_TOOLBAR) != ശൂന്യ)
		y += gui.toolbar_height;
#endif

#എങ്കിൽ defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
	    എങ്കിൽ (gui_has_tabline())
		y += gui.tabline_height;
#endif

	    എങ്കിൽ (wp->w_winrow == 0)
	    {
		// Height of top scrollbar includes width of top border
		h += gui.border_offset;
		y -= gui.border_offset;
	    }
	    എങ്കിൽ (gui.which_scrollbars[SBAR_ഇടത്])
	    {
		gui_mch_set_scrollbar_pos(&wp->w_scrollbars[SBAR_ഇടത്],
					  gui.ഇടത്_sbar_x, y,
					  gui.scrollbar_width, h);
		gui_do_scrollbar(wp, SBAR_ഇടത്, TRUE);
	    }
	    എങ്കിൽ (gui.which_scrollbars[SBAR_വലത്])
	    {
		gui_mch_set_scrollbar_pos(&wp->w_scrollbars[SBAR_വലത്],
					  gui.വലത്_sbar_x, y,
					  gui.scrollbar_width, h);
		gui_do_scrollbar(wp, SBAR_വലത്, TRUE);
	    }
	}

	എങ്കിൽ (force || sb->മൂല്യം != val || sb->size != size || sb->max != max)
	{
	    // Thumb of scrollbar has moved
	    sb->മൂല്യം = val;
	    sb->size = size;
	    sb->max = max;
	    എങ്കിൽ (gui.which_scrollbars[SBAR_ഇടത്]
		    && (gui.dragged_sb != SBAR_ഇടത് || gui.dragged_wp != wp))
		gui_mch_set_scrollbar_thumb(&wp->w_scrollbars[SBAR_ഇടത്],
					    val, size, max);
	    എങ്കിൽ (gui.which_scrollbars[SBAR_വലത്]
		    && (gui.dragged_sb != SBAR_വലത് || gui.dragged_wp != wp))
		gui_mch_set_scrollbar_thumb(&wp->w_scrollbars[SBAR_വലത്],
					    val, size, max);
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
    സ്ഥിതം ശൂന്യം
gui_do_scrollbar(
    win_T	*wp,
    പൂർണ്ണസംഖ്യ		which,	    // SBAR_LEFT or SBAR_RIGHT
    പൂർണ്ണസംഖ്യ		enable)	    // TRUE to enable scrollbar
{
    പൂർണ്ണസംഖ്യ		midcol = curwin->w_wincol + curwin->w_width / 2;
    പൂർണ്ണസംഖ്യ		has_midcol = (wp->w_wincol <= midcol
				     && wp->w_wincol + wp->w_width >= midcol);

    // Only enable scrollbars that contain the middle column of the current
    // window.
    എങ്കിൽ (gui.which_scrollbars[SBAR_വലത്] != gui.which_scrollbars[SBAR_ഇടത്])
    {
	// Scrollbars only on one side.  Don't enable scrollbars that don't
	// contain the middle column of the current window.
	എങ്കിൽ (!has_midcol)
	    enable = FALSE;
    }
    അല്ലെങ്കിൽ
    {
	// Scrollbars on both sides.  Don't enable scrollbars that neither
	// contain the middle column of the current window nor are on the far
	// side.
	എങ്കിൽ (midcol > Columns / 2)
	{
	    എങ്കിൽ (which == SBAR_ഇടത് ? wp->w_wincol != 0 : !has_midcol)
		enable = FALSE;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (which == SBAR_വലത് ? wp->w_wincol + wp->w_width != Columns
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
    പൂർണ്ണസംഖ്യ
gui_do_scroll(ശൂന്യം)
{
    win_T	*wp, *save_wp;
    പൂർണ്ണസംഖ്യ		i;
    long	nlines;
    pos_T	old_cursor;
    linenr_T	old_topline;
#ifdef FEAT_DIFF
    പൂർണ്ണസംഖ്യ		old_topfill;
#endif

    വേണ്ടി (wp = firstwin, i = 0; i < current_scrollbar; wp = W_NEXT(wp), i++)
	എങ്കിൽ (wp == ശൂന്യ)
	    നിർത്തുക;
    എങ്കിൽ (wp == ശൂന്യ)
	// Couldn't find window
	തിരികെ FALSE;
    // don't redraw, LineOffset and similar are not valid!
    എങ്കിൽ (exmode_active)
	തിരികെ FALSE;

    /*
     * Compute number of lines to scroll.  If zero, nothing to do.
     */
    nlines = (long)scrollbar_മൂല്യം + 1 - (long)wp->w_topline;
    എങ്കിൽ (nlines == 0)
	തിരികെ FALSE;

    save_wp = curwin;
    old_topline = wp->w_topline;
#ifdef FEAT_DIFF
    old_topfill = wp->w_topfill;
#endif
    old_cursor = wp->w_cursor;
    curwin = wp;
    curbuf = wp->w_buffer;
    എങ്കിൽ (nlines < 0)
	scrolldown(-nlines, gui.dragged_wp == ശൂന്യ);
    അല്ലെങ്കിൽ
	scrollup(nlines, gui.dragged_wp == ശൂന്യ);
    // Reset dragged_wp after using it.  "dragged_sb" will have been reset for
    // the mouse-up event already, but we still want it to behave like when
    // dragging.  But not the next click in an arrow.
    എങ്കിൽ (gui.dragged_sb == SBAR_NONE)
	gui.dragged_wp = ശൂന്യ;

    എങ്കിൽ (old_topline != wp->w_topline
#ifdef FEAT_DIFF
	    || old_topfill != wp->w_topfill
#endif
	    )
    {
	എങ്കിൽ (get_scrolloff_മൂല്യം() != 0)
	{
	    cursor_correct();		// fix window for 'so'
	    update_topline();		// avoid up/down jump
	}
	എങ്കിൽ (old_cursor.lnum != wp->w_cursor.lnum)
	    coladvance(wp->w_curswant);
	wp->w_scbind_pos = wp->w_topline;
    }

    // Make sure wp->w_leftcol and wp->w_skipcol are correct.
    validate_cursor();

    curwin = save_wp;
    curbuf = save_wp->w_buffer;

    /*
     * Don't call updateWindow() when nothing has changed (it will overwrite
     * the status line!).
     *
     * Check for ScreenLines, because in ex-mode, we don't have a valid display.
     */
    എങ്കിൽ (Screen_വരികൾ != ശൂന്യ && (old_topline != wp->w_topline
	    || wp->w_redr_type != 0
#ifdef FEAT_DIFF
	    || old_topfill != wp->w_topfill
#endif
	    ))
    {
	പൂർണ്ണസംഖ്യ type = UPD_VALID;

	എങ്കിൽ (pum_visible())
	{
	    type = UPD_NOT_VALID;
	    wp->w_വരികൾ_valid = 0;
	}

	// Don't set must_redraw here, it may cause the popup menu to
	// disappear when losing focus after a scrollbar drag.
	എങ്കിൽ (wp->w_redr_type < type)
	    wp->w_redr_type = type;
	mch_disable_flush();
	updateWindow(wp);   // update window, status line, and cmdline
	mch_enable_flush();
    }

    // May need to redraw the popup menu.
    എങ്കിൽ (pum_visible())
	pum_redraw();

    തിരികെ (wp == curwin && !EQUAL_POS(curwin->w_cursor, old_cursor));
}

/*
 * Horizontal scrollbar stuff:
 */
    സ്ഥിതം ശൂന്യം
gui_update_horiz_scrollbar(പൂർണ്ണസംഖ്യ force)
{
    long	മൂല്യം, size, max;

    എങ്കിൽ (!gui.which_scrollbars[SBAR_BOTTOM])
	തിരികെ;

    എങ്കിൽ (!force && gui.dragged_sb == SBAR_BOTTOM)
	തിരികെ;

    എങ്കിൽ (!force && curwin->w_p_wrap && gui.prev_wrap)
	തിരികെ;

    /*
     * It is possible for the cursor to be invalid if we're in the middle of
     * something (like changing files).  If so, don't do anything for now.
     */
    എങ്കിൽ (curwin->w_cursor.lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
    {
	gui.bottom_sbar.മൂല്യം = -1;
	തിരികെ;
    }

    size = curwin->w_width;
    എങ്കിൽ (curwin->w_p_wrap)
    {
	മൂല്യം = 0;
#ifdef SCROLL_PAST_END
	max = 0;
#അല്ലെങ്കിൽ
	max = curwin->w_width - 1;
#endif
    }
    അല്ലെങ്കിൽ
    {
	മൂല്യം = curwin->w_leftcol;
	max = scroll_വരി_len(ui_find_longest_lnum());

	എങ്കിൽ (virtual_active())
	{
	    // May move the cursor even further to the right.
	    എങ്കിൽ (curwin->w_virtcol >= (colnr_T)max)
		max = curwin->w_virtcol;
	}

#ifndef SCROLL_PAST_END
	max += curwin->w_width - 1;
#endif
	// The line number isn't scrolled, thus there is less space when
	// 'number' or 'relativenumber' is set (also for 'foldcolumn').
	size -= curwin_col_off();
#ifndef SCROLL_PAST_END
	max -= curwin_col_off();
#endif
    }

#ifndef SCROLL_PAST_END
    എങ്കിൽ (മൂല്യം > max - size + 1)
	മൂല്യം = max - size + 1;	    // limit the value to allowable range
#endif

#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (curwin->w_p_rl)
    {
	മൂല്യം = max + 1 - size - മൂല്യം;
	എങ്കിൽ (മൂല്യം < 0)
	{
	    size += മൂല്യം;
	    മൂല്യം = 0;
	}
    }
#endif
    എങ്കിൽ (!force && മൂല്യം == gui.bottom_sbar.മൂല്യം && size == gui.bottom_sbar.size
						&& max == gui.bottom_sbar.max)
	തിരികെ;

    gui.bottom_sbar.മൂല്യം = മൂല്യം;
    gui.bottom_sbar.size = size;
    gui.bottom_sbar.max = max;
    gui.prev_wrap = curwin->w_p_wrap;

    gui_mch_set_scrollbar_thumb(&gui.bottom_sbar, മൂല്യം, size, max);
}

/*
 * Check that none of the colors are the same as the background color
 */
    ശൂന്യം
gui_check_colors(ശൂന്യം)
{
    എങ്കിൽ (gui.norm_pixel == gui.back_pixel || gui.norm_pixel == INVALCOLOR)
    {
	gui_set_bg_color((char_u *)"White");
	എങ്കിൽ (gui.norm_pixel == gui.back_pixel || gui.norm_pixel == INVALCOLOR)
	    gui_set_fg_color((char_u *)"Black");
    }
}

    സ്ഥിതം ശൂന്യം
gui_set_fg_color(char_u *name)
{
    gui.norm_pixel = gui_get_color(name);
    hl_set_fg_color_name(vim_strsave(name));
}

    സ്ഥിതം ശൂന്യം
gui_set_bg_color(char_u *name)
{
    gui.back_pixel = gui_get_color(name);
    hl_set_bg_color_name(vim_strsave(name));
}

/*
 * Allocate a color by name.
 * Returns INVALCOLOR and gives an error message when failed.
 */
    guicolor_T
gui_get_color(char_u *name)
{
    guicolor_T	t;

    എങ്കിൽ (*name == NUL)
	തിരികെ INVALCOLOR;
    t = gui_mch_get_color(name);

    പൂർണ്ണസംഖ്യ is_none = STRCMP(name, "none") == 0;
    എങ്കിൽ (t == INVALCOLOR
#എങ്കിൽ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
	    && (gui.in_use || is_none)
#endif
	    )
    {
	എങ്കിൽ (is_none)
	    emsg(_(e_cannot_use_color_none_did_you_mean_none));
	അല്ലെങ്കിൽ
	    semsg(_(e_cannot_allocate_color_str), name);
    }
    തിരികെ t;
}

/*
 * Return the grey value of a color (range 0-255).
 */
    പൂർണ്ണസംഖ്യ
gui_get_lightness(guicolor_T pixel)
{
    long_u	rgb = (long_u)gui_mch_get_rgb(pixel);

    തിരികെ  (പൂർണ്ണസംഖ്യ)(  (((rgb >> 16) & 0xff) * 299)
		   + (((rgb >> 8) & 0xff) * 587)
		   +  ((rgb	  & 0xff) * 114)) / 1000;
}

    char_u *
gui_bg_default(ശൂന്യം)
{
    എങ്കിൽ (gui_get_lightness(gui.back_pixel) < 127)
	തിരികെ (char_u *)"dark";
    തിരികെ (char_u *)"light";
}

/*
 * Option initializations that can only be done after opening the GUI window.
 */
    സ്ഥിതം ശൂന്യം
init_gui_options(ശൂന്യം)
{
    // Set the 'background' option according to the lightness of the
    // background color, unless the user has set it already.
    എങ്കിൽ (!option_was_set((char_u *)"bg") && STRCMP(p_bg, gui_bg_default()) != 0)
    {
	set_option_മൂല്യം_give_err((char_u *)"bg", 0L, gui_bg_default(), 0);
	highlight_changed();
    }
}

#എങ്കിൽ defined(FEAT_GUI_X11)
    ശൂന്യം
gui_new_scrollbar_colors(ശൂന്യം)
{
    win_T	*wp;

    // Nothing to do if GUI hasn't started yet.
    എങ്കിൽ (!gui.in_use)
	തിരികെ;

    FOR_ALL_WINDOWS(wp)
    {
	gui_mch_set_scrollbar_colors(&(wp->w_scrollbars[SBAR_ഇടത്]));
	gui_mch_set_scrollbar_colors(&(wp->w_scrollbars[SBAR_വലത്]));
    }
    gui_mch_set_scrollbar_colors(&gui.bottom_sbar);
}
#endif

/*
 * Call this when focus has changed.
 */
    ശൂന്യം
gui_focus_change(പൂർണ്ണസംഖ്യ in_focus)
{
/*
 * Skip this code to avoid drawing the cursor when debugging and switching
 * between the debugger window and gvim.
 */
#എങ്കിൽ 1
    gui.in_focus = in_focus;
    out_flush_cursor(TRUE, FALSE);

# ifdef FEAT_GUI_MSWIN
    gui_mch_set_titlebar_colors();
# endif

# ifdef FEAT_XIM
    xim_set_focus(in_focus);
# endif

    // Put events in the input queue only when allowed.
    // ui_focus_change() isn't called directly, because it invokes
    // autocommands and that must not happen asynchronously.
    എങ്കിൽ (!hold_gui_events
# എങ്കിൽ defined(FEAT_GUI_GTK) && defined(FEAT_GUI_DIALOG)
	    && gui.dialogs_active == 0
# endif
       )
    {
	char_u  bytes[3];

	bytes[0] = CSI;
	bytes[1] = KS_EXTRA;
	bytes[2] = in_focus ? (പൂർണ്ണസംഖ്യ)KE_FOCUSGAINED : (പൂർണ്ണസംഖ്യ)KE_FOCUSLOST;
	add_to_നിവേശം_buf(bytes, 3);
    }
#endif
}

/*
 * When mouse moved: apply 'mousefocus'.
 * Also updates the mouse pointer shape.
 */
    സ്ഥിതം ശൂന്യം
gui_mouse_focus(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y)
{
    win_T	*wp;
    char_u	st[8];

#ifdef FEAT_MOUSESHAPE
    // Get window pointer, and update mouse shape as well.
    wp = xy2win(x, y, IGNORE_POPUP);
#endif

    // Only handle this when 'mousefocus' set and ...
    എങ്കിൽ (p_mousef
	    && !hold_gui_events		// not holding events
	    && (State & (MODE_NORMAL | MODE_തിരുകുക))// Normal/Visual/Insert mode
	    && State != MODE_HITRETURN	// but not hit-return prompt
	    && സന്ദേശം_scrolled == 0	// no scrolled message
	    && !need_mouse_correct	// not moving the pointer
	    && gui.in_focus)		// gvim in focus
    {
	// Don't move the mouse when it's left or right of the Vim window
	എങ്കിൽ (x < 0 || x > Columns * gui.char_width)
	    തിരികെ;
#ifndef FEAT_MOUSESHAPE
	wp = xy2win(x, y, IGNORE_POPUP);
#endif
	എങ്കിൽ (wp == curwin || wp == ശൂന്യ)
	    തിരികെ;	// still in the same old window, or none at all

	// Ignore position in the tab pages line.
	എങ്കിൽ (Y_2_ROW(y) < tabline_height())
	    തിരികെ;

	/*
	 * Format a mouse click on status line input,
	 * ala gui_send_mouse_event(0, x, y, 0, 0);
	 * Trick: Use a column number -1, so that get_pseudo_mouse_code() will
	 * generate a K_LEFTMOUSE_NM key code.
	 */
	എങ്കിൽ (finish_op)
	{
	    // abort the current operator first
	    st[0] = ESC;
	    add_to_നിവേശം_buf(st, 1);
	}
	st[0] = CSI;
	st[1] = KS_MOUSE;
	st[2] = KE_FILLER;
	st[3] = (char_u)MOUSE_ഇടത്;
	fill_mouse_coord(st + 4,
		wp->w_wincol == 0 ? -1 : wp->w_wincol + MOUSE_COLOFF,
		wp->w_height + W_WINROW(wp));

	add_to_നിവേശം_buf(st, 8);
	st[3] = (char_u)MOUSE_RELEASE;
	add_to_നിവേശം_buf(st, 8);
#ifdef FEAT_GUI_GTK
	// Need to wake up the main loop
	എങ്കിൽ (gtk_main_level() > 0)
	    gtk_main_quit();
#endif
    }
}

/*
 * Called when the mouse moved (but not when dragging).
 */
    ശൂന്യം
gui_mouse_moved(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y)
{
    // Ignore this while still starting up.
    എങ്കിൽ (!gui.in_use || gui.starting)
	തിരികെ;

    // apply 'mousefocus' and pointer shape
    gui_mouse_focus(x, y);

    എങ്കിൽ (p_mousemev
#ifdef FEAT_PROP_POPUP
	|| popup_uses_mouse_move
#endif
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
gui_mouse_window(mouse_find_T popup)
{
    പൂർണ്ണസംഖ്യ		x, y;

    എങ്കിൽ (!(gui.in_use && (p_mousef || popup == FIND_POPUP)))
	തിരികെ ശൂന്യ;
    gui_mch_getmouse(&x, &y);

    // Only use the mouse when it's on the Vim window
    എങ്കിൽ (x >= 0 && x <= Columns * gui.char_width
	    && y >= 0 && Y_2_ROW(y) >= tabline_height())
	തിരികെ xy2win(x, y, popup);
    തിരികെ ശൂന്യ;
}

/*
 * Called when mouse should be moved to window with focus.
 */
    ശൂന്യം
gui_mouse_correct(ശൂന്യം)
{
    win_T	*wp = ശൂന്യ;

    need_mouse_correct = FALSE;

    wp = gui_mouse_window(IGNORE_POPUP);
    എങ്കിൽ (wp == curwin || wp == ശൂന്യ)
	തിരികെ;

    // If in other than current window
    validate_cline_row();
    gui_mch_setmouse((പൂർണ്ണസംഖ്യ)W_ENDCOL(curwin) * gui.char_width - 3,
	    (W_WINROW(curwin) + curwin->w_wrow) * gui.char_height
	    + (gui.char_height) / 2);
}

/*
 * Find window where the mouse pointer "x" / "y" coordinate is in.
 * As a side effect update the shape of the mouse pointer.
 */
    സ്ഥിതം win_T *
xy2win(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, mouse_find_T popup)
{
    പൂർണ്ണസംഖ്യ		row;
    പൂർണ്ണസംഖ്യ		col;
    win_T	*wp;

    row = Y_2_ROW(y);
    col = X_2_COL(x);

    എങ്കിൽ (row < 0 || col < 0)		// before first window
	തിരികെ ശൂന്യ;
    wp = mouse_find_win(&row, &col, popup);
    എങ്കിൽ (wp == ശൂന്യ)
    {
#ifdef FEAT_MOUSESHAPE
	update_mouseshape(-2);
#endif
	തിരികെ ശൂന്യ;
    }
#ifdef FEAT_MOUSESHAPE
    എങ്കിൽ (State == MODE_HITRETURN || State == MODE_ASKMORE)
    {
	എങ്കിൽ (Y_2_ROW(y) >= സന്ദേശം_row)
	    update_mouseshape(SHAPE_IDX_MOREL);
	അല്ലെങ്കിൽ
	    update_mouseshape(SHAPE_IDX_MORE);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (row >= wp->w_height + wp->w_status_height)	// below status line
	update_mouseshape(SHAPE_IDX_CLINE);
    അല്ലെങ്കിൽ എങ്കിൽ (!(State & MODE_CMDLINE) && wp->w_vsep_width > 0 && col == wp->w_width
	    && (!(row >= wp->w_height && row < wp->w_height
	    + wp->w_status_height) || !stl_connected(wp)) && സന്ദേശം_scrolled == 0)
	update_mouseshape(SHAPE_IDX_VSEP);
    അല്ലെങ്കിൽ എങ്കിൽ (!(State & MODE_CMDLINE) && wp->w_status_height > 0
	    && row >= wp->w_height && row < wp->w_height + wp->w_status_height
	    && സന്ദേശം_scrolled == 0)
	update_mouseshape(SHAPE_IDX_STATUS);
    അല്ലെങ്കിൽ
	update_mouseshape(-2);
#endif
    തിരികെ wp;
}

/*
 * ":gui" and ":gvim": Change from the terminal version to the GUI version.
 * File names may be given to redefine the args list.
 */
    ശൂന്യം
ex_gui(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    /*
     * Check for "-f" argument: foreground, don't fork.
     * Also don't fork when started with "gvim -f".
     * Do fork when using "gui -b".
     */
    എങ്കിൽ (arg[0] == '-'
	    && (arg[1] == 'f' || arg[1] == 'b')
	    && (arg[2] == NUL || VIM_ISWHITE(arg[2])))
    {
	gui.dofork = (arg[1] == 'b');
	eap->arg = skipwhite(eap->arg + 2);
    }
    എങ്കിൽ (!gui.in_use)
    {
#എങ്കിൽ defined(VIMDLL) && !defined(EXPERIMENTAL_GUI_CMD)
	എങ്കിൽ (!gui.starting)
	{
	    emsg(_(e_gui_cannot_be_used_not_enabled_at_compile_time));
	    തിരികെ;
	}
#endif
	// Clear the command.  Needed for when forking+exiting, to avoid part
	// of the argument ending up after the shell prompt.
	സന്ദേശം_clr_eos_force();
#ifdef GUI_MAY_SPAWN
	എങ്കിൽ (!ends_excmd2(eap->cmd, eap->arg))
	    gui_start(eap->arg);
	അല്ലെങ്കിൽ
#endif
	    gui_start(ശൂന്യ);
#ifdef FEAT_JOB_CHANNEL
	channel_gui_register_all();
#endif
    }
    എങ്കിൽ (!ends_excmd2(eap->cmd, eap->arg))
	ex_next(eap);
}

#എങ്കിൽ (defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	    || defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON) \
	    || defined(FEAT_GUI_HAIKU)) \
	    && defined(FEAT_TOOLBAR)
/*
 * This is shared between Haiku, Motif, and GTK.
 */

/*
 * Callback function for do_in_runtimepath().
 */
    സ്ഥിതം ശൂന്യം
gfp_setname(char_u *fname, ശൂന്യം *cookie)
{
    char_u	*gfp_buffer = cookie;

    എങ്കിൽ (STRLEN(fname) >= MAXPATHL)
	*gfp_buffer = NUL;
    അല്ലെങ്കിൽ
	STRCPY(gfp_buffer, fname);
}

/*
 * Find the path of bitmap "name" with extension "ext" in 'runtimepath'.
 * Return FAIL for failure and OK if buffer[MAXPATHL] contains the result.
 */
    പൂർണ്ണസംഖ്യ
gui_find_bitmap(char_u *name, char_u *buffer, അക്ഷരം *ext)
{
    എങ്കിൽ (STRLEN(name) > MAXPATHL - 14)
	തിരികെ FAIL;
    vim_snprintf((അക്ഷരം *)buffer, MAXPATHL, "bitmaps/%s.%s", name, ext);
    എങ്കിൽ (do_in_runtimepath(buffer, 0, gfp_setname, buffer) == FAIL
							    || *buffer == NUL)
	തിരികെ FAIL;
    തിരികെ OK;
}

# എങ്കിൽ !defined(FEAT_GUI_GTK)
/*
 * Given the name of the "icon=" argument, try finding the bitmap file for the
 * icon.  If it is an absolute path name, use it as it is.  Otherwise append
 * "ext" and search for it in 'runtimepath'.
 * The result is put in "buffer[MAXPATHL]".  If something fails "buffer"
 * contains "name".
 */
    ശൂന്യം
gui_find_iconfile(char_u *name, char_u *buffer, അക്ഷരം *ext)
{
    char_u	buf[MAXPATHL + 1];

    expand_env(name, buffer, MAXPATHL);
    എങ്കിൽ (!mch_isFullName(buffer) && gui_find_bitmap(buffer, buf, ext) == OK)
	STRCPY(buffer, buf);
}
# endif
#endif

#എങ്കിൽ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)|| defined(FEAT_GUI_HAIKU)
    ശൂന്യം
display_errors(ശൂന്യം)
{
    char_u	*p;

    എങ്കിൽ (isatty(2))
    {
	fflush(stderr);
	തിരികെ;
    }

    എങ്കിൽ (error_ga.ga_data == ശൂന്യ)
	തിരികെ;

    // avoid putting up a message box with blanks only
    വേണ്ടി (p = (char_u *)error_ga.ga_data; *p != NUL; ++p)
	എങ്കിൽ (!SAFE_isspace(*p))
	{
	    // Truncate a very long message, it will go off-screen.
	    എങ്കിൽ (STRLEN(p) > 2000)
		STRCPY(p + 2000 - 14, "...(truncated)");
	    (ശൂന്യം)do_dialog(VIM_ERROR, (char_u *)_("Error"),
		    p, (char_u *)_("&Ok"), 1, ശൂന്യ, FALSE);
	    നിർത്തുക;
	}
    ga_clear(&error_ga);
}
#endif

#എങ്കിൽ defined(NO_CONSOLE_നിവേശം)
/*
 * Return TRUE if still starting up and there is no place to enter text.
 * For GTK and X11 we check if stderr is not a tty, which means we were
 * (probably) started from the desktop.  Also check stdin, "vim >& file" does
 * allow typing on stdin.
 */
    പൂർണ്ണസംഖ്യ
no_console_നിവേശം(ശൂന്യം)
{
    തിരികെ ((!gui.in_use || gui.starting)
# ifndef NO_CONSOLE
	    && !isatty(0) && !isatty(2)
# endif
	    );
}
#endif

#എങ്കിൽ defined(FIND_REPLACE_DIALOG) \
	|| defined(NEED_GUI_UPDATE_SCREEN)
/*
 * Update the current window and the screen.
 */
    ശൂന്യം
gui_update_screen(ശൂന്യം)
{
# ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_വരി = 0;
    linenr_T	conceal_new_cursor_വരി = 0;
    പൂർണ്ണസംഖ്യ		conceal_update_വരികൾ = FALSE;
# endif

    update_topline();
    validate_cursor();

    // Trigger CursorMoved if the cursor moved.
    എങ്കിൽ (!finish_op && (has_cursormoved()
# ifdef FEAT_PROP_POPUP
		|| popup_visible
# endif
# ifdef FEAT_CONCEAL
		|| curwin->w_p_cole > 0
# endif
		) && !EQUAL_POS(last_cursormoved, curwin->w_cursor))
    {
	എങ്കിൽ (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
# ifdef FEAT_PROP_POPUP
	എങ്കിൽ (popup_visible)
	    popup_check_cursor_pos();
# endif
# ifdef FEAT_CONCEAL
	എങ്കിൽ (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_വരി = last_cursormoved.lnum;
	    conceal_new_cursor_വരി = curwin->w_cursor.lnum;
	    conceal_update_വരികൾ = TRUE;
	}
# endif
	last_cursormoved = curwin->w_cursor;
    }

    എങ്കിൽ (!finish_op)
	may_trigger_win_scrolled_resized();

# ifdef FEAT_CONCEAL
    എങ്കിൽ (conceal_update_വരികൾ
	    && (conceal_old_cursor_വരി != conceal_new_cursor_വരി
		|| conceal_cursor_വരി(curwin)
		|| need_cursor_വരി_redraw))
    {
	എങ്കിൽ (conceal_old_cursor_വരി != conceal_new_cursor_വരി)
	    redrawWinline(curwin, conceal_old_cursor_വരി);
	redrawWinline(curwin, conceal_new_cursor_വരി);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_വരി_redraw = FALSE;
    }
# endif
    update_screen(0);	// may need to update the screen
    setcursor();
    out_flush_cursor(TRUE, FALSE);
}
#endif

#എങ്കിൽ defined(FIND_REPLACE_DIALOG)
/*
 * Get the text to use in a find/replace dialog.  Uses the last search pattern
 * if the argument is empty.
 * Returns an allocated string.
 */
    char_u *
get_find_dialog_text(
    char_u	*arg,
    പൂർണ്ണസംഖ്യ		*wwordp,	// return: TRUE if \< \> found
    പൂർണ്ണസംഖ്യ		*mcasep)	// return: TRUE if \C found
{
    char_u	*text;

    എങ്കിൽ (*arg == NUL)
	text = last_തിരയൽ_pat();
    അല്ലെങ്കിൽ
	text = arg;
    എങ്കിൽ (text != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ len = (പൂർണ്ണസംഖ്യ)STRLEN(text);

	text = vim_strnsave(text, len);
	എങ്കിൽ (text != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ i;

	    // Remove "\V"
	    എങ്കിൽ (len >= 2 && STRNCMP(text, "\\V", 2) == 0)
	    {
		mch_memmove(text, text + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\c" and "\C" and remove.
	    എങ്കിൽ (len >= 2 && *text == '\\' && (text[1] == 'c' || text[1] == 'C'))
	    {
		*mcasep = (text[1] == 'C');
		mch_memmove(text, text + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\<text\>" and remove.
	    എങ്കിൽ (len >= 4
		    && STRNCMP(text, "\\<", 2) == 0
		    && STRNCMP(text + len - 2, "\\>", 2) == 0)
	    {
		*wwordp = TRUE;
		mch_memmove(text, text + 2, (size_t)(len - 4));
		text[len - 4] = NUL;
	    }

	    // Recognize "\/" or "\?" and remove.
	    വേണ്ടി (i = 0; i + 1 < len; ++i)
		എങ്കിൽ (text[i] == '\\' && (text[i + 1] == '/'
						       || text[i + 1] == '?'))
		{
		    mch_memmove(text + i, text + i + 1, (size_t)(len - i));
		    --len;
		}
	}
    }
    തിരികെ text;
}

/*
 * Handle the press of a button in the find-replace dialog.
 * Return TRUE when something was added to the input buffer.
 */
    പൂർണ്ണസംഖ്യ
gui_do_findrepl(
    പൂർണ്ണസംഖ്യ		flags,		// one of FRD_REPLACE, FRD_FINDNEXT, etc.
    char_u	*find_text,
    char_u	*repl_text,
    പൂർണ്ണസംഖ്യ		down)		// Search downwards.
{
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		type = (flags & FRD_TYPE_MASK);
    char_u	*p;
    regmatch_T	regmatch;
    പൂർണ്ണസംഖ്യ		save_did_emsg = did_emsg;
    സ്ഥിതം പൂർണ്ണസംഖ്യ  busy = FALSE;

    // When the screen is being updated we should not change buffers and
    // windows structures, it may cause freed memory to be used.  Also don't
    // do this recursively (pressing "Find" quickly several times).
    എങ്കിൽ (updating_screen || busy)
	തിരികെ FALSE;

    // refuse replace when text cannot be changed
    എങ്കിൽ ((type == FRD_REPLACE || type == FRD_REPLACEALL) && text_locked())
	തിരികെ FALSE;

    busy = TRUE;

    ga_init2(&ga, 1, 100);
    എങ്കിൽ (type == FRD_REPLACEALL)
	GA_CONCAT_LITERAL(&ga, "%s/");

    GA_CONCAT_LITERAL(&ga, "\\V");
    എങ്കിൽ (flags & FRD_MATCH_CASE)
	GA_CONCAT_LITERAL(&ga, "\\C");
    അല്ലെങ്കിൽ
	GA_CONCAT_LITERAL(&ga, "\\c");
    എങ്കിൽ (flags & FRD_WHOLE_WORD)
	GA_CONCAT_LITERAL(&ga, "\\<");
    // escape slash and backslash
    p = vim_strsave_escaped(find_text, (char_u *)"/\\");
    എങ്കിൽ (p != ശൂന്യ)
	ga_concat(&ga, p);
    vim_free(p);
    എങ്കിൽ (flags & FRD_WHOLE_WORD)
	GA_CONCAT_LITERAL(&ga, "\\>");

    എങ്കിൽ (type == FRD_REPLACEALL)
    {
	GA_CONCAT_LITERAL(&ga, "/");
	// Escape slash and backslash.
	// Also escape tilde and ampersand if 'magic' is set.
	p = vim_strsave_escaped(repl_text,
				p_magic ? (char_u *)"/\\~&" : (char_u *)"/\\");
	എങ്കിൽ (p != ശൂന്യ)
	    ga_concat(&ga, p);
	vim_free(p);
	GA_CONCAT_LITERAL(&ga, "/g");
    }
    ga_append(&ga, NUL);

    എങ്കിൽ (type == FRD_REPLACE)
    {
	// Do the replacement when the text at the cursor matches.  Thus no
	// replacement is done if the cursor was moved!
	regmatch.regprog = vim_regcomp(ga.ga_data, RE_MAGIC + RE_STRING);
	regmatch.rm_ic = 0;
	എങ്കിൽ (regmatch.regprog != ശൂന്യ)
	{
	    p = ml_get_cursor();
	    എങ്കിൽ (vim_regexec_nl(&regmatch, p, (colnr_T)0)
						   && regmatch.startp[0] == p)
	    {
		// Clear the command line to remove any old "No match"
		// error.
		സന്ദേശം_end_prompt();

		എങ്കിൽ (u_save_cursor() == OK)
		{
		    // A button was pressed thus undo should be synced.
		    u_sync(FALSE);

		    del_bytes((long)(regmatch.endp[0] - regmatch.startp[0]),
								FALSE, FALSE);
		    ins_str(repl_text, STRLEN(repl_text));
		}
	    }
	    അല്ലെങ്കിൽ
		സന്ദേശം(_("No match at cursor, finding next"));
	    vim_regfree(regmatch.regprog);
	}
    }

    എങ്കിൽ (type == FRD_REPLACEALL)
    {
	// A button was pressed, thus undo should be synced.
	u_sync(FALSE);
	do_cmdline_cmd(ga.ga_data);
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ searchflags = തിരയൽ_സന്ദേശം + തിരയൽ_MARK;

	// Search for the next match.
	// Don't skip text under cursor for single replace.
	എങ്കിൽ (type == FRD_REPLACE)
	    searchflags += തിരയൽ_START;
	i = സന്ദേശം_scroll;
	എങ്കിൽ (down)
	{
	    (ശൂന്യം)do_തിരയൽ(ശൂന്യ, '/', '/', ga.ga_data, STRLEN(ga.ga_data), 1L, searchflags, ശൂന്യ);
	}
	അല്ലെങ്കിൽ
	{
	    // We need to escape '?' if and only if we are searching in the up
	    // direction
	    p = vim_strsave_escaped(ga.ga_data, (char_u *)"?");
	    എങ്കിൽ (p != ശൂന്യ)
		(ശൂന്യം)do_തിരയൽ(ശൂന്യ, '?', '?', p, STRLEN(p), 1L, searchflags, ശൂന്യ);
	    vim_free(p);
	}

	സന്ദേശം_scroll = i;	    // don't let an error message set msg_scroll
    }

    // Don't want to pass did_emsg to other code, it may cause disabling
    // syntax HL if we were busy redrawing.
    did_emsg = save_did_emsg;

    എങ്കിൽ (State & (MODE_NORMAL | MODE_തിരുകുക))
    {
	gui_update_screen();		// update the screen
	സന്ദേശം_didout = 0;			// overwrite any message
	need_wait_return = FALSE;	// don't wait for return
    }

    vim_free(ga.ga_data);
    busy = FALSE;
    തിരികെ (ga.ga_len > 0);
}

#endif

#എങ്കിൽ defined(HAVE_DROP_FILE)
/*
 * Jump to the window at specified point (x, y).
 */
    സ്ഥിതം ശൂന്യം
gui_wingoto_xy(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y)
{
    പൂർണ്ണസംഖ്യ		row = Y_2_ROW(y);
    പൂർണ്ണസംഖ്യ		col = X_2_COL(x);
    win_T	*wp;

    എങ്കിൽ (row < 0 || col < 0)
	തിരികെ;

    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    എങ്കിൽ (wp != ശൂന്യ && wp != curwin)
	win_goto(wp);
}

/*
 * Function passed to handle_drop() for the actions to be done after the
 * argument list has been updated.
 */
    സ്ഥിതം ശൂന്യം
drop_callback(ശൂന്യം *cookie)
{
    char_u	*p = cookie;
    പൂർണ്ണസംഖ്യ		do_shorten = FALSE;

    // If Shift held down, change to first file's directory.  If the first
    // item is a directory, change to that directory (and let the explorer
    // plugin show the contents).
    എങ്കിൽ (p != ശൂന്യ)
    {
	എങ്കിൽ (mch_isdir(p))
	{
	    എങ്കിൽ (mch_chdir((അക്ഷരം *)p) == 0)
		do_shorten = TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (vim_chdirfile(p, "drop") == OK)
	    do_shorten = TRUE;
	vim_free(p);
	എങ്കിൽ (do_shorten)
	{
	    shorten_fnames(TRUE);
	    last_chdir_reason = "drop";
	}
    }

    // Update the screen display
    update_screen(UPD_NOT_VALID);
# ifdef FEAT_MENU
    gui_update_menus(0);
# endif
    maketitle();
    setcursor();
    out_flush_cursor(FALSE, FALSE);
}

/*
 * Process file drop.  Mouse cursor position, key modifiers, name of files
 * and count of files are given.  Argument "fnames[count]" has full pathnames
 * of dropped files, they will be freed in this function, and caller can't use
 * fnames after call this function.
 */
    ശൂന്യം
gui_handle_drop(
    പൂർണ്ണസംഖ്യ		x UNUSED,
    പൂർണ്ണസംഖ്യ		y UNUSED,
    int_u	modifiers,
    char_u	**fnames,
    പൂർണ്ണസംഖ്യ		എണ്ണം)
{
    പൂർണ്ണസംഖ്യ		i;
    char_u	*p;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	entered = FALSE;

    /*
     * This function is called by event handlers.  Just in case we get a
     * second event before the first one is handled, ignore the second one.
     * Not sure if this can ever happen, just in case.
     */
    എങ്കിൽ (entered)
	തിരികെ;
    entered = TRUE;

    /*
     * When the cursor is at the command line, add the file names to the
     * command line, don't edit the files.
     */
    എങ്കിൽ (State & MODE_CMDLINE)
    {
	shorten_filenames(fnames, എണ്ണം);
	വേണ്ടി (i = 0; i < എണ്ണം; ++i)
	{
	    എങ്കിൽ (fnames[i] != ശൂന്യ)
	    {
		എങ്കിൽ (i > 0)
		    add_to_നിവേശം_buf((char_u*)" ", 1);

		// We don't know what command is used thus we can't be sure
		// about which characters need to be escaped.  Only escape the
		// most common ones.
# ifdef BACKSLASH_IN_FILENAME
		p = vim_strsave_escaped(fnames[i], (char_u *)" \t\"|");
# അല്ലെങ്കിൽ
		p = vim_strsave_escaped(fnames[i], (char_u *)"\\ \t\"|");
# endif
		എങ്കിൽ (p != ശൂന്യ)
		    add_to_നിവേശം_buf_csi(p, (പൂർണ്ണസംഖ്യ)STRLEN(p));
		vim_free(p);
		vim_free(fnames[i]);
	    }
	}
	vim_free(fnames);
    }
    അല്ലെങ്കിൽ
    {
	// Go to the window under mouse cursor, then shorten given "fnames" by
	// current window, because a window can have local current dir.
	gui_wingoto_xy(x, y);
	shorten_filenames(fnames, എണ്ണം);

	// If Shift held down, remember the first item.
	എങ്കിൽ ((modifiers & MOUSE_SHIFT) != 0)
	    p = vim_strsave(fnames[0]);
	അല്ലെങ്കിൽ
	    p = ശൂന്യ;

	// Handle the drop, :edit or :split to get to the file.  This also
	// frees fnames[].  Skip this if there is only one item, it's a
	// directory and Shift is held down.
	എങ്കിൽ (എണ്ണം == 1 && (modifiers & MOUSE_SHIFT) != 0
						     && mch_isdir(fnames[0]))
	{
	    vim_free(fnames[0]);
	    vim_free(fnames);
	    vim_free(p);
	}
	അല്ലെങ്കിൽ
	    handle_drop(എണ്ണം, fnames, (modifiers & MOUSE_CTRL) != 0,
						     drop_callback, (ശൂന്യം *)p);
    }

    entered = FALSE;
}
#endif

/*
 * Check if "key" is to interrupt us.  Handles a key that has not had modifiers
 * applied yet.
 * Return the key with modifiers applied if so, NUL if not.
 */
    പൂർണ്ണസംഖ്യ
check_for_interrupt(പൂർണ്ണസംഖ്യ താക്കോൽ, പൂർണ്ണസംഖ്യ modifiers_arg)
{
    പൂർണ്ണസംഖ്യ modifiers = modifiers_arg;
    പൂർണ്ണസംഖ്യ c = merge_modify_Other_താക്കോലുകൾ(താക്കോൽ, &modifiers);

    എങ്കിൽ ((c == Ctrl_C && ctrl_c_interrupts)
#ifdef UNIX
	    || (intr_char != Ctrl_C && c == intr_char)
#endif
	    )
    {
	got_int = TRUE;
	തിരികെ c;
    }
    തിരികെ NUL;
}

/*
 * If the "--gui-log-file fname" argument is given write the dialog title and
 * message to a file and return TRUE.  Otherwise return FALSE.
 * When there is any problem opening the file or writing to the file this is
 * ignored, showing the dialog might get the test to get stuck.
 */
    പൂർണ്ണസംഖ്യ
gui_dialog_log(char_u *title, char_u *message)
{
    char_u  *fname = get_gui_dialog_file();
    FILE    *fd;

    എങ്കിൽ (fname == ശൂന്യ)
	തിരികെ FALSE;

    fd = mch_fopen((അക്ഷരം *)fname, "a");
    എങ്കിൽ (fd != ശൂന്യ)
    {
	fprintf(fd, "%s: %s\n", title, message);
	fclose(fd);
    }
    തിരികെ TRUE;
}
