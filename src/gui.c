/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#అన్తర్భావ "vim.h"

// Structure containing all the GUI information
gui_T gui;

#ఒకవేళ defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
# define USE_సమితి_GUIFONTWIDE
స్థిర శూన్య సమితి_guifontwide(char_u *font_name);
#అయితే_ముగింపు
స్థిర శూన్య gui_check_pos(శూన్య);
స్థిర శూన్య gui_reset_scroll_region(శూన్య);
స్థిర శూన్య gui_outstr(char_u *, పూర్ణాంకం);
#నిర్వచితం_కాకపోతే USE_GTK4
స్థిర పూర్ణాంకం gui_screenchar(పూర్ణాంకం off, పూర్ణాంకం flags, guicolor_T fg, guicolor_T bg, పూర్ణాంకం వెనుక);
#అయితే_ముగింపు
స్థిర పూర్ణాంకం gui_outstr_nowrap(char_u *s, పూర్ణాంకం len, పూర్ణాంకం flags, guicolor_T fg, guicolor_T bg, పూర్ణాంకం వెనుక);
స్థిర శూన్య gui_delete_పంక్తులు(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం లెక్క);
స్థిర శూన్య gui_చొప్పించు_పంక్తులు(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం లెక్క);
స్థిర పూర్ణాంకం gui_xy2colrow(పూర్ణాంకం x, పూర్ణాంకం y, పూర్ణాంకం *colp);
#ఒకవేళ defined(FEAT_GUI_TABLINE)
స్థిర పూర్ణాంకం gui_has_tabline(శూన్య);
#అయితే_ముగింపు
స్థిర శూన్య gui_do_scrollbar(win_T *wp, పూర్ణాంకం which, పూర్ణాంకం enable);
స్థిర శూన్య gui_update_horiz_scrollbar(పూర్ణాంకం);
స్థిర శూన్య gui_సమితి_fg_color(char_u *name);
స్థిర శూన్య gui_సమితి_bg_color(char_u *name);
స్థిర శూన్య init_gui_options(శూన్య);
స్థిర win_T *xy2win(పూర్ణాంకం x, పూర్ణాంకం y, mouse_కనుగొను_T popup);

#నిర్వచితమైతే GUI_MAY_FORK
స్థిర శూన్య gui_do_fork(శూన్య);

స్థిర పూర్ణాంకం gui_read_బిడ్డ_pipe(పూర్ణాంకం fd);

// Return values for gui_read_child_pipe
గణన {
    GUI_బిడ్డ_IO_ERROR,
    GUI_బిడ్డ_OK,
    GUI_బిడ్డ_FAILED
};
#అయితే_ముగింపు

స్థిర శూన్య gui_attempt_ప్రారంభం(శూన్య);

స్థిర పూర్ణాంకం can_update_cursor = TRUE; // can display the cursor
స్థిర పూర్ణాంకం disable_flush = 0;	// If > 0, gui_mch_flush() is disabled.

/*
 * gui_start -- Called when user wants to start the GUI.
 *
 * Careful: This function can be called recursively when there is a ":gui"
 * command in the .gvimrc file.  Only the first call should fork, not the
 * recursive call.
 */
    శూన్య
gui_ప్రారంభం(char_u *arg వాడని)
{
    char_u	*old_పదం;
#నిర్వచితమైతే GUI_MAY_FORK
    స్థిర పూర్ణాంకం	recursive = 0;
#అయితే_ముగింపు
#ఒకవేళ defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
    అక్షరం	*సందేశం = ఖాళీ;
#అయితే_ముగింపు

    old_పదం = vim_strsave(T_NAME);

    settmode(TMODE_COOK);		// stop RAW mode
    ఒకవేళ (నిండిన_screen)
	cursor_on();			// needed for ":gui" in .vimrc
    నిండిన_screen = FALSE;

    // If GUI fails to start, then we will recover afterwards
    పదం_disable_dec();

#నిర్వచితమైతే GUI_MAY_FORK
    ++recursive;
    /*
     * Quit the current process and continue in the child.
     * Makes "gvim file" disconnect from the shell it was started in.
     * Don't do this when Vim was started with "-f" or the 'f' flag is present
     * in 'guioptions'.
     * Don't do this when there is a running job, we can only get the status
     * of a child from the parent.
     */
    ఒకవేళ (gui.dofork && !vim_strchr(p_go, GO_FORG) && recursive <= 1
# ifdef FEAT_పని_CHANNEL
	    && !పని_any_running()
# endif
	    )
    {
	gui_do_fork();
    }
    లేకపోతే
#అయితే_ముగింపు
#నిర్వచితమైతే GUI_MAY_SPAWN
    ఒకవేళ (gui.dospawn
# ifdef EXPERIMENTAL_GUI_CMD
	    && gui.dofork
# endif
	    && !vim_strchr(p_go, GO_FORG)
	    && !any_Buf_Is_మారింది()
# ifdef FEAT_పని_CHANNEL
	    && !పని_any_running()
# endif
	    )
    {
# ifdef EXPERIMENTAL_GUI_CMD
	సందేశం =
# endif
	    gui_mch_do_spawn(arg);
    }
    లేకపోతే
#అయితే_ముగింపు
    {
#నిర్వచితమైతే FEAT_GUI_GTK
	// If there is 'f' in 'guioptions' and specify -g argument,
	// gui_mch_init_check() was not called yet.
	ఒకవేళ (gui_mch_init_check() != OK)
	    getout_preserve_modified(1);
#అయితే_ముగింపు
	gui_attempt_ప్రారంభం();
    }

    ఒకవేళ (!gui.లోపల_use)			// failed to start GUI
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
	termcapinit(old_పదం);
	settmode(TMODE_RAW);		// restart RAW mode
	సమితి_title_defaults();		// set 'title' and 'icon' again
#నిర్వచితమైతే UNIX
	పదం_సమితి_win_resize(true);
#అయితే_ముగింపు
#ఒకవేళ defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
	ఒకవేళ (సందేశం != ఖాళీ)
	    emsg(సందేశం);
#అయితే_ముగింపు
    }
    లేకపోతే
    {
#నిర్వచితమైతే HAVE_CLIPMETHOD
	// Reset clipmethod to CLIPMETHOD_NONE
	ఎంచుకో_clipmethod();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_IMAGE
	update_cell_కొలత();
	update_image_backend();
#అయితే_ముగింపు
    }

#ఒకవేళ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    // Enable fullscreen mode
    ఒకవేళ (vim_strchr(p_go, GO_FULLSCREEN) != ఖాళీ)
       gui_mch_సమితి_fullscreen(TRUE);
#అయితే_ముగింపు

    vim_free(old_పదం);

    // If the GUI started successfully, trigger the GUIEnter event, otherwise
    // the GUIFailed event.
    gui_mch_update();
    apply_autocmds(gui.లోపల_use ? EVENT_GUIENTER : EVENT_GUIFAILED,
						   ఖాళీ, ఖాళీ, FALSE, curbuf);
#నిర్వచితమైతే GUI_MAY_FORK
    --recursive;
#అయితే_ముగింపు
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
    స్థిర శూన్య
gui_attempt_ప్రారంభం(శూన్య)
{
    స్థిర పూర్ణాంకం recursive = 0;

    ++recursive;
    gui.starting = true;

#నిర్వచితమైతే FEAT_GUI_GTK
    gui.event_సమయం = GDK_CURRENT_సమయం;
#అయితే_ముగింపు

    termcapinit((char_u *)"builtin_gui");
    gui.starting = recursive - 1;

#ఒకవేళ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
    ఒకవేళ (gui.లోపల_use)
    {
# ifdef FEAT_EVAL
	కిటికీ	x11_కిటికీ;
	Display	*x11_display;

	ఒకవేళ (gui_get_x11_windis(&x11_కిటికీ, &x11_display) == OK)
	    సమితి_vim_var_nr(VV_WINDOWID, (దీర్ఘ)x11_కిటికీ);
# endif
	// Display error messages in a dialog now.
	display_errors();
    }
#అయితే_ముగింపు
    --recursive;
}

#నిర్వచితమైతే GUI_MAY_FORK

// for waitpid()
# ఒకవేళ defined(HAVE_SYS_WAIT_H)
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
    స్థిర శూన్య
gui_do_fork(శూన్య)
{
    పూర్ణాంకం		pipefd[2];	// pipe between parent and child
    పూర్ణాంకం		pipe_error;
    పూర్ణాంకం		స్థితి_2;
    పూర్ణాంకం		exit_స్థితి;
    pid_t	pid = -1;

# ఒకవేళ defined(FEAT_RELTIME) && defined(PROF_NSEC)
    // a timer is not carried forward
    delete_timer();
# endif

    // Setup a pipe between the child and the parent, so that the parent
    // knows when the child has done the setsid() call and is allowed to
    // exit.
    pipe_error = (pipe(pipefd) < 0);
    pid = fork();
    ఒకవేళ (pid < 0)	    // Fork error
    {
	emsg(_(e_failed_గమ్యం_create_new_process_for_GUI));
	తిరిగి;
    }
    లేకపోతే ఒకవేళ (pid > 0)	    //మాతృ
    {
	// Give the child some time to do the setsid(), otherwise the
	// exit() may kill the child too (when starting gvim from inside a
	// gvim).
	ఒకవేళ (!pipe_error)
	{
	    // The read returns when the child closes the pipe (or when
	    // the child dies for some reason).
	    close(pipefd[1]);
	    స్థితి_2 = gui_read_బిడ్డ_pipe(pipefd[0]);
	    ఒకవేళ (స్థితి_2 == GUI_బిడ్డ_FAILED)
	    {
		// The child failed to start the GUI, so the caller must
		// continue. There may be more error information written
		// to stderr by the child.
		waitpid(pid, &exit_స్థితి, 0);
		emsg(_(e_the_బిడ్డ_process_failed_గమ్యం_ప్రారంభం_GUI));
		తిరిగి;
	    }
	    లేకపోతే ఒకవేళ (స్థితి_2 == GUI_బిడ్డ_IO_ERROR)
	    {
		pipe_error = TRUE;
	    }
	    // else GUI_CHILD_OK: parent exit
	}

	ఒకవేళ (pipe_error)
	    ui_delay(301L, TRUE);

	// When swapping screens we may need to go to the next line, e.g.,
	// after a hit-enter prompt and using ":gui".
	ఒకవేళ (newline_on_exit)
	    mch_errmsg("\r\n");

	/*
	 * The parent must skip the normal exit() processing, the child
	 * will do it.  For example, GTK messes up signals when exiting.
	 */
	_exit(0);
    }
    //బిడ్డ

# ifdef FEAT_GUI_GTK
    // Call gtk_init_check() here after fork(). See gui_init_check().
    ఒకవేళ (gui_mch_init_check() != OK)
	getout_preserve_modified(1);
# endif

# ఒకవేళ defined(HAVE_SETSID) || defined(HAVE_SETPGID)
    /*
     * Change our process group.  On some systems/shells a CTRL-C in the
     * shell where Vim was started would otherwise kill gvim!
     */
#  ఒకవేళ defined(HAVE_SETSID)
    (శూన్య)setsid();
#  లేకపోతే
    (శూన్య)setpgid(0, 0);
#  endif
# endif
    ఒకవేళ (!pipe_error)
	close(pipefd[0]);

# ఒకవేళ defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
    // Tell the session manager our new PID
    gui_mch_forked();
# endif

    // Try to start the GUI
    gui_attempt_ప్రారంభం();

    // Notify the parent
    ఒకవేళ (!pipe_error)
    {
	ఒకవేళ (gui.లోపల_use)
	    write_eintr(pipefd[1], "ok", 3);
	లేకపోతే
	    write_eintr(pipefd[1], "fail", 5);
	close(pipefd[1]);
    }

    // If we failed to start the GUI, exit now.
    ఒకవేళ (!gui.లోపల_use)
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
    స్థిర పూర్ణాంకం
gui_read_బిడ్డ_pipe(పూర్ణాంకం fd)
{
    దీర్ఘ	బైట్లు_read;
# define READ_బఫరు_కొలత 10
    అక్షరం	బఫరు[READ_బఫరు_కొలత];

    బైట్లు_read = read_eintr(fd, బఫరు, READ_బఫరు_కొలత - 1);
# undef READ_బఫరు_కొలత
    close(fd);
    ఒకవేళ (బైట్లు_read < 0)
	తిరిగి GUI_బిడ్డ_IO_ERROR;
    బఫరు[బైట్లు_read] = NUL;
    ఒకవేళ (STRCMP(బఫరు, "ok") == 0)
	తిరిగి GUI_బిడ్డ_OK;
    తిరిగి GUI_బిడ్డ_FAILED;
}

#అయితే_ముగింపు // GUI_MAY_FORK

/*
 * Call this when vim starts up, whether or not the GUI is started
 */
    శూన్య
gui_prepare(పూర్ణాంకం *argc, అక్షరం **argv)
{
    gui.లోపల_use = false;		    // No GUI yet (maybe later)
    gui.starting = false;	    // No GUI yet (maybe later)
    gui_mch_prepare(argc, argv);
}

/*
 * Try initializing the GUI and check if it can be started.
 * Used from main() to check early if "vim -g" can start the GUI.
 * Used from gui_init() to prepare for starting the GUI.
 * Returns FAIL or OK.
 */
    పూర్ణాంకం
gui_init_check(శూన్య)
{
    స్థిర పూర్ణాంకం ఫలితం = MAYBE;

    ఒకవేళ (ఫలితం != MAYBE)
    {
	ఒకవేళ (ఫలితం == FAIL)
	    emsg(_(e_cannot_ప్రారంభం_the_GUI));
	తిరిగి ఫలితం;
    }

    gui.shell_created = false;
    gui.dying = false;
    gui.లోపల_focus = true;		// so the guicursor setting works
    gui.dragged_sb = SBAR_NONE;
    gui.dragged_wp = ఖాళీ;
    gui.సూచిక_hidden = false;
    gui.నిలువు = 0;
    gui.అడ్డువరుస = 0;
    gui.అంకె_నిలువుల_సంఖ్య = Columns;
    gui.అంకె_అడ్డువరుసలు = అడ్డువరుసలు;

    gui.cursor_is_valid = false;
    gui.scroll_region_శిఖరం = 0;
    gui.scroll_region_bot = అడ్డువరుసలు - 1;
    gui.scroll_region_ఎడమ = 0;
    gui.scroll_region_కుడి = Columns - 1;
    gui.highlight_ముసుగు = HL_సాధారణ;
    gui.char_వెడల్పు = 1;
    gui.char_ఎత్తు = 1;
    gui.char_ascent = 0;
    gui.border_వెడల్పు = 0;

    gui.norm_font = NOFONT;
#నిర్వచితం_కాకపోతే FEAT_GUI_GTK
    gui.bold_font = NOFONT;
    gui.ital_font = NOFONT;
    gui.boldital_font = NOFONT;
# ifdef FEAT_XFONTSET
    gui.fontset = NOFONTSET;
# endif
#అయితే_ముగింపు
    gui.wide_font = NOFONT;
#నిర్వచితం_కాకపోతే FEAT_GUI_GTK
    gui.wide_bold_font = NOFONT;
    gui.wide_ital_font = NOFONT;
    gui.wide_boldital_font = NOFONT;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MENU
# ifndef FEAT_GUI_GTK
#  ifdef FONTSET_ALWAYS
    gui.menu_fontset = NOFONTSET;
#  లేకపోతే
    gui.menu_font = NOFONT;
#  endif
# endif
    gui.menu_is_చురుకైన = true;	    // default: include menu
# ifndef FEAT_GUI_GTK
    gui.menu_ఎత్తు = MENU_DEFAULT_ఎత్తు;
    gui.menu_వెడల్పు = 0;
# endif
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    gui.toolbar_ఎత్తు = 0;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_BEVAL_TIP
    gui.tooltip_fontset = NOFONTSET;
#అయితే_ముగింపు

    gui.scrollbar_వెడల్పు = gui.scrollbar_ఎత్తు = SB_DEFAULT_వెడల్పు;
    gui.prev_wrap = -1;

#ఒకవేళ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // Note: gui_set_ligatures() might already have been called e.g. from .vimrc,
    // and in that case we don't want to overwrite ligatures map that has already
    // been correctly populated (as that would lead to a cleared ligatures maps).
    ఒకవేళ (*p_guiligatures == NUL)
	CLEAR_క్షేత్రం(gui.ligatures_map);
#అయితే_ముగింపు

#ఒకవేళ defined(ALWAYS_USE_GUI) || defined(VIMDLL)
    ఫలితం = OK;
#లేకపోతే
# ifdef FEAT_GUI_GTK
    gui.is_x11 = false;
#  ifdef FEAT_GUI_DIALOG
    gui.dialogs_చురుకైన = 0;
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
    ఫలితం = gui.dofork ? gui_mch_early_init_check(TRUE) : gui_mch_init_check();
# లేకపోతే
    ఫలితం = gui_mch_init_check();
# endif
#అయితే_ముగింపు
    తిరిగి ఫలితం;
}

/*
 * This is the call which starts the GUI.
 */
    శూన్య
gui_init(శూన్య)
{
    win_T	*wp;
    tabpage_T	*tp;
    స్థిర పూర్ణాంకం	recursive = 0;

    /*
     * It's possible to use ":gui" in a .gvimrc file.  The first half of this
     * function will then be executed at the first call, the rest by the
     * recursive call.  This allow the shell to be opened halfway reading a
     * gvimrc file.
     */
    ఒకవేళ (!recursive)
    {
	++recursive;

	clip_init(TRUE);

	// If can't initialize, don't try doing the rest
	ఒకవేళ (gui_init_check() == FAIL)
	{
	    --recursive;
	    clip_init(FALSE);
	    తిరిగి;
	}

	/*
	 * Reset 'paste'.  It's useful in the terminal, but not in the GUI.  It
	 * breaks the Paste toolbar button.
	 */
	సమితి_option_విలువ_give_err((char_u *)"paste", 0L, ఖాళీ, 0);

	// Set t_Co to the number of colors: RGB.
	సమితి_color_లెక్క(256 * 256 * 256);

	/*
	 * Set up system-wide default menus.
	 */
#ఒకవేళ defined(SYS_MENU_FILE) && defined(FEAT_MENU)
	ఒకవేళ (vim_strchr(p_go, GO_NOSYSMENU) == ఖాళీ)
	{
	    sys_menu = TRUE;
	    do_ఆధారం((char_u *)SYS_MENU_FILE, FALSE, DOSO_NONE, ఖాళీ);
	    sys_menu = FALSE;
	}
#అయితే_ముగింపు

	/*
	 * Switch on the mouse by default, unless the user changed it already.
	 * This can then be changed in the .gvimrc.
	 */
	ఒకవేళ (!option_was_సమితి((char_u *)"mouse"))
	    సమితి_string_option_ప్రత్యక్ష((char_u *)"mouse", -1,
					   (char_u *)"a", OPT_FREE, SID_NONE);

	/*
	 * If -U option given, use only the initializations from that file and
	 * nothing else.  Skip all initializations for "-U NONE" or "-u NORC".
	 */
	ఒకవేళ (use_gvimrc != ఖాళీ)
	{
	    ఒకవేళ (STRCMP(use_gvimrc, "NONE") != 0
		    && STRCMP(use_gvimrc, "NORC") != 0
		    && do_ఆధారం(use_gvimrc, FALSE, DOSO_NONE, ఖాళీ) != OK)
		semsg(_(e_cannot_read_from_str), use_gvimrc);
	}
	లేకపోతే
	{
	    /*
	     * Get system wide defaults for gvim, only when file name defined.
	     */
#నిర్వచితమైతే SYS_GVIMRC_FILE
	    do_ఆధారం((char_u *)SYS_GVIMRC_FILE, FALSE, DOSO_NONE, ఖాళీ);
#అయితే_ముగింపు

	    /*
	     * Try to read GUI initialization commands from the following
	     * places:
	     * - environment variable GVIMINIT
	     * - the user gvimrc file (~/.gvimrc)
	     * - the second user gvimrc file ($VIM/.gvimrc for Dos)
	     * - the third user gvimrc file ($VIM/.gvimrc for Amiga)
	     * The first that exists is used, the rest is ignored.
	     */
	    ఒకవేళ (process_env((char_u *)"GVIMINIT", FALSE) == FAIL
		 && do_ఆధారం((char_u *)USR_GVIMRC_FILE, TRUE,
						     DOSO_GVIMRC, ఖాళీ) == FAIL
#నిర్వచితమైతే USR_GVIMRC_FILE2
		 && do_ఆధారం((char_u *)USR_GVIMRC_FILE2, TRUE,
						     DOSO_GVIMRC, ఖాళీ) == FAIL
#అయితే_ముగింపు
#నిర్వచితమైతే USR_GVIMRC_FILE3
		 && do_ఆధారం((char_u *)USR_GVIMRC_FILE3, TRUE,
						     DOSO_GVIMRC, ఖాళీ) == FAIL
#అయితే_ముగింపు
				)
	    {
#నిర్వచితమైతే USR_GVIMRC_FILE4
		(శూన్య)do_ఆధారం((char_u *)USR_GVIMRC_FILE4, TRUE,
							    DOSO_GVIMRC, ఖాళీ);
#అయితే_ముగింపు
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
	    ఒకవేళ (p_exrc)
	    {
#నిర్వచితమైతే UNIX
		{
		    stat_T s;

		    // if ".gvimrc" file is not owned by user, set 'secure'
		    //బహుళకం
		    ఒకవేళ (mch_stat(GVIMRC_FILE, &s) || s.st_uid != getuid())
			secure = p_secure;
		}
#లేకపోతే
		secure = p_secure;
#అయితే_ముగింపు

		ఒకవేళ (       fullpathcmp((char_u *)USR_GVIMRC_FILE,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#నిర్వచితమైతే SYS_GVIMRC_FILE
			&& fullpathcmp((char_u *)SYS_GVIMRC_FILE,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#అయితే_ముగింపు
#నిర్వచితమైతే USR_GVIMRC_FILE2
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE2,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#అయితే_ముగింపు
#నిర్వచితమైతే USR_GVIMRC_FILE3
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE3,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#అయితే_ముగింపు
#నిర్వచితమైతే USR_GVIMRC_FILE4
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE4,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#అయితే_ముగింపు
			)
		    do_ఆధారం((char_u *)GVIMRC_FILE, TRUE, DOSO_GVIMRC, ఖాళీ);

		ఒకవేళ (secure == 2)
		    need_wait_return = TRUE;
		secure = 0;
	    }
	}

	ఒకవేళ (need_wait_return || సందేశం_didany)
	    wait_return(TRUE);

	--recursive;
    }

    // If recursive call opened the shell, return here from the first call
    ఒకవేళ (gui.లోపల_use)
	తిరిగి;

    /*
     * Create the GUI shell.
     */
    gui.లోపల_use = true;		// Must be set after menus have been set up
#ఒకవేళ defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
    gui.scale = 1.0; // Default value
#అయితే_ముగింపు
    ఒకవేళ (gui_mch_init() == FAIL)
	వెళ్ళు error;

    // Avoid a delay for an error message that was printed in the terminal
    // where Vim was started.
    emsg_on_display = FALSE;
    సందేశం_scrolled = 0;
    clear_sb_పాఠ్యం(TRUE);
    need_wait_return = FALSE;
    సందేశం_didany = FALSE;

    /*
     * Check validity of any generic resources that may have been loaded.
     */
    ఒకవేళ (gui.border_వెడల్పు < 0)
	gui.border_వెడల్పు = 0;

    /*
     * Set up the fonts.  First use a font specified with "-fn" or "-font".
     */
    ఒకవేళ (font_వాదం != ఖాళీ)
	సమితి_option_విలువ_give_err((char_u *)"gfn",
					       0L, (char_u *)font_వాదం, 0);
    ఒకవేళ (
#నిర్వచితమైతే FEAT_XFONTSET
	    (*p_guifontset == NUL
	     || gui_init_font(p_guifontset, TRUE) == FAIL) &&
#అయితే_ముగింపు
	    gui_init_font(*p_guifont == NUL ? hl_get_font_name()
						  : p_guifont, FALSE) == FAIL)
    {
	emsg(_(e_cannot_ప్రారంభం_gui_no_valid_font_దొరికింది));
	వెళ్ళు error2;
    }
    ఒకవేళ (gui_get_wide_font() == FAIL)
	emsg(_(e_guifontwide_invalid));

    gui.అంకె_నిలువుల_సంఖ్య = Columns;
    gui.అంకె_అడ్డువరుసలు = అడ్డువరుసలు;
    gui_reset_scroll_region();

    // Create initial scrollbars
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_ఎడమ], SBAR_ఎడమ, wp);
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_కుడి], SBAR_కుడి, wp);
    }
    gui_create_scrollbar(&gui.bottom_sbar, SBAR_BOTTOM, ఖాళీ);

#నిర్వచితమైతే FEAT_MENU
    gui_create_initial_menus(మూలం_menu);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGN_ICONS
    sign_gui_started();
#అయితే_ముగింపు

    // Configure the desired menu and scrollbars
    gui_init_which_components(ఖాళీ);

    // All components of the GUI have been created now
    gui.shell_created = true;

#నిర్వచితమైతే FEAT_GUI_MSWIN
    // Set the shell size, adjusted for the screen size.  For GTK this only
    // works after the shell has been opened, thus it is further down.
    // If the window is already maximized (e.g. when --windowid is passed in),
    // we want to use the system-provided dimensions by passing FALSE to
    // mustset. Otherwise, we want to initialize with the default rows/columns.
    ఒకవేళ (gui_mch_maximized())
	gui_సమితి_shellsize(FALSE, TRUE, RESIZE_BOTH);
    లేకపోతే
	gui_సమితి_shellsize(TRUE, TRUE, RESIZE_BOTH);
#లేకపోతే
# ifndef FEAT_GUI_GTK
    gui_సమితి_shellsize(FALSE, TRUE, RESIZE_BOTH);
# endif
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    // Need to set the size of the menubar after all the menus have been
    // created.
    gui_mch_compute_menu_ఎత్తు((Widget)0);
#అయితే_ముగింపు

    /*
     * Actually open the GUI shell.
     */
    ఒకవేళ (gui_mch_open() != FAIL)
    {
	maketitle();
	resettitle();

	init_gui_options();
#నిర్వచితమైతే FEAT_ARABIC
	// Our GUI can't do bidi.
	p_tbidi = FALSE;
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI_GTK)
	// Give GTK+ a chance to put all widget's into place.
	gui_mch_update();

# ifdef FEAT_MENU
	// If there is no 'm' in 'guioptions' we need to remove the menu now.
	// It was still there to make F10 work.
	ఒకవేళ (vim_strchr(p_go, GO_MENUS) == ఖాళీ)
	{
	    bool save_starting = gui.starting;

	    gui.starting = false;
	    gui_mch_enable_menu(FALSE);
	    gui.starting = save_starting;
	    gui_mch_update();
	}
# endif

	// Now make sure the shell fits on the screen.
	ఒకవేళ (gui_mch_maximized())
	    gui_సమితి_shellsize(FALSE, TRUE, RESIZE_BOTH);
	లేకపోతే
	    gui_సమితి_shellsize(TRUE, TRUE, RESIZE_BOTH);
#అయితే_ముగింపు
	// When 'lines' was set while starting up the topframe may have to be
	// resized.
	win_new_shellsize();

#నిర్వచితమైతే FEAT_BEVAL_GUI
	// Always create the Balloon Evaluation area, but disable it when
	// 'ballooneval' is off.
	ఒకవేళ (balloonEval != ఖాళీ)
	{
# ifdef FEAT_VARTABS
	    vim_free(balloonEval->vts);
# endif
	    vim_free(balloonEval);
	}
	balloon_Eval_For_పదం = FALSE;
# ifdef FEAT_GUI_GTK
	balloonEval = gui_mch_create_beval_area(gui.drawarea, ఖాళీ,
						     &general_beval_cb, ఖాళీ);
# లేకపోతే
#  ఒకవేళ defined(FEAT_GUI_MOTIF)
	{
	    బాహ్య Widget	పాఠ్యం_Area;
	    balloonEval = gui_mch_create_beval_area(పాఠ్యం_Area, ఖాళీ,
						     &general_beval_cb, ఖాళీ);
	}
#  లేకపోతే
#   ifdef FEAT_GUI_MSWIN
	balloonEval = gui_mch_create_beval_area(ఖాళీ, ఖాళీ,
						     &general_beval_cb, ఖాళీ);
#   endif
#  endif
# endif
	ఒకవేళ (!p_beval)
	    gui_mch_disable_beval_area(balloonEval);
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే FEAT_GUI_MSWIN
	// In the GUI modifiers are prepended to keys.
	// Don't do this for MS-Windows yet, it sends CTRL-K without the
	// modifier.
	చూసిన_Modify_Other_కీలకాలు = TRUE;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	ఒకవేళ (!im_xim_isvalid_imactivate())
	    emsg(_(e_విలువ_యొక్క_imactivatekey_is_invalid));
#అయితే_ముగింపు
	// When 'cmdheight' was set during startup it may not have taken
	// effect yet.
	ఒకవేళ (p_ch != 1L)
	    command_ఎత్తు();

	తిరిగి;
    }

error2:
#నిర్వచితమైతే FEAT_GUI_X11
    // undo gui_mch_init()
    gui_mch_uninit();
#అయితే_ముగింపు

error:
    gui.లోపల_use = false;
    clip_init(FALSE);
}


    శూన్య
gui_exit(పూర్ణాంకం rc)
{
    // don't free the fonts, it leads to a BUS error
    // richard@whitequeen.com Jul 99
    free_highlight_fonts();
    gui.లోపల_use = false;
    gui_mch_exit(rc);
}

#ఒకవేళ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_PHOTON)
# define NEED_GUI_UPDATE_SCREEN 1
/*
 * Called when the GUI shell is closed by the user.  If there are no changed
 * files Vim exits, otherwise there will be a dialog to ask the user what to
 * do.
 * When this function returns, Vim should NOT exit!
 */
    శూన్య
gui_shell_closed(శూన్య)
{
    cmdmod_T	    save_cmdmod = cmdmod;

    ఒకవేళ (before_quit_autocmds(curwin, TRUE, FALSE))
	తిరిగి;

    // Only exit when there are no changed files
    exiting = TRUE;
# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    cmdmod.cmod_flags |= CMOD_CONFIRM;
# endif
    // If there are changed buffers, present the user with a dialog if
    // possible, otherwise give an error message.
    ఒకవేళ (!check_మారింది_any(FALSE, FALSE))
	getout(0);

    exiting = FALSE;
    cmdmod = save_cmdmod;
    gui_update_screen();	// redraw, window may show changed buffer
}
#అయితే_ముగింపు

/*
 * Set the font.  "font_list" is a comma separated list of font names.  The
 * first font name that works is used.  If none is found, use the default
 * font.
 * If "fontset" is TRUE, the "font_list" is used as one name for the fontset.
 * Return OK when able to set the font.  When it failed FAIL is returned and
 * the fonts are unchanged.
 */
    పూర్ణాంకం
gui_init_font(char_u *font_list, పూర్ణాంకం fontset వాడని)
{
#నిర్వచించు FONTLEN 320
    char_u	font_name[FONTLEN];
    పూర్ణాంకం		font_list_ఖాళీలు = FALSE;
    పూర్ణాంకం		ret = FAIL;

    ఒకవేళ (!gui.లోపల_use)
	తిరిగి FAIL;

    font_name[0] = NUL;
    ఒకవేళ (*font_list == NUL)
	font_list_ఖాళీలు = TRUE;
    లేకపోతే
    {
#నిర్వచితమైతే FEAT_XFONTSET
	// When using a fontset, the whole list of fonts is one name.
	ఒకవేళ (fontset)
	    ret = gui_mch_init_font(font_list, TRUE);
	లేకపోతే
#అయితే_ముగింపు
	    అయితే (*font_list != NUL)
	    {
		// Isolate one comma separated font name.
		(శూన్య)copy_option_part(&font_list, font_name, FONTLEN, ",");

		// Careful!!!  The Win32 version of gui_mch_init_font(), when
		// called with "*" will change p_guifont to the selected font
		// name, which frees the old value.  This makes font_list
		// invalid.  Thus when OK is returned here, font_list must no
		// longer be used!
		ఒకవేళ (gui_mch_init_font(font_name, FALSE) == OK)
		{
#నిర్వచితమైతే USE_సమితి_GUIFONTWIDE
		    // If it's a Unicode font, try setting 'guifontwide' to a
		    // similar double-width font.
		    ఒకవేళ ((p_guifontwide == ఖాళీ || *p_guifontwide == NUL)
				&& అక్షరశ్రేణి_శోధన((అక్షరం *)font_name, "10646") != ఖాళీ)
			సమితి_guifontwide(font_name);
#అయితే_ముగింపు
		    ret = OK;
		    ఆపు;
		}
	    }
    }

    ఒకవేళ (ret != OK
	    && STRCMP(font_list, "*") != 0
	    && (font_list_ఖాళీలు || gui.norm_font == NOFONT))
    {
	/*
	 * Couldn't load any font in 'font_list', keep the current font if
	 * there is one.  If 'font_list' is empty, or if there is no current
	 * font, tell gui_mch_init_font() to try to find a font we can load.
	 */
	ret = gui_mch_init_font(ఖాళీ, FALSE);
    }

    ఒకవేళ (ret == OK)
    {
#నిర్వచితం_కాకపోతే FEAT_GUI_GTK
	// Set normal font as current font
# ifdef FEAT_XFONTSET
	ఒకవేళ (gui.fontset != NOFONTSET)
	    gui_mch_సమితి_fontset(gui.fontset);
	లేకపోతే
# endif
	    gui_mch_సమితి_font(gui.norm_font);
#అయితే_ముగింపు
	gui_సమితి_shellsize(FALSE, TRUE, RESIZE_BOTH);
    }

    తిరిగి ret;
}

#నిర్వచితమైతే USE_సమితి_GUIFONTWIDE
/*
 * Try setting 'guifontwide' to a font twice as wide as "name".
 */
    స్థిర శూన్య
సమితి_guifontwide(char_u *name)
{
    పూర్ణాంకం		i = 0;
    char_u	wide_name[FONTLEN + 10]; // room for 2 * width and '*'
    char_u	*wp = ఖాళీ;
    char_u	*p;
    GuiFont	font;

    wp = wide_name;
    కొరకు (p = name; *p != NUL; ++p)
    {
	*wp++ = *p;
	ఒకవేళ (*p == '-')
	{
	    ++i;
	    ఒకవేళ (i == 6)		// font type: change "--" to "-*-"
	    {
		ఒకవేళ (p[1] == '-')
		    *wp++ = '*';
	    }
	    లేకపోతే ఒకవేళ (i == 12)	// found the width
	    {
		++p;
		i = getdigits(&p);
		ఒకవేళ (i != 0)
		{
		    // Double the width specification.
		    sprintf((అక్షరం *)wp, "%d%s", i * 2, p);
		    font = gui_mch_get_font(wide_name, FALSE);
		    ఒకవేళ (font != NOFONT)
		    {
			gui_mch_free_font(gui.wide_font);
			gui.wide_font = font;
			సమితి_string_option_ప్రత్యక్ష((char_u *)"gfw", -1,
						      wide_name, OPT_FREE, 0);
		    }
		}
		ఆపు;
	    }
	}
    }
}
#అయితే_ముగింపు

/*
 * Get the font for 'guifontwide'.
 * Return FAIL for an invalid font name.
 */
    పూర్ణాంకం
gui_get_wide_font(శూన్య)
{
    GuiFont	font = NOFONT;
    char_u	font_name[FONTLEN];
    char_u	*p;

    ఒకవేళ (!gui.లోపల_use)	    // Can't allocate font yet, assume it's OK.
	తిరిగి OK;	    // Will give an error message later.

    ఒకవేళ (p_guifontwide != ఖాళీ && *p_guifontwide != NUL)
    {
	కొరకు (p = p_guifontwide; *p != NUL; )
	{
	    // Isolate one comma separated font name.
	    (శూన్య)copy_option_part(&p, font_name, FONTLEN, ",");
	    font = gui_mch_get_font(font_name, FALSE);
	    ఒకవేళ (font != NOFONT)
		ఆపు;
	}
	ఒకవేళ (font == NOFONT)
	    తిరిగి FAIL;
    }

    gui_mch_free_font(gui.wide_font);
#నిర్వచితమైతే FEAT_GUI_GTK
    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    ఒకవేళ (font != NOFONT && gui.norm_font != NOFONT
			 && pango_font_description_equal(font, gui.norm_font))
    {
	gui.wide_font = NOFONT;
	gui_mch_free_font(font);
    }
    లేకపోతే
#అయితే_ముగింపు
	gui.wide_font = font;
#నిర్వచితమైతే FEAT_GUI_MSWIN
    gui_mch_wide_font_మారింది();
#లేకపోతే
    /*
     * TODO: setup wide_bold_font, wide_ital_font and wide_boldital_font to
     * support those fonts for 'guifontwide'.
     */
#అయితే_ముగింపు
    తిరిగి OK;
}

#ఒకవేళ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * Set list of ascii characters that combined can create ligature.
 * Store them in char map for quick access from gui_gtk_draw_string.
 */
    శూన్య
gui_సమితి_ligatures(శూన్య)
{
    char_u	*p;

    ఒకవేళ (*p_guiligatures != NUL)
    {
	// check for invalid characters
	కొరకు (p = p_guiligatures; *p != NUL; ++p)
	    ఒకవేళ (*p < 32 || *p > 127)
	    {
		emsg(_(e_ascii_సంకేతం_not_లోపల_range));
		తిరిగి;
	    }

	// store valid setting into ligatures_map
	CLEAR_క్షేత్రం(gui.ligatures_map);
	కొరకు (p = p_guiligatures; *p != NUL; ++p)
	    gui.ligatures_map[*p] = 1;
    }
    లేకపోతే
	CLEAR_క్షేత్రం(gui.ligatures_map);
}

/*
 * Adjust the columns to undraw for when the cursor is on ligatures.
 */
    స్థిర శూన్య
gui_adjust_undraw_cursor_for_ligatures(పూర్ణాంకం *startcol, పూర్ణాంకం *endcol)
{
    పూర్ణాంకం off;

    ఒకవేళ (Screen_పంక్తులు == ఖాళీ || *p_guiligatures == NUL)
	తిరిగి;

    // expand before the cursor for all the chars in gui.ligatures_map
    off = పంక్తి_స్థానభ్రంశం[gui.cursor_అడ్డువరుస] + *startcol;
    ఒకవేళ (gui.ligatures_map[Screen_పంక్తులు[off]])
	అయితే (*startcol > 0 && gui.ligatures_map[Screen_పంక్తులు[--off]])
	    (*startcol)--;

    // expand after the cursor for all the chars in gui.ligatures_map
    off = పంక్తి_స్థానభ్రంశం[gui.cursor_అడ్డువరుస] + *endcol;
    ఒకవేళ (gui.ligatures_map[Screen_పంక్తులు[off]])
	అయితే (*endcol < ((పూర్ణాంకం)screen_Columns - 1)
				      && gui.ligatures_map[Screen_పంక్తులు[++off]])
	   (*endcol)++;
}
#అయితే_ముగింపు

    స్థిర శూన్య
gui_సమితి_cursor(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    gui.అడ్డువరుస = అడ్డువరుస;
    gui.నిలువు = నిలువు;
}

/*
 * gui_check_pos - check if the cursor is on the screen.
 */
    స్థిర శూన్య
gui_check_pos(శూన్య)
{
    ఒకవేళ (gui.అడ్డువరుస >= screen_అడ్డువరుసలు)
	gui.అడ్డువరుస = screen_అడ్డువరుసలు - 1;
    ఒకవేళ (gui.నిలువు >= screen_Columns)
	gui.నిలువు = screen_Columns - 1;
    ఒకవేళ (gui.cursor_అడ్డువరుస >= screen_అడ్డువరుసలు || gui.cursor_నిలువు >= screen_Columns)
	gui.cursor_is_valid = false;
}

/*
 * Redraw the cursor if necessary or when forced.
 * Careful: The contents of ScreenLines[] must match what is on the screen,
 * otherwise this goes wrong.  May need to call out_flush() first.
 */
    శూన్య
gui_update_cursor(
    పూర్ణాంకం		force,		 // when TRUE, update even when not moved
    పూర్ణాంకం		clear_selection) // clear selection under cursor
{
    పూర్ణాంకం		cur_వెడల్పు = 0;
    పూర్ణాంకం		cur_ఎత్తు = 0;
    పూర్ణాంకం		old_hl_ముసుగు;
    cursorentry_T *shape;
    పూర్ణాంకం		id;
#నిర్వచితమైతే FEAT_TERMINAL
    guicolor_T	shape_fg = INVALCOLOR;
    guicolor_T	shape_bg = INVALCOLOR;
#అయితే_ముగింపు
    guicolor_T	cfg, cbg, cc;	// cursor fore-/background color
    పూర్ణాంకం		cattr;		// cursor attributes
    పూర్ణాంకం		attr;
    attrentry_T *aep = ఖాళీ;
#ఒకవేళ (defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)) && !defined(USE_GTK4)
    bool	lig_ఎడమ = false, lig_కుడి = false;
#అయితే_ముగింపు

    // Don't update the cursor when halfway busy scrolling or the screen size
    // doesn't match 'columns' and 'lines.  ScreenLines[] isn't valid then.
    ఒకవేళ (!can_update_cursor || screen_Columns != gui.అంకె_నిలువుల_సంఖ్య
					       || screen_అడ్డువరుసలు != gui.అంకె_అడ్డువరుసలు)
	తిరిగి;

    gui_check_pos();

    ఒకవేళ (gui.cursor_is_valid && !force
		&& gui.అడ్డువరుస == gui.cursor_అడ్డువరుస && gui.నిలువు == gui.cursor_నిలువు)
	తిరిగి;

    gui_undraw_cursor();

    // If a cursor-less sleep is ongoing, leave the cursor invisible
    ఒకవేళ (cursor_is_sleeping())
	తిరిగి;

    ఒకవేళ (gui.అడ్డువరుస < 0)
	తిరిగి;
#నిర్వచితమైతే HAVE_ఆదానం_పద్ధతి
    ఒకవేళ (gui.అడ్డువరుస != gui.cursor_అడ్డువరుస || gui.నిలువు != gui.cursor_నిలువు)
	im_సమితి_position(gui.అడ్డువరుస, gui.నిలువు);
#అయితే_ముగింపు
    gui.cursor_అడ్డువరుస = gui.అడ్డువరుస;
    gui.cursor_నిలువు = gui.నిలువు;

    // Only write to the screen after ScreenLines[] has been initialized
    ఒకవేళ (!screen_cleared || Screen_పంక్తులు == ఖాళీ)
	తిరిగి;

    // Clear the selection if we are about to write over it
    ఒకవేళ (clear_selection)
	clip_may_clear_selection(gui.అడ్డువరుస, gui.అడ్డువరుస);
    // Check that the cursor is inside the shell (resizing may have made
    // it invalid)
    ఒకవేళ (gui.అడ్డువరుస >= screen_అడ్డువరుసలు || gui.నిలువు >= screen_Columns)
	తిరిగి;

    gui.cursor_is_valid = true;

    /*
     * How the cursor is drawn depends on the current mode.
     * When in a terminal window use the shape/color specified there.
     */
#నిర్వచితమైతే FEAT_TERMINAL
    ఒకవేళ (terminal_is_చురుకైన())
	shape = పదం_get_cursor_shape(&shape_fg, &shape_bg);
    లేకపోతే
#అయితే_ముగింపు
	shape = &shape_పట్టిక[get_shape_idx(FALSE)];
    ఒకవేళ (State & బహుళకం_LANGMAP)
	id = shape->id_lm;
    లేకపోతే
	id = shape->id;

    // get the colors and attributes for the cursor.  Default is inverted
    cfg = INVALCOLOR;
    cbg = INVALCOLOR;
    cattr = HL_INVERSE;
    gui_mch_సమితి_blinking(shape->blinkwait,
	    shape->blinkon,
	    shape->blinkoff);
    ఒకవేళ (shape->blinkwait == 0 || shape->blinkon == 0
	    || shape->blinkoff == 0)
	gui_mch_stop_blink(FALSE);
#నిర్వచితమైతే FEAT_TERMINAL
    ఒకవేళ (shape_bg != INVALCOLOR)
    {
	cattr = 0;
	cfg = shape_fg;
	cbg = shape_bg;
    }
    లేకపోతే
#అయితే_ముగింపు
	ఒకవేళ (id > 0)
	{
	    cattr = syn_id2colors(id, &cfg, &cbg);
#ఒకవేళ defined(HAVE_ఆదానం_పద్ధతి)
	    {
		స్థిర పూర్ణాంకం iid;
		guicolor_T fg, bg;

		ఒకవేళ (
# ఒకవేళ defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
			preedit_get_స్థితి()
# లేకపోతే
			im_get_స్థితి()
# endif
		   )
		{
		    iid = syn_name2id((char_u *)"CursorIM");
		    ఒకవేళ (iid > 0)
		    {
			syn_id2colors(iid, &fg, &bg);
			ఒకవేళ (bg != INVALCOLOR)
			    cbg = bg;
			ఒకవేళ (fg != INVALCOLOR)
			    cfg = fg;
		    }
		}
	    }
#అయితే_ముగింపు
	}

    /*
     * Get the attributes for the character under the cursor.
     * When no cursor color was given, use the character color.
     */
    attr = ScreenAttrs[పంక్తి_స్థానభ్రంశం[gui.అడ్డువరుస] + gui.నిలువు];
    ఒకవేళ (attr > HL_ALL)
	aep = syn_gui_attr2entry(attr);
    ఒకవేళ (aep != ఖాళీ)
    {
	attr = aep->ae_attr;
	ఒకవేళ (cfg == INVALCOLOR)
	    cfg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.bg_color
		    : aep->ae_u.gui.fg_color);
	ఒకవేళ (cbg == INVALCOLOR)
	    cbg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.fg_color
		    : aep->ae_u.gui.bg_color);
    }
    ఒకవేళ (cfg == INVALCOLOR)
	cfg = (attr & HL_INVERSE) ? gui.వెనుక_pixel : gui.norm_pixel;
    ఒకవేళ (cbg == INVALCOLOR)
	cbg = (attr & HL_INVERSE) ? gui.norm_pixel : gui.వెనుక_pixel;

#నిర్వచితమైతే FEAT_XIM
    ఒకవేళ (aep != ఖాళీ)
    {
	xim_bg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.fg_color
		: aep->ae_u.gui.bg_color);
	xim_fg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.bg_color
		: aep->ae_u.gui.fg_color);
	ఒకవేళ (xim_bg_color == INVALCOLOR)
	    xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
		: gui.వెనుక_pixel;
	ఒకవేళ (xim_fg_color == INVALCOLOR)
	    xim_fg_color = (attr & HL_INVERSE) ? gui.వెనుక_pixel
		: gui.norm_pixel;
    }
    లేకపోతే
    {
	xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
	    : gui.వెనుక_pixel;
	xim_fg_color = (attr & HL_INVERSE) ? gui.వెనుక_pixel
	    : gui.norm_pixel;
    }
#అయితే_ముగింపు

    attr &= ~HL_INVERSE;
    ఒకవేళ (cattr & HL_INVERSE)
    {
	cc = cbg;
	cbg = cfg;
	cfg = cc;
    }
    cattr &= ~HL_INVERSE;

    /*
     * When we don't have window focus, draw a hollow cursor.
     */
    ఒకవేళ (!gui.లోపల_focus)
    {
#నిర్వచితమైతే USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, -1, -1);
#లేకపోతే
	gui_mch_draw_hollow_cursor(cbg);
#అయితే_ముగింపు
	తిరిగి;
    }

#నిర్వచితమైతే USE_GTK4
    // Make sure that character underneath is drawn again in case it is part of
    // a ligature.
    gui_redraw_ఖండం(gui.అడ్డువరుస, gui.నిలువు, gui.అడ్డువరుస, gui.నిలువు, GUI_MON_NOCLEAR);
    // gui_redraw_block() will invalidate the cursor
    gui.cursor_is_valid = true;
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // If cursor is in the middle of a ligature, then split the ligature at
    // the cursor boundaries by redrawing those cells again.
    కొరకు (పూర్ణాంకం c = gui.నిలువు - 1; c >= 0; c--)
	ఒకవేళ (!gui.ligatures_map[Screen_పంక్తులు[పంక్తి_స్థానభ్రంశం[gui.అడ్డువరుస] + c]])
	{
	    ఒకవేళ (c < gui.నిలువు - 1)
	    {
		gui_redraw_ఖండం(gui.అడ్డువరుస, c + 1, gui.అడ్డువరుస, gui.నిలువు - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_ఎడమ = true;
# endif
	    }
	    ఆపు;
	}

    కొరకు (పూర్ణాంకం c = gui.నిలువు + 1; c < screen_Columns - 1; c++)
	ఒకవేళ (!gui.ligatures_map[Screen_పంక్తులు[పంక్తి_స్థానభ్రంశం[gui.అడ్డువరుస] + c]])
	{
	    ఒకవేళ (c > gui.నిలువు + 1)
	    {
		gui_redraw_ఖండం(gui.అడ్డువరుస, gui.నిలువు + 1, gui.అడ్డువరుస, c - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_కుడి = true;
# endif
	    }
	    ఆపు;
	}

# ifndef USE_GTK4
    ఒకవేళ ((lig_ఎడమ || lig_కుడి) && shape->shape != SHAPE_ఖండం)
    {
	// Because the cursor is not drawn with gui_screenchar(), must blit the
	// cell again (with its background), so that old ligature does not
	// remain on screen. Not needed for GtkSnapshot, because char beneath is
	// always rerendered (see above ifdef).
	పూర్ణాంకం old = gui.నిలువు;
	gui.highlight_ముసుగు = ScreenAttrs[పంక్తి_స్థానభ్రంశం[gui.అడ్డువరుస] + gui.నిలువు];
	(శూన్య)gui_screenchar(పంక్తి_స్థానభ్రంశం[gui.అడ్డువరుస] + gui.నిలువు,
		GUI_MON_NOCLEAR, (guicolor_T)0, (guicolor_T)0, 0);
	gui.నిలువు = old;
    }
# endif
    // gui_redraw_block()/gui_screenchar() may invalidate the cursor, make sure
    // to validate it again.
    gui.cursor_is_valid = true;
#అయితే_ముగింపు

    old_hl_ముసుగు = gui.highlight_ముసుగు;
    ఒకవేళ (shape->shape == SHAPE_ఖండం)
    {
#నిర్వచితమైతే USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, 0, 0);
#లేకపోతే
	/*
	 * Draw the text character with the cursor colors.	Use the
	 * character attributes plus the cursor attributes.
	 */
	gui.highlight_ముసుగు = (cattr | attr);
	(శూన్య)gui_screenchar(పంక్తి_స్థానభ్రంశం[gui.అడ్డువరుస] + gui.నిలువు,
		GUI_MON_IS_CURSOR | GUI_MON_NOCLEAR, cfg, cbg, 0);
#అయితే_ముగింపు
    }
    లేకపోతే
    {
#ఒకవేళ defined(FEAT_RIGHTLEFT)
	పూర్ణాంకం	    నిలువు_off = FALSE;
#అయితే_ముగింపు
	/*
	 * First draw the partial cursor, then overwrite with the text
	 * character, using a transparent background.
	 */
	ఒకవేళ (shape->shape == SHAPE_VER)
	{
	    cur_ఎత్తు = gui.char_ఎత్తు;
	    cur_వెడల్పు = (gui.char_వెడల్పు * shape->percentage + 99) / 100;
	}
	లేకపోతే
	{
	    cur_ఎత్తు = (gui.char_ఎత్తు * shape->percentage + 99) / 100;
	    cur_వెడల్పు = gui.char_వెడల్పు;
	}
	ఒకవేళ (has_mbyte && (*mb_off2cells)(పంక్తి_స్థానభ్రంశం[gui.అడ్డువరుస] + gui.నిలువు,
		    పంక్తి_స్థానభ్రంశం[gui.అడ్డువరుస] + screen_Columns) > 1)
	{
	    // Double wide character.
	    ఒకవేళ (shape->shape != SHAPE_VER)
		cur_వెడల్పు += gui.char_వెడల్పు;
#నిర్వచితమైతే FEAT_RIGHTLEFT
	    ఒకవేళ (CURSOR_BAR_కుడి)
	    {
		// gui.col points to the left half of the character but
		// the vertical line needs to be on the right half.
		// A double-wide horizontal line is also drawn from the
		// right half in gui_mch_draw_part_cursor().
		నిలువు_off = TRUE;
		++gui.నిలువు;
	    }
#అయితే_ముగింపు
	}
#నిర్వచితమైతే USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, cur_వెడల్పు, cur_ఎత్తు);
#లేకపోతే
	gui_mch_draw_part_cursor(cur_వెడల్పు, cur_ఎత్తు, cbg);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_RIGHTLEFT)
	ఒకవేళ (నిలువు_off)
	    --gui.నిలువు;
#అయితే_ముగింపు

	// Doesn't seem to work for MSWindows. We call gui_redraw_block() above
	// for GtkSnapshot.
#ఒకవేళ !defined(FEAT_GUI_MSWIN) && !defined(USE_GTK4)
	gui.highlight_ముసుగు = ScreenAttrs[పంక్తి_స్థానభ్రంశం[gui.అడ్డువరుస] + gui.నిలువు];
	(శూన్య)gui_screenchar(పంక్తి_స్థానభ్రంశం[gui.అడ్డువరుస] + gui.నిలువు,
		GUI_MON_TRS_CURSOR | GUI_MON_NOCLEAR,
		(guicolor_T)0, (guicolor_T)0, 0);
#అయితే_ముగింపు
    }
    gui.highlight_ముసుగు = old_hl_ముసుగు;
}

#ఒకవేళ defined(FEAT_MENU)
    స్థిర శూన్య
gui_position_menu(శూన్య)
{
# ఒకవేళ !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
    ఒకవేళ (gui.menu_is_చురుకైన && gui.లోపల_use)
	gui_mch_సమితి_menu_pos(0, 0, gui.menu_వెడల్పు, gui.menu_ఎత్తు);
# endif
}
#అయితే_ముగింపు

/*
 * Position the various GUI components (text area, menu).  The vertical
 * scrollbars are NOT handled here.  See gui_update_scrollbars().
 */
    స్థిర శూన్య
gui_position_components(పూర్ణాంకం మొత్తం_వెడల్పు వాడని)
{
    పూర్ణాంకం	    పాఠ్యం_area_x;
    పూర్ణాంకం	    పాఠ్యం_area_y;
    పూర్ణాంకం	    పాఠ్యం_area_వెడల్పు;
    పూర్ణాంకం	    పాఠ్యం_area_ఎత్తు;

    // avoid that moving components around generates events
    ++hold_gui_events;

    పాఠ్యం_area_x = 0;
    ఒకవేళ (gui.which_scrollbars[SBAR_ఎడమ])
	పాఠ్యం_area_x += gui.scrollbar_వెడల్పు;

    పాఠ్యం_area_y = 0;
#ఒకవేళ defined(FEAT_MENU) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    gui.menu_వెడల్పు = మొత్తం_వెడల్పు;
    ఒకవేళ (gui.menu_is_చురుకైన)
	పాఠ్యం_area_y += gui.menu_ఎత్తు;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
    ఒకవేళ (gui_has_tabline())
	పాఠ్యం_area_y += gui.tabline_ఎత్తు;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    ఒకవేళ (vim_strchr(p_go, GO_TOOLBAR) != ఖాళీ)
    {
# ఒకవేళ defined(FEAT_GUI_HAIKU)
	gui_mch_సమితి_toolbar_pos(0, పాఠ్యం_area_y,
				gui.menu_వెడల్పు, gui.toolbar_ఎత్తు);
# endif
	పాఠ్యం_area_y += gui.toolbar_ఎత్తు;
    }
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_HAIKU)
    gui_mch_సమితి_tabline_pos(0, పాఠ్యం_area_y,
    gui.menu_వెడల్పు, gui.tabline_ఎత్తు);
    ఒకవేళ (gui_has_tabline())
	పాఠ్యం_area_y += gui.tabline_ఎత్తు;
#అయితే_ముగింపు

    పాఠ్యం_area_వెడల్పు = gui.అంకె_నిలువుల_సంఖ్య * gui.char_వెడల్పు + gui.border_స్థానభ్రంశం * 2;
    పాఠ్యం_area_ఎత్తు = gui.అంకె_అడ్డువరుసలు * gui.char_ఎత్తు + gui.border_స్థానభ్రంశం * 2;

    gui_mch_సమితి_పాఠ్యం_area_pos(పాఠ్యం_area_x,
			      పాఠ్యం_area_y,
			      పాఠ్యం_area_వెడల్పు,
			      పాఠ్యం_area_ఎత్తు
#ఒకవేళ defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
				  + xim_get_స్థితి_area_ఎత్తు()
#అయితే_ముగింపు
			      );
#నిర్వచితమైతే FEAT_MENU
    gui_position_menu();
#అయితే_ముగింపు
    ఒకవేళ (gui.which_scrollbars[SBAR_BOTTOM])
	gui_mch_సమితి_scrollbar_pos(&gui.bottom_sbar,
				  పాఠ్యం_area_x,
				  పాఠ్యం_area_y + పాఠ్యం_area_ఎత్తు
					+ gui_mch_get_scrollbar_ypadding(),
				  పాఠ్యం_area_వెడల్పు,
				  gui.scrollbar_ఎత్తు);
    gui.ఎడమ_sbar_x = 0;
    gui.కుడి_sbar_x = పాఠ్యం_area_x + పాఠ్యం_area_వెడల్పు
					+ gui_mch_get_scrollbar_xpadding();

    --hold_gui_events;
}

/*
 * Get the width of the widgets and decorations to the side of the text area.
 */
    పూర్ణాంకం
gui_get_పునాది_వెడల్పు(శూన్య)
{
    పూర్ణాంకం	    పునాది_వెడల్పు;

    పునాది_వెడల్పు = 2 * gui.border_స్థానభ్రంశం;
    ఒకవేళ (gui.which_scrollbars[SBAR_ఎడమ])
	పునాది_వెడల్పు += gui.scrollbar_వెడల్పు;
    ఒకవేళ (gui.which_scrollbars[SBAR_కుడి])
	పునాది_వెడల్పు += gui.scrollbar_వెడల్పు;
    తిరిగి పునాది_వెడల్పు;
}

/*
 * Get the height of the widgets and decorations above and below the text area.
 */
    పూర్ణాంకం
gui_get_పునాది_ఎత్తు(శూన్య)
{
    పూర్ణాంకం	    పునాది_ఎత్తు;

    పునాది_ఎత్తు = 2 * gui.border_స్థానభ్రంశం;
    ఒకవేళ (gui.which_scrollbars[SBAR_BOTTOM])
	పునాది_ఎత్తు += gui.scrollbar_ఎత్తు;
#నిర్వచితమైతే FEAT_GUI_GTK
    // We can't take the sizes properly into account until anything is
    // realized.  Therefore we recalculate all the values here just before
    // setting the size. (--mdcki)
#లేకపోతే
# ifdef FEAT_MENU
    ఒకవేళ (gui.menu_is_చురుకైన)
	పునాది_ఎత్తు += gui.menu_ఎత్తు;
# endif
# ifdef FEAT_TOOLBAR
    ఒకవేళ (vim_strchr(p_go, GO_TOOLBAR) != ఖాళీ)
	పునాది_ఎత్తు += gui.toolbar_ఎత్తు;
# endif
# ఒకవేళ defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    ఒకవేళ (gui_has_tabline())
	పునాది_ఎత్తు += gui.tabline_ఎత్తు;
# endif
# ఒకవేళ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    పునాది_ఎత్తు += gui_mch_పాఠ్యం_area_extra_ఎత్తు();
# endif
#అయితే_ముగింపు
    తిరిగి పునాది_ఎత్తు;
}

/*
 * Should be called after the GUI shell has been resized.  Its arguments are
 * the new width and height of the shell in pixels.
 */
    శూన్య
gui_resize_shell(పూర్ణాంకం pixel_వెడల్పు, పూర్ణాంకం pixel_ఎత్తు)
{
    స్థిర పూర్ణాంకం	busy = FALSE;

    ఒకవేళ (!gui.shell_created)	    // ignore when still initializing
	తిరిగి;

    /*
     * Can't resize the screen while it is being redrawn.  Remember the new
     * size and handle it later.
     */
    ఒకవేళ (updating_screen || busy)
    {
	new_pixel_వెడల్పు = pixel_వెడల్పు;
	new_pixel_ఎత్తు = pixel_ఎత్తు;
	తిరిగి;
    }

again:
    new_pixel_వెడల్పు = 0;
    new_pixel_ఎత్తు = 0;
    busy = TRUE;

#నిర్వచితమైతే FEAT_GUI_HAIKU
    vim_తాళం_screen();
#అయితే_ముగింపు

    // Flush pending output before redrawing
    బహిర్గతం_flush();
#ఒకవేళ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk_init_decor_ఎత్తు();
#అయితే_ముగింపు

    gui.అంకె_నిలువుల_సంఖ్య = (pixel_వెడల్పు - gui_get_పునాది_వెడల్పు()) / gui.char_వెడల్పు;
    gui.అంకె_అడ్డువరుసలు = (pixel_ఎత్తు - gui_get_పునాది_ఎత్తు()) / gui.char_ఎత్తు;

#నిర్వచితమైతే USE_GTK4
    gui_gtk4_update_కొలత();
#అయితే_ముగింపు

    gui_position_components(pixel_వెడల్పు);
    gui_reset_scroll_region();

#ఒకవేళ defined(FEAT_GUI_GTK) && defined(USE_GTK4) && !defined(USE_GTK4)
    // We do not resize the draw area via the "resize" signal. This is because
    // when the window is resized, the form widget is the one that is resized,
    // so let that call gui_resize_shell() which will allocate the surface and
    // allocate the drawing area size/position.
    gui_gtk4_resize(pixel_వెడల్పు, pixel_ఎత్తు);
#అయితే_ముగింపు

    /*
     * At the "more" and ":confirm" prompt there is no redraw, put the cursor
     * at the last line here (why does it have to be one row too low?).
     */
    ఒకవేళ (State == బహుళకం_ASKMORE || State == బహుళకం_CONFIRM)
	gui.అడ్డువరుస = gui.అంకె_అడ్డువరుసలు;

    // Only comparing Rows and Columns may be sufficient, but let's stay on
    // the safe side.
    ఒకవేళ (gui.అంకె_అడ్డువరుసలు != screen_అడ్డువరుసలు || gui.అంకె_నిలువుల_సంఖ్య != screen_Columns
	    || gui.అంకె_అడ్డువరుసలు != అడ్డువరుసలు || gui.అంకె_నిలువుల_సంఖ్య != Columns || gui.force_redraw)
    {
	shell_resized();
	gui.force_redraw = 0;
    }

#నిర్వచితమైతే FEAT_GUI_HAIKU
    vim_unlock_screen();
#అయితే_ముగింపు

    gui_update_scrollbars(TRUE);
    gui_update_cursor(FALSE, TRUE);
#ఒకవేళ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(pixel_వెడల్పు, pixel_ఎత్తు);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
    xim_సమితి_స్థితి_area();
#అయితే_ముగింపు

    busy = FALSE;

    // We may have been called again while redrawing the screen.
    // Need to do it all again with the latest size then.  But only if the size
    // actually changed.
    ఒకవేళ (new_pixel_ఎత్తు)
    {
	ఒకవేళ (pixel_వెడల్పు == new_pixel_వెడల్పు && pixel_ఎత్తు == new_pixel_ఎత్తు)
	{
	    new_pixel_వెడల్పు = 0;
	    new_pixel_ఎత్తు = 0;
	}
	లేకపోతే
	{
	    pixel_వెడల్పు = new_pixel_వెడల్పు;
	    pixel_ఎత్తు = new_pixel_ఎత్తు;
	    వెళ్ళు again;
	}
    }
}

/*
 * Check if gui_resize_shell() must be called.
 */
    శూన్య
gui_may_resize_shell(శూన్య)
{
    ఒకవేళ (new_pixel_ఎత్తు)
    {
	// careful: gui_resize_shell() may postpone the resize again if we
	// were called indirectly by it
	gui_resize_shell(new_pixel_వెడల్పు, new_pixel_ఎత్తు);
    }
}

    పూర్ణాంకం
gui_get_shellsize(శూన్య)
{
    అడ్డువరుసలు = gui.అంకె_అడ్డువరుసలు;
    Columns = gui.అంకె_నిలువుల_సంఖ్య;
    తిరిగి OK;
}

/*
 * Set the size of the Vim shell according to Rows and Columns.
 * If "fit_to_display" is TRUE then the size may be reduced to fit the window
 * on the screen.
 * When "mustset" is TRUE the size was set by the user. When FALSE a UI
 * component was added or removed (e.g., a scrollbar).
 */
    శూన్య
gui_సమితి_shellsize(
    పూర్ణాంకం		mustset వాడని,
    పూర్ణాంకం		fit_గమ్యం_display,
    పూర్ణాంకం		direction)		// RESIZE_HOR, RESIZE_VER
{
    పూర్ణాంకం		పునాది_వెడల్పు;
    పూర్ణాంకం		పునాది_ఎత్తు;
    పూర్ణాంకం		వెడల్పు;
    పూర్ణాంకం		ఎత్తు;
    పూర్ణాంకం		min_వెడల్పు;
    పూర్ణాంకం		min_ఎత్తు;
    పూర్ణాంకం		screen_w;
    పూర్ణాంకం		screen_h;
#నిర్వచితమైతే FEAT_GUI_GTK
    పూర్ణాంకం		un_maximize = mustset;
    పూర్ణాంకం		did_adjust = 0;
#అయితే_ముగింపు
    పూర్ణాంకం		x = -1, y = -1;

    ఒకవేళ (!gui.shell_created)
	తిరిగి;

#ఒకవేళ defined(MSWIN) || defined(FEAT_GUI_GTK)
    // If not setting to a user specified size and maximized, calculate the
    // number of characters that fit in the maximized window.
    // FIXME: gui_mch_newfont() is called here even when the font hasn't
    // changed at all.  For example, ":set guioptions=k" triggers this path
    // via gui_init_which_components() -> gui_set_shellsize(FALSE, ...).
    // The intent is to keep the window size and recalculate Rows/Columns,
    // which has nothing to do with fonts.  This should be a separate
    // function with a more descriptive name.
    ఒకవేళ (!mustset && (vim_strchr(p_go, GO_KEEPWINSIZE) != ఖాళీ
						       || gui_mch_maximized()))
    {
	gui_mch_newfont();
	తిరిగి;
    }
#అయితే_ముగింపు

    పునాది_వెడల్పు = gui_get_పునాది_వెడల్పు();
    పునాది_ఎత్తు = gui_get_పునాది_ఎత్తు();
    ఒకవేళ (fit_గమ్యం_display)
	// Remember the original window position.
	(శూన్య)gui_mch_get_winpos(&x, &y);

    వెడల్పు = Columns * gui.char_వెడల్పు + పునాది_వెడల్పు;
    ఎత్తు = అడ్డువరుసలు * gui.char_ఎత్తు + పునాది_ఎత్తు;

    ఒకవేళ (fit_గమ్యం_display)
    {
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	ఒకవేళ ((direction & RESIZE_HOR) && వెడల్పు > screen_w)
	{
	    Columns = (screen_w - పునాది_వెడల్పు) / gui.char_వెడల్పు;
	    ఒకవేళ (Columns < MIN_COLUMNS)
		Columns = MIN_COLUMNS;
	    వెడల్పు = Columns * gui.char_వెడల్పు + పునాది_వెడల్పు;
#నిర్వచితమైతే FEAT_GUI_GTK
	    ++did_adjust;
#అయితే_ముగింపు
	}
	ఒకవేళ ((direction & RESIZE_VERT) && ఎత్తు > screen_h)
	{
	    అడ్డువరుసలు = (screen_h - పునాది_ఎత్తు) / gui.char_ఎత్తు;
	    check_shellsize();
	    ఎత్తు = అడ్డువరుసలు * gui.char_ఎత్తు + పునాది_ఎత్తు;
#నిర్వచితమైతే FEAT_GUI_GTK
	    ++did_adjust;
#అయితే_ముగింపు
	}
#నిర్వచితమైతే FEAT_GUI_GTK
	ఒకవేళ (did_adjust == 2 || (వెడల్పు + gui.char_వెడల్పు >= screen_w
				     && ఎత్తు + gui.char_ఎత్తు >= screen_h))
	    // don't unmaximize if at maximum size
	    un_maximize = FALSE;
#అయితే_ముగింపు
    }
    హద్దు_screen_కొలత();
    gui.అంకె_నిలువుల_సంఖ్య = Columns;
    gui.అంకె_అడ్డువరుసలు = అడ్డువరుసలు;
#నిర్వచితమైతే USE_GTK4
    // Keep the drawing area in sync with the size Vim is going to draw.
    gui_gtk4_update_కొలత();
#అయితే_ముగింపు

    min_వెడల్పు = పునాది_వెడల్పు + MIN_COLUMNS * gui.char_వెడల్పు;
    min_ఎత్తు = పునాది_ఎత్తు + MIN_పంక్తులు * gui.char_ఎత్తు;
    min_ఎత్తు += tabline_ఎత్తు() * gui.char_ఎత్తు;

#నిర్వచితమైతే FEAT_GUI_GTK
    ఒకవేళ (un_maximize)
    {
	// If the window size is smaller than the screen unmaximize the
	// window, otherwise resizing won't work.
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	ఒకవేళ ((వెడల్పు + gui.char_వెడల్పు < screen_w
				   || ఎత్తు + gui.char_ఎత్తు * 2 < screen_h)
		&& gui_mch_maximized())
	    gui_mch_unmaximize();
    }
#అయితే_ముగింపు

    gui_mch_సమితి_shellsize(వెడల్పు, ఎత్తు, min_వెడల్పు, min_ఎత్తు,
					  పునాది_వెడల్పు, పునాది_ఎత్తు, direction);

    ఒకవేళ (fit_గమ్యం_display && x >= 0 && y >= 0)
    {
	// Some window managers put the Vim window left of/above the screen.
	// Only change the position if it wasn't already negative before
	// (happens on MS-Windows with a secondary monitor).
	gui_mch_update();
	ఒకవేళ (gui_mch_get_winpos(&x, &y) == OK && (x < 0 || y < 0))
	    gui_mch_సమితి_winpos(x < 0 ? 0 : x, y < 0 ? 0 : y);
    }

    gui_position_components(వెడల్పు);
    gui_update_scrollbars(TRUE);
    gui_reset_scroll_region();

#ఒకవేళ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(వెడల్పు, ఎత్తు);
#అయితే_ముగింపు
}

/*
 * Called when Rows and/or Columns has changed.
 */
    శూన్య
gui_new_shellsize(శూన్య)
{
    gui_reset_scroll_region();
}

/*
 * Make scroll region cover whole screen.
 */
    స్థిర శూన్య
gui_reset_scroll_region(శూన్య)
{
    gui.scroll_region_శిఖరం = 0;
    gui.scroll_region_bot = gui.అంకె_అడ్డువరుసలు - 1;
    gui.scroll_region_ఎడమ = 0;
    gui.scroll_region_కుడి = gui.అంకె_నిలువుల_సంఖ్య - 1;
}

    స్థిర శూన్య
gui_ప్రారంభం_highlight(పూర్ణాంకం ముసుగు)
{
    ఒకవేళ (ముసుగు > HL_ALL)		    // highlight code
	gui.highlight_ముసుగు = ముసుగు;
    లేకపోతే			    //ముసుగు
	gui.highlight_ముసుగు |= ముసుగు;
}

    శూన్య
gui_stop_highlight(పూర్ణాంకం ముసుగు)
{
    ఒకవేళ (ముసుగు > HL_ALL)		    // highlight code
	gui.highlight_ముసుగు = HL_సాధారణ;
    లేకపోతే			    //ముసుగు
	gui.highlight_ముసుగు &= ~ముసుగు;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    శూన్య
gui_clear_ఖండం(
    పూర్ణాంకం	    row1,
    పూర్ణాంకం	    col1,
    పూర్ణాంకం	    row2,
    పూర్ణాంకం	    col2)
{
    // Clear the selection if we are about to write over it
    clip_may_clear_selection(row1, row2);

    gui_mch_clear_ఖండం(row1, col1, row2, col2);

    // Invalidate cursor if it was in this block
    ఒకవేళ (       gui.cursor_అడ్డువరుస >= row1 && gui.cursor_అడ్డువరుస <= row2
	    && gui.cursor_నిలువు >= col1 && gui.cursor_నిలువు <= col2)
	gui.cursor_is_valid = false;
}

/*
 * Write code to update the cursor later.  This avoids the need to flush the
 * output buffer before calling gui_update_cursor().
 */
    శూన్య
gui_update_cursor_later(శూన్య)
{
    బహిర్గతం_STR("\033|s");
}

    శూన్య
gui_write(
    char_u	*s,
    పూర్ణాంకం		len)
{
    char_u	*p;
    పూర్ణాంకం		arg1 = 0, arg2 = 0;
    పూర్ణాంకం		force_cursor = FALSE;	// force cursor update
    పూర్ణాంకం		force_scrollbar = FALSE;
    స్థిర win_T	*old_curwin = ఖాళీ;

// #define DEBUG_GUI_WRITE
#నిర్వచితమైతే DEBUG_GUI_WRITE
    {
	పూర్ణాంకం i;
	char_u *str;

	రాయి("gui_write(%d):\n    ", len);
	కొరకు (i = 0; i < len; i++)
	    ఒకవేళ (s[i] == ESC)
	    {
		ఒకవేళ (i != 0)
		    రాయి("\n    ");
		రాయి("<ESC>");
	    }
	    లేకపోతే
	    {
		str = transchar_byte(s[i]);
		ఒకవేళ (str[0] && str[1])
		    రాయి("<%s>", (అక్షరం *)str);
		లేకపోతే
		    రాయి("%s", (అక్షరం *)str);
	    }
	రాయి("\n");
    }
#అయితే_ముగింపు
    అయితే (len)
    {
	ఒకవేళ (s[0] == ESC && s[1] == '|')
	{
	    p = s + 2;
	    ఒకవేళ (VIM_ISDIGIT(*p) || (*p == '-' && VIM_ISDIGIT(*(p + 1))))
	    {
		arg1 = getdigits(&p);
		ఒకవేళ (p > s + len)
		    ఆపు;
		ఒకవేళ (*p == ';')
		{
		    ++p;
		    arg2 = getdigits(&p);
		    ఒకవేళ (p > s + len)
			ఆపు;
		}
	    }
	    ఎంపిక (*p)
	    {
		స్థితి 'C':	// Clear screen
		    clip_scroll_selection(9999);
		    gui_mch_clear_all();
		    gui.cursor_is_valid = false;
		    force_scrollbar = TRUE;
		    ఆపు;
		స్థితి 'M':	// Move cursor
		    gui_సమితి_cursor(arg1, arg2);
		    ఆపు;
		స్థితి 's':	// force cursor (shape) update
		    force_cursor = TRUE;
		    ఆపు;
		స్థితి 'R':	// Set scroll region
		    ఒకవేళ (arg1 < arg2)
		    {
			gui.scroll_region_శిఖరం = arg1;
			gui.scroll_region_bot = arg2;
		    }
		    లేకపోతే
		    {
			gui.scroll_region_శిఖరం = arg2;
			gui.scroll_region_bot = arg1;
		    }
		    ఆపు;
		స్థితి 'V':	// Set vertical scroll region
		    ఒకవేళ (arg1 < arg2)
		    {
			gui.scroll_region_ఎడమ = arg1;
			gui.scroll_region_కుడి = arg2;
		    }
		    లేకపోతే
		    {
			gui.scroll_region_ఎడమ = arg2;
			gui.scroll_region_కుడి = arg1;
		    }
		    ఆపు;
		స్థితి 'd':	// Delete line
		    gui_delete_పంక్తులు(gui.అడ్డువరుస, 1);
		    ఆపు;
		స్థితి 'D':	// Delete lines
		    gui_delete_పంక్తులు(gui.అడ్డువరుస, arg1);
		    ఆపు;
		స్థితి 'i':	// Insert line
		    gui_చొప్పించు_పంక్తులు(gui.అడ్డువరుస, 1);
		    ఆపు;
		స్థితి 'I':	// Insert lines
		    gui_చొప్పించు_పంక్తులు(gui.అడ్డువరుస, arg1);
		    ఆపు;
		స్థితి '$':	// Clear to end-of-line
		    gui_clear_ఖండం(gui.అడ్డువరుస, gui.నిలువు, gui.అడ్డువరుస,
							    (పూర్ణాంకం)Columns - 1);
		    ఆపు;
		స్థితి 'h':	// Turn on highlighting
		    gui_ప్రారంభం_highlight(arg1);
		    ఆపు;
		స్థితి 'H':	// Turn off highlighting
		    gui_stop_highlight(arg1);
		    ఆపు;
		స్థితి 'f':	// flash the window (visual bell)
		    gui_mch_flash(arg1 == 0 ? 20 : arg1);
		    ఆపు;
		అప్రమేయ:
		    p = s + 1;	// Skip the ESC
		    ఆపు;
	    }
	    len -= (పూర్ణాంకం)(++p - s);
	    s = p;
	}
	లేకపోతే ఒకవేళ (s[0] < 0x20		// Ctrl character
#నిర్వచితమైతే FEAT_SIGN_ICONS
		&& s[0] != SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
		&& s[0] != MULTISIGN_BYTE
# endif
#అయితే_ముగింపు
		)
	{
	    ఒకవేళ (s[0] == '\n')		// NL
	    {
		gui.నిలువు = 0;
		ఒకవేళ (gui.అడ్డువరుస < gui.scroll_region_bot)
		    gui.అడ్డువరుస++;
		లేకపోతే
		    gui_delete_పంక్తులు(gui.scroll_region_శిఖరం, 1);
	    }
	    లేకపోతే ఒకవేళ (s[0] == '\r')	// CR
	    {
		gui.నిలువు = 0;
	    }
	    లేకపోతే ఒకవేళ (s[0] == '\b')	// Backspace
	    {
		ఒకవేళ (gui.నిలువు)
		    --gui.నిలువు;
	    }
	    లేకపోతే ఒకవేళ (s[0] == Ctrl_L)	// cursor-right
	    {
		++gui.నిలువు;
	    }
	    లేకపోతే ఒకవేళ (s[0] == Ctrl_G)	// Beep
	    {
		gui_mch_beep();
	    }
	    // Other Ctrl character: shouldn't happen!

	    --len;	// Skip this char
	    ++s;
	}
	లేకపోతే
	{
	    p = s;
	    అయితే (len > 0 && (
			*p >= 0x20
#నిర్వచితమైతే FEAT_SIGN_ICONS
			|| *p == SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
			|| *p == MULTISIGN_BYTE
# endif
#అయితే_ముగింపు
			))
	    {
		len--;
		p++;
	    }
	    gui_outstr(s, (పూర్ణాంకం)(p - s));
	    s = p;
	}
    }

    // Postponed update of the cursor (won't work if "can_update_cursor" isn't
    // set).
    ఒకవేళ (force_cursor)
	gui_update_cursor(TRUE, TRUE);

    // When switching to another window the dragging must have stopped.
    // Required for GTK, dragged_sb isn't reset.
    ఒకవేళ (old_curwin != curwin)
	gui.dragged_sb = SBAR_NONE;

    // Update the scrollbars after clearing the screen or when switched
    // to another window.
    // Update the horizontal scrollbar always, it's difficult to check all
    // situations where it might change.
    ఒకవేళ (force_scrollbar || old_curwin != curwin)
	gui_update_scrollbars(force_scrollbar);
    లేకపోతే
	gui_update_horiz_scrollbar(FALSE);
    old_curwin = curwin;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#ఒకవేళ defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#అయితే_ముగింపు

    gui_may_flush();		    // In case vim decides to take a nap
}

/*
 * When ScreenLines[] is invalid, updating the cursor should not be done, it
 * produces wrong results.  Call gui_dont_update_cursor() before that code and
 * gui_can_update_cursor() afterwards.
 */
    శూన్య
gui_dont_update_cursor(పూర్ణాంకం undraw)
{
    ఒకవేళ (!gui.లోపల_use)
	తిరిగి;

    // Undraw the cursor now, we probably can't do it after the change.
    ఒకవేళ (undraw)
	gui_undraw_cursor();
    can_update_cursor = FALSE;
}

    శూన్య
gui_can_update_cursor(శూన్య)
{
    can_update_cursor = TRUE;
    // No need to update the cursor right now, there is always more output
    // after scrolling.
}

/*
 * Disable issuing gui_mch_flush().
 */
    శూన్య
gui_disable_flush(శూన్య)
{
    ++disable_flush;
}

/*
 * Enable issuing gui_mch_flush().
 */
    శూన్య
gui_enable_flush(శూన్య)
{
    --disable_flush;
}

/*
 * Issue gui_mch_flush() if it is not disabled.
 */
    శూన్య
gui_may_flush(శూన్య)
{
    ఒకవేళ (disable_flush == 0)
	gui_mch_flush();
}

    స్థిర శూన్య
gui_outstr(char_u *s, పూర్ణాంకం len)
{
    పూర్ణాంకం	    this_len;
    పూర్ణాంకం	    cells;

    ఒకవేళ (len == 0)
	తిరిగి;

    ఒకవేళ (len < 0)
	len = (పూర్ణాంకం)STRLEN(s);

    అయితే (len > 0)
    {
	ఒకవేళ (has_mbyte)
	{
	    // Find out how many chars fit in the current line.
	    cells = 0;
	    కొరకు (this_len = 0; this_len < len; )
	    {
		cells += (*mb_ptr2cells)(s + this_len);
		ఒకవేళ (gui.నిలువు + cells > Columns)
		    ఆపు;
		this_len += (*mb_ptr2len)(s + this_len);
	    }
	    ఒకవేళ (this_len > len)
		this_len = len;	    // don't include following composing char
	}
	లేకపోతే ఒకవేళ (gui.నిలువు + len > Columns)
	    this_len = Columns - gui.నిలువు;
	లేకపోతే
	    this_len = len;

	(శూన్య)gui_outstr_nowrap(s, this_len,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	s += this_len;
	len -= this_len;
	// fill up for a double-width char that doesn't fit.
	ఒకవేళ (len > 0 && gui.నిలువు < Columns)
	    (శూన్య)gui_outstr_nowrap((char_u *)" ", 1,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	// The cursor may wrap to the next line.
	ఒకవేళ (gui.నిలువు >= Columns)
	{
	    gui.నిలువు = 0;
	    gui.అడ్డువరుస++;
	}
    }
}

#నిర్వచితం_కాకపోతే USE_GTK4
/*
 * Output one character (may be one or two display cells).
 * Caller must check for valid "off".
 * Returns FAIL or OK, just like gui_outstr_nowrap().
 */
    స్థిర పూర్ణాంకం
gui_screenchar(
    పూర్ణాంకం		off,	    // Offset from start of screen
    పూర్ణాంకం		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    పూర్ణాంకం		వెనుక)	    // backup this many chars when using bold trick
{
    char_u	buf[MB_MAXBYTES + 1];

    // Don't draw right half of a double-width UTF-8 char. "cannot happen"
    ఒకవేళ (enc_utf8 && Screen_పంక్తులు[off] == 0)
	తిరిగి OK;

    ఒకవేళ (enc_utf8 && Screen_పంక్తులు_UC[off] != 0)
	// Draw UTF-8 multi-byte character.
	తిరిగి gui_outstr_nowrap(buf, utfc_char2bytes(off, buf),
							 flags, fg, bg, వెనుక);

    ఒకవేళ (enc_dbcs == DBCS_JPNU && Screen_పంక్తులు[off] == 0x8e)
    {
	buf[0] = Screen_పంక్తులు[off];
	buf[1] = ScreenLines2[off];
	తిరిగి gui_outstr_nowrap(buf, 2, flags, fg, bg, వెనుక);
    }

    // Draw non-multi-byte character or DBCS character.
    తిరిగి gui_outstr_nowrap(Screen_పంక్తులు + off,
	    enc_dbcs ? (*mb_ptr2len)(Screen_పంక్తులు + off) : 1,
							 flags, fg, bg, వెనుక);
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_GTK
/*
 * Output the string at the given screen position.  This is used in place
 * of gui_screenchar() where possible because Pango needs as much context
 * as possible to work nicely.  It's a lot faster as well.
 */
    స్థిర పూర్ణాంకం
gui_screenstr(
    పూర్ణాంకం		off,	    // Offset from start of screen
    పూర్ణాంకం		len,	    // string length in screen cells
    పూర్ణాంకం		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    పూర్ణాంకం		వెనుక)	    // backup this many chars when using bold trick
{
    char_u  *buf;
    పూర్ణాంకం	    outlen = 0;
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    retval;

    ఒకవేళ (len <= 0) // "cannot happen"?
	తిరిగి OK;

    ఒకవేళ (enc_utf8)
    {
	buf = alloc(len * MB_MAXBYTES + 1);
	ఒకవేళ (buf == ఖాళీ)
	    తిరిగి OK; // not much we could do here...

	కొరకు (i = off; i < off + len; ++i)
	{
	    ఒకవేళ (Screen_పంక్తులు[i] == 0)
		కొనసాగు; // skip second half of double-width char

	    ఒకవేళ (Screen_పంక్తులు_UC[i] == 0)
		buf[outlen++] = Screen_పంక్తులు[i];
	    లేకపోతే
		outlen += utfc_char2bytes(i, buf + outlen);
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, వెనుక);
	vim_free(buf);

	తిరిగి retval;
    }
    లేకపోతే ఒకవేళ (enc_dbcs == DBCS_JPNU)
    {
	buf = alloc(len * 2 + 1);
	ఒకవేళ (buf == ఖాళీ)
	    తిరిగి OK; // not much we could do here...

	కొరకు (i = off; i < off + len; ++i)
	{
	    buf[outlen++] = Screen_పంక్తులు[i];

	    // handle double-byte single-width char
	    ఒకవేళ (Screen_పంక్తులు[i] == 0x8e)
		buf[outlen++] = ScreenLines2[i];
	    లేకపోతే ఒకవేళ (MB_BYTE2LEN(Screen_పంక్తులు[i]) == 2)
		buf[outlen++] = Screen_పంక్తులు[++i];
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, వెనుక);
	vim_free(buf);

	తిరిగి retval;
    }
    లేకపోతే
    {
	తిరిగి gui_outstr_nowrap(&Screen_పంక్తులు[off], len,
				 flags, fg, bg, వెనుక);
    }
}
#అయితే_ముగింపు // FEAT_GUI_GTK

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
    స్థిర పూర్ణాంకం
gui_outstr_nowrap(
    char_u	*s,
    పూర్ణాంకం		len,
    పూర్ణాంకం		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    పూర్ణాంకం		వెనుక)	    // backup this many chars when using bold trick
{
    long_u	highlight_ముసుగు;
    long_u	hl_ముసుగు_todo;
    guicolor_T	fg_color;
    guicolor_T	bg_color;
    guicolor_T	sp_color;
#ఒకవేళ !defined(FEAT_GUI_GTK)
    GuiFont	font = NOFONT;
    GuiFont	wide_font = NOFONT;
# ifdef FEAT_XFONTSET
    GuiFontset	fontset = NOFONTSET;
# endif
#అయితే_ముగింపు
    attrentry_T	*aep = ఖాళీ;
    పూర్ణాంకం		draw_flags;
    పూర్ణాంకం		నిలువు = gui.నిలువు;
#నిర్వచితమైతే FEAT_SIGN_ICONS
    పూర్ణాంకం		draw_sign = FALSE;
    పూర్ణాంకం		signcol = 0;
    char_u	extra[18];
# ifdef FEAT_NETBEANS_INTG
    పూర్ణాంకం		multi_sign = FALSE;
# endif
#అయితే_ముగింపు

    ఒకవేళ (len < 0)
	len = (పూర్ణాంకం)STRLEN(s);
    ఒకవేళ (len == 0)
	తిరిగి OK;

#నిర్వచితమైతే FEAT_SIGN_ICONS
    ఒకవేళ (*s == SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
	  || *s == MULTISIGN_BYTE
# endif
       )
    {
	size_t	slen = 2;		    // 2 spaces by default

# ifdef FEAT_NETBEANS_INTG
	ఒకవేళ (*s == MULTISIGN_BYTE)
	    multi_sign = TRUE;
# endif
	// draw spaces instead
	ఒకవేళ (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u' &&
		(curwin->w_p_nu || curwin->w_p_rnu))
	{
	    పూర్ణాంకం	n = సంఖ్య_వెడల్పు(curwin);

	    slen = MIN(n, (పూర్ణాంకం)పరిమాణం(extra) - 1);
	}
	vim_memset(extra, ' ', slen);
	extra[slen] = NUL;
	s = extra;
	ఒకవేళ (len == 1 && నిలువు > 0)
	    --నిలువు;
	len = (పూర్ణాంకం)slen;
	ఒకవేళ (len > 2)
	    // right align sign icon in the number column
	    signcol = నిలువు + len - 3;
	లేకపోతే
	    signcol = నిలువు;
	draw_sign = TRUE;
	highlight_ముసుగు = 0;
    }
    లేకపోతే
#అయితే_ముగింపు
    ఒకవేళ (gui.highlight_ముసుగు > HL_ALL)
    {
	aep = syn_gui_attr2entry(gui.highlight_ముసుగు);
	ఒకవేళ (aep == ఖాళీ)	    // highlighting not set
	    highlight_ముసుగు = 0;
	లేకపోతే
	    highlight_ముసుగు = aep->ae_attr;
    }
    లేకపోతే
	highlight_ముసుగు = gui.highlight_ముసుగు;
    hl_ముసుగు_todo = highlight_ముసుగు;

#ఒకవేళ !defined(FEAT_GUI_GTK)
    // Set the font
    ఒకవేళ (aep != ఖాళీ && aep->ae_u.gui.font != NOFONT)
	font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
    లేకపోతే ఒకవేళ (aep != ఖాళీ && aep->ae_u.gui.fontset != NOFONTSET)
	fontset = aep->ae_u.gui.fontset;
# endif
    లేకపోతే
    {
# ifdef FEAT_XFONTSET
	ఒకవేళ (gui.fontset != NOFONTSET)
	    fontset = gui.fontset;
	లేకపోతే
# endif
	    ఒకవేళ (hl_ముసుగు_todo & (HL_BOLD | HL_STANDOUT))
	{
	    ఒకవేళ ((hl_ముసుగు_todo & HL_ITALIC) && gui.boldital_font != NOFONT)
	    {
		font = gui.boldital_font;
		hl_ముసుగు_todo &= ~(HL_BOLD | HL_STANDOUT | HL_ITALIC);
	    }
	    లేకపోతే ఒకవేళ (gui.bold_font != NOFONT)
	    {
		font = gui.bold_font;
		hl_ముసుగు_todo &= ~(HL_BOLD | HL_STANDOUT);
	    }
	    లేకపోతే
		font = gui.norm_font;
	}
	లేకపోతే ఒకవేళ ((hl_ముసుగు_todo & HL_ITALIC) && gui.ital_font != NOFONT)
	{
	    font = gui.ital_font;
	    hl_ముసుగు_todo &= ~HL_ITALIC;
	}
	లేకపోతే
	    font = gui.norm_font;

	/*
	 * Choose correct wide_font by font.  wide_font should be set with font
	 * at same time in above block.  But it will make many "ifdef" nasty
	 * blocks.  So we do it here.
	 */
	ఒకవేళ (font == gui.boldital_font && gui.wide_boldital_font)
	    wide_font = gui.wide_boldital_font;
	లేకపోతే ఒకవేళ (font == gui.bold_font && gui.wide_bold_font)
	    wide_font = gui.wide_bold_font;
	లేకపోతే ఒకవేళ (font == gui.ital_font && gui.wide_ital_font)
	    wide_font = gui.wide_ital_font;
	లేకపోతే ఒకవేళ (font == gui.norm_font && gui.wide_font)
	    wide_font = gui.wide_font;
    }
# ifdef FEAT_XFONTSET
    ఒకవేళ (fontset != NOFONTSET)
	gui_mch_సమితి_fontset(fontset);
    లేకపోతే
# endif
	gui_mch_సమితి_font(font);
#అయితే_ముగింపు

    draw_flags = 0;

    // Set the color
    bg_color = gui.వెనుక_pixel;
    ఒకవేళ ((flags & GUI_MON_IS_CURSOR) && gui.లోపల_focus)
    {
	draw_flags |= DRAW_CURSOR;
	fg_color = fg;
	bg_color = bg;
	sp_color = fg;
    }
    లేకపోతే ఒకవేళ (aep != ఖాళీ)
    {
	fg_color = aep->ae_u.gui.fg_color;
	ఒకవేళ (fg_color == INVALCOLOR)
	    fg_color = gui.norm_pixel;
	bg_color = aep->ae_u.gui.bg_color;
	ఒకవేళ (bg_color == INVALCOLOR)
	    bg_color = gui.వెనుక_pixel;
	sp_color = aep->ae_u.gui.sp_color;
	ఒకవేళ (sp_color == INVALCOLOR)
	    sp_color = fg_color;
    }
    లేకపోతే
    {
	fg_color = gui.norm_pixel;
	sp_color = fg_color;
    }

    ఒకవేళ (highlight_ముసుగు & (HL_INVERSE | HL_STANDOUT))
    {
	gui_mch_సమితి_fg_color(bg_color);
	gui_mch_సమితి_bg_color(fg_color);
    }
    లేకపోతే
    {
	gui_mch_సమితి_fg_color(fg_color);
	gui_mch_సమితి_bg_color(bg_color);
    }
    gui_mch_సమితి_sp_color(sp_color);

    // Clear the selection if we are about to write over it
    ఒకవేళ (!(flags & GUI_MON_NOCLEAR))
	clip_may_clear_selection(gui.అడ్డువరుస, gui.అడ్డువరుస);


    // If there's no bold font, then fake it
    ఒకవేళ (hl_ముసుగు_todo & (HL_BOLD | HL_STANDOUT))
	draw_flags |= DRAW_BOLD;

    /*
     * When drawing bold or italic characters the spill-over from the left
     * neighbor may be destroyed.  Let the caller backup to start redrawing
     * just after a blank.
     */
    ఒకవేళ (వెనుక != 0 && ((draw_flags & DRAW_BOLD) || (highlight_ముసుగు & HL_ITALIC)))
	తిరిగి FAIL;

#ఒకవేళ defined(FEAT_GUI_GTK)
    // If there's no italic font, then fake it.
    // For GTK2, we don't need a different font for italic style.
    ఒకవేళ (hl_ముసుగు_todo & HL_ITALIC)
	draw_flags |= DRAW_ITALIC;

    // Do we underline the text?
    ఒకవేళ (hl_ముసుగు_todo & HL_UNDERLINE)
	draw_flags |= DRAW_UNDERL;

#లేకపోతే
    // Do we underline the text?
    ఒకవేళ ((hl_ముసుగు_todo & HL_UNDERLINE) || (hl_ముసుగు_todo & HL_ITALIC))
	draw_flags |= DRAW_UNDERL;
#అయితే_ముగింపు
    // Do we undercurl the text?
    ఒకవేళ (hl_ముసుగు_todo & HL_UNDERCURL)
	draw_flags |= DRAW_UNDERC;

    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

    // Do we strikethrough the text?
    ఒకవేళ (hl_ముసుగు_todo & HL_STRIKETHROUGH)
	draw_flags |= DRAW_STRIKE;

    // Do we draw transparently?
    ఒకవేళ (flags & GUI_MON_TRS_CURSOR)
	draw_flags |= DRAW_TRANSP;

    /*
     * Draw the text.
     */
#నిర్వచితమైతే FEAT_GUI_GTK
    // The value returned is the length in display cells
    len = gui_gtk_draw_string(gui.అడ్డువరుస, నిలువు, s, len, draw_flags);
#లేకపోతే
    ఒకవేళ (enc_utf8)
    {
	పూర్ణాంకం	ప్రారంభం;		// index of bytes to be drawn
	పూర్ణాంకం	cells;		// cellwidth of bytes to be drawn
	పూర్ణాంకం	thislen;	// length of bytes to be drawn
	పూర్ణాంకం	cn;		// cellwidth of current char
	పూర్ణాంకం	i;		// index of current char
	పూర్ణాంకం	c;		// current char value
	పూర్ణాంకం	cl;		// byte length of current char
	పూర్ణాంకం	comping;	// current char is composing
	పూర్ణాంకం	scol = నిలువు;	// screen column
	పూర్ణాంకం	curr_wide = FALSE;  // use 'guifontwide'
	పూర్ణాంకం	prev_wide = FALSE;
	పూర్ణాంకం	wide_మారింది;
# ifdef MSWIN
	పూర్ణాంకం	sep_comp = FALSE;   // Don't separate composing chars.
# లేకపోతే
	పూర్ణాంకం	sep_comp = TRUE;    // Separate composing chars.
# endif

	// Break the string at a composing character, it has to be drawn on
	// top of the previous character.
	ప్రారంభం = 0;
	cells = 0;
	కొరకు (i = 0; i < len; i += cl)
	{
	    c = utf_ptr2char(s + i);
	    cn = utf_char2cells(c);
	    comping = utf_iscomposing(c);
	    ఒకవేళ (!comping)	// count cells from non-composing chars
		cells += cn;
	    ఒకవేళ (!comping || sep_comp)
	    {
		ఒకవేళ (cn > 1
# ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
# endif
			&& wide_font != NOFONT)
		    curr_wide = TRUE;
		లేకపోతే
		    curr_wide = FALSE;
	    }
	    cl = utf_ptr2len(s + i);
	    ఒకవేళ (cl == 0)	// hit end of string
		len = i + cl;	// len must be wrong "cannot happen"

	    wide_మారింది = curr_wide != prev_wide;

	    // Print the string so far if it's the last character or there is
	    // a composing character.
	    ఒకవేళ (i + cl >= len || (comping && sep_comp && i > ప్రారంభం)
		    || wide_మారింది
# ఒకవేళ defined(FEAT_GUI_X11)
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
		ఒకవేళ ((comping && sep_comp) || wide_మారింది)
		    thislen = i - ప్రారంభం;
		లేకపోతే
		    thislen = i - ప్రారంభం + cl;
		ఒకవేళ (thislen > 0)
		{
		    ఒకవేళ (prev_wide)
			gui_mch_సమితి_font(wide_font);
		    gui_mch_draw_string(gui.అడ్డువరుస, scol, s + ప్రారంభం, thislen,
								  draw_flags);
		    ఒకవేళ (prev_wide)
			gui_mch_సమితి_font(font);
		    ప్రారంభం += thislen;
		}
		scol += cells;
		cells = 0;
		// Adjust to not draw a character which width is changed
		// against with last one.
		ఒకవేళ (wide_మారింది && !(comping && sep_comp))
		{
		    scol -= cn;
		    cl = 0;
		}

# ఒకవేళ defined(FEAT_GUI_X11)
		// No fontset: draw a space to fill the gap after a wide char
		//
		ఒకవేళ (cn > 1 && (draw_flags & DRAW_TRANSP) == 0
#  ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
#  endif
			&& !wide_మారింది)
		    gui_mch_draw_string(gui.అడ్డువరుస, scol - 1, (char_u *)" ",
							       1, draw_flags);
# endif
	    }
	    // Draw a composing char on top of the previous char.
	    ఒకవేళ (comping && sep_comp)
	    {
# ఒకవేళ defined(__APPLE_CC__) && లక్ష్యం_API_MAC_CARBON
		// Carbon ATSUI autodraws composing char over previous char
		gui_mch_draw_string(gui.అడ్డువరుస, scol, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# లేకపోతే
		gui_mch_draw_string(gui.అడ్డువరుస, scol - cn, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# endif
		ప్రారంభం = i + cl;
	    }
	    prev_wide = curr_wide;
	}
	// The stuff below assumes "len" is the length in screen columns.
	len = scol - నిలువు;
    }
    లేకపోతే
    {
	gui_mch_draw_string(gui.అడ్డువరుస, నిలువు, s, len, draw_flags);
	ఒకవేళ (enc_dbcs == DBCS_JPNU)
	{
	    // Get the length in display cells, this can be different from the
	    // number of bytes for "euc-jp".
	    len = mb_string2cells(s, len);
	}
    }
#అయితే_ముగింపు // !FEAT_GUI_GTK

    ఒకవేళ (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
	gui.నిలువు = నిలువు + len;

    // May need to invert it when it's part of the selection.
    ఒకవేళ (flags & GUI_MON_NOCLEAR)
	clip_may_redraw_selection(gui.అడ్డువరుస, నిలువు, len);

    ఒకవేళ (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
    {
	// Invalidate the old physical cursor position if we wrote over it
	ఒకవేళ (gui.cursor_అడ్డువరుస == gui.అడ్డువరుస
		&& gui.cursor_నిలువు >= నిలువు
		&& gui.cursor_నిలువు < నిలువు + len)
	    gui.cursor_is_valid = false;
    }

#నిర్వచితమైతే FEAT_SIGN_ICONS
    ఒకవేళ (draw_sign)
	// Draw the sign on top of the spaces.
	gui_mch_drawsign(gui.అడ్డువరుస, signcol, gui.highlight_ముసుగు);
# ఒకవేళ defined(FEAT_NETBEANS_INTG) && (defined(FEAT_GUI_X11) \
	|| defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN))
    ఒకవేళ (multi_sign)
	netbeans_draw_multisign_indicator(gui.అడ్డువరుస);
# endif
#అయితే_ముగింపు

    తిరిగి OK;
}

/*
 * Undraw the cursor.  This actually redraws the character at the cursor
 * position, plus some more characters when needed.
 */
    శూన్య
gui_undraw_cursor(శూన్య)
{
    ఒకవేళ (!gui.cursor_is_valid)
	తిరిగి;

    // Always redraw the character just before if there is one, because
    // with some fonts and characters there can be a one pixel overlap.
    పూర్ణాంకం startcol = gui.cursor_నిలువు > 0 ? gui.cursor_నిలువు - 1 : gui.cursor_నిలువు;
    పూర్ణాంకం endcol = gui.cursor_నిలువు;

#ఒకవేళ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    gui_adjust_undraw_cursor_for_ligatures(&startcol, &endcol);
#అయితే_ముగింపు
    gui_redraw_ఖండం(gui.cursor_అడ్డువరుస, startcol,
	    gui.cursor_అడ్డువరుస, endcol, GUI_MON_NOCLEAR);

    // Cursor_is_valid is reset when the cursor is undrawn, also reset it
    // here in case it wasn't needed to undraw it.
    gui.cursor_is_valid = false;
}

    శూన్య
gui_redraw(
    పూర్ణాంకం		x,
    పూర్ణాంకం		y,
    పూర్ణాంకం		w,
    పూర్ణాంకం		h)
{
    పూర్ణాంకం		row1, col1, row2, col2;

    row1 = Y_2_అడ్డువరుస(y);
    col1 = X_2_నిలువు(x);
    row2 = Y_2_అడ్డువరుస(y + h - 1);
    col2 = X_2_నిలువు(x + w - 1);

    gui_redraw_ఖండం(row1, col1, row2, col2, GUI_MON_NOCLEAR);

    /*
     * We may need to redraw the cursor, but don't take it upon us to change
     * its location after a scroll.
     * (maybe be more strict even and test col too?)
     * These things may be outside the update/clipping region and reality may
     * not reflect Vims internal ideas if these operations are clipped away.
     */
    ఒకవేళ (gui.అడ్డువరుస == gui.cursor_అడ్డువరుస)
	gui_update_cursor(TRUE, TRUE);
}

/*
 * Draw a rectangular block of characters, from row1 to row2 (inclusive) and
 * from col1 to col2 (inclusive).
 */
    శూన్య
gui_redraw_ఖండం(
    పూర్ణాంకం		row1,
    పూర్ణాంకం		col1,
    పూర్ణాంకం		row2,
    పూర్ణాంకం		col2,
    పూర్ణాంకం		flags)	// flags for gui_outstr_nowrap()
{
    పూర్ణాంకం		old_అడ్డువరుస, old_నిలువు;
    long_u	old_hl_ముసుగు;
    పూర్ణాంకం		off;
    sattr_T	మొదటి_attr;
    పూర్ణాంకం		idx, len;
    పూర్ణాంకం		వెనుక, nback;
    పూర్ణాంకం		orig_col1, orig_col2;

    // Don't try to update when ScreenLines is not valid
    ఒకవేళ (!screen_cleared || Screen_పంక్తులు == ఖాళీ)
	తిరిగి;

    // Don't try to draw outside the shell!
    // Check everything, strange values may be caused by a big border width
    col1 = check_నిలువు(col1);
    col2 = check_నిలువు(col2);
    row1 = check_అడ్డువరుస(row1);
    row2 = check_అడ్డువరుస(row2);

    // Remember where our cursor was
    old_అడ్డువరుస = gui.అడ్డువరుస;
    old_నిలువు = gui.నిలువు;
    old_hl_ముసుగు = gui.highlight_ముసుగు;
    orig_col1 = col1;
    orig_col2 = col2;

    కొరకు (gui.అడ్డువరుస = row1; gui.అడ్డువరుస <= row2; gui.అడ్డువరుస++)
    {
	// When only half of a double-wide character is in the block, include
	// the other half.
	col1 = orig_col1;
	col2 = orig_col2;
	off = పంక్తి_స్థానభ్రంశం[gui.అడ్డువరుస];
	ఒకవేళ (enc_dbcs != 0)
	{
	    ఒకవేళ (col1 > 0)
		col1 -= dbcs_screen_తల_off(Screen_పంక్తులు + off,
						    Screen_పంక్తులు + off + col1);
	    col2 += dbcs_screen_తోక_off(Screen_పంక్తులు + off,
						    Screen_పంక్తులు + off + col2);
	}
	లేకపోతే ఒకవేళ (enc_utf8)
	{
	    ఒకవేళ (Screen_పంక్తులు[off + col1] == 0)
	    {
		ఒకవేళ (col1 > 0)
		    --col1;
		లేకపోతే
		    // FIXME: how can the first character ever be zero?
		    siemsg("NUL in ScreenLines in row %ld", (దీర్ఘ)gui.అడ్డువరుస);
	    }
#నిర్వచితమైతే FEAT_GUI_GTK
	    ఒకవేళ (col2 + 1 < Columns && Screen_పంక్తులు[off + col2 + 1] == 0)
		++col2;
#అయితే_ముగింపు
	}
	gui.నిలువు = col1;
	off = పంక్తి_స్థానభ్రంశం[gui.అడ్డువరుస] + gui.నిలువు;
	len = col2 - col1 + 1;

	// Find how many chars back this highlighting starts, or where a space
	// is.  Needed for when the bold trick is used
	కొరకు (వెనుక = 0; వెనుక < col1; ++వెనుక)
	    ఒకవేళ (ScreenAttrs[off - 1 - వెనుక] != ScreenAttrs[off]
		    || Screen_పంక్తులు[off - 1 - వెనుక] == ' ')
		ఆపు;

	// Break it up in strings of characters with the same attributes.
	// Print UTF-8 characters individually.
	అయితే (len > 0)
	{
	    మొదటి_attr = ScreenAttrs[off];
	    gui.highlight_ముసుగు = మొదటి_attr;
#ఒకవేళ !defined(FEAT_GUI_GTK)
	    ఒకవేళ (enc_utf8 && Screen_పంక్తులు_UC[off] != 0)
	    {
		// output multi-byte character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, వెనుక);
		ఒకవేళ (gui.నిలువు < Columns && Screen_పంక్తులు[off + 1] == 0)
		    idx = 2;
		లేకపోతే
		    idx = 1;
	    }
	    లేకపోతే ఒకవేళ (enc_dbcs == DBCS_JPNU && Screen_పంక్తులు[off] == 0x8e)
	    {
		// output double-byte, single-width character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, వెనుక);
		idx = 1;
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    {
#నిర్వచితమైతే FEAT_GUI_GTK
		కొరకు (idx = 0; idx < len; ++idx)
		{
		    ఒకవేళ (enc_utf8 && Screen_పంక్తులు[off + idx] == 0)
			కొనసాగు; // skip second half of double-width char
		    ఒకవేళ (ScreenAttrs[off + idx] != మొదటి_attr)
			ఆపు;
		}
		// gui_screenstr() takes care of multibyte chars
		nback = gui_screenstr(off, idx, flags,
				      (guicolor_T)0, (guicolor_T)0, వెనుక);
#లేకపోతే
		కొరకు (idx = 0; idx < len && ScreenAttrs[off + idx] == మొదటి_attr;
									idx++)
		{
		    // Stop at a multi-byte Unicode character.
		    ఒకవేళ (enc_utf8 && Screen_పంక్తులు_UC[off + idx] != 0)
			ఆపు;
		    ఒకవేళ (enc_dbcs == DBCS_JPNU)
		    {
			// Stop at a double-byte single-width char.
			ఒకవేళ (Screen_పంక్తులు[off + idx] == 0x8e)
			    ఆపు;
			ఒకవేళ (len > 1 && (*mb_ptr2len)(Screen_పంక్తులు
							    + off + idx) == 2)
			    ++idx;  // skip second byte of double-byte char
		    }
		}
		nback = gui_outstr_nowrap(Screen_పంక్తులు + off, idx, flags,
					  (guicolor_T)0, (guicolor_T)0, వెనుక);
#అయితే_ముగింపు
	    }
	    ఒకవేళ (nback == FAIL)
	    {
		// Must back up to start drawing where a bold or italic word
		// starts.
		off -= వెనుక;
		len += వెనుక;
		gui.నిలువు -= వెనుక;
	    }
	    లేకపోతే
	    {
		off += idx;
		len -= idx;
	    }
	    వెనుక = 0;
	}
    }

    // Put the cursor back where it was
    gui.అడ్డువరుస = old_అడ్డువరుస;
    gui.నిలువు = old_నిలువు;
    gui.highlight_ముసుగు = (పూర్ణాంకం)old_hl_ముసుగు;
}

    స్థిర శూన్య
gui_delete_పంక్తులు(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం లెక్క)
{
    ఒకవేళ (లెక్క <= 0)
	తిరిగి;

    ఒకవేళ (అడ్డువరుస + లెక్క > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_ఖండం(అడ్డువరుస, gui.scroll_region_ఎడమ,
			      gui.scroll_region_bot, gui.scroll_region_కుడి);
    లేకపోతే
    {
	gui_mch_delete_పంక్తులు(అడ్డువరుస, లెక్క);

	// If the cursor was in the deleted lines it's now gone.  If the
	// cursor was in the scrolled lines adjust its position.
	ఒకవేళ (gui.cursor_అడ్డువరుస >= అడ్డువరుస
		&& gui.cursor_నిలువు >= gui.scroll_region_ఎడమ
		&& gui.cursor_నిలువు <= gui.scroll_region_కుడి)
	{
	    ఒకవేళ (gui.cursor_అడ్డువరుస < అడ్డువరుస + లెక్క)
		gui.cursor_is_valid = false;
	    లేకపోతే ఒకవేళ (gui.cursor_అడ్డువరుస <= gui.scroll_region_bot)
		gui.cursor_అడ్డువరుస -= లెక్క;
	}
    }
}

    స్థిర శూన్య
gui_చొప్పించు_పంక్తులు(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం లెక్క)
{
    ఒకవేళ (లెక్క <= 0)
	తిరిగి;

    ఒకవేళ (అడ్డువరుస + లెక్క > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_ఖండం(అడ్డువరుస, gui.scroll_region_ఎడమ,
			      gui.scroll_region_bot, gui.scroll_region_కుడి);
    లేకపోతే
    {
	gui_mch_చొప్పించు_పంక్తులు(అడ్డువరుస, లెక్క);

	ఒకవేళ (gui.cursor_అడ్డువరుస >= gui.అడ్డువరుస
		&& gui.cursor_నిలువు >= gui.scroll_region_ఎడమ
		&& gui.cursor_నిలువు <= gui.scroll_region_కుడి)
	{
	    ఒకవేళ (gui.cursor_అడ్డువరుస <= gui.scroll_region_bot - లెక్క)
		gui.cursor_అడ్డువరుస += లెక్క;
	    లేకపోతే ఒకవేళ (gui.cursor_అడ్డువరుస <= gui.scroll_region_bot)
		gui.cursor_is_valid = false;
	}
    }
}

#నిర్వచితమైతే FEAT_TIMERS
/*
 * Passed to ui_wait_for_chars_or_timer(), ignoring extra arguments.
 */
    స్థిర పూర్ణాంకం
gui_wait_for_chars_3(
    దీర్ఘ wtime,
    పూర్ణాంకం *interrupted వాడని,
    పూర్ణాంకం ignore_ఆదానం వాడని)
{
    తిరిగి gui_mch_wait_for_chars(wtime);
}
#అయితే_ముగింపు

/*
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    స్థిర పూర్ణాంకం
gui_wait_for_chars_or_timer(
	దీర్ఘ wtime,
	పూర్ణాంకం *interrupted వాడని,
	పూర్ణాంకం ignore_ఆదానం వాడని)
{
#నిర్వచితమైతే FEAT_TIMERS
    తిరిగి ui_wait_for_chars_or_timer(wtime, gui_wait_for_chars_3,
						    interrupted, ignore_ఆదానం);
#లేకపోతే
    తిరిగి gui_mch_wait_for_chars(wtime);
#అయితే_ముగింపు
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
    స్థిర పూర్ణాంకం
gui_wait_for_chars_buf(
    char_u	*buf,
    పూర్ణాంకం		maxlen,
    దీర్ఘ	wtime,	    // don't use "time", MIPS cannot handle it
    పూర్ణాంకం		tb_change_cnt)
{
    పూర్ణాంకం	    retval;
    పూర్ణాంకం	    keep_blinking; // Guard against restarting blink cycle on CursorHold

#నిర్వచితమైతే FEAT_MENU
    // If we're going to wait a bit, update the menus and mouse shape for the
    // current State.
    ఒకవేళ (wtime != 0)
	gui_update_menus(0);
#అయితే_ముగింపు

    gui_mch_update();
    ఒకవేళ (ఆదానం_అందుబాటు())	// Got char, return immediately
    {
	ఒకవేళ (gui_mch_is_blinking())
	    gui_mch_stop_blink(TRUE);
	ఒకవేళ (buf != ఖాళీ && !typebuf_మారింది(tb_change_cnt))
	    తిరిగి read_from_ఆదానం_buf(buf, (దీర్ఘ)maxlen);
	తిరిగి 0;
    }
    ఒకవేళ (wtime == 0)		// Don't wait for char
	తిరిగి FAIL;

    // Before waiting, flush any output to the screen.
    gui_mch_flush();

    // Blink while waiting for a character.
    ఒకవేళ (!gui_mch_is_blinking())
	gui_mch_ప్రారంభం_blink();

    // Common function to loop until "wtime" is met, while handling timers and
    // other callbacks.
    retval = inchar_loop(buf, maxlen, wtime, tb_change_cnt,
			 gui_wait_for_chars_or_timer, ఖాళీ);

    // Keep blinking when CursorHold wakes the input loop. (See PR #21115)
    keep_blinking = retval == 3 && buf != ఖాళీ
	&& buf[0] == K_SPECIAL && buf[1] == KS_EXTRA
	&& buf[2] == (పూర్ణాంకం)KE_CURSORHOLD;

    ఒకవేళ (!keep_blinking)
	gui_mch_stop_blink(TRUE);

    తిరిగి retval;
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
    పూర్ణాంకం
gui_wait_for_chars(దీర్ఘ wtime, పూర్ణాంకం tb_change_cnt)
{
    తిరిగి gui_wait_for_chars_buf(ఖాళీ, 0, wtime, tb_change_cnt);
}

/*
 * Equivalent of mch_inchar() for the GUI.
 */
    పూర్ణాంకం
gui_inchar(
    char_u  *buf,
    పూర్ణాంకం	    maxlen,
    దీర్ఘ    wtime,		// milliseconds
    పూర్ణాంకం	    tb_change_cnt)
{
    తిరిగి gui_wait_for_chars_buf(buf, maxlen, wtime, tb_change_cnt);
}

/*
 * Fill p[4] with mouse coordinates encoded for check_termcode().
 */
    స్థిర శూన్య
fill_mouse_coord(char_u *p, పూర్ణాంకం నిలువు, పూర్ణాంకం అడ్డువరుస)
{
    p[0] = (char_u)(నిలువు / 128 + ' ' + 1);
    p[1] = (char_u)(నిలువు % 128 + ' ' + 1);
    p[2] = (char_u)(అడ్డువరుస / 128 + ' ' + 1);
    p[3] = (char_u)(అడ్డువరుస % 128 + ' ' + 1);
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
    శూన్య
gui_send_mouse_event(
    పూర్ణాంకం	    button,
    పూర్ణాంకం	    x,
    పూర్ణాంకం	    y,
    పూర్ణాంకం	    repeated_click,
    int_u   modifiers)
{
    స్థిర పూర్ణాంకం	    prev_అడ్డువరుస = 0, prev_నిలువు = 0;
    స్థిర పూర్ణాంకం	    prev_button = -1;
    స్థిర పూర్ణాంకం	    అంకె_clicks = 1;
    char_u	    string[10];
    గణన కీలకం_extra  button_char;
    పూర్ణాంకం		    అడ్డువరుస, నిలువు;
#నిర్వచితమైతే FEAT_CLIPBOARD
    పూర్ణాంకం		    checkfor;
    పూర్ణాంకం		    did_clip = FALSE;
#అయితే_ముగింపు

    /*
     * Scrolling may happen at any time, also while a selection is present.
     */
    ఎంపిక (button)
    {
	స్థితి MOUSE_MOVE:
	    button_char = KE_MOUSEMOVE_XY;
	    వెళ్ళు button_సమితి;
	స్థితి MOUSE_X1:
	    button_char = KE_X1MOUSE;
	    వెళ్ళు button_సమితి;
	స్థితి MOUSE_X2:
	    button_char = KE_X2MOUSE;
	    వెళ్ళు button_సమితి;
	స్థితి MOUSE_4:
	    button_char = KE_MOUSEDOWN;
	    వెళ్ళు button_సమితి;
	స్థితి MOUSE_5:
	    button_char = KE_MOUSEUP;
	    వెళ్ళు button_సమితి;
	స్థితి MOUSE_6:
	    button_char = KE_MOUSELEFT;
	    వెళ్ళు button_సమితి;
	స్థితి MOUSE_7:
	    button_char = KE_MOUSERIGHT;
button_సమితి:
	    {
		// Don't put events in the input queue now.
		ఒకవేళ (hold_gui_events)
		    తిరిగి;

		అడ్డువరుస = gui_xy2colrow(x, y, &నిలువు);
		// Don't report a mouse move unless moved to a
		// different character position.
		ఒకవేళ (button == MOUSE_MOVE)
		{
		    ఒకవేళ (అడ్డువరుస == prev_అడ్డువరుస && నిలువు == prev_నిలువు)
			తిరిగి;
		    లేకపోతే
		    {
			prev_అడ్డువరుస = అడ్డువరుస >= 0 ? అడ్డువరుస : 0;
			prev_నిలువు = నిలువు;
		    }
		}

		string[3] = CSI;
		string[4] = KS_EXTRA;
		string[5] = (పూర్ణాంకం)button_char;

		// Pass the pointer coordinates of the scroll event so that we
		// know which window to scroll.
		string[6] = (char_u)(నిలువు / 128 + ' ' + 1);
		string[7] = (char_u)(నిలువు % 128 + ' ' + 1);
		string[8] = (char_u)(అడ్డువరుస / 128 + ' ' + 1);
		string[9] = (char_u)(అడ్డువరుస % 128 + ' ' + 1);

		ఒకవేళ (modifiers == 0)
		    కూడు_గమ్యం_ఆదానం_buf(string + 3, 7);
		లేకపోతే
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = 0;
		    ఒకవేళ (modifiers & MOUSE_జరుపు)
			string[2] |= MOD_ముసుగు_జరుపు;
		    ఒకవేళ (modifiers & MOUSE_CTRL)
			string[2] |= MOD_ముసుగు_CTRL;
		    ఒకవేళ (modifiers & MOUSE_ALT)
			string[2] |= MOD_ముసుగు_ALT;
		    కూడు_గమ్యం_ఆదానం_buf(string, 10);
		}
		తిరిగి;
	    }
    }

#నిర్వచితమైతే FEAT_CLIPBOARD
    // If a clipboard selection is in progress, handle it
    ఒకవేళ (clip_నక్షత్రం.state == SELECT_లోపల_పురోగతి)
    {
	clip_process_selection(button, X_2_నిలువు(x), Y_2_అడ్డువరుస(y), repeated_click);

	// A release event may still need to be sent if the position is equal.
	అడ్డువరుస = gui_xy2colrow(x, y, &నిలువు);
	ఒకవేళ (button != MOUSE_RELEASE || అడ్డువరుస != prev_అడ్డువరుస || నిలువు != prev_నిలువు)
	    తిరిగి;
    }

    // Determine which mouse settings to look for based on the current mode
    ఎంపిక (get_real_state())
    {
	స్థితి బహుళకం_సాధారణ_BUSY:
	స్థితి బహుళకం_OP_PENDING:
# ifdef FEAT_TERMINAL
	స్థితి బహుళకం_TERMINAL:
# endif
	స్థితి బహుళకం_సాధారణ:	checkfor = MOUSE_సాధారణ; ఆపు;
	స్థితి బహుళకం_VISUAL:	checkfor = MOUSE_VISUAL; ఆపు;
	స్థితి బహుళకం_SELECT:	checkfor = MOUSE_VISUAL; ఆపు;
	స్థితి బహుళకం_REPLACE:
	స్థితి బహుళకం_REPLACE | బహుళకం_LANGMAP:
	స్థితి బహుళకం_VREPLACE:
	స్థితి బహుళకం_VREPLACE | బహుళకం_LANGMAP:
	స్థితి బహుళకం_చొప్పించు:
	స్థితి బహుళకం_చొప్పించు | బహుళకం_LANGMAP:
				checkfor = MOUSE_చొప్పించు; ఆపు;
	స్థితి బహుళకం_ASKMORE:
	స్థితి బహుళకం_HITRETURN:	// At the more- and hit-enter prompt pass the
				// mouse event for a click on or below the
				// message line.
				ఒకవేళ (Y_2_అడ్డువరుస(y) >= సందేశం_అడ్డువరుస)
				    checkfor = MOUSE_సాధారణ;
				లేకపోతే
				    checkfor = MOUSE_RETURN;
				ఆపు;

	    /*
	     * On the command line, use the clipboard selection on all lines
	     * but the command line.  But not when pasting.
	     */
	స్థితి బహుళకం_CMDLINE:
	స్థితి బహుళకం_CMDLINE | బహుళకం_LANGMAP:
	    ఒకవేళ (Y_2_అడ్డువరుస(y) < cmdline_అడ్డువరుస && button != MOUSE_మధ్య)
		checkfor = MOUSE_NONE;
	    లేకపోతే
		checkfor = MOUSE_COMMAND;
	    ఆపు;

	అప్రమేయ:
	    checkfor = MOUSE_NONE;
	    ఆపు;
    };

    /*
     * Allow clipboard selection of text on the command line in "normal"
     * modes.  Don't do this when dragging the status line, or extending a
     * Visual selection.
     */
    ఒకవేళ ((State == బహుళకం_సాధారణ || State == బహుళకం_సాధారణ_BUSY
						      || (State & బహుళకం_చొప్పించు))
	    && X_2_నిలువు(x) >= firstwin->w_wincol
	    && X_2_నిలువు(x) < firstwin->w_wincol + topframe->fr_వెడల్పు
	    && Y_2_అడ్డువరుస(y) >= topframe->fr_ఎత్తు + firstwin->w_winrow
	    && button != MOUSE_DRAG
# ifdef FEAT_MOUSESHAPE
	    && !drag_స్థితి_పంక్తి
	    && !drag_sep_పంక్తి
# endif
	    )
	checkfor = MOUSE_NONE;

    /*
     * Use modeless selection when holding CTRL and SHIFT pressed.
     */
    ఒకవేళ ((modifiers & MOUSE_CTRL) && (modifiers & MOUSE_జరుపు))
	checkfor = MOUSE_NONEF;

    /*
     * In Ex mode, always use modeless selection.
     */
    ఒకవేళ (exmode_చురుకైన)
	checkfor = MOUSE_NONE;

    /*
     * If the mouse settings say to not use the mouse, use the modeless
     * selection.  But if Visual is active, assume that only the Visual area
     * will be selected.
     * Exception: On the command line, both the selection is used and a mouse
     * key is sent.
     */
    ఒకవేళ (!mouse_has(checkfor) || checkfor == MOUSE_COMMAND)
    {
	// Don't do modeless selection in Visual mode.
	ఒకవేళ (checkfor != MOUSE_NONEF && VIsual_చురుకైన && (State & బహుళకం_సాధారణ))
	    తిరిగి;

	/*
	 * When 'mousemodel' is "popup", shift-left is translated to right.
	 * But not when also using Ctrl.
	 */
	ఒకవేళ (mouse_model_popup() && button == MOUSE_ఎడమ
		&& (modifiers & MOUSE_జరుపు) && !(modifiers & MOUSE_CTRL))
	{
	    button = MOUSE_కుడి;
	    modifiers &= ~ MOUSE_జరుపు;
	}

	// If the selection is done, allow the right button to extend it.
	// If the selection is cleared, allow the right button to start it
	// from the cursor position.
	ఒకవేళ (button == MOUSE_కుడి)
	{
	    ఒకవేళ (clip_నక్షత్రం.state == SELECT_CLEARED)
	    {
		ఒకవేళ (State & బహుళకం_CMDLINE)
		{
		    నిలువు = సందేశం_నిలువు;
		    అడ్డువరుస = సందేశం_అడ్డువరుస;
		}
		లేకపోతే
		{
		    నిలువు = curwin->w_wcol;
		    అడ్డువరుస = curwin->w_wrow + W_WINROW(curwin);
		}
		clip_ప్రారంభం_selection(నిలువు, అడ్డువరుస, FALSE);
	    }
	    clip_process_selection(button, X_2_నిలువు(x), Y_2_అడ్డువరుస(y),
							      repeated_click);
	    did_clip = TRUE;
	}
	// Allow the left button to start the selection
	లేకపోతే ఒకవేళ (button == MOUSE_ఎడమ)
	{
	    clip_ప్రారంభం_selection(X_2_నిలువు(x), Y_2_అడ్డువరుస(y), repeated_click);
	    did_clip = TRUE;
	}

	// Always allow pasting
	ఒకవేళ (button != MOUSE_మధ్య)
	{
	    ఒకవేళ (!mouse_has(checkfor) || button == MOUSE_RELEASE)
		తిరిగి;
	    ఒకవేళ (checkfor != MOUSE_COMMAND)
		button = MOUSE_ఎడమ;
	}
	repeated_click = FALSE;
    }

    ఒకవేళ (clip_నక్షత్రం.state != SELECT_CLEARED && !did_clip)
	clip_clear_selection(&clip_నక్షత్రం);
#అయితే_ముగింపు

    // Don't put events in the input queue now.
    ఒకవేళ (hold_gui_events)
	తిరిగి;

    అడ్డువరుస = gui_xy2colrow(x, y, &నిలువు);

    /*
     * If we are dragging and the mouse hasn't moved far enough to be on a
     * different character, then don't send an event to vim.
     */
    ఒకవేళ (button == MOUSE_DRAG)
    {
	ఒకవేళ (అడ్డువరుస == prev_అడ్డువరుస && నిలువు == prev_నిలువు)
	    తిరిగి;
	// Dragging above the window, set "row" to -1 to cause a scroll.
	ఒకవేళ (y < 0)
	    అడ్డువరుస = -1;
    }

    /*
     * If topline has changed (window scrolled) since the last click, reset
     * repeated_click, because we don't want starting Visual mode when
     * clicking on a different character in the text.
     */
    ఒకవేళ (curwin->w_topline != gui_prev_topline
#నిర్వచితమైతే FEAT_DIFF
	    || curwin->w_topfill != gui_prev_topfill
#అయితే_ముగింపు
	    )
	repeated_click = FALSE;

    string[0] = CSI;	// this sequence is recognized by check_termcode()
    string[1] = KS_MOUSE;
    string[2] = KE_FILLER;
    ఒకవేళ (button != MOUSE_DRAG && button != MOUSE_RELEASE)
    {
	ఒకవేళ (repeated_click)
	{
	    /*
	     * Handle multiple clicks.	They only count if the mouse is still
	     * pointing at the same character.
	     */
	    ఒకవేళ (button != prev_button || అడ్డువరుస != prev_అడ్డువరుస || నిలువు != prev_నిలువు)
		అంకె_clicks = 1;
	    లేకపోతే ఒకవేళ (++అంకె_clicks > 4)
		అంకె_clicks = 1;
	}
	లేకపోతే
	    అంకె_clicks = 1;
	prev_button = button;
	gui_prev_topline = curwin->w_topline;
#నిర్వచితమైతే FEAT_DIFF
	gui_prev_topfill = curwin->w_topfill;
#అయితే_ముగింపు

	string[3] = (char_u)(button | 0x20);
	సమితి_అంకె_MOUSE_CLICKS(string[3], అంకె_clicks);
    }
    లేకపోతే
	string[3] = (char_u)button;

    string[3] |= modifiers;
    fill_mouse_coord(string + 4, నిలువు, అడ్డువరుస);
    కూడు_గమ్యం_ఆదానం_buf(string, 8);

    ఒకవేళ (అడ్డువరుస < 0)
	prev_అడ్డువరుస = 0;
    లేకపోతే
	prev_అడ్డువరుస = అడ్డువరుస;
    prev_నిలువు = నిలువు;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#ఒకవేళ defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#అయితే_ముగింపు
}

/*
 * Convert x and y coordinate to column and row in text window.
 * Corrects for multi-byte character.
 * returns column in "*colp" and row as return value;
 */
    స్థిర పూర్ణాంకం
gui_xy2colrow(పూర్ణాంకం x, పూర్ణాంకం y, పూర్ణాంకం *colp)
{
    పూర్ణాంకం		నిలువు = check_నిలువు(X_2_నిలువు(x));
    పూర్ణాంకం		అడ్డువరుస = check_అడ్డువరుస(Y_2_అడ్డువరుస(y));

    *colp = mb_fix_నిలువు(నిలువు, అడ్డువరుస);
    తిరిగి అడ్డువరుస;
}

#ఒకవేళ defined(FEAT_MENU)
/*
 * Callback function for when a menu entry has been selected.
 */
    శూన్య
gui_menu_cb(vimmenu_T *menu)
{
    char_u  బైట్లు[పరిమాణం(long_u)];

    // Don't put events in the input queue now.
    ఒకవేళ (hold_gui_events)
	తిరిగి;

    బైట్లు[0] = CSI;
    బైట్లు[1] = KS_MENU;
    బైట్లు[2] = KE_FILLER;
    కూడు_గమ్యం_ఆదానం_buf(బైట్లు, 3);
    కూడు_long_గమ్యం_buf((long_u)menu, బైట్లు);
    కూడు_గమ్యం_ఆదానం_buf_csi(బైట్లు, పరిమాణం(long_u));
}
#అయితే_ముగింపు

స్థిర పూర్ణాంకం	prev_which_scrollbars[3];

/*
 * Set which components are present.
 * If "oldval" is not NULL, "oldval" is the previous value, the new value is
 * in p_go.
 */
    శూన్య
gui_init_which_components(char_u *oldval వాడని)
{
#నిర్వచితమైతే FEAT_GUI_DARKTHEME
    స్థిర పూర్ణాంకం	prev_dark_theme = FALSE;
    పూర్ణాంకం		using_dark_theme = FALSE;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MENU
    స్థిర పూర్ణాంకం	prev_menu_is_చురుకైన = -1;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TOOLBAR
    స్థిర పూర్ణాంకం	prev_toolbar = -1;
    పూర్ణాంకం		using_toolbar = FALSE;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_TABLINE
    పూర్ణాంకం		using_tabline;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_MSWIN
    స్థిర పూర్ణాంకం	prev_titlebar = FALSE;
    పూర్ణాంకం		using_titlebar = FALSE;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    స్థిర పూర్ణాంకం	prev_fullscreen = FALSE;
    పూర్ణాంకం		using_fullscreen = FALSE;
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_MENU)
    స్థిర పూర్ణాంకం	prev_tearoff = -1;
    పూర్ణాంకం		using_tearoff = FALSE;
#అయితే_ముగింపు

    char_u	*p;
    పూర్ణాంకం		i;
#నిర్వచితమైతే FEAT_MENU
    పూర్ణాంకం		grey_old, grey_new;
    char_u	*తాత్కాలిక;
#అయితే_ముగింపు
    win_T	*wp;
    పూర్ణాంకం		need_సమితి_కొలత;
    పూర్ణాంకం		fix_కొలత;

#నిర్వచితమైతే FEAT_MENU
    ఒకవేళ (oldval != ఖాళీ && gui.లోపల_use)
    {
	/*
	 * Check if the menus go from grey to non-grey or vice versa.
	 */
	grey_old = (vim_strchr(oldval, GO_GREY) != ఖాళీ);
	grey_new = (vim_strchr(p_go, GO_GREY) != ఖాళీ);
	ఒకవేళ (grey_old != grey_new)
	{
	    తాత్కాలిక = p_go;
	    p_go = oldval;
	    gui_update_menus(MENU_ALL_MODES);
	    p_go = తాత్కాలిక;
	}
    }
    gui.menu_is_చురుకైన = false;
#అయితే_ముగింపు

    కొరకు (i = 0; i < 3; i++)
	gui.which_scrollbars[i] = false;
    కొరకు (p = p_go; *p; p++)
	ఎంపిక (*p)
	{
	    స్థితి GO_ఎడమ:
		gui.which_scrollbars[SBAR_ఎడమ] = true;
		ఆపు;
	    స్థితి GO_కుడి:
		gui.which_scrollbars[SBAR_కుడి] = true;
		ఆపు;
	    స్థితి GO_VLEFT:
		ఒకవేళ (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_ఎడమ] = true;
		ఆపు;
	    స్థితి GO_VRIGHT:
		ఒకవేళ (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_కుడి] = true;
		ఆపు;
	    స్థితి GO_BOT:
		gui.which_scrollbars[SBAR_BOTTOM] = true;
		ఆపు;
#నిర్వచితమైతే FEAT_GUI_DARKTHEME
	    స్థితి GO_DARKTHEME:
		using_dark_theme = TRUE;
		ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MENU
	    స్థితి GO_MENUS:
		gui.menu_is_చురుకైన = true;
		ఆపు;
#అయితే_ముగింపు
	    స్థితి GO_GREY:
		// make menu's have grey items, ignored here
		ఆపు;
#నిర్వచితమైతే FEAT_GUI_MSWIN
	    స్థితి GO_TITLEBAR:
		using_titlebar = TRUE;
		ఆపు;
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
	    స్థితి GO_FULLSCREEN:
		using_fullscreen = TRUE;
		ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TOOLBAR
	    స్థితి GO_TOOLBAR:
		using_toolbar = TRUE;
		ఆపు;
#అయితే_ముగింపు
	    స్థితి GO_TEAROFF:
#ఒకవేళ defined(FEAT_MENU)
		using_tearoff = TRUE;
#అయితే_ముగింపు
		ఆపు;
	    అప్రమేయ:
		// Ignore options that are not supported
		ఆపు;
	}

    ఒకవేళ (!gui.లోపల_use)
	తిరిగి;

    need_సమితి_కొలత = 0;
    fix_కొలత = FALSE;

#నిర్వచితమైతే FEAT_GUI_MSWIN
    ఒకవేళ (using_titlebar != prev_titlebar)
    {
	gui_mch_సమితి_titlebar_colors();
	prev_titlebar = using_titlebar;
    }
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    ఒకవేళ (using_fullscreen != prev_fullscreen)
    {
	gui_mch_సమితి_fullscreen(using_fullscreen);
	prev_fullscreen = using_fullscreen;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_DARKTHEME
    ఒకవేళ (using_dark_theme != prev_dark_theme)
    {
	gui_mch_సమితి_dark_theme(using_dark_theme);
	prev_dark_theme = using_dark_theme;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_TABLINE
    // Update the GUI tab line, it may appear or disappear.  This may
    // cause the non-GUI tab line to disappear or appear.
    using_tabline = gui_has_tabline();
    ఒకవేళ (!gui_mch_showing_tabline() != !using_tabline)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = అడ్డువరుసలు;
	gui_update_tabline();
	అడ్డువరుసలు = i;
	need_సమితి_కొలత |= RESIZE_VERT;
	ఒకవేళ (using_tabline)
	    fix_కొలత = TRUE;
	ఒకవేళ (!gui_use_tabline())
	    redraw_tabline = TRUE;    // may draw non-GUI tab line
    }
#అయితే_ముగింపు

    కొరకు (i = 0; i < 3; i++)
    {
	// The scrollbar needs to be updated when it is shown/unshown and
	// when switching tab pages.  But the size only changes when it's
	// shown/unshown.  Thus we need two places to remember whether a
	// scrollbar is there or not.
	ఒకవేళ (gui.which_scrollbars[i] != prev_which_scrollbars[i]
		|| gui.which_scrollbars[i]
		!= curtab->tp_prev_which_scrollbars[i])
	{
	    ఒకవేళ (i == SBAR_BOTTOM)
		gui_mch_enable_scrollbar(&gui.bottom_sbar,
			gui.which_scrollbars[i]);
	    లేకపోతే
	    {
		FOR_ALL_WINDOWS(wp)
		    gui_do_scrollbar(wp, i, gui.which_scrollbars[i]);
	    }
	    ఒకవేళ (gui.which_scrollbars[i] != prev_which_scrollbars[i])
	    {
		ఒకవేళ (i == SBAR_BOTTOM)
		    need_సమితి_కొలత |= RESIZE_VERT;
		లేకపోతే
		    need_సమితి_కొలత |= RESIZE_HOR;
		ఒకవేళ (gui.which_scrollbars[i])
		    fix_కొలత = TRUE;
	    }
	}
	curtab->tp_prev_which_scrollbars[i] = gui.which_scrollbars[i];
	prev_which_scrollbars[i] = gui.which_scrollbars[i];
    }

#నిర్వచితమైతే FEAT_MENU
    ఒకవేళ (gui.menu_is_చురుకైన != prev_menu_is_చురుకైన)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = అడ్డువరుసలు;
	gui_mch_enable_menu(gui.menu_is_చురుకైన);
	అడ్డువరుసలు = i;
	prev_menu_is_చురుకైన = gui.menu_is_చురుకైన;
	need_సమితి_కొలత |= RESIZE_VERT;
	ఒకవేళ (gui.menu_is_చురుకైన)
	    fix_కొలత = TRUE;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TOOLBAR
    ఒకవేళ (using_toolbar != prev_toolbar)
    {
	gui_mch_show_toolbar(using_toolbar);
	prev_toolbar = using_toolbar;
	need_సమితి_కొలత |= RESIZE_VERT;
	ఒకవేళ (using_toolbar)
	    fix_కొలత = TRUE;
    }
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_MENU) && !(defined(MSWIN) && !defined(FEAT_TEAROFF))
    ఒకవేళ (using_tearoff != prev_tearoff)
    {
	gui_mch_toggle_tearoffs(using_tearoff);
	prev_tearoff = using_tearoff;
    }
#అయితే_ముగింపు
    ఒకవేళ (need_సమితి_కొలత != 0)
    {
#నిర్వచితమైతే FEAT_GUI_GTK
	దీర్ఘ    prev_Columns = Columns;
	దీర్ఘ    prev_అడ్డువరుసలు = అడ్డువరుసలు;
#అయితే_ముగింపు
	// Adjust the size of the window to make the text area keep the
	// same size and to avoid that part of our window is off-screen
	// and a scrollbar can't be used, for example.
	gui_సమితి_shellsize(FALSE, fix_కొలత, need_సమితి_కొలత);

#నిర్వచితమైతే FEAT_GUI_GTK
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
	    దీర్ఘ    post_అడ్డువరుసలు = అడ్డువరుసలు;
	    దీర్ఘ    post_Columns = Columns;

	    ఒకవేళ (!gui.starting && need_సమితి_కొలత != (RESIZE_VERT | RESIZE_HOR))
		(శూన్య)char_avail();
	    ఒకవేళ ((need_సమితి_కొలత & RESIZE_VERT) == 0)
		అడ్డువరుసలు = prev_అడ్డువరుసలు;
	    లేకపోతే
		అడ్డువరుసలు = post_అడ్డువరుసలు;
	    ఒకవేళ ((need_సమితి_కొలత & RESIZE_HOR) == 0)
		Columns = prev_Columns;
	    లేకపోతే
		Columns = post_Columns;
	}
#అయితే_ముగింపు
    }
    // When the console tabline appears or disappears the window positions
    // change.
    ఒకవేళ (firstwin->w_winrow != tabline_ఎత్తు())
	shell_new_అడ్డువరుసలు();	// recompute window positions and heights
}

#ఒకవేళ defined(FEAT_GUI_TABLINE)
/*
 * Return TRUE if the GUI is taking care of the tabline.
 * It may still be hidden if 'showtabline' is zero.
 */
    పూర్ణాంకం
gui_use_tabline(శూన్య)
{
    తిరిగి gui.లోపల_use && vim_strchr(p_go, GO_TABLINE) != ఖాళీ;
}

/*
 * Return TRUE if the GUI is showing the tabline.
 * This uses 'showtabline'.
 */
    స్థిర పూర్ణాంకం
gui_has_tabline(శూన్య)
{
    ఒకవేళ (!gui_use_tabline()
	    || p_stal == 0
	    || (p_stal == 1 && మొదటి_tabpage->tp_తదుపరి == ఖాళీ))
	తిరిగి FALSE;
    తిరిగి TRUE;
}

/*
 * Update the tabline.
 * This may display/undisplay the tabline and update the labels.
 */
    శూన్య
gui_update_tabline(శూన్య)
{
    పూర్ణాంకం	    showit = gui_has_tabline();
    పూర్ణాంకం	    shown = gui_mch_showing_tabline();

    ఒకవేళ (!gui.starting && starting == 0)
    {
	// Updating the tabline uses direct GUI commands, flush
	// outstanding instructions first. (esp. clear screen)
	బహిర్గతం_flush();

	ఒకవేళ (!showit != !shown)
	{
	    // Save Rows/Columns before showing/hiding the tabline.
	    // gui_mch_show_tabline() processes GTK events (via
	    // gui_mch_update) which may trigger a spurious configure event
	    // that temporarily corrupts Rows.  Restore the original values so
	    // that gui_set_shellsize() uses the correct row/column count.
	    పూర్ణాంకం save_అడ్డువరుసలు = అడ్డువరుసలు;
	    పూర్ణాంకం save_Columns = Columns;

	    gui_mch_show_tabline(showit);
	    అడ్డువరుసలు = save_అడ్డువరుసలు;
	    Columns = save_Columns;

	    // Resize the outer window to compensate for the tabline height
	    // change.  This is also needed when called from draw_tabline()
	    // (e.g. after :tabclose), where no caller will do the resize.
	    // When called from gui_init_which_components() the subsequent
	    // gui_set_shellsize() call will redo this to the same dimensions,
	    // which is harmless.
	    gui_సమితి_shellsize(FALSE, showit, RESIZE_VERT);
	}
	ఒకవేళ (showit != 0)
	    gui_mch_update_tabline();
    }
}

/*
 * Get the label or tooltip for tab page "tp" into NameBuff[].
 */
    శూన్య
get_tabline_label(
    tabpage_T	*tp,
    పూర్ణాంకం		tooltip)	// TRUE: get tooltip
{
    char_u	**opt;

    // Use 'guitablabel' or 'guitabtooltip' if it's set.
    opt = (tooltip ? &p_gtt : &p_gtl);
    ఒకవేళ (**opt != NUL)
    {
	char_u	res[MAXPATHL];
	tabpage_T *save_curtab;
	char_u	*opt_name = (char_u *)(tooltip ? "guitabtooltip"
							     : "guitablabel");

	printer_పుట_అంకె = tabpage_సూచీ(tp);
# ifdef FEAT_EVAL
	సమితి_vim_var_nr(VV_LNUM, printer_పుట_అంకె);
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
	curbuf = curwin->w_బఫరు;

	// Can't use NameBuff directly, build_stl_str_hl() uses it.
	build_stl_str_hl(curwin, res, MAXPATHL, *opt, opt_name, 0,
					   0, (పూర్ణాంకం)Columns, ఖాళీ, ఖాళీ, ఖాళీ);
	STRCPY(NameBuff, res);

	// Back to the original curtab.
	curtab = save_curtab;
	topframe = curtab->tp_topframe;
	firstwin = curtab->tp_firstwin;
	lastwin = curtab->tp_lastwin;
	curwin = curtab->tp_curwin;
	curbuf = curwin->w_బఫరు;
    }

    // If 'guitablabel'/'guitabtooltip' is not set or the result is empty then
    // use a default label.
    ఒకవేళ (**opt == NUL || *NameBuff == NUL)
    {
	win_T	*wp;
	పూర్ణాంకం	wincount = 0;
	పూర్ణాంకం	modified = FALSE;

	// Get the buffer name into NameBuff[] and shorten it.
	get_trans_bufname(tp == curtab ? curbuf : tp->tp_curwin->w_బఫరు);
	ఒకవేళ (!tooltip)
	    shorten_dir(NameBuff);

	FOR_ALL_WINDOWS_లోపల_TAB(tp, wp)
	{
	    ++wincount;
	    ఒకవేళ (buf_Is_మారింది(wp->w_బఫరు))
		modified = TRUE;
	}

	ఒకవేళ (modified || wincount > 1)
	{
	    char_u  buf[40] = "+ ";		// Tentatively assume modified only
	    size_t  buflen = 2;
	    size_t  NameBufflen = STRLEN(NameBuff);

	    ఒకవేళ (wincount > 1)
		buflen = vim_snprintf_safelen(
		    (అక్షరం *)buf,
		    పరిమాణం(buf),
		    "%d%s ",
		    wincount,
		    modified ? "+" : "");

	    // Make sure resulting NameBuff will not exceed its bounds.
	    ఒకవేళ (NameBufflen + buflen < MAXPATHL)
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
    పూర్ణాంకం
send_tabline_event(పూర్ణాంకం nr)
{
    char_u string[3];

    ఒకవేళ (nr == tabpage_సూచీ(curtab))
	తిరిగి FALSE;

    // Don't put events in the input queue now.
    ఒకవేళ (hold_gui_events || cmdwin_type != 0)
    {
	// Set it back to the current tab page.
	gui_mch_సమితి_curtab(tabpage_సూచీ(curtab));
	తిరిగి FALSE;
    }

    string[0] = CSI;
    string[1] = KS_TABLINE;
    string[2] = KE_FILLER;
    కూడు_గమ్యం_ఆదానం_buf(string, 3);
    string[0] = nr;
    కూడు_గమ్యం_ఆదానం_buf_csi(string, 1);
    తిరిగి TRUE;
}

/*
 * Send a tabline menu event
 */
    శూన్య
send_tabline_menu_event(పూర్ణాంకం tabidx, పూర్ణాంకం event)
{
    char_u	    string[3];

    // Don't put events in the input queue now.
    ఒకవేళ (hold_gui_events)
	తిరిగి;

    // Cannot close the last tabpage.
    ఒకవేళ (event == TABLINE_MENU_CLOSE && మొదటి_tabpage->tp_తదుపరి == ఖాళీ)
	తిరిగి;

    string[0] = CSI;
    string[1] = KS_TABMENU;
    string[2] = KE_FILLER;
    కూడు_గమ్యం_ఆదానం_buf(string, 3);
    string[0] = tabidx;
    string[1] = (char_u)(దీర్ఘ)event;
    కూడు_గమ్యం_ఆదానం_buf_csi(string, 2);
}

#అయితే_ముగింపు

/*
 * Scrollbar stuff:
 */

/*
 * Remove all scrollbars.  Used before switching to another tab page.
 */
    శూన్య
gui_remove_scrollbars(శూన్య)
{
    పూర్ణాంకం	    i;
    win_T   *wp;

    కొరకు (i = 0; i < 3; i++)
    {
	ఒకవేళ (i == SBAR_BOTTOM)
	    gui_mch_enable_scrollbar(&gui.bottom_sbar, FALSE);
	లేకపోతే
	{
	    FOR_ALL_WINDOWS(wp)
		gui_do_scrollbar(wp, i, FALSE);
	}
	curtab->tp_prev_which_scrollbars[i] = -1;
    }
}

    శూన్య
gui_create_scrollbar(scrollbar_T *sb, పూర్ణాంకం type, win_T *wp)
{
    స్థిర పూర్ణాంకం	sbar_ident = 0;

    sb->ident = sbar_ident++;	// No check for too big, but would it happen?
    sb->wp = wp;
    sb->type = type;
    sb->విలువ = 0;
    sb->కొలత = 1;
    sb->max = 1;
    sb->శిఖరం = 0;
    sb->ఎత్తు = 0;
    sb->వెడల్పు = 0;
    sb->స్థితి_ఎత్తు = 0;
    gui_mch_create_scrollbar(sb, (wp == ఖాళీ) ? SBAR_HORIZ : SBAR_VERT);
}

/*
 * Find the scrollbar with the given index.
 */
    scrollbar_T *
gui_కనుగొను_scrollbar(దీర్ఘ ident)
{
    win_T	*wp;

    ఒకవేళ (gui.bottom_sbar.ident == ident)
	తిరిగి &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	ఒకవేళ (wp->w_scrollbars[SBAR_ఎడమ].ident == ident)
	    తిరిగి &wp->w_scrollbars[SBAR_ఎడమ];
	ఒకవేళ (wp->w_scrollbars[SBAR_కుడి].ident == ident)
	    తిరిగి &wp->w_scrollbars[SBAR_కుడి];
    }
    తిరిగి ఖాళీ;
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
    శూన్య
gui_drag_scrollbar(scrollbar_T *sb, దీర్ఘ విలువ, పూర్ణాంకం still_dragging)
{
    win_T	*wp;
    పూర్ణాంకం		sb_అంకె;
#నిర్వచితమైతే USE_ON_FLY_SCROLL
    colnr_T	old_leftcol = curwin->w_leftcol;
    linenr_T	old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    పూర్ణాంకం		old_topfill = curwin->w_topfill;
# endif
#లేకపోతే
    char_u	బైట్లు[పరిమాణం(long_u)];
    పూర్ణాంకం		byte_లెక్క;
#అయితే_ముగింపు

    ఒకవేళ (sb == ఖాళీ)
	తిరిగి;

    // Don't put events in the input queue now.
    ఒకవేళ (hold_gui_events)
	తిరిగి;

    ఒకవేళ (cmdwin_type != 0 && sb->wp != cmdwin_win)
	తిరిగి;

    ఒకవేళ (still_dragging)
    {
	ఒకవేళ (sb->wp == ఖాళీ)
	    gui.dragged_sb = SBAR_BOTTOM;
	లేకపోతే ఒకవేళ (sb == &sb->wp->w_scrollbars[SBAR_ఎడమ])
	    gui.dragged_sb = SBAR_ఎడమ;
	లేకపోతే
	    gui.dragged_sb = SBAR_కుడి;
	gui.dragged_wp = sb->wp;
    }
    లేకపోతే
    {
	gui.dragged_sb = SBAR_NONE;
#నిర్వచితమైతే FEAT_GUI_GTK
	// Keep the "dragged_wp" value until after the scrolling, for when the
	// mouse button is released.  GTK2 doesn't send the button-up event.
	gui.dragged_wp = ఖాళీ;
	// WinScrolled event
	gui_focus_change(TRUE);
#అయితే_ముగింపు
    }

    // Vertical sbar info is kept in the first sbar (the left one)
    ఒకవేళ (sb->wp != ఖాళీ)
	sb = &sb->wp->w_scrollbars[0];

    /*
     * Check validity of value
     */
    ఒకవేళ (విలువ < 0)
	విలువ = 0;
#నిర్వచితమైతే SCROLL_PAST_చివర
    లేకపోతే ఒకవేళ (విలువ > sb->max)
	విలువ = sb->max;
#లేకపోతే
    ఒకవేళ (విలువ > sb->max - sb->కొలత + 1)
	విలువ = sb->max - sb->కొలత + 1;
#అయితే_ముగింపు

    sb->విలువ = విలువ;

#నిర్వచితమైతే USE_ON_FLY_SCROLL
    // When not allowed to do the scrolling right now, return.
    // This also checked input_available(), but that causes the first click in
    // a scrollbar to be ignored when Vim doesn't have focus.
    ఒకవేళ (dont_scroll)
	తిరిగి;
#అయితే_ముగింపు
    // Disallow scrolling the current window when the completion popup menu is
    // visible.
    ఒకవేళ ((sb->wp == ఖాళీ || sb->wp == curwin) && pum_visible())
	తిరిగి;

#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (sb->wp == ఖాళీ && curwin->w_p_rl)
    {
	విలువ = sb->max + 1 - sb->కొలత - విలువ;
	ఒకవేళ (విలువ < 0)
	    విలువ = 0;
    }
#అయితే_ముగింపు

    ఒకవేళ (sb->wp != ఖాళీ)		// vertical scrollbar
    {
	sb_అంకె = 0;
	కొరకు (wp = firstwin; wp != sb->wp && wp != ఖాళీ; wp = wp->w_తదుపరి)
	    sb_అంకె++;
	ఒకవేళ (wp == ఖాళీ)
	    తిరిగి;

#నిర్వచితమైతే USE_ON_FLY_SCROLL
	current_scrollbar = sb_అంకె;
	scrollbar_విలువ = విలువ;
	ఒకవేళ (State & బహుళకం_సాధారణ)
	{
	    gui_do_scroll();
	    setcursor();
	}
	లేకపోతే ఒకవేళ (State & బహుళకం_చొప్పించు)
	{
	    ins_scroll();
	    setcursor();
	}
	లేకపోతే ఒకవేళ (State & బహుళకం_CMDLINE)
	{
	    ఒకవేళ (సందేశం_scrolled == 0)
	    {
		gui_do_scroll();
		redrawcmdline();
	    }
	}
# ifdef FEAT_FOLDING
	// Value may have been changed for closed fold.
	sb->విలువ = sb->wp->w_topline - 1;
# endif

	// When dragging one scrollbar and there is another one at the other
	// side move the thumb of that one too.
	ఒకవేళ (gui.which_scrollbars[SBAR_కుడి] && gui.which_scrollbars[SBAR_ఎడమ])
	    gui_mch_సమితి_scrollbar_thumb(
		    &sb->wp->w_scrollbars[
			    sb == &sb->wp->w_scrollbars[SBAR_కుడి]
						    ? SBAR_ఎడమ : SBAR_కుడి],
		    sb->విలువ, sb->కొలత, sb->max);

#లేకపోతే
	బైట్లు[0] = CSI;
	బైట్లు[1] = KS_VER_SCROLLBAR;
	బైట్లు[2] = KE_FILLER;
	బైట్లు[3] = (char_u)sb_అంకె;
	byte_లెక్క = 4;
#అయితే_ముగింపు
    }
    లేకపోతే
    {
#నిర్వచితమైతే USE_ON_FLY_SCROLL
	scrollbar_విలువ = విలువ;

	ఒకవేళ (State & బహుళకం_సాధారణ)
	    do_mousescroll_horiz(scrollbar_విలువ);
	లేకపోతే ఒకవేళ (State & బహుళకం_చొప్పించు)
	    ins_horscroll();
	లేకపోతే ఒకవేళ (State & బహుళకం_CMDLINE)
	{
	    ఒకవేళ (సందేశం_scrolled == 0)
	    {
		do_mousescroll_horiz(scrollbar_విలువ);
		redrawcmdline();
	    }
	}
	ఒకవేళ (old_leftcol != curwin->w_leftcol)
	{
	    update_కిటికీ(curwin);   // update window, status and cmdline
	    setcursor();
	}
#లేకపోతే
	బైట్లు[0] = CSI;
	బైట్లు[1] = KS_HOR_SCROLLBAR;
	బైట్లు[2] = KE_FILLER;
	byte_లెక్క = 3;
#అయితే_ముగింపు
    }

#నిర్వచితమైతే USE_ON_FLY_SCROLL
    /*
     * synchronize other windows, as necessary according to 'scrollbind'
     */
    ఒకవేళ (curwin->w_p_scb
	    && ((sb->wp == ఖాళీ && curwin->w_leftcol != old_leftcol)
		|| (sb->wp == curwin && (curwin->w_topline != old_topline
# ifdef FEAT_DIFF
					   || curwin->w_topfill != old_topfill
# endif
			))))
    {
	do_check_scrollbind(TRUE);
	// need to update the window right here
	FOR_ALL_WINDOWS(wp)
	    ఒకవేళ (wp->w_redr_type > 0)
		update_కిటికీ(wp);
	setcursor();
    }
    బహిర్గతం_flush_cursor(FALSE, TRUE);
#లేకపోతే
    కూడు_గమ్యం_ఆదానం_buf(బైట్లు, byte_లెక్క);
    కూడు_long_గమ్యం_buf((long_u)విలువ, బైట్లు);
    కూడు_గమ్యం_ఆదానం_buf_csi(బైట్లు, పరిమాణం(long_u));
#అయితే_ముగింపు
}

/*
 * Scrollbar stuff:
 */

/*
 * Called when something in the window layout has changed.
 */
    శూన్య
gui_may_update_scrollbars(శూన్య)
{
    ఒకవేళ (gui.లోపల_use && starting == 0)
    {
	బహిర్గతం_flush();
	gui_init_which_components(ఖాళీ);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
}

    శూన్య
gui_update_scrollbars(
    పూర్ణాంకం		force)	    // Force all scrollbars to get updated
{
    win_T	*wp;
    scrollbar_T	*sb;
    దీర్ఘ	val, కొలత, max;		// need 32 bits here
    పూర్ణాంకం		which_sb;
    పూర్ణాంకం		h, y;
    స్థిర win_T *prev_curwin = ఖాళీ;

    // Update the horizontal scrollbar
    gui_update_horiz_scrollbar(force);

#నిర్వచితం_కాకపోతే MSWIN
    // Return straight away if there is neither a left nor right scrollbar.
    // On MS-Windows this is required anyway for scrollwheel messages.
    ఒకవేళ (!gui.which_scrollbars[SBAR_ఎడమ] && !gui.which_scrollbars[SBAR_కుడి])
	తిరిగి;
#అయితే_ముగింపు

    /*
     * Don't want to update a scrollbar while we're dragging it.  But if we
     * have both a left and right scrollbar, and we drag one of them, we still
     * need to update the other one.
     */
    ఒకవేళ (!force && (gui.dragged_sb == SBAR_ఎడమ || gui.dragged_sb == SBAR_కుడి)
	    && gui.which_scrollbars[SBAR_ఎడమ]
	    && gui.which_scrollbars[SBAR_కుడి])
    {
	/*
	 * If we have two scrollbars and one of them is being dragged, just
	 * copy the scrollbar position from the dragged one to the other one.
	 */
	which_sb = SBAR_ఎడమ + SBAR_కుడి - gui.dragged_sb;
	ఒకవేళ (gui.dragged_wp != ఖాళీ)
	    gui_mch_సమితి_scrollbar_thumb(
		    &gui.dragged_wp->w_scrollbars[which_sb],
		    gui.dragged_wp->w_scrollbars[0].విలువ,
		    gui.dragged_wp->w_scrollbars[0].కొలత,
		    gui.dragged_wp->w_scrollbars[0].max);
    }

    // avoid that moving components around generates events
    ++hold_gui_events;

    FOR_ALL_WINDOWS(wp)
    {
	ఒకవేళ (wp->w_బఫరు == ఖాళీ)	// just in case
	    కొనసాగు;
	// Skip a scrollbar that is being dragged.
	ఒకవేళ (!force && (gui.dragged_sb == SBAR_ఎడమ
					     || gui.dragged_sb == SBAR_కుడి)
		&& gui.dragged_wp == wp)
	    కొనసాగు;

#నిర్వచితమైతే SCROLL_PAST_చివర
	max = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క - 1;
#లేకపోతే
	max = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క + wp->w_ఎత్తు - 2;
#అయితే_ముగింపు
	ఒకవేళ (max < 0)			// empty buffer
	    max = 0;
	val = wp->w_topline - 1;
	కొలత = wp->w_ఎత్తు;
#నిర్వచితమైతే SCROLL_PAST_చివర
	ఒకవేళ (val > max)			// just in case
	    val = max;
#లేకపోతే
	ఒకవేళ (కొలత > max + 1)		// just in case
	    కొలత = max + 1;
	ఒకవేళ (val > max - కొలత + 1)
	    val = max - కొలత + 1;
#అయితే_ముగింపు
	ఒకవేళ (val < 0)			// minimal value is 0
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
	ఒకవేళ (కొలత < 1 || wp->w_botline - 2 > max)
	{
	    /*
	     * This can happen during changing files.  Just don't update the
	     * scrollbar for now.
	     */
	    sb->ఎత్తు = 0;	    // Force update next time
	    ఒకవేళ (gui.which_scrollbars[SBAR_ఎడమ])
		gui_do_scrollbar(wp, SBAR_ఎడమ, FALSE);
	    ఒకవేళ (gui.which_scrollbars[SBAR_కుడి])
		gui_do_scrollbar(wp, SBAR_కుడి, FALSE);
	    కొనసాగు;
	}
	ఒకవేళ (force || sb->ఎత్తు != wp->w_ఎత్తు
	    || sb->శిఖరం != wp->w_winrow
	    || sb->స్థితి_ఎత్తు != wp->w_స్థితి_ఎత్తు
	    || sb->వెడల్పు != wp->w_వెడల్పు
	    || prev_curwin != curwin)
	{
	    // Height, width or position of scrollbar has changed.  For
	    // vertical split: curwin changed.
	    sb->ఎత్తు = wp->w_ఎత్తు;
	    sb->శిఖరం = wp->w_winrow;
	    sb->స్థితి_ఎత్తు = wp->w_స్థితి_ఎత్తు;
	    sb->వెడల్పు = wp->w_వెడల్పు;

	    // Calculate height and position in pixels
	    h = (sb->ఎత్తు + sb->స్థితి_ఎత్తు) * gui.char_ఎత్తు;
	    y = sb->శిఖరం * gui.char_ఎత్తు + gui.border_స్థానభ్రంశం;
#ఒకవేళ defined(FEAT_MENU) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF) && !defined(FEAT_GUI_PHOTON)
	    ఒకవేళ (gui.menu_is_చురుకైన)
		y += gui.menu_ఎత్తు;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU))
	    ఒకవేళ (vim_strchr(p_go, GO_TOOLBAR) != ఖాళీ)
		y += gui.toolbar_ఎత్తు;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
	    ఒకవేళ (gui_has_tabline())
		y += gui.tabline_ఎత్తు;
#అయితే_ముగింపు

	    ఒకవేళ (wp->w_winrow == 0)
	    {
		// Height of top scrollbar includes width of top border
		h += gui.border_స్థానభ్రంశం;
		y -= gui.border_స్థానభ్రంశం;
	    }
	    ఒకవేళ (gui.which_scrollbars[SBAR_ఎడమ])
	    {
		gui_mch_సమితి_scrollbar_pos(&wp->w_scrollbars[SBAR_ఎడమ],
					  gui.ఎడమ_sbar_x, y,
					  gui.scrollbar_వెడల్పు, h);
		gui_do_scrollbar(wp, SBAR_ఎడమ, TRUE);
	    }
	    ఒకవేళ (gui.which_scrollbars[SBAR_కుడి])
	    {
		gui_mch_సమితి_scrollbar_pos(&wp->w_scrollbars[SBAR_కుడి],
					  gui.కుడి_sbar_x, y,
					  gui.scrollbar_వెడల్పు, h);
		gui_do_scrollbar(wp, SBAR_కుడి, TRUE);
	    }
	}

	ఒకవేళ (force || sb->విలువ != val || sb->కొలత != కొలత || sb->max != max)
	{
	    // Thumb of scrollbar has moved
	    sb->విలువ = val;
	    sb->కొలత = కొలత;
	    sb->max = max;
	    ఒకవేళ (gui.which_scrollbars[SBAR_ఎడమ]
		    && (gui.dragged_sb != SBAR_ఎడమ || gui.dragged_wp != wp))
		gui_mch_సమితి_scrollbar_thumb(&wp->w_scrollbars[SBAR_ఎడమ],
					    val, కొలత, max);
	    ఒకవేళ (gui.which_scrollbars[SBAR_కుడి]
		    && (gui.dragged_sb != SBAR_కుడి || gui.dragged_wp != wp))
		gui_mch_సమితి_scrollbar_thumb(&wp->w_scrollbars[SBAR_కుడి],
					    val, కొలత, max);
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
    స్థిర శూన్య
gui_do_scrollbar(
    win_T	*wp,
    పూర్ణాంకం		which,	    // SBAR_LEFT or SBAR_RIGHT
    పూర్ణాంకం		enable)	    // TRUE to enable scrollbar
{
    పూర్ణాంకం		midcol = curwin->w_wincol + curwin->w_వెడల్పు / 2;
    పూర్ణాంకం		has_midcol = (wp->w_wincol <= midcol
				     && wp->w_wincol + wp->w_వెడల్పు >= midcol);

    // Only enable scrollbars that contain the middle column of the current
    // window.
    ఒకవేళ (gui.which_scrollbars[SBAR_కుడి] != gui.which_scrollbars[SBAR_ఎడమ])
    {
	// Scrollbars only on one side.  Don't enable scrollbars that don't
	// contain the middle column of the current window.
	ఒకవేళ (!has_midcol)
	    enable = FALSE;
    }
    లేకపోతే
    {
	// Scrollbars on both sides.  Don't enable scrollbars that neither
	// contain the middle column of the current window nor are on the far
	// side.
	ఒకవేళ (midcol > Columns / 2)
	{
	    ఒకవేళ (which == SBAR_ఎడమ ? wp->w_wincol != 0 : !has_midcol)
		enable = FALSE;
	}
	లేకపోతే
	{
	    ఒకవేళ (which == SBAR_కుడి ? wp->w_wincol + wp->w_వెడల్పు != Columns
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
    పూర్ణాంకం
gui_do_scroll(శూన్య)
{
    win_T	*wp, *save_wp;
    పూర్ణాంకం		i;
    దీర్ఘ	nlines;
    pos_T	old_cursor;
    linenr_T	old_topline;
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		old_topfill;
#అయితే_ముగింపు

    కొరకు (wp = firstwin, i = 0; i < current_scrollbar; wp = W_తదుపరి(wp), i++)
	ఒకవేళ (wp == ఖాళీ)
	    ఆపు;
    ఒకవేళ (wp == ఖాళీ)
	// Couldn't find window
	తిరిగి FALSE;
    // don't redraw, LineOffset and similar are not valid!
    ఒకవేళ (exmode_చురుకైన)
	తిరిగి FALSE;

    /*
     * Compute number of lines to scroll.  If zero, nothing to do.
     */
    nlines = (దీర్ఘ)scrollbar_విలువ + 1 - (దీర్ఘ)wp->w_topline;
    ఒకవేళ (nlines == 0)
	తిరిగి FALSE;

    save_wp = curwin;
    old_topline = wp->w_topline;
#నిర్వచితమైతే FEAT_DIFF
    old_topfill = wp->w_topfill;
#అయితే_ముగింపు
    old_cursor = wp->w_cursor;
    curwin = wp;
    curbuf = wp->w_బఫరు;
    ఒకవేళ (nlines < 0)
	scrolldown(-nlines, gui.dragged_wp == ఖాళీ);
    లేకపోతే
	scrollup(nlines, gui.dragged_wp == ఖాళీ);
    // Reset dragged_wp after using it.  "dragged_sb" will have been reset for
    // the mouse-up event already, but we still want it to behave like when
    // dragging.  But not the next click in an arrow.
    ఒకవేళ (gui.dragged_sb == SBAR_NONE)
	gui.dragged_wp = ఖాళీ;

    ఒకవేళ (old_topline != wp->w_topline
#నిర్వచితమైతే FEAT_DIFF
	    || old_topfill != wp->w_topfill
#అయితే_ముగింపు
	    )
    {
	ఒకవేళ (get_scrolloff_విలువ() != 0)
	{
	    cursor_correct();		// fix window for 'so'
	    update_topline();		// avoid up/down jump
	}
	ఒకవేళ (old_cursor.lnum != wp->w_cursor.lnum)
	    coladvance(wp->w_curswant);
	wp->w_scbind_pos = wp->w_topline;
    }

    // Make sure wp->w_leftcol and wp->w_skipcol are correct.
    validate_cursor();

    curwin = save_wp;
    curbuf = save_wp->w_బఫరు;

    /*
     * Don't call updateWindow() when nothing has changed (it will overwrite
     * the status line!).
     *
     * Check for ScreenLines, because in ex-mode, we don't have a valid display.
     */
    ఒకవేళ (Screen_పంక్తులు != ఖాళీ && (old_topline != wp->w_topline
	    || wp->w_redr_type != 0
#నిర్వచితమైతే FEAT_DIFF
	    || old_topfill != wp->w_topfill
#అయితే_ముగింపు
	    ))
    {
	పూర్ణాంకం type = UPD_VALID;

	ఒకవేళ (pum_visible())
	{
	    type = UPD_NOT_VALID;
	    wp->w_పంక్తులు_valid = 0;
	}

	// Don't set must_redraw here, it may cause the popup menu to
	// disappear when losing focus after a scrollbar drag.
	ఒకవేళ (wp->w_redr_type < type)
	    wp->w_redr_type = type;
	mch_disable_flush();
	update_కిటికీ(wp);   // update window, status line, and cmdline
	mch_enable_flush();
    }

    // May need to redraw the popup menu.
    ఒకవేళ (pum_visible())
	pum_redraw();

    తిరిగి (wp == curwin && !EQUAL_POS(curwin->w_cursor, old_cursor));
}

/*
 * Horizontal scrollbar stuff:
 */
    స్థిర శూన్య
gui_update_horiz_scrollbar(పూర్ణాంకం force)
{
    దీర్ఘ	విలువ, కొలత, max;

    ఒకవేళ (!gui.which_scrollbars[SBAR_BOTTOM])
	తిరిగి;

    ఒకవేళ (!force && gui.dragged_sb == SBAR_BOTTOM)
	తిరిగి;

    ఒకవేళ (!force && curwin->w_p_wrap && gui.prev_wrap)
	తిరిగి;

    /*
     * It is possible for the cursor to be invalid if we're in the middle of
     * something (like changing files).  If so, don't do anything for now.
     */
    ఒకవేళ (curwin->w_cursor.lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
    {
	gui.bottom_sbar.విలువ = -1;
	తిరిగి;
    }

    కొలత = curwin->w_వెడల్పు;
    ఒకవేళ (curwin->w_p_wrap)
    {
	విలువ = 0;
#నిర్వచితమైతే SCROLL_PAST_చివర
	max = 0;
#లేకపోతే
	max = curwin->w_వెడల్పు - 1;
#అయితే_ముగింపు
    }
    లేకపోతే
    {
	విలువ = curwin->w_leftcol;
	max = scroll_పంక్తి_len(ui_కనుగొను_longest_lnum());

	ఒకవేళ (virtual_చురుకైన())
	{
	    // May move the cursor even further to the right.
	    ఒకవేళ (curwin->w_virtcol >= (colnr_T)max)
		max = curwin->w_virtcol;
	}

#నిర్వచితం_కాకపోతే SCROLL_PAST_చివర
	max += curwin->w_వెడల్పు - 1;
#అయితే_ముగింపు
	// The line number isn't scrolled, thus there is less space when
	// 'number' or 'relativenumber' is set (also for 'foldcolumn').
	కొలత -= curwin_నిలువు_off();
#నిర్వచితం_కాకపోతే SCROLL_PAST_చివర
	max -= curwin_నిలువు_off();
#అయితే_ముగింపు
    }

#నిర్వచితం_కాకపోతే SCROLL_PAST_చివర
    ఒకవేళ (విలువ > max - కొలత + 1)
	విలువ = max - కొలత + 1;	    // limit the value to allowable range
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (curwin->w_p_rl)
    {
	విలువ = max + 1 - కొలత - విలువ;
	ఒకవేళ (విలువ < 0)
	{
	    కొలత += విలువ;
	    విలువ = 0;
	}
    }
#అయితే_ముగింపు
    ఒకవేళ (!force && విలువ == gui.bottom_sbar.విలువ && కొలత == gui.bottom_sbar.కొలత
						&& max == gui.bottom_sbar.max)
	తిరిగి;

    gui.bottom_sbar.విలువ = విలువ;
    gui.bottom_sbar.కొలత = కొలత;
    gui.bottom_sbar.max = max;
    gui.prev_wrap = curwin->w_p_wrap;

    gui_mch_సమితి_scrollbar_thumb(&gui.bottom_sbar, విలువ, కొలత, max);
}

/*
 * Check that none of the colors are the same as the background color
 */
    శూన్య
gui_check_colors(శూన్య)
{
    ఒకవేళ (gui.norm_pixel == gui.వెనుక_pixel || gui.norm_pixel == INVALCOLOR)
    {
	gui_సమితి_bg_color((char_u *)"White");
	ఒకవేళ (gui.norm_pixel == gui.వెనుక_pixel || gui.norm_pixel == INVALCOLOR)
	    gui_సమితి_fg_color((char_u *)"Black");
    }
}

    స్థిర శూన్య
gui_సమితి_fg_color(char_u *name)
{
    gui.norm_pixel = gui_get_color(name);
    hl_సమితి_fg_color_name(vim_strsave(name));
}

    స్థిర శూన్య
gui_సమితి_bg_color(char_u *name)
{
    gui.వెనుక_pixel = gui_get_color(name);
    hl_సమితి_bg_color_name(vim_strsave(name));
}

/*
 * Allocate a color by name.
 * Returns INVALCOLOR and gives an error message when failed.
 */
    guicolor_T
gui_get_color(char_u *name)
{
    guicolor_T	t;

    ఒకవేళ (*name == NUL)
	తిరిగి INVALCOLOR;
    t = gui_mch_get_color(name);

    పూర్ణాంకం is_none = STRCMP(name, "none") == 0;
    ఒకవేళ (t == INVALCOLOR
#ఒకవేళ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
	    && (gui.లోపల_use || is_none)
#అయితే_ముగింపు
	    )
    {
	ఒకవేళ (is_none)
	    emsg(_(e_cannot_use_color_none_did_you_సగటు_none));
	లేకపోతే
	    semsg(_(e_cannot_allocate_color_str), name);
    }
    తిరిగి t;
}

/*
 * Return the grey value of a color (range 0-255).
 */
    పూర్ణాంకం
gui_get_lightness(guicolor_T pixel)
{
    long_u	rgb = (long_u)gui_mch_get_rgb(pixel);

    తిరిగి  (పూర్ణాంకం)(  (((rgb >> 16) & 0xff) * 299)
		   + (((rgb >> 8) & 0xff) * 587)
		   +  ((rgb	  & 0xff) * 114)) / 1000;
}

    char_u *
gui_bg_default(శూన్య)
{
    ఒకవేళ (gui_get_lightness(gui.వెనుక_pixel) < 127)
	తిరిగి (char_u *)"dark";
    తిరిగి (char_u *)"light";
}

/*
 * Option initializations that can only be done after opening the GUI window.
 */
    స్థిర శూన్య
init_gui_options(శూన్య)
{
    // Set the 'background' option according to the lightness of the
    // background color, unless the user has set it already.
    ఒకవేళ (!option_was_సమితి((char_u *)"bg") && STRCMP(p_bg, gui_bg_default()) != 0)
    {
	సమితి_option_విలువ_give_err((char_u *)"bg", 0L, gui_bg_default(), 0);
	highlight_మారింది();
    }
}

#ఒకవేళ defined(FEAT_GUI_X11)
    శూన్య
gui_new_scrollbar_colors(శూన్య)
{
    win_T	*wp;

    // Nothing to do if GUI hasn't started yet.
    ఒకవేళ (!gui.లోపల_use)
	తిరిగి;

    FOR_ALL_WINDOWS(wp)
    {
	gui_mch_సమితి_scrollbar_colors(&(wp->w_scrollbars[SBAR_ఎడమ]));
	gui_mch_సమితి_scrollbar_colors(&(wp->w_scrollbars[SBAR_కుడి]));
    }
    gui_mch_సమితి_scrollbar_colors(&gui.bottom_sbar);
}
#అయితే_ముగింపు

/*
 * Call this when focus has changed.
 */
    శూన్య
gui_focus_change(పూర్ణాంకం లోపల_focus)
{
/*
 * Skip this code to avoid drawing the cursor when debugging and switching
 * between the debugger window and gvim.
 */
#ఒకవేళ 1
    gui.లోపల_focus = లోపల_focus;
    బహిర్గతం_flush_cursor(TRUE, FALSE);

# ifdef FEAT_GUI_MSWIN
    gui_mch_సమితి_titlebar_colors();
# endif

# ifdef FEAT_XIM
    xim_సమితి_focus(లోపల_focus);
# endif

    // Put events in the input queue only when allowed.
    // ui_focus_change() isn't called directly, because it invokes
    // autocommands and that must not happen asynchronously.
    ఒకవేళ (!hold_gui_events
# ఒకవేళ defined(FEAT_GUI_GTK) && defined(FEAT_GUI_DIALOG)
	    && gui.dialogs_చురుకైన == 0
# endif
       )
    {
	char_u  బైట్లు[3];

	బైట్లు[0] = CSI;
	బైట్లు[1] = KS_EXTRA;
	బైట్లు[2] = లోపల_focus ? (పూర్ణాంకం)KE_FOCUSGAINED : (పూర్ణాంకం)KE_FOCUSLOST;
	కూడు_గమ్యం_ఆదానం_buf(బైట్లు, 3);
    }
#అయితే_ముగింపు
}

/*
 * When mouse moved: apply 'mousefocus'.
 * Also updates the mouse pointer shape.
 */
    స్థిర శూన్య
gui_mouse_focus(పూర్ణాంకం x, పూర్ణాంకం y)
{
    win_T	*wp;
    char_u	st[8];

#నిర్వచితమైతే FEAT_MOUSESHAPE
    // Get window pointer, and update mouse shape as well.
    wp = xy2win(x, y, IGNORE_POPUP);
#అయితే_ముగింపు

    // Only handle this when 'mousefocus' set and ...
    ఒకవేళ (p_mousef
	    && !hold_gui_events		// not holding events
	    && (State & (బహుళకం_సాధారణ | బహుళకం_చొప్పించు))// Normal/Visual/Insert mode
	    && State != బహుళకం_HITRETURN	// but not hit-return prompt
	    && సందేశం_scrolled == 0	// no scrolled message
	    && !need_mouse_correct	// not moving the pointer
	    && gui.లోపల_focus)		// gvim in focus
    {
	// Don't move the mouse when it's left or right of the Vim window
	ఒకవేళ (x < 0 || x > Columns * gui.char_వెడల్పు)
	    తిరిగి;
#నిర్వచితం_కాకపోతే FEAT_MOUSESHAPE
	wp = xy2win(x, y, IGNORE_POPUP);
#అయితే_ముగింపు
	ఒకవేళ (wp == curwin || wp == ఖాళీ)
	    తిరిగి;	// still in the same old window, or none at all

	// Ignore position in the tab pages line.
	ఒకవేళ (Y_2_అడ్డువరుస(y) < tabline_ఎత్తు())
	    తిరిగి;

	/*
	 * Format a mouse click on status line input,
	 * ala gui_send_mouse_event(0, x, y, 0, 0);
	 * Trick: Use a column number -1, so that get_pseudo_mouse_code() will
	 * generate a K_LEFTMOUSE_NM key code.
	 */
	ఒకవేళ (ముగింపు_op)
	{
	    // abort the current operator first
	    st[0] = ESC;
	    కూడు_గమ్యం_ఆదానం_buf(st, 1);
	}
	st[0] = CSI;
	st[1] = KS_MOUSE;
	st[2] = KE_FILLER;
	st[3] = (char_u)MOUSE_ఎడమ;
	fill_mouse_coord(st + 4,
		wp->w_wincol == 0 ? -1 : wp->w_wincol + MOUSE_COLOFF,
		wp->w_ఎత్తు + W_WINROW(wp));

	కూడు_గమ్యం_ఆదానం_buf(st, 8);
	st[3] = (char_u)MOUSE_RELEASE;
	కూడు_గమ్యం_ఆదానం_buf(st, 8);
#నిర్వచితమైతే FEAT_GUI_GTK
	// Need to wake up the main loop
	ఒకవేళ (gtk_main_level() > 0)
	    gtk_main_quit();
#అయితే_ముగింపు
    }
}

/*
 * Called when the mouse moved (but not when dragging).
 */
    శూన్య
gui_mouse_moved(పూర్ణాంకం x, పూర్ణాంకం y)
{
    // Ignore this while still starting up.
    ఒకవేళ (!gui.లోపల_use || gui.starting)
	తిరిగి;

    // apply 'mousefocus' and pointer shape
    gui_mouse_focus(x, y);

    ఒకవేళ (p_mousemev
#నిర్వచితమైతే FEAT_PROP_POPUP
	|| popup_ఉపయోగిస్తుంది_mouse_move
#అయితే_ముగింపు
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
gui_mouse_కిటికీ(mouse_కనుగొను_T popup)
{
    పూర్ణాంకం		x, y;

    ఒకవేళ (!(gui.లోపల_use && (p_mousef || popup == కనుగొను_POPUP)))
	తిరిగి ఖాళీ;
    gui_mch_getmouse(&x, &y);

    // Only use the mouse when it's on the Vim window
    ఒకవేళ (x >= 0 && x <= Columns * gui.char_వెడల్పు
	    && y >= 0 && Y_2_అడ్డువరుస(y) >= tabline_ఎత్తు())
	తిరిగి xy2win(x, y, popup);
    తిరిగి ఖాళీ;
}

/*
 * Called when mouse should be moved to window with focus.
 */
    శూన్య
gui_mouse_correct(శూన్య)
{
    win_T	*wp = ఖాళీ;

    need_mouse_correct = FALSE;

    wp = gui_mouse_కిటికీ(IGNORE_POPUP);
    ఒకవేళ (wp == curwin || wp == ఖాళీ)
	తిరిగి;

    // If in other than current window
    validate_cline_అడ్డువరుస();
    gui_mch_setmouse((పూర్ణాంకం)W_ENDCOL(curwin) * gui.char_వెడల్పు - 3,
	    (W_WINROW(curwin) + curwin->w_wrow) * gui.char_ఎత్తు
	    + (gui.char_ఎత్తు) / 2);
}

/*
 * Find window where the mouse pointer "x" / "y" coordinate is in.
 * As a side effect update the shape of the mouse pointer.
 */
    స్థిర win_T *
xy2win(పూర్ణాంకం x, పూర్ణాంకం y, mouse_కనుగొను_T popup)
{
    పూర్ణాంకం		అడ్డువరుస;
    పూర్ణాంకం		నిలువు;
    win_T	*wp;

    అడ్డువరుస = Y_2_అడ్డువరుస(y);
    నిలువు = X_2_నిలువు(x);

    ఒకవేళ (అడ్డువరుస < 0 || నిలువు < 0)		// before first window
	తిరిగి ఖాళీ;
    wp = mouse_కనుగొను_win(&అడ్డువరుస, &నిలువు, popup);
    ఒకవేళ (wp == ఖాళీ)
    {
#నిర్వచితమైతే FEAT_MOUSESHAPE
	update_mouseshape(-2);
#అయితే_ముగింపు
	తిరిగి ఖాళీ;
    }
#నిర్వచితమైతే FEAT_MOUSESHAPE
    ఒకవేళ (State == బహుళకం_HITRETURN || State == బహుళకం_ASKMORE)
    {
	ఒకవేళ (Y_2_అడ్డువరుస(y) >= సందేశం_అడ్డువరుస)
	    update_mouseshape(SHAPE_IDX_MOREL);
	లేకపోతే
	    update_mouseshape(SHAPE_IDX_మరిన్ని);
    }
    లేకపోతే ఒకవేళ (అడ్డువరుస >= wp->w_ఎత్తు + wp->w_స్థితి_ఎత్తు)	// below status line
	update_mouseshape(SHAPE_IDX_CLINE);
    లేకపోతే ఒకవేళ (!(State & బహుళకం_CMDLINE) && wp->w_vsep_వెడల్పు > 0 && నిలువు == wp->w_వెడల్పు
	    && (!(అడ్డువరుస >= wp->w_ఎత్తు && అడ్డువరుస < wp->w_ఎత్తు
	    + wp->w_స్థితి_ఎత్తు) || !stl_connected(wp)) && సందేశం_scrolled == 0)
	update_mouseshape(SHAPE_IDX_VSEP);
    లేకపోతే ఒకవేళ (!(State & బహుళకం_CMDLINE) && wp->w_స్థితి_ఎత్తు > 0
	    && అడ్డువరుస >= wp->w_ఎత్తు && అడ్డువరుస < wp->w_ఎత్తు + wp->w_స్థితి_ఎత్తు
	    && సందేశం_scrolled == 0)
	update_mouseshape(SHAPE_IDX_స్థితి);
    లేకపోతే
	update_mouseshape(-2);
#అయితే_ముగింపు
    తిరిగి wp;
}

/*
 * ":gui" and ":gvim": Change from the terminal version to the GUI version.
 * File names may be given to redefine the args list.
 */
    శూన్య
ex_gui(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    /*
     * Check for "-f" argument: foreground, don't fork.
     * Also don't fork when started with "gvim -f".
     * Do fork when using "gui -b".
     */
    ఒకవేళ (arg[0] == '-'
	    && (arg[1] == 'f' || arg[1] == 'b')
	    && (arg[2] == NUL || VIM_ISWHITE(arg[2])))
    {
	gui.dofork = (arg[1] == 'b');
	eap->arg = skipwhite(eap->arg + 2);
    }
    ఒకవేళ (!gui.లోపల_use)
    {
#ఒకవేళ defined(VIMDLL) && !defined(EXPERIMENTAL_GUI_CMD)
	ఒకవేళ (!gui.starting)
	{
	    emsg(_(e_gui_cannot_be_వాడిన_not_enabled_at_compile_సమయం));
	    తిరిగి;
	}
#అయితే_ముగింపు
	// Clear the command.  Needed for when forking+exiting, to avoid part
	// of the argument ending up after the shell prompt.
	సందేశం_clr_eos_force();
#నిర్వచితమైతే GUI_MAY_SPAWN
	ఒకవేళ (!చివర్లు_excmd2(eap->cmd, eap->arg))
	    gui_ప్రారంభం(eap->arg);
	లేకపోతే
#అయితే_ముగింపు
	    gui_ప్రారంభం(ఖాళీ);
#నిర్వచితమైతే FEAT_పని_CHANNEL
	channel_gui_రిజిస్టరు_all();
#అయితే_ముగింపు
    }
    ఒకవేళ (!చివర్లు_excmd2(eap->cmd, eap->arg))
	ex_తదుపరి(eap);
}

#ఒకవేళ (defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	    || defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON) \
	    || defined(FEAT_GUI_HAIKU)) \
	    && defined(FEAT_TOOLBAR)
/*
 * This is shared between Haiku, Motif, and GTK.
 */

/*
 * Callback function for do_in_runtimepath().
 */
    స్థిర శూన్య
gfp_setname(char_u *fname, శూన్య *cookie)
{
    char_u	*gfp_బఫరు = cookie;

    ఒకవేళ (STRLEN(fname) >= MAXPATHL)
	*gfp_బఫరు = NUL;
    లేకపోతే
	STRCPY(gfp_బఫరు, fname);
}

/*
 * Find the path of bitmap "name" with extension "ext" in 'runtimepath'.
 * Return FAIL for failure and OK if buffer[MAXPATHL] contains the result.
 */
    పూర్ణాంకం
gui_కనుగొను_bitmap(char_u *name, char_u *బఫరు, అక్షరం *ext)
{
    ఒకవేళ (STRLEN(name) > MAXPATHL - 14)
	తిరిగి FAIL;
    vim_snprintf((అక్షరం *)బఫరు, MAXPATHL, "bitmaps/%s.%s", name, ext);
    ఒకవేళ (do_లోపల_runtimepath(బఫరు, 0, gfp_setname, బఫరు) == FAIL
							    || *బఫరు == NUL)
	తిరిగి FAIL;
    తిరిగి OK;
}

# ఒకవేళ !defined(FEAT_GUI_GTK)
/*
 * Given the name of the "icon=" argument, try finding the bitmap file for the
 * icon.  If it is an absolute path name, use it as it is.  Otherwise append
 * "ext" and search for it in 'runtimepath'.
 * The result is put in "buffer[MAXPATHL]".  If something fails "buffer"
 * contains "name".
 */
    శూన్య
gui_కనుగొను_iconfile(char_u *name, char_u *బఫరు, అక్షరం *ext)
{
    char_u	buf[MAXPATHL + 1];

    expand_env(name, బఫరు, MAXPATHL);
    ఒకవేళ (!mch_is_నిండిన_Name(బఫరు) && gui_కనుగొను_bitmap(బఫరు, buf, ext) == OK)
	STRCPY(బఫరు, buf);
}
# endif
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)|| defined(FEAT_GUI_HAIKU)
    శూన్య
display_errors(శూన్య)
{
    char_u	*p;

    ఒకవేళ (isatty(2))
    {
	fflush(stderr);
	తిరిగి;
    }

    ఒకవేళ (error_ga.ga_దత్తాంశం == ఖాళీ)
	తిరిగి;

    // avoid putting up a message box with blanks only
    కొరకు (p = (char_u *)error_ga.ga_దత్తాంశం; *p != NUL; ++p)
	ఒకవేళ (!సురక్షిత_isspace(*p))
	{
	    // Truncate a very long message, it will go off-screen.
	    ఒకవేళ (STRLEN(p) > 2000)
		STRCPY(p + 2000 - 14, "...(truncated)");
	    (శూన్య)do_dialog(VIM_ERROR, (char_u *)_("Error"),
		    p, (char_u *)_("&Ok"), 1, ఖాళీ, FALSE);
	    ఆపు;
	}
    ga_clear(&error_ga);
}
#అయితే_ముగింపు

#ఒకవేళ defined(NO_CONSOLE_ఆదానం)
/*
 * Return TRUE if still starting up and there is no place to enter text.
 * For GTK and X11 we check if stderr is not a tty, which means we were
 * (probably) started from the desktop.  Also check stdin, "vim >& file" does
 * allow typing on stdin.
 */
    పూర్ణాంకం
no_console_ఆదానం(శూన్య)
{
    తిరిగి ((!gui.లోపల_use || gui.starting)
# ifndef NO_CONSOLE
	    && !isatty(0) && !isatty(2)
# endif
	    );
}
#అయితే_ముగింపు

#ఒకవేళ defined(కనుగొను_REPLACE_DIALOG) \
	|| defined(NEED_GUI_UPDATE_SCREEN)
/*
 * Update the current window and the screen.
 */
    శూన్య
gui_update_screen(శూన్య)
{
# ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_పంక్తి = 0;
    linenr_T	conceal_new_cursor_పంక్తి = 0;
    పూర్ణాంకం		conceal_update_పంక్తులు = FALSE;
# endif

    update_topline();
    validate_cursor();

    // Trigger CursorMoved if the cursor moved.
    ఒకవేళ (!ముగింపు_op && (has_cursormoved()
# ifdef FEAT_PROP_POPUP
		|| popup_visible
# endif
# ifdef FEAT_CONCEAL
		|| curwin->w_p_cole > 0
# endif
		) && !EQUAL_POS(చివరి_cursormoved, curwin->w_cursor))
    {
	ఒకవేళ (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, ఖాళీ, ఖాళీ, FALSE, curbuf);
# ifdef FEAT_PROP_POPUP
	ఒకవేళ (popup_visible)
	    popup_check_cursor_pos();
# endif
# ifdef FEAT_CONCEAL
	ఒకవేళ (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_పంక్తి = చివరి_cursormoved.lnum;
	    conceal_new_cursor_పంక్తి = curwin->w_cursor.lnum;
	    conceal_update_పంక్తులు = TRUE;
	}
# endif
	చివరి_cursormoved = curwin->w_cursor;
    }

    ఒకవేళ (!ముగింపు_op)
	may_trigger_win_scrolled_resized();

# ifdef FEAT_CONCEAL
    ఒకవేళ (conceal_update_పంక్తులు
	    && (conceal_old_cursor_పంక్తి != conceal_new_cursor_పంక్తి
		|| conceal_cursor_పంక్తి(curwin)
		|| need_cursor_పంక్తి_redraw))
    {
	ఒకవేళ (conceal_old_cursor_పంక్తి != conceal_new_cursor_పంక్తి)
	    redrawWinline(curwin, conceal_old_cursor_పంక్తి);
	redrawWinline(curwin, conceal_new_cursor_పంక్తి);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_పంక్తి_redraw = FALSE;
    }
# endif
    update_screen(0);	// may need to update the screen
    setcursor();
    బహిర్గతం_flush_cursor(TRUE, FALSE);
}
#అయితే_ముగింపు

#ఒకవేళ defined(కనుగొను_REPLACE_DIALOG)
/*
 * Get the text to use in a find/replace dialog.  Uses the last search pattern
 * if the argument is empty.
 * Returns an allocated string.
 */
    char_u *
get_కనుగొను_dialog_పాఠ్యం(
    char_u	*arg,
    పూర్ణాంకం		*wwordp,	// return: TRUE if \< \> found
    పూర్ణాంకం		*mcasep)	// return: TRUE if \C found
{
    char_u	*పాఠ్యం;

    ఒకవేళ (*arg == NUL)
	పాఠ్యం = చివరి_శోధన_pat();
    లేకపోతే
	పాఠ్యం = arg;
    ఒకవేళ (పాఠ్యం != ఖాళీ)
    {
	పూర్ణాంకం len = (పూర్ణాంకం)STRLEN(పాఠ్యం);

	పాఠ్యం = vim_strnsave(పాఠ్యం, len);
	ఒకవేళ (పాఠ్యం != ఖాళీ)
	{
	    పూర్ణాంకం i;

	    // Remove "\V"
	    ఒకవేళ (len >= 2 && STRNCMP(పాఠ్యం, "\\V", 2) == 0)
	    {
		mch_memmove(పాఠ్యం, పాఠ్యం + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\c" and "\C" and remove.
	    ఒకవేళ (len >= 2 && *పాఠ్యం == '\\' && (పాఠ్యం[1] == 'c' || పాఠ్యం[1] == 'C'))
	    {
		*mcasep = (పాఠ్యం[1] == 'C');
		mch_memmove(పాఠ్యం, పాఠ్యం + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\<text\>" and remove.
	    ఒకవేళ (len >= 4
		    && STRNCMP(పాఠ్యం, "\\<", 2) == 0
		    && STRNCMP(పాఠ్యం + len - 2, "\\>", 2) == 0)
	    {
		*wwordp = TRUE;
		mch_memmove(పాఠ్యం, పాఠ్యం + 2, (size_t)(len - 4));
		పాఠ్యం[len - 4] = NUL;
	    }

	    // Recognize "\/" or "\?" and remove.
	    కొరకు (i = 0; i + 1 < len; ++i)
		ఒకవేళ (పాఠ్యం[i] == '\\' && (పాఠ్యం[i + 1] == '/'
						       || పాఠ్యం[i + 1] == '?'))
		{
		    mch_memmove(పాఠ్యం + i, పాఠ్యం + i + 1, (size_t)(len - i));
		    --len;
		}
	}
    }
    తిరిగి పాఠ్యం;
}

/*
 * Handle the press of a button in the find-replace dialog.
 * Return TRUE when something was added to the input buffer.
 */
    పూర్ణాంకం
gui_do_findrepl(
    పూర్ణాంకం		flags,		// one of FRD_REPLACE, FRD_FINDNEXT, etc.
    char_u	*కనుగొను_పాఠ్యం,
    char_u	*repl_పాఠ్యం,
    పూర్ణాంకం		కిందికి)		// Search downwards.
{
    garray_T	ga;
    పూర్ణాంకం		i;
    పూర్ణాంకం		type = (flags & FRD_TYPE_ముసుగు);
    char_u	*p;
    regmatch_T	regmatch;
    పూర్ణాంకం		save_did_emsg = did_emsg;
    స్థిర పూర్ణాంకం  busy = FALSE;

    // When the screen is being updated we should not change buffers and
    // windows structures, it may cause freed memory to be used.  Also don't
    // do this recursively (pressing "Find" quickly several times).
    ఒకవేళ (updating_screen || busy)
	తిరిగి FALSE;

    // refuse replace when text cannot be changed
    ఒకవేళ ((type == FRD_REPLACE || type == FRD_REPLACEALL) && పాఠ్యం_locked())
	తిరిగి FALSE;

    busy = TRUE;

    ga_init2(&ga, 1, 100);
    ఒకవేళ (type == FRD_REPLACEALL)
	GA_CONCAT_LITERAL(&ga, "%s/");

    GA_CONCAT_LITERAL(&ga, "\\V");
    ఒకవేళ (flags & FRD_సరిపోలిక_CASE)
	GA_CONCAT_LITERAL(&ga, "\\C");
    లేకపోతే
	GA_CONCAT_LITERAL(&ga, "\\c");
    ఒకవేళ (flags & FRD_సంపూర్ణ_మాట)
	GA_CONCAT_LITERAL(&ga, "\\<");
    // escape slash and backslash
    p = vim_strsave_escaped(కనుగొను_పాఠ్యం, (char_u *)"/\\");
    ఒకవేళ (p != ఖాళీ)
	ga_concat(&ga, p);
    vim_free(p);
    ఒకవేళ (flags & FRD_సంపూర్ణ_మాట)
	GA_CONCAT_LITERAL(&ga, "\\>");

    ఒకవేళ (type == FRD_REPLACEALL)
    {
	GA_CONCAT_LITERAL(&ga, "/");
	// Escape slash and backslash.
	// Also escape tilde and ampersand if 'magic' is set.
	p = vim_strsave_escaped(repl_పాఠ్యం,
				p_magic ? (char_u *)"/\\~&" : (char_u *)"/\\");
	ఒకవేళ (p != ఖాళీ)
	    ga_concat(&ga, p);
	vim_free(p);
	GA_CONCAT_LITERAL(&ga, "/g");
    }
    ga_append(&ga, NUL);

    ఒకవేళ (type == FRD_REPLACE)
    {
	// Do the replacement when the text at the cursor matches.  Thus no
	// replacement is done if the cursor was moved!
	regmatch.regprog = vim_regcomp(ga.ga_దత్తాంశం, RE_MAGIC + RE_STRING);
	regmatch.rm_ic = 0;
	ఒకవేళ (regmatch.regprog != ఖాళీ)
	{
	    p = ml_get_cursor();
	    ఒకవేళ (vim_regexec_nl(&regmatch, p, (colnr_T)0)
						   && regmatch.startp[0] == p)
	    {
		// Clear the command line to remove any old "No match"
		// error.
		సందేశం_చివర_prompt();

		ఒకవేళ (u_save_cursor() == OK)
		{
		    // A button was pressed thus undo should be synced.
		    u_sync(FALSE);

		    del_బైట్లు((దీర్ఘ)(regmatch.endp[0] - regmatch.startp[0]),
								FALSE, FALSE);
		    ins_str(repl_పాఠ్యం, STRLEN(repl_పాఠ్యం));
		}
	    }
	    లేకపోతే
		సందేశం(_("No match at cursor, finding next"));
	    vim_regfree(regmatch.regprog);
	}
    }

    ఒకవేళ (type == FRD_REPLACEALL)
    {
	// A button was pressed, thus undo should be synced.
	u_sync(FALSE);
	do_cmdline_cmd(ga.ga_దత్తాంశం);
    }
    లేకపోతే
    {
	పూర్ణాంకం searchflags = శోధన_సందేశం + శోధన_MARK;

	// Search for the next match.
	// Don't skip text under cursor for single replace.
	ఒకవేళ (type == FRD_REPLACE)
	    searchflags += శోధన_ప్రారంభం;
	i = సందేశం_scroll;
	ఒకవేళ (కిందికి)
	{
	    (శూన్య)do_శోధన(ఖాళీ, '/', '/', ga.ga_దత్తాంశం, STRLEN(ga.ga_దత్తాంశం), 1L, searchflags, ఖాళీ);
	}
	లేకపోతే
	{
	    // We need to escape '?' if and only if we are searching in the up
	    // direction
	    p = vim_strsave_escaped(ga.ga_దత్తాంశం, (char_u *)"?");
	    ఒకవేళ (p != ఖాళీ)
		(శూన్య)do_శోధన(ఖాళీ, '?', '?', p, STRLEN(p), 1L, searchflags, ఖాళీ);
	    vim_free(p);
	}

	సందేశం_scroll = i;	    // don't let an error message set msg_scroll
    }

    // Don't want to pass did_emsg to other code, it may cause disabling
    // syntax HL if we were busy redrawing.
    did_emsg = save_did_emsg;

    ఒకవేళ (State & (బహుళకం_సాధారణ | బహుళకం_చొప్పించు))
    {
	gui_update_screen();		// update the screen
	సందేశం_didout = 0;			// overwrite any message
	need_wait_return = FALSE;	// don't wait for return
    }

    vim_free(ga.ga_దత్తాంశం);
    busy = FALSE;
    తిరిగి (ga.ga_len > 0);
}

#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_DROP_FILE)
/*
 * Jump to the window at specified point (x, y).
 */
    స్థిర శూన్య
gui_wingoto_xy(పూర్ణాంకం x, పూర్ణాంకం y)
{
    పూర్ణాంకం		అడ్డువరుస = Y_2_అడ్డువరుస(y);
    పూర్ణాంకం		నిలువు = X_2_నిలువు(x);
    win_T	*wp;

    ఒకవేళ (అడ్డువరుస < 0 || నిలువు < 0)
	తిరిగి;

    wp = mouse_కనుగొను_win(&అడ్డువరుస, &నిలువు, FAIL_POPUP);
    ఒకవేళ (wp != ఖాళీ && wp != curwin)
	win_goto(wp);
}

/*
 * Function passed to handle_drop() for the actions to be done after the
 * argument list has been updated.
 */
    స్థిర శూన్య
drop_callback(శూన్య *cookie)
{
    char_u	*p = cookie;
    పూర్ణాంకం		do_shorten = FALSE;

    // If Shift held down, change to first file's directory.  If the first
    // item is a directory, change to that directory (and let the explorer
    // plugin show the contents).
    ఒకవేళ (p != ఖాళీ)
    {
	ఒకవేళ (mch_isdir(p))
	{
	    ఒకవేళ (mch_chdir((అక్షరం *)p) == 0)
		do_shorten = TRUE;
	}
	లేకపోతే ఒకవేళ (vim_chdirfile(p, "drop") == OK)
	    do_shorten = TRUE;
	vim_free(p);
	ఒకవేళ (do_shorten)
	{
	    shorten_fnames(TRUE);
	    చివరి_chdir_reason = "drop";
	}
    }

    // Update the screen display
    update_screen(UPD_NOT_VALID);
# ifdef FEAT_MENU
    gui_update_menus(0);
# endif
    maketitle();
    setcursor();
    బహిర్గతం_flush_cursor(FALSE, FALSE);
}

/*
 * Process file drop.  Mouse cursor position, key modifiers, name of files
 * and count of files are given.  Argument "fnames[count]" has full pathnames
 * of dropped files, they will be freed in this function, and caller can't use
 * fnames after call this function.
 */
    శూన్య
gui_handle_drop(
    పూర్ణాంకం		x వాడని,
    పూర్ణాంకం		y వాడని,
    int_u	modifiers,
    char_u	**fnames,
    పూర్ణాంకం		లెక్క)
{
    పూర్ణాంకం		i;
    char_u	*p;
    స్థిర పూర్ణాంకం	entered = FALSE;

    /*
     * This function is called by event handlers.  Just in case we get a
     * second event before the first one is handled, ignore the second one.
     * Not sure if this can ever happen, just in case.
     */
    ఒకవేళ (entered)
	తిరిగి;
    entered = TRUE;

    /*
     * When the cursor is at the command line, add the file names to the
     * command line, don't edit the files.
     */
    ఒకవేళ (State & బహుళకం_CMDLINE)
    {
	shorten_filenames(fnames, లెక్క);
	కొరకు (i = 0; i < లెక్క; ++i)
	{
	    ఒకవేళ (fnames[i] != ఖాళీ)
	    {
		ఒకవేళ (i > 0)
		    కూడు_గమ్యం_ఆదానం_buf((char_u*)" ", 1);

		// We don't know what command is used thus we can't be sure
		// about which characters need to be escaped.  Only escape the
		// most common ones.
# ifdef BACKSLASH_లోపల_FILENAME
		p = vim_strsave_escaped(fnames[i], (char_u *)" \t\"|");
# లేకపోతే
		p = vim_strsave_escaped(fnames[i], (char_u *)"\\ \t\"|");
# endif
		ఒకవేళ (p != ఖాళీ)
		    కూడు_గమ్యం_ఆదానం_buf_csi(p, (పూర్ణాంకం)STRLEN(p));
		vim_free(p);
		vim_free(fnames[i]);
	    }
	}
	vim_free(fnames);
    }
    లేకపోతే
    {
	// Go to the window under mouse cursor, then shorten given "fnames" by
	// current window, because a window can have local current dir.
	gui_wingoto_xy(x, y);
	shorten_filenames(fnames, లెక్క);

	// If Shift held down, remember the first item.
	ఒకవేళ ((modifiers & MOUSE_జరుపు) != 0)
	    p = vim_strsave(fnames[0]);
	లేకపోతే
	    p = ఖాళీ;

	// Handle the drop, :edit or :split to get to the file.  This also
	// frees fnames[].  Skip this if there is only one item, it's a
	// directory and Shift is held down.
	ఒకవేళ (లెక్క == 1 && (modifiers & MOUSE_జరుపు) != 0
						     && mch_isdir(fnames[0]))
	{
	    vim_free(fnames[0]);
	    vim_free(fnames);
	    vim_free(p);
	}
	లేకపోతే
	    handle_drop(లెక్క, fnames, (modifiers & MOUSE_CTRL) != 0,
						     drop_callback, (శూన్య *)p);
    }

    entered = FALSE;
}
#అయితే_ముగింపు

/*
 * Check if "key" is to interrupt us.  Handles a key that has not had modifiers
 * applied yet.
 * Return the key with modifiers applied if so, NUL if not.
 */
    పూర్ణాంకం
check_for_అంతరాయం(పూర్ణాంకం కీలకం, పూర్ణాంకం modifiers_arg)
{
    పూర్ణాంకం modifiers = modifiers_arg;
    పూర్ణాంకం c = విలీనం_modify_Other_కీలకాలు(కీలకం, &modifiers);

    ఒకవేళ ((c == Ctrl_C && ctrl_c_interrupts)
#నిర్వచితమైతే UNIX
	    || (intr_char != Ctrl_C && c == intr_char)
#అయితే_ముగింపు
	    )
    {
	got_int = TRUE;
	తిరిగి c;
    }
    తిరిగి NUL;
}

/*
 * If the "--gui-log-file fname" argument is given write the dialog title and
 * message to a file and return TRUE.  Otherwise return FALSE.
 * When there is any problem opening the file or writing to the file this is
 * ignored, showing the dialog might get the test to get stuck.
 */
    పూర్ణాంకం
gui_dialog_log(char_u *title, char_u *సందేశం_2)
{
    char_u  *fname = get_gui_dialog_file();
    FILE    *fd;

    ఒకవేళ (fname == ఖాళీ)
	తిరిగి FALSE;

    fd = mch_fopen((అక్షరం *)fname, "a");
    ఒకవేళ (fd != ఖాళీ)
    {
	fprintf(fd, "%s: %s\n", title, సందేశం_2);
	fclose(fd);
    }
    తిరిగి TRUE;
}
