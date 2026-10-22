/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#અન્તર્ભાવ "vim.h"

// Structure containing all the GUI information
gui_T gui;

#જો defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
# define USE_SET_GUIFONTWIDE
સ્થિર શૂન્ય set_guifontwide(char_u *font_name);
#endif
સ્થિર શૂન્ય gui_check_pos(શૂન્ય);
સ્થિર શૂન્ય gui_reset_scroll_region(શૂન્ય);
સ્થિર શૂન્ય gui_outstr(char_u *, પૂર્ણાંક);
#ifndef USE_GTK4
સ્થિર પૂર્ણાંક gui_screenchar(પૂર્ણાંક off, પૂર્ણાંક flags, guicolor_T fg, guicolor_T bg, પૂર્ણાંક back);
#endif
સ્થિર પૂર્ણાંક gui_outstr_nowrap(char_u *s, પૂર્ણાંક len, પૂર્ણાંક flags, guicolor_T fg, guicolor_T bg, પૂર્ણાંક back);
સ્થિર શૂન્ય gui_delete_લીટીઓ(પૂર્ણાંક row, પૂર્ણાંક ગણતરી);
સ્થિર શૂન્ય gui_ઉમેરો_લીટીઓ(પૂર્ણાંક row, પૂર્ણાંક ગણતરી);
સ્થિર પૂર્ણાંક gui_xy2colrow(પૂર્ણાંક x, પૂર્ણાંક y, પૂર્ણાંક *colp);
#જો defined(FEAT_GUI_TABLINE)
સ્થિર પૂર્ણાંક gui_has_tabline(શૂન્ય);
#endif
સ્થિર શૂન્ય gui_do_scrollbar(win_T *wp, પૂર્ણાંક which, પૂર્ણાંક enable);
સ્થિર શૂન્ય gui_update_horiz_scrollbar(પૂર્ણાંક);
સ્થિર શૂન્ય gui_set_fg_color(char_u *name);
સ્થિર શૂન્ય gui_set_bg_color(char_u *name);
સ્થિર શૂન્ય init_gui_options(શૂન્ય);
સ્થિર win_T *xy2win(પૂર્ણાંક x, પૂર્ણાંક y, mouse_find_T popup);

#ifdef GUI_MAY_FORK
સ્થિર શૂન્ય gui_do_fork(શૂન્ય);

સ્થિર પૂર્ણાંક gui_read_child_pipe(પૂર્ણાંક fd);

// Return values for gui_read_child_pipe
ગણના {
    GUI_CHILD_IO_ERROR,
    GUI_CHILD_OK,
    GUI_CHILD_FAILED
};
#endif

સ્થિર શૂન્ય gui_attempt_start(શૂન્ય);

સ્થિર પૂર્ણાંક can_update_cursor = TRUE; // can display the cursor
સ્થિર પૂર્ણાંક disable_flush = 0;	// If > 0, gui_mch_flush() is disabled.

/*
 * gui_start -- Called when user wants to start the GUI.
 *
 * Careful: This function can be called recursively when there is a ":gui"
 * command in the .gvimrc file.  Only the first call should fork, not the
 * recursive call.
 */
    શૂન્ય
gui_start(char_u *arg UNUSED)
{
    char_u	*old_term;
#ifdef GUI_MAY_FORK
    સ્થિર પૂર્ણાંક	recursive = 0;
#endif
#જો defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
    અક્ષર	*સંદેશ = ખાલી;
#endif

    old_term = vim_strsave(T_NAME);

    settmode(TMODE_COOK);		// stop RAW mode
    જો (full_screen)
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
    જો (gui.dofork && !vim_strchr(p_go, GO_FORG) && recursive <= 1
# ifdef FEAT_JOB_CHANNEL
	    && !job_any_running()
# endif
	    )
    {
	gui_do_fork();
    }
    નહીંતર
#endif
#ifdef GUI_MAY_SPAWN
    જો (gui.dospawn
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
	સંદેશ =
# endif
	    gui_mch_do_spawn(arg);
    }
    નહીંતર
#endif
    {
#ifdef FEAT_GUI_GTK
	// If there is 'f' in 'guioptions' and specify -g argument,
	// gui_mch_init_check() was not called yet.
	જો (gui_mch_init_check() != OK)
	    getout_preserve_modified(1);
#endif
	gui_attempt_start();
    }

    જો (!gui.in_use)			// failed to start GUI
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
#જો defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
	જો (સંદેશ != ખાલી)
	    emsg(સંદેશ);
#endif
    }
    નહીંતર
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

#જો defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    // Enable fullscreen mode
    જો (vim_strchr(p_go, GO_FULLSCREEN) != ખાલી)
       gui_mch_set_fullscreen(TRUE);
#endif

    vim_free(old_term);

    // If the GUI started successfully, trigger the GUIEnter event, otherwise
    // the GUIFailed event.
    gui_mch_update();
    apply_autocmds(gui.in_use ? EVENT_GUIENTER : EVENT_GUIFAILED,
						   ખાલી, ખાલી, FALSE, curbuf);
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
    સ્થિર શૂન્ય
gui_attempt_start(શૂન્ય)
{
    સ્થિર પૂર્ણાંક recursive = 0;

    ++recursive;
    gui.starting = true;

#ifdef FEAT_GUI_GTK
    gui.event_time = GDK_CURRENT_TIME;
#endif

    termcapinit((char_u *)"builtin_gui");
    gui.starting = recursive - 1;

#જો defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
    જો (gui.in_use)
    {
# ifdef FEAT_EVAL
	Window	x11_window;
	Display	*x11_display;

	જો (gui_get_x11_windis(&x11_window, &x11_display) == OK)
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
# જો defined(HAVE_SYS_WAIT_H)
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
    સ્થિર શૂન્ય
gui_do_fork(શૂન્ય)
{
    પૂર્ણાંક		pipefd[2];	// pipe between parent and child
    પૂર્ણાંક		pipe_error;
    પૂર્ણાંક		status;
    પૂર્ણાંક		exit_status;
    pid_t	pid = -1;

# જો defined(FEAT_RELTIME) && defined(PROF_NSEC)
    // a timer is not carried forward
    delete_timer();
# endif

    // Setup a pipe between the child and the parent, so that the parent
    // knows when the child has done the setsid() call and is allowed to
    // exit.
    pipe_error = (pipe(pipefd) < 0);
    pid = fork();
    જો (pid < 0)	    // Fork error
    {
	emsg(_(e_failed_to_create_new_process_for_GUI));
	પાછા;
    }
    નહીંતર જો (pid > 0)	    // Parent
    {
	// Give the child some time to do the setsid(), otherwise the
	// exit() may kill the child too (when starting gvim from inside a
	// gvim).
	જો (!pipe_error)
	{
	    // The read returns when the child closes the pipe (or when
	    // the child dies for some reason).
	    close(pipefd[1]);
	    status = gui_read_child_pipe(pipefd[0]);
	    જો (status == GUI_CHILD_FAILED)
	    {
		// The child failed to start the GUI, so the caller must
		// continue. There may be more error information written
		// to stderr by the child.
		waitpid(pid, &exit_status, 0);
		emsg(_(e_the_child_process_failed_to_start_GUI));
		પાછા;
	    }
	    નહીંતર જો (status == GUI_CHILD_IO_ERROR)
	    {
		pipe_error = TRUE;
	    }
	    // else GUI_CHILD_OK: parent exit
	}

	જો (pipe_error)
	    ui_delay(301L, TRUE);

	// When swapping screens we may need to go to the next line, e.g.,
	// after a hit-enter prompt and using ":gui".
	જો (newline_on_exit)
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
    જો (gui_mch_init_check() != OK)
	getout_preserve_modified(1);
# endif

# જો defined(HAVE_SETSID) || defined(HAVE_SETPGID)
    /*
     * Change our process group.  On some systems/shells a CTRL-C in the
     * shell where Vim was started would otherwise kill gvim!
     */
#  જો defined(HAVE_SETSID)
    (શૂન્ય)setsid();
#  નહીંતર
    (શૂન્ય)setpgid(0, 0);
#  endif
# endif
    જો (!pipe_error)
	close(pipefd[0]);

# જો defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
    // Tell the session manager our new PID
    gui_mch_forked();
# endif

    // Try to start the GUI
    gui_attempt_start();

    // Notify the parent
    જો (!pipe_error)
    {
	જો (gui.in_use)
	    write_eintr(pipefd[1], "ok", 3);
	નહીંતર
	    write_eintr(pipefd[1], "fail", 5);
	close(pipefd[1]);
    }

    // If we failed to start the GUI, exit now.
    જો (!gui.in_use)
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
    સ્થિર પૂર્ણાંક
gui_read_child_pipe(પૂર્ણાંક fd)
{
    long	bytes_read;
# define READ_BUFFER_SIZE 10
    અક્ષર	buffer[READ_BUFFER_SIZE];

    bytes_read = read_eintr(fd, buffer, READ_BUFFER_SIZE - 1);
# undef READ_BUFFER_SIZE
    close(fd);
    જો (bytes_read < 0)
	પાછા GUI_CHILD_IO_ERROR;
    buffer[bytes_read] = NUL;
    જો (STRCMP(buffer, "ok") == 0)
	પાછા GUI_CHILD_OK;
    પાછા GUI_CHILD_FAILED;
}

#endif // GUI_MAY_FORK

/*
 * Call this when vim starts up, whether or not the GUI is started
 */
    શૂન્ય
gui_prepare(પૂર્ણાંક *argc, અક્ષર **argv)
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
    પૂર્ણાંક
gui_init_check(શૂન્ય)
{
    સ્થિર પૂર્ણાંક result = MAYBE;

    જો (result != MAYBE)
    {
	જો (result == FAIL)
	    emsg(_(e_cannot_start_the_GUI));
	પાછા result;
    }

    gui.shell_created = false;
    gui.dying = false;
    gui.in_focus = true;		// so the guicursor setting works
    gui.dragged_sb = SBAR_NONE;
    gui.dragged_wp = ખાલી;
    gui.pointer_hidden = false;
    gui.col = 0;
    gui.row = 0;
    gui.અંક_cols = Columns;
    gui.અંક_rows = Rows;

    gui.cursor_is_valid = false;
    gui.scroll_region_ટોચ = 0;
    gui.scroll_region_bot = Rows - 1;
    gui.scroll_region_ડાબું = 0;
    gui.scroll_region_જમણું = Columns - 1;
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
#  નહીંતર
    gui.menu_font = NOFONT;
#  endif
# endif
    gui.menu_is_active = true;	    // default: include menu
# ifndef FEAT_GUI_GTK
    gui.menu_height = MENU_DEFAULT_HEIGHT;
    gui.menu_width = 0;
# endif
#endif
#જો defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    gui.toolbar_height = 0;
#endif
#ifdef FEAT_BEVAL_TIP
    gui.tooltip_fontset = NOFONTSET;
#endif

    gui.scrollbar_width = gui.scrollbar_height = SB_DEFAULT_WIDTH;
    gui.prev_wrap = -1;

#જો defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // Note: gui_set_ligatures() might already have been called e.g. from .vimrc,
    // and in that case we don't want to overwrite ligatures map that has already
    // been correctly populated (as that would lead to a cleared ligatures maps).
    જો (*p_guiligatures == NUL)
	CLEAR_FIELD(gui.ligatures_map);
#endif

#જો defined(ALWAYS_USE_GUI) || defined(VIMDLL)
    result = OK;
#નહીંતર
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
# નહીંતર
    result = gui_mch_init_check();
# endif
#endif
    પાછા result;
}

/*
 * This is the call which starts the GUI.
 */
    શૂન્ય
gui_init(શૂન્ય)
{
    win_T	*wp;
    tabpage_T	*tp;
    સ્થિર પૂર્ણાંક	recursive = 0;

    /*
     * It's possible to use ":gui" in a .gvimrc file.  The first half of this
     * function will then be executed at the first call, the rest by the
     * recursive call.  This allow the shell to be opened halfway reading a
     * gvimrc file.
     */
    જો (!recursive)
    {
	++recursive;

	clip_init(TRUE);

	// If can't initialize, don't try doing the rest
	જો (gui_init_check() == FAIL)
	{
	    --recursive;
	    clip_init(FALSE);
	    પાછા;
	}

	/*
	 * Reset 'paste'.  It's useful in the terminal, but not in the GUI.  It
	 * breaks the Paste toolbar button.
	 */
	set_option_કિંમત_give_err((char_u *)"paste", 0L, ખાલી, 0);

	// Set t_Co to the number of colors: RGB.
	set_color_ગણતરી(256 * 256 * 256);

	/*
	 * Set up system-wide default menus.
	 */
#જો defined(SYS_MENU_FILE) && defined(FEAT_MENU)
	જો (vim_strchr(p_go, GO_NOSYSMENU) == ખાલી)
	{
	    sys_menu = TRUE;
	    do_source((char_u *)SYS_MENU_FILE, FALSE, DOSO_NONE, ખાલી);
	    sys_menu = FALSE;
	}
#endif

	/*
	 * Switch on the mouse by default, unless the user changed it already.
	 * This can then be changed in the .gvimrc.
	 */
	જો (!option_was_set((char_u *)"mouse"))
	    set_string_option_direct((char_u *)"mouse", -1,
					   (char_u *)"a", OPT_FREE, SID_NONE);

	/*
	 * If -U option given, use only the initializations from that file and
	 * nothing else.  Skip all initializations for "-U NONE" or "-u NORC".
	 */
	જો (use_gvimrc != ખાલી)
	{
	    જો (STRCMP(use_gvimrc, "NONE") != 0
		    && STRCMP(use_gvimrc, "NORC") != 0
		    && do_source(use_gvimrc, FALSE, DOSO_NONE, ખાલી) != OK)
		semsg(_(e_cannot_read_from_str), use_gvimrc);
	}
	નહીંતર
	{
	    /*
	     * Get system wide defaults for gvim, only when file name defined.
	     */
#ifdef SYS_GVIMRC_FILE
	    do_source((char_u *)SYS_GVIMRC_FILE, FALSE, DOSO_NONE, ખાલી);
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
	    જો (process_env((char_u *)"GVIMINIT", FALSE) == FAIL
		 && do_source((char_u *)USR_GVIMRC_FILE, TRUE,
						     DOSO_GVIMRC, ખાલી) == FAIL
#ifdef USR_GVIMRC_FILE2
		 && do_source((char_u *)USR_GVIMRC_FILE2, TRUE,
						     DOSO_GVIMRC, ખાલી) == FAIL
#endif
#ifdef USR_GVIMRC_FILE3
		 && do_source((char_u *)USR_GVIMRC_FILE3, TRUE,
						     DOSO_GVIMRC, ખાલી) == FAIL
#endif
				)
	    {
#ifdef USR_GVIMRC_FILE4
		(શૂન્ય)do_source((char_u *)USR_GVIMRC_FILE4, TRUE,
							    DOSO_GVIMRC, ખાલી);
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
	    જો (p_exrc)
	    {
#ifdef UNIX
		{
		    stat_T s;

		    // if ".gvimrc" file is not owned by user, set 'secure'
		    // mode
		    જો (mch_stat(GVIMRC_FILE, &s) || s.st_uid != getuid())
			secure = p_secure;
		}
#નહીંતર
		secure = p_secure;
#endif

		જો (       fullpathcmp((char_u *)USR_GVIMRC_FILE,
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
		    do_source((char_u *)GVIMRC_FILE, TRUE, DOSO_GVIMRC, ખાલી);

		જો (secure == 2)
		    need_wait_return = TRUE;
		secure = 0;
	    }
	}

	જો (need_wait_return || સંદેશ_didany)
	    wait_return(TRUE);

	--recursive;
    }

    // If recursive call opened the shell, return here from the first call
    જો (gui.in_use)
	પાછા;

    /*
     * Create the GUI shell.
     */
    gui.in_use = true;		// Must be set after menus have been set up
#જો defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
    gui.scale = 1.0; // Default value
#endif
    જો (gui_mch_init() == FAIL)
	જાઓ error;

    // Avoid a delay for an error message that was printed in the terminal
    // where Vim was started.
    emsg_on_display = FALSE;
    સંદેશ_scrolled = 0;
    clear_sb_text(TRUE);
    need_wait_return = FALSE;
    સંદેશ_didany = FALSE;

    /*
     * Check validity of any generic resources that may have been loaded.
     */
    જો (gui.border_width < 0)
	gui.border_width = 0;

    /*
     * Set up the fonts.  First use a font specified with "-fn" or "-font".
     */
    જો (font_argument != ખાલી)
	set_option_કિંમત_give_err((char_u *)"gfn",
					       0L, (char_u *)font_argument, 0);
    જો (
#ifdef FEAT_XFONTSET
	    (*p_guifontset == NUL
	     || gui_init_font(p_guifontset, TRUE) == FAIL) &&
#endif
	    gui_init_font(*p_guifont == NUL ? hl_get_font_name()
						  : p_guifont, FALSE) == FAIL)
    {
	emsg(_(e_cannot_start_gui_no_valid_font_found));
	જાઓ error2;
    }
    જો (gui_get_wide_font() == FAIL)
	emsg(_(e_guifontwide_invalid));

    gui.અંક_cols = Columns;
    gui.અંક_rows = Rows;
    gui_reset_scroll_region();

    // Create initial scrollbars
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_ડાબું], SBAR_ડાબું, wp);
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_જમણું], SBAR_જમણું, wp);
    }
    gui_create_scrollbar(&gui.bottom_sbar, SBAR_BOTTOM, ખાલી);

#ifdef FEAT_MENU
    gui_create_initial_menus(મૂળ_menu);
#endif
#ifdef FEAT_SIGN_ICONS
    sign_gui_started();
#endif

    // Configure the desired menu and scrollbars
    gui_init_which_components(ખાલી);

    // All components of the GUI have been created now
    gui.shell_created = true;

#ifdef FEAT_GUI_MSWIN
    // Set the shell size, adjusted for the screen size.  For GTK this only
    // works after the shell has been opened, thus it is further down.
    // If the window is already maximized (e.g. when --windowid is passed in),
    // we want to use the system-provided dimensions by passing FALSE to
    // mustset. Otherwise, we want to initialize with the default rows/columns.
    જો (gui_mch_maximized())
	gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    નહીંતર
	gui_set_shellsize(TRUE, TRUE, RESIZE_BOTH);
#નહીંતર
# ifndef FEAT_GUI_GTK
    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
# endif
#endif
#જો defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    // Need to set the size of the menubar after all the menus have been
    // created.
    gui_mch_compute_menu_height((Widget)0);
#endif

    /*
     * Actually open the GUI shell.
     */
    જો (gui_mch_open() != FAIL)
    {
	maketitle();
	resettitle();

	init_gui_options();
#ifdef FEAT_ARABIC
	// Our GUI can't do bidi.
	p_tbidi = FALSE;
#endif
#જો defined(FEAT_GUI_GTK)
	// Give GTK+ a chance to put all widget's into place.
	gui_mch_update();

# ifdef FEAT_MENU
	// If there is no 'm' in 'guioptions' we need to remove the menu now.
	// It was still there to make F10 work.
	જો (vim_strchr(p_go, GO_MENUS) == ખાલી)
	{
	    bool save_starting = gui.starting;

	    gui.starting = false;
	    gui_mch_enable_menu(FALSE);
	    gui.starting = save_starting;
	    gui_mch_update();
	}
# endif

	// Now make sure the shell fits on the screen.
	જો (gui_mch_maximized())
	    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
	નહીંતર
	    gui_set_shellsize(TRUE, TRUE, RESIZE_BOTH);
#endif
	// When 'lines' was set while starting up the topframe may have to be
	// resized.
	win_new_shellsize();

#ifdef FEAT_BEVAL_GUI
	// Always create the Balloon Evaluation area, but disable it when
	// 'ballooneval' is off.
	જો (balloonEval != ખાલી)
	{
# ifdef FEAT_VARTABS
	    vim_free(balloonEval->vts);
# endif
	    vim_free(balloonEval);
	}
	balloonEvalForTerm = FALSE;
# ifdef FEAT_GUI_GTK
	balloonEval = gui_mch_create_beval_area(gui.drawarea, ખાલી,
						     &general_beval_cb, ખાલી);
# નહીંતર
#  જો defined(FEAT_GUI_MOTIF)
	{
	    બાહ્ય Widget	textArea;
	    balloonEval = gui_mch_create_beval_area(textArea, ખાલી,
						     &general_beval_cb, ખાલી);
	}
#  નહીંતર
#   ifdef FEAT_GUI_MSWIN
	balloonEval = gui_mch_create_beval_area(ખાલી, ખાલી,
						     &general_beval_cb, ખાલી);
#   endif
#  endif
# endif
	જો (!p_beval)
	    gui_mch_disable_beval_area(balloonEval);
#endif

#ifndef FEAT_GUI_MSWIN
	// In the GUI modifiers are prepended to keys.
	// Don't do this for MS-Windows yet, it sends CTRL-K without the
	// modifier.
	seen_Modify_Other_ચાવીઓ = TRUE;
#endif

#જો defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	જો (!im_xim_isvalid_imactivate())
	    emsg(_(e_કિંમત_of_imactivatekey_is_invalid));
#endif
	// When 'cmdheight' was set during startup it may not have taken
	// effect yet.
	જો (p_ch != 1L)
	    command_height();

	પાછા;
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


    શૂન્ય
gui_exit(પૂર્ણાંક rc)
{
    // don't free the fonts, it leads to a BUS error
    // richard@whitequeen.com Jul 99
    free_highlight_fonts();
    gui.in_use = false;
    gui_mch_exit(rc);
}

#જો defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_PHOTON)
# define NEED_GUI_UPDATE_SCREEN 1
/*
 * Called when the GUI shell is closed by the user.  If there are no changed
 * files Vim exits, otherwise there will be a dialog to ask the user what to
 * do.
 * When this function returns, Vim should NOT exit!
 */
    શૂન્ય
gui_shell_closed(શૂન્ય)
{
    cmdmod_T	    save_cmdmod = cmdmod;

    જો (before_quit_autocmds(curwin, TRUE, FALSE))
	પાછા;

    // Only exit when there are no changed files
    exiting = TRUE;
# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    cmdmod.cmod_flags |= CMOD_CONFIRM;
# endif
    // If there are changed buffers, present the user with a dialog if
    // possible, otherwise give an error message.
    જો (!check_changed_any(FALSE, FALSE))
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
    પૂર્ણાંક
gui_init_font(char_u *font_list, પૂર્ણાંક fontset UNUSED)
{
#define FONTLEN 320
    char_u	font_name[FONTLEN];
    પૂર્ણાંક		font_list_empty = FALSE;
    પૂર્ણાંક		ret = FAIL;

    જો (!gui.in_use)
	પાછા FAIL;

    font_name[0] = NUL;
    જો (*font_list == NUL)
	font_list_empty = TRUE;
    નહીંતર
    {
#ifdef FEAT_XFONTSET
	// When using a fontset, the whole list of fonts is one name.
	જો (fontset)
	    ret = gui_mch_init_font(font_list, TRUE);
	નહીંતર
#endif
	    જ્યારે (*font_list != NUL)
	    {
		// Isolate one comma separated font name.
		(શૂન્ય)copy_option_part(&font_list, font_name, FONTLEN, ",");

		// Careful!!!  The Win32 version of gui_mch_init_font(), when
		// called with "*" will change p_guifont to the selected font
		// name, which frees the old value.  This makes font_list
		// invalid.  Thus when OK is returned here, font_list must no
		// longer be used!
		જો (gui_mch_init_font(font_name, FALSE) == OK)
		{
#ifdef USE_SET_GUIFONTWIDE
		    // If it's a Unicode font, try setting 'guifontwide' to a
		    // similar double-width font.
		    જો ((p_guifontwide == ખાલી || *p_guifontwide == NUL)
				&& strstr((અક્ષર *)font_name, "10646") != ખાલી)
			set_guifontwide(font_name);
#endif
		    ret = OK;
		    તોડો;
		}
	    }
    }

    જો (ret != OK
	    && STRCMP(font_list, "*") != 0
	    && (font_list_empty || gui.norm_font == NOFONT))
    {
	/*
	 * Couldn't load any font in 'font_list', keep the current font if
	 * there is one.  If 'font_list' is empty, or if there is no current
	 * font, tell gui_mch_init_font() to try to find a font we can load.
	 */
	ret = gui_mch_init_font(ખાલી, FALSE);
    }

    જો (ret == OK)
    {
#ifndef FEAT_GUI_GTK
	// Set normal font as current font
# ifdef FEAT_XFONTSET
	જો (gui.fontset != NOFONTSET)
	    gui_mch_set_fontset(gui.fontset);
	નહીંતર
# endif
	    gui_mch_set_font(gui.norm_font);
#endif
	gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    }

    પાછા ret;
}

#ifdef USE_SET_GUIFONTWIDE
/*
 * Try setting 'guifontwide' to a font twice as wide as "name".
 */
    સ્થિર શૂન્ય
set_guifontwide(char_u *name)
{
    પૂર્ણાંક		i = 0;
    char_u	wide_name[FONTLEN + 10]; // room for 2 * width and '*'
    char_u	*wp = ખાલી;
    char_u	*p;
    GuiFont	font;

    wp = wide_name;
    માટે (p = name; *p != NUL; ++p)
    {
	*wp++ = *p;
	જો (*p == '-')
	{
	    ++i;
	    જો (i == 6)		// font type: change "--" to "-*-"
	    {
		જો (p[1] == '-')
		    *wp++ = '*';
	    }
	    નહીંતર જો (i == 12)	// found the width
	    {
		++p;
		i = getdigits(&p);
		જો (i != 0)
		{
		    // Double the width specification.
		    sprintf((અક્ષર *)wp, "%d%s", i * 2, p);
		    font = gui_mch_get_font(wide_name, FALSE);
		    જો (font != NOFONT)
		    {
			gui_mch_free_font(gui.wide_font);
			gui.wide_font = font;
			set_string_option_direct((char_u *)"gfw", -1,
						      wide_name, OPT_FREE, 0);
		    }
		}
		તોડો;
	    }
	}
    }
}
#endif

/*
 * Get the font for 'guifontwide'.
 * Return FAIL for an invalid font name.
 */
    પૂર્ણાંક
gui_get_wide_font(શૂન્ય)
{
    GuiFont	font = NOFONT;
    char_u	font_name[FONTLEN];
    char_u	*p;

    જો (!gui.in_use)	    // Can't allocate font yet, assume it's OK.
	પાછા OK;	    // Will give an error message later.

    જો (p_guifontwide != ખાલી && *p_guifontwide != NUL)
    {
	માટે (p = p_guifontwide; *p != NUL; )
	{
	    // Isolate one comma separated font name.
	    (શૂન્ય)copy_option_part(&p, font_name, FONTLEN, ",");
	    font = gui_mch_get_font(font_name, FALSE);
	    જો (font != NOFONT)
		તોડો;
	}
	જો (font == NOFONT)
	    પાછા FAIL;
    }

    gui_mch_free_font(gui.wide_font);
#ifdef FEAT_GUI_GTK
    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    જો (font != NOFONT && gui.norm_font != NOFONT
			 && pango_font_description_equal(font, gui.norm_font))
    {
	gui.wide_font = NOFONT;
	gui_mch_free_font(font);
    }
    નહીંતર
#endif
	gui.wide_font = font;
#ifdef FEAT_GUI_MSWIN
    gui_mch_wide_font_changed();
#નહીંતર
    /*
     * TODO: setup wide_bold_font, wide_ital_font and wide_boldital_font to
     * support those fonts for 'guifontwide'.
     */
#endif
    પાછા OK;
}

#જો defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * Set list of ascii characters that combined can create ligature.
 * Store them in char map for quick access from gui_gtk_draw_string.
 */
    શૂન્ય
gui_set_ligatures(શૂન્ય)
{
    char_u	*p;

    જો (*p_guiligatures != NUL)
    {
	// check for invalid characters
	માટે (p = p_guiligatures; *p != NUL; ++p)
	    જો (*p < 32 || *p > 127)
	    {
		emsg(_(e_ascii_code_not_in_range));
		પાછા;
	    }

	// store valid setting into ligatures_map
	CLEAR_FIELD(gui.ligatures_map);
	માટે (p = p_guiligatures; *p != NUL; ++p)
	    gui.ligatures_map[*p] = 1;
    }
    નહીંતર
	CLEAR_FIELD(gui.ligatures_map);
}

/*
 * Adjust the columns to undraw for when the cursor is on ligatures.
 */
    સ્થિર શૂન્ય
gui_adjust_undraw_cursor_for_ligatures(પૂર્ણાંક *startcol, પૂર્ણાંક *endcol)
{
    પૂર્ણાંક off;

    જો (Screen_લીટીઓ == ખાલી || *p_guiligatures == NUL)
	પાછા;

    // expand before the cursor for all the chars in gui.ligatures_map
    off = લીટી_Offset[gui.cursor_row] + *startcol;
    જો (gui.ligatures_map[Screen_લીટીઓ[off]])
	જ્યારે (*startcol > 0 && gui.ligatures_map[Screen_લીટીઓ[--off]])
	    (*startcol)--;

    // expand after the cursor for all the chars in gui.ligatures_map
    off = લીટી_Offset[gui.cursor_row] + *endcol;
    જો (gui.ligatures_map[Screen_લીટીઓ[off]])
	જ્યારે (*endcol < ((પૂર્ણાંક)screen_Columns - 1)
				      && gui.ligatures_map[Screen_લીટીઓ[++off]])
	   (*endcol)++;
}
#endif

    સ્થિર શૂન્ય
gui_set_cursor(પૂર્ણાંક row, પૂર્ણાંક col)
{
    gui.row = row;
    gui.col = col;
}

/*
 * gui_check_pos - check if the cursor is on the screen.
 */
    સ્થિર શૂન્ય
gui_check_pos(શૂન્ય)
{
    જો (gui.row >= screen_Rows)
	gui.row = screen_Rows - 1;
    જો (gui.col >= screen_Columns)
	gui.col = screen_Columns - 1;
    જો (gui.cursor_row >= screen_Rows || gui.cursor_col >= screen_Columns)
	gui.cursor_is_valid = false;
}

/*
 * Redraw the cursor if necessary or when forced.
 * Careful: The contents of ScreenLines[] must match what is on the screen,
 * otherwise this goes wrong.  May need to call out_flush() first.
 */
    શૂન્ય
gui_update_cursor(
    પૂર્ણાંક		force,		 // when TRUE, update even when not moved
    પૂર્ણાંક		clear_selection) // clear selection under cursor
{
    પૂર્ણાંક		cur_width = 0;
    પૂર્ણાંક		cur_height = 0;
    પૂર્ણાંક		old_hl_mask;
    cursorentry_T *shape;
    પૂર્ણાંક		id;
#ifdef FEAT_TERMINAL
    guicolor_T	shape_fg = INVALCOLOR;
    guicolor_T	shape_bg = INVALCOLOR;
#endif
    guicolor_T	cfg, cbg, cc;	// cursor fore-/background color
    પૂર્ણાંક		cattr;		// cursor attributes
    પૂર્ણાંક		attr;
    attrentry_T *aep = ખાલી;
#જો (defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)) && !defined(USE_GTK4)
    bool	lig_ડાબું = false, lig_જમણું = false;
#endif

    // Don't update the cursor when halfway busy scrolling or the screen size
    // doesn't match 'columns' and 'lines.  ScreenLines[] isn't valid then.
    જો (!can_update_cursor || screen_Columns != gui.અંક_cols
					       || screen_Rows != gui.અંક_rows)
	પાછા;

    gui_check_pos();

    જો (gui.cursor_is_valid && !force
		&& gui.row == gui.cursor_row && gui.col == gui.cursor_col)
	પાછા;

    gui_undraw_cursor();

    // If a cursor-less sleep is ongoing, leave the cursor invisible
    જો (cursor_is_sleeping())
	પાછા;

    જો (gui.row < 0)
	પાછા;
#ifdef HAVE_આગત_METHOD
    જો (gui.row != gui.cursor_row || gui.col != gui.cursor_col)
	im_set_position(gui.row, gui.col);
#endif
    gui.cursor_row = gui.row;
    gui.cursor_col = gui.col;

    // Only write to the screen after ScreenLines[] has been initialized
    જો (!screen_cleared || Screen_લીટીઓ == ખાલી)
	પાછા;

    // Clear the selection if we are about to write over it
    જો (clear_selection)
	clip_may_clear_selection(gui.row, gui.row);
    // Check that the cursor is inside the shell (resizing may have made
    // it invalid)
    જો (gui.row >= screen_Rows || gui.col >= screen_Columns)
	પાછા;

    gui.cursor_is_valid = true;

    /*
     * How the cursor is drawn depends on the current mode.
     * When in a terminal window use the shape/color specified there.
     */
#ifdef FEAT_TERMINAL
    જો (terminal_is_active())
	shape = term_get_cursor_shape(&shape_fg, &shape_bg);
    નહીંતર
#endif
	shape = &shape_table[get_shape_idx(FALSE)];
    જો (State & MODE_LANGMAP)
	id = shape->id_lm;
    નહીંતર
	id = shape->id;

    // get the colors and attributes for the cursor.  Default is inverted
    cfg = INVALCOLOR;
    cbg = INVALCOLOR;
    cattr = HL_INVERSE;
    gui_mch_set_blinking(shape->blinkwait,
	    shape->blinkon,
	    shape->blinkoff);
    જો (shape->blinkwait == 0 || shape->blinkon == 0
	    || shape->blinkoff == 0)
	gui_mch_stop_blink(FALSE);
#ifdef FEAT_TERMINAL
    જો (shape_bg != INVALCOLOR)
    {
	cattr = 0;
	cfg = shape_fg;
	cbg = shape_bg;
    }
    નહીંતર
#endif
	જો (id > 0)
	{
	    cattr = syn_id2colors(id, &cfg, &cbg);
#જો defined(HAVE_આગત_METHOD)
	    {
		સ્થિર પૂર્ણાંક iid;
		guicolor_T fg, bg;

		જો (
# જો defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
			preedit_get_status()
# નહીંતર
			im_get_status()
# endif
		   )
		{
		    iid = syn_name2id((char_u *)"CursorIM");
		    જો (iid > 0)
		    {
			syn_id2colors(iid, &fg, &bg);
			જો (bg != INVALCOLOR)
			    cbg = bg;
			જો (fg != INVALCOLOR)
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
    attr = ScreenAttrs[લીટી_Offset[gui.row] + gui.col];
    જો (attr > HL_ALL)
	aep = syn_gui_attr2entry(attr);
    જો (aep != ખાલી)
    {
	attr = aep->ae_attr;
	જો (cfg == INVALCOLOR)
	    cfg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.bg_color
		    : aep->ae_u.gui.fg_color);
	જો (cbg == INVALCOLOR)
	    cbg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.fg_color
		    : aep->ae_u.gui.bg_color);
    }
    જો (cfg == INVALCOLOR)
	cfg = (attr & HL_INVERSE) ? gui.back_pixel : gui.norm_pixel;
    જો (cbg == INVALCOLOR)
	cbg = (attr & HL_INVERSE) ? gui.norm_pixel : gui.back_pixel;

#ifdef FEAT_XIM
    જો (aep != ખાલી)
    {
	xim_bg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.fg_color
		: aep->ae_u.gui.bg_color);
	xim_fg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.bg_color
		: aep->ae_u.gui.fg_color);
	જો (xim_bg_color == INVALCOLOR)
	    xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
		: gui.back_pixel;
	જો (xim_fg_color == INVALCOLOR)
	    xim_fg_color = (attr & HL_INVERSE) ? gui.back_pixel
		: gui.norm_pixel;
    }
    નહીંતર
    {
	xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
	    : gui.back_pixel;
	xim_fg_color = (attr & HL_INVERSE) ? gui.back_pixel
	    : gui.norm_pixel;
    }
#endif

    attr &= ~HL_INVERSE;
    જો (cattr & HL_INVERSE)
    {
	cc = cbg;
	cbg = cfg;
	cfg = cc;
    }
    cattr &= ~HL_INVERSE;

    /*
     * When we don't have window focus, draw a hollow cursor.
     */
    જો (!gui.in_focus)
    {
#ifdef USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, -1, -1);
#નહીંતર
	gui_mch_draw_hollow_cursor(cbg);
#endif
	પાછા;
    }

#ifdef USE_GTK4
    // Make sure that character underneath is drawn again in case it is part of
    // a ligature.
    gui_redraw_block(gui.row, gui.col, gui.row, gui.col, GUI_MON_NOCLEAR);
    // gui_redraw_block() will invalidate the cursor
    gui.cursor_is_valid = true;
#endif
#જો defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // If cursor is in the middle of a ligature, then split the ligature at
    // the cursor boundaries by redrawing those cells again.
    માટે (પૂર્ણાંક c = gui.col - 1; c >= 0; c--)
	જો (!gui.ligatures_map[Screen_લીટીઓ[લીટી_Offset[gui.row] + c]])
	{
	    જો (c < gui.col - 1)
	    {
		gui_redraw_block(gui.row, c + 1, gui.row, gui.col - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_ડાબું = true;
# endif
	    }
	    તોડો;
	}

    માટે (પૂર્ણાંક c = gui.col + 1; c < screen_Columns - 1; c++)
	જો (!gui.ligatures_map[Screen_લીટીઓ[લીટી_Offset[gui.row] + c]])
	{
	    જો (c > gui.col + 1)
	    {
		gui_redraw_block(gui.row, gui.col + 1, gui.row, c - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_જમણું = true;
# endif
	    }
	    તોડો;
	}

# ifndef USE_GTK4
    જો ((lig_ડાબું || lig_જમણું) && shape->shape != SHAPE_BLOCK)
    {
	// Because the cursor is not drawn with gui_screenchar(), must blit the
	// cell again (with its background), so that old ligature does not
	// remain on screen. Not needed for GtkSnapshot, because char beneath is
	// always rerendered (see above ifdef).
	પૂર્ણાંક old = gui.col;
	gui.highlight_mask = ScreenAttrs[લીટી_Offset[gui.row] + gui.col];
	(શૂન્ય)gui_screenchar(લીટી_Offset[gui.row] + gui.col,
		GUI_MON_NOCLEAR, (guicolor_T)0, (guicolor_T)0, 0);
	gui.col = old;
    }
# endif
    // gui_redraw_block()/gui_screenchar() may invalidate the cursor, make sure
    // to validate it again.
    gui.cursor_is_valid = true;
#endif

    old_hl_mask = gui.highlight_mask;
    જો (shape->shape == SHAPE_BLOCK)
    {
#ifdef USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, 0, 0);
#નહીંતર
	/*
	 * Draw the text character with the cursor colors.	Use the
	 * character attributes plus the cursor attributes.
	 */
	gui.highlight_mask = (cattr | attr);
	(શૂન્ય)gui_screenchar(લીટી_Offset[gui.row] + gui.col,
		GUI_MON_IS_CURSOR | GUI_MON_NOCLEAR, cfg, cbg, 0);
#endif
    }
    નહીંતર
    {
#જો defined(FEAT_RIGHTLEFT)
	પૂર્ણાંક	    col_off = FALSE;
#endif
	/*
	 * First draw the partial cursor, then overwrite with the text
	 * character, using a transparent background.
	 */
	જો (shape->shape == SHAPE_VER)
	{
	    cur_height = gui.char_height;
	    cur_width = (gui.char_width * shape->percentage + 99) / 100;
	}
	નહીંતર
	{
	    cur_height = (gui.char_height * shape->percentage + 99) / 100;
	    cur_width = gui.char_width;
	}
	જો (has_mbyte && (*mb_off2cells)(લીટી_Offset[gui.row] + gui.col,
		    લીટી_Offset[gui.row] + screen_Columns) > 1)
	{
	    // Double wide character.
	    જો (shape->shape != SHAPE_VER)
		cur_width += gui.char_width;
#ifdef FEAT_RIGHTLEFT
	    જો (CURSOR_BAR_જમણું)
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
#નહીંતર
	gui_mch_draw_part_cursor(cur_width, cur_height, cbg);
#endif
#જો defined(FEAT_RIGHTLEFT)
	જો (col_off)
	    --gui.col;
#endif

	// Doesn't seem to work for MSWindows. We call gui_redraw_block() above
	// for GtkSnapshot.
#જો !defined(FEAT_GUI_MSWIN) && !defined(USE_GTK4)
	gui.highlight_mask = ScreenAttrs[લીટી_Offset[gui.row] + gui.col];
	(શૂન્ય)gui_screenchar(લીટી_Offset[gui.row] + gui.col,
		GUI_MON_TRS_CURSOR | GUI_MON_NOCLEAR,
		(guicolor_T)0, (guicolor_T)0, 0);
#endif
    }
    gui.highlight_mask = old_hl_mask;
}

#જો defined(FEAT_MENU)
    સ્થિર શૂન્ય
gui_position_menu(શૂન્ય)
{
# જો !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
    જો (gui.menu_is_active && gui.in_use)
	gui_mch_set_menu_pos(0, 0, gui.menu_width, gui.menu_height);
# endif
}
#endif

/*
 * Position the various GUI components (text area, menu).  The vertical
 * scrollbars are NOT handled here.  See gui_update_scrollbars().
 */
    સ્થિર શૂન્ય
gui_position_components(પૂર્ણાંક total_width UNUSED)
{
    પૂર્ણાંક	    text_area_x;
    પૂર્ણાંક	    text_area_y;
    પૂર્ણાંક	    text_area_width;
    પૂર્ણાંક	    text_area_height;

    // avoid that moving components around generates events
    ++hold_gui_events;

    text_area_x = 0;
    જો (gui.which_scrollbars[SBAR_ડાબું])
	text_area_x += gui.scrollbar_width;

    text_area_y = 0;
#જો defined(FEAT_MENU) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    gui.menu_width = total_width;
    જો (gui.menu_is_active)
	text_area_y += gui.menu_height;
#endif

#જો defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
    જો (gui_has_tabline())
	text_area_y += gui.tabline_height;
#endif

#જો defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    જો (vim_strchr(p_go, GO_TOOLBAR) != ખાલી)
    {
# જો defined(FEAT_GUI_HAIKU)
	gui_mch_set_toolbar_pos(0, text_area_y,
				gui.menu_width, gui.toolbar_height);
# endif
	text_area_y += gui.toolbar_height;
    }
#endif

#જો defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_HAIKU)
    gui_mch_set_tabline_pos(0, text_area_y,
    gui.menu_width, gui.tabline_height);
    જો (gui_has_tabline())
	text_area_y += gui.tabline_height;
#endif

    text_area_width = gui.અંક_cols * gui.char_width + gui.border_offset * 2;
    text_area_height = gui.અંક_rows * gui.char_height + gui.border_offset * 2;

    gui_mch_set_text_area_pos(text_area_x,
			      text_area_y,
			      text_area_width,
			      text_area_height
#જો defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
				  + xim_get_status_area_height()
#endif
			      );
#ifdef FEAT_MENU
    gui_position_menu();
#endif
    જો (gui.which_scrollbars[SBAR_BOTTOM])
	gui_mch_set_scrollbar_pos(&gui.bottom_sbar,
				  text_area_x,
				  text_area_y + text_area_height
					+ gui_mch_get_scrollbar_ypadding(),
				  text_area_width,
				  gui.scrollbar_height);
    gui.ડાબું_sbar_x = 0;
    gui.જમણું_sbar_x = text_area_x + text_area_width
					+ gui_mch_get_scrollbar_xpadding();

    --hold_gui_events;
}

/*
 * Get the width of the widgets and decorations to the side of the text area.
 */
    પૂર્ણાંક
gui_get_base_width(શૂન્ય)
{
    પૂર્ણાંક	    base_width;

    base_width = 2 * gui.border_offset;
    જો (gui.which_scrollbars[SBAR_ડાબું])
	base_width += gui.scrollbar_width;
    જો (gui.which_scrollbars[SBAR_જમણું])
	base_width += gui.scrollbar_width;
    પાછા base_width;
}

/*
 * Get the height of the widgets and decorations above and below the text area.
 */
    પૂર્ણાંક
gui_get_base_height(શૂન્ય)
{
    પૂર્ણાંક	    base_height;

    base_height = 2 * gui.border_offset;
    જો (gui.which_scrollbars[SBAR_BOTTOM])
	base_height += gui.scrollbar_height;
#ifdef FEAT_GUI_GTK
    // We can't take the sizes properly into account until anything is
    // realized.  Therefore we recalculate all the values here just before
    // setting the size. (--mdcki)
#નહીંતર
# ifdef FEAT_MENU
    જો (gui.menu_is_active)
	base_height += gui.menu_height;
# endif
# ifdef FEAT_TOOLBAR
    જો (vim_strchr(p_go, GO_TOOLBAR) != ખાલી)
	base_height += gui.toolbar_height;
# endif
# જો defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    જો (gui_has_tabline())
	base_height += gui.tabline_height;
# endif
# જો defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    base_height += gui_mch_text_area_extra_height();
# endif
#endif
    પાછા base_height;
}

/*
 * Should be called after the GUI shell has been resized.  Its arguments are
 * the new width and height of the shell in pixels.
 */
    શૂન્ય
gui_resize_shell(પૂર્ણાંક pixel_width, પૂર્ણાંક pixel_height)
{
    સ્થિર પૂર્ણાંક	busy = FALSE;

    જો (!gui.shell_created)	    // ignore when still initializing
	પાછા;

    /*
     * Can't resize the screen while it is being redrawn.  Remember the new
     * size and handle it later.
     */
    જો (updating_screen || busy)
    {
	new_pixel_width = pixel_width;
	new_pixel_height = pixel_height;
	પાછા;
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
#જો defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk_init_decor_height();
#endif

    gui.અંક_cols = (pixel_width - gui_get_base_width()) / gui.char_width;
    gui.અંક_rows = (pixel_height - gui_get_base_height()) / gui.char_height;

#ifdef USE_GTK4
    gui_gtk4_update_size();
#endif

    gui_position_components(pixel_width);
    gui_reset_scroll_region();

#જો defined(FEAT_GUI_GTK) && defined(USE_GTK4) && !defined(USE_GTK4)
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
    જો (State == MODE_ASKMORE || State == MODE_CONFIRM)
	gui.row = gui.અંક_rows;

    // Only comparing Rows and Columns may be sufficient, but let's stay on
    // the safe side.
    જો (gui.અંક_rows != screen_Rows || gui.અંક_cols != screen_Columns
	    || gui.અંક_rows != Rows || gui.અંક_cols != Columns || gui.force_redraw)
    {
	shell_resized();
	gui.force_redraw = 0;
    }

#ifdef FEAT_GUI_HAIKU
    vim_unlock_screen();
#endif

    gui_update_scrollbars(TRUE);
    gui_update_cursor(FALSE, TRUE);
#જો defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(pixel_width, pixel_height);
#endif
#જો defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
    xim_set_status_area();
#endif

    busy = FALSE;

    // We may have been called again while redrawing the screen.
    // Need to do it all again with the latest size then.  But only if the size
    // actually changed.
    જો (new_pixel_height)
    {
	જો (pixel_width == new_pixel_width && pixel_height == new_pixel_height)
	{
	    new_pixel_width = 0;
	    new_pixel_height = 0;
	}
	નહીંતર
	{
	    pixel_width = new_pixel_width;
	    pixel_height = new_pixel_height;
	    જાઓ again;
	}
    }
}

/*
 * Check if gui_resize_shell() must be called.
 */
    શૂન્ય
gui_may_resize_shell(શૂન્ય)
{
    જો (new_pixel_height)
    {
	// careful: gui_resize_shell() may postpone the resize again if we
	// were called indirectly by it
	gui_resize_shell(new_pixel_width, new_pixel_height);
    }
}

    પૂર્ણાંક
gui_get_shellsize(શૂન્ય)
{
    Rows = gui.અંક_rows;
    Columns = gui.અંક_cols;
    પાછા OK;
}

/*
 * Set the size of the Vim shell according to Rows and Columns.
 * If "fit_to_display" is TRUE then the size may be reduced to fit the window
 * on the screen.
 * When "mustset" is TRUE the size was set by the user. When FALSE a UI
 * component was added or removed (e.g., a scrollbar).
 */
    શૂન્ય
gui_set_shellsize(
    પૂર્ણાંક		mustset UNUSED,
    પૂર્ણાંક		fit_to_display,
    પૂર્ણાંક		direction)		// RESIZE_HOR, RESIZE_VER
{
    પૂર્ણાંક		base_width;
    પૂર્ણાંક		base_height;
    પૂર્ણાંક		width;
    પૂર્ણાંક		height;
    પૂર્ણાંક		min_width;
    પૂર્ણાંક		min_height;
    પૂર્ણાંક		screen_w;
    પૂર્ણાંક		screen_h;
#ifdef FEAT_GUI_GTK
    પૂર્ણાંક		un_maximize = mustset;
    પૂર્ણાંક		did_adjust = 0;
#endif
    પૂર્ણાંક		x = -1, y = -1;

    જો (!gui.shell_created)
	પાછા;

#જો defined(MSWIN) || defined(FEAT_GUI_GTK)
    // If not setting to a user specified size and maximized, calculate the
    // number of characters that fit in the maximized window.
    // FIXME: gui_mch_newfont() is called here even when the font hasn't
    // changed at all.  For example, ":set guioptions=k" triggers this path
    // via gui_init_which_components() -> gui_set_shellsize(FALSE, ...).
    // The intent is to keep the window size and recalculate Rows/Columns,
    // which has nothing to do with fonts.  This should be a separate
    // function with a more descriptive name.
    જો (!mustset && (vim_strchr(p_go, GO_KEEPWINSIZE) != ખાલી
						       || gui_mch_maximized()))
    {
	gui_mch_newfont();
	પાછા;
    }
#endif

    base_width = gui_get_base_width();
    base_height = gui_get_base_height();
    જો (fit_to_display)
	// Remember the original window position.
	(શૂન્ય)gui_mch_get_winpos(&x, &y);

    width = Columns * gui.char_width + base_width;
    height = Rows * gui.char_height + base_height;

    જો (fit_to_display)
    {
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	જો ((direction & RESIZE_HOR) && width > screen_w)
	{
	    Columns = (screen_w - base_width) / gui.char_width;
	    જો (Columns < MIN_COLUMNS)
		Columns = MIN_COLUMNS;
	    width = Columns * gui.char_width + base_width;
#ifdef FEAT_GUI_GTK
	    ++did_adjust;
#endif
	}
	જો ((direction & RESIZE_VERT) && height > screen_h)
	{
	    Rows = (screen_h - base_height) / gui.char_height;
	    check_shellsize();
	    height = Rows * gui.char_height + base_height;
#ifdef FEAT_GUI_GTK
	    ++did_adjust;
#endif
	}
#ifdef FEAT_GUI_GTK
	જો (did_adjust == 2 || (width + gui.char_width >= screen_w
				     && height + gui.char_height >= screen_h))
	    // don't unmaximize if at maximum size
	    un_maximize = FALSE;
#endif
    }
    limit_screen_size();
    gui.અંક_cols = Columns;
    gui.અંક_rows = Rows;
#ifdef USE_GTK4
    // Keep the drawing area in sync with the size Vim is going to draw.
    gui_gtk4_update_size();
#endif

    min_width = base_width + MIN_COLUMNS * gui.char_width;
    min_height = base_height + MIN_લીટીઓ * gui.char_height;
    min_height += tabline_height() * gui.char_height;

#ifdef FEAT_GUI_GTK
    જો (un_maximize)
    {
	// If the window size is smaller than the screen unmaximize the
	// window, otherwise resizing won't work.
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	જો ((width + gui.char_width < screen_w
				   || height + gui.char_height * 2 < screen_h)
		&& gui_mch_maximized())
	    gui_mch_unmaximize();
    }
#endif

    gui_mch_set_shellsize(width, height, min_width, min_height,
					  base_width, base_height, direction);

    જો (fit_to_display && x >= 0 && y >= 0)
    {
	// Some window managers put the Vim window left of/above the screen.
	// Only change the position if it wasn't already negative before
	// (happens on MS-Windows with a secondary monitor).
	gui_mch_update();
	જો (gui_mch_get_winpos(&x, &y) == OK && (x < 0 || y < 0))
	    gui_mch_set_winpos(x < 0 ? 0 : x, y < 0 ? 0 : y);
    }

    gui_position_components(width);
    gui_update_scrollbars(TRUE);
    gui_reset_scroll_region();

#જો defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(width, height);
#endif
}

/*
 * Called when Rows and/or Columns has changed.
 */
    શૂન્ય
gui_new_shellsize(શૂન્ય)
{
    gui_reset_scroll_region();
}

/*
 * Make scroll region cover whole screen.
 */
    સ્થિર શૂન્ય
gui_reset_scroll_region(શૂન્ય)
{
    gui.scroll_region_ટોચ = 0;
    gui.scroll_region_bot = gui.અંક_rows - 1;
    gui.scroll_region_ડાબું = 0;
    gui.scroll_region_જમણું = gui.અંક_cols - 1;
}

    સ્થિર શૂન્ય
gui_start_highlight(પૂર્ણાંક mask)
{
    જો (mask > HL_ALL)		    // highlight code
	gui.highlight_mask = mask;
    નહીંતર			    // mask
	gui.highlight_mask |= mask;
}

    શૂન્ય
gui_stop_highlight(પૂર્ણાંક mask)
{
    જો (mask > HL_ALL)		    // highlight code
	gui.highlight_mask = HL_NORMAL;
    નહીંતર			    // mask
	gui.highlight_mask &= ~mask;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    શૂન્ય
gui_clear_block(
    પૂર્ણાંક	    row1,
    પૂર્ણાંક	    col1,
    પૂર્ણાંક	    row2,
    પૂર્ણાંક	    col2)
{
    // Clear the selection if we are about to write over it
    clip_may_clear_selection(row1, row2);

    gui_mch_clear_block(row1, col1, row2, col2);

    // Invalidate cursor if it was in this block
    જો (       gui.cursor_row >= row1 && gui.cursor_row <= row2
	    && gui.cursor_col >= col1 && gui.cursor_col <= col2)
	gui.cursor_is_valid = false;
}

/*
 * Write code to update the cursor later.  This avoids the need to flush the
 * output buffer before calling gui_update_cursor().
 */
    શૂન્ય
gui_update_cursor_later(શૂન્ય)
{
    OUT_STR("\033|s");
}

    શૂન્ય
gui_write(
    char_u	*s,
    પૂર્ણાંક		len)
{
    char_u	*p;
    પૂર્ણાંક		arg1 = 0, arg2 = 0;
    પૂર્ણાંક		force_cursor = FALSE;	// force cursor update
    પૂર્ણાંક		force_scrollbar = FALSE;
    સ્થિર win_T	*old_curwin = ખાલી;

// #define DEBUG_GUI_WRITE
#ifdef DEBUG_GUI_WRITE
    {
	પૂર્ણાંક i;
	char_u *str;

	printf("gui_write(%d):\n    ", len);
	માટે (i = 0; i < len; i++)
	    જો (s[i] == ESC)
	    {
		જો (i != 0)
		    printf("\n    ");
		printf("<ESC>");
	    }
	    નહીંતર
	    {
		str = transchar_byte(s[i]);
		જો (str[0] && str[1])
		    printf("<%s>", (અક્ષર *)str);
		નહીંતર
		    printf("%s", (અક્ષર *)str);
	    }
	printf("\n");
    }
#endif
    જ્યારે (len)
    {
	જો (s[0] == ESC && s[1] == '|')
	{
	    p = s + 2;
	    જો (VIM_ISDIGIT(*p) || (*p == '-' && VIM_ISDIGIT(*(p + 1))))
	    {
		arg1 = getdigits(&p);
		જો (p > s + len)
		    તોડો;
		જો (*p == ';')
		{
		    ++p;
		    arg2 = getdigits(&p);
		    જો (p > s + len)
			તોડો;
		}
	    }
	    પસંદગી (*p)
	    {
		સ્થિતિ 'C':	// Clear screen
		    clip_scroll_selection(9999);
		    gui_mch_clear_all();
		    gui.cursor_is_valid = false;
		    force_scrollbar = TRUE;
		    તોડો;
		સ્થિતિ 'M':	// Move cursor
		    gui_set_cursor(arg1, arg2);
		    તોડો;
		સ્થિતિ 's':	// force cursor (shape) update
		    force_cursor = TRUE;
		    તોડો;
		સ્થિતિ 'R':	// Set scroll region
		    જો (arg1 < arg2)
		    {
			gui.scroll_region_ટોચ = arg1;
			gui.scroll_region_bot = arg2;
		    }
		    નહીંતર
		    {
			gui.scroll_region_ટોચ = arg2;
			gui.scroll_region_bot = arg1;
		    }
		    તોડો;
		સ્થિતિ 'V':	// Set vertical scroll region
		    જો (arg1 < arg2)
		    {
			gui.scroll_region_ડાબું = arg1;
			gui.scroll_region_જમણું = arg2;
		    }
		    નહીંતર
		    {
			gui.scroll_region_ડાબું = arg2;
			gui.scroll_region_જમણું = arg1;
		    }
		    તોડો;
		સ્થિતિ 'd':	// Delete line
		    gui_delete_લીટીઓ(gui.row, 1);
		    તોડો;
		સ્થિતિ 'D':	// Delete lines
		    gui_delete_લીટીઓ(gui.row, arg1);
		    તોડો;
		સ્થિતિ 'i':	// Insert line
		    gui_ઉમેરો_લીટીઓ(gui.row, 1);
		    તોડો;
		સ્થિતિ 'I':	// Insert lines
		    gui_ઉમેરો_લીટીઓ(gui.row, arg1);
		    તોડો;
		સ્થિતિ '$':	// Clear to end-of-line
		    gui_clear_block(gui.row, gui.col, gui.row,
							    (પૂર્ણાંક)Columns - 1);
		    તોડો;
		સ્થિતિ 'h':	// Turn on highlighting
		    gui_start_highlight(arg1);
		    તોડો;
		સ્થિતિ 'H':	// Turn off highlighting
		    gui_stop_highlight(arg1);
		    તોડો;
		સ્થિતિ 'f':	// flash the window (visual bell)
		    gui_mch_flash(arg1 == 0 ? 20 : arg1);
		    તોડો;
		મૂળભૂત:
		    p = s + 1;	// Skip the ESC
		    તોડો;
	    }
	    len -= (પૂર્ણાંક)(++p - s);
	    s = p;
	}
	નહીંતર જો (s[0] < 0x20		// Ctrl character
#ifdef FEAT_SIGN_ICONS
		&& s[0] != SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
		&& s[0] != MULTISIGN_BYTE
# endif
#endif
		)
	{
	    જો (s[0] == '\n')		// NL
	    {
		gui.col = 0;
		જો (gui.row < gui.scroll_region_bot)
		    gui.row++;
		નહીંતર
		    gui_delete_લીટીઓ(gui.scroll_region_ટોચ, 1);
	    }
	    નહીંતર જો (s[0] == '\r')	// CR
	    {
		gui.col = 0;
	    }
	    નહીંતર જો (s[0] == '\b')	// Backspace
	    {
		જો (gui.col)
		    --gui.col;
	    }
	    નહીંતર જો (s[0] == Ctrl_L)	// cursor-right
	    {
		++gui.col;
	    }
	    નહીંતર જો (s[0] == Ctrl_G)	// Beep
	    {
		gui_mch_beep();
	    }
	    // Other Ctrl character: shouldn't happen!

	    --len;	// Skip this char
	    ++s;
	}
	નહીંતર
	{
	    p = s;
	    જ્યારે (len > 0 && (
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
	    gui_outstr(s, (પૂર્ણાંક)(p - s));
	    s = p;
	}
    }

    // Postponed update of the cursor (won't work if "can_update_cursor" isn't
    // set).
    જો (force_cursor)
	gui_update_cursor(TRUE, TRUE);

    // When switching to another window the dragging must have stopped.
    // Required for GTK, dragged_sb isn't reset.
    જો (old_curwin != curwin)
	gui.dragged_sb = SBAR_NONE;

    // Update the scrollbars after clearing the screen or when switched
    // to another window.
    // Update the horizontal scrollbar always, it's difficult to check all
    // situations where it might change.
    જો (force_scrollbar || old_curwin != curwin)
	gui_update_scrollbars(force_scrollbar);
    નહીંતર
	gui_update_horiz_scrollbar(FALSE);
    old_curwin = curwin;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#જો defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#endif

    gui_may_flush();		    // In case vim decides to take a nap
}

/*
 * When ScreenLines[] is invalid, updating the cursor should not be done, it
 * produces wrong results.  Call gui_dont_update_cursor() before that code and
 * gui_can_update_cursor() afterwards.
 */
    શૂન્ય
gui_dont_update_cursor(પૂર્ણાંક undraw)
{
    જો (!gui.in_use)
	પાછા;

    // Undraw the cursor now, we probably can't do it after the change.
    જો (undraw)
	gui_undraw_cursor();
    can_update_cursor = FALSE;
}

    શૂન્ય
gui_can_update_cursor(શૂન્ય)
{
    can_update_cursor = TRUE;
    // No need to update the cursor right now, there is always more output
    // after scrolling.
}

/*
 * Disable issuing gui_mch_flush().
 */
    શૂન્ય
gui_disable_flush(શૂન્ય)
{
    ++disable_flush;
}

/*
 * Enable issuing gui_mch_flush().
 */
    શૂન્ય
gui_enable_flush(શૂન્ય)
{
    --disable_flush;
}

/*
 * Issue gui_mch_flush() if it is not disabled.
 */
    શૂન્ય
gui_may_flush(શૂન્ય)
{
    જો (disable_flush == 0)
	gui_mch_flush();
}

    સ્થિર શૂન્ય
gui_outstr(char_u *s, પૂર્ણાંક len)
{
    પૂર્ણાંક	    this_len;
    પૂર્ણાંક	    cells;

    જો (len == 0)
	પાછા;

    જો (len < 0)
	len = (પૂર્ણાંક)STRLEN(s);

    જ્યારે (len > 0)
    {
	જો (has_mbyte)
	{
	    // Find out how many chars fit in the current line.
	    cells = 0;
	    માટે (this_len = 0; this_len < len; )
	    {
		cells += (*mb_ptr2cells)(s + this_len);
		જો (gui.col + cells > Columns)
		    તોડો;
		this_len += (*mb_ptr2len)(s + this_len);
	    }
	    જો (this_len > len)
		this_len = len;	    // don't include following composing char
	}
	નહીંતર જો (gui.col + len > Columns)
	    this_len = Columns - gui.col;
	નહીંતર
	    this_len = len;

	(શૂન્ય)gui_outstr_nowrap(s, this_len,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	s += this_len;
	len -= this_len;
	// fill up for a double-width char that doesn't fit.
	જો (len > 0 && gui.col < Columns)
	    (શૂન્ય)gui_outstr_nowrap((char_u *)" ", 1,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	// The cursor may wrap to the next line.
	જો (gui.col >= Columns)
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
    સ્થિર પૂર્ણાંક
gui_screenchar(
    પૂર્ણાંક		off,	    // Offset from start of screen
    પૂર્ણાંક		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    પૂર્ણાંક		back)	    // backup this many chars when using bold trick
{
    char_u	buf[MB_MAXBYTES + 1];

    // Don't draw right half of a double-width UTF-8 char. "cannot happen"
    જો (enc_utf8 && Screen_લીટીઓ[off] == 0)
	પાછા OK;

    જો (enc_utf8 && Screen_લીટીઓ_UC[off] != 0)
	// Draw UTF-8 multi-byte character.
	પાછા gui_outstr_nowrap(buf, utfc_char2bytes(off, buf),
							 flags, fg, bg, back);

    જો (enc_dbcs == DBCS_JPNU && Screen_લીટીઓ[off] == 0x8e)
    {
	buf[0] = Screen_લીટીઓ[off];
	buf[1] = ScreenLines2[off];
	પાછા gui_outstr_nowrap(buf, 2, flags, fg, bg, back);
    }

    // Draw non-multi-byte character or DBCS character.
    પાછા gui_outstr_nowrap(Screen_લીટીઓ + off,
	    enc_dbcs ? (*mb_ptr2len)(Screen_લીટીઓ + off) : 1,
							 flags, fg, bg, back);
}
#endif

#ifdef FEAT_GUI_GTK
/*
 * Output the string at the given screen position.  This is used in place
 * of gui_screenchar() where possible because Pango needs as much context
 * as possible to work nicely.  It's a lot faster as well.
 */
    સ્થિર પૂર્ણાંક
gui_screenstr(
    પૂર્ણાંક		off,	    // Offset from start of screen
    પૂર્ણાંક		len,	    // string length in screen cells
    પૂર્ણાંક		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    પૂર્ણાંક		back)	    // backup this many chars when using bold trick
{
    char_u  *buf;
    પૂર્ણાંક	    outlen = 0;
    પૂર્ણાંક	    i;
    પૂર્ણાંક	    retval;

    જો (len <= 0) // "cannot happen"?
	પાછા OK;

    જો (enc_utf8)
    {
	buf = alloc(len * MB_MAXBYTES + 1);
	જો (buf == ખાલી)
	    પાછા OK; // not much we could do here...

	માટે (i = off; i < off + len; ++i)
	{
	    જો (Screen_લીટીઓ[i] == 0)
		ચાલુ; // skip second half of double-width char

	    જો (Screen_લીટીઓ_UC[i] == 0)
		buf[outlen++] = Screen_લીટીઓ[i];
	    નહીંતર
		outlen += utfc_char2bytes(i, buf + outlen);
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, back);
	vim_free(buf);

	પાછા retval;
    }
    નહીંતર જો (enc_dbcs == DBCS_JPNU)
    {
	buf = alloc(len * 2 + 1);
	જો (buf == ખાલી)
	    પાછા OK; // not much we could do here...

	માટે (i = off; i < off + len; ++i)
	{
	    buf[outlen++] = Screen_લીટીઓ[i];

	    // handle double-byte single-width char
	    જો (Screen_લીટીઓ[i] == 0x8e)
		buf[outlen++] = ScreenLines2[i];
	    નહીંતર જો (MB_BYTE2LEN(Screen_લીટીઓ[i]) == 2)
		buf[outlen++] = Screen_લીટીઓ[++i];
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, back);
	vim_free(buf);

	પાછા retval;
    }
    નહીંતર
    {
	પાછા gui_outstr_nowrap(&Screen_લીટીઓ[off], len,
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
    સ્થિર પૂર્ણાંક
gui_outstr_nowrap(
    char_u	*s,
    પૂર્ણાંક		len,
    પૂર્ણાંક		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    પૂર્ણાંક		back)	    // backup this many chars when using bold trick
{
    long_u	highlight_mask;
    long_u	hl_mask_todo;
    guicolor_T	fg_color;
    guicolor_T	bg_color;
    guicolor_T	sp_color;
#જો !defined(FEAT_GUI_GTK)
    GuiFont	font = NOFONT;
    GuiFont	wide_font = NOFONT;
# ifdef FEAT_XFONTSET
    GuiFontset	fontset = NOFONTSET;
# endif
#endif
    attrentry_T	*aep = ખાલી;
    પૂર્ણાંક		draw_flags;
    પૂર્ણાંક		col = gui.col;
#ifdef FEAT_SIGN_ICONS
    પૂર્ણાંક		draw_sign = FALSE;
    પૂર્ણાંક		signcol = 0;
    char_u	extra[18];
# ifdef FEAT_NETBEANS_INTG
    પૂર્ણાંક		multi_sign = FALSE;
# endif
#endif

    જો (len < 0)
	len = (પૂર્ણાંક)STRLEN(s);
    જો (len == 0)
	પાછા OK;

#ifdef FEAT_SIGN_ICONS
    જો (*s == SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
	  || *s == MULTISIGN_BYTE
# endif
       )
    {
	size_t	slen = 2;		    // 2 spaces by default

# ifdef FEAT_NETBEANS_INTG
	જો (*s == MULTISIGN_BYTE)
	    multi_sign = TRUE;
# endif
	// draw spaces instead
	જો (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u' &&
		(curwin->w_p_nu || curwin->w_p_rnu))
	{
	    પૂર્ણાંક	n = સંખ્યા_width(curwin);

	    slen = MIN(n, (પૂર્ણાંક)કદ(extra) - 1);
	}
	vim_memset(extra, ' ', slen);
	extra[slen] = NUL;
	s = extra;
	જો (len == 1 && col > 0)
	    --col;
	len = (પૂર્ણાંક)slen;
	જો (len > 2)
	    // right align sign icon in the number column
	    signcol = col + len - 3;
	નહીંતર
	    signcol = col;
	draw_sign = TRUE;
	highlight_mask = 0;
    }
    નહીંતર
#endif
    જો (gui.highlight_mask > HL_ALL)
    {
	aep = syn_gui_attr2entry(gui.highlight_mask);
	જો (aep == ખાલી)	    // highlighting not set
	    highlight_mask = 0;
	નહીંતર
	    highlight_mask = aep->ae_attr;
    }
    નહીંતર
	highlight_mask = gui.highlight_mask;
    hl_mask_todo = highlight_mask;

#જો !defined(FEAT_GUI_GTK)
    // Set the font
    જો (aep != ખાલી && aep->ae_u.gui.font != NOFONT)
	font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
    નહીંતર જો (aep != ખાલી && aep->ae_u.gui.fontset != NOFONTSET)
	fontset = aep->ae_u.gui.fontset;
# endif
    નહીંતર
    {
# ifdef FEAT_XFONTSET
	જો (gui.fontset != NOFONTSET)
	    fontset = gui.fontset;
	નહીંતર
# endif
	    જો (hl_mask_todo & (HL_BOLD | HL_STANDOUT))
	{
	    જો ((hl_mask_todo & HL_ITALIC) && gui.boldital_font != NOFONT)
	    {
		font = gui.boldital_font;
		hl_mask_todo &= ~(HL_BOLD | HL_STANDOUT | HL_ITALIC);
	    }
	    નહીંતર જો (gui.bold_font != NOFONT)
	    {
		font = gui.bold_font;
		hl_mask_todo &= ~(HL_BOLD | HL_STANDOUT);
	    }
	    નહીંતર
		font = gui.norm_font;
	}
	નહીંતર જો ((hl_mask_todo & HL_ITALIC) && gui.ital_font != NOFONT)
	{
	    font = gui.ital_font;
	    hl_mask_todo &= ~HL_ITALIC;
	}
	નહીંતર
	    font = gui.norm_font;

	/*
	 * Choose correct wide_font by font.  wide_font should be set with font
	 * at same time in above block.  But it will make many "ifdef" nasty
	 * blocks.  So we do it here.
	 */
	જો (font == gui.boldital_font && gui.wide_boldital_font)
	    wide_font = gui.wide_boldital_font;
	નહીંતર જો (font == gui.bold_font && gui.wide_bold_font)
	    wide_font = gui.wide_bold_font;
	નહીંતર જો (font == gui.ital_font && gui.wide_ital_font)
	    wide_font = gui.wide_ital_font;
	નહીંતર જો (font == gui.norm_font && gui.wide_font)
	    wide_font = gui.wide_font;
    }
# ifdef FEAT_XFONTSET
    જો (fontset != NOFONTSET)
	gui_mch_set_fontset(fontset);
    નહીંતર
# endif
	gui_mch_set_font(font);
#endif

    draw_flags = 0;

    // Set the color
    bg_color = gui.back_pixel;
    જો ((flags & GUI_MON_IS_CURSOR) && gui.in_focus)
    {
	draw_flags |= DRAW_CURSOR;
	fg_color = fg;
	bg_color = bg;
	sp_color = fg;
    }
    નહીંતર જો (aep != ખાલી)
    {
	fg_color = aep->ae_u.gui.fg_color;
	જો (fg_color == INVALCOLOR)
	    fg_color = gui.norm_pixel;
	bg_color = aep->ae_u.gui.bg_color;
	જો (bg_color == INVALCOLOR)
	    bg_color = gui.back_pixel;
	sp_color = aep->ae_u.gui.sp_color;
	જો (sp_color == INVALCOLOR)
	    sp_color = fg_color;
    }
    નહીંતર
    {
	fg_color = gui.norm_pixel;
	sp_color = fg_color;
    }

    જો (highlight_mask & (HL_INVERSE | HL_STANDOUT))
    {
	gui_mch_set_fg_color(bg_color);
	gui_mch_set_bg_color(fg_color);
    }
    નહીંતર
    {
	gui_mch_set_fg_color(fg_color);
	gui_mch_set_bg_color(bg_color);
    }
    gui_mch_set_sp_color(sp_color);

    // Clear the selection if we are about to write over it
    જો (!(flags & GUI_MON_NOCLEAR))
	clip_may_clear_selection(gui.row, gui.row);


    // If there's no bold font, then fake it
    જો (hl_mask_todo & (HL_BOLD | HL_STANDOUT))
	draw_flags |= DRAW_BOLD;

    /*
     * When drawing bold or italic characters the spill-over from the left
     * neighbor may be destroyed.  Let the caller backup to start redrawing
     * just after a blank.
     */
    જો (back != 0 && ((draw_flags & DRAW_BOLD) || (highlight_mask & HL_ITALIC)))
	પાછા FAIL;

#જો defined(FEAT_GUI_GTK)
    // If there's no italic font, then fake it.
    // For GTK2, we don't need a different font for italic style.
    જો (hl_mask_todo & HL_ITALIC)
	draw_flags |= DRAW_ITALIC;

    // Do we underline the text?
    જો (hl_mask_todo & HL_UNDERLINE)
	draw_flags |= DRAW_UNDERL;

#નહીંતર
    // Do we underline the text?
    જો ((hl_mask_todo & HL_UNDERLINE) || (hl_mask_todo & HL_ITALIC))
	draw_flags |= DRAW_UNDERL;
#endif
    // Do we undercurl the text?
    જો (hl_mask_todo & HL_UNDERCURL)
	draw_flags |= DRAW_UNDERC;

    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

    // Do we strikethrough the text?
    જો (hl_mask_todo & HL_STRIKETHROUGH)
	draw_flags |= DRAW_STRIKE;

    // Do we draw transparently?
    જો (flags & GUI_MON_TRS_CURSOR)
	draw_flags |= DRAW_TRANSP;

    /*
     * Draw the text.
     */
#ifdef FEAT_GUI_GTK
    // The value returned is the length in display cells
    len = gui_gtk_draw_string(gui.row, col, s, len, draw_flags);
#નહીંતર
    જો (enc_utf8)
    {
	પૂર્ણાંક	start;		// index of bytes to be drawn
	પૂર્ણાંક	cells;		// cellwidth of bytes to be drawn
	પૂર્ણાંક	thislen;	// length of bytes to be drawn
	પૂર્ણાંક	cn;		// cellwidth of current char
	પૂર્ણાંક	i;		// index of current char
	પૂર્ણાંક	c;		// current char value
	પૂર્ણાંક	cl;		// byte length of current char
	પૂર્ણાંક	comping;	// current char is composing
	પૂર્ણાંક	scol = col;	// screen column
	પૂર્ણાંક	curr_wide = FALSE;  // use 'guifontwide'
	પૂર્ણાંક	prev_wide = FALSE;
	પૂર્ણાંક	wide_changed;
# ifdef MSWIN
	પૂર્ણાંક	sep_comp = FALSE;   // Don't separate composing chars.
# નહીંતર
	પૂર્ણાંક	sep_comp = TRUE;    // Separate composing chars.
# endif

	// Break the string at a composing character, it has to be drawn on
	// top of the previous character.
	start = 0;
	cells = 0;
	માટે (i = 0; i < len; i += cl)
	{
	    c = utf_ptr2char(s + i);
	    cn = utf_char2cells(c);
	    comping = utf_iscomposing(c);
	    જો (!comping)	// count cells from non-composing chars
		cells += cn;
	    જો (!comping || sep_comp)
	    {
		જો (cn > 1
# ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
# endif
			&& wide_font != NOFONT)
		    curr_wide = TRUE;
		નહીંતર
		    curr_wide = FALSE;
	    }
	    cl = utf_ptr2len(s + i);
	    જો (cl == 0)	// hit end of string
		len = i + cl;	// len must be wrong "cannot happen"

	    wide_changed = curr_wide != prev_wide;

	    // Print the string so far if it's the last character or there is
	    // a composing character.
	    જો (i + cl >= len || (comping && sep_comp && i > start)
		    || wide_changed
# જો defined(FEAT_GUI_X11)
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
		જો ((comping && sep_comp) || wide_changed)
		    thislen = i - start;
		નહીંતર
		    thislen = i - start + cl;
		જો (thislen > 0)
		{
		    જો (prev_wide)
			gui_mch_set_font(wide_font);
		    gui_mch_draw_string(gui.row, scol, s + start, thislen,
								  draw_flags);
		    જો (prev_wide)
			gui_mch_set_font(font);
		    start += thislen;
		}
		scol += cells;
		cells = 0;
		// Adjust to not draw a character which width is changed
		// against with last one.
		જો (wide_changed && !(comping && sep_comp))
		{
		    scol -= cn;
		    cl = 0;
		}

# જો defined(FEAT_GUI_X11)
		// No fontset: draw a space to fill the gap after a wide char
		//
		જો (cn > 1 && (draw_flags & DRAW_TRANSP) == 0
#  ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
#  endif
			&& !wide_changed)
		    gui_mch_draw_string(gui.row, scol - 1, (char_u *)" ",
							       1, draw_flags);
# endif
	    }
	    // Draw a composing char on top of the previous char.
	    જો (comping && sep_comp)
	    {
# જો defined(__APPLE_CC__) && TARGET_API_MAC_CARBON
		// Carbon ATSUI autodraws composing char over previous char
		gui_mch_draw_string(gui.row, scol, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# નહીંતર
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
    નહીંતર
    {
	gui_mch_draw_string(gui.row, col, s, len, draw_flags);
	જો (enc_dbcs == DBCS_JPNU)
	{
	    // Get the length in display cells, this can be different from the
	    // number of bytes for "euc-jp".
	    len = mb_string2cells(s, len);
	}
    }
#endif // !FEAT_GUI_GTK

    જો (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
	gui.col = col + len;

    // May need to invert it when it's part of the selection.
    જો (flags & GUI_MON_NOCLEAR)
	clip_may_redraw_selection(gui.row, col, len);

    જો (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
    {
	// Invalidate the old physical cursor position if we wrote over it
	જો (gui.cursor_row == gui.row
		&& gui.cursor_col >= col
		&& gui.cursor_col < col + len)
	    gui.cursor_is_valid = false;
    }

#ifdef FEAT_SIGN_ICONS
    જો (draw_sign)
	// Draw the sign on top of the spaces.
	gui_mch_drawsign(gui.row, signcol, gui.highlight_mask);
# જો defined(FEAT_NETBEANS_INTG) && (defined(FEAT_GUI_X11) \
	|| defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN))
    જો (multi_sign)
	netbeans_draw_multisign_indicator(gui.row);
# endif
#endif

    પાછા OK;
}

/*
 * Undraw the cursor.  This actually redraws the character at the cursor
 * position, plus some more characters when needed.
 */
    શૂન્ય
gui_undraw_cursor(શૂન્ય)
{
    જો (!gui.cursor_is_valid)
	પાછા;

    // Always redraw the character just before if there is one, because
    // with some fonts and characters there can be a one pixel overlap.
    પૂર્ણાંક startcol = gui.cursor_col > 0 ? gui.cursor_col - 1 : gui.cursor_col;
    પૂર્ણાંક endcol = gui.cursor_col;

#જો defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    gui_adjust_undraw_cursor_for_ligatures(&startcol, &endcol);
#endif
    gui_redraw_block(gui.cursor_row, startcol,
	    gui.cursor_row, endcol, GUI_MON_NOCLEAR);

    // Cursor_is_valid is reset when the cursor is undrawn, also reset it
    // here in case it wasn't needed to undraw it.
    gui.cursor_is_valid = false;
}

    શૂન્ય
gui_redraw(
    પૂર્ણાંક		x,
    પૂર્ણાંક		y,
    પૂર્ણાંક		w,
    પૂર્ણાંક		h)
{
    પૂર્ણાંક		row1, col1, row2, col2;

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
    જો (gui.row == gui.cursor_row)
	gui_update_cursor(TRUE, TRUE);
}

/*
 * Draw a rectangular block of characters, from row1 to row2 (inclusive) and
 * from col1 to col2 (inclusive).
 */
    શૂન્ય
gui_redraw_block(
    પૂર્ણાંક		row1,
    પૂર્ણાંક		col1,
    પૂર્ણાંક		row2,
    પૂર્ણાંક		col2,
    પૂર્ણાંક		flags)	// flags for gui_outstr_nowrap()
{
    પૂર્ણાંક		old_row, old_col;
    long_u	old_hl_mask;
    પૂર્ણાંક		off;
    sattr_T	first_attr;
    પૂર્ણાંક		idx, len;
    પૂર્ણાંક		back, nback;
    પૂર્ણાંક		orig_col1, orig_col2;

    // Don't try to update when ScreenLines is not valid
    જો (!screen_cleared || Screen_લીટીઓ == ખાલી)
	પાછા;

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

    માટે (gui.row = row1; gui.row <= row2; gui.row++)
    {
	// When only half of a double-wide character is in the block, include
	// the other half.
	col1 = orig_col1;
	col2 = orig_col2;
	off = લીટી_Offset[gui.row];
	જો (enc_dbcs != 0)
	{
	    જો (col1 > 0)
		col1 -= dbcs_screen_head_off(Screen_લીટીઓ + off,
						    Screen_લીટીઓ + off + col1);
	    col2 += dbcs_screen_tail_off(Screen_લીટીઓ + off,
						    Screen_લીટીઓ + off + col2);
	}
	નહીંતર જો (enc_utf8)
	{
	    જો (Screen_લીટીઓ[off + col1] == 0)
	    {
		જો (col1 > 0)
		    --col1;
		નહીંતર
		    // FIXME: how can the first character ever be zero?
		    siemsg("NUL in ScreenLines in row %ld", (long)gui.row);
	    }
#ifdef FEAT_GUI_GTK
	    જો (col2 + 1 < Columns && Screen_લીટીઓ[off + col2 + 1] == 0)
		++col2;
#endif
	}
	gui.col = col1;
	off = લીટી_Offset[gui.row] + gui.col;
	len = col2 - col1 + 1;

	// Find how many chars back this highlighting starts, or where a space
	// is.  Needed for when the bold trick is used
	માટે (back = 0; back < col1; ++back)
	    જો (ScreenAttrs[off - 1 - back] != ScreenAttrs[off]
		    || Screen_લીટીઓ[off - 1 - back] == ' ')
		તોડો;

	// Break it up in strings of characters with the same attributes.
	// Print UTF-8 characters individually.
	જ્યારે (len > 0)
	{
	    first_attr = ScreenAttrs[off];
	    gui.highlight_mask = first_attr;
#જો !defined(FEAT_GUI_GTK)
	    જો (enc_utf8 && Screen_લીટીઓ_UC[off] != 0)
	    {
		// output multi-byte character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
		જો (gui.col < Columns && Screen_લીટીઓ[off + 1] == 0)
		    idx = 2;
		નહીંતર
		    idx = 1;
	    }
	    નહીંતર જો (enc_dbcs == DBCS_JPNU && Screen_લીટીઓ[off] == 0x8e)
	    {
		// output double-byte, single-width character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
		idx = 1;
	    }
	    નહીંતર
#endif
	    {
#ifdef FEAT_GUI_GTK
		માટે (idx = 0; idx < len; ++idx)
		{
		    જો (enc_utf8 && Screen_લીટીઓ[off + idx] == 0)
			ચાલુ; // skip second half of double-width char
		    જો (ScreenAttrs[off + idx] != first_attr)
			તોડો;
		}
		// gui_screenstr() takes care of multibyte chars
		nback = gui_screenstr(off, idx, flags,
				      (guicolor_T)0, (guicolor_T)0, back);
#નહીંતર
		માટે (idx = 0; idx < len && ScreenAttrs[off + idx] == first_attr;
									idx++)
		{
		    // Stop at a multi-byte Unicode character.
		    જો (enc_utf8 && Screen_લીટીઓ_UC[off + idx] != 0)
			તોડો;
		    જો (enc_dbcs == DBCS_JPNU)
		    {
			// Stop at a double-byte single-width char.
			જો (Screen_લીટીઓ[off + idx] == 0x8e)
			    તોડો;
			જો (len > 1 && (*mb_ptr2len)(Screen_લીટીઓ
							    + off + idx) == 2)
			    ++idx;  // skip second byte of double-byte char
		    }
		}
		nback = gui_outstr_nowrap(Screen_લીટીઓ + off, idx, flags,
					  (guicolor_T)0, (guicolor_T)0, back);
#endif
	    }
	    જો (nback == FAIL)
	    {
		// Must back up to start drawing where a bold or italic word
		// starts.
		off -= back;
		len += back;
		gui.col -= back;
	    }
	    નહીંતર
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
    gui.highlight_mask = (પૂર્ણાંક)old_hl_mask;
}

    સ્થિર શૂન્ય
gui_delete_લીટીઓ(પૂર્ણાંક row, પૂર્ણાંક ગણતરી)
{
    જો (ગણતરી <= 0)
	પાછા;

    જો (row + ગણતરી > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_block(row, gui.scroll_region_ડાબું,
			      gui.scroll_region_bot, gui.scroll_region_જમણું);
    નહીંતર
    {
	gui_mch_delete_લીટીઓ(row, ગણતરી);

	// If the cursor was in the deleted lines it's now gone.  If the
	// cursor was in the scrolled lines adjust its position.
	જો (gui.cursor_row >= row
		&& gui.cursor_col >= gui.scroll_region_ડાબું
		&& gui.cursor_col <= gui.scroll_region_જમણું)
	{
	    જો (gui.cursor_row < row + ગણતરી)
		gui.cursor_is_valid = false;
	    નહીંતર જો (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_row -= ગણતરી;
	}
    }
}

    સ્થિર શૂન્ય
gui_ઉમેરો_લીટીઓ(પૂર્ણાંક row, પૂર્ણાંક ગણતરી)
{
    જો (ગણતરી <= 0)
	પાછા;

    જો (row + ગણતરી > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_block(row, gui.scroll_region_ડાબું,
			      gui.scroll_region_bot, gui.scroll_region_જમણું);
    નહીંતર
    {
	gui_mch_ઉમેરો_લીટીઓ(row, ગણતરી);

	જો (gui.cursor_row >= gui.row
		&& gui.cursor_col >= gui.scroll_region_ડાબું
		&& gui.cursor_col <= gui.scroll_region_જમણું)
	{
	    જો (gui.cursor_row <= gui.scroll_region_bot - ગણતરી)
		gui.cursor_row += ગણતરી;
	    નહીંતર જો (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_is_valid = false;
	}
    }
}

#ifdef FEAT_TIMERS
/*
 * Passed to ui_wait_for_chars_or_timer(), ignoring extra arguments.
 */
    સ્થિર પૂર્ણાંક
gui_wait_for_chars_3(
    long wtime,
    પૂર્ણાંક *interrupted UNUSED,
    પૂર્ણાંક ignore_આગત UNUSED)
{
    પાછા gui_mch_wait_for_chars(wtime);
}
#endif

/*
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    સ્થિર પૂર્ણાંક
gui_wait_for_chars_or_timer(
	long wtime,
	પૂર્ણાંક *interrupted UNUSED,
	પૂર્ણાંક ignore_આગત UNUSED)
{
#ifdef FEAT_TIMERS
    પાછા ui_wait_for_chars_or_timer(wtime, gui_wait_for_chars_3,
						    interrupted, ignore_આગત);
#નહીંતર
    પાછા gui_mch_wait_for_chars(wtime);
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
    સ્થિર પૂર્ણાંક
gui_wait_for_chars_buf(
    char_u	*buf,
    પૂર્ણાંક		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    પૂર્ણાંક		tb_change_cnt)
{
    પૂર્ણાંક	    retval;
    પૂર્ણાંક	    keep_blinking; // Guard against restarting blink cycle on CursorHold

#ifdef FEAT_MENU
    // If we're going to wait a bit, update the menus and mouse shape for the
    // current State.
    જો (wtime != 0)
	gui_update_menus(0);
#endif

    gui_mch_update();
    જો (આગત_available())	// Got char, return immediately
    {
	જો (gui_mch_is_blinking())
	    gui_mch_stop_blink(TRUE);
	જો (buf != ખાલી && !typebuf_changed(tb_change_cnt))
	    પાછા read_from_આગત_buf(buf, (long)maxlen);
	પાછા 0;
    }
    જો (wtime == 0)		// Don't wait for char
	પાછા FAIL;

    // Before waiting, flush any output to the screen.
    gui_mch_flush();

    // Blink while waiting for a character.
    જો (!gui_mch_is_blinking())
	gui_mch_start_blink();

    // Common function to loop until "wtime" is met, while handling timers and
    // other callbacks.
    retval = inchar_loop(buf, maxlen, wtime, tb_change_cnt,
			 gui_wait_for_chars_or_timer, ખાલી);

    // Keep blinking when CursorHold wakes the input loop. (See PR #21115)
    keep_blinking = retval == 3 && buf != ખાલી
	&& buf[0] == K_SPECIAL && buf[1] == KS_EXTRA
	&& buf[2] == (પૂર્ણાંક)KE_CURSORHOLD;

    જો (!keep_blinking)
	gui_mch_stop_blink(TRUE);

    પાછા retval;
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
    પૂર્ણાંક
gui_wait_for_chars(long wtime, પૂર્ણાંક tb_change_cnt)
{
    પાછા gui_wait_for_chars_buf(ખાલી, 0, wtime, tb_change_cnt);
}

/*
 * Equivalent of mch_inchar() for the GUI.
 */
    પૂર્ણાંક
gui_inchar(
    char_u  *buf,
    પૂર્ણાંક	    maxlen,
    long    wtime,		// milliseconds
    પૂર્ણાંક	    tb_change_cnt)
{
    પાછા gui_wait_for_chars_buf(buf, maxlen, wtime, tb_change_cnt);
}

/*
 * Fill p[4] with mouse coordinates encoded for check_termcode().
 */
    સ્થિર શૂન્ય
fill_mouse_coord(char_u *p, પૂર્ણાંક col, પૂર્ણાંક row)
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
    શૂન્ય
gui_send_mouse_event(
    પૂર્ણાંક	    button,
    પૂર્ણાંક	    x,
    પૂર્ણાંક	    y,
    પૂર્ણાંક	    repeated_click,
    int_u   modifiers)
{
    સ્થિર પૂર્ણાંક	    prev_row = 0, prev_col = 0;
    સ્થિર પૂર્ણાંક	    prev_button = -1;
    સ્થિર પૂર્ણાંક	    અંક_clicks = 1;
    char_u	    string[10];
    ગણના ચાવી_extra  button_char;
    પૂર્ણાંક		    row, col;
#ifdef FEAT_CLIPBOARD
    પૂર્ણાંક		    checkfor;
    પૂર્ણાંક		    did_clip = FALSE;
#endif

    /*
     * Scrolling may happen at any time, also while a selection is present.
     */
    પસંદગી (button)
    {
	સ્થિતિ MOUSE_MOVE:
	    button_char = KE_MOUSEMOVE_XY;
	    જાઓ button_set;
	સ્થિતિ MOUSE_X1:
	    button_char = KE_X1MOUSE;
	    જાઓ button_set;
	સ્થિતિ MOUSE_X2:
	    button_char = KE_X2MOUSE;
	    જાઓ button_set;
	સ્થિતિ MOUSE_4:
	    button_char = KE_MOUSEDOWN;
	    જાઓ button_set;
	સ્થિતિ MOUSE_5:
	    button_char = KE_MOUSEUP;
	    જાઓ button_set;
	સ્થિતિ MOUSE_6:
	    button_char = KE_MOUSELEFT;
	    જાઓ button_set;
	સ્થિતિ MOUSE_7:
	    button_char = KE_MOUSERIGHT;
button_set:
	    {
		// Don't put events in the input queue now.
		જો (hold_gui_events)
		    પાછા;

		row = gui_xy2colrow(x, y, &col);
		// Don't report a mouse move unless moved to a
		// different character position.
		જો (button == MOUSE_MOVE)
		{
		    જો (row == prev_row && col == prev_col)
			પાછા;
		    નહીંતર
		    {
			prev_row = row >= 0 ? row : 0;
			prev_col = col;
		    }
		}

		string[3] = CSI;
		string[4] = KS_EXTRA;
		string[5] = (પૂર્ણાંક)button_char;

		// Pass the pointer coordinates of the scroll event so that we
		// know which window to scroll.
		string[6] = (char_u)(col / 128 + ' ' + 1);
		string[7] = (char_u)(col % 128 + ' ' + 1);
		string[8] = (char_u)(row / 128 + ' ' + 1);
		string[9] = (char_u)(row % 128 + ' ' + 1);

		જો (modifiers == 0)
		    add_to_આગત_buf(string + 3, 7);
		નહીંતર
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = 0;
		    જો (modifiers & MOUSE_SHIFT)
			string[2] |= MOD_MASK_SHIFT;
		    જો (modifiers & MOUSE_CTRL)
			string[2] |= MOD_MASK_CTRL;
		    જો (modifiers & MOUSE_ALT)
			string[2] |= MOD_MASK_ALT;
		    add_to_આગત_buf(string, 10);
		}
		પાછા;
	    }
    }

#ifdef FEAT_CLIPBOARD
    // If a clipboard selection is in progress, handle it
    જો (clip_star.state == SELECT_IN_PROGRESS)
    {
	clip_process_selection(button, X_2_COL(x), Y_2_ROW(y), repeated_click);

	// A release event may still need to be sent if the position is equal.
	row = gui_xy2colrow(x, y, &col);
	જો (button != MOUSE_RELEASE || row != prev_row || col != prev_col)
	    પાછા;
    }

    // Determine which mouse settings to look for based on the current mode
    પસંદગી (get_real_state())
    {
	સ્થિતિ MODE_NORMAL_BUSY:
	સ્થિતિ MODE_OP_PENDING:
# ifdef FEAT_TERMINAL
	સ્થિતિ MODE_TERMINAL:
# endif
	સ્થિતિ MODE_NORMAL:	checkfor = MOUSE_NORMAL; તોડો;
	સ્થિતિ MODE_VISUAL:	checkfor = MOUSE_VISUAL; તોડો;
	સ્થિતિ MODE_SELECT:	checkfor = MOUSE_VISUAL; તોડો;
	સ્થિતિ MODE_REPLACE:
	સ્થિતિ MODE_REPLACE | MODE_LANGMAP:
	સ્થિતિ MODE_VREPLACE:
	સ્થિતિ MODE_VREPLACE | MODE_LANGMAP:
	સ્થિતિ MODE_ઉમેરો:
	સ્થિતિ MODE_ઉમેરો | MODE_LANGMAP:
				checkfor = MOUSE_ઉમેરો; તોડો;
	સ્થિતિ MODE_ASKMORE:
	સ્થિતિ MODE_HITRETURN:	// At the more- and hit-enter prompt pass the
				// mouse event for a click on or below the
				// message line.
				જો (Y_2_ROW(y) >= સંદેશ_row)
				    checkfor = MOUSE_NORMAL;
				નહીંતર
				    checkfor = MOUSE_RETURN;
				તોડો;

	    /*
	     * On the command line, use the clipboard selection on all lines
	     * but the command line.  But not when pasting.
	     */
	સ્થિતિ MODE_CMDLINE:
	સ્થિતિ MODE_CMDLINE | MODE_LANGMAP:
	    જો (Y_2_ROW(y) < cmdline_row && button != MOUSE_મધ્ય)
		checkfor = MOUSE_NONE;
	    નહીંતર
		checkfor = MOUSE_COMMAND;
	    તોડો;

	મૂળભૂત:
	    checkfor = MOUSE_NONE;
	    તોડો;
    };

    /*
     * Allow clipboard selection of text on the command line in "normal"
     * modes.  Don't do this when dragging the status line, or extending a
     * Visual selection.
     */
    જો ((State == MODE_NORMAL || State == MODE_NORMAL_BUSY
						      || (State & MODE_ઉમેરો))
	    && X_2_COL(x) >= firstwin->w_wincol
	    && X_2_COL(x) < firstwin->w_wincol + topframe->fr_width
	    && Y_2_ROW(y) >= topframe->fr_height + firstwin->w_winrow
	    && button != MOUSE_DRAG
# ifdef FEAT_MOUSESHAPE
	    && !drag_status_લીટી
	    && !drag_sep_લીટી
# endif
	    )
	checkfor = MOUSE_NONE;

    /*
     * Use modeless selection when holding CTRL and SHIFT pressed.
     */
    જો ((modifiers & MOUSE_CTRL) && (modifiers & MOUSE_SHIFT))
	checkfor = MOUSE_NONEF;

    /*
     * In Ex mode, always use modeless selection.
     */
    જો (exmode_active)
	checkfor = MOUSE_NONE;

    /*
     * If the mouse settings say to not use the mouse, use the modeless
     * selection.  But if Visual is active, assume that only the Visual area
     * will be selected.
     * Exception: On the command line, both the selection is used and a mouse
     * key is sent.
     */
    જો (!mouse_has(checkfor) || checkfor == MOUSE_COMMAND)
    {
	// Don't do modeless selection in Visual mode.
	જો (checkfor != MOUSE_NONEF && VIsual_active && (State & MODE_NORMAL))
	    પાછા;

	/*
	 * When 'mousemodel' is "popup", shift-left is translated to right.
	 * But not when also using Ctrl.
	 */
	જો (mouse_model_popup() && button == MOUSE_ડાબું
		&& (modifiers & MOUSE_SHIFT) && !(modifiers & MOUSE_CTRL))
	{
	    button = MOUSE_જમણું;
	    modifiers &= ~ MOUSE_SHIFT;
	}

	// If the selection is done, allow the right button to extend it.
	// If the selection is cleared, allow the right button to start it
	// from the cursor position.
	જો (button == MOUSE_જમણું)
	{
	    જો (clip_star.state == SELECT_CLEARED)
	    {
		જો (State & MODE_CMDLINE)
		{
		    col = સંદેશ_col;
		    row = સંદેશ_row;
		}
		નહીંતર
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
	નહીંતર જો (button == MOUSE_ડાબું)
	{
	    clip_start_selection(X_2_COL(x), Y_2_ROW(y), repeated_click);
	    did_clip = TRUE;
	}

	// Always allow pasting
	જો (button != MOUSE_મધ્ય)
	{
	    જો (!mouse_has(checkfor) || button == MOUSE_RELEASE)
		પાછા;
	    જો (checkfor != MOUSE_COMMAND)
		button = MOUSE_ડાબું;
	}
	repeated_click = FALSE;
    }

    જો (clip_star.state != SELECT_CLEARED && !did_clip)
	clip_clear_selection(&clip_star);
#endif

    // Don't put events in the input queue now.
    જો (hold_gui_events)
	પાછા;

    row = gui_xy2colrow(x, y, &col);

    /*
     * If we are dragging and the mouse hasn't moved far enough to be on a
     * different character, then don't send an event to vim.
     */
    જો (button == MOUSE_DRAG)
    {
	જો (row == prev_row && col == prev_col)
	    પાછા;
	// Dragging above the window, set "row" to -1 to cause a scroll.
	જો (y < 0)
	    row = -1;
    }

    /*
     * If topline has changed (window scrolled) since the last click, reset
     * repeated_click, because we don't want starting Visual mode when
     * clicking on a different character in the text.
     */
    જો (curwin->w_topline != gui_prev_topline
#ifdef FEAT_DIFF
	    || curwin->w_topfill != gui_prev_topfill
#endif
	    )
	repeated_click = FALSE;

    string[0] = CSI;	// this sequence is recognized by check_termcode()
    string[1] = KS_MOUSE;
    string[2] = KE_FILLER;
    જો (button != MOUSE_DRAG && button != MOUSE_RELEASE)
    {
	જો (repeated_click)
	{
	    /*
	     * Handle multiple clicks.	They only count if the mouse is still
	     * pointing at the same character.
	     */
	    જો (button != prev_button || row != prev_row || col != prev_col)
		અંક_clicks = 1;
	    નહીંતર જો (++અંક_clicks > 4)
		અંક_clicks = 1;
	}
	નહીંતર
	    અંક_clicks = 1;
	prev_button = button;
	gui_prev_topline = curwin->w_topline;
#ifdef FEAT_DIFF
	gui_prev_topfill = curwin->w_topfill;
#endif

	string[3] = (char_u)(button | 0x20);
	SET_અંક_MOUSE_CLICKS(string[3], અંક_clicks);
    }
    નહીંતર
	string[3] = (char_u)button;

    string[3] |= modifiers;
    fill_mouse_coord(string + 4, col, row);
    add_to_આગત_buf(string, 8);

    જો (row < 0)
	prev_row = 0;
    નહીંતર
	prev_row = row;
    prev_col = col;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#જો defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#endif
}

/*
 * Convert x and y coordinate to column and row in text window.
 * Corrects for multi-byte character.
 * returns column in "*colp" and row as return value;
 */
    સ્થિર પૂર્ણાંક
gui_xy2colrow(પૂર્ણાંક x, પૂર્ણાંક y, પૂર્ણાંક *colp)
{
    પૂર્ણાંક		col = check_col(X_2_COL(x));
    પૂર્ણાંક		row = check_row(Y_2_ROW(y));

    *colp = mb_fix_col(col, row);
    પાછા row;
}

#જો defined(FEAT_MENU)
/*
 * Callback function for when a menu entry has been selected.
 */
    શૂન્ય
gui_menu_cb(vimmenu_T *menu)
{
    char_u  bytes[કદ(long_u)];

    // Don't put events in the input queue now.
    જો (hold_gui_events)
	પાછા;

    bytes[0] = CSI;
    bytes[1] = KS_MENU;
    bytes[2] = KE_FILLER;
    add_to_આગત_buf(bytes, 3);
    add_long_to_buf((long_u)menu, bytes);
    add_to_આગત_buf_csi(bytes, કદ(long_u));
}
#endif

સ્થિર પૂર્ણાંક	prev_which_scrollbars[3];

/*
 * Set which components are present.
 * If "oldval" is not NULL, "oldval" is the previous value, the new value is
 * in p_go.
 */
    શૂન્ય
gui_init_which_components(char_u *oldval UNUSED)
{
#ifdef FEAT_GUI_DARKTHEME
    સ્થિર પૂર્ણાંક	prev_dark_theme = FALSE;
    પૂર્ણાંક		using_dark_theme = FALSE;
#endif
#ifdef FEAT_MENU
    સ્થિર પૂર્ણાંક	prev_menu_is_active = -1;
#endif
#ifdef FEAT_TOOLBAR
    સ્થિર પૂર્ણાંક	prev_toolbar = -1;
    પૂર્ણાંક		using_toolbar = FALSE;
#endif
#ifdef FEAT_GUI_TABLINE
    પૂર્ણાંક		using_tabline;
#endif
#ifdef FEAT_GUI_MSWIN
    સ્થિર પૂર્ણાંક	prev_titlebar = FALSE;
    પૂર્ણાંક		using_titlebar = FALSE;
#endif

#જો defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    સ્થિર પૂર્ણાંક	prev_fullscreen = FALSE;
    પૂર્ણાંક		using_fullscreen = FALSE;
#endif
#જો defined(FEAT_MENU)
    સ્થિર પૂર્ણાંક	prev_tearoff = -1;
    પૂર્ણાંક		using_tearoff = FALSE;
#endif

    char_u	*p;
    પૂર્ણાંક		i;
#ifdef FEAT_MENU
    પૂર્ણાંક		grey_old, grey_new;
    char_u	*temp;
#endif
    win_T	*wp;
    પૂર્ણાંક		need_set_size;
    પૂર્ણાંક		fix_size;

#ifdef FEAT_MENU
    જો (oldval != ખાલી && gui.in_use)
    {
	/*
	 * Check if the menus go from grey to non-grey or vice versa.
	 */
	grey_old = (vim_strchr(oldval, GO_GREY) != ખાલી);
	grey_new = (vim_strchr(p_go, GO_GREY) != ખાલી);
	જો (grey_old != grey_new)
	{
	    temp = p_go;
	    p_go = oldval;
	    gui_update_menus(MENU_ALL_MODES);
	    p_go = temp;
	}
    }
    gui.menu_is_active = false;
#endif

    માટે (i = 0; i < 3; i++)
	gui.which_scrollbars[i] = false;
    માટે (p = p_go; *p; p++)
	પસંદગી (*p)
	{
	    સ્થિતિ GO_ડાબું:
		gui.which_scrollbars[SBAR_ડાબું] = true;
		તોડો;
	    સ્થિતિ GO_જમણું:
		gui.which_scrollbars[SBAR_જમણું] = true;
		તોડો;
	    સ્થિતિ GO_VLEFT:
		જો (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_ડાબું] = true;
		તોડો;
	    સ્થિતિ GO_VRIGHT:
		જો (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_જમણું] = true;
		તોડો;
	    સ્થિતિ GO_BOT:
		gui.which_scrollbars[SBAR_BOTTOM] = true;
		તોડો;
#ifdef FEAT_GUI_DARKTHEME
	    સ્થિતિ GO_DARKTHEME:
		using_dark_theme = TRUE;
		તોડો;
#endif
#ifdef FEAT_MENU
	    સ્થિતિ GO_MENUS:
		gui.menu_is_active = true;
		તોડો;
#endif
	    સ્થિતિ GO_GREY:
		// make menu's have grey items, ignored here
		તોડો;
#ifdef FEAT_GUI_MSWIN
	    સ્થિતિ GO_TITLEBAR:
		using_titlebar = TRUE;
		તોડો;
#endif
#જો defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
	    સ્થિતિ GO_FULLSCREEN:
		using_fullscreen = TRUE;
		તોડો;
#endif
#ifdef FEAT_TOOLBAR
	    સ્થિતિ GO_TOOLBAR:
		using_toolbar = TRUE;
		તોડો;
#endif
	    સ્થિતિ GO_TEAROFF:
#જો defined(FEAT_MENU)
		using_tearoff = TRUE;
#endif
		તોડો;
	    મૂળભૂત:
		// Ignore options that are not supported
		તોડો;
	}

    જો (!gui.in_use)
	પાછા;

    need_set_size = 0;
    fix_size = FALSE;

#ifdef FEAT_GUI_MSWIN
    જો (using_titlebar != prev_titlebar)
    {
	gui_mch_set_titlebar_colors();
	prev_titlebar = using_titlebar;
    }
#endif

#જો defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    જો (using_fullscreen != prev_fullscreen)
    {
	gui_mch_set_fullscreen(using_fullscreen);
	prev_fullscreen = using_fullscreen;
    }
#endif

#ifdef FEAT_GUI_DARKTHEME
    જો (using_dark_theme != prev_dark_theme)
    {
	gui_mch_set_dark_theme(using_dark_theme);
	prev_dark_theme = using_dark_theme;
    }
#endif

#ifdef FEAT_GUI_TABLINE
    // Update the GUI tab line, it may appear or disappear.  This may
    // cause the non-GUI tab line to disappear or appear.
    using_tabline = gui_has_tabline();
    જો (!gui_mch_showing_tabline() != !using_tabline)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = Rows;
	gui_update_tabline();
	Rows = i;
	need_set_size |= RESIZE_VERT;
	જો (using_tabline)
	    fix_size = TRUE;
	જો (!gui_use_tabline())
	    redraw_tabline = TRUE;    // may draw non-GUI tab line
    }
#endif

    માટે (i = 0; i < 3; i++)
    {
	// The scrollbar needs to be updated when it is shown/unshown and
	// when switching tab pages.  But the size only changes when it's
	// shown/unshown.  Thus we need two places to remember whether a
	// scrollbar is there or not.
	જો (gui.which_scrollbars[i] != prev_which_scrollbars[i]
		|| gui.which_scrollbars[i]
		!= curtab->tp_prev_which_scrollbars[i])
	{
	    જો (i == SBAR_BOTTOM)
		gui_mch_enable_scrollbar(&gui.bottom_sbar,
			gui.which_scrollbars[i]);
	    નહીંતર
	    {
		FOR_ALL_WINDOWS(wp)
		    gui_do_scrollbar(wp, i, gui.which_scrollbars[i]);
	    }
	    જો (gui.which_scrollbars[i] != prev_which_scrollbars[i])
	    {
		જો (i == SBAR_BOTTOM)
		    need_set_size |= RESIZE_VERT;
		નહીંતર
		    need_set_size |= RESIZE_HOR;
		જો (gui.which_scrollbars[i])
		    fix_size = TRUE;
	    }
	}
	curtab->tp_prev_which_scrollbars[i] = gui.which_scrollbars[i];
	prev_which_scrollbars[i] = gui.which_scrollbars[i];
    }

#ifdef FEAT_MENU
    જો (gui.menu_is_active != prev_menu_is_active)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = Rows;
	gui_mch_enable_menu(gui.menu_is_active);
	Rows = i;
	prev_menu_is_active = gui.menu_is_active;
	need_set_size |= RESIZE_VERT;
	જો (gui.menu_is_active)
	    fix_size = TRUE;
    }
#endif

#ifdef FEAT_TOOLBAR
    જો (using_toolbar != prev_toolbar)
    {
	gui_mch_show_toolbar(using_toolbar);
	prev_toolbar = using_toolbar;
	need_set_size |= RESIZE_VERT;
	જો (using_toolbar)
	    fix_size = TRUE;
    }
#endif
#જો defined(FEAT_MENU) && !(defined(MSWIN) && !defined(FEAT_TEAROFF))
    જો (using_tearoff != prev_tearoff)
    {
	gui_mch_toggle_tearoffs(using_tearoff);
	prev_tearoff = using_tearoff;
    }
#endif
    જો (need_set_size != 0)
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

	    જો (!gui.starting && need_set_size != (RESIZE_VERT | RESIZE_HOR))
		(શૂન્ય)char_avail();
	    જો ((need_set_size & RESIZE_VERT) == 0)
		Rows = prev_Rows;
	    નહીંતર
		Rows = post_Rows;
	    જો ((need_set_size & RESIZE_HOR) == 0)
		Columns = prev_Columns;
	    નહીંતર
		Columns = post_Columns;
	}
#endif
    }
    // When the console tabline appears or disappears the window positions
    // change.
    જો (firstwin->w_winrow != tabline_height())
	shell_new_rows();	// recompute window positions and heights
}

#જો defined(FEAT_GUI_TABLINE)
/*
 * Return TRUE if the GUI is taking care of the tabline.
 * It may still be hidden if 'showtabline' is zero.
 */
    પૂર્ણાંક
gui_use_tabline(શૂન્ય)
{
    પાછા gui.in_use && vim_strchr(p_go, GO_TABLINE) != ખાલી;
}

/*
 * Return TRUE if the GUI is showing the tabline.
 * This uses 'showtabline'.
 */
    સ્થિર પૂર્ણાંક
gui_has_tabline(શૂન્ય)
{
    જો (!gui_use_tabline()
	    || p_stal == 0
	    || (p_stal == 1 && first_tabpage->tp_next == ખાલી))
	પાછા FALSE;
    પાછા TRUE;
}

/*
 * Update the tabline.
 * This may display/undisplay the tabline and update the labels.
 */
    શૂન્ય
gui_update_tabline(શૂન્ય)
{
    પૂર્ણાંક	    showit = gui_has_tabline();
    પૂર્ણાંક	    shown = gui_mch_showing_tabline();

    જો (!gui.starting && starting == 0)
    {
	// Updating the tabline uses direct GUI commands, flush
	// outstanding instructions first. (esp. clear screen)
	out_flush();

	જો (!showit != !shown)
	{
	    // Save Rows/Columns before showing/hiding the tabline.
	    // gui_mch_show_tabline() processes GTK events (via
	    // gui_mch_update) which may trigger a spurious configure event
	    // that temporarily corrupts Rows.  Restore the original values so
	    // that gui_set_shellsize() uses the correct row/column count.
	    પૂર્ણાંક save_Rows = Rows;
	    પૂર્ણાંક save_Columns = Columns;

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
	જો (showit != 0)
	    gui_mch_update_tabline();
    }
}

/*
 * Get the label or tooltip for tab page "tp" into NameBuff[].
 */
    શૂન્ય
get_tabline_label(
    tabpage_T	*tp,
    પૂર્ણાંક		tooltip)	// TRUE: get tooltip
{
    char_u	**opt;

    // Use 'guitablabel' or 'guitabtooltip' if it's set.
    opt = (tooltip ? &p_gtt : &p_gtl);
    જો (**opt != NUL)
    {
	char_u	res[MAXPATHL];
	tabpage_T *save_curtab;
	char_u	*opt_name = (char_u *)(tooltip ? "guitabtooltip"
							     : "guitablabel");

	printer_page_અંક = tabpage_index(tp);
# ifdef FEAT_EVAL
	set_vim_var_nr(VV_LNUM, printer_page_અંક);
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
					   0, (પૂર્ણાંક)Columns, ખાલી, ખાલી, ખાલી);
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
    જો (**opt == NUL || *NameBuff == NUL)
    {
	win_T	*wp;
	પૂર્ણાંક	wincount = 0;
	પૂર્ણાંક	modified = FALSE;

	// Get the buffer name into NameBuff[] and shorten it.
	get_trans_bufname(tp == curtab ? curbuf : tp->tp_curwin->w_buffer);
	જો (!tooltip)
	    shorten_dir(NameBuff);

	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    ++wincount;
	    જો (bufIsChanged(wp->w_buffer))
		modified = TRUE;
	}

	જો (modified || wincount > 1)
	{
	    char_u  buf[40] = "+ ";		// Tentatively assume modified only
	    size_t  buflen = 2;
	    size_t  NameBufflen = STRLEN(NameBuff);

	    જો (wincount > 1)
		buflen = vim_snprintf_safelen(
		    (અક્ષર *)buf,
		    કદ(buf),
		    "%d%s ",
		    wincount,
		    modified ? "+" : "");

	    // Make sure resulting NameBuff will not exceed its bounds.
	    જો (NameBufflen + buflen < MAXPATHL)
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
    પૂર્ણાંક
send_tabline_event(પૂર્ણાંક nr)
{
    char_u string[3];

    જો (nr == tabpage_index(curtab))
	પાછા FALSE;

    // Don't put events in the input queue now.
    જો (hold_gui_events || cmdwin_type != 0)
    {
	// Set it back to the current tab page.
	gui_mch_set_curtab(tabpage_index(curtab));
	પાછા FALSE;
    }

    string[0] = CSI;
    string[1] = KS_TABLINE;
    string[2] = KE_FILLER;
    add_to_આગત_buf(string, 3);
    string[0] = nr;
    add_to_આગત_buf_csi(string, 1);
    પાછા TRUE;
}

/*
 * Send a tabline menu event
 */
    શૂન્ય
send_tabline_menu_event(પૂર્ણાંક tabidx, પૂર્ણાંક event)
{
    char_u	    string[3];

    // Don't put events in the input queue now.
    જો (hold_gui_events)
	પાછા;

    // Cannot close the last tabpage.
    જો (event == TABLINE_MENU_CLOSE && first_tabpage->tp_next == ખાલી)
	પાછા;

    string[0] = CSI;
    string[1] = KS_TABMENU;
    string[2] = KE_FILLER;
    add_to_આગત_buf(string, 3);
    string[0] = tabidx;
    string[1] = (char_u)(long)event;
    add_to_આગત_buf_csi(string, 2);
}

#endif

/*
 * Scrollbar stuff:
 */

/*
 * Remove all scrollbars.  Used before switching to another tab page.
 */
    શૂન્ય
gui_remove_scrollbars(શૂન્ય)
{
    પૂર્ણાંક	    i;
    win_T   *wp;

    માટે (i = 0; i < 3; i++)
    {
	જો (i == SBAR_BOTTOM)
	    gui_mch_enable_scrollbar(&gui.bottom_sbar, FALSE);
	નહીંતર
	{
	    FOR_ALL_WINDOWS(wp)
		gui_do_scrollbar(wp, i, FALSE);
	}
	curtab->tp_prev_which_scrollbars[i] = -1;
    }
}

    શૂન્ય
gui_create_scrollbar(scrollbar_T *sb, પૂર્ણાંક type, win_T *wp)
{
    સ્થિર પૂર્ણાંક	sbar_ident = 0;

    sb->ident = sbar_ident++;	// No check for too big, but would it happen?
    sb->wp = wp;
    sb->type = type;
    sb->કિંમત = 0;
    sb->size = 1;
    sb->max = 1;
    sb->ટોચ = 0;
    sb->height = 0;
    sb->width = 0;
    sb->status_height = 0;
    gui_mch_create_scrollbar(sb, (wp == ખાલી) ? SBAR_HORIZ : SBAR_VERT);
}

/*
 * Find the scrollbar with the given index.
 */
    scrollbar_T *
gui_find_scrollbar(long ident)
{
    win_T	*wp;

    જો (gui.bottom_sbar.ident == ident)
	પાછા &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	જો (wp->w_scrollbars[SBAR_ડાબું].ident == ident)
	    પાછા &wp->w_scrollbars[SBAR_ડાબું];
	જો (wp->w_scrollbars[SBAR_જમણું].ident == ident)
	    પાછા &wp->w_scrollbars[SBAR_જમણું];
    }
    પાછા ખાલી;
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
    શૂન્ય
gui_drag_scrollbar(scrollbar_T *sb, long કિંમત, પૂર્ણાંક still_dragging)
{
    win_T	*wp;
    પૂર્ણાંક		sb_અંક;
#ifdef USE_ON_FLY_SCROLL
    colnr_T	old_leftcol = curwin->w_leftcol;
    linenr_T	old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    પૂર્ણાંક		old_topfill = curwin->w_topfill;
# endif
#નહીંતર
    char_u	bytes[કદ(long_u)];
    પૂર્ણાંક		byte_ગણતરી;
#endif

    જો (sb == ખાલી)
	પાછા;

    // Don't put events in the input queue now.
    જો (hold_gui_events)
	પાછા;

    જો (cmdwin_type != 0 && sb->wp != cmdwin_win)
	પાછા;

    જો (still_dragging)
    {
	જો (sb->wp == ખાલી)
	    gui.dragged_sb = SBAR_BOTTOM;
	નહીંતર જો (sb == &sb->wp->w_scrollbars[SBAR_ડાબું])
	    gui.dragged_sb = SBAR_ડાબું;
	નહીંતર
	    gui.dragged_sb = SBAR_જમણું;
	gui.dragged_wp = sb->wp;
    }
    નહીંતર
    {
	gui.dragged_sb = SBAR_NONE;
#ifdef FEAT_GUI_GTK
	// Keep the "dragged_wp" value until after the scrolling, for when the
	// mouse button is released.  GTK2 doesn't send the button-up event.
	gui.dragged_wp = ખાલી;
	// WinScrolled event
	gui_focus_change(TRUE);
#endif
    }

    // Vertical sbar info is kept in the first sbar (the left one)
    જો (sb->wp != ખાલી)
	sb = &sb->wp->w_scrollbars[0];

    /*
     * Check validity of value
     */
    જો (કિંમત < 0)
	કિંમત = 0;
#ifdef SCROLL_PAST_END
    નહીંતર જો (કિંમત > sb->max)
	કિંમત = sb->max;
#નહીંતર
    જો (કિંમત > sb->max - sb->size + 1)
	કિંમત = sb->max - sb->size + 1;
#endif

    sb->કિંમત = કિંમત;

#ifdef USE_ON_FLY_SCROLL
    // When not allowed to do the scrolling right now, return.
    // This also checked input_available(), but that causes the first click in
    // a scrollbar to be ignored when Vim doesn't have focus.
    જો (dont_scroll)
	પાછા;
#endif
    // Disallow scrolling the current window when the completion popup menu is
    // visible.
    જો ((sb->wp == ખાલી || sb->wp == curwin) && pum_visible())
	પાછા;

#ifdef FEAT_RIGHTLEFT
    જો (sb->wp == ખાલી && curwin->w_p_rl)
    {
	કિંમત = sb->max + 1 - sb->size - કિંમત;
	જો (કિંમત < 0)
	    કિંમત = 0;
    }
#endif

    જો (sb->wp != ખાલી)		// vertical scrollbar
    {
	sb_અંક = 0;
	માટે (wp = firstwin; wp != sb->wp && wp != ખાલી; wp = wp->w_next)
	    sb_અંક++;
	જો (wp == ખાલી)
	    પાછા;

#ifdef USE_ON_FLY_SCROLL
	current_scrollbar = sb_અંક;
	scrollbar_કિંમત = કિંમત;
	જો (State & MODE_NORMAL)
	{
	    gui_do_scroll();
	    setcursor();
	}
	નહીંતર જો (State & MODE_ઉમેરો)
	{
	    ins_scroll();
	    setcursor();
	}
	નહીંતર જો (State & MODE_CMDLINE)
	{
	    જો (સંદેશ_scrolled == 0)
	    {
		gui_do_scroll();
		redrawcmdline();
	    }
	}
# ifdef FEAT_FOLDING
	// Value may have been changed for closed fold.
	sb->કિંમત = sb->wp->w_topline - 1;
# endif

	// When dragging one scrollbar and there is another one at the other
	// side move the thumb of that one too.
	જો (gui.which_scrollbars[SBAR_જમણું] && gui.which_scrollbars[SBAR_ડાબું])
	    gui_mch_set_scrollbar_thumb(
		    &sb->wp->w_scrollbars[
			    sb == &sb->wp->w_scrollbars[SBAR_જમણું]
						    ? SBAR_ડાબું : SBAR_જમણું],
		    sb->કિંમત, sb->size, sb->max);

#નહીંતર
	bytes[0] = CSI;
	bytes[1] = KS_VER_SCROLLBAR;
	bytes[2] = KE_FILLER;
	bytes[3] = (char_u)sb_અંક;
	byte_ગણતરી = 4;
#endif
    }
    નહીંતર
    {
#ifdef USE_ON_FLY_SCROLL
	scrollbar_કિંમત = કિંમત;

	જો (State & MODE_NORMAL)
	    do_mousescroll_horiz(scrollbar_કિંમત);
	નહીંતર જો (State & MODE_ઉમેરો)
	    ins_horscroll();
	નહીંતર જો (State & MODE_CMDLINE)
	{
	    જો (સંદેશ_scrolled == 0)
	    {
		do_mousescroll_horiz(scrollbar_કિંમત);
		redrawcmdline();
	    }
	}
	જો (old_leftcol != curwin->w_leftcol)
	{
	    updateWindow(curwin);   // update window, status and cmdline
	    setcursor();
	}
#નહીંતર
	bytes[0] = CSI;
	bytes[1] = KS_HOR_SCROLLBAR;
	bytes[2] = KE_FILLER;
	byte_ગણતરી = 3;
#endif
    }

#ifdef USE_ON_FLY_SCROLL
    /*
     * synchronize other windows, as necessary according to 'scrollbind'
     */
    જો (curwin->w_p_scb
	    && ((sb->wp == ખાલી && curwin->w_leftcol != old_leftcol)
		|| (sb->wp == curwin && (curwin->w_topline != old_topline
# ifdef FEAT_DIFF
					   || curwin->w_topfill != old_topfill
# endif
			))))
    {
	do_check_scrollbind(TRUE);
	// need to update the window right here
	FOR_ALL_WINDOWS(wp)
	    જો (wp->w_redr_type > 0)
		updateWindow(wp);
	setcursor();
    }
    out_flush_cursor(FALSE, TRUE);
#નહીંતર
    add_to_આગત_buf(bytes, byte_ગણતરી);
    add_long_to_buf((long_u)કિંમત, bytes);
    add_to_આગત_buf_csi(bytes, કદ(long_u));
#endif
}

/*
 * Scrollbar stuff:
 */

/*
 * Called when something in the window layout has changed.
 */
    શૂન્ય
gui_may_update_scrollbars(શૂન્ય)
{
    જો (gui.in_use && starting == 0)
    {
	out_flush();
	gui_init_which_components(ખાલી);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
}

    શૂન્ય
gui_update_scrollbars(
    પૂર્ણાંક		force)	    // Force all scrollbars to get updated
{
    win_T	*wp;
    scrollbar_T	*sb;
    long	val, size, max;		// need 32 bits here
    પૂર્ણાંક		which_sb;
    પૂર્ણાંક		h, y;
    સ્થિર win_T *prev_curwin = ખાલી;

    // Update the horizontal scrollbar
    gui_update_horiz_scrollbar(force);

#ifndef MSWIN
    // Return straight away if there is neither a left nor right scrollbar.
    // On MS-Windows this is required anyway for scrollwheel messages.
    જો (!gui.which_scrollbars[SBAR_ડાબું] && !gui.which_scrollbars[SBAR_જમણું])
	પાછા;
#endif

    /*
     * Don't want to update a scrollbar while we're dragging it.  But if we
     * have both a left and right scrollbar, and we drag one of them, we still
     * need to update the other one.
     */
    જો (!force && (gui.dragged_sb == SBAR_ડાબું || gui.dragged_sb == SBAR_જમણું)
	    && gui.which_scrollbars[SBAR_ડાબું]
	    && gui.which_scrollbars[SBAR_જમણું])
    {
	/*
	 * If we have two scrollbars and one of them is being dragged, just
	 * copy the scrollbar position from the dragged one to the other one.
	 */
	which_sb = SBAR_ડાબું + SBAR_જમણું - gui.dragged_sb;
	જો (gui.dragged_wp != ખાલી)
	    gui_mch_set_scrollbar_thumb(
		    &gui.dragged_wp->w_scrollbars[which_sb],
		    gui.dragged_wp->w_scrollbars[0].કિંમત,
		    gui.dragged_wp->w_scrollbars[0].size,
		    gui.dragged_wp->w_scrollbars[0].max);
    }

    // avoid that moving components around generates events
    ++hold_gui_events;

    FOR_ALL_WINDOWS(wp)
    {
	જો (wp->w_buffer == ખાલી)	// just in case
	    ચાલુ;
	// Skip a scrollbar that is being dragged.
	જો (!force && (gui.dragged_sb == SBAR_ડાબું
					     || gui.dragged_sb == SBAR_જમણું)
		&& gui.dragged_wp == wp)
	    ચાલુ;

#ifdef SCROLL_PAST_END
	max = wp->w_buffer->b_ml.ml_લીટી_ગણતરી - 1;
#નહીંતર
	max = wp->w_buffer->b_ml.ml_લીટી_ગણતરી + wp->w_height - 2;
#endif
	જો (max < 0)			// empty buffer
	    max = 0;
	val = wp->w_topline - 1;
	size = wp->w_height;
#ifdef SCROLL_PAST_END
	જો (val > max)			// just in case
	    val = max;
#નહીંતર
	જો (size > max + 1)		// just in case
	    size = max + 1;
	જો (val > max - size + 1)
	    val = max - size + 1;
#endif
	જો (val < 0)			// minimal value is 0
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
	જો (size < 1 || wp->w_botline - 2 > max)
	{
	    /*
	     * This can happen during changing files.  Just don't update the
	     * scrollbar for now.
	     */
	    sb->height = 0;	    // Force update next time
	    જો (gui.which_scrollbars[SBAR_ડાબું])
		gui_do_scrollbar(wp, SBAR_ડાબું, FALSE);
	    જો (gui.which_scrollbars[SBAR_જમણું])
		gui_do_scrollbar(wp, SBAR_જમણું, FALSE);
	    ચાલુ;
	}
	જો (force || sb->height != wp->w_height
	    || sb->ટોચ != wp->w_winrow
	    || sb->status_height != wp->w_status_height
	    || sb->width != wp->w_width
	    || prev_curwin != curwin)
	{
	    // Height, width or position of scrollbar has changed.  For
	    // vertical split: curwin changed.
	    sb->height = wp->w_height;
	    sb->ટોચ = wp->w_winrow;
	    sb->status_height = wp->w_status_height;
	    sb->width = wp->w_width;

	    // Calculate height and position in pixels
	    h = (sb->height + sb->status_height) * gui.char_height;
	    y = sb->ટોચ * gui.char_height + gui.border_offset;
#જો defined(FEAT_MENU) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF) && !defined(FEAT_GUI_PHOTON)
	    જો (gui.menu_is_active)
		y += gui.menu_height;
#endif

#જો defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU))
	    જો (vim_strchr(p_go, GO_TOOLBAR) != ખાલી)
		y += gui.toolbar_height;
#endif

#જો defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
	    જો (gui_has_tabline())
		y += gui.tabline_height;
#endif

	    જો (wp->w_winrow == 0)
	    {
		// Height of top scrollbar includes width of top border
		h += gui.border_offset;
		y -= gui.border_offset;
	    }
	    જો (gui.which_scrollbars[SBAR_ડાબું])
	    {
		gui_mch_set_scrollbar_pos(&wp->w_scrollbars[SBAR_ડાબું],
					  gui.ડાબું_sbar_x, y,
					  gui.scrollbar_width, h);
		gui_do_scrollbar(wp, SBAR_ડાબું, TRUE);
	    }
	    જો (gui.which_scrollbars[SBAR_જમણું])
	    {
		gui_mch_set_scrollbar_pos(&wp->w_scrollbars[SBAR_જમણું],
					  gui.જમણું_sbar_x, y,
					  gui.scrollbar_width, h);
		gui_do_scrollbar(wp, SBAR_જમણું, TRUE);
	    }
	}

	જો (force || sb->કિંમત != val || sb->size != size || sb->max != max)
	{
	    // Thumb of scrollbar has moved
	    sb->કિંમત = val;
	    sb->size = size;
	    sb->max = max;
	    જો (gui.which_scrollbars[SBAR_ડાબું]
		    && (gui.dragged_sb != SBAR_ડાબું || gui.dragged_wp != wp))
		gui_mch_set_scrollbar_thumb(&wp->w_scrollbars[SBAR_ડાબું],
					    val, size, max);
	    જો (gui.which_scrollbars[SBAR_જમણું]
		    && (gui.dragged_sb != SBAR_જમણું || gui.dragged_wp != wp))
		gui_mch_set_scrollbar_thumb(&wp->w_scrollbars[SBAR_જમણું],
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
    સ્થિર શૂન્ય
gui_do_scrollbar(
    win_T	*wp,
    પૂર્ણાંક		which,	    // SBAR_LEFT or SBAR_RIGHT
    પૂર્ણાંક		enable)	    // TRUE to enable scrollbar
{
    પૂર્ણાંક		midcol = curwin->w_wincol + curwin->w_width / 2;
    પૂર્ણાંક		has_midcol = (wp->w_wincol <= midcol
				     && wp->w_wincol + wp->w_width >= midcol);

    // Only enable scrollbars that contain the middle column of the current
    // window.
    જો (gui.which_scrollbars[SBAR_જમણું] != gui.which_scrollbars[SBAR_ડાબું])
    {
	// Scrollbars only on one side.  Don't enable scrollbars that don't
	// contain the middle column of the current window.
	જો (!has_midcol)
	    enable = FALSE;
    }
    નહીંતર
    {
	// Scrollbars on both sides.  Don't enable scrollbars that neither
	// contain the middle column of the current window nor are on the far
	// side.
	જો (midcol > Columns / 2)
	{
	    જો (which == SBAR_ડાબું ? wp->w_wincol != 0 : !has_midcol)
		enable = FALSE;
	}
	નહીંતર
	{
	    જો (which == SBAR_જમણું ? wp->w_wincol + wp->w_width != Columns
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
    પૂર્ણાંક
gui_do_scroll(શૂન્ય)
{
    win_T	*wp, *save_wp;
    પૂર્ણાંક		i;
    long	nlines;
    pos_T	old_cursor;
    linenr_T	old_topline;
#ifdef FEAT_DIFF
    પૂર્ણાંક		old_topfill;
#endif

    માટે (wp = firstwin, i = 0; i < current_scrollbar; wp = W_NEXT(wp), i++)
	જો (wp == ખાલી)
	    તોડો;
    જો (wp == ખાલી)
	// Couldn't find window
	પાછા FALSE;
    // don't redraw, LineOffset and similar are not valid!
    જો (exmode_active)
	પાછા FALSE;

    /*
     * Compute number of lines to scroll.  If zero, nothing to do.
     */
    nlines = (long)scrollbar_કિંમત + 1 - (long)wp->w_topline;
    જો (nlines == 0)
	પાછા FALSE;

    save_wp = curwin;
    old_topline = wp->w_topline;
#ifdef FEAT_DIFF
    old_topfill = wp->w_topfill;
#endif
    old_cursor = wp->w_cursor;
    curwin = wp;
    curbuf = wp->w_buffer;
    જો (nlines < 0)
	scrolldown(-nlines, gui.dragged_wp == ખાલી);
    નહીંતર
	scrollup(nlines, gui.dragged_wp == ખાલી);
    // Reset dragged_wp after using it.  "dragged_sb" will have been reset for
    // the mouse-up event already, but we still want it to behave like when
    // dragging.  But not the next click in an arrow.
    જો (gui.dragged_sb == SBAR_NONE)
	gui.dragged_wp = ખાલી;

    જો (old_topline != wp->w_topline
#ifdef FEAT_DIFF
	    || old_topfill != wp->w_topfill
#endif
	    )
    {
	જો (get_scrolloff_કિંમત() != 0)
	{
	    cursor_correct();		// fix window for 'so'
	    update_topline();		// avoid up/down jump
	}
	જો (old_cursor.lnum != wp->w_cursor.lnum)
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
    જો (Screen_લીટીઓ != ખાલી && (old_topline != wp->w_topline
	    || wp->w_redr_type != 0
#ifdef FEAT_DIFF
	    || old_topfill != wp->w_topfill
#endif
	    ))
    {
	પૂર્ણાંક type = UPD_VALID;

	જો (pum_visible())
	{
	    type = UPD_NOT_VALID;
	    wp->w_લીટીઓ_valid = 0;
	}

	// Don't set must_redraw here, it may cause the popup menu to
	// disappear when losing focus after a scrollbar drag.
	જો (wp->w_redr_type < type)
	    wp->w_redr_type = type;
	mch_disable_flush();
	updateWindow(wp);   // update window, status line, and cmdline
	mch_enable_flush();
    }

    // May need to redraw the popup menu.
    જો (pum_visible())
	pum_redraw();

    પાછા (wp == curwin && !EQUAL_POS(curwin->w_cursor, old_cursor));
}

/*
 * Horizontal scrollbar stuff:
 */
    સ્થિર શૂન્ય
gui_update_horiz_scrollbar(પૂર્ણાંક force)
{
    long	કિંમત, size, max;

    જો (!gui.which_scrollbars[SBAR_BOTTOM])
	પાછા;

    જો (!force && gui.dragged_sb == SBAR_BOTTOM)
	પાછા;

    જો (!force && curwin->w_p_wrap && gui.prev_wrap)
	પાછા;

    /*
     * It is possible for the cursor to be invalid if we're in the middle of
     * something (like changing files).  If so, don't do anything for now.
     */
    જો (curwin->w_cursor.lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
    {
	gui.bottom_sbar.કિંમત = -1;
	પાછા;
    }

    size = curwin->w_width;
    જો (curwin->w_p_wrap)
    {
	કિંમત = 0;
#ifdef SCROLL_PAST_END
	max = 0;
#નહીંતર
	max = curwin->w_width - 1;
#endif
    }
    નહીંતર
    {
	કિંમત = curwin->w_leftcol;
	max = scroll_લીટી_len(ui_find_longest_lnum());

	જો (virtual_active())
	{
	    // May move the cursor even further to the right.
	    જો (curwin->w_virtcol >= (colnr_T)max)
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
    જો (કિંમત > max - size + 1)
	કિંમત = max - size + 1;	    // limit the value to allowable range
#endif

#ifdef FEAT_RIGHTLEFT
    જો (curwin->w_p_rl)
    {
	કિંમત = max + 1 - size - કિંમત;
	જો (કિંમત < 0)
	{
	    size += કિંમત;
	    કિંમત = 0;
	}
    }
#endif
    જો (!force && કિંમત == gui.bottom_sbar.કિંમત && size == gui.bottom_sbar.size
						&& max == gui.bottom_sbar.max)
	પાછા;

    gui.bottom_sbar.કિંમત = કિંમત;
    gui.bottom_sbar.size = size;
    gui.bottom_sbar.max = max;
    gui.prev_wrap = curwin->w_p_wrap;

    gui_mch_set_scrollbar_thumb(&gui.bottom_sbar, કિંમત, size, max);
}

/*
 * Check that none of the colors are the same as the background color
 */
    શૂન્ય
gui_check_colors(શૂન્ય)
{
    જો (gui.norm_pixel == gui.back_pixel || gui.norm_pixel == INVALCOLOR)
    {
	gui_set_bg_color((char_u *)"White");
	જો (gui.norm_pixel == gui.back_pixel || gui.norm_pixel == INVALCOLOR)
	    gui_set_fg_color((char_u *)"Black");
    }
}

    સ્થિર શૂન્ય
gui_set_fg_color(char_u *name)
{
    gui.norm_pixel = gui_get_color(name);
    hl_set_fg_color_name(vim_strsave(name));
}

    સ્થિર શૂન્ય
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

    જો (*name == NUL)
	પાછા INVALCOLOR;
    t = gui_mch_get_color(name);

    પૂર્ણાંક is_none = STRCMP(name, "none") == 0;
    જો (t == INVALCOLOR
#જો defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
	    && (gui.in_use || is_none)
#endif
	    )
    {
	જો (is_none)
	    emsg(_(e_cannot_use_color_none_did_you_mean_none));
	નહીંતર
	    semsg(_(e_cannot_allocate_color_str), name);
    }
    પાછા t;
}

/*
 * Return the grey value of a color (range 0-255).
 */
    પૂર્ણાંક
gui_get_lightness(guicolor_T pixel)
{
    long_u	rgb = (long_u)gui_mch_get_rgb(pixel);

    પાછા  (પૂર્ણાંક)(  (((rgb >> 16) & 0xff) * 299)
		   + (((rgb >> 8) & 0xff) * 587)
		   +  ((rgb	  & 0xff) * 114)) / 1000;
}

    char_u *
gui_bg_default(શૂન્ય)
{
    જો (gui_get_lightness(gui.back_pixel) < 127)
	પાછા (char_u *)"dark";
    પાછા (char_u *)"light";
}

/*
 * Option initializations that can only be done after opening the GUI window.
 */
    સ્થિર શૂન્ય
init_gui_options(શૂન્ય)
{
    // Set the 'background' option according to the lightness of the
    // background color, unless the user has set it already.
    જો (!option_was_set((char_u *)"bg") && STRCMP(p_bg, gui_bg_default()) != 0)
    {
	set_option_કિંમત_give_err((char_u *)"bg", 0L, gui_bg_default(), 0);
	highlight_changed();
    }
}

#જો defined(FEAT_GUI_X11)
    શૂન્ય
gui_new_scrollbar_colors(શૂન્ય)
{
    win_T	*wp;

    // Nothing to do if GUI hasn't started yet.
    જો (!gui.in_use)
	પાછા;

    FOR_ALL_WINDOWS(wp)
    {
	gui_mch_set_scrollbar_colors(&(wp->w_scrollbars[SBAR_ડાબું]));
	gui_mch_set_scrollbar_colors(&(wp->w_scrollbars[SBAR_જમણું]));
    }
    gui_mch_set_scrollbar_colors(&gui.bottom_sbar);
}
#endif

/*
 * Call this when focus has changed.
 */
    શૂન્ય
gui_focus_change(પૂર્ણાંક in_focus)
{
/*
 * Skip this code to avoid drawing the cursor when debugging and switching
 * between the debugger window and gvim.
 */
#જો 1
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
    જો (!hold_gui_events
# જો defined(FEAT_GUI_GTK) && defined(FEAT_GUI_DIALOG)
	    && gui.dialogs_active == 0
# endif
       )
    {
	char_u  bytes[3];

	bytes[0] = CSI;
	bytes[1] = KS_EXTRA;
	bytes[2] = in_focus ? (પૂર્ણાંક)KE_FOCUSGAINED : (પૂર્ણાંક)KE_FOCUSLOST;
	add_to_આગત_buf(bytes, 3);
    }
#endif
}

/*
 * When mouse moved: apply 'mousefocus'.
 * Also updates the mouse pointer shape.
 */
    સ્થિર શૂન્ય
gui_mouse_focus(પૂર્ણાંક x, પૂર્ણાંક y)
{
    win_T	*wp;
    char_u	st[8];

#ifdef FEAT_MOUSESHAPE
    // Get window pointer, and update mouse shape as well.
    wp = xy2win(x, y, IGNORE_POPUP);
#endif

    // Only handle this when 'mousefocus' set and ...
    જો (p_mousef
	    && !hold_gui_events		// not holding events
	    && (State & (MODE_NORMAL | MODE_ઉમેરો))// Normal/Visual/Insert mode
	    && State != MODE_HITRETURN	// but not hit-return prompt
	    && સંદેશ_scrolled == 0	// no scrolled message
	    && !need_mouse_correct	// not moving the pointer
	    && gui.in_focus)		// gvim in focus
    {
	// Don't move the mouse when it's left or right of the Vim window
	જો (x < 0 || x > Columns * gui.char_width)
	    પાછા;
#ifndef FEAT_MOUSESHAPE
	wp = xy2win(x, y, IGNORE_POPUP);
#endif
	જો (wp == curwin || wp == ખાલી)
	    પાછા;	// still in the same old window, or none at all

	// Ignore position in the tab pages line.
	જો (Y_2_ROW(y) < tabline_height())
	    પાછા;

	/*
	 * Format a mouse click on status line input,
	 * ala gui_send_mouse_event(0, x, y, 0, 0);
	 * Trick: Use a column number -1, so that get_pseudo_mouse_code() will
	 * generate a K_LEFTMOUSE_NM key code.
	 */
	જો (finish_op)
	{
	    // abort the current operator first
	    st[0] = ESC;
	    add_to_આગત_buf(st, 1);
	}
	st[0] = CSI;
	st[1] = KS_MOUSE;
	st[2] = KE_FILLER;
	st[3] = (char_u)MOUSE_ડાબું;
	fill_mouse_coord(st + 4,
		wp->w_wincol == 0 ? -1 : wp->w_wincol + MOUSE_COLOFF,
		wp->w_height + W_WINROW(wp));

	add_to_આગત_buf(st, 8);
	st[3] = (char_u)MOUSE_RELEASE;
	add_to_આગત_buf(st, 8);
#ifdef FEAT_GUI_GTK
	// Need to wake up the main loop
	જો (gtk_main_level() > 0)
	    gtk_main_quit();
#endif
    }
}

/*
 * Called when the mouse moved (but not when dragging).
 */
    શૂન્ય
gui_mouse_moved(પૂર્ણાંક x, પૂર્ણાંક y)
{
    // Ignore this while still starting up.
    જો (!gui.in_use || gui.starting)
	પાછા;

    // apply 'mousefocus' and pointer shape
    gui_mouse_focus(x, y);

    જો (p_mousemev
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
    પૂર્ણાંક		x, y;

    જો (!(gui.in_use && (p_mousef || popup == FIND_POPUP)))
	પાછા ખાલી;
    gui_mch_getmouse(&x, &y);

    // Only use the mouse when it's on the Vim window
    જો (x >= 0 && x <= Columns * gui.char_width
	    && y >= 0 && Y_2_ROW(y) >= tabline_height())
	પાછા xy2win(x, y, popup);
    પાછા ખાલી;
}

/*
 * Called when mouse should be moved to window with focus.
 */
    શૂન્ય
gui_mouse_correct(શૂન્ય)
{
    win_T	*wp = ખાલી;

    need_mouse_correct = FALSE;

    wp = gui_mouse_window(IGNORE_POPUP);
    જો (wp == curwin || wp == ખાલી)
	પાછા;

    // If in other than current window
    validate_cline_row();
    gui_mch_setmouse((પૂર્ણાંક)W_ENDCOL(curwin) * gui.char_width - 3,
	    (W_WINROW(curwin) + curwin->w_wrow) * gui.char_height
	    + (gui.char_height) / 2);
}

/*
 * Find window where the mouse pointer "x" / "y" coordinate is in.
 * As a side effect update the shape of the mouse pointer.
 */
    સ્થિર win_T *
xy2win(પૂર્ણાંક x, પૂર્ણાંક y, mouse_find_T popup)
{
    પૂર્ણાંક		row;
    પૂર્ણાંક		col;
    win_T	*wp;

    row = Y_2_ROW(y);
    col = X_2_COL(x);

    જો (row < 0 || col < 0)		// before first window
	પાછા ખાલી;
    wp = mouse_find_win(&row, &col, popup);
    જો (wp == ખાલી)
    {
#ifdef FEAT_MOUSESHAPE
	update_mouseshape(-2);
#endif
	પાછા ખાલી;
    }
#ifdef FEAT_MOUSESHAPE
    જો (State == MODE_HITRETURN || State == MODE_ASKMORE)
    {
	જો (Y_2_ROW(y) >= સંદેશ_row)
	    update_mouseshape(SHAPE_IDX_MOREL);
	નહીંતર
	    update_mouseshape(SHAPE_IDX_MORE);
    }
    નહીંતર જો (row >= wp->w_height + wp->w_status_height)	// below status line
	update_mouseshape(SHAPE_IDX_CLINE);
    નહીંતર જો (!(State & MODE_CMDLINE) && wp->w_vsep_width > 0 && col == wp->w_width
	    && (!(row >= wp->w_height && row < wp->w_height
	    + wp->w_status_height) || !stl_connected(wp)) && સંદેશ_scrolled == 0)
	update_mouseshape(SHAPE_IDX_VSEP);
    નહીંતર જો (!(State & MODE_CMDLINE) && wp->w_status_height > 0
	    && row >= wp->w_height && row < wp->w_height + wp->w_status_height
	    && સંદેશ_scrolled == 0)
	update_mouseshape(SHAPE_IDX_STATUS);
    નહીંતર
	update_mouseshape(-2);
#endif
    પાછા wp;
}

/*
 * ":gui" and ":gvim": Change from the terminal version to the GUI version.
 * File names may be given to redefine the args list.
 */
    શૂન્ય
ex_gui(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    /*
     * Check for "-f" argument: foreground, don't fork.
     * Also don't fork when started with "gvim -f".
     * Do fork when using "gui -b".
     */
    જો (arg[0] == '-'
	    && (arg[1] == 'f' || arg[1] == 'b')
	    && (arg[2] == NUL || VIM_ISWHITE(arg[2])))
    {
	gui.dofork = (arg[1] == 'b');
	eap->arg = skipwhite(eap->arg + 2);
    }
    જો (!gui.in_use)
    {
#જો defined(VIMDLL) && !defined(EXPERIMENTAL_GUI_CMD)
	જો (!gui.starting)
	{
	    emsg(_(e_gui_cannot_be_used_not_enabled_at_compile_time));
	    પાછા;
	}
#endif
	// Clear the command.  Needed for when forking+exiting, to avoid part
	// of the argument ending up after the shell prompt.
	સંદેશ_clr_eos_force();
#ifdef GUI_MAY_SPAWN
	જો (!ends_excmd2(eap->cmd, eap->arg))
	    gui_start(eap->arg);
	નહીંતર
#endif
	    gui_start(ખાલી);
#ifdef FEAT_JOB_CHANNEL
	channel_gui_register_all();
#endif
    }
    જો (!ends_excmd2(eap->cmd, eap->arg))
	ex_next(eap);
}

#જો (defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	    || defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON) \
	    || defined(FEAT_GUI_HAIKU)) \
	    && defined(FEAT_TOOLBAR)
/*
 * This is shared between Haiku, Motif, and GTK.
 */

/*
 * Callback function for do_in_runtimepath().
 */
    સ્થિર શૂન્ય
gfp_setname(char_u *fname, શૂન્ય *cookie)
{
    char_u	*gfp_buffer = cookie;

    જો (STRLEN(fname) >= MAXPATHL)
	*gfp_buffer = NUL;
    નહીંતર
	STRCPY(gfp_buffer, fname);
}

/*
 * Find the path of bitmap "name" with extension "ext" in 'runtimepath'.
 * Return FAIL for failure and OK if buffer[MAXPATHL] contains the result.
 */
    પૂર્ણાંક
gui_find_bitmap(char_u *name, char_u *buffer, અક્ષર *ext)
{
    જો (STRLEN(name) > MAXPATHL - 14)
	પાછા FAIL;
    vim_snprintf((અક્ષર *)buffer, MAXPATHL, "bitmaps/%s.%s", name, ext);
    જો (do_in_runtimepath(buffer, 0, gfp_setname, buffer) == FAIL
							    || *buffer == NUL)
	પાછા FAIL;
    પાછા OK;
}

# જો !defined(FEAT_GUI_GTK)
/*
 * Given the name of the "icon=" argument, try finding the bitmap file for the
 * icon.  If it is an absolute path name, use it as it is.  Otherwise append
 * "ext" and search for it in 'runtimepath'.
 * The result is put in "buffer[MAXPATHL]".  If something fails "buffer"
 * contains "name".
 */
    શૂન્ય
gui_find_iconfile(char_u *name, char_u *buffer, અક્ષર *ext)
{
    char_u	buf[MAXPATHL + 1];

    expand_env(name, buffer, MAXPATHL);
    જો (!mch_isFullName(buffer) && gui_find_bitmap(buffer, buf, ext) == OK)
	STRCPY(buffer, buf);
}
# endif
#endif

#જો defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)|| defined(FEAT_GUI_HAIKU)
    શૂન્ય
display_errors(શૂન્ય)
{
    char_u	*p;

    જો (isatty(2))
    {
	fflush(stderr);
	પાછા;
    }

    જો (error_ga.ga_data == ખાલી)
	પાછા;

    // avoid putting up a message box with blanks only
    માટે (p = (char_u *)error_ga.ga_data; *p != NUL; ++p)
	જો (!SAFE_isspace(*p))
	{
	    // Truncate a very long message, it will go off-screen.
	    જો (STRLEN(p) > 2000)
		STRCPY(p + 2000 - 14, "...(truncated)");
	    (શૂન્ય)do_dialog(VIM_ERROR, (char_u *)_("Error"),
		    p, (char_u *)_("&Ok"), 1, ખાલી, FALSE);
	    તોડો;
	}
    ga_clear(&error_ga);
}
#endif

#જો defined(NO_CONSOLE_આગત)
/*
 * Return TRUE if still starting up and there is no place to enter text.
 * For GTK and X11 we check if stderr is not a tty, which means we were
 * (probably) started from the desktop.  Also check stdin, "vim >& file" does
 * allow typing on stdin.
 */
    પૂર્ણાંક
no_console_આગત(શૂન્ય)
{
    પાછા ((!gui.in_use || gui.starting)
# ifndef NO_CONSOLE
	    && !isatty(0) && !isatty(2)
# endif
	    );
}
#endif

#જો defined(FIND_REPLACE_DIALOG) \
	|| defined(NEED_GUI_UPDATE_SCREEN)
/*
 * Update the current window and the screen.
 */
    શૂન્ય
gui_update_screen(શૂન્ય)
{
# ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_લીટી = 0;
    linenr_T	conceal_new_cursor_લીટી = 0;
    પૂર્ણાંક		conceal_update_લીટીઓ = FALSE;
# endif

    update_topline();
    validate_cursor();

    // Trigger CursorMoved if the cursor moved.
    જો (!finish_op && (has_cursormoved()
# ifdef FEAT_PROP_POPUP
		|| popup_visible
# endif
# ifdef FEAT_CONCEAL
		|| curwin->w_p_cole > 0
# endif
		) && !EQUAL_POS(last_cursormoved, curwin->w_cursor))
    {
	જો (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, ખાલી, ખાલી, FALSE, curbuf);
# ifdef FEAT_PROP_POPUP
	જો (popup_visible)
	    popup_check_cursor_pos();
# endif
# ifdef FEAT_CONCEAL
	જો (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_લીટી = last_cursormoved.lnum;
	    conceal_new_cursor_લીટી = curwin->w_cursor.lnum;
	    conceal_update_લીટીઓ = TRUE;
	}
# endif
	last_cursormoved = curwin->w_cursor;
    }

    જો (!finish_op)
	may_trigger_win_scrolled_resized();

# ifdef FEAT_CONCEAL
    જો (conceal_update_લીટીઓ
	    && (conceal_old_cursor_લીટી != conceal_new_cursor_લીટી
		|| conceal_cursor_લીટી(curwin)
		|| need_cursor_લીટી_redraw))
    {
	જો (conceal_old_cursor_લીટી != conceal_new_cursor_લીટી)
	    redrawWinline(curwin, conceal_old_cursor_લીટી);
	redrawWinline(curwin, conceal_new_cursor_લીટી);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_લીટી_redraw = FALSE;
    }
# endif
    update_screen(0);	// may need to update the screen
    setcursor();
    out_flush_cursor(TRUE, FALSE);
}
#endif

#જો defined(FIND_REPLACE_DIALOG)
/*
 * Get the text to use in a find/replace dialog.  Uses the last search pattern
 * if the argument is empty.
 * Returns an allocated string.
 */
    char_u *
get_find_dialog_text(
    char_u	*arg,
    પૂર્ણાંક		*wwordp,	// return: TRUE if \< \> found
    પૂર્ણાંક		*mcasep)	// return: TRUE if \C found
{
    char_u	*text;

    જો (*arg == NUL)
	text = last_શોધ_pat();
    નહીંતર
	text = arg;
    જો (text != ખાલી)
    {
	પૂર્ણાંક len = (પૂર્ણાંક)STRLEN(text);

	text = vim_strnsave(text, len);
	જો (text != ખાલી)
	{
	    પૂર્ણાંક i;

	    // Remove "\V"
	    જો (len >= 2 && STRNCMP(text, "\\V", 2) == 0)
	    {
		mch_memmove(text, text + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\c" and "\C" and remove.
	    જો (len >= 2 && *text == '\\' && (text[1] == 'c' || text[1] == 'C'))
	    {
		*mcasep = (text[1] == 'C');
		mch_memmove(text, text + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\<text\>" and remove.
	    જો (len >= 4
		    && STRNCMP(text, "\\<", 2) == 0
		    && STRNCMP(text + len - 2, "\\>", 2) == 0)
	    {
		*wwordp = TRUE;
		mch_memmove(text, text + 2, (size_t)(len - 4));
		text[len - 4] = NUL;
	    }

	    // Recognize "\/" or "\?" and remove.
	    માટે (i = 0; i + 1 < len; ++i)
		જો (text[i] == '\\' && (text[i + 1] == '/'
						       || text[i + 1] == '?'))
		{
		    mch_memmove(text + i, text + i + 1, (size_t)(len - i));
		    --len;
		}
	}
    }
    પાછા text;
}

/*
 * Handle the press of a button in the find-replace dialog.
 * Return TRUE when something was added to the input buffer.
 */
    પૂર્ણાંક
gui_do_findrepl(
    પૂર્ણાંક		flags,		// one of FRD_REPLACE, FRD_FINDNEXT, etc.
    char_u	*find_text,
    char_u	*repl_text,
    પૂર્ણાંક		down)		// Search downwards.
{
    garray_T	ga;
    પૂર્ણાંક		i;
    પૂર્ણાંક		type = (flags & FRD_TYPE_MASK);
    char_u	*p;
    regmatch_T	regmatch;
    પૂર્ણાંક		save_did_emsg = did_emsg;
    સ્થિર પૂર્ણાંક  busy = FALSE;

    // When the screen is being updated we should not change buffers and
    // windows structures, it may cause freed memory to be used.  Also don't
    // do this recursively (pressing "Find" quickly several times).
    જો (updating_screen || busy)
	પાછા FALSE;

    // refuse replace when text cannot be changed
    જો ((type == FRD_REPLACE || type == FRD_REPLACEALL) && text_locked())
	પાછા FALSE;

    busy = TRUE;

    ga_init2(&ga, 1, 100);
    જો (type == FRD_REPLACEALL)
	GA_CONCAT_LITERAL(&ga, "%s/");

    GA_CONCAT_LITERAL(&ga, "\\V");
    જો (flags & FRD_MATCH_CASE)
	GA_CONCAT_LITERAL(&ga, "\\C");
    નહીંતર
	GA_CONCAT_LITERAL(&ga, "\\c");
    જો (flags & FRD_WHOLE_WORD)
	GA_CONCAT_LITERAL(&ga, "\\<");
    // escape slash and backslash
    p = vim_strsave_escaped(find_text, (char_u *)"/\\");
    જો (p != ખાલી)
	ga_concat(&ga, p);
    vim_free(p);
    જો (flags & FRD_WHOLE_WORD)
	GA_CONCAT_LITERAL(&ga, "\\>");

    જો (type == FRD_REPLACEALL)
    {
	GA_CONCAT_LITERAL(&ga, "/");
	// Escape slash and backslash.
	// Also escape tilde and ampersand if 'magic' is set.
	p = vim_strsave_escaped(repl_text,
				p_magic ? (char_u *)"/\\~&" : (char_u *)"/\\");
	જો (p != ખાલી)
	    ga_concat(&ga, p);
	vim_free(p);
	GA_CONCAT_LITERAL(&ga, "/g");
    }
    ga_append(&ga, NUL);

    જો (type == FRD_REPLACE)
    {
	// Do the replacement when the text at the cursor matches.  Thus no
	// replacement is done if the cursor was moved!
	regmatch.regprog = vim_regcomp(ga.ga_data, RE_MAGIC + RE_STRING);
	regmatch.rm_ic = 0;
	જો (regmatch.regprog != ખાલી)
	{
	    p = ml_get_cursor();
	    જો (vim_regexec_nl(&regmatch, p, (colnr_T)0)
						   && regmatch.startp[0] == p)
	    {
		// Clear the command line to remove any old "No match"
		// error.
		સંદેશ_end_prompt();

		જો (u_save_cursor() == OK)
		{
		    // A button was pressed thus undo should be synced.
		    u_sync(FALSE);

		    del_bytes((long)(regmatch.endp[0] - regmatch.startp[0]),
								FALSE, FALSE);
		    ins_str(repl_text, STRLEN(repl_text));
		}
	    }
	    નહીંતર
		સંદેશ(_("No match at cursor, finding next"));
	    vim_regfree(regmatch.regprog);
	}
    }

    જો (type == FRD_REPLACEALL)
    {
	// A button was pressed, thus undo should be synced.
	u_sync(FALSE);
	do_cmdline_cmd(ga.ga_data);
    }
    નહીંતર
    {
	પૂર્ણાંક searchflags = શોધ_સંદેશ + શોધ_MARK;

	// Search for the next match.
	// Don't skip text under cursor for single replace.
	જો (type == FRD_REPLACE)
	    searchflags += શોધ_START;
	i = સંદેશ_scroll;
	જો (down)
	{
	    (શૂન્ય)do_શોધ(ખાલી, '/', '/', ga.ga_data, STRLEN(ga.ga_data), 1L, searchflags, ખાલી);
	}
	નહીંતર
	{
	    // We need to escape '?' if and only if we are searching in the up
	    // direction
	    p = vim_strsave_escaped(ga.ga_data, (char_u *)"?");
	    જો (p != ખાલી)
		(શૂન્ય)do_શોધ(ખાલી, '?', '?', p, STRLEN(p), 1L, searchflags, ખાલી);
	    vim_free(p);
	}

	સંદેશ_scroll = i;	    // don't let an error message set msg_scroll
    }

    // Don't want to pass did_emsg to other code, it may cause disabling
    // syntax HL if we were busy redrawing.
    did_emsg = save_did_emsg;

    જો (State & (MODE_NORMAL | MODE_ઉમેરો))
    {
	gui_update_screen();		// update the screen
	સંદેશ_didout = 0;			// overwrite any message
	need_wait_return = FALSE;	// don't wait for return
    }

    vim_free(ga.ga_data);
    busy = FALSE;
    પાછા (ga.ga_len > 0);
}

#endif

#જો defined(HAVE_DROP_FILE)
/*
 * Jump to the window at specified point (x, y).
 */
    સ્થિર શૂન્ય
gui_wingoto_xy(પૂર્ણાંક x, પૂર્ણાંક y)
{
    પૂર્ણાંક		row = Y_2_ROW(y);
    પૂર્ણાંક		col = X_2_COL(x);
    win_T	*wp;

    જો (row < 0 || col < 0)
	પાછા;

    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    જો (wp != ખાલી && wp != curwin)
	win_goto(wp);
}

/*
 * Function passed to handle_drop() for the actions to be done after the
 * argument list has been updated.
 */
    સ્થિર શૂન્ય
drop_callback(શૂન્ય *cookie)
{
    char_u	*p = cookie;
    પૂર્ણાંક		do_shorten = FALSE;

    // If Shift held down, change to first file's directory.  If the first
    // item is a directory, change to that directory (and let the explorer
    // plugin show the contents).
    જો (p != ખાલી)
    {
	જો (mch_isdir(p))
	{
	    જો (mch_chdir((અક્ષર *)p) == 0)
		do_shorten = TRUE;
	}
	નહીંતર જો (vim_chdirfile(p, "drop") == OK)
	    do_shorten = TRUE;
	vim_free(p);
	જો (do_shorten)
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
    શૂન્ય
gui_handle_drop(
    પૂર્ણાંક		x UNUSED,
    પૂર્ણાંક		y UNUSED,
    int_u	modifiers,
    char_u	**fnames,
    પૂર્ણાંક		ગણતરી)
{
    પૂર્ણાંક		i;
    char_u	*p;
    સ્થિર પૂર્ણાંક	entered = FALSE;

    /*
     * This function is called by event handlers.  Just in case we get a
     * second event before the first one is handled, ignore the second one.
     * Not sure if this can ever happen, just in case.
     */
    જો (entered)
	પાછા;
    entered = TRUE;

    /*
     * When the cursor is at the command line, add the file names to the
     * command line, don't edit the files.
     */
    જો (State & MODE_CMDLINE)
    {
	shorten_filenames(fnames, ગણતરી);
	માટે (i = 0; i < ગણતરી; ++i)
	{
	    જો (fnames[i] != ખાલી)
	    {
		જો (i > 0)
		    add_to_આગત_buf((char_u*)" ", 1);

		// We don't know what command is used thus we can't be sure
		// about which characters need to be escaped.  Only escape the
		// most common ones.
# ifdef BACKSLASH_IN_FILENAME
		p = vim_strsave_escaped(fnames[i], (char_u *)" \t\"|");
# નહીંતર
		p = vim_strsave_escaped(fnames[i], (char_u *)"\\ \t\"|");
# endif
		જો (p != ખાલી)
		    add_to_આગત_buf_csi(p, (પૂર્ણાંક)STRLEN(p));
		vim_free(p);
		vim_free(fnames[i]);
	    }
	}
	vim_free(fnames);
    }
    નહીંતર
    {
	// Go to the window under mouse cursor, then shorten given "fnames" by
	// current window, because a window can have local current dir.
	gui_wingoto_xy(x, y);
	shorten_filenames(fnames, ગણતરી);

	// If Shift held down, remember the first item.
	જો ((modifiers & MOUSE_SHIFT) != 0)
	    p = vim_strsave(fnames[0]);
	નહીંતર
	    p = ખાલી;

	// Handle the drop, :edit or :split to get to the file.  This also
	// frees fnames[].  Skip this if there is only one item, it's a
	// directory and Shift is held down.
	જો (ગણતરી == 1 && (modifiers & MOUSE_SHIFT) != 0
						     && mch_isdir(fnames[0]))
	{
	    vim_free(fnames[0]);
	    vim_free(fnames);
	    vim_free(p);
	}
	નહીંતર
	    handle_drop(ગણતરી, fnames, (modifiers & MOUSE_CTRL) != 0,
						     drop_callback, (શૂન્ય *)p);
    }

    entered = FALSE;
}
#endif

/*
 * Check if "key" is to interrupt us.  Handles a key that has not had modifiers
 * applied yet.
 * Return the key with modifiers applied if so, NUL if not.
 */
    પૂર્ણાંક
check_for_interrupt(પૂર્ણાંક ચાવી, પૂર્ણાંક modifiers_arg)
{
    પૂર્ણાંક modifiers = modifiers_arg;
    પૂર્ણાંક c = merge_modify_Other_ચાવીઓ(ચાવી, &modifiers);

    જો ((c == Ctrl_C && ctrl_c_interrupts)
#ifdef UNIX
	    || (intr_char != Ctrl_C && c == intr_char)
#endif
	    )
    {
	got_int = TRUE;
	પાછા c;
    }
    પાછા NUL;
}

/*
 * If the "--gui-log-file fname" argument is given write the dialog title and
 * message to a file and return TRUE.  Otherwise return FALSE.
 * When there is any problem opening the file or writing to the file this is
 * ignored, showing the dialog might get the test to get stuck.
 */
    પૂર્ણાંક
gui_dialog_log(char_u *title, char_u *message)
{
    char_u  *fname = get_gui_dialog_file();
    FILE    *fd;

    જો (fname == ખાલી)
	પાછા FALSE;

    fd = mch_fopen((અક્ષર *)fname, "a");
    જો (fd != ખાલી)
    {
	fprintf(fd, "%s: %s\n", title, message);
	fclose(fd);
    }
    પાછા TRUE;
}
