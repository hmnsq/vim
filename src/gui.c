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
# define USE_سیٹ_GUIFONTWIDE
ساکن خالی سیٹ_guifontwide(char_u *font_name);
#اگر_ختم
ساکن خالی gui_check_pos(خالی);
ساکن خالی gui_reset_scroll_region(خالی);
ساکن خالی gui_outstr(char_u *, صحیح);
#اگر_غیر_تعریف_شدہ USE_GTK4
ساکن صحیح gui_screenchar(صحیح off, صحیح flags, guicolor_T fg, guicolor_T bg, صحیح عقب);
#اگر_ختم
ساکن صحیح gui_outstr_nowrap(char_u *s, صحیح len, صحیح flags, guicolor_T fg, guicolor_T bg, صحیح عقب);
ساکن خالی gui_delete_سطریں(صحیح صف, صحیح گنتی);
ساکن خالی gui_داخل_کرو_سطریں(صحیح صف, صحیح گنتی);
ساکن صحیح gui_xy2colrow(صحیح x, صحیح y, صحیح *colp);
#اگر defined(FEAT_GUI_TABLINE)
ساکن صحیح gui_has_tabline(خالی);
#اگر_ختم
ساکن خالی gui_do_scrollbar(win_T *wp, صحیح which, صحیح enable);
ساکن خالی gui_update_horiz_scrollbar(صحیح);
ساکن خالی gui_سیٹ_fg_color(char_u *name);
ساکن خالی gui_سیٹ_bg_color(char_u *name);
ساکن خالی init_gui_options(خالی);
ساکن win_T *xy2win(صحیح x, صحیح y, mouse_ڈھونڈو_T popup);

#اگر_تعریف_شدہ GUI_MAY_FORK
ساکن خالی gui_do_fork(خالی);

ساکن صحیح gui_read_بچہ_pipe(صحیح fd);

// Return values for gui_read_child_pipe
شمار {
    GUI_بچہ_IO_ERROR,
    GUI_بچہ_OK,
    GUI_بچہ_FAILED
};
#اگر_ختم

ساکن خالی gui_attempt_آغاز(خالی);

ساکن صحیح can_update_cursor = TRUE; // can display the cursor
ساکن صحیح disable_flush = 0;	// If > 0, gui_mch_flush() is disabled.

/*
 * gui_start -- Called when user wants to start the GUI.
 *
 * Careful: This function can be called recursively when there is a ":gui"
 * command in the .gvimrc file.  Only the first call should fork, not the
 * recursive call.
 */
    خالی
gui_آغاز(char_u *arg غیر_مستعمل)
{
    char_u	*old_رکن;
#اگر_تعریف_شدہ GUI_MAY_FORK
    ساکن صحیح	recursive = 0;
#اگر_ختم
#اگر defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
    حرف	*پیغام = تہی;
#اگر_ختم

    old_رکن = vim_strsave(T_NAME);

    settmode(TMODE_COOK);		// stop RAW mode
    اگر (بھرا_screen)
	cursor_on();			// needed for ":gui" in .vimrc
    بھرا_screen = FALSE;

    // If GUI fails to start, then we will recover afterwards
    رکن_disable_dec();

#اگر_تعریف_شدہ GUI_MAY_FORK
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
# ifdef FEAT_کام_CHANNEL
	    && !کام_any_running()
# endif
	    )
    {
	gui_do_fork();
    }
    ورنہ
#اگر_ختم
#اگر_تعریف_شدہ GUI_MAY_SPAWN
    اگر (gui.dospawn
# ifdef EXPERIMENTAL_GUI_CMD
	    && gui.dofork
# endif
	    && !vim_strchr(p_go, GO_FORG)
	    && !any_Buf_Is_بدلا()
# ifdef FEAT_کام_CHANNEL
	    && !کام_any_running()
# endif
	    )
    {
# ifdef EXPERIMENTAL_GUI_CMD
	پیغام =
# endif
	    gui_mch_do_spawn(arg);
    }
    ورنہ
#اگر_ختم
    {
#اگر_تعریف_شدہ FEAT_GUI_GTK
	// If there is 'f' in 'guioptions' and specify -g argument,
	// gui_mch_init_check() was not called yet.
	اگر (gui_mch_init_check() != OK)
	    getout_preserve_modified(1);
#اگر_ختم
	gui_attempt_آغاز();
    }

    اگر (!gui.اندر_use)			// failed to start GUI
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
#اگر_تعریف_شدہ UNIX
	رکن_سیٹ_win_resize(true);
#اگر_ختم
#اگر defined(GUI_MAY_SPAWN) && defined(EXPERIMENTAL_GUI_CMD)
	اگر (پیغام != تہی)
	    emsg(پیغام);
#اگر_ختم
    }
    ورنہ
    {
#اگر_تعریف_شدہ HAVE_CLIPMETHOD
	// Reset clipmethod to CLIPMETHOD_NONE
	انتخاب_clipmethod();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_IMAGE
	update_cell_سائز();
	update_image_backend();
#اگر_ختم
    }

#اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    // Enable fullscreen mode
    اگر (vim_strchr(p_go, GO_FULLSCREEN) != تہی)
       gui_mch_سیٹ_fullscreen(TRUE);
#اگر_ختم

    vim_free(old_رکن);

    // If the GUI started successfully, trigger the GUIEnter event, otherwise
    // the GUIFailed event.
    gui_mch_update();
    apply_autocmds(gui.اندر_use ? EVENT_GUIENTER : EVENT_GUIFAILED,
						   تہی, تہی, FALSE, curbuf);
#اگر_تعریف_شدہ GUI_MAY_FORK
    --recursive;
#اگر_ختم
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
    ساکن خالی
gui_attempt_آغاز(خالی)
{
    ساکن صحیح recursive = 0;

    ++recursive;
    gui.starting = true;

#اگر_تعریف_شدہ FEAT_GUI_GTK
    gui.event_وقت = GDK_CURRENT_وقت;
#اگر_ختم

    termcapinit((char_u *)"builtin_gui");
    gui.starting = recursive - 1;

#اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
    اگر (gui.اندر_use)
    {
# ifdef FEAT_EVAL
	کھڑکی	x11_کھڑکی;
	Display	*x11_display;

	اگر (gui_get_x11_windis(&x11_کھڑکی, &x11_display) == OK)
	    سیٹ_vim_var_nr(VV_WINDOWID, (طویل)x11_کھڑکی);
# endif
	// Display error messages in a dialog now.
	display_errors();
    }
#اگر_ختم
    --recursive;
}

#اگر_تعریف_شدہ GUI_MAY_FORK

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
    ساکن خالی
gui_do_fork(خالی)
{
    صحیح		pipefd[2];	// pipe between parent and child
    صحیح		pipe_error;
    صحیح		حالت_2;
    صحیح		exit_حالت;
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
	emsg(_(e_failed_مقصد_create_new_process_for_GUI));
	واپس;
    }
    ورنہ اگر (pid > 0)	    //والد
    {
	// Give the child some time to do the setsid(), otherwise the
	// exit() may kill the child too (when starting gvim from inside a
	// gvim).
	اگر (!pipe_error)
	{
	    // The read returns when the child closes the pipe (or when
	    // the child dies for some reason).
	    close(pipefd[1]);
	    حالت_2 = gui_read_بچہ_pipe(pipefd[0]);
	    اگر (حالت_2 == GUI_بچہ_FAILED)
	    {
		// The child failed to start the GUI, so the caller must
		// continue. There may be more error information written
		// to stderr by the child.
		waitpid(pid, &exit_حالت, 0);
		emsg(_(e_the_بچہ_process_failed_مقصد_آغاز_GUI));
		واپس;
	    }
	    ورنہ اگر (حالت_2 == GUI_بچہ_IO_ERROR)
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
    //بچہ

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
    (خالی)setsid();
#  ورنہ
    (خالی)setpgid(0, 0);
#  endif
# endif
    اگر (!pipe_error)
	close(pipefd[0]);

# اگر defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
    // Tell the session manager our new PID
    gui_mch_forked();
# endif

    // Try to start the GUI
    gui_attempt_آغاز();

    // Notify the parent
    اگر (!pipe_error)
    {
	اگر (gui.اندر_use)
	    write_eintr(pipefd[1], "ok", 3);
	ورنہ
	    write_eintr(pipefd[1], "fail", 5);
	close(pipefd[1]);
    }

    // If we failed to start the GUI, exit now.
    اگر (!gui.اندر_use)
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
    ساکن صحیح
gui_read_بچہ_pipe(صحیح fd)
{
    طویل	بائٹیں_read;
# define READ_بفر_سائز 10
    حرف	بفر[READ_بفر_سائز];

    بائٹیں_read = read_eintr(fd, بفر, READ_بفر_سائز - 1);
# undef READ_بفر_سائز
    close(fd);
    اگر (بائٹیں_read < 0)
	واپس GUI_بچہ_IO_ERROR;
    بفر[بائٹیں_read] = NUL;
    اگر (STRCMP(بفر, "ok") == 0)
	واپس GUI_بچہ_OK;
    واپس GUI_بچہ_FAILED;
}

#اگر_ختم // GUI_MAY_FORK

/*
 * Call this when vim starts up, whether or not the GUI is started
 */
    خالی
gui_prepare(صحیح *argc, حرف **argv)
{
    gui.اندر_use = false;		    // No GUI yet (maybe later)
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
gui_init_check(خالی)
{
    ساکن صحیح نتیجہ = MAYBE;

    اگر (نتیجہ != MAYBE)
    {
	اگر (نتیجہ == FAIL)
	    emsg(_(e_cannot_آغاز_the_GUI));
	واپس نتیجہ;
    }

    gui.shell_created = false;
    gui.dying = false;
    gui.اندر_focus = true;		// so the guicursor setting works
    gui.dragged_sb = SBAR_NONE;
    gui.dragged_wp = تہی;
    gui.اشارہ_گر_hidden = false;
    gui.ستون = 0;
    gui.صف = 0;
    gui.عدد_ستون_تعداد = Columns;
    gui.عدد_صفیں = صفیں;

    gui.cursor_is_valid = false;
    gui.scroll_region_چوٹی = 0;
    gui.scroll_region_bot = صفیں - 1;
    gui.scroll_region_بایاں = 0;
    gui.scroll_region_دایاں = Columns - 1;
    gui.highlight_نقاب = HL_معمول;
    gui.char_چوڑائی = 1;
    gui.char_اونچائی = 1;
    gui.char_ascent = 0;
    gui.border_چوڑائی = 0;

    gui.norm_font = NOFONT;
#اگر_غیر_تعریف_شدہ FEAT_GUI_GTK
    gui.bold_font = NOFONT;
    gui.ital_font = NOFONT;
    gui.boldital_font = NOFONT;
# ifdef FEAT_XFONTSET
    gui.fontset = NOFONTSET;
# endif
#اگر_ختم
    gui.wide_font = NOFONT;
#اگر_غیر_تعریف_شدہ FEAT_GUI_GTK
    gui.wide_bold_font = NOFONT;
    gui.wide_ital_font = NOFONT;
    gui.wide_boldital_font = NOFONT;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MENU
# ifndef FEAT_GUI_GTK
#  ifdef FONTSET_ALWAYS
    gui.menu_fontset = NOFONTSET;
#  ورنہ
    gui.menu_font = NOFONT;
#  endif
# endif
    gui.menu_is_فعال = true;	    // default: include menu
# ifndef FEAT_GUI_GTK
    gui.menu_اونچائی = MENU_DEFAULT_اونچائی;
    gui.menu_چوڑائی = 0;
# endif
#اگر_ختم
#اگر defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    gui.toolbar_اونچائی = 0;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_BEVAL_TIP
    gui.tooltip_fontset = NOFONTSET;
#اگر_ختم

    gui.scrollbar_چوڑائی = gui.scrollbar_اونچائی = SB_DEFAULT_چوڑائی;
    gui.پچھلا_wrap = -1;

#اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // Note: gui_set_ligatures() might already have been called e.g. from .vimrc,
    // and in that case we don't want to overwrite ligatures map that has already
    // been correctly populated (as that would lead to a cleared ligatures maps).
    اگر (*p_guiligatures == NUL)
	CLEAR_میدان(gui.ligatures_map);
#اگر_ختم

#اگر defined(ALWAYS_USE_GUI) || defined(VIMDLL)
    نتیجہ = OK;
#ورنہ
# ifdef FEAT_GUI_GTK
    gui.is_x11 = false;
#  ifdef FEAT_GUI_DIALOG
    gui.dialogs_فعال = 0;
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
# ورنہ
    نتیجہ = gui_mch_init_check();
# endif
#اگر_ختم
    واپس نتیجہ;
}

/*
 * This is the call which starts the GUI.
 */
    خالی
gui_init(خالی)
{
    win_T	*wp;
    tabpage_T	*tp;
    ساکن صحیح	recursive = 0;

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
	    واپس;
	}

	/*
	 * Reset 'paste'.  It's useful in the terminal, but not in the GUI.  It
	 * breaks the Paste toolbar button.
	 */
	سیٹ_option_قدر_give_err((char_u *)"paste", 0L, تہی, 0);

	// Set t_Co to the number of colors: RGB.
	سیٹ_color_گنتی(256 * 256 * 256);

	/*
	 * Set up system-wide default menus.
	 */
#اگر defined(SYS_MENU_FILE) && defined(FEAT_MENU)
	اگر (vim_strchr(p_go, GO_NOSYSMENU) == تہی)
	{
	    sys_menu = TRUE;
	    do_ماخذ((char_u *)SYS_MENU_FILE, FALSE, DOSO_NONE, تہی);
	    sys_menu = FALSE;
	}
#اگر_ختم

	/*
	 * Switch on the mouse by default, unless the user changed it already.
	 * This can then be changed in the .gvimrc.
	 */
	اگر (!option_was_سیٹ((char_u *)"mouse"))
	    سیٹ_string_option_براہ_راست((char_u *)"mouse", -1,
					   (char_u *)"a", OPT_FREE, SID_NONE);

	/*
	 * If -U option given, use only the initializations from that file and
	 * nothing else.  Skip all initializations for "-U NONE" or "-u NORC".
	 */
	اگر (use_gvimrc != تہی)
	{
	    اگر (STRCMP(use_gvimrc, "NONE") != 0
		    && STRCMP(use_gvimrc, "NORC") != 0
		    && do_ماخذ(use_gvimrc, FALSE, DOSO_NONE, تہی) != OK)
		semsg(_(e_cannot_read_from_str), use_gvimrc);
	}
	ورنہ
	{
	    /*
	     * Get system wide defaults for gvim, only when file name defined.
	     */
#اگر_تعریف_شدہ SYS_GVIMRC_FILE
	    do_ماخذ((char_u *)SYS_GVIMRC_FILE, FALSE, DOSO_NONE, تہی);
#اگر_ختم

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
		 && do_ماخذ((char_u *)USR_GVIMRC_FILE, TRUE,
						     DOSO_GVIMRC, تہی) == FAIL
#اگر_تعریف_شدہ USR_GVIMRC_FILE2
		 && do_ماخذ((char_u *)USR_GVIMRC_FILE2, TRUE,
						     DOSO_GVIMRC, تہی) == FAIL
#اگر_ختم
#اگر_تعریف_شدہ USR_GVIMRC_FILE3
		 && do_ماخذ((char_u *)USR_GVIMRC_FILE3, TRUE,
						     DOSO_GVIMRC, تہی) == FAIL
#اگر_ختم
				)
	    {
#اگر_تعریف_شدہ USR_GVIMRC_FILE4
		(خالی)do_ماخذ((char_u *)USR_GVIMRC_FILE4, TRUE,
							    DOSO_GVIMRC, تہی);
#اگر_ختم
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
#اگر_تعریف_شدہ UNIX
		{
		    stat_T s;

		    // if ".gvimrc" file is not owned by user, set 'secure'
		    //منوال
		    اگر (mch_stat(GVIMRC_FILE, &s) || s.st_uid != getuid())
			secure = p_secure;
		}
#ورنہ
		secure = p_secure;
#اگر_ختم

		اگر (       fullpathcmp((char_u *)USR_GVIMRC_FILE,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#اگر_تعریف_شدہ SYS_GVIMRC_FILE
			&& fullpathcmp((char_u *)SYS_GVIMRC_FILE,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#اگر_ختم
#اگر_تعریف_شدہ USR_GVIMRC_FILE2
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE2,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#اگر_ختم
#اگر_تعریف_شدہ USR_GVIMRC_FILE3
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE3,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#اگر_ختم
#اگر_تعریف_شدہ USR_GVIMRC_FILE4
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE4,
				(char_u *)GVIMRC_FILE, FALSE, TRUE) != FPC_SAME
#اگر_ختم
			)
		    do_ماخذ((char_u *)GVIMRC_FILE, TRUE, DOSO_GVIMRC, تہی);

		اگر (secure == 2)
		    need_wait_return = TRUE;
		secure = 0;
	    }
	}

	اگر (need_wait_return || پیغام_didany)
	    wait_return(TRUE);

	--recursive;
    }

    // If recursive call opened the shell, return here from the first call
    اگر (gui.اندر_use)
	واپس;

    /*
     * Create the GUI shell.
     */
    gui.اندر_use = true;		// Must be set after menus have been set up
#اگر defined(FEAT_GUI_GTK) && defined(FEAT_IMAGE)
    gui.scale = 1.0; // Default value
#اگر_ختم
    اگر (gui_mch_init() == FAIL)
	جاؤ error;

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
    اگر (gui.border_چوڑائی < 0)
	gui.border_چوڑائی = 0;

    /*
     * Set up the fonts.  First use a font specified with "-fn" or "-font".
     */
    اگر (font_دلیل != تہی)
	سیٹ_option_قدر_give_err((char_u *)"gfn",
					       0L, (char_u *)font_دلیل, 0);
    اگر (
#اگر_تعریف_شدہ FEAT_XFONTSET
	    (*p_guifontset == NUL
	     || gui_init_font(p_guifontset, TRUE) == FAIL) &&
#اگر_ختم
	    gui_init_font(*p_guifont == NUL ? hl_get_font_name()
						  : p_guifont, FALSE) == FAIL)
    {
	emsg(_(e_cannot_آغاز_gui_no_valid_font_ملا));
	جاؤ error2;
    }
    اگر (gui_get_wide_font() == FAIL)
	emsg(_(e_guifontwide_invalid));

    gui.عدد_ستون_تعداد = Columns;
    gui.عدد_صفیں = صفیں;
    gui_reset_scroll_region();

    // Create initial scrollbars
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_بایاں], SBAR_بایاں, wp);
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_دایاں], SBAR_دایاں, wp);
    }
    gui_create_scrollbar(&gui.bottom_sbar, SBAR_BOTTOM, تہی);

#اگر_تعریف_شدہ FEAT_MENU
    gui_create_initial_menus(جڑ_menu);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SIGN_ICONS
    sign_gui_started();
#اگر_ختم

    // Configure the desired menu and scrollbars
    gui_init_which_components(تہی);

    // All components of the GUI have been created now
    gui.shell_created = true;

#اگر_تعریف_شدہ FEAT_GUI_MSWIN
    // Set the shell size, adjusted for the screen size.  For GTK this only
    // works after the shell has been opened, thus it is further down.
    // If the window is already maximized (e.g. when --windowid is passed in),
    // we want to use the system-provided dimensions by passing FALSE to
    // mustset. Otherwise, we want to initialize with the default rows/columns.
    اگر (gui_mch_maximized())
	gui_سیٹ_shellsize(FALSE, TRUE, RESIZE_BOTH);
    ورنہ
	gui_سیٹ_shellsize(TRUE, TRUE, RESIZE_BOTH);
#ورنہ
# ifndef FEAT_GUI_GTK
    gui_سیٹ_shellsize(FALSE, TRUE, RESIZE_BOTH);
# endif
#اگر_ختم
#اگر defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    // Need to set the size of the menubar after all the menus have been
    // created.
    gui_mch_compute_menu_اونچائی((Widget)0);
#اگر_ختم

    /*
     * Actually open the GUI shell.
     */
    اگر (gui_mch_open() != FAIL)
    {
	maketitle();
	resettitle();

	init_gui_options();
#اگر_تعریف_شدہ FEAT_ARABIC
	// Our GUI can't do bidi.
	p_tbidi = FALSE;
#اگر_ختم
#اگر defined(FEAT_GUI_GTK)
	// Give GTK+ a chance to put all widget's into place.
	gui_mch_update();

# ifdef FEAT_MENU
	// If there is no 'm' in 'guioptions' we need to remove the menu now.
	// It was still there to make F10 work.
	اگر (vim_strchr(p_go, GO_MENUS) == تہی)
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
	    gui_سیٹ_shellsize(FALSE, TRUE, RESIZE_BOTH);
	ورنہ
	    gui_سیٹ_shellsize(TRUE, TRUE, RESIZE_BOTH);
#اگر_ختم
	// When 'lines' was set while starting up the topframe may have to be
	// resized.
	win_new_shellsize();

#اگر_تعریف_شدہ FEAT_BEVAL_GUI
	// Always create the Balloon Evaluation area, but disable it when
	// 'ballooneval' is off.
	اگر (balloonEval != تہی)
	{
# ifdef FEAT_VARTABS
	    vim_free(balloonEval->vts);
# endif
	    vim_free(balloonEval);
	}
	balloon_Eval_For_رکن = FALSE;
# ifdef FEAT_GUI_GTK
	balloonEval = gui_mch_create_beval_area(gui.drawarea, تہی,
						     &general_beval_cb, تہی);
# ورنہ
#  اگر defined(FEAT_GUI_MOTIF)
	{
	    خارجی Widget	متن_Area;
	    balloonEval = gui_mch_create_beval_area(متن_Area, تہی,
						     &general_beval_cb, تہی);
	}
#  ورنہ
#   ifdef FEAT_GUI_MSWIN
	balloonEval = gui_mch_create_beval_area(تہی, تہی,
						     &general_beval_cb, تہی);
#   endif
#  endif
# endif
	اگر (!p_beval)
	    gui_mch_disable_beval_area(balloonEval);
#اگر_ختم

#اگر_غیر_تعریف_شدہ FEAT_GUI_MSWIN
	// In the GUI modifiers are prepended to keys.
	// Don't do this for MS-Windows yet, it sends CTRL-K without the
	// modifier.
	نظر_آیا_Modify_Other_کلیدیں = TRUE;
#اگر_ختم

#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	اگر (!im_xim_isvalid_imactivate())
	    emsg(_(e_قدر_کا_imactivatekey_is_invalid));
#اگر_ختم
	// When 'cmdheight' was set during startup it may not have taken
	// effect yet.
	اگر (p_ch != 1L)
	    command_اونچائی();

	واپس;
    }

error2:
#اگر_تعریف_شدہ FEAT_GUI_X11
    // undo gui_mch_init()
    gui_mch_uninit();
#اگر_ختم

error:
    gui.اندر_use = false;
    clip_init(FALSE);
}


    خالی
gui_exit(صحیح rc)
{
    // don't free the fonts, it leads to a BUS error
    // richard@whitequeen.com Jul 99
    free_highlight_fonts();
    gui.اندر_use = false;
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
    خالی
gui_shell_closed(خالی)
{
    cmdmod_T	    save_cmdmod = cmdmod;

    اگر (before_quit_autocmds(curwin, TRUE, FALSE))
	واپس;

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
    اگر (!check_بدلا_any(FALSE, FALSE))
	getout(0);

    exiting = FALSE;
    cmdmod = save_cmdmod;
    gui_update_screen();	// redraw, window may show changed buffer
}
#اگر_ختم

/*
 * Set the font.  "font_list" is a comma separated list of font names.  The
 * first font name that works is used.  If none is found, use the default
 * font.
 * If "fontset" is TRUE, the "font_list" is used as one name for the fontset.
 * Return OK when able to set the font.  When it failed FAIL is returned and
 * the fonts are unchanged.
 */
    صحیح
gui_init_font(char_u *font_list, صحیح fontset غیر_مستعمل)
{
#تعریف_کرو FONTLEN 320
    char_u	font_name[FONTLEN];
    صحیح		font_list_خالی_خانے = FALSE;
    صحیح		ret = FAIL;

    اگر (!gui.اندر_use)
	واپس FAIL;

    font_name[0] = NUL;
    اگر (*font_list == NUL)
	font_list_خالی_خانے = TRUE;
    ورنہ
    {
#اگر_تعریف_شدہ FEAT_XFONTSET
	// When using a fontset, the whole list of fonts is one name.
	اگر (fontset)
	    ret = gui_mch_init_font(font_list, TRUE);
	ورنہ
#اگر_ختم
	    جب (*font_list != NUL)
	    {
		// Isolate one comma separated font name.
		(خالی)copy_option_part(&font_list, font_name, FONTLEN, ",");

		// Careful!!!  The Win32 version of gui_mch_init_font(), when
		// called with "*" will change p_guifont to the selected font
		// name, which frees the old value.  This makes font_list
		// invalid.  Thus when OK is returned here, font_list must no
		// longer be used!
		اگر (gui_mch_init_font(font_name, FALSE) == OK)
		{
#اگر_تعریف_شدہ USE_سیٹ_GUIFONTWIDE
		    // If it's a Unicode font, try setting 'guifontwide' to a
		    // similar double-width font.
		    اگر ((p_guifontwide == تہی || *p_guifontwide == NUL)
				&& لڑی_تلاش((حرف *)font_name, "10646") != تہی)
			سیٹ_guifontwide(font_name);
#اگر_ختم
		    ret = OK;
		    توڑو;
		}
	    }
    }

    اگر (ret != OK
	    && STRCMP(font_list, "*") != 0
	    && (font_list_خالی_خانے || gui.norm_font == NOFONT))
    {
	/*
	 * Couldn't load any font in 'font_list', keep the current font if
	 * there is one.  If 'font_list' is empty, or if there is no current
	 * font, tell gui_mch_init_font() to try to find a font we can load.
	 */
	ret = gui_mch_init_font(تہی, FALSE);
    }

    اگر (ret == OK)
    {
#اگر_غیر_تعریف_شدہ FEAT_GUI_GTK
	// Set normal font as current font
# ifdef FEAT_XFONTSET
	اگر (gui.fontset != NOFONTSET)
	    gui_mch_سیٹ_fontset(gui.fontset);
	ورنہ
# endif
	    gui_mch_سیٹ_font(gui.norm_font);
#اگر_ختم
	gui_سیٹ_shellsize(FALSE, TRUE, RESIZE_BOTH);
    }

    واپس ret;
}

#اگر_تعریف_شدہ USE_سیٹ_GUIFONTWIDE
/*
 * Try setting 'guifontwide' to a font twice as wide as "name".
 */
    ساکن خالی
سیٹ_guifontwide(char_u *name)
{
    صحیح		i = 0;
    char_u	wide_name[FONTLEN + 10]; // room for 2 * width and '*'
    char_u	*wp = تہی;
    char_u	*p;
    GuiFont	font;

    wp = wide_name;
    برائے (p = name; *p != NUL; ++p)
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
	    ورنہ اگر (i == 12)	// found the width
	    {
		++p;
		i = getdigits(&p);
		اگر (i != 0)
		{
		    // Double the width specification.
		    sprintf((حرف *)wp, "%d%s", i * 2, p);
		    font = gui_mch_get_font(wide_name, FALSE);
		    اگر (font != NOFONT)
		    {
			gui_mch_free_font(gui.wide_font);
			gui.wide_font = font;
			سیٹ_string_option_براہ_راست((char_u *)"gfw", -1,
						      wide_name, OPT_FREE, 0);
		    }
		}
		توڑو;
	    }
	}
    }
}
#اگر_ختم

/*
 * Get the font for 'guifontwide'.
 * Return FAIL for an invalid font name.
 */
    صحیح
gui_get_wide_font(خالی)
{
    GuiFont	font = NOFONT;
    char_u	font_name[FONTLEN];
    char_u	*p;

    اگر (!gui.اندر_use)	    // Can't allocate font yet, assume it's OK.
	واپس OK;	    // Will give an error message later.

    اگر (p_guifontwide != تہی && *p_guifontwide != NUL)
    {
	برائے (p = p_guifontwide; *p != NUL; )
	{
	    // Isolate one comma separated font name.
	    (خالی)copy_option_part(&p, font_name, FONTLEN, ",");
	    font = gui_mch_get_font(font_name, FALSE);
	    اگر (font != NOFONT)
		توڑو;
	}
	اگر (font == NOFONT)
	    واپس FAIL;
    }

    gui_mch_free_font(gui.wide_font);
#اگر_تعریف_شدہ FEAT_GUI_GTK
    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    اگر (font != NOFONT && gui.norm_font != NOFONT
			 && pango_font_description_equal(font, gui.norm_font))
    {
	gui.wide_font = NOFONT;
	gui_mch_free_font(font);
    }
    ورنہ
#اگر_ختم
	gui.wide_font = font;
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
    gui_mch_wide_font_بدلا();
#ورنہ
    /*
     * TODO: setup wide_bold_font, wide_ital_font and wide_boldital_font to
     * support those fonts for 'guifontwide'.
     */
#اگر_ختم
    واپس OK;
}

#اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * Set list of ascii characters that combined can create ligature.
 * Store them in char map for quick access from gui_gtk_draw_string.
 */
    خالی
gui_سیٹ_ligatures(خالی)
{
    char_u	*p;

    اگر (*p_guiligatures != NUL)
    {
	// check for invalid characters
	برائے (p = p_guiligatures; *p != NUL; ++p)
	    اگر (*p < 32 || *p > 127)
	    {
		emsg(_(e_ascii_رمز_not_اندر_range));
		واپس;
	    }

	// store valid setting into ligatures_map
	CLEAR_میدان(gui.ligatures_map);
	برائے (p = p_guiligatures; *p != NUL; ++p)
	    gui.ligatures_map[*p] = 1;
    }
    ورنہ
	CLEAR_میدان(gui.ligatures_map);
}

/*
 * Adjust the columns to undraw for when the cursor is on ligatures.
 */
    ساکن خالی
gui_adjust_undraw_cursor_for_ligatures(صحیح *startcol, صحیح *endcol)
{
    صحیح off;

    اگر (Screen_سطریں == تہی || *p_guiligatures == NUL)
	واپس;

    // expand before the cursor for all the chars in gui.ligatures_map
    off = سطر_انحراف[gui.cursor_صف] + *startcol;
    اگر (gui.ligatures_map[Screen_سطریں[off]])
	جب (*startcol > 0 && gui.ligatures_map[Screen_سطریں[--off]])
	    (*startcol)--;

    // expand after the cursor for all the chars in gui.ligatures_map
    off = سطر_انحراف[gui.cursor_صف] + *endcol;
    اگر (gui.ligatures_map[Screen_سطریں[off]])
	جب (*endcol < ((صحیح)screen_Columns - 1)
				      && gui.ligatures_map[Screen_سطریں[++off]])
	   (*endcol)++;
}
#اگر_ختم

    ساکن خالی
gui_سیٹ_cursor(صحیح صف, صحیح ستون)
{
    gui.صف = صف;
    gui.ستون = ستون;
}

/*
 * gui_check_pos - check if the cursor is on the screen.
 */
    ساکن خالی
gui_check_pos(خالی)
{
    اگر (gui.صف >= screen_صفیں)
	gui.صف = screen_صفیں - 1;
    اگر (gui.ستون >= screen_Columns)
	gui.ستون = screen_Columns - 1;
    اگر (gui.cursor_صف >= screen_صفیں || gui.cursor_ستون >= screen_Columns)
	gui.cursor_is_valid = false;
}

/*
 * Redraw the cursor if necessary or when forced.
 * Careful: The contents of ScreenLines[] must match what is on the screen,
 * otherwise this goes wrong.  May need to call out_flush() first.
 */
    خالی
gui_update_cursor(
    صحیح		force,		 // when TRUE, update even when not moved
    صحیح		clear_selection) // clear selection under cursor
{
    صحیح		cur_چوڑائی = 0;
    صحیح		cur_اونچائی = 0;
    صحیح		old_hl_نقاب;
    cursorentry_T *shape;
    صحیح		id;
#اگر_تعریف_شدہ FEAT_TERMINAL
    guicolor_T	shape_fg = INVALCOLOR;
    guicolor_T	shape_bg = INVALCOLOR;
#اگر_ختم
    guicolor_T	cfg, cbg, cc;	// cursor fore-/background color
    صحیح		cattr;		// cursor attributes
    صحیح		attr;
    attrentry_T *aep = تہی;
#اگر (defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)) && !defined(USE_GTK4)
    bool	lig_بایاں = false, lig_دایاں = false;
#اگر_ختم

    // Don't update the cursor when halfway busy scrolling or the screen size
    // doesn't match 'columns' and 'lines.  ScreenLines[] isn't valid then.
    اگر (!can_update_cursor || screen_Columns != gui.عدد_ستون_تعداد
					       || screen_صفیں != gui.عدد_صفیں)
	واپس;

    gui_check_pos();

    اگر (gui.cursor_is_valid && !force
		&& gui.صف == gui.cursor_صف && gui.ستون == gui.cursor_ستون)
	واپس;

    gui_undraw_cursor();

    // If a cursor-less sleep is ongoing, leave the cursor invisible
    اگر (cursor_is_sleeping())
	واپس;

    اگر (gui.صف < 0)
	واپس;
#اگر_تعریف_شدہ HAVE_داخلہ_طریقہ
    اگر (gui.صف != gui.cursor_صف || gui.ستون != gui.cursor_ستون)
	im_سیٹ_position(gui.صف, gui.ستون);
#اگر_ختم
    gui.cursor_صف = gui.صف;
    gui.cursor_ستون = gui.ستون;

    // Only write to the screen after ScreenLines[] has been initialized
    اگر (!screen_cleared || Screen_سطریں == تہی)
	واپس;

    // Clear the selection if we are about to write over it
    اگر (clear_selection)
	clip_may_clear_selection(gui.صف, gui.صف);
    // Check that the cursor is inside the shell (resizing may have made
    // it invalid)
    اگر (gui.صف >= screen_صفیں || gui.ستون >= screen_Columns)
	واپس;

    gui.cursor_is_valid = true;

    /*
     * How the cursor is drawn depends on the current mode.
     * When in a terminal window use the shape/color specified there.
     */
#اگر_تعریف_شدہ FEAT_TERMINAL
    اگر (terminal_is_فعال())
	shape = رکن_get_cursor_shape(&shape_fg, &shape_bg);
    ورنہ
#اگر_ختم
	shape = &shape_جدول[get_shape_idx(FALSE)];
    اگر (State & منوال_LANGMAP)
	id = shape->id_lm;
    ورنہ
	id = shape->id;

    // get the colors and attributes for the cursor.  Default is inverted
    cfg = INVALCOLOR;
    cbg = INVALCOLOR;
    cattr = HL_INVERSE;
    gui_mch_سیٹ_blinking(shape->blinkwait,
	    shape->blinkon,
	    shape->blinkoff);
    اگر (shape->blinkwait == 0 || shape->blinkon == 0
	    || shape->blinkoff == 0)
	gui_mch_stop_blink(FALSE);
#اگر_تعریف_شدہ FEAT_TERMINAL
    اگر (shape_bg != INVALCOLOR)
    {
	cattr = 0;
	cfg = shape_fg;
	cbg = shape_bg;
    }
    ورنہ
#اگر_ختم
	اگر (id > 0)
	{
	    cattr = syn_id2colors(id, &cfg, &cbg);
#اگر defined(HAVE_داخلہ_طریقہ)
	    {
		ساکن صحیح iid;
		guicolor_T fg, bg;

		اگر (
# اگر defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
			preedit_get_حالت()
# ورنہ
			im_get_حالت()
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
#اگر_ختم
	}

    /*
     * Get the attributes for the character under the cursor.
     * When no cursor color was given, use the character color.
     */
    attr = ScreenAttrs[سطر_انحراف[gui.صف] + gui.ستون];
    اگر (attr > HL_ALL)
	aep = syn_gui_attr2entry(attr);
    اگر (aep != تہی)
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
	cfg = (attr & HL_INVERSE) ? gui.عقب_pixel : gui.norm_pixel;
    اگر (cbg == INVALCOLOR)
	cbg = (attr & HL_INVERSE) ? gui.norm_pixel : gui.عقب_pixel;

#اگر_تعریف_شدہ FEAT_XIM
    اگر (aep != تہی)
    {
	xim_bg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.fg_color
		: aep->ae_u.gui.bg_color);
	xim_fg_color = ((attr & HL_INVERSE) ? aep->ae_u.gui.bg_color
		: aep->ae_u.gui.fg_color);
	اگر (xim_bg_color == INVALCOLOR)
	    xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
		: gui.عقب_pixel;
	اگر (xim_fg_color == INVALCOLOR)
	    xim_fg_color = (attr & HL_INVERSE) ? gui.عقب_pixel
		: gui.norm_pixel;
    }
    ورنہ
    {
	xim_bg_color = (attr & HL_INVERSE) ? gui.norm_pixel
	    : gui.عقب_pixel;
	xim_fg_color = (attr & HL_INVERSE) ? gui.عقب_pixel
	    : gui.norm_pixel;
    }
#اگر_ختم

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
    اگر (!gui.اندر_focus)
    {
#اگر_تعریف_شدہ USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, -1, -1);
#ورنہ
	gui_mch_draw_hollow_cursor(cbg);
#اگر_ختم
	واپس;
    }

#اگر_تعریف_شدہ USE_GTK4
    // Make sure that character underneath is drawn again in case it is part of
    // a ligature.
    gui_redraw_بلاک(gui.صف, gui.ستون, gui.صف, gui.ستون, GUI_MON_NOCLEAR);
    // gui_redraw_block() will invalidate the cursor
    gui.cursor_is_valid = true;
#اگر_ختم
#اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    // If cursor is in the middle of a ligature, then split the ligature at
    // the cursor boundaries by redrawing those cells again.
    برائے (صحیح c = gui.ستون - 1; c >= 0; c--)
	اگر (!gui.ligatures_map[Screen_سطریں[سطر_انحراف[gui.صف] + c]])
	{
	    اگر (c < gui.ستون - 1)
	    {
		gui_redraw_بلاک(gui.صف, c + 1, gui.صف, gui.ستون - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_بایاں = true;
# endif
	    }
	    توڑو;
	}

    برائے (صحیح c = gui.ستون + 1; c < screen_Columns - 1; c++)
	اگر (!gui.ligatures_map[Screen_سطریں[سطر_انحراف[gui.صف] + c]])
	{
	    اگر (c > gui.ستون + 1)
	    {
		gui_redraw_بلاک(gui.صف, gui.ستون + 1, gui.صف, c - 1,
			GUI_MON_NOCLEAR);
# ifndef USE_GTK4
		lig_دایاں = true;
# endif
	    }
	    توڑو;
	}

# ifndef USE_GTK4
    اگر ((lig_بایاں || lig_دایاں) && shape->shape != SHAPE_بلاک)
    {
	// Because the cursor is not drawn with gui_screenchar(), must blit the
	// cell again (with its background), so that old ligature does not
	// remain on screen. Not needed for GtkSnapshot, because char beneath is
	// always rerendered (see above ifdef).
	صحیح old = gui.ستون;
	gui.highlight_نقاب = ScreenAttrs[سطر_انحراف[gui.صف] + gui.ستون];
	(خالی)gui_screenchar(سطر_انحراف[gui.صف] + gui.ستون,
		GUI_MON_NOCLEAR, (guicolor_T)0, (guicolor_T)0, 0);
	gui.ستون = old;
    }
# endif
    // gui_redraw_block()/gui_screenchar() may invalidate the cursor, make sure
    // to validate it again.
    gui.cursor_is_valid = true;
#اگر_ختم

    old_hl_نقاب = gui.highlight_نقاب;
    اگر (shape->shape == SHAPE_بلاک)
    {
#اگر_تعریف_شدہ USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, 0, 0);
#ورنہ
	/*
	 * Draw the text character with the cursor colors.	Use the
	 * character attributes plus the cursor attributes.
	 */
	gui.highlight_نقاب = (cattr | attr);
	(خالی)gui_screenchar(سطر_انحراف[gui.صف] + gui.ستون,
		GUI_MON_IS_CURSOR | GUI_MON_NOCLEAR, cfg, cbg, 0);
#اگر_ختم
    }
    ورنہ
    {
#اگر defined(FEAT_RIGHTLEFT)
	صحیح	    ستون_off = FALSE;
#اگر_ختم
	/*
	 * First draw the partial cursor, then overwrite with the text
	 * character, using a transparent background.
	 */
	اگر (shape->shape == SHAPE_VER)
	{
	    cur_اونچائی = gui.char_اونچائی;
	    cur_چوڑائی = (gui.char_چوڑائی * shape->percentage + 99) / 100;
	}
	ورنہ
	{
	    cur_اونچائی = (gui.char_اونچائی * shape->percentage + 99) / 100;
	    cur_چوڑائی = gui.char_چوڑائی;
	}
	اگر (has_mbyte && (*mb_off2cells)(سطر_انحراف[gui.صف] + gui.ستون,
		    سطر_انحراف[gui.صف] + screen_Columns) > 1)
	{
	    // Double wide character.
	    اگر (shape->shape != SHAPE_VER)
		cur_چوڑائی += gui.char_چوڑائی;
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	    اگر (CURSOR_BAR_دایاں)
	    {
		// gui.col points to the left half of the character but
		// the vertical line needs to be on the right half.
		// A double-wide horizontal line is also drawn from the
		// right half in gui_mch_draw_part_cursor().
		ستون_off = TRUE;
		++gui.ستون;
	    }
#اگر_ختم
	}
#اگر_تعریف_شدہ USE_GTK4
	gui_gtk4_draw_cursor(cbg, cfg, cur_چوڑائی, cur_اونچائی);
#ورنہ
	gui_mch_draw_part_cursor(cur_چوڑائی, cur_اونچائی, cbg);
#اگر_ختم
#اگر defined(FEAT_RIGHTLEFT)
	اگر (ستون_off)
	    --gui.ستون;
#اگر_ختم

	// Doesn't seem to work for MSWindows. We call gui_redraw_block() above
	// for GtkSnapshot.
#اگر !defined(FEAT_GUI_MSWIN) && !defined(USE_GTK4)
	gui.highlight_نقاب = ScreenAttrs[سطر_انحراف[gui.صف] + gui.ستون];
	(خالی)gui_screenchar(سطر_انحراف[gui.صف] + gui.ستون,
		GUI_MON_TRS_CURSOR | GUI_MON_NOCLEAR,
		(guicolor_T)0, (guicolor_T)0, 0);
#اگر_ختم
    }
    gui.highlight_نقاب = old_hl_نقاب;
}

#اگر defined(FEAT_MENU)
    ساکن خالی
gui_position_menu(خالی)
{
# اگر !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
    اگر (gui.menu_is_فعال && gui.اندر_use)
	gui_mch_سیٹ_menu_pos(0, 0, gui.menu_چوڑائی, gui.menu_اونچائی);
# endif
}
#اگر_ختم

/*
 * Position the various GUI components (text area, menu).  The vertical
 * scrollbars are NOT handled here.  See gui_update_scrollbars().
 */
    ساکن خالی
gui_position_components(صحیح کل_چوڑائی غیر_مستعمل)
{
    صحیح	    متن_area_x;
    صحیح	    متن_area_y;
    صحیح	    متن_area_چوڑائی;
    صحیح	    متن_area_اونچائی;

    // avoid that moving components around generates events
    ++hold_gui_events;

    متن_area_x = 0;
    اگر (gui.which_scrollbars[SBAR_بایاں])
	متن_area_x += gui.scrollbar_چوڑائی;

    متن_area_y = 0;
#اگر defined(FEAT_MENU) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    gui.menu_چوڑائی = کل_چوڑائی;
    اگر (gui.menu_is_فعال)
	متن_area_y += gui.menu_اونچائی;
#اگر_ختم

#اگر defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF))
    اگر (gui_has_tabline())
	متن_area_y += gui.tabline_اونچائی;
#اگر_ختم

#اگر defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    اگر (vim_strchr(p_go, GO_TOOLBAR) != تہی)
    {
# اگر defined(FEAT_GUI_HAIKU)
	gui_mch_سیٹ_toolbar_pos(0, متن_area_y,
				gui.menu_چوڑائی, gui.toolbar_اونچائی);
# endif
	متن_area_y += gui.toolbar_اونچائی;
    }
#اگر_ختم

#اگر defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_HAIKU)
    gui_mch_سیٹ_tabline_pos(0, متن_area_y,
    gui.menu_چوڑائی, gui.tabline_اونچائی);
    اگر (gui_has_tabline())
	متن_area_y += gui.tabline_اونچائی;
#اگر_ختم

    متن_area_چوڑائی = gui.عدد_ستون_تعداد * gui.char_چوڑائی + gui.border_انحراف * 2;
    متن_area_اونچائی = gui.عدد_صفیں * gui.char_اونچائی + gui.border_انحراف * 2;

    gui_mch_سیٹ_متن_area_pos(متن_area_x,
			      متن_area_y,
			      متن_area_چوڑائی,
			      متن_area_اونچائی
#اگر defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
				  + xim_get_حالت_area_اونچائی()
#اگر_ختم
			      );
#اگر_تعریف_شدہ FEAT_MENU
    gui_position_menu();
#اگر_ختم
    اگر (gui.which_scrollbars[SBAR_BOTTOM])
	gui_mch_سیٹ_scrollbar_pos(&gui.bottom_sbar,
				  متن_area_x,
				  متن_area_y + متن_area_اونچائی
					+ gui_mch_get_scrollbar_ypadding(),
				  متن_area_چوڑائی,
				  gui.scrollbar_اونچائی);
    gui.بایاں_sbar_x = 0;
    gui.دایاں_sbar_x = متن_area_x + متن_area_چوڑائی
					+ gui_mch_get_scrollbar_xpadding();

    --hold_gui_events;
}

/*
 * Get the width of the widgets and decorations to the side of the text area.
 */
    صحیح
gui_get_بنیاد_چوڑائی(خالی)
{
    صحیح	    بنیاد_چوڑائی;

    بنیاد_چوڑائی = 2 * gui.border_انحراف;
    اگر (gui.which_scrollbars[SBAR_بایاں])
	بنیاد_چوڑائی += gui.scrollbar_چوڑائی;
    اگر (gui.which_scrollbars[SBAR_دایاں])
	بنیاد_چوڑائی += gui.scrollbar_چوڑائی;
    واپس بنیاد_چوڑائی;
}

/*
 * Get the height of the widgets and decorations above and below the text area.
 */
    صحیح
gui_get_بنیاد_اونچائی(خالی)
{
    صحیح	    بنیاد_اونچائی;

    بنیاد_اونچائی = 2 * gui.border_انحراف;
    اگر (gui.which_scrollbars[SBAR_BOTTOM])
	بنیاد_اونچائی += gui.scrollbar_اونچائی;
#اگر_تعریف_شدہ FEAT_GUI_GTK
    // We can't take the sizes properly into account until anything is
    // realized.  Therefore we recalculate all the values here just before
    // setting the size. (--mdcki)
#ورنہ
# ifdef FEAT_MENU
    اگر (gui.menu_is_فعال)
	بنیاد_اونچائی += gui.menu_اونچائی;
# endif
# ifdef FEAT_TOOLBAR
    اگر (vim_strchr(p_go, GO_TOOLBAR) != تہی)
	بنیاد_اونچائی += gui.toolbar_اونچائی;
# endif
# اگر defined(FEAT_GUI_TABLINE) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU))
    اگر (gui_has_tabline())
	بنیاد_اونچائی += gui.tabline_اونچائی;
# endif
# اگر defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    بنیاد_اونچائی += gui_mch_متن_area_extra_اونچائی();
# endif
#اگر_ختم
    واپس بنیاد_اونچائی;
}

/*
 * Should be called after the GUI shell has been resized.  Its arguments are
 * the new width and height of the shell in pixels.
 */
    خالی
gui_resize_shell(صحیح pixel_چوڑائی, صحیح pixel_اونچائی)
{
    ساکن صحیح	busy = FALSE;

    اگر (!gui.shell_created)	    // ignore when still initializing
	واپس;

    /*
     * Can't resize the screen while it is being redrawn.  Remember the new
     * size and handle it later.
     */
    اگر (updating_screen || busy)
    {
	new_pixel_چوڑائی = pixel_چوڑائی;
	new_pixel_اونچائی = pixel_اونچائی;
	واپس;
    }

again:
    new_pixel_چوڑائی = 0;
    new_pixel_اونچائی = 0;
    busy = TRUE;

#اگر_تعریف_شدہ FEAT_GUI_HAIKU
    vim_تالا_screen();
#اگر_ختم

    // Flush pending output before redrawing
    برآمد_flush();
#اگر defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk_init_decor_اونچائی();
#اگر_ختم

    gui.عدد_ستون_تعداد = (pixel_چوڑائی - gui_get_بنیاد_چوڑائی()) / gui.char_چوڑائی;
    gui.عدد_صفیں = (pixel_اونچائی - gui_get_بنیاد_اونچائی()) / gui.char_اونچائی;

#اگر_تعریف_شدہ USE_GTK4
    gui_gtk4_update_سائز();
#اگر_ختم

    gui_position_components(pixel_چوڑائی);
    gui_reset_scroll_region();

#اگر defined(FEAT_GUI_GTK) && defined(USE_GTK4) && !defined(USE_GTK4)
    // We do not resize the draw area via the "resize" signal. This is because
    // when the window is resized, the form widget is the one that is resized,
    // so let that call gui_resize_shell() which will allocate the surface and
    // allocate the drawing area size/position.
    gui_gtk4_resize(pixel_چوڑائی, pixel_اونچائی);
#اگر_ختم

    /*
     * At the "more" and ":confirm" prompt there is no redraw, put the cursor
     * at the last line here (why does it have to be one row too low?).
     */
    اگر (State == منوال_ASKMORE || State == منوال_CONFIRM)
	gui.صف = gui.عدد_صفیں;

    // Only comparing Rows and Columns may be sufficient, but let's stay on
    // the safe side.
    اگر (gui.عدد_صفیں != screen_صفیں || gui.عدد_ستون_تعداد != screen_Columns
	    || gui.عدد_صفیں != صفیں || gui.عدد_ستون_تعداد != Columns || gui.force_redraw)
    {
	shell_resized();
	gui.force_redraw = 0;
    }

#اگر_تعریف_شدہ FEAT_GUI_HAIKU
    vim_unlock_screen();
#اگر_ختم

    gui_update_scrollbars(TRUE);
    gui_update_cursor(FALSE, TRUE);
#اگر defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(pixel_چوڑائی, pixel_اونچائی);
#اگر_ختم
#اگر defined(FEAT_XIM) && !defined(FEAT_GUI_GTK)
    xim_سیٹ_حالت_area();
#اگر_ختم

    busy = FALSE;

    // We may have been called again while redrawing the screen.
    // Need to do it all again with the latest size then.  But only if the size
    // actually changed.
    اگر (new_pixel_اونچائی)
    {
	اگر (pixel_چوڑائی == new_pixel_چوڑائی && pixel_اونچائی == new_pixel_اونچائی)
	{
	    new_pixel_چوڑائی = 0;
	    new_pixel_اونچائی = 0;
	}
	ورنہ
	{
	    pixel_چوڑائی = new_pixel_چوڑائی;
	    pixel_اونچائی = new_pixel_اونچائی;
	    جاؤ again;
	}
    }
}

/*
 * Check if gui_resize_shell() must be called.
 */
    خالی
gui_may_resize_shell(خالی)
{
    اگر (new_pixel_اونچائی)
    {
	// careful: gui_resize_shell() may postpone the resize again if we
	// were called indirectly by it
	gui_resize_shell(new_pixel_چوڑائی, new_pixel_اونچائی);
    }
}

    صحیح
gui_get_shellsize(خالی)
{
    صفیں = gui.عدد_صفیں;
    Columns = gui.عدد_ستون_تعداد;
    واپس OK;
}

/*
 * Set the size of the Vim shell according to Rows and Columns.
 * If "fit_to_display" is TRUE then the size may be reduced to fit the window
 * on the screen.
 * When "mustset" is TRUE the size was set by the user. When FALSE a UI
 * component was added or removed (e.g., a scrollbar).
 */
    خالی
gui_سیٹ_shellsize(
    صحیح		mustset غیر_مستعمل,
    صحیح		fit_مقصد_display,
    صحیح		direction)		// RESIZE_HOR, RESIZE_VER
{
    صحیح		بنیاد_چوڑائی;
    صحیح		بنیاد_اونچائی;
    صحیح		چوڑائی;
    صحیح		اونچائی;
    صحیح		min_چوڑائی;
    صحیح		min_اونچائی;
    صحیح		screen_w;
    صحیح		screen_h;
#اگر_تعریف_شدہ FEAT_GUI_GTK
    صحیح		un_maximize = mustset;
    صحیح		did_adjust = 0;
#اگر_ختم
    صحیح		x = -1, y = -1;

    اگر (!gui.shell_created)
	واپس;

#اگر defined(MSWIN) || defined(FEAT_GUI_GTK)
    // If not setting to a user specified size and maximized, calculate the
    // number of characters that fit in the maximized window.
    // FIXME: gui_mch_newfont() is called here even when the font hasn't
    // changed at all.  For example, ":set guioptions=k" triggers this path
    // via gui_init_which_components() -> gui_set_shellsize(FALSE, ...).
    // The intent is to keep the window size and recalculate Rows/Columns,
    // which has nothing to do with fonts.  This should be a separate
    // function with a more descriptive name.
    اگر (!mustset && (vim_strchr(p_go, GO_KEEPWINSIZE) != تہی
						       || gui_mch_maximized()))
    {
	gui_mch_newfont();
	واپس;
    }
#اگر_ختم

    بنیاد_چوڑائی = gui_get_بنیاد_چوڑائی();
    بنیاد_اونچائی = gui_get_بنیاد_اونچائی();
    اگر (fit_مقصد_display)
	// Remember the original window position.
	(خالی)gui_mch_get_winpos(&x, &y);

    چوڑائی = Columns * gui.char_چوڑائی + بنیاد_چوڑائی;
    اونچائی = صفیں * gui.char_اونچائی + بنیاد_اونچائی;

    اگر (fit_مقصد_display)
    {
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	اگر ((direction & RESIZE_HOR) && چوڑائی > screen_w)
	{
	    Columns = (screen_w - بنیاد_چوڑائی) / gui.char_چوڑائی;
	    اگر (Columns < MIN_COLUMNS)
		Columns = MIN_COLUMNS;
	    چوڑائی = Columns * gui.char_چوڑائی + بنیاد_چوڑائی;
#اگر_تعریف_شدہ FEAT_GUI_GTK
	    ++did_adjust;
#اگر_ختم
	}
	اگر ((direction & RESIZE_VERT) && اونچائی > screen_h)
	{
	    صفیں = (screen_h - بنیاد_اونچائی) / gui.char_اونچائی;
	    check_shellsize();
	    اونچائی = صفیں * gui.char_اونچائی + بنیاد_اونچائی;
#اگر_تعریف_شدہ FEAT_GUI_GTK
	    ++did_adjust;
#اگر_ختم
	}
#اگر_تعریف_شدہ FEAT_GUI_GTK
	اگر (did_adjust == 2 || (چوڑائی + gui.char_چوڑائی >= screen_w
				     && اونچائی + gui.char_اونچائی >= screen_h))
	    // don't unmaximize if at maximum size
	    un_maximize = FALSE;
#اگر_ختم
    }
    حد_بندی_screen_سائز();
    gui.عدد_ستون_تعداد = Columns;
    gui.عدد_صفیں = صفیں;
#اگر_تعریف_شدہ USE_GTK4
    // Keep the drawing area in sync with the size Vim is going to draw.
    gui_gtk4_update_سائز();
#اگر_ختم

    min_چوڑائی = بنیاد_چوڑائی + MIN_COLUMNS * gui.char_چوڑائی;
    min_اونچائی = بنیاد_اونچائی + MIN_سطریں * gui.char_اونچائی;
    min_اونچائی += tabline_اونچائی() * gui.char_اونچائی;

#اگر_تعریف_شدہ FEAT_GUI_GTK
    اگر (un_maximize)
    {
	// If the window size is smaller than the screen unmaximize the
	// window, otherwise resizing won't work.
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	اگر ((چوڑائی + gui.char_چوڑائی < screen_w
				   || اونچائی + gui.char_اونچائی * 2 < screen_h)
		&& gui_mch_maximized())
	    gui_mch_unmaximize();
    }
#اگر_ختم

    gui_mch_سیٹ_shellsize(چوڑائی, اونچائی, min_چوڑائی, min_اونچائی,
					  بنیاد_چوڑائی, بنیاد_اونچائی, direction);

    اگر (fit_مقصد_display && x >= 0 && y >= 0)
    {
	// Some window managers put the Vim window left of/above the screen.
	// Only change the position if it wasn't already negative before
	// (happens on MS-Windows with a secondary monitor).
	gui_mch_update();
	اگر (gui_mch_get_winpos(&x, &y) == OK && (x < 0 || y < 0))
	    gui_mch_سیٹ_winpos(x < 0 ? 0 : x, y < 0 ? 0 : y);
    }

    gui_position_components(چوڑائی);
    gui_update_scrollbars(TRUE);
    gui_reset_scroll_region();

#اگر defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    gui_gtk4_calculate_bleed(چوڑائی, اونچائی);
#اگر_ختم
}

/*
 * Called when Rows and/or Columns has changed.
 */
    خالی
gui_new_shellsize(خالی)
{
    gui_reset_scroll_region();
}

/*
 * Make scroll region cover whole screen.
 */
    ساکن خالی
gui_reset_scroll_region(خالی)
{
    gui.scroll_region_چوٹی = 0;
    gui.scroll_region_bot = gui.عدد_صفیں - 1;
    gui.scroll_region_بایاں = 0;
    gui.scroll_region_دایاں = gui.عدد_ستون_تعداد - 1;
}

    ساکن خالی
gui_آغاز_highlight(صحیح نقاب)
{
    اگر (نقاب > HL_ALL)		    // highlight code
	gui.highlight_نقاب = نقاب;
    ورنہ			    //نقاب
	gui.highlight_نقاب |= نقاب;
}

    خالی
gui_stop_highlight(صحیح نقاب)
{
    اگر (نقاب > HL_ALL)		    // highlight code
	gui.highlight_نقاب = HL_معمول;
    ورنہ			    //نقاب
	gui.highlight_نقاب &= ~نقاب;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    خالی
gui_clear_بلاک(
    صحیح	    row1,
    صحیح	    col1,
    صحیح	    row2,
    صحیح	    col2)
{
    // Clear the selection if we are about to write over it
    clip_may_clear_selection(row1, row2);

    gui_mch_clear_بلاک(row1, col1, row2, col2);

    // Invalidate cursor if it was in this block
    اگر (       gui.cursor_صف >= row1 && gui.cursor_صف <= row2
	    && gui.cursor_ستون >= col1 && gui.cursor_ستون <= col2)
	gui.cursor_is_valid = false;
}

/*
 * Write code to update the cursor later.  This avoids the need to flush the
 * output buffer before calling gui_update_cursor().
 */
    خالی
gui_update_cursor_later(خالی)
{
    برآمد_STR("\033|s");
}

    خالی
gui_write(
    char_u	*s,
    صحیح		len)
{
    char_u	*p;
    صحیح		arg1 = 0, arg2 = 0;
    صحیح		force_cursor = FALSE;	// force cursor update
    صحیح		force_scrollbar = FALSE;
    ساکن win_T	*old_curwin = تہی;

// #define DEBUG_GUI_WRITE
#اگر_تعریف_شدہ DEBUG_GUI_WRITE
    {
	صحیح i;
	char_u *str;

	لکھو("gui_write(%d):\n    ", len);
	برائے (i = 0; i < len; i++)
	    اگر (s[i] == ESC)
	    {
		اگر (i != 0)
		    لکھو("\n    ");
		لکھو("<ESC>");
	    }
	    ورنہ
	    {
		str = transchar_byte(s[i]);
		اگر (str[0] && str[1])
		    لکھو("<%s>", (حرف *)str);
		ورنہ
		    لکھو("%s", (حرف *)str);
	    }
	لکھو("\n");
    }
#اگر_ختم
    جب (len)
    {
	اگر (s[0] == ESC && s[1] == '|')
	{
	    p = s + 2;
	    اگر (VIM_ISDIGIT(*p) || (*p == '-' && VIM_ISDIGIT(*(p + 1))))
	    {
		arg1 = getdigits(&p);
		اگر (p > s + len)
		    توڑو;
		اگر (*p == ';')
		{
		    ++p;
		    arg2 = getdigits(&p);
		    اگر (p > s + len)
			توڑو;
		}
	    }
	    انتخاب (*p)
	    {
		حالت 'C':	// Clear screen
		    clip_scroll_selection(9999);
		    gui_mch_clear_all();
		    gui.cursor_is_valid = false;
		    force_scrollbar = TRUE;
		    توڑو;
		حالت 'M':	// Move cursor
		    gui_سیٹ_cursor(arg1, arg2);
		    توڑو;
		حالت 's':	// force cursor (shape) update
		    force_cursor = TRUE;
		    توڑو;
		حالت 'R':	// Set scroll region
		    اگر (arg1 < arg2)
		    {
			gui.scroll_region_چوٹی = arg1;
			gui.scroll_region_bot = arg2;
		    }
		    ورنہ
		    {
			gui.scroll_region_چوٹی = arg2;
			gui.scroll_region_bot = arg1;
		    }
		    توڑو;
		حالت 'V':	// Set vertical scroll region
		    اگر (arg1 < arg2)
		    {
			gui.scroll_region_بایاں = arg1;
			gui.scroll_region_دایاں = arg2;
		    }
		    ورنہ
		    {
			gui.scroll_region_بایاں = arg2;
			gui.scroll_region_دایاں = arg1;
		    }
		    توڑو;
		حالت 'd':	// Delete line
		    gui_delete_سطریں(gui.صف, 1);
		    توڑو;
		حالت 'D':	// Delete lines
		    gui_delete_سطریں(gui.صف, arg1);
		    توڑو;
		حالت 'i':	// Insert line
		    gui_داخل_کرو_سطریں(gui.صف, 1);
		    توڑو;
		حالت 'I':	// Insert lines
		    gui_داخل_کرو_سطریں(gui.صف, arg1);
		    توڑو;
		حالت '$':	// Clear to end-of-line
		    gui_clear_بلاک(gui.صف, gui.ستون, gui.صف,
							    (صحیح)Columns - 1);
		    توڑو;
		حالت 'h':	// Turn on highlighting
		    gui_آغاز_highlight(arg1);
		    توڑو;
		حالت 'H':	// Turn off highlighting
		    gui_stop_highlight(arg1);
		    توڑو;
		حالت 'f':	// flash the window (visual bell)
		    gui_mch_flash(arg1 == 0 ? 20 : arg1);
		    توڑو;
		طےشدہ:
		    p = s + 1;	// Skip the ESC
		    توڑو;
	    }
	    len -= (صحیح)(++p - s);
	    s = p;
	}
	ورنہ اگر (s[0] < 0x20		// Ctrl character
#اگر_تعریف_شدہ FEAT_SIGN_ICONS
		&& s[0] != SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
		&& s[0] != MULTISIGN_BYTE
# endif
#اگر_ختم
		)
	{
	    اگر (s[0] == '\n')		// NL
	    {
		gui.ستون = 0;
		اگر (gui.صف < gui.scroll_region_bot)
		    gui.صف++;
		ورنہ
		    gui_delete_سطریں(gui.scroll_region_چوٹی, 1);
	    }
	    ورنہ اگر (s[0] == '\r')	// CR
	    {
		gui.ستون = 0;
	    }
	    ورنہ اگر (s[0] == '\b')	// Backspace
	    {
		اگر (gui.ستون)
		    --gui.ستون;
	    }
	    ورنہ اگر (s[0] == Ctrl_L)	// cursor-right
	    {
		++gui.ستون;
	    }
	    ورنہ اگر (s[0] == Ctrl_G)	// Beep
	    {
		gui_mch_beep();
	    }
	    // Other Ctrl character: shouldn't happen!

	    --len;	// Skip this char
	    ++s;
	}
	ورنہ
	{
	    p = s;
	    جب (len > 0 && (
			*p >= 0x20
#اگر_تعریف_شدہ FEAT_SIGN_ICONS
			|| *p == SIGN_BYTE
# ifdef FEAT_NETBEANS_INTG
			|| *p == MULTISIGN_BYTE
# endif
#اگر_ختم
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
    ورنہ
	gui_update_horiz_scrollbar(FALSE);
    old_curwin = curwin;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#اگر defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#اگر_ختم

    gui_may_flush();		    // In case vim decides to take a nap
}

/*
 * When ScreenLines[] is invalid, updating the cursor should not be done, it
 * produces wrong results.  Call gui_dont_update_cursor() before that code and
 * gui_can_update_cursor() afterwards.
 */
    خالی
gui_dont_update_cursor(صحیح undraw)
{
    اگر (!gui.اندر_use)
	واپس;

    // Undraw the cursor now, we probably can't do it after the change.
    اگر (undraw)
	gui_undraw_cursor();
    can_update_cursor = FALSE;
}

    خالی
gui_can_update_cursor(خالی)
{
    can_update_cursor = TRUE;
    // No need to update the cursor right now, there is always more output
    // after scrolling.
}

/*
 * Disable issuing gui_mch_flush().
 */
    خالی
gui_disable_flush(خالی)
{
    ++disable_flush;
}

/*
 * Enable issuing gui_mch_flush().
 */
    خالی
gui_enable_flush(خالی)
{
    --disable_flush;
}

/*
 * Issue gui_mch_flush() if it is not disabled.
 */
    خالی
gui_may_flush(خالی)
{
    اگر (disable_flush == 0)
	gui_mch_flush();
}

    ساکن خالی
gui_outstr(char_u *s, صحیح len)
{
    صحیح	    this_len;
    صحیح	    cells;

    اگر (len == 0)
	واپس;

    اگر (len < 0)
	len = (صحیح)STRLEN(s);

    جب (len > 0)
    {
	اگر (has_mbyte)
	{
	    // Find out how many chars fit in the current line.
	    cells = 0;
	    برائے (this_len = 0; this_len < len; )
	    {
		cells += (*mb_ptr2cells)(s + this_len);
		اگر (gui.ستون + cells > Columns)
		    توڑو;
		this_len += (*mb_ptr2len)(s + this_len);
	    }
	    اگر (this_len > len)
		this_len = len;	    // don't include following composing char
	}
	ورنہ اگر (gui.ستون + len > Columns)
	    this_len = Columns - gui.ستون;
	ورنہ
	    this_len = len;

	(خالی)gui_outstr_nowrap(s, this_len,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	s += this_len;
	len -= this_len;
	// fill up for a double-width char that doesn't fit.
	اگر (len > 0 && gui.ستون < Columns)
	    (خالی)gui_outstr_nowrap((char_u *)" ", 1,
					  0, (guicolor_T)0, (guicolor_T)0, 0);
	// The cursor may wrap to the next line.
	اگر (gui.ستون >= Columns)
	{
	    gui.ستون = 0;
	    gui.صف++;
	}
    }
}

#اگر_غیر_تعریف_شدہ USE_GTK4
/*
 * Output one character (may be one or two display cells).
 * Caller must check for valid "off".
 * Returns FAIL or OK, just like gui_outstr_nowrap().
 */
    ساکن صحیح
gui_screenchar(
    صحیح		off,	    // Offset from start of screen
    صحیح		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    صحیح		عقب)	    // backup this many chars when using bold trick
{
    char_u	buf[MB_MAXBYTES + 1];

    // Don't draw right half of a double-width UTF-8 char. "cannot happen"
    اگر (enc_utf8 && Screen_سطریں[off] == 0)
	واپس OK;

    اگر (enc_utf8 && Screen_سطریں_UC[off] != 0)
	// Draw UTF-8 multi-byte character.
	واپس gui_outstr_nowrap(buf, utfc_char2bytes(off, buf),
							 flags, fg, bg, عقب);

    اگر (enc_dbcs == DBCS_JPNU && Screen_سطریں[off] == 0x8e)
    {
	buf[0] = Screen_سطریں[off];
	buf[1] = ScreenLines2[off];
	واپس gui_outstr_nowrap(buf, 2, flags, fg, bg, عقب);
    }

    // Draw non-multi-byte character or DBCS character.
    واپس gui_outstr_nowrap(Screen_سطریں + off,
	    enc_dbcs ? (*mb_ptr2len)(Screen_سطریں + off) : 1,
							 flags, fg, bg, عقب);
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_GTK
/*
 * Output the string at the given screen position.  This is used in place
 * of gui_screenchar() where possible because Pango needs as much context
 * as possible to work nicely.  It's a lot faster as well.
 */
    ساکن صحیح
gui_screenstr(
    صحیح		off,	    // Offset from start of screen
    صحیح		len,	    // string length in screen cells
    صحیح		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    صحیح		عقب)	    // backup this many chars when using bold trick
{
    char_u  *buf;
    صحیح	    outlen = 0;
    صحیح	    i;
    صحیح	    retval;

    اگر (len <= 0) // "cannot happen"?
	واپس OK;

    اگر (enc_utf8)
    {
	buf = alloc(len * MB_MAXBYTES + 1);
	اگر (buf == تہی)
	    واپس OK; // not much we could do here...

	برائے (i = off; i < off + len; ++i)
	{
	    اگر (Screen_سطریں[i] == 0)
		جاری; // skip second half of double-width char

	    اگر (Screen_سطریں_UC[i] == 0)
		buf[outlen++] = Screen_سطریں[i];
	    ورنہ
		outlen += utfc_char2bytes(i, buf + outlen);
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, عقب);
	vim_free(buf);

	واپس retval;
    }
    ورنہ اگر (enc_dbcs == DBCS_JPNU)
    {
	buf = alloc(len * 2 + 1);
	اگر (buf == تہی)
	    واپس OK; // not much we could do here...

	برائے (i = off; i < off + len; ++i)
	{
	    buf[outlen++] = Screen_سطریں[i];

	    // handle double-byte single-width char
	    اگر (Screen_سطریں[i] == 0x8e)
		buf[outlen++] = ScreenLines2[i];
	    ورنہ اگر (MB_BYTE2LEN(Screen_سطریں[i]) == 2)
		buf[outlen++] = Screen_سطریں[++i];
	}

	buf[outlen] = NUL; // only to aid debugging
	retval = gui_outstr_nowrap(buf, outlen, flags, fg, bg, عقب);
	vim_free(buf);

	واپس retval;
    }
    ورنہ
    {
	واپس gui_outstr_nowrap(&Screen_سطریں[off], len,
				 flags, fg, bg, عقب);
    }
}
#اگر_ختم // FEAT_GUI_GTK

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
    ساکن صحیح
gui_outstr_nowrap(
    char_u	*s,
    صحیح		len,
    صحیح		flags,
    guicolor_T	fg,	    // colors for cursor
    guicolor_T	bg,	    // colors for cursor
    صحیح		عقب)	    // backup this many chars when using bold trick
{
    long_u	highlight_نقاب;
    long_u	hl_نقاب_todo;
    guicolor_T	fg_color;
    guicolor_T	bg_color;
    guicolor_T	sp_color;
#اگر !defined(FEAT_GUI_GTK)
    GuiFont	font = NOFONT;
    GuiFont	wide_font = NOFONT;
# ifdef FEAT_XFONTSET
    GuiFontset	fontset = NOFONTSET;
# endif
#اگر_ختم
    attrentry_T	*aep = تہی;
    صحیح		draw_flags;
    صحیح		ستون = gui.ستون;
#اگر_تعریف_شدہ FEAT_SIGN_ICONS
    صحیح		draw_sign = FALSE;
    صحیح		signcol = 0;
    char_u	extra[18];
# ifdef FEAT_NETBEANS_INTG
    صحیح		multi_sign = FALSE;
# endif
#اگر_ختم

    اگر (len < 0)
	len = (صحیح)STRLEN(s);
    اگر (len == 0)
	واپس OK;

#اگر_تعریف_شدہ FEAT_SIGN_ICONS
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
	    صحیح	n = نمبر_چوڑائی(curwin);

	    slen = MIN(n, (صحیح)حجم(extra) - 1);
	}
	vim_memset(extra, ' ', slen);
	extra[slen] = NUL;
	s = extra;
	اگر (len == 1 && ستون > 0)
	    --ستون;
	len = (صحیح)slen;
	اگر (len > 2)
	    // right align sign icon in the number column
	    signcol = ستون + len - 3;
	ورنہ
	    signcol = ستون;
	draw_sign = TRUE;
	highlight_نقاب = 0;
    }
    ورنہ
#اگر_ختم
    اگر (gui.highlight_نقاب > HL_ALL)
    {
	aep = syn_gui_attr2entry(gui.highlight_نقاب);
	اگر (aep == تہی)	    // highlighting not set
	    highlight_نقاب = 0;
	ورنہ
	    highlight_نقاب = aep->ae_attr;
    }
    ورنہ
	highlight_نقاب = gui.highlight_نقاب;
    hl_نقاب_todo = highlight_نقاب;

#اگر !defined(FEAT_GUI_GTK)
    // Set the font
    اگر (aep != تہی && aep->ae_u.gui.font != NOFONT)
	font = aep->ae_u.gui.font;
# ifdef FEAT_XFONTSET
    ورنہ اگر (aep != تہی && aep->ae_u.gui.fontset != NOFONTSET)
	fontset = aep->ae_u.gui.fontset;
# endif
    ورنہ
    {
# ifdef FEAT_XFONTSET
	اگر (gui.fontset != NOFONTSET)
	    fontset = gui.fontset;
	ورنہ
# endif
	    اگر (hl_نقاب_todo & (HL_BOLD | HL_STANDOUT))
	{
	    اگر ((hl_نقاب_todo & HL_ITALIC) && gui.boldital_font != NOFONT)
	    {
		font = gui.boldital_font;
		hl_نقاب_todo &= ~(HL_BOLD | HL_STANDOUT | HL_ITALIC);
	    }
	    ورنہ اگر (gui.bold_font != NOFONT)
	    {
		font = gui.bold_font;
		hl_نقاب_todo &= ~(HL_BOLD | HL_STANDOUT);
	    }
	    ورنہ
		font = gui.norm_font;
	}
	ورنہ اگر ((hl_نقاب_todo & HL_ITALIC) && gui.ital_font != NOFONT)
	{
	    font = gui.ital_font;
	    hl_نقاب_todo &= ~HL_ITALIC;
	}
	ورنہ
	    font = gui.norm_font;

	/*
	 * Choose correct wide_font by font.  wide_font should be set with font
	 * at same time in above block.  But it will make many "ifdef" nasty
	 * blocks.  So we do it here.
	 */
	اگر (font == gui.boldital_font && gui.wide_boldital_font)
	    wide_font = gui.wide_boldital_font;
	ورنہ اگر (font == gui.bold_font && gui.wide_bold_font)
	    wide_font = gui.wide_bold_font;
	ورنہ اگر (font == gui.ital_font && gui.wide_ital_font)
	    wide_font = gui.wide_ital_font;
	ورنہ اگر (font == gui.norm_font && gui.wide_font)
	    wide_font = gui.wide_font;
    }
# ifdef FEAT_XFONTSET
    اگر (fontset != NOFONTSET)
	gui_mch_سیٹ_fontset(fontset);
    ورنہ
# endif
	gui_mch_سیٹ_font(font);
#اگر_ختم

    draw_flags = 0;

    // Set the color
    bg_color = gui.عقب_pixel;
    اگر ((flags & GUI_MON_IS_CURSOR) && gui.اندر_focus)
    {
	draw_flags |= DRAW_CURSOR;
	fg_color = fg;
	bg_color = bg;
	sp_color = fg;
    }
    ورنہ اگر (aep != تہی)
    {
	fg_color = aep->ae_u.gui.fg_color;
	اگر (fg_color == INVALCOLOR)
	    fg_color = gui.norm_pixel;
	bg_color = aep->ae_u.gui.bg_color;
	اگر (bg_color == INVALCOLOR)
	    bg_color = gui.عقب_pixel;
	sp_color = aep->ae_u.gui.sp_color;
	اگر (sp_color == INVALCOLOR)
	    sp_color = fg_color;
    }
    ورنہ
    {
	fg_color = gui.norm_pixel;
	sp_color = fg_color;
    }

    اگر (highlight_نقاب & (HL_INVERSE | HL_STANDOUT))
    {
	gui_mch_سیٹ_fg_color(bg_color);
	gui_mch_سیٹ_bg_color(fg_color);
    }
    ورنہ
    {
	gui_mch_سیٹ_fg_color(fg_color);
	gui_mch_سیٹ_bg_color(bg_color);
    }
    gui_mch_سیٹ_sp_color(sp_color);

    // Clear the selection if we are about to write over it
    اگر (!(flags & GUI_MON_NOCLEAR))
	clip_may_clear_selection(gui.صف, gui.صف);


    // If there's no bold font, then fake it
    اگر (hl_نقاب_todo & (HL_BOLD | HL_STANDOUT))
	draw_flags |= DRAW_BOLD;

    /*
     * When drawing bold or italic characters the spill-over from the left
     * neighbor may be destroyed.  Let the caller backup to start redrawing
     * just after a blank.
     */
    اگر (عقب != 0 && ((draw_flags & DRAW_BOLD) || (highlight_نقاب & HL_ITALIC)))
	واپس FAIL;

#اگر defined(FEAT_GUI_GTK)
    // If there's no italic font, then fake it.
    // For GTK2, we don't need a different font for italic style.
    اگر (hl_نقاب_todo & HL_ITALIC)
	draw_flags |= DRAW_ITALIC;

    // Do we underline the text?
    اگر (hl_نقاب_todo & HL_UNDERLINE)
	draw_flags |= DRAW_UNDERL;

#ورنہ
    // Do we underline the text?
    اگر ((hl_نقاب_todo & HL_UNDERLINE) || (hl_نقاب_todo & HL_ITALIC))
	draw_flags |= DRAW_UNDERL;
#اگر_ختم
    // Do we undercurl the text?
    اگر (hl_نقاب_todo & HL_UNDERCURL)
	draw_flags |= DRAW_UNDERC;

    // TODO: HL_UNDERDOUBLE, HL_UNDERDOTTED, HL_UNDERDASHED

    // Do we strikethrough the text?
    اگر (hl_نقاب_todo & HL_STRIKETHROUGH)
	draw_flags |= DRAW_STRIKE;

    // Do we draw transparently?
    اگر (flags & GUI_MON_TRS_CURSOR)
	draw_flags |= DRAW_TRANSP;

    /*
     * Draw the text.
     */
#اگر_تعریف_شدہ FEAT_GUI_GTK
    // The value returned is the length in display cells
    len = gui_gtk_draw_string(gui.صف, ستون, s, len, draw_flags);
#ورنہ
    اگر (enc_utf8)
    {
	صحیح	آغاز;		// index of bytes to be drawn
	صحیح	cells;		// cellwidth of bytes to be drawn
	صحیح	thislen;	// length of bytes to be drawn
	صحیح	cn;		// cellwidth of current char
	صحیح	i;		// index of current char
	صحیح	c;		// current char value
	صحیح	cl;		// byte length of current char
	صحیح	comping;	// current char is composing
	صحیح	scol = ستون;	// screen column
	صحیح	curr_wide = FALSE;  // use 'guifontwide'
	صحیح	پچھلا_wide = FALSE;
	صحیح	wide_بدلا;
# ifdef MSWIN
	صحیح	sep_comp = FALSE;   // Don't separate composing chars.
# ورنہ
	صحیح	sep_comp = TRUE;    // Separate composing chars.
# endif

	// Break the string at a composing character, it has to be drawn on
	// top of the previous character.
	آغاز = 0;
	cells = 0;
	برائے (i = 0; i < len; i += cl)
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
		ورنہ
		    curr_wide = FALSE;
	    }
	    cl = utf_ptr2len(s + i);
	    اگر (cl == 0)	// hit end of string
		len = i + cl;	// len must be wrong "cannot happen"

	    wide_بدلا = curr_wide != پچھلا_wide;

	    // Print the string so far if it's the last character or there is
	    // a composing character.
	    اگر (i + cl >= len || (comping && sep_comp && i > آغاز)
		    || wide_بدلا
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
		اگر ((comping && sep_comp) || wide_بدلا)
		    thislen = i - آغاز;
		ورنہ
		    thislen = i - آغاز + cl;
		اگر (thislen > 0)
		{
		    اگر (پچھلا_wide)
			gui_mch_سیٹ_font(wide_font);
		    gui_mch_draw_string(gui.صف, scol, s + آغاز, thislen,
								  draw_flags);
		    اگر (پچھلا_wide)
			gui_mch_سیٹ_font(font);
		    آغاز += thislen;
		}
		scol += cells;
		cells = 0;
		// Adjust to not draw a character which width is changed
		// against with last one.
		اگر (wide_بدلا && !(comping && sep_comp))
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
			&& !wide_بدلا)
		    gui_mch_draw_string(gui.صف, scol - 1, (char_u *)" ",
							       1, draw_flags);
# endif
	    }
	    // Draw a composing char on top of the previous char.
	    اگر (comping && sep_comp)
	    {
# اگر defined(__APPLE_CC__) && ہدف_API_MAC_CARBON
		// Carbon ATSUI autodraws composing char over previous char
		gui_mch_draw_string(gui.صف, scol, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# ورنہ
		gui_mch_draw_string(gui.صف, scol - cn, s + i, cl,
						    draw_flags | DRAW_TRANSP);
# endif
		آغاز = i + cl;
	    }
	    پچھلا_wide = curr_wide;
	}
	// The stuff below assumes "len" is the length in screen columns.
	len = scol - ستون;
    }
    ورنہ
    {
	gui_mch_draw_string(gui.صف, ستون, s, len, draw_flags);
	اگر (enc_dbcs == DBCS_JPNU)
	{
	    // Get the length in display cells, this can be different from the
	    // number of bytes for "euc-jp".
	    len = mb_string2cells(s, len);
	}
    }
#اگر_ختم // !FEAT_GUI_GTK

    اگر (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
	gui.ستون = ستون + len;

    // May need to invert it when it's part of the selection.
    اگر (flags & GUI_MON_NOCLEAR)
	clip_may_redraw_selection(gui.صف, ستون, len);

    اگر (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
    {
	// Invalidate the old physical cursor position if we wrote over it
	اگر (gui.cursor_صف == gui.صف
		&& gui.cursor_ستون >= ستون
		&& gui.cursor_ستون < ستون + len)
	    gui.cursor_is_valid = false;
    }

#اگر_تعریف_شدہ FEAT_SIGN_ICONS
    اگر (draw_sign)
	// Draw the sign on top of the spaces.
	gui_mch_drawsign(gui.صف, signcol, gui.highlight_نقاب);
# اگر defined(FEAT_NETBEANS_INTG) && (defined(FEAT_GUI_X11) \
	|| defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN))
    اگر (multi_sign)
	netbeans_draw_multisign_indicator(gui.صف);
# endif
#اگر_ختم

    واپس OK;
}

/*
 * Undraw the cursor.  This actually redraws the character at the cursor
 * position, plus some more characters when needed.
 */
    خالی
gui_undraw_cursor(خالی)
{
    اگر (!gui.cursor_is_valid)
	واپس;

    // Always redraw the character just before if there is one, because
    // with some fonts and characters there can be a one pixel overlap.
    صحیح startcol = gui.cursor_ستون > 0 ? gui.cursor_ستون - 1 : gui.cursor_ستون;
    صحیح endcol = gui.cursor_ستون;

#اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    gui_adjust_undraw_cursor_for_ligatures(&startcol, &endcol);
#اگر_ختم
    gui_redraw_بلاک(gui.cursor_صف, startcol,
	    gui.cursor_صف, endcol, GUI_MON_NOCLEAR);

    // Cursor_is_valid is reset when the cursor is undrawn, also reset it
    // here in case it wasn't needed to undraw it.
    gui.cursor_is_valid = false;
}

    خالی
gui_redraw(
    صحیح		x,
    صحیح		y,
    صحیح		w,
    صحیح		h)
{
    صحیح		row1, col1, row2, col2;

    row1 = Y_2_صف(y);
    col1 = X_2_ستون(x);
    row2 = Y_2_صف(y + h - 1);
    col2 = X_2_ستون(x + w - 1);

    gui_redraw_بلاک(row1, col1, row2, col2, GUI_MON_NOCLEAR);

    /*
     * We may need to redraw the cursor, but don't take it upon us to change
     * its location after a scroll.
     * (maybe be more strict even and test col too?)
     * These things may be outside the update/clipping region and reality may
     * not reflect Vims internal ideas if these operations are clipped away.
     */
    اگر (gui.صف == gui.cursor_صف)
	gui_update_cursor(TRUE, TRUE);
}

/*
 * Draw a rectangular block of characters, from row1 to row2 (inclusive) and
 * from col1 to col2 (inclusive).
 */
    خالی
gui_redraw_بلاک(
    صحیح		row1,
    صحیح		col1,
    صحیح		row2,
    صحیح		col2,
    صحیح		flags)	// flags for gui_outstr_nowrap()
{
    صحیح		old_صف, old_ستون;
    long_u	old_hl_نقاب;
    صحیح		off;
    sattr_T	پہلا_attr;
    صحیح		idx, len;
    صحیح		عقب, nback;
    صحیح		orig_col1, orig_col2;

    // Don't try to update when ScreenLines is not valid
    اگر (!screen_cleared || Screen_سطریں == تہی)
	واپس;

    // Don't try to draw outside the shell!
    // Check everything, strange values may be caused by a big border width
    col1 = check_ستون(col1);
    col2 = check_ستون(col2);
    row1 = check_صف(row1);
    row2 = check_صف(row2);

    // Remember where our cursor was
    old_صف = gui.صف;
    old_ستون = gui.ستون;
    old_hl_نقاب = gui.highlight_نقاب;
    orig_col1 = col1;
    orig_col2 = col2;

    برائے (gui.صف = row1; gui.صف <= row2; gui.صف++)
    {
	// When only half of a double-wide character is in the block, include
	// the other half.
	col1 = orig_col1;
	col2 = orig_col2;
	off = سطر_انحراف[gui.صف];
	اگر (enc_dbcs != 0)
	{
	    اگر (col1 > 0)
		col1 -= dbcs_screen_سر_off(Screen_سطریں + off,
						    Screen_سطریں + off + col1);
	    col2 += dbcs_screen_دم_off(Screen_سطریں + off,
						    Screen_سطریں + off + col2);
	}
	ورنہ اگر (enc_utf8)
	{
	    اگر (Screen_سطریں[off + col1] == 0)
	    {
		اگر (col1 > 0)
		    --col1;
		ورنہ
		    // FIXME: how can the first character ever be zero?
		    siemsg("NUL in ScreenLines in row %ld", (طویل)gui.صف);
	    }
#اگر_تعریف_شدہ FEAT_GUI_GTK
	    اگر (col2 + 1 < Columns && Screen_سطریں[off + col2 + 1] == 0)
		++col2;
#اگر_ختم
	}
	gui.ستون = col1;
	off = سطر_انحراف[gui.صف] + gui.ستون;
	len = col2 - col1 + 1;

	// Find how many chars back this highlighting starts, or where a space
	// is.  Needed for when the bold trick is used
	برائے (عقب = 0; عقب < col1; ++عقب)
	    اگر (ScreenAttrs[off - 1 - عقب] != ScreenAttrs[off]
		    || Screen_سطریں[off - 1 - عقب] == ' ')
		توڑو;

	// Break it up in strings of characters with the same attributes.
	// Print UTF-8 characters individually.
	جب (len > 0)
	{
	    پہلا_attr = ScreenAttrs[off];
	    gui.highlight_نقاب = پہلا_attr;
#اگر !defined(FEAT_GUI_GTK)
	    اگر (enc_utf8 && Screen_سطریں_UC[off] != 0)
	    {
		// output multi-byte character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, عقب);
		اگر (gui.ستون < Columns && Screen_سطریں[off + 1] == 0)
		    idx = 2;
		ورنہ
		    idx = 1;
	    }
	    ورنہ اگر (enc_dbcs == DBCS_JPNU && Screen_سطریں[off] == 0x8e)
	    {
		// output double-byte, single-width character separately
		nback = gui_screenchar(off, flags,
					  (guicolor_T)0, (guicolor_T)0, عقب);
		idx = 1;
	    }
	    ورنہ
#اگر_ختم
	    {
#اگر_تعریف_شدہ FEAT_GUI_GTK
		برائے (idx = 0; idx < len; ++idx)
		{
		    اگر (enc_utf8 && Screen_سطریں[off + idx] == 0)
			جاری; // skip second half of double-width char
		    اگر (ScreenAttrs[off + idx] != پہلا_attr)
			توڑو;
		}
		// gui_screenstr() takes care of multibyte chars
		nback = gui_screenstr(off, idx, flags,
				      (guicolor_T)0, (guicolor_T)0, عقب);
#ورنہ
		برائے (idx = 0; idx < len && ScreenAttrs[off + idx] == پہلا_attr;
									idx++)
		{
		    // Stop at a multi-byte Unicode character.
		    اگر (enc_utf8 && Screen_سطریں_UC[off + idx] != 0)
			توڑو;
		    اگر (enc_dbcs == DBCS_JPNU)
		    {
			// Stop at a double-byte single-width char.
			اگر (Screen_سطریں[off + idx] == 0x8e)
			    توڑو;
			اگر (len > 1 && (*mb_ptr2len)(Screen_سطریں
							    + off + idx) == 2)
			    ++idx;  // skip second byte of double-byte char
		    }
		}
		nback = gui_outstr_nowrap(Screen_سطریں + off, idx, flags,
					  (guicolor_T)0, (guicolor_T)0, عقب);
#اگر_ختم
	    }
	    اگر (nback == FAIL)
	    {
		// Must back up to start drawing where a bold or italic word
		// starts.
		off -= عقب;
		len += عقب;
		gui.ستون -= عقب;
	    }
	    ورنہ
	    {
		off += idx;
		len -= idx;
	    }
	    عقب = 0;
	}
    }

    // Put the cursor back where it was
    gui.صف = old_صف;
    gui.ستون = old_ستون;
    gui.highlight_نقاب = (صحیح)old_hl_نقاب;
}

    ساکن خالی
gui_delete_سطریں(صحیح صف, صحیح گنتی)
{
    اگر (گنتی <= 0)
	واپس;

    اگر (صف + گنتی > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_بلاک(صف, gui.scroll_region_بایاں,
			      gui.scroll_region_bot, gui.scroll_region_دایاں);
    ورنہ
    {
	gui_mch_delete_سطریں(صف, گنتی);

	// If the cursor was in the deleted lines it's now gone.  If the
	// cursor was in the scrolled lines adjust its position.
	اگر (gui.cursor_صف >= صف
		&& gui.cursor_ستون >= gui.scroll_region_بایاں
		&& gui.cursor_ستون <= gui.scroll_region_دایاں)
	{
	    اگر (gui.cursor_صف < صف + گنتی)
		gui.cursor_is_valid = false;
	    ورنہ اگر (gui.cursor_صف <= gui.scroll_region_bot)
		gui.cursor_صف -= گنتی;
	}
    }
}

    ساکن خالی
gui_داخل_کرو_سطریں(صحیح صف, صحیح گنتی)
{
    اگر (گنتی <= 0)
	واپس;

    اگر (صف + گنتی > gui.scroll_region_bot)
	// Scrolled out of region, just blank the lines out
	gui_clear_بلاک(صف, gui.scroll_region_بایاں,
			      gui.scroll_region_bot, gui.scroll_region_دایاں);
    ورنہ
    {
	gui_mch_داخل_کرو_سطریں(صف, گنتی);

	اگر (gui.cursor_صف >= gui.صف
		&& gui.cursor_ستون >= gui.scroll_region_بایاں
		&& gui.cursor_ستون <= gui.scroll_region_دایاں)
	{
	    اگر (gui.cursor_صف <= gui.scroll_region_bot - گنتی)
		gui.cursor_صف += گنتی;
	    ورنہ اگر (gui.cursor_صف <= gui.scroll_region_bot)
		gui.cursor_is_valid = false;
	}
    }
}

#اگر_تعریف_شدہ FEAT_TIMERS
/*
 * Passed to ui_wait_for_chars_or_timer(), ignoring extra arguments.
 */
    ساکن صحیح
gui_wait_for_chars_3(
    طویل wtime,
    صحیح *interrupted غیر_مستعمل,
    صحیح ignore_داخلہ غیر_مستعمل)
{
    واپس gui_mch_wait_for_chars(wtime);
}
#اگر_ختم

/*
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    ساکن صحیح
gui_wait_for_chars_or_timer(
	طویل wtime,
	صحیح *interrupted غیر_مستعمل,
	صحیح ignore_داخلہ غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_TIMERS
    واپس ui_wait_for_chars_or_timer(wtime, gui_wait_for_chars_3,
						    interrupted, ignore_داخلہ);
#ورنہ
    واپس gui_mch_wait_for_chars(wtime);
#اگر_ختم
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
    ساکن صحیح
gui_wait_for_chars_buf(
    char_u	*buf,
    صحیح		maxlen,
    طویل	wtime,	    // don't use "time", MIPS cannot handle it
    صحیح		tb_change_cnt)
{
    صحیح	    retval;
    صحیح	    keep_blinking; // Guard against restarting blink cycle on CursorHold

#اگر_تعریف_شدہ FEAT_MENU
    // If we're going to wait a bit, update the menus and mouse shape for the
    // current State.
    اگر (wtime != 0)
	gui_update_menus(0);
#اگر_ختم

    gui_mch_update();
    اگر (داخلہ_دستیاب())	// Got char, return immediately
    {
	اگر (gui_mch_is_blinking())
	    gui_mch_stop_blink(TRUE);
	اگر (buf != تہی && !typebuf_بدلا(tb_change_cnt))
	    واپس read_from_داخلہ_buf(buf, (طویل)maxlen);
	واپس 0;
    }
    اگر (wtime == 0)		// Don't wait for char
	واپس FAIL;

    // Before waiting, flush any output to the screen.
    gui_mch_flush();

    // Blink while waiting for a character.
    اگر (!gui_mch_is_blinking())
	gui_mch_آغاز_blink();

    // Common function to loop until "wtime" is met, while handling timers and
    // other callbacks.
    retval = inchar_loop(buf, maxlen, wtime, tb_change_cnt,
			 gui_wait_for_chars_or_timer, تہی);

    // Keep blinking when CursorHold wakes the input loop. (See PR #21115)
    keep_blinking = retval == 3 && buf != تہی
	&& buf[0] == K_SPECIAL && buf[1] == KS_EXTRA
	&& buf[2] == (صحیح)KE_CURSORHOLD;

    اگر (!keep_blinking)
	gui_mch_stop_blink(TRUE);

    واپس retval;
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
gui_wait_for_chars(طویل wtime, صحیح tb_change_cnt)
{
    واپس gui_wait_for_chars_buf(تہی, 0, wtime, tb_change_cnt);
}

/*
 * Equivalent of mch_inchar() for the GUI.
 */
    صحیح
gui_inchar(
    char_u  *buf,
    صحیح	    maxlen,
    طویل    wtime,		// milliseconds
    صحیح	    tb_change_cnt)
{
    واپس gui_wait_for_chars_buf(buf, maxlen, wtime, tb_change_cnt);
}

/*
 * Fill p[4] with mouse coordinates encoded for check_termcode().
 */
    ساکن خالی
fill_mouse_coord(char_u *p, صحیح ستون, صحیح صف)
{
    p[0] = (char_u)(ستون / 128 + ' ' + 1);
    p[1] = (char_u)(ستون % 128 + ' ' + 1);
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
    خالی
gui_send_mouse_event(
    صحیح	    button,
    صحیح	    x,
    صحیح	    y,
    صحیح	    repeated_click,
    int_u   modifiers)
{
    ساکن صحیح	    پچھلا_صف = 0, پچھلا_ستون = 0;
    ساکن صحیح	    پچھلا_button = -1;
    ساکن صحیح	    عدد_clicks = 1;
    char_u	    string[10];
    شمار کلید_extra  button_char;
    صحیح		    صف, ستون;
#اگر_تعریف_شدہ FEAT_CLIPBOARD
    صحیح		    checkfor;
    صحیح		    did_clip = FALSE;
#اگر_ختم

    /*
     * Scrolling may happen at any time, also while a selection is present.
     */
    انتخاب (button)
    {
	حالت MOUSE_MOVE:
	    button_char = KE_MOUSEMOVE_XY;
	    جاؤ button_سیٹ;
	حالت MOUSE_X1:
	    button_char = KE_X1MOUSE;
	    جاؤ button_سیٹ;
	حالت MOUSE_X2:
	    button_char = KE_X2MOUSE;
	    جاؤ button_سیٹ;
	حالت MOUSE_4:
	    button_char = KE_MOUSEDOWN;
	    جاؤ button_سیٹ;
	حالت MOUSE_5:
	    button_char = KE_MOUSEUP;
	    جاؤ button_سیٹ;
	حالت MOUSE_6:
	    button_char = KE_MOUSELEFT;
	    جاؤ button_سیٹ;
	حالت MOUSE_7:
	    button_char = KE_MOUSERIGHT;
button_سیٹ:
	    {
		// Don't put events in the input queue now.
		اگر (hold_gui_events)
		    واپس;

		صف = gui_xy2colrow(x, y, &ستون);
		// Don't report a mouse move unless moved to a
		// different character position.
		اگر (button == MOUSE_MOVE)
		{
		    اگر (صف == پچھلا_صف && ستون == پچھلا_ستون)
			واپس;
		    ورنہ
		    {
			پچھلا_صف = صف >= 0 ? صف : 0;
			پچھلا_ستون = ستون;
		    }
		}

		string[3] = CSI;
		string[4] = KS_EXTRA;
		string[5] = (صحیح)button_char;

		// Pass the pointer coordinates of the scroll event so that we
		// know which window to scroll.
		string[6] = (char_u)(ستون / 128 + ' ' + 1);
		string[7] = (char_u)(ستون % 128 + ' ' + 1);
		string[8] = (char_u)(صف / 128 + ' ' + 1);
		string[9] = (char_u)(صف % 128 + ' ' + 1);

		اگر (modifiers == 0)
		    جوڑو_مقصد_داخلہ_buf(string + 3, 7);
		ورنہ
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = 0;
		    اگر (modifiers & MOUSE_سرکاؤ)
			string[2] |= MOD_نقاب_سرکاؤ;
		    اگر (modifiers & MOUSE_CTRL)
			string[2] |= MOD_نقاب_CTRL;
		    اگر (modifiers & MOUSE_ALT)
			string[2] |= MOD_نقاب_ALT;
		    جوڑو_مقصد_داخلہ_buf(string, 10);
		}
		واپس;
	    }
    }

#اگر_تعریف_شدہ FEAT_CLIPBOARD
    // If a clipboard selection is in progress, handle it
    اگر (clip_ستارہ.state == SELECT_اندر_پیش_رفت)
    {
	clip_process_selection(button, X_2_ستون(x), Y_2_صف(y), repeated_click);

	// A release event may still need to be sent if the position is equal.
	صف = gui_xy2colrow(x, y, &ستون);
	اگر (button != MOUSE_RELEASE || صف != پچھلا_صف || ستون != پچھلا_ستون)
	    واپس;
    }

    // Determine which mouse settings to look for based on the current mode
    انتخاب (get_real_state())
    {
	حالت منوال_معمول_BUSY:
	حالت منوال_OP_PENDING:
# ifdef FEAT_TERMINAL
	حالت منوال_TERMINAL:
# endif
	حالت منوال_معمول:	checkfor = MOUSE_معمول; توڑو;
	حالت منوال_VISUAL:	checkfor = MOUSE_VISUAL; توڑو;
	حالت منوال_SELECT:	checkfor = MOUSE_VISUAL; توڑو;
	حالت منوال_REPLACE:
	حالت منوال_REPLACE | منوال_LANGMAP:
	حالت منوال_VREPLACE:
	حالت منوال_VREPLACE | منوال_LANGMAP:
	حالت منوال_داخل_کرو:
	حالت منوال_داخل_کرو | منوال_LANGMAP:
				checkfor = MOUSE_داخل_کرو; توڑو;
	حالت منوال_ASKMORE:
	حالت منوال_HITRETURN:	// At the more- and hit-enter prompt pass the
				// mouse event for a click on or below the
				// message line.
				اگر (Y_2_صف(y) >= پیغام_صف)
				    checkfor = MOUSE_معمول;
				ورنہ
				    checkfor = MOUSE_RETURN;
				توڑو;

	    /*
	     * On the command line, use the clipboard selection on all lines
	     * but the command line.  But not when pasting.
	     */
	حالت منوال_CMDLINE:
	حالت منوال_CMDLINE | منوال_LANGMAP:
	    اگر (Y_2_صف(y) < cmdline_صف && button != MOUSE_درمیان)
		checkfor = MOUSE_NONE;
	    ورنہ
		checkfor = MOUSE_COMMAND;
	    توڑو;

	طےشدہ:
	    checkfor = MOUSE_NONE;
	    توڑو;
    };

    /*
     * Allow clipboard selection of text on the command line in "normal"
     * modes.  Don't do this when dragging the status line, or extending a
     * Visual selection.
     */
    اگر ((State == منوال_معمول || State == منوال_معمول_BUSY
						      || (State & منوال_داخل_کرو))
	    && X_2_ستون(x) >= firstwin->w_wincol
	    && X_2_ستون(x) < firstwin->w_wincol + topframe->fr_چوڑائی
	    && Y_2_صف(y) >= topframe->fr_اونچائی + firstwin->w_winrow
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
    اگر ((modifiers & MOUSE_CTRL) && (modifiers & MOUSE_سرکاؤ))
	checkfor = MOUSE_NONEF;

    /*
     * In Ex mode, always use modeless selection.
     */
    اگر (exmode_فعال)
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
	اگر (checkfor != MOUSE_NONEF && VIsual_فعال && (State & منوال_معمول))
	    واپس;

	/*
	 * When 'mousemodel' is "popup", shift-left is translated to right.
	 * But not when also using Ctrl.
	 */
	اگر (mouse_model_popup() && button == MOUSE_بایاں
		&& (modifiers & MOUSE_سرکاؤ) && !(modifiers & MOUSE_CTRL))
	{
	    button = MOUSE_دایاں;
	    modifiers &= ~ MOUSE_سرکاؤ;
	}

	// If the selection is done, allow the right button to extend it.
	// If the selection is cleared, allow the right button to start it
	// from the cursor position.
	اگر (button == MOUSE_دایاں)
	{
	    اگر (clip_ستارہ.state == SELECT_CLEARED)
	    {
		اگر (State & منوال_CMDLINE)
		{
		    ستون = پیغام_ستون;
		    صف = پیغام_صف;
		}
		ورنہ
		{
		    ستون = curwin->w_wcol;
		    صف = curwin->w_wrow + W_WINROW(curwin);
		}
		clip_آغاز_selection(ستون, صف, FALSE);
	    }
	    clip_process_selection(button, X_2_ستون(x), Y_2_صف(y),
							      repeated_click);
	    did_clip = TRUE;
	}
	// Allow the left button to start the selection
	ورنہ اگر (button == MOUSE_بایاں)
	{
	    clip_آغاز_selection(X_2_ستون(x), Y_2_صف(y), repeated_click);
	    did_clip = TRUE;
	}

	// Always allow pasting
	اگر (button != MOUSE_درمیان)
	{
	    اگر (!mouse_has(checkfor) || button == MOUSE_RELEASE)
		واپس;
	    اگر (checkfor != MOUSE_COMMAND)
		button = MOUSE_بایاں;
	}
	repeated_click = FALSE;
    }

    اگر (clip_ستارہ.state != SELECT_CLEARED && !did_clip)
	clip_clear_selection(&clip_ستارہ);
#اگر_ختم

    // Don't put events in the input queue now.
    اگر (hold_gui_events)
	واپس;

    صف = gui_xy2colrow(x, y, &ستون);

    /*
     * If we are dragging and the mouse hasn't moved far enough to be on a
     * different character, then don't send an event to vim.
     */
    اگر (button == MOUSE_DRAG)
    {
	اگر (صف == پچھلا_صف && ستون == پچھلا_ستون)
	    واپس;
	// Dragging above the window, set "row" to -1 to cause a scroll.
	اگر (y < 0)
	    صف = -1;
    }

    /*
     * If topline has changed (window scrolled) since the last click, reset
     * repeated_click, because we don't want starting Visual mode when
     * clicking on a different character in the text.
     */
    اگر (curwin->w_topline != gui_پچھلا_topline
#اگر_تعریف_شدہ FEAT_DIFF
	    || curwin->w_topfill != gui_پچھلا_topfill
#اگر_ختم
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
	    اگر (button != پچھلا_button || صف != پچھلا_صف || ستون != پچھلا_ستون)
		عدد_clicks = 1;
	    ورنہ اگر (++عدد_clicks > 4)
		عدد_clicks = 1;
	}
	ورنہ
	    عدد_clicks = 1;
	پچھلا_button = button;
	gui_پچھلا_topline = curwin->w_topline;
#اگر_تعریف_شدہ FEAT_DIFF
	gui_پچھلا_topfill = curwin->w_topfill;
#اگر_ختم

	string[3] = (char_u)(button | 0x20);
	سیٹ_عدد_MOUSE_CLICKS(string[3], عدد_clicks);
    }
    ورنہ
	string[3] = (char_u)button;

    string[3] |= modifiers;
    fill_mouse_coord(string + 4, ستون, صف);
    جوڑو_مقصد_داخلہ_buf(string, 8);

    اگر (صف < 0)
	پچھلا_صف = 0;
    ورنہ
	پچھلا_صف = صف;
    پچھلا_ستون = ستون;

    /*
     * We need to make sure this is cleared since GTK doesn't tell us when
     * the user is done dragging.
     */
#اگر defined(FEAT_GUI_GTK)
    gui.dragged_sb = SBAR_NONE;
#اگر_ختم
}

/*
 * Convert x and y coordinate to column and row in text window.
 * Corrects for multi-byte character.
 * returns column in "*colp" and row as return value;
 */
    ساکن صحیح
gui_xy2colrow(صحیح x, صحیح y, صحیح *colp)
{
    صحیح		ستون = check_ستون(X_2_ستون(x));
    صحیح		صف = check_صف(Y_2_صف(y));

    *colp = mb_fix_ستون(ستون, صف);
    واپس صف;
}

#اگر defined(FEAT_MENU)
/*
 * Callback function for when a menu entry has been selected.
 */
    خالی
gui_menu_cb(vimmenu_T *menu)
{
    char_u  بائٹیں[حجم(long_u)];

    // Don't put events in the input queue now.
    اگر (hold_gui_events)
	واپس;

    بائٹیں[0] = CSI;
    بائٹیں[1] = KS_MENU;
    بائٹیں[2] = KE_FILLER;
    جوڑو_مقصد_داخلہ_buf(بائٹیں, 3);
    جوڑو_long_مقصد_buf((long_u)menu, بائٹیں);
    جوڑو_مقصد_داخلہ_buf_csi(بائٹیں, حجم(long_u));
}
#اگر_ختم

ساکن صحیح	پچھلا_which_scrollbars[3];

/*
 * Set which components are present.
 * If "oldval" is not NULL, "oldval" is the previous value, the new value is
 * in p_go.
 */
    خالی
gui_init_which_components(char_u *oldval غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_GUI_DARKTHEME
    ساکن صحیح	پچھلا_dark_theme = FALSE;
    صحیح		using_dark_theme = FALSE;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MENU
    ساکن صحیح	پچھلا_menu_is_فعال = -1;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TOOLBAR
    ساکن صحیح	پچھلا_toolbar = -1;
    صحیح		using_toolbar = FALSE;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    صحیح		using_tabline;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
    ساکن صحیح	پچھلا_titlebar = FALSE;
    صحیح		using_titlebar = FALSE;
#اگر_ختم

#اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    ساکن صحیح	پچھلا_fullscreen = FALSE;
    صحیح		using_fullscreen = FALSE;
#اگر_ختم
#اگر defined(FEAT_MENU)
    ساکن صحیح	پچھلا_tearoff = -1;
    صحیح		using_tearoff = FALSE;
#اگر_ختم

    char_u	*p;
    صحیح		i;
#اگر_تعریف_شدہ FEAT_MENU
    صحیح		grey_old, grey_new;
    char_u	*عارضی;
#اگر_ختم
    win_T	*wp;
    صحیح		need_سیٹ_سائز;
    صحیح		fix_سائز;

#اگر_تعریف_شدہ FEAT_MENU
    اگر (oldval != تہی && gui.اندر_use)
    {
	/*
	 * Check if the menus go from grey to non-grey or vice versa.
	 */
	grey_old = (vim_strchr(oldval, GO_GREY) != تہی);
	grey_new = (vim_strchr(p_go, GO_GREY) != تہی);
	اگر (grey_old != grey_new)
	{
	    عارضی = p_go;
	    p_go = oldval;
	    gui_update_menus(MENU_ALL_MODES);
	    p_go = عارضی;
	}
    }
    gui.menu_is_فعال = false;
#اگر_ختم

    برائے (i = 0; i < 3; i++)
	gui.which_scrollbars[i] = false;
    برائے (p = p_go; *p; p++)
	انتخاب (*p)
	{
	    حالت GO_بایاں:
		gui.which_scrollbars[SBAR_بایاں] = true;
		توڑو;
	    حالت GO_دایاں:
		gui.which_scrollbars[SBAR_دایاں] = true;
		توڑو;
	    حالت GO_VLEFT:
		اگر (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_بایاں] = true;
		توڑو;
	    حالت GO_VRIGHT:
		اگر (win_hasvertsplit())
		    gui.which_scrollbars[SBAR_دایاں] = true;
		توڑو;
	    حالت GO_BOT:
		gui.which_scrollbars[SBAR_BOTTOM] = true;
		توڑو;
#اگر_تعریف_شدہ FEAT_GUI_DARKTHEME
	    حالت GO_DARKTHEME:
		using_dark_theme = TRUE;
		توڑو;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MENU
	    حالت GO_MENUS:
		gui.menu_is_فعال = true;
		توڑو;
#اگر_ختم
	    حالت GO_GREY:
		// make menu's have grey items, ignored here
		توڑو;
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
	    حالت GO_TITLEBAR:
		using_titlebar = TRUE;
		توڑو;
#اگر_ختم
#اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
	    حالت GO_FULLSCREEN:
		using_fullscreen = TRUE;
		توڑو;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TOOLBAR
	    حالت GO_TOOLBAR:
		using_toolbar = TRUE;
		توڑو;
#اگر_ختم
	    حالت GO_TEAROFF:
#اگر defined(FEAT_MENU)
		using_tearoff = TRUE;
#اگر_ختم
		توڑو;
	    طےشدہ:
		// Ignore options that are not supported
		توڑو;
	}

    اگر (!gui.اندر_use)
	واپس;

    need_سیٹ_سائز = 0;
    fix_سائز = FALSE;

#اگر_تعریف_شدہ FEAT_GUI_MSWIN
    اگر (using_titlebar != پچھلا_titlebar)
    {
	gui_mch_سیٹ_titlebar_colors();
	پچھلا_titlebar = using_titlebar;
    }
#اگر_ختم

#اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    اگر (using_fullscreen != پچھلا_fullscreen)
    {
	gui_mch_سیٹ_fullscreen(using_fullscreen);
	پچھلا_fullscreen = using_fullscreen;
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_DARKTHEME
    اگر (using_dark_theme != پچھلا_dark_theme)
    {
	gui_mch_سیٹ_dark_theme(using_dark_theme);
	پچھلا_dark_theme = using_dark_theme;
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    // Update the GUI tab line, it may appear or disappear.  This may
    // cause the non-GUI tab line to disappear or appear.
    using_tabline = gui_has_tabline();
    اگر (!gui_mch_showing_tabline() != !using_tabline)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = صفیں;
	gui_update_tabline();
	صفیں = i;
	need_سیٹ_سائز |= RESIZE_VERT;
	اگر (using_tabline)
	    fix_سائز = TRUE;
	اگر (!gui_use_tabline())
	    redraw_tabline = TRUE;    // may draw non-GUI tab line
    }
#اگر_ختم

    برائے (i = 0; i < 3; i++)
    {
	// The scrollbar needs to be updated when it is shown/unshown and
	// when switching tab pages.  But the size only changes when it's
	// shown/unshown.  Thus we need two places to remember whether a
	// scrollbar is there or not.
	اگر (gui.which_scrollbars[i] != پچھلا_which_scrollbars[i]
		|| gui.which_scrollbars[i]
		!= curtab->tp_پچھلا_which_scrollbars[i])
	{
	    اگر (i == SBAR_BOTTOM)
		gui_mch_enable_scrollbar(&gui.bottom_sbar,
			gui.which_scrollbars[i]);
	    ورنہ
	    {
		FOR_ALL_WINDOWS(wp)
		    gui_do_scrollbar(wp, i, gui.which_scrollbars[i]);
	    }
	    اگر (gui.which_scrollbars[i] != پچھلا_which_scrollbars[i])
	    {
		اگر (i == SBAR_BOTTOM)
		    need_سیٹ_سائز |= RESIZE_VERT;
		ورنہ
		    need_سیٹ_سائز |= RESIZE_HOR;
		اگر (gui.which_scrollbars[i])
		    fix_سائز = TRUE;
	    }
	}
	curtab->tp_پچھلا_which_scrollbars[i] = gui.which_scrollbars[i];
	پچھلا_which_scrollbars[i] = gui.which_scrollbars[i];
    }

#اگر_تعریف_شدہ FEAT_MENU
    اگر (gui.menu_is_فعال != پچھلا_menu_is_فعال)
    {
	// We don't want a resize event change "Rows" here, save and
	// restore it.  Resizing is handled below.
	i = صفیں;
	gui_mch_enable_menu(gui.menu_is_فعال);
	صفیں = i;
	پچھلا_menu_is_فعال = gui.menu_is_فعال;
	need_سیٹ_سائز |= RESIZE_VERT;
	اگر (gui.menu_is_فعال)
	    fix_سائز = TRUE;
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TOOLBAR
    اگر (using_toolbar != پچھلا_toolbar)
    {
	gui_mch_show_toolbar(using_toolbar);
	پچھلا_toolbar = using_toolbar;
	need_سیٹ_سائز |= RESIZE_VERT;
	اگر (using_toolbar)
	    fix_سائز = TRUE;
    }
#اگر_ختم
#اگر defined(FEAT_MENU) && !(defined(MSWIN) && !defined(FEAT_TEAROFF))
    اگر (using_tearoff != پچھلا_tearoff)
    {
	gui_mch_toggle_tearoffs(using_tearoff);
	پچھلا_tearoff = using_tearoff;
    }
#اگر_ختم
    اگر (need_سیٹ_سائز != 0)
    {
#اگر_تعریف_شدہ FEAT_GUI_GTK
	طویل    پچھلا_Columns = Columns;
	طویل    پچھلا_صفیں = صفیں;
#اگر_ختم
	// Adjust the size of the window to make the text area keep the
	// same size and to avoid that part of our window is off-screen
	// and a scrollbar can't be used, for example.
	gui_سیٹ_shellsize(FALSE, fix_سائز, need_سیٹ_سائز);

#اگر_تعریف_شدہ FEAT_GUI_GTK
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
	    طویل    post_صفیں = صفیں;
	    طویل    post_Columns = Columns;

	    اگر (!gui.starting && need_سیٹ_سائز != (RESIZE_VERT | RESIZE_HOR))
		(خالی)char_avail();
	    اگر ((need_سیٹ_سائز & RESIZE_VERT) == 0)
		صفیں = پچھلا_صفیں;
	    ورنہ
		صفیں = post_صفیں;
	    اگر ((need_سیٹ_سائز & RESIZE_HOR) == 0)
		Columns = پچھلا_Columns;
	    ورنہ
		Columns = post_Columns;
	}
#اگر_ختم
    }
    // When the console tabline appears or disappears the window positions
    // change.
    اگر (firstwin->w_winrow != tabline_اونچائی())
	shell_new_صفیں();	// recompute window positions and heights
}

#اگر defined(FEAT_GUI_TABLINE)
/*
 * Return TRUE if the GUI is taking care of the tabline.
 * It may still be hidden if 'showtabline' is zero.
 */
    صحیح
gui_use_tabline(خالی)
{
    واپس gui.اندر_use && vim_strchr(p_go, GO_TABLINE) != تہی;
}

/*
 * Return TRUE if the GUI is showing the tabline.
 * This uses 'showtabline'.
 */
    ساکن صحیح
gui_has_tabline(خالی)
{
    اگر (!gui_use_tabline()
	    || p_stal == 0
	    || (p_stal == 1 && پہلا_tabpage->tp_اگلا == تہی))
	واپس FALSE;
    واپس TRUE;
}

/*
 * Update the tabline.
 * This may display/undisplay the tabline and update the labels.
 */
    خالی
gui_update_tabline(خالی)
{
    صحیح	    showit = gui_has_tabline();
    صحیح	    shown = gui_mch_showing_tabline();

    اگر (!gui.starting && starting == 0)
    {
	// Updating the tabline uses direct GUI commands, flush
	// outstanding instructions first. (esp. clear screen)
	برآمد_flush();

	اگر (!showit != !shown)
	{
	    // Save Rows/Columns before showing/hiding the tabline.
	    // gui_mch_show_tabline() processes GTK events (via
	    // gui_mch_update) which may trigger a spurious configure event
	    // that temporarily corrupts Rows.  Restore the original values so
	    // that gui_set_shellsize() uses the correct row/column count.
	    صحیح save_صفیں = صفیں;
	    صحیح save_Columns = Columns;

	    gui_mch_show_tabline(showit);
	    صفیں = save_صفیں;
	    Columns = save_Columns;

	    // Resize the outer window to compensate for the tabline height
	    // change.  This is also needed when called from draw_tabline()
	    // (e.g. after :tabclose), where no caller will do the resize.
	    // When called from gui_init_which_components() the subsequent
	    // gui_set_shellsize() call will redo this to the same dimensions,
	    // which is harmless.
	    gui_سیٹ_shellsize(FALSE, showit, RESIZE_VERT);
	}
	اگر (showit != 0)
	    gui_mch_update_tabline();
    }
}

/*
 * Get the label or tooltip for tab page "tp" into NameBuff[].
 */
    خالی
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

	printer_صفحہ_عدد = tabpage_اشاریہ(tp);
# ifdef FEAT_EVAL
	سیٹ_vim_var_nr(VV_LNUM, printer_صفحہ_عدد);
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
					   0, (صحیح)Columns, تہی, تہی, تہی);
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
    اگر (**opt == NUL || *NameBuff == NUL)
    {
	win_T	*wp;
	صحیح	wincount = 0;
	صحیح	modified = FALSE;

	// Get the buffer name into NameBuff[] and shorten it.
	get_trans_bufname(tp == curtab ? curbuf : tp->tp_curwin->w_بفر);
	اگر (!tooltip)
	    shorten_dir(NameBuff);

	FOR_ALL_WINDOWS_اندر_TAB(tp, wp)
	{
	    ++wincount;
	    اگر (buf_Is_بدلا(wp->w_بفر))
		modified = TRUE;
	}

	اگر (modified || wincount > 1)
	{
	    char_u  buf[40] = "+ ";		// Tentatively assume modified only
	    size_t  buflen = 2;
	    size_t  NameBufflen = STRLEN(NameBuff);

	    اگر (wincount > 1)
		buflen = vim_snprintf_safelen(
		    (حرف *)buf,
		    حجم(buf),
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

    اگر (nr == tabpage_اشاریہ(curtab))
	واپس FALSE;

    // Don't put events in the input queue now.
    اگر (hold_gui_events || cmdwin_type != 0)
    {
	// Set it back to the current tab page.
	gui_mch_سیٹ_curtab(tabpage_اشاریہ(curtab));
	واپس FALSE;
    }

    string[0] = CSI;
    string[1] = KS_TABLINE;
    string[2] = KE_FILLER;
    جوڑو_مقصد_داخلہ_buf(string, 3);
    string[0] = nr;
    جوڑو_مقصد_داخلہ_buf_csi(string, 1);
    واپس TRUE;
}

/*
 * Send a tabline menu event
 */
    خالی
send_tabline_menu_event(صحیح tabidx, صحیح event)
{
    char_u	    string[3];

    // Don't put events in the input queue now.
    اگر (hold_gui_events)
	واپس;

    // Cannot close the last tabpage.
    اگر (event == TABLINE_MENU_CLOSE && پہلا_tabpage->tp_اگلا == تہی)
	واپس;

    string[0] = CSI;
    string[1] = KS_TABMENU;
    string[2] = KE_FILLER;
    جوڑو_مقصد_داخلہ_buf(string, 3);
    string[0] = tabidx;
    string[1] = (char_u)(طویل)event;
    جوڑو_مقصد_داخلہ_buf_csi(string, 2);
}

#اگر_ختم

/*
 * Scrollbar stuff:
 */

/*
 * Remove all scrollbars.  Used before switching to another tab page.
 */
    خالی
gui_remove_scrollbars(خالی)
{
    صحیح	    i;
    win_T   *wp;

    برائے (i = 0; i < 3; i++)
    {
	اگر (i == SBAR_BOTTOM)
	    gui_mch_enable_scrollbar(&gui.bottom_sbar, FALSE);
	ورنہ
	{
	    FOR_ALL_WINDOWS(wp)
		gui_do_scrollbar(wp, i, FALSE);
	}
	curtab->tp_پچھلا_which_scrollbars[i] = -1;
    }
}

    خالی
gui_create_scrollbar(scrollbar_T *sb, صحیح type, win_T *wp)
{
    ساکن صحیح	sbar_ident = 0;

    sb->ident = sbar_ident++;	// No check for too big, but would it happen?
    sb->wp = wp;
    sb->type = type;
    sb->قدر = 0;
    sb->سائز = 1;
    sb->max = 1;
    sb->چوٹی = 0;
    sb->اونچائی = 0;
    sb->چوڑائی = 0;
    sb->حالت_اونچائی = 0;
    gui_mch_create_scrollbar(sb, (wp == تہی) ? SBAR_HORIZ : SBAR_VERT);
}

/*
 * Find the scrollbar with the given index.
 */
    scrollbar_T *
gui_ڈھونڈو_scrollbar(طویل ident)
{
    win_T	*wp;

    اگر (gui.bottom_sbar.ident == ident)
	واپس &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	اگر (wp->w_scrollbars[SBAR_بایاں].ident == ident)
	    واپس &wp->w_scrollbars[SBAR_بایاں];
	اگر (wp->w_scrollbars[SBAR_دایاں].ident == ident)
	    واپس &wp->w_scrollbars[SBAR_دایاں];
    }
    واپس تہی;
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
    خالی
gui_drag_scrollbar(scrollbar_T *sb, طویل قدر, صحیح still_dragging)
{
    win_T	*wp;
    صحیح		sb_عدد;
#اگر_تعریف_شدہ USE_ON_FLY_SCROLL
    colnr_T	old_leftcol = curwin->w_leftcol;
    linenr_T	old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    صحیح		old_topfill = curwin->w_topfill;
# endif
#ورنہ
    char_u	بائٹیں[حجم(long_u)];
    صحیح		byte_گنتی;
#اگر_ختم

    اگر (sb == تہی)
	واپس;

    // Don't put events in the input queue now.
    اگر (hold_gui_events)
	واپس;

    اگر (cmdwin_type != 0 && sb->wp != cmdwin_win)
	واپس;

    اگر (still_dragging)
    {
	اگر (sb->wp == تہی)
	    gui.dragged_sb = SBAR_BOTTOM;
	ورنہ اگر (sb == &sb->wp->w_scrollbars[SBAR_بایاں])
	    gui.dragged_sb = SBAR_بایاں;
	ورنہ
	    gui.dragged_sb = SBAR_دایاں;
	gui.dragged_wp = sb->wp;
    }
    ورنہ
    {
	gui.dragged_sb = SBAR_NONE;
#اگر_تعریف_شدہ FEAT_GUI_GTK
	// Keep the "dragged_wp" value until after the scrolling, for when the
	// mouse button is released.  GTK2 doesn't send the button-up event.
	gui.dragged_wp = تہی;
	// WinScrolled event
	gui_focus_change(TRUE);
#اگر_ختم
    }

    // Vertical sbar info is kept in the first sbar (the left one)
    اگر (sb->wp != تہی)
	sb = &sb->wp->w_scrollbars[0];

    /*
     * Check validity of value
     */
    اگر (قدر < 0)
	قدر = 0;
#اگر_تعریف_شدہ SCROLL_PAST_انتہا
    ورنہ اگر (قدر > sb->max)
	قدر = sb->max;
#ورنہ
    اگر (قدر > sb->max - sb->سائز + 1)
	قدر = sb->max - sb->سائز + 1;
#اگر_ختم

    sb->قدر = قدر;

#اگر_تعریف_شدہ USE_ON_FLY_SCROLL
    // When not allowed to do the scrolling right now, return.
    // This also checked input_available(), but that causes the first click in
    // a scrollbar to be ignored when Vim doesn't have focus.
    اگر (dont_scroll)
	واپس;
#اگر_ختم
    // Disallow scrolling the current window when the completion popup menu is
    // visible.
    اگر ((sb->wp == تہی || sb->wp == curwin) && pum_visible())
	واپس;

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (sb->wp == تہی && curwin->w_p_rl)
    {
	قدر = sb->max + 1 - sb->سائز - قدر;
	اگر (قدر < 0)
	    قدر = 0;
    }
#اگر_ختم

    اگر (sb->wp != تہی)		// vertical scrollbar
    {
	sb_عدد = 0;
	برائے (wp = firstwin; wp != sb->wp && wp != تہی; wp = wp->w_اگلا)
	    sb_عدد++;
	اگر (wp == تہی)
	    واپس;

#اگر_تعریف_شدہ USE_ON_FLY_SCROLL
	current_scrollbar = sb_عدد;
	scrollbar_قدر = قدر;
	اگر (State & منوال_معمول)
	{
	    gui_do_scroll();
	    setcursor();
	}
	ورنہ اگر (State & منوال_داخل_کرو)
	{
	    ins_scroll();
	    setcursor();
	}
	ورنہ اگر (State & منوال_CMDLINE)
	{
	    اگر (پیغام_scrolled == 0)
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
	اگر (gui.which_scrollbars[SBAR_دایاں] && gui.which_scrollbars[SBAR_بایاں])
	    gui_mch_سیٹ_scrollbar_thumb(
		    &sb->wp->w_scrollbars[
			    sb == &sb->wp->w_scrollbars[SBAR_دایاں]
						    ? SBAR_بایاں : SBAR_دایاں],
		    sb->قدر, sb->سائز, sb->max);

#ورنہ
	بائٹیں[0] = CSI;
	بائٹیں[1] = KS_VER_SCROLLBAR;
	بائٹیں[2] = KE_FILLER;
	بائٹیں[3] = (char_u)sb_عدد;
	byte_گنتی = 4;
#اگر_ختم
    }
    ورنہ
    {
#اگر_تعریف_شدہ USE_ON_FLY_SCROLL
	scrollbar_قدر = قدر;

	اگر (State & منوال_معمول)
	    do_mousescroll_horiz(scrollbar_قدر);
	ورنہ اگر (State & منوال_داخل_کرو)
	    ins_horscroll();
	ورنہ اگر (State & منوال_CMDLINE)
	{
	    اگر (پیغام_scrolled == 0)
	    {
		do_mousescroll_horiz(scrollbar_قدر);
		redrawcmdline();
	    }
	}
	اگر (old_leftcol != curwin->w_leftcol)
	{
	    update_کھڑکی(curwin);   // update window, status and cmdline
	    setcursor();
	}
#ورنہ
	بائٹیں[0] = CSI;
	بائٹیں[1] = KS_HOR_SCROLLBAR;
	بائٹیں[2] = KE_FILLER;
	byte_گنتی = 3;
#اگر_ختم
    }

#اگر_تعریف_شدہ USE_ON_FLY_SCROLL
    /*
     * synchronize other windows, as necessary according to 'scrollbind'
     */
    اگر (curwin->w_p_scb
	    && ((sb->wp == تہی && curwin->w_leftcol != old_leftcol)
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
		update_کھڑکی(wp);
	setcursor();
    }
    برآمد_flush_cursor(FALSE, TRUE);
#ورنہ
    جوڑو_مقصد_داخلہ_buf(بائٹیں, byte_گنتی);
    جوڑو_long_مقصد_buf((long_u)قدر, بائٹیں);
    جوڑو_مقصد_داخلہ_buf_csi(بائٹیں, حجم(long_u));
#اگر_ختم
}

/*
 * Scrollbar stuff:
 */

/*
 * Called when something in the window layout has changed.
 */
    خالی
gui_may_update_scrollbars(خالی)
{
    اگر (gui.اندر_use && starting == 0)
    {
	برآمد_flush();
	gui_init_which_components(تہی);
	gui_update_scrollbars(TRUE);
    }
    need_mouse_correct = TRUE;
}

    خالی
gui_update_scrollbars(
    صحیح		force)	    // Force all scrollbars to get updated
{
    win_T	*wp;
    scrollbar_T	*sb;
    طویل	val, سائز, max;		// need 32 bits here
    صحیح		which_sb;
    صحیح		h, y;
    ساکن win_T *پچھلا_curwin = تہی;

    // Update the horizontal scrollbar
    gui_update_horiz_scrollbar(force);

#اگر_غیر_تعریف_شدہ MSWIN
    // Return straight away if there is neither a left nor right scrollbar.
    // On MS-Windows this is required anyway for scrollwheel messages.
    اگر (!gui.which_scrollbars[SBAR_بایاں] && !gui.which_scrollbars[SBAR_دایاں])
	واپس;
#اگر_ختم

    /*
     * Don't want to update a scrollbar while we're dragging it.  But if we
     * have both a left and right scrollbar, and we drag one of them, we still
     * need to update the other one.
     */
    اگر (!force && (gui.dragged_sb == SBAR_بایاں || gui.dragged_sb == SBAR_دایاں)
	    && gui.which_scrollbars[SBAR_بایاں]
	    && gui.which_scrollbars[SBAR_دایاں])
    {
	/*
	 * If we have two scrollbars and one of them is being dragged, just
	 * copy the scrollbar position from the dragged one to the other one.
	 */
	which_sb = SBAR_بایاں + SBAR_دایاں - gui.dragged_sb;
	اگر (gui.dragged_wp != تہی)
	    gui_mch_سیٹ_scrollbar_thumb(
		    &gui.dragged_wp->w_scrollbars[which_sb],
		    gui.dragged_wp->w_scrollbars[0].قدر,
		    gui.dragged_wp->w_scrollbars[0].سائز,
		    gui.dragged_wp->w_scrollbars[0].max);
    }

    // avoid that moving components around generates events
    ++hold_gui_events;

    FOR_ALL_WINDOWS(wp)
    {
	اگر (wp->w_بفر == تہی)	// just in case
	    جاری;
	// Skip a scrollbar that is being dragged.
	اگر (!force && (gui.dragged_sb == SBAR_بایاں
					     || gui.dragged_sb == SBAR_دایاں)
		&& gui.dragged_wp == wp)
	    جاری;

#اگر_تعریف_شدہ SCROLL_PAST_انتہا
	max = wp->w_بفر->b_ml.ml_سطر_گنتی - 1;
#ورنہ
	max = wp->w_بفر->b_ml.ml_سطر_گنتی + wp->w_اونچائی - 2;
#اگر_ختم
	اگر (max < 0)			// empty buffer
	    max = 0;
	val = wp->w_topline - 1;
	سائز = wp->w_اونچائی;
#اگر_تعریف_شدہ SCROLL_PAST_انتہا
	اگر (val > max)			// just in case
	    val = max;
#ورنہ
	اگر (سائز > max + 1)		// just in case
	    سائز = max + 1;
	اگر (val > max - سائز + 1)
	    val = max - سائز + 1;
#اگر_ختم
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
	اگر (سائز < 1 || wp->w_botline - 2 > max)
	{
	    /*
	     * This can happen during changing files.  Just don't update the
	     * scrollbar for now.
	     */
	    sb->اونچائی = 0;	    // Force update next time
	    اگر (gui.which_scrollbars[SBAR_بایاں])
		gui_do_scrollbar(wp, SBAR_بایاں, FALSE);
	    اگر (gui.which_scrollbars[SBAR_دایاں])
		gui_do_scrollbar(wp, SBAR_دایاں, FALSE);
	    جاری;
	}
	اگر (force || sb->اونچائی != wp->w_اونچائی
	    || sb->چوٹی != wp->w_winrow
	    || sb->حالت_اونچائی != wp->w_حالت_اونچائی
	    || sb->چوڑائی != wp->w_چوڑائی
	    || پچھلا_curwin != curwin)
	{
	    // Height, width or position of scrollbar has changed.  For
	    // vertical split: curwin changed.
	    sb->اونچائی = wp->w_اونچائی;
	    sb->چوٹی = wp->w_winrow;
	    sb->حالت_اونچائی = wp->w_حالت_اونچائی;
	    sb->چوڑائی = wp->w_چوڑائی;

	    // Calculate height and position in pixels
	    h = (sb->اونچائی + sb->حالت_اونچائی) * gui.char_اونچائی;
	    y = sb->چوٹی * gui.char_اونچائی + gui.border_انحراف;
#اگر defined(FEAT_MENU) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF) && !defined(FEAT_GUI_PHOTON)
	    اگر (gui.menu_is_فعال)
		y += gui.menu_اونچائی;
#اگر_ختم

#اگر defined(FEAT_TOOLBAR) && (defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU))
	    اگر (vim_strchr(p_go, GO_TOOLBAR) != تہی)
		y += gui.toolbar_اونچائی;
#اگر_ختم

#اگر defined(FEAT_GUI_TABLINE) && defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
	    اگر (gui_has_tabline())
		y += gui.tabline_اونچائی;
#اگر_ختم

	    اگر (wp->w_winrow == 0)
	    {
		// Height of top scrollbar includes width of top border
		h += gui.border_انحراف;
		y -= gui.border_انحراف;
	    }
	    اگر (gui.which_scrollbars[SBAR_بایاں])
	    {
		gui_mch_سیٹ_scrollbar_pos(&wp->w_scrollbars[SBAR_بایاں],
					  gui.بایاں_sbar_x, y,
					  gui.scrollbar_چوڑائی, h);
		gui_do_scrollbar(wp, SBAR_بایاں, TRUE);
	    }
	    اگر (gui.which_scrollbars[SBAR_دایاں])
	    {
		gui_mch_سیٹ_scrollbar_pos(&wp->w_scrollbars[SBAR_دایاں],
					  gui.دایاں_sbar_x, y,
					  gui.scrollbar_چوڑائی, h);
		gui_do_scrollbar(wp, SBAR_دایاں, TRUE);
	    }
	}

	اگر (force || sb->قدر != val || sb->سائز != سائز || sb->max != max)
	{
	    // Thumb of scrollbar has moved
	    sb->قدر = val;
	    sb->سائز = سائز;
	    sb->max = max;
	    اگر (gui.which_scrollbars[SBAR_بایاں]
		    && (gui.dragged_sb != SBAR_بایاں || gui.dragged_wp != wp))
		gui_mch_سیٹ_scrollbar_thumb(&wp->w_scrollbars[SBAR_بایاں],
					    val, سائز, max);
	    اگر (gui.which_scrollbars[SBAR_دایاں]
		    && (gui.dragged_sb != SBAR_دایاں || gui.dragged_wp != wp))
		gui_mch_سیٹ_scrollbar_thumb(&wp->w_scrollbars[SBAR_دایاں],
					    val, سائز, max);
	}
    }

    // update the title, it may show the scroll position
    maketitle();

    پچھلا_curwin = curwin;
    --hold_gui_events;
}

/*
 * Enable or disable a scrollbar.
 * Check for scrollbars for vertically split windows which are not enabled
 * sometimes.
 */
    ساکن خالی
gui_do_scrollbar(
    win_T	*wp,
    صحیح		which,	    // SBAR_LEFT or SBAR_RIGHT
    صحیح		enable)	    // TRUE to enable scrollbar
{
    صحیح		midcol = curwin->w_wincol + curwin->w_چوڑائی / 2;
    صحیح		has_midcol = (wp->w_wincol <= midcol
				     && wp->w_wincol + wp->w_چوڑائی >= midcol);

    // Only enable scrollbars that contain the middle column of the current
    // window.
    اگر (gui.which_scrollbars[SBAR_دایاں] != gui.which_scrollbars[SBAR_بایاں])
    {
	// Scrollbars only on one side.  Don't enable scrollbars that don't
	// contain the middle column of the current window.
	اگر (!has_midcol)
	    enable = FALSE;
    }
    ورنہ
    {
	// Scrollbars on both sides.  Don't enable scrollbars that neither
	// contain the middle column of the current window nor are on the far
	// side.
	اگر (midcol > Columns / 2)
	{
	    اگر (which == SBAR_بایاں ? wp->w_wincol != 0 : !has_midcol)
		enable = FALSE;
	}
	ورنہ
	{
	    اگر (which == SBAR_دایاں ? wp->w_wincol + wp->w_چوڑائی != Columns
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
gui_do_scroll(خالی)
{
    win_T	*wp, *save_wp;
    صحیح		i;
    طویل	nlines;
    pos_T	old_cursor;
    linenr_T	old_topline;
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		old_topfill;
#اگر_ختم

    برائے (wp = firstwin, i = 0; i < current_scrollbar; wp = W_اگلا(wp), i++)
	اگر (wp == تہی)
	    توڑو;
    اگر (wp == تہی)
	// Couldn't find window
	واپس FALSE;
    // don't redraw, LineOffset and similar are not valid!
    اگر (exmode_فعال)
	واپس FALSE;

    /*
     * Compute number of lines to scroll.  If zero, nothing to do.
     */
    nlines = (طویل)scrollbar_قدر + 1 - (طویل)wp->w_topline;
    اگر (nlines == 0)
	واپس FALSE;

    save_wp = curwin;
    old_topline = wp->w_topline;
#اگر_تعریف_شدہ FEAT_DIFF
    old_topfill = wp->w_topfill;
#اگر_ختم
    old_cursor = wp->w_cursor;
    curwin = wp;
    curbuf = wp->w_بفر;
    اگر (nlines < 0)
	scrolldown(-nlines, gui.dragged_wp == تہی);
    ورنہ
	scrollup(nlines, gui.dragged_wp == تہی);
    // Reset dragged_wp after using it.  "dragged_sb" will have been reset for
    // the mouse-up event already, but we still want it to behave like when
    // dragging.  But not the next click in an arrow.
    اگر (gui.dragged_sb == SBAR_NONE)
	gui.dragged_wp = تہی;

    اگر (old_topline != wp->w_topline
#اگر_تعریف_شدہ FEAT_DIFF
	    || old_topfill != wp->w_topfill
#اگر_ختم
	    )
    {
	اگر (get_scrolloff_قدر() != 0)
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
    curbuf = save_wp->w_بفر;

    /*
     * Don't call updateWindow() when nothing has changed (it will overwrite
     * the status line!).
     *
     * Check for ScreenLines, because in ex-mode, we don't have a valid display.
     */
    اگر (Screen_سطریں != تہی && (old_topline != wp->w_topline
	    || wp->w_redr_type != 0
#اگر_تعریف_شدہ FEAT_DIFF
	    || old_topfill != wp->w_topfill
#اگر_ختم
	    ))
    {
	صحیح type = UPD_VALID;

	اگر (pum_visible())
	{
	    type = UPD_NOT_VALID;
	    wp->w_سطریں_valid = 0;
	}

	// Don't set must_redraw here, it may cause the popup menu to
	// disappear when losing focus after a scrollbar drag.
	اگر (wp->w_redr_type < type)
	    wp->w_redr_type = type;
	mch_disable_flush();
	update_کھڑکی(wp);   // update window, status line, and cmdline
	mch_enable_flush();
    }

    // May need to redraw the popup menu.
    اگر (pum_visible())
	pum_redraw();

    واپس (wp == curwin && !EQUAL_POS(curwin->w_cursor, old_cursor));
}

/*
 * Horizontal scrollbar stuff:
 */
    ساکن خالی
gui_update_horiz_scrollbar(صحیح force)
{
    طویل	قدر, سائز, max;

    اگر (!gui.which_scrollbars[SBAR_BOTTOM])
	واپس;

    اگر (!force && gui.dragged_sb == SBAR_BOTTOM)
	واپس;

    اگر (!force && curwin->w_p_wrap && gui.پچھلا_wrap)
	واپس;

    /*
     * It is possible for the cursor to be invalid if we're in the middle of
     * something (like changing files).  If so, don't do anything for now.
     */
    اگر (curwin->w_cursor.lnum > curbuf->b_ml.ml_سطر_گنتی)
    {
	gui.bottom_sbar.قدر = -1;
	واپس;
    }

    سائز = curwin->w_چوڑائی;
    اگر (curwin->w_p_wrap)
    {
	قدر = 0;
#اگر_تعریف_شدہ SCROLL_PAST_انتہا
	max = 0;
#ورنہ
	max = curwin->w_چوڑائی - 1;
#اگر_ختم
    }
    ورنہ
    {
	قدر = curwin->w_leftcol;
	max = scroll_سطر_len(ui_ڈھونڈو_longest_lnum());

	اگر (virtual_فعال())
	{
	    // May move the cursor even further to the right.
	    اگر (curwin->w_virtcol >= (colnr_T)max)
		max = curwin->w_virtcol;
	}

#اگر_غیر_تعریف_شدہ SCROLL_PAST_انتہا
	max += curwin->w_چوڑائی - 1;
#اگر_ختم
	// The line number isn't scrolled, thus there is less space when
	// 'number' or 'relativenumber' is set (also for 'foldcolumn').
	سائز -= curwin_ستون_off();
#اگر_غیر_تعریف_شدہ SCROLL_PAST_انتہا
	max -= curwin_ستون_off();
#اگر_ختم
    }

#اگر_غیر_تعریف_شدہ SCROLL_PAST_انتہا
    اگر (قدر > max - سائز + 1)
	قدر = max - سائز + 1;	    // limit the value to allowable range
#اگر_ختم

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (curwin->w_p_rl)
    {
	قدر = max + 1 - سائز - قدر;
	اگر (قدر < 0)
	{
	    سائز += قدر;
	    قدر = 0;
	}
    }
#اگر_ختم
    اگر (!force && قدر == gui.bottom_sbar.قدر && سائز == gui.bottom_sbar.سائز
						&& max == gui.bottom_sbar.max)
	واپس;

    gui.bottom_sbar.قدر = قدر;
    gui.bottom_sbar.سائز = سائز;
    gui.bottom_sbar.max = max;
    gui.پچھلا_wrap = curwin->w_p_wrap;

    gui_mch_سیٹ_scrollbar_thumb(&gui.bottom_sbar, قدر, سائز, max);
}

/*
 * Check that none of the colors are the same as the background color
 */
    خالی
gui_check_colors(خالی)
{
    اگر (gui.norm_pixel == gui.عقب_pixel || gui.norm_pixel == INVALCOLOR)
    {
	gui_سیٹ_bg_color((char_u *)"White");
	اگر (gui.norm_pixel == gui.عقب_pixel || gui.norm_pixel == INVALCOLOR)
	    gui_سیٹ_fg_color((char_u *)"Black");
    }
}

    ساکن خالی
gui_سیٹ_fg_color(char_u *name)
{
    gui.norm_pixel = gui_get_color(name);
    hl_سیٹ_fg_color_name(vim_strsave(name));
}

    ساکن خالی
gui_سیٹ_bg_color(char_u *name)
{
    gui.عقب_pixel = gui_get_color(name);
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

    اگر (*name == NUL)
	واپس INVALCOLOR;
    t = gui_mch_get_color(name);

    صحیح is_none = STRCMP(name, "none") == 0;
    اگر (t == INVALCOLOR
#اگر defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
	    && (gui.اندر_use || is_none)
#اگر_ختم
	    )
    {
	اگر (is_none)
	    emsg(_(e_cannot_use_color_none_did_you_اوسط_none));
	ورنہ
	    semsg(_(e_cannot_allocate_color_str), name);
    }
    واپس t;
}

/*
 * Return the grey value of a color (range 0-255).
 */
    صحیح
gui_get_lightness(guicolor_T pixel)
{
    long_u	rgb = (long_u)gui_mch_get_rgb(pixel);

    واپس  (صحیح)(  (((rgb >> 16) & 0xff) * 299)
		   + (((rgb >> 8) & 0xff) * 587)
		   +  ((rgb	  & 0xff) * 114)) / 1000;
}

    char_u *
gui_bg_default(خالی)
{
    اگر (gui_get_lightness(gui.عقب_pixel) < 127)
	واپس (char_u *)"dark";
    واپس (char_u *)"light";
}

/*
 * Option initializations that can only be done after opening the GUI window.
 */
    ساکن خالی
init_gui_options(خالی)
{
    // Set the 'background' option according to the lightness of the
    // background color, unless the user has set it already.
    اگر (!option_was_سیٹ((char_u *)"bg") && STRCMP(p_bg, gui_bg_default()) != 0)
    {
	سیٹ_option_قدر_give_err((char_u *)"bg", 0L, gui_bg_default(), 0);
	highlight_بدلا();
    }
}

#اگر defined(FEAT_GUI_X11)
    خالی
gui_new_scrollbar_colors(خالی)
{
    win_T	*wp;

    // Nothing to do if GUI hasn't started yet.
    اگر (!gui.اندر_use)
	واپس;

    FOR_ALL_WINDOWS(wp)
    {
	gui_mch_سیٹ_scrollbar_colors(&(wp->w_scrollbars[SBAR_بایاں]));
	gui_mch_سیٹ_scrollbar_colors(&(wp->w_scrollbars[SBAR_دایاں]));
    }
    gui_mch_سیٹ_scrollbar_colors(&gui.bottom_sbar);
}
#اگر_ختم

/*
 * Call this when focus has changed.
 */
    خالی
gui_focus_change(صحیح اندر_focus)
{
/*
 * Skip this code to avoid drawing the cursor when debugging and switching
 * between the debugger window and gvim.
 */
#اگر 1
    gui.اندر_focus = اندر_focus;
    برآمد_flush_cursor(TRUE, FALSE);

# ifdef FEAT_GUI_MSWIN
    gui_mch_سیٹ_titlebar_colors();
# endif

# ifdef FEAT_XIM
    xim_سیٹ_focus(اندر_focus);
# endif

    // Put events in the input queue only when allowed.
    // ui_focus_change() isn't called directly, because it invokes
    // autocommands and that must not happen asynchronously.
    اگر (!hold_gui_events
# اگر defined(FEAT_GUI_GTK) && defined(FEAT_GUI_DIALOG)
	    && gui.dialogs_فعال == 0
# endif
       )
    {
	char_u  بائٹیں[3];

	بائٹیں[0] = CSI;
	بائٹیں[1] = KS_EXTRA;
	بائٹیں[2] = اندر_focus ? (صحیح)KE_FOCUSGAINED : (صحیح)KE_FOCUSLOST;
	جوڑو_مقصد_داخلہ_buf(بائٹیں, 3);
    }
#اگر_ختم
}

/*
 * When mouse moved: apply 'mousefocus'.
 * Also updates the mouse pointer shape.
 */
    ساکن خالی
gui_mouse_focus(صحیح x, صحیح y)
{
    win_T	*wp;
    char_u	st[8];

#اگر_تعریف_شدہ FEAT_MOUSESHAPE
    // Get window pointer, and update mouse shape as well.
    wp = xy2win(x, y, IGNORE_POPUP);
#اگر_ختم

    // Only handle this when 'mousefocus' set and ...
    اگر (p_mousef
	    && !hold_gui_events		// not holding events
	    && (State & (منوال_معمول | منوال_داخل_کرو))// Normal/Visual/Insert mode
	    && State != منوال_HITRETURN	// but not hit-return prompt
	    && پیغام_scrolled == 0	// no scrolled message
	    && !need_mouse_correct	// not moving the pointer
	    && gui.اندر_focus)		// gvim in focus
    {
	// Don't move the mouse when it's left or right of the Vim window
	اگر (x < 0 || x > Columns * gui.char_چوڑائی)
	    واپس;
#اگر_غیر_تعریف_شدہ FEAT_MOUSESHAPE
	wp = xy2win(x, y, IGNORE_POPUP);
#اگر_ختم
	اگر (wp == curwin || wp == تہی)
	    واپس;	// still in the same old window, or none at all

	// Ignore position in the tab pages line.
	اگر (Y_2_صف(y) < tabline_اونچائی())
	    واپس;

	/*
	 * Format a mouse click on status line input,
	 * ala gui_send_mouse_event(0, x, y, 0, 0);
	 * Trick: Use a column number -1, so that get_pseudo_mouse_code() will
	 * generate a K_LEFTMOUSE_NM key code.
	 */
	اگر (اختتام_op)
	{
	    // abort the current operator first
	    st[0] = ESC;
	    جوڑو_مقصد_داخلہ_buf(st, 1);
	}
	st[0] = CSI;
	st[1] = KS_MOUSE;
	st[2] = KE_FILLER;
	st[3] = (char_u)MOUSE_بایاں;
	fill_mouse_coord(st + 4,
		wp->w_wincol == 0 ? -1 : wp->w_wincol + MOUSE_COLOFF,
		wp->w_اونچائی + W_WINROW(wp));

	جوڑو_مقصد_داخلہ_buf(st, 8);
	st[3] = (char_u)MOUSE_RELEASE;
	جوڑو_مقصد_داخلہ_buf(st, 8);
#اگر_تعریف_شدہ FEAT_GUI_GTK
	// Need to wake up the main loop
	اگر (gtk_main_level() > 0)
	    gtk_main_quit();
#اگر_ختم
    }
}

/*
 * Called when the mouse moved (but not when dragging).
 */
    خالی
gui_mouse_moved(صحیح x, صحیح y)
{
    // Ignore this while still starting up.
    اگر (!gui.اندر_use || gui.starting)
	واپس;

    // apply 'mousefocus' and pointer shape
    gui_mouse_focus(x, y);

    اگر (p_mousemev
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	|| popup_استعمال_کرتا_mouse_move
#اگر_ختم
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
gui_mouse_کھڑکی(mouse_ڈھونڈو_T popup)
{
    صحیح		x, y;

    اگر (!(gui.اندر_use && (p_mousef || popup == ڈھونڈو_POPUP)))
	واپس تہی;
    gui_mch_getmouse(&x, &y);

    // Only use the mouse when it's on the Vim window
    اگر (x >= 0 && x <= Columns * gui.char_چوڑائی
	    && y >= 0 && Y_2_صف(y) >= tabline_اونچائی())
	واپس xy2win(x, y, popup);
    واپس تہی;
}

/*
 * Called when mouse should be moved to window with focus.
 */
    خالی
gui_mouse_correct(خالی)
{
    win_T	*wp = تہی;

    need_mouse_correct = FALSE;

    wp = gui_mouse_کھڑکی(IGNORE_POPUP);
    اگر (wp == curwin || wp == تہی)
	واپس;

    // If in other than current window
    validate_cline_صف();
    gui_mch_setmouse((صحیح)W_ENDCOL(curwin) * gui.char_چوڑائی - 3,
	    (W_WINROW(curwin) + curwin->w_wrow) * gui.char_اونچائی
	    + (gui.char_اونچائی) / 2);
}

/*
 * Find window where the mouse pointer "x" / "y" coordinate is in.
 * As a side effect update the shape of the mouse pointer.
 */
    ساکن win_T *
xy2win(صحیح x, صحیح y, mouse_ڈھونڈو_T popup)
{
    صحیح		صف;
    صحیح		ستون;
    win_T	*wp;

    صف = Y_2_صف(y);
    ستون = X_2_ستون(x);

    اگر (صف < 0 || ستون < 0)		// before first window
	واپس تہی;
    wp = mouse_ڈھونڈو_win(&صف, &ستون, popup);
    اگر (wp == تہی)
    {
#اگر_تعریف_شدہ FEAT_MOUSESHAPE
	update_mouseshape(-2);
#اگر_ختم
	واپس تہی;
    }
#اگر_تعریف_شدہ FEAT_MOUSESHAPE
    اگر (State == منوال_HITRETURN || State == منوال_ASKMORE)
    {
	اگر (Y_2_صف(y) >= پیغام_صف)
	    update_mouseshape(SHAPE_IDX_MOREL);
	ورنہ
	    update_mouseshape(SHAPE_IDX_مزید);
    }
    ورنہ اگر (صف >= wp->w_اونچائی + wp->w_حالت_اونچائی)	// below status line
	update_mouseshape(SHAPE_IDX_CLINE);
    ورنہ اگر (!(State & منوال_CMDLINE) && wp->w_vsep_چوڑائی > 0 && ستون == wp->w_چوڑائی
	    && (!(صف >= wp->w_اونچائی && صف < wp->w_اونچائی
	    + wp->w_حالت_اونچائی) || !stl_connected(wp)) && پیغام_scrolled == 0)
	update_mouseshape(SHAPE_IDX_VSEP);
    ورنہ اگر (!(State & منوال_CMDLINE) && wp->w_حالت_اونچائی > 0
	    && صف >= wp->w_اونچائی && صف < wp->w_اونچائی + wp->w_حالت_اونچائی
	    && پیغام_scrolled == 0)
	update_mouseshape(SHAPE_IDX_حالت);
    ورنہ
	update_mouseshape(-2);
#اگر_ختم
    واپس wp;
}

/*
 * ":gui" and ":gvim": Change from the terminal version to the GUI version.
 * File names may be given to redefine the args list.
 */
    خالی
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
    اگر (!gui.اندر_use)
    {
#اگر defined(VIMDLL) && !defined(EXPERIMENTAL_GUI_CMD)
	اگر (!gui.starting)
	{
	    emsg(_(e_gui_cannot_be_مستعمل_not_enabled_at_compile_وقت));
	    واپس;
	}
#اگر_ختم
	// Clear the command.  Needed for when forking+exiting, to avoid part
	// of the argument ending up after the shell prompt.
	پیغام_clr_eos_force();
#اگر_تعریف_شدہ GUI_MAY_SPAWN
	اگر (!سرے_excmd2(eap->cmd, eap->arg))
	    gui_آغاز(eap->arg);
	ورنہ
#اگر_ختم
	    gui_آغاز(تہی);
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	channel_gui_رجسٹر_all();
#اگر_ختم
    }
    اگر (!سرے_excmd2(eap->cmd, eap->arg))
	ex_اگلا(eap);
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
    ساکن خالی
gfp_setname(char_u *fname, خالی *cookie)
{
    char_u	*gfp_بفر = cookie;

    اگر (STRLEN(fname) >= MAXPATHL)
	*gfp_بفر = NUL;
    ورنہ
	STRCPY(gfp_بفر, fname);
}

/*
 * Find the path of bitmap "name" with extension "ext" in 'runtimepath'.
 * Return FAIL for failure and OK if buffer[MAXPATHL] contains the result.
 */
    صحیح
gui_ڈھونڈو_bitmap(char_u *name, char_u *بفر, حرف *ext)
{
    اگر (STRLEN(name) > MAXPATHL - 14)
	واپس FAIL;
    vim_snprintf((حرف *)بفر, MAXPATHL, "bitmaps/%s.%s", name, ext);
    اگر (do_اندر_runtimepath(بفر, 0, gfp_setname, بفر) == FAIL
							    || *بفر == NUL)
	واپس FAIL;
    واپس OK;
}

# اگر !defined(FEAT_GUI_GTK)
/*
 * Given the name of the "icon=" argument, try finding the bitmap file for the
 * icon.  If it is an absolute path name, use it as it is.  Otherwise append
 * "ext" and search for it in 'runtimepath'.
 * The result is put in "buffer[MAXPATHL]".  If something fails "buffer"
 * contains "name".
 */
    خالی
gui_ڈھونڈو_iconfile(char_u *name, char_u *بفر, حرف *ext)
{
    char_u	buf[MAXPATHL + 1];

    expand_env(name, بفر, MAXPATHL);
    اگر (!mch_is_بھرا_Name(بفر) && gui_ڈھونڈو_bitmap(بفر, buf, ext) == OK)
	STRCPY(بفر, buf);
}
# endif
#اگر_ختم

#اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)|| defined(FEAT_GUI_HAIKU)
    خالی
display_errors(خالی)
{
    char_u	*p;

    اگر (isatty(2))
    {
	fflush(stderr);
	واپس;
    }

    اگر (error_ga.ga_ڈیٹا == تہی)
	واپس;

    // avoid putting up a message box with blanks only
    برائے (p = (char_u *)error_ga.ga_ڈیٹا; *p != NUL; ++p)
	اگر (!محفوظ_isspace(*p))
	{
	    // Truncate a very long message, it will go off-screen.
	    اگر (STRLEN(p) > 2000)
		STRCPY(p + 2000 - 14, "...(truncated)");
	    (خالی)do_dialog(VIM_ERROR, (char_u *)_("Error"),
		    p, (char_u *)_("&Ok"), 1, تہی, FALSE);
	    توڑو;
	}
    ga_clear(&error_ga);
}
#اگر_ختم

#اگر defined(NO_CONSOLE_داخلہ)
/*
 * Return TRUE if still starting up and there is no place to enter text.
 * For GTK and X11 we check if stderr is not a tty, which means we were
 * (probably) started from the desktop.  Also check stdin, "vim >& file" does
 * allow typing on stdin.
 */
    صحیح
no_console_داخلہ(خالی)
{
    واپس ((!gui.اندر_use || gui.starting)
# ifndef NO_CONSOLE
	    && !isatty(0) && !isatty(2)
# endif
	    );
}
#اگر_ختم

#اگر defined(ڈھونڈو_REPLACE_DIALOG) \
	|| defined(NEED_GUI_UPDATE_SCREEN)
/*
 * Update the current window and the screen.
 */
    خالی
gui_update_screen(خالی)
{
# ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_سطر = 0;
    linenr_T	conceal_new_cursor_سطر = 0;
    صحیح		conceal_update_سطریں = FALSE;
# endif

    update_topline();
    validate_cursor();

    // Trigger CursorMoved if the cursor moved.
    اگر (!اختتام_op && (has_cursormoved()
# ifdef FEAT_PROP_POPUP
		|| popup_visible
# endif
# ifdef FEAT_CONCEAL
		|| curwin->w_p_cole > 0
# endif
		) && !EQUAL_POS(آخری_cursormoved, curwin->w_cursor))
    {
	اگر (has_cursormoved())
	    apply_autocmds(EVENT_CURSORMOVED, تہی, تہی, FALSE, curbuf);
# ifdef FEAT_PROP_POPUP
	اگر (popup_visible)
	    popup_check_cursor_pos();
# endif
# ifdef FEAT_CONCEAL
	اگر (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_سطر = آخری_cursormoved.lnum;
	    conceal_new_cursor_سطر = curwin->w_cursor.lnum;
	    conceal_update_سطریں = TRUE;
	}
# endif
	آخری_cursormoved = curwin->w_cursor;
    }

    اگر (!اختتام_op)
	may_trigger_win_scrolled_resized();

# ifdef FEAT_CONCEAL
    اگر (conceal_update_سطریں
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
    برآمد_flush_cursor(TRUE, FALSE);
}
#اگر_ختم

#اگر defined(ڈھونڈو_REPLACE_DIALOG)
/*
 * Get the text to use in a find/replace dialog.  Uses the last search pattern
 * if the argument is empty.
 * Returns an allocated string.
 */
    char_u *
get_ڈھونڈو_dialog_متن(
    char_u	*arg,
    صحیح		*wwordp,	// return: TRUE if \< \> found
    صحیح		*mcasep)	// return: TRUE if \C found
{
    char_u	*متن;

    اگر (*arg == NUL)
	متن = آخری_تلاش_pat();
    ورنہ
	متن = arg;
    اگر (متن != تہی)
    {
	صحیح len = (صحیح)STRLEN(متن);

	متن = vim_strnsave(متن, len);
	اگر (متن != تہی)
	{
	    صحیح i;

	    // Remove "\V"
	    اگر (len >= 2 && STRNCMP(متن, "\\V", 2) == 0)
	    {
		mch_memmove(متن, متن + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\c" and "\C" and remove.
	    اگر (len >= 2 && *متن == '\\' && (متن[1] == 'c' || متن[1] == 'C'))
	    {
		*mcasep = (متن[1] == 'C');
		mch_memmove(متن, متن + 2, (size_t)(len - 1));
		len -= 2;
	    }

	    // Recognize "\<text\>" and remove.
	    اگر (len >= 4
		    && STRNCMP(متن, "\\<", 2) == 0
		    && STRNCMP(متن + len - 2, "\\>", 2) == 0)
	    {
		*wwordp = TRUE;
		mch_memmove(متن, متن + 2, (size_t)(len - 4));
		متن[len - 4] = NUL;
	    }

	    // Recognize "\/" or "\?" and remove.
	    برائے (i = 0; i + 1 < len; ++i)
		اگر (متن[i] == '\\' && (متن[i + 1] == '/'
						       || متن[i + 1] == '?'))
		{
		    mch_memmove(متن + i, متن + i + 1, (size_t)(len - i));
		    --len;
		}
	}
    }
    واپس متن;
}

/*
 * Handle the press of a button in the find-replace dialog.
 * Return TRUE when something was added to the input buffer.
 */
    صحیح
gui_do_findrepl(
    صحیح		flags,		// one of FRD_REPLACE, FRD_FINDNEXT, etc.
    char_u	*ڈھونڈو_متن,
    char_u	*repl_متن,
    صحیح		نیچے_کو)		// Search downwards.
{
    garray_T	ga;
    صحیح		i;
    صحیح		type = (flags & FRD_TYPE_نقاب);
    char_u	*p;
    regmatch_T	regmatch;
    صحیح		save_did_emsg = did_emsg;
    ساکن صحیح  busy = FALSE;

    // When the screen is being updated we should not change buffers and
    // windows structures, it may cause freed memory to be used.  Also don't
    // do this recursively (pressing "Find" quickly several times).
    اگر (updating_screen || busy)
	واپس FALSE;

    // refuse replace when text cannot be changed
    اگر ((type == FRD_REPLACE || type == FRD_REPLACEALL) && متن_locked())
	واپس FALSE;

    busy = TRUE;

    ga_init2(&ga, 1, 100);
    اگر (type == FRD_REPLACEALL)
	GA_CONCAT_LITERAL(&ga, "%s/");

    GA_CONCAT_LITERAL(&ga, "\\V");
    اگر (flags & FRD_مطابقت_CASE)
	GA_CONCAT_LITERAL(&ga, "\\C");
    ورنہ
	GA_CONCAT_LITERAL(&ga, "\\c");
    اگر (flags & FRD_پورا_لفظ)
	GA_CONCAT_LITERAL(&ga, "\\<");
    // escape slash and backslash
    p = vim_strsave_escaped(ڈھونڈو_متن, (char_u *)"/\\");
    اگر (p != تہی)
	ga_concat(&ga, p);
    vim_free(p);
    اگر (flags & FRD_پورا_لفظ)
	GA_CONCAT_LITERAL(&ga, "\\>");

    اگر (type == FRD_REPLACEALL)
    {
	GA_CONCAT_LITERAL(&ga, "/");
	// Escape slash and backslash.
	// Also escape tilde and ampersand if 'magic' is set.
	p = vim_strsave_escaped(repl_متن,
				p_magic ? (char_u *)"/\\~&" : (char_u *)"/\\");
	اگر (p != تہی)
	    ga_concat(&ga, p);
	vim_free(p);
	GA_CONCAT_LITERAL(&ga, "/g");
    }
    ga_append(&ga, NUL);

    اگر (type == FRD_REPLACE)
    {
	// Do the replacement when the text at the cursor matches.  Thus no
	// replacement is done if the cursor was moved!
	regmatch.regprog = vim_regcomp(ga.ga_ڈیٹا, RE_MAGIC + RE_STRING);
	regmatch.rm_ic = 0;
	اگر (regmatch.regprog != تہی)
	{
	    p = ml_get_cursor();
	    اگر (vim_regexec_nl(&regmatch, p, (colnr_T)0)
						   && regmatch.startp[0] == p)
	    {
		// Clear the command line to remove any old "No match"
		// error.
		پیغام_انتہا_prompt();

		اگر (u_save_cursor() == OK)
		{
		    // A button was pressed thus undo should be synced.
		    u_sync(FALSE);

		    del_بائٹیں((طویل)(regmatch.endp[0] - regmatch.startp[0]),
								FALSE, FALSE);
		    ins_str(repl_متن, STRLEN(repl_متن));
		}
	    }
	    ورنہ
		پیغام(_("No match at cursor, finding next"));
	    vim_regfree(regmatch.regprog);
	}
    }

    اگر (type == FRD_REPLACEALL)
    {
	// A button was pressed, thus undo should be synced.
	u_sync(FALSE);
	do_cmdline_cmd(ga.ga_ڈیٹا);
    }
    ورنہ
    {
	صحیح searchflags = تلاش_پیغام + تلاش_MARK;

	// Search for the next match.
	// Don't skip text under cursor for single replace.
	اگر (type == FRD_REPLACE)
	    searchflags += تلاش_آغاز;
	i = پیغام_scroll;
	اگر (نیچے_کو)
	{
	    (خالی)do_تلاش(تہی, '/', '/', ga.ga_ڈیٹا, STRLEN(ga.ga_ڈیٹا), 1L, searchflags, تہی);
	}
	ورنہ
	{
	    // We need to escape '?' if and only if we are searching in the up
	    // direction
	    p = vim_strsave_escaped(ga.ga_ڈیٹا, (char_u *)"?");
	    اگر (p != تہی)
		(خالی)do_تلاش(تہی, '?', '?', p, STRLEN(p), 1L, searchflags, تہی);
	    vim_free(p);
	}

	پیغام_scroll = i;	    // don't let an error message set msg_scroll
    }

    // Don't want to pass did_emsg to other code, it may cause disabling
    // syntax HL if we were busy redrawing.
    did_emsg = save_did_emsg;

    اگر (State & (منوال_معمول | منوال_داخل_کرو))
    {
	gui_update_screen();		// update the screen
	پیغام_didout = 0;			// overwrite any message
	need_wait_return = FALSE;	// don't wait for return
    }

    vim_free(ga.ga_ڈیٹا);
    busy = FALSE;
    واپس (ga.ga_len > 0);
}

#اگر_ختم

#اگر defined(HAVE_DROP_FILE)
/*
 * Jump to the window at specified point (x, y).
 */
    ساکن خالی
gui_wingoto_xy(صحیح x, صحیح y)
{
    صحیح		صف = Y_2_صف(y);
    صحیح		ستون = X_2_ستون(x);
    win_T	*wp;

    اگر (صف < 0 || ستون < 0)
	واپس;

    wp = mouse_ڈھونڈو_win(&صف, &ستون, FAIL_POPUP);
    اگر (wp != تہی && wp != curwin)
	win_goto(wp);
}

/*
 * Function passed to handle_drop() for the actions to be done after the
 * argument list has been updated.
 */
    ساکن خالی
drop_callback(خالی *cookie)
{
    char_u	*p = cookie;
    صحیح		do_shorten = FALSE;

    // If Shift held down, change to first file's directory.  If the first
    // item is a directory, change to that directory (and let the explorer
    // plugin show the contents).
    اگر (p != تہی)
    {
	اگر (mch_isdir(p))
	{
	    اگر (mch_chdir((حرف *)p) == 0)
		do_shorten = TRUE;
	}
	ورنہ اگر (vim_chdirfile(p, "drop") == OK)
	    do_shorten = TRUE;
	vim_free(p);
	اگر (do_shorten)
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
    برآمد_flush_cursor(FALSE, FALSE);
}

/*
 * Process file drop.  Mouse cursor position, key modifiers, name of files
 * and count of files are given.  Argument "fnames[count]" has full pathnames
 * of dropped files, they will be freed in this function, and caller can't use
 * fnames after call this function.
 */
    خالی
gui_handle_drop(
    صحیح		x غیر_مستعمل,
    صحیح		y غیر_مستعمل,
    int_u	modifiers,
    char_u	**fnames,
    صحیح		گنتی)
{
    صحیح		i;
    char_u	*p;
    ساکن صحیح	entered = FALSE;

    /*
     * This function is called by event handlers.  Just in case we get a
     * second event before the first one is handled, ignore the second one.
     * Not sure if this can ever happen, just in case.
     */
    اگر (entered)
	واپس;
    entered = TRUE;

    /*
     * When the cursor is at the command line, add the file names to the
     * command line, don't edit the files.
     */
    اگر (State & منوال_CMDLINE)
    {
	shorten_filenames(fnames, گنتی);
	برائے (i = 0; i < گنتی; ++i)
	{
	    اگر (fnames[i] != تہی)
	    {
		اگر (i > 0)
		    جوڑو_مقصد_داخلہ_buf((char_u*)" ", 1);

		// We don't know what command is used thus we can't be sure
		// about which characters need to be escaped.  Only escape the
		// most common ones.
# ifdef BACKSLASH_اندر_FILENAME
		p = vim_strsave_escaped(fnames[i], (char_u *)" \t\"|");
# ورنہ
		p = vim_strsave_escaped(fnames[i], (char_u *)"\\ \t\"|");
# endif
		اگر (p != تہی)
		    جوڑو_مقصد_داخلہ_buf_csi(p, (صحیح)STRLEN(p));
		vim_free(p);
		vim_free(fnames[i]);
	    }
	}
	vim_free(fnames);
    }
    ورنہ
    {
	// Go to the window under mouse cursor, then shorten given "fnames" by
	// current window, because a window can have local current dir.
	gui_wingoto_xy(x, y);
	shorten_filenames(fnames, گنتی);

	// If Shift held down, remember the first item.
	اگر ((modifiers & MOUSE_سرکاؤ) != 0)
	    p = vim_strsave(fnames[0]);
	ورنہ
	    p = تہی;

	// Handle the drop, :edit or :split to get to the file.  This also
	// frees fnames[].  Skip this if there is only one item, it's a
	// directory and Shift is held down.
	اگر (گنتی == 1 && (modifiers & MOUSE_سرکاؤ) != 0
						     && mch_isdir(fnames[0]))
	{
	    vim_free(fnames[0]);
	    vim_free(fnames);
	    vim_free(p);
	}
	ورنہ
	    handle_drop(گنتی, fnames, (modifiers & MOUSE_CTRL) != 0,
						     drop_callback, (خالی *)p);
    }

    entered = FALSE;
}
#اگر_ختم

/*
 * Check if "key" is to interrupt us.  Handles a key that has not had modifiers
 * applied yet.
 * Return the key with modifiers applied if so, NUL if not.
 */
    صحیح
check_for_مداخلت(صحیح کلید, صحیح modifiers_arg)
{
    صحیح modifiers = modifiers_arg;
    صحیح c = ملاؤ_modify_Other_کلیدیں(کلید, &modifiers);

    اگر ((c == Ctrl_C && ctrl_c_interrupts)
#اگر_تعریف_شدہ UNIX
	    || (intr_char != Ctrl_C && c == intr_char)
#اگر_ختم
	    )
    {
	got_int = TRUE;
	واپس c;
    }
    واپس NUL;
}

/*
 * If the "--gui-log-file fname" argument is given write the dialog title and
 * message to a file and return TRUE.  Otherwise return FALSE.
 * When there is any problem opening the file or writing to the file this is
 * ignored, showing the dialog might get the test to get stuck.
 */
    صحیح
gui_dialog_log(char_u *title, char_u *پیغام_2)
{
    char_u  *fname = get_gui_dialog_file();
    FILE    *fd;

    اگر (fname == تہی)
	واپس FALSE;

    fd = mch_fopen((حرف *)fname, "a");
    اگر (fd != تہی)
    {
	fprintf(fd, "%s: %s\n", title, پیغام_2);
	fclose(fd);
    }
    واپس TRUE;
}
