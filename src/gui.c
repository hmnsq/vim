/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#include "vim.h"

// Structure containing all the GUI information
gui_T gui;

#ប្រសិនបើ defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
# define USE_SET_GUIFONTWIDE
static void set_guifontwide(char_u *font_name);
#endif
static void gui_check_pos(void);
static void gui_reset_scroll_region(void);
static void gui_outstr(char_u *, ចំនួនគត់);
#ifndef USE_GTK4
static ចំនួនគត់ gui_screenchar(ចំនួនគត់ off, ចំនួនគត់ flags, guicolor_T fg, guicolor_T bg, ចំនួនគត់ back);
#endif
static ចំនួនគត់ gui_outstr_nowrap(char_u *s, ចំនួនគត់ len, ចំនួនគត់ flags, guicolor_T fg, guicolor_T bg, ចំនួនគត់ back);
static void gui_delete_lines(ចំនួនគត់ row, ចំនួនគត់ count);
static void gui_insert_lines(ចំនួនគត់ row, ចំនួនគត់ count);
static ចំនួនគត់ gui_xy2colrow(ចំនួនគត់ x, ចំនួនគត់ y, ចំនួនគត់ *colp);
#ប្រសិនបើ defined(FEAT_GUI_TABLINE)
static ចំនួនគត់ gui_has_tabline(void);
#endif
static void gui_do_scrollbar(win_T *wp, ចំនួនគត់ which, ចំនួនគត់ enable);
static void gui_update_horiz_scrollbar(ចំនួនគត់);
static void gui_set_fg_color(char_u *name);
static void gui_set_bg_color(char_u *name);
static void init_gui_options(void);
static win_T *xy2win(ចំនួនគត់ x, ចំនួនគត់ y, mouse_find_T popup);

#ifdef GUI_MAY_FORK
static void gui_do_fork(void);

static ចំនួនគត់ gui_read_child_pipe(ចំនួនគត់ fd);

// Return values for gui_read_child_pipe
ga_nanaa {
    GUI_CHILD_IO_ERROR,
    GUI_CHILD_OK,
    GUI_CHILD_FAILED
};
#endif

static void gui_attempt_start(void);

static ចំនួនគត់ can_update_cursor = TRUE; // can display the cursor
static ចំនួនគត់ disable_flush = 0;	// If > 0, gui_mch_flush() is disabled.

/*
 * gui_start -- Called when user wants to start the GUI.
 *
 * Careful: This function can be called recursively when there is a ":gui"
 * command in the .gvimrc file.  Only the first call should fork, not the
 * recursive call.
 */
    void
gui_start(char_u *arg UNUSED)
{
    char_u	*old_term;
#ifdef GUI_MAY_FORK
    static ចំនួនគត់	recursive = 0;
#endif
#ប្រសិនបើ defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
    var_na	*msg = NULL;
#endif

    old_term = vim_strsave(T_NAME);

    settmode(TMODE_COOK);		// stop RAW mode
    ប្រសិនបើ (full_screen)
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
    ប្រសិនបើ (gui.dofork && !vim_strchr(p_go, GO_FORG) && recursive <= 1
# ifdef FEAT_JOB_CHANNEL
	    && !job_any_running()
# endif
	    )
    {
	gui_do_fork();
    }
    បើមិនដូច្នេះ
#endif
#ifdef GUI_MAY_SPAWN
    ប្រសិនបើ (gui.dospawn
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
	msg =
# endif
	    gui_mch_do_spawn(arg);
    }
    បើមិនដូច្នេះ
#endif
    {
#ifdef FEAT_GUI_GTK
	// If there is 'f' in 'guioptions' and specify -g argument,
	// gui_mch_init_check() was not called yet.
	ប្រសិនបើ (gui_mch_init_check() != OK)
	    getout_preserve_modified(1);
#endif
	gui_attempt_start();
    }

    ប្រសិនបើ (!gui.in_use)			// failed to start GUI
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
#ប្រសិនបើ defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
	ប្រសិនបើ (msg != NULL)
	    emsg(msg);
#endif
    }
    បើមិនដូច្នេះ
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

#ប្រសិនបើ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    // Enable fullscreen mode
    ប្រសិនបើ (vim_strchr(p_go, GO_FULLSCREEN) != NULL)
       gui_mch_set_fullscreen(TRUE);
#endif

    vim_free(old_term);

    // If the GUI started successfully, trigger the GUIEnter event, otherwise
    // the GUIFailed event.
    gui_mch_update();
    apply_autocmds(gui.in_use ? EVENT_GUIENTER : EVENT_GUIFAILED,
						   NULL, NULL, FALSE, curbuf);
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
    static void
gui_attempt_start(void)
{
    static ចំនួនគត់ recursive = 0;

    ++recursive;
    gui.starting = true;

#ifdef FEAT_GUI_GTK
    gui.event_time = GDK_CURRENT_TIME;
#endif

    termcapinit((char_u *)"builtin_gui");
    gui.starting = recursive - 1;

#ប្រសិនបើ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
    ប្រសិនបើ (gui.in_use)
    {
# ifdef FEAT_EVAL
	Window	x11_window;
	Display	*x11_display;

	ប្រសិនបើ (gui_get_x11_windis(&x11_window, &x11_display) == OK)
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
# ប្រសិនបើ defined(HAVE_SYS_WAIT_H)
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
    static void
gui_do_fork(void)
{
    ចំនួនគត់		pipefd[2];	// pipe between parent and child
    ចំនួនគត់		pipe_error;
    ចំនួនគត់		status;
    ចំនួនគត់		exit_status;
    pid_t	pid = -1;

# ប្រសិនបើ defined(FEAT_RELTIME) && defined(PROF_NSEC)
    // a timer is not carried forward
    delete_timer();
# endif

    // Setup a pipe between the child and the parent, so that the parent
    // knows when the child has done the setsid() call and is allowed to
    // exit.
    pipe_error = (pipe(pipefd) < 0);
    pid = fork();
    ប្រសិនបើ (pid < 0)	    // Fork error
    {
	emsg(_(e_failed_to_create_new_process_for_GUI));
	ត្រឡប់;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (pid > 0)	    // Parent
    {
	// Give the child some time to do the setsid(), otherwise the
	// exit() may kill the child too (when starting gvim from inside a
	// gvim).
	ប្រសិនបើ (!pipe_error)
	{
	    // The read returns when the child closes the pipe (or when
	    // the child dies for some reason).
	    close(pipefd[1]);
	    status = gui_read_child_pipe(pipefd[0]);
	    ប្រសិនបើ (status == GUI_CHILD_FAILED)
	    {
		// The child failed to start the GUI, so the caller must
		// continue. There may be more error information written
		// to stderr by the child.
		waitpid(pid, &exit_status, 0);
		emsg(_(e_the_child_process_failed_to_start_GUI));
		ត្រឡប់;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (status == GUI_CHILD_IO_ERROR)
	    {
		pipe_error = TRUE;
	    }
	    // else GUI_CHILD_OK: parent exit
	}

	ប្រសិនបើ (pipe_error)
	    ui_delay(301L, TRUE);

	// When swapping screens we may need to go to the next line, e.g.,
	// after a hit-enter prompt and using ":gui".
	ប្រសិនបើ (newline_on_exit)
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
    ប្រសិនបើ (gui_mch_init_check() != OK)
	getout_preserve_modified(1);
# endif

# ប្រសិនបើ defined(HAVE_SETSID) || defined(HAVE_SETPGID)
    /*
     * Change our process group.  On some systems/shells a CTRL-C in the
     * shell where Vim was started would otherwise kill gvim!
     */
#  ប្រសិនបើ defined(HAVE_SETSID)
    (void)setsid();
#  បើមិនដូច្នេះ
    (void)setpgid(0, 0);
#  endif
# endif
    ប្រសិនបើ (!pipe_error)
	close(pipefd[0]);

# ប្រសិនបើ defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
    // Tell the session manager our new PID
    gui_mch_forked();
# endif

    // Try to start the GUI
    gui_attempt_start();

    // Notify the parent
    ប្រសិនបើ (!pipe_error)
    {
	ប្រសិនបើ (gui.in_use)
	    write_eintr(pipefd[1], "ok", 3);
	បើមិនដូច្នេះ
	    write_eintr(pipefd[1], "fail", 5);
	close(pipefd[1]);
    }

    // If we failed to start the GUI, exit now.
    ប្រសិនបើ (!gui.in_use)
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
    static ចំនួនគត់
gui_read_child_pipe(ចំនួនគត់ fd)
{
    long	bytes_read;
# define READ_BUFFER_SIZE 10
    var_na	buffer[READ_BUFFER_SIZE];

    bytes_read = read_eintr(fd, buffer, READ_BUFFER_SIZE - 1);
# undef READ_BUFFER_SIZE
    close(fd);
    ប្រសិនបើ (bytes_read < 0)
	ត្រឡប់ GUI_CHILD_IO_ERROR;
    buffer[bytes_read] = NUL;
    ប្រសិនបើ (STRCMP(buffer, "ok") == 0)
	ត្រឡប់ GUI_CHILD_OK;
    ត្រឡប់ GUI_CHILD_FAILED;
}

#endif // GUI_MAY_FORK

/*
 * Call this when vim starts up, whether or not the GUI is started
 */
    void
gui_prepare(ចំនួនគត់ *argc, var_na **argv)
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
    ចំនួនគត់
gui_init_check(void)
{
    static ចំនួនគត់ result = MAYBE;

    ប្រសិនបើ (result != MAYBE)
    {
	ប្រសិនបើ (result == FAIL)
	    emsg(_(e_cannot_start_the_GUI));
	ត្រឡប់ result;
    }

    gui.shell_created = false;
    gui.dying = false;
    gui.in_focus = true;		// so the guicursor setting works
    gui.dragged_sb = SBAR_NONE;
    gui.dragged_wp = NULL;
    gui.pointer_hidden = false;
    gui.col = 0;
    gui.row = 0;
    gui.num_cols = Columns;
    gui.num_rows = Rows;

    gui.cursor_is_valid = false;
    gui.scroll_region_top = 0;
    gui.scroll_region_bot = Rows - 1;
    gui.scroll_region_left = 0;
    gui.scroll_region_right = Columns - 1;
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
#  បើមិនដូច្នេះ
    gui.menu_font = NOFONT;
#  endif
# endif
    gui.menu_is_active = true;	    // default: include menu
# ifndef FEAT_GUI_GTK
    gui.menu_height = MENU_DEFAULT_HEIGHT;
    gui.menu_width = 0;
# endif
#endif
#ប្រសិនបើ defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    gui.toolbar_height = 0;
#endif
#ifdef FEAT_BEVAL_TIP
    gui.tooltip_fontset = NOFONTSET;
#endif

    gui.scrollbar_width = gui.scrollbar_height = SB_DEFAULT_WIDTH;
    gui.prev_wrap = -1;

#ប្រសិនបើ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // Note: gui_set_ligatures() might already have been called e.g. from .vimrc,
    // and in that case we don't want to overwrite ligatures map that has already
    // been correctly populated (as that would lead to a cleared ligatures maps).
    ប្រសិនបើ (*p_guiligatures == NUL)
	CLEAR_FIELD(gui.ligatures_map);
#endif

#ប្រសិនបើ defined(ALWAYS_USE_GUI) || defined(VIMDLL)
    result = OK;
#បើមិនដូច្នេះ
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
# បើមិនដូច្នេះ
    result = gui_mch_init_check();
# endif
#endif
    ត្រឡប់ result;
}

/*
 * This is the call which starts the GUI.
 */
    void
gui_init(void)
{
    win_T	*wp;
    tabpage_T	*tp;
    static ចំនួនគត់	recursive = 0;

    /*
     * It's possible to use ":gui" in a .gvimrc file.  The first half of this
     * function will then be executed at the first call, the rest by the
     * recursive call.  This allow the shell to be opened halfway reading a
     * gvimrc file.
     */
    ប្រសិនបើ (!recursive)
    {
	++recursive;

	clip_init(TRUE);

	// If can't initialize, don't try doing the rest
	ប្រសិនបើ (gui_init_check() == FAIL)
	{
	    --recursive;
	    clip_init(FALSE);
	    ត្រឡប់;
	}

	/*
	 * Reset 'paste'.  It's useful in the terminal, but not in the GUI.  It
	 * breaks the Paste toolbar button.
	 */
	set_option_value_give_err((char_u *)"paste", 0L, NULL, 0);

	// Set t_Co to the number of colors: RGB.
	set_color_count(256 * 256 * 256);

	/*
	 * Set up system-wide default menus.
	 */
#ប្រសិនបើ defined(SYS_MENU_FILE) && defined(FEAT_MENU)
	ប្រសិនបើ (vim_strchr(p_go, GO_NOSYSMENU) == NULL)
	{
	    sys_menu = TRUE;
	    do_source((char_u *)SYS_MENU_FILE, FALSE, DOSO_NONE, NULL);
	    sys_menu = FALSE;
	}
#endif

	/*
	 * Switch on the mouse by default, unless the user changed it already.
	 * This can then be changed in the .gvimrc.
	 */
	ប្រសិនបើ (!option_was_set((char_u *)"mouse"))
	    set_string_option_direct((char_u *)"mouse", -1,
					   (char_u *)"a", OPT_FREE, SID_NONE);

	/*
	 * If -U option given, use only the initializations from that file and
	 * nothing else.  Skip all initializations for "-U NONE" or "-u NORC".
	 */
	ប្រសិនបើ (use_gvimrc != NULL)
	{
	    ប្រសិនបើ (STRCMP(use_gvimrc, "NONE") != 0
		    && STRCMP(use_gvimrc, "NORC") != 0
		    && do_source(use_gvimrc, FALSE, DOSO_NONE, NULL) != OK)
		semsg(_(e_cannot_read_from_str), use_gvimrc);
	}
	បើមិនដូច្នេះ
	{
	    /*
	     * Get system wide defaults for gvim, only when file name defined.
	     */
#ifdef SYS_GVIMRC_FILE
	    do_source((char_u *)SYS_GVIMRC_FILE, FALSE, DOSO_NONE, NULL);
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
	    ប្រសិនបើ (process_env((char_u *)"GVIMINIT", FALSE) == FAIL
		 && do_source((char_u *)USR_GVIMRC_FILE, TRUE,
						     DOSO_GVIMRC, NULL) == FAIL
#ifdef USR_GVIMRC_FILE2
		 && do_source((char_u *)USR_GVIMRC_FILE2, TRUE,
						     DOSO_GVIMRC, NULL) == FAIL
#endif
#ifdef USR_GVIMRC_FILE3
		 && do_source((char_u *)USR_GVIMRC_FILE3, TRUE,
						     DOSO_GVIMRC, NULL) == FAIL
#endif
				)
	    {
#ifdef USR_GVIMRC_FILE4
		(void)do_source((char_u *)USR_GVIMRC_FILE4, TRUE,
							    DOSO_GVIMRC, NULL);
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
	    ប្រសិនបើ (p_exrc)
	    {
#ifdef UNIX
		{
		    stat_T s;

		    // if ".gvimrc" file is not owned by user, set 'secure'
		    // mode
		    ប្រសិនបើ (mch_stat(GVIMRC_FILE, &s) || s.st_uid != getuid())
			secure = p_secure;
		}
#បើមិនដូច្នេះ
		secure = p_secure;
#endif

		ប្រសិនបើ (       fullpathcmp((char_u *)USR_GVIMRC_FILE,
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
		    do_source((char_u *)GVIMRC_FILE, TRUE, DOSO_GVIMRC, NULL);

		ប្រសិនបើ (secure == 2)
		    need_wait_return = TRUE;
		secure = 0;
	    }
	}

	ប្រសិនបើ (need_wait_return || msg_didany)
	    wait_return(TRUE);

	--recursive;
    }

    // If recursive call opened the shell, return here from the first call
    ប្រសិនបើ (gui.in_use)
	ត្រឡប់;

    /*
     * Create the GUI shell.
     */
    gui.in_use = true;		// Must be set after menus have been set up
#ប្រសិនបើ defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
    gui.scale = 1.0; // Default value
#endif
    ប្រសិនបើ (gui_mch_init() == FAIL)
	goto error;

    // Avoid a delay for an error message that was printed in the terminal
    // where Vim was started.
    emsg_on_display = FALSE;
    msg_scrolled = 0;
    clear_sb_text(TRUE);
    need_wait_return = FALSE;
    msg_didany = FALSE;

    /*
     * Check validity of any generic resources that may have been loaded.
     */
    ប្រសិនបើ (gui.border_width < 0)
	gui.border_width = 0;

    /*
     * Set up the fonts.  First use a font specified with "-fn" or "-font".
     */
    ប្រសិនបើ (font_argument != NULL)
	set_option_value_give_err((char_u *)"gfn",
					       0L, (char_u *)font_argument, 0);
    ប្រសិនបើ (
#ifdef FEAT_XFONTSET
	    (*p_guifontset == NUL
	     || gui_init_font(p_guifontset, TRUE) == FAIL) &&
#endif
	    gui_init_font(*p_guifont == NUL ? hl_get_font_name()
						  : p_guifont, FALSE) == FAIL)
    {
	emsg(_(e_cannot_start_gui_no_valid_font_found));
	goto error2;
    }
    ប្រសិនបើ (gui_get_wide_font() == FAIL)
	emsg(_(e_guifontwide_invalid));

    gui.num_cols = Columns;
    gui.num_rows = Rows;
    gui_reset_scroll_region();

    // Create initial scrollbars
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_LEFT], SBAR_LEFT, wp);
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_RIGHT], SBAR_RIGHT, wp);
    }
    gui_create_scrollbar(&gui.bottom_sbar, SBAR_BOTTOM, NULL);

#ifdef FEAT_MENU
    gui_create_initial_menus(root_menu);
#endif
#ifdef FEAT_SIGN_ICONS
    sign_gui_started();
#endif

    // Configure the desired menu and scrollbars
    gui_init_which_components(NULL);

    // All components of the GUI have been created now
    gui.shell_created = true;

#ifdef FEAT_GUI_MSWIN
    // Set the shell size, adjusted for the screen size.  For GTK this only
    // works after the shell has been opened, thus it is further down.
    // If the window is already maximized (e.g. when --windowid is passed in),
    // we want to use the system-provided dimensions by passing FALSE to
    // mustset. Otherwise, we want to initialize with the default rows/columns.
    ប្រសិនបើ (gui_mch_maximized())
	gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    បើមិនដូច្នេះ
	gui_set_shellsize(TRUE, TRUE, RESIZE_BOTH);
#បើមិនដូច្នេះ
# ifndef FEAT_GUI_GTK
    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
# endif
#endif
#ប្រសិនបើ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    // Need to set the size of the menubar after all the menus have been
    // created.
    gui_mch_compute_menu_height((Widget)0);
#endif

    /*
     * Actually open the GUI shell.
     */
    ប្រសិនបើ (gui_mch_open() != FAIL)
    {
	maketitle();
	resettitle();

	init_gui_options();
#ifdef FEAT_ARABIC
	// Our GUI can't do bidi.
	p_tbidi = FALSE;
#endif
#ប្រសិនបើ defined(FEAT_GUI_GTK)
	// Give GTK+ a chance to put all widget's into place.
	gui_mch_update();

# ifdef FEAT_MENU
	// If there is no 'm' in 'guioptions' we need to remove the menu now.
	// It was still there to make F10 work.
	ប្រសិនបើ (vim_strchr(p_go, GO_MENUS) == NULL)
	{
	    bool save_starting = gui.starting;

	    gui.starting = false;
	    gui_mch_enable_menu(FALSE);
	    gui.starting = save_starting;
	    gui_mch_update();
	}
# endif

	// Now make sure the shell fits on the screen.
	ប្រសិនបើ (gui_mch_maximized())
	    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
	បើមិនដូច្នេះ
	    gui_set_shellsize(TRUE, TRUE, RESIZE_BOTH);
#endif
	// When 'lines' was set while starting up the topframe may have to be
	// resized.
	win_new_shellsize();

#ifdef FEAT_BEVAL_GUI
	// Always create the Balloon Evaluation area, but disable it when
	// 'ballooneval' is off.
	ប្រសិនបើ (balloonEval != NULL)
	{
# ifdef FEAT_VARTABS
	    vim_free(balloonEval->vts);
# endif
	    vim_free(balloonEval);
	}
	balloonEvalForTerm = FALSE;
# ifdef FEAT_GUI_GTK
	balloonEval = gui_mch_create_beval_area(gui.drawarea, NULL,
						     &general_beval_cb, NULL);
# បើមិនដូច្នេះ
#  ប្រសិនបើ defined(FEAT_GUI_MOTIF)
	{
	    extern Widget	textArea;
	    balloonEval = gui_mch_create_beval_area(textArea, NULL,
						     &general_beval_cb, NULL);
	}
#  បើមិនដូច្នេះ
#   ifdef FEAT_GUI_MSWIN
	balloonEval = gui_mch_create_beval_area(NULL, NULL,
						     &general_beval_cb, NULL);
#   endif
#  endif
# endif
	ប្រសិនបើ (!p_beval)
	    gui_mch_disable_beval_area(balloonEval);
#endif

#ifndef FEAT_GUI_MSWIN
	// In the GUI modifiers are prepended to keys.
	// Don't do this for MS-Windows yet, it sends CTRL-K without the
	// modifier.
	seenModifyOtherKeys = TRUE;
#endif

#ប្រសិនបើ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	ប្រសិនបើ (!im_xim_isvalid_imactivate())
	    emsg(_(e_value_of_imactivatekey_is_invalid));
#endif
	// When 'cmdheight' was set during startup it may not have taken
	// effect yet.
	ប្រសិនបើ (p_ch != 1L)
	    command_height();

	ត្រឡប់;
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


    void
gui_exit(ចំនួនគត់ rc)
{
    // don't free the fonts, it leads to a BUS error
    // richard@whitequeen.com Jul 99
    free_highlight_fonts();
    gui.in_use = false;
    gui_mch_exit(rc);
}

#ប្រសិនបើ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_PHOTON)
# define NEED_GUI_UPDATE_SCREEN 1
/*
 * Called when the GUI shell is closed by the user.  If there are no changed
 * files Vim exits, otherwise there will be a dialog to ask the user what to
 * do.
 * When this function returns, Vim should NOT exit!
 */
    void
gui_shell_closed(void)
{
    cmdmod_T	    save_cmdmod = cmdmod;

    ប្រសិនបើ (before_quit_autocmds(curwin, TRUE, FALSE))
	ត្រឡប់;

    // Only exit when there are no changed files
    exiting = TRUE;
# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# ប្រសិនបើ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    cmdmod.cmod_flags |= CMOD_CONFIRM;
# endif
    // If there are changed buffers, present the user with a dialog if
    // possible, otherwise give an error message.
    ប្រសិនបើ (!check_changed_any(FALSE, FALSE))
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
    ចំនួនគត់
gui_init_font(char_u *font_list, ចំនួនគត់ fontset UNUSED)
{
#define FONTLEN 320
    char_u	font_name[FONTLEN];
    ចំនួនគត់		font_list_empty = FALSE;
    ចំនួនគត់		ret = FAIL;

    ប្រសិនបើ (!gui.in_use)
	ត្រឡប់ FAIL;

    font_name[0] = NUL;
    ប្រសិនបើ (*font_list == NUL)
	font_list_empty = TRUE;
    បើមិនដូច្នេះ
    {
#ifdef FEAT_XFONTSET
	// When using a fontset, the whole list of fonts is one name.
	ប្រសិនបើ (fontset)
	    ret = gui_mch_init_font(font_list, TRUE);
	បើមិនដូច្នេះ
#endif
	    ខណៈពេល (*font_list != NUL)
	    {
		// Isolate one comma separated font name.
		(void)copy_option_part(&font_list, font_name, FONTLEN, ",");

		// Careful!!!  The Win32 version of gui_mch_init_font(), when
		// called with "*" will change p_guifont to the selected font
		// name, which frees the old value.  This makes font_list
		// invalid.  Thus when OK is returned here, font_list must no
		// longer be used!
		ប្រសិនបើ (gui_mch_init_font(font_name, FALSE) == OK)
		{
#ifdef USE_SET_GUIFONTWIDE
		    // If it's a Unicode font, try setting 'guifontwide' to a
		    // similar double-width font.
		    ប្រសិនបើ ((p_guifontwide == NULL || *p_guifontwide == NUL)
				&& strstr((var_na *)font_name, "10646") != NULL)
			set_guifontwide(font_name);
#endif
		    ret = OK;
		    bha_nga;
		}
	    }
    }

    ប្រសិនបើ (ret != OK
	    && STRCMP(font_list, "*") != 0
	    && (font_list_empty || gui.norm_font == NOFONT))
    {
	/*
	 * Couldn't load any font in 'font_list', keep the current font if
	 * there is one.  If 'font_list' is empty, or if there is no current
	 * font, tell gui_mch_init_font() to try to find a font we can load.
	 */
	ret = gui_mch_init_font(NULL, FALSE);
    }

    ប្រសិនបើ (ret == OK)
    {
#ifndef FEAT_GUI_GTK
	// Set normal font as current font
# ifdef FEAT_XFONTSET
	ប្រសិនបើ (gui.fontset != NOFONTSET)
	    gui_mch_set_fontset(gui.fontset);
	បើមិនដូច្នេះ
# endif
	    gui_mch_set_font(gui.norm_font);
#endif
	gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    }

    ត្រឡប់ ret;
}

#ifdef USE_SET_GUIFONTWIDE
/*
 * Try setting 'guifontwide' to a font twice as wide as "name".
 */
    static void
set_guifontwide(char_u *name)
{
    ចំនួនគត់		i = 0;
    char_u	wide_name[FONTLEN + 10]; // room for 2 * width and '*'
    char_u	*wp = NULL;
    char_u	*p;
    GuiFont	font;

    wp = wide_name;
    សម្រាប់ (p = name; *p != NUL; ++p)
    {
	*wp++ = *p;
	ប្រសិនបើ (*p == '-')
	{
	    ++i;
	    ប្រសិនបើ (i == 6)		// font type: change "--" to "-*-"
	    {
		ប្រសិនបើ (p[1] == '-')
		    *wp++ = '*';
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (i == 12)	// found the width
	    {
		++p;
		i = getdigits(&p);
		ប្រសិនបើ (i != 0)
		{
		    // Double the width specification.
		    sprintf((var_na *)wp, "%d%s", i * 2, p);
		    font = gui_mch_get_font(wide_name, FALSE);
		    ប្រសិនបើ (font != NOFONT)
		    {
			gui_mch_free_font(gui.wide_font);
			gui.wide_font = font;
			set_string_option_direct((char_u *)"gfw", -1,
						      wide_name, OPT_FREE, 0);
		    }
		}
		bha_nga;
	    }
	}
    }
}
#endif

/*
 * Get the font for 'guifontwide'.
 * Return FAIL for an invalid font name.
 */
    ចំនួនគត់
gui_get_wide_font(void)
{
    GuiFont	font = NOFONT;
    char_u	font_name[FONTLEN];
    char_u	*p;

    ប្រសិនបើ (!gui.in_use)	    // Can't allocate font yet, assume it's OK.
	ត្រឡប់ OK;	    // Will give an error message later.

    ប្រសិនបើ (p_guifontwide != NULL && *p_guifontwide != NUL)
    {
	សម្រាប់ (p = p_guifontwide; *p != NUL; )
	{
	    // Isolate one comma separated font name.
	    (void)copy_option_part(&p, font_name, FONTLEN, ",");
	    font = gui_mch_get_font(font_name, FALSE);
	    ប្រសិនបើ (font != NOFONT)
		bha_nga;
	}
	ប្រសិនបើ (font == NOFONT)
	    ត្រឡប់ FAIL;
    }

    gui_mch_free_font(gui.wide_font);
#ifdef FEAT_GUI_GTK
    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    ប្រសិនបើ (font != NOFONT && gui.norm_font != NOFONT
			 && pango_font_description_equal(font, gui.norm_font))
    {
	gui.wide_font = NOFONT;
	gui_mch_free_font(font);
    }
    បើមិនដូច្នេះ
#endif
	gui.wide_font = font;
#ifdef FEAT_GUI_MSWIN
    gui_mch_wide_font_changed();
#បើមិនដូច្នេះ
    /*
     * TODO: setup wide_bold_font, wide_ital_font and wide_boldital_font to
     * support those fonts for 'guifontwide'.
     */
#endif
    ត្រឡប់ OK;
}

#ប្រសិនបើ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * Set list of ascii characters that combined can create ligature.
 * Store them in char map for quick access from gui_gtk_draw_string.
 */
    void
gui_set_ligatures(void)
{
    char_u	*p;

    ប្រសិនបើ (*p_guiligatures != NUL)
    {
	// check for invalid characters
	សម្រាប់ (p = p_guiligatures; *p != NUL; ++p)
	    ប្រសិនបើ (*p < 32 || *p > 127)
	    {
		emsg(_(e_ascii_code_not_in_range));
		ត្រឡប់;
	    }

	// store valid setting into ligatures_map
	CLEAR_FIELD(gui.ligatures_map);
	សម្រាប់ (p = p_guiligatures; *p != NUL; ++p)
	    gui.ligatures_map[*p] = 1;
    }
    បើមិនដូច្នេះ
	CLEAR_FIELD(gui.ligatures_map);
}

/*
 * Adjust the columns to undraw for when the cursor is on ligatures.
 */
    static void
gui_adjust_undraw_cursor_for_ligatures(ចំនួនគត់ *startcol, ចំនួនគត់ *endcol)
{
    ចំនួនគត់ off;

    ប្រសិនបើ (ScreenLines == NULL || *p_guiligatures == NUL)
	ត្រឡប់;

    // expand before the cursor for all the chars in gui.ligatures_map
    off = LineOffset[gui.cursor_row] + *startcol;
    ប្រសិនបើ (gui.ligatures_map[ScreenLines[off]])
	ខណៈពេល (*startcol > 0 && gui.ligatures_map[ScreenLines[--off]])
	    (*startcol)--;

    // expand after the cursor for all the chars in gui.ligatures_map
    off = LineOffset[gui.cursor_row] + *endcol;
    ប្រសិនបើ (gui.ligatures_map[ScreenLines[off]])
	ខណៈពេល (*endcol < ((ចំនួនគត់)screen_Columns - 1)
				      && gui.ligatures_map[ScreenLines[++off]])
	   (*endcol)++;
}
#endif

    static void
gui_set_cursor(ចំនួនគត់ row, ចំនួនគត់ col)
{
    gui.row = row;
    gui.col = col;
}

/*
 * gui_check_pos - check if the cursor is on the screen.
 */
    static void
gui_check_pos(void)
{
    ប្រសិនបើ (gui.row >= screen_Rows)
	gui.row = screen_Rows - 1;
    ប្រសិនបើ (gui.col >= screen_Columns)
	gui.col = screen_Columns - 1;
    ប្រសិនបើ (gui.cursor_row >= screen_Rows || gui.cursor_col >= screen_Columns)
	gui.cursor_is_valid = false;
}

/*
 * Redraw the cursor if necessary or when forced.
 * Careful: The contents of ScreenLines[] must match what is on the screen,
 * otherwise this goes wrong.  May need to call out_flush() first.
 */
    void
gui_update_cursor(
    ចំនួនគត់		force,		 // when TRUE, update even when not moved
    ចំនួនគត់		clear_selection) // clear selection under cursor
{
    ចំនួនគត់		cur_width = 0;
    ចំនួនគត់		cur_height = 0;
    ចំនួនគត់		old_hl_mask;
    cursorentry_T *shape;
    ចំនួនគត់		id;
#ifdef FEAT_TERMINAL
    guicolor_T	shape_fg = INVALCOLOR;
    guicolor_T	shape_bg = INVALCOLOR;
#endif
    guicolor_T	cfg, cbg, cc;	// cursor fore-/background color
    ចំនួនគត់		cattr;		// cursor attributes
    ចំនួនគត់		attr;
    attrentry_T *aep = NULL;
#ប្រសិនបើ (defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)) && !defined(USE_GTK4)
    bool	lig_left = false, lig_right = false;
#endif

    // Don't update the cursor when halfway busy scrolling or the screen size
    // doesn't match 'columns' and 'lines.  ScreenLines[] isn't valid then.
    ប្រសិនបើ (!can_update_cursor || screen_Columns != gui.num_cols
					       || screen_Rows != gui.num_rows)
	ត្រឡប់;

    gui_check_pos();

    ប្រសិនបើ (gui.cursor_is_valid && !force
		&& gui.row == gui.cursor_row && gui.col == gui.cursor_col)
	ត្រឡប់;

    gui_undraw_cursor();

    // If a cursor-less sleep is ongoing, leave the cursor invisible
    ប្រសិនបើ (cursor_is_sleeping())
	ត្រឡប់;

    ប្រសិនបើ (gui.row < 0)
	ត្រឡប់;
#ifdef HAVE_INPUT_METHOD
    ប្រសិនបើ (gui.row != gui.cursor_row || gui.col != gui.cursor_col)
	im_set_position(gui.row, gui.col);
#endif
    gui.cursor_row = gui.row;
    gui.cursor_col = gui.col;

    // Only write to the screen after ScreenLines[] has been initialized
    ប្រសិនបើ (!screen_cleared || ScreenLines == NULL)
	ត្រឡប់;

    // Clear the selection if we are about to write over it
    ប្រសិនបើ (clear_selection)
	clip_may_clear_selection(gui.row, gui.row);
    // Check that the cursor is inside the shell (resizing may have made
    // it invalid)
    ប្រសិនបើ (gui.row >= screen_Rows || gui.col >= screen_Columns)
	ត្រឡប់;

    gui.cursor_is_valid = true;

    /*
     * How the cursor is drawn depends on the current mode.
     * When in a terminal window use the shape/color specified there.
     */
#ifdef FEAT_TERMINAL
    ប្រសិនបើ (terminal_is_active())
	shape = term_get_cursor_shape(&shape_fg, &shape_bg);
    បើមិនដូច្នេះ
#endif
	shape = &shape_table[get_shape_idx(FALSE)];
    ប្រសិនបើ (State & MODE_LANGMAP)
	id = shape->id_lm;
    បើមិនដូច្នេះ
	id = shape->id;

    // get the colors and attributes for the cursor.  Default is inverted
    cfg = INVALCOLOR;
    cbg = INVALCOLOR;
    cattr = HL_INVERSE;
    gui_mch_set_blinking(shape->blinkwait,
	    shape->blinkon,
	    shape->blinkoff);
    ប្រសិនបើ (shape->blinkwait == 0 || shape->blinkon == 0
	    || shape->blinkoff == 0)
	gui_mch_stop_blink(FALSE);
#ifdef FEAT_TERMINAL
    ប្រសិនបើ (shape_bg != INVALCOLOR)
    {
	cattr = 0;
	cfg = shape_fg;
	cbg = shape_bg;
    }
    បើមិនដូច្នេះ
#endif
	ប្រសិនបើ (id > 0)
	{
	    cattr = syn_id2colors(id, &cfg, &cbg);
#ប្រសិនបើ defined(HAVE_INPUT_METHOD)
	    {
		static ចំនួនគត់ iid;
		guicolor_T fg, bg;

		ប្រសិនបើ (
# ប្រសិនបើ defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
			preedit_get_status()
# បើមិនដូច្នេះ
			im_get_status()
# endif
		   )
		{
		    iid = syn_name2id((char_u *)"CursorIM");
		    ប្រសិនបើ (iid > 0)
		    {
			syn_id2colors(iid, &fg, &bg);
			ប្រសិនបើ (bg != INVALCOLOR)
			    cbg = bg;
			ប្រសិនបើ (fg != INVALCOLOR)
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
    attr = ScreenAttrs[LineOffset[gui.row] + gui.col];
    ប្រសិនបើ (attr > HL_ALL)
	aep = syn_gui_attr2entry(attr);
    ប្រសិនបើ (aep != NULL)
    {
	attr = aep->ae_attr;
	ប្រសិនបើ (cfg == INVALCOLOR)
	    cfg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.bg_color
		    : aep->ae_u.gui.fg_color);
	ប្រសិនបើ (cbg == INVALCOLOR)
	    cbg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.fg_color
		    : aep->ae_u.gui.bg_color);
    }
    ប្រសិនបើ (cfg == INVALCOLOR)
	cfg = (attr & HL_INVERSE) ? gui.back_pixel : gui.norm_pixel;
    ប្រសិនបើ (cbg == INVALCOLOR)
	cbg = (attr & HL_INVERSE) ? gui.norm_pixel : gui.back_pixel;

#ifdef FEAT_XIM
    ប្រសិនបើ (aep != NULL)
    {
	xim_bg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.fg_color
		: aep->ae_u.gui.bg_color);
	xim_fg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.bg_color
		: aep->ae_u.gui.fg_color);
	ប្រសិនបើ (xim_bg_color == INVALCOLOR)
	    xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
		: gui.back_pixel;
	ប្រសិនបើ (xim_fg_color == INVALCOLOR)
	    xim_fg_color = (attr & HL_INVERSE) ? gui.back_pixel
		: gui.norm_pixel;
    }
    បើមិនដូច្នេះ
    {
	xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
	    : gui.back_pixel;
	xim_fg_color = (attr & HL_INVERSE) ? gui.back_pixel
	    : gui.norm_pixel;
    }
#endif

    attr &= ~HL_INVERSE;
    ប្រសិនបើ (cattr & HL_INVERSE)
    {
	cc = cbg;
	cbg = cfg;
	cfg = cc;
    }
    cattr &= ~HL_INVERSE;

    /*
     * When we don't have window focus, draw a hollow cursor.
     */
    ប្រសិនបើ (!gui.in_focus)
    {
#ifdef USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, -1, -1);
#បើមិនដូច្នេះ
	gui_mch_draw_hollow_cursor(cbg);
#endif
	ត្រឡប់;
    }

#ifdef USE_GTK4
    // Make sure that character underneath is drawn again in case it is part of
    // a ligature.
    gui_redraw_block(gui.row, gui.col, gui.row, gui.col, GUI_MON_NOCLEAR);
    // gui_redraw_block() will invalidate the cursor
    gui.cursor_is_valid = true;
#endif
#ប្រសិនបើ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // If cursor is in the middle of a ligature, then split the ligature at
    // the cursor boundaries by redrawing those cells again.
    សម្រាប់ (ចំនួនគត់ c = gui.col - 1; c >= 0; c--)
	ប្រសិនបើ (!gui.ligatures_map[ScreenLines[LineOffset[gui.row] + c]])
	{
	    ប្រសិនបើ (c < gui.col - 1)
	    {
		gui_redraw_block(gui.row, c + 1, gui.row, gui.col - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_left = true;
# endif
	    }
	    bha_nga;
	}

    សម្រាប់ (ចំនួនគត់ c = gui.col + 1; c < screen_Columns - 1; c++)
	ប្រសិនបើ (!gui.ligatures_map[ScreenLines[LineOffset[gui.row] + c]])
	{
	    ប្រសិនបើ (c > gui.col + 1)
	    {
		gui_redraw_block(gui.row, gui.col + 1, gui.row, c - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_right = true;
# endif
	    }
	    bha_nga;
	}

# ifndef USE_GTK4
    ប្រសិនបើ ((lig_left || lig_right) && shape->shape != SHAPE_BLOCK)
    {
	// Because the cursor is not drawn with gui_screenchar(), must blit the
	// cell again (with its background), so that old ligature does not
	// remain on screen. Not needed for GtkSnapshot, because char beneath is
	// always rerendered (see above ifdef).
	ចំនួនគត់ old = gui.col;
	gui.highlight_mask = ScreenAttrs[LineOffset[gui.row] + gui.col];
	(void)gui_screenchar(LineOffset[gui.row] + gui.col,
		GUI_MON_NOCLEAR, (guicolor_T)0, (guicolor_T)0, 0);
	gui.col = old;
    }
# endif
    // gui_redraw_block()/gui_screenchar() may invalidate the cursor, make sure
    // to validate it again.
    gui.cursor_is_valid = true;
#endif

    old_hl_mask = gui.highlight_mask;
    ប្រសិនបើ (shape->shape == SHAPE_BLOCK)
    {
#ifdef USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, 0, 0);
#បើមិនដូច្នេះ
	/*
	 * Draw the text character with the cursor colors.	Use the
	 * character attributes plus the cursor attributes.
	 */
	gui.highlight_mask = (cattr | attr);
	(void)gui_screenchar(LineOffset[gui.row] + gui.col,
		GUI_MON_IS_CURSOR | GUI_MON_NOCLEAR, cfg, cbg, 0);
#endif
    }
    បើមិនដូច្នេះ
    {
#ប្រសិនបើ defined(FEAT_RIGHTLEFT)
	ចំនួនគត់	    col_off = FALSE;
#endif
	/*
	 * First draw the partial cursor, then overwrite with the text
	 * character, using a transparent background.
	 */
	ប្រសិនបើ (shape->shape == SHAPE_VER)
	{
	    cur_height = gui.char_height;
	    cur_width = (gui.char_width * shape->percentage + 99) / 100;
	}
	បើមិនដូច្នេះ
	{
	    cur_height = (gui.char_height * shape->percentage + 99) / 100;
	    cur_width = gui.char_width;
	}
	ប្រសិនបើ (has_mbyte && (*mb_off2cells)(LineOffset[gui.row] + gui.col,
		    LineOffset[gui.row] + screen_Columns) > 1)
	{
	    // Double wide character.
	    ប្រសិនបើ (shape->shape != SHAPE_VER)
		cur_width += gui.char_width;
#ifdef FEAT_RIGHTLEFT
	    ប្រសិនបើ (CURSOR_BAR_RIGHT)
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
#បើមិនដូច្នេះ
	gui_mch_draw_part_cursor(cur_width, cur_height, cbg);
#endif
#ប្រសិនបើ defined(FEAT_RIGHTLEFT)
	ប្រសិនបើ (col_off)
	    --gui.col;
#endif

	// Doesn't seem to work for MSWindows. We call gui_redraw_block() above
	// for GtkSnapshot.
#ប្រសិនបើ !defined(FEAT_GUI_MSWIN) && !defined(USE_GTK4)
	gui.highlight_mask = ScreenAttrs[LineOffset[gui.row] + gui.col];
	(void)gui_screenchar(LineOffset[gui.row] + gui.col,
		GUI_MON_TRS_CURSOR | GUI_MON_NOCLEAR,
		(guicolor_T)0, (guicolor_T)0, 0);
#endif
    }
    gui.highlight_mask = old_hl_mask;
}

#ប្រសិនបើ defined(FEAT_MENU)
    static void
gui_position_menu(void)
{
# ប្រសិនបើ !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
    ប្រសិនបើ (gui.menu_is_active && gui.in_use)
	gui_mch_set_menu_pos(0, 0, gui.menu_width, gui.menu_height);
# endif
}
#endif

/*
 * Position the various GUI components (text area, menu).  The vertical
 * scrollbars are NOT handled here.  See gui_update_scrollbars().
 */
    static void
gui_position_components(ចំនួនគត់ total_width UNUSED)
{
    ចំនួនគត់	    text_area_x;
    ចំនួនគត់	    text_area_y;
    ចំនួនគត់	    text_area_width;
    ចំនួនគត់	    text_area_height;

    // avoid that moving components around generates events
    ++hold_gui_events;

    text_area_x = 0;
    ប្រសិនបើ (gui.which_scrollbars[SBAR_LEFT])
	text_area_x += gui.scrollbar_width;

    text_area_y = 0;
#ប្រសិនបើ defined(FEAT_MENU) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    gui.menu_width = total_width;
    ប្រសិនបើ (gui.menu_is_active)
	text_area_y += gui.menu_height;
#endif

#ប្រសិនបើ defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
    ប្រសិនបើ (gui_has_tabline())
	text_area_y += gui.tabline_height;
#endif

#ប្រសិនបើ defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    ប្រសិនបើ (vim_strchr(p_go, GO_TOOLBAR) != NULL)
    {
# ប្រសិនបើ defined(FEAT_GUI_HAIKU)
	gui_mch_set_toolbar_pos(0, text_area_y,
				gui.menu_width, gui.toolbar_height);
# endif
	text_area_y += gui.toolbar_height;
    }
#endif

#ប្រសិនបើ defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_HAIKU)
    gui_mch_set_tabline_pos(0, text_area_y,
    gui.menu_width, gui.tabline_height);
    ប្រសិនបើ (gui_has_tabline())
	text_area_y += gui.tabline_height;
#endif

    text_area_width = gui.num_cols * gui.char_width + gui.border_offset * 2;
    text_area_height = gui.num_rows * gui.char_height + gui.border_offset * 2;

    gui_mch_set_text_area_pos(text_area_x,
			      text_area_y,
			      text_area_width,
			      text_area_height
#ប្រសិនបើ defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
				  + xim_get_status_area_height()
#endif
			      );
#ifdef FEAT_MENU
    gui_position_menu();
#endif
    ប្រសិនបើ (gui.which_scrollbars[SBAR_BOTTOM])
	gui_mch_set_scrollbar_pos(&gui.bottom_sbar,
				  text_area_x,
				  text_area_y + text_area_height
					+ gui_mch_get_scrollbar_ypadding(),
				  text_area_width,
				  gui.scrollbar_height);
    gui.left_sbar_x = 0;
    gui.right_sbar_x = text_area_x + text_area_width
					+ gui_mch_get_scrollbar_xpadding();

    --hold_gui_events;
}

/*
 * Get the width of the widgets and decorations to the side of the text area.
 */
    ចំនួនគត់
gui_get_base_width(void)
{
    ចំនួនគត់	    base_width;

    base_width = 2 * gui.border_offset;
    ប្រសិនបើ (gui.which_scrollbars[SBAR_LEFT])
	base_width += gui.scrollbar_width;
    ប្រសិនបើ (gui.which_scrollbars[SBAR_RIGHT])
	base_width += gui.scrollbar_width;
    ត្រឡប់ base_width;
}

/*
 * Get the height of the widgets and decorations above and below the text area.
 */
    ចំនួនគត់
gui_get_base_height(void)
{
    ចំនួនគត់	    base_height;

    base_height = 2 * gui.border_offset;
    ប្រសិនបើ (gui.which_scrollbars[SBAR_BOTTOM])
	base_height += gui.scrollbar_height;
#ifdef FEAT_GUI_GTK
    // We can't take the sizes properly into account until anything is
    // realized.  Therefore we recalculate all the values here just before
    // setting the size. (--mdcki)
#បើមិនដូច្នេះ
# ifdef FEAT_MENU
    ប្រសិនបើ (gui.menu_is_active)
	base_height += gui.menu_height;
# endif
# ifdef FEAT_TOOLBAR
    ប្រសិនបើ (vim_strchr(p_go, GO_TOOLBAR) != NULL)
	base_height += gui.toolbar_height;
# endif
# ប្រសិនបើ defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    ប្រសិនបើ (gui_has_tabline())
	base_height += gui.tabline_height;
# endif
# ប្រសិនបើ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    base_height += gui_mch_text_area_extra_height();
# endif
#endif
    ត្រឡប់ base_height;
}

/*
 * Should be called after the GUI shell has been resized.  Its arguments are
 * the new width and height of the shell in pixels.
 */
    void
gui_resize_shell(ចំនួនគត់ pixel_width, ចំនួនគត់ pixel_height)
{
    static ចំនួនគត់	busy = FALSE;

    ប្រសិនបើ (!gui.shell_created)	    // ignore when still initializing
	ត្រឡប់;

    /*
     * Can't resize the screen while it is being redrawn.  Remember the new
     * size and handle it later.
     */
    ប្រសិនបើ (updating_screen || busy)
    {
	new_pixel_width = pixel_width;
	new_pixel_height = pixel_height;
	ត្រឡប់;
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
#ប្រសិនបើ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk_init_decor_height();
#endif

    gui.num_cols = (pixel_width - gui_get_base_width()) / gui.char_width;
    gui.num_rows = (pixel_height - gui_get_base_height()) / gui.char_height;

#ifdef USE_GTK4
    gui_gtk4_update_size();
#endif

    gui_position_components(pixel_width);
    gui_reset_scroll_region();

#ប្រសិនបើ defined(FEAT_GUI_GTK) && defined(USE_GTK4) && !defined(USE_GTK4)
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
    ប្រសិនបើ (State == MODE_ASKMORE || State == MODE_CONFIRM)
	gui.row = gui.num_rows;

    // Only comparing Rows and Columns may be sufficient, but let's stay on
    // the safe side.
    ប្រសិនបើ (gui.num_rows != screen_Rows || gui.num_cols != screen_Columns
	    || gui.num_rows != Rows || gui.num_cols != Columns || gui.force_redraw)
    {
	shell_resized();
	gui.force_redraw = 0;
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    gui_update_scrollbars(TRUE);
    gui_update_cursor(FALSE, TRUE);
#ប្រសិនបើ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(pixel_width, pixel_height);
#endif
#ប្រសិនបើ defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
    xim_set_status_area();
#endif

    busy = FALSE;

    // We may have been called again while redrawing the screen.
    // Need to do it all again with the latest size then.  But only if the size
    // actually changed.
    ប្រសិនបើ (new_pixel_height)
    {
	ប្រសិនបើ (pixel_width == new_pixel_width && pixel_height == new_pixel_height)
	{
	    new_pixel_width = 0;
	    new_pixel_height = 0;
	}
	បើមិនដូច្នេះ
	{
	    pixel_width = new_pixel_width;
	    pixel_height = new_pixel_height;
	    goto again;
	}
    }
}

/*
 * Check if gui_resize_shell() must be called.
 */
    void
gui_may_resize_shell(void)
{
    ប្រសិនបើ (new_pixel_height)
    {
	// careful: gui_resize_shell() may postpone the resize again if we
	// were called indirectly by it
	gui_resize_shell(new_pixel_width, new_pixel_height);
    }
}

    ចំនួនគត់
gui_get_shellsize(void)
{
    Rows = gui.num_rows;
    Columns = gui.num_cols;
    ត្រឡប់ OK;
}

/*
 * Set the size of the Vim shell according to Rows and Columns.
 * If "fit_to_display" is TRUE then the size may be reduced to fit the window
 * on the screen.
 * When "mustset" is TRUE the size was set by the user. When FALSE a UI
 * component was added or removed (e.g., a scrollbar).
 */
    void
gui_set_shellsize(
    ចំនួនគត់		mustset UNUSED,
    ចំនួនគត់		fit_to_display,
    ចំនួនគត់		direction)		// RESIZE_HOR, RESIZE_VER
{
    ចំនួនគត់		base_width;
    ចំនួនគត់		base_height;
    ចំនួនគត់		width;
    ចំនួនគត់		height;
    ចំនួនគត់		min_width;
    ចំនួនគត់		min_height;
    ចំនួនគត់		screen_w;
    ចំនួនគត់		screen_h;
#ifdef FEAT_GUI_GTK
    ចំនួនគត់		un_maximize = mustset;
    ចំនួនគត់		did_adjust = 0;
#endif
    ចំនួនគត់		x = -1, y = -1;

    ប្រសិនបើ (!gui.shell_created)
	ត្រឡប់;

#ប្រសិនបើ defined(MSWIN) || defined(FEAT_GUI_GTK)
    // If not setting to a user specified size and maximized, calculate the
    // number of characters that fit in the maximized window.
    // FIXME: gui_mch_newfont() is called here even when the font hasn't
    // changed at all.  For example, ":set guioptions=k" triggers this path
    // via gui_init_which_components() -> gui_set_shellsize(FALSE, ...).
    // The intent is to keep the window size and recalculate Rows/Columns,
    // which has nothing to do with fonts.  This should be a separate
    // function with a more descriptive name.
    ប្រសិនបើ (!mustset && (vim_strchr(p_go, GO_KEEPWINSIZE) != NULL
						       || gui_mch_maximized()))
    {
	gui_mch_newfont();
	ត្រឡប់;
    }
#endif

    base_width = gui_get_base_width();
    base_height = gui_get_base_height();
    ប្រសិនបើ (fit_to_display)
	// Remember the original window position.
	(void)gui_mch_get_winpos(&x, &y);

    width = Columns * gui.char_width + base_width;
    height = Rows * gui.char_height + base_height;

    ប្រសិនបើ (fit_to_display)
    {
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	ប្រសិនបើ ((direction & RESIZE_HOR) && width > screen_w)
	{
	    Columns = (screen_w - base_width) / gui.char_width;
	    ប្រសិនបើ (Columns < MIN_COLUMNS)
		Columns = MIN_COLUMNS;
	    width = Columns * gui.char_width + base_width;
#ifdef FEAT_GUI_GTK
	    ++did_adjust;
#endif
	}
	ប្រសិនបើ ((direction & RESIZE_VERT) && height > screen_h)
	{
	    Rows = (screen_h - base_height) / gui.char_height;
	    check_shellsize();
	    height = Rows * gui.char_height + base_height;
#ifdef FEAT_GUI_GTK
	    ++did_adjust;
#endif
	}
#ifdef FEAT_GUI_GTK
	ប្រសិនបើ (did_adjust == 2 || (width + gui.char_width >= screen_w
				     && height + gui.char_height >= screen_h))
	    // don't unmaximize if at maximum size
	    un_maximize = FALSE;
#endif
    }
    limit_screen_size();
    gui.num_cols = Columns;
    gui.num_rows = Rows;
#ifdef USE_GTK4
    // Keep the drawing area in sync with the size Vim is going to draw.
    gui_gtk4_update_size();
#endif

    min_width = base_width + MIN_COLUMNS * gui.char_width;
    min_height = base_height + MIN_LINES * gui.char_height;
    min_height += tabline_height() * gui.char_height;

#ifdef FEAT_GUI_GTK
    ប្រសិនបើ (un_maximize)
    {
	// If the window size is smaller than the screen unmaximize the
	// window, otherwise resizing won't work.
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	ប្រសិនបើ ((width + gui.char_width < screen_w
				   || height + gui.char_height * 2 < screen_h)
		&& gui_mch_maximized())
	    gui_mch_unmaximize();
    }
#endif

    gui_mch_set_shellsize(width, height, min_width, min_height,
					  base_width, base_height, direction);

    ប្រសិនបើ (fit_to_display && x >= 0 && y >= 0)
    {
	// Some window managers put the Vim window left of/above the screen.
	// Only change the position if it wasn't already negative before
	// (happens on MS-Windows with a secondary monitor).
	gui_mch_update();
	ប្រសិនបើ (gui_mch_get_winpos(&x, &y) == OK && (x < 0 || y < 0))
	    gui_mch_set_winpos(x < 0 ? 0 : x, y < 0 ? 0 : y);
    }

    gui_position_components(width);
    gui_update_scrollbars(TRUE);
    gui_reset_scroll_region();

#ប្រសិនបើ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(width, height);
#endif
}

/*
 * Called when Rows and/or Columns has changed.
 */
    void
gui_new_shellsize(void)
{
    gui_reset_scroll_region();
}

/*
 * Make scroll region cover whole screen.
 */
    static void
gui_reset_scroll_region(void)
{
    gui.scroll_region_top = 0;
    gui.scroll_region_bot = gui.num_rows - 1;
    gui.scroll_region_left = 0;
    gui.scroll_region_right = gui.num_cols - 1;
}

    static void
gui_start_highlight(ចំនួនគត់ mask)
{
    ប្រសិនបើ (mask > HL_ALL)		    // highlight code
	gui.highlight_mask = mask;
    បើមិនដូច្នេះ			    // mask
	gui.highlight_mask |= mask;
}

    void
gui_stop_highlight(ចំនួនគត់ mask)
{
    ប្រសិនបើ (mask > HL_ALL)		    // highlight code
	gui.highlight_mask = HL_NORMAL;
    បើមិនដូច្នេះ			    // mask
	gui.highlight_mask &= ~mask;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    void
gui_clear_block(
    ចំនួនគត់	    row1,
    ចំនួនគត់	    col1,
    ចំនួនគត់	    row2,
    ចំនួនគត់	    col2)
{
    // Clear the selection if we are about to write over it
    clip_may_clear_selection(row1, row2);

    gui_mch_clear_block(row1, col1, row2, col2);

    // Invalidate cursor if it was in this block
    ប្រសិនបើ (       gui.cursor_row >= row1 && gui.cursor_row <= row2
	    && gui.cursor_col >= col1 && gui.cursor_col <= col2)
	gui.cursor_is_valid = false;
}

/*
 * Write code to update the cursor later.  This avoids the need to flush the
 * output buffer before calling gui_update_cursor().
 */
    void
gui_update_cursor_later(void)
{
    OUT_STR("\033|s");
}

    void
gui_write(
    char_u	*s,
    ចំនួនគត់		len)
{
    char_u	*p;
    ចំនួនគត់		arg1 = 0, arg2 = 0;
    ចំនួនគត់		force_cursor = FALSE;	// force cursor update
    ចំនួនគត់		force_scrollbar = FALSE;
    static win_T	*old_curwin = NULL;

// #define DEBUG_GUI_WRITE
#ifdef DEBUG_GUI_WRITE
    {
	ចំនួនគត់ i;
	char_u *str;

	printf("gui_write(%d):\n    ", len);
	សម្រាប់ (i = 0; i < len; i++)
	    ប្រសិនបើ (s[i] == ESC)
	    {
		ប្រសិនបើ (i != 0)
		    printf("\n    ");
		printf("<ESC>");
	    }
	    បើមិនដូច្នេះ
	    {
		str = transchar_byte(s[i]);
		ប្រសិនបើ (str[0] && str[1])
		    printf("<%s>", (var_na *)str);
		បើមិនដូច្នេះ
		    printf("%s", (var_na *)str);
	    }
	printf("\n");
    }
#endif
    ខណៈពេល (len)
    {
	ប្រសិនបើ (s[0] == ESC && s[1] == '|')
	{
	    p = s + 2;
	    ប្រសិនបើ (VIM_ISDIGIT(*p) || (*p == '-' && VIM_ISDIGIT(*(p + 1))))
	    {
		arg1 = getdigits(&p);
		ប្រសិនបើ (p > s + len)
		    bha_nga;
		ប្រសិនបើ (*p == ';')
		{
		    ++p;
		    arg2 = getdigits(&p);
		    ប្រសិនបើ (p > s + len)
			bha_nga;
		}
	    }
	    switch (*p)
	    {
		pak_sa 'C':	// Clear screen
		    clip_scroll_selection(9999);
		    gui_mch_clear_all();
		    gui.cursor_is_valid = false;
		    force_scrollbar = TRUE;
		    bha_nga;
		pak_sa 'M':	// Move cursor
		    gui_set_cursor(arg1, arg2);
		    bha_nga;
		pak_sa 's':	// force cursor (shape) update
		    force_cursor = TRUE;
		    bha_nga;
		pak_sa 'R':	// Set scroll region
		    ប្រសិនបើ (arg1 < arg2)
		    {
			gui.scroll_region_top = arg1;
			gui.scroll_region_bot = arg2;
		    }
		    បើមិនដូច្នេះ
		    {
			gui.scroll_region_top = arg2;
			gui.scroll_region_bot = arg1;
		    }
		    bha_nga;
		pak_sa 'V':	// Set vertical scroll region
		    ប្រសិនបើ (arg1 < arg2)
		    {
			gui.scroll_region_left = arg1;
			gui.scroll_region_right = arg2;
		    }
		    បើមិនដូច្នេះ
		    {
			gui.scroll_region_left = arg2;
			gui.scroll_region_right = arg1;
		    }
		    bha_nga;
		pak_sa 'd':	// Delete line
		    gui_delete_lines(gui.row, 1);
		    bha_nga;
		pak_sa 'D':	// Delete lines
		    gui_delete_lines(gui.row, arg1);
		    bha_nga;
		pak_sa 'i':	// Insert line
		    gui_insert_lines(gui.row, 1);
		    bha_nga;
		pak_sa 'I':	// Insert lines
		    gui_insert_lines(gui.row, arg1);
		    bha_nga;
		pak_sa '$':	// Clear to end-of-line
		    gui_clear_block(gui.row, gui.col, gui.row,
							    (ចំនួនគត់)Columns - 1);
		    bha_nga;
		pak_sa 'h':	// Turn on highlighting
		    gui_start_highlight(arg1);
		    bha_nga;
		pak_sa 'H':	// Turn off highlighting
		    gui_stop_highlight(arg1);
		    bha_nga;
		pak_sa 'f':	// flash the window (visual bell)
		    gui_mch_flash(arg1 == 0 ? 20 : arg1);
		    bha_nga;
		default:
		    p = s + 1;	// Skip the ESC
		    bha_nga;
	    }
	    len -= (ចំនួនគត់)(++p - s);
	    s = p;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (s[0] < 0x20		// Ctrl character
#ifdef FEAT_SIGN_ICONS
		&& s[0] != SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
		&& s[0] != MULTISIGN_BYTE
# endif
#endif
		)
	{
	    ប្រសិនបើ (s[0] == '\n')		// NL
	    {
		gui.col = 0;
		ប្រសិនបើ (gui.row < gui.scroll_region_bot)
		    gui.row++;
		បើមិនដូច្នេះ
		    gui_delete_lines(gui.scroll_region_top, 1);
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (s[0] == '\r')	// CR
	    {
		gui.col = 0;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (s[0] == '\b')	// Backspace
	    {
		ប្រសិនបើ (gui.col)
		    --gui.col;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (s[0] == Ctrl_L)	// cursor-right
	    {
		++gui.col;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (s[0] == Ctrl_G)	// Beep
	    {
		gui_mch_beep();
	    }
	    // Other Ctrl character: shouldn't happen!

	    --len;	// Skip this char
	    ++s;
	}
	បើមិនដូច្នេះ
	{
	    p = s;
	    ខណៈពេល (len > 0 && (
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
	    gui_outstr(s, (ចំនួនគត់)(p - s));
	    s = p;
	}
    }

    // Postponed update of the cursor (won't work if "can_update_cursor" isn't
    // set).
    ប្រសិនបើ (force_cursor)
	gui_update_cursor(TRUE, TRUE);

    // When switching to another window the dragging must have stopped.
    // Required for GTK, dragged_sb isn't reset.
    ប្រសិនបើ (old_curwin != curwin)
	gui.dragged_sb = SBAR_NONE;

    // Update the scrollbars after clearing the screen or when switched
    // to another window.
    // Update the horizontal scrollbar always, it's difficult to check all
    // situations where it might change.
    ប្រសិនបើ (force_scrollbar || old_curwin != curwin)
	gui_update_scrollbars(force_scrollbar);
    បើមិនដូច្នេះ
	gui_update_horiz_scrollbar(FALSE);
    old_curwin = curwin;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#ប្រសិនបើ defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#endif

    gui_may_flush();		    // In case vim decides to take a nap
}

/*
 * When ScreenLines[] is invalid, updating the cursor should not be done, it
 * produces wrong results.  Call gui_dont_update_cursor() before that code and
 * gui_can_update_cursor() afterwards.
 */
    void
gui_dont_update_cursor(ចំនួនគត់ undraw)
{
    ប្រសិនបើ (!gui.in_use)
	ត្រឡប់;

    // Undraw the cursor now, we probably can't do it after the change.
    ប្រសិនបើ (undraw)
	gui_undraw_cursor();
    can_update_cursor = FALSE;
}

    void
gui_can_update_cursor(void)
{
    can_update_cursor = TRUE;
    // No need to update the cursor right now, there is always more output
    // after scrolling.
}

/*
 * Disable issuing gui_mch_flush().
 */
    void
gui_disable_flush(void)
{
    ++disable_flush;
}

/*
 * Enable issuing gui_mch_flush().
 */
    void
gui_enable_flush(void)
{
    --disable_flush;
}

/*
 * Issue gui_mch_flush() if it is not disabled.
 */
    void
gui_may_flush(void)
{
    ប្រសិនបើ (disable_flush == 0)
	gui_mch_flush();
}

    static void
gui_outstr(char_u *s, ចំនួនគត់ len)
{
    ចំនួនគត់	    this_len;
    ចំនួនគត់	    cells;

    ប្រសិនបើ (len == 0)
	ត្រឡប់;

    ប្រសិនបើ (len < 0)
	len = (ចំនួនគត់)STRLEN(s);

    ខណៈពេល (len > 0)
    {
	ប្រសិនបើ (has_mbyte)
	{
	    // Find out how many chars fit in the current line.
	    cells = 0;
	    សម្រាប់ (this_len = 0; this_len < len; )
	    {
		cells += (*mb_ptr2cells)(s + this_len);
		ប្រសិនបើ (gui.col + cells > Columns)
		    bha_nga;
		this_len += (*mb_ptr2len)(s + this_len);
	    }
	    ប្រសិនបើ (this_len > len)
		this_len = len;	    // don't include following composing char
	}
	បើមិនដូច្នេះ ប្រសិនបើ (gui.col + len > Columns)
	    this_len = Columns - gui.col;
	បើមិនដូច្នេះ
	    this_len = len;

	(void)gui_outstr_nowrap(s, this_len,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	s += this_len;
	len -= this_len;
	// fill up for a double-width char that doesn't fit.
	ប្រសិនបើ (len > 0 && gui.col < Columns)
	    (void)gui_outstr_nowrap((char_u *)" ", 1,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	// The cursor may wrap to the next line.
	ប្រសិនបើ (gui.col >= Columns)
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
    static ចំនួនគត់
gui_screenchar(
    ចំនួនគត់		off,	    // Offset from start of screen
    ចំនួនគត់		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    ចំនួនគត់		back)	    // backup this many chars when using bold trick
{
    char_u	buf[MB_MAXBYTES + 1];

    // Don't draw right half of a double-width UTF-8 char. "cannot happen"
    ប្រសិនបើ (enc_utf8 && ScreenLines[off] == 0)
	ត្រឡប់ OK;

    ប្រសិនបើ (enc_utf8 && ScreenLinesUC[off] != 0)
	// Draw UTF-8 multi-byte character.
	ត្រឡប់ gui_outstr_nowrap(buf, utfc_char2bytes(off, buf),
							 flags, fg, bg, back);

    ប្រសិនបើ (enc_dbcs == DBCS_JPNU && ScreenLines[off] == 0x8e)
    {
	buf[0] = ScreenLines[off];
	buf[1] = ScreenLines2[off];
	ត្រឡប់ gui_outstr_nowrap(buf, 2, flags, fg, bg, back);
    }

    // Draw non-multi-byte character or DBCS character.
    ត្រឡប់ gui_outstr_nowrap(ScreenLines + off,
	    enc_dbcs ? (*mb_ptr2len)(ScreenLines + off) : 1,
							 flags, fg, bg, back);
}
#endif

#ifdef FEAT_GUI_GTK
/*
 * Output the string at the given screen position.  This is used in place
 * of gui_screenchar() where possible because Pango needs as much context
 * as possible to work nicely.  It's a lot faster as well.
 */
    static ចំនួនគត់
gui_screenstr(
    ចំនួនគត់		off,	    // Offset from start of screen
    ចំនួនគត់		len,	    // string length in screen cells
    ចំនួនគត់		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    ចំនួនគត់		back)	    // backup this many chars when using bold trick
{
    char_u  *buf;
    ចំនួនគត់	    outlen = 0;
    ចំនួនគត់	    i;
    ចំនួនគត់	    retval;

    ប្រសិនបើ (len <= 0) // "cannot happen"?
	ត្រឡប់ OK;

    ប្រសិនបើ (enc_utf8)
    {
	buf = alloc(len * MB_MAXBYTES + 1);
	ប្រសិនបើ (buf == NULL)
	    ត្រឡប់ OK; // not much we could do here...

	សម្រាប់ (i = off; i < off + len; ++i)
	{
	    ប្រសិនបើ (ScreenLines[i] == 0)
		continue; // skip second half of double-width char

	    ប្រសិនបើ (ScreenLinesUC[i] == 0)
		buf[outlen++] = ScreenLines[i];
	    បើមិនដូច្នេះ
		outlen += utfc_char2bytes(i, buf + outlen);
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, back);
	vim_free(buf);

	ត្រឡប់ retval;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (enc_dbcs == DBCS_JPNU)
    {
	buf = alloc(len * 2 + 1);
	ប្រសិនបើ (buf == NULL)
	    ត្រឡប់ OK; // not much we could do here...

	សម្រាប់ (i = off; i < off + len; ++i)
	{
	    buf[outlen++] = ScreenLines[i];

	    // handle double-byte single-width char
	    ប្រសិនបើ (ScreenLines[i] == 0x8e)
		buf[outlen++] = ScreenLines2[i];
	    បើមិនដូច្នេះ ប្រសិនបើ (MB_BYTE2LEN(ScreenLines[i]) == 2)
		buf[outlen++] = ScreenLines[++i];
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, back);
	vim_free(buf);

	ត្រឡប់ retval;
    }
    បើមិនដូច្នេះ
    {
	ត្រឡប់ gui_outstr_nowrap(&ScreenLines[off], len,
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
    static ចំនួនគត់
gui_outstr_nowrap(
    char_u	*s,
    ចំនួនគត់		len,
    ចំនួនគត់		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    ចំនួនគត់		back)	    // backup this many chars when using bold trick
{
    long_u	highlight_mask;
    long_u	hl_mask_todo;
    guicolor_T	fg_color;
    guicolor_T	bg_color;
    guicolor_T	sp_color;
#ប្រសិនបើ !defined(FEAT_GUI_GTK)
    GuiFont	font = NOFONT;
    GuiFont	wide_font = NOFONT;
# ifdef FEAT_XFONTSET
    GuiFontset	fontset = NOFONTSET;
# endif
#endif
    attrentry_T	*aep = NULL;
    ចំនួនគត់		draw_flags;
    ចំនួនគត់		col = gui.col;
#ifdef FEAT_SIGN_ICONS
    ចំនួនគត់		draw_sign = FALSE;
    ចំនួនគត់		signcol = 0;
    char_u	extra[18];
# ifdef FEAT_NETBEANS_INTG
    ចំនួនគត់		multi_sign = FALSE;
# endif
#endif

    ប្រសិនបើ (len < 0)
	len = (ចំនួនគត់)STRLEN(s);
    ប្រសិនបើ (len == 0)
	ត្រឡប់ OK;

#ifdef FEAT_SIGN_ICONS
    ប្រសិនបើ (*s == SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
	  || *s == MULTISIGN_BYTE
# endif
       )
    {
	size_t	slen = 2;		    // 2 spaces by default

# ifdef FEAT_NETBEANS_INTG
	ប្រសិនបើ (*s == MULTISIGN_BYTE)
	    multi_sign = TRUE;
# endif
	// draw spaces instead
	ប្រសិនបើ (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u' &&
		(curwin->w_p_nu || curwin->w_p_rnu))
	{
	    ចំនួនគត់	n = number_width(curwin);

	    slen = MIN(n, (ចំនួនគត់)parimaa_na(extra) - 1);
	}
	vim_memset(extra, ' ', slen);
	extra[slen] = NUL;
	s = extra;
	ប្រសិនបើ (len == 1 && col > 0)
	    --col;
	len = (ចំនួនគត់)slen;
	ប្រសិនបើ (len > 2)
	    // right align sign icon in the number column
	    signcol = col + len - 3;
	បើមិនដូច្នេះ
	    signcol = col;
	draw_sign = TRUE;
	highlight_mask = 0;
    }
    បើមិនដូច្នេះ
#endif
    ប្រសិនបើ (gui.highlight_mask > HL_ALL)
    {
	aep = syn_gui_attr2entry(gui.highlight_mask);
	ប្រសិនបើ (aep == NULL)	    // highlighting not set
	    highlight_mask = 0;
	បើមិនដូច្នេះ
	    highlight_mask = aep->ae_attr;
    }
    បើមិនដូច្នេះ
	highlight_mask = gui.highlight_mask;
    hl_mask_todo = highlight_mask;

#ប្រសិនបើ !defined(FEAT_GUI_GTK)
    // Set the font
    ប្រសិនបើ (aep != NULL && aep->ae_u.gui.font != NOFONT)
	font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
    បើមិនដូច្នេះ ប្រសិនបើ (aep != NULL && aep->ae_u.gui.fontset != NOFONTSET)
	fontset = aep->ae_u.gui.fontset;
# endif
    បើមិនដូច្នេះ
    {
# ifdef FEAT_XFONTSET
	ប្រសិនបើ (gui.fontset != NOFONTSET)
	    fontset = gui.fontset;
	បើមិនដូច្នេះ
# endif
	    ប្រសិនបើ (hl_mask_todo & (HL_BOLD | HL_STANDOUT))
	{
	    ប្រសិនបើ ((hl_mask_todo & HL_ITALIC) && gui.boldital_font != NOFONT)
	    {
		font = gui.boldital_font;
		hl_mask_todo &= ~(HL_BOLD | HL_STANDOUT | HL_ITALIC);
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (gui.bold_font != NOFONT)
	    {
		font = gui.bold_font;
		hl_mask_todo &= ~(HL_BOLD | HL_STANDOUT);
	    }
	    បើមិនដូច្នេះ
		font = gui.norm_font;
	}
	បើមិនដូច្នេះ ប្រសិនបើ ((hl_mask_todo & HL_ITALIC) && gui.ital_font != NOFONT)
	{
	    font = gui.ital_font;
	    hl_mask_todo &= ~HL_ITALIC;
	}
	បើមិនដូច្នេះ
	    font = gui.norm_font;

	/*
	 * Choose correct wide_font by font.  wide_font should be set with font
	 * at same time in above block.  But it will make many "ifdef" nasty
	 * blocks.  So we do it here.
	 */
	ប្រសិនបើ (font == gui.boldital_font && gui.wide_boldital_font)
	    wide_font = gui.wide_boldital_font;
	បើមិនដូច្នេះ ប្រសិនបើ (font == gui.bold_font && gui.wide_bold_font)
	    wide_font = gui.wide_bold_font;
	បើមិនដូច្នេះ ប្រសិនបើ (font == gui.ital_font && gui.wide_ital_font)
	    wide_font = gui.wide_ital_font;
	បើមិនដូច្នេះ ប្រសិនបើ (font == gui.norm_font && gui.wide_font)
	    wide_font = gui.wide_font;
    }
# ifdef FEAT_XFONTSET
    ប្រសិនបើ (fontset != NOFONTSET)
	gui_mch_set_fontset(fontset);
    បើមិនដូច្នេះ
# endif
	gui_mch_set_font(font);
#endif

    draw_flags = 0;

    // Set the color
    bg_color = gui.back_pixel;
    ប្រសិនបើ ((flags & GUI_MON_IS_CURSOR) && gui.in_focus)
    {
	draw_flags |= DRAW_CURSOR;
	fg_color = fg;
	bg_color = bg;
	sp_color = fg;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (aep != NULL)
    {
	fg_color = aep->ae_u.gui.fg_color;
	ប្រសិនបើ (fg_color == INVALCOLOR)
	    fg_color = gui.norm_pixel;
	bg_color = aep->ae_u.gui.bg_color;
	ប្រសិនបើ (bg_color == INVALCOLOR)
	    bg_color = gui.back_pixel;
	sp_color = aep->ae_u.gui.sp_color;
	ប្រសិនបើ (sp_color == INVALCOLOR)
	    sp_color = fg_color;
    }
    បើមិនដូច្នេះ
    {
	fg_color = gui.norm_pixel;
	sp_color = fg_color;
    }

    ប្រសិនបើ (highlight_mask & (HL_INVERSE | HL_STANDOUT))
    {
	gui_mch_set_fg_color(bg_color);
	gui_mch_set_bg_color(fg_color);
    }
    បើមិនដូច្នេះ
    {
	gui_mch_set_fg_color(fg_color);
	gui_mch_set_bg_color(bg_color);
    }
    gui_mch_set_sp_color(sp_color);

    // Clear the selection if we are about to write over it
    ប្រសិនបើ (!(flags & GUI_MON_NOCLEAR))
	clip_may_clear_selection(gui.row, gui.row);


    // If there's no bold font, then fake it
    ប្រសិនបើ (hl_mask_todo & (HL_BOLD | HL_STANDOUT))
	draw_flags |= DRAW_BOLD;

    /*
     * When drawing bold or italic characters the spill-over from the left
     * neighbor may be destroyed.  Let the caller backup to start redrawing
     * just after a blank.
     */
    ប្រសិនបើ (back != 0 && ((draw_flags & DRAW_BOLD) || (highlight_mask & HL_ITALIC)))
	ត្រឡប់ FAIL;

#ប្រសិនបើ defined(FEAT_GUI_GTK)
    // If there's no italic font, then fake it.
    // For GTK2, we don't need a different font for italic style.
    ប្រសិនបើ (hl_mask_todo & HL_ITALIC)
	draw_flags |= DRAW_ITALIC;

    // Do we underline the text?
    ប្រសិនបើ (hl_mask_todo & HL_UNDERLINE)
	draw_flags |= DRAW_UNDERL;

#បើមិនដូច្នេះ
    // Do we underline the text?
    ប្រសិនបើ ((hl_mask_todo & HL_UNDERLINE) || (hl_mask_todo & HL_ITALIC))
	draw_flags |= DRAW_UNDERL;
#endif
    // Do we undercurl the text?
    ប្រសិនបើ (hl_mask_todo & HL_UNDERCURL)
	draw_flags |= DRAW_UNDERC;

    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

    // Do we strikethrough the text?
    ប្រសិនបើ (hl_mask_todo & HL_STRIKETHROUGH)
	draw_flags |= DRAW_STRIKE;

    // Do we draw transparently?
    ប្រសិនបើ (flags & GUI_MON_TRS_CURSOR)
	draw_flags |= DRAW_TRANSP;

    /*
     * Draw the text.
     */
#ifdef FEAT_GUI_GTK
    // The value returned is the length in display cells
    len = gui_gtk_draw_string(gui.row, col, s, len, draw_flags);
#បើមិនដូច្នេះ
    ប្រសិនបើ (enc_utf8)
    {
	ចំនួនគត់	start;		// index of bytes to be drawn
	ចំនួនគត់	cells;		// cellwidth of bytes to be drawn
	ចំនួនគត់	thislen;	// length of bytes to be drawn
	ចំនួនគត់	cn;		// cellwidth of current char
	ចំនួនគត់	i;		// index of current char
	ចំនួនគត់	c;		// current char value
	ចំនួនគត់	cl;		// byte length of current char
	ចំនួនគត់	comping;	// current char is composing
	ចំនួនគត់	scol = col;	// screen column
	ចំនួនគត់	curr_wide = FALSE;  // use 'guifontwide'
	ចំនួនគត់	prev_wide = FALSE;
	ចំនួនគត់	wide_changed;
# ifdef MSWIN
	ចំនួនគត់	sep_comp = FALSE;   // Don't separate composing chars.
# បើមិនដូច្នេះ
	ចំនួនគត់	sep_comp = TRUE;    // Separate composing chars.
# endif

	// Break the string at a composing character, it has to be drawn on
	// top of the previous character.
	start = 0;
	cells = 0;
	សម្រាប់ (i = 0; i < len; i += cl)
	{
	    c = utf_ptr2char(s + i);
	    cn = utf_char2cells(c);
	    comping = utf_iscomposing(c);
	    ប្រសិនបើ (!comping)	// count cells from non-composing chars
		cells += cn;
	    ប្រសិនបើ (!comping || sep_comp)
	    {
		ប្រសិនបើ (cn > 1
# ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
# endif
			&& wide_font != NOFONT)
		    curr_wide = TRUE;
		បើមិនដូច្នេះ
		    curr_wide = FALSE;
	    }
	    cl = utf_ptr2len(s + i);
	    ប្រសិនបើ (cl == 0)	// hit end of string
		len = i + cl;	// len must be wrong "cannot happen"

	    wide_changed = curr_wide != prev_wide;

	    // Print the string so far if it's the last character or there is
	    // a composing character.
	    ប្រសិនបើ (i + cl >= len || (comping && sep_comp && i > start)
		    || wide_changed
# ប្រសិនបើ defined(FEAT_GUI_X11)
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
		ប្រសិនបើ ((comping && sep_comp) || wide_changed)
		    thislen = i - start;
		បើមិនដូច្នេះ
		    thislen = i - start + cl;
		ប្រសិនបើ (thislen > 0)
		{
		    ប្រសិនបើ (prev_wide)
			gui_mch_set_font(wide_font);
		    gui_mch_draw_string(gui.row, scol, s + start, thislen,
								  draw_flags);
		    ប្រសិនបើ (prev_wide)
			gui_mch_set_font(font);
		    start += thislen;
		}
		scol += cells;
		cells = 0;
		// Adjust to not draw a character which width is changed
		// against with last one.
		ប្រសិនបើ (wide_changed && !(comping && sep_comp))
		{
		    scol -= cn;
		    cl = 0;
		}

# ប្រសិនបើ defined(FEAT_GUI_X11)
		// No fontset: draw a space to fill the gap after a wide char
		//
		ប្រសិនបើ (cn > 1 && (draw_flags & DRAW_TRANSP) == 0
#  ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
#  endif
			&& !wide_changed)
		    gui_mch_draw_string(gui.row, scol - 1, (char_u *)" ",
							       1, draw_flags);
# endif
	    }
	    // Draw a composing char on top of the previous char.
	    ប្រសិនបើ (comping && sep_comp)
	    {
# ប្រសិនបើ defined(__APPLE_CC__) && TARGET_API_MAC_CARBON
		// Carbon ATSUI autodraws composing char over previous char
		gui_mch_draw_string(gui.row, scol, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# បើមិនដូច្នេះ
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
    បើមិនដូច្នេះ
    {
	gui_mch_draw_string(gui.row, col, s, len, draw_flags);
	ប្រសិនបើ (enc_dbcs == DBCS_JPNU)
	{
	    // Get the length in display cells, this can be different from the
	    // number of bytes for "euc-jp".
	    len = mb_string2cells(s, len);
	}
    }
#endif // !FEAT_GUI_GTK

    ប្រសិនបើ (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
	gui.col = col + len;

    // May need to invert it when it's part of the selection.
    ប្រសិនបើ (flags & GUI_MON_NOCLEAR)
	clip_may_redraw_selection(gui.row, col, len);

    ប្រសិនបើ (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
    {
	// Invalidate the old physical cursor position if we wrote over it
	ប្រសិនបើ (gui.cursor_row == gui.row
		&& gui.cursor_col >= col
		&& gui.cursor_col < col + len)
	    gui.cursor_is_valid = false;
    }

#ifdef FEAT_SIGN_ICONS
    ប្រសិនបើ (draw_sign)
	// Draw the sign on top of the spaces.
	gui_mch_drawsign(gui.row, signcol, gui.highlight_mask);
# ប្រសិនបើ defined(FEAT_NETBEANS_INTG) && (defined(FEAT_GUI_X11) \
	|| defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN))
    ប្រសិនបើ (multi_sign)
	netbeans_draw_multisign_indicator(gui.row);
# endif
#endif

    ត្រឡប់ OK;
}

/*
 * Undraw the cursor.  This actually redraws the character at the cursor
 * position, plus some more characters when needed.
 */
    void
gui_undraw_cursor(void)
{
    ប្រសិនបើ (!gui.cursor_is_valid)
	ត្រឡប់;

    // Always redraw the character just before if there is one, because
    // with some fonts and characters there can be a one pixel overlap.
    ចំនួនគត់ startcol = gui.cursor_col > 0 ? gui.cursor_col - 1 : gui.cursor_col;
    ចំនួនគត់ endcol = gui.cursor_col;

#ប្រសិនបើ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    gui_adjust_undraw_cursor_for_ligatures(&startcol, &endcol);
#endif
    gui_redraw_block(gui.cursor_row, startcol,
	    gui.cursor_row, endcol, GUI_MON_NOCLEAR);

    // Cursor_is_valid is reset when the cursor is undrawn, also reset it
    // here in case it wasn't needed to undraw it.
    gui.cursor_is_valid = false;
}

    void
gui_redraw(
    ចំនួនគត់		x,
    ចំនួនគត់		y,
    ចំនួនគត់		w,
    ចំនួនគត់		h)
{
    ចំនួនគត់		row1, col1, row2, col2;

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
    ប្រសិនបើ (gui.row == gui.cursor_row)
	gui_update_cursor(TRUE, TRUE);
}

/*
 * Draw a rectangular block of characters, from row1 to row2 (inclusive) and
 * from col1 to col2 (inclusive).
 */
    void
gui_redraw_block(
    ចំនួនគត់		row1,
    ចំនួនគត់		col1,
    ចំនួនគត់		row2,
    ចំនួនគត់		col2,
    ចំនួនគត់		flags)	// flags for gui_outstr_nowrap()
{
    ចំនួនគត់		old_row, old_col;
    long_u	old_hl_mask;
    ចំនួនគត់		off;
    sattr_T	first_attr;
    ចំនួនគត់		idx, len;
    ចំនួនគត់		back, nback;
    ចំនួនគត់		orig_col1, orig_col2;

    // Don't try to update when ScreenLines is not valid
    ប្រសិនបើ (!screen_cleared || ScreenLines == NULL)
	ត្រឡប់;

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

    សម្រាប់ (gui.row = row1; gui.row <= row2; gui.row++)
    {
	// When only half of a double-wide character is in the block, include
	// the other half.
	col1 = orig_col1;
	col2 = orig_col2;
	off = LineOffset[gui.row];
	ប្រសិនបើ (enc_dbcs != 0)
	{
	    ប្រសិនបើ (col1 > 0)
		col1 -= dbcs_screen_head_off(ScreenLines + off,
						    ScreenLines + off + col1);
	    col2 += dbcs_screen_tail_off(ScreenLines + off,
						    ScreenLines + off + col2);
	}
	បើមិនដូច្នេះ ប្រសិនបើ (enc_utf8)
	{
	    ប្រសិនបើ (ScreenLines[off + col1] == 0)
	    {
		ប្រសិនបើ (col1 > 0)
		    --col1;
		បើមិនដូច្នេះ
		    // FIXME: how can the first character ever be zero?
		    siemsg("NUL in ScreenLines in row %ld", (long)gui.row);
	    }
#ifdef FEAT_GUI_GTK
	    ប្រសិនបើ (col2 + 1 < Columns && ScreenLines[off + col2 + 1] == 0)
		++col2;
#endif
	}
	gui.col = col1;
	off = LineOffset[gui.row] + gui.col;
	len = col2 - col1 + 1;

	// Find how many chars back this highlighting starts, or where a space
	// is.  Needed for when the bold trick is used
	សម្រាប់ (back = 0; back < col1; ++back)
	    ប្រសិនបើ (ScreenAttrs[off - 1 - back] != ScreenAttrs[off]
		    || ScreenLines[off - 1 - back] == ' ')
		bha_nga;

	// Break it up in strings of characters with the same attributes.
	// Print UTF-8 characters individually.
	ខណៈពេល (len > 0)
	{
	    first_attr = ScreenAttrs[off];
	    gui.highlight_mask = first_attr;
#ប្រសិនបើ !defined(FEAT_GUI_GTK)
	    ប្រសិនបើ (enc_utf8 && ScreenLinesUC[off] != 0)
	    {
		// output multi-byte character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
		ប្រសិនបើ (gui.col < Columns && ScreenLines[off + 1] == 0)
		    idx = 2;
		បើមិនដូច្នេះ
		    idx = 1;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (enc_dbcs == DBCS_JPNU && ScreenLines[off] == 0x8e)
	    {
		// output double-byte, single-width character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
		idx = 1;
	    }
	    បើមិនដូច្នេះ
#endif
	    {
#ifdef FEAT_GUI_GTK
		សម្រាប់ (idx = 0; idx < len; ++idx)
		{
		    ប្រសិនបើ (enc_utf8 && ScreenLines[off + idx] == 0)
			continue; // skip second half of double-width char
		    ប្រសិនបើ (ScreenAttrs[off + idx] != first_attr)
			bha_nga;
		}
		// gui_screenstr() takes care of multibyte chars
		nback = gui_screenstr(off, idx, flags,
				      (guicolor_T)0, (guicolor_T)0, back);
#បើមិនដូច្នេះ
		សម្រាប់ (idx = 0; idx < len && ScreenAttrs[off + idx] == first_attr;
									idx++)
		{
		    // Stop at a multi-byte Unicode character.
		    ប្រសិនបើ (enc_utf8 && ScreenLinesUC[off + idx] != 0)
			bha_nga;
		    ប្រសិនបើ (enc_dbcs == DBCS_JPNU)
		    {
			// Stop at a double-byte single-width char.
			ប្រសិនបើ (ScreenLines[off + idx] == 0x8e)
			    bha_nga;
			ប្រសិនបើ (len > 1 && (*mb_ptr2len)(ScreenLines
							    + off + idx) == 2)
			    ++idx;  // skip second byte of double-byte char
		    }
		}
		nback = gui_outstr_nowrap(ScreenLines + off, idx, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
#endif
	    }
	    ប្រសិនបើ (nback == FAIL)
	    {
		// Must back up to start drawing where a bold or italic word
		// starts.
		off -= back;
		len += back;
		gui.col -= back;
	    }
	    បើមិនដូច្នេះ
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
    gui.highlight_mask = (ចំនួនគត់)old_hl_mask;
}

    static void
gui_delete_lines(ចំនួនគត់ row, ចំនួនគត់ count)
{
    ប្រសិនបើ (count <= 0)
	ត្រឡប់;

    ប្រសិនបើ (row + count > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_block(row, gui.scroll_region_left,
			      gui.scroll_region_bot, gui.scroll_region_right);
    បើមិនដូច្នេះ
    {
	gui_mch_delete_lines(row, count);

	// If the cursor was in the deleted lines it's now gone.  If the
	// cursor was in the scrolled lines adjust its position.
	ប្រសិនបើ (gui.cursor_row >= row
		&& gui.cursor_col >= gui.scroll_region_left
		&& gui.cursor_col <= gui.scroll_region_right)
	{
	    ប្រសិនបើ (gui.cursor_row < row + count)
		gui.cursor_is_valid = false;
	    បើមិនដូច្នេះ ប្រសិនបើ (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_row -= count;
	}
    }
}

    static void
gui_insert_lines(ចំនួនគត់ row, ចំនួនគត់ count)
{
    ប្រសិនបើ (count <= 0)
	ត្រឡប់;

    ប្រសិនបើ (row + count > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_block(row, gui.scroll_region_left,
			      gui.scroll_region_bot, gui.scroll_region_right);
    បើមិនដូច្នេះ
    {
	gui_mch_insert_lines(row, count);

	ប្រសិនបើ (gui.cursor_row >= gui.row
		&& gui.cursor_col >= gui.scroll_region_left
		&& gui.cursor_col <= gui.scroll_region_right)
	{
	    ប្រសិនបើ (gui.cursor_row <= gui.scroll_region_bot - count)
		gui.cursor_row += count;
	    បើមិនដូច្នេះ ប្រសិនបើ (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_is_valid = false;
	}
    }
}

#ifdef FEAT_TIMERS
/*
 * Passed to ui_wait_for_chars_or_timer(), ignoring extra arguments.
 */
    static ចំនួនគត់
gui_wait_for_chars_3(
    long wtime,
    ចំនួនគត់ *interrupted UNUSED,
    ចំនួនគត់ ignore_input UNUSED)
{
    ត្រឡប់ gui_mch_wait_for_chars(wtime);
}
#endif

/*
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    static ចំនួនគត់
gui_wait_for_chars_or_timer(
	long wtime,
	ចំនួនគត់ *interrupted UNUSED,
	ចំនួនគត់ ignore_input UNUSED)
{
#ifdef FEAT_TIMERS
    ត្រឡប់ ui_wait_for_chars_or_timer(wtime, gui_wait_for_chars_3,
						    interrupted, ignore_input);
#បើមិនដូច្នេះ
    ត្រឡប់ gui_mch_wait_for_chars(wtime);
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
    static ចំនួនគត់
gui_wait_for_chars_buf(
    char_u	*buf,
    ចំនួនគត់		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    ចំនួនគត់		tb_change_cnt)
{
    ចំនួនគត់	    retval;
    ចំនួនគត់	    keep_blinking; // Guard against restarting blink cycle on CursorHold

#ifdef FEAT_MENU
    // If we're going to wait a bit, update the menus and mouse shape for the
    // current State.
    ប្រសិនបើ (wtime != 0)
	gui_update_menus(0);
#endif

    gui_mch_update();
    ប្រសិនបើ (input_available())	// Got char, return immediately
    {
	ប្រសិនបើ (gui_mch_is_blinking())
	    gui_mch_stop_blink(TRUE);
	ប្រសិនបើ (buf != NULL && !typebuf_changed(tb_change_cnt))
	    ត្រឡប់ read_from_input_buf(buf, (long)maxlen);
	ត្រឡប់ 0;
    }
    ប្រសិនបើ (wtime == 0)		// Don't wait for char
	ត្រឡប់ FAIL;

    // Before waiting, flush any output to the screen.
    gui_mch_flush();

    // Blink while waiting for a character.
    ប្រសិនបើ (!gui_mch_is_blinking())
	gui_mch_start_blink();

    // Common function to loop until "wtime" is met, while handling timers and
    // other callbacks.
    retval = inchar_loop(buf, maxlen, wtime, tb_change_cnt,
			 gui_wait_for_chars_or_timer, NULL);

    // Keep blinking when CursorHold wakes the input loop. (See PR #21115)
    keep_blinking = retval == 3 && buf != NULL
	&& buf[0] == K_SPECIAL && buf[1] == KS_EXTRA
	&& buf[2] == (ចំនួនគត់)KE_CURSORHOLD;

    ប្រសិនបើ (!keep_blinking)
	gui_mch_stop_blink(TRUE);

    ត្រឡប់ retval;
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
    ចំនួនគត់
gui_wait_for_chars(long wtime, ចំនួនគត់ tb_change_cnt)
{
    ត្រឡប់ gui_wait_for_chars_buf(NULL, 0, wtime, tb_change_cnt);
}

/*
 * Equivalent of mch_inchar() for the GUI.
 */
    ចំនួនគត់
gui_inchar(
    char_u  *buf,
    ចំនួនគត់	    maxlen,
    long    wtime,		// milliseconds
    ចំនួនគត់	    tb_change_cnt)
{
    ត្រឡប់ gui_wait_for_chars_buf(buf, maxlen, wtime, tb_change_cnt);
}

/*
 * Fill p[4] with mouse coordinates encoded for check_termcode().
 */
    static void
fill_mouse_coord(char_u *p, ចំនួនគត់ col, ចំនួនគត់ row)
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
    void
gui_send_mouse_event(
    ចំនួនគត់	    button,
    ចំនួនគត់	    x,
    ចំនួនគត់	    y,
    ចំនួនគត់	    repeated_click,
    int_u   modifiers)
{
    static ចំនួនគត់	    prev_row = 0, prev_col = 0;
    static ចំនួនគត់	    prev_button = -1;
    static ចំនួនគត់	    num_clicks = 1;
    char_u	    string[10];
    ga_nanaa key_extra  button_char;
    ចំនួនគត់		    row, col;
#ifdef FEAT_CLIPBOARD
    ចំនួនគត់		    checkfor;
    ចំនួនគត់		    did_clip = FALSE;
#endif

    /*
     * Scrolling may happen at any time, also while a selection is present.
     */
    switch (button)
    {
	pak_sa MOUSE_MOVE:
	    button_char = KE_MOUSEMOVE_XY;
	    goto button_set;
	pak_sa MOUSE_X1:
	    button_char = KE_X1MOUSE;
	    goto button_set;
	pak_sa MOUSE_X2:
	    button_char = KE_X2MOUSE;
	    goto button_set;
	pak_sa MOUSE_4:
	    button_char = KE_MOUSEDOWN;
	    goto button_set;
	pak_sa MOUSE_5:
	    button_char = KE_MOUSEUP;
	    goto button_set;
	pak_sa MOUSE_6:
	    button_char = KE_MOUSELEFT;
	    goto button_set;
	pak_sa MOUSE_7:
	    button_char = KE_MOUSERIGHT;
button_set:
	    {
		// Don't put events in the input queue now.
		ប្រសិនបើ (hold_gui_events)
		    ត្រឡប់;

		row = gui_xy2colrow(x, y, &col);
		// Don't report a mouse move unless moved to a
		// different character position.
		ប្រសិនបើ (button == MOUSE_MOVE)
		{
		    ប្រសិនបើ (row == prev_row && col == prev_col)
			ត្រឡប់;
		    បើមិនដូច្នេះ
		    {
			prev_row = row >= 0 ? row : 0;
			prev_col = col;
		    }
		}

		string[3] = CSI;
		string[4] = KS_EXTRA;
		string[5] = (ចំនួនគត់)button_char;

		// Pass the pointer coordinates of the scroll event so that we
		// know which window to scroll.
		string[6] = (char_u)(col / 128 + ' ' + 1);
		string[7] = (char_u)(col % 128 + ' ' + 1);
		string[8] = (char_u)(row / 128 + ' ' + 1);
		string[9] = (char_u)(row % 128 + ' ' + 1);

		ប្រសិនបើ (modifiers == 0)
		    add_to_input_buf(string + 3, 7);
		បើមិនដូច្នេះ
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = 0;
		    ប្រសិនបើ (modifiers & MOUSE_SHIFT)
			string[2] |= MOD_MASK_SHIFT;
		    ប្រសិនបើ (modifiers & MOUSE_CTRL)
			string[2] |= MOD_MASK_CTRL;
		    ប្រសិនបើ (modifiers & MOUSE_ALT)
			string[2] |= MOD_MASK_ALT;
		    add_to_input_buf(string, 10);
		}
		ត្រឡប់;
	    }
    }

#ifdef FEAT_CLIPBOARD
    // If a clipboard selection is in progress, handle it
    ប្រសិនបើ (clip_star.state == SELECT_IN_PROGRESS)
    {
	clip_process_selection(button, X_2_COL(x), Y_2_ROW(y), repeated_click);

	// A release event may still need to be sent if the position is equal.
	row = gui_xy2colrow(x, y, &col);
	ប្រសិនបើ (button != MOUSE_RELEASE || row != prev_row || col != prev_col)
	    ត្រឡប់;
    }

    // Determine which mouse settings to look for based on the current mode
    switch (get_real_state())
    {
	pak_sa MODE_NORMAL_BUSY:
	pak_sa MODE_OP_PENDING:
# ifdef FEAT_TERMINAL
	pak_sa MODE_TERMINAL:
# endif
	pak_sa MODE_NORMAL:	checkfor = MOUSE_NORMAL; bha_nga;
	pak_sa MODE_VISUAL:	checkfor = MOUSE_VISUAL; bha_nga;
	pak_sa MODE_SELECT:	checkfor = MOUSE_VISUAL; bha_nga;
	pak_sa MODE_REPLACE:
	pak_sa MODE_REPLACE | MODE_LANGMAP:
	pak_sa MODE_VREPLACE:
	pak_sa MODE_VREPLACE | MODE_LANGMAP:
	pak_sa MODE_INSERT:
	pak_sa MODE_INSERT | MODE_LANGMAP:
				checkfor = MOUSE_INSERT; bha_nga;
	pak_sa MODE_ASKMORE:
	pak_sa MODE_HITRETURN:	// At the more- and hit-enter prompt pass the
				// mouse event for a click on or below the
				// message line.
				ប្រសិនបើ (Y_2_ROW(y) >= msg_row)
				    checkfor = MOUSE_NORMAL;
				បើមិនដូច្នេះ
				    checkfor = MOUSE_RETURN;
				bha_nga;

	    /*
	     * On the command line, use the clipboard selection on all lines
	     * but the command line.  But not when pasting.
	     */
	pak_sa MODE_CMDLINE:
	pak_sa MODE_CMDLINE | MODE_LANGMAP:
	    ប្រសិនបើ (Y_2_ROW(y) < cmdline_row && button != MOUSE_MIDDLE)
		checkfor = MOUSE_NONE;
	    បើមិនដូច្នេះ
		checkfor = MOUSE_COMMAND;
	    bha_nga;

	default:
	    checkfor = MOUSE_NONE;
	    bha_nga;
    };

    /*
     * Allow clipboard selection of text on the command line in "normal"
     * modes.  Don't do this when dragging the status line, or extending a
     * Visual selection.
     */
    ប្រសិនបើ ((State == MODE_NORMAL || State == MODE_NORMAL_BUSY
						      || (State & MODE_INSERT))
	    && X_2_COL(x) >= firstwin->w_wincol
	    && X_2_COL(x) < firstwin->w_wincol + topframe->fr_width
	    && Y_2_ROW(y) >= topframe->fr_height + firstwin->w_winrow
	    && button != MOUSE_DRAG
# ifdef FEAT_MOUSESHAPE
	    && !drag_status_line
	    && !drag_sep_line
# endif
	    )
	checkfor = MOUSE_NONE;

    /*
     * Use modeless selection when holding CTRL and SHIFT pressed.
     */
    ប្រសិនបើ ((modifiers & MOUSE_CTRL) && (modifiers & MOUSE_SHIFT))
	checkfor = MOUSE_NONEF;

    /*
     * In Ex mode, always use modeless selection.
     */
    ប្រសិនបើ (exmode_active)
	checkfor = MOUSE_NONE;

    /*
     * If the mouse settings say to not use the mouse, use the modeless
     * selection.  But if Visual is active, assume that only the Visual area
     * will be selected.
     * Exception: On the command line, both the selection is used and a mouse
     * key is sent.
     */
    ប្រសិនបើ (!mouse_has(checkfor) || checkfor == MOUSE_COMMAND)
    {
	// Don't do modeless selection in Visual mode.
	ប្រសិនបើ (checkfor != MOUSE_NONEF && VIsual_active && (State & MODE_NORMAL))
	    ត្រឡប់;

	/*
	 * When 'mousemodel' is "popup", shift-left is translated to right.
	 * But not when also using Ctrl.
	 */
	ប្រសិនបើ (mouse_model_popup() && button == MOUSE_LEFT
		&& (modifiers & MOUSE_SHIFT) && !(modifiers & MOUSE_CTRL))
	{
	    button = MOUSE_RIGHT;
	    modifiers &= ~ MOUSE_SHIFT;
	}

	// If the selection is done, allow the right button to extend it.
	// If the selection is cleared, allow the right button to start it
	// from the cursor position.
	ប្រសិនបើ (button == MOUSE_RIGHT)
	{
	    ប្រសិនបើ (clip_star.state == SELECT_CLEARED)
	    {
		ប្រសិនបើ (State & MODE_CMDLINE)
		{
		    col = msg_col;
		    row = msg_row;
		}
		បើមិនដូច្នេះ
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
	បើមិនដូច្នេះ ប្រសិនបើ (button == MOUSE_LEFT)
	{
	    clip_start_selection(X_2_COL(x), Y_2_ROW(y), repeated_click);
	    did_clip = TRUE;
	}

	// Always allow pasting
	ប្រសិនបើ (button != MOUSE_MIDDLE)
	{
	    ប្រសិនបើ (!mouse_has(checkfor) || button == MOUSE_RELEASE)
		ត្រឡប់;
	    ប្រសិនបើ (checkfor != MOUSE_COMMAND)
		button = MOUSE_LEFT;
	}
	repeated_click = FALSE;
    }

    ប្រសិនបើ (clip_star.state != SELECT_CLEARED && !did_clip)
	clip_clear_selection(&clip_star);
#endif

    // Don't put events in the input queue now.
    ប្រសិនបើ (hold_gui_events)
	ត្រឡប់;

    row = gui_xy2colrow(x, y, &col);

    /*
     * If we are dragging and the mouse hasn't moved far enough to be on a
     * different character, then don't send an event to vim.
     */
    ប្រសិនបើ (button == MOUSE_DRAG)
    {
	ប្រសិនបើ (row == prev_row && col == prev_col)
	    ត្រឡប់;
	// Dragging above the window, set "row" to -1 to cause a scroll.
	ប្រសិនបើ (y < 0)
	    row = -1;
    }

    /*
     * If topline has changed (window scrolled) since the last click, reset
     * repeated_click, because we don't want starting Visual mode when
     * clicking on a different character in the text.
     */
    ប្រសិនបើ (curwin->w_topline != gui_prev_topline
#ifdef FEAT_DIFF
	    || curwin->w_topfill != gui_prev_topfill
#endif
	    )
	repeated_click = FALSE;

    string[0] = CSI;	// this sequence is recognized by check_termcode()
    string[1] = KS_MOUSE;
    string[2] = KE_FILLER;
    ប្រសិនបើ (button != MOUSE_DRAG && button != MOUSE_RELEASE)
    {
	ប្រសិនបើ (repeated_click)
	{
	    /*
	     * Handle multiple clicks.	They only count if the mouse is still
	     * pointing at the same character.
	     */
	    ប្រសិនបើ (button != prev_button || row != prev_row || col != prev_col)
		num_clicks = 1;
	    បើមិនដូច្នេះ ប្រសិនបើ (++num_clicks > 4)
		num_clicks = 1;
	}
	បើមិនដូច្នេះ
	    num_clicks = 1;
	prev_button = button;
	gui_prev_topline = curwin->w_topline;
#ifdef FEAT_DIFF
	gui_prev_topfill = curwin->w_topfill;
#endif

	string[3] = (char_u)(button | 0x20);
	SET_NUM_MOUSE_CLICKS(string[3], num_clicks);
    }
    បើមិនដូច្នេះ
	string[3] = (char_u)button;

    string[3] |= modifiers;
    fill_mouse_coord(string + 4, col, row);
    add_to_input_buf(string, 8);

    ប្រសិនបើ (row < 0)
	prev_row = 0;
    បើមិនដូច្នេះ
	prev_row = row;
    prev_col = col;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#ប្រសិនបើ defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#endif
}

/*
 * Convert x and y coordinate to column and row in text window.
 * Corrects for multi-byte character.
 * returns column in "*colp" and row as return value;
 */
    static ចំនួនគត់
gui_xy2colrow(ចំនួនគត់ x, ចំនួនគត់ y, ចំនួនគត់ *colp)
{
    ចំនួនគត់		col = check_col(X_2_COL(x));
    ចំនួនគត់		row = check_row(Y_2_ROW(y));

    *colp = mb_fix_col(col, row);
    ត្រឡប់ row;
}

#ប្រសិនបើ defined(FEAT_MENU)
/*
 * Callback function for when a menu entry has been selected.
 */
    void
gui_menu_cb(vimmenu_T *menu)
{
    char_u  bytes[parimaa_na(long_u)];

    // Don't put events in the input queue now.
    ប្រសិនបើ (hold_gui_events)
	ត្រឡប់;

    bytes[0] = CSI;
    bytes[1] = KS_MENU;
    bytes[2] = KE_FILLER;
    add_to_input_buf(bytes, 3);
    add_long_to_buf((long_u)menu, bytes);
    add_to_input_buf_csi(bytes, parimaa_na(long_u));
}
#endif

static ចំនួនគត់	prev_which_scrollbars[3];

/*
 * Set which components are present.
 * If "oldval" is not NULL, "oldval" is the previous value, the new value is
 * in p_go.
 */
    void
gui_init_which_components(char_u *oldval UNUSED)
{
#ifdef FEAT_GUI_DARKTHEME
    static ចំនួនគត់	prev_dark_theme = FALSE;
    ចំនួនគត់		using_dark_theme = FALSE;
#endif
#ifdef FEAT_MENU
    static ចំនួនគត់	prev_menu_is_active = -1;
#endif
#ifdef FEAT_TOOLBAR
    static ចំនួនគត់	prev_toolbar = -1;
    ចំនួនគត់		using_toolbar = FALSE;
#endif
#ifdef FEAT_GUI_TABLINE
    ចំនួនគត់		using_tabline;
#endif
#ifdef FEAT_GUI_MSWIN
    static ចំនួនគត់	prev_titlebar = FALSE;
    ចំនួនគត់		using_titlebar = FALSE;
#endif

#ប្រសិនបើ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    static ចំនួនគត់	prev_fullscreen = FALSE;
    ចំនួនគត់		using_fullscreen = FALSE;
#endif
#ប្រសិនបើ defined(FEAT_MENU)
    static ចំនួនគត់	prev_tearoff = -1;
    ចំនួនគត់		using_tearoff = FALSE;
#endif

    char_u	*p;
    ចំនួនគត់		i;
#ifdef FEAT_MENU
    ចំនួនគត់		grey_old, grey_new;
    char_u	*temp;
#endif
    win_T	*wp;
    ចំនួនគត់		need_set_size;
    ចំនួនគត់		fix_size;

#ifdef FEAT_MENU
    ប្រសិនបើ (oldval != NULL && gui.in_use)
    {
	/*
	 * Check if the menus go from grey to non-grey or vice versa.
	 */
	grey_old = (vim_strchr(oldval, GO_GREY) != NULL);
	grey_new = (vim_strchr(p_go, GO_GREY) != NULL);
	ប្រសិនបើ (grey_old != grey_new)
	{
	    temp = p_go;
	    p_go = oldval;
	    gui_update_menus(MENU_ALL_MODES);
	    p_go = temp;
	}
    }
    gui.menu_is_active = false;
#endif

    សម្រាប់ (i = 0; i < 3; i++)
	gui.which_scrollbars[i] = false;
    សម្រាប់ (p = p_go; *p; p++)
	switch (*p)
	{
	    pak_sa GO_LEFT:
		gui.which_scrollbars[SBAR_LEFT] = true;
		bha_nga;
	    pak_sa GO_RIGHT:
		gui.which_scrollbars[SBAR_RIGHT] = true;
		bha_nga;
	    pak_sa GO_VLEFT:
		ប្រសិនបើ (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_LEFT] = true;
		bha_nga;
	    pak_sa GO_VRIGHT:
		ប្រសិនបើ (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_RIGHT] = true;
		bha_nga;
	    pak_sa GO_BOT:
		gui.which_scrollbars[SBAR_BOTTOM] = true;
		bha_nga;
#ifdef FEAT_GUI_DARKTHEME
	    pak_sa GO_DARKTHEME:
		using_dark_theme = TRUE;
		bha_nga;
#endif
#ifdef FEAT_MENU
	    pak_sa GO_MENUS:
		gui.menu_is_active = true;
		bha_nga;
#endif
	    pak_sa GO_GREY:
		// make menu's have grey items, ignored here
		bha_nga;
#ifdef FEAT_GUI_MSWIN
	    pak_sa GO_TITLEBAR:
		using_titlebar = TRUE;
		bha_nga;
#endif
#ប្រសិនបើ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
	    pak_sa GO_FULLSCREEN:
		using_fullscreen = TRUE;
		bha_nga;
#endif
#ifdef FEAT_TOOLBAR
	    pak_sa GO_TOOLBAR:
		using_toolbar = TRUE;
		bha_nga;
#endif
	    pak_sa GO_TEAROFF:
#ប្រសិនបើ defined(FEAT_MENU)
		using_tearoff = TRUE;
#endif
		bha_nga;
	    default:
		// Ignore options that are not supported
		bha_nga;
	}

    ប្រសិនបើ (!gui.in_use)
	ត្រឡប់;

    need_set_size = 0;
    fix_size = FALSE;

#ifdef FEAT_GUI_MSWIN
    ប្រសិនបើ (using_titlebar != prev_titlebar)
    {
	gui_mch_set_titlebar_colors();
	prev_titlebar = using_titlebar;
    }
#endif

#ប្រសិនបើ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    ប្រសិនបើ (using_fullscreen != prev_fullscreen)
    {
	gui_mch_set_fullscreen(using_fullscreen);
	prev_fullscreen = using_fullscreen;
    }
#endif

#ifdef FEAT_GUI_DARKTHEME
    ប្រសិនបើ (using_dark_theme != prev_dark_theme)
    {
	gui_mch_set_dark_theme(using_dark_theme);
	prev_dark_theme = using_dark_theme;
    }
#endif

#ifdef FEAT_GUI_TABLINE
    // Update the GUI tab line, it may appear or disappear.  This may
    // cause the non-GUI tab line to disappear or appear.
    using_tabline = gui_has_tabline();
    ប្រសិនបើ (!gui_mch_showing_tabline() != !using_tabline)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = Rows;
	gui_update_tabline();
	Rows = i;
	need_set_size |= RESIZE_VERT;
	ប្រសិនបើ (using_tabline)
	    fix_size = TRUE;
	ប្រសិនបើ (!gui_use_tabline())
	    redraw_tabline = TRUE;    // may draw non-GUI tab line
    }
#endif

    សម្រាប់ (i = 0; i < 3; i++)
    {
	// The scrollbar needs to be updated when it is shown/unshown and
	// when switching tab pages.  But the size only changes when it's
	// shown/unshown.  Thus we need two places to remember whether a
	// scrollbar is there or not.
	ប្រសិនបើ (gui.which_scrollbars[i] != prev_which_scrollbars[i]
		|| gui.which_scrollbars[i]
		!= curtab->tp_prev_which_scrollbars[i])
	{
	    ប្រសិនបើ (i == SBAR_BOTTOM)
		gui_mch_enable_scrollbar(&gui.bottom_sbar,
			gui.which_scrollbars[i]);
	    បើមិនដូច្នេះ
	    {
		FOR_ALL_WINDOWS(wp)
		    gui_do_scrollbar(wp, i, gui.which_scrollbars[i]);
	    }
	    ប្រសិនបើ (gui.which_scrollbars[i] != prev_which_scrollbars[i])
	    {
		ប្រសិនបើ (i == SBAR_BOTTOM)
		    need_set_size |= RESIZE_VERT;
		បើមិនដូច្នេះ
		    need_set_size |= RESIZE_HOR;
		ប្រសិនបើ (gui.which_scrollbars[i])
		    fix_size = TRUE;
	    }
	}
	curtab->tp_prev_which_scrollbars[i] = gui.which_scrollbars[i];
	prev_which_scrollbars[i] = gui.which_scrollbars[i];
    }

#ifdef FEAT_MENU
    ប្រសិនបើ (gui.menu_is_active != prev_menu_is_active)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = Rows;
	gui_mch_enable_menu(gui.menu_is_active);
	Rows = i;
	prev_menu_is_active = gui.menu_is_active;
	need_set_size |= RESIZE_VERT;
	ប្រសិនបើ (gui.menu_is_active)
	    fix_size = TRUE;
    }
#endif

#ifdef FEAT_TOOLBAR
    ប្រសិនបើ (using_toolbar != prev_toolbar)
    {
	gui_mch_show_toolbar(using_toolbar);
	prev_toolbar = using_toolbar;
	need_set_size |= RESIZE_VERT;
	ប្រសិនបើ (using_toolbar)
	    fix_size = TRUE;
    }
#endif
#ប្រសិនបើ defined(FEAT_MENU) && !(defined(MSWIN) && !defined(FEAT_TEAROFF))
    ប្រសិនបើ (using_tearoff != prev_tearoff)
    {
	gui_mch_toggle_tearoffs(using_tearoff);
	prev_tearoff = using_tearoff;
    }
#endif
    ប្រសិនបើ (need_set_size != 0)
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

	    ប្រសិនបើ (!gui.starting && need_set_size != (RESIZE_VERT | RESIZE_HOR))
		(void)char_avail();
	    ប្រសិនបើ ((need_set_size & RESIZE_VERT) == 0)
		Rows = prev_Rows;
	    បើមិនដូច្នេះ
		Rows = post_Rows;
	    ប្រសិនបើ ((need_set_size & RESIZE_HOR) == 0)
		Columns = prev_Columns;
	    បើមិនដូច្នេះ
		Columns = post_Columns;
	}
#endif
    }
    // When the console tabline appears or disappears the window positions
    // change.
    ប្រសិនបើ (firstwin->w_winrow != tabline_height())
	shell_new_rows();	// recompute window positions and heights
}

#ប្រសិនបើ defined(FEAT_GUI_TABLINE)
/*
 * Return TRUE if the GUI is taking care of the tabline.
 * It may still be hidden if 'showtabline' is zero.
 */
    ចំនួនគត់
gui_use_tabline(void)
{
    ត្រឡប់ gui.in_use && vim_strchr(p_go, GO_TABLINE) != NULL;
}

/*
 * Return TRUE if the GUI is showing the tabline.
 * This uses 'showtabline'.
 */
    static ចំនួនគត់
gui_has_tabline(void)
{
    ប្រសិនបើ (!gui_use_tabline()
	    || p_stal == 0
	    || (p_stal == 1 && first_tabpage->tp_next == NULL))
	ត្រឡប់ FALSE;
    ត្រឡប់ TRUE;
}

/*
 * Update the tabline.
 * This may display/undisplay the tabline and update the labels.
 */
    void
gui_update_tabline(void)
{
    ចំនួនគត់	    showit = gui_has_tabline();
    ចំនួនគត់	    shown = gui_mch_showing_tabline();

    ប្រសិនបើ (!gui.starting && starting == 0)
    {
	// Updating the tabline uses direct GUI commands, flush
	// outstanding instructions first. (esp. clear screen)
	out_flush();

	ប្រសិនបើ (!showit != !shown)
	{
	    // Save Rows/Columns before showing/hiding the tabline.
	    // gui_mch_show_tabline() processes GTK events (via
	    // gui_mch_update) which may trigger a spurious configure event
	    // that temporarily corrupts Rows.  Restore the original values so
	    // that gui_set_shellsize() uses the correct row/column count.
	    ចំនួនគត់ save_Rows = Rows;
	    ចំនួនគត់ save_Columns = Columns;

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
	ប្រសិនបើ (showit != 0)
	    gui_mch_update_tabline();
    }
}

/*
 * Get the label or tooltip for tab page "tp" into NameBuff[].
 */
    void
get_tabline_label(
    tabpage_T	*tp,
    ចំនួនគត់		tooltip)	// TRUE: get tooltip
{
    char_u	**opt;

    // Use 'guitablabel' or 'guitabtooltip' if it's set.
    opt = (tooltip ? &p_gtt : &p_gtl);
    ប្រសិនបើ (**opt != NUL)
    {
	char_u	res[MAXPATHL];
	tabpage_T *save_curtab;
	char_u	*opt_name = (char_u *)(tooltip ? "guitabtooltip"
							     : "guitablabel");

	printer_page_num = tabpage_index(tp);
# ifdef FEAT_EVAL
	set_vim_var_nr(VV_LNUM, printer_page_num);
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
					   0, (ចំនួនគត់)Columns, NULL, NULL, NULL);
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
    ប្រសិនបើ (**opt == NUL || *NameBuff == NUL)
    {
	win_T	*wp;
	ចំនួនគត់	wincount = 0;
	ចំនួនគត់	modified = FALSE;

	// Get the buffer name into NameBuff[] and shorten it.
	get_trans_bufname(tp == curtab ? curbuf : tp->tp_curwin->w_buffer);
	ប្រសិនបើ (!tooltip)
	    shorten_dir(NameBuff);

	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    ++wincount;
	    ប្រសិនបើ (bufIsChanged(wp->w_buffer))
		modified = TRUE;
	}

	ប្រសិនបើ (modified || wincount > 1)
	{
	    char_u  buf[40] = "+ ";		// Tentatively assume modified only
	    size_t  buflen = 2;
	    size_t  NameBufflen = STRLEN(NameBuff);

	    ប្រសិនបើ (wincount > 1)
		buflen = vim_snprintf_safelen(
		    (var_na *)buf,
		    parimaa_na(buf),
		    "%d%s ",
		    wincount,
		    modified ? "+" : "");

	    // Make sure resulting NameBuff will not exceed its bounds.
	    ប្រសិនបើ (NameBufflen + buflen < MAXPATHL)
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
    ចំនួនគត់
send_tabline_event(ចំនួនគត់ nr)
{
    char_u string[3];

    ប្រសិនបើ (nr == tabpage_index(curtab))
	ត្រឡប់ FALSE;

    // Don't put events in the input queue now.
    ប្រសិនបើ (hold_gui_events || cmdwin_type != 0)
    {
	// Set it back to the current tab page.
	gui_mch_set_curtab(tabpage_index(curtab));
	ត្រឡប់ FALSE;
    }

    string[0] = CSI;
    string[1] = KS_TABLINE;
    string[2] = KE_FILLER;
    add_to_input_buf(string, 3);
    string[0] = nr;
    add_to_input_buf_csi(string, 1);
    ត្រឡប់ TRUE;
}

/*
 * Send a tabline menu event
 */
    void
send_tabline_menu_event(ចំនួនគត់ tabidx, ចំនួនគត់ event)
{
    char_u	    string[3];

    // Don't put events in the input queue now.
    ប្រសិនបើ (hold_gui_events)
	ត្រឡប់;

    // Cannot close the last tabpage.
    ប្រសិនបើ (event == TABLINE_MENU_CLOSE && first_tabpage->tp_next == NULL)
	ត្រឡប់;

    string[0] = CSI;
    string[1] = KS_TABMENU;
    string[2] = KE_FILLER;
    add_to_input_buf(string, 3);
    string[0] = tabidx;
    string[1] = (char_u)(long)event;
    add_to_input_buf_csi(string, 2);
}

#endif

/*
 * Scrollbar stuff:
 */

/*
 * Remove all scrollbars.  Used before switching to another tab page.
 */
    void
gui_remove_scrollbars(void)
{
    ចំនួនគត់	    i;
    win_T   *wp;

    សម្រាប់ (i = 0; i < 3; i++)
    {
	ប្រសិនបើ (i == SBAR_BOTTOM)
	    gui_mch_enable_scrollbar(&gui.bottom_sbar, FALSE);
	បើមិនដូច្នេះ
	{
	    FOR_ALL_WINDOWS(wp)
		gui_do_scrollbar(wp, i, FALSE);
	}
	curtab->tp_prev_which_scrollbars[i] = -1;
    }
}

    void
gui_create_scrollbar(scrollbar_T *sb, ចំនួនគត់ type, win_T *wp)
{
    static ចំនួនគត់	sbar_ident = 0;

    sb->ident = sbar_ident++;	// No check for too big, but would it happen?
    sb->wp = wp;
    sb->type = type;
    sb->value = 0;
    sb->size = 1;
    sb->max = 1;
    sb->top = 0;
    sb->height = 0;
    sb->width = 0;
    sb->status_height = 0;
    gui_mch_create_scrollbar(sb, (wp == NULL) ? SBAR_HORIZ : SBAR_VERT);
}

/*
 * Find the scrollbar with the given index.
 */
    scrollbar_T *
gui_find_scrollbar(long ident)
{
    win_T	*wp;

    ប្រសិនបើ (gui.bottom_sbar.ident == ident)
	ត្រឡប់ &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	ប្រសិនបើ (wp->w_scrollbars[SBAR_LEFT].ident == ident)
	    ត្រឡប់ &wp->w_scrollbars[SBAR_LEFT];
	ប្រសិនបើ (wp->w_scrollbars[SBAR_RIGHT].ident == ident)
	    ត្រឡប់ &wp->w_scrollbars[SBAR_RIGHT];
    }
    ត្រឡប់ NULL;
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
    void
gui_drag_scrollbar(scrollbar_T *sb, long value, ចំនួនគត់ still_dragging)
{
    win_T	*wp;
    ចំនួនគត់		sb_num;
#ifdef USE_ON_FLY_SCROLL
    colnr_T	old_leftcol = curwin->w_leftcol;
    linenr_T	old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    ចំនួនគត់		old_topfill = curwin->w_topfill;
# endif
#បើមិនដូច្នេះ
    char_u	bytes[parimaa_na(long_u)];
    ចំនួនគត់		byte_count;
#endif

    ប្រសិនបើ (sb == NULL)
	ត្រឡប់;

    // Don't put events in the input queue now.
    ប្រសិនបើ (hold_gui_events)
	ត្រឡប់;

    ប្រសិនបើ (cmdwin_type != 0 && sb->wp != cmdwin_win)
	ត្រឡប់;

    ប្រសិនបើ (still_dragging)
    {
	ប្រសិនបើ (sb->wp == NULL)
	    gui.dragged_sb = SBAR_BOTTOM;
	បើមិនដូច្នេះ ប្រសិនបើ (sb == &sb->wp->w_scrollbars[SBAR_LEFT])
	    gui.dragged_sb = SBAR_LEFT;
	បើមិនដូច្នេះ
	    gui.dragged_sb = SBAR_RIGHT;
	gui.dragged_wp = sb->wp;
    }
    បើមិនដូច្នេះ
    {
	gui.dragged_sb = SBAR_NONE;
#ifdef FEAT_GUI_GTK
	// Keep the "dragged_wp" value until after the scrolling, for when the
	// mouse button is released.  GTK2 doesn't send the button-up event.
	gui.dragged_wp = NULL;
	// WinScrolled event
	gui_focus_change(TRUE);
#endif
    }

    // Vertical sbar info is kept in the first sbar (the left one)
    ប្រសិនបើ (sb->wp != NULL)
	sb = &sb->wp->w_scrollbars[0];

    /*
     * Check validity of value
     */
    ប្រសិនបើ (value < 0)
	value = 0;
#ifdef SCROLL_PAST_END
    បើមិនដូច្នេះ ប្រសិនបើ (value > sb->max)
	value = sb->max;
#បើមិនដូច្នេះ
    ប្រសិនបើ (value > sb->max - sb->size + 1)
	value = sb->max - sb->size + 1;
#endif

    sb->value = value;

#ifdef USE_ON_FLY_SCROLL
    // When not allowed to do the scrolling right now, return.
    // This also checked input_available(), but that causes the first click in
    // a scrollbar to be ignored when Vim doesn't have focus.
    ប្រសិនបើ (dont_scroll)
	ត្រឡប់;
#endif
    // Disallow scrolling the current window when the completion popup menu is
    // visible.
    ប្រសិនបើ ((sb->wp == NULL || sb->wp == curwin) && pum_visible())
	ត្រឡប់;

#ifdef FEAT_RIGHTLEFT
    ប្រសិនបើ (sb->wp == NULL && curwin->w_p_rl)
    {
	value = sb->max + 1 - sb->size - value;
	ប្រសិនបើ (value < 0)
	    value = 0;
    }
#endif

    ប្រសិនបើ (sb->wp != NULL)		// vertical scrollbar
    {
	sb_num = 0;
	សម្រាប់ (wp = firstwin; wp != sb->wp && wp != NULL; wp = wp->w_next)
	    sb_num++;
	ប្រសិនបើ (wp == NULL)
	    ត្រឡប់;

#ifdef USE_ON_FLY_SCROLL
	current_scrollbar = sb_num;
	scrollbar_value = value;
	ប្រសិនបើ (State & MODE_NORMAL)
	{
	    gui_do_scroll();
	    setcursor();
	}
	បើមិនដូច្នេះ ប្រសិនបើ (State & MODE_INSERT)
	{
	    ins_scroll();
	    setcursor();
	}
	បើមិនដូច្នេះ ប្រសិនបើ (State & MODE_CMDLINE)
	{
	    ប្រសិនបើ (msg_scrolled == 0)
	    {
		gui_do_scroll();
		redrawcmdline();
	    }
	}
# ifdef FEAT_FOLDING
	// Value may have been changed for closed fold.
	sb->value = sb->wp->w_topline - 1;
# endif

	// When dragging one scrollbar and there is another one at the other
	// side move the thumb of that one too.
	ប្រសិនបើ (gui.which_scrollbars[SBAR_RIGHT] && gui.which_scrollbars[SBAR_LEFT])
	    gui_mch_set_scrollbar_thumb(
		    &sb->wp->w_scrollbars[
			    sb == &sb->wp->w_scrollbars[SBAR_RIGHT]
						    ? SBAR_LEFT : SBAR_RIGHT],
		    sb->value, sb->size, sb->max);

#បើមិនដូច្នេះ
	bytes[0] = CSI;
	bytes[1] = KS_VER_SCROLLBAR;
	bytes[2] = KE_FILLER;
	bytes[3] = (char_u)sb_num;
	byte_count = 4;
#endif
    }
    បើមិនដូច្នេះ
    {
#ifdef USE_ON_FLY_SCROLL
	scrollbar_value = value;

	ប្រសិនបើ (State & MODE_NORMAL)
	    do_mousescroll_horiz(scrollbar_value);
	បើមិនដូច្នេះ ប្រសិនបើ (State & MODE_INSERT)
	    ins_horscroll();
	បើមិនដូច្នេះ ប្រសិនបើ (State & MODE_CMDLINE)
	{
	    ប្រសិនបើ (msg_scrolled == 0)
	    {
		do_mousescroll_horiz(scrollbar_value);
		redrawcmdline();
	    }
	}
	ប្រសិនបើ (old_leftcol != curwin->w_leftcol)
	{
	    updateWindow(curwin);   // update window, status and cmdline
	    setcursor();
	}
#បើមិនដូច្នេះ
	bytes[0] = CSI;
	bytes[1] = KS_HOR_SCROLLBAR;
	bytes[2] = KE_FILLER;
	byte_count = 3;
#endif
    }

#ifdef USE_ON_FLY_SCROLL
    /*
     * synchronize other windows, as necessary according to 'scrollbind'
     */
    ប្រសិនបើ (curwin->w_p_scb
	    && ((sb->wp == NULL && curwin->w_leftcol != old_leftcol)
		|| (sb->wp == curwin && (curwin->w_topline != old_topline
# ifdef FEAT_DIFF
					   || curwin->w_topfill != old_topfill
# endif
			))))
    {
	do_check_scrollbind(TRUE);
	// need to update the window right here
	FOR_ALL_WINDOWS(wp)
	    ប្រសិនបើ (wp->w_redr_type > 0)
		updateWindow(wp);
	setcursor();
    }
    out_flush_cursor(FALSE, TRUE);
#បើមិនដូច្នេះ
    add_to_input_buf(bytes, byte_count);
    add_long_to_buf((long_u)value, bytes);
    add_to_input_buf_csi(bytes, parimaa_na(long_u));
#endif
}

/*
 * Scrollbar stuff:
 */

/*
 * Called when something in the window layout has changed.
 */
    void
gui_may_update_scrollbars(void)
{
    ប្រសិនបើ (gui.in_use && starting == 0)
    {
	out_flush();
	gui_init_which_components(NULL);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
}

    void
gui_update_scrollbars(
    ចំនួនគត់		force)	    // Force all scrollbars to get updated
{
    win_T	*wp;
    scrollbar_T	*sb;
    long	val, size, max;		// need 32 bits here
    ចំនួនគត់		which_sb;
    ចំនួនគត់		h, y;
    static win_T *prev_curwin = NULL;

    // Update the horizontal scrollbar
    gui_update_horiz_scrollbar(force);

#ifndef MSWIN
    // Return straight away if there is neither a left nor right scrollbar.
    // On MS-Windows this is required anyway for scrollwheel messages.
    ប្រសិនបើ (!gui.which_scrollbars[SBAR_LEFT] && !gui.which_scrollbars[SBAR_RIGHT])
	ត្រឡប់;
#endif

    /*
     * Don't want to update a scrollbar while we're dragging it.  But if we
     * have both a left and right scrollbar, and we drag one of them, we still
     * need to update the other one.
     */
    ប្រសិនបើ (!force && (gui.dragged_sb == SBAR_LEFT || gui.dragged_sb == SBAR_RIGHT)
	    && gui.which_scrollbars[SBAR_LEFT]
	    && gui.which_scrollbars[SBAR_RIGHT])
    {
	/*
	 * If we have two scrollbars and one of them is being dragged, just
	 * copy the scrollbar position from the dragged one to the other one.
	 */
	which_sb = SBAR_LEFT + SBAR_RIGHT - gui.dragged_sb;
	ប្រសិនបើ (gui.dragged_wp != NULL)
	    gui_mch_set_scrollbar_thumb(
		    &gui.dragged_wp->w_scrollbars[which_sb],
		    gui.dragged_wp->w_scrollbars[0].value,
		    gui.dragged_wp->w_scrollbars[0].size,
		    gui.dragged_wp->w_scrollbars[0].max);
    }

    // avoid that moving components around generates events
    ++hold_gui_events;

    FOR_ALL_WINDOWS(wp)
    {
	ប្រសិនបើ (wp->w_buffer == NULL)	// just in case
	    continue;
	// Skip a scrollbar that is being dragged.
	ប្រសិនបើ (!force && (gui.dragged_sb == SBAR_LEFT
					     || gui.dragged_sb == SBAR_RIGHT)
		&& gui.dragged_wp == wp)
	    continue;

#ifdef SCROLL_PAST_END
	max = wp->w_buffer->b_ml.ml_line_count - 1;
#បើមិនដូច្នេះ
	max = wp->w_buffer->b_ml.ml_line_count + wp->w_height - 2;
#endif
	ប្រសិនបើ (max < 0)			// empty buffer
	    max = 0;
	val = wp->w_topline - 1;
	size = wp->w_height;
#ifdef SCROLL_PAST_END
	ប្រសិនបើ (val > max)			// just in case
	    val = max;
#បើមិនដូច្នេះ
	ប្រសិនបើ (size > max + 1)		// just in case
	    size = max + 1;
	ប្រសិនបើ (val > max - size + 1)
	    val = max - size + 1;
#endif
	ប្រសិនបើ (val < 0)			// minimal value is 0
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
	ប្រសិនបើ (size < 1 || wp->w_botline - 2 > max)
	{
	    /*
	     * This can happen during changing files.  Just don't update the
	     * scrollbar for now.
	     */
	    sb->height = 0;	    // Force update next time
	    ប្រសិនបើ (gui.which_scrollbars[SBAR_LEFT])
		gui_do_scrollbar(wp, SBAR_LEFT, FALSE);
	    ប្រសិនបើ (gui.which_scrollbars[SBAR_RIGHT])
		gui_do_scrollbar(wp, SBAR_RIGHT, FALSE);
	    continue;
	}
	ប្រសិនបើ (force || sb->height != wp->w_height
	    || sb->top != wp->w_winrow
	    || sb->status_height != wp->w_status_height
	    || sb->width != wp->w_width
	    || prev_curwin != curwin)
	{
	    // Height, width or position of scrollbar has changed.  For
	    // vertical split: curwin changed.
	    sb->height = wp->w_height;
	    sb->top = wp->w_winrow;
	    sb->status_height = wp->w_status_height;
	    sb->width = wp->w_width;

	    // Calculate height and position in pixels
	    h = (sb->height + sb->status_height) * gui.char_height;
	    y = sb->top * gui.char_height + gui.border_offset;
#ប្រសិនបើ defined(FEAT_MENU) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF) && !defined(FEAT_GUI_PHOTON)
	    ប្រសិនបើ (gui.menu_is_active)
		y += gui.menu_height;
#endif

#ប្រសិនបើ defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU))
	    ប្រសិនបើ (vim_strchr(p_go, GO_TOOLBAR) != NULL)
		y += gui.toolbar_height;
#endif

#ប្រសិនបើ defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
	    ប្រសិនបើ (gui_has_tabline())
		y += gui.tabline_height;
#endif

	    ប្រសិនបើ (wp->w_winrow == 0)
	    {
		// Height of top scrollbar includes width of top border
		h += gui.border_offset;
		y -= gui.border_offset;
	    }
	    ប្រសិនបើ (gui.which_scrollbars[SBAR_LEFT])
	    {
		gui_mch_set_scrollbar_pos(&wp->w_scrollbars[SBAR_LEFT],
					  gui.left_sbar_x, y,
					  gui.scrollbar_width, h);
		gui_do_scrollbar(wp, SBAR_LEFT, TRUE);
	    }
	    ប្រសិនបើ (gui.which_scrollbars[SBAR_RIGHT])
	    {
		gui_mch_set_scrollbar_pos(&wp->w_scrollbars[SBAR_RIGHT],
					  gui.right_sbar_x, y,
					  gui.scrollbar_width, h);
		gui_do_scrollbar(wp, SBAR_RIGHT, TRUE);
	    }
	}

	ប្រសិនបើ (force || sb->value != val || sb->size != size || sb->max != max)
	{
	    // Thumb of scrollbar has moved
	    sb->value = val;
	    sb->size = size;
	    sb->max = max;
	    ប្រសិនបើ (gui.which_scrollbars[SBAR_LEFT]
		    && (gui.dragged_sb != SBAR_LEFT || gui.dragged_wp != wp))
		gui_mch_set_scrollbar_thumb(&wp->w_scrollbars[SBAR_LEFT],
					    val, size, max);
	    ប្រសិនបើ (gui.which_scrollbars[SBAR_RIGHT]
		    && (gui.dragged_sb != SBAR_RIGHT || gui.dragged_wp != wp))
		gui_mch_set_scrollbar_thumb(&wp->w_scrollbars[SBAR_RIGHT],
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
    static void
gui_do_scrollbar(
    win_T	*wp,
    ចំនួនគត់		which,	    // SBAR_LEFT or SBAR_RIGHT
    ចំនួនគត់		enable)	    // TRUE to enable scrollbar
{
    ចំនួនគត់		midcol = curwin->w_wincol + curwin->w_width / 2;
    ចំនួនគត់		has_midcol = (wp->w_wincol <= midcol
				     && wp->w_wincol + wp->w_width >= midcol);

    // Only enable scrollbars that contain the middle column of the current
    // window.
    ប្រសិនបើ (gui.which_scrollbars[SBAR_RIGHT] != gui.which_scrollbars[SBAR_LEFT])
    {
	// Scrollbars only on one side.  Don't enable scrollbars that don't
	// contain the middle column of the current window.
	ប្រសិនបើ (!has_midcol)
	    enable = FALSE;
    }
    បើមិនដូច្នេះ
    {
	// Scrollbars on both sides.  Don't enable scrollbars that neither
	// contain the middle column of the current window nor are on the far
	// side.
	ប្រសិនបើ (midcol > Columns / 2)
	{
	    ប្រសិនបើ (which == SBAR_LEFT ? wp->w_wincol != 0 : !has_midcol)
		enable = FALSE;
	}
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (which == SBAR_RIGHT ? wp->w_wincol + wp->w_width != Columns
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
    ចំនួនគត់
gui_do_scroll(void)
{
    win_T	*wp, *save_wp;
    ចំនួនគត់		i;
    long	nlines;
    pos_T	old_cursor;
    linenr_T	old_topline;
#ifdef FEAT_DIFF
    ចំនួនគត់		old_topfill;
#endif

    សម្រាប់ (wp = firstwin, i = 0; i < current_scrollbar; wp = W_NEXT(wp), i++)
	ប្រសិនបើ (wp == NULL)
	    bha_nga;
    ប្រសិនបើ (wp == NULL)
	// Couldn't find window
	ត្រឡប់ FALSE;
    // don't redraw, LineOffset and similar are not valid!
    ប្រសិនបើ (exmode_active)
	ត្រឡប់ FALSE;

    /*
     * Compute number of lines to scroll.  If zero, nothing to do.
     */
    nlines = (long)scrollbar_value + 1 - (long)wp->w_topline;
    ប្រសិនបើ (nlines == 0)
	ត្រឡប់ FALSE;

    save_wp = curwin;
    old_topline = wp->w_topline;
#ifdef FEAT_DIFF
    old_topfill = wp->w_topfill;
#endif
    old_cursor = wp->w_cursor;
    curwin = wp;
    curbuf = wp->w_buffer;
    ប្រសិនបើ (nlines < 0)
	scrolldown(-nlines, gui.dragged_wp == NULL);
    បើមិនដូច្នេះ
	scrollup(nlines, gui.dragged_wp == NULL);
    // Reset dragged_wp after using it.  "dragged_sb" will have been reset for
    // the mouse-up event already, but we still want it to behave like when
    // dragging.  But not the next click in an arrow.
    ប្រសិនបើ (gui.dragged_sb == SBAR_NONE)
	gui.dragged_wp = NULL;

    ប្រសិនបើ (old_topline != wp->w_topline
#ifdef FEAT_DIFF
	    || old_topfill != wp->w_topfill
#endif
	    )
    {
	ប្រសិនបើ (get_scrolloff_value() != 0)
	{
	    cursor_correct();		// fix window for 'so'
	    update_topline();		// avoid up/down jump
	}
	ប្រសិនបើ (old_cursor.lnum != wp->w_cursor.lnum)
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
    ប្រសិនបើ (ScreenLines != NULL && (old_topline != wp->w_topline
	    || wp->w_redr_type != 0
#ifdef FEAT_DIFF
	    || old_topfill != wp->w_topfill
#endif
	    ))
    {
	ចំនួនគត់ type = UPD_VALID;

	ប្រសិនបើ (pum_visible())
	{
	    type = UPD_NOT_VALID;
	    wp->w_lines_valid = 0;
	}

	// Don't set must_redraw here, it may cause the popup menu to
	// disappear when losing focus after a scrollbar drag.
	ប្រសិនបើ (wp->w_redr_type < type)
	    wp->w_redr_type = type;
	mch_disable_flush();
	updateWindow(wp);   // update window, status line, and cmdline
	mch_enable_flush();
    }

    // May need to redraw the popup menu.
    ប្រសិនបើ (pum_visible())
	pum_redraw();

    ត្រឡប់ (wp == curwin && !EQUAL_POS(curwin->w_cursor, old_cursor));
}

/*
 * Horizontal scrollbar stuff:
 */
    static void
gui_update_horiz_scrollbar(ចំនួនគត់ force)
{
    long	value, size, max;

    ប្រសិនបើ (!gui.which_scrollbars[SBAR_BOTTOM])
	ត្រឡប់;

    ប្រសិនបើ (!force && gui.dragged_sb == SBAR_BOTTOM)
	ត្រឡប់;

    ប្រសិនបើ (!force && curwin->w_p_wrap && gui.prev_wrap)
	ត្រឡប់;

    /*
     * It is possible for the cursor to be invalid if we're in the middle of
     * something (like changing files).  If so, don't do anything for now.
     */
    ប្រសិនបើ (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
    {
	gui.bottom_sbar.value = -1;
	ត្រឡប់;
    }

    size = curwin->w_width;
    ប្រសិនបើ (curwin->w_p_wrap)
    {
	value = 0;
#ifdef SCROLL_PAST_END
	max = 0;
#បើមិនដូច្នេះ
	max = curwin->w_width - 1;
#endif
    }
    បើមិនដូច្នេះ
    {
	value = curwin->w_leftcol;
	max = scroll_line_len(ui_find_longest_lnum());

	ប្រសិនបើ (virtual_active())
	{
	    // May move the cursor even further to the right.
	    ប្រសិនបើ (curwin->w_virtcol >= (colnr_T)max)
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
    ប្រសិនបើ (value > max - size + 1)
	value = max - size + 1;	    // limit the value to allowable range
#endif

#ifdef FEAT_RIGHTLEFT
    ប្រសិនបើ (curwin->w_p_rl)
    {
	value = max + 1 - size - value;
	ប្រសិនបើ (value < 0)
	{
	    size += value;
	    value = 0;
	}
    }
#endif
    ប្រសិនបើ (!force && value == gui.bottom_sbar.value && size == gui.bottom_sbar.size
						&& max == gui.bottom_sbar.max)
	ត្រឡប់;

    gui.bottom_sbar.value = value;
    gui.bottom_sbar.size = size;
    gui.bottom_sbar.max = max;
    gui.prev_wrap = curwin->w_p_wrap;

    gui_mch_set_scrollbar_thumb(&gui.bottom_sbar, value, size, max);
}

/*
 * Check that none of the colors are the same as the background color
 */
    void
gui_check_colors(void)
{
    ប្រសិនបើ (gui.norm_pixel == gui.back_pixel || gui.norm_pixel == INVALCOLOR)
    {
	gui_set_bg_color((char_u *)"White");
	ប្រសិនបើ (gui.norm_pixel == gui.back_pixel || gui.norm_pixel == INVALCOLOR)
	    gui_set_fg_color((char_u *)"Black");
    }
}

    static void
gui_set_fg_color(char_u *name)
{
    gui.norm_pixel = gui_get_color(name);
    hl_set_fg_color_name(vim_strsave(name));
}

    static void
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

    ប្រសិនបើ (*name == NUL)
	ត្រឡប់ INVALCOLOR;
    t = gui_mch_get_color(name);

    ចំនួនគត់ is_none = STRCMP(name, "none") == 0;
    ប្រសិនបើ (t == INVALCOLOR
#ប្រសិនបើ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
	    && (gui.in_use || is_none)
#endif
	    )
    {
	ប្រសិនបើ (is_none)
	    emsg(_(e_cannot_use_color_none_did_you_mean_none));
	បើមិនដូច្នេះ
	    semsg(_(e_cannot_allocate_color_str), name);
    }
    ត្រឡប់ t;
}

/*
 * Return the grey value of a color (range 0-255).
 */
    ចំនួនគត់
gui_get_lightness(guicolor_T pixel)
{
    long_u	rgb = (long_u)gui_mch_get_rgb(pixel);

    ត្រឡប់  (ចំនួនគត់)(  (((rgb >> 16) & 0xff) * 299)
		   + (((rgb >> 8) & 0xff) * 587)
		   +  ((rgb	  & 0xff) * 114)) / 1000;
}

    char_u *
gui_bg_default(void)
{
    ប្រសិនបើ (gui_get_lightness(gui.back_pixel) < 127)
	ត្រឡប់ (char_u *)"dark";
    ត្រឡប់ (char_u *)"light";
}

/*
 * Option initializations that can only be done after opening the GUI window.
 */
    static void
init_gui_options(void)
{
    // Set the 'background' option according to the lightness of the
    // background color, unless the user has set it already.
    ប្រសិនបើ (!option_was_set((char_u *)"bg") && STRCMP(p_bg, gui_bg_default()) != 0)
    {
	set_option_value_give_err((char_u *)"bg", 0L, gui_bg_default(), 0);
	highlight_changed();
    }
}

#ប្រសិនបើ defined(FEAT_GUI_X11)
    void
gui_new_scrollbar_colors(void)
{
    win_T	*wp;

    // Nothing to do if GUI hasn't started yet.
    ប្រសិនបើ (!gui.in_use)
	ត្រឡប់;

    FOR_ALL_WINDOWS(wp)
    {
	gui_mch_set_scrollbar_colors(&(wp->w_scrollbars[SBAR_LEFT]));
	gui_mch_set_scrollbar_colors(&(wp->w_scrollbars[SBAR_RIGHT]));
    }
    gui_mch_set_scrollbar_colors(&gui.bottom_sbar);
}
#endif

/*
 * Call this when focus has changed.
 */
    void
gui_focus_change(ចំនួនគត់ in_focus)
{
/*
 * Skip this code to avoid drawing the cursor when debugging and switching
 * between the debugger window and gvim.
 */
#ប្រសិនបើ 1
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
    ប្រសិនបើ (!hold_gui_events
# ប្រសិនបើ defined(FEAT_GUI_GTK) && defined(FEAT_GUI_DIALOG)
	    && gui.dialogs_active == 0
# endif
       )
    {
	char_u  bytes[3];

	bytes[0] = CSI;
	bytes[1] = KS_EXTRA;
	bytes[2] = in_focus ? (ចំនួនគត់)KE_FOCUSGAINED : (ចំនួនគត់)KE_FOCUSLOST;
	add_to_input_buf(bytes, 3);
    }
#endif
}

/*
 * When mouse moved: apply 'mousefocus'.
 * Also updates the mouse pointer shape.
 */
    static void
gui_mouse_focus(ចំនួនគត់ x, ចំនួនគត់ y)
{
    win_T	*wp;
    char_u	st[8];

#ifdef FEAT_MOUSESHAPE
    // Get window pointer, and update mouse shape as well.
    wp = xy2win(x, y, IGNORE_POPUP);
#endif

    // Only handle this when 'mousefocus' set and ...
    ប្រសិនបើ (p_mousef
	    && !hold_gui_events		// not holding events
	    && (State & (MODE_NORMAL | MODE_INSERT))// Normal/Visual/Insert mode
	    && State != MODE_HITRETURN	// but not hit-return prompt
	    && msg_scrolled == 0	// no scrolled message
	    && !need_mouse_correct	// not moving the pointer
	    && gui.in_focus)		// gvim in focus
    {
	// Don't move the mouse when it's left or right of the Vim window
	ប្រសិនបើ (x < 0 || x > Columns * gui.char_width)
	    ត្រឡប់;
#ifndef FEAT_MOUSESHAPE
	wp = xy2win(x, y, IGNORE_POPUP);
#endif
	ប្រសិនបើ (wp == curwin || wp == NULL)
	    ត្រឡប់;	// still in the same old window, or none at all

	// Ignore position in the tab pages line.
	ប្រសិនបើ (Y_2_ROW(y) < tabline_height())
	    ត្រឡប់;

	/*
	 * Format a mouse click on status line input,
	 * ala gui_send_mouse_event(0, x, y, 0, 0);
	 * Trick: Use a column number -1, so that get_pseudo_mouse_code() will
	 * generate a K_LEFTMOUSE_NM key code.
	 */
	ប្រសិនបើ (finish_op)
	{
	    // abort the current operator first
	    st[0] = ESC;
	    add_to_input_buf(st, 1);
	}
	st[0] = CSI;
	st[1] = KS_MOUSE;
	st[2] = KE_FILLER;
	st[3] = (char_u)MOUSE_LEFT;
	fill_mouse_coord(st + 4,
		wp->w_wincol == 0 ? -1 : wp->w_wincol + MOUSE_COLOFF,
		wp->w_height + W_WINROW(wp));

	add_to_input_buf(st, 8);
	st[3] = (char_u)MOUSE_RELEASE;
	add_to_input_buf(st, 8);
#ifdef FEAT_GUI_GTK
	// Need to wake up the main loop
	ប្រសិនបើ (gtk_main_level() > 0)
	    gtk_main_quit();
#endif
    }
}

/*
 * Called when the mouse moved (but not when dragging).
 */
    void
gui_mouse_moved(ចំនួនគត់ x, ចំនួនគត់ y)
{
    // Ignore this while still starting up.
    ប្រសិនបើ (!gui.in_use || gui.starting)
	ត្រឡប់;

    // apply 'mousefocus' and pointer shape
    gui_mouse_focus(x, y);

    ប្រសិនបើ (p_mousemev
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
    ចំនួនគត់		x, y;

    ប្រសិនបើ (!(gui.in_use && (p_mousef || popup == FIND_POPUP)))
	ត្រឡប់ NULL;
    gui_mch_getmouse(&x, &y);

    // Only use the mouse when it's on the Vim window
    ប្រសិនបើ (x >= 0 && x <= Columns * gui.char_width
	    && y >= 0 && Y_2_ROW(y) >= tabline_height())
	ត្រឡប់ xy2win(x, y, popup);
    ត្រឡប់ NULL;
}

/*
 * Called when mouse should be moved to window with focus.
 */
    void
gui_mouse_correct(void)
{
    win_T	*wp = NULL;

    need_mouse_correct = FALSE;

    wp = gui_mouse_window(IGNORE_POPUP);
    ប្រសិនបើ (wp == curwin || wp == NULL)
	ត្រឡប់;

    // If in other than current window
    validate_cline_row();
    gui_mch_setmouse((ចំនួនគត់)W_ENDCOL(curwin) * gui.char_width - 3,
	    (W_WINROW(curwin) + curwin->w_wrow) * gui.char_height
	    + (gui.char_height) / 2);
}

/*
 * Find window where the mouse pointer "x" / "y" coordinate is in.
 * As a side effect update the shape of the mouse pointer.
 */
    static win_T *
xy2win(ចំនួនគត់ x, ចំនួនគត់ y, mouse_find_T popup)
{
    ចំនួនគត់		row;
    ចំនួនគត់		col;
    win_T	*wp;

    row = Y_2_ROW(y);
    col = X_2_COL(x);

    ប្រសិនបើ (row < 0 || col < 0)		// before first window
	ត្រឡប់ NULL;
    wp = mouse_find_win(&row, &col, popup);
    ប្រសិនបើ (wp == NULL)
    {
#ifdef FEAT_MOUSESHAPE
	update_mouseshape(-2);
#endif
	ត្រឡប់ NULL;
    }
#ifdef FEAT_MOUSESHAPE
    ប្រសិនបើ (State == MODE_HITRETURN || State == MODE_ASKMORE)
    {
	ប្រសិនបើ (Y_2_ROW(y) >= msg_row)
	    update_mouseshape(SHAPE_IDX_MOREL);
	បើមិនដូច្នេះ
	    update_mouseshape(SHAPE_IDX_MORE);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (row >= wp->w_height + wp->w_status_height)	// below status line
	update_mouseshape(SHAPE_IDX_CLINE);
    បើមិនដូច្នេះ ប្រសិនបើ (!(State & MODE_CMDLINE) && wp->w_vsep_width > 0 && col == wp->w_width
	    && (!(row >= wp->w_height && row < wp->w_height
	    + wp->w_status_height) || !stl_connected(wp)) && msg_scrolled == 0)
	update_mouseshape(SHAPE_IDX_VSEP);
    បើមិនដូច្នេះ ប្រសិនបើ (!(State & MODE_CMDLINE) && wp->w_status_height > 0
	    && row >= wp->w_height && row < wp->w_height + wp->w_status_height
	    && msg_scrolled == 0)
	update_mouseshape(SHAPE_IDX_STATUS);
    បើមិនដូច្នេះ
	update_mouseshape(-2);
#endif
    ត្រឡប់ wp;
}

/*
 * ":gui" and ":gvim": Change from the terminal version to the GUI version.
 * File names may be given to redefine the args list.
 */
    void
ex_gui(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    /*
     * Check for "-f" argument: foreground, don't fork.
     * Also don't fork when started with "gvim -f".
     * Do fork when using "gui -b".
     */
    ប្រសិនបើ (arg[0] == '-'
	    && (arg[1] == 'f' || arg[1] == 'b')
	    && (arg[2] == NUL || VIM_ISWHITE(arg[2])))
    {
	gui.dofork = (arg[1] == 'b');
	eap->arg = skipwhite(eap->arg + 2);
    }
    ប្រសិនបើ (!gui.in_use)
    {
#ប្រសិនបើ defined(VIMDLL) && !defined(EXPERIMENTAL_GUI_CMD)
	ប្រសិនបើ (!gui.starting)
	{
	    emsg(_(e_gui_cannot_be_used_not_enabled_at_compile_time));
	    ត្រឡប់;
	}
#endif
	// Clear the command.  Needed for when forking+exiting, to avoid part
	// of the argument ending up after the shell prompt.
	msg_clr_eos_force();
#ifdef GUI_MAY_SPAWN
	ប្រសិនបើ (!ends_excmd2(eap->cmd, eap->arg))
	    gui_start(eap->arg);
	បើមិនដូច្នេះ
#endif
	    gui_start(NULL);
#ifdef FEAT_JOB_CHANNEL
	channel_gui_register_all();
#endif
    }
    ប្រសិនបើ (!ends_excmd2(eap->cmd, eap->arg))
	ex_next(eap);
}

#ប្រសិនបើ (defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	    || defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON) \
	    || defined(FEAT_GUI_HAIKU)) \
	    && defined(FEAT_TOOLBAR)
/*
 * This is shared between Haiku, Motif, and GTK.
 */

/*
 * Callback function for do_in_runtimepath().
 */
    static void
gfp_setname(char_u *fname, void *cookie)
{
    char_u	*gfp_buffer = cookie;

    ប្រសិនបើ (STRLEN(fname) >= MAXPATHL)
	*gfp_buffer = NUL;
    បើមិនដូច្នេះ
	STRCPY(gfp_buffer, fname);
}

/*
 * Find the path of bitmap "name" with extension "ext" in 'runtimepath'.
 * Return FAIL for failure and OK if buffer[MAXPATHL] contains the result.
 */
    ចំនួនគត់
gui_find_bitmap(char_u *name, char_u *buffer, var_na *ext)
{
    ប្រសិនបើ (STRLEN(name) > MAXPATHL - 14)
	ត្រឡប់ FAIL;
    vim_snprintf((var_na *)buffer, MAXPATHL, "bitmaps/%s.%s", name, ext);
    ប្រសិនបើ (do_in_runtimepath(buffer, 0, gfp_setname, buffer) == FAIL
							    || *buffer == NUL)
	ត្រឡប់ FAIL;
    ត្រឡប់ OK;
}

# ប្រសិនបើ !defined(FEAT_GUI_GTK)
/*
 * Given the name of the "icon=" argument, try finding the bitmap file for the
 * icon.  If it is an absolute path name, use it as it is.  Otherwise append
 * "ext" and search for it in 'runtimepath'.
 * The result is put in "buffer[MAXPATHL]".  If something fails "buffer"
 * contains "name".
 */
    void
gui_find_iconfile(char_u *name, char_u *buffer, var_na *ext)
{
    char_u	buf[MAXPATHL + 1];

    expand_env(name, buffer, MAXPATHL);
    ប្រសិនបើ (!mch_isFullName(buffer) && gui_find_bitmap(buffer, buf, ext) == OK)
	STRCPY(buffer, buf);
}
# endif
#endif

#ប្រសិនបើ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)|| defined(FEAT_GUI_HAIKU)
    void
display_errors(void)
{
    char_u	*p;

    ប្រសិនបើ (isatty(2))
    {
	fflush(stderr);
	ត្រឡប់;
    }

    ប្រសិនបើ (error_ga.ga_data == NULL)
	ត្រឡប់;

    // avoid putting up a message box with blanks only
    សម្រាប់ (p = (char_u *)error_ga.ga_data; *p != NUL; ++p)
	ប្រសិនបើ (!SAFE_isspace(*p))
	{
	    // Truncate a very long message, it will go off-screen.
	    ប្រសិនបើ (STRLEN(p) > 2000)
		STRCPY(p + 2000 - 14, "...(truncated)");
	    (void)do_dialog(VIM_ERROR, (char_u *)_("Error"),
		    p, (char_u *)_("&Ok"), 1, NULL, FALSE);
	    bha_nga;
	}
    ga_clear(&error_ga);
}
#endif

#ប្រសិនបើ defined(NO_CONSOLE_INPUT)
/*
 * Return TRUE if still starting up and there is no place to enter text.
 * For GTK and X11 we check if stderr is not a tty, which means we were
 * (probably) started from the desktop.  Also check stdin, "vim >& file" does
 * allow typing on stdin.
 */
    ចំនួនគត់
no_console_input(void)
{
    ត្រឡប់ ((!gui.in_use || gui.starting)
# ifndef NO_CONSOLE
	    && !isatty(0) && !isatty(2)
# endif
	    );
}
#endif

#ប្រសិនបើ defined(FIND_REPLACE_DIALOG) \
	|| defined(NEED_GUI_UPDATE_SCREEN)
/*
 * Update the current window and the screen.
 */
    void
gui_update_screen(void)
{
# ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_line = 0;
    linenr_T	conceal_new_cursor_line = 0;
    ចំនួនគត់		conceal_update_lines = FALSE;
# endif

    update_topline();
    validate_cursor();

    // Trigger CursorMoved if the cursor moved.
    ប្រសិនបើ (!finish_op && (has_cursormoved()
# ifdef FEAT_PROP_POPUP
		|| popup_visible
# endif
# ifdef FEAT_CONCEAL
		|| curwin->w_p_cole > 0
# endif
		) && !EQUAL_POS(last_cursormoved, curwin->w_cursor))
    {
	ប្រសិនបើ (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, NULL, NULL, FALSE, curbuf);
# ifdef FEAT_PROP_POPUP
	ប្រសិនបើ (popup_visible)
	    popup_check_cursor_pos();
# endif
# ifdef FEAT_CONCEAL
	ប្រសិនបើ (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_line = last_cursormoved.lnum;
	    conceal_new_cursor_line = curwin->w_cursor.lnum;
	    conceal_update_lines = TRUE;
	}
# endif
	last_cursormoved = curwin->w_cursor;
    }

    ប្រសិនបើ (!finish_op)
	may_trigger_win_scrolled_resized();

# ifdef FEAT_CONCEAL
    ប្រសិនបើ (conceal_update_lines
	    && (conceal_old_cursor_line != conceal_new_cursor_line
		|| conceal_cursor_line(curwin)
		|| need_cursor_line_redraw))
    {
	ប្រសិនបើ (conceal_old_cursor_line != conceal_new_cursor_line)
	    redrawWinline(curwin, conceal_old_cursor_line);
	redrawWinline(curwin, conceal_new_cursor_line);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_line_redraw = FALSE;
    }
# endif
    update_screen(0);	// may need to update the screen
    setcursor();
    out_flush_cursor(TRUE, FALSE);
}
#endif

#ប្រសិនបើ defined(FIND_REPLACE_DIALOG)
/*
 * Get the text to use in a find/replace dialog.  Uses the last search pattern
 * if the argument is empty.
 * Returns an allocated string.
 */
    char_u *
get_find_dialog_text(
    char_u	*arg,
    ចំនួនគត់		*wwordp,	// return: TRUE if \< \> found
    ចំនួនគត់		*mcasep)	// return: TRUE if \C found
{
    char_u	*text;

    ប្រសិនបើ (*arg == NUL)
	text = last_search_pat();
    បើមិនដូច្នេះ
	text = arg;
    ប្រសិនបើ (text != NULL)
    {
	ចំនួនគត់ len = (ចំនួនគត់)STRLEN(text);

	text = vim_strnsave(text, len);
	ប្រសិនបើ (text != NULL)
	{
	    ចំនួនគត់ i;

	    // Remove "\V"
	    ប្រសិនបើ (len >= 2 && STRNCMP(text, "\\V", 2) == 0)
	    {
		mch_memmove(text, text + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\c" and "\C" and remove.
	    ប្រសិនបើ (len >= 2 && *text == '\\' && (text[1] == 'c' || text[1] == 'C'))
	    {
		*mcasep = (text[1] == 'C');
		mch_memmove(text, text + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\<text\>" and remove.
	    ប្រសិនបើ (len >= 4
		    && STRNCMP(text, "\\<", 2) == 0
		    && STRNCMP(text + len - 2, "\\>", 2) == 0)
	    {
		*wwordp = TRUE;
		mch_memmove(text, text + 2, (size_t)(len - 4));
		text[len - 4] = NUL;
	    }

	    // Recognize "\/" or "\?" and remove.
	    សម្រាប់ (i = 0; i + 1 < len; ++i)
		ប្រសិនបើ (text[i] == '\\' && (text[i + 1] == '/'
						       || text[i + 1] == '?'))
		{
		    mch_memmove(text + i, text + i + 1, (size_t)(len - i));
		    --len;
		}
	}
    }
    ត្រឡប់ text;
}

/*
 * Handle the press of a button in the find-replace dialog.
 * Return TRUE when something was added to the input buffer.
 */
    ចំនួនគត់
gui_do_findrepl(
    ចំនួនគត់		flags,		// one of FRD_REPLACE, FRD_FINDNEXT, etc.
    char_u	*find_text,
    char_u	*repl_text,
    ចំនួនគត់		down)		// Search downwards.
{
    garray_T	ga;
    ចំនួនគត់		i;
    ចំនួនគត់		type = (flags & FRD_TYPE_MASK);
    char_u	*p;
    regmatch_T	regmatch;
    ចំនួនគត់		save_did_emsg = did_emsg;
    static ចំនួនគត់  busy = FALSE;

    // When the screen is being updated we should not change buffers and
    // windows structures, it may cause freed memory to be used.  Also don't
    // do this recursively (pressing "Find" quickly several times).
    ប្រសិនបើ (updating_screen || busy)
	ត្រឡប់ FALSE;

    // refuse replace when text cannot be changed
    ប្រសិនបើ ((type == FRD_REPLACE || type == FRD_REPLACEALL) && text_locked())
	ត្រឡប់ FALSE;

    busy = TRUE;

    ga_init2(&ga, 1, 100);
    ប្រសិនបើ (type == FRD_REPLACEALL)
	GA_CONCAT_LITERAL(&ga, "%s/");

    GA_CONCAT_LITERAL(&ga, "\\V");
    ប្រសិនបើ (flags & FRD_MATCH_CASE)
	GA_CONCAT_LITERAL(&ga, "\\C");
    បើមិនដូច្នេះ
	GA_CONCAT_LITERAL(&ga, "\\c");
    ប្រសិនបើ (flags & FRD_WHOLE_WORD)
	GA_CONCAT_LITERAL(&ga, "\\<");
    // escape slash and backslash
    p = vim_strsave_escaped(find_text, (char_u *)"/\\");
    ប្រសិនបើ (p != NULL)
	ga_concat(&ga, p);
    vim_free(p);
    ប្រសិនបើ (flags & FRD_WHOLE_WORD)
	GA_CONCAT_LITERAL(&ga, "\\>");

    ប្រសិនបើ (type == FRD_REPLACEALL)
    {
	GA_CONCAT_LITERAL(&ga, "/");
	// Escape slash and backslash.
	// Also escape tilde and ampersand if 'magic' is set.
	p = vim_strsave_escaped(repl_text,
				p_magic ? (char_u *)"/\\~&" : (char_u *)"/\\");
	ប្រសិនបើ (p != NULL)
	    ga_concat(&ga, p);
	vim_free(p);
	GA_CONCAT_LITERAL(&ga, "/g");
    }
    ga_append(&ga, NUL);

    ប្រសិនបើ (type == FRD_REPLACE)
    {
	// Do the replacement when the text at the cursor matches.  Thus no
	// replacement is done if the cursor was moved!
	regmatch.regprog = vim_regcomp(ga.ga_data, RE_MAGIC + RE_STRING);
	regmatch.rm_ic = 0;
	ប្រសិនបើ (regmatch.regprog != NULL)
	{
	    p = ml_get_cursor();
	    ប្រសិនបើ (vim_regexec_nl(&regmatch, p, (colnr_T)0)
						   && regmatch.startp[0] == p)
	    {
		// Clear the command line to remove any old "No match"
		// error.
		msg_end_prompt();

		ប្រសិនបើ (u_save_cursor() == OK)
		{
		    // A button was pressed thus undo should be synced.
		    u_sync(FALSE);

		    del_bytes((long)(regmatch.endp[0] - regmatch.startp[0]),
								FALSE, FALSE);
		    ins_str(repl_text, STRLEN(repl_text));
		}
	    }
	    បើមិនដូច្នេះ
		msg(_("No match at cursor, finding next"));
	    vim_regfree(regmatch.regprog);
	}
    }

    ប្រសិនបើ (type == FRD_REPLACEALL)
    {
	// A button was pressed, thus undo should be synced.
	u_sync(FALSE);
	do_cmdline_cmd(ga.ga_data);
    }
    បើមិនដូច្នេះ
    {
	ចំនួនគត់ searchflags = SEARCH_MSG + SEARCH_MARK;

	// Search for the next match.
	// Don't skip text under cursor for single replace.
	ប្រសិនបើ (type == FRD_REPLACE)
	    searchflags += SEARCH_START;
	i = msg_scroll;
	ប្រសិនបើ (down)
	{
	    (void)do_search(NULL, '/', '/', ga.ga_data, STRLEN(ga.ga_data), 1L, searchflags, NULL);
	}
	បើមិនដូច្នេះ
	{
	    // We need to escape '?' if and only if we are searching in the up
	    // direction
	    p = vim_strsave_escaped(ga.ga_data, (char_u *)"?");
	    ប្រសិនបើ (p != NULL)
		(void)do_search(NULL, '?', '?', p, STRLEN(p), 1L, searchflags, NULL);
	    vim_free(p);
	}

	msg_scroll = i;	    // don't let an error message set msg_scroll
    }

    // Don't want to pass did_emsg to other code, it may cause disabling
    // syntax HL if we were busy redrawing.
    did_emsg = save_did_emsg;

    ប្រសិនបើ (State & (MODE_NORMAL | MODE_INSERT))
    {
	gui_update_screen();		// update the screen
	msg_didout = 0;			// overwrite any message
	need_wait_return = FALSE;	// don't wait for return
    }

    vim_free(ga.ga_data);
    busy = FALSE;
    ត្រឡប់ (ga.ga_len > 0);
}

#endif

#ប្រសិនបើ defined(HAVE_DROP_FILE)
/*
 * Jump to the window at specified point (x, y).
 */
    static void
gui_wingoto_xy(ចំនួនគត់ x, ចំនួនគត់ y)
{
    ចំនួនគត់		row = Y_2_ROW(y);
    ចំនួនគត់		col = X_2_COL(x);
    win_T	*wp;

    ប្រសិនបើ (row < 0 || col < 0)
	ត្រឡប់;

    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    ប្រសិនបើ (wp != NULL && wp != curwin)
	win_goto(wp);
}

/*
 * Function passed to handle_drop() for the actions to be done after the
 * argument list has been updated.
 */
    static void
drop_callback(void *cookie)
{
    char_u	*p = cookie;
    ចំនួនគត់		do_shorten = FALSE;

    // If Shift held down, change to first file's directory.  If the first
    // item is a directory, change to that directory (and let the explorer
    // plugin show the contents).
    ប្រសិនបើ (p != NULL)
    {
	ប្រសិនបើ (mch_isdir(p))
	{
	    ប្រសិនបើ (mch_chdir((var_na *)p) == 0)
		do_shorten = TRUE;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (vim_chdirfile(p, "drop") == OK)
	    do_shorten = TRUE;
	vim_free(p);
	ប្រសិនបើ (do_shorten)
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
    void
gui_handle_drop(
    ចំនួនគត់		x UNUSED,
    ចំនួនគត់		y UNUSED,
    int_u	modifiers,
    char_u	**fnames,
    ចំនួនគត់		count)
{
    ចំនួនគត់		i;
    char_u	*p;
    static ចំនួនគត់	entered = FALSE;

    /*
     * This function is called by event handlers.  Just in case we get a
     * second event before the first one is handled, ignore the second one.
     * Not sure if this can ever happen, just in case.
     */
    ប្រសិនបើ (entered)
	ត្រឡប់;
    entered = TRUE;

    /*
     * When the cursor is at the command line, add the file names to the
     * command line, don't edit the files.
     */
    ប្រសិនបើ (State & MODE_CMDLINE)
    {
	shorten_filenames(fnames, count);
	សម្រាប់ (i = 0; i < count; ++i)
	{
	    ប្រសិនបើ (fnames[i] != NULL)
	    {
		ប្រសិនបើ (i > 0)
		    add_to_input_buf((char_u*)" ", 1);

		// We don't know what command is used thus we can't be sure
		// about which characters need to be escaped.  Only escape the
		// most common ones.
# ifdef BACKSLASH_IN_FILENAME
		p = vim_strsave_escaped(fnames[i], (char_u *)" \t\"|");
# បើមិនដូច្នេះ
		p = vim_strsave_escaped(fnames[i], (char_u *)"\\ \t\"|");
# endif
		ប្រសិនបើ (p != NULL)
		    add_to_input_buf_csi(p, (ចំនួនគត់)STRLEN(p));
		vim_free(p);
		vim_free(fnames[i]);
	    }
	}
	vim_free(fnames);
    }
    បើមិនដូច្នេះ
    {
	// Go to the window under mouse cursor, then shorten given "fnames" by
	// current window, because a window can have local current dir.
	gui_wingoto_xy(x, y);
	shorten_filenames(fnames, count);

	// If Shift held down, remember the first item.
	ប្រសិនបើ ((modifiers & MOUSE_SHIFT) != 0)
	    p = vim_strsave(fnames[0]);
	បើមិនដូច្នេះ
	    p = NULL;

	// Handle the drop, :edit or :split to get to the file.  This also
	// frees fnames[].  Skip this if there is only one item, it's a
	// directory and Shift is held down.
	ប្រសិនបើ (count == 1 && (modifiers & MOUSE_SHIFT) != 0
						     && mch_isdir(fnames[0]))
	{
	    vim_free(fnames[0]);
	    vim_free(fnames);
	    vim_free(p);
	}
	បើមិនដូច្នេះ
	    handle_drop(count, fnames, (modifiers & MOUSE_CTRL) != 0,
						     drop_callback, (void *)p);
    }

    entered = FALSE;
}
#endif

/*
 * Check if "key" is to interrupt us.  Handles a key that has not had modifiers
 * applied yet.
 * Return the key with modifiers applied if so, NUL if not.
 */
    ចំនួនគត់
check_for_interrupt(ចំនួនគត់ key, ចំនួនគត់ modifiers_arg)
{
    ចំនួនគត់ modifiers = modifiers_arg;
    ចំនួនគត់ c = merge_modifyOtherKeys(key, &modifiers);

    ប្រសិនបើ ((c == Ctrl_C && ctrl_c_interrupts)
#ifdef UNIX
	    || (intr_char != Ctrl_C && c == intr_char)
#endif
	    )
    {
	got_int = TRUE;
	ត្រឡប់ c;
    }
    ត្រឡប់ NUL;
}

/*
 * If the "--gui-log-file fname" argument is given write the dialog title and
 * message to a file and return TRUE.  Otherwise return FALSE.
 * When there is any problem opening the file or writing to the file this is
 * ignored, showing the dialog might get the test to get stuck.
 */
    ចំនួនគត់
gui_dialog_log(char_u *title, char_u *message)
{
    char_u  *fname = get_gui_dialog_file();
    FILE    *fd;

    ប្រសិនបើ (fname == NULL)
	ត្រឡប់ FALSE;

    fd = mch_fopen((var_na *)fname, "a");
    ប្រសិនបើ (fd != NULL)
    {
	fprintf(fd, "%s: %s\n", title, message);
	fclose(fd);
    }
    ត្រឡប់ TRUE;
}
