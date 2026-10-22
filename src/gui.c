/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#अन्तर्भाव "vim.h"

// Structure containing all the GUI information
gui_T gui;

#जर defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
# define USE_SET_GUIFONTWIDE
स्थिर शून्य set_guifontwide(char_u *font_name);
#endif
स्थिर शून्य gui_check_pos(शून्य);
स्थिर शून्य gui_reset_scroll_region(शून्य);
स्थिर शून्य gui_outstr(char_u *, पूर्णांक);
#ifndef USE_GTK4
स्थिर पूर्णांक gui_screenchar(पूर्णांक off, पूर्णांक flags, guicolor_T fg, guicolor_T bg, पूर्णांक back);
#endif
स्थिर पूर्णांक gui_outstr_nowrap(char_u *s, पूर्णांक len, पूर्णांक flags, guicolor_T fg, guicolor_T bg, पूर्णांक back);
स्थिर शून्य gui_delete_ओळी(पूर्णांक row, पूर्णांक मोजणी);
स्थिर शून्य gui_घाला_ओळी(पूर्णांक row, पूर्णांक मोजणी);
स्थिर पूर्णांक gui_xy2colrow(पूर्णांक x, पूर्णांक y, पूर्णांक *colp);
#जर defined(FEAT_GUI_TABLINE)
स्थिर पूर्णांक gui_has_tabline(शून्य);
#endif
स्थिर शून्य gui_do_scrollbar(win_T *wp, पूर्णांक which, पूर्णांक enable);
स्थिर शून्य gui_update_horiz_scrollbar(पूर्णांक);
स्थिर शून्य gui_set_fg_color(char_u *name);
स्थिर शून्य gui_set_bg_color(char_u *name);
स्थिर शून्य init_gui_options(शून्य);
स्थिर win_T *xy2win(पूर्णांक x, पूर्णांक y, mouse_find_T popup);

#ifdef GUI_MAY_FORK
स्थिर शून्य gui_do_fork(शून्य);

स्थिर पूर्णांक gui_read_child_pipe(पूर्णांक fd);

// Return values for gui_read_child_pipe
गणना {
    GUI_CHILD_IO_ERROR,
    GUI_CHILD_OK,
    GUI_CHILD_FAILED
};
#endif

स्थिर शून्य gui_attempt_start(शून्य);

स्थिर पूर्णांक can_update_cursor = TRUE; // can display the cursor
स्थिर पूर्णांक disable_flush = 0;	// If > 0, gui_mch_flush() is disabled.

/*
 * gui_start -- Called when user wants to start the GUI.
 *
 * Careful: This function can be called recursively when there is a ":gui"
 * command in the .gvimrc file.  Only the first call should fork, not the
 * recursive call.
 */
    शून्य
gui_start(char_u *arg UNUSED)
{
    char_u	*old_term;
#ifdef GUI_MAY_FORK
    स्थिर पूर्णांक	recursive = 0;
#endif
#जर defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
    अक्षर	*संदेश = रिक्त;
#endif

    old_term = vim_strsave(T_NAME);

    settmode(TMODE_COOK);		// stop RAW mode
    जर (full_screen)
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
    जर (gui.dofork && !vim_strchr(p_go, GO_FORG) && recursive <= 1
# ifdef FEAT_JOB_CHANNEL
	    && !job_any_running()
# endif
	    )
    {
	gui_do_fork();
    }
    अन्यथा
#endif
#ifdef GUI_MAY_SPAWN
    जर (gui.dospawn
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
	संदेश =
# endif
	    gui_mch_do_spawn(arg);
    }
    अन्यथा
#endif
    {
#ifdef FEAT_GUI_GTK
	// If there is 'f' in 'guioptions' and specify -g argument,
	// gui_mch_init_check() was not called yet.
	जर (gui_mch_init_check() != OK)
	    getout_preserve_modified(1);
#endif
	gui_attempt_start();
    }

    जर (!gui.in_use)			// failed to start GUI
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
#जर defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
	जर (संदेश != रिक्त)
	    emsg(संदेश);
#endif
    }
    अन्यथा
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

#जर defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    // Enable fullscreen mode
    जर (vim_strchr(p_go, GO_FULLSCREEN) != रिक्त)
       gui_mch_set_fullscreen(TRUE);
#endif

    vim_free(old_term);

    // If the GUI started successfully, trigger the GUIEnter event, otherwise
    // the GUIFailed event.
    gui_mch_update();
    apply_autocmds(gui.in_use ? EVENT_GUIENTER : EVENT_GUIFAILED,
						   रिक्त, रिक्त, FALSE, curbuf);
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
    स्थिर शून्य
gui_attempt_start(शून्य)
{
    स्थिर पूर्णांक recursive = 0;

    ++recursive;
    gui.starting = true;

#ifdef FEAT_GUI_GTK
    gui.event_time = GDK_CURRENT_TIME;
#endif

    termcapinit((char_u *)"builtin_gui");
    gui.starting = recursive - 1;

#जर defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
    जर (gui.in_use)
    {
# ifdef FEAT_EVAL
	Window	x11_window;
	Display	*x11_display;

	जर (gui_get_x11_windis(&x11_window, &x11_display) == OK)
	    set_vim_var_nr(VV_WINDOWID, (दीर्घ)x11_window);
# endif
	// Display error messages in a dialog now.
	display_errors();
    }
#endif
    --recursive;
}

#ifdef GUI_MAY_FORK

// for waitpid()
# जर defined(HAVE_SYS_WAIT_H)
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
    स्थिर शून्य
gui_do_fork(शून्य)
{
    पूर्णांक		pipefd[2];	// pipe between parent and child
    पूर्णांक		pipe_error;
    पूर्णांक		status;
    पूर्णांक		exit_status;
    pid_t	pid = -1;

# जर defined(FEAT_RELTIME) && defined(PROF_NSEC)
    // a timer is not carried forward
    delete_timer();
# endif

    // Setup a pipe between the child and the parent, so that the parent
    // knows when the child has done the setsid() call and is allowed to
    // exit.
    pipe_error = (pipe(pipefd) < 0);
    pid = विभाजन();
    जर (pid < 0)	    // Fork error
    {
	emsg(_(e_failed_to_create_new_process_for_GUI));
	परत;
    }
    अन्यथा जर (pid > 0)	    // Parent
    {
	// Give the child some time to do the setsid(), otherwise the
	// exit() may kill the child too (when starting gvim from inside a
	// gvim).
	जर (!pipe_error)
	{
	    // The read returns when the child closes the pipe (or when
	    // the child dies for some reason).
	    बंद(pipefd[1]);
	    status = gui_read_child_pipe(pipefd[0]);
	    जर (status == GUI_CHILD_FAILED)
	    {
		// The child failed to start the GUI, so the caller must
		// continue. There may be more error information written
		// to stderr by the child.
		waitpid(pid, &exit_status, 0);
		emsg(_(e_the_child_process_failed_to_start_GUI));
		परत;
	    }
	    अन्यथा जर (status == GUI_CHILD_IO_ERROR)
	    {
		pipe_error = TRUE;
	    }
	    // else GUI_CHILD_OK: parent exit
	}

	जर (pipe_error)
	    ui_delay(301L, TRUE);

	// When swapping screens we may need to go to the next line, e.g.,
	// after a hit-enter prompt and using ":gui".
	जर (newline_on_exit)
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
    जर (gui_mch_init_check() != OK)
	getout_preserve_modified(1);
# endif

# जर defined(HAVE_SETSID) || defined(HAVE_SETPGID)
    /*
     * Change our process group.  On some systems/shells a CTRL-C in the
     * shell where Vim was started would otherwise kill gvim!
     */
#  जर defined(HAVE_SETSID)
    (शून्य)setsid();
#  अन्यथा
    (शून्य)setpgid(0, 0);
#  endif
# endif
    जर (!pipe_error)
	बंद(pipefd[0]);

# जर defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
    // Tell the session manager our new PID
    gui_mch_forked();
# endif

    // Try to start the GUI
    gui_attempt_start();

    // Notify the parent
    जर (!pipe_error)
    {
	जर (gui.in_use)
	    write_eintr(pipefd[1], "ok", 3);
	अन्यथा
	    write_eintr(pipefd[1], "fail", 5);
	बंद(pipefd[1]);
    }

    // If we failed to start the GUI, exit now.
    जर (!gui.in_use)
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
    स्थिर पूर्णांक
gui_read_child_pipe(पूर्णांक fd)
{
    दीर्घ	bytes_read;
# define READ_BUFFER_SIZE 10
    अक्षर	buffer[READ_BUFFER_SIZE];

    bytes_read = read_eintr(fd, buffer, READ_BUFFER_SIZE - 1);
# undef READ_BUFFER_SIZE
    बंद(fd);
    जर (bytes_read < 0)
	परत GUI_CHILD_IO_ERROR;
    buffer[bytes_read] = NUL;
    जर (STRCMP(buffer, "ok") == 0)
	परत GUI_CHILD_OK;
    परत GUI_CHILD_FAILED;
}

#endif // GUI_MAY_FORK

/*
 * Call this when vim starts up, whether or not the GUI is started
 */
    शून्य
gui_prepare(पूर्णांक *argc, अक्षर **argv)
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
    पूर्णांक
gui_init_check(शून्य)
{
    स्थिर पूर्णांक result = MAYBE;

    जर (result != MAYBE)
    {
	जर (result == FAIL)
	    emsg(_(e_cannot_start_the_GUI));
	परत result;
    }

    gui.shell_created = false;
    gui.dying = false;
    gui.in_focus = true;		// so the guicursor setting works
    gui.dragged_sb = SBAR_NONE;
    gui.dragged_wp = रिक्त;
    gui.pointer_hidden = false;
    gui.col = 0;
    gui.row = 0;
    gui.अंक_cols = Columns;
    gui.अंक_rows = Rows;

    gui.cursor_is_valid = false;
    gui.scroll_region_शीर्ष = 0;
    gui.scroll_region_bot = Rows - 1;
    gui.scroll_region_डावा = 0;
    gui.scroll_region_उजवा = Columns - 1;
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
#  अन्यथा
    gui.menu_font = NOFONT;
#  endif
# endif
    gui.menu_is_active = true;	    // default: include menu
# ifndef FEAT_GUI_GTK
    gui.menu_height = MENU_DEFAULT_HEIGHT;
    gui.menu_width = 0;
# endif
#endif
#जर defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    gui.toolbar_height = 0;
#endif
#ifdef FEAT_BEVAL_TIP
    gui.tooltip_fontset = NOFONTSET;
#endif

    gui.scrollbar_width = gui.scrollbar_height = SB_DEFAULT_WIDTH;
    gui.prev_wrap = -1;

#जर defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // Note: gui_set_ligatures() might already have been called e.g. from .vimrc,
    // and in that case we don't want to overwrite ligatures map that has already
    // been correctly populated (as that would lead to a cleared ligatures maps).
    जर (*p_guiligatures == NUL)
	CLEAR_FIELD(gui.ligatures_map);
#endif

#जर defined(ALWAYS_USE_GUI) || defined(VIMDLL)
    result = OK;
#अन्यथा
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
# अन्यथा
    result = gui_mch_init_check();
# endif
#endif
    परत result;
}

/*
 * This is the call which starts the GUI.
 */
    शून्य
gui_init(शून्य)
{
    win_T	*wp;
    tabpage_T	*tp;
    स्थिर पूर्णांक	recursive = 0;

    /*
     * It's possible to use ":gui" in a .gvimrc file.  The first half of this
     * function will then be executed at the first call, the rest by the
     * recursive call.  This allow the shell to be opened halfway reading a
     * gvimrc file.
     */
    जर (!recursive)
    {
	++recursive;

	clip_init(TRUE);

	// If can't initialize, don't try doing the rest
	जर (gui_init_check() == FAIL)
	{
	    --recursive;
	    clip_init(FALSE);
	    परत;
	}

	/*
	 * Reset 'paste'.  It's useful in the terminal, but not in the GUI.  It
	 * breaks the Paste toolbar button.
	 */
	set_option_मूल्य_give_err((char_u *)"paste", 0L, रिक्त, 0);

	// Set t_Co to the number of colors: RGB.
	set_color_मोजणी(256 * 256 * 256);

	/*
	 * Set up system-wide default menus.
	 */
#जर defined(SYS_MENU_FILE) && defined(FEAT_MENU)
	जर (vim_strchr(p_go, GO_NOSYSMENU) == रिक्त)
	{
	    sys_menu = TRUE;
	    do_source((char_u *)SYS_MENU_FILE, FALSE, DOSO_NONE, रिक्त);
	    sys_menu = FALSE;
	}
#endif

	/*
	 * Switch on the mouse by default, unless the user changed it already.
	 * This can then be changed in the .gvimrc.
	 */
	जर (!option_was_set((char_u *)"mouse"))
	    set_string_option_direct((char_u *)"mouse", -1,
					   (char_u *)"a", OPT_FREE, SID_NONE);

	/*
	 * If -U option given, use only the initializations from that file and
	 * nothing else.  Skip all initializations for "-U NONE" or "-u NORC".
	 */
	जर (use_gvimrc != रिक्त)
	{
	    जर (STRCMP(use_gvimrc, "NONE") != 0
		    && STRCMP(use_gvimrc, "NORC") != 0
		    && do_source(use_gvimrc, FALSE, DOSO_NONE, रिक्त) != OK)
		semsg(_(e_cannot_read_from_str), use_gvimrc);
	}
	अन्यथा
	{
	    /*
	     * Get system wide defaults for gvim, only when file name defined.
	     */
#ifdef SYS_GVIMRC_FILE
	    do_source((char_u *)SYS_GVIMRC_FILE, FALSE, DOSO_NONE, रिक्त);
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
	    जर (process_env((char_u *)"GVIMINIT", FALSE) == FAIL
		 && do_source((char_u *)USR_GVIMRC_FILE, TRUE,
						     DOSO_GVIMRC, रिक्त) == FAIL
#ifdef USR_GVIMRC_FILE2
		 && do_source((char_u *)USR_GVIMRC_FILE2, TRUE,
						     DOSO_GVIMRC, रिक्त) == FAIL
#endif
#ifdef USR_GVIMRC_FILE3
		 && do_source((char_u *)USR_GVIMRC_FILE3, TRUE,
						     DOSO_GVIMRC, रिक्त) == FAIL
#endif
				)
	    {
#ifdef USR_GVIMRC_FILE4
		(शून्य)do_source((char_u *)USR_GVIMRC_FILE4, TRUE,
							    DOSO_GVIMRC, रिक्त);
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
	    जर (p_exrc)
	    {
#ifdef UNIX
		{
		    stat_T s;

		    // if ".gvimrc" file is not owned by user, set 'secure'
		    // mode
		    जर (mch_stat(GVIMRC_FILE, &s) || s.st_uid != getuid())
			secure = p_secure;
		}
#अन्यथा
		secure = p_secure;
#endif

		जर (       fullpathcmp((char_u *)USR_GVIMRC_FILE,
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
		    do_source((char_u *)GVIMRC_FILE, TRUE, DOSO_GVIMRC, रिक्त);

		जर (secure == 2)
		    need_wait_return = TRUE;
		secure = 0;
	    }
	}

	जर (need_wait_return || संदेश_didany)
	    wait_return(TRUE);

	--recursive;
    }

    // If recursive call opened the shell, return here from the first call
    जर (gui.in_use)
	परत;

    /*
     * Create the GUI shell.
     */
    gui.in_use = true;		// Must be set after menus have been set up
#जर defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
    gui.scale = 1.0; // Default value
#endif
    जर (gui_mch_init() == FAIL)
	जा error;

    // Avoid a delay for an error message that was printed in the terminal
    // where Vim was started.
    emsg_on_display = FALSE;
    संदेश_scrolled = 0;
    clear_sb_text(TRUE);
    need_wait_return = FALSE;
    संदेश_didany = FALSE;

    /*
     * Check validity of any generic resources that may have been loaded.
     */
    जर (gui.border_width < 0)
	gui.border_width = 0;

    /*
     * Set up the fonts.  First use a font specified with "-fn" or "-font".
     */
    जर (font_argument != रिक्त)
	set_option_मूल्य_give_err((char_u *)"gfn",
					       0L, (char_u *)font_argument, 0);
    जर (
#ifdef FEAT_XFONTSET
	    (*p_guifontset == NUL
	     || gui_init_font(p_guifontset, TRUE) == FAIL) &&
#endif
	    gui_init_font(*p_guifont == NUL ? hl_get_font_name()
						  : p_guifont, FALSE) == FAIL)
    {
	emsg(_(e_cannot_start_gui_no_valid_font_found));
	जा error2;
    }
    जर (gui_get_wide_font() == FAIL)
	emsg(_(e_guifontwide_invalid));

    gui.अंक_cols = Columns;
    gui.अंक_rows = Rows;
    gui_reset_scroll_region();

    // Create initial scrollbars
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_डावा], SBAR_डावा, wp);
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_उजवा], SBAR_उजवा, wp);
    }
    gui_create_scrollbar(&gui.bottom_sbar, SBAR_BOTTOM, रिक्त);

#ifdef FEAT_MENU
    gui_create_initial_menus(मूळ_menu);
#endif
#ifdef FEAT_SIGN_ICONS
    sign_gui_started();
#endif

    // Configure the desired menu and scrollbars
    gui_init_which_components(रिक्त);

    // All components of the GUI have been created now
    gui.shell_created = true;

#ifdef FEAT_GUI_MSWIN
    // Set the shell size, adjusted for the screen size.  For GTK this only
    // works after the shell has been opened, thus it is further down.
    // If the window is already maximized (e.g. when --windowid is passed in),
    // we want to use the system-provided dimensions by passing FALSE to
    // mustset. Otherwise, we want to initialize with the default rows/columns.
    जर (gui_mch_maximized())
	gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    अन्यथा
	gui_set_shellsize(TRUE, TRUE, RESIZE_BOTH);
#अन्यथा
# ifndef FEAT_GUI_GTK
    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
# endif
#endif
#जर defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    // Need to set the size of the menubar after all the menus have been
    // created.
    gui_mch_compute_menu_height((Widget)0);
#endif

    /*
     * Actually open the GUI shell.
     */
    जर (gui_mch_open() != FAIL)
    {
	maketitle();
	resettitle();

	init_gui_options();
#ifdef FEAT_ARABIC
	// Our GUI can't do bidi.
	p_tbidi = FALSE;
#endif
#जर defined(FEAT_GUI_GTK)
	// Give GTK+ a chance to put all widget's into place.
	gui_mch_update();

# ifdef FEAT_MENU
	// If there is no 'm' in 'guioptions' we need to remove the menu now.
	// It was still there to make F10 work.
	जर (vim_strchr(p_go, GO_MENUS) == रिक्त)
	{
	    bool save_starting = gui.starting;

	    gui.starting = false;
	    gui_mch_enable_menu(FALSE);
	    gui.starting = save_starting;
	    gui_mch_update();
	}
# endif

	// Now make sure the shell fits on the screen.
	जर (gui_mch_maximized())
	    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
	अन्यथा
	    gui_set_shellsize(TRUE, TRUE, RESIZE_BOTH);
#endif
	// When 'lines' was set while starting up the topframe may have to be
	// resized.
	win_new_shellsize();

#ifdef FEAT_BEVAL_GUI
	// Always create the Balloon Evaluation area, but disable it when
	// 'ballooneval' is off.
	जर (balloonEval != रिक्त)
	{
# ifdef FEAT_VARTABS
	    vim_free(balloonEval->vts);
# endif
	    vim_free(balloonEval);
	}
	balloonEvalForTerm = FALSE;
# ifdef FEAT_GUI_GTK
	balloonEval = gui_mch_create_beval_area(gui.drawarea, रिक्त,
						     &general_beval_cb, रिक्त);
# अन्यथा
#  जर defined(FEAT_GUI_MOTIF)
	{
	    बाह्य Widget	textArea;
	    balloonEval = gui_mch_create_beval_area(textArea, रिक्त,
						     &general_beval_cb, रिक्त);
	}
#  अन्यथा
#   ifdef FEAT_GUI_MSWIN
	balloonEval = gui_mch_create_beval_area(रिक्त, रिक्त,
						     &general_beval_cb, रिक्त);
#   endif
#  endif
# endif
	जर (!p_beval)
	    gui_mch_disable_beval_area(balloonEval);
#endif

#ifndef FEAT_GUI_MSWIN
	// In the GUI modifiers are prepended to keys.
	// Don't do this for MS-Windows yet, it sends CTRL-K without the
	// modifier.
	seen_Modify_Other_किल्ल्या = TRUE;
#endif

#जर defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	जर (!im_xim_isvalid_imactivate())
	    emsg(_(e_मूल्य_of_imactivatekey_is_invalid));
#endif
	// When 'cmdheight' was set during startup it may not have taken
	// effect yet.
	जर (p_ch != 1L)
	    command_height();

	परत;
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


    शून्य
gui_exit(पूर्णांक rc)
{
    // don't free the fonts, it leads to a BUS error
    // richard@whitequeen.com Jul 99
    free_highlight_fonts();
    gui.in_use = false;
    gui_mch_exit(rc);
}

#जर defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_PHOTON)
# define NEED_GUI_UPDATE_SCREEN 1
/*
 * Called when the GUI shell is closed by the user.  If there are no changed
 * files Vim exits, otherwise there will be a dialog to ask the user what to
 * do.
 * When this function returns, Vim should NOT exit!
 */
    शून्य
gui_shell_closed(शून्य)
{
    cmdmod_T	    save_cmdmod = cmdmod;

    जर (before_quit_autocmds(curwin, TRUE, FALSE))
	परत;

    // Only exit when there are no changed files
    exiting = TRUE;
# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# जर defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    cmdmod.cmod_flags |= CMOD_CONFIRM;
# endif
    // If there are changed buffers, present the user with a dialog if
    // possible, otherwise give an error message.
    जर (!check_changed_any(FALSE, FALSE))
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
    पूर्णांक
gui_init_font(char_u *font_list, पूर्णांक fontset UNUSED)
{
#define FONTLEN 320
    char_u	font_name[FONTLEN];
    पूर्णांक		font_list_empty = FALSE;
    पूर्णांक		ret = FAIL;

    जर (!gui.in_use)
	परत FAIL;

    font_name[0] = NUL;
    जर (*font_list == NUL)
	font_list_empty = TRUE;
    अन्यथा
    {
#ifdef FEAT_XFONTSET
	// When using a fontset, the whole list of fonts is one name.
	जर (fontset)
	    ret = gui_mch_init_font(font_list, TRUE);
	अन्यथा
#endif
	    पर्यंत (*font_list != NUL)
	    {
		// Isolate one comma separated font name.
		(शून्य)copy_option_part(&font_list, font_name, FONTLEN, ",");

		// Careful!!!  The Win32 version of gui_mch_init_font(), when
		// called with "*" will change p_guifont to the selected font
		// name, which frees the old value.  This makes font_list
		// invalid.  Thus when OK is returned here, font_list must no
		// longer be used!
		जर (gui_mch_init_font(font_name, FALSE) == OK)
		{
#ifdef USE_SET_GUIFONTWIDE
		    // If it's a Unicode font, try setting 'guifontwide' to a
		    // similar double-width font.
		    जर ((p_guifontwide == रिक्त || *p_guifontwide == NUL)
				&& म_माला((अक्षर *)font_name, "10646") != रिक्त)
			set_guifontwide(font_name);
#endif
		    ret = OK;
		    थांबा;
		}
	    }
    }

    जर (ret != OK
	    && STRCMP(font_list, "*") != 0
	    && (font_list_empty || gui.norm_font == NOFONT))
    {
	/*
	 * Couldn't load any font in 'font_list', keep the current font if
	 * there is one.  If 'font_list' is empty, or if there is no current
	 * font, tell gui_mch_init_font() to try to find a font we can load.
	 */
	ret = gui_mch_init_font(रिक्त, FALSE);
    }

    जर (ret == OK)
    {
#ifndef FEAT_GUI_GTK
	// Set normal font as current font
# ifdef FEAT_XFONTSET
	जर (gui.fontset != NOFONTSET)
	    gui_mch_set_fontset(gui.fontset);
	अन्यथा
# endif
	    gui_mch_set_font(gui.norm_font);
#endif
	gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    }

    परत ret;
}

#ifdef USE_SET_GUIFONTWIDE
/*
 * Try setting 'guifontwide' to a font twice as wide as "name".
 */
    स्थिर शून्य
set_guifontwide(char_u *name)
{
    पूर्णांक		i = 0;
    char_u	wide_name[FONTLEN + 10]; // room for 2 * width and '*'
    char_u	*wp = रिक्त;
    char_u	*p;
    GuiFont	font;

    wp = wide_name;
    साठी (p = name; *p != NUL; ++p)
    {
	*wp++ = *p;
	जर (*p == '-')
	{
	    ++i;
	    जर (i == 6)		// font type: change "--" to "-*-"
	    {
		जर (p[1] == '-')
		    *wp++ = '*';
	    }
	    अन्यथा जर (i == 12)	// found the width
	    {
		++p;
		i = getdigits(&p);
		जर (i != 0)
		{
		    // Double the width specification.
		    प्र_लिखो((अक्षर *)wp, "%d%s", i * 2, p);
		    font = gui_mch_get_font(wide_name, FALSE);
		    जर (font != NOFONT)
		    {
			gui_mch_free_font(gui.wide_font);
			gui.wide_font = font;
			set_string_option_direct((char_u *)"gfw", -1,
						      wide_name, OPT_FREE, 0);
		    }
		}
		थांबा;
	    }
	}
    }
}
#endif

/*
 * Get the font for 'guifontwide'.
 * Return FAIL for an invalid font name.
 */
    पूर्णांक
gui_get_wide_font(शून्य)
{
    GuiFont	font = NOFONT;
    char_u	font_name[FONTLEN];
    char_u	*p;

    जर (!gui.in_use)	    // Can't allocate font yet, assume it's OK.
	परत OK;	    // Will give an error message later.

    जर (p_guifontwide != रिक्त && *p_guifontwide != NUL)
    {
	साठी (p = p_guifontwide; *p != NUL; )
	{
	    // Isolate one comma separated font name.
	    (शून्य)copy_option_part(&p, font_name, FONTLEN, ",");
	    font = gui_mch_get_font(font_name, FALSE);
	    जर (font != NOFONT)
		थांबा;
	}
	जर (font == NOFONT)
	    परत FAIL;
    }

    gui_mch_free_font(gui.wide_font);
#ifdef FEAT_GUI_GTK
    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    जर (font != NOFONT && gui.norm_font != NOFONT
			 && pango_font_description_equal(font, gui.norm_font))
    {
	gui.wide_font = NOFONT;
	gui_mch_free_font(font);
    }
    अन्यथा
#endif
	gui.wide_font = font;
#ifdef FEAT_GUI_MSWIN
    gui_mch_wide_font_changed();
#अन्यथा
    /*
     * TODO: setup wide_bold_font, wide_ital_font and wide_boldital_font to
     * support those fonts for 'guifontwide'.
     */
#endif
    परत OK;
}

#जर defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * Set list of ascii characters that combined can create ligature.
 * Store them in char map for quick access from gui_gtk_draw_string.
 */
    शून्य
gui_set_ligatures(शून्य)
{
    char_u	*p;

    जर (*p_guiligatures != NUL)
    {
	// check for invalid characters
	साठी (p = p_guiligatures; *p != NUL; ++p)
	    जर (*p < 32 || *p > 127)
	    {
		emsg(_(e_ascii_code_not_in_range));
		परत;
	    }

	// store valid setting into ligatures_map
	CLEAR_FIELD(gui.ligatures_map);
	साठी (p = p_guiligatures; *p != NUL; ++p)
	    gui.ligatures_map[*p] = 1;
    }
    अन्यथा
	CLEAR_FIELD(gui.ligatures_map);
}

/*
 * Adjust the columns to undraw for when the cursor is on ligatures.
 */
    स्थिर शून्य
gui_adjust_undraw_cursor_for_ligatures(पूर्णांक *startcol, पूर्णांक *endcol)
{
    पूर्णांक off;

    जर (Screen_ओळी == रिक्त || *p_guiligatures == NUL)
	परत;

    // expand before the cursor for all the chars in gui.ligatures_map
    off = ओळ_Offset[gui.cursor_row] + *startcol;
    जर (gui.ligatures_map[Screen_ओळी[off]])
	पर्यंत (*startcol > 0 && gui.ligatures_map[Screen_ओळी[--off]])
	    (*startcol)--;

    // expand after the cursor for all the chars in gui.ligatures_map
    off = ओळ_Offset[gui.cursor_row] + *endcol;
    जर (gui.ligatures_map[Screen_ओळी[off]])
	पर्यंत (*endcol < ((पूर्णांक)screen_Columns - 1)
				      && gui.ligatures_map[Screen_ओळी[++off]])
	   (*endcol)++;
}
#endif

    स्थिर शून्य
gui_set_cursor(पूर्णांक row, पूर्णांक col)
{
    gui.row = row;
    gui.col = col;
}

/*
 * gui_check_pos - check if the cursor is on the screen.
 */
    स्थिर शून्य
gui_check_pos(शून्य)
{
    जर (gui.row >= screen_Rows)
	gui.row = screen_Rows - 1;
    जर (gui.col >= screen_Columns)
	gui.col = screen_Columns - 1;
    जर (gui.cursor_row >= screen_Rows || gui.cursor_col >= screen_Columns)
	gui.cursor_is_valid = false;
}

/*
 * Redraw the cursor if necessary or when forced.
 * Careful: The contents of ScreenLines[] must match what is on the screen,
 * otherwise this goes wrong.  May need to call out_flush() first.
 */
    शून्य
gui_update_cursor(
    पूर्णांक		force,		 // when TRUE, update even when not moved
    पूर्णांक		clear_selection) // clear selection under cursor
{
    पूर्णांक		cur_width = 0;
    पूर्णांक		cur_height = 0;
    पूर्णांक		old_hl_mask;
    cursorentry_T *shape;
    पूर्णांक		id;
#ifdef FEAT_TERMINAL
    guicolor_T	shape_fg = INVALCOLOR;
    guicolor_T	shape_bg = INVALCOLOR;
#endif
    guicolor_T	cfg, cbg, cc;	// cursor fore-/background color
    पूर्णांक		cattr;		// cursor attributes
    पूर्णांक		attr;
    attrentry_T *aep = रिक्त;
#जर (defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)) && !defined(USE_GTK4)
    bool	lig_डावा = false, lig_उजवा = false;
#endif

    // Don't update the cursor when halfway busy scrolling or the screen size
    // doesn't match 'columns' and 'lines.  ScreenLines[] isn't valid then.
    जर (!can_update_cursor || screen_Columns != gui.अंक_cols
					       || screen_Rows != gui.अंक_rows)
	परत;

    gui_check_pos();

    जर (gui.cursor_is_valid && !force
		&& gui.row == gui.cursor_row && gui.col == gui.cursor_col)
	परत;

    gui_undraw_cursor();

    // If a cursor-less sleep is ongoing, leave the cursor invisible
    जर (cursor_is_sleeping())
	परत;

    जर (gui.row < 0)
	परत;
#ifdef HAVE_आदान_METHOD
    जर (gui.row != gui.cursor_row || gui.col != gui.cursor_col)
	im_set_position(gui.row, gui.col);
#endif
    gui.cursor_row = gui.row;
    gui.cursor_col = gui.col;

    // Only write to the screen after ScreenLines[] has been initialized
    जर (!screen_cleared || Screen_ओळी == रिक्त)
	परत;

    // Clear the selection if we are about to write over it
    जर (clear_selection)
	clip_may_clear_selection(gui.row, gui.row);
    // Check that the cursor is inside the shell (resizing may have made
    // it invalid)
    जर (gui.row >= screen_Rows || gui.col >= screen_Columns)
	परत;

    gui.cursor_is_valid = true;

    /*
     * How the cursor is drawn depends on the current mode.
     * When in a terminal window use the shape/color specified there.
     */
#ifdef FEAT_TERMINAL
    जर (terminal_is_active())
	shape = term_get_cursor_shape(&shape_fg, &shape_bg);
    अन्यथा
#endif
	shape = &shape_table[get_shape_idx(FALSE)];
    जर (State & MODE_LANGMAP)
	id = shape->id_lm;
    अन्यथा
	id = shape->id;

    // get the colors and attributes for the cursor.  Default is inverted
    cfg = INVALCOLOR;
    cbg = INVALCOLOR;
    cattr = HL_INVERSE;
    gui_mch_set_blinking(shape->blinkwait,
	    shape->blinkon,
	    shape->blinkoff);
    जर (shape->blinkwait == 0 || shape->blinkon == 0
	    || shape->blinkoff == 0)
	gui_mch_stop_blink(FALSE);
#ifdef FEAT_TERMINAL
    जर (shape_bg != INVALCOLOR)
    {
	cattr = 0;
	cfg = shape_fg;
	cbg = shape_bg;
    }
    अन्यथा
#endif
	जर (id > 0)
	{
	    cattr = syn_id2colors(id, &cfg, &cbg);
#जर defined(HAVE_आदान_METHOD)
	    {
		स्थिर पूर्णांक iid;
		guicolor_T fg, bg;

		जर (
# जर defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
			preedit_get_status()
# अन्यथा
			im_get_status()
# endif
		   )
		{
		    iid = syn_name2id((char_u *)"CursorIM");
		    जर (iid > 0)
		    {
			syn_id2colors(iid, &fg, &bg);
			जर (bg != INVALCOLOR)
			    cbg = bg;
			जर (fg != INVALCOLOR)
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
    attr = ScreenAttrs[ओळ_Offset[gui.row] + gui.col];
    जर (attr > HL_ALL)
	aep = syn_gui_attr2entry(attr);
    जर (aep != रिक्त)
    {
	attr = aep->ae_attr;
	जर (cfg == INVALCOLOR)
	    cfg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.bg_color
		    : aep->ae_u.gui.fg_color);
	जर (cbg == INVALCOLOR)
	    cbg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.fg_color
		    : aep->ae_u.gui.bg_color);
    }
    जर (cfg == INVALCOLOR)
	cfg = (attr & HL_INVERSE) ? gui.back_pixel : gui.norm_pixel;
    जर (cbg == INVALCOLOR)
	cbg = (attr & HL_INVERSE) ? gui.norm_pixel : gui.back_pixel;

#ifdef FEAT_XIM
    जर (aep != रिक्त)
    {
	xim_bg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.fg_color
		: aep->ae_u.gui.bg_color);
	xim_fg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.bg_color
		: aep->ae_u.gui.fg_color);
	जर (xim_bg_color == INVALCOLOR)
	    xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
		: gui.back_pixel;
	जर (xim_fg_color == INVALCOLOR)
	    xim_fg_color = (attr & HL_INVERSE) ? gui.back_pixel
		: gui.norm_pixel;
    }
    अन्यथा
    {
	xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
	    : gui.back_pixel;
	xim_fg_color = (attr & HL_INVERSE) ? gui.back_pixel
	    : gui.norm_pixel;
    }
#endif

    attr &= ~HL_INVERSE;
    जर (cattr & HL_INVERSE)
    {
	cc = cbg;
	cbg = cfg;
	cfg = cc;
    }
    cattr &= ~HL_INVERSE;

    /*
     * When we don't have window focus, draw a hollow cursor.
     */
    जर (!gui.in_focus)
    {
#ifdef USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, -1, -1);
#अन्यथा
	gui_mch_draw_hollow_cursor(cbg);
#endif
	परत;
    }

#ifdef USE_GTK4
    // Make sure that character underneath is drawn again in case it is part of
    // a ligature.
    gui_redraw_block(gui.row, gui.col, gui.row, gui.col, GUI_MON_NOCLEAR);
    // gui_redraw_block() will invalidate the cursor
    gui.cursor_is_valid = true;
#endif
#जर defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // If cursor is in the middle of a ligature, then split the ligature at
    // the cursor boundaries by redrawing those cells again.
    साठी (पूर्णांक c = gui.col - 1; c >= 0; c--)
	जर (!gui.ligatures_map[Screen_ओळी[ओळ_Offset[gui.row] + c]])
	{
	    जर (c < gui.col - 1)
	    {
		gui_redraw_block(gui.row, c + 1, gui.row, gui.col - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_डावा = true;
# endif
	    }
	    थांबा;
	}

    साठी (पूर्णांक c = gui.col + 1; c < screen_Columns - 1; c++)
	जर (!gui.ligatures_map[Screen_ओळी[ओळ_Offset[gui.row] + c]])
	{
	    जर (c > gui.col + 1)
	    {
		gui_redraw_block(gui.row, gui.col + 1, gui.row, c - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_उजवा = true;
# endif
	    }
	    थांबा;
	}

# ifndef USE_GTK4
    जर ((lig_डावा || lig_उजवा) && shape->shape != SHAPE_BLOCK)
    {
	// Because the cursor is not drawn with gui_screenchar(), must blit the
	// cell again (with its background), so that old ligature does not
	// remain on screen. Not needed for GtkSnapshot, because char beneath is
	// always rerendered (see above ifdef).
	पूर्णांक old = gui.col;
	gui.highlight_mask = ScreenAttrs[ओळ_Offset[gui.row] + gui.col];
	(शून्य)gui_screenchar(ओळ_Offset[gui.row] + gui.col,
		GUI_MON_NOCLEAR, (guicolor_T)0, (guicolor_T)0, 0);
	gui.col = old;
    }
# endif
    // gui_redraw_block()/gui_screenchar() may invalidate the cursor, make sure
    // to validate it again.
    gui.cursor_is_valid = true;
#endif

    old_hl_mask = gui.highlight_mask;
    जर (shape->shape == SHAPE_BLOCK)
    {
#ifdef USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, 0, 0);
#अन्यथा
	/*
	 * Draw the text character with the cursor colors.	Use the
	 * character attributes plus the cursor attributes.
	 */
	gui.highlight_mask = (cattr | attr);
	(शून्य)gui_screenchar(ओळ_Offset[gui.row] + gui.col,
		GUI_MON_IS_CURSOR | GUI_MON_NOCLEAR, cfg, cbg, 0);
#endif
    }
    अन्यथा
    {
#जर defined(FEAT_RIGHTLEFT)
	पूर्णांक	    col_off = FALSE;
#endif
	/*
	 * First draw the partial cursor, then overwrite with the text
	 * character, using a transparent background.
	 */
	जर (shape->shape == SHAPE_VER)
	{
	    cur_height = gui.char_height;
	    cur_width = (gui.char_width * shape->percentage + 99) / 100;
	}
	अन्यथा
	{
	    cur_height = (gui.char_height * shape->percentage + 99) / 100;
	    cur_width = gui.char_width;
	}
	जर (has_mbyte && (*mb_off2cells)(ओळ_Offset[gui.row] + gui.col,
		    ओळ_Offset[gui.row] + screen_Columns) > 1)
	{
	    // Double wide character.
	    जर (shape->shape != SHAPE_VER)
		cur_width += gui.char_width;
#ifdef FEAT_RIGHTLEFT
	    जर (CURSOR_BAR_उजवा)
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
#अन्यथा
	gui_mch_draw_part_cursor(cur_width, cur_height, cbg);
#endif
#जर defined(FEAT_RIGHTLEFT)
	जर (col_off)
	    --gui.col;
#endif

	// Doesn't seem to work for MSWindows. We call gui_redraw_block() above
	// for GtkSnapshot.
#जर !defined(FEAT_GUI_MSWIN) && !defined(USE_GTK4)
	gui.highlight_mask = ScreenAttrs[ओळ_Offset[gui.row] + gui.col];
	(शून्य)gui_screenchar(ओळ_Offset[gui.row] + gui.col,
		GUI_MON_TRS_CURSOR | GUI_MON_NOCLEAR,
		(guicolor_T)0, (guicolor_T)0, 0);
#endif
    }
    gui.highlight_mask = old_hl_mask;
}

#जर defined(FEAT_MENU)
    स्थिर शून्य
gui_position_menu(शून्य)
{
# जर !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
    जर (gui.menu_is_active && gui.in_use)
	gui_mch_set_menu_pos(0, 0, gui.menu_width, gui.menu_height);
# endif
}
#endif

/*
 * Position the various GUI components (text area, menu).  The vertical
 * scrollbars are NOT handled here.  See gui_update_scrollbars().
 */
    स्थिर शून्य
gui_position_components(पूर्णांक total_width UNUSED)
{
    पूर्णांक	    text_area_x;
    पूर्णांक	    text_area_y;
    पूर्णांक	    text_area_width;
    पूर्णांक	    text_area_height;

    // avoid that moving components around generates events
    ++hold_gui_events;

    text_area_x = 0;
    जर (gui.which_scrollbars[SBAR_डावा])
	text_area_x += gui.scrollbar_width;

    text_area_y = 0;
#जर defined(FEAT_MENU) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    gui.menu_width = total_width;
    जर (gui.menu_is_active)
	text_area_y += gui.menu_height;
#endif

#जर defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
    जर (gui_has_tabline())
	text_area_y += gui.tabline_height;
#endif

#जर defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    जर (vim_strchr(p_go, GO_TOOLBAR) != रिक्त)
    {
# जर defined(FEAT_GUI_HAIKU)
	gui_mch_set_toolbar_pos(0, text_area_y,
				gui.menu_width, gui.toolbar_height);
# endif
	text_area_y += gui.toolbar_height;
    }
#endif

#जर defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_HAIKU)
    gui_mch_set_tabline_pos(0, text_area_y,
    gui.menu_width, gui.tabline_height);
    जर (gui_has_tabline())
	text_area_y += gui.tabline_height;
#endif

    text_area_width = gui.अंक_cols * gui.char_width + gui.border_offset * 2;
    text_area_height = gui.अंक_rows * gui.char_height + gui.border_offset * 2;

    gui_mch_set_text_area_pos(text_area_x,
			      text_area_y,
			      text_area_width,
			      text_area_height
#जर defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
				  + xim_get_status_area_height()
#endif
			      );
#ifdef FEAT_MENU
    gui_position_menu();
#endif
    जर (gui.which_scrollbars[SBAR_BOTTOM])
	gui_mch_set_scrollbar_pos(&gui.bottom_sbar,
				  text_area_x,
				  text_area_y + text_area_height
					+ gui_mch_get_scrollbar_ypadding(),
				  text_area_width,
				  gui.scrollbar_height);
    gui.डावा_sbar_x = 0;
    gui.उजवा_sbar_x = text_area_x + text_area_width
					+ gui_mch_get_scrollbar_xpadding();

    --hold_gui_events;
}

/*
 * Get the width of the widgets and decorations to the side of the text area.
 */
    पूर्णांक
gui_get_base_width(शून्य)
{
    पूर्णांक	    base_width;

    base_width = 2 * gui.border_offset;
    जर (gui.which_scrollbars[SBAR_डावा])
	base_width += gui.scrollbar_width;
    जर (gui.which_scrollbars[SBAR_उजवा])
	base_width += gui.scrollbar_width;
    परत base_width;
}

/*
 * Get the height of the widgets and decorations above and below the text area.
 */
    पूर्णांक
gui_get_base_height(शून्य)
{
    पूर्णांक	    base_height;

    base_height = 2 * gui.border_offset;
    जर (gui.which_scrollbars[SBAR_BOTTOM])
	base_height += gui.scrollbar_height;
#ifdef FEAT_GUI_GTK
    // We can't take the sizes properly into account until anything is
    // realized.  Therefore we recalculate all the values here just before
    // setting the size. (--mdcki)
#अन्यथा
# ifdef FEAT_MENU
    जर (gui.menu_is_active)
	base_height += gui.menu_height;
# endif
# ifdef FEAT_TOOLBAR
    जर (vim_strchr(p_go, GO_TOOLBAR) != रिक्त)
	base_height += gui.toolbar_height;
# endif
# जर defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    जर (gui_has_tabline())
	base_height += gui.tabline_height;
# endif
# जर defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    base_height += gui_mch_text_area_extra_height();
# endif
#endif
    परत base_height;
}

/*
 * Should be called after the GUI shell has been resized.  Its arguments are
 * the new width and height of the shell in pixels.
 */
    शून्य
gui_resize_shell(पूर्णांक pixel_width, पूर्णांक pixel_height)
{
    स्थिर पूर्णांक	busy = FALSE;

    जर (!gui.shell_created)	    // ignore when still initializing
	परत;

    /*
     * Can't resize the screen while it is being redrawn.  Remember the new
     * size and handle it later.
     */
    जर (updating_screen || busy)
    {
	new_pixel_width = pixel_width;
	new_pixel_height = pixel_height;
	परत;
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
#जर defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk_init_decor_height();
#endif

    gui.अंक_cols = (pixel_width - gui_get_base_width()) / gui.char_width;
    gui.अंक_rows = (pixel_height - gui_get_base_height()) / gui.char_height;

#ifdef USE_GTK4
    gui_gtk4_update_size();
#endif

    gui_position_components(pixel_width);
    gui_reset_scroll_region();

#जर defined(FEAT_GUI_GTK) && defined(USE_GTK4) && !defined(USE_GTK4)
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
    जर (State == MODE_ASKMORE || State == MODE_CONFIRM)
	gui.row = gui.अंक_rows;

    // Only comparing Rows and Columns may be sufficient, but let's stay on
    // the safe side.
    जर (gui.अंक_rows != screen_Rows || gui.अंक_cols != screen_Columns
	    || gui.अंक_rows != Rows || gui.अंक_cols != Columns || gui.force_redraw)
    {
	shell_resized();
	gui.force_redraw = 0;
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    gui_update_scrollbars(TRUE);
    gui_update_cursor(FALSE, TRUE);
#जर defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(pixel_width, pixel_height);
#endif
#जर defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
    xim_set_status_area();
#endif

    busy = FALSE;

    // We may have been called again while redrawing the screen.
    // Need to do it all again with the latest size then.  But only if the size
    // actually changed.
    जर (new_pixel_height)
    {
	जर (pixel_width == new_pixel_width && pixel_height == new_pixel_height)
	{
	    new_pixel_width = 0;
	    new_pixel_height = 0;
	}
	अन्यथा
	{
	    pixel_width = new_pixel_width;
	    pixel_height = new_pixel_height;
	    जा again;
	}
    }
}

/*
 * Check if gui_resize_shell() must be called.
 */
    शून्य
gui_may_resize_shell(शून्य)
{
    जर (new_pixel_height)
    {
	// careful: gui_resize_shell() may postpone the resize again if we
	// were called indirectly by it
	gui_resize_shell(new_pixel_width, new_pixel_height);
    }
}

    पूर्णांक
gui_get_shellsize(शून्य)
{
    Rows = gui.अंक_rows;
    Columns = gui.अंक_cols;
    परत OK;
}

/*
 * Set the size of the Vim shell according to Rows and Columns.
 * If "fit_to_display" is TRUE then the size may be reduced to fit the window
 * on the screen.
 * When "mustset" is TRUE the size was set by the user. When FALSE a UI
 * component was added or removed (e.g., a scrollbar).
 */
    शून्य
gui_set_shellsize(
    पूर्णांक		mustset UNUSED,
    पूर्णांक		fit_to_display,
    पूर्णांक		direction)		// RESIZE_HOR, RESIZE_VER
{
    पूर्णांक		base_width;
    पूर्णांक		base_height;
    पूर्णांक		width;
    पूर्णांक		height;
    पूर्णांक		min_width;
    पूर्णांक		min_height;
    पूर्णांक		screen_w;
    पूर्णांक		screen_h;
#ifdef FEAT_GUI_GTK
    पूर्णांक		un_maximize = mustset;
    पूर्णांक		did_adjust = 0;
#endif
    पूर्णांक		x = -1, y = -1;

    जर (!gui.shell_created)
	परत;

#जर defined(MSWIN) || defined(FEAT_GUI_GTK)
    // If not setting to a user specified size and maximized, calculate the
    // number of characters that fit in the maximized window.
    // FIXME: gui_mch_newfont() is called here even when the font hasn't
    // changed at all.  For example, ":set guioptions=k" triggers this path
    // via gui_init_which_components() -> gui_set_shellsize(FALSE, ...).
    // The intent is to keep the window size and recalculate Rows/Columns,
    // which has nothing to do with fonts.  This should be a separate
    // function with a more descriptive name.
    जर (!mustset && (vim_strchr(p_go, GO_KEEPWINSIZE) != रिक्त
						       || gui_mch_maximized()))
    {
	gui_mch_newfont();
	परत;
    }
#endif

    base_width = gui_get_base_width();
    base_height = gui_get_base_height();
    जर (fit_to_display)
	// Remember the original window position.
	(शून्य)gui_mch_get_winpos(&x, &y);

    width = Columns * gui.char_width + base_width;
    height = Rows * gui.char_height + base_height;

    जर (fit_to_display)
    {
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	जर ((direction & RESIZE_HOR) && width > screen_w)
	{
	    Columns = (screen_w - base_width) / gui.char_width;
	    जर (Columns < MIN_COLUMNS)
		Columns = MIN_COLUMNS;
	    width = Columns * gui.char_width + base_width;
#ifdef FEAT_GUI_GTK
	    ++did_adjust;
#endif
	}
	जर ((direction & RESIZE_VERT) && height > screen_h)
	{
	    Rows = (screen_h - base_height) / gui.char_height;
	    check_shellsize();
	    height = Rows * gui.char_height + base_height;
#ifdef FEAT_GUI_GTK
	    ++did_adjust;
#endif
	}
#ifdef FEAT_GUI_GTK
	जर (did_adjust == 2 || (width + gui.char_width >= screen_w
				     && height + gui.char_height >= screen_h))
	    // don't unmaximize if at maximum size
	    un_maximize = FALSE;
#endif
    }
    limit_screen_size();
    gui.अंक_cols = Columns;
    gui.अंक_rows = Rows;
#ifdef USE_GTK4
    // Keep the drawing area in sync with the size Vim is going to draw.
    gui_gtk4_update_size();
#endif

    min_width = base_width + MIN_COLUMNS * gui.char_width;
    min_height = base_height + MIN_ओळी * gui.char_height;
    min_height += tabline_height() * gui.char_height;

#ifdef FEAT_GUI_GTK
    जर (un_maximize)
    {
	// If the window size is smaller than the screen unmaximize the
	// window, otherwise resizing won't work.
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	जर ((width + gui.char_width < screen_w
				   || height + gui.char_height * 2 < screen_h)
		&& gui_mch_maximized())
	    gui_mch_unmaximize();
    }
#endif

    gui_mch_set_shellsize(width, height, min_width, min_height,
					  base_width, base_height, direction);

    जर (fit_to_display && x >= 0 && y >= 0)
    {
	// Some window managers put the Vim window left of/above the screen.
	// Only change the position if it wasn't already negative before
	// (happens on MS-Windows with a secondary monitor).
	gui_mch_update();
	जर (gui_mch_get_winpos(&x, &y) == OK && (x < 0 || y < 0))
	    gui_mch_set_winpos(x < 0 ? 0 : x, y < 0 ? 0 : y);
    }

    gui_position_components(width);
    gui_update_scrollbars(TRUE);
    gui_reset_scroll_region();

#जर defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(width, height);
#endif
}

/*
 * Called when Rows and/or Columns has changed.
 */
    शून्य
gui_new_shellsize(शून्य)
{
    gui_reset_scroll_region();
}

/*
 * Make scroll region cover whole screen.
 */
    स्थिर शून्य
gui_reset_scroll_region(शून्य)
{
    gui.scroll_region_शीर्ष = 0;
    gui.scroll_region_bot = gui.अंक_rows - 1;
    gui.scroll_region_डावा = 0;
    gui.scroll_region_उजवा = gui.अंक_cols - 1;
}

    स्थिर शून्य
gui_start_highlight(पूर्णांक mask)
{
    जर (mask > HL_ALL)		    // highlight code
	gui.highlight_mask = mask;
    अन्यथा			    // mask
	gui.highlight_mask |= mask;
}

    शून्य
gui_stop_highlight(पूर्णांक mask)
{
    जर (mask > HL_ALL)		    // highlight code
	gui.highlight_mask = HL_NORMAL;
    अन्यथा			    // mask
	gui.highlight_mask &= ~mask;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    शून्य
gui_clear_block(
    पूर्णांक	    row1,
    पूर्णांक	    col1,
    पूर्णांक	    row2,
    पूर्णांक	    col2)
{
    // Clear the selection if we are about to write over it
    clip_may_clear_selection(row1, row2);

    gui_mch_clear_block(row1, col1, row2, col2);

    // Invalidate cursor if it was in this block
    जर (       gui.cursor_row >= row1 && gui.cursor_row <= row2
	    && gui.cursor_col >= col1 && gui.cursor_col <= col2)
	gui.cursor_is_valid = false;
}

/*
 * Write code to update the cursor later.  This avoids the need to flush the
 * output buffer before calling gui_update_cursor().
 */
    शून्य
gui_update_cursor_later(शून्य)
{
    OUT_STR("\033|s");
}

    शून्य
gui_write(
    char_u	*s,
    पूर्णांक		len)
{
    char_u	*p;
    पूर्णांक		arg1 = 0, arg2 = 0;
    पूर्णांक		force_cursor = FALSE;	// force cursor update
    पूर्णांक		force_scrollbar = FALSE;
    स्थिर win_T	*old_curwin = रिक्त;

// #define DEBUG_GUI_WRITE
#ifdef DEBUG_GUI_WRITE
    {
	पूर्णांक i;
	char_u *str;

	म_लिखो("gui_write(%d):\n    ", len);
	साठी (i = 0; i < len; i++)
	    जर (s[i] == ESC)
	    {
		जर (i != 0)
		    म_लिखो("\n    ");
		म_लिखो("<ESC>");
	    }
	    अन्यथा
	    {
		str = transchar_byte(s[i]);
		जर (str[0] && str[1])
		    म_लिखो("<%s>", (अक्षर *)str);
		अन्यथा
		    म_लिखो("%s", (अक्षर *)str);
	    }
	म_लिखो("\n");
    }
#endif
    पर्यंत (len)
    {
	जर (s[0] == ESC && s[1] == '|')
	{
	    p = s + 2;
	    जर (VIM_ISDIGIT(*p) || (*p == '-' && VIM_ISDIGIT(*(p + 1))))
	    {
		arg1 = getdigits(&p);
		जर (p > s + len)
		    थांबा;
		जर (*p == ';')
		{
		    ++p;
		    arg2 = getdigits(&p);
		    जर (p > s + len)
			थांबा;
		}
	    }
	    निवड (*p)
	    {
		स्थिती 'C':	// Clear screen
		    clip_scroll_selection(9999);
		    gui_mch_clear_all();
		    gui.cursor_is_valid = false;
		    force_scrollbar = TRUE;
		    थांबा;
		स्थिती 'M':	// Move cursor
		    gui_set_cursor(arg1, arg2);
		    थांबा;
		स्थिती 's':	// force cursor (shape) update
		    force_cursor = TRUE;
		    थांबा;
		स्थिती 'R':	// Set scroll region
		    जर (arg1 < arg2)
		    {
			gui.scroll_region_शीर्ष = arg1;
			gui.scroll_region_bot = arg2;
		    }
		    अन्यथा
		    {
			gui.scroll_region_शीर्ष = arg2;
			gui.scroll_region_bot = arg1;
		    }
		    थांबा;
		स्थिती 'V':	// Set vertical scroll region
		    जर (arg1 < arg2)
		    {
			gui.scroll_region_डावा = arg1;
			gui.scroll_region_उजवा = arg2;
		    }
		    अन्यथा
		    {
			gui.scroll_region_डावा = arg2;
			gui.scroll_region_उजवा = arg1;
		    }
		    थांबा;
		स्थिती 'd':	// Delete line
		    gui_delete_ओळी(gui.row, 1);
		    थांबा;
		स्थिती 'D':	// Delete lines
		    gui_delete_ओळी(gui.row, arg1);
		    थांबा;
		स्थिती 'i':	// Insert line
		    gui_घाला_ओळी(gui.row, 1);
		    थांबा;
		स्थिती 'I':	// Insert lines
		    gui_घाला_ओळी(gui.row, arg1);
		    थांबा;
		स्थिती '$':	// Clear to end-of-line
		    gui_clear_block(gui.row, gui.col, gui.row,
							    (पूर्णांक)Columns - 1);
		    थांबा;
		स्थिती 'h':	// Turn on highlighting
		    gui_start_highlight(arg1);
		    थांबा;
		स्थिती 'H':	// Turn off highlighting
		    gui_stop_highlight(arg1);
		    थांबा;
		स्थिती 'f':	// flash the window (visual bell)
		    gui_mch_flash(arg1 == 0 ? 20 : arg1);
		    थांबा;
		पूर्वनिर्धारित:
		    p = s + 1;	// Skip the ESC
		    थांबा;
	    }
	    len -= (पूर्णांक)(++p - s);
	    s = p;
	}
	अन्यथा जर (s[0] < 0x20		// Ctrl character
#ifdef FEAT_SIGN_ICONS
		&& s[0] != SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
		&& s[0] != MULTISIGN_BYTE
# endif
#endif
		)
	{
	    जर (s[0] == '\n')		// NL
	    {
		gui.col = 0;
		जर (gui.row < gui.scroll_region_bot)
		    gui.row++;
		अन्यथा
		    gui_delete_ओळी(gui.scroll_region_शीर्ष, 1);
	    }
	    अन्यथा जर (s[0] == '\r')	// CR
	    {
		gui.col = 0;
	    }
	    अन्यथा जर (s[0] == '\b')	// Backspace
	    {
		जर (gui.col)
		    --gui.col;
	    }
	    अन्यथा जर (s[0] == Ctrl_L)	// cursor-right
	    {
		++gui.col;
	    }
	    अन्यथा जर (s[0] == Ctrl_G)	// Beep
	    {
		gui_mch_beep();
	    }
	    // Other Ctrl character: shouldn't happen!

	    --len;	// Skip this char
	    ++s;
	}
	अन्यथा
	{
	    p = s;
	    पर्यंत (len > 0 && (
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
	    gui_outstr(s, (पूर्णांक)(p - s));
	    s = p;
	}
    }

    // Postponed update of the cursor (won't work if "can_update_cursor" isn't
    // set).
    जर (force_cursor)
	gui_update_cursor(TRUE, TRUE);

    // When switching to another window the dragging must have stopped.
    // Required for GTK, dragged_sb isn't reset.
    जर (old_curwin != curwin)
	gui.dragged_sb = SBAR_NONE;

    // Update the scrollbars after clearing the screen or when switched
    // to another window.
    // Update the horizontal scrollbar always, it's difficult to check all
    // situations where it might change.
    जर (force_scrollbar || old_curwin != curwin)
	gui_update_scrollbars(force_scrollbar);
    अन्यथा
	gui_update_horiz_scrollbar(FALSE);
    old_curwin = curwin;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#जर defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#endif

    gui_may_flush();		    // In case vim decides to take a nap
}

/*
 * When ScreenLines[] is invalid, updating the cursor should not be done, it
 * produces wrong results.  Call gui_dont_update_cursor() before that code and
 * gui_can_update_cursor() afterwards.
 */
    शून्य
gui_dont_update_cursor(पूर्णांक undraw)
{
    जर (!gui.in_use)
	परत;

    // Undraw the cursor now, we probably can't do it after the change.
    जर (undraw)
	gui_undraw_cursor();
    can_update_cursor = FALSE;
}

    शून्य
gui_can_update_cursor(शून्य)
{
    can_update_cursor = TRUE;
    // No need to update the cursor right now, there is always more output
    // after scrolling.
}

/*
 * Disable issuing gui_mch_flush().
 */
    शून्य
gui_disable_flush(शून्य)
{
    ++disable_flush;
}

/*
 * Enable issuing gui_mch_flush().
 */
    शून्य
gui_enable_flush(शून्य)
{
    --disable_flush;
}

/*
 * Issue gui_mch_flush() if it is not disabled.
 */
    शून्य
gui_may_flush(शून्य)
{
    जर (disable_flush == 0)
	gui_mch_flush();
}

    स्थिर शून्य
gui_outstr(char_u *s, पूर्णांक len)
{
    पूर्णांक	    this_len;
    पूर्णांक	    cells;

    जर (len == 0)
	परत;

    जर (len < 0)
	len = (पूर्णांक)STRLEN(s);

    पर्यंत (len > 0)
    {
	जर (has_mbyte)
	{
	    // Find out how many chars fit in the current line.
	    cells = 0;
	    साठी (this_len = 0; this_len < len; )
	    {
		cells += (*mb_ptr2cells)(s + this_len);
		जर (gui.col + cells > Columns)
		    थांबा;
		this_len += (*mb_ptr2len)(s + this_len);
	    }
	    जर (this_len > len)
		this_len = len;	    // don't include following composing char
	}
	अन्यथा जर (gui.col + len > Columns)
	    this_len = Columns - gui.col;
	अन्यथा
	    this_len = len;

	(शून्य)gui_outstr_nowrap(s, this_len,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	s += this_len;
	len -= this_len;
	// fill up for a double-width char that doesn't fit.
	जर (len > 0 && gui.col < Columns)
	    (शून्य)gui_outstr_nowrap((char_u *)" ", 1,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	// The cursor may wrap to the next line.
	जर (gui.col >= Columns)
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
    स्थिर पूर्णांक
gui_screenchar(
    पूर्णांक		off,	    // Offset from start of screen
    पूर्णांक		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    पूर्णांक		back)	    // backup this many chars when using bold trick
{
    char_u	buf[MB_MAXBYTES + 1];

    // Don't draw right half of a double-width UTF-8 char. "cannot happen"
    जर (enc_utf8 && Screen_ओळी[off] == 0)
	परत OK;

    जर (enc_utf8 && Screen_ओळी_UC[off] != 0)
	// Draw UTF-8 multi-byte character.
	परत gui_outstr_nowrap(buf, utfc_char2bytes(off, buf),
							 flags, fg, bg, back);

    जर (enc_dbcs == DBCS_JPNU && Screen_ओळी[off] == 0x8e)
    {
	buf[0] = Screen_ओळी[off];
	buf[1] = ScreenLines2[off];
	परत gui_outstr_nowrap(buf, 2, flags, fg, bg, back);
    }

    // Draw non-multi-byte character or DBCS character.
    परत gui_outstr_nowrap(Screen_ओळी + off,
	    enc_dbcs ? (*mb_ptr2len)(Screen_ओळी + off) : 1,
							 flags, fg, bg, back);
}
#endif

#ifdef FEAT_GUI_GTK
/*
 * Output the string at the given screen position.  This is used in place
 * of gui_screenchar() where possible because Pango needs as much context
 * as possible to work nicely.  It's a lot faster as well.
 */
    स्थिर पूर्णांक
gui_screenstr(
    पूर्णांक		off,	    // Offset from start of screen
    पूर्णांक		len,	    // string length in screen cells
    पूर्णांक		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    पूर्णांक		back)	    // backup this many chars when using bold trick
{
    char_u  *buf;
    पूर्णांक	    outlen = 0;
    पूर्णांक	    i;
    पूर्णांक	    retval;

    जर (len <= 0) // "cannot happen"?
	परत OK;

    जर (enc_utf8)
    {
	buf = alloc(len * MB_MAXBYTES + 1);
	जर (buf == रिक्त)
	    परत OK; // not much we could do here...

	साठी (i = off; i < off + len; ++i)
	{
	    जर (Screen_ओळी[i] == 0)
		सुरूठेवा; // skip second half of double-width char

	    जर (Screen_ओळी_UC[i] == 0)
		buf[outlen++] = Screen_ओळी[i];
	    अन्यथा
		outlen += utfc_char2bytes(i, buf + outlen);
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, back);
	vim_free(buf);

	परत retval;
    }
    अन्यथा जर (enc_dbcs == DBCS_JPNU)
    {
	buf = alloc(len * 2 + 1);
	जर (buf == रिक्त)
	    परत OK; // not much we could do here...

	साठी (i = off; i < off + len; ++i)
	{
	    buf[outlen++] = Screen_ओळी[i];

	    // handle double-byte single-width char
	    जर (Screen_ओळी[i] == 0x8e)
		buf[outlen++] = ScreenLines2[i];
	    अन्यथा जर (MB_BYTE2LEN(Screen_ओळी[i]) == 2)
		buf[outlen++] = Screen_ओळी[++i];
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, back);
	vim_free(buf);

	परत retval;
    }
    अन्यथा
    {
	परत gui_outstr_nowrap(&Screen_ओळी[off], len,
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
    स्थिर पूर्णांक
gui_outstr_nowrap(
    char_u	*s,
    पूर्णांक		len,
    पूर्णांक		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    पूर्णांक		back)	    // backup this many chars when using bold trick
{
    long_u	highlight_mask;
    long_u	hl_mask_todo;
    guicolor_T	fg_color;
    guicolor_T	bg_color;
    guicolor_T	sp_color;
#जर !defined(FEAT_GUI_GTK)
    GuiFont	font = NOFONT;
    GuiFont	wide_font = NOFONT;
# ifdef FEAT_XFONTSET
    GuiFontset	fontset = NOFONTSET;
# endif
#endif
    attrentry_T	*aep = रिक्त;
    पूर्णांक		draw_flags;
    पूर्णांक		col = gui.col;
#ifdef FEAT_SIGN_ICONS
    पूर्णांक		draw_sign = FALSE;
    पूर्णांक		signcol = 0;
    char_u	extra[18];
# ifdef FEAT_NETBEANS_INTG
    पूर्णांक		multi_sign = FALSE;
# endif
#endif

    जर (len < 0)
	len = (पूर्णांक)STRLEN(s);
    जर (len == 0)
	परत OK;

#ifdef FEAT_SIGN_ICONS
    जर (*s == SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
	  || *s == MULTISIGN_BYTE
# endif
       )
    {
	माप_प्रकार	slen = 2;		    // 2 spaces by default

# ifdef FEAT_NETBEANS_INTG
	जर (*s == MULTISIGN_BYTE)
	    multi_sign = TRUE;
# endif
	// draw spaces instead
	जर (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u' &&
		(curwin->w_p_nu || curwin->w_p_rnu))
	{
	    पूर्णांक	n = संख्या_width(curwin);

	    slen = MIN(n, (पूर्णांक)आकार(extra) - 1);
	}
	vim_memset(extra, ' ', slen);
	extra[slen] = NUL;
	s = extra;
	जर (len == 1 && col > 0)
	    --col;
	len = (पूर्णांक)slen;
	जर (len > 2)
	    // right align sign icon in the number column
	    signcol = col + len - 3;
	अन्यथा
	    signcol = col;
	draw_sign = TRUE;
	highlight_mask = 0;
    }
    अन्यथा
#endif
    जर (gui.highlight_mask > HL_ALL)
    {
	aep = syn_gui_attr2entry(gui.highlight_mask);
	जर (aep == रिक्त)	    // highlighting not set
	    highlight_mask = 0;
	अन्यथा
	    highlight_mask = aep->ae_attr;
    }
    अन्यथा
	highlight_mask = gui.highlight_mask;
    hl_mask_todo = highlight_mask;

#जर !defined(FEAT_GUI_GTK)
    // Set the font
    जर (aep != रिक्त && aep->ae_u.gui.font != NOFONT)
	font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
    अन्यथा जर (aep != रिक्त && aep->ae_u.gui.fontset != NOFONTSET)
	fontset = aep->ae_u.gui.fontset;
# endif
    अन्यथा
    {
# ifdef FEAT_XFONTSET
	जर (gui.fontset != NOFONTSET)
	    fontset = gui.fontset;
	अन्यथा
# endif
	    जर (hl_mask_todo & (HL_BOLD | HL_STANDOUT))
	{
	    जर ((hl_mask_todo & HL_ITALIC) && gui.boldital_font != NOFONT)
	    {
		font = gui.boldital_font;
		hl_mask_todo &= ~(HL_BOLD | HL_STANDOUT | HL_ITALIC);
	    }
	    अन्यथा जर (gui.bold_font != NOFONT)
	    {
		font = gui.bold_font;
		hl_mask_todo &= ~(HL_BOLD | HL_STANDOUT);
	    }
	    अन्यथा
		font = gui.norm_font;
	}
	अन्यथा जर ((hl_mask_todo & HL_ITALIC) && gui.ital_font != NOFONT)
	{
	    font = gui.ital_font;
	    hl_mask_todo &= ~HL_ITALIC;
	}
	अन्यथा
	    font = gui.norm_font;

	/*
	 * Choose correct wide_font by font.  wide_font should be set with font
	 * at same time in above block.  But it will make many "ifdef" nasty
	 * blocks.  So we do it here.
	 */
	जर (font == gui.boldital_font && gui.wide_boldital_font)
	    wide_font = gui.wide_boldital_font;
	अन्यथा जर (font == gui.bold_font && gui.wide_bold_font)
	    wide_font = gui.wide_bold_font;
	अन्यथा जर (font == gui.ital_font && gui.wide_ital_font)
	    wide_font = gui.wide_ital_font;
	अन्यथा जर (font == gui.norm_font && gui.wide_font)
	    wide_font = gui.wide_font;
    }
# ifdef FEAT_XFONTSET
    जर (fontset != NOFONTSET)
	gui_mch_set_fontset(fontset);
    अन्यथा
# endif
	gui_mch_set_font(font);
#endif

    draw_flags = 0;

    // Set the color
    bg_color = gui.back_pixel;
    जर ((flags & GUI_MON_IS_CURSOR) && gui.in_focus)
    {
	draw_flags |= DRAW_CURSOR;
	fg_color = fg;
	bg_color = bg;
	sp_color = fg;
    }
    अन्यथा जर (aep != रिक्त)
    {
	fg_color = aep->ae_u.gui.fg_color;
	जर (fg_color == INVALCOLOR)
	    fg_color = gui.norm_pixel;
	bg_color = aep->ae_u.gui.bg_color;
	जर (bg_color == INVALCOLOR)
	    bg_color = gui.back_pixel;
	sp_color = aep->ae_u.gui.sp_color;
	जर (sp_color == INVALCOLOR)
	    sp_color = fg_color;
    }
    अन्यथा
    {
	fg_color = gui.norm_pixel;
	sp_color = fg_color;
    }

    जर (highlight_mask & (HL_INVERSE | HL_STANDOUT))
    {
	gui_mch_set_fg_color(bg_color);
	gui_mch_set_bg_color(fg_color);
    }
    अन्यथा
    {
	gui_mch_set_fg_color(fg_color);
	gui_mch_set_bg_color(bg_color);
    }
    gui_mch_set_sp_color(sp_color);

    // Clear the selection if we are about to write over it
    जर (!(flags & GUI_MON_NOCLEAR))
	clip_may_clear_selection(gui.row, gui.row);


    // If there's no bold font, then fake it
    जर (hl_mask_todo & (HL_BOLD | HL_STANDOUT))
	draw_flags |= DRAW_BOLD;

    /*
     * When drawing bold or italic characters the spill-over from the left
     * neighbor may be destroyed.  Let the caller backup to start redrawing
     * just after a blank.
     */
    जर (back != 0 && ((draw_flags & DRAW_BOLD) || (highlight_mask & HL_ITALIC)))
	परत FAIL;

#जर defined(FEAT_GUI_GTK)
    // If there's no italic font, then fake it.
    // For GTK2, we don't need a different font for italic style.
    जर (hl_mask_todo & HL_ITALIC)
	draw_flags |= DRAW_ITALIC;

    // Do we underline the text?
    जर (hl_mask_todo & HL_UNDERLINE)
	draw_flags |= DRAW_UNDERL;

#अन्यथा
    // Do we underline the text?
    जर ((hl_mask_todo & HL_UNDERLINE) || (hl_mask_todo & HL_ITALIC))
	draw_flags |= DRAW_UNDERL;
#endif
    // Do we undercurl the text?
    जर (hl_mask_todo & HL_UNDERCURL)
	draw_flags |= DRAW_UNDERC;

    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

    // Do we strikethrough the text?
    जर (hl_mask_todo & HL_STRIKETHROUGH)
	draw_flags |= DRAW_STRIKE;

    // Do we draw transparently?
    जर (flags & GUI_MON_TRS_CURSOR)
	draw_flags |= DRAW_TRANSP;

    /*
     * Draw the text.
     */
#ifdef FEAT_GUI_GTK
    // The value returned is the length in display cells
    len = gui_gtk_draw_string(gui.row, col, s, len, draw_flags);
#अन्यथा
    जर (enc_utf8)
    {
	पूर्णांक	start;		// index of bytes to be drawn
	पूर्णांक	cells;		// cellwidth of bytes to be drawn
	पूर्णांक	thislen;	// length of bytes to be drawn
	पूर्णांक	cn;		// cellwidth of current char
	पूर्णांक	i;		// index of current char
	पूर्णांक	c;		// current char value
	पूर्णांक	cl;		// byte length of current char
	पूर्णांक	comping;	// current char is composing
	पूर्णांक	scol = col;	// screen column
	पूर्णांक	curr_wide = FALSE;  // use 'guifontwide'
	पूर्णांक	prev_wide = FALSE;
	पूर्णांक	wide_changed;
# ifdef MSWIN
	पूर्णांक	sep_comp = FALSE;   // Don't separate composing chars.
# अन्यथा
	पूर्णांक	sep_comp = TRUE;    // Separate composing chars.
# endif

	// Break the string at a composing character, it has to be drawn on
	// top of the previous character.
	start = 0;
	cells = 0;
	साठी (i = 0; i < len; i += cl)
	{
	    c = utf_ptr2char(s + i);
	    cn = utf_char2cells(c);
	    comping = utf_iscomposing(c);
	    जर (!comping)	// count cells from non-composing chars
		cells += cn;
	    जर (!comping || sep_comp)
	    {
		जर (cn > 1
# ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
# endif
			&& wide_font != NOFONT)
		    curr_wide = TRUE;
		अन्यथा
		    curr_wide = FALSE;
	    }
	    cl = utf_ptr2len(s + i);
	    जर (cl == 0)	// hit end of string
		len = i + cl;	// len must be wrong "cannot happen"

	    wide_changed = curr_wide != prev_wide;

	    // Print the string so far if it's the last character or there is
	    // a composing character.
	    जर (i + cl >= len || (comping && sep_comp && i > start)
		    || wide_changed
# जर defined(FEAT_GUI_X11)
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
		जर ((comping && sep_comp) || wide_changed)
		    thislen = i - start;
		अन्यथा
		    thislen = i - start + cl;
		जर (thislen > 0)
		{
		    जर (prev_wide)
			gui_mch_set_font(wide_font);
		    gui_mch_draw_string(gui.row, scol, s + start, thislen,
								  draw_flags);
		    जर (prev_wide)
			gui_mch_set_font(font);
		    start += thislen;
		}
		scol += cells;
		cells = 0;
		// Adjust to not draw a character which width is changed
		// against with last one.
		जर (wide_changed && !(comping && sep_comp))
		{
		    scol -= cn;
		    cl = 0;
		}

# जर defined(FEAT_GUI_X11)
		// No fontset: draw a space to fill the gap after a wide char
		//
		जर (cn > 1 && (draw_flags & DRAW_TRANSP) == 0
#  ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
#  endif
			&& !wide_changed)
		    gui_mch_draw_string(gui.row, scol - 1, (char_u *)" ",
							       1, draw_flags);
# endif
	    }
	    // Draw a composing char on top of the previous char.
	    जर (comping && sep_comp)
	    {
# जर defined(__APPLE_CC__) && TARGET_API_MAC_CARBON
		// Carbon ATSUI autodraws composing char over previous char
		gui_mch_draw_string(gui.row, scol, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# अन्यथा
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
    अन्यथा
    {
	gui_mch_draw_string(gui.row, col, s, len, draw_flags);
	जर (enc_dbcs == DBCS_JPNU)
	{
	    // Get the length in display cells, this can be different from the
	    // number of bytes for "euc-jp".
	    len = mb_string2cells(s, len);
	}
    }
#endif // !FEAT_GUI_GTK

    जर (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
	gui.col = col + len;

    // May need to invert it when it's part of the selection.
    जर (flags & GUI_MON_NOCLEAR)
	clip_may_redraw_selection(gui.row, col, len);

    जर (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
    {
	// Invalidate the old physical cursor position if we wrote over it
	जर (gui.cursor_row == gui.row
		&& gui.cursor_col >= col
		&& gui.cursor_col < col + len)
	    gui.cursor_is_valid = false;
    }

#ifdef FEAT_SIGN_ICONS
    जर (draw_sign)
	// Draw the sign on top of the spaces.
	gui_mch_drawsign(gui.row, signcol, gui.highlight_mask);
# जर defined(FEAT_NETBEANS_INTG) && (defined(FEAT_GUI_X11) \
	|| defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN))
    जर (multi_sign)
	netbeans_draw_multisign_indicator(gui.row);
# endif
#endif

    परत OK;
}

/*
 * Undraw the cursor.  This actually redraws the character at the cursor
 * position, plus some more characters when needed.
 */
    शून्य
gui_undraw_cursor(शून्य)
{
    जर (!gui.cursor_is_valid)
	परत;

    // Always redraw the character just before if there is one, because
    // with some fonts and characters there can be a one pixel overlap.
    पूर्णांक startcol = gui.cursor_col > 0 ? gui.cursor_col - 1 : gui.cursor_col;
    पूर्णांक endcol = gui.cursor_col;

#जर defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    gui_adjust_undraw_cursor_for_ligatures(&startcol, &endcol);
#endif
    gui_redraw_block(gui.cursor_row, startcol,
	    gui.cursor_row, endcol, GUI_MON_NOCLEAR);

    // Cursor_is_valid is reset when the cursor is undrawn, also reset it
    // here in case it wasn't needed to undraw it.
    gui.cursor_is_valid = false;
}

    शून्य
gui_redraw(
    पूर्णांक		x,
    पूर्णांक		y,
    पूर्णांक		w,
    पूर्णांक		h)
{
    पूर्णांक		row1, col1, row2, col2;

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
    जर (gui.row == gui.cursor_row)
	gui_update_cursor(TRUE, TRUE);
}

/*
 * Draw a rectangular block of characters, from row1 to row2 (inclusive) and
 * from col1 to col2 (inclusive).
 */
    शून्य
gui_redraw_block(
    पूर्णांक		row1,
    पूर्णांक		col1,
    पूर्णांक		row2,
    पूर्णांक		col2,
    पूर्णांक		flags)	// flags for gui_outstr_nowrap()
{
    पूर्णांक		old_row, old_col;
    long_u	old_hl_mask;
    पूर्णांक		off;
    sattr_T	first_attr;
    पूर्णांक		idx, len;
    पूर्णांक		back, nback;
    पूर्णांक		orig_col1, orig_col2;

    // Don't try to update when ScreenLines is not valid
    जर (!screen_cleared || Screen_ओळी == रिक्त)
	परत;

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

    साठी (gui.row = row1; gui.row <= row2; gui.row++)
    {
	// When only half of a double-wide character is in the block, include
	// the other half.
	col1 = orig_col1;
	col2 = orig_col2;
	off = ओळ_Offset[gui.row];
	जर (enc_dbcs != 0)
	{
	    जर (col1 > 0)
		col1 -= dbcs_screen_head_off(Screen_ओळी + off,
						    Screen_ओळी + off + col1);
	    col2 += dbcs_screen_tail_off(Screen_ओळी + off,
						    Screen_ओळी + off + col2);
	}
	अन्यथा जर (enc_utf8)
	{
	    जर (Screen_ओळी[off + col1] == 0)
	    {
		जर (col1 > 0)
		    --col1;
		अन्यथा
		    // FIXME: how can the first character ever be zero?
		    siemsg("NUL in ScreenLines in row %ld", (दीर्घ)gui.row);
	    }
#ifdef FEAT_GUI_GTK
	    जर (col2 + 1 < Columns && Screen_ओळी[off + col2 + 1] == 0)
		++col2;
#endif
	}
	gui.col = col1;
	off = ओळ_Offset[gui.row] + gui.col;
	len = col2 - col1 + 1;

	// Find how many chars back this highlighting starts, or where a space
	// is.  Needed for when the bold trick is used
	साठी (back = 0; back < col1; ++back)
	    जर (ScreenAttrs[off - 1 - back] != ScreenAttrs[off]
		    || Screen_ओळी[off - 1 - back] == ' ')
		थांबा;

	// Break it up in strings of characters with the same attributes.
	// Print UTF-8 characters individually.
	पर्यंत (len > 0)
	{
	    first_attr = ScreenAttrs[off];
	    gui.highlight_mask = first_attr;
#जर !defined(FEAT_GUI_GTK)
	    जर (enc_utf8 && Screen_ओळी_UC[off] != 0)
	    {
		// output multi-byte character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
		जर (gui.col < Columns && Screen_ओळी[off + 1] == 0)
		    idx = 2;
		अन्यथा
		    idx = 1;
	    }
	    अन्यथा जर (enc_dbcs == DBCS_JPNU && Screen_ओळी[off] == 0x8e)
	    {
		// output double-byte, single-width character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
		idx = 1;
	    }
	    अन्यथा
#endif
	    {
#ifdef FEAT_GUI_GTK
		साठी (idx = 0; idx < len; ++idx)
		{
		    जर (enc_utf8 && Screen_ओळी[off + idx] == 0)
			सुरूठेवा; // skip second half of double-width char
		    जर (ScreenAttrs[off + idx] != first_attr)
			थांबा;
		}
		// gui_screenstr() takes care of multibyte chars
		nback = gui_screenstr(off, idx, flags,
				      (guicolor_T)0, (guicolor_T)0, back);
#अन्यथा
		साठी (idx = 0; idx < len && ScreenAttrs[off + idx] == first_attr;
									idx++)
		{
		    // Stop at a multi-byte Unicode character.
		    जर (enc_utf8 && Screen_ओळी_UC[off + idx] != 0)
			थांबा;
		    जर (enc_dbcs == DBCS_JPNU)
		    {
			// Stop at a double-byte single-width char.
			जर (Screen_ओळी[off + idx] == 0x8e)
			    थांबा;
			जर (len > 1 && (*mb_ptr2len)(Screen_ओळी
							    + off + idx) == 2)
			    ++idx;  // skip second byte of double-byte char
		    }
		}
		nback = gui_outstr_nowrap(Screen_ओळी + off, idx, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
#endif
	    }
	    जर (nback == FAIL)
	    {
		// Must back up to start drawing where a bold or italic word
		// starts.
		off -= back;
		len += back;
		gui.col -= back;
	    }
	    अन्यथा
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
    gui.highlight_mask = (पूर्णांक)old_hl_mask;
}

    स्थिर शून्य
gui_delete_ओळी(पूर्णांक row, पूर्णांक मोजणी)
{
    जर (मोजणी <= 0)
	परत;

    जर (row + मोजणी > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_block(row, gui.scroll_region_डावा,
			      gui.scroll_region_bot, gui.scroll_region_उजवा);
    अन्यथा
    {
	gui_mch_delete_ओळी(row, मोजणी);

	// If the cursor was in the deleted lines it's now gone.  If the
	// cursor was in the scrolled lines adjust its position.
	जर (gui.cursor_row >= row
		&& gui.cursor_col >= gui.scroll_region_डावा
		&& gui.cursor_col <= gui.scroll_region_उजवा)
	{
	    जर (gui.cursor_row < row + मोजणी)
		gui.cursor_is_valid = false;
	    अन्यथा जर (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_row -= मोजणी;
	}
    }
}

    स्थिर शून्य
gui_घाला_ओळी(पूर्णांक row, पूर्णांक मोजणी)
{
    जर (मोजणी <= 0)
	परत;

    जर (row + मोजणी > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_block(row, gui.scroll_region_डावा,
			      gui.scroll_region_bot, gui.scroll_region_उजवा);
    अन्यथा
    {
	gui_mch_घाला_ओळी(row, मोजणी);

	जर (gui.cursor_row >= gui.row
		&& gui.cursor_col >= gui.scroll_region_डावा
		&& gui.cursor_col <= gui.scroll_region_उजवा)
	{
	    जर (gui.cursor_row <= gui.scroll_region_bot - मोजणी)
		gui.cursor_row += मोजणी;
	    अन्यथा जर (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_is_valid = false;
	}
    }
}

#ifdef FEAT_TIMERS
/*
 * Passed to ui_wait_for_chars_or_timer(), ignoring extra arguments.
 */
    स्थिर पूर्णांक
gui_wait_for_chars_3(
    दीर्घ wtime,
    पूर्णांक *interrupted UNUSED,
    पूर्णांक ignore_आदान UNUSED)
{
    परत gui_mch_wait_for_chars(wtime);
}
#endif

/*
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    स्थिर पूर्णांक
gui_wait_for_chars_or_timer(
	दीर्घ wtime,
	पूर्णांक *interrupted UNUSED,
	पूर्णांक ignore_आदान UNUSED)
{
#ifdef FEAT_TIMERS
    परत ui_wait_for_chars_or_timer(wtime, gui_wait_for_chars_3,
						    interrupted, ignore_आदान);
#अन्यथा
    परत gui_mch_wait_for_chars(wtime);
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
    स्थिर पूर्णांक
gui_wait_for_chars_buf(
    char_u	*buf,
    पूर्णांक		maxlen,
    दीर्घ	wtime,	    // don't use "time", MIPS cannot handle it
    पूर्णांक		tb_change_cnt)
{
    पूर्णांक	    retval;
    पूर्णांक	    keep_blinking; // Guard against restarting blink cycle on CursorHold

#ifdef FEAT_MENU
    // If we're going to wait a bit, update the menus and mouse shape for the
    // current State.
    जर (wtime != 0)
	gui_update_menus(0);
#endif

    gui_mch_update();
    जर (आदान_available())	// Got char, return immediately
    {
	जर (gui_mch_is_blinking())
	    gui_mch_stop_blink(TRUE);
	जर (buf != रिक्त && !typebuf_changed(tb_change_cnt))
	    परत read_from_आदान_buf(buf, (दीर्घ)maxlen);
	परत 0;
    }
    जर (wtime == 0)		// Don't wait for char
	परत FAIL;

    // Before waiting, flush any output to the screen.
    gui_mch_flush();

    // Blink while waiting for a character.
    जर (!gui_mch_is_blinking())
	gui_mch_start_blink();

    // Common function to loop until "wtime" is met, while handling timers and
    // other callbacks.
    retval = inchar_loop(buf, maxlen, wtime, tb_change_cnt,
			 gui_wait_for_chars_or_timer, रिक्त);

    // Keep blinking when CursorHold wakes the input loop. (See PR #21115)
    keep_blinking = retval == 3 && buf != रिक्त
	&& buf[0] == K_SPECIAL && buf[1] == KS_EXTRA
	&& buf[2] == (पूर्णांक)KE_CURSORHOLD;

    जर (!keep_blinking)
	gui_mch_stop_blink(TRUE);

    परत retval;
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
    पूर्णांक
gui_wait_for_chars(दीर्घ wtime, पूर्णांक tb_change_cnt)
{
    परत gui_wait_for_chars_buf(रिक्त, 0, wtime, tb_change_cnt);
}

/*
 * Equivalent of mch_inchar() for the GUI.
 */
    पूर्णांक
gui_inchar(
    char_u  *buf,
    पूर्णांक	    maxlen,
    दीर्घ    wtime,		// milliseconds
    पूर्णांक	    tb_change_cnt)
{
    परत gui_wait_for_chars_buf(buf, maxlen, wtime, tb_change_cnt);
}

/*
 * Fill p[4] with mouse coordinates encoded for check_termcode().
 */
    स्थिर शून्य
fill_mouse_coord(char_u *p, पूर्णांक col, पूर्णांक row)
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
    शून्य
gui_send_mouse_event(
    पूर्णांक	    button,
    पूर्णांक	    x,
    पूर्णांक	    y,
    पूर्णांक	    repeated_click,
    int_u   modifiers)
{
    स्थिर पूर्णांक	    prev_row = 0, prev_col = 0;
    स्थिर पूर्णांक	    prev_button = -1;
    स्थिर पूर्णांक	    अंक_clicks = 1;
    char_u	    string[10];
    गणना किल्ली_extra  button_char;
    पूर्णांक		    row, col;
#ifdef FEAT_CLIPBOARD
    पूर्णांक		    checkfor;
    पूर्णांक		    did_clip = FALSE;
#endif

    /*
     * Scrolling may happen at any time, also while a selection is present.
     */
    निवड (button)
    {
	स्थिती MOUSE_MOVE:
	    button_char = KE_MOUSEMOVE_XY;
	    जा button_set;
	स्थिती MOUSE_X1:
	    button_char = KE_X1MOUSE;
	    जा button_set;
	स्थिती MOUSE_X2:
	    button_char = KE_X2MOUSE;
	    जा button_set;
	स्थिती MOUSE_4:
	    button_char = KE_MOUSEDOWN;
	    जा button_set;
	स्थिती MOUSE_5:
	    button_char = KE_MOUSEUP;
	    जा button_set;
	स्थिती MOUSE_6:
	    button_char = KE_MOUSELEFT;
	    जा button_set;
	स्थिती MOUSE_7:
	    button_char = KE_MOUSERIGHT;
button_set:
	    {
		// Don't put events in the input queue now.
		जर (hold_gui_events)
		    परत;

		row = gui_xy2colrow(x, y, &col);
		// Don't report a mouse move unless moved to a
		// different character position.
		जर (button == MOUSE_MOVE)
		{
		    जर (row == prev_row && col == prev_col)
			परत;
		    अन्यथा
		    {
			prev_row = row >= 0 ? row : 0;
			prev_col = col;
		    }
		}

		string[3] = CSI;
		string[4] = KS_EXTRA;
		string[5] = (पूर्णांक)button_char;

		// Pass the pointer coordinates of the scroll event so that we
		// know which window to scroll.
		string[6] = (char_u)(col / 128 + ' ' + 1);
		string[7] = (char_u)(col % 128 + ' ' + 1);
		string[8] = (char_u)(row / 128 + ' ' + 1);
		string[9] = (char_u)(row % 128 + ' ' + 1);

		जर (modifiers == 0)
		    add_to_आदान_buf(string + 3, 7);
		अन्यथा
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = 0;
		    जर (modifiers & MOUSE_SHIFT)
			string[2] |= MOD_MASK_SHIFT;
		    जर (modifiers & MOUSE_CTRL)
			string[2] |= MOD_MASK_CTRL;
		    जर (modifiers & MOUSE_ALT)
			string[2] |= MOD_MASK_ALT;
		    add_to_आदान_buf(string, 10);
		}
		परत;
	    }
    }

#ifdef FEAT_CLIPBOARD
    // If a clipboard selection is in progress, handle it
    जर (clip_star.state == SELECT_IN_PROGRESS)
    {
	clip_process_selection(button, X_2_COL(x), Y_2_ROW(y), repeated_click);

	// A release event may still need to be sent if the position is equal.
	row = gui_xy2colrow(x, y, &col);
	जर (button != MOUSE_RELEASE || row != prev_row || col != prev_col)
	    परत;
    }

    // Determine which mouse settings to look for based on the current mode
    निवड (get_real_state())
    {
	स्थिती MODE_NORMAL_BUSY:
	स्थिती MODE_OP_PENDING:
# ifdef FEAT_TERMINAL
	स्थिती MODE_TERMINAL:
# endif
	स्थिती MODE_NORMAL:	checkfor = MOUSE_NORMAL; थांबा;
	स्थिती MODE_VISUAL:	checkfor = MOUSE_VISUAL; थांबा;
	स्थिती MODE_SELECT:	checkfor = MOUSE_VISUAL; थांबा;
	स्थिती MODE_REPLACE:
	स्थिती MODE_REPLACE | MODE_LANGMAP:
	स्थिती MODE_VREPLACE:
	स्थिती MODE_VREPLACE | MODE_LANGMAP:
	स्थिती MODE_घाला:
	स्थिती MODE_घाला | MODE_LANGMAP:
				checkfor = MOUSE_घाला; थांबा;
	स्थिती MODE_ASKMORE:
	स्थिती MODE_HITRETURN:	// At the more- and hit-enter prompt pass the
				// mouse event for a click on or below the
				// message line.
				जर (Y_2_ROW(y) >= संदेश_row)
				    checkfor = MOUSE_NORMAL;
				अन्यथा
				    checkfor = MOUSE_RETURN;
				थांबा;

	    /*
	     * On the command line, use the clipboard selection on all lines
	     * but the command line.  But not when pasting.
	     */
	स्थिती MODE_CMDLINE:
	स्थिती MODE_CMDLINE | MODE_LANGMAP:
	    जर (Y_2_ROW(y) < cmdline_row && button != MOUSE_मधला)
		checkfor = MOUSE_NONE;
	    अन्यथा
		checkfor = MOUSE_COMMAND;
	    थांबा;

	पूर्वनिर्धारित:
	    checkfor = MOUSE_NONE;
	    थांबा;
    };

    /*
     * Allow clipboard selection of text on the command line in "normal"
     * modes.  Don't do this when dragging the status line, or extending a
     * Visual selection.
     */
    जर ((State == MODE_NORMAL || State == MODE_NORMAL_BUSY
						      || (State & MODE_घाला))
	    && X_2_COL(x) >= firstwin->w_wincol
	    && X_2_COL(x) < firstwin->w_wincol + topframe->fr_width
	    && Y_2_ROW(y) >= topframe->fr_height + firstwin->w_winrow
	    && button != MOUSE_DRAG
# ifdef FEAT_MOUSESHAPE
	    && !drag_status_ओळ
	    && !drag_sep_ओळ
# endif
	    )
	checkfor = MOUSE_NONE;

    /*
     * Use modeless selection when holding CTRL and SHIFT pressed.
     */
    जर ((modifiers & MOUSE_CTRL) && (modifiers & MOUSE_SHIFT))
	checkfor = MOUSE_NONEF;

    /*
     * In Ex mode, always use modeless selection.
     */
    जर (exmode_active)
	checkfor = MOUSE_NONE;

    /*
     * If the mouse settings say to not use the mouse, use the modeless
     * selection.  But if Visual is active, assume that only the Visual area
     * will be selected.
     * Exception: On the command line, both the selection is used and a mouse
     * key is sent.
     */
    जर (!mouse_has(checkfor) || checkfor == MOUSE_COMMAND)
    {
	// Don't do modeless selection in Visual mode.
	जर (checkfor != MOUSE_NONEF && VIsual_active && (State & MODE_NORMAL))
	    परत;

	/*
	 * When 'mousemodel' is "popup", shift-left is translated to right.
	 * But not when also using Ctrl.
	 */
	जर (mouse_model_popup() && button == MOUSE_डावा
		&& (modifiers & MOUSE_SHIFT) && !(modifiers & MOUSE_CTRL))
	{
	    button = MOUSE_उजवा;
	    modifiers &= ~ MOUSE_SHIFT;
	}

	// If the selection is done, allow the right button to extend it.
	// If the selection is cleared, allow the right button to start it
	// from the cursor position.
	जर (button == MOUSE_उजवा)
	{
	    जर (clip_star.state == SELECT_CLEARED)
	    {
		जर (State & MODE_CMDLINE)
		{
		    col = संदेश_col;
		    row = संदेश_row;
		}
		अन्यथा
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
	अन्यथा जर (button == MOUSE_डावा)
	{
	    clip_start_selection(X_2_COL(x), Y_2_ROW(y), repeated_click);
	    did_clip = TRUE;
	}

	// Always allow pasting
	जर (button != MOUSE_मधला)
	{
	    जर (!mouse_has(checkfor) || button == MOUSE_RELEASE)
		परत;
	    जर (checkfor != MOUSE_COMMAND)
		button = MOUSE_डावा;
	}
	repeated_click = FALSE;
    }

    जर (clip_star.state != SELECT_CLEARED && !did_clip)
	clip_clear_selection(&clip_star);
#endif

    // Don't put events in the input queue now.
    जर (hold_gui_events)
	परत;

    row = gui_xy2colrow(x, y, &col);

    /*
     * If we are dragging and the mouse hasn't moved far enough to be on a
     * different character, then don't send an event to vim.
     */
    जर (button == MOUSE_DRAG)
    {
	जर (row == prev_row && col == prev_col)
	    परत;
	// Dragging above the window, set "row" to -1 to cause a scroll.
	जर (y < 0)
	    row = -1;
    }

    /*
     * If topline has changed (window scrolled) since the last click, reset
     * repeated_click, because we don't want starting Visual mode when
     * clicking on a different character in the text.
     */
    जर (curwin->w_topline != gui_prev_topline
#ifdef FEAT_DIFF
	    || curwin->w_topfill != gui_prev_topfill
#endif
	    )
	repeated_click = FALSE;

    string[0] = CSI;	// this sequence is recognized by check_termcode()
    string[1] = KS_MOUSE;
    string[2] = KE_FILLER;
    जर (button != MOUSE_DRAG && button != MOUSE_RELEASE)
    {
	जर (repeated_click)
	{
	    /*
	     * Handle multiple clicks.	They only count if the mouse is still
	     * pointing at the same character.
	     */
	    जर (button != prev_button || row != prev_row || col != prev_col)
		अंक_clicks = 1;
	    अन्यथा जर (++अंक_clicks > 4)
		अंक_clicks = 1;
	}
	अन्यथा
	    अंक_clicks = 1;
	prev_button = button;
	gui_prev_topline = curwin->w_topline;
#ifdef FEAT_DIFF
	gui_prev_topfill = curwin->w_topfill;
#endif

	string[3] = (char_u)(button | 0x20);
	SET_अंक_MOUSE_CLICKS(string[3], अंक_clicks);
    }
    अन्यथा
	string[3] = (char_u)button;

    string[3] |= modifiers;
    fill_mouse_coord(string + 4, col, row);
    add_to_आदान_buf(string, 8);

    जर (row < 0)
	prev_row = 0;
    अन्यथा
	prev_row = row;
    prev_col = col;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#जर defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#endif
}

/*
 * Convert x and y coordinate to column and row in text window.
 * Corrects for multi-byte character.
 * returns column in "*colp" and row as return value;
 */
    स्थिर पूर्णांक
gui_xy2colrow(पूर्णांक x, पूर्णांक y, पूर्णांक *colp)
{
    पूर्णांक		col = check_col(X_2_COL(x));
    पूर्णांक		row = check_row(Y_2_ROW(y));

    *colp = mb_fix_col(col, row);
    परत row;
}

#जर defined(FEAT_MENU)
/*
 * Callback function for when a menu entry has been selected.
 */
    शून्य
gui_menu_cb(vimmenu_T *menu)
{
    char_u  bytes[आकार(long_u)];

    // Don't put events in the input queue now.
    जर (hold_gui_events)
	परत;

    bytes[0] = CSI;
    bytes[1] = KS_MENU;
    bytes[2] = KE_FILLER;
    add_to_आदान_buf(bytes, 3);
    add_long_to_buf((long_u)menu, bytes);
    add_to_आदान_buf_csi(bytes, आकार(long_u));
}
#endif

स्थिर पूर्णांक	prev_which_scrollbars[3];

/*
 * Set which components are present.
 * If "oldval" is not NULL, "oldval" is the previous value, the new value is
 * in p_go.
 */
    शून्य
gui_init_which_components(char_u *oldval UNUSED)
{
#ifdef FEAT_GUI_DARKTHEME
    स्थिर पूर्णांक	prev_dark_theme = FALSE;
    पूर्णांक		using_dark_theme = FALSE;
#endif
#ifdef FEAT_MENU
    स्थिर पूर्णांक	prev_menu_is_active = -1;
#endif
#ifdef FEAT_TOOLBAR
    स्थिर पूर्णांक	prev_toolbar = -1;
    पूर्णांक		using_toolbar = FALSE;
#endif
#ifdef FEAT_GUI_TABLINE
    पूर्णांक		using_tabline;
#endif
#ifdef FEAT_GUI_MSWIN
    स्थिर पूर्णांक	prev_titlebar = FALSE;
    पूर्णांक		using_titlebar = FALSE;
#endif

#जर defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    स्थिर पूर्णांक	prev_fullscreen = FALSE;
    पूर्णांक		using_fullscreen = FALSE;
#endif
#जर defined(FEAT_MENU)
    स्थिर पूर्णांक	prev_tearoff = -1;
    पूर्णांक		using_tearoff = FALSE;
#endif

    char_u	*p;
    पूर्णांक		i;
#ifdef FEAT_MENU
    पूर्णांक		grey_old, grey_new;
    char_u	*temp;
#endif
    win_T	*wp;
    पूर्णांक		need_set_size;
    पूर्णांक		fix_size;

#ifdef FEAT_MENU
    जर (oldval != रिक्त && gui.in_use)
    {
	/*
	 * Check if the menus go from grey to non-grey or vice versa.
	 */
	grey_old = (vim_strchr(oldval, GO_GREY) != रिक्त);
	grey_new = (vim_strchr(p_go, GO_GREY) != रिक्त);
	जर (grey_old != grey_new)
	{
	    temp = p_go;
	    p_go = oldval;
	    gui_update_menus(MENU_ALL_MODES);
	    p_go = temp;
	}
    }
    gui.menu_is_active = false;
#endif

    साठी (i = 0; i < 3; i++)
	gui.which_scrollbars[i] = false;
    साठी (p = p_go; *p; p++)
	निवड (*p)
	{
	    स्थिती GO_डावा:
		gui.which_scrollbars[SBAR_डावा] = true;
		थांबा;
	    स्थिती GO_उजवा:
		gui.which_scrollbars[SBAR_उजवा] = true;
		थांबा;
	    स्थिती GO_VLEFT:
		जर (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_डावा] = true;
		थांबा;
	    स्थिती GO_VRIGHT:
		जर (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_उजवा] = true;
		थांबा;
	    स्थिती GO_BOT:
		gui.which_scrollbars[SBAR_BOTTOM] = true;
		थांबा;
#ifdef FEAT_GUI_DARKTHEME
	    स्थिती GO_DARKTHEME:
		using_dark_theme = TRUE;
		थांबा;
#endif
#ifdef FEAT_MENU
	    स्थिती GO_MENUS:
		gui.menu_is_active = true;
		थांबा;
#endif
	    स्थिती GO_GREY:
		// make menu's have grey items, ignored here
		थांबा;
#ifdef FEAT_GUI_MSWIN
	    स्थिती GO_TITLEBAR:
		using_titlebar = TRUE;
		थांबा;
#endif
#जर defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
	    स्थिती GO_FULLSCREEN:
		using_fullscreen = TRUE;
		थांबा;
#endif
#ifdef FEAT_TOOLBAR
	    स्थिती GO_TOOLBAR:
		using_toolbar = TRUE;
		थांबा;
#endif
	    स्थिती GO_TEAROFF:
#जर defined(FEAT_MENU)
		using_tearoff = TRUE;
#endif
		थांबा;
	    पूर्वनिर्धारित:
		// Ignore options that are not supported
		थांबा;
	}

    जर (!gui.in_use)
	परत;

    need_set_size = 0;
    fix_size = FALSE;

#ifdef FEAT_GUI_MSWIN
    जर (using_titlebar != prev_titlebar)
    {
	gui_mch_set_titlebar_colors();
	prev_titlebar = using_titlebar;
    }
#endif

#जर defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    जर (using_fullscreen != prev_fullscreen)
    {
	gui_mch_set_fullscreen(using_fullscreen);
	prev_fullscreen = using_fullscreen;
    }
#endif

#ifdef FEAT_GUI_DARKTHEME
    जर (using_dark_theme != prev_dark_theme)
    {
	gui_mch_set_dark_theme(using_dark_theme);
	prev_dark_theme = using_dark_theme;
    }
#endif

#ifdef FEAT_GUI_TABLINE
    // Update the GUI tab line, it may appear or disappear.  This may
    // cause the non-GUI tab line to disappear or appear.
    using_tabline = gui_has_tabline();
    जर (!gui_mch_showing_tabline() != !using_tabline)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = Rows;
	gui_update_tabline();
	Rows = i;
	need_set_size |= RESIZE_VERT;
	जर (using_tabline)
	    fix_size = TRUE;
	जर (!gui_use_tabline())
	    redraw_tabline = TRUE;    // may draw non-GUI tab line
    }
#endif

    साठी (i = 0; i < 3; i++)
    {
	// The scrollbar needs to be updated when it is shown/unshown and
	// when switching tab pages.  But the size only changes when it's
	// shown/unshown.  Thus we need two places to remember whether a
	// scrollbar is there or not.
	जर (gui.which_scrollbars[i] != prev_which_scrollbars[i]
		|| gui.which_scrollbars[i]
		!= curtab->tp_prev_which_scrollbars[i])
	{
	    जर (i == SBAR_BOTTOM)
		gui_mch_enable_scrollbar(&gui.bottom_sbar,
			gui.which_scrollbars[i]);
	    अन्यथा
	    {
		FOR_ALL_WINDOWS(wp)
		    gui_do_scrollbar(wp, i, gui.which_scrollbars[i]);
	    }
	    जर (gui.which_scrollbars[i] != prev_which_scrollbars[i])
	    {
		जर (i == SBAR_BOTTOM)
		    need_set_size |= RESIZE_VERT;
		अन्यथा
		    need_set_size |= RESIZE_HOR;
		जर (gui.which_scrollbars[i])
		    fix_size = TRUE;
	    }
	}
	curtab->tp_prev_which_scrollbars[i] = gui.which_scrollbars[i];
	prev_which_scrollbars[i] = gui.which_scrollbars[i];
    }

#ifdef FEAT_MENU
    जर (gui.menu_is_active != prev_menu_is_active)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = Rows;
	gui_mch_enable_menu(gui.menu_is_active);
	Rows = i;
	prev_menu_is_active = gui.menu_is_active;
	need_set_size |= RESIZE_VERT;
	जर (gui.menu_is_active)
	    fix_size = TRUE;
    }
#endif

#ifdef FEAT_TOOLBAR
    जर (using_toolbar != prev_toolbar)
    {
	gui_mch_show_toolbar(using_toolbar);
	prev_toolbar = using_toolbar;
	need_set_size |= RESIZE_VERT;
	जर (using_toolbar)
	    fix_size = TRUE;
    }
#endif
#जर defined(FEAT_MENU) && !(defined(MSWIN) && !defined(FEAT_TEAROFF))
    जर (using_tearoff != prev_tearoff)
    {
	gui_mch_toggle_tearoffs(using_tearoff);
	prev_tearoff = using_tearoff;
    }
#endif
    जर (need_set_size != 0)
    {
#ifdef FEAT_GUI_GTK
	दीर्घ    prev_Columns = Columns;
	दीर्घ    prev_Rows = Rows;
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
	    दीर्घ    post_Rows = Rows;
	    दीर्घ    post_Columns = Columns;

	    जर (!gui.starting && need_set_size != (RESIZE_VERT | RESIZE_HOR))
		(शून्य)char_avail();
	    जर ((need_set_size & RESIZE_VERT) == 0)
		Rows = prev_Rows;
	    अन्यथा
		Rows = post_Rows;
	    जर ((need_set_size & RESIZE_HOR) == 0)
		Columns = prev_Columns;
	    अन्यथा
		Columns = post_Columns;
	}
#endif
    }
    // When the console tabline appears or disappears the window positions
    // change.
    जर (firstwin->w_winrow != tabline_height())
	shell_new_rows();	// recompute window positions and heights
}

#जर defined(FEAT_GUI_TABLINE)
/*
 * Return TRUE if the GUI is taking care of the tabline.
 * It may still be hidden if 'showtabline' is zero.
 */
    पूर्णांक
gui_use_tabline(शून्य)
{
    परत gui.in_use && vim_strchr(p_go, GO_TABLINE) != रिक्त;
}

/*
 * Return TRUE if the GUI is showing the tabline.
 * This uses 'showtabline'.
 */
    स्थिर पूर्णांक
gui_has_tabline(शून्य)
{
    जर (!gui_use_tabline()
	    || p_stal == 0
	    || (p_stal == 1 && first_tabpage->tp_next == रिक्त))
	परत FALSE;
    परत TRUE;
}

/*
 * Update the tabline.
 * This may display/undisplay the tabline and update the labels.
 */
    शून्य
gui_update_tabline(शून्य)
{
    पूर्णांक	    showit = gui_has_tabline();
    पूर्णांक	    shown = gui_mch_showing_tabline();

    जर (!gui.starting && starting == 0)
    {
	// Updating the tabline uses direct GUI commands, flush
	// outstanding instructions first. (esp. clear screen)
	out_flush();

	जर (!showit != !shown)
	{
	    // Save Rows/Columns before showing/hiding the tabline.
	    // gui_mch_show_tabline() processes GTK events (via
	    // gui_mch_update) which may trigger a spurious configure event
	    // that temporarily corrupts Rows.  Restore the original values so
	    // that gui_set_shellsize() uses the correct row/column count.
	    पूर्णांक save_Rows = Rows;
	    पूर्णांक save_Columns = Columns;

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
	जर (showit != 0)
	    gui_mch_update_tabline();
    }
}

/*
 * Get the label or tooltip for tab page "tp" into NameBuff[].
 */
    शून्य
get_tabline_label(
    tabpage_T	*tp,
    पूर्णांक		tooltip)	// TRUE: get tooltip
{
    char_u	**opt;

    // Use 'guitablabel' or 'guitabtooltip' if it's set.
    opt = (tooltip ? &p_gtt : &p_gtl);
    जर (**opt != NUL)
    {
	char_u	res[MAXPATHL];
	tabpage_T *save_curtab;
	char_u	*opt_name = (char_u *)(tooltip ? "guitabtooltip"
							     : "guitablabel");

	printer_page_अंक = tabpage_index(tp);
# ifdef FEAT_EVAL
	set_vim_var_nr(VV_LNUM, printer_page_अंक);
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
					   0, (पूर्णांक)Columns, रिक्त, रिक्त, रिक्त);
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
    जर (**opt == NUL || *NameBuff == NUL)
    {
	win_T	*wp;
	पूर्णांक	wincount = 0;
	पूर्णांक	modified = FALSE;

	// Get the buffer name into NameBuff[] and shorten it.
	get_trans_bufname(tp == curtab ? curbuf : tp->tp_curwin->w_buffer);
	जर (!tooltip)
	    shorten_dir(NameBuff);

	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    ++wincount;
	    जर (bufIsChanged(wp->w_buffer))
		modified = TRUE;
	}

	जर (modified || wincount > 1)
	{
	    char_u  buf[40] = "+ ";		// Tentatively assume modified only
	    माप_प्रकार  buflen = 2;
	    माप_प्रकार  NameBufflen = STRLEN(NameBuff);

	    जर (wincount > 1)
		buflen = vim_snprintf_safelen(
		    (अक्षर *)buf,
		    आकार(buf),
		    "%d%s ",
		    wincount,
		    modified ? "+" : "");

	    // Make sure resulting NameBuff will not exceed its bounds.
	    जर (NameBufflen + buflen < MAXPATHL)
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
    पूर्णांक
send_tabline_event(पूर्णांक nr)
{
    char_u string[3];

    जर (nr == tabpage_index(curtab))
	परत FALSE;

    // Don't put events in the input queue now.
    जर (hold_gui_events || cmdwin_type != 0)
    {
	// Set it back to the current tab page.
	gui_mch_set_curtab(tabpage_index(curtab));
	परत FALSE;
    }

    string[0] = CSI;
    string[1] = KS_TABLINE;
    string[2] = KE_FILLER;
    add_to_आदान_buf(string, 3);
    string[0] = nr;
    add_to_आदान_buf_csi(string, 1);
    परत TRUE;
}

/*
 * Send a tabline menu event
 */
    शून्य
send_tabline_menu_event(पूर्णांक tabidx, पूर्णांक event)
{
    char_u	    string[3];

    // Don't put events in the input queue now.
    जर (hold_gui_events)
	परत;

    // Cannot close the last tabpage.
    जर (event == TABLINE_MENU_CLOSE && first_tabpage->tp_next == रिक्त)
	परत;

    string[0] = CSI;
    string[1] = KS_TABMENU;
    string[2] = KE_FILLER;
    add_to_आदान_buf(string, 3);
    string[0] = tabidx;
    string[1] = (char_u)(दीर्घ)event;
    add_to_आदान_buf_csi(string, 2);
}

#endif

/*
 * Scrollbar stuff:
 */

/*
 * Remove all scrollbars.  Used before switching to another tab page.
 */
    शून्य
gui_remove_scrollbars(शून्य)
{
    पूर्णांक	    i;
    win_T   *wp;

    साठी (i = 0; i < 3; i++)
    {
	जर (i == SBAR_BOTTOM)
	    gui_mch_enable_scrollbar(&gui.bottom_sbar, FALSE);
	अन्यथा
	{
	    FOR_ALL_WINDOWS(wp)
		gui_do_scrollbar(wp, i, FALSE);
	}
	curtab->tp_prev_which_scrollbars[i] = -1;
    }
}

    शून्य
gui_create_scrollbar(scrollbar_T *sb, पूर्णांक type, win_T *wp)
{
    स्थिर पूर्णांक	sbar_ident = 0;

    sb->ident = sbar_ident++;	// No check for too big, but would it happen?
    sb->wp = wp;
    sb->type = type;
    sb->मूल्य = 0;
    sb->size = 1;
    sb->max = 1;
    sb->शीर्ष = 0;
    sb->height = 0;
    sb->width = 0;
    sb->status_height = 0;
    gui_mch_create_scrollbar(sb, (wp == रिक्त) ? SBAR_HORIZ : SBAR_VERT);
}

/*
 * Find the scrollbar with the given index.
 */
    scrollbar_T *
gui_find_scrollbar(दीर्घ ident)
{
    win_T	*wp;

    जर (gui.bottom_sbar.ident == ident)
	परत &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	जर (wp->w_scrollbars[SBAR_डावा].ident == ident)
	    परत &wp->w_scrollbars[SBAR_डावा];
	जर (wp->w_scrollbars[SBAR_उजवा].ident == ident)
	    परत &wp->w_scrollbars[SBAR_उजवा];
    }
    परत रिक्त;
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
    शून्य
gui_drag_scrollbar(scrollbar_T *sb, दीर्घ मूल्य, पूर्णांक still_dragging)
{
    win_T	*wp;
    पूर्णांक		sb_अंक;
#ifdef USE_ON_FLY_SCROLL
    colnr_T	old_leftcol = curwin->w_leftcol;
    linenr_T	old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    पूर्णांक		old_topfill = curwin->w_topfill;
# endif
#अन्यथा
    char_u	bytes[आकार(long_u)];
    पूर्णांक		byte_मोजणी;
#endif

    जर (sb == रिक्त)
	परत;

    // Don't put events in the input queue now.
    जर (hold_gui_events)
	परत;

    जर (cmdwin_type != 0 && sb->wp != cmdwin_win)
	परत;

    जर (still_dragging)
    {
	जर (sb->wp == रिक्त)
	    gui.dragged_sb = SBAR_BOTTOM;
	अन्यथा जर (sb == &sb->wp->w_scrollbars[SBAR_डावा])
	    gui.dragged_sb = SBAR_डावा;
	अन्यथा
	    gui.dragged_sb = SBAR_उजवा;
	gui.dragged_wp = sb->wp;
    }
    अन्यथा
    {
	gui.dragged_sb = SBAR_NONE;
#ifdef FEAT_GUI_GTK
	// Keep the "dragged_wp" value until after the scrolling, for when the
	// mouse button is released.  GTK2 doesn't send the button-up event.
	gui.dragged_wp = रिक्त;
	// WinScrolled event
	gui_focus_change(TRUE);
#endif
    }

    // Vertical sbar info is kept in the first sbar (the left one)
    जर (sb->wp != रिक्त)
	sb = &sb->wp->w_scrollbars[0];

    /*
     * Check validity of value
     */
    जर (मूल्य < 0)
	मूल्य = 0;
#ifdef SCROLL_PAST_END
    अन्यथा जर (मूल्य > sb->max)
	मूल्य = sb->max;
#अन्यथा
    जर (मूल्य > sb->max - sb->size + 1)
	मूल्य = sb->max - sb->size + 1;
#endif

    sb->मूल्य = मूल्य;

#ifdef USE_ON_FLY_SCROLL
    // When not allowed to do the scrolling right now, return.
    // This also checked input_available(), but that causes the first click in
    // a scrollbar to be ignored when Vim doesn't have focus.
    जर (dont_scroll)
	परत;
#endif
    // Disallow scrolling the current window when the completion popup menu is
    // visible.
    जर ((sb->wp == रिक्त || sb->wp == curwin) && pum_visible())
	परत;

#ifdef FEAT_RIGHTLEFT
    जर (sb->wp == रिक्त && curwin->w_p_rl)
    {
	मूल्य = sb->max + 1 - sb->size - मूल्य;
	जर (मूल्य < 0)
	    मूल्य = 0;
    }
#endif

    जर (sb->wp != रिक्त)		// vertical scrollbar
    {
	sb_अंक = 0;
	साठी (wp = firstwin; wp != sb->wp && wp != रिक्त; wp = wp->w_next)
	    sb_अंक++;
	जर (wp == रिक्त)
	    परत;

#ifdef USE_ON_FLY_SCROLL
	current_scrollbar = sb_अंक;
	scrollbar_मूल्य = मूल्य;
	जर (State & MODE_NORMAL)
	{
	    gui_do_scroll();
	    setcursor();
	}
	अन्यथा जर (State & MODE_घाला)
	{
	    ins_scroll();
	    setcursor();
	}
	अन्यथा जर (State & MODE_CMDLINE)
	{
	    जर (संदेश_scrolled == 0)
	    {
		gui_do_scroll();
		redrawcmdline();
	    }
	}
# ifdef FEAT_FOLDING
	// Value may have been changed for closed fold.
	sb->मूल्य = sb->wp->w_topline - 1;
# endif

	// When dragging one scrollbar and there is another one at the other
	// side move the thumb of that one too.
	जर (gui.which_scrollbars[SBAR_उजवा] && gui.which_scrollbars[SBAR_डावा])
	    gui_mch_set_scrollbar_thumb(
		    &sb->wp->w_scrollbars[
			    sb == &sb->wp->w_scrollbars[SBAR_उजवा]
						    ? SBAR_डावा : SBAR_उजवा],
		    sb->मूल्य, sb->size, sb->max);

#अन्यथा
	bytes[0] = CSI;
	bytes[1] = KS_VER_SCROLLBAR;
	bytes[2] = KE_FILLER;
	bytes[3] = (char_u)sb_अंक;
	byte_मोजणी = 4;
#endif
    }
    अन्यथा
    {
#ifdef USE_ON_FLY_SCROLL
	scrollbar_मूल्य = मूल्य;

	जर (State & MODE_NORMAL)
	    do_mousescroll_horiz(scrollbar_मूल्य);
	अन्यथा जर (State & MODE_घाला)
	    ins_horscroll();
	अन्यथा जर (State & MODE_CMDLINE)
	{
	    जर (संदेश_scrolled == 0)
	    {
		do_mousescroll_horiz(scrollbar_मूल्य);
		redrawcmdline();
	    }
	}
	जर (old_leftcol != curwin->w_leftcol)
	{
	    updateWindow(curwin);   // update window, status and cmdline
	    setcursor();
	}
#अन्यथा
	bytes[0] = CSI;
	bytes[1] = KS_HOR_SCROLLBAR;
	bytes[2] = KE_FILLER;
	byte_मोजणी = 3;
#endif
    }

#ifdef USE_ON_FLY_SCROLL
    /*
     * synchronize other windows, as necessary according to 'scrollbind'
     */
    जर (curwin->w_p_scb
	    && ((sb->wp == रिक्त && curwin->w_leftcol != old_leftcol)
		|| (sb->wp == curwin && (curwin->w_topline != old_topline
# ifdef FEAT_DIFF
					   || curwin->w_topfill != old_topfill
# endif
			))))
    {
	do_check_scrollbind(TRUE);
	// need to update the window right here
	FOR_ALL_WINDOWS(wp)
	    जर (wp->w_redr_type > 0)
		updateWindow(wp);
	setcursor();
    }
    out_flush_cursor(FALSE, TRUE);
#अन्यथा
    add_to_आदान_buf(bytes, byte_मोजणी);
    add_long_to_buf((long_u)मूल्य, bytes);
    add_to_आदान_buf_csi(bytes, आकार(long_u));
#endif
}

/*
 * Scrollbar stuff:
 */

/*
 * Called when something in the window layout has changed.
 */
    शून्य
gui_may_update_scrollbars(शून्य)
{
    जर (gui.in_use && starting == 0)
    {
	out_flush();
	gui_init_which_components(रिक्त);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
}

    शून्य
gui_update_scrollbars(
    पूर्णांक		force)	    // Force all scrollbars to get updated
{
    win_T	*wp;
    scrollbar_T	*sb;
    दीर्घ	val, size, max;		// need 32 bits here
    पूर्णांक		which_sb;
    पूर्णांक		h, y;
    स्थिर win_T *prev_curwin = रिक्त;

    // Update the horizontal scrollbar
    gui_update_horiz_scrollbar(force);

#ifndef MSWIN
    // Return straight away if there is neither a left nor right scrollbar.
    // On MS-Windows this is required anyway for scrollwheel messages.
    जर (!gui.which_scrollbars[SBAR_डावा] && !gui.which_scrollbars[SBAR_उजवा])
	परत;
#endif

    /*
     * Don't want to update a scrollbar while we're dragging it.  But if we
     * have both a left and right scrollbar, and we drag one of them, we still
     * need to update the other one.
     */
    जर (!force && (gui.dragged_sb == SBAR_डावा || gui.dragged_sb == SBAR_उजवा)
	    && gui.which_scrollbars[SBAR_डावा]
	    && gui.which_scrollbars[SBAR_उजवा])
    {
	/*
	 * If we have two scrollbars and one of them is being dragged, just
	 * copy the scrollbar position from the dragged one to the other one.
	 */
	which_sb = SBAR_डावा + SBAR_उजवा - gui.dragged_sb;
	जर (gui.dragged_wp != रिक्त)
	    gui_mch_set_scrollbar_thumb(
		    &gui.dragged_wp->w_scrollbars[which_sb],
		    gui.dragged_wp->w_scrollbars[0].मूल्य,
		    gui.dragged_wp->w_scrollbars[0].size,
		    gui.dragged_wp->w_scrollbars[0].max);
    }

    // avoid that moving components around generates events
    ++hold_gui_events;

    FOR_ALL_WINDOWS(wp)
    {
	जर (wp->w_buffer == रिक्त)	// just in case
	    सुरूठेवा;
	// Skip a scrollbar that is being dragged.
	जर (!force && (gui.dragged_sb == SBAR_डावा
					     || gui.dragged_sb == SBAR_उजवा)
		&& gui.dragged_wp == wp)
	    सुरूठेवा;

#ifdef SCROLL_PAST_END
	max = wp->w_buffer->b_ml.ml_ओळ_मोजणी - 1;
#अन्यथा
	max = wp->w_buffer->b_ml.ml_ओळ_मोजणी + wp->w_height - 2;
#endif
	जर (max < 0)			// empty buffer
	    max = 0;
	val = wp->w_topline - 1;
	size = wp->w_height;
#ifdef SCROLL_PAST_END
	जर (val > max)			// just in case
	    val = max;
#अन्यथा
	जर (size > max + 1)		// just in case
	    size = max + 1;
	जर (val > max - size + 1)
	    val = max - size + 1;
#endif
	जर (val < 0)			// minimal value is 0
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
	जर (size < 1 || wp->w_botline - 2 > max)
	{
	    /*
	     * This can happen during changing files.  Just don't update the
	     * scrollbar for now.
	     */
	    sb->height = 0;	    // Force update next time
	    जर (gui.which_scrollbars[SBAR_डावा])
		gui_do_scrollbar(wp, SBAR_डावा, FALSE);
	    जर (gui.which_scrollbars[SBAR_उजवा])
		gui_do_scrollbar(wp, SBAR_उजवा, FALSE);
	    सुरूठेवा;
	}
	जर (force || sb->height != wp->w_height
	    || sb->शीर्ष != wp->w_winrow
	    || sb->status_height != wp->w_status_height
	    || sb->width != wp->w_width
	    || prev_curwin != curwin)
	{
	    // Height, width or position of scrollbar has changed.  For
	    // vertical split: curwin changed.
	    sb->height = wp->w_height;
	    sb->शीर्ष = wp->w_winrow;
	    sb->status_height = wp->w_status_height;
	    sb->width = wp->w_width;

	    // Calculate height and position in pixels
	    h = (sb->height + sb->status_height) * gui.char_height;
	    y = sb->शीर्ष * gui.char_height + gui.border_offset;
#जर defined(FEAT_MENU) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF) && !defined(FEAT_GUI_PHOTON)
	    जर (gui.menu_is_active)
		y += gui.menu_height;
#endif

#जर defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU))
	    जर (vim_strchr(p_go, GO_TOOLBAR) != रिक्त)
		y += gui.toolbar_height;
#endif

#जर defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
	    जर (gui_has_tabline())
		y += gui.tabline_height;
#endif

	    जर (wp->w_winrow == 0)
	    {
		// Height of top scrollbar includes width of top border
		h += gui.border_offset;
		y -= gui.border_offset;
	    }
	    जर (gui.which_scrollbars[SBAR_डावा])
	    {
		gui_mch_set_scrollbar_pos(&wp->w_scrollbars[SBAR_डावा],
					  gui.डावा_sbar_x, y,
					  gui.scrollbar_width, h);
		gui_do_scrollbar(wp, SBAR_डावा, TRUE);
	    }
	    जर (gui.which_scrollbars[SBAR_उजवा])
	    {
		gui_mch_set_scrollbar_pos(&wp->w_scrollbars[SBAR_उजवा],
					  gui.उजवा_sbar_x, y,
					  gui.scrollbar_width, h);
		gui_do_scrollbar(wp, SBAR_उजवा, TRUE);
	    }
	}

	जर (force || sb->मूल्य != val || sb->size != size || sb->max != max)
	{
	    // Thumb of scrollbar has moved
	    sb->मूल्य = val;
	    sb->size = size;
	    sb->max = max;
	    जर (gui.which_scrollbars[SBAR_डावा]
		    && (gui.dragged_sb != SBAR_डावा || gui.dragged_wp != wp))
		gui_mch_set_scrollbar_thumb(&wp->w_scrollbars[SBAR_डावा],
					    val, size, max);
	    जर (gui.which_scrollbars[SBAR_उजवा]
		    && (gui.dragged_sb != SBAR_उजवा || gui.dragged_wp != wp))
		gui_mch_set_scrollbar_thumb(&wp->w_scrollbars[SBAR_उजवा],
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
    स्थिर शून्य
gui_do_scrollbar(
    win_T	*wp,
    पूर्णांक		which,	    // SBAR_LEFT or SBAR_RIGHT
    पूर्णांक		enable)	    // TRUE to enable scrollbar
{
    पूर्णांक		midcol = curwin->w_wincol + curwin->w_width / 2;
    पूर्णांक		has_midcol = (wp->w_wincol <= midcol
				     && wp->w_wincol + wp->w_width >= midcol);

    // Only enable scrollbars that contain the middle column of the current
    // window.
    जर (gui.which_scrollbars[SBAR_उजवा] != gui.which_scrollbars[SBAR_डावा])
    {
	// Scrollbars only on one side.  Don't enable scrollbars that don't
	// contain the middle column of the current window.
	जर (!has_midcol)
	    enable = FALSE;
    }
    अन्यथा
    {
	// Scrollbars on both sides.  Don't enable scrollbars that neither
	// contain the middle column of the current window nor are on the far
	// side.
	जर (midcol > Columns / 2)
	{
	    जर (which == SBAR_डावा ? wp->w_wincol != 0 : !has_midcol)
		enable = FALSE;
	}
	अन्यथा
	{
	    जर (which == SBAR_उजवा ? wp->w_wincol + wp->w_width != Columns
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
    पूर्णांक
gui_do_scroll(शून्य)
{
    win_T	*wp, *save_wp;
    पूर्णांक		i;
    दीर्घ	nlines;
    pos_T	old_cursor;
    linenr_T	old_topline;
#ifdef FEAT_DIFF
    पूर्णांक		old_topfill;
#endif

    साठी (wp = firstwin, i = 0; i < current_scrollbar; wp = W_NEXT(wp), i++)
	जर (wp == रिक्त)
	    थांबा;
    जर (wp == रिक्त)
	// Couldn't find window
	परत FALSE;
    // don't redraw, LineOffset and similar are not valid!
    जर (exmode_active)
	परत FALSE;

    /*
     * Compute number of lines to scroll.  If zero, nothing to do.
     */
    nlines = (दीर्घ)scrollbar_मूल्य + 1 - (दीर्घ)wp->w_topline;
    जर (nlines == 0)
	परत FALSE;

    save_wp = curwin;
    old_topline = wp->w_topline;
#ifdef FEAT_DIFF
    old_topfill = wp->w_topfill;
#endif
    old_cursor = wp->w_cursor;
    curwin = wp;
    curbuf = wp->w_buffer;
    जर (nlines < 0)
	scrolldown(-nlines, gui.dragged_wp == रिक्त);
    अन्यथा
	scrollup(nlines, gui.dragged_wp == रिक्त);
    // Reset dragged_wp after using it.  "dragged_sb" will have been reset for
    // the mouse-up event already, but we still want it to behave like when
    // dragging.  But not the next click in an arrow.
    जर (gui.dragged_sb == SBAR_NONE)
	gui.dragged_wp = रिक्त;

    जर (old_topline != wp->w_topline
#ifdef FEAT_DIFF
	    || old_topfill != wp->w_topfill
#endif
	    )
    {
	जर (get_scrolloff_मूल्य() != 0)
	{
	    cursor_correct();		// fix window for 'so'
	    update_topline();		// avoid up/down jump
	}
	जर (old_cursor.lnum != wp->w_cursor.lnum)
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
    जर (Screen_ओळी != रिक्त && (old_topline != wp->w_topline
	    || wp->w_redr_type != 0
#ifdef FEAT_DIFF
	    || old_topfill != wp->w_topfill
#endif
	    ))
    {
	पूर्णांक type = UPD_VALID;

	जर (pum_visible())
	{
	    type = UPD_NOT_VALID;
	    wp->w_ओळी_valid = 0;
	}

	// Don't set must_redraw here, it may cause the popup menu to
	// disappear when losing focus after a scrollbar drag.
	जर (wp->w_redr_type < type)
	    wp->w_redr_type = type;
	mch_disable_flush();
	updateWindow(wp);   // update window, status line, and cmdline
	mch_enable_flush();
    }

    // May need to redraw the popup menu.
    जर (pum_visible())
	pum_redraw();

    परत (wp == curwin && !EQUAL_POS(curwin->w_cursor, old_cursor));
}

/*
 * Horizontal scrollbar stuff:
 */
    स्थिर शून्य
gui_update_horiz_scrollbar(पूर्णांक force)
{
    दीर्घ	मूल्य, size, max;

    जर (!gui.which_scrollbars[SBAR_BOTTOM])
	परत;

    जर (!force && gui.dragged_sb == SBAR_BOTTOM)
	परत;

    जर (!force && curwin->w_p_wrap && gui.prev_wrap)
	परत;

    /*
     * It is possible for the cursor to be invalid if we're in the middle of
     * something (like changing files).  If so, don't do anything for now.
     */
    जर (curwin->w_cursor.lnum > curbuf->b_ml.ml_ओळ_मोजणी)
    {
	gui.bottom_sbar.मूल्य = -1;
	परत;
    }

    size = curwin->w_width;
    जर (curwin->w_p_wrap)
    {
	मूल्य = 0;
#ifdef SCROLL_PAST_END
	max = 0;
#अन्यथा
	max = curwin->w_width - 1;
#endif
    }
    अन्यथा
    {
	मूल्य = curwin->w_leftcol;
	max = scroll_ओळ_len(ui_find_longest_lnum());

	जर (virtual_active())
	{
	    // May move the cursor even further to the right.
	    जर (curwin->w_virtcol >= (colnr_T)max)
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
    जर (मूल्य > max - size + 1)
	मूल्य = max - size + 1;	    // limit the value to allowable range
#endif

#ifdef FEAT_RIGHTLEFT
    जर (curwin->w_p_rl)
    {
	मूल्य = max + 1 - size - मूल्य;
	जर (मूल्य < 0)
	{
	    size += मूल्य;
	    मूल्य = 0;
	}
    }
#endif
    जर (!force && मूल्य == gui.bottom_sbar.मूल्य && size == gui.bottom_sbar.size
						&& max == gui.bottom_sbar.max)
	परत;

    gui.bottom_sbar.मूल्य = मूल्य;
    gui.bottom_sbar.size = size;
    gui.bottom_sbar.max = max;
    gui.prev_wrap = curwin->w_p_wrap;

    gui_mch_set_scrollbar_thumb(&gui.bottom_sbar, मूल्य, size, max);
}

/*
 * Check that none of the colors are the same as the background color
 */
    शून्य
gui_check_colors(शून्य)
{
    जर (gui.norm_pixel == gui.back_pixel || gui.norm_pixel == INVALCOLOR)
    {
	gui_set_bg_color((char_u *)"White");
	जर (gui.norm_pixel == gui.back_pixel || gui.norm_pixel == INVALCOLOR)
	    gui_set_fg_color((char_u *)"Black");
    }
}

    स्थिर शून्य
gui_set_fg_color(char_u *name)
{
    gui.norm_pixel = gui_get_color(name);
    hl_set_fg_color_name(vim_strsave(name));
}

    स्थिर शून्य
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

    जर (*name == NUL)
	परत INVALCOLOR;
    t = gui_mch_get_color(name);

    पूर्णांक is_none = STRCMP(name, "none") == 0;
    जर (t == INVALCOLOR
#जर defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
	    && (gui.in_use || is_none)
#endif
	    )
    {
	जर (is_none)
	    emsg(_(e_cannot_use_color_none_did_you_mean_none));
	अन्यथा
	    semsg(_(e_cannot_allocate_color_str), name);
    }
    परत t;
}

/*
 * Return the grey value of a color (range 0-255).
 */
    पूर्णांक
gui_get_lightness(guicolor_T pixel)
{
    long_u	rgb = (long_u)gui_mch_get_rgb(pixel);

    परत  (पूर्णांक)(  (((rgb >> 16) & 0xff) * 299)
		   + (((rgb >> 8) & 0xff) * 587)
		   +  ((rgb	  & 0xff) * 114)) / 1000;
}

    char_u *
gui_bg_default(शून्य)
{
    जर (gui_get_lightness(gui.back_pixel) < 127)
	परत (char_u *)"dark";
    परत (char_u *)"light";
}

/*
 * Option initializations that can only be done after opening the GUI window.
 */
    स्थिर शून्य
init_gui_options(शून्य)
{
    // Set the 'background' option according to the lightness of the
    // background color, unless the user has set it already.
    जर (!option_was_set((char_u *)"bg") && STRCMP(p_bg, gui_bg_default()) != 0)
    {
	set_option_मूल्य_give_err((char_u *)"bg", 0L, gui_bg_default(), 0);
	highlight_changed();
    }
}

#जर defined(FEAT_GUI_X11)
    शून्य
gui_new_scrollbar_colors(शून्य)
{
    win_T	*wp;

    // Nothing to do if GUI hasn't started yet.
    जर (!gui.in_use)
	परत;

    FOR_ALL_WINDOWS(wp)
    {
	gui_mch_set_scrollbar_colors(&(wp->w_scrollbars[SBAR_डावा]));
	gui_mch_set_scrollbar_colors(&(wp->w_scrollbars[SBAR_उजवा]));
    }
    gui_mch_set_scrollbar_colors(&gui.bottom_sbar);
}
#endif

/*
 * Call this when focus has changed.
 */
    शून्य
gui_focus_change(पूर्णांक in_focus)
{
/*
 * Skip this code to avoid drawing the cursor when debugging and switching
 * between the debugger window and gvim.
 */
#जर 1
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
    जर (!hold_gui_events
# जर defined(FEAT_GUI_GTK) && defined(FEAT_GUI_DIALOG)
	    && gui.dialogs_active == 0
# endif
       )
    {
	char_u  bytes[3];

	bytes[0] = CSI;
	bytes[1] = KS_EXTRA;
	bytes[2] = in_focus ? (पूर्णांक)KE_FOCUSGAINED : (पूर्णांक)KE_FOCUSLOST;
	add_to_आदान_buf(bytes, 3);
    }
#endif
}

/*
 * When mouse moved: apply 'mousefocus'.
 * Also updates the mouse pointer shape.
 */
    स्थिर शून्य
gui_mouse_focus(पूर्णांक x, पूर्णांक y)
{
    win_T	*wp;
    char_u	st[8];

#ifdef FEAT_MOUSESHAPE
    // Get window pointer, and update mouse shape as well.
    wp = xy2win(x, y, IGNORE_POPUP);
#endif

    // Only handle this when 'mousefocus' set and ...
    जर (p_mousef
	    && !hold_gui_events		// not holding events
	    && (State & (MODE_NORMAL | MODE_घाला))// Normal/Visual/Insert mode
	    && State != MODE_HITRETURN	// but not hit-return prompt
	    && संदेश_scrolled == 0	// no scrolled message
	    && !need_mouse_correct	// not moving the pointer
	    && gui.in_focus)		// gvim in focus
    {
	// Don't move the mouse when it's left or right of the Vim window
	जर (x < 0 || x > Columns * gui.char_width)
	    परत;
#ifndef FEAT_MOUSESHAPE
	wp = xy2win(x, y, IGNORE_POPUP);
#endif
	जर (wp == curwin || wp == रिक्त)
	    परत;	// still in the same old window, or none at all

	// Ignore position in the tab pages line.
	जर (Y_2_ROW(y) < tabline_height())
	    परत;

	/*
	 * Format a mouse click on status line input,
	 * ala gui_send_mouse_event(0, x, y, 0, 0);
	 * Trick: Use a column number -1, so that get_pseudo_mouse_code() will
	 * generate a K_LEFTMOUSE_NM key code.
	 */
	जर (finish_op)
	{
	    // abort the current operator first
	    st[0] = ESC;
	    add_to_आदान_buf(st, 1);
	}
	st[0] = CSI;
	st[1] = KS_MOUSE;
	st[2] = KE_FILLER;
	st[3] = (char_u)MOUSE_डावा;
	fill_mouse_coord(st + 4,
		wp->w_wincol == 0 ? -1 : wp->w_wincol + MOUSE_COLOFF,
		wp->w_height + W_WINROW(wp));

	add_to_आदान_buf(st, 8);
	st[3] = (char_u)MOUSE_RELEASE;
	add_to_आदान_buf(st, 8);
#ifdef FEAT_GUI_GTK
	// Need to wake up the main loop
	जर (gtk_main_level() > 0)
	    gtk_main_quit();
#endif
    }
}

/*
 * Called when the mouse moved (but not when dragging).
 */
    शून्य
gui_mouse_moved(पूर्णांक x, पूर्णांक y)
{
    // Ignore this while still starting up.
    जर (!gui.in_use || gui.starting)
	परत;

    // apply 'mousefocus' and pointer shape
    gui_mouse_focus(x, y);

    जर (p_mousemev
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
    पूर्णांक		x, y;

    जर (!(gui.in_use && (p_mousef || popup == FIND_POPUP)))
	परत रिक्त;
    gui_mch_getmouse(&x, &y);

    // Only use the mouse when it's on the Vim window
    जर (x >= 0 && x <= Columns * gui.char_width
	    && y >= 0 && Y_2_ROW(y) >= tabline_height())
	परत xy2win(x, y, popup);
    परत रिक्त;
}

/*
 * Called when mouse should be moved to window with focus.
 */
    शून्य
gui_mouse_correct(शून्य)
{
    win_T	*wp = रिक्त;

    need_mouse_correct = FALSE;

    wp = gui_mouse_window(IGNORE_POPUP);
    जर (wp == curwin || wp == रिक्त)
	परत;

    // If in other than current window
    validate_cline_row();
    gui_mch_setmouse((पूर्णांक)W_ENDCOL(curwin) * gui.char_width - 3,
	    (W_WINROW(curwin) + curwin->w_wrow) * gui.char_height
	    + (gui.char_height) / 2);
}

/*
 * Find window where the mouse pointer "x" / "y" coordinate is in.
 * As a side effect update the shape of the mouse pointer.
 */
    स्थिर win_T *
xy2win(पूर्णांक x, पूर्णांक y, mouse_find_T popup)
{
    पूर्णांक		row;
    पूर्णांक		col;
    win_T	*wp;

    row = Y_2_ROW(y);
    col = X_2_COL(x);

    जर (row < 0 || col < 0)		// before first window
	परत रिक्त;
    wp = mouse_find_win(&row, &col, popup);
    जर (wp == रिक्त)
    {
#ifdef FEAT_MOUSESHAPE
	update_mouseshape(-2);
#endif
	परत रिक्त;
    }
#ifdef FEAT_MOUSESHAPE
    जर (State == MODE_HITRETURN || State == MODE_ASKMORE)
    {
	जर (Y_2_ROW(y) >= संदेश_row)
	    update_mouseshape(SHAPE_IDX_MOREL);
	अन्यथा
	    update_mouseshape(SHAPE_IDX_MORE);
    }
    अन्यथा जर (row >= wp->w_height + wp->w_status_height)	// below status line
	update_mouseshape(SHAPE_IDX_CLINE);
    अन्यथा जर (!(State & MODE_CMDLINE) && wp->w_vsep_width > 0 && col == wp->w_width
	    && (!(row >= wp->w_height && row < wp->w_height
	    + wp->w_status_height) || !stl_connected(wp)) && संदेश_scrolled == 0)
	update_mouseshape(SHAPE_IDX_VSEP);
    अन्यथा जर (!(State & MODE_CMDLINE) && wp->w_status_height > 0
	    && row >= wp->w_height && row < wp->w_height + wp->w_status_height
	    && संदेश_scrolled == 0)
	update_mouseshape(SHAPE_IDX_STATUS);
    अन्यथा
	update_mouseshape(-2);
#endif
    परत wp;
}

/*
 * ":gui" and ":gvim": Change from the terminal version to the GUI version.
 * File names may be given to redefine the args list.
 */
    शून्य
ex_gui(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    /*
     * Check for "-f" argument: foreground, don't fork.
     * Also don't fork when started with "gvim -f".
     * Do fork when using "gui -b".
     */
    जर (arg[0] == '-'
	    && (arg[1] == 'f' || arg[1] == 'b')
	    && (arg[2] == NUL || VIM_ISWHITE(arg[2])))
    {
	gui.dofork = (arg[1] == 'b');
	eap->arg = skipwhite(eap->arg + 2);
    }
    जर (!gui.in_use)
    {
#जर defined(VIMDLL) && !defined(EXPERIMENTAL_GUI_CMD)
	जर (!gui.starting)
	{
	    emsg(_(e_gui_cannot_be_used_not_enabled_at_compile_time));
	    परत;
	}
#endif
	// Clear the command.  Needed for when forking+exiting, to avoid part
	// of the argument ending up after the shell prompt.
	संदेश_clr_eos_force();
#ifdef GUI_MAY_SPAWN
	जर (!ends_excmd2(eap->cmd, eap->arg))
	    gui_start(eap->arg);
	अन्यथा
#endif
	    gui_start(रिक्त);
#ifdef FEAT_JOB_CHANNEL
	channel_gui_register_all();
#endif
    }
    जर (!ends_excmd2(eap->cmd, eap->arg))
	ex_next(eap);
}

#जर (defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	    || defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON) \
	    || defined(FEAT_GUI_HAIKU)) \
	    && defined(FEAT_TOOLBAR)
/*
 * This is shared between Haiku, Motif, and GTK.
 */

/*
 * Callback function for do_in_runtimepath().
 */
    स्थिर शून्य
gfp_setname(char_u *fname, शून्य *cookie)
{
    char_u	*gfp_buffer = cookie;

    जर (STRLEN(fname) >= MAXPATHL)
	*gfp_buffer = NUL;
    अन्यथा
	STRCPY(gfp_buffer, fname);
}

/*
 * Find the path of bitmap "name" with extension "ext" in 'runtimepath'.
 * Return FAIL for failure and OK if buffer[MAXPATHL] contains the result.
 */
    पूर्णांक
gui_find_bitmap(char_u *name, char_u *buffer, अक्षर *ext)
{
    जर (STRLEN(name) > MAXPATHL - 14)
	परत FAIL;
    vim_snprintf((अक्षर *)buffer, MAXPATHL, "bitmaps/%s.%s", name, ext);
    जर (do_in_runtimepath(buffer, 0, gfp_setname, buffer) == FAIL
							    || *buffer == NUL)
	परत FAIL;
    परत OK;
}

# जर !defined(FEAT_GUI_GTK)
/*
 * Given the name of the "icon=" argument, try finding the bitmap file for the
 * icon.  If it is an absolute path name, use it as it is.  Otherwise append
 * "ext" and search for it in 'runtimepath'.
 * The result is put in "buffer[MAXPATHL]".  If something fails "buffer"
 * contains "name".
 */
    शून्य
gui_find_iconfile(char_u *name, char_u *buffer, अक्षर *ext)
{
    char_u	buf[MAXPATHL + 1];

    expand_env(name, buffer, MAXPATHL);
    जर (!mch_isFullName(buffer) && gui_find_bitmap(buffer, buf, ext) == OK)
	STRCPY(buffer, buf);
}
# endif
#endif

#जर defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)|| defined(FEAT_GUI_HAIKU)
    शून्य
display_errors(शून्य)
{
    char_u	*p;

    जर (isatty(2))
    {
	ख_साफ(मानक_त्रुटि);
	परत;
    }

    जर (error_ga.ga_data == रिक्त)
	परत;

    // avoid putting up a message box with blanks only
    साठी (p = (char_u *)error_ga.ga_data; *p != NUL; ++p)
	जर (!SAFE_isspace(*p))
	{
	    // Truncate a very long message, it will go off-screen.
	    जर (STRLEN(p) > 2000)
		STRCPY(p + 2000 - 14, "...(truncated)");
	    (शून्य)do_dialog(VIM_ERROR, (char_u *)_("Error"),
		    p, (char_u *)_("&Ok"), 1, रिक्त, FALSE);
	    थांबा;
	}
    ga_clear(&error_ga);
}
#endif

#जर defined(NO_CONSOLE_आदान)
/*
 * Return TRUE if still starting up and there is no place to enter text.
 * For GTK and X11 we check if stderr is not a tty, which means we were
 * (probably) started from the desktop.  Also check stdin, "vim >& file" does
 * allow typing on stdin.
 */
    पूर्णांक
no_console_आदान(शून्य)
{
    परत ((!gui.in_use || gui.starting)
# ifndef NO_CONSOLE
	    && !isatty(0) && !isatty(2)
# endif
	    );
}
#endif

#जर defined(FIND_REPLACE_DIALOG) \
	|| defined(NEED_GUI_UPDATE_SCREEN)
/*
 * Update the current window and the screen.
 */
    शून्य
gui_update_screen(शून्य)
{
# ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_ओळ = 0;
    linenr_T	conceal_new_cursor_ओळ = 0;
    पूर्णांक		conceal_update_ओळी = FALSE;
# endif

    update_topline();
    validate_cursor();

    // Trigger CursorMoved if the cursor moved.
    जर (!finish_op && (has_cursormoved()
# ifdef FEAT_PROP_POPUP
		|| popup_visible
# endif
# ifdef FEAT_CONCEAL
		|| curwin->w_p_cole > 0
# endif
		) && !EQUAL_POS(last_cursormoved, curwin->w_cursor))
    {
	जर (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, रिक्त, रिक्त, FALSE, curbuf);
# ifdef FEAT_PROP_POPUP
	जर (popup_visible)
	    popup_check_cursor_pos();
# endif
# ifdef FEAT_CONCEAL
	जर (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_ओळ = last_cursormoved.lnum;
	    conceal_new_cursor_ओळ = curwin->w_cursor.lnum;
	    conceal_update_ओळी = TRUE;
	}
# endif
	last_cursormoved = curwin->w_cursor;
    }

    जर (!finish_op)
	may_trigger_win_scrolled_resized();

# ifdef FEAT_CONCEAL
    जर (conceal_update_ओळी
	    && (conceal_old_cursor_ओळ != conceal_new_cursor_ओळ
		|| conceal_cursor_ओळ(curwin)
		|| need_cursor_ओळ_redraw))
    {
	जर (conceal_old_cursor_ओळ != conceal_new_cursor_ओळ)
	    redrawWinline(curwin, conceal_old_cursor_ओळ);
	redrawWinline(curwin, conceal_new_cursor_ओळ);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_ओळ_redraw = FALSE;
    }
# endif
    update_screen(0);	// may need to update the screen
    setcursor();
    out_flush_cursor(TRUE, FALSE);
}
#endif

#जर defined(FIND_REPLACE_DIALOG)
/*
 * Get the text to use in a find/replace dialog.  Uses the last search pattern
 * if the argument is empty.
 * Returns an allocated string.
 */
    char_u *
get_find_dialog_text(
    char_u	*arg,
    पूर्णांक		*wwordp,	// return: TRUE if \< \> found
    पूर्णांक		*mcasep)	// return: TRUE if \C found
{
    char_u	*text;

    जर (*arg == NUL)
	text = last_शोध_pat();
    अन्यथा
	text = arg;
    जर (text != रिक्त)
    {
	पूर्णांक len = (पूर्णांक)STRLEN(text);

	text = vim_strnsave(text, len);
	जर (text != रिक्त)
	{
	    पूर्णांक i;

	    // Remove "\V"
	    जर (len >= 2 && STRNCMP(text, "\\V", 2) == 0)
	    {
		mch_memmove(text, text + 2, (माप_प्रकार)(len - 1));
		len -= 2;
	    }

	    // Recognize "\c" and "\C" and remove.
	    जर (len >= 2 && *text == '\\' && (text[1] == 'c' || text[1] == 'C'))
	    {
		*mcasep = (text[1] == 'C');
		mch_memmove(text, text + 2, (माप_प्रकार)(len - 1));
		len -= 2;
	    }

	    // Recognize "\<text\>" and remove.
	    जर (len >= 4
		    && STRNCMP(text, "\\<", 2) == 0
		    && STRNCMP(text + len - 2, "\\>", 2) == 0)
	    {
		*wwordp = TRUE;
		mch_memmove(text, text + 2, (माप_प्रकार)(len - 4));
		text[len - 4] = NUL;
	    }

	    // Recognize "\/" or "\?" and remove.
	    साठी (i = 0; i + 1 < len; ++i)
		जर (text[i] == '\\' && (text[i + 1] == '/'
						       || text[i + 1] == '?'))
		{
		    mch_memmove(text + i, text + i + 1, (माप_प्रकार)(len - i));
		    --len;
		}
	}
    }
    परत text;
}

/*
 * Handle the press of a button in the find-replace dialog.
 * Return TRUE when something was added to the input buffer.
 */
    पूर्णांक
gui_do_findrepl(
    पूर्णांक		flags,		// one of FRD_REPLACE, FRD_FINDNEXT, etc.
    char_u	*find_text,
    char_u	*repl_text,
    पूर्णांक		down)		// Search downwards.
{
    garray_T	ga;
    पूर्णांक		i;
    पूर्णांक		type = (flags & FRD_TYPE_MASK);
    char_u	*p;
    regmatch_T	regmatch;
    पूर्णांक		save_did_emsg = did_emsg;
    स्थिर पूर्णांक  busy = FALSE;

    // When the screen is being updated we should not change buffers and
    // windows structures, it may cause freed memory to be used.  Also don't
    // do this recursively (pressing "Find" quickly several times).
    जर (updating_screen || busy)
	परत FALSE;

    // refuse replace when text cannot be changed
    जर ((type == FRD_REPLACE || type == FRD_REPLACEALL) && text_locked())
	परत FALSE;

    busy = TRUE;

    ga_init2(&ga, 1, 100);
    जर (type == FRD_REPLACEALL)
	GA_CONCAT_LITERAL(&ga, "%s/");

    GA_CONCAT_LITERAL(&ga, "\\V");
    जर (flags & FRD_MATCH_CASE)
	GA_CONCAT_LITERAL(&ga, "\\C");
    अन्यथा
	GA_CONCAT_LITERAL(&ga, "\\c");
    जर (flags & FRD_WHOLE_WORD)
	GA_CONCAT_LITERAL(&ga, "\\<");
    // escape slash and backslash
    p = vim_strsave_escaped(find_text, (char_u *)"/\\");
    जर (p != रिक्त)
	ga_concat(&ga, p);
    vim_free(p);
    जर (flags & FRD_WHOLE_WORD)
	GA_CONCAT_LITERAL(&ga, "\\>");

    जर (type == FRD_REPLACEALL)
    {
	GA_CONCAT_LITERAL(&ga, "/");
	// Escape slash and backslash.
	// Also escape tilde and ampersand if 'magic' is set.
	p = vim_strsave_escaped(repl_text,
				p_magic ? (char_u *)"/\\~&" : (char_u *)"/\\");
	जर (p != रिक्त)
	    ga_concat(&ga, p);
	vim_free(p);
	GA_CONCAT_LITERAL(&ga, "/g");
    }
    ga_append(&ga, NUL);

    जर (type == FRD_REPLACE)
    {
	// Do the replacement when the text at the cursor matches.  Thus no
	// replacement is done if the cursor was moved!
	regmatch.regprog = vim_regcomp(ga.ga_data, RE_MAGIC + RE_STRING);
	regmatch.rm_ic = 0;
	जर (regmatch.regprog != रिक्त)
	{
	    p = ml_get_cursor();
	    जर (vim_regexec_nl(&regmatch, p, (colnr_T)0)
						   && regmatch.startp[0] == p)
	    {
		// Clear the command line to remove any old "No match"
		// error.
		संदेश_end_prompt();

		जर (u_save_cursor() == OK)
		{
		    // A button was pressed thus undo should be synced.
		    u_sync(FALSE);

		    del_bytes((दीर्घ)(regmatch.endp[0] - regmatch.startp[0]),
								FALSE, FALSE);
		    ins_str(repl_text, STRLEN(repl_text));
		}
	    }
	    अन्यथा
		संदेश(_("No match at cursor, finding next"));
	    vim_regfree(regmatch.regprog);
	}
    }

    जर (type == FRD_REPLACEALL)
    {
	// A button was pressed, thus undo should be synced.
	u_sync(FALSE);
	do_cmdline_cmd(ga.ga_data);
    }
    अन्यथा
    {
	पूर्णांक searchflags = शोध_संदेश + शोध_MARK;

	// Search for the next match.
	// Don't skip text under cursor for single replace.
	जर (type == FRD_REPLACE)
	    searchflags += शोध_START;
	i = संदेश_scroll;
	जर (down)
	{
	    (शून्य)do_शोध(रिक्त, '/', '/', ga.ga_data, STRLEN(ga.ga_data), 1L, searchflags, रिक्त);
	}
	अन्यथा
	{
	    // We need to escape '?' if and only if we are searching in the up
	    // direction
	    p = vim_strsave_escaped(ga.ga_data, (char_u *)"?");
	    जर (p != रिक्त)
		(शून्य)do_शोध(रिक्त, '?', '?', p, STRLEN(p), 1L, searchflags, रिक्त);
	    vim_free(p);
	}

	संदेश_scroll = i;	    // don't let an error message set msg_scroll
    }

    // Don't want to pass did_emsg to other code, it may cause disabling
    // syntax HL if we were busy redrawing.
    did_emsg = save_did_emsg;

    जर (State & (MODE_NORMAL | MODE_घाला))
    {
	gui_update_screen();		// update the screen
	संदेश_didout = 0;			// overwrite any message
	need_wait_return = FALSE;	// don't wait for return
    }

    vim_free(ga.ga_data);
    busy = FALSE;
    परत (ga.ga_len > 0);
}

#endif

#जर defined(HAVE_DROP_FILE)
/*
 * Jump to the window at specified point (x, y).
 */
    स्थिर शून्य
gui_wingoto_xy(पूर्णांक x, पूर्णांक y)
{
    पूर्णांक		row = Y_2_ROW(y);
    पूर्णांक		col = X_2_COL(x);
    win_T	*wp;

    जर (row < 0 || col < 0)
	परत;

    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    जर (wp != रिक्त && wp != curwin)
	win_goto(wp);
}

/*
 * Function passed to handle_drop() for the actions to be done after the
 * argument list has been updated.
 */
    स्थिर शून्य
drop_callback(शून्य *cookie)
{
    char_u	*p = cookie;
    पूर्णांक		do_shorten = FALSE;

    // If Shift held down, change to first file's directory.  If the first
    // item is a directory, change to that directory (and let the explorer
    // plugin show the contents).
    जर (p != रिक्त)
    {
	जर (mch_isdir(p))
	{
	    जर (mch_chdir((अक्षर *)p) == 0)
		do_shorten = TRUE;
	}
	अन्यथा जर (vim_chdirfile(p, "drop") == OK)
	    do_shorten = TRUE;
	vim_free(p);
	जर (do_shorten)
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
    शून्य
gui_handle_drop(
    पूर्णांक		x UNUSED,
    पूर्णांक		y UNUSED,
    int_u	modifiers,
    char_u	**fnames,
    पूर्णांक		मोजणी)
{
    पूर्णांक		i;
    char_u	*p;
    स्थिर पूर्णांक	entered = FALSE;

    /*
     * This function is called by event handlers.  Just in case we get a
     * second event before the first one is handled, ignore the second one.
     * Not sure if this can ever happen, just in case.
     */
    जर (entered)
	परत;
    entered = TRUE;

    /*
     * When the cursor is at the command line, add the file names to the
     * command line, don't edit the files.
     */
    जर (State & MODE_CMDLINE)
    {
	shorten_filenames(fnames, मोजणी);
	साठी (i = 0; i < मोजणी; ++i)
	{
	    जर (fnames[i] != रिक्त)
	    {
		जर (i > 0)
		    add_to_आदान_buf((char_u*)" ", 1);

		// We don't know what command is used thus we can't be sure
		// about which characters need to be escaped.  Only escape the
		// most common ones.
# ifdef BACKSLASH_IN_FILENAME
		p = vim_strsave_escaped(fnames[i], (char_u *)" \t\"|");
# अन्यथा
		p = vim_strsave_escaped(fnames[i], (char_u *)"\\ \t\"|");
# endif
		जर (p != रिक्त)
		    add_to_आदान_buf_csi(p, (पूर्णांक)STRLEN(p));
		vim_free(p);
		vim_free(fnames[i]);
	    }
	}
	vim_free(fnames);
    }
    अन्यथा
    {
	// Go to the window under mouse cursor, then shorten given "fnames" by
	// current window, because a window can have local current dir.
	gui_wingoto_xy(x, y);
	shorten_filenames(fnames, मोजणी);

	// If Shift held down, remember the first item.
	जर ((modifiers & MOUSE_SHIFT) != 0)
	    p = vim_strsave(fnames[0]);
	अन्यथा
	    p = रिक्त;

	// Handle the drop, :edit or :split to get to the file.  This also
	// frees fnames[].  Skip this if there is only one item, it's a
	// directory and Shift is held down.
	जर (मोजणी == 1 && (modifiers & MOUSE_SHIFT) != 0
						     && mch_isdir(fnames[0]))
	{
	    vim_free(fnames[0]);
	    vim_free(fnames);
	    vim_free(p);
	}
	अन्यथा
	    handle_drop(मोजणी, fnames, (modifiers & MOUSE_CTRL) != 0,
						     drop_callback, (शून्य *)p);
    }

    entered = FALSE;
}
#endif

/*
 * Check if "key" is to interrupt us.  Handles a key that has not had modifiers
 * applied yet.
 * Return the key with modifiers applied if so, NUL if not.
 */
    पूर्णांक
check_for_interrupt(पूर्णांक किल्ली, पूर्णांक modifiers_arg)
{
    पूर्णांक modifiers = modifiers_arg;
    पूर्णांक c = merge_modify_Other_किल्ल्या(किल्ली, &modifiers);

    जर ((c == Ctrl_C && ctrl_c_interrupts)
#ifdef UNIX
	    || (intr_char != Ctrl_C && c == intr_char)
#endif
	    )
    {
	got_int = TRUE;
	परत c;
    }
    परत NUL;
}

/*
 * If the "--gui-log-file fname" argument is given write the dialog title and
 * message to a file and return TRUE.  Otherwise return FALSE.
 * When there is any problem opening the file or writing to the file this is
 * ignored, showing the dialog might get the test to get stuck.
 */
    पूर्णांक
gui_dialog_log(char_u *title, char_u *message)
{
    char_u  *fname = get_gui_dialog_file();
    खाता    *fd;

    जर (fname == रिक्त)
	परत FALSE;

    fd = mch_fopen((अक्षर *)fname, "a");
    जर (fd != रिक्त)
    {
	ख_लिखो(fd, "%s: %s\n", title, message);
	ख_बंद(fd);
    }
    परत TRUE;
}
