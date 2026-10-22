/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"

// Structure containing all the GUI information
gui_T gui;

#إذا defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
# define USE_سیٹ_GUIFONTWIDE
ساكن فراغ سیٹ_guifontwide(char_u *font_name);
#جے_انت
ساكن فراغ gui_check_pos(فراغ);
ساكن فراغ gui_reset_scroll_region(فراغ);
ساكن فراغ gui_outstr(char_u *, صحيح);
#جے_بنا_تعریف USE_GTK4
ساكن صحيح gui_screenchar(صحيح off, صحيح flags, guicolor_T fg, guicolor_T bg, صحيح پٹھ);
#جے_انت
ساكن صحيح gui_outstr_nowrap(char_u *s, صحيح len, صحيح flags, guicolor_T fg, guicolor_T bg, صحيح پٹھ);
ساكن فراغ gui_delete_سطریں(صحيح صف, صحيح گنتی);
ساكن فراغ gui_داخل_کرو_سطریں(صحيح صف, صحيح گنتی);
ساكن صحيح gui_xy2colrow(صحيح x, صحيح y, صحيح *colp);
#إذا defined(FEAT_GUI_TABLINE)
ساكن صحيح gui_has_tabline(فراغ);
#جے_انت
ساكن فراغ gui_do_scrollbar(win_T *wp, صحيح which, صحيح enable);
ساكن فراغ gui_update_horiz_scrollbar(صحيح);
ساكن فراغ gui_سیٹ_fg_color(char_u *name);
ساكن فراغ gui_سیٹ_bg_color(char_u *name);
ساكن فراغ init_gui_options(فراغ);
ساكن win_T *xy2win(صحيح x, صحيح y, mouse_لبھو_T popup);

#جے_تعریف_شدہ GUI_MAY_FORK
ساكن فراغ gui_do_fork(فراغ);

ساكن صحيح gui_read_بچہ_pipe(صحيح fd);

// Return values for gui_read_child_pipe
تعداد {
    GUI_بچہ_IO_ERROR,
    GUI_بچہ_OK,
    GUI_بچہ_FAILED
};
#جے_انت

ساكن فراغ gui_attempt_شروع(فراغ);

ساكن صحيح can_update_cursor = TRUE; // can display the cursor
ساكن صحيح disable_flush = 0;	// If > 0, gui_mch_flush() is disabled.

/*
 * gui_start -- Called when user wants to start the GUI.
 *
 * Careful: This function can be called recursively when there is a ":gui"
 * command in the .gvimrc file.  Only the first call should fork, not the
 * recursive call.
 */
    فراغ
gui_شروع(char_u *arg بن_ورتیا)
{
    char_u	*old_رکن;
#جے_تعریف_شدہ GUI_MAY_FORK
    ساكن صحيح	recursive = 0;
#جے_انت
#إذا defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
    حرف	*پیغام = عدم;
#جے_انت

    old_رکن = vim_strsave(T_NAME);

    settmode(TMODE_COOK);		// stop RAW mode
    إذا (بھریا_screen)
	cursor_on();			// needed for ":gui" in .vimrc
    بھریا_screen = FALSE;

    // If GUI fails to start, then we will recover afterwards
    رکن_disable_dec();

#جے_تعریف_شدہ GUI_MAY_FORK
    ++recursive;
    /*
     * Quit the current process and continue in the child.
     * Makes "gvim file" disconnect from the shell it was started in.
     * Don't do this when Vim was started with "-f" or the 'f' flag is present
     * in 'guioptions'.
     * Don't do this when there is a running job, we can only get the status
     * of a child from the parent.
     */
    إذا (gui.dofork && !vim_strchr(p_go, GO_FORG) && recursive <= 1
# ifdef FEAT_کم_CHANNEL
	    && !کم_any_running()
# endif
	    )
    {
	gui_do_fork();
    }
    وإلا
#جے_انت
#جے_تعریف_شدہ GUI_MAY_SPAWN
    إذا (gui.dospawn
# ifdef EXPERIMENTAL_GUI_CMD
	    && gui.dofork
# endif
	    && !vim_strchr(p_go, GO_FORG)
	    && !any_Buf_Is_بدلیا()
# ifdef FEAT_کم_CHANNEL
	    && !کم_any_running()
# endif
	    )
    {
# ifdef EXPERIMENTAL_GUI_CMD
	پیغام =
# endif
	    gui_mch_do_spawn(arg);
    }
    وإلا
#جے_انت
    {
#جے_تعریف_شدہ FEAT_GUI_GTK
	// If there is 'f' in 'guioptions' and specify -g argument,
	// gui_mch_init_check() was not called yet.
	إذا (gui_mch_init_check() != OK)
	    getout_preserve_modified(1);
#جے_انت
	gui_attempt_شروع();
    }

    إذا (!gui.وچ_use)			// failed to start GUI
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
	termcapinit(old_رکن);
	settmode(TMODE_RAW);		// restart RAW mode
	سیٹ_title_defaults();		// set 'title' and 'icon' again
#جے_تعریف_شدہ UNIX
	رکن_سیٹ_win_resize(true);
#جے_انت
#إذا defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
	إذا (پیغام != عدم)
	    emsg(پیغام);
#جے_انت
    }
    وإلا
    {
#جے_تعریف_شدہ HAVE_CLIPMETHOD
	// Reset clipmethod to CLIPMETHOD_NONE
	چنو_clipmethod();
#جے_انت
#جے_تعریف_شدہ FEAT_IMAGE
	update_cell_ناپ();
	update_image_backend();
#جے_انت
    }

#إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    // Enable fullscreen mode
    إذا (vim_strchr(p_go, GO_FULLSCREEN) != عدم)
       gui_mch_سیٹ_fullscreen(TRUE);
#جے_انت

    vim_free(old_رکن);

    // If the GUI started successfully, trigger the GUIEnter event, otherwise
    // the GUIFailed event.
    gui_mch_update();
    apply_autocmds(gui.وچ_use ? EVENT_GUIENTER : EVENT_GUIFAILED,
						   عدم, عدم, FALSE, curbuf);
#جے_تعریف_شدہ GUI_MAY_FORK
    --recursive;
#جے_انت
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
    ساكن فراغ
gui_attempt_شروع(فراغ)
{
    ساكن صحيح recursive = 0;

    ++recursive;
    gui.starting = true;

#جے_تعریف_شدہ FEAT_GUI_GTK
    gui.event_ویلا = GDK_CURRENT_ویلا;
#جے_انت

    termcapinit((char_u *)"builtin_gui");
    gui.starting = recursive - 1;

#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
    إذا (gui.وچ_use)
    {
# ifdef FEAT_EVAL
	کھڑکی	x11_کھڑکی;
	Display	*x11_display;

	إذا (gui_get_x11_windis(&x11_کھڑکی, &x11_display) == OK)
	    سیٹ_vim_var_nr(VV_WINDOWID, (لما)x11_کھڑکی);
# endif
	// Display error messages in a dialog now.
	display_errors();
    }
#جے_انت
    --recursive;
}

#جے_تعریف_شدہ GUI_MAY_FORK

// for waitpid()
# إذا defined(HAVE_SYS_WAIT_H)
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
    ساكن فراغ
gui_do_fork(فراغ)
{
    صحيح		pipefd[2];	// pipe between parent and child
    صحيح		pipe_error;
    صحيح		حالت;
    صحيح		exit_حالت;
    pid_t	pid = -1;

# إذا defined(FEAT_RELTIME) && defined(PROF_NSEC)
    // a timer is not carried forward
    delete_timer();
# endif

    // Setup a pipe between the child and the parent, so that the parent
    // knows when the child has done the setsid() call and is allowed to
    // exit.
    pipe_error = (pipe(pipefd) < 0);
    pid = fork();
    إذا (pid < 0)	    // Fork error
    {
	emsg(_(e_failed_منزل_create_new_process_for_GUI));
	أرجع;
    }
    وإلا إذا (pid > 0)	    //ماپا
    {
	// Give the child some time to do the setsid(), otherwise the
	// exit() may kill the child too (when starting gvim from inside a
	// gvim).
	إذا (!pipe_error)
	{
	    // The read returns when the child closes the pipe (or when
	    // the child dies for some reason).
	    close(pipefd[1]);
	    حالت = gui_read_بچہ_pipe(pipefd[0]);
	    إذا (حالت == GUI_بچہ_FAILED)
	    {
		// The child failed to start the GUI, so the caller must
		// continue. There may be more error information written
		// to stderr by the child.
		waitpid(pid, &exit_حالت, 0);
		emsg(_(e_the_بچہ_process_failed_منزل_شروع_GUI));
		أرجع;
	    }
	    وإلا إذا (حالت == GUI_بچہ_IO_ERROR)
	    {
		pipe_error = TRUE;
	    }
	    // else GUI_CHILD_OK: parent exit
	}

	إذا (pipe_error)
	    ui_delay(301L, TRUE);

	// When swapping screens we may need to go to the next line, e.g.,
	// after a hit-enter prompt and using ":gui".
	إذا (newline_on_exit)
	    mch_errmsg("\r\n");

	/*
	 * The parent must skip the normal exit() processing, the child
	 * will do it.  For example, GTK messes up signals when exiting.
	 */
	_exit(0);
    }
    //بچہ

# ifdef FEAT_GUI_GTK
    // Call gtk_init_check() here after fork(). See gui_init_check().
    إذا (gui_mch_init_check() != OK)
	getout_preserve_modified(1);
# endif

# إذا defined(HAVE_SETSID) || defined(HAVE_SETPGID)
    /*
     * Change our process group.  On some systems/shells a CTRL-C in the
     * shell where Vim was started would otherwise kill gvim!
     */
#  إذا defined(HAVE_SETSID)
    (فراغ)setsid();
#  وإلا
    (فراغ)setpgid(0, 0);
#  endif
# endif
    إذا (!pipe_error)
	close(pipefd[0]);

# إذا defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
    // Tell the session manager our new PID
    gui_mch_forked();
# endif

    // Try to start the GUI
    gui_attempt_شروع();

    // Notify the parent
    إذا (!pipe_error)
    {
	إذا (gui.وچ_use)
	    write_eintr(pipefd[1], "ok", 3);
	وإلا
	    write_eintr(pipefd[1], "fail", 5);
	close(pipefd[1]);
    }

    // If we failed to start the GUI, exit now.
    إذا (!gui.وچ_use)
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
    ساكن صحيح
gui_read_بچہ_pipe(صحيح fd)
{
    لما	بائٹاں_read;
# define READ_بفر_ناپ 10
    حرف	بفر[READ_بفر_ناپ];

    بائٹاں_read = read_eintr(fd, بفر, READ_بفر_ناپ - 1);
# undef READ_بفر_ناپ
    close(fd);
    إذا (بائٹاں_read < 0)
	أرجع GUI_بچہ_IO_ERROR;
    بفر[بائٹاں_read] = NUL;
    إذا (STRCMP(بفر, "ok") == 0)
	أرجع GUI_بچہ_OK;
    أرجع GUI_بچہ_FAILED;
}

#جے_انت // GUI_MAY_FORK

/*
 * Call this when vim starts up, whether or not the GUI is started
 */
    فراغ
gui_prepare(صحيح *argc, حرف **argv)
{
    gui.وچ_use = false;		    // No GUI yet (maybe later)
    gui.starting = false;	    // No GUI yet (maybe later)
    gui_mch_prepare(argc, argv);
}

/*
 * Try initializing the GUI and check if it can be started.
 * Used from main() to check early if "vim -g" can start the GUI.
 * Used from gui_init() to prepare for starting the GUI.
 * Returns FAIL or OK.
 */
    صحيح
gui_init_check(فراغ)
{
    ساكن صحيح نتیجہ = MAYBE;

    إذا (نتیجہ != MAYBE)
    {
	إذا (نتیجہ == FAIL)
	    emsg(_(e_cannot_شروع_the_GUI));
	أرجع نتیجہ;
    }

    gui.shell_created = false;
    gui.dying = false;
    gui.وچ_focus = true;		// so the guicursor setting works
    gui.dragged_sb = SBAR_NONE;
    gui.dragged_wp = عدم;
    gui.اشارہ_گر_hidden = false;
    gui.خانہ = 0;
    gui.صف = 0;
    gui.عدد_خانے_گنتی = Columns;
    gui.عدد_صفاں = صفاں;

    gui.cursor_is_valid = false;
    gui.scroll_region_چوٹی = 0;
    gui.scroll_region_bot = صفاں - 1;
    gui.scroll_region_بایاں = 0;
    gui.scroll_region_دایاں = Columns - 1;
    gui.highlight_نقاب = HL_عام;
    gui.char_چوڑائی = 1;
    gui.char_اچائی = 1;
    gui.char_ascent = 0;
    gui.border_چوڑائی = 0;

    gui.norm_font = NOFONT;
#جے_بنا_تعریف FEAT_GUI_GTK
    gui.bold_font = NOFONT;
    gui.ital_font = NOFONT;
    gui.boldital_font = NOFONT;
# ifdef FEAT_XFONTSET
    gui.fontset = NOFONTSET;
# endif
#جے_انت
    gui.wide_font = NOFONT;
#جے_بنا_تعریف FEAT_GUI_GTK
    gui.wide_bold_font = NOFONT;
    gui.wide_ital_font = NOFONT;
    gui.wide_boldital_font = NOFONT;
#جے_انت

#جے_تعریف_شدہ FEAT_MENU
# ifndef FEAT_GUI_GTK
#  ifdef FONTSET_ALWAYS
    gui.menu_fontset = NOFONTSET;
#  وإلا
    gui.menu_font = NOFONT;
#  endif
# endif
    gui.menu_is_سرگرم = true;	    // default: include menu
# ifndef FEAT_GUI_GTK
    gui.menu_اچائی = MENU_DEFAULT_اچائی;
    gui.menu_چوڑائی = 0;
# endif
#جے_انت
#إذا defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    gui.toolbar_اچائی = 0;
#جے_انت
#جے_تعریف_شدہ FEAT_BEVAL_TIP
    gui.tooltip_fontset = NOFONTSET;
#جے_انت

    gui.scrollbar_چوڑائی = gui.scrollbar_اچائی = SB_DEFAULT_چوڑائی;
    gui.prev_wrap = -1;

#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // Note: gui_set_ligatures() might already have been called e.g. from .vimrc,
    // and in that case we don't want to overwrite ligatures map that has already
    // been correctly populated (as that would lead to a cleared ligatures maps).
    إذا (*p_guiligatures == NUL)
	CLEAR_میدان(gui.ligatures_map);
#جے_انت

#إذا defined(ALWAYS_USE_GUI) || defined(VIMDLL)
    نتیجہ = OK;
#وإلا
# ifdef FEAT_GUI_GTK
    gui.is_x11 = false;
#  ifdef FEAT_GUI_DIALOG
    gui.dialogs_سرگرم = 0;
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
    نتیجہ = gui.dofork ? gui_mch_early_init_check(TRUE) : gui_mch_init_check();
# وإلا
    نتیجہ = gui_mch_init_check();
# endif
#جے_انت
    أرجع نتیجہ;
}

/*
 * This is the call which starts the GUI.
 */
    فراغ
gui_init(فراغ)
{
    win_T	*wp;
    tabpage_T	*tp;
    ساكن صحيح	recursive = 0;

    /*
     * It's possible to use ":gui" in a .gvimrc file.  The first half of this
     * function will then be executed at the first call, the rest by the
     * recursive call.  This allow the shell to be opened halfway reading a
     * gvimrc file.
     */
    إذا (!recursive)
    {
	++recursive;

	clip_init(TRUE);

	// If can't initialize, don't try doing the rest
	إذا (gui_init_check() == FAIL)
	{
	    --recursive;
	    clip_init(FALSE);
	    أرجع;
	}

	/*
	 * Reset 'paste'.  It's useful in the terminal, but not in the GUI.  It
	 * breaks the Paste toolbar button.
	 */
	سیٹ_option_قدر_give_err((char_u *)"paste", 0L, عدم, 0);

	// Set t_Co to the number of colors: RGB.
	سیٹ_color_گنتی(256 * 256 * 256);

	/*
	 * Set up system-wide default menus.
	 */
#إذا defined(SYS_MENU_FILE) && defined(FEAT_MENU)
	إذا (vim_strchr(p_go, GO_NOSYSMENU) == عدم)
	{
	    sys_menu = TRUE;
	    do_سروت((char_u *)SYS_MENU_FILE, FALSE, DOSO_NONE, عدم);
	    sys_menu = FALSE;
	}
#جے_انت

	/*
	 * Switch on the mouse by default, unless the user changed it already.
	 * This can then be changed in the .gvimrc.
	 */
	إذا (!option_was_سیٹ((char_u *)"mouse"))
	    سیٹ_string_option_سدھا((char_u *)"mouse", -1,
					   (char_u *)"a", OPT_FREE, SID_NONE);

	/*
	 * If -U option given, use only the initializations from that file and
	 * nothing else.  Skip all initializations for "-U NONE" or "-u NORC".
	 */
	إذا (use_gvimrc != عدم)
	{
	    إذا (STRCMP(use_gvimrc, "NONE") != 0
		    && STRCMP(use_gvimrc, "NORC") != 0
		    && do_سروت(use_gvimrc, FALSE, DOSO_NONE, عدم) != OK)
		semsg(_(e_cannot_read_from_str), use_gvimrc);
	}
	وإلا
	{
	    /*
	     * Get system wide defaults for gvim, only when file name defined.
	     */
#جے_تعریف_شدہ SYS_GVIMRC_FILE
	    do_سروت((char_u *)SYS_GVIMRC_FILE, FALSE, DOSO_NONE, عدم);
#جے_انت

	    /*
	     * Try to read GUI initialization commands from the following
	     * places:
	     * - environment variable GVIMINIT
	     * - the user gvimrc file (~/.gvimrc)
	     * - the second user gvimrc file ($VIM/.gvimrc for Dos)
	     * - the third user gvimrc file ($VIM/.gvimrc for Amiga)
	     * The first that exists is used, the rest is ignored.
	     */
	    إذا (process_env((char_u *)"GVIMINIT", FALSE) == FAIL
		 && do_سروت((char_u *)USR_GVIMRC_FILE, TRUE,
						     DOSO_GVIMRC, عدم) == FAIL
#جے_تعریف_شدہ USR_GVIMRC_FILE2
		 && do_سروت((char_u *)USR_GVIMRC_FILE2, TRUE,
						     DOSO_GVIMRC, عدم) == FAIL
#جے_انت
#جے_تعریف_شدہ USR_GVIMRC_FILE3
		 && do_سروت((char_u *)USR_GVIMRC_FILE3, TRUE,
						     DOSO_GVIMRC, عدم) == FAIL
#جے_انت
				)
	    {
#جے_تعریف_شدہ USR_GVIMRC_FILE4
		(فراغ)do_سروت((char_u *)USR_GVIMRC_FILE4, TRUE,
							    DOSO_GVIMRC, عدم);
#جے_انت
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
	    إذا (p_exrc)
	    {
#جے_تعریف_شدہ UNIX
		{
		    stat_T s;

		    // if ".gvimrc" file is not owned by user, set 'secure'
		    //منوال
		    إذا (mch_stat(GVIMRC_FILE, &s) || s.st_uid != getuid())
			secure = p_secure;
		}
#وإلا
		secure = p_secure;
#جے_انت

		إذا (       fullpathcmp((char_u *)USR_GVIMRC_FILE,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#جے_تعریف_شدہ SYS_GVIMRC_FILE
			&& fullpathcmp((char_u *)SYS_GVIMRC_FILE,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#جے_انت
#جے_تعریف_شدہ USR_GVIMRC_FILE2
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE2,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#جے_انت
#جے_تعریف_شدہ USR_GVIMRC_FILE3
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE3,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#جے_انت
#جے_تعریف_شدہ USR_GVIMRC_FILE4
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE4,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#جے_انت
			)
		    do_سروت((char_u *)GVIMRC_FILE, TRUE, DOSO_GVIMRC, عدم);

		إذا (secure == 2)
		    need_wait_return = TRUE;
		secure = 0;
	    }
	}

	إذا (need_wait_return || پیغام_didany)
	    wait_return(TRUE);

	--recursive;
    }

    // If recursive call opened the shell, return here from the first call
    إذا (gui.وچ_use)
	أرجع;

    /*
     * Create the GUI shell.
     */
    gui.وچ_use = true;		// Must be set after menus have been set up
#إذا defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
    gui.scale = 1.0; // Default value
#جے_انت
    إذا (gui_mch_init() == FAIL)
	اذهب error;

    // Avoid a delay for an error message that was printed in the terminal
    // where Vim was started.
    emsg_on_display = FALSE;
    پیغام_scrolled = 0;
    clear_sb_متن(TRUE);
    need_wait_return = FALSE;
    پیغام_didany = FALSE;

    /*
     * Check validity of any generic resources that may have been loaded.
     */
    إذا (gui.border_چوڑائی < 0)
	gui.border_چوڑائی = 0;

    /*
     * Set up the fonts.  First use a font specified with "-fn" or "-font".
     */
    إذا (font_دلیل != عدم)
	سیٹ_option_قدر_give_err((char_u *)"gfn",
					       0L, (char_u *)font_دلیل, 0);
    إذا (
#جے_تعریف_شدہ FEAT_XFONTSET
	    (*p_guifontset == NUL
	     || gui_init_font(p_guifontset, TRUE) == FAIL) &&
#جے_انت
	    gui_init_font(*p_guifont == NUL ? hl_get_font_name()
						  : p_guifont, FALSE) == FAIL)
    {
	emsg(_(e_cannot_شروع_gui_no_valid_font_لبھیا));
	اذهب error2;
    }
    إذا (gui_get_wide_font() == FAIL)
	emsg(_(e_guifontwide_invalid));

    gui.عدد_خانے_گنتی = Columns;
    gui.عدد_صفاں = صفاں;
    gui_reset_scroll_region();

    // Create initial scrollbars
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_بایاں], SBAR_بایاں, wp);
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_دایاں], SBAR_دایاں, wp);
    }
    gui_create_scrollbar(&gui.bottom_sbar, SBAR_BOTTOM, عدم);

#جے_تعریف_شدہ FEAT_MENU
    gui_create_initial_menus(جڑ_menu);
#جے_انت
#جے_تعریف_شدہ FEAT_SIGN_ICONS
    sign_gui_started();
#جے_انت

    // Configure the desired menu and scrollbars
    gui_init_which_components(عدم);

    // All components of the GUI have been created now
    gui.shell_created = true;

#جے_تعریف_شدہ FEAT_GUI_MSWIN
    // Set the shell size, adjusted for the screen size.  For GTK this only
    // works after the shell has been opened, thus it is further down.
    // If the window is already maximized (e.g. when --windowid is passed in),
    // we want to use the system-provided dimensions by passing FALSE to
    // mustset. Otherwise, we want to initialize with the default rows/columns.
    إذا (gui_mch_maximized())
	gui_سیٹ_shellsize(FALSE, TRUE, RESIZE_BOTH);
    وإلا
	gui_سیٹ_shellsize(TRUE, TRUE, RESIZE_BOTH);
#وإلا
# ifndef FEAT_GUI_GTK
    gui_سیٹ_shellsize(FALSE, TRUE, RESIZE_BOTH);
# endif
#جے_انت
#إذا defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    // Need to set the size of the menubar after all the menus have been
    // created.
    gui_mch_compute_menu_اچائی((Widget)0);
#جے_انت

    /*
     * Actually open the GUI shell.
     */
    إذا (gui_mch_open() != FAIL)
    {
	maketitle();
	resettitle();

	init_gui_options();
#جے_تعریف_شدہ FEAT_ARABIC
	// Our GUI can't do bidi.
	p_tbidi = FALSE;
#جے_انت
#إذا defined(FEAT_GUI_GTK)
	// Give GTK+ a chance to put all widget's into place.
	gui_mch_update();

# ifdef FEAT_MENU
	// If there is no 'm' in 'guioptions' we need to remove the menu now.
	// It was still there to make F10 work.
	إذا (vim_strchr(p_go, GO_MENUS) == عدم)
	{
	    bool save_starting = gui.starting;

	    gui.starting = false;
	    gui_mch_enable_menu(FALSE);
	    gui.starting = save_starting;
	    gui_mch_update();
	}
# endif

	// Now make sure the shell fits on the screen.
	إذا (gui_mch_maximized())
	    gui_سیٹ_shellsize(FALSE, TRUE, RESIZE_BOTH);
	وإلا
	    gui_سیٹ_shellsize(TRUE, TRUE, RESIZE_BOTH);
#جے_انت
	// When 'lines' was set while starting up the topframe may have to be
	// resized.
	win_new_shellsize();

#جے_تعریف_شدہ FEAT_BEVAL_GUI
	// Always create the Balloon Evaluation area, but disable it when
	// 'ballooneval' is off.
	إذا (balloonEval != عدم)
	{
# ifdef FEAT_VARTABS
	    vim_free(balloonEval->vts);
# endif
	    vim_free(balloonEval);
	}
	balloon_Eval_For_رکن = FALSE;
# ifdef FEAT_GUI_GTK
	balloonEval = gui_mch_create_beval_area(gui.drawarea, عدم,
						     &general_beval_cb, عدم);
# وإلا
#  إذا defined(FEAT_GUI_MOTIF)
	{
	    خارجي Widget	متن_Area;
	    balloonEval = gui_mch_create_beval_area(متن_Area, عدم,
						     &general_beval_cb, عدم);
	}
#  وإلا
#   ifdef FEAT_GUI_MSWIN
	balloonEval = gui_mch_create_beval_area(عدم, عدم,
						     &general_beval_cb, عدم);
#   endif
#  endif
# endif
	إذا (!p_beval)
	    gui_mch_disable_beval_area(balloonEval);
#جے_انت

#جے_بنا_تعریف FEAT_GUI_MSWIN
	// In the GUI modifiers are prepended to keys.
	// Don't do this for MS-Windows yet, it sends CTRL-K without the
	// modifier.
	تکیا_Modify_Other_کلیدیں = TRUE;
#جے_انت

#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	إذا (!im_xim_isvalid_imactivate())
	    emsg(_(e_قدر_دا_imactivatekey_is_invalid));
#جے_انت
	// When 'cmdheight' was set during startup it may not have taken
	// effect yet.
	إذا (p_ch != 1L)
	    command_اچائی();

	أرجع;
    }

error2:
#جے_تعریف_شدہ FEAT_GUI_X11
    // undo gui_mch_init()
    gui_mch_uninit();
#جے_انت

error:
    gui.وچ_use = false;
    clip_init(FALSE);
}


    فراغ
gui_exit(صحيح rc)
{
    // don't free the fonts, it leads to a BUS error
    // richard@whitequeen.com Jul 99
    free_highlight_fonts();
    gui.وچ_use = false;
    gui_mch_exit(rc);
}

#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_PHOTON)
# define NEED_GUI_UPDATE_SCREEN 1
/*
 * Called when the GUI shell is closed by the user.  If there are no changed
 * files Vim exits, otherwise there will be a dialog to ask the user what to
 * do.
 * When this function returns, Vim should NOT exit!
 */
    فراغ
gui_shell_closed(فراغ)
{
    cmdmod_T	    save_cmdmod = cmdmod;

    إذا (before_quit_autocmds(curwin, TRUE, FALSE))
	أرجع;

    // Only exit when there are no changed files
    exiting = TRUE;
# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    cmdmod.cmod_flags |= CMOD_CONFIRM;
# endif
    // If there are changed buffers, present the user with a dialog if
    // possible, otherwise give an error message.
    إذا (!check_بدلیا_any(FALSE, FALSE))
	getout(0);

    exiting = FALSE;
    cmdmod = save_cmdmod;
    gui_update_screen();	// redraw, window may show changed buffer
}
#جے_انت

/*
 * Set the font.  "font_list" is a comma separated list of font names.  The
 * first font name that works is used.  If none is found, use the default
 * font.
 * If "fontset" is TRUE, the "font_list" is used as one name for the fontset.
 * Return OK when able to set the font.  When it failed FAIL is returned and
 * the fonts are unchanged.
 */
    صحيح
gui_init_font(char_u *font_list, صحيح fontset بن_ورتیا)
{
#تعریف_کرو FONTLEN 320
    char_u	font_name[FONTLEN];
    صحيح		font_list_خالی_تھاواں = FALSE;
    صحيح		ret = FAIL;

    إذا (!gui.وچ_use)
	أرجع FAIL;

    font_name[0] = NUL;
    إذا (*font_list == NUL)
	font_list_خالی_تھاواں = TRUE;
    وإلا
    {
#جے_تعریف_شدہ FEAT_XFONTSET
	// When using a fontset, the whole list of fonts is one name.
	إذا (fontset)
	    ret = gui_mch_init_font(font_list, TRUE);
	وإلا
#جے_انت
	    طالما (*font_list != NUL)
	    {
		// Isolate one comma separated font name.
		(فراغ)copy_option_part(&font_list, font_name, FONTLEN, ",");

		// Careful!!!  The Win32 version of gui_mch_init_font(), when
		// called with "*" will change p_guifont to the selected font
		// name, which frees the old value.  This makes font_list
		// invalid.  Thus when OK is returned here, font_list must no
		// longer be used!
		إذا (gui_mch_init_font(font_name, FALSE) == OK)
		{
#جے_تعریف_شدہ USE_سیٹ_GUIFONTWIDE
		    // If it's a Unicode font, try setting 'guifontwide' to a
		    // similar double-width font.
		    إذا ((p_guifontwide == عدم || *p_guifontwide == NUL)
				&& لڑی_کھوج((حرف *)font_name, "10646") != عدم)
			سیٹ_guifontwide(font_name);
#جے_انت
		    ret = OK;
		    اكسر;
		}
	    }
    }

    إذا (ret != OK
	    && STRCMP(font_list, "*") != 0
	    && (font_list_خالی_تھاواں || gui.norm_font == NOFONT))
    {
	/*
	 * Couldn't load any font in 'font_list', keep the current font if
	 * there is one.  If 'font_list' is empty, or if there is no current
	 * font, tell gui_mch_init_font() to try to find a font we can load.
	 */
	ret = gui_mch_init_font(عدم, FALSE);
    }

    إذا (ret == OK)
    {
#جے_بنا_تعریف FEAT_GUI_GTK
	// Set normal font as current font
# ifdef FEAT_XFONTSET
	إذا (gui.fontset != NOFONTSET)
	    gui_mch_سیٹ_fontset(gui.fontset);
	وإلا
# endif
	    gui_mch_سیٹ_font(gui.norm_font);
#جے_انت
	gui_سیٹ_shellsize(FALSE, TRUE, RESIZE_BOTH);
    }

    أرجع ret;
}

#جے_تعریف_شدہ USE_سیٹ_GUIFONTWIDE
/*
 * Try setting 'guifontwide' to a font twice as wide as "name".
 */
    ساكن فراغ
سیٹ_guifontwide(char_u *name)
{
    صحيح		i = 0;
    char_u	wide_name[FONTLEN + 10]; // room for 2 * width and '*'
    char_u	*wp = عدم;
    char_u	*p;
    GuiFont	font;

    wp = wide_name;
    لكل (p = name; *p != NUL; ++p)
    {
	*wp++ = *p;
	إذا (*p == '-')
	{
	    ++i;
	    إذا (i == 6)		// font type: change "--" to "-*-"
	    {
		إذا (p[1] == '-')
		    *wp++ = '*';
	    }
	    وإلا إذا (i == 12)	// found the width
	    {
		++p;
		i = getdigits(&p);
		إذا (i != 0)
		{
		    // Double the width specification.
		    sprintf((حرف *)wp, "%d%s", i * 2, p);
		    font = gui_mch_get_font(wide_name, FALSE);
		    إذا (font != NOFONT)
		    {
			gui_mch_free_font(gui.wide_font);
			gui.wide_font = font;
			سیٹ_string_option_سدھا((char_u *)"gfw", -1,
						      wide_name, OPT_FREE, 0);
		    }
		}
		اكسر;
	    }
	}
    }
}
#جے_انت

/*
 * Get the font for 'guifontwide'.
 * Return FAIL for an invalid font name.
 */
    صحيح
gui_get_wide_font(فراغ)
{
    GuiFont	font = NOFONT;
    char_u	font_name[FONTLEN];
    char_u	*p;

    إذا (!gui.وچ_use)	    // Can't allocate font yet, assume it's OK.
	أرجع OK;	    // Will give an error message later.

    إذا (p_guifontwide != عدم && *p_guifontwide != NUL)
    {
	لكل (p = p_guifontwide; *p != NUL; )
	{
	    // Isolate one comma separated font name.
	    (فراغ)copy_option_part(&p, font_name, FONTLEN, ",");
	    font = gui_mch_get_font(font_name, FALSE);
	    إذا (font != NOFONT)
		اكسر;
	}
	إذا (font == NOFONT)
	    أرجع FAIL;
    }

    gui_mch_free_font(gui.wide_font);
#جے_تعریف_شدہ FEAT_GUI_GTK
    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    إذا (font != NOFONT && gui.norm_font != NOFONT
			 && pango_font_description_equal(font, gui.norm_font))
    {
	gui.wide_font = NOFONT;
	gui_mch_free_font(font);
    }
    وإلا
#جے_انت
	gui.wide_font = font;
#جے_تعریف_شدہ FEAT_GUI_MSWIN
    gui_mch_wide_font_بدلیا();
#وإلا
    /*
     * TODO: setup wide_bold_font, wide_ital_font and wide_boldital_font to
     * support those fonts for 'guifontwide'.
     */
#جے_انت
    أرجع OK;
}

#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * Set list of ascii characters that combined can create ligature.
 * Store them in char map for quick access from gui_gtk_draw_string.
 */
    فراغ
gui_سیٹ_ligatures(فراغ)
{
    char_u	*p;

    إذا (*p_guiligatures != NUL)
    {
	// check for invalid characters
	لكل (p = p_guiligatures; *p != NUL; ++p)
	    إذا (*p < 32 || *p > 127)
	    {
		emsg(_(e_ascii_کوڈ_not_وچ_range));
		أرجع;
	    }

	// store valid setting into ligatures_map
	CLEAR_میدان(gui.ligatures_map);
	لكل (p = p_guiligatures; *p != NUL; ++p)
	    gui.ligatures_map[*p] = 1;
    }
    وإلا
	CLEAR_میدان(gui.ligatures_map);
}

/*
 * Adjust the columns to undraw for when the cursor is on ligatures.
 */
    ساكن فراغ
gui_adjust_undraw_cursor_for_ligatures(صحيح *startcol, صحيح *endcol)
{
    صحيح off;

    إذا (Screen_سطریں == عدم || *p_guiligatures == NUL)
	أرجع;

    // expand before the cursor for all the chars in gui.ligatures_map
    off = سطر_وتھ[gui.cursor_صف] + *startcol;
    إذا (gui.ligatures_map[Screen_سطریں[off]])
	طالما (*startcol > 0 && gui.ligatures_map[Screen_سطریں[--off]])
	    (*startcol)--;

    // expand after the cursor for all the chars in gui.ligatures_map
    off = سطر_وتھ[gui.cursor_صف] + *endcol;
    إذا (gui.ligatures_map[Screen_سطریں[off]])
	طالما (*endcol < ((صحيح)screen_Columns - 1)
				      && gui.ligatures_map[Screen_سطریں[++off]])
	   (*endcol)++;
}
#جے_انت

    ساكن فراغ
gui_سیٹ_cursor(صحيح صف, صحيح خانہ)
{
    gui.صف = صف;
    gui.خانہ = خانہ;
}

/*
 * gui_check_pos - check if the cursor is on the screen.
 */
    ساكن فراغ
gui_check_pos(فراغ)
{
    إذا (gui.صف >= screen_صفاں)
	gui.صف = screen_صفاں - 1;
    إذا (gui.خانہ >= screen_Columns)
	gui.خانہ = screen_Columns - 1;
    إذا (gui.cursor_صف >= screen_صفاں || gui.cursor_خانہ >= screen_Columns)
	gui.cursor_is_valid = false;
}

/*
 * Redraw the cursor if necessary or when forced.
 * Careful: The contents of ScreenLines[] must match what is on the screen,
 * otherwise this goes wrong.  May need to call out_flush() first.
 */
    فراغ
gui_update_cursor(
    صحيح		force,		 // when TRUE, update even when not moved
    صحيح		clear_selection) // clear selection under cursor
{
    صحيح		cur_چوڑائی = 0;
    صحيح		cur_اچائی = 0;
    صحيح		old_hl_نقاب;
    cursorentry_T *shape;
    صحيح		id;
#جے_تعریف_شدہ FEAT_TERMINAL
    guicolor_T	shape_fg = INVALCOLOR;
    guicolor_T	shape_bg = INVALCOLOR;
#جے_انت
    guicolor_T	cfg, cbg, cc;	// cursor fore-/background color
    صحيح		cattr;		// cursor attributes
    صحيح		attr;
    attrentry_T *aep = عدم;
#إذا (defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)) && !defined(USE_GTK4)
    bool	lig_بایاں = false, lig_دایاں = false;
#جے_انت

    // Don't update the cursor when halfway busy scrolling or the screen size
    // doesn't match 'columns' and 'lines.  ScreenLines[] isn't valid then.
    إذا (!can_update_cursor || screen_Columns != gui.عدد_خانے_گنتی
					       || screen_صفاں != gui.عدد_صفاں)
	أرجع;

    gui_check_pos();

    إذا (gui.cursor_is_valid && !force
		&& gui.صف == gui.cursor_صف && gui.خانہ == gui.cursor_خانہ)
	أرجع;

    gui_undraw_cursor();

    // If a cursor-less sleep is ongoing, leave the cursor invisible
    إذا (cursor_is_sleeping())
	أرجع;

    إذا (gui.صف < 0)
	أرجع;
#جے_تعریف_شدہ HAVE_داخلہ_طریقہ
    إذا (gui.صف != gui.cursor_صف || gui.خانہ != gui.cursor_خانہ)
	im_سیٹ_position(gui.صف, gui.خانہ);
#جے_انت
    gui.cursor_صف = gui.صف;
    gui.cursor_خانہ = gui.خانہ;

    // Only write to the screen after ScreenLines[] has been initialized
    إذا (!screen_cleared || Screen_سطریں == عدم)
	أرجع;

    // Clear the selection if we are about to write over it
    إذا (clear_selection)
	clip_may_clear_selection(gui.صف, gui.صف);
    // Check that the cursor is inside the shell (resizing may have made
    // it invalid)
    إذا (gui.صف >= screen_صفاں || gui.خانہ >= screen_Columns)
	أرجع;

    gui.cursor_is_valid = true;

    /*
     * How the cursor is drawn depends on the current mode.
     * When in a terminal window use the shape/color specified there.
     */
#جے_تعریف_شدہ FEAT_TERMINAL
    إذا (terminal_is_سرگرم())
	shape = رکن_get_cursor_shape(&shape_fg, &shape_bg);
    وإلا
#جے_انت
	shape = &shape_جدول[get_shape_idx(FALSE)];
    إذا (State & منوال_LANGMAP)
	id = shape->id_lm;
    وإلا
	id = shape->id;

    // get the colors and attributes for the cursor.  Default is inverted
    cfg = INVALCOLOR;
    cbg = INVALCOLOR;
    cattr = HL_INVERSE;
    gui_mch_سیٹ_blinking(shape->blinkwait,
	    shape->blinkon,
	    shape->blinkoff);
    إذا (shape->blinkwait == 0 || shape->blinkon == 0
	    || shape->blinkoff == 0)
	gui_mch_stop_blink(FALSE);
#جے_تعریف_شدہ FEAT_TERMINAL
    إذا (shape_bg != INVALCOLOR)
    {
	cattr = 0;
	cfg = shape_fg;
	cbg = shape_bg;
    }
    وإلا
#جے_انت
	إذا (id > 0)
	{
	    cattr = syn_id2colors(id, &cfg, &cbg);
#إذا defined(HAVE_داخلہ_طریقہ)
	    {
		ساكن صحيح iid;
		guicolor_T fg, bg;

		إذا (
# إذا defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
			preedit_get_حالت()
# وإلا
			im_get_حالت()
# endif
		   )
		{
		    iid = syn_name2id((char_u *)"CursorIM");
		    إذا (iid > 0)
		    {
			syn_id2colors(iid, &fg, &bg);
			إذا (bg != INVALCOLOR)
			    cbg = bg;
			إذا (fg != INVALCOLOR)
			    cfg = fg;
		    }
		}
	    }
#جے_انت
	}

    /*
     * Get the attributes for the character under the cursor.
     * When no cursor color was given, use the character color.
     */
    attr = ScreenAttrs[سطر_وتھ[gui.صف] + gui.خانہ];
    إذا (attr > HL_ALL)
	aep = syn_gui_attr2entry(attr);
    إذا (aep != عدم)
    {
	attr = aep->ae_attr;
	إذا (cfg == INVALCOLOR)
	    cfg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.bg_color
		    : aep->ae_u.gui.fg_color);
	إذا (cbg == INVALCOLOR)
	    cbg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.fg_color
		    : aep->ae_u.gui.bg_color);
    }
    إذا (cfg == INVALCOLOR)
	cfg = (attr & HL_INVERSE) ? gui.پٹھ_pixel : gui.norm_pixel;
    إذا (cbg == INVALCOLOR)
	cbg = (attr & HL_INVERSE) ? gui.norm_pixel : gui.پٹھ_pixel;

#جے_تعریف_شدہ FEAT_XIM
    إذا (aep != عدم)
    {
	xim_bg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.fg_color
		: aep->ae_u.gui.bg_color);
	xim_fg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.bg_color
		: aep->ae_u.gui.fg_color);
	إذا (xim_bg_color == INVALCOLOR)
	    xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
		: gui.پٹھ_pixel;
	إذا (xim_fg_color == INVALCOLOR)
	    xim_fg_color = (attr & HL_INVERSE) ? gui.پٹھ_pixel
		: gui.norm_pixel;
    }
    وإلا
    {
	xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
	    : gui.پٹھ_pixel;
	xim_fg_color = (attr & HL_INVERSE) ? gui.پٹھ_pixel
	    : gui.norm_pixel;
    }
#جے_انت

    attr &= ~HL_INVERSE;
    إذا (cattr & HL_INVERSE)
    {
	cc = cbg;
	cbg = cfg;
	cfg = cc;
    }
    cattr &= ~HL_INVERSE;

    /*
     * When we don't have window focus, draw a hollow cursor.
     */
    إذا (!gui.وچ_focus)
    {
#جے_تعریف_شدہ USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, -1, -1);
#وإلا
	gui_mch_draw_hollow_cursor(cbg);
#جے_انت
	أرجع;
    }

#جے_تعریف_شدہ USE_GTK4
    // Make sure that character underneath is drawn again in case it is part of
    // a ligature.
    gui_redraw_بلاک(gui.صف, gui.خانہ, gui.صف, gui.خانہ, GUI_MON_NOCLEAR);
    // gui_redraw_block() will invalidate the cursor
    gui.cursor_is_valid = true;
#جے_انت
#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // If cursor is in the middle of a ligature, then split the ligature at
    // the cursor boundaries by redrawing those cells again.
    لكل (صحيح c = gui.خانہ - 1; c >= 0; c--)
	إذا (!gui.ligatures_map[Screen_سطریں[سطر_وتھ[gui.صف] + c]])
	{
	    إذا (c < gui.خانہ - 1)
	    {
		gui_redraw_بلاک(gui.صف, c + 1, gui.صف, gui.خانہ - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_بایاں = true;
# endif
	    }
	    اكسر;
	}

    لكل (صحيح c = gui.خانہ + 1; c < screen_Columns - 1; c++)
	إذا (!gui.ligatures_map[Screen_سطریں[سطر_وتھ[gui.صف] + c]])
	{
	    إذا (c > gui.خانہ + 1)
	    {
		gui_redraw_بلاک(gui.صف, gui.خانہ + 1, gui.صف, c - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_دایاں = true;
# endif
	    }
	    اكسر;
	}

# ifndef USE_GTK4
    إذا ((lig_بایاں || lig_دایاں) && shape->shape != SHAPE_بلاک)
    {
	// Because the cursor is not drawn with gui_screenchar(), must blit the
	// cell again (with its background), so that old ligature does not
	// remain on screen. Not needed for GtkSnapshot, because char beneath is
	// always rerendered (see above ifdef).
	صحيح old = gui.خانہ;
	gui.highlight_نقاب = ScreenAttrs[سطر_وتھ[gui.صف] + gui.خانہ];
	(فراغ)gui_screenchar(سطر_وتھ[gui.صف] + gui.خانہ,
		GUI_MON_NOCLEAR, (guicolor_T)0, (guicolor_T)0, 0);
	gui.خانہ = old;
    }
# endif
    // gui_redraw_block()/gui_screenchar() may invalidate the cursor, make sure
    // to validate it again.
    gui.cursor_is_valid = true;
#جے_انت

    old_hl_نقاب = gui.highlight_نقاب;
    إذا (shape->shape == SHAPE_بلاک)
    {
#جے_تعریف_شدہ USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, 0, 0);
#وإلا
	/*
	 * Draw the text character with the cursor colors.	Use the
	 * character attributes plus the cursor attributes.
	 */
	gui.highlight_نقاب = (cattr | attr);
	(فراغ)gui_screenchar(سطر_وتھ[gui.صف] + gui.خانہ,
		GUI_MON_IS_CURSOR | GUI_MON_NOCLEAR, cfg, cbg, 0);
#جے_انت
    }
    وإلا
    {
#إذا defined(FEAT_RIGHTLEFT)
	صحيح	    خانہ_off = FALSE;
#جے_انت
	/*
	 * First draw the partial cursor, then overwrite with the text
	 * character, using a transparent background.
	 */
	إذا (shape->shape == SHAPE_VER)
	{
	    cur_اچائی = gui.char_اچائی;
	    cur_چوڑائی = (gui.char_چوڑائی * shape->percentage + 99) / 100;
	}
	وإلا
	{
	    cur_اچائی = (gui.char_اچائی * shape->percentage + 99) / 100;
	    cur_چوڑائی = gui.char_چوڑائی;
	}
	إذا (has_mbyte && (*mb_off2cells)(سطر_وتھ[gui.صف] + gui.خانہ,
		    سطر_وتھ[gui.صف] + screen_Columns) > 1)
	{
	    // Double wide character.
	    إذا (shape->shape != SHAPE_VER)
		cur_چوڑائی += gui.char_چوڑائی;
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	    إذا (CURSOR_BAR_دایاں)
	    {
		// gui.col points to the left half of the character but
		// the vertical line needs to be on the right half.
		// A double-wide horizontal line is also drawn from the
		// right half in gui_mch_draw_part_cursor().
		خانہ_off = TRUE;
		++gui.خانہ;
	    }
#جے_انت
	}
#جے_تعریف_شدہ USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, cur_چوڑائی, cur_اچائی);
#وإلا
	gui_mch_draw_part_cursor(cur_چوڑائی, cur_اچائی, cbg);
#جے_انت
#إذا defined(FEAT_RIGHTLEFT)
	إذا (خانہ_off)
	    --gui.خانہ;
#جے_انت

	// Doesn't seem to work for MSWindows. We call gui_redraw_block() above
	// for GtkSnapshot.
#إذا !defined(FEAT_GUI_MSWIN) && !defined(USE_GTK4)
	gui.highlight_نقاب = ScreenAttrs[سطر_وتھ[gui.صف] + gui.خانہ];
	(فراغ)gui_screenchar(سطر_وتھ[gui.صف] + gui.خانہ,
		GUI_MON_TRS_CURSOR | GUI_MON_NOCLEAR,
		(guicolor_T)0, (guicolor_T)0, 0);
#جے_انت
    }
    gui.highlight_نقاب = old_hl_نقاب;
}

#إذا defined(FEAT_MENU)
    ساكن فراغ
gui_position_menu(فراغ)
{
# إذا !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
    إذا (gui.menu_is_سرگرم && gui.وچ_use)
	gui_mch_سیٹ_menu_pos(0, 0, gui.menu_چوڑائی, gui.menu_اچائی);
# endif
}
#جے_انت

/*
 * Position the various GUI components (text area, menu).  The vertical
 * scrollbars are NOT handled here.  See gui_update_scrollbars().
 */
    ساكن فراغ
gui_position_components(صحيح کل_چوڑائی بن_ورتیا)
{
    صحيح	    متن_area_x;
    صحيح	    متن_area_y;
    صحيح	    متن_area_چوڑائی;
    صحيح	    متن_area_اچائی;

    // avoid that moving components around generates events
    ++hold_gui_events;

    متن_area_x = 0;
    إذا (gui.which_scrollbars[SBAR_بایاں])
	متن_area_x += gui.scrollbar_چوڑائی;

    متن_area_y = 0;
#إذا defined(FEAT_MENU) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    gui.menu_چوڑائی = کل_چوڑائی;
    إذا (gui.menu_is_سرگرم)
	متن_area_y += gui.menu_اچائی;
#جے_انت

#إذا defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
    إذا (gui_has_tabline())
	متن_area_y += gui.tabline_اچائی;
#جے_انت

#إذا defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    إذا (vim_strchr(p_go, GO_TOOLBAR) != عدم)
    {
# إذا defined(FEAT_GUI_HAIKU)
	gui_mch_سیٹ_toolbar_pos(0, متن_area_y,
				gui.menu_چوڑائی, gui.toolbar_اچائی);
# endif
	متن_area_y += gui.toolbar_اچائی;
    }
#جے_انت

#إذا defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_HAIKU)
    gui_mch_سیٹ_tabline_pos(0, متن_area_y,
    gui.menu_چوڑائی, gui.tabline_اچائی);
    إذا (gui_has_tabline())
	متن_area_y += gui.tabline_اچائی;
#جے_انت

    متن_area_چوڑائی = gui.عدد_خانے_گنتی * gui.char_چوڑائی + gui.border_وتھ * 2;
    متن_area_اچائی = gui.عدد_صفاں * gui.char_اچائی + gui.border_وتھ * 2;

    gui_mch_سیٹ_متن_area_pos(متن_area_x,
			      متن_area_y,
			      متن_area_چوڑائی,
			      متن_area_اچائی
#إذا defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
				  + xim_get_حالت_area_اچائی()
#جے_انت
			      );
#جے_تعریف_شدہ FEAT_MENU
    gui_position_menu();
#جے_انت
    إذا (gui.which_scrollbars[SBAR_BOTTOM])
	gui_mch_سیٹ_scrollbar_pos(&gui.bottom_sbar,
				  متن_area_x,
				  متن_area_y + متن_area_اچائی
					+ gui_mch_get_scrollbar_ypadding(),
				  متن_area_چوڑائی,
				  gui.scrollbar_اچائی);
    gui.بایاں_sbar_x = 0;
    gui.دایاں_sbar_x = متن_area_x + متن_area_چوڑائی
					+ gui_mch_get_scrollbar_xpadding();

    --hold_gui_events;
}

/*
 * Get the width of the widgets and decorations to the side of the text area.
 */
    صحيح
gui_get_ادھار_چوڑائی(فراغ)
{
    صحيح	    ادھار_چوڑائی;

    ادھار_چوڑائی = 2 * gui.border_وتھ;
    إذا (gui.which_scrollbars[SBAR_بایاں])
	ادھار_چوڑائی += gui.scrollbar_چوڑائی;
    إذا (gui.which_scrollbars[SBAR_دایاں])
	ادھار_چوڑائی += gui.scrollbar_چوڑائی;
    أرجع ادھار_چوڑائی;
}

/*
 * Get the height of the widgets and decorations above and below the text area.
 */
    صحيح
gui_get_ادھار_اچائی(فراغ)
{
    صحيح	    ادھار_اچائی;

    ادھار_اچائی = 2 * gui.border_وتھ;
    إذا (gui.which_scrollbars[SBAR_BOTTOM])
	ادھار_اچائی += gui.scrollbar_اچائی;
#جے_تعریف_شدہ FEAT_GUI_GTK
    // We can't take the sizes properly into account until anything is
    // realized.  Therefore we recalculate all the values here just before
    // setting the size. (--mdcki)
#وإلا
# ifdef FEAT_MENU
    إذا (gui.menu_is_سرگرم)
	ادھار_اچائی += gui.menu_اچائی;
# endif
# ifdef FEAT_TOOLBAR
    إذا (vim_strchr(p_go, GO_TOOLBAR) != عدم)
	ادھار_اچائی += gui.toolbar_اچائی;
# endif
# إذا defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    إذا (gui_has_tabline())
	ادھار_اچائی += gui.tabline_اچائی;
# endif
# إذا defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    ادھار_اچائی += gui_mch_متن_area_extra_اچائی();
# endif
#جے_انت
    أرجع ادھار_اچائی;
}

/*
 * Should be called after the GUI shell has been resized.  Its arguments are
 * the new width and height of the shell in pixels.
 */
    فراغ
gui_resize_shell(صحيح pixel_چوڑائی, صحيح pixel_اچائی)
{
    ساكن صحيح	busy = FALSE;

    إذا (!gui.shell_created)	    // ignore when still initializing
	أرجع;

    /*
     * Can't resize the screen while it is being redrawn.  Remember the new
     * size and handle it later.
     */
    إذا (updating_screen || busy)
    {
	new_pixel_چوڑائی = pixel_چوڑائی;
	new_pixel_اچائی = pixel_اچائی;
	أرجع;
    }

again:
    new_pixel_چوڑائی = 0;
    new_pixel_اچائی = 0;
    busy = TRUE;

#جے_تعریف_شدہ FEAT_GUI_HAIKU
    vim_جندرا_screen();
#جے_انت

    // Flush pending output before redrawing
    باہرلا_flush();
#إذا defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk_init_decor_اچائی();
#جے_انت

    gui.عدد_خانے_گنتی = (pixel_چوڑائی - gui_get_ادھار_چوڑائی()) / gui.char_چوڑائی;
    gui.عدد_صفاں = (pixel_اچائی - gui_get_ادھار_اچائی()) / gui.char_اچائی;

#جے_تعریف_شدہ USE_GTK4
    gui_gtk4_update_ناپ();
#جے_انت

    gui_position_components(pixel_چوڑائی);
    gui_reset_scroll_region();

#إذا defined(FEAT_GUI_GTK) && defined(USE_GTK4) && !defined(USE_GTK4)
    // We do not resize the draw area via the "resize" signal. This is because
    // when the window is resized, the form widget is the one that is resized,
    // so let that call gui_resize_shell() which will allocate the surface and
    // allocate the drawing area size/position.
    gui_gtk4_resize(pixel_چوڑائی, pixel_اچائی);
#جے_انت

    /*
     * At the "more" and ":confirm" prompt there is no redraw, put the cursor
     * at the last line here (why does it have to be one row too low?).
     */
    إذا (State == منوال_ASKMORE || State == منوال_CONFIRM)
	gui.صف = gui.عدد_صفاں;

    // Only comparing Rows and Columns may be sufficient, but let's stay on
    // the safe side.
    إذا (gui.عدد_صفاں != screen_صفاں || gui.عدد_خانے_گنتی != screen_Columns
	    || gui.عدد_صفاں != صفاں || gui.عدد_خانے_گنتی != Columns || gui.force_redraw)
    {
	shell_resized();
	gui.force_redraw = 0;
    }

#جے_تعریف_شدہ FEAT_GUI_HAIKU
    vim_unlock_screen();
#جے_انت

    gui_update_scrollbars(TRUE);
    gui_update_cursor(FALSE, TRUE);
#إذا defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(pixel_چوڑائی, pixel_اچائی);
#جے_انت
#إذا defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
    xim_سیٹ_حالت_area();
#جے_انت

    busy = FALSE;

    // We may have been called again while redrawing the screen.
    // Need to do it all again with the latest size then.  But only if the size
    // actually changed.
    إذا (new_pixel_اچائی)
    {
	إذا (pixel_چوڑائی == new_pixel_چوڑائی && pixel_اچائی == new_pixel_اچائی)
	{
	    new_pixel_چوڑائی = 0;
	    new_pixel_اچائی = 0;
	}
	وإلا
	{
	    pixel_چوڑائی = new_pixel_چوڑائی;
	    pixel_اچائی = new_pixel_اچائی;
	    اذهب again;
	}
    }
}

/*
 * Check if gui_resize_shell() must be called.
 */
    فراغ
gui_may_resize_shell(فراغ)
{
    إذا (new_pixel_اچائی)
    {
	// careful: gui_resize_shell() may postpone the resize again if we
	// were called indirectly by it
	gui_resize_shell(new_pixel_چوڑائی, new_pixel_اچائی);
    }
}

    صحيح
gui_get_shellsize(فراغ)
{
    صفاں = gui.عدد_صفاں;
    Columns = gui.عدد_خانے_گنتی;
    أرجع OK;
}

/*
 * Set the size of the Vim shell according to Rows and Columns.
 * If "fit_to_display" is TRUE then the size may be reduced to fit the window
 * on the screen.
 * When "mustset" is TRUE the size was set by the user. When FALSE a UI
 * component was added or removed (e.g., a scrollbar).
 */
    فراغ
gui_سیٹ_shellsize(
    صحيح		mustset بن_ورتیا,
    صحيح		fit_منزل_display,
    صحيح		direction)		// RESIZE_HOR, RESIZE_VER
{
    صحيح		ادھار_چوڑائی;
    صحيح		ادھار_اچائی;
    صحيح		چوڑائی;
    صحيح		اچائی;
    صحيح		min_چوڑائی;
    صحيح		min_اچائی;
    صحيح		screen_w;
    صحيح		screen_h;
#جے_تعریف_شدہ FEAT_GUI_GTK
    صحيح		un_maximize = mustset;
    صحيح		did_adjust = 0;
#جے_انت
    صحيح		x = -1, y = -1;

    إذا (!gui.shell_created)
	أرجع;

#إذا defined(MSWIN) || defined(FEAT_GUI_GTK)
    // If not setting to a user specified size and maximized, calculate the
    // number of characters that fit in the maximized window.
    // FIXME: gui_mch_newfont() is called here even when the font hasn't
    // changed at all.  For example, ":set guioptions=k" triggers this path
    // via gui_init_which_components() -> gui_set_shellsize(FALSE, ...).
    // The intent is to keep the window size and recalculate Rows/Columns,
    // which has nothing to do with fonts.  This should be a separate
    // function with a more descriptive name.
    إذا (!mustset && (vim_strchr(p_go, GO_KEEPWINSIZE) != عدم
						       || gui_mch_maximized()))
    {
	gui_mch_newfont();
	أرجع;
    }
#جے_انت

    ادھار_چوڑائی = gui_get_ادھار_چوڑائی();
    ادھار_اچائی = gui_get_ادھار_اچائی();
    إذا (fit_منزل_display)
	// Remember the original window position.
	(فراغ)gui_mch_get_winpos(&x, &y);

    چوڑائی = Columns * gui.char_چوڑائی + ادھار_چوڑائی;
    اچائی = صفاں * gui.char_اچائی + ادھار_اچائی;

    إذا (fit_منزل_display)
    {
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	إذا ((direction & RESIZE_HOR) && چوڑائی > screen_w)
	{
	    Columns = (screen_w - ادھار_چوڑائی) / gui.char_چوڑائی;
	    إذا (Columns < MIN_COLUMNS)
		Columns = MIN_COLUMNS;
	    چوڑائی = Columns * gui.char_چوڑائی + ادھار_چوڑائی;
#جے_تعریف_شدہ FEAT_GUI_GTK
	    ++did_adjust;
#جے_انت
	}
	إذا ((direction & RESIZE_VERT) && اچائی > screen_h)
	{
	    صفاں = (screen_h - ادھار_اچائی) / gui.char_اچائی;
	    check_shellsize();
	    اچائی = صفاں * gui.char_اچائی + ادھار_اچائی;
#جے_تعریف_شدہ FEAT_GUI_GTK
	    ++did_adjust;
#جے_انت
	}
#جے_تعریف_شدہ FEAT_GUI_GTK
	إذا (did_adjust == 2 || (چوڑائی + gui.char_چوڑائی >= screen_w
				     && اچائی + gui.char_اچائی >= screen_h))
	    // don't unmaximize if at maximum size
	    un_maximize = FALSE;
#جے_انت
    }
    حد_بندی_screen_ناپ();
    gui.عدد_خانے_گنتی = Columns;
    gui.عدد_صفاں = صفاں;
#جے_تعریف_شدہ USE_GTK4
    // Keep the drawing area in sync with the size Vim is going to draw.
    gui_gtk4_update_ناپ();
#جے_انت

    min_چوڑائی = ادھار_چوڑائی + MIN_COLUMNS * gui.char_چوڑائی;
    min_اچائی = ادھار_اچائی + MIN_سطریں * gui.char_اچائی;
    min_اچائی += tabline_اچائی() * gui.char_اچائی;

#جے_تعریف_شدہ FEAT_GUI_GTK
    إذا (un_maximize)
    {
	// If the window size is smaller than the screen unmaximize the
	// window, otherwise resizing won't work.
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	إذا ((چوڑائی + gui.char_چوڑائی < screen_w
				   || اچائی + gui.char_اچائی * 2 < screen_h)
		&& gui_mch_maximized())
	    gui_mch_unmaximize();
    }
#جے_انت

    gui_mch_سیٹ_shellsize(چوڑائی, اچائی, min_چوڑائی, min_اچائی,
					  ادھار_چوڑائی, ادھار_اچائی, direction);

    إذا (fit_منزل_display && x >= 0 && y >= 0)
    {
	// Some window managers put the Vim window left of/above the screen.
	// Only change the position if it wasn't already negative before
	// (happens on MS-Windows with a secondary monitor).
	gui_mch_update();
	إذا (gui_mch_get_winpos(&x, &y) == OK && (x < 0 || y < 0))
	    gui_mch_سیٹ_winpos(x < 0 ? 0 : x, y < 0 ? 0 : y);
    }

    gui_position_components(چوڑائی);
    gui_update_scrollbars(TRUE);
    gui_reset_scroll_region();

#إذا defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(چوڑائی, اچائی);
#جے_انت
}

/*
 * Called when Rows and/or Columns has changed.
 */
    فراغ
gui_new_shellsize(فراغ)
{
    gui_reset_scroll_region();
}

/*
 * Make scroll region cover whole screen.
 */
    ساكن فراغ
gui_reset_scroll_region(فراغ)
{
    gui.scroll_region_چوٹی = 0;
    gui.scroll_region_bot = gui.عدد_صفاں - 1;
    gui.scroll_region_بایاں = 0;
    gui.scroll_region_دایاں = gui.عدد_خانے_گنتی - 1;
}

    ساكن فراغ
gui_شروع_highlight(صحيح نقاب)
{
    إذا (نقاب > HL_ALL)		    // highlight code
	gui.highlight_نقاب = نقاب;
    وإلا			    //نقاب
	gui.highlight_نقاب |= نقاب;
}

    فراغ
gui_stop_highlight(صحيح نقاب)
{
    إذا (نقاب > HL_ALL)		    // highlight code
	gui.highlight_نقاب = HL_عام;
    وإلا			    //نقاب
	gui.highlight_نقاب &= ~نقاب;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    فراغ
gui_clear_بلاک(
    صحيح	    row1,
    صحيح	    col1,
    صحيح	    row2,
    صحيح	    col2)
{
    // Clear the selection if we are about to write over it
    clip_may_clear_selection(row1, row2);

    gui_mch_clear_بلاک(row1, col1, row2, col2);

    // Invalidate cursor if it was in this block
    إذا (       gui.cursor_صف >= row1 && gui.cursor_صف <= row2
	    && gui.cursor_خانہ >= col1 && gui.cursor_خانہ <= col2)
	gui.cursor_is_valid = false;
}

/*
 * Write code to update the cursor later.  This avoids the need to flush the
 * output buffer before calling gui_update_cursor().
 */
    فراغ
gui_update_cursor_later(فراغ)
{
    باہرلا_STR("\033|s");
}

    فراغ
gui_write(
    char_u	*s,
    صحيح		len)
{
    char_u	*p;
    صحيح		arg1 = 0, arg2 = 0;
    صحيح		force_cursor = FALSE;	// force cursor update
    صحيح		force_scrollbar = FALSE;
    ساكن win_T	*old_curwin = عدم;

// #define DEBUG_GUI_WRITE
#جے_تعریف_شدہ DEBUG_GUI_WRITE
    {
	صحيح i;
	char_u *str;

	لکھو("gui_write(%d):\n    ", len);
	لكل (i = 0; i < len; i++)
	    إذا (s[i] == ESC)
	    {
		إذا (i != 0)
		    لکھو("\n    ");
		لکھو("<ESC>");
	    }
	    وإلا
	    {
		str = transchar_byte(s[i]);
		إذا (str[0] && str[1])
		    لکھو("<%s>", (حرف *)str);
		وإلا
		    لکھو("%s", (حرف *)str);
	    }
	لکھو("\n");
    }
#جے_انت
    طالما (len)
    {
	إذا (s[0] == ESC && s[1] == '|')
	{
	    p = s + 2;
	    إذا (VIM_ISDIGIT(*p) || (*p == '-' && VIM_ISDIGIT(*(p + 1))))
	    {
		arg1 = getdigits(&p);
		إذا (p > s + len)
		    اكسر;
		إذا (*p == ';')
		{
		    ++p;
		    arg2 = getdigits(&p);
		    إذا (p > s + len)
			اكسر;
		}
	    }
	    بدل (*p)
	    {
		حالة 'C':	// Clear screen
		    clip_scroll_selection(9999);
		    gui_mch_clear_all();
		    gui.cursor_is_valid = false;
		    force_scrollbar = TRUE;
		    اكسر;
		حالة 'M':	// Move cursor
		    gui_سیٹ_cursor(arg1, arg2);
		    اكسر;
		حالة 's':	// force cursor (shape) update
		    force_cursor = TRUE;
		    اكسر;
		حالة 'R':	// Set scroll region
		    إذا (arg1 < arg2)
		    {
			gui.scroll_region_چوٹی = arg1;
			gui.scroll_region_bot = arg2;
		    }
		    وإلا
		    {
			gui.scroll_region_چوٹی = arg2;
			gui.scroll_region_bot = arg1;
		    }
		    اكسر;
		حالة 'V':	// Set vertical scroll region
		    إذا (arg1 < arg2)
		    {
			gui.scroll_region_بایاں = arg1;
			gui.scroll_region_دایاں = arg2;
		    }
		    وإلا
		    {
			gui.scroll_region_بایاں = arg2;
			gui.scroll_region_دایاں = arg1;
		    }
		    اكسر;
		حالة 'd':	// Delete line
		    gui_delete_سطریں(gui.صف, 1);
		    اكسر;
		حالة 'D':	// Delete lines
		    gui_delete_سطریں(gui.صف, arg1);
		    اكسر;
		حالة 'i':	// Insert line
		    gui_داخل_کرو_سطریں(gui.صف, 1);
		    اكسر;
		حالة 'I':	// Insert lines
		    gui_داخل_کرو_سطریں(gui.صف, arg1);
		    اكسر;
		حالة '$':	// Clear to end-of-line
		    gui_clear_بلاک(gui.صف, gui.خانہ, gui.صف,
							    (صحيح)Columns - 1);
		    اكسر;
		حالة 'h':	// Turn on highlighting
		    gui_شروع_highlight(arg1);
		    اكسر;
		حالة 'H':	// Turn off highlighting
		    gui_stop_highlight(arg1);
		    اكسر;
		حالة 'f':	// flash the window (visual bell)
		    gui_mch_flash(arg1 == 0 ? 20 : arg1);
		    اكسر;
		افتراضي:
		    p = s + 1;	// Skip the ESC
		    اكسر;
	    }
	    len -= (صحيح)(++p - s);
	    s = p;
	}
	وإلا إذا (s[0] < 0x20		// Ctrl character
#جے_تعریف_شدہ FEAT_SIGN_ICONS
		&& s[0] != SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
		&& s[0] != MULTISIGN_BYTE
# endif
#جے_انت
		)
	{
	    إذا (s[0] == '\n')		// NL
	    {
		gui.خانہ = 0;
		إذا (gui.صف < gui.scroll_region_bot)
		    gui.صف++;
		وإلا
		    gui_delete_سطریں(gui.scroll_region_چوٹی, 1);
	    }
	    وإلا إذا (s[0] == '\r')	// CR
	    {
		gui.خانہ = 0;
	    }
	    وإلا إذا (s[0] == '\b')	// Backspace
	    {
		إذا (gui.خانہ)
		    --gui.خانہ;
	    }
	    وإلا إذا (s[0] == Ctrl_L)	// cursor-right
	    {
		++gui.خانہ;
	    }
	    وإلا إذا (s[0] == Ctrl_G)	// Beep
	    {
		gui_mch_beep();
	    }
	    // Other Ctrl character: shouldn't happen!

	    --len;	// Skip this char
	    ++s;
	}
	وإلا
	{
	    p = s;
	    طالما (len > 0 && (
			*p >= 0x20
#جے_تعریف_شدہ FEAT_SIGN_ICONS
			|| *p == SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
			|| *p == MULTISIGN_BYTE
# endif
#جے_انت
			))
	    {
		len--;
		p++;
	    }
	    gui_outstr(s, (صحيح)(p - s));
	    s = p;
	}
    }

    // Postponed update of the cursor (won't work if "can_update_cursor" isn't
    // set).
    إذا (force_cursor)
	gui_update_cursor(TRUE, TRUE);

    // When switching to another window the dragging must have stopped.
    // Required for GTK, dragged_sb isn't reset.
    إذا (old_curwin != curwin)
	gui.dragged_sb = SBAR_NONE;

    // Update the scrollbars after clearing the screen or when switched
    // to another window.
    // Update the horizontal scrollbar always, it's difficult to check all
    // situations where it might change.
    إذا (force_scrollbar || old_curwin != curwin)
	gui_update_scrollbars(force_scrollbar);
    وإلا
	gui_update_horiz_scrollbar(FALSE);
    old_curwin = curwin;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#إذا defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#جے_انت

    gui_may_flush();		    // In case vim decides to take a nap
}

/*
 * When ScreenLines[] is invalid, updating the cursor should not be done, it
 * produces wrong results.  Call gui_dont_update_cursor() before that code and
 * gui_can_update_cursor() afterwards.
 */
    فراغ
gui_dont_update_cursor(صحيح undraw)
{
    إذا (!gui.وچ_use)
	أرجع;

    // Undraw the cursor now, we probably can't do it after the change.
    إذا (undraw)
	gui_undraw_cursor();
    can_update_cursor = FALSE;
}

    فراغ
gui_can_update_cursor(فراغ)
{
    can_update_cursor = TRUE;
    // No need to update the cursor right now, there is always more output
    // after scrolling.
}

/*
 * Disable issuing gui_mch_flush().
 */
    فراغ
gui_disable_flush(فراغ)
{
    ++disable_flush;
}

/*
 * Enable issuing gui_mch_flush().
 */
    فراغ
gui_enable_flush(فراغ)
{
    --disable_flush;
}

/*
 * Issue gui_mch_flush() if it is not disabled.
 */
    فراغ
gui_may_flush(فراغ)
{
    إذا (disable_flush == 0)
	gui_mch_flush();
}

    ساكن فراغ
gui_outstr(char_u *s, صحيح len)
{
    صحيح	    this_len;
    صحيح	    cells;

    إذا (len == 0)
	أرجع;

    إذا (len < 0)
	len = (صحيح)STRLEN(s);

    طالما (len > 0)
    {
	إذا (has_mbyte)
	{
	    // Find out how many chars fit in the current line.
	    cells = 0;
	    لكل (this_len = 0; this_len < len; )
	    {
		cells += (*mb_ptr2cells)(s + this_len);
		إذا (gui.خانہ + cells > Columns)
		    اكسر;
		this_len += (*mb_ptr2len)(s + this_len);
	    }
	    إذا (this_len > len)
		this_len = len;	    // don't include following composing char
	}
	وإلا إذا (gui.خانہ + len > Columns)
	    this_len = Columns - gui.خانہ;
	وإلا
	    this_len = len;

	(فراغ)gui_outstr_nowrap(s, this_len,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	s += this_len;
	len -= this_len;
	// fill up for a double-width char that doesn't fit.
	إذا (len > 0 && gui.خانہ < Columns)
	    (فراغ)gui_outstr_nowrap((char_u *)" ", 1,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	// The cursor may wrap to the next line.
	إذا (gui.خانہ >= Columns)
	{
	    gui.خانہ = 0;
	    gui.صف++;
	}
    }
}

#جے_بنا_تعریف USE_GTK4
/*
 * Output one character (may be one or two display cells).
 * Caller must check for valid "off".
 * Returns FAIL or OK, just like gui_outstr_nowrap().
 */
    ساكن صحيح
gui_screenchar(
    صحيح		off,	    // Offset from start of screen
    صحيح		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    صحيح		پٹھ)	    // backup this many chars when using bold trick
{
    char_u	buf[MB_MAXBYTES + 1];

    // Don't draw right half of a double-width UTF-8 char. "cannot happen"
    إذا (enc_utf8 && Screen_سطریں[off] == 0)
	أرجع OK;

    إذا (enc_utf8 && Screen_سطریں_UC[off] != 0)
	// Draw UTF-8 multi-byte character.
	أرجع gui_outstr_nowrap(buf, utfc_char2bytes(off, buf),
							 flags, fg, bg, پٹھ);

    إذا (enc_dbcs == DBCS_JPNU && Screen_سطریں[off] == 0x8e)
    {
	buf[0] = Screen_سطریں[off];
	buf[1] = ScreenLines2[off];
	أرجع gui_outstr_nowrap(buf, 2, flags, fg, bg, پٹھ);
    }

    // Draw non-multi-byte character or DBCS character.
    أرجع gui_outstr_nowrap(Screen_سطریں + off,
	    enc_dbcs ? (*mb_ptr2len)(Screen_سطریں + off) : 1,
							 flags, fg, bg, پٹھ);
}
#جے_انت

#جے_تعریف_شدہ FEAT_GUI_GTK
/*
 * Output the string at the given screen position.  This is used in place
 * of gui_screenchar() where possible because Pango needs as much context
 * as possible to work nicely.  It's a lot faster as well.
 */
    ساكن صحيح
gui_screenstr(
    صحيح		off,	    // Offset from start of screen
    صحيح		len,	    // string length in screen cells
    صحيح		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    صحيح		پٹھ)	    // backup this many chars when using bold trick
{
    char_u  *buf;
    صحيح	    outlen = 0;
    صحيح	    i;
    صحيح	    retval;

    إذا (len <= 0) // "cannot happen"?
	أرجع OK;

    إذا (enc_utf8)
    {
	buf = alloc(len * MB_MAXBYTES + 1);
	إذا (buf == عدم)
	    أرجع OK; // not much we could do here...

	لكل (i = off; i < off + len; ++i)
	{
	    إذا (Screen_سطریں[i] == 0)
		استمر; // skip second half of double-width char

	    إذا (Screen_سطریں_UC[i] == 0)
		buf[outlen++] = Screen_سطریں[i];
	    وإلا
		outlen += utfc_char2bytes(i, buf + outlen);
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, پٹھ);
	vim_free(buf);

	أرجع retval;
    }
    وإلا إذا (enc_dbcs == DBCS_JPNU)
    {
	buf = alloc(len * 2 + 1);
	إذا (buf == عدم)
	    أرجع OK; // not much we could do here...

	لكل (i = off; i < off + len; ++i)
	{
	    buf[outlen++] = Screen_سطریں[i];

	    // handle double-byte single-width char
	    إذا (Screen_سطریں[i] == 0x8e)
		buf[outlen++] = ScreenLines2[i];
	    وإلا إذا (MB_BYTE2LEN(Screen_سطریں[i]) == 2)
		buf[outlen++] = Screen_سطریں[++i];
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, پٹھ);
	vim_free(buf);

	أرجع retval;
    }
    وإلا
    {
	أرجع gui_outstr_nowrap(&Screen_سطریں[off], len,
				 flags, fg, bg, پٹھ);
    }
}
#جے_انت // FEAT_GUI_GTK

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
    ساكن صحيح
gui_outstr_nowrap(
    char_u	*s,
    صحيح		len,
    صحيح		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    صحيح		پٹھ)	    // backup this many chars when using bold trick
{
    long_u	highlight_نقاب;
    long_u	hl_نقاب_todo;
    guicolor_T	fg_color;
    guicolor_T	bg_color;
    guicolor_T	sp_color;
#إذا !defined(FEAT_GUI_GTK)
    GuiFont	font = NOFONT;
    GuiFont	wide_font = NOFONT;
# ifdef FEAT_XFONTSET
    GuiFontset	fontset = NOFONTSET;
# endif
#جے_انت
    attrentry_T	*aep = عدم;
    صحيح		draw_flags;
    صحيح		خانہ = gui.خانہ;
#جے_تعریف_شدہ FEAT_SIGN_ICONS
    صحيح		draw_sign = FALSE;
    صحيح		signcol = 0;
    char_u	extra[18];
# ifdef FEAT_NETBEANS_INTG
    صحيح		multi_sign = FALSE;
# endif
#جے_انت

    إذا (len < 0)
	len = (صحيح)STRLEN(s);
    إذا (len == 0)
	أرجع OK;

#جے_تعریف_شدہ FEAT_SIGN_ICONS
    إذا (*s == SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
	  || *s == MULTISIGN_BYTE
# endif
       )
    {
	size_t	slen = 2;		    // 2 spaces by default

# ifdef FEAT_NETBEANS_INTG
	إذا (*s == MULTISIGN_BYTE)
	    multi_sign = TRUE;
# endif
	// draw spaces instead
	إذا (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) == 'u' &&
		(curwin->w_p_nu || curwin->w_p_rnu))
	{
	    صحيح	n = نمبر_چوڑائی(curwin);

	    slen = MIN(n, (صحيح)حجم(extra) - 1);
	}
	vim_memset(extra, ' ', slen);
	extra[slen] = NUL;
	s = extra;
	إذا (len == 1 && خانہ > 0)
	    --خانہ;
	len = (صحيح)slen;
	إذا (len > 2)
	    // right align sign icon in the number column
	    signcol = خانہ + len - 3;
	وإلا
	    signcol = خانہ;
	draw_sign = TRUE;
	highlight_نقاب = 0;
    }
    وإلا
#جے_انت
    إذا (gui.highlight_نقاب > HL_ALL)
    {
	aep = syn_gui_attr2entry(gui.highlight_نقاب);
	إذا (aep == عدم)	    // highlighting not set
	    highlight_نقاب = 0;
	وإلا
	    highlight_نقاب = aep->ae_attr;
    }
    وإلا
	highlight_نقاب = gui.highlight_نقاب;
    hl_نقاب_todo = highlight_نقاب;

#إذا !defined(FEAT_GUI_GTK)
    // Set the font
    إذا (aep != عدم && aep->ae_u.gui.font != NOFONT)
	font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
    وإلا إذا (aep != عدم && aep->ae_u.gui.fontset != NOFONTSET)
	fontset = aep->ae_u.gui.fontset;
# endif
    وإلا
    {
# ifdef FEAT_XFONTSET
	إذا (gui.fontset != NOFONTSET)
	    fontset = gui.fontset;
	وإلا
# endif
	    إذا (hl_نقاب_todo & (HL_BOLD | HL_STANDOUT))
	{
	    إذا ((hl_نقاب_todo & HL_ITALIC) && gui.boldital_font != NOFONT)
	    {
		font = gui.boldital_font;
		hl_نقاب_todo &= ~(HL_BOLD | HL_STANDOUT | HL_ITALIC);
	    }
	    وإلا إذا (gui.bold_font != NOFONT)
	    {
		font = gui.bold_font;
		hl_نقاب_todo &= ~(HL_BOLD | HL_STANDOUT);
	    }
	    وإلا
		font = gui.norm_font;
	}
	وإلا إذا ((hl_نقاب_todo & HL_ITALIC) && gui.ital_font != NOFONT)
	{
	    font = gui.ital_font;
	    hl_نقاب_todo &= ~HL_ITALIC;
	}
	وإلا
	    font = gui.norm_font;

	/*
	 * Choose correct wide_font by font.  wide_font should be set with font
	 * at same time in above block.  But it will make many "ifdef" nasty
	 * blocks.  So we do it here.
	 */
	إذا (font == gui.boldital_font && gui.wide_boldital_font)
	    wide_font = gui.wide_boldital_font;
	وإلا إذا (font == gui.bold_font && gui.wide_bold_font)
	    wide_font = gui.wide_bold_font;
	وإلا إذا (font == gui.ital_font && gui.wide_ital_font)
	    wide_font = gui.wide_ital_font;
	وإلا إذا (font == gui.norm_font && gui.wide_font)
	    wide_font = gui.wide_font;
    }
# ifdef FEAT_XFONTSET
    إذا (fontset != NOFONTSET)
	gui_mch_سیٹ_fontset(fontset);
    وإلا
# endif
	gui_mch_سیٹ_font(font);
#جے_انت

    draw_flags = 0;

    // Set the color
    bg_color = gui.پٹھ_pixel;
    إذا ((flags & GUI_MON_IS_CURSOR) && gui.وچ_focus)
    {
	draw_flags |= DRAW_CURSOR;
	fg_color = fg;
	bg_color = bg;
	sp_color = fg;
    }
    وإلا إذا (aep != عدم)
    {
	fg_color = aep->ae_u.gui.fg_color;
	إذا (fg_color == INVALCOLOR)
	    fg_color = gui.norm_pixel;
	bg_color = aep->ae_u.gui.bg_color;
	إذا (bg_color == INVALCOLOR)
	    bg_color = gui.پٹھ_pixel;
	sp_color = aep->ae_u.gui.sp_color;
	إذا (sp_color == INVALCOLOR)
	    sp_color = fg_color;
    }
    وإلا
    {
	fg_color = gui.norm_pixel;
	sp_color = fg_color;
    }

    إذا (highlight_نقاب & (HL_INVERSE | HL_STANDOUT))
    {
	gui_mch_سیٹ_fg_color(bg_color);
	gui_mch_سیٹ_bg_color(fg_color);
    }
    وإلا
    {
	gui_mch_سیٹ_fg_color(fg_color);
	gui_mch_سیٹ_bg_color(bg_color);
    }
    gui_mch_سیٹ_sp_color(sp_color);

    // Clear the selection if we are about to write over it
    إذا (!(flags & GUI_MON_NOCLEAR))
	clip_may_clear_selection(gui.صف, gui.صف);


    // If there's no bold font, then fake it
    إذا (hl_نقاب_todo & (HL_BOLD | HL_STANDOUT))
	draw_flags |= DRAW_BOLD;

    /*
     * When drawing bold or italic characters the spill-over from the left
     * neighbor may be destroyed.  Let the caller backup to start redrawing
     * just after a blank.
     */
    إذا (پٹھ != 0 && ((draw_flags & DRAW_BOLD) || (highlight_نقاب & HL_ITALIC)))
	أرجع FAIL;

#إذا defined(FEAT_GUI_GTK)
    // If there's no italic font, then fake it.
    // For GTK2, we don't need a different font for italic style.
    إذا (hl_نقاب_todo & HL_ITALIC)
	draw_flags |= DRAW_ITALIC;

    // Do we underline the text?
    إذا (hl_نقاب_todo & HL_UNDERLINE)
	draw_flags |= DRAW_UNDERL;

#وإلا
    // Do we underline the text?
    إذا ((hl_نقاب_todo & HL_UNDERLINE) || (hl_نقاب_todo & HL_ITALIC))
	draw_flags |= DRAW_UNDERL;
#جے_انت
    // Do we undercurl the text?
    إذا (hl_نقاب_todo & HL_UNDERCURL)
	draw_flags |= DRAW_UNDERC;

    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

    // Do we strikethrough the text?
    إذا (hl_نقاب_todo & HL_STRIKETHROUGH)
	draw_flags |= DRAW_STRIKE;

    // Do we draw transparently?
    إذا (flags & GUI_MON_TRS_CURSOR)
	draw_flags |= DRAW_TRANSP;

    /*
     * Draw the text.
     */
#جے_تعریف_شدہ FEAT_GUI_GTK
    // The value returned is the length in display cells
    len = gui_gtk_draw_string(gui.صف, خانہ, s, len, draw_flags);
#وإلا
    إذا (enc_utf8)
    {
	صحيح	شروع;		// index of bytes to be drawn
	صحيح	cells;		// cellwidth of bytes to be drawn
	صحيح	thislen;	// length of bytes to be drawn
	صحيح	cn;		// cellwidth of current char
	صحيح	i;		// index of current char
	صحيح	c;		// current char value
	صحيح	cl;		// byte length of current char
	صحيح	comping;	// current char is composing
	صحيح	scol = خانہ;	// screen column
	صحيح	curr_wide = FALSE;  // use 'guifontwide'
	صحيح	prev_wide = FALSE;
	صحيح	wide_بدلیا;
# ifdef MSWIN
	صحيح	sep_comp = FALSE;   // Don't separate composing chars.
# وإلا
	صحيح	sep_comp = TRUE;    // Separate composing chars.
# endif

	// Break the string at a composing character, it has to be drawn on
	// top of the previous character.
	شروع = 0;
	cells = 0;
	لكل (i = 0; i < len; i += cl)
	{
	    c = utf_ptr2char(s + i);
	    cn = utf_char2cells(c);
	    comping = utf_iscomposing(c);
	    إذا (!comping)	// count cells from non-composing chars
		cells += cn;
	    إذا (!comping || sep_comp)
	    {
		إذا (cn > 1
# ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
# endif
			&& wide_font != NOFONT)
		    curr_wide = TRUE;
		وإلا
		    curr_wide = FALSE;
	    }
	    cl = utf_ptr2len(s + i);
	    إذا (cl == 0)	// hit end of string
		len = i + cl;	// len must be wrong "cannot happen"

	    wide_بدلیا = curr_wide != prev_wide;

	    // Print the string so far if it's the last character or there is
	    // a composing character.
	    إذا (i + cl >= len || (comping && sep_comp && i > شروع)
		    || wide_بدلیا
# إذا defined(FEAT_GUI_X11)
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
		إذا ((comping && sep_comp) || wide_بدلیا)
		    thislen = i - شروع;
		وإلا
		    thislen = i - شروع + cl;
		إذا (thislen > 0)
		{
		    إذا (prev_wide)
			gui_mch_سیٹ_font(wide_font);
		    gui_mch_draw_string(gui.صف, scol, s + شروع, thislen,
								  draw_flags);
		    إذا (prev_wide)
			gui_mch_سیٹ_font(font);
		    شروع += thislen;
		}
		scol += cells;
		cells = 0;
		// Adjust to not draw a character which width is changed
		// against with last one.
		إذا (wide_بدلیا && !(comping && sep_comp))
		{
		    scol -= cn;
		    cl = 0;
		}

# إذا defined(FEAT_GUI_X11)
		// No fontset: draw a space to fill the gap after a wide char
		//
		إذا (cn > 1 && (draw_flags & DRAW_TRANSP) == 0
#  ifdef FEAT_XFONTSET
			&& fontset == NOFONTSET
#  endif
			&& !wide_بدلیا)
		    gui_mch_draw_string(gui.صف, scol - 1, (char_u *)" ",
							       1, draw_flags);
# endif
	    }
	    // Draw a composing char on top of the previous char.
	    إذا (comping && sep_comp)
	    {
# إذا defined(__APPLE_CC__) && نشانہ_API_MAC_CARBON
		// Carbon ATSUI autodraws composing char over previous char
		gui_mch_draw_string(gui.صف, scol, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# وإلا
		gui_mch_draw_string(gui.صف, scol - cn, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# endif
		شروع = i + cl;
	    }
	    prev_wide = curr_wide;
	}
	// The stuff below assumes "len" is the length in screen columns.
	len = scol - خانہ;
    }
    وإلا
    {
	gui_mch_draw_string(gui.صف, خانہ, s, len, draw_flags);
	إذا (enc_dbcs == DBCS_JPNU)
	{
	    // Get the length in display cells, this can be different from the
	    // number of bytes for "euc-jp".
	    len = mb_string2cells(s, len);
	}
    }
#جے_انت // !FEAT_GUI_GTK

    إذا (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
	gui.خانہ = خانہ + len;

    // May need to invert it when it's part of the selection.
    إذا (flags & GUI_MON_NOCLEAR)
	clip_may_redraw_selection(gui.صف, خانہ, len);

    إذا (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
    {
	// Invalidate the old physical cursor position if we wrote over it
	إذا (gui.cursor_صف == gui.صف
		&& gui.cursor_خانہ >= خانہ
		&& gui.cursor_خانہ < خانہ + len)
	    gui.cursor_is_valid = false;
    }

#جے_تعریف_شدہ FEAT_SIGN_ICONS
    إذا (draw_sign)
	// Draw the sign on top of the spaces.
	gui_mch_drawsign(gui.صف, signcol, gui.highlight_نقاب);
# إذا defined(FEAT_NETBEANS_INTG) && (defined(FEAT_GUI_X11) \
	|| defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN))
    إذا (multi_sign)
	netbeans_draw_multisign_indicator(gui.صف);
# endif
#جے_انت

    أرجع OK;
}

/*
 * Undraw the cursor.  This actually redraws the character at the cursor
 * position, plus some more characters when needed.
 */
    فراغ
gui_undraw_cursor(فراغ)
{
    إذا (!gui.cursor_is_valid)
	أرجع;

    // Always redraw the character just before if there is one, because
    // with some fonts and characters there can be a one pixel overlap.
    صحيح startcol = gui.cursor_خانہ > 0 ? gui.cursor_خانہ - 1 : gui.cursor_خانہ;
    صحيح endcol = gui.cursor_خانہ;

#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    gui_adjust_undraw_cursor_for_ligatures(&startcol, &endcol);
#جے_انت
    gui_redraw_بلاک(gui.cursor_صف, startcol,
	    gui.cursor_صف, endcol, GUI_MON_NOCLEAR);

    // Cursor_is_valid is reset when the cursor is undrawn, also reset it
    // here in case it wasn't needed to undraw it.
    gui.cursor_is_valid = false;
}

    فراغ
gui_redraw(
    صحيح		x,
    صحيح		y,
    صحيح		w,
    صحيح		h)
{
    صحيح		row1, col1, row2, col2;

    row1 = Y_2_صف(y);
    col1 = X_2_خانہ(x);
    row2 = Y_2_صف(y + h - 1);
    col2 = X_2_خانہ(x + w - 1);

    gui_redraw_بلاک(row1, col1, row2, col2, GUI_MON_NOCLEAR);

    /*
     * We may need to redraw the cursor, but don't take it upon us to change
     * its location after a scroll.
     * (maybe be more strict even and test col too?)
     * These things may be outside the update/clipping region and reality may
     * not reflect Vims internal ideas if these operations are clipped away.
     */
    إذا (gui.صف == gui.cursor_صف)
	gui_update_cursor(TRUE, TRUE);
}

/*
 * Draw a rectangular block of characters, from row1 to row2 (inclusive) and
 * from col1 to col2 (inclusive).
 */
    فراغ
gui_redraw_بلاک(
    صحيح		row1,
    صحيح		col1,
    صحيح		row2,
    صحيح		col2,
    صحيح		flags)	// flags for gui_outstr_nowrap()
{
    صحيح		old_صف, old_خانہ;
    long_u	old_hl_نقاب;
    صحيح		off;
    sattr_T	پہلا_attr;
    صحيح		idx, len;
    صحيح		پٹھ, nback;
    صحيح		orig_col1, orig_col2;

    // Don't try to update when ScreenLines is not valid
    إذا (!screen_cleared || Screen_سطریں == عدم)
	أرجع;

    // Don't try to draw outside the shell!
    // Check everything, strange values may be caused by a big border width
    col1 = check_خانہ(col1);
    col2 = check_خانہ(col2);
    row1 = check_صف(row1);
    row2 = check_صف(row2);

    // Remember where our cursor was
    old_صف = gui.صف;
    old_خانہ = gui.خانہ;
    old_hl_نقاب = gui.highlight_نقاب;
    orig_col1 = col1;
    orig_col2 = col2;

    لكل (gui.صف = row1; gui.صف <= row2; gui.صف++)
    {
	// When only half of a double-wide character is in the block, include
	// the other half.
	col1 = orig_col1;
	col2 = orig_col2;
	off = سطر_وتھ[gui.صف];
	إذا (enc_dbcs != 0)
	{
	    إذا (col1 > 0)
		col1 -= dbcs_screen_سر_off(Screen_سطریں + off,
						    Screen_سطریں + off + col1);
	    col2 += dbcs_screen_پوچھ_off(Screen_سطریں + off,
						    Screen_سطریں + off + col2);
	}
	وإلا إذا (enc_utf8)
	{
	    إذا (Screen_سطریں[off + col1] == 0)
	    {
		إذا (col1 > 0)
		    --col1;
		وإلا
		    // FIXME: how can the first character ever be zero?
		    siemsg("NUL in ScreenLines in row %ld", (لما)gui.صف);
	    }
#جے_تعریف_شدہ FEAT_GUI_GTK
	    إذا (col2 + 1 < Columns && Screen_سطریں[off + col2 + 1] == 0)
		++col2;
#جے_انت
	}
	gui.خانہ = col1;
	off = سطر_وتھ[gui.صف] + gui.خانہ;
	len = col2 - col1 + 1;

	// Find how many chars back this highlighting starts, or where a space
	// is.  Needed for when the bold trick is used
	لكل (پٹھ = 0; پٹھ < col1; ++پٹھ)
	    إذا (ScreenAttrs[off - 1 - پٹھ] != ScreenAttrs[off]
		    || Screen_سطریں[off - 1 - پٹھ] == ' ')
		اكسر;

	// Break it up in strings of characters with the same attributes.
	// Print UTF-8 characters individually.
	طالما (len > 0)
	{
	    پہلا_attr = ScreenAttrs[off];
	    gui.highlight_نقاب = پہلا_attr;
#إذا !defined(FEAT_GUI_GTK)
	    إذا (enc_utf8 && Screen_سطریں_UC[off] != 0)
	    {
		// output multi-byte character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, پٹھ);
		إذا (gui.خانہ < Columns && Screen_سطریں[off + 1] == 0)
		    idx = 2;
		وإلا
		    idx = 1;
	    }
	    وإلا إذا (enc_dbcs == DBCS_JPNU && Screen_سطریں[off] == 0x8e)
	    {
		// output double-byte, single-width character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, پٹھ);
		idx = 1;
	    }
	    وإلا
#جے_انت
	    {
#جے_تعریف_شدہ FEAT_GUI_GTK
		لكل (idx = 0; idx < len; ++idx)
		{
		    إذا (enc_utf8 && Screen_سطریں[off + idx] == 0)
			استمر; // skip second half of double-width char
		    إذا (ScreenAttrs[off + idx] != پہلا_attr)
			اكسر;
		}
		// gui_screenstr() takes care of multibyte chars
		nback = gui_screenstr(off, idx, flags,
				      (guicolor_T)0, (guicolor_T)0, پٹھ);
#وإلا
		لكل (idx = 0; idx < len && ScreenAttrs[off + idx] == پہلا_attr;
									idx++)
		{
		    // Stop at a multi-byte Unicode character.
		    إذا (enc_utf8 && Screen_سطریں_UC[off + idx] != 0)
			اكسر;
		    إذا (enc_dbcs == DBCS_JPNU)
		    {
			// Stop at a double-byte single-width char.
			إذا (Screen_سطریں[off + idx] == 0x8e)
			    اكسر;
			إذا (len > 1 && (*mb_ptr2len)(Screen_سطریں
							    + off + idx) == 2)
			    ++idx;  // skip second byte of double-byte char
		    }
		}
		nback = gui_outstr_nowrap(Screen_سطریں + off, idx, flags,
					  (guicolor_T)0, (guicolor_T)0, پٹھ);
#جے_انت
	    }
	    إذا (nback == FAIL)
	    {
		// Must back up to start drawing where a bold or italic word
		// starts.
		off -= پٹھ;
		len += پٹھ;
		gui.خانہ -= پٹھ;
	    }
	    وإلا
	    {
		off += idx;
		len -= idx;
	    }
	    پٹھ = 0;
	}
    }

    // Put the cursor back where it was
    gui.صف = old_صف;
    gui.خانہ = old_خانہ;
    gui.highlight_نقاب = (صحيح)old_hl_نقاب;
}

    ساكن فراغ
gui_delete_سطریں(صحيح صف, صحيح گنتی)
{
    إذا (گنتی <= 0)
	أرجع;

    إذا (صف + گنتی > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_بلاک(صف, gui.scroll_region_بایاں,
			      gui.scroll_region_bot, gui.scroll_region_دایاں);
    وإلا
    {
	gui_mch_delete_سطریں(صف, گنتی);

	// If the cursor was in the deleted lines it's now gone.  If the
	// cursor was in the scrolled lines adjust its position.
	إذا (gui.cursor_صف >= صف
		&& gui.cursor_خانہ >= gui.scroll_region_بایاں
		&& gui.cursor_خانہ <= gui.scroll_region_دایاں)
	{
	    إذا (gui.cursor_صف < صف + گنتی)
		gui.cursor_is_valid = false;
	    وإلا إذا (gui.cursor_صف <= gui.scroll_region_bot)
		gui.cursor_صف -= گنتی;
	}
    }
}

    ساكن فراغ
gui_داخل_کرو_سطریں(صحيح صف, صحيح گنتی)
{
    إذا (گنتی <= 0)
	أرجع;

    إذا (صف + گنتی > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_بلاک(صف, gui.scroll_region_بایاں,
			      gui.scroll_region_bot, gui.scroll_region_دایاں);
    وإلا
    {
	gui_mch_داخل_کرو_سطریں(صف, گنتی);

	إذا (gui.cursor_صف >= gui.صف
		&& gui.cursor_خانہ >= gui.scroll_region_بایاں
		&& gui.cursor_خانہ <= gui.scroll_region_دایاں)
	{
	    إذا (gui.cursor_صف <= gui.scroll_region_bot - گنتی)
		gui.cursor_صف += گنتی;
	    وإلا إذا (gui.cursor_صف <= gui.scroll_region_bot)
		gui.cursor_is_valid = false;
	}
    }
}

#جے_تعریف_شدہ FEAT_TIMERS
/*
 * Passed to ui_wait_for_chars_or_timer(), ignoring extra arguments.
 */
    ساكن صحيح
gui_wait_for_chars_3(
    لما wtime,
    صحيح *interrupted بن_ورتیا,
    صحيح ignore_داخلہ بن_ورتیا)
{
    أرجع gui_mch_wait_for_chars(wtime);
}
#جے_انت

/*
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    ساكن صحيح
gui_wait_for_chars_or_timer(
	لما wtime,
	صحيح *interrupted بن_ورتیا,
	صحيح ignore_داخلہ بن_ورتیا)
{
#جے_تعریف_شدہ FEAT_TIMERS
    أرجع ui_wait_for_chars_or_timer(wtime, gui_wait_for_chars_3,
						    interrupted, ignore_داخلہ);
#وإلا
    أرجع gui_mch_wait_for_chars(wtime);
#جے_انت
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
    ساكن صحيح
gui_wait_for_chars_buf(
    char_u	*buf,
    صحيح		maxlen,
    لما	wtime,	    // don't use "time", MIPS cannot handle it
    صحيح		tb_change_cnt)
{
    صحيح	    retval;
    صحيح	    keep_blinking; // Guard against restarting blink cycle on CursorHold

#جے_تعریف_شدہ FEAT_MENU
    // If we're going to wait a bit, update the menus and mouse shape for the
    // current State.
    إذا (wtime != 0)
	gui_update_menus(0);
#جے_انت

    gui_mch_update();
    إذا (داخلہ_دستیاب())	// Got char, return immediately
    {
	إذا (gui_mch_is_blinking())
	    gui_mch_stop_blink(TRUE);
	إذا (buf != عدم && !typebuf_بدلیا(tb_change_cnt))
	    أرجع read_from_داخلہ_buf(buf, (لما)maxlen);
	أرجع 0;
    }
    إذا (wtime == 0)		// Don't wait for char
	أرجع FAIL;

    // Before waiting, flush any output to the screen.
    gui_mch_flush();

    // Blink while waiting for a character.
    إذا (!gui_mch_is_blinking())
	gui_mch_شروع_blink();

    // Common function to loop until "wtime" is met, while handling timers and
    // other callbacks.
    retval = inchar_loop(buf, maxlen, wtime, tb_change_cnt,
			 gui_wait_for_chars_or_timer, عدم);

    // Keep blinking when CursorHold wakes the input loop. (See PR #21115)
    keep_blinking = retval == 3 && buf != عدم
	&& buf[0] == K_SPECIAL && buf[1] == KS_EXTRA
	&& buf[2] == (صحيح)KE_CURSORHOLD;

    إذا (!keep_blinking)
	gui_mch_stop_blink(TRUE);

    أرجع retval;
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
    صحيح
gui_wait_for_chars(لما wtime, صحيح tb_change_cnt)
{
    أرجع gui_wait_for_chars_buf(عدم, 0, wtime, tb_change_cnt);
}

/*
 * Equivalent of mch_inchar() for the GUI.
 */
    صحيح
gui_inchar(
    char_u  *buf,
    صحيح	    maxlen,
    لما    wtime,		// milliseconds
    صحيح	    tb_change_cnt)
{
    أرجع gui_wait_for_chars_buf(buf, maxlen, wtime, tb_change_cnt);
}

/*
 * Fill p[4] with mouse coordinates encoded for check_termcode().
 */
    ساكن فراغ
fill_mouse_coord(char_u *p, صحيح خانہ, صحيح صف)
{
    p[0] = (char_u)(خانہ / 128 + ' ' + 1);
    p[1] = (char_u)(خانہ % 128 + ' ' + 1);
    p[2] = (char_u)(صف / 128 + ' ' + 1);
    p[3] = (char_u)(صف % 128 + ' ' + 1);
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
    فراغ
gui_send_mouse_event(
    صحيح	    button,
    صحيح	    x,
    صحيح	    y,
    صحيح	    repeated_click,
    int_u   modifiers)
{
    ساكن صحيح	    prev_صف = 0, prev_خانہ = 0;
    ساكن صحيح	    prev_button = -1;
    ساكن صحيح	    عدد_clicks = 1;
    char_u	    string[10];
    تعداد کلید_extra  button_char;
    صحيح		    صف, خانہ;
#جے_تعریف_شدہ FEAT_CLIPBOARD
    صحيح		    checkfor;
    صحيح		    did_clip = FALSE;
#جے_انت

    /*
     * Scrolling may happen at any time, also while a selection is present.
     */
    بدل (button)
    {
	حالة MOUSE_MOVE:
	    button_char = KE_MOUSEMOVE_XY;
	    اذهب button_سیٹ;
	حالة MOUSE_X1:
	    button_char = KE_X1MOUSE;
	    اذهب button_سیٹ;
	حالة MOUSE_X2:
	    button_char = KE_X2MOUSE;
	    اذهب button_سیٹ;
	حالة MOUSE_4:
	    button_char = KE_MOUSEDOWN;
	    اذهب button_سیٹ;
	حالة MOUSE_5:
	    button_char = KE_MOUSEUP;
	    اذهب button_سیٹ;
	حالة MOUSE_6:
	    button_char = KE_MOUSELEFT;
	    اذهب button_سیٹ;
	حالة MOUSE_7:
	    button_char = KE_MOUSERIGHT;
button_سیٹ:
	    {
		// Don't put events in the input queue now.
		إذا (hold_gui_events)
		    أرجع;

		صف = gui_xy2colrow(x, y, &خانہ);
		// Don't report a mouse move unless moved to a
		// different character position.
		إذا (button == MOUSE_MOVE)
		{
		    إذا (صف == prev_صف && خانہ == prev_خانہ)
			أرجع;
		    وإلا
		    {
			prev_صف = صف >= 0 ? صف : 0;
			prev_خانہ = خانہ;
		    }
		}

		string[3] = CSI;
		string[4] = KS_EXTRA;
		string[5] = (صحيح)button_char;

		// Pass the pointer coordinates of the scroll event so that we
		// know which window to scroll.
		string[6] = (char_u)(خانہ / 128 + ' ' + 1);
		string[7] = (char_u)(خانہ % 128 + ' ' + 1);
		string[8] = (char_u)(صف / 128 + ' ' + 1);
		string[9] = (char_u)(صف % 128 + ' ' + 1);

		إذا (modifiers == 0)
		    جوڑو_منزل_داخلہ_buf(string + 3, 7);
		وإلا
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = 0;
		    إذا (modifiers & MOUSE_کھسکاؤ)
			string[2] |= MOD_نقاب_کھسکاؤ;
		    إذا (modifiers & MOUSE_CTRL)
			string[2] |= MOD_نقاب_CTRL;
		    إذا (modifiers & MOUSE_ALT)
			string[2] |= MOD_نقاب_ALT;
		    جوڑو_منزل_داخلہ_buf(string, 10);
		}
		أرجع;
	    }
    }

#جے_تعریف_شدہ FEAT_CLIPBOARD
    // If a clipboard selection is in progress, handle it
    إذا (clip_تارا.state == SELECT_وچ_ترقی)
    {
	clip_process_selection(button, X_2_خانہ(x), Y_2_صف(y), repeated_click);

	// A release event may still need to be sent if the position is equal.
	صف = gui_xy2colrow(x, y, &خانہ);
	إذا (button != MOUSE_RELEASE || صف != prev_صف || خانہ != prev_خانہ)
	    أرجع;
    }

    // Determine which mouse settings to look for based on the current mode
    بدل (get_real_state())
    {
	حالة منوال_عام_BUSY:
	حالة منوال_OP_PENDING:
# ifdef FEAT_TERMINAL
	حالة منوال_TERMINAL:
# endif
	حالة منوال_عام:	checkfor = MOUSE_عام; اكسر;
	حالة منوال_VISUAL:	checkfor = MOUSE_VISUAL; اكسر;
	حالة منوال_SELECT:	checkfor = MOUSE_VISUAL; اكسر;
	حالة منوال_REPLACE:
	حالة منوال_REPLACE | منوال_LANGMAP:
	حالة منوال_VREPLACE:
	حالة منوال_VREPLACE | منوال_LANGMAP:
	حالة منوال_داخل_کرو:
	حالة منوال_داخل_کرو | منوال_LANGMAP:
				checkfor = MOUSE_داخل_کرو; اكسر;
	حالة منوال_ASKMORE:
	حالة منوال_HITRETURN:	// At the more- and hit-enter prompt pass the
				// mouse event for a click on or below the
				// message line.
				إذا (Y_2_صف(y) >= پیغام_صف)
				    checkfor = MOUSE_عام;
				وإلا
				    checkfor = MOUSE_RETURN;
				اكسر;

	    /*
	     * On the command line, use the clipboard selection on all lines
	     * but the command line.  But not when pasting.
	     */
	حالة منوال_CMDLINE:
	حالة منوال_CMDLINE | منوال_LANGMAP:
	    إذا (Y_2_صف(y) < cmdline_صف && button != MOUSE_درمیان)
		checkfor = MOUSE_NONE;
	    وإلا
		checkfor = MOUSE_COMMAND;
	    اكسر;

	افتراضي:
	    checkfor = MOUSE_NONE;
	    اكسر;
    };

    /*
     * Allow clipboard selection of text on the command line in "normal"
     * modes.  Don't do this when dragging the status line, or extending a
     * Visual selection.
     */
    إذا ((State == منوال_عام || State == منوال_عام_BUSY
						      || (State & منوال_داخل_کرو))
	    && X_2_خانہ(x) >= firstwin->w_wincol
	    && X_2_خانہ(x) < firstwin->w_wincol + topframe->fr_چوڑائی
	    && Y_2_صف(y) >= topframe->fr_اچائی + firstwin->w_winrow
	    && button != MOUSE_DRAG
# ifdef FEAT_MOUSESHAPE
	    && !drag_حالت_سطر
	    && !drag_sep_سطر
# endif
	    )
	checkfor = MOUSE_NONE;

    /*
     * Use modeless selection when holding CTRL and SHIFT pressed.
     */
    إذا ((modifiers & MOUSE_CTRL) && (modifiers & MOUSE_کھسکاؤ))
	checkfor = MOUSE_NONEF;

    /*
     * In Ex mode, always use modeless selection.
     */
    إذا (exmode_سرگرم)
	checkfor = MOUSE_NONE;

    /*
     * If the mouse settings say to not use the mouse, use the modeless
     * selection.  But if Visual is active, assume that only the Visual area
     * will be selected.
     * Exception: On the command line, both the selection is used and a mouse
     * key is sent.
     */
    إذا (!mouse_has(checkfor) || checkfor == MOUSE_COMMAND)
    {
	// Don't do modeless selection in Visual mode.
	إذا (checkfor != MOUSE_NONEF && VIsual_سرگرم && (State & منوال_عام))
	    أرجع;

	/*
	 * When 'mousemodel' is "popup", shift-left is translated to right.
	 * But not when also using Ctrl.
	 */
	إذا (mouse_model_popup() && button == MOUSE_بایاں
		&& (modifiers & MOUSE_کھسکاؤ) && !(modifiers & MOUSE_CTRL))
	{
	    button = MOUSE_دایاں;
	    modifiers &= ~ MOUSE_کھسکاؤ;
	}

	// If the selection is done, allow the right button to extend it.
	// If the selection is cleared, allow the right button to start it
	// from the cursor position.
	إذا (button == MOUSE_دایاں)
	{
	    إذا (clip_تارا.state == SELECT_CLEARED)
	    {
		إذا (State & منوال_CMDLINE)
		{
		    خانہ = پیغام_خانہ;
		    صف = پیغام_صف;
		}
		وإلا
		{
		    خانہ = curwin->w_wcol;
		    صف = curwin->w_wrow + W_WINROW(curwin);
		}
		clip_شروع_selection(خانہ, صف, FALSE);
	    }
	    clip_process_selection(button, X_2_خانہ(x), Y_2_صف(y),
							      repeated_click);
	    did_clip = TRUE;
	}
	// Allow the left button to start the selection
	وإلا إذا (button == MOUSE_بایاں)
	{
	    clip_شروع_selection(X_2_خانہ(x), Y_2_صف(y), repeated_click);
	    did_clip = TRUE;
	}

	// Always allow pasting
	إذا (button != MOUSE_درمیان)
	{
	    إذا (!mouse_has(checkfor) || button == MOUSE_RELEASE)
		أرجع;
	    إذا (checkfor != MOUSE_COMMAND)
		button = MOUSE_بایاں;
	}
	repeated_click = FALSE;
    }

    إذا (clip_تارا.state != SELECT_CLEARED && !did_clip)
	clip_clear_selection(&clip_تارا);
#جے_انت

    // Don't put events in the input queue now.
    إذا (hold_gui_events)
	أرجع;

    صف = gui_xy2colrow(x, y, &خانہ);

    /*
     * If we are dragging and the mouse hasn't moved far enough to be on a
     * different character, then don't send an event to vim.
     */
    إذا (button == MOUSE_DRAG)
    {
	إذا (صف == prev_صف && خانہ == prev_خانہ)
	    أرجع;
	// Dragging above the window, set "row" to -1 to cause a scroll.
	إذا (y < 0)
	    صف = -1;
    }

    /*
     * If topline has changed (window scrolled) since the last click, reset
     * repeated_click, because we don't want starting Visual mode when
     * clicking on a different character in the text.
     */
    إذا (curwin->w_topline != gui_prev_topline
#جے_تعریف_شدہ FEAT_DIFF
	    || curwin->w_topfill != gui_prev_topfill
#جے_انت
	    )
	repeated_click = FALSE;

    string[0] = CSI;	// this sequence is recognized by check_termcode()
    string[1] = KS_MOUSE;
    string[2] = KE_FILLER;
    إذا (button != MOUSE_DRAG && button != MOUSE_RELEASE)
    {
	إذا (repeated_click)
	{
	    /*
	     * Handle multiple clicks.	They only count if the mouse is still
	     * pointing at the same character.
	     */
	    إذا (button != prev_button || صف != prev_صف || خانہ != prev_خانہ)
		عدد_clicks = 1;
	    وإلا إذا (++عدد_clicks > 4)
		عدد_clicks = 1;
	}
	وإلا
	    عدد_clicks = 1;
	prev_button = button;
	gui_prev_topline = curwin->w_topline;
#جے_تعریف_شدہ FEAT_DIFF
	gui_prev_topfill = curwin->w_topfill;
#جے_انت

	string[3] = (char_u)(button | 0x20);
	سیٹ_عدد_MOUSE_CLICKS(string[3], عدد_clicks);
    }
    وإلا
	string[3] = (char_u)button;

    string[3] |= modifiers;
    fill_mouse_coord(string + 4, خانہ, صف);
    جوڑو_منزل_داخلہ_buf(string, 8);

    إذا (صف < 0)
	prev_صف = 0;
    وإلا
	prev_صف = صف;
    prev_خانہ = خانہ;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#إذا defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#جے_انت
}

/*
 * Convert x and y coordinate to column and row in text window.
 * Corrects for multi-byte character.
 * returns column in "*colp" and row as return value;
 */
    ساكن صحيح
gui_xy2colrow(صحيح x, صحيح y, صحيح *colp)
{
    صحيح		خانہ = check_خانہ(X_2_خانہ(x));
    صحيح		صف = check_صف(Y_2_صف(y));

    *colp = mb_fix_خانہ(خانہ, صف);
    أرجع صف;
}

#إذا defined(FEAT_MENU)
/*
 * Callback function for when a menu entry has been selected.
 */
    فراغ
gui_menu_cb(vimmenu_T *menu)
{
    char_u  بائٹاں[حجم(long_u)];

    // Don't put events in the input queue now.
    إذا (hold_gui_events)
	أرجع;

    بائٹاں[0] = CSI;
    بائٹاں[1] = KS_MENU;
    بائٹاں[2] = KE_FILLER;
    جوڑو_منزل_داخلہ_buf(بائٹاں, 3);
    جوڑو_long_منزل_buf((long_u)menu, بائٹاں);
    جوڑو_منزل_داخلہ_buf_csi(بائٹاں, حجم(long_u));
}
#جے_انت

ساكن صحيح	prev_which_scrollbars[3];

/*
 * Set which components are present.
 * If "oldval" is not NULL, "oldval" is the previous value, the new value is
 * in p_go.
 */
    فراغ
gui_init_which_components(char_u *oldval بن_ورتیا)
{
#جے_تعریف_شدہ FEAT_GUI_DARKTHEME
    ساكن صحيح	prev_dark_theme = FALSE;
    صحيح		using_dark_theme = FALSE;
#جے_انت
#جے_تعریف_شدہ FEAT_MENU
    ساكن صحيح	prev_menu_is_سرگرم = -1;
#جے_انت
#جے_تعریف_شدہ FEAT_TOOLBAR
    ساكن صحيح	prev_toolbar = -1;
    صحيح		using_toolbar = FALSE;
#جے_انت
#جے_تعریف_شدہ FEAT_GUI_TABLINE
    صحيح		using_tabline;
#جے_انت
#جے_تعریف_شدہ FEAT_GUI_MSWIN
    ساكن صحيح	prev_titlebar = FALSE;
    صحيح		using_titlebar = FALSE;
#جے_انت

#إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    ساكن صحيح	prev_fullscreen = FALSE;
    صحيح		using_fullscreen = FALSE;
#جے_انت
#إذا defined(FEAT_MENU)
    ساكن صحيح	prev_tearoff = -1;
    صحيح		using_tearoff = FALSE;
#جے_انت

    char_u	*p;
    صحيح		i;
#جے_تعریف_شدہ FEAT_MENU
    صحيح		grey_old, grey_new;
    char_u	*عارضی;
#جے_انت
    win_T	*wp;
    صحيح		need_سیٹ_ناپ;
    صحيح		fix_ناپ;

#جے_تعریف_شدہ FEAT_MENU
    إذا (oldval != عدم && gui.وچ_use)
    {
	/*
	 * Check if the menus go from grey to non-grey or vice versa.
	 */
	grey_old = (vim_strchr(oldval, GO_GREY) != عدم);
	grey_new = (vim_strchr(p_go, GO_GREY) != عدم);
	إذا (grey_old != grey_new)
	{
	    عارضی = p_go;
	    p_go = oldval;
	    gui_update_menus(MENU_ALL_MODES);
	    p_go = عارضی;
	}
    }
    gui.menu_is_سرگرم = false;
#جے_انت

    لكل (i = 0; i < 3; i++)
	gui.which_scrollbars[i] = false;
    لكل (p = p_go; *p; p++)
	بدل (*p)
	{
	    حالة GO_بایاں:
		gui.which_scrollbars[SBAR_بایاں] = true;
		اكسر;
	    حالة GO_دایاں:
		gui.which_scrollbars[SBAR_دایاں] = true;
		اكسر;
	    حالة GO_VLEFT:
		إذا (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_بایاں] = true;
		اكسر;
	    حالة GO_VRIGHT:
		إذا (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_دایاں] = true;
		اكسر;
	    حالة GO_BOT:
		gui.which_scrollbars[SBAR_BOTTOM] = true;
		اكسر;
#جے_تعریف_شدہ FEAT_GUI_DARKTHEME
	    حالة GO_DARKTHEME:
		using_dark_theme = TRUE;
		اكسر;
#جے_انت
#جے_تعریف_شدہ FEAT_MENU
	    حالة GO_MENUS:
		gui.menu_is_سرگرم = true;
		اكسر;
#جے_انت
	    حالة GO_GREY:
		// make menu's have grey items, ignored here
		اكسر;
#جے_تعریف_شدہ FEAT_GUI_MSWIN
	    حالة GO_TITLEBAR:
		using_titlebar = TRUE;
		اكسر;
#جے_انت
#إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
	    حالة GO_FULLSCREEN:
		using_fullscreen = TRUE;
		اكسر;
#جے_انت
#جے_تعریف_شدہ FEAT_TOOLBAR
	    حالة GO_TOOLBAR:
		using_toolbar = TRUE;
		اكسر;
#جے_انت
	    حالة GO_TEAROFF:
#إذا defined(FEAT_MENU)
		using_tearoff = TRUE;
#جے_انت
		اكسر;
	    افتراضي:
		// Ignore options that are not supported
		اكسر;
	}

    إذا (!gui.وچ_use)
	أرجع;

    need_سیٹ_ناپ = 0;
    fix_ناپ = FALSE;

#جے_تعریف_شدہ FEAT_GUI_MSWIN
    إذا (using_titlebar != prev_titlebar)
    {
	gui_mch_سیٹ_titlebar_colors();
	prev_titlebar = using_titlebar;
    }
#جے_انت

#إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    إذا (using_fullscreen != prev_fullscreen)
    {
	gui_mch_سیٹ_fullscreen(using_fullscreen);
	prev_fullscreen = using_fullscreen;
    }
#جے_انت

#جے_تعریف_شدہ FEAT_GUI_DARKTHEME
    إذا (using_dark_theme != prev_dark_theme)
    {
	gui_mch_سیٹ_dark_theme(using_dark_theme);
	prev_dark_theme = using_dark_theme;
    }
#جے_انت

#جے_تعریف_شدہ FEAT_GUI_TABLINE
    // Update the GUI tab line, it may appear or disappear.  This may
    // cause the non-GUI tab line to disappear or appear.
    using_tabline = gui_has_tabline();
    إذا (!gui_mch_showing_tabline() != !using_tabline)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = صفاں;
	gui_update_tabline();
	صفاں = i;
	need_سیٹ_ناپ |= RESIZE_VERT;
	إذا (using_tabline)
	    fix_ناپ = TRUE;
	إذا (!gui_use_tabline())
	    redraw_tabline = TRUE;    // may draw non-GUI tab line
    }
#جے_انت

    لكل (i = 0; i < 3; i++)
    {
	// The scrollbar needs to be updated when it is shown/unshown and
	// when switching tab pages.  But the size only changes when it's
	// shown/unshown.  Thus we need two places to remember whether a
	// scrollbar is there or not.
	إذا (gui.which_scrollbars[i] != prev_which_scrollbars[i]
		|| gui.which_scrollbars[i]
		!= curtab->tp_prev_which_scrollbars[i])
	{
	    إذا (i == SBAR_BOTTOM)
		gui_mch_enable_scrollbar(&gui.bottom_sbar,
			gui.which_scrollbars[i]);
	    وإلا
	    {
		FOR_ALL_WINDOWS(wp)
		    gui_do_scrollbar(wp, i, gui.which_scrollbars[i]);
	    }
	    إذا (gui.which_scrollbars[i] != prev_which_scrollbars[i])
	    {
		إذا (i == SBAR_BOTTOM)
		    need_سیٹ_ناپ |= RESIZE_VERT;
		وإلا
		    need_سیٹ_ناپ |= RESIZE_HOR;
		إذا (gui.which_scrollbars[i])
		    fix_ناپ = TRUE;
	    }
	}
	curtab->tp_prev_which_scrollbars[i] = gui.which_scrollbars[i];
	prev_which_scrollbars[i] = gui.which_scrollbars[i];
    }

#جے_تعریف_شدہ FEAT_MENU
    إذا (gui.menu_is_سرگرم != prev_menu_is_سرگرم)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = صفاں;
	gui_mch_enable_menu(gui.menu_is_سرگرم);
	صفاں = i;
	prev_menu_is_سرگرم = gui.menu_is_سرگرم;
	need_سیٹ_ناپ |= RESIZE_VERT;
	إذا (gui.menu_is_سرگرم)
	    fix_ناپ = TRUE;
    }
#جے_انت

#جے_تعریف_شدہ FEAT_TOOLBAR
    إذا (using_toolbar != prev_toolbar)
    {
	gui_mch_show_toolbar(using_toolbar);
	prev_toolbar = using_toolbar;
	need_سیٹ_ناپ |= RESIZE_VERT;
	إذا (using_toolbar)
	    fix_ناپ = TRUE;
    }
#جے_انت
#إذا defined(FEAT_MENU) && !(defined(MSWIN) && !defined(FEAT_TEAROFF))
    إذا (using_tearoff != prev_tearoff)
    {
	gui_mch_toggle_tearoffs(using_tearoff);
	prev_tearoff = using_tearoff;
    }
#جے_انت
    إذا (need_سیٹ_ناپ != 0)
    {
#جے_تعریف_شدہ FEAT_GUI_GTK
	لما    prev_Columns = Columns;
	لما    prev_صفاں = صفاں;
#جے_انت
	// Adjust the size of the window to make the text area keep the
	// same size and to avoid that part of our window is off-screen
	// and a scrollbar can't be used, for example.
	gui_سیٹ_shellsize(FALSE, fix_ناپ, need_سیٹ_ناپ);

#جے_تعریف_شدہ FEAT_GUI_GTK
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
	    لما    post_صفاں = صفاں;
	    لما    post_Columns = Columns;

	    إذا (!gui.starting && need_سیٹ_ناپ != (RESIZE_VERT | RESIZE_HOR))
		(فراغ)char_avail();
	    إذا ((need_سیٹ_ناپ & RESIZE_VERT) == 0)
		صفاں = prev_صفاں;
	    وإلا
		صفاں = post_صفاں;
	    إذا ((need_سیٹ_ناپ & RESIZE_HOR) == 0)
		Columns = prev_Columns;
	    وإلا
		Columns = post_Columns;
	}
#جے_انت
    }
    // When the console tabline appears or disappears the window positions
    // change.
    إذا (firstwin->w_winrow != tabline_اچائی())
	shell_new_صفاں();	// recompute window positions and heights
}

#إذا defined(FEAT_GUI_TABLINE)
/*
 * Return TRUE if the GUI is taking care of the tabline.
 * It may still be hidden if 'showtabline' is zero.
 */
    صحيح
gui_use_tabline(فراغ)
{
    أرجع gui.وچ_use && vim_strchr(p_go, GO_TABLINE) != عدم;
}

/*
 * Return TRUE if the GUI is showing the tabline.
 * This uses 'showtabline'.
 */
    ساكن صحيح
gui_has_tabline(فراغ)
{
    إذا (!gui_use_tabline()
	    || p_stal == 0
	    || (p_stal == 1 && پہلا_tabpage->tp_اگلا == عدم))
	أرجع FALSE;
    أرجع TRUE;
}

/*
 * Update the tabline.
 * This may display/undisplay the tabline and update the labels.
 */
    فراغ
gui_update_tabline(فراغ)
{
    صحيح	    showit = gui_has_tabline();
    صحيح	    shown = gui_mch_showing_tabline();

    إذا (!gui.starting && starting == 0)
    {
	// Updating the tabline uses direct GUI commands, flush
	// outstanding instructions first. (esp. clear screen)
	باہرلا_flush();

	إذا (!showit != !shown)
	{
	    // Save Rows/Columns before showing/hiding the tabline.
	    // gui_mch_show_tabline() processes GTK events (via
	    // gui_mch_update) which may trigger a spurious configure event
	    // that temporarily corrupts Rows.  Restore the original values so
	    // that gui_set_shellsize() uses the correct row/column count.
	    صحيح save_صفاں = صفاں;
	    صحيح save_Columns = Columns;

	    gui_mch_show_tabline(showit);
	    صفاں = save_صفاں;
	    Columns = save_Columns;

	    // Resize the outer window to compensate for the tabline height
	    // change.  This is also needed when called from draw_tabline()
	    // (e.g. after :tabclose), where no caller will do the resize.
	    // When called from gui_init_which_components() the subsequent
	    // gui_set_shellsize() call will redo this to the same dimensions,
	    // which is harmless.
	    gui_سیٹ_shellsize(FALSE, showit, RESIZE_VERT);
	}
	إذا (showit != 0)
	    gui_mch_update_tabline();
    }
}

/*
 * Get the label or tooltip for tab page "tp" into NameBuff[].
 */
    فراغ
get_tabline_label(
    tabpage_T	*tp,
    صحيح		tooltip)	// TRUE: get tooltip
{
    char_u	**opt;

    // Use 'guitablabel' or 'guitabtooltip' if it's set.
    opt = (tooltip ? &p_gtt : &p_gtl);
    إذا (**opt != NUL)
    {
	char_u	res[MAXPATHL];
	tabpage_T *save_curtab;
	char_u	*opt_name = (char_u *)(tooltip ? "guitabtooltip"
							     : "guitablabel");

	printer_ورقہ_عدد = tabpage_اشاریہ(tp);
# ifdef FEAT_EVAL
	سیٹ_vim_var_nr(VV_LNUM, printer_ورقہ_عدد);
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
	curbuf = curwin->w_بفر;

	// Can't use NameBuff directly, build_stl_str_hl() uses it.
	build_stl_str_hl(curwin, res, MAXPATHL, *opt, opt_name, 0,
					   0, (صحيح)Columns, عدم, عدم, عدم);
	STRCPY(NameBuff, res);

	// Back to the original curtab.
	curtab = save_curtab;
	topframe = curtab->tp_topframe;
	firstwin = curtab->tp_firstwin;
	lastwin = curtab->tp_lastwin;
	curwin = curtab->tp_curwin;
	curbuf = curwin->w_بفر;
    }

    // If 'guitablabel'/'guitabtooltip' is not set or the result is empty then
    // use a default label.
    إذا (**opt == NUL || *NameBuff == NUL)
    {
	win_T	*wp;
	صحيح	wincount = 0;
	صحيح	modified = FALSE;

	// Get the buffer name into NameBuff[] and shorten it.
	get_trans_bufname(tp == curtab ? curbuf : tp->tp_curwin->w_بفر);
	إذا (!tooltip)
	    shorten_dir(NameBuff);

	FOR_ALL_WINDOWS_وچ_TAB(tp, wp)
	{
	    ++wincount;
	    إذا (buf_Is_بدلیا(wp->w_بفر))
		modified = TRUE;
	}

	إذا (modified || wincount > 1)
	{
	    char_u  buf[40] = "+ ";		// Tentatively assume modified only
	    size_t  buflen = 2;
	    size_t  NameBufflen = STRLEN(NameBuff);

	    إذا (wincount > 1)
		buflen = vim_snprintf_safelen(
		    (حرف *)buf,
		    حجم(buf),
		    "%d%s ",
		    wincount,
		    modified ? "+" : "");

	    // Make sure resulting NameBuff will not exceed its bounds.
	    إذا (NameBufflen + buflen < MAXPATHL)
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
    صحيح
send_tabline_event(صحيح nr)
{
    char_u string[3];

    إذا (nr == tabpage_اشاریہ(curtab))
	أرجع FALSE;

    // Don't put events in the input queue now.
    إذا (hold_gui_events || cmdwin_type != 0)
    {
	// Set it back to the current tab page.
	gui_mch_سیٹ_curtab(tabpage_اشاریہ(curtab));
	أرجع FALSE;
    }

    string[0] = CSI;
    string[1] = KS_TABLINE;
    string[2] = KE_FILLER;
    جوڑو_منزل_داخلہ_buf(string, 3);
    string[0] = nr;
    جوڑو_منزل_داخلہ_buf_csi(string, 1);
    أرجع TRUE;
}

/*
 * Send a tabline menu event
 */
    فراغ
send_tabline_menu_event(صحيح tabidx, صحيح event)
{
    char_u	    string[3];

    // Don't put events in the input queue now.
    إذا (hold_gui_events)
	أرجع;

    // Cannot close the last tabpage.
    إذا (event == TABLINE_MENU_CLOSE && پہلا_tabpage->tp_اگلا == عدم)
	أرجع;

    string[0] = CSI;
    string[1] = KS_TABMENU;
    string[2] = KE_FILLER;
    جوڑو_منزل_داخلہ_buf(string, 3);
    string[0] = tabidx;
    string[1] = (char_u)(لما)event;
    جوڑو_منزل_داخلہ_buf_csi(string, 2);
}

#جے_انت

/*
 * Scrollbar stuff:
 */

/*
 * Remove all scrollbars.  Used before switching to another tab page.
 */
    فراغ
gui_remove_scrollbars(فراغ)
{
    صحيح	    i;
    win_T   *wp;

    لكل (i = 0; i < 3; i++)
    {
	إذا (i == SBAR_BOTTOM)
	    gui_mch_enable_scrollbar(&gui.bottom_sbar, FALSE);
	وإلا
	{
	    FOR_ALL_WINDOWS(wp)
		gui_do_scrollbar(wp, i, FALSE);
	}
	curtab->tp_prev_which_scrollbars[i] = -1;
    }
}

    فراغ
gui_create_scrollbar(scrollbar_T *sb, صحيح type, win_T *wp)
{
    ساكن صحيح	sbar_ident = 0;

    sb->ident = sbar_ident++;	// No check for too big, but would it happen?
    sb->wp = wp;
    sb->type = type;
    sb->قدر = 0;
    sb->ناپ = 1;
    sb->max = 1;
    sb->چوٹی = 0;
    sb->اچائی = 0;
    sb->چوڑائی = 0;
    sb->حالت_اچائی = 0;
    gui_mch_create_scrollbar(sb, (wp == عدم) ? SBAR_HORIZ : SBAR_VERT);
}

/*
 * Find the scrollbar with the given index.
 */
    scrollbar_T *
gui_لبھو_scrollbar(لما ident)
{
    win_T	*wp;

    إذا (gui.bottom_sbar.ident == ident)
	أرجع &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	إذا (wp->w_scrollbars[SBAR_بایاں].ident == ident)
	    أرجع &wp->w_scrollbars[SBAR_بایاں];
	إذا (wp->w_scrollbars[SBAR_دایاں].ident == ident)
	    أرجع &wp->w_scrollbars[SBAR_دایاں];
    }
    أرجع عدم;
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
    فراغ
gui_drag_scrollbar(scrollbar_T *sb, لما قدر, صحيح still_dragging)
{
    win_T	*wp;
    صحيح		sb_عدد;
#جے_تعریف_شدہ USE_ON_FLY_SCROLL
    colnr_T	old_leftcol = curwin->w_leftcol;
    linenr_T	old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    صحيح		old_topfill = curwin->w_topfill;
# endif
#وإلا
    char_u	بائٹاں[حجم(long_u)];
    صحيح		byte_گنتی;
#جے_انت

    إذا (sb == عدم)
	أرجع;

    // Don't put events in the input queue now.
    إذا (hold_gui_events)
	أرجع;

    إذا (cmdwin_type != 0 && sb->wp != cmdwin_win)
	أرجع;

    إذا (still_dragging)
    {
	إذا (sb->wp == عدم)
	    gui.dragged_sb = SBAR_BOTTOM;
	وإلا إذا (sb == &sb->wp->w_scrollbars[SBAR_بایاں])
	    gui.dragged_sb = SBAR_بایاں;
	وإلا
	    gui.dragged_sb = SBAR_دایاں;
	gui.dragged_wp = sb->wp;
    }
    وإلا
    {
	gui.dragged_sb = SBAR_NONE;
#جے_تعریف_شدہ FEAT_GUI_GTK
	// Keep the "dragged_wp" value until after the scrolling, for when the
	// mouse button is released.  GTK2 doesn't send the button-up event.
	gui.dragged_wp = عدم;
	// WinScrolled event
	gui_focus_change(TRUE);
#جے_انت
    }

    // Vertical sbar info is kept in the first sbar (the left one)
    إذا (sb->wp != عدم)
	sb = &sb->wp->w_scrollbars[0];

    /*
     * Check validity of value
     */
    إذا (قدر < 0)
	قدر = 0;
#جے_تعریف_شدہ SCROLL_PAST_سرا
    وإلا إذا (قدر > sb->max)
	قدر = sb->max;
#وإلا
    إذا (قدر > sb->max - sb->ناپ + 1)
	قدر = sb->max - sb->ناپ + 1;
#جے_انت

    sb->قدر = قدر;

#جے_تعریف_شدہ USE_ON_FLY_SCROLL
    // When not allowed to do the scrolling right now, return.
    // This also checked input_available(), but that causes the first click in
    // a scrollbar to be ignored when Vim doesn't have focus.
    إذا (dont_scroll)
	أرجع;
#جے_انت
    // Disallow scrolling the current window when the completion popup menu is
    // visible.
    إذا ((sb->wp == عدم || sb->wp == curwin) && pum_visible())
	أرجع;

#جے_تعریف_شدہ FEAT_RIGHTLEFT
    إذا (sb->wp == عدم && curwin->w_p_rl)
    {
	قدر = sb->max + 1 - sb->ناپ - قدر;
	إذا (قدر < 0)
	    قدر = 0;
    }
#جے_انت

    إذا (sb->wp != عدم)		// vertical scrollbar
    {
	sb_عدد = 0;
	لكل (wp = firstwin; wp != sb->wp && wp != عدم; wp = wp->w_اگلا)
	    sb_عدد++;
	إذا (wp == عدم)
	    أرجع;

#جے_تعریف_شدہ USE_ON_FLY_SCROLL
	current_scrollbar = sb_عدد;
	scrollbar_قدر = قدر;
	إذا (State & منوال_عام)
	{
	    gui_do_scroll();
	    setcursor();
	}
	وإلا إذا (State & منوال_داخل_کرو)
	{
	    ins_scroll();
	    setcursor();
	}
	وإلا إذا (State & منوال_CMDLINE)
	{
	    إذا (پیغام_scrolled == 0)
	    {
		gui_do_scroll();
		redrawcmdline();
	    }
	}
# ifdef FEAT_FOLDING
	// Value may have been changed for closed fold.
	sb->قدر = sb->wp->w_topline - 1;
# endif

	// When dragging one scrollbar and there is another one at the other
	// side move the thumb of that one too.
	إذا (gui.which_scrollbars[SBAR_دایاں] && gui.which_scrollbars[SBAR_بایاں])
	    gui_mch_سیٹ_scrollbar_thumb(
		    &sb->wp->w_scrollbars[
			    sb == &sb->wp->w_scrollbars[SBAR_دایاں]
						    ? SBAR_بایاں : SBAR_دایاں],
		    sb->قدر, sb->ناپ, sb->max);

#وإلا
	بائٹاں[0] = CSI;
	بائٹاں[1] = KS_VER_SCROLLBAR;
	بائٹاں[2] = KE_FILLER;
	بائٹاں[3] = (char_u)sb_عدد;
	byte_گنتی = 4;
#جے_انت
    }
    وإلا
    {
#جے_تعریف_شدہ USE_ON_FLY_SCROLL
	scrollbar_قدر = قدر;

	إذا (State & منوال_عام)
	    do_mousescroll_horiz(scrollbar_قدر);
	وإلا إذا (State & منوال_داخل_کرو)
	    ins_horscroll();
	وإلا إذا (State & منوال_CMDLINE)
	{
	    إذا (پیغام_scrolled == 0)
	    {
		do_mousescroll_horiz(scrollbar_قدر);
		redrawcmdline();
	    }
	}
	إذا (old_leftcol != curwin->w_leftcol)
	{
	    update_کھڑکی(curwin);   // update window, status and cmdline
	    setcursor();
	}
#وإلا
	بائٹاں[0] = CSI;
	بائٹاں[1] = KS_HOR_SCROLLBAR;
	بائٹاں[2] = KE_FILLER;
	byte_گنتی = 3;
#جے_انت
    }

#جے_تعریف_شدہ USE_ON_FLY_SCROLL
    /*
     * synchronize other windows, as necessary according to 'scrollbind'
     */
    إذا (curwin->w_p_scb
	    && ((sb->wp == عدم && curwin->w_leftcol != old_leftcol)
		|| (sb->wp == curwin && (curwin->w_topline != old_topline
# ifdef FEAT_DIFF
					   || curwin->w_topfill != old_topfill
# endif
			))))
    {
	do_check_scrollbind(TRUE);
	// need to update the window right here
	FOR_ALL_WINDOWS(wp)
	    إذا (wp->w_redr_type > 0)
		update_کھڑکی(wp);
	setcursor();
    }
    باہرلا_flush_cursor(FALSE, TRUE);
#وإلا
    جوڑو_منزل_داخلہ_buf(بائٹاں, byte_گنتی);
    جوڑو_long_منزل_buf((long_u)قدر, بائٹاں);
    جوڑو_منزل_داخلہ_buf_csi(بائٹاں, حجم(long_u));
#جے_انت
}

/*
 * Scrollbar stuff:
 */

/*
 * Called when something in the window layout has changed.
 */
    فراغ
gui_may_update_scrollbars(فراغ)
{
    إذا (gui.وچ_use && starting == 0)
    {
	باہرلا_flush();
	gui_init_which_components(عدم);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
}

    فراغ
gui_update_scrollbars(
    صحيح		force)	    // Force all scrollbars to get updated
{
    win_T	*wp;
    scrollbar_T	*sb;
    لما	val, ناپ, max;		// need 32 bits here
    صحيح		which_sb;
    صحيح		h, y;
    ساكن win_T *prev_curwin = عدم;

    // Update the horizontal scrollbar
    gui_update_horiz_scrollbar(force);

#جے_بنا_تعریف MSWIN
    // Return straight away if there is neither a left nor right scrollbar.
    // On MS-Windows this is required anyway for scrollwheel messages.
    إذا (!gui.which_scrollbars[SBAR_بایاں] && !gui.which_scrollbars[SBAR_دایاں])
	أرجع;
#جے_انت

    /*
     * Don't want to update a scrollbar while we're dragging it.  But if we
     * have both a left and right scrollbar, and we drag one of them, we still
     * need to update the other one.
     */
    إذا (!force && (gui.dragged_sb == SBAR_بایاں || gui.dragged_sb == SBAR_دایاں)
	    && gui.which_scrollbars[SBAR_بایاں]
	    && gui.which_scrollbars[SBAR_دایاں])
    {
	/*
	 * If we have two scrollbars and one of them is being dragged, just
	 * copy the scrollbar position from the dragged one to the other one.
	 */
	which_sb = SBAR_بایاں + SBAR_دایاں - gui.dragged_sb;
	إذا (gui.dragged_wp != عدم)
	    gui_mch_سیٹ_scrollbar_thumb(
		    &gui.dragged_wp->w_scrollbars[which_sb],
		    gui.dragged_wp->w_scrollbars[0].قدر,
		    gui.dragged_wp->w_scrollbars[0].ناپ,
		    gui.dragged_wp->w_scrollbars[0].max);
    }

    // avoid that moving components around generates events
    ++hold_gui_events;

    FOR_ALL_WINDOWS(wp)
    {
	إذا (wp->w_بفر == عدم)	// just in case
	    استمر;
	// Skip a scrollbar that is being dragged.
	إذا (!force && (gui.dragged_sb == SBAR_بایاں
					     || gui.dragged_sb == SBAR_دایاں)
		&& gui.dragged_wp == wp)
	    استمر;

#جے_تعریف_شدہ SCROLL_PAST_سرا
	max = wp->w_بفر->b_ml.ml_سطر_گنتی - 1;
#وإلا
	max = wp->w_بفر->b_ml.ml_سطر_گنتی + wp->w_اچائی - 2;
#جے_انت
	إذا (max < 0)			// empty buffer
	    max = 0;
	val = wp->w_topline - 1;
	ناپ = wp->w_اچائی;
#جے_تعریف_شدہ SCROLL_PAST_سرا
	إذا (val > max)			// just in case
	    val = max;
#وإلا
	إذا (ناپ > max + 1)		// just in case
	    ناپ = max + 1;
	إذا (val > max - ناپ + 1)
	    val = max - ناپ + 1;
#جے_انت
	إذا (val < 0)			// minimal value is 0
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
	إذا (ناپ < 1 || wp->w_botline - 2 > max)
	{
	    /*
	     * This can happen during changing files.  Just don't update the
	     * scrollbar for now.
	     */
	    sb->اچائی = 0;	    // Force update next time
	    إذا (gui.which_scrollbars[SBAR_بایاں])
		gui_do_scrollbar(wp, SBAR_بایاں, FALSE);
	    إذا (gui.which_scrollbars[SBAR_دایاں])
		gui_do_scrollbar(wp, SBAR_دایاں, FALSE);
	    استمر;
	}
	إذا (force || sb->اچائی != wp->w_اچائی
	    || sb->چوٹی != wp->w_winrow
	    || sb->حالت_اچائی != wp->w_حالت_اچائی
	    || sb->چوڑائی != wp->w_چوڑائی
	    || prev_curwin != curwin)
	{
	    // Height, width or position of scrollbar has changed.  For
	    // vertical split: curwin changed.
	    sb->اچائی = wp->w_اچائی;
	    sb->چوٹی = wp->w_winrow;
	    sb->حالت_اچائی = wp->w_حالت_اچائی;
	    sb->چوڑائی = wp->w_چوڑائی;

	    // Calculate height and position in pixels
	    h = (sb->اچائی + sb->حالت_اچائی) * gui.char_اچائی;
	    y = sb->چوٹی * gui.char_اچائی + gui.border_وتھ;
#إذا defined(FEAT_MENU) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF) && !defined(FEAT_GUI_PHOTON)
	    إذا (gui.menu_is_سرگرم)
		y += gui.menu_اچائی;
#جے_انت

#إذا defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU))
	    إذا (vim_strchr(p_go, GO_TOOLBAR) != عدم)
		y += gui.toolbar_اچائی;
#جے_انت

#إذا defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
	    إذا (gui_has_tabline())
		y += gui.tabline_اچائی;
#جے_انت

	    إذا (wp->w_winrow == 0)
	    {
		// Height of top scrollbar includes width of top border
		h += gui.border_وتھ;
		y -= gui.border_وتھ;
	    }
	    إذا (gui.which_scrollbars[SBAR_بایاں])
	    {
		gui_mch_سیٹ_scrollbar_pos(&wp->w_scrollbars[SBAR_بایاں],
					  gui.بایاں_sbar_x, y,
					  gui.scrollbar_چوڑائی, h);
		gui_do_scrollbar(wp, SBAR_بایاں, TRUE);
	    }
	    إذا (gui.which_scrollbars[SBAR_دایاں])
	    {
		gui_mch_سیٹ_scrollbar_pos(&wp->w_scrollbars[SBAR_دایاں],
					  gui.دایاں_sbar_x, y,
					  gui.scrollbar_چوڑائی, h);
		gui_do_scrollbar(wp, SBAR_دایاں, TRUE);
	    }
	}

	إذا (force || sb->قدر != val || sb->ناپ != ناپ || sb->max != max)
	{
	    // Thumb of scrollbar has moved
	    sb->قدر = val;
	    sb->ناپ = ناپ;
	    sb->max = max;
	    إذا (gui.which_scrollbars[SBAR_بایاں]
		    && (gui.dragged_sb != SBAR_بایاں || gui.dragged_wp != wp))
		gui_mch_سیٹ_scrollbar_thumb(&wp->w_scrollbars[SBAR_بایاں],
					    val, ناپ, max);
	    إذا (gui.which_scrollbars[SBAR_دایاں]
		    && (gui.dragged_sb != SBAR_دایاں || gui.dragged_wp != wp))
		gui_mch_سیٹ_scrollbar_thumb(&wp->w_scrollbars[SBAR_دایاں],
					    val, ناپ, max);
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
    ساكن فراغ
gui_do_scrollbar(
    win_T	*wp,
    صحيح		which,	    // SBAR_LEFT or SBAR_RIGHT
    صحيح		enable)	    // TRUE to enable scrollbar
{
    صحيح		midcol = curwin->w_wincol + curwin->w_چوڑائی / 2;
    صحيح		has_midcol = (wp->w_wincol <= midcol
				     && wp->w_wincol + wp->w_چوڑائی >= midcol);

    // Only enable scrollbars that contain the middle column of the current
    // window.
    إذا (gui.which_scrollbars[SBAR_دایاں] != gui.which_scrollbars[SBAR_بایاں])
    {
	// Scrollbars only on one side.  Don't enable scrollbars that don't
	// contain the middle column of the current window.
	إذا (!has_midcol)
	    enable = FALSE;
    }
    وإلا
    {
	// Scrollbars on both sides.  Don't enable scrollbars that neither
	// contain the middle column of the current window nor are on the far
	// side.
	إذا (midcol > Columns / 2)
	{
	    إذا (which == SBAR_بایاں ? wp->w_wincol != 0 : !has_midcol)
		enable = FALSE;
	}
	وإلا
	{
	    إذا (which == SBAR_دایاں ? wp->w_wincol + wp->w_چوڑائی != Columns
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
    صحيح
gui_do_scroll(فراغ)
{
    win_T	*wp, *save_wp;
    صحيح		i;
    لما	nlines;
    pos_T	old_cursor;
    linenr_T	old_topline;
#جے_تعریف_شدہ FEAT_DIFF
    صحيح		old_topfill;
#جے_انت

    لكل (wp = firstwin, i = 0; i < current_scrollbar; wp = W_اگلا(wp), i++)
	إذا (wp == عدم)
	    اكسر;
    إذا (wp == عدم)
	// Couldn't find window
	أرجع FALSE;
    // don't redraw, LineOffset and similar are not valid!
    إذا (exmode_سرگرم)
	أرجع FALSE;

    /*
     * Compute number of lines to scroll.  If zero, nothing to do.
     */
    nlines = (لما)scrollbar_قدر + 1 - (لما)wp->w_topline;
    إذا (nlines == 0)
	أرجع FALSE;

    save_wp = curwin;
    old_topline = wp->w_topline;
#جے_تعریف_شدہ FEAT_DIFF
    old_topfill = wp->w_topfill;
#جے_انت
    old_cursor = wp->w_cursor;
    curwin = wp;
    curbuf = wp->w_بفر;
    إذا (nlines < 0)
	scrolldown(-nlines, gui.dragged_wp == عدم);
    وإلا
	scrollup(nlines, gui.dragged_wp == عدم);
    // Reset dragged_wp after using it.  "dragged_sb" will have been reset for
    // the mouse-up event already, but we still want it to behave like when
    // dragging.  But not the next click in an arrow.
    إذا (gui.dragged_sb == SBAR_NONE)
	gui.dragged_wp = عدم;

    إذا (old_topline != wp->w_topline
#جے_تعریف_شدہ FEAT_DIFF
	    || old_topfill != wp->w_topfill
#جے_انت
	    )
    {
	إذا (get_scrolloff_قدر() != 0)
	{
	    cursor_correct();		// fix window for 'so'
	    update_topline();		// avoid up/down jump
	}
	إذا (old_cursor.lnum != wp->w_cursor.lnum)
	    coladvance(wp->w_curswant);
	wp->w_scbind_pos = wp->w_topline;
    }

    // Make sure wp->w_leftcol and wp->w_skipcol are correct.
    validate_cursor();

    curwin = save_wp;
    curbuf = save_wp->w_بفر;

    /*
     * Don't call updateWindow() when nothing has changed (it will overwrite
     * the status line!).
     *
     * Check for ScreenLines, because in ex-mode, we don't have a valid display.
     */
    إذا (Screen_سطریں != عدم && (old_topline != wp->w_topline
	    || wp->w_redr_type != 0
#جے_تعریف_شدہ FEAT_DIFF
	    || old_topfill != wp->w_topfill
#جے_انت
	    ))
    {
	صحيح type = UPD_VALID;

	إذا (pum_visible())
	{
	    type = UPD_NOT_VALID;
	    wp->w_سطریں_valid = 0;
	}

	// Don't set must_redraw here, it may cause the popup menu to
	// disappear when losing focus after a scrollbar drag.
	إذا (wp->w_redr_type < type)
	    wp->w_redr_type = type;
	mch_disable_flush();
	update_کھڑکی(wp);   // update window, status line, and cmdline
	mch_enable_flush();
    }

    // May need to redraw the popup menu.
    إذا (pum_visible())
	pum_redraw();

    أرجع (wp == curwin && !EQUAL_POS(curwin->w_cursor, old_cursor));
}

/*
 * Horizontal scrollbar stuff:
 */
    ساكن فراغ
gui_update_horiz_scrollbar(صحيح force)
{
    لما	قدر, ناپ, max;

    إذا (!gui.which_scrollbars[SBAR_BOTTOM])
	أرجع;

    إذا (!force && gui.dragged_sb == SBAR_BOTTOM)
	أرجع;

    إذا (!force && curwin->w_p_wrap && gui.prev_wrap)
	أرجع;

    /*
     * It is possible for the cursor to be invalid if we're in the middle of
     * something (like changing files).  If so, don't do anything for now.
     */
    إذا (curwin->w_cursor.lnum > curbuf->b_ml.ml_سطر_گنتی)
    {
	gui.bottom_sbar.قدر = -1;
	أرجع;
    }

    ناپ = curwin->w_چوڑائی;
    إذا (curwin->w_p_wrap)
    {
	قدر = 0;
#جے_تعریف_شدہ SCROLL_PAST_سرا
	max = 0;
#وإلا
	max = curwin->w_چوڑائی - 1;
#جے_انت
    }
    وإلا
    {
	قدر = curwin->w_leftcol;
	max = scroll_سطر_len(ui_لبھو_longest_lnum());

	إذا (virtual_سرگرم())
	{
	    // May move the cursor even further to the right.
	    إذا (curwin->w_virtcol >= (colnr_T)max)
		max = curwin->w_virtcol;
	}

#جے_بنا_تعریف SCROLL_PAST_سرا
	max += curwin->w_چوڑائی - 1;
#جے_انت
	// The line number isn't scrolled, thus there is less space when
	// 'number' or 'relativenumber' is set (also for 'foldcolumn').
	ناپ -= curwin_خانہ_off();
#جے_بنا_تعریف SCROLL_PAST_سرا
	max -= curwin_خانہ_off();
#جے_انت
    }

#جے_بنا_تعریف SCROLL_PAST_سرا
    إذا (قدر > max - ناپ + 1)
	قدر = max - ناپ + 1;	    // limit the value to allowable range
#جے_انت

#جے_تعریف_شدہ FEAT_RIGHTLEFT
    إذا (curwin->w_p_rl)
    {
	قدر = max + 1 - ناپ - قدر;
	إذا (قدر < 0)
	{
	    ناپ += قدر;
	    قدر = 0;
	}
    }
#جے_انت
    إذا (!force && قدر == gui.bottom_sbar.قدر && ناپ == gui.bottom_sbar.ناپ
						&& max == gui.bottom_sbar.max)
	أرجع;

    gui.bottom_sbar.قدر = قدر;
    gui.bottom_sbar.ناپ = ناپ;
    gui.bottom_sbar.max = max;
    gui.prev_wrap = curwin->w_p_wrap;

    gui_mch_سیٹ_scrollbar_thumb(&gui.bottom_sbar, قدر, ناپ, max);
}

/*
 * Check that none of the colors are the same as the background color
 */
    فراغ
gui_check_colors(فراغ)
{
    إذا (gui.norm_pixel == gui.پٹھ_pixel || gui.norm_pixel == INVALCOLOR)
    {
	gui_سیٹ_bg_color((char_u *)"White");
	إذا (gui.norm_pixel == gui.پٹھ_pixel || gui.norm_pixel == INVALCOLOR)
	    gui_سیٹ_fg_color((char_u *)"Black");
    }
}

    ساكن فراغ
gui_سیٹ_fg_color(char_u *name)
{
    gui.norm_pixel = gui_get_color(name);
    hl_سیٹ_fg_color_name(vim_strsave(name));
}

    ساكن فراغ
gui_سیٹ_bg_color(char_u *name)
{
    gui.پٹھ_pixel = gui_get_color(name);
    hl_سیٹ_bg_color_name(vim_strsave(name));
}

/*
 * Allocate a color by name.
 * Returns INVALCOLOR and gives an error message when failed.
 */
    guicolor_T
gui_get_color(char_u *name)
{
    guicolor_T	t;

    إذا (*name == NUL)
	أرجع INVALCOLOR;
    t = gui_mch_get_color(name);

    صحيح is_none = STRCMP(name, "none") == 0;
    إذا (t == INVALCOLOR
#إذا defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
	    && (gui.وچ_use || is_none)
#جے_انت
	    )
    {
	إذا (is_none)
	    emsg(_(e_cannot_use_color_none_did_you_اوسط_none));
	وإلا
	    semsg(_(e_cannot_allocate_color_str), name);
    }
    أرجع t;
}

/*
 * Return the grey value of a color (range 0-255).
 */
    صحيح
gui_get_lightness(guicolor_T pixel)
{
    long_u	rgb = (long_u)gui_mch_get_rgb(pixel);

    أرجع  (صحيح)(  (((rgb >> 16) & 0xff) * 299)
		   + (((rgb >> 8) & 0xff) * 587)
		   +  ((rgb	  & 0xff) * 114)) / 1000;
}

    char_u *
gui_bg_default(فراغ)
{
    إذا (gui_get_lightness(gui.پٹھ_pixel) < 127)
	أرجع (char_u *)"dark";
    أرجع (char_u *)"light";
}

/*
 * Option initializations that can only be done after opening the GUI window.
 */
    ساكن فراغ
init_gui_options(فراغ)
{
    // Set the 'background' option according to the lightness of the
    // background color, unless the user has set it already.
    إذا (!option_was_سیٹ((char_u *)"bg") && STRCMP(p_bg, gui_bg_default()) != 0)
    {
	سیٹ_option_قدر_give_err((char_u *)"bg", 0L, gui_bg_default(), 0);
	highlight_بدلیا();
    }
}

#إذا defined(FEAT_GUI_X11)
    فراغ
gui_new_scrollbar_colors(فراغ)
{
    win_T	*wp;

    // Nothing to do if GUI hasn't started yet.
    إذا (!gui.وچ_use)
	أرجع;

    FOR_ALL_WINDOWS(wp)
    {
	gui_mch_سیٹ_scrollbar_colors(&(wp->w_scrollbars[SBAR_بایاں]));
	gui_mch_سیٹ_scrollbar_colors(&(wp->w_scrollbars[SBAR_دایاں]));
    }
    gui_mch_سیٹ_scrollbar_colors(&gui.bottom_sbar);
}
#جے_انت

/*
 * Call this when focus has changed.
 */
    فراغ
gui_focus_change(صحيح وچ_focus)
{
/*
 * Skip this code to avoid drawing the cursor when debugging and switching
 * between the debugger window and gvim.
 */
#إذا 1
    gui.وچ_focus = وچ_focus;
    باہرلا_flush_cursor(TRUE, FALSE);

# ifdef FEAT_GUI_MSWIN
    gui_mch_سیٹ_titlebar_colors();
# endif

# ifdef FEAT_XIM
    xim_سیٹ_focus(وچ_focus);
# endif

    // Put events in the input queue only when allowed.
    // ui_focus_change() isn't called directly, because it invokes
    // autocommands and that must not happen asynchronously.
    إذا (!hold_gui_events
# إذا defined(FEAT_GUI_GTK) && defined(FEAT_GUI_DIALOG)
	    && gui.dialogs_سرگرم == 0
# endif
       )
    {
	char_u  بائٹاں[3];

	بائٹاں[0] = CSI;
	بائٹاں[1] = KS_EXTRA;
	بائٹاں[2] = وچ_focus ? (صحيح)KE_FOCUSGAINED : (صحيح)KE_FOCUSLOST;
	جوڑو_منزل_داخلہ_buf(بائٹاں, 3);
    }
#جے_انت
}

/*
 * When mouse moved: apply 'mousefocus'.
 * Also updates the mouse pointer shape.
 */
    ساكن فراغ
gui_mouse_focus(صحيح x, صحيح y)
{
    win_T	*wp;
    char_u	st[8];

#جے_تعریف_شدہ FEAT_MOUSESHAPE
    // Get window pointer, and update mouse shape as well.
    wp = xy2win(x, y, IGNORE_POPUP);
#جے_انت

    // Only handle this when 'mousefocus' set and ...
    إذا (p_mousef
	    && !hold_gui_events		// not holding events
	    && (State & (منوال_عام | منوال_داخل_کرو))// Normal/Visual/Insert mode
	    && State != منوال_HITRETURN	// but not hit-return prompt
	    && پیغام_scrolled == 0	// no scrolled message
	    && !need_mouse_correct	// not moving the pointer
	    && gui.وچ_focus)		// gvim in focus
    {
	// Don't move the mouse when it's left or right of the Vim window
	إذا (x < 0 || x > Columns * gui.char_چوڑائی)
	    أرجع;
#جے_بنا_تعریف FEAT_MOUSESHAPE
	wp = xy2win(x, y, IGNORE_POPUP);
#جے_انت
	إذا (wp == curwin || wp == عدم)
	    أرجع;	// still in the same old window, or none at all

	// Ignore position in the tab pages line.
	إذا (Y_2_صف(y) < tabline_اچائی())
	    أرجع;

	/*
	 * Format a mouse click on status line input,
	 * ala gui_send_mouse_event(0, x, y, 0, 0);
	 * Trick: Use a column number -1, so that get_pseudo_mouse_code() will
	 * generate a K_LEFTMOUSE_NM key code.
	 */
	إذا (انت_op)
	{
	    // abort the current operator first
	    st[0] = ESC;
	    جوڑو_منزل_داخلہ_buf(st, 1);
	}
	st[0] = CSI;
	st[1] = KS_MOUSE;
	st[2] = KE_FILLER;
	st[3] = (char_u)MOUSE_بایاں;
	fill_mouse_coord(st + 4,
		wp->w_wincol == 0 ? -1 : wp->w_wincol + MOUSE_COLOFF,
		wp->w_اچائی + W_WINROW(wp));

	جوڑو_منزل_داخلہ_buf(st, 8);
	st[3] = (char_u)MOUSE_RELEASE;
	جوڑو_منزل_داخلہ_buf(st, 8);
#جے_تعریف_شدہ FEAT_GUI_GTK
	// Need to wake up the main loop
	إذا (gtk_main_level() > 0)
	    gtk_main_quit();
#جے_انت
    }
}

/*
 * Called when the mouse moved (but not when dragging).
 */
    فراغ
gui_mouse_moved(صحيح x, صحيح y)
{
    // Ignore this while still starting up.
    إذا (!gui.وچ_use || gui.starting)
	أرجع;

    // apply 'mousefocus' and pointer shape
    gui_mouse_focus(x, y);

    إذا (p_mousemev
#جے_تعریف_شدہ FEAT_PROP_POPUP
	|| popup_ورتدا_mouse_move
#جے_انت
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
gui_mouse_کھڑکی(mouse_لبھو_T popup)
{
    صحيح		x, y;

    إذا (!(gui.وچ_use && (p_mousef || popup == لبھو_POPUP)))
	أرجع عدم;
    gui_mch_getmouse(&x, &y);

    // Only use the mouse when it's on the Vim window
    إذا (x >= 0 && x <= Columns * gui.char_چوڑائی
	    && y >= 0 && Y_2_صف(y) >= tabline_اچائی())
	أرجع xy2win(x, y, popup);
    أرجع عدم;
}

/*
 * Called when mouse should be moved to window with focus.
 */
    فراغ
gui_mouse_correct(فراغ)
{
    win_T	*wp = عدم;

    need_mouse_correct = FALSE;

    wp = gui_mouse_کھڑکی(IGNORE_POPUP);
    إذا (wp == curwin || wp == عدم)
	أرجع;

    // If in other than current window
    validate_cline_صف();
    gui_mch_setmouse((صحيح)W_ENDCOL(curwin) * gui.char_چوڑائی - 3,
	    (W_WINROW(curwin) + curwin->w_wrow) * gui.char_اچائی
	    + (gui.char_اچائی) / 2);
}

/*
 * Find window where the mouse pointer "x" / "y" coordinate is in.
 * As a side effect update the shape of the mouse pointer.
 */
    ساكن win_T *
xy2win(صحيح x, صحيح y, mouse_لبھو_T popup)
{
    صحيح		صف;
    صحيح		خانہ;
    win_T	*wp;

    صف = Y_2_صف(y);
    خانہ = X_2_خانہ(x);

    إذا (صف < 0 || خانہ < 0)		// before first window
	أرجع عدم;
    wp = mouse_لبھو_win(&صف, &خانہ, popup);
    إذا (wp == عدم)
    {
#جے_تعریف_شدہ FEAT_MOUSESHAPE
	update_mouseshape(-2);
#جے_انت
	أرجع عدم;
    }
#جے_تعریف_شدہ FEAT_MOUSESHAPE
    إذا (State == منوال_HITRETURN || State == منوال_ASKMORE)
    {
	إذا (Y_2_صف(y) >= پیغام_صف)
	    update_mouseshape(SHAPE_IDX_MOREL);
	وإلا
	    update_mouseshape(SHAPE_IDX_ہور);
    }
    وإلا إذا (صف >= wp->w_اچائی + wp->w_حالت_اچائی)	// below status line
	update_mouseshape(SHAPE_IDX_CLINE);
    وإلا إذا (!(State & منوال_CMDLINE) && wp->w_vsep_چوڑائی > 0 && خانہ == wp->w_چوڑائی
	    && (!(صف >= wp->w_اچائی && صف < wp->w_اچائی
	    + wp->w_حالت_اچائی) || !stl_connected(wp)) && پیغام_scrolled == 0)
	update_mouseshape(SHAPE_IDX_VSEP);
    وإلا إذا (!(State & منوال_CMDLINE) && wp->w_حالت_اچائی > 0
	    && صف >= wp->w_اچائی && صف < wp->w_اچائی + wp->w_حالت_اچائی
	    && پیغام_scrolled == 0)
	update_mouseshape(SHAPE_IDX_حالت);
    وإلا
	update_mouseshape(-2);
#جے_انت
    أرجع wp;
}

/*
 * ":gui" and ":gvim": Change from the terminal version to the GUI version.
 * File names may be given to redefine the args list.
 */
    فراغ
ex_gui(exarg_T *eap)
{
    char_u	*arg = eap->arg;

    /*
     * Check for "-f" argument: foreground, don't fork.
     * Also don't fork when started with "gvim -f".
     * Do fork when using "gui -b".
     */
    إذا (arg[0] == '-'
	    && (arg[1] == 'f' || arg[1] == 'b')
	    && (arg[2] == NUL || VIM_ISWHITE(arg[2])))
    {
	gui.dofork = (arg[1] == 'b');
	eap->arg = skipwhite(eap->arg + 2);
    }
    إذا (!gui.وچ_use)
    {
#إذا defined(VIMDLL) && !defined(EXPERIMENTAL_GUI_CMD)
	إذا (!gui.starting)
	{
	    emsg(_(e_gui_cannot_be_ورتیا_not_enabled_at_compile_ویلا));
	    أرجع;
	}
#جے_انت
	// Clear the command.  Needed for when forking+exiting, to avoid part
	// of the argument ending up after the shell prompt.
	پیغام_clr_eos_force();
#جے_تعریف_شدہ GUI_MAY_SPAWN
	إذا (!سرے_excmd2(eap->cmd, eap->arg))
	    gui_شروع(eap->arg);
	وإلا
#جے_انت
	    gui_شروع(عدم);
#جے_تعریف_شدہ FEAT_کم_CHANNEL
	channel_gui_رجسٹر_all();
#جے_انت
    }
    إذا (!سرے_excmd2(eap->cmd, eap->arg))
	ex_اگلا(eap);
}

#إذا (defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	    || defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON) \
	    || defined(FEAT_GUI_HAIKU)) \
	    && defined(FEAT_TOOLBAR)
/*
 * This is shared between Haiku, Motif, and GTK.
 */

/*
 * Callback function for do_in_runtimepath().
 */
    ساكن فراغ
gfp_setname(char_u *fname, فراغ *cookie)
{
    char_u	*gfp_بفر = cookie;

    إذا (STRLEN(fname) >= MAXPATHL)
	*gfp_بفر = NUL;
    وإلا
	STRCPY(gfp_بفر, fname);
}

/*
 * Find the path of bitmap "name" with extension "ext" in 'runtimepath'.
 * Return FAIL for failure and OK if buffer[MAXPATHL] contains the result.
 */
    صحيح
gui_لبھو_bitmap(char_u *name, char_u *بفر, حرف *ext)
{
    إذا (STRLEN(name) > MAXPATHL - 14)
	أرجع FAIL;
    vim_snprintf((حرف *)بفر, MAXPATHL, "bitmaps/%s.%s", name, ext);
    إذا (do_وچ_runtimepath(بفر, 0, gfp_setname, بفر) == FAIL
							    || *بفر == NUL)
	أرجع FAIL;
    أرجع OK;
}

# إذا !defined(FEAT_GUI_GTK)
/*
 * Given the name of the "icon=" argument, try finding the bitmap file for the
 * icon.  If it is an absolute path name, use it as it is.  Otherwise append
 * "ext" and search for it in 'runtimepath'.
 * The result is put in "buffer[MAXPATHL]".  If something fails "buffer"
 * contains "name".
 */
    فراغ
gui_لبھو_iconfile(char_u *name, char_u *بفر, حرف *ext)
{
    char_u	buf[MAXPATHL + 1];

    expand_env(name, بفر, MAXPATHL);
    إذا (!mch_is_بھریا_Name(بفر) && gui_لبھو_bitmap(بفر, buf, ext) == OK)
	STRCPY(بفر, buf);
}
# endif
#جے_انت

#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)|| defined(FEAT_GUI_HAIKU)
    فراغ
display_errors(فراغ)
{
    char_u	*p;

    إذا (isatty(2))
    {
	fflush(stderr);
	أرجع;
    }

    إذا (error_ga.ga_انکڑے == عدم)
	أرجع;

    // avoid putting up a message box with blanks only
    لكل (p = (char_u *)error_ga.ga_انکڑے; *p != NUL; ++p)
	إذا (!محفوظ_isspace(*p))
	{
	    // Truncate a very long message, it will go off-screen.
	    إذا (STRLEN(p) > 2000)
		STRCPY(p + 2000 - 14, "...(truncated)");
	    (فراغ)do_dialog(VIM_ERROR, (char_u *)_("Error"),
		    p, (char_u *)_("&Ok"), 1, عدم, FALSE);
	    اكسر;
	}
    ga_clear(&error_ga);
}
#جے_انت

#إذا defined(NO_CONSOLE_داخلہ)
/*
 * Return TRUE if still starting up and there is no place to enter text.
 * For GTK and X11 we check if stderr is not a tty, which means we were
 * (probably) started from the desktop.  Also check stdin, "vim >& file" does
 * allow typing on stdin.
 */
    صحيح
no_console_داخلہ(فراغ)
{
    أرجع ((!gui.وچ_use || gui.starting)
# ifndef NO_CONSOLE
	    && !isatty(0) && !isatty(2)
# endif
	    );
}
#جے_انت

#إذا defined(لبھو_REPLACE_DIALOG) \
	|| defined(NEED_GUI_UPDATE_SCREEN)
/*
 * Update the current window and the screen.
 */
    فراغ
gui_update_screen(فراغ)
{
# ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_سطر = 0;
    linenr_T	conceal_new_cursor_سطر = 0;
    صحيح		conceal_update_سطریں = FALSE;
# endif

    update_topline();
    validate_cursor();

    // Trigger CursorMoved if the cursor moved.
    إذا (!انت_op && (has_cursormoved()
# ifdef FEAT_PROP_POPUP
		|| popup_visible
# endif
# ifdef FEAT_CONCEAL
		|| curwin->w_p_cole > 0
# endif
		) && !EQUAL_POS(آخری_cursormoved, curwin->w_cursor))
    {
	إذا (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, عدم, عدم, FALSE, curbuf);
# ifdef FEAT_PROP_POPUP
	إذا (popup_visible)
	    popup_check_cursor_pos();
# endif
# ifdef FEAT_CONCEAL
	إذا (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_سطر = آخری_cursormoved.lnum;
	    conceal_new_cursor_سطر = curwin->w_cursor.lnum;
	    conceal_update_سطریں = TRUE;
	}
# endif
	آخری_cursormoved = curwin->w_cursor;
    }

    إذا (!انت_op)
	may_trigger_win_scrolled_resized();

# ifdef FEAT_CONCEAL
    إذا (conceal_update_سطریں
	    && (conceal_old_cursor_سطر != conceal_new_cursor_سطر
		|| conceal_cursor_سطر(curwin)
		|| need_cursor_سطر_redraw))
    {
	إذا (conceal_old_cursor_سطر != conceal_new_cursor_سطر)
	    redrawWinline(curwin, conceal_old_cursor_سطر);
	redrawWinline(curwin, conceal_new_cursor_سطر);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_سطر_redraw = FALSE;
    }
# endif
    update_screen(0);	// may need to update the screen
    setcursor();
    باہرلا_flush_cursor(TRUE, FALSE);
}
#جے_انت

#إذا defined(لبھو_REPLACE_DIALOG)
/*
 * Get the text to use in a find/replace dialog.  Uses the last search pattern
 * if the argument is empty.
 * Returns an allocated string.
 */
    char_u *
get_لبھو_dialog_متن(
    char_u	*arg,
    صحيح		*wwordp,	// return: TRUE if \< \> found
    صحيح		*mcasep)	// return: TRUE if \C found
{
    char_u	*متن;

    إذا (*arg == NUL)
	متن = آخری_تلاش_pat();
    وإلا
	متن = arg;
    إذا (متن != عدم)
    {
	صحيح len = (صحيح)STRLEN(متن);

	متن = vim_strnsave(متن, len);
	إذا (متن != عدم)
	{
	    صحيح i;

	    // Remove "\V"
	    إذا (len >= 2 && STRNCMP(متن, "\\V", 2) == 0)
	    {
		mch_memmove(متن, متن + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\c" and "\C" and remove.
	    إذا (len >= 2 && *متن == '\\' && (متن[1] == 'c' || متن[1] == 'C'))
	    {
		*mcasep = (متن[1] == 'C');
		mch_memmove(متن, متن + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\<text\>" and remove.
	    إذا (len >= 4
		    && STRNCMP(متن, "\\<", 2) == 0
		    && STRNCMP(متن + len - 2, "\\>", 2) == 0)
	    {
		*wwordp = TRUE;
		mch_memmove(متن, متن + 2, (size_t)(len - 4));
		متن[len - 4] = NUL;
	    }

	    // Recognize "\/" or "\?" and remove.
	    لكل (i = 0; i + 1 < len; ++i)
		إذا (متن[i] == '\\' && (متن[i + 1] == '/'
						       || متن[i + 1] == '?'))
		{
		    mch_memmove(متن + i, متن + i + 1, (size_t)(len - i));
		    --len;
		}
	}
    }
    أرجع متن;
}

/*
 * Handle the press of a button in the find-replace dialog.
 * Return TRUE when something was added to the input buffer.
 */
    صحيح
gui_do_findrepl(
    صحيح		flags,		// one of FRD_REPLACE, FRD_FINDNEXT, etc.
    char_u	*لبھو_متن,
    char_u	*repl_متن,
    صحيح		تھلے)		// Search downwards.
{
    garray_T	ga;
    صحيح		i;
    صحيح		type = (flags & FRD_TYPE_نقاب);
    char_u	*p;
    regmatch_T	regmatch;
    صحيح		save_did_emsg = did_emsg;
    ساكن صحيح  busy = FALSE;

    // When the screen is being updated we should not change buffers and
    // windows structures, it may cause freed memory to be used.  Also don't
    // do this recursively (pressing "Find" quickly several times).
    إذا (updating_screen || busy)
	أرجع FALSE;

    // refuse replace when text cannot be changed
    إذا ((type == FRD_REPLACE || type == FRD_REPLACEALL) && متن_locked())
	أرجع FALSE;

    busy = TRUE;

    ga_init2(&ga, 1, 100);
    إذا (type == FRD_REPLACEALL)
	GA_CONCAT_LITERAL(&ga, "%s/");

    GA_CONCAT_LITERAL(&ga, "\\V");
    إذا (flags & FRD_میل_CASE)
	GA_CONCAT_LITERAL(&ga, "\\C");
    وإلا
	GA_CONCAT_LITERAL(&ga, "\\c");
    إذا (flags & FRD_سارا_لفظ)
	GA_CONCAT_LITERAL(&ga, "\\<");
    // escape slash and backslash
    p = vim_strsave_escaped(لبھو_متن, (char_u *)"/\\");
    إذا (p != عدم)
	ga_concat(&ga, p);
    vim_free(p);
    إذا (flags & FRD_سارا_لفظ)
	GA_CONCAT_LITERAL(&ga, "\\>");

    إذا (type == FRD_REPLACEALL)
    {
	GA_CONCAT_LITERAL(&ga, "/");
	// Escape slash and backslash.
	// Also escape tilde and ampersand if 'magic' is set.
	p = vim_strsave_escaped(repl_متن,
				p_magic ? (char_u *)"/\\~&" : (char_u *)"/\\");
	إذا (p != عدم)
	    ga_concat(&ga, p);
	vim_free(p);
	GA_CONCAT_LITERAL(&ga, "/g");
    }
    ga_append(&ga, NUL);

    إذا (type == FRD_REPLACE)
    {
	// Do the replacement when the text at the cursor matches.  Thus no
	// replacement is done if the cursor was moved!
	regmatch.regprog = vim_regcomp(ga.ga_انکڑے, RE_MAGIC + RE_STRING);
	regmatch.rm_ic = 0;
	إذا (regmatch.regprog != عدم)
	{
	    p = ml_get_cursor();
	    إذا (vim_regexec_nl(&regmatch, p, (colnr_T)0)
						   && regmatch.startp[0] == p)
	    {
		// Clear the command line to remove any old "No match"
		// error.
		پیغام_سرا_prompt();

		إذا (u_save_cursor() == OK)
		{
		    // A button was pressed thus undo should be synced.
		    u_sync(FALSE);

		    del_بائٹاں((لما)(regmatch.endp[0] - regmatch.startp[0]),
								FALSE, FALSE);
		    ins_str(repl_متن, STRLEN(repl_متن));
		}
	    }
	    وإلا
		پیغام(_("No match at cursor, finding next"));
	    vim_regfree(regmatch.regprog);
	}
    }

    إذا (type == FRD_REPLACEALL)
    {
	// A button was pressed, thus undo should be synced.
	u_sync(FALSE);
	do_cmdline_cmd(ga.ga_انکڑے);
    }
    وإلا
    {
	صحيح searchflags = تلاش_پیغام + تلاش_MARK;

	// Search for the next match.
	// Don't skip text under cursor for single replace.
	إذا (type == FRD_REPLACE)
	    searchflags += تلاش_شروع;
	i = پیغام_scroll;
	إذا (تھلے)
	{
	    (فراغ)do_تلاش(عدم, '/', '/', ga.ga_انکڑے, STRLEN(ga.ga_انکڑے), 1L, searchflags, عدم);
	}
	وإلا
	{
	    // We need to escape '?' if and only if we are searching in the up
	    // direction
	    p = vim_strsave_escaped(ga.ga_انکڑے, (char_u *)"?");
	    إذا (p != عدم)
		(فراغ)do_تلاش(عدم, '?', '?', p, STRLEN(p), 1L, searchflags, عدم);
	    vim_free(p);
	}

	پیغام_scroll = i;	    // don't let an error message set msg_scroll
    }

    // Don't want to pass did_emsg to other code, it may cause disabling
    // syntax HL if we were busy redrawing.
    did_emsg = save_did_emsg;

    إذا (State & (منوال_عام | منوال_داخل_کرو))
    {
	gui_update_screen();		// update the screen
	پیغام_didout = 0;			// overwrite any message
	need_wait_return = FALSE;	// don't wait for return
    }

    vim_free(ga.ga_انکڑے);
    busy = FALSE;
    أرجع (ga.ga_len > 0);
}

#جے_انت

#إذا defined(HAVE_DROP_FILE)
/*
 * Jump to the window at specified point (x, y).
 */
    ساكن فراغ
gui_wingoto_xy(صحيح x, صحيح y)
{
    صحيح		صف = Y_2_صف(y);
    صحيح		خانہ = X_2_خانہ(x);
    win_T	*wp;

    إذا (صف < 0 || خانہ < 0)
	أرجع;

    wp = mouse_لبھو_win(&صف, &خانہ, FAIL_POPUP);
    إذا (wp != عدم && wp != curwin)
	win_goto(wp);
}

/*
 * Function passed to handle_drop() for the actions to be done after the
 * argument list has been updated.
 */
    ساكن فراغ
drop_callback(فراغ *cookie)
{
    char_u	*p = cookie;
    صحيح		do_shorten = FALSE;

    // If Shift held down, change to first file's directory.  If the first
    // item is a directory, change to that directory (and let the explorer
    // plugin show the contents).
    إذا (p != عدم)
    {
	إذا (mch_isdir(p))
	{
	    إذا (mch_chdir((حرف *)p) == 0)
		do_shorten = TRUE;
	}
	وإلا إذا (vim_chdirfile(p, "drop") == OK)
	    do_shorten = TRUE;
	vim_free(p);
	إذا (do_shorten)
	{
	    shorten_fnames(TRUE);
	    آخری_chdir_reason = "drop";
	}
    }

    // Update the screen display
    update_screen(UPD_NOT_VALID);
# ifdef FEAT_MENU
    gui_update_menus(0);
# endif
    maketitle();
    setcursor();
    باہرلا_flush_cursor(FALSE, FALSE);
}

/*
 * Process file drop.  Mouse cursor position, key modifiers, name of files
 * and count of files are given.  Argument "fnames[count]" has full pathnames
 * of dropped files, they will be freed in this function, and caller can't use
 * fnames after call this function.
 */
    فراغ
gui_handle_drop(
    صحيح		x بن_ورتیا,
    صحيح		y بن_ورتیا,
    int_u	modifiers,
    char_u	**fnames,
    صحيح		گنتی)
{
    صحيح		i;
    char_u	*p;
    ساكن صحيح	entered = FALSE;

    /*
     * This function is called by event handlers.  Just in case we get a
     * second event before the first one is handled, ignore the second one.
     * Not sure if this can ever happen, just in case.
     */
    إذا (entered)
	أرجع;
    entered = TRUE;

    /*
     * When the cursor is at the command line, add the file names to the
     * command line, don't edit the files.
     */
    إذا (State & منوال_CMDLINE)
    {
	shorten_filenames(fnames, گنتی);
	لكل (i = 0; i < گنتی; ++i)
	{
	    إذا (fnames[i] != عدم)
	    {
		إذا (i > 0)
		    جوڑو_منزل_داخلہ_buf((char_u*)" ", 1);

		// We don't know what command is used thus we can't be sure
		// about which characters need to be escaped.  Only escape the
		// most common ones.
# ifdef BACKSLASH_وچ_FILENAME
		p = vim_strsave_escaped(fnames[i], (char_u *)" \t\"|");
# وإلا
		p = vim_strsave_escaped(fnames[i], (char_u *)"\\ \t\"|");
# endif
		إذا (p != عدم)
		    جوڑو_منزل_داخلہ_buf_csi(p, (صحيح)STRLEN(p));
		vim_free(p);
		vim_free(fnames[i]);
	    }
	}
	vim_free(fnames);
    }
    وإلا
    {
	// Go to the window under mouse cursor, then shorten given "fnames" by
	// current window, because a window can have local current dir.
	gui_wingoto_xy(x, y);
	shorten_filenames(fnames, گنتی);

	// If Shift held down, remember the first item.
	إذا ((modifiers & MOUSE_کھسکاؤ) != 0)
	    p = vim_strsave(fnames[0]);
	وإلا
	    p = عدم;

	// Handle the drop, :edit or :split to get to the file.  This also
	// frees fnames[].  Skip this if there is only one item, it's a
	// directory and Shift is held down.
	إذا (گنتی == 1 && (modifiers & MOUSE_کھسکاؤ) != 0
						     && mch_isdir(fnames[0]))
	{
	    vim_free(fnames[0]);
	    vim_free(fnames);
	    vim_free(p);
	}
	وإلا
	    handle_drop(گنتی, fnames, (modifiers & MOUSE_CTRL) != 0,
						     drop_callback, (فراغ *)p);
    }

    entered = FALSE;
}
#جے_انت

/*
 * Check if "key" is to interrupt us.  Handles a key that has not had modifiers
 * applied yet.
 * Return the key with modifiers applied if so, NUL if not.
 */
    صحيح
check_for_رکاوٹ(صحيح کلید, صحيح modifiers_arg)
{
    صحيح modifiers = modifiers_arg;
    صحيح c = ملاؤ_modify_Other_کلیدیں(کلید, &modifiers);

    إذا ((c == Ctrl_C && ctrl_c_interrupts)
#جے_تعریف_شدہ UNIX
	    || (intr_char != Ctrl_C && c == intr_char)
#جے_انت
	    )
    {
	got_int = TRUE;
	أرجع c;
    }
    أرجع NUL;
}

/*
 * If the "--gui-log-file fname" argument is given write the dialog title and
 * message to a file and return TRUE.  Otherwise return FALSE.
 * When there is any problem opening the file or writing to the file this is
 * ignored, showing the dialog might get the test to get stuck.
 */
    صحيح
gui_dialog_log(char_u *title, char_u *سنیہا)
{
    char_u  *fname = get_gui_dialog_file();
    FILE    *fd;

    إذا (fname == عدم)
	أرجع FALSE;

    fd = mch_fopen((حرف *)fname, "a");
    إذا (fd != عدم)
    {
	fprintf(fd, "%s: %s\n", title, سنیہا);
	fclose(fd);
    }
    أرجع TRUE;
}
