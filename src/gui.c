/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#அந்தர்பாவ "vim.h"

// Structure containing all the GUI information
gui_T gui;

#எனில் defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
# define USE_SET_GUIFONTWIDE
நிலையான வெறுமை set_guifontwide(char_u *font_name);
#endif
நிலையான வெறுமை gui_check_pos(வெறுமை);
நிலையான வெறுமை gui_reset_scroll_region(வெறுமை);
நிலையான வெறுமை gui_outstr(char_u *, முழு);
#ifndef USE_GTK4
நிலையான முழு gui_screenchar(முழு off, முழு flags, guicolor_T fg, guicolor_T bg, முழு back);
#endif
நிலையான முழு gui_outstr_nowrap(char_u *s, முழு len, முழு flags, guicolor_T fg, guicolor_T bg, முழு back);
நிலையான வெறுமை gui_delete_வரிகள்(முழு row, முழு மொத்தம்);
நிலையான வெறுமை gui_செருகு_வரிகள்(முழு row, முழு மொத்தம்);
நிலையான முழு gui_xy2colrow(முழு x, முழு y, முழு *colp);
#எனில் defined(FEAT_GUI_TABLINE)
நிலையான முழு gui_has_tabline(வெறுமை);
#endif
நிலையான வெறுமை gui_do_scrollbar(win_T *wp, முழு which, முழு enable);
நிலையான வெறுமை gui_update_horiz_scrollbar(முழு);
நிலையான வெறுமை gui_set_fg_color(char_u *name);
நிலையான வெறுமை gui_set_bg_color(char_u *name);
நிலையான வெறுமை init_gui_options(வெறுமை);
நிலையான win_T *xy2win(முழு x, முழு y, mouse_find_T popup);

#ifdef GUI_MAY_FORK
நிலையான வெறுமை gui_do_fork(வெறுமை);

நிலையான முழு gui_read_child_pipe(முழு fd);

// Return values for gui_read_child_pipe
எண்ணிக்கை {
    GUI_CHILD_IO_ERROR,
    GUI_CHILD_OK,
    GUI_CHILD_FAILED
};
#endif

நிலையான வெறுமை gui_attempt_start(வெறுமை);

நிலையான முழு can_update_cursor = TRUE; // can display the cursor
நிலையான முழு disable_flush = 0;	// If > 0, gui_mch_flush() is disabled.

/*
 * gui_start -- Called when user wants to start the GUI.
 *
 * Careful: This function can be called recursively when there is a ":gui"
 * command in the .gvimrc file.  Only the first call should fork, not the
 * recursive call.
 */
    வெறுமை
gui_start(char_u *arg UNUSED)
{
    char_u	*old_term;
#ifdef GUI_MAY_FORK
    நிலையான முழு	recursive = 0;
#endif
#எனில் defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
    எழுத்து	*செய்தி = வெற்று;
#endif

    old_term = vim_strsave(T_NAME);

    settmode(TMODE_COOK);		// stop RAW mode
    எனில் (full_screen)
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
    எனில் (gui.dofork && !vim_strchr(p_go, GO_FORG) && recursive <= 1
# ifdef FEAT_JOB_CHANNEL
	    && !job_any_running()
# endif
	    )
    {
	gui_do_fork();
    }
    இல்லையெனில்
#endif
#ifdef GUI_MAY_SPAWN
    எனில் (gui.dospawn
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
	செய்தி =
# endif
	    gui_mch_do_spawn(arg);
    }
    இல்லையெனில்
#endif
    {
#ifdef FEAT_GUI_GTK
	// If there is 'f' in 'guioptions' and specify -g argument,
	// gui_mch_init_check() was not called yet.
	எனில் (gui_mch_init_check() != OK)
	    getout_preserve_modified(1);
#endif
	gui_attempt_start();
    }

    எனில் (!gui.in_use)			// failed to start GUI
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
#எனில் defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
	எனில் (செய்தி != வெற்று)
	    emsg(செய்தி);
#endif
    }
    இல்லையெனில்
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

#எனில் defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    // Enable fullscreen mode
    எனில் (vim_strchr(p_go, GO_FULLSCREEN) != வெற்று)
       gui_mch_set_fullscreen(TRUE);
#endif

    vim_free(old_term);

    // If the GUI started successfully, trigger the GUIEnter event, otherwise
    // the GUIFailed event.
    gui_mch_update();
    apply_autocmds(gui.in_use ? EVENT_GUIENTER : EVENT_GUIFAILED,
						   வெற்று, வெற்று, FALSE, curbuf);
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
    நிலையான வெறுமை
gui_attempt_start(வெறுமை)
{
    நிலையான முழு recursive = 0;

    ++recursive;
    gui.starting = true;

#ifdef FEAT_GUI_GTK
    gui.event_time = GDK_CURRENT_TIME;
#endif

    termcapinit((char_u *)"builtin_gui");
    gui.starting = recursive - 1;

#எனில் defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
    எனில் (gui.in_use)
    {
# ifdef FEAT_EVAL
	Window	x11_window;
	Display	*x11_display;

	எனில் (gui_get_x11_windis(&x11_window, &x11_display) == OK)
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
# எனில் defined(HAVE_SYS_WAIT_H)
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
    நிலையான வெறுமை
gui_do_fork(வெறுமை)
{
    முழு		pipefd[2];	// pipe between parent and child
    முழு		pipe_error;
    முழு		status;
    முழு		exit_status;
    pid_t	pid = -1;

# எனில் defined(FEAT_RELTIME) && defined(PROF_NSEC)
    // a timer is not carried forward
    delete_timer();
# endif

    // Setup a pipe between the child and the parent, so that the parent
    // knows when the child has done the setsid() call and is allowed to
    // exit.
    pipe_error = (pipe(pipefd) < 0);
    pid = fork();
    எனில் (pid < 0)	    // Fork error
    {
	emsg(_(e_failed_to_create_new_process_for_GUI));
	திரும்பு;
    }
    இல்லையெனில் எனில் (pid > 0)	    // Parent
    {
	// Give the child some time to do the setsid(), otherwise the
	// exit() may kill the child too (when starting gvim from inside a
	// gvim).
	எனில் (!pipe_error)
	{
	    // The read returns when the child closes the pipe (or when
	    // the child dies for some reason).
	    close(pipefd[1]);
	    status = gui_read_child_pipe(pipefd[0]);
	    எனில் (status == GUI_CHILD_FAILED)
	    {
		// The child failed to start the GUI, so the caller must
		// continue. There may be more error information written
		// to stderr by the child.
		waitpid(pid, &exit_status, 0);
		emsg(_(e_the_child_process_failed_to_start_GUI));
		திரும்பு;
	    }
	    இல்லையெனில் எனில் (status == GUI_CHILD_IO_ERROR)
	    {
		pipe_error = TRUE;
	    }
	    // else GUI_CHILD_OK: parent exit
	}

	எனில் (pipe_error)
	    ui_delay(301L, TRUE);

	// When swapping screens we may need to go to the next line, e.g.,
	// after a hit-enter prompt and using ":gui".
	எனில் (newline_on_exit)
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
    எனில் (gui_mch_init_check() != OK)
	getout_preserve_modified(1);
# endif

# எனில் defined(HAVE_SETSID) || defined(HAVE_SETPGID)
    /*
     * Change our process group.  On some systems/shells a CTRL-C in the
     * shell where Vim was started would otherwise kill gvim!
     */
#  எனில் defined(HAVE_SETSID)
    (வெறுமை)setsid();
#  இல்லையெனில்
    (வெறுமை)setpgid(0, 0);
#  endif
# endif
    எனில் (!pipe_error)
	close(pipefd[0]);

# எனில் defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
    // Tell the session manager our new PID
    gui_mch_forked();
# endif

    // Try to start the GUI
    gui_attempt_start();

    // Notify the parent
    எனில் (!pipe_error)
    {
	எனில் (gui.in_use)
	    write_eintr(pipefd[1], "ok", 3);
	இல்லையெனில்
	    write_eintr(pipefd[1], "fail", 5);
	close(pipefd[1]);
    }

    // If we failed to start the GUI, exit now.
    எனில் (!gui.in_use)
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
    நிலையான முழு
gui_read_child_pipe(முழு fd)
{
    long	bytes_read;
# define READ_BUFFER_SIZE 10
    எழுத்து	buffer[READ_BUFFER_SIZE];

    bytes_read = read_eintr(fd, buffer, READ_BUFFER_SIZE - 1);
# undef READ_BUFFER_SIZE
    close(fd);
    எனில் (bytes_read < 0)
	திரும்பு GUI_CHILD_IO_ERROR;
    buffer[bytes_read] = NUL;
    எனில் (STRCMP(buffer, "ok") == 0)
	திரும்பு GUI_CHILD_OK;
    திரும்பு GUI_CHILD_FAILED;
}

#endif // GUI_MAY_FORK

/*
 * Call this when vim starts up, whether or not the GUI is started
 */
    வெறுமை
gui_prepare(முழு *argc, எழுத்து **argv)
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
    முழு
gui_init_check(வெறுமை)
{
    நிலையான முழு result = MAYBE;

    எனில் (result != MAYBE)
    {
	எனில் (result == FAIL)
	    emsg(_(e_cannot_start_the_GUI));
	திரும்பு result;
    }

    gui.shell_created = false;
    gui.dying = false;
    gui.in_focus = true;		// so the guicursor setting works
    gui.dragged_sb = SBAR_NONE;
    gui.dragged_wp = வெற்று;
    gui.pointer_hidden = false;
    gui.col = 0;
    gui.row = 0;
    gui.இலக்கம்_cols = Columns;
    gui.இலக்கம்_rows = Rows;

    gui.cursor_is_valid = false;
    gui.scroll_region_உச்சி = 0;
    gui.scroll_region_bot = Rows - 1;
    gui.scroll_region_இடது = 0;
    gui.scroll_region_வலது = Columns - 1;
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
#  இல்லையெனில்
    gui.menu_font = NOFONT;
#  endif
# endif
    gui.menu_is_active = true;	    // default: include menu
# ifndef FEAT_GUI_GTK
    gui.menu_height = MENU_DEFAULT_HEIGHT;
    gui.menu_width = 0;
# endif
#endif
#எனில் defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    gui.toolbar_height = 0;
#endif
#ifdef FEAT_BEVAL_TIP
    gui.tooltip_fontset = NOFONTSET;
#endif

    gui.scrollbar_width = gui.scrollbar_height = SB_DEFAULT_WIDTH;
    gui.prev_wrap = -1;

#எனில் defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // Note: gui_set_ligatures() might already have been called e.g. from .vimrc,
    // and in that case we don't want to overwrite ligatures map that has already
    // been correctly populated (as that would lead to a cleared ligatures maps).
    எனில் (*p_guiligatures == NUL)
	CLEAR_FIELD(gui.ligatures_map);
#endif

#எனில் defined(ALWAYS_USE_GUI) || defined(VIMDLL)
    result = OK;
#இல்லையெனில்
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
# இல்லையெனில்
    result = gui_mch_init_check();
# endif
#endif
    திரும்பு result;
}

/*
 * This is the call which starts the GUI.
 */
    வெறுமை
gui_init(வெறுமை)
{
    win_T	*wp;
    tabpage_T	*tp;
    நிலையான முழு	recursive = 0;

    /*
     * It's possible to use ":gui" in a .gvimrc file.  The first half of this
     * function will then be executed at the first call, the rest by the
     * recursive call.  This allow the shell to be opened halfway reading a
     * gvimrc file.
     */
    எனில் (!recursive)
    {
	++recursive;

	clip_init(TRUE);

	// If can't initialize, don't try doing the rest
	எனில் (gui_init_check() == FAIL)
	{
	    --recursive;
	    clip_init(FALSE);
	    திரும்பு;
	}

	/*
	 * Reset 'paste'.  It's useful in the terminal, but not in the GUI.  It
	 * breaks the Paste toolbar button.
	 */
	set_option_மதிப்பு_give_err((char_u *)"paste", 0L, வெற்று, 0);

	// Set t_Co to the number of colors: RGB.
	set_color_மொத்தம்(256 * 256 * 256);

	/*
	 * Set up system-wide default menus.
	 */
#எனில் defined(SYS_MENU_FILE) && defined(FEAT_MENU)
	எனில் (vim_strchr(p_go, GO_NOSYSMENU) == வெற்று)
	{
	    sys_menu = TRUE;
	    do_source((char_u *)SYS_MENU_FILE, FALSE, DOSO_NONE, வெற்று);
	    sys_menu = FALSE;
	}
#endif

	/*
	 * Switch on the mouse by default, unless the user changed it already.
	 * This can then be changed in the .gvimrc.
	 */
	எனில் (!option_was_set((char_u *)"mouse"))
	    set_string_option_direct((char_u *)"mouse", -1,
					   (char_u *)"a", OPT_FREE, SID_NONE);

	/*
	 * If -U option given, use only the initializations from that file and
	 * nothing else.  Skip all initializations for "-U NONE" or "-u NORC".
	 */
	எனில் (use_gvimrc != வெற்று)
	{
	    எனில் (STRCMP(use_gvimrc, "NONE") != 0
		    && STRCMP(use_gvimrc, "NORC") != 0
		    && do_source(use_gvimrc, FALSE, DOSO_NONE, வெற்று) != OK)
		semsg(_(e_cannot_read_from_str), use_gvimrc);
	}
	இல்லையெனில்
	{
	    /*
	     * Get system wide defaults for gvim, only when file name defined.
	     */
#ifdef SYS_GVIMRC_FILE
	    do_source((char_u *)SYS_GVIMRC_FILE, FALSE, DOSO_NONE, வெற்று);
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
	    எனில் (process_env((char_u *)"GVIMINIT", FALSE) == FAIL
		 && do_source((char_u *)USR_GVIMRC_FILE, TRUE,
						     DOSO_GVIMRC, வெற்று) == FAIL
#ifdef USR_GVIMRC_FILE2
		 && do_source((char_u *)USR_GVIMRC_FILE2, TRUE,
						     DOSO_GVIMRC, வெற்று) == FAIL
#endif
#ifdef USR_GVIMRC_FILE3
		 && do_source((char_u *)USR_GVIMRC_FILE3, TRUE,
						     DOSO_GVIMRC, வெற்று) == FAIL
#endif
				)
	    {
#ifdef USR_GVIMRC_FILE4
		(வெறுமை)do_source((char_u *)USR_GVIMRC_FILE4, TRUE,
							    DOSO_GVIMRC, வெற்று);
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
	    எனில் (p_exrc)
	    {
#ifdef UNIX
		{
		    stat_T s;

		    // if ".gvimrc" file is not owned by user, set 'secure'
		    // mode
		    எனில் (mch_stat(GVIMRC_FILE, &s) || s.st_uid != getuid())
			secure = p_secure;
		}
#இல்லையெனில்
		secure = p_secure;
#endif

		எனில் (       fullpathcmp((char_u *)USR_GVIMRC_FILE,
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
		    do_source((char_u *)GVIMRC_FILE, TRUE, DOSO_GVIMRC, வெற்று);

		எனில் (secure == 2)
		    need_wait_return = TRUE;
		secure = 0;
	    }
	}

	எனில் (need_wait_return || செய்தி_didany)
	    wait_return(TRUE);

	--recursive;
    }

    // If recursive call opened the shell, return here from the first call
    எனில் (gui.in_use)
	திரும்பு;

    /*
     * Create the GUI shell.
     */
    gui.in_use = true;		// Must be set after menus have been set up
#எனில் defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
    gui.scale = 1.0; // Default value
#endif
    எனில் (gui_mch_init() == FAIL)
	செல் error;

    // Avoid a delay for an error message that was printed in the terminal
    // where Vim was started.
    emsg_on_display = FALSE;
    செய்தி_scrolled = 0;
    clear_sb_text(TRUE);
    need_wait_return = FALSE;
    செய்தி_didany = FALSE;

    /*
     * Check validity of any generic resources that may have been loaded.
     */
    எனில் (gui.border_width < 0)
	gui.border_width = 0;

    /*
     * Set up the fonts.  First use a font specified with "-fn" or "-font".
     */
    எனில் (font_argument != வெற்று)
	set_option_மதிப்பு_give_err((char_u *)"gfn",
					       0L, (char_u *)font_argument, 0);
    எனில் (
#ifdef FEAT_XFONTSET
	    (*p_guifontset == NUL
	     || gui_init_font(p_guifontset, TRUE) == FAIL) &&
#endif
	    gui_init_font(*p_guifont == NUL ? hl_get_font_name()
						  : p_guifont, FALSE) == FAIL)
    {
	emsg(_(e_cannot_start_gui_no_valid_font_found));
	செல் error2;
    }
    எனில் (gui_get_wide_font() == FAIL)
	emsg(_(e_guifontwide_invalid));

    gui.இலக்கம்_cols = Columns;
    gui.இலக்கம்_rows = Rows;
    gui_reset_scroll_region();

    // Create initial scrollbars
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_இடது], SBAR_இடது, wp);
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_வலது], SBAR_வலது, wp);
    }
    gui_create_scrollbar(&gui.bottom_sbar, SBAR_BOTTOM, வெற்று);

#ifdef FEAT_MENU
    gui_create_initial_menus(வேர்_menu);
#endif
#ifdef FEAT_SIGN_ICONS
    sign_gui_started();
#endif

    // Configure the desired menu and scrollbars
    gui_init_which_components(வெற்று);

    // All components of the GUI have been created now
    gui.shell_created = true;

#ifdef FEAT_GUI_MSWIN
    // Set the shell size, adjusted for the screen size.  For GTK this only
    // works after the shell has been opened, thus it is further down.
    // If the window is already maximized (e.g. when --windowid is passed in),
    // we want to use the system-provided dimensions by passing FALSE to
    // mustset. Otherwise, we want to initialize with the default rows/columns.
    எனில் (gui_mch_maximized())
	gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    இல்லையெனில்
	gui_set_shellsize(TRUE, TRUE, RESIZE_BOTH);
#இல்லையெனில்
# ifndef FEAT_GUI_GTK
    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
# endif
#endif
#எனில் defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    // Need to set the size of the menubar after all the menus have been
    // created.
    gui_mch_compute_menu_height((Widget)0);
#endif

    /*
     * Actually open the GUI shell.
     */
    எனில் (gui_mch_open() != FAIL)
    {
	maketitle();
	resettitle();

	init_gui_options();
#ifdef FEAT_ARABIC
	// Our GUI can't do bidi.
	p_tbidi = FALSE;
#endif
#எனில் defined(FEAT_GUI_GTK)
	// Give GTK+ a chance to put all widget's into place.
	gui_mch_update();

# ifdef FEAT_MENU
	// If there is no 'm' in 'guioptions' we need to remove the menu now.
	// It was still there to make F10 work.
	எனில் (vim_strchr(p_go, GO_MENUS) == வெற்று)
	{
	    bool save_starting = gui.starting;

	    gui.starting = false;
	    gui_mch_enable_menu(FALSE);
	    gui.starting = save_starting;
	    gui_mch_update();
	}
# endif

	// Now make sure the shell fits on the screen.
	எனில் (gui_mch_maximized())
	    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
	இல்லையெனில்
	    gui_set_shellsize(TRUE, TRUE, RESIZE_BOTH);
#endif
	// When 'lines' was set while starting up the topframe may have to be
	// resized.
	win_new_shellsize();

#ifdef FEAT_BEVAL_GUI
	// Always create the Balloon Evaluation area, but disable it when
	// 'ballooneval' is off.
	எனில் (balloonEval != வெற்று)
	{
# ifdef FEAT_VARTABS
	    vim_free(balloonEval->vts);
# endif
	    vim_free(balloonEval);
	}
	balloonEvalForTerm = FALSE;
# ifdef FEAT_GUI_GTK
	balloonEval = gui_mch_create_beval_area(gui.drawarea, வெற்று,
						     &general_beval_cb, வெற்று);
# இல்லையெனில்
#  எனில் defined(FEAT_GUI_MOTIF)
	{
	    வெளி Widget	textArea;
	    balloonEval = gui_mch_create_beval_area(textArea, வெற்று,
						     &general_beval_cb, வெற்று);
	}
#  இல்லையெனில்
#   ifdef FEAT_GUI_MSWIN
	balloonEval = gui_mch_create_beval_area(வெற்று, வெற்று,
						     &general_beval_cb, வெற்று);
#   endif
#  endif
# endif
	எனில் (!p_beval)
	    gui_mch_disable_beval_area(balloonEval);
#endif

#ifndef FEAT_GUI_MSWIN
	// In the GUI modifiers are prepended to keys.
	// Don't do this for MS-Windows yet, it sends CTRL-K without the
	// modifier.
	seen_Modify_Other_சாவிகள் = TRUE;
#endif

#எனில் defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	எனில் (!im_xim_isvalid_imactivate())
	    emsg(_(e_மதிப்பு_of_imactivatekey_is_invalid));
#endif
	// When 'cmdheight' was set during startup it may not have taken
	// effect yet.
	எனில் (p_ch != 1L)
	    command_height();

	திரும்பு;
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


    வெறுமை
gui_exit(முழு rc)
{
    // don't free the fonts, it leads to a BUS error
    // richard@whitequeen.com Jul 99
    free_highlight_fonts();
    gui.in_use = false;
    gui_mch_exit(rc);
}

#எனில் defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_PHOTON)
# define NEED_GUI_UPDATE_SCREEN 1
/*
 * Called when the GUI shell is closed by the user.  If there are no changed
 * files Vim exits, otherwise there will be a dialog to ask the user what to
 * do.
 * When this function returns, Vim should NOT exit!
 */
    வெறுமை
gui_shell_closed(வெறுமை)
{
    cmdmod_T	    save_cmdmod = cmdmod;

    எனில் (before_quit_autocmds(curwin, TRUE, FALSE))
	திரும்பு;

    // Only exit when there are no changed files
    exiting = TRUE;
# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    cmdmod.cmod_flags |= CMOD_CONFIRM;
# endif
    // If there are changed buffers, present the user with a dialog if
    // possible, otherwise give an error message.
    எனில் (!check_changed_any(FALSE, FALSE))
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
    முழு
gui_init_font(char_u *font_list, முழு fontset UNUSED)
{
#define FONTLEN 320
    char_u	font_name[FONTLEN];
    முழு		font_list_empty = FALSE;
    முழு		ret = FAIL;

    எனில் (!gui.in_use)
	திரும்பு FAIL;

    font_name[0] = NUL;
    எனில் (*font_list == NUL)
	font_list_empty = TRUE;
    இல்லையெனில்
    {
#ifdef FEAT_XFONTSET
	// When using a fontset, the whole list of fonts is one name.
	எனில் (fontset)
	    ret = gui_mch_init_font(font_list, TRUE);
	இல்லையெனில்
#endif
	    போது (*font_list != NUL)
	    {
		// Isolate one comma separated font name.
		(வெறுமை)copy_option_part(&font_list, font_name, FONTLEN, ",");

		// Careful!!!  The Win32 version of gui_mch_init_font(), when
		// called with "*" will change p_guifont to the selected font
		// name, which frees the old value.  This makes font_list
		// invalid.  Thus when OK is returned here, font_list must no
		// longer be used!
		எனில் (gui_mch_init_font(font_name, FALSE) == OK)
		{
#ifdef USE_SET_GUIFONTWIDE
		    // If it's a Unicode font, try setting 'guifontwide' to a
		    // similar double-width font.
		    எனில் ((p_guifontwide == வெற்று || *p_guifontwide == NUL)
				&& strstr((எழுத்து *)font_name, "10646") != வெற்று)
			set_guifontwide(font_name);
#endif
		    ret = OK;
		    நிறுத்து;
		}
	    }
    }

    எனில் (ret != OK
	    && STRCMP(font_list, "*") != 0
	    && (font_list_empty || gui.norm_font == NOFONT))
    {
	/*
	 * Couldn't load any font in 'font_list', keep the current font if
	 * there is one.  If 'font_list' is empty, or if there is no current
	 * font, tell gui_mch_init_font() to try to find a font we can load.
	 */
	ret = gui_mch_init_font(வெற்று, FALSE);
    }

    எனில் (ret == OK)
    {
#ifndef FEAT_GUI_GTK
	// Set normal font as current font
# ifdef FEAT_XFONTSET
	எனில் (gui.fontset != NOFONTSET)
	    gui_mch_set_fontset(gui.fontset);
	இல்லையெனில்
# endif
	    gui_mch_set_font(gui.norm_font);
#endif
	gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    }

    திரும்பு ret;
}

#ifdef USE_SET_GUIFONTWIDE
/*
 * Try setting 'guifontwide' to a font twice as wide as "name".
 */
    நிலையான வெறுமை
set_guifontwide(char_u *name)
{
    முழு		i = 0;
    char_u	wide_name[FONTLEN + 10]; // room for 2 * width and '*'
    char_u	*wp = வெற்று;
    char_u	*p;
    GuiFont	font;

    wp = wide_name;
    க்கு (p = name; *p != NUL; ++p)
    {
	*wp++ = *p;
	எனில் (*p == '-')
	{
	    ++i;
	    எனில் (i == 6)		// font type: change "--" to "-*-"
	    {
		எனில் (p[1] == '-')
		    *wp++ = '*';
	    }
	    இல்லையெனில் எனில் (i == 12)	// found the width
	    {
		++p;
		i = getdigits(&p);
		எனில் (i != 0)
		{
		    // Double the width specification.
		    sprintf((எழுத்து *)wp, "%d%s", i * 2, p);
		    font = gui_mch_get_font(wide_name, FALSE);
		    எனில் (font != NOFONT)
		    {
			gui_mch_free_font(gui.wide_font);
			gui.wide_font = font;
			set_string_option_direct((char_u *)"gfw", -1,
						      wide_name, OPT_FREE, 0);
		    }
		}
		நிறுத்து;
	    }
	}
    }
}
#endif

/*
 * Get the font for 'guifontwide'.
 * Return FAIL for an invalid font name.
 */
    முழு
gui_get_wide_font(வெறுமை)
{
    GuiFont	font = NOFONT;
    char_u	font_name[FONTLEN];
    char_u	*p;

    எனில் (!gui.in_use)	    // Can't allocate font yet, assume it's OK.
	திரும்பு OK;	    // Will give an error message later.

    எனில் (p_guifontwide != வெற்று && *p_guifontwide != NUL)
    {
	க்கு (p = p_guifontwide; *p != NUL; )
	{
	    // Isolate one comma separated font name.
	    (வெறுமை)copy_option_part(&p, font_name, FONTLEN, ",");
	    font = gui_mch_get_font(font_name, FALSE);
	    எனில் (font != NOFONT)
		நிறுத்து;
	}
	எனில் (font == NOFONT)
	    திரும்பு FAIL;
    }

    gui_mch_free_font(gui.wide_font);
#ifdef FEAT_GUI_GTK
    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    எனில் (font != NOFONT && gui.norm_font != NOFONT
			 && pango_font_description_equal(font, gui.norm_font))
    {
	gui.wide_font = NOFONT;
	gui_mch_free_font(font);
    }
    இல்லையெனில்
#endif
	gui.wide_font = font;
#ifdef FEAT_GUI_MSWIN
    gui_mch_wide_font_changed();
#இல்லையெனில்
    /*
     * TODO: setup wide_bold_font, wide_ital_font and wide_boldital_font to
     * support those fonts for 'guifontwide'.
     */
#endif
    திரும்பு OK;
}

#எனில் defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * Set list of ascii characters that combined can create ligature.
 * Store them in char map for quick access from gui_gtk_draw_string.
 */
    வெறுமை
gui_set_ligatures(வெறுமை)
{
    char_u	*p;

    எனில் (*p_guiligatures != NUL)
    {
	// check for invalid characters
	க்கு (p = p_guiligatures; *p != NUL; ++p)
	    எனில் (*p < 32 || *p > 127)
	    {
		emsg(_(e_ascii_code_not_in_range));
		திரும்பு;
	    }

	// store valid setting into ligatures_map
	CLEAR_FIELD(gui.ligatures_map);
	க்கு (p = p_guiligatures; *p != NUL; ++p)
	    gui.ligatures_map[*p] = 1;
    }
    இல்லையெனில்
	CLEAR_FIELD(gui.ligatures_map);
}

/*
 * Adjust the columns to undraw for when the cursor is on ligatures.
 */
    நிலையான வெறுமை
gui_adjust_undraw_cursor_for_ligatures(முழு *startcol, முழு *endcol)
{
    முழு off;

    எனில் (Screen_வரிகள் == வெற்று || *p_guiligatures == NUL)
	திரும்பு;

    // expand before the cursor for all the chars in gui.ligatures_map
    off = வரி_Offset[gui.cursor_row] + *startcol;
    எனில் (gui.ligatures_map[Screen_வரிகள்[off]])
	போது (*startcol > 0 && gui.ligatures_map[Screen_வரிகள்[--off]])
	    (*startcol)--;

    // expand after the cursor for all the chars in gui.ligatures_map
    off = வரி_Offset[gui.cursor_row] + *endcol;
    எனில் (gui.ligatures_map[Screen_வரிகள்[off]])
	போது (*endcol < ((முழு)screen_Columns - 1)
				      && gui.ligatures_map[Screen_வரிகள்[++off]])
	   (*endcol)++;
}
#endif

    நிலையான வெறுமை
gui_set_cursor(முழு row, முழு col)
{
    gui.row = row;
    gui.col = col;
}

/*
 * gui_check_pos - check if the cursor is on the screen.
 */
    நிலையான வெறுமை
gui_check_pos(வெறுமை)
{
    எனில் (gui.row >= screen_Rows)
	gui.row = screen_Rows - 1;
    எனில் (gui.col >= screen_Columns)
	gui.col = screen_Columns - 1;
    எனில் (gui.cursor_row >= screen_Rows || gui.cursor_col >= screen_Columns)
	gui.cursor_is_valid = false;
}

/*
 * Redraw the cursor if necessary or when forced.
 * Careful: The contents of ScreenLines[] must match what is on the screen,
 * otherwise this goes wrong.  May need to call out_flush() first.
 */
    வெறுமை
gui_update_cursor(
    முழு		force,		 // when TRUE, update even when not moved
    முழு		clear_selection) // clear selection under cursor
{
    முழு		cur_width = 0;
    முழு		cur_height = 0;
    முழு		old_hl_mask;
    cursorentry_T *shape;
    முழு		id;
#ifdef FEAT_TERMINAL
    guicolor_T	shape_fg = INVALCOLOR;
    guicolor_T	shape_bg = INVALCOLOR;
#endif
    guicolor_T	cfg, cbg, cc;	// cursor fore-/background color
    முழு		cattr;		// cursor attributes
    முழு		attr;
    attrentry_T *aep = வெற்று;
#எனில் (defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)) && !defined(USE_GTK4)
    bool	lig_இடது = false, lig_வலது = false;
#endif

    // Don't update the cursor when halfway busy scrolling or the screen size
    // doesn't match 'columns' and 'lines.  ScreenLines[] isn't valid then.
    எனில் (!can_update_cursor || screen_Columns != gui.இலக்கம்_cols
					       || screen_Rows != gui.இலக்கம்_rows)
	திரும்பு;

    gui_check_pos();

    எனில் (gui.cursor_is_valid && !force
		&& gui.row == gui.cursor_row && gui.col == gui.cursor_col)
	திரும்பு;

    gui_undraw_cursor();

    // If a cursor-less sleep is ongoing, leave the cursor invisible
    எனில் (cursor_is_sleeping())
	திரும்பு;

    எனில் (gui.row < 0)
	திரும்பு;
#ifdef HAVE_உள்ளீடு_METHOD
    எனில் (gui.row != gui.cursor_row || gui.col != gui.cursor_col)
	im_set_position(gui.row, gui.col);
#endif
    gui.cursor_row = gui.row;
    gui.cursor_col = gui.col;

    // Only write to the screen after ScreenLines[] has been initialized
    எனில் (!screen_cleared || Screen_வரிகள் == வெற்று)
	திரும்பு;

    // Clear the selection if we are about to write over it
    எனில் (clear_selection)
	clip_may_clear_selection(gui.row, gui.row);
    // Check that the cursor is inside the shell (resizing may have made
    // it invalid)
    எனில் (gui.row >= screen_Rows || gui.col >= screen_Columns)
	திரும்பு;

    gui.cursor_is_valid = true;

    /*
     * How the cursor is drawn depends on the current mode.
     * When in a terminal window use the shape/color specified there.
     */
#ifdef FEAT_TERMINAL
    எனில் (terminal_is_active())
	shape = term_get_cursor_shape(&shape_fg, &shape_bg);
    இல்லையெனில்
#endif
	shape = &shape_table[get_shape_idx(FALSE)];
    எனில் (State & MODE_LANGMAP)
	id = shape->id_lm;
    இல்லையெனில்
	id = shape->id;

    // get the colors and attributes for the cursor.  Default is inverted
    cfg = INVALCOLOR;
    cbg = INVALCOLOR;
    cattr = HL_INVERSE;
    gui_mch_set_blinking(shape->blinkwait,
	    shape->blinkon,
	    shape->blinkoff);
    எனில் (shape->blinkwait == 0 || shape->blinkon == 0
	    || shape->blinkoff == 0)
	gui_mch_stop_blink(FALSE);
#ifdef FEAT_TERMINAL
    எனில் (shape_bg != INVALCOLOR)
    {
	cattr = 0;
	cfg = shape_fg;
	cbg = shape_bg;
    }
    இல்லையெனில்
#endif
	எனில் (id > 0)
	{
	    cattr = syn_id2colors(id, &cfg, &cbg);
#எனில் defined(HAVE_உள்ளீடு_METHOD)
	    {
		நிலையான முழு iid;
		guicolor_T fg, bg;

		எனில் (
# எனில் defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
			preedit_get_status()
# இல்லையெனில்
			im_get_status()
# endif
		   )
		{
		    iid = syn_name2id((char_u *)"CursorIM");
		    எனில் (iid > 0)
		    {
			syn_id2colors(iid, &fg, &bg);
			எனில் (bg != INVALCOLOR)
			    cbg = bg;
			எனில் (fg != INVALCOLOR)
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
    attr = ScreenAttrs[வரி_Offset[gui.row] + gui.col];
    எனில் (attr > HL_ALL)
	aep = syn_gui_attr2entry(attr);
    எனில் (aep != வெற்று)
    {
	attr = aep->ae_attr;
	எனில் (cfg == INVALCOLOR)
	    cfg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.bg_color
		    : aep->ae_u.gui.fg_color);
	எனில் (cbg == INVALCOLOR)
	    cbg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.fg_color
		    : aep->ae_u.gui.bg_color);
    }
    எனில் (cfg == INVALCOLOR)
	cfg = (attr & HL_INVERSE) ? gui.back_pixel : gui.norm_pixel;
    எனில் (cbg == INVALCOLOR)
	cbg = (attr & HL_INVERSE) ? gui.norm_pixel : gui.back_pixel;

#ifdef FEAT_XIM
    எனில் (aep != வெற்று)
    {
	xim_bg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.fg_color
		: aep->ae_u.gui.bg_color);
	xim_fg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.bg_color
		: aep->ae_u.gui.fg_color);
	எனில் (xim_bg_color == INVALCOLOR)
	    xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
		: gui.back_pixel;
	எனில் (xim_fg_color == INVALCOLOR)
	    xim_fg_color = (attr & HL_INVERSE) ? gui.back_pixel
		: gui.norm_pixel;
    }
    இல்லையெனில்
    {
	xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
	    : gui.back_pixel;
	xim_fg_color = (attr & HL_INVERSE) ? gui.back_pixel
	    : gui.norm_pixel;
    }
#endif

    attr &= ~HL_INVERSE;
    எனில் (cattr & HL_INVERSE)
    {
	cc = cbg;
	cbg = cfg;
	cfg = cc;
    }
    cattr &= ~HL_INVERSE;

    /*
     * When we don't have window focus, draw a hollow cursor.
     */
    எனில் (!gui.in_focus)
    {
#ifdef USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, -1, -1);
#இல்லையெனில்
	gui_mch_draw_hollow_cursor(cbg);
#endif
	திரும்பு;
    }

#ifdef USE_GTK4
    // Make sure that character underneath is drawn again in case it is part of
    // a ligature.
    gui_redraw_block(gui.row, gui.col, gui.row, gui.col, GUI_MON_NOCLEAR);
    // gui_redraw_block() will invalidate the cursor
    gui.cursor_is_valid = true;
#endif
#எனில் defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // If cursor is in the middle of a ligature, then split the ligature at
    // the cursor boundaries by redrawing those cells again.
    க்கு (முழு c = gui.col - 1; c >= 0; c--)
	எனில் (!gui.ligatures_map[Screen_வரிகள்[வரி_Offset[gui.row] + c]])
	{
	    எனில் (c < gui.col - 1)
	    {
		gui_redraw_block(gui.row, c + 1, gui.row, gui.col - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_இடது = true;
# endif
	    }
	    நிறுத்து;
	}

    க்கு (முழு c = gui.col + 1; c < screen_Columns - 1; c++)
	எனில் (!gui.ligatures_map[Screen_வரிகள்[வரி_Offset[gui.row] + c]])
	{
	    எனில் (c > gui.col + 1)
	    {
		gui_redraw_block(gui.row, gui.col + 1, gui.row, c - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_வலது = true;
# endif
	    }
	    நிறுத்து;
	}

# ifndef USE_GTK4
    எனில் ((lig_இடது || lig_வலது) && shape->shape != SHAPE_BLOCK)
    {
	// Because the cursor is not drawn with gui_screenchar(), must blit the
	// cell again (with its background), so that old ligature does not
	// remain on screen. Not needed for GtkSnapshot, because char beneath is
	// always rerendered (see above ifdef).
	முழு old = gui.col;
	gui.highlight_mask = ScreenAttrs[வரி_Offset[gui.row] + gui.col];
	(வெறுமை)gui_screenchar(வரி_Offset[gui.row] + gui.col,
		GUI_MON_NOCLEAR, (guicolor_T)0, (guicolor_T)0, 0);
	gui.col = old;
    }
# endif
    // gui_redraw_block()/gui_screenchar() may invalidate the cursor, make sure
    // to validate it again.
    gui.cursor_is_valid = true;
#endif

    old_hl_mask = gui.highlight_mask;
    எனில் (shape->shape == SHAPE_BLOCK)
    {
#ifdef USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, 0, 0);
#இல்லையெனில்
	/*
	 * Draw the text character with the cursor colors.	Use the
	 * character attributes plus the cursor attributes.
	 */
	gui.highlight_mask = (cattr | attr);
	(வெறுமை)gui_screenchar(வரி_Offset[gui.row] + gui.col,
		GUI_MON_IS_CURSOR | GUI_MON_NOCLEAR, cfg, cbg, 0);
#endif
    }
    இல்லையெனில்
    {
#எனில் defined(FEAT_RIGHTLEFT)
	முழு	    col_off = FALSE;
#endif
	/*
	 * First draw the partial cursor, then overwrite with the text
	 * character, using a transparent background.
	 */
	எனில் (shape->shape == SHAPE_VER)
	{
	    cur_height = gui.char_height;
	    cur_width = (gui.char_width * shape->percentage + 99) / 100;
	}
	இல்லையெனில்
	{
	    cur_height = (gui.char_height * shape->percentage + 99) / 100;
	    cur_width = gui.char_width;
	}
	எனில் (has_mbyte && (*mb_off2cells)(வரி_Offset[gui.row] + gui.col,
		    வரி_Offset[gui.row] + screen_Columns) > 1)
	{
	    // Double wide character.
	    எனில் (shape->shape != SHAPE_VER)
		cur_width += gui.char_width;
#ifdef FEAT_RIGHTLEFT
	    எனில் (CURSOR_BAR_வலது)
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
#இல்லையெனில்
	gui_mch_draw_part_cursor(cur_width, cur_height, cbg);
#endif
#எனில் defined(FEAT_RIGHTLEFT)
	எனில் (col_off)
	    --gui.col;
#endif

	// Doesn't seem to work for MSWindows. We call gui_redraw_block() above
	// for GtkSnapshot.
#எனில் !defined(FEAT_GUI_MSWIN) && !defined(USE_GTK4)
	gui.highlight_mask = ScreenAttrs[வரி_Offset[gui.row] + gui.col];
	(வெறுமை)gui_screenchar(வரி_Offset[gui.row] + gui.col,
		GUI_MON_TRS_CURSOR | GUI_MON_NOCLEAR,
		(guicolor_T)0, (guicolor_T)0, 0);
#endif
    }
    gui.highlight_mask = old_hl_mask;
}

#எனில் defined(FEAT_MENU)
    நிலையான வெறுமை
gui_position_menu(வெறுமை)
{
# எனில் !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
    எனில் (gui.menu_is_active && gui.in_use)
	gui_mch_set_menu_pos(0, 0, gui.menu_width, gui.menu_height);
# endif
}
#endif

/*
 * Position the various GUI components (text area, menu).  The vertical
 * scrollbars are NOT handled here.  See gui_update_scrollbars().
 */
    நிலையான வெறுமை
gui_position_components(முழு total_width UNUSED)
{
    முழு	    text_area_x;
    முழு	    text_area_y;
    முழு	    text_area_width;
    முழு	    text_area_height;

    // avoid that moving components around generates events
    ++hold_gui_events;

    text_area_x = 0;
    எனில் (gui.which_scrollbars[SBAR_இடது])
	text_area_x += gui.scrollbar_width;

    text_area_y = 0;
#எனில் defined(FEAT_MENU) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    gui.menu_width = total_width;
    எனில் (gui.menu_is_active)
	text_area_y += gui.menu_height;
#endif

#எனில் defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
    எனில் (gui_has_tabline())
	text_area_y += gui.tabline_height;
#endif

#எனில் defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    எனில் (vim_strchr(p_go, GO_TOOLBAR) != வெற்று)
    {
# எனில் defined(FEAT_GUI_HAIKU)
	gui_mch_set_toolbar_pos(0, text_area_y,
				gui.menu_width, gui.toolbar_height);
# endif
	text_area_y += gui.toolbar_height;
    }
#endif

#எனில் defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_HAIKU)
    gui_mch_set_tabline_pos(0, text_area_y,
    gui.menu_width, gui.tabline_height);
    எனில் (gui_has_tabline())
	text_area_y += gui.tabline_height;
#endif

    text_area_width = gui.இலக்கம்_cols * gui.char_width + gui.border_offset * 2;
    text_area_height = gui.இலக்கம்_rows * gui.char_height + gui.border_offset * 2;

    gui_mch_set_text_area_pos(text_area_x,
			      text_area_y,
			      text_area_width,
			      text_area_height
#எனில் defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
				  + xim_get_status_area_height()
#endif
			      );
#ifdef FEAT_MENU
    gui_position_menu();
#endif
    எனில் (gui.which_scrollbars[SBAR_BOTTOM])
	gui_mch_set_scrollbar_pos(&gui.bottom_sbar,
				  text_area_x,
				  text_area_y + text_area_height
					+ gui_mch_get_scrollbar_ypadding(),
				  text_area_width,
				  gui.scrollbar_height);
    gui.இடது_sbar_x = 0;
    gui.வலது_sbar_x = text_area_x + text_area_width
					+ gui_mch_get_scrollbar_xpadding();

    --hold_gui_events;
}

/*
 * Get the width of the widgets and decorations to the side of the text area.
 */
    முழு
gui_get_base_width(வெறுமை)
{
    முழு	    base_width;

    base_width = 2 * gui.border_offset;
    எனில் (gui.which_scrollbars[SBAR_இடது])
	base_width += gui.scrollbar_width;
    எனில் (gui.which_scrollbars[SBAR_வலது])
	base_width += gui.scrollbar_width;
    திரும்பு base_width;
}

/*
 * Get the height of the widgets and decorations above and below the text area.
 */
    முழு
gui_get_base_height(வெறுமை)
{
    முழு	    base_height;

    base_height = 2 * gui.border_offset;
    எனில் (gui.which_scrollbars[SBAR_BOTTOM])
	base_height += gui.scrollbar_height;
#ifdef FEAT_GUI_GTK
    // We can't take the sizes properly into account until anything is
    // realized.  Therefore we recalculate all the values here just before
    // setting the size. (--mdcki)
#இல்லையெனில்
# ifdef FEAT_MENU
    எனில் (gui.menu_is_active)
	base_height += gui.menu_height;
# endif
# ifdef FEAT_TOOLBAR
    எனில் (vim_strchr(p_go, GO_TOOLBAR) != வெற்று)
	base_height += gui.toolbar_height;
# endif
# எனில் defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    எனில் (gui_has_tabline())
	base_height += gui.tabline_height;
# endif
# எனில் defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    base_height += gui_mch_text_area_extra_height();
# endif
#endif
    திரும்பு base_height;
}

/*
 * Should be called after the GUI shell has been resized.  Its arguments are
 * the new width and height of the shell in pixels.
 */
    வெறுமை
gui_resize_shell(முழு pixel_width, முழு pixel_height)
{
    நிலையான முழு	busy = FALSE;

    எனில் (!gui.shell_created)	    // ignore when still initializing
	திரும்பு;

    /*
     * Can't resize the screen while it is being redrawn.  Remember the new
     * size and handle it later.
     */
    எனில் (updating_screen || busy)
    {
	new_pixel_width = pixel_width;
	new_pixel_height = pixel_height;
	திரும்பு;
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
#எனில் defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk_init_decor_height();
#endif

    gui.இலக்கம்_cols = (pixel_width - gui_get_base_width()) / gui.char_width;
    gui.இலக்கம்_rows = (pixel_height - gui_get_base_height()) / gui.char_height;

#ifdef USE_GTK4
    gui_gtk4_update_size();
#endif

    gui_position_components(pixel_width);
    gui_reset_scroll_region();

#எனில் defined(FEAT_GUI_GTK) && defined(USE_GTK4) && !defined(USE_GTK4)
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
    எனில் (State == MODE_ASKMORE || State == MODE_CONFIRM)
	gui.row = gui.இலக்கம்_rows;

    // Only comparing Rows and Columns may be sufficient, but let's stay on
    // the safe side.
    எனில் (gui.இலக்கம்_rows != screen_Rows || gui.இலக்கம்_cols != screen_Columns
	    || gui.இலக்கம்_rows != Rows || gui.இலக்கம்_cols != Columns || gui.force_redraw)
    {
	shell_resized();
	gui.force_redraw = 0;
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    gui_update_scrollbars(TRUE);
    gui_update_cursor(FALSE, TRUE);
#எனில் defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(pixel_width, pixel_height);
#endif
#எனில் defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
    xim_set_status_area();
#endif

    busy = FALSE;

    // We may have been called again while redrawing the screen.
    // Need to do it all again with the latest size then.  But only if the size
    // actually changed.
    எனில் (new_pixel_height)
    {
	எனில் (pixel_width == new_pixel_width && pixel_height == new_pixel_height)
	{
	    new_pixel_width = 0;
	    new_pixel_height = 0;
	}
	இல்லையெனில்
	{
	    pixel_width = new_pixel_width;
	    pixel_height = new_pixel_height;
	    செல் again;
	}
    }
}

/*
 * Check if gui_resize_shell() must be called.
 */
    வெறுமை
gui_may_resize_shell(வெறுமை)
{
    எனில் (new_pixel_height)
    {
	// careful: gui_resize_shell() may postpone the resize again if we
	// were called indirectly by it
	gui_resize_shell(new_pixel_width, new_pixel_height);
    }
}

    முழு
gui_get_shellsize(வெறுமை)
{
    Rows = gui.இலக்கம்_rows;
    Columns = gui.இலக்கம்_cols;
    திரும்பு OK;
}

/*
 * Set the size of the Vim shell according to Rows and Columns.
 * If "fit_to_display" is TRUE then the size may be reduced to fit the window
 * on the screen.
 * When "mustset" is TRUE the size was set by the user. When FALSE a UI
 * component was added or removed (e.g., a scrollbar).
 */
    வெறுமை
gui_set_shellsize(
    முழு		mustset UNUSED,
    முழு		fit_to_display,
    முழு		direction)		// RESIZE_HOR, RESIZE_VER
{
    முழு		base_width;
    முழு		base_height;
    முழு		width;
    முழு		height;
    முழு		min_width;
    முழு		min_height;
    முழு		screen_w;
    முழு		screen_h;
#ifdef FEAT_GUI_GTK
    முழு		un_maximize = mustset;
    முழு		did_adjust = 0;
#endif
    முழு		x = -1, y = -1;

    எனில் (!gui.shell_created)
	திரும்பு;

#எனில் defined(MSWIN) || defined(FEAT_GUI_GTK)
    // If not setting to a user specified size and maximized, calculate the
    // number of characters that fit in the maximized window.
    // FIXME: gui_mch_newfont() is called here even when the font hasn't
    // changed at all.  For example, ":set guioptions=k" triggers this path
    // via gui_init_which_components() -> gui_set_shellsize(FALSE, ...).
    // The intent is to keep the window size and recalculate Rows/Columns,
    // which has nothing to do with fonts.  This should be a separate
    // function with a more descriptive name.
    எனில் (!mustset && (vim_strchr(p_go, GO_KEEPWINSIZE) != வெற்று
						       || gui_mch_maximized()))
    {
	gui_mch_newfont();
	திரும்பு;
    }
#endif

    base_width = gui_get_base_width();
    base_height = gui_get_base_height();
    எனில் (fit_to_display)
	// Remember the original window position.
	(வெறுமை)gui_mch_get_winpos(&x, &y);

    width = Columns * gui.char_width + base_width;
    height = Rows * gui.char_height + base_height;

    எனில் (fit_to_display)
    {
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	எனில் ((direction & RESIZE_HOR) && width > screen_w)
	{
	    Columns = (screen_w - base_width) / gui.char_width;
	    எனில் (Columns < MIN_COLUMNS)
		Columns = MIN_COLUMNS;
	    width = Columns * gui.char_width + base_width;
#ifdef FEAT_GUI_GTK
	    ++did_adjust;
#endif
	}
	எனில் ((direction & RESIZE_VERT) && height > screen_h)
	{
	    Rows = (screen_h - base_height) / gui.char_height;
	    check_shellsize();
	    height = Rows * gui.char_height + base_height;
#ifdef FEAT_GUI_GTK
	    ++did_adjust;
#endif
	}
#ifdef FEAT_GUI_GTK
	எனில் (did_adjust == 2 || (width + gui.char_width >= screen_w
				     && height + gui.char_height >= screen_h))
	    // don't unmaximize if at maximum size
	    un_maximize = FALSE;
#endif
    }
    limit_screen_size();
    gui.இலக்கம்_cols = Columns;
    gui.இலக்கம்_rows = Rows;
#ifdef USE_GTK4
    // Keep the drawing area in sync with the size Vim is going to draw.
    gui_gtk4_update_size();
#endif

    min_width = base_width + MIN_COLUMNS * gui.char_width;
    min_height = base_height + MIN_வரிகள் * gui.char_height;
    min_height += tabline_height() * gui.char_height;

#ifdef FEAT_GUI_GTK
    எனில் (un_maximize)
    {
	// If the window size is smaller than the screen unmaximize the
	// window, otherwise resizing won't work.
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	எனில் ((width + gui.char_width < screen_w
				   || height + gui.char_height * 2 < screen_h)
		&& gui_mch_maximized())
	    gui_mch_unmaximize();
    }
#endif

    gui_mch_set_shellsize(width, height, min_width, min_height,
					  base_width, base_height, direction);

    எனில் (fit_to_display && x >= 0 && y >= 0)
    {
	// Some window managers put the Vim window left of/above the screen.
	// Only change the position if it wasn't already negative before
	// (happens on MS-Windows with a secondary monitor).
	gui_mch_update();
	எனில் (gui_mch_get_winpos(&x, &y) == OK && (x < 0 || y < 0))
	    gui_mch_set_winpos(x < 0 ? 0 : x, y < 0 ? 0 : y);
    }

    gui_position_components(width);
    gui_update_scrollbars(TRUE);
    gui_reset_scroll_region();

#எனில் defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(width, height);
#endif
}

/*
 * Called when Rows and/or Columns has changed.
 */
    வெறுமை
gui_new_shellsize(வெறுமை)
{
    gui_reset_scroll_region();
}

/*
 * Make scroll region cover whole screen.
 */
    நிலையான வெறுமை
gui_reset_scroll_region(வெறுமை)
{
    gui.scroll_region_உச்சி = 0;
    gui.scroll_region_bot = gui.இலக்கம்_rows - 1;
    gui.scroll_region_இடது = 0;
    gui.scroll_region_வலது = gui.இலக்கம்_cols - 1;
}

    நிலையான வெறுமை
gui_start_highlight(முழு mask)
{
    எனில் (mask > HL_ALL)		    // highlight code
	gui.highlight_mask = mask;
    இல்லையெனில்			    // mask
	gui.highlight_mask |= mask;
}

    வெறுமை
gui_stop_highlight(முழு mask)
{
    எனில் (mask > HL_ALL)		    // highlight code
	gui.highlight_mask = HL_NORMAL;
    இல்லையெனில்			    // mask
	gui.highlight_mask &= ~mask;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    வெறுமை
gui_clear_block(
    முழு	    row1,
    முழு	    col1,
    முழு	    row2,
    முழு	    col2)
{
    // Clear the selection if we are about to write over it
    clip_may_clear_selection(row1, row2);

    gui_mch_clear_block(row1, col1, row2, col2);

    // Invalidate cursor if it was in this block
    எனில் (       gui.cursor_row >= row1 && gui.cursor_row <= row2
	    && gui.cursor_col >= col1 && gui.cursor_col <= col2)
	gui.cursor_is_valid = false;
}

/*
 * Write code to update the cursor later.  This avoids the need to flush the
 * output buffer before calling gui_update_cursor().
 */
    வெறுமை
gui_update_cursor_later(வெறுமை)
{
    OUT_STR("\033|s");
}

    வெறுமை
gui_write(
    char_u	*s,
    முழு		len)
{
    char_u	*p;
    முழு		arg1 = 0, arg2 = 0;
    முழு		force_cursor = FALSE;	// force cursor update
    முழு		force_scrollbar = FALSE;
    நிலையான win_T	*old_curwin = வெற்று;

// #define DEBUG_GUI_WRITE
#ifdef DEBUG_GUI_WRITE
    {
	முழு i;
	char_u *str;

	printf("gui_write(%d):\n    ", len);
	க்கு (i = 0; i < len; i++)
	    எனில் (s[i] == ESC)
	    {
		எனில் (i != 0)
		    printf("\n    ");
		printf("<ESC>");
	    }
	    இல்லையெனில்
	    {
		str = transchar_byte(s[i]);
		எனில் (str[0] && str[1])
		    printf("<%s>", (எழுத்து *)str);
		இல்லையெனில்
		    printf("%s", (எழுத்து *)str);
	    }
	printf("\n");
    }
#endif
    போது (len)
    {
	எனில் (s[0] == ESC && s[1] == '|')
	{
	    p = s + 2;
	    எனில் (VIM_ISDIGIT(*p) || (*p == '-' && VIM_ISDIGIT(*(p + 1))))
	    {
		arg1 = getdigits(&p);
		எனில் (p > s + len)
		    நிறுத்து;
		எனில் (*p == ';')
		{
		    ++p;
		    arg2 = getdigits(&p);
		    எனில் (p > s + len)
			நிறுத்து;
		}
	    }
	    தேர்வு (*p)
	    {
		நிலை 'C':	// Clear screen
		    clip_scroll_selection(9999);
		    gui_mch_clear_all();
		    gui.cursor_is_valid = false;
		    force_scrollbar = TRUE;
		    நிறுத்து;
		நிலை 'M':	// Move cursor
		    gui_set_cursor(arg1, arg2);
		    நிறுத்து;
		நிலை 's':	// force cursor (shape) update
		    force_cursor = TRUE;
		    நிறுத்து;
		நிலை 'R':	// Set scroll region
		    எனில் (arg1 < arg2)
		    {
			gui.scroll_region_உச்சி = arg1;
			gui.scroll_region_bot = arg2;
		    }
		    இல்லையெனில்
		    {
			gui.scroll_region_உச்சி = arg2;
			gui.scroll_region_bot = arg1;
		    }
		    நிறுத்து;
		நிலை 'V':	// Set vertical scroll region
		    எனில் (arg1 < arg2)
		    {
			gui.scroll_region_இடது = arg1;
			gui.scroll_region_வலது = arg2;
		    }
		    இல்லையெனில்
		    {
			gui.scroll_region_இடது = arg2;
			gui.scroll_region_வலது = arg1;
		    }
		    நிறுத்து;
		நிலை 'd':	// Delete line
		    gui_delete_வரிகள்(gui.row, 1);
		    நிறுத்து;
		நிலை 'D':	// Delete lines
		    gui_delete_வரிகள்(gui.row, arg1);
		    நிறுத்து;
		நிலை 'i':	// Insert line
		    gui_செருகு_வரிகள்(gui.row, 1);
		    நிறுத்து;
		நிலை 'I':	// Insert lines
		    gui_செருகு_வரிகள்(gui.row, arg1);
		    நிறுத்து;
		நிலை '$':	// Clear to end-of-line
		    gui_clear_block(gui.row, gui.col, gui.row,
							    (முழு)Columns - 1);
		    நிறுத்து;
		நிலை 'h':	// Turn on highlighting
		    gui_start_highlight(arg1);
		    நிறுத்து;
		நிலை 'H':	// Turn off highlighting
		    gui_stop_highlight(arg1);
		    நிறுத்து;
		நிலை 'f':	// flash the window (visual bell)
		    gui_mch_flash(arg1 == 0 ? 20 : arg1);
		    நிறுத்து;
		இயல்பு:
		    p = s + 1;	// Skip the ESC
		    நிறுத்து;
	    }
	    len -= (முழு)(++p - s);
	    s = p;
	}
	இல்லையெனில் எனில் (s[0] < 0x20		// Ctrl character
#ifdef FEAT_SIGN_ICONS
		&& s[0] != SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
		&& s[0] != MULTISIGN_BYTE
# endif
#endif
		)
	{
	    எனில் (s[0] == '\n')		// NL
	    {
		gui.col = 0;
		எனில் (gui.row < gui.scroll_region_bot)
		    gui.row++;
		இல்லையெனில்
		    gui_delete_வரிகள்(gui.scroll_region_உச்சி, 1);
	    }
	    இல்லையெனில் எனில் (s[0] == '\r')	// CR
	    {
		gui.col = 0;
	    }
	    இல்லையெனில் எனில் (s[0] == '\b')	// Backspace
	    {
		எனில் (gui.col)
		    --gui.col;
	    }
	    இல்லையெனில் எனில் (s[0] == Ctrl_L)	// cursor-right
	    {
		++gui.col;
	    }
	    இல்லையெனில் எனில் (s[0] == Ctrl_G)	// Beep
	    {
		gui_mch_beep();
	    }
	    // Other Ctrl character: shouldn't happen!

	    --len;	// Skip this char
	    ++s;
	}
	இல்லையெனில்
	{
	    p = s;
	    போது (len > 0 && (
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
	    gui_outstr(s, (முழு)(p - s));
	    s = p;
	}
    }

    // Postponed update of the cursor (won't work if "can_update_cursor" isn't
    // set).
    எனில் (force_cursor)
	gui_update_cursor(TRUE, TRUE);

    // When switching to another window the dragging must have stopped.
    // Required for GTK, dragged_sb isn't reset.
    எனில் (old_curwin != curwin)
	gui.dragged_sb = SBAR_NONE;

    // Update the scrollbars after clearing the screen or when switched
    // to another window.
    // Update the horizontal scrollbar always, it's difficult to check all
    // situations where it might change.
    எனில் (force_scrollbar || old_curwin != curwin)
	gui_update_scrollbars(force_scrollbar);
    இல்லையெனில்
	gui_update_horiz_scrollbar(FALSE);
    old_curwin = curwin;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#எனில் defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#endif

    gui_may_flush();		    // In case vim decides to take a nap
}

/*
 * When ScreenLines[] is invalid, updating the cursor should not be done, it
 * produces wrong results.  Call gui_dont_update_cursor() before that code and
 * gui_can_update_cursor() afterwards.
 */
    வெறுமை
gui_dont_update_cursor(முழு undraw)
{
    எனில் (!gui.in_use)
	திரும்பு;

    // Undraw the cursor now, we probably can't do it after the change.
    எனில் (undraw)
	gui_undraw_cursor();
    can_update_cursor = FALSE;
}

    வெறுமை
gui_can_update_cursor(வெறுமை)
{
    can_update_cursor = TRUE;
    // No need to update the cursor right now, there is always more output
    // after scrolling.
}

/*
 * Disable issuing gui_mch_flush().
 */
    வெறுமை
gui_disable_flush(வெறுமை)
{
    ++disable_flush;
}

/*
 * Enable issuing gui_mch_flush().
 */
    வெறுமை
gui_enable_flush(வெறுமை)
{
    --disable_flush;
}

/*
 * Issue gui_mch_flush() if it is not disabled.
 */
    வெறுமை
gui_may_flush(வெறுமை)
{
    எனில் (disable_flush == 0)
	gui_mch_flush();
}

    நிலையான வெறுமை
gui_outstr(char_u *s, முழு len)
{
    முழு	    this_len;
    முழு	    cells;

    எனில் (len == 0)
	திரும்பு;

    எனில் (len < 0)
	len = (முழு)STRLEN(s);

    போது (len > 0)
    {
	எனில் (has_mbyte)
	{
	    // Find out how many chars fit in the current line.
	    cells = 0;
	    க்கு (this_len = 0; this_len < len; )
	    {
		cells += (*mb_ptr2cells)(s + this_len);
		எனில் (gui.col + cells > Columns)
		    நிறுத்து;
		this_len += (*mb_ptr2len)(s + this_len);
	    }
	    எனில் (this_len > len)
		this_len = len;	    // don't include following composing char
	}
	இல்லையெனில் எனில் (gui.col + len > Columns)
	    this_len = Columns - gui.col;
	இல்லையெனில்
	    this_len = len;

	(வெறுமை)gui_outstr_nowrap(s, this_len,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	s += this_len;
	len -= this_len;
	// fill up for a double-width char that doesn't fit.
	எனில் (len > 0 && gui.col < Columns)
	    (வெறுமை)gui_outstr_nowrap((char_u *)" ", 1,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	// The cursor may wrap to the next line.
	எனில் (gui.col >= Columns)
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
    நிலையான முழு
gui_screenchar(
    முழு		off,	    // Offset from start of screen
    முழு		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    முழு		back)	    // backup this many chars when using bold trick
{
    char_u	buf[MB_MAXBYTES + 1];

    // Don't draw right half of a double-width UTF-8 char. "cannot happen"
    எனில் (enc_utf8 && Screen_வரிகள்[off] == 0)
	திரும்பு OK;

    எனில் (enc_utf8 && Screen_வரிகள்_UC[off] != 0)
	// Draw UTF-8 multi-byte character.
	திரும்பு gui_outstr_nowrap(buf, utfc_char2bytes(off, buf),
							 flags, fg, bg, back);

    எனில் (enc_dbcs == DBCS_JPNU && Screen_வரிகள்[off] == 0x8e)
    {
	buf[0] = Screen_வரிகள்[off];
	buf[1] = ScreenLines2[off];
	திரும்பு gui_outstr_nowrap(buf, 2, flags, fg, bg, back);
    }

    // Draw non-multi-byte character or DBCS character.
    திரும்பு gui_outstr_nowrap(Screen_வரிகள் + off,
	    enc_dbcs ? (*mb_ptr2len)(Screen_வரிகள் + off) : 1,
							 flags, fg, bg, back);
}
#endif

#ifdef FEAT_GUI_GTK
/*
 * Output the string at the given screen position.  This is used in place
 * of gui_screenchar() where possible because Pango needs as much context
 * as possible to work nicely.  It's a lot faster as well.
 */
    நிலையான முழு
gui_screenstr(
    முழு		off,	    // Offset from start of screen
    முழு		len,	    // string length in screen cells
    முழு		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    முழு		back)	    // backup this many chars when using bold trick
{
    char_u  *buf;
    முழு	    outlen = 0;
    முழு	    i;
    முழு	    retval;

    எனில் (len <= 0) // "cannot happen"?
	திரும்பு OK;

    எனில் (enc_utf8)
    {
	buf = alloc(len * MB_MAXBYTES + 1);
	எனில் (buf == வெற்று)
	    திரும்பு OK; // not much we could do here...

	க்கு (i = off; i < off + len; ++i)
	{
	    எனில் (Screen_வரிகள்[i] == 0)
		தொடர்; // skip second half of double-width char

	    எனில் (Screen_வரிகள்_UC[i] == 0)
		buf[outlen++] = Screen_வரிகள்[i];
	    இல்லையெனில்
		outlen += utfc_char2bytes(i, buf + outlen);
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, back);
	vim_free(buf);

	திரும்பு retval;
    }
    இல்லையெனில் எனில் (enc_dbcs == DBCS_JPNU)
    {
	buf = alloc(len * 2 + 1);
	எனில் (buf == வெற்று)
	    திரும்பு OK; // not much we could do here...

	க்கு (i = off; i < off + len; ++i)
	{
	    buf[outlen++] = Screen_வரிகள்[i];

	    // handle double-byte single-width char
	    எனில் (Screen_வரிகள்[i] == 0x8e)
		buf[outlen++] = ScreenLines2[i];
	    இல்லையெனில் எனில் (MB_BYTE2LEN(Screen_வரிகள்[i]) == 2)
		buf[outlen++] = Screen_வரிகள்[++i];
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, back);
	vim_free(buf);

	திரும்பு retval;
    }
    இல்லையெனில்
    {
	திரும்பு gui_outstr_nowrap(&Screen_வரிகள்[off], len,
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
    நிலையான முழு
gui_outstr_nowrap(
    char_u	*s,
    முழு		len,
    முழு		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    முழு		back)	    // backup this many chars when using bold trick
{
    long_u	highlight_mask;
    long_u	hl_mask_todo;
    guicolor_T	fg_color;
    guicolor_T	bg_color;
    guicolor_T	sp_color;
#எனில் !defined(FEAT_GUI_GTK)
    GuiFont	font = NOFONT;
    GuiFont	wide_font = NOFONT;
# ifdef FEAT_XFONTSET
    GuiFontset	fontset = NOFONTSET;
# endif
#endif
    attrentry_T	*aep = வெற்று;
    முழு		draw_flags;
    முழு		col = gui.col;
#ifdef FEAT_SIGN_ICONS
    முழு		draw_sign = FALSE;
    முழு		signcol = 0;
    char_u	extra[18];
# ifdef FEAT_NETBEANS_INTG
    முழு		multi_sign = FALSE;
# endif
#endif

    எனில் (len < 0)
	len = (முழு)STRLEN(s);
    எனில் (len == 0)
	திரும்பு OK;

#ifdef FEAT_SIGN_ICONS
    எனில் (*s == SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
	  || *s == MULTISIGN_BYTE
# endif
       )
    {
	size_t	slen = 2;		    // 2 spaces by default

# ifdef FEAT_NETBEANS_INTG
	எனில் (*s == MULTISIGN_BYTE)
	    multi_sign = TRUE;
# endif
	// draw spaces instead
	எனில் (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u' &&
		(curwin->w_p_nu || curwin->w_p_rnu))
	{
	    முழு	n = எண்_width(curwin);

	    slen = MIN(n, (முழு)அளவு(extra) - 1);
	}
	vim_memset(extra, ' ', slen);
	extra[slen] = NUL;
	s = extra;
	எனில் (len == 1 && col > 0)
	    --col;
	len = (முழு)slen;
	எனில் (len > 2)
	    // right align sign icon in the number column
	    signcol = col + len - 3;
	இல்லையெனில்
	    signcol = col;
	draw_sign = TRUE;
	highlight_mask = 0;
    }
    இல்லையெனில்
#endif
    எனில் (gui.highlight_mask > HL_ALL)
    {
	aep = syn_gui_attr2entry(gui.highlight_mask);
	எனில் (aep == வெற்று)	    // highlighting not set
	    highlight_mask = 0;
	இல்லையெனில்
	    highlight_mask = aep->ae_attr;
    }
    இல்லையெனில்
	highlight_mask = gui.highlight_mask;
    hl_mask_todo = highlight_mask;

#எனில் !defined(FEAT_GUI_GTK)
    // Set the font
    எனில் (aep != வெற்று && aep->ae_u.gui.font != NOFONT)
	font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
    இல்லையெனில் எனில் (aep != வெற்று && aep->ae_u.gui.fontset != NOFONTSET)
	fontset = aep->ae_u.gui.fontset;
# endif
    இல்லையெனில்
    {
# ifdef FEAT_XFONTSET
	எனில் (gui.fontset != NOFONTSET)
	    fontset = gui.fontset;
	இல்லையெனில்
# endif
	    எனில் (hl_mask_todo & (HL_BOLD | HL_STANDOUT))
	{
	    எனில் ((hl_mask_todo & HL_ITALIC) && gui.boldital_font != NOFONT)
	    {
		font = gui.boldital_font;
		hl_mask_todo &= ~(HL_BOLD | HL_STANDOUT | HL_ITALIC);
	    }
	    இல்லையெனில் எனில் (gui.bold_font != NOFONT)
	    {
		font = gui.bold_font;
		hl_mask_todo &= ~(HL_BOLD | HL_STANDOUT);
	    }
	    இல்லையெனில்
		font = gui.norm_font;
	}
	இல்லையெனில் எனில் ((hl_mask_todo & HL_ITALIC) && gui.ital_font != NOFONT)
	{
	    font = gui.ital_font;
	    hl_mask_todo &= ~HL_ITALIC;
	}
	இல்லையெனில்
	    font = gui.norm_font;

	/*
	 * Choose correct wide_font by font.  wide_font should be set with font
	 * at same time in above block.  But it will make many "ifdef" nasty
	 * blocks.  So we do it here.
	 */
	எனில் (font == gui.boldital_font && gui.wide_boldital_font)
	    wide_font = gui.wide_boldital_font;
	இல்லையெனில் எனில் (font == gui.bold_font && gui.wide_bold_font)
	    wide_font = gui.wide_bold_font;
	இல்லையெனில் எனில் (font == gui.ital_font && gui.wide_ital_font)
	    wide_font = gui.wide_ital_font;
	இல்லையெனில் எனில் (font == gui.norm_font && gui.wide_font)
	    wide_font = gui.wide_font;
    }
# ifdef FEAT_XFONTSET
    எனில் (fontset != NOFONTSET)
	gui_mch_set_fontset(fontset);
    இல்லையெனில்
# endif
	gui_mch_set_font(font);
#endif

    draw_flags = 0;

    // Set the color
    bg_color = gui.back_pixel;
    எனில் ((flags & GUI_MON_IS_CURSOR) && gui.in_focus)
    {
	draw_flags |= DRAW_CURSOR;
	fg_color = fg;
	bg_color = bg;
	sp_color = fg;
    }
    இல்லையெனில் எனில் (aep != வெற்று)
    {
	fg_color = aep->ae_u.gui.fg_color;
	எனில் (fg_color == INVALCOLOR)
	    fg_color = gui.norm_pixel;
	bg_color = aep->ae_u.gui.bg_color;
	எனில் (bg_color == INVALCOLOR)
	    bg_color = gui.back_pixel;
	sp_color = aep->ae_u.gui.sp_color;
	எனில் (sp_color == INVALCOLOR)
	    sp_color = fg_color;
    }
    இல்லையெனில்
    {
	fg_color = gui.norm_pixel;
	sp_color = fg_color;
    }

    எனில் (highlight_mask & (HL_INVERSE | HL_STANDOUT))
    {
	gui_mch_set_fg_color(bg_color);
	gui_mch_set_bg_color(fg_color);
    }
    இல்லையெனில்
    {
	gui_mch_set_fg_color(fg_color);
	gui_mch_set_bg_color(bg_color);
    }
    gui_mch_set_sp_color(sp_color);

    // Clear the selection if we are about to write over it
    எனில் (!(flags & GUI_MON_NOCLEAR))
	clip_may_clear_selection(gui.row, gui.row);


    // If there's no bold font, then fake it
    எனில் (hl_mask_todo & (HL_BOLD | HL_STANDOUT))
	draw_flags |= DRAW_BOLD;

    /*
     * When drawing bold or italic characters the spill-over from the left
     * neighbor may be destroyed.  Let the caller backup to start redrawing
     * just after a blank.
     */
    எனில் (back != 0 && ((draw_flags & DRAW_BOLD) || (highlight_mask & HL_ITALIC)))
	திரும்பு FAIL;

#எனில் defined(FEAT_GUI_GTK)
    // If there's no italic font, then fake it.
    // For GTK2, we don't need a different font for italic style.
    எனில் (hl_mask_todo & HL_ITALIC)
	draw_flags |= DRAW_ITALIC;

    // Do we underline the text?
    எனில் (hl_mask_todo & HL_UNDERLINE)
	draw_flags |= DRAW_UNDERL;

#இல்லையெனில்
    // Do we underline the text?
    எனில் ((hl_mask_todo & HL_UNDERLINE) || (hl_mask_todo & HL_ITALIC))
	draw_flags |= DRAW_UNDERL;
#endif
    // Do we undercurl the text?
    எனில் (hl_mask_todo & HL_UNDERCURL)
	draw_flags |= DRAW_UNDERC;

    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

    // Do we strikethrough the text?
    எனில் (hl_mask_todo & HL_STRIKETHROUGH)
	draw_flags |= DRAW_STRIKE;

    // Do we draw transparently?
    எனில் (flags & GUI_MON_TRS_CURSOR)
	draw_flags |= DRAW_TRANSP;

    /*
     * Draw the text.
     */
#ifdef FEAT_GUI_GTK
    // The value returned is the length in display cells
    len = gui_gtk_draw_string(gui.row, col, s, len, draw_flags);
#இல்லையெனில்
    எனில் (enc_utf8)
    {
	முழு	start;		// index of bytes to be drawn
	முழு	cells;		// cellwidth of bytes to be drawn
	முழு	thislen;	// length of bytes to be drawn
	முழு	cn;		// cellwidth of current char
	முழு	i;		// index of current char
	முழு	c;		// current char value
	முழு	cl;		// byte length of current char
	முழு	comping;	// current char is composing
	முழு	scol = col;	// screen column
	முழு	curr_wide = FALSE;  // use 'guifontwide'
	முழு	prev_wide = FALSE;
	முழு	wide_changed;
# ifdef MSWIN
	முழு	sep_comp = FALSE;   // Don't separate composing chars.
# இல்லையெனில்
	முழு	sep_comp = TRUE;    // Separate composing chars.
# endif

	// Break the string at a composing character, it has to be drawn on
	// top of the previous character.
	start = 0;
	cells = 0;
	க்கு (i = 0; i < len; i += cl)
	{
	    c = utf_ptr2char(s + i);
	    cn = utf_char2cells(c);
	    comping = utf_iscomposing(c);
	    எனில் (!comping)	// count cells from non-composing chars
		cells += cn;
	    எனில் (!comping || sep_comp)
	    {
		எனில் (cn > 1
# ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
# endif
			&& wide_font != NOFONT)
		    curr_wide = TRUE;
		இல்லையெனில்
		    curr_wide = FALSE;
	    }
	    cl = utf_ptr2len(s + i);
	    எனில் (cl == 0)	// hit end of string
		len = i + cl;	// len must be wrong "cannot happen"

	    wide_changed = curr_wide != prev_wide;

	    // Print the string so far if it's the last character or there is
	    // a composing character.
	    எனில் (i + cl >= len || (comping && sep_comp && i > start)
		    || wide_changed
# எனில் defined(FEAT_GUI_X11)
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
		எனில் ((comping && sep_comp) || wide_changed)
		    thislen = i - start;
		இல்லையெனில்
		    thislen = i - start + cl;
		எனில் (thislen > 0)
		{
		    எனில் (prev_wide)
			gui_mch_set_font(wide_font);
		    gui_mch_draw_string(gui.row, scol, s + start, thislen,
								  draw_flags);
		    எனில் (prev_wide)
			gui_mch_set_font(font);
		    start += thislen;
		}
		scol += cells;
		cells = 0;
		// Adjust to not draw a character which width is changed
		// against with last one.
		எனில் (wide_changed && !(comping && sep_comp))
		{
		    scol -= cn;
		    cl = 0;
		}

# எனில் defined(FEAT_GUI_X11)
		// No fontset: draw a space to fill the gap after a wide char
		//
		எனில் (cn > 1 && (draw_flags & DRAW_TRANSP) == 0
#  ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
#  endif
			&& !wide_changed)
		    gui_mch_draw_string(gui.row, scol - 1, (char_u *)" ",
							       1, draw_flags);
# endif
	    }
	    // Draw a composing char on top of the previous char.
	    எனில் (comping && sep_comp)
	    {
# எனில் defined(__APPLE_CC__) && TARGET_API_MAC_CARBON
		// Carbon ATSUI autodraws composing char over previous char
		gui_mch_draw_string(gui.row, scol, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# இல்லையெனில்
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
    இல்லையெனில்
    {
	gui_mch_draw_string(gui.row, col, s, len, draw_flags);
	எனில் (enc_dbcs == DBCS_JPNU)
	{
	    // Get the length in display cells, this can be different from the
	    // number of bytes for "euc-jp".
	    len = mb_string2cells(s, len);
	}
    }
#endif // !FEAT_GUI_GTK

    எனில் (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
	gui.col = col + len;

    // May need to invert it when it's part of the selection.
    எனில் (flags & GUI_MON_NOCLEAR)
	clip_may_redraw_selection(gui.row, col, len);

    எனில் (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
    {
	// Invalidate the old physical cursor position if we wrote over it
	எனில் (gui.cursor_row == gui.row
		&& gui.cursor_col >= col
		&& gui.cursor_col < col + len)
	    gui.cursor_is_valid = false;
    }

#ifdef FEAT_SIGN_ICONS
    எனில் (draw_sign)
	// Draw the sign on top of the spaces.
	gui_mch_drawsign(gui.row, signcol, gui.highlight_mask);
# எனில் defined(FEAT_NETBEANS_INTG) && (defined(FEAT_GUI_X11) \
	|| defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN))
    எனில் (multi_sign)
	netbeans_draw_multisign_indicator(gui.row);
# endif
#endif

    திரும்பு OK;
}

/*
 * Undraw the cursor.  This actually redraws the character at the cursor
 * position, plus some more characters when needed.
 */
    வெறுமை
gui_undraw_cursor(வெறுமை)
{
    எனில் (!gui.cursor_is_valid)
	திரும்பு;

    // Always redraw the character just before if there is one, because
    // with some fonts and characters there can be a one pixel overlap.
    முழு startcol = gui.cursor_col > 0 ? gui.cursor_col - 1 : gui.cursor_col;
    முழு endcol = gui.cursor_col;

#எனில் defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    gui_adjust_undraw_cursor_for_ligatures(&startcol, &endcol);
#endif
    gui_redraw_block(gui.cursor_row, startcol,
	    gui.cursor_row, endcol, GUI_MON_NOCLEAR);

    // Cursor_is_valid is reset when the cursor is undrawn, also reset it
    // here in case it wasn't needed to undraw it.
    gui.cursor_is_valid = false;
}

    வெறுமை
gui_redraw(
    முழு		x,
    முழு		y,
    முழு		w,
    முழு		h)
{
    முழு		row1, col1, row2, col2;

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
    எனில் (gui.row == gui.cursor_row)
	gui_update_cursor(TRUE, TRUE);
}

/*
 * Draw a rectangular block of characters, from row1 to row2 (inclusive) and
 * from col1 to col2 (inclusive).
 */
    வெறுமை
gui_redraw_block(
    முழு		row1,
    முழு		col1,
    முழு		row2,
    முழு		col2,
    முழு		flags)	// flags for gui_outstr_nowrap()
{
    முழு		old_row, old_col;
    long_u	old_hl_mask;
    முழு		off;
    sattr_T	first_attr;
    முழு		idx, len;
    முழு		back, nback;
    முழு		orig_col1, orig_col2;

    // Don't try to update when ScreenLines is not valid
    எனில் (!screen_cleared || Screen_வரிகள் == வெற்று)
	திரும்பு;

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

    க்கு (gui.row = row1; gui.row <= row2; gui.row++)
    {
	// When only half of a double-wide character is in the block, include
	// the other half.
	col1 = orig_col1;
	col2 = orig_col2;
	off = வரி_Offset[gui.row];
	எனில் (enc_dbcs != 0)
	{
	    எனில் (col1 > 0)
		col1 -= dbcs_screen_head_off(Screen_வரிகள் + off,
						    Screen_வரிகள் + off + col1);
	    col2 += dbcs_screen_tail_off(Screen_வரிகள் + off,
						    Screen_வரிகள் + off + col2);
	}
	இல்லையெனில் எனில் (enc_utf8)
	{
	    எனில் (Screen_வரிகள்[off + col1] == 0)
	    {
		எனில் (col1 > 0)
		    --col1;
		இல்லையெனில்
		    // FIXME: how can the first character ever be zero?
		    siemsg("NUL in ScreenLines in row %ld", (long)gui.row);
	    }
#ifdef FEAT_GUI_GTK
	    எனில் (col2 + 1 < Columns && Screen_வரிகள்[off + col2 + 1] == 0)
		++col2;
#endif
	}
	gui.col = col1;
	off = வரி_Offset[gui.row] + gui.col;
	len = col2 - col1 + 1;

	// Find how many chars back this highlighting starts, or where a space
	// is.  Needed for when the bold trick is used
	க்கு (back = 0; back < col1; ++back)
	    எனில் (ScreenAttrs[off - 1 - back] != ScreenAttrs[off]
		    || Screen_வரிகள்[off - 1 - back] == ' ')
		நிறுத்து;

	// Break it up in strings of characters with the same attributes.
	// Print UTF-8 characters individually.
	போது (len > 0)
	{
	    first_attr = ScreenAttrs[off];
	    gui.highlight_mask = first_attr;
#எனில் !defined(FEAT_GUI_GTK)
	    எனில் (enc_utf8 && Screen_வரிகள்_UC[off] != 0)
	    {
		// output multi-byte character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
		எனில் (gui.col < Columns && Screen_வரிகள்[off + 1] == 0)
		    idx = 2;
		இல்லையெனில்
		    idx = 1;
	    }
	    இல்லையெனில் எனில் (enc_dbcs == DBCS_JPNU && Screen_வரிகள்[off] == 0x8e)
	    {
		// output double-byte, single-width character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
		idx = 1;
	    }
	    இல்லையெனில்
#endif
	    {
#ifdef FEAT_GUI_GTK
		க்கு (idx = 0; idx < len; ++idx)
		{
		    எனில் (enc_utf8 && Screen_வரிகள்[off + idx] == 0)
			தொடர்; // skip second half of double-width char
		    எனில் (ScreenAttrs[off + idx] != first_attr)
			நிறுத்து;
		}
		// gui_screenstr() takes care of multibyte chars
		nback = gui_screenstr(off, idx, flags,
				      (guicolor_T)0, (guicolor_T)0, back);
#இல்லையெனில்
		க்கு (idx = 0; idx < len && ScreenAttrs[off + idx] == first_attr;
									idx++)
		{
		    // Stop at a multi-byte Unicode character.
		    எனில் (enc_utf8 && Screen_வரிகள்_UC[off + idx] != 0)
			நிறுத்து;
		    எனில் (enc_dbcs == DBCS_JPNU)
		    {
			// Stop at a double-byte single-width char.
			எனில் (Screen_வரிகள்[off + idx] == 0x8e)
			    நிறுத்து;
			எனில் (len > 1 && (*mb_ptr2len)(Screen_வரிகள்
							    + off + idx) == 2)
			    ++idx;  // skip second byte of double-byte char
		    }
		}
		nback = gui_outstr_nowrap(Screen_வரிகள் + off, idx, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
#endif
	    }
	    எனில் (nback == FAIL)
	    {
		// Must back up to start drawing where a bold or italic word
		// starts.
		off -= back;
		len += back;
		gui.col -= back;
	    }
	    இல்லையெனில்
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
    gui.highlight_mask = (முழு)old_hl_mask;
}

    நிலையான வெறுமை
gui_delete_வரிகள்(முழு row, முழு மொத்தம்)
{
    எனில் (மொத்தம் <= 0)
	திரும்பு;

    எனில் (row + மொத்தம் > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_block(row, gui.scroll_region_இடது,
			      gui.scroll_region_bot, gui.scroll_region_வலது);
    இல்லையெனில்
    {
	gui_mch_delete_வரிகள்(row, மொத்தம்);

	// If the cursor was in the deleted lines it's now gone.  If the
	// cursor was in the scrolled lines adjust its position.
	எனில் (gui.cursor_row >= row
		&& gui.cursor_col >= gui.scroll_region_இடது
		&& gui.cursor_col <= gui.scroll_region_வலது)
	{
	    எனில் (gui.cursor_row < row + மொத்தம்)
		gui.cursor_is_valid = false;
	    இல்லையெனில் எனில் (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_row -= மொத்தம்;
	}
    }
}

    நிலையான வெறுமை
gui_செருகு_வரிகள்(முழு row, முழு மொத்தம்)
{
    எனில் (மொத்தம் <= 0)
	திரும்பு;

    எனில் (row + மொத்தம் > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_block(row, gui.scroll_region_இடது,
			      gui.scroll_region_bot, gui.scroll_region_வலது);
    இல்லையெனில்
    {
	gui_mch_செருகு_வரிகள்(row, மொத்தம்);

	எனில் (gui.cursor_row >= gui.row
		&& gui.cursor_col >= gui.scroll_region_இடது
		&& gui.cursor_col <= gui.scroll_region_வலது)
	{
	    எனில் (gui.cursor_row <= gui.scroll_region_bot - மொத்தம்)
		gui.cursor_row += மொத்தம்;
	    இல்லையெனில் எனில் (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_is_valid = false;
	}
    }
}

#ifdef FEAT_TIMERS
/*
 * Passed to ui_wait_for_chars_or_timer(), ignoring extra arguments.
 */
    நிலையான முழு
gui_wait_for_chars_3(
    long wtime,
    முழு *interrupted UNUSED,
    முழு ignore_உள்ளீடு UNUSED)
{
    திரும்பு gui_mch_wait_for_chars(wtime);
}
#endif

/*
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    நிலையான முழு
gui_wait_for_chars_or_timer(
	long wtime,
	முழு *interrupted UNUSED,
	முழு ignore_உள்ளீடு UNUSED)
{
#ifdef FEAT_TIMERS
    திரும்பு ui_wait_for_chars_or_timer(wtime, gui_wait_for_chars_3,
						    interrupted, ignore_உள்ளீடு);
#இல்லையெனில்
    திரும்பு gui_mch_wait_for_chars(wtime);
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
    நிலையான முழு
gui_wait_for_chars_buf(
    char_u	*buf,
    முழு		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    முழு		tb_change_cnt)
{
    முழு	    retval;
    முழு	    keep_blinking; // Guard against restarting blink cycle on CursorHold

#ifdef FEAT_MENU
    // If we're going to wait a bit, update the menus and mouse shape for the
    // current State.
    எனில் (wtime != 0)
	gui_update_menus(0);
#endif

    gui_mch_update();
    எனில் (உள்ளீடு_available())	// Got char, return immediately
    {
	எனில் (gui_mch_is_blinking())
	    gui_mch_stop_blink(TRUE);
	எனில் (buf != வெற்று && !typebuf_changed(tb_change_cnt))
	    திரும்பு read_from_உள்ளீடு_buf(buf, (long)maxlen);
	திரும்பு 0;
    }
    எனில் (wtime == 0)		// Don't wait for char
	திரும்பு FAIL;

    // Before waiting, flush any output to the screen.
    gui_mch_flush();

    // Blink while waiting for a character.
    எனில் (!gui_mch_is_blinking())
	gui_mch_start_blink();

    // Common function to loop until "wtime" is met, while handling timers and
    // other callbacks.
    retval = inchar_loop(buf, maxlen, wtime, tb_change_cnt,
			 gui_wait_for_chars_or_timer, வெற்று);

    // Keep blinking when CursorHold wakes the input loop. (See PR #21115)
    keep_blinking = retval == 3 && buf != வெற்று
	&& buf[0] == K_SPECIAL && buf[1] == KS_EXTRA
	&& buf[2] == (முழு)KE_CURSORHOLD;

    எனில் (!keep_blinking)
	gui_mch_stop_blink(TRUE);

    திரும்பு retval;
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
    முழு
gui_wait_for_chars(long wtime, முழு tb_change_cnt)
{
    திரும்பு gui_wait_for_chars_buf(வெற்று, 0, wtime, tb_change_cnt);
}

/*
 * Equivalent of mch_inchar() for the GUI.
 */
    முழு
gui_inchar(
    char_u  *buf,
    முழு	    maxlen,
    long    wtime,		// milliseconds
    முழு	    tb_change_cnt)
{
    திரும்பு gui_wait_for_chars_buf(buf, maxlen, wtime, tb_change_cnt);
}

/*
 * Fill p[4] with mouse coordinates encoded for check_termcode().
 */
    நிலையான வெறுமை
fill_mouse_coord(char_u *p, முழு col, முழு row)
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
    வெறுமை
gui_send_mouse_event(
    முழு	    button,
    முழு	    x,
    முழு	    y,
    முழு	    repeated_click,
    int_u   modifiers)
{
    நிலையான முழு	    prev_row = 0, prev_col = 0;
    நிலையான முழு	    prev_button = -1;
    நிலையான முழு	    இலக்கம்_clicks = 1;
    char_u	    string[10];
    எண்ணிக்கை சாவி_extra  button_char;
    முழு		    row, col;
#ifdef FEAT_CLIPBOARD
    முழு		    checkfor;
    முழு		    did_clip = FALSE;
#endif

    /*
     * Scrolling may happen at any time, also while a selection is present.
     */
    தேர்வு (button)
    {
	நிலை MOUSE_MOVE:
	    button_char = KE_MOUSEMOVE_XY;
	    செல் button_set;
	நிலை MOUSE_X1:
	    button_char = KE_X1MOUSE;
	    செல் button_set;
	நிலை MOUSE_X2:
	    button_char = KE_X2MOUSE;
	    செல் button_set;
	நிலை MOUSE_4:
	    button_char = KE_MOUSEDOWN;
	    செல் button_set;
	நிலை MOUSE_5:
	    button_char = KE_MOUSEUP;
	    செல் button_set;
	நிலை MOUSE_6:
	    button_char = KE_MOUSELEFT;
	    செல் button_set;
	நிலை MOUSE_7:
	    button_char = KE_MOUSERIGHT;
button_set:
	    {
		// Don't put events in the input queue now.
		எனில் (hold_gui_events)
		    திரும்பு;

		row = gui_xy2colrow(x, y, &col);
		// Don't report a mouse move unless moved to a
		// different character position.
		எனில் (button == MOUSE_MOVE)
		{
		    எனில் (row == prev_row && col == prev_col)
			திரும்பு;
		    இல்லையெனில்
		    {
			prev_row = row >= 0 ? row : 0;
			prev_col = col;
		    }
		}

		string[3] = CSI;
		string[4] = KS_EXTRA;
		string[5] = (முழு)button_char;

		// Pass the pointer coordinates of the scroll event so that we
		// know which window to scroll.
		string[6] = (char_u)(col / 128 + ' ' + 1);
		string[7] = (char_u)(col % 128 + ' ' + 1);
		string[8] = (char_u)(row / 128 + ' ' + 1);
		string[9] = (char_u)(row % 128 + ' ' + 1);

		எனில் (modifiers == 0)
		    add_to_உள்ளீடு_buf(string + 3, 7);
		இல்லையெனில்
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = 0;
		    எனில் (modifiers & MOUSE_SHIFT)
			string[2] |= MOD_MASK_SHIFT;
		    எனில் (modifiers & MOUSE_CTRL)
			string[2] |= MOD_MASK_CTRL;
		    எனில் (modifiers & MOUSE_ALT)
			string[2] |= MOD_MASK_ALT;
		    add_to_உள்ளீடு_buf(string, 10);
		}
		திரும்பு;
	    }
    }

#ifdef FEAT_CLIPBOARD
    // If a clipboard selection is in progress, handle it
    எனில் (clip_star.state == SELECT_IN_PROGRESS)
    {
	clip_process_selection(button, X_2_COL(x), Y_2_ROW(y), repeated_click);

	// A release event may still need to be sent if the position is equal.
	row = gui_xy2colrow(x, y, &col);
	எனில் (button != MOUSE_RELEASE || row != prev_row || col != prev_col)
	    திரும்பு;
    }

    // Determine which mouse settings to look for based on the current mode
    தேர்வு (get_real_state())
    {
	நிலை MODE_NORMAL_BUSY:
	நிலை MODE_OP_PENDING:
# ifdef FEAT_TERMINAL
	நிலை MODE_TERMINAL:
# endif
	நிலை MODE_NORMAL:	checkfor = MOUSE_NORMAL; நிறுத்து;
	நிலை MODE_VISUAL:	checkfor = MOUSE_VISUAL; நிறுத்து;
	நிலை MODE_SELECT:	checkfor = MOUSE_VISUAL; நிறுத்து;
	நிலை MODE_REPLACE:
	நிலை MODE_REPLACE | MODE_LANGMAP:
	நிலை MODE_VREPLACE:
	நிலை MODE_VREPLACE | MODE_LANGMAP:
	நிலை MODE_செருகு:
	நிலை MODE_செருகு | MODE_LANGMAP:
				checkfor = MOUSE_செருகு; நிறுத்து;
	நிலை MODE_ASKMORE:
	நிலை MODE_HITRETURN:	// At the more- and hit-enter prompt pass the
				// mouse event for a click on or below the
				// message line.
				எனில் (Y_2_ROW(y) >= செய்தி_row)
				    checkfor = MOUSE_NORMAL;
				இல்லையெனில்
				    checkfor = MOUSE_RETURN;
				நிறுத்து;

	    /*
	     * On the command line, use the clipboard selection on all lines
	     * but the command line.  But not when pasting.
	     */
	நிலை MODE_CMDLINE:
	நிலை MODE_CMDLINE | MODE_LANGMAP:
	    எனில் (Y_2_ROW(y) < cmdline_row && button != MOUSE_நடு)
		checkfor = MOUSE_NONE;
	    இல்லையெனில்
		checkfor = MOUSE_COMMAND;
	    நிறுத்து;

	இயல்பு:
	    checkfor = MOUSE_NONE;
	    நிறுத்து;
    };

    /*
     * Allow clipboard selection of text on the command line in "normal"
     * modes.  Don't do this when dragging the status line, or extending a
     * Visual selection.
     */
    எனில் ((State == MODE_NORMAL || State == MODE_NORMAL_BUSY
						      || (State & MODE_செருகு))
	    && X_2_COL(x) >= firstwin->w_wincol
	    && X_2_COL(x) < firstwin->w_wincol + topframe->fr_width
	    && Y_2_ROW(y) >= topframe->fr_height + firstwin->w_winrow
	    && button != MOUSE_DRAG
# ifdef FEAT_MOUSESHAPE
	    && !drag_status_வரி
	    && !drag_sep_வரி
# endif
	    )
	checkfor = MOUSE_NONE;

    /*
     * Use modeless selection when holding CTRL and SHIFT pressed.
     */
    எனில் ((modifiers & MOUSE_CTRL) && (modifiers & MOUSE_SHIFT))
	checkfor = MOUSE_NONEF;

    /*
     * In Ex mode, always use modeless selection.
     */
    எனில் (exmode_active)
	checkfor = MOUSE_NONE;

    /*
     * If the mouse settings say to not use the mouse, use the modeless
     * selection.  But if Visual is active, assume that only the Visual area
     * will be selected.
     * Exception: On the command line, both the selection is used and a mouse
     * key is sent.
     */
    எனில் (!mouse_has(checkfor) || checkfor == MOUSE_COMMAND)
    {
	// Don't do modeless selection in Visual mode.
	எனில் (checkfor != MOUSE_NONEF && VIsual_active && (State & MODE_NORMAL))
	    திரும்பு;

	/*
	 * When 'mousemodel' is "popup", shift-left is translated to right.
	 * But not when also using Ctrl.
	 */
	எனில் (mouse_model_popup() && button == MOUSE_இடது
		&& (modifiers & MOUSE_SHIFT) && !(modifiers & MOUSE_CTRL))
	{
	    button = MOUSE_வலது;
	    modifiers &= ~ MOUSE_SHIFT;
	}

	// If the selection is done, allow the right button to extend it.
	// If the selection is cleared, allow the right button to start it
	// from the cursor position.
	எனில் (button == MOUSE_வலது)
	{
	    எனில் (clip_star.state == SELECT_CLEARED)
	    {
		எனில் (State & MODE_CMDLINE)
		{
		    col = செய்தி_col;
		    row = செய்தி_row;
		}
		இல்லையெனில்
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
	இல்லையெனில் எனில் (button == MOUSE_இடது)
	{
	    clip_start_selection(X_2_COL(x), Y_2_ROW(y), repeated_click);
	    did_clip = TRUE;
	}

	// Always allow pasting
	எனில் (button != MOUSE_நடு)
	{
	    எனில் (!mouse_has(checkfor) || button == MOUSE_RELEASE)
		திரும்பு;
	    எனில் (checkfor != MOUSE_COMMAND)
		button = MOUSE_இடது;
	}
	repeated_click = FALSE;
    }

    எனில் (clip_star.state != SELECT_CLEARED && !did_clip)
	clip_clear_selection(&clip_star);
#endif

    // Don't put events in the input queue now.
    எனில் (hold_gui_events)
	திரும்பு;

    row = gui_xy2colrow(x, y, &col);

    /*
     * If we are dragging and the mouse hasn't moved far enough to be on a
     * different character, then don't send an event to vim.
     */
    எனில் (button == MOUSE_DRAG)
    {
	எனில் (row == prev_row && col == prev_col)
	    திரும்பு;
	// Dragging above the window, set "row" to -1 to cause a scroll.
	எனில் (y < 0)
	    row = -1;
    }

    /*
     * If topline has changed (window scrolled) since the last click, reset
     * repeated_click, because we don't want starting Visual mode when
     * clicking on a different character in the text.
     */
    எனில் (curwin->w_topline != gui_prev_topline
#ifdef FEAT_DIFF
	    || curwin->w_topfill != gui_prev_topfill
#endif
	    )
	repeated_click = FALSE;

    string[0] = CSI;	// this sequence is recognized by check_termcode()
    string[1] = KS_MOUSE;
    string[2] = KE_FILLER;
    எனில் (button != MOUSE_DRAG && button != MOUSE_RELEASE)
    {
	எனில் (repeated_click)
	{
	    /*
	     * Handle multiple clicks.	They only count if the mouse is still
	     * pointing at the same character.
	     */
	    எனில் (button != prev_button || row != prev_row || col != prev_col)
		இலக்கம்_clicks = 1;
	    இல்லையெனில் எனில் (++இலக்கம்_clicks > 4)
		இலக்கம்_clicks = 1;
	}
	இல்லையெனில்
	    இலக்கம்_clicks = 1;
	prev_button = button;
	gui_prev_topline = curwin->w_topline;
#ifdef FEAT_DIFF
	gui_prev_topfill = curwin->w_topfill;
#endif

	string[3] = (char_u)(button | 0x20);
	SET_இலக்கம்_MOUSE_CLICKS(string[3], இலக்கம்_clicks);
    }
    இல்லையெனில்
	string[3] = (char_u)button;

    string[3] |= modifiers;
    fill_mouse_coord(string + 4, col, row);
    add_to_உள்ளீடு_buf(string, 8);

    எனில் (row < 0)
	prev_row = 0;
    இல்லையெனில்
	prev_row = row;
    prev_col = col;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#எனில் defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#endif
}

/*
 * Convert x and y coordinate to column and row in text window.
 * Corrects for multi-byte character.
 * returns column in "*colp" and row as return value;
 */
    நிலையான முழு
gui_xy2colrow(முழு x, முழு y, முழு *colp)
{
    முழு		col = check_col(X_2_COL(x));
    முழு		row = check_row(Y_2_ROW(y));

    *colp = mb_fix_col(col, row);
    திரும்பு row;
}

#எனில் defined(FEAT_MENU)
/*
 * Callback function for when a menu entry has been selected.
 */
    வெறுமை
gui_menu_cb(vimmenu_T *menu)
{
    char_u  bytes[அளவு(long_u)];

    // Don't put events in the input queue now.
    எனில் (hold_gui_events)
	திரும்பு;

    bytes[0] = CSI;
    bytes[1] = KS_MENU;
    bytes[2] = KE_FILLER;
    add_to_உள்ளீடு_buf(bytes, 3);
    add_long_to_buf((long_u)menu, bytes);
    add_to_உள்ளீடு_buf_csi(bytes, அளவு(long_u));
}
#endif

நிலையான முழு	prev_which_scrollbars[3];

/*
 * Set which components are present.
 * If "oldval" is not NULL, "oldval" is the previous value, the new value is
 * in p_go.
 */
    வெறுமை
gui_init_which_components(char_u *oldval UNUSED)
{
#ifdef FEAT_GUI_DARKTHEME
    நிலையான முழு	prev_dark_theme = FALSE;
    முழு		using_dark_theme = FALSE;
#endif
#ifdef FEAT_MENU
    நிலையான முழு	prev_menu_is_active = -1;
#endif
#ifdef FEAT_TOOLBAR
    நிலையான முழு	prev_toolbar = -1;
    முழு		using_toolbar = FALSE;
#endif
#ifdef FEAT_GUI_TABLINE
    முழு		using_tabline;
#endif
#ifdef FEAT_GUI_MSWIN
    நிலையான முழு	prev_titlebar = FALSE;
    முழு		using_titlebar = FALSE;
#endif

#எனில் defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    நிலையான முழு	prev_fullscreen = FALSE;
    முழு		using_fullscreen = FALSE;
#endif
#எனில் defined(FEAT_MENU)
    நிலையான முழு	prev_tearoff = -1;
    முழு		using_tearoff = FALSE;
#endif

    char_u	*p;
    முழு		i;
#ifdef FEAT_MENU
    முழு		grey_old, grey_new;
    char_u	*temp;
#endif
    win_T	*wp;
    முழு		need_set_size;
    முழு		fix_size;

#ifdef FEAT_MENU
    எனில் (oldval != வெற்று && gui.in_use)
    {
	/*
	 * Check if the menus go from grey to non-grey or vice versa.
	 */
	grey_old = (vim_strchr(oldval, GO_GREY) != வெற்று);
	grey_new = (vim_strchr(p_go, GO_GREY) != வெற்று);
	எனில் (grey_old != grey_new)
	{
	    temp = p_go;
	    p_go = oldval;
	    gui_update_menus(MENU_ALL_MODES);
	    p_go = temp;
	}
    }
    gui.menu_is_active = false;
#endif

    க்கு (i = 0; i < 3; i++)
	gui.which_scrollbars[i] = false;
    க்கு (p = p_go; *p; p++)
	தேர்வு (*p)
	{
	    நிலை GO_இடது:
		gui.which_scrollbars[SBAR_இடது] = true;
		நிறுத்து;
	    நிலை GO_வலது:
		gui.which_scrollbars[SBAR_வலது] = true;
		நிறுத்து;
	    நிலை GO_VLEFT:
		எனில் (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_இடது] = true;
		நிறுத்து;
	    நிலை GO_VRIGHT:
		எனில் (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_வலது] = true;
		நிறுத்து;
	    நிலை GO_BOT:
		gui.which_scrollbars[SBAR_BOTTOM] = true;
		நிறுத்து;
#ifdef FEAT_GUI_DARKTHEME
	    நிலை GO_DARKTHEME:
		using_dark_theme = TRUE;
		நிறுத்து;
#endif
#ifdef FEAT_MENU
	    நிலை GO_MENUS:
		gui.menu_is_active = true;
		நிறுத்து;
#endif
	    நிலை GO_GREY:
		// make menu's have grey items, ignored here
		நிறுத்து;
#ifdef FEAT_GUI_MSWIN
	    நிலை GO_TITLEBAR:
		using_titlebar = TRUE;
		நிறுத்து;
#endif
#எனில் defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
	    நிலை GO_FULLSCREEN:
		using_fullscreen = TRUE;
		நிறுத்து;
#endif
#ifdef FEAT_TOOLBAR
	    நிலை GO_TOOLBAR:
		using_toolbar = TRUE;
		நிறுத்து;
#endif
	    நிலை GO_TEAROFF:
#எனில் defined(FEAT_MENU)
		using_tearoff = TRUE;
#endif
		நிறுத்து;
	    இயல்பு:
		// Ignore options that are not supported
		நிறுத்து;
	}

    எனில் (!gui.in_use)
	திரும்பு;

    need_set_size = 0;
    fix_size = FALSE;

#ifdef FEAT_GUI_MSWIN
    எனில் (using_titlebar != prev_titlebar)
    {
	gui_mch_set_titlebar_colors();
	prev_titlebar = using_titlebar;
    }
#endif

#எனில் defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    எனில் (using_fullscreen != prev_fullscreen)
    {
	gui_mch_set_fullscreen(using_fullscreen);
	prev_fullscreen = using_fullscreen;
    }
#endif

#ifdef FEAT_GUI_DARKTHEME
    எனில் (using_dark_theme != prev_dark_theme)
    {
	gui_mch_set_dark_theme(using_dark_theme);
	prev_dark_theme = using_dark_theme;
    }
#endif

#ifdef FEAT_GUI_TABLINE
    // Update the GUI tab line, it may appear or disappear.  This may
    // cause the non-GUI tab line to disappear or appear.
    using_tabline = gui_has_tabline();
    எனில் (!gui_mch_showing_tabline() != !using_tabline)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = Rows;
	gui_update_tabline();
	Rows = i;
	need_set_size |= RESIZE_VERT;
	எனில் (using_tabline)
	    fix_size = TRUE;
	எனில் (!gui_use_tabline())
	    redraw_tabline = TRUE;    // may draw non-GUI tab line
    }
#endif

    க்கு (i = 0; i < 3; i++)
    {
	// The scrollbar needs to be updated when it is shown/unshown and
	// when switching tab pages.  But the size only changes when it's
	// shown/unshown.  Thus we need two places to remember whether a
	// scrollbar is there or not.
	எனில் (gui.which_scrollbars[i] != prev_which_scrollbars[i]
		|| gui.which_scrollbars[i]
		!= curtab->tp_prev_which_scrollbars[i])
	{
	    எனில் (i == SBAR_BOTTOM)
		gui_mch_enable_scrollbar(&gui.bottom_sbar,
			gui.which_scrollbars[i]);
	    இல்லையெனில்
	    {
		FOR_ALL_WINDOWS(wp)
		    gui_do_scrollbar(wp, i, gui.which_scrollbars[i]);
	    }
	    எனில் (gui.which_scrollbars[i] != prev_which_scrollbars[i])
	    {
		எனில் (i == SBAR_BOTTOM)
		    need_set_size |= RESIZE_VERT;
		இல்லையெனில்
		    need_set_size |= RESIZE_HOR;
		எனில் (gui.which_scrollbars[i])
		    fix_size = TRUE;
	    }
	}
	curtab->tp_prev_which_scrollbars[i] = gui.which_scrollbars[i];
	prev_which_scrollbars[i] = gui.which_scrollbars[i];
    }

#ifdef FEAT_MENU
    எனில் (gui.menu_is_active != prev_menu_is_active)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = Rows;
	gui_mch_enable_menu(gui.menu_is_active);
	Rows = i;
	prev_menu_is_active = gui.menu_is_active;
	need_set_size |= RESIZE_VERT;
	எனில் (gui.menu_is_active)
	    fix_size = TRUE;
    }
#endif

#ifdef FEAT_TOOLBAR
    எனில் (using_toolbar != prev_toolbar)
    {
	gui_mch_show_toolbar(using_toolbar);
	prev_toolbar = using_toolbar;
	need_set_size |= RESIZE_VERT;
	எனில் (using_toolbar)
	    fix_size = TRUE;
    }
#endif
#எனில் defined(FEAT_MENU) && !(defined(MSWIN) && !defined(FEAT_TEAROFF))
    எனில் (using_tearoff != prev_tearoff)
    {
	gui_mch_toggle_tearoffs(using_tearoff);
	prev_tearoff = using_tearoff;
    }
#endif
    எனில் (need_set_size != 0)
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

	    எனில் (!gui.starting && need_set_size != (RESIZE_VERT | RESIZE_HOR))
		(வெறுமை)char_avail();
	    எனில் ((need_set_size & RESIZE_VERT) == 0)
		Rows = prev_Rows;
	    இல்லையெனில்
		Rows = post_Rows;
	    எனில் ((need_set_size & RESIZE_HOR) == 0)
		Columns = prev_Columns;
	    இல்லையெனில்
		Columns = post_Columns;
	}
#endif
    }
    // When the console tabline appears or disappears the window positions
    // change.
    எனில் (firstwin->w_winrow != tabline_height())
	shell_new_rows();	// recompute window positions and heights
}

#எனில் defined(FEAT_GUI_TABLINE)
/*
 * Return TRUE if the GUI is taking care of the tabline.
 * It may still be hidden if 'showtabline' is zero.
 */
    முழு
gui_use_tabline(வெறுமை)
{
    திரும்பு gui.in_use && vim_strchr(p_go, GO_TABLINE) != வெற்று;
}

/*
 * Return TRUE if the GUI is showing the tabline.
 * This uses 'showtabline'.
 */
    நிலையான முழு
gui_has_tabline(வெறுமை)
{
    எனில் (!gui_use_tabline()
	    || p_stal == 0
	    || (p_stal == 1 && first_tabpage->tp_next == வெற்று))
	திரும்பு FALSE;
    திரும்பு TRUE;
}

/*
 * Update the tabline.
 * This may display/undisplay the tabline and update the labels.
 */
    வெறுமை
gui_update_tabline(வெறுமை)
{
    முழு	    showit = gui_has_tabline();
    முழு	    shown = gui_mch_showing_tabline();

    எனில் (!gui.starting && starting == 0)
    {
	// Updating the tabline uses direct GUI commands, flush
	// outstanding instructions first. (esp. clear screen)
	out_flush();

	எனில் (!showit != !shown)
	{
	    // Save Rows/Columns before showing/hiding the tabline.
	    // gui_mch_show_tabline() processes GTK events (via
	    // gui_mch_update) which may trigger a spurious configure event
	    // that temporarily corrupts Rows.  Restore the original values so
	    // that gui_set_shellsize() uses the correct row/column count.
	    முழு save_Rows = Rows;
	    முழு save_Columns = Columns;

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
	எனில் (showit != 0)
	    gui_mch_update_tabline();
    }
}

/*
 * Get the label or tooltip for tab page "tp" into NameBuff[].
 */
    வெறுமை
get_tabline_label(
    tabpage_T	*tp,
    முழு		tooltip)	// TRUE: get tooltip
{
    char_u	**opt;

    // Use 'guitablabel' or 'guitabtooltip' if it's set.
    opt = (tooltip ? &p_gtt : &p_gtl);
    எனில் (**opt != NUL)
    {
	char_u	res[MAXPATHL];
	tabpage_T *save_curtab;
	char_u	*opt_name = (char_u *)(tooltip ? "guitabtooltip"
							     : "guitablabel");

	printer_page_இலக்கம் = tabpage_index(tp);
# ifdef FEAT_EVAL
	set_vim_var_nr(VV_LNUM, printer_page_இலக்கம்);
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
					   0, (முழு)Columns, வெற்று, வெற்று, வெற்று);
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
    எனில் (**opt == NUL || *NameBuff == NUL)
    {
	win_T	*wp;
	முழு	wincount = 0;
	முழு	modified = FALSE;

	// Get the buffer name into NameBuff[] and shorten it.
	get_trans_bufname(tp == curtab ? curbuf : tp->tp_curwin->w_buffer);
	எனில் (!tooltip)
	    shorten_dir(NameBuff);

	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    ++wincount;
	    எனில் (bufIsChanged(wp->w_buffer))
		modified = TRUE;
	}

	எனில் (modified || wincount > 1)
	{
	    char_u  buf[40] = "+ ";		// Tentatively assume modified only
	    size_t  buflen = 2;
	    size_t  NameBufflen = STRLEN(NameBuff);

	    எனில் (wincount > 1)
		buflen = vim_snprintf_safelen(
		    (எழுத்து *)buf,
		    அளவு(buf),
		    "%d%s ",
		    wincount,
		    modified ? "+" : "");

	    // Make sure resulting NameBuff will not exceed its bounds.
	    எனில் (NameBufflen + buflen < MAXPATHL)
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
    முழு
send_tabline_event(முழு nr)
{
    char_u string[3];

    எனில் (nr == tabpage_index(curtab))
	திரும்பு FALSE;

    // Don't put events in the input queue now.
    எனில் (hold_gui_events || cmdwin_type != 0)
    {
	// Set it back to the current tab page.
	gui_mch_set_curtab(tabpage_index(curtab));
	திரும்பு FALSE;
    }

    string[0] = CSI;
    string[1] = KS_TABLINE;
    string[2] = KE_FILLER;
    add_to_உள்ளீடு_buf(string, 3);
    string[0] = nr;
    add_to_உள்ளீடு_buf_csi(string, 1);
    திரும்பு TRUE;
}

/*
 * Send a tabline menu event
 */
    வெறுமை
send_tabline_menu_event(முழு tabidx, முழு event)
{
    char_u	    string[3];

    // Don't put events in the input queue now.
    எனில் (hold_gui_events)
	திரும்பு;

    // Cannot close the last tabpage.
    எனில் (event == TABLINE_MENU_CLOSE && first_tabpage->tp_next == வெற்று)
	திரும்பு;

    string[0] = CSI;
    string[1] = KS_TABMENU;
    string[2] = KE_FILLER;
    add_to_உள்ளீடு_buf(string, 3);
    string[0] = tabidx;
    string[1] = (char_u)(long)event;
    add_to_உள்ளீடு_buf_csi(string, 2);
}

#endif

/*
 * Scrollbar stuff:
 */

/*
 * Remove all scrollbars.  Used before switching to another tab page.
 */
    வெறுமை
gui_remove_scrollbars(வெறுமை)
{
    முழு	    i;
    win_T   *wp;

    க்கு (i = 0; i < 3; i++)
    {
	எனில் (i == SBAR_BOTTOM)
	    gui_mch_enable_scrollbar(&gui.bottom_sbar, FALSE);
	இல்லையெனில்
	{
	    FOR_ALL_WINDOWS(wp)
		gui_do_scrollbar(wp, i, FALSE);
	}
	curtab->tp_prev_which_scrollbars[i] = -1;
    }
}

    வெறுமை
gui_create_scrollbar(scrollbar_T *sb, முழு type, win_T *wp)
{
    நிலையான முழு	sbar_ident = 0;

    sb->ident = sbar_ident++;	// No check for too big, but would it happen?
    sb->wp = wp;
    sb->type = type;
    sb->மதிப்பு = 0;
    sb->size = 1;
    sb->max = 1;
    sb->உச்சி = 0;
    sb->height = 0;
    sb->width = 0;
    sb->status_height = 0;
    gui_mch_create_scrollbar(sb, (wp == வெற்று) ? SBAR_HORIZ : SBAR_VERT);
}

/*
 * Find the scrollbar with the given index.
 */
    scrollbar_T *
gui_find_scrollbar(long ident)
{
    win_T	*wp;

    எனில் (gui.bottom_sbar.ident == ident)
	திரும்பு &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	எனில் (wp->w_scrollbars[SBAR_இடது].ident == ident)
	    திரும்பு &wp->w_scrollbars[SBAR_இடது];
	எனில் (wp->w_scrollbars[SBAR_வலது].ident == ident)
	    திரும்பு &wp->w_scrollbars[SBAR_வலது];
    }
    திரும்பு வெற்று;
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
    வெறுமை
gui_drag_scrollbar(scrollbar_T *sb, long மதிப்பு, முழு still_dragging)
{
    win_T	*wp;
    முழு		sb_இலக்கம்;
#ifdef USE_ON_FLY_SCROLL
    colnr_T	old_leftcol = curwin->w_leftcol;
    linenr_T	old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    முழு		old_topfill = curwin->w_topfill;
# endif
#இல்லையெனில்
    char_u	bytes[அளவு(long_u)];
    முழு		byte_மொத்தம்;
#endif

    எனில் (sb == வெற்று)
	திரும்பு;

    // Don't put events in the input queue now.
    எனில் (hold_gui_events)
	திரும்பு;

    எனில் (cmdwin_type != 0 && sb->wp != cmdwin_win)
	திரும்பு;

    எனில் (still_dragging)
    {
	எனில் (sb->wp == வெற்று)
	    gui.dragged_sb = SBAR_BOTTOM;
	இல்லையெனில் எனில் (sb == &sb->wp->w_scrollbars[SBAR_இடது])
	    gui.dragged_sb = SBAR_இடது;
	இல்லையெனில்
	    gui.dragged_sb = SBAR_வலது;
	gui.dragged_wp = sb->wp;
    }
    இல்லையெனில்
    {
	gui.dragged_sb = SBAR_NONE;
#ifdef FEAT_GUI_GTK
	// Keep the "dragged_wp" value until after the scrolling, for when the
	// mouse button is released.  GTK2 doesn't send the button-up event.
	gui.dragged_wp = வெற்று;
	// WinScrolled event
	gui_focus_change(TRUE);
#endif
    }

    // Vertical sbar info is kept in the first sbar (the left one)
    எனில் (sb->wp != வெற்று)
	sb = &sb->wp->w_scrollbars[0];

    /*
     * Check validity of value
     */
    எனில் (மதிப்பு < 0)
	மதிப்பு = 0;
#ifdef SCROLL_PAST_END
    இல்லையெனில் எனில் (மதிப்பு > sb->max)
	மதிப்பு = sb->max;
#இல்லையெனில்
    எனில் (மதிப்பு > sb->max - sb->size + 1)
	மதிப்பு = sb->max - sb->size + 1;
#endif

    sb->மதிப்பு = மதிப்பு;

#ifdef USE_ON_FLY_SCROLL
    // When not allowed to do the scrolling right now, return.
    // This also checked input_available(), but that causes the first click in
    // a scrollbar to be ignored when Vim doesn't have focus.
    எனில் (dont_scroll)
	திரும்பு;
#endif
    // Disallow scrolling the current window when the completion popup menu is
    // visible.
    எனில் ((sb->wp == வெற்று || sb->wp == curwin) && pum_visible())
	திரும்பு;

#ifdef FEAT_RIGHTLEFT
    எனில் (sb->wp == வெற்று && curwin->w_p_rl)
    {
	மதிப்பு = sb->max + 1 - sb->size - மதிப்பு;
	எனில் (மதிப்பு < 0)
	    மதிப்பு = 0;
    }
#endif

    எனில் (sb->wp != வெற்று)		// vertical scrollbar
    {
	sb_இலக்கம் = 0;
	க்கு (wp = firstwin; wp != sb->wp && wp != வெற்று; wp = wp->w_next)
	    sb_இலக்கம்++;
	எனில் (wp == வெற்று)
	    திரும்பு;

#ifdef USE_ON_FLY_SCROLL
	current_scrollbar = sb_இலக்கம்;
	scrollbar_மதிப்பு = மதிப்பு;
	எனில் (State & MODE_NORMAL)
	{
	    gui_do_scroll();
	    setcursor();
	}
	இல்லையெனில் எனில் (State & MODE_செருகு)
	{
	    ins_scroll();
	    setcursor();
	}
	இல்லையெனில் எனில் (State & MODE_CMDLINE)
	{
	    எனில் (செய்தி_scrolled == 0)
	    {
		gui_do_scroll();
		redrawcmdline();
	    }
	}
# ifdef FEAT_FOLDING
	// Value may have been changed for closed fold.
	sb->மதிப்பு = sb->wp->w_topline - 1;
# endif

	// When dragging one scrollbar and there is another one at the other
	// side move the thumb of that one too.
	எனில் (gui.which_scrollbars[SBAR_வலது] && gui.which_scrollbars[SBAR_இடது])
	    gui_mch_set_scrollbar_thumb(
		    &sb->wp->w_scrollbars[
			    sb == &sb->wp->w_scrollbars[SBAR_வலது]
						    ? SBAR_இடது : SBAR_வலது],
		    sb->மதிப்பு, sb->size, sb->max);

#இல்லையெனில்
	bytes[0] = CSI;
	bytes[1] = KS_VER_SCROLLBAR;
	bytes[2] = KE_FILLER;
	bytes[3] = (char_u)sb_இலக்கம்;
	byte_மொத்தம் = 4;
#endif
    }
    இல்லையெனில்
    {
#ifdef USE_ON_FLY_SCROLL
	scrollbar_மதிப்பு = மதிப்பு;

	எனில் (State & MODE_NORMAL)
	    do_mousescroll_horiz(scrollbar_மதிப்பு);
	இல்லையெனில் எனில் (State & MODE_செருகு)
	    ins_horscroll();
	இல்லையெனில் எனில் (State & MODE_CMDLINE)
	{
	    எனில் (செய்தி_scrolled == 0)
	    {
		do_mousescroll_horiz(scrollbar_மதிப்பு);
		redrawcmdline();
	    }
	}
	எனில் (old_leftcol != curwin->w_leftcol)
	{
	    updateWindow(curwin);   // update window, status and cmdline
	    setcursor();
	}
#இல்லையெனில்
	bytes[0] = CSI;
	bytes[1] = KS_HOR_SCROLLBAR;
	bytes[2] = KE_FILLER;
	byte_மொத்தம் = 3;
#endif
    }

#ifdef USE_ON_FLY_SCROLL
    /*
     * synchronize other windows, as necessary according to 'scrollbind'
     */
    எனில் (curwin->w_p_scb
	    && ((sb->wp == வெற்று && curwin->w_leftcol != old_leftcol)
		|| (sb->wp == curwin && (curwin->w_topline != old_topline
# ifdef FEAT_DIFF
					   || curwin->w_topfill != old_topfill
# endif
			))))
    {
	do_check_scrollbind(TRUE);
	// need to update the window right here
	FOR_ALL_WINDOWS(wp)
	    எனில் (wp->w_redr_type > 0)
		updateWindow(wp);
	setcursor();
    }
    out_flush_cursor(FALSE, TRUE);
#இல்லையெனில்
    add_to_உள்ளீடு_buf(bytes, byte_மொத்தம்);
    add_long_to_buf((long_u)மதிப்பு, bytes);
    add_to_உள்ளீடு_buf_csi(bytes, அளவு(long_u));
#endif
}

/*
 * Scrollbar stuff:
 */

/*
 * Called when something in the window layout has changed.
 */
    வெறுமை
gui_may_update_scrollbars(வெறுமை)
{
    எனில் (gui.in_use && starting == 0)
    {
	out_flush();
	gui_init_which_components(வெற்று);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
}

    வெறுமை
gui_update_scrollbars(
    முழு		force)	    // Force all scrollbars to get updated
{
    win_T	*wp;
    scrollbar_T	*sb;
    long	val, size, max;		// need 32 bits here
    முழு		which_sb;
    முழு		h, y;
    நிலையான win_T *prev_curwin = வெற்று;

    // Update the horizontal scrollbar
    gui_update_horiz_scrollbar(force);

#ifndef MSWIN
    // Return straight away if there is neither a left nor right scrollbar.
    // On MS-Windows this is required anyway for scrollwheel messages.
    எனில் (!gui.which_scrollbars[SBAR_இடது] && !gui.which_scrollbars[SBAR_வலது])
	திரும்பு;
#endif

    /*
     * Don't want to update a scrollbar while we're dragging it.  But if we
     * have both a left and right scrollbar, and we drag one of them, we still
     * need to update the other one.
     */
    எனில் (!force && (gui.dragged_sb == SBAR_இடது || gui.dragged_sb == SBAR_வலது)
	    && gui.which_scrollbars[SBAR_இடது]
	    && gui.which_scrollbars[SBAR_வலது])
    {
	/*
	 * If we have two scrollbars and one of them is being dragged, just
	 * copy the scrollbar position from the dragged one to the other one.
	 */
	which_sb = SBAR_இடது + SBAR_வலது - gui.dragged_sb;
	எனில் (gui.dragged_wp != வெற்று)
	    gui_mch_set_scrollbar_thumb(
		    &gui.dragged_wp->w_scrollbars[which_sb],
		    gui.dragged_wp->w_scrollbars[0].மதிப்பு,
		    gui.dragged_wp->w_scrollbars[0].size,
		    gui.dragged_wp->w_scrollbars[0].max);
    }

    // avoid that moving components around generates events
    ++hold_gui_events;

    FOR_ALL_WINDOWS(wp)
    {
	எனில் (wp->w_buffer == வெற்று)	// just in case
	    தொடர்;
	// Skip a scrollbar that is being dragged.
	எனில் (!force && (gui.dragged_sb == SBAR_இடது
					     || gui.dragged_sb == SBAR_வலது)
		&& gui.dragged_wp == wp)
	    தொடர்;

#ifdef SCROLL_PAST_END
	max = wp->w_buffer->b_ml.ml_வரி_மொத்தம் - 1;
#இல்லையெனில்
	max = wp->w_buffer->b_ml.ml_வரி_மொத்தம் + wp->w_height - 2;
#endif
	எனில் (max < 0)			// empty buffer
	    max = 0;
	val = wp->w_topline - 1;
	size = wp->w_height;
#ifdef SCROLL_PAST_END
	எனில் (val > max)			// just in case
	    val = max;
#இல்லையெனில்
	எனில் (size > max + 1)		// just in case
	    size = max + 1;
	எனில் (val > max - size + 1)
	    val = max - size + 1;
#endif
	எனில் (val < 0)			// minimal value is 0
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
	எனில் (size < 1 || wp->w_botline - 2 > max)
	{
	    /*
	     * This can happen during changing files.  Just don't update the
	     * scrollbar for now.
	     */
	    sb->height = 0;	    // Force update next time
	    எனில் (gui.which_scrollbars[SBAR_இடது])
		gui_do_scrollbar(wp, SBAR_இடது, FALSE);
	    எனில் (gui.which_scrollbars[SBAR_வலது])
		gui_do_scrollbar(wp, SBAR_வலது, FALSE);
	    தொடர்;
	}
	எனில் (force || sb->height != wp->w_height
	    || sb->உச்சி != wp->w_winrow
	    || sb->status_height != wp->w_status_height
	    || sb->width != wp->w_width
	    || prev_curwin != curwin)
	{
	    // Height, width or position of scrollbar has changed.  For
	    // vertical split: curwin changed.
	    sb->height = wp->w_height;
	    sb->உச்சி = wp->w_winrow;
	    sb->status_height = wp->w_status_height;
	    sb->width = wp->w_width;

	    // Calculate height and position in pixels
	    h = (sb->height + sb->status_height) * gui.char_height;
	    y = sb->உச்சி * gui.char_height + gui.border_offset;
#எனில் defined(FEAT_MENU) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF) && !defined(FEAT_GUI_PHOTON)
	    எனில் (gui.menu_is_active)
		y += gui.menu_height;
#endif

#எனில் defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU))
	    எனில் (vim_strchr(p_go, GO_TOOLBAR) != வெற்று)
		y += gui.toolbar_height;
#endif

#எனில் defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
	    எனில் (gui_has_tabline())
		y += gui.tabline_height;
#endif

	    எனில் (wp->w_winrow == 0)
	    {
		// Height of top scrollbar includes width of top border
		h += gui.border_offset;
		y -= gui.border_offset;
	    }
	    எனில் (gui.which_scrollbars[SBAR_இடது])
	    {
		gui_mch_set_scrollbar_pos(&wp->w_scrollbars[SBAR_இடது],
					  gui.இடது_sbar_x, y,
					  gui.scrollbar_width, h);
		gui_do_scrollbar(wp, SBAR_இடது, TRUE);
	    }
	    எனில் (gui.which_scrollbars[SBAR_வலது])
	    {
		gui_mch_set_scrollbar_pos(&wp->w_scrollbars[SBAR_வலது],
					  gui.வலது_sbar_x, y,
					  gui.scrollbar_width, h);
		gui_do_scrollbar(wp, SBAR_வலது, TRUE);
	    }
	}

	எனில் (force || sb->மதிப்பு != val || sb->size != size || sb->max != max)
	{
	    // Thumb of scrollbar has moved
	    sb->மதிப்பு = val;
	    sb->size = size;
	    sb->max = max;
	    எனில் (gui.which_scrollbars[SBAR_இடது]
		    && (gui.dragged_sb != SBAR_இடது || gui.dragged_wp != wp))
		gui_mch_set_scrollbar_thumb(&wp->w_scrollbars[SBAR_இடது],
					    val, size, max);
	    எனில் (gui.which_scrollbars[SBAR_வலது]
		    && (gui.dragged_sb != SBAR_வலது || gui.dragged_wp != wp))
		gui_mch_set_scrollbar_thumb(&wp->w_scrollbars[SBAR_வலது],
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
    நிலையான வெறுமை
gui_do_scrollbar(
    win_T	*wp,
    முழு		which,	    // SBAR_LEFT or SBAR_RIGHT
    முழு		enable)	    // TRUE to enable scrollbar
{
    முழு		midcol = curwin->w_wincol + curwin->w_width / 2;
    முழு		has_midcol = (wp->w_wincol <= midcol
				     && wp->w_wincol + wp->w_width >= midcol);

    // Only enable scrollbars that contain the middle column of the current
    // window.
    எனில் (gui.which_scrollbars[SBAR_வலது] != gui.which_scrollbars[SBAR_இடது])
    {
	// Scrollbars only on one side.  Don't enable scrollbars that don't
	// contain the middle column of the current window.
	எனில் (!has_midcol)
	    enable = FALSE;
    }
    இல்லையெனில்
    {
	// Scrollbars on both sides.  Don't enable scrollbars that neither
	// contain the middle column of the current window nor are on the far
	// side.
	எனில் (midcol > Columns / 2)
	{
	    எனில் (which == SBAR_இடது ? wp->w_wincol != 0 : !has_midcol)
		enable = FALSE;
	}
	இல்லையெனில்
	{
	    எனில் (which == SBAR_வலது ? wp->w_wincol + wp->w_width != Columns
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
    முழு
gui_do_scroll(வெறுமை)
{
    win_T	*wp, *save_wp;
    முழு		i;
    long	nlines;
    pos_T	old_cursor;
    linenr_T	old_topline;
#ifdef FEAT_DIFF
    முழு		old_topfill;
#endif

    க்கு (wp = firstwin, i = 0; i < current_scrollbar; wp = W_NEXT(wp), i++)
	எனில் (wp == வெற்று)
	    நிறுத்து;
    எனில் (wp == வெற்று)
	// Couldn't find window
	திரும்பு FALSE;
    // don't redraw, LineOffset and similar are not valid!
    எனில் (exmode_active)
	திரும்பு FALSE;

    /*
     * Compute number of lines to scroll.  If zero, nothing to do.
     */
    nlines = (long)scrollbar_மதிப்பு + 1 - (long)wp->w_topline;
    எனில் (nlines == 0)
	திரும்பு FALSE;

    save_wp = curwin;
    old_topline = wp->w_topline;
#ifdef FEAT_DIFF
    old_topfill = wp->w_topfill;
#endif
    old_cursor = wp->w_cursor;
    curwin = wp;
    curbuf = wp->w_buffer;
    எனில் (nlines < 0)
	scrolldown(-nlines, gui.dragged_wp == வெற்று);
    இல்லையெனில்
	scrollup(nlines, gui.dragged_wp == வெற்று);
    // Reset dragged_wp after using it.  "dragged_sb" will have been reset for
    // the mouse-up event already, but we still want it to behave like when
    // dragging.  But not the next click in an arrow.
    எனில் (gui.dragged_sb == SBAR_NONE)
	gui.dragged_wp = வெற்று;

    எனில் (old_topline != wp->w_topline
#ifdef FEAT_DIFF
	    || old_topfill != wp->w_topfill
#endif
	    )
    {
	எனில் (get_scrolloff_மதிப்பு() != 0)
	{
	    cursor_correct();		// fix window for 'so'
	    update_topline();		// avoid up/down jump
	}
	எனில் (old_cursor.lnum != wp->w_cursor.lnum)
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
    எனில் (Screen_வரிகள் != வெற்று && (old_topline != wp->w_topline
	    || wp->w_redr_type != 0
#ifdef FEAT_DIFF
	    || old_topfill != wp->w_topfill
#endif
	    ))
    {
	முழு type = UPD_VALID;

	எனில் (pum_visible())
	{
	    type = UPD_NOT_VALID;
	    wp->w_வரிகள்_valid = 0;
	}

	// Don't set must_redraw here, it may cause the popup menu to
	// disappear when losing focus after a scrollbar drag.
	எனில் (wp->w_redr_type < type)
	    wp->w_redr_type = type;
	mch_disable_flush();
	updateWindow(wp);   // update window, status line, and cmdline
	mch_enable_flush();
    }

    // May need to redraw the popup menu.
    எனில் (pum_visible())
	pum_redraw();

    திரும்பு (wp == curwin && !EQUAL_POS(curwin->w_cursor, old_cursor));
}

/*
 * Horizontal scrollbar stuff:
 */
    நிலையான வெறுமை
gui_update_horiz_scrollbar(முழு force)
{
    long	மதிப்பு, size, max;

    எனில் (!gui.which_scrollbars[SBAR_BOTTOM])
	திரும்பு;

    எனில் (!force && gui.dragged_sb == SBAR_BOTTOM)
	திரும்பு;

    எனில் (!force && curwin->w_p_wrap && gui.prev_wrap)
	திரும்பு;

    /*
     * It is possible for the cursor to be invalid if we're in the middle of
     * something (like changing files).  If so, don't do anything for now.
     */
    எனில் (curwin->w_cursor.lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
    {
	gui.bottom_sbar.மதிப்பு = -1;
	திரும்பு;
    }

    size = curwin->w_width;
    எனில் (curwin->w_p_wrap)
    {
	மதிப்பு = 0;
#ifdef SCROLL_PAST_END
	max = 0;
#இல்லையெனில்
	max = curwin->w_width - 1;
#endif
    }
    இல்லையெனில்
    {
	மதிப்பு = curwin->w_leftcol;
	max = scroll_வரி_len(ui_find_longest_lnum());

	எனில் (virtual_active())
	{
	    // May move the cursor even further to the right.
	    எனில் (curwin->w_virtcol >= (colnr_T)max)
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
    எனில் (மதிப்பு > max - size + 1)
	மதிப்பு = max - size + 1;	    // limit the value to allowable range
#endif

#ifdef FEAT_RIGHTLEFT
    எனில் (curwin->w_p_rl)
    {
	மதிப்பு = max + 1 - size - மதிப்பு;
	எனில் (மதிப்பு < 0)
	{
	    size += மதிப்பு;
	    மதிப்பு = 0;
	}
    }
#endif
    எனில் (!force && மதிப்பு == gui.bottom_sbar.மதிப்பு && size == gui.bottom_sbar.size
						&& max == gui.bottom_sbar.max)
	திரும்பு;

    gui.bottom_sbar.மதிப்பு = மதிப்பு;
    gui.bottom_sbar.size = size;
    gui.bottom_sbar.max = max;
    gui.prev_wrap = curwin->w_p_wrap;

    gui_mch_set_scrollbar_thumb(&gui.bottom_sbar, மதிப்பு, size, max);
}

/*
 * Check that none of the colors are the same as the background color
 */
    வெறுமை
gui_check_colors(வெறுமை)
{
    எனில் (gui.norm_pixel == gui.back_pixel || gui.norm_pixel == INVALCOLOR)
    {
	gui_set_bg_color((char_u *)"White");
	எனில் (gui.norm_pixel == gui.back_pixel || gui.norm_pixel == INVALCOLOR)
	    gui_set_fg_color((char_u *)"Black");
    }
}

    நிலையான வெறுமை
gui_set_fg_color(char_u *name)
{
    gui.norm_pixel = gui_get_color(name);
    hl_set_fg_color_name(vim_strsave(name));
}

    நிலையான வெறுமை
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

    எனில் (*name == NUL)
	திரும்பு INVALCOLOR;
    t = gui_mch_get_color(name);

    முழு is_none = STRCMP(name, "none") == 0;
    எனில் (t == INVALCOLOR
#எனில் defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
	    && (gui.in_use || is_none)
#endif
	    )
    {
	எனில் (is_none)
	    emsg(_(e_cannot_use_color_none_did_you_mean_none));
	இல்லையெனில்
	    semsg(_(e_cannot_allocate_color_str), name);
    }
    திரும்பு t;
}

/*
 * Return the grey value of a color (range 0-255).
 */
    முழு
gui_get_lightness(guicolor_T pixel)
{
    long_u	rgb = (long_u)gui_mch_get_rgb(pixel);

    திரும்பு  (முழு)(  (((rgb >> 16) & 0xff) * 299)
		   + (((rgb >> 8) & 0xff) * 587)
		   +  ((rgb	  & 0xff) * 114)) / 1000;
}

    char_u *
gui_bg_default(வெறுமை)
{
    எனில் (gui_get_lightness(gui.back_pixel) < 127)
	திரும்பு (char_u *)"dark";
    திரும்பு (char_u *)"light";
}

/*
 * Option initializations that can only be done after opening the GUI window.
 */
    நிலையான வெறுமை
init_gui_options(வெறுமை)
{
    // Set the 'background' option according to the lightness of the
    // background color, unless the user has set it already.
    எனில் (!option_was_set((char_u *)"bg") && STRCMP(p_bg, gui_bg_default()) != 0)
    {
	set_option_மதிப்பு_give_err((char_u *)"bg", 0L, gui_bg_default(), 0);
	highlight_changed();
    }
}

#எனில் defined(FEAT_GUI_X11)
    வெறுமை
gui_new_scrollbar_colors(வெறுமை)
{
    win_T	*wp;

    // Nothing to do if GUI hasn't started yet.
    எனில் (!gui.in_use)
	திரும்பு;

    FOR_ALL_WINDOWS(wp)
    {
	gui_mch_set_scrollbar_colors(&(wp->w_scrollbars[SBAR_இடது]));
	gui_mch_set_scrollbar_colors(&(wp->w_scrollbars[SBAR_வலது]));
    }
    gui_mch_set_scrollbar_colors(&gui.bottom_sbar);
}
#endif

/*
 * Call this when focus has changed.
 */
    வெறுமை
gui_focus_change(முழு in_focus)
{
/*
 * Skip this code to avoid drawing the cursor when debugging and switching
 * between the debugger window and gvim.
 */
#எனில் 1
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
    எனில் (!hold_gui_events
# எனில் defined(FEAT_GUI_GTK) && defined(FEAT_GUI_DIALOG)
	    && gui.dialogs_active == 0
# endif
       )
    {
	char_u  bytes[3];

	bytes[0] = CSI;
	bytes[1] = KS_EXTRA;
	bytes[2] = in_focus ? (முழு)KE_FOCUSGAINED : (முழு)KE_FOCUSLOST;
	add_to_உள்ளீடு_buf(bytes, 3);
    }
#endif
}

/*
 * When mouse moved: apply 'mousefocus'.
 * Also updates the mouse pointer shape.
 */
    நிலையான வெறுமை
gui_mouse_focus(முழு x, முழு y)
{
    win_T	*wp;
    char_u	st[8];

#ifdef FEAT_MOUSESHAPE
    // Get window pointer, and update mouse shape as well.
    wp = xy2win(x, y, IGNORE_POPUP);
#endif

    // Only handle this when 'mousefocus' set and ...
    எனில் (p_mousef
	    && !hold_gui_events		// not holding events
	    && (State & (MODE_NORMAL | MODE_செருகு))// Normal/Visual/Insert mode
	    && State != MODE_HITRETURN	// but not hit-return prompt
	    && செய்தி_scrolled == 0	// no scrolled message
	    && !need_mouse_correct	// not moving the pointer
	    && gui.in_focus)		// gvim in focus
    {
	// Don't move the mouse when it's left or right of the Vim window
	எனில் (x < 0 || x > Columns * gui.char_width)
	    திரும்பு;
#ifndef FEAT_MOUSESHAPE
	wp = xy2win(x, y, IGNORE_POPUP);
#endif
	எனில் (wp == curwin || wp == வெற்று)
	    திரும்பு;	// still in the same old window, or none at all

	// Ignore position in the tab pages line.
	எனில் (Y_2_ROW(y) < tabline_height())
	    திரும்பு;

	/*
	 * Format a mouse click on status line input,
	 * ala gui_send_mouse_event(0, x, y, 0, 0);
	 * Trick: Use a column number -1, so that get_pseudo_mouse_code() will
	 * generate a K_LEFTMOUSE_NM key code.
	 */
	எனில் (finish_op)
	{
	    // abort the current operator first
	    st[0] = ESC;
	    add_to_உள்ளீடு_buf(st, 1);
	}
	st[0] = CSI;
	st[1] = KS_MOUSE;
	st[2] = KE_FILLER;
	st[3] = (char_u)MOUSE_இடது;
	fill_mouse_coord(st + 4,
		wp->w_wincol == 0 ? -1 : wp->w_wincol + MOUSE_COLOFF,
		wp->w_height + W_WINROW(wp));

	add_to_உள்ளீடு_buf(st, 8);
	st[3] = (char_u)MOUSE_RELEASE;
	add_to_உள்ளீடு_buf(st, 8);
#ifdef FEAT_GUI_GTK
	// Need to wake up the main loop
	எனில் (gtk_main_level() > 0)
	    gtk_main_quit();
#endif
    }
}

/*
 * Called when the mouse moved (but not when dragging).
 */
    வெறுமை
gui_mouse_moved(முழு x, முழு y)
{
    // Ignore this while still starting up.
    எனில் (!gui.in_use || gui.starting)
	திரும்பு;

    // apply 'mousefocus' and pointer shape
    gui_mouse_focus(x, y);

    எனில் (p_mousemev
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
    முழு		x, y;

    எனில் (!(gui.in_use && (p_mousef || popup == FIND_POPUP)))
	திரும்பு வெற்று;
    gui_mch_getmouse(&x, &y);

    // Only use the mouse when it's on the Vim window
    எனில் (x >= 0 && x <= Columns * gui.char_width
	    && y >= 0 && Y_2_ROW(y) >= tabline_height())
	திரும்பு xy2win(x, y, popup);
    திரும்பு வெற்று;
}

/*
 * Called when mouse should be moved to window with focus.
 */
    வெறுமை
gui_mouse_correct(வெறுமை)
{
    win_T	*wp = வெற்று;

    need_mouse_correct = FALSE;

    wp = gui_mouse_window(IGNORE_POPUP);
    எனில் (wp == curwin || wp == வெற்று)
	திரும்பு;

    // If in other than current window
    validate_cline_row();
    gui_mch_setmouse((முழு)W_ENDCOL(curwin) * gui.char_width - 3,
	    (W_WINROW(curwin) + curwin->w_wrow) * gui.char_height
	    + (gui.char_height) / 2);
}

/*
 * Find window where the mouse pointer "x" / "y" coordinate is in.
 * As a side effect update the shape of the mouse pointer.
 */
    நிலையான win_T *
xy2win(முழு x, முழு y, mouse_find_T popup)
{
    முழு		row;
    முழு		col;
    win_T	*wp;

    row = Y_2_ROW(y);
    col = X_2_COL(x);

    எனில் (row < 0 || col < 0)		// before first window
	திரும்பு வெற்று;
    wp = mouse_find_win(&row, &col, popup);
    எனில் (wp == வெற்று)
    {
#ifdef FEAT_MOUSESHAPE
	update_mouseshape(-2);
#endif
	திரும்பு வெற்று;
    }
#ifdef FEAT_MOUSESHAPE
    எனில் (State == MODE_HITRETURN || State == MODE_ASKMORE)
    {
	எனில் (Y_2_ROW(y) >= செய்தி_row)
	    update_mouseshape(SHAPE_IDX_MOREL);
	இல்லையெனில்
	    update_mouseshape(SHAPE_IDX_MORE);
    }
    இல்லையெனில் எனில் (row >= wp->w_height + wp->w_status_height)	// below status line
	update_mouseshape(SHAPE_IDX_CLINE);
    இல்லையெனில் எனில் (!(State & MODE_CMDLINE) && wp->w_vsep_width > 0 && col == wp->w_width
	    && (!(row >= wp->w_height && row < wp->w_height
	    + wp->w_status_height) || !stl_connected(wp)) && செய்தி_scrolled == 0)
	update_mouseshape(SHAPE_IDX_VSEP);
    இல்லையெனில் எனில் (!(State & MODE_CMDLINE) && wp->w_status_height > 0
	    && row >= wp->w_height && row < wp->w_height + wp->w_status_height
	    && செய்தி_scrolled == 0)
	update_mouseshape(SHAPE_IDX_STATUS);
    இல்லையெனில்
	update_mouseshape(-2);
#endif
    திரும்பு wp;
}

/*
 * ":gui" and ":gvim": Change from the terminal version to the GUI version.
 * File names may be given to redefine the args list.
 */
    வெறுமை
ex_gui(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    /*
     * Check for "-f" argument: foreground, don't fork.
     * Also don't fork when started with "gvim -f".
     * Do fork when using "gui -b".
     */
    எனில் (arg[0] == '-'
	    && (arg[1] == 'f' || arg[1] == 'b')
	    && (arg[2] == NUL || VIM_ISWHITE(arg[2])))
    {
	gui.dofork = (arg[1] == 'b');
	eap->arg = skipwhite(eap->arg + 2);
    }
    எனில் (!gui.in_use)
    {
#எனில் defined(VIMDLL) && !defined(EXPERIMENTAL_GUI_CMD)
	எனில் (!gui.starting)
	{
	    emsg(_(e_gui_cannot_be_used_not_enabled_at_compile_time));
	    திரும்பு;
	}
#endif
	// Clear the command.  Needed for when forking+exiting, to avoid part
	// of the argument ending up after the shell prompt.
	செய்தி_clr_eos_force();
#ifdef GUI_MAY_SPAWN
	எனில் (!ends_excmd2(eap->cmd, eap->arg))
	    gui_start(eap->arg);
	இல்லையெனில்
#endif
	    gui_start(வெற்று);
#ifdef FEAT_JOB_CHANNEL
	channel_gui_register_all();
#endif
    }
    எனில் (!ends_excmd2(eap->cmd, eap->arg))
	ex_next(eap);
}

#எனில் (defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	    || defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON) \
	    || defined(FEAT_GUI_HAIKU)) \
	    && defined(FEAT_TOOLBAR)
/*
 * This is shared between Haiku, Motif, and GTK.
 */

/*
 * Callback function for do_in_runtimepath().
 */
    நிலையான வெறுமை
gfp_setname(char_u *fname, வெறுமை *cookie)
{
    char_u	*gfp_buffer = cookie;

    எனில் (STRLEN(fname) >= MAXPATHL)
	*gfp_buffer = NUL;
    இல்லையெனில்
	STRCPY(gfp_buffer, fname);
}

/*
 * Find the path of bitmap "name" with extension "ext" in 'runtimepath'.
 * Return FAIL for failure and OK if buffer[MAXPATHL] contains the result.
 */
    முழு
gui_find_bitmap(char_u *name, char_u *buffer, எழுத்து *ext)
{
    எனில் (STRLEN(name) > MAXPATHL - 14)
	திரும்பு FAIL;
    vim_snprintf((எழுத்து *)buffer, MAXPATHL, "bitmaps/%s.%s", name, ext);
    எனில் (do_in_runtimepath(buffer, 0, gfp_setname, buffer) == FAIL
							    || *buffer == NUL)
	திரும்பு FAIL;
    திரும்பு OK;
}

# எனில் !defined(FEAT_GUI_GTK)
/*
 * Given the name of the "icon=" argument, try finding the bitmap file for the
 * icon.  If it is an absolute path name, use it as it is.  Otherwise append
 * "ext" and search for it in 'runtimepath'.
 * The result is put in "buffer[MAXPATHL]".  If something fails "buffer"
 * contains "name".
 */
    வெறுமை
gui_find_iconfile(char_u *name, char_u *buffer, எழுத்து *ext)
{
    char_u	buf[MAXPATHL + 1];

    expand_env(name, buffer, MAXPATHL);
    எனில் (!mch_isFullName(buffer) && gui_find_bitmap(buffer, buf, ext) == OK)
	STRCPY(buffer, buf);
}
# endif
#endif

#எனில் defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)|| defined(FEAT_GUI_HAIKU)
    வெறுமை
display_errors(வெறுமை)
{
    char_u	*p;

    எனில் (isatty(2))
    {
	fflush(stderr);
	திரும்பு;
    }

    எனில் (error_ga.ga_data == வெற்று)
	திரும்பு;

    // avoid putting up a message box with blanks only
    க்கு (p = (char_u *)error_ga.ga_data; *p != NUL; ++p)
	எனில் (!SAFE_isspace(*p))
	{
	    // Truncate a very long message, it will go off-screen.
	    எனில் (STRLEN(p) > 2000)
		STRCPY(p + 2000 - 14, "...(truncated)");
	    (வெறுமை)do_dialog(VIM_ERROR, (char_u *)_("Error"),
		    p, (char_u *)_("&Ok"), 1, வெற்று, FALSE);
	    நிறுத்து;
	}
    ga_clear(&error_ga);
}
#endif

#எனில் defined(NO_CONSOLE_உள்ளீடு)
/*
 * Return TRUE if still starting up and there is no place to enter text.
 * For GTK and X11 we check if stderr is not a tty, which means we were
 * (probably) started from the desktop.  Also check stdin, "vim >& file" does
 * allow typing on stdin.
 */
    முழு
no_console_உள்ளீடு(வெறுமை)
{
    திரும்பு ((!gui.in_use || gui.starting)
# ifndef NO_CONSOLE
	    && !isatty(0) && !isatty(2)
# endif
	    );
}
#endif

#எனில் defined(FIND_REPLACE_DIALOG) \
	|| defined(NEED_GUI_UPDATE_SCREEN)
/*
 * Update the current window and the screen.
 */
    வெறுமை
gui_update_screen(வெறுமை)
{
# ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_வரி = 0;
    linenr_T	conceal_new_cursor_வரி = 0;
    முழு		conceal_update_வரிகள் = FALSE;
# endif

    update_topline();
    validate_cursor();

    // Trigger CursorMoved if the cursor moved.
    எனில் (!finish_op && (has_cursormoved()
# ifdef FEAT_PROP_POPUP
		|| popup_visible
# endif
# ifdef FEAT_CONCEAL
		|| curwin->w_p_cole > 0
# endif
		) && !EQUAL_POS(last_cursormoved, curwin->w_cursor))
    {
	எனில் (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, வெற்று, வெற்று, FALSE, curbuf);
# ifdef FEAT_PROP_POPUP
	எனில் (popup_visible)
	    popup_check_cursor_pos();
# endif
# ifdef FEAT_CONCEAL
	எனில் (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_வரி = last_cursormoved.lnum;
	    conceal_new_cursor_வரி = curwin->w_cursor.lnum;
	    conceal_update_வரிகள் = TRUE;
	}
# endif
	last_cursormoved = curwin->w_cursor;
    }

    எனில் (!finish_op)
	may_trigger_win_scrolled_resized();

# ifdef FEAT_CONCEAL
    எனில் (conceal_update_வரிகள்
	    && (conceal_old_cursor_வரி != conceal_new_cursor_வரி
		|| conceal_cursor_வரி(curwin)
		|| need_cursor_வரி_redraw))
    {
	எனில் (conceal_old_cursor_வரி != conceal_new_cursor_வரி)
	    redrawWinline(curwin, conceal_old_cursor_வரி);
	redrawWinline(curwin, conceal_new_cursor_வரி);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_வரி_redraw = FALSE;
    }
# endif
    update_screen(0);	// may need to update the screen
    setcursor();
    out_flush_cursor(TRUE, FALSE);
}
#endif

#எனில் defined(FIND_REPLACE_DIALOG)
/*
 * Get the text to use in a find/replace dialog.  Uses the last search pattern
 * if the argument is empty.
 * Returns an allocated string.
 */
    char_u *
get_find_dialog_text(
    char_u	*arg,
    முழு		*wwordp,	// return: TRUE if \< \> found
    முழு		*mcasep)	// return: TRUE if \C found
{
    char_u	*text;

    எனில் (*arg == NUL)
	text = last_தேடல்_pat();
    இல்லையெனில்
	text = arg;
    எனில் (text != வெற்று)
    {
	முழு len = (முழு)STRLEN(text);

	text = vim_strnsave(text, len);
	எனில் (text != வெற்று)
	{
	    முழு i;

	    // Remove "\V"
	    எனில் (len >= 2 && STRNCMP(text, "\\V", 2) == 0)
	    {
		mch_memmove(text, text + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\c" and "\C" and remove.
	    எனில் (len >= 2 && *text == '\\' && (text[1] == 'c' || text[1] == 'C'))
	    {
		*mcasep = (text[1] == 'C');
		mch_memmove(text, text + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\<text\>" and remove.
	    எனில் (len >= 4
		    && STRNCMP(text, "\\<", 2) == 0
		    && STRNCMP(text + len - 2, "\\>", 2) == 0)
	    {
		*wwordp = TRUE;
		mch_memmove(text, text + 2, (size_t)(len - 4));
		text[len - 4] = NUL;
	    }

	    // Recognize "\/" or "\?" and remove.
	    க்கு (i = 0; i + 1 < len; ++i)
		எனில் (text[i] == '\\' && (text[i + 1] == '/'
						       || text[i + 1] == '?'))
		{
		    mch_memmove(text + i, text + i + 1, (size_t)(len - i));
		    --len;
		}
	}
    }
    திரும்பு text;
}

/*
 * Handle the press of a button in the find-replace dialog.
 * Return TRUE when something was added to the input buffer.
 */
    முழு
gui_do_findrepl(
    முழு		flags,		// one of FRD_REPLACE, FRD_FINDNEXT, etc.
    char_u	*find_text,
    char_u	*repl_text,
    முழு		down)		// Search downwards.
{
    garray_T	ga;
    முழு		i;
    முழு		type = (flags & FRD_TYPE_MASK);
    char_u	*p;
    regmatch_T	regmatch;
    முழு		save_did_emsg = did_emsg;
    நிலையான முழு  busy = FALSE;

    // When the screen is being updated we should not change buffers and
    // windows structures, it may cause freed memory to be used.  Also don't
    // do this recursively (pressing "Find" quickly several times).
    எனில் (updating_screen || busy)
	திரும்பு FALSE;

    // refuse replace when text cannot be changed
    எனில் ((type == FRD_REPLACE || type == FRD_REPLACEALL) && text_locked())
	திரும்பு FALSE;

    busy = TRUE;

    ga_init2(&ga, 1, 100);
    எனில் (type == FRD_REPLACEALL)
	GA_CONCAT_LITERAL(&ga, "%s/");

    GA_CONCAT_LITERAL(&ga, "\\V");
    எனில் (flags & FRD_MATCH_CASE)
	GA_CONCAT_LITERAL(&ga, "\\C");
    இல்லையெனில்
	GA_CONCAT_LITERAL(&ga, "\\c");
    எனில் (flags & FRD_WHOLE_WORD)
	GA_CONCAT_LITERAL(&ga, "\\<");
    // escape slash and backslash
    p = vim_strsave_escaped(find_text, (char_u *)"/\\");
    எனில் (p != வெற்று)
	ga_concat(&ga, p);
    vim_free(p);
    எனில் (flags & FRD_WHOLE_WORD)
	GA_CONCAT_LITERAL(&ga, "\\>");

    எனில் (type == FRD_REPLACEALL)
    {
	GA_CONCAT_LITERAL(&ga, "/");
	// Escape slash and backslash.
	// Also escape tilde and ampersand if 'magic' is set.
	p = vim_strsave_escaped(repl_text,
				p_magic ? (char_u *)"/\\~&" : (char_u *)"/\\");
	எனில் (p != வெற்று)
	    ga_concat(&ga, p);
	vim_free(p);
	GA_CONCAT_LITERAL(&ga, "/g");
    }
    ga_append(&ga, NUL);

    எனில் (type == FRD_REPLACE)
    {
	// Do the replacement when the text at the cursor matches.  Thus no
	// replacement is done if the cursor was moved!
	regmatch.regprog = vim_regcomp(ga.ga_data, RE_MAGIC + RE_STRING);
	regmatch.rm_ic = 0;
	எனில் (regmatch.regprog != வெற்று)
	{
	    p = ml_get_cursor();
	    எனில் (vim_regexec_nl(&regmatch, p, (colnr_T)0)
						   && regmatch.startp[0] == p)
	    {
		// Clear the command line to remove any old "No match"
		// error.
		செய்தி_end_prompt();

		எனில் (u_save_cursor() == OK)
		{
		    // A button was pressed thus undo should be synced.
		    u_sync(FALSE);

		    del_bytes((long)(regmatch.endp[0] - regmatch.startp[0]),
								FALSE, FALSE);
		    ins_str(repl_text, STRLEN(repl_text));
		}
	    }
	    இல்லையெனில்
		செய்தி(_("No match at cursor, finding next"));
	    vim_regfree(regmatch.regprog);
	}
    }

    எனில் (type == FRD_REPLACEALL)
    {
	// A button was pressed, thus undo should be synced.
	u_sync(FALSE);
	do_cmdline_cmd(ga.ga_data);
    }
    இல்லையெனில்
    {
	முழு searchflags = தேடல்_செய்தி + தேடல்_MARK;

	// Search for the next match.
	// Don't skip text under cursor for single replace.
	எனில் (type == FRD_REPLACE)
	    searchflags += தேடல்_START;
	i = செய்தி_scroll;
	எனில் (down)
	{
	    (வெறுமை)do_தேடல்(வெற்று, '/', '/', ga.ga_data, STRLEN(ga.ga_data), 1L, searchflags, வெற்று);
	}
	இல்லையெனில்
	{
	    // We need to escape '?' if and only if we are searching in the up
	    // direction
	    p = vim_strsave_escaped(ga.ga_data, (char_u *)"?");
	    எனில் (p != வெற்று)
		(வெறுமை)do_தேடல்(வெற்று, '?', '?', p, STRLEN(p), 1L, searchflags, வெற்று);
	    vim_free(p);
	}

	செய்தி_scroll = i;	    // don't let an error message set msg_scroll
    }

    // Don't want to pass did_emsg to other code, it may cause disabling
    // syntax HL if we were busy redrawing.
    did_emsg = save_did_emsg;

    எனில் (State & (MODE_NORMAL | MODE_செருகு))
    {
	gui_update_screen();		// update the screen
	செய்தி_didout = 0;			// overwrite any message
	need_wait_return = FALSE;	// don't wait for return
    }

    vim_free(ga.ga_data);
    busy = FALSE;
    திரும்பு (ga.ga_len > 0);
}

#endif

#எனில் defined(HAVE_DROP_FILE)
/*
 * Jump to the window at specified point (x, y).
 */
    நிலையான வெறுமை
gui_wingoto_xy(முழு x, முழு y)
{
    முழு		row = Y_2_ROW(y);
    முழு		col = X_2_COL(x);
    win_T	*wp;

    எனில் (row < 0 || col < 0)
	திரும்பு;

    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    எனில் (wp != வெற்று && wp != curwin)
	win_goto(wp);
}

/*
 * Function passed to handle_drop() for the actions to be done after the
 * argument list has been updated.
 */
    நிலையான வெறுமை
drop_callback(வெறுமை *cookie)
{
    char_u	*p = cookie;
    முழு		do_shorten = FALSE;

    // If Shift held down, change to first file's directory.  If the first
    // item is a directory, change to that directory (and let the explorer
    // plugin show the contents).
    எனில் (p != வெற்று)
    {
	எனில் (mch_isdir(p))
	{
	    எனில் (mch_chdir((எழுத்து *)p) == 0)
		do_shorten = TRUE;
	}
	இல்லையெனில் எனில் (vim_chdirfile(p, "drop") == OK)
	    do_shorten = TRUE;
	vim_free(p);
	எனில் (do_shorten)
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
    வெறுமை
gui_handle_drop(
    முழு		x UNUSED,
    முழு		y UNUSED,
    int_u	modifiers,
    char_u	**fnames,
    முழு		மொத்தம்)
{
    முழு		i;
    char_u	*p;
    நிலையான முழு	entered = FALSE;

    /*
     * This function is called by event handlers.  Just in case we get a
     * second event before the first one is handled, ignore the second one.
     * Not sure if this can ever happen, just in case.
     */
    எனில் (entered)
	திரும்பு;
    entered = TRUE;

    /*
     * When the cursor is at the command line, add the file names to the
     * command line, don't edit the files.
     */
    எனில் (State & MODE_CMDLINE)
    {
	shorten_filenames(fnames, மொத்தம்);
	க்கு (i = 0; i < மொத்தம்; ++i)
	{
	    எனில் (fnames[i] != வெற்று)
	    {
		எனில் (i > 0)
		    add_to_உள்ளீடு_buf((char_u*)" ", 1);

		// We don't know what command is used thus we can't be sure
		// about which characters need to be escaped.  Only escape the
		// most common ones.
# ifdef BACKSLASH_IN_FILENAME
		p = vim_strsave_escaped(fnames[i], (char_u *)" \t\"|");
# இல்லையெனில்
		p = vim_strsave_escaped(fnames[i], (char_u *)"\\ \t\"|");
# endif
		எனில் (p != வெற்று)
		    add_to_உள்ளீடு_buf_csi(p, (முழு)STRLEN(p));
		vim_free(p);
		vim_free(fnames[i]);
	    }
	}
	vim_free(fnames);
    }
    இல்லையெனில்
    {
	// Go to the window under mouse cursor, then shorten given "fnames" by
	// current window, because a window can have local current dir.
	gui_wingoto_xy(x, y);
	shorten_filenames(fnames, மொத்தம்);

	// If Shift held down, remember the first item.
	எனில் ((modifiers & MOUSE_SHIFT) != 0)
	    p = vim_strsave(fnames[0]);
	இல்லையெனில்
	    p = வெற்று;

	// Handle the drop, :edit or :split to get to the file.  This also
	// frees fnames[].  Skip this if there is only one item, it's a
	// directory and Shift is held down.
	எனில் (மொத்தம் == 1 && (modifiers & MOUSE_SHIFT) != 0
						     && mch_isdir(fnames[0]))
	{
	    vim_free(fnames[0]);
	    vim_free(fnames);
	    vim_free(p);
	}
	இல்லையெனில்
	    handle_drop(மொத்தம், fnames, (modifiers & MOUSE_CTRL) != 0,
						     drop_callback, (வெறுமை *)p);
    }

    entered = FALSE;
}
#endif

/*
 * Check if "key" is to interrupt us.  Handles a key that has not had modifiers
 * applied yet.
 * Return the key with modifiers applied if so, NUL if not.
 */
    முழு
check_for_interrupt(முழு சாவி, முழு modifiers_arg)
{
    முழு modifiers = modifiers_arg;
    முழு c = merge_modify_Other_சாவிகள்(சாவி, &modifiers);

    எனில் ((c == Ctrl_C && ctrl_c_interrupts)
#ifdef UNIX
	    || (intr_char != Ctrl_C && c == intr_char)
#endif
	    )
    {
	got_int = TRUE;
	திரும்பு c;
    }
    திரும்பு NUL;
}

/*
 * If the "--gui-log-file fname" argument is given write the dialog title and
 * message to a file and return TRUE.  Otherwise return FALSE.
 * When there is any problem opening the file or writing to the file this is
 * ignored, showing the dialog might get the test to get stuck.
 */
    முழு
gui_dialog_log(char_u *title, char_u *message)
{
    char_u  *fname = get_gui_dialog_file();
    FILE    *fd;

    எனில் (fname == வெற்று)
	திரும்பு FALSE;

    fd = mch_fopen((எழுத்து *)fname, "a");
    எனில் (fd != வெற்று)
    {
	fprintf(fd, "%s: %s\n", title, message);
	fclose(fd);
    }
    திரும்பு TRUE;
}
