/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#شامل "vim.h"

// Structure containing all the GUI information
gui_T gui;

#اگر defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
# define USE_SET_GUIFONTWIDE
ایستا تهی set_guifontwide(char_u *font_name);
#endif
ایستا تهی gui_check_pos(تهی);
ایستا تهی gui_reset_scroll_region(تهی);
ایستا تهی gui_outstr(char_u *, صحیح);
#ifndef USE_GTK4
ایستا صحیح gui_screenchar(صحیح off, صحیح flags, guicolor_T fg, guicolor_T bg, صحیح back);
#endif
ایستا صحیح gui_outstr_nowrap(char_u *s, صحیح len, صحیح flags, guicolor_T fg, guicolor_T bg, صحیح back);
ایستا تهی gui_delete_سطرها(صحیح row, صحیح شمار);
ایستا تهی gui_درج_سطرها(صحیح row, صحیح شمار);
ایستا صحیح gui_xy2colrow(صحیح x, صحیح y, صحیح *colp);
#اگر defined(FEAT_GUI_TABLINE)
ایستا صحیح gui_has_tabline(تهی);
#endif
ایستا تهی gui_do_scrollbar(win_T *wp, صحیح which, صحیح enable);
ایستا تهی gui_update_horiz_scrollbar(صحیح);
ایستا تهی gui_set_fg_color(char_u *name);
ایستا تهی gui_set_bg_color(char_u *name);
ایستا تهی init_gui_options(تهی);
ایستا win_T *xy2win(صحیح x, صحیح y, mouse_find_T popup);

#ifdef GUI_MAY_FORK
ایستا تهی gui_do_fork(تهی);

ایستا صحیح gui_read_child_pipe(صحیح fd);

// Return values for gui_read_child_pipe
شمارش {
    GUI_CHILD_IO_ERROR,
    GUI_CHILD_OK,
    GUI_CHILD_FAILED
};
#endif

ایستا تهی gui_attempt_start(تهی);

ایستا صحیح can_update_cursor = TRUE; // can display the cursor
ایستا صحیح disable_flush = 0;	// If > 0, gui_mch_flush() is disabled.

/*
 * gui_start -- Called when user wants to start the GUI.
 *
 * Careful: This function can be called recursively when there is a ":gui"
 * command in the .gvimrc file.  Only the first call should fork, not the
 * recursive call.
 */
    تهی
gui_start(char_u *arg UNUSED)
{
    char_u	*old_term;
#ifdef GUI_MAY_FORK
    ایستا صحیح	recursive = 0;
#endif
#اگر defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
    نویسه	*پیام = پوچ;
#endif

    old_term = vim_strsave(T_NAME);

    settmode(TMODE_COOK);		// stop RAW mode
    اگر (full_screen)
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
    اگر (gui.dofork && !vim_strchr(p_go, GO_FORG) && recursive <= 1
# ifdef FEAT_JOB_CHANNEL
	    && !job_any_running()
# endif
	    )
    {
	gui_do_fork();
    }
    وگرنه
#endif
#ifdef GUI_MAY_SPAWN
    اگر (gui.dospawn
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
	پیام =
# endif
	    gui_mch_do_spawn(arg);
    }
    وگرنه
#endif
    {
#ifdef FEAT_GUI_GTK
	// If there is 'f' in 'guioptions' and specify -g argument,
	// gui_mch_init_check() was not called yet.
	اگر (gui_mch_init_check() != OK)
	    getout_preserve_modified(1);
#endif
	gui_attempt_start();
    }

    اگر (!gui.in_use)			// failed to start GUI
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
#اگر defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
	اگر (پیام != پوچ)
	    emsg(پیام);
#endif
    }
    وگرنه
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

#اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    // Enable fullscreen mode
    اگر (vim_strchr(p_go, GO_FULLSCREEN) != پوچ)
       gui_mch_set_fullscreen(TRUE);
#endif

    vim_free(old_term);

    // If the GUI started successfully, trigger the GUIEnter event, otherwise
    // the GUIFailed event.
    gui_mch_update();
    apply_autocmds(gui.in_use ? EVENT_GUIENTER : EVENT_GUIFAILED,
						   پوچ, پوچ, FALSE, curbuf);
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
    ایستا تهی
gui_attempt_start(تهی)
{
    ایستا صحیح recursive = 0;

    ++recursive;
    gui.starting = true;

#ifdef FEAT_GUI_GTK
    gui.event_time = GDK_CURRENT_TIME;
#endif

    termcapinit((char_u *)"builtin_gui");
    gui.starting = recursive - 1;

#اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
    اگر (gui.in_use)
    {
# ifdef FEAT_EVAL
	Window	x11_window;
	Display	*x11_display;

	اگر (gui_get_x11_windis(&x11_window, &x11_display) == OK)
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
# اگر defined(HAVE_SYS_WAIT_H)
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
    ایستا تهی
gui_do_fork(تهی)
{
    صحیح		pipefd[2];	// pipe between parent and child
    صحیح		pipe_error;
    صحیح		status;
    صحیح		exit_status;
    pid_t	pid = -1;

# اگر defined(FEAT_RELTIME) && defined(PROF_NSEC)
    // a timer is not carried forward
    delete_timer();
# endif

    // Setup a pipe between the child and the parent, so that the parent
    // knows when the child has done the setsid() call and is allowed to
    // exit.
    pipe_error = (pipe(pipefd) < 0);
    pid = fork();
    اگر (pid < 0)	    // Fork error
    {
	emsg(_(e_failed_to_create_new_process_for_GUI));
	بازگشت;
    }
    وگرنه اگر (pid > 0)	    // Parent
    {
	// Give the child some time to do the setsid(), otherwise the
	// exit() may kill the child too (when starting gvim from inside a
	// gvim).
	اگر (!pipe_error)
	{
	    // The read returns when the child closes the pipe (or when
	    // the child dies for some reason).
	    close(pipefd[1]);
	    status = gui_read_child_pipe(pipefd[0]);
	    اگر (status == GUI_CHILD_FAILED)
	    {
		// The child failed to start the GUI, so the caller must
		// continue. There may be more error information written
		// to stderr by the child.
		waitpid(pid, &exit_status, 0);
		emsg(_(e_the_child_process_failed_to_start_GUI));
		بازگشت;
	    }
	    وگرنه اگر (status == GUI_CHILD_IO_ERROR)
	    {
		pipe_error = TRUE;
	    }
	    // else GUI_CHILD_OK: parent exit
	}

	اگر (pipe_error)
	    ui_delay(301L, TRUE);

	// When swapping screens we may need to go to the next line, e.g.,
	// after a hit-enter prompt and using ":gui".
	اگر (newline_on_exit)
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
    اگر (gui_mch_init_check() != OK)
	getout_preserve_modified(1);
# endif

# اگر defined(HAVE_SETSID) || defined(HAVE_SETPGID)
    /*
     * Change our process group.  On some systems/shells a CTRL-C in the
     * shell where Vim was started would otherwise kill gvim!
     */
#  اگر defined(HAVE_SETSID)
    (تهی)setsid();
#  وگرنه
    (تهی)setpgid(0, 0);
#  endif
# endif
    اگر (!pipe_error)
	close(pipefd[0]);

# اگر defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
    // Tell the session manager our new PID
    gui_mch_forked();
# endif

    // Try to start the GUI
    gui_attempt_start();

    // Notify the parent
    اگر (!pipe_error)
    {
	اگر (gui.in_use)
	    write_eintr(pipefd[1], "ok", 3);
	وگرنه
	    write_eintr(pipefd[1], "fail", 5);
	close(pipefd[1]);
    }

    // If we failed to start the GUI, exit now.
    اگر (!gui.in_use)
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
    ایستا صحیح
gui_read_child_pipe(صحیح fd)
{
    long	bytes_read;
# define READ_BUFFER_SIZE 10
    نویسه	buffer[READ_BUFFER_SIZE];

    bytes_read = read_eintr(fd, buffer, READ_BUFFER_SIZE - 1);
# undef READ_BUFFER_SIZE
    close(fd);
    اگر (bytes_read < 0)
	بازگشت GUI_CHILD_IO_ERROR;
    buffer[bytes_read] = NUL;
    اگر (STRCMP(buffer, "ok") == 0)
	بازگشت GUI_CHILD_OK;
    بازگشت GUI_CHILD_FAILED;
}

#endif // GUI_MAY_FORK

/*
 * Call this when vim starts up, whether or not the GUI is started
 */
    تهی
gui_prepare(صحیح *argc, نویسه **argv)
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
    صحیح
gui_init_check(تهی)
{
    ایستا صحیح result = MAYBE;

    اگر (result != MAYBE)
    {
	اگر (result == FAIL)
	    emsg(_(e_cannot_start_the_GUI));
	بازگشت result;
    }

    gui.shell_created = false;
    gui.dying = false;
    gui.in_focus = true;		// so the guicursor setting works
    gui.dragged_sb = SBAR_NONE;
    gui.dragged_wp = پوچ;
    gui.pointer_hidden = false;
    gui.col = 0;
    gui.row = 0;
    gui.رقم_cols = Columns;
    gui.رقم_rows = Rows;

    gui.cursor_is_valid = false;
    gui.scroll_region_بالاترین = 0;
    gui.scroll_region_bot = Rows - 1;
    gui.scroll_region_چپ = 0;
    gui.scroll_region_راست = Columns - 1;
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
#  وگرنه
    gui.menu_font = NOFONT;
#  endif
# endif
    gui.menu_is_active = true;	    // default: include menu
# ifndef FEAT_GUI_GTK
    gui.menu_height = MENU_DEFAULT_HEIGHT;
    gui.menu_width = 0;
# endif
#endif
#اگر defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    gui.toolbar_height = 0;
#endif
#ifdef FEAT_BEVAL_TIP
    gui.tooltip_fontset = NOFONTSET;
#endif

    gui.scrollbar_width = gui.scrollbar_height = SB_DEFAULT_WIDTH;
    gui.prev_wrap = -1;

#اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // Note: gui_set_ligatures() might already have been called e.g. from .vimrc,
    // and in that case we don't want to overwrite ligatures map that has already
    // been correctly populated (as that would lead to a cleared ligatures maps).
    اگر (*p_guiligatures == NUL)
	CLEAR_FIELD(gui.ligatures_map);
#endif

#اگر defined(ALWAYS_USE_GUI) || defined(VIMDLL)
    result = OK;
#وگرنه
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
# وگرنه
    result = gui_mch_init_check();
# endif
#endif
    بازگشت result;
}

/*
 * This is the call which starts the GUI.
 */
    تهی
gui_init(تهی)
{
    win_T	*wp;
    tabpage_T	*tp;
    ایستا صحیح	recursive = 0;

    /*
     * It's possible to use ":gui" in a .gvimrc file.  The first half of this
     * function will then be executed at the first call, the rest by the
     * recursive call.  This allow the shell to be opened halfway reading a
     * gvimrc file.
     */
    اگر (!recursive)
    {
	++recursive;

	clip_init(TRUE);

	// If can't initialize, don't try doing the rest
	اگر (gui_init_check() == FAIL)
	{
	    --recursive;
	    clip_init(FALSE);
	    بازگشت;
	}

	/*
	 * Reset 'paste'.  It's useful in the terminal, but not in the GUI.  It
	 * breaks the Paste toolbar button.
	 */
	set_option_مقدار_give_err((char_u *)"paste", 0L, پوچ, 0);

	// Set t_Co to the number of colors: RGB.
	set_color_شمار(256 * 256 * 256);

	/*
	 * Set up system-wide default menus.
	 */
#اگر defined(SYS_MENU_FILE) && defined(FEAT_MENU)
	اگر (vim_strchr(p_go, GO_NOSYSMENU) == پوچ)
	{
	    sys_menu = TRUE;
	    do_source((char_u *)SYS_MENU_FILE, FALSE, DOSO_NONE, پوچ);
	    sys_menu = FALSE;
	}
#endif

	/*
	 * Switch on the mouse by default, unless the user changed it already.
	 * This can then be changed in the .gvimrc.
	 */
	اگر (!option_was_set((char_u *)"mouse"))
	    set_string_option_direct((char_u *)"mouse", -1,
					   (char_u *)"a", OPT_FREE, SID_NONE);

	/*
	 * If -U option given, use only the initializations from that file and
	 * nothing else.  Skip all initializations for "-U NONE" or "-u NORC".
	 */
	اگر (use_gvimrc != پوچ)
	{
	    اگر (STRCMP(use_gvimrc, "NONE") != 0
		    && STRCMP(use_gvimrc, "NORC") != 0
		    && do_source(use_gvimrc, FALSE, DOSO_NONE, پوچ) != OK)
		semsg(_(e_cannot_read_from_str), use_gvimrc);
	}
	وگرنه
	{
	    /*
	     * Get system wide defaults for gvim, only when file name defined.
	     */
#ifdef SYS_GVIMRC_FILE
	    do_source((char_u *)SYS_GVIMRC_FILE, FALSE, DOSO_NONE, پوچ);
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
	    اگر (process_env((char_u *)"GVIMINIT", FALSE) == FAIL
		 && do_source((char_u *)USR_GVIMRC_FILE, TRUE,
						     DOSO_GVIMRC, پوچ) == FAIL
#ifdef USR_GVIMRC_FILE2
		 && do_source((char_u *)USR_GVIMRC_FILE2, TRUE,
						     DOSO_GVIMRC, پوچ) == FAIL
#endif
#ifdef USR_GVIMRC_FILE3
		 && do_source((char_u *)USR_GVIMRC_FILE3, TRUE,
						     DOSO_GVIMRC, پوچ) == FAIL
#endif
				)
	    {
#ifdef USR_GVIMRC_FILE4
		(تهی)do_source((char_u *)USR_GVIMRC_FILE4, TRUE,
							    DOSO_GVIMRC, پوچ);
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
	    اگر (p_exrc)
	    {
#ifdef UNIX
		{
		    stat_T s;

		    // if ".gvimrc" file is not owned by user, set 'secure'
		    // mode
		    اگر (mch_stat(GVIMRC_FILE, &s) || s.st_uid != getuid())
			secure = p_secure;
		}
#وگرنه
		secure = p_secure;
#endif

		اگر (       fullpathcmp((char_u *)USR_GVIMRC_FILE,
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
		    do_source((char_u *)GVIMRC_FILE, TRUE, DOSO_GVIMRC, پوچ);

		اگر (secure == 2)
		    need_wait_return = TRUE;
		secure = 0;
	    }
	}

	اگر (need_wait_return || پیام_didany)
	    wait_return(TRUE);

	--recursive;
    }

    // If recursive call opened the shell, return here from the first call
    اگر (gui.in_use)
	بازگشت;

    /*
     * Create the GUI shell.
     */
    gui.in_use = true;		// Must be set after menus have been set up
#اگر defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
    gui.scale = 1.0; // Default value
#endif
    اگر (gui_mch_init() == FAIL)
	برو error;

    // Avoid a delay for an error message that was printed in the terminal
    // where Vim was started.
    emsg_on_display = FALSE;
    پیام_scrolled = 0;
    clear_sb_text(TRUE);
    need_wait_return = FALSE;
    پیام_didany = FALSE;

    /*
     * Check validity of any generic resources that may have been loaded.
     */
    اگر (gui.border_width < 0)
	gui.border_width = 0;

    /*
     * Set up the fonts.  First use a font specified with "-fn" or "-font".
     */
    اگر (font_argument != پوچ)
	set_option_مقدار_give_err((char_u *)"gfn",
					       0L, (char_u *)font_argument, 0);
    اگر (
#ifdef FEAT_XFONTSET
	    (*p_guifontset == NUL
	     || gui_init_font(p_guifontset, TRUE) == FAIL) &&
#endif
	    gui_init_font(*p_guifont == NUL ? hl_get_font_name()
						  : p_guifont, FALSE) == FAIL)
    {
	emsg(_(e_cannot_start_gui_no_valid_font_found));
	برو error2;
    }
    اگر (gui_get_wide_font() == FAIL)
	emsg(_(e_guifontwide_invalid));

    gui.رقم_cols = Columns;
    gui.رقم_rows = Rows;
    gui_reset_scroll_region();

    // Create initial scrollbars
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_چپ], SBAR_چپ, wp);
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_راست], SBAR_راست, wp);
    }
    gui_create_scrollbar(&gui.bottom_sbar, SBAR_BOTTOM, پوچ);

#ifdef FEAT_MENU
    gui_create_initial_menus(ریشه_menu);
#endif
#ifdef FEAT_SIGN_ICONS
    sign_gui_started();
#endif

    // Configure the desired menu and scrollbars
    gui_init_which_components(پوچ);

    // All components of the GUI have been created now
    gui.shell_created = true;

#ifdef FEAT_GUI_MSWIN
    // Set the shell size, adjusted for the screen size.  For GTK this only
    // works after the shell has been opened, thus it is further down.
    // If the window is already maximized (e.g. when --windowid is passed in),
    // we want to use the system-provided dimensions by passing FALSE to
    // mustset. Otherwise, we want to initialize with the default rows/columns.
    اگر (gui_mch_maximized())
	gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    وگرنه
	gui_set_shellsize(TRUE, TRUE, RESIZE_BOTH);
#وگرنه
# ifndef FEAT_GUI_GTK
    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
# endif
#endif
#اگر defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    // Need to set the size of the menubar after all the menus have been
    // created.
    gui_mch_compute_menu_height((Widget)0);
#endif

    /*
     * Actually open the GUI shell.
     */
    اگر (gui_mch_open() != FAIL)
    {
	maketitle();
	resettitle();

	init_gui_options();
#ifdef FEAT_ARABIC
	// Our GUI can't do bidi.
	p_tbidi = FALSE;
#endif
#اگر defined(FEAT_GUI_GTK)
	// Give GTK+ a chance to put all widget's into place.
	gui_mch_update();

# ifdef FEAT_MENU
	// If there is no 'm' in 'guioptions' we need to remove the menu now.
	// It was still there to make F10 work.
	اگر (vim_strchr(p_go, GO_MENUS) == پوچ)
	{
	    bool save_starting = gui.starting;

	    gui.starting = false;
	    gui_mch_enable_menu(FALSE);
	    gui.starting = save_starting;
	    gui_mch_update();
	}
# endif

	// Now make sure the shell fits on the screen.
	اگر (gui_mch_maximized())
	    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
	وگرنه
	    gui_set_shellsize(TRUE, TRUE, RESIZE_BOTH);
#endif
	// When 'lines' was set while starting up the topframe may have to be
	// resized.
	win_new_shellsize();

#ifdef FEAT_BEVAL_GUI
	// Always create the Balloon Evaluation area, but disable it when
	// 'ballooneval' is off.
	اگر (balloonEval != پوچ)
	{
# ifdef FEAT_VARTABS
	    vim_free(balloonEval->vts);
# endif
	    vim_free(balloonEval);
	}
	balloonEvalForTerm = FALSE;
# ifdef FEAT_GUI_GTK
	balloonEval = gui_mch_create_beval_area(gui.drawarea, پوچ,
						     &general_beval_cb, پوچ);
# وگرنه
#  اگر defined(FEAT_GUI_MOTIF)
	{
	    بیرونی Widget	textArea;
	    balloonEval = gui_mch_create_beval_area(textArea, پوچ,
						     &general_beval_cb, پوچ);
	}
#  وگرنه
#   ifdef FEAT_GUI_MSWIN
	balloonEval = gui_mch_create_beval_area(پوچ, پوچ,
						     &general_beval_cb, پوچ);
#   endif
#  endif
# endif
	اگر (!p_beval)
	    gui_mch_disable_beval_area(balloonEval);
#endif

#ifndef FEAT_GUI_MSWIN
	// In the GUI modifiers are prepended to keys.
	// Don't do this for MS-Windows yet, it sends CTRL-K without the
	// modifier.
	seen_Modify_Other_کلیدها = TRUE;
#endif

#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	اگر (!im_xim_isvalid_imactivate())
	    emsg(_(e_مقدار_of_imactivatekey_is_invalid));
#endif
	// When 'cmdheight' was set during startup it may not have taken
	// effect yet.
	اگر (p_ch != 1L)
	    command_height();

	بازگشت;
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


    تهی
gui_exit(صحیح rc)
{
    // don't free the fonts, it leads to a BUS error
    // richard@whitequeen.com Jul 99
    free_highlight_fonts();
    gui.in_use = false;
    gui_mch_exit(rc);
}

#اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_PHOTON)
# define NEED_GUI_UPDATE_SCREEN 1
/*
 * Called when the GUI shell is closed by the user.  If there are no changed
 * files Vim exits, otherwise there will be a dialog to ask the user what to
 * do.
 * When this function returns, Vim should NOT exit!
 */
    تهی
gui_shell_closed(تهی)
{
    cmdmod_T	    save_cmdmod = cmdmod;

    اگر (before_quit_autocmds(curwin, TRUE, FALSE))
	بازگشت;

    // Only exit when there are no changed files
    exiting = TRUE;
# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    cmdmod.cmod_flags |= CMOD_CONFIRM;
# endif
    // If there are changed buffers, present the user with a dialog if
    // possible, otherwise give an error message.
    اگر (!check_changed_any(FALSE, FALSE))
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
    صحیح
gui_init_font(char_u *font_list, صحیح fontset UNUSED)
{
#define FONTLEN 320
    char_u	font_name[FONTLEN];
    صحیح		font_list_empty = FALSE;
    صحیح		ret = FAIL;

    اگر (!gui.in_use)
	بازگشت FAIL;

    font_name[0] = NUL;
    اگر (*font_list == NUL)
	font_list_empty = TRUE;
    وگرنه
    {
#ifdef FEAT_XFONTSET
	// When using a fontset, the whole list of fonts is one name.
	اگر (fontset)
	    ret = gui_mch_init_font(font_list, TRUE);
	وگرنه
#endif
	    وقتی (*font_list != NUL)
	    {
		// Isolate one comma separated font name.
		(تهی)copy_option_part(&font_list, font_name, FONTLEN, ",");

		// Careful!!!  The Win32 version of gui_mch_init_font(), when
		// called with "*" will change p_guifont to the selected font
		// name, which frees the old value.  This makes font_list
		// invalid.  Thus when OK is returned here, font_list must no
		// longer be used!
		اگر (gui_mch_init_font(font_name, FALSE) == OK)
		{
#ifdef USE_SET_GUIFONTWIDE
		    // If it's a Unicode font, try setting 'guifontwide' to a
		    // similar double-width font.
		    اگر ((p_guifontwide == پوچ || *p_guifontwide == NUL)
				&& strstr((نویسه *)font_name, "10646") != پوچ)
			set_guifontwide(font_name);
#endif
		    ret = OK;
		    بشکن;
		}
	    }
    }

    اگر (ret != OK
	    && STRCMP(font_list, "*") != 0
	    && (font_list_empty || gui.norm_font == NOFONT))
    {
	/*
	 * Couldn't load any font in 'font_list', keep the current font if
	 * there is one.  If 'font_list' is empty, or if there is no current
	 * font, tell gui_mch_init_font() to try to find a font we can load.
	 */
	ret = gui_mch_init_font(پوچ, FALSE);
    }

    اگر (ret == OK)
    {
#ifndef FEAT_GUI_GTK
	// Set normal font as current font
# ifdef FEAT_XFONTSET
	اگر (gui.fontset != NOFONTSET)
	    gui_mch_set_fontset(gui.fontset);
	وگرنه
# endif
	    gui_mch_set_font(gui.norm_font);
#endif
	gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    }

    بازگشت ret;
}

#ifdef USE_SET_GUIFONTWIDE
/*
 * Try setting 'guifontwide' to a font twice as wide as "name".
 */
    ایستا تهی
set_guifontwide(char_u *name)
{
    صحیح		i = 0;
    char_u	wide_name[FONTLEN + 10]; // room for 2 * width and '*'
    char_u	*wp = پوچ;
    char_u	*p;
    GuiFont	font;

    wp = wide_name;
    برای (p = name; *p != NUL; ++p)
    {
	*wp++ = *p;
	اگر (*p == '-')
	{
	    ++i;
	    اگر (i == 6)		// font type: change "--" to "-*-"
	    {
		اگر (p[1] == '-')
		    *wp++ = '*';
	    }
	    وگرنه اگر (i == 12)	// found the width
	    {
		++p;
		i = getdigits(&p);
		اگر (i != 0)
		{
		    // Double the width specification.
		    sprintf((نویسه *)wp, "%d%s", i * 2, p);
		    font = gui_mch_get_font(wide_name, FALSE);
		    اگر (font != NOFONT)
		    {
			gui_mch_free_font(gui.wide_font);
			gui.wide_font = font;
			set_string_option_direct((char_u *)"gfw", -1,
						      wide_name, OPT_FREE, 0);
		    }
		}
		بشکن;
	    }
	}
    }
}
#endif

/*
 * Get the font for 'guifontwide'.
 * Return FAIL for an invalid font name.
 */
    صحیح
gui_get_wide_font(تهی)
{
    GuiFont	font = NOFONT;
    char_u	font_name[FONTLEN];
    char_u	*p;

    اگر (!gui.in_use)	    // Can't allocate font yet, assume it's OK.
	بازگشت OK;	    // Will give an error message later.

    اگر (p_guifontwide != پوچ && *p_guifontwide != NUL)
    {
	برای (p = p_guifontwide; *p != NUL; )
	{
	    // Isolate one comma separated font name.
	    (تهی)copy_option_part(&p, font_name, FONTLEN, ",");
	    font = gui_mch_get_font(font_name, FALSE);
	    اگر (font != NOFONT)
		بشکن;
	}
	اگر (font == NOFONT)
	    بازگشت FAIL;
    }

    gui_mch_free_font(gui.wide_font);
#ifdef FEAT_GUI_GTK
    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    اگر (font != NOFONT && gui.norm_font != NOFONT
			 && pango_font_description_equal(font, gui.norm_font))
    {
	gui.wide_font = NOFONT;
	gui_mch_free_font(font);
    }
    وگرنه
#endif
	gui.wide_font = font;
#ifdef FEAT_GUI_MSWIN
    gui_mch_wide_font_changed();
#وگرنه
    /*
     * TODO: setup wide_bold_font, wide_ital_font and wide_boldital_font to
     * support those fonts for 'guifontwide'.
     */
#endif
    بازگشت OK;
}

#اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * Set list of ascii characters that combined can create ligature.
 * Store them in char map for quick access from gui_gtk_draw_string.
 */
    تهی
gui_set_ligatures(تهی)
{
    char_u	*p;

    اگر (*p_guiligatures != NUL)
    {
	// check for invalid characters
	برای (p = p_guiligatures; *p != NUL; ++p)
	    اگر (*p < 32 || *p > 127)
	    {
		emsg(_(e_ascii_code_not_in_range));
		بازگشت;
	    }

	// store valid setting into ligatures_map
	CLEAR_FIELD(gui.ligatures_map);
	برای (p = p_guiligatures; *p != NUL; ++p)
	    gui.ligatures_map[*p] = 1;
    }
    وگرنه
	CLEAR_FIELD(gui.ligatures_map);
}

/*
 * Adjust the columns to undraw for when the cursor is on ligatures.
 */
    ایستا تهی
gui_adjust_undraw_cursor_for_ligatures(صحیح *startcol, صحیح *endcol)
{
    صحیح off;

    اگر (Screen_سطرها == پوچ || *p_guiligatures == NUL)
	بازگشت;

    // expand before the cursor for all the chars in gui.ligatures_map
    off = سطر_Offset[gui.cursor_row] + *startcol;
    اگر (gui.ligatures_map[Screen_سطرها[off]])
	وقتی (*startcol > 0 && gui.ligatures_map[Screen_سطرها[--off]])
	    (*startcol)--;

    // expand after the cursor for all the chars in gui.ligatures_map
    off = سطر_Offset[gui.cursor_row] + *endcol;
    اگر (gui.ligatures_map[Screen_سطرها[off]])
	وقتی (*endcol < ((صحیح)screen_Columns - 1)
				      && gui.ligatures_map[Screen_سطرها[++off]])
	   (*endcol)++;
}
#endif

    ایستا تهی
gui_set_cursor(صحیح row, صحیح col)
{
    gui.row = row;
    gui.col = col;
}

/*
 * gui_check_pos - check if the cursor is on the screen.
 */
    ایستا تهی
gui_check_pos(تهی)
{
    اگر (gui.row >= screen_Rows)
	gui.row = screen_Rows - 1;
    اگر (gui.col >= screen_Columns)
	gui.col = screen_Columns - 1;
    اگر (gui.cursor_row >= screen_Rows || gui.cursor_col >= screen_Columns)
	gui.cursor_is_valid = false;
}

/*
 * Redraw the cursor if necessary or when forced.
 * Careful: The contents of ScreenLines[] must match what is on the screen,
 * otherwise this goes wrong.  May need to call out_flush() first.
 */
    تهی
gui_update_cursor(
    صحیح		force,		 // when TRUE, update even when not moved
    صحیح		clear_selection) // clear selection under cursor
{
    صحیح		cur_width = 0;
    صحیح		cur_height = 0;
    صحیح		old_hl_mask;
    cursorentry_T *shape;
    صحیح		id;
#ifdef FEAT_TERMINAL
    guicolor_T	shape_fg = INVALCOLOR;
    guicolor_T	shape_bg = INVALCOLOR;
#endif
    guicolor_T	cfg, cbg, cc;	// cursor fore-/background color
    صحیح		cattr;		// cursor attributes
    صحیح		attr;
    attrentry_T *aep = پوچ;
#اگر (defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)) && !defined(USE_GTK4)
    bool	lig_چپ = false, lig_راست = false;
#endif

    // Don't update the cursor when halfway busy scrolling or the screen size
    // doesn't match 'columns' and 'lines.  ScreenLines[] isn't valid then.
    اگر (!can_update_cursor || screen_Columns != gui.رقم_cols
					       || screen_Rows != gui.رقم_rows)
	بازگشت;

    gui_check_pos();

    اگر (gui.cursor_is_valid && !force
		&& gui.row == gui.cursor_row && gui.col == gui.cursor_col)
	بازگشت;

    gui_undraw_cursor();

    // If a cursor-less sleep is ongoing, leave the cursor invisible
    اگر (cursor_is_sleeping())
	بازگشت;

    اگر (gui.row < 0)
	بازگشت;
#ifdef HAVE_درون_داد_METHOD
    اگر (gui.row != gui.cursor_row || gui.col != gui.cursor_col)
	im_set_position(gui.row, gui.col);
#endif
    gui.cursor_row = gui.row;
    gui.cursor_col = gui.col;

    // Only write to the screen after ScreenLines[] has been initialized
    اگر (!screen_cleared || Screen_سطرها == پوچ)
	بازگشت;

    // Clear the selection if we are about to write over it
    اگر (clear_selection)
	clip_may_clear_selection(gui.row, gui.row);
    // Check that the cursor is inside the shell (resizing may have made
    // it invalid)
    اگر (gui.row >= screen_Rows || gui.col >= screen_Columns)
	بازگشت;

    gui.cursor_is_valid = true;

    /*
     * How the cursor is drawn depends on the current mode.
     * When in a terminal window use the shape/color specified there.
     */
#ifdef FEAT_TERMINAL
    اگر (terminal_is_active())
	shape = term_get_cursor_shape(&shape_fg, &shape_bg);
    وگرنه
#endif
	shape = &shape_table[get_shape_idx(FALSE)];
    اگر (State & MODE_LANGMAP)
	id = shape->id_lm;
    وگرنه
	id = shape->id;

    // get the colors and attributes for the cursor.  Default is inverted
    cfg = INVALCOLOR;
    cbg = INVALCOLOR;
    cattr = HL_INVERSE;
    gui_mch_set_blinking(shape->blinkwait,
	    shape->blinkon,
	    shape->blinkoff);
    اگر (shape->blinkwait == 0 || shape->blinkon == 0
	    || shape->blinkoff == 0)
	gui_mch_stop_blink(FALSE);
#ifdef FEAT_TERMINAL
    اگر (shape_bg != INVALCOLOR)
    {
	cattr = 0;
	cfg = shape_fg;
	cbg = shape_bg;
    }
    وگرنه
#endif
	اگر (id > 0)
	{
	    cattr = syn_id2colors(id, &cfg, &cbg);
#اگر defined(HAVE_درون_داد_METHOD)
	    {
		ایستا صحیح iid;
		guicolor_T fg, bg;

		اگر (
# اگر defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
			preedit_get_status()
# وگرنه
			im_get_status()
# endif
		   )
		{
		    iid = syn_name2id((char_u *)"CursorIM");
		    اگر (iid > 0)
		    {
			syn_id2colors(iid, &fg, &bg);
			اگر (bg != INVALCOLOR)
			    cbg = bg;
			اگر (fg != INVALCOLOR)
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
    attr = ScreenAttrs[سطر_Offset[gui.row] + gui.col];
    اگر (attr > HL_ALL)
	aep = syn_gui_attr2entry(attr);
    اگر (aep != پوچ)
    {
	attr = aep->ae_attr;
	اگر (cfg == INVALCOLOR)
	    cfg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.bg_color
		    : aep->ae_u.gui.fg_color);
	اگر (cbg == INVALCOLOR)
	    cbg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.fg_color
		    : aep->ae_u.gui.bg_color);
    }
    اگر (cfg == INVALCOLOR)
	cfg = (attr & HL_INVERSE) ? gui.back_pixel : gui.norm_pixel;
    اگر (cbg == INVALCOLOR)
	cbg = (attr & HL_INVERSE) ? gui.norm_pixel : gui.back_pixel;

#ifdef FEAT_XIM
    اگر (aep != پوچ)
    {
	xim_bg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.fg_color
		: aep->ae_u.gui.bg_color);
	xim_fg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.bg_color
		: aep->ae_u.gui.fg_color);
	اگر (xim_bg_color == INVALCOLOR)
	    xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
		: gui.back_pixel;
	اگر (xim_fg_color == INVALCOLOR)
	    xim_fg_color = (attr & HL_INVERSE) ? gui.back_pixel
		: gui.norm_pixel;
    }
    وگرنه
    {
	xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
	    : gui.back_pixel;
	xim_fg_color = (attr & HL_INVERSE) ? gui.back_pixel
	    : gui.norm_pixel;
    }
#endif

    attr &= ~HL_INVERSE;
    اگر (cattr & HL_INVERSE)
    {
	cc = cbg;
	cbg = cfg;
	cfg = cc;
    }
    cattr &= ~HL_INVERSE;

    /*
     * When we don't have window focus, draw a hollow cursor.
     */
    اگر (!gui.in_focus)
    {
#ifdef USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, -1, -1);
#وگرنه
	gui_mch_draw_hollow_cursor(cbg);
#endif
	بازگشت;
    }

#ifdef USE_GTK4
    // Make sure that character underneath is drawn again in case it is part of
    // a ligature.
    gui_redraw_block(gui.row, gui.col, gui.row, gui.col, GUI_MON_NOCLEAR);
    // gui_redraw_block() will invalidate the cursor
    gui.cursor_is_valid = true;
#endif
#اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // If cursor is in the middle of a ligature, then split the ligature at
    // the cursor boundaries by redrawing those cells again.
    برای (صحیح c = gui.col - 1; c >= 0; c--)
	اگر (!gui.ligatures_map[Screen_سطرها[سطر_Offset[gui.row] + c]])
	{
	    اگر (c < gui.col - 1)
	    {
		gui_redraw_block(gui.row, c + 1, gui.row, gui.col - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_چپ = true;
# endif
	    }
	    بشکن;
	}

    برای (صحیح c = gui.col + 1; c < screen_Columns - 1; c++)
	اگر (!gui.ligatures_map[Screen_سطرها[سطر_Offset[gui.row] + c]])
	{
	    اگر (c > gui.col + 1)
	    {
		gui_redraw_block(gui.row, gui.col + 1, gui.row, c - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_راست = true;
# endif
	    }
	    بشکن;
	}

# ifndef USE_GTK4
    اگر ((lig_چپ || lig_راست) && shape->shape != SHAPE_BLOCK)
    {
	// Because the cursor is not drawn with gui_screenchar(), must blit the
	// cell again (with its background), so that old ligature does not
	// remain on screen. Not needed for GtkSnapshot, because char beneath is
	// always rerendered (see above ifdef).
	صحیح old = gui.col;
	gui.highlight_mask = ScreenAttrs[سطر_Offset[gui.row] + gui.col];
	(تهی)gui_screenchar(سطر_Offset[gui.row] + gui.col,
		GUI_MON_NOCLEAR, (guicolor_T)0, (guicolor_T)0, 0);
	gui.col = old;
    }
# endif
    // gui_redraw_block()/gui_screenchar() may invalidate the cursor, make sure
    // to validate it again.
    gui.cursor_is_valid = true;
#endif

    old_hl_mask = gui.highlight_mask;
    اگر (shape->shape == SHAPE_BLOCK)
    {
#ifdef USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, 0, 0);
#وگرنه
	/*
	 * Draw the text character with the cursor colors.	Use the
	 * character attributes plus the cursor attributes.
	 */
	gui.highlight_mask = (cattr | attr);
	(تهی)gui_screenchar(سطر_Offset[gui.row] + gui.col,
		GUI_MON_IS_CURSOR | GUI_MON_NOCLEAR, cfg, cbg, 0);
#endif
    }
    وگرنه
    {
#اگر defined(FEAT_RIGHTLEFT)
	صحیح	    col_off = FALSE;
#endif
	/*
	 * First draw the partial cursor, then overwrite with the text
	 * character, using a transparent background.
	 */
	اگر (shape->shape == SHAPE_VER)
	{
	    cur_height = gui.char_height;
	    cur_width = (gui.char_width * shape->percentage + 99) / 100;
	}
	وگرنه
	{
	    cur_height = (gui.char_height * shape->percentage + 99) / 100;
	    cur_width = gui.char_width;
	}
	اگر (has_mbyte && (*mb_off2cells)(سطر_Offset[gui.row] + gui.col,
		    سطر_Offset[gui.row] + screen_Columns) > 1)
	{
	    // Double wide character.
	    اگر (shape->shape != SHAPE_VER)
		cur_width += gui.char_width;
#ifdef FEAT_RIGHTLEFT
	    اگر (CURSOR_BAR_راست)
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
#وگرنه
	gui_mch_draw_part_cursor(cur_width, cur_height, cbg);
#endif
#اگر defined(FEAT_RIGHTLEFT)
	اگر (col_off)
	    --gui.col;
#endif

	// Doesn't seem to work for MSWindows. We call gui_redraw_block() above
	// for GtkSnapshot.
#اگر !defined(FEAT_GUI_MSWIN) && !defined(USE_GTK4)
	gui.highlight_mask = ScreenAttrs[سطر_Offset[gui.row] + gui.col];
	(تهی)gui_screenchar(سطر_Offset[gui.row] + gui.col,
		GUI_MON_TRS_CURSOR | GUI_MON_NOCLEAR,
		(guicolor_T)0, (guicolor_T)0, 0);
#endif
    }
    gui.highlight_mask = old_hl_mask;
}

#اگر defined(FEAT_MENU)
    ایستا تهی
gui_position_menu(تهی)
{
# اگر !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
    اگر (gui.menu_is_active && gui.in_use)
	gui_mch_set_menu_pos(0, 0, gui.menu_width, gui.menu_height);
# endif
}
#endif

/*
 * Position the various GUI components (text area, menu).  The vertical
 * scrollbars are NOT handled here.  See gui_update_scrollbars().
 */
    ایستا تهی
gui_position_components(صحیح total_width UNUSED)
{
    صحیح	    text_area_x;
    صحیح	    text_area_y;
    صحیح	    text_area_width;
    صحیح	    text_area_height;

    // avoid that moving components around generates events
    ++hold_gui_events;

    text_area_x = 0;
    اگر (gui.which_scrollbars[SBAR_چپ])
	text_area_x += gui.scrollbar_width;

    text_area_y = 0;
#اگر defined(FEAT_MENU) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    gui.menu_width = total_width;
    اگر (gui.menu_is_active)
	text_area_y += gui.menu_height;
#endif

#اگر defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
    اگر (gui_has_tabline())
	text_area_y += gui.tabline_height;
#endif

#اگر defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    اگر (vim_strchr(p_go, GO_TOOLBAR) != پوچ)
    {
# اگر defined(FEAT_GUI_HAIKU)
	gui_mch_set_toolbar_pos(0, text_area_y,
				gui.menu_width, gui.toolbar_height);
# endif
	text_area_y += gui.toolbar_height;
    }
#endif

#اگر defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_HAIKU)
    gui_mch_set_tabline_pos(0, text_area_y,
    gui.menu_width, gui.tabline_height);
    اگر (gui_has_tabline())
	text_area_y += gui.tabline_height;
#endif

    text_area_width = gui.رقم_cols * gui.char_width + gui.border_offset * 2;
    text_area_height = gui.رقم_rows * gui.char_height + gui.border_offset * 2;

    gui_mch_set_text_area_pos(text_area_x,
			      text_area_y,
			      text_area_width,
			      text_area_height
#اگر defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
				  + xim_get_status_area_height()
#endif
			      );
#ifdef FEAT_MENU
    gui_position_menu();
#endif
    اگر (gui.which_scrollbars[SBAR_BOTTOM])
	gui_mch_set_scrollbar_pos(&gui.bottom_sbar,
				  text_area_x,
				  text_area_y + text_area_height
					+ gui_mch_get_scrollbar_ypadding(),
				  text_area_width,
				  gui.scrollbar_height);
    gui.چپ_sbar_x = 0;
    gui.راست_sbar_x = text_area_x + text_area_width
					+ gui_mch_get_scrollbar_xpadding();

    --hold_gui_events;
}

/*
 * Get the width of the widgets and decorations to the side of the text area.
 */
    صحیح
gui_get_base_width(تهی)
{
    صحیح	    base_width;

    base_width = 2 * gui.border_offset;
    اگر (gui.which_scrollbars[SBAR_چپ])
	base_width += gui.scrollbar_width;
    اگر (gui.which_scrollbars[SBAR_راست])
	base_width += gui.scrollbar_width;
    بازگشت base_width;
}

/*
 * Get the height of the widgets and decorations above and below the text area.
 */
    صحیح
gui_get_base_height(تهی)
{
    صحیح	    base_height;

    base_height = 2 * gui.border_offset;
    اگر (gui.which_scrollbars[SBAR_BOTTOM])
	base_height += gui.scrollbar_height;
#ifdef FEAT_GUI_GTK
    // We can't take the sizes properly into account until anything is
    // realized.  Therefore we recalculate all the values here just before
    // setting the size. (--mdcki)
#وگرنه
# ifdef FEAT_MENU
    اگر (gui.menu_is_active)
	base_height += gui.menu_height;
# endif
# ifdef FEAT_TOOLBAR
    اگر (vim_strchr(p_go, GO_TOOLBAR) != پوچ)
	base_height += gui.toolbar_height;
# endif
# اگر defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    اگر (gui_has_tabline())
	base_height += gui.tabline_height;
# endif
# اگر defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    base_height += gui_mch_text_area_extra_height();
# endif
#endif
    بازگشت base_height;
}

/*
 * Should be called after the GUI shell has been resized.  Its arguments are
 * the new width and height of the shell in pixels.
 */
    تهی
gui_resize_shell(صحیح pixel_width, صحیح pixel_height)
{
    ایستا صحیح	busy = FALSE;

    اگر (!gui.shell_created)	    // ignore when still initializing
	بازگشت;

    /*
     * Can't resize the screen while it is being redrawn.  Remember the new
     * size and handle it later.
     */
    اگر (updating_screen || busy)
    {
	new_pixel_width = pixel_width;
	new_pixel_height = pixel_height;
	بازگشت;
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
#اگر defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk_init_decor_height();
#endif

    gui.رقم_cols = (pixel_width - gui_get_base_width()) / gui.char_width;
    gui.رقم_rows = (pixel_height - gui_get_base_height()) / gui.char_height;

#ifdef USE_GTK4
    gui_gtk4_update_size();
#endif

    gui_position_components(pixel_width);
    gui_reset_scroll_region();

#اگر defined(FEAT_GUI_GTK) && defined(USE_GTK4) && !defined(USE_GTK4)
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
    اگر (State == MODE_ASKMORE || State == MODE_CONFIRM)
	gui.row = gui.رقم_rows;

    // Only comparing Rows and Columns may be sufficient, but let's stay on
    // the safe side.
    اگر (gui.رقم_rows != screen_Rows || gui.رقم_cols != screen_Columns
	    || gui.رقم_rows != Rows || gui.رقم_cols != Columns || gui.force_redraw)
    {
	shell_resized();
	gui.force_redraw = 0;
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    gui_update_scrollbars(TRUE);
    gui_update_cursor(FALSE, TRUE);
#اگر defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(pixel_width, pixel_height);
#endif
#اگر defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
    xim_set_status_area();
#endif

    busy = FALSE;

    // We may have been called again while redrawing the screen.
    // Need to do it all again with the latest size then.  But only if the size
    // actually changed.
    اگر (new_pixel_height)
    {
	اگر (pixel_width == new_pixel_width && pixel_height == new_pixel_height)
	{
	    new_pixel_width = 0;
	    new_pixel_height = 0;
	}
	وگرنه
	{
	    pixel_width = new_pixel_width;
	    pixel_height = new_pixel_height;
	    برو again;
	}
    }
}

/*
 * Check if gui_resize_shell() must be called.
 */
    تهی
gui_may_resize_shell(تهی)
{
    اگر (new_pixel_height)
    {
	// careful: gui_resize_shell() may postpone the resize again if we
	// were called indirectly by it
	gui_resize_shell(new_pixel_width, new_pixel_height);
    }
}

    صحیح
gui_get_shellsize(تهی)
{
    Rows = gui.رقم_rows;
    Columns = gui.رقم_cols;
    بازگشت OK;
}

/*
 * Set the size of the Vim shell according to Rows and Columns.
 * If "fit_to_display" is TRUE then the size may be reduced to fit the window
 * on the screen.
 * When "mustset" is TRUE the size was set by the user. When FALSE a UI
 * component was added or removed (e.g., a scrollbar).
 */
    تهی
gui_set_shellsize(
    صحیح		mustset UNUSED,
    صحیح		fit_to_display,
    صحیح		direction)		// RESIZE_HOR, RESIZE_VER
{
    صحیح		base_width;
    صحیح		base_height;
    صحیح		width;
    صحیح		height;
    صحیح		min_width;
    صحیح		min_height;
    صحیح		screen_w;
    صحیح		screen_h;
#ifdef FEAT_GUI_GTK
    صحیح		un_maximize = mustset;
    صحیح		did_adjust = 0;
#endif
    صحیح		x = -1, y = -1;

    اگر (!gui.shell_created)
	بازگشت;

#اگر defined(MSWIN) || defined(FEAT_GUI_GTK)
    // If not setting to a user specified size and maximized, calculate the
    // number of characters that fit in the maximized window.
    // FIXME: gui_mch_newfont() is called here even when the font hasn't
    // changed at all.  For example, ":set guioptions=k" triggers this path
    // via gui_init_which_components() -> gui_set_shellsize(FALSE, ...).
    // The intent is to keep the window size and recalculate Rows/Columns,
    // which has nothing to do with fonts.  This should be a separate
    // function with a more descriptive name.
    اگر (!mustset && (vim_strchr(p_go, GO_KEEPWINSIZE) != پوچ
						       || gui_mch_maximized()))
    {
	gui_mch_newfont();
	بازگشت;
    }
#endif

    base_width = gui_get_base_width();
    base_height = gui_get_base_height();
    اگر (fit_to_display)
	// Remember the original window position.
	(تهی)gui_mch_get_winpos(&x, &y);

    width = Columns * gui.char_width + base_width;
    height = Rows * gui.char_height + base_height;

    اگر (fit_to_display)
    {
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	اگر ((direction & RESIZE_HOR) && width > screen_w)
	{
	    Columns = (screen_w - base_width) / gui.char_width;
	    اگر (Columns < MIN_COLUMNS)
		Columns = MIN_COLUMNS;
	    width = Columns * gui.char_width + base_width;
#ifdef FEAT_GUI_GTK
	    ++did_adjust;
#endif
	}
	اگر ((direction & RESIZE_VERT) && height > screen_h)
	{
	    Rows = (screen_h - base_height) / gui.char_height;
	    check_shellsize();
	    height = Rows * gui.char_height + base_height;
#ifdef FEAT_GUI_GTK
	    ++did_adjust;
#endif
	}
#ifdef FEAT_GUI_GTK
	اگر (did_adjust == 2 || (width + gui.char_width >= screen_w
				     && height + gui.char_height >= screen_h))
	    // don't unmaximize if at maximum size
	    un_maximize = FALSE;
#endif
    }
    limit_screen_size();
    gui.رقم_cols = Columns;
    gui.رقم_rows = Rows;
#ifdef USE_GTK4
    // Keep the drawing area in sync with the size Vim is going to draw.
    gui_gtk4_update_size();
#endif

    min_width = base_width + MIN_COLUMNS * gui.char_width;
    min_height = base_height + MIN_سطرها * gui.char_height;
    min_height += tabline_height() * gui.char_height;

#ifdef FEAT_GUI_GTK
    اگر (un_maximize)
    {
	// If the window size is smaller than the screen unmaximize the
	// window, otherwise resizing won't work.
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	اگر ((width + gui.char_width < screen_w
				   || height + gui.char_height * 2 < screen_h)
		&& gui_mch_maximized())
	    gui_mch_unmaximize();
    }
#endif

    gui_mch_set_shellsize(width, height, min_width, min_height,
					  base_width, base_height, direction);

    اگر (fit_to_display && x >= 0 && y >= 0)
    {
	// Some window managers put the Vim window left of/above the screen.
	// Only change the position if it wasn't already negative before
	// (happens on MS-Windows with a secondary monitor).
	gui_mch_update();
	اگر (gui_mch_get_winpos(&x, &y) == OK && (x < 0 || y < 0))
	    gui_mch_set_winpos(x < 0 ? 0 : x, y < 0 ? 0 : y);
    }

    gui_position_components(width);
    gui_update_scrollbars(TRUE);
    gui_reset_scroll_region();

#اگر defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(width, height);
#endif
}

/*
 * Called when Rows and/or Columns has changed.
 */
    تهی
gui_new_shellsize(تهی)
{
    gui_reset_scroll_region();
}

/*
 * Make scroll region cover whole screen.
 */
    ایستا تهی
gui_reset_scroll_region(تهی)
{
    gui.scroll_region_بالاترین = 0;
    gui.scroll_region_bot = gui.رقم_rows - 1;
    gui.scroll_region_چپ = 0;
    gui.scroll_region_راست = gui.رقم_cols - 1;
}

    ایستا تهی
gui_start_highlight(صحیح mask)
{
    اگر (mask > HL_ALL)		    // highlight code
	gui.highlight_mask = mask;
    وگرنه			    // mask
	gui.highlight_mask |= mask;
}

    تهی
gui_stop_highlight(صحیح mask)
{
    اگر (mask > HL_ALL)		    // highlight code
	gui.highlight_mask = HL_NORMAL;
    وگرنه			    // mask
	gui.highlight_mask &= ~mask;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    تهی
gui_clear_block(
    صحیح	    row1,
    صحیح	    col1,
    صحیح	    row2,
    صحیح	    col2)
{
    // Clear the selection if we are about to write over it
    clip_may_clear_selection(row1, row2);

    gui_mch_clear_block(row1, col1, row2, col2);

    // Invalidate cursor if it was in this block
    اگر (       gui.cursor_row >= row1 && gui.cursor_row <= row2
	    && gui.cursor_col >= col1 && gui.cursor_col <= col2)
	gui.cursor_is_valid = false;
}

/*
 * Write code to update the cursor later.  This avoids the need to flush the
 * output buffer before calling gui_update_cursor().
 */
    تهی
gui_update_cursor_later(تهی)
{
    OUT_STR("\033|s");
}

    تهی
gui_write(
    char_u	*s,
    صحیح		len)
{
    char_u	*p;
    صحیح		arg1 = 0, arg2 = 0;
    صحیح		force_cursor = FALSE;	// force cursor update
    صحیح		force_scrollbar = FALSE;
    ایستا win_T	*old_curwin = پوچ;

// #define DEBUG_GUI_WRITE
#ifdef DEBUG_GUI_WRITE
    {
	صحیح i;
	char_u *str;

	printf("gui_write(%d):\n    ", len);
	برای (i = 0; i < len; i++)
	    اگر (s[i] == ESC)
	    {
		اگر (i != 0)
		    printf("\n    ");
		printf("<ESC>");
	    }
	    وگرنه
	    {
		str = transchar_byte(s[i]);
		اگر (str[0] && str[1])
		    printf("<%s>", (نویسه *)str);
		وگرنه
		    printf("%s", (نویسه *)str);
	    }
	printf("\n");
    }
#endif
    وقتی (len)
    {
	اگر (s[0] == ESC && s[1] == '|')
	{
	    p = s + 2;
	    اگر (VIM_ISDIGIT(*p) || (*p == '-' && VIM_ISDIGIT(*(p + 1))))
	    {
		arg1 = getdigits(&p);
		اگر (p > s + len)
		    بشکن;
		اگر (*p == ';')
		{
		    ++p;
		    arg2 = getdigits(&p);
		    اگر (p > s + len)
			بشکن;
		}
	    }
	    انتخاب (*p)
	    {
		حالت 'C':	// Clear screen
		    clip_scroll_selection(9999);
		    gui_mch_clear_all();
		    gui.cursor_is_valid = false;
		    force_scrollbar = TRUE;
		    بشکن;
		حالت 'M':	// Move cursor
		    gui_set_cursor(arg1, arg2);
		    بشکن;
		حالت 's':	// force cursor (shape) update
		    force_cursor = TRUE;
		    بشکن;
		حالت 'R':	// Set scroll region
		    اگر (arg1 < arg2)
		    {
			gui.scroll_region_بالاترین = arg1;
			gui.scroll_region_bot = arg2;
		    }
		    وگرنه
		    {
			gui.scroll_region_بالاترین = arg2;
			gui.scroll_region_bot = arg1;
		    }
		    بشکن;
		حالت 'V':	// Set vertical scroll region
		    اگر (arg1 < arg2)
		    {
			gui.scroll_region_چپ = arg1;
			gui.scroll_region_راست = arg2;
		    }
		    وگرنه
		    {
			gui.scroll_region_چپ = arg2;
			gui.scroll_region_راست = arg1;
		    }
		    بشکن;
		حالت 'd':	// Delete line
		    gui_delete_سطرها(gui.row, 1);
		    بشکن;
		حالت 'D':	// Delete lines
		    gui_delete_سطرها(gui.row, arg1);
		    بشکن;
		حالت 'i':	// Insert line
		    gui_درج_سطرها(gui.row, 1);
		    بشکن;
		حالت 'I':	// Insert lines
		    gui_درج_سطرها(gui.row, arg1);
		    بشکن;
		حالت '$':	// Clear to end-of-line
		    gui_clear_block(gui.row, gui.col, gui.row,
							    (صحیح)Columns - 1);
		    بشکن;
		حالت 'h':	// Turn on highlighting
		    gui_start_highlight(arg1);
		    بشکن;
		حالت 'H':	// Turn off highlighting
		    gui_stop_highlight(arg1);
		    بشکن;
		حالت 'f':	// flash the window (visual bell)
		    gui_mch_flash(arg1 == 0 ? 20 : arg1);
		    بشکن;
		پیشفرض:
		    p = s + 1;	// Skip the ESC
		    بشکن;
	    }
	    len -= (صحیح)(++p - s);
	    s = p;
	}
	وگرنه اگر (s[0] < 0x20		// Ctrl character
#ifdef FEAT_SIGN_ICONS
		&& s[0] != SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
		&& s[0] != MULTISIGN_BYTE
# endif
#endif
		)
	{
	    اگر (s[0] == '\n')		// NL
	    {
		gui.col = 0;
		اگر (gui.row < gui.scroll_region_bot)
		    gui.row++;
		وگرنه
		    gui_delete_سطرها(gui.scroll_region_بالاترین, 1);
	    }
	    وگرنه اگر (s[0] == '\r')	// CR
	    {
		gui.col = 0;
	    }
	    وگرنه اگر (s[0] == '\b')	// Backspace
	    {
		اگر (gui.col)
		    --gui.col;
	    }
	    وگرنه اگر (s[0] == Ctrl_L)	// cursor-right
	    {
		++gui.col;
	    }
	    وگرنه اگر (s[0] == Ctrl_G)	// Beep
	    {
		gui_mch_beep();
	    }
	    // Other Ctrl character: shouldn't happen!

	    --len;	// Skip this char
	    ++s;
	}
	وگرنه
	{
	    p = s;
	    وقتی (len > 0 && (
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
	    gui_outstr(s, (صحیح)(p - s));
	    s = p;
	}
    }

    // Postponed update of the cursor (won't work if "can_update_cursor" isn't
    // set).
    اگر (force_cursor)
	gui_update_cursor(TRUE, TRUE);

    // When switching to another window the dragging must have stopped.
    // Required for GTK, dragged_sb isn't reset.
    اگر (old_curwin != curwin)
	gui.dragged_sb = SBAR_NONE;

    // Update the scrollbars after clearing the screen or when switched
    // to another window.
    // Update the horizontal scrollbar always, it's difficult to check all
    // situations where it might change.
    اگر (force_scrollbar || old_curwin != curwin)
	gui_update_scrollbars(force_scrollbar);
    وگرنه
	gui_update_horiz_scrollbar(FALSE);
    old_curwin = curwin;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#اگر defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#endif

    gui_may_flush();		    // In case vim decides to take a nap
}

/*
 * When ScreenLines[] is invalid, updating the cursor should not be done, it
 * produces wrong results.  Call gui_dont_update_cursor() before that code and
 * gui_can_update_cursor() afterwards.
 */
    تهی
gui_dont_update_cursor(صحیح undraw)
{
    اگر (!gui.in_use)
	بازگشت;

    // Undraw the cursor now, we probably can't do it after the change.
    اگر (undraw)
	gui_undraw_cursor();
    can_update_cursor = FALSE;
}

    تهی
gui_can_update_cursor(تهی)
{
    can_update_cursor = TRUE;
    // No need to update the cursor right now, there is always more output
    // after scrolling.
}

/*
 * Disable issuing gui_mch_flush().
 */
    تهی
gui_disable_flush(تهی)
{
    ++disable_flush;
}

/*
 * Enable issuing gui_mch_flush().
 */
    تهی
gui_enable_flush(تهی)
{
    --disable_flush;
}

/*
 * Issue gui_mch_flush() if it is not disabled.
 */
    تهی
gui_may_flush(تهی)
{
    اگر (disable_flush == 0)
	gui_mch_flush();
}

    ایستا تهی
gui_outstr(char_u *s, صحیح len)
{
    صحیح	    this_len;
    صحیح	    cells;

    اگر (len == 0)
	بازگشت;

    اگر (len < 0)
	len = (صحیح)STRLEN(s);

    وقتی (len > 0)
    {
	اگر (has_mbyte)
	{
	    // Find out how many chars fit in the current line.
	    cells = 0;
	    برای (this_len = 0; this_len < len; )
	    {
		cells += (*mb_ptr2cells)(s + this_len);
		اگر (gui.col + cells > Columns)
		    بشکن;
		this_len += (*mb_ptr2len)(s + this_len);
	    }
	    اگر (this_len > len)
		this_len = len;	    // don't include following composing char
	}
	وگرنه اگر (gui.col + len > Columns)
	    this_len = Columns - gui.col;
	وگرنه
	    this_len = len;

	(تهی)gui_outstr_nowrap(s, this_len,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	s += this_len;
	len -= this_len;
	// fill up for a double-width char that doesn't fit.
	اگر (len > 0 && gui.col < Columns)
	    (تهی)gui_outstr_nowrap((char_u *)" ", 1,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	// The cursor may wrap to the next line.
	اگر (gui.col >= Columns)
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
    ایستا صحیح
gui_screenchar(
    صحیح		off,	    // Offset from start of screen
    صحیح		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    صحیح		back)	    // backup this many chars when using bold trick
{
    char_u	buf[MB_MAXBYTES + 1];

    // Don't draw right half of a double-width UTF-8 char. "cannot happen"
    اگر (enc_utf8 && Screen_سطرها[off] == 0)
	بازگشت OK;

    اگر (enc_utf8 && Screen_سطرها_UC[off] != 0)
	// Draw UTF-8 multi-byte character.
	بازگشت gui_outstr_nowrap(buf, utfc_char2bytes(off, buf),
							 flags, fg, bg, back);

    اگر (enc_dbcs == DBCS_JPNU && Screen_سطرها[off] == 0x8e)
    {
	buf[0] = Screen_سطرها[off];
	buf[1] = ScreenLines2[off];
	بازگشت gui_outstr_nowrap(buf, 2, flags, fg, bg, back);
    }

    // Draw non-multi-byte character or DBCS character.
    بازگشت gui_outstr_nowrap(Screen_سطرها + off,
	    enc_dbcs ? (*mb_ptr2len)(Screen_سطرها + off) : 1,
							 flags, fg, bg, back);
}
#endif

#ifdef FEAT_GUI_GTK
/*
 * Output the string at the given screen position.  This is used in place
 * of gui_screenchar() where possible because Pango needs as much context
 * as possible to work nicely.  It's a lot faster as well.
 */
    ایستا صحیح
gui_screenstr(
    صحیح		off,	    // Offset from start of screen
    صحیح		len,	    // string length in screen cells
    صحیح		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    صحیح		back)	    // backup this many chars when using bold trick
{
    char_u  *buf;
    صحیح	    outlen = 0;
    صحیح	    i;
    صحیح	    retval;

    اگر (len <= 0) // "cannot happen"?
	بازگشت OK;

    اگر (enc_utf8)
    {
	buf = alloc(len * MB_MAXBYTES + 1);
	اگر (buf == پوچ)
	    بازگشت OK; // not much we could do here...

	برای (i = off; i < off + len; ++i)
	{
	    اگر (Screen_سطرها[i] == 0)
		ادامه; // skip second half of double-width char

	    اگر (Screen_سطرها_UC[i] == 0)
		buf[outlen++] = Screen_سطرها[i];
	    وگرنه
		outlen += utfc_char2bytes(i, buf + outlen);
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, back);
	vim_free(buf);

	بازگشت retval;
    }
    وگرنه اگر (enc_dbcs == DBCS_JPNU)
    {
	buf = alloc(len * 2 + 1);
	اگر (buf == پوچ)
	    بازگشت OK; // not much we could do here...

	برای (i = off; i < off + len; ++i)
	{
	    buf[outlen++] = Screen_سطرها[i];

	    // handle double-byte single-width char
	    اگر (Screen_سطرها[i] == 0x8e)
		buf[outlen++] = ScreenLines2[i];
	    وگرنه اگر (MB_BYTE2LEN(Screen_سطرها[i]) == 2)
		buf[outlen++] = Screen_سطرها[++i];
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, back);
	vim_free(buf);

	بازگشت retval;
    }
    وگرنه
    {
	بازگشت gui_outstr_nowrap(&Screen_سطرها[off], len,
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
    ایستا صحیح
gui_outstr_nowrap(
    char_u	*s,
    صحیح		len,
    صحیح		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    صحیح		back)	    // backup this many chars when using bold trick
{
    long_u	highlight_mask;
    long_u	hl_mask_todo;
    guicolor_T	fg_color;
    guicolor_T	bg_color;
    guicolor_T	sp_color;
#اگر !defined(FEAT_GUI_GTK)
    GuiFont	font = NOFONT;
    GuiFont	wide_font = NOFONT;
# ifdef FEAT_XFONTSET
    GuiFontset	fontset = NOFONTSET;
# endif
#endif
    attrentry_T	*aep = پوچ;
    صحیح		draw_flags;
    صحیح		col = gui.col;
#ifdef FEAT_SIGN_ICONS
    صحیح		draw_sign = FALSE;
    صحیح		signcol = 0;
    char_u	extra[18];
# ifdef FEAT_NETBEANS_INTG
    صحیح		multi_sign = FALSE;
# endif
#endif

    اگر (len < 0)
	len = (صحیح)STRLEN(s);
    اگر (len == 0)
	بازگشت OK;

#ifdef FEAT_SIGN_ICONS
    اگر (*s == SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
	  || *s == MULTISIGN_BYTE
# endif
       )
    {
	size_t	slen = 2;		    // 2 spaces by default

# ifdef FEAT_NETBEANS_INTG
	اگر (*s == MULTISIGN_BYTE)
	    multi_sign = TRUE;
# endif
	// draw spaces instead
	اگر (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u' &&
		(curwin->w_p_nu || curwin->w_p_rnu))
	{
	    صحیح	n = عدد_width(curwin);

	    slen = MIN(n, (صحیح)اندازه(extra) - 1);
	}
	vim_memset(extra, ' ', slen);
	extra[slen] = NUL;
	s = extra;
	اگر (len == 1 && col > 0)
	    --col;
	len = (صحیح)slen;
	اگر (len > 2)
	    // right align sign icon in the number column
	    signcol = col + len - 3;
	وگرنه
	    signcol = col;
	draw_sign = TRUE;
	highlight_mask = 0;
    }
    وگرنه
#endif
    اگر (gui.highlight_mask > HL_ALL)
    {
	aep = syn_gui_attr2entry(gui.highlight_mask);
	اگر (aep == پوچ)	    // highlighting not set
	    highlight_mask = 0;
	وگرنه
	    highlight_mask = aep->ae_attr;
    }
    وگرنه
	highlight_mask = gui.highlight_mask;
    hl_mask_todo = highlight_mask;

#اگر !defined(FEAT_GUI_GTK)
    // Set the font
    اگر (aep != پوچ && aep->ae_u.gui.font != NOFONT)
	font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
    وگرنه اگر (aep != پوچ && aep->ae_u.gui.fontset != NOFONTSET)
	fontset = aep->ae_u.gui.fontset;
# endif
    وگرنه
    {
# ifdef FEAT_XFONTSET
	اگر (gui.fontset != NOFONTSET)
	    fontset = gui.fontset;
	وگرنه
# endif
	    اگر (hl_mask_todo & (HL_BOLD | HL_STANDOUT))
	{
	    اگر ((hl_mask_todo & HL_ITALIC) && gui.boldital_font != NOFONT)
	    {
		font = gui.boldital_font;
		hl_mask_todo &= ~(HL_BOLD | HL_STANDOUT | HL_ITALIC);
	    }
	    وگرنه اگر (gui.bold_font != NOFONT)
	    {
		font = gui.bold_font;
		hl_mask_todo &= ~(HL_BOLD | HL_STANDOUT);
	    }
	    وگرنه
		font = gui.norm_font;
	}
	وگرنه اگر ((hl_mask_todo & HL_ITALIC) && gui.ital_font != NOFONT)
	{
	    font = gui.ital_font;
	    hl_mask_todo &= ~HL_ITALIC;
	}
	وگرنه
	    font = gui.norm_font;

	/*
	 * Choose correct wide_font by font.  wide_font should be set with font
	 * at same time in above block.  But it will make many "ifdef" nasty
	 * blocks.  So we do it here.
	 */
	اگر (font == gui.boldital_font && gui.wide_boldital_font)
	    wide_font = gui.wide_boldital_font;
	وگرنه اگر (font == gui.bold_font && gui.wide_bold_font)
	    wide_font = gui.wide_bold_font;
	وگرنه اگر (font == gui.ital_font && gui.wide_ital_font)
	    wide_font = gui.wide_ital_font;
	وگرنه اگر (font == gui.norm_font && gui.wide_font)
	    wide_font = gui.wide_font;
    }
# ifdef FEAT_XFONTSET
    اگر (fontset != NOFONTSET)
	gui_mch_set_fontset(fontset);
    وگرنه
# endif
	gui_mch_set_font(font);
#endif

    draw_flags = 0;

    // Set the color
    bg_color = gui.back_pixel;
    اگر ((flags & GUI_MON_IS_CURSOR) && gui.in_focus)
    {
	draw_flags |= DRAW_CURSOR;
	fg_color = fg;
	bg_color = bg;
	sp_color = fg;
    }
    وگرنه اگر (aep != پوچ)
    {
	fg_color = aep->ae_u.gui.fg_color;
	اگر (fg_color == INVALCOLOR)
	    fg_color = gui.norm_pixel;
	bg_color = aep->ae_u.gui.bg_color;
	اگر (bg_color == INVALCOLOR)
	    bg_color = gui.back_pixel;
	sp_color = aep->ae_u.gui.sp_color;
	اگر (sp_color == INVALCOLOR)
	    sp_color = fg_color;
    }
    وگرنه
    {
	fg_color = gui.norm_pixel;
	sp_color = fg_color;
    }

    اگر (highlight_mask & (HL_INVERSE | HL_STANDOUT))
    {
	gui_mch_set_fg_color(bg_color);
	gui_mch_set_bg_color(fg_color);
    }
    وگرنه
    {
	gui_mch_set_fg_color(fg_color);
	gui_mch_set_bg_color(bg_color);
    }
    gui_mch_set_sp_color(sp_color);

    // Clear the selection if we are about to write over it
    اگر (!(flags & GUI_MON_NOCLEAR))
	clip_may_clear_selection(gui.row, gui.row);


    // If there's no bold font, then fake it
    اگر (hl_mask_todo & (HL_BOLD | HL_STANDOUT))
	draw_flags |= DRAW_BOLD;

    /*
     * When drawing bold or italic characters the spill-over from the left
     * neighbor may be destroyed.  Let the caller backup to start redrawing
     * just after a blank.
     */
    اگر (back != 0 && ((draw_flags & DRAW_BOLD) || (highlight_mask & HL_ITALIC)))
	بازگشت FAIL;

#اگر defined(FEAT_GUI_GTK)
    // If there's no italic font, then fake it.
    // For GTK2, we don't need a different font for italic style.
    اگر (hl_mask_todo & HL_ITALIC)
	draw_flags |= DRAW_ITALIC;

    // Do we underline the text?
    اگر (hl_mask_todo & HL_UNDERLINE)
	draw_flags |= DRAW_UNDERL;

#وگرنه
    // Do we underline the text?
    اگر ((hl_mask_todo & HL_UNDERLINE) || (hl_mask_todo & HL_ITALIC))
	draw_flags |= DRAW_UNDERL;
#endif
    // Do we undercurl the text?
    اگر (hl_mask_todo & HL_UNDERCURL)
	draw_flags |= DRAW_UNDERC;

    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

    // Do we strikethrough the text?
    اگر (hl_mask_todo & HL_STRIKETHROUGH)
	draw_flags |= DRAW_STRIKE;

    // Do we draw transparently?
    اگر (flags & GUI_MON_TRS_CURSOR)
	draw_flags |= DRAW_TRANSP;

    /*
     * Draw the text.
     */
#ifdef FEAT_GUI_GTK
    // The value returned is the length in display cells
    len = gui_gtk_draw_string(gui.row, col, s, len, draw_flags);
#وگرنه
    اگر (enc_utf8)
    {
	صحیح	start;		// index of bytes to be drawn
	صحیح	cells;		// cellwidth of bytes to be drawn
	صحیح	thislen;	// length of bytes to be drawn
	صحیح	cn;		// cellwidth of current char
	صحیح	i;		// index of current char
	صحیح	c;		// current char value
	صحیح	cl;		// byte length of current char
	صحیح	comping;	// current char is composing
	صحیح	scol = col;	// screen column
	صحیح	curr_wide = FALSE;  // use 'guifontwide'
	صحیح	prev_wide = FALSE;
	صحیح	wide_changed;
# ifdef MSWIN
	صحیح	sep_comp = FALSE;   // Don't separate composing chars.
# وگرنه
	صحیح	sep_comp = TRUE;    // Separate composing chars.
# endif

	// Break the string at a composing character, it has to be drawn on
	// top of the previous character.
	start = 0;
	cells = 0;
	برای (i = 0; i < len; i += cl)
	{
	    c = utf_ptr2char(s + i);
	    cn = utf_char2cells(c);
	    comping = utf_iscomposing(c);
	    اگر (!comping)	// count cells from non-composing chars
		cells += cn;
	    اگر (!comping || sep_comp)
	    {
		اگر (cn > 1
# ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
# endif
			&& wide_font != NOFONT)
		    curr_wide = TRUE;
		وگرنه
		    curr_wide = FALSE;
	    }
	    cl = utf_ptr2len(s + i);
	    اگر (cl == 0)	// hit end of string
		len = i + cl;	// len must be wrong "cannot happen"

	    wide_changed = curr_wide != prev_wide;

	    // Print the string so far if it's the last character or there is
	    // a composing character.
	    اگر (i + cl >= len || (comping && sep_comp && i > start)
		    || wide_changed
# اگر defined(FEAT_GUI_X11)
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
		اگر ((comping && sep_comp) || wide_changed)
		    thislen = i - start;
		وگرنه
		    thislen = i - start + cl;
		اگر (thislen > 0)
		{
		    اگر (prev_wide)
			gui_mch_set_font(wide_font);
		    gui_mch_draw_string(gui.row, scol, s + start, thislen,
								  draw_flags);
		    اگر (prev_wide)
			gui_mch_set_font(font);
		    start += thislen;
		}
		scol += cells;
		cells = 0;
		// Adjust to not draw a character which width is changed
		// against with last one.
		اگر (wide_changed && !(comping && sep_comp))
		{
		    scol -= cn;
		    cl = 0;
		}

# اگر defined(FEAT_GUI_X11)
		// No fontset: draw a space to fill the gap after a wide char
		//
		اگر (cn > 1 && (draw_flags & DRAW_TRANSP) == 0
#  ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
#  endif
			&& !wide_changed)
		    gui_mch_draw_string(gui.row, scol - 1, (char_u *)" ",
							       1, draw_flags);
# endif
	    }
	    // Draw a composing char on top of the previous char.
	    اگر (comping && sep_comp)
	    {
# اگر defined(__APPLE_CC__) && TARGET_API_MAC_CARBON
		// Carbon ATSUI autodraws composing char over previous char
		gui_mch_draw_string(gui.row, scol, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# وگرنه
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
    وگرنه
    {
	gui_mch_draw_string(gui.row, col, s, len, draw_flags);
	اگر (enc_dbcs == DBCS_JPNU)
	{
	    // Get the length in display cells, this can be different from the
	    // number of bytes for "euc-jp".
	    len = mb_string2cells(s, len);
	}
    }
#endif // !FEAT_GUI_GTK

    اگر (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
	gui.col = col + len;

    // May need to invert it when it's part of the selection.
    اگر (flags & GUI_MON_NOCLEAR)
	clip_may_redraw_selection(gui.row, col, len);

    اگر (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
    {
	// Invalidate the old physical cursor position if we wrote over it
	اگر (gui.cursor_row == gui.row
		&& gui.cursor_col >= col
		&& gui.cursor_col < col + len)
	    gui.cursor_is_valid = false;
    }

#ifdef FEAT_SIGN_ICONS
    اگر (draw_sign)
	// Draw the sign on top of the spaces.
	gui_mch_drawsign(gui.row, signcol, gui.highlight_mask);
# اگر defined(FEAT_NETBEANS_INTG) && (defined(FEAT_GUI_X11) \
	|| defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN))
    اگر (multi_sign)
	netbeans_draw_multisign_indicator(gui.row);
# endif
#endif

    بازگشت OK;
}

/*
 * Undraw the cursor.  This actually redraws the character at the cursor
 * position, plus some more characters when needed.
 */
    تهی
gui_undraw_cursor(تهی)
{
    اگر (!gui.cursor_is_valid)
	بازگشت;

    // Always redraw the character just before if there is one, because
    // with some fonts and characters there can be a one pixel overlap.
    صحیح startcol = gui.cursor_col > 0 ? gui.cursor_col - 1 : gui.cursor_col;
    صحیح endcol = gui.cursor_col;

#اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    gui_adjust_undraw_cursor_for_ligatures(&startcol, &endcol);
#endif
    gui_redraw_block(gui.cursor_row, startcol,
	    gui.cursor_row, endcol, GUI_MON_NOCLEAR);

    // Cursor_is_valid is reset when the cursor is undrawn, also reset it
    // here in case it wasn't needed to undraw it.
    gui.cursor_is_valid = false;
}

    تهی
gui_redraw(
    صحیح		x,
    صحیح		y,
    صحیح		w,
    صحیح		h)
{
    صحیح		row1, col1, row2, col2;

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
    اگر (gui.row == gui.cursor_row)
	gui_update_cursor(TRUE, TRUE);
}

/*
 * Draw a rectangular block of characters, from row1 to row2 (inclusive) and
 * from col1 to col2 (inclusive).
 */
    تهی
gui_redraw_block(
    صحیح		row1,
    صحیح		col1,
    صحیح		row2,
    صحیح		col2,
    صحیح		flags)	// flags for gui_outstr_nowrap()
{
    صحیح		old_row, old_col;
    long_u	old_hl_mask;
    صحیح		off;
    sattr_T	first_attr;
    صحیح		idx, len;
    صحیح		back, nback;
    صحیح		orig_col1, orig_col2;

    // Don't try to update when ScreenLines is not valid
    اگر (!screen_cleared || Screen_سطرها == پوچ)
	بازگشت;

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

    برای (gui.row = row1; gui.row <= row2; gui.row++)
    {
	// When only half of a double-wide character is in the block, include
	// the other half.
	col1 = orig_col1;
	col2 = orig_col2;
	off = سطر_Offset[gui.row];
	اگر (enc_dbcs != 0)
	{
	    اگر (col1 > 0)
		col1 -= dbcs_screen_head_off(Screen_سطرها + off,
						    Screen_سطرها + off + col1);
	    col2 += dbcs_screen_tail_off(Screen_سطرها + off,
						    Screen_سطرها + off + col2);
	}
	وگرنه اگر (enc_utf8)
	{
	    اگر (Screen_سطرها[off + col1] == 0)
	    {
		اگر (col1 > 0)
		    --col1;
		وگرنه
		    // FIXME: how can the first character ever be zero?
		    siemsg("NUL in ScreenLines in row %ld", (long)gui.row);
	    }
#ifdef FEAT_GUI_GTK
	    اگر (col2 + 1 < Columns && Screen_سطرها[off + col2 + 1] == 0)
		++col2;
#endif
	}
	gui.col = col1;
	off = سطر_Offset[gui.row] + gui.col;
	len = col2 - col1 + 1;

	// Find how many chars back this highlighting starts, or where a space
	// is.  Needed for when the bold trick is used
	برای (back = 0; back < col1; ++back)
	    اگر (ScreenAttrs[off - 1 - back] != ScreenAttrs[off]
		    || Screen_سطرها[off - 1 - back] == ' ')
		بشکن;

	// Break it up in strings of characters with the same attributes.
	// Print UTF-8 characters individually.
	وقتی (len > 0)
	{
	    first_attr = ScreenAttrs[off];
	    gui.highlight_mask = first_attr;
#اگر !defined(FEAT_GUI_GTK)
	    اگر (enc_utf8 && Screen_سطرها_UC[off] != 0)
	    {
		// output multi-byte character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
		اگر (gui.col < Columns && Screen_سطرها[off + 1] == 0)
		    idx = 2;
		وگرنه
		    idx = 1;
	    }
	    وگرنه اگر (enc_dbcs == DBCS_JPNU && Screen_سطرها[off] == 0x8e)
	    {
		// output double-byte, single-width character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
		idx = 1;
	    }
	    وگرنه
#endif
	    {
#ifdef FEAT_GUI_GTK
		برای (idx = 0; idx < len; ++idx)
		{
		    اگر (enc_utf8 && Screen_سطرها[off + idx] == 0)
			ادامه; // skip second half of double-width char
		    اگر (ScreenAttrs[off + idx] != first_attr)
			بشکن;
		}
		// gui_screenstr() takes care of multibyte chars
		nback = gui_screenstr(off, idx, flags,
				      (guicolor_T)0, (guicolor_T)0, back);
#وگرنه
		برای (idx = 0; idx < len && ScreenAttrs[off + idx] == first_attr;
									idx++)
		{
		    // Stop at a multi-byte Unicode character.
		    اگر (enc_utf8 && Screen_سطرها_UC[off + idx] != 0)
			بشکن;
		    اگر (enc_dbcs == DBCS_JPNU)
		    {
			// Stop at a double-byte single-width char.
			اگر (Screen_سطرها[off + idx] == 0x8e)
			    بشکن;
			اگر (len > 1 && (*mb_ptr2len)(Screen_سطرها
							    + off + idx) == 2)
			    ++idx;  // skip second byte of double-byte char
		    }
		}
		nback = gui_outstr_nowrap(Screen_سطرها + off, idx, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
#endif
	    }
	    اگر (nback == FAIL)
	    {
		// Must back up to start drawing where a bold or italic word
		// starts.
		off -= back;
		len += back;
		gui.col -= back;
	    }
	    وگرنه
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
    gui.highlight_mask = (صحیح)old_hl_mask;
}

    ایستا تهی
gui_delete_سطرها(صحیح row, صحیح شمار)
{
    اگر (شمار <= 0)
	بازگشت;

    اگر (row + شمار > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_block(row, gui.scroll_region_چپ,
			      gui.scroll_region_bot, gui.scroll_region_راست);
    وگرنه
    {
	gui_mch_delete_سطرها(row, شمار);

	// If the cursor was in the deleted lines it's now gone.  If the
	// cursor was in the scrolled lines adjust its position.
	اگر (gui.cursor_row >= row
		&& gui.cursor_col >= gui.scroll_region_چپ
		&& gui.cursor_col <= gui.scroll_region_راست)
	{
	    اگر (gui.cursor_row < row + شمار)
		gui.cursor_is_valid = false;
	    وگرنه اگر (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_row -= شمار;
	}
    }
}

    ایستا تهی
gui_درج_سطرها(صحیح row, صحیح شمار)
{
    اگر (شمار <= 0)
	بازگشت;

    اگر (row + شمار > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_block(row, gui.scroll_region_چپ,
			      gui.scroll_region_bot, gui.scroll_region_راست);
    وگرنه
    {
	gui_mch_درج_سطرها(row, شمار);

	اگر (gui.cursor_row >= gui.row
		&& gui.cursor_col >= gui.scroll_region_چپ
		&& gui.cursor_col <= gui.scroll_region_راست)
	{
	    اگر (gui.cursor_row <= gui.scroll_region_bot - شمار)
		gui.cursor_row += شمار;
	    وگرنه اگر (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_is_valid = false;
	}
    }
}

#ifdef FEAT_TIMERS
/*
 * Passed to ui_wait_for_chars_or_timer(), ignoring extra arguments.
 */
    ایستا صحیح
gui_wait_for_chars_3(
    long wtime,
    صحیح *interrupted UNUSED,
    صحیح ignore_درون_داد UNUSED)
{
    بازگشت gui_mch_wait_for_chars(wtime);
}
#endif

/*
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    ایستا صحیح
gui_wait_for_chars_or_timer(
	long wtime,
	صحیح *interrupted UNUSED,
	صحیح ignore_درون_داد UNUSED)
{
#ifdef FEAT_TIMERS
    بازگشت ui_wait_for_chars_or_timer(wtime, gui_wait_for_chars_3,
						    interrupted, ignore_درون_داد);
#وگرنه
    بازگشت gui_mch_wait_for_chars(wtime);
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
    ایستا صحیح
gui_wait_for_chars_buf(
    char_u	*buf,
    صحیح		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    صحیح		tb_change_cnt)
{
    صحیح	    retval;
    صحیح	    keep_blinking; // Guard against restarting blink cycle on CursorHold

#ifdef FEAT_MENU
    // If we're going to wait a bit, update the menus and mouse shape for the
    // current State.
    اگر (wtime != 0)
	gui_update_menus(0);
#endif

    gui_mch_update();
    اگر (درون_داد_available())	// Got char, return immediately
    {
	اگر (gui_mch_is_blinking())
	    gui_mch_stop_blink(TRUE);
	اگر (buf != پوچ && !typebuf_changed(tb_change_cnt))
	    بازگشت read_from_درون_داد_buf(buf, (long)maxlen);
	بازگشت 0;
    }
    اگر (wtime == 0)		// Don't wait for char
	بازگشت FAIL;

    // Before waiting, flush any output to the screen.
    gui_mch_flush();

    // Blink while waiting for a character.
    اگر (!gui_mch_is_blinking())
	gui_mch_start_blink();

    // Common function to loop until "wtime" is met, while handling timers and
    // other callbacks.
    retval = inchar_loop(buf, maxlen, wtime, tb_change_cnt,
			 gui_wait_for_chars_or_timer, پوچ);

    // Keep blinking when CursorHold wakes the input loop. (See PR #21115)
    keep_blinking = retval == 3 && buf != پوچ
	&& buf[0] == K_SPECIAL && buf[1] == KS_EXTRA
	&& buf[2] == (صحیح)KE_CURSORHOLD;

    اگر (!keep_blinking)
	gui_mch_stop_blink(TRUE);

    بازگشت retval;
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
    صحیح
gui_wait_for_chars(long wtime, صحیح tb_change_cnt)
{
    بازگشت gui_wait_for_chars_buf(پوچ, 0, wtime, tb_change_cnt);
}

/*
 * Equivalent of mch_inchar() for the GUI.
 */
    صحیح
gui_inchar(
    char_u  *buf,
    صحیح	    maxlen,
    long    wtime,		// milliseconds
    صحیح	    tb_change_cnt)
{
    بازگشت gui_wait_for_chars_buf(buf, maxlen, wtime, tb_change_cnt);
}

/*
 * Fill p[4] with mouse coordinates encoded for check_termcode().
 */
    ایستا تهی
fill_mouse_coord(char_u *p, صحیح col, صحیح row)
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
    تهی
gui_send_mouse_event(
    صحیح	    button,
    صحیح	    x,
    صحیح	    y,
    صحیح	    repeated_click,
    int_u   modifiers)
{
    ایستا صحیح	    prev_row = 0, prev_col = 0;
    ایستا صحیح	    prev_button = -1;
    ایستا صحیح	    رقم_clicks = 1;
    char_u	    string[10];
    شمارش کلید_extra  button_char;
    صحیح		    row, col;
#ifdef FEAT_CLIPBOARD
    صحیح		    checkfor;
    صحیح		    did_clip = FALSE;
#endif

    /*
     * Scrolling may happen at any time, also while a selection is present.
     */
    انتخاب (button)
    {
	حالت MOUSE_MOVE:
	    button_char = KE_MOUSEMOVE_XY;
	    برو button_set;
	حالت MOUSE_X1:
	    button_char = KE_X1MOUSE;
	    برو button_set;
	حالت MOUSE_X2:
	    button_char = KE_X2MOUSE;
	    برو button_set;
	حالت MOUSE_4:
	    button_char = KE_MOUSEDOWN;
	    برو button_set;
	حالت MOUSE_5:
	    button_char = KE_MOUSEUP;
	    برو button_set;
	حالت MOUSE_6:
	    button_char = KE_MOUSELEFT;
	    برو button_set;
	حالت MOUSE_7:
	    button_char = KE_MOUSERIGHT;
button_set:
	    {
		// Don't put events in the input queue now.
		اگر (hold_gui_events)
		    بازگشت;

		row = gui_xy2colrow(x, y, &col);
		// Don't report a mouse move unless moved to a
		// different character position.
		اگر (button == MOUSE_MOVE)
		{
		    اگر (row == prev_row && col == prev_col)
			بازگشت;
		    وگرنه
		    {
			prev_row = row >= 0 ? row : 0;
			prev_col = col;
		    }
		}

		string[3] = CSI;
		string[4] = KS_EXTRA;
		string[5] = (صحیح)button_char;

		// Pass the pointer coordinates of the scroll event so that we
		// know which window to scroll.
		string[6] = (char_u)(col / 128 + ' ' + 1);
		string[7] = (char_u)(col % 128 + ' ' + 1);
		string[8] = (char_u)(row / 128 + ' ' + 1);
		string[9] = (char_u)(row % 128 + ' ' + 1);

		اگر (modifiers == 0)
		    add_to_درون_داد_buf(string + 3, 7);
		وگرنه
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = 0;
		    اگر (modifiers & MOUSE_SHIFT)
			string[2] |= MOD_MASK_SHIFT;
		    اگر (modifiers & MOUSE_CTRL)
			string[2] |= MOD_MASK_CTRL;
		    اگر (modifiers & MOUSE_ALT)
			string[2] |= MOD_MASK_ALT;
		    add_to_درون_داد_buf(string, 10);
		}
		بازگشت;
	    }
    }

#ifdef FEAT_CLIPBOARD
    // If a clipboard selection is in progress, handle it
    اگر (clip_star.state == SELECT_IN_PROGRESS)
    {
	clip_process_selection(button, X_2_COL(x), Y_2_ROW(y), repeated_click);

	// A release event may still need to be sent if the position is equal.
	row = gui_xy2colrow(x, y, &col);
	اگر (button != MOUSE_RELEASE || row != prev_row || col != prev_col)
	    بازگشت;
    }

    // Determine which mouse settings to look for based on the current mode
    انتخاب (get_real_state())
    {
	حالت MODE_NORMAL_BUSY:
	حالت MODE_OP_PENDING:
# ifdef FEAT_TERMINAL
	حالت MODE_TERMINAL:
# endif
	حالت MODE_NORMAL:	checkfor = MOUSE_NORMAL; بشکن;
	حالت MODE_VISUAL:	checkfor = MOUSE_VISUAL; بشکن;
	حالت MODE_SELECT:	checkfor = MOUSE_VISUAL; بشکن;
	حالت MODE_REPLACE:
	حالت MODE_REPLACE | MODE_LANGMAP:
	حالت MODE_VREPLACE:
	حالت MODE_VREPLACE | MODE_LANGMAP:
	حالت MODE_درج:
	حالت MODE_درج | MODE_LANGMAP:
				checkfor = MOUSE_درج; بشکن;
	حالت MODE_ASKMORE:
	حالت MODE_HITRETURN:	// At the more- and hit-enter prompt pass the
				// mouse event for a click on or below the
				// message line.
				اگر (Y_2_ROW(y) >= پیام_row)
				    checkfor = MOUSE_NORMAL;
				وگرنه
				    checkfor = MOUSE_RETURN;
				بشکن;

	    /*
	     * On the command line, use the clipboard selection on all lines
	     * but the command line.  But not when pasting.
	     */
	حالت MODE_CMDLINE:
	حالت MODE_CMDLINE | MODE_LANGMAP:
	    اگر (Y_2_ROW(y) < cmdline_row && button != MOUSE_میانه)
		checkfor = MOUSE_NONE;
	    وگرنه
		checkfor = MOUSE_COMMAND;
	    بشکن;

	پیشفرض:
	    checkfor = MOUSE_NONE;
	    بشکن;
    };

    /*
     * Allow clipboard selection of text on the command line in "normal"
     * modes.  Don't do this when dragging the status line, or extending a
     * Visual selection.
     */
    اگر ((State == MODE_NORMAL || State == MODE_NORMAL_BUSY
						      || (State & MODE_درج))
	    && X_2_COL(x) >= firstwin->w_wincol
	    && X_2_COL(x) < firstwin->w_wincol + topframe->fr_width
	    && Y_2_ROW(y) >= topframe->fr_height + firstwin->w_winrow
	    && button != MOUSE_DRAG
# ifdef FEAT_MOUSESHAPE
	    && !drag_status_سطر
	    && !drag_sep_سطر
# endif
	    )
	checkfor = MOUSE_NONE;

    /*
     * Use modeless selection when holding CTRL and SHIFT pressed.
     */
    اگر ((modifiers & MOUSE_CTRL) && (modifiers & MOUSE_SHIFT))
	checkfor = MOUSE_NONEF;

    /*
     * In Ex mode, always use modeless selection.
     */
    اگر (exmode_active)
	checkfor = MOUSE_NONE;

    /*
     * If the mouse settings say to not use the mouse, use the modeless
     * selection.  But if Visual is active, assume that only the Visual area
     * will be selected.
     * Exception: On the command line, both the selection is used and a mouse
     * key is sent.
     */
    اگر (!mouse_has(checkfor) || checkfor == MOUSE_COMMAND)
    {
	// Don't do modeless selection in Visual mode.
	اگر (checkfor != MOUSE_NONEF && VIsual_active && (State & MODE_NORMAL))
	    بازگشت;

	/*
	 * When 'mousemodel' is "popup", shift-left is translated to right.
	 * But not when also using Ctrl.
	 */
	اگر (mouse_model_popup() && button == MOUSE_چپ
		&& (modifiers & MOUSE_SHIFT) && !(modifiers & MOUSE_CTRL))
	{
	    button = MOUSE_راست;
	    modifiers &= ~ MOUSE_SHIFT;
	}

	// If the selection is done, allow the right button to extend it.
	// If the selection is cleared, allow the right button to start it
	// from the cursor position.
	اگر (button == MOUSE_راست)
	{
	    اگر (clip_star.state == SELECT_CLEARED)
	    {
		اگر (State & MODE_CMDLINE)
		{
		    col = پیام_col;
		    row = پیام_row;
		}
		وگرنه
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
	وگرنه اگر (button == MOUSE_چپ)
	{
	    clip_start_selection(X_2_COL(x), Y_2_ROW(y), repeated_click);
	    did_clip = TRUE;
	}

	// Always allow pasting
	اگر (button != MOUSE_میانه)
	{
	    اگر (!mouse_has(checkfor) || button == MOUSE_RELEASE)
		بازگشت;
	    اگر (checkfor != MOUSE_COMMAND)
		button = MOUSE_چپ;
	}
	repeated_click = FALSE;
    }

    اگر (clip_star.state != SELECT_CLEARED && !did_clip)
	clip_clear_selection(&clip_star);
#endif

    // Don't put events in the input queue now.
    اگر (hold_gui_events)
	بازگشت;

    row = gui_xy2colrow(x, y, &col);

    /*
     * If we are dragging and the mouse hasn't moved far enough to be on a
     * different character, then don't send an event to vim.
     */
    اگر (button == MOUSE_DRAG)
    {
	اگر (row == prev_row && col == prev_col)
	    بازگشت;
	// Dragging above the window, set "row" to -1 to cause a scroll.
	اگر (y < 0)
	    row = -1;
    }

    /*
     * If topline has changed (window scrolled) since the last click, reset
     * repeated_click, because we don't want starting Visual mode when
     * clicking on a different character in the text.
     */
    اگر (curwin->w_topline != gui_prev_topline
#ifdef FEAT_DIFF
	    || curwin->w_topfill != gui_prev_topfill
#endif
	    )
	repeated_click = FALSE;

    string[0] = CSI;	// this sequence is recognized by check_termcode()
    string[1] = KS_MOUSE;
    string[2] = KE_FILLER;
    اگر (button != MOUSE_DRAG && button != MOUSE_RELEASE)
    {
	اگر (repeated_click)
	{
	    /*
	     * Handle multiple clicks.	They only count if the mouse is still
	     * pointing at the same character.
	     */
	    اگر (button != prev_button || row != prev_row || col != prev_col)
		رقم_clicks = 1;
	    وگرنه اگر (++رقم_clicks > 4)
		رقم_clicks = 1;
	}
	وگرنه
	    رقم_clicks = 1;
	prev_button = button;
	gui_prev_topline = curwin->w_topline;
#ifdef FEAT_DIFF
	gui_prev_topfill = curwin->w_topfill;
#endif

	string[3] = (char_u)(button | 0x20);
	SET_رقم_MOUSE_CLICKS(string[3], رقم_clicks);
    }
    وگرنه
	string[3] = (char_u)button;

    string[3] |= modifiers;
    fill_mouse_coord(string + 4, col, row);
    add_to_درون_داد_buf(string, 8);

    اگر (row < 0)
	prev_row = 0;
    وگرنه
	prev_row = row;
    prev_col = col;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#اگر defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#endif
}

/*
 * Convert x and y coordinate to column and row in text window.
 * Corrects for multi-byte character.
 * returns column in "*colp" and row as return value;
 */
    ایستا صحیح
gui_xy2colrow(صحیح x, صحیح y, صحیح *colp)
{
    صحیح		col = check_col(X_2_COL(x));
    صحیح		row = check_row(Y_2_ROW(y));

    *colp = mb_fix_col(col, row);
    بازگشت row;
}

#اگر defined(FEAT_MENU)
/*
 * Callback function for when a menu entry has been selected.
 */
    تهی
gui_menu_cb(vimmenu_T *menu)
{
    char_u  bytes[اندازه(long_u)];

    // Don't put events in the input queue now.
    اگر (hold_gui_events)
	بازگشت;

    bytes[0] = CSI;
    bytes[1] = KS_MENU;
    bytes[2] = KE_FILLER;
    add_to_درون_داد_buf(bytes, 3);
    add_long_to_buf((long_u)menu, bytes);
    add_to_درون_داد_buf_csi(bytes, اندازه(long_u));
}
#endif

ایستا صحیح	prev_which_scrollbars[3];

/*
 * Set which components are present.
 * If "oldval" is not NULL, "oldval" is the previous value, the new value is
 * in p_go.
 */
    تهی
gui_init_which_components(char_u *oldval UNUSED)
{
#ifdef FEAT_GUI_DARKTHEME
    ایستا صحیح	prev_dark_theme = FALSE;
    صحیح		using_dark_theme = FALSE;
#endif
#ifdef FEAT_MENU
    ایستا صحیح	prev_menu_is_active = -1;
#endif
#ifdef FEAT_TOOLBAR
    ایستا صحیح	prev_toolbar = -1;
    صحیح		using_toolbar = FALSE;
#endif
#ifdef FEAT_GUI_TABLINE
    صحیح		using_tabline;
#endif
#ifdef FEAT_GUI_MSWIN
    ایستا صحیح	prev_titlebar = FALSE;
    صحیح		using_titlebar = FALSE;
#endif

#اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    ایستا صحیح	prev_fullscreen = FALSE;
    صحیح		using_fullscreen = FALSE;
#endif
#اگر defined(FEAT_MENU)
    ایستا صحیح	prev_tearoff = -1;
    صحیح		using_tearoff = FALSE;
#endif

    char_u	*p;
    صحیح		i;
#ifdef FEAT_MENU
    صحیح		grey_old, grey_new;
    char_u	*temp;
#endif
    win_T	*wp;
    صحیح		need_set_size;
    صحیح		fix_size;

#ifdef FEAT_MENU
    اگر (oldval != پوچ && gui.in_use)
    {
	/*
	 * Check if the menus go from grey to non-grey or vice versa.
	 */
	grey_old = (vim_strchr(oldval, GO_GREY) != پوچ);
	grey_new = (vim_strchr(p_go, GO_GREY) != پوچ);
	اگر (grey_old != grey_new)
	{
	    temp = p_go;
	    p_go = oldval;
	    gui_update_menus(MENU_ALL_MODES);
	    p_go = temp;
	}
    }
    gui.menu_is_active = false;
#endif

    برای (i = 0; i < 3; i++)
	gui.which_scrollbars[i] = false;
    برای (p = p_go; *p; p++)
	انتخاب (*p)
	{
	    حالت GO_چپ:
		gui.which_scrollbars[SBAR_چپ] = true;
		بشکن;
	    حالت GO_راست:
		gui.which_scrollbars[SBAR_راست] = true;
		بشکن;
	    حالت GO_VLEFT:
		اگر (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_چپ] = true;
		بشکن;
	    حالت GO_VRIGHT:
		اگر (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_راست] = true;
		بشکن;
	    حالت GO_BOT:
		gui.which_scrollbars[SBAR_BOTTOM] = true;
		بشکن;
#ifdef FEAT_GUI_DARKTHEME
	    حالت GO_DARKTHEME:
		using_dark_theme = TRUE;
		بشکن;
#endif
#ifdef FEAT_MENU
	    حالت GO_MENUS:
		gui.menu_is_active = true;
		بشکن;
#endif
	    حالت GO_GREY:
		// make menu's have grey items, ignored here
		بشکن;
#ifdef FEAT_GUI_MSWIN
	    حالت GO_TITLEBAR:
		using_titlebar = TRUE;
		بشکن;
#endif
#اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
	    حالت GO_FULLSCREEN:
		using_fullscreen = TRUE;
		بشکن;
#endif
#ifdef FEAT_TOOLBAR
	    حالت GO_TOOLBAR:
		using_toolbar = TRUE;
		بشکن;
#endif
	    حالت GO_TEAROFF:
#اگر defined(FEAT_MENU)
		using_tearoff = TRUE;
#endif
		بشکن;
	    پیشفرض:
		// Ignore options that are not supported
		بشکن;
	}

    اگر (!gui.in_use)
	بازگشت;

    need_set_size = 0;
    fix_size = FALSE;

#ifdef FEAT_GUI_MSWIN
    اگر (using_titlebar != prev_titlebar)
    {
	gui_mch_set_titlebar_colors();
	prev_titlebar = using_titlebar;
    }
#endif

#اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    اگر (using_fullscreen != prev_fullscreen)
    {
	gui_mch_set_fullscreen(using_fullscreen);
	prev_fullscreen = using_fullscreen;
    }
#endif

#ifdef FEAT_GUI_DARKTHEME
    اگر (using_dark_theme != prev_dark_theme)
    {
	gui_mch_set_dark_theme(using_dark_theme);
	prev_dark_theme = using_dark_theme;
    }
#endif

#ifdef FEAT_GUI_TABLINE
    // Update the GUI tab line, it may appear or disappear.  This may
    // cause the non-GUI tab line to disappear or appear.
    using_tabline = gui_has_tabline();
    اگر (!gui_mch_showing_tabline() != !using_tabline)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = Rows;
	gui_update_tabline();
	Rows = i;
	need_set_size |= RESIZE_VERT;
	اگر (using_tabline)
	    fix_size = TRUE;
	اگر (!gui_use_tabline())
	    redraw_tabline = TRUE;    // may draw non-GUI tab line
    }
#endif

    برای (i = 0; i < 3; i++)
    {
	// The scrollbar needs to be updated when it is shown/unshown and
	// when switching tab pages.  But the size only changes when it's
	// shown/unshown.  Thus we need two places to remember whether a
	// scrollbar is there or not.
	اگر (gui.which_scrollbars[i] != prev_which_scrollbars[i]
		|| gui.which_scrollbars[i]
		!= curtab->tp_prev_which_scrollbars[i])
	{
	    اگر (i == SBAR_BOTTOM)
		gui_mch_enable_scrollbar(&gui.bottom_sbar,
			gui.which_scrollbars[i]);
	    وگرنه
	    {
		FOR_ALL_WINDOWS(wp)
		    gui_do_scrollbar(wp, i, gui.which_scrollbars[i]);
	    }
	    اگر (gui.which_scrollbars[i] != prev_which_scrollbars[i])
	    {
		اگر (i == SBAR_BOTTOM)
		    need_set_size |= RESIZE_VERT;
		وگرنه
		    need_set_size |= RESIZE_HOR;
		اگر (gui.which_scrollbars[i])
		    fix_size = TRUE;
	    }
	}
	curtab->tp_prev_which_scrollbars[i] = gui.which_scrollbars[i];
	prev_which_scrollbars[i] = gui.which_scrollbars[i];
    }

#ifdef FEAT_MENU
    اگر (gui.menu_is_active != prev_menu_is_active)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = Rows;
	gui_mch_enable_menu(gui.menu_is_active);
	Rows = i;
	prev_menu_is_active = gui.menu_is_active;
	need_set_size |= RESIZE_VERT;
	اگر (gui.menu_is_active)
	    fix_size = TRUE;
    }
#endif

#ifdef FEAT_TOOLBAR
    اگر (using_toolbar != prev_toolbar)
    {
	gui_mch_show_toolbar(using_toolbar);
	prev_toolbar = using_toolbar;
	need_set_size |= RESIZE_VERT;
	اگر (using_toolbar)
	    fix_size = TRUE;
    }
#endif
#اگر defined(FEAT_MENU) && !(defined(MSWIN) && !defined(FEAT_TEAROFF))
    اگر (using_tearoff != prev_tearoff)
    {
	gui_mch_toggle_tearoffs(using_tearoff);
	prev_tearoff = using_tearoff;
    }
#endif
    اگر (need_set_size != 0)
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

	    اگر (!gui.starting && need_set_size != (RESIZE_VERT | RESIZE_HOR))
		(تهی)char_avail();
	    اگر ((need_set_size & RESIZE_VERT) == 0)
		Rows = prev_Rows;
	    وگرنه
		Rows = post_Rows;
	    اگر ((need_set_size & RESIZE_HOR) == 0)
		Columns = prev_Columns;
	    وگرنه
		Columns = post_Columns;
	}
#endif
    }
    // When the console tabline appears or disappears the window positions
    // change.
    اگر (firstwin->w_winrow != tabline_height())
	shell_new_rows();	// recompute window positions and heights
}

#اگر defined(FEAT_GUI_TABLINE)
/*
 * Return TRUE if the GUI is taking care of the tabline.
 * It may still be hidden if 'showtabline' is zero.
 */
    صحیح
gui_use_tabline(تهی)
{
    بازگشت gui.in_use && vim_strchr(p_go, GO_TABLINE) != پوچ;
}

/*
 * Return TRUE if the GUI is showing the tabline.
 * This uses 'showtabline'.
 */
    ایستا صحیح
gui_has_tabline(تهی)
{
    اگر (!gui_use_tabline()
	    || p_stal == 0
	    || (p_stal == 1 && first_tabpage->tp_next == پوچ))
	بازگشت FALSE;
    بازگشت TRUE;
}

/*
 * Update the tabline.
 * This may display/undisplay the tabline and update the labels.
 */
    تهی
gui_update_tabline(تهی)
{
    صحیح	    showit = gui_has_tabline();
    صحیح	    shown = gui_mch_showing_tabline();

    اگر (!gui.starting && starting == 0)
    {
	// Updating the tabline uses direct GUI commands, flush
	// outstanding instructions first. (esp. clear screen)
	out_flush();

	اگر (!showit != !shown)
	{
	    // Save Rows/Columns before showing/hiding the tabline.
	    // gui_mch_show_tabline() processes GTK events (via
	    // gui_mch_update) which may trigger a spurious configure event
	    // that temporarily corrupts Rows.  Restore the original values so
	    // that gui_set_shellsize() uses the correct row/column count.
	    صحیح save_Rows = Rows;
	    صحیح save_Columns = Columns;

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
	اگر (showit != 0)
	    gui_mch_update_tabline();
    }
}

/*
 * Get the label or tooltip for tab page "tp" into NameBuff[].
 */
    تهی
get_tabline_label(
    tabpage_T	*tp,
    صحیح		tooltip)	// TRUE: get tooltip
{
    char_u	**opt;

    // Use 'guitablabel' or 'guitabtooltip' if it's set.
    opt = (tooltip ? &p_gtt : &p_gtl);
    اگر (**opt != NUL)
    {
	char_u	res[MAXPATHL];
	tabpage_T *save_curtab;
	char_u	*opt_name = (char_u *)(tooltip ? "guitabtooltip"
							     : "guitablabel");

	printer_page_رقم = tabpage_index(tp);
# ifdef FEAT_EVAL
	set_vim_var_nr(VV_LNUM, printer_page_رقم);
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
					   0, (صحیح)Columns, پوچ, پوچ, پوچ);
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
    اگر (**opt == NUL || *NameBuff == NUL)
    {
	win_T	*wp;
	صحیح	wincount = 0;
	صحیح	modified = FALSE;

	// Get the buffer name into NameBuff[] and shorten it.
	get_trans_bufname(tp == curtab ? curbuf : tp->tp_curwin->w_buffer);
	اگر (!tooltip)
	    shorten_dir(NameBuff);

	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    ++wincount;
	    اگر (bufIsChanged(wp->w_buffer))
		modified = TRUE;
	}

	اگر (modified || wincount > 1)
	{
	    char_u  buf[40] = "+ ";		// Tentatively assume modified only
	    size_t  buflen = 2;
	    size_t  NameBufflen = STRLEN(NameBuff);

	    اگر (wincount > 1)
		buflen = vim_snprintf_safelen(
		    (نویسه *)buf,
		    اندازه(buf),
		    "%d%s ",
		    wincount,
		    modified ? "+" : "");

	    // Make sure resulting NameBuff will not exceed its bounds.
	    اگر (NameBufflen + buflen < MAXPATHL)
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
    صحیح
send_tabline_event(صحیح nr)
{
    char_u string[3];

    اگر (nr == tabpage_index(curtab))
	بازگشت FALSE;

    // Don't put events in the input queue now.
    اگر (hold_gui_events || cmdwin_type != 0)
    {
	// Set it back to the current tab page.
	gui_mch_set_curtab(tabpage_index(curtab));
	بازگشت FALSE;
    }

    string[0] = CSI;
    string[1] = KS_TABLINE;
    string[2] = KE_FILLER;
    add_to_درون_داد_buf(string, 3);
    string[0] = nr;
    add_to_درون_داد_buf_csi(string, 1);
    بازگشت TRUE;
}

/*
 * Send a tabline menu event
 */
    تهی
send_tabline_menu_event(صحیح tabidx, صحیح event)
{
    char_u	    string[3];

    // Don't put events in the input queue now.
    اگر (hold_gui_events)
	بازگشت;

    // Cannot close the last tabpage.
    اگر (event == TABLINE_MENU_CLOSE && first_tabpage->tp_next == پوچ)
	بازگشت;

    string[0] = CSI;
    string[1] = KS_TABMENU;
    string[2] = KE_FILLER;
    add_to_درون_داد_buf(string, 3);
    string[0] = tabidx;
    string[1] = (char_u)(long)event;
    add_to_درون_داد_buf_csi(string, 2);
}

#endif

/*
 * Scrollbar stuff:
 */

/*
 * Remove all scrollbars.  Used before switching to another tab page.
 */
    تهی
gui_remove_scrollbars(تهی)
{
    صحیح	    i;
    win_T   *wp;

    برای (i = 0; i < 3; i++)
    {
	اگر (i == SBAR_BOTTOM)
	    gui_mch_enable_scrollbar(&gui.bottom_sbar, FALSE);
	وگرنه
	{
	    FOR_ALL_WINDOWS(wp)
		gui_do_scrollbar(wp, i, FALSE);
	}
	curtab->tp_prev_which_scrollbars[i] = -1;
    }
}

    تهی
gui_create_scrollbar(scrollbar_T *sb, صحیح type, win_T *wp)
{
    ایستا صحیح	sbar_ident = 0;

    sb->ident = sbar_ident++;	// No check for too big, but would it happen?
    sb->wp = wp;
    sb->type = type;
    sb->مقدار = 0;
    sb->size = 1;
    sb->max = 1;
    sb->بالاترین = 0;
    sb->height = 0;
    sb->width = 0;
    sb->status_height = 0;
    gui_mch_create_scrollbar(sb, (wp == پوچ) ? SBAR_HORIZ : SBAR_VERT);
}

/*
 * Find the scrollbar with the given index.
 */
    scrollbar_T *
gui_find_scrollbar(long ident)
{
    win_T	*wp;

    اگر (gui.bottom_sbar.ident == ident)
	بازگشت &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	اگر (wp->w_scrollbars[SBAR_چپ].ident == ident)
	    بازگشت &wp->w_scrollbars[SBAR_چپ];
	اگر (wp->w_scrollbars[SBAR_راست].ident == ident)
	    بازگشت &wp->w_scrollbars[SBAR_راست];
    }
    بازگشت پوچ;
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
    تهی
gui_drag_scrollbar(scrollbar_T *sb, long مقدار, صحیح still_dragging)
{
    win_T	*wp;
    صحیح		sb_رقم;
#ifdef USE_ON_FLY_SCROLL
    colnr_T	old_leftcol = curwin->w_leftcol;
    linenr_T	old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    صحیح		old_topfill = curwin->w_topfill;
# endif
#وگرنه
    char_u	bytes[اندازه(long_u)];
    صحیح		byte_شمار;
#endif

    اگر (sb == پوچ)
	بازگشت;

    // Don't put events in the input queue now.
    اگر (hold_gui_events)
	بازگشت;

    اگر (cmdwin_type != 0 && sb->wp != cmdwin_win)
	بازگشت;

    اگر (still_dragging)
    {
	اگر (sb->wp == پوچ)
	    gui.dragged_sb = SBAR_BOTTOM;
	وگرنه اگر (sb == &sb->wp->w_scrollbars[SBAR_چپ])
	    gui.dragged_sb = SBAR_چپ;
	وگرنه
	    gui.dragged_sb = SBAR_راست;
	gui.dragged_wp = sb->wp;
    }
    وگرنه
    {
	gui.dragged_sb = SBAR_NONE;
#ifdef FEAT_GUI_GTK
	// Keep the "dragged_wp" value until after the scrolling, for when the
	// mouse button is released.  GTK2 doesn't send the button-up event.
	gui.dragged_wp = پوچ;
	// WinScrolled event
	gui_focus_change(TRUE);
#endif
    }

    // Vertical sbar info is kept in the first sbar (the left one)
    اگر (sb->wp != پوچ)
	sb = &sb->wp->w_scrollbars[0];

    /*
     * Check validity of value
     */
    اگر (مقدار < 0)
	مقدار = 0;
#ifdef SCROLL_PAST_END
    وگرنه اگر (مقدار > sb->max)
	مقدار = sb->max;
#وگرنه
    اگر (مقدار > sb->max - sb->size + 1)
	مقدار = sb->max - sb->size + 1;
#endif

    sb->مقدار = مقدار;

#ifdef USE_ON_FLY_SCROLL
    // When not allowed to do the scrolling right now, return.
    // This also checked input_available(), but that causes the first click in
    // a scrollbar to be ignored when Vim doesn't have focus.
    اگر (dont_scroll)
	بازگشت;
#endif
    // Disallow scrolling the current window when the completion popup menu is
    // visible.
    اگر ((sb->wp == پوچ || sb->wp == curwin) && pum_visible())
	بازگشت;

#ifdef FEAT_RIGHTLEFT
    اگر (sb->wp == پوچ && curwin->w_p_rl)
    {
	مقدار = sb->max + 1 - sb->size - مقدار;
	اگر (مقدار < 0)
	    مقدار = 0;
    }
#endif

    اگر (sb->wp != پوچ)		// vertical scrollbar
    {
	sb_رقم = 0;
	برای (wp = firstwin; wp != sb->wp && wp != پوچ; wp = wp->w_next)
	    sb_رقم++;
	اگر (wp == پوچ)
	    بازگشت;

#ifdef USE_ON_FLY_SCROLL
	current_scrollbar = sb_رقم;
	scrollbar_مقدار = مقدار;
	اگر (State & MODE_NORMAL)
	{
	    gui_do_scroll();
	    setcursor();
	}
	وگرنه اگر (State & MODE_درج)
	{
	    ins_scroll();
	    setcursor();
	}
	وگرنه اگر (State & MODE_CMDLINE)
	{
	    اگر (پیام_scrolled == 0)
	    {
		gui_do_scroll();
		redrawcmdline();
	    }
	}
# ifdef FEAT_FOLDING
	// Value may have been changed for closed fold.
	sb->مقدار = sb->wp->w_topline - 1;
# endif

	// When dragging one scrollbar and there is another one at the other
	// side move the thumb of that one too.
	اگر (gui.which_scrollbars[SBAR_راست] && gui.which_scrollbars[SBAR_چپ])
	    gui_mch_set_scrollbar_thumb(
		    &sb->wp->w_scrollbars[
			    sb == &sb->wp->w_scrollbars[SBAR_راست]
						    ? SBAR_چپ : SBAR_راست],
		    sb->مقدار, sb->size, sb->max);

#وگرنه
	bytes[0] = CSI;
	bytes[1] = KS_VER_SCROLLBAR;
	bytes[2] = KE_FILLER;
	bytes[3] = (char_u)sb_رقم;
	byte_شمار = 4;
#endif
    }
    وگرنه
    {
#ifdef USE_ON_FLY_SCROLL
	scrollbar_مقدار = مقدار;

	اگر (State & MODE_NORMAL)
	    do_mousescroll_horiz(scrollbar_مقدار);
	وگرنه اگر (State & MODE_درج)
	    ins_horscroll();
	وگرنه اگر (State & MODE_CMDLINE)
	{
	    اگر (پیام_scrolled == 0)
	    {
		do_mousescroll_horiz(scrollbar_مقدار);
		redrawcmdline();
	    }
	}
	اگر (old_leftcol != curwin->w_leftcol)
	{
	    updateWindow(curwin);   // update window, status and cmdline
	    setcursor();
	}
#وگرنه
	bytes[0] = CSI;
	bytes[1] = KS_HOR_SCROLLBAR;
	bytes[2] = KE_FILLER;
	byte_شمار = 3;
#endif
    }

#ifdef USE_ON_FLY_SCROLL
    /*
     * synchronize other windows, as necessary according to 'scrollbind'
     */
    اگر (curwin->w_p_scb
	    && ((sb->wp == پوچ && curwin->w_leftcol != old_leftcol)
		|| (sb->wp == curwin && (curwin->w_topline != old_topline
# ifdef FEAT_DIFF
					   || curwin->w_topfill != old_topfill
# endif
			))))
    {
	do_check_scrollbind(TRUE);
	// need to update the window right here
	FOR_ALL_WINDOWS(wp)
	    اگر (wp->w_redr_type > 0)
		updateWindow(wp);
	setcursor();
    }
    out_flush_cursor(FALSE, TRUE);
#وگرنه
    add_to_درون_داد_buf(bytes, byte_شمار);
    add_long_to_buf((long_u)مقدار, bytes);
    add_to_درون_داد_buf_csi(bytes, اندازه(long_u));
#endif
}

/*
 * Scrollbar stuff:
 */

/*
 * Called when something in the window layout has changed.
 */
    تهی
gui_may_update_scrollbars(تهی)
{
    اگر (gui.in_use && starting == 0)
    {
	out_flush();
	gui_init_which_components(پوچ);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
}

    تهی
gui_update_scrollbars(
    صحیح		force)	    // Force all scrollbars to get updated
{
    win_T	*wp;
    scrollbar_T	*sb;
    long	val, size, max;		// need 32 bits here
    صحیح		which_sb;
    صحیح		h, y;
    ایستا win_T *prev_curwin = پوچ;

    // Update the horizontal scrollbar
    gui_update_horiz_scrollbar(force);

#ifndef MSWIN
    // Return straight away if there is neither a left nor right scrollbar.
    // On MS-Windows this is required anyway for scrollwheel messages.
    اگر (!gui.which_scrollbars[SBAR_چپ] && !gui.which_scrollbars[SBAR_راست])
	بازگشت;
#endif

    /*
     * Don't want to update a scrollbar while we're dragging it.  But if we
     * have both a left and right scrollbar, and we drag one of them, we still
     * need to update the other one.
     */
    اگر (!force && (gui.dragged_sb == SBAR_چپ || gui.dragged_sb == SBAR_راست)
	    && gui.which_scrollbars[SBAR_چپ]
	    && gui.which_scrollbars[SBAR_راست])
    {
	/*
	 * If we have two scrollbars and one of them is being dragged, just
	 * copy the scrollbar position from the dragged one to the other one.
	 */
	which_sb = SBAR_چپ + SBAR_راست - gui.dragged_sb;
	اگر (gui.dragged_wp != پوچ)
	    gui_mch_set_scrollbar_thumb(
		    &gui.dragged_wp->w_scrollbars[which_sb],
		    gui.dragged_wp->w_scrollbars[0].مقدار,
		    gui.dragged_wp->w_scrollbars[0].size,
		    gui.dragged_wp->w_scrollbars[0].max);
    }

    // avoid that moving components around generates events
    ++hold_gui_events;

    FOR_ALL_WINDOWS(wp)
    {
	اگر (wp->w_buffer == پوچ)	// just in case
	    ادامه;
	// Skip a scrollbar that is being dragged.
	اگر (!force && (gui.dragged_sb == SBAR_چپ
					     || gui.dragged_sb == SBAR_راست)
		&& gui.dragged_wp == wp)
	    ادامه;

#ifdef SCROLL_PAST_END
	max = wp->w_buffer->b_ml.ml_سطر_شمار - 1;
#وگرنه
	max = wp->w_buffer->b_ml.ml_سطر_شمار + wp->w_height - 2;
#endif
	اگر (max < 0)			// empty buffer
	    max = 0;
	val = wp->w_topline - 1;
	size = wp->w_height;
#ifdef SCROLL_PAST_END
	اگر (val > max)			// just in case
	    val = max;
#وگرنه
	اگر (size > max + 1)		// just in case
	    size = max + 1;
	اگر (val > max - size + 1)
	    val = max - size + 1;
#endif
	اگر (val < 0)			// minimal value is 0
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
	اگر (size < 1 || wp->w_botline - 2 > max)
	{
	    /*
	     * This can happen during changing files.  Just don't update the
	     * scrollbar for now.
	     */
	    sb->height = 0;	    // Force update next time
	    اگر (gui.which_scrollbars[SBAR_چپ])
		gui_do_scrollbar(wp, SBAR_چپ, FALSE);
	    اگر (gui.which_scrollbars[SBAR_راست])
		gui_do_scrollbar(wp, SBAR_راست, FALSE);
	    ادامه;
	}
	اگر (force || sb->height != wp->w_height
	    || sb->بالاترین != wp->w_winrow
	    || sb->status_height != wp->w_status_height
	    || sb->width != wp->w_width
	    || prev_curwin != curwin)
	{
	    // Height, width or position of scrollbar has changed.  For
	    // vertical split: curwin changed.
	    sb->height = wp->w_height;
	    sb->بالاترین = wp->w_winrow;
	    sb->status_height = wp->w_status_height;
	    sb->width = wp->w_width;

	    // Calculate height and position in pixels
	    h = (sb->height + sb->status_height) * gui.char_height;
	    y = sb->بالاترین * gui.char_height + gui.border_offset;
#اگر defined(FEAT_MENU) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF) && !defined(FEAT_GUI_PHOTON)
	    اگر (gui.menu_is_active)
		y += gui.menu_height;
#endif

#اگر defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU))
	    اگر (vim_strchr(p_go, GO_TOOLBAR) != پوچ)
		y += gui.toolbar_height;
#endif

#اگر defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
	    اگر (gui_has_tabline())
		y += gui.tabline_height;
#endif

	    اگر (wp->w_winrow == 0)
	    {
		// Height of top scrollbar includes width of top border
		h += gui.border_offset;
		y -= gui.border_offset;
	    }
	    اگر (gui.which_scrollbars[SBAR_چپ])
	    {
		gui_mch_set_scrollbar_pos(&wp->w_scrollbars[SBAR_چپ],
					  gui.چپ_sbar_x, y,
					  gui.scrollbar_width, h);
		gui_do_scrollbar(wp, SBAR_چپ, TRUE);
	    }
	    اگر (gui.which_scrollbars[SBAR_راست])
	    {
		gui_mch_set_scrollbar_pos(&wp->w_scrollbars[SBAR_راست],
					  gui.راست_sbar_x, y,
					  gui.scrollbar_width, h);
		gui_do_scrollbar(wp, SBAR_راست, TRUE);
	    }
	}

	اگر (force || sb->مقدار != val || sb->size != size || sb->max != max)
	{
	    // Thumb of scrollbar has moved
	    sb->مقدار = val;
	    sb->size = size;
	    sb->max = max;
	    اگر (gui.which_scrollbars[SBAR_چپ]
		    && (gui.dragged_sb != SBAR_چپ || gui.dragged_wp != wp))
		gui_mch_set_scrollbar_thumb(&wp->w_scrollbars[SBAR_چپ],
					    val, size, max);
	    اگر (gui.which_scrollbars[SBAR_راست]
		    && (gui.dragged_sb != SBAR_راست || gui.dragged_wp != wp))
		gui_mch_set_scrollbar_thumb(&wp->w_scrollbars[SBAR_راست],
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
    ایستا تهی
gui_do_scrollbar(
    win_T	*wp,
    صحیح		which,	    // SBAR_LEFT or SBAR_RIGHT
    صحیح		enable)	    // TRUE to enable scrollbar
{
    صحیح		midcol = curwin->w_wincol + curwin->w_width / 2;
    صحیح		has_midcol = (wp->w_wincol <= midcol
				     && wp->w_wincol + wp->w_width >= midcol);

    // Only enable scrollbars that contain the middle column of the current
    // window.
    اگر (gui.which_scrollbars[SBAR_راست] != gui.which_scrollbars[SBAR_چپ])
    {
	// Scrollbars only on one side.  Don't enable scrollbars that don't
	// contain the middle column of the current window.
	اگر (!has_midcol)
	    enable = FALSE;
    }
    وگرنه
    {
	// Scrollbars on both sides.  Don't enable scrollbars that neither
	// contain the middle column of the current window nor are on the far
	// side.
	اگر (midcol > Columns / 2)
	{
	    اگر (which == SBAR_چپ ? wp->w_wincol != 0 : !has_midcol)
		enable = FALSE;
	}
	وگرنه
	{
	    اگر (which == SBAR_راست ? wp->w_wincol + wp->w_width != Columns
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
    صحیح
gui_do_scroll(تهی)
{
    win_T	*wp, *save_wp;
    صحیح		i;
    long	nlines;
    pos_T	old_cursor;
    linenr_T	old_topline;
#ifdef FEAT_DIFF
    صحیح		old_topfill;
#endif

    برای (wp = firstwin, i = 0; i < current_scrollbar; wp = W_NEXT(wp), i++)
	اگر (wp == پوچ)
	    بشکن;
    اگر (wp == پوچ)
	// Couldn't find window
	بازگشت FALSE;
    // don't redraw, LineOffset and similar are not valid!
    اگر (exmode_active)
	بازگشت FALSE;

    /*
     * Compute number of lines to scroll.  If zero, nothing to do.
     */
    nlines = (long)scrollbar_مقدار + 1 - (long)wp->w_topline;
    اگر (nlines == 0)
	بازگشت FALSE;

    save_wp = curwin;
    old_topline = wp->w_topline;
#ifdef FEAT_DIFF
    old_topfill = wp->w_topfill;
#endif
    old_cursor = wp->w_cursor;
    curwin = wp;
    curbuf = wp->w_buffer;
    اگر (nlines < 0)
	scrolldown(-nlines, gui.dragged_wp == پوچ);
    وگرنه
	scrollup(nlines, gui.dragged_wp == پوچ);
    // Reset dragged_wp after using it.  "dragged_sb" will have been reset for
    // the mouse-up event already, but we still want it to behave like when
    // dragging.  But not the next click in an arrow.
    اگر (gui.dragged_sb == SBAR_NONE)
	gui.dragged_wp = پوچ;

    اگر (old_topline != wp->w_topline
#ifdef FEAT_DIFF
	    || old_topfill != wp->w_topfill
#endif
	    )
    {
	اگر (get_scrolloff_مقدار() != 0)
	{
	    cursor_correct();		// fix window for 'so'
	    update_topline();		// avoid up/down jump
	}
	اگر (old_cursor.lnum != wp->w_cursor.lnum)
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
    اگر (Screen_سطرها != پوچ && (old_topline != wp->w_topline
	    || wp->w_redr_type != 0
#ifdef FEAT_DIFF
	    || old_topfill != wp->w_topfill
#endif
	    ))
    {
	صحیح type = UPD_VALID;

	اگر (pum_visible())
	{
	    type = UPD_NOT_VALID;
	    wp->w_سطرها_valid = 0;
	}

	// Don't set must_redraw here, it may cause the popup menu to
	// disappear when losing focus after a scrollbar drag.
	اگر (wp->w_redr_type < type)
	    wp->w_redr_type = type;
	mch_disable_flush();
	updateWindow(wp);   // update window, status line, and cmdline
	mch_enable_flush();
    }

    // May need to redraw the popup menu.
    اگر (pum_visible())
	pum_redraw();

    بازگشت (wp == curwin && !EQUAL_POS(curwin->w_cursor, old_cursor));
}

/*
 * Horizontal scrollbar stuff:
 */
    ایستا تهی
gui_update_horiz_scrollbar(صحیح force)
{
    long	مقدار, size, max;

    اگر (!gui.which_scrollbars[SBAR_BOTTOM])
	بازگشت;

    اگر (!force && gui.dragged_sb == SBAR_BOTTOM)
	بازگشت;

    اگر (!force && curwin->w_p_wrap && gui.prev_wrap)
	بازگشت;

    /*
     * It is possible for the cursor to be invalid if we're in the middle of
     * something (like changing files).  If so, don't do anything for now.
     */
    اگر (curwin->w_cursor.lnum > curbuf->b_ml.ml_سطر_شمار)
    {
	gui.bottom_sbar.مقدار = -1;
	بازگشت;
    }

    size = curwin->w_width;
    اگر (curwin->w_p_wrap)
    {
	مقدار = 0;
#ifdef SCROLL_PAST_END
	max = 0;
#وگرنه
	max = curwin->w_width - 1;
#endif
    }
    وگرنه
    {
	مقدار = curwin->w_leftcol;
	max = scroll_سطر_len(ui_find_longest_lnum());

	اگر (virtual_active())
	{
	    // May move the cursor even further to the right.
	    اگر (curwin->w_virtcol >= (colnr_T)max)
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
    اگر (مقدار > max - size + 1)
	مقدار = max - size + 1;	    // limit the value to allowable range
#endif

#ifdef FEAT_RIGHTLEFT
    اگر (curwin->w_p_rl)
    {
	مقدار = max + 1 - size - مقدار;
	اگر (مقدار < 0)
	{
	    size += مقدار;
	    مقدار = 0;
	}
    }
#endif
    اگر (!force && مقدار == gui.bottom_sbar.مقدار && size == gui.bottom_sbar.size
						&& max == gui.bottom_sbar.max)
	بازگشت;

    gui.bottom_sbar.مقدار = مقدار;
    gui.bottom_sbar.size = size;
    gui.bottom_sbar.max = max;
    gui.prev_wrap = curwin->w_p_wrap;

    gui_mch_set_scrollbar_thumb(&gui.bottom_sbar, مقدار, size, max);
}

/*
 * Check that none of the colors are the same as the background color
 */
    تهی
gui_check_colors(تهی)
{
    اگر (gui.norm_pixel == gui.back_pixel || gui.norm_pixel == INVALCOLOR)
    {
	gui_set_bg_color((char_u *)"White");
	اگر (gui.norm_pixel == gui.back_pixel || gui.norm_pixel == INVALCOLOR)
	    gui_set_fg_color((char_u *)"Black");
    }
}

    ایستا تهی
gui_set_fg_color(char_u *name)
{
    gui.norm_pixel = gui_get_color(name);
    hl_set_fg_color_name(vim_strsave(name));
}

    ایستا تهی
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

    اگر (*name == NUL)
	بازگشت INVALCOLOR;
    t = gui_mch_get_color(name);

    صحیح is_none = STRCMP(name, "none") == 0;
    اگر (t == INVALCOLOR
#اگر defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
	    && (gui.in_use || is_none)
#endif
	    )
    {
	اگر (is_none)
	    emsg(_(e_cannot_use_color_none_did_you_mean_none));
	وگرنه
	    semsg(_(e_cannot_allocate_color_str), name);
    }
    بازگشت t;
}

/*
 * Return the grey value of a color (range 0-255).
 */
    صحیح
gui_get_lightness(guicolor_T pixel)
{
    long_u	rgb = (long_u)gui_mch_get_rgb(pixel);

    بازگشت  (صحیح)(  (((rgb >> 16) & 0xff) * 299)
		   + (((rgb >> 8) & 0xff) * 587)
		   +  ((rgb	  & 0xff) * 114)) / 1000;
}

    char_u *
gui_bg_default(تهی)
{
    اگر (gui_get_lightness(gui.back_pixel) < 127)
	بازگشت (char_u *)"dark";
    بازگشت (char_u *)"light";
}

/*
 * Option initializations that can only be done after opening the GUI window.
 */
    ایستا تهی
init_gui_options(تهی)
{
    // Set the 'background' option according to the lightness of the
    // background color, unless the user has set it already.
    اگر (!option_was_set((char_u *)"bg") && STRCMP(p_bg, gui_bg_default()) != 0)
    {
	set_option_مقدار_give_err((char_u *)"bg", 0L, gui_bg_default(), 0);
	highlight_changed();
    }
}

#اگر defined(FEAT_GUI_X11)
    تهی
gui_new_scrollbar_colors(تهی)
{
    win_T	*wp;

    // Nothing to do if GUI hasn't started yet.
    اگر (!gui.in_use)
	بازگشت;

    FOR_ALL_WINDOWS(wp)
    {
	gui_mch_set_scrollbar_colors(&(wp->w_scrollbars[SBAR_چپ]));
	gui_mch_set_scrollbar_colors(&(wp->w_scrollbars[SBAR_راست]));
    }
    gui_mch_set_scrollbar_colors(&gui.bottom_sbar);
}
#endif

/*
 * Call this when focus has changed.
 */
    تهی
gui_focus_change(صحیح in_focus)
{
/*
 * Skip this code to avoid drawing the cursor when debugging and switching
 * between the debugger window and gvim.
 */
#اگر 1
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
    اگر (!hold_gui_events
# اگر defined(FEAT_GUI_GTK) && defined(FEAT_GUI_DIALOG)
	    && gui.dialogs_active == 0
# endif
       )
    {
	char_u  bytes[3];

	bytes[0] = CSI;
	bytes[1] = KS_EXTRA;
	bytes[2] = in_focus ? (صحیح)KE_FOCUSGAINED : (صحیح)KE_FOCUSLOST;
	add_to_درون_داد_buf(bytes, 3);
    }
#endif
}

/*
 * When mouse moved: apply 'mousefocus'.
 * Also updates the mouse pointer shape.
 */
    ایستا تهی
gui_mouse_focus(صحیح x, صحیح y)
{
    win_T	*wp;
    char_u	st[8];

#ifdef FEAT_MOUSESHAPE
    // Get window pointer, and update mouse shape as well.
    wp = xy2win(x, y, IGNORE_POPUP);
#endif

    // Only handle this when 'mousefocus' set and ...
    اگر (p_mousef
	    && !hold_gui_events		// not holding events
	    && (State & (MODE_NORMAL | MODE_درج))// Normal/Visual/Insert mode
	    && State != MODE_HITRETURN	// but not hit-return prompt
	    && پیام_scrolled == 0	// no scrolled message
	    && !need_mouse_correct	// not moving the pointer
	    && gui.in_focus)		// gvim in focus
    {
	// Don't move the mouse when it's left or right of the Vim window
	اگر (x < 0 || x > Columns * gui.char_width)
	    بازگشت;
#ifndef FEAT_MOUSESHAPE
	wp = xy2win(x, y, IGNORE_POPUP);
#endif
	اگر (wp == curwin || wp == پوچ)
	    بازگشت;	// still in the same old window, or none at all

	// Ignore position in the tab pages line.
	اگر (Y_2_ROW(y) < tabline_height())
	    بازگشت;

	/*
	 * Format a mouse click on status line input,
	 * ala gui_send_mouse_event(0, x, y, 0, 0);
	 * Trick: Use a column number -1, so that get_pseudo_mouse_code() will
	 * generate a K_LEFTMOUSE_NM key code.
	 */
	اگر (finish_op)
	{
	    // abort the current operator first
	    st[0] = ESC;
	    add_to_درون_داد_buf(st, 1);
	}
	st[0] = CSI;
	st[1] = KS_MOUSE;
	st[2] = KE_FILLER;
	st[3] = (char_u)MOUSE_چپ;
	fill_mouse_coord(st + 4,
		wp->w_wincol == 0 ? -1 : wp->w_wincol + MOUSE_COLOFF,
		wp->w_height + W_WINROW(wp));

	add_to_درون_داد_buf(st, 8);
	st[3] = (char_u)MOUSE_RELEASE;
	add_to_درون_داد_buf(st, 8);
#ifdef FEAT_GUI_GTK
	// Need to wake up the main loop
	اگر (gtk_main_level() > 0)
	    gtk_main_quit();
#endif
    }
}

/*
 * Called when the mouse moved (but not when dragging).
 */
    تهی
gui_mouse_moved(صحیح x, صحیح y)
{
    // Ignore this while still starting up.
    اگر (!gui.in_use || gui.starting)
	بازگشت;

    // apply 'mousefocus' and pointer shape
    gui_mouse_focus(x, y);

    اگر (p_mousemev
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
    صحیح		x, y;

    اگر (!(gui.in_use && (p_mousef || popup == FIND_POPUP)))
	بازگشت پوچ;
    gui_mch_getmouse(&x, &y);

    // Only use the mouse when it's on the Vim window
    اگر (x >= 0 && x <= Columns * gui.char_width
	    && y >= 0 && Y_2_ROW(y) >= tabline_height())
	بازگشت xy2win(x, y, popup);
    بازگشت پوچ;
}

/*
 * Called when mouse should be moved to window with focus.
 */
    تهی
gui_mouse_correct(تهی)
{
    win_T	*wp = پوچ;

    need_mouse_correct = FALSE;

    wp = gui_mouse_window(IGNORE_POPUP);
    اگر (wp == curwin || wp == پوچ)
	بازگشت;

    // If in other than current window
    validate_cline_row();
    gui_mch_setmouse((صحیح)W_ENDCOL(curwin) * gui.char_width - 3,
	    (W_WINROW(curwin) + curwin->w_wrow) * gui.char_height
	    + (gui.char_height) / 2);
}

/*
 * Find window where the mouse pointer "x" / "y" coordinate is in.
 * As a side effect update the shape of the mouse pointer.
 */
    ایستا win_T *
xy2win(صحیح x, صحیح y, mouse_find_T popup)
{
    صحیح		row;
    صحیح		col;
    win_T	*wp;

    row = Y_2_ROW(y);
    col = X_2_COL(x);

    اگر (row < 0 || col < 0)		// before first window
	بازگشت پوچ;
    wp = mouse_find_win(&row, &col, popup);
    اگر (wp == پوچ)
    {
#ifdef FEAT_MOUSESHAPE
	update_mouseshape(-2);
#endif
	بازگشت پوچ;
    }
#ifdef FEAT_MOUSESHAPE
    اگر (State == MODE_HITRETURN || State == MODE_ASKMORE)
    {
	اگر (Y_2_ROW(y) >= پیام_row)
	    update_mouseshape(SHAPE_IDX_MOREL);
	وگرنه
	    update_mouseshape(SHAPE_IDX_MORE);
    }
    وگرنه اگر (row >= wp->w_height + wp->w_status_height)	// below status line
	update_mouseshape(SHAPE_IDX_CLINE);
    وگرنه اگر (!(State & MODE_CMDLINE) && wp->w_vsep_width > 0 && col == wp->w_width
	    && (!(row >= wp->w_height && row < wp->w_height
	    + wp->w_status_height) || !stl_connected(wp)) && پیام_scrolled == 0)
	update_mouseshape(SHAPE_IDX_VSEP);
    وگرنه اگر (!(State & MODE_CMDLINE) && wp->w_status_height > 0
	    && row >= wp->w_height && row < wp->w_height + wp->w_status_height
	    && پیام_scrolled == 0)
	update_mouseshape(SHAPE_IDX_STATUS);
    وگرنه
	update_mouseshape(-2);
#endif
    بازگشت wp;
}

/*
 * ":gui" and ":gvim": Change from the terminal version to the GUI version.
 * File names may be given to redefine the args list.
 */
    تهی
ex_gui(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    /*
     * Check for "-f" argument: foreground, don't fork.
     * Also don't fork when started with "gvim -f".
     * Do fork when using "gui -b".
     */
    اگر (arg[0] == '-'
	    && (arg[1] == 'f' || arg[1] == 'b')
	    && (arg[2] == NUL || VIM_ISWHITE(arg[2])))
    {
	gui.dofork = (arg[1] == 'b');
	eap->arg = skipwhite(eap->arg + 2);
    }
    اگر (!gui.in_use)
    {
#اگر defined(VIMDLL) && !defined(EXPERIMENTAL_GUI_CMD)
	اگر (!gui.starting)
	{
	    emsg(_(e_gui_cannot_be_used_not_enabled_at_compile_time));
	    بازگشت;
	}
#endif
	// Clear the command.  Needed for when forking+exiting, to avoid part
	// of the argument ending up after the shell prompt.
	پیام_clr_eos_force();
#ifdef GUI_MAY_SPAWN
	اگر (!ends_excmd2(eap->cmd, eap->arg))
	    gui_start(eap->arg);
	وگرنه
#endif
	    gui_start(پوچ);
#ifdef FEAT_JOB_CHANNEL
	channel_gui_register_all();
#endif
    }
    اگر (!ends_excmd2(eap->cmd, eap->arg))
	ex_next(eap);
}

#اگر (defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	    || defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON) \
	    || defined(FEAT_GUI_HAIKU)) \
	    && defined(FEAT_TOOLBAR)
/*
 * This is shared between Haiku, Motif, and GTK.
 */

/*
 * Callback function for do_in_runtimepath().
 */
    ایستا تهی
gfp_setname(char_u *fname, تهی *cookie)
{
    char_u	*gfp_buffer = cookie;

    اگر (STRLEN(fname) >= MAXPATHL)
	*gfp_buffer = NUL;
    وگرنه
	STRCPY(gfp_buffer, fname);
}

/*
 * Find the path of bitmap "name" with extension "ext" in 'runtimepath'.
 * Return FAIL for failure and OK if buffer[MAXPATHL] contains the result.
 */
    صحیح
gui_find_bitmap(char_u *name, char_u *buffer, نویسه *ext)
{
    اگر (STRLEN(name) > MAXPATHL - 14)
	بازگشت FAIL;
    vim_snprintf((نویسه *)buffer, MAXPATHL, "bitmaps/%s.%s", name, ext);
    اگر (do_in_runtimepath(buffer, 0, gfp_setname, buffer) == FAIL
							    || *buffer == NUL)
	بازگشت FAIL;
    بازگشت OK;
}

# اگر !defined(FEAT_GUI_GTK)
/*
 * Given the name of the "icon=" argument, try finding the bitmap file for the
 * icon.  If it is an absolute path name, use it as it is.  Otherwise append
 * "ext" and search for it in 'runtimepath'.
 * The result is put in "buffer[MAXPATHL]".  If something fails "buffer"
 * contains "name".
 */
    تهی
gui_find_iconfile(char_u *name, char_u *buffer, نویسه *ext)
{
    char_u	buf[MAXPATHL + 1];

    expand_env(name, buffer, MAXPATHL);
    اگر (!mch_isFullName(buffer) && gui_find_bitmap(buffer, buf, ext) == OK)
	STRCPY(buffer, buf);
}
# endif
#endif

#اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)|| defined(FEAT_GUI_HAIKU)
    تهی
display_errors(تهی)
{
    char_u	*p;

    اگر (isatty(2))
    {
	fflush(stderr);
	بازگشت;
    }

    اگر (error_ga.ga_data == پوچ)
	بازگشت;

    // avoid putting up a message box with blanks only
    برای (p = (char_u *)error_ga.ga_data; *p != NUL; ++p)
	اگر (!SAFE_isspace(*p))
	{
	    // Truncate a very long message, it will go off-screen.
	    اگر (STRLEN(p) > 2000)
		STRCPY(p + 2000 - 14, "...(truncated)");
	    (تهی)do_dialog(VIM_ERROR, (char_u *)_("Error"),
		    p, (char_u *)_("&Ok"), 1, پوچ, FALSE);
	    بشکن;
	}
    ga_clear(&error_ga);
}
#endif

#اگر defined(NO_CONSOLE_درون_داد)
/*
 * Return TRUE if still starting up and there is no place to enter text.
 * For GTK and X11 we check if stderr is not a tty, which means we were
 * (probably) started from the desktop.  Also check stdin, "vim >& file" does
 * allow typing on stdin.
 */
    صحیح
no_console_درون_داد(تهی)
{
    بازگشت ((!gui.in_use || gui.starting)
# ifndef NO_CONSOLE
	    && !isatty(0) && !isatty(2)
# endif
	    );
}
#endif

#اگر defined(FIND_REPLACE_DIALOG) \
	|| defined(NEED_GUI_UPDATE_SCREEN)
/*
 * Update the current window and the screen.
 */
    تهی
gui_update_screen(تهی)
{
# ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_سطر = 0;
    linenr_T	conceal_new_cursor_سطر = 0;
    صحیح		conceal_update_سطرها = FALSE;
# endif

    update_topline();
    validate_cursor();

    // Trigger CursorMoved if the cursor moved.
    اگر (!finish_op && (has_cursormoved()
# ifdef FEAT_PROP_POPUP
		|| popup_visible
# endif
# ifdef FEAT_CONCEAL
		|| curwin->w_p_cole > 0
# endif
		) && !EQUAL_POS(last_cursormoved, curwin->w_cursor))
    {
	اگر (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, پوچ, پوچ, FALSE, curbuf);
# ifdef FEAT_PROP_POPUP
	اگر (popup_visible)
	    popup_check_cursor_pos();
# endif
# ifdef FEAT_CONCEAL
	اگر (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_سطر = last_cursormoved.lnum;
	    conceal_new_cursor_سطر = curwin->w_cursor.lnum;
	    conceal_update_سطرها = TRUE;
	}
# endif
	last_cursormoved = curwin->w_cursor;
    }

    اگر (!finish_op)
	may_trigger_win_scrolled_resized();

# ifdef FEAT_CONCEAL
    اگر (conceal_update_سطرها
	    && (conceal_old_cursor_سطر != conceal_new_cursor_سطر
		|| conceal_cursor_سطر(curwin)
		|| need_cursor_سطر_redraw))
    {
	اگر (conceal_old_cursor_سطر != conceal_new_cursor_سطر)
	    redrawWinline(curwin, conceal_old_cursor_سطر);
	redrawWinline(curwin, conceal_new_cursor_سطر);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_سطر_redraw = FALSE;
    }
# endif
    update_screen(0);	// may need to update the screen
    setcursor();
    out_flush_cursor(TRUE, FALSE);
}
#endif

#اگر defined(FIND_REPLACE_DIALOG)
/*
 * Get the text to use in a find/replace dialog.  Uses the last search pattern
 * if the argument is empty.
 * Returns an allocated string.
 */
    char_u *
get_find_dialog_text(
    char_u	*arg,
    صحیح		*wwordp,	// return: TRUE if \< \> found
    صحیح		*mcasep)	// return: TRUE if \C found
{
    char_u	*text;

    اگر (*arg == NUL)
	text = last_جستجو_pat();
    وگرنه
	text = arg;
    اگر (text != پوچ)
    {
	صحیح len = (صحیح)STRLEN(text);

	text = vim_strnsave(text, len);
	اگر (text != پوچ)
	{
	    صحیح i;

	    // Remove "\V"
	    اگر (len >= 2 && STRNCMP(text, "\\V", 2) == 0)
	    {
		mch_memmove(text, text + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\c" and "\C" and remove.
	    اگر (len >= 2 && *text == '\\' && (text[1] == 'c' || text[1] == 'C'))
	    {
		*mcasep = (text[1] == 'C');
		mch_memmove(text, text + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\<text\>" and remove.
	    اگر (len >= 4
		    && STRNCMP(text, "\\<", 2) == 0
		    && STRNCMP(text + len - 2, "\\>", 2) == 0)
	    {
		*wwordp = TRUE;
		mch_memmove(text, text + 2, (size_t)(len - 4));
		text[len - 4] = NUL;
	    }

	    // Recognize "\/" or "\?" and remove.
	    برای (i = 0; i + 1 < len; ++i)
		اگر (text[i] == '\\' && (text[i + 1] == '/'
						       || text[i + 1] == '?'))
		{
		    mch_memmove(text + i, text + i + 1, (size_t)(len - i));
		    --len;
		}
	}
    }
    بازگشت text;
}

/*
 * Handle the press of a button in the find-replace dialog.
 * Return TRUE when something was added to the input buffer.
 */
    صحیح
gui_do_findrepl(
    صحیح		flags,		// one of FRD_REPLACE, FRD_FINDNEXT, etc.
    char_u	*find_text,
    char_u	*repl_text,
    صحیح		down)		// Search downwards.
{
    garray_T	ga;
    صحیح		i;
    صحیح		type = (flags & FRD_TYPE_MASK);
    char_u	*p;
    regmatch_T	regmatch;
    صحیح		save_did_emsg = did_emsg;
    ایستا صحیح  busy = FALSE;

    // When the screen is being updated we should not change buffers and
    // windows structures, it may cause freed memory to be used.  Also don't
    // do this recursively (pressing "Find" quickly several times).
    اگر (updating_screen || busy)
	بازگشت FALSE;

    // refuse replace when text cannot be changed
    اگر ((type == FRD_REPLACE || type == FRD_REPLACEALL) && text_locked())
	بازگشت FALSE;

    busy = TRUE;

    ga_init2(&ga, 1, 100);
    اگر (type == FRD_REPLACEALL)
	GA_CONCAT_LITERAL(&ga, "%s/");

    GA_CONCAT_LITERAL(&ga, "\\V");
    اگر (flags & FRD_MATCH_CASE)
	GA_CONCAT_LITERAL(&ga, "\\C");
    وگرنه
	GA_CONCAT_LITERAL(&ga, "\\c");
    اگر (flags & FRD_WHOLE_WORD)
	GA_CONCAT_LITERAL(&ga, "\\<");
    // escape slash and backslash
    p = vim_strsave_escaped(find_text, (char_u *)"/\\");
    اگر (p != پوچ)
	ga_concat(&ga, p);
    vim_free(p);
    اگر (flags & FRD_WHOLE_WORD)
	GA_CONCAT_LITERAL(&ga, "\\>");

    اگر (type == FRD_REPLACEALL)
    {
	GA_CONCAT_LITERAL(&ga, "/");
	// Escape slash and backslash.
	// Also escape tilde and ampersand if 'magic' is set.
	p = vim_strsave_escaped(repl_text,
				p_magic ? (char_u *)"/\\~&" : (char_u *)"/\\");
	اگر (p != پوچ)
	    ga_concat(&ga, p);
	vim_free(p);
	GA_CONCAT_LITERAL(&ga, "/g");
    }
    ga_append(&ga, NUL);

    اگر (type == FRD_REPLACE)
    {
	// Do the replacement when the text at the cursor matches.  Thus no
	// replacement is done if the cursor was moved!
	regmatch.regprog = vim_regcomp(ga.ga_data, RE_MAGIC + RE_STRING);
	regmatch.rm_ic = 0;
	اگر (regmatch.regprog != پوچ)
	{
	    p = ml_get_cursor();
	    اگر (vim_regexec_nl(&regmatch, p, (colnr_T)0)
						   && regmatch.startp[0] == p)
	    {
		// Clear the command line to remove any old "No match"
		// error.
		پیام_end_prompt();

		اگر (u_save_cursor() == OK)
		{
		    // A button was pressed thus undo should be synced.
		    u_sync(FALSE);

		    del_bytes((long)(regmatch.endp[0] - regmatch.startp[0]),
								FALSE, FALSE);
		    ins_str(repl_text, STRLEN(repl_text));
		}
	    }
	    وگرنه
		پیام(_("No match at cursor, finding next"));
	    vim_regfree(regmatch.regprog);
	}
    }

    اگر (type == FRD_REPLACEALL)
    {
	// A button was pressed, thus undo should be synced.
	u_sync(FALSE);
	do_cmdline_cmd(ga.ga_data);
    }
    وگرنه
    {
	صحیح searchflags = جستجو_پیام + جستجو_MARK;

	// Search for the next match.
	// Don't skip text under cursor for single replace.
	اگر (type == FRD_REPLACE)
	    searchflags += جستجو_START;
	i = پیام_scroll;
	اگر (down)
	{
	    (تهی)do_جستجو(پوچ, '/', '/', ga.ga_data, STRLEN(ga.ga_data), 1L, searchflags, پوچ);
	}
	وگرنه
	{
	    // We need to escape '?' if and only if we are searching in the up
	    // direction
	    p = vim_strsave_escaped(ga.ga_data, (char_u *)"?");
	    اگر (p != پوچ)
		(تهی)do_جستجو(پوچ, '?', '?', p, STRLEN(p), 1L, searchflags, پوچ);
	    vim_free(p);
	}

	پیام_scroll = i;	    // don't let an error message set msg_scroll
    }

    // Don't want to pass did_emsg to other code, it may cause disabling
    // syntax HL if we were busy redrawing.
    did_emsg = save_did_emsg;

    اگر (State & (MODE_NORMAL | MODE_درج))
    {
	gui_update_screen();		// update the screen
	پیام_didout = 0;			// overwrite any message
	need_wait_return = FALSE;	// don't wait for return
    }

    vim_free(ga.ga_data);
    busy = FALSE;
    بازگشت (ga.ga_len > 0);
}

#endif

#اگر defined(HAVE_DROP_FILE)
/*
 * Jump to the window at specified point (x, y).
 */
    ایستا تهی
gui_wingoto_xy(صحیح x, صحیح y)
{
    صحیح		row = Y_2_ROW(y);
    صحیح		col = X_2_COL(x);
    win_T	*wp;

    اگر (row < 0 || col < 0)
	بازگشت;

    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    اگر (wp != پوچ && wp != curwin)
	win_goto(wp);
}

/*
 * Function passed to handle_drop() for the actions to be done after the
 * argument list has been updated.
 */
    ایستا تهی
drop_callback(تهی *cookie)
{
    char_u	*p = cookie;
    صحیح		do_shorten = FALSE;

    // If Shift held down, change to first file's directory.  If the first
    // item is a directory, change to that directory (and let the explorer
    // plugin show the contents).
    اگر (p != پوچ)
    {
	اگر (mch_isdir(p))
	{
	    اگر (mch_chdir((نویسه *)p) == 0)
		do_shorten = TRUE;
	}
	وگرنه اگر (vim_chdirfile(p, "drop") == OK)
	    do_shorten = TRUE;
	vim_free(p);
	اگر (do_shorten)
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
    تهی
gui_handle_drop(
    صحیح		x UNUSED,
    صحیح		y UNUSED,
    int_u	modifiers,
    char_u	**fnames,
    صحیح		شمار)
{
    صحیح		i;
    char_u	*p;
    ایستا صحیح	entered = FALSE;

    /*
     * This function is called by event handlers.  Just in case we get a
     * second event before the first one is handled, ignore the second one.
     * Not sure if this can ever happen, just in case.
     */
    اگر (entered)
	بازگشت;
    entered = TRUE;

    /*
     * When the cursor is at the command line, add the file names to the
     * command line, don't edit the files.
     */
    اگر (State & MODE_CMDLINE)
    {
	shorten_filenames(fnames, شمار);
	برای (i = 0; i < شمار; ++i)
	{
	    اگر (fnames[i] != پوچ)
	    {
		اگر (i > 0)
		    add_to_درون_داد_buf((char_u*)" ", 1);

		// We don't know what command is used thus we can't be sure
		// about which characters need to be escaped.  Only escape the
		// most common ones.
# ifdef BACKSLASH_IN_FILENAME
		p = vim_strsave_escaped(fnames[i], (char_u *)" \t\"|");
# وگرنه
		p = vim_strsave_escaped(fnames[i], (char_u *)"\\ \t\"|");
# endif
		اگر (p != پوچ)
		    add_to_درون_داد_buf_csi(p, (صحیح)STRLEN(p));
		vim_free(p);
		vim_free(fnames[i]);
	    }
	}
	vim_free(fnames);
    }
    وگرنه
    {
	// Go to the window under mouse cursor, then shorten given "fnames" by
	// current window, because a window can have local current dir.
	gui_wingoto_xy(x, y);
	shorten_filenames(fnames, شمار);

	// If Shift held down, remember the first item.
	اگر ((modifiers & MOUSE_SHIFT) != 0)
	    p = vim_strsave(fnames[0]);
	وگرنه
	    p = پوچ;

	// Handle the drop, :edit or :split to get to the file.  This also
	// frees fnames[].  Skip this if there is only one item, it's a
	// directory and Shift is held down.
	اگر (شمار == 1 && (modifiers & MOUSE_SHIFT) != 0
						     && mch_isdir(fnames[0]))
	{
	    vim_free(fnames[0]);
	    vim_free(fnames);
	    vim_free(p);
	}
	وگرنه
	    handle_drop(شمار, fnames, (modifiers & MOUSE_CTRL) != 0,
						     drop_callback, (تهی *)p);
    }

    entered = FALSE;
}
#endif

/*
 * Check if "key" is to interrupt us.  Handles a key that has not had modifiers
 * applied yet.
 * Return the key with modifiers applied if so, NUL if not.
 */
    صحیح
check_for_interrupt(صحیح کلید, صحیح modifiers_arg)
{
    صحیح modifiers = modifiers_arg;
    صحیح c = merge_modify_Other_کلیدها(کلید, &modifiers);

    اگر ((c == Ctrl_C && ctrl_c_interrupts)
#ifdef UNIX
	    || (intr_char != Ctrl_C && c == intr_char)
#endif
	    )
    {
	got_int = TRUE;
	بازگشت c;
    }
    بازگشت NUL;
}

/*
 * If the "--gui-log-file fname" argument is given write the dialog title and
 * message to a file and return TRUE.  Otherwise return FALSE.
 * When there is any problem opening the file or writing to the file this is
 * ignored, showing the dialog might get the test to get stuck.
 */
    صحیح
gui_dialog_log(char_u *title, char_u *message)
{
    char_u  *fname = get_gui_dialog_file();
    FILE    *fd;

    اگر (fname == پوچ)
	بازگشت FALSE;

    fd = mch_fopen((نویسه *)fname, "a");
    اگر (fd != پوچ)
    {
	fprintf(fd, "%s: %s\n", title, message);
	fclose(fd);
    }
    بازگشت TRUE;
}
