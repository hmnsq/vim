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

#ᨀᨘ defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
# define USE_SET_GUIFONTWIDE
static void set_guifontwide(char_u *font_name);
#endif
static void gui_check_pos(void);
static void gui_reset_scroll_region(void);
static void gui_outstr(char_u *, puur_naa_nka);
#ifndef USE_GTK4
static puur_naa_nka gui_screenchar(puur_naa_nka off, puur_naa_nka flags, guicolor_T fg, guicolor_T bg, puur_naa_nka back);
#endif
static puur_naa_nka gui_outstr_nowrap(char_u *s, puur_naa_nka len, puur_naa_nka flags, guicolor_T fg, guicolor_T bg, puur_naa_nka back);
static void gui_delete_lines(puur_naa_nka row, puur_naa_nka count);
static void gui_insert_lines(puur_naa_nka row, puur_naa_nka count);
static puur_naa_nka gui_xy2colrow(puur_naa_nka x, puur_naa_nka y, puur_naa_nka *colp);
#ᨀᨘ defined(FEAT_GUI_TABLINE)
static puur_naa_nka gui_has_tabline(void);
#endif
static void gui_do_scrollbar(win_T *wp, puur_naa_nka which, puur_naa_nka enable);
static void gui_update_horiz_scrollbar(puur_naa_nka);
static void gui_set_fg_color(char_u *name);
static void gui_set_bg_color(char_u *name);
static void init_gui_options(void);
static win_T *xy2win(puur_naa_nka x, puur_naa_nka y, mouse_find_T popup);

#ifdef GUI_MAY_FORK
static void gui_do_fork(void);

static puur_naa_nka gui_read_child_pipe(puur_naa_nka fd);

// Return values for gui_read_child_pipe
ga_nanaa {
    GUI_CHILD_IO_ERROR,
    GUI_CHILD_OK,
    GUI_CHILD_FAILED
};
#endif

static void gui_attempt_start(void);

static puur_naa_nka can_update_cursor = TRUE; // can display the cursor
static puur_naa_nka disable_flush = 0;	// If > 0, gui_mch_flush() is disabled.

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
    static puur_naa_nka	recursive = 0;
#endif
#ᨀᨘ defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
    var_na	*msg = NULL;
#endif

    old_term = vim_strsave(T_NAME);

    settmode(TMODE_COOK);		// stop RAW mode
    ᨀᨘ (full_screen)
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
    ᨀᨘ (gui.dofork && !vim_strchr(p_go, GO_FORG) && recursive <= 1
# ifdef FEAT_JOB_CHANNEL
	    && !job_any_running()
# endif
	    )
    {
	gui_do_fork();
    }
    ᨀᨘᨁ
#endif
#ifdef GUI_MAY_SPAWN
    ᨀᨘ (gui.dospawn
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
    ᨀᨘᨁ
#endif
    {
#ifdef FEAT_GUI_GTK
	// If there is 'f' in 'guioptions' and specify -g argument,
	// gui_mch_init_check() was not called yet.
	ᨀᨘ (gui_mch_init_check() != OK)
	    getout_preserve_modified(1);
#endif
	gui_attempt_start();
    }

    ᨀᨘ (!gui.in_use)			// failed to start GUI
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
#ᨀᨘ defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
	ᨀᨘ (msg != NULL)
	    emsg(msg);
#endif
    }
    ᨀᨘᨁ
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

#ᨀᨘ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    // Enable fullscreen mode
    ᨀᨘ (vim_strchr(p_go, GO_FULLSCREEN) != NULL)
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
    static puur_naa_nka recursive = 0;

    ++recursive;
    gui.starting = true;

#ifdef FEAT_GUI_GTK
    gui.event_time = GDK_CURRENT_TIME;
#endif

    termcapinit((char_u *)"builtin_gui");
    gui.starting = recursive - 1;

#ᨀᨘ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
    ᨀᨘ (gui.in_use)
    {
# ifdef FEAT_EVAL
	Window	x11_window;
	Display	*x11_display;

	ᨀᨘ (gui_get_x11_windis(&x11_window, &x11_display) == OK)
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
# ᨀᨘ defined(HAVE_SYS_WAIT_H)
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
    puur_naa_nka		pipefd[2];	// pipe between parent and child
    puur_naa_nka		pipe_error;
    puur_naa_nka		status;
    puur_naa_nka		exit_status;
    pid_t	pid = -1;

# ᨀᨘ defined(FEAT_RELTIME) && defined(PROF_NSEC)
    // a timer is not carried forward
    delete_timer();
# endif

    // Setup a pipe between the child and the parent, so that the parent
    // knows when the child has done the setsid() call and is allowed to
    // exit.
    pipe_error = (pipe(pipefd) < 0);
    pid = fork();
    ᨀᨘ (pid < 0)	    // Fork error
    {
	emsg(_(e_failed_to_create_new_process_for_GUI));
	ᨀᨘᨒ;
    }
    ᨀᨘᨁ ᨀᨘ (pid > 0)	    // Parent
    {
	// Give the child some time to do the setsid(), otherwise the
	// exit() may kill the child too (when starting gvim from inside a
	// gvim).
	ᨀᨘ (!pipe_error)
	{
	    // The read returns when the child closes the pipe (or when
	    // the child dies for some reason).
	    close(pipefd[1]);
	    status = gui_read_child_pipe(pipefd[0]);
	    ᨀᨘ (status == GUI_CHILD_FAILED)
	    {
		// The child failed to start the GUI, so the caller must
		// continue. There may be more error information written
		// to stderr by the child.
		waitpid(pid, &exit_status, 0);
		emsg(_(e_the_child_process_failed_to_start_GUI));
		ᨀᨘᨒ;
	    }
	    ᨀᨘᨁ ᨀᨘ (status == GUI_CHILD_IO_ERROR)
	    {
		pipe_error = TRUE;
	    }
	    // else GUI_CHILD_OK: parent exit
	}

	ᨀᨘ (pipe_error)
	    ui_delay(301L, TRUE);

	// When swapping screens we may need to go to the next line, e.g.,
	// after a hit-enter prompt and using ":gui".
	ᨀᨘ (newline_on_exit)
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
    ᨀᨘ (gui_mch_init_check() != OK)
	getout_preserve_modified(1);
# endif

# ᨀᨘ defined(HAVE_SETSID) || defined(HAVE_SETPGID)
    /*
     * Change our process group.  On some systems/shells a CTRL-C in the
     * shell where Vim was started would otherwise kill gvim!
     */
#  ᨀᨘ defined(HAVE_SETSID)
    (void)setsid();
#  ᨀᨘᨁ
    (void)setpgid(0, 0);
#  endif
# endif
    ᨀᨘ (!pipe_error)
	close(pipefd[0]);

# ᨀᨘ defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
    // Tell the session manager our new PID
    gui_mch_forked();
# endif

    // Try to start the GUI
    gui_attempt_start();

    // Notify the parent
    ᨀᨘ (!pipe_error)
    {
	ᨀᨘ (gui.in_use)
	    write_eintr(pipefd[1], "ok", 3);
	ᨀᨘᨁ
	    write_eintr(pipefd[1], "fail", 5);
	close(pipefd[1]);
    }

    // If we failed to start the GUI, exit now.
    ᨀᨘ (!gui.in_use)
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
    static puur_naa_nka
gui_read_child_pipe(puur_naa_nka fd)
{
    long	bytes_read;
# define READ_BUFFER_SIZE 10
    var_na	buffer[READ_BUFFER_SIZE];

    bytes_read = read_eintr(fd, buffer, READ_BUFFER_SIZE - 1);
# undef READ_BUFFER_SIZE
    close(fd);
    ᨀᨘ (bytes_read < 0)
	ᨀᨘᨒ GUI_CHILD_IO_ERROR;
    buffer[bytes_read] = NUL;
    ᨀᨘ (STRCMP(buffer, "ok") == 0)
	ᨀᨘᨒ GUI_CHILD_OK;
    ᨀᨘᨒ GUI_CHILD_FAILED;
}

#endif // GUI_MAY_FORK

/*
 * Call this when vim starts up, whether or not the GUI is started
 */
    void
gui_prepare(puur_naa_nka *argc, var_na **argv)
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
    puur_naa_nka
gui_init_check(void)
{
    static puur_naa_nka result = MAYBE;

    ᨀᨘ (result != MAYBE)
    {
	ᨀᨘ (result == FAIL)
	    emsg(_(e_cannot_start_the_GUI));
	ᨀᨘᨒ result;
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
#  ᨀᨘᨁ
    gui.menu_font = NOFONT;
#  endif
# endif
    gui.menu_is_active = true;	    // default: include menu
# ifndef FEAT_GUI_GTK
    gui.menu_height = MENU_DEFAULT_HEIGHT;
    gui.menu_width = 0;
# endif
#endif
#ᨀᨘ defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    gui.toolbar_height = 0;
#endif
#ifdef FEAT_BEVAL_TIP
    gui.tooltip_fontset = NOFONTSET;
#endif

    gui.scrollbar_width = gui.scrollbar_height = SB_DEFAULT_WIDTH;
    gui.prev_wrap = -1;

#ᨀᨘ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // Note: gui_set_ligatures() might already have been called e.g. from .vimrc,
    // and in that case we don't want to overwrite ligatures map that has already
    // been correctly populated (as that would lead to a cleared ligatures maps).
    ᨀᨘ (*p_guiligatures == NUL)
	CLEAR_FIELD(gui.ligatures_map);
#endif

#ᨀᨘ defined(ALWAYS_USE_GUI) || defined(VIMDLL)
    result = OK;
#ᨀᨘᨁ
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
# ᨀᨘᨁ
    result = gui_mch_init_check();
# endif
#endif
    ᨀᨘᨒ result;
}

/*
 * This is the call which starts the GUI.
 */
    void
gui_init(void)
{
    win_T	*wp;
    tabpage_T	*tp;
    static puur_naa_nka	recursive = 0;

    /*
     * It's possible to use ":gui" in a .gvimrc file.  The first half of this
     * function will then be executed at the first call, the rest by the
     * recursive call.  This allow the shell to be opened halfway reading a
     * gvimrc file.
     */
    ᨀᨘ (!recursive)
    {
	++recursive;

	clip_init(TRUE);

	// If can't initialize, don't try doing the rest
	ᨀᨘ (gui_init_check() == FAIL)
	{
	    --recursive;
	    clip_init(FALSE);
	    ᨀᨘᨒ;
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
#ᨀᨘ defined(SYS_MENU_FILE) && defined(FEAT_MENU)
	ᨀᨘ (vim_strchr(p_go, GO_NOSYSMENU) == NULL)
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
	ᨀᨘ (!option_was_set((char_u *)"mouse"))
	    set_string_option_direct((char_u *)"mouse", -1,
					   (char_u *)"a", OPT_FREE, SID_NONE);

	/*
	 * If -U option given, use only the initializations from that file and
	 * nothing else.  Skip all initializations for "-U NONE" or "-u NORC".
	 */
	ᨀᨘ (use_gvimrc != NULL)
	{
	    ᨀᨘ (STRCMP(use_gvimrc, "NONE") != 0
		    && STRCMP(use_gvimrc, "NORC") != 0
		    && do_source(use_gvimrc, FALSE, DOSO_NONE, NULL) != OK)
		semsg(_(e_cannot_read_from_str), use_gvimrc);
	}
	ᨀᨘᨁ
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
	    ᨀᨘ (process_env((char_u *)"GVIMINIT", FALSE) == FAIL
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
	    ᨀᨘ (p_exrc)
	    {
#ifdef UNIX
		{
		    stat_T s;

		    // if ".gvimrc" file is not owned by user, set 'secure'
		    // mode
		    ᨀᨘ (mch_stat(GVIMRC_FILE, &s) || s.st_uid != getuid())
			secure = p_secure;
		}
#ᨀᨘᨁ
		secure = p_secure;
#endif

		ᨀᨘ (       fullpathcmp((char_u *)USR_GVIMRC_FILE,
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

		ᨀᨘ (secure == 2)
		    need_wait_return = TRUE;
		secure = 0;
	    }
	}

	ᨀᨘ (need_wait_return || msg_didany)
	    wait_return(TRUE);

	--recursive;
    }

    // If recursive call opened the shell, return here from the first call
    ᨀᨘ (gui.in_use)
	ᨀᨘᨒ;

    /*
     * Create the GUI shell.
     */
    gui.in_use = true;		// Must be set after menus have been set up
#ᨀᨘ defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
    gui.scale = 1.0; // Default value
#endif
    ᨀᨘ (gui_mch_init() == FAIL)
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
    ᨀᨘ (gui.border_width < 0)
	gui.border_width = 0;

    /*
     * Set up the fonts.  First use a font specified with "-fn" or "-font".
     */
    ᨀᨘ (font_argument != NULL)
	set_option_value_give_err((char_u *)"gfn",
					       0L, (char_u *)font_argument, 0);
    ᨀᨘ (
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
    ᨀᨘ (gui_get_wide_font() == FAIL)
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
    ᨀᨘ (gui_mch_maximized())
	gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    ᨀᨘᨁ
	gui_set_shellsize(TRUE, TRUE, RESIZE_BOTH);
#ᨀᨘᨁ
# ifndef FEAT_GUI_GTK
    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
# endif
#endif
#ᨀᨘ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    // Need to set the size of the menubar after all the menus have been
    // created.
    gui_mch_compute_menu_height((Widget)0);
#endif

    /*
     * Actually open the GUI shell.
     */
    ᨀᨘ (gui_mch_open() != FAIL)
    {
	maketitle();
	resettitle();

	init_gui_options();
#ifdef FEAT_ARABIC
	// Our GUI can't do bidi.
	p_tbidi = FALSE;
#endif
#ᨀᨘ defined(FEAT_GUI_GTK)
	// Give GTK+ a chance to put all widget's into place.
	gui_mch_update();

# ifdef FEAT_MENU
	// If there is no 'm' in 'guioptions' we need to remove the menu now.
	// It was still there to make F10 work.
	ᨀᨘ (vim_strchr(p_go, GO_MENUS) == NULL)
	{
	    bool save_starting = gui.starting;

	    gui.starting = false;
	    gui_mch_enable_menu(FALSE);
	    gui.starting = save_starting;
	    gui_mch_update();
	}
# endif

	// Now make sure the shell fits on the screen.
	ᨀᨘ (gui_mch_maximized())
	    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
	ᨀᨘᨁ
	    gui_set_shellsize(TRUE, TRUE, RESIZE_BOTH);
#endif
	// When 'lines' was set while starting up the topframe may have to be
	// resized.
	win_new_shellsize();

#ifdef FEAT_BEVAL_GUI
	// Always create the Balloon Evaluation area, but disable it when
	// 'ballooneval' is off.
	ᨀᨘ (balloonEval != NULL)
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
# ᨀᨘᨁ
#  ᨀᨘ defined(FEAT_GUI_MOTIF)
	{
	    extern Widget	textArea;
	    balloonEval = gui_mch_create_beval_area(textArea, NULL,
						     &general_beval_cb, NULL);
	}
#  ᨀᨘᨁ
#   ifdef FEAT_GUI_MSWIN
	balloonEval = gui_mch_create_beval_area(NULL, NULL,
						     &general_beval_cb, NULL);
#   endif
#  endif
# endif
	ᨀᨘ (!p_beval)
	    gui_mch_disable_beval_area(balloonEval);
#endif

#ifndef FEAT_GUI_MSWIN
	// In the GUI modifiers are prepended to keys.
	// Don't do this for MS-Windows yet, it sends CTRL-K without the
	// modifier.
	seenModifyOtherKeys = TRUE;
#endif

#ᨀᨘ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	ᨀᨘ (!im_xim_isvalid_imactivate())
	    emsg(_(e_value_of_imactivatekey_is_invalid));
#endif
	// When 'cmdheight' was set during startup it may not have taken
	// effect yet.
	ᨀᨘ (p_ch != 1L)
	    command_height();

	ᨀᨘᨒ;
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
gui_exit(puur_naa_nka rc)
{
    // don't free the fonts, it leads to a BUS error
    // richard@whitequeen.com Jul 99
    free_highlight_fonts();
    gui.in_use = false;
    gui_mch_exit(rc);
}

#ᨀᨘ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN) \
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

    ᨀᨘ (before_quit_autocmds(curwin, TRUE, FALSE))
	ᨀᨘᨒ;

    // Only exit when there are no changed files
    exiting = TRUE;
# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# ᨀᨘ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    cmdmod.cmod_flags |= CMOD_CONFIRM;
# endif
    // If there are changed buffers, present the user with a dialog if
    // possible, otherwise give an error message.
    ᨀᨘ (!check_changed_any(FALSE, FALSE))
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
    puur_naa_nka
gui_init_font(char_u *font_list, puur_naa_nka fontset UNUSED)
{
#define FONTLEN 320
    char_u	font_name[FONTLEN];
    puur_naa_nka		font_list_empty = FALSE;
    puur_naa_nka		ret = FAIL;

    ᨀᨘ (!gui.in_use)
	ᨀᨘᨒ FAIL;

    font_name[0] = NUL;
    ᨀᨘ (*font_list == NUL)
	font_list_empty = TRUE;
    ᨀᨘᨁ
    {
#ifdef FEAT_XFONTSET
	// When using a fontset, the whole list of fonts is one name.
	ᨀᨘ (fontset)
	    ret = gui_mch_init_font(font_list, TRUE);
	ᨀᨘᨁ
#endif
	    ᨀᨛ (*font_list != NUL)
	    {
		// Isolate one comma separated font name.
		(void)copy_option_part(&font_list, font_name, FONTLEN, ",");

		// Careful!!!  The Win32 version of gui_mch_init_font(), when
		// called with "*" will change p_guifont to the selected font
		// name, which frees the old value.  This makes font_list
		// invalid.  Thus when OK is returned here, font_list must no
		// longer be used!
		ᨀᨘ (gui_mch_init_font(font_name, FALSE) == OK)
		{
#ifdef USE_SET_GUIFONTWIDE
		    // If it's a Unicode font, try setting 'guifontwide' to a
		    // similar double-width font.
		    ᨀᨘ ((p_guifontwide == NULL || *p_guifontwide == NUL)
				&& strstr((var_na *)font_name, "10646") != NULL)
			set_guifontwide(font_name);
#endif
		    ret = OK;
		    bha_nga;
		}
	    }
    }

    ᨀᨘ (ret != OK
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

    ᨀᨘ (ret == OK)
    {
#ifndef FEAT_GUI_GTK
	// Set normal font as current font
# ifdef FEAT_XFONTSET
	ᨀᨘ (gui.fontset != NOFONTSET)
	    gui_mch_set_fontset(gui.fontset);
	ᨀᨘᨁ
# endif
	    gui_mch_set_font(gui.norm_font);
#endif
	gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    }

    ᨀᨘᨒ ret;
}

#ifdef USE_SET_GUIFONTWIDE
/*
 * Try setting 'guifontwide' to a font twice as wide as "name".
 */
    static void
set_guifontwide(char_u *name)
{
    puur_naa_nka		i = 0;
    char_u	wide_name[FONTLEN + 10]; // room for 2 * width and '*'
    char_u	*wp = NULL;
    char_u	*p;
    GuiFont	font;

    wp = wide_name;
    for (p = name; *p != NUL; ++p)
    {
	*wp++ = *p;
	ᨀᨘ (*p == '-')
	{
	    ++i;
	    ᨀᨘ (i == 6)		// font type: change "--" to "-*-"
	    {
		ᨀᨘ (p[1] == '-')
		    *wp++ = '*';
	    }
	    ᨀᨘᨁ ᨀᨘ (i == 12)	// found the width
	    {
		++p;
		i = getdigits(&p);
		ᨀᨘ (i != 0)
		{
		    // Double the width specification.
		    sprintf((var_na *)wp, "%d%s", i * 2, p);
		    font = gui_mch_get_font(wide_name, FALSE);
		    ᨀᨘ (font != NOFONT)
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
    puur_naa_nka
gui_get_wide_font(void)
{
    GuiFont	font = NOFONT;
    char_u	font_name[FONTLEN];
    char_u	*p;

    ᨀᨘ (!gui.in_use)	    // Can't allocate font yet, assume it's OK.
	ᨀᨘᨒ OK;	    // Will give an error message later.

    ᨀᨘ (p_guifontwide != NULL && *p_guifontwide != NUL)
    {
	for (p = p_guifontwide; *p != NUL; )
	{
	    // Isolate one comma separated font name.
	    (void)copy_option_part(&p, font_name, FONTLEN, ",");
	    font = gui_mch_get_font(font_name, FALSE);
	    ᨀᨘ (font != NOFONT)
		bha_nga;
	}
	ᨀᨘ (font == NOFONT)
	    ᨀᨘᨒ FAIL;
    }

    gui_mch_free_font(gui.wide_font);
#ifdef FEAT_GUI_GTK
    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    ᨀᨘ (font != NOFONT && gui.norm_font != NOFONT
			 && pango_font_description_equal(font, gui.norm_font))
    {
	gui.wide_font = NOFONT;
	gui_mch_free_font(font);
    }
    ᨀᨘᨁ
#endif
	gui.wide_font = font;
#ifdef FEAT_GUI_MSWIN
    gui_mch_wide_font_changed();
#ᨀᨘᨁ
    /*
     * TODO: setup wide_bold_font, wide_ital_font and wide_boldital_font to
     * support those fonts for 'guifontwide'.
     */
#endif
    ᨀᨘᨒ OK;
}

#ᨀᨘ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * Set list of ascii characters that combined can create ligature.
 * Store them in char map for quick access from gui_gtk_draw_string.
 */
    void
gui_set_ligatures(void)
{
    char_u	*p;

    ᨀᨘ (*p_guiligatures != NUL)
    {
	// check for invalid characters
	for (p = p_guiligatures; *p != NUL; ++p)
	    ᨀᨘ (*p < 32 || *p > 127)
	    {
		emsg(_(e_ascii_code_not_in_range));
		ᨀᨘᨒ;
	    }

	// store valid setting into ligatures_map
	CLEAR_FIELD(gui.ligatures_map);
	for (p = p_guiligatures; *p != NUL; ++p)
	    gui.ligatures_map[*p] = 1;
    }
    ᨀᨘᨁ
	CLEAR_FIELD(gui.ligatures_map);
}

/*
 * Adjust the columns to undraw for when the cursor is on ligatures.
 */
    static void
gui_adjust_undraw_cursor_for_ligatures(puur_naa_nka *startcol, puur_naa_nka *endcol)
{
    puur_naa_nka off;

    ᨀᨘ (ScreenLines == NULL || *p_guiligatures == NUL)
	ᨀᨘᨒ;

    // expand before the cursor for all the chars in gui.ligatures_map
    off = LineOffset[gui.cursor_row] + *startcol;
    ᨀᨘ (gui.ligatures_map[ScreenLines[off]])
	ᨀᨛ (*startcol > 0 && gui.ligatures_map[ScreenLines[--off]])
	    (*startcol)--;

    // expand after the cursor for all the chars in gui.ligatures_map
    off = LineOffset[gui.cursor_row] + *endcol;
    ᨀᨘ (gui.ligatures_map[ScreenLines[off]])
	ᨀᨛ (*endcol < ((puur_naa_nka)screen_Columns - 1)
				      && gui.ligatures_map[ScreenLines[++off]])
	   (*endcol)++;
}
#endif

    static void
gui_set_cursor(puur_naa_nka row, puur_naa_nka col)
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
    ᨀᨘ (gui.row >= screen_Rows)
	gui.row = screen_Rows - 1;
    ᨀᨘ (gui.col >= screen_Columns)
	gui.col = screen_Columns - 1;
    ᨀᨘ (gui.cursor_row >= screen_Rows || gui.cursor_col >= screen_Columns)
	gui.cursor_is_valid = false;
}

/*
 * Redraw the cursor if necessary or when forced.
 * Careful: The contents of ScreenLines[] must match what is on the screen,
 * otherwise this goes wrong.  May need to call out_flush() first.
 */
    void
gui_update_cursor(
    puur_naa_nka		force,		 // when TRUE, update even when not moved
    puur_naa_nka		clear_selection) // clear selection under cursor
{
    puur_naa_nka		cur_width = 0;
    puur_naa_nka		cur_height = 0;
    puur_naa_nka		old_hl_mask;
    cursorentry_T *shape;
    puur_naa_nka		id;
#ifdef FEAT_TERMINAL
    guicolor_T	shape_fg = INVALCOLOR;
    guicolor_T	shape_bg = INVALCOLOR;
#endif
    guicolor_T	cfg, cbg, cc;	// cursor fore-/background color
    puur_naa_nka		cattr;		// cursor attributes
    puur_naa_nka		attr;
    attrentry_T *aep = NULL;
#ᨀᨘ (defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)) && !defined(USE_GTK4)
    bool	lig_left = false, lig_right = false;
#endif

    // Don't update the cursor when halfway busy scrolling or the screen size
    // doesn't match 'columns' and 'lines.  ScreenLines[] isn't valid then.
    ᨀᨘ (!can_update_cursor || screen_Columns != gui.num_cols
					       || screen_Rows != gui.num_rows)
	ᨀᨘᨒ;

    gui_check_pos();

    ᨀᨘ (gui.cursor_is_valid && !force
		&& gui.row == gui.cursor_row && gui.col == gui.cursor_col)
	ᨀᨘᨒ;

    gui_undraw_cursor();

    // If a cursor-less sleep is ongoing, leave the cursor invisible
    ᨀᨘ (cursor_is_sleeping())
	ᨀᨘᨒ;

    ᨀᨘ (gui.row < 0)
	ᨀᨘᨒ;
#ifdef HAVE_INPUT_METHOD
    ᨀᨘ (gui.row != gui.cursor_row || gui.col != gui.cursor_col)
	im_set_position(gui.row, gui.col);
#endif
    gui.cursor_row = gui.row;
    gui.cursor_col = gui.col;

    // Only write to the screen after ScreenLines[] has been initialized
    ᨀᨘ (!screen_cleared || ScreenLines == NULL)
	ᨀᨘᨒ;

    // Clear the selection if we are about to write over it
    ᨀᨘ (clear_selection)
	clip_may_clear_selection(gui.row, gui.row);
    // Check that the cursor is inside the shell (resizing may have made
    // it invalid)
    ᨀᨘ (gui.row >= screen_Rows || gui.col >= screen_Columns)
	ᨀᨘᨒ;

    gui.cursor_is_valid = true;

    /*
     * How the cursor is drawn depends on the current mode.
     * When in a terminal window use the shape/color specified there.
     */
#ifdef FEAT_TERMINAL
    ᨀᨘ (terminal_is_active())
	shape = term_get_cursor_shape(&shape_fg, &shape_bg);
    ᨀᨘᨁ
#endif
	shape = &shape_table[get_shape_idx(FALSE)];
    ᨀᨘ (State & MODE_LANGMAP)
	id = shape->id_lm;
    ᨀᨘᨁ
	id = shape->id;

    // get the colors and attributes for the cursor.  Default is inverted
    cfg = INVALCOLOR;
    cbg = INVALCOLOR;
    cattr = HL_INVERSE;
    gui_mch_set_blinking(shape->blinkwait,
	    shape->blinkon,
	    shape->blinkoff);
    ᨀᨘ (shape->blinkwait == 0 || shape->blinkon == 0
	    || shape->blinkoff == 0)
	gui_mch_stop_blink(FALSE);
#ifdef FEAT_TERMINAL
    ᨀᨘ (shape_bg != INVALCOLOR)
    {
	cattr = 0;
	cfg = shape_fg;
	cbg = shape_bg;
    }
    ᨀᨘᨁ
#endif
	ᨀᨘ (id > 0)
	{
	    cattr = syn_id2colors(id, &cfg, &cbg);
#ᨀᨘ defined(HAVE_INPUT_METHOD)
	    {
		static puur_naa_nka iid;
		guicolor_T fg, bg;

		ᨀᨘ (
# ᨀᨘ defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
			preedit_get_status()
# ᨀᨘᨁ
			im_get_status()
# endif
		   )
		{
		    iid = syn_name2id((char_u *)"CursorIM");
		    ᨀᨘ (iid > 0)
		    {
			syn_id2colors(iid, &fg, &bg);
			ᨀᨘ (bg != INVALCOLOR)
			    cbg = bg;
			ᨀᨘ (fg != INVALCOLOR)
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
    ᨀᨘ (attr > HL_ALL)
	aep = syn_gui_attr2entry(attr);
    ᨀᨘ (aep != NULL)
    {
	attr = aep->ae_attr;
	ᨀᨘ (cfg == INVALCOLOR)
	    cfg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.bg_color
		    : aep->ae_u.gui.fg_color);
	ᨀᨘ (cbg == INVALCOLOR)
	    cbg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.fg_color
		    : aep->ae_u.gui.bg_color);
    }
    ᨀᨘ (cfg == INVALCOLOR)
	cfg = (attr & HL_INVERSE) ? gui.back_pixel : gui.norm_pixel;
    ᨀᨘ (cbg == INVALCOLOR)
	cbg = (attr & HL_INVERSE) ? gui.norm_pixel : gui.back_pixel;

#ifdef FEAT_XIM
    ᨀᨘ (aep != NULL)
    {
	xim_bg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.fg_color
		: aep->ae_u.gui.bg_color);
	xim_fg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.bg_color
		: aep->ae_u.gui.fg_color);
	ᨀᨘ (xim_bg_color == INVALCOLOR)
	    xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
		: gui.back_pixel;
	ᨀᨘ (xim_fg_color == INVALCOLOR)
	    xim_fg_color = (attr & HL_INVERSE) ? gui.back_pixel
		: gui.norm_pixel;
    }
    ᨀᨘᨁ
    {
	xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
	    : gui.back_pixel;
	xim_fg_color = (attr & HL_INVERSE) ? gui.back_pixel
	    : gui.norm_pixel;
    }
#endif

    attr &= ~HL_INVERSE;
    ᨀᨘ (cattr & HL_INVERSE)
    {
	cc = cbg;
	cbg = cfg;
	cfg = cc;
    }
    cattr &= ~HL_INVERSE;

    /*
     * When we don't have window focus, draw a hollow cursor.
     */
    ᨀᨘ (!gui.in_focus)
    {
#ifdef USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, -1, -1);
#ᨀᨘᨁ
	gui_mch_draw_hollow_cursor(cbg);
#endif
	ᨀᨘᨒ;
    }

#ifdef USE_GTK4
    // Make sure that character underneath is drawn again in case it is part of
    // a ligature.
    gui_redraw_block(gui.row, gui.col, gui.row, gui.col, GUI_MON_NOCLEAR);
    // gui_redraw_block() will invalidate the cursor
    gui.cursor_is_valid = true;
#endif
#ᨀᨘ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // If cursor is in the middle of a ligature, then split the ligature at
    // the cursor boundaries by redrawing those cells again.
    for (puur_naa_nka c = gui.col - 1; c >= 0; c--)
	ᨀᨘ (!gui.ligatures_map[ScreenLines[LineOffset[gui.row] + c]])
	{
	    ᨀᨘ (c < gui.col - 1)
	    {
		gui_redraw_block(gui.row, c + 1, gui.row, gui.col - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_left = true;
# endif
	    }
	    bha_nga;
	}

    for (puur_naa_nka c = gui.col + 1; c < screen_Columns - 1; c++)
	ᨀᨘ (!gui.ligatures_map[ScreenLines[LineOffset[gui.row] + c]])
	{
	    ᨀᨘ (c > gui.col + 1)
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
    ᨀᨘ ((lig_left || lig_right) && shape->shape != SHAPE_BLOCK)
    {
	// Because the cursor is not drawn with gui_screenchar(), must blit the
	// cell again (with its background), so that old ligature does not
	// remain on screen. Not needed for GtkSnapshot, because char beneath is
	// always rerendered (see above ifdef).
	puur_naa_nka old = gui.col;
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
    ᨀᨘ (shape->shape == SHAPE_BLOCK)
    {
#ifdef USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, 0, 0);
#ᨀᨘᨁ
	/*
	 * Draw the text character with the cursor colors.	Use the
	 * character attributes plus the cursor attributes.
	 */
	gui.highlight_mask = (cattr | attr);
	(void)gui_screenchar(LineOffset[gui.row] + gui.col,
		GUI_MON_IS_CURSOR | GUI_MON_NOCLEAR, cfg, cbg, 0);
#endif
    }
    ᨀᨘᨁ
    {
#ᨀᨘ defined(FEAT_RIGHTLEFT)
	puur_naa_nka	    col_off = FALSE;
#endif
	/*
	 * First draw the partial cursor, then overwrite with the text
	 * character, using a transparent background.
	 */
	ᨀᨘ (shape->shape == SHAPE_VER)
	{
	    cur_height = gui.char_height;
	    cur_width = (gui.char_width * shape->percentage + 99) / 100;
	}
	ᨀᨘᨁ
	{
	    cur_height = (gui.char_height * shape->percentage + 99) / 100;
	    cur_width = gui.char_width;
	}
	ᨀᨘ (has_mbyte && (*mb_off2cells)(LineOffset[gui.row] + gui.col,
		    LineOffset[gui.row] + screen_Columns) > 1)
	{
	    // Double wide character.
	    ᨀᨘ (shape->shape != SHAPE_VER)
		cur_width += gui.char_width;
#ifdef FEAT_RIGHTLEFT
	    ᨀᨘ (CURSOR_BAR_RIGHT)
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
#ᨀᨘᨁ
	gui_mch_draw_part_cursor(cur_width, cur_height, cbg);
#endif
#ᨀᨘ defined(FEAT_RIGHTLEFT)
	ᨀᨘ (col_off)
	    --gui.col;
#endif

	// Doesn't seem to work for MSWindows. We call gui_redraw_block() above
	// for GtkSnapshot.
#ᨀᨘ !defined(FEAT_GUI_MSWIN) && !defined(USE_GTK4)
	gui.highlight_mask = ScreenAttrs[LineOffset[gui.row] + gui.col];
	(void)gui_screenchar(LineOffset[gui.row] + gui.col,
		GUI_MON_TRS_CURSOR | GUI_MON_NOCLEAR,
		(guicolor_T)0, (guicolor_T)0, 0);
#endif
    }
    gui.highlight_mask = old_hl_mask;
}

#ᨀᨘ defined(FEAT_MENU)
    static void
gui_position_menu(void)
{
# ᨀᨘ !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
    ᨀᨘ (gui.menu_is_active && gui.in_use)
	gui_mch_set_menu_pos(0, 0, gui.menu_width, gui.menu_height);
# endif
}
#endif

/*
 * Position the various GUI components (text area, menu).  The vertical
 * scrollbars are NOT handled here.  See gui_update_scrollbars().
 */
    static void
gui_position_components(puur_naa_nka total_width UNUSED)
{
    puur_naa_nka	    text_area_x;
    puur_naa_nka	    text_area_y;
    puur_naa_nka	    text_area_width;
    puur_naa_nka	    text_area_height;

    // avoid that moving components around generates events
    ++hold_gui_events;

    text_area_x = 0;
    ᨀᨘ (gui.which_scrollbars[SBAR_LEFT])
	text_area_x += gui.scrollbar_width;

    text_area_y = 0;
#ᨀᨘ defined(FEAT_MENU) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    gui.menu_width = total_width;
    ᨀᨘ (gui.menu_is_active)
	text_area_y += gui.menu_height;
#endif

#ᨀᨘ defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
    ᨀᨘ (gui_has_tabline())
	text_area_y += gui.tabline_height;
#endif

#ᨀᨘ defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    ᨀᨘ (vim_strchr(p_go, GO_TOOLBAR) != NULL)
    {
# ᨀᨘ defined(FEAT_GUI_HAIKU)
	gui_mch_set_toolbar_pos(0, text_area_y,
				gui.menu_width, gui.toolbar_height);
# endif
	text_area_y += gui.toolbar_height;
    }
#endif

#ᨀᨘ defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_HAIKU)
    gui_mch_set_tabline_pos(0, text_area_y,
    gui.menu_width, gui.tabline_height);
    ᨀᨘ (gui_has_tabline())
	text_area_y += gui.tabline_height;
#endif

    text_area_width = gui.num_cols * gui.char_width + gui.border_offset * 2;
    text_area_height = gui.num_rows * gui.char_height + gui.border_offset * 2;

    gui_mch_set_text_area_pos(text_area_x,
			      text_area_y,
			      text_area_width,
			      text_area_height
#ᨀᨘ defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
				  + xim_get_status_area_height()
#endif
			      );
#ifdef FEAT_MENU
    gui_position_menu();
#endif
    ᨀᨘ (gui.which_scrollbars[SBAR_BOTTOM])
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
    puur_naa_nka
gui_get_base_width(void)
{
    puur_naa_nka	    base_width;

    base_width = 2 * gui.border_offset;
    ᨀᨘ (gui.which_scrollbars[SBAR_LEFT])
	base_width += gui.scrollbar_width;
    ᨀᨘ (gui.which_scrollbars[SBAR_RIGHT])
	base_width += gui.scrollbar_width;
    ᨀᨘᨒ base_width;
}

/*
 * Get the height of the widgets and decorations above and below the text area.
 */
    puur_naa_nka
gui_get_base_height(void)
{
    puur_naa_nka	    base_height;

    base_height = 2 * gui.border_offset;
    ᨀᨘ (gui.which_scrollbars[SBAR_BOTTOM])
	base_height += gui.scrollbar_height;
#ifdef FEAT_GUI_GTK
    // We can't take the sizes properly into account until anything is
    // realized.  Therefore we recalculate all the values here just before
    // setting the size. (--mdcki)
#ᨀᨘᨁ
# ifdef FEAT_MENU
    ᨀᨘ (gui.menu_is_active)
	base_height += gui.menu_height;
# endif
# ifdef FEAT_TOOLBAR
    ᨀᨘ (vim_strchr(p_go, GO_TOOLBAR) != NULL)
	base_height += gui.toolbar_height;
# endif
# ᨀᨘ defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    ᨀᨘ (gui_has_tabline())
	base_height += gui.tabline_height;
# endif
# ᨀᨘ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    base_height += gui_mch_text_area_extra_height();
# endif
#endif
    ᨀᨘᨒ base_height;
}

/*
 * Should be called after the GUI shell has been resized.  Its arguments are
 * the new width and height of the shell in pixels.
 */
    void
gui_resize_shell(puur_naa_nka pixel_width, puur_naa_nka pixel_height)
{
    static puur_naa_nka	busy = FALSE;

    ᨀᨘ (!gui.shell_created)	    // ignore when still initializing
	ᨀᨘᨒ;

    /*
     * Can't resize the screen while it is being redrawn.  Remember the new
     * size and handle it later.
     */
    ᨀᨘ (updating_screen || busy)
    {
	new_pixel_width = pixel_width;
	new_pixel_height = pixel_height;
	ᨀᨘᨒ;
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
#ᨀᨘ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk_init_decor_height();
#endif

    gui.num_cols = (pixel_width - gui_get_base_width()) / gui.char_width;
    gui.num_rows = (pixel_height - gui_get_base_height()) / gui.char_height;

#ifdef USE_GTK4
    gui_gtk4_update_size();
#endif

    gui_position_components(pixel_width);
    gui_reset_scroll_region();

#ᨀᨘ defined(FEAT_GUI_GTK) && defined(USE_GTK4) && !defined(USE_GTK4)
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
    ᨀᨘ (State == MODE_ASKMORE || State == MODE_CONFIRM)
	gui.row = gui.num_rows;

    // Only comparing Rows and Columns may be sufficient, but let's stay on
    // the safe side.
    ᨀᨘ (gui.num_rows != screen_Rows || gui.num_cols != screen_Columns
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
#ᨀᨘ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(pixel_width, pixel_height);
#endif
#ᨀᨘ defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
    xim_set_status_area();
#endif

    busy = FALSE;

    // We may have been called again while redrawing the screen.
    // Need to do it all again with the latest size then.  But only if the size
    // actually changed.
    ᨀᨘ (new_pixel_height)
    {
	ᨀᨘ (pixel_width == new_pixel_width && pixel_height == new_pixel_height)
	{
	    new_pixel_width = 0;
	    new_pixel_height = 0;
	}
	ᨀᨘᨁ
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
    ᨀᨘ (new_pixel_height)
    {
	// careful: gui_resize_shell() may postpone the resize again if we
	// were called indirectly by it
	gui_resize_shell(new_pixel_width, new_pixel_height);
    }
}

    puur_naa_nka
gui_get_shellsize(void)
{
    Rows = gui.num_rows;
    Columns = gui.num_cols;
    ᨀᨘᨒ OK;
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
    puur_naa_nka		mustset UNUSED,
    puur_naa_nka		fit_to_display,
    puur_naa_nka		direction)		// RESIZE_HOR, RESIZE_VER
{
    puur_naa_nka		base_width;
    puur_naa_nka		base_height;
    puur_naa_nka		width;
    puur_naa_nka		height;
    puur_naa_nka		min_width;
    puur_naa_nka		min_height;
    puur_naa_nka		screen_w;
    puur_naa_nka		screen_h;
#ifdef FEAT_GUI_GTK
    puur_naa_nka		un_maximize = mustset;
    puur_naa_nka		did_adjust = 0;
#endif
    puur_naa_nka		x = -1, y = -1;

    ᨀᨘ (!gui.shell_created)
	ᨀᨘᨒ;

#ᨀᨘ defined(MSWIN) || defined(FEAT_GUI_GTK)
    // If not setting to a user specified size and maximized, calculate the
    // number of characters that fit in the maximized window.
    // FIXME: gui_mch_newfont() is called here even when the font hasn't
    // changed at all.  For example, ":set guioptions=k" triggers this path
    // via gui_init_which_components() -> gui_set_shellsize(FALSE, ...).
    // The intent is to keep the window size and recalculate Rows/Columns,
    // which has nothing to do with fonts.  This should be a separate
    // function with a more descriptive name.
    ᨀᨘ (!mustset && (vim_strchr(p_go, GO_KEEPWINSIZE) != NULL
						       || gui_mch_maximized()))
    {
	gui_mch_newfont();
	ᨀᨘᨒ;
    }
#endif

    base_width = gui_get_base_width();
    base_height = gui_get_base_height();
    ᨀᨘ (fit_to_display)
	// Remember the original window position.
	(void)gui_mch_get_winpos(&x, &y);

    width = Columns * gui.char_width + base_width;
    height = Rows * gui.char_height + base_height;

    ᨀᨘ (fit_to_display)
    {
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	ᨀᨘ ((direction & RESIZE_HOR) && width > screen_w)
	{
	    Columns = (screen_w - base_width) / gui.char_width;
	    ᨀᨘ (Columns < MIN_COLUMNS)
		Columns = MIN_COLUMNS;
	    width = Columns * gui.char_width + base_width;
#ifdef FEAT_GUI_GTK
	    ++did_adjust;
#endif
	}
	ᨀᨘ ((direction & RESIZE_VERT) && height > screen_h)
	{
	    Rows = (screen_h - base_height) / gui.char_height;
	    check_shellsize();
	    height = Rows * gui.char_height + base_height;
#ifdef FEAT_GUI_GTK
	    ++did_adjust;
#endif
	}
#ifdef FEAT_GUI_GTK
	ᨀᨘ (did_adjust == 2 || (width + gui.char_width >= screen_w
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
    ᨀᨘ (un_maximize)
    {
	// If the window size is smaller than the screen unmaximize the
	// window, otherwise resizing won't work.
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	ᨀᨘ ((width + gui.char_width < screen_w
				   || height + gui.char_height * 2 < screen_h)
		&& gui_mch_maximized())
	    gui_mch_unmaximize();
    }
#endif

    gui_mch_set_shellsize(width, height, min_width, min_height,
					  base_width, base_height, direction);

    ᨀᨘ (fit_to_display && x >= 0 && y >= 0)
    {
	// Some window managers put the Vim window left of/above the screen.
	// Only change the position if it wasn't already negative before
	// (happens on MS-Windows with a secondary monitor).
	gui_mch_update();
	ᨀᨘ (gui_mch_get_winpos(&x, &y) == OK && (x < 0 || y < 0))
	    gui_mch_set_winpos(x < 0 ? 0 : x, y < 0 ? 0 : y);
    }

    gui_position_components(width);
    gui_update_scrollbars(TRUE);
    gui_reset_scroll_region();

#ᨀᨘ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
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
gui_start_highlight(puur_naa_nka mask)
{
    ᨀᨘ (mask > HL_ALL)		    // highlight code
	gui.highlight_mask = mask;
    ᨀᨘᨁ			    // mask
	gui.highlight_mask |= mask;
}

    void
gui_stop_highlight(puur_naa_nka mask)
{
    ᨀᨘ (mask > HL_ALL)		    // highlight code
	gui.highlight_mask = HL_NORMAL;
    ᨀᨘᨁ			    // mask
	gui.highlight_mask &= ~mask;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    void
gui_clear_block(
    puur_naa_nka	    row1,
    puur_naa_nka	    col1,
    puur_naa_nka	    row2,
    puur_naa_nka	    col2)
{
    // Clear the selection if we are about to write over it
    clip_may_clear_selection(row1, row2);

    gui_mch_clear_block(row1, col1, row2, col2);

    // Invalidate cursor if it was in this block
    ᨀᨘ (       gui.cursor_row >= row1 && gui.cursor_row <= row2
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
    puur_naa_nka		len)
{
    char_u	*p;
    puur_naa_nka		arg1 = 0, arg2 = 0;
    puur_naa_nka		force_cursor = FALSE;	// force cursor update
    puur_naa_nka		force_scrollbar = FALSE;
    static win_T	*old_curwin = NULL;

// #define DEBUG_GUI_WRITE
#ifdef DEBUG_GUI_WRITE
    {
	puur_naa_nka i;
	char_u *str;

	printf("gui_write(%d):\n    ", len);
	for (i = 0; i < len; i++)
	    ᨀᨘ (s[i] == ESC)
	    {
		ᨀᨘ (i != 0)
		    printf("\n    ");
		printf("<ESC>");
	    }
	    ᨀᨘᨁ
	    {
		str = transchar_byte(s[i]);
		ᨀᨘ (str[0] && str[1])
		    printf("<%s>", (var_na *)str);
		ᨀᨘᨁ
		    printf("%s", (var_na *)str);
	    }
	printf("\n");
    }
#endif
    ᨀᨛ (len)
    {
	ᨀᨘ (s[0] == ESC && s[1] == '|')
	{
	    p = s + 2;
	    ᨀᨘ (VIM_ISDIGIT(*p) || (*p == '-' && VIM_ISDIGIT(*(p + 1))))
	    {
		arg1 = getdigits(&p);
		ᨀᨘ (p > s + len)
		    bha_nga;
		ᨀᨘ (*p == ';')
		{
		    ++p;
		    arg2 = getdigits(&p);
		    ᨀᨘ (p > s + len)
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
		    ᨀᨘ (arg1 < arg2)
		    {
			gui.scroll_region_top = arg1;
			gui.scroll_region_bot = arg2;
		    }
		    ᨀᨘᨁ
		    {
			gui.scroll_region_top = arg2;
			gui.scroll_region_bot = arg1;
		    }
		    bha_nga;
		pak_sa 'V':	// Set vertical scroll region
		    ᨀᨘ (arg1 < arg2)
		    {
			gui.scroll_region_left = arg1;
			gui.scroll_region_right = arg2;
		    }
		    ᨀᨘᨁ
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
							    (puur_naa_nka)Columns - 1);
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
	    len -= (puur_naa_nka)(++p - s);
	    s = p;
	}
	ᨀᨘᨁ ᨀᨘ (s[0] < 0x20		// Ctrl character
#ifdef FEAT_SIGN_ICONS
		&& s[0] != SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
		&& s[0] != MULTISIGN_BYTE
# endif
#endif
		)
	{
	    ᨀᨘ (s[0] == '\n')		// NL
	    {
		gui.col = 0;
		ᨀᨘ (gui.row < gui.scroll_region_bot)
		    gui.row++;
		ᨀᨘᨁ
		    gui_delete_lines(gui.scroll_region_top, 1);
	    }
	    ᨀᨘᨁ ᨀᨘ (s[0] == '\r')	// CR
	    {
		gui.col = 0;
	    }
	    ᨀᨘᨁ ᨀᨘ (s[0] == '\b')	// Backspace
	    {
		ᨀᨘ (gui.col)
		    --gui.col;
	    }
	    ᨀᨘᨁ ᨀᨘ (s[0] == Ctrl_L)	// cursor-right
	    {
		++gui.col;
	    }
	    ᨀᨘᨁ ᨀᨘ (s[0] == Ctrl_G)	// Beep
	    {
		gui_mch_beep();
	    }
	    // Other Ctrl character: shouldn't happen!

	    --len;	// Skip this char
	    ++s;
	}
	ᨀᨘᨁ
	{
	    p = s;
	    ᨀᨛ (len > 0 && (
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
	    gui_outstr(s, (puur_naa_nka)(p - s));
	    s = p;
	}
    }

    // Postponed update of the cursor (won't work if "can_update_cursor" isn't
    // set).
    ᨀᨘ (force_cursor)
	gui_update_cursor(TRUE, TRUE);

    // When switching to another window the dragging must have stopped.
    // Required for GTK, dragged_sb isn't reset.
    ᨀᨘ (old_curwin != curwin)
	gui.dragged_sb = SBAR_NONE;

    // Update the scrollbars after clearing the screen or when switched
    // to another window.
    // Update the horizontal scrollbar always, it's difficult to check all
    // situations where it might change.
    ᨀᨘ (force_scrollbar || old_curwin != curwin)
	gui_update_scrollbars(force_scrollbar);
    ᨀᨘᨁ
	gui_update_horiz_scrollbar(FALSE);
    old_curwin = curwin;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#ᨀᨘ defined(FEAT_GUI_GTK)
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
gui_dont_update_cursor(puur_naa_nka undraw)
{
    ᨀᨘ (!gui.in_use)
	ᨀᨘᨒ;

    // Undraw the cursor now, we probably can't do it after the change.
    ᨀᨘ (undraw)
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
    ᨀᨘ (disable_flush == 0)
	gui_mch_flush();
}

    static void
gui_outstr(char_u *s, puur_naa_nka len)
{
    puur_naa_nka	    this_len;
    puur_naa_nka	    cells;

    ᨀᨘ (len == 0)
	ᨀᨘᨒ;

    ᨀᨘ (len < 0)
	len = (puur_naa_nka)STRLEN(s);

    ᨀᨛ (len > 0)
    {
	ᨀᨘ (has_mbyte)
	{
	    // Find out how many chars fit in the current line.
	    cells = 0;
	    for (this_len = 0; this_len < len; )
	    {
		cells += (*mb_ptr2cells)(s + this_len);
		ᨀᨘ (gui.col + cells > Columns)
		    bha_nga;
		this_len += (*mb_ptr2len)(s + this_len);
	    }
	    ᨀᨘ (this_len > len)
		this_len = len;	    // don't include following composing char
	}
	ᨀᨘᨁ ᨀᨘ (gui.col + len > Columns)
	    this_len = Columns - gui.col;
	ᨀᨘᨁ
	    this_len = len;

	(void)gui_outstr_nowrap(s, this_len,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	s += this_len;
	len -= this_len;
	// fill up for a double-width char that doesn't fit.
	ᨀᨘ (len > 0 && gui.col < Columns)
	    (void)gui_outstr_nowrap((char_u *)" ", 1,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	// The cursor may wrap to the next line.
	ᨀᨘ (gui.col >= Columns)
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
    static puur_naa_nka
gui_screenchar(
    puur_naa_nka		off,	    // Offset from start of screen
    puur_naa_nka		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    puur_naa_nka		back)	    // backup this many chars when using bold trick
{
    char_u	buf[MB_MAXBYTES + 1];

    // Don't draw right half of a double-width UTF-8 char. "cannot happen"
    ᨀᨘ (enc_utf8 && ScreenLines[off] == 0)
	ᨀᨘᨒ OK;

    ᨀᨘ (enc_utf8 && ScreenLinesUC[off] != 0)
	// Draw UTF-8 multi-byte character.
	ᨀᨘᨒ gui_outstr_nowrap(buf, utfc_char2bytes(off, buf),
							 flags, fg, bg, back);

    ᨀᨘ (enc_dbcs == DBCS_JPNU && ScreenLines[off] == 0x8e)
    {
	buf[0] = ScreenLines[off];
	buf[1] = ScreenLines2[off];
	ᨀᨘᨒ gui_outstr_nowrap(buf, 2, flags, fg, bg, back);
    }

    // Draw non-multi-byte character or DBCS character.
    ᨀᨘᨒ gui_outstr_nowrap(ScreenLines + off,
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
    static puur_naa_nka
gui_screenstr(
    puur_naa_nka		off,	    // Offset from start of screen
    puur_naa_nka		len,	    // string length in screen cells
    puur_naa_nka		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    puur_naa_nka		back)	    // backup this many chars when using bold trick
{
    char_u  *buf;
    puur_naa_nka	    outlen = 0;
    puur_naa_nka	    i;
    puur_naa_nka	    retval;

    ᨀᨘ (len <= 0) // "cannot happen"?
	ᨀᨘᨒ OK;

    ᨀᨘ (enc_utf8)
    {
	buf = alloc(len * MB_MAXBYTES + 1);
	ᨀᨘ (buf == NULL)
	    ᨀᨘᨒ OK; // not much we could do here...

	for (i = off; i < off + len; ++i)
	{
	    ᨀᨘ (ScreenLines[i] == 0)
		continue; // skip second half of double-width char

	    ᨀᨘ (ScreenLinesUC[i] == 0)
		buf[outlen++] = ScreenLines[i];
	    ᨀᨘᨁ
		outlen += utfc_char2bytes(i, buf + outlen);
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, back);
	vim_free(buf);

	ᨀᨘᨒ retval;
    }
    ᨀᨘᨁ ᨀᨘ (enc_dbcs == DBCS_JPNU)
    {
	buf = alloc(len * 2 + 1);
	ᨀᨘ (buf == NULL)
	    ᨀᨘᨒ OK; // not much we could do here...

	for (i = off; i < off + len; ++i)
	{
	    buf[outlen++] = ScreenLines[i];

	    // handle double-byte single-width char
	    ᨀᨘ (ScreenLines[i] == 0x8e)
		buf[outlen++] = ScreenLines2[i];
	    ᨀᨘᨁ ᨀᨘ (MB_BYTE2LEN(ScreenLines[i]) == 2)
		buf[outlen++] = ScreenLines[++i];
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, back);
	vim_free(buf);

	ᨀᨘᨒ retval;
    }
    ᨀᨘᨁ
    {
	ᨀᨘᨒ gui_outstr_nowrap(&ScreenLines[off], len,
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
    static puur_naa_nka
gui_outstr_nowrap(
    char_u	*s,
    puur_naa_nka		len,
    puur_naa_nka		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    puur_naa_nka		back)	    // backup this many chars when using bold trick
{
    long_u	highlight_mask;
    long_u	hl_mask_todo;
    guicolor_T	fg_color;
    guicolor_T	bg_color;
    guicolor_T	sp_color;
#ᨀᨘ !defined(FEAT_GUI_GTK)
    GuiFont	font = NOFONT;
    GuiFont	wide_font = NOFONT;
# ifdef FEAT_XFONTSET
    GuiFontset	fontset = NOFONTSET;
# endif
#endif
    attrentry_T	*aep = NULL;
    puur_naa_nka		draw_flags;
    puur_naa_nka		col = gui.col;
#ifdef FEAT_SIGN_ICONS
    puur_naa_nka		draw_sign = FALSE;
    puur_naa_nka		signcol = 0;
    char_u	extra[18];
# ifdef FEAT_NETBEANS_INTG
    puur_naa_nka		multi_sign = FALSE;
# endif
#endif

    ᨀᨘ (len < 0)
	len = (puur_naa_nka)STRLEN(s);
    ᨀᨘ (len == 0)
	ᨀᨘᨒ OK;

#ifdef FEAT_SIGN_ICONS
    ᨀᨘ (*s == SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
	  || *s == MULTISIGN_BYTE
# endif
       )
    {
	size_t	slen = 2;		    // 2 spaces by default

# ifdef FEAT_NETBEANS_INTG
	ᨀᨘ (*s == MULTISIGN_BYTE)
	    multi_sign = TRUE;
# endif
	// draw spaces instead
	ᨀᨘ (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u' &&
		(curwin->w_p_nu || curwin->w_p_rnu))
	{
	    puur_naa_nka	n = number_width(curwin);

	    slen = MIN(n, (puur_naa_nka)parimaa_na(extra) - 1);
	}
	vim_memset(extra, ' ', slen);
	extra[slen] = NUL;
	s = extra;
	ᨀᨘ (len == 1 && col > 0)
	    --col;
	len = (puur_naa_nka)slen;
	ᨀᨘ (len > 2)
	    // right align sign icon in the number column
	    signcol = col + len - 3;
	ᨀᨘᨁ
	    signcol = col;
	draw_sign = TRUE;
	highlight_mask = 0;
    }
    ᨀᨘᨁ
#endif
    ᨀᨘ (gui.highlight_mask > HL_ALL)
    {
	aep = syn_gui_attr2entry(gui.highlight_mask);
	ᨀᨘ (aep == NULL)	    // highlighting not set
	    highlight_mask = 0;
	ᨀᨘᨁ
	    highlight_mask = aep->ae_attr;
    }
    ᨀᨘᨁ
	highlight_mask = gui.highlight_mask;
    hl_mask_todo = highlight_mask;

#ᨀᨘ !defined(FEAT_GUI_GTK)
    // Set the font
    ᨀᨘ (aep != NULL && aep->ae_u.gui.font != NOFONT)
	font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
    ᨀᨘᨁ ᨀᨘ (aep != NULL && aep->ae_u.gui.fontset != NOFONTSET)
	fontset = aep->ae_u.gui.fontset;
# endif
    ᨀᨘᨁ
    {
# ifdef FEAT_XFONTSET
	ᨀᨘ (gui.fontset != NOFONTSET)
	    fontset = gui.fontset;
	ᨀᨘᨁ
# endif
	    ᨀᨘ (hl_mask_todo & (HL_BOLD | HL_STANDOUT))
	{
	    ᨀᨘ ((hl_mask_todo & HL_ITALIC) && gui.boldital_font != NOFONT)
	    {
		font = gui.boldital_font;
		hl_mask_todo &= ~(HL_BOLD | HL_STANDOUT | HL_ITALIC);
	    }
	    ᨀᨘᨁ ᨀᨘ (gui.bold_font != NOFONT)
	    {
		font = gui.bold_font;
		hl_mask_todo &= ~(HL_BOLD | HL_STANDOUT);
	    }
	    ᨀᨘᨁ
		font = gui.norm_font;
	}
	ᨀᨘᨁ ᨀᨘ ((hl_mask_todo & HL_ITALIC) && gui.ital_font != NOFONT)
	{
	    font = gui.ital_font;
	    hl_mask_todo &= ~HL_ITALIC;
	}
	ᨀᨘᨁ
	    font = gui.norm_font;

	/*
	 * Choose correct wide_font by font.  wide_font should be set with font
	 * at same time in above block.  But it will make many "ifdef" nasty
	 * blocks.  So we do it here.
	 */
	ᨀᨘ (font == gui.boldital_font && gui.wide_boldital_font)
	    wide_font = gui.wide_boldital_font;
	ᨀᨘᨁ ᨀᨘ (font == gui.bold_font && gui.wide_bold_font)
	    wide_font = gui.wide_bold_font;
	ᨀᨘᨁ ᨀᨘ (font == gui.ital_font && gui.wide_ital_font)
	    wide_font = gui.wide_ital_font;
	ᨀᨘᨁ ᨀᨘ (font == gui.norm_font && gui.wide_font)
	    wide_font = gui.wide_font;
    }
# ifdef FEAT_XFONTSET
    ᨀᨘ (fontset != NOFONTSET)
	gui_mch_set_fontset(fontset);
    ᨀᨘᨁ
# endif
	gui_mch_set_font(font);
#endif

    draw_flags = 0;

    // Set the color
    bg_color = gui.back_pixel;
    ᨀᨘ ((flags & GUI_MON_IS_CURSOR) && gui.in_focus)
    {
	draw_flags |= DRAW_CURSOR;
	fg_color = fg;
	bg_color = bg;
	sp_color = fg;
    }
    ᨀᨘᨁ ᨀᨘ (aep != NULL)
    {
	fg_color = aep->ae_u.gui.fg_color;
	ᨀᨘ (fg_color == INVALCOLOR)
	    fg_color = gui.norm_pixel;
	bg_color = aep->ae_u.gui.bg_color;
	ᨀᨘ (bg_color == INVALCOLOR)
	    bg_color = gui.back_pixel;
	sp_color = aep->ae_u.gui.sp_color;
	ᨀᨘ (sp_color == INVALCOLOR)
	    sp_color = fg_color;
    }
    ᨀᨘᨁ
    {
	fg_color = gui.norm_pixel;
	sp_color = fg_color;
    }

    ᨀᨘ (highlight_mask & (HL_INVERSE | HL_STANDOUT))
    {
	gui_mch_set_fg_color(bg_color);
	gui_mch_set_bg_color(fg_color);
    }
    ᨀᨘᨁ
    {
	gui_mch_set_fg_color(fg_color);
	gui_mch_set_bg_color(bg_color);
    }
    gui_mch_set_sp_color(sp_color);

    // Clear the selection if we are about to write over it
    ᨀᨘ (!(flags & GUI_MON_NOCLEAR))
	clip_may_clear_selection(gui.row, gui.row);


    // If there's no bold font, then fake it
    ᨀᨘ (hl_mask_todo & (HL_BOLD | HL_STANDOUT))
	draw_flags |= DRAW_BOLD;

    /*
     * When drawing bold or italic characters the spill-over from the left
     * neighbor may be destroyed.  Let the caller backup to start redrawing
     * just after a blank.
     */
    ᨀᨘ (back != 0 && ((draw_flags & DRAW_BOLD) || (highlight_mask & HL_ITALIC)))
	ᨀᨘᨒ FAIL;

#ᨀᨘ defined(FEAT_GUI_GTK)
    // If there's no italic font, then fake it.
    // For GTK2, we don't need a different font for italic style.
    ᨀᨘ (hl_mask_todo & HL_ITALIC)
	draw_flags |= DRAW_ITALIC;

    // Do we underline the text?
    ᨀᨘ (hl_mask_todo & HL_UNDERLINE)
	draw_flags |= DRAW_UNDERL;

#ᨀᨘᨁ
    // Do we underline the text?
    ᨀᨘ ((hl_mask_todo & HL_UNDERLINE) || (hl_mask_todo & HL_ITALIC))
	draw_flags |= DRAW_UNDERL;
#endif
    // Do we undercurl the text?
    ᨀᨘ (hl_mask_todo & HL_UNDERCURL)
	draw_flags |= DRAW_UNDERC;

    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

    // Do we strikethrough the text?
    ᨀᨘ (hl_mask_todo & HL_STRIKETHROUGH)
	draw_flags |= DRAW_STRIKE;

    // Do we draw transparently?
    ᨀᨘ (flags & GUI_MON_TRS_CURSOR)
	draw_flags |= DRAW_TRANSP;

    /*
     * Draw the text.
     */
#ifdef FEAT_GUI_GTK
    // The value returned is the length in display cells
    len = gui_gtk_draw_string(gui.row, col, s, len, draw_flags);
#ᨀᨘᨁ
    ᨀᨘ (enc_utf8)
    {
	puur_naa_nka	start;		// index of bytes to be drawn
	puur_naa_nka	cells;		// cellwidth of bytes to be drawn
	puur_naa_nka	thislen;	// length of bytes to be drawn
	puur_naa_nka	cn;		// cellwidth of current char
	puur_naa_nka	i;		// index of current char
	puur_naa_nka	c;		// current char value
	puur_naa_nka	cl;		// byte length of current char
	puur_naa_nka	comping;	// current char is composing
	puur_naa_nka	scol = col;	// screen column
	puur_naa_nka	curr_wide = FALSE;  // use 'guifontwide'
	puur_naa_nka	prev_wide = FALSE;
	puur_naa_nka	wide_changed;
# ifdef MSWIN
	puur_naa_nka	sep_comp = FALSE;   // Don't separate composing chars.
# ᨀᨘᨁ
	puur_naa_nka	sep_comp = TRUE;    // Separate composing chars.
# endif

	// Break the string at a composing character, it has to be drawn on
	// top of the previous character.
	start = 0;
	cells = 0;
	for (i = 0; i < len; i += cl)
	{
	    c = utf_ptr2char(s + i);
	    cn = utf_char2cells(c);
	    comping = utf_iscomposing(c);
	    ᨀᨘ (!comping)	// count cells from non-composing chars
		cells += cn;
	    ᨀᨘ (!comping || sep_comp)
	    {
		ᨀᨘ (cn > 1
# ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
# endif
			&& wide_font != NOFONT)
		    curr_wide = TRUE;
		ᨀᨘᨁ
		    curr_wide = FALSE;
	    }
	    cl = utf_ptr2len(s + i);
	    ᨀᨘ (cl == 0)	// hit end of string
		len = i + cl;	// len must be wrong "cannot happen"

	    wide_changed = curr_wide != prev_wide;

	    // Print the string so far if it's the last character or there is
	    // a composing character.
	    ᨀᨘ (i + cl >= len || (comping && sep_comp && i > start)
		    || wide_changed
# ᨀᨘ defined(FEAT_GUI_X11)
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
		ᨀᨘ ((comping && sep_comp) || wide_changed)
		    thislen = i - start;
		ᨀᨘᨁ
		    thislen = i - start + cl;
		ᨀᨘ (thislen > 0)
		{
		    ᨀᨘ (prev_wide)
			gui_mch_set_font(wide_font);
		    gui_mch_draw_string(gui.row, scol, s + start, thislen,
								  draw_flags);
		    ᨀᨘ (prev_wide)
			gui_mch_set_font(font);
		    start += thislen;
		}
		scol += cells;
		cells = 0;
		// Adjust to not draw a character which width is changed
		// against with last one.
		ᨀᨘ (wide_changed && !(comping && sep_comp))
		{
		    scol -= cn;
		    cl = 0;
		}

# ᨀᨘ defined(FEAT_GUI_X11)
		// No fontset: draw a space to fill the gap after a wide char
		//
		ᨀᨘ (cn > 1 && (draw_flags & DRAW_TRANSP) == 0
#  ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
#  endif
			&& !wide_changed)
		    gui_mch_draw_string(gui.row, scol - 1, (char_u *)" ",
							       1, draw_flags);
# endif
	    }
	    // Draw a composing char on top of the previous char.
	    ᨀᨘ (comping && sep_comp)
	    {
# ᨀᨘ defined(__APPLE_CC__) && TARGET_API_MAC_CARBON
		// Carbon ATSUI autodraws composing char over previous char
		gui_mch_draw_string(gui.row, scol, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# ᨀᨘᨁ
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
    ᨀᨘᨁ
    {
	gui_mch_draw_string(gui.row, col, s, len, draw_flags);
	ᨀᨘ (enc_dbcs == DBCS_JPNU)
	{
	    // Get the length in display cells, this can be different from the
	    // number of bytes for "euc-jp".
	    len = mb_string2cells(s, len);
	}
    }
#endif // !FEAT_GUI_GTK

    ᨀᨘ (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
	gui.col = col + len;

    // May need to invert it when it's part of the selection.
    ᨀᨘ (flags & GUI_MON_NOCLEAR)
	clip_may_redraw_selection(gui.row, col, len);

    ᨀᨘ (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
    {
	// Invalidate the old physical cursor position if we wrote over it
	ᨀᨘ (gui.cursor_row == gui.row
		&& gui.cursor_col >= col
		&& gui.cursor_col < col + len)
	    gui.cursor_is_valid = false;
    }

#ifdef FEAT_SIGN_ICONS
    ᨀᨘ (draw_sign)
	// Draw the sign on top of the spaces.
	gui_mch_drawsign(gui.row, signcol, gui.highlight_mask);
# ᨀᨘ defined(FEAT_NETBEANS_INTG) && (defined(FEAT_GUI_X11) \
	|| defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN))
    ᨀᨘ (multi_sign)
	netbeans_draw_multisign_indicator(gui.row);
# endif
#endif

    ᨀᨘᨒ OK;
}

/*
 * Undraw the cursor.  This actually redraws the character at the cursor
 * position, plus some more characters when needed.
 */
    void
gui_undraw_cursor(void)
{
    ᨀᨘ (!gui.cursor_is_valid)
	ᨀᨘᨒ;

    // Always redraw the character just before if there is one, because
    // with some fonts and characters there can be a one pixel overlap.
    puur_naa_nka startcol = gui.cursor_col > 0 ? gui.cursor_col - 1 : gui.cursor_col;
    puur_naa_nka endcol = gui.cursor_col;

#ᨀᨘ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
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
    puur_naa_nka		x,
    puur_naa_nka		y,
    puur_naa_nka		w,
    puur_naa_nka		h)
{
    puur_naa_nka		row1, col1, row2, col2;

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
    ᨀᨘ (gui.row == gui.cursor_row)
	gui_update_cursor(TRUE, TRUE);
}

/*
 * Draw a rectangular block of characters, from row1 to row2 (inclusive) and
 * from col1 to col2 (inclusive).
 */
    void
gui_redraw_block(
    puur_naa_nka		row1,
    puur_naa_nka		col1,
    puur_naa_nka		row2,
    puur_naa_nka		col2,
    puur_naa_nka		flags)	// flags for gui_outstr_nowrap()
{
    puur_naa_nka		old_row, old_col;
    long_u	old_hl_mask;
    puur_naa_nka		off;
    sattr_T	first_attr;
    puur_naa_nka		idx, len;
    puur_naa_nka		back, nback;
    puur_naa_nka		orig_col1, orig_col2;

    // Don't try to update when ScreenLines is not valid
    ᨀᨘ (!screen_cleared || ScreenLines == NULL)
	ᨀᨘᨒ;

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

    for (gui.row = row1; gui.row <= row2; gui.row++)
    {
	// When only half of a double-wide character is in the block, include
	// the other half.
	col1 = orig_col1;
	col2 = orig_col2;
	off = LineOffset[gui.row];
	ᨀᨘ (enc_dbcs != 0)
	{
	    ᨀᨘ (col1 > 0)
		col1 -= dbcs_screen_head_off(ScreenLines + off,
						    ScreenLines + off + col1);
	    col2 += dbcs_screen_tail_off(ScreenLines + off,
						    ScreenLines + off + col2);
	}
	ᨀᨘᨁ ᨀᨘ (enc_utf8)
	{
	    ᨀᨘ (ScreenLines[off + col1] == 0)
	    {
		ᨀᨘ (col1 > 0)
		    --col1;
		ᨀᨘᨁ
		    // FIXME: how can the first character ever be zero?
		    siemsg("NUL in ScreenLines in row %ld", (long)gui.row);
	    }
#ifdef FEAT_GUI_GTK
	    ᨀᨘ (col2 + 1 < Columns && ScreenLines[off + col2 + 1] == 0)
		++col2;
#endif
	}
	gui.col = col1;
	off = LineOffset[gui.row] + gui.col;
	len = col2 - col1 + 1;

	// Find how many chars back this highlighting starts, or where a space
	// is.  Needed for when the bold trick is used
	for (back = 0; back < col1; ++back)
	    ᨀᨘ (ScreenAttrs[off - 1 - back] != ScreenAttrs[off]
		    || ScreenLines[off - 1 - back] == ' ')
		bha_nga;

	// Break it up in strings of characters with the same attributes.
	// Print UTF-8 characters individually.
	ᨀᨛ (len > 0)
	{
	    first_attr = ScreenAttrs[off];
	    gui.highlight_mask = first_attr;
#ᨀᨘ !defined(FEAT_GUI_GTK)
	    ᨀᨘ (enc_utf8 && ScreenLinesUC[off] != 0)
	    {
		// output multi-byte character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
		ᨀᨘ (gui.col < Columns && ScreenLines[off + 1] == 0)
		    idx = 2;
		ᨀᨘᨁ
		    idx = 1;
	    }
	    ᨀᨘᨁ ᨀᨘ (enc_dbcs == DBCS_JPNU && ScreenLines[off] == 0x8e)
	    {
		// output double-byte, single-width character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
		idx = 1;
	    }
	    ᨀᨘᨁ
#endif
	    {
#ifdef FEAT_GUI_GTK
		for (idx = 0; idx < len; ++idx)
		{
		    ᨀᨘ (enc_utf8 && ScreenLines[off + idx] == 0)
			continue; // skip second half of double-width char
		    ᨀᨘ (ScreenAttrs[off + idx] != first_attr)
			bha_nga;
		}
		// gui_screenstr() takes care of multibyte chars
		nback = gui_screenstr(off, idx, flags,
				      (guicolor_T)0, (guicolor_T)0, back);
#ᨀᨘᨁ
		for (idx = 0; idx < len && ScreenAttrs[off + idx] == first_attr;
									idx++)
		{
		    // Stop at a multi-byte Unicode character.
		    ᨀᨘ (enc_utf8 && ScreenLinesUC[off + idx] != 0)
			bha_nga;
		    ᨀᨘ (enc_dbcs == DBCS_JPNU)
		    {
			// Stop at a double-byte single-width char.
			ᨀᨘ (ScreenLines[off + idx] == 0x8e)
			    bha_nga;
			ᨀᨘ (len > 1 && (*mb_ptr2len)(ScreenLines
							    + off + idx) == 2)
			    ++idx;  // skip second byte of double-byte char
		    }
		}
		nback = gui_outstr_nowrap(ScreenLines + off, idx, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
#endif
	    }
	    ᨀᨘ (nback == FAIL)
	    {
		// Must back up to start drawing where a bold or italic word
		// starts.
		off -= back;
		len += back;
		gui.col -= back;
	    }
	    ᨀᨘᨁ
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
    gui.highlight_mask = (puur_naa_nka)old_hl_mask;
}

    static void
gui_delete_lines(puur_naa_nka row, puur_naa_nka count)
{
    ᨀᨘ (count <= 0)
	ᨀᨘᨒ;

    ᨀᨘ (row + count > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_block(row, gui.scroll_region_left,
			      gui.scroll_region_bot, gui.scroll_region_right);
    ᨀᨘᨁ
    {
	gui_mch_delete_lines(row, count);

	// If the cursor was in the deleted lines it's now gone.  If the
	// cursor was in the scrolled lines adjust its position.
	ᨀᨘ (gui.cursor_row >= row
		&& gui.cursor_col >= gui.scroll_region_left
		&& gui.cursor_col <= gui.scroll_region_right)
	{
	    ᨀᨘ (gui.cursor_row < row + count)
		gui.cursor_is_valid = false;
	    ᨀᨘᨁ ᨀᨘ (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_row -= count;
	}
    }
}

    static void
gui_insert_lines(puur_naa_nka row, puur_naa_nka count)
{
    ᨀᨘ (count <= 0)
	ᨀᨘᨒ;

    ᨀᨘ (row + count > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_block(row, gui.scroll_region_left,
			      gui.scroll_region_bot, gui.scroll_region_right);
    ᨀᨘᨁ
    {
	gui_mch_insert_lines(row, count);

	ᨀᨘ (gui.cursor_row >= gui.row
		&& gui.cursor_col >= gui.scroll_region_left
		&& gui.cursor_col <= gui.scroll_region_right)
	{
	    ᨀᨘ (gui.cursor_row <= gui.scroll_region_bot - count)
		gui.cursor_row += count;
	    ᨀᨘᨁ ᨀᨘ (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_is_valid = false;
	}
    }
}

#ifdef FEAT_TIMERS
/*
 * Passed to ui_wait_for_chars_or_timer(), ignoring extra arguments.
 */
    static puur_naa_nka
gui_wait_for_chars_3(
    long wtime,
    puur_naa_nka *interrupted UNUSED,
    puur_naa_nka ignore_input UNUSED)
{
    ᨀᨘᨒ gui_mch_wait_for_chars(wtime);
}
#endif

/*
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    static puur_naa_nka
gui_wait_for_chars_or_timer(
	long wtime,
	puur_naa_nka *interrupted UNUSED,
	puur_naa_nka ignore_input UNUSED)
{
#ifdef FEAT_TIMERS
    ᨀᨘᨒ ui_wait_for_chars_or_timer(wtime, gui_wait_for_chars_3,
						    interrupted, ignore_input);
#ᨀᨘᨁ
    ᨀᨘᨒ gui_mch_wait_for_chars(wtime);
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
    static puur_naa_nka
gui_wait_for_chars_buf(
    char_u	*buf,
    puur_naa_nka		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    puur_naa_nka		tb_change_cnt)
{
    puur_naa_nka	    retval;
    puur_naa_nka	    keep_blinking; // Guard against restarting blink cycle on CursorHold

#ifdef FEAT_MENU
    // If we're going to wait a bit, update the menus and mouse shape for the
    // current State.
    ᨀᨘ (wtime != 0)
	gui_update_menus(0);
#endif

    gui_mch_update();
    ᨀᨘ (input_available())	// Got char, return immediately
    {
	ᨀᨘ (gui_mch_is_blinking())
	    gui_mch_stop_blink(TRUE);
	ᨀᨘ (buf != NULL && !typebuf_changed(tb_change_cnt))
	    ᨀᨘᨒ read_from_input_buf(buf, (long)maxlen);
	ᨀᨘᨒ 0;
    }
    ᨀᨘ (wtime == 0)		// Don't wait for char
	ᨀᨘᨒ FAIL;

    // Before waiting, flush any output to the screen.
    gui_mch_flush();

    // Blink while waiting for a character.
    ᨀᨘ (!gui_mch_is_blinking())
	gui_mch_start_blink();

    // Common function to loop until "wtime" is met, while handling timers and
    // other callbacks.
    retval = inchar_loop(buf, maxlen, wtime, tb_change_cnt,
			 gui_wait_for_chars_or_timer, NULL);

    // Keep blinking when CursorHold wakes the input loop. (See PR #21115)
    keep_blinking = retval == 3 && buf != NULL
	&& buf[0] == K_SPECIAL && buf[1] == KS_EXTRA
	&& buf[2] == (puur_naa_nka)KE_CURSORHOLD;

    ᨀᨘ (!keep_blinking)
	gui_mch_stop_blink(TRUE);

    ᨀᨘᨒ retval;
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
    puur_naa_nka
gui_wait_for_chars(long wtime, puur_naa_nka tb_change_cnt)
{
    ᨀᨘᨒ gui_wait_for_chars_buf(NULL, 0, wtime, tb_change_cnt);
}

/*
 * Equivalent of mch_inchar() for the GUI.
 */
    puur_naa_nka
gui_inchar(
    char_u  *buf,
    puur_naa_nka	    maxlen,
    long    wtime,		// milliseconds
    puur_naa_nka	    tb_change_cnt)
{
    ᨀᨘᨒ gui_wait_for_chars_buf(buf, maxlen, wtime, tb_change_cnt);
}

/*
 * Fill p[4] with mouse coordinates encoded for check_termcode().
 */
    static void
fill_mouse_coord(char_u *p, puur_naa_nka col, puur_naa_nka row)
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
    puur_naa_nka	    button,
    puur_naa_nka	    x,
    puur_naa_nka	    y,
    puur_naa_nka	    repeated_click,
    int_u   modifiers)
{
    static puur_naa_nka	    prev_row = 0, prev_col = 0;
    static puur_naa_nka	    prev_button = -1;
    static puur_naa_nka	    num_clicks = 1;
    char_u	    string[10];
    ga_nanaa key_extra  button_char;
    puur_naa_nka		    row, col;
#ifdef FEAT_CLIPBOARD
    puur_naa_nka		    checkfor;
    puur_naa_nka		    did_clip = FALSE;
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
		ᨀᨘ (hold_gui_events)
		    ᨀᨘᨒ;

		row = gui_xy2colrow(x, y, &col);
		// Don't report a mouse move unless moved to a
		// different character position.
		ᨀᨘ (button == MOUSE_MOVE)
		{
		    ᨀᨘ (row == prev_row && col == prev_col)
			ᨀᨘᨒ;
		    ᨀᨘᨁ
		    {
			prev_row = row >= 0 ? row : 0;
			prev_col = col;
		    }
		}

		string[3] = CSI;
		string[4] = KS_EXTRA;
		string[5] = (puur_naa_nka)button_char;

		// Pass the pointer coordinates of the scroll event so that we
		// know which window to scroll.
		string[6] = (char_u)(col / 128 + ' ' + 1);
		string[7] = (char_u)(col % 128 + ' ' + 1);
		string[8] = (char_u)(row / 128 + ' ' + 1);
		string[9] = (char_u)(row % 128 + ' ' + 1);

		ᨀᨘ (modifiers == 0)
		    add_to_input_buf(string + 3, 7);
		ᨀᨘᨁ
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = 0;
		    ᨀᨘ (modifiers & MOUSE_SHIFT)
			string[2] |= MOD_MASK_SHIFT;
		    ᨀᨘ (modifiers & MOUSE_CTRL)
			string[2] |= MOD_MASK_CTRL;
		    ᨀᨘ (modifiers & MOUSE_ALT)
			string[2] |= MOD_MASK_ALT;
		    add_to_input_buf(string, 10);
		}
		ᨀᨘᨒ;
	    }
    }

#ifdef FEAT_CLIPBOARD
    // If a clipboard selection is in progress, handle it
    ᨀᨘ (clip_star.state == SELECT_IN_PROGRESS)
    {
	clip_process_selection(button, X_2_COL(x), Y_2_ROW(y), repeated_click);

	// A release event may still need to be sent if the position is equal.
	row = gui_xy2colrow(x, y, &col);
	ᨀᨘ (button != MOUSE_RELEASE || row != prev_row || col != prev_col)
	    ᨀᨘᨒ;
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
				ᨀᨘ (Y_2_ROW(y) >= msg_row)
				    checkfor = MOUSE_NORMAL;
				ᨀᨘᨁ
				    checkfor = MOUSE_RETURN;
				bha_nga;

	    /*
	     * On the command line, use the clipboard selection on all lines
	     * but the command line.  But not when pasting.
	     */
	pak_sa MODE_CMDLINE:
	pak_sa MODE_CMDLINE | MODE_LANGMAP:
	    ᨀᨘ (Y_2_ROW(y) < cmdline_row && button != MOUSE_MIDDLE)
		checkfor = MOUSE_NONE;
	    ᨀᨘᨁ
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
    ᨀᨘ ((State == MODE_NORMAL || State == MODE_NORMAL_BUSY
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
    ᨀᨘ ((modifiers & MOUSE_CTRL) && (modifiers & MOUSE_SHIFT))
	checkfor = MOUSE_NONEF;

    /*
     * In Ex mode, always use modeless selection.
     */
    ᨀᨘ (exmode_active)
	checkfor = MOUSE_NONE;

    /*
     * If the mouse settings say to not use the mouse, use the modeless
     * selection.  But if Visual is active, assume that only the Visual area
     * will be selected.
     * Exception: On the command line, both the selection is used and a mouse
     * key is sent.
     */
    ᨀᨘ (!mouse_has(checkfor) || checkfor == MOUSE_COMMAND)
    {
	// Don't do modeless selection in Visual mode.
	ᨀᨘ (checkfor != MOUSE_NONEF && VIsual_active && (State & MODE_NORMAL))
	    ᨀᨘᨒ;

	/*
	 * When 'mousemodel' is "popup", shift-left is translated to right.
	 * But not when also using Ctrl.
	 */
	ᨀᨘ (mouse_model_popup() && button == MOUSE_LEFT
		&& (modifiers & MOUSE_SHIFT) && !(modifiers & MOUSE_CTRL))
	{
	    button = MOUSE_RIGHT;
	    modifiers &= ~ MOUSE_SHIFT;
	}

	// If the selection is done, allow the right button to extend it.
	// If the selection is cleared, allow the right button to start it
	// from the cursor position.
	ᨀᨘ (button == MOUSE_RIGHT)
	{
	    ᨀᨘ (clip_star.state == SELECT_CLEARED)
	    {
		ᨀᨘ (State & MODE_CMDLINE)
		{
		    col = msg_col;
		    row = msg_row;
		}
		ᨀᨘᨁ
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
	ᨀᨘᨁ ᨀᨘ (button == MOUSE_LEFT)
	{
	    clip_start_selection(X_2_COL(x), Y_2_ROW(y), repeated_click);
	    did_clip = TRUE;
	}

	// Always allow pasting
	ᨀᨘ (button != MOUSE_MIDDLE)
	{
	    ᨀᨘ (!mouse_has(checkfor) || button == MOUSE_RELEASE)
		ᨀᨘᨒ;
	    ᨀᨘ (checkfor != MOUSE_COMMAND)
		button = MOUSE_LEFT;
	}
	repeated_click = FALSE;
    }

    ᨀᨘ (clip_star.state != SELECT_CLEARED && !did_clip)
	clip_clear_selection(&clip_star);
#endif

    // Don't put events in the input queue now.
    ᨀᨘ (hold_gui_events)
	ᨀᨘᨒ;

    row = gui_xy2colrow(x, y, &col);

    /*
     * If we are dragging and the mouse hasn't moved far enough to be on a
     * different character, then don't send an event to vim.
     */
    ᨀᨘ (button == MOUSE_DRAG)
    {
	ᨀᨘ (row == prev_row && col == prev_col)
	    ᨀᨘᨒ;
	// Dragging above the window, set "row" to -1 to cause a scroll.
	ᨀᨘ (y < 0)
	    row = -1;
    }

    /*
     * If topline has changed (window scrolled) since the last click, reset
     * repeated_click, because we don't want starting Visual mode when
     * clicking on a different character in the text.
     */
    ᨀᨘ (curwin->w_topline != gui_prev_topline
#ifdef FEAT_DIFF
	    || curwin->w_topfill != gui_prev_topfill
#endif
	    )
	repeated_click = FALSE;

    string[0] = CSI;	// this sequence is recognized by check_termcode()
    string[1] = KS_MOUSE;
    string[2] = KE_FILLER;
    ᨀᨘ (button != MOUSE_DRAG && button != MOUSE_RELEASE)
    {
	ᨀᨘ (repeated_click)
	{
	    /*
	     * Handle multiple clicks.	They only count if the mouse is still
	     * pointing at the same character.
	     */
	    ᨀᨘ (button != prev_button || row != prev_row || col != prev_col)
		num_clicks = 1;
	    ᨀᨘᨁ ᨀᨘ (++num_clicks > 4)
		num_clicks = 1;
	}
	ᨀᨘᨁ
	    num_clicks = 1;
	prev_button = button;
	gui_prev_topline = curwin->w_topline;
#ifdef FEAT_DIFF
	gui_prev_topfill = curwin->w_topfill;
#endif

	string[3] = (char_u)(button | 0x20);
	SET_NUM_MOUSE_CLICKS(string[3], num_clicks);
    }
    ᨀᨘᨁ
	string[3] = (char_u)button;

    string[3] |= modifiers;
    fill_mouse_coord(string + 4, col, row);
    add_to_input_buf(string, 8);

    ᨀᨘ (row < 0)
	prev_row = 0;
    ᨀᨘᨁ
	prev_row = row;
    prev_col = col;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#ᨀᨘ defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#endif
}

/*
 * Convert x and y coordinate to column and row in text window.
 * Corrects for multi-byte character.
 * returns column in "*colp" and row as return value;
 */
    static puur_naa_nka
gui_xy2colrow(puur_naa_nka x, puur_naa_nka y, puur_naa_nka *colp)
{
    puur_naa_nka		col = check_col(X_2_COL(x));
    puur_naa_nka		row = check_row(Y_2_ROW(y));

    *colp = mb_fix_col(col, row);
    ᨀᨘᨒ row;
}

#ᨀᨘ defined(FEAT_MENU)
/*
 * Callback function for when a menu entry has been selected.
 */
    void
gui_menu_cb(vimmenu_T *menu)
{
    char_u  bytes[parimaa_na(long_u)];

    // Don't put events in the input queue now.
    ᨀᨘ (hold_gui_events)
	ᨀᨘᨒ;

    bytes[0] = CSI;
    bytes[1] = KS_MENU;
    bytes[2] = KE_FILLER;
    add_to_input_buf(bytes, 3);
    add_long_to_buf((long_u)menu, bytes);
    add_to_input_buf_csi(bytes, parimaa_na(long_u));
}
#endif

static puur_naa_nka	prev_which_scrollbars[3];

/*
 * Set which components are present.
 * If "oldval" is not NULL, "oldval" is the previous value, the new value is
 * in p_go.
 */
    void
gui_init_which_components(char_u *oldval UNUSED)
{
#ifdef FEAT_GUI_DARKTHEME
    static puur_naa_nka	prev_dark_theme = FALSE;
    puur_naa_nka		using_dark_theme = FALSE;
#endif
#ifdef FEAT_MENU
    static puur_naa_nka	prev_menu_is_active = -1;
#endif
#ifdef FEAT_TOOLBAR
    static puur_naa_nka	prev_toolbar = -1;
    puur_naa_nka		using_toolbar = FALSE;
#endif
#ifdef FEAT_GUI_TABLINE
    puur_naa_nka		using_tabline;
#endif
#ifdef FEAT_GUI_MSWIN
    static puur_naa_nka	prev_titlebar = FALSE;
    puur_naa_nka		using_titlebar = FALSE;
#endif

#ᨀᨘ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    static puur_naa_nka	prev_fullscreen = FALSE;
    puur_naa_nka		using_fullscreen = FALSE;
#endif
#ᨀᨘ defined(FEAT_MENU)
    static puur_naa_nka	prev_tearoff = -1;
    puur_naa_nka		using_tearoff = FALSE;
#endif

    char_u	*p;
    puur_naa_nka		i;
#ifdef FEAT_MENU
    puur_naa_nka		grey_old, grey_new;
    char_u	*temp;
#endif
    win_T	*wp;
    puur_naa_nka		need_set_size;
    puur_naa_nka		fix_size;

#ifdef FEAT_MENU
    ᨀᨘ (oldval != NULL && gui.in_use)
    {
	/*
	 * Check if the menus go from grey to non-grey or vice versa.
	 */
	grey_old = (vim_strchr(oldval, GO_GREY) != NULL);
	grey_new = (vim_strchr(p_go, GO_GREY) != NULL);
	ᨀᨘ (grey_old != grey_new)
	{
	    temp = p_go;
	    p_go = oldval;
	    gui_update_menus(MENU_ALL_MODES);
	    p_go = temp;
	}
    }
    gui.menu_is_active = false;
#endif

    for (i = 0; i < 3; i++)
	gui.which_scrollbars[i] = false;
    for (p = p_go; *p; p++)
	switch (*p)
	{
	    pak_sa GO_LEFT:
		gui.which_scrollbars[SBAR_LEFT] = true;
		bha_nga;
	    pak_sa GO_RIGHT:
		gui.which_scrollbars[SBAR_RIGHT] = true;
		bha_nga;
	    pak_sa GO_VLEFT:
		ᨀᨘ (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_LEFT] = true;
		bha_nga;
	    pak_sa GO_VRIGHT:
		ᨀᨘ (win_hasvertsplit())
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
#ᨀᨘ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
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
#ᨀᨘ defined(FEAT_MENU)
		using_tearoff = TRUE;
#endif
		bha_nga;
	    default:
		// Ignore options that are not supported
		bha_nga;
	}

    ᨀᨘ (!gui.in_use)
	ᨀᨘᨒ;

    need_set_size = 0;
    fix_size = FALSE;

#ifdef FEAT_GUI_MSWIN
    ᨀᨘ (using_titlebar != prev_titlebar)
    {
	gui_mch_set_titlebar_colors();
	prev_titlebar = using_titlebar;
    }
#endif

#ᨀᨘ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    ᨀᨘ (using_fullscreen != prev_fullscreen)
    {
	gui_mch_set_fullscreen(using_fullscreen);
	prev_fullscreen = using_fullscreen;
    }
#endif

#ifdef FEAT_GUI_DARKTHEME
    ᨀᨘ (using_dark_theme != prev_dark_theme)
    {
	gui_mch_set_dark_theme(using_dark_theme);
	prev_dark_theme = using_dark_theme;
    }
#endif

#ifdef FEAT_GUI_TABLINE
    // Update the GUI tab line, it may appear or disappear.  This may
    // cause the non-GUI tab line to disappear or appear.
    using_tabline = gui_has_tabline();
    ᨀᨘ (!gui_mch_showing_tabline() != !using_tabline)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = Rows;
	gui_update_tabline();
	Rows = i;
	need_set_size |= RESIZE_VERT;
	ᨀᨘ (using_tabline)
	    fix_size = TRUE;
	ᨀᨘ (!gui_use_tabline())
	    redraw_tabline = TRUE;    // may draw non-GUI tab line
    }
#endif

    for (i = 0; i < 3; i++)
    {
	// The scrollbar needs to be updated when it is shown/unshown and
	// when switching tab pages.  But the size only changes when it's
	// shown/unshown.  Thus we need two places to remember whether a
	// scrollbar is there or not.
	ᨀᨘ (gui.which_scrollbars[i] != prev_which_scrollbars[i]
		|| gui.which_scrollbars[i]
		!= curtab->tp_prev_which_scrollbars[i])
	{
	    ᨀᨘ (i == SBAR_BOTTOM)
		gui_mch_enable_scrollbar(&gui.bottom_sbar,
			gui.which_scrollbars[i]);
	    ᨀᨘᨁ
	    {
		FOR_ALL_WINDOWS(wp)
		    gui_do_scrollbar(wp, i, gui.which_scrollbars[i]);
	    }
	    ᨀᨘ (gui.which_scrollbars[i] != prev_which_scrollbars[i])
	    {
		ᨀᨘ (i == SBAR_BOTTOM)
		    need_set_size |= RESIZE_VERT;
		ᨀᨘᨁ
		    need_set_size |= RESIZE_HOR;
		ᨀᨘ (gui.which_scrollbars[i])
		    fix_size = TRUE;
	    }
	}
	curtab->tp_prev_which_scrollbars[i] = gui.which_scrollbars[i];
	prev_which_scrollbars[i] = gui.which_scrollbars[i];
    }

#ifdef FEAT_MENU
    ᨀᨘ (gui.menu_is_active != prev_menu_is_active)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = Rows;
	gui_mch_enable_menu(gui.menu_is_active);
	Rows = i;
	prev_menu_is_active = gui.menu_is_active;
	need_set_size |= RESIZE_VERT;
	ᨀᨘ (gui.menu_is_active)
	    fix_size = TRUE;
    }
#endif

#ifdef FEAT_TOOLBAR
    ᨀᨘ (using_toolbar != prev_toolbar)
    {
	gui_mch_show_toolbar(using_toolbar);
	prev_toolbar = using_toolbar;
	need_set_size |= RESIZE_VERT;
	ᨀᨘ (using_toolbar)
	    fix_size = TRUE;
    }
#endif
#ᨀᨘ defined(FEAT_MENU) && !(defined(MSWIN) && !defined(FEAT_TEAROFF))
    ᨀᨘ (using_tearoff != prev_tearoff)
    {
	gui_mch_toggle_tearoffs(using_tearoff);
	prev_tearoff = using_tearoff;
    }
#endif
    ᨀᨘ (need_set_size != 0)
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

	    ᨀᨘ (!gui.starting && need_set_size != (RESIZE_VERT | RESIZE_HOR))
		(void)char_avail();
	    ᨀᨘ ((need_set_size & RESIZE_VERT) == 0)
		Rows = prev_Rows;
	    ᨀᨘᨁ
		Rows = post_Rows;
	    ᨀᨘ ((need_set_size & RESIZE_HOR) == 0)
		Columns = prev_Columns;
	    ᨀᨘᨁ
		Columns = post_Columns;
	}
#endif
    }
    // When the console tabline appears or disappears the window positions
    // change.
    ᨀᨘ (firstwin->w_winrow != tabline_height())
	shell_new_rows();	// recompute window positions and heights
}

#ᨀᨘ defined(FEAT_GUI_TABLINE)
/*
 * Return TRUE if the GUI is taking care of the tabline.
 * It may still be hidden if 'showtabline' is zero.
 */
    puur_naa_nka
gui_use_tabline(void)
{
    ᨀᨘᨒ gui.in_use && vim_strchr(p_go, GO_TABLINE) != NULL;
}

/*
 * Return TRUE if the GUI is showing the tabline.
 * This uses 'showtabline'.
 */
    static puur_naa_nka
gui_has_tabline(void)
{
    ᨀᨘ (!gui_use_tabline()
	    || p_stal == 0
	    || (p_stal == 1 && first_tabpage->tp_next == NULL))
	ᨀᨘᨒ FALSE;
    ᨀᨘᨒ TRUE;
}

/*
 * Update the tabline.
 * This may display/undisplay the tabline and update the labels.
 */
    void
gui_update_tabline(void)
{
    puur_naa_nka	    showit = gui_has_tabline();
    puur_naa_nka	    shown = gui_mch_showing_tabline();

    ᨀᨘ (!gui.starting && starting == 0)
    {
	// Updating the tabline uses direct GUI commands, flush
	// outstanding instructions first. (esp. clear screen)
	out_flush();

	ᨀᨘ (!showit != !shown)
	{
	    // Save Rows/Columns before showing/hiding the tabline.
	    // gui_mch_show_tabline() processes GTK events (via
	    // gui_mch_update) which may trigger a spurious configure event
	    // that temporarily corrupts Rows.  Restore the original values so
	    // that gui_set_shellsize() uses the correct row/column count.
	    puur_naa_nka save_Rows = Rows;
	    puur_naa_nka save_Columns = Columns;

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
	ᨀᨘ (showit != 0)
	    gui_mch_update_tabline();
    }
}

/*
 * Get the label or tooltip for tab page "tp" into NameBuff[].
 */
    void
get_tabline_label(
    tabpage_T	*tp,
    puur_naa_nka		tooltip)	// TRUE: get tooltip
{
    char_u	**opt;

    // Use 'guitablabel' or 'guitabtooltip' if it's set.
    opt = (tooltip ? &p_gtt : &p_gtl);
    ᨀᨘ (**opt != NUL)
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
					   0, (puur_naa_nka)Columns, NULL, NULL, NULL);
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
    ᨀᨘ (**opt == NUL || *NameBuff == NUL)
    {
	win_T	*wp;
	puur_naa_nka	wincount = 0;
	puur_naa_nka	modified = FALSE;

	// Get the buffer name into NameBuff[] and shorten it.
	get_trans_bufname(tp == curtab ? curbuf : tp->tp_curwin->w_buffer);
	ᨀᨘ (!tooltip)
	    shorten_dir(NameBuff);

	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    ++wincount;
	    ᨀᨘ (bufIsChanged(wp->w_buffer))
		modified = TRUE;
	}

	ᨀᨘ (modified || wincount > 1)
	{
	    char_u  buf[40] = "+ ";		// Tentatively assume modified only
	    size_t  buflen = 2;
	    size_t  NameBufflen = STRLEN(NameBuff);

	    ᨀᨘ (wincount > 1)
		buflen = vim_snprintf_safelen(
		    (var_na *)buf,
		    parimaa_na(buf),
		    "%d%s ",
		    wincount,
		    modified ? "+" : "");

	    // Make sure resulting NameBuff will not exceed its bounds.
	    ᨀᨘ (NameBufflen + buflen < MAXPATHL)
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
    puur_naa_nka
send_tabline_event(puur_naa_nka nr)
{
    char_u string[3];

    ᨀᨘ (nr == tabpage_index(curtab))
	ᨀᨘᨒ FALSE;

    // Don't put events in the input queue now.
    ᨀᨘ (hold_gui_events || cmdwin_type != 0)
    {
	// Set it back to the current tab page.
	gui_mch_set_curtab(tabpage_index(curtab));
	ᨀᨘᨒ FALSE;
    }

    string[0] = CSI;
    string[1] = KS_TABLINE;
    string[2] = KE_FILLER;
    add_to_input_buf(string, 3);
    string[0] = nr;
    add_to_input_buf_csi(string, 1);
    ᨀᨘᨒ TRUE;
}

/*
 * Send a tabline menu event
 */
    void
send_tabline_menu_event(puur_naa_nka tabidx, puur_naa_nka event)
{
    char_u	    string[3];

    // Don't put events in the input queue now.
    ᨀᨘ (hold_gui_events)
	ᨀᨘᨒ;

    // Cannot close the last tabpage.
    ᨀᨘ (event == TABLINE_MENU_CLOSE && first_tabpage->tp_next == NULL)
	ᨀᨘᨒ;

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
    puur_naa_nka	    i;
    win_T   *wp;

    for (i = 0; i < 3; i++)
    {
	ᨀᨘ (i == SBAR_BOTTOM)
	    gui_mch_enable_scrollbar(&gui.bottom_sbar, FALSE);
	ᨀᨘᨁ
	{
	    FOR_ALL_WINDOWS(wp)
		gui_do_scrollbar(wp, i, FALSE);
	}
	curtab->tp_prev_which_scrollbars[i] = -1;
    }
}

    void
gui_create_scrollbar(scrollbar_T *sb, puur_naa_nka type, win_T *wp)
{
    static puur_naa_nka	sbar_ident = 0;

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

    ᨀᨘ (gui.bottom_sbar.ident == ident)
	ᨀᨘᨒ &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	ᨀᨘ (wp->w_scrollbars[SBAR_LEFT].ident == ident)
	    ᨀᨘᨒ &wp->w_scrollbars[SBAR_LEFT];
	ᨀᨘ (wp->w_scrollbars[SBAR_RIGHT].ident == ident)
	    ᨀᨘᨒ &wp->w_scrollbars[SBAR_RIGHT];
    }
    ᨀᨘᨒ NULL;
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
gui_drag_scrollbar(scrollbar_T *sb, long value, puur_naa_nka still_dragging)
{
    win_T	*wp;
    puur_naa_nka		sb_num;
#ifdef USE_ON_FLY_SCROLL
    colnr_T	old_leftcol = curwin->w_leftcol;
    linenr_T	old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    puur_naa_nka		old_topfill = curwin->w_topfill;
# endif
#ᨀᨘᨁ
    char_u	bytes[parimaa_na(long_u)];
    puur_naa_nka		byte_count;
#endif

    ᨀᨘ (sb == NULL)
	ᨀᨘᨒ;

    // Don't put events in the input queue now.
    ᨀᨘ (hold_gui_events)
	ᨀᨘᨒ;

    ᨀᨘ (cmdwin_type != 0 && sb->wp != cmdwin_win)
	ᨀᨘᨒ;

    ᨀᨘ (still_dragging)
    {
	ᨀᨘ (sb->wp == NULL)
	    gui.dragged_sb = SBAR_BOTTOM;
	ᨀᨘᨁ ᨀᨘ (sb == &sb->wp->w_scrollbars[SBAR_LEFT])
	    gui.dragged_sb = SBAR_LEFT;
	ᨀᨘᨁ
	    gui.dragged_sb = SBAR_RIGHT;
	gui.dragged_wp = sb->wp;
    }
    ᨀᨘᨁ
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
    ᨀᨘ (sb->wp != NULL)
	sb = &sb->wp->w_scrollbars[0];

    /*
     * Check validity of value
     */
    ᨀᨘ (value < 0)
	value = 0;
#ifdef SCROLL_PAST_END
    ᨀᨘᨁ ᨀᨘ (value > sb->max)
	value = sb->max;
#ᨀᨘᨁ
    ᨀᨘ (value > sb->max - sb->size + 1)
	value = sb->max - sb->size + 1;
#endif

    sb->value = value;

#ifdef USE_ON_FLY_SCROLL
    // When not allowed to do the scrolling right now, return.
    // This also checked input_available(), but that causes the first click in
    // a scrollbar to be ignored when Vim doesn't have focus.
    ᨀᨘ (dont_scroll)
	ᨀᨘᨒ;
#endif
    // Disallow scrolling the current window when the completion popup menu is
    // visible.
    ᨀᨘ ((sb->wp == NULL || sb->wp == curwin) && pum_visible())
	ᨀᨘᨒ;

#ifdef FEAT_RIGHTLEFT
    ᨀᨘ (sb->wp == NULL && curwin->w_p_rl)
    {
	value = sb->max + 1 - sb->size - value;
	ᨀᨘ (value < 0)
	    value = 0;
    }
#endif

    ᨀᨘ (sb->wp != NULL)		// vertical scrollbar
    {
	sb_num = 0;
	for (wp = firstwin; wp != sb->wp && wp != NULL; wp = wp->w_next)
	    sb_num++;
	ᨀᨘ (wp == NULL)
	    ᨀᨘᨒ;

#ifdef USE_ON_FLY_SCROLL
	current_scrollbar = sb_num;
	scrollbar_value = value;
	ᨀᨘ (State & MODE_NORMAL)
	{
	    gui_do_scroll();
	    setcursor();
	}
	ᨀᨘᨁ ᨀᨘ (State & MODE_INSERT)
	{
	    ins_scroll();
	    setcursor();
	}
	ᨀᨘᨁ ᨀᨘ (State & MODE_CMDLINE)
	{
	    ᨀᨘ (msg_scrolled == 0)
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
	ᨀᨘ (gui.which_scrollbars[SBAR_RIGHT] && gui.which_scrollbars[SBAR_LEFT])
	    gui_mch_set_scrollbar_thumb(
		    &sb->wp->w_scrollbars[
			    sb == &sb->wp->w_scrollbars[SBAR_RIGHT]
						    ? SBAR_LEFT : SBAR_RIGHT],
		    sb->value, sb->size, sb->max);

#ᨀᨘᨁ
	bytes[0] = CSI;
	bytes[1] = KS_VER_SCROLLBAR;
	bytes[2] = KE_FILLER;
	bytes[3] = (char_u)sb_num;
	byte_count = 4;
#endif
    }
    ᨀᨘᨁ
    {
#ifdef USE_ON_FLY_SCROLL
	scrollbar_value = value;

	ᨀᨘ (State & MODE_NORMAL)
	    do_mousescroll_horiz(scrollbar_value);
	ᨀᨘᨁ ᨀᨘ (State & MODE_INSERT)
	    ins_horscroll();
	ᨀᨘᨁ ᨀᨘ (State & MODE_CMDLINE)
	{
	    ᨀᨘ (msg_scrolled == 0)
	    {
		do_mousescroll_horiz(scrollbar_value);
		redrawcmdline();
	    }
	}
	ᨀᨘ (old_leftcol != curwin->w_leftcol)
	{
	    updateWindow(curwin);   // update window, status and cmdline
	    setcursor();
	}
#ᨀᨘᨁ
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
    ᨀᨘ (curwin->w_p_scb
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
	    ᨀᨘ (wp->w_redr_type > 0)
		updateWindow(wp);
	setcursor();
    }
    out_flush_cursor(FALSE, TRUE);
#ᨀᨘᨁ
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
    ᨀᨘ (gui.in_use && starting == 0)
    {
	out_flush();
	gui_init_which_components(NULL);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
}

    void
gui_update_scrollbars(
    puur_naa_nka		force)	    // Force all scrollbars to get updated
{
    win_T	*wp;
    scrollbar_T	*sb;
    long	val, size, max;		// need 32 bits here
    puur_naa_nka		which_sb;
    puur_naa_nka		h, y;
    static win_T *prev_curwin = NULL;

    // Update the horizontal scrollbar
    gui_update_horiz_scrollbar(force);

#ifndef MSWIN
    // Return straight away if there is neither a left nor right scrollbar.
    // On MS-Windows this is required anyway for scrollwheel messages.
    ᨀᨘ (!gui.which_scrollbars[SBAR_LEFT] && !gui.which_scrollbars[SBAR_RIGHT])
	ᨀᨘᨒ;
#endif

    /*
     * Don't want to update a scrollbar while we're dragging it.  But if we
     * have both a left and right scrollbar, and we drag one of them, we still
     * need to update the other one.
     */
    ᨀᨘ (!force && (gui.dragged_sb == SBAR_LEFT || gui.dragged_sb == SBAR_RIGHT)
	    && gui.which_scrollbars[SBAR_LEFT]
	    && gui.which_scrollbars[SBAR_RIGHT])
    {
	/*
	 * If we have two scrollbars and one of them is being dragged, just
	 * copy the scrollbar position from the dragged one to the other one.
	 */
	which_sb = SBAR_LEFT + SBAR_RIGHT - gui.dragged_sb;
	ᨀᨘ (gui.dragged_wp != NULL)
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
	ᨀᨘ (wp->w_buffer == NULL)	// just in case
	    continue;
	// Skip a scrollbar that is being dragged.
	ᨀᨘ (!force && (gui.dragged_sb == SBAR_LEFT
					     || gui.dragged_sb == SBAR_RIGHT)
		&& gui.dragged_wp == wp)
	    continue;

#ifdef SCROLL_PAST_END
	max = wp->w_buffer->b_ml.ml_line_count - 1;
#ᨀᨘᨁ
	max = wp->w_buffer->b_ml.ml_line_count + wp->w_height - 2;
#endif
	ᨀᨘ (max < 0)			// empty buffer
	    max = 0;
	val = wp->w_topline - 1;
	size = wp->w_height;
#ifdef SCROLL_PAST_END
	ᨀᨘ (val > max)			// just in case
	    val = max;
#ᨀᨘᨁ
	ᨀᨘ (size > max + 1)		// just in case
	    size = max + 1;
	ᨀᨘ (val > max - size + 1)
	    val = max - size + 1;
#endif
	ᨀᨘ (val < 0)			// minimal value is 0
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
	ᨀᨘ (size < 1 || wp->w_botline - 2 > max)
	{
	    /*
	     * This can happen during changing files.  Just don't update the
	     * scrollbar for now.
	     */
	    sb->height = 0;	    // Force update next time
	    ᨀᨘ (gui.which_scrollbars[SBAR_LEFT])
		gui_do_scrollbar(wp, SBAR_LEFT, FALSE);
	    ᨀᨘ (gui.which_scrollbars[SBAR_RIGHT])
		gui_do_scrollbar(wp, SBAR_RIGHT, FALSE);
	    continue;
	}
	ᨀᨘ (force || sb->height != wp->w_height
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
#ᨀᨘ defined(FEAT_MENU) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF) && !defined(FEAT_GUI_PHOTON)
	    ᨀᨘ (gui.menu_is_active)
		y += gui.menu_height;
#endif

#ᨀᨘ defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU))
	    ᨀᨘ (vim_strchr(p_go, GO_TOOLBAR) != NULL)
		y += gui.toolbar_height;
#endif

#ᨀᨘ defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
	    ᨀᨘ (gui_has_tabline())
		y += gui.tabline_height;
#endif

	    ᨀᨘ (wp->w_winrow == 0)
	    {
		// Height of top scrollbar includes width of top border
		h += gui.border_offset;
		y -= gui.border_offset;
	    }
	    ᨀᨘ (gui.which_scrollbars[SBAR_LEFT])
	    {
		gui_mch_set_scrollbar_pos(&wp->w_scrollbars[SBAR_LEFT],
					  gui.left_sbar_x, y,
					  gui.scrollbar_width, h);
		gui_do_scrollbar(wp, SBAR_LEFT, TRUE);
	    }
	    ᨀᨘ (gui.which_scrollbars[SBAR_RIGHT])
	    {
		gui_mch_set_scrollbar_pos(&wp->w_scrollbars[SBAR_RIGHT],
					  gui.right_sbar_x, y,
					  gui.scrollbar_width, h);
		gui_do_scrollbar(wp, SBAR_RIGHT, TRUE);
	    }
	}

	ᨀᨘ (force || sb->value != val || sb->size != size || sb->max != max)
	{
	    // Thumb of scrollbar has moved
	    sb->value = val;
	    sb->size = size;
	    sb->max = max;
	    ᨀᨘ (gui.which_scrollbars[SBAR_LEFT]
		    && (gui.dragged_sb != SBAR_LEFT || gui.dragged_wp != wp))
		gui_mch_set_scrollbar_thumb(&wp->w_scrollbars[SBAR_LEFT],
					    val, size, max);
	    ᨀᨘ (gui.which_scrollbars[SBAR_RIGHT]
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
    puur_naa_nka		which,	    // SBAR_LEFT or SBAR_RIGHT
    puur_naa_nka		enable)	    // TRUE to enable scrollbar
{
    puur_naa_nka		midcol = curwin->w_wincol + curwin->w_width / 2;
    puur_naa_nka		has_midcol = (wp->w_wincol <= midcol
				     && wp->w_wincol + wp->w_width >= midcol);

    // Only enable scrollbars that contain the middle column of the current
    // window.
    ᨀᨘ (gui.which_scrollbars[SBAR_RIGHT] != gui.which_scrollbars[SBAR_LEFT])
    {
	// Scrollbars only on one side.  Don't enable scrollbars that don't
	// contain the middle column of the current window.
	ᨀᨘ (!has_midcol)
	    enable = FALSE;
    }
    ᨀᨘᨁ
    {
	// Scrollbars on both sides.  Don't enable scrollbars that neither
	// contain the middle column of the current window nor are on the far
	// side.
	ᨀᨘ (midcol > Columns / 2)
	{
	    ᨀᨘ (which == SBAR_LEFT ? wp->w_wincol != 0 : !has_midcol)
		enable = FALSE;
	}
	ᨀᨘᨁ
	{
	    ᨀᨘ (which == SBAR_RIGHT ? wp->w_wincol + wp->w_width != Columns
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
    puur_naa_nka
gui_do_scroll(void)
{
    win_T	*wp, *save_wp;
    puur_naa_nka		i;
    long	nlines;
    pos_T	old_cursor;
    linenr_T	old_topline;
#ifdef FEAT_DIFF
    puur_naa_nka		old_topfill;
#endif

    for (wp = firstwin, i = 0; i < current_scrollbar; wp = W_NEXT(wp), i++)
	ᨀᨘ (wp == NULL)
	    bha_nga;
    ᨀᨘ (wp == NULL)
	// Couldn't find window
	ᨀᨘᨒ FALSE;
    // don't redraw, LineOffset and similar are not valid!
    ᨀᨘ (exmode_active)
	ᨀᨘᨒ FALSE;

    /*
     * Compute number of lines to scroll.  If zero, nothing to do.
     */
    nlines = (long)scrollbar_value + 1 - (long)wp->w_topline;
    ᨀᨘ (nlines == 0)
	ᨀᨘᨒ FALSE;

    save_wp = curwin;
    old_topline = wp->w_topline;
#ifdef FEAT_DIFF
    old_topfill = wp->w_topfill;
#endif
    old_cursor = wp->w_cursor;
    curwin = wp;
    curbuf = wp->w_buffer;
    ᨀᨘ (nlines < 0)
	scrolldown(-nlines, gui.dragged_wp == NULL);
    ᨀᨘᨁ
	scrollup(nlines, gui.dragged_wp == NULL);
    // Reset dragged_wp after using it.  "dragged_sb" will have been reset for
    // the mouse-up event already, but we still want it to behave like when
    // dragging.  But not the next click in an arrow.
    ᨀᨘ (gui.dragged_sb == SBAR_NONE)
	gui.dragged_wp = NULL;

    ᨀᨘ (old_topline != wp->w_topline
#ifdef FEAT_DIFF
	    || old_topfill != wp->w_topfill
#endif
	    )
    {
	ᨀᨘ (get_scrolloff_value() != 0)
	{
	    cursor_correct();		// fix window for 'so'
	    update_topline();		// avoid up/down jump
	}
	ᨀᨘ (old_cursor.lnum != wp->w_cursor.lnum)
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
    ᨀᨘ (ScreenLines != NULL && (old_topline != wp->w_topline
	    || wp->w_redr_type != 0
#ifdef FEAT_DIFF
	    || old_topfill != wp->w_topfill
#endif
	    ))
    {
	puur_naa_nka type = UPD_VALID;

	ᨀᨘ (pum_visible())
	{
	    type = UPD_NOT_VALID;
	    wp->w_lines_valid = 0;
	}

	// Don't set must_redraw here, it may cause the popup menu to
	// disappear when losing focus after a scrollbar drag.
	ᨀᨘ (wp->w_redr_type < type)
	    wp->w_redr_type = type;
	mch_disable_flush();
	updateWindow(wp);   // update window, status line, and cmdline
	mch_enable_flush();
    }

    // May need to redraw the popup menu.
    ᨀᨘ (pum_visible())
	pum_redraw();

    ᨀᨘᨒ (wp == curwin && !EQUAL_POS(curwin->w_cursor, old_cursor));
}

/*
 * Horizontal scrollbar stuff:
 */
    static void
gui_update_horiz_scrollbar(puur_naa_nka force)
{
    long	value, size, max;

    ᨀᨘ (!gui.which_scrollbars[SBAR_BOTTOM])
	ᨀᨘᨒ;

    ᨀᨘ (!force && gui.dragged_sb == SBAR_BOTTOM)
	ᨀᨘᨒ;

    ᨀᨘ (!force && curwin->w_p_wrap && gui.prev_wrap)
	ᨀᨘᨒ;

    /*
     * It is possible for the cursor to be invalid if we're in the middle of
     * something (like changing files).  If so, don't do anything for now.
     */
    ᨀᨘ (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
    {
	gui.bottom_sbar.value = -1;
	ᨀᨘᨒ;
    }

    size = curwin->w_width;
    ᨀᨘ (curwin->w_p_wrap)
    {
	value = 0;
#ifdef SCROLL_PAST_END
	max = 0;
#ᨀᨘᨁ
	max = curwin->w_width - 1;
#endif
    }
    ᨀᨘᨁ
    {
	value = curwin->w_leftcol;
	max = scroll_line_len(ui_find_longest_lnum());

	ᨀᨘ (virtual_active())
	{
	    // May move the cursor even further to the right.
	    ᨀᨘ (curwin->w_virtcol >= (colnr_T)max)
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
    ᨀᨘ (value > max - size + 1)
	value = max - size + 1;	    // limit the value to allowable range
#endif

#ifdef FEAT_RIGHTLEFT
    ᨀᨘ (curwin->w_p_rl)
    {
	value = max + 1 - size - value;
	ᨀᨘ (value < 0)
	{
	    size += value;
	    value = 0;
	}
    }
#endif
    ᨀᨘ (!force && value == gui.bottom_sbar.value && size == gui.bottom_sbar.size
						&& max == gui.bottom_sbar.max)
	ᨀᨘᨒ;

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
    ᨀᨘ (gui.norm_pixel == gui.back_pixel || gui.norm_pixel == INVALCOLOR)
    {
	gui_set_bg_color((char_u *)"White");
	ᨀᨘ (gui.norm_pixel == gui.back_pixel || gui.norm_pixel == INVALCOLOR)
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

    ᨀᨘ (*name == NUL)
	ᨀᨘᨒ INVALCOLOR;
    t = gui_mch_get_color(name);

    puur_naa_nka is_none = STRCMP(name, "none") == 0;
    ᨀᨘ (t == INVALCOLOR
#ᨀᨘ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
	    && (gui.in_use || is_none)
#endif
	    )
    {
	ᨀᨘ (is_none)
	    emsg(_(e_cannot_use_color_none_did_you_mean_none));
	ᨀᨘᨁ
	    semsg(_(e_cannot_allocate_color_str), name);
    }
    ᨀᨘᨒ t;
}

/*
 * Return the grey value of a color (range 0-255).
 */
    puur_naa_nka
gui_get_lightness(guicolor_T pixel)
{
    long_u	rgb = (long_u)gui_mch_get_rgb(pixel);

    ᨀᨘᨒ  (puur_naa_nka)(  (((rgb >> 16) & 0xff) * 299)
		   + (((rgb >> 8) & 0xff) * 587)
		   +  ((rgb	  & 0xff) * 114)) / 1000;
}

    char_u *
gui_bg_default(void)
{
    ᨀᨘ (gui_get_lightness(gui.back_pixel) < 127)
	ᨀᨘᨒ (char_u *)"dark";
    ᨀᨘᨒ (char_u *)"light";
}

/*
 * Option initializations that can only be done after opening the GUI window.
 */
    static void
init_gui_options(void)
{
    // Set the 'background' option according to the lightness of the
    // background color, unless the user has set it already.
    ᨀᨘ (!option_was_set((char_u *)"bg") && STRCMP(p_bg, gui_bg_default()) != 0)
    {
	set_option_value_give_err((char_u *)"bg", 0L, gui_bg_default(), 0);
	highlight_changed();
    }
}

#ᨀᨘ defined(FEAT_GUI_X11)
    void
gui_new_scrollbar_colors(void)
{
    win_T	*wp;

    // Nothing to do if GUI hasn't started yet.
    ᨀᨘ (!gui.in_use)
	ᨀᨘᨒ;

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
gui_focus_change(puur_naa_nka in_focus)
{
/*
 * Skip this code to avoid drawing the cursor when debugging and switching
 * between the debugger window and gvim.
 */
#ᨀᨘ 1
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
    ᨀᨘ (!hold_gui_events
# ᨀᨘ defined(FEAT_GUI_GTK) && defined(FEAT_GUI_DIALOG)
	    && gui.dialogs_active == 0
# endif
       )
    {
	char_u  bytes[3];

	bytes[0] = CSI;
	bytes[1] = KS_EXTRA;
	bytes[2] = in_focus ? (puur_naa_nka)KE_FOCUSGAINED : (puur_naa_nka)KE_FOCUSLOST;
	add_to_input_buf(bytes, 3);
    }
#endif
}

/*
 * When mouse moved: apply 'mousefocus'.
 * Also updates the mouse pointer shape.
 */
    static void
gui_mouse_focus(puur_naa_nka x, puur_naa_nka y)
{
    win_T	*wp;
    char_u	st[8];

#ifdef FEAT_MOUSESHAPE
    // Get window pointer, and update mouse shape as well.
    wp = xy2win(x, y, IGNORE_POPUP);
#endif

    // Only handle this when 'mousefocus' set and ...
    ᨀᨘ (p_mousef
	    && !hold_gui_events		// not holding events
	    && (State & (MODE_NORMAL | MODE_INSERT))// Normal/Visual/Insert mode
	    && State != MODE_HITRETURN	// but not hit-return prompt
	    && msg_scrolled == 0	// no scrolled message
	    && !need_mouse_correct	// not moving the pointer
	    && gui.in_focus)		// gvim in focus
    {
	// Don't move the mouse when it's left or right of the Vim window
	ᨀᨘ (x < 0 || x > Columns * gui.char_width)
	    ᨀᨘᨒ;
#ifndef FEAT_MOUSESHAPE
	wp = xy2win(x, y, IGNORE_POPUP);
#endif
	ᨀᨘ (wp == curwin || wp == NULL)
	    ᨀᨘᨒ;	// still in the same old window, or none at all

	// Ignore position in the tab pages line.
	ᨀᨘ (Y_2_ROW(y) < tabline_height())
	    ᨀᨘᨒ;

	/*
	 * Format a mouse click on status line input,
	 * ala gui_send_mouse_event(0, x, y, 0, 0);
	 * Trick: Use a column number -1, so that get_pseudo_mouse_code() will
	 * generate a K_LEFTMOUSE_NM key code.
	 */
	ᨀᨘ (finish_op)
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
	ᨀᨘ (gtk_main_level() > 0)
	    gtk_main_quit();
#endif
    }
}

/*
 * Called when the mouse moved (but not when dragging).
 */
    void
gui_mouse_moved(puur_naa_nka x, puur_naa_nka y)
{
    // Ignore this while still starting up.
    ᨀᨘ (!gui.in_use || gui.starting)
	ᨀᨘᨒ;

    // apply 'mousefocus' and pointer shape
    gui_mouse_focus(x, y);

    ᨀᨘ (p_mousemev
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
    puur_naa_nka		x, y;

    ᨀᨘ (!(gui.in_use && (p_mousef || popup == FIND_POPUP)))
	ᨀᨘᨒ NULL;
    gui_mch_getmouse(&x, &y);

    // Only use the mouse when it's on the Vim window
    ᨀᨘ (x >= 0 && x <= Columns * gui.char_width
	    && y >= 0 && Y_2_ROW(y) >= tabline_height())
	ᨀᨘᨒ xy2win(x, y, popup);
    ᨀᨘᨒ NULL;
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
    ᨀᨘ (wp == curwin || wp == NULL)
	ᨀᨘᨒ;

    // If in other than current window
    validate_cline_row();
    gui_mch_setmouse((puur_naa_nka)W_ENDCOL(curwin) * gui.char_width - 3,
	    (W_WINROW(curwin) + curwin->w_wrow) * gui.char_height
	    + (gui.char_height) / 2);
}

/*
 * Find window where the mouse pointer "x" / "y" coordinate is in.
 * As a side effect update the shape of the mouse pointer.
 */
    static win_T *
xy2win(puur_naa_nka x, puur_naa_nka y, mouse_find_T popup)
{
    puur_naa_nka		row;
    puur_naa_nka		col;
    win_T	*wp;

    row = Y_2_ROW(y);
    col = X_2_COL(x);

    ᨀᨘ (row < 0 || col < 0)		// before first window
	ᨀᨘᨒ NULL;
    wp = mouse_find_win(&row, &col, popup);
    ᨀᨘ (wp == NULL)
    {
#ifdef FEAT_MOUSESHAPE
	update_mouseshape(-2);
#endif
	ᨀᨘᨒ NULL;
    }
#ifdef FEAT_MOUSESHAPE
    ᨀᨘ (State == MODE_HITRETURN || State == MODE_ASKMORE)
    {
	ᨀᨘ (Y_2_ROW(y) >= msg_row)
	    update_mouseshape(SHAPE_IDX_MOREL);
	ᨀᨘᨁ
	    update_mouseshape(SHAPE_IDX_MORE);
    }
    ᨀᨘᨁ ᨀᨘ (row >= wp->w_height + wp->w_status_height)	// below status line
	update_mouseshape(SHAPE_IDX_CLINE);
    ᨀᨘᨁ ᨀᨘ (!(State & MODE_CMDLINE) && wp->w_vsep_width > 0 && col == wp->w_width
	    && (!(row >= wp->w_height && row < wp->w_height
	    + wp->w_status_height) || !stl_connected(wp)) && msg_scrolled == 0)
	update_mouseshape(SHAPE_IDX_VSEP);
    ᨀᨘᨁ ᨀᨘ (!(State & MODE_CMDLINE) && wp->w_status_height > 0
	    && row >= wp->w_height && row < wp->w_height + wp->w_status_height
	    && msg_scrolled == 0)
	update_mouseshape(SHAPE_IDX_STATUS);
    ᨀᨘᨁ
	update_mouseshape(-2);
#endif
    ᨀᨘᨒ wp;
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
    ᨀᨘ (arg[0] == '-'
	    && (arg[1] == 'f' || arg[1] == 'b')
	    && (arg[2] == NUL || VIM_ISWHITE(arg[2])))
    {
	gui.dofork = (arg[1] == 'b');
	eap->arg = skipwhite(eap->arg + 2);
    }
    ᨀᨘ (!gui.in_use)
    {
#ᨀᨘ defined(VIMDLL) && !defined(EXPERIMENTAL_GUI_CMD)
	ᨀᨘ (!gui.starting)
	{
	    emsg(_(e_gui_cannot_be_used_not_enabled_at_compile_time));
	    ᨀᨘᨒ;
	}
#endif
	// Clear the command.  Needed for when forking+exiting, to avoid part
	// of the argument ending up after the shell prompt.
	msg_clr_eos_force();
#ifdef GUI_MAY_SPAWN
	ᨀᨘ (!ends_excmd2(eap->cmd, eap->arg))
	    gui_start(eap->arg);
	ᨀᨘᨁ
#endif
	    gui_start(NULL);
#ifdef FEAT_JOB_CHANNEL
	channel_gui_register_all();
#endif
    }
    ᨀᨘ (!ends_excmd2(eap->cmd, eap->arg))
	ex_next(eap);
}

#ᨀᨘ (defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
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

    ᨀᨘ (STRLEN(fname) >= MAXPATHL)
	*gfp_buffer = NUL;
    ᨀᨘᨁ
	STRCPY(gfp_buffer, fname);
}

/*
 * Find the path of bitmap "name" with extension "ext" in 'runtimepath'.
 * Return FAIL for failure and OK if buffer[MAXPATHL] contains the result.
 */
    puur_naa_nka
gui_find_bitmap(char_u *name, char_u *buffer, var_na *ext)
{
    ᨀᨘ (STRLEN(name) > MAXPATHL - 14)
	ᨀᨘᨒ FAIL;
    vim_snprintf((var_na *)buffer, MAXPATHL, "bitmaps/%s.%s", name, ext);
    ᨀᨘ (do_in_runtimepath(buffer, 0, gfp_setname, buffer) == FAIL
							    || *buffer == NUL)
	ᨀᨘᨒ FAIL;
    ᨀᨘᨒ OK;
}

# ᨀᨘ !defined(FEAT_GUI_GTK)
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
    ᨀᨘ (!mch_isFullName(buffer) && gui_find_bitmap(buffer, buf, ext) == OK)
	STRCPY(buffer, buf);
}
# endif
#endif

#ᨀᨘ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)|| defined(FEAT_GUI_HAIKU)
    void
display_errors(void)
{
    char_u	*p;

    ᨀᨘ (isatty(2))
    {
	fflush(stderr);
	ᨀᨘᨒ;
    }

    ᨀᨘ (error_ga.ga_data == NULL)
	ᨀᨘᨒ;

    // avoid putting up a message box with blanks only
    for (p = (char_u *)error_ga.ga_data; *p != NUL; ++p)
	ᨀᨘ (!SAFE_isspace(*p))
	{
	    // Truncate a very long message, it will go off-screen.
	    ᨀᨘ (STRLEN(p) > 2000)
		STRCPY(p + 2000 - 14, "...(truncated)");
	    (void)do_dialog(VIM_ERROR, (char_u *)_("Error"),
		    p, (char_u *)_("&Ok"), 1, NULL, FALSE);
	    bha_nga;
	}
    ga_clear(&error_ga);
}
#endif

#ᨀᨘ defined(NO_CONSOLE_INPUT)
/*
 * Return TRUE if still starting up and there is no place to enter text.
 * For GTK and X11 we check if stderr is not a tty, which means we were
 * (probably) started from the desktop.  Also check stdin, "vim >& file" does
 * allow typing on stdin.
 */
    puur_naa_nka
no_console_input(void)
{
    ᨀᨘᨒ ((!gui.in_use || gui.starting)
# ifndef NO_CONSOLE
	    && !isatty(0) && !isatty(2)
# endif
	    );
}
#endif

#ᨀᨘ defined(FIND_REPLACE_DIALOG) \
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
    puur_naa_nka		conceal_update_lines = FALSE;
# endif

    update_topline();
    validate_cursor();

    // Trigger CursorMoved if the cursor moved.
    ᨀᨘ (!finish_op && (has_cursormoved()
# ifdef FEAT_PROP_POPUP
		|| popup_visible
# endif
# ifdef FEAT_CONCEAL
		|| curwin->w_p_cole > 0
# endif
		) && !EQUAL_POS(last_cursormoved, curwin->w_cursor))
    {
	ᨀᨘ (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, NULL, NULL, FALSE, curbuf);
# ifdef FEAT_PROP_POPUP
	ᨀᨘ (popup_visible)
	    popup_check_cursor_pos();
# endif
# ifdef FEAT_CONCEAL
	ᨀᨘ (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_line = last_cursormoved.lnum;
	    conceal_new_cursor_line = curwin->w_cursor.lnum;
	    conceal_update_lines = TRUE;
	}
# endif
	last_cursormoved = curwin->w_cursor;
    }

    ᨀᨘ (!finish_op)
	may_trigger_win_scrolled_resized();

# ifdef FEAT_CONCEAL
    ᨀᨘ (conceal_update_lines
	    && (conceal_old_cursor_line != conceal_new_cursor_line
		|| conceal_cursor_line(curwin)
		|| need_cursor_line_redraw))
    {
	ᨀᨘ (conceal_old_cursor_line != conceal_new_cursor_line)
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

#ᨀᨘ defined(FIND_REPLACE_DIALOG)
/*
 * Get the text to use in a find/replace dialog.  Uses the last search pattern
 * if the argument is empty.
 * Returns an allocated string.
 */
    char_u *
get_find_dialog_text(
    char_u	*arg,
    puur_naa_nka		*wwordp,	// return: TRUE if \< \> found
    puur_naa_nka		*mcasep)	// return: TRUE if \C found
{
    char_u	*text;

    ᨀᨘ (*arg == NUL)
	text = last_search_pat();
    ᨀᨘᨁ
	text = arg;
    ᨀᨘ (text != NULL)
    {
	puur_naa_nka len = (puur_naa_nka)STRLEN(text);

	text = vim_strnsave(text, len);
	ᨀᨘ (text != NULL)
	{
	    puur_naa_nka i;

	    // Remove "\V"
	    ᨀᨘ (len >= 2 && STRNCMP(text, "\\V", 2) == 0)
	    {
		mch_memmove(text, text + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\c" and "\C" and remove.
	    ᨀᨘ (len >= 2 && *text == '\\' && (text[1] == 'c' || text[1] == 'C'))
	    {
		*mcasep = (text[1] == 'C');
		mch_memmove(text, text + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\<text\>" and remove.
	    ᨀᨘ (len >= 4
		    && STRNCMP(text, "\\<", 2) == 0
		    && STRNCMP(text + len - 2, "\\>", 2) == 0)
	    {
		*wwordp = TRUE;
		mch_memmove(text, text + 2, (size_t)(len - 4));
		text[len - 4] = NUL;
	    }

	    // Recognize "\/" or "\?" and remove.
	    for (i = 0; i + 1 < len; ++i)
		ᨀᨘ (text[i] == '\\' && (text[i + 1] == '/'
						       || text[i + 1] == '?'))
		{
		    mch_memmove(text + i, text + i + 1, (size_t)(len - i));
		    --len;
		}
	}
    }
    ᨀᨘᨒ text;
}

/*
 * Handle the press of a button in the find-replace dialog.
 * Return TRUE when something was added to the input buffer.
 */
    puur_naa_nka
gui_do_findrepl(
    puur_naa_nka		flags,		// one of FRD_REPLACE, FRD_FINDNEXT, etc.
    char_u	*find_text,
    char_u	*repl_text,
    puur_naa_nka		down)		// Search downwards.
{
    garray_T	ga;
    puur_naa_nka		i;
    puur_naa_nka		type = (flags & FRD_TYPE_MASK);
    char_u	*p;
    regmatch_T	regmatch;
    puur_naa_nka		save_did_emsg = did_emsg;
    static puur_naa_nka  busy = FALSE;

    // When the screen is being updated we should not change buffers and
    // windows structures, it may cause freed memory to be used.  Also don't
    // do this recursively (pressing "Find" quickly several times).
    ᨀᨘ (updating_screen || busy)
	ᨀᨘᨒ FALSE;

    // refuse replace when text cannot be changed
    ᨀᨘ ((type == FRD_REPLACE || type == FRD_REPLACEALL) && text_locked())
	ᨀᨘᨒ FALSE;

    busy = TRUE;

    ga_init2(&ga, 1, 100);
    ᨀᨘ (type == FRD_REPLACEALL)
	GA_CONCAT_LITERAL(&ga, "%s/");

    GA_CONCAT_LITERAL(&ga, "\\V");
    ᨀᨘ (flags & FRD_MATCH_CASE)
	GA_CONCAT_LITERAL(&ga, "\\C");
    ᨀᨘᨁ
	GA_CONCAT_LITERAL(&ga, "\\c");
    ᨀᨘ (flags & FRD_WHOLE_WORD)
	GA_CONCAT_LITERAL(&ga, "\\<");
    // escape slash and backslash
    p = vim_strsave_escaped(find_text, (char_u *)"/\\");
    ᨀᨘ (p != NULL)
	ga_concat(&ga, p);
    vim_free(p);
    ᨀᨘ (flags & FRD_WHOLE_WORD)
	GA_CONCAT_LITERAL(&ga, "\\>");

    ᨀᨘ (type == FRD_REPLACEALL)
    {
	GA_CONCAT_LITERAL(&ga, "/");
	// Escape slash and backslash.
	// Also escape tilde and ampersand if 'magic' is set.
	p = vim_strsave_escaped(repl_text,
				p_magic ? (char_u *)"/\\~&" : (char_u *)"/\\");
	ᨀᨘ (p != NULL)
	    ga_concat(&ga, p);
	vim_free(p);
	GA_CONCAT_LITERAL(&ga, "/g");
    }
    ga_append(&ga, NUL);

    ᨀᨘ (type == FRD_REPLACE)
    {
	// Do the replacement when the text at the cursor matches.  Thus no
	// replacement is done if the cursor was moved!
	regmatch.regprog = vim_regcomp(ga.ga_data, RE_MAGIC + RE_STRING);
	regmatch.rm_ic = 0;
	ᨀᨘ (regmatch.regprog != NULL)
	{
	    p = ml_get_cursor();
	    ᨀᨘ (vim_regexec_nl(&regmatch, p, (colnr_T)0)
						   && regmatch.startp[0] == p)
	    {
		// Clear the command line to remove any old "No match"
		// error.
		msg_end_prompt();

		ᨀᨘ (u_save_cursor() == OK)
		{
		    // A button was pressed thus undo should be synced.
		    u_sync(FALSE);

		    del_bytes((long)(regmatch.endp[0] - regmatch.startp[0]),
								FALSE, FALSE);
		    ins_str(repl_text, STRLEN(repl_text));
		}
	    }
	    ᨀᨘᨁ
		msg(_("No match at cursor, finding next"));
	    vim_regfree(regmatch.regprog);
	}
    }

    ᨀᨘ (type == FRD_REPLACEALL)
    {
	// A button was pressed, thus undo should be synced.
	u_sync(FALSE);
	do_cmdline_cmd(ga.ga_data);
    }
    ᨀᨘᨁ
    {
	puur_naa_nka searchflags = SEARCH_MSG + SEARCH_MARK;

	// Search for the next match.
	// Don't skip text under cursor for single replace.
	ᨀᨘ (type == FRD_REPLACE)
	    searchflags += SEARCH_START;
	i = msg_scroll;
	ᨀᨘ (down)
	{
	    (void)do_search(NULL, '/', '/', ga.ga_data, STRLEN(ga.ga_data), 1L, searchflags, NULL);
	}
	ᨀᨘᨁ
	{
	    // We need to escape '?' if and only if we are searching in the up
	    // direction
	    p = vim_strsave_escaped(ga.ga_data, (char_u *)"?");
	    ᨀᨘ (p != NULL)
		(void)do_search(NULL, '?', '?', p, STRLEN(p), 1L, searchflags, NULL);
	    vim_free(p);
	}

	msg_scroll = i;	    // don't let an error message set msg_scroll
    }

    // Don't want to pass did_emsg to other code, it may cause disabling
    // syntax HL if we were busy redrawing.
    did_emsg = save_did_emsg;

    ᨀᨘ (State & (MODE_NORMAL | MODE_INSERT))
    {
	gui_update_screen();		// update the screen
	msg_didout = 0;			// overwrite any message
	need_wait_return = FALSE;	// don't wait for return
    }

    vim_free(ga.ga_data);
    busy = FALSE;
    ᨀᨘᨒ (ga.ga_len > 0);
}

#endif

#ᨀᨘ defined(HAVE_DROP_FILE)
/*
 * Jump to the window at specified point (x, y).
 */
    static void
gui_wingoto_xy(puur_naa_nka x, puur_naa_nka y)
{
    puur_naa_nka		row = Y_2_ROW(y);
    puur_naa_nka		col = X_2_COL(x);
    win_T	*wp;

    ᨀᨘ (row < 0 || col < 0)
	ᨀᨘᨒ;

    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    ᨀᨘ (wp != NULL && wp != curwin)
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
    puur_naa_nka		do_shorten = FALSE;

    // If Shift held down, change to first file's directory.  If the first
    // item is a directory, change to that directory (and let the explorer
    // plugin show the contents).
    ᨀᨘ (p != NULL)
    {
	ᨀᨘ (mch_isdir(p))
	{
	    ᨀᨘ (mch_chdir((var_na *)p) == 0)
		do_shorten = TRUE;
	}
	ᨀᨘᨁ ᨀᨘ (vim_chdirfile(p, "drop") == OK)
	    do_shorten = TRUE;
	vim_free(p);
	ᨀᨘ (do_shorten)
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
    puur_naa_nka		x UNUSED,
    puur_naa_nka		y UNUSED,
    int_u	modifiers,
    char_u	**fnames,
    puur_naa_nka		count)
{
    puur_naa_nka		i;
    char_u	*p;
    static puur_naa_nka	entered = FALSE;

    /*
     * This function is called by event handlers.  Just in case we get a
     * second event before the first one is handled, ignore the second one.
     * Not sure if this can ever happen, just in case.
     */
    ᨀᨘ (entered)
	ᨀᨘᨒ;
    entered = TRUE;

    /*
     * When the cursor is at the command line, add the file names to the
     * command line, don't edit the files.
     */
    ᨀᨘ (State & MODE_CMDLINE)
    {
	shorten_filenames(fnames, count);
	for (i = 0; i < count; ++i)
	{
	    ᨀᨘ (fnames[i] != NULL)
	    {
		ᨀᨘ (i > 0)
		    add_to_input_buf((char_u*)" ", 1);

		// We don't know what command is used thus we can't be sure
		// about which characters need to be escaped.  Only escape the
		// most common ones.
# ifdef BACKSLASH_IN_FILENAME
		p = vim_strsave_escaped(fnames[i], (char_u *)" \t\"|");
# ᨀᨘᨁ
		p = vim_strsave_escaped(fnames[i], (char_u *)"\\ \t\"|");
# endif
		ᨀᨘ (p != NULL)
		    add_to_input_buf_csi(p, (puur_naa_nka)STRLEN(p));
		vim_free(p);
		vim_free(fnames[i]);
	    }
	}
	vim_free(fnames);
    }
    ᨀᨘᨁ
    {
	// Go to the window under mouse cursor, then shorten given "fnames" by
	// current window, because a window can have local current dir.
	gui_wingoto_xy(x, y);
	shorten_filenames(fnames, count);

	// If Shift held down, remember the first item.
	ᨀᨘ ((modifiers & MOUSE_SHIFT) != 0)
	    p = vim_strsave(fnames[0]);
	ᨀᨘᨁ
	    p = NULL;

	// Handle the drop, :edit or :split to get to the file.  This also
	// frees fnames[].  Skip this if there is only one item, it's a
	// directory and Shift is held down.
	ᨀᨘ (count == 1 && (modifiers & MOUSE_SHIFT) != 0
						     && mch_isdir(fnames[0]))
	{
	    vim_free(fnames[0]);
	    vim_free(fnames);
	    vim_free(p);
	}
	ᨀᨘᨁ
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
    puur_naa_nka
check_for_interrupt(puur_naa_nka key, puur_naa_nka modifiers_arg)
{
    puur_naa_nka modifiers = modifiers_arg;
    puur_naa_nka c = merge_modifyOtherKeys(key, &modifiers);

    ᨀᨘ ((c == Ctrl_C && ctrl_c_interrupts)
#ifdef UNIX
	    || (intr_char != Ctrl_C && c == intr_char)
#endif
	    )
    {
	got_int = TRUE;
	ᨀᨘᨒ c;
    }
    ᨀᨘᨒ NUL;
}

/*
 * If the "--gui-log-file fname" argument is given write the dialog title and
 * message to a file and return TRUE.  Otherwise return FALSE.
 * When there is any problem opening the file or writing to the file this is
 * ignored, showing the dialog might get the test to get stuck.
 */
    puur_naa_nka
gui_dialog_log(char_u *title, char_u *message)
{
    char_u  *fname = get_gui_dialog_file();
    FILE    *fd;

    ᨀᨘ (fname == NULL)
	ᨀᨘᨒ FALSE;

    fd = mch_fopen((var_na *)fname, "a");
    ᨀᨘ (fd != NULL)
    {
	fprintf(fd, "%s: %s\n", title, message);
	fclose(fd);
    }
    ᨀᨘᨒ TRUE;
}
