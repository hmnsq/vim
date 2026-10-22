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

#если defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
# define USE_SET_GUIFONTWIDE
static void set_guifontwide(char_u *font_name);
#endif
static void gui_check_pos(void);
static void gui_reset_scroll_region(void);
static void gui_outstr(char_u *, целое);
#ifndef USE_GTK4
static целое gui_screenchar(целое off, целое flags, guicolor_T fg, guicolor_T bg, целое back);
#endif
static целое gui_outstr_nowrap(char_u *s, целое len, целое flags, guicolor_T fg, guicolor_T bg, целое back);
static void gui_delete_lines(целое row, целое count);
static void gui_insert_lines(целое row, целое count);
static целое gui_xy2colrow(целое x, целое y, целое *colp);
#если defined(FEAT_GUI_TABLINE)
static целое gui_has_tabline(void);
#endif
static void gui_do_scrollbar(win_T *wp, целое which, целое enable);
static void gui_update_horiz_scrollbar(целое);
static void gui_set_fg_color(char_u *name);
static void gui_set_bg_color(char_u *name);
static void init_gui_options(void);
static win_T *xy2win(целое x, целое y, mouse_find_T popup);

#ifdef GUI_MAY_FORK
static void gui_do_fork(void);

static целое gui_read_child_pipe(целое fd);

// Return values for gui_read_child_pipe
enum {
    GUI_CHILD_IO_ERROR,
    GUI_CHILD_OK,
    GUI_CHILD_FAILED
};
#endif

static void gui_attempt_start(void);

static целое can_update_cursor = TRUE; // can display the cursor
static целое disable_flush = 0;	// If > 0, gui_mch_flush() is disabled.

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
    static целое	recursive = 0;
#endif
#если defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
    char	*msg = NULL;
#endif

    old_term = vim_strsave(T_NAME);

    settmode(TMODE_COOK);		// stop RAW mode
    если (full_screen)
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
    если (gui.dofork && !vim_strchr(p_go, GO_FORG) && recursive <= 1
# ifdef FEAT_JOB_CHANNEL
	    && !job_any_running()
# endif
	    )
    {
	gui_do_fork();
    }
    иначе
#endif
#ifdef GUI_MAY_SPAWN
    если (gui.dospawn
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
    иначе
#endif
    {
#ifdef FEAT_GUI_GTK
	// If there is 'f' in 'guioptions' and specify -g argument,
	// gui_mch_init_check() was not called yet.
	если (gui_mch_init_check() != OK)
	    getout_preserve_modified(1);
#endif
	gui_attempt_start();
    }

    если (!gui.in_use)			// failed to start GUI
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
#если defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
	если (msg != NULL)
	    emsg(msg);
#endif
    }
    иначе
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

#если defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    // Enable fullscreen mode
    если (vim_strchr(p_go, GO_FULLSCREEN) != NULL)
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
    static целое recursive = 0;

    ++recursive;
    gui.starting = true;

#ifdef FEAT_GUI_GTK
    gui.event_time = GDK_CURRENT_TIME;
#endif

    termcapinit((char_u *)"builtin_gui");
    gui.starting = recursive - 1;

#если defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
    если (gui.in_use)
    {
# ifdef FEAT_EVAL
	Window	x11_window;
	Display	*x11_display;

	если (gui_get_x11_windis(&x11_window, &x11_display) == OK)
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
# если defined(HAVE_SYS_WAIT_H)
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
    целое		pipefd[2];	// pipe between parent and child
    целое		pipe_error;
    целое		status;
    целое		exit_status;
    pid_t	pid = -1;

# если defined(FEAT_RELTIME) && defined(PROF_NSEC)
    // a timer is not carried forward
    delete_timer();
# endif

    // Setup a pipe between the child and the parent, so that the parent
    // knows when the child has done the setsid() call and is allowed to
    // exit.
    pipe_error = (pipe(pipefd) < 0);
    pid = fork();
    если (pid < 0)	    // Fork error
    {
	emsg(_(e_failed_to_create_new_process_for_GUI));
	вернуть;
    }
    иначе если (pid > 0)	    // Parent
    {
	// Give the child some time to do the setsid(), otherwise the
	// exit() may kill the child too (when starting gvim from inside a
	// gvim).
	если (!pipe_error)
	{
	    // The read returns when the child closes the pipe (or when
	    // the child dies for some reason).
	    close(pipefd[1]);
	    status = gui_read_child_pipe(pipefd[0]);
	    если (status == GUI_CHILD_FAILED)
	    {
		// The child failed to start the GUI, so the caller must
		// continue. There may be more error information written
		// to stderr by the child.
		waitpid(pid, &exit_status, 0);
		emsg(_(e_the_child_process_failed_to_start_GUI));
		вернуть;
	    }
	    иначе если (status == GUI_CHILD_IO_ERROR)
	    {
		pipe_error = TRUE;
	    }
	    // else GUI_CHILD_OK: parent exit
	}

	если (pipe_error)
	    ui_delay(301L, TRUE);

	// When swapping screens we may need to go to the next line, e.g.,
	// after a hit-enter prompt and using ":gui".
	если (newline_on_exit)
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
    если (gui_mch_init_check() != OK)
	getout_preserve_modified(1);
# endif

# если defined(HAVE_SETSID) || defined(HAVE_SETPGID)
    /*
     * Change our process group.  On some systems/shells a CTRL-C in the
     * shell where Vim was started would otherwise kill gvim!
     */
#  если defined(HAVE_SETSID)
    (void)setsid();
#  иначе
    (void)setpgid(0, 0);
#  endif
# endif
    если (!pipe_error)
	close(pipefd[0]);

# если defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
    // Tell the session manager our new PID
    gui_mch_forked();
# endif

    // Try to start the GUI
    gui_attempt_start();

    // Notify the parent
    если (!pipe_error)
    {
	если (gui.in_use)
	    write_eintr(pipefd[1], "ok", 3);
	иначе
	    write_eintr(pipefd[1], "fail", 5);
	close(pipefd[1]);
    }

    // If we failed to start the GUI, exit now.
    если (!gui.in_use)
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
    static целое
gui_read_child_pipe(целое fd)
{
    long	bytes_read;
# define READ_BUFFER_SIZE 10
    char	buffer[READ_BUFFER_SIZE];

    bytes_read = read_eintr(fd, buffer, READ_BUFFER_SIZE - 1);
# undef READ_BUFFER_SIZE
    close(fd);
    если (bytes_read < 0)
	вернуть GUI_CHILD_IO_ERROR;
    buffer[bytes_read] = NUL;
    если (STRCMP(buffer, "ok") == 0)
	вернуть GUI_CHILD_OK;
    вернуть GUI_CHILD_FAILED;
}

#endif // GUI_MAY_FORK

/*
 * Call this when vim starts up, whether or not the GUI is started
 */
    void
gui_prepare(целое *argc, char **argv)
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
    целое
gui_init_check(void)
{
    static целое result = MAYBE;

    если (result != MAYBE)
    {
	если (result == FAIL)
	    emsg(_(e_cannot_start_the_GUI));
	вернуть result;
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
#  иначе
    gui.menu_font = NOFONT;
#  endif
# endif
    gui.menu_is_active = true;	    // default: include menu
# ifndef FEAT_GUI_GTK
    gui.menu_height = MENU_DEFAULT_HEIGHT;
    gui.menu_width = 0;
# endif
#endif
#если defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    gui.toolbar_height = 0;
#endif
#ifdef FEAT_BEVAL_TIP
    gui.tooltip_fontset = NOFONTSET;
#endif

    gui.scrollbar_width = gui.scrollbar_height = SB_DEFAULT_WIDTH;
    gui.prev_wrap = -1;

#если defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // Note: gui_set_ligatures() might already have been called e.g. from .vimrc,
    // and in that case we don't want to overwrite ligatures map that has already
    // been correctly populated (as that would lead to a cleared ligatures maps).
    если (*p_guiligatures == NUL)
	CLEAR_FIELD(gui.ligatures_map);
#endif

#если defined(ALWAYS_USE_GUI) || defined(VIMDLL)
    result = OK;
#иначе
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
# иначе
    result = gui_mch_init_check();
# endif
#endif
    вернуть result;
}

/*
 * This is the call which starts the GUI.
 */
    void
gui_init(void)
{
    win_T	*wp;
    tabpage_T	*tp;
    static целое	recursive = 0;

    /*
     * It's possible to use ":gui" in a .gvimrc file.  The first half of this
     * function will then be executed at the first call, the rest by the
     * recursive call.  This allow the shell to be opened halfway reading a
     * gvimrc file.
     */
    если (!recursive)
    {
	++recursive;

	clip_init(TRUE);

	// If can't initialize, don't try doing the rest
	если (gui_init_check() == FAIL)
	{
	    --recursive;
	    clip_init(FALSE);
	    вернуть;
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
#если defined(SYS_MENU_FILE) && defined(FEAT_MENU)
	если (vim_strchr(p_go, GO_NOSYSMENU) == NULL)
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
	если (!option_was_set((char_u *)"mouse"))
	    set_string_option_direct((char_u *)"mouse", -1,
					   (char_u *)"a", OPT_FREE, SID_NONE);

	/*
	 * If -U option given, use only the initializations from that file and
	 * nothing else.  Skip all initializations for "-U NONE" or "-u NORC".
	 */
	если (use_gvimrc != NULL)
	{
	    если (STRCMP(use_gvimrc, "NONE") != 0
		    && STRCMP(use_gvimrc, "NORC") != 0
		    && do_source(use_gvimrc, FALSE, DOSO_NONE, NULL) != OK)
		semsg(_(e_cannot_read_from_str), use_gvimrc);
	}
	иначе
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
	    если (process_env((char_u *)"GVIMINIT", FALSE) == FAIL
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
	    если (p_exrc)
	    {
#ifdef UNIX
		{
		    stat_T s;

		    // if ".gvimrc" file is not owned by user, set 'secure'
		    // mode
		    если (mch_stat(GVIMRC_FILE, &s) || s.st_uid != getuid())
			secure = p_secure;
		}
#иначе
		secure = p_secure;
#endif

		если (       fullpathcmp((char_u *)USR_GVIMRC_FILE,
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

		если (secure == 2)
		    need_wait_return = TRUE;
		secure = 0;
	    }
	}

	если (need_wait_return || msg_didany)
	    wait_return(TRUE);

	--recursive;
    }

    // If recursive call opened the shell, return here from the first call
    если (gui.in_use)
	вернуть;

    /*
     * Create the GUI shell.
     */
    gui.in_use = true;		// Must be set after menus have been set up
#если defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
    gui.scale = 1.0; // Default value
#endif
    если (gui_mch_init() == FAIL)
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
    если (gui.border_width < 0)
	gui.border_width = 0;

    /*
     * Set up the fonts.  First use a font specified with "-fn" or "-font".
     */
    если (font_argument != NULL)
	set_option_value_give_err((char_u *)"gfn",
					       0L, (char_u *)font_argument, 0);
    если (
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
    если (gui_get_wide_font() == FAIL)
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
    если (gui_mch_maximized())
	gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    иначе
	gui_set_shellsize(TRUE, TRUE, RESIZE_BOTH);
#иначе
# ifndef FEAT_GUI_GTK
    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
# endif
#endif
#если defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    // Need to set the size of the menubar after all the menus have been
    // created.
    gui_mch_compute_menu_height((Widget)0);
#endif

    /*
     * Actually open the GUI shell.
     */
    если (gui_mch_open() != FAIL)
    {
	maketitle();
	resettitle();

	init_gui_options();
#ifdef FEAT_ARABIC
	// Our GUI can't do bidi.
	p_tbidi = FALSE;
#endif
#если defined(FEAT_GUI_GTK)
	// Give GTK+ a chance to put all widget's into place.
	gui_mch_update();

# ifdef FEAT_MENU
	// If there is no 'm' in 'guioptions' we need to remove the menu now.
	// It was still there to make F10 work.
	если (vim_strchr(p_go, GO_MENUS) == NULL)
	{
	    bool save_starting = gui.starting;

	    gui.starting = false;
	    gui_mch_enable_menu(FALSE);
	    gui.starting = save_starting;
	    gui_mch_update();
	}
# endif

	// Now make sure the shell fits on the screen.
	если (gui_mch_maximized())
	    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
	иначе
	    gui_set_shellsize(TRUE, TRUE, RESIZE_BOTH);
#endif
	// When 'lines' was set while starting up the topframe may have to be
	// resized.
	win_new_shellsize();

#ifdef FEAT_BEVAL_GUI
	// Always create the Balloon Evaluation area, but disable it when
	// 'ballooneval' is off.
	если (balloonEval != NULL)
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
# иначе
#  если defined(FEAT_GUI_MOTIF)
	{
	    extern Widget	textArea;
	    balloonEval = gui_mch_create_beval_area(textArea, NULL,
						     &general_beval_cb, NULL);
	}
#  иначе
#   ifdef FEAT_GUI_MSWIN
	balloonEval = gui_mch_create_beval_area(NULL, NULL,
						     &general_beval_cb, NULL);
#   endif
#  endif
# endif
	если (!p_beval)
	    gui_mch_disable_beval_area(balloonEval);
#endif

#ifndef FEAT_GUI_MSWIN
	// In the GUI modifiers are prepended to keys.
	// Don't do this for MS-Windows yet, it sends CTRL-K without the
	// modifier.
	seenModifyOtherKeys = TRUE;
#endif

#если defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	если (!im_xim_isvalid_imactivate())
	    emsg(_(e_value_of_imactivatekey_is_invalid));
#endif
	// When 'cmdheight' was set during startup it may not have taken
	// effect yet.
	если (p_ch != 1L)
	    command_height();

	вернуть;
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
gui_exit(целое rc)
{
    // don't free the fonts, it leads to a BUS error
    // richard@whitequeen.com Jul 99
    free_highlight_fonts();
    gui.in_use = false;
    gui_mch_exit(rc);
}

#если defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN) \
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

    если (before_quit_autocmds(curwin, TRUE, FALSE))
	вернуть;

    // Only exit when there are no changed files
    exiting = TRUE;
# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# если defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    cmdmod.cmod_flags |= CMOD_CONFIRM;
# endif
    // If there are changed buffers, present the user with a dialog if
    // possible, otherwise give an error message.
    если (!check_changed_any(FALSE, FALSE))
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
    целое
gui_init_font(char_u *font_list, целое fontset UNUSED)
{
#define FONTLEN 320
    char_u	font_name[FONTLEN];
    целое		font_list_empty = FALSE;
    целое		ret = FAIL;

    если (!gui.in_use)
	вернуть FAIL;

    font_name[0] = NUL;
    если (*font_list == NUL)
	font_list_empty = TRUE;
    иначе
    {
#ifdef FEAT_XFONTSET
	// When using a fontset, the whole list of fonts is one name.
	если (fontset)
	    ret = gui_mch_init_font(font_list, TRUE);
	иначе
#endif
	    пока (*font_list != NUL)
	    {
		// Isolate one comma separated font name.
		(void)copy_option_part(&font_list, font_name, FONTLEN, ",");

		// Careful!!!  The Win32 version of gui_mch_init_font(), when
		// called with "*" will change p_guifont to the selected font
		// name, which frees the old value.  This makes font_list
		// invalid.  Thus when OK is returned here, font_list must no
		// longer be used!
		если (gui_mch_init_font(font_name, FALSE) == OK)
		{
#ifdef USE_SET_GUIFONTWIDE
		    // If it's a Unicode font, try setting 'guifontwide' to a
		    // similar double-width font.
		    если ((p_guifontwide == NULL || *p_guifontwide == NUL)
				&& strstr((char *)font_name, "10646") != NULL)
			set_guifontwide(font_name);
#endif
		    ret = OK;
		    break;
		}
	    }
    }

    если (ret != OK
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

    если (ret == OK)
    {
#ifndef FEAT_GUI_GTK
	// Set normal font as current font
# ifdef FEAT_XFONTSET
	если (gui.fontset != NOFONTSET)
	    gui_mch_set_fontset(gui.fontset);
	иначе
# endif
	    gui_mch_set_font(gui.norm_font);
#endif
	gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    }

    вернуть ret;
}

#ifdef USE_SET_GUIFONTWIDE
/*
 * Try setting 'guifontwide' to a font twice as wide as "name".
 */
    static void
set_guifontwide(char_u *name)
{
    целое		i = 0;
    char_u	wide_name[FONTLEN + 10]; // room for 2 * width and '*'
    char_u	*wp = NULL;
    char_u	*p;
    GuiFont	font;

    wp = wide_name;
    для (p = name; *p != NUL; ++p)
    {
	*wp++ = *p;
	если (*p == '-')
	{
	    ++i;
	    если (i == 6)		// font type: change "--" to "-*-"
	    {
		если (p[1] == '-')
		    *wp++ = '*';
	    }
	    иначе если (i == 12)	// found the width
	    {
		++p;
		i = getdigits(&p);
		если (i != 0)
		{
		    // Double the width specification.
		    sprintf((char *)wp, "%d%s", i * 2, p);
		    font = gui_mch_get_font(wide_name, FALSE);
		    если (font != NOFONT)
		    {
			gui_mch_free_font(gui.wide_font);
			gui.wide_font = font;
			set_string_option_direct((char_u *)"gfw", -1,
						      wide_name, OPT_FREE, 0);
		    }
		}
		break;
	    }
	}
    }
}
#endif

/*
 * Get the font for 'guifontwide'.
 * Return FAIL for an invalid font name.
 */
    целое
gui_get_wide_font(void)
{
    GuiFont	font = NOFONT;
    char_u	font_name[FONTLEN];
    char_u	*p;

    если (!gui.in_use)	    // Can't allocate font yet, assume it's OK.
	вернуть OK;	    // Will give an error message later.

    если (p_guifontwide != NULL && *p_guifontwide != NUL)
    {
	для (p = p_guifontwide; *p != NUL; )
	{
	    // Isolate one comma separated font name.
	    (void)copy_option_part(&p, font_name, FONTLEN, ",");
	    font = gui_mch_get_font(font_name, FALSE);
	    если (font != NOFONT)
		break;
	}
	если (font == NOFONT)
	    вернуть FAIL;
    }

    gui_mch_free_font(gui.wide_font);
#ifdef FEAT_GUI_GTK
    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    если (font != NOFONT && gui.norm_font != NOFONT
			 && pango_font_description_equal(font, gui.norm_font))
    {
	gui.wide_font = NOFONT;
	gui_mch_free_font(font);
    }
    иначе
#endif
	gui.wide_font = font;
#ifdef FEAT_GUI_MSWIN
    gui_mch_wide_font_changed();
#иначе
    /*
     * TODO: setup wide_bold_font, wide_ital_font and wide_boldital_font to
     * support those fonts for 'guifontwide'.
     */
#endif
    вернуть OK;
}

#если defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * Set list of ascii characters that combined can create ligature.
 * Store them in char map for quick access from gui_gtk_draw_string.
 */
    void
gui_set_ligatures(void)
{
    char_u	*p;

    если (*p_guiligatures != NUL)
    {
	// check for invalid characters
	для (p = p_guiligatures; *p != NUL; ++p)
	    если (*p < 32 || *p > 127)
	    {
		emsg(_(e_ascii_code_not_in_range));
		вернуть;
	    }

	// store valid setting into ligatures_map
	CLEAR_FIELD(gui.ligatures_map);
	для (p = p_guiligatures; *p != NUL; ++p)
	    gui.ligatures_map[*p] = 1;
    }
    иначе
	CLEAR_FIELD(gui.ligatures_map);
}

/*
 * Adjust the columns to undraw for when the cursor is on ligatures.
 */
    static void
gui_adjust_undraw_cursor_for_ligatures(целое *startcol, целое *endcol)
{
    целое off;

    если (ScreenLines == NULL || *p_guiligatures == NUL)
	вернуть;

    // expand before the cursor for all the chars in gui.ligatures_map
    off = LineOffset[gui.cursor_row] + *startcol;
    если (gui.ligatures_map[ScreenLines[off]])
	пока (*startcol > 0 && gui.ligatures_map[ScreenLines[--off]])
	    (*startcol)--;

    // expand after the cursor for all the chars in gui.ligatures_map
    off = LineOffset[gui.cursor_row] + *endcol;
    если (gui.ligatures_map[ScreenLines[off]])
	пока (*endcol < ((целое)screen_Columns - 1)
				      && gui.ligatures_map[ScreenLines[++off]])
	   (*endcol)++;
}
#endif

    static void
gui_set_cursor(целое row, целое col)
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
    если (gui.row >= screen_Rows)
	gui.row = screen_Rows - 1;
    если (gui.col >= screen_Columns)
	gui.col = screen_Columns - 1;
    если (gui.cursor_row >= screen_Rows || gui.cursor_col >= screen_Columns)
	gui.cursor_is_valid = false;
}

/*
 * Redraw the cursor if necessary or when forced.
 * Careful: The contents of ScreenLines[] must match what is on the screen,
 * otherwise this goes wrong.  May need to call out_flush() first.
 */
    void
gui_update_cursor(
    целое		force,		 // when TRUE, update even when not moved
    целое		clear_selection) // clear selection under cursor
{
    целое		cur_width = 0;
    целое		cur_height = 0;
    целое		old_hl_mask;
    cursorentry_T *shape;
    целое		id;
#ifdef FEAT_TERMINAL
    guicolor_T	shape_fg = INVALCOLOR;
    guicolor_T	shape_bg = INVALCOLOR;
#endif
    guicolor_T	cfg, cbg, cc;	// cursor fore-/background color
    целое		cattr;		// cursor attributes
    целое		attr;
    attrentry_T *aep = NULL;
#если (defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)) && !defined(USE_GTK4)
    bool	lig_left = false, lig_right = false;
#endif

    // Don't update the cursor when halfway busy scrolling or the screen size
    // doesn't match 'columns' and 'lines.  ScreenLines[] isn't valid then.
    если (!can_update_cursor || screen_Columns != gui.num_cols
					       || screen_Rows != gui.num_rows)
	вернуть;

    gui_check_pos();

    если (gui.cursor_is_valid && !force
		&& gui.row == gui.cursor_row && gui.col == gui.cursor_col)
	вернуть;

    gui_undraw_cursor();

    // If a cursor-less sleep is ongoing, leave the cursor invisible
    если (cursor_is_sleeping())
	вернуть;

    если (gui.row < 0)
	вернуть;
#ifdef HAVE_INPUT_METHOD
    если (gui.row != gui.cursor_row || gui.col != gui.cursor_col)
	im_set_position(gui.row, gui.col);
#endif
    gui.cursor_row = gui.row;
    gui.cursor_col = gui.col;

    // Only write to the screen after ScreenLines[] has been initialized
    если (!screen_cleared || ScreenLines == NULL)
	вернуть;

    // Clear the selection if we are about to write over it
    если (clear_selection)
	clip_may_clear_selection(gui.row, gui.row);
    // Check that the cursor is inside the shell (resizing may have made
    // it invalid)
    если (gui.row >= screen_Rows || gui.col >= screen_Columns)
	вернуть;

    gui.cursor_is_valid = true;

    /*
     * How the cursor is drawn depends on the current mode.
     * When in a terminal window use the shape/color specified there.
     */
#ifdef FEAT_TERMINAL
    если (terminal_is_active())
	shape = term_get_cursor_shape(&shape_fg, &shape_bg);
    иначе
#endif
	shape = &shape_table[get_shape_idx(FALSE)];
    если (State & MODE_LANGMAP)
	id = shape->id_lm;
    иначе
	id = shape->id;

    // get the colors and attributes for the cursor.  Default is inverted
    cfg = INVALCOLOR;
    cbg = INVALCOLOR;
    cattr = HL_INVERSE;
    gui_mch_set_blinking(shape->blinkwait,
	    shape->blinkon,
	    shape->blinkoff);
    если (shape->blinkwait == 0 || shape->blinkon == 0
	    || shape->blinkoff == 0)
	gui_mch_stop_blink(FALSE);
#ifdef FEAT_TERMINAL
    если (shape_bg != INVALCOLOR)
    {
	cattr = 0;
	cfg = shape_fg;
	cbg = shape_bg;
    }
    иначе
#endif
	если (id > 0)
	{
	    cattr = syn_id2colors(id, &cfg, &cbg);
#если defined(HAVE_INPUT_METHOD)
	    {
		static целое iid;
		guicolor_T fg, bg;

		если (
# если defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
			preedit_get_status()
# иначе
			im_get_status()
# endif
		   )
		{
		    iid = syn_name2id((char_u *)"CursorIM");
		    если (iid > 0)
		    {
			syn_id2colors(iid, &fg, &bg);
			если (bg != INVALCOLOR)
			    cbg = bg;
			если (fg != INVALCOLOR)
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
    если (attr > HL_ALL)
	aep = syn_gui_attr2entry(attr);
    если (aep != NULL)
    {
	attr = aep->ae_attr;
	если (cfg == INVALCOLOR)
	    cfg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.bg_color
		    : aep->ae_u.gui.fg_color);
	если (cbg == INVALCOLOR)
	    cbg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.fg_color
		    : aep->ae_u.gui.bg_color);
    }
    если (cfg == INVALCOLOR)
	cfg = (attr & HL_INVERSE) ? gui.back_pixel : gui.norm_pixel;
    если (cbg == INVALCOLOR)
	cbg = (attr & HL_INVERSE) ? gui.norm_pixel : gui.back_pixel;

#ifdef FEAT_XIM
    если (aep != NULL)
    {
	xim_bg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.fg_color
		: aep->ae_u.gui.bg_color);
	xim_fg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.bg_color
		: aep->ae_u.gui.fg_color);
	если (xim_bg_color == INVALCOLOR)
	    xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
		: gui.back_pixel;
	если (xim_fg_color == INVALCOLOR)
	    xim_fg_color = (attr & HL_INVERSE) ? gui.back_pixel
		: gui.norm_pixel;
    }
    иначе
    {
	xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
	    : gui.back_pixel;
	xim_fg_color = (attr & HL_INVERSE) ? gui.back_pixel
	    : gui.norm_pixel;
    }
#endif

    attr &= ~HL_INVERSE;
    если (cattr & HL_INVERSE)
    {
	cc = cbg;
	cbg = cfg;
	cfg = cc;
    }
    cattr &= ~HL_INVERSE;

    /*
     * When we don't have window focus, draw a hollow cursor.
     */
    если (!gui.in_focus)
    {
#ifdef USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, -1, -1);
#иначе
	gui_mch_draw_hollow_cursor(cbg);
#endif
	вернуть;
    }

#ifdef USE_GTK4
    // Make sure that character underneath is drawn again in case it is part of
    // a ligature.
    gui_redraw_block(gui.row, gui.col, gui.row, gui.col, GUI_MON_NOCLEAR);
    // gui_redraw_block() will invalidate the cursor
    gui.cursor_is_valid = true;
#endif
#если defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // If cursor is in the middle of a ligature, then split the ligature at
    // the cursor boundaries by redrawing those cells again.
    для (целое c = gui.col - 1; c >= 0; c--)
	если (!gui.ligatures_map[ScreenLines[LineOffset[gui.row] + c]])
	{
	    если (c < gui.col - 1)
	    {
		gui_redraw_block(gui.row, c + 1, gui.row, gui.col - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_left = true;
# endif
	    }
	    break;
	}

    для (целое c = gui.col + 1; c < screen_Columns - 1; c++)
	если (!gui.ligatures_map[ScreenLines[LineOffset[gui.row] + c]])
	{
	    если (c > gui.col + 1)
	    {
		gui_redraw_block(gui.row, gui.col + 1, gui.row, c - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_right = true;
# endif
	    }
	    break;
	}

# ifndef USE_GTK4
    если ((lig_left || lig_right) && shape->shape != SHAPE_BLOCK)
    {
	// Because the cursor is not drawn with gui_screenchar(), must blit the
	// cell again (with its background), so that old ligature does not
	// remain on screen. Not needed for GtkSnapshot, because char beneath is
	// always rerendered (see above ifdef).
	целое old = gui.col;
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
    если (shape->shape == SHAPE_BLOCK)
    {
#ifdef USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, 0, 0);
#иначе
	/*
	 * Draw the text character with the cursor colors.	Use the
	 * character attributes plus the cursor attributes.
	 */
	gui.highlight_mask = (cattr | attr);
	(void)gui_screenchar(LineOffset[gui.row] + gui.col,
		GUI_MON_IS_CURSOR | GUI_MON_NOCLEAR, cfg, cbg, 0);
#endif
    }
    иначе
    {
#если defined(FEAT_RIGHTLEFT)
	целое	    col_off = FALSE;
#endif
	/*
	 * First draw the partial cursor, then overwrite with the text
	 * character, using a transparent background.
	 */
	если (shape->shape == SHAPE_VER)
	{
	    cur_height = gui.char_height;
	    cur_width = (gui.char_width * shape->percentage + 99) / 100;
	}
	иначе
	{
	    cur_height = (gui.char_height * shape->percentage + 99) / 100;
	    cur_width = gui.char_width;
	}
	если (has_mbyte && (*mb_off2cells)(LineOffset[gui.row] + gui.col,
		    LineOffset[gui.row] + screen_Columns) > 1)
	{
	    // Double wide character.
	    если (shape->shape != SHAPE_VER)
		cur_width += gui.char_width;
#ifdef FEAT_RIGHTLEFT
	    если (CURSOR_BAR_RIGHT)
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
#иначе
	gui_mch_draw_part_cursor(cur_width, cur_height, cbg);
#endif
#если defined(FEAT_RIGHTLEFT)
	если (col_off)
	    --gui.col;
#endif

	// Doesn't seem to work for MSWindows. We call gui_redraw_block() above
	// for GtkSnapshot.
#если !defined(FEAT_GUI_MSWIN) && !defined(USE_GTK4)
	gui.highlight_mask = ScreenAttrs[LineOffset[gui.row] + gui.col];
	(void)gui_screenchar(LineOffset[gui.row] + gui.col,
		GUI_MON_TRS_CURSOR | GUI_MON_NOCLEAR,
		(guicolor_T)0, (guicolor_T)0, 0);
#endif
    }
    gui.highlight_mask = old_hl_mask;
}

#если defined(FEAT_MENU)
    static void
gui_position_menu(void)
{
# если !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
    если (gui.menu_is_active && gui.in_use)
	gui_mch_set_menu_pos(0, 0, gui.menu_width, gui.menu_height);
# endif
}
#endif

/*
 * Position the various GUI components (text area, menu).  The vertical
 * scrollbars are NOT handled here.  See gui_update_scrollbars().
 */
    static void
gui_position_components(целое total_width UNUSED)
{
    целое	    text_area_x;
    целое	    text_area_y;
    целое	    text_area_width;
    целое	    text_area_height;

    // avoid that moving components around generates events
    ++hold_gui_events;

    text_area_x = 0;
    если (gui.which_scrollbars[SBAR_LEFT])
	text_area_x += gui.scrollbar_width;

    text_area_y = 0;
#если defined(FEAT_MENU) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    gui.menu_width = total_width;
    если (gui.menu_is_active)
	text_area_y += gui.menu_height;
#endif

#если defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
    если (gui_has_tabline())
	text_area_y += gui.tabline_height;
#endif

#если defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    если (vim_strchr(p_go, GO_TOOLBAR) != NULL)
    {
# если defined(FEAT_GUI_HAIKU)
	gui_mch_set_toolbar_pos(0, text_area_y,
				gui.menu_width, gui.toolbar_height);
# endif
	text_area_y += gui.toolbar_height;
    }
#endif

#если defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_HAIKU)
    gui_mch_set_tabline_pos(0, text_area_y,
    gui.menu_width, gui.tabline_height);
    если (gui_has_tabline())
	text_area_y += gui.tabline_height;
#endif

    text_area_width = gui.num_cols * gui.char_width + gui.border_offset * 2;
    text_area_height = gui.num_rows * gui.char_height + gui.border_offset * 2;

    gui_mch_set_text_area_pos(text_area_x,
			      text_area_y,
			      text_area_width,
			      text_area_height
#если defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
				  + xim_get_status_area_height()
#endif
			      );
#ifdef FEAT_MENU
    gui_position_menu();
#endif
    если (gui.which_scrollbars[SBAR_BOTTOM])
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
    целое
gui_get_base_width(void)
{
    целое	    base_width;

    base_width = 2 * gui.border_offset;
    если (gui.which_scrollbars[SBAR_LEFT])
	base_width += gui.scrollbar_width;
    если (gui.which_scrollbars[SBAR_RIGHT])
	base_width += gui.scrollbar_width;
    вернуть base_width;
}

/*
 * Get the height of the widgets and decorations above and below the text area.
 */
    целое
gui_get_base_height(void)
{
    целое	    base_height;

    base_height = 2 * gui.border_offset;
    если (gui.which_scrollbars[SBAR_BOTTOM])
	base_height += gui.scrollbar_height;
#ifdef FEAT_GUI_GTK
    // We can't take the sizes properly into account until anything is
    // realized.  Therefore we recalculate all the values here just before
    // setting the size. (--mdcki)
#иначе
# ifdef FEAT_MENU
    если (gui.menu_is_active)
	base_height += gui.menu_height;
# endif
# ifdef FEAT_TOOLBAR
    если (vim_strchr(p_go, GO_TOOLBAR) != NULL)
	base_height += gui.toolbar_height;
# endif
# если defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    если (gui_has_tabline())
	base_height += gui.tabline_height;
# endif
# если defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    base_height += gui_mch_text_area_extra_height();
# endif
#endif
    вернуть base_height;
}

/*
 * Should be called after the GUI shell has been resized.  Its arguments are
 * the new width and height of the shell in pixels.
 */
    void
gui_resize_shell(целое pixel_width, целое pixel_height)
{
    static целое	busy = FALSE;

    если (!gui.shell_created)	    // ignore when still initializing
	вернуть;

    /*
     * Can't resize the screen while it is being redrawn.  Remember the new
     * size and handle it later.
     */
    если (updating_screen || busy)
    {
	new_pixel_width = pixel_width;
	new_pixel_height = pixel_height;
	вернуть;
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
#если defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk_init_decor_height();
#endif

    gui.num_cols = (pixel_width - gui_get_base_width()) / gui.char_width;
    gui.num_rows = (pixel_height - gui_get_base_height()) / gui.char_height;

#ifdef USE_GTK4
    gui_gtk4_update_size();
#endif

    gui_position_components(pixel_width);
    gui_reset_scroll_region();

#если defined(FEAT_GUI_GTK) && defined(USE_GTK4) && !defined(USE_GTK4)
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
    если (State == MODE_ASKMORE || State == MODE_CONFIRM)
	gui.row = gui.num_rows;

    // Only comparing Rows and Columns may be sufficient, but let's stay on
    // the safe side.
    если (gui.num_rows != screen_Rows || gui.num_cols != screen_Columns
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
#если defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(pixel_width, pixel_height);
#endif
#если defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
    xim_set_status_area();
#endif

    busy = FALSE;

    // We may have been called again while redrawing the screen.
    // Need to do it all again with the latest size then.  But only if the size
    // actually changed.
    если (new_pixel_height)
    {
	если (pixel_width == new_pixel_width && pixel_height == new_pixel_height)
	{
	    new_pixel_width = 0;
	    new_pixel_height = 0;
	}
	иначе
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
    если (new_pixel_height)
    {
	// careful: gui_resize_shell() may postpone the resize again if we
	// were called indirectly by it
	gui_resize_shell(new_pixel_width, new_pixel_height);
    }
}

    целое
gui_get_shellsize(void)
{
    Rows = gui.num_rows;
    Columns = gui.num_cols;
    вернуть OK;
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
    целое		mustset UNUSED,
    целое		fit_to_display,
    целое		direction)		// RESIZE_HOR, RESIZE_VER
{
    целое		base_width;
    целое		base_height;
    целое		width;
    целое		height;
    целое		min_width;
    целое		min_height;
    целое		screen_w;
    целое		screen_h;
#ifdef FEAT_GUI_GTK
    целое		un_maximize = mustset;
    целое		did_adjust = 0;
#endif
    целое		x = -1, y = -1;

    если (!gui.shell_created)
	вернуть;

#если defined(MSWIN) || defined(FEAT_GUI_GTK)
    // If not setting to a user specified size and maximized, calculate the
    // number of characters that fit in the maximized window.
    // FIXME: gui_mch_newfont() is called here even when the font hasn't
    // changed at all.  For example, ":set guioptions=k" triggers this path
    // via gui_init_which_components() -> gui_set_shellsize(FALSE, ...).
    // The intent is to keep the window size and recalculate Rows/Columns,
    // which has nothing to do with fonts.  This should be a separate
    // function with a more descriptive name.
    если (!mustset && (vim_strchr(p_go, GO_KEEPWINSIZE) != NULL
						       || gui_mch_maximized()))
    {
	gui_mch_newfont();
	вернуть;
    }
#endif

    base_width = gui_get_base_width();
    base_height = gui_get_base_height();
    если (fit_to_display)
	// Remember the original window position.
	(void)gui_mch_get_winpos(&x, &y);

    width = Columns * gui.char_width + base_width;
    height = Rows * gui.char_height + base_height;

    если (fit_to_display)
    {
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	если ((direction & RESIZE_HOR) && width > screen_w)
	{
	    Columns = (screen_w - base_width) / gui.char_width;
	    если (Columns < MIN_COLUMNS)
		Columns = MIN_COLUMNS;
	    width = Columns * gui.char_width + base_width;
#ifdef FEAT_GUI_GTK
	    ++did_adjust;
#endif
	}
	если ((direction & RESIZE_VERT) && height > screen_h)
	{
	    Rows = (screen_h - base_height) / gui.char_height;
	    check_shellsize();
	    height = Rows * gui.char_height + base_height;
#ifdef FEAT_GUI_GTK
	    ++did_adjust;
#endif
	}
#ifdef FEAT_GUI_GTK
	если (did_adjust == 2 || (width + gui.char_width >= screen_w
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
    если (un_maximize)
    {
	// If the window size is smaller than the screen unmaximize the
	// window, otherwise resizing won't work.
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	если ((width + gui.char_width < screen_w
				   || height + gui.char_height * 2 < screen_h)
		&& gui_mch_maximized())
	    gui_mch_unmaximize();
    }
#endif

    gui_mch_set_shellsize(width, height, min_width, min_height,
					  base_width, base_height, direction);

    если (fit_to_display && x >= 0 && y >= 0)
    {
	// Some window managers put the Vim window left of/above the screen.
	// Only change the position if it wasn't already negative before
	// (happens on MS-Windows with a secondary monitor).
	gui_mch_update();
	если (gui_mch_get_winpos(&x, &y) == OK && (x < 0 || y < 0))
	    gui_mch_set_winpos(x < 0 ? 0 : x, y < 0 ? 0 : y);
    }

    gui_position_components(width);
    gui_update_scrollbars(TRUE);
    gui_reset_scroll_region();

#если defined(FEAT_GUI_GTK) && defined(USE_GTK4)
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
gui_start_highlight(целое mask)
{
    если (mask > HL_ALL)		    // highlight code
	gui.highlight_mask = mask;
    иначе			    // mask
	gui.highlight_mask |= mask;
}

    void
gui_stop_highlight(целое mask)
{
    если (mask > HL_ALL)		    // highlight code
	gui.highlight_mask = HL_NORMAL;
    иначе			    // mask
	gui.highlight_mask &= ~mask;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    void
gui_clear_block(
    целое	    row1,
    целое	    col1,
    целое	    row2,
    целое	    col2)
{
    // Clear the selection if we are about to write over it
    clip_may_clear_selection(row1, row2);

    gui_mch_clear_block(row1, col1, row2, col2);

    // Invalidate cursor if it was in this block
    если (       gui.cursor_row >= row1 && gui.cursor_row <= row2
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
    целое		len)
{
    char_u	*p;
    целое		arg1 = 0, arg2 = 0;
    целое		force_cursor = FALSE;	// force cursor update
    целое		force_scrollbar = FALSE;
    static win_T	*old_curwin = NULL;

// #define DEBUG_GUI_WRITE
#ifdef DEBUG_GUI_WRITE
    {
	целое i;
	char_u *str;

	printf("gui_write(%d):\n    ", len);
	для (i = 0; i < len; i++)
	    если (s[i] == ESC)
	    {
		если (i != 0)
		    printf("\n    ");
		printf("<ESC>");
	    }
	    иначе
	    {
		str = transchar_byte(s[i]);
		если (str[0] && str[1])
		    printf("<%s>", (char *)str);
		иначе
		    printf("%s", (char *)str);
	    }
	printf("\n");
    }
#endif
    пока (len)
    {
	если (s[0] == ESC && s[1] == '|')
	{
	    p = s + 2;
	    если (VIM_ISDIGIT(*p) || (*p == '-' && VIM_ISDIGIT(*(p + 1))))
	    {
		arg1 = getdigits(&p);
		если (p > s + len)
		    break;
		если (*p == ';')
		{
		    ++p;
		    arg2 = getdigits(&p);
		    если (p > s + len)
			break;
		}
	    }
	    switch (*p)
	    {
		case 'C':	// Clear screen
		    clip_scroll_selection(9999);
		    gui_mch_clear_all();
		    gui.cursor_is_valid = false;
		    force_scrollbar = TRUE;
		    break;
		case 'M':	// Move cursor
		    gui_set_cursor(arg1, arg2);
		    break;
		case 's':	// force cursor (shape) update
		    force_cursor = TRUE;
		    break;
		case 'R':	// Set scroll region
		    если (arg1 < arg2)
		    {
			gui.scroll_region_top = arg1;
			gui.scroll_region_bot = arg2;
		    }
		    иначе
		    {
			gui.scroll_region_top = arg2;
			gui.scroll_region_bot = arg1;
		    }
		    break;
		case 'V':	// Set vertical scroll region
		    если (arg1 < arg2)
		    {
			gui.scroll_region_left = arg1;
			gui.scroll_region_right = arg2;
		    }
		    иначе
		    {
			gui.scroll_region_left = arg2;
			gui.scroll_region_right = arg1;
		    }
		    break;
		case 'd':	// Delete line
		    gui_delete_lines(gui.row, 1);
		    break;
		case 'D':	// Delete lines
		    gui_delete_lines(gui.row, arg1);
		    break;
		case 'i':	// Insert line
		    gui_insert_lines(gui.row, 1);
		    break;
		case 'I':	// Insert lines
		    gui_insert_lines(gui.row, arg1);
		    break;
		case '$':	// Clear to end-of-line
		    gui_clear_block(gui.row, gui.col, gui.row,
							    (целое)Columns - 1);
		    break;
		case 'h':	// Turn on highlighting
		    gui_start_highlight(arg1);
		    break;
		case 'H':	// Turn off highlighting
		    gui_stop_highlight(arg1);
		    break;
		case 'f':	// flash the window (visual bell)
		    gui_mch_flash(arg1 == 0 ? 20 : arg1);
		    break;
		default:
		    p = s + 1;	// Skip the ESC
		    break;
	    }
	    len -= (целое)(++p - s);
	    s = p;
	}
	иначе если (s[0] < 0x20		// Ctrl character
#ifdef FEAT_SIGN_ICONS
		&& s[0] != SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
		&& s[0] != MULTISIGN_BYTE
# endif
#endif
		)
	{
	    если (s[0] == '\n')		// NL
	    {
		gui.col = 0;
		если (gui.row < gui.scroll_region_bot)
		    gui.row++;
		иначе
		    gui_delete_lines(gui.scroll_region_top, 1);
	    }
	    иначе если (s[0] == '\r')	// CR
	    {
		gui.col = 0;
	    }
	    иначе если (s[0] == '\b')	// Backspace
	    {
		если (gui.col)
		    --gui.col;
	    }
	    иначе если (s[0] == Ctrl_L)	// cursor-right
	    {
		++gui.col;
	    }
	    иначе если (s[0] == Ctrl_G)	// Beep
	    {
		gui_mch_beep();
	    }
	    // Other Ctrl character: shouldn't happen!

	    --len;	// Skip this char
	    ++s;
	}
	иначе
	{
	    p = s;
	    пока (len > 0 && (
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
	    gui_outstr(s, (целое)(p - s));
	    s = p;
	}
    }

    // Postponed update of the cursor (won't work if "can_update_cursor" isn't
    // set).
    если (force_cursor)
	gui_update_cursor(TRUE, TRUE);

    // When switching to another window the dragging must have stopped.
    // Required for GTK, dragged_sb isn't reset.
    если (old_curwin != curwin)
	gui.dragged_sb = SBAR_NONE;

    // Update the scrollbars after clearing the screen or when switched
    // to another window.
    // Update the horizontal scrollbar always, it's difficult to check all
    // situations where it might change.
    если (force_scrollbar || old_curwin != curwin)
	gui_update_scrollbars(force_scrollbar);
    иначе
	gui_update_horiz_scrollbar(FALSE);
    old_curwin = curwin;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#если defined(FEAT_GUI_GTK)
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
gui_dont_update_cursor(целое undraw)
{
    если (!gui.in_use)
	вернуть;

    // Undraw the cursor now, we probably can't do it after the change.
    если (undraw)
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
    если (disable_flush == 0)
	gui_mch_flush();
}

    static void
gui_outstr(char_u *s, целое len)
{
    целое	    this_len;
    целое	    cells;

    если (len == 0)
	вернуть;

    если (len < 0)
	len = (целое)STRLEN(s);

    пока (len > 0)
    {
	если (has_mbyte)
	{
	    // Find out how many chars fit in the current line.
	    cells = 0;
	    для (this_len = 0; this_len < len; )
	    {
		cells += (*mb_ptr2cells)(s + this_len);
		если (gui.col + cells > Columns)
		    break;
		this_len += (*mb_ptr2len)(s + this_len);
	    }
	    если (this_len > len)
		this_len = len;	    // don't include following composing char
	}
	иначе если (gui.col + len > Columns)
	    this_len = Columns - gui.col;
	иначе
	    this_len = len;

	(void)gui_outstr_nowrap(s, this_len,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	s += this_len;
	len -= this_len;
	// fill up for a double-width char that doesn't fit.
	если (len > 0 && gui.col < Columns)
	    (void)gui_outstr_nowrap((char_u *)" ", 1,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	// The cursor may wrap to the next line.
	если (gui.col >= Columns)
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
    static целое
gui_screenchar(
    целое		off,	    // Offset from start of screen
    целое		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    целое		back)	    // backup this many chars when using bold trick
{
    char_u	buf[MB_MAXBYTES + 1];

    // Don't draw right half of a double-width UTF-8 char. "cannot happen"
    если (enc_utf8 && ScreenLines[off] == 0)
	вернуть OK;

    если (enc_utf8 && ScreenLinesUC[off] != 0)
	// Draw UTF-8 multi-byte character.
	вернуть gui_outstr_nowrap(buf, utfc_char2bytes(off, buf),
							 flags, fg, bg, back);

    если (enc_dbcs == DBCS_JPNU && ScreenLines[off] == 0x8e)
    {
	buf[0] = ScreenLines[off];
	buf[1] = ScreenLines2[off];
	вернуть gui_outstr_nowrap(buf, 2, flags, fg, bg, back);
    }

    // Draw non-multi-byte character or DBCS character.
    вернуть gui_outstr_nowrap(ScreenLines + off,
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
    static целое
gui_screenstr(
    целое		off,	    // Offset from start of screen
    целое		len,	    // string length in screen cells
    целое		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    целое		back)	    // backup this many chars when using bold trick
{
    char_u  *buf;
    целое	    outlen = 0;
    целое	    i;
    целое	    retval;

    если (len <= 0) // "cannot happen"?
	вернуть OK;

    если (enc_utf8)
    {
	buf = alloc(len * MB_MAXBYTES + 1);
	если (buf == NULL)
	    вернуть OK; // not much we could do here...

	для (i = off; i < off + len; ++i)
	{
	    если (ScreenLines[i] == 0)
		continue; // skip second half of double-width char

	    если (ScreenLinesUC[i] == 0)
		buf[outlen++] = ScreenLines[i];
	    иначе
		outlen += utfc_char2bytes(i, buf + outlen);
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, back);
	vim_free(buf);

	вернуть retval;
    }
    иначе если (enc_dbcs == DBCS_JPNU)
    {
	buf = alloc(len * 2 + 1);
	если (buf == NULL)
	    вернуть OK; // not much we could do here...

	для (i = off; i < off + len; ++i)
	{
	    buf[outlen++] = ScreenLines[i];

	    // handle double-byte single-width char
	    если (ScreenLines[i] == 0x8e)
		buf[outlen++] = ScreenLines2[i];
	    иначе если (MB_BYTE2LEN(ScreenLines[i]) == 2)
		buf[outlen++] = ScreenLines[++i];
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, back);
	vim_free(buf);

	вернуть retval;
    }
    иначе
    {
	вернуть gui_outstr_nowrap(&ScreenLines[off], len,
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
    static целое
gui_outstr_nowrap(
    char_u	*s,
    целое		len,
    целое		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    целое		back)	    // backup this many chars when using bold trick
{
    long_u	highlight_mask;
    long_u	hl_mask_todo;
    guicolor_T	fg_color;
    guicolor_T	bg_color;
    guicolor_T	sp_color;
#если !defined(FEAT_GUI_GTK)
    GuiFont	font = NOFONT;
    GuiFont	wide_font = NOFONT;
# ifdef FEAT_XFONTSET
    GuiFontset	fontset = NOFONTSET;
# endif
#endif
    attrentry_T	*aep = NULL;
    целое		draw_flags;
    целое		col = gui.col;
#ifdef FEAT_SIGN_ICONS
    целое		draw_sign = FALSE;
    целое		signcol = 0;
    char_u	extra[18];
# ifdef FEAT_NETBEANS_INTG
    целое		multi_sign = FALSE;
# endif
#endif

    если (len < 0)
	len = (целое)STRLEN(s);
    если (len == 0)
	вернуть OK;

#ifdef FEAT_SIGN_ICONS
    если (*s == SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
	  || *s == MULTISIGN_BYTE
# endif
       )
    {
	size_t	slen = 2;		    // 2 spaces by default

# ifdef FEAT_NETBEANS_INTG
	если (*s == MULTISIGN_BYTE)
	    multi_sign = TRUE;
# endif
	// draw spaces instead
	если (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u' &&
		(curwin->w_p_nu || curwin->w_p_rnu))
	{
	    целое	n = number_width(curwin);

	    slen = MIN(n, (целое)sizeof(extra) - 1);
	}
	vim_memset(extra, ' ', slen);
	extra[slen] = NUL;
	s = extra;
	если (len == 1 && col > 0)
	    --col;
	len = (целое)slen;
	если (len > 2)
	    // right align sign icon in the number column
	    signcol = col + len - 3;
	иначе
	    signcol = col;
	draw_sign = TRUE;
	highlight_mask = 0;
    }
    иначе
#endif
    если (gui.highlight_mask > HL_ALL)
    {
	aep = syn_gui_attr2entry(gui.highlight_mask);
	если (aep == NULL)	    // highlighting not set
	    highlight_mask = 0;
	иначе
	    highlight_mask = aep->ae_attr;
    }
    иначе
	highlight_mask = gui.highlight_mask;
    hl_mask_todo = highlight_mask;

#если !defined(FEAT_GUI_GTK)
    // Set the font
    если (aep != NULL && aep->ae_u.gui.font != NOFONT)
	font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
    иначе если (aep != NULL && aep->ae_u.gui.fontset != NOFONTSET)
	fontset = aep->ae_u.gui.fontset;
# endif
    иначе
    {
# ifdef FEAT_XFONTSET
	если (gui.fontset != NOFONTSET)
	    fontset = gui.fontset;
	иначе
# endif
	    если (hl_mask_todo & (HL_BOLD | HL_STANDOUT))
	{
	    если ((hl_mask_todo & HL_ITALIC) && gui.boldital_font != NOFONT)
	    {
		font = gui.boldital_font;
		hl_mask_todo &= ~(HL_BOLD | HL_STANDOUT | HL_ITALIC);
	    }
	    иначе если (gui.bold_font != NOFONT)
	    {
		font = gui.bold_font;
		hl_mask_todo &= ~(HL_BOLD | HL_STANDOUT);
	    }
	    иначе
		font = gui.norm_font;
	}
	иначе если ((hl_mask_todo & HL_ITALIC) && gui.ital_font != NOFONT)
	{
	    font = gui.ital_font;
	    hl_mask_todo &= ~HL_ITALIC;
	}
	иначе
	    font = gui.norm_font;

	/*
	 * Choose correct wide_font by font.  wide_font should be set with font
	 * at same time in above block.  But it will make many "ifdef" nasty
	 * blocks.  So we do it here.
	 */
	если (font == gui.boldital_font && gui.wide_boldital_font)
	    wide_font = gui.wide_boldital_font;
	иначе если (font == gui.bold_font && gui.wide_bold_font)
	    wide_font = gui.wide_bold_font;
	иначе если (font == gui.ital_font && gui.wide_ital_font)
	    wide_font = gui.wide_ital_font;
	иначе если (font == gui.norm_font && gui.wide_font)
	    wide_font = gui.wide_font;
    }
# ifdef FEAT_XFONTSET
    если (fontset != NOFONTSET)
	gui_mch_set_fontset(fontset);
    иначе
# endif
	gui_mch_set_font(font);
#endif

    draw_flags = 0;

    // Set the color
    bg_color = gui.back_pixel;
    если ((flags & GUI_MON_IS_CURSOR) && gui.in_focus)
    {
	draw_flags |= DRAW_CURSOR;
	fg_color = fg;
	bg_color = bg;
	sp_color = fg;
    }
    иначе если (aep != NULL)
    {
	fg_color = aep->ae_u.gui.fg_color;
	если (fg_color == INVALCOLOR)
	    fg_color = gui.norm_pixel;
	bg_color = aep->ae_u.gui.bg_color;
	если (bg_color == INVALCOLOR)
	    bg_color = gui.back_pixel;
	sp_color = aep->ae_u.gui.sp_color;
	если (sp_color == INVALCOLOR)
	    sp_color = fg_color;
    }
    иначе
    {
	fg_color = gui.norm_pixel;
	sp_color = fg_color;
    }

    если (highlight_mask & (HL_INVERSE | HL_STANDOUT))
    {
	gui_mch_set_fg_color(bg_color);
	gui_mch_set_bg_color(fg_color);
    }
    иначе
    {
	gui_mch_set_fg_color(fg_color);
	gui_mch_set_bg_color(bg_color);
    }
    gui_mch_set_sp_color(sp_color);

    // Clear the selection if we are about to write over it
    если (!(flags & GUI_MON_NOCLEAR))
	clip_may_clear_selection(gui.row, gui.row);


    // If there's no bold font, then fake it
    если (hl_mask_todo & (HL_BOLD | HL_STANDOUT))
	draw_flags |= DRAW_BOLD;

    /*
     * When drawing bold or italic characters the spill-over from the left
     * neighbor may be destroyed.  Let the caller backup to start redrawing
     * just after a blank.
     */
    если (back != 0 && ((draw_flags & DRAW_BOLD) || (highlight_mask & HL_ITALIC)))
	вернуть FAIL;

#если defined(FEAT_GUI_GTK)
    // If there's no italic font, then fake it.
    // For GTK2, we don't need a different font for italic style.
    если (hl_mask_todo & HL_ITALIC)
	draw_flags |= DRAW_ITALIC;

    // Do we underline the text?
    если (hl_mask_todo & HL_UNDERLINE)
	draw_flags |= DRAW_UNDERL;

#иначе
    // Do we underline the text?
    если ((hl_mask_todo & HL_UNDERLINE) || (hl_mask_todo & HL_ITALIC))
	draw_flags |= DRAW_UNDERL;
#endif
    // Do we undercurl the text?
    если (hl_mask_todo & HL_UNDERCURL)
	draw_flags |= DRAW_UNDERC;

    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

    // Do we strikethrough the text?
    если (hl_mask_todo & HL_STRIKETHROUGH)
	draw_flags |= DRAW_STRIKE;

    // Do we draw transparently?
    если (flags & GUI_MON_TRS_CURSOR)
	draw_flags |= DRAW_TRANSP;

    /*
     * Draw the text.
     */
#ifdef FEAT_GUI_GTK
    // The value returned is the length in display cells
    len = gui_gtk_draw_string(gui.row, col, s, len, draw_flags);
#иначе
    если (enc_utf8)
    {
	целое	start;		// index of bytes to be drawn
	целое	cells;		// cellwidth of bytes to be drawn
	целое	thislen;	// length of bytes to be drawn
	целое	cn;		// cellwidth of current char
	целое	i;		// index of current char
	целое	c;		// current char value
	целое	cl;		// byte length of current char
	целое	comping;	// current char is composing
	целое	scol = col;	// screen column
	целое	curr_wide = FALSE;  // use 'guifontwide'
	целое	prev_wide = FALSE;
	целое	wide_changed;
# ifdef MSWIN
	целое	sep_comp = FALSE;   // Don't separate composing chars.
# иначе
	целое	sep_comp = TRUE;    // Separate composing chars.
# endif

	// Break the string at a composing character, it has to be drawn on
	// top of the previous character.
	start = 0;
	cells = 0;
	для (i = 0; i < len; i += cl)
	{
	    c = utf_ptr2char(s + i);
	    cn = utf_char2cells(c);
	    comping = utf_iscomposing(c);
	    если (!comping)	// count cells from non-composing chars
		cells += cn;
	    если (!comping || sep_comp)
	    {
		если (cn > 1
# ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
# endif
			&& wide_font != NOFONT)
		    curr_wide = TRUE;
		иначе
		    curr_wide = FALSE;
	    }
	    cl = utf_ptr2len(s + i);
	    если (cl == 0)	// hit end of string
		len = i + cl;	// len must be wrong "cannot happen"

	    wide_changed = curr_wide != prev_wide;

	    // Print the string so far if it's the last character or there is
	    // a composing character.
	    если (i + cl >= len || (comping && sep_comp && i > start)
		    || wide_changed
# если defined(FEAT_GUI_X11)
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
		если ((comping && sep_comp) || wide_changed)
		    thislen = i - start;
		иначе
		    thislen = i - start + cl;
		если (thislen > 0)
		{
		    если (prev_wide)
			gui_mch_set_font(wide_font);
		    gui_mch_draw_string(gui.row, scol, s + start, thislen,
								  draw_flags);
		    если (prev_wide)
			gui_mch_set_font(font);
		    start += thislen;
		}
		scol += cells;
		cells = 0;
		// Adjust to not draw a character which width is changed
		// against with last one.
		если (wide_changed && !(comping && sep_comp))
		{
		    scol -= cn;
		    cl = 0;
		}

# если defined(FEAT_GUI_X11)
		// No fontset: draw a space to fill the gap after a wide char
		//
		если (cn > 1 && (draw_flags & DRAW_TRANSP) == 0
#  ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
#  endif
			&& !wide_changed)
		    gui_mch_draw_string(gui.row, scol - 1, (char_u *)" ",
							       1, draw_flags);
# endif
	    }
	    // Draw a composing char on top of the previous char.
	    если (comping && sep_comp)
	    {
# если defined(__APPLE_CC__) && TARGET_API_MAC_CARBON
		// Carbon ATSUI autodraws composing char over previous char
		gui_mch_draw_string(gui.row, scol, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# иначе
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
    иначе
    {
	gui_mch_draw_string(gui.row, col, s, len, draw_flags);
	если (enc_dbcs == DBCS_JPNU)
	{
	    // Get the length in display cells, this can be different from the
	    // number of bytes for "euc-jp".
	    len = mb_string2cells(s, len);
	}
    }
#endif // !FEAT_GUI_GTK

    если (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
	gui.col = col + len;

    // May need to invert it when it's part of the selection.
    если (flags & GUI_MON_NOCLEAR)
	clip_may_redraw_selection(gui.row, col, len);

    если (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
    {
	// Invalidate the old physical cursor position if we wrote over it
	если (gui.cursor_row == gui.row
		&& gui.cursor_col >= col
		&& gui.cursor_col < col + len)
	    gui.cursor_is_valid = false;
    }

#ifdef FEAT_SIGN_ICONS
    если (draw_sign)
	// Draw the sign on top of the spaces.
	gui_mch_drawsign(gui.row, signcol, gui.highlight_mask);
# если defined(FEAT_NETBEANS_INTG) && (defined(FEAT_GUI_X11) \
	|| defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN))
    если (multi_sign)
	netbeans_draw_multisign_indicator(gui.row);
# endif
#endif

    вернуть OK;
}

/*
 * Undraw the cursor.  This actually redraws the character at the cursor
 * position, plus some more characters when needed.
 */
    void
gui_undraw_cursor(void)
{
    если (!gui.cursor_is_valid)
	вернуть;

    // Always redraw the character just before if there is one, because
    // with some fonts and characters there can be a one pixel overlap.
    целое startcol = gui.cursor_col > 0 ? gui.cursor_col - 1 : gui.cursor_col;
    целое endcol = gui.cursor_col;

#если defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
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
    целое		x,
    целое		y,
    целое		w,
    целое		h)
{
    целое		row1, col1, row2, col2;

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
    если (gui.row == gui.cursor_row)
	gui_update_cursor(TRUE, TRUE);
}

/*
 * Draw a rectangular block of characters, from row1 to row2 (inclusive) and
 * from col1 to col2 (inclusive).
 */
    void
gui_redraw_block(
    целое		row1,
    целое		col1,
    целое		row2,
    целое		col2,
    целое		flags)	// flags for gui_outstr_nowrap()
{
    целое		old_row, old_col;
    long_u	old_hl_mask;
    целое		off;
    sattr_T	first_attr;
    целое		idx, len;
    целое		back, nback;
    целое		orig_col1, orig_col2;

    // Don't try to update when ScreenLines is not valid
    если (!screen_cleared || ScreenLines == NULL)
	вернуть;

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

    для (gui.row = row1; gui.row <= row2; gui.row++)
    {
	// When only half of a double-wide character is in the block, include
	// the other half.
	col1 = orig_col1;
	col2 = orig_col2;
	off = LineOffset[gui.row];
	если (enc_dbcs != 0)
	{
	    если (col1 > 0)
		col1 -= dbcs_screen_head_off(ScreenLines + off,
						    ScreenLines + off + col1);
	    col2 += dbcs_screen_tail_off(ScreenLines + off,
						    ScreenLines + off + col2);
	}
	иначе если (enc_utf8)
	{
	    если (ScreenLines[off + col1] == 0)
	    {
		если (col1 > 0)
		    --col1;
		иначе
		    // FIXME: how can the first character ever be zero?
		    siemsg("NUL in ScreenLines in row %ld", (long)gui.row);
	    }
#ifdef FEAT_GUI_GTK
	    если (col2 + 1 < Columns && ScreenLines[off + col2 + 1] == 0)
		++col2;
#endif
	}
	gui.col = col1;
	off = LineOffset[gui.row] + gui.col;
	len = col2 - col1 + 1;

	// Find how many chars back this highlighting starts, or where a space
	// is.  Needed for when the bold trick is used
	для (back = 0; back < col1; ++back)
	    если (ScreenAttrs[off - 1 - back] != ScreenAttrs[off]
		    || ScreenLines[off - 1 - back] == ' ')
		break;

	// Break it up in strings of characters with the same attributes.
	// Print UTF-8 characters individually.
	пока (len > 0)
	{
	    first_attr = ScreenAttrs[off];
	    gui.highlight_mask = first_attr;
#если !defined(FEAT_GUI_GTK)
	    если (enc_utf8 && ScreenLinesUC[off] != 0)
	    {
		// output multi-byte character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
		если (gui.col < Columns && ScreenLines[off + 1] == 0)
		    idx = 2;
		иначе
		    idx = 1;
	    }
	    иначе если (enc_dbcs == DBCS_JPNU && ScreenLines[off] == 0x8e)
	    {
		// output double-byte, single-width character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
		idx = 1;
	    }
	    иначе
#endif
	    {
#ifdef FEAT_GUI_GTK
		для (idx = 0; idx < len; ++idx)
		{
		    если (enc_utf8 && ScreenLines[off + idx] == 0)
			continue; // skip second half of double-width char
		    если (ScreenAttrs[off + idx] != first_attr)
			break;
		}
		// gui_screenstr() takes care of multibyte chars
		nback = gui_screenstr(off, idx, flags,
				      (guicolor_T)0, (guicolor_T)0, back);
#иначе
		для (idx = 0; idx < len && ScreenAttrs[off + idx] == first_attr;
									idx++)
		{
		    // Stop at a multi-byte Unicode character.
		    если (enc_utf8 && ScreenLinesUC[off + idx] != 0)
			break;
		    если (enc_dbcs == DBCS_JPNU)
		    {
			// Stop at a double-byte single-width char.
			если (ScreenLines[off + idx] == 0x8e)
			    break;
			если (len > 1 && (*mb_ptr2len)(ScreenLines
							    + off + idx) == 2)
			    ++idx;  // skip second byte of double-byte char
		    }
		}
		nback = gui_outstr_nowrap(ScreenLines + off, idx, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
#endif
	    }
	    если (nback == FAIL)
	    {
		// Must back up to start drawing where a bold or italic word
		// starts.
		off -= back;
		len += back;
		gui.col -= back;
	    }
	    иначе
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
    gui.highlight_mask = (целое)old_hl_mask;
}

    static void
gui_delete_lines(целое row, целое count)
{
    если (count <= 0)
	вернуть;

    если (row + count > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_block(row, gui.scroll_region_left,
			      gui.scroll_region_bot, gui.scroll_region_right);
    иначе
    {
	gui_mch_delete_lines(row, count);

	// If the cursor was in the deleted lines it's now gone.  If the
	// cursor was in the scrolled lines adjust its position.
	если (gui.cursor_row >= row
		&& gui.cursor_col >= gui.scroll_region_left
		&& gui.cursor_col <= gui.scroll_region_right)
	{
	    если (gui.cursor_row < row + count)
		gui.cursor_is_valid = false;
	    иначе если (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_row -= count;
	}
    }
}

    static void
gui_insert_lines(целое row, целое count)
{
    если (count <= 0)
	вернуть;

    если (row + count > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_block(row, gui.scroll_region_left,
			      gui.scroll_region_bot, gui.scroll_region_right);
    иначе
    {
	gui_mch_insert_lines(row, count);

	если (gui.cursor_row >= gui.row
		&& gui.cursor_col >= gui.scroll_region_left
		&& gui.cursor_col <= gui.scroll_region_right)
	{
	    если (gui.cursor_row <= gui.scroll_region_bot - count)
		gui.cursor_row += count;
	    иначе если (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_is_valid = false;
	}
    }
}

#ifdef FEAT_TIMERS
/*
 * Passed to ui_wait_for_chars_or_timer(), ignoring extra arguments.
 */
    static целое
gui_wait_for_chars_3(
    long wtime,
    целое *interrupted UNUSED,
    целое ignore_input UNUSED)
{
    вернуть gui_mch_wait_for_chars(wtime);
}
#endif

/*
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    static целое
gui_wait_for_chars_or_timer(
	long wtime,
	целое *interrupted UNUSED,
	целое ignore_input UNUSED)
{
#ifdef FEAT_TIMERS
    вернуть ui_wait_for_chars_or_timer(wtime, gui_wait_for_chars_3,
						    interrupted, ignore_input);
#иначе
    вернуть gui_mch_wait_for_chars(wtime);
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
    static целое
gui_wait_for_chars_buf(
    char_u	*buf,
    целое		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    целое		tb_change_cnt)
{
    целое	    retval;
    целое	    keep_blinking; // Guard against restarting blink cycle on CursorHold

#ifdef FEAT_MENU
    // If we're going to wait a bit, update the menus and mouse shape for the
    // current State.
    если (wtime != 0)
	gui_update_menus(0);
#endif

    gui_mch_update();
    если (input_available())	// Got char, return immediately
    {
	если (gui_mch_is_blinking())
	    gui_mch_stop_blink(TRUE);
	если (buf != NULL && !typebuf_changed(tb_change_cnt))
	    вернуть read_from_input_buf(buf, (long)maxlen);
	вернуть 0;
    }
    если (wtime == 0)		// Don't wait for char
	вернуть FAIL;

    // Before waiting, flush any output to the screen.
    gui_mch_flush();

    // Blink while waiting for a character.
    если (!gui_mch_is_blinking())
	gui_mch_start_blink();

    // Common function to loop until "wtime" is met, while handling timers and
    // other callbacks.
    retval = inchar_loop(buf, maxlen, wtime, tb_change_cnt,
			 gui_wait_for_chars_or_timer, NULL);

    // Keep blinking when CursorHold wakes the input loop. (See PR #21115)
    keep_blinking = retval == 3 && buf != NULL
	&& buf[0] == K_SPECIAL && buf[1] == KS_EXTRA
	&& buf[2] == (целое)KE_CURSORHOLD;

    если (!keep_blinking)
	gui_mch_stop_blink(TRUE);

    вернуть retval;
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
    целое
gui_wait_for_chars(long wtime, целое tb_change_cnt)
{
    вернуть gui_wait_for_chars_buf(NULL, 0, wtime, tb_change_cnt);
}

/*
 * Equivalent of mch_inchar() for the GUI.
 */
    целое
gui_inchar(
    char_u  *buf,
    целое	    maxlen,
    long    wtime,		// milliseconds
    целое	    tb_change_cnt)
{
    вернуть gui_wait_for_chars_buf(buf, maxlen, wtime, tb_change_cnt);
}

/*
 * Fill p[4] with mouse coordinates encoded for check_termcode().
 */
    static void
fill_mouse_coord(char_u *p, целое col, целое row)
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
    целое	    button,
    целое	    x,
    целое	    y,
    целое	    repeated_click,
    int_u   modifiers)
{
    static целое	    prev_row = 0, prev_col = 0;
    static целое	    prev_button = -1;
    static целое	    num_clicks = 1;
    char_u	    string[10];
    enum key_extra  button_char;
    целое		    row, col;
#ifdef FEAT_CLIPBOARD
    целое		    checkfor;
    целое		    did_clip = FALSE;
#endif

    /*
     * Scrolling may happen at any time, also while a selection is present.
     */
    switch (button)
    {
	case MOUSE_MOVE:
	    button_char = KE_MOUSEMOVE_XY;
	    goto button_set;
	case MOUSE_X1:
	    button_char = KE_X1MOUSE;
	    goto button_set;
	case MOUSE_X2:
	    button_char = KE_X2MOUSE;
	    goto button_set;
	case MOUSE_4:
	    button_char = KE_MOUSEDOWN;
	    goto button_set;
	case MOUSE_5:
	    button_char = KE_MOUSEUP;
	    goto button_set;
	case MOUSE_6:
	    button_char = KE_MOUSELEFT;
	    goto button_set;
	case MOUSE_7:
	    button_char = KE_MOUSERIGHT;
button_set:
	    {
		// Don't put events in the input queue now.
		если (hold_gui_events)
		    вернуть;

		row = gui_xy2colrow(x, y, &col);
		// Don't report a mouse move unless moved to a
		// different character position.
		если (button == MOUSE_MOVE)
		{
		    если (row == prev_row && col == prev_col)
			вернуть;
		    иначе
		    {
			prev_row = row >= 0 ? row : 0;
			prev_col = col;
		    }
		}

		string[3] = CSI;
		string[4] = KS_EXTRA;
		string[5] = (целое)button_char;

		// Pass the pointer coordinates of the scroll event so that we
		// know which window to scroll.
		string[6] = (char_u)(col / 128 + ' ' + 1);
		string[7] = (char_u)(col % 128 + ' ' + 1);
		string[8] = (char_u)(row / 128 + ' ' + 1);
		string[9] = (char_u)(row % 128 + ' ' + 1);

		если (modifiers == 0)
		    add_to_input_buf(string + 3, 7);
		иначе
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = 0;
		    если (modifiers & MOUSE_SHIFT)
			string[2] |= MOD_MASK_SHIFT;
		    если (modifiers & MOUSE_CTRL)
			string[2] |= MOD_MASK_CTRL;
		    если (modifiers & MOUSE_ALT)
			string[2] |= MOD_MASK_ALT;
		    add_to_input_buf(string, 10);
		}
		вернуть;
	    }
    }

#ifdef FEAT_CLIPBOARD
    // If a clipboard selection is in progress, handle it
    если (clip_star.state == SELECT_IN_PROGRESS)
    {
	clip_process_selection(button, X_2_COL(x), Y_2_ROW(y), repeated_click);

	// A release event may still need to be sent if the position is equal.
	row = gui_xy2colrow(x, y, &col);
	если (button != MOUSE_RELEASE || row != prev_row || col != prev_col)
	    вернуть;
    }

    // Determine which mouse settings to look for based on the current mode
    switch (get_real_state())
    {
	case MODE_NORMAL_BUSY:
	case MODE_OP_PENDING:
# ifdef FEAT_TERMINAL
	case MODE_TERMINAL:
# endif
	case MODE_NORMAL:	checkfor = MOUSE_NORMAL; break;
	case MODE_VISUAL:	checkfor = MOUSE_VISUAL; break;
	case MODE_SELECT:	checkfor = MOUSE_VISUAL; break;
	case MODE_REPLACE:
	case MODE_REPLACE | MODE_LANGMAP:
	case MODE_VREPLACE:
	case MODE_VREPLACE | MODE_LANGMAP:
	case MODE_INSERT:
	case MODE_INSERT | MODE_LANGMAP:
				checkfor = MOUSE_INSERT; break;
	case MODE_ASKMORE:
	case MODE_HITRETURN:	// At the more- and hit-enter prompt pass the
				// mouse event for a click on or below the
				// message line.
				если (Y_2_ROW(y) >= msg_row)
				    checkfor = MOUSE_NORMAL;
				иначе
				    checkfor = MOUSE_RETURN;
				break;

	    /*
	     * On the command line, use the clipboard selection on all lines
	     * but the command line.  But not when pasting.
	     */
	case MODE_CMDLINE:
	case MODE_CMDLINE | MODE_LANGMAP:
	    если (Y_2_ROW(y) < cmdline_row && button != MOUSE_MIDDLE)
		checkfor = MOUSE_NONE;
	    иначе
		checkfor = MOUSE_COMMAND;
	    break;

	default:
	    checkfor = MOUSE_NONE;
	    break;
    };

    /*
     * Allow clipboard selection of text on the command line in "normal"
     * modes.  Don't do this when dragging the status line, or extending a
     * Visual selection.
     */
    если ((State == MODE_NORMAL || State == MODE_NORMAL_BUSY
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
    если ((modifiers & MOUSE_CTRL) && (modifiers & MOUSE_SHIFT))
	checkfor = MOUSE_NONEF;

    /*
     * In Ex mode, always use modeless selection.
     */
    если (exmode_active)
	checkfor = MOUSE_NONE;

    /*
     * If the mouse settings say to not use the mouse, use the modeless
     * selection.  But if Visual is active, assume that only the Visual area
     * will be selected.
     * Exception: On the command line, both the selection is used and a mouse
     * key is sent.
     */
    если (!mouse_has(checkfor) || checkfor == MOUSE_COMMAND)
    {
	// Don't do modeless selection in Visual mode.
	если (checkfor != MOUSE_NONEF && VIsual_active && (State & MODE_NORMAL))
	    вернуть;

	/*
	 * When 'mousemodel' is "popup", shift-left is translated to right.
	 * But not when also using Ctrl.
	 */
	если (mouse_model_popup() && button == MOUSE_LEFT
		&& (modifiers & MOUSE_SHIFT) && !(modifiers & MOUSE_CTRL))
	{
	    button = MOUSE_RIGHT;
	    modifiers &= ~ MOUSE_SHIFT;
	}

	// If the selection is done, allow the right button to extend it.
	// If the selection is cleared, allow the right button to start it
	// from the cursor position.
	если (button == MOUSE_RIGHT)
	{
	    если (clip_star.state == SELECT_CLEARED)
	    {
		если (State & MODE_CMDLINE)
		{
		    col = msg_col;
		    row = msg_row;
		}
		иначе
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
	иначе если (button == MOUSE_LEFT)
	{
	    clip_start_selection(X_2_COL(x), Y_2_ROW(y), repeated_click);
	    did_clip = TRUE;
	}

	// Always allow pasting
	если (button != MOUSE_MIDDLE)
	{
	    если (!mouse_has(checkfor) || button == MOUSE_RELEASE)
		вернуть;
	    если (checkfor != MOUSE_COMMAND)
		button = MOUSE_LEFT;
	}
	repeated_click = FALSE;
    }

    если (clip_star.state != SELECT_CLEARED && !did_clip)
	clip_clear_selection(&clip_star);
#endif

    // Don't put events in the input queue now.
    если (hold_gui_events)
	вернуть;

    row = gui_xy2colrow(x, y, &col);

    /*
     * If we are dragging and the mouse hasn't moved far enough to be on a
     * different character, then don't send an event to vim.
     */
    если (button == MOUSE_DRAG)
    {
	если (row == prev_row && col == prev_col)
	    вернуть;
	// Dragging above the window, set "row" to -1 to cause a scroll.
	если (y < 0)
	    row = -1;
    }

    /*
     * If topline has changed (window scrolled) since the last click, reset
     * repeated_click, because we don't want starting Visual mode when
     * clicking on a different character in the text.
     */
    если (curwin->w_topline != gui_prev_topline
#ifdef FEAT_DIFF
	    || curwin->w_topfill != gui_prev_topfill
#endif
	    )
	repeated_click = FALSE;

    string[0] = CSI;	// this sequence is recognized by check_termcode()
    string[1] = KS_MOUSE;
    string[2] = KE_FILLER;
    если (button != MOUSE_DRAG && button != MOUSE_RELEASE)
    {
	если (repeated_click)
	{
	    /*
	     * Handle multiple clicks.	They only count if the mouse is still
	     * pointing at the same character.
	     */
	    если (button != prev_button || row != prev_row || col != prev_col)
		num_clicks = 1;
	    иначе если (++num_clicks > 4)
		num_clicks = 1;
	}
	иначе
	    num_clicks = 1;
	prev_button = button;
	gui_prev_topline = curwin->w_topline;
#ifdef FEAT_DIFF
	gui_prev_topfill = curwin->w_topfill;
#endif

	string[3] = (char_u)(button | 0x20);
	SET_NUM_MOUSE_CLICKS(string[3], num_clicks);
    }
    иначе
	string[3] = (char_u)button;

    string[3] |= modifiers;
    fill_mouse_coord(string + 4, col, row);
    add_to_input_buf(string, 8);

    если (row < 0)
	prev_row = 0;
    иначе
	prev_row = row;
    prev_col = col;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#если defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#endif
}

/*
 * Convert x and y coordinate to column and row in text window.
 * Corrects for multi-byte character.
 * returns column in "*colp" and row as return value;
 */
    static целое
gui_xy2colrow(целое x, целое y, целое *colp)
{
    целое		col = check_col(X_2_COL(x));
    целое		row = check_row(Y_2_ROW(y));

    *colp = mb_fix_col(col, row);
    вернуть row;
}

#если defined(FEAT_MENU)
/*
 * Callback function for when a menu entry has been selected.
 */
    void
gui_menu_cb(vimmenu_T *menu)
{
    char_u  bytes[sizeof(long_u)];

    // Don't put events in the input queue now.
    если (hold_gui_events)
	вернуть;

    bytes[0] = CSI;
    bytes[1] = KS_MENU;
    bytes[2] = KE_FILLER;
    add_to_input_buf(bytes, 3);
    add_long_to_buf((long_u)menu, bytes);
    add_to_input_buf_csi(bytes, sizeof(long_u));
}
#endif

static целое	prev_which_scrollbars[3];

/*
 * Set which components are present.
 * If "oldval" is not NULL, "oldval" is the previous value, the new value is
 * in p_go.
 */
    void
gui_init_which_components(char_u *oldval UNUSED)
{
#ifdef FEAT_GUI_DARKTHEME
    static целое	prev_dark_theme = FALSE;
    целое		using_dark_theme = FALSE;
#endif
#ifdef FEAT_MENU
    static целое	prev_menu_is_active = -1;
#endif
#ifdef FEAT_TOOLBAR
    static целое	prev_toolbar = -1;
    целое		using_toolbar = FALSE;
#endif
#ifdef FEAT_GUI_TABLINE
    целое		using_tabline;
#endif
#ifdef FEAT_GUI_MSWIN
    static целое	prev_titlebar = FALSE;
    целое		using_titlebar = FALSE;
#endif

#если defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    static целое	prev_fullscreen = FALSE;
    целое		using_fullscreen = FALSE;
#endif
#если defined(FEAT_MENU)
    static целое	prev_tearoff = -1;
    целое		using_tearoff = FALSE;
#endif

    char_u	*p;
    целое		i;
#ifdef FEAT_MENU
    целое		grey_old, grey_new;
    char_u	*temp;
#endif
    win_T	*wp;
    целое		need_set_size;
    целое		fix_size;

#ifdef FEAT_MENU
    если (oldval != NULL && gui.in_use)
    {
	/*
	 * Check if the menus go from grey to non-grey or vice versa.
	 */
	grey_old = (vim_strchr(oldval, GO_GREY) != NULL);
	grey_new = (vim_strchr(p_go, GO_GREY) != NULL);
	если (grey_old != grey_new)
	{
	    temp = p_go;
	    p_go = oldval;
	    gui_update_menus(MENU_ALL_MODES);
	    p_go = temp;
	}
    }
    gui.menu_is_active = false;
#endif

    для (i = 0; i < 3; i++)
	gui.which_scrollbars[i] = false;
    для (p = p_go; *p; p++)
	switch (*p)
	{
	    case GO_LEFT:
		gui.which_scrollbars[SBAR_LEFT] = true;
		break;
	    case GO_RIGHT:
		gui.which_scrollbars[SBAR_RIGHT] = true;
		break;
	    case GO_VLEFT:
		если (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_LEFT] = true;
		break;
	    case GO_VRIGHT:
		если (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_RIGHT] = true;
		break;
	    case GO_BOT:
		gui.which_scrollbars[SBAR_BOTTOM] = true;
		break;
#ifdef FEAT_GUI_DARKTHEME
	    case GO_DARKTHEME:
		using_dark_theme = TRUE;
		break;
#endif
#ifdef FEAT_MENU
	    case GO_MENUS:
		gui.menu_is_active = true;
		break;
#endif
	    case GO_GREY:
		// make menu's have grey items, ignored here
		break;
#ifdef FEAT_GUI_MSWIN
	    case GO_TITLEBAR:
		using_titlebar = TRUE;
		break;
#endif
#если defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
	    case GO_FULLSCREEN:
		using_fullscreen = TRUE;
		break;
#endif
#ifdef FEAT_TOOLBAR
	    case GO_TOOLBAR:
		using_toolbar = TRUE;
		break;
#endif
	    case GO_TEAROFF:
#если defined(FEAT_MENU)
		using_tearoff = TRUE;
#endif
		break;
	    default:
		// Ignore options that are not supported
		break;
	}

    если (!gui.in_use)
	вернуть;

    need_set_size = 0;
    fix_size = FALSE;

#ifdef FEAT_GUI_MSWIN
    если (using_titlebar != prev_titlebar)
    {
	gui_mch_set_titlebar_colors();
	prev_titlebar = using_titlebar;
    }
#endif

#если defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    если (using_fullscreen != prev_fullscreen)
    {
	gui_mch_set_fullscreen(using_fullscreen);
	prev_fullscreen = using_fullscreen;
    }
#endif

#ifdef FEAT_GUI_DARKTHEME
    если (using_dark_theme != prev_dark_theme)
    {
	gui_mch_set_dark_theme(using_dark_theme);
	prev_dark_theme = using_dark_theme;
    }
#endif

#ifdef FEAT_GUI_TABLINE
    // Update the GUI tab line, it may appear or disappear.  This may
    // cause the non-GUI tab line to disappear or appear.
    using_tabline = gui_has_tabline();
    если (!gui_mch_showing_tabline() != !using_tabline)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = Rows;
	gui_update_tabline();
	Rows = i;
	need_set_size |= RESIZE_VERT;
	если (using_tabline)
	    fix_size = TRUE;
	если (!gui_use_tabline())
	    redraw_tabline = TRUE;    // may draw non-GUI tab line
    }
#endif

    для (i = 0; i < 3; i++)
    {
	// The scrollbar needs to be updated when it is shown/unshown and
	// when switching tab pages.  But the size only changes when it's
	// shown/unshown.  Thus we need two places to remember whether a
	// scrollbar is there or not.
	если (gui.which_scrollbars[i] != prev_which_scrollbars[i]
		|| gui.which_scrollbars[i]
		!= curtab->tp_prev_which_scrollbars[i])
	{
	    если (i == SBAR_BOTTOM)
		gui_mch_enable_scrollbar(&gui.bottom_sbar,
			gui.which_scrollbars[i]);
	    иначе
	    {
		FOR_ALL_WINDOWS(wp)
		    gui_do_scrollbar(wp, i, gui.which_scrollbars[i]);
	    }
	    если (gui.which_scrollbars[i] != prev_which_scrollbars[i])
	    {
		если (i == SBAR_BOTTOM)
		    need_set_size |= RESIZE_VERT;
		иначе
		    need_set_size |= RESIZE_HOR;
		если (gui.which_scrollbars[i])
		    fix_size = TRUE;
	    }
	}
	curtab->tp_prev_which_scrollbars[i] = gui.which_scrollbars[i];
	prev_which_scrollbars[i] = gui.which_scrollbars[i];
    }

#ifdef FEAT_MENU
    если (gui.menu_is_active != prev_menu_is_active)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = Rows;
	gui_mch_enable_menu(gui.menu_is_active);
	Rows = i;
	prev_menu_is_active = gui.menu_is_active;
	need_set_size |= RESIZE_VERT;
	если (gui.menu_is_active)
	    fix_size = TRUE;
    }
#endif

#ifdef FEAT_TOOLBAR
    если (using_toolbar != prev_toolbar)
    {
	gui_mch_show_toolbar(using_toolbar);
	prev_toolbar = using_toolbar;
	need_set_size |= RESIZE_VERT;
	если (using_toolbar)
	    fix_size = TRUE;
    }
#endif
#если defined(FEAT_MENU) && !(defined(MSWIN) && !defined(FEAT_TEAROFF))
    если (using_tearoff != prev_tearoff)
    {
	gui_mch_toggle_tearoffs(using_tearoff);
	prev_tearoff = using_tearoff;
    }
#endif
    если (need_set_size != 0)
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

	    если (!gui.starting && need_set_size != (RESIZE_VERT | RESIZE_HOR))
		(void)char_avail();
	    если ((need_set_size & RESIZE_VERT) == 0)
		Rows = prev_Rows;
	    иначе
		Rows = post_Rows;
	    если ((need_set_size & RESIZE_HOR) == 0)
		Columns = prev_Columns;
	    иначе
		Columns = post_Columns;
	}
#endif
    }
    // When the console tabline appears or disappears the window positions
    // change.
    если (firstwin->w_winrow != tabline_height())
	shell_new_rows();	// recompute window positions and heights
}

#если defined(FEAT_GUI_TABLINE)
/*
 * Return TRUE if the GUI is taking care of the tabline.
 * It may still be hidden if 'showtabline' is zero.
 */
    целое
gui_use_tabline(void)
{
    вернуть gui.in_use && vim_strchr(p_go, GO_TABLINE) != NULL;
}

/*
 * Return TRUE if the GUI is showing the tabline.
 * This uses 'showtabline'.
 */
    static целое
gui_has_tabline(void)
{
    если (!gui_use_tabline()
	    || p_stal == 0
	    || (p_stal == 1 && first_tabpage->tp_next == NULL))
	вернуть FALSE;
    вернуть TRUE;
}

/*
 * Update the tabline.
 * This may display/undisplay the tabline and update the labels.
 */
    void
gui_update_tabline(void)
{
    целое	    showit = gui_has_tabline();
    целое	    shown = gui_mch_showing_tabline();

    если (!gui.starting && starting == 0)
    {
	// Updating the tabline uses direct GUI commands, flush
	// outstanding instructions first. (esp. clear screen)
	out_flush();

	если (!showit != !shown)
	{
	    // Save Rows/Columns before showing/hiding the tabline.
	    // gui_mch_show_tabline() processes GTK events (via
	    // gui_mch_update) which may trigger a spurious configure event
	    // that temporarily corrupts Rows.  Restore the original values so
	    // that gui_set_shellsize() uses the correct row/column count.
	    целое save_Rows = Rows;
	    целое save_Columns = Columns;

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
	если (showit != 0)
	    gui_mch_update_tabline();
    }
}

/*
 * Get the label or tooltip for tab page "tp" into NameBuff[].
 */
    void
get_tabline_label(
    tabpage_T	*tp,
    целое		tooltip)	// TRUE: get tooltip
{
    char_u	**opt;

    // Use 'guitablabel' or 'guitabtooltip' if it's set.
    opt = (tooltip ? &p_gtt : &p_gtl);
    если (**opt != NUL)
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
					   0, (целое)Columns, NULL, NULL, NULL);
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
    если (**opt == NUL || *NameBuff == NUL)
    {
	win_T	*wp;
	целое	wincount = 0;
	целое	modified = FALSE;

	// Get the buffer name into NameBuff[] and shorten it.
	get_trans_bufname(tp == curtab ? curbuf : tp->tp_curwin->w_buffer);
	если (!tooltip)
	    shorten_dir(NameBuff);

	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    ++wincount;
	    если (bufIsChanged(wp->w_buffer))
		modified = TRUE;
	}

	если (modified || wincount > 1)
	{
	    char_u  buf[40] = "+ ";		// Tentatively assume modified only
	    size_t  buflen = 2;
	    size_t  NameBufflen = STRLEN(NameBuff);

	    если (wincount > 1)
		buflen = vim_snprintf_safelen(
		    (char *)buf,
		    sizeof(buf),
		    "%d%s ",
		    wincount,
		    modified ? "+" : "");

	    // Make sure resulting NameBuff will not exceed its bounds.
	    если (NameBufflen + buflen < MAXPATHL)
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
    целое
send_tabline_event(целое nr)
{
    char_u string[3];

    если (nr == tabpage_index(curtab))
	вернуть FALSE;

    // Don't put events in the input queue now.
    если (hold_gui_events || cmdwin_type != 0)
    {
	// Set it back to the current tab page.
	gui_mch_set_curtab(tabpage_index(curtab));
	вернуть FALSE;
    }

    string[0] = CSI;
    string[1] = KS_TABLINE;
    string[2] = KE_FILLER;
    add_to_input_buf(string, 3);
    string[0] = nr;
    add_to_input_buf_csi(string, 1);
    вернуть TRUE;
}

/*
 * Send a tabline menu event
 */
    void
send_tabline_menu_event(целое tabidx, целое event)
{
    char_u	    string[3];

    // Don't put events in the input queue now.
    если (hold_gui_events)
	вернуть;

    // Cannot close the last tabpage.
    если (event == TABLINE_MENU_CLOSE && first_tabpage->tp_next == NULL)
	вернуть;

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
    целое	    i;
    win_T   *wp;

    для (i = 0; i < 3; i++)
    {
	если (i == SBAR_BOTTOM)
	    gui_mch_enable_scrollbar(&gui.bottom_sbar, FALSE);
	иначе
	{
	    FOR_ALL_WINDOWS(wp)
		gui_do_scrollbar(wp, i, FALSE);
	}
	curtab->tp_prev_which_scrollbars[i] = -1;
    }
}

    void
gui_create_scrollbar(scrollbar_T *sb, целое type, win_T *wp)
{
    static целое	sbar_ident = 0;

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

    если (gui.bottom_sbar.ident == ident)
	вернуть &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	если (wp->w_scrollbars[SBAR_LEFT].ident == ident)
	    вернуть &wp->w_scrollbars[SBAR_LEFT];
	если (wp->w_scrollbars[SBAR_RIGHT].ident == ident)
	    вернуть &wp->w_scrollbars[SBAR_RIGHT];
    }
    вернуть NULL;
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
gui_drag_scrollbar(scrollbar_T *sb, long value, целое still_dragging)
{
    win_T	*wp;
    целое		sb_num;
#ifdef USE_ON_FLY_SCROLL
    colnr_T	old_leftcol = curwin->w_leftcol;
    linenr_T	old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    целое		old_topfill = curwin->w_topfill;
# endif
#иначе
    char_u	bytes[sizeof(long_u)];
    целое		byte_count;
#endif

    если (sb == NULL)
	вернуть;

    // Don't put events in the input queue now.
    если (hold_gui_events)
	вернуть;

    если (cmdwin_type != 0 && sb->wp != cmdwin_win)
	вернуть;

    если (still_dragging)
    {
	если (sb->wp == NULL)
	    gui.dragged_sb = SBAR_BOTTOM;
	иначе если (sb == &sb->wp->w_scrollbars[SBAR_LEFT])
	    gui.dragged_sb = SBAR_LEFT;
	иначе
	    gui.dragged_sb = SBAR_RIGHT;
	gui.dragged_wp = sb->wp;
    }
    иначе
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
    если (sb->wp != NULL)
	sb = &sb->wp->w_scrollbars[0];

    /*
     * Check validity of value
     */
    если (value < 0)
	value = 0;
#ifdef SCROLL_PAST_END
    иначе если (value > sb->max)
	value = sb->max;
#иначе
    если (value > sb->max - sb->size + 1)
	value = sb->max - sb->size + 1;
#endif

    sb->value = value;

#ifdef USE_ON_FLY_SCROLL
    // When not allowed to do the scrolling right now, return.
    // This also checked input_available(), but that causes the first click in
    // a scrollbar to be ignored when Vim doesn't have focus.
    если (dont_scroll)
	вернуть;
#endif
    // Disallow scrolling the current window when the completion popup menu is
    // visible.
    если ((sb->wp == NULL || sb->wp == curwin) && pum_visible())
	вернуть;

#ifdef FEAT_RIGHTLEFT
    если (sb->wp == NULL && curwin->w_p_rl)
    {
	value = sb->max + 1 - sb->size - value;
	если (value < 0)
	    value = 0;
    }
#endif

    если (sb->wp != NULL)		// vertical scrollbar
    {
	sb_num = 0;
	для (wp = firstwin; wp != sb->wp && wp != NULL; wp = wp->w_next)
	    sb_num++;
	если (wp == NULL)
	    вернуть;

#ifdef USE_ON_FLY_SCROLL
	current_scrollbar = sb_num;
	scrollbar_value = value;
	если (State & MODE_NORMAL)
	{
	    gui_do_scroll();
	    setcursor();
	}
	иначе если (State & MODE_INSERT)
	{
	    ins_scroll();
	    setcursor();
	}
	иначе если (State & MODE_CMDLINE)
	{
	    если (msg_scrolled == 0)
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
	если (gui.which_scrollbars[SBAR_RIGHT] && gui.which_scrollbars[SBAR_LEFT])
	    gui_mch_set_scrollbar_thumb(
		    &sb->wp->w_scrollbars[
			    sb == &sb->wp->w_scrollbars[SBAR_RIGHT]
						    ? SBAR_LEFT : SBAR_RIGHT],
		    sb->value, sb->size, sb->max);

#иначе
	bytes[0] = CSI;
	bytes[1] = KS_VER_SCROLLBAR;
	bytes[2] = KE_FILLER;
	bytes[3] = (char_u)sb_num;
	byte_count = 4;
#endif
    }
    иначе
    {
#ifdef USE_ON_FLY_SCROLL
	scrollbar_value = value;

	если (State & MODE_NORMAL)
	    do_mousescroll_horiz(scrollbar_value);
	иначе если (State & MODE_INSERT)
	    ins_horscroll();
	иначе если (State & MODE_CMDLINE)
	{
	    если (msg_scrolled == 0)
	    {
		do_mousescroll_horiz(scrollbar_value);
		redrawcmdline();
	    }
	}
	если (old_leftcol != curwin->w_leftcol)
	{
	    updateWindow(curwin);   // update window, status and cmdline
	    setcursor();
	}
#иначе
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
    если (curwin->w_p_scb
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
	    если (wp->w_redr_type > 0)
		updateWindow(wp);
	setcursor();
    }
    out_flush_cursor(FALSE, TRUE);
#иначе
    add_to_input_buf(bytes, byte_count);
    add_long_to_buf((long_u)value, bytes);
    add_to_input_buf_csi(bytes, sizeof(long_u));
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
    если (gui.in_use && starting == 0)
    {
	out_flush();
	gui_init_which_components(NULL);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
}

    void
gui_update_scrollbars(
    целое		force)	    // Force all scrollbars to get updated
{
    win_T	*wp;
    scrollbar_T	*sb;
    long	val, size, max;		// need 32 bits here
    целое		which_sb;
    целое		h, y;
    static win_T *prev_curwin = NULL;

    // Update the horizontal scrollbar
    gui_update_horiz_scrollbar(force);

#ifndef MSWIN
    // Return straight away if there is neither a left nor right scrollbar.
    // On MS-Windows this is required anyway for scrollwheel messages.
    если (!gui.which_scrollbars[SBAR_LEFT] && !gui.which_scrollbars[SBAR_RIGHT])
	вернуть;
#endif

    /*
     * Don't want to update a scrollbar while we're dragging it.  But if we
     * have both a left and right scrollbar, and we drag one of them, we still
     * need to update the other one.
     */
    если (!force && (gui.dragged_sb == SBAR_LEFT || gui.dragged_sb == SBAR_RIGHT)
	    && gui.which_scrollbars[SBAR_LEFT]
	    && gui.which_scrollbars[SBAR_RIGHT])
    {
	/*
	 * If we have two scrollbars and one of them is being dragged, just
	 * copy the scrollbar position from the dragged one to the other one.
	 */
	which_sb = SBAR_LEFT + SBAR_RIGHT - gui.dragged_sb;
	если (gui.dragged_wp != NULL)
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
	если (wp->w_buffer == NULL)	// just in case
	    continue;
	// Skip a scrollbar that is being dragged.
	если (!force && (gui.dragged_sb == SBAR_LEFT
					     || gui.dragged_sb == SBAR_RIGHT)
		&& gui.dragged_wp == wp)
	    continue;

#ifdef SCROLL_PAST_END
	max = wp->w_buffer->b_ml.ml_line_count - 1;
#иначе
	max = wp->w_buffer->b_ml.ml_line_count + wp->w_height - 2;
#endif
	если (max < 0)			// empty buffer
	    max = 0;
	val = wp->w_topline - 1;
	size = wp->w_height;
#ifdef SCROLL_PAST_END
	если (val > max)			// just in case
	    val = max;
#иначе
	если (size > max + 1)		// just in case
	    size = max + 1;
	если (val > max - size + 1)
	    val = max - size + 1;
#endif
	если (val < 0)			// minimal value is 0
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
	если (size < 1 || wp->w_botline - 2 > max)
	{
	    /*
	     * This can happen during changing files.  Just don't update the
	     * scrollbar for now.
	     */
	    sb->height = 0;	    // Force update next time
	    если (gui.which_scrollbars[SBAR_LEFT])
		gui_do_scrollbar(wp, SBAR_LEFT, FALSE);
	    если (gui.which_scrollbars[SBAR_RIGHT])
		gui_do_scrollbar(wp, SBAR_RIGHT, FALSE);
	    continue;
	}
	если (force || sb->height != wp->w_height
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
#если defined(FEAT_MENU) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF) && !defined(FEAT_GUI_PHOTON)
	    если (gui.menu_is_active)
		y += gui.menu_height;
#endif

#если defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU))
	    если (vim_strchr(p_go, GO_TOOLBAR) != NULL)
		y += gui.toolbar_height;
#endif

#если defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
	    если (gui_has_tabline())
		y += gui.tabline_height;
#endif

	    если (wp->w_winrow == 0)
	    {
		// Height of top scrollbar includes width of top border
		h += gui.border_offset;
		y -= gui.border_offset;
	    }
	    если (gui.which_scrollbars[SBAR_LEFT])
	    {
		gui_mch_set_scrollbar_pos(&wp->w_scrollbars[SBAR_LEFT],
					  gui.left_sbar_x, y,
					  gui.scrollbar_width, h);
		gui_do_scrollbar(wp, SBAR_LEFT, TRUE);
	    }
	    если (gui.which_scrollbars[SBAR_RIGHT])
	    {
		gui_mch_set_scrollbar_pos(&wp->w_scrollbars[SBAR_RIGHT],
					  gui.right_sbar_x, y,
					  gui.scrollbar_width, h);
		gui_do_scrollbar(wp, SBAR_RIGHT, TRUE);
	    }
	}

	если (force || sb->value != val || sb->size != size || sb->max != max)
	{
	    // Thumb of scrollbar has moved
	    sb->value = val;
	    sb->size = size;
	    sb->max = max;
	    если (gui.which_scrollbars[SBAR_LEFT]
		    && (gui.dragged_sb != SBAR_LEFT || gui.dragged_wp != wp))
		gui_mch_set_scrollbar_thumb(&wp->w_scrollbars[SBAR_LEFT],
					    val, size, max);
	    если (gui.which_scrollbars[SBAR_RIGHT]
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
    целое		which,	    // SBAR_LEFT or SBAR_RIGHT
    целое		enable)	    // TRUE to enable scrollbar
{
    целое		midcol = curwin->w_wincol + curwin->w_width / 2;
    целое		has_midcol = (wp->w_wincol <= midcol
				     && wp->w_wincol + wp->w_width >= midcol);

    // Only enable scrollbars that contain the middle column of the current
    // window.
    если (gui.which_scrollbars[SBAR_RIGHT] != gui.which_scrollbars[SBAR_LEFT])
    {
	// Scrollbars only on one side.  Don't enable scrollbars that don't
	// contain the middle column of the current window.
	если (!has_midcol)
	    enable = FALSE;
    }
    иначе
    {
	// Scrollbars on both sides.  Don't enable scrollbars that neither
	// contain the middle column of the current window nor are on the far
	// side.
	если (midcol > Columns / 2)
	{
	    если (which == SBAR_LEFT ? wp->w_wincol != 0 : !has_midcol)
		enable = FALSE;
	}
	иначе
	{
	    если (which == SBAR_RIGHT ? wp->w_wincol + wp->w_width != Columns
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
    целое
gui_do_scroll(void)
{
    win_T	*wp, *save_wp;
    целое		i;
    long	nlines;
    pos_T	old_cursor;
    linenr_T	old_topline;
#ifdef FEAT_DIFF
    целое		old_topfill;
#endif

    для (wp = firstwin, i = 0; i < current_scrollbar; wp = W_NEXT(wp), i++)
	если (wp == NULL)
	    break;
    если (wp == NULL)
	// Couldn't find window
	вернуть FALSE;
    // don't redraw, LineOffset and similar are not valid!
    если (exmode_active)
	вернуть FALSE;

    /*
     * Compute number of lines to scroll.  If zero, nothing to do.
     */
    nlines = (long)scrollbar_value + 1 - (long)wp->w_topline;
    если (nlines == 0)
	вернуть FALSE;

    save_wp = curwin;
    old_topline = wp->w_topline;
#ifdef FEAT_DIFF
    old_topfill = wp->w_topfill;
#endif
    old_cursor = wp->w_cursor;
    curwin = wp;
    curbuf = wp->w_buffer;
    если (nlines < 0)
	scrolldown(-nlines, gui.dragged_wp == NULL);
    иначе
	scrollup(nlines, gui.dragged_wp == NULL);
    // Reset dragged_wp after using it.  "dragged_sb" will have been reset for
    // the mouse-up event already, but we still want it to behave like when
    // dragging.  But not the next click in an arrow.
    если (gui.dragged_sb == SBAR_NONE)
	gui.dragged_wp = NULL;

    если (old_topline != wp->w_topline
#ifdef FEAT_DIFF
	    || old_topfill != wp->w_topfill
#endif
	    )
    {
	если (get_scrolloff_value() != 0)
	{
	    cursor_correct();		// fix window for 'so'
	    update_topline();		// avoid up/down jump
	}
	если (old_cursor.lnum != wp->w_cursor.lnum)
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
    если (ScreenLines != NULL && (old_topline != wp->w_topline
	    || wp->w_redr_type != 0
#ifdef FEAT_DIFF
	    || old_topfill != wp->w_topfill
#endif
	    ))
    {
	целое type = UPD_VALID;

	если (pum_visible())
	{
	    type = UPD_NOT_VALID;
	    wp->w_lines_valid = 0;
	}

	// Don't set must_redraw here, it may cause the popup menu to
	// disappear when losing focus after a scrollbar drag.
	если (wp->w_redr_type < type)
	    wp->w_redr_type = type;
	mch_disable_flush();
	updateWindow(wp);   // update window, status line, and cmdline
	mch_enable_flush();
    }

    // May need to redraw the popup menu.
    если (pum_visible())
	pum_redraw();

    вернуть (wp == curwin && !EQUAL_POS(curwin->w_cursor, old_cursor));
}

/*
 * Horizontal scrollbar stuff:
 */
    static void
gui_update_horiz_scrollbar(целое force)
{
    long	value, size, max;

    если (!gui.which_scrollbars[SBAR_BOTTOM])
	вернуть;

    если (!force && gui.dragged_sb == SBAR_BOTTOM)
	вернуть;

    если (!force && curwin->w_p_wrap && gui.prev_wrap)
	вернуть;

    /*
     * It is possible for the cursor to be invalid if we're in the middle of
     * something (like changing files).  If so, don't do anything for now.
     */
    если (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
    {
	gui.bottom_sbar.value = -1;
	вернуть;
    }

    size = curwin->w_width;
    если (curwin->w_p_wrap)
    {
	value = 0;
#ifdef SCROLL_PAST_END
	max = 0;
#иначе
	max = curwin->w_width - 1;
#endif
    }
    иначе
    {
	value = curwin->w_leftcol;
	max = scroll_line_len(ui_find_longest_lnum());

	если (virtual_active())
	{
	    // May move the cursor even further to the right.
	    если (curwin->w_virtcol >= (colnr_T)max)
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
    если (value > max - size + 1)
	value = max - size + 1;	    // limit the value to allowable range
#endif

#ifdef FEAT_RIGHTLEFT
    если (curwin->w_p_rl)
    {
	value = max + 1 - size - value;
	если (value < 0)
	{
	    size += value;
	    value = 0;
	}
    }
#endif
    если (!force && value == gui.bottom_sbar.value && size == gui.bottom_sbar.size
						&& max == gui.bottom_sbar.max)
	вернуть;

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
    если (gui.norm_pixel == gui.back_pixel || gui.norm_pixel == INVALCOLOR)
    {
	gui_set_bg_color((char_u *)"White");
	если (gui.norm_pixel == gui.back_pixel || gui.norm_pixel == INVALCOLOR)
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

    если (*name == NUL)
	вернуть INVALCOLOR;
    t = gui_mch_get_color(name);

    целое is_none = STRCMP(name, "none") == 0;
    если (t == INVALCOLOR
#если defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
	    && (gui.in_use || is_none)
#endif
	    )
    {
	если (is_none)
	    emsg(_(e_cannot_use_color_none_did_you_mean_none));
	иначе
	    semsg(_(e_cannot_allocate_color_str), name);
    }
    вернуть t;
}

/*
 * Return the grey value of a color (range 0-255).
 */
    целое
gui_get_lightness(guicolor_T pixel)
{
    long_u	rgb = (long_u)gui_mch_get_rgb(pixel);

    вернуть  (целое)(  (((rgb >> 16) & 0xff) * 299)
		   + (((rgb >> 8) & 0xff) * 587)
		   +  ((rgb	  & 0xff) * 114)) / 1000;
}

    char_u *
gui_bg_default(void)
{
    если (gui_get_lightness(gui.back_pixel) < 127)
	вернуть (char_u *)"dark";
    вернуть (char_u *)"light";
}

/*
 * Option initializations that can only be done after opening the GUI window.
 */
    static void
init_gui_options(void)
{
    // Set the 'background' option according to the lightness of the
    // background color, unless the user has set it already.
    если (!option_was_set((char_u *)"bg") && STRCMP(p_bg, gui_bg_default()) != 0)
    {
	set_option_value_give_err((char_u *)"bg", 0L, gui_bg_default(), 0);
	highlight_changed();
    }
}

#если defined(FEAT_GUI_X11)
    void
gui_new_scrollbar_colors(void)
{
    win_T	*wp;

    // Nothing to do if GUI hasn't started yet.
    если (!gui.in_use)
	вернуть;

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
gui_focus_change(целое in_focus)
{
/*
 * Skip this code to avoid drawing the cursor when debugging and switching
 * between the debugger window and gvim.
 */
#если 1
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
    если (!hold_gui_events
# если defined(FEAT_GUI_GTK) && defined(FEAT_GUI_DIALOG)
	    && gui.dialogs_active == 0
# endif
       )
    {
	char_u  bytes[3];

	bytes[0] = CSI;
	bytes[1] = KS_EXTRA;
	bytes[2] = in_focus ? (целое)KE_FOCUSGAINED : (целое)KE_FOCUSLOST;
	add_to_input_buf(bytes, 3);
    }
#endif
}

/*
 * When mouse moved: apply 'mousefocus'.
 * Also updates the mouse pointer shape.
 */
    static void
gui_mouse_focus(целое x, целое y)
{
    win_T	*wp;
    char_u	st[8];

#ifdef FEAT_MOUSESHAPE
    // Get window pointer, and update mouse shape as well.
    wp = xy2win(x, y, IGNORE_POPUP);
#endif

    // Only handle this when 'mousefocus' set and ...
    если (p_mousef
	    && !hold_gui_events		// not holding events
	    && (State & (MODE_NORMAL | MODE_INSERT))// Normal/Visual/Insert mode
	    && State != MODE_HITRETURN	// but not hit-return prompt
	    && msg_scrolled == 0	// no scrolled message
	    && !need_mouse_correct	// not moving the pointer
	    && gui.in_focus)		// gvim in focus
    {
	// Don't move the mouse when it's left or right of the Vim window
	если (x < 0 || x > Columns * gui.char_width)
	    вернуть;
#ifndef FEAT_MOUSESHAPE
	wp = xy2win(x, y, IGNORE_POPUP);
#endif
	если (wp == curwin || wp == NULL)
	    вернуть;	// still in the same old window, or none at all

	// Ignore position in the tab pages line.
	если (Y_2_ROW(y) < tabline_height())
	    вернуть;

	/*
	 * Format a mouse click on status line input,
	 * ala gui_send_mouse_event(0, x, y, 0, 0);
	 * Trick: Use a column number -1, so that get_pseudo_mouse_code() will
	 * generate a K_LEFTMOUSE_NM key code.
	 */
	если (finish_op)
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
	если (gtk_main_level() > 0)
	    gtk_main_quit();
#endif
    }
}

/*
 * Called when the mouse moved (but not when dragging).
 */
    void
gui_mouse_moved(целое x, целое y)
{
    // Ignore this while still starting up.
    если (!gui.in_use || gui.starting)
	вернуть;

    // apply 'mousefocus' and pointer shape
    gui_mouse_focus(x, y);

    если (p_mousemev
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
    целое		x, y;

    если (!(gui.in_use && (p_mousef || popup == FIND_POPUP)))
	вернуть NULL;
    gui_mch_getmouse(&x, &y);

    // Only use the mouse when it's on the Vim window
    если (x >= 0 && x <= Columns * gui.char_width
	    && y >= 0 && Y_2_ROW(y) >= tabline_height())
	вернуть xy2win(x, y, popup);
    вернуть NULL;
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
    если (wp == curwin || wp == NULL)
	вернуть;

    // If in other than current window
    validate_cline_row();
    gui_mch_setmouse((целое)W_ENDCOL(curwin) * gui.char_width - 3,
	    (W_WINROW(curwin) + curwin->w_wrow) * gui.char_height
	    + (gui.char_height) / 2);
}

/*
 * Find window where the mouse pointer "x" / "y" coordinate is in.
 * As a side effect update the shape of the mouse pointer.
 */
    static win_T *
xy2win(целое x, целое y, mouse_find_T popup)
{
    целое		row;
    целое		col;
    win_T	*wp;

    row = Y_2_ROW(y);
    col = X_2_COL(x);

    если (row < 0 || col < 0)		// before first window
	вернуть NULL;
    wp = mouse_find_win(&row, &col, popup);
    если (wp == NULL)
    {
#ifdef FEAT_MOUSESHAPE
	update_mouseshape(-2);
#endif
	вернуть NULL;
    }
#ifdef FEAT_MOUSESHAPE
    если (State == MODE_HITRETURN || State == MODE_ASKMORE)
    {
	если (Y_2_ROW(y) >= msg_row)
	    update_mouseshape(SHAPE_IDX_MOREL);
	иначе
	    update_mouseshape(SHAPE_IDX_MORE);
    }
    иначе если (row >= wp->w_height + wp->w_status_height)	// below status line
	update_mouseshape(SHAPE_IDX_CLINE);
    иначе если (!(State & MODE_CMDLINE) && wp->w_vsep_width > 0 && col == wp->w_width
	    && (!(row >= wp->w_height && row < wp->w_height
	    + wp->w_status_height) || !stl_connected(wp)) && msg_scrolled == 0)
	update_mouseshape(SHAPE_IDX_VSEP);
    иначе если (!(State & MODE_CMDLINE) && wp->w_status_height > 0
	    && row >= wp->w_height && row < wp->w_height + wp->w_status_height
	    && msg_scrolled == 0)
	update_mouseshape(SHAPE_IDX_STATUS);
    иначе
	update_mouseshape(-2);
#endif
    вернуть wp;
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
    если (arg[0] == '-'
	    && (arg[1] == 'f' || arg[1] == 'b')
	    && (arg[2] == NUL || VIM_ISWHITE(arg[2])))
    {
	gui.dofork = (arg[1] == 'b');
	eap->arg = skipwhite(eap->arg + 2);
    }
    если (!gui.in_use)
    {
#если defined(VIMDLL) && !defined(EXPERIMENTAL_GUI_CMD)
	если (!gui.starting)
	{
	    emsg(_(e_gui_cannot_be_used_not_enabled_at_compile_time));
	    вернуть;
	}
#endif
	// Clear the command.  Needed for when forking+exiting, to avoid part
	// of the argument ending up after the shell prompt.
	msg_clr_eos_force();
#ifdef GUI_MAY_SPAWN
	если (!ends_excmd2(eap->cmd, eap->arg))
	    gui_start(eap->arg);
	иначе
#endif
	    gui_start(NULL);
#ifdef FEAT_JOB_CHANNEL
	channel_gui_register_all();
#endif
    }
    если (!ends_excmd2(eap->cmd, eap->arg))
	ex_next(eap);
}

#если (defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
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

    если (STRLEN(fname) >= MAXPATHL)
	*gfp_buffer = NUL;
    иначе
	STRCPY(gfp_buffer, fname);
}

/*
 * Find the path of bitmap "name" with extension "ext" in 'runtimepath'.
 * Return FAIL for failure and OK if buffer[MAXPATHL] contains the result.
 */
    целое
gui_find_bitmap(char_u *name, char_u *buffer, char *ext)
{
    если (STRLEN(name) > MAXPATHL - 14)
	вернуть FAIL;
    vim_snprintf((char *)buffer, MAXPATHL, "bitmaps/%s.%s", name, ext);
    если (do_in_runtimepath(buffer, 0, gfp_setname, buffer) == FAIL
							    || *buffer == NUL)
	вернуть FAIL;
    вернуть OK;
}

# если !defined(FEAT_GUI_GTK)
/*
 * Given the name of the "icon=" argument, try finding the bitmap file for the
 * icon.  If it is an absolute path name, use it as it is.  Otherwise append
 * "ext" and search for it in 'runtimepath'.
 * The result is put in "buffer[MAXPATHL]".  If something fails "buffer"
 * contains "name".
 */
    void
gui_find_iconfile(char_u *name, char_u *buffer, char *ext)
{
    char_u	buf[MAXPATHL + 1];

    expand_env(name, buffer, MAXPATHL);
    если (!mch_isFullName(buffer) && gui_find_bitmap(buffer, buf, ext) == OK)
	STRCPY(buffer, buf);
}
# endif
#endif

#если defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)|| defined(FEAT_GUI_HAIKU)
    void
display_errors(void)
{
    char_u	*p;

    если (isatty(2))
    {
	fflush(stderr);
	вернуть;
    }

    если (error_ga.ga_data == NULL)
	вернуть;

    // avoid putting up a message box with blanks only
    для (p = (char_u *)error_ga.ga_data; *p != NUL; ++p)
	если (!SAFE_isspace(*p))
	{
	    // Truncate a very long message, it will go off-screen.
	    если (STRLEN(p) > 2000)
		STRCPY(p + 2000 - 14, "...(truncated)");
	    (void)do_dialog(VIM_ERROR, (char_u *)_("Error"),
		    p, (char_u *)_("&Ok"), 1, NULL, FALSE);
	    break;
	}
    ga_clear(&error_ga);
}
#endif

#если defined(NO_CONSOLE_INPUT)
/*
 * Return TRUE if still starting up and there is no place to enter text.
 * For GTK and X11 we check if stderr is not a tty, which means we were
 * (probably) started from the desktop.  Also check stdin, "vim >& file" does
 * allow typing on stdin.
 */
    целое
no_console_input(void)
{
    вернуть ((!gui.in_use || gui.starting)
# ifndef NO_CONSOLE
	    && !isatty(0) && !isatty(2)
# endif
	    );
}
#endif

#если defined(FIND_REPLACE_DIALOG) \
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
    целое		conceal_update_lines = FALSE;
# endif

    update_topline();
    validate_cursor();

    // Trigger CursorMoved if the cursor moved.
    если (!finish_op && (has_cursormoved()
# ifdef FEAT_PROP_POPUP
		|| popup_visible
# endif
# ifdef FEAT_CONCEAL
		|| curwin->w_p_cole > 0
# endif
		) && !EQUAL_POS(last_cursormoved, curwin->w_cursor))
    {
	если (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, NULL, NULL, FALSE, curbuf);
# ifdef FEAT_PROP_POPUP
	если (popup_visible)
	    popup_check_cursor_pos();
# endif
# ifdef FEAT_CONCEAL
	если (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_line = last_cursormoved.lnum;
	    conceal_new_cursor_line = curwin->w_cursor.lnum;
	    conceal_update_lines = TRUE;
	}
# endif
	last_cursormoved = curwin->w_cursor;
    }

    если (!finish_op)
	may_trigger_win_scrolled_resized();

# ifdef FEAT_CONCEAL
    если (conceal_update_lines
	    && (conceal_old_cursor_line != conceal_new_cursor_line
		|| conceal_cursor_line(curwin)
		|| need_cursor_line_redraw))
    {
	если (conceal_old_cursor_line != conceal_new_cursor_line)
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

#если defined(FIND_REPLACE_DIALOG)
/*
 * Get the text to use in a find/replace dialog.  Uses the last search pattern
 * if the argument is empty.
 * Returns an allocated string.
 */
    char_u *
get_find_dialog_text(
    char_u	*arg,
    целое		*wwordp,	// return: TRUE if \< \> found
    целое		*mcasep)	// return: TRUE if \C found
{
    char_u	*text;

    если (*arg == NUL)
	text = last_search_pat();
    иначе
	text = arg;
    если (text != NULL)
    {
	целое len = (целое)STRLEN(text);

	text = vim_strnsave(text, len);
	если (text != NULL)
	{
	    целое i;

	    // Remove "\V"
	    если (len >= 2 && STRNCMP(text, "\\V", 2) == 0)
	    {
		mch_memmove(text, text + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\c" and "\C" and remove.
	    если (len >= 2 && *text == '\\' && (text[1] == 'c' || text[1] == 'C'))
	    {
		*mcasep = (text[1] == 'C');
		mch_memmove(text, text + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\<text\>" and remove.
	    если (len >= 4
		    && STRNCMP(text, "\\<", 2) == 0
		    && STRNCMP(text + len - 2, "\\>", 2) == 0)
	    {
		*wwordp = TRUE;
		mch_memmove(text, text + 2, (size_t)(len - 4));
		text[len - 4] = NUL;
	    }

	    // Recognize "\/" or "\?" and remove.
	    для (i = 0; i + 1 < len; ++i)
		если (text[i] == '\\' && (text[i + 1] == '/'
						       || text[i + 1] == '?'))
		{
		    mch_memmove(text + i, text + i + 1, (size_t)(len - i));
		    --len;
		}
	}
    }
    вернуть text;
}

/*
 * Handle the press of a button in the find-replace dialog.
 * Return TRUE when something was added to the input buffer.
 */
    целое
gui_do_findrepl(
    целое		flags,		// one of FRD_REPLACE, FRD_FINDNEXT, etc.
    char_u	*find_text,
    char_u	*repl_text,
    целое		down)		// Search downwards.
{
    garray_T	ga;
    целое		i;
    целое		type = (flags & FRD_TYPE_MASK);
    char_u	*p;
    regmatch_T	regmatch;
    целое		save_did_emsg = did_emsg;
    static целое  busy = FALSE;

    // When the screen is being updated we should not change buffers and
    // windows structures, it may cause freed memory to be used.  Also don't
    // do this recursively (pressing "Find" quickly several times).
    если (updating_screen || busy)
	вернуть FALSE;

    // refuse replace when text cannot be changed
    если ((type == FRD_REPLACE || type == FRD_REPLACEALL) && text_locked())
	вернуть FALSE;

    busy = TRUE;

    ga_init2(&ga, 1, 100);
    если (type == FRD_REPLACEALL)
	GA_CONCAT_LITERAL(&ga, "%s/");

    GA_CONCAT_LITERAL(&ga, "\\V");
    если (flags & FRD_MATCH_CASE)
	GA_CONCAT_LITERAL(&ga, "\\C");
    иначе
	GA_CONCAT_LITERAL(&ga, "\\c");
    если (flags & FRD_WHOLE_WORD)
	GA_CONCAT_LITERAL(&ga, "\\<");
    // escape slash and backslash
    p = vim_strsave_escaped(find_text, (char_u *)"/\\");
    если (p != NULL)
	ga_concat(&ga, p);
    vim_free(p);
    если (flags & FRD_WHOLE_WORD)
	GA_CONCAT_LITERAL(&ga, "\\>");

    если (type == FRD_REPLACEALL)
    {
	GA_CONCAT_LITERAL(&ga, "/");
	// Escape slash and backslash.
	// Also escape tilde and ampersand if 'magic' is set.
	p = vim_strsave_escaped(repl_text,
				p_magic ? (char_u *)"/\\~&" : (char_u *)"/\\");
	если (p != NULL)
	    ga_concat(&ga, p);
	vim_free(p);
	GA_CONCAT_LITERAL(&ga, "/g");
    }
    ga_append(&ga, NUL);

    если (type == FRD_REPLACE)
    {
	// Do the replacement when the text at the cursor matches.  Thus no
	// replacement is done if the cursor was moved!
	regmatch.regprog = vim_regcomp(ga.ga_data, RE_MAGIC + RE_STRING);
	regmatch.rm_ic = 0;
	если (regmatch.regprog != NULL)
	{
	    p = ml_get_cursor();
	    если (vim_regexec_nl(&regmatch, p, (colnr_T)0)
						   && regmatch.startp[0] == p)
	    {
		// Clear the command line to remove any old "No match"
		// error.
		msg_end_prompt();

		если (u_save_cursor() == OK)
		{
		    // A button was pressed thus undo should be synced.
		    u_sync(FALSE);

		    del_bytes((long)(regmatch.endp[0] - regmatch.startp[0]),
								FALSE, FALSE);
		    ins_str(repl_text, STRLEN(repl_text));
		}
	    }
	    иначе
		msg(_("No match at cursor, finding next"));
	    vim_regfree(regmatch.regprog);
	}
    }

    если (type == FRD_REPLACEALL)
    {
	// A button was pressed, thus undo should be synced.
	u_sync(FALSE);
	do_cmdline_cmd(ga.ga_data);
    }
    иначе
    {
	целое searchflags = SEARCH_MSG + SEARCH_MARK;

	// Search for the next match.
	// Don't skip text under cursor for single replace.
	если (type == FRD_REPLACE)
	    searchflags += SEARCH_START;
	i = msg_scroll;
	если (down)
	{
	    (void)do_search(NULL, '/', '/', ga.ga_data, STRLEN(ga.ga_data), 1L, searchflags, NULL);
	}
	иначе
	{
	    // We need to escape '?' if and only if we are searching in the up
	    // direction
	    p = vim_strsave_escaped(ga.ga_data, (char_u *)"?");
	    если (p != NULL)
		(void)do_search(NULL, '?', '?', p, STRLEN(p), 1L, searchflags, NULL);
	    vim_free(p);
	}

	msg_scroll = i;	    // don't let an error message set msg_scroll
    }

    // Don't want to pass did_emsg to other code, it may cause disabling
    // syntax HL if we were busy redrawing.
    did_emsg = save_did_emsg;

    если (State & (MODE_NORMAL | MODE_INSERT))
    {
	gui_update_screen();		// update the screen
	msg_didout = 0;			// overwrite any message
	need_wait_return = FALSE;	// don't wait for return
    }

    vim_free(ga.ga_data);
    busy = FALSE;
    вернуть (ga.ga_len > 0);
}

#endif

#если defined(HAVE_DROP_FILE)
/*
 * Jump to the window at specified point (x, y).
 */
    static void
gui_wingoto_xy(целое x, целое y)
{
    целое		row = Y_2_ROW(y);
    целое		col = X_2_COL(x);
    win_T	*wp;

    если (row < 0 || col < 0)
	вернуть;

    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    если (wp != NULL && wp != curwin)
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
    целое		do_shorten = FALSE;

    // If Shift held down, change to first file's directory.  If the first
    // item is a directory, change to that directory (and let the explorer
    // plugin show the contents).
    если (p != NULL)
    {
	если (mch_isdir(p))
	{
	    если (mch_chdir((char *)p) == 0)
		do_shorten = TRUE;
	}
	иначе если (vim_chdirfile(p, "drop") == OK)
	    do_shorten = TRUE;
	vim_free(p);
	если (do_shorten)
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
    целое		x UNUSED,
    целое		y UNUSED,
    int_u	modifiers,
    char_u	**fnames,
    целое		count)
{
    целое		i;
    char_u	*p;
    static целое	entered = FALSE;

    /*
     * This function is called by event handlers.  Just in case we get a
     * second event before the first one is handled, ignore the second one.
     * Not sure if this can ever happen, just in case.
     */
    если (entered)
	вернуть;
    entered = TRUE;

    /*
     * When the cursor is at the command line, add the file names to the
     * command line, don't edit the files.
     */
    если (State & MODE_CMDLINE)
    {
	shorten_filenames(fnames, count);
	для (i = 0; i < count; ++i)
	{
	    если (fnames[i] != NULL)
	    {
		если (i > 0)
		    add_to_input_buf((char_u*)" ", 1);

		// We don't know what command is used thus we can't be sure
		// about which characters need to be escaped.  Only escape the
		// most common ones.
# ifdef BACKSLASH_IN_FILENAME
		p = vim_strsave_escaped(fnames[i], (char_u *)" \t\"|");
# иначе
		p = vim_strsave_escaped(fnames[i], (char_u *)"\\ \t\"|");
# endif
		если (p != NULL)
		    add_to_input_buf_csi(p, (целое)STRLEN(p));
		vim_free(p);
		vim_free(fnames[i]);
	    }
	}
	vim_free(fnames);
    }
    иначе
    {
	// Go to the window under mouse cursor, then shorten given "fnames" by
	// current window, because a window can have local current dir.
	gui_wingoto_xy(x, y);
	shorten_filenames(fnames, count);

	// If Shift held down, remember the first item.
	если ((modifiers & MOUSE_SHIFT) != 0)
	    p = vim_strsave(fnames[0]);
	иначе
	    p = NULL;

	// Handle the drop, :edit or :split to get to the file.  This also
	// frees fnames[].  Skip this if there is only one item, it's a
	// directory and Shift is held down.
	если (count == 1 && (modifiers & MOUSE_SHIFT) != 0
						     && mch_isdir(fnames[0]))
	{
	    vim_free(fnames[0]);
	    vim_free(fnames);
	    vim_free(p);
	}
	иначе
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
    целое
check_for_interrupt(целое key, целое modifiers_arg)
{
    целое modifiers = modifiers_arg;
    целое c = merge_modifyOtherKeys(key, &modifiers);

    если ((c == Ctrl_C && ctrl_c_interrupts)
#ifdef UNIX
	    || (intr_char != Ctrl_C && c == intr_char)
#endif
	    )
    {
	got_int = TRUE;
	вернуть c;
    }
    вернуть NUL;
}

/*
 * If the "--gui-log-file fname" argument is given write the dialog title and
 * message to a file and return TRUE.  Otherwise return FALSE.
 * When there is any problem opening the file or writing to the file this is
 * ignored, showing the dialog might get the test to get stuck.
 */
    целое
gui_dialog_log(char_u *title, char_u *message)
{
    char_u  *fname = get_gui_dialog_file();
    FILE    *fd;

    если (fname == NULL)
	вернуть FALSE;

    fd = mch_fopen((char *)fname, "a");
    если (fd != NULL)
    {
	fprintf(fd, "%s: %s\n", title, message);
	fclose(fd);
    }
    вернуть TRUE;
}
